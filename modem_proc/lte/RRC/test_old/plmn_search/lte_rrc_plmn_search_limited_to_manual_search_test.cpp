/*!
  @file lte_rrc_plmn_search_limited_to_manual_search_test.cpp

  @brief
  Unit test code for taking PLMN_SEARCH to camped state.
  Controller is in Inactive state and there is nothing in the Acq DB. Band
  Scan is started

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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
12/22/09   vatsac  Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include <lte_rrc_int_msg.h>
#include <lte_rrc_ext_msg.h>
#include <lte_cphy_msg.h>
#include <__lte_rrc_plmn_search.h>
#include <__lte_rrc_controller.h>
#include <lte_rrc_utils.h>
}

#include <TestFramework.h>
#include <lte_rrc_utf.h>
#include "lte_rrc_plmn_search_test_common.h"
#include "lte_rrc_test_integration_common.h"

//external messages generated by code under test
static uint32 ext_umid_list[] = 
{ 
  LTE_CPHY_BPLMN_START_REQ,
  LTE_CPHY_BPLMN_CELL_REQ,
  LTE_CPHY_BPLMN_STOP_REQ,
  LTE_RRC_PLMN_SEARCH_CNF
};

// Message structures
static lte_rrc_plmn_search_req_s search_req;
static lte_rrc_mode_change_cnfi_s mode_change_cnfi;
static lte_cphy_msg_bplmn_cell_cnf_s cell_cnf;
static lte_rrc_unsolicited_sib_indi_s unsolicited_sib_indi;
static lte_cphy_msg_bplmn_stop_cnf_s stop_cnf;
//static lte_cphy_msg_bplmn_complete_ind_s complete_ind;
//static lte_rrc_plmn_search_abort_req_s abort_req;
//static lte_rrc_stopped_indi_s stopped_indi;

//transaction ID to be returned from config request
#define LTE_RRC_PLMN_SEARCH_TEST_CFG_TRANS_ID 7

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPlmnSearchLimitedToManualSearchTest);

void LteRrcPlmnSearchLimitedToManualSearchTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_PLMN_SEARCH_SM,ext_umid_list,4);
  lte_rrc_plmn_search_test_stub_init();
}

void LteRrcPlmnSearchLimitedToManualSearchTest::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  lte_rrc_mode_change_reqi_s *mode_change_req_ptr;
  lte_cphy_msg_bplmn_start_req_s *start_req_ptr;
  lte_cphy_msg_bplmn_cell_req_s *cell_req_ptr;
  lte_rrc_plmn_search_cnf_s *search_cnf_ptr;
  lte_cphy_msg_bplmn_stop_req_s *stop_req_ptr;
  
  lte_rrc_osys_BCCH_DL_SCH_Message *sib1_pdu_ptr;
  lte_rrc_pdu_buf_s encoded_msg_ptr;

  int encode_status = 0;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_PLMN_SEARCH_SM) == LTE_RRC_PLMN_SEARCH_SM__INITIAL);

  lte_rrc_plmn_search_test_stub_controller_set_state(LTE_RRC_CONTROLLER_SM__INACTIVE);

  lte_rrc_plmn_search_test_stub_controller_set_access_class(8);

  lte_rrc_plmn_search_test_construct_basic_sib1();

  lte_rrc_utf_set_sync_cfg_reqi_params(LTE_RRC_PLMN_SEARCH_TEST_CFG_TRANS_ID);

  TF_MSG("Sending Service Req");
  msgr_init_hdr(&search_req.msg_hdr, MSGR_NAS_EMM, LTE_RRC_PLMN_SEARCH_REQ);
  search_req.trans_id = 1;
  search_req.network_select_mode = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;
  search_req.search_type = SYS_SERVICE_SEARCH_BACKGROUND_MANUAL;
  memset(&search_req.rat_pri_list,0,sizeof(search_req.rat_pri_list));
  search_req.rat_pri_list.num_items = 1;
  search_req.rat_pri_list.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_LTE;
  search_req.rat_pri_list.priority_list_info[0].band_cap.lte_band_cap.bits_1_64 |= SYS_BAND_MASK_LTE_BAND1;
  search_req.plmn_rat_list.length = 1;
  search_req.plmn_rat_list.info[0].plmn.identity[0] = 0x21;
  search_req.plmn_rat_list.info[0].plmn.identity[1] = 0x63;
  search_req.plmn_rat_list.info[0].plmn.identity[2] = 0x54;
  search_req.plmn_rat_list.info[0].rat = SYS_RAT_LTE_RADIO_ACCESS;
  search_req.rplmn_info_is_valid = TRUE;
  search_req.rplmn.mcc[0] = 7;
  search_req.rplmn.mcc[1] = 8;
  search_req.rplmn.mcc[2] = 9;
  search_req.rplmn.num_mnc_digits = 3;
  search_req.rplmn.mnc[0] = 10;
  search_req.rplmn.mnc[1] = 11;
  search_req.rplmn.mnc[2] = 12;
  search_req.hplmn_info_is_valid = TRUE;
  search_req.hplmn.mcc[0] = 13;
  search_req.hplmn.mcc[1] = 14;
  search_req.hplmn.mcc[2] = 15;
  search_req.hplmn.num_mnc_digits = 3;
  search_req.hplmn.mnc[0] = 16;
  search_req.hplmn.mnc[1] = 17;
  search_req.hplmn.mnc[2] = 18;
  search_req.ehplmn_list.num_plmns = 1;
  search_req.ehplmn_list.plmn[0].mcc[0] = 19;
  search_req.ehplmn_list.plmn[0].mcc[1] = 20;
  search_req.ehplmn_list.plmn[0].mcc[2] = 21;
  search_req.ehplmn_list.plmn[0].num_mnc_digits = 3;
  search_req.ehplmn_list.plmn[0].mnc[0] = 22;
  search_req.ehplmn_list.plmn[0].mnc[1] = 23;
  search_req.ehplmn_list.plmn[0].mnc[2] = 24;
  memset(&search_req.eplmn_list,0,sizeof(search_req.eplmn_list));
  memset(&search_req.forbidden_ta_list,0,
         sizeof(search_req.forbidden_ta_list));
  lte_rrc_utf_send_msg((byte*)&search_req, sizeof(search_req));

  TF_MSG("Checking for LTE_RRC_MODE_CHANGE_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_mode_change_reqi_s) == buf_len);
  mode_change_req_ptr = (lte_rrc_mode_change_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_MODE_CHANGE_REQI == mode_change_req_ptr->msg_hdr.id);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for WT_MODE_CHANGE_CNF state");
  TF_ASSERT(stm_get_state(LTE_RRC_PLMN_SEARCH_SM) == 
            LTE_RRC_PLMN_SEARCH_SM__WT_MODE_CHANGE_CNF);

  lte_rrc_plmn_search_test_stub_controller_set_state(LTE_RRC_CONTROLLER_SM__IDLE_NOT_CAMPED);
 
  TF_MSG("Sending Mode Change Cnf");
  lte_rrc_init_default_hdr(&mode_change_cnfi.msg_hdr,LTE_RRC_MODE_CHANGE_CNFI);
  mode_change_cnfi.status = TRUE;
  lte_rrc_utf_send_msg((byte*)&mode_change_cnfi, sizeof(mode_change_cnfi));

  TF_MSG("Checking for LTE_CPHY_BPLMN_START_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_msg_bplmn_start_req_s) == buf_len);
  start_req_ptr = (lte_cphy_msg_bplmn_start_req_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_CPHY_BPLMN_START_REQ == start_req_ptr->msgr_hdr.id);
  TF_ASSERT(start_req_ptr->new_req == TRUE);
  TF_ASSERT(start_req_ptr->req_type == LTE_CPHY_BPLMN_TYPE_LTE_LTE);
  TF_ASSERT(start_req_ptr->start_now == TRUE);
  TF_ASSERT(start_req_ptr->num_chans == 0);
  TF_ASSERT(start_req_ptr->num_bands == 1);
  TF_ASSERT(start_req_ptr->bands[0] == SYS_SBAND_LTE_EUTRAN_BAND1);
  TF_ASSERT(start_req_ptr->num_black_freqs == 0);
  TF_ASSERT(start_req_ptr->irat_lte_plmn_time_allowed == LTE_CPHY_BPLMN_UNLIMITED_TIME);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for WT_SIB1S_AND_CELL_SELECT state");
  TF_ASSERT(stm_get_state(LTE_RRC_PLMN_SEARCH_SM) == 
            LTE_RRC_PLMN_SEARCH_SM__WT_SIB1S_AND_CELL_SELECT);

  TF_MSG("Get 1st SIB1");
  sib1_pdu_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message*)lte_rrc_plmn_search_test_get_sib1_pdu_ptr();
  TF_ASSERT(sib1_pdu_ptr != NULL);

  /* set PLMN mismatch */
  sib1_pdu_ptr->message.u.c1->u.systemInformationBlockType1->
      cellAccessRelatedInfo.plmn_IdentityList.elem->plmn_Identity.mcc.
      elem[0] = 2;

  encode_status = lte_rrc_osys_asn1_encode_pdu(sib1_pdu_ptr, &encoded_msg_ptr,
                               BCCH_DL_SCH_Message);

  TF_MSG("Sending LTE_RRC_UNSOLICITED_SIB_INDI");
  lte_rrc_init_default_hdr(&unsolicited_sib_indi.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);
  unsolicited_sib_indi.cell_id.freq = 1;
  unsolicited_sib_indi.cell_id.phy_cell_id = 1;
  memcpy(unsolicited_sib_indi.encoded_si, encoded_msg_ptr.value, encoded_msg_ptr.length);
  unsolicited_sib_indi.encoded_si_size = encoded_msg_ptr.length;
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_indi, sizeof(unsolicited_sib_indi));

  TF_MSG("Checking for LTE_CPHY_BPLMN_CELL_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_msg_bplmn_cell_req_s) == buf_len);
  cell_req_ptr = (lte_cphy_msg_bplmn_cell_req_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_CPHY_BPLMN_CELL_REQ == cell_req_ptr->msgr_hdr.id);
  TF_ASSERT(cell_req_ptr->freq == 1);
  TF_ASSERT(cell_req_ptr->cell_id == 1);
  TF_ASSERT(cell_req_ptr->barred_status == LTE_CPHY_BPLMN_NOT_BARRED);
  TF_ASSERT(cell_req_ptr->cell_select_cfg.q_rxlevmin == -140);
  TF_ASSERT(cell_req_ptr->cell_select_cfg.q_rxlevmin_offset == 0);
  TF_ASSERT(cell_req_ptr->cell_select_cfg.p_max == LTE_DEFAULT_PMAX);

  TF_MSG("Sending LTE_CPHY_BPLMN_CELL_CNF");
  msgr_init_hdr(&cell_cnf.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_BPLMN_CELL_CNF);
  cell_cnf.earfcn = 1; cell_cnf.freq = 1;
  cell_cnf.cell_id = 1;
  cell_cnf.bw = LTE_BW_NRB_100;
  cell_cnf.rsrp = -110; //LTE_RRC_CSP_HIGH_CELL_QUALITY_THRESHOLD;
  cell_cnf.srxlev_calculated = TRUE;
  cell_cnf.srxlev = 0;  cell_cnf.squal = 1;
  lte_rrc_utf_send_msg((byte*)&cell_cnf, sizeof(cell_cnf));

  TF_MSG("Get 2nd SIB1");
  sib1_pdu_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message*)lte_rrc_plmn_search_test_get_sib1_pdu_ptr();
  TF_ASSERT(sib1_pdu_ptr != NULL);

  /* set PLMN match */
  sib1_pdu_ptr->message.u.c1->u.systemInformationBlockType1->
      cellAccessRelatedInfo.plmn_IdentityList.elem->plmn_Identity.mcc.
      elem[0] = 1;

  encode_status = lte_rrc_osys_asn1_encode_pdu(sib1_pdu_ptr, &encoded_msg_ptr,
                               BCCH_DL_SCH_Message);

  TF_MSG("Sending LTE_RRC_UNSOLICITED_SIB_INDI");
  lte_rrc_init_default_hdr(&unsolicited_sib_indi.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);
  unsolicited_sib_indi.cell_id.freq = 1;
  unsolicited_sib_indi.cell_id.phy_cell_id = 2;
  memcpy(unsolicited_sib_indi.encoded_si, encoded_msg_ptr.value, encoded_msg_ptr.length);
  unsolicited_sib_indi.encoded_si_size = encoded_msg_ptr.length;
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_indi, sizeof(unsolicited_sib_indi));

  TF_MSG("Checking for LTE_CPHY_BPLMN_CELL_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_msg_bplmn_cell_req_s) == buf_len);
  cell_req_ptr = (lte_cphy_msg_bplmn_cell_req_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_CPHY_BPLMN_CELL_REQ == cell_req_ptr->msgr_hdr.id);
  TF_ASSERT(cell_req_ptr->freq == 1);
  TF_ASSERT(cell_req_ptr->cell_id == 2);
  TF_ASSERT(cell_req_ptr->barred_status == LTE_CPHY_BPLMN_NOT_BARRED);
  TF_ASSERT(cell_req_ptr->cell_select_cfg.q_rxlevmin == -140);
  TF_ASSERT(cell_req_ptr->cell_select_cfg.q_rxlevmin_offset == 0);
  TF_ASSERT(cell_req_ptr->cell_select_cfg.p_max == LTE_DEFAULT_PMAX);

  TF_MSG("Sending LTE_CPHY_BPLMN_CELL_CNF");
  msgr_init_hdr(&cell_cnf.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_BPLMN_CELL_CNF);
  cell_cnf.earfcn = 1; cell_cnf.freq = 1;
  cell_cnf.cell_id = 2;
  cell_cnf.bw = LTE_BW_NRB_100;
  cell_cnf.rsrp = -110; //LTE_RRC_CSP_HIGH_CELL_QUALITY_THRESHOLD;
  cell_cnf.srxlev_calculated = TRUE;
  cell_cnf.srxlev = 0;  cell_cnf.squal = 1;
  lte_rrc_utf_send_msg((byte*)&cell_cnf, sizeof(cell_cnf));

  TF_MSG("Checking for LTE_CPHY_BPLMN_STOP_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_msg_bplmn_stop_req_s) == buf_len);
  stop_req_ptr = (lte_cphy_msg_bplmn_stop_req_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_CPHY_BPLMN_STOP_REQ == stop_req_ptr->msgr_hdr.id);

  TF_MSG("Sending LTE_CPHY_BPLMN_STOP_CNF");
  msgr_init_hdr(&stop_cnf.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_BPLMN_STOP_CNF);
  stop_cnf.status = TRUE;
  lte_rrc_utf_send_msg((byte*)&stop_cnf, sizeof(stop_cnf));

  TF_MSG("Checking for LTE_RRC_PLMN_SEARCH_CNF");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_plmn_search_cnf_s) == buf_len);
  search_cnf_ptr = (lte_rrc_plmn_search_cnf_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_PLMN_SEARCH_CNF == search_cnf_ptr->msg_hdr.id);
  TF_ASSERT(search_cnf_ptr->trans_id == 1);
  TF_ASSERT(search_cnf_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC);
  TF_ASSERT(search_cnf_ptr->search_type == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL);
  TF_ASSERT(search_cnf_ptr->status == LTE_RRC_PLMN_SEARCH_COMPLETED);
  TF_ASSERT(search_cnf_ptr->found_plmn_rat_list.length == 1);
  TF_ASSERT(search_cnf_ptr->found_plmn_rat_list.info[0].plmn.identity[0] == 0x21);
  TF_ASSERT(search_cnf_ptr->found_plmn_rat_list.info[0].plmn.identity[1] == 0x63);
  TF_ASSERT(search_cnf_ptr->found_plmn_rat_list.info[0].plmn.identity[2] == 0x54);
  TF_ASSERT(search_cnf_ptr->found_plmn_rat_list.info[0].rat == SYS_RAT_LTE_RADIO_ACCESS);
  TF_ASSERT(search_cnf_ptr->found_plmn_rat_list.info[0].plmn_service_capability == SYS_SRV_CAPABILITY_PS_SERVICE_ONLY);
  TF_ASSERT(search_cnf_ptr->found_plmn_rat_list.info[0].plmn_forbidden == FALSE);
  TF_ASSERT(search_cnf_ptr->found_plmn_rat_list.info[0].list_category == SYS_DETAILED_PLMN_LIST_CATEGORY_OTHER);
  TF_ASSERT(search_cnf_ptr->found_plmn_rat_list.info[0].signal_quality == SYS_SIGNAL_QUALITY_HIGH);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_PLMN_SEARCH_SM) == 
            LTE_RRC_PLMN_SEARCH_SM__INITIAL);

  lte_rrc_plmn_search_test_free_sib1_pdu();

  TF_MSG("End of test");
}

