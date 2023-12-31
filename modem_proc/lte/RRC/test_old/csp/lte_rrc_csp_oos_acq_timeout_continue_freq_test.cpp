/*!
  @file lte_rrc_csp_oos_acq_timeout_continue_freq_test.cpp

  @brief
  Controller is in Inactive state and there is nothing in the Acq DB. Band
  Scan is started by means of a Service Request with use_timer = TRUE. The
  timer is made to expire before an acquisition cnf.

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

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/09   fhu     replaced lte_cphy_sap.h by lte_cphy_msg.h
04/09/09   sureshs Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include <lte_rrc_int_msg.h>
#include <lte_rrc_ext_msg.h>
#include <lte_cphy_msg.h>
#include <__lte_rrc_csp.h>
#include <__lte_rrc_controller.h>
#include <lte_rrc_utils.h>
#include <lte_rrc_csp.h>
}

#include <TestFramework.h>
#include <lte_rrc_utf.h>
#include "lte_rrc_csp_test_common.h"

//external messages generated by code under test
static uint32 ext_umid_list[] = 
{ 
  LTE_CPHY_BAND_SCAN_REQ,
  LTE_CPHY_ACQ_REQ,
  LTE_RRC_SERVICE_IND,
  LTE_RRC_BARRING_UPDATE_IND
};

//message structures
static lte_rrc_service_req_s svc_req;
static lte_rrc_mode_change_cnfi_s mode_change_cnf;
static lte_cphy_band_scan_cnf_s bs_cnf;

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCspOosAcqTimeoutContinueFreqTest);

void LteRrcCspOosAcqTimeoutContinueFreqTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CSP_SM,ext_umid_list,4);
  lte_rrc_csp_test_stub_init();
}

void LteRrcCspOosAcqTimeoutContinueFreqTest::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  lte_rrc_mode_change_reqi_s *mode_change_req_ptr;
  lte_cphy_band_scan_req_s *bs_req_ptr;
  lte_rrc_service_ind_s *svc_ind_ptr;
  lte_cphy_acq_req_s *acq_req_ptr;
  lte_rrc_cfg_reqi_s *cfg_req_ptr;
  lte_rrc_tmri_s *oos_tmr_ind_ptr;
  
  lte_rrc_csp_test_cfg_req_param_s			cfg_req_param;
  lte_rrc_csp_validate_system_scan_req_s  validate_system_scan_req[LTE_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN];;
  lte_rrc_csp_send_band_scan_cnf_s			send_band_scan_cnf;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == LTE_RRC_CSP_SM__INITIAL);

  lte_rrc_csp_test_stub_controller_set_state(LTE_RRC_CONTROLLER_SM__INACTIVE);

  lte_rrc_csp_test_stub_controller_set_access_class(8);

  //lte_rrc_csp_set_eoos_complete_current_freq(TRUE);

  lte_rrc_csp_test_set_band_scan_result_ptr(&bs_cnf);

  lte_rrc_csp_set_iterative_band_scan_enabled(FALSE);

  TF_MSG("Sending Service Req");
  msgr_init_hdr(&svc_req.msg_hdr,MSGR_NAS_EMM,LTE_RRC_SERVICE_REQ);
  svc_req.trans_id = 1;
  svc_req.network_select_mode = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;
  svc_req.csg_id = SYS_CSG_ID_INVALID;  svc_req.req_plmn_info_is_valid = TRUE;
  svc_req.req_plmn.mcc[0] = 1;
  svc_req.req_plmn.mcc[1] = 2;
  svc_req.req_plmn.mcc[2] = 3;
  svc_req.req_plmn.num_mnc_digits = 3;
  svc_req.req_plmn.mnc[0] = 4;
  svc_req.req_plmn.mnc[1] = 5;
  svc_req.req_plmn.mnc[2] = 6;
  svc_req.rplmn_info_is_valid = TRUE;
  svc_req.rplmn.mcc[0] = 7;
  svc_req.rplmn.mcc[1] = 8;
  svc_req.rplmn.mcc[2] = 9;
  svc_req.rplmn.num_mnc_digits = 3;
  svc_req.rplmn.mnc[0] = 10;
  svc_req.rplmn.mnc[1] = 11;
  svc_req.rplmn.mnc[2] = 12;
  svc_req.hplmn_info_is_valid = TRUE;
  svc_req.hplmn.mcc[0] = 13;
  svc_req.hplmn.mcc[1] = 14;
  svc_req.hplmn.mcc[2] = 15;
  svc_req.hplmn.num_mnc_digits = 3;
  svc_req.hplmn.mnc[0] = 16;
  svc_req.hplmn.mnc[1] = 17;
  svc_req.hplmn.mnc[2] = 18;
  svc_req.ehplmn_list.num_plmns = 1;
  svc_req.ehplmn_list.plmn[0].mcc[0] = 19;
  svc_req.ehplmn_list.plmn[0].mcc[1] = 20;
  svc_req.ehplmn_list.plmn[0].mcc[2] = 21;
  svc_req.ehplmn_list.plmn[0].num_mnc_digits = 3;
  svc_req.ehplmn_list.plmn[0].mnc[0] = 22;
  svc_req.ehplmn_list.plmn[0].mnc[1] = 23;
  svc_req.ehplmn_list.plmn[0].mnc[2] = 24;
  memset(&svc_req.forbidden_ta_list,0,
         sizeof(svc_req.forbidden_ta_list));
  memset(&svc_req.eplmn_list,0,sizeof(svc_req.eplmn_list));
  memset(&svc_req.rat_pri_list,0,sizeof(svc_req.rat_pri_list));
  svc_req.rat_pri_list.num_items = 1;
  svc_req.rat_pri_list.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_LTE;
  svc_req.rat_pri_list.priority_list_info[0].band_cap.lte_band_cap.bits_1_64 |= SYS_BAND_MASK_LTE_BAND1;
  svc_req.rat_pri_list.priority_list_info[0].bst_band_cap.lte_band_cap.bits_1_64 |= svc_req.rat_pri_list.priority_list_info[0].band_cap.lte_band_cap.bits_1_64;
  svc_req.rat_pri_list.priority_list_info[0].bst_rat_acq_required = TRUE;
  svc_req.rat_pri_list.scan_type.new_scan = TRUE;
  svc_req.rat_pri_list.scan_type.use_timer = TRUE;
  svc_req.rat_pri_list.priority_list_info[0].acq_sys_time_interval = 3000;
  lte_rrc_utf_send_msg((byte*)&svc_req, sizeof(svc_req));

  TF_MSG("Checking for LTE_RRC_MODE_CHANGE_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_mode_change_reqi_s) == buf_len);
  mode_change_req_ptr = (lte_rrc_mode_change_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_MODE_CHANGE_REQI == mode_change_req_ptr->msg_hdr.id);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for WT_MODE_CHANGE_CNF state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == 
            LTE_RRC_CSP_SM__WT_MODE_CHANGE_CNF);

  TF_MSG("Sending Mode Change Cnf");
  lte_rrc_init_default_hdr(&mode_change_cnf.msg_hdr,LTE_RRC_MODE_CHANGE_CNFI);
  mode_change_cnf.mode = LTE_RRC_MODE_CHANGE_ACTIVATE;  mode_change_cnf.status = TRUE;
  lte_rrc_utf_send_msg((byte*)&mode_change_cnf, sizeof(mode_change_cnf));

  cfg_req_param.trans_id = 1;
  cfg_req_param.num_systems = 1;
  cfg_req_param.band = SYS_SBAND_LTE_EUTRAN_BAND1;
  cfg_req_param.earfcn = 1;
  cfg_req_param.bandwidth = LTE_BW_NRB_6;
  cfg_req_param.phy_cell_id = 1;
  cfg_req_param.num_black_cells = 0;
  cfg_req_param.energy = -60;
  cfg_req_param.num_candidates = 1;
  cfg_req_param.status = E_SUCCESS;

  send_band_scan_cnf.band = SYS_SBAND_LTE_EUTRAN_BAND1;
  send_band_scan_cnf.num_candidates = 1;
  send_band_scan_cnf.unusable_systems.num_systems = 0;

  send_band_scan_cnf.candidates[0].energy = -60;
  send_band_scan_cnf.candidates[0].system.band = SYS_SBAND_LTE_EUTRAN_BAND1;
  send_band_scan_cnf.candidates[0].system.bandwidth = LTE_BW_NRB_6;
  send_band_scan_cnf.candidates[0].system.earfcn = 1;
 
  lte_rrc_csp_test_handle_rrc_cfg_req (&cfg_req_param, validate_system_scan_req,&send_band_scan_cnf);

  TF_MSG("Checking for LTE_RRC_CFG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);
  
  cfg_req_ptr = (lte_rrc_cfg_reqi_s *)(void*) buf_ptr;
  printf("\nLTE_RRC_CFG_REQI[%d]  cfg_req_ptr->msg_hdr.id[%d]\n",LTE_RRC_CFG_REQI , cfg_req_ptr->msg_hdr.id);
  TF_ASSERT(LTE_RRC_CFG_REQI == cfg_req_ptr->msg_hdr.id);
  TF_ASSERT(FALSE == cfg_req_ptr->cnfi_required);
  TF_ASSERT(LTE_RRC_PROC_CSP == cfg_req_ptr->proc);
  TF_ASSERT(1 == cfg_req_ptr->num_cfg);
  TF_ASSERT(LTE_RRC_CFG_ACQ == cfg_req_ptr->cfg_info[0].cfg_type);
  
  TF_MSG("LTE_RRC_CFG_ACQ rcvd");
  TF_ASSERT(sizeof(lte_cphy_acq_req_s) == sizeof(cfg_req_ptr->cfg_info[0].cfg_u.acq_req_cfg));
  acq_req_ptr = (lte_cphy_acq_req_s *) &(cfg_req_ptr->cfg_info[0].cfg_u.acq_req_cfg);
  
  TF_ASSERT(LTE_CPHY_ACQ_REQ == acq_req_ptr->msgr_hdr.id);
  TF_ASSERT(2 == acq_req_ptr->trans_id);
  TF_ASSERT(1== acq_req_ptr->earfcn);
  TF_ASSERT(SYS_SBAND_LTE_EUTRAN_BAND1== acq_req_ptr->band);
  TF_ASSERT(0 == acq_req_ptr->num_black_cells);
  TF_ASSERT(LTE_CPHY_ACQ_FREQ == acq_req_ptr->acq_type);
  
  lte_rrc_utf_wait_for_done();
  
  TF_MSG("Checking for WT_ACQ_CNF state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == LTE_RRC_CSP_SM__WT_ACQ_CNF);

	
  TF_MSG("Expiring time");
  TF_SLEEP(3000);

  TF_MSG("Checking for LTE_RRC_CFG_REQI for abort");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);
  cfg_req_ptr = (lte_rrc_cfg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_CFG_REQI == cfg_req_ptr->msg_hdr.id);
  TF_ASSERT(FALSE == cfg_req_ptr->cnfi_required);
  TF_ASSERT(LTE_RRC_PROC_CSP == cfg_req_ptr->proc);
  TF_ASSERT(1 == cfg_req_ptr->num_cfg);
  TF_ASSERT(LTE_RRC_CFG_ABORT_ML1 == cfg_req_ptr->cfg_info[0].cfg_type);

  TF_MSG("Checking for LTE_RRC_OOS_TMRI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  oos_tmr_ind_ptr = (lte_rrc_tmri_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_OOS_TMRI == oos_tmr_ind_ptr->msg_hdr.id);
lte_rrc_test_integration_verify_req_send_cnf(NULL, LTE_RRC_BARRING_UPDATE_IND, LTE_RRC_TEST_CELL_0, FALSE, NULL);
  TF_MSG("Checking for LTE_RRC_SERVICE_IND");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_service_ind_s) == buf_len);
  svc_ind_ptr = (lte_rrc_service_ind_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SERVICE_IND == svc_ind_ptr->msg_hdr.id);
  TF_ASSERT(1 == svc_ind_ptr->trans_id);
  TF_ASSERT(LTE_RRC_SVC_STATUS_NOT_AVAILABLE == svc_ind_ptr->svc_status);
  TF_ASSERT(FALSE == svc_ind_ptr->oos_search_done);

  TF_MSG("Checking for LTE_RRC_SERVICE_IND");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_service_ind_s) == buf_len);
  svc_ind_ptr = (lte_rrc_service_ind_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SERVICE_IND == svc_ind_ptr->msg_hdr.id);

  TF_MSG("End of test");
}

void LteRrcCspOosAcqTimeoutContinueFreqTest::Teardown()
{
  lte_rrc_utf_teardown();
}

