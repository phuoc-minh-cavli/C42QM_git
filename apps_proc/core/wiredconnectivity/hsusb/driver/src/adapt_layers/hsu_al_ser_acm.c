/*==============================================================================

USB Serial I/F ACM (Modem) Adaptation Layer for High Speed USB

GENERAL DESCRIPTION
Contains the implementation of the API for the serial functions' over ACM
adapt. layer

EXTERNALIZED FUNCTIONS
hsu_al_ser_acm_populate_acm_specific_data
hsu_al_ser_acm_ioctl
hsu_al_ser_acm_open
hsu_al_ser_acm_close
hsu_al_ser_acm_transmit
hsu_al_ser_acm_flush_tx

INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_al_ser_base_init must be called for function instance, 
prior to performing any other operations on the instance.

Copyright (c) 2006 - 2010, 2020 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_ser_acm.c#3 $

when      who     what, where, why
--------  ---     ------------------------------------------------------
05/21/20  cs      Added a log when DTR changed. Drop the DSM packets if DTR is low.
07/23/08  de      added: set rx timeout through ioctl
07/30/06  esh     First revision of file
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "compiler_int.h" //todo: remove
#include "hsu_al_ser_acm_int.h"
#include "hsu_common_int.h"
#include "hsu_al_ser_base_i.h"
#include "hsu_al_dsm_utils.h"
#include "hsu_conf_sel_i.h"

#include "jusb_core.h"
#include "cdc_fd.h"

#include "jtypes_int.h"
#include "jerrno.h"
#include "port_int.h"
#include "uw_device_cdc.h"
#include "jos_int.h"
#include "jslave_init.h" /* for jslave_wakeup_host */

#include "err.h"
//#include "rex.h"
#include "hsu_qurt_util.h"

#include "dsm_pool.h"

#include "hsu_al_ser.h"
#include "hsu_al_task.h"
#include "hsu_al_task_int.h"
#include "hsu_log.h"


#ifdef FEATURE_DATA_S2_REG
#include "dsatapi.h" /* for dsat_get_sreg_val */
#endif /* FEATURE_DATA_S2_REG */

#include "hsu_core_config_int.h"

#include "sio_wrapper.h" /* for SIO_PAYLOAD */
#include "customer.h"
#include "hsu_al_bam_utils.h"
#include "hsu_bus_mode_notifier.h"
#include "hsu_platform.h"

#if (defined FEATURE_HS_USB_BAM_A2_STUB) && (defined FEATURE_HS_USB_ACM_BAM)
#include "hsu_sps_a2_mem_stub.h"
#endif /* FEATURE_HS_USB_BAM_A2_STUB & FEATURE_HS_USB_ACM_BAM */

#include "hsu_os_abstract_bus.h"
#include "hsu_al_ser_i.h"

/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
#define AL_SER_ACM_DTR_ASSERTED_MASK (0x1)
  /* See CDC Spec. Section 6.2.14 */
#define AL_SER_ACM_RTS_ASSERTED_MASK (0x2)
  /* See CDC Spec. Section 6.2.14*/
#define AL_SER_ACM_CD_ASSERTED_MASK (0x1)
  /* See CDC Spec. Section 6.2.5 */
#define AL_SER_ACM_DSR_ASSERTED_MASK (0x2)
  /* See CDC Spec. Section 6.2.5 */
#define AL_SER_ACM_RI_ASSERTED_MASK (0x8)
  /* See CDC Spec. Section 6.2.5 */

/* max time in ms for waiting rx is not more pending before setting DCD */
#define HSU_ACM_RX_PENDING_MAX_DELAY 20


/* For Raw Data Mode: */
#define AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS 900
  /* Was previously 1000 msec; now it seems to be 900. */

#ifndef FEATURE_DATA_S2_REG
#define AL_SER_ACM_RAWDATA_ESC_SEQ_CHAR '+'
#else /* FEATURE_DATA_S2_REG */
#define AL_SER_ACM_ESC_SEQ_CHAR_REG 2
#define AL_SER_ACM_RAWDATA_ESC_SEQ_CHAR dsat_get_sreg_val(AL_SER_ACM_ESC_SEQ_CHAR_REG)
#endif /* !FEATURE_DATA_S2_REG */
#define AL_SER_ACM_RAWDATA_ESC_SEQ_LENGTH 3

#define AL_SER_ACM_SIO_IOCTL_SET_BUFFER_SIZE_DEFAULT 0

#define HSU_AL_SER_ACM_SUBCLASS         0xFF
#ifdef FEATURE_HS_USB_CSVT_PORT
  #define HSU_AL_SER_ACM_CSVT_SUBCLASS	0xFE	/* Subclass Changed for CSVT Composition*/
#endif /* FEATURE_HS_USB_CSVT_PORT */ 
#define HSU_AL_SER_ACM_PROTOCOL         0xFF

#ifdef FEATURE_HS_USB_CSVT_PORT

  // MAX_NUM_OF_ACM_USERS value is determined by the combination of feature 
  // flags.  So, new array size that is bigger than the max possible USERS 
  // is required
  #define HSU_ACM_STRING_ARRAY_SIZE 2
  
  #if (HSU_ACM_STRING_ARRAY_SIZE < MAX_NUM_OF_ACM_USERS)
    #error "insufficient HSU_ACM_STRING_ARRAY_SIZE < MAX_NUM_OF_ACM_USERS"
  #endif

  #ifndef HSU_ACM_CONTROL_STRING_0
  #define HSU_ACM_CONTROL_STRING_0 NULL
  #endif /* HSU_ACM_CONTROL_STRING_0 */
  #ifndef HSU_ACM_DATA_STRING_0
  #define HSU_ACM_DATA_STRING_0 NULL
  #endif /* HSU_ACM_DATA_STRING_0 */
  #ifndef HSU_ACM_CONTROL_STRING_1
  #define HSU_ACM_CONTROL_STRING_1 NULL
  #endif /* HSU_ACM_CONTROL_STRING_1 */
  #ifndef HSU_ACM_DATA_STRING_1
  #define HSU_ACM_DATA_STRING_1 NULL
  #endif /* HSU_ACM_DATA_STRING_1 */
#else /* FEATURE_HS_USB_CSVT_PORT */ 
  #ifndef HSU_ACM_CONTROL_STRING
    #define HSU_ACM_CONTROL_STRING NULL
  #endif /* HSU_ACM_CONTROL_STRING */
  #ifndef HSU_ACM_DATA_STRING
    #define HSU_ACM_DATA_STRING NULL
  #endif /* HSU_ACM_DATA_STRING */
#endif /* FEATURE_HS_USB_CSVT_PORT */ 

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
Variables
==============================================================================*/

#ifdef FEATURE_HS_USB_CSVT_PORT
/* String descriptors for control and data interfaces */
static struct 
{  
  char *control,*data;
} hsu_al_acm_strings[HSU_ACM_STRING_ARRAY_SIZE] = 
{  
  {HSU_ACM_CONTROL_STRING_0 , HSU_ACM_DATA_STRING_0},
  {HSU_ACM_CONTROL_STRING_1 , HSU_ACM_DATA_STRING_1}
};
#endif /* FEATURE_HS_USB_CSVT_PORT */

/*---------------------------------------------------------------------------
FORWARD DECLARATIONS
---------------------------------------------------------------------------*/
/*
The *_ignore_* functions are needed only because cannot pass NULL to 
mandatory ACM requests' callbacks:
*/
static jresult_t al_ser_acm_ignore_encap_command
(
  void* command, 
  juint16_t size, 
  cdc_appctx_t ctx
);

static void al_ser_acm_ignore_encap_response
(
  cdc_iostatus_t status, 
  juint32_t bytes_sent, 
  cdc_appctx_t ctx
);

/* this function is used for a special case:
   When a SIO_IOCTL_CHANGE_MODE is received, 
   and the mode is changed from SIO_DS_AUTODETECT_MODE  to a different mode,
   we need to flush the tx queue.
   The application does not expect that - therefor it does not supply a CB.
   The ACM itself supplies this decadent CB instead. */
static void al_ser_acm_flush_tx_from_ioctl_cb
(
  void*
);

/* Line coding (see CDC Spec. chapter 6.2.12 & 6.2.13) */
static jresult_t al_ser_acm_set_line_coding_cb
(
  acm_line_coding_t* coding, 
  cdc_appctx_t ctx
);

static jresult_t al_ser_acm_get_line_coding_cb
(
  acm_line_coding_t* coding,
  cdc_appctx_t ctx
);

/*
Calls modem_specific_data.app_dtr_changed_cb_func only if the dtr bit in
modem_specific_data.ctrl_line_state is different than ctrl_sig_bitmap, 
received in callback...
*/
/* Set Control Line State (See CDC Spec. chapter 6.2.14) */
static jresult_t al_ser_acm_set_control_line_state_cb
(
  juint16_t ctrl_sig_bitmap, 
  cdc_appctx_t ctx
);

/* Serial State (see CDC Spec. chapter 6.2.5) */
static void al_ser_acm_notify_serial_state_cb
(
  cdc_iostatus_t status,
  cdc_appctx_t ctx
);

static void al_ser_acm_rx_done
(
  cdc_iostatus_t *status,        /* array of statuses for each transfer */
  void **buffers,                /* array of buffers with data */
  juint32_t *bytes_transferred,  /* array of sizes for each buffer */
  juint32_t req_num,             /* number of request in current callback (if 
                                 timeout occurred it is not the same as the
                                 number passed to the cdc_read_multiple) */
  cdc_appctx_t ctx
);


static void al_ser_acm_tx_done
(
  cdc_iostatus_t *status,         /* array of statuses for each transfer */
  void **buffers,                 /* array of data buffers */
  juint32_t *buf_sizes,           /* array of sizes for each buffer */
  juint32_t req_num,              /* number of request that were sent */
  cdc_appctx_t ctx
);

static jresult_t al_ser_acm_enabled
(
  cdc_appctx_t ctx
);

static jresult_t al_ser_acm_disabled
(
  cdc_appctx_t ctx
);

static jresult_t al_ser_acm_resumed
(
  cdc_appctx_t ctx
);

static jresult_t al_ser_acm_uninit
(
 cdc_appctx_t ctx
 );

static jresult_t al_ser_acm_notify_serial_state
(
  cdc_handle_t acm_fd_handle,
  juint16_t serial_state_bitmap
);

static jresult_t al_ser_acm_notify_serial_state_if_enabled
(
  hsu_al_ser_acm_data* acm_data_ptr
);

static boolean al_ser_acm_update_serial_state_param
(
  hsu_al_ser_acm_data* acm_data_ptr,
  juint16_t param_mask,
  boolean param_is_asserted
);

/* Support for Rawdata mode (circuit switched )*/
static void al_ser_acm_detect_esc_seq
(
  rawdata_mode_data_struct* rawdata_data_ptr,
  const char* buffer_ptr,
  juint32_t buffer_size
);

static void al_ser_acm_init_rawdata_mode_data
(
  rawdata_mode_data_struct* rawdata_data_ptr
);

static void al_ser_acm_rawdata_esc_seq_isr
(
  void *rawdata_data_ptr_as_timer_data
);

static void al_ser_acm_rawdata_esc_seq_isr_cb
(
  unsigned long rawdata_data_ptr_as_timer_data
);

static void al_ser_acm_unarm_esc_seq_clk
(
  rawdata_mode_data_struct* rawdata_data_ptr
);

static void al_ser_acm_arm_esc_seq_clk
(
  rawdata_mode_data_struct* rawdata_data_ptr
);

static void hsu_al_acm_suspend_resume_mode_changed_cb
(
  hsu_bus_mode_enum new_mode,
  void* app_data
);

static jresult_t hsu_al_acm_register_bus_mode_notifications
(
  hsu_al_ser_acm_data* acm_data_ptr,
  cdc_handle_t handle
);

static void al_ser_acm_rx_handle_rawdata_mode
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr,   
  dsm_item_type        *cur_dsm_item,
  char                 *buffer_ptr, 
  juint32_t             bytes_transferred
);

/*==============================================================================

Utility functions for unit testing

==============================================================================*/

/*==============================================================================

FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

#ifdef FEATURE_HS_USB_ACM_BAM
/*===========================================================================

FUNCTION hsu_al_acm_wakeup_host

DESCRIPTION
This function will trigger remote wakeup

DEPENDENCIES
Called in HSU_AL_TASK context upon pipe wakeup event from SPS

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
static void hsu_al_acm_wakeup_host(void* cmd_param)
{
  hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)cmd_param;

  if (acm_data_ptr->base_data.instance_enabled_by_cdc_fd == FALSE)
  {
    return;
  }

  hsu_al_task_signal_remote_wakeup();
} /* hsu_al_acm_wakeup_host */ 

/*===========================================================================

FUNCTION hsu_al_acm_pipe_wakeup_cb

DESCRIPTION
This function is the callback function for SPS pipe wakeup event.
This callback will be invoked when USB is in suspend and SPS pipe now has data
to send to host. We need to trigger remote wakeup so SPS data can be delivered
to host.
We let HSU_AL_TASK take care of remote wakeup so that SPS driver will not get
delayed

DEPENDENCIES
None.

RETURN VALUE
always return NULL

SIDE EFFECTS
None.

===========================================================================*/
static void hsu_al_acm_pipe_wakeup_cb
(
bam_result_type bam_result
)
{   
   HSU_ULOG_1(HIGH_MSG, BUS_LOG, "bam_result_evt %u", bam_result.event);
 
   if (bam_result.event != BAM_EVENT_WAKEUP)
   {
     HSU_ERR_FATAL("hsu_al_ecm_pipe_wakeup_cb: unexpected event %d", bam_result.event, 0, 0);
     return;
   }
 
   hsu_al_acm_wakeup_host(&bam_result.cb_data);
   return;
} /* hsu_al_acm_pipe_wakeup_cb */

