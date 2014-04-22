/*!
  @file lte_rrc_meas_test_idle_all_rat_meas_config.cpp

  @brief
  Unit test code for testing idle meas config parameters for all RATs.

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

$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/meas/
lte_rrc_meas_test_idle_irat_gsm.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/17/13   md     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "lte_static_rrc_mem_util.h"
#include "__lte_rrc_meas.h"
#include "lte_rrc_measi.h"
#include "lte_rrc_meas.h"
#include "lte_rrc_osys_asn1util.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_meas_test_msg.h"
#include <TestFramework.h>
#include <tf_stub.h>

#define STD_ZEROAT(p)  memset((p), 0, sizeof(*p))

extern "C"
{
  ////TF_PROTO_STUB(boolean,lte_rrc_csp_cap_tdscdma_is_supported,void);
   TF_PROTO_STUB(const lte_rrc_irat_to_tds_prev_acq_freq_s*, lte_rrc_irat_get_tds_freq_from_irat_db, void);
}


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_meas_test_idle_all_rat_meas_config.cpp
*/
DECLARE_LTE_RRC_MEAS_TEST_SIB3_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG_WITH_DIFF_TDS_PRIO;
DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG2;
DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG3;
DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG4;
DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG_WO_LTE;
DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG5;
DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG6;
DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG7;
DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG8;
DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG9;
DECLARE_LTE_RRC_MEAS_TEST_SIB5_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB4_MSG1;

extern "C"
{
  extern lte_rrc_meas_s lte_rrc_meas;
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasCmccNwWith1TDSMeasLayer);

void LteRrcMeasCmccNwWith1TDSMeasLayer::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  
}

void LteRrcMeasCmccNwWith1TDSMeasLayer::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; STD_ZEROAT(&sib_updated_indi);
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; STD_ZEROAT(&cfg_cnf);
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  // Get Meas module to idle camped state
  lte_rrc_meas_test_get_meas_sm_to_idle_camped();

  //TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_meas_test_csp_tdscdma_is_supported;
  TF_STUB(lte_rrc_irat_get_tds_freq_from_irat_db) = lte_rrc_irat_to_tds_mgr_irat_acq_db_stub_1;
  
  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_si((void*) lte_rrc_meas_test_idle_cmcc_msg1,
                               sizeof(lte_rrc_meas_test_idle_cmcc_msg1));


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB6_BITMASK |
                                          LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 36);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 38);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 5);

  // Verify UTRA TDD CarrierFreq parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_ngbr_freq == 6);
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_active_list_ngbr_freq == 6);

  // Verify GSM reselection parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->t_reselection == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);

  // Verify GSM CarrierFreq group parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_frequency_groups == 11);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_freqs == 13);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasCmccNwWith1TDSMeasLayer::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasCmccNwWithDifferentTDSPriorities);

void LteRrcMeasCmccNwWithDifferentTDSPriorities::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  
}

void LteRrcMeasCmccNwWithDifferentTDSPriorities::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; STD_ZEROAT(&sib_updated_indi);
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; STD_ZEROAT(&cfg_cnf);
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  // Get Meas module to idle camped state
  lte_rrc_meas_test_get_meas_sm_to_idle_camped();

 //// TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_meas_test_csp_tdscdma_is_supported;
  TF_STUB(lte_rrc_irat_get_tds_freq_from_irat_db) = lte_rrc_irat_to_tds_mgr_irat_acq_db_stub_2;
  
  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_si((void*) lte_rrc_meas_test_idle_cmcc_msg_with_different_tds_prio,
                               sizeof(lte_rrc_meas_test_idle_cmcc_msg_with_different_tds_prio));


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB6_BITMASK |
                                          LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 36);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 38);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 5);

  // Verify UTRA TDD CarrierFreq parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_ngbr_freq == 6);
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_active_list_ngbr_freq == 6);

  // Verify GSM reselection parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->t_reselection == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);

  // Verify GSM CarrierFreq group parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_frequency_groups == 11);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_freqs == 13);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasCmccNwWithDifferentTDSPriorities::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasCmccMultipleSibUpdatedIndi);

void LteRrcMeasCmccMultipleSibUpdatedIndi::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  
}

void LteRrcMeasCmccMultipleSibUpdatedIndi::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; STD_ZEROAT(&sib_updated_indi);
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; STD_ZEROAT(&cfg_cnf);
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  // Get Meas module to idle camped state
  lte_rrc_meas_test_get_meas_sm_to_idle_camped();

  //TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_meas_test_csp_tdscdma_is_supported;
  
  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA)); 

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA)); 
 
  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  lte_rrc_meas_test_decode_si((void*) lte_rrc_meas_test_idle_cmcc_msg_wo_lte,
                               sizeof(lte_rrc_meas_test_idle_cmcc_msg_wo_lte));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask =
                                          LTE_RRC_SIB6_BITMASK |
                                          LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == ((LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA))); 
 
  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

}

void LteRrcMeasCmccMultipleSibUpdatedIndi::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasCmccMultipleSibUpdatedIndi1);

void LteRrcMeasCmccMultipleSibUpdatedIndi1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  
}

void LteRrcMeasCmccMultipleSibUpdatedIndi1::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; STD_ZEROAT(&sib_updated_indi);
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; STD_ZEROAT(&cfg_cnf);
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  // Get Meas module to idle camped state
  lte_rrc_meas_test_get_meas_sm_to_idle_camped();

 // TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_meas_test_csp_tdscdma_is_supported;
  
  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA)); 

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  

  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  lte_rrc_meas_test_decode_si((void*) lte_rrc_meas_test_idle_cmcc_msg_wo_lte,
                               sizeof(lte_rrc_meas_test_idle_cmcc_msg_wo_lte));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB5_BITMASK|
                                                               LTE_RRC_SIB6_BITMASK |
                                                               LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM|
                                                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                                                      LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA)); 
 
  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

}

void LteRrcMeasCmccMultipleSibUpdatedIndi1::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasCmccNwWithGsmFreqGroupsGettingPruned);

void LteRrcMeasCmccNwWithGsmFreqGroupsGettingPruned::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  
}

void LteRrcMeasCmccNwWithGsmFreqGroupsGettingPruned::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; STD_ZEROAT(&sib_updated_indi);
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; STD_ZEROAT(&cfg_cnf);
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  // Get Meas module to idle camped state
  lte_rrc_meas_test_get_meas_sm_to_idle_camped();

 // TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_meas_test_csp_tdscdma_is_supported;
  
  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_si((void*) lte_rrc_meas_test_idle_cmcc_msg2,
                               sizeof(lte_rrc_meas_test_idle_cmcc_msg2));


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB6_BITMASK |
                                          LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 36);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 38);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 5);

  // Verify UTRA TDD CarrierFreq parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_active_list_ngbr_freq == 6);
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_ngbr_freq == 6);

  // Verify GSM reselection parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->t_reselection == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);

  // Verify GSM CarrierFreq group parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_frequency_groups == 11);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_freqs == 13);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasCmccNwWithGsmFreqGroupsGettingPruned::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasCmccNwWithCDMALayers);

void LteRrcMeasCmccNwWithCDMALayers::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  
}

void LteRrcMeasCmccNwWithCDMALayers::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; STD_ZEROAT(&sib_updated_indi);
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; STD_ZEROAT(&cfg_cnf);
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  // Get Meas module to idle camped state
  lte_rrc_meas_test_get_meas_sm_to_idle_camped();

 // TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_meas_test_csp_tdscdma_is_supported;
  
  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_si((void*) lte_rrc_meas_test_idle_cmcc_msg3,
                               sizeof(lte_rrc_meas_test_idle_cmcc_msg3));


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB6_BITMASK |
                                          LTE_RRC_SIB7_BITMASK |
                                          LTE_RRC_SIB8_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK |
                                            LTE_RRC_SIB8_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 36);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 38);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);

  // Verify UTRA TDD CarrierFreq parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_active_list_ngbr_freq == 6);
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_ngbr_freq == 6);

  // Verify CDMA CarrierFreq parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list_size == 4);

  // Verify GSM reselection parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->t_reselection == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);

  // Verify GSM CarrierFreq group parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_frequency_groups == 11);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_freqs == 13);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasCmccNwWithCDMALayers::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasCmccNwWith4TDSMeasLayers);

