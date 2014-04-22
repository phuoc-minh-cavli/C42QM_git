/*!
  @file lte_rrc_config_meas_cfg_test.cpp

  @brief
  Unit test code for the Config procedure that tests the processing of 
  a Reconfig message containing Measurement Configuration alone when security
  has not been activated. Config should process the Measurement Configuration.

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
09/22/08   mm     Initial Revision
           
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
#include "lte_static_rrc_mem_util.h"
#include "lte_rrc_sec.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_config_test_common.h"
#include <tf_stub.h> 

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

extern "C"
{
  TF_PROTO_STUB(boolean, lte_rrc_sec_security_is_configured, void);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
#ifdef FEATURE_FEMTO_CSG
  TF_PROTO_STUB(boolean, lte_rrc_cap_eutra_proximity_ind_supported, void);
  TF_PROTO_STUB(boolean, lte_rrc_cap_utra_proximity_ind_supported, void);
  #endif
}

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigMeasCfgTest);

void LteRrcConfigMeasCfgTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, NULL, 0);
  lte_rrc_config_test_stub_init();
  TF_STUB(lte_rrc_sec_security_is_configured) = lte_rrc_config_meas_test_is_sec_activated;
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_config_test_get_3gpp_rel_version_for_rel9;
  
#ifdef FEATURE_FEMTO_CSG
  TF_STUB(lte_rrc_cap_eutra_proximity_ind_supported) = lte_rrc_config_prox_cfg_allowed;
  TF_STUB(lte_rrc_cap_utra_proximity_ind_supported) = lte_rrc_config_prox_cfg_allowed;
  #endif
}

void LteRrcConfigMeasCfgTest::Test()
{
  lte_rrc_mh_dlm_s dl_msg;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_meas_cfg_reqi_s* meas_cfg_req_msg_ptr = NULL;
  lte_rrc_meas_cfg_cnfi_s meas_cfg_cnf;
  lte_rrc_send_ul_msg_reqi_s *mh_msg_ptr = NULL;
  lte_rrc_irat_hrpd_pre_reg_info_indi_s *hrpd_prereg_info_ind_msg_ptr = NULL;
  lte_rrc_dlm_processed_indi_s *dlm_processed_ind_ptr = NULL;
  lte_rrc_prox_cfg_indi_s *prox_cfg_ind_ptr = NULL;
  uint32 buf_len = 0;
  int decode_status = 0;

  /* ---------------------------------------------------------------------- */

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&dl_msg.msg_hdr,LTE_RRC_RRC_CONNECTION_RECONFIGURATION_DLM);
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_config_ota_msg.h for 
     all Config OTA message definitions */ 
  encoded_pdu.length = sizeof(reconfig_meas_info_msg);
  encoded_pdu.value = reconfig_meas_info_msg;
  lte_rrc_config_test_encoded_pdu = encoded_pdu;

  TF_MSG("\nConfig Request Test III - Meas Config Test Begins \n");
  /* On receiving a DLM from message handler, we should stay in the same state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);
  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  dl_msg.dl_msg_ptr = lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                                                   DL_DCCH_Message, &decode_status);
  /* Send the decoded MH msg containing the Reconfig message to Config module */
  dl_msg.pdu_num = lte_rrc_osys_DL_DCCH_Message_PDU;
  lte_rrc_utf_send_msg((byte*)&dl_msg, sizeof(dl_msg));

  /* We expect meas config request since Reconfig message can come with Meas Config
    before security is activated */
  /* Catch the meas config request sent by Config module to Meas module */
  lte_rrc_utf_get_next_int_msg((byte **)&meas_cfg_req_msg_ptr,&buf_len);
  /* Make sure the Config request is valid */
  TF_ASSERT(sizeof(lte_rrc_meas_cfg_reqi_s) == buf_len);
  TF_ASSERT(meas_cfg_req_msg_ptr != NULL);
  TF_ASSERT(meas_cfg_req_msg_ptr->cell_id.freq == 10689);
  TF_ASSERT(meas_cfg_req_msg_ptr->cell_id.phy_cell_id == 511);

  /* After Config completes processing, assert its in WAIT_FOR_MEAS_CFG_CNFI state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__WAIT_FOR_MEAS_CFG_CNFI);
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&meas_cfg_cnf.msg_hdr,LTE_RRC_MEAS_CFG_CNFI);
  /* Setting the fields in the CFG_CNFI sent by Meas procedure */
  meas_cfg_cnf.status = E_SUCCESS;
  /* Send the CFG_CNFI to Config module there by mimicing the action of Meas */
  lte_rrc_utf_send_msg((byte*)&meas_cfg_cnf, sizeof(meas_cfg_cnf));

  /* Catch the PreRegInfo indi sent by Config */
  lte_rrc_utf_get_next_int_msg((byte **)&hrpd_prereg_info_ind_msg_ptr,&buf_len);
  /* Make sure the PreRegInfo indication is valid */
  TF_ASSERT(sizeof(lte_rrc_irat_hrpd_pre_reg_info_indi_s) == buf_len);
  TF_ASSERT(hrpd_prereg_info_ind_msg_ptr->msg_hdr.id == LTE_RRC_IRAT_HRPD_PRE_REG_INFO_INDI);
  TF_ASSERT(hrpd_prereg_info_ind_msg_ptr->prereg_with_handover == FALSE);

  /* Catch the Reconfig complete message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&mh_msg_ptr,&buf_len);
  /* Make sure the Reconfig complete message is valid */
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);

  /* Catch the Prox config indication message*/
  lte_rrc_utf_get_next_int_msg((byte **)&prox_cfg_ind_ptr,&buf_len);
  /* Make sure the processed indication message is valid */
  TF_ASSERT(prox_cfg_ind_ptr->msg_hdr.id == LTE_RRC_PROX_CFG_INDI);
  TF_ASSERT(prox_cfg_ind_ptr->eutra_configured == 1);
  TF_ASSERT(prox_cfg_ind_ptr->utra_configured == 1);

  /* Catch the dlm processed indication message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&dlm_processed_ind_ptr,&buf_len);
  /* Make sure the processed indication message is valid */
  TF_ASSERT(dlm_processed_ind_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);
  TF_MSG("\nConfig Request Test III - Meas Config Test Ends \n");
  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(mh_msg_ptr->ul_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcConfigMeasCfgTest::Teardown()
{
  lte_rrc_utf_teardown();
}