/*===========================================================================

FUNCTION hsu_al_acm_enable_sps

DESCRIPTION
Enable SPS mode for this ACM instance. Sets up SPS read and write operations

DEPENDENCIES
This function is assumed to be called with giant mutex already locked

RETURN VALUE
None.

SIDE EFFECTS
Allocate SPS pipes

===========================================================================*/
static boolean hsu_al_acm_enable_sps
(
  hsu_al_ser_acm_data* acm_data_ptr
)
{
  jresult_t cdc_res;
  static boolean sps_pipes_allocated = FALSE;
  hsu_al_acm_sps_info_type* sps_info_ptr = &acm_data_ptr->modem_if_data.sps_info;

  HSU_MSG_HIGH("hsu_al_acm_enable_sps: Enter");

  if(sps_info_ptr->sps_enabled == TRUE)
  {
    HSU_MSG_HIGH("hsu_al_acm_enable_sps: SPS mode already enabled");
    return TRUE;
  }

  if (!sps_pipes_allocated)
  {
    /* until further notice, SPS pipes are allocated at startup and never deallocated */
    sps_info_ptr->sps_connection.consumer_pipe_wakeup_event_cb.func = hsu_al_acm_pipe_wakeup_cb;
    sps_info_ptr->sps_connection.consumer_pipe_wakeup_event_cb.data = acm_data_ptr;
    if (!hsu_al_sps_allocate_pipes(&(sps_info_ptr->sps_connection)))
    {
      HSU_ERR_FATAL("Allocating SPS pipe failed", 0, 0, 0);
      return FALSE;
    }
    sps_pipes_allocated = TRUE;
  }

#if (defined FEATURE_HS_USB_BAM_A2_STUB)
  hsu_sps_a2_mem_stub_init(&acm_data_ptr->base_data, 
    &sps_info_ptr->read_pipe, &sps_info_ptr->write_pipe);
#endif /* FEATURE_HS_USB_BAM_A2_STUB */

  cdc_res = cdc_sps_read_write_infinite(acm_data_ptr->base_data.instance_of_fd_used, 
    (juint8_t)sps_info_ptr->sps_connection.producer_pipe_index, HSU_AL_BAM_UL, /* read == DIRECTION OUT == producer */
    (juint8_t)sps_info_ptr->sps_connection.consumer_pipe_index, HSU_AL_BAM_DL); /* write == DIRECTION IN == consumer */

  switch(cdc_res)
  {
  case HSU_JSUCCESS:
    sps_info_ptr->sps_enabled = TRUE;
    acm_data_ptr->base_data.rx_info.read_oper_is_in_progress = TRUE;
    acm_data_ptr->base_data.tx_info.is_write_in_progress = TRUE;
    break;

  default:
    HSU_ERR_FATAL("hsu_al_acm_enable_sps: cdc_sps_read_write_infinite failed %d",
      cdc_res, 0, 0);
  }

  HSU_MSG_HIGH("hsu_al_acm_enable_sps: Exit");

  return sps_info_ptr->sps_enabled;
}
/*===========================================================================

FUNCTION hsu_al_acm_disable_sps

DESCRIPTION
Disable SPS mode for this ACM instance.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
static void hsu_al_acm_disable_sps
(
  hsu_al_ser_acm_data* acm_data_ptr
)
{
  HSU_MSG_HIGH("hsu_al_acm_disable_sps: Enter");

  acm_data_ptr->modem_if_data.sps_info.sps_enabled = FALSE;

  HSU_MSG_HIGH("hsu_al_acm_disable_sps: Exit");
}

#endif /* FEATURE_HS_USB_ACM_BAM */

/*===========================================================================
FUNCTION hsu_al_ser_acm_populate_acm_specific_data()
===========================================================================*/
void hsu_al_ser_acm_populate_acm_specific_data
(
  hsu_al_ser_acm_data* acm_data_ptr,
  cdc_handle_t handle,
  acm_callbacks_t* cbs,
  acm_init_info_t* info,
#ifdef FEATURE_HS_USB_CSVT_PORT
  cdc_appctx_t* ctx,
  uint8 instance_number
#else /* FEATURE_HS_USB_CSVT_PORT */   
  cdc_appctx_t* ctx
#endif /* FEATURE_HS_USB_CSVT_PORT */   
)
{
  HSU_ASSERT(acm_data_ptr != NULL);
  HSU_ASSERT(handle != NULL);
  HSU_ASSERT(cbs != NULL);
  HSU_ASSERT(info != NULL);
  HSU_ASSERT(ctx != NULL);

  /* Save context for later callbacks */
  *ctx = (cdc_appctx_t)acm_data_ptr;

  /* First, explicitly mark the callbacks which aren't used and can be NULL */
  /* Comm. Feature not used */
  cbs->clear_comm_feature_h = NULL;
  cbs->get_comm_feature_complete_h = NULL;
  cbs->get_comm_feature_h = NULL;
  cbs->set_comm_feature_h = NULL;
  /* No need for Get Line Coding Completion */
  cbs->get_line_coding_complete_h = NULL;
  /* Send Break and Network Connection not used */
  cbs->notify_net_conn_complete_h = NULL;
  cbs->send_break_h = NULL;
  cbs->basic.read_complete_h = NULL;
  cbs->basic.write_complete_h = NULL;

  /* The remaining callbacks are used */
  cbs->basic.read_multi_complete_h = al_ser_acm_rx_done;
  cbs->basic.write_multi_complete_h = al_ser_acm_tx_done;
  cbs->basic.uninit_h = al_ser_acm_uninit;

  cbs->basic.enabled_h = al_ser_acm_enabled;
  cbs->basic.disabled_h = al_ser_acm_disabled;

  /* This AL will not be using encap commands and responses */
  cbs->basic.encap_command_h = al_ser_acm_ignore_encap_command;
  cbs->basic.encap_response_complete_h = al_ser_acm_ignore_encap_response;
  
  cbs->set_line_coding_h = al_ser_acm_set_line_coding_cb;
  cbs->get_line_coding_h = al_ser_acm_get_line_coding_cb;

  cbs->set_control_line_state_h = al_ser_acm_set_control_line_state_cb;

  cbs->notify_serial_state_complete_h = al_ser_acm_notify_serial_state_cb;

  cbs->basic.resumed_h = al_ser_acm_resumed;

  /* Init info struct */
  info->basic.flags = (juint32_t)( hsu_config_selector_is_standard_acm_supported() ?
                      (
                      CDC_INIT_BUFFER_DMABLE | 
                      CDC_INIT_NO_IAD) :  /* Union descriptor is supported instead of IAD */
                      (CDC_INIT_BUFFER_DMABLE | 
                      CDC_INIT_SINGLE_INTERFACE |
                      CDC_INIT_VENDOR_SPECIFIC_CLASS |
                      CDC_INIT_NO_CLASS_DESC) );

  /* Enable support for ZLP */
  info->basic.zlp_terminator = TRUE;

#if defined (FEATURE_HS_USB_OS_DETECTION)

  /* In order for the MDLM descriptors to appear, 
  CDC_INIT_NO_CLASS_DESC must not be used. */
  info->basic.flags &= ~(juint32_t)CDC_INIT_NO_CLASS_DESC;

#endif /* FEATURE_HS_USB_OS_DETECTION */

#ifdef FEATURE_HS_USB_CSVT_PORT
  if (2 == instance_number)  /* 2nd instance of ACM is the CSVT */
  	{ 
  	  info->basic.cci_subclass = HSU_AL_SER_ACM_CSVT_SUBCLASS;
  	}
  else
  	{
  	  info->basic.cci_subclass = HSU_AL_SER_ACM_SUBCLASS;
  	}
  info->basic.cci_protocol = HSU_AL_SER_ACM_PROTOCOL;
#else /* FEATURE_HS_USB_CSVT_PORT */
  info->basic.cci_subclass = HSU_AL_SER_ACM_SUBCLASS;
  info->basic.cci_protocol = HSU_AL_SER_ACM_PROTOCOL;
#endif   /* FEATURE_HS_USB_CSVT_PORT */

  /* Similar to FS: Allow line coding, and call mgmt by itself, over data plane */
  info->capabilities = 
    (juint32_t)ACM_CAP_LINE_CODING | \
      ((juint32_t)ACM_CAP_CALL_MGMT | (juint32_t)ACM_CAP_CALL_MGMT_OVER_DC);

#ifdef FEATURE_HS_USB_CSVT_PORT

  // Check for hsu_al_acm_strings array out bound
  HSU_ASSERT((instance_number-1) < HSU_ACM_STRING_ARRAY_SIZE);
  
  /* Communication interface string descriptor */
  info->basic.comm_string = hsu_al_acm_strings[instance_number-1].control;
  /* data interface string descriptor */
  info->basic.data_string = hsu_al_acm_strings[instance_number-1].data;
#else /* FEATURE_HS_USB_CSVT_PORT */
  /* Communication interface string descriptor */
  info->basic.comm_string = HSU_ACM_CONTROL_STRING;
  /* data interface string descriptor */
  info->basic.data_string = HSU_ACM_DATA_STRING;
#endif /* FEATURE_HS_USB_CSVT_PORT */

  hsu_al_acm_register_bus_mode_notifications(acm_data_ptr, handle);
}

/*===========================================================================
FUNCTION hsu_al_ser_acm_init()
===========================================================================*/
void hsu_al_ser_acm_init
(
  hsu_al_ser_acm_data* acm_data_ptr          /* Pointer to ACM Data struct */
)
{
  HSU_ASSERT(acm_data_ptr != NULL);

  /* Call base class' method: */
  hsu_al_ser_base_init(&(acm_data_ptr->base_data));

  /* "zero" line coding (see uw_device_cdc_acm.h for more details)
  The smallest values are used here. Whenever the host sends the SET_LINE_CODING 
  request, the following fields will be updated accordingly and will remain
  updated even after receiving the disabled callback. */
  acm_data_ptr->modem_if_data.line_coding.char_format = (juint8_t)ACM_LC_1_STOP_BITS;
  acm_data_ptr->modem_if_data.line_coding.data_bits = (juint8_t)ACM_LC_DATA_5_BITS;
  acm_data_ptr->modem_if_data.line_coding.dte_rate = 0;
  acm_data_ptr->modem_if_data.line_coding.parity_type = (juint8_t)ACM_LC_PARITY_NONE;
  acm_data_ptr->modem_if_data.serial_state_notification_in_progress = FALSE;
  acm_data_ptr->modem_if_data.hsu_al_acm_suspend_resume_mode = HSU_BUS_MODE_SUSPENDED;

  /* Register for rawdata mode handler */
  acm_data_ptr->base_data.rx_info.handle_rawdata_mode_cb = 
    al_ser_acm_rx_handle_rawdata_mode;
}

/*===========================================================================

FUNCTION al_ser_acm_verify_buff_size

DESCRIPTION
Utility function to adjust buffer size to following restrictions:
- must be multiple of ACM_ROUNDED_DSM_ITEM_SIZE (imply that it's also a multiple of 512 bytes)
- must not be larger than the given max_buff_size

DEPENDENCIES
None

RETURN VALUE
adjusted buffer size

SIDE EFFECTS
None

===========================================================================*/
static uint32 al_ser_acm_verify_buff_size
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr,
  uint32 buff_size,
  uint32 max_buff_size
)
{
  uint32 rounded_dsm_size;

  rounded_dsm_size = ser_if_data_ptr->rx_info.dsm_item_size_rounded_down_to_512;

  /* make sure it's a multiple of ACM_ROUNDED_DSM_ITEM_SIZE */
  buff_size = (buff_size / rounded_dsm_size) * rounded_dsm_size;
  if (buff_size == 0)
  {
    buff_size = rounded_dsm_size;
  }
  /* make sure it's no larger than max_buff_size */
  return MIN(buff_size, max_buff_size);
}

