/*!
  @file lte_rrc_cep_test_pending_not_camped.cpp


  @brief
  Unit test code for testing handling of NotCamped indication 
  by CEP in Access state .

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/conn_establishment/lte_rrc_cep_test_pending_not_camped.cpp#1 $

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
#include <qtf_efs.h>


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
//static lte_rrc_mh_dlm_s conn_setup_dlm;
//static lte_rrc_cfg_cnfi_s cfg_cnf;

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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCEPPendingNotCampedTest1);

void LteRrcCEPPendingNotCampedTest1::Setup()
{
  int ext_count = 0;
  int result;
  uint8 t300_rf_unavail_thresh = 20;

  //qtf_efs_initialize("LteRrcCEPPendingNotCampedTest1");
  //result = efs_put(LTE_RRC_CEP_TRY_COUNT_EFS_FILENAME,&num_cep_tries,
  //result = efs_put("/nvm/alpha/modem/lte/rrc/cep/try_count",
  result = efs_put("/nv/item_files/modem/lte/rrc/cep/t300_tmr_rf_unavail",
                   &t300_rf_unavail_thresh,
                   1, O_RDWR|O_AUTODIR|O_TRUNC,
                   0777);
  TF_ASSERT(0 == result);	
  //TF_STUB(lte_rrc_controller_get_state) = lte_rrc_test_cep_stub_controller_get_state;
  //lte_rrc_test_cep_init();
  //lte_rrc_cep_test.ext_umid_list[0] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_list[0] = LTE_MAC_ACCESS_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[1] = LTE_MAC_ACCESS_ABORT_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[2] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[3] = LTE_MAC_CANCEL_CONN_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[4] = LTE_CPHY_CANCEL_CONN_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;

  
  lte_rrc_utf_setup(LTE_RRC_CEP_SM, 
                    &(lte_rrc_cep_test.ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_cep_init();
}

void LteRrcCEPPendingNotCampedTest1::Test()
{
  lte_rrc_conn_est_cnf_s        *conn_est_cnf_ptr = NULL;
  lte_rrc_conn_establishment_failure_indi_s *conn_est_failure_indi_ptr = NULL;
  lte_rrc_conn_establishment_started_indi_s *conn_est_started_indi_ptr = NULL;
  lte_rrc_cep_test_conn_req_s my_msg;
  lte_mac_access_req_msg_s *access_req_ptr= NULL;
	lte_cphy_rf_unavailable_ind_s unavailable_req;	
  //lte_rrc_cfg_reqi_s *cfg_req_ptr = NULL;
  //lte_rrc_stop_cell_resel_indi_s *stop_cell_resel_indi_ptr = NULL;
  //lte_rrc_connected_indi_s *connected_indi_ptr = NULL;
  //lte_rrc_cep_s *i_ptr = NULL;

  //lte_rrc_tmri_s *tmri_ptr  = NULL;

  lte_mac_access_abort_req_msg_s *access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s access_abort_cnf;
  //#if 0
  lte_rrc_osys_RRCConnectionRequest *conn_req_msg_ptr = NULL;
  lte_rrc_osys_UL_CCCH_Message *ul_ccch_msg_ptr = NULL;
  lte_rrc_osys_InitialUE_Identity *ue_identity_ptr = NULL;
  lte_rrc_trm_priority_change_indi_s *priority_change_indi_ptr = NULL;

  lte_rrc_not_camped_indi_s not_camped_indi ;

  //#endif
  /*Set the Est cause to MO_DATA*/
  lte_rrc_est_cause_e est_cause = LTE_RRC_EST_CAUSE_MO_DATA;
  lte_rrc_osys_SystemInformationBlockType2 *sib2 = NULL;
  //int num_max_retries = 0;
  

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

  /*!@todo: Figure out a way to add the probabilistic thing later*/
  sib2->ac_BarringInfo.m.ac_BarringForMO_DataPresent = 0;
  sib2->ac_BarringInfo.m.ac_BarringForMO_SignallingPresent = 0;
  /*
    sib2->accessBarringInformation.bit_mask = 
    lte_rrc_accessBarringForOriginatingCalls_present;
  sib2->accessBarringInformation.accessBarringForOriginatingCalls.accessProbabilityFactor = 
    lte_rrc_osys_AC_BarringConfig_ac_BarringFactor_p95;
  sib2->accessBarringInformation.accessBarringForOriginatingCalls.accessBarringTime = 
    lte_rrc_osys_AC_BarringConfig_ac_BarringTime_s4;
    */
  /*Setting the T300 timer value to 400 ms*/
  sib2->ue_TimersAndConstants.t300 = lte_rrc_osys_UE_TimersAndConstants_t300_ms300;

  // Send RF unavailable IND
  lte_rrc_init_default_hdr((msgr_hdr_s *)&unavailable_req.msgr_hdr,LTE_CPHY_RF_UNAVAILABLE_IND);
  TF_MSG("\nSending RF UNAVAILABLE IND\n");
  lte_rrc_utf_send_msg((byte*)&unavailable_req, sizeof(unavailable_req));
  lte_rrc_utf_wait_for_done();

  // On receiving a conn est req, we should move to pending retry
  TF_MSG("Sending the ConnEstReq");
  lte_rrc_init_default_hdr(&conn_est_req.msg_hdr, LTE_RRC_CONN_EST_REQ);
  conn_est_req.est_cause = est_cause;
  lte_rrc_utf_send_msg((byte*)&conn_est_req, sizeof(conn_est_req));

  lte_rrc_utf_wait_for_done();

	TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__PENDING_RETRY);

   // On receiving NotCamped indi, CEP should transition to inactive  state
  TF_MSG("Sending the NotCampedIndi");
  lte_rrc_init_default_hdr(&not_camped_indi.msg_hdr, 
                           LTE_RRC_NOT_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&not_camped_indi, sizeof(not_camped_indi));
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
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_est_cnf_s) == buf_len);
  conn_est_cnf_ptr = 
    (lte_rrc_conn_est_cnf_s *)(void*) buf_ptr;
  TF_ASSERT(conn_est_cnf_ptr !=NULL);
  TF_ASSERT((conn_est_cnf_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_EST_CNF);
  TF_ASSERT(LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE == conn_est_cnf_ptr->est_status); 


  /*Verify that the CEP SM transitions to Inactive*/
  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__INACTIVE);


  /*Free the Connection Req (encoded) message*/
  lte_rrc_osys_asn1_free_pdu(ul_ccch_msg_ptr, lte_rrc_osys_UL_CCCH_Message_PDU); 
  lte_rrc_utf_reset();
}

void LteRrcCEPPendingNotCampedTest1::Teardown()
{
  lte_rrc_test_cep_teardown();
  lte_rrc_utf_teardown();
}
