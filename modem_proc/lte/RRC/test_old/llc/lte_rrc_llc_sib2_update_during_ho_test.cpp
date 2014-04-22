/*!
  @file lte_rrc_llc_sib2_update_during_ho_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the handling of SIB2 update
  during HO. The Common Config request caused by SIB2 update should not be
  pended by LLC until it sees the HO Completed indication from Config. To start 
  with receives a Connection Setup followed by Connection Reconfiguration message 
  that sets up a UM Bi-directional DRB, before handover.

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
lte_rrc_llc_sib2_update_during_ho_test.cpp#1 $

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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcSib2UpdateDuringHOTest);

void LteRrcLlcSib2UpdateDuringHOTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcSib2UpdateDuringHOTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_cfg_reqi_s common_cfg_req;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
  lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
  lte_cphy_handover_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_handover_cnf_s cphy_cfg_cnf_msg;
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
  lte_rrc_serving_cell_changed_indi_s *serving_cell_changed_ind_ptr = NULL;
  lte_rrc_handover_completed_ind_s ho_completed_ind_msg;
  uint32 buf_len = 0, trans_id;

  TF_MSG("\nSIB2 update during HO Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update(); 
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_intra_freq_mobility_info_only_msg);
  encoded_pdu.value = llc_reconfig_intra_freq_mobility_info_only_msg;

  /* On receiving a Config Req from Config, LLC should be in the INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  /* After PDCP Config prep req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCP Config Prep Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));
  /*--------------------------------------------------------------------------
                          MAC ACCESS ABORT Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the ACCESS_ABORT_REQ to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_access_abort_req_ptr,&buf_len);
  /* Make sure the ACCESS_ABORT_REQ is valid */
  TF_ASSERT(sizeof(lte_mac_access_abort_req_msg_s) == buf_len);

  /* Send ACCESS_ABORT_CNF to Config */
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&mac_access_abort_cnf_msg.hdr,LTE_MAC_ACCESS_ABORT_CNF);
  mac_access_abort_cnf_msg.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&mac_access_abort_cnf_msg, sizeof(mac_access_abort_cnf_msg));
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_handover_req_s) == buf_len);

  /* After CPHY Config req is sent, assert LLC is in WAIT_FOR_CPHY_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  TF_MSG("\nCPHY Config Request sent to CPHY and LLC Module in WAIT_FOR_CPHY_CFG_CNF state\n");

  /* Store the assigned tans_id for the first req */
  trans_id = lte_rrc_llc_test_common_data.trans_id[0];

  /* At this point, send CFG_REQI (with cfg_type LTE_RRC_CFG_SRV_COMMON) to LLC */
  TF_MSG("\nLLC: Send CFG_REQI for SIB2 Update \n");
  /* build CFG_REQI for COMMON CFG and send */
  lte_rrc_llc_test_construct_common_cfg_reqi(&common_cfg_req, FALSE, TRUE, TRUE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&common_cfg_req, sizeof(common_cfg_req));
  lte_rrc_utf_wait_for_done();

 
  /* The above CFG_REQI should be ignored by LLC and CFG_CNFI with status
    E_NOT_ALLOWED should be returned */
  /* Catch the CFG_CNFI message sent by LLC to Config */
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_NOT_ALLOWED, LTE_RRC_PROC_CSP, common_cfg_req.trans_id_ptr);

  /* Ensure LLC is still in WAIT_FOR_CPHY_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_cfg_cnf_msg.msgr_hdr,LTE_CPHY_HANDOVER_CNF);
  cphy_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_CPHY_HANDOVER_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf_msg, sizeof(cphy_cfg_cnf_msg));

  /* Catch the serving cell changed indication thrown by LLC */
  lte_rrc_utf_get_next_int_msg((byte **) &serving_cell_changed_ind_ptr,&buf_len);
  TF_ASSERT(serving_cell_changed_ind_ptr->msg_hdr.id == LTE_RRC_SERVING_CELL_CHANGED_INDI);
  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the MAC Config request sent by LLC to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_cfg_req_ptr,&buf_len);
  /* Make sure the MAC Config request is valid */
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);

  /* After MAC Config req is sent, assert LLC is in WAIT_FOR_MAC_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);
  TF_MSG("\nMAC Config Request sent to MAC and LLC Module in WAIT_FOR_MAC_CFG_CNF state\n");
  /* Construct and send the lte_mac_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr,LTE_MAC_CFG_CNF);
  /* Setting the fields in the LTE_MAC_CFG_CNF sent by MAC */
  mac_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_MAC_CFG_CNF to LLC there by mimicing the action of MAC */
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg, sizeof(mac_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);

  /* After RLCDL Config req is sent, assert LLC is in WAIT_FOR_RLC_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_RLC_CFG_CNF);
  TF_MSG("\nRLCDL Config Request sent to RLC and LLC Module in WAIT_FOR_RLC_CFG_CNF state\n");
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLCDL_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);

  /* After RLCUL Config req is sent, assert LLC is in WAIT_FOR_RLC_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_RLC_CFG_CNF);
  TF_MSG("\nRLCUL Config Request sent to RLC and LLC Module in WAIT_FOR_RLC_CFG_CNF state\n");
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLCUL_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);

  /* After PDCPDL Config req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCPDL Config Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);

  /* After PDCPUL Config req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCPUL Config Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));

  
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, &trans_id);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* At this point, send CFG_REQI (with cfg_type LTE_RRC_CFG_SRV_COMMON) to LLC */
  TF_MSG("\nLLC: Send CFG_REQI for SIB2 Update \n");
  /* build CFG_REQI for COMMON CFG and send */
  trans_id = LTE_RRC_INVALID_TRANS_ID;
  lte_rrc_llc_test_construct_common_cfg_reqi(&common_cfg_req, FALSE, TRUE, TRUE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&common_cfg_req, sizeof(common_cfg_req));
  lte_rrc_utf_wait_for_done();

  /* The above CFG_REQI should be ignored by LLC since HO completed indication
     has not been sent yet. CFG_CNFI with status E_NOT_ALLOWED should be returned */
  /* Catch the CFG_CNFI message sent by LLC to Config */
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_NOT_ALLOWED, LTE_RRC_PROC_CSP, common_cfg_req.trans_id_ptr);

  /* Ensure LLC is still in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  /* Send HO completed indication to LLC */
  lte_rrc_init_default_hdr(&ho_completed_ind_msg.msg_hdr,LTE_RRC_HANDOVER_COMPLETED_IND);
  ho_completed_ind_msg.phy_cell_id = 10;
  ho_completed_ind_msg.dl_freq = 10689;
  lte_rrc_utf_send_msg((byte*)&ho_completed_ind_msg.msg_hdr,sizeof(ho_completed_ind_msg));
  lte_rrc_utf_wait_for_done();
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* At this point, LLC should have reset ho_config_in_progress flag and should
    handle Common config request */
  /* At this point, send CFG_REQI (with cfg_type LTE_RRC_CFG_SRV_COMMON) to LLC */
  TF_MSG("\nLLC: Send CFG_REQI for SIB2 Update \n");
  /* build CFG_REQI for COMMON CFG and send */
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

  TF_MSG("\nSIB2 update during HO Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_utf_reset(); 
}

void LteRrcLlcSib2UpdateDuringHOTest::Teardown()
{
  lte_rrc_utf_teardown();
}








