/*!
  @file lte_rrc_llc_reest_no_phy_params_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a Connection Reestablishment message that doesn't include phyConfigDedicated IE.
  LLC should still send CPHY_DEDICATEd_CONFIG_REQ with default values but with
  default_config_flag set to FALSE to enable ML1 to go to CONNECTED state.
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/llc/lte_rrc_llc_reest_no_phy_params_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/10   mm     Initial Revision
           
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

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcReestNoPhyParamsTest);

void LteRrcLlcReestNoPhyParamsTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcReestNoPhyParamsTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_ded_ptr = NULL;
  int decode_status;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reestablishment with no phy params Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Test RACH failure for HO mesg with DRB Add */
  lte_rrc_llc_test_ho_with_drb_add_rach_failure();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_cre_msg);
  encoded_pdu.value = llc_cre_msg;

  lte_rrc_llc_test_common_data.dl_ccch_msg_ptr = (lte_rrc_osys_DL_CCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_CCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);

  /* Reset the bitmask for physicalConfigDedicated IE in Connection Reestablishment message */
  radio_resrc_ded_ptr = &lte_rrc_llc_test_common_data.dl_ccch_msg_ptr->message.u.c1->u.
    rrcConnectionReestablishment->criticalExtensions.u.c1->u.rrcConnectionReestablishment_r8->
    radioResourceConfigDedicated;
  radio_resrc_ded_ptr->m.physicalConfigDedicatedPresent = 0;
  radio_resrc_ded_ptr->m.sps_ConfigPresent = 0;

  radio_resrc_ded_ptr->mac_MainConfig.u.explicitValue->m.ul_SCH_ConfigPresent = 1;
  radio_resrc_ded_ptr->mac_MainConfig.u.explicitValue->ul_SCH_Config.m.maxHARQ_TxPresent = 1;
  radio_resrc_ded_ptr->mac_MainConfig.u.explicitValue->ul_SCH_Config.m.periodicBSR_TimerPresent = 1;
  radio_resrc_ded_ptr->mac_MainConfig.u.explicitValue->ul_SCH_Config.maxHARQ_Tx = lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n20;
  radio_resrc_ded_ptr->mac_MainConfig.u.explicitValue->ul_SCH_Config.
    periodicBSR_Timer = lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf128;
  radio_resrc_ded_ptr->mac_MainConfig.u.explicitValue->ul_SCH_Config.retxBSR_Timer = lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_retxBSR_Timer_sf2560;
  radio_resrc_ded_ptr->mac_MainConfig.u.explicitValue->ul_SCH_Config.ttiBundling = TRUE;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CRE, TRUE);

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CRE, TRUE);

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

  TF_MSG("\nConnection Reestablishment with no phy params Test Ends \n");

  /* Setting the bitmask in the original OTA so that the allocated memory is
    freed correctly */
  radio_resrc_ded_ptr->m.physicalConfigDedicatedPresent = 1;
  radio_resrc_ded_ptr->m.sps_ConfigPresent = 1;

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_utf_reset(); 

}

void LteRrcLlcReestNoPhyParamsTest::Teardown()
{
  lte_rrc_utf_teardown();
}





