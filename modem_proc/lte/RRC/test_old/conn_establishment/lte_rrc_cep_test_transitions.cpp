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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/conn_establishment/lte_rrc_cep_test_transitions.cpp#1 $

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
}

#include "lte_rrc_cep_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_cep_test_transitions.cpp
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

============================================================est===========*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCEPTest1);

void LteRrcCEPTest1::Setup()
{
  int ext_count = 0;
  //TF_STUB(lte_rrc_controller_get_state) = lte_rrc_test_cep_stub_controller_get_state;
  //lte_rrc_test_cep_init();
  lte_rrc_cep_test.ext_umid_list[0] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  
  lte_rrc_utf_setup(LTE_RRC_CEP_SM, 
                    &(lte_rrc_cep_test.ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_cep_init();
}

void LteRrcCEPTest1::Test()
{
  lte_rrc_conn_est_req_s        conn_est_req;
  lte_rrc_conn_est_cnf_s        *conn_est_cnf_ptr;
  //lte_rrc_camped_indi_s         camped_indi;
  //lte_rrc_stopped_indi_s        stopped_indi;
  //lte_rrc_connected_indi_s      connected_indi;
  //lte_rrc_not_camped_indi_s     not_camped_indi;
  //lte_rrc_paging_dlm_s          paging_dlm;

  //lte_rrc_drx_info_req_s        drx_info_req;
  //lte_rrc_sim_update_req_s      sim_update_req;

  //uint8 int_queue_count;
  //uint8 ext_queue_count;
  byte *buf_ptr;
  uint32 buf_len; 

  TF_MSG("Start of test");
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_MSG("Checking for the INACTIVE test");
  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__INACTIVE);

  // ----------------------------------------------------------------------
  
  //lte_rrc_cep_test.rrc_controller_state = 
   // LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  lte_rrc_test_cep_set_controller_state(LTE_RRC_CONTROLLER_SM__IDLE_NOT_CAMPED);

  /*Set the Access class in the USIM to invalid*/
  lte_rrc_test_cep_set_controller_access_class(LTE_RRC_INVALID_ACCESS_CLASS);

  // On receiving a conn est req, we should stay in the same state
  TF_MSG("Sending the ConnEstReq");
  lte_rrc_init_default_hdr(&conn_est_req.msg_hdr, LTE_RRC_CONN_EST_REQ);
  lte_rrc_utf_send_msg((byte*)&conn_est_req, sizeof(conn_est_req));

  lte_rrc_utf_wait_for_done();

  
  TF_ASSERT(stm_get_state(LTE_RRC_CEP_SM) == LTE_RRC_CEP_SM__INACTIVE);
  TF_MSG("Waiting for ConnEstCnf");
 
   
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  
  TF_ASSERT(sizeof(lte_rrc_conn_est_cnf_s) == buf_len);
  conn_est_cnf_ptr = (lte_rrc_conn_est_cnf_s *)(void*) buf_ptr;
  TF_ASSERT((conn_est_cnf_ptr->msg_hdr.id) ==  LTE_RRC_CONN_EST_CNF);
  TF_ASSERT(LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED 
            == conn_est_cnf_ptr->est_status); 
  
  //We don't expect ConnectionEstablishmentFailure internal indication anymore..
  //'cos CEP doesn't send that indication before ConnEststarted indication has 
  // been sent
  /*
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_establishment_failure_indi_s) == buf_len);
  conn_est_failure_indi_ptr = 
    (lte_rrc_conn_establishment_failure_indi_s *)(void*) buf_ptr;
  TF_ASSERT((conn_est_failure_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_ESTABLISHMENT_FAILURE_INDI);
  */

/* 
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
*/

  lte_rrc_utf_reset();

 
  
  
}


void LteRrcCEPTest1::Teardown()
{
  lte_rrc_test_cep_teardown();
  lte_rrc_utf_teardown();
}
