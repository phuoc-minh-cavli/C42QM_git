/*!
  @file lte_static_rrc_dispatcher.h

  @brief
  Header definitions for LTE RRC Dispatcher.

*/

/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/lte.mpss/4.7.2/api/lte_static_rrc_dispatcher.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/24/16   sc      Initial Revision

===========================================================================*/

#ifndef LTE_STATIC_RRC_DISPATCHER_H
#define LTE_STATIC_RRC_DISPATCHER_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <lte.h>
#include <msgr.h>
#include <msgr_lte.h>

#include <lte_cphy_msg.h>
#include <lte_mac_msg.h>
#include <lte_mac_ind.h>
#include <lte_mac_rrc.h>
#include <lte_rlc_msg.h>
#include <lte_rlc_rrc.h>
#include <lte_pdcp_msg.h>

#include <lte_rrc_ext_msg.h>
#include <lte_rrc_int_msg.h>
#include "nb1_cphy_msg.h"
#include "nb1_mac_msg.h"
#include "nb1_mac_ind.h"
#include "nb1_mac_rrc.h"
#include "nb1_rlc_msg.h"
#include "nb1_rlc_rrc.h"
#include "nb1_pdcp_msg.h"

#include "nb1_rrc_ext_msg.h"
#include "nb1_rrc_int_msg.h"

#ifdef FEATURE_LTE_TO_1X
#include "onex_cp_msg.h"
#endif
#ifdef FEATURE_LTE_TO_WCDMA
#include "wcdma_rrc_msg.h"
#endif
#ifdef FEATURE_LTE_TO_TDSCDMA
#include "tds_rrc_msg.h"
#endif
#include "geran_grr_msg.h"
#include "gnss_api.h"
#include "lte_fc.h"
#include "lte_static_as_msg.h"
#if defined (FEATURE_DYNAMIC_LOADING_LTE) || defined (FEATURE_EXTERNAL_PAGING)
#include "marb.h"
#endif


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Union of external message types
    @detail This is a dummy union only declared to get the max size of an ext msg
