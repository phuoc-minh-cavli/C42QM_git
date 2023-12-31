/*!
  @file lte_rrc_csp_irat_to_lte_resel_fail_test.cpp

  @brief
  Unit test code for taking CSP to camped state through IRAT to LTE reselection.
  Controller is in Inactive state and there is nothing in the Acq DB. IRAT to
  LTE reselection request comes in and cell selection proceeds. Cell is found
  to be barred.

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
#include <lte_rrc_osys_asn1.h>
#include <lte_rrc_osys_asn1util.h>
}

#include <TestFramework.h>
#include <lte_rrc_utf.h>
#include "lte_rrc_csp_test_common.h"

//external messages generated by code under test
static uint32 ext_umid_list[] = 
{ 
  LTE_CPHY_ACQ_REQ,
  LTE_CPHY_CELL_BAR_REQ
};

//message structures
static lte_rrc_system_update_req_s sys_upd_req;
static lte_rrc_irat_to_lte_resel_reqi_s resel_reqi;
static lte_cphy_acq_cnf_s acq_cnf;
static lte_rrc_get_sibs_cnfi_s get_sib_cnf;

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCspIratToLteReselFailTest);

void LteRrcCspIratToLteReselFailTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CSP_SM,ext_umid_list,2);
  lte_rrc_csp_test_stub_init();
}

void LteRrcCspIratToLteReselFailTest::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  lte_cphy_acq_req_s *acq_req_ptr;
  lte_rrc_get_sibs_reqi_s *sib_req_ptr;
  lte_rrc_irat_to_lte_resel_cnfi_s * resel_cnfi_ptr;
  lte_rrc_sib_abort_reqi_s *sib_abort_ptr;
  lte_cphy_cell_bar_req_s *cphy_cell_bar_req_ptr;
  lte_rrc_osys_BCCH_DL_SCH_Message *sib1_pdu_ptr;
  lte_rrc_csp_test_cfg_req_param_s			cfg_req_param;
  lte_rrc_csp_validate_system_scan_req_s  validate_system_scan_req[LTE_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN];;
  lte_rrc_csp_send_band_scan_cnf_s			send_band_scan_cnf;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == LTE_RRC_CSP_SM__INITIAL);

  lte_rrc_csp_test_stub_controller_set_access_class(8);

  lte_rrc_csp_test_construct_basic_sib1();

  TF_MSG("Setting cell barring");
  sib1_pdu_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message*) 
                  lte_rrc_csp_test_get_sib1_pdu_ptr();
  TF_ASSERT(sib1_pdu_ptr != NULL);

  /* set cell barring */
  sib1_pdu_ptr->message.u.c1->u.systemInformationBlockType1->
    cellAccessRelatedInfo.cellBarred = 
    lte_rrc_osys_SystemInformationBlockType1_cellAccessRelatedInfo_cellBarred_barred;

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

  TF_MSG("Checking for LTE_RRC_SIB_ABORT_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_sib_abort_reqi_s) == buf_len);
  sib_abort_ptr = (lte_rrc_sib_abort_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SIB_ABORT_REQI == sib_abort_ptr->msg_hdr.id);

  TF_MSG("Checking for LTE_CPHY_CELL_BAR_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_cell_bar_req_s) == buf_len);
  cphy_cell_bar_req_ptr = (lte_cphy_cell_bar_req_s*)(void*) buf_ptr;
  TF_ASSERT(LTE_CPHY_CELL_BAR_REQ == cphy_cell_bar_req_ptr->msgr_hdr.id);
  TF_ASSERT(1 == cphy_cell_bar_req_ptr->bar_info.num_cells);
  TF_ASSERT(LTE_RAT_EUTRAN == cphy_cell_bar_req_ptr->bar_info.cell_info_list[0].rat_type );
  TF_ASSERT(300 == cphy_cell_bar_req_ptr->bar_info.t_barred );
  
  TF_MSG("Checking for LTE_RRC_IRAT_TO_LTE_RESEL_CNFI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_irat_to_lte_resel_cnfi_s) == buf_len);
  resel_cnfi_ptr = (lte_rrc_irat_to_lte_resel_cnfi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_IRAT_TO_LTE_RESEL_CNFI == resel_cnfi_ptr->msg_hdr.id);
  TF_ASSERT(E_FAILURE == resel_cnfi_ptr->status);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == 
            LTE_RRC_CSP_SM__INITIAL);

  lte_rrc_csp_test_free_sib1_pdu();

  TF_MSG("End of test");
}

void LteRrcCspIratToLteReselFailTest::Teardown()
{
  lte_rrc_utf_teardown();
}


#if 1
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCspIratToLteReselForbiddenTaFailTest);