void LteRrcMeasCmccNwWith4TDSMeasLayers::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  
}

void LteRrcMeasCmccNwWith4TDSMeasLayers::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; STD_ZEROAT(&sib_updated_indi);
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; STD_ZEROAT(&cfg_cnf);
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  // Get Meas module to idle camped state
  lte_rrc_meas_test_get_meas_sm_to_idle_camped();

 // TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_meas_test_csp_tdscdma_is_supported;
  
  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_si((void*) lte_rrc_meas_test_idle_cmcc_msg4,
                               sizeof(lte_rrc_meas_test_idle_cmcc_msg4));


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB6_BITMASK |
                                          LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 36);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 38);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 2);

  // Verify UTRA TDD CarrierFreq parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_active_list_ngbr_freq == 6);
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_ngbr_freq == 16);

  
  // Verify GSM reselection parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->t_reselection == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);

  // Verify GSM CarrierFreq group parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_frequency_groups == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_freqs == 4);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasCmccNwWith4TDSMeasLayers::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasCmccNwWith5TDSMeasLayers);

void LteRrcMeasCmccNwWith5TDSMeasLayers::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  
}

void LteRrcMeasCmccNwWith5TDSMeasLayers::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; STD_ZEROAT(&sib_updated_indi);
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; STD_ZEROAT(&cfg_cnf);
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  // Get Meas module to idle camped state
  lte_rrc_meas_test_get_meas_sm_to_idle_camped();

 // TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_meas_test_csp_tdscdma_is_supported;
  
  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_si((void*) lte_rrc_meas_test_idle_cmcc_msg5,
                               sizeof(lte_rrc_meas_test_idle_cmcc_msg5));


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB6_BITMASK |
                                          LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 36);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 38);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);

  // Verify UTRA TDD CarrierFreq parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_active_list_ngbr_freq == 6);
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_ngbr_freq == 16);

  
  // Verify GSM reselection parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->t_reselection == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);

  // Verify GSM CarrierFreq group parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_frequency_groups == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_freqs == 4);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasCmccNwWith5TDSMeasLayers::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasCmccNwWith2TDSFreqsLayers);

void LteRrcMeasCmccNwWith2TDSFreqsLayers::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  
}

void LteRrcMeasCmccNwWith2TDSFreqsLayers::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; STD_ZEROAT(&sib_updated_indi);
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; STD_ZEROAT(&cfg_cnf);
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  // Get Meas module to idle camped state
  lte_rrc_meas_test_get_meas_sm_to_idle_camped();

 // TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_meas_test_csp_tdscdma_is_supported;
  
  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_si((void*) lte_rrc_meas_test_idle_cmcc_msg6,
                               sizeof(lte_rrc_meas_test_idle_cmcc_msg6));


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB6_BITMASK |
                                          LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 36);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 38);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);

  // Verify UTRA TDD CarrierFreq parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_active_list_ngbr_freq == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_ngbr_freq == 2);

  
  // Verify GSM reselection parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->t_reselection == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);

  // Verify GSM CarrierFreq group parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_frequency_groups == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_freqs == 4);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasCmccNwWith2TDSFreqsLayers::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasCmccNwWith8TDSFreqsLayers);

void LteRrcMeasCmccNwWith8TDSFreqsLayers::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  
}

void LteRrcMeasCmccNwWith8TDSFreqsLayers::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; STD_ZEROAT(&sib_updated_indi);
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; STD_ZEROAT(&cfg_cnf);
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  // Get Meas module to idle camped state
  lte_rrc_meas_test_get_meas_sm_to_idle_camped();

 // TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_meas_test_csp_tdscdma_is_supported;
  
  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_si((void*) lte_rrc_meas_test_idle_cmcc_msg7,
                               sizeof(lte_rrc_meas_test_idle_cmcc_msg7));


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB6_BITMASK |
                                          LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 36);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 38);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 3);

  // Verify UTRA TDD CarrierFreq parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_active_list_ngbr_freq == 6);
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_ngbr_freq == 8);

  
  // Verify GSM reselection parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->t_reselection == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);

  // Verify GSM CarrierFreq group parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_frequency_groups == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_freqs == 4);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasCmccNwWith8TDSFreqsLayers::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasCmccNwWithLteGettingPruned);

