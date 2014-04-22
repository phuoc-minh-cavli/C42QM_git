/*!
  @file lte_rrc_cep_test_conn_setup.cpp


  @brief
  Unit test code for CEP Connnection Setup .

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/conn_establishment/lte_rrc_cep_test_conn_setup_sib2_rx.cpp#1 $

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

#include <dsm_item.h>
#include <dsm_init.h>

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

static lte_rrc_conn_est_req_s conn_est_req;
static lte_rrc_mh_dlm_s conn_setup_dlm;
static lte_rrc_cfg_cnfi_s cfg_cnf;
static lte_rrc_sib_updated_indi_s sib_update_indi;

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

TF_DEFINE_TEST_CASE(NO_RUN, LteRrcCEPSibRxConnSetupTest1);

void LteRrcCEPSibRxConnSetupTest1::Setup()
{
  int ext_count = 0;
  //TF_STUB(lte_rrc_controller_get_state) = lte_rrc_test_cep_stub_controller_get_state;
  //lte_rrc_test_cep_init();
  //lte_rrc_cep_test.ext_umid_list[0] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_list[0] = LTE_MAC_ACCESS_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[1] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[2] = LTE_MAC_ACCESS_ABORT_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;

  lte_rrc_cep_test.ext_umid_list[3] = LTE_CPHY_CANCEL_CONN_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;

  lte_rrc_cep_test.ext_umid_list[4] = LTE_MAC_CANCEL_CONN_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;

  
  lte_rrc_utf_setup(LTE_RRC_CEP_SM, 
                    &(lte_rrc_cep_test.ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_cep_init();
}

void LteRrcCEPSibRxConnSetupTest1::Test()
{
  lte_rrc_conn_est_cnf_s        *conn_est_cnf_ptr = NULL;
  //lte_rrc_conn_establishment_failure_indi_s *conn_est_failure_indi_ptr = NULL;
  lte_rrc_conn_establishment_started_indi_s *conn_est_started_indi_ptr = NULL;
  lte_rrc_cep_test_conn_req_s my_msg;
  lte_mac_access_req_msg_s *access_req_ptr= NULL;
  lte_mac_access_abort_req_msg_s *access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s access_abort_cnf;

  lte_rrc_cfg_reqi_s *cfg_req_ptr = NULL;
  lte_rrc_proceed_with_resel_indi_s *proceed_with_resel_indi_ptr = NULL;
  lte_rrc_send_ul_msg_reqi_s *ulm_ptr;
  lte_rrc_connected_indi_s *connected_indi_ptr = NULL;
  //#if 0
  lte_rrc_osys_RRCConnectionRequest *conn_req_msg_ptr = NULL;
  lte_rrc_osys_UL_CCCH_Message *ul_ccch_msg_ptr = NULL;
  lte_rrc_osys_InitialUE_Identity *ue_identity_ptr = NULL;
  //#endif
  /*Set the Est cause to MO_DATA*/
  lte_rrc_est_cause_e est_cause = LTE_RRC_EST_CAUSE_MO_DATA;
  lte_rrc_osys_SystemInformationBlockType2 *sib2 = NULL;

  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_RRCConnectionSetupComplete *conn_setup_complete_ptr = NULL;
  lte_rrc_osys_RRCConnectionSetupComplete_r8_IEs *r8_ie_ptr = NULL ;
  lte_rrc_cep_test_type_s *test_state_ptr = NULL;
  int i = 0;
  const lte_rrc_ue_id_list_s *ue_id_list_ptr ;
  const lte_rrc_s_tmsi_s *s_tmsi_ptr ;

  lte_mac_cancel_conn_req_msg_s *mac_cancel_conn_req_ptr = NULL;
  lte_cphy_cancel_conn_req_s *cphy_cancel_conn_req_ptr = NULL;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr = NULL;
 

  byte *buf_ptr;
  uint32 buf_len; int decode_status = 0; 

   /* DSM pointer */
  dsm_item_type *dsm_ptr=NULL;
  uint16 dsm_pkt_len;
  uint8 nas_buf[LTE_RRC_MAX_NAS_MSG_CONTAINER_SIZE];
  /* Message attachment pointer */
  msgr_attach_struct_type *att_ptr = NULL;

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
  /*!@todo: Figure out a way to add the probabilitic thing later*/
  //sib2->accessBarringInformation.bit_mask = 0; 
  sib2->ac_BarringInfo.m.ac_BarringForMO_DataPresent = 0; sib2->ac_BarringInfo.m.ac_BarringForMO_SignallingPresent = 0; 
  
  /*Setting the T300 timer value to 400 ms*/
  sib2->ue_TimersAndConstants.t300 = lte_rrc_osys_UE_TimersAndConstants_t300_ms400;


  // On receiving a conn est req, we should stay in the same state
  TF_MSG("Sending the ConnEstReq");
  lte_rrc_init_default_hdr(&conn_est_req.msg_hdr, LTE_RRC_CONN_EST_REQ);
  conn_est_req.est_cause = est_cause;
  /*No registered MME*/
  conn_est_req.registered_mme_is_present= FALSE;

  /*Include S-TMSI*/
  conn_est_req.include_s_tmsi = TRUE;

  /*Fill up the NAS container*/
  //conn_est_req.nas_msg_container.msg_size = LTE_RRC_MAX_NAS_MSG_CONTAINER_SIZE;
  for(i=0; i< LTE_RRC_MAX_NAS_MSG_CONTAINER_SIZE; i++)
  {
    //conn_est_req.nas_msg_container.msg_payload[i] = i;
    nas_buf[i] = i;


  }
   dsm_pkt_len = dsm_pushdown(&dsm_ptr, 
							 nas_buf, 
							 LTE_RRC_MAX_NAS_MSG_CONTAINER_SIZE,
							 DSM_DS_SMALL_ITEM_POOL);

  TF_ASSERT(LTE_RRC_MAX_NAS_MSG_CONTAINER_SIZE == dsm_pkt_len);

  /* Set DSM attach */
  conn_est_req.msg_hdr.num_attach = 1;
  att_ptr = msgr_get_attach(&conn_est_req.msg_hdr, 0);
  TF_ASSERT(att_ptr != NULL);
  msgr_set_dsm_attach(att_ptr, dsm_ptr);

  

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
  if(TRUE == lte_rrc_test_cep_stub_controller_ue_id_list_is_valid())
  {
    ue_id_list_ptr = lte_rrc_test_cep_stub_controller_get_ue_id_list();
    TF_ASSERT(2 == ue_id_list_ptr->num_ue_identities);
    TF_ASSERT(LTE_RRC_UE_ID_STMSI == ue_id_list_ptr->ue_id[1].type);
    s_tmsi_ptr = &(ue_id_list_ptr->ue_id[1].value.s_tmsi);
   

  }
  TF_ASSERT(ue_identity_ptr->t == 
             T_lte_rrc_osys_InitialUE_Identity_s_TMSI);
  TF_ASSERT(0x12 == ue_identity_ptr->u.s_TMSI->mmec.data[0]);
  TF_ASSERT(8 == ue_identity_ptr->u.s_TMSI->mmec.numbits);

  TF_ASSERT(32 == ue_identity_ptr->u.s_TMSI->m_TMSI.numbits);
  TF_ASSERT(0x0a == ue_identity_ptr->u.s_TMSI->m_TMSI.data[0]);
  TF_ASSERT(0x0b == ue_identity_ptr->u.s_TMSI->m_TMSI.data[1]);
  TF_ASSERT(0x0c == ue_identity_ptr->u.s_TMSI->m_TMSI.data[2]);
  TF_ASSERT(0x0d == ue_identity_ptr->u.s_TMSI->m_TMSI.data[3]);


  
  /*Verify that the state of the CEP SM is ACCESS*/
  TF_MSG("Before sending the ConnSetup msg: Verifying that the CEP SM is in ACCESS state");

  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__ACCESS);

  /*Send SIB Updated indication*/
  TF_MSG("Sending the SIB Update");
  lte_rrc_init_default_hdr(&sib_update_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  sib_update_indi.sibs_changed_bitmask = LTE_RRC_SIB2_BITMASK; 

  lte_rrc_utf_send_msg((byte*)&sib_update_indi, sizeof(sib_update_indi));

  lte_rrc_utf_wait_for_done();

  /*Expect MAC Access Abort*/
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_mac_access_abort_req_msg_s) == buf_len);
  access_abort_req_ptr = 
    (lte_mac_access_abort_req_msg_s *)(void*) buf_ptr;

  TF_ASSERT(access_abort_req_ptr !=NULL);

  TF_ASSERT(LTE_MAC_ACCESS_ABORT_REQ == 
            access_abort_req_ptr->hdr.id);

  TF_MSG("Received MAC Access Abort");

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

  /*Expect a  ProceedWithResel from CEP */
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_proceed_with_resel_indi_s) == buf_len);
  proceed_with_resel_indi_ptr = 
    (lte_rrc_proceed_with_resel_indi_s *)(void*) buf_ptr;
  TF_ASSERT(proceed_with_resel_indi_ptr !=NULL);
  TF_ASSERT((proceed_with_resel_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_PROCEED_WITH_RESEL_INDI);
  TF_ASSERT(TRUE == proceed_with_resel_indi_ptr->status);
 


  /*Make sure that the CEP SM is in Pending Retry state*/
  TF_MSG("Checking that the CEP SM is in PENDING_RETRY state");
  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__PENDING_RETRY);
                                             
  /*Send the Connection Setup message*/

  lte_rrc_test_cep_build_connection_setup_dlm(&conn_setup_dlm, &lte_rrc_cep_test);
  lte_rrc_utf_send_msg((byte*)&conn_setup_dlm, sizeof(conn_setup_dlm));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__PENDING_RETRY);

  /*Expect a LTE_RRC_DLM_PROCESSED_INDI from CEP*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_dlm_processed_indi_s) == buf_len);
  dlm_processed_indi_ptr = 
    (lte_rrc_dlm_processed_indi_s  *)(void*) buf_ptr;
  TF_ASSERT(dlm_processed_indi_ptr != NULL);
  TF_ASSERT((dlm_processed_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_DLM_PROCESSED_INDI); 

  /*Free the Connection Req (unencoded) message*/
  lte_rrc_osys_asn1_free_pdu(ul_ccch_msg_ptr, lte_rrc_osys_UL_CCCH_Message_PDU); 

  lte_rrc_utf_reset(); 
}

void LteRrcCEPSibRxConnSetupTest1::Teardown()
{
  lte_rrc_test_cep_teardown();
  lte_rrc_utf_teardown();
}