/*===========================================================================
FUNCTION hsu_al_ser_acm_ioctl()
Should be called with the Giant mutex locked
===========================================================================*/
boolean hsu_al_ser_acm_ioctl
(
  hsu_al_ser_acm_data* acm_data_ptr,
  sio_ioctl_cmd_type ioctl_cmd,
  sio_ioctl_param_type* ioctl_param_ptr
)
{
  boolean update_ser_state_param_is_successful;
  sio_stream_mode_type prev_stream_mode = SIO_MAX_MODE;
  dsm_watermark_type* prev_rx_wmq_ptr = NULL;
  dsm_watermark_type* prev_tx_wmq_ptr = NULL;
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr;
#ifdef FEATURE_HS_USB_ACM_BAM
  modem_specific_data_struct* modem_data_ptr = NULL;
#endif /* FEATURE_HS_USB_ACM_BAM */
  uint32 rx_timeout;

  if (!hsu_os_is_giant_locked_by_curr_task())
  {
    HSU_ERR_FATAL("hsu_al_ser_acm_ioctl: Should be called "
      " with Giant mutex locked!", 0, 0, 0);
    return NULL;
  }

  HSU_ASSERT(acm_data_ptr != NULL);
  HSU_ASSERT(ioctl_param_ptr != NULL);

  /* First try the "base class": */
  if (hsu_al_ser_base_ioctl(&acm_data_ptr->base_data, 
    ioctl_cmd, ioctl_param_ptr) == TRUE)
  {
    /* Handled in "base" */
    return TRUE;
  }

  /* Try the "acm class" ioctl handler: */

  /* Handles the following I/O CTLs: 
  SIO_IOCTL_ENABLE_DTR_EVENT
  SIO_IOCTL_DISABLE_DTR_EVENT
  SIO_IOCTL_DTE_READY_ASSERTED
  SIO_IOCTL_GET_CURRENT_RTS

  SIO_IOCTL_CHANGE_MODE
  SIO_IOCTL_RECORD_ESC_FUNC_PTR - For Raw Data mode only
  SIO_IOCTL_ENABLE_AUTODETECT
  SIO_IOCTL_RECORD_PKT_FUNC_PTR - Not used anymore...

  SIO_IOCTL_CD_ASSERT
  SIO_IOCTL_CD_DEASSERT
  SIO_IOCTL_RI_ASSERT
  SIO_IOCTL_RI_DEASSERT
  SIO_IOCTL_DSR_ASSERT
  SIO_IOCTL_DSR_DEASSERT

  SIO_IOCTL_INBOUND_FLOW_ENABLE
  SIO_IOCTL_INBOUND_FLOW_DISABLE

  SIO_IOCTL_SET_BUFFER_SIZE
  SIO_IOCTL_GET_BUFFER_SIZE
  SIO_IOCTL_SET_PACKET_SIZE
  SIO_IOCTL_GET_PACKET_SIZE

  ACM_IOCTL_SET_RX_TIMEOUT - defined in this file, as a temporary thing
  */
#ifdef FEATURE_HS_USB_ACM_BAM
  modem_data_ptr = &acm_data_ptr->modem_if_data;
#endif /* FEATURE_HS_USB_ACM_BAM */
  ser_if_data_ptr = &(acm_data_ptr->base_data);

  switch (ioctl_cmd)
  {
    /* Control Line State Commands */
    case SIO_IOCTL_ENABLE_DTR_EVENT:
    {
      HSU_MSG_LOW("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_ENABLE_DTR_EVENT");

      acm_data_ptr->modem_if_data.app_dtr_changed_cb_func = 
        (sio_vv_func_ptr_type)((*ioctl_param_ptr).enable_dte_ready_event);
      break;
    }
    case SIO_IOCTL_DISABLE_DTR_EVENT:
    {
      HSU_MSG_LOW("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_DISABLE_DTR_EVENT");

      acm_data_ptr->modem_if_data.app_dtr_changed_cb_func = NULL;
      break;
    }
    case SIO_IOCTL_DTE_READY_ASSERTED:
    {
      HSU_MSG_LOW("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_DTE_READY_ASSERTED");
      if (
        (acm_data_ptr->modem_if_data.ctrl_line_state & AL_SER_ACM_DTR_ASSERTED_MASK) 
          != 0)
      {
        *(ioctl_param_ptr->dte_ready_asserted) = TRUE;
      }
      else
      {
        *(ioctl_param_ptr->dte_ready_asserted) = FALSE;
      }
      break;
    }
    case SIO_IOCTL_GET_CURRENT_RTS:
    {
      HSU_MSG_LOW("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_GET_CURRENT_RTS");
      if (
        (acm_data_ptr->modem_if_data.ctrl_line_state & AL_SER_ACM_RTS_ASSERTED_MASK) 
        != 0)
      {
        *(ioctl_param_ptr->rts_asserted) = TRUE;
      }
      else
      {
        *(ioctl_param_ptr->rts_asserted) = FALSE;
      }
      break;
    }

    /* Autodetect/Rawdata/Packet mode commands */
    case SIO_IOCTL_CHANGE_MODE:
    {
#if (defined FEATURE_HS_USB_ACM_BAM) && (!defined FEATURE_HS_USB_BAM_A2_STUB)
      if (modem_data_ptr->sps_info.is_sps_mode)
      {
        HSU_MSG_ERROR("SIO_IOCTL_CHANGE_MODE: called in SPS mode");
        return TRUE;
      }
#endif /* FEATURE_HS_USB_ACM_BAM & !FEATURE_HS_USB_BAM_A2_STUB */

      HSU_MSG_HIGH("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_CHANGE_MODE");

      /* Save the WMQs previously used */
      prev_stream_mode = acm_data_ptr->base_data.stream_mode;
      prev_rx_wmq_ptr = acm_data_ptr->base_data.rx_wmq_ptr;
      prev_tx_wmq_ptr = acm_data_ptr->base_data.tx_wmq_ptr;

      HSU_ASSERT((*ioctl_param_ptr).open_ptr != NULL);

      /* Update the new stream mode: */
      acm_data_ptr->base_data.stream_mode = 
        (*ioctl_param_ptr).open_ptr->stream_mode;

      /* When changing from autodetect to non autodetect, need to verify the 
      previous WMQs are now empty*/
      if (prev_stream_mode == SIO_DS_AUTODETECT_MODE &&
        acm_data_ptr->base_data.stream_mode != SIO_DS_AUTODETECT_MODE)
      {
        if (prev_rx_wmq_ptr != NULL)
        {
          dsm_empty_queue(prev_rx_wmq_ptr);
        }
        if ((prev_tx_wmq_ptr != NULL) && 
          ((acm_data_ptr->base_data.tx_info.remaining_chain_ptr != NULL) ||
            (dsm_is_wm_empty(prev_tx_wmq_ptr)== FALSE)) )
        {
          HSU_MSG_HIGH("al_ser_acm_ioctl got CHANGE_MODE=>flushing tx queue");
          acm_data_ptr->modem_if_data.flush_tx_ended = FALSE;
          hsu_al_ser_base_flush_tx(&acm_data_ptr->base_data, al_ser_acm_flush_tx_from_ioctl_cb);  
          /* while flush is on going - sleep(1). flush has a timeout of 4sec (in case of failure)*/
          while (acm_data_ptr->modem_if_data.flush_tx_ended == FALSE)
          {
            /*We have to release the Giant mutex here in order to prevent the following deadlock:
            al_ser_base_flush_tx_timer_cb has to acquire the Giant mutex in order to call 
            cdc_handle_tx_completion */
            jsafe_leave();
            hsu_qurt_sleep_ms(1);
            jsafe_enter();
          }
        }
      }

     /* Update the rest of the new parameters: */
      acm_data_ptr->base_data.ioctl_flow_ctl_mirror.rx_flow = 
        (*ioctl_param_ptr).open_ptr->rx_flow;
      acm_data_ptr->base_data.ioctl_flow_ctl_mirror.tx_flow =
        (*ioctl_param_ptr).open_ptr->tx_flow;

      /* TODO: Is there a use case in which there is a need to guard against 
      FD Rx and Tx done callbacks, since they use the following fields? 
      It seems theoretical, since:
      This mode change is done when the app received an AT command from host (Rx).
      Therefore the host cannot send anything (Rx) until receiving an answer from
      the app, which is only when the app sends something (Tx).
      However, the app cannot send anything (Tx) until we exit this function.
      */
      /* Note: In ds3gsiolib, when switching from autodetect to another mode,
      the autodetect WMQs are emptied without any INTLOCK/TASKLOCK...*/
      acm_data_ptr->base_data.rx_info.app_rx_cb_func = 
        (*ioctl_param_ptr).open_ptr->rx_func_ptr;
      acm_data_ptr->base_data.rx_wmq_ptr =
        (*ioctl_param_ptr).open_ptr->rx_queue;
      acm_data_ptr->base_data.tx_wmq_ptr =
        (*ioctl_param_ptr).open_ptr->tx_queue;

      /* When moving from Rawdata to another mode, make sure the clk isr won't 
      be called */
      if (prev_stream_mode == SIO_DS_RAWDATA_MODE &&
        acm_data_ptr->base_data.stream_mode != SIO_DS_RAWDATA_MODE)
      {
        al_ser_acm_unarm_esc_seq_clk(&(acm_data_ptr->modem_if_data.rawdata_data));
      }

      /* When moving to Rawdata, make sure the clk and states are initialized */
      if (acm_data_ptr->base_data.stream_mode == SIO_DS_RAWDATA_MODE)
      {
        al_ser_acm_init_rawdata_mode_data(&(acm_data_ptr->modem_if_data.rawdata_data));

        acm_data_ptr->modem_if_data.stream_mode_specific_data_ptr = &(acm_data_ptr->modem_if_data.rawdata_data);

        al_ser_acm_arm_esc_seq_clk(
          acm_data_ptr->modem_if_data.stream_mode_specific_data_ptr);
      }
      break;
    }
    case SIO_IOCTL_RECORD_ESC_FUNC_PTR:
    {
      HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_RECORD_ESC_FUNC_PTR");
      if (acm_data_ptr->base_data.stream_mode != SIO_DS_RAWDATA_MODE)
      {
        HSU_MSG_ERROR_3("hsu_al_ser_acm_ioctl: Requested SIO_IOCTL_RECORD_ESC_FUNC_PTR\
                      (%u), but stream_mode(%u) != SIO_DS_RAWDATA_MODE (%u)", 
                      SIO_IOCTL_RECORD_ESC_FUNC_PTR, 
                      acm_data_ptr->base_data.stream_mode, SIO_DS_RAWDATA_MODE);
        return FALSE;
      }
      /* else (acm_data_ptr->base_data.stream_mode == SIO_DS_RAWDATA_MODE)*/
      acm_data_ptr->modem_if_data.rawdata_data.rawdata_mode_esc_sequence_cb_func = 
        (*ioctl_param_ptr).record_escape_func_ptr;
      break;
    }
    case SIO_IOCTL_ENABLE_AUTODETECT:
    {
      HSU_MSG_HIGH_1("*_acm_ioctl: cmd %u (ENABLE_AUTODETECT) nothing to do", 
        ioctl_cmd);
      break;
    }

    /* Serial state commands */
    case SIO_IOCTL_CD_ASSERT:
    {
      uint32 retry_cnt = HSU_ACM_RX_PENDING_MAX_DELAY;
	  volatile boolean *is_pending = &(ser_if_data_ptr->rx_info.is_pending);

      /* wait for potential enqueued input data */
      while( (*is_pending != FALSE) && ( retry_cnt-- > 0 ) )
      {
         /* We have to release the Giant mutex here in order to prevent the following deadlock:
         al_ser_base_flush_tx_timer_cb has to acquire the Giant mutex in order to call
         cdc_handle_tx_completion */
         jsafe_leave();
         hsu_qurt_sleep_ms( 1 );
         jsafe_enter();
      }
	  
	  if (retry_cnt == 0)
	  {
		 HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_CD_ASSERT, timed out while trying to complete RX transactions");
	  }
	  
      /* handle tx completion */
      if( !( ( ( ser_if_data_ptr->tx_wmq_ptr == NULL ) || dsm_is_wm_empty( ser_if_data_ptr->tx_wmq_ptr ) ) && ( ser_if_data_ptr->tx_info.remaining_chain_ptr == NULL ) ) )
      {
         cdc_handle_tx_completion( ser_if_data_ptr->instance_of_fd_used );
      }

	  HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_CD_ASSERT");
      /* Check CD status, only update and send notification if changed */
      update_ser_state_param_is_successful = 
        al_ser_acm_update_serial_state_param(acm_data_ptr, 
          AL_SER_ACM_CD_ASSERTED_MASK, TRUE);
      HSU_ASSERT(update_ser_state_param_is_successful);
      break;
    }
    case SIO_IOCTL_CD_DEASSERT:
    {
      uint32 retry_cnt = HSU_ACM_RX_PENDING_MAX_DELAY;
	  volatile boolean *is_pending = &(ser_if_data_ptr->rx_info.is_pending);
	  
      /* wait for potential enqueued input data */
      while( (*is_pending != FALSE) && ( retry_cnt-- > 0 ) )
      {
         /* We have to release the Giant mutex here in order to prevent the following deadlock:
         al_ser_base_flush_tx_timer_cb has to acquire the Giant mutex in order to call
         cdc_handle_tx_completion */
         jsafe_leave();
         hsu_qurt_sleep_ms( 1 );
         jsafe_enter();
      }
	  
	  if (retry_cnt == 0)
	  {
		 HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_CD_DEASSERT, timed out while trying to complete RX transactions");
	  }
      
	  /* handle tx completion */
      if( !( ( ( ser_if_data_ptr->tx_wmq_ptr == NULL ) || dsm_is_wm_empty( ser_if_data_ptr->tx_wmq_ptr ) ) && ( ser_if_data_ptr->tx_info.remaining_chain_ptr == NULL ) ) )
      {
         cdc_handle_tx_completion( ser_if_data_ptr->instance_of_fd_used );
      }
	  
	  HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_CD_DEASSERT");
      /* Check CD status, only update and send notification if changed */
      update_ser_state_param_is_successful = 
        al_ser_acm_update_serial_state_param(acm_data_ptr, 
        AL_SER_ACM_CD_ASSERTED_MASK, FALSE);
      HSU_ASSERT(update_ser_state_param_is_successful);
      break;
    }
    case SIO_IOCTL_RI_ASSERT:
    {
      HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_RI_ASSERT");
      /* Check RI status, only update and send notification if changed */
      update_ser_state_param_is_successful = 
        al_ser_acm_update_serial_state_param(acm_data_ptr, 
        AL_SER_ACM_RI_ASSERTED_MASK, TRUE);
      HSU_ASSERT(update_ser_state_param_is_successful);
      break;
    }
    case SIO_IOCTL_RI_DEASSERT:
    {
      HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_RI_DEASSERT");
      /* Check RI status, only update and send notification if changed */
      update_ser_state_param_is_successful = 
        al_ser_acm_update_serial_state_param(acm_data_ptr, 
        AL_SER_ACM_RI_ASSERTED_MASK, FALSE);
      HSU_ASSERT(update_ser_state_param_is_successful);
      break;
    }
    case SIO_IOCTL_DSR_ASSERT:
    {
      HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_DSR_ASSERT");
      /* Check DSR status, only update and send notification if changed */
      update_ser_state_param_is_successful = 
        al_ser_acm_update_serial_state_param(acm_data_ptr, 
        AL_SER_ACM_DSR_ASSERTED_MASK, TRUE);
      HSU_ASSERT(update_ser_state_param_is_successful);
      break;
    }
    case SIO_IOCTL_DSR_DEASSERT:
    {
      HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_DSR_DEASSERT");
      /* Check DSR status, only update and send notification if changed */
      update_ser_state_param_is_successful = 
        al_ser_acm_update_serial_state_param(acm_data_ptr, 
        AL_SER_ACM_DSR_ASSERTED_MASK, FALSE);
      HSU_ASSERT(update_ser_state_param_is_successful);
      break;
    }

    /* In-bound flow commands */
    case SIO_IOCTL_INBOUND_FLOW_ENABLE:
    {
#ifdef FEATURE_HS_USB_ACM_BAM
      if (modem_data_ptr->sps_info.is_sps_mode)
      {
        HSU_MSG_ERROR("SIO_IOCTL_INBOUND_FLOW_ENABLE: called in SPS mode");

        /* When SMD opens the port, inbound flow is disabled by default.
           Right right opening the port, SDM enables inbound flow.
           Inbound flow control has no effect if SPS is used.
           But, if we return FLASE, hsu_al_ser_ioctl() assertion will fail. 
        */
        return TRUE;
      }
#endif /* FEATURE_HS_USB_ACM_BAM */

      HSU_MSG_HIGH_2("*_acm_ioctl: SIO_IOCTL_INBOUND_FLOW_ENABLE:"
        "pool 0x%X, free items %u",       
        ser_if_data_ptr->dsm_pool_id, 
        DSM_POOL_FREE_CNT(ser_if_data_ptr->dsm_pool_id));

      /* mutual exclusion between DS and HS-USB0. HS-USB0 may handle rx_done at
         same time */
      jsafe_enter();

      if (ser_if_data_ptr->rx_info.inbound_flow_enabled == FALSE)
      {
        ser_if_data_ptr->rx_info.inbound_flow_enabled = TRUE;

        // Signal AL task to drain ACM rx queue
        hsu_al_task_forward_acm_rx_dsm(NULL);
        
        if (ser_if_data_ptr->rx_info.num_buf_waiting == 0)
        {
          /* There are no buffers waiting to be sent to the application.
           * Check if we need to issue cdc_read.
           * This can occur in the following scenario:
           * 1. Suspend occurs
           * 2. Application disables inbound flow
           * 3. Resume occurs (remote wakeup to send the disable inbound flow)
           * 4. cdc_read is not issued in resume because flow is disabled
           * 5. Application enables inbound flow
           */
          if ((!ser_if_data_ptr->rx_info.dsm_chain_ptr) && 
              (ser_if_data_ptr->instance_enabled_by_cdc_fd)) 
          {
            HSU_MSG_HIGH("*_acm_ioctl: flow enabled, calling setup_read");
            
            hsu_al_ser_base_i_rx_setup_read(ser_if_data_ptr);
          }

          jsafe_leave();
          break;
        }

        HSU_MSG_HIGH_1("*_acm_ioctl: forwarding %d waiting buffers",
          ser_if_data_ptr->rx_info.num_buf_waiting);

        hsu_al_ser_base_i_rx_done_from_flow_enable(
          &ser_if_data_ptr->rx_info.buf_status_array[
            ser_if_data_ptr->rx_info.num_buf_already_forwarded],
          (void**)&ser_if_data_ptr->rx_info.buf_array[
            ser_if_data_ptr->rx_info.num_buf_already_forwarded],
          &ser_if_data_ptr->rx_info.buf_size_array[
            ser_if_data_ptr->rx_info.num_buf_already_forwarded],
          ser_if_data_ptr->rx_info.num_buf_waiting,
          (cdc_appctx_t)acm_data_ptr);

        ser_if_data_ptr->rx_info.num_buf_waiting = 0;
      }

      jsafe_leave();
      break;
    }

    case SIO_IOCTL_INBOUND_FLOW_DISABLE:
    {
#ifdef FEATURE_HS_USB_ACM_BAM
      if (modem_data_ptr->sps_info.is_sps_mode)
      {
        HSU_MSG_ERROR("SIO_IOCTL_INBOUND_FLOW_DISABLE: called in SPS mode");

        /* If we return FLASE, hsu_al_ser_ioctl() assertion will fail. */
        return TRUE;
      }
#endif /* FEATURE_HS_USB_ACM_BAM */

      HSU_MSG_HIGH_2("*_acm_ioctl: SIO_IOCTL_INBOUND_FLOW_DISABLE: "
        "pool 0x%X, free items %u",
          ser_if_data_ptr->dsm_pool_id, 
          DSM_POOL_FREE_CNT(ser_if_data_ptr->dsm_pool_id));

      ser_if_data_ptr->rx_info.inbound_flow_enabled = FALSE;
      /* In the next Rx done callback, this will be taken into account */
      break;
    }
    case SIO_IOCTL_SET_BUFFER_SIZE:
    {
      HSU_MSG_HIGH_3("*_acm_ioctl: SIO_IOCTL_SET_BUFFER_SIZE - Nothing to do", 
        0, 0, 0);
      acm_data_ptr->modem_if_data.sio_ioctl_set_buffer_size_mirror = 
        ioctl_param_ptr->buffer_size;
      break;
    }
    case SIO_IOCTL_GET_BUFFER_SIZE:
    {
      HSU_MSG_HIGH("*_acm_ioctl: SIO_IOCTL_GET_BUFFER_SIZE - Nothing to do");
      ioctl_param_ptr->buffer_size =
        acm_data_ptr->modem_if_data.sio_ioctl_set_buffer_size_mirror;
      break;
    }
    case SIO_IOCTL_SET_PACKET_SIZE:
    {
      HSU_MSG_HIGH_3("*_acm_ioctl: SIO_IOCTL_SET_PACKET_SIZE. packet_size=%d", 
        ioctl_param_ptr->packet_size, 0, 0);
      ser_if_data_ptr->rx_info.rx_buff_size_for_read = 
        al_ser_acm_verify_buff_size(ser_if_data_ptr, 
          ioctl_param_ptr->packet_size, 
          ser_if_data_ptr->rx_info.max_buf_size);

      HSU_ASSERT(ser_if_data_ptr->rx_info.rx_buff_size_for_read > 0);

      ser_if_data_ptr->rx_info.array_size = 
        ser_if_data_ptr->rx_info.rx_buff_size_for_read / 
        ser_if_data_ptr->rx_info.dsm_item_size_rounded_down_to_512;

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

      
      break;
    }
    case SIO_IOCTL_GET_PACKET_SIZE:
    {
      HSU_MSG_HIGH_1("acm_ioctl: SIO_IOCTL_GET_PACKET_SIZE. packet_size=%d", 
        ser_if_data_ptr->rx_info.rx_buff_size_for_read);
      ioctl_param_ptr->packet_size = ser_if_data_ptr->rx_info.rx_buff_size_for_read;
      break;
    }
    default:
      if (ACM_IOCTL_SET_RX_TIMEOUT == (int)ioctl_cmd)
      {
        rx_timeout = (uint32)ioctl_param_ptr;
        HSU_MSG_HIGH_1("*_acm_ioctl: ACM_IOCTL_SET_RX_TIMEOUT. read multi timeout=%d", rx_timeout);
        ser_if_data_ptr->rx_info.rx_timeout = rx_timeout;
        break;
      }
      else
      {
        HSU_MSG_ERROR_1("hsu_al_ser_acm_ioctl: cmd %u not supported", ioctl_cmd);
        return FALSE;
      }
  }

  return TRUE;
}

