/*==============================================================================

USB Serial I/F OBEX (DIAG/NMEA) Adaptation Layer for High Speed USB

GENERAL DESCRIPTION
Contains the implementation of the API for the serial functions' over OBEX 
adapt. layer

EXTERNALIZED FUNCTIONS
hsu_al_ser_obex_populate_obex_specific_data

INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_al_ser_base_init must be called for each function instance, 
prior to performing any other operations on the instance.

Copyright (c) 2006 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_ser_obex.c#2 $
$DateTime: 2021/02/08 00:10:03 $

when      who     what, where, why
--------  ---     ------------------------------------------------------
06/29/06  esh     First revision of file
==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h" /* For FEATURE_HSU_TEST to work */
#include "hsu_common_int.h"
#include "hsu_al_ser_obex.h"
#include "hsu_al_ser.h"
#include "hsu_al_ser_base_i.h"
#include "hsu_al_ser_i.h"

#include "hsu_bus_mode_notifier.h"

#include "jusb_common_int.h"
#include "jusb_core_fd_int.h"
#include "jos_int.h"
#include "jslave_init.h"
#include "jusb_core.h"
#include "cdc_fd.h"

#if defined (T_UNDP) && defined (FEATURE_HS_USB_NMEA) && defined (FEATURE_CGPS_LBS_TASK) 
#include "hsu_core_config_int.h"
#endif /* T_UNDP && FEATURE_HS_USB_NMEA && FEATURE_CGPS_LBS_TASK */

#ifdef T_UNDP
#include "hsu_conf_sel_stack_utils_int.h"
#endif /* T_UNDP */

#include "hsu_os_abstract_bus.h"
#include "hsu_al_task_int.h"
#include "hsu_al_dsm_utils.h"
#include "hsu_conf_sel_i.h"
#include "hsu_core_config_int.h"
#include "hsu_bam_sm.h"
#include "hsu_log.h"
#include "hsu_cdc_fd_api.h"

/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
#define HSU_AL_SER_OBEX_SUBCLASS         0xFF
#define HSU_AL_SER_OBEX_PROTOCOL         0xFF

#if defined (T_UNDP) 
#if defined (FEATURE_HS_USB_NMEA) && defined (FEATURE_CGPS_LBS_TASK) 
/* Assuming NMEA is always the 2nd OBEX instance in UNDP */
#define HSU_AL_SER_OBEX_NMEA_INSTANCE_NO 2
#endif /* FEATURE_HS_USB_NMEA && FEATURE_CGPS_LBS_TASK */

/* In UNDP Ser3 is used in China Telecom connection manager.
*  
*  - w/  NMEA, the instance number of serial3 obex is 3 
*  - w/o NMEA, the instance number of serial3 obex is 2
*/
#define HSU_AL_SER_OBEX_SER3_INSTANCE_NO_W_NMEA  3
#define HSU_AL_SER_OBEX_SER3_INSTANCE_NO_WO_NMEA 2
#endif /* T_UNDP */

// MAX_NUM_OF_OBEX_USERS value is determined by the combination of feature 
// flags.  So, new array size that is bigger than the max possible OBEX_USERS 
// is required
#define HSU_OBEX_STRING_ARRAY_SIZE 8

#if (HSU_OBEX_STRING_ARRAY_SIZE < MAX_NUM_OF_OBEX_USERS)
  #error "insufficient HSU_OBEX_STRING_ARRAY_SIZE < MAX_NUM_OF_OBEX_USERS"
#endif

#ifndef HSU_OBEX_CONTROL_STRING_0
#define HSU_OBEX_CONTROL_STRING_0 NULL
#endif /* HSU_OBEX_CONTROL_STRING_0 */
#ifndef HSU_OBEX_DATA_STRING_0
#define HSU_OBEX_DATA_STRING_0 NULL
#endif /* HSU_OBEX_DATA_STRING_0 */

#ifndef HSU_OBEX_CONTROL_STRING_1
#define HSU_OBEX_CONTROL_STRING_1 NULL
#endif /* HSU_OBEX_CONTROL_STRING_1 */
#ifndef HSU_OBEX_DATA_STRING_1
#define HSU_OBEX_DATA_STRING_1 NULL
#endif /* HSU_OBEX_DATA_STRING_1 */

#ifndef HSU_OBEX_CONTROL_STRING_2
#define HSU_OBEX_CONTROL_STRING_2 NULL
#endif /* HSU_OBEX_CONTROL_STRING_2 */
#ifndef HSU_OBEX_DATA_STRING_2
#define HSU_OBEX_DATA_STRING_2 NULL
#endif /* HSU_OBEX_DATA_STRING_2 */

#ifndef HSU_OBEX_CONTROL_STRING_3
#define HSU_OBEX_CONTROL_STRING_3 NULL
#endif /* HSU_OBEX_CONTROL_STRING_3 */
#ifndef HSU_OBEX_DATA_STRING_3
#define HSU_OBEX_DATA_STRING_3 NULL
#endif /* HSU_OBEX_DATA_STRING_3 */

#ifndef HSU_OBEX_CONTROL_STRING_4
#define HSU_OBEX_CONTROL_STRING_4 NULL
#endif /* HSU_OBEX_CONTROL_STRING_4 */
#ifndef HSU_OBEX_DATA_STRING_4
#define HSU_OBEX_DATA_STRING_4 NULL
#endif /* HSU_OBEX_DATA_STRING_4 */

#ifndef HSU_OBEX_CONTROL_STRING_5
#define HSU_OBEX_CONTROL_STRING_5 NULL
#endif /* HSU_OBEX_CONTROL_STRING_5 */
#ifndef HSU_OBEX_DATA_STRING_5
#define HSU_OBEX_DATA_STRING_5 NULL
#endif /* HSU_OBEX_DATA_STRING_5 */

#ifndef HSU_OBEX_CONTROL_STRING_6
#define HSU_OBEX_CONTROL_STRING_6 NULL
#endif /* HSU_OBEX_CONTROL_STRING_6 */
#ifndef HSU_OBEX_DATA_STRING_6
#define HSU_OBEX_DATA_STRING_6 NULL
#endif /* HSU_OBEX_DATA_STRING_6 */

