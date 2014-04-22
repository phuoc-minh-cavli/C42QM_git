/*==============================================================================

Serial Functions (Modem/DIAG/NMEA) Adaptation Layer for High Speed USB
"Base Class" Implementation

GENERAL DESCRIPTION
Contains the implementation of the API for the serial functions'
adapt. layer "Base Class"

EXTERNALIZED FUNCTIONS

hsu_al_ser_base_init
hsu_al_ser_base_open
hsu_al_ser_base_close
hsu_al_ser_base_transmit
hsu_al_ser_base_flush_tx
hsu_al_ser_base_ioctl

INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_al_ser_base_init must be called for each function instance, 
prior to performing any other operations on the instance.

Copyright (c) 2006 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_ser_base.c#1 $

when      who     what, where, why
--------  ---     ------------------------------------------------------
07/30/06  esh     First revision of file
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h" /* For FEATURE_HSU_TEST to work */
#include "hsu_common_int.h"
#include "err.h"

#include "hsu_al_ser_base_int.h"
#include "hsu_al_ser_base_i.h"

#include "hsu_al_ser.h"
#include "hsu_al_ser_i.h"

#include "hsu_al_dsm_utils.h"

#include "port_int.h"

#include "hsu_al_task.h"

#include "hsu_cdc_fd_api.h"

#include "sio_wrapper.h" /* for SIO_PAYLOAD */

/* For ESUSPENDED */
#include "jerrno.h"
#include "jtypes_int.h"
#include "dsm_pool.h"
#include "jusb_core.h"
#include "cdc_fd.h"
#include "hsu_bus_mode_notifier.h"

#include "hsu_log.h"
#include "hsu_al_task_int.h"
#include "hsu_conf_sel_i.h"

/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/

#define USB_MDM_RX_THROTTLE_TIMEOUT_VAL_MS     (5)
  /*Timeout value in ms for throttling of UL buffer queuing to the HW */
#define USB_MDM_DUN_MIN_SMALL_PKT_CNT          (5)
 /* Minimim number of consecutive small packets to be received in the UL direction for throttling*/
#define USB_MDM_DUN_RX_MIN_PKT_SIZE            (64)
 /* Minimum size of the packets received in the UL direction for throttling */

#define AL_SER_BASE_MAX_IO_ABORTS_BEFORE_CLOSE (2)
  /* Can issue no more than one abort read and one abort write */

 #define AL_SER_BASE_FLUSH_TX_TIMEOUT_MS       (200)
  /* Timeout to wait for flush_tx_cb to be invoked in msec 
     This value is an estimation of the time that will take the AL to
     send all the data from a full tx_wmq:
     The DNE value of DIAG tx_wmq is 400K
     Out transfer rate is 2MBps
     => it will take us ~200 msec to send all data from a full tx_wmq
   */
#define AL_SER_BASE_FLUSH_TX_INTERVAL_MS  20
/* Interval of time to wait after flush operation.
   The timer will be set X times x= AL_SER_BASE_FLUSH_TX_TIMEOUT_MS/AL_SER_BASE_FLUSH_TX_INTERVAL_MS
   before declaring a timeout- and aborting the flush operation.*/

/* Rounds down to a number to nearest 512.  i.e 1023 -> 512 */
#define HSU_AL_SER_BASE_ROUND_DOWN_TO_512(num) ((num) & ~511)

#ifdef FEATURE_HS_USB_SER4_PORT
	/* single data buffer size is 16KB */
	#define USB_SER4_MAX_WRITE_BUF_COUNT 64
	#define HSU_AL_SER4_DATA_BUFFER_SIZE 0x4000

#define USB_SNPS_SER4_MAX_WRITE_BUF_COUNT 1
#define HSU_AL_SNPS_SER4_DATA_BUFFER_SIZE 0x1000000  // 16MB

//#define USB_SNPS_SER4_MAX_WRITE_BUF_COUNT 8
//#define HSU_AL_SNPS_SER4_DATA_BUFFER_SIZE 0x20000

	
	/* max cdc write size -- 1MB */
	#define USB_SER4_MAX_CDC_WRITE_MULTIPLE_SIZE 0x00100000
#endif /* FEATURE_HS_USB_SER4_PORT */

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
Variables
==============================================================================*/
#ifdef FEATURE_HS_USB_SER4_PORT
static uint32 mem_read_counter=0;
#endif /* FEATURE_HS_USB_SER4_PORT */


static void al_ser_base_internal_abort_read
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
);

static void al_ser_base_internal_abort_write
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
);

static void al_ser_base_end_close
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
);
 
static boolean al_ser_base_handle_closing
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
);

/* APIs to send TX buffers */
static jresult_t al_ser_base_tx_send_buffer
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr,
  dsm_item_type        *tx_ptr
);

static void al_ser_base_tx_send_all_data
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
);

static void al_ser_base_tx_complete_timer_cb
(
  unsigned long         arg
);

static void al_ser_base_tx_check_complete
(
  void                 *args
);

/* API to handle Max Pending Buffers in HW Queue
static boolean al_ser_base_tx_handle_max_pending
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
); */
/* API to write TX buffers which is caled from Send */
static jresult_t al_ser_base_tx_write
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr,
  dsm_item_type        *tx_ptr,
  uint8                *buf_ptr,
  uint32                buf_size,
  boolean               contiguous_buf_used
);

/* Tx data flush timer callback */
static void al_ser_base_flush_tx_timer_cb
(
  unsigned long         ser_if_data_as_cb_arg
);

static void al_ser_base_clear_dsm_items_from_ser_data
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
);


static void al_ser_base_rx_done_with_flow_disabled
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr,
  const cdc_iostatus_t *status,
  const juint32_t      *bytes_transferred,
  juint32_t             req_num
);

static void al_ser_base_rx_done_forward_to_app
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr,
  dsm_item_type        *rx_dsm_chain_ptr
);

static void al_ser_base_rx_done
(
  cdc_iostatus_t       *status,
  void                **buffers,
  juint32_t            *bytes_transferred,
  juint32_t             req_num,
  cdc_appctx_t          ctx,
  boolean               due_to_flow_enabled
);

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/
/*===========================================================================
FUNCTION hsu_al_ser_base_init()
===========================================================================*/
void hsu_al_ser_base_init
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
)
{
  /* Timer must be created first before _i_uninit() calls timer_cancel
  ** unconditionally */
  ser_if_data_ptr->tx_info.flush_tx_timer = hsu_timer_init(
    HSU_TIMER_CID__SER_FLUSH_TX,
    HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
    al_ser_base_flush_tx_timer_cb,
    (timer_cb_data_type)ser_if_data_ptr);

  (void)hsu_al_ser_base_i_uninit(ser_if_data_ptr);

  ser_if_data_ptr->tx_info.is_write_abort_issued = FALSE;

  ser_if_data_ptr->tx_info.flush_tx_duration = 0;

  ser_if_data_ptr->stream_id = SIO_NO_STREAM_ID;

  ser_if_data_ptr->tx_info.flush_tx_in_progress = FALSE;

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  ser_if_data_ptr->tx_info.recover_dropped_packet = TRUE;
#else  /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
  ser_if_data_ptr->tx_info.recover_dropped_packet = FALSE;
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
  

  ser_if_data_ptr->tx_info.drop_packet_on_suspend = FALSE;

  ser_if_data_ptr->debug_mode = FALSE;
  
  ser_if_data_ptr->uses_dsm_app_ptr = FALSE;
}

/*===========================================================================
FUNCTION hsu_al_ser_base_open()
Should be called with Giant mutex locked
===========================================================================*/
boolean hsu_al_ser_base_open
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  const sio_open_type* open_ptr,
  dsm_mempool_id_enum_type pool_id,
  juint8_t              rx_buff_cnt,
  uint32                rx_timout,
  uint32                tx_max_pending_chain_cnt,
  uint32                tx_timeout,
  uint32                tx_complete_timer_val,
  void                 *arg_cb
)
{
  if (!hsu_os_is_giant_locked_by_curr_task())
  {
    HSU_ERR_FATAL("hsu_al_ser_base_open: "
      "Should be called with Giant mutex locked!", 0, 0, 0);
    return FALSE;
  }
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  HSU_ASSERT(open_ptr != NULL);

  /* When dynamic configurations are supported, it is possible that an app 
  requested to open a HSU port, but without the HSU stack initializing it.
  To verify this, instance_of_fd_used must be checked to be non NULL.
  It is assumed that hsu_al_ser_base_init was called previously for this instance. */
  if (ser_if_data_ptr->instance_of_fd_used == NULL)
  {
    HSU_MSG_ERROR_1("hsu_al_ser_base_open: instance for port_id=%u was not"
                  " initialized by stack!", open_ptr->port_id);
    return FALSE;
  }

  hsu_al_ser_base_i_open_set_data(
    ser_if_data_ptr, 
    open_ptr->port_id,
    open_ptr->stream_id,
    open_ptr->stream_mode,
    open_ptr->tx_queue, 
    open_ptr->rx_queue, 
    open_ptr->rx_func_ptr, 
    pool_id,
    rx_buff_cnt,
    rx_timout,
    tx_max_pending_chain_cnt,
    tx_timeout,
    tx_complete_timer_val,
    open_ptr->rx_flow, 
    open_ptr->tx_flow,
    arg_cb);

  /* Register to DSM pool level change notification */
  hsu_al_dsm_utils_init(pool_id);

  if (ser_if_data_ptr->instance_enabled_by_cdc_fd == TRUE)
  {
    /* Reaching this point means enable callback was already called */
    
    /* Now, even if a disable cb is invoked, al_ser_base_alloc_buffer_and_setup_read 
    checks instance_enabled_by_cdc_fd (without interference by FD) prior to calling 
    cdc_read */

    /* There can't be a call to disable cb and then enable cb before 
    the call to hsu_cdc_fd_api_cdc_read inside al_ser_base_alloc_buffer_and_setup_read 
    because the Giant is locked*/

    /* Setup a new read: */    
    hsu_al_ser_base_i_rx_setup_read(ser_if_data_ptr);
  }
  /* Reaching this point means enable callback was not yet called, 
  but there is no problem it would be called from now on */

  HSU_MSG_LOW("hsu_al_ser_base_open: Operation Successful.");
  return TRUE;
}

/*===========================================================================
FUNCTION hsu_al_ser_base_close()
Called with the Giant locked
===========================================================================*/
void hsu_al_ser_base_close
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  hsu_al_ser_base_op_complete_cb close_cb,
  boolean abort_write_op_immediately
)
{
  HSU_ASSERT(ser_if_data_ptr != NULL);

  if (!hsu_os_is_giant_locked_by_curr_task())
  {
    HSU_ERR_FATAL("hsu_al_ser_base_close: Should be called "
      " with Giant mutex locked!", 0, 0, 0);
    return;
  }

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);
  
  /* In case flow control is still disabled, release the allocated buffer */
  if (ser_if_data_ptr->rx_info.num_buf_waiting != 0)
  {
    dsm_free_packet(&ser_if_data_ptr->rx_info.dsm_chain_ptr);
  }

  /* Save close callback (invoked when either all I/O operations were aborted, 
  or when Tx was flushed and after which, Rx was aborted */
  ser_if_data_ptr->close_complete_cb = close_cb;


  HSU_ASSERT(ser_if_data_ptr->num_of_aborts_issued_prior_to_close == 0);

  /* First check if interface isn't in a middle of a read operation (can only 
  be changed to TRUE by an enabled operation) AND is currently not Tx'ing 
  (note that hsu_al_ser_transmit is app context) */

  if ((ser_if_data_ptr->rx_info.read_oper_is_in_progress == FALSE) &&
      (ser_if_data_ptr->tx_info.is_write_in_progress == FALSE))
  {
    HSU_ULOG_ENUM_1(HIGH_MSG, DATA_LOG, "hsu_al_ser_base_close, no_rw_inprog, cb 0x%8X", ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2, (uint32)close_cb);

    /* No more aborts - call close immediately */
    hsu_al_ser_base_i_internal_close(ser_if_data_ptr);

    /* TX cleanup */
    /* hsu_al_ser_base_close will invoke abort_write that will clean any pending
     * buffers in stack. hsu_al_ser_base_i_set_params_in_open_close will clean
     * remaining_chain_ptr and tx_wmq_ptr 
     * (called from hsu_al_ser_base_i_internal_close) */

    HSU_MSG_HIGH_1("*_base_close: exit, abort_write_immediate=%u", 
      abort_write_op_immediately);

    if (ser_if_data_ptr->tx_info.is_tx_complete_timer_scheduled)
    {
      hsu_timer_cancel(&ser_if_data_ptr->tx_info.tx_complete_timer);
      ser_if_data_ptr->tx_info.is_tx_complete_timer_scheduled = FALSE;
    }  

    return;
  }
  /* Else: abort read AND/OR abort write
  (when it is done - complete close) */

  /* Note: The update of ser_if_data_ptr->num_of_aborts_issued_prior_to_close 
  is done here, prior to calling al_ser_base_internal_abort_*, 
  instead of INSIDE the implementation of al_ser_base_internal_abort_*. 
  This is due to the following scenario:

  1. Issued al_ser_base_internal_abort_read, (and no scatter DMOV in progress) 
  2. Inside there, num_of_aborts_issued_prior_to_close is set to 1
  3. cdc_abort_read called, but it is synchronous, so the read completion callback
  (with CANCELLED) is invoked, setting num_of_aborts_issued_prior_to_close to 0
  4. This causes a call to hsu_al_ser_base_i_internal_close prior to abort write.
  */

   /* Check if client isn't in a middle of a read operation (can only 
  be changed to TRUE by an enabled operation, by invoking 
  hsu_al_ser_base_i_rx_setup_read) */
  if (ser_if_data_ptr->rx_info.read_oper_is_in_progress == TRUE)
  {
    if (abort_write_op_immediately&&
        (ser_if_data_ptr->tx_info.is_write_in_progress == TRUE))
    {
      /* num_of_aborts_issued_prior_to_close is immediately set to max value, 
      so it is an atomic operation. */
      ser_if_data_ptr->num_of_aborts_issued_prior_to_close = 
        AL_SER_BASE_MAX_IO_ABORTS_BEFORE_CLOSE;

      al_ser_base_internal_abort_write(ser_if_data_ptr);
    }
    else /* only abort read is needed: */
    {
      ++ser_if_data_ptr->num_of_aborts_issued_prior_to_close;
    }

    al_ser_base_internal_abort_read(ser_if_data_ptr);
  }
  else if (abort_write_op_immediately &&
            (ser_if_data_ptr->tx_info.is_write_in_progress == TRUE))
  {
    /* Only abort write is needed: */
    ++ser_if_data_ptr->num_of_aborts_issued_prior_to_close;

    al_ser_base_internal_abort_write(ser_if_data_ptr);
  }
  al_ser_base_end_close(ser_if_data_ptr);

  /* TX cleanup */
  /* hsu_al_ser_base_close will invoke abort_write that will clean any pending
   * buffers in stack. hsu_al_ser_base_i_set_params_in_open_close will clean
   * remaining_chain_ptr and tx_wmq_ptr 
   * (called from hsu_al_ser_base_i_internal_close) */
  
  hsu_timer_cancel(&ser_if_data_ptr->tx_info.tx_complete_timer);
  ser_if_data_ptr->tx_info.is_tx_complete_timer_scheduled = FALSE;

  HSU_MSG_HIGH_1("*_base_close: exit, abort_write_immediate=%u", 
    abort_write_op_immediately);

}

