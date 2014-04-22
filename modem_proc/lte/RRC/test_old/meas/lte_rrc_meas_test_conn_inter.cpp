/*!
  @file lte_rrc_meas_test_conn_inter.cpp

  @brief
  Unit test code for the meas procedure.

*/

/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/meas/lte_rrc_meas_test_conn_inter.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/24/11   da     Initial Revision

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
#include "lte_rrc_meas.h"
#include "lte_rrc_measi.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_capabilities.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_meas_test_msg.h"
#include <TestFramework.h>
#include <tf_stub.h>
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_meas_test_conn_intra.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */

} lte_rrc_meas_test_conn_inter_s;

#define LTE_RRC_MEAS_TEST_THRESH_RSRP_MIN       0
#define LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX       97
#define LTE_RRC_MEAS_TEST_THRESH_RSRP_INVALID   98

#define LTE_RRC_MEAS_TEST_THRESH_RSRQ_MIN       0
#define LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX       34
#define LTE_RRC_MEAS_TEST_THRESH_RSRQ_INVALID   35

#define LTE_RRC_MEAS_TEST_N_CELL_CHANGE_MIN     1
#define LTE_RRC_MEAS_TEST_N_CELL_CHANGE_MAX     16

extern "C"
{

#ifdef FEATURE_LTE_CA
  TF_PROTO_STUB(lte_rrc_cell_type_e, lte_rrc_llc_check_freq_match_with_pcell_scell_freq, lte_earfcn_t, lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs *);
#endif
	TF_PROTO_STUB(lte_3gpp_release_ver_e,lte_rrc_get_3gpp_release_version, void);

#ifdef FEATURE_LTE_CA
	TF_PROTO_STUB(boolean,lte_rrc_is_rel_10_ca_enabled, void);	
#endif
	/*TF_PROTO_STUB(uint8, _lte_rrc_cap_inter_freq_gap_less_meas_enabled, sys_sband_lte_e_type,sys_sband_lte_e_type);*/
}

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_meas_test_conn_intra.cpp
*/
DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_IFREQ_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_NO_MEAS_CFG_DURING_HO_MSG;
DECLARE_LTE_RRC_MEAS_TEST_EMPTY_RECFG_MSG;
DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_MSG_GAPS;
DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_MSG_GAPS_HO;


extern "C"
{
  extern lte_rrc_meas_s lte_rrc_meas_data;
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                                FUNCTIONS

===========================================================================*/

static lte_3gpp_release_ver_e lte_rrc_meas_test_stub_get_3gpp_release_version_for_rel10
(
  void
)
{
  return LTE_3GPP_REL10 ;
}

static boolean lte_rrc_is_rel_10_ca_enabled_true
(
  void
)
{
  return TRUE ;
}


static uint8 lte_rrc_cap_inter_freq_gap_less_meas_enabled_true
(
  sys_sband_lte_e_type band1,
  sys_sband_lte_e_type band2
)
{
  return TRUE ;
}
static uint8 lte_rrc_cap_inter_freq_gap_less_meas_enabled_false
(
  sys_sband_lte_e_type band1,
  sys_sband_lte_e_type band2
)
{
  return FALSE ;
}


/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnInterFreqTest);

void LteRrcMeasConnInterFreqTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnInterFreqTest::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_cnf_s cphy_meas_cfg_cnf;
  uint32 buf_len;
  lte_rrc_meas_cfg_cnfi_s *meas_cfg_cnfi;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_ifreq_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_ifreq_msg1),
                                       &meas_cfg);

  //------------------------------------
  // Quantity config addition
  // -----------------------------------
  meas_cfg->m.quantityConfigPresent = 1;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP 
    = lte_rrc_osys_FilterCoefficient_fc3;

  lte_rrc_meas_test_cell_id.freq = 2130;
  lte_rrc_meas_test_cell_id.phy_cell_id = 0x10;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;

  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 2130);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 3);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 3);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 3);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 3);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 3);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 3);
  TF_ASSERT(cfg_req->other_mod_bitmask == (LTE_CPHY_MEAS_CFG_BIT_GAP_CFG | 
                                           LTE_CPHY_MEAS_CFG_BIT_QTY_CFG));

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cphy_meas_cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cphy_meas_cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cphy_meas_cfg_cnf, sizeof(cphy_meas_cfg_cnf));


  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&meas_cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(meas_cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(meas_cfg_cnfi->status == E_SUCCESS);

  
  // Point the meas_config to be returned to the typical meas_config
    lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_empty_reconfig_msg,
                                         sizeof(lte_rrc_meas_test_empty_reconfig_msg),
                                         &meas_cfg);
  
    // Validate meas cfg
    TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                      TRUE) == E_SUCCESS);

  // Send the meas config req to meas sm with handover cause
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = NULL;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_HANDOVER;
  meas_cfg_reqi.cell_id.phy_cell_id = 0x11 ;
  meas_cfg_reqi.cell_id.freq = 2300;

  lte_rrc_meas_test_cell_id.freq = 2300;
  lte_rrc_meas_test_cell_id.phy_cell_id = 0x11;

  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));

   // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x11);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 2300);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 3);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 3);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 3);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_GAP_CFG);

  // Check that meas id 1 is now tied to target freq
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);

  // Check that meas id 2 is now tied to src freq
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[1].meas_obj_id == 2);

  // Gap should be disabled as part of HO
  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.gap_cfg_enabled == FALSE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cphy_meas_cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cphy_meas_cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cphy_meas_cfg_cnf, sizeof(cphy_meas_cfg_cnf));


  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&meas_cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(meas_cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(meas_cfg_cnfi->status == E_SUCCESS);
  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnInterFreqTest::Teardown()
{
  //  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnInterFreqToNewTgtTest);

void LteRrcMeasConnInterFreqToNewTgtTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnInterFreqToNewTgtTest::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_cnf_s cphy_meas_cfg_cnf;
  uint32 buf_len;
  lte_rrc_meas_cfg_cnfi_s *meas_cfg_cnfi;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_ifreq_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_ifreq_msg1),
                                       &meas_cfg);

  //------------------------------------
  // Quantity config addition
  // -----------------------------------
  meas_cfg->m.quantityConfigPresent = 1;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP 
    = lte_rrc_osys_FilterCoefficient_fc3;

  lte_rrc_meas_test_cell_id.freq = 2130;
  lte_rrc_meas_test_cell_id.phy_cell_id = 0x10;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;

  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 2130);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 3);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 3);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 3);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 3);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 3);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 3);
  TF_ASSERT(cfg_req->other_mod_bitmask == (LTE_CPHY_MEAS_CFG_BIT_GAP_CFG |
                                           LTE_CPHY_MEAS_CFG_BIT_QTY_CFG));

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cphy_meas_cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cphy_meas_cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cphy_meas_cfg_cnf, sizeof(cphy_meas_cfg_cnf));


  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&meas_cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(meas_cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(meas_cfg_cnfi->status == E_SUCCESS);

  // Send the meas config req to meas sm with handover cause
  // HO is to a new target freq so the expectation is for UE to delete all
  // meas ID with object ID belonging to src cell

   // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_empty_reconfig_msg,
                                       sizeof(lte_rrc_meas_test_empty_reconfig_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    TRUE) == E_SUCCESS);

  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = NULL;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_HANDOVER;
  meas_cfg_reqi.cell_id.phy_cell_id = 0x11 ;
  meas_cfg_reqi.cell_id.freq = 100;

  lte_rrc_meas_test_cell_id.freq = 100;
  lte_rrc_meas_test_cell_id.phy_cell_id = 0x11;

  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));

   // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x11);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 3);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 3);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 3);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_GAP_CFG);

  // Gap should be disabled as part of HO
  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.gap_cfg_enabled == FALSE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cphy_meas_cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cphy_meas_cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cphy_meas_cfg_cnf, sizeof(cphy_meas_cfg_cnf));


  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&meas_cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(meas_cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(meas_cfg_cnfi->status == E_SUCCESS);
  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnInterFreqToNewTgtTest::Teardown()
{
  //  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnInterFreqWithNoServingMeasObj);

void LteRrcMeasConnInterFreqWithNoServingMeasObj::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnInterFreqWithNoServingMeasObj::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_cnf_s cphy_meas_cfg_cnf;
  uint32 buf_len;
  lte_rrc_meas_cfg_cnfi_s *meas_cfg_cnfi;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_ifreq_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_ifreq_msg1),
                                       &meas_cfg);

  lte_rrc_meas_test_cell_id.freq = 2130;
  lte_rrc_meas_test_cell_id.phy_cell_id = 0x10;

  // Set the first measObj to a freq other than serving freq
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 2200;
  meas_cfg->measIdToAddModList.elem[0].reportConfigId = 1;

  //------------------------------------
  // Quantity config addition
  // -----------------------------------
  meas_cfg->m.quantityConfigPresent = 1;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP 
    = lte_rrc_osys_FilterCoefficient_fc3;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;

  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 2130);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 3);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 3);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 3);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 3);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 3);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 3);
  TF_ASSERT(cfg_req->other_mod_bitmask == (LTE_CPHY_MEAS_CFG_BIT_GAP_CFG |
                                           LTE_CPHY_MEAS_CFG_BIT_QTY_CFG));

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cphy_meas_cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cphy_meas_cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cphy_meas_cfg_cnf, sizeof(cphy_meas_cfg_cnf));


  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&meas_cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(meas_cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(meas_cfg_cnfi->status == E_SUCCESS);

  // Send the meas config req to meas sm with handover cause
  // HO is to a new target freq so the expectation is for UE to delete all
  // meas ID with object ID belonging to src cell

   // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_empty_reconfig_msg,
                                       sizeof(lte_rrc_meas_test_empty_reconfig_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    TRUE) == E_SUCCESS);

  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = NULL;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_HANDOVER;
  meas_cfg_reqi.cell_id.phy_cell_id = 0x11 ;
  meas_cfg_reqi.cell_id.freq = 2300;

  lte_rrc_meas_test_cell_id.freq = 2300;
  lte_rrc_meas_test_cell_id.phy_cell_id = 0x11;

  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));

   // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x11);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 2300);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 3);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 3);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 3);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_GAP_CFG);

  // Gap should be disabled as part of HO
  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.gap_cfg_enabled == FALSE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cphy_meas_cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cphy_meas_cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cphy_meas_cfg_cnf, sizeof(cphy_meas_cfg_cnf));


  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&meas_cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(meas_cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(meas_cfg_cnfi->status == E_SUCCESS);
  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnInterFreqWithNoServingMeasObj::Teardown()
{
  //  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnInterFreqSameMeasObjIdAddedAndDeleted);

void LteRrcMeasConnInterFreqSameMeasObjIdAddedAndDeleted::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnInterFreqSameMeasObjIdAddedAndDeleted::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_cnf_s cphy_meas_cfg_cnf;
  uint32 buf_len, i;
  lte_rrc_meas_cfg_cnfi_s *meas_cfg_cnfi;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Populate the next measConfig that removes neighbor cells that were added previously
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  //----------------------------------------------------------------------------
  // Test Message Changes
  //----------------------------------------------------------------------------
  // Set the bitmask for measObj to add/mod list, reportConfig to add/mod list and 
  // measId to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  meas_cfg->m.reportConfigToAddModListPresent = 1;
  meas_cfg->m.measIdToAddModListPresent = 1;

  //------------------------------------
  // MeasObj reconfiguration
  // -----------------------------------
  // Add a MeasObj for EUTRA
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod) * 3);
  meas_cfg->measObjectToAddModList.n = 3;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 100;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  meas_cfg->measObjectToAddModList.elem[1].measObjectId = 2;
  meas_cfg->measObjectToAddModList.elem[1].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->carrierFreq = 200;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  meas_cfg->measObjectToAddModList.elem[2].measObjectId = 3;
  meas_cfg->measObjectToAddModList.elem[2].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->carrierFreq = 300;
  meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
  meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  //------------------------------------
  // ReportConfig addition
  // -----------------------------------
  meas_cfg->reportConfigToAddModList.elem = 
    (lte_rrc_osys_ReportConfigToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigToAddMod));
  meas_cfg->reportConfigToAddModList.n = 1;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfigId = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.t = T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA = 
    (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->
    triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->
    triggerType.u.periodical = (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical * )
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->
    triggerType.u.periodical->purpose 
    = lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 2;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = 4;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity 
    = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval 
    = lte_rrc_osys_ReportInterval_min1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity 
    = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_sameAsTriggerQuantity;

  //------------------------------------
  // Quantity config addition
  // -----------------------------------
  meas_cfg->m.quantityConfigPresent = 1;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP 
    = lte_rrc_osys_FilterCoefficient_fc3;

  //------------------------------------
  // MeasId addition
  // -----------------------------------
  meas_cfg->measIdToAddModList.elem = (lte_rrc_osys_MeasIdToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod) * 3);
  meas_cfg->measIdToAddModList.n = 3;
  meas_cfg->measIdToAddModList.elem[0].measId = 1;
  meas_cfg->measIdToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measIdToAddModList.elem[0].reportConfigId = 1;
  meas_cfg->measIdToAddModList.elem[1].measId = 2;
  meas_cfg->measIdToAddModList.elem[1].measObjectId = 2;
  meas_cfg->measIdToAddModList.elem[1].reportConfigId = 1;
  meas_cfg->measIdToAddModList.elem[2].measId = 3;
  meas_cfg->measIdToAddModList.elem[2].measObjectId = 3;
  meas_cfg->measIdToAddModList.elem[2].reportConfigId = 1;
  
  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                   FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x7);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x7);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x7);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x7);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  // Verify meas obj 1
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.report_cgi_cell_present == FALSE);

  // Verify meas obj 2
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.carrier_freq == 200);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.report_cgi_cell_present == FALSE);

  // Verify meas obj 3
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.carrier_freq == 300);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.report_cgi_cell_present == FALSE);

  // Verify reportConfig 1
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[1].meas_obj_id == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[1].report_cfg_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[2].meas_obj_id == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[2].report_cfg_id == 1);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  for (i = 0; i < 20; i++)
  {
    // Populate the next measConfig that removes neighbor cells that were added previously
    lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                         sizeof(lte_rrc_meas_test_meas_config_msg1),
                                         &meas_cfg);

    //----------------------------------------------------------------------------
    // Test Message Changes
    //----------------------------------------------------------------------------
    // Set the bitmask for measObj to add/mod list, reportConfig to add/mod list and 
    // measId to add/mod list
    meas_cfg->m.measObjectToAddModListPresent = 1;
    meas_cfg->m.measObjectToRemoveListPresent = 1;
    meas_cfg->m.measIdToRemoveListPresent = 1;
    meas_cfg->m.measIdToAddModListPresent = 1;

    //------------------------------------
    // MeasObj reconfiguration
    // -----------------------------------
    // Remove a MeasObj 1, 2 and 3
    meas_cfg->measObjectToRemoveList.elem = (lte_rrc_osys_MeasObjectId*)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectId) * 3);
    meas_cfg->measObjectToRemoveList.n = 3;
    meas_cfg->measObjectToRemoveList.elem[0] = 1;
    meas_cfg->measObjectToRemoveList.elem[1] = 2;
    meas_cfg->measObjectToRemoveList.elem[2] = 3;

    // Add a MeasObj for EUTRA
    meas_cfg->measObjectToAddModList.elem = 
      (lte_rrc_osys_MeasObjectToAddMod*)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod) * 3);
    meas_cfg->measObjectToAddModList.n = 3;

    meas_cfg->measObjectToAddModList.elem[0].measObjectId = 1;
    meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
      (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 100;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

    meas_cfg->measObjectToAddModList.elem[1].measObjectId = 2;
    meas_cfg->measObjectToAddModList.elem[1].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
    meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA = 
      (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
    meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
    meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->carrierFreq = 200;
    meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
    meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
    meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

    meas_cfg->measObjectToAddModList.elem[2].measObjectId = 3;
    meas_cfg->measObjectToAddModList.elem[2].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
    meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA = 
      (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
    meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
    meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->carrierFreq = 300;
    meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
    meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
    meas_cfg->measObjectToAddModList.elem[2].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

    //------------------------------------
    // MeasId removal
    // -----------------------------------
    meas_cfg->measIdToRemoveList.elem = (lte_rrc_osys_MeasId*)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasId) * 3);
    meas_cfg->measIdToRemoveList.n = 3;
    meas_cfg->measIdToRemoveList.elem[0] = 1;
    meas_cfg->measIdToRemoveList.elem[1] = 2;
    meas_cfg->measIdToRemoveList.elem[2] = 3;

    //------------------------------------
    // Quantity config addition
    // -----------------------------------
    meas_cfg->m.quantityConfigPresent = 1;
    meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
    meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP 
      = lte_rrc_osys_FilterCoefficient_fc3;

    //------------------------------------
    // MeasId addition
    // -----------------------------------
    meas_cfg->measIdToAddModList.elem = (lte_rrc_osys_MeasIdToAddMod*)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod) * 3);
    meas_cfg->measIdToAddModList.n = 3;
    meas_cfg->measIdToAddModList.elem[0].measId = 1;
    meas_cfg->measIdToAddModList.elem[0].measObjectId = 1;
    meas_cfg->measIdToAddModList.elem[0].reportConfigId = 1;
    meas_cfg->measIdToAddModList.elem[1].measId = 2;
    meas_cfg->measIdToAddModList.elem[1].measObjectId = 2;
    meas_cfg->measIdToAddModList.elem[1].reportConfigId = 1;
    meas_cfg->measIdToAddModList.elem[2].measId = 3;
    meas_cfg->measIdToAddModList.elem[2].measObjectId = 3;
    meas_cfg->measIdToAddModList.elem[2].reportConfigId = 1;
    
    // Validate meas cfg
    TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                     FALSE) == E_SUCCESS);

    // Send the meas config req to meas sm
    lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
    meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
    meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
    lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
    
    // We wait for Meas to process the meas cfg indi
    lte_rrc_utf_wait_for_done();

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // We expect meas sm to have thrown the conn meas cfg req
    lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
    TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

    // Verify cell info
    TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
    TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

    // Verify valid and mod bitmasks
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x7);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x7);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x1);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x7);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x7);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
    TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

    // Verify meas obj info
    // Verify meas obj 1
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.presence_antenna_port1 == TRUE);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.report_cgi_cell_present == FALSE);

    // Verify meas obj 2
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.carrier_freq == 200);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.presence_antenna_port1 == TRUE);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.freq_offset == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.neigh_cells.active_bitmask == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.report_cgi_cell_present == FALSE);

    // Verify meas obj 3
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.carrier_freq == 300);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.presence_antenna_port1 == TRUE);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.freq_offset == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.neigh_cells.active_bitmask == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.report_cgi_cell_present == FALSE);

    // Verify reportConfig 1
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.
              type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.periodical.
              purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);

    // Verify MeasId
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[1].meas_obj_id == 2);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[1].report_cfg_id == 1);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[2].meas_obj_id == 3);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[2].report_cfg_id == 1);

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

    // We expect RRC to be in the connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // Check for no further messages from meas
    lte_rrc_utf_check_no_more_msgs();

    // Free allocated memory for reconfig message
    lte_rrc_meas_test_free_reconfig_msg_ptr();
  }

}