*/
typedef union
{
  /* Requests */
  lte_rrc_conn_est_req_s            lte_rrc_conn_est_req;            
  lte_rrc_ul_data_req_s             lte_rrc_ul_data_req;
  lte_rrc_irat_tunnel_ul_msg_req_s  lte_rrc_irat_tunnel_ul_msg_req;
  lte_irat_cdma_tunnel_ul_msg_req_s lte_irat_cdma_tunnel_ul_msg_req;
  lte_rrc_deactivate_req_s          lte_rrc_deactivate_req;
  lte_rrc_deactivate_cnf_s          lte_rrc_deactivate_cnf;
  lte_rrc_conn_abort_req_s          lte_rrc_conn_abort_req;
  lte_rrc_service_req_s             lte_rrc_service_req;
  lte_rrc_system_update_req_s       lte_rrc_system_update_req;
  lte_rrc_sim_update_req_s          lte_rrc_sim_update_req;
  lte_rrc_forbidden_ta_list_reset_req_s lte_rrc_forbidden_ta_list_reset_req;
  lte_rrc_drx_info_req_s            lte_rrc_drx_info_req;
  lte_rrc_edrx_cfg_req_s            lte_rrc_edrx_cfg_req;
  lte_rrc_plmn_search_req_s         lte_rrc_plmn_search_req;
  lte_rrc_plmn_search_abort_req_s   lte_rrc_plmn_search_abort_req;
  lte_rrc_G_resel_req_s             lte_rrc_G_resel_req;
  lte_rrc_G_abort_resel_req_s       lte_rrc_G_abort_resel_req;
  lte_rrc_G_redir_req_s             lte_rrc_G_redir_req;
  lte_rrc_G_abort_redir_req_s       lte_rrc_G_abort_redir_req;
  lte_rrc_wcdma_resel_req_s         lte_rrc_wcdma_resel_req;
  lte_rrc_wcdma_abort_resel_req_s   lte_rrc_wcdma_abort_resel_req;
  lte_rrc_wcdma_redir_req_s         lte_rrc_wcdma_redir_req;
  lte_rrc_wcdma_abort_redir_req_s   lte_rrc_wcdma_abort_redir_req;
  lte_rrc_wcdma_psho_req_s          lte_rrc_wcdma_psho_req;
  lte_rrc_wcdma_abort_psho_req_s    lte_rrc_wcdma_abort_psho_req;
  lte_rrc_eHRPD_resel_req_s         lte_rrc_eHRPD_resel_req;
  lte_rrc_eHRPD_abort_resel_req_s   lte_rrc_eHRPD_abort_resel_req;
  lte_rrc_1x_resel_req_s            lte_rrc_1x_resel_req;
  lte_rrc_1x_abort_resel_req_s      lte_rrc_1x_abort_resel_req;
  /* TO DO: Remove after WMS removes reference to the old UMID */
  lte_rrc_etws_msg_id_list_req_s    lte_rrc_etws_msg_id_list_req;
  lte_rrc_msg_id_list_req_s         lte_rrc_msg_id_list_req;

  lte_rrc_nw_sel_mode_reset_req_s   lte_rrc_nw_sel_mode_reset_req;
  lte_rrc_eutra_capabilities_req_s  lte_rrc_eutra_capabilities_freq;
  lte_irat_plmn_srch_req_s          lte_irat_plmn_srch_req;
  lte_irat_abort_plmn_srch_req_s    lte_irat_abort_plmn_srch_req;
  lte_irat_suspend_plmn_srch_ind_s  lte_irat_suspend_plmn_srch_req;
  lte_rrc_get_dedicated_pri_req_s   lte_rrc_get_dedicated_pri_req;
  lte_rrc_im3_backoff_applied_req_s	lte_rrc_im3_backoff_applied_req;

  lte_rrc_embms_act_tmgi_req_s      lte_rrc_embms_act_tmgi_req;
  lte_rrc_embms_deact_tmgi_req_s    lte_rrc_embms_deact_tmgi_req;
  lte_rrc_embms_act_deact_tmgi_req_s lte_rrc_embms_act_deact_tmgi_req;
  lte_rrc_embms_avail_tmgi_list_req_s lte_rrc_embms_avail_tmgi_list_req;
  lte_rrc_embms_act_tmgi_list_req_s lte_rrc_embms_act_tmgi_list_req;
  lte_rrc_embms_enable_req_s        lte_rrc_embms_enable_req;
  lte_rrc_embms_disable_req_s       lte_rrc_embms_disable_req;
  lte_tlb_rrc_cfg_req_s             lte_tlb_rrc_cfg_req_s;
  lte_rrc_band_pri_change_req_s     lte_rrc_band_pri_change_req;
  lte_rrc_get_band_pri_list_req_s   lte_rrc_get_band_pri_list_req;
  lte_rrc_cmapi_embms_cov_status_req_s lte_rrc_cmapi_embms_cov_status_req;
  lte_rrc_cmapi_embms_datamcs_req_s lte_rrc_cmapi_embms_datamcs_req;
  lte_rrc_cell_lock_req_s           lte_rrc_cell_lock_req;
  lte_rrc_get_serv_cell_sib_req_s   lte_rrc_get_serv_cell_sib_req;
  lte_rrc_utra_serving_cell_info_req_s     lte_rrc_utra_serving_cell_info_req;
  lte_rrc_mtc_cfg_req_s             lte_rrc_mtc_cfg_req;
  lte_rrc_irat_capability_req_s     lte_rrc_irat_capability_req;
  lte_rrc_wwan_set_priority_req_s   lte_rrc_wwan_set_priority_req;
  lte_rrc_psm_ready_req_s           lte_rrc_csp_psm_ready_req;
  lte_rrc_psm_enter_req_s           lte_rrc_csp_psm_enter_req;

  /* Responses */
  lte_rrc_act_rsp_s                 lte_rrc_act_rsp;
#ifdef FEATURE_LTE_TO_HDR
  hdrcp_msg_rsp_u                   lte_irat_to_do_redir_reject_rsp;
  hdrcp_msg_rsp_u                   lte_irat_to_do_resel_reject_rsp;
#endif
  geran_grr_lte_redir_failed_rsp_t  lte_irat_to_G_redir_reject_rsp;
  geran_grr_lte_resel_failed_rsp_t  lte_irat_to_G_resel_reject_rsp;
  geran_grr_lte_cco_rsp_t           lte_irat_to_G_cco_rsp;

 #ifdef FEATURE_LTE_TO_WCDMA
  wcdma_rrc_lte_resel_failed_rsp_type lte_irat_to_wcdma_resel_reject_rsp;
  wcdma_rrc_lte_abort_resel_rsp_type  lte_irat_to_wcdma_abort_resel_rsp;
  wcdma_rrc_lte_redir_failed_rsp_type lte_irat_to_wcdma_redir_reject_rsp;
  wcdma_rrc_lte_abort_redir_rsp_type  lte_irat_to_wcdma_abort_redir_rsp;
  wcdma_rrc_lte_utra_capabilities_rsp_type lte_irat_utra_capabilities_rsp;
#endif

#ifdef FEATURE_LTE_TO_1X
  onex_cp_lte_redir_failed_rsp_msg_t lte_irat_to_1x_redir_reject_rsp;
  onex_cp_lte_resel_failed_rsp_msg_t lte_irat_to_1x_resel_reject_rsp;
  onex_cp_lte_abort_redir_rsp_msg_t  lte_irat_to_1x_abort_redir_reject_rsp;
  onex_cp_lte_abort_resel_rsp_msg_t  lte_irat_to_1x_abort_resel_reject_rsp;
  onex_cp_lte_1xcsfb_call_rel_ind_t  lte_irat_to_1x_call_rel_ind;
  onex_cp_lte_1xcsfb_ho_fail_rsp_t   lte_irat_to_1x_ho_fail_rsp;
#endif 

  lte_irat_plmn_srch_rsp_s          lte_irat_plmn_srch_rsp;
  lte_irat_abort_plmn_srch_rsp_s    lte_irat_abort_plmn_srch_rsp;

  lte_cphy_ueinfo_rpt_rsp_s         lte_cphy_ueinfo_rpt_rsp;

  /* Indications */
  lte_cphy_mod_prd_boundary_ind_s   lte_cphy_mod_prd_boundary_ind;
  lte_mac_random_access_problem_ind_msg_s lte_mac_random_access_problem_ind_msg;
  lte_cphy_dl_weak_ind_s            lte_cphy_dl_weak_ind;
  lte_cphy_cell_resel_ind_s         lte_cphy_cell_resel_ind;
  lte_cphy_oos_ind_s                lte_cphy_oos_ind;
  lte_cphy_irat_asf_needed_ind_s    lte_cphy_irat_asf_needed_ind;
  lte_cphy_rl_failure_ind_s         lte_cphy_rl_failure_ind;
  lte_cphy_mcch_change_notification_ind_s lte_cphy_mcch_change_notification_ind;
  lte_rlcul_max_retx_ind_s          lte_rlcul_max_retx_ind;
  lte_l2_rlf_ind_msg_s              lte_l2_rlf_ind;
  lte_pdcpul_rlf_ind_msg_s          lte_pdcpul_rl_fail_ind;
  lte_rrc_handover_completed_ind_s  lte_rrc_handover_completed_ind;
  lte_rrc_service_ind_s             lte_rrc_service_ind;
  lte_rrc_conn_est_cnf_s            lte_rrc_conn_est_cnf;
  lte_rrc_ds_service_ind_s          lte_rrc_ds_service_ind;
  lte_rrc_clear_dedicated_pri_ind_s lte_rrc_clear_dedicated_pri_ind;
  lte_rrc_ue_mode_ind_s             lte_rrc_ue_mode_ind;
  lte_rrc_depri_requested_plmn_ind_s lte_rrc_depri_req_plmn_ind;
  lte_rrc_wcdma_resel_rsp_s         lte_rrc_wcdma_resel_rsp;
  lte_cphy_ifreq_otdoa_ind_s        lte_cphy_ifreq_otdoa_ind;
  lte_rrc_eab_update_ind_s      lte_rrc_eab_update_ind;
  cm_ims_emerg_cap_ind_s_type       cm_ims_emerg_cap_ind;
  lte_rrc_barring_update_ind_s      lte_rrc_barring_update_ind;
  lte_rrc_utra_serving_cell_info_rsp_s     lte_rrc_utra_serving_cell_info_rsp;
  #ifdef FEATURE_LTE_MDT
  lte_tlb_ctrl_update_loc_info_s    lte_tlb_ctrl_update_loc_info;
  #endif

  /* Confirmations */
  lte_cphy_start_cnf_s              lte_cphy_start_cnf;
  lte_cphy_stop_cnf_s               lte_cphy_stop_cnf;
  lte_cphy_cfg_cnf_s                lte_cphy_cfg_cnf;
  lte_cphy_handover_cnf_s           lte_cphy_handover_cnf;
  lte_cphy_abort_cnf_s              lte_cphy_abort_cnf;
  #ifdef FEATURE_FMD_SPEED_INFO
  lte_cphy_fmd_speed_info_cnf_s     lte_cphy_fmd_speed_info_cnf;
  nb1_cphy_fmd_speed_info_cnf_s        nb1_cphy_fmd_speed_info_cnf;  
  #endif /* FEATURE_FMD_SPEED_INFO */  
  lte_cphy_con_release_cnf_s        lte_cphy_con_release_cnf;
  lte_mac_cfg_cnf_msg_s             lte_mac_cfg_cnf_msg;
  lte_mac_access_cnf_msg_s          lte_mac_access_cnf_msg;    
  lte_mac_access_abort_cnf_msg_s    lte_mac_access_abort_cnf_msg;
  lte_mac_rach_rpt_cnf_msg_s        lte_mac_rach_rpt_cnf;
  lte_cphy_ueinfo_conn_failure_report_cnf_s lte_cphy_ueinfo_conn_failure_report_cnf;
  lte_rlc_cfg_cnf_s                 lte_rlc_cfg_cnf;
  lte_pdcpdl_cfg_cnf_msg_s          lte_pdcpdl_cfg_cnf_msg;
  lte_pdcpul_cfg_cnf_msg_s          lte_pdcpul_cfg_cnf_msg;
  lte_pdcpul_sdu_cnf_msg_s          lte_pdcpul_sdu_cnf_msg;
  lte_pdcpdl_recfg_prep_cnf_msg_s   lte_pdcpdl_recfg_prep_cnf_msg;
  lte_pdcpul_recfg_prep_cnf_msg_s   lte_pdcpul_recfg_prep_cnf_msg;
  lte_cphy_acq_cnf_s                lte_cphy_acq_cnf;
  lte_cphy_system_scan_cnf_s        lte_cphy_system_scan_cnf;
  lte_cphy_band_scan_cnf_s          lte_cphy_band_scan_cnf;
  lte_cphy_cell_select_cnf_s        lte_cphy_cell_select_cnf;
  lte_cphy_idle_meas_cfg_cnf_s      lte_cphy_idle_meas_cfg_cnf;
  lte_cphy_conn_meas_cfg_cnf_s      lte_cphy_conn_meas_cfg_cnf;
  lte_cphy_serv_cell_meas_cnf_s     lte_cphy_serv_cell_meas_cnf;
  lte_cphy_conn_meas_report_ind_s   lte_cphy_conn_meas_report_ind;
  lte_mac_release_resources_ind_s   lte_mac_release_resources_ind;
#ifdef FEATURE_LTE_TO_HDR
  hdrcp_msg_rsp_u                   lte_irat_to_do_redir_abort_cnf;
  hdrcp_msg_rsp_u                   lte_irat_to_do_resel_abort_cnf;
#endif
  geran_grr_msg_rsp_u               lte_irat_to_G_redir_abort_cnf;
  geran_grr_msg_rsp_u               lte_irat_to_G_resel_abort_cnf;
  lte_cphy_suspend_cnf_s            lte_cphy_suspend_cnf;
  lte_cphy_resume_cnf_s             lte_cphy_resume_cnf;
  lte_cphy_edrx_hyper_sfn_cnf_s     lte_cphy_edrx_hyper_sfn_cnf;
  lte_cphy_msg_bplmn_cell_cnf_s     lte_cphy_bplmn_cell_cnf;
  lte_cphy_msg_bplmn_complete_ind_s lte_cphy_bplmn_complete_ind;
  lte_cphy_msg_bplmn_stop_cnf_s     lte_cphy_bplmn_stop_cnf;
  lte_cphy_msg_bplmn_time_avail_ind_s lte_cphy_bplmn_time_avail_ind;
  lte_cphy_msg_bplmn_suspend_cnf_s     lte_cphy_bplmn_suspend_cnf;
  geran_grr_lte_cs_capabilities_rsp_t geran_grr_lte_cs_capabilities_rsp;
  geran_grr_lte_ps_capabilities_rsp_t geran_grr_lte_ps_capabilities_rsp;
  geran_grr_lte_dedicated_priorities_rsp_t geran_grr_lte_get_dedicated_pri_rsp;
  lte_mac_start_cnf_msg_s           lte_mac_start_cnf_msg;
  lte_mac_stop_cnf_msg_s            lte_mac_stop_cnf_msg;
  lte_pdcp_start_cnf_msg_s          lte_pdcp_start_cnf_msg;
  lte_pdcp_stop_cnf_msg_s           lte_pdcp_stop_cnf_msg;
  lte_rlc_start_cnf_s               lte_rlc_start_cnf;
  lte_rlc_stop_cnf_s                lte_rlc_stop_cnf;
  lte_fc_shutdown_ind_s             lte_fc_shutdown_ind;
  lte_pdcpul_conn_rel_ind_msg_s         lte_pdcpul_conn_rel_ind;
  lte_rrc_nmr_info_req_s            lte_rrc_nmr_info_req;
  lte_cphy_nmr_info_cnf_s           lte_cphy_nmr_info_cnf;
  lte_cphy_best_mbms_neigh_freq_cnf_s lte_cphy_best_mbms_neigh_freq_cnf;
  lte_cphy_release_trm_cnf_s        lte_cphy_release_trm_cnf;
  lte_common_msim_crat_feature_cnf_s  lte_rrc_common_msim_crat_feature_cnf;
  lte_cphy_harq_ack_tx_sts_cnf_s    lte_cphy_harq_ack_tx_sts_cnf;
  /* Supervisory */
  msgr_spr_loopback_struct_type     msgr_spr_loopback;
  lte_rrc_nas_lte_key_rsp_s			lte_rrc_nas_lte_key_rsp;
  lte_tlb_rrc_cfg_cnf_s             lte_tlb_rrc_cfg_cnf;
  lte_dsds_trm_priority_req_s       lte_rrc_dsds_trm_priority_req;
  lte_cphy_rf_available_ind_s       lte_cphy_rf_available_ind;
  lte_cphy_rf_unavailable_ind_s     lte_cphy_rf_unavailable_ind;
  lte_rrc_wwan_set_priority_cnf_s   lte_rrc_wwan_set_priority_cnf;

  /* NB1 Structs */
  /* Requests */
  //nb1_tlb_rrc_cfg_req_s             nb1_tlb_rrc_cfg_req_s;

  /* Responses */
  nb1_cphy_ueinfo_rpt_rsp_s         nb1_cphy_ueinfo_rpt_rsp;

  /* Indications */
  nb1_cphy_mod_prd_boundary_ind_s   nb1_cphy_mod_prd_boundary_ind;
  nb1_mac_random_access_problem_ind_msg_s nb1_mac_random_access_problem_ind_msg;
  nb1_cphy_dl_weak_ind_s            nb1_cphy_dl_weak_ind;
  nb1_cphy_cell_resel_ind_s         nb1_cphy_cell_resel_ind;
  nb1_cphy_oos_ind_s                nb1_cphy_oos_ind;
  nb1_cphy_irat_asf_needed_ind_s    nb1_cphy_irat_asf_needed_ind;
  nb1_cphy_rl_failure_ind_s         nb1_cphy_rl_failure_ind;
#ifdef FEATURE_NB1_TO_BE_REMOVED  
  nb1_cphy_mcch_change_notification_ind_s nb1_cphy_mcch_change_notification_ind;
#endif
  nb1_rlcul_max_retx_ind_s          nb1_rlcul_max_retx_ind;
  #if 0
  nb1_l2_rlf_ind_msg_s              nb1_l2_rlf_ind;
  #endif
  nb1_pdcpul_rlf_ind_msg_s          nb1_pdcpul_rl_fail_ind;
  nb1_rrc_handover_completed_ind_s  nb1_rrc_handover_completed_ind;
  nb1_cphy_ifreq_otdoa_ind_s        nb1_cphy_ifreq_otdoa_ind;

  /* Confirmations */
  nb1_cphy_start_cnf_s              nb1_cphy_start_cnf;
  nb1_cphy_stop_cnf_s               nb1_cphy_stop_cnf;
  nb1_cphy_common_cfg_cnf_s         nb1_cphy_common_cfg_cnf;
  nb1_cphy_dedicated_cfg_cnf_s      nb1_cphy_dedicated_cfg_cnf;
#ifdef FEATURE_NB1_TO_BE_REMOVED  
  nb1_cphy_handover_cnf_s           nb1_cphy_handover_cnf;
#endif
  nb1_cphy_abort_cnf_s              nb1_cphy_abort_cnf;
  nb1_cphy_con_release_cnf_s        nb1_cphy_con_release_cnf;
  nb1_mac_cfg_cnf_msg_s             nb1_mac_cfg_cnf_msg;
  nb1_mac_access_cnf_msg_s          nb1_mac_access_cnf_msg;
  nb1_mac_access_abort_cnf_msg_s    nb1_mac_access_abort_cnf_msg;
  nb1_mac_rach_rpt_cnf_msg_s        nb1_mac_rach_rpt_cnf;
  nb1_cphy_ueinfo_conn_failure_report_cnf_s nb1_cphy_ueinfo_conn_failure_report_cnf;
  nb1_rlc_cfg_cnf_s                 nb1_rlc_cfg_cnf;
  nb1_pdcpdl_cfg_cnf_msg_s          nb1_pdcpdl_cfg_cnf_msg;
  nb1_pdcpul_cfg_cnf_msg_s          nb1_pdcpul_cfg_cnf_msg;
  nb1_pdcpul_sdu_cnf_msg_s          nb1_pdcpul_sdu_cnf_msg;
  nb1_pdcpdl_recfg_prep_cnf_msg_s   nb1_pdcpdl_recfg_prep_cnf_msg;
  nb1_pdcpul_recfg_prep_cnf_msg_s   nb1_pdcpul_recfg_prep_cnf_msg;
  nb1_cphy_acq_cnf_s                nb1_cphy_acq_cnf;
  nb1_cphy_system_scan_cnf_s        nb1_cphy_system_scan_cnf;
  nb1_cphy_band_scan_cnf_s          nb1_cphy_band_scan_cnf;
  nb1_cphy_cell_select_cnf_s        nb1_cphy_cell_select_cnf;
  nb1_cphy_idle_meas_cfg_cnf_s      nb1_cphy_idle_meas_cfg_cnf;
  nb1_cphy_conn_meas_cfg_cnf_s      nb1_cphy_conn_meas_cfg_cnf;
  nb1_cphy_serv_cell_meas_cnf_s     nb1_cphy_serv_cell_meas_cnf;
  nb1_cphy_conn_meas_report_ind_s   nb1_cphy_conn_meas_report_ind;
  nb1_mac_release_resources_ind_s   nb1_mac_release_resources_ind;
  nb1_cphy_suspend_cnf_s            nb1_cphy_suspend_cnf;
  nb1_cphy_resume_cnf_s             nb1_cphy_resume_cnf;
  nb1_cphy_msg_bplmn_cell_cnf_s     nb1_cphy_bplmn_cell_cnf;
  nb1_cphy_msg_bplmn_complete_ind_s nb1_cphy_bplmn_complete_ind;
  nb1_cphy_msg_bplmn_stop_cnf_s     nb1_cphy_bplmn_stop_cnf;
  nb1_cphy_msg_bplmn_time_avail_ind_s nb1_cphy_bplmn_time_avail_ind;
  nb1_cphy_msg_bplmn_suspend_cnf_s     nb1_cphy_bplmn_suspend_cnf;
  nb1_mac_start_cnf_msg_s           nb1_mac_start_cnf_msg;
  nb1_mac_stop_cnf_msg_s            nb1_mac_stop_cnf_msg;
  nb1_pdcp_start_cnf_msg_s          nb1_pdcp_start_cnf_msg;
  nb1_pdcp_stop_cnf_msg_s           nb1_pdcp_stop_cnf_msg;
  nb1_rlc_start_cnf_s               nb1_rlc_start_cnf;
  nb1_rlc_stop_cnf_s                nb1_rlc_stop_cnf;
  nb1_pdcpul_conn_rel_ind_msg_s         nb1_pdcpul_conn_rel_ind;
  nb1_cphy_nmr_info_cnf_s           nb1_cphy_nmr_info_cnf;
  nb1_cphy_best_mbms_neigh_freq_cnf_s nb1_cphy_best_mbms_neigh_freq_cnf;
  nb1_mac_rrc_msg5_grant_ind_s      nb1_mac_rrc_msg5_grant_ind;
#ifdef FEATURE_NB1_TO_BE_REMOVED  
  nb1_cphy_release_trm_cnf_s        nb1_cphy_release_trm_cnf;
#endif
  nb1_cphy_harq_ack_tx_sts_cnf_s    nb1_cphy_harq_ack_tx_sts_cnf;
  /* Supervisory */
  //nb1_tlb_rrc_cfg_cnf_s             nb1_tlb_rrc_cfg_cnf;
#ifdef FEATURE_NB1_TO_BE_REMOVED  
  nb1_cphy_rf_available_ind_s                 nb1_cphy_rf_available_ind;
  nb1_cphy_rf_unavailable_ind_s                 nb1_cphy_rf_unavailable_ind;
#endif  

} lte_static_rrc_ext_msg_u;


