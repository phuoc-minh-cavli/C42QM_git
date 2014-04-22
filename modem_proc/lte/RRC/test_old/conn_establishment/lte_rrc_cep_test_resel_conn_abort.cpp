/*!
  @file lte_rrc_cep_test_conn_abort.cpp


  @brief
  Unit test code for CEP Connnection Reject.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/conn_establishment/lte_rrc_cep_test_resel_conn_abort.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/02/08   dd     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "__lte_rrc_cep.h"
#include "lte_rrc_cep.h"
#include "__lte_rrc_controller.h"
#include "stm2.h"
#include "lte_rrc_controller.h"
#include "lte_mac_msg.h"
#include "lte_mac_rrc.h"
#include "lte_rrc_cep_test_asn1_common.h"
//#if 0

#include "lte_rrc_osys_asn1util.h"
//#endif
#include "lte_rrc_sib.h"
#include "lte_rrc_cepi.h"
}

#include "lte_rrc_cep_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"

#include <tf_stub.h>
#define LTE_RRC_CEP_TEST_WAIT_TIME 10
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module 
lte_rrc_cep_test_transitions.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */
  uint32 ext_umid_list[NUM_MAX_UMIDS];
  uint32 ext_umid_count;
  stm_state_t rrc_controller_state;

} lte_rrc_cep_test_transitions_type_s;

static lte_rrc_conn_est_req_s conn_est_req;
//static lte_rrc_mh_dlm_s conn_reject_dlm;
//static lte_rrc_cfg_cnfi_s cfg_cnf;

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_cep_test_transitions.cpp
*/


extern "C"
{

  //TF_DECLARE_STUB( stm_state_t, lte_rrc_controller_get_state, void);
/*
  static stm_state_t lte_rrc_test_cep_stub_controller_get_state(void)
  {
    return (lte_rrc_cep_test.rrc_controller_state);
  }
*/     

}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/