/*===========================================================================
FUNCTION hsu_al_base_transmit()
Should be called with Giant mutex locked
===========================================================================*/
void hsu_al_ser_base_transmit
( 
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  dsm_item_type *tx_ptr
)
{
  if (!hsu_os_is_giant_locked_by_curr_task())
  {
    HSU_ERR_FATAL("base_transmit: Should be called with Giant mutex locked!", 
      0, 0, 0);
    return;
  }

  /* Instance not enabled, discard packet */
  if (ser_if_data_ptr->instance_enabled_by_cdc_fd == FALSE)
  {
    HSU_SER_STAT_INC(ser_if_data_ptr->statistics.tx_dropped_packet_cnt,1);
    
    if (ser_if_data_ptr->tx_info.recover_dropped_packet)
    {
      dsm_enqueue(ser_if_data_ptr->tx_wmq_ptr, &tx_ptr);
      hsu_al_ser_i_base_tx_set_handle_complete_timer(ser_if_data_ptr);        
    }
    else
    {
      dsm_free_packet(&tx_ptr);
    }
    return;
  }   

  HSU_SER_STAT_INC(ser_if_data_ptr->statistics.tx_total_transmit_cnt,1);

  /* Check HW Queue has reached the Max Pending size */     
  if (ser_if_data_ptr->tx_info.pending_chain_cnt >= 
      ser_if_data_ptr->tx_info.tx_max_pending_chain_cnt)
  {
    dsm_enqueue(ser_if_data_ptr->tx_wmq_ptr, &tx_ptr);
    hsu_al_ser_i_base_tx_set_handle_complete_timer(ser_if_data_ptr);        
    return;
  }

  /* If Remaining Chain exists or Tx Water Mark Queue (WMQ) is not empty,
   * ( enqueue current item to TxWMQ, send all data in remaining chain or
   * TxWMQ */
  if (ser_if_data_ptr->tx_info.remaining_chain_ptr != NULL)
  {
    dsm_enqueue(ser_if_data_ptr->tx_wmq_ptr, &tx_ptr);
    al_ser_base_tx_send_all_data(ser_if_data_ptr);  
    return;
  }
  
  /* Send the Tx item */
  al_ser_base_tx_send_buffer(ser_if_data_ptr, tx_ptr);

  /* Check if HW Queue has reached Max Pending */
  if (ser_if_data_ptr->tx_info.pending_chain_cnt <
      ser_if_data_ptr->tx_info.tx_max_pending_chain_cnt)
  {
    /* Send the remaining or queued if any */
    if ((ser_if_data_ptr->tx_info.remaining_chain_ptr != NULL) ||
        (!dsm_is_wm_empty(ser_if_data_ptr->tx_wmq_ptr)))
    {
      al_ser_base_tx_send_all_data(ser_if_data_ptr);
    }
  }
  
  hsu_al_ser_i_base_tx_set_handle_complete_timer(ser_if_data_ptr);

}

/*===========================================================================
FUNCTION hsu_al_ser_base_flush_tx()
===========================================================================*/
void hsu_al_ser_base_flush_tx
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  hsu_al_ser_base_op_complete_cb flush_tx_cb
)
{
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);  

  /* Grab giant mutex */
  jsafe_enter();
  /* Check if there was a "write" that didn't send a tx_done yet.
   * It will send it if needed */
  cdc_handle_tx_completion(ser_if_data_ptr->instance_of_fd_used);
  /* Release giant mutex */
  jsafe_leave();

  ser_if_data_ptr->tx_info.flush_tx_complete_cb = flush_tx_cb;

  /* If  flush_tx_in_progress it means that the application already called
  _flush_tx but the flush_tx_cb() wasn't called  yet.  */
  if (ser_if_data_ptr->tx_info.flush_tx_in_progress == TRUE)
  {
    HSU_ULOG_ENUM_1(HIGH_MSG, DATA_LOG, "hsu_al_ser_base_flush_tx, already_called, cb 0x%8X", ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2, (uint32)flush_tx_cb);
    return;
  }

  ser_if_data_ptr->tx_info.flush_tx_in_progress = TRUE;

  if (ser_if_data_ptr->tx_info.is_write_in_progress == FALSE)
  {
    /* No more data to send, call Flush Tx callback: */
    HSU_MSG_LOW("*_flush_tx: Operation Successful (no more data).");

    if (ser_if_data_ptr->tx_info.flush_tx_complete_cb != NULL)
    {
      HSU_ULOG_ENUM_1(HIGH_MSG, DATA_LOG, "hsu_al_ser_base_flush_tx, cb_called, cb 0x%8X", ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2, (uint32)flush_tx_cb);
      ser_if_data_ptr->tx_info.flush_tx_complete_cb(ser_if_data_ptr->arg_cb);
      ser_if_data_ptr->tx_info.flush_tx_complete_cb = NULL;
    }
    ser_if_data_ptr->tx_info.flush_tx_in_progress = FALSE;
    return;
  }
  /* else: ser_if_data_ptr->tx_info.is_write_in_progress == TRUE */

  /* Arm the flush_tx timer. Need to do so before calling
     al_ser_base_cancel_aggregation_and_flush, to avoid arming the timer after
     all TX had already finished */
  ser_if_data_ptr->tx_info.flush_tx_duration = 0;/* clear the flush duration! cleared only here*/
  hsu_timer_set(
    &(ser_if_data_ptr->tx_info.flush_tx_timer),
    AL_SER_BASE_FLUSH_TX_INTERVAL_MS,
    T_MSEC);

  HSU_ULOG_ENUM_1(HIGH_MSG, DATA_LOG, "hsu_al_ser_base_flush_tx, start_timer, timeout %u", ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2, AL_SER_BASE_FLUSH_TX_INTERVAL_MS);
}

/*===========================================================================
FUNCTION hsu_al_ser_base_ioctl()
Should be called with the Giant mutex locked
===========================================================================*/
boolean hsu_al_ser_base_ioctl
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  sio_ioctl_cmd_type ioctl_cmd,
  sio_ioctl_param_type* ioctl_param_ptr
)
{
  if (!hsu_os_is_giant_locked_by_curr_task())
  {
    HSU_ERR_FATAL("hsu_al_ser_base_ioctl: Should be called "
              " with Giant mutex locked!",0,0,0);
  }

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE); 

  /* It doesn't make sense to call ioctl if the stream is not open */
  HSU_ASSERT(ser_if_data_ptr->stream_id != SIO_NO_STREAM_ID);

  /* Handles only the following IOCTLs:
  SIO_IOCTL_SET_FLOW_CTL
  SIO_IOCTL_GET_FLOW_CTL

  SIO_IOCTL_CHANGE_BAUD_RATE
  SIO_IOCTL_CHANGE_BAUD_NOW
  */
  switch (ioctl_cmd)
  {
    case SIO_IOCTL_SET_FLOW_CTL:
    {
      ser_if_data_ptr->ioctl_flow_ctl_mirror = ioctl_param_ptr->flow_ctl;
      break;
    }
    case SIO_IOCTL_GET_FLOW_CTL:
    {
      ioctl_param_ptr->flow_ctl = ser_if_data_ptr->ioctl_flow_ctl_mirror;
      break;
    }
    case SIO_IOCTL_CHANGE_BAUD_RATE:
    case SIO_IOCTL_CHANGE_BAUD_NOW:
    {
      /* Do nothing, baud rate is not supported in USB...*/
      break;
    }
    default:
    {
      return FALSE;
    }
  }
  return TRUE;
}

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR INTERNAL MODULE (hsu_al_ser_base_i.h)

