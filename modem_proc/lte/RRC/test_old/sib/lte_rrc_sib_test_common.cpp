/*!
  @file
  lte_rrc_sib_test_common.cpp

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_test_common.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "__lte_rrc_sib.h"
#include "__lte_rrc_controller.h"
#include "lte_rrc_sibi.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_pdcp_msg.h"
#include "dsm_item.h"
#include "dsm_init.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_mac_ind.h"
#include "lte_mac_msg.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>
#include "lte_rrc_sib_test_common.h"



/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/





/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

extern "C"
{
  extern lte_rrc_sib_s lte_rrc_sib;
}

boolean lte_rrc_sib_test_camped;
int lte_rrc_sib_test_sib_db_key;
lte_phy_cell_id_t lte_rrc_sib_test_serv_cell_phy_cell_id;
lte_earfcn_t lte_rrc_sib_test_serv_cell_freq;
stm_state_t lte_rrc_sib_test_controller_state;
boolean lte_rrc_sib_test_wcdma_supported;
boolean lte_rrc_sib_test_tdscdma_supported;
boolean lte_rrc_sib_test_gsm_supported;
boolean lte_rrc_sib_test_cdma_1x_supported;
boolean lte_rrc_sib_test_cdma_do_supported;
boolean lte_rrc_sib_test_lte_band_is_supported;
boolean lte_rrc_sib_test_ho_in_progress;
boolean lte_rrc_sib_rel_9;
lte_rrc_revoke_priority_e lte_rrc_sib_test_revoke_priority;
boolean lte_rrc_sib_test_embms_is_enabled;
boolean lte_rrc_sib_test_embms_is_preferred;
lte_rrc_rf_status_e lte_rrc_sib_test_rf_status;
uint64 lte_rrc_sib_test_rf_status_time;
lte_l1_sfn_t lte_rrc_sib_test_rf_status_sfn;
uint64 lte_rrc_sib_test_wt_tmr_before_rf_unavailable;
uint64 lte_rrc_sib_test_wt_tmr_after_rf_available;
uint8 lte_rrc_sib_num_po_mtr_from_rf_unavail;
uint8 lte_rrc_sib_num_po_missed_from_rf_avail;
boolean lte_rrc_sib_test_auto_gap_is_active;

DECLARE_SIB1_2345_MCC_8_5;
DECLARE_SIB2_SIB3_8_5;
DECLARE_SIB4_SIB5_8_5;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern "C"
{
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);
  TF_PROTO_STUB(stm_state_t, lte_rrc_controller_get_state, void);
  //TF_PROTO_STUB(boolean, lte_rrc_csp_wcdma_is_supported, void);
  //TF_PROTO_STUB(boolean, lte_rrc_csp_tdscdma_is_supported, void);
  //TF_PROTO_STUB(boolean, lte_rrc_csp_gsm_is_supported, void);
 // TF_PROTO_STUB(boolean, lte_rrc_csp_cdma_1x_is_supported, void);
  //TF_PROTO_STUB(boolean, lte_rrc_csp_cdma_do_is_supported, void);
  TF_PROTO_STUB(boolean, lte_rrc_csp_lte_band_is_supported, sys_sband_lte_e_type);
  TF_PROTO_STUB(boolean, lte_rrc_config_ho_in_progress, void);
  TF_PROTO_STUB(lte_rrc_revoke_priority_e, lte_rrc_get_revoke_priority, lte_rrc_revoke_trigger_e);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
  TF_PROTO_STUB(boolean, lte_rrc_esmgr_embms_is_enabled, void);
  TF_PROTO_STUB(boolean, lte_rrc_esmgr_embms_is_preferred, void);
  TF_PROTO_STUB(boolean, lte_rrc_llc_is_mib_changed_after_ho, lte_l1_mib_info_param_s *);
 // TF_PROTO_STUB(void, lte_rrc_controller_get_cphy_rf_status_info, lte_rrc_rf_status_info_s *);
  TF_PROTO_STUB(boolean, lte_rrc_meas_auto_gap_is_in_progress, void);
}


/*===========================================================================

                                FUNCTIONS

===========================================================================*/

