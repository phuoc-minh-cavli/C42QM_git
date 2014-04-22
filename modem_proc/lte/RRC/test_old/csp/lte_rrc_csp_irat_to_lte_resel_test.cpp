/*!
  @file lte_rrc_csp_irat_to_lte_resel_test.cpp

  @brief
  Unit test code for taking CSP to camped state through IRAT to LTE reselection.
  Controller is in Inactive state and there is nothing in the Acq DB. IRAT to
  LTE reselection request comes in and cell selection proceeds.

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
12/22/09   sureshs Initial Revision
           
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
  LTE_CPHY_ACQ_REQ,
  LTE_CPHY_CELL_SELECT_REQ,
  LTE_RRC_SERVICE_IND,
  LTE_RRC_BARRING_UPDATE_IND
};

//message structures
static lte_rrc_system_update_req_s sys_upd_req;
static lte_rrc_irat_to_lte_resel_reqi_s resel_reqi;
static lte_cphy_acq_cnf_s acq_cnf;
static lte_rrc_get_sibs_cnfi_s get_sib_cnf;
static lte_cphy_cell_select_cnf_s cell_sel_cnf;
static lte_rrc_cfg_cnfi_s config_cnf;
static lte_rrc_irat_to_lte_resel_completed_indi_s resel_compl_ind;

//transaction ID to be returned from config request
#define LTE_RRC_CSP_TEST_CFG_TRANS_ID 7

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCspIratToLteReselTest);

void LteRrcCspIratToLteReselTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CSP_SM,ext_umid_list,4);
  lte_rrc_csp_test_stub_init();
}

void LteRrcCspIratToLteReselTest::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  lte_cphy_acq_req_s *acq_req_ptr;
  lte_rrc_get_sibs_reqi_s *sib_req_ptr;
  lte_cphy_cell_select_req_s *cell_sel_req_ptr;
  lte_rrc_cfg_reqi_s *cfg_req_ptr;
  lte_rrc_irat_to_lte_resel_cnfi_s * resel_cnfi_ptr;
  lte_rrc_camped_indi_s *camped_ind_ptr;
  lte_rrc_service_ind_s *svc_ind_ptr;
  lte_rrc_csp_test_cfg_req_param_s		  cfg_req_param;
  lte_rrc_csp_validate_system_scan_req_s  validate_system_scan_req[LTE_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN];;
  lte_rrc_csp_send_band_scan_cnf_s		  send_band_scan_cnf;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == LTE_RRC_CSP_SM__INITIAL);

  lte_rrc_csp_test_stub_controller_set_access_class(8);

  lte_rrc_csp_test_construct_basic_sib1();

  lte_rrc_utf_set_sync_cfg_reqi_params(LTE_RRC_CSP_TEST_CFG_TRANS_ID);

  TF_MSG("Sending System Update Req");
  msgr_init_hdr(&sys_upd_req.msg_hdr,MSGR_NAS_EMM,LTE_RRC_SYSTEM_UPDATE_REQ);
  sys_upd_req.rplmn_info_is_valid = TRUE;
  sys_upd_req.rplmn.mcc[0] = 7;
  sys_upd_req.rplmn.mcc[1] = 8;
  sys_upd_req.rplmn.mcc[2] = 9;
  sys_upd_req.rplmn.num_mnc_digits = 3;
  sys_upd_req.rplmn.mnc[0] = 10;
  sys_upd_req.rplmn.mnc[1] = 11;
  sys_upd_req.rplmn.mnc[2] = 12;
  sys_upd_req.hplmn_info_is_valid = TRUE;
  sys_upd_req.hplmn.mcc[0] = 13;
  sys_upd_req.hplmn.mcc[1] = 14;
  sys_upd_req.hplmn.mcc[2] = 15;
  sys_upd_req.hplmn.num_mnc_digits = 3;
  sys_upd_req.hplmn.mnc[0] = 16;
  sys_upd_req.hplmn.mnc[1] = 17;
  sys_upd_req.hplmn.mnc[2] = 18;
  sys_upd_req.ehplmn_list.num_plmns = 1;
  sys_upd_req.ehplmn_list.plmn[0].mcc[0] = 19;
  sys_upd_req.ehplmn_list.plmn[0].mcc[1] = 20;
  sys_upd_req.ehplmn_list.plmn[0].mcc[2] = 21;
  sys_upd_req.ehplmn_list.plmn[0].num_mnc_digits = 3;
  sys_upd_req.ehplmn_list.plmn[0].mnc[0] = 22;
  sys_upd_req.ehplmn_list.plmn[0].mnc[1] = 23;
  sys_upd_req.ehplmn_list.plmn[0].mnc[2] = 24;
  memset(&sys_upd_req.forbidden_ta_list,0,
         sizeof(sys_upd_req.forbidden_ta_list));
  memset(&sys_upd_req.eplmn_list,0,sizeof(sys_upd_req.eplmn_list));
  lte_rrc_utf_send_msg((byte*)&sys_upd_req, sizeof(sys_upd_req));

  TF_MSG("Sending IRAT to LTE Resel Reqi");
  lte_rrc_init_default_hdr(&resel_reqi.msg_hdr,LTE_RRC_IRAT_TO_LTE_RESEL_REQI);
  memset(&resel_reqi.rat_pri_list,0,sizeof(resel_reqi.rat_pri_list));
  resel_reqi.rat_pri_list.num_items = 1;
  resel_reqi.rat_pri_list.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_LTE;
  resel_reqi.rat_pri_list.priority_list_info[0].band_cap.lte_band_cap.bits_1_64 |= SYS_BAND_MASK_LTE_BAND1;
  resel_reqi.rat_pri_list.priority_list_info[0].bst_band_cap.lte_band_cap.bits_1_64 |= resel_reqi.rat_pri_list.priority_list_info[0].band_cap.lte_band_cap.bits_1_64;
  resel_reqi.rat_pri_list.priority_list_info[0].bst_rat_acq_required = TRUE;
  resel_reqi.network_select_mode = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;
  resel_reqi.req_plmn_info_is_valid = TRUE;
  resel_reqi.req_plmn.mcc[0] = 1;
  resel_reqi.req_plmn.mcc[1] = 2;
  resel_reqi.req_plmn.mcc[2] = 3;
  resel_reqi.req_plmn.num_mnc_digits = 3;
  resel_reqi.req_plmn.mnc[0] = 4;
  resel_reqi.req_plmn.mnc[1] = 5;
  resel_reqi.req_plmn.mnc[2] = 6;
  resel_reqi.dl_cell_frequency = 1;
  resel_reqi.phy_cell_id = 1;
  lte_rrc_utf_send_msg((byte*)&resel_reqi, sizeof(resel_reqi));

  cfg_req_param.trans_id = 1;
  cfg_req_param.band = SYS_SBAND_LTE_EUTRAN_BAND1;
  cfg_req_param.earfcn = 1;
  cfg_req_param.bandwidth = LTE_BW_NRB_6;
  cfg_req_param.phy_cell_id = 1;
  cfg_req_param.num_black_cells = 0;
  cfg_req_param.energy = -60;
  cfg_req_param.status = E_SUCCESS;
 
  lte_rrc_csp_test_handle_rrc_cfg_req (&cfg_req_param, validate_system_scan_req,&send_band_scan_cnf);

  TF_MSG("Checking for LTE_RRC_GET_SIBS_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_get_sibs_reqi_s) == buf_len);
  sib_req_ptr = (lte_rrc_get_sibs_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_GET_SIBS_REQI == sib_req_ptr->msg_hdr.id);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for WT_SIB_CNF state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == 
            LTE_RRC_CSP_SM__WT_SIB_CNF);

  TF_MSG("Sending Get Sibs Cnf");
  lte_rrc_init_default_hdr(&get_sib_cnf.msg_hdr,LTE_RRC_GET_SIBS_CNFI);
  get_sib_cnf.status = LTE_RRC_GET_SIBS_SUCCESS;
  get_sib_cnf.cell_info.cell_id.freq = 1;
  get_sib_cnf.cell_info.cell_id.phy_cell_id = 1;
  get_sib_cnf.cell_info.sib_db_key = 1;
  lte_rrc_utf_send_msg((byte*)&get_sib_cnf, sizeof(get_sib_cnf));

  TF_MSG("Checking for LTE_CPHY_CELL_SELECT_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_cell_select_req_s) == buf_len);
  cell_sel_req_ptr = (lte_cphy_cell_select_req_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_CPHY_CELL_SELECT_REQ == cell_sel_req_ptr->msgr_hdr.id);
  TF_ASSERT(SYS_SBAND_LTE_EUTRAN_BAND1 == cell_sel_req_ptr->band);
  TF_ASSERT(1 == cell_sel_req_ptr->earfcn);
  TF_ASSERT(1 == cell_sel_req_ptr->phy_cell_id);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for WT_CELL_SELECT_CNF state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == 
            LTE_RRC_CSP_SM__WT_CELL_SELECT_CNF);

  TF_MSG("Sending LTE_CPHY_CELL_SELECT_CNF");
  msgr_init_hdr(&cell_sel_cnf.msgr_hdr,MSGR_LTE_CPHY,LTE_CPHY_CELL_SELECT_CNF);
  cell_sel_cnf.status = E_SUCCESS;
  cell_sel_cnf.failure_reason = 0;
  cell_sel_cnf.band = SYS_SBAND_LTE_EUTRAN_BAND1;
  cell_sel_cnf.earfcn = 1;
  cell_sel_cnf.phy_cell_id = 1;
  cell_sel_cnf.rsrp = 0;
  lte_rrc_utf_send_msg((byte*)&cell_sel_cnf, sizeof(cell_sel_cnf));

  TF_MSG("Checking for LTE_RRC_CFG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);
  cfg_req_ptr = (lte_rrc_cfg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_CFG_REQI == cfg_req_ptr->msg_hdr.id);
  TF_ASSERT(TRUE == cfg_req_ptr->cnfi_required);
  TF_ASSERT(LTE_RRC_PROC_CSP == cfg_req_ptr->proc);
  TF_ASSERT(1 == cfg_req_ptr->num_cfg);
  TF_ASSERT(LTE_RRC_CFG_SRV_COMMON == cfg_req_ptr->cfg_info[0].cfg_type);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for WT_CFG_CNF state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == 
            LTE_RRC_CSP_SM__WT_CFG_CNF);

  TF_MSG("Sending Config Cnf");
  lte_rrc_init_default_hdr(&config_cnf.msg_hdr,LTE_RRC_CFG_CNFI);
  config_cnf.proc = LTE_RRC_PROC_CSP;
  config_cnf.trans_id = LTE_RRC_CSP_TEST_CFG_TRANS_ID;
  config_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&config_cnf, sizeof(config_cnf));

  TF_MSG("Checking for LTE_RRC_IRAT_TO_LTE_RESEL_CNFI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_irat_to_lte_resel_cnfi_s) == buf_len);
  resel_cnfi_ptr = (lte_rrc_irat_to_lte_resel_cnfi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_IRAT_TO_LTE_RESEL_CNFI == resel_cnfi_ptr->msg_hdr.id);
  TF_ASSERT(E_SUCCESS == resel_cnfi_ptr->status);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for CAMPED state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == 
            LTE_RRC_CSP_SM__CAMPED);

  TF_MSG("Sending LTE_RRC_IRAT_TO_LTE_RESEL_COMPLETED_INDI");
  lte_rrc_init_default_hdr(&resel_compl_ind.msg_hdr,
                           LTE_RRC_IRAT_TO_LTE_RESEL_COMPLETED_INDI);
  lte_rrc_utf_send_msg((byte*)&resel_compl_ind, sizeof(resel_compl_ind));

   lte_rrc_csp_test_stub_controller_set_state(LTE_RRC_CONTROLLER_SM__IDLE_CAMPED);  TF_MSG("Checking for LTE_RRC_CAMPED_INDI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_camped_indi_s) == buf_len);
  camped_ind_ptr = (lte_rrc_camped_indi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_CAMPED_INDI == camped_ind_ptr->msg_hdr.id);

  lte_rrc_utf_wait_for_done();
lte_rrc_test_integration_verify_req_send_cnf(NULL, LTE_RRC_BARRING_UPDATE_IND, LTE_RRC_TEST_CELL_0, FALSE, NULL);
  TF_MSG("Checking for LTE_RRC_SERVICE_IND");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_service_ind_s) == buf_len);
  svc_ind_ptr = (lte_rrc_service_ind_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SERVICE_IND == svc_ind_ptr->msg_hdr.id);
  TF_ASSERT(LTE_RRC_ASYNC_SRV_IND_TRANS_ID == svc_ind_ptr->trans_id);
  TF_ASSERT(LTE_RRC_SVC_STATUS_AVAILABLE == svc_ind_ptr->svc_status);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for CAMPED state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == 
            LTE_RRC_CSP_SM__CAMPED);

  lte_rrc_csp_test_free_sib1_pdu();

  TF_MSG("End of test");
}

void LteRrcCspIratToLteReselTest::Teardown()
{
  lte_rrc_utf_teardown();
}

