/*!
  @file lte_rrc_llc_embms_config_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of CFG_REQI for
  eMBMS to configure MCCH and MTCH.

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

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/27/11   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_llc.h"
#include "lte_static_rrc_mem_util.h"

}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h>  

extern "C"
{
  TF_PROTO_STUB(void *, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
}
 
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcEMBMSConfigTest);

void LteRrcLlcEMBMSConfigTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_srs_get_serv_cell_sib;
}

void LteRrcLlcEMBMSConfigTest::Test()
{
  lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg(); 
  
  TF_MSG("\neMBMS - MCCH/MTCH setup Test Begins \n");

  /* Send LTE_RRC_CFG_REQI to LLC for MCCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_setup();

  /* Verify MCCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  /* Send LTE_RRC_CFG_REQI to LLC for MTCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mtch_setup();

  /* Verify MCCH/MTCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();
  
  TF_MSG("\neMBMS - MCCH/MTCH setup Test Ends \n");
  lte_rrc_utf_reset(); 

}

void LteRrcLlcEMBMSConfigTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcEMBMSPMCHOrderingTest);

void LteRrcLlcEMBMSPMCHOrderingTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_srs_get_serv_cell_sib;
}

void LteRrcLlcEMBMSPMCHOrderingTest::Test()
{
  lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg(); 
  
  TF_MSG("\neMBMS - MCCH/MTCH setup Test Begins \n");

  /* Send LTE_RRC_CFG_REQI to LLC for MCCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_setup();

  /* Verify MCCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  /* Send LTE_RRC_CFG_REQI to LLC for MTCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_same_pmch();

  /* Verify MCCH/MTCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();
  
  TF_MSG("\neMBMS - MCCH/MTCH setup Test Ends \n");
  lte_rrc_utf_reset(); 

}

void LteRrcLlcEMBMSPMCHOrderingTest::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcEMBMSPMCHLCReconfigTest);

void LteRrcLlcEMBMSPMCHLCReconfigTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_srs_get_serv_cell_sib;
}

void LteRrcLlcEMBMSPMCHLCReconfigTest::Test()
{
  lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg(); 

  /* Send LTE_RRC_CFG_REQI to LLC for MCCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_setup();

  /* Verify MCCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  /* Send LTE_RRC_CFG_REQI to LLC for MTCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mtch_setup();

  /* Verify MCCH/MTCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mrb_reconfig();

  /* Verify MCCH/MTCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcEMBMSPMCHLCReconfigTest::Teardown()
{
  lte_rrc_utf_teardown();
}



