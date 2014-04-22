
/*!
  @file lte_rrc_config_test_pho_abort_during_llc_config.cpp

  @brief
  Unit test code for the Config procedure that tests the processing of 
  PSHO Abort during LLC Config.

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
02/29/12   mm     Initial Revision
           
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

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigPSHOAbortDuringLLConfigTest);

void LteRrcConfigPSHOAbortDuringLLConfigTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, NULL, 0);
  lte_rrc_config_test_stub_init();
}

void LteRrcConfigPSHOAbortDuringLLConfigTest::Test()
{
  lte_rrc_irat_to_lte_psho_started_indi_s psho_started_indi_msg;
  lte_rrc_irat_to_lte_psho_reqi_s lte_psho_reqi_msg;
  lte_rrc_irat_to_lte_psho_cnfi_s *ps_ho_cnfi_msg_ptr = NULL;
  lte_rrc_irat_to_lte_psho_abort_reqi_s psho_abort_reqi_msg;
  lte_rrc_irat_to_lte_psho_abort_cnfi_s *psho_abort_cnfi_ptr = NULL;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_cfg_reqi_s* cfg_req_msg_ptr = NULL;
  uint32 buf_len = 0;
  int decode_status = 0;

  /* ---------------------------------------------------------------------- */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&psho_started_indi_msg.msg_hdr,LTE_RRC_IRAT_TO_LTE_PSHO_STARTED_INDI);
  psho_started_indi_msg.source_rat = LTE_IRAT_RAT_TYPE_UTRAN;
  psho_started_indi_msg.t_304_value_ms = 1000;
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

  TF_MSG("\nConfig: PSHO Abort during LLC Config Test Begins \n");

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

  /* At this point send PSHO Abort REQI to Config */
  lte_rrc_init_default_hdr(&psho_abort_reqi_msg.msg_hdr,LTE_RRC_IRAT_TO_LTE_PSHO_ABORT_REQI);
  lte_rrc_utf_send_msg((byte*)&psho_abort_reqi_msg, sizeof(psho_abort_reqi_msg));
  lte_rrc_utf_wait_for_done();

  /* Catch the PSHO Abort CNFI message sent by Config to IRAT MGR */
  lte_rrc_utf_get_next_int_msg((byte **)&psho_abort_cnfi_ptr,&buf_len);
  /* Make sure the PS HO CNFI message is valid */
  TF_ASSERT(psho_abort_cnfi_ptr->msg_hdr.id == LTE_RRC_IRAT_TO_LTE_PSHO_ABORT_CNFI);

  /* After Config completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);

  TF_MSG("\nConfig: PSHO Abort during LLC Config Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcConfigPSHOAbortDuringLLConfigTest::Teardown()
{
  lte_rrc_utf_teardown();
}