==============================================================================*/
/*===========================================================================
FUNCTION hsu_al_ser_base_i_ctx_is_valid()
===========================================================================*/
boolean hsu_al_ser_base_i_ctx_is_valid
(
  const hsu_al_ser_base_data_for_ser_if* ctx
)
{
  if (ctx == NULL)
  {
    HSU_MSG_ERROR("hsu_al_ser_base_i_ctx_is_valid: ctx is NULL!");
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION hsu_al_ser_base_i_uninit()
===========================================================================*/
jresult_t hsu_al_ser_base_i_uninit
(
  cdc_appctx_t ctx
)
{
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ctx;

  HSU_MSG_HIGH("*_i_uninit: entered");

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* Initialize general data */
  /* When acm_init/obex_init is issued, this field must point to a valid FD */
  ser_if_data_ptr->instance_of_fd_used              = NULL;
  ser_if_data_ptr->instance_enabled_by_cdc_fd       = FALSE;

  hsu_timer_cancel(&ser_if_data_ptr->tx_info.flush_tx_timer);

#ifdef FEATURE_HS_USB_SER_STATISTICS
  /* Clear the statistic structures */
  memset(&ser_if_data_ptr->statistics, 0, 
    sizeof(hsu_al_ser_base_statistics_struct));
#endif /* FEATURE_HS_USB_SER_STATISTICS */

  HSU_MSG_HIGH("base_i_uninit: exited");

  return HSU_JSUCCESS;
}

/*===========================================================================
FUNCTION hsu_al_ser_base_i_rx_done()
===========================================================================*/
void hsu_al_ser_base_i_rx_done
(
  cdc_iostatus_t       *status,
  void                **buffers,
  juint32_t            *bytes_transferred,
  juint32_t             req_num,
  cdc_appctx_t          ctx
)
{
  al_ser_base_rx_done(
    status, buffers, bytes_transferred, req_num, ctx, FALSE);
}

/*===========================================================================
FUNCTION hsu_al_ser_base_i_tx_done()
===========================================================================*/
void hsu_al_ser_base_i_tx_done
(
  cdc_iostatus_t       *status,
  void                **buffers, 
  juint32_t            *buf_sizes,
  juint32_t             req_num,
  cdc_appctx_t          ctx
)
{
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ctx;

  uint32 i;
  dsm_item_type *cur_dsm_item, *prev_dsm_item = NULL;

  HSU_USE_PARAM(buf_sizes);

  if (req_num == 0)
  {
    HSU_MSG_ERROR("base_i_tx_done: req_num == 0");
    return;
  }

  if (ser_if_data_ptr->debug_mode)
  {
    HSU_MSG_MED_1("base_i_tx_done. req_num=%d", req_num);
  }
  
  /* No need to protect against abort write operation since it can be activated 
   * only from app context that also needs to lock Giant mutex in order to 
   * perform close
   */
  for (i=0, cur_dsm_item = ser_if_data_ptr->tx_info.pending_chain_ptr;
       (i < req_num) && cur_dsm_item; ++i)
  {
    if (ser_if_data_ptr->debug_mode)
    {
      HSU_MSG_MED_1("base_i_tx_done. cur_dsm_item=%x", cur_dsm_item);
    }

    //if (ser_if_data_ptr->iface_type == DIAG_HSU_IFACE)
    //{
    //  HSU_ULOG_ENUM_1(NO_MSG, DATA_LOG, "diag_hsu_iface, buf_size %u", status[i], HSU_TX_CDC_TRANSFER_OFFSET2, buf_sizes[i]);
    //}

    switch (status[i])
    {
      case CDC_TRANSFER_ERROR:
        HSU_MSG_ERROR("base_i_tx_done: CDC_TRANSFER_ERROR");

        HSU_SER_STAT_INC(ser_if_data_ptr->statistics.tx_transfer_error_cnt,1);

        if (ser_if_data_ptr->tx_info.recover_dropped_packet == FALSE)
        {
          /* break out of the switch */
          break;
        }
        /* else fall-through to CDC_TRANSFER_CANCELLED handler */

      case CDC_TRANSFER_CANCELLED:       
        HSU_ULOG_ENUM_2(HIGH_MSG, DATA_LOG, "CDC_TRANSFER_CANCELLED, ser_if_data_ptr 0x%X, is_abort_write_issued %d",
          ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2,
          (uint32) ser_if_data_ptr,
          ser_if_data_ptr->tx_info.is_write_abort_issued);
       
        /* If app_data_ptr->tx_info.is_write_abort_issued == TRUE (and status
         * == CDC_TRANSFER_CANCELLED) it means that flush_tx timer expired
         * and cdc_abort_write was issued 
         */
        if (ser_if_data_ptr->tx_info.is_write_abort_issued)
        {
          dsm_free_packet(&ser_if_data_ptr->tx_info.pending_chain_ptr);
          ser_if_data_ptr->tx_info.pending_chain_cnt = 0;
          if (ser_if_data_ptr->port_id == AL_SER_PORT_DIAG)
          {
            HSU_ULOG_1(NO_MSG, SNPS_LOG, "CANCELLED, pending_chain_cnt %u", ser_if_data_ptr->tx_info.pending_chain_cnt);
          }
        }
        else if (ser_if_data_ptr->tx_info.recover_dropped_packet)
        {
          /* If not all packets in the hardware has completed with ERROR or 
          ** CANCEL, then there is no point of recovering packet that is in the 
          ** middle because host would recieve it out of sequence.
          ** In this case, just drop the packet.
          */
          if (ser_if_data_ptr->tx_info.pending_chain_cnt != req_num)
          {
            HSU_ULOG_ENUM_2(NO_MSG, DATA_LOG, "recover_dropped_packet, unable_to_recover, pending_chain_cnt %u, req_num %u",
              ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2,
              ser_if_data_ptr->tx_info.pending_chain_cnt, req_num);

            HSU_SER_STAT_INC(ser_if_data_ptr->statistics.tx_dropped_packet_cnt,1);
            break;
          }
          
          if (prev_dsm_item != NULL)
          {
            /* If previous DSM item in the chain was not null, 
             * break the chain, and free up to current chain */
            prev_dsm_item->pkt_ptr = NULL;
            dsm_free_packet(&ser_if_data_ptr->tx_info.pending_chain_ptr);
          }
          
          /* Clear pending chain */
          ser_if_data_ptr->tx_info.pending_chain_ptr = NULL;
          ser_if_data_ptr->tx_info.pending_chain_cnt = 0;
          if (ser_if_data_ptr->port_id == AL_SER_PORT_DIAG)
          {
            HSU_ULOG_1(NO_MSG, SNPS_LOG, "CANCELLED_2, pending_chain_cnt %u", ser_if_data_ptr->tx_info.pending_chain_cnt);
          }
          ser_if_data_ptr->tx_info.is_write_in_progress = FALSE;

          /* If there was any cancelled items present and no abort issues:
           * 1. Put the chain to the front of the remaining_ptr
           *    Note: It will not hold "contiguous buffer" in app_ptr 
           *    of the DSM item if "recover_dropped_packet" is TRUE.
           * 2. Update pending_chain and pending_chain_cnt
           */
          if (ser_if_data_ptr->tx_info.remaining_chain_ptr == NULL)
          {
            /* If remaining chain is empty, set the current chain to 
             * remaining chain */
            ser_if_data_ptr->tx_info.remaining_chain_ptr = cur_dsm_item;
          }
          else
          {
            /* Since remaining is not empty, the current chain has to go
             * front of remaining chain */       
            dsm_append(
              &cur_dsm_item, 
              &ser_if_data_ptr->tx_info.remaining_chain_ptr);
            ser_if_data_ptr->tx_info.remaining_chain_ptr = cur_dsm_item;
          }
          
          /* Handle closing of the port if the port was closing at same time 
           * before return */
          if (!al_ser_base_handle_closing(ser_if_data_ptr))
          {
            /* If the send_all loop is in progress, the loop will handle the 
            ** completion.  Enqueuing to AL_TASK QUEUE in a loop will cause
            ** the QUEUE to overflow.
            */
            if (!ser_if_data_ptr->tx_info.is_send_all_in_progress)
            {
              HSU_ULOG_ENUM_1(NO_MSG, DATA_LOG, "recover_dropped, al_ser_base_tx_check_complete, ser_if_data_ptr 0x%08X",
                ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2,
                (uint32)ser_if_data_ptr);

              hsu_al_task_enqueue_cmd(al_ser_base_tx_check_complete, (void*)ser_if_data_ptr);
            }
          }
          return;
        }
        break;

      case CDC_TRANSFER_OK:
        if (ser_if_data_ptr->debug_mode)
        {
          HSU_MSG_MED_3("base_i_tx_done: CDC_TRANSFER_OK. "
            "length=%d, buf[0]=%x, buf[1]=%x",
            buf_sizes[i], ((uint8*)(buffers[i]))[0], ((uint8*)(buffers[i]))[1]);
        }

        if (ser_if_data_ptr->port_id == AL_SER_PORT_DIAG)
        {
          HSU_ULOG_2(NO_MSG, SNPS_LOG, "buf 0x%X, size %u", (uint32)buffers[i], buf_sizes[i]);
        }

        break;

      default:
        HSU_ERR_FATAL("base_i_tx_done: unexpected status %d", status[i], 0, 0);
        break;
    }

    if (cur_dsm_item->app_ptr)
    {
      if (ser_if_data_ptr->uses_dsm_app_ptr)
      {
      if(ser_if_data_ptr->port_id == AL_SER_PORT_USB_SER4)
      {
        HSU_ULOG_1(NO_MSG, SER4_LOG, "SER4, packet_tansfer_completed, mem_read_counter %u", mem_read_counter);
        mem_read_counter++;
      } 
      } 
      else
      {
        hsu_al_dsm_utils_free_contiguous_buf(cur_dsm_item->app_ptr);
        cur_dsm_item->app_ptr = NULL;
      }   
    }

    if (!ser_if_data_ptr->uses_dsm_app_ptr)
    {
      // For EFS_SYNC we have only 1 dsm item per Memory read, we need to skip these steps
      prev_dsm_item = cur_dsm_item;
      cur_dsm_item = cur_dsm_item->pkt_ptr;
    }

  }

  if (ser_if_data_ptr->debug_mode)
  {
    HSU_MSG_LOW_2("base_i_tx_done. pending_chain_ptr=%x, cur_dsm_item=0x%x",
      ser_if_data_ptr->tx_info.pending_chain_ptr, cur_dsm_item);
  }

  /* See corresponding comment in hsu_al_ser_i_rx_done. */
  if (al_ser_base_handle_closing(ser_if_data_ptr))
  {
    dsm_free_packet(&ser_if_data_ptr->tx_info.pending_chain_ptr);
    ser_if_data_ptr->tx_info.pending_chain_cnt = 0;
    if (ser_if_data_ptr->port_id == AL_SER_PORT_DIAG)
    {
      HSU_ULOG_1(NO_MSG, SNPS_LOG, "CLOSING, pending_chain_cnt %u", ser_if_data_ptr->tx_info.pending_chain_cnt);
    }
    ser_if_data_ptr->tx_info.is_write_in_progress = FALSE;
    return;
  }

  /* Free the DSM items that were sent. pending_chain_ptr will hold the 
   * remaining items that were not yet sent */
  if (cur_dsm_item && prev_dsm_item)
  {
    prev_dsm_item->pkt_ptr = NULL;
  }
  if (ser_if_data_ptr->tx_info.pending_chain_ptr)
  {
    if (ser_if_data_ptr->uses_dsm_app_ptr)
    {
      ser_if_data_ptr->tx_info.pending_chain_cnt -= req_num;
      if (ser_if_data_ptr->port_id == AL_SER_PORT_DIAG)
      {
        HSU_ULOG_2(NO_MSG, SNPS_LOG, "DEC %u, pending_chain_cnt %u", req_num, ser_if_data_ptr->tx_info.pending_chain_cnt);
      }

      if (ser_if_data_ptr->tx_info.pending_chain_cnt == 0)
      {
        if (cur_dsm_item)
        {
          cur_dsm_item->app_ptr = NULL;
        }
        dsm_free_packet(&ser_if_data_ptr->tx_info.pending_chain_ptr);
        ser_if_data_ptr->tx_info.pending_chain_ptr = NULL;
      }
    }
    else
    {
      dsm_free_packet(&ser_if_data_ptr->tx_info.pending_chain_ptr);
      ser_if_data_ptr->tx_info.pending_chain_ptr = cur_dsm_item;
      ser_if_data_ptr->tx_info.pending_chain_cnt -= req_num;
      if (ser_if_data_ptr->port_id == AL_SER_PORT_DIAG)
      {
        HSU_ULOG_2(NO_MSG, SNPS_LOG, "DEC %u, pending_chain_cnt %u", req_num, ser_if_data_ptr->tx_info.pending_chain_cnt);
      }
    }
  }

  if (ser_if_data_ptr->tx_info.pending_chain_ptr == NULL)
  {
    if ( ((ser_if_data_ptr->tx_wmq_ptr == NULL) || dsm_is_wm_empty(ser_if_data_ptr->tx_wmq_ptr))
       && (ser_if_data_ptr->tx_info.remaining_chain_ptr == NULL))
    {
      /* No more pending buffers */
      ser_if_data_ptr->tx_info.is_write_in_progress = FALSE;

      if (ser_if_data_ptr->tx_info.flush_tx_complete_cb != NULL)
      {        
        HSU_ULOG_ENUM_1(HIGH_MSG, DATA_LOG, "hsu_al_ser_base_i_tx_done, cb_invote, flush_tx_complete_cb 0x%8X", 
          ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2, (uint32)ser_if_data_ptr->tx_info.flush_tx_complete_cb);
        ser_if_data_ptr->tx_info.flush_tx_complete_cb(ser_if_data_ptr);
        ser_if_data_ptr->tx_info.flush_tx_complete_cb = NULL;
        hsu_timer_cancel(&(ser_if_data_ptr->tx_info.flush_tx_timer));
        ser_if_data_ptr->tx_info.flush_tx_in_progress = FALSE;
      }

      /* Since there are no more buffers, we can always clear the flag for
      ** write abort
      */
      ser_if_data_ptr->tx_info.is_write_abort_issued = FALSE;
    }
  }
}

/*===========================================================================
FUNCTION hsu_al_ser_base_i_enabled()
Called when the Giant is locked from enabled_cb
===========================================================================*/
jresult_t hsu_al_ser_base_i_enabled
(
  cdc_appctx_t ctx
)
{
  hsu_al_ser_base_data_for_ser_if* app_data_ptr = NULL;
  cdc_ctx_t * cdc_ctx = NULL;  
  if (!hsu_os_is_giant_locked_by_curr_task())
  {
    HSU_ERR_FATAL("hsu_al_ser_base_i_enabled: Should be called "
      " with Giant mutex locked!", 0, 0, 0);
    return 0;
  }

  app_data_ptr = (hsu_al_ser_base_data_for_ser_if*)ctx;
  cdc_ctx = (cdc_ctx_t*) app_data_ptr->instance_of_fd_used; 

  HSU_MSG_HIGH("*_i_enabled: Entered");

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(app_data_ptr) == TRUE);

  /* Make sure no two enabled cbs are received without a disabled in between*/
  HSU_ASSERT(app_data_ptr->instance_enabled_by_cdc_fd == FALSE);

  app_data_ptr->instance_enabled_by_cdc_fd = TRUE;

  /* Check if the app is in closing process (or already closed): */
  /* If so, don't do any reads...*/
  HSU_MSG_LOW("*_i_enabled: Checking if app is closed/closing process");

  if ((app_data_ptr->num_of_aborts_issued_prior_to_close > 0)
    || (hsu_al_ser_base_i_has_already_been_closed(app_data_ptr)) == TRUE)
  {
    HSU_MSG_HIGH("*_i_enabled: Exit (app closed/closing)");
    return HSU_JSUCCESS;
  }

  /* Setup a new read, but only if open already issued (or if interface 
   * is using read) and if the inbound flow is enabled:*/
  if (app_data_ptr->rx_info.inbound_flow_enabled)
  {
    if((cdc_ctx->info.basic.flags & CDC_INIT_OBEX_BULK_IN_ONLY) != CDC_INIT_OBEX_BULK_IN_ONLY)    
    {          
    hsu_al_ser_base_i_rx_setup_read(app_data_ptr);
  }
  }

  if (app_data_ptr->tx_info.recover_dropped_packet)
  {
    /* To resume TX (Downlink) */
    hsu_al_ser_i_base_tx_set_handle_complete_timer(app_data_ptr);
  }

  HSU_MSG_HIGH("*_i_enabled: Exit (after requesting a read operation)");

  return HSU_JSUCCESS;
}

/*===========================================================================
FUNCTION hsu_al_ser_base_i_disabled()
===========================================================================*/
jresult_t hsu_al_ser_base_i_disabled
(
  cdc_appctx_t ctx
)
{
  hsu_al_ser_base_data_for_ser_if* app_data_ptr = NULL;
  app_data_ptr = (hsu_al_ser_base_data_for_ser_if*)ctx;

  HSU_MSG_HIGH("*_i_disabled: Entered");

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(app_data_ptr) == TRUE);

  /* Make sure no two disabled cbs are received without an enabled in between*/
  HSU_ASSERT(app_data_ptr->instance_enabled_by_cdc_fd == TRUE);


  /* No need for critical section here. In all open/close/read/write scenarios
  Eventually there is a check if instance_enabled_by_cdc_fd is FALSE before
  invoking cdc_read / cdc_write.
  */
  app_data_ptr->instance_enabled_by_cdc_fd = FALSE;

  /* It is assumed that any abort read/writes will receive callbacks for */
  HSU_MSG_HIGH("base_i_disabled: Exit");

  if (!app_data_ptr->tx_info.recover_dropped_packet)
  {
    /* TX cleanup. stack will clean any pending buffers with tx_done. The other
     * data that was not yet passed to stack remains and will be sent after next
     * cable connection upon hsu_al_ser_base_transmit */
    hsu_timer_cancel(&app_data_ptr->tx_info.tx_complete_timer);
    
    app_data_ptr->tx_info.is_tx_complete_timer_scheduled = FALSE;
    app_data_ptr->tx_info.is_write_in_progress = FALSE;
  }

  return HSU_JSUCCESS;
}

/*===========================================================================
FUNCTION hsu_al_ser_base_i_resumed()
===========================================================================*/
jresult_t hsu_al_ser_base_i_resumed
(
  cdc_appctx_t ctx
)
{
  hsu_al_ser_base_data_for_ser_if* app_data_ptr = NULL;
  cdc_ctx_t * cdc_ctx = NULL;

  app_data_ptr = (hsu_al_ser_base_data_for_ser_if*)ctx;
  cdc_ctx = (cdc_ctx_t*) app_data_ptr->instance_of_fd_used;

  HSU_MSG_HIGH("*_i_resumed: Entered");

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(app_data_ptr) == TRUE);

  if ((app_data_ptr->rx_info.inbound_flow_enabled == FALSE) ||
      (app_data_ptr->rx_info.num_buf_waiting != 0))
  {
    /* Either inbound flow disabled, or it is in the process of forwarding 
     * a waiting DSM chain to the application. In both cases, do not issue 
     * a cdc_read. */
    HSU_MSG_LOW("*_i_resumed: inbound flow is disabled or in process of forwarding to app");
    return HSU_JSUCCESS;
  }

  if (app_data_ptr->instance_enabled_by_cdc_fd == FALSE)
  {
    HSU_MSG_LOW("*_i_resumed: instance not enabled, nothing to do...");
    return HSU_JSUCCESS;
  }

  if (hsu_al_ser_base_i_has_already_been_closed(app_data_ptr) == TRUE)
  {
    HSU_MSG_LOW("*_i_resumed: app not using instance, nothing to do...");
    return HSU_JSUCCESS;
  }

  if ((app_data_ptr->tx_info.recover_dropped_packet)
      /*&& (app_data_ptr->instance_enabled_by_cdc_fd)*/)
  {   
    /* To resume TX (Downlink) */
    hsu_al_ser_i_base_tx_set_handle_complete_timer(app_data_ptr);
  }

  if ((cdc_ctx->info.basic.flags & CDC_INIT_OBEX_BULK_IN_ONLY) != CDC_INIT_OBEX_BULK_IN_ONLY)
  {
    if (NULL == app_data_ptr->rx_info.dsm_chain_ptr)
    {
      HSU_ULOG_ENUM(ERROR_MSG, DATA_LOG, "hsu_al_ser_base_i_resumed, queuing_rx_buffer", 
        app_data_ptr->iface_type, HSU_IFACE_OFFSET2);
      
      hsu_al_ser_base_i_rx_setup_read(app_data_ptr);
    }
  }

  HSU_MSG_LOW("*_i_resumed: Exited");

  return HSU_JSUCCESS;
}