/*===========================================================================
FUNCTION hsu_al_ser_acm_open()
Should be called with Giant mutex locked!
===========================================================================*/
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
)
{
  HSU_ASSERT(acm_data_ptr != NULL);

  HSU_MSG_HIGH("*_acm_open: entered");

  if (!hsu_os_is_giant_locked_by_curr_task())
  {
    HSU_ERR_FATAL("hsu_al_ser_acm_open: Should be called "
            " with Giant mutex locked!", 0, 0, 0);
    return NULL;    
  }

#ifdef FEATURE_HS_USB_ACM_BAM
#ifndef FEATURE_HS_USB_BAM_A2_STUB
  if (open_ptr->open_param == NULL)
  {
    HSU_MSG_LOW("hsu_al_ser_acm_open: opening in SIO_USB_MODE_LEGACY mode"
      " due to NULL open param.");
    acm_data_ptr->modem_if_data.sps_info.is_sps_mode = FALSE;
  }
  else
  {
    switch (open_ptr->open_param->usb_end_point_info.mode)
    {
      case SIO_USB_MODE_LEGACY:
        HSU_MSG_LOW("hsu_al_ser_acm_open: opening in SIO_USB_MODE_LEGACY mode");
        acm_data_ptr->modem_if_data.sps_info.is_sps_mode = FALSE;
      break;
      case SIO_USB_MODE_SPS_USB_BAM:
        HSU_MSG_LOW("hsu_al_ser_acm_open: opening in SIO_USB_MODE_SPS_USB_BAM mode");
        acm_data_ptr->modem_if_data.sps_info.is_sps_mode = TRUE;
        /* We'll use SPS_ENDPOINT_A2_0 and SPS_ENDPOINT_A2_1 for RmNET. */
        acm_data_ptr->modem_if_data.sps_info.sio_port_id = SIO_PORT_USB_MDM; /* by design go on 3rd interface in hsu_al_sps_utils */
        break;
      case SIO_USB_MODE_SPS_USB_SYSTEM:
        HSU_MSG_ERROR("hsu_al_ser_acm_open: SIO_USB_MODE_SPS_USB_SYSTEM "
          " is not supported.");
        acm_data_ptr->modem_if_data.sps_info.is_sps_mode = FALSE;
        break;
      default:
        HSU_MSG_ERROR_1("hsu_al_ser_acm_open: unknown SPS mode %d",
          open_ptr->open_param->usb_end_point_info.mode);
        acm_data_ptr->modem_if_data.sps_info.is_sps_mode = FALSE;
    }
  }
#else /* FEATURE_HS_USB_BAM_A2_STUB */
  /* If A2 stub is in use, assume SPS is enabled in BAM-System mode */
  acm_data_ptr->modem_if_data.sps_info.is_sps_mode = TRUE;
  /* choice of BAM2BAM vs. SYSTEM memory mode is done in the hsu_al_sps_utils layer */
#endif /* !FEATURE_HS_USB_BAM_A2_STUB */
#endif /* FEATURE_HS_USB_ACM_BAM */

  /* When dynamic configurations are supported, it is possible that an app 
  requested to open a HSU port, but without the HSU stack initializing it.
  To verify this, instance_of_fd_used must be checked to be non NULL.
  It is assumed that hsu_al_ser_base_init was called previously for this instance. */
  if (acm_data_ptr->base_data.instance_of_fd_used == NULL)
  {
    HSU_MSG_ERROR_1("*_acm_open: instance not initialized by stack. port_id=%u",
      open_ptr->port_id);
    return FALSE;
  }

  /* The following scenario is possible: 
    0. enable callback not yet invoked
    1. Inside hsu_al_ser_base_i_set_params_in_open_close, memory_pool_for_rx was set
    2. CDC FD invokes enable -> causes cdc_read operation
    3. (context returned here): because enable was invoked
    ser_if_data_ptr->instance_enabled_by_cdc_fd is TRUE , so yet another read is 
    set...
    This whole function is protected by Giant mutex
  */

#ifdef FEATURE_HS_USB_ACM_BAM
  if (acm_data_ptr->modem_if_data.sps_info.sps_enabled)
  {
    hsu_al_ser_base_i_open_set_data(
      &acm_data_ptr->base_data, 
      open_ptr->port_id,
      open_ptr->stream_id, 
      open_ptr->stream_mode,
      NULL, 
      NULL, 
      NULL, 
      pool_id,
      rx_buff_cnt,
      0,
      0,
      0,
      0,
      open_ptr->rx_flow, 
      open_ptr->tx_flow,
      acm_data_ptr);    
  }
  else
#endif /* FEATURE_HS_USB_ACM_BAM */
  {
    hsu_al_ser_base_i_open_set_data(
      &acm_data_ptr->base_data,
      open_ptr->port_id,
      open_ptr->stream_id,
      open_ptr->stream_mode,
      open_ptr->tx_queue, 
      open_ptr->rx_queue, 
      open_ptr->rx_func_ptr, 
      pool_id,
      rx_buff_cnt,
      rx_timeout,
      tx_max_pending_chain_cnt,
      tx_timeout,
      tx_complete_timer_val,
      open_ptr->rx_flow, 
      open_ptr->tx_flow,
      acm_data_ptr);
  }

  /* Skip the use of DS DSM pool level notification for DIAG on modem port. */
  if (acm_data_ptr->base_data.stream_mode != SIO_DM_MODE)
  {
    hsu_al_dsm_utils_init(ACM_RX_DSM_ITEM_POOL);
  }

  if (acm_data_ptr->base_data.instance_enabled_by_cdc_fd == TRUE)
  {
    /* Reaching this point means enable callback was already called */
    
    /* Now, even if a disable cb is invoked, hsu_al_ser_acm_setup_read 
    checks instance_enabled_by_cdc_fd (without interference by FD) prior to calling 
    cdc_read */

    /* Assuming there can't be a call to disable cb and then enable cb before 
    the call to cdc_read inside hsu_al_ser_acm_setup_read */

#ifdef FEATURE_HS_USB_ACM_BAM
    if ((acm_data_ptr->modem_if_data.sps_info.is_sps_mode) && 
        ((acm_data_ptr->modem_if_data.hsu_al_acm_suspend_resume_mode) != HSU_BUS_MODE_SUSPENDED))
    {
      if (hsu_al_acm_enable_sps(acm_data_ptr) == FALSE)
      {
        HSU_ERR_FATAL("_ser_acm_internal_open: enable_sps failed", 0, 0, 0);
        /* TODO: implement fall-back to legacy mode. make sure the app provided
        all data related parameters (tx/rx queue, etc) */
        acm_data_ptr->modem_if_data.sps_info.is_sps_mode = FALSE;
        hsu_al_ser_base_i_rx_setup_read(&acm_data_ptr->base_data);
      }
    }
    else if(FALSE == acm_data_ptr->modem_if_data.sps_info.is_sps_mode)
    {
      hsu_al_ser_base_i_rx_setup_read(&acm_data_ptr->base_data);
    }
#else
    hsu_al_ser_base_i_rx_setup_read(&acm_data_ptr->base_data);
#endif /* FEATURE_HS_USB_ACM_BAM */
  }
  else
  {
    /* Reaching this point means enable callback was not yet called, 
    but there is no problem it would be called from now on */
  }

  return TRUE;
}

