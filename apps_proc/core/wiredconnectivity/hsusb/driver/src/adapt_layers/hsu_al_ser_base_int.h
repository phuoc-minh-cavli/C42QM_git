#ifndef _HSU_AL_SER_BASE_INT_H_
#define _HSU_AL_SER_BASE_INT_H_

/*==============================================================================

Serial Functions (Modem/DIAG/NMEA) Adaptation Layer for High Speed USB
"Base Class" Header File

GENERAL DESCRIPTION
Contains data structures and API for the "base class"
of the various serial I/Fs which are used in the serial functions' 
adaptation layer.

EXTERNALIZED FUNCTIONS
hsu_al_ser_base_init
hsu_al_ser_base_open
hsu_al_ser_base_close
hsu_al_ser_base_transmit
hsu_al_ser_base_flush_tx
hsu_al_ser_base_ioctl

Copyright (c) 2006 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_ser_base_int.h#1 $
$DateTime: 2020/01/30 22:49:35 $


when     who  what, where, why
-------- ---  ---------------------------------------------------------
07/30/06 esh  First revision of file  
==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h" /* For FEATURE_HSU_TEST to work */

#include "hsu_al_ser_base.h"
#include "hsu_al_ser_i.h"

#include <sio.h>
#include "dsm.h"
#include "uw_device_cdc.h"
#include "jtypes_int.h"
#include "hsu_timer.h"
#include "hsu_conf_sel_types_int.h"

/*==============================================================================
Constants and Macros
==============================================================================*/

#ifdef FEATURE_HSU_TEST

#define HSU_AL_SER_BASE_UT_CDC_STUB_ACKS_WHEN_TRANSMIT_BEFORE_DSM_ENQUEUE (0x0001)
#define HSU_AL_SER_BASE_UT_CDC_STUB_ACKS_WHEN_TRANSMIT_BEFORE_2ND_DMOV_BUSY_CHK (0x0002)
#define HSU_AL_SER_BASE_UT_CDC_STUB_ACKS_WHEN_CLOSE_ISSUED_BEFORE_CRIT_SECT (0x0004)
#define HSU_AL_SER_BASE_UT_CDC_STUB_ACKS_WHEN_CLOSE_ISSUED_AFTER_CRIT_SECT (0x0008)

#define HSU_AL_SER_BASE_UT_STUB_INVOKES_ENABLE_WHEN_OPEN_ISSUED_DURING_PARAMS (0x0010)
#define HSU_AL_SER_BASE_UT_STUB_INVOKES_DISABLE_WHEN_OPEN_ISSUED (0x0020)
#define HSU_AL_SER_BASE_UT_STUB_INVOKES_ENABLE_WHEN_OPEN_ISSUED_AFTER_CHK (0x0040)

#define HSU_AL_SER_BASE_UT_STUB_INVOKES_DISABLE_WHEN_CLOSE_ISSUED (0x0080)
#define HSU_AL_SER_BASE_UT_STUB_INVOKES_ABORT_READ_WHEN_CLOSE_ISSUED (0x0100)

#endif /* FEATURE_HSU_TEST */

/*==============================================================================
Forward Declarations
==============================================================================*/
struct hsu_al_ser_base_data_for_ser_if_st;

/*==============================================================================
Typedefs
==============================================================================*/
typedef void (*hsu_al_ser_base_op_complete_cb)
  (void*);
    /* Used for completion of close / flush tx operations */

typedef void (*hsu_al_ser_rx_handle_rawdata_cb)
  (struct hsu_al_ser_base_data_for_ser_if_st*, 
   dsm_item_type*, 
   char*,
   juint32_t);