lte_errno_e lte_rrc_sib_test_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr /*!< Pointer to cell info */
)
{
  /* Just hard code to something for now*/
  //cell_info_ptr->cell_id.freq = 10689;
  //cell_info_ptr->cell_id.phy_cell_id = 511;
  if(lte_rrc_sib_test_camped == FALSE)
  {
    return E_FAILURE;
  }
  else
  {
    cell_info_ptr->sib_db_key = lte_rrc_sib_test_sib_db_key;
    cell_info_ptr->cell_id.phy_cell_id = lte_rrc_sib_test_serv_cell_phy_cell_id;
    cell_info_ptr->cell_id.freq = lte_rrc_sib_test_serv_cell_freq;
    return E_SUCCESS;
  }
}

stm_state_t lte_rrc_sib_test_controller_get_state()
{
  return lte_rrc_sib_test_controller_state;
}

boolean lte_rrc_sib_test_csp_wcdma_is_supported()
{
  return lte_rrc_sib_test_wcdma_supported;
}

boolean lte_rrc_sib_test_csp_tdscdma_is_supported()
{
  return lte_rrc_sib_test_tdscdma_supported;
}

boolean lte_rrc_sib_test_csp_gsm_is_supported()
{
  return lte_rrc_sib_test_gsm_supported;
}

boolean lte_rrc_sib_test_csp_cdma_1x_is_supported()
{
  return lte_rrc_sib_test_cdma_1x_supported;
}

boolean lte_rrc_sib_test_csp_cdma_do_is_supported()
{
  return lte_rrc_sib_test_cdma_do_supported;
}

boolean lte_rrc_sib_test_csp_lte_band_is_supported(sys_sband_lte_e_type band)
{
  return lte_rrc_sib_test_lte_band_is_supported;
}

boolean lte_rrc_sib_test_config_ho_in_progress()
{
  return lte_rrc_sib_test_ho_in_progress;
}

lte_rrc_revoke_priority_e lte_rrc_sib_test_get_revoke_priority(lte_rrc_revoke_trigger_e revoke_trigger)
{
  return lte_rrc_sib_test_revoke_priority;
}

lte_3gpp_release_ver_e lte_rrc_sib_test_get_3gpp_release_version_for_rel9(void)
{
  return LTE_3GPP_REL9_JUN10;
}

boolean lte_rrc_sib_test_esmgr_embms_is_enabled(void)
{
  return lte_rrc_sib_test_embms_is_enabled;
}

boolean lte_rrc_sib_test_esmgr_embms_is_preferred(void)
{
return lte_rrc_sib_test_embms_is_preferred;
}


boolean lte_rrc_sib_test_llc_is_mib_changed_after_ho(lte_l1_mib_info_param_s *rcvd_mib)
{
  return FALSE;
}

uint16 lte_rrc_sib_test_paging_get_ue_specific_paging_cycle(void)
{
  return 10;
}

void lte_rrc_sib_test_controller_get_cphy_rf_status_info
(
   lte_rrc_rf_status_info_s *rf_status_info_ptr
)
{
    rf_status_info_ptr->rf_status = lte_rrc_sib_test_rf_status;
    rf_status_info_ptr->ref_time.curr_rtc = lte_rrc_sib_test_rf_status_time;
    rf_status_info_ptr->ref_time.curr_sfn = lte_rrc_sib_test_rf_status_sfn;
    return;
}

boolean lte_rrc_sib_test_meas_auto_gap_is_in_progress
(
  void
)
{
  return lte_rrc_sib_test_auto_gap_is_active;
}

