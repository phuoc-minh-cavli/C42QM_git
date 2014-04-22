/*!
  @file lte_rrc_llc_active_eps_bearer_test.cpp
 
  @brief
  Full Call cycle prcessing - 
  Unit test code for the LLC procedure that tests the processing of SIB2 update,
  Connection Setup, Connection Reconfiguration message that sets up a DRB3, 
  followed by another Reconfiguration message that reconfigures SRB2 and DRB3 
  and sets up DRB4 and DRB5. Finally a connection release causes all RBs to be
  released. The function lte_rrc_llc_get_active_epsbearer_list() is called after 
  each Reconfiguration/release message to check if the correct active eps 
  bearer list is returned.

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
lte_rrc_llc_active_eps_bearer_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/08   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte_rrc_utils.h"
#include "__lte_rrc_llc.h"
#include "lte_rrc_sib.h"
#include "lte_rrc_llc.h"
#include "lte_rrc_llci.h"
#include "lte_static_rrc_mem_util.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h>  


extern "C"
{
  TF_PROTO_STUB(stm_state_t, lte_rrc_controller_get_state, void);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcActiveEPSBearerTest);

void LteRrcLlcActiveEPSBearerTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcActiveEPSBearerTest::Test()
{
  lte_rrc_active_eps_bearer_update_ind_s eps_list_msg;
  lte_rrc_llc_test_basic_sib2_update(); 
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  TF_STUB(lte_rrc_controller_get_state) = lte_rrc_llc_test_controller_get_connected_state;
  lte_rrc_llc_get_epsbearer_info(&eps_list_msg);
  TF_ASSERT(eps_list_msg.num_active_bearers == 1);
  TF_ASSERT(eps_list_msg.active_eps_bearers[0].eps_id == 3);
  TF_ASSERT(eps_list_msg.num_added_bearers == 1);
  TF_ASSERT(eps_list_msg.added_eps_bearers[0].eps_id == 3);
  TF_ASSERT(eps_list_msg.num_removed_bearers == 0);
  lte_rrc_llc_test_reconfig_drb3_setup_drb4_drb5_um_bi();
  lte_rrc_llc_get_epsbearer_info(&eps_list_msg);
  TF_ASSERT(eps_list_msg.num_active_bearers == 3);
  TF_ASSERT(eps_list_msg.active_eps_bearers[0].eps_id  == 3);
  TF_ASSERT(eps_list_msg.active_eps_bearers[0].rb_id  == 3);
  TF_ASSERT(eps_list_msg.active_eps_bearers[1].eps_id  == 4);
  TF_ASSERT(eps_list_msg.active_eps_bearers[1].rb_id  == 4);
  TF_ASSERT(eps_list_msg.active_eps_bearers[2].eps_id  == 5);
  TF_ASSERT(eps_list_msg.active_eps_bearers[2].rb_id  == 5);
  TF_ASSERT(eps_list_msg.num_added_bearers == 2);
  TF_ASSERT(eps_list_msg.added_eps_bearers[0].eps_id  == 4);
  TF_ASSERT(eps_list_msg.added_eps_bearers[0].rb_id  == 4);
  TF_ASSERT(eps_list_msg.added_eps_bearers[1].eps_id  == 5);
  TF_ASSERT(eps_list_msg.added_eps_bearers[1].rb_id  == 5);
  TF_ASSERT(eps_list_msg.num_removed_bearers == 0);
  lte_rrc_llc_test_conn_release(TRUE);
  lte_rrc_llc_get_epsbearer_info(&eps_list_msg);
  TF_ASSERT(eps_list_msg.num_active_bearers == 0);
  TF_ASSERT(eps_list_msg.num_added_bearers == 0);
  TF_ASSERT(eps_list_msg.num_removed_bearers == 3);
  TF_ASSERT(eps_list_msg.removed_eps_bearers[0].eps_id  == 3);
  TF_ASSERT(eps_list_msg.removed_eps_bearers[0].rb_id  == 3);
  TF_ASSERT(eps_list_msg.removed_eps_bearers[1].eps_id  == 4);
  TF_ASSERT(eps_list_msg.removed_eps_bearers[1].rb_id  == 4);
  TF_ASSERT(eps_list_msg.removed_eps_bearers[2].eps_id  == 5);
  TF_ASSERT(eps_list_msg.removed_eps_bearers[2].rb_id  == 5);
  lte_rrc_utf_reset(); 
}

void LteRrcLlcActiveEPSBearerTest::Teardown()
{
  lte_rrc_utf_teardown();
}










