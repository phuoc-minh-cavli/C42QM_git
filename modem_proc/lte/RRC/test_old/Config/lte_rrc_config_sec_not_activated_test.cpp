/*!
  @file lte_rrc_config_sec_not_activated_test.cpp

  @brief
  Unit test code for the Config procedure that tests the processing of 
  a Reconfig message containing radio resource configuration alone, when
  security is not yet activated.

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
06/11/09   mm     Initial Revision
           
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
#include "lte_rrc_sec.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_config_test_common.h"
#include <tf_stub.h> 

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
extern "C"
{
  TF_PROTO_STUB(boolean, lte_rrc_sec_security_is_configured, void);
}

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_rrc_config_meas_test_is_sec_activated

===========================================================================*/
/*!
    @brief
    This function returns simulates the security module API that returns
    if security is activated.

    @return
    FALSE, to imply security has not been activated

 */
/*=========================================================================*/
boolean lte_rrc_config_sec_test_is_sec_activated
(
  void
)
{
  return FALSE;
}


/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigSecNotActivatedTest);

void LteRrcConfigSecNotActivatedTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, NULL, 0);
  lte_rrc_config_test_stub_init();
  TF_STUB(lte_rrc_sec_security_is_configured) = lte_rrc_config_sec_test_is_sec_activated;
}

void LteRrcConfigSecNotActivatedTest::Test()
{
  lte_rrc_mh_dlm_s dl_msg;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_conn_mode_failure_indi_s *config_failure_ind_msg_ptr = NULL;
  lte_rrc_dlm_processed_indi_s *dlm_processed_ind_ptr = NULL;
  uint32 buf_len = 0;
  int decode_status = 0;
  lte_rrc_config_s *i_ptr = NULL;
  lte_rrc_cfg_reqi_s *cfg_req_ptr;
  static lte_rrc_cfg_cnfi_s cfg_cnfi;
  byte *buf_ptr;
  lte_rrc_send_ul_msg_reqi_s *ulm_ptr;

  /* ---------------------------------------------------------------------- */

  i_ptr = (lte_rrc_config_s *)stm_get_user_data(LTE_RRC_CONFIG_SM);
  LTE_ASSERT(i_ptr != NULL);
  /* Enable security validation */
  i_ptr->sd_ptr->sec_validation_enabled = 0;

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&dl_msg.msg_hdr,LTE_RRC_RRC_CONNECTION_RECONFIGURATION_DLM);
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_config_ota_msg.h for 
     all Config OTA message definitions */ 
  encoded_pdu.length = sizeof(reconfig_rrc_msg);
  encoded_pdu.value = reconfig_rrc_msg;
  /* Initialise the currently used encoded message*/
  lte_rrc_config_test_encoded_pdu = encoded_pdu;
    
  TF_MSG("\nConfig Security not activated Test Begins \n");

  /* While receiving a DLM from message handler, Config should stay in the same state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);
  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  dl_msg.dl_msg_ptr = lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                                                   DL_DCCH_Message, &decode_status);
  /* Send the decoded MH msg containing the Reconfig message to Config module */
  dl_msg.pdu_num = lte_rrc_osys_DL_DCCH_Message_PDU;
  lte_rrc_utf_send_msg((byte*)&dl_msg, sizeof(dl_msg));

   TF_MSG("Checking for LTE_RRC_CFG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);
  cfg_req_ptr = (lte_rrc_cfg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_CFG_REQI == cfg_req_ptr->msg_hdr.id);

  TF_MSG("Sending cfg_cnfi");
  lte_rrc_init_default_hdr(&cfg_cnfi.msg_hdr,LTE_RRC_CFG_CNFI);
  cfg_cnfi.proc = LTE_RRC_PROC_CONFIG;
  cfg_cnfi.trans_id = 0;
  lte_rrc_utf_send_msg((byte*)&cfg_cnfi, sizeof(cfg_cnfi));

  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ulm_ptr = 
    (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(ulm_ptr !=NULL);
  TF_ASSERT((ulm_ptr->msg_hdr.id) ==  
            LTE_RRC_SEND_UL_MSG_REQI);

  /* Catch the dlm processed indication message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&dlm_processed_ind_ptr,&buf_len);
  /* Make sure the processed indication message is valid */
  TF_ASSERT(dlm_processed_ind_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  /* After Config completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);

  TF_MSG("\nConfig Security not activated Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcConfigSecNotActivatedTest::Teardown()
{
  lte_rrc_utf_teardown();
}



