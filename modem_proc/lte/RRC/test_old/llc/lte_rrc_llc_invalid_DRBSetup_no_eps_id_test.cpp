/*!
  @file lte_rrc_llc_invalid_drbsetup_no_eps_id_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a Connection Reconfiguration that tries to add a DRB without including an
  EPS bearer ID.

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
06/30/09   mm     Initial Revision
           
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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidDRBSetupNoEPSIDTest);

void LteRrcLlcInvalidDRBSetupNoEPSIDTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL, 0);
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcInvalidDRBSetupNoEPSIDTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Reconfig for DRB Setup without EPS bearer ID Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_drb3_setup_msg);
  encoded_pdu.value = llc_reconfig_drb3_setup_msg;
  
  /* On receiving a Config Req from Config, LLC should be in the INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Construct asn1 pdu to be sent to LLC module after decoding the encoded pdu*/
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);

  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* Reset the bitmask for EPS bearer ID */
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->m.eps_BearerIdentityPresent = 0;

  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = dl_dcch_msg_ptr;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nInvalid Reconfig for DRB Setup without EPS bearer ID Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInvalidDRBSetupNoEPSIDTest::Teardown()
{
  lte_rrc_utf_teardown();
}