void LteRrcMeasConnInterFreqSameMeasObjIdAddedAndDeleted::Teardown()
{
  //  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnInterFreqA1ReportConfiguredInvalidAutonomousMeasIdRemoval);

void LteRrcMeasConnInterFreqA1ReportConfiguredInvalidAutonomousMeasIdRemoval::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnInterFreqA1ReportConfiguredInvalidAutonomousMeasIdRemoval::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cphy_meas_cfg_cnf;

  lte_rrc_meas_cfg_cnfi_s *meas_cfg_cnfi;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

#ifdef FEATURE_LTE_CA
  TF_STUB(lte_rrc_llc_check_freq_match_with_pcell_scell_freq) = lte_rrc_meas_test_freq_match_with_pcell_scell_none;
#endif
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub_get_3gpp_release_version_for_rel10;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas id to add/mod list
  meas_cfg->m.measIdToAddModListPresent = 1;
  
  meas_cfg->measIdToAddModList.elem = 
    (lte_rrc_osys_MeasIdToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod));
  meas_cfg->measIdToAddModList.n = 1;

  meas_cfg->measIdToAddModList.elem[0].measId = 1;
  meas_cfg->measIdToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measIdToAddModList.elem[0].reportConfigId = 1;

  // Also need to add corresponding meas object and report config
    // Add an item to the meas obj to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod));
  meas_cfg->measObjectToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 100;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

    // Add an item to the report cfg to add/mod list
  meas_cfg->m.reportConfigToAddModListPresent = 1;
  
  meas_cfg->reportConfigToAddModList.elem = 
    (lte_rrc_osys_ReportConfigToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigToAddMod));
  meas_cfg->reportConfigToAddModList.n = 1;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfigId = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.t = 
    T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA = 
    (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.u.event = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event *)lte_rrc_asn1_osys_IE_malloc
    (sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1));
  
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

  //------------------------------------
  // Quantity config addition
  // -----------------------------------
  meas_cfg->m.quantityConfigPresent = 1;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP 
    = lte_rrc_osys_FilterCoefficient_fc3;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
	// Send the meas config req to meas sm with handover cause
	
		lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
		meas_cfg_reqi.dl_dcch_ptr = NULL;
		meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_HANDOVER;
		meas_cfg_reqi.cell_id.phy_cell_id = 0x11 ;
		meas_cfg_reqi.cell_id.freq = 2300;
	
		lte_rrc_meas_test_cell_id.freq = 2300;
		lte_rrc_meas_test_cell_id.phy_cell_id = 0x11;
	
		lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
	
		 // We wait for Meas to process the meas cfg indi
		lte_rrc_utf_wait_for_done();
	
		// We expect meas sm to have thrown the conn meas cfg req
		lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);
	
		TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
		TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);
	
		// Verify cell info
		TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x11);
		TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 2300);
	
		// Verify valid and mod bitmasks
		TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
		TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
		TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
		TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
		TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
		TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
		TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);
	
		// Check that meas id 1 is now tied to target freq
		TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
	
		// Gap should be disabled as part of HO
		TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.gap_cfg_enabled == FALSE);
	
		// We expect RRC to be in the wait_for_connected_config state
		TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);
	
		// Send successful config cnf from CPHY
		lte_rrc_init_default_hdr(&cphy_meas_cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
		cphy_meas_cfg_cnf.status = E_SUCCESS;
		lte_rrc_utf_send_msg((byte*)&cphy_meas_cfg_cnf, sizeof(cphy_meas_cfg_cnf));
	
	
		// We expect meas sm to have thrown the internal meas_cfg_cnfi
		lte_rrc_utf_get_next_int_msg((byte**)&meas_cfg_cnfi, &buf_len);
		
		TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
		TF_ASSERT(meas_cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
		TF_ASSERT(meas_cfg_cnfi->status == E_SUCCESS);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnInterFreqA1ReportConfiguredInvalidAutonomousMeasIdRemoval::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnInterFreqA2ReportConfiguredInvalidAutonomousMeasIdRemoval);

void LteRrcMeasConnInterFreqA2ReportConfiguredInvalidAutonomousMeasIdRemoval::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnInterFreqA2ReportConfiguredInvalidAutonomousMeasIdRemoval::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cphy_meas_cfg_cnf;

  lte_rrc_meas_cfg_cnfi_s *meas_cfg_cnfi;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

#ifdef FEATURE_LTE_CA
  TF_STUB(lte_rrc_llc_check_freq_match_with_pcell_scell_freq) = lte_rrc_meas_test_freq_match_with_pcell_scell_none;
#endif
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub_get_3gpp_release_version_for_rel10;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas id to add/mod list
  meas_cfg->m.measIdToAddModListPresent = 1;
  
  meas_cfg->measIdToAddModList.elem = 
    (lte_rrc_osys_MeasIdToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod));
  meas_cfg->measIdToAddModList.n = 1;

  meas_cfg->measIdToAddModList.elem[0].measId = 1;
  meas_cfg->measIdToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measIdToAddModList.elem[0].reportConfigId = 1;

  // Also need to add corresponding meas object and report config
    // Add an item to the meas obj to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod));
  meas_cfg->measObjectToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 100;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

    // Add an item to the report cfg to add/mod list
  meas_cfg->m.reportConfigToAddModListPresent = 1;
  
  meas_cfg->reportConfigToAddModList.elem = 
    (lte_rrc_osys_ReportConfigToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigToAddMod));
  meas_cfg->reportConfigToAddModList.n = 1;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfigId = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.t = 
    T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA = 
    (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.u.event = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event *)lte_rrc_asn1_osys_IE_malloc
    (sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA2;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA2 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA2 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA2));
  
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA2->a2_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA2->a2_Threshold.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

  //------------------------------------
  // Quantity config addition
  // -----------------------------------
  meas_cfg->m.quantityConfigPresent = 1;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP 
    = lte_rrc_osys_FilterCoefficient_fc3;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

	// Send the meas config req to meas sm with handover cause
		lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
		meas_cfg_reqi.dl_dcch_ptr = NULL;
		meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_HANDOVER;
		meas_cfg_reqi.cell_id.phy_cell_id = 0x11 ;
		meas_cfg_reqi.cell_id.freq = 2300;
	
		lte_rrc_meas_test_cell_id.freq = 2300;
		lte_rrc_meas_test_cell_id.phy_cell_id = 0x11;
	
		lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
	
		 // We wait for Meas to process the meas cfg indi
		lte_rrc_utf_wait_for_done();
	
		// We expect meas sm to have thrown the conn meas cfg req
		lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);
	
		TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
		TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);
	
		// Verify cell info
		TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x11);
		TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 2300);
	
		// Verify valid and mod bitmasks
		TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
		TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
		TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
		TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
		TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
		TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
		TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);
	
		// Check that meas id 1 is now tied to target freq
		TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
	
		// Gap should be disabled as part of HO
		TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.gap_cfg_enabled == FALSE);
	
		// We expect RRC to be in the wait_for_connected_config state
		TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);
	
		// Send successful config cnf from CPHY
		lte_rrc_init_default_hdr(&cphy_meas_cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
		cphy_meas_cfg_cnf.status = E_SUCCESS;
		lte_rrc_utf_send_msg((byte*)&cphy_meas_cfg_cnf, sizeof(cphy_meas_cfg_cnf));
	
	
		// We expect meas sm to have thrown the internal meas_cfg_cnfi
		lte_rrc_utf_get_next_int_msg((byte**)&meas_cfg_cnfi, &buf_len);
		
		TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
		TF_ASSERT(meas_cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
		TF_ASSERT(meas_cfg_cnfi->status == E_SUCCESS);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnInterFreqA2ReportConfiguredInvalidAutonomousMeasIdRemoval::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

#if 0
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnInterFreqB2ReportConfiguredInvalid);

void LteRrcMeasConnInterFreqB2ReportConfiguredInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnInterFreqB2ReportConfiguredInvalid::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas id to add/mod list
  meas_cfg->m.measIdToAddModListPresent = 1;
  
  meas_cfg->measIdToAddModList.elem = 
    (lte_rrc_osys_MeasIdToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod));
  meas_cfg->measIdToAddModList.n = 1;

  meas_cfg->measIdToAddModList.elem[0].measId = 1;
  meas_cfg->measIdToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measIdToAddModList.elem[0].reportConfigId = 1;

  // Also need to add corresponding meas object and report config
    // Add an item to the meas obj to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod));
  meas_cfg->measObjectToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 100;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

    // Add an item to the report cfg to add/mod list
  meas_cfg->m.reportConfigToAddModListPresent = 1;
  
  meas_cfg->reportConfigToAddModList.elem = 
    (lte_rrc_osys_ReportConfigToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigToAddMod));
  meas_cfg->reportConfigToAddModList.n = 1;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfigId = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.t = T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigInterRAT;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT = 
    (lte_rrc_osys_ReportConfigInterRAT *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigInterRAT));
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.t = T_lte_rrc_osys_ReportConfigInterRAT_triggerType_event;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.event = (lte_rrc_osys_ReportConfigInterRAT_triggerType_event * )
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigInterRAT_triggerType_event));
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigInterRAT_triggerType_event_eventId_eventB2;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.event->eventId.u.eventB2 = (lte_rrc_osys_ReportConfigInterRAT_triggerType_event_eventId_eventB2 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigInterRAT_triggerType_event_eventId_eventB2));
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.event->eventId.u.eventB2->b2_Threshold1.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.event->eventId.u.eventB2->b2_Threshold1.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX;
  // Set the B2 threshold2 to CDMA 2000
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.event->eventId.u.eventB2->b2_Threshold2.
    t = T_lte_rrc_osys_ReportConfigInterRAT_triggerType_event_eventId_eventB2_b2_Threshold2_b2_Threshold2CDMA2000;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.event->eventId.u.eventB2->b2_Threshold2.u.b2_Threshold2CDMA2000 = 50;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.event->hysteresis = 14;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.event->timeToTrigger = lte_rrc_osys_TimeToTrigger_ms480;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->maxReportCells = 2;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    reportAmount = lte_rrc_osys_ReportConfigInterRAT_reportAmount_r8;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    reportInterval = lte_rrc_osys_ReportInterval_ms640;

  //------------------------------------
  // Quantity config addition
  // -----------------------------------
  meas_cfg->m.quantityConfigPresent = 1;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP 
    = lte_rrc_osys_FilterCoefficient_fc3;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnInterFreqB2ReportConfiguredInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}