void LteRrcPlmnSearchLimitedToManualSearchTest::Teardown()
{
  lte_rrc_utf_teardown();
}

void validate_bplmn_start_req_manual(lte_rrc_utf_rrc2ext_msg_u* rrc_req_msg)
{
	TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.new_req == TRUE);
	TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.req_type == LTE_CPHY_BPLMN_TYPE_LTE_LTE);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.start_now == TRUE);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.num_chans == 0);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.num_bands == 1);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.bands[0] == SYS_SBAND_LTE_EUTRAN_BAND1);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.num_black_freqs == 0);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.irat_lte_plmn_time_allowed == LTE_CPHY_BPLMN_UNLIMITED_TIME);
}

void populate_plmn_search_req_manual()
{
	lte_rrc_plmn_search_req_s* plmn_search_req;
	plmn_search_req = lte_rrc_test_get_plmn_search_req_ptr();
	
	plmn_search_req -> search_type = SYS_SERVICE_SEARCH_BACKGROUND_MANUAL;
	//rat_pri_list.priority_list_info[0].band_cap.lte_band_cap.bits_1_64 = SYS_BAND_MASK_LTE_BAND40;
	plmn_search_req -> forbidden_ta_list.num_entries = 1;
	plmn_search_req -> forbidden_ta_list.tracking_area[0].tac = 1;
	plmn_search_req -> forbidden_ta_list.tracking_area[0].plmn.mcc[0] = 9;
	plmn_search_req -> forbidden_ta_list.tracking_area[0].plmn.mcc[1] = 9;
	plmn_search_req -> forbidden_ta_list.tracking_area[0].plmn.mcc[2] = 9;
	plmn_search_req -> forbidden_ta_list.tracking_area[0].plmn.num_mnc_digits = 3;
	plmn_search_req -> forbidden_ta_list.tracking_area[0].plmn.mnc[0] = 9;
	plmn_search_req -> forbidden_ta_list.tracking_area[0].plmn.mnc[1] = 9;
	plmn_search_req -> forbidden_ta_list.tracking_area[0].plmn.mnc[2] = 9;

}

