/*!
  @file lte_rrc_llc_ho_with_rlc_sn_modify_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of a reconfig 
  message with mobility that changes the SN length in RLC configuration of a UM RB 
  that is already established.

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

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/09   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_sib.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h>  	

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcHOWithRLCSNModifyTest);

void LteRrcLlcHOWithRLCSNModifyTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcHOWithRLCSNModifyTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr;
  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;
  lte_mac_access_abort_req_msg_s *mac_access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s mac_access_abort_cnf_msg;
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;
  int decode_status = 0;
  uint32 buf_len = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Reconfig with RLC SN length change Test Begins \n");
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_phy_mac_drb3_rlc_msg);
  encoded_pdu.value = llc_reconfig_phy_mac_drb3_rlc_msg;
  
  /* On receiving a Config Req from Config, LLC should be in the INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Construct asn1 pdu to be sent to LLC module after decoding the encoded pdu*/
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);

  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* Modify the SN length in RLC configuration. Since an existing message is being
    reused, the PHY and MAC Config bitmasks in the message are turned off */
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.m.
    physicalConfigDedicatedPresent = 1;
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.m.
    mac_MainConfigPresent = 1;
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->rlc_Config.t = T_lte_rrc_osys_RLC_Config_um_Bi_Directional;
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->rlc_Config.u.um_Bi_Directional->dl_UM_RLC.
    sn_FieldLength = lte_rrc_osys_SN_FieldLength_size10;
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = dl_dcch_msg_ptr;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  TF_MSG("\nInvalid Reconfig with RLC SN length change Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();

  TF_MSG("\nHO with RLC SN length modify Test Begins \n");

  encoded_pdu.length = sizeof(llc_invalid_ho_with_pdcp_modify_msg);
  encoded_pdu.value = llc_invalid_ho_with_pdcp_modify_msg;
  
  /* On receiving a Config Req from Config, LLC should be in the INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Construct asn1 pdu to be sent to LLC module after decoding the encoded pdu*/
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* Modify the SN length in RLC configuration. Since an existing message is being
    reused, the PDCP bitmask in the message is turned off, RLC bitmask is turned on.*/
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->m.pdcp_ConfigPresent = 0;
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->m.rlc_ConfigPresent = 1;
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->rlc_Config.t = T_lte_rrc_osys_RLC_Config_um_Bi_Directional;

  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->rlc_Config.u.um_Bi_Directional = 
    (lte_rrc_osys_RLC_Config_um_Bi_Directional *) 
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_RLC_Config_um_Bi_Directional));

  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->rlc_Config.u.um_Bi_Directional->dl_UM_RLC.
    sn_FieldLength = lte_rrc_osys_SN_FieldLength_size10;
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->rlc_Config.u.um_Bi_Directional->ul_UM_RLC.
    sn_FieldLength = lte_rrc_osys_SN_FieldLength_size10;
  /* Mask off TDD config that is present */
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo.radioResourceConfigCommon
    .m.tdd_ConfigPresent = 0;
  
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = dl_dcch_msg_ptr;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));

  /* Catch the ACCESS_ABORT_REQ to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_access_abort_req_ptr,&buf_len);
  /* Make sure the ACCESS_ABORT_REQ is valid */
  TF_ASSERT(sizeof(lte_mac_access_abort_req_msg_s) == buf_len);

  /* Send ACCESS_ABORT_CNF to Config */
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&mac_access_abort_cnf_msg.hdr,LTE_MAC_ACCESS_ABORT_CNF);
  mac_access_abort_cnf_msg.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&mac_access_abort_cnf_msg, sizeof(mac_access_abort_cnf_msg));

  /* Verify phy handover request parameters when handover succeeds (TRUE) */
  lte_rrc_llc_test_verify_phy_handover_cfg_req_and_send_cnf(TRUE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config request */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));

  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);
  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nHO with RLC SN length modify Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcHOWithRLCSNModifyTest::Teardown()
{
  lte_rrc_utf_teardown();
}
