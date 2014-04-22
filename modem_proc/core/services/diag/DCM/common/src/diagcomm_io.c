/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diag I/O Wrapper APIs for Interprocessor Communication and I/O

General Description

Copyright (c) 2011-2016, 2018-2019 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.10/services/diag/DCM/common/src/diagcomm_io.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/14/19   kdey    Porting of Qsockets
02/21/18   kdey    Fixed KW Errors
01/05/16   ph      Flow control handled for buffering mode switch.
12/23/15   ph      Extended messages are HDLC encoded properly now on standlone modem.
12/01/15   ph      Flow control handled properly over control channel.
11/23/15   ph      WP logging stopped issue fixes.
09/15/15   gn      Removed check for diag_prev_tx_pending in diagcomm_io_enable_flow()
08/26/15   ph      Buffering mode revamp to make it functional.
08/11/15   ph      Fixed Compiler Warnings.
06/11/15   sa      Fixed Compiler Warnings.
05/15/15   sr      Changed the SMDL FIFO size to 8k
04/08/15   ps      Diag over sockets
01/21/15   sr      Added changes to remove DSM items
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
08/31/13   is      Support for peripheral buffering mode
08/30/13   ph      Resetting the signal DIAG_BLOCK_SIG before we call sio_flush_tx().
08/29/13   sr      Fixed the missing break line issue in switch statements
01/03/13   sg      Command Response Channel separation
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
11/15/12   is      Support for preset masks
10/11/12   rh      Support t-put improvements with SMDL
10/11/12   rh      Added api diagcomm_io_get_tx_wmq_cnt()
05/03/12   sa      Mainlining DIAG_CENTRAL_ROUTING
03/04/12   sg      Set DTR using sio_control_ioctl for control channel
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/09/11   wjg     Modified to initialize new data_pending member
04/28/11   is      Resolve compilation issues
04/27/11   is      Resolve compiler warnings
03/25/11   is      Fix compilation errors for modem-only configuration
03/04/11   is      Support for Diag over SMD-Lite

===========================================================================*/

#include "diagcomm_io.h"
#include "diagcomm_ctrl.h"        /* For NUM_SMD_PORT_TYPES */
#include "diagcomm_smd.h"         /* For diagcomm_smd_open_init */
#include "diag_v.h"               /* For diag_block() */
#include "diagi_v.h"              /* For DIAG_UPDATE_MASK_NONE, etc */
#include "err.h"                  /* For ERR_FATAL() */
#include "assert.h"               /* For ASSERT() */
#include "diagtarget.h"           /* For diag_tcb */
#include "ULog_Diag.h"

#if defined(DIAG_MP_MASTER)
#include "msgi.h"                 /* For diagcomm_ctrl_send_msg_mask() */
#include "eventi.h"               /* For diagcomm_ctrl_send_event_mask() */
#include "diaglogi.h"             /* For diagcomm_ctrl_send_log_mask() */
#endif

#ifdef DIAG_SIO_SUPPORT
  #include "diagcomm_io_sio.h"    /* For diagcomm_smd_sio_set_port */
#endif

#include "diagcomm_io_socket.h"
#include "diagcomm_io.h"
#include "time_svc.h"


extern osal_timer_t diag_sio_tout_timer;
extern osal_mutex_arg_t diag_flow_control_mutex;

/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];

uint32 diag_send_buf_threshold = DIAG_MAX_SMDL_DATA_BLOCK_SIZE; /*Buffer up data upto this threshold value */
uint32 diag_max_send_buf_len = DIAG_MAX_TX_PKT_SIZ; /*Maximum send size*/
extern diag_tx_mode_type diag_tx_mode[DIAG_MAX_STREAM_ID];

extern osal_timer_t  diag_nomem_tout_timer;   

#define DIAG_TX_RETRY_NOTIFY_TIMER_LEN    50    // Wait delay in Milliseconds before next re-try of sending data on sockect in QNOMEM case

#if defined(DIAG_SOCKET_SUPPORT)
static uint32 total_retry_count;    // Re-try count since boot-up
static uint32 retry_count;          // count variable for total number of retries since the last time we encountered heap exhaustion
#endif



/*===========================================================================

FUNCTION DIAGCOMM_IO_OPEN_INIT

DESCRIPTION

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  None

===========================================================================*/
void diagcomm_io_open_init( diagcomm_io_conn_type * conn )
{
  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      if( conn->channel_type == DIAGCOMM_IO_DATA )
      {
        if( DIAGCOMM_PORT_SMD != conn->port_type )
        {
          diagcomm_sio_open_init( conn );
        }
      }
      else if( conn->channel_type == DIAGCOMM_IO_CTRL )
      {
        diagcomm_ctrl_open_init( conn );
      }
      break;
    }
#endif

    default:
      break;
  }

} /* diagcomm_io_open_init */