#ifndef HSU_OBEX_CONTROL_STRING_7
#define HSU_OBEX_CONTROL_STRING_7 NULL
#endif /* HSU_OBEX_CONTROL_STRING_7 */
#ifndef HSU_OBEX_DATA_STRING_7
#define HSU_OBEX_DATA_STRING_7 NULL
#endif /* HSU_OBEX_DATA_STRING_7 */

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
Variables
==============================================================================*/
/*---------------------------------------------------------------------------
FORWARD DECLARATIONS
---------------------------------------------------------------------------*/
static boolean al_ser_obex_smd_ioctl
(
  hsu_al_ser_obex_data* obex_data_ptr, 
  sio_ioctl_cmd_type ioctl_cmd, 
  sio_ioctl_param_type* ioctl_param_ptr
);

static jresult_t hsu_al_ser_obex_request_handler
(
  cdc_appctx_t ctx, 
  juint8_t bRequest,
  juint16_t wValue, 
  juint16_t wIndex, 
  juint16_t wLength
);

static jresult_t hsu_al_ser_obex_resumed
(
  cdc_appctx_t ctx
);

static void hsu_al_ser_obex_suspend_resume_mode_changed_cb
(
 hsu_bus_mode_enum new_mode,
 void* app_data
);

/* String descriptors for control and data interfaces */
static struct 
{
  char *control, *data;
} hsu_al_obex_strings[HSU_OBEX_STRING_ARRAY_SIZE] = 
{
  {HSU_OBEX_CONTROL_STRING_0 , HSU_OBEX_DATA_STRING_0},
  {HSU_OBEX_CONTROL_STRING_1 , HSU_OBEX_DATA_STRING_1},
  {HSU_OBEX_CONTROL_STRING_2 , HSU_OBEX_DATA_STRING_2},
  {HSU_OBEX_CONTROL_STRING_3 , HSU_OBEX_DATA_STRING_3},
  {HSU_OBEX_CONTROL_STRING_4 , HSU_OBEX_DATA_STRING_4},
  {HSU_OBEX_CONTROL_STRING_5 , HSU_OBEX_DATA_STRING_5},
  {HSU_OBEX_CONTROL_STRING_6 , HSU_OBEX_DATA_STRING_6},
  {HSU_OBEX_CONTROL_STRING_7 , HSU_OBEX_DATA_STRING_7},
};

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/
#ifdef FEATURE_HS_USB_OBEX_BAM
/*===========================================================================

FUNCTION hsu_al_obex_wakeup_host

DESCRIPTION
This function will trigger remote wakeup

DEPENDENCIES
Called in HSU_AL_TASK context upon pipe wakeup event from SPS

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
static void hsu_al_obex_wakeup_host(void* cmd_param)
{
  hsu_al_ser_obex_data* obex_data_ptr = (hsu_al_ser_obex_data*)cmd_param;

  if (obex_data_ptr->base_data.instance_enabled_by_cdc_fd == FALSE)
  {
    return;
  }

  hsu_al_task_signal_remote_wakeup();
} /* hsu_al_obex_wakeup_host */ 
/*===========================================================================

FUNCTION hsu_al_obex_pipe_wakeup_cb

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
static void hsu_al_obex_pipe_wakeup_cb
(
bam_result_type bam_result
)
{
   HSU_MSG_HIGH("hsu_al_obex_pipe_wakeup_cb");
   
   if (bam_result.event != BAM_EVENT_WAKEUP)
   {
     HSU_ERR_FATAL("hsu_al_obex_pipe_wakeup_cb: unexpected event %d", bam_result.event, 0, 0);
     return;
   }
 
   hsu_al_obex_wakeup_host(&bam_result.cb_data);
   return;
} /* hsu_al_obex_pipe_wakeup_cb */
/*===========================================================================

FUNCTION hsu_al_obex_enable_bam

DESCRIPTION
Enable BAM mode for this OBEX instance. Sets up BAM read and write operations

DEPENDENCIES
This function is assumed to be called with giant mutex already locked

RETURN VALUE
None.

SIDE EFFECTS
Allocate BAM pipes

===========================================================================*/
boolean hsu_al_obex_enable_bam
(
  hsu_al_ser_obex_data* obex_data_ptr
)
{
  jresult_t cdc_res = HSU_JSUCCESS;
  hsu_al_obex_bam_info_type* bam_info_ptr = &obex_data_ptr->obex_if_data.bam_info;

  cdc_ctx_t *  cdc_ctx = (cdc_ctx_t *) obex_data_ptr->base_data.instance_of_fd_used;
  bam_info_ptr->bam_connection.cdc_ctx_ptr = cdc_ctx;

  HSU_MSG_HIGH("hsu_al_obex_enable_bam: Enter");

  if(bam_info_ptr->bam_flow_enabled == TRUE)
  {
    HSU_MSG_HIGH("hsu_al_obex_enable_bam: BAM mode already enabled");
    return TRUE;
  }

  // Allocate BAM pipes and enable BAM flow only for the SIO open case for QDSS.
  // Subsequent BAM flow is controlled from the BAM state machine.
  if (!bam_info_ptr->bam_pipes_allocated)
  {
    /* until further notice, BAM pipes are allocated at startup and never deallocated */
    bam_info_ptr->bam_connection.consumer_pipe_wakeup_event_cb.func = hsu_al_obex_pipe_wakeup_cb;
    bam_info_ptr->bam_connection.consumer_pipe_wakeup_event_cb.data = obex_data_ptr;
    if (!hsu_al_bam_allocate_pipes(&(bam_info_ptr->bam_connection)))
    {
      HSU_ERR_FATAL("Allocating BAM pipe failed", 0, 0, 0);
      return FALSE;
    }

    if (HSU_CORE_CONFIG_USB3_IDX == HSU_CORE_IDX_PRIMARY)
    {
        cdc_set_bam_pipe_params_for_dbm( cdc_ctx,
                                         (void*)&(bam_info_ptr->bam_connection),
                                         cdc_ctx->out_pipe.desc, bam_info_ptr->bam_connection.producer_pipe_index,
                                         0, 0, 0, 0,
                                         cdc_ctx->in_pipe.desc, bam_info_ptr->bam_connection.consumer_pipe_index,
                                         bam_info_ptr->bam_connection.consumer_pipe_config->data_base_pa, 
                                         bam_info_ptr->bam_connection.consumer_pipe_config->data_size,
                                         bam_info_ptr->bam_connection.consumer_pipe_config->desc_base_pa, 
                                         bam_info_ptr->bam_connection.consumer_pipe_config->desc_size);
    }

    // Use the NO ECM version of the BAM configuration for compositions that doesn't have ECM data channels over BAM
    //if (hsu_get_num_of_net_instances() == 0)
    {
      hsu_no_ecm_bam_sm_hw_initial_configuration();
    }

    // BAM Reset must be done before configuration.
    // However, in compositions with ECM, BAM reset is already done by hsu_bam_sm_hw_initial_configuration()
    //if (hsu_get_num_of_net_instances() == 0)
    {
      hsu_al_bam_reset();
    }

    hsu_al_bam_init_pipes_for_ipa_and_bam(&(bam_info_ptr->bam_connection));

    if (HSU_CORE_CONFIG_USB3_IDX == HSU_CORE_IDX_PRIMARY)
    {
        cdc_dbm_init_qdss(cdc_ctx,
                          bam_info_ptr->bam_connection.consumer_pipe_index,
                          bam_info_ptr->bam_connection.consumer_pipe_config->data_base_pa, 
                          bam_info_ptr->bam_connection.consumer_pipe_config->data_size);

        cdc_dbm_ep_init(cdc_ctx, bam_info_ptr->bam_connection.consumer_pipe_index);
    }


    cdc_res = cdc_sps_read_write_infinite(obex_data_ptr->base_data.instance_of_fd_used, 
      (juint8_t)bam_info_ptr->bam_connection.producer_pipe_index, HSU_AL_BAM_UL, /* read == DIRECTION OUT == producer - For QDSS it is set to all FFs */ 
      (juint8_t)bam_info_ptr->bam_connection.consumer_pipe_index, HSU_AL_BAM_DL); /* write == DIRECTION IN == consumer */

    switch(cdc_res)
    {
      case HSU_JSUCCESS:
        bam_info_ptr->bam_flow_enabled = TRUE;
        HSU_ULOG(NO_MSG, BAM_LOG, "cdc_sps_read_write_infinite_qdss");
        break;

      default:
        HSU_ERR_FATAL("hsu_al_obex_enable_bam: cdc_sps_read_write_infinite failed %d", cdc_res, 0, 0);
    }

    bam_info_ptr->bam_pipes_allocated = TRUE;
    HSU_MSG_HIGH("hsu_al_obex_enable_bam: Exit");
  }

  // No errors so return TRUE; 
  return TRUE;
}
/*===========================================================================

FUNCTION hsu_al_obex_disable_bam

DESCRIPTION
Disable bam mode for this OBEX instance.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
static void hsu_al_obex_disable_bam
(
  hsu_al_ser_obex_data* obex_data_ptr
)
{
  HSU_MSG_HIGH("hsu_al_obex_disable_bam: Enter");

  obex_data_ptr->obex_if_data.bam_info.bam_flow_enabled = FALSE;

  HSU_MSG_HIGH("hsu_al_obex_disable_bam: Exit");
}

#endif /* FEATURE_HS_USB_OBEX_BAM */

