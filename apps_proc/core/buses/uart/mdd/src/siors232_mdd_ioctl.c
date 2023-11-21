/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             S I O R S 2 3 2  M D D
                             
            SIO Media Dependent Driver layer for RS-232 communication

GENERAL DESCRIPTION

  This module contain the code shared by different serial driver.  It calls
  seperate hardware dependent driver code to access the actual UART hardware.
  

EXTERNALIZED FUNCTIONS
    
  siors_mdd_api_ioctl()
    Control open stream.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  siors_mdd_api_dev_init() must be called prior to any of the other functions of
  this module.

Copyright (c) 1999-2017 Qualcomm Technologies Incorporated.
All Rights Reserved.  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/buses/uart/mdd/src/siors232_mdd_ioctl.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when         who       what, where, why
-----------  --------  ----------------------------------------------------------
03-Mar-2017  rchava    Added new IOCTL to get the current baud rate 
06-Dec-2013  pchopra   Adding one way UART communication
05-Apr-2010  cmihalik  Initiali revision.  Split ioctl call hierarchy into file.

===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

#include "DalUart.h"
#include "sio_wrapper.h"

#include "siors232_mdd.h"
#include "siors232_mdd_defs.h"
#include "siors232_mdd_data.h"
#include "uart_compat.h"
#include "uart_log.h"
#include "qurt_mutex.h"


//  If there is is at least one UART.
#if( defined(FEATURE_FIRST_UART)  || \
     defined(FEATURE_SECOND_UART) || \
     defined(FEATURE_THIRD_UART))              //{

//--------------------------------------------------------------------------
//  File id for buffer logging.
//--------------------------------------------------------------------------

#define  UART_TRACE_BUFFER_FILE_ID  4

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------

static void     ioctl_baudrate_change_now( sio_mdd_port_type*, sio_bitrate_type );

/*===========================================================================

FUNCTION SIORS_MDD_SET_PACKET_MODE

DESCRIPTION
  Check the condition to see if it is possible to set up packet mode
  operation.  Physical device driver may respond to the packt mode
  variable by increase the size of the packet in each transfer.

DEPENDENCIES
  UART is running and enabled.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void 
set_packet_mode( sio_mdd_port_type* mdd_port )
{
  int                              device_id = mdd_port->port_data.device_id;
  sio_mdd_rx_data_type*            rxd = &mdd_port->rx_data;
  sio_mdd_tx_data_type*            txd = &mdd_port->tx_data;


  UART_LOG_1( TRACE, "[%d] +set_packet_mode", device_id );

  rxd->sio_uart_packet_mode = TRUE;

  if( mdd_port->port_data.device_id != UART_DEVICE_1 )
  {
    rxd->sio_uart_packet_mode = FALSE;
  }

  if( rxd->sio_current_stream_mode != SIO_DS_PKT_MODE )
  {
    rxd->sio_uart_packet_mode = FALSE;
  }
  
  if(( rxd->sio_rx_flow_control_method != SIO_CTSRFR_FCTL ) &&
     ( rxd->sio_rx_flow_control_method != SIO_CTSRFR_AUTO_FCTL ))
  {
    rxd->sio_uart_packet_mode = FALSE;
  }

  if(( txd->sio_tx_flow_control_method != SIO_CTSRFR_FCTL ) &&
     ( txd->sio_tx_flow_control_method != SIO_CTSRFR_AUTO_FCTL ))
  {
    rxd->sio_uart_packet_mode = FALSE;
  }

  if (siors_mdd_set_dsm_pool_id (NULL, rxd) == 0)
  {
    UART_LOG_2( FATAL, "[%d] Invalid dsm pool detected for stream mode: %d", device_id, rxd->sio_current_stream_mode);
  }

  UART_LOG_1( TRACE, "[%d] -set_packet_mode", device_id );
}

/*===========================================================================

FUNCTION SIORS_MDD_SET_FLOW_CTL                             INTERNAL FUNCTION

DESCRIPTION
  This function will set the flow control method between the DCE
  and the DTE for the given device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static void 
ioctl_set_flow_ctl( sio_mdd_port_type *mdd_port, sio_flow_ctl_type tx_flow, sio_flow_ctl_type rx_flow )             
{
  int                              device_id = mdd_port->port_data.device_id;
  sio_mdd_rx_data_type*            rxd = &mdd_port->rx_data;
  sio_mdd_tx_data_type*            txd = &mdd_port->tx_data;



  UART_LOG_1( TRACE, "[%d] +ioctl_set_flow_ctl", device_id );
  qurt_mutex_lock( rxd->lock_handle );
  qurt_mutex_lock( txd->lock_handle );
  if( txd->is_open )
  {
    
    if( tx_flow == SIO_FCTL_BEST )
    {
      tx_flow = txd->sio_tx_flow_control_method;
    }
    
    if( txd->sio_tx_flow_control_method != tx_flow )
    {
      //  It is ambiguous as to whether or not the old flow control state 
      //  should be kept when flow control method is changed.  For instance,
      //  in software flow control TX has to be explicitly stopped by software,
      //  wheareas in hardware flow control, TX flow control is done automatically
      //  by UART hardware.
      txd->sio_outbound_flow_disabled = FALSE;
    }

    txd->sio_tx_flow_control_method = tx_flow;

    switch(tx_flow)
    {
      case SIO_XONXOFF_STRIP_FCTL_FS:
      case SIO_XONXOFF_STRIP_FCTL_NFS:
      case SIO_XONXOFF_NSTRIP_FCTL_FS:
      case SIO_XONXOFF_NSTRIP_FCTL_NFS:
      case SIO_MAX_FLOW:      
        UART_LOG_1( ERROR, "[%d] Software flow control not supported!", device_id );
        break;

      case SIO_FCTL_OFF:
        DalUart_SetCtsControl( mdd_port->port_data.dal_device_handle, CTS_DISABLE );
        break;

      case SIO_CTSRFR_AUTO_FCTL:
      case SIO_FCTL_BEST:
      case SIO_CTSRFR_FCTL:
        DalUart_SetCtsControl( mdd_port->port_data.dal_device_handle, CTS_ENABLE );
        break;

      default:
        break;
    }
  }
  else
  {
    UART_LOG_1( TRACE, "[%d] This port won't support the transmit path", device_id );
  }

  if( rxd->is_open )
  {  
  
    if( rx_flow == SIO_FCTL_BEST )
    {
      rx_flow = rxd->sio_rx_flow_control_method;
    }
    //  Every time that flow control is reset, need to reset XON XOFF flow 
    //  control variables.  If switching to XON/XOFF, need to have the 
    //  variables in a good state.  And if switching from XON/XOFF, need to 
    //  have XON/XOFF flow control variables to not interfere with HW or other 
    //  flow control methods
  
    if( rxd->sio_rx_flow_control_method != rx_flow )
    {
      //  It is ambiguous as to whether or not the old flow control state should
      //  be kept when flow control method is changed.  For instance, should
      //  software consider a previous asserted hardware flow control line as
      //  tantamount to having received an XOFF ?  For now, default to deasserted
      //  flow control.

      rxd->sio_inbound_flow_enabled = TRUE;
    } 
  
    rxd->sio_rx_flow_control_method = rx_flow;
  
    switch(rx_flow)
    {
      case SIO_XONXOFF_STRIP_FCTL_FS:
      case SIO_XONXOFF_STRIP_FCTL_NFS:
      case SIO_XONXOFF_NSTRIP_FCTL_FS:
      case SIO_XONXOFF_NSTRIP_FCTL_NFS:
      case SIO_MAX_FLOW:
      case SIO_FCTL_BEST:
      case SIO_CTSRFR_FCTL:
    UART_LOG_1( ERROR, "[%d] Software flow control not supported!", device_id );
        break;

      case SIO_FCTL_OFF:
        DalUart_SetRtsControl( mdd_port->port_data.dal_device_handle, RTS_DEASSERT );
        break;
      
      case SIO_CTSRFR_AUTO_FCTL:
        //  If auto hardware flow control is what caller requested,
        //  enable auto hardware flow control.
        //  Have to set the RFR trigger level before enable the flow control */  
        DalUart_SetRtsControl( mdd_port->port_data.dal_device_handle, RTS_AUTO );
        break;

      default:
        break;
    }
    set_packet_mode( mdd_port );
  }
  else
  {
    UART_LOG_1( TRACE, "[%d] This port won't support the recieve path", device_id );
  }
  qurt_mutex_unlock( txd->lock_handle );
  qurt_mutex_unlock( rxd->lock_handle);
  UART_LOG_1( TRACE, "[%d] -ioctl_set_flow_ctl", device_id );
} 