/*===========================================================================

FUNCTION DIAGCOMM_IO_OPEN

DESCRIPTION
  Opens a connection between two processors or I/O

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  True - If connection opened successfully
  False - If connection was not opened

===========================================================================*/
boolean diagcomm_io_open( diagcomm_io_conn_type * conn )
{
  conn->connected = FALSE;

  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    sio_ioctl_param_type param;

    case DIAGCOMM_IO_TYPE_SIO:
    {
      if( (DIAGCOMM_IO_DATA == conn->channel_type) && (conn->open_params.sio_params.params.port_id != SIO_PORT_NULL) )
      {
        conn->handle.sio_handle = sio_open( &(conn->open_params.sio_params.params) );

        if( conn->handle.sio_handle != SIO_NO_STREAM_ID )
        {
          conn->connected = TRUE;

          /* Set flow control to allow flow */
          sio_ioctl( conn->handle.sio_handle, SIO_IOCTL_INBOUND_FLOW_ENABLE, &param);

          /* Set CD line */
          sio_ioctl( conn->handle.sio_handle, SIO_IOCTL_CD_ASSERT, &param);

          /* Set DSR line */
          sio_ioctl( conn->handle.sio_handle, SIO_IOCTL_DSR_ASSERT, &param);

          /* Clear RI line */
          sio_ioctl( conn->handle.sio_handle, SIO_IOCTL_RI_DEASSERT, &param);
        }
      }
      else if( (DIAGCOMM_IO_CTRL == conn->channel_type) && (conn->open_params.sio_params.params.port_id != SIO_PORT_NULL) )
      {
        conn->handle.sio_handle = sio_control_open( &(conn->open_params.sio_params.params) );

        if( conn->handle.sio_handle != SIO_NO_STREAM_ID )
        {
          conn->connected = TRUE;

          /* Set flow control to allow flow */
          //sio_control_ioctl( conn->handle.sio_handle, SIO_IOCTL_INBOUND_FLOW_ENABLE, &param);

          /* Set CD line */
          //sio_control_ioctl( conn->handle.sio_handle, SIO_IOCTL_CD_ASSERT, &param);

          /* Set DSR line */
          //sio_control_ioctl( conn->handle.sio_handle, SIO_IOCTL_DSR_ASSERT, &param);

          /* Clear RI line */
          //sio_control_ioctl( conn->handle.sio_handle, SIO_IOCTL_RI_DEASSERT, &param);
        }
      }
      break;
    }
#endif

#if defined(DIAG_SOCKET_SUPPORT)
    case DIAGCOMM_IO_TYPE_IPCR_UDP:
    {
      int return_val= 0;
      return_val = diagcomm_io_socket_open(conn);
      ASSERT(DIAG_EOK == return_val);
    }
#endif

    default:
      break;
  }

  return conn->connected;

} /* diagcomm_io_open */


/*===========================================================================

FUNCTION DIAGCOMM_IO_CLOSE

DESCRIPTION
  Closes a connection between two processors or I/O

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  None

===========================================================================*/
void diagcomm_io_close( diagcomm_io_conn_type * conn )
{
  /* Success or not, consider connection closed */
  conn->connected = FALSE;

  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      /* void sio_close( sio_stream_id_type stream_id,   //Stream ID
                         void *close_func_ptr)(void) );  //Function to call when transmission is complete
       */
      if( DIAGCOMM_IO_DATA == conn->channel_type )
        sio_close( conn->handle.sio_handle, conn->open_params.sio_params.close_func_ptr );
      else if( DIAGCOMM_IO_CTRL == conn->channel_type )
        sio_control_close( conn->handle.sio_handle, conn->open_params.sio_params.close_func_ptr );

      conn->handle.sio_handle = SIO_NO_STREAM_ID;

      /* Block on DIAG_BLOCK_SIG signal, which should be set via close function callbacks
         by calling diag_release(). */
      diag_block();

      #if defined (DIAG_RUNTIME_DEVMAP) || defined (FEATURE_WINCE)
        if( conn->open_params.sio_params.close_func_ptr != NULL )
        {
          (conn->open_params.sio_params.close_func_ptr) ();
        }
      #endif /* DIAG_RUNTIME_DEVMAP || FEATURE_WINCE */
      break;
    }
#endif

#if defined(DIAG_SOCKET_SUPPORT)
    case DIAGCOMM_IO_TYPE_IPCR_UDP:
    {
      diagcomm_io_socket_close(conn);
    }
#endif

    default:
      break;
  }

} /* diagcomm_io_close */

