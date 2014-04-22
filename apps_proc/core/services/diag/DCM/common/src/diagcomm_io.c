/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diag I/O Wrapper APIs for Interprocessor Communication and I/O

General Description

Copyright (c) 2011-2014, 2016-2017, 2019-2020, 2022 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcomm_io.c#4 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/01/17   gn      Fixed invalid send_size issue on control channel
07/20/16   nk      Changes to support ThreadX
04/05/16   is      Check connection status prior to transmit
02/12/16   is      Remove diag_block() and don't call close with callback to allow 
                   MHI to drop packets completely.
02/01/16   is      Do not send data on closed channels
01/22/16   tbg     Removed PCIe fallback (now handled by port switching service)
11/25/14   ph      Mainlined the feature DIAG_CONSUMER_API.
11/06/14   is      Do not flush on first sio open and debug code
10/08/14   xy      Implement DCI discovery and DIAG DCI over PCIe 
08/12/14   xy      Set DTR=1 explicitly for SMD control channels in diagcomm_io_open() 
06/04/14   rh      Removed odd special case where we were sending to the
                   command channel immediately.
05/28/14   rh      Added error handling for command channel
03/13/14   is      Fix for 4k commands
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
01/27/14   is      Command/response separation
01/07/14   xy      Added fusion DCI feature   
11/10/13   sa      Support for UART specific SIO IOCTL
10/01/13   xy      Removed code under FEATURE_WINCE 
09/27/13   is      Fallback to USB port if PCIE fails
08/30/13   ph      Resetting the signal DIAG_BLOCK_SIG before we call sio_flush_tx().
08/29/13   sr      Fixed the missing break line issue in switch statements  
06/14/13   rh      Support for resending mask updates if we run out of memory
03/01/13   sr      Set DTR using sio_control_ioctl for control channel  
01/30/13   is      Klocwork warning fixes
01/16/13   rh      Renamed DIAG_TX_WMQ_SIG to DIAG_SMDL_WRITE_SIG
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
11/27/12   sa      Mainlining DIAG_CENTRAL_ROUTING
11/15/12   is      Support for preset masks
10/17/12   rh      Support t-put improvements with SMDL 
10/17/12   rh      Added api diagcomm_io_get_tx_wmq_cnt()
05/15/12   is      Allow "build time" mask queries from Master Diag 
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
#include "diag_fwd_v.h"
#include "dsm.h"
#include "ULog_Diag.h"
#include "diagdiag_v.h"

#if defined(DIAG_MP_MASTER)
#include "msgi.h"                 /* For diagcomm_ctrl_send_msg_mask() */
#include "eventi.h"               /* For diagcomm_ctrl_send_event_mask() */
#include "diaglogi.h"             /* For diagcomm_ctrl_send_log_mask() */
#endif
#include "diagcomm_dci.h"

#if defined(DIAG_SOCKET_SUPPORT)
#include "diagcomm_io_socket.h"
#endif

#ifdef DIAG_SIO_SUPPORT
  #include "diagcomm_io_sio.h"    /* For diagcomm_smd_sio_set_port */
#endif

#if defined(DIAG_FEATURE_EFS2)
  #include "diag_fs.h"
#endif


#if defined(DIAG_FLASH_LOGGING_SUPPORT)
  #include "diag_flash_logging_internal.h"
#endif



/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
extern osal_timer_t diag_sio_tout_timer;
extern osal_tcb_t diag_fwd_task_tcb;
extern osal_mutex_arg_t diag_flow_control_mutex;
#ifdef DIAG_CONSUMER_API
  extern diagcomm_io_conn_type diagcomm_io_dci_conn[NUM_SMD_PORTS];
#endif /* #ifdef DIAG_CONSUMER_API */
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern dsm_watermark_type diagcomm_smd_rx_wmq[NUM_SMD_PORTS];
extern dsm_watermark_type diagcomm_cmd_rx_wmq[NUM_SMD_PORTS];
#ifdef DIAG_CONSUMER_API
  extern dsm_watermark_type diagcomm_smd_dci_rx_wmq[NUM_SMD_PORTS];
#endif
extern dsm_watermark_type diagcomm_sio_rx_wmq[NUM_SIO_PORTS];