/*! @brief Union of externalota message types
    @detail This is a dummy union only declared to get the max size of an extota msg
*/
typedef union
{
  /* Indications */
  lte_cphy_mib_ind_s                lte_cphy_mib_ind;
  lte_mac_rrc_dl_data_ind_s         lte_mac_rrc_dl_data_ind;
  lte_pdcpdl_sdu_ind_msg_s          lte_pdcpdl_sdu_ind_msg;
  lte_rlcdl_mcch_pdu_ind_s          lte_rlcdl_mcch_pdu_ind;

  /* NB1 Structs */
  /* Indications */
  nb1_cphy_mib_ind_s                nb1_cphy_mib_ind;
  nb1_mac_rrc_dl_data_ind_s         nb1_mac_rrc_dl_data_ind;
  nb1_pdcpdl_sdu_ind_msg_s          nb1_pdcpdl_sdu_ind_msg;
  nb1_rlcdl_mcch_pdu_ind_s          nb1_rlcdl_mcch_pdu_ind;

} lte_static_rrc_extota_msg_u;


/*! @brief Union of internal message types
    @detail This is a dummy union only declared to get the max size of an int msg
*/
typedef union
{
  /* Requests */
  lte_rrc_send_ul_msg_reqi_s lte_rrc_send_ulmsg_reqi;
  lte_rrc_mode_change_reqi_s lte_rrc_mode_change_reqi;
  lte_rrc_get_sibs_reqi_s lte_rrc_get_sibs_reqi;
  lte_rrc_sib_abort_reqi_s lte_rrc_sibs_abort_reqi;
  lte_rrc_cfg_reqi_s lte_rrc_cfg_reqi;
  lte_rrc_pended_cfg_reqi_s lte_rrc_pended_cfg_reqi;
  lte_rrc_meas_cfg_reqi_s lte_rrc_meas_cfg_reqi;
  lte_rrc_initiate_cell_sel_reqi_s lte_rrc_initiate_cell_sel_reqi;
  lte_rrc_irat_from_lte_redir_reqi_s  lte_rrc_irat_redir_reqi;
  lte_rrc_irat_from_lte_internal_redir_reqi_s  lte_rrc_irat_internal_redir_reqi;
  lte_rrc_irat_from_lte_abort_reqi_s  lte_rrc_irat_from_lte_abort_reqi;
  lte_rrc_irat_to_lte_resel_abort_reqi_s  lte_rrc_irat_to_lte_resel_abort_reqi;
  lte_rrc_irat_from_lte_resel_reqi_s  lte_rrc_irat_resel_reqi;
  lte_rrc_irat_to_lte_resel_reqi_s lte_rrc_irat_to_lte_resel_reqi;
  lte_rrc_irat_to_lte_psho_reqi_s  lte_rrc_irat_psho_reqi;
  lte_rrc_irat_to_lte_psho_abort_reqi_s lte_rrc_irat_to_lte_psho_abort_reqi;
  lte_rrc_irat_from_lte_to_do_capabilities_reqi_s lte_rrc_irat_do_capabilities_reqi;
  lte_rrc_irat_from_lte_to_w_capabilities_reqi_s lte_rrc_irat_w_capabilities_reqi;
  lte_rrc_irat_from_lte_to_tds_capabilities_reqi_s lte_rrc_irat_tds_capabilities_reqi;
  lte_rrc_irat_from_lte_to_g_cs_capabilities_reqi_s lte_rrc_irat_g_cs_capabilities_reqi;
  lte_rrc_irat_from_lte_to_g_ps_capabilities_reqi_s lte_rrc_irat_g_ps_capabilities_reqi;
  lte_rrc_csp_cell_select_abort_reqi_s lte_rrc_csp_cell_select_abort_reqi;
  lte_rrc_plmn_search_abort_reqi_s lte_rrc_plmn_search_abort_reqi;
  lte_rrc_irat_to_lte_redir_list_reqi_s lte_rrc_irat_to_lte_redir_list_reqi;
  lte_rrc_irat_to_lte_redir_full_reqi_s lte_rrc_irat_to_lte_redir_full_reqi;
  lte_rrc_irat_to_lte_redir_abort_reqi_s  lte_rrc_irat_to_lte_redir_abort_reqi;
  lte_irat_plmn_srch_req_s lte_irat_plmn_srch_reqi;
  lte_irat_abort_plmn_srch_req_s lte_irat_abort_plmn_srch_reqi;
  lte_irat_suspend_plmn_srch_ind_s lte_irat_suspend_plmn_srch_indi;
  lte_rrc_ml1_revoke_reqi_s lte_rrc_ml1_revoke_reqi;
  lte_rrc_irat_from_lte_to_g_cco_reqi_s lte_rrc_irat_from_lte_to_g_cco_reqi;
  lte_rrc_irat_from_lte_to_w_get_cgi_reqi_s lte_rrc_irat_from_lte_to_w_get_cgi_reqi;
  lte_rrc_irat_from_lte_to_w_abort_cgi_reqi_s lte_rrc_irat_from_lte_to_w_abort_cgi_reqi;
  lte_rrc_irat_from_lte_to_g_get_cgi_reqi_s lte_rrc_irat_from_lte_to_g_get_cgi_reqi;
  lte_rrc_irat_from_lte_to_g_abort_cgi_reqi_s lte_rrc_irat_from_lte_to_g_abort_cgi_reqi;
  lte_rrc_irat_from_lte_to_do_get_cgi_reqi_s lte_rrc_irat_from_lte_to_do_get_cgi_reqi;
  lte_rrc_irat_from_lte_to_do_abort_cgi_reqi_s lte_rrc_irat_from_lte_to_do_abort_cgi_reqi;
  lte_rrc_irat_from_lte_to_1x_get_cgi_reqi_s lte_rrc_irat_from_lte_to_1x_get_cgi_reqi;
  lte_rrc_esmgr_act_tmgi_reqi_s lte_rrc_esmgr_act_tmgi_reqi;
  lte_rrc_esmgr_deact_tmgi_reqi_s lte_rrc_esmgr_deact_tmgi_reqi;
  lte_rrc_esmgr_act_deact_tmgi_reqi_s lte_rrc_esmgr_act_deact_tmgi_reqi;
  lte_rrc_esmgr_avail_tmgi_list_reqi_s lte_rrc_esmgr_avail_tmgi_list_reqi;
  lte_rrc_start_internal_cell_selection_reqi_s lte_rrc_start_internal_cell_selection_reqi;

  /* Confirmations */
  lte_rrc_ul_msg_cnfi_s lte_rrc_ul_msg_cnfi;
  lte_rrc_mode_change_cnfi_s lte_rrc_mode_change_cnfi;
  lte_rrc_get_sibs_cnfi_s lte_rrc_get_sibs_cnfi;
  lte_rrc_cfg_cnfi_s lte_rrc_cfg_cnfi;
  lte_rrc_meas_cfg_cnfi_s lte_rrc_meas_cfg_cnfi;
  lte_rrc_irat_to_lte_resel_cnfi_s lte_rrc_irat_to_lte_resel_cnfi;
  lte_rrc_irat_to_lte_resel_abort_cnfi_s lte_rrc_irat_to_lte_resel_abort_cnfi;
  lte_rrc_irat_to_lte_psho_cnfi_s lte_rrc_irat_to_lte_psho_cnfi;
  lte_rrc_irat_to_lte_psho_abort_cnfi_s lte_rrc_irat_to_lte_psho_abort_cnfi;
  lte_rrc_irat_from_do_to_lte_capabilities_cnfi_s lte_rrc_irat_do_capabilities_cnfi;
  lte_rrc_irat_from_w_to_lte_capabilities_cnfi_s lte_rrc_irat_w_capabilities_cnfi;
  lte_rrc_irat_from_g_cs_to_lte_capabilities_cnfi_s lte_rrc_irat_g_cs_capabilities_cnfi;
  lte_rrc_irat_from_g_ps_to_lte_capabilities_cnfi_s lte_rrc_irat_g_ps_capabilities_cnfi;
  lte_rrc_csp_cell_select_abort_cnfi_s lte_rrc_cell_select_abort_cnfi;
  lte_rrc_irat_to_lte_redir_cnfi_s lte_rrc_irat_to_lte_redir_cnfi;
  lte_rrc_irat_to_lte_redir_abort_cnfi_s lte_rrc_irat_to_lte_redir_abort_cnfi;
  lte_irat_plmn_srch_cnfi_s lte_irat_plmn_srch_cnfi;
  lte_irat_abort_plmn_srch_rsp_s lte_irat_abort_plmn_srch_cnfi;
  lte_rrc_irat_from_lte_abort_cnfi_s  lte_rrc_irat_from_lte_abort_cnfi;
  lte_rrc_ml1_revoke_cnfi_s lte_rrc_ml1_revoke_cnfi;
  lte_rrc_irat_from_lte_to_w_get_cgi_cnfi_s lte_rrc_irat_from_lte_to_w_get_cgi_cnfi;
  lte_rrc_irat_from_lte_to_g_get_cgi_cnfi_s lte_rrc_irat_from_lte_to_g_get_cgi_cnfi;
  lte_rrc_irat_from_lte_to_g_abort_cgi_cnfi_s lte_rrc_irat_from_lte_to_g_abort_cgi_cnfi;
  lte_rrc_irat_from_lte_to_do_get_cgi_cnfi_s lte_rrc_irat_from_lte_to_do_get_cgi_cnfi;
  lte_rrc_irat_from_lte_to_1x_get_cgi_cnfi_s lte_rrc_irat_from_lte_to_1x_get_cgi_cnfi;
  lte_rrc_esmgr_act_tmgi_cnfi_s lte_rrc_esmgr_act_tmgi_cnfi;
  lte_rrc_esmgr_deact_tmgi_cnfi_s lte_rrc_esmgr_deact_tmgi_cnfi;
  lte_rrc_esmgr_act_deact_tmgi_cnfi_s lte_rrc_esmgr_act_deact_tmgi_cnfi;
#ifdef FEATURE_FEMTO_CSG
  lte_rrc_csg_asf_search_cnfi_s lte_rrc_csg_asf_search_cnfi;
  lte_rrc_csg_cfg_indi_s lte_rrc_csg_cfg_indi;
#endif
  lte_rrc_plmn_search_cnfi_s lte_rrc_plmn_search_cnfi;

  /* Indications */
  lte_rrc_dlm_processed_indi_s lte_rrc_dlm_processed_indi;
  lte_rrc_stopped_indi_s lte_rrc_stopped_indi;
  lte_rrc_unsolicited_sib_indi_s lte_rrc_unsolicited_sib_indi;
  lte_rrc_sib_updated_indi_s lte_rrc_sib_updated_indi;
  lte_rrc_sib_updated_indi_s lte_rrc_meas_pend_sib_updated_indi;
  lte_rrc_sib_read_error_indi_s lte_rrc_sib_read_error_indi;
  lte_rrc_conn_establishment_started_indi_s lte_rrc_conn_establishment_started_indi;
  lte_rrc_connected_indi_s lte_rrc_connected_indi;
  lte_rrc_conn_establishment_failure_indi_s lte_rrc_conn_establishment_failure_indi;
  lte_rrc_stop_cell_resel_indi_s lte_rrc_stop_cell_resel_indi;
  lte_rrc_proceed_with_resel_indi_s lte_rrc_proceed_with_resel_indi;
  lte_rrc_conn_rel_started_indi_s lte_rrc_conn_rel_started_indi;
  lte_rrc_conn_released_indi_s lte_rrc_conn_released_indi;
  lte_rrc_not_camped_indi_s lte_rrc_not_camped_indi;
  lte_rrc_camped_indi_s lte_rrc_camped_indi;
  lte_rrc_cell_resel_started_indi_s lte_rrc_cell_resel_started_indi;
  lte_rrc_cell_resel_canceled_indi_s lte_rrc_cell_resel_canceled_indi;
  lte_rrc_handover_started_indi_s lte_rrc_handover_started_indi;
  lte_rrc_cre_started_indi_s lte_rrc_cre_started_indi;
  lte_rrc_cre_completed_indi_s lte_rrc_cre_completed_indi;
  lte_rrc_conn_mode_failure_indi_s lte_rrc_conn_mode_failure_indi;
  lte_rrc_initiate_conn_rel_indi_s lte_rrc_initiate_conn_rel_indi;
  lte_rrc_serving_cell_changed_indi_s lte_rrc_serving_cell_changed_indi;
  lte_rrc_irat_from_lte_redir_failed_indi_s lte_rrc_irat_from_lte_redir_failed_indi;
  lte_rrc_irat_from_lte_resel_failed_indi_s lte_rrc_irat_from_lte_resel_failed_indi;
  lte_rrc_irat_from_lte_plmn_srch_resume_failed_indi_s lte_rrc_irat_from_lte_plmn_srch_resume_failed_indi;
  lte_rrc_ded_priority_list_indi_s lte_rrc_ded_priority_list_indi;
  lte_rrc_irat_to_lte_resel_completed_indi_s lte_rrc_irat_to_lte_resel_completed_indi;
  lte_rrc_irat_to_lte_psho_completed_indi_s lte_rrc_irat_to_lte_psho_completed_indi;
  lte_rrc_irat_to_lte_redir_completed_indi_s lte_rrc_irat_to_lte_redir_completed_indi;
  lte_rrc_irat_utra_resel_wait_time_indi_s lte_rrc_irat_utra_resel_wait_time_indi;
  lte_rrc_irat_from_lte_to_g_mobility_indi_s lte_rrc_irat_from_lte_to_g_mobility_indi;
  lte_rrc_irat_from_lte_to_w_mobility_indi_s lte_rrc_irat_from_lte_to_w_mobility_indi;
  lte_rrc_irat_from_lte_to_1x_mobility_indi_s lte_rrc_irat_from_lte_to_1x_mobility_indi;
  lte_rrc_irat_from_lte_mobility_started_indi_s lte_rrc_irat_from_lte_mobility_started_indi;
  lte_rrc_srb2_resumed_indi_s lte_rrc_srb2_resumed_indi;
  lte_rrc_irat_hrpd_pre_reg_info_indi_s lte_rrc_irat_hrpd_pre_reg_info_indi;
  lte_rrc_irat_to_lte_psho_started_indi_s lte_rrc_irat_to_lte_psho_started_indi;
  lte_rrc_irat_to_lte_psho_key_gen_indi_s lte_rrc_irat_to_lte_psho_key_gen_indi;
  lte_rrc_irat_lte_depri_indi_s lte_rrc_irat_lte_depri_indi;
  lte_rrc_esmgr_resel_prio_change_indi_s lte_rrc_esmgr_resel_prio_change_indi;
  lte_rrc_trm_priority_change_indi_s lte_rrc_trm_priority_change_indi;
  lte_rrc_irat_from_lte_resel_success_indi_s lte_rrc_irat_from_lte_resel_success_indi;
  lte_rrc_prox_cfg_indi_s lte_rrc_prox_cfg_indi;
  #ifdef FEATURE_LTE_MDT
  lte_rrc_loc_services_info_indi_s lte_rrc_loc_services_info_indi;
  #endif
  lte_rrc_sib1_avail_indi_s lte_rrc_sib1_avail_indi;
  lte_rrc_sec_active_indi_s lte_rrc_sec_active_indi;

  //CEP opt
  lte_rrc_oos_indi_s lte_rrc_oos_indi;
  lte_rrc_emp_deact_tmgi_indi_s lte_rrc_emp_deact_tmgi_indi;
  lte_rrc_emp_oos_warn_indi_s lte_rrc_emp_oos_warn_indi;
  lte_rrc_emp_pended_service_indi_s lte_rrc_emp_pended_service_indi;
  lte_rrc_esmgr_avail_tmgi_list_indi_s lte_rrc_esmgr_avail_tmgi_list_indi;
  lte_rrc_depri_freq_indi_s lte_rrc_depri_freq_indi;
  lte_rrc_clear_depri_freq_indi_s lte_rrc_clear_depri_freq_indi;

  /* DLMs */
  lte_rrc_mh_dlm_s lte_rrc_mh_dlm;

  /* Timers */
  lte_rrc_tmri_s lte_rrc_tmri;

  lte_rrc_irat_to_lte_psho_success_indi_s lte_rrc_irat_to_lte_psho_success_indi;

#ifdef FEATURE_LTE_ELS_ENABLED
  lte_rrc_auto_els_ue_cap_info_indi_s lte_rrc_auto_els_ue_cap_info_indi;
#endif


  /* NB1 Structs */
  /* Requests */
  nb1_rrc_send_ul_msg_reqi_s nb1_rrc_send_ulmsg_reqi;
  nb1_rrc_mode_change_reqi_s nb1_rrc_mode_change_reqi;
  nb1_rrc_get_sibs_reqi_s nb1_rrc_get_sibs_reqi;
  nb1_rrc_sib_abort_reqi_s nb1_rrc_sibs_abort_reqi;
  nb1_rrc_cfg_reqi_s nb1_rrc_cfg_reqi;
  nb1_rrc_pended_cfg_reqi_s nb1_rrc_pended_cfg_reqi;
  nb1_rrc_meas_cfg_reqi_s nb1_rrc_meas_cfg_reqi;
  nb1_rrc_initiate_cell_sel_reqi_s nb1_rrc_initiate_cell_sel_reqi;
  nb1_rrc_csp_cell_select_abort_reqi_s nb1_rrc_csp_cell_select_abort_reqi;
  nb1_rrc_start_internal_cell_selection_reqi_s nb1_rrc_start_internal_cell_selection_reqi;

  /* Confirmations */
  nb1_rrc_ul_msg_cnfi_s nb1_rrc_ul_msg_cnfi;
  nb1_rrc_mode_change_cnfi_s nb1_rrc_mode_change_cnfi;
  nb1_rrc_get_sibs_cnfi_s nb1_rrc_get_sibs_cnfi;
  nb1_rrc_cfg_cnfi_s nb1_rrc_cfg_cnfi;
  nb1_rrc_meas_cfg_cnfi_s nb1_rrc_meas_cfg_cnfi;
  nb1_rrc_csp_cell_select_abort_cnfi_s nb1_rrc_cell_select_abort_cnfi;

  /* Indications */
  nb1_rrc_dlm_processed_indi_s nb1_rrc_dlm_processed_indi;
  nb1_rrc_stopped_indi_s nb1_rrc_stopped_indi;
  nb1_rrc_unsolicited_sib_indi_s nb1_rrc_unsolicited_sib_indi;
  nb1_rrc_sib_updated_indi_s nb1_rrc_sib_updated_indi;
  nb1_rrc_sib_updated_indi_s nb1_rrc_meas_pend_sib_updated_indi;
  nb1_rrc_sib_read_error_indi_s nb1_rrc_sib_read_error_indi;
  nb1_rrc_conn_establishment_started_indi_s nb1_rrc_conn_establishment_started_indi;
  nb1_rrc_connected_indi_s nb1_rrc_connected_indi;
  nb1_rrc_conn_establishment_failure_indi_s nb1_rrc_conn_establishment_failure_indi;
  nb1_rrc_stop_cell_resel_indi_s nb1_rrc_stop_cell_resel_indi;
  nb1_rrc_proceed_with_resel_indi_s nb1_rrc_proceed_with_resel_indi;
  nb1_rrc_conn_rel_started_indi_s nb1_rrc_conn_rel_started_indi;
  nb1_rrc_conn_released_indi_s nb1_rrc_conn_released_indi;
  nb1_rrc_not_camped_indi_s nb1_rrc_not_camped_indi;
  nb1_rrc_camped_indi_s nb1_rrc_camped_indi;
  nb1_rrc_cell_resel_started_indi_s nb1_rrc_cell_resel_started_indi;
  nb1_rrc_cell_resel_canceled_indi_s nb1_rrc_cell_resel_canceled_indi;
  nb1_rrc_cre_started_indi_s nb1_rrc_cre_started_indi;
  nb1_rrc_cre_completed_indi_s nb1_rrc_cre_completed_indi;
  nb1_rrc_conn_mode_failure_indi_s nb1_rrc_conn_mode_failure_indi;
  nb1_rrc_initiate_conn_rel_indi_s nb1_rrc_initiate_conn_rel_indi;
  nb1_rrc_serving_cell_changed_indi_s nb1_rrc_serving_cell_changed_indi;
  nb1_rrc_rb_resumed_indi_s nb1_rrc_rb_resumed_indi;
  nb1_rrc_sib1_avail_indi_s nb1_rrc_sib1_avail_indi;

  //CEP opt
  nb1_rrc_oos_indi_s nb1_rrc_oos_indi;

  /* CEP RACH Opt */
  nb1_rrc_proceed_with_rach_opt_indi_s nb1_rrc_proceed_with_rach_opt_indi;

  /* DLMs */
  nb1_rrc_mh_dlm_s nb1_rrc_mh_dlm;

  /* Timers */
  nb1_rrc_tmri_s nb1_rrc_tmri;


#ifdef FEATURE_NB1_ELS_ENABLED
  nb1_rrc_auto_els_ue_cap_info_indi_s nb1_rrc_auto_els_ue_cap_info_indi;
#endif

} lte_static_rrc_int_msg_u;