void LteRrcMeasCmccNwWithLteGettingPruned::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  
}

void LteRrcMeasCmccNwWithLteGettingPruned::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; STD_ZEROAT(&sib_updated_indi);
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; STD_ZEROAT(&cfg_cnf);
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  // Get Meas module to idle camped state
  lte_rrc_meas_test_get_meas_sm_to_idle_camped();

 // TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_meas_test_csp_tdscdma_is_supported;
  
  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_si((void*) lte_rrc_meas_test_idle_cmcc_msg8,
                               sizeof(lte_rrc_meas_test_idle_cmcc_msg8));


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB6_BITMASK |
                                          LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 36);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 38);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);

  // Verify UTRA TDD CarrierFreq parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_active_list_ngbr_freq == 6);
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_ngbr_freq == 6);
  
  // Verify GSM reselection parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->t_reselection == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);

  // Verify GSM CarrierFreq group parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_frequency_groups == 15);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_freqs == 17);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasCmccNwWithLteGettingPruned::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasCmccNwWithRandomRatPriority);

void LteRrcMeasCmccNwWithRandomRatPriority::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  
}

void LteRrcMeasCmccNwWithRandomRatPriority::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; STD_ZEROAT(&sib_updated_indi);
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; STD_ZEROAT(&cfg_cnf);
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  // Get Meas module to idle camped state
  lte_rrc_meas_test_get_meas_sm_to_idle_camped();

 // TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_meas_test_csp_tdscdma_is_supported;
  
  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_si((void*) lte_rrc_meas_test_idle_cmcc_msg9,
                               sizeof(lte_rrc_meas_test_idle_cmcc_msg9));


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB6_BITMASK |
                                          LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 36);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 38);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 3);

  // Verify UTRA TDD CarrierFreq parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_active_list_ngbr_freq == 6);
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_ngbr_freq == 9);

  
  // Verify GSM reselection parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->t_reselection == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);

  // Verify GSM CarrierFreq group parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_frequency_groups == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_freqs == 4);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasCmccNwWithRandomRatPriority::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasCmccNwWithBackToBackPruning);

void LteRrcMeasCmccNwWithBackToBackPruning::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  
}

void LteRrcMeasCmccNwWithBackToBackPruning::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; STD_ZEROAT(&sib_updated_indi);
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; STD_ZEROAT(&cfg_cnf);
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  // Get Meas module to idle camped state
  lte_rrc_meas_test_get_meas_sm_to_idle_camped();

 // TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_meas_test_csp_tdscdma_is_supported;
  
  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_si((void*) lte_rrc_meas_test_idle_cmcc_msg5,
                               sizeof(lte_rrc_meas_test_idle_cmcc_msg5));


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB6_BITMASK |
                                          LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 36);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 38);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);

  // Verify UTRA TDD CarrierFreq parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_active_list_ngbr_freq == 6);
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_ngbr_freq == 16);
  
  // Verify GSM reselection parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->t_reselection == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);

  // Verify GSM CarrierFreq group parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_frequency_groups == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_freqs == 4);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_si((void*) lte_rrc_meas_test_idle_cmcc_msg2,
                               sizeof(lte_rrc_meas_test_idle_cmcc_msg2));


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB6_BITMASK |
                                          LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 36);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 38);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == 3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 5);

  // Verify UTRA TDD CarrierFreq parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_active_list_ngbr_freq == 6);
  TF_ASSERT(cfg_req->meas_cfg.irat_tdscdma_info_ptr->num_ngbr_freq == 6);

  // Verify GSM reselection parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->t_reselection == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);

  // Verify GSM CarrierFreq group parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_frequency_groups == 11);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_freqs == 13);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  
}

void LteRrcMeasCmccNwWithBackToBackPruning::Teardown()
{
  lte_rrc_utf_teardown();
}