void lte_rrc_sib_test_stub_init()
{
  TF_STUB(lte_rrc_csp_get_serving_cell_params) = lte_rrc_sib_test_get_serving_cell_params;
  TF_STUB(lte_rrc_controller_get_state) = lte_rrc_sib_test_controller_get_state;
  //TF_STUB(lte_rrc_csp_wcdma_is_supported) = lte_rrc_sib_test_csp_wcdma_is_supported;
  //TF_STUB(lte_rrc_csp_tdscdma_is_supported) = lte_rrc_sib_test_csp_tdscdma_is_supported;
 // TF_STUB(lte_rrc_csp_gsm_is_supported) = lte_rrc_sib_test_csp_gsm_is_supported;
  //TF_STUB(lte_rrc_csp_cdma_1x_is_supported) = lte_rrc_sib_test_csp_cdma_1x_is_supported;
  //TF_STUB(lte_rrc_csp_cdma_do_is_supported) = lte_rrc_sib_test_csp_cdma_do_is_supported;
  TF_STUB(lte_rrc_csp_lte_band_is_supported) = lte_rrc_sib_test_csp_lte_band_is_supported;
  TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_sib_test_config_ho_in_progress;
  TF_STUB(lte_rrc_get_revoke_priority) = lte_rrc_sib_test_get_revoke_priority;
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_sib_test_get_3gpp_release_version_for_rel9;
  TF_STUB(lte_rrc_esmgr_embms_is_enabled) = lte_rrc_sib_test_esmgr_embms_is_enabled;
  TF_STUB(lte_rrc_esmgr_embms_is_preferred) = lte_rrc_sib_test_esmgr_embms_is_preferred;
  TF_STUB(lte_rrc_llc_is_mib_changed_after_ho) = lte_rrc_sib_test_llc_is_mib_changed_after_ho;
  //TF_STUB(lte_rrc_controller_get_cphy_rf_status_info) = lte_rrc_sib_test_controller_get_cphy_rf_status_info;
  TF_STUB(lte_rrc_meas_auto_gap_is_in_progress) = lte_rrc_sib_test_meas_auto_gap_is_in_progress;

  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_NOT_CAMPED;
  lte_rrc_sib_test_wcdma_supported = TRUE;
  lte_rrc_sib_test_gsm_supported = TRUE;
  lte_rrc_sib_test_cdma_1x_supported = TRUE;
  lte_rrc_sib_test_cdma_do_supported = TRUE;
  lte_rrc_sib_test_lte_band_is_supported = TRUE;
  lte_rrc_sib_test_ho_in_progress = FALSE;
  lte_rrc_sib_test_embms_is_enabled = TRUE;
  lte_rrc_sib_test_embms_is_preferred = TRUE;

  lte_rrc_sib_test_rf_status = LTE_RRC_RF_STATUS_NONE;
  lte_rrc_sib_test_rf_status_time = 0;
  lte_rrc_sib_test_rf_status_sfn = LTE_RRC_NULL_SFN;
  lte_rrc_sib_num_po_mtr_from_rf_unavail = 0;
  lte_rrc_sib_num_po_missed_from_rf_avail = 0;

  lte_rrc_sib_test_auto_gap_is_active = FALSE;

}

void lte_rrc_sib_test_send_stopped_indi()
{
  lte_rrc_stopped_indi_s stopped_indi;
  
  // ----------------------------------------------------------------------

  lte_rrc_init_default_hdr(&stopped_indi.msg_hdr, LTE_RRC_STOPPED_INDI);

  lte_rrc_utf_send_msg((byte*)&stopped_indi, sizeof(stopped_indi));
  lte_rrc_utf_wait_for_done();
  lte_rrc_sib_test_camped = FALSE;
}

void lte_rrc_sib_test_send_deact_req()
{
  lte_rrc_deactivate_req_s lte_rrc_deact_req;
  
  // ----------------------------------------------------------------------

  lte_rrc_init_default_hdr(&lte_rrc_deact_req.msg_hdr, LTE_RRC_DEACTIVATE_REQ);

  lte_rrc_utf_send_msg((byte*)&lte_rrc_deact_req, sizeof(lte_rrc_deactivate_req_s));
  lte_rrc_utf_wait_for_done();
  lte_rrc_sib_test_camped = FALSE;
}

void lte_rrc_sib_test_send_abort_reqi(lte_phy_cell_id_t phy_cell_id,
                                      lte_earfcn_t freq)
{
  
  lte_rrc_sib_abort_reqi_s abort_reqi;

  // Prepare abort_reqi to send to SIB
  lte_rrc_init_default_hdr(&abort_reqi.msg_hdr, LTE_RRC_SIB_ABORT_REQI);
  abort_reqi.cell_id.phy_cell_id = phy_cell_id;
  abort_reqi.cell_id.freq = freq;
  
  // Send abort_reqi
  lte_rrc_utf_send_msg((byte*)&abort_reqi, sizeof(abort_reqi));
  lte_rrc_utf_wait_for_done();
}

void lte_rrc_sib_test_send_camped_indi(lte_phy_cell_id_t phy_cell_id,
                                       lte_earfcn_t freq)
{
  
  lte_rrc_camped_indi_s camped_indi;

  // After camped_ind is sent, lte_rrc_csp_get_serving_cell_params should 
  // return following values
  lte_rrc_sib_test_camped = TRUE;
  //lte_rrc_sib_test_sib_db_key;
  lte_rrc_sib_test_serv_cell_phy_cell_id = phy_cell_id;
  lte_rrc_sib_test_serv_cell_freq = freq;

  // Prepare camped_indi to send to SIB
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_indi.camped_cell.phy_cell_id = phy_cell_id;
  camped_indi.camped_cell.freq = freq;
  
  // Send camped_indi
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  lte_rrc_utf_wait_for_done();
}