/*===========================================================================

FUNCTION SIORS_MDD_GET_FLOW_CTL                             INTERNAL FUNCTION

DESCRIPTION
  This function will get the flow control method between the DCE
  and the DTE for the given device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Fills in the flow control methods into the param union
  
===========================================================================*/

static void 
ioctl_get_flow_ctl( sio_mdd_port_type* mdd_port, 
                    sio_flow_ctl_type* rx_flow, 
                    sio_flow_ctl_type* tx_flow )
{
  sio_mdd_rx_data_type*            rxd = &mdd_port->rx_data;
  sio_mdd_tx_data_type*            txd = &mdd_port->tx_data;
  uint32 device_id = mdd_port->port_data.device_id;

  qurt_mutex_lock( rxd->lock_handle );
  qurt_mutex_lock( txd->lock_handle );

  if( rxd->is_open )
  {
    *rx_flow = mdd_port->rx_data.sio_rx_flow_control_method; 
  }
  else
  {
    *rx_flow = SIO_MAX_FLOW;      
    UART_LOG_1( TRACE, "[%d] This port won't support the recieve path", device_id );
  }

  if( txd->is_open )
  {
    *tx_flow = mdd_port->tx_data.sio_tx_flow_control_method;
  }
  else
  {
    *tx_flow = SIO_MAX_FLOW;
    UART_LOG_1( TRACE, "[%d] This port won't support the transmit path", device_id );
  }
  qurt_mutex_unlock( txd->lock_handle );
  qurt_mutex_unlock( rxd->lock_handle );
} 

/*===========================================================================

FUNCTION 

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/

#define bit_rate_func( instance )                               \
                                                                \
static void                                                     \
do_bitrate_change_##instance( void )                            \
{                                                               \
  sio_mdd_port_type*   mdd_port;                                \
  sio_bitrate_type     new_bitrate;                             \
                                                                \
  mdd_port = &siors_mdd_ports[UART_DEVICE_##instance];          \
  new_bitrate = mdd_port->stream_data.sio_new_bitrate;          \
  ioctl_baudrate_change_now( mdd_port, new_bitrate );           \
}

bit_rate_func(1)
bit_rate_func(2)
bit_rate_func(3)

/*===========================================================================

FUNCTION SIORS_MDD_SCHEDULE_BAUDRATE_CHANGE

DESCRIPTION
  This function schedules a baud-rate change.  Here's what happens - 
  this function will set up some vars that tell the code to change
  the baudrate.  After the NEXT TX ISR, where there is no data
  left to be transmitted (either in the Tx watermark or in the Tx
  FIFO), then the baudrate is changed.
  
  Note: when this function is called, if there is currently no data
  left to be xmitted (s/w watermark and h/w fifo are empty), then the
  baudrate is NOT changed.  It must still wait for the next Tx ISR.
  
  This follows the scheme used in the old SIO.  That scheme assumed
  the baudrate was only changed on AT+IPR from the connected laptop;
  the DS code would call the sched_baudrate command, and then call
  the transmit() function to send the "OK" back to the laptop.  So
  we assume the same scheme.

DEPENDENCIES
  Assume that a Tx ISR will happen in our future.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void 
ioctl_schedule_baudrate_change( sio_mdd_port_type* mdd_port, sio_bitrate_type new_bitrate )
{
  int                  device_id = mdd_port->port_data.device_id;
  sio_flush_func       on_flush_complete = NULL;
  boolean              port_locked = FALSE;




  UART_LOG_1( TRACE, "[%d] +ioctl_schedule_baudrate_change", device_id );

  //  Verify parameters.

  if( !( new_bitrate < SIO_BITRATE_MAX ))
  {
    UART_LOG_2( TRACE, "[%d] Invalid bitrate %d", device_id, new_bitrate );
    goto DONE;
  }

  qurt_mutex_lock( mdd_port->port_data.lock_handle );
  port_locked = TRUE;

  if( !mdd_port->stream_data.is_open )
    goto DONE;

  //  Save the baudrate info in a global.  This info is later 
  //  accessed in siors_mdd_set_bitrate.

  mdd_port->stream_data.sio_new_bitrate = new_bitrate;

  //  We then store the bitrate change func as the close_func_ptr.
  //  This is because when the tx finally empties, this callback is
  //  called (if appropriate flags are set).  We are re-using that
  //  functionality, but using our own callback.

  switch( mdd_port->port_data.device_id )
  {
          case UART_DEVICE_1: 
      
      on_flush_complete = do_bitrate_change_1; 
      break;

          case UART_DEVICE_2: 
      
      on_flush_complete = do_bitrate_change_2; 
      break;

          case UART_DEVICE_3: 
      
      on_flush_complete = do_bitrate_change_3; 
      break;

      default:
      goto DONE;
  }

  if( is_tx_stream( mdd_port->stream_data.sio_type_of_stream ))
  {
    qurt_mutex_unlock(( mdd_port->port_data.lock_handle) );
    port_locked = FALSE;

    qurt_mutex_lock(( mdd_port->tx_data.lock_handle) );

    siors_mdd_flush_tx( mdd_port, FALSE, on_flush_complete );
    //  siors_mdd_flush_tx unlocks TX.
  }

DONE:;

  if( port_locked )
  {
    qurt_mutex_unlock( mdd_port->port_data.lock_handle );
  }

  UART_LOG_1( TRACE, "[%d] -ioctl_schedule_baudrate_change", device_id );
} 

/*===========================================================================

FUNCTION SIORS_MDD_GET_BAUDRATE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void 
ioctl_uart_get_baudrate( sio_mdd_port_type* mdd_port, sio_bitrate_type *current_bitrate )
{
  int                      device_id = mdd_port->port_data.device_id;


  UART_LOG_1( TRACE, "[%d] +ioctl_uart_get_baudrate", device_id );

  qurt_mutex_lock( mdd_port->port_data.lock_handle );

  if( !mdd_port->stream_data.is_open )
  {
    *current_bitrate = SIO_BITRATE_ILLEGAL_1;
    goto DONE;
  }

  *current_bitrate = mdd_port->stream_data.sio_current_bitrate;

DONE:;

  qurt_mutex_unlock( mdd_port->port_data.lock_handle );

  UART_LOG_1( TRACE, "[%d] -ioctl_uart_get_baudrate", device_id );
} 


/*===========================================================================

FUNCTION SIORS_MDD_BAUDRATE_CHANGE_NOW

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void 
ioctl_baudrate_change_now( sio_mdd_port_type* mdd_port, sio_bitrate_type new_bitrate )
{
  int                      device_id = mdd_port->port_data.device_id;
  uint32                   dal_baud_rate;
  DALResult                dal_result;
  sio_bitrate_type         current_bitrate;
  boolean                  bad_state;
  boolean                  port_locked = FALSE;
  boolean                  tx_locked = FALSE;




  UART_LOG_1( TRACE, "[%d] +ioctl_baudrate_change_now", device_id );

  qurt_mutex_lock( mdd_port->port_data.lock_handle );
  port_locked = TRUE;

  if( !mdd_port->stream_data.is_open )
    goto DONE;

  if( !( new_bitrate < SIO_BITRATE_MAX ))
    goto DONE;

  current_bitrate = mdd_port->stream_data.sio_current_bitrate;

  if( current_bitrate == new_bitrate )
  {
    UART_LOG_1( TRACE, "[%d] new bitrate = current bitrate, do nothing", device_id );
    goto DONE;
  }

  //  Normally, don't need to do stream state validation here.
  //  But this function can be called as a callback.  Let's 
  //  see if it still makes sense to change the baud rate.

  qurt_mutex_lock( mdd_port->tx_data.lock_handle );
  tx_locked = TRUE;

  bad_state = FALSE;
  if(  mdd_port->tx_data.is_open )
  {
    switch( mdd_port->tx_data.tx_stream_state )
    {
      case TX_STREAM_STATE_OPEN:

        //  OK, proceed.
        break;

      case TX_STREAM_STATE_CLOSED:
      case TX_STREAM_STATE_FLUSH_WAIT_FLUSH_TX_FIFO:
      case TX_STREAM_STATE_FLUSH_WAIT_TX_COMPLETE:
      case TX_STREAM_STATE_FLUSH_WAIT_NEXT_TX:

        //  Woopsie.
        UART_LOG_1( TRACE, "[%d] Invalid state", device_id );
        bad_state = TRUE;
        break;

      default:

        UART_LOG_1( ERROR, "[%d] Unhandled case in switch", device_id );
        break;
    }
  }
  else
  {
    UART_LOG_1( TRACE, "[%d] This port won't support the transmit path", device_id );
  }  
  qurt_mutex_unlock(mdd_port->tx_data.lock_handle );
  tx_locked = FALSE;

  if( bad_state )
    goto DONE;

  //  Set the baud rate.

  dal_baud_rate = siors_mdd_to_dal_baud_rate( new_bitrate );
  dal_result    = DalUart_SetRate( mdd_port->port_data.dal_device_handle, dal_baud_rate );

  if( UART_SUCCESS == dal_result )
    mdd_port->stream_data.sio_current_bitrate = new_bitrate;
  else
    UART_LOG_2( TRACE, "[%d] Error %d changing baud rate", device_id, dal_result );

DONE:;

  if( tx_locked )
    qurt_mutex_unlock( mdd_port->tx_data.lock_handle );

  if( port_locked )
    qurt_mutex_unlock( mdd_port->port_data.lock_handle );

  UART_LOG_1( TRACE, "[%d] -ioctl_baudrate_change_now", device_id );
} 

/*===========================================================================

FUNCTION IOCTL_CANCEL_FLUSH

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

static void
ioctl_cancel_flush( sio_mdd_port_type *mdd_port )
{
  qurt_mutex_lock( mdd_port->tx_data.lock_handle );

  UART_LOG_1( TRACE, "+ioctl_cancel_flush: state %d", mdd_port->tx_data.tx_stream_state );

  switch( mdd_port->tx_data.tx_stream_state )
  {
    case TX_STREAM_STATE_OPEN:
    case TX_STREAM_STATE_CLOSED:
      //  Nothing to do -- port is not flushing.
      break;

    case TX_STREAM_STATE_FLUSH_WAIT_FLUSH_TX_FIFO:
    case TX_STREAM_STATE_FLUSH_WAIT_NEXT_TX:
      mdd_port->tx_data.tx_stream_state = TX_STREAM_STATE_OPEN;
      break;

    case TX_STREAM_STATE_FLUSH_WAIT_TX_COMPLETE:
      mdd_port->tx_data.tx_stream_state = TX_STREAM_STATE_OPEN;
      timer_clr( &mdd_port->tx_data.sio_stream_close_cb_item, T_NONE );
      break;

    default:
      UART_LOG_1( ERROR, "ioctl_cancel_flush: Invalid state %d", mdd_port->tx_data.tx_stream_state );
      break;
  }

  UART_LOG( TRACE, "-ioctl_cancel_flush" );

  qurt_mutex_unlock( mdd_port->tx_data.lock_handle );
}

/*===========================================================================

FUNCTION SIORS_MDD_CHANGE_MODE

DESCRIPTION
  Changes the operating mode characteristics of the SIO stream.
  The Tx and Rx may be enabled during this (if we had been autobauding 
  and we're now changing to RAW or PKT modes).
  
  if the new mode == the old mode, then we exit without doing 
  anything.

DEPENDENCIES
  Assumes the user has already opened the stream, and is merely
  changing modes.
  
  Any watermarks which are obsoleted are not freed; it is up to the 
  application to free information stored on the watermark.

  Also assumes that the user has flushed any pending output.
  
RETURN VALUE
  None

SIDE EFFECTS
  May end up in changing baud rate (if going into/out of autodetect).
===========================================================================*/

