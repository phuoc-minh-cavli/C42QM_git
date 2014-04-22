/*!
  @file lte_rrc_cre_ip_chk_failure_and_srb1_resume_failure_test.cpp

  @brief
  Unit test code for CRE procedure that tests the handling of CONN_MODE_FAILURE_INDI
  due to IP check failure from MH followed by CRE where resuming SRB1 fails.

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


$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/cre/
lte_rrc_cre_ip_chk_failure_and_srb1_resume_failure_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/09   mm     Initial version
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "__lte_rrc_cre.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_rrc_cre.h"
#include "lte_rrc_sec.h"
#include "lte_mac_msg.h"
#include "lte_mac_rrc.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_cre_test_common.h"
#include <tf_stub.h>


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

//external messages generated by code under test
static uint32 ext_umid_list[] = 
{ 
  LTE_MAC_ACCESS_REQ,
  LTE_CPHY_CANCEL_CONN_REQ
};

extern "C"
{
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s*);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCreIPChkFailureAndSRB1ResumeFailureTest);

void LteRrcCreIPChkFailureAndSRB1ResumeFailureTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CRE_SM, ext_umid_list, 2);
  lte_rrc_cre_test_stub_init();
}

void LteRrcCreIPChkFailureAndSRB1ResumeFailureTest::Test()
{
  lte_rrc_conn_mode_failure_indi_s conn_mode_failure_ind;
  lte_rrc_cre_started_indi_s *cre_started_indi_ptr = NULL;
  lte_rrc_sib_abort_reqi_s* sib_abort_reqi_ptr = NULL;
  lte_rrc_cfg_reqi_s* cfg_req_msg_ptr = NULL;
  lte_rrc_cfg_cnfi_s cfg_cnfi_msg;
  lte_rrc_initiate_cell_sel_reqi_s *cel_sel_reqi_ptr = NULL;
  lte_rrc_camped_indi_s camped_indi_msg;
  lte_mac_access_req_msg_s *mac_access_req_ptr = NULL;
  lte_mac_access_cnf_msg_s mac_access_cnf;
  lte_rrc_osys_UL_CCCH_Message *ul_ccch_msg_ptr = NULL;
  lte_cphy_cancel_conn_req_s *cphy_cancel_conn_req_ptr = NULL;
  lte_rrc_initiate_conn_rel_indi_s *initiate_conn_rel_indi_ptr = NULL;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_mh_dlm_s mh_dlm;
  uint64 crnti_integer_val = 0;
  uint32 buf_len = 0; int decode_status = 0;

  /* ---------------------------------------------------------------------- */
 
  TF_MSG("\nCRE IP Check failure and SRB1 resume failure test begins \n");

  /* Make sure CRE is in initial state on receiving a max re-transmission indication */
  TF_ASSERT(lte_rrc_cre_get_state() == LTE_RRC_CRE_SM__INITIAL);

  /* Setting the header for conn_mode_failure_ind */
  lte_rrc_init_default_hdr(&conn_mode_failure_ind.msg_hdr, LTE_RRC_CONN_MODE_FAILURE_INDI);
  conn_mode_failure_ind.conn_mode_failure_cause = LTE_RRC_CONN_MODE_FAILURE_CAUSE_HO;
  conn_mode_failure_ind.ho_failure_cause = LTE_RRC_HO_FAILURE_CAUSE_INVALID_CFG;
  lte_rrc_utf_send_msg((byte*)&conn_mode_failure_ind, sizeof(conn_mode_failure_ind));


  /* Catch SIB abort request */
  lte_rrc_utf_get_next_int_msg((byte **)&sib_abort_reqi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_sib_abort_reqi_s) == buf_len);
  TF_ASSERT(LTE_RRC_SIB_ABORT_REQI == sib_abort_reqi_ptr->msg_hdr.id);

  /* Catch the Config request sent by CRE to LLC */
  lte_rrc_utf_get_next_int_msg((byte **)&cfg_req_msg_ptr,&buf_len);
  /* Make sure the Config request is valid */
  TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);
  TF_ASSERT(cfg_req_msg_ptr != NULL);
  TF_ASSERT(cfg_req_msg_ptr->num_cfg == 1);
  TF_ASSERT(cfg_req_msg_ptr->cfg_info[0].cfg_type == LTE_RRC_CFG_RLF);
  TF_ASSERT(cfg_req_msg_ptr->cfg_info[0].cfg_u.susp_cfg.revert_l2_config == FALSE);

  /* Catch the CRE started internal indication thrown by CRE */
  lte_rrc_utf_get_next_int_msg((byte **)&cre_started_indi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cre_started_indi_s) == buf_len);

  /* Assert that CRE is in WAIT_FOR_CFG_CNFI state */
  TF_ASSERT(lte_rrc_cre_get_state() == LTE_RRC_CRE_SM__WAIT_FOR_CFG_CNFI);
  TF_MSG("\nCRE cfg request sent to LLC and CRE Module in WAIT_FOR_CFG_CNFI state\n");

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_cnfi_msg.msg_hdr, LTE_RRC_CFG_CNFI);
  /* Setting the fields in the CFG_CNFI sent by LLC */
  cfg_cnfi_msg.proc = LTE_RRC_PROC_CRE;
  cfg_cnfi_msg.status = E_SUCCESS;
  cfg_cnfi_msg.trans_id = 0;
  /* Send the CFG_CNFI to CRE module there by mimicing the action of LLC */
  lte_rrc_utf_send_msg((byte*)&cfg_cnfi_msg, sizeof(cfg_cnfi_msg));

  /* Catch the Initiate Cell selection message sent to CSP */
  lte_rrc_utf_get_next_int_msg((byte **)&cel_sel_reqi_ptr,&buf_len);
  /* Make sure the message is valid */
  TF_ASSERT(sizeof(lte_rrc_initiate_cell_sel_reqi_s) == buf_len);
  TF_ASSERT(cel_sel_reqi_ptr != NULL);
  TF_ASSERT(cel_sel_reqi_ptr->cause == LTE_RRC_INITIATE_CELL_SEL_RL_FAILURE);

  /* Assert that CRE is in WAIT_FOR_UE_TO_CAMP state */
  TF_ASSERT(lte_rrc_cre_get_state() == LTE_RRC_CRE_SM__WAIT_FOR_UE_TO_CAMP);
  TF_MSG("\nInititate Cell select request sent to CSP and "
         "CRE Module in WAIT_FOR_UE_TO_CAMP state\n");

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&camped_indi_msg.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_indi_msg.reason = LTE_RRC_CAMPED_CELL_SELECTION_RLF;
  camped_indi_msg.camped_cell.freq = 10689;
  camped_indi_msg.camped_cell.phy_cell_id = 25;
  /* Send the CAMPED_INDI to CRE module there by mimicing the action of CSP */
  lte_rrc_utf_send_msg((byte*)&camped_indi_msg, sizeof(camped_indi_msg));

  TF_STUB(lte_rrc_csp_get_serving_cell_params) = lte_rrc_cre_test_csp_get_serving_cell_params;

  /* Catch the MAC_ACCESS_REQ message sent by CRE to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_access_req_ptr,&buf_len);
  /* Make sure the message is valid */
  TF_ASSERT(sizeof(lte_mac_access_req_msg_s) == buf_len);
  TF_ASSERT(mac_access_req_ptr != NULL);
  TF_ASSERT(mac_access_req_ptr->access_info.
            access_reason == LTE_MAC_ACCESS_REASON_RADIO_FAILURE);
  TF_ASSERT(mac_access_req_ptr->access_info.raid == LTE_INVALID_RA_ID);
  TF_ASSERT(mac_access_req_ptr->access_info.raid_mask == LTE_INVALID_RA_ID_MASK);

  /* Decode the sent ReestablishmentRequest within the MAC_ACCESS_REQ */

  ul_ccch_msg_ptr = (lte_rrc_osys_UL_CCCH_Message *)
  lte_rrc_osys_asn1_decode_pdu(mac_access_req_ptr->access_info.msg_ptr, 
                               mac_access_req_ptr->access_info.msg_length,
                               UL_CCCH_Message,
                               &decode_status);

  TF_ASSERT(ul_ccch_msg_ptr != NULL);
  /* Make sure the Connection Reestablishment Request sent is valid */
  TF_ASSERT(ul_ccch_msg_ptr->message.t == T_lte_rrc_osys_UL_CCCH_MessageType_c1);
  TF_ASSERT(ul_ccch_msg_ptr->message.u.c1->t == T_lte_rrc_osys_UL_CCCH_MessageType_c1_rrcConnectionReestablishmentRequest);
  TF_ASSERT(ul_ccch_msg_ptr->message.u.c1->u.rrcConnectionReestablishmentRequest->
    criticalExtensions.t == T_lte_rrc_osys_RRCConnectionReestablishmentRequest_criticalExtensions_rrcConnectionReestablishmentRequest_r8);
  TF_ASSERT(ul_ccch_msg_ptr->message.u.c1->u.rrcConnectionReestablishmentRequest->
            criticalExtensions.u.rrcConnectionReestablishmentRequest_r8->ue_Identity.
            physCellId == 501);
  crnti_integer_val = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint64),&ul_ccch_msg_ptr->message.
                                                    u.c1->u.rrcConnectionReestablishmentRequest->
                                                    criticalExtensions.u.rrcConnectionReestablishmentRequest_r8->
                                                    ue_Identity.c_RNTI);
  TF_ASSERT(crnti_integer_val == 5);
  TF_ASSERT(ul_ccch_msg_ptr->message.u.c1->u.rrcConnectionReestablishmentRequest->
      criticalExtensions.u.rrcConnectionReestablishmentRequest_r8->
      reestablishmentCause == lte_rrc_osys_ReestablishmentCause_reconfigurationFailure);

  /* Assert that CRE is in WAIT_FOR_CONN_RE state */
  TF_ASSERT(lte_rrc_cre_get_state() == LTE_RRC_CRE_SM__WAIT_FOR_CONN_RE);
  TF_MSG("\nReestablishmentRequest sent and CRE Module in WAIT_FOR_CONN_RE state\n");

  /* Send ACCESS_CNF to indicate RACH succeeded */
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&mac_access_cnf.hdr, LTE_MAC_ACCESS_CNF);
  mac_access_cnf.crnti_included = TRUE;
  mac_access_cnf.crnti = 10;
  /* Send the ACCESS_CNF to CRE module there by mimicing the action of MAC */
  lte_rrc_utf_send_msg((byte*)&mac_access_cnf, sizeof(mac_access_cnf));

  encoded_pdu.length = sizeof(cre_reestablishment_msg);
  encoded_pdu.value = cre_reestablishment_msg;
  /* Construct asn1 pdu to be sent to the CRE module after decoding the 
     encoded pdu*/
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr,
                           LTE_RRC_RRC_CONNECTION_REESTABLISHMENT_DLM);
   
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_DL_CCCH_Message *)
  lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, DL_CCCH_Message, &decode_status);
  mh_dlm.pdu_num = lte_rrc_osys_DL_CCCH_Message_PDU;
  /* Send the MH DLM */
  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  /* Catch the Config request sent by CRE to LLC */
  lte_rrc_utf_get_next_int_msg((byte **)&cfg_req_msg_ptr,&buf_len);
  /* Make sure the Config request is valid */
  TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);
  TF_ASSERT(cfg_req_msg_ptr != NULL);
  TF_ASSERT(cfg_req_msg_ptr->cfg_info[0].cfg_type == LTE_RRC_CFG_DED);

  /* Assert that CRE is in WAIT_FOR_CFG_CNFI state */
  TF_ASSERT(lte_rrc_cre_get_state() == LTE_RRC_CRE_SM__WAIT_FOR_CFG_CNFI);
  TF_MSG("\nCRE cfg request sent to LLC and CRE Module in WAIT_FOR_CFG_CNFI state\n");

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_cnfi_msg.msg_hdr, LTE_RRC_CFG_CNFI);
  /* Setting the fields in the CFG_CNFI sent by LLC */
  cfg_cnfi_msg.proc = LTE_RRC_PROC_CRE;
  cfg_cnfi_msg.status = E_DATA_INVALID;
  cfg_cnfi_msg.failure_type = LTE_RRC_CFG_FAILURE_NORMAL;
  cfg_cnfi_msg.trans_id = 0;
  /* Send the CFG_CNFI to CRE module there by mimicing the action of LLC */
  lte_rrc_utf_send_msg((byte*)&cfg_cnfi_msg, sizeof(cfg_cnfi_msg));

  /* catch the initiate connection release indication thrown by CRE */
  lte_rrc_utf_get_next_int_msg((byte **)&initiate_conn_rel_indi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_initiate_conn_rel_indi_s) == buf_len);
  TF_ASSERT(initiate_conn_rel_indi_ptr != NULL);
  TF_ASSERT(initiate_conn_rel_indi_ptr->rel_reason == LTE_RRC_CONN_REL_CRE_FAILURE);

  /* Catch the LTE_CPHY_CANCEL_CONN_REQ message sent by CRE to ML1 */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cancel_conn_req_ptr,&buf_len);
  /* Make sure the message is valid */
  TF_ASSERT(sizeof(lte_cphy_cancel_conn_req_s) == buf_len);
  TF_ASSERT(cphy_cancel_conn_req_ptr != NULL);
  TF_ASSERT(cphy_cancel_conn_req_ptr->msgr_hdr.id == LTE_CPHY_CANCEL_CONN_REQ);

  /* Assert that CRE is in INITIAL state */
  TF_ASSERT(lte_rrc_cre_get_state() == LTE_RRC_CRE_SM__INITIAL);
  lte_rrc_osys_asn1_free_pdu(ul_ccch_msg_ptr, lte_rrc_osys_UL_CCCH_Message_PDU);
  lte_rrc_cre_test_free_sib_ptr();

  TF_MSG("\nCRE IP Check failure and SRB1 resume failure test ends \n");
  lte_rrc_utf_reset(); 

}

void LteRrcCreIPChkFailureAndSRB1ResumeFailureTest::Teardown()
{
  lte_rrc_utf_teardown();
}