/*===========================================================================

FUNCTION DIAGCOMM_IO_TRANSMIT

DESCRIPTION
  Transmits data between two processors or I/O

PARAMETERS
  conn - Diag I/O connection handle
  tx_param - Tx data containing buffer to transmit

RETURN VALUE
  DIAG_EOK : Transmit successful
  DIAG_EFAILED: Transmit failed over the connection channel
  DIAG_EPARAM: Input paramater validation failed
  DIAG_ERETRY: Flow is disabled, retry later

===========================================================================*/
int diagcomm_io_transmit( diagcomm_io_conn_type * conn, diagcomm_io_tx_params_type * tx_param )
{
  int ret = DIAG_EOK;
  // Featurized num_write as it is only used under flag DIAG_SOCKET_SUPPORT, without featurization code compilation is failing for standalone.
  #if defined(DIAG_SOCKET_SUPPORT)
    int num_write = 0;
  int return_val;
  #endif /* DIAG_SOCKET_SUPPORT */

  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      /* void sio_transmit( sio_stream_id_type stream_id,    // Stream ID
                            dsm_item_type *tx_ptr );         // Packet which needs to be transmitted

         This function will transmit a packet or if transmitter is busy, schedule
         the transmission of a packet.
       */

          if((DIAGCOMM_IO_DATA == conn->channel_type) || (conn->channel_type == DIAGCOMM_IO_CMD))
          {
            /*clear the index in send buffer*/
            diagbuf_clear_index(conn, tx_param);

            sio_transmit( conn->handle.sio_handle, (dsm_item_type *)tx_param->dsm_ptr );

          }
          else if( DIAGCOMM_IO_CTRL == conn->channel_type )
          {
            sio_control_transmit( conn->handle.sio_handle, (dsm_item_type *)tx_param->dsm_ptr );
          }

          ret = DIAG_EOK;

      break;
    }
#endif

#if defined(DIAG_SOCKET_SUPPORT)
        case DIAGCOMM_IO_TYPE_IPCR_UDP:
          if(FALSE == conn->allow_flow)
          {
            ULogFront_RealTimePrintf(diag_data_ulog_handle, 4, "diagcomm_io_transmit: allow_flow = %d, channel_type = %d, io_type = %d, port_num = %d",\
                conn->allow_flow, conn->channel_type, conn->io_type, conn->port_num);
             return DIAG_ERETRY;
          }
          if(0 == tx_param->params.len)
          {
            ULogFront_RealTimePrintf(diag_data_ulog_handle, 4, "diagcomm_io_transmit: Error - data length = %d, channel_type = %d, io_type = %d, port_num = %d",\
            tx_param->params.len, conn->channel_type, conn->io_type, conn->port_num);
             return DIAG_EPARAM;
          }

          osal_lock_mutex(&diag_flow_control_mutex);
          num_write = diagcomm_io_socket_send(conn, tx_param->params.buffer, tx_param->params.len);
          //Check if the previous send succeeded. If not, disable the flow.
          if( num_write == tx_param->params.len )
           {
               /*If we are sending data enable flow*/
               conn->allow_flow = TRUE;
               conn->diag_prev_tx_pending = FALSE; /* Packet successfully sent */
               if(conn->channel_type == DIAGCOMM_IO_DATA)
                 ULogFront_RealTimePrintf(diag_data_ulog_handle, 4, "Sent %d bytes on channel = %d, io_type = %d, port_num = %d", num_write, conn->channel_type, conn->io_type, conn->port_num);
               
               diagbuf_clear_index(conn, tx_param);
               if (0 != retry_count)  { // printing logs for every successful re-try after QNOMEM has encountered
                  ULogFront_RealTimePrintf(diag_debug_ulog_handle, 1, "diagcomm_io_transmit: Success - Number of re-tries for sending data since the last time we've encountered QENOMEM are = %d",\
                                                         retry_count);
                  retry_count = 0;  // re-setting a counter after a successful re-try 
               }
               ret = DIAG_EOK;
           }
           else if(num_write == QEAGAIN)
           {
              conn->diag_prev_tx_pending = TRUE;  /* FIFO is full; come back later */
              conn->allow_flow = FALSE;
              diagcomm_incr_flow_ctrl_count(DIAGCOMM_PORT_SIO);
              ULogFront_RealTimePrintf(diag_data_ulog_handle, 5, "diagcomm_io_transmit: allow_flow = %d , diag_prev_tx_pending = %d, channel_type = %d, io_type = %d, port_num = %d, Retry - QEAGAIN",\
                  conn->allow_flow, conn->diag_prev_tx_pending, conn->channel_type, conn->io_type, conn->port_num);
              ret = DIAG_EFAILED;
           }
           else if (num_write == QENOMEM)
           {     		  
              conn->diag_prev_tx_pending = TRUE;  /* Heap memory Exhausted; come back later */
              conn->allow_flow = FALSE;	
              retry_count++;
              total_retry_count++;
              if (retry_count == 1)
                ULogFront_RealTimePrintf(diag_debug_ulog_handle, 0, "diagcomm_io_transmit: Received QNOMEM - Initiating Re-try Mechanism... ");
  			  
               if (0 == (total_retry_count % 15)) {  // printing logs after every 15 re-tries for total number of retries since bootup
                 ULogFront_RealTimePrintf(diag_debug_ulog_handle, 1, "diagcomm_io_transmit: Retrying - Total number of re-tries for sending data when encountered QENOMEM since bootup are = %d",\
                   total_retry_count);
               }
               if (osal_get_remaining_time(&diag_nomem_tout_timer) == 0)
               {
                 return_val =  osal_set_timer(&diag_nomem_tout_timer, DIAG_TX_RETRY_NOTIFY_TIMER_LEN);
                 ASSERT(OSAL_SUCCESS == return_val);
               }
               ret = DIAG_EHEAP;
          }
           else
           {
              conn->last_error = num_write;
              ERR_FATAL ("Error seen when sending data: %d",num_write,0,0);
           }
           osal_unlock_mutex(&diag_flow_control_mutex);
           break;