void lte_rrc_sib_test_send_connected_indi()
{
  lte_rrc_connected_indi_s connected_indi;
  
  // ----------------------------------------------------------------------

  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__CONNECTED;
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);

  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));
  lte_rrc_utf_wait_for_done();
}

void lte_rrc_sib_test_send_mib(uint8 num_tx_antennas,
                               lte_bandwidth_e dl_bandwidth,
                               lte_l1_phich_duration_e phich_duration,
                               lte_l1_phich_resource_e phich_resource,
                               uint16 phy_cell_id,
                               lte_earfcn_t freq,
                               uint16 sfn)
{
  lte_cphy_mib_ind_s mib_ind;

  // Prepare mib_ind to send to RRC
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);

  mib_ind.mib.num_tx_antennas = num_tx_antennas;
  mib_ind.mib.dl_bandwidth = dl_bandwidth;
  mib_ind.mib.phich_info.phich_duration = phich_duration;
  mib_ind.mib.phich_info.phich_resource = phich_resource;
  mib_ind.phy_cell_id = phy_cell_id;
  mib_ind.freq = freq;
  mib_ind.sfn = sfn;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();
}

void lte_rrc_sib_test_send_bcch_dl_sch_msg(unsigned char *msg,
                                           uint16 msg_size,
                                           uint16 phy_cell_id,
                                           lte_earfcn_t freq,
                                           uint16 sfn)
{
  lte_mac_rrc_dl_data_ind_s bcch_dl_data_ind;
  dsm_item_type *dsm_ptr = NULL;            /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */

  msgr_init_hdr_attach(&bcch_dl_data_ind.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_BCCH_DL_DATA_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr, msg, msg_size, DSM_DS_SMALL_ITEM_POOL) == msg_size);

  TF_ASSERT(dsm_ptr != NULL);

  att_ptr = msgr_get_attach(&bcch_dl_data_ind.hdr, 0);
  msgr_set_dsm_attach(att_ptr, dsm_ptr);

  bcch_dl_data_ind.freq = freq;
  bcch_dl_data_ind.phy_cell_id = phy_cell_id;

  bcch_dl_data_ind.sfn.sys_fn = sfn;

  // Send bcch_dl_data_ind to RRC
  lte_rrc_utf_send_msg((byte*)&bcch_dl_data_ind, sizeof(bcch_dl_data_ind));
  lte_rrc_utf_wait_for_done();
}

void lte_rrc_sib_test_send_ho_started_indi()
{
  lte_rrc_handover_started_indi_s ho_started_indi;

  // Prepare ho_started_indi to send to SIB
  lte_rrc_init_default_hdr(&ho_started_indi.msg_hdr, LTE_RRC_HANDOVER_STARTED_INDI);
  
  // Send ho_started_indi
  lte_rrc_utf_send_msg((byte*)&ho_started_indi, sizeof(ho_started_indi));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_ho_in_progress = TRUE;
}

void lte_rrc_sib_test_send_tdd_cfg_cnf(lte_phy_cell_id_t phy_cell_id,
                                       lte_earfcn_t freq,
                                       errno_enum_type status)
{
  lte_cphy_tdd_cfg_cnf_s tdd_cfg_cnf;

  // Prepare tdd_cfg_cnf to send to SIB
  msgr_init_hdr(&tdd_cfg_cnf.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_TDD_CFG_CNF);

  tdd_cfg_cnf.phy_cell_id = phy_cell_id;
  tdd_cfg_cnf.freq = freq;
  tdd_cfg_cnf.status = status;

  // Send tdd_cfg_cnf
  lte_rrc_utf_send_msg((byte*)&tdd_cfg_cnf, sizeof(tdd_cfg_cnf));
  //lte_rrc_utf_wait_for_done();

}

