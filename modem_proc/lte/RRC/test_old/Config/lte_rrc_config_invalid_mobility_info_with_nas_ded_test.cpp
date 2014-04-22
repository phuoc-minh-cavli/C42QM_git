/*!
  @file lte_rrc_config_invalid_mobility_info_with_nas_ded_test.cpp

  @brief
  Unit test code for the Config procedure that tests the processing of 
  an invalid Reconfig message containing mobility info and NAS dedicated info.

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

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/10   aramani Ported code to Objective Systems 
05/18/09   da     Initial Revision
           
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
#include "lte_rrc_configi.h"
#include "lte_rrc_config.h"
#include "lte_static_rrc_mem_util.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_config_test_common.h"
#include <tf_stub.h> 

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigInvalidMobilityInfoWithNasDedTest);

void LteRrcConfigInvalidMobilityInfoWithNasDedTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, NULL, 0);
  lte_rrc_config_test_stub_init();
}

void LteRrcConfigInvalidMobilityInfoWithNasDedTest::Test()
{
  lte_rrc_mh_dlm_s dl_msg;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_conn_mode_failure_indi_s *config_failure_ind_msg_ptr = NULL;
  lte_rrc_dlm_processed_indi_s *dlm_processed_ind_ptr = NULL;
  uint32 buf_len = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&dl_msg.msg_hdr,LTE_RRC_RRC_CONNECTION_RECONFIGURATION_DLM);
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_config_ota_msg.h for 
     all Config OTA message definitions */ 
  encoded_pdu.length = sizeof(invalid_mobility_info_with_nas_ded_msg);
  encoded_pdu.value = invalid_mobility_info_with_nas_ded_msg;

  /* Initialise the currently used encoded message*/
  lte_rrc_config_test_encoded_pdu = encoded_pdu;

  TF_MSG("\nReconfig: Invalid mobility Test Begins \n");
  /* On receiving a DLM from message handler, we should stay in the same state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);
  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  dl_msg.dl_msg_ptr = lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                                                   DL_DCCH_Message, &decode_status);

  dl_msg.pdu_num = lte_rrc_osys_DL_DCCH_Message_PDU;
  lte_rrc_utf_send_msg((byte*)&dl_msg, sizeof(dl_msg));

  /* Catch the conn mode failure indication send by Config */
  lte_rrc_utf_get_next_int_msg((byte **)&config_failure_ind_msg_ptr,&buf_len);
  /* Make sure the HO Started indication is valid */
  TF_ASSERT(sizeof(lte_rrc_conn_mode_failure_indi_s) == buf_len);
  TF_ASSERT(config_failure_ind_msg_ptr != NULL);
  TF_ASSERT(config_failure_ind_msg_ptr->conn_mode_failure_cause 
            == LTE_RRC_CONN_MODE_FAILURE_CAUSE_INVALID_CFG);

  /* Catch the dlm processed indication message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&dlm_processed_ind_ptr,&buf_len);
  /* Make sure the processed indication message is valid */
  TF_ASSERT(dlm_processed_ind_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);
  
  /* Make sure Config is back in INITIAL state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);

  TF_MSG("\nReconfig: Invalid Mobility Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcConfigInvalidMobilityInfoWithNasDedTest::Teardown()
{
  lte_rrc_utf_teardown();
}