#endif

    default:
      break;
  }


  return ret;
} /* diagcomm_io_transmit */


/*===========================================================================

FUNCTION DIAGCOMM_IO_IOCTL

DESCRIPTION
  Sets SIO-specific ioctl parameters

PARAMETERS
  conn - Diag I/O connection handle
  cmd - IOCTL command to set
  param - Parameter to be used for command

RETURN VALUE
  None

===========================================================================*/
void diagcomm_io_ioctl( diagcomm_io_conn_type * conn, uint32 cmd, void * param )
{
  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      /* void sio_ioctl( sio_stream_id_type    stream_id, // Stream ID
                         sio_ioctl_cmd_type    cmd,       // IOCTL command
                         sio_ioctl_param_type *param );   // Parameter to be used for command
       */
      if ( conn->channel_type == DIAGCOMM_IO_CTRL )
      {
        sio_control_ioctl( conn->handle.sio_handle,
                          (sio_ioctl_cmd_type)cmd,
                          (sio_ioctl_param_type *)param );
      }
      else if ( conn->channel_type == DIAGCOMM_IO_DATA )
      {
        if( conn->handle.sio_handle != SIO_NO_STREAM_ID )
        {
          sio_ioctl( conn->handle.sio_handle,
                    (sio_ioctl_cmd_type)cmd,
                    (sio_ioctl_param_type *)param );
        }
      }
      break;
    }
#endif

    default:
      break;
  }

} /* diagcomm_io_ioctl */


/*===========================================================================

FUNCTION DIAGCOMM_IO_READ

DESCRIPTION

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  None

===========================================================================*/
void diagcomm_io_read( diagcomm_io_conn_type * conn )
{
  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      break;
    }
#endif
    default:
      break;
  }

} /* diagcomm_io_read */


/*===========================================================================

FUNCTION DIAGCOMM_IO_SET_PORT

DESCRIPTION
  Sets Sio-specific port number

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  None

===========================================================================*/
void diagcomm_io_set_port( diagcomm_io_conn_type * conn )
{
  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      if( (DIAGCOMM_IO_DATA == conn->channel_type) && (DIAGCOMM_PORT_SMD == conn->port_type) )
      {
        diagcomm_smd_sio_set_port( conn );
      }
      else if( (DIAGCOMM_IO_DATA == conn->channel_type) && (DIAGCOMM_PORT_SIO == conn->port_type) )
      {
        diagcomm_sio_set_port( conn );
      }
      else if( DIAGCOMM_IO_CTRL == conn->channel_type )
      {
        diagcomm_ctrl_sio_set_port( conn );
      }
      break;
    }
#endif

    default:
      break;
  }

} /* diagcomm_io_set_port */


/*===========================================================================

FUNCTION DIAGCOMM_IO_FLUSH_TX

DESCRIPTION

PARAMETERS
  conn - Diag I/O connection handle
  flush_cb - callback to be registered with SIO
  timeoutVal - duration for flush timeout.

RETURN VALUE
  None

===========================================================================*/
void diagcomm_io_flush_tx( diagcomm_io_conn_type * conn, void (*flush_cb) (void),unsigned int timeoutVal)
{
  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      if( (DIAGCOMM_PORT_SIO == conn->port_type) && (DIAGCOMM_PORT_1 == conn->port_num) )
      {
        osal_sigs_t return_sigs;
        int return_val_tout = 0, return_val = 0;
        /* Clearing the signal to avoid any corner case when
        SIO calls the flush callback after the timeout expires and the
        DIAG_BLOCK_SIG would be already set by then.*/
        return_val_tout = osal_reset_sigs(diag_task_tcb, DIAG_SIO_TOUT_TIMER_SIG, &return_sigs);
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_BLOCK_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        ASSERT(OSAL_SUCCESS == return_val_tout);
        /* Tell SIO to flush the TX FIFO */
        sio_flush_tx( conn->handle.sio_handle, flush_cb );

        if( timeoutVal > 0 )
        {
          osal_set_timer(&diag_sio_tout_timer, timeoutVal);
        }
        diag_block ();
      }
      break;
    }
#endif

    default:
      break;
  }

} /* diagcomm_io_flush_tx */


