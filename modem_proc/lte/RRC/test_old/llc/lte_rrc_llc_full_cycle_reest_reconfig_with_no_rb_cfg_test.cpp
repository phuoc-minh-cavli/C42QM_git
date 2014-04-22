/*!
  @file lte_rrc_llc_full_cycle_reest_reconfig_with_no_rb_cfg_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the complete Reestablishment 
  cycle from HO failure, to handling the Reestablishment message and the first
  Reconfiguration message following Reestablishment contains no RB configuration.
  All suspended RBs need to be resumed, however. This test specifically tests
  suspending and resuming DL-only and UL-only RBs.
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

$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/llc/
lte_rrc_llc_full_cycle_reest_reconfig_with_no_rb_cfg_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/09   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
  #include "lte_rrc_sib.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include "lte_rrc_config.h"
#include <tf_stub.h>
#include <TestFramework.h>

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

extern "C"
{
  TF_PROTO_STUB(boolean, lte_rrc_config_is_first_reconfig_after_cre, void);
}

boolean reconfig_after_cre = FALSE;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_after_cre

===========================================================================*/
/*!
    @brief
    The function simulates the config that returns if the reconfig message that 
    is being processed is the first after connectionr reestablishment

    @detail
    The function returns true if the reconfig message that is being processed
    is the first after connectionr reestablishment. Else it returns false.

    @return
    boolean

    @retval value
    boolean

*/
/*=========================================================================*/
boolean lte_rrc_llc_test_reconfig_after_cre
(
  void
)
{
  if (reconfig_after_cre)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcFullCycleReestReconfigWithNoRBCfgTest);

void LteRrcLlcFullCycleReestReconfigWithNoRBCfgTest ::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_config_is_first_reconfig_after_cre) = lte_rrc_llc_test_reconfig_after_cre;
}

void LteRrcLlcFullCycleReestReconfigWithNoRBCfgTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_common_cfg_mask_t common_cfg_mask;
  
  /* ---------------------------------------------------------------------- */
  /* Test RLF processing and resuming for DL only RB */

  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_dl();
  /* Test the processing of RLF for a non-HO scenario */
  lte_rrc_llc_test_rlf();

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

  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_reestablishment();
  reconfig_after_cre = TRUE;
  lte_rrc_llc_test_first_reconfig_after_cre();

  encoded_pdu.length = sizeof(llc_reconfig_with_drb_rel_msg);
  encoded_pdu.value = llc_reconfig_with_drb_rel_msg;
  reconfig_after_cre = FALSE;
  lte_rrc_llc_test_reconfig_release_drb3(encoded_pdu,LTE_RRC_LLC_RB_DL_DIRECTION);
  lte_rrc_llc_test_conn_release(TRUE);

  /* ---------------------------------------------------------------------- */
  /* Test RLF processing and resuming for UL only RB */
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_ul();
    /* Test the processing of RLF for a non-HO scenario */
  lte_rrc_llc_test_rlf();

  lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_reestablishment();
  reconfig_after_cre = TRUE;
  lte_rrc_llc_test_first_reconfig_after_cre();
  reconfig_after_cre = FALSE;
  lte_rrc_llc_test_reconfig_release_drb3(encoded_pdu,LTE_RRC_LLC_RB_UL_DIRECTION);
  lte_rrc_llc_test_conn_release(TRUE);
  lte_rrc_utf_reset(); 

}

void LteRrcLlcFullCycleReestReconfigWithNoRBCfgTest::Teardown()
{
  lte_rrc_utf_teardown();
}






