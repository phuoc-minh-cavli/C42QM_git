/*==============================================================================

  USB xDCI State Machine

  GENERAL DESCRIPTION

  EXTERNALIZED FUNCTIONS

  INITALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/classes/src/jslave/fd/cdc/cdc_gsi.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  04/06/15  ac      Initial version
==============================================================================*/

#include "err.h"
#include "hsu_common.h"
#include "jusb_core.h"
#include "hsu_timer.h"
#include "hsu_platform.h"
#include "hsu_log.h"
#include "cdc_fd.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_core_config_int.h"
#include "hsu_al_task_int.h"
#include "hsu_cdc_fd_api.h"
#include "sio.h"
#include "hsu_hwio_dependency.h"
#include "hsu_config_selector.h"
#include "ds_port_mapper.h"

#include "cdc_gsi.h"

#define LINK_STATE_SUSPEND_U3 (3)

// Client buffer and TRB configurations
static const uint32 n_for_UL = 16;
static const uint32 n_for_DL = 5;
static const uint32 UL_client_buf_size = 4096;
static const uint32 DL_client_buf_size = 16384;


// Declarations
boolean cdc_gsi__usb_setup_event_buf_and_gsi_interrupters(cdc_ctx_t *ctx);
boolean cdc_gsi__usb_gsi_endpoint_config(cdc_ctx_t *ctx);
boolean cdc_gsi__setup_l1_exit_irq_mask(cdc_ctx_t *ctx);
boolean cdc_gsi__vote_for_ipa_clock_enablement(cdc_ctx_t *ctx);
boolean cdc_gsi__trb_initialization(cdc_ctx_t *ctx);
boolean cdc_gsi__ipa_per_request_channel(cdc_ctx_t *ctx);
boolean cdc_gsi__ds_port_mapper_configure_instances(cdc_ctx_t *ctx);
boolean cdc_gsi__startxfer_ul(cdc_ctx_t *ctx, boolean is_ep_resume);
boolean cdc_gsi__startxfer_dl(cdc_ctx_t *ctx, boolean is_ep_resume);
boolean cdc_gsi__ipa_per_mgr_connect_channel_ul(cdc_ctx_t *ctx);
boolean cdc_gsi__ipa_per_mgr_connect_channel_dl(cdc_ctx_t *ctx);

boolean cdc_gsi__gsi_post_channel_connect_init(cdc_ctx_t *ctx);
boolean cdc_gsi__gsi_general_cfg(cdc_ctx_t *ctx);
boolean cdc_gsi__set_hwo_bit__updatexfer_ul(cdc_ctx_t *ctx);
boolean cdc_gsi__ring_doorbell_dl(cdc_ctx_t *ctx);

boolean cdc_gsi__usb_exit_low_power_mode(cdc_ctx_t *ctx);

boolean cdc_gsi__ipa_per_stop_and_drop(cdc_ctx_t *ctx);
boolean cdc_gsi__ipa_per_undrop(cdc_ctx_t *ctx);
boolean cdc_gsi__endxfer_ul(cdc_ctx_t *ctx);
boolean cdc_gsi__endxfer_dl(cdc_ctx_t *ctx);
boolean cdc_gsi__chk_UL_for_emptiness_clear_dp_upon_expiry(cdc_ctx_t *ctx);
boolean cdc_gsi__ipa_per_mgr_disconnect_channel_ul(cdc_ctx_t *ctx);
boolean cdc_gsi__ipa_per_mgr_disconnect_channel_dl(cdc_ctx_t *ctx);
boolean cdc_gsi__disable_clear_dp(cdc_ctx_t *ctx);
boolean cdc_gsi__vote_for_ipa_clock_gating(cdc_ctx_t *ctx);
boolean cdc_gsi__usb_enter_low_power_mode(cdc_ctx_t *ctx);

boolean cdc_gsi__ipa_per_mgr_reset_channel_ul(cdc_ctx_t *ctx);
boolean cdc_gsi__ipa_per_mgr_reset_channel_dl(cdc_ctx_t *ctx);

boolean cdc_gsi__chk_ul_dl_for_emptiness_clear_dp_when_req(cdc_ctx_t *ctx);
boolean cdc_gsi__issue_function_wakeup(cdc_ctx_t *ctx);
boolean cdc_gsi__function_wakeup_if_incoming_dl_data(cdc_ctx_t *ctx);
boolean cdc_gsi__is_ul_channel_empty(cdc_ctx_t *ctx);
boolean cdc_gsi__is_dl_channel_empty(cdc_ctx_t *ctx);



static cdc_gsi_ctx_type cdc_gsi_sm_ctx[HSU_MAX_NUM_OF_IFACE_USING_GSI];
static cdc_gsi_global_ctx_type cdc_gsi_sm_global_ctx;

//++ Main State Machine Functions
boolean cdc_gsi__enable(void *cdc_ctx_ptr)
{
  cdc_ctx_t *ctx = (cdc_ctx_t*)cdc_ctx_ptr;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  HSU_ASSERT(cdc_gsi_sm_global_ctx.usb_gsi_fd_instance_counter <= HSU_MAX_NUM_OF_IFACE_USING_GSI);
  cdc_gsi_sm_global_ctx.usb_gsi_fd_instance_counter++;

  cdc_gsi__usb_setup_event_buf_and_gsi_interrupters(ctx); // Only run if there was a restart core
  cdc_gsi__usb_gsi_endpoint_config(ctx); // Only run if there was a restart core
  cdc_gsi__setup_l1_exit_irq_mask(ctx); // Only run if there was a restart core
  cdc_gsi__vote_for_ipa_clock_enablement(ctx);
  cdc_gsi__trb_initialization(ctx);
  cdc_gsi__ipa_per_request_channel(ctx); // Run once only upon bootup

  // Note that port mapper configuration for the interface can only be run after request channel
  // Port mapper can support initialization that is per interface (no need to gather then invoke)
  if (FALSE == ctx->usb_gsi_sm_ctx->is_port_mapper_configured)
  {
    if ((ctx->info.basic.flags & CDC_INIT_INTR_ONLY_ECM_PIPE) == 0)
    {
      if (hsu_get_num_of_net_instances() > 0)
      {
        cdc_gsi__ds_port_mapper_configure_instances(ctx);
  
        // Port mapper allows configuration to be invoked per interface
        HSU_ULOG_1(NO_MSG, BAM_LOG, "config_rmnet_instances, gsi inst %u", cdc_gsi_sm_global_ctx.usb_gsi_fd_instance_counter);
        if (FALSE == ds_port_mapper_config_rmnet_instances(&(ctx->usb_gsi_sm_ctx->dpm_info_ptr)))
        {
          HSU_ERR_FATAL("ds_port_mapper_config_rmnet_instances() failure, instance ", cdc_gsi_sm_global_ctx.usb_gsi_fd_instance_counter, 0, 0);
        }
        ctx->usb_gsi_sm_ctx->is_port_mapper_configured = TRUE;
      }
    }
    else
    {
      HSU_ULOG_1(NO_MSG, BAM_LOG, "config_rmnet_instances_filtered, gsi inst %u", cdc_gsi_sm_global_ctx.usb_gsi_fd_instance_counter);
    }
  }

  cdc_gsi__startxfer_ul(ctx, FALSE);
  cdc_gsi__startxfer_dl(ctx, FALSE);
  cdc_gsi__ipa_per_mgr_connect_channel_ul(ctx); // Start channel
  cdc_gsi__ipa_per_mgr_connect_channel_dl(ctx); // Start channel

  // Must gate GSI enable logic until all interfaces are enabled due to TZ access violation upon StartXfer of DPL
  if (cdc_gsi_sm_global_ctx.usb_gsi_fd_instance_counter == hsu_get_num_of_net_instances())
  {
    uint32 i;
    for (i = 0; i < hsu_get_num_of_net_instances(); i++)
    {
      ctx = cdc_gsi_sm_ctx[i].cdc_ctx_ptr;
      HSU_ULOG_1(NO_MSG, BAM_LOG, "usb_gsi_inst %d", i);

      HWIO_USB30_USB30_GSI_GENERAL_CFG_OUTM(HWIO_USB30_USB30_GSI_GENERAL_CFG_USB30_BLOCK_GSI_WR_GO_BMSK, 0 << HWIO_USB30_USB30_GSI_GENERAL_CFG_USB30_BLOCK_GSI_WR_GO_SHFT);
      // Grouping multiple actions into a single unit for efficiency
      cdc_gsi__gsi_post_channel_connect_init(ctx); // Only run if there was a restart core
      cdc_gsi__gsi_general_cfg(ctx);

      cdc_gsi__set_hwo_bit__updatexfer_ul(ctx);
      cdc_gsi__ring_doorbell_dl(ctx);
    }
  }

  return TRUE;
}