/*===========================================================================

FUNCTION hsu_al_ser_base_i_internal_close

DESCRIPTION
Reverts various fields of the given data for serial I/F struct to default values,
and calls the app's close callback.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_base_i_internal_close
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
                               /* Serial I/F data struct used in operation */
)
{
  hsu_al_ser_base_op_complete_cb close_cb = NULL;

  HSU_ULOG_ENUM_1(HIGH_MSG, DATA_LOG, "hsu_al_ser_base_i_internal_close, close_complete_cb 0x%8X", ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2, (uint32)ser_if_data_ptr->close_complete_cb);

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  HSU_ASSERT(ser_if_data_ptr->num_of_aborts_issued_prior_to_close == 0);

  close_cb = ser_if_data_ptr->close_complete_cb;

  /* Clear the flush timer */
  hsu_timer_cancel(&(ser_if_data_ptr->tx_info.flush_tx_timer));

  /* Clear the parameter */
  hsu_al_ser_base_i_close_clear_data(ser_if_data_ptr);

  /* Finally, invoke the app close cb (if given): */
  if (close_cb != NULL)
  {
    close_cb(ser_if_data_ptr->arg_cb);
  }
}

/*===========================================================================

FUNCTION al_ser_base_internal_abort_read

DESCRIPTION
Requests the CDC FD to abort all read operations for the given ser_if_data_ptr

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_internal_abort_read
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
                               /* Serial I/F data struct used in operation */
)
{
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* Abort read: */
  /* Note that ser_if_data_ptr->memory_pool_for_rx must still exist - 
  since the read callback (with status CANCELLED) must free the buffer to the pool.*/
  /* Therefore, in the case of close, cleaning of ser_if_data_ptr would be invoked there */

  (void)hsu_cdc_fd_api_cdc_abort_read(ser_if_data_ptr->instance_of_fd_used);

  HSU_ULOG_ENUM(HIGH_MSG, DATA_LOG, "al_ser_base_internal_abort_read", ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2);
}

/*===========================================================================

FUNCTION al_ser_base_internal_abort_write

DESCRIPTION
Requests the CDC FD to abort all write operations for the given ser_if_data_ptr

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_internal_abort_write
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
                               /* Serial I/F data struct used in operation */
)
{
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* Abort write: */
  /* Note that ser_if_data_ptr->memory_pool_for_tx must still exist - since the write callback (with status CANCELLED) must free the buffer to the pool.*/
  /* Therefore, in the case of close, cleaning of ser_if_data_ptr would be invoked there */

  (void)hsu_cdc_fd_api_cdc_abort_write(ser_if_data_ptr->instance_of_fd_used);

  HSU_ULOG_ENUM(HIGH_MSG, DATA_LOG, "al_ser_base_internal_abort_write", ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2);
}

/*===========================================================================

FUNCTION al_ser_base_end_close

DESCRIPTION
Serves as a "client" callback for Flush Tx, whenever the client invoked 
hsu_al_ser_base_close, with the flag: abort_io_opertions_immediately being 
set to FALSE.

Will simply abort any read operation (and eventually call 
hsu_al_ser_base_i_internal_close)

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_end_close
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
                               /* Serial I/F data struct used in operation */
)
{
  HSU_MSG_HIGH("*al_ser_base_end_close: Entered");

  HSU_MSG_LOW_1("*al_ser_base_end_close: Checking if \
              ser_if_data_ptr (0x%x) valid...", ser_if_data_ptr);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);


  /* Abort read (and eventually: clean the data struct and 
  finally call client close callback) */

  /* No need for abort write as flush tx callback is called when no more Tx */

  /* Check if client is in the middle of a read operation (note that this is 
  invoked from either DPC Tx done or from app context (close), the latter needs 
  to grab the Giant mutex to protect it from enabled cb.
  */

  if (ser_if_data_ptr->rx_info.read_oper_is_in_progress == TRUE)
  {
    ++ser_if_data_ptr->num_of_aborts_issued_prior_to_close;
    HSU_ASSERT(ser_if_data_ptr->num_of_aborts_issued_prior_to_close <= 
      AL_SER_BASE_MAX_IO_ABORTS_BEFORE_CLOSE);

    al_ser_base_internal_abort_read(ser_if_data_ptr);
  }
  else
  {
    /* No more aborts - call close immediately */
    hsu_al_ser_base_i_internal_close(ser_if_data_ptr);
  }

  HSU_MSG_HIGH("*al_ser_base_end_close: Exit");
}

/*===========================================================================

FUNCTION hsu_al_ser_base_i_has_already_been_closed

DESCRIPTION
Checks if this serial I/F data struct has already been closed by this layer. 
This means that the client's close cb had been called before, and therefore
had set the field stream_id to SIO_NO_STREAM_ID.

DEPENDENCIES
None

RETURN VALUE
TRUE - Close has been issued. FALSE - Otherwise

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_ser_base_i_has_already_been_closed
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
                               /* Serial I/F data struct used in operation */
)
{
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr));

  return (boolean)(ser_if_data_ptr->stream_id == SIO_NO_STREAM_ID);
}

/*===========================================================================

FUNCTION al_ser_base_flush_tx_timer_cb

DESCRIPTION
Tx_flush timer callback, invoked upon timer expiration.
This cb is called every AL_SER_BASE_FLUSH_TX_INTERVAL_MS(=20 mili).
It calls cdc_handle_tx_completion every time to check if there was a
packet that was sent but we didnt receive "tx_done" for it.

This is for preventing the case where the the last packet is sent and we wait
for it for a very long time= until the flush times out (after AL_SER_BASE_FLUSH_TX_TIMEOUT_MS)

The flush ends when the last tx_done is received,
and the "flush_cb" passed is called from handle_tx_done func.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
Issues cdc_abort_write.

===========================================================================*/
static void al_ser_base_flush_tx_timer_cb
(
 unsigned long ser_if_data_as_cb_arg
)
{
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ser_if_data_as_cb_arg;

  HSU_MSG_LOW_1("*_flush_tx_timer_cb: Checking if ctx (0x%x) valid...", 
    ser_if_data_as_cb_arg);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  ser_if_data_ptr->tx_info.flush_tx_duration +=     AL_SER_BASE_FLUSH_TX_INTERVAL_MS;
  /*
  check how much time has passed since flush has started.
  If it reached AL_SER_BASE_FLUSH_TX_TIMEOUT_MS then - abort.
  */
  if(ser_if_data_ptr->tx_info.flush_tx_duration >=     AL_SER_BASE_FLUSH_TX_TIMEOUT_MS)
  {  
    HSU_ULOG_ENUM(HIGH_MSG, DATA_LOG, "al_ser_base_flush_tx_timer_cb, aborting", ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2);

    jsafe_enter();
    ser_if_data_ptr->tx_info.is_write_abort_issued = TRUE;
    jsafe_leave();

    (void)hsu_cdc_fd_api_cdc_abort_write(ser_if_data_ptr->instance_of_fd_used);
    return;
  }


  /*call cdc_handle_completion so the tx_done will be invoked explicitly
    for the last packet sent- to eventually end the "flush" process*/
  /* Grab giant mutex */
  jsafe_enter();
  cdc_handle_tx_completion(ser_if_data_ptr->instance_of_fd_used);

  if (ser_if_data_ptr->tx_info.is_write_in_progress == TRUE)
  {
    hsu_timer_set(
      &(ser_if_data_ptr->tx_info.flush_tx_timer),
      AL_SER_BASE_FLUSH_TX_INTERVAL_MS,
      T_MSEC);
  }
  /* Release giant mutex */
  jsafe_leave();
}


/*==============================================================================

FUNCTION hsu_al_base_acm_send_tx_buffer

DESCRIPTION
Sends a TX buffer (contiguous buffer or a single DSM item).

PARAMETERS
ser_if_data_ptr         : Pointer to serial I/F data struct
tx_ptr                  : Tx data to be sent to the host

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
static jresult_t al_ser_base_tx_send_buffer
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr,
  dsm_item_type        *tx_ptr
)
{
  boolean ret_val             = FALSE;
  uint32  orig_chain_pkt_len  = 0;
  uint32  new_chain_pkt_len   = 0; 
  jresult_t jresult           = HSU_JSUCCESS;

  /* Contiguous buffer pointer.  If this is not NULL, 
   * contiguous buffer is used */
  uint8*  contig_buf_ptr      = NULL;

  /* Below must be filled out for the item to be sent */
  uint8*  send_buf_ptr        = NULL;
  uint32  send_byte           = 0;

  cdc_ctx_t *ctx;
  core_t    *core;
  hsu_bus_mode_enum current_bus_mode;

  HSU_ASSERT(ser_if_data_ptr != NULL);
  HSU_ASSERT(tx_ptr!= NULL);

  ctx = (cdc_ctx_t*)(ser_if_data_ptr->instance_of_fd_used);
  core = (core_t *)(ctx->core_ctx);

  current_bus_mode = hsu_get_current_bus_mode((uint32)core->core_number);
    
  if (current_bus_mode == HSU_BUS_MODE_SUSPENDED)
  {
    HSU_ULOG_ENUM_1(ERROR_MSG, BUS_LOG, "during_suspend, ser_if_data_ptr 0x%08X", ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2, (uint32)ser_if_data_ptr);
  }

  /* Check if drop packet on suspend bus mode is enabled */
  if ((ser_if_data_ptr->tx_info.drop_packet_on_suspend)
   && (ser_if_data_ptr->instance_of_fd_used))
  {
    if ( (current_bus_mode == HSU_BUS_MODE_SUSPENDED)
      || (current_bus_mode == HSU_BUS_MODE_DISCONNECTED)
      || (current_bus_mode == HSU_BUS_MODE_UNCONFIGURED) )
    {
      /* Drop packet if the bus is suspended, disconnected, or unconfigured */
      dsm_free_packet(&tx_ptr);

      HSU_SER_STAT_INC(ser_if_data_ptr->statistics.tx_dropped_packet_cnt,1);
      return jresult;
    }
  }

  if (tx_ptr->pkt_ptr == NULL)
  {
#ifdef FEATURE_HS_USB_SER4_PORT
  if (ser_if_data_ptr->uses_dsm_app_ptr)
  {
    if (!tx_ptr->app_ptr)
    {
      HSU_ULOG(NO_MSG, DATA_LOG, "hsu_efs_sync_app_ptr_null");
    }
    
    if (!tx_ptr->app_field)
    {
      HSU_ULOG(NO_MSG, DATA_LOG, "hsu_efs_sync_app_field_null");
    }
    /* If tx_ptr->app_ptr & tx_ptr->app_field are valid, set send buffer, size, and byte */
      send_buf_ptr = tx_ptr->app_ptr;
      send_byte = tx_ptr->app_field;
  }
  else
#endif /* FEATURE_HS_USB_SER4_PORT */
  {
  /* If there only single DSM item, set send buffer, size, and byte */
      send_buf_ptr = tx_ptr->data_ptr;
      send_byte = tx_ptr->used;
  }  
  }
  else
  {
    /* If 2 packets cannot fit in the Contiguous buffer,
     * it is not worth it to copy it. Also check if contiguous buffer is 
     * available. Contiguous buffer will not be used for data using 
     * "recover_dropped_packet".
     */
    if (((tx_ptr->used + tx_ptr->pkt_ptr->used) >=
          HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE) || 
        (hsu_al_dsm_utils_get_contiguous_buf_free_count() == 0) ||
        (ser_if_data_ptr->tx_info.recover_dropped_packet))
    {
      /* Setting the new chain packet length to used length will effectively
       * split only the first packet */
      new_chain_pkt_len = tx_ptr->used;
    }
    else
    {      
      /* Obtain Contiguous buffer */
      contig_buf_ptr = hsu_al_dsm_utils_alloc_contiguous_buf();
      if (contig_buf_ptr != NULL)
      {       
        new_chain_pkt_len = HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE;
      }
      else
      {        
        /* If obtaining contiguous buffer fails, enqueue the tx_ptr to TxWMQ */
        HSU_MSG_ERROR("send_tx_buffer: NULL contiguous_buf");

        /* Since obtaining Contiguous buffer failed, just split the first 
         * DSM item */
        new_chain_pkt_len = tx_ptr->used;
      }
    }
    
    orig_chain_pkt_len = dsm_length_packet(tx_ptr);

    /* Split the DSM chain */
    ret_val = hsu_al_dsm_utils_split_dsm_chain(
      tx_ptr->pool_id,
      tx_ptr, 
      orig_chain_pkt_len, 
      &(ser_if_data_ptr->tx_info.remaining_chain_ptr), 
      new_chain_pkt_len);
    
    HSU_ASSERT(ret_val);
   
    /* Update length of Tx chain */
    new_chain_pkt_len = dsm_length_packet(tx_ptr);

    if (contig_buf_ptr == NULL)
    {
      HSU_ASSERT(tx_ptr->pkt_ptr == NULL);
      
      send_buf_ptr = tx_ptr->data_ptr;
      send_byte = tx_ptr->used;
    }
    else
    {
      send_buf_ptr = contig_buf_ptr;
      
      /* Gather the chain into a Contiguous buffer */
      send_byte = hsu_al_dsm_utils_gather_buffer_without_dmov(
        tx_ptr, 
        new_chain_pkt_len, 
        send_buf_ptr, 
        HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE);

      /* Debug: count contiguous buffer usage */
      HSU_SER_STAT_INC(ser_if_data_ptr->statistics.tx_contiguous_buf_usage_cnt,1);
    }    
  }

  /* Check Error, if send byte is 0 return */
  if (send_byte == 0)
  {
    HSU_MSG_ERROR("send_tx_buffer: bytes_to_send == 0");

    if (contig_buf_ptr != NULL)
    {
      hsu_al_dsm_utils_free_contiguous_buf(contig_buf_ptr);
    }

    HSU_SER_STAT_INC(ser_if_data_ptr->statistics.tx_dropped_packet_cnt,1);

    dsm_free_packet(&tx_ptr);
    return jresult;
  }

  HSU_SER_STAT_INC(ser_if_data_ptr->statistics.tx_total_bytes, send_byte);

  /* Flush cache in region to ensure the data is up to date */
  hsu_dcache_clean_region(send_buf_ptr, send_byte);

  // Get PA from VA (because USB hardware needs PA)
  send_buf_ptr = (void*) (qurt_lookup_physaddr((qurt_addr_t)send_buf_ptr));
  
  if(send_buf_ptr == NULL)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "Error: Addr passed is not mapped");
	dsm_free_packet(&tx_ptr);
	return jresult;
  }

  if (ser_if_data_ptr->iface_type == MDM_HSU_IFACE)
  {
    hsu_dun_log(DUN__TX__LOG, send_buf_ptr, send_byte);
  }

  jresult = al_ser_base_tx_write(
    ser_if_data_ptr, 
    tx_ptr, 
    send_buf_ptr, 
    send_byte, 
    ((contig_buf_ptr != NULL) ? TRUE : FALSE));

  return jresult;
}