static void 
ioctl_change_mode( sio_stream_id_type stream_id, sio_mdd_port_type* mdd_port, sio_open_type* open_ptr )
{
  int                              device_id = mdd_port->port_data.device_id;
  sio_mdd_rx_data_type*            rxd = &mdd_port->rx_data;
  sio_mdd_tx_data_type*            txd = &mdd_port->tx_data;
  sio_stream_mode_type             old_mode;   
  boolean                          port_locked = FALSE;
  boolean                          rx_locked = FALSE;
  boolean                          tx_locked = FALSE;



  UART_LOG_1( TRACE, "[%d] +ioctl_change_mode", device_id );

  qurt_mutex_lock( mdd_port->port_data.lock_handle );
  port_locked = TRUE;

  qurt_mutex_lock( rxd->lock_handle );
  rx_locked = TRUE;

  if( !is_rx_stream( mdd_port->stream_data.sio_type_of_stream ))
  {
    UART_LOG_1( TRACE, "[%d] Stream not open for rx", device_id );
    goto DONE;
  }

  if( mdd_port->stream_data.open_stream_id != stream_id )
  {
    UART_LOG_1( TRACE, "[%d] Stream ids don't match", device_id );
    goto DONE;
  }

  if( open_ptr->rx_bitrate != open_ptr->tx_bitrate )
  {
    UART_LOG_1( TRACE, "[%d] rx_bitrate != tx_bitrate", device_id );
    goto DONE;
  }
   
  if( open_ptr->stream_mode == rxd->sio_current_stream_mode )
  {
    goto DONE;   //  do nothing, same mode.
  }

  //  Based on the new mode, we will have to reset some
  //  vars, and then set some other vars.  We take each
  //  one within a switch statement.  
  //
  //  We will first do the the assignments common to all
  //  modes.  Then we will proceed with any special initialization
  //  required by a mode.

  //  Reset All of the variables upon mode change.  This
  //  is common for all types of modes.  We do the tx first,
  //  then the Rx vars.

  //  TX vars.

  qurt_mutex_lock( txd->lock_handle );
  tx_locked = TRUE;
  if( txd->is_open )
  {
    if( open_ptr->tx_queue != txd->sio_tx_watermark_queue_ptr )
    {
      if( txd->sio_tx_watermark_queue_ptr != NULL )
      {
        if( txd->sio_tx_watermark_queue_ptr->current_cnt > 0 )
        {
          UART_LOG_1( ERROR, "[%d] We are going to erase a non-empty watermark", device_id );
        }
        dsm_empty_queue( txd->sio_tx_watermark_queue_ptr );
      }

      txd->sio_tx_watermark_queue_ptr = open_ptr->tx_queue;

      if( txd->sio_tx_current_wm_item_ptr != NULL )
        dsm_free_packet( &txd->sio_tx_current_wm_item_ptr );

      txd->sio_bytes_to_tx = 0;
    }
  }
  else
  {
    UART_LOG_1( TRACE, "[%d] This port won't support the transmit path", device_id );
  }
  
  qurt_mutex_unlock( txd->lock_handle );
  tx_locked = FALSE;

  //  RX vars.

  if( !rxd->is_open )
  {
    UART_LOG_1( TRACE, "[%d] This port won't support the recieve path", device_id );
    goto DONE;
  }
  rxd->sio_tail_char_used       = open_ptr->tail_char_used;
  rxd->sio_tail_char            = open_ptr->tail_char;
  rxd->sio_escape_code_func_ptr = NULL;
  rxd->sio_packet_func_ptr      = NULL;
                                       
  if( open_ptr->rx_queue != rxd->sio_rx_watermark_queue_ptr )
  {
    if( rxd->sio_rx_watermark_queue_ptr != NULL )
    {
      if( rxd->sio_rx_watermark_queue_ptr->current_cnt > 0 )
      {
        UART_LOG_1( ERROR, "[%d] We are going to erase a non-empty watermark", device_id );
      }
      dsm_empty_queue( rxd->sio_rx_watermark_queue_ptr );
    }
    rxd->sio_rx_watermark_queue_ptr = open_ptr->rx_queue; 
  }

  rxd->sio_rx_func_ptr = open_ptr->rx_func_ptr;
  
  //  Some of the modes need special cleanup, so we do that
  //  here

  old_mode = rxd->sio_current_stream_mode;
  rxd->sio_current_stream_mode = open_ptr->stream_mode;

  if (siors_mdd_set_dsm_pool_id (open_ptr, rxd) == 0)
  {
    UART_LOG_2 (FATAL, "[%d] Invalid dsm pool detected for stream mode: %d", device_id, open_ptr->stream_mode);
  }
  
  switch( old_mode )
  {
    case SIO_DS_PKT_MODE:

      rxd->adv.ds_incall_device_id = UART_DEVICE_INVALID;
      rxd->adv.ds_incall_stream_id = SIO_NO_STREAM_ID;
      break;

    case SIO_DS_RAWDATA_MODE:

      rxd->adv.ds_incall_device_id = UART_DEVICE_INVALID;
      rxd->adv.ds_incall_stream_id = SIO_NO_STREAM_ID;
      timer_clr( &rxd->sio_esc_seq_cb_item, T_NONE );
      break;

    case SIO_DS_AUTODETECT_MODE:

      rxd->adv.autodetect_device_id = UART_DEVICE_INVALID;
      rxd->adv.autodetect_stream_id = SIO_NO_STREAM_ID;
      timer_clr( &rxd->adv.pausecheck_cb_item, T_NONE );
      break;

    default:
      break; 
  }

  //  If the mode we are changing to is packet or rawdata, then
  //  we want to make sure that this is the only stream doing
  //  that mode.  Ditto for autodetect.

  if(( rxd->sio_current_stream_mode == SIO_DS_RAWDATA_MODE ) ||
     ( rxd->sio_current_stream_mode == SIO_DS_PKT_MODE ))
  {
    //  Check to insure that RAW_DATA mode is not already
    //  active on either UART. If so kill the phone as
    //  this is a pathological error that should never
    //  exit lab testing.

    if( rxd->adv.ds_incall_device_id != UART_DEVICE_INVALID )
    {
      UART_LOG_1( ERROR, "[%d] Multiple UARTS in 'data call' mode", device_id );
    }
  }
  else if( rxd->sio_current_stream_mode == SIO_DS_AUTODETECT_MODE )
  {
    if( rxd->adv.autodetect_device_id != UART_DEVICE_INVALID)
    {
      UART_LOG_1( ERROR, "[%d] Multiple UARTS in Autodetect mode", device_id );
    }
  }

  //  Now do any special setup required by the new mode. 

  switch( open_ptr->stream_mode )
  {
    case SIO_DS_RAWDATA_MODE:

      //  Since connecting a call takes many seconds, we assume
      //  that the first guardtime has already happened.
      rxd->sio_esc_seq_state= SIO_ESC_SEQ_1ST_GUARDTIME;

      // Fall through.

    case SIO_DS_PKT_MODE: /*lint !e825*/

      rxd->adv.ds_incall_device_id = mdd_port->port_data.device_id;
      rxd->adv.ds_incall_stream_id = stream_id;
      break;
   
    case SIO_DS_AUTODETECT_MODE:

      rxd->adv.autodetect_stream_id = stream_id;
      rxd->adv.autodetect_device_id = mdd_port->port_data.device_id;
      siors_mdd_setup_uart_autodetect( mdd_port );
      break;

    case SIO_DM_MODE:
    case SIO_GENERIC_MODE:

      //  No special setup.
      break;

    default:

      UART_LOG_2( ERROR, "[%d] Unknown sio mode %d", device_id, open_ptr->stream_mode );
      break;
  }

DONE:;

  if( tx_locked )
  {
    qurt_mutex_unlock( txd->lock_handle );
  }

  if( rx_locked )
  {
    qurt_mutex_unlock( rxd->lock_handle );
  }

  if( port_locked )
  {
    qurt_mutex_unlock( mdd_port->port_data.lock_handle );
  }

  UART_LOG_1( TRACE, "[%d] -ioctl_change_mode", device_id );
} 

