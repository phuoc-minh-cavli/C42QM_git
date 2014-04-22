/*!
  @file lte_rrc_llc_reconfig_same_drb_released_setup_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  Connection Setup followed by Connection Reconfiguration message that sets up
  a UM bi-directional DRB. This is followed by a Reconfiguration message that, 
  in one go, releases the DRB setup previously, and sets up the same DRB as 
  a bi-directional RB.

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
lte_rrc_llc_reconfig_same_drb_released_setup_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/08   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_sib.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h>  

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcReconfigSameDRBReleasedSetupTest);

void LteRrcLlcReconfigSameDRBReleasedSetupTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcReconfigSameDRBReleasedSetupTest::Test()
{
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  lte_rrc_llc_test_reconfig_same_drb_released_setup();
  lte_rrc_utf_reset(); 
}

void LteRrcLlcReconfigSameDRBReleasedSetupTest::Teardown()
{
  lte_rrc_utf_teardown();
}









