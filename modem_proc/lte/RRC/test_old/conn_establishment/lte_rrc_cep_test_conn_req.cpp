/*!
  @file lte_rrc_cep_test_transitions.cpp

  @brief
  Unit test code for CEP transitions .

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/conn_establishment/lte_rrc_cep_test_conn_req.cpp#1 $

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
#include "lte_rrc_sib.h"
}

#include "lte_rrc_cep_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>

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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCEPConnReqTest1);

void LteRrcCEPConnReqTest1::Setup()
{
  int ext_count = 0;
  //TF_STUB(lte_rrc_controller_get_state) = lte_rrc_test_cep_stub_controller_get_state;
  //lte_rrc_test_cep_init();
  //lte_rrc_cep_test.ext_umid_list[0] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_list[0] = LTE_MAC_ACCESS_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[1] = LTE_MAC_ACCESS_ABORT_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[2] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;

  lte_rrc_cep_test.ext_umid_list[3] = LTE_CPHY_CANCEL_CONN_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;

  lte_rrc_cep_test.ext_umid_list[4] = LTE_MAC_CANCEL_CONN_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  
  lte_rrc_cep_test.ext_umid_list[5] = LTE_CPHY_DEBUG_DEADLOCK_TMR_EXP_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;

  
  lte_rrc_utf_setup(LTE_RRC_CEP_SM, 
                    &(lte_rrc_cep_test.ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_cep_init();
}

void LteRrcCEPConnReqTest1::Test()
{
  lte_rrc_conn_est_req_s        conn_est_req;
  lte_rrc_conn_establishment_started_indi_s *conn_est_started_indi_ptr = NULL;
  lte_rrc_cep_test_conn_req_s my_msg;
  lte_mac_access_req_msg_s *access_req_ptr= NULL;
  lte_rrc_osys_RRCConnectionRequest *conn_req_msg_ptr = NULL;
  lte_rrc_osys_UL_CCCH_Message *ul_ccch_msg_ptr = NULL;
  lte_rrc_osys_InitialUE_Identity *ue_identity_ptr = NULL;
  lte_cphy_debug_deadlock_tmr_exp_req_s *cphy_debug_data_ptr;
  /*Set the Est cause to MO_DATA*/
  lte_rrc_est_cause_e est_cause = LTE_RRC_EST_CAUSE_MO_DATA;
  
  lte_rrc_osys_SystemInformationBlockType2 *sib2 = NULL;

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

   /*Access barring for Mobile Originated calls*/
  sib2 = (lte_rrc_osys_SystemInformationBlockType2 *)lte_rrc_test_cep_stub_get_cell_sib(LTE_RRC_SIB2_BITMASK,0);

  /*Access Barring info abesnt... So, allow the call*/
  sib2->m.ac_BarringInfoPresent = 0;
  sib2->m.mbsfn_SubframeConfigListPresent = 0;

  /*Set T300 to 400 ms*/
  sib2->ue_TimersAndConstants.t300 = lte_rrc_osys_UE_TimersAndConstants_t300_ms400;

  // On receiving a conn est req, we should stay in the same state
  TF_MSG("Sending the ConnEstReq");
  lte_rrc_init_default_hdr(&conn_est_req.msg_hdr, LTE_RRC_CONN_EST_REQ);
  conn_est_req.est_cause = est_cause;
  lte_rrc_utf_send_msg((byte*)&conn_est_req, sizeof(conn_est_req));

  lte_rrc_utf_wait_for_done();

  /*!@todo provide the proper SIB2 info; so that Access Barring check passes*/

  /*Make sure that the CEP SM is in Access state*/
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
  TF_ASSERT((conn_est_started_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_ESTABLISHMENT_STARTED_INDI);

  /*Expect a Access Req from CEP*/
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_mac_access_req_msg_s) == buf_len);
  access_req_ptr = (lte_mac_access_req_msg_s *)(void*) buf_ptr;
  TF_ASSERT((access_req_ptr->hdr.id) ==  LTE_MAC_ACCESS_REQ);

  /*We have indeed received an Access Request fron CEP*/

  /*Validate the Contents*/
  TF_ASSERT(access_req_ptr->access_info.access_reason == 
            LTE_MAC_ACCESS_REASON_CONNECTION_REQ);
  TF_ASSERT(access_req_ptr->access_info.raid == 
            LTE_INVALID_RA_ID);
  /*!@todo Validate the UEID later*/
  lte_rrc_test_cep_decode_conn_req(access_req_ptr->access_info.msg_ptr,
                                   (void *)(&my_msg),
                                   access_req_ptr->access_info.msg_length);

  TF_ASSERT( my_msg.msg_c1_choice == 1);
  /*lte_rrc_osys_UL_CCCH_MessageType_c1_chosen  = 1*/

  TF_ASSERT( my_msg.msg_choice == 2);
  /*lte_rrc_rrcConnectionRequest_chosen = 2*/

  TF_ASSERT( my_msg.conn_req_r8_choice == 1);
  /*lte_rrc_rrcConnectionRequest_r8_chosen = 1*/

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

  /*Set the RRC controller in Camped state*/
  lte_rrc_test_cep_set_controller_state(LTE_RRC_CONTROLLER_SM__CONNECTING);

  
  /*Expire the T300 timer*/
  TF_MSG("Expiring the timer");
  TF_SLEEP(10000);

#if 0
lte_rrc_utf_get_next_ext_msg(&buf_ptr, &buf_len); 
cphy_debug_data_ptr = 
   (lte_cphy_debug_deadlock_tmr_exp_req_s *)(void *)buf_ptr; 
TF_ASSERT(cphy_debug_data_ptr != NULL); 
TF_ASSERT((cphy_debug_data_ptr->msgr_hdr.id) ==  
             LTE_CPHY_DEBUG_DEADLOCK_TMR_EXP_REQ); 
#endif    



  /*Free the Unncoded ConnReq*/
  
  lte_rrc_osys_asn1_free_pdu(ul_ccch_msg_ptr, lte_rrc_osys_UL_CCCH_Message_PDU); 

  /*Free the encoded ConnReq*/
 // lte_rrc_asn1_free_buf(access_req_ptr->access_info.msg_ptr);
/* 
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
*/
//#endif
  lte_rrc_utf_reset();
}

void LteRrcCEPConnReqTest1::Teardown()
{
  lte_rrc_test_cep_teardown();
  lte_rrc_utf_teardown();
}