/*===========================================================================

FUNCTION DIAGCOMM_IO_GET_RX_WMQ_CNT

DESCRIPTION
  This function returns the channel's Rx watermark queue count.

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  The number of DSMs queued in the channel's Rx watermark queue.

===========================================================================*/
uint32 diagcomm_io_get_rx_wmq_cnt( diagcomm_io_conn_type * conn )
{
  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      if( conn->open_params.sio_params.params.rx_queue )
      {
        if( conn->open_params.sio_params.params.rx_queue->q_ptr )
          return q_cnt( conn->open_params.sio_params.params.rx_queue->q_ptr );
      }
      break;
    }
#endif
    default:
      break;
  }

  /* fall through */
  return 0;

} /* diagcomm_io_get_rx_wmq_cnt */

/*===========================================================================

FUNCTION DIAGCOMM_IO_GET_TX_WMQ_CNT

DESCRIPTION
  This function returns the channel's Tx watermark queue count.

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  The number of DSMs queued in the channel's Tx watermark queue.

===========================================================================*/
uint32 diagcomm_io_get_tx_wmq_cnt( diagcomm_io_conn_type * conn )
{
  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      if( conn->open_params.sio_params.params.tx_queue )
      {
        if( conn->open_params.sio_params.params.tx_queue->q_ptr )
          return q_cnt( conn->open_params.sio_params.params.tx_queue->q_ptr );
      }
      break;
    }
#endif
    default:
      break;
  }

  /* fall through */
  return 0;

} /* diagcomm_io_get_tx_wmq_cnt */



/*===========================================================================

FUNCTION DIAGCOMM_IO_DEQUEUE_RX_WMQ

DESCRIPTION
  Dequeues a DSM from the Rx watermark queue

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  Pointer to DSM dequeued

===========================================================================*/
dsm_item_type * diagcomm_io_dequeue_rx_wmq( diagcomm_io_conn_type * conn )
{
  uint32 q_count = 0;

  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      if( conn->open_params.sio_params.params.rx_queue )
      {
        if( conn->open_params.sio_params.params.rx_queue->q_ptr )
          q_count = q_cnt( conn->open_params.sio_params.params.rx_queue->q_ptr );
      }
      if( q_count > 0 )
      {
        return dsm_dequeue( conn->open_params.sio_params.params.rx_queue );
      }
      break;
    }
#endif
    default:
      break;
  }

  /* fall through */
  return NULL;

} /* diagcomm_io_dequeue_rx_wmq */

/*===========================================================================

FUNCTION DIAGCOMM_IO_DEQUEUE_TX_WMQ

DESCRIPTION
  Dequeues a DSM from the Tx watermark queue

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  Pointer to DSM dequeued

===========================================================================*/
dsm_item_type * diagcomm_io_dequeue_tx_wmq( diagcomm_io_conn_type * conn )
{
  uint32 q_count = 0;

  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      if( conn->open_params.sio_params.params.tx_queue )
      {
        if( conn->open_params.sio_params.params.tx_queue->q_ptr )
          q_count = q_cnt( conn->open_params.sio_params.params.tx_queue->q_ptr );
      }
      if( q_count > 0 )
      {
        return dsm_dequeue( conn->open_params.sio_params.params.tx_queue );
      }
      break;
    }
#endif

    default:
      break;
  }

  /* fall through */
  return NULL;

} /* diagcomm_io_dequeue_tx_wmq */


/*===========================================================================

FUNCTION DIAGCOMM_IO_ENABLE_FLOW

DESCRIPTION
  Enables flow control for channel

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  None

===========================================================================*/
void diagcomm_io_enable_flow( diagcomm_io_conn_type * conn )
{
  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      sio_ioctl_param_type param;
      diagcomm_io_ioctl( conn, SIO_IOCTL_INBOUND_FLOW_ENABLE, &param );
      break;
    }
#endif
#ifdef DIAG_SOCKET_SUPPORT
    case DIAGCOMM_IO_TYPE_IPCR_UDP:
    {
      osal_lock_mutex(&diag_flow_control_mutex);
      conn->diag_resend_data = TRUE;
      ULogFront_RealTimePrintf(diag_debug_ulog_handle, 3, "1: diagcomm_io_enable_flow() to check and set signal diag_prev_tx_pending %d diag_resend_data %d allow_flow %d",
                               diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].diag_prev_tx_pending, diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].diag_resend_data,
                               diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].allow_flow);


      if(conn->diag_prev_tx_pending)
      {
          if((DIAGCOMM_IO_DATA == conn->channel_type) && (DIAGCOMM_PORT_SIO == conn->port_type))
          {
            ULogFront_RealTimePrintf(diag_debug_ulog_handle, 3, "1: diagcomm_io_enable_flow() Signal set. diag_prev_tx_pending %d diag_resend_data %d allow_flow %d",
                                     diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].diag_prev_tx_pending, diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].diag_resend_data,
                                     diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].allow_flow);
            diag_set_internal_sigs(DIAG_INT_RESEND_DATA_SIG);
          }
          else if ((DIAGCOMM_IO_CMD == conn->channel_type) && (DIAGCOMM_PORT_SIO == conn->port_type))
          {
          /*We should not get flow controlled on command channel - how do we handle this case?*/
            diag_set_internal_sigs(DIAG_INT_RESEND_CMD_SIG);
          }
          else if ((DIAGCOMM_IO_CTRL == conn->channel_type) && (DIAGCOMM_PORT_SIO == conn->port_type))
          {
            diag_set_internal_sigs(DIAG_INT_RESEND_CTRL_SIG);
          }
      }
      osal_unlock_mutex(&diag_flow_control_mutex);
      break;
    }
