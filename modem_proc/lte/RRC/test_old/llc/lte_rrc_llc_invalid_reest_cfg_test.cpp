/*!
  @file lte_rrc_llc_invalid_reest_cfg_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a Connection Reestablishment message that contains DRBToAddModify list.

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
lte_rrc_llc_invalid_reest_cfg_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/02/08   mm     Initial Revision
           
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

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidReestCfgTest);

void LteRrcLlcInvalidReestCfgTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcInvalidReestCfgTest::Test()
{

  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_CCCH_Message *dl_ccch_msg_ptr = NULL;
  int decode_status = 0;

  TF_MSG("\nInvalid Reestablishment Config Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Test RACH failure for HO mesg with DRB Add */
  lte_rrc_llc_test_ho_with_drb_add_rach_failure();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */
  encoded_pdu.length = sizeof(llc_cre_msg);
  encoded_pdu.value = llc_cre_msg;
  
  /* On receiving a Config Req from CEP, LLC should be in the INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  dl_ccch_msg_ptr = (lte_rrc_osys_DL_CCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_CCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_ccch_msg_ptr != NULL);

  /* Setting the drb_ToAddModifyList_present bitmask */
  dl_ccch_msg_ptr->message.u.c1->u.rrcConnectionReestablishment->criticalExtensions.
    u.c1->u.rrcConnectionReestablishment_r8->radioResourceConfigDedicated.m.drb_ToAddModListPresent = 1;

  lte_rrc_llc_test_common_data.dl_ccch_msg_ptr = dl_ccch_msg_ptr;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CRE, TRUE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CRE, NULL);
  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nInvalid Reestablishment Config Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

}

void LteRrcLlcInvalidReestCfgTest::Teardown()
{
  lte_rrc_utf_teardown();
}





