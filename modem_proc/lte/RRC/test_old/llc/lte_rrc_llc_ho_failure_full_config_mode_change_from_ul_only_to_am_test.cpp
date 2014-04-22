
/*!
  @file lte_rrc_llc_rel9_ho_failure_full_config_mode_change_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the complete Reestablishment 
  cycle from HO failure, to handling the Reestablishment message and the first
  Reconfiguration message following Reestablishment containing the full config
  option for Rel 9.
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
lte_rrc_llc_ho_failure_full_config_mode_change_from_bi_to_ul_only_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/14/10   mm     Initial Revision
           
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
  TF_PROTO_STUB(boolean, lte_rrc_config_ho_in_progress, void);
}

boolean mode_change_ul_reconfig_after_cre_for_ul_to_am = FALSE;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_full_config_mode_change_reconfig_after_cre_for_UL_to_AM

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
boolean lte_rrc_llc_test_full_config_mode_change_ul_reconfig_after_cre_for_UL_to_AM
(
  void
)
{
  if (mode_change_ul_reconfig_after_cre_for_ul_to_am)
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


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcHOFullConfigFailureModeChangeFromUlOnlyToAmTest);

void LteRrcLlcHOFullConfigFailureModeChangeFromUlOnlyToAmTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_config_is_first_reconfig_after_cre) = 
    lte_rrc_llc_test_full_config_mode_change_ul_reconfig_after_cre_for_UL_to_AM;
	TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_llc_test_get_ho_in_progress;
}

void LteRrcLlcHOFullConfigFailureModeChangeFromUlOnlyToAmTest::Test()
{   
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_ul();
  /* Test RACH failure for HO mesg with full config option */
  lte_rrc_llc_test_ho_with_mode_change_from_ul_only_to_am_full_config_failure();
  lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(LTE_CPHY_COMMON_CFG_CURRENT_VALID_MASK_BITS);
  lte_rrc_llc_test_basic_sib2_update(); 
  lte_rrc_llc_test_rrc_connection_reestablishment();
  mode_change_ul_reconfig_after_cre_for_ul_to_am = TRUE;
  lte_rrc_llc_test_rel9_reconfig_after_cre_full_config(FALSE);
  lte_rrc_utf_reset(); 

}

void LteRrcLlcHOFullConfigFailureModeChangeFromUlOnlyToAmTest::Teardown()
{
  lte_rrc_utf_teardown();
}


