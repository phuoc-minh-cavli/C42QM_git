/*!
  @file lte_rrc_llc_cqi_Aperiodic_with_invalid_bw_test.cpp

  @brief
  Unit test code for the LLC procedure that tests to validate CQI report Aperiodic CFG Indicator against BW.

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

$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/llc/lte_rrc_llc_cqi_Aperiodic_with_invalid_bw_test.cpp

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/13   md     Initial Revision
           
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
#include <tf_stub.h>  

extern "C"
{
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_camped_cell_bw, lte_bandwidth_e *);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcCQIAperiodicCfgWithInvalidBwTest);

void LteRrcLlcCQIAperiodicCfgWithInvalidBwTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcCQIAperiodicCfgWithInvalidBwTest::Test()
{

  lte_rrc_pdu_buf_s encoded_pdu;

  TF_MSG("\nLLC CQI config test begins \n");
  lte_rrc_llc_test_basic_sib2_update();

  /* Sending a Conn setup that configures periodic CQI */
  encoded_pdu.length = sizeof(llc_conn_setup_msg);
  encoded_pdu.value = llc_conn_setup_msg;

  lte_rrc_llc_test_rrc_connection_setup(encoded_pdu);

  /* Send Reconfig messages that doesn't include CQIConfig */
  lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_dl();
  lte_rrc_llc_test_reconfig_phy_mac_drb3_rlc();

  TF_STUB(lte_rrc_csp_get_camped_cell_bw) = lte_rrc_llc_test_csp_get_camped_cell_bw1;

  /* Send a Reconfig message triggering intra-freq HO */ 
  encoded_pdu.length = sizeof(llc_reconfig_intra_freq_mobility_info_only_msg);
  encoded_pdu.value = llc_reconfig_intra_freq_mobility_info_only_msg;
  lte_rrc_llc_test_intra_freq_mobility_only(encoded_pdu, TRUE);

  lte_rrc_llc_test_cqi_fmt_indi(FALSE);

  TF_MSG("\nLLC CQI config test ends \n");
  lte_rrc_utf_reset();

}

void LteRrcLlcCQIAperiodicCfgWithInvalidBwTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcCQIAperiodicCfgWithInvalidBwTestForScell);

void LteRrcLlcCQIAperiodicCfgWithInvalidBwTestForScell::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcCQIAperiodicCfgWithInvalidBwTestForScell::Test()
{

  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update(); 
  
  encoded_pdu.length = sizeof(llc_conn_setup_with_csi_ies);
  encoded_pdu.value = llc_conn_setup_with_csi_ies;
  lte_rrc_llc_test_rrc_connection_setup_for_tm9(encoded_pdu);

  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();

  TF_STUB(lte_rrc_csp_get_camped_cell_bw) = lte_rrc_llc_test_csp_get_camped_cell_bw1;
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

  encoded_pdu.length = sizeof(llc_scell_cqi_bw_test);
  encoded_pdu.value = llc_scell_cqi_bw_test;

  lte_rrc_llc_scell_cqi_bw_test(encoded_pdu);

  lte_rrc_utf_reset();

}

void LteRrcLlcCQIAperiodicCfgWithInvalidBwTestForScell::Teardown()
{
  lte_rrc_utf_teardown();
}


