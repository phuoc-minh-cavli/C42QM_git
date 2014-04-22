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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/conn_establishment/lte_rrc_cep_t305_expiry.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/13/08   dd     Initial Revision
           
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
  uint64 access_barring_time;
  uint8 probability;

} lte_rrc_cep_test_transitions_type_s;

static lte_rrc_conn_est_req_s conn_est_req;
//static lte_rrc_mh_dlm_s conn_setup_dlm;
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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCEPT305ExpiryTest1);

void LteRrcCEPT305ExpiryTest1::Setup()
{
  int ext_count = 0;
  //TF_STUB(lte_rrc_controller_get_state) = lte_rrc_test_cep_stub_controller_get_state;
  //lte_rrc_test_cep_init();
  //lte_rrc_cep_test.ext_umid_list[0] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_list[0] = LTE_MAC_ACCESS_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[1] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;

  
  lte_rrc_utf_setup(LTE_RRC_CEP_SM, 
                    &(lte_rrc_cep_test.ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_cep_init();
}

void LteRrcCEPT305ExpiryTest1::Test()
{
  lte_rrc_conn_est_cnf_s        *conn_est_cnf_ptr = NULL;
  lte_rrc_conn_establishment_failure_indi_s *conn_est_failure_indi_ptr = NULL;
  //lte_rrc_conn_establishment_started_indi_s *conn_est_started_indi_ptr = NULL;
  //lte_mac_access_req_msg_s *access_req_ptr= NULL;
  //lte_rrc_cfg_reqi_s *cfg_req_ptr = NULL;
  //lte_rrc_stop_cell_resel_indi_s *stop_cell_resel_indi_ptr = NULL;
  //lte_rrc_connected_indi_s *connected_indi_ptr = NULL;
  //#if 0
  //lte_rrc_osys_RRCConnectionRequest *conn_req_msg_ptr = NULL;
  lte_rrc_osys_UL_CCCH_Message *ul_ccch_msg_ptr = NULL;
  //lte_rrc_osys_InitialUE_Identity *ue_identity_ptr = NULL;
  //#endif
  /*Set the Est cause to MO_DATA*/
  lte_rrc_est_cause_e est_cause = LTE_RRC_EST_CAUSE_MO_SIGNALING;
  lte_rrc_osys_SystemInformationBlockType2 *sib2 = NULL;
  lte_rrc_tmri_s *tmri_ptr  = NULL;
  uint8 *value_ptr = NULL;


  byte *buf_ptr;
  uint32 buf_len; 

  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr = NULL;

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
  /*Access barring for Mobile Originated Signaling calls*/
  sib2 = (lte_rrc_osys_SystemInformationBlockType2 *)lte_rrc_test_cep_stub_get_cell_sib(LTE_RRC_SIB2_BITMASK,0);
  //sib2->bit_mask = lte_rrc_accessBarringInformation_present;
  sib2->m.ac_BarringInfoPresent = 1;
  //sib2->accessBarringInformation.bit_mask = 
  //  lte_rrc_accessBarringForSignalling_present;
  sib2->ac_BarringInfo.m.ac_BarringForMO_SignallingPresent = 1;

  /*Set the Access Barring probability to 0 so that access is always barred*/
  //sib2->accessBarringInformation.accessBarringForSignalling.accessProbabilityFactor = 
    //lte_rrc_osys_AC_BarringConfig_ac_BarringFactor_p00;
  sib2->ac_BarringInfo.ac_BarringForMO_Signalling.ac_BarringFactor = 
    lte_rrc_osys_AC_BarringConfig_ac_BarringFactor_p00;
  sib2->ac_BarringInfo.ac_BarringForMO_Signalling.ac_BarringTime = 
    lte_rrc_osys_AC_BarringConfig_ac_BarringTime_s4;
  /*Setting the T300 timer value to 400 ms*/
  sib2->ue_TimersAndConstants.t300 = lte_rrc_osys_UE_TimersAndConstants_t300_ms400;

  sib2->ac_BarringInfo.ac_BarringForMO_Signalling.ac_BarringForSpecialAC.numbits = 5;
  value_ptr = sib2->ac_BarringInfo.ac_BarringForMO_Signalling.ac_BarringForSpecialAC.data;
  TF_ASSERT(LTE_RRC_CEP_TEST_USIM_ACCESS_CLASS >= 11);
  TF_ASSERT(LTE_RRC_CEP_TEST_USIM_ACCESS_CLASS <= 15);
 
  /*Fill in the appropriate bit mask*/
  *(value_ptr) = (0x0 >>(LTE_RRC_CEP_TEST_USIM_ACCESS_CLASS -11))&(0xff);

  // On receiving a conn est req, we should stay in the same state
  TF_MSG("Sending the ConnEstReq");
  lte_rrc_init_default_hdr(&conn_est_req.msg_hdr, LTE_RRC_CONN_EST_REQ);
  conn_est_req.est_cause = est_cause;
  lte_rrc_utf_send_msg((byte*)&conn_est_req, sizeof(conn_est_req));

  lte_rrc_utf_wait_for_done();


   /*Expect a ConnnectionEstablishmentFailure from CEP*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_establishment_failure_indi_s) == buf_len);
  conn_est_failure_indi_ptr = 
    (lte_rrc_conn_establishment_failure_indi_s *)(void*) buf_ptr;
  TF_ASSERT(conn_est_failure_indi_ptr !=NULL);
  TF_ASSERT((conn_est_failure_indi_ptr->msg_hdr.id) ==  
          LTE_RRC_CONN_ESTABLISHMENT_FAILURE_INDI);
  /*Make sure that CEP state variables are reset appropriately*/

  /*Expect ConnEstCnf from CEP */
  TF_MSG("Expecting ConnEstCnf");
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_est_cnf_s) == buf_len);
  conn_est_cnf_ptr = 
    (lte_rrc_conn_est_cnf_s *)(void*) buf_ptr;
  TF_ASSERT(conn_est_cnf_ptr !=NULL);
  TF_ASSERT((conn_est_cnf_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_EST_CNF);
  TF_ASSERT(LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED == conn_est_cnf_ptr->est_status);
  TF_ASSERT(TRUE == conn_est_cnf_ptr->barring_info_is_present);
  TF_ASSERT((1<< (uint8)LTE_RRC_EST_CAUSE_MO_SIGNALING) == 
            conn_est_cnf_ptr->call_type_bitmask);
  /*Make sure that the Access Barring timers match the limts */
  TF_ASSERT(conn_est_cnf_ptr->barring_timer_value >=(0.7*4000));

  TF_ASSERT(conn_est_cnf_ptr->barring_timer_value <=(1.3*4000));


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


  /*Sleep for the Access Barring time as given by T303*/
  TF_SLEEP((int)(conn_est_cnf_ptr->barring_timer_value));
  /*Expect T305 tmri*/
  TF_MSG("Checking for T305 tmr expiry");
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  tmri_ptr = 
    (lte_rrc_tmri_s *)(void*) buf_ptr;
  TF_ASSERT(tmri_ptr !=NULL);
  TF_ASSERT((tmri_ptr->msg_hdr.id) ==  
          LTE_RRC_T305_TMRI);


    /*Make sure that the CEP SM is in Inactive state*/
  TF_MSG("Checking that the CEP SM is in INACTIVE state");
  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__INACTIVE);

#if 0
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
  ul_ccch_msg_ptr = 
    (lte_rrc_osys_UL_CCCH_Message *) LTE_RRC_OSYS_ASN1_DECODE_PDU(access_req_ptr->access_info.msg_ptr,
                                             lte_rrc_osys_UL_CCCH_Message_PDU,
                                             access_req_ptr->access_info.msg_length);
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
             == est_cause);

  ue_identity_ptr = 
    &(conn_req_msg_ptr->criticalExtensions.u.rrcConnectionRequest_r8->ue_Identity);

  /* !@todo Validate UE identity*/

  /*Send the Connection Setup message*/

  /*Verify that the state of the CEP SM is ACCESS*/
  TF_MSG("Before sending the ConnSetup msg: Verifying that the CEP SM is in ACCESS state");

  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__ACCESS);

  lte_rrc_test_cep_build_connection_setup_dlm(&conn_setup_dlm, &lte_rrc_cep_test);
  lte_rrc_utf_send_msg((byte*)&conn_setup_dlm, sizeof(conn_setup_dlm));


  lte_rrc_utf_wait_for_done();

  /*Expect a Config Req from CEP*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);
  cfg_req_ptr = 
    (lte_rrc_cfg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(cfg_req_ptr !=NULL);
  TF_ASSERT((cfg_req_ptr->msg_hdr.id) ==  
            LTE_RRC_CFG_REQI);
  /*Assign a transaction ID to the CFG req*/
  lte_rrc_cep_test.cfg_transaction_id = 3;
  TF_ASSERT(cfg_req_ptr->trans_id_ptr !=NULL);
  *(cfg_req_ptr->trans_id_ptr)= lte_rrc_cep_test.cfg_transaction_id;
  TF_ASSERT(cfg_req_ptr->cnfi_required == TRUE);
  TF_ASSERT(cfg_req_ptr->num_cfg == 1);
  TF_ASSERT(cfg_req_ptr->cfg_info[0].cfg_type == LTE_RRC_CFG_DED);

  /*Verify that the state of the CEP SM is CSRB1*/
  TF_MSG("Verifying that the CEP SM is in CSRB1 state");

  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__CONFIGURING_SRB1);

  /*Expect StopCellResel Ind*/

  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_stop_cell_resel_indi_s) == buf_len);
  stop_cell_resel_indi_ptr = 
    (lte_rrc_stop_cell_resel_indi_s *)(void*) buf_ptr;
  TF_ASSERT(stop_cell_resel_indi_ptr !=NULL);
  TF_ASSERT((stop_cell_resel_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_STOP_CELL_RESEL_INDI);

  TF_MSG("Before sending CfgCnf: Verifying that the CEP SM is in CSRB1 state");

  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__CONFIGURING_SRB1);

  /*Make sure that the Encode status for ConnSetupComplete is set to 0*/
  lte_rrc_utf_set_sync_ul_msg_params(0,0);
  TF_MSG("Sending the ConfigCnf");
  lte_rrc_init_default_hdr(&cfg_cnf.msg_hdr, LTE_RRC_CFG_CNFI);
  cfg_cnf.status = E_SUCCESS ; 
  cfg_cnf.proc= LTE_RRC_PROC_CEP;
  cfg_cnf.trans_id = lte_rrc_cep_test.cfg_transaction_id;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(lte_rrc_cfg_cnfi_s));

  lte_rrc_utf_wait_for_done();
  /*Expect the ConenctionSetupComplete*/

  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ulm_ptr = 
    (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(ulm_ptr !=NULL);
  TF_ASSERT((ulm_ptr->msg_hdr.id) ==  
            LTE_RRC_SEND_UL_MSG_REQI);
  /*Send on SRB1 */

  TF_ASSERT(ulm_ptr->rb_id = 1);
  *(ulm_ptr->encode_status) = 0; 
  //ulm_ptr->ul_msg_ptr

  /*Expect the Connected internal indication within RRC*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_connected_indi_s) == buf_len);
  connected_indi_ptr = 
    (lte_rrc_connected_indi_s *)(void*) buf_ptr;
  TF_ASSERT(connected_indi_ptr !=NULL);
  TF_ASSERT((connected_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_CONNECTED_INDI);


  /*Expect ConnEstCnf from CEP */
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_est_cnf_s) == buf_len);
  conn_est_cnf_ptr = 
    (lte_rrc_conn_est_cnf_s *)(void*) buf_ptr;
  TF_ASSERT(conn_est_cnf_ptr !=NULL);
  TF_ASSERT((conn_est_cnf_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_EST_CNF);
  TF_ASSERT(LTE_RRC_CONN_EST_SUCCESS == conn_est_cnf_ptr->est_status); 


  /*Verify that the CEP SM is in Connected state*/
  TF_MSG("After receiving ConnectionSetupComplete: Verifying that the CEP SM is in Connected state");

  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__CONNECTED);

  /*Free the Connection Req (encoded) message*/
  LTE_RRC_OSYS_ASN1_FREE_PDU( ul_ccch_msg_ptr, lte_rrc_osys_UL_CCCH_Message_PDU); 

  /*Free the ConnectionSetup (unencoded) message*/
  LTE_RRC_OSYS_ASN1_FREE_PDU(lte_rrc_osys_DL_CCCH_Message_PDU, 
                        lte_rrc_cep_test.conn_setup_dl_ccch_ptr); 
#endif

/* 
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
*/
//#endif
  lte_rrc_utf_reset();
}

void LteRrcCEPT305ExpiryTest1::Teardown()
{ 
  lte_rrc_test_cep_teardown();
  lte_rrc_utf_teardown();
}
