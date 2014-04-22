/*!
  @file lte_rrc_llc_max_drb_test.cpp
 
  @brief
  Full Call cycle prcessing - 
  Unit test code for the LLC procedure that tests the handling of maximum allowed
  DRBs (8 as of now) in LLC.

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
lte_rrc_llc_max_drb_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/09   mm     Initial Revision
           
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

#define MAX_LC_ID_VALUE 10

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcMaxDRBTest);

void LteRrcLlcMaxDRBTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcMaxDRBTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  uint8 i = 0, lc_id = MAX_LC_ID_VALUE - 1, drb_count = 1;
  int decode_status;

  TF_MSG("\nMax DRB Test Begins \n");
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_setup_srb2_drb3_am();

  for (i = 1; i <= LTE_MAX_ACTIVE_DRB; i++)
  {
    /* Since DRB3 is already setup */
    if (i == 3)
    {
      continue;
    }
    lte_rrc_llc_test_setup_drb_um_bi(i, lc_id);
    drb_count++;
    lc_id--;
  }

  /* ---------------------------------------------------------------------- */
  /* Try to add another DRB, this should fail */

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */
  encoded_pdu.length = sizeof(llc_reconfig_drb3_setup_msg);
  encoded_pdu.value = llc_reconfig_drb3_setup_msg;

  /* On receiving a Config Req from Config, LLC should be in the INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);

  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* Set the DRB id based on the passed parameter drb_id */
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem[0].drb_Identity = 20;

  /* Set the EPS bearer id based on the passed parameter drb_id */
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem[0].eps_BearerIdentity = 20;

  /* Set the LC id based on the passed parameter lc_id */
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem[0].logicalChannelIdentity = 11;

  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = dl_dcch_msg_ptr;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Release all the RBs that are setup */
  lte_rrc_llc_test_conn_release(TRUE);

  lte_rrc_llc_test_free_dl_ded_msg_ptr();

  TF_MSG("\nMax DRB Test Ends \n");
  lte_rrc_utf_reset(); 
}

void LteRrcLlcMaxDRBTest::Teardown()
{
  lte_rrc_utf_teardown();
}










