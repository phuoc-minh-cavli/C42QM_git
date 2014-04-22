/*!
  @file lte_rrc_llc_basic_sib1_and_sib2_update_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a LTE_RRC_CFG_REQI with basic SIB1 and SIB2 update. This also verifies the
  handling of CFG_REQI by LLC when CSP is not in camped state.

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
#include "lte_rrc_csp.h"
#include "lte_rrc_sib.h"
#include "lte_static_rrc_mem_util.h"
}

#include "lte_rrc_llc_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>  

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_llc_basic_sib1_and_sib2_update_test.cpp
*/
extern "C"
{
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s*);
  TF_PROTO_STUB(void *, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
}

/*===========================================================================

                         INTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_not_camped_csp_get_serving_cell_params

===========================================================================*/
/*!
    @brief
    This function returns E_FAILURE to simulate CSP being in IDLE_NOT_CAMPED state

    @return
    E_FAILURE

 */
/*=========================================================================*/
lte_errno_e lte_rrc_llc_test_not_camped_csp_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr /*!< Pointer to cell info */
)
{
  return E_FAILURE;
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcBasicSib1AndSib2Update);

void LteRrcLlcBasicSib1AndSib2Update::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params;
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;
}

void LteRrcLlcBasicSib1AndSib2Update::Test()
{
  lte_rrc_cfg_reqi_s cfg_reqi;
  uint32 trans_id;
  lte_cphy_common_cfg_mask_t common_cfg_mask;

  common_cfg_mask = 
  (
  	LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL |
    LTE_CPHY_COMMON_CFG_MASK_BIT_DRX_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH |
    LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM |
    LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG
  );
  lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
 
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: Basic SIB1 and SIB2 Update Test Begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* build CFG_REQI for COMMON CFG and send */
  lte_rrc_llc_test_construct_common_cfg_reqi(&cfg_reqi, TRUE, TRUE, TRUE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  /* Verify phy common config request */
  lte_rrc_llc_test_verify_phy_common_cfg_req_and_send_cnf();

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CSP, TRUE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(TRUE, NULL, LTE_RRC_PROC_CSP);
  
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CSP, cfg_reqi.trans_id_ptr);

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Make the stub fn for lte_rrc_csp_get_serving_cell_params to return E_FAILURE */
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_not_camped_csp_get_serving_cell_params;

  /* build CFG_REQI for COMMON CFG and send */
  lte_rrc_llc_test_construct_common_cfg_reqi(&cfg_reqi, TRUE, TRUE, TRUE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  /* The CFG_REQI should be ignored by LLC and it should still be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_sib_ptr();
  TF_MSG("\nLLC: Basic SIB1 and SIB2 Update Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcBasicSib1AndSib2Update::Teardown()
{
  lte_rrc_utf_teardown();
}