/*! @brief Union of all msgs that RRC can receive
*/
typedef union
{
  msgr_hdr_s        msgr_hdr;

  lte_static_rrc_ext_msg_u ext_msg;
  lte_static_rrc_int_msg_u int_msg;
  lte_static_rrc_extota_msg_u extdlm_msg;
} lte_static_rrc_msg_u;


/*! @brief REQUIRED brief description of this structure typedef
 * */
typedef struct
{
  msgr_hdr_s msg_hdr;           /*!< Message router header */
  lte_static_rrc_msg_u pending_msg;    /*!< Payload of the pended msg */
  uint32 pending_msg_len;       /*!< Length of the pended msg */

} lte_static_rrc_pending_msg_indi_s;

typedef union
{
  lte_static_rrc_pending_msg_indi_s    pending_indi;
  lte_static_rrc_msg_u                 regular_msg;
} lte_static_rrc_dispatcher_dispatch_msg_u;


/*! @brief Used by the dispatcher to encapsulate messages sent to state machines
*/
typedef struct
{
  msgr_hdr_s *msg_ptr;  /*!< Ptr to the message buffer */
  uint32     msg_len;   /*!< Length of the message buffer */

} lte_static_rrc_dispatcher_message_s;



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern lte_errno_e lte_static_rrc_dispatcher_init
(
  void
);

