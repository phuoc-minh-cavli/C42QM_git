/*!
  @file lte_rrc_llc_sib2_update_after_ho_rach_failure_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the handling of SIB2 update
  during HO RACH failure. The Common Config request caused by SIB2 update should 
  not be pended by LLC completes reverting back to source cell configuration. To 
  start with receives a Connection Setup followed by Connection Reconfiguration 
  message that sets up a UM Bi-directional DRB, before handover.

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

$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/llc/
lte_rrc_llc_sib2_update_after_ho_rach_failure_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/09   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_sib.h"
#include "lte_static_rrc_mem_util.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h>  

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcSib2UpdateAfterHORACHFailureTest);

void LteRrcLlcSib2UpdateAfterHORACHFailureTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcSib2UpdateAfterHORACHFailureTest::Test()
{
  lte_rrc_cfg_reqi_s common_cfg_req;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
  lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;
  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf_msg;
  lte_mac_access_abort_req_msg_s *mac_access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s mac_access_abort_cnf_msg;
  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;
  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;
  uint32 buf_len = 0, trans_id;

  TF_MSG("\nSIB2 update during HO RACH failure Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */
  encoded_pdu.length = sizeof(llc_reconfig_intra_freq_mobility_info_only_msg);
  encoded_pdu.value = llc_reconfig_intra_freq_mobility_info_only_msg;
  lte_rrc_llc_test_basic_sib2_update(); 
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Test HO msg with no DRB information */
  lte_rrc_llc_test_intra_freq_mobility_only(encoded_pdu, FALSE);

  /* At this point, send CFG_REQI (with cfg_type LTE_RRC_CFG_SRV_COMMON) to LLC */
  lte_rrc_llc_test_construct_common_cfg_reqi(&common_cfg_req, FALSE, TRUE, TRUE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&common_cfg_req, sizeof(common_cfg_req));
  lte_rrc_utf_wait_for_done();


  /* The above CFG_REQI should be ignored by LLC since HO completed indication
     has not been sent yet. CFG_CNFI with status E_NOT_ALLOWED should be returned */
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_NOT_ALLOWED, LTE_RRC_PROC_CSP, common_cfg_req.trans_id_ptr);

  /* Ensure LLC is still in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_RACH);
  // ---------------------------------------------

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  // ---------------------------------------------
  // Catch the ACCESS_ABORT_REQ to MAC 
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_access_abort_req_ptr,&buf_len);
  // Make sure the ACCESS_ABORT_REQ is valid 
  TF_ASSERT(sizeof(lte_mac_access_abort_req_msg_s) == buf_len);

  // Send ACCESS_ABORT_CNF to Config 
  // Setting the header for internal RRC messages 
  lte_rrc_init_default_hdr(&mac_access_abort_cnf_msg.hdr,LTE_MAC_ACCESS_ABORT_CNF);
  mac_access_abort_cnf_msg.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&mac_access_abort_cnf_msg, sizeof(mac_access_abort_cnf_msg));


 // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_LC_INFO_CFG_BM);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 3);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 0);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[2] == 10);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf_msg.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg.hdr, sizeof(mac_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req_ptr != NULL);
  TF_ASSERT(rlcdl_cfg_req_ptr->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_MODIFY);

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req_ptr != NULL);
  TF_ASSERT(rlcul_cfg_req_ptr->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_MODIFY);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpdl_cfg_req_ptr->hdr.id == LTE_PDCPDL_CFG_REQ);

  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg.hdr, sizeof(pdcpdl_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_req_ptr->hdr.id == LTE_PDCPUL_CFG_REQ);

  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg.hdr, sizeof(pdcpul_cfg_cnf_msg));

  
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  /* At this point, LLC should have reset ho_config_in_progress flag and should
    handle Common config request */
  /* Send CFG_REQI (with cfg_type LTE_RRC_CFG_SRV_COMMON) to LLC */
  lte_rrc_llc_test_construct_common_cfg_reqi(&common_cfg_req, FALSE, TRUE, TRUE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&common_cfg_req, sizeof(common_cfg_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  /* After CPHY Config req is sent, assert LLC is in WAIT_FOR_CPHY_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  TF_MSG("\nCPHY_DEDICATED_CFG_REQ rcvd and LLC Module in WAIT_FOR_CPHY_CFG_CNF state\n");

  /* send CPHY_COMMON_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send CPHY_COMMON_CFG_CNF with sucess \n");
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  TF_MSG("\nLLC: Rcvd CPHY_DEDICATED_CFG_REQ \n");
  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;
  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));

  TF_MSG("\nLLC: Rcvd MAC_CFG_REQ \n");
  lte_rrc_utf_get_next_ext_msg((byte **) &mac_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  /* LLC should be in WAIT_FOR_MAC_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);
  TF_MSG("\nMAC_CFG_REQ rcvd and LLC Module in WAIT_FOR_MAC_CFG_CNF state\n");

  /* send MAC_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send MAC_CFG_CNF with sucess \n");
  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf_msg, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg, sizeof(mac_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();

  
  
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CSP, common_cfg_req.trans_id_ptr);

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_sib_ptr();

  TF_MSG("\nSIB2 update during HO RACH failure Test Ends \n");

  lte_rrc_utf_reset(); 
}

void LteRrcLlcSib2UpdateAfterHORACHFailureTest::Teardown()
{
  lte_rrc_utf_teardown();
}