void validate_bplmn_cell_req_manual(lte_rrc_utf_rrc2ext_msg_u* rrc_req_msg)
{
	TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_cell_req.barred_status == LTE_CPHY_BPLMN_NOT_BARRED);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_cell_req.cell_select_cfg.q_rxlevmin == -140);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_cell_req.cell_select_cfg.q_rxlevmin_offset == 0);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_cell_req.cell_select_cfg.p_max == LTE_DEFAULT_PMAX);

}

void validate_plmn_srch_cnf_manual(lte_rrc_utf_rrc2ext_msg_u* rrc_req_msg)
{
	TF_ASSERT(rrc_req_msg ->lte_rrc_plmn_search_cnf.trans_id == 1);
	TF_ASSERT(rrc_req_msg ->lte_rrc_plmn_search_cnf.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC);
	TF_ASSERT(rrc_req_msg ->lte_rrc_plmn_search_cnf.search_type == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL);
	TF_ASSERT(rrc_req_msg ->lte_rrc_plmn_search_cnf.status == LTE_RRC_PLMN_SEARCH_COMPLETED);
	TF_ASSERT(rrc_req_msg ->lte_rrc_plmn_search_cnf.found_plmn_rat_list.length == 1);
	TF_ASSERT(rrc_req_msg ->lte_rrc_plmn_search_cnf.found_plmn_rat_list.info[0].plmn.identity[0] == 0x21);
	TF_ASSERT(rrc_req_msg ->lte_rrc_plmn_search_cnf.found_plmn_rat_list.info[0].plmn.identity[1] == 0x63);
	TF_ASSERT(rrc_req_msg ->lte_rrc_plmn_search_cnf.found_plmn_rat_list.info[0].plmn.identity[2] == 0x54);
	TF_ASSERT(rrc_req_msg ->lte_rrc_plmn_search_cnf.found_plmn_rat_list.info[0].rat == SYS_RAT_LTE_RADIO_ACCESS);
	TF_ASSERT(rrc_req_msg ->lte_rrc_plmn_search_cnf.found_plmn_rat_list.info[0].plmn_service_capability == SYS_SRV_CAPABILITY_PS_SERVICE_ONLY);
	TF_ASSERT(rrc_req_msg ->lte_rrc_plmn_search_cnf.found_plmn_rat_list.info[0].plmn_forbidden == FALSE);
	TF_ASSERT(rrc_req_msg ->lte_rrc_plmn_search_cnf.found_plmn_rat_list.info[0].list_category == SYS_DETAILED_PLMN_LIST_CATEGORY_OTHER);
	TF_ASSERT(rrc_req_msg ->lte_rrc_plmn_search_cnf.found_plmn_rat_list.info[0].signal_quality == SYS_SIGNAL_QUALITY_HIGH);
	//TF_ASSERT(rrc_req_msg ->lte_rrc_plmn_search_cnf.found_plmn_rat_list.info[0].signal_strength == 110);
}