/*===========================================================================

FUNCTION SIORS_MDD_SET_DCD_OUTPUT                           INTERNAL FUNCTION

DESCRIPTION
  This procedure assert/deassert the DCD line

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void 
ioctl_set_dcd_output( sio_mdd_port_type* mdd_port, boolean CDC_value )
{
  UART_LOG_1( ERROR, "[%d] DCD signal not implemented.", mdd_port->port_data.device_id );
}

/*===========================================================================

FUNCTION SIORS_ENABLE_DTR_EVENT                        INTERNAL FUNCTION

DESCRIPTION
  This procedure enable the process of firing a call back when DTR event
  is detected

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void 
ioctl_enable_dtr_event( sio_mdd_port_type* mdd_port, sio_ioctl_param_type* param )
{
  UART_LOG_1( ERROR, "[%d] DTR event not implemented.", mdd_port->port_data.device_id );
}

/*===========================================================================

FUNCTION SIORS_MDD_DISABLE_DTR_EVENT                       INTERNAL FUNCTION

DESCRIPTION
  This procedure disable the process of firing a call back when DTR event
  is detected

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void 
siors_mdd_ioctl_disable_dtr_event( sio_mdd_port_type* mdd_port, sio_ioctl_param_type* param )
{
  UART_LOG_1( ERROR, "[%d] DTR event not implemented.", mdd_port->port_data.device_id );
}

/*===========================================================================

FUNCTION SIORS_MDD_CHECK_DTE_READY_ASSERTED                INTERNAL FUNCTION

DESCRIPTION
  This procedure check to see if the DTE signal is asserted by the host

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void 
ioctl_check_dte_ready_asserted( sio_mdd_port_type *mdd_port, boolean* dte_ready_asserted )
{
  UART_LOG_1( ERROR, "[%d] DTE signal not implemented.", mdd_port->port_data.device_id );
}

/*===========================================================================

FUNCTION 

DESCRIPTION
  
DEPENDENCIES
                                                  
RETURN VALUE
    
SIDE EFFECTS

===========================================================================*/

static void
ioctl_set_record_escape_function( sio_mdd_port_type* mdd_port, 
                                  sio_vv_func_ptr_type record_escape_func )
{
  uint32                           device_id = mdd_port->port_data.device_id;
  sio_mdd_rx_data_type*            rxd = &mdd_port->rx_data;



  qurt_mutex_lock( rxd->lock_handle );
  if( rxd->is_open )
  {
    if( rxd->sio_current_stream_mode != SIO_DS_RAWDATA_MODE )
      UART_LOG_1( TRACE, "[%d] Escape code func ptr recorded in mode != RawData", device_id );

    rxd->sio_escape_code_func_ptr = record_escape_func;
  }
  else
  {
    UART_LOG_1( TRACE, "[%d] This port won't support the recieve path", device_id );
  }
  qurt_mutex_unlock( rxd->lock_handle );
}

/*===========================================================================

FUNCTION 

DESCRIPTION
  
DEPENDENCIES
                                                  
RETURN VALUE
    
SIDE EFFECTS

===========================================================================*/

