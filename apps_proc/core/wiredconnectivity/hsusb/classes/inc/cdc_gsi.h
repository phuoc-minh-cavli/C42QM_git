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
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/classes/inc/cdc_gsi.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  04/06/15  ac      Initial version
==============================================================================*/


#ifndef _CDC_GSI__H_
#define _CDC_GSI__H_

#include "rex.h"
#include "ipa_usb.h"
#include "ds_port_mapper.h"

typedef enum
{
  HSU_DEPCMD_ENUM_STARTXFER,
  HSU_DEPCMD_ENUM_UPDATEXFER,
} hsu_depcmd_begin_hw_accel_xfer_enum;

typedef struct
{
  uint32 usb_gsi_interrupter_idx_counter; // For incrementally assign the next "n" interrupter index
  uint32 usb_gsi_fd_init_instance_counter; // For incremental creation of individual state machine context (init/deinit ctx)
  uint32 usb_gsi_fd_instance_counter; // For incremental creation of individual state machine context (enable/disable ctx)
#ifdef FEATURE_HS_USB_TEST_SPOOF_DISCONNECT
  hsu_timer_handle_type spoof_disconnect_timer_id; /* unique timer ID  */
#endif // FEATURE_HS_USB_TEST_SPOOF_DISCONNECT

  uint32 ipa_data_activity_cb_cnt; // Need to update this to per instance
} cdc_gsi_global_ctx_type;

typedef struct
{
  boolean is_ipa_clk_gated; // Whether vote for IPA clock gating was initiated

  boolean is_ul_ipa_channel_allocated; // Allocate once and keep handle
  boolean is_ul_ipa_channel_connected; // Whether the IPA/GSI channel was started and channel is not in reset
  boolean is_clear_ul_data_path_enabled;
//  boolean is_ul_trb_and_evnt_buff_initialized; // Whether ring TRB and event buffers are initialized

  boolean is_dl_ipa_channel_allocated; // Allocate once and keep handle
  boolean is_dl_ipa_channel_connected; // Whether the IPA/GSI channel was started and channel is not in reset
  boolean is_drop_dl_data_enabled;
//  boolean is_dl_trb_and_evnt_buff_initialized; // Whether ring TRB and event buffers are initialized

  boolean is_usb_gsi_interrupter_setup_completed;
  boolean is_post_channel_connect_init_completed;
  boolean is_usb_gsi_endpont_config_completed;
//  boolean is_clear_ul_data_path_activated; // True when clear_dp was called but disable_clear_dp not yet called
//  boolean is_stop_and_drop_data_path_activated;
} cdc_gsi_action_type;

typedef struct
{
  // Note on EPs
  // [31..16] TX/IN/DL  [15..0] RX/OUT/UL

  // Client Buffers
  void  *smem_buf_va_base_addr;
  void  *smem_buf_pa_base_addr;
  uint32  smem_buf_alloc_size;
  void  *smem_buf_next_free_pa_running_ptr;

  // UL
  void  *ul_jdevice_ptr; // jdevice_t / a.k.a. dcd_dev
  void  *ul_pipe_ptr; // *pipe_desc_t
  ipa_usb_ch_handle_t    ul_ipa_ep_handle; // handle returned by IPA
  ipa_usb_ch_alloc_cfg_t ul_ipa_ch_cfg;
  ipa_usb_ch_start_cfg_t ul_ipa_start_cfg;
  ipa_force_clear_qmi_msg_id ul_clear_dp_msg_id;

  uint8  ul_usb_gsi_interrupter_index;
  void  *ul_trb_ring_va_addr;
  void  *ul_trb_ring_pa_addr;
  void  *ul_trb_ring_cookie;
  void  *ul_trb_link_pa_addr;  // Required by IPA driver
  void  *ul_event_va_buffer_addr;
  uint64 ul_ch_doorbell_addr;


  // DL
  void  *dl_jdevice_ptr; // jdevice_t / a.k.a. dcd_dev
  void  *dl_pipe_ptr; // *pipe_desc_t
  ipa_usb_ch_handle_t    dl_ipa_ep_handle; // handle returned by IPA
  ipa_usb_ch_alloc_cfg_t dl_ipa_ch_cfg;
  ipa_usb_ch_start_cfg_t dl_ipa_start_cfg;

  uint8  dl_usb_gsi_interrupter_index;
  void  *dl_trb_ring_va_addr;
  void  *dl_trb_ring_pa_addr;
  void  *dl_trb_ring_cookie;
  void  *dl_trb_link_pa_addr;  // Required by IPA driver
  void  *dl_event_va_buffer_addr;
  uint64 dl_ch_doorbell_addr;

  boolean is_port_mapper_configured;  // Configured once and assume EP mapping remains static
  ds_port_mapper_qmi_rmnet_info_type dpm_info_ptr;

  boolean is_remote_wakeup_requested;

#ifdef FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT
  hsu_timer_handle_type dbg_timer; // Timeout when invoking IPA APIs
#endif // FEATURE_HS_USB_DEBUG_IPA_API_TIMEOUT

  void  *cdc_ctx_ptr; 
  cdc_gsi_action_type xdci_actions;
  cdc_gsi_global_ctx_type *global_ctx;
} cdc_gsi_ctx_type;

#define CDC_GSI_CTX_CHK(ctx) \
  if ((NULL == ctx) || (NULL == ctx->usb_gsi_sm_ctx)) \
    ERR_FATAL("NULL cdc gsi ctx", 0, 0, 0);


#define HSU_MAX_NUM_OF_USB_GSI_CHANNELS (3)
#define HSU_MAX_NUM_OF_IFACE_USING_GSI (2)


boolean cdc_gsi__init(void);
boolean cdc_gsi__init_ctx_instance(void *cdc_ctx_ptr);
boolean cdc_gsi__deinit_ctx_instance(void *cdc_ctx_ptr);
boolean cdc_gsi__enable(void *cdc_ctx_ptr);
boolean cdc_gsi__disable(void *cdc_ctx_ptr);
boolean cdc_gsi__suspend(void *cdc_ctx_ptr);
boolean cdc_gsi__resume(void *cdc_ctx_ptr);
void cdc_gsi__set_pipe_ep_info(void *dev_ptr, void *pipe_ptr, void *cdc_ctx_ptr);
void cdc_gsi__clear_pipe_ep_info(void *dev_ptr, void *pipe_ptr, void *cdc_ctx_ptr);

#endif /* _CDC_GSI__H_ */