/*===========================================================================
FUNCTION hsu_al_ser_acm_close()
===========================================================================*/
void hsu_al_ser_acm_close
(
  hsu_al_ser_acm_data* acm_data_ptr,
  hsu_al_ser_base_op_complete_cb close_cb,
  boolean abort_write_op_immediately
)
{
  HSU_ASSERT(acm_data_ptr != NULL);

  HSU_MSG_HIGH("*_acm_close: entered");

  /* Skip the use of DS DSM pool level notification for DIAG on modem port. */
  if (acm_data_ptr->base_data.stream_mode != SIO_DM_MODE)
  {
    hsu_al_dsm_utils_uninit(ACM_RX_DSM_ITEM_POOL);
  }

  hsu_al_ser_base_close(
    &acm_data_ptr->base_data, 
    close_cb, 
    abort_write_op_immediately);
}

/*==============================================================================

INTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/
/*===========================================================================

FUNCTION al_ser_acm_ignore_encap_command

DESCRIPTION
Called whenever the ACM FD receives a SEND_ENCAPUSLATED_COMMAND from the host
(CDC Spec. chapter 6.2.1).
This request is not supported and is not expected to be received.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_ignore_encap_command
(
  void* command,        /* Pointer to data representing the encap. command */
  juint16_t size,       /* Length, in bytes, of the received command       */
  cdc_appctx_t ctx      /* Context provided to the ACM FD, in acm_init     */
)
{
  HSU_USE_PARAM(command);
  HSU_USE_PARAM(size);
  HSU_USE_PARAM(ctx);
  HSU_MSG_ERROR(
      "*_ignore_encap_command(): encapsulated commands not supported in ACM");
  return JENOTSUP;
}

/*===========================================================================

FUNCTION al_ser_acm_ignore_encap_response

DESCRIPTION
Called whenever the ACM FD receives a GET_ENCAPUSLATED_RESPONSE from the host
(CDC Spec. chapter 6.2.2).
This request is not supported and is not expected to be received.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_acm_ignore_encap_response
(
  cdc_iostatus_t status, /* indication from host if the response was 
                         received */
  juint32_t bytes_sent,  /* number of bytes actually sent */
  cdc_appctx_t ctx       /* Context provided to the ACM FD, in acm_init    */
)
{
  HSU_USE_PARAM(status);
  HSU_USE_PARAM(bytes_sent);
  HSU_USE_PARAM(ctx);

  HSU_ERR_FATAL("encapsulated commands not supported in ACM", 0, 0, 0);
}



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
void hsu_al_ser_acm_flush_tx(hsu_al_ser_acm_data* acm_data_ptr,
                        hsu_al_ser_base_op_complete_cb flush_cb)
{
   hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = &acm_data_ptr->base_data;

#ifdef FEATURE_HS_USB_ACM_BAM
  if (acm_data_ptr->modem_if_data.sps_info.is_sps_mode)
  {
    HSU_MSG_ERROR("acm_flush_tx: called in SPS mode");
    return;
  }
#endif /* FEATURE_HS_USB_ACM_BAM */

   /* check if instance_of_fd_used is valid. If instance_of_fd_used is NULL, 
   * call flush_cb to inform caller of ending hsu_al_ser_acm_flush_tx.
   */
  if(!acm_data_ptr->base_data.instance_of_fd_used)
  {
    HSU_ULOG_ENUM(HIGH_MSG, DATA_LOG, "hsu_al_ser_acm_flush_tx, invoke_flush_cb", acm_data_ptr->base_data.iface_type, HSU_IFACE_OFFSET2);
    flush_cb(ser_if_data_ptr->arg_cb);
    return;
  }


  hsu_al_ser_base_flush_tx(&(acm_data_ptr->base_data), flush_cb); 
}
/*===========================================================================
FUNCTION al_ser_acm_flush_tx_from_ioctl_cb

DESCRIPTION
this function is used for a special case: 
when a SIO_IOCTL_CHANGE_MODE cmd is received, 
and the mode is changed from SIO_DS_AUTODETECT_MODE  to a different mode,
we need to flush the tx queue.
The application does not expect that - therefor it does not supply a CB.
The ACM itself supplies this decadent CB instead. 

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
static void al_ser_acm_flush_tx_from_ioctl_cb
(
 void* arg_cb
)
{
  ((hsu_al_ser_acm_data*)arg_cb)->modem_if_data.flush_tx_ended = TRUE;
  HSU_MSG_LOW("*_flush_tx_from_ioctl_cb: ended");
}
/*===========================================================================

FUNCTION al_ser_acm_set_line_coding_cb

DESCRIPTION
Called whenever the ACM FD receives a SET_LINE_CODING from the host
(CDC Spec. chapter 6.2.12). It will simply save the received data, so that
when a GET_LINE_CODING occurs, it will return the data it has just stored.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_set_line_coding_cb
(
  acm_line_coding_t* coding, /* Pointer to struct holding the line coding 
                             values, as was given by the host */
  cdc_appctx_t ctx           /* Context provided to the ACM FD, in acm_init*/
)
{
  hsu_al_ser_acm_data* acm_data_ptr = 
    (hsu_al_ser_acm_data*)ctx;
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = &acm_data_ptr->base_data;

  HSU_PARAM_COULD_BE_CONST(coding);

  HSU_ASSERT(coding != NULL);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  acm_data_ptr->modem_if_data.line_coding = *coding;

  return HSU_JSUCCESS;
}

/*===========================================================================

FUNCTION al_ser_acm_get_line_coding_cb

DESCRIPTION
Called whenever the ACM FD receives a GET_LINE_CODING from the host
(CDC Spec. chapter 6.2.13). It will simply provide the ACM FD the line coding 
data it had previously stored.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_get_line_coding_cb
(
  acm_line_coding_t* coding, /* Pointer to struct holding the line coding 
                             values, as will be given to the host */
  cdc_appctx_t ctx           /* Context provided to the ACM FD, in acm_init*/
)
{
  hsu_al_ser_acm_data* acm_data_ptr = 
    (hsu_al_ser_acm_data*)ctx;
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = &acm_data_ptr->base_data;

  HSU_ASSERT(coding != NULL);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  *coding = acm_data_ptr->modem_if_data.line_coding;

  return HSU_JSUCCESS;  
}

/*===========================================================================

FUNCTION al_ser_acm_send_recent_serial_state

DESCRIPTION
Sends the last serial state notification if it is different from the last one the host received. 
Called from resumed and when we get DTE ready from host to resolve cases where the 
messages was not sent to the host (we got suspended or the application
on the host was not open yet).


DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS for succees

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_send_recent_serial_state(hsu_al_ser_acm_data* acm_data_ptr)
{
  jresult_t notify_serial_state_result;

  if ((acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host != 0) &&
    (acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host != 
    acm_data_ptr->modem_if_data.last_serial_state_bitmap_rxed_by_host))
  {
    notify_serial_state_result = 
      al_ser_acm_notify_serial_state_if_enabled(acm_data_ptr);

    if (notify_serial_state_result != HSU_JSUCCESS)
    {
      HSU_MSG_ERROR_2("_send_recent_serial_state: "
        "Failed to send serial state =%d, notify_serial_state_result", 
        acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host, 
        notify_serial_state_result);
    }
    return notify_serial_state_result;
  }
  return HSU_JSUCCESS;
}
/*===========================================================================

FUNCTION al_ser_acm_set_control_line_state_cb

DESCRIPTION
Called whenever the ACM FD receives a SET_CONTROL_LINE_STATE from the host
(CDC Spec. chapter 6.2.14). It will simply check if the DTR (Data Terminal Ready)
is different than what was previously saved. If so, will update the new DTR and 
invoke the modem's DTR flag changed callback.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_set_control_line_state_cb
(
  juint16_t ctrl_sig_bitmap, /* Bitmap representing the control line's 
                             state */
  cdc_appctx_t ctx           /* Context provided to the ACM FD, in acm_init*/
)
{
  hsu_al_ser_acm_data* acm_data_ptr = 
    (hsu_al_ser_acm_data*)ctx;
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = &acm_data_ptr->base_data;
  juint16_t orig_ctrl_line_state;

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* Save original ctrl_line_state and update new ctrl line state prior
  to invoking client callback (since if client DTR callback immediately checks 
  new ctrl line state, it will already be updated)*/
  orig_ctrl_line_state = acm_data_ptr->modem_if_data.ctrl_line_state;
  acm_data_ptr->modem_if_data.ctrl_line_state = ctrl_sig_bitmap;

  /* In case we get DTE ready we need to resend the serial state notification, 
  for cases where we tried to send it but the application on the host side
  was not ready yet */
  if ((acm_data_ptr->modem_if_data.ctrl_line_state & AL_SER_ACM_DTR_ASSERTED_MASK)
    != 0)
  {
    (void)al_ser_acm_send_recent_serial_state(acm_data_ptr);
  }

  /* Check if the DTR bit is different between the given ctrl_sig_bitmap and
  the current ctrl_line_state: */
  if ((orig_ctrl_line_state & AL_SER_ACM_DTR_ASSERTED_MASK) !=
    (ctrl_sig_bitmap & AL_SER_ACM_DTR_ASSERTED_MASK))
  {
    if (acm_data_ptr->modem_if_data.app_dtr_changed_cb_func != NULL)
    {
      acm_data_ptr->modem_if_data.app_dtr_changed_cb_func();
	  HSU_ULOG_2(ERROR_MSG, BUS_LOG, "changed DTR, invoking callback, prev DTR=%d new DTR=%d",
		(orig_ctrl_line_state & AL_SER_ACM_DTR_ASSERTED_MASK),
		(ctrl_sig_bitmap & AL_SER_ACM_DTR_ASSERTED_MASK));
    }
  }

  return HSU_JSUCCESS;
}


