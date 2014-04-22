/*!
  @file lte_rrc_llc_eICIC_CSI_SubframePattern_test.cpp

  @brief
  Unit test code for checking eICIC RRC-ML1 interface.

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
lte_rrc_llc_conn_setup_reconfig_phy_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/13   md     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_sib.h"
#include "lte_rrc_llci.h"
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
  TF_PROTO_STUB(lte_rrc_div_duplex_e, lte_rrc_csp_get_last_camped_mode, void);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
TF_PROTO_STUB(boolean, lte_rrc_cell_is_tdd, lte_earfcn_t);
TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s*);
TF_PROTO_STUB(void *, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
}

extern "C"
{
extern lte_rrc_llc_s lte_rrc_llc_data;
}


/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlceICICCSISubframePatternTest);

void LteRrcLlceICICCSISubframePatternTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlceICICCSISubframePatternTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_errno_e status;
  lte_rrc_osys_CQI_ReportConfig_r10 *cqi_report_cfg_ptr = NULL;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_llc_s *llc_ptr;
  int decode_status = 0;
  int i = 0;

  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

  encoded_pdu.length = sizeof(llc_eicic_set_release_msg);
  
  encoded_pdu.value = llc_eicic_set_release_msg;

  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                   (encoded_pdu.value,encoded_pdu.length,
                                                    DL_DCCH_Message, &decode_status);

  cqi_report_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                          criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                          radioResourceConfigDedicated.physicalConfigDedicated.
                          cqi_ReportConfig_r10;
  
  /*This test eICIC CSF interface for release message*/
  status = lte_rrc_llc_process_cqi_reporting_rel10(llc_ptr, &llc_ptr->dd_ptr->oc_ptr->cqi_cfg, 
                                                   cqi_report_cfg_ptr);

  TF_ASSERT(status == E_SUCCESS);

  //Verify eICIC data
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.
              csi_subfm_pattern_cfg.enable == FALSE);

  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

  encoded_pdu.length = sizeof(llc_eicic_set_FDD_msg);
  
  encoded_pdu.value = llc_eicic_set_FDD_msg;

  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                    (encoded_pdu.value,encoded_pdu.length,
                                                     DL_DCCH_Message, &decode_status);

  cqi_report_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                          criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                          radioResourceConfigDedicated.physicalConfigDedicated.
                          cqi_ReportConfig_r10;
  
  /*This test eICIC CSF interface for FDD*/
  status = lte_rrc_llc_process_cqi_reporting_rel10(llc_ptr, &llc_ptr->dd_ptr->oc_ptr->cqi_cfg, 
                                                   cqi_report_cfg_ptr);

  TF_ASSERT(status == E_DATA_INVALID);  

  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;

  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

  encoded_pdu.length = sizeof(llc_eicic_set_TDD_msg);
  
  encoded_pdu.value = llc_eicic_set_TDD_msg;

  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                    (encoded_pdu.value,encoded_pdu.length,
                                                     DL_DCCH_Message, &decode_status);

  cqi_report_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                          criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                          radioResourceConfigDedicated.physicalConfigDedicated.
                          cqi_ReportConfig_r10;
  
  /*This test eICIC CSF interface for TDD*/
  status = lte_rrc_llc_process_cqi_reporting_rel10(llc_ptr, &llc_ptr->dd_ptr->oc_ptr->cqi_cfg, 
                                                   cqi_report_cfg_ptr);

  TF_ASSERT(status == E_DATA_INVALID);

  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

  encoded_pdu.length = sizeof(llc_eicic_set_TDD_msg1);
  
  encoded_pdu.value = llc_eicic_set_TDD_msg1;

  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                    (encoded_pdu.value,encoded_pdu.length,
                                                     DL_DCCH_Message, &decode_status);

  cqi_report_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                          criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                          radioResourceConfigDedicated.physicalConfigDedicated.
                          cqi_ReportConfig_r10;
  
  llc_ptr->dd_ptr->oc_ptr->tdd_cfg.subframe_assign = LTE_L1_TDD_UL_DL_CFG_1;
  
  /*This test eICIC CSF interface for TDD*/
  status = lte_rrc_llc_process_cqi_reporting_rel10(llc_ptr, &llc_ptr->dd_ptr->oc_ptr->cqi_cfg, 
                                                   cqi_report_cfg_ptr);

  TF_ASSERT(status == E_SUCCESS);

  //Verify eICIC data
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param_present  == TRUE);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.
              csi_subfm_pattern_cfg.enable == TRUE);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
              csi_meas_subfm_set1.type == LTE_L1_MEAS_SUBFM_PATTERN_TDD);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
              csi_meas_subfm_set1.subfm_num_bits == 
            LTE_RRC_SUBFRAME_PATTERN_TDD_SUBFRAME_CONFIG_1_5_NUMBITS);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
              csi_meas_subfm_set1.subfm_pattern[0] == 0x0a000000);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
              csi_meas_subfm_set2.type == LTE_L1_MEAS_SUBFM_PATTERN_TDD);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
              csi_meas_subfm_set2.subfm_num_bits == 
            LTE_RRC_SUBFRAME_PATTERN_TDD_SUBFRAME_CONFIG_1_5_NUMBITS);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
              csi_meas_subfm_set2.subfm_pattern[0] == 0x01000000);

  for (i = 0; i < 20; i++)
  {
    llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

    encoded_pdu.length = sizeof(llc_eicic_set_release_msg);
  
    encoded_pdu.value = llc_eicic_set_release_msg;

    dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                      (encoded_pdu.value,encoded_pdu.length,
                                                       DL_DCCH_Message, &decode_status);

    cqi_report_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                          criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                          radioResourceConfigDedicated.physicalConfigDedicated.
                          cqi_ReportConfig_r10;
  
    /*This test eICIC CSF interface for release message*/
    status = lte_rrc_llc_process_cqi_reporting_rel10(llc_ptr, &llc_ptr->dd_ptr->oc_ptr->cqi_cfg, 
                                                     cqi_report_cfg_ptr);

    TF_ASSERT(status == E_SUCCESS);

    //Verify eICIC data
    TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.
                csi_subfm_pattern_cfg.enable == FALSE);

    if ((i % 2) == 0)
    {

      TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;
      
      llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

      encoded_pdu.length = sizeof(llc_eicic_set_FDD_msg);
  
      encoded_pdu.value = llc_eicic_set_FDD_msg;

      dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                        (encoded_pdu.value,encoded_pdu.length,
                                                         DL_DCCH_Message, &decode_status);

      cqi_report_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                              criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                              radioResourceConfigDedicated.physicalConfigDedicated.
                              cqi_ReportConfig_r10;

      /*This test eICIC CSF interface for FDD*/
      status = lte_rrc_llc_process_cqi_reporting_rel10(llc_ptr, &llc_ptr->dd_ptr->oc_ptr->cqi_cfg, 
                                                       cqi_report_cfg_ptr);

      TF_ASSERT(status == E_DATA_INVALID);
      
      
    }

    else
    {

      TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;
      
      llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

      encoded_pdu.length = sizeof(llc_eicic_set_TDD_msg1);
  
      encoded_pdu.value = llc_eicic_set_TDD_msg1;

      dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                        (encoded_pdu.value,encoded_pdu.length,
                                                         DL_DCCH_Message, &decode_status);

      cqi_report_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                              criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                              radioResourceConfigDedicated.physicalConfigDedicated.
                              cqi_ReportConfig_r10;

      llc_ptr->dd_ptr->oc_ptr->tdd_cfg.subframe_assign = LTE_L1_TDD_UL_DL_CFG_5;

      /*This test eICIC CSF interface for TDD*/
      status = lte_rrc_llc_process_cqi_reporting_rel10(llc_ptr, &llc_ptr->dd_ptr->oc_ptr->cqi_cfg, 
                                                       cqi_report_cfg_ptr);

      TF_ASSERT(status == E_SUCCESS);

      //Verify eICIC data
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param_present  == TRUE);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.
                  csi_subfm_pattern_cfg.enable == TRUE);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
                  csi_meas_subfm_set1.type == LTE_L1_MEAS_SUBFM_PATTERN_TDD);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
                  csi_meas_subfm_set1.subfm_num_bits == 
                LTE_RRC_SUBFRAME_PATTERN_TDD_SUBFRAME_CONFIG_1_5_NUMBITS);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
                  csi_meas_subfm_set1.subfm_pattern[0] == 0x0a000000);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
                  csi_meas_subfm_set2.type == LTE_L1_MEAS_SUBFM_PATTERN_TDD);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
                  csi_meas_subfm_set2.subfm_num_bits == 
                LTE_RRC_SUBFRAME_PATTERN_TDD_SUBFRAME_CONFIG_1_5_NUMBITS);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
                  csi_meas_subfm_set2.subfm_pattern[0] == 0x01000000);
      
    }

  }
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

    encoded_pdu.length = sizeof(llc_eicic_zero_subframe_msg);

    encoded_pdu.value = llc_eicic_zero_subframe_msg;

    dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                      (encoded_pdu.value,encoded_pdu.length,
                                                       DL_DCCH_Message, &decode_status);

    cqi_report_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                            criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                            radioResourceConfigDedicated.physicalConfigDedicated.
                            cqi_ReportConfig_r10;

    llc_ptr->dd_ptr->oc_ptr->tdd_cfg.subframe_assign = LTE_L1_TDD_UL_DL_CFG_3;

    /*This test eICIC CSF interface for TDD*/
    status = lte_rrc_llc_process_cqi_reporting_rel10(llc_ptr, &llc_ptr->dd_ptr->oc_ptr->cqi_cfg, 
                                                     cqi_report_cfg_ptr);

    TF_ASSERT(status == E_SUCCESS);

    //Verify eICIC data
    TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param_present  == TRUE);
    TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.
                csi_subfm_pattern_cfg.enable == TRUE);
    TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
                csi_meas_subfm_set1.type == LTE_L1_MEAS_SUBFM_PATTERN_TDD);
    TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
                csi_meas_subfm_set1.subfm_num_bits == 
              LTE_RRC_SUBFRAME_PATTERN_TDD_SUBFRAME_CONFIG_1_5_NUMBITS);
    TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
                csi_meas_subfm_set1.subfm_pattern[0] == 0);
    TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
                csi_meas_subfm_set2.type == LTE_L1_MEAS_SUBFM_PATTERN_TDD);
    TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
                csi_meas_subfm_set2.subfm_num_bits == 
              LTE_RRC_SUBFRAME_PATTERN_TDD_SUBFRAME_CONFIG_1_5_NUMBITS);
    TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.
                csi_meas_subfm_set2.subfm_pattern[0] == 0);
  
  lte_rrc_utf_reset(); 
  
}

void LteRrcLlceICICCSISubframePatternTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlceICICCSISubframePatternRelTest);

void LteRrcLlceICICCSISubframePatternRelTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params;
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
}

void LteRrcLlceICICCSISubframePatternRelTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
  lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
  uint32 buf_len;
  
  lte_rrc_llc_test_basic_sib2_update();

  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband();

  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;

  encoded_pdu.length = sizeof(llc_eicic_csi_setup);
  encoded_pdu.value = llc_eicic_csi_setup;
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);
  TF_MSG("\nLLC: Rcvd CPHY_DEDICATED_CFG_REQ \n");

  /* After CPHY Config req is sent, assert LLC is in WAIT_FOR_CPHY_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  TF_MSG("\nCPHY_DEDICATED_CFG_REQ rcvd and LLC Module in WAIT_FOR_CPHY_CFG_CNF state\n");

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  /* Setting the fields in the LTE_CPHY_DEDICATED_CFG_CNF sent by CPHY, The bits
    set in dedicated_cfg_validity field reflect the channel config that were set
    in the llc_conn_setup_msg in the lte_rrc_llc_ota_msg.h */
  cphy_dedicated_cfg_cnf.dedicated_cfg_validity = cphy_dedicated_cfg_req_ptr->dedicated_cfg_param.cfg_mask;
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->cqi_cfg.cqi_rel10_param_present ==  TRUE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->cqi_cfg.cqi_rel10_param.cqi_periodic_cfg.enable==  TRUE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->cqi_cfg.cqi_rel10_param.cqi_aperiodic_cfg.enable==  TRUE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.enable==  TRUE);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  encoded_pdu.length = sizeof(llc_eicic_csi_rel);
  encoded_pdu.value = llc_eicic_csi_rel;
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);
  TF_MSG("\nLLC: Rcvd CPHY_DEDICATED_CFG_REQ \n");

  /* After CPHY Config req is sent, assert LLC is in WAIT_FOR_CPHY_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  TF_MSG("\nCPHY_DEDICATED_CFG_REQ rcvd and LLC Module in WAIT_FOR_CPHY_CFG_CNF state\n");

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  /* Setting the fields in the LTE_CPHY_DEDICATED_CFG_CNF sent by CPHY, The bits
    set in dedicated_cfg_validity field reflect the channel config that were set
    in the llc_conn_setup_msg in the lte_rrc_llc_ota_msg.h */
  cphy_dedicated_cfg_cnf.dedicated_cfg_validity = cphy_dedicated_cfg_req_ptr->dedicated_cfg_param.cfg_mask;
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->cqi_cfg.cqi_rel10_param_present ==  TRUE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->cqi_cfg.cqi_reporting_periodic.enable==  FALSE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->cqi_cfg.cqi_rel10_param.cqi_aperiodic_cfg.enable==  FALSE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->cqi_cfg.cqi_rel10_param.csi_subfm_pattern_cfg.enable==  FALSE);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

}

void LteRrcLlceICICCSISubframePatternRelTest::Teardown()
{
  lte_rrc_utf_teardown();
}


