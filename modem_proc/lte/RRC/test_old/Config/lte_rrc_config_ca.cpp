/*!
  @file lte_rrc_config_rrc_test.cpp

  @brief
  Unit test code for the Config procedure that tests the processing of 
  a Reconfig message containing radio resource configuration alone.

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

when       who      what, where, why
--------   ---      ---------------------------------------------------------- 
06/09/10   aramani  Ported code to Objective Systems 
09/11/08   mm       Initial Revision
           
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
#include "lte.h"
#include "lte_rrc_configi.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_mac_msg.h"
#include "lte_mac_rrc.h"
#include "lte_rrc_meas.h"
#include "__lte_rrc_meas.h"
#include "lte_rrc_meas.h"
#include "lte_rrc_measi.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_config_test_common.h"
#include <tf_stub.h> 

/*===========================================================================

                                UNIT TEST

===========================================================================*/

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

extern "C"
{
  TF_PROTO_STUB(void, lte_rrc_llc_get_epsbearer_info, lte_rrc_active_eps_bearer_update_ind_s *);
}

uint32 external_umid_list[] = 
{
  LTE_RRC_DL_DATA_IND,
  LTE_RRC_ACTIVE_EPS_BEARER_UPDATE_IND,
  LTE_RRC_CONFIG_COMPLETED_IND
};

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_rrc_config_ca__nas_info_test_get_eps_info

===========================================================================*/
/*!
    @brief
    This function simulates the LLC API that gets the list of active EPS Bearers 
    list from PDCP configuration stored in the Config DB. The function 
    returns the number of active EPS bearers.

    @return
    uint8 - Number of Active EPS bearers
*/
/*=========================================================================*/
void lte_rrc_config_ca__nas_info_test_get_eps_info
(
  lte_rrc_active_eps_bearer_update_ind_s *eps_list_msg_ptr  /*!< EPS bearer Info */
)
{
  eps_list_msg_ptr->active_eps_bearers[0].eps_id = 3;
  eps_list_msg_ptr->num_active_bearers = 1;
} /* lte_rrc_config_ca__nas_info_test_get_eps_info() */

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigCaScellAddTest);

void LteRrcConfigCaScellAddTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, external_umid_list, 3);
  lte_rrc_config_test_stub_init();
  TF_STUB(lte_rrc_llc_get_epsbearer_info) = lte_rrc_config_ca__nas_info_test_get_eps_info;
}

