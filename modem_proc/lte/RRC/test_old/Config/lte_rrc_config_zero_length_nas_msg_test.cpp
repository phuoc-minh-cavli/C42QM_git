/*!
  @file lte_rrc_config_zero_length_nas_msg_test.cpp

  @brief
  Unit test code for the Config procedure that tests the processing of 
  a Reconfig message containing NAS Dedicated info along radio resource 
  configuration where the first element in the NAS Dedicated info list is empty.

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
06/09/10   aramani Ported code to Objective Systems 
03/31/09   mm     Initial Revision
           
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
  TF_PROTO_STUB(void, lte_rrc_llc_get_epsbearer_info, lte_rrc_active_eps_bearer_update_ind_s *);
}

static uint32 ext_umid_list[] = 
{
  LTE_RRC_DL_DATA_IND,
  LTE_RRC_ACTIVE_EPS_BEARER_UPDATE_IND,
  LTE_RRC_CONFIG_COMPLETED_IND
};

/*===========================================================================

                                FUNCTIONS

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_rrc_config_zero_nas_msg_test_get_eps_info

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
void lte_rrc_config_zero_nas_msg_test_get_eps_info
(
  lte_rrc_active_eps_bearer_update_ind_s *eps_list_msg_ptr  /*!< EPS bearer Info */
)
{
  eps_list_msg_ptr->active_eps_bearers[0].eps_id = 3;
  eps_list_msg_ptr->active_eps_bearers[0].rb_id = 3;
  eps_list_msg_ptr->num_active_bearers = 1;
} /* lte_rrc_config_zero_nas_msg_test_get_eps_info() */


/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigZeroLengthNASMsgTest);

void LteRrcConfigZeroLengthNASMsgTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, ext_umid_list, 3);
  lte_rrc_config_test_stub_init();
  TF_STUB(lte_rrc_llc_get_epsbearer_info) = 
    lte_rrc_config_zero_nas_msg_test_get_eps_info;
}

void LteRrcConfigZeroLengthNASMsgTest::Test()
{
  lte_rrc_mh_dlm_s dl_msg;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_cfg_reqi_s* cfg_req_msg_ptr = NULL;
  lte_rrc_cfg_cnfi_s cfg_cnf_msg;
  uint32 buf_len = 0, num_of_nas_message_elements = 2;
  lte_rrc_send_ul_msg_reqi_s *mh_msg_ptr = NULL;
  lte_rrc_dl_data_ind_s *nas_msg_ptr = NULL;
  lte_rrc_active_eps_bearer_update_ind_s *eps_list_msg_ptr = NULL;
  lte_rrc_dlm_processed_indi_s *dlm_processed_ind_ptr = NULL;
  lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs *reconfig_r8_ie_ptr;
  lte_rrc_config_completed_ind_s *config_completed_ind_ptr = NULL;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&dl_msg.msg_hdr,LTE_RRC_RRC_CONNECTION_RECONFIGURATION_DLM);
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_config_ota_msg.h for 
     all Config OTA message definitions */ 
  encoded_pdu.length = sizeof(reconfig_meas_nas_info_msg_zero_length);
  encoded_pdu.value = reconfig_meas_nas_info_msg_zero_length;

  /* Initialise the currently used encoded message*/
  lte_rrc_config_test_encoded_pdu = encoded_pdu;

  TF_MSG("\nZero length NAS message Test Begins \n");
  /* On receiving a DLM from message handler, we should stay in the same state */
  TF_ASSERT(lte_rrc_config_get_state() == LTE_RRC_CONFIG_SM__INITIAL);

  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  dl_dcch_msg_ptr =  (lte_rrc_osys_DL_DCCH_Message*)
  lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length,
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  dl_msg.dl_msg_ptr = dl_dcch_msg_ptr;
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

  while (num_of_nas_message_elements > 0)
  {
    /* Catch the NAS message sent by Config to NAS */
    lte_rrc_utf_get_next_ext_msg((byte **)&nas_msg_ptr,&buf_len);
    /* Make sure the NAS message sent by Config is valid */
    TF_ASSERT(sizeof(lte_rrc_dl_data_ind_s) == buf_len);
    //TF_ASSERT(nas_msg_ptr->nas_msg_container.msg_size == 3);
    num_of_nas_message_elements--;
  }

  /* Catch the active EPS bearer list sent by Config to NAS */
  lte_rrc_utf_get_next_ext_msg((byte **)&eps_list_msg_ptr,&buf_len);
  /* Make sure the NAS message sent by Config is valid */
  TF_ASSERT(sizeof(lte_rrc_active_eps_bearer_update_ind_s) == buf_len);
  /* Make sure passed EPS bearer list is valid */
  TF_ASSERT(eps_list_msg_ptr->num_active_bearers == 1);
  TF_ASSERT(eps_list_msg_ptr->active_eps_bearers[0].eps_id == 3);
  TF_ASSERT(eps_list_msg_ptr->active_eps_bearers[0].rb_id == 3);

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
  TF_MSG("\nZero length NAS message Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcConfigZeroLengthNASMsgTest::Teardown()
{
  lte_rrc_utf_teardown();
}

