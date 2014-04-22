/*!
  @file lte_rrc_llc_invalid_conn_setup_no_srb_list_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a Connection Setup message.

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
05/28/09   da     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_static_rrc_mem_util.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidConnSetupNoSrbListTest);

void LteRrcLlcInvalidConnSetupNoSrbListTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL, 0);
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcInvalidConnSetupNoSrbListTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_CCCH_Message *ccch_msg_ptr;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Connection Setup with no SRBs Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_invalid_conn_setup_both_srbs);
  encoded_pdu.value = llc_invalid_conn_setup_both_srbs;
  
  /* On receiving a Config Req from CEP, LLC should be in the INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  ccch_msg_ptr = (lte_rrc_osys_DL_CCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_CCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);

  TF_ASSERT(ccch_msg_ptr != NULL);
  ccch_msg_ptr->message.u.c1->u.rrcConnectionSetup->criticalExtensions.u.c1->u.
    rrcConnectionSetup_r8->radioResourceConfigDedicated.m.srb_ToAddModListPresent = 0;

  lte_rrc_llc_test_common_data.dl_ccch_msg_ptr = ccch_msg_ptr;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CEP, TRUE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CEP, NULL);

  TF_MSG("\nInvalid Connection Setup with no SRBs Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInvalidConnSetupNoSrbListTest::Teardown()
{
  lte_rrc_utf_teardown();
}