/*===========================================================================
FUNCTION hsu_al_ser_obex_open()
Should be called with Giant mutex locked! 
===========================================================================*/
boolean hsu_al_ser_obex_open
(
  hsu_al_ser_obex_data  *obex_data_ptr,
  const sio_open_type   *open_ptr,
  dsm_mempool_id_enum_type pool_id,
  juint8_t              rx_buff_cnt,
  uint32                rx_timeout,
  uint32                tx_max_pending_chain_cnt,
  uint32                tx_timeout,
  uint32                tx_complete_timer_val
)
{
  cdc_ctx_t *  cdc_ctx = NULL; 

  HSU_ASSERT(obex_data_ptr != NULL);

  HSU_MSG_HIGH("*_obex_open: entered");

  if (!hsu_os_is_giant_locked_by_curr_task())
  {
    HSU_ERR_FATAL("hsu_al_ser_obex_open: Should be called "
      " with Giant mutex locked!", 0, 0, 0);
    return NULL;    
  }

#ifdef FEATURE_HS_USB_OBEX_BAM
  if (open_ptr->open_param == NULL)
  {
    HSU_MSG_LOW("hsu_al_ser_obex_open: opening in SIO_USB_MODE_LEGACY mode"
      " due to NULL open param.");
    obex_data_ptr->obex_if_data.bam_info.is_bam_mode = FALSE;
  }
  else
  {

    switch (open_ptr->open_param->dsm_pool_param.usb_mode)
    {
      case SIO_USB_MODE_LEGACY:
        HSU_MSG_LOW("hsu_al_ser_obex_open: opening in SIO_USB_MODE_LEGACY mode");
        obex_data_ptr->obex_if_data.bam_info.is_bam_mode = FALSE;
      break;
      case SIO_USB_MODE_SPS_USB_BAM:
        HSU_MSG_LOW("hsu_al_ser_obex_open: opening in SIO_USB_MODE_SPS_USB_BAM mode");
        obex_data_ptr->obex_if_data.bam_info.is_bam_mode = TRUE;
        obex_data_ptr->obex_if_data.bam_info.bam_connection.sio_port_id = SIO_PORT_USB_QDSS; /* by design go on 3rd interface in hsu_al_bam_utils */
        break;
      case SIO_USB_MODE_SPS_USB_SYSTEM:
        HSU_MSG_ERROR("hsu_al_ser_obex_open: SIO_USB_MODE_SPS_USB_SYSTEM "
          " is not supported.");
        obex_data_ptr->obex_if_data.bam_info.is_bam_mode = FALSE;
        break;
      default:
        HSU_MSG_ERROR_1("hsu_al_ser_obex_open: unknown BAM mode %d",
          open_ptr->open_param->dsm_pool_param.usb_mode);
        obex_data_ptr->obex_if_data.bam_info.is_bam_mode = FALSE;
    }
  }
#endif /* FEATURE_HS_USB_OBEX_BAM */

  /* When dynamic configurations are supported, it is possible that an app 
  requested to open a HSU port, but without the HSU stack initializing it.
  To verify this, instance_of_fd_used must be checked to be non NULL.
  It is assumed that hsu_al_ser_base_init was called previously for this instance. */
  if (obex_data_ptr->base_data.instance_of_fd_used == NULL)
  {
    HSU_MSG_ERROR_1("*_obex_open: instance not initialized by stack. port_id=%u",
      open_ptr->port_id);
    return FALSE;
  }

   cdc_ctx = (cdc_ctx_t *) obex_data_ptr->base_data.instance_of_fd_used;
  /* The following scenario is possible: 
    0. enable callback not yet invoked
    1. Inside hsu_al_ser_base_i_set_params_in_open_close, memory_pool_for_rx was set
    2. CDC FD invokes enable -> causes cdc_read operation
    3. (context returned here): because enable was invoked
    ser_if_data_ptr->instance_enabled_by_cdc_fd is TRUE , so yet another read is 
    set...
    This whole function is protected by Giant mutex
  */

#ifdef FEATURE_HS_USB_OBEX_BAM
  if (obex_data_ptr->obex_if_data.bam_info.bam_flow_enabled)
  {
  hsu_al_ser_base_i_open_set_data(
     &obex_data_ptr->base_data, 
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
      obex_data_ptr);    
  }
  else
#endif /* FEATURE_HS_USB_OBEX_BAM */
  {
    hsu_al_ser_base_i_open_set_data(
      &obex_data_ptr->base_data,
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
      obex_data_ptr);
  }

  /* Skip the use of DS DSM pool level notification for DIAG on modem port. */
  if (obex_data_ptr->base_data.stream_mode != SIO_DM_MODE)
  {
    hsu_al_dsm_utils_init(DSM_DS_LARGE_ITEM_POOL);
  }

  if (obex_data_ptr->base_data.instance_enabled_by_cdc_fd == TRUE)
  {
    /* Reaching this point means enable callback was already called */
    
    /* Now, even if a disable cb is invoked, hsu_al_ser_obex_setup_read 
    checks instance_enabled_by_cdc_fd (without interference by FD) prior to calling 
    cdc_read */

    /* Assuming there can't be a call to disable cb and then enable cb before 
    the call to cdc_read inside hsu_al_ser_obex_setup_read */

#ifdef FEATURE_HS_USB_OBEX_BAM
    if ((obex_data_ptr->obex_if_data.bam_info.is_bam_mode))
    {
      if (hsu_al_obex_enable_bam(obex_data_ptr) == FALSE)
      {
        HSU_ERR_FATAL("_ser_obex_internal_open: enable_bam failed", 0, 0, 0);
      }
    }
    else if(FALSE == obex_data_ptr->obex_if_data.bam_info.is_bam_mode)
    {
      if((cdc_ctx->info.basic.flags & CDC_INIT_OBEX_BULK_IN_ONLY) != CDC_INIT_OBEX_BULK_IN_ONLY)
      {
        hsu_al_ser_base_i_rx_setup_read(&obex_data_ptr->base_data);
      }
    }
#else
    if((cdc_ctx->info.basic.flags & CDC_INIT_OBEX_BULK_IN_ONLY) != CDC_INIT_OBEX_BULK_IN_ONLY)
    {
      hsu_al_ser_base_i_rx_setup_read(&obex_data_ptr->base_data);
    }

#endif /* FEATURE_HS_USB_OBEX_BAM */
  }
  else
  {
    /* Reaching this point means enable callback was not yet called, 
    but there is no problem it would be called from now on */
  }

  return TRUE;
}