#endif

    default:
      break;
  }
} /* diagcomm_io_enable_flow */


/*===========================================================================

FUNCTION DIAGCOMM_IO_DISABLE_FLOW

DESCRIPTION
  Disables flow control for channel

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  None

===========================================================================*/
void diagcomm_io_disable_flow( diagcomm_io_conn_type * conn )
{
  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      sio_ioctl_param_type param;
      diagcomm_io_ioctl( conn, SIO_IOCTL_INBOUND_FLOW_DISABLE, &param );
      break;
    }
#endif

    default:
      break;
  }
} /* diagcomm_io_disable_flow */


/*===========================================================================

FUNCTION DIAGCOMM_IO_DETECT_CONNECTION

DESCRIPTION

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  None

===========================================================================*/
void diag_io_detect_connection( diagcomm_io_conn_type * conn )
{
  /* Ensure connected first */
  if( !conn->connected )
    return;

  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      diag_dtr_enable( conn );             /* Registering the callback for DTR changes on SMD and SIO ports */
      diag_dtr_changed_cb( (void*)conn );  /* To ensure that the DTR state is correct */
      break;
    }
#endif

    default:
      break;
  }
} /* diag_io_detect_connection */


/*===========================================================================

FUNCTION DIAGCOMM_IO_STATUS

DESCRIPTION
  This function returns TRUE if the communications layer has an open port.

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  True - If the communications layer has an open port
  False - Otherwise

===========================================================================*/
boolean diagcomm_io_status( diagcomm_io_conn_type * conn )
{
  ASSERT( conn != NULL );

  if( (conn->connected == FALSE) || (conn->channel_connected == FALSE) )
    return FALSE;

  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
       if( conn->handle.sio_handle == SIO_NO_STREAM_ID )
       {
         conn->connected = FALSE;
         return FALSE;
       }
       else
       {
         return (conn->connected && conn->channel_connected);
       }
       break;
    }
#endif

#if defined(DIAG_SOCKET_SUPPORT)
    case DIAGCOMM_IO_TYPE_IPCR_UDP:
    {
       return (conn->connected && conn->channel_connected);
    }
#endif

    default:
      break;
  }

  return FALSE;

} /* diagcomm_io_status */


/*===========================================================================

FUNCTION DIAGCOMM_IO_IS_ONDEVICE_MODE

DESCRIPTION
  This function returns TRUE if the communications layer is in on-device
  logging mode. Else returns False.

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  True - If the communications layer is in on-device logging mode
  False - Otherwise

===========================================================================*/
boolean diagcomm_is_ondevice_mode( diagcomm_io_conn_type * conn )
{
#ifdef DIAG_SIO_SUPPORT
 if( conn->open_params.sio_params.params.port_id == SIO_PORT_ONDEVICE_DIAG )
 {
   return TRUE;
 }
#endif

  return FALSE;

} /* diagcomm_is_ondevice_mode */


/*===========================================================================

FUNCTION DIAGCOMM_IO_CONN_CHANGED

DESCRIPTION
  This function is a wrapper for handling connection status changes.

PARAMETERS
  conn - Diag I/O connection handle
  connected - True if connection is opened.  False if connection is closed.


RETURN VALUE

===========================================================================*/
void diagcomm_io_conn_changed( diagcomm_io_conn_type * conn , boolean connected )
{
  //ToDo: Make this more generic
  if( (DIAGCOMM_PORT_SIO == conn->port_type) && (DIAGCOMM_IO_DATA == conn->channel_type) )
  {
    diagcomm_sio_conn_changed( conn, connected );
  }
  else if( (DIAGCOMM_PORT_SIO == conn->port_type) && (DIAGCOMM_IO_CTRL == conn->channel_type) )
  {
    diagcomm_sio_ctrl_conn_changed( conn, connected );
  }
  else if( DIAGCOMM_PORT_SMD == conn->port_type )
  {
    diagcomm_smd_conn_changed( conn, connected );
  }


} /* diagcomm_io_conn_changed */


