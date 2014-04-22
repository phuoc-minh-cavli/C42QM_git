/*!
  @file lte_rrc_controller_sim_update_test.cpp

  @brief
  Unit test code for processing SIM update in INACTIVE state

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
10/13/08   sureshs Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <lte_rrc_ext_msg.h>
#include <lte_rrc_controller.h>
}

#include <TestFramework.h>
#include <lte_rrc_utf.h>

//message structures
static lte_rrc_sim_update_req_s sim_update_req;

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcControllerSimUpdateTest);

void LteRrcControllerSimUpdateTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONTROLLER_SM, NULL, 0);
}

void LteRrcControllerSimUpdateTest::Test()
{
  TF_MSG("Start of test");

  TF_MSG("Checking for INACTIVE state");
  TF_ASSERT(lte_rrc_controller_get_state() == 
            LTE_RRC_CONTROLLER_SM__INACTIVE);

  TF_MSG("Sending SIM update req");
  memset(&sim_update_req,0,sizeof(sim_update_req));
  msgr_init_hdr(&sim_update_req.msg_hdr,MSGR_NAS_EMM,LTE_RRC_SIM_UPDATE_REQ);
  sim_update_req.ue_id_list_is_valid = TRUE;
  sim_update_req.access_class = 3;
  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for access class value");
  TF_ASSERT(lte_rrc_controller_get_access_class() == 3);

  TF_MSG("Checking for INACTIVE state");
  TF_ASSERT(lte_rrc_controller_get_state() == 
            LTE_RRC_CONTROLLER_SM__INACTIVE);

  TF_MSG("End of test");
}

void LteRrcControllerSimUpdateTest::Teardown()
{
  lte_rrc_utf_teardown();
}

