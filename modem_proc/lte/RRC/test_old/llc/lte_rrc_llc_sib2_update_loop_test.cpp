/*!
  @file lte_rrc_llc_sib2_update_loop_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a LTE_RRC_CFG_REQI with basic SIB2 update.

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
#include "lte_rrc_sib.h"
#include "lte_static_rrc_mem_util.h"
}

#include "lte_rrc_llc_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>  


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_llc_sib2_update_loop_test.cpp
*/

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_llc_sib2_update_loop_test.cpp
*/

extern "C"
{
extern lte_rrc_llc_s lte_rrc_llc_data;
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcSib2UpdateLoop);

void LteRrcLlcSib2UpdateLoop::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcSib2UpdateLoop::Test()
{
  lte_rrc_cfg_reqi_s cfg_reqi;
  int loop_ctr = 0;
  uint32 trans_id;
  uint8 sib_dup = 0;
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
  TF_MSG("\nLLC: SIB2 Update Loop Test Begins \n");
  for (loop_ctr = 0; loop_ctr < 20; loop_ctr++) 
  {
    /* LLC should be in initial state  */
    TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
    /* build CFG_REQI for COMMON CFG and send */
    lte_rrc_llc_test_construct_common_cfg_reqi(&cfg_reqi, TRUE, TRUE, TRUE, &trans_id);
    lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
    lte_rrc_utf_wait_for_done();

#if 0
    if(0 == sib_dup)
    {
      sib_dup = 1;

      /* check the next cmd indx in cmd_db */
      TF_ASSERT(lte_rrc_llc_data.dd_ptr->cmd_db.next_common_phy_idx == (loop_ctr + 1) % LTE_RRC_MAX_NUM_OF_CMD_IN_HIST);
    }
    else
#endif
    {
      common_cfg_mask = LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG | LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG;
      lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
    }
    
    /* Verify phy common config request */
    lte_rrc_llc_test_verify_phy_common_cfg_req_and_send_cnf();

    /* Verify phy dedicated config request */
    lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CSP, TRUE);

    /* Verify MAC config request */
    lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(TRUE, NULL, LTE_RRC_PROC_CSP);
#if 0

    /* check the next cmd indx in cmd_db */
    TF_ASSERT(lte_rrc_llc_data.dd_ptr->cmd_db.next_mac_idx == 
              (loop_ctr + 1) % LTE_RRC_MAX_NUM_OF_CMD_IN_HIST);
#endif

    
        
    /* Verify CFG_CNFI from LLC */
    lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CSP, cfg_reqi.trans_id_ptr);
	
    /* LLC should be in initial state  */
    TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
    
    lte_rrc_llc_test_free_sib_ptr();
    TF_MSG("\nLLC: SIB2 Update Loop Test Ends \n");
    
  } /* for loop */
  
  lte_rrc_utf_reset(); 
  
  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcSib2UpdateLoop::Teardown()
{
  lte_rrc_utf_teardown();
}







