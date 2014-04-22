/*!
  @file lte_rrc_llc_rel9_reconfig_cqi_pmi_antenna_rlf_timers_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the setup/release of rel 9 params
  - CQI mask, PMI reporting, Antenna info dedicated with TM8 and dedicated RLF timers.

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
lte_rrc_llc_rel9_reconfig_cqi_pmi_antenna_rlf_timers_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/10   mm     Initial Revision
           
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

                         INTERNAL VARIABLES

===========================================================================*/
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcRel9ReconfigCQIPMIAntennaRLFTimersTest);

void LteRrcLlcRel9ReconfigCQIPMIAntennaRLFTimersTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcRel9ReconfigCQIPMIAntennaRLFTimersTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_rlf_tmr_s dedicated_rlf_tmr;

  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* This test is only for physical dedicated configuration that go to PHY alone. 
    Hence passing TRUE here */
  lte_rrc_llc_test_reconfig_phyded_cfg(TRUE);

  /* Verify if dedicated RLF timers and constants are enabled */
  TF_ASSERT(lte_rrc_llc_get_dedicated_rlf_timers(&dedicated_rlf_tmr) == TRUE);
  TF_ASSERT(dedicated_rlf_tmr.t301 == 200);
  TF_ASSERT(dedicated_rlf_tmr.t311 == 5000);

  /* Initiate explicit release of CQI/PMI reporting */
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_reconfig_rel9_cqi_pmi_explicit_rel_msg);
  encoded_pdu.value = llc_reconfig_rel9_cqi_pmi_explicit_rel_msg;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* Verify if dedicated RLF timers and constants are disabled */
  TF_ASSERT(lte_rrc_llc_get_dedicated_rlf_timers(&dedicated_rlf_tmr) == FALSE);

  lte_rrc_llc_test_free_dl_ded_msg_ptr();

  /* Enable CQI/PMI reporting again */
  lte_rrc_llc_test_reconfig_phyded_cfg(TRUE);

  /* Verify if dedicated RLF timers and constants are enabled */
  TF_ASSERT(lte_rrc_llc_get_dedicated_rlf_timers(&dedicated_rlf_tmr) == TRUE);
  TF_ASSERT(dedicated_rlf_tmr.t301 == 200);
  TF_ASSERT(dedicated_rlf_tmr.t311 == 5000);

  /* Initiate implicit release of CQI/PMI reporting */
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_reconfig_rel9_cqi_pmi_implicit_rel_msg);
  encoded_pdu.value = llc_reconfig_rel9_cqi_pmi_implicit_rel_msg;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* Verify if dedicated RLF timers and constants are disabled */
  TF_ASSERT(lte_rrc_llc_get_dedicated_rlf_timers(&dedicated_rlf_tmr) == FALSE);

  lte_rrc_llc_test_free_dl_ded_msg_ptr();

  lte_rrc_utf_reset(); 
}

void LteRrcLlcRel9ReconfigCQIPMIAntennaRLFTimersTest::Teardown()
{
  lte_rrc_utf_teardown();
}










