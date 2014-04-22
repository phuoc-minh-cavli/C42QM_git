/*!
  @file lte_rrc_llc_antennaInfo_cfg_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the handling of AntennaInfo cfg in
  connection setup, reconfiguration, handover and out of sync scenarios.

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

$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/llc/lte_rrc_llc_antennaInfo_cfg_test.cpp

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/14   ka     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
  #include "lte_rrc_sib.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h>  

extern "C"
{
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s*);
  TF_PROTO_STUB(boolean, lte_rrc_config_is_first_reconfig_after_cre, void);
  
  extern lte_rrc_llc_s lte_rrc_llc_data;
}


boolean reconfig_after_cre1 = FALSE;

boolean lte_rrc_llc_test_reconfig_after_cre1
(
  void
)
{
  if (reconfig_after_cre1)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

//AntennaInfo Rel10 to Rel8 during HO and HO fails
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcAntennaCfgR10toR8duringHOinvalidTest);

void LteRrcLlcAntennaCfgR10toR8duringHOinvalidTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel11;
}

void LteRrcLlcAntennaCfgR10toR8duringHOinvalidTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  int decode_status=0;

  TF_MSG("\nLLC AntennaInfo config test begins \n");
  lte_rrc_llc_test_basic_sib2_update();

  /* Sending a Conn setup that configures periodic CQI */
  lte_rrc_llc_test_rrc_connection_setup_with_r10_antenna_selection();

  /* Send Reconfig messages that doesn't include CQIConfig */
  lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_dl();

  /* Send a Reconfig message triggering intra-freq HO */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg2);
  encoded_pdu.value = llc_reconfig_ho_msg2;

  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(lte_rrc_llc_test_common_data.dl_dcch_msg_ptr != NULL);

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);

  TF_MSG("\nLLC AntennaInfo config test ends \n");
  lte_rrc_utf_reset(); 

}

void LteRrcLlcAntennaCfgR10toR8duringHOinvalidTest::Teardown()
{
  lte_rrc_utf_teardown();
}

//AntennaInfo Rel10 to Rel8 during CRE and rel8 applied
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcAntennaCfgR10toR8duringCREvalidTest);

void LteRrcLlcAntennaCfgR10toR8duringCREvalidTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_config_is_first_reconfig_after_cre) = lte_rrc_llc_test_reconfig_after_cre1;
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel11;
}

void LteRrcLlcAntennaCfgR10toR8duringCREvalidTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_common_cfg_mask_t common_cfg_mask;
  int decode_status=0;

  TF_MSG("\nLLC AntennaInfo config test begins \n");
  lte_rrc_llc_test_basic_sib2_update();

  /* Sending a Conn setup that configures periodic CQI */
  lte_rrc_llc_test_rrc_connection_setup_with_r10_antenna_selection();

  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->antenna_cfg_rel_version == LTE_3GPP_REL10);

  /* Test the processing of RLF for a non-HO scenario */
  lte_rrc_llc_test_rlf();

  common_cfg_mask = 
  (
    LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL |
    LTE_CPHY_COMMON_CFG_MASK_BIT_DRX_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH |
    LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM |
    LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG
  );
  lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reestablishment Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_cre_msg_without_srs_dedicated);
  encoded_pdu.value = llc_cre_msg_without_srs_dedicated;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CRE, FALSE);

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CRE, FALSE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CRE);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CRE);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CRE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  /* Set the reconfig after cre flag in test DB to TRUE */
  lte_rrc_llc_test_common_data.reconfig_after_cre = TRUE;

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reestablishment Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();

  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->antenna_cfg_rel_version == LTE_3GPP_REL8);

  reconfig_after_cre1 = TRUE;
  lte_rrc_llc_test_first_reconfig_after_cre();

  TF_MSG("\nLLC AntennaInfo config test ends \n");
  lte_rrc_utf_reset(); 

}

void LteRrcLlcAntennaCfgR10toR8duringCREvalidTest::Teardown()
{
  lte_rrc_utf_teardown();
}

//CBR reverted back to 0 if not present in case of TM8 or TM9
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcAntennaCfgCRBrevertinTM8Test);

void LteRrcLlcAntennaCfgCRBrevertinTM8Test::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
}

void LteRrcLlcAntennaCfgCRBrevertinTM8Test::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  int decode_status=0;

  TF_MSG("\nLLC AntennaInfo config test begins \n");
  lte_rrc_llc_test_basic_sib2_update();

  /* Sending a Conn setup that configures periodic CQI */
  encoded_pdu.length = sizeof(llc_conn_setup_with_tm9_ies);
  encoded_pdu.value = llc_conn_setup_with_tm9_ies;

  lte_rrc_llc_test_rrc_connection_setup_for_tm9(encoded_pdu);

  encoded_pdu.length = sizeof(llc_reconfig_tm9_aperiodic_rm20);
  encoded_pdu.value = llc_reconfig_tm9_aperiodic_rm20;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->antenna_cfg_dedicated.codebook_subset_restriction == 0);

  TF_MSG("\nLLC AntennaInfo config test ends \n");
  lte_rrc_utf_reset(); 

}

void LteRrcLlcAntennaCfgCRBrevertinTM8Test::Teardown()
{
  lte_rrc_utf_teardown();
}