/*==============================================================================
FUNCTION hsu_al_base_acm_send_all_tx_data

DESCRIPTION
This function goes over the remaining DSM chain and then the WMQ and sends 
the DSM chains until there are no resources left.

PARAMETERS
ser_if_data_ptr         : Pointer to serial I/F data struct

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
static void al_ser_base_tx_send_all_data
(
 hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
)
{
  jresult_t jresult;
  
  dsm_item_type *tx_ptr = NULL;

  /* Watermark queue could be null because if port gets closed before this function call */
  if (ser_if_data_ptr->tx_wmq_ptr == NULL)
  {
    HSU_MSG_ERROR_1("tx_send_all_data: TX WMQ is NULL: 0x%x", 
      ser_if_data_ptr);
    return;
  }

  ser_if_data_ptr->tx_info.is_send_all_in_progress = TRUE;

  while ((ser_if_data_ptr->tx_info.remaining_chain_ptr != NULL) || 
         (dsm_is_wm_empty(ser_if_data_ptr->tx_wmq_ptr) == FALSE))
  {
    tx_ptr = NULL;

    if ( (FALSE == ser_if_data_ptr->instance_enabled_by_cdc_fd)
      && (ser_if_data_ptr->tx_info.recover_dropped_packet)
      && (FALSE == ser_if_data_ptr->tx_info.drop_packet_on_suspend) )
    {
      break;
    }

    /* Check HW Queue has reached the Max Pending size */     
    if (ser_if_data_ptr->tx_info.pending_chain_cnt >= 
        ser_if_data_ptr->tx_info.tx_max_pending_chain_cnt)
    {
      hsu_al_ser_i_base_tx_set_handle_complete_timer(ser_if_data_ptr);        
      break;
    }

    if (ser_if_data_ptr->tx_info.remaining_chain_ptr != NULL)
    {
      tx_ptr = ser_if_data_ptr->tx_info.remaining_chain_ptr;     
      ser_if_data_ptr->tx_info.remaining_chain_ptr = NULL;
    }
    else
    {
      tx_ptr = dsm_dequeue(ser_if_data_ptr->tx_wmq_ptr);

      if (NULL == tx_ptr)
      {
        // Since watermark queue also belongs to client, we cannot assume
        // that there would be dsm item is still in the queue from last time
        // we checked watermark queued in while loop condition.
        // The queue can get emptied by the client at any point in between. 
        HSU_MSG_ERROR_1("tx_send_all_data: TX WMQ returned no item: 0x%x", 
          ser_if_data_ptr);
        break;
      }

      HSU_SER_STAT_INC(ser_if_data_ptr->statistics.tx_total_dequeue,1);
    }

    if (DIAG_HSU_IFACE == ser_if_data_ptr->iface_type)
    {
      HSU_ULOG(NO_MSG, SNPS_LOG, "DIAG AL SEND ALL");
    }
    jresult = al_ser_base_tx_send_buffer(ser_if_data_ptr, tx_ptr);

    if (jresult == ESUSPENDED)
    {
      /* If the device is ESUSPEND, device cannot commit any buffer to h/w */
      break;
    }
  }
  
  ser_if_data_ptr->tx_info.is_send_all_in_progress = FALSE;
}


/*==============================================================================

FUNCTION al_ser_base_tx_write

DESCRIPTION
This function will invoke cdc_write_multiple in order to send a dsm chain buffer
or contiguous buffer.

PARAMETERS
ser_if_data_ptr         : Pointer to serial I/F data struct
tx_ptr                  : Pointer to Tx data in DSM item
buf_ptr                 : Buffer pointer inside the DSM item or contiguous 
                          buffer
buf_size                : Buffer size of the data pointed by buf_ptr
contiguous_buf_used     : Flag to indicate if the buf_ptr is pointing to
                          a contiguous buffer

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
static jresult_t al_ser_base_tx_write
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr,
  dsm_item_type        *tx_ptr,
  uint8                *buf_ptr,
  uint32                buf_size,
  boolean               contiguous_buf_used
)
{
  jresult_t res;
  jresult_t jresult;
#ifdef FEATURE_HS_USB_SER4_PORT
  uint32 write_req_num = 1;
  static byte *         write_buf_array[USB_SER4_MAX_WRITE_BUF_COUNT];
  static uint32         write_buf_size_array[USB_SER4_MAX_WRITE_BUF_COUNT];

  uint32 i = 0;
  byte * p_tx_buffer = NULL;

  if (hsu_conf_sel_is_usb_core_snps()) 
  {
    uint32 trb_count = 0;

    trb_count = (buf_size + HSU_AL_SNPS_SER4_DATA_BUFFER_SIZE - 1) 
                / HSU_AL_SNPS_SER4_DATA_BUFFER_SIZE;
    if(trb_count > USB_SNPS_SER4_MAX_WRITE_BUF_COUNT)
    {
      trb_count = USB_SNPS_SER4_MAX_WRITE_BUF_COUNT;
    }
    
    p_tx_buffer = buf_ptr;

    if(buf_size > HSU_AL_SNPS_SER4_DATA_BUFFER_SIZE)
    {
      // we do not support this - HSU_ERR_FATAL
    }
    
    for ( i = 0 ; i < trb_count; ++i )
    {
      if (i == (trb_count-1))
      {
        /* For the last TD, set the TD buf size to multiples of max packet size */
        uint32 last_trb_size = buf_size % HSU_AL_SNPS_SER4_DATA_BUFFER_SIZE;
        if ( last_trb_size == 0 )
        {
          last_trb_size = HSU_AL_SNPS_SER4_DATA_BUFFER_SIZE;
        }
        write_buf_array[i] = p_tx_buffer;
        write_buf_size_array[i] = last_trb_size;
        p_tx_buffer += last_trb_size;
      }
      else
      {
        write_buf_array[i] = p_tx_buffer;
        write_buf_size_array[i] = HSU_AL_SNPS_SER4_DATA_BUFFER_SIZE;
        p_tx_buffer += HSU_AL_SNPS_SER4_DATA_BUFFER_SIZE;
      }
    }
    
    write_req_num = trb_count;
  }
  else
  {

    uint32 td_count = 0;

    /* Set up write write buffers for upto 1MB.
       This requires setting up 64 TDs each with max 16KB.  */
    
    /* Figure out how many TDs we need to get the write data */
    td_count = (buf_size + HSU_AL_SER4_DATA_BUFFER_SIZE - 1) 
               / HSU_AL_SER4_DATA_BUFFER_SIZE;
    
    /* Limit TD count to USB_MAX_WRITE_BUF_COUNT */
    if (td_count > USB_SER4_MAX_WRITE_BUF_COUNT)
    {
      td_count = USB_SER4_MAX_WRITE_BUF_COUNT;
    } 
    
    p_tx_buffer = buf_ptr;
    for( i = 0 ; i < td_count ; ++i )
    {
      if (i == (td_count-1))
      {
        /* For the last TD, set the TD buf size to multiples of max packet size */
        uint32 last_td_size = buf_size % HSU_AL_SER4_DATA_BUFFER_SIZE;
        if ( last_td_size == 0 )
        {
          last_td_size = HSU_AL_SER4_DATA_BUFFER_SIZE;
        }
        write_buf_array[i] = p_tx_buffer;
        write_buf_size_array[i] = last_td_size;
        p_tx_buffer += last_td_size;
      }
      else
      {
        write_buf_array[i] = p_tx_buffer;
        write_buf_size_array[i] = HSU_AL_SER4_DATA_BUFFER_SIZE;
        p_tx_buffer += HSU_AL_SER4_DATA_BUFFER_SIZE;
      }
    }
    
    write_req_num = td_count;
  }


  HSU_SER_STAT_INC(ser_if_data_ptr->statistics.tx_total_items, write_req_num);

  hsu_trace_iface_set(ser_if_data_ptr->iface_type);
  hsu_trace_msg2_set(AL_SER_BASE_TX_WRITE__TRC);

  if (ser_if_data_ptr->port_id == AL_SER_PORT_DIAG)
  {
     HSU_ULOG_2(NO_MSG, SNPS_LOG, "buf 0x%X, size %u", (uint32)write_buf_array[0], write_buf_size_array[0]);
  }

  jresult = cdc_write_multiple(ser_if_data_ptr->instance_of_fd_used, 
  (void**)&write_buf_array, write_buf_size_array, write_req_num, ser_if_data_ptr->tx_info.tx_timeout);
  
  if(ser_if_data_ptr->port_id == AL_SER_PORT_USB_SER4)
  {
    HSU_ULOG_1(NO_MSG, SER4_LOG, "ser4_transfer_submitted, buf_size %u", buf_size);

    /* Mem Debug Table is 52 bytes after which we start Memory read & raw data transfers
    ** For this we are initializing the counter to count no of Memory reads 
    */
    if(buf_size == 52)
    {
      mem_read_counter = 0;
    }
  }

#else/* FEATURE_HS_USB_SER4_PORT */

  HSU_SER_STAT_INC(ser_if_data_ptr->statistics.tx_total_items,1);

  hsu_trace_iface_set(ser_if_data_ptr->iface_type);
  hsu_trace_msg2_set(AL_SER_BASE_TX_WRITE__TRC);

  jresult = cdc_write_multiple(ser_if_data_ptr->instance_of_fd_used, 
    (void**)&buf_ptr, &buf_size, 1, ser_if_data_ptr->tx_info.tx_timeout);

#endif /* FEATURE_HS_USB_SER4_PORT */

  hsu_trace_iface_clear();
  hsu_trace_msg2_clear();

  switch(jresult)
  {
    case HSU_JSUCCESS:
    {
      /* Check if CONTIG BUF was used, if so, free packet ptr and assign buf 
       * ptr to app_ptr */
      if (contiguous_buf_used)
      {
        /* we insert the first DSM item from the chain into pending_chain_ptr.
         * the allocated contiguous buffer is stored in app_ptr so we can free
         * it in write_comp_cb */
        dsm_free_packet(&tx_ptr->pkt_ptr);
        tx_ptr->app_ptr = buf_ptr;
      }
      else
      {
	if (!ser_if_data_ptr->uses_dsm_app_ptr)
        {
          tx_ptr->app_ptr = NULL;
        } 
      }

      dsm_append(&ser_if_data_ptr->tx_info.pending_chain_ptr, &tx_ptr);

      if (ser_if_data_ptr->uses_dsm_app_ptr)
      {
        ser_if_data_ptr->tx_info.pending_chain_cnt += write_req_num;
        if (ser_if_data_ptr->port_id == AL_SER_PORT_DIAG)
        {
          HSU_ULOG_2(NO_MSG, SNPS_LOG, "INC, num %u, pending_chain_cnt %u", write_req_num, ser_if_data_ptr->tx_info.pending_chain_cnt);
        }
      }
      else
      {
        ++ser_if_data_ptr->tx_info.pending_chain_cnt;
        if (ser_if_data_ptr->port_id == AL_SER_PORT_DIAG)
        {
          HSU_ULOG_2(NO_MSG, SNPS_LOG, "INC, num %u, pending_chain_cnt %u", 1, ser_if_data_ptr->tx_info.pending_chain_cnt);
        }
      }

#ifdef FEATURE_HS_USB_SER_STATISTICS
      if (ser_if_data_ptr->tx_info.pending_chain_cnt > 
          ser_if_data_ptr->statistics.tx_max_num_pending)
      {
        ser_if_data_ptr->statistics.tx_max_num_pending = 
          ser_if_data_ptr->tx_info.pending_chain_cnt;
      }
#endif /* FEATURE_HS_USB_SER_STATISTICS */

      ser_if_data_ptr->tx_info.is_write_in_progress = TRUE;
    }
    break;

    case ESUSPENDED:
    {
      if (contiguous_buf_used)
      {
        hsu_al_dsm_utils_free_contiguous_buf(buf_ptr);
      }
#if 0 // For ESUSPENDED case, there is no need to recover
      if (ser_if_data_ptr->tx_info.recover_dropped_packet)
      {
        HSU_MSG_HIGH("base_write: cdc_write_multiple returned ESUSPENDED"
          " rescheduling to send upon resume");

        /* If remaining chain is empty, set the pending_chain to remaining chain */
        if (ser_if_data_ptr->tx_info.remaining_chain_ptr == NULL)
        {
          ser_if_data_ptr->tx_info.remaining_chain_ptr = tx_ptr;
        }
        else
        {
          /* Since remaining is not empty, the tx_ptr has to go front of
           * remaining chain */
          dsm_append(
            &tx_ptr, 
            &ser_if_data_ptr->tx_info.remaining_chain_ptr);

          ser_if_data_ptr->tx_info.remaining_chain_ptr = tx_ptr;
        }
      }
      else
#endif
      {
        HSU_ULOG(ERROR_MSG, DATA_LOG, "ESUSPENDED, dropping_all_pacet_tx_wmq");

        HSU_SER_STAT_INC(ser_if_data_ptr->statistics.tx_dropped_packet_cnt,1);

        dsm_free_packet(&tx_ptr);

        if (ser_if_data_ptr->tx_wmq_ptr && (FALSE == dsm_is_wm_empty(ser_if_data_ptr->tx_wmq_ptr)))
        {
          dsm_empty_queue(ser_if_data_ptr->tx_wmq_ptr);
        }
        
        if (ser_if_data_ptr->tx_info.remaining_chain_ptr)
        {
          dsm_free_packet(&ser_if_data_ptr->tx_info.remaining_chain_ptr);
          ser_if_data_ptr->tx_info.remaining_chain_ptr = NULL;
        }

        /* If there is no more pending data, check that is_write_in_progress is 
        ** clearedd and TX FLUSH COMPLETE callabck is invoked.
        ** Otherwise, wait till pending_chain_ptr is completed on tx_done.
        */
        if (ser_if_data_ptr->tx_info.pending_chain_ptr == NULL)
        {
          ser_if_data_ptr->tx_info.is_write_in_progress = FALSE;

          if (ser_if_data_ptr->tx_info.flush_tx_complete_cb != NULL)
          {
            ser_if_data_ptr->tx_info.flush_tx_complete_cb(ser_if_data_ptr);
            ser_if_data_ptr->tx_info.flush_tx_complete_cb = NULL;
            hsu_timer_cancel(&(ser_if_data_ptr->tx_info.flush_tx_timer));
            ser_if_data_ptr->tx_info.flush_tx_in_progress = FALSE;
          }           

          /* Since there are no more buffers, we can always clear the flag for
          ** write abort
          */
          ser_if_data_ptr->tx_info.is_write_abort_issued = FALSE;

          
          /* Handle closing of the port if the port was closing at same time 
           * before return */
          if (al_ser_base_handle_closing(ser_if_data_ptr))
          {
            return jresult;
          }
        }
      }
    }
    break;

    default:
    {
      HSU_ERR_FATAL("base_write: cdc_write_multiple returned %d",
        jresult, 0, 0);

      if (contiguous_buf_used)
      {
        hsu_al_dsm_utils_free_contiguous_buf(buf_ptr);
      }

      HSU_SER_STAT_INC(ser_if_data_ptr->statistics.tx_dropped_packet_cnt,1);

      dsm_free_packet(&tx_ptr);
    }
    break;
  }

  //if (ser_if_data_ptr->iface_type == DIAG_HSU_IFACE)
  //{
  //    HSU_ULOG_1(NO_MSG, DATA_LOG, "diag_hsu_iface, jresult %u", jresult);
  //}

  res = cdc_handle_tx_completion(ser_if_data_ptr->instance_of_fd_used);
  if (res != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR_1("cdc_handle_tx_completion failed. res=%d", res);
  }

  return jresult;
}