/*===========================================================================

FUNCTION al_ser_acm_notify_serial_state_cb

DESCRIPTION
Called whenever the ACM FD notifies completion of a previous call to 
acm_notify_serial_state.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_acm_notify_serial_state_cb
(
  cdc_iostatus_t status,                               /* Operation status */
  cdc_appctx_t ctx           /* Context provided to the ACM FD, in acm_init*/
)
{
  jresult_t notify_serial_state_result;
  hsu_al_ser_acm_data* acm_data_ptr = 
    (hsu_al_ser_acm_data*)ctx;
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = &acm_data_ptr->base_data;


  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  HSU_ASSERT(status != CDC_TRANSFER_ERROR);


  acm_data_ptr->modem_if_data.serial_state_notification_in_progress = FALSE;

  HSU_ULOG_ENUM_1(NO_MSG, DATA_LOG, "last_serial_state_send_to_host 0x%08X", 
    status, HSU_TX_CDC_TRANSFER_OFFSET2, 
    acm_data_ptr->modem_if_data.last_serial_state_bitmap_sent_to_host);

  if (status == CDC_TRANSFER_CANCELLED)
  {
    HSU_MSG_HIGH("*_notify_serial_state_cb: canceled");

    /* We can get the CDC_TRANSFER_CANCELLED in case the application on the host was not ready, 
       or if we requested to send the serial state and there is a suspend and in case of 
       cable disconnect (disable). 
       Therefore, we keep acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host
       and we will send it when we get DTE ready, when we resume and in enable.*/
    acm_data_ptr->modem_if_data.last_serial_state_bitmap_sent_to_host = 0;
    acm_data_ptr->modem_if_data.last_serial_state_bitmap_rxed_by_host = 0;
    return;
  }
  /* else: status == CDC_TRANSFER_OK */

  /* Host acks it received last_serial_state_bitmap_sent_to_host, update accordingly: */
  acm_data_ptr->modem_if_data.last_serial_state_bitmap_rxed_by_host = 
    acm_data_ptr->modem_if_data.last_serial_state_bitmap_sent_to_host;

  HSU_MSG_MED_1("*_notify_serial_state_cb: host acked updated ser. state: %u", 
    acm_data_ptr->modem_if_data.last_serial_state_bitmap_rxed_by_host);

  /* Check if there is a need to send a more updated serial state to host: */
  if (acm_data_ptr->modem_if_data.last_serial_state_bitmap_sent_to_host
    != acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host)
  {
    /* Since this callback is called from DSR context, it means
    that the FD is still enabled, therefore it is guaranteed that a call will be
    made to acm_notify_serial_state. The reason 
    al_ser_acm_notify_serial_state_if_enabled is used here is to avoid code
    duplication: */
    HSU_ASSERT(acm_data_ptr->base_data.instance_enabled_by_cdc_fd == TRUE);

    HSU_ULOG(NO_MSG, DATA_LOG, "update_latest_pending_state");

    notify_serial_state_result = 
      al_ser_acm_notify_serial_state_if_enabled(acm_data_ptr);

    if (notify_serial_state_result != HSU_JSUCCESS)
    {
      HSU_MSG_ERROR_2("_notify_serial_state_cb: "
        "Failed to send serial state =%d, notify_serial_state_result", 
        acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host, 
        notify_serial_state_result);
    }
    else
    {
      HSU_MSG_HIGH("*_notify_serial_state_cb: sent a more updated ser. state");
    }
  }
}

/*===========================================================================

FUNCTION al_ser_acm_rx_done

DESCRIPTION
Rx Callback whenever the ACM FD wishes to notify some buffers has been received
over the data class I/F.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_acm_rx_done
(
 cdc_iostatus_t *status,        /* array of statuses for each transfer */
 void **buffers,                /* array of buffers with data */
 juint32_t *bytes_transferred,  /* array of sizes for each buffer */
 juint32_t req_num,             /* number of request in current callback (if 
                                timeout occurred it is not the same as the
                                number passed to the cdc_read_multiple) */
 cdc_appctx_t ctx
)
{ 
#ifdef FEATURE_HS_USB_ACM_BAM
  hsu_al_ser_acm_data *acm_data_ptr = (hsu_al_ser_acm_data *)ctx;

  if (acm_data_ptr->modem_if_data.sps_info.is_sps_mode)
  {
    /* rx_done is expected in SPS mode (with status CDC_TRANSFER_CANCELLED) 
     * upon cable disconnect and USB suspend */
    HSU_MSG_HIGH("al_ser_acm_rx_done_internal: called in SPS mode");
    acm_data_ptr->base_data.rx_info.read_oper_is_in_progress = FALSE;
    return;
  }
#endif /* FEATURE_HS_USB_ACM_BAM */

  hsu_al_ser_base_i_rx_done(status, buffers, bytes_transferred, req_num, ctx);
}


/*==============================================================================

FUNCTION al_ser_acm_enabled_internal

DESCRIPTION
Continue the enabled operation

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_enabled_internal
(
  cdc_appctx_t ctx
)
{
  hsu_al_ser_acm_data* acm_data_ptr = 
    (hsu_al_ser_acm_data*)ctx;
  hsu_al_ser_base_data_for_ser_if* app_data_ptr = &acm_data_ptr->base_data;

  /* The Giant is locked so this section is already protected*/
  if (app_data_ptr->instance_enabled_by_cdc_fd == TRUE)
    {
    HSU_MSG_ERROR("*_acm_enabled_internal: instance already enabled");
    return HSU_JSUCCESS;
  }

  app_data_ptr->instance_enabled_by_cdc_fd = TRUE;

  /* Check if the app is in closing process (or already closed): */
  /* If so, don't do any reads...*/
  if ((app_data_ptr->num_of_aborts_issued_prior_to_close > 0)
    || (hsu_al_ser_base_i_has_already_been_closed(app_data_ptr)) == TRUE)
  {
    HSU_MSG_HIGH("*_acm_enabled_internal: app closed/closing");
  }
  else
  {
    /* Setup a new read, but only if open already issued (or if interface 
       is using read) and if the inbound flow is enabled:*/
    if (app_data_ptr->rx_info.inbound_flow_enabled)
    {
#ifdef FEATURE_HS_USB_ACM_BAM
      if (acm_data_ptr->modem_if_data.sps_info.is_sps_mode)
      {
        if (hsu_al_acm_enable_sps(acm_data_ptr) == FALSE)
        {
          HSU_ERR_FATAL("al_ser_acm_enabled_internal: enable_sps failed", 
            0, 0, 0);
          /* TODO: implement fall-back to legacy mode. make sure the app provided
          all data related parameters (tx/rx queue, etc) */
          acm_data_ptr->modem_if_data.sps_info.is_sps_mode = FALSE;
          hsu_al_ser_base_i_rx_setup_read(app_data_ptr);
        }
      }
      else
      {
        hsu_al_ser_base_i_rx_setup_read(app_data_ptr);
      }
#else
      hsu_al_ser_base_i_rx_setup_read(app_data_ptr);
#endif /* FEATURE_HS_USB_ACM_BAM */

    }
    else
    {
      HSU_MSG_HIGH("*_acm_enabled_internal: inbound flow is disabled"
                   " not issuing cdc_read");
    }
  }

  return HSU_JSUCCESS;
}

/*===========================================================================

FUNCTION al_ser_acm_enabled

DESCRIPTION
Callback called whenever the ACM FD wishes to notify the instance is enabled.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Success. Otherwise, see hsu_al_ser_base_i_enabled.

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_enabled
(
  cdc_appctx_t ctx                           /* Context passed in callback */
)
{
  jresult_t result;
  hsu_al_ser_acm_data* acm_data_ptr = 
    (hsu_al_ser_acm_data*)ctx;

  HSU_MSG_HIGH("al_ser_acm_enabled: Enter");

  /* Set the FD handle to the current configuration's FD */
  /* Assume ACM is supported for primary core only. Otherwise, core_index 
     should be added to ACM data structure */
	hsu_al_ser_handle_set_configuration(HSU_CORE_IDX_PRIMARY);

  result = al_ser_acm_enabled_internal(ctx);
  if (result != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR_1("al_ser_acm_enabled: al_ser_acm_enabled_internal failed %d",
      result);
    return result;
  }

  /* send an updated serial_state: 
  */

  /* Since this callback means the FD is enabled, therefore it is guaranteed 
  that a call will be made to acm_notify_serial_state. The reason 
  al_ser_acm_notify_serial_state_if_enabled is used here is to avoid code
  duplication: */
  result = al_ser_acm_notify_serial_state_if_enabled(acm_data_ptr);
  HSU_ASSERT(result == HSU_JSUCCESS);

  acm_data_ptr->modem_if_data.hsu_al_acm_suspend_resume_mode = HSU_BUS_MODE_RESUMED;

  HSU_MSG_HIGH("al_ser_acm_enabled: Exit");
  return HSU_JSUCCESS;
}

/*===========================================================================

FUNCTION al_ser_acm_disabled

DESCRIPTION
Callback called whenever the ACM FD wishes to notify the instance is disabled.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_disabled
(
  cdc_appctx_t ctx                           /* Context passed in callback */
)
{
  jresult_t base_i_disabled_result;
  hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)ctx;
  //cdc_ctx_t *cdc_ctx = (cdc_ctx_t*)(acm_data_ptr->base_data.instance_of_fd_used);

  /* Call base class: shouldn't fail */
  base_i_disabled_result = hsu_al_ser_base_i_disabled(ctx);
  HSU_ASSERT(base_i_disabled_result == HSU_JSUCCESS);

  /* commented for DTR propogation on disconnect */
  //if (acm_data_ptr->base_data.tx_info.recover_dropped_packet == FALSE)
  {
    /* TX cleanup. stack will clean any pending buffers with tx_done. The other
    data that was not yet passed to stack remains and will be sent after next
    cable connection upon hsu_al_ser_acm_transmit */
    hsu_timer_cancel(&acm_data_ptr->base_data.tx_info.tx_complete_timer);
    acm_data_ptr->base_data.tx_info.is_tx_complete_timer_scheduled = FALSE;

    hsu_timer_cancel(&acm_data_ptr->base_data.rx_info.rx_throttle_timer);
    acm_data_ptr->base_data.rx_info.is_rx_throttle_timer_scheduled = FALSE;

    acm_data_ptr->base_data.tx_info.is_write_in_progress = FALSE;

    /* Call DTE Ready Callback, but do so via set ctrl line state, to simulate
    as if a terminal was connected and it issued a normal disconnection. */
    (void)al_ser_acm_set_control_line_state_cb(0, ctx);

#ifdef FEATURE_HS_USB_ACM_BAM
    hsu_al_acm_disable_sps(acm_data_ptr);
#endif /* FEATURE_HS_USB_ACM_BAM */
  }

  return HSU_JSUCCESS;
}

/*===========================================================================

FUNCTION al_ser_acm_resumed_internal

DESCRIPTION
Continue the resumed operation by checking if a new read operation should
be performed

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
jresult_t al_ser_acm_resumed_internal
(
  cdc_appctx_t ctx
)
{
  hsu_al_ser_acm_data* acm_data_ptr = 
    (hsu_al_ser_acm_data*)ctx;
  hsu_al_ser_base_data_for_ser_if* app_data_ptr = &acm_data_ptr->base_data;

  if (app_data_ptr->instance_enabled_by_cdc_fd == FALSE)
  {
    HSU_MSG_LOW("*_acm_resumed_internal: instance not enabled");
    return HSU_JSUCCESS;
  }

  if (hsu_al_ser_base_i_has_already_been_closed(app_data_ptr) == TRUE)
  {
    HSU_MSG_LOW("*_acm_resumed_internal: app not using instance");
    return HSU_JSUCCESS;
  }

#ifdef FEATURE_HS_USB_ACM_BAM
  if (acm_data_ptr->modem_if_data.sps_info.is_sps_mode)
  {
    if (hsu_al_acm_enable_sps(acm_data_ptr) == FALSE)
    {
      HSU_ERR_FATAL("al_ser_acm_resumed_internal: enable_sps failed", 0, 0, 0);
      /* TODO: implement fall-back to legacy mode. make sure the app provided
      all data related parameters (tx/rx queue, etc) */
      acm_data_ptr->modem_if_data.sps_info.is_sps_mode = FALSE;
      hsu_al_ser_base_i_rx_setup_read(app_data_ptr);
    }
    else
    {
      if (hsu_al_sps_unregister_wakeup_event(&acm_data_ptr->modem_if_data.sps_info.connection) == FALSE)
      {
        HSU_MSG_ERROR(
          "*_acm_resumed_internal: unregister_wakeup_event failed");
      }
    }
  }
  else
  {
    hsu_al_ser_base_i_rx_setup_read(app_data_ptr);
  }
#else
  hsu_al_ser_base_i_rx_setup_read(app_data_ptr);
#endif /* FEATURE_HS_USB_ACM_BAM */

  return HSU_JSUCCESS;
}

/*===========================================================================

FUNCTION al_ser_acm_resumed

DESCRIPTION
Callback called whenever the ACM FD wishes to notify resumed occurred.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_resumed
(
  cdc_appctx_t ctx                           /* Context passed in callback */  
)
{
  jresult_t result;
  hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)ctx;
  hsu_al_ser_base_data_for_ser_if* app_data_ptr = &acm_data_ptr->base_data;

  HSU_MSG_HIGH("al_ser_acm_resumed: Enter");

  /* In case we are resumed we need to resend the serial state notification, 
     for cases where we requested to send it but then got suspended */  
  (void)al_ser_acm_send_recent_serial_state(acm_data_ptr);

  if (
#ifdef FEATURE_HS_USB_ACM_BAM
      (!acm_data_ptr->modem_if_data.sps_info.is_sps_mode) &&
#endif /* FEATURE_HS_USB_ACM_BAM */
      (app_data_ptr->tx_info.recover_dropped_packet)
    )
  {
    /* To resume TX (Downlink) */
    hsu_al_ser_i_base_tx_set_handle_complete_timer(app_data_ptr);
  }

  if ((acm_data_ptr->base_data.rx_info.inbound_flow_enabled == FALSE) ||
      (acm_data_ptr->base_data.rx_info.num_buf_waiting != 0))
  {
    /* Either inbound flow disabled, or it is in the process of
       forwarding a waiting DSM chain to the application. In both cases, 
    do not issue a cdc_read. */
    return HSU_JSUCCESS;  
  }
  /* else: need to issue cdc_read */

  result = al_ser_acm_resumed_internal(ctx);

  HSU_MSG_HIGH("al_ser_acm_resumed: Exit");

  return result;
}