extern lte_errno_e lte_static_rrc_dispatcher_deinit
(
  void
);

extern void* lte_static_rrc_dispatcher_get_msg_buf
(
  void *msg_ptr,        /*!< Msg payload received by stm handler */
  int *buf_len          /*!< <OUT> Length of the returned buffer */ 
);

extern lte_errno_e lte_static_rrc_dispatcher_send_synch_msg
(
  msgr_hdr_s *msg_ptr,      /*!< Ptr to the message buffer */
  uint32 msg_len            /*!< Length of the message buffer */
);

extern lte_errno_e lte_static_rrc_dispatcher_dispatch
(
  msgr_hdr_s *msg_ptr,      /*!< Ptr to the message buffer */
  uint32    msg_len         /*!< Length of the message buffer */
);

extern void lte_static_rrc_dispatcher_loop
(
  void
);

extern void lte_static_rrc_dispatcher_deactivate
(
 void
);

extern boolean lte_static_rrc_get_nb1_csp_common_param
(
  nb1_rrc_csp_shim_common_param_s *shim_common_db 
    /*!< NB1 CSP Common params stored at SHIM Layer */
);

extern boolean lte_static_rrc_get_nb1_band_scan_param
(
  nb1_rrc_csp_shim_band_scan_param_s *shim_band_scan_db 
   /*!< Band scan params stored at SHIM Layer */
);

