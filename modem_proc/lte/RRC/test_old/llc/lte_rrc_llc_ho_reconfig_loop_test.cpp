/*!
  @file lte_rrc_llc_ho_reconfig_loop_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of back to back
  sequence of Reconfig without Mobility and Reconfig with Mobility information.
  To start with, UE receives a Connection Setup followed by Connection Reconfiguration 
  message that sets up a UM Bi-directional DRB, before handover.

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
lte_rrc_llc_ho_reconfig_loop_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/09   mm     Initial Revision
           
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

#define MAX_LOOP_COUNT 10
/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcReconfigHOLoopTest);

void LteRrcLlcReconfigHOLoopTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcReconfigHOLoopTest::Test()
{
  int i = 0;
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  lte_rrc_llc_test_ho_delta_config_with_drb3_release();
  lte_rrc_llc_test_setup_drb_um_bi(3,10);
  for (i = 1;i < MAX_LOOP_COUNT; i++)
  {
    lte_rrc_llc_test_ho_delta_config_with_drb3_release();
    lte_rrc_llc_test_setup_drb_um_bi(3,10);
  }
  lte_rrc_utf_reset(); 
}

void LteRrcLlcReconfigHOLoopTest::Teardown()
{
  lte_rrc_utf_teardown();
}