void LteRrcCspIratToLteReselForbiddenTaFailTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CSP_SM,ext_umid_list,2);
  lte_rrc_csp_test_stub_init();
}

void LteRrcCspIratToLteReselForbiddenTaFailTest::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  lte_cphy_acq_req_s *acq_req_ptr;
  lte_rrc_get_sibs_reqi_s *sib_req_ptr;
  lte_rrc_irat_to_lte_resel_cnfi_s * resel_cnfi_ptr;
  lte_rrc_sib_abort_reqi_s *sib_abort_ptr;
  lte_cphy_cell_bar_req_s *cphy_cell_bar_req_ptr;
  lte_rrc_osys_BCCH_DL_SCH_Message *sib1_pdu_ptr;
  lte_rrc_csp_test_cfg_req_param_s			cfg_req_param;
  lte_rrc_csp_validate_system_scan_req_s  validate_system_scan_req[LTE_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN];;
  lte_rrc_csp_send_band_scan_cnf_s			send_band_scan_cnf;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == LTE_RRC_CSP_SM__INITIAL);

  lte_rrc_csp_test_stub_controller_set_access_class(8);

  lte_rrc_csp_test_construct_basic_sib1();

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
  sys_upd_req.forbidden_manual_ta_list.num_entries = 1;
  sys_upd_req.forbidden_manual_ta_list.tracking_area[0].tac = 1;
  sys_upd_req.forbidden_manual_ta_list.tracking_area[0].plmn.mcc[0] = 1;
  sys_upd_req.forbidden_manual_ta_list.tracking_area[0].plmn.mcc[1] = 2;
  sys_upd_req.forbidden_manual_ta_list.tracking_area[0].plmn.mcc[2] = 3;
  sys_upd_req.forbidden_manual_ta_list.tracking_area[0].plmn.num_mnc_digits = 3;
  sys_upd_req.forbidden_manual_ta_list.tracking_area[0].plmn.mnc[0] = 4;
  sys_upd_req.forbidden_manual_ta_list.tracking_area[0].plmn.mnc[1] = 5;
  sys_upd_req.forbidden_manual_ta_list.tracking_area[0].plmn.mnc[2] = 6;
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
  resel_reqi.network_select_mode = SYS_NETWORK_SELECTION_MODE_MANUAL;
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

  TF_MSG("Checking for LTE_RRC_SIB_ABORT_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_sib_abort_reqi_s) == buf_len);
  sib_abort_ptr = (lte_rrc_sib_abort_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SIB_ABORT_REQI == sib_abort_ptr->msg_hdr.id);


  TF_MSG("Checking for LTE_RRC_IRAT_TO_LTE_RESEL_CNFI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_irat_to_lte_resel_cnfi_s) == buf_len);
  resel_cnfi_ptr = (lte_rrc_irat_to_lte_resel_cnfi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_IRAT_TO_LTE_RESEL_CNFI == resel_cnfi_ptr->msg_hdr.id);
  TF_ASSERT(E_FAILURE == resel_cnfi_ptr->status);
  TF_ASSERT(LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_FORBIDDEN_TA == resel_cnfi_ptr->fail_cause);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == 
            LTE_RRC_CSP_SM__INITIAL);

  lte_rrc_csp_test_free_sib1_pdu();

  TF_MSG("End of test");
}

void LteRrcCspIratToLteReselForbiddenTaFailTest::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif
#if 0
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCspIratToLteReselForbiddenTaFailTest);

void LteRrcCspIratToLteReselForbiddenTaFailTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CSP_SM,ext_umid_list,3);
  lte_rrc_csp_test_stub_init();
}