extern boolean lte_static_rrc_get_nb1_system_scan_param
(
  nb1_rrc_csp_shim_system_scan_param_s *shim_system_scan_db
   /*!< System scan params stored at SHIM Layer */
);

extern boolean lte_static_rrc_get_nb1_plmn_search_param
(
  nb1_rrc_plmn_search_shim_param_s *shim_plmn_search_db
   /*!< PLMN Search params stored at SHIM Layer */
);

extern boolean lte_static_rrc_get_nb1_marb_unload_acq_info_param
(
  nb1_rrc_csp_marb_unload_acq_info_s *shim_marb_unload_acq_info
   /*!< Marb Unload Acq Info params stored at SHIM Layer */
);

extern boolean  lte_static_rrc_get_nb1_interfreq_list
(
  nb1_rrc_interfreq_list  *freq_list
);

extern boolean  lte_static_rrc_get_nb1_acq_db
(
  nb1_rrc_csp_efs_acq_db_v3_s  *efs_acq_db_v3
);

extern boolean  lte_static_rrc_get_lte_acq_db
(
  lte_rrc_csp_efs_acq_db_v2_s  *efs_acq_db_v2
);
extern lte_rrc_tmr_s* lte_static_rrc_get_m1_tmr
(
  lte_rrc_tmr_type_e timer_type
);

