/*!
  @file lte_rrc_llc_scell_related_test.cpp

  @brief
  Unit test code for the LLC procedure that tests Scell related configs

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
6/4/14   md     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_rrc_csp.h"
#include "lte_rrc_sib.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h>  

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

extern "C"
{
extern lte_rrc_llc_s lte_rrc_llc_data;
}

extern "C"
{
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);

#ifdef FEATURE_LTE_CA
	TF_PROTO_STUB(boolean, lte_rrc_cap_is_band_combo_valid, sys_sband_lte_e_type, uint8 , 
    const sys_sband_lte_e_type *, const lte_bandwidth_e *, const uint8 *, boolean);
#endif
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

/*This test verifies the following RRC bug
1. Reconfig msg 1 with PCC Aperiodic CQI R10 containing Trigger group
2. Reconfig msg 2 with SCC Aperiodic CQI R10.  At this point RRC is disabling PCC aperiodic CQI R10 config containing trigger group, which is wrong (SCC AP CQI reporting is pruned at this point)
3. Reconfig msg 3 with PCC periodic CQI R10 (SCC AP CQI reporting is pruned at this point as well)
*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPccCqiAPTriggerGrpTest);

void LteRrcLlcPccCqiAPTriggerGrpTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

#ifdef FEATURE_LTE_CA
  TF_STUB(lte_rrc_cap_is_band_combo_valid) = lte_rrc_llc_test_get_band_combo_bitmask_FF;
#endif

  //TF_STUB(lte_rrc_cap_is_ca_bandwidth_combo_valid) = lte_rrc_llc_test_cap_is_ca_bandwidth_combo_valid_true;
}

void LteRrcLlcPccCqiAPTriggerGrpTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update(); 
  
  encoded_pdu.length = sizeof(llc_conn_setup_with_csi_ies);
  encoded_pdu.value = llc_conn_setup_with_csi_ies;
  lte_rrc_llc_test_rrc_connection_setup_for_tm9(encoded_pdu);

  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();

  // Reconfig 1
  encoded_pdu.length = sizeof(llc_rcfg_with_pcc_ap_cqi);
  encoded_pdu.value = llc_rcfg_with_pcc_ap_cqi;
  // Verify Reconfig 1
  lte_rrc_llc_test_scc_cqi_reporting(encoded_pdu);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->cqi_cfg.cqi_rel10_param.cqi_aperiodic_cfg.enable == TRUE);

  // Reconfig 2
  encoded_pdu.length = sizeof(llc_rcfg_with_scc_ap_cqi);
  encoded_pdu.value = llc_rcfg_with_scc_ap_cqi;
  // Verify Reconfig 2
  lte_rrc_llc_test_scc_cqi_reporting(encoded_pdu);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->cqi_cfg.cqi_rel10_param.cqi_aperiodic_cfg.enable == TRUE);
  
  // Reconfig 3
  encoded_pdu.length = sizeof(llc_rcfg_with_pcc_prd_cqi);
  encoded_pdu.value = llc_rcfg_with_pcc_prd_cqi;
  // Verify Reconfig 3
  lte_rrc_llc_test_scc_cqi_reporting(encoded_pdu);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->cqi_cfg.cqi_rel10_param.cqi_aperiodic_cfg.enable == TRUE);
  
  lte_rrc_utf_reset();
}

void LteRrcLlcPccCqiAPTriggerGrpTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*This test verifies the following
1. Reconfig msg 1 with SCC and PUCCH R10
2. Reconfig msg 2 with SCC released and PUCCH R10 released
3. Reconfig msg 3 with SCC and PUCCH R10
*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPucchCfgSetupAndRelTest);

void LteRrcLlcPucchCfgSetupAndRelTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

#ifdef FEATURE_LTE_CA
  TF_STUB(lte_rrc_cap_is_band_combo_valid) = lte_rrc_llc_test_get_band_combo_bitmask_FF;
#endif
}

void LteRrcLlcPucchCfgSetupAndRelTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update(); 
  
  encoded_pdu.length = sizeof(llc_conn_setup_with_csi_ies);
  encoded_pdu.value = llc_conn_setup_with_csi_ies;
  lte_rrc_llc_test_rrc_connection_setup_for_tm9(encoded_pdu);

  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();

  TF_MSG("\nRcfg 1\n");

  // Reconfig 1
  encoded_pdu.length = sizeof(llc_rcfg_with_scc_ap_cqi);
  encoded_pdu.value = llc_rcfg_with_scc_ap_cqi;
  // Verify Reconfig 1
  lte_rrc_llc_test_scc_cqi_reporting(encoded_pdu);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->pucch_cfg_dedicated.rel10_pucch_cfg.pucch_rel10_format_cfg.pucch_rel10_cs_cfg.n1_pucch_an_cs_list[0].cs_list[0] == 1);

  TF_MSG("\nRcfg 2\n");
  // Reconfig 2
  encoded_pdu.length = sizeof(llc_rcfg_with_scc_rel);
  encoded_pdu.value = llc_rcfg_with_scc_rel;
  // Verify Reconfig 2
  lte_rrc_llc_test_scc_cqi_reporting(encoded_pdu);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->pucch_cfg_dedicated.rel10_pucch_cfg.pucch_rel10_format_cfg.pucch_rel10_cs_cfg.enable == FALSE);

  TF_MSG("\nRcfg 3\n");
  
  // Reconfig 3
  encoded_pdu.length = sizeof(llc_rcfg_with_scc_added_back);
  encoded_pdu.value = llc_rcfg_with_scc_added_back;
  // Verify Reconfig 3
  lte_rrc_llc_test_scc_cqi_reporting(encoded_pdu);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->pucch_cfg_dedicated.rel10_pucch_cfg_valid == TRUE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->pucch_cfg_dedicated.rel10_pucch_cfg.pucch_rel10_format_cfg.pucch_rel10_cs_cfg.n1_pucch_an_cs_list[0].cs_list[0] == 6);
  
  lte_rrc_utf_reset();
}

void LteRrcLlcPucchCfgSetupAndRelTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidScellCqiBwTest);

void LteRrcLlcInvalidScellCqiBwTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

#ifdef FEATURE_LTE_CA
  TF_STUB(lte_rrc_cap_is_band_combo_valid) = lte_rrc_llc_test_get_band_combo_bitmask_FF;
#endif
}

void LteRrcLlcInvalidScellCqiBwTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update(); 
  
  encoded_pdu.length = sizeof(llc_conn_setup_with_csi_ies);
  encoded_pdu.value = llc_conn_setup_with_csi_ies;
  lte_rrc_llc_test_rrc_connection_setup_for_tm9(encoded_pdu);

  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();

  TF_MSG("\nRcfg 1\n");

  // Reconfig 1
  encoded_pdu.length = sizeof(llc_rcfg_with_invalid_bw_scc_ap_cqi);
  encoded_pdu.value = llc_rcfg_with_invalid_bw_scc_ap_cqi;
  // Verify Reconfig 1
  lte_rrc_llc_test_invalid_scc_cqi_reporting(encoded_pdu);
  
  lte_rrc_utf_reset();
}

void LteRrcLlcInvalidScellCqiBwTest::Teardown()
{
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcCsiCfgIdxEnabledForScc);

void LteRrcLlcCsiCfgIdxEnabledForScc::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

#ifdef FEATURE_LTE_CA
  TF_STUB(lte_rrc_cap_is_band_combo_valid) = lte_rrc_llc_test_get_band_combo_bitmask_FF;
#endif
}

void LteRrcLlcCsiCfgIdxEnabledForScc::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  int decode_status = 0;

  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
  lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
  uint32 buf_len;
  lte_mac_cfg_req_msg_s *mac_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  lte_rrc_llc_test_basic_sib2_update(); 
  
  encoded_pdu.length = sizeof(llc_conn_setup_with_csi_ies);
  encoded_pdu.value = llc_conn_setup_with_csi_ies;
  lte_rrc_llc_test_rrc_connection_setup_for_tm9(encoded_pdu);

  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();

  TF_MSG("\nRcfg 1\n");

  // Reconfig 1
  encoded_pdu.length = sizeof(llc_rcfg_with_scc_csi);
  encoded_pdu.value = llc_rcfg_with_scc_csi;
  
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(lte_rrc_llc_test_common_data.dl_dcch_msg_ptr != NULL);

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
  
  lte_rrc_utf_reset();
}

void LteRrcLlcCsiCfgIdxEnabledForScc::Teardown()
{
  lte_rrc_utf_teardown();
}