/*===========================================================================

FUNCTION AL_SER_ACM_UNINIT

DESCRIPTION
Serves as a callback whenever the CDC FD wishes to notify it is being 
un-initialized.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Success. Otherwise, see hsu_al_ser_base_i_uninit.

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_uninit
(
  cdc_appctx_t ctx                           /* Context passed in callback */
)
{
  hsu_bus_mode_multi_core_cb_info_struct suspend_resume_callback_info;

  hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)ctx;
  cdc_ctx_t *cdc_ctx = (cdc_ctx_t*)(acm_data_ptr->base_data.instance_of_fd_used);
  core_t *core = (core_t*)(cdc_ctx->core_ctx);

  suspend_resume_callback_info.core_index = (uint32)(core->core_number); /* HAL core index */
  suspend_resume_callback_info.bus_mode_cb = hsu_al_acm_suspend_resume_mode_changed_cb;
  suspend_resume_callback_info.app_data = ctx;
  hsu_unregister_bus_mode_multi_core_notif_cb(suspend_resume_callback_info);

  hsu_al_ser_handle_acm_uninit(ctx);
  return hsu_al_ser_base_i_uninit(ctx);
}

/*===========================================================================

FUNCTION al_ser_acm_notify_serial_state

DESCRIPTION
Serves as a wrapper function for acm_notify_serial_state, by grabbing the
"Giant Mutex", invoking acm_notify_serial_state then releasing the "Giant Mutex".

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Operation Successful. Otherwise - Failure. 
See acm_notify_serial_state for more details.

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_notify_serial_state
(
  cdc_handle_t acm_fd_handle,
  juint16_t serial_state_bitmap
)
{
  jresult_t notify_serial_state_result;

  /* Grab giant mutex */
  jsafe_enter();

  notify_serial_state_result = acm_notify_serial_state(acm_fd_handle, 
    serial_state_bitmap);

  /* Release giant mutex */
  jsafe_leave();

  return notify_serial_state_result;
}

/*===========================================================================

FUNCTION al_ser_acm_notify_serial_state_if_enabled

DESCRIPTION
Sends serial state notification to host, but only if the ACM FD is in an enabled 
state (thus avoiding an error of JENXIO).

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Operation Successful. Otherwise - Failure. 
See al_ser_acm_notify_serial_state for more details.

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_notify_serial_state_if_enabled
(
  hsu_al_ser_acm_data* acm_data_ptr    /* Pointer to ACM data struct */
)
{
  /* Return success by default to satify the caller */
  jresult_t notify_serial_state_result = HSU_JSUCCESS;

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(
    &acm_data_ptr->base_data) == TRUE);

  /* Grab giant mutex */
  jsafe_enter();

  /* Here it is safe to check whether instance enabled or not */
  if (acm_data_ptr->base_data.instance_enabled_by_cdc_fd == FALSE)
  {
    /* Release giant mutex */
    jsafe_leave();

    /* If instance is disabled, there is no point in sending notif */
    HSU_MSG_HIGH_1("al_ser_acm_notify_serial_state_if_enabled: instance 0x%x disabled,\
                 host will not receive notification. Returning success", 
                 acm_data_ptr->base_data.instance_of_fd_used);
    return HSU_JSUCCESS;
  }

  if (acm_data_ptr->modem_if_data.serial_state_notification_in_progress)
  {
    HSU_ULOG_1(MED_MSG, DATA_LOG, "in_prog, bitmap 0x%08X",
      acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host);
  }
  else
  {
    HSU_ULOG_1(MED_MSG, DATA_LOG, "bitmap 0x%08X",
      acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host);
  }

 // HSU_MSG_MED_2("*_notify_serial_state_if_enabled: sending bitmap: %u, in_progress: %u", 
 //   acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host, 
 //   acm_data_ptr->modem_if_data.serial_state_notification_in_progress);

  /* Send the notification only if it is not already in progress */
  if (FALSE == acm_data_ptr->modem_if_data.serial_state_notification_in_progress)
  {
    acm_data_ptr->modem_if_data.serial_state_notification_in_progress = TRUE;
    acm_data_ptr->modem_if_data.last_serial_state_bitmap_rxed_by_host = 0;
    
    notify_serial_state_result = al_ser_acm_notify_serial_state(
      acm_data_ptr->base_data.instance_of_fd_used, 
      acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host);
    
    /* Update the serial state which has just been sent, as the last one ever 
    sent to host */
    acm_data_ptr->modem_if_data.last_serial_state_bitmap_sent_to_host = 
      acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host;
  }

  /* Release giant mutex */
  jsafe_leave();

  return notify_serial_state_result;
}

/*===========================================================================

FUNCTION al_ser_acm_update_serial_state_param

DESCRIPTION
Sends serial state notification to host.

DEPENDENCIES
param_mask must contain only one bit turned on ('1').

RETURN VALUE
TRUE - Operation Successful. Otherwise - Failure. 
See al_ser_acm_notify_serial_state_if_enabled for more details.

SIDE EFFECTS
None

===========================================================================*/
static boolean al_ser_acm_update_serial_state_param
(
  hsu_al_ser_acm_data* acm_data_ptr,         /* Pointer to ACM data struct */
  juint16_t param_mask,                      /* Param mask which indicates
                                             the parameter (CD, RI, or DSR)
                                             to update */
  boolean param_is_asserted                  /* Whether to assert (1) the param 
                                             or deassert (0)*/
)
{
  boolean need_to_notify_serial_state = FALSE;
  jresult_t notify_serial_state_result;
  HSU_ASSERT(acm_data_ptr != NULL);

  /* The giant mutex is used here to prevent the following two scenarios: 
  Scenario I:
  1. The FD is in disabled (i.e. upon startup or upon cable disconnection)
  2. The app issues an IOCTL which causes invoking al_ser_acm_update_serial_state_param, 
  as a result:
  (serial_state_bitmap_to_send_to_host = new value,
  last_serial_state_bitmap_sent_to_host == last_serial_state_bitmap_rxed_by_host 
  != new value), but notification is not sent (since FD disabled)
  3. enabled callback invoked, but since serial_state_bitmap_to_send_to_host != 
  last_serial_state_bitmap_rxed_by_host a notification is sent via call to
  al_ser_acm_notify_serial_state. When this call returns a context switch occurs.
  This happens prior to updating last_serial_state_bitmap_sent_to_host to be
  serial_state_bitmap_to_send_to_host.
  4. The app now issues yet another IOCTL, but since 
  last_serial_state_bitmap_sent_to_host is not updated, the following if statement 
  is not taken (but should have been). 
  5. The app is blocked by giant mutex, but when enabled cb is done, it resumes 
  execution, but since the if was not taken, it wrongly calls 
  al_ser_acm_notify_serial_state_if_enabled,
  causing an assert (since the FD is busy handling the previous request and has not yet 
  invoked its completion callback).


  Scenario II:
  1. The FD is in disabled (i.e. upon startup or upon cable disconnection)
  2. The app issues an IOCTL which causes invoking al_ser_acm_update_serial_state_param, 
  as a result:
  (serial_state_bitmap_to_send_to_host = new value,
  last_serial_state_bitmap_sent_to_host == last_serial_state_bitmap_rxed_by_host 
  != new value), but before it calls al_ser_acm_notify_serial_state_if_enabled
  a context switch occurs, and the FD's enabled callback is invoked.
  3. Since last_serial_state_bitmap_rxed_by_host != serial_state_bitmap_to_send_to_host
  al_ser_acm_notify_serial_state_if_enabled is called.
  4. The app resumes execution, which means it calls 
  al_ser_acm_notify_serial_state_if_enabled as well, causing an assertion for the 
  same reason.
  */

  /* Grab giant mutex */
  jsafe_enter();


  /* Assuming param_mask has only one bit as '1' (the rest are '0')*/

  if (param_is_asserted == TRUE)
  {
    if (
      (acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host & param_mask)
      == 0)
    {
      acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host |= param_mask;
      need_to_notify_serial_state = TRUE;
    }

  }
  else /* param_is_asserted == FALSE */
  {
    if (
      (acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host & param_mask) 
      != 0)
    {
      acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host &= (~param_mask);
      need_to_notify_serial_state = TRUE;
    }
  }

  if (need_to_notify_serial_state == FALSE)
  {
    /* Release giant mutex */
    jsafe_leave();

    HSU_MSG_MED("*_update_serial_state_param: Nothing to update");
    return TRUE;
  }

  HSU_ULOG_1(NO_MSG, DATA_LOG, "serial_state_bitmap 0x%08X",
    acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host);

  notify_serial_state_result = 
    al_ser_acm_notify_serial_state_if_enabled(acm_data_ptr);
  if (notify_serial_state_result != HSU_JSUCCESS)
  {
    /* Release giant mutex */
    jsafe_leave();

    /* ESUSPENDED might happen here, in case of the following scenario: 
    1. Device with Mdm function was enumerated, thus the ACM FD is enabled, but the 
    USB is now disconnected. Only upon reconnecting will we get a disabled followed 
    by enabled callback.
    2. Mdm app sends a SIO_IOCTL_CD_DEASSERT, receiving priority over HSU (so 
    stack DPC which wants to call disable and then re-enable did not get the chance).
    3. We call update serial state, since the last ever txed serial_state == last ever 
    acked by host so we proceed to call acm_notify_serial_state which returns ESUSPENDED
    */
    if (notify_serial_state_result == ESUSPENDED)
    {
      /* Similar to handling cdc_write/cdc_read, nothing is done. Upon receiving the
      enabled callback, the updated serial state will be sent */
      return TRUE;
    }
    /* else: notify_serial_state_result != ESUSPENDED */

    HSU_ERR_FATAL("al_ser_acm_update_serial_state_param: " 
        "al_ser_acm_notify_serial_state_if_enabled returned failure %u", 
        notify_serial_state_result, 0, 0);
  }

  /* Release giant mutex */
  jsafe_leave();

  return TRUE;
}

/*===========================================================================

FUNCTION al_ser_acm_detect_esc_seq

DESCRIPTION
Detects if so far, a partial or full esc. sequence in raw data mode has been 
detected.

A legal esc. sequence should be:

At least AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence,
Escape Character, e.g. '+',
No more than AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence,
Escape Character,
No more than AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence,
Escape Character,
At least AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_acm_detect_esc_seq
(
  rawdata_mode_data_struct* rawdata_data_ptr,
  const char* buffer_ptr,
  juint32_t buffer_size
)
{
  juint32_t esc_seq_detection_itor;

  HSU_ASSERT(rawdata_data_ptr != NULL);
  HSU_ASSERT(buffer_ptr != NULL);
  HSU_ASSERT(buffer_size != 0);

  /* Make sure clk won't interrupt in the middle */
  al_ser_acm_unarm_esc_seq_clk(rawdata_data_ptr);

  if (rawdata_data_ptr->should_wait_for_clk == TRUE)
  {
    /* Received data while waiting for inactivity, reset the clock */
    al_ser_acm_arm_esc_seq_clk(rawdata_data_ptr);
    return;
  }
  /* else: rawdata_data_ptr->should_wait_for_clk == FALSE */

  /* Verify that received buffer contains a partial (or full) legal esc. seq.: */
  if (rawdata_data_ptr->num_of_esc_seq_characters_seen + buffer_size <= 
    AL_SER_ACM_RAWDATA_ESC_SEQ_LENGTH)
  {
    for (esc_seq_detection_itor = 0 ;  esc_seq_detection_itor < buffer_size ; 
      ++esc_seq_detection_itor)
    {
      if (buffer_ptr[esc_seq_detection_itor] != AL_SER_ACM_RAWDATA_ESC_SEQ_CHAR)
      {
        /* Character not valid for esc sequence, need to reset */
        rawdata_data_ptr->should_wait_for_clk = TRUE;
        rawdata_data_ptr->num_of_esc_seq_characters_seen = 0;
        break;
      }
    }
    if (esc_seq_detection_itor == buffer_size)
    {
      /* A partial or full ESC Seq detected, update the additional num of 
      characters seen: */
      /* To overcome VC++ compiler warning C4244*/
      /* It is okay to cast buffer_size to uint8, as the above "if" already verified
      that the sum of rawdata_data_ptr->num_of_esc_seq_characters_seen and buffer_size
      is not greater than AL_SER_ACM_RAWDATA_ESC_SEQ_LENGTH */
      rawdata_data_ptr->num_of_esc_seq_characters_seen = 
        rawdata_data_ptr->num_of_esc_seq_characters_seen + (uint8)buffer_size;
    }
  }
  else /* rawdata_data_ptr->num_of_esc_seq_characters_seen + buffer_size >
    AL_SER_ACM_RAWDATA_ESC_SEQ_LENGTH */
  {
    /* Too many characters received, need to reset */
    rawdata_data_ptr->should_wait_for_clk = TRUE;
    rawdata_data_ptr->num_of_esc_seq_characters_seen = 0;
  }

  /* rearm clock */
  al_ser_acm_arm_esc_seq_clk(rawdata_data_ptr);
}

