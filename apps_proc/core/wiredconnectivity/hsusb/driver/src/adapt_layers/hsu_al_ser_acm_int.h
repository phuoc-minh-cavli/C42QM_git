#ifndef _HSU_AL_SER_ACM_INT_H_
#define _HSU_AL_SER_ACM_INT_H_

/*==============================================================================

Serial Functions over ACM Adaptation Layer for High Speed USB
H E A D E R   F I L E

GENERAL DESCRIPTION
Contains the API for the serial functions over ACM adapt. layer

EXTERNALIZED FUNCTIONS
hsu_al_ser_acm_init
hsu_al_ser_acm_populate_acm_specific_data
hsu_al_ser_acm_ioctl
hsu_al_ser_acm_open
hsu_al_ser_acm_close
hsu_al_ser_acm_transmit
hsu_al_ser_acm_flush_tx

INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_al_ser_base_init must be called for function instance, 
prior to performing any other operations on the instance.

Copyright (c) 2006 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_ser_acm_int.h#1 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
07/30/06 esh  First revision of file    
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h" /* For FEATURE_HSU_TEST to work */

#include "hsu_al_ser_acm.h"
#include "hsu_al_ser_base_int.h"

#include "hsu_al_bam_utils.h"
#include "jtypes_int.h"
#include "jusb_int.h"
#include "hsu_bus_mode_notifier.h"
/*==============================================================================
Constants and Macros
==============================================================================*/
#ifdef FEATURE_HSU_TEST

/* RX buffer size must be multiple of 512
 * If it is not multiple of 512, it will be rounded down to nearest 512.
 * For this reason, its value must be at least 512. */
#if defined(FEATURE_HS_USB_SMALL_HEAP)
  #define ACM_RX_BUF_DEFAULT_SIZE (2*1024)
#else /* FEATURE_HS_USB_SMALL_HEAP */
  #ifndef HS_USB_ACM_RX_BUF_SIZE
  #define ACM_RX_BUF_DEFAULT_SIZE (12*512)
  #elif (HS_USB_ACM_RX_BUF_SIZE < 512)
  #error "HS_USB_ACM_RX_BUF_SIZE must be at least 512"
  #else
  #define ACM_RX_BUF_DEFAULT_SIZE HS_USB_ACM_RX_BUF_SIZE
  #endif /* HS_USB_ACM_RX_BUF_SIZE */
#endif /* FEATURE_HS_USB_SMALL_HEAP */

#define HSU_ROUND_DOWN_TO_512(size) ((size) & ~511)

/* DSM pool to allocate RX buffers from */
#define ACM_RX_DSM_ITEM_POOL DSM_DS_LARGE_ITEM_POOL

/* each Rx buffer provided to the HW must be multiple of 512 bytes */
#define ACM_ROUNDED_DSM_ITEM_SIZE HSU_ROUND_DOWN_TO_512(DSM_POOL_ITEM_SIZE(ACM_RX_DSM_ITEM_POOL))

/* max number of DSM items that will be provided to HW in single cdc_read_multiple
   operation. calculated based on default size of RX buf which means 
   SIO_IOCTL_SET_PACKET_SIZE will not allow the application to increase 
   Rx buffer size above ACM_RX_BUF_DEFAULT_SIZE */
#define ACM_MAX_NUM_DSM_ITEMS_IN_CDC_READ (ACM_RX_BUF_DEFAULT_SIZE/ACM_ROUNDED_DSM_ITEM_SIZE)

#endif /* FEATURE_HSU_TEST */

/*==============================================================================
Typedefs
==============================================================================*/
#ifdef FEATURE_HS_USB_ACM_BAM
typedef struct
{
  boolean is_sps_mode;
  boolean sps_enabled;
  /* when working in SPS mode (is_sps_mode==TRUE), indicate if SPS mode was 
  enabled including setup of SPS read and write eTDs */
  hsu_sps_connection_type sps_connection;
  /* upon receiving pipe wakeup event from SPS we will trigger remote wakeup */
} hsu_al_acm_sps_info_type;
#endif /* FEATURE_HS_USB_ACM_BAM */