void populate_plmn_search_req1_manual()
{
	lte_rrc_plmn_search_req_s* plmn_search_req;
	plmn_search_req = lte_rrc_test_get_plmn_search_req_ptr();
	
	plmn_search_req -> forbidden_ta_list.num_entries = 0;

}

void validate_bplmn_start_req1_manual(lte_rrc_utf_rrc2ext_msg_u* rrc_req_msg)
{
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.new_req == TRUE);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.req_type == LTE_CPHY_BPLMN_TYPE_LTE_LTE);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.start_now == TRUE);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.num_chans == 1);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.num_bands == 1);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.chans[0].chan == 1);
 //TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.chans[0].bw == LTE_BW_NRB_100);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.bands[0] == SYS_SBAND_LTE_EUTRAN_BAND1);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.num_black_freqs == 0);
 TF_ASSERT(rrc_req_msg->lte_cphy_msg_bplmn_start_req.irat_lte_plmn_time_allowed == LTE_CPHY_BPLMN_UNLIMITED_TIME);
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPlmnSearchLimitedToManualSearchTestForbiddenTest);

void LteRrcPlmnSearchLimitedToManualSearchTestForbiddenTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_PLMN_SEARCH_SM,ext_umid_list,4);
  lte_rrc_plmn_search_test_stub_init();
  lte_rrc_test_integration_common_init_all( UNIT );
}