void lte_rrc_sib_test_send_utc_time_update_cnf(errno_enum_type status)
{
  lte_cphy_utc_time_update_cnf_s utc_time_update_cnf;

  lte_rrc_init_default_hdr(&utc_time_update_cnf.msgr_hdr, LTE_CPHY_UTC_TIME_UPDATE_CNF);
  utc_time_update_cnf.status = status;

  lte_rrc_utf_send_msg((byte*)&utc_time_update_cnf, sizeof(utc_time_update_cnf));
  lte_rrc_utf_wait_for_done();

}


void lte_rrc_sib_test_send_service_ind()
{
  lte_rrc_service_ind_s service_ind;

  // Prepare service_ind to send
  lte_rrc_init_default_hdr(&service_ind.msg_hdr, LTE_RRC_SERVICE_IND);

  // Send service_ind
  lte_rrc_utf_send_msg((byte*)&service_ind, sizeof(service_ind));
  lte_rrc_utf_wait_for_done();

}

void lte_rrc_sib_test_send_rf_unavail_ind()
{
  lte_cphy_rf_unavailable_ind_s rf_unavail_ind;

  lte_rrc_init_default_hdr(&rf_unavail_ind.msgr_hdr, LTE_CPHY_RF_UNAVAILABLE_IND);
  rf_unavail_ind.ref_time.curr_rtc = lte_rrc_sib_test_rf_status_time;
  rf_unavail_ind.ref_time.curr_sfn = lte_rrc_sib_test_rf_status_sfn;
  rf_unavail_ind.num_po_mtrd_in_curr_mod_pd = lte_rrc_sib_num_po_mtr_from_rf_unavail;

  lte_rrc_utf_send_msg((byte*)&rf_unavail_ind, sizeof(lte_cphy_rf_unavailable_ind_s));
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
  lte_rrc_utf_wait_for_done();
}

void lte_rrc_sib_test_send_rf_avail_ind()
{
  lte_cphy_rf_available_ind_s rf_avail_ind;

  lte_rrc_init_default_hdr(&rf_avail_ind.msgr_hdr, LTE_CPHY_RF_AVAILABLE_IND);
  rf_avail_ind.ref_time.curr_rtc = lte_rrc_sib_test_rf_status_time;
  rf_avail_ind.ref_time.curr_sfn = lte_rrc_sib_test_rf_status_sfn;
  rf_avail_ind.num_po_missed_during_tune_away = lte_rrc_sib_num_po_missed_from_rf_avail;

  lte_rrc_utf_send_msg((byte*)&rf_avail_ind, sizeof(lte_cphy_rf_available_ind_s));
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
  lte_rrc_utf_wait_for_done();
}

lte_cphy_sib_sched_req_s* lte_rrc_sib_test_rcv_sib_sched_req(lte_phy_cell_id_t phy_cell_id,
                                        lte_earfcn_t freq,
                                        lte_rrc_sib_bitmask_t curr_mod_prd_sib_mask,
                                        lte_l1_sfn_t mod_period_boundary_sfn,
                                        lte_rrc_sib_bitmask_t next_mod_prd_sib_mask,
                                        lte_l1_sfn_t last_sfn_in_sib_rsp,
                                        lte_cphy_si_window_length_e si_window_length,
                                        uint8 num_si_msgs)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ)
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == phy_cell_id);
  TF_ASSERT(sib_sched_req_ptr->freq == freq);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == curr_mod_prd_sib_mask);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == mod_period_boundary_sfn);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == next_mod_prd_sib_mask);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == last_sfn_in_sib_rsp);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == si_window_length);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == num_si_msgs);
  return sib_sched_req_ptr;
}

void lte_rrc_sib_test_rcv_get_sibs_cnfi(lte_phy_cell_id_t phy_cell_id,
                                        lte_earfcn_t freq,
                                        lte_rrc_sib_db_key_t db_key,
                                        lte_rrc_get_sibs_status_e status)
{
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  uint32 buf_len;
  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(get_sibs_cnf_ptr->msg_hdr.id == LTE_RRC_GET_SIBS_CNFI)
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == freq);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == db_key);
  TF_ASSERT(get_sibs_cnf_ptr->status == status);
}

