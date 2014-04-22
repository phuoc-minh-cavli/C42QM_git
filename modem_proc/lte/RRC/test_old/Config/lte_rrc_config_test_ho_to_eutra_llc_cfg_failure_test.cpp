/*!
  @file lte_rrc_config_ho_to_eutra_llc_cfg_failure_test.cpp

  @brief
  Unit test code for the Config procedure that tests the processing of LLC Config failure
  during the processing of HO to EUTRA message from IRAT MGR.

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
01/05/12   mm     Initial Revision
           
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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigHOToEUTRALLCCfgFailureTest);

void LteRrcConfigHOToEUTRALLCCfgFailureTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, NULL, 0);
  lte_rrc_config_test_stub_init();
}

void LteRrcConfigHOToEUTRALLCCfgFailureTest::Test()
{
  lte_rrc_irat_to_lte_psho_started_indi_s psho_started_indi_msg;
  lte_rrc_irat_to_lte_psho_reqi_s lte_psho_reqi_msg;
  lte_rrc_irat_to_lte_psho_cnfi_s *ps_ho_cnfi_msg_ptr = NULL;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_cfg_reqi_s* cfg_req_msg_ptr = NULL;
  lte_rrc_cfg_cnfi_s cfg_cnf_msg;
  lte_rrc_dlm_processed_indi_s *dlm_processed_ind_ptr = NULL;
  uint32 buf_len = 0;
  int decode_status = 0;

  /* ---------------------------------------------------------------------- */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&psho_started_indi_msg.msg_hdr,LTE_RRC_IRAT_TO_LTE_PSHO_STARTED_INDI);
  psho_started_indi_msg.source_rat = LTE_IRAT_RAT_TYPE_UTRAN;
  psho_started_indi_msg.t_304_value_ms = 150;
  lte_rrc_utf_send_msg((byte*)&psho_started_indi_msg, sizeof(psho_started_indi_msg));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&lte_psho_reqi_msg.msg_hdr,LTE_RRC_IRAT_TO_LTE_PSHO_REQI);
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_config_ota_msg.h for 
     all Config OTA message definitions */ 
  encoded_pdu.length = sizeof(ho_to_eutra_msg);
  encoded_pdu.value = ho_to_eutra_msg;
  lte_rrc_config_test_encoded_pdu = encoded_pdu;

  TF_MSG("\nConfig: HO to EUTRA LLC Config failure Test Begins \n");

  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  lte_psho_reqi_msg.dl_msg_ptr = lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                                                   DL_DCCH_Message, &decode_status);
  lte_rrc_utf_send_msg((byte*)&lte_psho_reqi_msg, sizeof(lte_psho_reqi_msg));

  /* Catch the Config request sent by Config to LLC */
  lte_rrc_utf_get_next_int_msg((byte **)&cfg_req_msg_ptr,&buf_len);
  /* Make sure the Config request is valid */
  TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);

  /* After Config completes processing, assert its in WAIT_FOR_CFG_CNFI state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__WAIT_FOR_CFG_CNFI);
  TF_MSG("\nConfig Request sent to LLC and Config Module in WAIT_FOR_CFG_CNFI state\n");

  /* Send the CFG_CNFI to Config module there by mimicing the action of LLC */
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_cnf_msg.msg_hdr,LTE_RRC_CFG_CNFI);

  /* Setting the fields in the CFG_CNFI sent by LLC */
  cfg_cnf_msg.proc = LTE_RRC_PROC_CONFIG;
  cfg_cnf_msg.status = E_DATA_INVALID;
  cfg_cnf_msg.failure_type = LTE_RRC_CFG_FAILURE_NORMAL;
  cfg_cnf_msg.trans_id = 0;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf_msg, sizeof(cfg_cnf_msg));

  /* Catch the PS HO CNFI message sent by Config to IRAT MGR */
  lte_rrc_utf_get_next_int_msg((byte **)&ps_ho_cnfi_msg_ptr,&buf_len);
  /* Make sure the PS HO CNFI message is valid */
  TF_ASSERT(ps_ho_cnfi_msg_ptr->msg_hdr.id == LTE_RRC_IRAT_TO_LTE_PSHO_CNFI);
  TF_ASSERT(ps_ho_cnfi_msg_ptr->ho_status == LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE);
  TF_ASSERT(ps_ho_cnfi_msg_ptr->failure_cause == LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_INVALID_CONFIG);

  /* Catch the dlm processed indication message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&dlm_processed_ind_ptr,&buf_len);
  /* Make sure the processed indication message is valid */
  TF_ASSERT(dlm_processed_ind_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  /* After Config completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);

  TF_MSG("\nConfig: HO to EUTRA LLC Config failure Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcConfigHOToEUTRALLCCfgFailureTest::Teardown()
{
  lte_rrc_utf_teardown();
}







