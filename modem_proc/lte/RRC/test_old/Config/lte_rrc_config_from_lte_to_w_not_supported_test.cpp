/*!
  @file lte_rrc_config_from_lte_to_w_mobility_not_supported_test.cpp

  @brief
  Unit test code for the Config procedure that tests the processing of 
  a Mobility from EUTRA message that contains Mobility to W when the feature
  is not supported.

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
10/19/10   mm     Initial Revision
           
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

extern "C"
{
  TF_PROTO_STUB(uint8, lte_rrc_cap_get_fgi_bit, uint8);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigFromFromLteToWMobilityNotSupportedTest);

void LteRrcConfigFromFromLteToWMobilityNotSupportedTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, NULL, 0);
  lte_rrc_config_test_stub_init();
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_config_test_cap_get_fgi_bit_0;
}

void LteRrcConfigFromFromLteToWMobilityNotSupportedTest::Test()
{
  lte_rrc_mh_dlm_s dl_msg;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_conn_mode_failure_indi_s *config_failure_ind_msg_ptr = NULL;
  lte_rrc_dlm_processed_indi_s *dlm_processed_ind_ptr = NULL;
  uint32 buf_len = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&dl_msg.msg_hdr, LTE_RRC_MOBILITY_FROM_EUTRA_COMMAND_DLM);

  encoded_pdu.length = sizeof(mobility_from_eutra_to_w);
  encoded_pdu.value = mobility_from_eutra_to_w;

  TF_MSG("\nMobility from EUTRA to W not supported Test Begins \n");
  /* On receiving a DLM from message handler, we should stay in the same state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);
  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  dl_msg.dl_msg_ptr = lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                                                   DL_DCCH_Message, &decode_status);
  /* Send the decoded MH msg containing the Reconfig message to Config module */
  dl_msg.pdu_num = lte_rrc_osys_DL_DCCH_Message_PDU;
  lte_rrc_utf_send_msg((byte*)&dl_msg, sizeof(dl_msg));
  lte_rrc_utf_wait_for_done();

  /* Catch the conn mode failure indication send by Config */
  lte_rrc_utf_get_next_int_msg((byte **)&config_failure_ind_msg_ptr,&buf_len);
  /* Make sure the HO Started indication is valid */
  TF_ASSERT(sizeof(lte_rrc_conn_mode_failure_indi_s) == buf_len);
  TF_ASSERT(config_failure_ind_msg_ptr != NULL);
  TF_ASSERT(config_failure_ind_msg_ptr->conn_mode_failure_cause 
            == LTE_RRC_CONN_MODE_FAILURE_CAUSE_HO);
  TF_ASSERT(config_failure_ind_msg_ptr->ho_failure_cause 
            == LTE_RRC_HO_FAILURE_CAUSE_INVALID_CFG);

  /* Catch the dlm processed indication message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&dlm_processed_ind_ptr,&buf_len);
  /* Make sure the processed indication message is valid */
  TF_ASSERT(dlm_processed_ind_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  /* Since mobility to 1x is not supported, the msg should be ignored */
  /* After Config completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);

  /* Make sure no messages are thrown by Config */
  lte_rrc_utf_check_no_more_msgs();

  TF_MSG("\nMobility from EUTRA to W not supported Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcConfigFromFromLteToWMobilityNotSupportedTest::Teardown()
{
  lte_rrc_utf_teardown();
}