#endif

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnInterFreqA1ReportConfigValidatonDuringHandover);

void LteRrcMeasConnInterFreqA1ReportConfigValidatonDuringHandover::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnInterFreqA1ReportConfigValidatonDuringHandover::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas id to add/mod list
  meas_cfg->m.measIdToAddModListPresent = 1;
  
  meas_cfg->measIdToAddModList.elem = 
    (lte_rrc_osys_MeasIdToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod));
  meas_cfg->measIdToAddModList.n = 1;

  meas_cfg->measIdToAddModList.elem[0].measId = 1;
  meas_cfg->measIdToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measIdToAddModList.elem[0].reportConfigId = 1;

  // Also need to add corresponding meas object and report config
    // Add an item to the meas obj to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod) * 2);
  meas_cfg->measObjectToAddModList.n = 2;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 0x100;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  meas_cfg->measObjectToAddModList.elem[1].measObjectId = 2;
  meas_cfg->measObjectToAddModList.elem[1].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->carrierFreq = 0x200;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

    // Add an item to the report cfg to add/mod list
  meas_cfg->m.reportConfigToAddModListPresent = 1;
  
  meas_cfg->reportConfigToAddModList.elem = 
    (lte_rrc_osys_ReportConfigToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigToAddMod));
  meas_cfg->reportConfigToAddModList.n = 1;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfigId = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.t = 
    T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA = 
    (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.u.event = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event *)lte_rrc_asn1_osys_IE_malloc
    (sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1));
  
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

  //------------------------------------
  // Quantity config addition
  // -----------------------------------
  meas_cfg->m.quantityConfigPresent = 1;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP 
    = lte_rrc_osys_FilterCoefficient_fc3;

  //------------------------------------
  // MeasId addition
  // -----------------------------------
  meas_cfg->measIdToAddModList.elem = (lte_rrc_osys_MeasIdToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod));
  meas_cfg->measIdToAddModList.n = 1;

  meas_cfg->measIdToAddModList.elem[0].measId = 1;
  meas_cfg->measIdToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measIdToAddModList.elem[0].reportConfigId = 1;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 3);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 3);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify MeasObj
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.carrier_freq == 0x200);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // Verify reportConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a1.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].hyst == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_amt == 16);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Test Meas cfg cnf processing
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // Send a HO message that doesn't include any meas config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_no_meas_config_during_ho_msg,
                                       sizeof(lte_rrc_meas_test_no_meas_config_during_ho_msg),
                                       &meas_cfg);

  // Validate meas cfg, trying to delete a non-existant meas obj should be ignored
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    TRUE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.cell_id.phy_cell_id = 0x10;
  meas_cfg_reqi.cell_id.freq = 0x100;
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_HANDOVER;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 3);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);

  // Test Meas cfg cnf processing
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();

}