#if defined(DIAG_FLASH_LOGGING_SUPPORT)

  /*! @brief
      This global object shows the total number of bytes written to
      flash by diag. Only on re-boot this value would reset to 0.
  */
  uint32 bytes_written_to_flash = 0;

  
  /*! @brief 
      If diag_current_flash_filenum is 0 then we will be writing
      the logs to DIAG_FLASH_FILE1. 

      If diag_current_flash_filenum is 1 then we will be writing
      the logs to DIAG_FLASH_FILE2.
  */
    
  int diag_current_flash_filenum = 0;

  extern boolean diag_flash_logging_enabled;
  extern diag_fs_type diag_flash_log_files[2];
  
#endif // DIAG_FLASH_LOGGING_SUPPORT && DIAG_FEATURE_EFS2


/* Track whether this is the first time we are checking DTR status after sio_open on port 1 */
boolean diag_first_sio_DTR_check = TRUE;

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
        if( DIAGCOMM_PORT_SMD == conn->port_type )
        {
#ifdef DIAG_SMD_SUPPORT
          diagcomm_smd_open_init( conn );
#endif
        }
        else
        {
          diagcomm_sio_open_init( conn );
        }
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
  boolean dci_conn_check = FALSE;
  conn->connected = FALSE;
  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT  
    sio_ioctl_param_type param;
    
    case DIAGCOMM_IO_TYPE_SIO:
    { 
      #ifdef DIAG_CONSUMER_API
      if(DIAGCOMM_IO_DCI == conn->channel_type)
      {
        dci_conn_check = TRUE;
      }
      else
      {
        dci_conn_check = FALSE;
      }
      #else
        dci_conn_check = FALSE;
      #endif /* #ifdef DIAG_CONSUMER_API */
      if( ((DIAGCOMM_IO_DATA == conn->channel_type)||(dci_conn_check)) && (conn->open_params.sio_params.params.port_id != SIO_PORT_NULL) )
      {
       
        conn->handle.sio_handle = sio_open( &(conn->open_params.sio_params.params) );

        if( conn->handle.sio_handle != SIO_NO_STREAM_ID )
        {
          conn->connected = TRUE;
        
          /* Set flow control to allow flow.IOCTL will send command to firmware to up & down the RTS/CTS GPIOs based on request.
		     For 4 wire UART , these GPIOs are configured. For 2 wire, the above GPIOs are not configured, so IOCTL will ignore
			 it's functionality. As a result, same code works for both 2 wired and 4 wired UART. */
          sio_ioctl( conn->handle.sio_handle, SIO_IOCTL_INBOUND_FLOW_ENABLE, &param);
          #ifndef FEATURE_DIAG_SIO_UART
			/* Set CD line */
		    sio_ioctl( conn->handle.sio_handle, SIO_IOCTL_CD_ASSERT, &param);
       
			/* Set DSR line */
			sio_ioctl( conn->handle.sio_handle, SIO_IOCTL_DSR_ASSERT, &param);

			/* Clear RI line */
			sio_ioctl( conn->handle.sio_handle, SIO_IOCTL_RI_DEASSERT, &param);
		  #endif
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
          sio_control_ioctl( conn->handle.sio_handle, SIO_IOCTL_DSR_ASSERT, &param);

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
      int return_val;
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
  boolean dci_conn_check = FALSE;
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
     #ifdef DIAG_CONSUMER_API
       if(DIAGCOMM_IO_DCI == conn->channel_type)
       {
         dci_conn_check = TRUE;
       }
       else
       {
         dci_conn_check = FALSE;
       }
     #else
      dci_conn_check = FALSE;
	  #endif /* #ifdef DIAG_CONSUMER_API */
	  
      if( (DIAGCOMM_IO_DATA == conn->channel_type )||(dci_conn_check) )
        sio_close( conn->handle.sio_handle, NULL );
      else if( DIAGCOMM_IO_CTRL == conn->channel_type )
        sio_control_close( conn->handle.sio_handle, NULL );
        
      conn->handle.sio_handle = SIO_NO_STREAM_ID;
      
      #if defined (DIAG_RUNTIME_DEVMAP) 
        if( conn->open_params.sio_params.close_func_ptr != NULL )
        {
          (conn->open_params.sio_params.close_func_ptr) ();
        }
      #endif /* DIAG_RUNTIME_DEVMAP  */
    
      /* int32 smdl_close( smdl_handle_type port ); */
      
      /* Block on DIAG_BLOCK_SIG signal, which should be set by calling diag_release() 
         when SMDL_EVENT_CLOSE_COMPLETE event is recieved. */
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

#if defined(DIAG_FLASH_LOGGING_SUPPORT)
/*===================================================================================

FUNCTION DIAGCOMM_FLASH_TRANSMIT

DESCRIPTION
  This routine is called from diagcomm_io_transmit to route diag packets over flash
  
PARAMETERS
  @param tx_param
  Tx data containing buffer to transmit
 
DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the write to flash is successful and FALSE otherwise. 
============================================================================================*/

boolean diagcomm_flash_transmit(diagcomm_io_tx_params_type * tx_param)
{
  uint32 total_bytes = 0;
  int reset = 0;
  boolean write_result = FALSE;
  
  /* Total bytes of data in the DSMs. If DSMs are chained, this
     would be the summation of the data present in all the DSMs. */
  total_bytes = dsm_length_packet(tx_param->dsm_ptr);
  if(total_bytes > DIAG_FLASH_LOG_FILE_SIZE)
  {
    ULogFront_RealTimePrintf(diag_flash_logging_ulog_handle, 0, "Size of logs is more than the flash log file. Logs will be dropped");
	dsm_free_packet(&tx_param->dsm_ptr);
    return FALSE;
  }

  if(total_bytes > DIAG_FLASH_LOG_FILE_SIZE - diag_flash_log_files[diag_current_flash_filenum].write_pos)
  {
    /* There is not enough space in the current file. 
       Move to the other file, reset it by truncating its size to zero. */
    diag_current_flash_filenum = 1 - diag_current_flash_filenum; // toggles b/w 0 and 1
    
    // reset arg is passed as TRUE
    reset = diag_open_flash_file(&diag_flash_log_files[diag_current_flash_filenum], TRUE);
    if(reset<0)
    {
      ULogFront_RealTimePrintf(diag_flash_logging_ulog_handle, 1, "Reset failed for %dth flash file. Logs will be dropped",diag_current_flash_filenum);
      dsm_free_packet(&tx_param->dsm_ptr);
      return FALSE;
    }
  }

  write_result = diag_write_to_flash(&diag_flash_log_files[diag_current_flash_filenum],tx_param->dsm_ptr);
  if(write_result == FALSE)
  {
    ULogFront_RealTimePrintf(diag_flash_logging_ulog_handle, 1, "Write failed for %dth flash file. Logs will be dropped",diag_current_flash_filenum);
    dsm_free_packet(&tx_param->dsm_ptr);
    return FALSE;
  }

  /* Free the dsm item after writing to flash successfully.
     Not freeing the item would cause DSM exhaustion. */
  dsm_free_packet(&tx_param->dsm_ptr); 
  
  return TRUE;
}
#endif // DIAG_FLASH_LOGGING_SUPPORT

/*===========================================================================

FUNCTION DIAGCOMM_IO_TRANSMIT

DESCRIPTION
  Transmits data between two processors or I/O
  
PARAMETERS
  conn - Diag I/O connection handle
  tx_param - Tx data containing buffer to transmit
  
RETURN VALUE
  None
  
===========================================================================*/
int diagcomm_io_transmit( diagcomm_io_conn_type * conn, diagcomm_io_tx_params_type * tx_param )
{
        
  int num_write = 0;
  int ret = DIAG_EOK;
  boolean dci_conn_check = FALSE;

#if defined(DIAG_FLASH_LOGGING_SUPPORT)
    boolean ret_val = FALSE;
#endif

  if( (conn == NULL) || (tx_param == NULL) )
    return DIAG_EFAILED;

  /* Do not send data if port is not open. */
  if( conn->connected == FALSE )
    return DIAG_EFAILED;

  /* Do not send data if channel is not connected, except outbound transport port.
     PCIE does not notify Diag that channel is connected. */
  if( DIAGCOMM_PORT_SIO != conn->port_type )
  {
    if( conn->channel_connected == FALSE )
      return DIAG_EFAILED;
  }
  
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
      
       if( conn->connected )
       {
       #ifdef DIAG_CONSUMER_API
         if(DIAGCOMM_IO_DCI == conn->channel_type)
         {
           dci_conn_check = TRUE;
         }
         else
         {
           dci_conn_check = FALSE;
         }
       #else
         dci_conn_check = FALSE;
       #endif /* #ifdef DIAG_CONSUMER_API */
         if( (DIAGCOMM_IO_DATA == conn->channel_type)||(dci_conn_check) )
         {
#if defined(DIAG_FLASH_LOGGING_SUPPORT)
           if(DIAG_UNLIKELY(diag_flash_logging_enabled == TRUE)) // logging over flash
           {
             ret_val = diagcomm_flash_transmit(tx_param);
             if(ret_val == FALSE)
             {
               ret = DIAG_EFAILED;
               break;
             }
           }
           else // logging over usb
#endif // DIAG_FLASH_LOGGING_SUPPORT && DIAG_FEATURE_EFS2
           {
             sio_transmit( conn->handle.sio_handle, (dsm_item_type *)tx_param->dsm_ptr );
           }
   
         } 
         else if( DIAGCOMM_IO_CTRL == conn->channel_type )
         {
           sio_control_transmit( conn->handle.sio_handle, (dsm_item_type *)tx_param->dsm_ptr );
         }
       }
       ret = DIAG_EOK;

    
        /* enqueue every incoming dsm_ptr into the tx wmq */
        /* dsm_enqueue() supports prioritized queuing which puts DSM_HIGHEST priority item to
           the head of the queue. RSP packets won't be delayed due to queueing */

        /* set DIAG_TX_SIG to indicate that dsm item is queued up for Tx */
      break;
    }
#endif
#if defined(DIAG_SOCKET_SUPPORT)
        case DIAGCOMM_IO_TYPE_IPCR_UDP:
          if(FALSE == conn->allow_flow)
          {
             return DIAG_ERETRY;
          }
          if(0 == tx_param->smdl_params.len)
          {
             return DIAG_EPARAM;
          }

          osal_lock_mutex(&diag_flow_control_mutex);
          num_write = diagcomm_io_socket_send(conn, tx_param->smdl_params.buffer, tx_param->smdl_params.len);
          //Check if the previous send succeeded. If not, disable the flow.
          if( num_write == tx_param->smdl_params.len )
           {
               /*If we are sending data enable flow*/
               conn->allow_flow = TRUE;
               conn->diag_prev_tx_pending = FALSE; /* Packet successfully sent */
               ret = DIAG_EOK;
           }
           else if(num_write == QEAGAIN)
           {
              conn->diag_prev_tx_pending = TRUE;  /* FIFO is full; come back later */
              conn->allow_flow = FALSE;
              diagcomm_incr_flow_ctrl_count(DIAGCOMM_PORT_SMD);
              ret = DIAG_EFAILED;
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
  boolean dci_conn_check = FALSE;
  switch( conn->io_type )
  {
#ifdef DIAG_SIO_SUPPORT  
    case DIAGCOMM_IO_TYPE_SIO:
    {
      /* void sio_ioctl( sio_stream_id_type    stream_id, // Stream ID
                         sio_ioctl_cmd_type    cmd,       // IOCTL command
                         sio_ioctl_param_type *param );   // Parameter to be used for command
       */
      #ifdef DIAG_CONSUMER_API
      if(DIAGCOMM_IO_DCI == conn->channel_type)
      {
        dci_conn_check = TRUE;
      }
      else
      {
        dci_conn_check = FALSE;
      }
      #else
        dci_conn_check = FALSE;
      #endif /* #ifdef DIAG_CONSUMER_API */
      if ( conn->channel_type == DIAGCOMM_IO_CTRL )
      {
        sio_control_ioctl( conn->handle.sio_handle,
                            (sio_ioctl_cmd_type)cmd,
                            (sio_ioctl_param_type *)param );
      }
      else if ( (conn->channel_type == DIAGCOMM_IO_DATA)||(dci_conn_check) )
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
    
      /* int32 smdl_read( smdl_handle_type port,
                          uint32 len,
                          void *buffer,
                          uint32 flags ); */
    
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
    #ifdef DIAG_CONSUMER_API
      else if ((DIAGCOMM_IO_DCI == conn->channel_type) && (DIAGCOMM_PORT_SMD == conn->port_type)) {
        diagcomm_smd_dci_set_port(conn);
      }
    #endif /* #ifdef DIAG_CONSUMER_API */
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
        return_val_tout = osal_reset_sigs(&diag_task_tcb, DIAG_SIO_TOUT_TIMER_SIG, &return_sigs);
        return_val = osal_reset_sigs(&diag_task_tcb, DIAG_BLOCK_SIG, &return_sigs);
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
uint32 diagcomm_io_get_wmq_cnt( dsm_watermark_type * wmq_ptr )
{
  uint32 q_count = 0;

  if(wmq_ptr)
    {
    if(wmq_ptr->q_ptr)
      {
        q_count = q_cnt( wmq_ptr->q_ptr );
      }
  }
  
  /* fall through */
  return q_count;

} /* diagcomm_io_get_wmq_cnt */



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

  ASSERT(conn !=NULL );
  if(conn->rx_wmq_ptr)
    {
    if(conn->rx_wmq_ptr->q_ptr)
      {
        q_count = q_cnt( conn->rx_wmq_ptr->q_ptr );

      if( q_count > 0 )
      {
            return dsm_dequeue( conn->rx_wmq_ptr );
      }
    }
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

      if(conn->diag_prev_tx_pending)
      {
        if ((DIAGCOMM_IO_CMD == conn->channel_type) && (DIAGCOMM_PORT_SMD == conn->port_type))
        {
            /* We might get flow enabled on command channel
               Scenario - Break Q6 and Resume. This means Apps continues to send data to slave
               and eventually hit QEAGAIN error because socket command channel is full.
               In this case set allow_flow flag back to TRUE */
            MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Flow disabled on command channel ");
            conn->allow_flow = TRUE;
            conn->diag_resend_data = FALSE;
            conn->data_pending = FALSE;
        }
        else if ((DIAGCOMM_IO_CTRL == conn->channel_type) && (DIAGCOMM_PORT_SMD == conn->port_type))
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
} /* diagcomm_io_disable_flow */


/*===========================================================================

FUNCTION DIAGCOMM_IO_DETECT_CONNECTION

DESCRIPTION
  Register DTR callbacks to detect connection change.
  
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
      #ifdef DIAG_CONSUMER_API
        if( conn->port_type == DIAGCOMM_PORT_SMD && conn->channel_type == DIAGCOMM_IO_DCI )
        {    
          diag_dci_dtr_enable(conn);
          return;
        }
      #endif /* #ifdef DIAG_CONSUMER_API */
      diag_dtr_enable( conn );             /* Registering the callback for DTR changes on SMD and SIO ports */
     

#if defined (DIAG_MP_MASTER)
      if( conn->port_type == DIAGCOMM_PORT_SIO )
      {
         if(conn->port_num == DIAGCOMM_PORT_1)
         {    
            diag_dtr_changed_cb();
            diag_first_sio_DTR_check = FALSE;
            diagcomm_sio_conn_changed( conn, TRUE );
            return;
         }
      }
#endif
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
  if( conn == NULL )
    return FALSE;
    
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
 //if( conn->open_params.sio_params.params.port_id == SIO_PORT_ONDEVICE_DIAG )
 //{
   return TRUE;
 //}
#endif

  //return FALSE;
  
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
  if( (DIAGCOMM_PORT_SMD == conn->port_type) && (DIAGCOMM_IO_DATA == conn->channel_type) )
  {
    diagcomm_sio_conn_changed( conn, connected );
  }
  else if( (DIAGCOMM_PORT_SMD == conn->port_type))
  {
    if( (DIAGCOMM_IO_CTRL == conn->channel_type) ||
        (DIAGCOMM_IO_CMD == conn->channel_type) 
        #ifdef DIAG_CONSUMER_API
        ||(DIAGCOMM_IO_DCI == conn->channel_type)
        #endif /* DIAG_CONSUMER_API */
        )
    {
        if (!connected)
        {
            conn->channel_connected = FALSE;
  }
        else
  {
            conn->channel_connected = TRUE;
        }
    }
  }
} /* diagcomm_io_conn_changed */
/*===========================================================================

FUNCTION DIAGCOMM_IO_NOTIFY_RX

DESCRIPTION
  This function is to notify of data received on any diag channel over
  IPCRTR. This sets signal to DIAG or DIAG_FWD task depending on the
  channel where the data is received.

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  None

===========================================================================*/
void diagcomm_io_notify_rx(diagcomm_io_conn_type *conn)
{
   int return_val = 0;
   osal_sigs_t return_sigs;

   if( (DIAGCOMM_IO_CMD == conn->channel_type) && (DIAGCOMM_PORT_SMD == conn->port_type) )
   {
     if( conn->port_num == DIAGCOMM_PORT_1 )
     {
        /* Responses recieved from peripherals available to be read. */
        return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_CMD_READ_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
         
     }
   #ifdef DIAG_CONSUMER_API
       else if( conn->port_num == DIAGCOMM_PORT_2 )
       {
           /* Responses recieved from DCI available to be read. */
           return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_READ_DCI_SIG, &return_sigs);
           ASSERT(OSAL_SUCCESS == return_val);
       }
   #endif /* DIAG_CONSUMER_API */
     else
     {
         MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid port number %d", conn->port_num );
     }
   }
   else if( DIAGCOMM_IO_CTRL == conn->channel_type )
   {
    /* Set signal to diag task to process ctrl traffic from slave */
    return_val = osal_set_sigs(&diag_task_tcb, DIAG_CTRL_MSG_SIG, &return_sigs );
    ASSERT(OSAL_SUCCESS == return_val);

   }
   else if( (DIAGCOMM_IO_DATA == conn->channel_type) && (DIAGCOMM_PORT_SMD == conn->port_type) )
   {
      /* Data recieved from peripherals available to be read. */
     if( conn->port_num == DIAGCOMM_PORT_1 )
     {
        return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_READ_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
     }
   }
   #ifdef DIAG_CONSUMER_API
     else if ((DIAGCOMM_IO_DCI == conn->channel_type) && (DIAGCOMM_PORT_SMD == conn->port_type) )
     {
        /* Responses recieved from DCI available to be read. */
        return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_READ_DCI_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
     }
   #endif /* DIAG_CONSUMER_API */

}


/*===========================================================================

FUNCTION DIAGCOMM_MASK_UPDATE

DESCRIPTION
  This function is used to set a bitmask to indicate some F3, log, or event
  mask for a particular stream id has been updated and needs to be propagated
  to the Slaves. This function may also set a signal to notify Diag task to 
  propagate the masks.

PARAMETERS
  
  mask
  stream_id
  preset_id
  send_now
  
  mask - One of DIAG_UPDATE_MASK_* bitmask values to indicate that some 
         F3, log, or event mask has been updated and needs to be propagated
         to slaves.
  stream_id - One of DIAG_STREAM_# to indicate which stream IDs mask has 
              been updated.
  preset_id - A value between 0 (all presets) and DIAG_MAX_PRESET_ID
              which indicates what preset needs to be updated
  send_now - whether to do things immediately or set a signal
            
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
          diagcomm_ctrl_set_mask_status(DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num, mask, DIAG_STREAM_1, preset_index+1);
          
          MSG_6(MSG_SSID_DIAG, MSG_LEGACY_LOW, "mask_update port_num=%d stream_id=%d preset_index=%d send=%d set=%d ret=%d", 
                port_num, DIAG_STREAM_1, preset_index, send_now, mask, 
                diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].mask_preset_update[preset_index]);
        }
      }
      else
      {
        diagcomm_ctrl_set_mask_status(DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num, mask, DIAG_STREAM_1, preset_id);
        MSG_6(MSG_SSID_DIAG, MSG_LEGACY_LOW, "mask_update port_num=%d stream=%d preset=%d send=%d set=%d ret=%d", 
              port_num, DIAG_STREAM_1, preset_id, send_now, mask, 
              diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].mask_preset_update[preset_id-1]);
      }
    }

  #ifdef DIAG_CONSUMER_API
    if( (stream_id == DIAG_STREAM_2) || (stream_id == DIAG_STREAM_ALL && DIAG_STREAM_2 <= DIAG_MAX_STREAM_ID) )
    {
      diagcomm_ctrl_set_mask_status(DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num, mask, DIAG_STREAM_2, DIAG_DCI_MASK_1);
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
      if( diagcomm_ctrl_status(DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num) )
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
             if (diagcomm_io_dci_conn[port_num].connected) 
             {
    
               MSG (MSG_SSID_DIAG, MSG_LEGACY_LOW, "diacomm_ctrl_send_event_mask");
               diagcomm_ctrl_send_event_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                             (diagcomm_enum_port_type)DIAG_STREAM_2, DIAG_DCI_MASK_1, TRUE );
             }
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
             if (diagcomm_io_dci_conn[port_num].connected) 
             {
               MSG (MSG_SSID_DIAG, MSG_LEGACY_LOW, "diacomm_ctrl_send_log_mask");
               diagcomm_ctrl_send_log_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                           (diagcomm_enum_port_type)DIAG_STREAM_2, DIAG_DCI_MASK_1, TRUE );
             }
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
      #endif /* #ifdef DIAG_CONSUMER_API */
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