/*===========================================================================
FUNCTION hsu_al_ser_obex_populate_obex_specific_data()
===========================================================================*/
void hsu_al_ser_obex_populate_obex_specific_data
(
  hsu_al_ser_obex_data* obex_data_ptr,
  cdc_handle_t handle,
  obex_callbacks_t* cbs,
  obex_init_info_t* info,
  cdc_appctx_t* ctx,
  uint8 instance_number
)
{
  hsu_iface_index_enum_type iface_type;

  hsu_bus_mode_multi_core_cb_info_struct suspend_resume_callback_info;

  HSU_ASSERT(obex_data_ptr != NULL);
  HSU_ASSERT(handle != NULL);
  HSU_ASSERT(cbs != NULL);
  HSU_ASSERT(info != NULL);
  HSU_ASSERT(ctx != NULL);

  /* Save context for later callbacks */
  *ctx = (cdc_appctx_t)(obex_data_ptr);

  cbs->basic.read_complete_h = NULL;
  cbs->basic.write_complete_h = NULL;

  suspend_resume_callback_info.core_index = obex_data_ptr->obex_if_data.core_idx;
  suspend_resume_callback_info.bus_mode_cb = hsu_al_ser_obex_suspend_resume_mode_changed_cb;
  suspend_resume_callback_info.app_data = obex_data_ptr;
  if (hsu_register_bus_mode_multi_core_notif_cb(suspend_resume_callback_info)
    == FALSE)
  {
    HSU_ERR_FATAL("hsu_al_ser_obex_populate_obex_specific_data(): "
      "hsu_register_bus_mode_multi_core_notif_cb failed", 0, 0, 0);
  }

  /* The remaining callbacks are used */
  cbs->basic.read_multi_complete_h = hsu_al_ser_base_i_rx_done;
  cbs->basic.write_multi_complete_h = hsu_al_ser_base_i_tx_done;

  cbs->basic.uninit_h = hsu_al_ser_obex_uninit;

  /* TODO: Fill more callbacks, for now they are NULL */
  cbs->basic.enabled_h = hsu_al_ser_obex_enabled;
  cbs->basic.disabled_h = hsu_al_ser_obex_disabled;

  /* And these will never be used in OBEX: */
  cbs->basic.encap_command_h = NULL;
  cbs->basic.encap_response_complete_h = NULL;

  cbs->basic.resumed_h = hsu_al_ser_obex_resumed;

  cbs->request_handler_cb = hsu_al_ser_obex_request_handler;
  /* Init info struct */
  info->basic.flags = (juint32_t)(
                                  CDC_INIT_BUFFER_DMABLE | 
                                  CDC_INIT_SINGLE_INTERFACE |
                                  CDC_INIT_VENDOR_SPECIFIC_CLASS |
                                  CDC_INIT_NO_CLASS_DESC |
                                  CDC_INIT_NO_ALT_SETTINGS);

  iface_type = hsu_conf_sel_get_iface_type_from_instance_number(instance_number-1, OBEX_HSU_IFACE_START);  
#ifdef FEATURE_HS_USB_QDSS_PORT
  if(iface_type == QDSS_HSU_IFACE) 
  {
    info->basic.flags |= (juint32_t)(CDC_INIT_OBEX_BULK_IN_ONLY);
  }
#endif /* FEATURE_HS_USB_QDSS_PORT */

#if defined (T_UNDP) && defined (FEATURE_HS_USB_NMEA) && defined (FEATURE_CGPS_LBS_TASK) 
  /* Check whether this is the the NMEA instance of OBEX */
  if ((obex_data_ptr->obex_if_data.core_idx == HSU_CORE_IDX_PRIMARY) && 
    (instance_number == HSU_AL_SER_OBEX_NMEA_INSTANCE_NO) &&
    (hsu_is_undp_curr_comp_id_for_nmea () == TRUE) )
  {
    /* set the flag CDC_INIT_BI_DIR_NMEA so we could distinguish the NMEA instance of OBEX 
    from other instances of OBEX */
    info->basic.flags |= (juint32_t)CDC_INIT_BI_DIR_NMEA;
    /* clear CDC_INIT_NO_CLASS_DESC so MDLM descriptors could be used */
    info->basic.flags &= ~(juint32_t)CDC_INIT_NO_CLASS_DESC;
  }
#else
  HSU_USE_PARAM (instance_number);
#endif /* T_UNDP && FEATURE_HS_USB_NMEA && FEATURE_CGPS_LBS_TASK */
 
  /* Enable support for ZLP */
  info->basic.zlp_terminator = TRUE;

  info->basic.cci_subclass = HSU_AL_SER_OBEX_SUBCLASS;
  switch (iface_type)
  {
    case DIAG_HSU_IFACE:  info->basic.cci_protocol = CDC_FD_IF_PROTOCOL__DIAG_HDLC; break;
    case NMEA_HSU_IFACE:  info->basic.cci_protocol = CDC_FD_IF_PROTOCOL__NMEA;      break;
    case SER4_HSU_IFACE:  info->basic.cci_protocol = CDC_FD_IF_PROTOCOL__SAHARA;    break;
    case QDSS_HSU_IFACE:  info->basic.cci_protocol = CDC_FD_IF_PROTOCOL__QDSS;      break;
    default:              info->basic.cci_protocol = HSU_AL_SER_OBEX_PROTOCOL;      break;   
  }


#if defined (FEATURE_HS_USB_OS_DETECTION)

  /* In order for the MDLM descriptors to appear, 
  CDC_INIT_NO_CLASS_DESC must not be used. */
  info->basic.flags &= ~(juint32_t)CDC_INIT_NO_CLASS_DESC;

#endif /* FEATURE_HS_USB_OS_DETECTION */

  /* Communication interface string descriptor */
  /* instance-number 1 uses  hsu_al_obex_strings[0], etc. */

  // Check for hsu_al_obex_strings array out bound
  HSU_ASSERT((instance_number-1) < HSU_OBEX_STRING_ARRAY_SIZE);

  info->basic.comm_string = hsu_al_obex_strings[instance_number-1].control;
  /* data interface string descriptor */
  info->basic.data_string = hsu_al_obex_strings[instance_number-1].data;

#ifdef T_UNDP
  /* 
  ** In UNDP (Gobi 2 or later version), to match to PC driver interface 
  ** - the first obex interface (DIAG) will be set as 1 in static interfaces
  ** - the second obex interface (NMEA) should be set as 3 in static interfaces
  ** - the third obex instance (serial 3) should be set as 4 in static interface
  */
  if (hsu_is_undp_using_static_interface () == TRUE) 
  {
    if ( info->basic.flags & (juint32_t)CDC_INIT_BI_DIR_NMEA )
    {
      info->basic.interface_number = 3;
    }
    else 
    {
      /* SER3 instance */
      if ( (obex_data_ptr->obex_if_data.core_idx == HSU_CORE_IDX_PRIMARY) && 
        ( (instance_number == HSU_AL_SER_OBEX_SER3_INSTANCE_NO_W_NMEA) ||
        (instance_number == HSU_AL_SER_OBEX_SER3_INSTANCE_NO_WO_NMEA) ) &&
        (hsu_is_undp_ser3_port_enabled() == TRUE) )
      { 
        info->basic.interface_number = 4;
      }
      /* DIAG instance */
      else 
      { 
        info->basic.interface_number = 1;
      }
    }
  }
#endif /* T_UNDP */
}