/*==============================================================================
FUNCTION hsu_al_ser_i_base_tx_set_handle_complete_timer

DESCRIPTION
This function sets the handle complete timer if needed

PARAMETERS
ser_if_data_ptr         : Pointer to serial I/F data struct

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
void hsu_al_ser_i_base_tx_set_handle_complete_timer
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
)
{  
  /* Set the handle complete timer */
  if ((ser_if_data_ptr->instance_enabled_by_cdc_fd) &&
      (ser_if_data_ptr->tx_info.tx_timeout == 0) &&
      (ser_if_data_ptr->tx_info.is_tx_complete_timer_scheduled == FALSE) &&
      ((ser_if_data_ptr->tx_info.pending_chain_cnt > 0) ||
       (ser_if_data_ptr->tx_info.remaining_chain_ptr != NULL) ||
       (ser_if_data_ptr->tx_wmq_ptr && 
       !dsm_is_wm_empty(ser_if_data_ptr->tx_wmq_ptr))))
  {
    hsu_timer_set(
      &(ser_if_data_ptr->tx_info.tx_complete_timer),
      ser_if_data_ptr->tx_info.tx_complete_timer_val,
      T_MSEC); 

    ser_if_data_ptr->tx_info.is_tx_complete_timer_scheduled = TRUE;
  }
}

/*==============================================================================
FUNCTION hsu_al_ser_i_base_rx_set_throttle_timer

DESCRIPTION
This function sets the  Rx data throttiling  timer if needed.
Timer is armed when there is a flow of consecutive smaller UL packets 

PARAMETERS
ser_if_data_ptr         : Pointer to serial I/F data struct

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
void hsu_al_ser_i_base_rx_set_throttle_timer
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
)
{  
  /* Set the handle complete timer */
  if ((ser_if_data_ptr->instance_enabled_by_cdc_fd) &&
      (ser_if_data_ptr->rx_info.rx_timeout == 0) &&
      (ser_if_data_ptr->rx_info.is_rx_throttle_timer_scheduled == FALSE))     
  {
    hsu_timer_set(
      &(ser_if_data_ptr->rx_info.rx_throttle_timer),
      ser_if_data_ptr->rx_info.rx_throttle_timer_val,
      T_MSEC); 

    ser_if_data_ptr->rx_info.is_rx_throttle_timer_scheduled = TRUE;
  }
}


/*==============================================================================

FUNCTION al_ser_base_rx_throttle_timer_cb

DESCRIPTION
RX Data throttling  timer callback, invoked upon timer expiration.

PARAMETERS
arg                     : Argument to timer callback.  This is actually a 
                          pointer to serial I/F data struct

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
static void al_ser_base_rx_throttle_timer_cb
(
 unsigned long          arg
)
{
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    ((hsu_al_ser_base_data_for_ser_if*)arg);

  HSU_ASSERT(ser_if_data_ptr != NULL);

  HSU_ULOG(NO_MSG, DATA_LOG, "DUN throttle");

  hsu_al_ser_base_i_rx_setup_read((hsu_al_ser_base_data_for_ser_if*)arg);
}



/*==============================================================================

FUNCTION al_ser_base_tx_complete_timer_cb

DESCRIPTION
TX handle complete timer callback, invoked upon timer expiration.

PARAMETERS
arg                     : Argument to timer callback.  This is actually a 
                          pointer to serial I/F data struct

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
static void al_ser_base_tx_complete_timer_cb
(
 unsigned long          arg
)
{
  al_ser_base_tx_check_complete((void*)arg);
}


/*==============================================================================

FUNCTION al_ser_base_tx_check_complete

DESCRIPTION
This function is executed in HSU_AL_TASK context and calls the stack 
cdc_handle_tx_completion function.
When this API is called, the stack will notify us on completed transfers. 
Needed when TX_TIMEOUT_MS is set to 0, otherwise when we reach the 
High WM the application stops calling the transmit function and usb_stack will 
not check for completed transfers and we will never go back to the Low WM.
The function also calls al_ser_base_tx_send_all_data to send the remaining
or enqueued data, since cdc_handle_tx_completion may release resources

PARAMETERS
arg                     : Argument to timer callback.  This is actually a 
                          pointer to serial I/F data struct

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
static void al_ser_base_tx_check_complete
(
  void                  *args
)
{
  jresult_t res;

  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    ((hsu_al_ser_base_data_for_ser_if*)args);

  jsafe_enter();

  /* The timer expired so we set the is_tx_complete_timer_scheduled to false 
   * to indicate that it should be set if there are resources */
  ser_if_data_ptr->tx_info.is_tx_complete_timer_scheduled = FALSE;

  if ((ser_if_data_ptr->instance_enabled_by_cdc_fd == FALSE)
      && (!ser_if_data_ptr->tx_info.recover_dropped_packet))
  {
    /* Instance not enabled, nothing to do  */
    jsafe_leave();
    return;
  }

  /* Check if the app is in closing process (or already closed): */
  if ((ser_if_data_ptr->num_of_aborts_issued_prior_to_close > 0) ||
      (hsu_al_ser_base_i_has_already_been_closed(ser_if_data_ptr)) == TRUE)
  {
    /* instance is closing, nothing to do... */
    jsafe_leave();
    return;
  }

  if (ser_if_data_ptr->debug_mode)
  {
    HSU_MSG_LOW("base_check_tx_complete: calling cdc_handle_tx_completion");
  }

  HSU_SER_STAT_INC(ser_if_data_ptr->statistics.tx_total_complete_cnt,1);

  /* Skip checking the TX completion if USB is not enumerated */
  if (ser_if_data_ptr->instance_enabled_by_cdc_fd)
  {
    res = cdc_handle_tx_completion(ser_if_data_ptr->instance_of_fd_used);
    if (res != HSU_JSUCCESS)
    {
      HSU_MSG_ERROR_1("cdc_handle_tx_completion failed. res=%d", res);
    }
  }

  /* send the remaining or enqueued data, if any */
  al_ser_base_tx_send_all_data(ser_if_data_ptr);

  hsu_al_ser_i_base_tx_set_handle_complete_timer(ser_if_data_ptr);

  
  jsafe_leave();
}

/*==============================================================================

FUNCTION al_ser_base_rx_done_with_flow_disabled

DESCRIPTION
Called by rx_done to handle the case of inbound flow disabled.

Inbound flow may be disabled due to one of the following:
1. Rx WMQ reaches High mark. hiwater_func_ptr callback gets invoked and the 
   application disables inbound flow.

2. number of DSM items in the pool gets low. application which previously 
   registered a callback with DSM_REG_MEM_EVENT_CB() disables inbound flow to
   avoid out of DSM items situation.

PARAMETERS
ser_if_data_ptr         : Pointer to serial I/F data struct
status                  : I/O status of the transfer
bytes_transferred       : Number of bytes transfered
req_num                 : Number of request pending

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
rx_info.num_buf_waiting is increased to indicate buffers are waiting to be 
sent to application. These buffers will be forwarded to upper layer upon 
inbound flow enable
==============================================================================*/
static void al_ser_base_rx_done_with_flow_disabled
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr,
  const cdc_iostatus_t *status,
  const juint32_t      *bytes_transferred,
  juint32_t             req_num
)
{
  uint32                i, cur_idx;
  dsm_item_type        *cur_dsm_item = ser_if_data_ptr->rx_info.dsm_chain_ptr;

  /* skip the already "waiting" items (from previous rx_done with flow 
   * disabled) */
  for(i = 0 ; i < ser_if_data_ptr->rx_info.num_buf_waiting; ++i)
  {
    cur_dsm_item = cur_dsm_item->pkt_ptr;
  }

  for (i = 0, cur_idx = ser_if_data_ptr->rx_info.num_buf_already_forwarded + 
                        ser_if_data_ptr->rx_info.num_buf_waiting;
       i < req_num;
       ++i, ++cur_idx, cur_dsm_item = cur_dsm_item->pkt_ptr)
  {
    ser_if_data_ptr->rx_info.buf_size_array[cur_idx] = 
      (uint16)bytes_transferred[i];
    ser_if_data_ptr->rx_info.buf_status_array[cur_idx] = status[i];
  }

  HSU_MSG_LOW_1("base_rx_done while inbound flow disabled. req_num=%d", req_num);

  ser_if_data_ptr->rx_info.num_buf_waiting += req_num;
}


/*==============================================================================

FUNCTION al_ser_base_rx_done_forward_to_app

DESCRIPTION
forward the received data to the application by calling its rx callback 
or by enqueing the data into the RX WMQ

PARAMETERS
ser_if_data_ptr         : Pointer to serial I/F data struct
rx_dsm_chain_ptr        : Pointer to Rx DSM chain

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
static void al_ser_base_rx_done_forward_to_app
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr,
  dsm_item_type        *rx_dsm_chain_ptr
)
{
  dsm_item_type        *rx_dsm_chain_ptr_for_app;
  rx_dsm_chain_ptr_for_app = rx_dsm_chain_ptr;

  /* Optimize the Large DSM items usage by replacing them with Small DSM items 
   * (based on the threshold) in the dsm_chain and drop ZLP (empty items) */
  hsu_al_dsm_utils_optimize_dsm_chain_item_alloc(
    &rx_dsm_chain_ptr_for_app,
    HSU_USE_SMALL_DSM_ITEMS_TREASHOLD);
  
  if (rx_dsm_chain_ptr_for_app)
  {
    if ((ser_if_data_ptr->port_id == AL_SER_PORT_MDM) ||
        (ser_if_data_ptr->port_id == AL_SER_PORT_MDM2))
    {
      if((rx_dsm_chain_ptr_for_app->used !=0 ) && (rx_dsm_chain_ptr_for_app->used < USB_MDM_DUN_RX_MIN_PKT_SIZE))
      {
        ser_if_data_ptr->statistics.rx_small_packet_cnt++;
        HSU_ULOG_2(NO_MSG, DATA_LOG, "DUN throttle+, cnt:%u sz:%u",ser_if_data_ptr->statistics.rx_small_packet_cnt,
                                                              rx_dsm_chain_ptr_for_app->used);
      }
      else
      {
        if(ser_if_data_ptr->statistics.rx_small_packet_cnt != 0)
        {
          ser_if_data_ptr->statistics.rx_small_packet_cnt--;  
          HSU_ULOG_2(NO_MSG, DATA_LOG, "DUN throttle-, cnt:%u sz:%u",ser_if_data_ptr->statistics.rx_small_packet_cnt,
                                                              rx_dsm_chain_ptr_for_app->used);
        }  
      }
    }
    
    if (ser_if_data_ptr->port_id == AL_SER_PORT_USB_SER4)
    {
      HSU_ULOG_1(NO_MSG, SER4_LOG, "ser4_recieved_from_ap, size %u", rx_dsm_chain_ptr_for_app->used);
      /* 16 bytes is a Memory read command packet for EFS sync we want to log the contents 
      */
      if(rx_dsm_chain_ptr_for_app->used == 16)
      {
        HSU_ULOG_DATA_4(SER4_LOG, rx_dsm_chain_ptr_for_app->data_ptr, rx_dsm_chain_ptr_for_app->used);
      }
    }

    if ((ser_if_data_ptr->rx_info.app_rx_cb_func != NULL)
     || (ser_if_data_ptr->rx_wmq_ptr != NULL))
    {
      /* Defer the handling to HSU AL TASK without Giant mutex locked 
      ** It will use DSM item's app_ptr to pass hsu_al_ecm_instance_info_type
      */
      rx_dsm_chain_ptr_for_app->app_ptr = ser_if_data_ptr;
      hsu_al_task_forward_ser_rx_dsm(rx_dsm_chain_ptr_for_app);
    }
    else /* No Rx callback AND no WMQ - Not supposed to happen... */
    {
      //HSU_MSG_ERROR("No Rx callback AND no Rx WMQ. Packet dropped");
      dsm_free_packet(&rx_dsm_chain_ptr_for_app);

      HSU_ULOG_1(ERROR_MSG, DATA_LOG, "no_rx_cb_and_no_rx_wmq, package_drop, data_ptr 0x%08X", (uint32)ser_if_data_ptr);

      HSU_SER_STAT_INC(ser_if_data_ptr->statistics.rx_transfer_error_cnt,1);
    }
  }
  ser_if_data_ptr->rx_info.dsm_chain_ptr = NULL;
}