boolean cdc_gsi__disable(void *cdc_ctx_ptr)
{
  cdc_ctx_t *ctx = (cdc_ctx_t*)cdc_ctx_ptr;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  // Must gate GSI enable logic until all interfaces are enabled due to TZ access violation upon StartXfer of DPL
  if (cdc_gsi_sm_global_ctx.usb_gsi_fd_instance_counter == hsu_get_num_of_net_instances())
  {
    uint32 i;
    for (i = 0; i < hsu_get_num_of_net_instances(); i++)
    {
      ctx = cdc_gsi_sm_ctx[i].cdc_ctx_ptr;
      HSU_ULOG_1(NO_MSG, BAM_LOG, "usb_gsi_inst %d", i);

      cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;
      cdc_gsi__usb_exit_low_power_mode(ctx);
      cdc_gsi__vote_for_ipa_clock_enablement(ctx);
      cdc_gsi__ipa_per_stop_and_drop(ctx);
      HWIO_USB30_USB30_GSI_GENERAL_CFG_OUTM(HWIO_USB30_USB30_GSI_GENERAL_CFG_USB30_BLOCK_GSI_WR_GO_BMSK, (1 << HWIO_USB30_USB30_GSI_GENERAL_CFG_USB30_BLOCK_GSI_WR_GO_SHFT));
      cdc_gsi__endxfer_ul(ctx);
      cdc_gsi__endxfer_dl(ctx);
      hsu_os_delay_us(1000);
      cdc_gsi__chk_UL_for_emptiness_clear_dp_upon_expiry(ctx);
      cdc_gsi__ipa_per_mgr_disconnect_channel_ul(ctx);
      cdc_gsi__ipa_per_mgr_disconnect_channel_dl(ctx);
      cdc_gsi__disable_clear_dp(ctx);
      cdc_gsi__ipa_per_mgr_reset_channel_ul(ctx);
      cdc_gsi__ipa_per_mgr_reset_channel_dl(ctx);
      cdc_gsi__ipa_per_undrop(ctx);
      cdc_gsi__vote_for_ipa_clock_gating(ctx);
      // TODO AC
      // Invoking low power mode causes crash due to subsequent controller access by non-accelerated endpoint
    //  cdc_gsi__usb_enter_low_power_mode(ctx);

      // This reset of running pointer allows populate TRB to assign client buffers again without new allocation of memory
      cdc_gsi_ctx->smem_buf_next_free_pa_running_ptr = cdc_gsi_ctx->smem_buf_pa_base_addr;

      cdc_gsi_ctx->xdci_actions.is_post_channel_connect_init_completed = FALSE;
      cdc_gsi_ctx->xdci_actions.is_usb_gsi_endpont_config_completed = FALSE;

      HSU_ASSERT(cdc_gsi_sm_global_ctx.usb_gsi_fd_instance_counter > 0);
      --cdc_gsi_sm_global_ctx.usb_gsi_fd_instance_counter;
    }
  }
  return TRUE;
}



boolean cdc_gsi__suspend(void *cdc_ctx_ptr)
{
  boolean is_empty;
  cdc_ctx_t *ctx = (cdc_ctx_t*)cdc_ctx_ptr;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;
  HWIO_USB30_USB30_GSI_GENERAL_CFG_OUTM(HWIO_USB30_USB30_GSI_GENERAL_CFG_USB30_BLOCK_GSI_WR_GO_BMSK, (1 << HWIO_USB30_USB30_GSI_GENERAL_CFG_USB30_BLOCK_GSI_WR_GO_SHFT));
  while (0 != HWIO_USB30_USB30_GSI_IF_STS1_INM(HWIO_USB30_USB30_GSI_IF_STS1_USB30_GSI_WR_CTRL_STATE_BMSK))
  {
    hsu_os_delay_us(1);
  }
  if (LINK_STATE_SUSPEND_U3 != (HWIO_USB30_DSTS_INM(HWIO_USB30_DSTS_USBLNKST_BMSK) >> HWIO_USB30_DSTS_USBLNKST_SHFT))
  {
    // Return to U0 active state (do not execute remaining sequence)
    hsu_event_log(EVT_UNABLE_TO_STOP_GSI_CHANNEL_DUE_TO_BUS_IN_U0_A, HSU_CORE_IDX_PRIMARY);
    return TRUE;
  }
  // else Execute remainder of the sequence

  is_empty = cdc_gsi__chk_ul_dl_for_emptiness_clear_dp_when_req(ctx);
  if (FALSE == is_empty)
  {
    cdc_gsi_ctx->is_remote_wakeup_requested = TRUE;
  }
  cdc_gsi__ipa_per_mgr_disconnect_channel_ul(ctx);
  cdc_gsi__ipa_per_mgr_disconnect_channel_dl(ctx);

  //++ Check link state again and if it's not in U3, resume GSI channel and set BLOCK_GSI_WR_GO bit back to 0
  if (LINK_STATE_SUSPEND_U3 != (HWIO_USB30_DSTS_INM(HWIO_USB30_DSTS_USBLNKST_BMSK) >> HWIO_USB30_DSTS_USBLNKST_SHFT))
  {
    // Return to U0 active state (but resume GSI channel)
    hsu_event_log(EVT_UNABLE_TO_STOP_GSI_CHANNEL_DUE_TO_BUS_IN_U0_B, HSU_CORE_IDX_PRIMARY);    
    if (cdc_gsi_ctx->xdci_actions.is_clear_ul_data_path_enabled)
    {
      cdc_gsi__disable_clear_dp(ctx);
    }
    cdc_gsi__ipa_per_mgr_connect_channel_ul(ctx); // Start channel
    cdc_gsi__ipa_per_mgr_connect_channel_dl(ctx); // Start channel
    HWIO_USB30_USB30_GSI_GENERAL_CFG_OUTM(HWIO_USB30_USB30_GSI_GENERAL_CFG_USB30_BLOCK_GSI_WR_GO_BMSK, (0 << HWIO_USB30_USB30_GSI_GENERAL_CFG_USB30_BLOCK_GSI_WR_GO_SHFT));
    return TRUE;
  }
  // else Execute remainder of the sequence
  //--

  is_empty = cdc_gsi__is_dl_channel_empty(ctx);
  if (FALSE == is_empty)
  {    
    cdc_gsi_ctx->is_remote_wakeup_requested = TRUE;
  }
  if (cdc_gsi_ctx->xdci_actions.is_clear_ul_data_path_enabled)
  {
    cdc_gsi__disable_clear_dp(ctx);
  }
  if (cdc_gsi_ctx->is_remote_wakeup_requested)
  {
    cdc_gsi_ctx->is_remote_wakeup_requested = FALSE;
    cdc_gsi__issue_function_wakeup(ctx);
  }
  else
  {
    cdc_gsi__vote_for_ipa_clock_gating(ctx);
// Invoking low power mode causes crash due to subsequent controller access by non-accelerated endpoint
//    cdc_gsi__usb_enter_low_power_mode(ctx);
  }
  return TRUE;
}


boolean cdc_gsi__resume(void *cdc_ctx_ptr)
{
  cdc_ctx_t *ctx = (cdc_ctx_t*)cdc_ctx_ptr;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi__usb_exit_low_power_mode(ctx);
  cdc_gsi__vote_for_ipa_clock_enablement(ctx);
  cdc_gsi__ipa_per_mgr_connect_channel_ul(ctx); // Start channel
  cdc_gsi__ipa_per_mgr_connect_channel_dl(ctx); // Start channel

  HWIO_USB30_USB30_GSI_GENERAL_CFG_OUTM(HWIO_USB30_USB30_GSI_GENERAL_CFG_USB30_BLOCK_GSI_WR_GO_BMSK, 0 << HWIO_USB30_USB30_GSI_GENERAL_CFG_USB30_BLOCK_GSI_WR_GO_SHFT);

  return TRUE;
}

#ifdef FEATURE_HS_USB_TEST_SPOOF_DISCONNECT
static void cdc_gsi__spoof_disconnect_cb(timer_cb_data_type data)
{
  static boolean usb_spoof_disconnect = FALSE;
  static boolean usb_spoof_connect = FALSE;

  if (usb_spoof_disconnect)
  {
    jnotify_os(NOTIFY_DEVICE_SPOOF_DISCONNECT, NULL);
    usb_spoof_disconnect = FALSE;
  }
  if (usb_spoof_connect)
  {
    jnotify_os(NOTIFY_DEVICE_SPOOF_CONNECT, NULL);
    usb_spoof_connect = FALSE;
  }
  hsu_timer_set(&(cdc_gsi_sm_global_ctx.spoof_disconnect_timer_id), 250, T_MSEC);
}
#endif // FEATURE_HS_USB_TEST_SPOOF_DISCONNECT