/*===========================================================================
FUNCTION hsu_al_ser_obex_ioctl()
Should be called with the Giant mutex locked
===========================================================================*/
boolean hsu_al_ser_obex_ioctl
(
  hsu_al_ser_obex_data* obex_data_ptr,
  sio_ioctl_cmd_type ioctl_cmd,
  sio_ioctl_param_type* ioctl_param_ptr
)
{
  boolean data_ptr_is_valid = FALSE;

  HSU_ASSERT(ioctl_param_ptr != NULL);

  if (!hsu_os_is_giant_locked_by_curr_task())
  {
    HSU_ERR_FATAL("hsu_al_ser_obex_ioctl: Should be called "
      " with Giant mutex locked!", 0, 0, 0);
    return FALSE;
  }

  data_ptr_is_valid = hsu_al_ser_base_i_ctx_is_valid(&obex_data_ptr->base_data);
  HSU_ASSERT(data_ptr_is_valid);

  /* First try the "base class": */
  if (hsu_al_ser_base_ioctl(&obex_data_ptr->base_data, ioctl_cmd, ioctl_param_ptr) == TRUE)
  {
    /* Handled in "base" */
    return TRUE;
  }

  /* Handle DTR */
  switch(ioctl_cmd) 
  {
    /* Control Line State Commands */
  case SIO_IOCTL_ENABLE_DTR_EVENT:
    {
      HSU_MSG_LOW("*_obex_ioctl: handling I/O Ctl: SIO_IOCTL_ENABLE_DTR_EVENT");

      obex_data_ptr->obex_if_data.app_dtr_changed_cb_func = 
        (sio_vv_func_ptr_type)((*ioctl_param_ptr).enable_dte_ready_event);

      if (obex_data_ptr->obex_if_data.app_dtr_changed_cb_func != NULL)
      {
        obex_data_ptr->obex_if_data.app_dtr_changed_cb_func();
      }

      break;
    }
  case SIO_IOCTL_DISABLE_DTR_EVENT:
    {
      HSU_MSG_LOW("*_obex_ioctl: handling I/O Ctl: SIO_IOCTL_DISABLE_DTR_EVENT");

      obex_data_ptr->obex_if_data.app_dtr_changed_cb_func = NULL;
      break;
    }
  case SIO_IOCTL_DTE_READY_ASSERTED:
    {
      if (obex_data_ptr->base_data.tx_info.recover_dropped_packet)
      {
        *(ioctl_param_ptr->dte_ready_asserted) = TRUE;        
      }
      else
      {
        HSU_MSG_LOW_1("*_obex_ioctl: handling SIO_IOCTL_DTE_READY_ASSERTED, returning %d", 
        obex_data_ptr->obex_if_data.dte_ready);
        *(ioctl_param_ptr->dte_ready_asserted) = obex_data_ptr->obex_if_data.dte_ready;
      }
      break;
    }
    default:
    {
      /* If the IOCTL is not handled here - Handle the following set of SMD I/O Ctls*/
      return al_ser_obex_smd_ioctl(obex_data_ptr, ioctl_cmd, ioctl_param_ptr);
    }
  }
  return TRUE;
}


