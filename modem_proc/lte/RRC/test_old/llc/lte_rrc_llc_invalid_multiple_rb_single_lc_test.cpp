/*!
  @file lte_rrc_llc_invalid_multiple_rb_single_lc_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a Connection Setup message.

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
02/03/09   da     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidMultipleRbSingleLcTest);

void LteRrcLlcInvalidMultipleRbSingleLcTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL, 0);
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcInvalidMultipleRbSingleLcTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Reconfig for Multiple DRBs on a single LC Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_invalid_multiple_rb_single_lc);
  encoded_pdu.value = llc_invalid_multiple_rb_single_lc;
  
  /* On receiving a Config Req from Config, LLC should be in the INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  TF_MSG("\nInvalid Reconfig for Multiple DRBs on a single LC Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInvalidMultipleRbSingleLcTest::Teardown()
{
  lte_rrc_utf_teardown();
}