/*===========================================================================

                                UNIT TEST

=======================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCEPReselConnAbortTest1);

void LteRrcCEPReselConnAbortTest1::Setup()
{
  int ext_count = 0;
  //TF_STUB(lte_rrc_controller_get_state) = lte_rrc_test_cep_stub_controller_get_state;
  //lte_rrc_test_cep_init();
  //lte_rrc_cep_test.ext_umid_list[0] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_list[0] = LTE_MAC_ACCESS_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[1] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[2] = LTE_CPHY_CANCEL_CONN_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[3] = LTE_MAC_CANCEL_CONN_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[4] = LTE_MAC_ACCESS_ABORT_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;

  
  lte_rrc_utf_setup(LTE_RRC_CEP_SM, 
                    &(lte_rrc_cep_test.ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_cep_init();
}

void LteRrcCEPReselConnAbortTest1::Test()
{
  lte_rrc_conn_est_cnf_s        *conn_est_cnf_ptr = NULL;
  lte_rrc_conn_establishment_failure_indi_s *conn_est_failure_indi_ptr = NULL;
  lte_rrc_conn_establishment_started_indi_s *conn_est_started_indi_ptr = NULL;
  lte_rrc_cep_test_conn_req_s my_msg;
  lte_mac_access_req_msg_s *access_req_ptr= NULL;
  //lte_rrc_cfg_reqi_s *cfg_req_ptr = NULL;
  lte_rrc_proceed_with_resel_indi_s *proceed_with_resel_indi_ptr = NULL;
  //lte_rrc_connected_indi_s *connected_indi_ptr = NULL;
  lte_rrc_osys_RRCConnectionRequest *conn_req_msg_ptr = NULL;
  lte_rrc_osys_UL_CCCH_Message *ul_ccch_msg_ptr = NULL;
  lte_rrc_osys_InitialUE_Identity *ue_identity_ptr = NULL;
  /*Set the Est cause to MO_DATA*/
  lte_rrc_est_cause_e est_cause = LTE_RRC_EST_CAUSE_MO_DATA;
  lte_rrc_osys_SystemInformationBlockType2 *sib2 = NULL;
  lte_rrc_cell_resel_started_indi_s resel_started_indi ;
  //lte_mac_access_abort_cnf_msg_s access_abort_cnf;
  lte_rrc_conn_abort_req_s conn_abort_req;
  lte_rrc_cep_s *i_ptr = NULL;
  lte_mac_access_abort_req_msg_s *access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s access_abort_cnf;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr = NULL;

  lte_cphy_cancel_conn_req_s * cphy_cancel_conn_req_ptr = NULL;
  lte_mac_cancel_conn_req_msg_s *mac_cancel_conn_req_ptr = NULL;
  lte_rrc_trm_priority_change_indi_s *priority_change_indi_ptr = NULL;

  byte *buf_ptr;
  uint32 buf_len; 
  int decode_status = 0; 

  TF_MSG("Start of test");
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_MSG("Checking for the INACTIVE test");
  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__INACTIVE);

  // ----------------------------------------------------------------------
  
  //lte_rrc_cep_test.rrc_controller_state = 
   // LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;

  /*Set the RRC controller in Camped state*/
  lte_rrc_test_cep_set_controller_state(LTE_RRC_CONTROLLER_SM__IDLE_CAMPED);  

  /*Set the Access class in the USIM to invalid*/
  lte_rrc_test_cep_set_controller_access_class(LTE_RRC_INVALID_ACCESS_CLASS);

  /*!@todo provide the proper SIB2 info; so that Access Barring check passes*/
  /*Access barring for Mobile Originated calls*/
  sib2 = (lte_rrc_osys_SystemInformationBlockType2 *)lte_rrc_test_cep_stub_get_cell_sib(LTE_RRC_SIB2_BITMASK,0);
  //sib2->bit_mask = lte_rrc_accessBarringInformation_present;
  sib2->m.ac_BarringInfoPresent = 1;
  //sib2->accessBarringInformation.bit_mask = 0; 
  sib2->ac_BarringInfo.m.ac_BarringForMO_DataPresent = 0; 
  sib2->ac_BarringInfo.m.ac_BarringForMO_SignallingPresent = 0; 
  /*!@todo: Figure out a way to add the probabilitic thing later*/
  /*
  sib2->accessBarringInformation.bit_mask = 
    lte_rrc_accessBarringForOriginatingCalls_present;
  sib2->accessBarringInformation.accessBarringForOriginatingCalls.accessProbabilityFactor = 
    lte_rrc_osys_AC_BarringConfig_ac_BarringFactor_p95;
  sib2->accessBarringInformation.accessBarringForOriginatingCalls.accessBarringTime = 
    lte_rrc_osys_AC_BarringConfig_ac_BarringTime_s4;
    */
  /*Setting the T300 timer value to 400 ms*/
  sib2->ue_TimersAndConstants.t300 = lte_rrc_osys_UE_TimersAndConstants_t300_ms400;


  // On receiving a conn est req, we should stay in the same state
  TF_MSG("Sending the ConnEstReq");
  lte_rrc_init_default_hdr(&conn_est_req.msg_hdr, LTE_RRC_CONN_EST_REQ);
  conn_est_req.est_cause = est_cause;
  lte_rrc_utf_send_msg((byte*)&conn_est_req, sizeof(conn_est_req));

  lte_rrc_utf_wait_for_done();

    /*Make sure that the CEP SM is in Access state*/
  TF_MSG("Checking that the CEP SM is in ACCESS state");
  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__ACCESS);

  TF_MSG("Waiting for ConnEstCnf");
 

  /*
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_est_cnf_s) == buf_len);
  conn_est_cnf_ptr = (lte_rrc_conn_est_cnf_s *)(void*) buf_ptr;
  TF_ASSERT((conn_est_cnf_ptr->msg_hdr.id) ==  LTE_RRC_CONN_EST_CNF);
  
 */

  /*Expect a Connection Establishment Started Ind from CEP*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_establishment_started_indi_s) == buf_len);
  conn_est_started_indi_ptr = 
    (lte_rrc_conn_establishment_started_indi_s *)(void*) buf_ptr;
  TF_ASSERT(conn_est_started_indi_ptr !=NULL)

  TF_ASSERT((conn_est_started_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_ESTABLISHMENT_STARTED_INDI);

  /*Expect a TrmPriorityChangeInd*/
  TF_MSG("Checking for LTE_RRC_TRM_PRIORITY_CHANGE_INDI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_trm_priority_change_indi_s) == buf_len);
  priority_change_indi_ptr = (lte_rrc_trm_priority_change_indi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_TRM_PRIORITY_CHANGE_INDI == priority_change_indi_ptr->msg_hdr.id);

  /*Expect a Access Req from CEP*/
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_mac_access_req_msg_s) == buf_len);
  access_req_ptr = (lte_mac_access_req_msg_s *)(void*) buf_ptr;
  TF_ASSERT(access_req_ptr !=NULL);

  TF_ASSERT((access_req_ptr->hdr.id) ==  LTE_MAC_ACCESS_REQ);

  /*We have indeed received an Access Request fron CEP*/

  /*Validate the Contents*/
  TF_ASSERT(access_req_ptr->access_info.access_reason == 
            LTE_MAC_ACCESS_REASON_CONNECTION_REQ);
  TF_ASSERT(access_req_ptr->access_info.raid == 
            LTE_INVALID_RA_ID);
  /*!@todo Validate the UEID later*/
  TF_ASSERT(access_req_ptr->access_info.msg_ptr);

  lte_rrc_test_cep_decode_conn_req(access_req_ptr->access_info.msg_ptr,
                                   (void *)(&my_msg),
                                   access_req_ptr->access_info.msg_length);

  TF_ASSERT( my_msg.msg_c1_choice == 1);
  /*T_lte_rrc_osys_UL_CCCH_MessageType_c1  = 1*/

  TF_ASSERT( my_msg.msg_choice == 2);
  /*lte_rrc_rrcConnectionRequest_chosen = 2*/

  TF_ASSERT( my_msg.conn_req_r8_choice == 1);
  /*T_lte_rrc_osys_RRCConnectionRequest_criticalExtensions_rrcConnectionRequest_r8 = 1*/

  TF_ASSERT( my_msg.est_cause == est_cause);

                                   