static jresult_t hsu_al_ser_obex_request_handler
(
  cdc_appctx_t ctx, 
  juint8_t bRequest,
  juint16_t wValue, 
  juint16_t wIndex, 
  juint16_t wLength
)
{
  HSU_USE_PARAM(ctx);
  HSU_USE_PARAM(bRequest);
  HSU_USE_PARAM(wValue);
  HSU_USE_PARAM(wIndex);
  HSU_USE_PARAM(wLength);
  /* The reason to change is as following:
     The USB CV MSC ClassRequestTest_DeviceConfigured 
     issues a Bulk-Only Mass Storage Reset request with 
     wValue != 0 and wLength != 0 errors (step 10)on interface 0 
     (wIndex = 0) instead of a real Mass Storage interface 2.
     As a result, this requests are routed to this function that 
     supports no requests but does not notify about that.
  */
  return JENOTSUP;
}

/*===========================================================================

FUNCTION al_ser_obex_smd_ioctl

DESCRIPTION
Handles I/O Ctl commands coming from SMD to the specified OBEX I/F.

DEPENDENCIES
Must be called with the Giant mutex locked

RETURN VALUE
TRUE - I/O Ctl command handled. Otherwise - Failure. 

SIDE EFFECTS
None

===========================================================================*/
static boolean al_ser_obex_smd_ioctl
(
  hsu_al_ser_obex_data* obex_data_ptr, 
  sio_ioctl_cmd_type ioctl_cmd, 
  sio_ioctl_param_type* ioctl_param_ptr
)
{
/* Handles the following I/O CTLs:

  SIO_IOCTL_INBOUND_FLOW_ENABLE
  SIO_IOCTL_INBOUND_FLOW_DISABLE


  SIO_IOCTL_DSR_ASSERT
  SIO_IOCTL_DSR_DEASSERT  

  SIO_IOCTL_CD_ASSERT
  SIO_IOCTL_CD_DEASSERT

  SIO_IOCTL_RI_ASSERT
  SIO_IOCTL_RI_DEASSERT

*/

  HSU_ASSERT(obex_data_ptr != NULL);
  HSU_ASSERT(ioctl_param_ptr != NULL);

  HSU_PARAM_COULD_BE_CONST(obex_data_ptr);
  HSU_PARAM_COULD_BE_CONST(ioctl_param_ptr);

  if (!hsu_os_is_giant_locked_by_curr_task())
  {
    HSU_ERR_FATAL("al_ser_obex_smd_ioctl: Should be called "
      " with Giant mutex locked!", 0, 0, 0);
    return FALSE;
  }

  switch (ioctl_cmd)
  {
    /* Do nothing for the following: */
    case SIO_IOCTL_INBOUND_FLOW_ENABLE:
    case SIO_IOCTL_INBOUND_FLOW_DISABLE:
    case SIO_IOCTL_DSR_ASSERT:
    case SIO_IOCTL_DSR_DEASSERT:
    case SIO_IOCTL_CD_ASSERT:
    case SIO_IOCTL_CD_DEASSERT:
    case SIO_IOCTL_RI_ASSERT:
    case SIO_IOCTL_RI_DEASSERT:
    {
      break;
    }

    default:
    {
      HSU_MSG_ERROR_1("al_ser_obex_smd_ioctl: cmd %u not supported", 
        ioctl_cmd);
      return FALSE;
    }
  }
  return TRUE;
}

