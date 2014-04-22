/*!
  @file lte_static_rrc_ext_api.h

  @brief
  Pending queue implementation for RRC dispatcher.

  @detail
  - This code uses the lockless circular queue implementation in lte. Since
    lockless circular queues are not a good match for the needs of the pending
    queue, the implementation is not optimized for speed. Depending on need,
    we will have to re-visit this design at a later point. The best implementation
    might be to use qube style queues.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/lte.mpss/4.7.2/api/lte_static_rrc_ext_api.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/04/16   sc      Initial Revision

===========================================================================*/

#ifndef LTE_STATIC_RRC_EXT_API_H
#define LTE_STATIC_RRC_EXT_API_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#if defined (FEATURE_DYNAMIC_LOADING_LTE) || defined (FEATURE_EXTERNAL_PAGING)
#include "dmm.h"
#endif

#include <lte_rrc_ext_api.h>
#include <lte_static_rrc_pendq.h>
#include <lte_static_rrc_dispatcher.h>
#include <lte_rrc_int_msg.h>
#include <nb1_rrc_int_msg.h>

extern const char* M1_GUID;

extern const char* NB1_GUID;


typedef boolean (*dispatcher_process_msg_t)
(
  lte_static_rrc_dispatcher_message_s *
);

typedef boolean (*dispatcher_process_pending_msg_t)
(
  lte_static_rrc_pendq_queue_item_s *
);

typedef boolean (*get_camped_cell_info_t)
(
  lte_rrc_plmn_s *,
  lte_earfcn_t *,
  lte_phy_cell_id_t *,
  lte_rrc_global_cell_id_t *,
  uint16 *
);


typedef boolean (*get_camped_cell_info_with_primary_plmn_t)
(
  lte_rrc_plmn_s *,
  lte_earfcn_t *,
  lte_phy_cell_id_t *,
  lte_rrc_global_cell_id_t *,
  uint16 *
);

typedef boolean (*earfcn_is_supported_t)
(
  lte_earfcn_t,
  sys_lte_band_mask_e_type
);

typedef lte_rrc_dedicated_pri_owner_e (*dedicated_pri_get_owner_t)
(
  void
);

typedef boolean (*dedicated_pri_set_owner_t)
(
  lte_rrc_dedicated_pri_owner_e owner
);

typedef boolean (*cell_is_tdd_t)
(
  lte_earfcn_t dl_earfcn
);

typedef boolean (*lte_fdd_is_supported_t)
(
  sys_lte_band_mask_e_type lte_band_preference
);

typedef boolean (*lte_tdd_is_supported_t)
(
  sys_lte_band_mask_e_type lte_band_preference
);

typedef boolean (*get_tdd_prune_required_t)
(
  void
);

typedef boolean (*irat_from_w_set_redir_list_tmr_t)
(
  uint64
);

typedef boolean (*irat_from_g_set_redir_list_tmr_t)
(
  uint64,
  boolean
);

typedef lte_3gpp_release_ver_e (*get_3gpp_release_version_t)
(
  void
);

typedef mcfg_fs_status_e_type (*delete_acq_db_t)
(
  void
);

typedef int (*get_decoded_sib_string_t)
(
  void *,
  uint32,
  void *,
  uint32
);

typedef boolean (*cmapi_fgi_change_req_t)
(
  lte_fgi_rel_type_e,
  uint32,
  lte_rrc_div_duplex_e
);

typedef boolean (*cmapi_efs_reread_req_t)
(
  uint64
);

typedef boolean (*cmapi_set_pci_lock_req_t)
(
  lte_earfcn_t,
  uint16
);

typedef boolean (*cmapi_set_earfcn_lock_req_t)
(
  lte_earfcn_t,
  lte_earfcn_t
);

typedef uint8 (*cmapi_get_embms_coverage_status_t)
(
  void
);

typedef void (*cmapi_get_pmch_datamcs_info_t)
(
  lte_rrc_datamcs_info_s *
);

typedef void (*cmapi_get_mbsfn_snr_t)
(
  lte_rrc_mbsfn_snr_info_s *
);