void cdc_gsi__ipa_cb(uint32 ind_type, void* info, void* cb_data)
{
  if (IPA_PER_READY_FOR_CONNECT == ind_type)
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "ipa_ready_for_connect");
  }
  else if (IPA_PER_DATA_ACTIVITY_DETECTED == ind_type)
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "ipa_data_activity_evt");
    ++cdc_gsi_sm_global_ctx.ipa_data_activity_cb_cnt;
    hsu_al_task_signal_remote_wakeup(); // TODO implement function wakeup
  }
  else if (IPA_PER_GENERAL_ERROR == ind_type)
  {
    HSU_ERR_FATAL("IPA indicated top level error", 0, 0, 0);
  }
  /* else do nothing */
}


//++ Helper Functions
boolean cdc_gsi__init(void)
{
  HSU_ULOG_FUNC(BAM_LOG);
  // According to the HPG, the "n" should start with 1
  cdc_gsi_sm_global_ctx.usb_gsi_interrupter_idx_counter = 1;
  cdc_gsi_sm_global_ctx.usb_gsi_fd_instance_counter = 0;
#ifdef FEATURE_HS_USB_TEST_SPOOF_DISCONNECT
  cdc_gsi_sm_global_ctx.spoof_disconnect_timer_id = hsu_timer_init(HSU_TIMER_CID__SPOOF_DISCONNECT, 
                         HSU_TIMER_DISALLOW_REPRIME_WHILE_TIMER_ACTIVE | HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
                         cdc_gsi__spoof_disconnect_cb, 
                         NULL);
  hsu_timer_set(&(cdc_gsi_sm_global_ctx.spoof_disconnect_timer_id), 250, T_MSEC);
#endif //s FEATURE_HS_USB_TEST_SPOOF_DISCONNECT

#ifdef FEATURE_HS_USB_USE_IPA_GSI
    if (IPA_PER_ERROR_NO_ERROR != 
        ipa_per_reg_cb(
          IPA_PER_TYPE_USB,
          cdc_gsi__ipa_cb,
          NULL))
    {
      HSU_ERR_FATAL("ipa_per_reg_cb() returned error", 0, 0, 0);
    }
#endif /* FEATURE_HS_USB_USE_IPA_GSI */
    HSU_ULOG(NO_MSG, BAM_LOG, "ipa_per_reg_cb");

  // TODO for removing compile error during prototyping
  HSU_USE_PARAM(cdc_gsi_sm_ctx);
  HSU_USE_PARAM(cdc_gsi_sm_global_ctx);
  return TRUE;
}


#ifdef FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT
#define HSU_CDC_GSI_IPA_API_TIMEOUT  (500)
static void cdc_gsi__ipa_api_timeout_cb(timer_cb_data_type data)
{
  HSU_ERR_FATAL("Check HSU_0 task for IPA API timeout", 0, 0, 0);
}
#endif // FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT

boolean cdc_gsi__init_ctx_instance(void *cdc_ctx_ptr)
{
  cdc_ctx_t *cdc_ctx = cdc_ctx_ptr;
  #ifdef FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT
  hsu_timer_client_id_type timer_client_id;
  #endif // FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT
  HSU_ULOG_FUNC(BAM_LOG);

  // This is initialized back to back upon boot (unlike enable which occurs during enumeratoin)
  // This implies interface number is not available at this time to pass to port mapper

  if (cdc_ctx)
  {
    cdc_ctx->usb_gsi_sm_ctx = &cdc_gsi_sm_ctx[cdc_gsi_sm_global_ctx.usb_gsi_fd_init_instance_counter];
    cdc_ctx->usb_gsi_sm_ctx->global_ctx = &cdc_gsi_sm_global_ctx;
    cdc_gsi_sm_ctx[cdc_gsi_sm_global_ctx.usb_gsi_fd_init_instance_counter].cdc_ctx_ptr = cdc_ctx;

#ifdef FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT
    switch (cdc_gsi_sm_global_ctx.usb_gsi_fd_init_instance_counter)
    {
      case 0:
        timer_client_id = HSU_TIMER_CID_CDC_GSI_DBG_TIMER_1;
        break;
      case 1:
        timer_client_id = HSU_TIMER_CID_CDC_GSI_DBG_TIMER_2;
        break;
      case 2:
        timer_client_id = HSU_TIMER_CID_CDC_GSI_DBG_TIMER_3;
        break;
      default:
        HSU_ERR_FATAL("CDC GSI # of clients exceeded", 0, 0, 0);
        timer_client_id = HSU_TIMER_CID_CDC_GSI_DBG_TIMER_3; // for warning purposes
        break;
    } 
    cdc_ctx->usb_gsi_sm_ctx->dbg_timer = hsu_timer_init(
      timer_client_id,
      HSU_TIMER_DEFAULT_TIMER,
      cdc_gsi__ipa_api_timeout_cb,
      (timer_cb_data_type)NULL);
#endif // FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT

    cdc_ctx->usb_gsi_sm_ctx->xdci_actions.is_ipa_clk_gated = TRUE;
    ++cdc_gsi_sm_global_ctx.usb_gsi_fd_init_instance_counter;
  }
  else
  {
    HSU_ERR_FATAL("cdc_gsi__init_ctx_instance FAILED with NULL cdc ctx", 0, 0, 0);
  }
  return TRUE;
}

boolean cdc_gsi__deinit_ctx_instance(void *cdc_ctx_ptr)
{
  cdc_ctx_t *cdc_ctx = cdc_ctx_ptr;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(cdc_ctx);

  // This function runs as part of restart core
  cdc_gsi_ctx = cdc_ctx->usb_gsi_sm_ctx;

  // Pipe handle already de-allocated.  Only CDC remaining.
  if (cdc_gsi_ctx->ul_ipa_ep_handle)
  {
    cdc_gsi_ctx->ul_usb_gsi_interrupter_index = 1;
    if (1 < cdc_gsi_sm_global_ctx.usb_gsi_interrupter_idx_counter)
    {
      --cdc_gsi_sm_global_ctx.usb_gsi_interrupter_idx_counter;
    }
  }
  if (cdc_gsi_ctx->dl_ipa_ep_handle)
  { 
    cdc_gsi_ctx->dl_usb_gsi_interrupter_index = 1;
    if (1 < cdc_gsi_sm_global_ctx.usb_gsi_interrupter_idx_counter)
    {
      --cdc_gsi_sm_global_ctx.usb_gsi_interrupter_idx_counter;
    }
  }

#ifdef FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT
  hsu_timer_deinit(&(cdc_gsi_ctx->dbg_timer));
#endif // FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT

  cdc_gsi_ctx->xdci_actions.is_usb_gsi_interrupter_setup_completed = FALSE;
  cdc_gsi_ctx->xdci_actions.is_post_channel_connect_init_completed = FALSE;
  cdc_gsi_ctx->xdci_actions.is_usb_gsi_endpont_config_completed = FALSE;

  cdc_gsi_ctx->cdc_ctx_ptr = NULL;
  cdc_gsi_ctx = NULL;
  cdc_gsi_sm_global_ctx.ipa_data_activity_cb_cnt = 0;
  cdc_gsi_sm_global_ctx.usb_gsi_fd_init_instance_counter = 0;
  return TRUE;
}

