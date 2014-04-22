/*!
  @file lte_rrc_llc_conn_setup_reconfig_drb3_am_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  Connection Setup followed by Connection Reconfiguration message that sets up
  a UM Bi-directional DRB.

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
lte_rrc_llc_sib2_update_conn_setup_reconfig_drb3_rel_test.cpp#1 $

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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcConnSetupReconfigDRB3AMTest);

void LteRrcLlcConnSetupReconfigDRB3AMTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcConnSetupReconfigDRB3AMTest::Test()
{
  lte_rrc_active_eps_bearer_update_ind_s eps_list_msg;
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_setup_srb2_drb3_am();
  lte_rrc_llc_get_epsbearer_info(&eps_list_msg);
  TF_ASSERT(eps_list_msg.num_active_bearers == 1);
  TF_ASSERT(eps_list_msg.active_eps_bearers[0].eps_id == 3);
  TF_ASSERT(eps_list_msg.active_eps_bearers[0].rb_id == 3);
  TF_ASSERT(eps_list_msg.num_added_bearers == 1);
  TF_ASSERT(eps_list_msg.added_eps_bearers[0].eps_id == 3);
  TF_ASSERT(eps_list_msg.added_eps_bearers[0].rb_id == 3);
  TF_ASSERT(eps_list_msg.num_removed_bearers == 0);

  /* Test STOPPED_INDI*/
  lte_rrc_llc_test_send_stopped_indi();

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_utf_reset(); 
}

void LteRrcLlcConnSetupReconfigDRB3AMTest::Teardown()
{
  lte_rrc_utf_teardown();
}







