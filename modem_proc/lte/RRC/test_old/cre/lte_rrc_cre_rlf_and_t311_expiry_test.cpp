/*!
  @file lte_rrc_cre_rlf_and_t311_expiry_test.cpp

  @brief
  Unit test code for CRE procedure that tests the handling of RLF indication 
  from ML1 followed by T311 expiry

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
lte_rrc_cre_rlf_and_t311_expiry_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/09   mm     Changed the test case to verify T311 expiry handling following
                  RLF indication from ML1
02/10/09   da     Initial Revision
           
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
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_cre_test_common.h"
#include <tf_stub.h>


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define LTE_RRC_CONFIG_TEST_T311_TMRI_VAL 1000
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCreRlfAndT311expiryTest);

void LteRrcCreRlfAndT311expiryTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CRE_SM, NULL, 0);
  lte_rrc_cre_test_stub_init();
}

void LteRrcCreRlfAndT311expiryTest::Test()
{
  lte_cphy_rl_failure_ind_s rl_fail_ind;
  lte_rrc_cre_started_indi_s *cre_started_indi_ptr = NULL;
  lte_rrc_sib_abort_reqi_s* sib_abort_reqi_ptr = NULL;
  lte_rrc_cfg_reqi_s* cfg_req_msg_ptr = NULL;
  lte_rrc_cfg_cnfi_s cfg_cnfi_msg;
  lte_rrc_initiate_cell_sel_reqi_s *cel_sel_reqi_ptr = NULL;
  lte_rrc_tmri_s *t311_tmri_ptr = NULL;
  lte_rrc_initiate_conn_rel_indi_s *initiate_conn_rel_indi_ptr = NULL;
  uint32 buf_len = 0; uint32 decode_status = 0;

  /* ---------------------------------------------------------------------- */
 
  TF_MSG("\nCRE RLF test and T311 expiry begins \n");

  /* Make sure CRE is in initial state on receiving a RLF indication */
  TF_ASSERT(lte_rrc_cre_get_state() == LTE_RRC_CRE_SM__INITIAL);

  /* Setting the header for RLF indication */
  lte_rrc_init_default_hdr(&rl_fail_ind.msg_hdr,LTE_CPHY_RL_FAILURE_IND);
  lte_rrc_utf_send_msg((byte*)&rl_fail_ind, sizeof(rl_fail_ind));

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

  TF_MSG("T311 timer expires");
  TF_SLEEP(LTE_RRC_CONFIG_TEST_T311_TMRI_VAL);
  
  /* Catch the T311 Timer expiry */
  lte_rrc_utf_get_next_int_msg((byte **)&t311_tmri_ptr,&buf_len);
  /* Make sure the timer indication is valid */
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);

  /* catch the initiate connection release indication thrown by CRE */
  lte_rrc_utf_get_next_int_msg((byte **)&initiate_conn_rel_indi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_initiate_conn_rel_indi_s) == buf_len);
  TF_ASSERT(initiate_conn_rel_indi_ptr != NULL);
  TF_ASSERT(initiate_conn_rel_indi_ptr->rel_reason == LTE_RRC_CONN_REL_RLF);

  /* Assert that CRE is in INITIAL state */
  TF_ASSERT(lte_rrc_cre_get_state() == LTE_RRC_CRE_SM__INITIAL);

  lte_rrc_cre_test_free_sib_ptr();

  TF_MSG("\nCRE RLF and T311 expiry test ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcCreRlfAndT311expiryTest::Teardown()
{
  lte_rrc_utf_teardown();
}


