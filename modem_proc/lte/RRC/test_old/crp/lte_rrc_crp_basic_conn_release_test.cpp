/*!
  @file lte_rrc_crp_basic_conn_release_test.cpp

  @brief
  Unit test code for taking CRP from Inactive -> Wait for Timer ->
  Wait for LLC confirm -> Inactive

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

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/08   sureshs Removed use of conn_rel_msg_ptr
09/15/08   sureshs Used TF_SLEEP instead of injecting TMRI manually
09/15/08   sureshs Refactored connection release message construction routine
09/15/08   sureshs Moved extern statements to header file
09/13/08   sureshs Replaced TF_SLEEP by injection of timer expired indication
09/12/08   sureshs Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include <lte_rrc_int_msg.h>
#include <lte_rrc_ext_msg.h>
#include <__lte_rrc_crp.h>
#include <__lte_rrc_controller.h>
#include <lte_rrc_utils.h>
#include "lte_rrc_crp_test_asn.h"
}

#include <TestFramework.h>
#include <lte_rrc_utf.h>
#include "lte_rrc_crp_test_common.h"

//external messages generated by code under test
static uint32 ext_umid_list[] = 
{ 
  LTE_RRC_CONN_REL_IND
};

//message structures
static lte_rrc_mh_dlm_s conn_rel_dlm;
static lte_rrc_cfg_cnfi_s config_cnf;

//transaction ID to be returned from config request
#define LTE_RRC_CRP_TEST_CFG_TRANS_ID 7

//Connection Release Timer value in ms
#define LTE_RRC_CRP_TEST_CONN_REL_TMR_VAL 60

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCrpBasicConnRelTest);

void LteRrcCrpBasicConnRelTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CRP_SM,ext_umid_list,1);
  lte_rrc_crp_test_stub_init();
}

void LteRrcCrpBasicConnRelTest::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  lte_rrc_conn_rel_started_indi_s *conn_rel_started_ind_ptr;
  lte_rrc_cfg_reqi_s *cfg_req_ptr;
  lte_rrc_conn_released_indi_s *conn_rel_ind_ptr;
  lte_rrc_conn_rel_ind_s *ext_conn_rel_ind_ptr;
  lte_rrc_tmri_s *conn_rel_tmr_ind_ptr;
  lte_rrc_dlm_processed_indi_s *ind_ptr;
  lte_rrc_trm_priority_change_indi_s *priority_change_indi_ptr;
  
  TF_MSG("Start of test");

  TF_MSG("Checking for INACTIVE state");
  TF_ASSERT(stm_get_state(LTE_RRC_CRP_SM) == LTE_RRC_CRP_SM__INACTIVE);

  lte_rrc_crp_test_stub_controller_set_state(LTE_RRC_CONTROLLER_SM__CONNECTED);

  TF_MSG("Sending Conn Release DLM");
  lte_rrc_crp_test_construct_conn_rel_msg(&conn_rel_dlm,FALSE,FALSE,FALSE);
  lte_rrc_utf_send_msg((byte*)&conn_rel_dlm, sizeof(conn_rel_dlm));
  
  TF_MSG("Checking for LTE_RRC_CONN_REL_STARTED_INDI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_rel_started_indi_s) == buf_len);
  conn_rel_started_ind_ptr = (lte_rrc_conn_rel_started_indi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_CONN_REL_STARTED_INDI == 
            conn_rel_started_ind_ptr->msg_hdr.id);
#if 0
  TF_MSG("Checking for LTE_RRC_TRM_PRIORITY_CHANGE_INDI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_trm_priority_change_indi_s) == buf_len);
  priority_change_indi_ptr = (lte_rrc_trm_priority_change_indi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_TRM_PRIORITY_CHANGE_INDI == priority_change_indi_ptr->msg_hdr.id);
#endif  
  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for WAIT_FOR_TIMER state");
  TF_ASSERT(stm_get_state(LTE_RRC_CRP_SM) == 
            LTE_RRC_CRP_SM__WAIT_FOR_TIMER);

  lte_rrc_utf_set_sync_cfg_reqi_params(LTE_RRC_CRP_TEST_CFG_TRANS_ID);

  TF_MSG("Expiring time");
  TF_SLEEP(LTE_RRC_CRP_TEST_CONN_REL_TMR_VAL);

  TF_MSG("Checking for LTE_RRC_CFG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);
  cfg_req_ptr = (lte_rrc_cfg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_CFG_REQI == cfg_req_ptr->msg_hdr.id);
  TF_ASSERT(TRUE == cfg_req_ptr->cnfi_required);
  TF_ASSERT(LTE_RRC_PROC_CRP == cfg_req_ptr->proc);
  TF_ASSERT(1 == cfg_req_ptr->num_cfg);
  TF_ASSERT(LTE_RRC_CFG_CONN_REL == cfg_req_ptr->cfg_info[0].cfg_type);

  TF_MSG("Checking for LTE_RRC_CONN_REL_TMRI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  conn_rel_tmr_ind_ptr = (lte_rrc_tmri_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_CONN_REL_TMRI == conn_rel_tmr_ind_ptr->msg_hdr.id);

  lte_rrc_utf_wait_for_done();
  
  TF_MSG("Checking for WAIT_FOR_LLC_CONFIRM state");
  TF_ASSERT(stm_get_state(LTE_RRC_CRP_SM) == 
            LTE_RRC_CRP_SM__WAIT_FOR_LLC_CONFIRM);

  TF_MSG("Sending Config Cnf");
  lte_rrc_init_default_hdr(&config_cnf.msg_hdr,LTE_RRC_CFG_CNFI);
  config_cnf.proc = LTE_RRC_PROC_CRP;
  config_cnf.trans_id = LTE_RRC_CRP_TEST_CFG_TRANS_ID;
  config_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&config_cnf, sizeof(config_cnf));

  TF_MSG("Checking for LTE_RRC_CONN_RELEASED_INDI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_released_indi_s) == buf_len);
  conn_rel_ind_ptr = (lte_rrc_conn_released_indi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_CONN_RELEASED_INDI == 
            conn_rel_ind_ptr->msg_hdr.id);
  TF_ASSERT(FALSE == conn_rel_ind_ptr->cell_barring_is_reqd);

  TF_MSG("Checking for LTE_RRC_CONN_REL_IND");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_rel_ind_s) == buf_len);
  ext_conn_rel_ind_ptr = (lte_rrc_conn_rel_ind_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_CONN_REL_IND == 
            ext_conn_rel_ind_ptr->msg_hdr.id);
  TF_ASSERT(FALSE == ext_conn_rel_ind_ptr->load_balancing_tau_is_reqd);
  TF_ASSERT(LTE_RRC_CONN_REL_NORMAL == ext_conn_rel_ind_ptr->rel_reason);

  TF_MSG("Checking for LTE_RRC_DLM_PROCESSED_INDI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_dlm_processed_indi_s) == buf_len);
  ind_ptr = (lte_rrc_dlm_processed_indi_s*)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_DLM_PROCESSED_INDI == ind_ptr->msg_hdr.id); 

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INACTIVE state");
  TF_ASSERT(stm_get_state(LTE_RRC_CRP_SM) == LTE_RRC_CRP_SM__INACTIVE);


  lte_rrc_osys_asn1_free_pdu(conn_rel_dlm.dl_msg_ptr, lte_rrc_osys_DL_DCCH_Message_PDU);

  TF_MSG("End of test");
}

void LteRrcCrpBasicConnRelTest::Teardown()
{
  lte_rrc_utf_teardown();
}