void lte_rrc_sib_test_rcv_sib_updated_indi(lte_phy_cell_id_t phy_cell_id,
                                           lte_earfcn_t freq,
                                           lte_rrc_sib_bitmask_t sibs_changed_bitmask,
                                           lte_rrc_sib_bitmask_t sibs_scheduled_bitmask)
{
  lte_rrc_sib_updated_indi_s *sib_updated_ind_ptr;
  uint32 buf_len;
  lte_rrc_utf_get_next_int_msg((byte**)&sib_updated_ind_ptr, &buf_len);
  TF_ASSERT(sib_updated_ind_ptr->msg_hdr.id == LTE_RRC_SIB_UPDATED_INDI);
  TF_ASSERT(sib_updated_ind_ptr->cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(sib_updated_ind_ptr->cell_id.freq == freq);
  TF_ASSERT(sib_updated_ind_ptr->sibs_changed_bitmask == sibs_changed_bitmask);
  TF_ASSERT(sib_updated_ind_ptr->sibs_scheduled_bitmask == sibs_scheduled_bitmask);
}

void lte_rrc_sib_test_rcv_sib_read_error_indi(lte_phy_cell_id_t phy_cell_id,
                                              lte_earfcn_t freq,
                                              lte_rrc_sib_read_error_cause_e cause,
                                              lte_rrc_sib_bitmask_t sibs_rcvd_curr_bitmask,
                                              boolean csg_indication,
                                              boolean intra_freq_resel_allowed)
{
  lte_rrc_sib_read_error_indi_s *sib_read_error_ind_ptr;
  uint32 buf_len;
  lte_rrc_utf_get_next_int_msg((byte**)&sib_read_error_ind_ptr, &buf_len);
  TF_ASSERT(sib_read_error_ind_ptr->msg_hdr.id == LTE_RRC_SIB_READ_ERROR_INDI);
  TF_ASSERT(sib_read_error_ind_ptr->cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(sib_read_error_ind_ptr->cell_id.freq == freq);
  TF_ASSERT(sib_read_error_ind_ptr->cause == cause);
  TF_ASSERT(sib_read_error_ind_ptr->sibs_rcvd_curr_bitmask == sibs_rcvd_curr_bitmask);
  TF_ASSERT(sib_read_error_ind_ptr->csg_indication == csg_indication);
  TF_ASSERT(sib_read_error_ind_ptr->intra_freq_resel_allowed == intra_freq_resel_allowed);
}

void lte_rrc_sib_test_rcv_revoke_reqi()
{
  lte_rrc_ml1_revoke_reqi_s *revoke_reqi_ptr;
  uint32 buf_len;
  lte_rrc_utf_get_next_int_msg((byte**)&revoke_reqi_ptr, &buf_len);
  TF_ASSERT(revoke_reqi_ptr->msg_hdr.id == LTE_RRC_ML1_REVOKE_REQI);
}

void lte_rrc_sib_test_rcv_get_sibs_reqi()
{
  lte_rrc_get_sibs_reqi_s *get_sibs_reqi_ptr;
  uint32 buf_len;
  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_reqi_ptr, &buf_len);
  TF_ASSERT(get_sibs_reqi_ptr->msg_hdr.id == LTE_RRC_GET_SIBS_REQI);
}

void lte_rrc_sib_test_send_get_sibs_reqi(lte_phy_cell_id_t phy_cell_id,
                                   lte_earfcn_t freq,
                                   lte_rrc_proc_e req_proc,
                                   lte_rrc_get_sibs_cause_e cause,
                                   boolean cnf_needed,
                                   lte_l1_sfn_t sfn)
{
  lte_rrc_get_sibs_reqi_s get_sibs_req;

  lte_rrc_init_default_hdr(&get_sibs_req.msg_hdr, LTE_RRC_GET_SIBS_REQI);
  get_sibs_req.cell_id.phy_cell_id = phy_cell_id;
  get_sibs_req.cell_id.freq = freq;
  get_sibs_req.req_proc = req_proc;
  get_sibs_req.cause = cause;
  get_sibs_req.cnf_needed = cnf_needed;
  get_sibs_req.sfn.sys_fn = sfn;
  
  lte_rrc_utf_send_msg((byte*)&get_sibs_req, sizeof(get_sibs_req));
  lte_rrc_utf_wait_for_done();
}

void lte_rrc_sib_test_camp_phase1()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  
  uint32 buf_len;

  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_CPHY_NULL_SFN);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
  //                                                                LTE_RRC_SIB3_BITMASK));
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
  //                                                                LTE_RRC_SIB5_BITMASK));
}