//#if 0
  /*Decode the ConnReq message*/
  ul_ccch_msg_ptr = (lte_rrc_osys_UL_CCCH_Message *)
  lte_rrc_osys_asn1_decode_pdu(access_req_ptr->access_info.msg_ptr, 
                               access_req_ptr->access_info.msg_length,
                               UL_CCCH_Message, 
                               &decode_status);
  TF_ASSERT(ul_ccch_msg_ptr !=NULL);
  TF_ASSERT(ul_ccch_msg_ptr->message.t == 
            T_lte_rrc_osys_UL_CCCH_MessageType_c1);


//  #if 0
  conn_req_msg_ptr = 
    ul_ccch_msg_ptr->message.u.c1->u.rrcConnectionRequest; 
  /*Validate the ConnReq message*/
  TF_ASSERT(conn_req_msg_ptr != NULL);
  TF_ASSERT(conn_req_msg_ptr->criticalExtensions.t == 
            T_lte_rrc_osys_RRCConnectionRequest_criticalExtensions_rrcConnectionRequest_r8);
  TF_ASSERT(conn_req_msg_ptr->criticalExtensions.u.rrcConnectionRequest_r8->establishmentCause
             == (lte_rrc_osys_EstablishmentCause)est_cause);

  ue_identity_ptr = 
    &(conn_req_msg_ptr->criticalExtensions.u.rrcConnectionRequest_r8->ue_Identity);

  /* !@todo Validate UE identity*/

  /*Send the Connection Setup message*/

  /*Verify that the state of the CEP SM is ACCESS*/
  TF_MSG("Before sending the ConnReject msg: Verifying that the CEP SM is in ACCESS state");

  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__ACCESS);
  i_ptr = (lte_rrc_cep_s *)stm_get_user_data(LTE_RRC_CEP_SM);

  TF_ASSERT(NULL != i_ptr);
  TF_ASSERT(NULL != i_ptr->dd_ptr);


  TF_ASSERT(TRUE == i_ptr->dd_ptr->rach_in_progress);

 
  // On receiving ReselStarted indi, CEP should transition to inactive  state
  TF_MSG("Sending the ReselStartedIndi");
  lte_rrc_init_default_hdr(&resel_started_indi.msg_hdr, 
                           LTE_RRC_CELL_RESEL_STARTED_INDI);
  lte_rrc_utf_send_msg((byte*)&resel_started_indi, sizeof(resel_started_indi));

  lte_rrc_utf_wait_for_done();

  /*Expect a AccessAbortReq from CEP*/
  TF_MSG("Checking for the MAC Access Abort");
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_mac_access_abort_req_msg_s) == buf_len);
  access_abort_req_ptr = 
    (lte_mac_access_abort_req_msg_s *)(void*) buf_ptr;
  TF_ASSERT(access_abort_req_ptr !=NULL);
  TF_ASSERT((access_abort_req_ptr->hdr.id) ==  
            LTE_MAC_ACCESS_ABORT_REQ);

  /*Verify that the CEP SM transitions to Access */
  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__ACCESS);

  /* Send Access Abort Cnf  */
  TF_MSG("Sending AccessAbortCnf");
  lte_rrc_init_default_hdr(&access_abort_cnf.hdr, 
                           LTE_MAC_ACCESS_ABORT_CNF);
  lte_rrc_utf_send_msg((byte*)&access_abort_cnf, sizeof(access_abort_cnf));
  lte_rrc_utf_wait_for_done();

  /*Expect a MACCancelConnReq from CEP*/
  TF_MSG("Checking for MAC Cancel Conn Req");
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_mac_cancel_conn_req_msg_s) == buf_len);
  mac_cancel_conn_req_ptr = 
    (lte_mac_cancel_conn_req_msg_s *)(void*) buf_ptr;
  TF_ASSERT(mac_cancel_conn_req_ptr !=NULL);
  TF_ASSERT((mac_cancel_conn_req_ptr->hdr.id) ==  
          LTE_MAC_CANCEL_CONN_REQ);


  /*Expect a CPHYCancelConnReq from CEP*/
  TF_MSG("Checking for CPHY Cancel Conn Req");
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_cphy_cancel_conn_req_s) == buf_len);
  cphy_cancel_conn_req_ptr = 
    (lte_cphy_cancel_conn_req_s *)(void*) buf_ptr;
  TF_ASSERT(cphy_cancel_conn_req_ptr !=NULL);
  TF_ASSERT((cphy_cancel_conn_req_ptr->msgr_hdr.id) ==  
          LTE_CPHY_CANCEL_CONN_REQ);

 
  /*Expect a ConnnectionEstablishmentFailure from CEP*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_establishment_failure_indi_s) == buf_len);
  conn_est_failure_indi_ptr = 
    (lte_rrc_conn_establishment_failure_indi_s *)(void*) buf_ptr;
  TF_ASSERT(conn_est_failure_indi_ptr != NULL);
  TF_ASSERT((conn_est_failure_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_ESTABLISHMENT_FAILURE_INDI);

  /*Expect a  ProceedWithResel from CEP */
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_proceed_with_resel_indi_s) == buf_len);
  proceed_with_resel_indi_ptr = 
    (lte_rrc_proceed_with_resel_indi_s *)(void*) buf_ptr;
  TF_ASSERT(proceed_with_resel_indi_ptr !=NULL);
  TF_ASSERT((proceed_with_resel_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_PROCEED_WITH_RESEL_INDI);
  TF_ASSERT(TRUE == proceed_with_resel_indi_ptr->status);


  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__PENDING_RETRY);
  TF_MSG("After receiving MAC Access Abort Req: Verifying that the CEP SM is in PENDING RETRY state");


  /*Inject a ConnAbortReq*/ 
  TF_MSG("Sending the ConnAbortReq");
  lte_rrc_init_default_hdr(&conn_abort_req.msg_hdr, 
                           LTE_RRC_CONN_ABORT_REQ);
  lte_rrc_utf_send_msg((byte*)&conn_abort_req, sizeof(conn_abort_req));
  lte_rrc_utf_wait_for_done();

   /*Expect a ConnnectionEstablishmentFailure from CEP*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_establishment_failure_indi_s) == buf_len);
  conn_est_failure_indi_ptr = 
    (lte_rrc_conn_establishment_failure_indi_s *)(void*) buf_ptr;
  TF_ASSERT(conn_est_failure_indi_ptr != NULL);
  TF_ASSERT((conn_est_failure_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_ESTABLISHMENT_FAILURE_INDI);

  TF_MSG("Received ConnEstFailure from CEP");
  
  /*Expect ConnEstCnf from CEP */
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_est_cnf_s) == buf_len);
  conn_est_cnf_ptr = 
    (lte_rrc_conn_est_cnf_s *)(void*) buf_ptr;
  TF_ASSERT(conn_est_cnf_ptr !=NULL);
  TF_ASSERT((conn_est_cnf_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_EST_CNF);

  TF_MSG("Received ConnEstCnf from CEP");

  TF_ASSERT(LTE_RRC_CONN_EST_FAILURE_ABORTED == 
            conn_est_cnf_ptr->est_status);
  /*
  bit_mask = (1<< (uint8)LTE_RRC_EST_CAUSE_EMERGENCY |
              1<< (uint8)LTE_RRC_EST_CAUSE_MT_ACCESS |
              1<< (uint8)LTE_RRC_EST_CAUSE_MO_SIGNALING|
              1<< (uint8)LTE_RRC_EST_CAUSE_MO_DATA);
  */
  /*
  bit_mask = (1<< (uint8)LTE_RRC_EST_CAUSE_MT_ACCESS |
              1<< (uint8)LTE_RRC_EST_CAUSE_MO_SIGNALING|
              1<< (uint8)LTE_RRC_EST_CAUSE_MO_DATA);
   */

  TF_ASSERT(conn_est_cnf_ptr->barring_info_is_present == FALSE);


  /*Expect a LTE_RRC_DLM_PROCESSED_INDI from CEP*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_dlm_processed_indi_s) == buf_len);
  dlm_processed_indi_ptr = 
    (lte_rrc_dlm_processed_indi_s  *)(void*) buf_ptr;
  TF_ASSERT(dlm_processed_indi_ptr != NULL);
  TF_ASSERT((dlm_processed_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_DLM_PROCESSED_INDI);

    /*Make sure that the CEP SM is in Inactive state*/
  TF_MSG("Checking that the CEP SM is in INACTIVE state");
  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__INACTIVE);
 
  /*Free the Connection Req (encoded) message*/
  lte_rrc_osys_asn1_free_pdu(ul_ccch_msg_ptr, lte_rrc_osys_UL_CCCH_Message_PDU); 

  lte_rrc_utf_reset();
}