/*===========================================================================

FUNCTION DIAGCOMM_IO_HAS_TX_DATA

DESCRIPTION
  This function checks whether the port has pending data to transmit.

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  None

===========================================================================*/
boolean diagcomm_io_has_tx_data( diagcomm_io_conn_type * conn )
{
 ASSERT( conn != NULL );

  if( (conn->connected == FALSE) || (conn->channel_connected == FALSE) )
    return FALSE;

  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT
    case DIAGCOMM_IO_TYPE_SIO:
    {
      return FALSE;
    }
#endif

    default:
      return FALSE;
  }

} /* diagcomm_io_has_tx_data */


/*===========================================================================

FUNCTION DIAGCOMM_IO_HAS_TX_DATA

DESCRIPTION
  This function is used to notify diag task of data being available on a channel.
  Depending on the connection type, the corresponding notify function is called.

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  None

===========================================================================*/

void diagcomm_io_notify_rx(diagcomm_io_conn_type *conn)
{
   int return_val = 0;
   osal_sigs_t return_sigs;

   if( (DIAGCOMM_IO_CMD == conn->channel_type) && (DIAGCOMM_PORT_SIO == conn->port_type) )
   {
     if( conn->port_num == DIAGCOMM_PORT_1 )
     {
       diag_rx_notify();
     }
     #ifdef DIAG_CONSUMER_API
       else if(  conn->port_num == DIAGCOMM_PORT_2 )
       {
         diag_rx_dci_notify();
       }
     #endif /* DIAG_CONSUMER_API */
   }
   else if( DIAGCOMM_IO_CTRL == conn->channel_type )
   {
     /* Set signal to diag task to process ctrl traffic from slave */
     return_val = osal_set_sigs(diag_task_tcb, DIAG_CTRL_MSG_SIG, &return_sigs);
     ASSERT(OSAL_SUCCESS == return_val);
   }
   else if( (DIAGCOMM_IO_DATA == conn->channel_type) && (DIAGCOMM_PORT_SIO == conn->port_type) )
   {
     diag_rx_notify();
   }
}


void diagcomm_io_change_connection(diagcomm_io_type io_type_to)
{
   int return_val;
   osal_sigs_t return_sigs;
   if(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].io_type != io_type_to)
   {
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].change_to_type = io_type_to;
      diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].change_to_type = io_type_to;
      diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].change_to_type = io_type_to;

      return_val = osal_set_sigs(diag_task_tcb, DIAG_CHANGE_CONN_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
   }

   if (io_type_to == DIAGCOMM_IO_TYPE_IPCR_UDP)
   {
      diag_send_buf_threshold = DIAG_MAX_SOCKETS_DATA_BLOCK_SIZE;
      diag_max_send_buf_len = DIAG_MAX_SOCKETS_DATA_BLOCK_SIZE;
      diag_tx_mode[DIAG_STREAM_1 - 1].curr_threshold = diag_max_send_buf_len;
   }
}