void LteRrcMeasConnInterFreqA1ReportConfigValidatonDuringHandover::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify handling of reconfiguration of MeasObj id tied to serving freq. The MeasObj id
   gets reconfigured and associated with a neighbor freq */
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnServingMeasObjIdReconfiguredToInterfreq);

void LteRrcMeasConnServingMeasObjIdReconfiguredToInterfreq::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnServingMeasObjIdReconfiguredToInterfreq::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_cnf_s cphy_meas_cfg_cnf;
  uint32 buf_len, i;
  lte_rrc_meas_cfg_cnfi_s *meas_cfg_cnfi;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Populate the next measConfig that removes neighbor cells that were added previously
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  //----------------------------------------------------------------------------
  // Test Message Changes
  //----------------------------------------------------------------------------
  // Set the bitmask for measObj to add/mod list, reportConfig to add/mod list and 
  // measId to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  meas_cfg->m.reportConfigToAddModListPresent = 1;
  meas_cfg->m.measIdToAddModListPresent = 1;

  //------------------------------------
  // MeasObj reconfiguration
  // -----------------------------------
  // Add a MeasObj for EUTRA
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod));
  meas_cfg->measObjectToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 0x100;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  //------------------------------------
  // ReportConfig addition
  // -----------------------------------
  meas_cfg->reportConfigToAddModList.elem = 
    (lte_rrc_osys_ReportConfigToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigToAddMod));
  meas_cfg->reportConfigToAddModList.n = 1;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfigId = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.t = T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA = 
    (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->
    triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->
    triggerType.u.periodical = (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical * )
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->
    triggerType.u.periodical->purpose 
    = lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 2;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = 4;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity 
    = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval 
    = lte_rrc_osys_ReportInterval_min1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity 
    = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_sameAsTriggerQuantity;

  //------------------------------------
  // Quantity config addition
  // -----------------------------------
  meas_cfg->m.quantityConfigPresent = 1;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP 
    = lte_rrc_osys_FilterCoefficient_fc3;

  //------------------------------------
  // MeasId addition
  // -----------------------------------
  meas_cfg->measIdToAddModList.elem = (lte_rrc_osys_MeasIdToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod));
  meas_cfg->measIdToAddModList.n = 1;
  meas_cfg->measIdToAddModList.elem[0].measId = 1;
  meas_cfg->measIdToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measIdToAddModList.elem[0].reportConfigId = 1;
  
  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                   FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x1);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  // Verify meas obj 1
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.report_cgi_cell_present == FALSE);

  // Verify reportConfig 1
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // Populate the next measConfig that removes neighbor cells that were added previously
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  //----------------------------------------------------------------------------
  // Test Message Changes
  //----------------------------------------------------------------------------
  // Set the bitmask for measObj to add/mod list, reportConfig to add/mod list and 
  // measId to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  meas_cfg->m.measIdToAddModListPresent = 1;

  //------------------------------------
  // MeasObj reconfiguration
  // -----------------------------------
  // Add a MeasObj for EUTRA
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod) * 2);
  meas_cfg->measObjectToAddModList.n = 2;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 100;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  meas_cfg->measObjectToAddModList.elem[1].measObjectId = 2;
  meas_cfg->measObjectToAddModList.elem[1].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->carrierFreq = 0x100;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
  meas_cfg->measObjectToAddModList.elem[1].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  //------------------------------------
  // MeasId addition
  // -----------------------------------
  meas_cfg->measIdToAddModList.elem = (lte_rrc_osys_MeasIdToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod) * 2);
  meas_cfg->measIdToAddModList.n = 2;
  meas_cfg->measIdToAddModList.elem[0].measId = 1;
  meas_cfg->measIdToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measIdToAddModList.elem[0].reportConfigId = 1;

  meas_cfg->measIdToAddModList.elem[1].measId = 2;
  meas_cfg->measIdToAddModList.elem[1].measObjectId = 2;
  meas_cfg->measIdToAddModList.elem[1].reportConfigId = 1;
  
  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                   FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x3);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x3);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x3);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x3);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Verify meas obj info
  // Verify meas obj 1
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.report_cgi_cell_present == FALSE);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.report_cgi_cell_present == FALSE);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[1].meas_obj_id == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[1].report_cfg_id == 1);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnServingMeasObjIdReconfiguredToInterfreq::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnGaplessConfigHOtoGapConfig);

void LteRrcMeasConnGaplessConfigHOtoGapConfig::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnGaplessConfigHOtoGapConfig::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;

  uint32 buf_len;
	lte_rrc_meas_test_cell_id.phy_cell_id = 397;
	lte_rrc_meas_test_cell_id.freq = 2150;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg_gaps,
                                       sizeof(lte_rrc_meas_test_meas_config_msg_gaps),
                                       &meas_cfg);
 
  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

#ifdef FEATURE_LTE_CA	
	TF_STUB(lte_rrc_is_rel_10_ca_enabled) = lte_rrc_is_rel_10_ca_enabled_true;
#endif
//	TF_STUB(_lte_rrc_cap_inter_freq_gap_less_meas_enabled) = lte_rrc_cap_inter_freq_gap_less_meas_enabled_true;

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 397);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 2150);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 7);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 7);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 7);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 7);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify MeasObj
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 2150);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.carrier_freq == 5790);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[2].obj_info.eutra.carrier_freq == 38000);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[1].meas_obj_id == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[1].report_cfg_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[2].meas_obj_id == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[2].report_cfg_id == 1);

    //Verify Gapless
  TF_ASSERT(((((cfg_req->meas_cfg_ptr->meas_obj_list)[0]).obj_info).eutra).gaps_needed == FALSE);
  TF_ASSERT(((((cfg_req->meas_cfg_ptr->meas_obj_list)[1]).obj_info).eutra).gaps_needed == FALSE);
  TF_ASSERT(((((cfg_req->meas_cfg_ptr->meas_obj_list)[2]).obj_info).eutra).gaps_needed == FALSE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Test Meas cfg cnf processing
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

	//TF_STUB(_lte_rrc_cap_inter_freq_gap_less_meas_enabled) = lte_rrc_cap_inter_freq_gap_less_meas_enabled_false;

  // Send a HO message that doesn't include any meas config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg_gaps_HO,
                                       sizeof(lte_rrc_meas_test_meas_config_msg_gaps_HO),
                                       &meas_cfg);

  // Validate meas cfg, trying to delete a non-existant meas obj should be ignored
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    TRUE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.cell_id.phy_cell_id = 3;
  meas_cfg_reqi.cell_id.freq = 38000;
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_HANDOVER;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 397);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 2150);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 7);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 7);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x07);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 5);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);

  //Verify Gapless
  TF_ASSERT(((((cfg_req->meas_cfg_ptr->meas_obj_list)[0]).obj_info).eutra).gaps_needed == TRUE);
  TF_ASSERT(((((cfg_req->meas_cfg_ptr->meas_obj_list)[1]).obj_info).eutra).gaps_needed == TRUE);
  TF_ASSERT(((((cfg_req->meas_cfg_ptr->meas_obj_list)[2]).obj_info).eutra).gaps_needed == TRUE);

  // Test Meas cfg cnf processing
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();

}

void LteRrcMeasConnGaplessConfigHOtoGapConfig::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