/*==============================================================================

FUNCTION      HSU_AL_SER_OBEX_RESUME

DESCRIPTION   
Invoked when the USB stack gets resumed

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
If the application has registered a callback, it will be invoked.

==============================================================================*/
static jresult_t hsu_al_ser_obex_resumed
(
  cdc_appctx_t ctx
)
{
  jresult_t jresult = HSU_JSUCCESS;
#ifdef FEATURE_HS_USB_OBEX_BAM
  hsu_al_ser_obex_data* obex_data_ptr = (hsu_al_ser_obex_data*)ctx;
  
  /* Defer to the base class if BAM is not enabled
     For the case where BAM is enabled, the suspend/resume CB
     will handle enabling and disabling of the BAM.
  */
  if (!obex_data_ptr->obex_if_data.bam_info.is_bam_mode)
#endif // FEATURE_HS_USB_OBEX_BAM
  {
     jresult = hsu_al_ser_base_i_resumed(ctx); 
  }

  return jresult;
}
/*==============================================================================

FUNCTION      HSU_AL_SER_OBEX_SUSPEND_RESUME_MODE_CHANGED_CB

DESCRIPTION   
Invoked when the USB stack gets suspended/resumed.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
If the application has registered a callback, it will be invoked.

==============================================================================*/
static void hsu_al_ser_obex_suspend_resume_mode_changed_cb
(
 hsu_bus_mode_enum new_mode,
 void* app_data
)
{
  hsu_al_ser_obex_data* obex_data_ptr = NULL;

  HSU_MSG_HIGH_1("_suspend_resume_mode_changed_cb(): new mode == %d (0=S,1=R)",
    new_mode);

  obex_data_ptr = (hsu_al_ser_obex_data*)app_data;

  HSU_ASSERT(obex_data_ptr != NULL);

#ifdef FEATURE_HS_USB_OBEX_BAM
  switch (new_mode)
  {
  case HSU_BUS_MODE_RESUMED:
    if (obex_data_ptr->obex_if_data.bam_info.is_bam_mode)
    {
     /* Dont enable BAM path yet. We expect to do that on hsu_al_ser_obex_open() */
#ifdef FEATURE_HS_USB_EXCLUDE_FOR_PHASE_1_IPA_BU
        if (hsu_al_bam_unregister_wakeup_event(&obex_data_ptr->obex_if_data.bam_info.bam_connection) == FALSE)
        {
          HSU_MSG_ERROR(
            "*_obex_resumed_internal: unregister_wakeup_event failed");
        }
#endif /* FEATURE_HS_USB_EXCLUDE_FOR_PHASE_1_IPA_BU */
     }
    break;

  case HSU_BUS_MODE_SUSPENDED:
    if (obex_data_ptr->obex_if_data.bam_info.is_bam_mode)
    {
      hsu_al_obex_disable_bam(obex_data_ptr);

#ifdef FEATURE_HS_USB_EXCLUDE_FOR_PHASE_1_IPA_BU
      if(hsu_al_bam_utils_get_dl_pipes_status())
      {
        /* register BAM pipe wakeup event on IN pipe */
        if (hsu_al_bam_register_wakeup_event(&obex_data_ptr->obex_if_data.bam_info.bam_connection) == FALSE)
        {
          HSU_ERR_FATAL("*_obex_suspend_resume_mode_changed_cb: "
            "register_wakeup_event failed", 0, 0, 0);
        }
      }
#endif /* FEATURE_HS_USB_EXCLUDE_FOR_PHASE_1_IPA_BU */
    }
    else // non-BAM mode
    {
      // For non-bam mode, QDSS EP needs to be aborted
      if ( (QDSS_HSU_IFACE == obex_data_ptr->base_data.iface_type)
        && (obex_data_ptr->base_data.instance_enabled_by_cdc_fd) 
        && (obex_data_ptr->base_data.tx_info.is_write_in_progress) )
      {
        HSU_ULOG(NO_MSG, BUS_LOG, "qdss_nonbam_mode_abort_write");
        (void)hsu_cdc_fd_api_cdc_abort_write(obex_data_ptr->base_data.instance_of_fd_used);
      }
    }
    break;
  }
#endif /* FEATURE_HS_USB_OBEX_BAM */

// For USB SUSPEND, there is no need to invoke DTR drop to the client 
// since REMOTE WAKEUP is supported.  For cable disconnect, DTR drop will be 
// invoked.
#if 0 
  /* Check if the the dte ready state should be changed */
  if (((HSU_BUS_MODE_SUSPENDED == new_mode) && (TRUE == obex_data_ptr->obex_if_data.dte_ready)) ||
    ((HSU_BUS_MODE_RESUMED == new_mode) && (FALSE == obex_data_ptr->obex_if_data.dte_ready)))
  {
    HSU_MSG_LOW("*_obex_suspend_resume_mode_changed_cb: dtr changed callback should be called");

    /* Update the new state and call the application callback */
#ifdef FEATURE_HS_USB_SER4_PORT
    if(obex_data_ptr->base_data.instance_enabled_by_cdc_fd)
#endif /* FEATURE_HS_USB_SER4_PORT */
    {
      obex_data_ptr->obex_if_data.dte_ready = !obex_data_ptr->obex_if_data.dte_ready;
      if (obex_data_ptr->obex_if_data.app_dtr_changed_cb_func != NULL)
      {
        HSU_MSG_LOW("*_obex_suspend_resume_mode_changed_cb: calling the dtr changed callback");
        obex_data_ptr->obex_if_data.app_dtr_changed_cb_func();
      }
    }
  }
#endif

  if ((HSU_BUS_MODE_RESUMED == new_mode)
     && (obex_data_ptr->base_data.tx_info.recover_dropped_packet) 
#ifdef FEATURE_HS_USB_OBEX_BAM
      && !obex_data_ptr->obex_if_data.bam_info.bam_flow_enabled 
#endif // FEATURE_HS_USB_OBEX_BAM
      )
  {
    /* To resume TX (Downlink) */
    hsu_al_ser_i_base_tx_set_handle_complete_timer(&obex_data_ptr->base_data);
  }
}

/*===========================================================================

FUNCTION hsu_al_ser_obex_init

DESCRIPTION
Initializes OBEX AL specific fields.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_obex_init
(
 hsu_al_ser_obex_data* obex_data_ptr          /* Pointer to OBEX Data struct */
 )
{
  HSU_ASSERT(obex_data_ptr != NULL);

  /* Call base class' method: */
  hsu_al_ser_base_init(&(obex_data_ptr->base_data));

  /* Update obex specific fields: */
  obex_data_ptr->obex_if_data.app_dtr_changed_cb_func = NULL;
  obex_data_ptr->obex_if_data.dte_ready = FALSE;
}

/*===========================================================================

FUNCTION hsu_al_ser_obex_uninit

DESCRIPTION
Serves as a callback whenever the CDC FD wishes to notify it is being 
un-initialized, or as an initializer for the given context when the very first 
hsu_al_ser_base_init is called.

DEPENDENCIES
See cdc_uninit_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always.

SIDE EFFECTS
The data structure corresponding to the given context is invalidated.

===========================================================================*/
jresult_t hsu_al_ser_obex_uninit
(
 cdc_appctx_t ctx                           /* Context passed in callback */
 )
{
  hsu_bus_mode_multi_core_cb_info_struct suspend_resume_callback_info;

  hsu_al_ser_obex_data* obex_data_ptr = NULL;
  obex_data_ptr = (hsu_al_ser_obex_data*)ctx;

  hsu_al_ser_handle_obex_uninit(ctx);

  (void)hsu_al_ser_base_i_uninit(ctx);

  suspend_resume_callback_info.core_index = obex_data_ptr->obex_if_data.core_idx;
  suspend_resume_callback_info.bus_mode_cb = hsu_al_ser_obex_suspend_resume_mode_changed_cb;
  suspend_resume_callback_info.app_data = ctx;
  hsu_unregister_bus_mode_multi_core_notif_cb(suspend_resume_callback_info);

  return HSU_JSUCCESS;
}