void LteRrcCEPReselConnAbortTest1::Teardown()
{
  lte_rrc_test_cep_teardown();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCEPReselConnAbortTestwSIBUpdate);

void LteRrcCEPReselConnAbortTestwSIBUpdate::Setup()
{
  int ext_count = 0;
  lte_rrc_cep_test.ext_umid_list[0] = LTE_MAC_ACCESS_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[1] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[2] = LTE_CPHY_CANCEL_CONN_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[3] = LTE_MAC_CANCEL_CONN_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[4] = LTE_MAC_ACCESS_ABORT_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;

  
  lte_rrc_utf_setup(LTE_RRC_CEP_SM, 
                    &(lte_rrc_cep_test.ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_cep_init();
}

void LteRrcCEPReselConnAbortTestwSIBUpdate::Test()
{
  lte_rrc_conn_est_cnf_s        *conn_est_cnf_ptr = NULL;
  lte_rrc_conn_establishment_failure_indi_s *conn_est_failure_indi_ptr = NULL;
  lte_rrc_conn_establishment_started_indi_s *conn_est_started_indi_ptr = NULL;
  lte_rrc_cep_test_conn_req_s my_msg;
  lte_mac_access_req_msg_s *access_req_ptr= NULL;
  lte_rrc_proceed_with_resel_indi_s *proceed_with_resel_indi_ptr = NULL;
  lte_rrc_osys_RRCConnectionRequest *conn_req_msg_ptr = NULL;
  lte_rrc_osys_UL_CCCH_Message *ul_ccch_msg_ptr = NULL;
  lte_rrc_osys_InitialUE_Identity *ue_identity_ptr = NULL;
  /*Set the Est cause to MO_DATA*/
  lte_rrc_est_cause_e est_cause = LTE_RRC_EST_CAUSE_MO_DATA;
  lte_rrc_osys_SystemInformationBlockType2 *sib2 = NULL;
  lte_rrc_cell_resel_started_indi_s resel_started_indi ;
  lte_rrc_conn_abort_req_s conn_abort_req;
  lte_rrc_cep_s *i_ptr = NULL;
  lte_mac_access_abort_req_msg_s *access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s access_abort_cnf;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr = NULL;

  lte_cphy_cancel_conn_req_s * cphy_cancel_conn_req_ptr = NULL;
  lte_mac_cancel_conn_req_msg_s *mac_cancel_conn_req_ptr = NULL;
  lte_rrc_sib_updated_indi_s sib_update_indi;

  lte_rrc_trm_priority_change_indi_s *priority_change_indi_ptr = NULL;

  byte *buf_ptr;
  uint32 buf_len; 
  int decode_status = 0; 

  TF_MSG("Start of test");
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_MSG("Checking for the INACTIVE test");
  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__INACTIVE);

  // ----------------------------------------------------------------------
  
  /*Set the RRC controller in Camped state*/
  lte_rrc_test_cep_set_controller_state(LTE_RRC_CONTROLLER_SM__IDLE_CAMPED);  

  /*Set the Access class in the USIM to invalid*/
  lte_rrc_test_cep_set_controller_access_class(LTE_RRC_INVALID_ACCESS_CLASS);

  /*!@todo provide the proper SIB2 info; so that Access Barring check passes*/
  /*Access barring for Mobile Originated calls*/
  sib2 = (lte_rrc_osys_SystemInformationBlockType2 *)lte_rrc_test_cep_stub_get_cell_sib(LTE_RRC_SIB2_BITMASK,0);
  sib2->m.ac_BarringInfoPresent = 1;
  sib2->ac_BarringInfo.m.ac_BarringForMO_DataPresent = 0; 
  sib2->ac_BarringInfo.m.ac_BarringForMO_SignallingPresent = 0; 
  /*!@todo: Figure out a way to add the probabilitic thing later*/
  /*Setting the T300 timer value to 400 ms*/
  sib2->ue_TimersAndConstants.t300 = lte_rrc_osys_UE_TimersAndConstants_t300_ms400;

  // On receiving a conn est req, we should stay in the same state
  TF_MSG("Sending the ConnEstReq");
  lte_rrc_init_default_hdr(&conn_est_req.msg_hdr, LTE_RRC_CONN_EST_REQ);
  conn_est_req.est_cause = est_cause;
  lte_rrc_utf_send_msg((byte*)&conn_est_req, sizeof(conn_est_req));

  lte_rrc_utf_wait_for_done();

    /*Make sure that the CEP SM is in Access state*/
  TF_MSG("Checking that the CEP SM is in ACCESS state");
  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__ACCESS);

  /*Expect a Connection Establishment Started Ind from CEP*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_establishment_started_indi_s) == buf_len);
  conn_est_started_indi_ptr = 
    (lte_rrc_conn_establishment_started_indi_s *)(void*) buf_ptr;
  TF_ASSERT(conn_est_started_indi_ptr !=NULL)

  TF_ASSERT((conn_est_started_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_ESTABLISHMENT_STARTED_INDI);

  /*Expect a TrmPriorityChangeInd*/
  TF_MSG("Checking for LTE_RRC_TRM_PRIORITY_CHANGE_INDI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_trm_priority_change_indi_s) == buf_len);
  priority_change_indi_ptr = (lte_rrc_trm_priority_change_indi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_TRM_PRIORITY_CHANGE_INDI == priority_change_indi_ptr->msg_hdr.id);

  /*Expect a Access Req from CEP*/
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_mac_access_req_msg_s) == buf_len);
  access_req_ptr = (lte_mac_access_req_msg_s *)(void*) buf_ptr;
  TF_ASSERT(access_req_ptr !=NULL);

  TF_ASSERT((access_req_ptr->hdr.id) ==  LTE_MAC_ACCESS_REQ);

  /*We have indeed received an Access Request fron CEP*/
  /*Validate the Contents*/
  TF_ASSERT(access_req_ptr->access_info.access_reason == 
            LTE_MAC_ACCESS_REASON_CONNECTION_REQ);
  TF_ASSERT(access_req_ptr->access_info.raid == 
            LTE_INVALID_RA_ID);
  /*!@todo Validate the UEID later*/
  TF_ASSERT(access_req_ptr->access_info.msg_ptr);

  lte_rrc_test_cep_decode_conn_req(access_req_ptr->access_info.msg_ptr,
                                   (void *)(&my_msg),
                                   access_req_ptr->access_info.msg_length);

  TF_ASSERT( my_msg.msg_c1_choice == 1);
  /*T_lte_rrc_osys_UL_CCCH_MessageType_c1  = 1*/

  TF_ASSERT( my_msg.msg_choice == 2);
  /*lte_rrc_rrcConnectionRequest_chosen = 2*/

  TF_ASSERT( my_msg.conn_req_r8_choice == 1);
  /*T_lte_rrc_osys_RRCConnectionRequest_criticalExtensions_rrcConnectionRequest_r8 = 1*/

  TF_ASSERT( my_msg.est_cause == est_cause);
                                 
  /*Decode the ConnReq message*/
  ul_ccch_msg_ptr = (lte_rrc_osys_UL_CCCH_Message *)
  lte_rrc_osys_asn1_decode_pdu(access_req_ptr->access_info.msg_ptr, 
                               access_req_ptr->access_info.msg_length,
                               UL_CCCH_Message, 
                               &decode_status);
  TF_ASSERT(ul_ccch_msg_ptr !=NULL);
  TF_ASSERT(ul_ccch_msg_ptr->message.t == 
            T_lte_rrc_osys_UL_CCCH_MessageType_c1);

  conn_req_msg_ptr = 
    ul_ccch_msg_ptr->message.u.c1->u.rrcConnectionRequest; 
  /*Validate the ConnReq message*/
  TF_ASSERT(conn_req_msg_ptr != NULL);
  TF_ASSERT(conn_req_msg_ptr->criticalExtensions.t == 
            T_lte_rrc_osys_RRCConnectionRequest_criticalExtensions_rrcConnectionRequest_r8);
  TF_ASSERT(conn_req_msg_ptr->criticalExtensions.u.rrcConnectionRequest_r8->establishmentCause
             == (lte_rrc_osys_EstablishmentCause)est_cause);

  ue_identity_ptr = 
    &(conn_req_msg_ptr->criticalExtensions.u.rrcConnectionRequest_r8->ue_Identity);

  /* !@todo Validate UE identity*/
  /*Send the Connection Setup message*/
  /*Verify that the state of the CEP SM is ACCESS*/
  TF_MSG("Before sending the ConnReject msg: Verifying that the CEP SM is in ACCESS state");

  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__ACCESS);
  i_ptr = (lte_rrc_cep_s *)stm_get_user_data(LTE_RRC_CEP_SM);

  TF_ASSERT(NULL != i_ptr);
  TF_ASSERT(NULL != i_ptr->dd_ptr);

  TF_ASSERT(TRUE == i_ptr->dd_ptr->rach_in_progress);

  /*Send SIB Updated indication*/
  TF_MSG("Sending the SIB Update");
  lte_rrc_init_default_hdr(&sib_update_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  sib_update_indi.sibs_changed_bitmask = LTE_RRC_SIB2_BITMASK; 

  lte_rrc_utf_send_msg((byte*)&sib_update_indi, sizeof(sib_update_indi));

  lte_rrc_utf_wait_for_done();
 
  // On receiving ReselStarted indi, CEP should transition to inactive  state
  TF_MSG("Sending the ReselStartedIndi");
  lte_rrc_init_default_hdr(&resel_started_indi.msg_hdr, 
                           LTE_RRC_CELL_RESEL_STARTED_INDI);
  lte_rrc_utf_send_msg((byte*)&resel_started_indi, sizeof(resel_started_indi));

  lte_rrc_utf_wait_for_done();

  /*Expect a AccessAbortReq from CEP*/
  TF_MSG("Checking for the MAC Access Abort");
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_mac_access_abort_req_msg_s) == buf_len);
  access_abort_req_ptr = 
    (lte_mac_access_abort_req_msg_s *)(void*) buf_ptr;
  TF_ASSERT(access_abort_req_ptr !=NULL);
  TF_ASSERT((access_abort_req_ptr->hdr.id) ==  
            LTE_MAC_ACCESS_ABORT_REQ);

  /*Verify that the CEP SM transitions to Access */
  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__ACCESS);

  /* Send Access Abort Cnf  */
  TF_MSG("Sending AccessAbortCnf");
  lte_rrc_init_default_hdr(&access_abort_cnf.hdr, 
                           LTE_MAC_ACCESS_ABORT_CNF);
  lte_rrc_utf_send_msg((byte*)&access_abort_cnf, sizeof(access_abort_cnf));
  lte_rrc_utf_wait_for_done();

  /*Expect a MACCancelConnReq from CEP*/
  TF_MSG("Checking for MAC Cancel Conn Req");
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_mac_cancel_conn_req_msg_s) == buf_len);
  mac_cancel_conn_req_ptr = 
    (lte_mac_cancel_conn_req_msg_s *)(void*) buf_ptr;
  TF_ASSERT(mac_cancel_conn_req_ptr !=NULL);
  TF_ASSERT((mac_cancel_conn_req_ptr->hdr.id) ==  
          LTE_MAC_CANCEL_CONN_REQ);

  /*Expect a CPHYCancelConnReq from CEP*/
  TF_MSG("Checking for CPHY Cancel Conn Req");
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_cphy_cancel_conn_req_s) == buf_len);
  cphy_cancel_conn_req_ptr = 
    (lte_cphy_cancel_conn_req_s *)(void*) buf_ptr;
  TF_ASSERT(cphy_cancel_conn_req_ptr !=NULL);
  TF_ASSERT((cphy_cancel_conn_req_ptr->msgr_hdr.id) ==  
          LTE_CPHY_CANCEL_CONN_REQ);
 
  /*Expect a ConnnectionEstablishmentFailure from CEP*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_establishment_failure_indi_s) == buf_len);
  conn_est_failure_indi_ptr = 
    (lte_rrc_conn_establishment_failure_indi_s *)(void*) buf_ptr;
  TF_ASSERT(conn_est_failure_indi_ptr != NULL);
  TF_ASSERT((conn_est_failure_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_ESTABLISHMENT_FAILURE_INDI);

  /*Expect a  ProceedWithResel from CEP */
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_proceed_with_resel_indi_s) == buf_len);
  proceed_with_resel_indi_ptr = 
    (lte_rrc_proceed_with_resel_indi_s *)(void*) buf_ptr;
  TF_ASSERT(proceed_with_resel_indi_ptr !=NULL);
  TF_ASSERT((proceed_with_resel_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_PROCEED_WITH_RESEL_INDI);
  TF_ASSERT(TRUE == proceed_with_resel_indi_ptr->status);

  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__PENDING_RETRY);
  TF_MSG("After receiving MAC Access Abort Req: Verifying that the CEP SM is in PENDING RETRY state");

  /*Inject a ConnAbortReq*/ 
  TF_MSG("Sending the ConnAbortReq");
  lte_rrc_init_default_hdr(&conn_abort_req.msg_hdr, 
                           LTE_RRC_CONN_ABORT_REQ);
  lte_rrc_utf_send_msg((byte*)&conn_abort_req, sizeof(conn_abort_req));
  lte_rrc_utf_wait_for_done();

   /*Expect a ConnnectionEstablishmentFailure from CEP*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_establishment_failure_indi_s) == buf_len);
  conn_est_failure_indi_ptr = 
    (lte_rrc_conn_establishment_failure_indi_s *)(void*) buf_ptr;
  TF_ASSERT(conn_est_failure_indi_ptr != NULL);
  TF_ASSERT((conn_est_failure_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_ESTABLISHMENT_FAILURE_INDI);

  TF_MSG("Received ConnEstFailure from CEP");
  
  /*Expect ConnEstCnf from CEP */
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_est_cnf_s) == buf_len);
  conn_est_cnf_ptr = 
    (lte_rrc_conn_est_cnf_s *)(void*) buf_ptr;
  TF_ASSERT(conn_est_cnf_ptr !=NULL);
  TF_ASSERT((conn_est_cnf_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_EST_CNF);

  TF_MSG("Received ConnEstCnf from CEP");

  TF_ASSERT(LTE_RRC_CONN_EST_FAILURE_ABORTED == 
            conn_est_cnf_ptr->est_status);
  TF_ASSERT(conn_est_cnf_ptr->barring_info_is_present == FALSE);

  /*Expect a LTE_RRC_DLM_PROCESSED_INDI from CEP*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_dlm_processed_indi_s) == buf_len);
  dlm_processed_indi_ptr = 
    (lte_rrc_dlm_processed_indi_s  *)(void*) buf_ptr;
  TF_ASSERT(dlm_processed_indi_ptr != NULL);
  TF_ASSERT((dlm_processed_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_DLM_PROCESSED_INDI);

    /*Make sure that the CEP SM is in Inactive state*/
  TF_MSG("Checking that the CEP SM is in INACTIVE state");
  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__INACTIVE);
 
  /*Free the Connection Req (encoded) message*/
  lte_rrc_osys_asn1_free_pdu(ul_ccch_msg_ptr, lte_rrc_osys_UL_CCCH_Message_PDU); 

  lte_rrc_utf_reset();
}

void LteRrcCEPReselConnAbortTestwSIBUpdate::Teardown()
{
  lte_rrc_test_cep_teardown();
  lte_rrc_utf_teardown();
}

