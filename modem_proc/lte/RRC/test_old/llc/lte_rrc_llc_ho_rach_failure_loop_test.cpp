/*!
  @file lte_rrc_llc_ho_rach_failure_loop_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of HO related
  configuration when a UE receives a Reconfig message containing mobility 
  information and a failure due to RACH. Different RACH failure scenarios are
  tested by HO msg with no DRB, HO msg with DRB add, modify or release in isolation
  as well as HO msg with DRB add, modify and release together. To start with UE 
  receives a Connection Setup followed by Connection Reconfiguration message 
  that sets up a UM Bi-directional DRB, before handover. The above steps are 
  tested in a 50-time loop.

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
lte_rrc_llc_ho_rach_failure_loop_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/09   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_sib.h"
#include "lte_rrc_csp.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h>  	
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define MAX_LOOP_COUNT 50
/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
extern "C"
{
  TF_PROTO_STUB(boolean, lte_rrc_config_is_first_reconfig_after_cre, void);
}

boolean rach_loop_reconfig_after_cre = FALSE;
/*===========================================================================

                                FUNCTIONS

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_rrc_llc_rach_loop_test_reconfig_after_cre

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
boolean lte_rrc_llc_rach_loop_test_reconfig_after_cre
(
  void
)
{
  if (rach_loop_reconfig_after_cre)
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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcHORACHFailureLoopTest);

void LteRrcLlcHORACHFailureLoopTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_config_is_first_reconfig_after_cre) = lte_rrc_llc_rach_loop_test_reconfig_after_cre;
}

void LteRrcLlcHORACHFailureLoopTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  int i = 0;
  lte_cphy_common_cfg_mask_t common_cfg_mask;
  
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */
  encoded_pdu.length = sizeof(llc_reconfig_intra_freq_mobility_info_only_msg);
  encoded_pdu.value = llc_reconfig_intra_freq_mobility_info_only_msg;
  
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
  
  for (i = 0; i < MAX_LOOP_COUNT; i++)
  {
    TF_MSG("LOOP: %d",i);
    lte_rrc_llc_test_basic_sib2_update();
    lte_rrc_llc_test_rrc_connection_setup_default();
    lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
    /* Test HO msg with no DRB information */
    lte_rrc_llc_test_intra_freq_mobility_only(encoded_pdu, FALSE);

    /* Test RACH failure for the above mesg */
    lte_rrc_llc_test_ho_with_no_drb_rach_failure();

    lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
    lte_rrc_llc_test_basic_sib2_update();
    lte_rrc_llc_test_rrc_connection_reestablishment();
    rach_loop_reconfig_after_cre = TRUE;
    lte_rrc_llc_test_first_reconfig_after_cre();

    /* Test RACH failure for HO mesg with DRB Add */
    rach_loop_reconfig_after_cre = FALSE;
    lte_rrc_llc_test_ho_with_drb_add_rach_failure();

    /*Effective for the first loop*/
    lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
    lte_rrc_llc_test_basic_sib2_update();
    lte_rrc_llc_test_rrc_connection_reestablishment();
    rach_loop_reconfig_after_cre = TRUE;
    lte_rrc_llc_test_first_reconfig_after_cre();

    /* Test RACH failure for HO mesg with DRB Modify */
    rach_loop_reconfig_after_cre = FALSE;
    lte_rrc_llc_test_ho_with_drb_modify_rach_failure();
    lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
    lte_rrc_llc_test_basic_sib2_update();
    lte_rrc_llc_test_rrc_connection_reestablishment();
    rach_loop_reconfig_after_cre = TRUE;
    lte_rrc_llc_test_first_reconfig_after_cre();

    /* Test RACH failure for HO msg with DRB release */
    rach_loop_reconfig_after_cre = FALSE;
    /* Test RACH failure for the above mesg */
    lte_rrc_llc_test_ho_with_drb_rel_rach_failure();
    lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
    lte_rrc_llc_test_basic_sib2_update();
    lte_rrc_llc_test_rrc_connection_reestablishment(); 
    rach_loop_reconfig_after_cre = TRUE;
    lte_rrc_llc_test_reconfig_drb3_setup_drb4_drb5_um_bi();
    rach_loop_reconfig_after_cre = FALSE;
    /* Test RACH failure for HO mesg with DRB Add, Modify and Release */
    lte_rrc_llc_test_ho_with_drb_add_modify_rel_rach_failure();
    /* Release all DRBs and the RRC connection*/
    lte_rrc_llc_test_conn_release(FALSE);

    /* Needed for lte_rrc_llc_test_basic_sib2_update in the next loop */
	lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(LTE_CPHY_COMMON_CFG_CURRENT_VALID_MASK_BITS);
  }
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_utf_reset(); 
}

void LteRrcLlcHORACHFailureLoopTest::Teardown()
{
  lte_rrc_utf_teardown();
}












