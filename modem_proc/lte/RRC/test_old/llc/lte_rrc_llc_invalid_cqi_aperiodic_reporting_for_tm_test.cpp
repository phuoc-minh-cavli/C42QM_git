/*!
  @file lte_rrc_llc_invalid_cqi_aperiodic_reporting_for_tm_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the LLC validation for aperiodic
  reporting for TM.

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
04/28/09   mm     Initial Revision
           
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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidCQIAperiodicReportingForTMTest);

void LteRrcLlcInvalidCQIAperiodicReportingForTMTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcInvalidCQIAperiodicReportingForTMTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  int decode_status = 0;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid CQI aperiodic reporting for TM Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* This test is only for physical dedicated configuration that go to PHY alone. 
    Hence passing TRUE here */
  lte_rrc_llc_test_reconfig_phyded_cfg(TRUE);

  encoded_pdu.length = sizeof(llc_reconfig_rel9_cqi_pmi_explicit_rel_msg);
  encoded_pdu.value = llc_reconfig_rel9_cqi_pmi_explicit_rel_msg;

  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                                                                                 DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);

  /* Set Aperiodic CQI reporting to an invalid value */
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.physicalConfigDedicated.
    cqi_ReportConfig.cqi_ReportModeAperiodic = lte_rrc_osys_CQI_ReportModeAperiodic_rm12;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nInvalid CQI aperiodic reporting for TM Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInvalidCQIAperiodicReportingForTMTest::Teardown()
{
  lte_rrc_utf_teardown();
}