void LteRrcConfigCaScellAddTest::Test()
{
  lte_rrc_mh_dlm_s dl_msg;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_cfg_reqi_s* cfg_req_msg_ptr = NULL;
  lte_rrc_cfg_cnfi_s cfg_cnf_msg;
  uint32 buf_len = 0;
  lte_rrc_send_ul_msg_reqi_s *mh_msg_ptr = NULL;
  lte_rrc_meas_cfg_reqi_s* meas_cfg_req_msg_ptr = NULL;
  lte_rrc_meas_cfg_cnfi_s meas_cfg_cnf;
  lte_rrc_dl_data_ind_s *nas_msg_ptr = NULL;
  lte_rrc_active_eps_bearer_update_ind_s *eps_list_msg_ptr = NULL;
  lte_rrc_dlm_processed_indi_s *dlm_processed_ind_ptr = NULL;
  lte_rrc_config_completed_ind_s *config_completed_ind_ptr = NULL;
  int decode_status = 0;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nRe-Config Request with Scell add Test Begins \n");
  /* The first iteration through the for loop would setup DRB3 and do checks 
    related to the processing DRB setup. The second iteration would release
    the DRB3 that was setup in the first iteration. In processing both the 
    reconfig messages, the stub function for returning the active EPS bearer
    list should be called. */

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&dl_msg.msg_hdr,LTE_RRC_RRC_CONNECTION_RECONFIGURATION_DLM);
  /* Setting the encoded message to the byte string corresponding to radio 
   resource configuration. Please refer to lte_rrc_config_ota_msg.h for 
   all Config OTA message definitions */ 
 
  encoded_pdu.length = sizeof(reconfig_ca_scell_add);
  encoded_pdu.value = reconfig_ca_scell_add;
  lte_rrc_config_test_encoded_pdu = encoded_pdu;
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

  /* Catch the NAS message sent by Config to NAS */
  lte_rrc_utf_get_next_ext_msg((byte **)&nas_msg_ptr,&buf_len);
  /* Make sure the NAS message sent by Config is valid */
  TF_ASSERT(sizeof(lte_rrc_dl_data_ind_s) == buf_len);
  //TF_ASSERT(nas_msg_ptr->nas_msg_container.msg_size == 3);

  /* Catch the active EPS bearer list sent by Config to NAS */
  lte_rrc_utf_get_next_ext_msg((byte **)&eps_list_msg_ptr,&buf_len);
  /* Make sure the NAS message sent by Config is valid */
  TF_ASSERT(sizeof(lte_rrc_active_eps_bearer_update_ind_s) == buf_len);
  /* Make sure passed EPS bearer list is valid */
  TF_ASSERT(eps_list_msg_ptr->num_active_bearers == 1);
  TF_ASSERT(eps_list_msg_ptr->active_eps_bearers[0].eps_id == 3);


   TF_MSG("\Check\n");

  /* Catch the Reconfig complete message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&mh_msg_ptr,&buf_len);
  /* Make sure the Reconfig complete message is valid */
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);

  /* Catch the dlm processed indication message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&dlm_processed_ind_ptr,&buf_len);
  /* Make sure the processed indication message is valid */
  TF_ASSERT(dlm_processed_ind_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  /* Catch the config completed indication message sent by Config to L2 */
  lte_rrc_utf_get_next_ext_msg((byte **)&config_completed_ind_ptr,&buf_len);
  /* Make sure the config completed indication message is valid */
  TF_ASSERT(config_completed_ind_ptr->msg_hdr.id == LTE_RRC_CONFIG_COMPLETED_IND);

  /* After Config completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);
  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(mh_msg_ptr->ul_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("\n Re-Config Request with Scell add Test Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcConfigCaScellAddTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigCaScellModifyTest);

void LteRrcConfigCaScellModifyTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, external_umid_list, 3);
  lte_rrc_config_test_stub_init();
  TF_STUB(lte_rrc_llc_get_epsbearer_info) = lte_rrc_config_ca__nas_info_test_get_eps_info;
}

void LteRrcConfigCaScellModifyTest::Test()
{
  lte_rrc_mh_dlm_s dl_msg;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_cfg_reqi_s* cfg_req_msg_ptr = NULL;
  lte_rrc_cfg_cnfi_s cfg_cnf_msg;
  uint32 buf_len = 0;
  lte_rrc_send_ul_msg_reqi_s *mh_msg_ptr = NULL;
  lte_rrc_meas_cfg_reqi_s* meas_cfg_req_msg_ptr = NULL;
  lte_rrc_meas_cfg_cnfi_s meas_cfg_cnf;
  lte_rrc_dl_data_ind_s *nas_msg_ptr = NULL;
  lte_rrc_active_eps_bearer_update_ind_s *eps_list_msg_ptr = NULL;
  lte_rrc_dlm_processed_indi_s *dlm_processed_ind_ptr = NULL;
  lte_rrc_config_completed_ind_s *config_completed_ind_ptr = NULL;
  int decode_status = 0;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nRe-Config Request with Scell modify Test Begins \n");
  /* The first iteration through the for loop would setup DRB3 and do checks 
    related to the processing DRB setup. The second iteration would release
    the DRB3 that was setup in the first iteration. In processing both the 
    reconfig messages, the stub function for returning the active EPS bearer
    list should be called. */

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&dl_msg.msg_hdr,LTE_RRC_RRC_CONNECTION_RECONFIGURATION_DLM);
  /* Setting the encoded message to the byte string corresponding to radio 
   resource configuration. Please refer to lte_rrc_config_ota_msg.h for 
   all Config OTA message definitions */ 
 
  encoded_pdu.length = sizeof(reconfig_ca_scell_modify);
  encoded_pdu.value = reconfig_ca_scell_modify;
  lte_rrc_config_test_encoded_pdu = encoded_pdu;
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

  /* Catch the NAS message sent by Config to NAS */
  lte_rrc_utf_get_next_ext_msg((byte **)&nas_msg_ptr,&buf_len);
  /* Make sure the NAS message sent by Config is valid */
  TF_ASSERT(sizeof(lte_rrc_dl_data_ind_s) == buf_len);
  //TF_ASSERT(nas_msg_ptr->nas_msg_container.msg_size == 3);

  /* Catch the active EPS bearer list sent by Config to NAS */
  lte_rrc_utf_get_next_ext_msg((byte **)&eps_list_msg_ptr,&buf_len);
  /* Make sure the NAS message sent by Config is valid */
  TF_ASSERT(sizeof(lte_rrc_active_eps_bearer_update_ind_s) == buf_len);
  /* Make sure passed EPS bearer list is valid */
  TF_ASSERT(eps_list_msg_ptr->num_active_bearers == 1);
  TF_ASSERT(eps_list_msg_ptr->active_eps_bearers[0].eps_id == 3);


   TF_MSG("\Check\n");

  /* Catch the Reconfig complete message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&mh_msg_ptr,&buf_len);
  /* Make sure the Reconfig complete message is valid */
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);

  /* Catch the dlm processed indication message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&dlm_processed_ind_ptr,&buf_len);
  /* Make sure the processed indication message is valid */
  TF_ASSERT(dlm_processed_ind_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  /* Catch the config completed indication message sent by Config to L2 */
  lte_rrc_utf_get_next_ext_msg((byte **)&config_completed_ind_ptr,&buf_len);
  /* Make sure the config completed indication message is valid */
  TF_ASSERT(config_completed_ind_ptr->msg_hdr.id == LTE_RRC_CONFIG_COMPLETED_IND);

  /* After Config completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);
  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(mh_msg_ptr->ul_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("\n Re-Config Request with Scell modify Test Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcConfigCaScellModifyTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigCaScellChangeTest);

void LteRrcConfigCaScellChangeTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, external_umid_list, 3);
  lte_rrc_config_test_stub_init();
  TF_STUB(lte_rrc_llc_get_epsbearer_info) = lte_rrc_config_ca__nas_info_test_get_eps_info;
}

void LteRrcConfigCaScellChangeTest::Test()
{
    lte_rrc_mh_dlm_s dl_msg;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_cfg_reqi_s* cfg_req_msg_ptr = NULL;
  lte_rrc_cfg_cnfi_s cfg_cnf_msg;
  uint32 buf_len = 0;
  lte_rrc_send_ul_msg_reqi_s *mh_msg_ptr = NULL;
  lte_rrc_meas_cfg_reqi_s* meas_cfg_req_msg_ptr = NULL;
  lte_rrc_meas_cfg_cnfi_s meas_cfg_cnf;
  lte_rrc_dl_data_ind_s *nas_msg_ptr = NULL;
  lte_rrc_active_eps_bearer_update_ind_s *eps_list_msg_ptr = NULL;
  lte_rrc_dlm_processed_indi_s *dlm_processed_ind_ptr = NULL;
  lte_rrc_config_completed_ind_s *config_completed_ind_ptr = NULL;
  int decode_status = 0;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nRe-Config Request with Scell change Test Begins \n");
  /* The first iteration through the for loop would setup DRB3 and do checks 
    related to the processing DRB setup. The second iteration would release
    the DRB3 that was setup in the first iteration. In processing both the 
    reconfig messages, the stub function for returning the active EPS bearer
    list should be called. */

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&dl_msg.msg_hdr,LTE_RRC_RRC_CONNECTION_RECONFIGURATION_DLM);
  /* Setting the encoded message to the byte string corresponding to radio 
   resource configuration. Please refer to lte_rrc_config_ota_msg.h for 
   all Config OTA message definitions */ 
 
  encoded_pdu.length = sizeof(reconfig_ca_scell_change);
  encoded_pdu.value = reconfig_ca_scell_change;
  lte_rrc_config_test_encoded_pdu = encoded_pdu;
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

  /* Catch the NAS message sent by Config to NAS */
  lte_rrc_utf_get_next_ext_msg((byte **)&nas_msg_ptr,&buf_len);
  /* Make sure the NAS message sent by Config is valid */
  TF_ASSERT(sizeof(lte_rrc_dl_data_ind_s) == buf_len);
  //TF_ASSERT(nas_msg_ptr->nas_msg_container.msg_size == 3);

  /* Catch the active EPS bearer list sent by Config to NAS */
  lte_rrc_utf_get_next_ext_msg((byte **)&eps_list_msg_ptr,&buf_len);
  /* Make sure the NAS message sent by Config is valid */
  TF_ASSERT(sizeof(lte_rrc_active_eps_bearer_update_ind_s) == buf_len);
  /* Make sure passed EPS bearer list is valid */
  TF_ASSERT(eps_list_msg_ptr->num_active_bearers == 1);
  TF_ASSERT(eps_list_msg_ptr->active_eps_bearers[0].eps_id == 3);


   TF_MSG("\Check\n");

  /* Catch the Reconfig complete message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&mh_msg_ptr,&buf_len);
  /* Make sure the Reconfig complete message is valid */
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);

  /* Catch the dlm processed indication message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&dlm_processed_ind_ptr,&buf_len);
  /* Make sure the processed indication message is valid */
  TF_ASSERT(dlm_processed_ind_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  /* Catch the config completed indication message sent by Config to L2 */
  lte_rrc_utf_get_next_ext_msg((byte **)&config_completed_ind_ptr,&buf_len);
  /* Make sure the config completed indication message is valid */
  TF_ASSERT(config_completed_ind_ptr->msg_hdr.id == LTE_RRC_CONFIG_COMPLETED_IND);

  /* After Config completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);
  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(mh_msg_ptr->ul_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("\n Re-Config Request with Scell change Test Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcConfigCaScellChangeTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

//external messages generated by code under test
static uint32 ext_ca_umid_list[] = 
{ 
  LTE_MAC_ACCESS_REQ,
  LTE_RRC_ACTIVE_EPS_BEARER_UPDATE_IND,
  LTE_RRC_CONFIG_COMPLETED_IND
};

extern "C"
{
  TF_PROTO_STUB(void, lte_rrc_llc_get_epsbearer_info, lte_rrc_active_eps_bearer_update_ind_s *);
}

/*===========================================================================

                                FUNCTIONS

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_rrc_config_ca_ho_full_config_test_get_eps_info

===========================================================================*/
/*!
    @brief
    This function simulates the LLC API that gets the list of active EPS Bearers 
    list from PDCP configuration stored in the Config DB. The function 
    returns the number of active EPS bearers.

    @return
    uint8 - Number of Active EPS bearers
*/
/*=========================================================================*/
void lte_rrc_config_ca_ho_full_config_test_get_eps_info
(
  lte_rrc_active_eps_bearer_update_ind_s *eps_list_msg_ptr  /*!< list of EPS bearer IDs */
)
{
  eps_list_msg_ptr->active_eps_bearers[0].eps_id = 5;
  eps_list_msg_ptr->active_eps_bearers[0].rb_id = 5;
  eps_list_msg_ptr->num_active_bearers = 1;
} /* lte_rrc_config_ca_ho_full_config_test_get_eps_info() */

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigHOWithPcellToPcellScellTest);

void LteRrcConfigHOWithPcellToPcellScellTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, ext_ca_umid_list, 3);
  lte_rrc_config_test_stub_init();
  TF_STUB(lte_rrc_llc_get_epsbearer_info) = lte_rrc_config_ca_ho_full_config_test_get_eps_info;
}

void LteRrcConfigHOWithPcellToPcellScellTest::Test()
{
  lte_rrc_mh_dlm_s dl_msg;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_cfg_reqi_s* cfg_req_msg_ptr = NULL;
  lte_rrc_cfg_cnfi_s cfg_cnf_msg;
  lte_rrc_handover_started_indi_s *ho_started_ind_msg_ptr = NULL;
  lte_rrc_send_ul_msg_reqi_s *mh_msg_ptr = NULL;
  lte_rrc_meas_cfg_reqi_s *meas_cfg_req_ptr = NULL;
  lte_rrc_meas_cfg_cnfi_s meas_cfg_cnf;
  lte_mac_access_req_msg_s *mac_access_req_msg_ptr = NULL;
  lte_mac_access_cnf_msg_s mac_access_cnf_msg;
  lte_rrc_handover_completed_ind_s *ho_completed_ind_msg_ptr = NULL;
  lte_rrc_active_eps_bearer_update_ind_s *eps_list_msg_ptr = NULL;
  lte_rrc_dlm_processed_indi_s *dlm_processed_ind_ptr = NULL;
  lte_rrc_config_completed_ind_s *config_completed_ind_ptr = NULL;
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
  encoded_pdu.length = sizeof(reconfig_with_ho_pcell_to_pcell_scell);
  encoded_pdu.value = reconfig_with_ho_pcell_to_pcell_scell;
  lte_rrc_config_test_encoded_pdu = encoded_pdu;

  TF_MSG("\nConfig: HO from Pcell to Pcell+Scell Test Begins \n");
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

  /* Send the CFG_CNFI to Config module there by mimicing the action of LLC */
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_cnf_msg.msg_hdr,LTE_RRC_CFG_CNFI);
  /* Setting the fields in the CFG_CNFI sent by LLC */
  cfg_cnf_msg.proc = LTE_RRC_PROC_CONFIG;
  cfg_cnf_msg.status = E_SUCCESS;
  cfg_cnf_msg.trans_id = 0;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf_msg, sizeof(cfg_cnf_msg));

  /*Expect a TrmPriorityChangeInd*/
  TF_MSG("Checking for LTE_RRC_TRM_PRIORITY_CHANGE_INDI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_trm_priority_change_indi_s) == buf_len);
  priority_change_indi_ptr = (lte_rrc_trm_priority_change_indi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_TRM_PRIORITY_CHANGE_INDI == priority_change_indi_ptr->msg_hdr.id);

  /* Catch the MEAS_CFG_REQI message sent by Config to Meas module */
  lte_rrc_utf_get_next_int_msg((byte **)&meas_cfg_req_ptr,&buf_len);
  /* Make sure the MEAS_CFG_REQI message is valid */
  TF_ASSERT(sizeof(lte_rrc_meas_cfg_reqi_s) == buf_len);
  TF_ASSERT(meas_cfg_req_ptr != NULL);
  TF_ASSERT(meas_cfg_req_ptr->cause == LTE_RRC_MEAS_CFG_REQI_CAUSE_HANDOVER);

  /* Send the MEAS_CFG_CNFI to Config module there by mimicing the action of Meas module */
  lte_rrc_init_default_hdr(&meas_cfg_cnf.msg_hdr,LTE_RRC_MEAS_CFG_CNFI);
  meas_cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_cnf, sizeof(meas_cfg_cnf));

  /* Catch the Reconfig complete message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&mh_msg_ptr,&buf_len);
  /* Make sure the Reconfig complete message is valid */
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);

  /* Catch the Access Request message sent by Config to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_access_req_msg_ptr,&buf_len); 
  /* Make sure the Access Request message is valid */
  TF_ASSERT(sizeof(lte_mac_access_req_msg_s) == buf_len);
  TF_ASSERT(mac_access_req_msg_ptr->access_info.access_reason == LTE_MAC_ACCESS_REASON_HAND_OVER);
  TF_ASSERT(mac_access_req_msg_ptr->access_info.raid == 1);
  TF_ASSERT(mac_access_req_msg_ptr->access_info.raid_mask == 1);
  /* After Config sends Access request, assert its in WAIT_FOR_MAC_ACCESS_CNF state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__WAIT_FOR_MAC_ACCESS_CNF);

  /* Setting the header for MAC_ACCESS_CNF */
  lte_rrc_init_default_hdr(&mac_access_cnf_msg.hdr,LTE_MAC_ACCESS_CNF);
  /* Send the MAC_ACCESS_CNF to Config module there by mimicing the action of MAC
    when RACH is successful */
  lte_rrc_utf_send_msg((byte*)&mac_access_cnf_msg, sizeof(mac_access_cnf_msg));

  /* Catch the active EPS bearer list sent by Config to NAS */
  lte_rrc_utf_get_next_ext_msg((byte **)&eps_list_msg_ptr,&buf_len);
  /* Make sure the NAS message sent by Config is valid */
  TF_ASSERT(sizeof(lte_rrc_active_eps_bearer_update_ind_s) == buf_len);
  /* Make sure passed EPS bearer list is valid */
  TF_ASSERT(eps_list_msg_ptr->num_active_bearers == 1);
  TF_ASSERT(eps_list_msg_ptr->active_eps_bearers[0].eps_id== 5);
  TF_ASSERT(eps_list_msg_ptr->active_eps_bearers[0].rb_id== 5);

  /* Catch the HO Completed indication sent by Config */
  lte_rrc_utf_get_next_int_msg((byte **)&ho_completed_ind_msg_ptr,&buf_len);
  /* Make sure the HO Completed indication is valid */
  TF_ASSERT(sizeof(lte_rrc_handover_completed_ind_s) == buf_len);
  TF_ASSERT(ho_completed_ind_msg_ptr->phy_cell_id == 10);
  TF_ASSERT(ho_completed_ind_msg_ptr->dl_freq == 36000);
  TF_ASSERT(ho_completed_ind_msg_ptr->ul_freq == 36199);
  TF_ASSERT(ho_completed_ind_msg_ptr->dl_bandwidth == 25);

  /*Expect a TrmPriorityChangeInd*/
  TF_MSG("Checking for LTE_RRC_TRM_PRIORITY_CHANGE_INDI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_trm_priority_change_indi_s) == buf_len);
  priority_change_indi_ptr = (lte_rrc_trm_priority_change_indi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_TRM_PRIORITY_CHANGE_INDI == priority_change_indi_ptr->msg_hdr.id);

  /* Catch the dlm processed indication message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&dlm_processed_ind_ptr,&buf_len);
  /* Make sure the processed indication message is valid */
  TF_ASSERT(dlm_processed_ind_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  /* Catch the config completed indication message sent by Config to L2 */
  lte_rrc_utf_get_next_ext_msg((byte **)&config_completed_ind_ptr,&buf_len);
  /* Make sure the config completed indication message is valid */
  TF_ASSERT(config_completed_ind_ptr->msg_hdr.id == LTE_RRC_CONFIG_COMPLETED_IND);

  /* After Config completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);
  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(mh_msg_ptr->ul_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);
  TF_MSG("\nConfig: HO from Pcell to Pcell+Scell Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcConfigHOWithPcellToPcellScellTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigCaScellAddConfigureA1Test);

void LteRrcConfigCaScellAddConfigureA1Test::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, external_umid_list, 3);
  lte_rrc_config_test_stub_init();
  TF_STUB(lte_rrc_llc_get_epsbearer_info) = lte_rrc_config_ca__nas_info_test_get_eps_info;
}

void LteRrcConfigCaScellAddConfigureA1Test::Test()
{
  lte_rrc_mh_dlm_s dl_msg;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_cfg_reqi_s* cfg_req_msg_ptr = NULL;
  lte_rrc_cfg_cnfi_s cfg_cnf_msg;
  uint32 buf_len = 0;
  lte_rrc_send_ul_msg_reqi_s *mh_msg_ptr = NULL;
  lte_rrc_meas_cfg_reqi_s* meas_cfg_req_msg_ptr = NULL;
  lte_rrc_meas_cfg_cnfi_s meas_cfg_cnf;
  lte_rrc_dl_data_ind_s *nas_msg_ptr = NULL;
  lte_rrc_active_eps_bearer_update_ind_s *eps_list_msg_ptr = NULL;
  lte_rrc_dlm_processed_indi_s *dlm_processed_ind_ptr = NULL;
  lte_rrc_config_completed_ind_s *config_completed_ind_ptr = NULL;
  int decode_status = 0;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nRe-Config Request with Scell add Test Begins \n");
  /* The first iteration through the for loop would setup DRB3 and do checks 
    related to the processing DRB setup. The second iteration would release
    the DRB3 that was setup in the first iteration. In processing both the 
    reconfig messages, the stub function for returning the active EPS bearer
    list should be called. */

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&dl_msg.msg_hdr,LTE_RRC_RRC_CONNECTION_RECONFIGURATION_DLM);
  /* Setting the encoded message to the byte string corresponding to radio 
   resource configuration. Please refer to lte_rrc_config_ota_msg.h for 
   all Config OTA message definitions */ 
 
  encoded_pdu.length = sizeof(reconfig_ca_scell_add_configure_a1);
  encoded_pdu.value = reconfig_ca_scell_add_configure_a1;
  lte_rrc_config_test_encoded_pdu = encoded_pdu;
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

  /* Catch the Meas Config request sent by Config to Meas proc */
  lte_rrc_utf_get_next_int_msg((byte **)&meas_cfg_req_msg_ptr,&buf_len);
  /* Make sure the Config request is valid */
  TF_ASSERT(sizeof(lte_rrc_meas_cfg_reqi_s) == buf_len);
  /* After Config completes processing, assert its in WAIT_FOR_MEAS_CFG_CNFI state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__WAIT_FOR_MEAS_CFG_CNFI);

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&meas_cfg_cnf.msg_hdr,LTE_RRC_MEAS_CFG_CNFI);
  /* Setting the fields in the CFG_CNFI sent by Meas procedure */
  meas_cfg_cnf.status = E_SUCCESS;
  /* Send the CFG_CNFI to Config module there by mimicing the action of Meas */
  lte_rrc_utf_send_msg((byte*)&meas_cfg_cnf, sizeof(meas_cfg_cnf));

  /* Catch the NAS message sent by Config to NAS */
  lte_rrc_utf_get_next_ext_msg((byte **)&nas_msg_ptr,&buf_len);
  /* Make sure the NAS message sent by Config is valid */
  TF_ASSERT(sizeof(lte_rrc_dl_data_ind_s) == buf_len);
  //TF_ASSERT(nas_msg_ptr->nas_msg_container.msg_size == 3);

  /* Catch the active EPS bearer list sent by Config to NAS */
  lte_rrc_utf_get_next_ext_msg((byte **)&eps_list_msg_ptr,&buf_len);
  /* Make sure the NAS message sent by Config is valid */
  TF_ASSERT(sizeof(lte_rrc_active_eps_bearer_update_ind_s) == buf_len);
  /* Make sure passed EPS bearer list is valid */
  TF_ASSERT(eps_list_msg_ptr->num_active_bearers == 1);
  TF_ASSERT(eps_list_msg_ptr->active_eps_bearers[0].eps_id == 3);


   TF_MSG("\Check\n");

  /* Catch the Reconfig complete message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&mh_msg_ptr,&buf_len);
  /* Make sure the Reconfig complete message is valid */
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);

  /* Catch the dlm processed indication message sent by Config to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&dlm_processed_ind_ptr,&buf_len);
  /* Make sure the processed indication message is valid */
  TF_ASSERT(dlm_processed_ind_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  /* Catch the config completed indication message sent by Config to L2 */
  lte_rrc_utf_get_next_ext_msg((byte **)&config_completed_ind_ptr,&buf_len);
  /* Make sure the config completed indication message is valid */
  TF_ASSERT(config_completed_ind_ptr->msg_hdr.id == LTE_RRC_CONFIG_COMPLETED_IND);

  /* After Config completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);
  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(mh_msg_ptr->ul_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("\n Re-Config Request with Scell add Test Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcConfigCaScellAddConfigureA1Test::Teardown()
{
  lte_rrc_utf_teardown();
}