void lte_rrc_sib_test_camp_phase1_asserts()
{
  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == LTE_RRC_SIB_DB_MAX_CELLS);
  for(int i = 0; i < LTE_RRC_SIB_DB_MAX_CELLS; i++ )
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_bitmask == 0);
  for(int i = 0; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == TRUE);
  for(int i = 0; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == LTE_RRC_SIB_INVALID_TIME);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                       LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == 0);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB_UPDATE_NEEDED_BITMASK &
                                                                         LTE_RRC_SIB_NON_MANDATORY_SIBS_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == LTE_RRC_SIB_INVALID_WINDOW_LENGTH);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == LTE_RRC_NULL_SFN);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 0);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == );
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */

  // ----------------------------------------------------------------------
}

void lte_rrc_sib_test_camp_phase2()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  // send MIB to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            1);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
  //                                                                LTE_RRC_SIB3_BITMASK));
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
  //                                                                LTE_RRC_SIB5_BITMASK));
}

void lte_rrc_sib_test_camp_phase2_asserts()
{
  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == LTE_RRC_SIB_DB_MAX_CELLS);
  for(int i = 0; i < LTE_RRC_SIB_DB_MAX_CELLS; i++ )
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_bitmask == 0);
  for(int i = 0; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == 1);
  for(int i = 1; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == LTE_RRC_SIB_MIB_SIB1_WT_TMR);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                       LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == LTE_RRC_MIB_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == LTE_RRC_MIB_BITMASK);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB_UPDATE_NEEDED_BITMASK &
                                                                         LTE_RRC_SIB_NON_MANDATORY_SIBS_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 1);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 0);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == );
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */

  // ----------------------------------------------------------------------
}

void lte_rrc_sib_test_camp_phase3()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;

  uint32 buf_len;

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
}

void lte_rrc_sib_test_camp_phase3_asserts()
{
  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == LTE_RRC_SIB_DB_MAX_CELLS);
  for(int i = 0; i < LTE_RRC_SIB_DB_MAX_CELLS; i++ )
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr != NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_bitmask == 0);
  for(int i = 0; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[1] == 2);
  for(int i = 2; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == 160 * LTE_RRC_SIB_SIB2_WT_TMR_FACTOR);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK | 
                                                                       LTE_RRC_SIB1_BITMASK |
                                                                       LTE_RRC_SIB2_BITMASK |
                                                                       LTE_RRC_SIB3_BITMASK |
                                                                       LTE_RRC_SIB4_BITMASK |
                                                                       LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                 LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                      LTE_RRC_SIB1_BITMASK));

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                         LTE_RRC_SIB4_BITMASK |
                                                                         LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                                               LTE_RRC_SIB3_BITMASK |
                                                                               LTE_RRC_SIB4_BITMASK |
                                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK |
                                                                               LTE_RRC_SIB2_BITMASK |
                                                                               LTE_RRC_SIB3_BITMASK |
                                                                               LTE_RRC_SIB4_BITMASK |
                                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                        LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_SIB2);

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */

  // ----------------------------------------------------------------------
}

void lte_rrc_sib_test_camp_phase4()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  uint32 buf_len;

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  // this will make CSP return this cell as serving cell, though camped_indi as 
  // not been sent for the cell
  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;
  lte_rrc_sib_test_serv_cell_phy_cell_id = PHY_CELL_ID_1;
  lte_rrc_sib_test_serv_cell_freq = FREQ_1;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
}

void lte_rrc_sib_test_camp_phase4_asserts()
{
  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == LTE_RRC_SIB_DB_MAX_CELLS);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  for(int i = 1; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr != NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_bitmask == 0);
  for(int i = 0; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[1] == 2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[2] == 3);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[3] == 3);
  for(int i = 4; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == (LTE_RRC_SIB_SI_WT_TMR_FACTOR * 80));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK | 
                                                                       LTE_RRC_SIB1_BITMASK |
                                                                       LTE_RRC_SIB2_BITMASK |
                                                                       LTE_RRC_SIB3_BITMASK |
                                                                       LTE_RRC_SIB4_BITMASK |
                                                                       LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                 LTE_RRC_SIB1_BITMASK |
                                                                 LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                      LTE_RRC_SIB1_BITMASK |
                                                                      LTE_RRC_SIB2_BITMASK |
                                                                      LTE_RRC_SIB3_BITMASK));

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                         LTE_RRC_SIB4_BITMASK |
                                                                         LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK |
                                                                               LTE_RRC_SIB2_BITMASK |
                                                                               LTE_RRC_SIB3_BITMASK |
                                                                               LTE_RRC_SIB4_BITMASK |
                                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 3);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                        LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_SIB3_SIB4);

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_counter == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[0] == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[1] == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[2] == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.num_mnc_digits == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[0] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[1] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[2] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.global_cell_id == 0x400000);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                               LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_sib3_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] != NULL);
  for(int i = 1; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_ptr == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib3_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib4_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib5_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);

  // ----------------------------------------------------------------------
}