/*===========================================================================

FUNCTION DIAGCOMM_MASK_UPDATE

DESCRIPTION

PARAMETERS
  mask
  stream_id
  preset_id
  send_now

RETURN VALUE
  None

===========================================================================*/
#if defined(DIAG_MP_MASTER)
void diagcomm_mask_update( dword mask, uint8 stream_id, uint8 preset_id, boolean send_now ) //ToDo: OR the mask?
{
  uint8 port_num = 0;
  uint8 preset_index = 0;

  if( (stream_id > DIAG_MAX_STREAM_ID) || (preset_id > DIAG_MAX_PRESET_ID) )
    return;

  for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
  {
    if( (stream_id == DIAG_STREAM_1) || (stream_id == DIAG_STREAM_ALL) )
    {
      if( preset_id == DIAG_PRESET_MASK_ALL )
      {
        for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
        {
          DIAG_SET_MASK( diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].mask_preset_update[preset_index], mask );

          MSG_6(MSG_SSID_DIAG, MSG_LEGACY_LOW, "mask_update port_num=%d stream_id=%d preset_index=%d send=%d set=%d ret=%d",
                port_num, DIAG_STREAM_1, preset_index, send_now, mask,
                diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].mask_preset_update[preset_index]);
        }
      }
      else
      {
        DIAG_SET_MASK( diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].mask_preset_update[preset_id-1], mask );
        MSG_6(MSG_SSID_DIAG, MSG_LEGACY_LOW, "mask_update port_num=%d stream=%d preset=%d send=%d set=%d ret=%d",
              port_num, DIAG_STREAM_1, preset_id, send_now, mask,
              diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].mask_preset_update[preset_id-1]);
      }
    }

  #ifdef DIAG_CONSUMER_API
    if( (stream_id == DIAG_STREAM_2) || (stream_id == DIAG_STREAM_ALL) )
    {
      DIAG_SET_MASK( diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].mask_dci_update[DIAG_DCI_MASK_1-1], mask );
      MSG_6(MSG_SSID_DIAG, MSG_LEGACY_LOW, "mask_update port_num=%d stream=%d preset=%d send=%d set=%d ret=%d",
            port_num, DIAG_STREAM_2, preset_id, send_now, mask,
            diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].mask_dci_update[DIAG_DCI_MASK_1-1]);
    }
  #endif /* #ifdef DIAG_CONSUMER_API */
  }

  if( send_now )
  {
    for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
    {
      if( diagcomm_ctrl_channel_status(DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num) )
      {
        if( mask == DIAG_UPDATE_MASK_EVENTS )
        {
          if( (stream_id == DIAG_STREAM_1) || (stream_id == DIAG_STREAM_ALL) )
          {
            if( preset_id == DIAG_PRESET_MASK_ALL )
            {
              for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
              {
                diagcomm_ctrl_send_event_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                              (diagcomm_enum_port_type)DIAG_STREAM_1, preset_index+1, TRUE );
              }
            }
            else
            {
              diagcomm_ctrl_send_event_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                            (diagcomm_enum_port_type)DIAG_STREAM_1, preset_id, TRUE );
            }
          }
      #ifdef DIAG_CONSUMER_API
          if( (stream_id == DIAG_STREAM_2) || (stream_id == DIAG_STREAM_ALL) )
          {
            diagcomm_ctrl_send_event_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                          (diagcomm_enum_port_type)DIAG_STREAM_2, DIAG_DCI_MASK_1, TRUE );
          }
      #endif /* #ifdef DIAG_CONSUMER_API */
        }

        if( mask == DIAG_UPDATE_MASK_LOGS )
        {
          if( (stream_id == DIAG_STREAM_1) || (stream_id == DIAG_STREAM_ALL) )
          {
            if( preset_id == DIAG_PRESET_MASK_ALL )
            {
              for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
              {
                diagcomm_ctrl_send_log_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                            (diagcomm_enum_port_type)DIAG_STREAM_1, preset_index+1, TRUE );
              }
            }
            else
            {
              diagcomm_ctrl_send_log_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                          (diagcomm_enum_port_type)DIAG_STREAM_1, preset_id, TRUE );
            }
          }
      #ifdef DIAG_CONSUMER_API
          if( (stream_id == DIAG_STREAM_2) || (stream_id == DIAG_STREAM_ALL) )
          {
            diagcomm_ctrl_send_log_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                        (diagcomm_enum_port_type)DIAG_STREAM_2, DIAG_DCI_MASK_1, TRUE );
          }
      #endif /* #ifdef DIAG_CONSUMER_API */
        }

        if( mask == DIAG_UPDATE_MASK_PARTIAL_LOGS )
        {
          if( stream_id == DIAG_STREAM_1 )
          {
            if( preset_id != DIAG_PRESET_MASK_ALL )
            {
              diagcomm_ctrl_send_log_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                          (diagcomm_enum_port_type)DIAG_STREAM_1, preset_id, TRUE );
            }
          }
       #ifdef DIAG_CONSUMER_API
          if( stream_id == DIAG_STREAM_2 )
          {
            diagcomm_ctrl_send_log_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                        (diagcomm_enum_port_type)DIAG_STREAM_2, DIAG_DCI_MASK_1, TRUE );
          }
       #endif */ #ifdef DIAG_CONSUMER_API */
        }

        if( mask == DIAG_UPDATE_MASK_F3 )
        {
          if( (stream_id == DIAG_STREAM_1) || (stream_id == DIAG_STREAM_ALL) )
          {
            if( preset_id == DIAG_PRESET_MASK_ALL )
            {
              for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
              {
                diagcomm_ctrl_send_msg_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                            (diagcomm_enum_port_type)DIAG_STREAM_1, preset_index+1, TRUE );
              }
            }
            else
            {
              diagcomm_ctrl_send_msg_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                          (diagcomm_enum_port_type)DIAG_STREAM_1, preset_id, TRUE );
            }
          }
          #if defined(DIAG_STREAM_2_MASK_CHECK)
          if( (stream_id == DIAG_STREAM_2) || (stream_id == DIAG_STREAM_ALL) )
          {
            diagcomm_ctrl_send_msg_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                        (diagcomm_enum_port_type)DIAG_STREAM_2, DIAG_DCI_MASK_1, TRUE );
          }
          #endif
        }

        if( mask == DIAG_UPDATE_MASK_PARTIAL_F3 )
        {
          if( stream_id == DIAG_STREAM_1 )
          {
            diagcomm_ctrl_send_msg_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                        (diagcomm_enum_port_type)DIAG_STREAM_1, preset_id, TRUE );
          }
          #if defined(DIAG_STREAM_2_MASK_CHECK)
          if( stream_id == DIAG_STREAM_2 )
          {
            diagcomm_ctrl_send_msg_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                        (diagcomm_enum_port_type)DIAG_STREAM_2, DIAG_DCI_MASK_1, TRUE );
          }
          #endif
        }
      }
    }
  }
  else
  {
    diag_set_internal_sigs( DIAG_INT_MASK_UPDATE_SIG );
  }

} /* diagcomm_mask_update */
#endif /* DIAG_MP_MASTER */