extern nb1_rrc_tmr_s* lte_static_rrc_get_nb1_tmr
(
  nb1_rrc_tmr_type_e timer_type
);

extern lte_rrc_sib_db_s* lte_static_rrc_get_m1_sib_db_in_shim
(
  void
);

extern nb1_rrc_sib_db_s* lte_static_rrc_get_nb1_sib_db_in_shim
(
  void
);
extern nb1_rrc_meas_redir_carrier_info_s* lte_static_rrc_get_nb1_meas_redir_info_in_shim
(
  void
);
extern void lte_static_rrc_set_as_pending_task_mask
(
  lte_as_task_mask_t pending_mask,
  lte_static_as_init_req_type_e req_type
);

extern void lte_static_rrc_set_lte_cap_mem_allocated(void);

extern void lte_static_rrc_reset_lte_cap_mem_allocated(void);

extern boolean lte_static_rrc_get_nb1_cell_lock_list(nb1_rrc_csp_cell_lock_s *cell_lock_info_ptr);

extern boolean lte_static_rrc_get_m1_cell_lock_list(lte_rrc_csp_cell_lock_s *cell_lock_info_ptr);

#ifdef FEATURE_LTE_RRC_MEM_DEBUG 
extern void lte_static_rrc_log_mem_db
(
  lte_active_rat_type_e act_rat_type
);
extern void lte_static_rrc_m1_mem_stats
(
  lte_rrc_osys_mem_alloc_type_e alloc_type,
  size_t size,
  void* ptr,
  void* ret_ptr
);
extern void lte_static_rrc_mem_stats
(
  lte_active_rat_type_e act_rat_type,
  lte_rrc_osys_mem_alloc_type_e alloc_type,
  size_t size,
  void* ptr,
  void* ret_ptr
);
#endif

#if defined (FEATURE_DYNAMIC_LOADING_LTE) || defined (FEATURE_EXTERNAL_PAGING)
extern boolean lte_static_rrc_is_deactivate_req_to_unload
(
  void
);

extern void lte_static_rrc_set_internal_deact_for_unload_flag
(
  boolean trigger_internal_deact
);

extern void lte_static_rrc_reset_deact_ongoing_flag
(
  void
);

extern boolean lte_rrc_static_ext_client_init_msgr
(
  void
);
boolean lte_rrc_static_ext_client_deinit_msgr
(
  void
);
stm_state_t lte_static_rrc_trigger_dmm_resume
(
  void
);
void lte_static_rrc_trigger_internal_service_req
(
  void
);
void lte_static_rrc_trigger_internal_plmn_search_req
(
  void
);

boolean lte_static_rrc_shim_handling_required_for_umid
(
  msgr_umid_type msg_id     /*!< UMID */
);
#endif