/*===========================================================================

FUNCTION hsu_al_ser_obex_enabled

DESCRIPTION
Enabled callback whenever the serial CDC FD wishes to notify that the interface
is enabled.

DEPENDENCIES
See cdc_enabled_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
jresult_t hsu_al_ser_obex_enabled
(
 cdc_appctx_t ctx                           /* Context passed in callback */
 )
{
  hsu_al_ser_obex_data* obex_data_ptr = NULL;
  hsu_al_ser_base_data_for_ser_if* app_data_ptr = NULL;

  obex_data_ptr = (hsu_al_ser_obex_data*)ctx;
  app_data_ptr =  &obex_data_ptr->base_data;

  HSU_USE_PARAM(app_data_ptr);

  HSU_MSG_LOW("*_obex_enabled: obex enabled");

  /* Set the FD handle to the current configuration's FD */
  hsu_al_ser_handle_set_configuration(obex_data_ptr->obex_if_data.core_idx);

#ifdef FEATURE_HS_USB_OBEX_BAM
    if (obex_data_ptr->obex_if_data.bam_info.is_bam_mode)
    {
      app_data_ptr->instance_enabled_by_cdc_fd = TRUE;
      if (hsu_al_obex_enable_bam(obex_data_ptr) == FALSE)
      {
        HSU_ERR_FATAL("al_ser_obex_enabled : enable_bam failed", 0, 0, 0);
      }
    }
    else
#endif // FEATURE_HS_USB_OBEX_BAM
    {
      (void)hsu_al_ser_base_i_enabled(ctx);
    }


  if (obex_data_ptr->obex_if_data.dte_ready == TRUE)
  {
    HSU_MSG_LOW("*_obex_disabled: dte_ready is already TRUE");
    return HSU_JSUCCESS;
  }

  obex_data_ptr->obex_if_data.dte_ready = TRUE;

  /* Calling the dtr changed callback if it is set*/
  if (obex_data_ptr->obex_if_data.app_dtr_changed_cb_func != NULL)
  {
    HSU_MSG_LOW("*_obex_enabled: calling the dtr changed callback");
    obex_data_ptr->obex_if_data.app_dtr_changed_cb_func();
  }

  return HSU_JSUCCESS;
}

/*===========================================================================

FUNCTION hsu_al_ser_obex_disabled

DESCRIPTION
Disabled callback whenever the serial CDC FD wishes to notify that the interface
is disabled.

DEPENDENCIES
See cdc_disabled_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
jresult_t hsu_al_ser_obex_disabled
(
 cdc_appctx_t ctx                           /* Context passed in callback */
 )
{
  hsu_al_ser_obex_data* obex_data_ptr = (hsu_al_ser_obex_data*)ctx;
  hsu_al_ser_base_data_for_ser_if* app_data_ptr = (hsu_al_ser_base_data_for_ser_if*)ctx;

  dsm_watermark_type *tx_wmq_ptr;

  HSU_MSG_LOW("*_obex_disabled: obex disabled");

  HSU_USE_PARAM(obex_data_ptr);
  HSU_USE_PARAM(app_data_ptr);

#ifdef FEATURE_HS_USB_OBEX_BAM
    if (obex_data_ptr->obex_if_data.bam_info.is_bam_mode)
    {
        hsu_al_obex_disable_bam(obex_data_ptr);
        app_data_ptr->instance_enabled_by_cdc_fd = FALSE;

        if (obex_data_ptr->obex_if_data.dte_ready == FALSE)
        {
          HSU_MSG_LOW("*_obex_disabled: dte_ready is already FALSE");
          return HSU_JSUCCESS;
        }

        obex_data_ptr->obex_if_data.dte_ready = FALSE;

        /* Calling the dtr changed callback if it is set*/
        if (obex_data_ptr->obex_if_data.app_dtr_changed_cb_func != NULL)
        {
          HSU_MSG_LOW("*_obex_disabled: calling the dtr changed callback");
          obex_data_ptr->obex_if_data.app_dtr_changed_cb_func();
        }
        return HSU_JSUCCESS;
    }
#endif

  (void)hsu_al_ser_base_i_disabled(ctx);

  if (obex_data_ptr->base_data.tx_info.recover_dropped_packet == FALSE)
  {
     tx_wmq_ptr = obex_data_ptr->base_data.tx_wmq_ptr;

    /* free the dsm in tx wm */
    if(tx_wmq_ptr && tx_wmq_ptr->q_ptr)
    {
      dsm_empty_queue(tx_wmq_ptr);
    }

    /* free the dsm in remaining chain */
    if (obex_data_ptr->base_data.tx_info.remaining_chain_ptr != NULL)
    {
      dsm_free_packet(&obex_data_ptr->base_data.tx_info.remaining_chain_ptr);
    }
    obex_data_ptr->base_data.tx_info.remaining_chain_ptr = NULL;

    if (obex_data_ptr->obex_if_data.dte_ready == FALSE)
    {
      HSU_MSG_LOW("*_obex_disabled: dte_ready is already FALSE");
      return HSU_JSUCCESS;
    }

    obex_data_ptr->obex_if_data.dte_ready = FALSE;

    /* Calling the dtr changed callback if it is set*/
    if (obex_data_ptr->obex_if_data.app_dtr_changed_cb_func != NULL)
    {
      HSU_MSG_LOW("*_obex_disabled: calling the dtr changed callback");
      obex_data_ptr->obex_if_data.app_dtr_changed_cb_func();
    }
  }
  
  return HSU_JSUCCESS;
}

/*===========================================================================
FUNCTION hsu_al_ser_obex_transmit

DESCRIPTION
This function transmit a DSM chain over CDC/OBEX FD.

DEPENDENCIES
Should be called with the Giant mutex locked

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_ser_obex_transmit
( 
  hsu_al_ser_obex_data* obex_data_ptr,
  dsm_item_type *tx_ptr
)
{
  obex_specific_data_struct * obex_if_data = &obex_data_ptr->obex_if_data;
  hsu_al_ser_base_data_for_ser_if * ser_base_data = &obex_data_ptr->base_data;

  HSU_USE_PARAM(obex_if_data);

#ifdef FEATURE_HS_USB_OBEX_BAM
  if (obex_if_data->bam_info.is_bam_mode)
  {
    HSU_MSG_ERROR("hsu_al_ser_obex_transmit(): called in BAM mode");
    dsm_free_packet(&tx_ptr);
    return;
  }
  else
#endif /* FEATURE_HS_USB_OBEX_BAM */
  {
     hsu_al_ser_base_transmit(ser_base_data, tx_ptr);
  }

}

/*===========================================================================
FUNCTION hsu_al_ser_obex_close()
Must be called with the Giant locked
===========================================================================*/
void hsu_al_ser_obex_close
(
 hsu_al_ser_obex_data* obex_data_ptr,
 hsu_al_ser_base_op_complete_cb close_cb,
 boolean abort_io_operations_immediately
)
{
  if (!hsu_os_is_giant_locked_by_curr_task())
  {
    HSU_ERR_FATAL("hsu_al_ser_obex_close: Should be called "
      " with Giant mutex locked!", 0, 0, 0);
    return;
  }
  hsu_al_ser_base_close(&obex_data_ptr->base_data, 
                          close_cb, 
                          abort_io_operations_immediately);

  obex_data_ptr->obex_if_data.app_dtr_changed_cb_func = NULL;
}