typedef struct
{
  juint16_t             ctrl_line_state;
    /* Ctrl Line State: See CDC Spec. Section 6.2.14 */

  sio_vv_func_ptr_type  app_dtr_changed_cb_func;
    /* App callback to be invoked whenever DTR state is changed */

  acm_line_coding_t     line_coding;
    /* Holds the current line coding, as was determined by the host */
    /* See CDC Spec. Section 6.2.13 */

  juint16_t             last_serial_state_bitmap_rxed_by_host;
    /* Holds the last serial_state bitmap ever received by the host. */
    /* See CDC Spec. Section 6.2.5 */

  juint16_t             last_serial_state_bitmap_sent_to_host;
    /* Holds the last serial_state bitmap ever sent to the host
    by the FD, not necessarily received by the host. */
    /* See CDC Spec. Section 6.2.5 */

  juint16_t             serial_state_bitmap_to_send_to_host;
    /* Holds the most updated serial_state bitmap which needs to be sent to host.
    This is to prevent consecutive calls to acm_notify without waiting for the 
    completion callback. */
    /* See CDC Spec. Section 6.2.5 */

  void*                 stream_mode_specific_data_ptr;
    /* Specific data, depending on strea_mode */

  uint32                sio_ioctl_set_buffer_size_mirror;
    /* "Mirror image" of the given I/O ctl 
    SIO_IOCTL_SET_BUFFER_SIZE/SIO_IOCTL_GET_BUFFER_SIZE, so that the app
    will be under the impression its set buffer size got through */

  boolean                           flush_tx_ended;
  /* flag to indicate that the flush has ended.
  Currently used for a special case of flush - 
  that needs to be performed synchronously (during acm_ioctl that handles the cmd:CHANGE_MODE*/

#ifdef FEATURE_HS_USB_ACM_BAM
  hsu_al_acm_sps_info_type sps_info;
  /* holds SPS information */
#endif /* FEATURE_HS_USB_ACM_BAM */

  uint8                core_idx;                  
  /* The core index of this ACM FD */

  rawdata_mode_data_struct rawdata_data;

  boolean serial_state_notification_in_progress;
  
  /* indicates the bus' suspend/ resume mode */
  hsu_bus_mode_enum hsu_al_acm_suspend_resume_mode;//

} modem_specific_data_struct;
  /* Data unique for the modem function */

typedef struct  
{
  hsu_al_ser_base_data_for_ser_if     base_data;
  modem_specific_data_struct          modem_if_data;
} hsu_al_ser_acm_data; 
/* Information needed from an ACM I/F */

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_ser_acm_populate_acm_specific_data

DESCRIPTION
This function, called following acm_init, populates the various fields 
needed by the ACM FD.  

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_acm_populate_acm_specific_data
(
  hsu_al_ser_acm_data* acm_data_ptr,        /* Pointer to ACM Data struct */
  cdc_handle_t handle,                      /* Given ACM FD handle        */
  acm_callbacks_t* cbs,                     /* output param: ACM callback 
                                               struct to populate          */
  acm_init_info_t* info,                    /* output param: ACM info struct 
                                               to populate                 */
#ifdef FEATURE_HS_USB_CSVT_PORT
  cdc_appctx_t* ctx,                         /* output param: Context to be 
                                               passed by ACM FD in various 
                                               callbacks                   */
  uint8 instance_number
#else /* FEATURE_HS_USB_CSVT_PORT */  
  cdc_appctx_t* ctx                         /* output param: Context to be 
                                               passed by ACM FD in various 
                                               callbacks                   */
#endif /* FEATURE_HS_USB_CSVT_PORT */  
);

/*===========================================================================

FUNCTION hsu_al_ser_acm_init

DESCRIPTION
Initializes CDC/ACM AL specific fields.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_acm_init
(
  hsu_al_ser_acm_data* acm_data_ptr          /* Pointer to ACM Data struct */
);