void LteRrcPlmnSearchLimitedToManualSearchTestForbiddenTest::Test()
{
  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_PLMN_SEARCH_SM) == LTE_RRC_PLMN_SEARCH_SM__INITIAL);

  lte_rrc_plmn_search_test_stub_controller_set_state(LTE_RRC_CONTROLLER_SM__INACTIVE);

  lte_rrc_plmn_search_test_stub_controller_set_access_class(8);

  lte_rrc_utf_set_sync_cfg_reqi_params(LTE_RRC_PLMN_SEARCH_TEST_CFG_TRANS_ID);

  lte_rrc_test_integration_common_send_umid (LTE_RRC_PLMN_SEARCH_REQ, &populate_plmn_search_req_manual, LTE_RRC_TEST_MAX_CELL, LTE_RRC_TEST_MAX_SIB_UPDATE, 0, FALSE);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for WT_MODE_CHANGE_CNF state");
  TF_ASSERT(stm_get_state(LTE_RRC_PLMN_SEARCH_SM) == 
            LTE_RRC_PLMN_SEARCH_SM__WT_MODE_CHANGE_CNF);

  lte_rrc_test_integration_verify_req_send_cnf (NULL, LTE_RRC_MODE_CHANGE_REQI, LTE_RRC_TEST_CELL_0, TRUE, NULL);

  lte_rrc_plmn_search_test_stub_controller_set_state(LTE_RRC_CONTROLLER_SM__IDLE_NOT_CAMPED);

  TF_MSG("Checking for LTE_CPHY_BPLMN_START_REQ");

  lte_rrc_test_integration_verify_req_send_cnf (&validate_bplmn_start_req_manual, LTE_CPHY_BPLMN_START_REQ, LTE_RRC_TEST_CELL_0, FALSE, NULL);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for WT_SIB1S_AND_CELL_SELECT state");
  TF_ASSERT(stm_get_state(LTE_RRC_PLMN_SEARCH_SM) == 
            LTE_RRC_PLMN_SEARCH_SM__WT_SIB1S_AND_CELL_SELECT);


  lte_rrc_test_common_set_sib1_indi_plmn_mcc_mnc(0x223456);
  lte_rrc_test_integration_common_send_umid (LTE_RRC_UNSOLICITED_SIB_INDI, NULL, LTE_RRC_TEST_CELL_0, LTE_RRC_TEST_SIB_UPDATE_0, 0, FALSE);

  lte_rrc_test_integration_verify_req_send_cnf (&validate_bplmn_cell_req_manual, LTE_CPHY_BPLMN_CELL_REQ, LTE_RRC_TEST_CELL_0, TRUE, NULL);

  lte_rrc_test_common_set_sib1_indi_plmn_mcc_mnc(0x123456);
  lte_rrc_test_integration_common_send_umid (LTE_RRC_UNSOLICITED_SIB_INDI, NULL, LTE_RRC_TEST_CELL_1, LTE_RRC_TEST_SIB_UPDATE_0, 0, FALSE);

  lte_rrc_test_integration_verify_req_send_cnf (&validate_bplmn_cell_req_manual, LTE_CPHY_BPLMN_CELL_REQ, LTE_RRC_TEST_CELL_1, TRUE, NULL);


  lte_rrc_test_integration_verify_req_send_cnf (NULL, LTE_CPHY_BPLMN_STOP_REQ, LTE_RRC_TEST_MAX_CELL, TRUE, NULL);

  lte_rrc_test_integration_verify_req_send_cnf (&validate_plmn_srch_cnf_manual, LTE_RRC_PLMN_SEARCH_CNF, LTE_RRC_TEST_MAX_CELL, FALSE, NULL);

  lte_rrc_test_integration_verify_req_send_cnf (NULL, LTE_RRC_PLMN_SEARCH_CNFI, LTE_RRC_TEST_MAX_CELL, FALSE, NULL);
  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_PLMN_SEARCH_SM) == 
            LTE_RRC_PLMN_SEARCH_SM__INITIAL);

  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_plmn_search_test_stub_controller_set_state(LTE_RRC_CONTROLLER_SM__INACTIVE);

  lte_rrc_plmn_search_test_stub_controller_set_access_class(8);

  lte_rrc_utf_set_sync_cfg_reqi_params(LTE_RRC_PLMN_SEARCH_TEST_CFG_TRANS_ID);

  lte_rrc_test_integration_common_send_umid (LTE_RRC_PLMN_SEARCH_REQ, &populate_plmn_search_req1_manual, LTE_RRC_TEST_MAX_CELL, LTE_RRC_TEST_MAX_SIB_UPDATE, 0, FALSE);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for WT_MODE_CHANGE_CNF state");
  TF_ASSERT(stm_get_state(LTE_RRC_PLMN_SEARCH_SM) == 
			LTE_RRC_PLMN_SEARCH_SM__WT_MODE_CHANGE_CNF);

  lte_rrc_test_integration_verify_req_send_cnf (NULL, LTE_RRC_MODE_CHANGE_REQI, LTE_RRC_TEST_CELL_0, TRUE, NULL);

  lte_rrc_plmn_search_test_stub_controller_set_state(LTE_RRC_CONTROLLER_SM__IDLE_NOT_CAMPED);

  TF_MSG("Checking for LTE_CPHY_BPLMN_START_REQ");

  lte_rrc_test_integration_verify_req_send_cnf (&validate_bplmn_start_req1_manual, LTE_CPHY_BPLMN_START_REQ, LTE_RRC_TEST_CELL_0, FALSE, NULL);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for WT_SIB1S_AND_CELL_SELECT state");
  TF_ASSERT(stm_get_state(LTE_RRC_PLMN_SEARCH_SM) == 
			LTE_RRC_PLMN_SEARCH_SM__WT_SIB1S_AND_CELL_SELECT);

  lte_rrc_test_common_set_sib1_indi_tac(1);
  lte_rrc_test_common_set_sib1_indi_plmn_mcc_mnc(0x999999);
  lte_rrc_test_integration_common_send_umid (LTE_RRC_UNSOLICITED_SIB_INDI, NULL, LTE_RRC_TEST_CELL_0, LTE_RRC_TEST_SIB_UPDATE_0, 0, FALSE);

  lte_rrc_test_integration_verify_req_send_cnf (&validate_bplmn_cell_req_manual, LTE_CPHY_BPLMN_CELL_REQ, LTE_RRC_TEST_CELL_0, TRUE, NULL);

  lte_rrc_test_common_set_sib1_indi_plmn_mcc_mnc(0x123456);
  lte_rrc_test_integration_common_send_umid (LTE_RRC_UNSOLICITED_SIB_INDI, NULL, LTE_RRC_TEST_CELL_1, LTE_RRC_TEST_SIB_UPDATE_0, 0, FALSE);

  lte_rrc_test_integration_verify_req_send_cnf (&validate_bplmn_cell_req_manual, LTE_CPHY_BPLMN_CELL_REQ, LTE_RRC_TEST_CELL_1, TRUE, NULL);


  lte_rrc_test_integration_verify_req_send_cnf (NULL, LTE_CPHY_BPLMN_STOP_REQ, LTE_RRC_TEST_MAX_CELL, TRUE, NULL);

  lte_rrc_test_integration_verify_req_send_cnf (&validate_plmn_srch_cnf_manual, LTE_RRC_PLMN_SEARCH_CNF, LTE_RRC_TEST_MAX_CELL, FALSE, NULL);

    lte_rrc_test_integration_verify_req_send_cnf (NULL, LTE_RRC_PLMN_SEARCH_CNFI, LTE_RRC_TEST_MAX_CELL, FALSE, NULL);


  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_PLMN_SEARCH_SM) == 
			LTE_RRC_PLMN_SEARCH_SM__INITIAL);

   lte_rrc_utf_check_no_more_msgs();

  TF_MSG("End of test");
}

void LteRrcPlmnSearchLimitedToManualSearchTestForbiddenTest::Teardown()
{
	lte_rrc_test_integration_common_deinit_all();
  lte_rrc_utf_teardown();
}