void lte_rrc_sib_test_camp_phase5()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;

  uint32 buf_len;

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
}

void lte_rrc_sib_test_camp_phase5_asserts()
{
  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == LTE_RRC_SIB_DB_MAX_CELLS);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  for(int i = 1; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr != NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_bitmask == 0);
  for(int i = 0; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[1] == 2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[2] == 3);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[3] == 3);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[4] == 4);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[5] == 4);
  for(int i = 6; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == 
            (uint32)(LTE_RRC_SIB_MIB_SIB1_WT_TMR + 
                     10*(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end + 1 - 
                         lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp)));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK | 
                                                                       LTE_RRC_SIB1_BITMASK |
                                                                       LTE_RRC_SIB2_BITMASK |
                                                                       LTE_RRC_SIB3_BITMASK |
                                                                       LTE_RRC_SIB4_BITMASK |
                                                                       LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                 LTE_RRC_SIB1_BITMASK |
                                                                 LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                      LTE_RRC_SIB1_BITMASK |
                                                                      LTE_RRC_SIB2_BITMASK |
                                                                      LTE_RRC_SIB3_BITMASK |
                                                                      LTE_RRC_SIB4_BITMASK |
                                                                      LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                         LTE_RRC_SIB4_BITMASK |
                                                                         LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (0));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK |
                                                                               LTE_RRC_SIB2_BITMASK |
                                                                               LTE_RRC_SIB3_BITMASK |
                                                                               LTE_RRC_SIB4_BITMASK |
                                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 4);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                        LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_counter == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[0] == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[1] == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[2] == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.num_mnc_digits == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[0] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[1] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[2] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.global_cell_id == 0x400000);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                               LTE_RRC_SIB3_BITMASK |
                                                               LTE_RRC_SIB4_BITMASK |
                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_sib3_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[1] == (LTE_RRC_SIB4_BITMASK |
                                                                        LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_size == sizeof(sib4_sib5_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1] != NULL);
  for(int i = 2; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib3_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib4_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib5_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib5);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);

  // ----------------------------------------------------------------------
}

void lte_rrc_sib_test_camp_phase6()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  
  uint32 buf_len;

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);
  
  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  /* Timer should have been restarted for MIB and SIB1 and duration is current 
     mod prd remaining time + MIB_SIB1 timer */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == 
            (LTE_RRC_SIB_MIB_SIB1_WT_TMR + 600));
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr) == 
            (LTE_RRC_SIB_MIB_SIB1_WT_TMR + 600));
  
  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            64);
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
}

void lte_rrc_sib_test_camp_phase6_asserts()
{
  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  for(int i = 1; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_counter == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[0] == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[1] == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[2] == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.num_mnc_digits == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[0] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[1] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[2] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.global_cell_id == 0x400000);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                               LTE_RRC_SIB3_BITMASK |
                                                               LTE_RRC_SIB4_BITMASK |
                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_sib3_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[1] == (LTE_RRC_SIB4_BITMASK |
                                                                        LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_size == sizeof(sib4_sib5_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1] != NULL);
  for(int i = 2; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib3_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib4_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib5_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib5);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);

  // ----------------------------------------------------------------------
}

void lte_rrc_sib_test_stub_send_sib8_unavailable_to_1x ( void )
{
  TF_MSG("SIB Module sent LTE_RRC_1X_SIB8_INFO_IND to 1x on SIB8 Timeout");
}

void lte_rrc_sib_test_send_extnd_tmr_reqi
(
  uint32 time_duration
)
{
  lte_rrc_sib_extnd_tmr_reqi_s req;
  lte_errno_e status;

  lte_rrc_init_default_hdr(&req.msg_hdr, LTE_RRC_SIB_EXTND_TMR_REQI);
  req.reason = LTE_RRC_SIB_EXTND_TMR_AUTO_GAP;
  req.duration = time_duration;

  lte_rrc_utf_send_msg((byte*)&req, sizeof(req));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