/*===========================================================================

FUNCTION hsu_al_ser_acm_ioctl

DESCRIPTION
Allows passing an I/O control command to the serial I/F using CDC/ACM.
Note: for the command ACM_IOCTL_SET_RX_TIMEOUT, third parameter should be 
      uint32 casted to (sio_ioctl_param_type*). TODO: this should be changed
      to comply with other IO params(should be defined in sio.h).This is just
      for internal use.


DEPENDENCIES
None

RETURN VALUE
TRUE - I/O control command handled. FALSE - Otherwise

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_ser_acm_ioctl
(
  hsu_al_ser_acm_data* acm_data_ptr,         /* Pointer to ACM Data struct */
  sio_ioctl_cmd_type ioctl_cmd,              /* SIO I/O control command    */
  sio_ioctl_param_type* ioctl_param_ptr      /* SIO I/O control command's 
                                                parameter                  */
);

/*===========================================================================

FUNCTION hsu_al_ser_acm_open

DESCRIPTION
This function opens a stream across a serial interface on the CDC/ACM FD. 

PARAMETERS
acm_data_ptr            : Pointer to ACM I/F data struct
open_ptr                : Point to SIO open port that contains Tx/Rx info
pool_id                 : DSM item pool Id used by the port
rx_buff_size_for_read   : Rx buffer size for read.
tx_max_pending_chain_cnt: Tx max pending chain count for HS-USB H/W queue
tx_timeout              : Tx timeout value for CDC I/F
tx_complete_timer_val   : Tx timer value in milliseconds

DEPENDENCIES
None

RETURN VALUE
TRUE - Operation Successful. FALSE - Otherwise

SIDE EFFECTS
None
==============================================================================*/
boolean hsu_al_ser_acm_open
(
  hsu_al_ser_acm_data  *acm_data_ptr,
  const sio_open_type  *open_ptr,
  dsm_mempool_id_enum_type pool_id,
  juint8_t              rx_buff_cnt,
  uint32                rx_timeout,
  uint32                tx_max_pending_chain_cnt,
  uint32                tx_timeout,
  uint32                tx_complete_timer_val
);

/*===========================================================================

FUNCTION hsu_al_ser_acm_close

DESCRIPTION
This function closes an open stream of a serial interface above the CDC/ACM FD.
It allows an optional callback to notify caller when all the bookkeeping 
needed for closing the stream (flushing Tx, aborting Rx, etc.) 
has been completed.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_acm_close
(
  hsu_al_ser_acm_data* acm_data_ptr,         /* Pointer to ACM Data struct */
  hsu_al_ser_base_op_complete_cb close_cb,   /* Callback to be invoked 
                                             when close has been completed */
  boolean abort_io_operations_immediately    /* Whether or not to abort all 
                                             read/write operations 
                                             immediately or wait until all 
                                             Tx data is flushed, followed 
                                             by an abort read              */
);


/*===========================================================================
FUNCTION hsu_al_ser_acm_transmit

DESCRIPTION
This function transmit a DSM chain over CDC/ACM FD.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_ser_acm_transmit
( 
  hsu_al_ser_acm_data* acm_data_ptr,
  dsm_item_type *tx_ptr
);

/*===========================================================================
FUNCTION hsu_al_ser_acm_flush_tx

DESCRIPTION
flush_tx, does specific acm operation: because of HW queueing that is used here.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_ser_acm_flush_tx
(
  hsu_al_ser_acm_data* acm_data_ptr,
  hsu_al_ser_base_op_complete_cb flush_cb
);

/*===========================================================================

FUNCTION      hsu_al_ser_acm_pool_many_cb

DESCRIPTION
This function invokes the appropriate actions when the ACM memory pool arrives 
to DSM_MEM_LEVEL_USB_MANY on DSM_MEM_OP_FREE.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_ser_acm_pool_many_cb
(
 dsm_mempool_id_enum_type pool_id, 
 hsu_al_ser_acm_data* acm_data_ptr
);

#endif /* _HSU_AL_SER_ACM_INT_H_ */
