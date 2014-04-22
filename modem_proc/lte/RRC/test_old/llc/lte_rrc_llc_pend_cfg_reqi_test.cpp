/*!
  @file lte_rrc_llc_pend_cfg_reqi_test.cpp

  @brief
  Unit test code for the LLC procedure that tests pending  
  a LTE_RRC_CFG_REQI and LTE_MAC_RELEASE_RESOURCES_IND, when prior LTE_RRC_CFG_REQI 
  has not finished.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/llc/lte_rrc_llc_pend_cfg_reqi_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/09/08   da     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_llc.h"
#include "lte_rrc_sib.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_mac_ind.h"
}

#include "lte_rrc_llc_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>  

#define MAX_CMD_IN_PEND_Q 10

/*===========================================================================

   FUNCTION:  lte_rrc_llc_test_pendq_construct_and_send_common_cfg_reqi

===========================================================================*/
/*!
    @brief
    This function builds LTE_RRC_CFG_REQI for COMMON CFG 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_llc_test_pendq_construct_and_send_common_cfg_reqi
( 
  lte_rrc_cfg_reqi_s *cfg_reqi_ptr, /*!< pointer to cfg reqi */
  uint32 trans_id_idx
  
)
{
  LTE_ASSERT(cfg_reqi_ptr != NULL);
  TF_MSG("\nLLC: Send LTE_RRC_CFG_REQI for SIB2 Update \n");
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_reqi_ptr->msg_hdr,LTE_RRC_CFG_REQI);

  /* construct cfg_reqi for basic sib2 update */
  cfg_reqi_ptr->num_cfg = 1;
  cfg_reqi_ptr->proc = LTE_RRC_PROC_CSP;
  cfg_reqi_ptr->cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[trans_id_idx] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi_ptr->trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[trans_id_idx];
  cfg_reqi_ptr->num_cfg = 1;
  cfg_reqi_ptr->cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi_ptr->cfg_info[0].cfg_u.common_cfg.sib2_update = TRUE;
  cfg_reqi_ptr->cfg_info[0].cfg_u.common_cfg.sib1_update = TRUE;
  cfg_reqi_ptr->cfg_info[0].cfg_u.common_cfg.default_cfg = TRUE;
}/*lte_rrc_llc_test_pendq_construct_and_send_common_cfg_reqi*/

