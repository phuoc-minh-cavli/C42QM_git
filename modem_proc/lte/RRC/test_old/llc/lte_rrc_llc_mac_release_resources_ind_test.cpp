/*!
  @file lte_rrc_llc_mac_release_resources_ind_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a LTE_MAC_RELEASE_RESOURCES_IND message.

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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
lte_rrc_llc_mac_release_resources_ind_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/00   da     Initial Revision
           
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
#include "lte_mac_ind.h"
#include <tf_stub.h>  
/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcMacReleaseResourcesIndTest);

void LteRrcLlcMacReleaseResourcesIndTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcMacReleaseResourcesIndTest::Test()
{
  lte_mac_release_resources_ind_s mac_rel_resrc_ind;

  TF_MSG("\nLLC LTE_MAC_RELEASE_RESOURCES_IND test begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();

  /* Send an out-of-sync request */
  lte_rrc_llc_test_mac_release_resources_ind();

  /* send another LTE_MAC_RELEASE_RESOURCES_IND.  Since SRS/CQI/SR configurations have already
     been release, LLC shouldn't send anything to ML1 */
  lte_rrc_init_default_hdr(&mac_rel_resrc_ind.hdr, LTE_MAC_RELEASE_RESOURCES_IND);
  lte_rrc_utf_send_msg((byte*)&mac_rel_resrc_ind, sizeof(mac_rel_resrc_ind));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nLLC LTE_MAC_RELEASE_RESOURCES_IND test ends \n");
  lte_rrc_utf_reset(); 

}

void LteRrcLlcMacReleaseResourcesIndTest::Teardown()
{
  lte_rrc_utf_teardown();
}