void LteRrcCspIratToLteReselForbiddenTaFailTest::Test()
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
  lte_rrc_sib_abort_reqi_s *sib_abort_ptr;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == LTE_RRC_CSP_SM__INITIAL);

  lte_rrc_csp_test_stub_controller_set_access_class(8);

  lte_rrc_csp_test_construct_basic_sib1();

  //lte_rrc_utf_set_sync_cfg_reqi_params(LTE_RRC_CSP_TEST_CFG_TRANS_ID);

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
  //memset(&sys_upd_req.forbidden_ta_list,0,
         //sizeof(sys_upd_req.forbidden_ta_list));
  sys_upd_req.forbidden_ta_list.tracking_area[0].tac = 1;
  sys_upd_req.forbidden_ta_list.tracking_area[0].plmn.mcc[0] = 1;
  sys_upd_req.forbidden_ta_list.tracking_area[0].plmn.mcc[1] = 2;
  sys_upd_req.forbidden_ta_list.tracking_area[0].plmn.mcc[2] = 3;
  sys_upd_req.forbidden_ta_list.tracking_area[0].plmn.num_mnc_digits = 3;
  sys_upd_req.forbidden_ta_list.tracking_area[0].plmn.mcc[0] = 4;
  sys_upd_req.forbidden_ta_list.tracking_area[0].plmn.mcc[1] = 5;
  sys_upd_req.forbidden_ta_list.tracking_area[0].plmn.mcc[2] = 6;
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

  TF_MSG("Checking for LTE_CPHY_ACQ_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_acq_req_s) == buf_len);
  acq_req_ptr = (lte_cphy_acq_req_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_CPHY_ACQ_REQ == acq_req_ptr->msgr_hdr.id);
  TF_ASSERT(1 == acq_req_ptr->trans_id);
  TF_ASSERT(SYS_SBAND_LTE_EUTRAN_BAND1 == acq_req_ptr->band);
  TF_ASSERT(1 == acq_req_ptr->earfcn);
  TF_ASSERT(0 == acq_req_ptr->num_black_cells);
  TF_ASSERT(LTE_CPHY_ACQ_CELL == acq_req_ptr->acq_type);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for WT_ACQ_CNF state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == 
            LTE_RRC_CSP_SM__WT_ACQ_CNF);

  TF_MSG("Sending LTE_CPHY_ACQ_CNF");
  msgr_init_hdr(&acq_cnf.msgr_hdr,MSGR_LTE_CPHY,LTE_CPHY_ACQ_CNF);
  acq_cnf.trans_id = 1;
  acq_cnf.status = E_SUCCESS;
  acq_cnf.band = SYS_SBAND_LTE_EUTRAN_BAND1;
  acq_cnf.earfcn = 1;
  acq_cnf.acquired_cell.cp = LTE_L1_CP_MODE_NORMAL;
  memset(&acq_cnf.acquired_cell.mib,0,sizeof(acq_cnf.acquired_cell.mib));
  acq_cnf.acquired_cell.mib.dl_bandwidth = LTE_BW_NRB_6;
  acq_cnf.acquired_cell.phy_cell_id = 1;
  lte_rrc_utf_send_msg((byte*)&acq_cnf, sizeof(acq_cnf));

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


  TF_MSG("Checking for LTE_RRC_SIB_ABORT_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_sib_abort_reqi_s) == buf_len);
  sib_abort_ptr = (lte_rrc_sib_abort_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SIB_ABORT_REQI == sib_abort_ptr->msg_hdr.id);

  TF_MSG("Checking for LTE_RRC_IRAT_TO_LTE_RESEL_CNFI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_irat_to_lte_resel_cnfi_s) == buf_len);
  resel_cnfi_ptr = (lte_rrc_irat_to_lte_resel_cnfi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_IRAT_TO_LTE_RESEL_CNFI == resel_cnfi_ptr->msg_hdr.id);
  TF_ASSERT(E_FAILURE == resel_cnfi_ptr->status);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == 
            LTE_RRC_CSP_SM__INITIAL);

  lte_rrc_csp_test_free_sib1_pdu();

  TF_MSG("End of test");

}

void LteRrcCspIratToLteReselForbiddenTaFailTest::Teardown()
{
  lte_rrc_utf_teardown();
}

#endif