/*===========================================================================

FUNCTION al_ser_acm_init_rawdata_mode_data

DESCRIPTION
Initializes the data structs used for raw data mode.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_acm_init_rawdata_mode_data
(
  rawdata_mode_data_struct* rawdata_data_ptr
)
{
  HSU_ASSERT(rawdata_data_ptr != NULL);

  rawdata_data_ptr->num_of_esc_seq_characters_seen = 0;
  rawdata_data_ptr->should_wait_for_clk = TRUE;
  rawdata_data_ptr->rawdata_mode_esc_sequence_cb_func = NULL;

  rawdata_data_ptr->rawdata_mode_esc_seq_timer_cb_item = hsu_timer_init(
    HSU_TIMER_CID__RAWDATA_MODE_ESC_SEQ,
    HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
    al_ser_acm_rawdata_esc_seq_isr_cb,
    (timer_cb_data_type)rawdata_data_ptr);
}

/*==============================================================================

FUNCTION hsu_chg_wallchg_detect_cb

DESCRIPTION
The function queues al_ser_acm_rawdata_esc_seq_isr to AL task. 
Triggered by rawdata_data.rawdata_mode_esc_seq_timer_cb_item timer.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
==============================================================================*/
static void al_ser_acm_rawdata_esc_seq_isr_cb
(
  unsigned long rawdata_data_ptr_as_timer_data
)
{
  hsu_al_task_enqueue_cmd(al_ser_acm_rawdata_esc_seq_isr, (void *)rawdata_data_ptr_as_timer_data);
}

/*===========================================================================

FUNCTION al_ser_acm_rawdata_esc_seq_isr

DESCRIPTION
ISR invoked when rex timer used for esc. sequence detection has expired.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_acm_rawdata_esc_seq_isr
(
  void *rawdata_data_ptr_as_timer_data
)
{
  rawdata_mode_data_struct* rawdata_data_ptr = NULL;

  HSU_ASSERT((void*)(rawdata_data_ptr_as_timer_data) != NULL);

  rawdata_data_ptr = 
    (rawdata_mode_data_struct*)rawdata_data_ptr_as_timer_data;

  if (rawdata_data_ptr->should_wait_for_clk == TRUE)
  {
    /* The period of inactivity prior to esc seq has passed, and no data has been 
    received (otherwise rx callback would reset clock) so it is okay to proceed */
    rawdata_data_ptr->should_wait_for_clk = FALSE;

    return;
  }
  /* else: rawdata_data_ptr->should_wait_for_clk == FALSE
  This means whatever has accumulated so far has to be a full esc. seq.,
  anything else resets back to inactivity period. */

  /* Check if a full and exact esc seq. exists */
  if (rawdata_data_ptr->num_of_esc_seq_characters_seen == 
    AL_SER_ACM_RAWDATA_ESC_SEQ_LENGTH)
  {
    /* Esc. Seq. detected, invoke callback */
    if (rawdata_data_ptr->rawdata_mode_esc_sequence_cb_func != NULL)
    {
      rawdata_data_ptr->rawdata_mode_esc_sequence_cb_func();
      /* TODO: Re-arm the clk? This is done also in other SIO modules. */
    }
  }

  /* Restore state (now waiting for inactivity timeout) */
  rawdata_data_ptr->should_wait_for_clk = TRUE;
  rawdata_data_ptr->num_of_esc_seq_characters_seen = 0;

  /* Rearm the clk. If mode has changed from raw_data, then it already has cancelled
  this clk. */
  al_ser_acm_arm_esc_seq_clk(rawdata_data_ptr);
}

/*===========================================================================

FUNCTION al_ser_acm_unarm_esc_seq_clk

DESCRIPTION
Cancels the timer used for esc. seq. detection in rawdata mode.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
Cancels the rex timer

===========================================================================*/
static void al_ser_acm_unarm_esc_seq_clk
(
  rawdata_mode_data_struct* rawdata_data_ptr
)
{
  HSU_ASSERT(rawdata_data_ptr != NULL);

  hsu_timer_cancel(&(rawdata_data_ptr->rawdata_mode_esc_seq_timer_cb_item));
    /* This will cause the registered timer to seize its operation*/
}

/*===========================================================================

FUNCTION al_ser_acm_arm_esc_seq_clk

DESCRIPTION
Arms a timer to expire when AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS
msecs have passed. This is used in the detecting a legal esc. sequence when
in rawdata mode.

A legal esc. sequence should be:

At least AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence,
Escape Character, e.g. '+',
No more than AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence,
Escape Character,
No more than AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence,
Escape Character,
At least AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
Arms a rex timer for AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs.

===========================================================================*/
static void al_ser_acm_arm_esc_seq_clk
(
  rawdata_mode_data_struct* rawdata_data_ptr
)
{
  HSU_ASSERT(rawdata_data_ptr != NULL);

  hsu_timer_set(
    &(rawdata_data_ptr->rawdata_mode_esc_seq_timer_cb_item), 
    AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS,
    T_MSEC);
}


/*==============================================================================

FUNCTION al_ser_acm_rx_handle_rawdata_mode

DESCRIPTION   
Invoked when the USB bus mode changes (suspend/resume/disconnect, etc).

PARAMETERS
ser_if_data_ptr         : Pointer to serial I/F data
cur_dsm_item            : Pointer to current DSM item
buffer_ptr              : Pointer to buffer
bytes_transferred       : Number of bytes transferred in the current DSM item

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
static void al_ser_acm_rx_handle_rawdata_mode
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr,   
  dsm_item_type        *cur_dsm_item,
  char                 *buffer_ptr, 
  juint32_t             bytes_transferred
)
{   
  hsu_al_ser_acm_data *ser_acm_data_ptr = 
    (hsu_al_ser_acm_data *)ser_if_data_ptr;

  al_ser_acm_detect_esc_seq(
    ser_acm_data_ptr->modem_if_data.stream_mode_specific_data_ptr,
    buffer_ptr,
    bytes_transferred);
  
  cur_dsm_item->kind = SIO_PAYLOAD;
}


/*===========================================================================
FUNCTION hsu_al_ser_acm_transmit

DESCRIPTION
This function transmit a DSM chain over CDC/ACM FD.

DEPENDENCIES
Should be called with the Giant mutex locked

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_ser_acm_transmit
( 
  hsu_al_ser_acm_data* acm_data_ptr,
  dsm_item_type *tx_ptr
)
{
  hsu_platform_info_type const *platform_info;

#ifdef FEATURE_HS_USB_ACM_BAM
#ifndef FEATURE_HS_USB_BAM_A2_STUB
  modem_specific_data_struct* modem_data_ptr = &acm_data_ptr->modem_if_data;

  if (modem_data_ptr->sps_info.is_sps_mode)
  {
    HSU_MSG_ERROR("hsu_al_ser_acm_transmit(): called in SPS mode");
    dsm_free_packet(&tx_ptr);
    return;
  }
#else /* FEATURE_HS_USB_BAM_A2_STUB */
  /* Pass TX data through SPS A2 stub. */
  if (!hsu_sps_a2_mem_stub_tx(tx_ptr))
  {
    HSU_MSG_ERROR("hsu_al_ser_acm_transmit(): hsu_sps_a2_mem_stub_tx failed");
  }
  return;
#endif /* !FEATURE_HS_USB_BAM_A2_STUB */
#endif /* FEATURE_HS_USB_ACM_BAM */

  platform_info = hsu_platform_get_info();
  HSU_ASSERT(platform_info != NULL)
  
  switch(platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_STANDALONE:
	case HSU_PLATFORM_LA_FUSION:
      if((acm_data_ptr->modem_if_data.ctrl_line_state & AL_SER_ACM_DTR_ASSERTED_MASK) == 0)
      {
        dsm_free_packet(&tx_ptr);

        HSU_ULOG_ENUM_1(ERROR_MSG, DATA_LOG, "hsu_drop_dun_packet, stream_id %u",
          acm_data_ptr->base_data.iface_type, HSU_IFACE_OFFSET2, 
          (uint32)(acm_data_ptr->base_data.stream_id));
        
        //HSU_MSG_ERROR("hsu_al_ser_acm_transmit(): HSU_DROP_DUN_PACKET_LOG");
        return;
      }
      break;

    default:
      break;
  }

  hsu_al_ser_base_transmit(&(acm_data_ptr->base_data), tx_ptr);
  
  /* handle tx completion */
  if( acm_data_ptr->base_data.tx_info.pending_chain_ptr != NULL )
  {
     if( !( ( ( acm_data_ptr->base_data.tx_wmq_ptr == NULL ) || dsm_is_wm_empty( acm_data_ptr->base_data.tx_wmq_ptr ) ) &&
       ( acm_data_ptr->base_data.tx_info.remaining_chain_ptr == NULL ) ) )
     {
        cdc_handle_tx_completion( acm_data_ptr->base_data.instance_of_fd_used );
     }
  }

}

/*===========================================================================

FUNCTION al_ser_acm_tx_done

DESCRIPTION
Tx Callback whenever the ACM FD wishes to notify some buffers has been sent
over the data class I/F.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_acm_tx_done
(
  cdc_iostatus_t *status,
  void **buffers, 
  juint32_t *buf_sizes,
  juint32_t req_num,
  cdc_appctx_t ctx
)
{
#ifdef FEATURE_HS_USB_ACM_BAM
  hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)ctx;
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = &acm_data_ptr->base_data;

  modem_specific_data_struct* modem_data_ptr = &acm_data_ptr->modem_if_data;

  if (modem_data_ptr->sps_info.is_sps_mode)
  {
    /* tx_done is expected in SPS mode (with status CDC_TRANSFER_CANCELLED) upon 
    cable disconnect and USB suspend */
     HSU_MSG_HIGH("al_ser_acm_tx_done: called in SPS mode");
    ser_if_data_ptr->tx_info.is_write_in_progress = FALSE;
    return;
  }
#endif /* FEATURE_HS_USB_ACM_BAM */

  hsu_al_ser_base_i_tx_done(status, buffers, buf_sizes, req_num, ctx);
}


/*==============================================================================

FUNCTION      HSU_AL_ACM_SUSPEND_RESUME_MODE_CHANGED_CB

DESCRIPTION   
Invoked when the USB bus mode changes (suspend/resume/disconnect, etc).

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_acm_suspend_resume_mode_changed_cb
(
  hsu_bus_mode_enum new_mode,
  void* app_data
)
{
  hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)app_data;

  HSU_MSG_HIGH_1("*_suspend_resume_mode_changed_cb: new_mode=%d", new_mode);

  if ((acm_data_ptr->modem_if_data.hsu_al_acm_suspend_resume_mode) == new_mode)
  {
    HSU_MSG_ERROR(
      "*_suspend_resume_mode_changed_cb: mode not changed");
    return;
  }
  
  acm_data_ptr->modem_if_data.hsu_al_acm_suspend_resume_mode = new_mode;

  switch (new_mode)
  {
  case HSU_BUS_MODE_RESUMED:
    /* if we are resumed we need to issue cdc_read for the buffers that were
    canceled upon suspend */
    //Resume is handled by CDC callback.
    //al_ser_acm_resumed(app_data);
    //HSU_ULOG(NO_MSG, BUS_LOG, "resume_skip, using_default_cdc_cb");
    break;

#ifdef FEATURE_HS_USB_ACM_BAM
  case HSU_BUS_MODE_SUSPENDED:
    if (acm_data_ptr->modem_if_data.sps_info.sps_enabled)
    {
      hsu_al_acm_disable_sps(acm_data_ptr);

      if(hsu_al_sps_utils_get_pipes_status())
      {
        /* register SPS pipe wakeup event on IN pipe */
        if (hsu_al_sps_register_wakeup_event(&acm_data_ptr->modem_if_data.sps_info.sps_connection))
        {
          HSU_ERR_FATAL("*_acm_suspend_resume_mode_changed_cb: "
            "register_wakeup_event failed", 0, 0, 0);
        }
      }
    }
    break;
#endif /* FEATURE_HS_USB_ACM_BAM */
  }
} /* hsu_al_acm_suspend_resume_mode_changed_cb */

/*==============================================================================

FUNCTION      HSU_AL_ACM_REGISTER_BUS_MODE_NOTIFICATIONS

DESCRIPTION   
Called by the acm_init upon stack initialization. 

DEPENDENCIES  
None.

RETURN VALUE  
HSU_JSUCCESS for success.

SIDE EFFECTS  
None.

==============================================================================*/
static jresult_t hsu_al_acm_register_bus_mode_notifications
(
  hsu_al_ser_acm_data* acm_data_ptr,
  cdc_handle_t handle
)
{
  hsu_bus_mode_multi_core_cb_info_struct suspend_resume_callback_info;
  cdc_ctx_t *ctx = (cdc_ctx_t*)(handle);
  core_t *core = (core_t *)(ctx->core_ctx);

  suspend_resume_callback_info.core_index = (uint32)(core->core_number); /* HAL core index */
  HSU_USE_PARAM(handle);

  suspend_resume_callback_info.bus_mode_cb = hsu_al_acm_suspend_resume_mode_changed_cb;
  suspend_resume_callback_info.app_data = acm_data_ptr;
  if(hsu_register_bus_mode_multi_core_notif_cb(suspend_resume_callback_info) == FALSE)
  {
    HSU_ERR_FATAL("hsu_al_acm_register_bus_mode_notifications: "
      "hsu_register_bus_mode_multi_core_notif_cb failed", 0, 0, 0);
    return EUNKNOWN;
  }

  return HSU_JSUCCESS;
}