boolean cdc_gsi__is_ul_channel_empty(cdc_ctx_t *ctx)
{
  int32 ipa_rc;
  boolean is_empty;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;
  if (cdc_gsi_ctx->ul_pipe_ptr)
  {    
    ipa_rc = ipa_usb_ch_is_empty(cdc_gsi_ctx->ul_ipa_ep_handle, &is_empty);
    if (IPA_SUCCESS != ipa_rc)
    {
      HSU_ERR_FATAL("ipa_usb_ch_is_empty failed", 0, 0, 0);
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__is_ul_channel_empty NO UL pipe");
    is_empty = TRUE;
  }
  return is_empty;
}

boolean cdc_gsi__is_dl_channel_empty(cdc_ctx_t *ctx)
{
  int32 ipa_rc;
  boolean is_empty;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;
  if (cdc_gsi_ctx->dl_pipe_ptr)
  {    
    ipa_rc = ipa_usb_ch_is_empty(cdc_gsi_ctx->dl_ipa_ep_handle, &is_empty);
    if (IPA_SUCCESS != ipa_rc)
    {
      HSU_ERR_FATAL("ipa_usb_ch_is_empty failed", 0, 0, 0);
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__is_dl_channel_empty NO DL pipe");
    is_empty = TRUE;
  }
  return is_empty;
}

boolean cdc_gsi__usb_setup_event_buf_and_gsi_interrupters(cdc_ctx_t *ctx)
{
  // Only run if there was a restart core
  // TODO refactor to move xdci_sm logic back into this function
  cdc_ctx_t* cdc_ctx = ctx;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(cdc_ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;
  if (FALSE == cdc_gsi_ctx->xdci_actions.is_usb_gsi_interrupter_setup_completed)
  {
    cdc_gsi_ctx->xdci_actions.is_usb_gsi_interrupter_setup_completed = TRUE;

    HSU_ULOG_1(NO_MSG, BAM_LOG, "cdc_gsi__usb_setup_event_buf_and_gsi_interrupters %x", (uint32)ctx);

    if (cdc_gsi_ctx->ul_pipe_ptr)
    {
      cdc_gsi_ctx->ul_usb_gsi_interrupter_index = cdc_gsi_ctx->global_ctx->usb_gsi_interrupter_idx_counter;
      core_setup_event_buf_and_gsi_interrupters(cdc_ctx->core_ctx, cdc_gsi_ctx->ul_usb_gsi_interrupter_index, n_for_UL * 2);
      cdc_gsi_ctx->global_ctx->usb_gsi_interrupter_idx_counter++;
    }

    if (cdc_gsi_ctx->dl_pipe_ptr)
    {
      cdc_gsi_ctx->dl_usb_gsi_interrupter_index = cdc_gsi_ctx->global_ctx->usb_gsi_interrupter_idx_counter;
      core_setup_event_buf_and_gsi_interrupters(cdc_ctx->core_ctx, cdc_gsi_ctx->dl_usb_gsi_interrupter_index, 2 * n_for_DL);
      cdc_gsi_ctx->global_ctx->usb_gsi_interrupter_idx_counter++;
    }
  }
  else
  {
    if (cdc_gsi_ctx->ul_pipe_ptr)
    {
      core_reset_event_buffer(cdc_ctx->core_ctx, cdc_gsi_ctx->ul_usb_gsi_interrupter_index);
    }
    if (cdc_gsi_ctx->dl_pipe_ptr)
    {
      core_reset_event_buffer(cdc_ctx->core_ctx, cdc_gsi_ctx->dl_usb_gsi_interrupter_index);
    }
  }
  return TRUE;
}


void cdc_gsi__set_pipe_ep_info(void *dev_ptr, void *pipe_ptr, void *cdc_ctx_ptr)
{
//  jdevice_t dev = (jdevice_t)dev_ptr;
  cdc_ctx_t *cdc_ctx = cdc_ctx_ptr;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  pipe_desc_t* pipe = (pipe_desc_t*)pipe_ptr;
  HSU_ULOG_FUNC(BAM_LOG);
  cdc_gsi_ctx = cdc_ctx->usb_gsi_sm_ctx;
  if (NULL == pipe)
  {
    HSU_ERR_FATAL("NULL pipe in cdc_gsi__set_pipe_ep_info", 0, 0, 0);
  }

  // Found corresponding index to update pipe information
  if ((pipe->is_pipe_hw_accelerated) && (PIPE_BULK == pipe->type) && (DIRECTION_IN == pipe->direction))
  {
    cdc_gsi_ctx->dl_jdevice_ptr = (void*)dev_ptr;
    cdc_gsi_ctx->dl_pipe_ptr = (void*)pipe;
  }
  else if ((pipe->is_pipe_hw_accelerated) && (PIPE_BULK == pipe->type) && (DIRECTION_OUT == pipe->direction))
  {
    cdc_gsi_ctx->ul_jdevice_ptr = (void*)dev_ptr;
    cdc_gsi_ctx->ul_pipe_ptr = (void*)pipe;
  }
  // Else do nothing

  return;
}


void cdc_gsi__clear_pipe_ep_info(void *dev_ptr, void *pipe_ptr, void *cdc_ctx_ptr)
{
//  jdevice_t dev = (jdevice_t)dev_ptr;
  cdc_ctx_t *cdc_ctx = cdc_ctx_ptr;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  pipe_desc_t* pipe = (pipe_desc_t*)pipe_ptr;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(cdc_ctx);

  cdc_gsi_ctx = cdc_ctx->usb_gsi_sm_ctx;

  // Found corresponding index to update pipe information
  if ((PIPE_BULK == pipe->type) && (DIRECTION_IN == pipe->direction) && (pipe->is_pipe_hw_accelerated))
  {
    cdc_gsi_ctx->dl_jdevice_ptr = NULL;
    cdc_gsi_ctx->dl_pipe_ptr = NULL;
  }
  else if ((PIPE_BULK == pipe->type) && (DIRECTION_OUT == pipe->direction) && (pipe->is_pipe_hw_accelerated))
  {
    cdc_gsi_ctx->ul_jdevice_ptr = NULL;
    cdc_gsi_ctx->ul_pipe_ptr = NULL;
  }
  // Else do nothing

  return;
}

boolean cdc_gsi__usb_gsi_endpoint_config(cdc_ctx_t *ctx)
{
  // Only run if there was a restart core
  // TODO refactor to move xdci_sm logic back into this function
  cdc_ctx_t* cdc_ctx = ctx;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;
  if (FALSE == cdc_gsi_ctx->xdci_actions.is_usb_gsi_endpont_config_completed)
  {
    cdc_gsi_ctx->xdci_actions.is_usb_gsi_endpont_config_completed = TRUE;

    // TODO AC verify if the below condition is already populated FD ALLOC
    if (cdc_gsi_ctx->ul_pipe_ptr)
    {
      core_dcd_usb_gsi_endpoint_config(cdc_ctx->core_ctx, cdc_gsi_ctx->ul_pipe_ptr, cdc_gsi_ctx->ul_usb_gsi_interrupter_index);
    }
    if (cdc_gsi_ctx->dl_pipe_ptr)
    {
      core_dcd_usb_gsi_endpoint_config(cdc_ctx->core_ctx, cdc_gsi_ctx->dl_pipe_ptr, cdc_gsi_ctx->dl_usb_gsi_interrupter_index);
    }
  }    
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__usb_gsi_endpoint_config SKIPPED");
  }
  return TRUE;
}

boolean cdc_gsi__setup_l1_exit_irq_mask(cdc_ctx_t *ctx)
{  
  juint8_t  ep_hw_addr;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  core_get_ep_num(ctx->core_ctx, ctx->usb_gsi_sm_ctx->dl_pipe_ptr, &ep_hw_addr);
  HWIO_USB30_USB30_GSI_DEPCMD_ADDR_L_IPA_EPn_OUTI(ctx->usb_gsi_sm_ctx->dl_usb_gsi_interrupter_index - 1, HWIO_USB30_DEPCMDPAR2_REGS_p_DEPCMD_PHYS(ep_hw_addr));
  HWIO_USB30_PWR_EVNT_IRQ_MASK_OUTM(HWIO_USB30_PWR_EVNT_IRQ_MASK_USB30_GSI_L1_EXIT_IRQ_MASK_BMSK, 1 << HWIO_USB30_PWR_EVNT_IRQ_MASK_USB30_GSI_L1_EXIT_IRQ_MASK_SHFT);
  return TRUE;
}

boolean cdc_gsi__vote_for_ipa_clock_enablement(cdc_ctx_t *ctx)
{
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  if (ctx->usb_gsi_sm_ctx->xdci_actions.is_ipa_clk_gated)
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "Actual vote, ipa_per_pm_request");
    ipa_per_pm_request(IPA_PER_TYPE_USB, NULL);
    ctx->usb_gsi_sm_ctx->xdci_actions.is_ipa_clk_gated = FALSE;
  }
  return TRUE;
}