static void
ioctl_set_record_packet_function( sio_mdd_port_type* mdd_port, 
                                  sio_vv_func_ptr_type record_packet_func )
{
  uint32                           device_id = mdd_port->port_data.device_id;
  sio_mdd_rx_data_type*            rxd = &mdd_port->rx_data;


  qurt_mutex_lock( rxd->lock_handle );

  if( rxd->is_open )
  {
  if( rxd->sio_current_stream_mode != SIO_DS_AUTODETECT_MODE )
    UART_LOG_1( TRACE, "[%d] Packet func. ptr recorded in mode != AutoDetect", device_id );

  rxd->sio_packet_func_ptr = record_packet_func;
  }
  else
  {
    UART_LOG_1( TRACE, "[%d] This port won't support the recieve path", device_id );
  }
  qurt_mutex_unlock( rxd->lock_handle );
}

/*===========================================================================

FUNCTION 

DESCRIPTION
  
DEPENDENCIES
                                                  
RETURN VALUE
    
SIDE EFFECTS

===========================================================================*/

static void
ioctl_set_tx_break_state( sio_mdd_port_type* mdd_port, boolean start )
{
  if( start )
    DalUart_SetBreak( mdd_port->port_data.dal_device_handle );
  else
    DalUart_ClearBreak( mdd_port->port_data.dal_device_handle );
}

/*===========================================================================

FUNCTION 

DESCRIPTION
  
DEPENDENCIES
                                                  
RETURN VALUE
    
SIDE EFFECTS

===========================================================================*/

static void
ioctl_get_current_rts( sio_mdd_port_type* mdd_port, boolean* rts_asserted )
{
  uint32           cts;

  //  Read the current value of RTS/RFR from the PC (DTE) which is
  //  CTS on the MSM (DCE). Return TRUE if RTS is asserted (High).

  DalUart_GetCTS( mdd_port->port_data.dal_device_handle, &cts );
  *rts_asserted = ( cts ? TRUE : FALSE );
}

/*===========================================================================

FUNCTION 

DESCRIPTION
  
DEPENDENCIES
                                                  
RETURN VALUE
    
SIDE EFFECTS

===========================================================================*/

static void
ioctl_set_rx_activity_callback( sio_mdd_port_type* mdd_port, sio_vv_func_ptr_type rx_activity_func ) 
{
  sio_mdd_rx_data_type*            rxd = &mdd_port->rx_data;
  sio_vv_func_ptr_type             on_rx_activity;


  qurt_mutex_lock( rxd->lock_handle );
  if( rxd->is_open )
  {
    mdd_port->rx_data.rx_activity_cb_func_ptr = rx_activity_func;
    rx_activity_func();
  }
  else
  {
    UART_LOG( TRACE, "This port won't support the recieve path");
  }

  qurt_mutex_unlock( rxd->lock_handle );
}

/*===========================================================================

FUNCTION 

DESCRIPTION
  
DEPENDENCIES
                                                  
RETURN VALUE
    
SIDE EFFECTS

===========================================================================*/

static void
ioctl_check_cable_state( sio_mdd_port_type* mdd_port, boolean* rx_line_state )
{
  UartStatusType status;

  if( NULL == rx_line_state )
    return;

  if(mdd_port->port_data.power_down_reason == SIO_PDR_NO_CABLE)
  {
     *rx_line_state = FALSE;
  }
  else
  {
    siors_mdd_on_activity( mdd_port );

    if(!(DalUart_GetStatus(mdd_port->port_data.dal_device_handle, &status)))
    {
      if(status & STATUS_RX_BREAK)
   	  {
   	    *rx_line_state = FALSE;
   	  }
	    else
	    {
        *rx_line_state = TRUE;
	    }
    }
  }
}

/*===========================================================================

FUNCTION 

DESCRIPTION
  
DEPENDENCIES
                                                  
RETURN VALUE
    
SIDE EFFECTS

===========================================================================*/

static void
ioctl_set_inbound_flow( sio_mdd_port_type* mdd_port, boolean enable ) 
{
  int                              device_id = mdd_port->port_data.device_id;
  sio_mdd_rx_data_type*            rxd = &mdd_port->rx_data;



  UART_LOG_1( TRACE, "[%d] +ioctl_set_inbound_flow", device_id );

  qurt_mutex_lock( rxd->lock_handle );
  if( rxd->is_open )
  {
    if( enable )
    {
      mdd_port->rx_data.sio_inbound_flow_enabled = TRUE;
      DalUart_SetRtsControl( mdd_port->port_data.dal_device_handle, RTS_ASSERT );
    }
    else
    {
      mdd_port->rx_data.sio_inbound_flow_enabled = FALSE;
      DalUart_SetRtsControl( mdd_port->port_data.dal_device_handle, RTS_DEASSERT );
    }
  }
  else
  {
    UART_LOG_1( TRACE, "[%d] This port won't support the recieve path", device_id );
  }
  qurt_mutex_unlock( rxd->lock_handle );

  UART_LOG_1( TRACE, "[%d] -ioctl_set_inbound_flow", device_id );
}

/*===========================================================================

FUNCTION IOCTL_SET_INACTIVITY_TIMEOUT

DESCRIPTION
  
DEPENDENCIES
  None
                                                  
RETURN VALUE
  None
    
SIDE EFFECTS

===========================================================================*/

static void
ioctl_set_inactivity_timeout( sio_mdd_port_type* mdd_port, uint32 inactivity_ms ) 
{
  uint32 device_id = mdd_port->port_data.device_id;

  UART_LOG_1( TRACE, "[%d] +ioctl_set_inactivity_timeout", device_id );

  qurt_mutex_lock( mdd_port->port_data.lock_handle );

  mdd_port->port_data.inactivity_ms = inactivity_ms;

  if( mdd_port->port_data.is_clock_on )
  {
     if( mdd_port->port_data.inactivity_ms > 0 )
     {
        /*timer_reg( &mdd_port->port_data.inactivity_timer,
                   siors_mdd_on_inactivity_isr,
                   (timer_cb_data_type) mdd_port,
                   mdd_port->port_data.inactivity_ms, 
                   0 );*/
		timer_set(&mdd_port->port_data.inactivity_timer,
			      mdd_port->port_data.inactivity_ms,0,T_MSEC);
     }
     else 
     {
        timer_clr( &mdd_port->port_data.inactivity_timer, T_NONE );
     }
  }

   qurt_mutex_unlock( mdd_port->port_data.lock_handle );

  UART_LOG_1( TRACE, "[%d] -ioctl_set_inactivity_timeout", device_id );
}

/*===========================================================================

FUNCTION IOCTL_UART_GET_STATUS

DESCRIPTION
  
DEPENDENCIES
                                                  
RETURN VALUE
    
SIDE EFFECTS

===========================================================================*/

static void
ioctl_uart_get_status( sio_mdd_port_type* mdd_port, uart_status_type *uart_status )
{
  if( NULL == uart_status )
    return;

  uart_status->is_clock_on = mdd_port->port_data.is_clock_on;

  siors_mdd_debug(mdd_port);
}

/*===========================================================================

FUNCTION ioctl_set_cxm

DESCRIPTION
  
DEPENDENCIES
                                                  
RETURN VALUE
    
SIDE EFFECTS

===========================================================================*/

