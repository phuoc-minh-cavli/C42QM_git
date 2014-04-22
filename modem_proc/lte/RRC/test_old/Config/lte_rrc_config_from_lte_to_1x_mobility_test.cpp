/*!
  @file lte_rrc_config_from_lte_to_1x_mobility_test.cpp

  @brief
  Unit test code for the Config procedure that tests the processing of 
  a Mobility from EUTRA message that contains HO to 1x for CSFB.

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
10/18/10   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "__lte_rrc_config.h"
#include "lte_rrc_config.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_static_rrc_mem_util.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_config_test_common.h"
#include <tf_stub.h> 

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigFromLteTo1xMobilityTest);

void LteRrcConfigFromLteTo1xMobilityTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, NULL, 0);
  lte_rrc_config_test_stub_init();
}

void LteRrcConfigFromLteTo1xMobilityTest::Test()
{
  lte_rrc_mh_dlm_s dl_msg;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_irat_from_lte_to_1x_mobility_indi_s *lte_to_1x_mobility_indi_ptr;
  lte_rrc_cfg_reqi_s* cfg_req_msg_ptr = NULL;
  lte_rrc_cfg_cnfi_s cfg_cnf_msg;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  uint32 buf_len = 0;
  int decode_status = 0;
  lte_rrc_irat_from_lte_mobility_started_indi_s *mobility_started_indi;
  /* ---------------------------------------------------------------------- */

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&dl_msg.msg_hdr, LTE_RRC_MOBILITY_FROM_EUTRA_COMMAND_DLM);

  encoded_pdu.length = sizeof(mobility_from_eutra_to_1x);
  encoded_pdu.value = mobility_from_eutra_to_1x;
  lte_rrc_config_test_encoded_pdu = encoded_pdu;

  TF_MSG("\nMobility from EUTRA to 1x msg dispatch Test Begins \n");
  /* On receiving a DLM from message handler, we should stay in the same state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);
  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  dl_msg.dl_msg_ptr = lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                                                   DL_DCCH_Message, &decode_status);
  /* Send the decoded MH msg containing the Reconfig message to Config module */
  dl_msg.pdu_num = lte_rrc_osys_DL_DCCH_Message_PDU;
  lte_rrc_utf_send_msg((byte*)&dl_msg, sizeof(dl_msg));

  /* Catch the Config request sent by Config to LLC */
  lte_rrc_utf_get_next_int_msg((byte **)&cfg_req_msg_ptr,&buf_len);
  /* Make sure the Config request is valid */
  TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);
  /* After Config completes processing, assert its in WAIT_FOR_CFG_CNFI state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__WAIT_FOR_CFG_CNFI);
  TF_MSG("\nConfig Request sent to LLC and Config Module in WAIT_FOR_CFG_CNFI state\n");

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_cnf_msg.msg_hdr,LTE_RRC_CFG_CNFI);
  /* Setting the fields in the CFG_CNFI sent by LLC */
  cfg_cnf_msg.proc = LTE_RRC_PROC_CONFIG;
  cfg_cnf_msg.status = E_SUCCESS;
  cfg_cnf_msg.trans_id = 0;
  /* Send the CFG_CNFI to Config module there by mimicing the action of LLC */
  lte_rrc_utf_send_msg((byte*)&cfg_cnf_msg, sizeof(cfg_cnf_msg));

  /* Catch LTE_RRC_IRAT_FROM_LTE_MOBILITY_STARTED_INDI sent by Config to 1x IRAT MGR */
  lte_rrc_utf_get_next_int_msg((byte **)&mobility_started_indi,&buf_len);
  TF_ASSERT(mobility_started_indi->msg_hdr.id == LTE_RRC_IRAT_FROM_LTE_MOBILITY_STARTED_INDI);

  /* Catch LTE_RRC_IRAT_FROM_LTE_TO_1X_MOBILITY_INDI sent by Config to 1x IRAT MGR */
  lte_rrc_utf_get_next_int_msg((byte **)&lte_to_1x_mobility_indi_ptr,&buf_len);
  /* Make sure the Config request is valid */
  TF_ASSERT(lte_to_1x_mobility_indi_ptr->msg_hdr.id == LTE_RRC_IRAT_FROM_LTE_TO_1X_MOBILITY_INDI);
  TF_ASSERT(lte_to_1x_mobility_indi_ptr->dl_msg_ptr != NULL);
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *) lte_to_1x_mobility_indi_ptr->dl_msg_ptr;
  TF_ASSERT(dl_dcch_msg_ptr->message.u.c1->t == 
			T_lte_rrc_osys_DL_DCCH_MessageType_c1_mobilityFromEUTRACommand);

  /* After Config completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);

  TF_MSG("\nMobility from EUTRA to 1x msg dispatch Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcConfigFromLteTo1xMobilityTest::Teardown()
{
  lte_rrc_utf_teardown();
}