typedef sys_sband_lte_e_type (*get_band_from_dl_earfcn_t)
(
  lte_earfcn_t,
  boolean *
);

typedef uint8 (*csg_get_fp_lcsg_info_for_w_t)
(
 const source_cell_info_type *,
 source_cell_info_type *
);

typedef sys_sband_lte_e_type (*band_from_int_to_enum_t)
(
  uint16
);

typedef errno_enum_type (*convert_dl_earfcn_by_band_t)
(
  lte_earfcn_t *,
  sys_sband_lte_e_type,
  sys_sband_lte_e_type
);

typedef uint16 (*get_freq_from_earfcn_t)
(
  lte_earfcn_t
);

typedef boolean (*cap_mfbi_is_supported_t)
(
  boolean
);

typedef lte_rrc_state_ext_e (*get_rrc_state_ext_t)
(
  void
);

typedef boolean (*cap_inter_freq_rstd_meas_supported_t)
(
  void
);

typedef boolean (*nv_read_t)
(
  nv_items_enum_type,
  nv_item_type *
);

typedef uint8 (*cap_inter_freq_gap_less_meas_enabled_t)
(
  sys_sband_lte_e_type,
  sys_sband_lte_e_type
);

typedef lte_rrc_csp_efs_acq_db_v2_s* (*lte_csp_get_efs_acq_db_t)
(
  void
);

typedef nb1_rrc_csp_efs_acq_db_v3_s* (*nb1_csp_get_efs_acq_db_t)
(
  void
);

typedef boolean (*rrc_csp_is_psm_succeed_t)
(
  void
);

typedef uint32 (*rrc_csp_get_psm_threshhold_t)
(
  void
);

typedef boolean (*cap_multi_drb_is_supported_t)
(
  void
);

typedef nb1_rrc_csp_static_data_s * (*nb1_rrc_csp_get_csp_static_data_t)
(
  void
);

typedef sys_stop_mode_reason_e_type (*lte_rrc_csp_get_deactivate_reason_t)
(
 void
);

typedef sys_stop_mode_reason_e_type (*nb1_rrc_csp_get_deactivate_reason_t)
(
 void
);
typedef lte_rrc_sib_db_s* (*sib_get_sib_db_t)
(
  void
);

typedef void (*sib_clear_sib_db_t)
(
  uint8 ,  /*!< Index of cell in sib_db */
  lte_rrc_sib_cell_s *
);

typedef void (*sib_copy_sib_db_t)
(
  uint8 ,  /*!< Index of cell in sib_db */
  lte_rrc_sib_cell_s *
);

typedef uint8 (*nb1_rrc_get_cell_info_age_efs_t)
(
  void
);

typedef void (*nb1_sib_clear_sib_db_t)
(
  uint8 ,  /*!< Index of cell in sib_db */
  nb1_rrc_sib_cell_s *
);

typedef boolean (*rrc_cre_rlf_in_progress_t)
(
  void
);

typedef boolean (*rrc_csp_get_camped_cell_raster_info_t)
(
  sys_nb1_raster_offset_e_type* nb1_raster_offset_ptr
);

typedef boolean (*rrc_csp_reselection_is_in_progress_t)
(
  void
);