/* TX information for HS-USB serial interface */
typedef struct
{
  /* Partial DSM chain to Tx, in case in a previous transfer only part of a 
   * DSM chain was taken (the remaining part is in this field) */
  dsm_item_type        *remaining_chain_ptr;

  /* DSM chain of pending Tx requests. append this chain each time there is a
   * call to cdc_write_multiple() */
  dsm_item_type        *pending_chain_ptr;

  /* Number of DSM items in pending_chain_ptr */
  uint32                pending_chain_cnt;

  /* Maximum number of pending chain count */
  uint32                tx_max_pending_chain_cnt;

  /* TX timeout value (for  multiple write) in milliseconds */
  uint32                tx_timeout;

  /* Timer struct for Tx handle complete timer */
  hsu_timer_handle_type tx_complete_timer;

  /* Interval in milliseconds that the complete timer should run */
  uint32                tx_complete_timer_val;

  /* A flag to indicate if the tx_complete_timer is set or not. */
  boolean               is_tx_complete_timer_scheduled;

  /* Indicator whether or not CDC FD is currently busy with cdc_write.
   * In such case, the I/F must abort the write and free the items that was 
   * pending for write
   */
  boolean               is_write_in_progress;

  /* This flag is set to TRUE when cdc_abort_write was issued following a 
   * call to flush_tx */
  boolean               is_write_abort_issued ;

  /* Flag to notify the lower layer that code stack is in send all loop.
   * Because it is in loop, we should call hsu_al_task_enqueue() function
   * in the context to prevent running out of task queue.
   */
  boolean               is_send_all_in_progress;

  /* This timer is set when flush_tx is issued. 
   * It is used for two purposes: 
   * (1) it expires after AL_SER_BASE_FLUSH_TX_INTERVAL_MS
   *     and then we issue cdc_handle... 
   *    (see al_ser_base_flush_tx_timer_cb for details).
   * (2) When this timer expires, and AL_SER_BASE_FLUSH_TX_TIMEOUT_MS has 
   *     passed:  if flush_tx_cb was not issued yet, TX WMQ will be emptied 
   *     and cdc_abort_write will be issued */
  hsu_timer_handle_type flush_tx_timer;

  /* Flush Tx callback used by this I/F */
  hsu_al_ser_base_op_complete_cb flush_tx_complete_cb;

  /* How many milis has the flush been going on.  Valid only during flush!
   * counts to AL_SER_BASE_FLUSH_TX_TIMEOUT_MS and then flush is aborted*/
  uint32                flush_tx_duration;

  /* Flag to state that the flush_tx was called & hasn't ended yet */
  boolean               flush_tx_in_progress;

  /* Flag to re-sending dropped packets due to suspend or cancellation TDs */
  boolean               recover_dropped_packet;

  /* Flag to drop packet on suspend */
  boolean               drop_packet_on_suspend; 
} hsu_al_ser_base_tx_info_struct;


 /* RX information for HS-USB serial interface */
typedef struct  
{

  /* Indication whether or not inbound flow is enabled, which means more read 
   * operations can be done. If disabled, no further reads should be made */
  boolean               inbound_flow_enabled;

  /* When inbound flow is disabled num_buf_waiting will hold number of DSM 
   * items waiting to be forwarded to the application upon enabling inbound 
   * flow back */
  uint32                num_buf_waiting;

  /* Current DSM chain used for Rx operation */
  dsm_item_type        *dsm_chain_ptr;

  /* DSM chain rounded down to 512 bytes */
  juint32_t             dsm_item_size_rounded_down_to_512;

  /* Maximum RX buffer size */
  juint32_t             max_buf_size;

  /* Array of buffer for Rx */
  uint8                *buf_array[HS_USB_SER_MAX_RX_ARRAY_CNT];

  /* Array of buffer size for each Rx buffer */
  uint32                buf_size_array[HS_USB_SER_MAX_RX_ARRAY_CNT];

  /* Array of status for each Rx buffer */
  cdc_iostatus_t        buf_status_array[HS_USB_SER_MAX_RX_ARRAY_CNT];

  /* Number of Rx buffers given in each cdc_read_multiple call */
  uint32                array_size;

  /* Number of Rx buffers already forwarded to the application */
  uint32                num_buf_already_forwarded;

  /* rx timeout value (for  multiple read) in milliseconds */
  uint32                rx_timeout;
  
  /* Indication whether there is a "read" operation in progress.
  This means either a call for the FD's read was made, or that
  DMOV is in the middle of a scatter operation which will forward the 
  results of the read operation to the app */
  boolean               read_oper_is_in_progress;

  // Size of buffer to allocate from Rx pools
  // rx_buff_size_for_read can be changed by IOCTL to any valid value.
  // Range of valid values: 512 to DSMI_DS_LARGE_ITEM_SIZ
  juint32_t             rx_buff_size_for_read;

  /* Determines whether a transfer is pending */
  boolean               is_pending;

  /* Rx callback used by this I/F */
  sio_rx_func_ptr_type  app_rx_cb_func;

  hsu_al_ser_rx_handle_rawdata_cb handle_rawdata_mode_cb;

  /* Timer struct for Rx Data throttling  timer */
  hsu_timer_handle_type rx_throttle_timer;

  /* A flag to indicate if the rx_throttle_timer is set or not. */
  boolean               is_rx_throttle_timer_scheduled;

  /* Interval in milliseconds that the throttle timer should run */
  uint32                rx_throttle_timer_val;
} hsu_al_ser_base_rx_info_struct;

