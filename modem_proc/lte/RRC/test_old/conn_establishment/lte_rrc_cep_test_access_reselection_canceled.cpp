/*!
  @file lte_rrc_cep_test_access_reselection.cpp


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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/conn_establishment/lte_rrc_cep_test_access_reselection_canceled.cpp#1 $

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

#define LTE_RRC_CEP_TEST_T300_VAL  400
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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCEPAccessReselectionTest2);
/*!@todo: Fix this test with the reseelction stuff*/
//TF_DEFINE_TEST_CASE(XYZ, LteRrcCEPAccessReselectionTest1);

void LteRrcCEPAccessReselectionTest2::Setup()
{
  int ext_count = 0;
  //TF_STUB(lte_rrc_controller_get_state) = lte_rrc_test_cep_stub_controller_get_state;
  //lte_rrc_test_cep_init();
  //lte_rrc_cep_test.ext_umid_list[0] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_list[0] = LTE_MAC_ACCESS_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[1] = LTE_MAC_ACCESS_ABORT_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[2] = LTE_MAC_CANCEL_CONN_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[3] = LTE_CPHY_CANCEL_CONN_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[4] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[5] = LTE_RRC_BARRING_UPDATE_IND;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;

  
  lte_rrc_utf_setup(LTE_RRC_CEP_SM, 
                    &(lte_rrc_cep_test.ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_cep_init();
}

void LteRrcCEPAccessReselectionTest2::Test()
{
  lte_rrc_conn_est_cnf_s        *conn_est_cnf_ptr = NULL;
  lte_rrc_conn_establishment_failure_indi_s *conn_est_failure_indi_ptr = NULL;
  lte_rrc_conn_establishment_started_indi_s *conn_est_started_indi_ptr = NULL;
  lte_rrc_cep_test_conn_req_s my_msg;
  lte_mac_access_req_msg_s *access_req_ptr= NULL;
  lte_mac_cancel_conn_req_msg_s *mac_cancel_conn_req_ptr = NULL;
  lte_cphy_cancel_conn_req_s *cphy_cancel_conn_req_ptr = NULL;
  lte_rrc_cfg_reqi_s *cfg_req_ptr = NULL;
  lte_rrc_proceed_with_resel_indi_s *proceed_with_resel_indi_ptr = NULL;
  lte_rrc_connected_indi_s *connected_indi_ptr = NULL;
  lte_rrc_cep_s *i_ptr = NULL;

  int decode_status = 0;

 


  //lte_rrc_tmri_s *tmri_ptr  = NULL;

  lte_mac_access_abort_req_msg_s *access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s access_abort_cnf;
  //#if 0
  lte_rrc_osys_RRCConnectionRequest *conn_req_msg_ptr = NULL;
  lte_rrc_osys_UL_CCCH_Message *ul_ccch_msg_ptr = NULL;
  lte_rrc_osys_InitialUE_Identity *ue_identity_ptr = NULL;

  lte_rrc_cell_resel_started_indi_s resel_started_indi ;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_cell_resel_canceled_indi_s resel_canceled_indi;
  lte_rrc_send_ul_msg_reqi_s *ulm_ptr = NULL;
  
  
  
  //#endif
  /*Set the Est cause to MO_DATA*/
  lte_rrc_est_cause_e est_cause = LTE_RRC_EST_CAUSE_MO_DATA;
  lte_rrc_osys_SystemInformationBlockType2 *sib2 = NULL;
  //int num_max_retries = 0;
  int i = 0;
  int reselect = 1;
   
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_RRCConnectionSetupComplete *conn_setup_complete_ptr = NULL;
  lte_rrc_osys_RRCConnectionSetupComplete_r8_IEs *r8_ie_ptr = NULL ;
  lte_rrc_trm_priority_change_indi_s *priority_change_indi_ptr = NULL;


  byte *buf_ptr;
  uint32 buf_len;
  lte_rrc_cep_test_type_s *test_state_ptr = NULL;

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
  //sib2->accessBarringInformation.bit_mask =  0;
  sib2->ac_BarringInfo.m.ac_BarringForMO_DataPresent =  0;
  sib2->ac_BarringInfo.m.ac_BarringForMO_SignallingPresent =  0;

  /*!@todo: Figure out a way to add the probabilitic thing later*/
 
  /*Setting the T300 timer value to 400 ms*/
  sib2->ue_TimersAndConstants.t300 = lte_rrc_osys_UE_TimersAndConstants_t300_ms400;

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

  
  // On receiving a conn est req, we should stay in the same state
  TF_MSG("Sending the ConnEstReq");
  lte_rrc_init_default_hdr(&conn_est_req.msg_hdr, LTE_RRC_CONN_EST_REQ);
  conn_est_req.est_cause = est_cause;
  /* Set DSM attach */
  conn_est_req.msg_hdr.num_attach = 1;
  att_ptr = msgr_get_attach(&conn_est_req.msg_hdr, 0);
  TF_ASSERT(att_ptr != NULL);
  msgr_set_dsm_attach(att_ptr, dsm_ptr);

  lte_rrc_utf_send_msg((byte*)&conn_est_req, sizeof(conn_est_req));

  lte_rrc_utf_wait_for_done();

  for(i =0; i<2; i++)
  {
  
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


    //   #if 0
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
    TF_MSG("After receiving ConnReq msg: Verifying that the CEP SM is in ACCESS state");

    TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__ACCESS);
    i_ptr = (lte_rrc_cep_s *)stm_get_user_data(LTE_RRC_CEP_SM);
    TF_ASSERT(NULL != i_ptr);
    TF_ASSERT(NULL != i_ptr->dd_ptr);

    TF_ASSERT(TRUE == i_ptr->dd_ptr->rach_in_progress);

    if (reselect == 1)
    {
  
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

      /* Verify that the CEP SM transitions to Access */
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
      TF_ASSERT(conn_est_failure_indi_ptr !=NULL);
      TF_ASSERT((conn_est_failure_indi_ptr->msg_hdr.id) ==  
                LTE_RRC_CONN_ESTABLISHMENT_FAILURE_INDI);

      /*Expect a ProceedWithResel indi from CEP*/
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

      /*Free the Connection Req (unencoded) message*/
      lte_rrc_osys_asn1_free_pdu( ul_ccch_msg_ptr, lte_rrc_osys_UL_CCCH_Message_PDU); 

      /*Make sure that Resel timer is running*/
      i_ptr = (lte_rrc_cep_s *)stm_get_user_data(LTE_RRC_CEP_SM);
      TF_ASSERT(FALSE == i_ptr->dd_ptr->rach_in_progress);
      TF_ASSERT(TRUE == i_ptr->dd_ptr->resel_timer_is_running);

      TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__PENDING_RETRY);
      TF_MSG("After receiving ProceedWithResel indication: Verified that the CEP SM is in PENDING RETRY state. RACH is not in progress");

      /*Set the RRC controller in Camped state*/
      lte_rrc_test_cep_set_controller_state(LTE_RRC_CONTROLLER_SM__IDLE_CAMPED);  
  
      /* Send Camped indication */
      TF_MSG("Sending Reselection CanceledIndi");
      lte_rrc_init_default_hdr(&resel_canceled_indi.msg_hdr, 
                               LTE_RRC_CELL_RESEL_CANCELED_INDI);
      camped_indi.reason = LTE_RRC_CAMPED_CELL_RESELECTION;
      lte_rrc_utf_send_msg((byte*)&resel_canceled_indi, sizeof(resel_canceled_indi));
      lte_rrc_utf_wait_for_done();
      reselect = 0;
    }/*reselect = 1*/
    else
    {
      /*reselect = 0*/
      
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
      TF_ASSERT(sizeof(lte_rrc_proceed_with_resel_indi_s) == buf_len);
      proceed_with_resel_indi_ptr = 
        (lte_rrc_proceed_with_resel_indi_s *)(void*) buf_ptr;
      TF_ASSERT(proceed_with_resel_indi_ptr !=NULL);
      TF_ASSERT((proceed_with_resel_indi_ptr->msg_hdr.id) ==  
                LTE_RRC_PROCEED_WITH_RESEL_INDI);
      TF_ASSERT(FALSE == proceed_with_resel_indi_ptr->status);

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

      TF_ASSERT(ulm_ptr->rb_id == 1);
      *(ulm_ptr->encode_status) = 0; 
      ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ulm_ptr->ul_msg_ptr;
      TF_ASSERT(ul_dcch_msg_ptr !=NULL);

      TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
                ul_dcch_msg_ptr->message.t);

      TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1_rrcConnectionSetupComplete ==
                ul_dcch_msg_ptr->message.u.c1->t);
      conn_setup_complete_ptr = (ul_dcch_msg_ptr->message).u.c1->u.rrcConnectionSetupComplete;

  
      test_state_ptr = lte_rrc_test_cep_get_state();
      TF_ASSERT(test_state_ptr !=NULL);
   
      TF_ASSERT(lte_rrc_cep_test.conn_setup_transaction_id == 
                conn_setup_complete_ptr->rrc_TransactionIdentifier);

      TF_ASSERT(T_lte_rrc_osys_RRCConnectionSetupComplete_criticalExtensions_c1_rrcConnectionSetupComplete_r8 == 
                conn_setup_complete_ptr->criticalExtensions.u.c1->t);
      r8_ie_ptr = conn_setup_complete_ptr->criticalExtensions.u.c1->u.
       rrcConnectionSetupComplete_r8;
      if(FALSE == conn_est_req.registered_mme_is_present)
      {
        TF_ASSERT(0 == r8_ie_ptr->m.nonCriticalExtensionPresent && 
                  0 == r8_ie_ptr->m.registeredMMEPresent);

      }
      else
      {
        TF_ASSERT(0!= r8_ie_ptr->m.registeredMMEPresent);
      }
      TF_ASSERT(((test_state_ptr->selected_plmn_index)+1) == 
                r8_ie_ptr->selectedPLMN_Identity);
      //TF_ASSERT(r8_ie_ptr->dedicatedInfoNAS.numocts == 
      //          conn_est_req.nas_msg_container.msg_size); 


      //if(conn_est_req.nas_msg_container.msg_size >0)
      if(r8_ie_ptr->dedicatedInfoNAS.numocts >0)
      {
		#if 0
        TF_ASSERT(0 == (memcmp(r8_ie_ptr->dedicatedInfoNAS.data, 
                               conn_est_req.nas_msg_container.msg_payload, 
                               conn_est_req.nas_msg_container.msg_size)));
		#endif
		TF_ASSERT(0 == (memcmp(r8_ie_ptr->dedicatedInfoNAS.data, 
							   nas_buf, 
							   r8_ie_ptr->dedicatedInfoNAS.numocts)));
      }

      /*Expect the Connected internal indication within RRC*/
      lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
      TF_ASSERT(sizeof(lte_rrc_connected_indi_s) == buf_len);
      connected_indi_ptr = 
        (lte_rrc_connected_indi_s *)(void*) buf_ptr;
      TF_ASSERT(connected_indi_ptr !=NULL);
      TF_ASSERT((connected_indi_ptr->msg_hdr.id) ==  
                LTE_RRC_CONNECTED_INDI);

lte_rrc_test_integration_verify_req_send_cnf(NULL, LTE_RRC_BARRING_UPDATE_IND, LTE_RRC_TEST_CELL_0, FALSE, NULL);

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
      lte_rrc_osys_asn1_free_pdu(ul_ccch_msg_ptr, lte_rrc_osys_UL_CCCH_Message_PDU); 
      /*Free ConnectionSetupComplete (unencoded) message*/
      lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);

    }
  }

  #if 0
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
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_est_cnf_s) == buf_len);
  conn_est_cnf_ptr = 
    (lte_rrc_conn_est_cnf_s *)(void*) buf_ptr;
  TF_ASSERT(conn_est_cnf_ptr !=NULL);
  TF_ASSERT((conn_est_cnf_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_EST_CNF);
  TF_ASSERT(LTE_RRC_CONN_EST_FAILURE_CELL_RESEL == conn_est_cnf_ptr->est_status); 


  /*Verify that the CEP SM transitions to Inactive*/
  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__INACTIVE);
  #endif


   lte_rrc_utf_reset();
}

void LteRrcCEPAccessReselectionTest2::Teardown()
{
  lte_rrc_test_cep_teardown();
  lte_rrc_utf_teardown();
}