/*static void 
ioctl_set_cxm(sio_mdd_port_type* mdd_port, uart_cxm_type set_cxm)
{
  // The Advance type 2 CXM core has a bug that disabling the CXM by putting
  // configuring it for bypass mode does not remove the type2 logic in the 
  // tx and rx path. So when the client calls disable or enable, check the 
  // type 2 callback registration to decide whether we need to disable or 
  // enable them with this call.
  if (set_cxm.enable_cxm)
  {
    DalUart_SetCxm(mdd_port->port_data.dal_device_handle, 
                   set_cxm.enable_cxm, set_cxm.sam);
    if (mdd_port->rx_data.cxm_type2_misalign_cb_func_ptr)
    {
      DalUart_SetCxmType2Mode(mdd_port->port_data.dal_device_handle, TRUE);
    }
  }

  else
  {
    if (mdd_port->rx_data.cxm_type2_misalign_cb_func_ptr)
    {
      DalUart_SetCxmType2Mode(mdd_port->port_data.dal_device_handle, FALSE);
    }
    DalUart_SetCxm(mdd_port->port_data.dal_device_handle, 
                   set_cxm.enable_cxm, set_cxm.sam);
  } 
}
*/	
/*===========================================================================

FUNCTION ioctl_set_cxm_type2

DESCRIPTION
  
DEPENDENCIES
                                                  
RETURN VALUE
    
SIDE EFFECTS

===========================================================================*/
/*static void
ioctl_set_cxm_type2(sio_mdd_port_type* mdd_port, 
                    uart_cxm_type2_misalign_param_type cxm_type2_misalign_param, 
                    boolean type2_enable)
{
  sio_status_type* status = cxm_type2_misalign_param.status_ptr;
  *status = SIO_BADP_S;

  qurt_mutex_lock(& mdd_port->rx_data.lock_handle );
  if( !mdd_port->rx_data.is_open )
  {
    UART_LOG_0(ERROR, "Calling SIO_IOCTL_ENABLE_CXM_TYPE2 when RX stream is not open.");
    qurt_mutex_unlock(& mdd_port->rx_data.lock_handle );
    return;
  }
  qurt_mutex_unlock(& mdd_port->rx_data.lock_handle );

  if ( type2_enable )
  {
    if (NULL == cxm_type2_misalign_param.cxm_type2_misalign_cb)
    {
       UART_LOG_0(ERROR,
                  "Calling SIO_IOCTL_ENABLE_CXM_TYPE2 with NULL misalign interrupt callback");
       return;
    }
    qurt_mutex_lock(& mdd_port->rx_data.lock_handle );
    mdd_port->rx_data.cxm_type2_misalign_cb_func_ptr = cxm_type2_misalign_param.cxm_type2_misalign_cb; 
    qurt_mutex_unlock(& mdd_port->rx_data.lock_handle );

    if(UART_SUCCESS == DalUart_SetCxmType2Mode(mdd_port->port_data.dal_device_handle, TRUE))
    {
       *status = SIO_DONE_S;
    }
  }
  else
  {
    if(UART_SUCCESS == DalUart_SetCxmType2Mode(mdd_port->port_data.dal_device_handle, FALSE))
    {
      *status = SIO_DONE_S;
      qurt_mutex_lock(& mdd_port->rx_data.lock_handle );
      mdd_port->rx_data.cxm_type2_misalign_cb_func_ptr = NULL; 
      qurt_mutex_unlock(& mdd_port->rx_data.lock_handle );
    }
  }
}
*/
/*===========================================================================

FUNCTION ioctl_set_cxm_type2_wci2_msg_event

DESCRIPTION
  
DEPENDENCIES
                                                  
RETURN VALUE
    
SIDE EFFECTS
 
===========================================================================*/

/*static void 
ioctl_set_cxm_type2_wci2_msg_event (sio_mdd_port_type* mdd_port, 
                                    uart_cxm_type2_wci2_msg_param_type  wci2_msg_param, 
                                    boolean wci2_msg_enable)
{
  sio_status_type* status = wci2_msg_param.status_ptr;
  *status = SIO_BADP_S;

  qurt_mutex_lock(& mdd_port->rx_data.lock_handle );
  if( !mdd_port->rx_data.is_open )
  {
    UART_LOG_0(ERROR,
               "Calling SIO_IOCTL_ENABLE_CXM_TYPE2_WCI2_MSG_EVENT when RX stream is not open.");
    qurt_mutex_unlock(& mdd_port->rx_data.lock_handle );
    return;
  }
  qurt_mutex_unlock(& mdd_port->rx_data.lock_handle );

  if (wci2_msg_enable)
  {
    if (NULL == wci2_msg_param.cxm_type2_wci2_msg_cb)
    {
      UART_LOG_0(ERROR,
                 "Calling SIO_IOCTL_ENABLE_CXM_TYPE2_WCI2_MSG_EVENT with NULL message callback");
      return;
    }
    if (NULL == wci2_msg_param.wci2_msg_intr_status_ptr)
    {
      UART_LOG_0(ERROR,
                 "Calling SIO_IOCTL_ENABLE_CXM_TYPE2_WCI2_MSG_EVENT with NULL intr status pointer");
      return;
    }
    if (NULL == wci2_msg_param.wci2_msg_ptr)
    {
      UART_LOG_0(ERROR,
                 "Calling SIO_IOCTL_ENABLE_CXM_TYPE2_WCI2_MSG_EVENT with NULL message pointer");
      return;
    }
    qurt_mutex_lock(& mdd_port->rx_data.lock_handle );
    mdd_port->rx_data.cxm_type2_wci2_msg_cb_func_ptr = wci2_msg_param.cxm_type2_wci2_msg_cb;
    mdd_port->rx_data.cxm_type2_wci2_msg_intr_status_ptr = wci2_msg_param.wci2_msg_intr_status_ptr;
    mdd_port->rx_data.cxm_type2_wci2_msg_ptr = wci2_msg_param.wci2_msg_ptr;
    mdd_port->rx_data.cxm_type2_wci2_msg_length = wci2_msg_param.wci2_msg_length;
    qurt_mutex_unlock(& mdd_port->rx_data.lock_handle );

    *status = SIO_DONE_S;
  }
  else
  {
    qurt_mutex_lock(& mdd_port->rx_data.lock_handle );
    mdd_port->rx_data.cxm_type2_wci2_msg_cb_func_ptr = NULL;
    mdd_port->rx_data.cxm_type2_wci2_msg_intr_status_ptr = NULL;
    mdd_port->rx_data.cxm_type2_wci2_msg_ptr = NULL;
    qurt_mutex_unlock(& mdd_port->rx_data.lock_handle );

    *status = SIO_DONE_S;
  }
}
*/
/*===========================================================================

FUNCTION ioctl_set_cxm_rat_id

DESCRIPTION
  
DEPENDENCIES
                                                  
RETURN VALUE
    
SIDE EFFECTS
 
===========================================================================*/

/*static void 
ioctl_set_cxm_rat_id(sio_mdd_port_type* mdd_port,
                     uart_cxm_rat_id_param_type uart_cxm_rat_id_param, 
                     boolean rat_id_enable)
{
  sio_status_type* status = uart_cxm_rat_id_param.status_ptr;
  *status = SIO_BADP_S;

  qurt_mutex_lock(& mdd_port->rx_data.lock_handle );
  if( !mdd_port->rx_data.is_open )
  {
    UART_LOG_0(ERROR, "Calling SIO_IOCTL_ENABLE_CXM_TYPE2 when RX stream is not open.");
    qurt_mutex_unlock(& mdd_port->rx_data.lock_handle );
    return;
  }
  qurt_mutex_unlock(& mdd_port->rx_data.lock_handle );

  if (rat_id_enable)
  {
    qurt_mutex_lock(& mdd_port->rx_data.lock_handle );
    mdd_port->rx_data.rat_event_cb = uart_cxm_rat_id_param.rat_event_cb;
    qurt_mutex_unlock(& mdd_port->rx_data.lock_handle );
    if(UART_SUCCESS == DalUart_SetCxmRatIdMode(mdd_port->port_data.dal_device_handle, 
                                              uart_cxm_rat_id_param.rat_id_msg_type,
                                              uart_cxm_rat_id_param.enable_rat_msg_to_fifo,
                                              uart_cxm_rat_id_param.enable_tcm_resend,
                                              uart_cxm_rat_id_param.sam,
                                              TRUE))
    {
       *status = SIO_DONE_S;
    }  
  }
  else
  {
    if(UART_SUCCESS == DalUart_SetCxmRatIdMode(mdd_port->port_data.dal_device_handle, 
                                              0,
                                              FALSE,
                                              FALSE,
                                              0,
                                              FALSE))
    {
      *status = SIO_DONE_S;
      qurt_mutex_lock(& mdd_port->rx_data.lock_handle );
      mdd_port->rx_data.rat_event_cb = NULL; 
      qurt_mutex_unlock(& mdd_port->rx_data.lock_handle );
    }
  }
}*/
/*===========================================================================

FUNCTION SIORS_MDD_API_IOCTL                        EXTERNALIZED FUNCTION

DESCRIPTION
  Allows control of open stream, given that the device is an RS232 serial
  device.    
  
DEPENDENCIES
  None
                                                  
RETURN VALUE
  None
    
SIDE EFFECTS

===========================================================================*/