boolean cdc_gsi__trb_initialization(cdc_ctx_t *ctx)
{    
  cdc_gsi_ctx_type *cdc_gsi_ctx;

  DALResult dal_result;
  DALSYSMemHandle mem_handle = NULL;
  DALSYSMemInfo memInfo;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  // Allocate client buffer as one large allocation and then divide them up based on a running pointer    
  // Client buffer sizes:
  //   UL/OUT: 8k buffer with n = 8
  //     Total buffer size needed: 8 * 1024 * 8 = 65536 bytes
  //   DL/IN: 2k buffer with n = 64
  //     Total buffer size needed: 64 * 2 * 1024 = 131072 bytes
  if (NULL == cdc_gsi_ctx->smem_buf_pa_base_addr)
  {
    uint32 size = (UL_client_buf_size * n_for_UL) + (DL_client_buf_size * n_for_DL);

    dal_result = DALSYS_MemRegionAlloc(
      DALSYS_MEM_PROPS_PHYS_CONT /* | DALSYS_MEM_PROPS_UNCACHED */,
      DALSYS_MEM_ADDR_NOT_SPECIFIED, 
      DALSYS_MEM_ADDR_NOT_SPECIFIED,
      size,
      &mem_handle,
      NULL);
    
    if ( (dal_result != DAL_SUCCESS)
      || (mem_handle == NULL) 
      || (DAL_SUCCESS != DALSYS_MemInfo(mem_handle, &memInfo))
      || (memInfo.VirtualAddr == NULL)
      || (memInfo.PhysicalAddr == NULL))
    {
      HSU_ERR_FATAL("uncached mem alloc failed size (%d)", size, 0, 0);
    }
    
    cdc_gsi_ctx->smem_buf_va_base_addr = (void*)memInfo.VirtualAddr;
    cdc_gsi_ctx->smem_buf_pa_base_addr = (void*)memInfo.PhysicalAddr;
    cdc_gsi_ctx->smem_buf_alloc_size = memInfo.dwLen;

    // Update running pointer
    cdc_gsi_ctx->smem_buf_next_free_pa_running_ptr = cdc_gsi_ctx->smem_buf_pa_base_addr;

    if (memInfo.dwLen < size)
    {
      ERR_FATAL("cdc_gsi__trb_initialization: DAL did not allocate enough memory", memInfo.dwLen, size, 0);
    }
  }


  if (cdc_gsi_ctx->ul_pipe_ptr)
  {
    if (FALSE == cdc_gsi_ctx->ul_trb_ring_cookie)
    {
      core_init_ep_trbs_hw_accel(ctx->core_ctx, cdc_gsi_ctx->ul_pipe_ptr,
        &(cdc_gsi_ctx->ul_trb_ring_cookie), &(cdc_gsi_ctx->ul_trb_ring_va_addr), &(cdc_gsi_ctx->ul_trb_ring_pa_addr), n_for_UL);
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "UL trb alloc SKIPPED");
    }
  }

  if (cdc_gsi_ctx->dl_pipe_ptr)
  {
    if (FALSE == cdc_gsi_ctx->dl_trb_ring_cookie)
    {
      core_init_ep_trbs_hw_accel(ctx->core_ctx, cdc_gsi_ctx->dl_pipe_ptr,
        &(cdc_gsi_ctx->dl_trb_ring_cookie), &(cdc_gsi_ctx->dl_trb_ring_va_addr), &(cdc_gsi_ctx->dl_trb_ring_pa_addr), n_for_DL);
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "DL trb alloc SKIPPED");
    }
  }

  // Populate the TRB structure and attach client buffers to the TRBs as defined in the HPG
  if (cdc_gsi_ctx->ul_pipe_ptr)
  {
    core_populate_ep_trb_hw_accel(ctx->core_ctx, cdc_gsi_ctx->ul_pipe_ptr, 
      n_for_UL, &(cdc_gsi_ctx->smem_buf_next_free_pa_running_ptr), UL_client_buf_size,
      cdc_gsi_ctx->ul_trb_ring_va_addr, cdc_gsi_ctx->ul_trb_ring_pa_addr, &(cdc_gsi_ctx->ul_trb_link_pa_addr), cdc_gsi_ctx->ul_usb_gsi_interrupter_index);
  }

  if (cdc_gsi_ctx->dl_pipe_ptr)
  {
    core_populate_ep_trb_hw_accel(ctx->core_ctx, cdc_gsi_ctx->dl_pipe_ptr, 
      n_for_DL, &(cdc_gsi_ctx->smem_buf_next_free_pa_running_ptr), DL_client_buf_size,
      cdc_gsi_ctx->dl_trb_ring_va_addr, cdc_gsi_ctx->dl_trb_ring_pa_addr, &(cdc_gsi_ctx->dl_trb_link_pa_addr), cdc_gsi_ctx->dl_usb_gsi_interrupter_index);
  }
  return TRUE;
}

boolean cdc_gsi__ipa_per_request_channel(cdc_ctx_t *ctx)
{
#ifdef FEATURE_HS_USB_USE_IPA_GSI
  int32 rc;
  cdc_gsi_ctx_type *cdc_gsi_ctx;  
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  // IPA channel is never de-allocated once requested
  if (FALSE == cdc_gsi_ctx->xdci_actions.is_ul_ipa_channel_allocated)
  {
    if (cdc_gsi_ctx->ul_pipe_ptr)
    {
      cdc_gsi_ctx->xdci_actions.is_ul_ipa_channel_allocated = TRUE;
      if (ctx->info.basic.flags & CDC_INIT_DPL)
      {
        HSU_ERR_FATAL("DPL should not have UL channel", 0, 0, 0);
      }
      cdc_gsi_ctx->ul_ipa_ch_cfg.ch_traffic            = IPA_USB_CH_TRAFFIC_XDCI_UL;
      cdc_gsi_ctx->ul_ipa_ch_cfg.usb_ep_id             = ctx->in_pipe.if_num;
      cdc_gsi_ctx->ul_ipa_ch_cfg.xfer_ring_length      = n_for_UL + 1;
      cdc_gsi_ctx->ul_ipa_ch_cfg.xfer_ring_ba_pa       = (uint64)cdc_gsi_ctx->ul_trb_ring_pa_addr;
      cdc_gsi_ctx->ul_ipa_ch_cfg.xfer_ring_last_trb_pa = (uint64)cdc_gsi_ctx->ul_trb_link_pa_addr;
//    ctx->ul_ipa_ch_cfg.const_buf_size                = UL_client_buf_size / cdc_get_max_packet_size(cdc_ctx);
      cdc_gsi_ctx->ul_ipa_ch_cfg.const_buf_size        = UL_client_buf_size / 1024;
      cdc_gsi_ctx->ul_ipa_ch_cfg.DEPCMD_low_pa         = HWIO_USB30_DEPCMDPAR2_REGS_p_DEPCMD_PHYS(cdc_gsi_ctx->ul_ipa_ch_cfg.usb_ep_id);
      cdc_gsi_ctx->ul_ipa_ch_cfg.DEPCMD_high_pa        = 0;
      cdc_gsi_ctx->ul_ipa_ch_cfg.GEVNTCOUNT_low_pa     = HWIO_USB30_GEVNTADRLO_REGS_p_GEVNTCOUNT_PHYS(cdc_gsi_ctx->ul_usb_gsi_interrupter_index);
      cdc_gsi_ctx->ul_ipa_ch_cfg.GEVNTCOUNT_high_pa    = 0;
      // Populate scratch registers
      rc = ipa_usb_ch_alloc(&(cdc_gsi_ctx->ul_ipa_ch_cfg), &(cdc_gsi_ctx->ul_ipa_ep_handle), &(cdc_gsi_ctx->ul_ch_doorbell_addr));
      if (IPA_SUCCESS != rc)
      {
        HSU_ERR_FATAL("ipa_usb_ch_alloc failed", 0, 0, 0);
      }
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_request_channel NO UL pipe");
    }
  }

  // IPA channel is never de-allocated once requested
  if (FALSE == cdc_gsi_ctx->xdci_actions.is_dl_ipa_channel_allocated)
  {
    if (cdc_gsi_ctx->dl_pipe_ptr)
    {
      cdc_gsi_ctx->xdci_actions.is_dl_ipa_channel_allocated = TRUE;
      if (ctx->info.basic.flags & CDC_INIT_DPL)
      {
        cdc_gsi_ctx->dl_ipa_ch_cfg.ch_traffic            = IPA_USB_CH_TRAFFIC_XDCI_DPL;
      }
      else
      {
        cdc_gsi_ctx->dl_ipa_ch_cfg.ch_traffic            = IPA_USB_CH_TRAFFIC_XDCI_DL;
      }
      cdc_gsi_ctx->dl_ipa_ch_cfg.usb_ep_id             = ctx->in_pipe.if_num;
      cdc_gsi_ctx->dl_ipa_ch_cfg.xfer_ring_length      = 2 * n_for_DL + 2;
      cdc_gsi_ctx->dl_ipa_ch_cfg.xfer_ring_ba_pa       = (uint64)cdc_gsi_ctx->dl_trb_ring_pa_addr;
      cdc_gsi_ctx->dl_ipa_ch_cfg.xfer_ring_last_trb_pa = (uint64)cdc_gsi_ctx->dl_trb_link_pa_addr;
//    ctx->dl_ipa_ch_cfg.const_buf_size                = DL_client_buf_size / cdc_get_max_packet_size(cdc_ctx);
      cdc_gsi_ctx->dl_ipa_ch_cfg.const_buf_size        = DL_client_buf_size / 1024;
      cdc_gsi_ctx->dl_ipa_ch_cfg.DEPCMD_low_pa         = HWIO_USB30_DEPCMDPAR2_REGS_p_DEPCMD_PHYS(cdc_gsi_ctx->dl_ipa_ch_cfg.usb_ep_id);
      cdc_gsi_ctx->dl_ipa_ch_cfg.DEPCMD_high_pa        = 0;
      cdc_gsi_ctx->dl_ipa_ch_cfg.GEVNTCOUNT_low_pa     = HWIO_USB30_GEVNTADRLO_REGS_p_GEVNTCOUNT_PHYS(cdc_gsi_ctx->dl_usb_gsi_interrupter_index);
      cdc_gsi_ctx->dl_ipa_ch_cfg.GEVNTCOUNT_high_pa    = 0;
      // Populate scratch registers
      rc = ipa_usb_ch_alloc(&(cdc_gsi_ctx->dl_ipa_ch_cfg), &(cdc_gsi_ctx->dl_ipa_ep_handle), &(cdc_gsi_ctx->dl_ch_doorbell_addr));
      if (IPA_SUCCESS != rc)
      {
        HSU_ERR_FATAL("ipa_usb_ch_alloc failed", 0, 0, 0);
      }
    }      
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_request_channel NO DL pipe");
    }
  }