#ifdef FEATURE_HS_USB_SER_STATISTICS
/* This MACRO is used to increment the USB serial statistic variables */
#define HSU_SER_STAT_INC(var, inc) ((var)+=(inc))
#else
#define HSU_SER_STAT_INC(var, inc)
#endif

#ifdef FEATURE_HS_USB_SER_STATISTICS
/* Statistic information for HS-USB serial interface */
typedef struct  
{
  uint32                rx_zlp_count;
  uint32                rx_total_bytes;
  uint32                rx_done_cnt;
  uint32                rx_timeout_cnt;
  uint32                rx_transfer_error_cnt;
  uint32                rx_small_packet_cnt;
  uint32                tx_total_transmit_cnt;
  uint32                tx_total_bytes;
  uint32                tx_max_num_pending;
  uint32                tx_dropped_packet_cnt;
  uint32                tx_transfer_error_cnt;
  uint32                tx_contiguous_buf_usage_cnt;
  uint32                tx_total_dequeue;
  uint32                tx_total_items;
  uint32                tx_total_complete_cnt;
} hsu_al_ser_base_statistics_struct;
#endif /* FEATURE_HS_USB_SER_STATISTICS */

typedef struct hsu_al_ser_base_data_for_ser_if_st
{

  uint32                            data_per_ser_if_magic_num;
    /* "Magic Number" to allow pointer verification in callbacks */

  sio_port_id_type   port_id;
  /* port id used by this I/F */

  hsu_iface_index_enum_type         iface_type;
    /* USB interface type */

  sio_stream_id_type                stream_id;
    /* stream id used by this I/F */

  sio_stream_mode_type              stream_mode;
  /* The stream mode given by the app (autodetect or raw data)*/

  dsm_watermark_type*               tx_wmq_ptr;
    /* Tx WMQ used by this I/F */

  dsm_watermark_type*               rx_wmq_ptr;
    /* Rx WMQ used by this I/F */

  hsu_al_ser_base_op_complete_cb    close_complete_cb;
    /* Close callback used by this I/F */

  cdc_handle_t                      instance_of_fd_used;
    /* Handle of the FD used by this I/F */


  /* TODO: Obsolete type - change to dsm_mempool_id_type 
  when moving to targets >= 7500 */
  dsm_mempool_id_enum_type          dsm_pool_id;
    /* Pool ID from which DSM items would be allocated */

  /* "Reference Count" of the number of aborts */
  uint8                             num_of_aborts_issued_prior_to_close;

  sio_ioctl_flow_ctl_type           ioctl_flow_ctl_mirror;
    /* "Mirror image" of the given I/O ctl flow type, so that the app
    will be under the impression its set flow ioctl got through */

  /* Indication whether this instance is enabled or not by the FD */
  boolean                           instance_enabled_by_cdc_fd;

  boolean                           overcome_1_2_byte_hw_crc_err;



  /* Indicates whether simulating HSU over UART (for serial I/Fs (e.g. DIAG)) or not. */
  /* When running unit tests using these serial I/Fs, 
  it is not desired to have this simulation, so the unit tests would turn this off. */
  boolean                           simulating_hsu_over_uart;

  uint32                            unit_test_mask;
    /* Mask of types of testing wish to be done:
      e.g.: mask to cause the CDC FD to occur in a critical section, at a specific 
      location.
      TODO: If there are more than 30 interesting places, consider not using a mask - 
      but rather values.
    */

  boolean debug_mode;
  /* Flag for additional debugging messages */

  void*   arg_cb;
  /* Callbacks' argument */

  hsu_al_ser_base_tx_info_struct tx_info;
  /* Tx information */

  hsu_al_ser_base_rx_info_struct rx_info;
  /* Rx information */

  boolean uses_dsm_app_ptr;

#ifdef FEATURE_HS_USB_SER_STATISTICS
  hsu_al_ser_base_statistics_struct statistics;
  /* Statitical data for analyzing error and throughput */
#endif /* FEATURE_HS_USB_SER_STATISTICS */
} hsu_al_ser_base_data_for_ser_if; 
    /* Data common to all Serial I/Fs */

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_ser_base_init

DESCRIPTION
Initializes the given serial I/F data structure for later usage.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_al_ser_base_init
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
                                              /* Pointer to serial I/F data 
                                               struct                      */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_open

DESCRIPTION
This function opens a stream across a serial interface on the CDC FD.  