#ifdef FEATURE_FMD_SPEED_INFO
extern boolean lte_static_rrc_is_fmd_speed_info_req_guard_tmr_running(void);
extern void lte_static_rrc_send_fmd_speed_info_req_to_ml1(void);
extern boolean lte_static_rrc_is_fmd_speed_info_req_to_ml1_pending(void);
#endif /* FEATURE_FMD_SPEED_INFO */

/* This API Returns time difference between two timestamps provided in ms, 
   taking care of wrap-around cases */
extern uint64 lte_static_rrc_get_time_diff
(
  uint64 cur_time, 
  uint64 ref_time
);

void lte_static_rrc_m1_set_valid_saved_cfg_db_context(boolean saved_cf_db_context_flag);
void lte_static_rrc_m1_get_saved_cfg_db(lte_rrc_saved_cfg_db_type** lte_rrc_saved_cfg_db_type_ptr);
boolean lte_static_rrc_m1_has_valid_saved_cfg_db_context(void);

void lte_static_rrc_m1_set_resume_identity(lte_rrc_osys_ResumeIdentity_r13 *resume_id_ptr);
void lte_static_rrc_m1_get_resume_identity(lte_rrc_osys_ResumeIdentity_r13 *resume_id_ptr);
void lte_static_rrc_m1_get_truncated_resume_identity(uint32 *truncated_resume_id_ptr);
boolean lte_static_rrc_m1_has_valid_resume_id(void);

void lte_static_rrc_m1_save_sec_context
(
  lte_rrc_sec_as_security_context_s *as_sec_context_ptr,
  lte_rrc_sec_nas_security_context_s *nas_sec_context_ptr,
  uint8 as_sec_context_index,
  uint8 nas_sec_context_index,
  uint8 sec_a2_key_index_n
);

void lte_static_rrc_m1_get_saved_sec_index
(
  uint8 *as_sec_context_index,
  uint8 *nas_sec_context_index,
  uint8 *sec_a2_key_index_n
);

void lte_static_rrc_m1_get_saved_sec_context
(
  lte_rrc_sec_as_security_context_s *as_sec_context_ptr,
  lte_rrc_sec_nas_security_context_s *nas_sec_context_ptr
);
boolean lte_static_rrc_m1_has_valid_saved_sec_context(void);

lte_integrity_algo_e lte_static_rrc_m1_get_saved_integrity_algo(void);

uint8* lte_static_rrc_m1_get_saved_krrc_int_key(void);

void lte_static_rrc_m1_clear_saved_resume_context(void);

boolean lte_static_rrc_m1_reset_osys_ctxt_needed(void);

void lte_static_rrc_m1_reset_osys_ctxt_index_counter(void);
  
boolean lte_static_rrc_custom_spacing_feature_enable
(
  void
);

void lte_static_rrc_nb1_set_resume_id
(
  nb1_rrc_osys_ResumeIdentity_r13 *resume_id_ptr
);
void lte_static_rrc_nb1_get_resume_id
(
  nb1_rrc_osys_ResumeIdentity_r13 *resume_id_ptr
);
void lte_static_rrc_nb1_save_sec_context
(
  nb1_rrc_sec_as_security_context_s *as_sec_context_ptr,
  nb1_rrc_sec_nas_security_context_s *nas_sec_context_ptr,
  uint8 as_sec_context_index,
  uint8 nas_sec_context_index,
  uint8 sec_a2_key_index_n
);

void lte_static_rrc_nb1_clear_saved_resume_context( void );
boolean lte_static_rrc_nb1_has_valid_resume_id( void );
nb1_integrity_algo_e lte_static_rrc_nb1_get_saved_integrity_algo( void );
uint8* lte_static_rrc_nb1_get_saved_krrc_int_key( void );
void lte_static_rrc_nb1_get_saved_sec_context
(
  nb1_rrc_sec_as_security_context_s *as_sec_context_ptr,
  nb1_rrc_sec_nas_security_context_s *nas_sec_context_ptr
);
boolean lte_static_rrc_nb1_has_valid_saved_sec_context(void);
void lte_static_rrc_nb1_get_saved_sec_index
(
  uint8 *as_sec_context_index,
  uint8 *nas_sec_context_index,
  uint8 *sec_a2_key_index_n
);

void lte_static_rrc_nb1_get_saved_cfg_db
(
  nb1_rrc_saved_cfg_db_type** lte_rrc_saved_cfg_db_type_ptr  
);

boolean lte_static_rrc_nb1_has_valid_saved_cfg_db_context( void );


void lte_static_rrc_nb1_set_valid_saved_cfg_db_context
(
  boolean saved_cf_db_context_flag 
);

void lte_static_rrc_m1_update_rpt_status
(
  boolean rpt_present
);
void lte_static_rrc_m1_update_rpt_mask
(
lte_cphy_ueinfo_rpt_mask_e rpt_mask
);

lte_cphy_ueinfo_rpt_mask_e lte_static_rrc_m1_get_rpt_mask();

boolean lte_static_rrc_m1_get_report_status();

void lte_static_rrc_m1_update_rlfinfo_rplmn
(
  lte_rrc_plmn_s rplmn
);

void lte_static_rrc_m1_get_rlfinfo_rplmn
(
  lte_rrc_plmn_s* temp_plmn
);

#ifdef FEATURE_LTE_MDT

void lte_static_rrc_m1_update_mdt_log_meas_available_rpt_status
(
  boolean rpt_available
);

boolean lte_static_rrc_m1_get_mdt_log_meas_available_rpt_status();

void lte_static_rrc_m1_update_mdt_rplmn
(
  lte_rrc_plmn_s rplmn
);

void lte_static_rrc_m1_get_mdt_rplmn
(
  lte_rrc_plmn_s* temp_plmn
);

void lte_static_rrc_m1_update_mdt_selected_plmn
(
  lte_rrc_plmn_s selected_plmn
);


void lte_static_rrc_m1_get_mdt_selected_plmn
(
  lte_rrc_plmn_s* temp_plmn
);
#endif
void lte_static_rrc_m1_update_failed_pcell_info_selected_plmn
(
  lte_rrc_plmn_s failed_plmn
);

void lte_static_rrc_m1_get_failed_pcell_info_selected_plmn();

void lte_static_rrc_m1_update_valid_tmr_tick
(
 uint8 time_tick
);
uint8 lte_static_rrc_m1_get_valid_tmr_tick();
void lte_static_rrc_m1_update_cphy_rlf_rpt
(
  lte_cphy_ueinfo_rlf_rpt_s rlf_rpt
);

lte_cphy_ueinfo_rlf_rpt_s lte_static_rrc_m1_get_cphy_rlf_rpt();

uint64 lte_static_get_ueinfo_validity_period
(
  void
);

void lte_static_rrc_process_rlf_info_validity_tmr_expiry
(
  void
);

void lte_static_rrc_nb1_set_anchor_nonanchor_band_mismatch
(
  boolean anchor_nonanchor_band_mismatch
);

boolean lte_static_rrc_nb1_get_anchor_nonanchor_band_mismatch(void);

boolean lte_static_rrc_psm_ready_req_cb(void);

void lte_static_rrc_send_psm_ready_rsp
(
  sys_psm_status_e_type     status,
  uint32                    rem_time_in_sec,
  uint64                    rem_time_in_msec,
  boolean                   is_next_full_image  
);

void lte_static_rrc_send_psm_ready_ind
(
  uint32  psm_duration_in_sec,
  uint64  psm_duration_in_msec,
  boolean is_first_psm
);

boolean lte_static_rrc_psm_enter_req_cb(void);

void lte_static_rrc_send_enter_psm_rsp
(
  boolean status,
  boolean context_present,
  uint32  context_size,
  uint8   *context
);

void lte_static_rrc_send_wwan_high_priority_req
(
  boolean high_priority_req
);

void lte_static_rrc_init_active_sys
(
  sys_sys_mode_e_type acq_sys_mode 
);

#endif /* LTE_STATIC_RRC_DISPATCHER_H */