#endif // FEATURE_HS_USB_USE_IPA_GSI
  return TRUE;
}

boolean cdc_gsi__ds_port_mapper_configure_instances(cdc_ctx_t *ctx)
{
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  ds_port_mapper_qmi_rmnet_info_type *dpm_info_ptr;
  uint32 gsi_idx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;
  if ((cdc_gsi_sm_global_ctx.usb_gsi_fd_instance_counter > HSU_MAX_NUM_OF_IFACE_USING_GSI)
      || (cdc_gsi_sm_global_ctx.usb_gsi_fd_instance_counter == 0))
  {
    HSU_ERR_FATAL("usb_gsi_fd_instance_counter out of range %d", cdc_gsi_sm_global_ctx.usb_gsi_fd_instance_counter, 0, 0);
  }


  // Port mapper can handle per interface initialization; thus removing the need to know total number of
  //   interfaces.
  //  gsi_idx = cdc_gsi_sm_global_ctx.usb_gsi_fd_instance_counter - 1;
  gsi_idx = 0;

  if (FALSE == cdc_gsi_ctx->is_port_mapper_configured)
  {
    if (ctx->info.basic.flags & CDC_INIT_DPL)
    {
      dpm_info_ptr = &(cdc_gsi_ctx->dpm_info_ptr);
      dpm_info_ptr->peripheral_type = DATA_EP_TYPE_HSUSB_V01;
      dpm_info_ptr->num_ctrl_ch = dpm_info_ptr->num_ctrl_ch + 1;
      dpm_info_ptr->ctrl_ch_info[gsi_idx].ctl_port = SIO_PORT_USB_DPL;
      dpm_info_ptr->data_ch_info[gsi_idx].data_port = SIO_PORT_USB_DPL;
      dpm_info_ptr->ctrl_ch_info[gsi_idx].def_per_end_pt = (uint32)ctx->in_pipe.if_num;
      dpm_info_ptr->num_data_ch = dpm_info_ptr->num_data_ch + 1;
      dpm_info_ptr->data_ch_info[gsi_idx].per_end_pt = dpm_info_ptr->ctrl_ch_info[gsi_idx].def_per_end_pt;
      dpm_info_ptr->data_ch_info[gsi_idx].is_hw_acc = TRUE;  
      // Port mapper is never de-initialized
      cdc_gsi_ctx->is_port_mapper_configured = TRUE;
    }
    else // Assume ECM
    {
      dpm_info_ptr = &(cdc_gsi_ctx->dpm_info_ptr);
      dpm_info_ptr->peripheral_type = DATA_EP_TYPE_HSUSB_V01;
      dpm_info_ptr->num_ctrl_ch = dpm_info_ptr->num_ctrl_ch + 1;
      dpm_info_ptr->ctrl_ch_info[gsi_idx].ctl_port = SIO_PORT_USB_NET_WWAN;
      dpm_info_ptr->ctrl_ch_info[gsi_idx].def_per_end_pt = (uint32)ctx->in_pipe.if_num;
      dpm_info_ptr->num_data_ch = dpm_info_ptr->num_data_ch + 1;
      dpm_info_ptr->data_ch_info[gsi_idx].per_end_pt = dpm_info_ptr->ctrl_ch_info[gsi_idx].def_per_end_pt;
      dpm_info_ptr->data_ch_info[gsi_idx].is_hw_acc = TRUE;
      // Port mapper is never de-initialized
      cdc_gsi_ctx->is_port_mapper_configured = TRUE;
    }
  }


  return TRUE;
}