DEPENDENCIES

RETURN VALUE
TRUE - Operation Successful. FALSE - Otherwise

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_ser_base_open
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,    
                                             /* Pointer to serial I/F data 
                                               struct                      */
  const sio_open_type* open_ptr,             /* Configurations of stream    */
  dsm_mempool_id_enum_type pool_id,          /* Pool ID from where to take 
                                               DSM items                   */
  juint8_t rx_buff_cnt,
  uint32 rx_timeout,
  uint32 tx_max_pending_chain_cnt,
  uint32 tx_timeout,
  uint32 tx_complete_timer_val,
  void*   arg_cb                            /* Callbacks' argument         */

);

/*===========================================================================

FUNCTION hsu_al_ser_base_close

DESCRIPTION
This function closes an open stream of a serial interface above the CDC FD.
It allows an optional callback to notify caller when all the bookkeeping 
needed for closing the stream (flushing Tx, aborting Rx, etc.) 
has been completed.

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_base_close
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,    
                                            /* Pointer to serial I/F data 
                                               struct                      */
  hsu_al_ser_base_op_complete_cb close_cb,  /* Callback to be invoked 
                                             when close has been completed */
  boolean abort_io_operations_immediately   /* Whether or not to abort all 
                                             read/write operations 
                                             immediately or wait until all 
                                             Tx data is flushed, followed 
                                             by an abort read              */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_transmit

DESCRIPTION
This function will transmit a packet or if transmitter is busy, schedule
the transmission of a packet. Note that following this invocation, the caller
is no longer the owner of the packet and must not use it anymore.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_base_transmit
( 
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
                                   /* Pointer to serial I/F data struct    */
  dsm_item_type *tx_ptr            /* Packet which needs to be transmitted */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_flush_tx

DESCRIPTION
This function will keep transmitting until no more data is pending transmission.
It requires provding a callback to notify caller when reaching this situation.

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_base_flush_tx
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
                                   /* Pointer to serial I/F data struct    */
  hsu_al_ser_base_op_complete_cb flush_tx_cb 
                                   /* App callback to be invoked when all 
                                   pending data has been transmitted       */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_ioctl

DESCRIPTION
Allows passing an I/O control to the serial I/F using CDC.

DEPENDENCIES

RETURN VALUE
TRUE - IOCTL handled. FALSE - Otherwise

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_ser_base_ioctl
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
                                             /* Pointer to serial I/F data 
                                                struct                     */
  sio_ioctl_cmd_type ioctl_cmd,              /* SIO I/O control command    */
  sio_ioctl_param_type* ioctl_param_ptr      /* SIO I/O control command's 
                                                parameter                  */
);

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
);

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
void hsu_al_ser_base_forward_rx_to_app(dsm_item_type *dsm_ptr);


#if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
/*===========================================================================

FUNCTION hsu_al_ser_base_ut_dont_simulate_hsu_over_uart

DESCRIPTION
This function will cause any call to hsu_al_ser_open to bypass
the HSU over UART emulation (which is on, by default, if any of the serial I/Fs 
are mapped over UART). This allows unit tests to call hsu_al_ser_base_*, 
without the above emulation. 

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
No HSU over UART simulation is possible

===========================================================================*/
void hsu_al_ser_base_ut_dont_simulate_hsu_over_uart
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
                                   /* Pointer to serial I/F data struct    */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_hsu_over_uart_is_simulated

DESCRIPTION
This function will check whether or not to bypass
the HSU over UART emulation (which is on, by default, if any of the serial I/Fs 
are mapped over UART).

DEPENDENCIES
None

RETURN VALUE
TRUE - There is HSU simulation above UART. FALSE - Otherwise.

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_ser_base_hsu_over_uart_is_simulated
(
  const hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
                                   /* Pointer to serial I/F data struct    */
);

#endif /* FEATURE_HSU_TEST && !T_REXNT */


#ifdef FEATURE_HSU_TEST
/*===========================================================================

FUNCTION hsu_al_ser_base_ut_set_unit_test_mask

DESCRIPTION
This function will set the unit test mask for the given serial I/F data.
As the code for the serial AL is executed, this mask will be checked in 
interesting places, and if a match is made, will cause the CDC FD stub to interfere.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_base_ut_set_unit_test_mask
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
                                   /* Pointer to serial I/F data struct    */
  uint32  ut_test_mask             /* The test mask                        */
);
#endif /* FEATURE_HSU_TEST */

#endif /* _HSU_AL_SER_BASE_INT_H_ */
