/*!
  @file lte_rrc_config_channel_cfg_failure_test.cpp

  @brief
  Unit test code for the Config procedure that tests channel config failure 
  handling while processing a Reconfig message containing HO related information.

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
12/23/08   mm     Initial Revision
           
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
#include "lte_mac_msg.h"
#include "lte_mac_rrc.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_config_test_common.h"
#include <tf_stub.h> 

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigChannelCfgFailureTest);

void LteRrcConfigChannelCfgFailureTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, NULL, 0);
  lte_rrc_config_test_stub_init();
}

void LteRrcConfigChannelCfgFailureTest::Test()
{
  lte_rrc_mh_dlm_s dl_msg;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_cfg_reqi_s* cfg_req_msg_ptr = NULL;
  lte_rrc_handover_started_indi_s *ho_started_ind_msg_ptr = NULL;
  lte_rrc_cfg_cnfi_s cfg_cnf_msg; 
  lte_rrc_conn_mode_failure_indi_s *config_failure_ind_msg_ptr = NULL;
  lte_rrc_dlm_processed_indi_s *dlm_processed_ind_ptr = NULL;
  uint32 buf_len = 0;
  int decode_status = 0;
  lte_rrc_trm_priority_change_indi_s *priority_change_indi_ptr = NULL;
  byte *buf_ptr;
  /* ---------------------------------------------------------------------- */

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&dl_msg.msg_hdr,LTE_RRC_RRC_CONNECTION_RECONFIGURATION_DLM);
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_config_ota_msg.h for 
     all Config OTA message definitions */ 
  encoded_pdu.length = sizeof(reconfig_with_ho_only_msg);
  encoded_pdu.value = reconfig_with_ho_only_msg;
  lte_rrc_config_test_encoded_pdu = encoded_pdu;

  TF_MSG("\nConfig: Channel Configuration Failure Begins \n");
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

  /* Catch the HO Started indication sent by Config */
  lte_rrc_utf_get_next_int_msg((byte **)&ho_started_ind_msg_ptr,&buf_len);
  /* Make sure the HO Started indication is valid */
  TF_ASSERT(sizeof(lte_rrc_handover_started_indi_s) == buf_len);

  /* Send the CFG_CNFI with FAILURE to Config module there by mimicing the action of LLC */
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_cnf_msg.msg_hdr,LTE_RRC_CFG_CNFI);
  /* Setting the fields in the CFG_CNFI sent by LLC */
  cfg_cnf_msg.proc = LTE_RRC_PROC_CONFIG;
  cfg_cnf_msg.status = E_FAILURE;
  cfg_cnf_msg.failure_type = LTE_RRC_CFG_FAILURE_NORMAL;
  cfg_cnf_msg.trans_id = 0;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf_msg, sizeof(cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();

  /*Expect a TrmPriorityChangeInd*/
  TF_MSG("Checking for LTE_RRC_TRM_PRIORITY_CHANGE_INDI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_trm_priority_change_indi_s) == buf_len);
  priority_change_indi_ptr = (lte_rrc_trm_priority_change_indi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_TRM_PRIORITY_CHANGE_INDI == priority_change_indi_ptr->msg_hdr.id);

  /*Expect a TrmPriorityChangeInd*/
  TF_MSG("Checking for LTE_RRC_TRM_PRIORITY_CHANGE_INDI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_trm_priority_change_indi_s) == buf_len);
  priority_change_indi_ptr = (lte_rrc_trm_priority_change_indi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_TRM_PRIORITY_CHANGE_INDI == priority_change_indi_ptr->msg_hdr.id);

  /* Catch the conn mode failure indication send by Config */
  lte_rrc_utf_get_next_int_msg((byte **)&config_failure_ind_msg_ptr,&buf_len);
  /* Make sure the HO Started indication is valid */
  TF_ASSERT(sizeof(lte_rrc_conn_mode_failure_indi_s) == buf_len);
  TF_ASSERT(config_failure_ind_msg_ptr->conn_mode_failure_cause 
            == LTE_RRC_CONN_MODE_FAILURE_CAUSE_HO);

  /* Catch the dlm processed indication message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&dlm_processed_ind_ptr,&buf_len);
  /* Make sure the processed indication message is valid */
  TF_ASSERT(dlm_processed_ind_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  /* CFG_CNFI with FAILURE would cause Config to call the stub function for 
    lte_rrc_config_start_reestablishment */
  TF_MSG("\nConfig: Channel Configuration Failure Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcConfigChannelCfgFailureTest::Teardown()
{
  lte_rrc_utf_teardown();
}