typedef struct
{
  dispatcher_process_msg_t                 rrc_dispatcher_process_msg;
  dispatcher_process_pending_msg_t         rrc_dispatcher_process_pending_msg;
  get_camped_cell_info_t                   rrc_get_camped_cell_info;
  get_camped_cell_info_with_primary_plmn_t rrc_get_camped_cell_info_with_primary_plmn;
  earfcn_is_supported_t                    rrc_earfcn_is_supported;
  dedicated_pri_get_owner_t                rrc_dedicated_pri_get_owner;
  dedicated_pri_set_owner_t                rrc_dedicated_pri_set_owner;
  cell_is_tdd_t                            rrc_cell_is_tdd;
  lte_fdd_is_supported_t                   rrc_lte_fdd_is_supported;
  lte_tdd_is_supported_t                   rrc_lte_tdd_is_supported;
  get_tdd_prune_required_t                 rrc_get_tdd_prune_required;
  irat_from_w_set_redir_list_tmr_t         rrc_irat_from_w_set_redir_list_tmr;
  irat_from_g_set_redir_list_tmr_t         rrc_irat_from_g_set_redir_list_tmr;
  get_3gpp_release_version_t               rrc_get_3gpp_release_version;
  delete_acq_db_t                          rrc_delete_acq_db;
  get_decoded_sib_string_t                 rrc_get_decoded_sib_string;
  cmapi_fgi_change_req_t                   rrc_cmapi_fgi_change_req;
  cmapi_efs_reread_req_t                   rrc_cmapi_efs_reread_req;
  cmapi_set_pci_lock_req_t                 rrc_cmapi_set_pci_lock_req;
  cmapi_set_earfcn_lock_req_t              rrc_cmapi_set_earfcn_lock_req;
  cmapi_get_embms_coverage_status_t        rrc_cmapi_get_embms_coverage_status;
  cmapi_get_pmch_datamcs_info_t            rrc_cmapi_get_pmch_datamcs_info;
  cmapi_get_mbsfn_snr_t                    rrc_cmapi_get_mbsfn_snr;
  get_band_from_dl_earfcn_t                rrc_get_band_from_dl_earfcn;
  csg_get_fp_lcsg_info_for_w_t             rrc_csg_get_fp_lcsg_info_for_w;
  band_from_int_to_enum_t                  rrc_band_from_int_to_enum;
  convert_dl_earfcn_by_band_t              rrc_convert_dl_earfcn_by_band;
  get_freq_from_earfcn_t                   rrc_get_freq_from_earfcn;
  cap_mfbi_is_supported_t                  rrc_cap_mfbi_is_supported;
  cap_inter_freq_rstd_meas_supported_t     rrc_cap_inter_freq_rstd_meas_supported;
  nv_read_t                                rrc_nv_read;
  cap_inter_freq_gap_less_meas_enabled_t   rrc_cap_inter_freq_gap_less_meas_enabled;
  lte_csp_get_efs_acq_db_t                 lte_rrc_csp_get_efs_acq_db;
  nb1_csp_get_efs_acq_db_t                 nb1_rrc_csp_get_efs_acq_db;
  rrc_csp_is_psm_succeed_t                 rrc_csp_is_psm_succeed;
  rrc_csp_get_psm_threshhold_t             rrc_csp_get_psm_threshhold;
  cap_multi_drb_is_supported_t             rrc_cap_multi_drb_is_supported;
  get_rrc_state_ext_t                      rrc_get_rrc_state_ext;  
  nb1_rrc_csp_get_csp_static_data_t        nb1_rrc_csp_get_csp_static_data;
  lte_rrc_csp_get_deactivate_reason_t      lte_rrc_csp_get_deactivate_reason;
  nb1_rrc_csp_get_deactivate_reason_t      nb1_rrc_csp_get_deactivate_reason;
  nb1_rrc_get_cell_info_age_efs_t          rrc_get_nb1_cell_info_age_efs;
  rrc_cre_rlf_in_progress_t                rrc_cre_rlf_in_progress;
  rrc_csp_get_camped_cell_raster_info_t    rrc_csp_get_camped_cell_raster_info;
  rrc_csp_reselection_is_in_progress_t     rrc_csp_reselection_is_in_progress;
} lte_static_rrc_ext_api_s;

#if !defined(FEATURE_DYNAMIC_LOADING_LTE) && !defined(FEATURE_EXTERNAL_PAGING)
extern lte_static_rrc_ext_api_s *lte_rrc_get_lte_m1_vtbl
(
  void
);
#endif
extern void lte_m1_rrc_init();
extern void lte_m1_rrc_deinit();
extern void lte_m1_rrc_suspend();
extern void lte_m1_rrc_resume();
extern void lte_nb1_rrc_init();
extern void lte_nb1_rrc_deinit();
extern void lte_nb1_rrc_suspend();
extern void lte_nb1_rrc_resume();

boolean lte_static_rrc_custom_spacing_feature_enable(void);

#endif /* LTE_STATIC_RRC_EXT_API_H */