/*===========================================================================

FUNCTION hsu_al_ser_base_forward_rx_to_app

DESCRIPTION
Function calls either the control Rx callback or enqueue in WMQ for the DSM 
item received.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS  
None

===========================================================================*/
void hsu_al_ser_base_forward_rx_to_app(dsm_item_type *dsm_ptr)
{
  /*
  ** It will use DSM item's app_ptr to pass hsu_al_ser_base_data_for_ser_if
  */
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr;
  sio_rx_func_ptr_type rx_func_ptr;
  dsm_watermark_type *rx_queue_ptr;

  HSU_ASSERT(dsm_ptr != NULL);

  ser_if_data_ptr = (hsu_al_ser_base_data_for_ser_if *)dsm_ptr->app_ptr;
  HSU_ASSERT(ser_if_data_ptr != NULL);

  rx_func_ptr = ser_if_data_ptr->rx_info.app_rx_cb_func;
  rx_queue_ptr = ser_if_data_ptr->rx_wmq_ptr;
  
  if (rx_func_ptr)
  {
    rx_func_ptr(&dsm_ptr);
  }
  else if(rx_queue_ptr)
  {
    dsm_enqueue(rx_queue_ptr, &dsm_ptr);
  }
  else
  {
    /* Dropping packet assuming that rx_func_ptr and rx_queue got cleared
     * due to port closing from the point HS-USB0 task enqeueuing
     * to HSU_AL_TASK invoking this function. */
    dsm_free_packet(&dsm_ptr);

    HSU_ULOG_1(NO_MSG, DATA_LOG, "hsu_status_error, data_ptr 0x%08X", (uint32)ser_if_data_ptr);

    HSU_SER_STAT_INC(ser_if_data_ptr->statistics.rx_transfer_error_cnt,1);
  }
} /* hsu_al_ser_base_forward_rx_to_app */


/*==============================================================================

FUNCTION al_ser_base_rx_done

DESCRIPTION
Rx Callback whenever the FD wishes to notify some buffers has been received
over the data class I/F.

PARAMETERS
status                  : Operation status
buffers                 : Ptr to buffer used
bytes_transferred       : Buffer size in bytes
req_num                 : Number of requests
ctx                     : Context passed in callback This is actuallp pointer 
                          to serial I/F data struct
due_to_flow_enabled     : This API call was made when flow was enabled, 
                          therefore Rx read setup must be done.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
static void al_ser_base_rx_done
(
  cdc_iostatus_t       *status,
  void                **buffers,
  juint32_t            *bytes_transferred,
  juint32_t             req_num,
  cdc_appctx_t          ctx,
  boolean               due_to_flow_enabled
)
{
  uint32                i, cur_idx;
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr = (hsu_al_ser_base_data_for_ser_if*)ctx;

  dsm_item_type        *cur_dsm_item  = ser_if_data_ptr->rx_info.dsm_chain_ptr;
  dsm_item_type        *prev_dsm_item = NULL;

  HSU_ASSERT(req_num > 0);

  if (ser_if_data_ptr->rx_info.inbound_flow_enabled == FALSE)
  {
    al_ser_base_rx_done_with_flow_disabled(
      ser_if_data_ptr, status, bytes_transferred, req_num);
    return;
  }

  /* iterate thru all items in the received DSM chain */
  for (i = 0, cur_idx = ser_if_data_ptr->rx_info.num_buf_already_forwarded;
       i < req_num;
       ++i, ++cur_idx, cur_dsm_item = cur_dsm_item->pkt_ptr)
  {
    switch(status[i])
    {
      case CDC_TRANSFER_OK:
      {
        cur_dsm_item->used = (uint16)bytes_transferred[i];

        if (bytes_transferred[i] != 0)
        {
          /* Invalidate cache before we further process the buffer to make 
           * sure whoever reads data gets it from the system memory instead 
           * of cache. */
          hsu_dcache_inval_region(
            cur_dsm_item->data_ptr, 
            ser_if_data_ptr->rx_info.dsm_item_size_rounded_down_to_512);

          if (ser_if_data_ptr->iface_type == MDM_HSU_IFACE)
          {
            hsu_dun_log(DUN__RX__LOG, buffers[i], bytes_transferred[i]);
          }
          
          HSU_SER_STAT_INC(ser_if_data_ptr->statistics.rx_total_bytes, bytes_transferred[i]);

           if (ser_if_data_ptr->stream_mode == SIO_DS_RAWDATA_MODE)
          {
            if (ser_if_data_ptr->rx_info.handle_rawdata_mode_cb == NULL)
            {
              HSU_MSG_ERROR_1("base_rx_done: "
                "RAWDATA_MODE not supported, fd instance: %u", 
                ser_if_data_ptr->instance_of_fd_used);
            }
            else
            {
              ser_if_data_ptr->rx_info.handle_rawdata_mode_cb(
                ser_if_data_ptr,
                cur_dsm_item,
                (char*)buffers[i], 
                bytes_transferred[i]);
            }
          }
        }
        else
        {
          HSU_SER_STAT_INC(ser_if_data_ptr->statistics.rx_zlp_count,1);
        }

        break;
      }

      case CDC_TRANSFER_CANCELLED:
      {
        HSU_MSG_HIGH("base_rx_done: CDC_TRANSFER_CANCELLED");
        /* Assuming all remaining items are CANCELLED */
        dsm_free_packet(&cur_dsm_item);
        if (prev_dsm_item == NULL)
        {
          ser_if_data_ptr->rx_info.dsm_chain_ptr = NULL;
        }
        else
        {
          prev_dsm_item->pkt_ptr = NULL;
        }

        if (!al_ser_base_handle_closing(ser_if_data_ptr))
        {
          if (ser_if_data_ptr->rx_info.dsm_chain_ptr != NULL)
          {
            /* all items before this canceled one are OK */
            al_ser_base_rx_done_forward_to_app(
              ser_if_data_ptr, 
              ser_if_data_ptr->rx_info.dsm_chain_ptr);
          }
          ser_if_data_ptr->rx_info.num_buf_already_forwarded = 0;
        }

        if (due_to_flow_enabled)
        {
          hsu_al_ser_base_i_rx_setup_read(ser_if_data_ptr);
        }
        else
        {
          /* No further calls to cdc_read */
          ser_if_data_ptr->rx_info.read_oper_is_in_progress = FALSE;
        }

      return ;
    }
    case CDC_TRANSFER_ERROR:
    {
      HSU_MSG_ERROR("base_rx_done: CDC_TRANSFER_ERROR");

      cur_dsm_item->used = 0;

      HSU_SER_STAT_INC(ser_if_data_ptr->statistics.rx_transfer_error_cnt,1);

      break;
    }

    default:
      HSU_MSG_ERROR_3("base_rx_done: unexpected status %d, i=%d, cur_idx=%d",
        status[i], i, cur_idx);
      break;

    }

    prev_dsm_item = cur_dsm_item;
  }

  /* Need to forward rx_info.dsm_chain_ptr[0 - (req_num-1)] to app */

  HSU_SER_STAT_INC(ser_if_data_ptr->statistics.rx_done_cnt,1);

  if (cur_dsm_item == NULL)
  {
    /* All buffers are exhausted */
    al_ser_base_rx_done_forward_to_app(
      ser_if_data_ptr, 
      ser_if_data_ptr->rx_info.dsm_chain_ptr);
    
    ser_if_data_ptr->rx_info.num_buf_already_forwarded = 0;
    hsu_al_ser_base_i_rx_setup_read(ser_if_data_ptr);
  }
  else
  {
    /* Split rx_info.dsm_chain_ptr into exhausted items (to be sent to app) 
     * and usable items which will remain as Rx buffers */
    prev_dsm_item->pkt_ptr = NULL;

    al_ser_base_rx_done_forward_to_app(
      ser_if_data_ptr, 
      ser_if_data_ptr->rx_info.dsm_chain_ptr);

    ser_if_data_ptr->rx_info.dsm_chain_ptr = cur_dsm_item;
    ser_if_data_ptr->rx_info.num_buf_already_forwarded += req_num;

    HSU_SER_STAT_INC(ser_if_data_ptr->statistics.rx_timeout_cnt,1);
  }
}

/*==============================================================================
FUNCTION hsu_al_ser_base_i_rx_done_from_flow_enable                   INTERNAL
==============================================================================*/
void hsu_al_ser_base_i_rx_done_from_flow_enable
(
  cdc_iostatus_t       *status,
  void                **buffers,
  juint32_t            *bytes_transferred,
  juint32_t             req_num,
  cdc_appctx_t          ctx
)
{
  al_ser_base_rx_done(
    status, buffers, bytes_transferred, req_num, ctx, TRUE); 
}


/*==============================================================================
FUNCTION hsu_al_ser_base_i_rx_setup_read                              INTERNAL
==============================================================================*/
void hsu_al_ser_base_i_rx_setup_read
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
)
{
  dsm_item_type        *rx_dsm_chain_ptr, *cur_dsm_item_ptr;
  jresult_t             cdc_read_res;
  uint32                i;
  uint32                rx_timeout;
  uint32                dsm_pool_item_size;

  /* NOTE: instance_of_fd_used is already known at this stage 
  (base_init was already called) */

  if (ser_if_data_ptr->rx_info.dsm_chain_ptr != NULL)
  {
    HSU_MSG_ERROR("hsu_al_ser_base_i_rx_setup_read: read was already setup");
    return;
  }

  /* Grab giant mutex prior to checking enable/disable: */
  jsafe_enter();

  if((ser_if_data_ptr->port_id == AL_SER_PORT_MDM) ||
     (ser_if_data_ptr->port_id == AL_SER_PORT_MDM2))
    
  {
    /* When there are cosecutive smaller packets, we hold the queuing of Rx buffers until the timer expires.
         * This is needed,  as data stream of smaller packets is leading to too many interrupts at the HW and 
         * ultimately  starving other low priority threads */
    if(ser_if_data_ptr->statistics.rx_small_packet_cnt == USB_MDM_DUN_MIN_SMALL_PKT_CNT)
    {
      HSU_ULOG_1(NO_MSG, DATA_LOG, "DUN throttle, cnt %u", ser_if_data_ptr->statistics.rx_small_packet_cnt);
      hsu_al_ser_i_base_rx_set_throttle_timer(ser_if_data_ptr);
      ser_if_data_ptr->statistics.rx_small_packet_cnt = 0;
    
      jsafe_leave();
      return;
    }
    else
    {
     /* The timer could have expired so set the is_rx_throttle_timer_scheduled to false 
           * to indicate that it should be set if there are resources */
      ser_if_data_ptr->rx_info.is_rx_throttle_timer_scheduled = FALSE; 
    }    
  }      

  dsm_pool_item_size = DSM_POOL_ITEM_SIZE(ser_if_data_ptr->dsm_pool_id);

  /* Here is safe to check if enabled/disabled - as only the FD can modify 
   * these values - and right now it does not have the giant mutex*/
  if (ser_if_data_ptr->instance_enabled_by_cdc_fd == TRUE)
  {
    if (hsu_al_dsm_utils_pool_is_not_available(
          ser_if_data_ptr->dsm_pool_id))
    {
      ser_if_data_ptr->rx_info.is_pending = TRUE;
      
      HSU_ULOG_ENUM_1(HIGH_MSG, DATA_LOG, "dsm_pool_not_available, dsm_pool_id %u", 
        ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2, 
        ser_if_data_ptr->dsm_pool_id);
//      HSU_MSG_HIGH("base_i_rx_setup_read: read is suspended");
      jsafe_leave();
      return;
    }

    rx_dsm_chain_ptr = hsu_al_dsm_utils_generate_dsm_chain(
      ser_if_data_ptr->dsm_pool_id, 
      ser_if_data_ptr->rx_info.array_size * dsm_pool_item_size);

    if (rx_dsm_chain_ptr == NULL)
    {
      ser_if_data_ptr->rx_info.is_pending = TRUE;
      HSU_MSG_HIGH("base_i_setup_read: read is suspended, DSM pool allocation failed");
      jsafe_leave();
      return;
    }

    cur_dsm_item_ptr = rx_dsm_chain_ptr;
	
	if (ser_if_data_ptr->rx_info.array_size > HS_USB_SER_MAX_RX_ARRAY_CNT ||
          ser_if_data_ptr->rx_info.array_size == 0)
    {
      HSU_ERR_FATAL("base_open: Invalid requested Rx buffer array size",
      ser_if_data_ptr->rx_info.array_size,
      HS_USB_SER_MAX_RX_ARRAY_CNT,
      0);
    }

    for (i=0; i<ser_if_data_ptr->rx_info.array_size; ++i)
    {
      HSU_ASSERT(cur_dsm_item_ptr != NULL);

      ser_if_data_ptr->rx_info.buf_array[i] = cur_dsm_item_ptr->data_ptr;
      ser_if_data_ptr->rx_info.buf_size_array[i] = 
        ser_if_data_ptr->rx_info.dsm_item_size_rounded_down_to_512;

      cur_dsm_item_ptr = cur_dsm_item_ptr->pkt_ptr;

      hsu_dcache_inval_region(
        ser_if_data_ptr->rx_info.buf_array[i], 
        ser_if_data_ptr->rx_info.dsm_item_size_rounded_down_to_512);
    }

    rx_timeout = ((ser_if_data_ptr->rx_info.array_size == 1) ? 
      0 : ser_if_data_ptr->rx_info.rx_timeout);

    hsu_trace_iface_set(ser_if_data_ptr->iface_type);
    hsu_trace_msg2_set(HSU_AL_SER_BASE_SETUP_READ__TRC);

    cdc_read_res = cdc_read_multiple(
      ser_if_data_ptr->instance_of_fd_used, 
      (void**)ser_if_data_ptr->rx_info.buf_array,
      ser_if_data_ptr->rx_info.buf_size_array,
      ser_if_data_ptr->rx_info.array_size, rx_timeout);

    hsu_trace_iface_clear();
    hsu_trace_msg2_clear();

    switch(cdc_read_res)
    {
      case ESUSPENDED:
        HSU_MSG_HIGH("cdc_read_multiple returned ESUSPENDED");
        /* a new read operation will be invoked upon resume */
        dsm_free_packet(&rx_dsm_chain_ptr);
        break;

      case HSU_JSUCCESS:
        ser_if_data_ptr->rx_info.read_oper_is_in_progress = TRUE;
        ser_if_data_ptr->rx_info.dsm_chain_ptr = rx_dsm_chain_ptr;
        break;

      default:
        HSU_MSG_ERROR_1("cdc_read_multiple failed: %d", cdc_read_res);
        dsm_free_packet(&rx_dsm_chain_ptr);
    }
  }
  /* if instance_enabled_by_cdc_fd==FALSE then a read operation will be 
   * invoked when cdc fd get enabled */

  jsafe_leave();
}