boolean cdc_gsi__startxfer_ul(cdc_ctx_t *ctx, boolean is_ep_resume)
{
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (cdc_gsi_ctx->ul_pipe_ptr)
  {
    if (0 == ((pipe_desc_t*)(cdc_gsi_ctx->ul_pipe_ptr))->xfer_rsc_idx_hw_accelerated)
    {
      // When is_ep_resume is TRUE, the USB GSI writeback capture address is used
      core_begin_hw_accel_xfer(ctx->core_ctx, cdc_gsi_ctx->ul_pipe_ptr, cdc_gsi_ctx->ul_trb_ring_pa_addr, cdc_gsi_ctx->ul_usb_gsi_interrupter_index, is_ep_resume);
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__startxfer_ul SKIPPED");
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__startxfer_ul NO UL pipe");
  }
  return TRUE;
}

boolean cdc_gsi__startxfer_dl(cdc_ctx_t *ctx, boolean is_ep_resume)
{
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (cdc_gsi_ctx->dl_pipe_ptr)
  {
    if (0 == ((pipe_desc_t*)(cdc_gsi_ctx->dl_pipe_ptr))->xfer_rsc_idx_hw_accelerated)
    {
      // When is_ep_resume is TRUE, the USB GSI writeback capture address is used
      core_begin_hw_accel_xfer(ctx->core_ctx, cdc_gsi_ctx->dl_pipe_ptr, cdc_gsi_ctx->dl_trb_ring_pa_addr, cdc_gsi_ctx->dl_usb_gsi_interrupter_index, is_ep_resume);
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__startxfer_dl SKIPPED");
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__startxfer_dl NO DL pipe");
  }
  return TRUE;
}


boolean cdc_gsi__ipa_per_mgr_connect_channel_ul(cdc_ctx_t *ctx)
{
  int32 rc;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (cdc_gsi_ctx->ul_pipe_ptr)
  {
    if (FALSE == cdc_gsi_ctx->xdci_actions.is_ul_ipa_channel_connected)
    {
      cdc_gsi_ctx->xdci_actions.is_ul_ipa_channel_connected = TRUE;

      cdc_gsi_ctx->ul_ipa_start_cfg.xferrscidx = ((pipe_desc_t*)(cdc_gsi_ctx->ul_pipe_ptr))->xfer_rsc_idx_hw_accelerated;
      rc = ipa_usb_ch_start(cdc_gsi_ctx->ul_ipa_ep_handle, &(cdc_gsi_ctx->ul_ipa_start_cfg));
      if (IPA_SUCCESS != rc)
      {
        HSU_ERR_FATAL("ipa_usb_ch_start failed", 0, 0, 0);
      }
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_mgr_connect_channel_ul SKIPPED");
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_mgr_connect_channel_ul NO UL pipe");
  }
  return TRUE;
}

boolean cdc_gsi__ipa_per_mgr_connect_channel_dl(cdc_ctx_t *ctx)
{
  int32 rc;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (cdc_gsi_ctx->dl_pipe_ptr)
  {
    if (FALSE == cdc_gsi_ctx->xdci_actions.is_dl_ipa_channel_connected)
    {
      cdc_gsi_ctx->xdci_actions.is_dl_ipa_channel_connected = TRUE;

      cdc_gsi_ctx->dl_ipa_start_cfg.xferrscidx = ((pipe_desc_t*)(cdc_gsi_ctx->dl_pipe_ptr))->xfer_rsc_idx_hw_accelerated;
      rc = ipa_usb_ch_start(cdc_gsi_ctx->dl_ipa_ep_handle, &(cdc_gsi_ctx->dl_ipa_start_cfg));
      if (IPA_SUCCESS != rc)
      {
        HSU_ERR_FATAL("ipa_usb_ch_start failed", 0, 0, 0);
      }
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_mgr_connect_channel_dl SKIPPED");
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_mgr_connect_channel_dl NO DL pipe");
  }
  return TRUE;
}

boolean cdc_gsi__gsi_post_channel_connect_init(cdc_ctx_t *ctx)
{
  // Configure doorbell addrress
  // Configure ring base address
  // Set general GSI config
  // Set GSI enable bit

  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);
  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (FALSE == cdc_gsi_ctx->xdci_actions.is_post_channel_connect_init_completed)
  {
    cdc_gsi_ctx->xdci_actions.is_post_channel_connect_init_completed = TRUE;

    if (cdc_gsi_ctx->ul_pipe_ptr)
    {
      core_gsi_post_channel_connect_init(ctx->core_ctx, cdc_gsi_ctx->ul_pipe_ptr, cdc_gsi_ctx->ul_usb_gsi_interrupter_index, (void*)(uint32)(cdc_gsi_ctx->ul_ch_doorbell_addr), cdc_gsi_ctx->ul_trb_ring_pa_addr);
    }
    if (cdc_gsi_ctx->dl_pipe_ptr)
    {
      core_gsi_post_channel_connect_init(ctx->core_ctx, cdc_gsi_ctx->dl_pipe_ptr, cdc_gsi_ctx->dl_usb_gsi_interrupter_index, (void*)(uint32)(cdc_gsi_ctx->dl_ch_doorbell_addr), cdc_gsi_ctx->dl_trb_ring_pa_addr);
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__gsi_post_channel_connect_init SKIPPED");
  }
  return TRUE;
}


boolean cdc_gsi__gsi_general_cfg(cdc_ctx_t *ctx)
{
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  core_gsi_general_cfg(ctx->core_ctx);
  return TRUE;
}

boolean cdc_gsi__set_hwo_bit__updatexfer_ul(cdc_ctx_t *ctx)
{
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;
  if (cdc_gsi_ctx->ul_pipe_ptr)
  {
    core_set_hwio_bit__updatexfer_ul(ctx->core_ctx, cdc_gsi_ctx->ul_pipe_ptr, n_for_UL, cdc_gsi_ctx->ul_trb_ring_va_addr);
  }
  return TRUE;
}

boolean cdc_gsi__ring_doorbell_dl(cdc_ctx_t *ctx)
{
#ifdef FEATURE_HS_USB_USE_IPA_GSI
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (cdc_gsi_ctx->dl_pipe_ptr)
  {
    ipa_usb_ch_ring_db(cdc_gsi_ctx->dl_ipa_ep_handle, (uint32)cdc_gsi_ctx->dl_trb_link_pa_addr);
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ring_doorbell_dl NO DL pipe");
  }
#endif // FEATURE_HS_USB_USE_IPA_GSI
  return TRUE;
}

boolean cdc_gsi__usb_exit_low_power_mode(cdc_ctx_t *ctx)
{
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  hsu_os_bus_core_exit_lpm(hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY));  
  return TRUE;
}

boolean cdc_gsi__ipa_per_stop_and_drop(cdc_ctx_t *ctx)
{  
  int32 ipa_rc;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (cdc_gsi_ctx->dl_pipe_ptr)
  {
    if (FALSE == cdc_gsi_ctx->xdci_actions.is_drop_dl_data_enabled)
    {
      cdc_gsi_ctx->xdci_actions.is_drop_dl_data_enabled = TRUE;
      ipa_rc = ipa_usb_ch_drop_data(cdc_gsi_ctx->dl_ipa_ep_handle);
      if (IPA_SUCCESS != ipa_rc)
      {
        HSU_ERR_FATAL("ipa_usb_ch_drop_data failed  handle %d", cdc_gsi_ctx->dl_ipa_ep_handle, 0, 0);
      }
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_stop_and_drop SKIPPED");
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_stop_and_drop NO DL pipe");
  }
  return TRUE;
}

boolean cdc_gsi__ipa_per_undrop(cdc_ctx_t *ctx)
{
  int32 ipa_rc;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (cdc_gsi_ctx->dl_pipe_ptr)
  {
    if (cdc_gsi_ctx->xdci_actions.is_drop_dl_data_enabled)
    {
      cdc_gsi_ctx->xdci_actions.is_drop_dl_data_enabled = FALSE;
      ipa_rc = ipa_usb_ch_undrop_data(cdc_gsi_ctx->dl_ipa_ep_handle);
      if (IPA_SUCCESS != ipa_rc)
      {
        HSU_ERR_FATAL("ipa_usb_ch_drop_data failed  handle %d", cdc_gsi_ctx->dl_ipa_ep_handle, 0, 0);
      }
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_undrop SKIPPED");
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_undrop NO DL pipe");
  }
  return TRUE;
}

boolean cdc_gsi__endxfer_ul(cdc_ctx_t *ctx)
{
  jresult_t rc;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (cdc_gsi_ctx->ul_pipe_ptr)
  {
    if (0 != ((pipe_desc_t*)(cdc_gsi_ctx->ul_pipe_ptr))->xfer_rsc_idx_hw_accelerated)
    {
      rc = hsu_cdc_fd_api_cdc_abort_read(ctx);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ERR_FATAL("cdc_gsi__endxfer_ul failed: code %u, ctx %u", 
          rc, ctx, 0);
      }
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__endxfer_ul SKIPPING xfer already stopped");
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__endxfer_ul NO UL pipe");
  }
  return TRUE;
}

boolean cdc_gsi__endxfer_dl(cdc_ctx_t *ctx)
{
  jresult_t rc;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (cdc_gsi_ctx->dl_pipe_ptr) 
  {
    if (0 != ((pipe_desc_t*)(cdc_gsi_ctx->dl_pipe_ptr))->xfer_rsc_idx_hw_accelerated)
    {
      rc = hsu_cdc_fd_api_cdc_abort_write(ctx);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ERR_FATAL("cdc_gsi__endxfer_dl failed: code %u, ctx %u", 
          rc, ctx, 0);
      }
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__endxfer_dl SKIPPING xfer already stopped");
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__endxfer_dl NO DL pipe");
  }
  return TRUE;
}

boolean cdc_gsi__chk_UL_for_emptiness_clear_dp_upon_expiry(cdc_ctx_t *ctx)
{
  const uint32 num_of_iter = 100;
  boolean is_empty;
  uint32 i;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  for (i = 0; i < num_of_iter; i++)
  {
    is_empty = cdc_gsi__is_ul_channel_empty(ctx);
    if (is_empty)
    {
      break;
    }
    hsu_os_delay_us(20);
  }
  // Since not empty within the specified time, purge UL data
  if (num_of_iter == i)
  {
    int32 ipa_rc;
    if (cdc_gsi_ctx->ul_pipe_ptr)
    {
      if (FALSE == cdc_gsi_ctx->xdci_actions.is_clear_ul_data_path_enabled)
      {
        ipa_rc = ipa_usb_enable_clear_dp(cdc_gsi_ctx->ul_ipa_ep_handle, &cdc_gsi_ctx->ul_clear_dp_msg_id);
        cdc_gsi_ctx->xdci_actions.is_clear_ul_data_path_enabled = TRUE;
        if (IPA_SUCCESS != ipa_rc)
        {
          HSU_ERR_FATAL("ipa_usb_enable_clear_dp( ) failed, handle %d", cdc_gsi_ctx->ul_ipa_ep_handle, 0, 0);
        }
      }

      // Give UL another chance to drain after clear data path
      for (i = 0; i < num_of_iter; i++)
      {
        is_empty = cdc_gsi__is_ul_channel_empty(ctx);
        if (is_empty)
        {
          break;
        }
        hsu_os_delay_us(20);
      }
      if (num_of_iter == i)
      {
        HSU_ERR_FATAL("UL not empty even after clear_dp", 0, 0, 0);
      }
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__chk_UL_for_emptiness_clear_dp_upon_expiry NO UL pipe");
    }
  }
  return TRUE;
}


boolean cdc_gsi__chk_ul_dl_for_emptiness_clear_dp_when_req(cdc_ctx_t *ctx)
{
  const uint32 num_of_iter = 100;
  boolean is_ul_empty;
  boolean is_dl_empty;
  uint32 i;
  int32 ipa_rc;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  for (i = 0; i < num_of_iter; i++)
  {
    is_dl_empty = cdc_gsi__is_dl_channel_empty(ctx);
    if (FALSE == is_dl_empty)
    {
      return FALSE; // Caller will issue remote wakeup
    }
    // Allow time for UL to drain
    is_ul_empty = cdc_gsi__is_ul_channel_empty(ctx);
    if (is_ul_empty)
    {
      break;
    }
    hsu_os_delay_us(20);
  }
  // Since not empty within the specified time, purge UL data
  if (num_of_iter == i)
  {
    if (cdc_gsi_ctx->ul_pipe_ptr)
    {
      if (FALSE == cdc_gsi_ctx->xdci_actions.is_clear_ul_data_path_enabled)
      {
        ipa_rc = ipa_usb_enable_clear_dp(cdc_gsi_ctx->ul_ipa_ep_handle, &(cdc_gsi_ctx->ul_clear_dp_msg_id));
        cdc_gsi_ctx->xdci_actions.is_clear_ul_data_path_enabled = TRUE;
        if (IPA_SUCCESS != ipa_rc)
        {
          HSU_ERR_FATAL("ipa_usb_enable_clear_dp failed handle %d", cdc_gsi_ctx->ul_ipa_ep_handle, 0, 0);
        }
      }
      // Allow time for UL to drain
      for (i = 0; i < num_of_iter; i++)
      {
        is_ul_empty = cdc_gsi__is_ul_channel_empty(ctx);
        if (is_ul_empty)
        {
          break;
        }
        hsu_os_delay_us(20);
      }
      if (num_of_iter == i)
      {
        HSU_ERR_FATAL("UL pipe still not empty after draining  ctx %d", ctx, 0, 0);
      }
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__chk_ul_dl_for_emptiness_clear_dp_when_req NO UL pipe");
    }
  }
  return TRUE;
}


boolean cdc_gsi__ipa_per_mgr_disconnect_channel_ul(cdc_ctx_t *ctx)
{
  int32 ipa_rc;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (cdc_gsi_ctx->ul_pipe_ptr)
  {
    if (cdc_gsi_ctx->xdci_actions.is_ul_ipa_channel_connected)
    {
      cdc_gsi_ctx->xdci_actions.is_ul_ipa_channel_connected = FALSE;

      #ifdef FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT
      hsu_timer_set(&(cdc_gsi_ctx->dbg_timer), HSU_CDC_GSI_IPA_API_TIMEOUT, T_MSEC);
      #endif // FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT
      ipa_rc =  ipa_usb_ch_stop(cdc_gsi_ctx->ul_ipa_ep_handle);
      if (IPA_SUCCESS != ipa_rc)
      {
        HSU_ERR_FATAL("ipa_usb_ch_stop( ) failed, handle %d", cdc_gsi_ctx->ul_ipa_ep_handle, 0, 0);
      }
      #ifdef FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT
      hsu_timer_cancel(&(cdc_gsi_ctx->dbg_timer));
      #endif // FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_mgr_disconnect_channel_ul SKIPPED");
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_mgr_disconnect_channel_ul NO UL pipe");
  }
  return TRUE;
}

boolean cdc_gsi__ipa_per_mgr_disconnect_channel_dl(cdc_ctx_t *ctx)
{
  int32 ipa_rc;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (cdc_gsi_ctx->dl_pipe_ptr)
  {
    if (cdc_gsi_ctx->xdci_actions.is_dl_ipa_channel_connected)
    {
      cdc_gsi_ctx->xdci_actions.is_dl_ipa_channel_connected = FALSE;

      #ifdef FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT
      hsu_timer_set(&(cdc_gsi_ctx->dbg_timer), HSU_CDC_GSI_IPA_API_TIMEOUT, T_MSEC);
      #endif // FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT
      ipa_rc =  ipa_usb_ch_stop(cdc_gsi_ctx->dl_ipa_ep_handle);
      if (IPA_SUCCESS != ipa_rc)
      {
        HSU_ERR_FATAL("ipa_usb_ch_stop( ) failed, handle %d", cdc_gsi_ctx->dl_ipa_ep_handle, 0, 0);
      }
      #ifdef FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT
      hsu_timer_cancel(&(cdc_gsi_ctx->dbg_timer));
      #endif // FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_mgr_disconnect_channel_dl SKIPPED");
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_mgr_disconnect_channel_dl NO DL pipe");
  }
  return TRUE;
}

boolean cdc_gsi__disable_clear_dp(cdc_ctx_t *ctx)
{
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (cdc_gsi_ctx->ul_pipe_ptr)
  {
    if (cdc_gsi_ctx->xdci_actions.is_clear_ul_data_path_enabled)
    {
      int32 ipa_rc;
      cdc_gsi_ctx->xdci_actions.is_clear_ul_data_path_enabled = FALSE;
      ipa_rc = ipa_usb_disable_clear_dp(cdc_gsi_ctx->ul_ipa_ep_handle, cdc_gsi_ctx->ul_clear_dp_msg_id);

      if (IPA_SUCCESS != ipa_rc)
      {
        HSU_ERR_FATAL("ipa_usb_ch_stop( ) failed, handle %d", cdc_gsi_ctx->ul_ipa_ep_handle, 0, 0);
      }
      cdc_gsi_ctx->ul_clear_dp_msg_id = 0;
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__disable_clear_dp SKIPPED");
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__disable_clear_dp NO UL pipe");
  }
  return TRUE;
}

boolean cdc_gsi__vote_for_ipa_clock_gating(cdc_ctx_t *ctx)
{
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (FALSE == cdc_gsi_ctx->xdci_actions.is_ipa_clk_gated)
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "Actual vote, ipa_per_pm_release");
    ipa_per_pm_release(IPA_PER_TYPE_USB, IPA_PER_PM_RES_CLK_GATING_BM, NULL);
    cdc_gsi_ctx->xdci_actions.is_ipa_clk_gated = TRUE;
  }
  return TRUE;
}

boolean cdc_gsi__usb_enter_low_power_mode(cdc_ctx_t *ctx)
{
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  // TODO AC (how does this incorprate with function suspend?)
  hsu_os_bus_core_enter_lpm(hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY), FALSE, FALSE);  
  return TRUE;
}

boolean cdc_gsi__ipa_per_mgr_reset_channel_ul(cdc_ctx_t *ctx)
{
  int32 ipa_rc;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;
  if (cdc_gsi_ctx->ul_pipe_ptr)
  {
    ipa_rc = ipa_usb_ch_reset(cdc_gsi_ctx->ul_ipa_ep_handle); 
    if (IPA_SUCCESS != ipa_rc)
    {
      HSU_ERR_FATAL("cdc_gsi__ipa_per_mgr_reset_channel_ul( ) failed, handle %d", cdc_gsi_ctx->ul_ipa_ep_handle, 0, 0);    
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_mgr_reset_channel_ul NO UL pipe");
  }
  return TRUE;
}

boolean cdc_gsi__ipa_per_mgr_reset_channel_dl(cdc_ctx_t *ctx)
{
  int32 ipa_rc;
  cdc_gsi_ctx_type *cdc_gsi_ctx;
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  cdc_gsi_ctx = ctx->usb_gsi_sm_ctx;

  if (cdc_gsi_ctx->dl_pipe_ptr)
  {
    ipa_rc = ipa_usb_ch_reset(cdc_gsi_ctx->dl_ipa_ep_handle); 
    if (IPA_SUCCESS != ipa_rc)
    {
      HSU_ERR_FATAL("cdc_gsi__ipa_per_mgr_reset_channel_ul( ) failed, handle %d", cdc_gsi_ctx->dl_ipa_ep_handle, 0, 0);    
    }
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "cdc_gsi__ipa_per_mgr_reset_channel_dl NO DL pipe");
  }
  return TRUE;
}

boolean cdc_gsi__issue_function_wakeup(cdc_ctx_t *ctx)
{
  // TODO AC when function wakeup is integrated
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);

  return TRUE;
}

boolean cdc_gsi__function_wakeup_if_incoming_dl_data(cdc_ctx_t *ctx)
{
  // TODO AC when function wakeup is integrated
  HSU_ULOG_FUNC(BAM_LOG);
  CDC_GSI_CTX_CHK(ctx);
 
  return TRUE;
}

//-- End of Helper Functions