/*===========================================================================

   FUNCTION:  lte_rrc_llc_test_pended_msg_handling

===========================================================================*/
/*!
    @brief
    This function tests the handling of the pended config request

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_llc_test_pended_msg_handling
( 
  uint32 trans_id
)
{
  lte_rrc_pended_cfg_reqi_s *pended_cfg_reqi_ptr = NULL;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  byte *buf_ptr;
  uint32 buf_len;

  TF_MSG("\nLLC: Rcvd CPHY_COMMON_CFG_REQ \n");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  /* LLC should be in WAIT_FOR_CPHY_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  /* send CPHY_COMMON_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send CPHY_COMMON_CFG_CNF with sucess \n");
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CSP, TRUE);

    /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(TRUE, NULL, LTE_RRC_PROC_CSP);

  

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);

	/* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CSP, &trans_id);

}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPendCfgReqiTest);

void LteRrcLlcPendCfgReqiTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcPendCfgReqiTest::Test()
{
  byte *buf_ptr;
  uint32 buf_len, i, j, trans_id = 0;
  lte_rrc_cfg_reqi_s cfg_reqi, *cfg_reqi_ptr = NULL;
  lte_rrc_cfg_cnfi_s *cfg_cnfi_ptr = NULL;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  lte_cphy_dedicated_cfg_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
  lte_mac_release_resources_ind_s mac_rel_resrc_ind;
  lte_mac_cfg_req_msg_s *mac_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  lte_rrc_pended_cfg_reqi_s *pended_cfg_reqi_ptr = NULL;
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
  lte_rrc_proc_e proc;
  boolean test_default_cfg;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: Pend CFG_REQI Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nLLC: Send CFG_REQI for SIB2 Update \n");
  /* build CFG_REQI for COMMON CFG and send */
  lte_rrc_llc_test_construct_common_cfg_reqi(&cfg_reqi, TRUE, TRUE, TRUE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));

  proc = LTE_RRC_PROC_CSP;
  test_default_cfg = TRUE;

  TF_MSG("\nLLC: Rcvd CPHY_COMMON_CFG_REQ \n");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  /* LLC should be in WAIT_FOR_CPHY_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  /* send CPHY_COMMON_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send CPHY_COMMON_CFG_CNF with sucess \n");
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);
  TF_MSG("\nLLC: Rcvd CPHY_DEDICATED_CFG_REQ \n");
  
  if ((test_default_cfg) && (proc != LTE_RRC_PROC_CEP) && (proc != LTE_RRC_PROC_CRE))
  {
    TF_ASSERT(cphy_dedicated_cfg_req_ptr->is_default_cfg == TRUE);
  }
  else
  {
    TF_ASSERT(cphy_dedicated_cfg_req_ptr->is_default_cfg == FALSE);
  }
  
  /* Verify default dedicated PHY Config params */
  lte_rrc_llc_test_phy_dedicated_cfg(&cphy_dedicated_cfg_req_ptr->dedicated_cfg_param,test_default_cfg,proc);
  
  /* After CPHY Config req is sent, assert LLC is in WAIT_FOR_CPHY_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  
  TF_MSG("\nCPHY_DEDICATED_CFG_REQ rcvd and LLC Module in WAIT_FOR_CPHY_CFG_CNF state\n");

  for (i = 0; i < MAX_CMD_IN_PEND_Q; i++)
  {
    TF_MSG("\nLLC: Send CFG_REQI for SIB2 Update \n");
    /* build CFG_REQI for COMMON CFG and send */
    lte_rrc_llc_test_pendq_construct_and_send_common_cfg_reqi(&cfg_reqi, i);

    /* Send another cfg_reqi.  This one should be pended and handled once LLC
       goes back to initial state */
    lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
    lte_rrc_utf_wait_for_done();
  }

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  /* Setting the fields in the LTE_CPHY_DEDICATED_CFG_CNF sent by CPHY, The bits
    set in dedicated_cfg_validity field reflect the channel config that were set
    in the llc_conn_setup_msg in the lte_rrc_llc_ota_msg.h */
  cphy_dedicated_cfg_cnf.dedicated_cfg_validity = cphy_dedicated_cfg_req_ptr->dedicated_cfg_param.cfg_mask;
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();
  
  /* Verify phy dedicated config request */
  //lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CSP, TRUE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(TRUE, NULL, LTE_RRC_PROC_CSP);

  

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CSP, &trans_id);

  // -------------
  // Trans id 1
  // -------------
  trans_id = 1;
	lte_rrc_utf_wait_for_done();
  lte_rrc_llc_test_pended_msg_handling(trans_id);

  // -------------
  // Trans id 3
  // -------------
  trans_id = 3;
	lte_rrc_utf_wait_for_done();
  lte_rrc_llc_test_pended_msg_handling(trans_id);

  // -------------
  // Trans id 5
  // -------------
  trans_id = 5;
	lte_rrc_utf_wait_for_done();
  lte_rrc_llc_test_pended_msg_handling(trans_id);

  // -------------
  // Trans id 7
  // -------------
  trans_id = 7;
	lte_rrc_utf_wait_for_done();
  lte_rrc_llc_test_pended_msg_handling(trans_id);

  // -------------
  // Trans id 9
  // -------------
  trans_id = 9;
	lte_rrc_utf_wait_for_done();
  lte_rrc_llc_test_pended_msg_handling(trans_id);

  // -------------
  // Trans id 2
  // -------------
  trans_id = 2;
	lte_rrc_utf_wait_for_done();
  lte_rrc_llc_test_pended_msg_handling(trans_id);

  // -------------
  // Trans id 6
  // -------------
  trans_id = 6;
	lte_rrc_utf_wait_for_done();
  lte_rrc_llc_test_pended_msg_handling(trans_id);

  // -------------
  // Trans id 10
  // -------------
  trans_id = 10;
	lte_rrc_utf_wait_for_done();
  lte_rrc_llc_test_pended_msg_handling(trans_id);

  // -------------
  // Trans id 8
  // -------------
  trans_id = 8;
	lte_rrc_utf_wait_for_done();
  lte_rrc_llc_test_pended_msg_handling(trans_id);


	lte_rrc_utf_wait_for_done();
  // -------------
  // Trans id 4
  // -------------
  // Not calling lte_rrc_llc_test_pended_msg_handling fn since only one pended req
  // is remaining and will get unpended. In the fn we expect two pended requests
  // to be unpended
  trans_id = 4;

  TF_MSG("\nLLC: Rcvd CPHY_COMMON_CFG_REQ \n");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  /* LLC should be in WAIT_FOR_CPHY_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  /* send CPHY_COMMON_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send CPHY_COMMON_CFG_CNF with sucess \n");
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CSP, TRUE);

    /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(TRUE, NULL, LTE_RRC_PROC_CSP);

  

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CSP, &trans_id);

  /* Send LTE_MAC_RELEASE_RESOURCES_IND and send to LLC. This one should be pended and handled 
    once LLC goes back to initial state */
  lte_rrc_init_default_hdr(&mac_rel_resrc_ind.hdr, LTE_MAC_RELEASE_RESOURCES_IND);
  lte_rrc_utf_send_msg((byte*)&mac_rel_resrc_ind, sizeof(mac_rel_resrc_ind));
  lte_rrc_utf_wait_for_done();

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_sib_ptr();
  TF_MSG("\nLLC: Pend CFG_REQI Test Ends \n");

  /* ---------------------------------------------------------------------- */

  lte_rrc_utf_reset(); 
}

void LteRrcLlcPendCfgReqiTest::Teardown()
{
  lte_rrc_utf_teardown();
}







