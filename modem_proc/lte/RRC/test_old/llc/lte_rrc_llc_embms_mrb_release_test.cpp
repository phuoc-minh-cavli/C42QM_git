/*!
  @file lte_rrc_llc_embms_mrb_release_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of CFG_REQI for
  eMBMS to release previously configured MCCH and MTCH.

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
09/28/11   mm     Initial Revision
           
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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcEMBMSMRBReleaseTest);

void LteRrcLlcEMBMSMRBReleaseTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_srs_get_serv_cell_sib;
}

void LteRrcLlcEMBMSMRBReleaseTest::Test()
{
  boolean cfg_for_release = TRUE, release_all_mrb = FALSE, release_all_rb = FALSE;

  lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg(); 

  /* Send LTE_RRC_CFG_REQI to LLC for MCCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_setup();

  /* Verify MCCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  /* Send LTE_RRC_CFG_REQI to LLC for MTCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mtch_setup();

  /* Verify MCCH/MTCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  /* Verify MCCH/MTCH release handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_release(cfg_for_release, release_all_mrb, release_all_rb);

  lte_rrc_utf_reset(); 

}

void LteRrcLlcEMBMSMRBReleaseTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcEMBMSReleaseAllMRBTest);

void LteRrcLlcEMBMSReleaseAllMRBTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_srs_get_serv_cell_sib;
}

void LteRrcLlcEMBMSReleaseAllMRBTest::Test()
{
  boolean cfg_for_release = TRUE, release_all_mrb = TRUE, release_all_rb = FALSE;

  lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg(); 

  /* Send LTE_RRC_CFG_REQI to LLC for MCCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_setup();

  /* Verify MCCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  /* Send LTE_RRC_CFG_REQI to LLC for MTCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mtch_setup();

  /* Verify MCCH/MTCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  /* Verify eMBMS releaseAll handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_release(cfg_for_release, release_all_mrb, release_all_rb);

  lte_rrc_utf_reset(); 

}

void LteRrcLlcEMBMSReleaseAllMRBTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcEMBMSReleaseForLTEStopTest);

void LteRrcLlcEMBMSReleaseForLTEStopTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_srs_get_serv_cell_sib;
}

void LteRrcLlcEMBMSReleaseForLTEStopTest::Test()
{
  boolean cfg_for_release = TRUE, release_all_mrb = TRUE, release_all_rb = TRUE;

  lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg(); 

  /* Send LTE_RRC_CFG_REQI to LLC for MCCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_setup();

  /* Verify MCCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  /* Send LTE_RRC_CFG_REQI to LLC for MTCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mtch_setup();

  /* Verify MCCH/MTCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  /* Verify handling of MRBs/eMBMS config during LTE stop */
  lte_rrc_llc_test_embms_release_for_lte_stop(cfg_for_release, release_all_mrb, release_all_rb);

  lte_rrc_utf_reset(); 

}

void LteRrcLlcEMBMSReleaseForLTEStopTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcUnicastAndMulticastTest);

void LteRrcLlcUnicastAndMulticastTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_srs_get_serv_cell_sib;
}