void 
siors_mdd_api_ioctl( sio_stream_id_type    stream_id,
                     sio_port_id_type      port_id,
                     sio_ioctl_cmd_type    cmd,   
                     sio_ioctl_param_type* param )
{
  sio_mdd_port_type*       mdd_port = NULL;            
  uart_device_id_type      device_id;
  boolean                  power_down_on_exit = FALSE;
  boolean                  port_locked = FALSE;


  //  Validate param.
  
  switch( cmd )
  {
    case SIO_IOCTL_CHANGE_BAUD_RATE:
    case SIO_IOCTL_CHANGE_BAUD_NOW:
    case SIO_IOCTL_GET_CURRENT_BAUD_RATE:		
    case SIO_IOCTL_CHANGE_MODE:
    case SIO_IOCTL_ENABLE_DTR_EVENT:
    case SIO_IOCTL_DISABLE_DTR_EVENT:
    case SIO_IOCTL_DTE_READY_ASSERTED:
    case SIO_IOCTL_SET_FLOW_CTL:
    case SIO_IOCTL_GET_FLOW_CTL:
    case SIO_IOCTL_RECORD_ESC_FUNC_PTR:
    case SIO_IOCTL_RECORD_PKT_FUNC_PTR:
    case SIO_IOCTL_FLUSH_TX:
    case SIO_IOCTL_GET_CURRENT_RTS:
    case SIO_IOCTL_RX_HIGH_EVENT:
    case SIO_IOCTL_CHECK_RX_STATE:
    case SIO_IOCTL_INACTIVITY_TIMER:
    case SIO_IOCTL_GET_STATUS:
    case SIO_IOCTL_SET_LOOPBACK_MODE:
    case SIO_IOCTL_SET_CXM:
    case SIO_IOCTL_GET_CXM_TX:
    case SIO_IOCTL_PURGE_TX:
    case SIO_IOCTL_ENABLE_CXM_TYPE2:
    case SIO_IOCTL_DISABLE_CXM_TYPE2:
    case SIO_IOCTL_ENABLE_CXM_TYPE2_WCI2_MSG_EVENT:
    case SIO_IOCTL_DISABLE_CXM_TYPE2_WCI2_MSG_EVENT:
    case SIO_IOCTL_SET_TYPE2_WCI2_MSG_INTR_MASK:
    case SIO_IOCTL_ENABLE_RAT_ID_CXM:
    case SIO_IOCTL_SET_RAT_EVENTS:

      if( NULL == param )
      {
        UART_LOG( ERROR, "Input parameter param is NULL.");
        return;
      }
      break;

    default:
      break;
  }
  
  //  Validate port_id.  Convert to device_id, mdd_port.

  device_id = uart_sio_port_id_to_device_id( port_id );
  if( UART_DEVICE_INVALID == device_id )
  {
    UART_LOG_1( ERROR, "UART port id %d is invalid", port_id );
    return;
  }

  UART_LOG_1( MEDIUM, "[%d] +siors_mdd_api_ioctl", device_id );

  mdd_port = &siors_mdd_ports[device_id];

  //  Check that the port is in a good state.

  if( !mdd_port->stream_data.is_open )
    goto DONE;

  // Prevent certain ioctls from calling the on_activity function 
  switch (cmd)
  {
     case SIO_IOCTL_WAKEUP:          // waking up here is redundant
     case SIO_IOCTL_POWERDOWN:       // waking up here is unnecessary
     case SIO_IOCTL_GET_STATUS:      // waking up here may change UART state 
                                     // (this IOCTL is intended to be passive)
        break;
     
     default:
        // All other IOCTLs count as activity and may reset the inactivity timer
        siors_mdd_on_activity( mdd_port );
        break;
   }

  switch (cmd)
  {
    case SIO_IOCTL_CHANGE_BAUD_RATE:
    case SIO_IOCTL_CHANGE_BAUD_NOW:
    case SIO_IOCTL_INBOUND_FLOW_ENABLE:
    case SIO_IOCTL_INBOUND_FLOW_DISABLE:
    case SIO_IOCTL_SET_FLOW_CTL:
    case SIO_IOCTL_TX_START_BREAK:
    case SIO_IOCTL_TX_END_BREAK:
    case SIO_IOCTL_GET_CURRENT_RTS:
    case SIO_IOCTL_SET_LOOPBACK_MODE:
    case SIO_IOCTL_SET_CXM:
    case SIO_IOCTL_GET_CXM_TX:
    case SIO_IOCTL_CXM_TX_DIRECT_CHAR:
    case SIO_IOCTL_PURGE_TX:
    case SIO_IOCTL_ENABLE_CXM_TYPE2:
    case SIO_IOCTL_DISABLE_CXM_TYPE2:
    case SIO_IOCTL_SET_TYPE2_WCI2_MSG_INTR_MASK:
    case SIO_IOCTL_ENABLE_RAT_ID_CXM:
    case SIO_IOCTL_DISABLE_RAT_ID_CXM:
    case SIO_IOCTL_SET_RAT_EVENTS:

       qurt_mutex_lock( mdd_port->port_data.lock_handle );
       port_locked = TRUE;
       if( mdd_port->port_data.is_clock_on )
       {
         UART_LOG_1( TRACE, "[%d] Clock is already on.", device_id );
       }
       else if ( SIO_PDR_NO_CABLE == mdd_port->port_data.power_down_reason )
       {
          UART_LOG_1( TRACE, "[%d] Clock is off. Waking up to process the IOCTL.", device_id );
          siors_mdd_wakeup(mdd_port);
          power_down_on_exit = TRUE;
       }
       else
       {
          UART_LOG_1(TRACE,"[%d] IOCTL called on an explicitly powered down port. Skipping.",
                             device_id );
          goto DONE;
       }
       break;
    default:
      break;
  }
  //  Execute command.

  switch( cmd )
  {
    case SIO_IOCTL_CHANGE_BAUD_RATE:

      ioctl_schedule_baudrate_change( mdd_port, (sio_bitrate_type) param->bitrate );
      break;

    case SIO_IOCTL_CHANGE_BAUD_NOW:

      ioctl_baudrate_change_now( mdd_port, (sio_bitrate_type) param->bitrate );
      break;
	  
    case SIO_IOCTL_GET_CURRENT_BAUD_RATE:
		
	  ioctl_uart_get_baudrate(mdd_port, (sio_bitrate_type *) param->cur_bitrate );
	  break;
	

    case SIO_IOCTL_CHANGE_MODE:

      ioctl_change_mode( stream_id, mdd_port, (sio_open_type*) param->open_ptr );
      break;
    
    case SIO_IOCTL_CD_ASSERT:

      ioctl_set_dcd_output( mdd_port, TRUE );
      break; 

    case SIO_IOCTL_CD_DEASSERT:

      ioctl_set_dcd_output( mdd_port, FALSE );
      break; 

    case SIO_IOCTL_ENABLE_DTR_EVENT:

      ioctl_enable_dtr_event( mdd_port, param );
      break; 

    case SIO_IOCTL_DISABLE_DTR_EVENT:

      siors_mdd_ioctl_disable_dtr_event( mdd_port, param );
      break; 

    case SIO_IOCTL_INBOUND_FLOW_ENABLE:

      ioctl_set_inbound_flow( mdd_port, TRUE ); 
      break;

    case SIO_IOCTL_INBOUND_FLOW_DISABLE:

      ioctl_set_inbound_flow( mdd_port, FALSE ); 
      break; 

    case SIO_IOCTL_DTE_READY_ASSERTED:

      ioctl_check_dte_ready_asserted( mdd_port, param->dte_ready_asserted );
      break;

    case SIO_IOCTL_SET_FLOW_CTL:

      ioctl_set_flow_ctl( mdd_port, param->flow_ctl.tx_flow, param->flow_ctl.rx_flow );
      break; 

    case SIO_IOCTL_GET_FLOW_CTL:

      ioctl_get_flow_ctl( mdd_port, &param->flow_ctl.rx_flow, &param->flow_ctl.tx_flow );
      break; 

    case SIO_IOCTL_ENABLE_AUTODETECT:

      qurt_mutex_lock( mdd_port->rx_data.lock_handle );
      siors_mdd_setup_uart_autodetect( mdd_port );
      qurt_mutex_unlock( mdd_port->rx_data.lock_handle );
      break; 

    case SIO_IOCTL_RECORD_ESC_FUNC_PTR:

      ioctl_set_record_escape_function( mdd_port, param->record_escape_func_ptr );
      break; 

    case SIO_IOCTL_RECORD_PKT_FUNC_PTR:

      ioctl_set_record_packet_function( mdd_port, param->record_pkt_func_ptr );
      break; 

    case SIO_IOCTL_FLUSH_TX:

      siors_mdd_api_flush_tx( stream_id, port_id, param->record_flush_func_ptr );
      break; 

    case SIO_IOCTL_TX_START_BREAK:

      ioctl_set_tx_break_state( mdd_port, TRUE );
      break;

    case SIO_IOCTL_TX_END_BREAK:

      ioctl_set_tx_break_state( mdd_port, FALSE );
      break;

    case SIO_IOCTL_GET_CURRENT_RTS:

      ioctl_get_current_rts( mdd_port, param->rts_asserted );
      break; 

    case SIO_IOCTL_POWERDOWN:

      if( NULL != param)
      {
        *(sio_status_type*)(param->status_ptr) = siors_mdd_powerdown( mdd_port, SIO_PDR_EXPLICIT );
      }
      else
      {
        siors_mdd_powerdown( mdd_port, SIO_PDR_EXPLICIT );
      }
      break;

    case SIO_IOCTL_WAKEUP:

      siors_mdd_wakeup( mdd_port );
      break;

    case SIO_IOCTL_INACTIVITY_TIMER:

      ioctl_set_inactivity_timeout( mdd_port, param->inactivity_ms );
      break;

    case SIO_IOCTL_GET_STATUS:

      ioctl_uart_get_status( mdd_port, param->status_ptr);
      break;

    case SIO_IOCTL_SET_LOOPBACK_MODE:
      DalUart_LoopbackSet( mdd_port->port_data.dal_device_handle, 
                           param->enable_loopback_mode );
      break;

    case SIO_IOCTL_RX_HIGH_EVENT:

      ioctl_set_rx_activity_callback( mdd_port, param->rx_high_event_func_ptr );
      break;

    case SIO_IOCTL_CHECK_RX_STATE:

      ioctl_check_cable_state( mdd_port, param->rx_line_state );
      break;

    case SIO_IOCTL_RI_ASSERT:
    case SIO_IOCTL_RI_DEASSERT:

      UART_LOG_1( ERROR, "[%d] RI not supported", device_id );
      break;

    case SIO_IOCTL_DSR_ASSERT:
    case SIO_IOCTL_DSR_DEASSERT:

      UART_LOG_1( ERROR, "[%d] DTR not supported", device_id );
      break;

    case SIO_IOCTL_BEGIN_FAX_IMAGE:

      UART_LOG_1( ERROR, "[%d] No FAX image support", device_id );  
      break; 

    case SIO_IOCTL_GET_PACKET_SIZE:
    case SIO_IOCTL_SET_PACKET_SIZE:

      UART_LOG_1( ERROR, "[%d] Ignoring IOCTL to get/set packet size", device_id );
      break;

    case SIO_IOCTL_SET_CXM:
      //ioctl_set_cxm(mdd_port, param->set_cxm);
      break;

    case SIO_IOCTL_GET_CXM_TX:
      //DalUart_GetCxmTxSticky(mdd_port->port_data.dal_device_handle,
      //    &param->cxm_tx.tx_sticky, param->cxm_tx.clear);
      break;

    case SIO_IOCTL_CXM_TX_DIRECT_CHAR:
      /*param->cxm_tx_direct_char.tx_direct_char_status = SIO_BADP_S;
      if( UART_SUCCESS == DalUart_CxmTxDirectChar(mdd_port->port_data.dal_device_handle, 
                                                 param->cxm_tx_direct_char.character) )
      {
        param->cxm_tx_direct_char.tx_direct_char_status = SIO_DONE_S;
      }*/
      break;

    case SIO_IOCTL_CANCEL_FLUSH_TX:
      ioctl_cancel_flush( mdd_port );
      break;

    case SIO_IOCTL_PURGE_TX:

      *(sio_status_type*)param->status_ptr = siors_mdd_api_purge_tx( stream_id, port_id );
      break;

    case SIO_IOCTL_ENABLE_CXM_TYPE2:
      //ioctl_set_cxm_type2(mdd_port, param->uart_cxm_type2_misalign_param, TRUE);
      break;

    case SIO_IOCTL_DISABLE_CXM_TYPE2:
      //ioctl_set_cxm_type2(mdd_port, param->uart_cxm_type2_misalign_param, FALSE);
      break;

    case SIO_IOCTL_ENABLE_CXM_TYPE2_WCI2_MSG_EVENT:
      /*ioctl_set_cxm_type2_wci2_msg_event(mdd_port, param->uart_cxm_type2_wci2_msg_param,
                                         TRUE);*/
      break;

    case SIO_IOCTL_DISABLE_CXM_TYPE2_WCI2_MSG_EVENT:
      //ioctl_set_cxm_type2_wci2_msg_event(mdd_port, param->uart_cxm_type2_wci2_msg_param, FALSE);
      break;

    case SIO_IOCTL_SET_TYPE2_WCI2_MSG_INTR_MASK:
      //DalUart_SetCxmMsgIntrMask(mdd_port->port_data.dal_device_handle, 
                                //param->cxm_type2_wci2_msg_intr_mask);
      break;

    case SIO_IOCTL_ENABLE_RAT_ID_CXM:
      //ioctl_set_cxm_rat_id(mdd_port, param->uart_cxm_rat_id_param, TRUE);
      break;

    case SIO_IOCTL_DISABLE_RAT_ID_CXM:
      //ioctl_set_cxm_rat_id(mdd_port, param->uart_cxm_rat_id_param, FALSE);
      break;

    case SIO_IOCTL_SET_RAT_EVENTS:

    default:

      UART_LOG_2( ERROR, "[%d] Invalid SIO IOCTL command %d", device_id, cmd );
      break;
  }

DONE:;
  // If the UART is powered up to process the IOCTL, power it down on exit.
  // We will wakeup only for Break, so the reason is always SIO_PDR_NO_CABLE
  if (power_down_on_exit)
  {
     siors_mdd_powerdown( mdd_port, SIO_PDR_NO_CABLE );
  }

  if (port_locked) 
  {
     qurt_mutex_unlock( mdd_port->port_data.lock_handle );
  }

  UART_LOG_1( MEDIUM, "[%d] -siors_mdd_api_ioctl", device_id );
}

//-----------------------------------------------------------------------------------
//  End of file
//-----------------------------------------------------------------------------------

#endif //} 