/*==============================================================================
FUNCTION hsu_al_ser_base_i_close_clear_data                           INTERNAL
==============================================================================*/
void hsu_al_ser_base_i_close_clear_data
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
)
{
  /* Open/Close common items, close should clear following items */
  ser_if_data_ptr->dsm_pool_id                      = 0;
  ser_if_data_ptr->stream_id                        = SIO_NO_STREAM_ID;
  ser_if_data_ptr->num_of_aborts_issued_prior_to_close = 0;
  ser_if_data_ptr->close_complete_cb                = NULL;

  ser_if_data_ptr->ioctl_flow_ctl_mirror.rx_flow    = SIO_MAX_FLOW;
  ser_if_data_ptr->ioctl_flow_ctl_mirror.tx_flow    = SIO_MAX_FLOW;

  ser_if_data_ptr->tx_info.tx_max_pending_chain_cnt = 0;
  ser_if_data_ptr->tx_info.tx_complete_timer_val    = 0;
  ser_if_data_ptr->tx_info.tx_timeout               = 0;
  ser_if_data_ptr->tx_info.is_write_in_progress     = FALSE;
  ser_if_data_ptr->tx_info.flush_tx_complete_cb     = NULL;
  
  ser_if_data_ptr->rx_info.app_rx_cb_func           = NULL;
  ser_if_data_ptr->rx_info.rx_buff_size_for_read    = 0;
  ser_if_data_ptr->rx_info.read_oper_is_in_progress = FALSE;
  ser_if_data_ptr->rx_info.rx_timeout               = 0;
  ser_if_data_ptr->rx_info.rx_throttle_timer_val    = 0;
  ser_if_data_ptr->statistics.rx_small_packet_cnt   = 0;

  /* Clear the DSM items held by the serial data structure */
  al_ser_base_clear_dsm_items_from_ser_data(ser_if_data_ptr);
}


/*==============================================================================
FUNCTION hsu_al_ser_base_i_open_set_data                              INTERNAL
==============================================================================*/
void hsu_al_ser_base_i_open_set_data
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr, 
  sio_port_id_type      port_id,
  sio_stream_id_type    stream_id,
  sio_stream_mode_type  stream_mode,
  dsm_watermark_type   *tx_wmq_ptr,
  dsm_watermark_type   *rx_wmq_ptr,
  sio_rx_func_ptr_type  app_rx_cb_func,
  dsm_mempool_id_enum_type dsm_pool_id,
  juint8_t              rx_buff_cnt,
  uint32                rx_timout,
  uint32                tx_max_pending_chain_cnt,
  uint32                tx_timeout,
  uint32                tx_complete_timer_val,
  sio_flow_ctl_type     rx_flow_ctl,
  sio_flow_ctl_type     tx_flow_ctl,
  void                 *arg_cb
)
{
  HSU_ASSERT(ser_if_data_ptr != NULL);

  /* Workaround for receiving an uninitialized value in rx_wmq_ptr */
  /* If we get a valid callback function use it and ignore wmq_ptr */
  if (app_rx_cb_func != NULL && rx_wmq_ptr != NULL)
  {
    HSU_MSG_ERROR("base_open_i_open_set_data: Application passed both rx "
                  "watermark queue and rx callback function. Ignoring watermark "
                  "queue.");
    rx_wmq_ptr = NULL;
  }


  ser_if_data_ptr->dsm_pool_id                      = dsm_pool_id;
  ser_if_data_ptr->port_id                          = port_id;
  ser_if_data_ptr->iface_type                       = 
    hsu_conf_sel_get_iface_type_from_port_id(port_id);
  
  ser_if_data_ptr->stream_id                        = stream_id;
  ser_if_data_ptr->num_of_aborts_issued_prior_to_close = 0;
  ser_if_data_ptr->close_complete_cb                = NULL;

  ser_if_data_ptr->ioctl_flow_ctl_mirror.rx_flow    = rx_flow_ctl;
  ser_if_data_ptr->ioctl_flow_ctl_mirror.tx_flow    = tx_flow_ctl;

  ser_if_data_ptr->tx_info.tx_max_pending_chain_cnt = tx_max_pending_chain_cnt;
  ser_if_data_ptr->tx_info.tx_complete_timer_val    = tx_complete_timer_val;
  ser_if_data_ptr->tx_info.tx_timeout               = tx_timeout;
  ser_if_data_ptr->tx_info.is_write_in_progress     = FALSE;
  ser_if_data_ptr->tx_info.flush_tx_complete_cb     = NULL;
  
  ser_if_data_ptr->rx_info.app_rx_cb_func           = app_rx_cb_func;
  ser_if_data_ptr->rx_info.rx_throttle_timer_val    = USB_MDM_RX_THROTTLE_TIMEOUT_VAL_MS;

  ser_if_data_ptr->rx_info.dsm_item_size_rounded_down_to_512 = 
    HSU_AL_SER_BASE_ROUND_DOWN_TO_512(DSM_POOL_ITEM_SIZE(dsm_pool_id));

  // Initialize rx_buff_size_for_read to be max_buf_size.
  ser_if_data_ptr->rx_info.max_buf_size             =
  // rx_buff_size_for_read can be change by IOCTL to any valid value less than 2k but > 512.
  ser_if_data_ptr->rx_info.rx_buff_size_for_read    =
    rx_buff_cnt * ser_if_data_ptr->rx_info.dsm_item_size_rounded_down_to_512;

  ser_if_data_ptr->rx_info.read_oper_is_in_progress = FALSE;
  ser_if_data_ptr->rx_info.rx_timeout               = rx_timout;
  ser_if_data_ptr->arg_cb                           = arg_cb;

  /* Clear the DSM items held by the serial data structure */
  al_ser_base_clear_dsm_items_from_ser_data(ser_if_data_ptr);
  
  if (ser_if_data_ptr->tx_wmq_ptr != NULL)
  {
    dsm_empty_queue(ser_if_data_ptr->tx_wmq_ptr);
  }
  ser_if_data_ptr->tx_wmq_ptr = tx_wmq_ptr;

  if (ser_if_data_ptr->rx_wmq_ptr != NULL)
  {
    dsm_empty_queue(ser_if_data_ptr->rx_wmq_ptr);
  }
  ser_if_data_ptr->rx_wmq_ptr = rx_wmq_ptr;
    
  /* Save the stream mode, do not clear the stream mode on close */
  ser_if_data_ptr->stream_mode = stream_mode;

  /* Create timer only if the Tx time out value is 0, if Tx complete
   * timer value does not equal to 0, and if timer has not been created 
   * in prior (check for NULL(0)). */
  if ((ser_if_data_ptr->tx_info.tx_timeout == 0) &&
      (ser_if_data_ptr->tx_info.tx_complete_timer_val != 0) &&
      (ser_if_data_ptr->tx_info.tx_complete_timer == NULL))
  {
    /* Tx handle complete timer */
    ser_if_data_ptr->tx_info.tx_complete_timer = hsu_timer_init(
      HSU_TIMER_CID__SER_TX_COMPLETE,
      HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
      al_ser_base_tx_complete_timer_cb,
      (unsigned long)ser_if_data_ptr);
  }

  /* Create timer only if the Rx time out value is 0, if Rx complete
   * timer value does not equal to 0, and if timer has not been created 
   * in prior (check for NULL(0)). */
  if((ser_if_data_ptr->iface_type == MDM_HSU_IFACE) ||
    (ser_if_data_ptr->iface_type == MDM2_HSU_IFACE))
  {
    ser_if_data_ptr->statistics.rx_small_packet_cnt = 0;
    if ((ser_if_data_ptr->rx_info.rx_throttle_timer_val != 0) &&
        (ser_if_data_ptr->rx_info.rx_throttle_timer == NULL))
    {
      /* Rx Data throttle timer */
      ser_if_data_ptr->rx_info.rx_throttle_timer = hsu_timer_init(
        HSU_TIMER_CID__SER_RX_THROTTLE,
        HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
        al_ser_base_rx_throttle_timer_cb,
        (unsigned long)ser_if_data_ptr);
    }
  }  

  /* Enable the Rx flow */
  ser_if_data_ptr->rx_info.inbound_flow_enabled       = TRUE;
  ser_if_data_ptr->rx_info.num_buf_waiting            = 0;
  ser_if_data_ptr->rx_info.num_buf_already_forwarded  = 0;

  ser_if_data_ptr->rx_info.array_size = rx_buff_cnt;

  /* Array size must be smaller than equal to the pre-allocated
   * memory for the arrays, or array index out of bound will occur */
  if (ser_if_data_ptr->rx_info.array_size > HS_USB_SER_MAX_RX_ARRAY_CNT ||
        ser_if_data_ptr->rx_info.array_size == 0)
  {
      HSU_ERR_FATAL("base_open: Invalid requested Rx buffer array size", 
      ser_if_data_ptr->rx_info.array_size,
      HS_USB_SER_MAX_RX_ARRAY_CNT,
      0);
   }

#ifdef FEATURE_HS_USB_SER_STATISTICS
  /* Only close the statistic for open */
  memset((void*)&ser_if_data_ptr->statistics, 0, 
    sizeof(ser_if_data_ptr->statistics));
#endif /* FEATURE_HS_USB_SER_STATISTICS */
}


/*==============================================================================

FUNCTION al_ser_base_clear_dsm_items_from_ser_data

DESCRIPTION
Check if al_ser_acm is getting closed

PARAMETERS
ser_if_data_ptr         : Pointer to serial I/F data struct

DEPENDENCIES
Assumes that parameter is not NULL

RETURN VALUE
TRUE if al_ser_acm is getting closed

SIDE EFFECTS
None
==============================================================================*/
static void al_ser_base_clear_dsm_items_from_ser_data
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
)
{  
  if (ser_if_data_ptr->tx_info.remaining_chain_ptr != NULL)
  {
    HSU_MSG_HIGH("base_clear_dsm_items_from_ser_data: remaining chain exist");

    dsm_free_packet(&ser_if_data_ptr->tx_info.remaining_chain_ptr);
  }
  ser_if_data_ptr->tx_info.remaining_chain_ptr = 0;
    
  if (ser_if_data_ptr->tx_wmq_ptr != NULL)
  {
    HSU_MSG_HIGH("base_clear_dsm_items_from_ser_data: tx_wmq_ptr is not empty");

    dsm_empty_queue(ser_if_data_ptr->tx_wmq_ptr);
  }
  ser_if_data_ptr->tx_wmq_ptr = NULL;
  
  if (ser_if_data_ptr->rx_wmq_ptr != NULL)
  {
    HSU_MSG_HIGH("base_clear_dsm_items_from_ser_data: rx_wmq_ptr is not empty");

    dsm_empty_queue(ser_if_data_ptr->rx_wmq_ptr);
  }  
  ser_if_data_ptr->rx_wmq_ptr = NULL;
}


/*==============================================================================

FUNCTION al_ser_base_handle_closing

DESCRIPTION
Check if al_ser_acm is getting closed

PARAMETERS
ser_if_data_ptr         : Pointer to serial I/F data struct

DEPENDENCIES
None

RETURN VALUE
TRUE if al_ser_acm is getting closed

SIDE EFFECTS
None
==============================================================================*/
static boolean al_ser_base_handle_closing
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
)
{
  /* In case abort read/write was issued (e.g. close), and the FD notified us 
   * it aborted the read/write, then if no more aborts are needed, finish the 
   * call to close */
  if (ser_if_data_ptr->num_of_aborts_issued_prior_to_close > 0)
  {
    HSU_MSG_HIGH("base_handle_closing: aborts were issued");

    --ser_if_data_ptr->num_of_aborts_issued_prior_to_close;

    if (ser_if_data_ptr->num_of_aborts_issued_prior_to_close == 0)
    {
      /* Check if close has already been called:
         This is in order to prevent the following scenario:
         1. Close issued (with no callback) - abort read & write issued
         2. hsu_al_ser_base_i_rx_done called, sees num_of_aborts > 0 
         3. It decreases it by one, but then the gather DMOV callback 
            interrupts.
         4. Gather callback decreases num_of_aborts to zero and calls 
            hsu_al_ser_base_i_internal_close 
         5. Now we return here, check 
            app_data_ptr->num_of_aborts_issued_prior_to_close == 0 
         6. call close again */

      /* Note: If hsu_al_ser_base_i_has_already_been_closed returns FALSE, 
       * this also means DMOV callback cannot be called now (because 
       * num_of_aborts_issued_prior_to_close == 0) */
      if (hsu_al_ser_base_i_has_already_been_closed(ser_if_data_ptr) == FALSE)
      {
        hsu_al_ser_base_i_internal_close(ser_if_data_ptr);
      }
    }

    return TRUE;
  }

  return FALSE;
}


/*===========================================================================

FUNCTION      hsu_al_ser_base_pool_many_cb

DESCRIPTION
This function invokes the appropriate actions when the memory pool arrives 
to DSM_MEM_LEVEL_USB_MANY on DSM_MEM_OP_FREE.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_ser_base_pool_many_cb
(
  dsm_mempool_id_enum_type pool_id, 
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
)
{
  if (pool_id != ser_if_data_ptr->dsm_pool_id)
    return;
  
  if (ser_if_data_ptr->rx_info.is_pending)
  {   
    ser_if_data_ptr->rx_info.is_pending = FALSE;
    //HSU_MSG_HIGH("hsu_al_ser_base_pool_many_cb: read is resumed");
    HSU_ULOG_ENUM_1(HIGH_MSG, DATA_LOG, "dsm_pool_many_cb_setup_read, dsm_pool_id %u", 
      ser_if_data_ptr->iface_type, HSU_IFACE_OFFSET2,
      ser_if_data_ptr->dsm_pool_id);

    
    hsu_al_ser_base_i_rx_setup_read(ser_if_data_ptr);
  }
}