void LteRrcLlcUnicastAndMulticastTest::Test()
{
  boolean cfg_for_release = TRUE, release_all_mrb = TRUE, release_all_rb = FALSE;

  lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg(); 

  /* Send LTE_RRC_CFG_REQI to LLC for MCCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_setup();

  /* Verify MCCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  /* Send LTE_RRC_CFG_REQI to LLC for MTCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mtch_setup();

  /* Verify MCCH/MTCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  /* Setup unicast bearers, verify how multicast bearers get handled unicast bearers are added */
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  lte_rrc_llc_test_reconfig_drb3_setup_drb4_drb5_um_bi();

  /* Verify connection release / release of all unicast bearers */
  lte_rrc_llc_test_conn_release(TRUE);

  /* Reconfigure multicast bearers */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mrb_reconfig();

  /* Verify MCCH/MTCH reconfig handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  /* Setup Unicast bearers */
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  lte_rrc_llc_test_reconfig_drb3_setup_drb4_drb5_um_bi();

  /* Verify eMBMS releaseAll handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_release(cfg_for_release, release_all_mrb, release_all_rb);

  /* Verify connection release / release of all unicast bearers */
  lte_rrc_llc_test_conn_release(TRUE);

  /* Resetup Multicast bearers */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_setup();

  /* Verify MCCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  /* Send LTE_RRC_CFG_REQI to LLC for MTCH setup */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mtch_setup();

  /* Verify MCCH/MTCH setup handling in LLC */
  lte_rrc_llc_test_embms_mcch_mtch_config();

  release_all_rb = TRUE;
  
  /* Verify handling of MRBs/eMBMS config during LTE stop */
  lte_rrc_llc_test_embms_release_for_lte_stop(cfg_for_release, release_all_mrb, release_all_rb);

  lte_rrc_utf_reset(); 

}

void LteRrcLlcUnicastAndMulticastTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcUnicastAndMulticastLoopTest);

void LteRrcLlcUnicastAndMulticastLoopTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_srs_get_serv_cell_sib;
}

void LteRrcLlcUnicastAndMulticastLoopTest::Test()
{
  boolean cfg_for_release = TRUE, release_all_mrb = TRUE, release_all_rb = FALSE;
  uint8 i = 0;

  for (i = 0; i < 50; i++)
  {
    release_all_rb = FALSE;

  lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg(); 

    /* Send LTE_RRC_CFG_REQI to LLC */
    lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_setup();

    /* Verify MCCH setup handling in LLC */
    lte_rrc_llc_test_embms_mcch_mtch_config();

    /* Send LTE_RRC_CFG_REQI to LLC */
    lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mtch_setup();

    /* Verify MTCH setup handling in LLC */
    lte_rrc_llc_test_embms_mcch_mtch_config();

    /* Setup unicast bearers, verify how multicast bearers get handled unicast bearers are added */
    lte_rrc_llc_test_rrc_connection_setup_default();
    lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
    lte_rrc_llc_test_reconfig_drb3_setup_drb4_drb5_um_bi();

    /* Verify connection release / release of all unicast bearers */
    lte_rrc_llc_test_conn_release(TRUE);

    /* Reconfigure multicast bearers */
    lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mrb_reconfig();

    /* Verify MCCH/MTCH reconfig handling in LLC */
    lte_rrc_llc_test_embms_mcch_mtch_config();

    /* Setup Unicast bearers */
    lte_rrc_llc_test_rrc_connection_setup_default();
    lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
    lte_rrc_llc_test_reconfig_drb3_setup_drb4_drb5_um_bi();

    /* Verify eMBMS releaseAll handling in LLC */
    lte_rrc_llc_test_embms_mcch_mtch_release(cfg_for_release, release_all_mrb, release_all_rb);

    /* Verify connection release / release of all unicast bearers */
    lte_rrc_llc_test_conn_release(TRUE);

    /* Resetup Multicast bearers */
    lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_setup();

    /* Verify MCCH setup handling in LLC */
    lte_rrc_llc_test_embms_mcch_mtch_config();

    /* Send LTE_RRC_CFG_REQI to LLC for MTCH setup */
    lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mtch_setup();

    /* Verify MCCH/MTCH setup handling in LLC */
    lte_rrc_llc_test_embms_mcch_mtch_config();

    release_all_rb = TRUE;
    
    /* Verify handling of MRBs/eMBMS config during LTE stop */
    lte_rrc_llc_test_embms_release_for_lte_stop(cfg_for_release, release_all_mrb, release_all_rb);
    
  }

  lte_rrc_utf_reset(); 

}

void LteRrcLlcUnicastAndMulticastLoopTest::Teardown()
{
  lte_rrc_utf_teardown();
}

