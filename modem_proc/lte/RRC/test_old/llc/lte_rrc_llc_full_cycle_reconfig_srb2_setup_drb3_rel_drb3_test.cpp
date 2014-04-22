/*!
  @file lte_rrc_llc_full_cycle_reconfig_srb2_setup_drb3_rel_drb3_test.cpp

  @brief
  Full Call cycle prcessing - 
  Unit test code for the LLC procedure that tests the processing of SIB2 update,
  Connection Setup followed by Connection Reconfiguration message that reconfigures 
  SRB2 and sets up DRB3, another Reconfiguration message that releases DRB3 and finally 
  connection release message that releases all the SRBs.

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
lte_rrc_llc_full_cycle_reconfig_srb2_setup_drb3_rel_drb3_test.cpp#1 $

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
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h> 

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcFullCycleReconfigSrb2SetupDrb3RelDRB3Test);

void LteRrcFullCycleReconfigSrb2SetupDrb3RelDRB3Test::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcFullCycleReconfigSrb2SetupDrb3RelDRB3Test::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_active_eps_bearer_update_ind_s eps_list_msg;
  TF_MSG("\nFull Cycle - Connection Reconfiguration with DRB3 Release Test Begins \n");
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  lte_rrc_llc_get_epsbearer_info(&eps_list_msg);
  TF_ASSERT(eps_list_msg.num_active_bearers == 1);
  TF_ASSERT(eps_list_msg.active_eps_bearers[0].eps_id == 3);
  TF_ASSERT(eps_list_msg.active_eps_bearers[0].rb_id == 3);
  TF_ASSERT(eps_list_msg.num_added_bearers == 1);
  TF_ASSERT(eps_list_msg.added_eps_bearers[0].eps_id == 3);
  TF_ASSERT(eps_list_msg.added_eps_bearers[0].rb_id == 3);
  TF_ASSERT(eps_list_msg.num_removed_bearers == 0);
  encoded_pdu.length = sizeof(llc_reconfig_with_drb_rel_msg);
  encoded_pdu.value = llc_reconfig_with_drb_rel_msg;
  lte_rrc_llc_test_reconfig_release_drb3(encoded_pdu,LTE_RRC_LLC_RB_BI_DIRECTION);
  lte_rrc_llc_get_epsbearer_info(&eps_list_msg);
  TF_ASSERT(eps_list_msg.num_active_bearers == 0);
  TF_ASSERT(eps_list_msg.num_added_bearers == 0);
  TF_ASSERT(eps_list_msg.num_removed_bearers == 1);
  TF_ASSERT(eps_list_msg.removed_eps_bearers[0].eps_id == 3);
  TF_ASSERT(eps_list_msg.removed_eps_bearers[0].rb_id == 3);
  lte_rrc_llc_test_conn_release(TRUE);
  TF_MSG("\nFull Cycle - Connection Reconfiguration with DRB3 Release Test Ends \n");
  lte_rrc_utf_reset(); 
}

void LteRrcFullCycleReconfigSrb2SetupDrb3RelDRB3Test::Teardown()
{
  lte_rrc_utf_teardown();
}





