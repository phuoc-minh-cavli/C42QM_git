/*!
  @file lte_rrc_meas_test_conn_intra.cpp

  @brief
  Unit test code for the meas procedure.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/meas/lte_rrc_meas_test_conn_intra.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/12/09   sk      March 09 spec migration for SIB4,SIB4,SIB5 and conn meas
06/30/09   ask     Meas changes for RLF and HO failure handling
04/17/09   sk      Updated meas state names
01/23/09   ask     Initial Revision

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
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_meas_test_msg.h"
#include "lte_rrc_cep_test_common.h"
#include <TestFramework.h>
#include <tf_stub.h>

extern "C"
{
  TF_PROTO_STUB(uint8, lte_rrc_cap_get_fgi_bit, uint8);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
  TF_PROTO_STUB(lte_rrc_div_duplex_e, lte_rrc_csp_get_last_camped_mode, void);
#ifdef FEATURE_LTE_MDT
  TF_PROTO_STUB(void, lte_rrc_loc_services_get_loc_info, lte_rrc_osys_LocationInfo_r10 *);    
#endif
}

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#if 1
/*! @brief Typedef of variables internal to module lte_rrc_meas_test_conn_intra.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */

} lte_rrc_meas_test_conn_intra_s;

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
  TF_PROTO_STUB(boolean, lte_rrc_sib_b12_to_b17_conversion_is_needed, void);
}

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_meas_test_conn_intra.cpp
*/
DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_MEAS_EICIC_REL_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_MEAS_EICIC_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_MEAS_EICIC_TDD_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_WCDMA_MSG;
DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_GSM_MSG;
DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_CDMA_MSG;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_PERIODICAL_MEAS_MSG;
DECLARE_LTE_RRC_MEAS_TEST_IRAT_PERIODICAL_MEAS_MSG;
DECLARE_LTE_RRC_MEAS_TEST_IRAT_EVENT_B1_MEAS_MSG;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_EVENT_A4_MEAS_MSG;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_EVENT_A5_MEAS_MSG;
DECLARE_LTE_RRC_MEAS_TEST_EMPTY_RECFG_MSG;


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



/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqEmpty);

void LteRrcMeasConnIntraFreqEmpty::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqEmpty::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_meas_cfg_cnfi_s *meas_cfg_cnfi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;

  uint32 buf_len;
  
  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  // We expect Meas to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

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

void LteRrcMeasConnIntraFreqEmpty::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjRemFromEmpty);

void LteRrcMeasConnIntraFreqMeasObjRemFromEmpty::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjRemFromEmpty::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to remove list
  meas_cfg->m.measObjectToRemoveListPresent = 1;
  
  meas_cfg->measObjectToRemoveList.elem = 
    (lte_rrc_osys_MeasObjectId*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectId));
  meas_cfg->measObjectToRemoveList.n = 1;

  meas_cfg->measObjectToRemoveList.elem[0] = 1;

  // Validate meas cfg, trying to delete a non-existant meas obj should be ignored
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjRemFromEmpty::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjRemFromFull);

void LteRrcMeasConnIntraFreqMeasObjRemFromFull::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjRemFromFull::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasObjectToAddModList *cur_ptr;
  lte_rrc_osys_MeasObjectToRemoveList *cur_rem_ptr;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  int i;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // ---------------------------------------------
  // Configure full list of meas objects
  // ---------------------------------------------

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod) * ASN1V_maxObjectId);
  meas_cfg->measObjectToAddModList.n = ASN1V_maxObjectId;

  cur_ptr = &meas_cfg->measObjectToAddModList;

  for (i = 1; i <= ASN1V_maxObjectId; i++)
  {
    cur_ptr->elem[i-1].measObjectId = i;
    cur_ptr->elem[i-1].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA = 
      (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
    // Start with serving freq
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->carrierFreq = 0x100+(i-1);
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  }

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

#if 0
  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0xFF);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0xFF);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  for (i = 0; i < LTE_CPHY_MEAS_MAX_ALLOWED_FREQ_LAYERS; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.carrier_freq == (0x100+i));
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.freq_offset == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.neigh_cells.active_bitmask == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  }

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
#endif
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // ---------------------------------------------
  // Delete all meas objects
  // ---------------------------------------------

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to remove list
  meas_cfg->m.measObjectToRemoveListPresent = 1;
  
  meas_cfg->measObjectToRemoveList.elem = 
    (lte_rrc_osys_MeasObjectId*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectId) * ASN1V_maxObjectId);
  meas_cfg->measObjectToRemoveList.n = ASN1V_maxObjectId;

  for (i = 1; i <= ASN1V_maxObjectId; i++)
  {
    meas_cfg->measObjectToRemoveList.elem[i - 1] = i;
  }

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
#if 0
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0xFF);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  #endif
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjRemFromFull::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasIdRemFromEmpty);

void LteRrcMeasConnIntraFreqMeasIdRemFromEmpty::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasIdRemFromEmpty::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to remove list
  meas_cfg->m.measIdToRemoveListPresent = 1;
  
  meas_cfg->measIdToRemoveList.elem = 
    (lte_rrc_osys_MeasId*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasId));
  meas_cfg->measIdToRemoveList.n = 1;

  meas_cfg->measIdToRemoveList.elem[0] = 1;

  // Validate meas cfg, trying to delete a non-existant meas id should be ignored
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasIdRemFromEmpty::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgRemFromEmpty);

void LteRrcMeasConnReportCfgRemFromEmpty::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgRemFromEmpty::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to remove list
  meas_cfg->m.reportConfigToRemoveListPresent = 1;
  
  meas_cfg->reportConfigToRemoveList.elem = 
    (lte_rrc_osys_ReportConfigId*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigId));
  meas_cfg->reportConfigToRemoveList.n = 1;

  meas_cfg->reportConfigToRemoveList.elem[0] = 1;

  // Validate meas cfg, trying to delete a non-existant reportConfig should be ignored
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgRemFromEmpty::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjAddToEmpty);

void LteRrcMeasConnIntraFreqMeasObjAddToEmpty::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjAddToEmpty::Test()
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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->neighCellConfig.numbits = 2;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->neighCellConfig.data[0] = 0x80;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->neighCellConfig.data[1] = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->neighCellConfig.data[2] = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->neighCellConfig.data[3] = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neighbourCellConfig == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjAddToEmpty::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqeICICReleaseTest);

void LteRrcMeasConnIntraFreqeICICReleaseTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqeICICReleaseTest::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_eicic_rel_msg1,
                                       sizeof(lte_rrc_meas_test_meas_eicic_rel_msg1),
                                       &meas_cfg);

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

  //verify eICIC interface
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.enabled == FALSE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_pattern_neigh.enable == FALSE);
  

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqeICICReleaseTest::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqeICICFDDTest);

void LteRrcMeasConnIntraFreqeICICFDDTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqeICICFDDTest::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_eicic_msg1,
                                       sizeof(lte_rrc_meas_test_meas_eicic_msg1),
                                       &meas_cfg);

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

  //verify eICIC interface
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.enabled == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_pattern_neigh.enable == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_pattern_neigh.meas_subfm.type == LTE_L1_MEAS_SUBFM_PATTERN_FDD);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_pattern_neigh.meas_subfm.subfm_num_bits == 40);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_pattern_neigh.meas_subfm.subfm_pattern[0] == 0x41424344);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_pattern_neigh.meas_subfm.subfm_pattern[1] == 0x45000000);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.num_cells_list == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_cell_list[0].start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_cell_list[0].end_pci == lte_rrc_osys_PhysCellIdRange_range_n252);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_cell_list[1].start_pci == 4);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_cell_list[1].end_pci == lte_rrc_osys_PhysCellIdRange_range_n24);
  

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqeICICFDDTest::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqeICICTDDTest);

void LteRrcMeasConnIntraFreqeICICTDDTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqeICICTDDTest::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Set TDD for camped cell
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_meas_test_stub__get_mode_tdd_for_last_camped_cell;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_eicic_tdd_msg1,
                                       sizeof(lte_rrc_meas_test_meas_eicic_tdd_msg1),
                                       &meas_cfg);

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

  //verify eICIC interface
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.enabled == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_pattern_neigh.enable == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_pattern_neigh.meas_subfm.type == LTE_L1_MEAS_SUBFM_PATTERN_TDD);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_pattern_neigh.meas_subfm.subfm_num_bits == 20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_pattern_neigh.meas_subfm.subfm_pattern[0] == 0x99999000);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.num_cells_list == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_cell_list[0].start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_cell_list[0].end_pci == lte_rrc_osys_PhysCellIdRange_range_n252);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_cell_list[1].start_pci == 4);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_subfm_pattern_cfg_neigh.meas_subfm_cell_list[1].end_pci == lte_rrc_osys_PhysCellIdRange_range_n24);
  

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  

}

void LteRrcMeasConnIntraFreqeICICTDDTest::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjAddForBand12);

void LteRrcMeasConnIntraFreqMeasObjAddForBand12::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjAddForBand12::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  TF_STUB(lte_rrc_sib_b12_to_b17_conversion_is_needed) = 
    lte_rrc_meas_test_stub__b12_to_b17_conversion_is_needed;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod *)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod));
  meas_cfg->measObjectToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 5100;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq 
    = lte_rrc_osys_Q_OffsetRange_dB_16;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 5770);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -16);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjAddForBand12::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqInvalidMeasObjAdd);

void LteRrcMeasConnIntraFreqInvalidMeasObjAdd::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqInvalidMeasObjAdd::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Enable MeasObj check
  lte_rrc_meas_data.sd_ptr->flag.dup_meas_obj_chk_enabled = TRUE;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  // Free the allocated memory
  lte_rrc_asn1_osys_IE_free(meas_cfg->measObjectToAddModList.elem);
  meas_cfg->measObjectToAddModList.n = 0;
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // Add an item to the meas obj to add/mod list now with handover
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod));
  meas_cfg->measObjectToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 2;
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

  // We are trying to add a new MeasObj for the same CarrierFreq... with meas
  // Validate meas cfg should catch this error and should return failure
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);
  
  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqInvalidMeasObjAdd::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjFullList);

void LteRrcMeasConnIntraFreqMeasObjFullList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjFullList::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasObjectToAddModList *cur_ptr;
  int i;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod) * ASN1V_maxObjectId);
  meas_cfg->measObjectToAddModList.n = ASN1V_maxObjectId;

  cur_ptr = &meas_cfg->measObjectToAddModList;

  for (i = 1; i <= ASN1V_maxObjectId; i++)
  {
    cur_ptr->elem[i-1].measObjectId = i;
    cur_ptr->elem[i-1].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA = 
      (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
    // Start with serving freq
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->carrierFreq = 0x100+(i-1);
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  }

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

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

#if 0
  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0xFF);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0xFF);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  for (i = 0; i < ASN1V_maxObjectId; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.carrier_freq == (0x100+i));
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.freq_offset == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.neigh_cells.active_bitmask == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  }
#endif

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjFullList::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjListOverflow);

void LteRrcMeasConnIntraFreqMeasObjListOverflow::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjListOverflow::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_osys_MeasObjectToAddModList *cur_ptr;
  int i;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod) * (ASN1V_maxObjectId + 1));
  meas_cfg->measObjectToAddModList.n = ASN1V_maxObjectId + 1;

  cur_ptr = &meas_cfg->measObjectToAddModList;

  for (i = 1; i <= ASN1V_maxObjectId + 1; i++)
  {
    cur_ptr->elem[i-1].measObjectId = i;
    cur_ptr->elem[i-1].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA = 
      (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->carrierFreq = 100;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  }

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

void LteRrcMeasConnIntraFreqMeasObjListOverflow::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgAddToEmpty);

void LteRrcMeasConnReportCfgAddToEmpty::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgAddToEmpty::Test()
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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.periodical.purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgAddToEmpty::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgFullList);

void LteRrcMeasConnReportCfgFullList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgFullList::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_ReportConfigToAddModList *cur_ptr;
  int i;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the report cfg to add/mod list
  meas_cfg->m.reportConfigToAddModListPresent = 1;
  
  meas_cfg->reportConfigToAddModList.elem = 
    (lte_rrc_osys_ReportConfigToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigToAddMod) * ASN1V_maxReportConfigId);
  meas_cfg->reportConfigToAddModList.n = ASN1V_maxReportConfigId;

  cur_ptr = &meas_cfg->reportConfigToAddModList;

  for (i = 1; i <= ASN1V_maxReportConfigId; i++)
  {
    cur_ptr->elem[i-1].reportConfigId = i;
    cur_ptr->elem[i-1].reportConfig.t = T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA = 
      (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->triggerType.t = 
      T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
    cur_ptr->elem[i-1].reportConfig.u.
      reportConfigEUTRA->triggerType.u.periodical = 
      (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));
    cur_ptr->elem[i-1].reportConfig.u.
      reportConfigEUTRA->triggerType.u.periodical->purpose = 
      lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

  }

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0xFFFFFFFF);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0xFFFFFFFF);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  for (i = 0; i < ASN1V_maxReportConfigId; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[i].report_int == 5120);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[i].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[i].trig_cfg.data.periodical.purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[i].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[i].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[i].max_cells == 6);
  }

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgFullList::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgListOverflow);

void LteRrcMeasConnReportCfgListOverflow::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgListOverflow::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_osys_ReportConfigToAddModList *cur_ptr;
  int i;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the report cfg to add/mod list
  meas_cfg->m.reportConfigToAddModListPresent = 1;
  
  meas_cfg->reportConfigToAddModList.elem = 
    (lte_rrc_osys_ReportConfigToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigToAddMod) * (ASN1V_maxReportConfigId + 1));
  meas_cfg->reportConfigToAddModList.n = ASN1V_maxReportConfigId + 1;

  cur_ptr = &meas_cfg->reportConfigToAddModList;

  for (i = 1; i <= ASN1V_maxReportConfigId + 1; i++)
  {
    cur_ptr->elem[i-1].reportConfigId = i;
    cur_ptr->elem[i-1].reportConfig.t = T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA = 
      (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->triggerType.t = 
      T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
    cur_ptr->elem[i-1].reportConfig.u.
      reportConfigEUTRA->triggerType.u.periodical = 
      (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

    cur_ptr->elem[i-1].reportConfig.u.
      reportConfigEUTRA->triggerType.u.periodical->purpose = 
      lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

  }

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

void LteRrcMeasConnReportCfgListOverflow::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasIdAddWithoutQty);

void LteRrcMeasConnIntraFreqMeasIdAddWithoutQty::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  // Enable Qty config validation
  lte_rrc_meas_data.sd_ptr->flag.qty_chk_enabled = TRUE;
}

void LteRrcMeasConnIntraFreqMeasIdAddWithoutQty::Test()
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
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.t = 
    T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA = 
    (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasIdAddWithoutQty::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasIdAddToEmpty);

void LteRrcMeasConnIntraFreqMeasIdAddToEmpty::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasIdAddToEmpty::Test()
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
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.t = 
    T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA = 
    (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);

  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 3);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasIdAddToEmpty::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqRptCfgEutraLocInfoAddRem);

void LteRrcMeasConnIntraFreqRptCfgEutraLocInfoAddRem::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqRptCfgEutraLocInfoAddRem::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
    lte_cphy_conn_meas_report_ind_s cphy_meas_report_ind;
    lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
    lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
    lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
   lte_rrc_meas_s *i_ptr;
   lte_rrc_osys_UL_DCCH_MessageType *ul_dcch_msg_ptr;
   lte_rrc_osys_MeasResults measResults;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

#ifdef FEATURE_LTE_MDT
   TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
#endif
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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->m.includeLocationInfo_r10Present = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->includeLocationInfo_r10 = 
     lte_rrc_osys_ReportConfigEUTRA_includeLocationInfo_r10_true;


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

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);


  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);

  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 3);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].location_info_enabled == TRUE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);


  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);



/****************************************************************************************************/
/*Verification of NEED-OR field for includeLocationInfo for EUTRA rpt cfg*/

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

#ifdef FEATURE_LTE_MDT
   TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
#endif

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->m.includeLocationInfo_r10Present = 0;

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

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);


  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);

  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 3);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].location_info_enabled == FALSE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);


  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);





  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

#ifdef FEATURE_LTE_MDT
   TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
#endif

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->m.includeLocationInfo_r10Present = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->includeLocationInfo_r10 = 
     lte_rrc_osys_ReportConfigEUTRA_includeLocationInfo_r10_true;


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

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);


  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);

  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 3);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].location_info_enabled == TRUE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);


  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  /***************************************************************************************************/
  memset(&cphy_meas_report_ind, 0, sizeof(lte_cphy_conn_meas_report_ind_s));

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&cphy_meas_report_ind.msgr_hdr, LTE_CPHY_MEAS_REPORT_IND);
  cphy_meas_report_ind.meas_id = 1;
  cphy_meas_report_ind.auto_delete_meas_id = FALSE;
  cphy_meas_report_ind.serving_rsrp = 40;
  cphy_meas_report_ind.serving_rsrq = 34;
  cphy_meas_report_ind.neigh_results.size = 5;
  cphy_meas_report_ind.neigh_results.type = LTE_CPHY_CONN_MEAS_REPORT_TYPE_EUTRA;
  cphy_meas_report_ind.neigh_results.list.eutra[0].pci = 100;
  cphy_meas_report_ind.neigh_results.list.eutra[0].rsrp = 97;
  cphy_meas_report_ind.neigh_results.list.eutra[0].rsrq = 34;
  cphy_meas_report_ind.neigh_results.list.eutra[1].pci = 150;
  cphy_meas_report_ind.neigh_results.list.eutra[1].rsrp = 30;
  cphy_meas_report_ind.neigh_results.list.eutra[1].rsrq = 0;
  cphy_meas_report_ind.neigh_results.list.eutra[2].pci = 200;
  cphy_meas_report_ind.neigh_results.list.eutra[2].rsrp = 97;
  cphy_meas_report_ind.neigh_results.list.eutra[2].rsrq = 25;
  cphy_meas_report_ind.neigh_results.list.eutra[3].pci = 250;
  cphy_meas_report_ind.neigh_results.list.eutra[3].rsrp = 85;
  cphy_meas_report_ind.neigh_results.list.eutra[3].rsrq = -1;
  cphy_meas_report_ind.neigh_results.list.eutra[4].pci = 300;
  cphy_meas_report_ind.neigh_results.list.eutra[4].rsrp = -5;
  cphy_meas_report_ind.neigh_results.list.eutra[4].rsrq = 34;

  lte_rrc_utf_send_msg((byte*)&cphy_meas_report_ind, sizeof(cphy_meas_report_ind));

  lte_rrc_utf_wait_for_done();

#ifdef FEATURE_LTE_MDT
  TF_ASSERT(lte_rrc_meas_start_loc_ser_is_sent() == TRUE);
#endif
  // We expect meas sm to have thrown the ul msg reqi containing the meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);


  memset(&cphy_meas_report_ind, 0, sizeof(lte_cphy_conn_meas_report_ind_s));

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&cphy_meas_report_ind.msgr_hdr, LTE_CPHY_MEAS_REPORT_IND);
  cphy_meas_report_ind.meas_id = 1;
  cphy_meas_report_ind.auto_delete_meas_id = TRUE;
  cphy_meas_report_ind.serving_rsrp = 40;
  cphy_meas_report_ind.serving_rsrq = 34;
  cphy_meas_report_ind.neigh_results.size = 5;
  cphy_meas_report_ind.neigh_results.type = LTE_CPHY_CONN_MEAS_REPORT_TYPE_EUTRA;
  cphy_meas_report_ind.neigh_results.list.eutra[0].pci = 100;
  cphy_meas_report_ind.neigh_results.list.eutra[0].rsrp = 97;
  cphy_meas_report_ind.neigh_results.list.eutra[0].rsrq = 34;
  cphy_meas_report_ind.neigh_results.list.eutra[1].pci = 150;
  cphy_meas_report_ind.neigh_results.list.eutra[1].rsrp = 30;
  cphy_meas_report_ind.neigh_results.list.eutra[1].rsrq = 0;
  cphy_meas_report_ind.neigh_results.list.eutra[2].pci = 200;
  cphy_meas_report_ind.neigh_results.list.eutra[2].rsrp = 97;
  cphy_meas_report_ind.neigh_results.list.eutra[2].rsrq = 25;
  cphy_meas_report_ind.neigh_results.list.eutra[3].pci = 250;
  cphy_meas_report_ind.neigh_results.list.eutra[3].rsrp = 85;
  cphy_meas_report_ind.neigh_results.list.eutra[3].rsrq = -1;
  cphy_meas_report_ind.neigh_results.list.eutra[4].pci = 300;
  cphy_meas_report_ind.neigh_results.list.eutra[4].rsrp = -5;
  cphy_meas_report_ind.neigh_results.list.eutra[4].rsrq = 34;

  lte_rrc_utf_send_msg((byte*)&cphy_meas_report_ind, sizeof(cphy_meas_report_ind));

  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the ul msg reqi containing the meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);


  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_MessageType *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.t 
            == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->m.nonCriticalExtensionPresent == 0);

  measResults = ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults;

#if 0
  /*Validate the UL MR for LocationInfo*/
  TF_ASSERT(measResults.m.locationInfo_r10Present == 1);
  TF_ASSERT(measResults.locationInfo_r10.m.gnss_TOD_msec_r10Present == 1);

  TF_ASSERT(measResults.locationInfo_r10.m.horizontalVelocity_r10Present ==0);
  TF_ASSERT(measResults.locationInfo_r10.locationCoordinates_r10.t == 
            T_lte_rrc_osys_LocationInfo_r10_locationCoordinates_r10_ellipsoid_Point_r10);
  TF_ASSERT(measResults.locationInfo_r10.locationCoordinates_r10.u.ellipsoid_Point_r10->numocts == 6);
#endif
#ifdef FEATURE_LTE_MDT
    TF_ASSERT(lte_rrc_meas_start_loc_ser_is_sent() == TRUE);
#endif

   /******************************************************************************/

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

#ifdef FEATURE_LTE_MDT
   TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
#endif

  // Add an item to the meas id to add/mod list
  meas_cfg->m.measIdToAddModListPresent = 1;

  meas_cfg->measIdToAddModList.elem = 
    (lte_rrc_osys_MeasIdToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod));
  meas_cfg->measIdToAddModList.n = 1;

  meas_cfg->measIdToAddModList.elem[0].measId = 2;
  meas_cfg->measIdToAddModList.elem[0].measObjectId = 2;
  meas_cfg->measIdToAddModList.elem[0].reportConfigId = 2;

  // Also need to add corresponding meas object and report config
    // Add an item to the meas obj to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod));
  meas_cfg->measObjectToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 2;
  meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 200;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

    // Add an item to the report cfg to add/mod list
  meas_cfg->m.reportConfigToAddModListPresent = 1;

  meas_cfg->reportConfigToAddModList.elem = 
    (lte_rrc_osys_ReportConfigToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigToAddMod));
  meas_cfg->reportConfigToAddModList.n = 1;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfigId = 2;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.t = 
    T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA = 
    (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->m.includeLocationInfo_r10Present = 0;

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

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[1].location_info_enabled == FALSE);


  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);


  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);


/****************************************************************************************/
    // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  /****************************************************************/
  /*Remove the RptCfg - This should cause a stop ser being sent to Loc ser engine*/
  /***************************************************************/

  // Add an item to the meas obj to remove list
  meas_cfg->m.measGapConfigPresent = 0;
  meas_cfg->m.measIdToAddModListPresent = 0;
  meas_cfg->m.measIdToRemoveListPresent = 0;
  meas_cfg->m.measObjectToAddModListPresent = 0;
  meas_cfg->m.measObjectToRemoveListPresent = 0;
  meas_cfg->m.preRegistrationInfoHRPDPresent = 0;
  meas_cfg->m.quantityConfigPresent = 0;
  meas_cfg->m.reportConfigToAddModListPresent = 0;
  meas_cfg->m.reportConfigToRemoveListPresent = 0;
  meas_cfg->m.s_MeasurePresent = 0;
  meas_cfg->m.speedStateParsPresent = 0;
  meas_cfg->m.reportConfigToRemoveListPresent = 1;
  
  meas_cfg->reportConfigToRemoveList.elem = 
    (lte_rrc_osys_ReportConfigId*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigId));
  meas_cfg->reportConfigToRemoveList.n = 1;

  meas_cfg->reportConfigToRemoveList.elem[0] = 1;

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


  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();

  /**/
  #ifdef FEATURE_LTE_REL11

#ifdef FEATURE_LTE_MDT
  TF_ASSERT(lte_rrc_meas_start_loc_ser_is_sent() == FALSE);
  TF_ASSERT(lte_rrc_meas_obtain_loc_info_is_configured() == FALSE);
#endif
  #endif
	  
	    // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

	  
	  
	  
	  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqRptCfgEutraLocInfoAddRem::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqRptCfgEutraObtainLocCfgAddRem);

void LteRrcMeasConnIntraFreqRptCfgEutraObtainLocCfgAddRem::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}
void LteRrcMeasConnIntraFreqRptCfgEutraObtainLocCfgAddRem::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
    lte_cphy_conn_meas_report_ind_s cphy_meas_report_ind;
    lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
    lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
    lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
   lte_rrc_meas_s *i_ptr;
lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs rrc_recfg_msg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  memset(&rrc_recfg_msg, 0, sizeof(rrc_recfg_msg));
  
  #ifdef FEATURE_LTE_REL11
  rrc_recfg_msg.m.nonCriticalExtensionPresent = 1;
  rrc_recfg_msg.nonCriticalExtension.m.nonCriticalExtensionPresent = 1;
  rrc_recfg_msg.nonCriticalExtension.nonCriticalExtension.m.otherConfig_r9Present = 1;
  rrc_recfg_msg.nonCriticalExtension.nonCriticalExtension.otherConfig_r9.m.obtainLocationConfig_r11Present = 1;
  rrc_recfg_msg.nonCriticalExtension.nonCriticalExtension.otherConfig_r9.obtainLocationConfig_r11.m.obtainLocation_r11Present = 1;
  rrc_recfg_msg.nonCriticalExtension.nonCriticalExtension.otherConfig_r9.obtainLocationConfig_r11.obtainLocation_r11 =        
     lte_rrc_osys_ObtainLocationConfig_r11_obtainLocation_r11_setup;
  #ifdef FEATURE_LTE_MDT
  lte_rrc_meas_update_loc_cfg(&rrc_recfg_msg);

  TF_ASSERT(lte_rrc_meas_obtain_loc_info_is_configured() == TRUE);
  #endif
  #endif
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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->m.includeLocationInfo_r10Present = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->includeLocationInfo_r10 = 
     lte_rrc_osys_ReportConfigEUTRA_includeLocationInfo_r10_true;


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

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);

  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].location_info_enabled == TRUE);
  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);


  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  memset(&cphy_meas_report_ind, 0, sizeof(lte_cphy_conn_meas_report_ind_s));

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&cphy_meas_report_ind.msgr_hdr, LTE_CPHY_MEAS_REPORT_IND);
  cphy_meas_report_ind.meas_id = 1;
  cphy_meas_report_ind.auto_delete_meas_id = TRUE;
  cphy_meas_report_ind.serving_rsrp = 40;
  cphy_meas_report_ind.serving_rsrq = 34;
  cphy_meas_report_ind.neigh_results.size = 5;
  cphy_meas_report_ind.neigh_results.type = LTE_CPHY_CONN_MEAS_REPORT_TYPE_EUTRA;
  cphy_meas_report_ind.neigh_results.list.eutra[0].pci = 100;
  cphy_meas_report_ind.neigh_results.list.eutra[0].rsrp = 97;
  cphy_meas_report_ind.neigh_results.list.eutra[0].rsrq = 34;
  cphy_meas_report_ind.neigh_results.list.eutra[1].pci = 150;
  cphy_meas_report_ind.neigh_results.list.eutra[1].rsrp = 30;
  cphy_meas_report_ind.neigh_results.list.eutra[1].rsrq = 0;
  cphy_meas_report_ind.neigh_results.list.eutra[2].pci = 200;
  cphy_meas_report_ind.neigh_results.list.eutra[2].rsrp = 97;
  cphy_meas_report_ind.neigh_results.list.eutra[2].rsrq = 25;
  cphy_meas_report_ind.neigh_results.list.eutra[3].pci = 250;
  cphy_meas_report_ind.neigh_results.list.eutra[3].rsrp = 85;
  cphy_meas_report_ind.neigh_results.list.eutra[3].rsrq = -1;
  cphy_meas_report_ind.neigh_results.list.eutra[4].pci = 300;
  cphy_meas_report_ind.neigh_results.list.eutra[4].rsrp = -5;
  cphy_meas_report_ind.neigh_results.list.eutra[4].rsrq = 34;

  lte_rrc_utf_send_msg((byte*)&cphy_meas_report_ind, sizeof(cphy_meas_report_ind));

  lte_rrc_utf_wait_for_done();

#ifdef FEATURE_LTE_MDT
  TF_ASSERT(lte_rrc_meas_start_loc_ser_is_sent() == TRUE);
#endif

  // We expect meas sm to have thrown the ul msg reqi containing the meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  memset(&rrc_recfg_msg, 0, sizeof(rrc_recfg_msg));	  

  #ifdef FEATURE_LTE_REL11  
  rrc_recfg_msg.m.nonCriticalExtensionPresent = 1;
  rrc_recfg_msg.nonCriticalExtension.m.nonCriticalExtensionPresent = 1;
  rrc_recfg_msg.nonCriticalExtension.nonCriticalExtension.m.otherConfig_r9Present = 1;
  rrc_recfg_msg.nonCriticalExtension.nonCriticalExtension.otherConfig_r9.m.obtainLocationConfig_r11Present = 1;
  rrc_recfg_msg.nonCriticalExtension.nonCriticalExtension.otherConfig_r9.obtainLocationConfig_r11.m.obtainLocation_r11Present = 0;
  #ifdef FEATURE_LTE_MDT
  lte_rrc_meas_update_loc_cfg(&rrc_recfg_msg);
  #endif

  lte_rrc_utf_wait_for_done();

#ifdef FEATURE_LTE_MDT
  TF_ASSERT(lte_rrc_meas_obtain_loc_info_is_configured() == FALSE);
  TF_ASSERT(lte_rrc_meas_start_loc_ser_is_sent() == TRUE);  
#endif
  #endif

    // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to remove list
  meas_cfg->m.measGapConfigPresent = 0;
  meas_cfg->m.measIdToAddModListPresent = 0;
  meas_cfg->m.measIdToRemoveListPresent = 0;
  meas_cfg->m.measObjectToAddModListPresent = 0;
  meas_cfg->m.measObjectToRemoveListPresent = 0;
  meas_cfg->m.preRegistrationInfoHRPDPresent = 0;
  meas_cfg->m.quantityConfigPresent = 0;
  meas_cfg->m.reportConfigToAddModListPresent = 0;
  meas_cfg->m.reportConfigToRemoveListPresent = 0;
  meas_cfg->m.s_MeasurePresent = 0;
  meas_cfg->m.speedStateParsPresent = 0;
  meas_cfg->m.reportConfigToRemoveListPresent = 1;
  
  meas_cfg->reportConfigToRemoveList.elem = 
    (lte_rrc_osys_ReportConfigId*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigId));
  meas_cfg->reportConfigToRemoveList.n = 1;

  meas_cfg->reportConfigToRemoveList.elem[0] = 1;

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

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();

#ifdef FEATURE_LTE_MDT
  TF_ASSERT(lte_rrc_meas_start_loc_ser_is_sent() == FALSE);
#endif	  
	    // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

	  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqRptCfgEutraObtainLocCfgAddRem::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasIdDeleteOnMeasObjRemoval);

void LteRrcMeasConnIntraFreqMeasIdDeleteOnMeasObjRemoval::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasIdDeleteOnMeasObjRemoval::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // ---------------------------------------------
  // Add 1 meas obj, 1 report cfg, 1 meas id
  // ---------------------------------------------

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
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

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();


  // -----------------------------------------------------------
  // Delete the meas obj and check if meas id is deleted as well
  // -----------------------------------------------------------

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to remove list
  meas_cfg->m.measObjectToRemoveListPresent = 1;

  meas_cfg->measObjectToRemoveList.elem = 
    (lte_rrc_osys_MeasObjectId*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectId));
  meas_cfg->measObjectToRemoveList.n = 1;

  meas_cfg->measObjectToRemoveList.elem[0] = 1;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
}

void LteRrcMeasConnIntraFreqMeasIdDeleteOnMeasObjRemoval::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasIdDeleteOnReportCfgRemoval);

void LteRrcMeasConnIntraFreqMeasIdDeleteOnReportCfgRemoval::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasIdDeleteOnReportCfgRemoval::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // ---------------------------------------------
  // Add 1 meas obj, 1 report cfg, 1 meas id
  // ---------------------------------------------

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
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

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();


  // -----------------------------------------------------------
  // Delete the report cfg and check if meas id is deleted as well
  // -----------------------------------------------------------

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to remove list
  meas_cfg->m.reportConfigToRemoveListPresent = 1;

  meas_cfg->reportConfigToRemoveList.elem = 
    (lte_rrc_osys_ReportConfigId*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigId));
  meas_cfg->reportConfigToRemoveList.n = 1;

  meas_cfg->reportConfigToRemoveList.elem[0] = 1;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
}

void LteRrcMeasConnIntraFreqMeasIdDeleteOnReportCfgRemoval::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasIdFullList);

void LteRrcMeasConnIntraFreqMeasIdFullList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasIdFullList::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasIdToAddModList *cur_ptr;
  int i;

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
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod) * ASN1V_maxMeasId);
  meas_cfg->measIdToAddModList.n = ASN1V_maxMeasId;

  cur_ptr = &meas_cfg->measIdToAddModList;

  for (i = 1; i <= ASN1V_maxMeasId; i++)
  {
    cur_ptr->elem[i-1].measId = i;
    cur_ptr->elem[i-1].measObjectId = 1;
    cur_ptr->elem[i-1].reportConfigId = 1;

  }

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0xFFFFFFFF);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0xFFFFFFFF);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  for (i = 0; i < ASN1V_maxMeasId; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[i].meas_obj_id == 1);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[i].report_cfg_id == 1);
  }

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasIdFullList::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}





TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqNmrTest);

void LteRrcMeasConnIntraFreqNmrTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_NMR_INFO_REQ,
      LTE_RRC_NMR_INFO_CNF,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqNmrTest::Test()
{
  uint32 buf_len;
  lte_rrc_nmr_info_req_s rrc_nmr_info_req;
  lte_rrc_nmr_info_cnf_s *rrc_nmr_info_cnf_ptr;
  lte_cphy_nmr_info_req_s *cphy_nmr_info_req_ptr;
  lte_cphy_nmr_info_cnf_s cphy_nmr_info_cnf;


  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // send RRC_NMR_INFO_REQ
  lte_rrc_init_default_hdr(&rrc_nmr_info_req.msg_hdr, LTE_RRC_NMR_INFO_REQ);
  rrc_nmr_info_req.report_type = LTE_RRC_NMR_REPORT_INTRA_FREQ;
  lte_rrc_utf_send_msg((byte*)&rrc_nmr_info_req, sizeof(rrc_nmr_info_req));

  // Receive CPHY_NMR_INFO_REQ
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_nmr_info_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_nmr_info_req_s));
  TF_ASSERT(cphy_nmr_info_req_ptr->msgr_hdr.id == LTE_CPHY_NMR_INFO_REQ);
  TF_ASSERT(cphy_nmr_info_req_ptr->report_type == LTE_CPHY_NMR_REPORT_INTRA_FREQ);

  // Send CPHY_NMR_INFO_CNF
  lte_rrc_init_default_hdr(&cphy_nmr_info_cnf.msgr_hdr, LTE_CPHY_NMR_INFO_CNF);
  cphy_nmr_info_cnf.meas_id = 1;
  cphy_nmr_info_cnf.serving_rsrp = 15;
  cphy_nmr_info_cnf.serving_rsrq = 15;
  cphy_nmr_info_cnf.auto_delete_meas_id = FALSE;
  cphy_nmr_info_cnf.neigh_results.type = LTE_CPHY_CONN_MEAS_REPORT_TYPE_EUTRA;
  cphy_nmr_info_cnf.neigh_results.size = 1;
  cphy_nmr_info_cnf.neigh_results.list.eutra[0].pci = 100;
  cphy_nmr_info_cnf.neigh_results.list.eutra[0].rsrp = 10;
  cphy_nmr_info_cnf.neigh_results.list.eutra[0].rsrq = 10;
  lte_rrc_utf_send_msg((byte*)&cphy_nmr_info_cnf, sizeof(cphy_nmr_info_cnf));

  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Receive RRC_NMR_INFO_CNF
  lte_rrc_utf_get_next_ext_msg((byte**)&rrc_nmr_info_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_nmr_info_cnf_s));
  TF_ASSERT(rrc_nmr_info_cnf_ptr->msg_hdr.id == LTE_RRC_NMR_INFO_CNF);
  TF_ASSERT(rrc_nmr_info_cnf_ptr->status == LTE_RRC_NMR_INFO_SUCCESS);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();

}

void LteRrcMeasConnIntraFreqNmrTest::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasIdListOverflow);

void LteRrcMeasConnIntraFreqMeasIdListOverflow::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasIdListOverflow::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_osys_MeasIdToAddModList *cur_ptr;
  int i;

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
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod) * (ASN1V_maxMeasId + 1));
  meas_cfg->measIdToAddModList.n = ASN1V_maxMeasId + 1;

  cur_ptr = &meas_cfg->measIdToAddModList;

  for (i = 1; i <= ASN1V_maxMeasId + 1; i++)
  {
    cur_ptr->elem[i-1].measId = i;
    cur_ptr->elem[i-1].measObjectId = 1;
    cur_ptr->elem[i-1].reportConfigId = 1;

  }

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

void LteRrcMeasConnIntraFreqMeasIdListOverflow::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraCarrierFreqMin);

void LteRrcMeasConnIntraFreqMeasObjEutraCarrierFreqMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraCarrierFreqMin::Test()
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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraCarrierFreqMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraCarrierFreqMax);

void LteRrcMeasConnIntraFreqMeasObjEutraCarrierFreqMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraCarrierFreqMax::Test()
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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 5180;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraCarrierFreqMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraMeasBandwidth);

void LteRrcMeasConnIntraFreqMeasObjEutraMeasBandwidth::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraMeasBandwidth::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  int i;
  lte_rrc_osys_AllowedMeasBandwidth test_values[] = 
  {
    lte_rrc_osys_AllowedMeasBandwidth_mbw6,
    lte_rrc_osys_AllowedMeasBandwidth_mbw15,
    lte_rrc_osys_AllowedMeasBandwidth_mbw25,
    lte_rrc_osys_AllowedMeasBandwidth_mbw50,
    lte_rrc_osys_AllowedMeasBandwidth_mbw75,
    lte_rrc_osys_AllowedMeasBandwidth_mbw100,
  };

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod));
  meas_cfg->measObjectToAddModList.n = 1;

  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    meas_cfg->measObjectToAddModList.elem[0].measObjectId = 1;
    meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
      (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 5180 - i;
    
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = test_values[i];
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
    TF_ASSERT(cfg_req->other_mod_bitmask == 0);

    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 5180 - i);

    switch (meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth)
    {
      case lte_rrc_osys_AllowedMeasBandwidth_mbw6:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_6);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw15:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw25:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw50:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_50);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw75:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_75);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw100:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_100);
        break;

      default:
        TF_ASSERT(0);
    }

    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Send successful config cnf from CPHY
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the meas cfg cnf
    lte_rrc_utf_wait_for_done();

    // We expect RRC to be in the connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // We expect meas sm to have thrown the internal meas_cfg_cnfi
    lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
    TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
    TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  }

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraMeasBandwidth::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraMeasBandwidthInvalid);

void LteRrcMeasConnIntraFreqMeasObjEutraMeasBandwidthInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraMeasBandwidthInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 5180;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = (lte_rrc_osys_AllowedMeasBandwidth) 6;

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

void LteRrcMeasConnIntraFreqMeasObjEutraMeasBandwidthInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraFreqOffsetNotPresent);

void LteRrcMeasConnIntraFreqMeasObjEutraFreqOffsetNotPresent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraFreqOffsetNotPresent::Test()
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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 5180;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraFreqOffsetNotPresent::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraFreqOffset);

void LteRrcMeasConnIntraFreqMeasObjEutraFreqOffset::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraFreqOffset::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  int i;
  lte_rrc_osys_Q_OffsetRange test_values[] = 
  {
    lte_rrc_osys_Q_OffsetRange_dB_24,
    lte_rrc_osys_Q_OffsetRange_dB_22,
    lte_rrc_osys_Q_OffsetRange_dB_20,
    lte_rrc_osys_Q_OffsetRange_dB_18,
    lte_rrc_osys_Q_OffsetRange_dB_16,
    lte_rrc_osys_Q_OffsetRange_dB_14,
    lte_rrc_osys_Q_OffsetRange_dB_12,
    lte_rrc_osys_Q_OffsetRange_dB_10,
    lte_rrc_osys_Q_OffsetRange_dB_8,
    lte_rrc_osys_Q_OffsetRange_dB_6,
    lte_rrc_osys_Q_OffsetRange_dB_5,
    lte_rrc_osys_Q_OffsetRange_dB_4,
    lte_rrc_osys_Q_OffsetRange_dB_3,
    lte_rrc_osys_Q_OffsetRange_dB_2,
    lte_rrc_osys_Q_OffsetRange_dB_1,
    lte_rrc_osys_Q_OffsetRange_dB0,
    lte_rrc_osys_Q_OffsetRange_dB1,
    lte_rrc_osys_Q_OffsetRange_dB2,
    lte_rrc_osys_Q_OffsetRange_dB3,
    lte_rrc_osys_Q_OffsetRange_dB4,
    lte_rrc_osys_Q_OffsetRange_dB5,
    lte_rrc_osys_Q_OffsetRange_dB6,
    lte_rrc_osys_Q_OffsetRange_dB8,
    lte_rrc_osys_Q_OffsetRange_dB10,
    lte_rrc_osys_Q_OffsetRange_dB12,
    lte_rrc_osys_Q_OffsetRange_dB14,
    lte_rrc_osys_Q_OffsetRange_dB16,
    lte_rrc_osys_Q_OffsetRange_dB18,
    lte_rrc_osys_Q_OffsetRange_dB20,
    lte_rrc_osys_Q_OffsetRange_dB22,
    lte_rrc_osys_Q_OffsetRange_dB24,
  };

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod));
  meas_cfg->measObjectToAddModList.n = 1;

  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    meas_cfg->measObjectToAddModList.elem[0].measObjectId = 1;
    meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
      (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 5180-i;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = test_values[i];

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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
    TF_ASSERT(cfg_req->other_mod_bitmask == 0);

    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 5180-i);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);

    switch (meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq)
    {
      case lte_rrc_osys_Q_OffsetRange_dB_24:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -24);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB_22:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -22);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB_20:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -20);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB_18:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -18);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB_16:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -16);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB_14:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -14);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB_12:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -12);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB_10:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -10);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB_8:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -8);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB_6:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -6);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB_5:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -5);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB_4:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -4);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB_3:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -3);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB_2:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -2);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB_1:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == -1);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB0:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB1:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 1);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB2:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 2);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB3:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 3);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB4:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 4);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB5:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 5);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB6:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 6);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB8:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 8);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB10:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 10);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB12:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 12);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB14:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 14);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB16:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 16);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB18:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 18);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB20:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 20);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB22:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 22);
        break;

      case lte_rrc_osys_Q_OffsetRange_dB24:
        TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 24);
        break;

      default:
        TF_ASSERT(0);
    }

    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Send successful config cnf from CPHY
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the meas cfg cnf
    lte_rrc_utf_wait_for_done();

    // We expect RRC to be in the connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // We expect meas sm to have thrown the internal meas_cfg_cnfi
    lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
    TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
    TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  }

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraFreqOffset::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraNeighCellEmptyList);

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellEmptyList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellEmptyList::Test()
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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Make the neigh cell list empty
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem = NULL;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.n = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellEmptyList::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraNeighCellAddToEmpty);

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellAddToEmpty::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellAddToEmpty::Test()
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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Add one neigh cell to the neigh list
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem = 
    (lte_rrc_osys_CellsToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CellsToAddMod));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem[0].cellIndex = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem[0].physCellId = 257;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem[0].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_22;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[0].pci == 257);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[0].cell_offset == -22);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellAddToEmpty::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraNeighCellListFull);

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellListFull::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellListFull::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_CellsToAddModList *cur_ptr;
  int i;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Add one neigh cell to the neigh list
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem = 
    (lte_rrc_osys_CellsToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CellsToAddMod) * ASN1V_maxCellMeas);
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.n = ASN1V_maxCellMeas;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  cur_ptr = &meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList;

  for (i = 1; i <= ASN1V_maxCellMeas; i++)
  {
    cur_ptr->elem[i-1].cellIndex = i;
    cur_ptr->elem[i-1].physCellId = 5*i;
    cur_ptr->elem[i-1].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_22;

  }

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0xFFFFFFFF);

  for (i = 0; i < ASN1V_maxCellMeas; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[i].pci == (5 * (i + 1)));
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[i].cell_offset == -22);
  }

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellListFull::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraNeighCellListOverflow);

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellListOverflow::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellListOverflow::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_osys_CellsToAddModList *cur_ptr;
  int i;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Add one neigh cell to the neigh list
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem = 
    (lte_rrc_osys_CellsToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CellsToAddMod) * (ASN1V_maxCellMeas + 1));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.n = ASN1V_maxCellMeas + 1;

  cur_ptr = &meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList;

  for (i = 1; i <= ASN1V_maxCellMeas + 1; i++)
  {
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem[0].cellIndex = 1;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem[0].physCellId = 1;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem[0].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_22;

  }

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

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellListOverflow::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellEmptyList);  

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellEmptyList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellEmptyList::Test()
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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Make the black cell list empty
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.elem = NULL;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.n = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellEmptyList::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellAddToEmpty);

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellAddToEmpty::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellAddToEmpty::Test()
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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Add one black cell to the black list
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.elem = 
    (lte_rrc_osys_BlackCellsToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_BlackCellsToAddMod));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.elem[0].cellIndex = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.elem[0].physCellIdRange.m.rangePresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.elem[0].physCellIdRange.start = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 1);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.cell_info[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.cell_info[0].pci_range.end_pci == 1);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellAddToEmpty::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellListFull);

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellListFull::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellListFull::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_BlackCellsToAddModList *cur_ptr;
  int i;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Add one black cell to the black list
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.elem = 
    (lte_rrc_osys_BlackCellsToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_BlackCellsToAddMod) * ASN1V_maxCellMeas);
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.n = ASN1V_maxCellMeas;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  cur_ptr = &meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList;

  for (i = 1; i <= ASN1V_maxCellMeas; i++)
  {
    cur_ptr->elem[i-1].cellIndex = i;
    cur_ptr->elem[i-1].physCellIdRange.m.rangePresent = 0;
    cur_ptr->elem[i-1].physCellIdRange.start = 500;

  }

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0xFFFFFFFF);

  for (i = 0; i < ASN1V_maxCellMeas; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.cell_info[i].pci_range.start_pci == 500);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.cell_info[i].pci_range.end_pci == 500);
  }

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellListFull::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellListOverflow);

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellListOverflow::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellListOverflow::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_osys_BlackCellsToAddModList *cur_ptr;
  int i;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Add one black cell to the black list
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.elem = 
    (lte_rrc_osys_BlackCellsToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_BlackCellsToAddMod) * (ASN1V_maxCellMeas + 1));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.n = ASN1V_maxCellMeas + 1;

  cur_ptr = &meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList;

  for (i = 1; i <= ASN1V_maxCellMeas + 1; i++)
  {
    cur_ptr->elem[i-1].cellIndex = 1;
    cur_ptr->elem[i-1].physCellIdRange.m.rangePresent = 0;
    cur_ptr->elem[i-1].physCellIdRange.start = 1;

  }

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

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellListOverflow::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraNeighCellAddToEmptyAndRemove);

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellAddToEmptyAndRemove::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellAddToEmptyAndRemove::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Add one neigh cell to the neigh list
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem = 
    (lte_rrc_osys_CellsToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CellsToAddMod));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem[0].cellIndex = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem[0].physCellId = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem[0].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_22;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[0].cell_offset == -22);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  // Modify the meas cfg to send a EUTRA Neigh Cell Remove config
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Add one neigh cell to the neigh cell remove list
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToRemoveList.elem = 
    (lte_rrc_osys_CellIndex*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CellIndex));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToRemoveList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToRemoveList.elem[0] = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  
  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellAddToEmptyAndRemove::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraNeighCellListFullRemove);

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellListFullRemove::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellListFullRemove::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  lte_rrc_osys_CellsToAddModList *cur_ptr_addmod;
  lte_rrc_osys_CellIndexList *cur_ptr_rem;
  int i;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Add one neigh cell to the neigh list
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem = 
    (lte_rrc_osys_CellsToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CellsToAddMod) * ASN1V_maxCellMeas);
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.n = ASN1V_maxCellMeas;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  cur_ptr_addmod = &meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList;

  for (i = 1; i <= ASN1V_maxCellMeas; i++)
  {
    cur_ptr_addmod->elem[i-1].cellIndex = i;
    cur_ptr_addmod->elem[i-1].physCellId = i + 1;
    cur_ptr_addmod->elem[i-1].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_22;

  }

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0xFFFFFFFF);

  for (i = 0; i < ASN1V_maxCellMeas; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[i].pci == i + 2);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[i].cell_offset == -22);
  }

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  // Modify the meas cfg to send a EUTRA Neigh Cell Remove config
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Add one neigh cell to the neigh cell remove list
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToRemoveList.elem = 
    (lte_rrc_osys_CellIndex*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CellIndex) * ASN1V_maxCellMeas);
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToRemoveList.n = ASN1V_maxCellMeas;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  cur_ptr_rem = &meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToRemoveList;

  for (i = 1; i <= ASN1V_maxCellMeas; i++)
  {
    cur_ptr_rem->elem[i-1] = i;

  }

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  
  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraNeighCellListFullRemove::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellAddToEmptyAndRemove);

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellAddToEmptyAndRemove::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellAddToEmptyAndRemove::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Add one black cell to the black list
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.elem = 
    (lte_rrc_osys_BlackCellsToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_BlackCellsToAddMod));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.elem[0].cellIndex = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.elem[0].physCellIdRange.m.rangePresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.elem[0].physCellIdRange.start = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.cell_info[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.cell_info[0].pci_range.end_pci == 1);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  lte_rrc_asn1_osys_IE_free(meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.elem);
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.n = 0;
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  
  // Modify the meas cfg to send a EUTRA Blacklisted Cell Remove config
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Add one neigh cell to the neigh cell remove list
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToRemoveList.elem = 
    (lte_rrc_osys_CellIndex*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CellIndex));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToRemoveList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToRemoveList.elem[0] = 1;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  
  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellAddToEmptyAndRemove::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellListFullRemove);

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellListFullRemove::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellListFullRemove::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  lte_rrc_osys_BlackCellsToAddModList *cur_ptr_addmod;
  lte_rrc_osys_CellIndexList *cur_ptr_rem;
  int i;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Add one neigh cell to the neigh list
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.elem = 
    (lte_rrc_osys_BlackCellsToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_BlackCellsToAddMod) * ASN1V_maxCellMeas);
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList.n = ASN1V_maxCellMeas;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  cur_ptr_addmod = &meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToAddModList;

  for (i = 1; i <= ASN1V_maxCellMeas; i++)
  {
    cur_ptr_addmod->elem[i-1].cellIndex = i;
    cur_ptr_addmod->elem[i-1].physCellIdRange.m.rangePresent = 0;
    cur_ptr_addmod->elem[i-1].physCellIdRange.start = i + 1;

  }

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0xFFFFFFFF);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);

  for (i = 0; i < ASN1V_maxCellMeas; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.cell_info[i].pci_range.start_pci == i + 2);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.cell_info[i].pci_range.end_pci == i + 2);
  }

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  // Modify the meas cfg to send a EUTRA Blacklisted Cell Remove config
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

  // Add one neigh cell to the neigh cell remove list
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToRemoveList.elem = 
    (lte_rrc_osys_CellIndex*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CellIndex) * ASN1V_maxCellMeas);
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToRemoveList.n = ASN1V_maxCellMeas;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  cur_ptr_rem = &meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->blackCellsToRemoveList;

  for (i = 1; i <= ASN1V_maxCellMeas; i++)
  {
    cur_ptr_rem->elem[i-1] = i;

  }

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  
  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjEutraBlacklistedCellListFullRemove::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgAddToEmptyAndRemove);

void LteRrcMeasConnReportCfgAddToEmptyAndRemove::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgAddToEmptyAndRemove::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.periodical.purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to remove list
  meas_cfg->m.measGapConfigPresent = 0;
  meas_cfg->m.measIdToAddModListPresent = 0;
  meas_cfg->m.measIdToRemoveListPresent = 0;
  meas_cfg->m.measObjectToAddModListPresent = 0;
  meas_cfg->m.measObjectToRemoveListPresent = 0;
  meas_cfg->m.preRegistrationInfoHRPDPresent = 0;
  meas_cfg->m.quantityConfigPresent = 0;
  meas_cfg->m.reportConfigToAddModListPresent = 0;
  meas_cfg->m.reportConfigToRemoveListPresent = 0;
  meas_cfg->m.s_MeasurePresent = 0;
  meas_cfg->m.speedStateParsPresent = 0;
  meas_cfg->m.reportConfigToRemoveListPresent = 1;
  
  meas_cfg->reportConfigToRemoveList.elem = 
    (lte_rrc_osys_ReportConfigId*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigId));
  meas_cfg->reportConfigToRemoveList.n = 1;

  meas_cfg->reportConfigToRemoveList.elem[0] = 1;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  
  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgAddToEmptyAndRemove::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgFullListRemove);

void LteRrcMeasConnReportCfgFullListRemove::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgFullListRemove::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_ReportConfigToAddModList *cur_ptr;
  lte_rrc_osys_ReportConfigToRemoveList *cur_ptr_rem;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  int i;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the report cfg to add/mod list
  meas_cfg->m.reportConfigToAddModListPresent = 1;
  
  meas_cfg->reportConfigToAddModList.elem = 
    (lte_rrc_osys_ReportConfigToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigToAddMod) * ASN1V_maxReportConfigId);
  meas_cfg->reportConfigToAddModList.n = ASN1V_maxReportConfigId;

  cur_ptr = &meas_cfg->reportConfigToAddModList;

  for (i = 1; i <= ASN1V_maxReportConfigId; i++)
  {
    cur_ptr->elem[i-1].reportConfigId = i;
    cur_ptr->elem[i-1].reportConfig.t = T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA = 
      (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->triggerType.t = 
      T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
    cur_ptr->elem[i-1].reportConfig.u.
      reportConfigEUTRA->triggerType.u.periodical = 
      (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));
    cur_ptr->elem[i-1].reportConfig.u.
      reportConfigEUTRA->triggerType.u.periodical->purpose = 
      lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
    cur_ptr->elem[i-1].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

  }

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0xFFFFFFFF);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0xFFFFFFFF);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  for (i = 0; i < ASN1V_maxReportConfigId; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[i].report_int == 5120);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[i].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[i].trig_cfg.data.periodical.purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[i].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[i].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[i].max_cells == 6);
  }

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Remove all report configs
  meas_cfg->m.measGapConfigPresent = 0;
  meas_cfg->m.measIdToAddModListPresent = 0;
  meas_cfg->m.measIdToRemoveListPresent = 0;
  meas_cfg->m.measObjectToAddModListPresent = 0;
  meas_cfg->m.measObjectToRemoveListPresent = 0;
  meas_cfg->m.preRegistrationInfoHRPDPresent = 0;
  meas_cfg->m.quantityConfigPresent = 0;
  meas_cfg->m.reportConfigToAddModListPresent = 0;
  meas_cfg->m.reportConfigToRemoveListPresent = 0;
  meas_cfg->m.s_MeasurePresent = 0;
  meas_cfg->m.speedStateParsPresent = 0;
  meas_cfg->m.reportConfigToRemoveListPresent = 1;
  
  meas_cfg->reportConfigToRemoveList.elem = 
    (lte_rrc_osys_ReportConfigId*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigId) * ASN1V_maxReportConfigId);
  meas_cfg->reportConfigToRemoveList.n = ASN1V_maxReportConfigId;

  cur_ptr_rem = &meas_cfg->reportConfigToRemoveList;

  for (i = 1; i <= ASN1V_maxReportConfigId; i++)
  {
    cur_ptr_rem->elem[i-1] = i;
    
  }

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0xFFFFFFFF);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  
  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgFullListRemove::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraTrigQty);

void LteRrcMeasConnReportCfgEutraTrigQty::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraTrigQty::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  int i;
  lte_rrc_osys_ReportConfigEUTRA_triggerQuantity test_values[] = 
  {
    lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp,
    lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq,
  };

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = test_values[i];

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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
    TF_ASSERT(cfg_req->other_mod_bitmask == 0);

    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.periodical.purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

    switch (meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity)
    {
      case lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);
        break;

      case lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ);
        break;

      default:
        TF_ASSERT(0);
    }

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Send successful config cnf from CPHY
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the meas cfg cnf
    lte_rrc_utf_wait_for_done();

    // We expect RRC to be in the connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // We expect meas sm to have thrown the internal meas_cfg_cnfi
    lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
    TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
    TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  }

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraTrigQty::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraTrigQtyInvalid);

void LteRrcMeasConnReportCfgEutraTrigQtyInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraTrigQtyInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = (lte_rrc_osys_ReportConfigEUTRA_triggerQuantity) 2;

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

void LteRrcMeasConnReportCfgEutraTrigQtyInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraReportQty);

void LteRrcMeasConnReportCfgEutraReportQty::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraReportQty::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  int i;
  lte_rrc_osys_ReportConfigEUTRA_reportQuantity test_values[] = 
  {
    lte_rrc_osys_ReportConfigEUTRA_reportQuantity_sameAsTriggerQuantity,
    lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both,
  };

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;

  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = test_values[i];

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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
    TF_ASSERT(cfg_req->other_mod_bitmask == 0);

    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.periodical.purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);

    switch (meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity)
    {
      case lte_rrc_osys_ReportConfigEUTRA_reportQuantity_sameAsTriggerQuantity:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_SAME);
        break;

      case lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
        break;

      default:
        TF_ASSERT(0);
    }

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Send successful config cnf from CPHY
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the meas cfg cnf
    lte_rrc_utf_wait_for_done();

    // We expect RRC to be in the connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // We expect meas sm to have thrown the internal meas_cfg_cnfi
    lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
    TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
    TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  }

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraReportQty::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraReportQtyInvalid);

void LteRrcMeasConnReportCfgEutraReportQtyInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraReportQtyInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = (lte_rrc_osys_ReportConfigEUTRA_reportQuantity) 2;

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

void LteRrcMeasConnReportCfgEutraReportQtyInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraReportCellsMin);

void LteRrcMeasConnReportCfgEutraReportCellsMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraReportCellsMin::Test()
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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.periodical.purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 1);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraReportCellsMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraReportCellsMax);

void LteRrcMeasConnReportCfgEutraReportCellsMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraReportCellsMax::Test()
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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 8;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.periodical.purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 8);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraReportCellsMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraReportCellsInvalid);

void LteRrcMeasConnReportCfgEutraReportCellsInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraReportCellsInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 0;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Change the maxReportCells field to another invalid value
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 9;

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

void LteRrcMeasConnReportCfgEutraReportCellsInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraReportInterval);

void LteRrcMeasConnReportCfgEutraReportInterval::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraReportInterval::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  int i;
  lte_rrc_osys_ReportInterval test_values[] = 
  {
    lte_rrc_osys_ReportInterval_ms120,
    lte_rrc_osys_ReportInterval_ms240,
    lte_rrc_osys_ReportInterval_ms480,
    lte_rrc_osys_ReportInterval_ms640,
    lte_rrc_osys_ReportInterval_ms1024,
    lte_rrc_osys_ReportInterval_ms2048,
    lte_rrc_osys_ReportInterval_ms5120,
    lte_rrc_osys_ReportInterval_ms10240,
    lte_rrc_osys_ReportInterval_min1,
    lte_rrc_osys_ReportInterval_min6,
    lte_rrc_osys_ReportInterval_min12,
    lte_rrc_osys_ReportInterval_min30,
    lte_rrc_osys_ReportInterval_min60,
  };

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;

  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = test_values[i];

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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
    TF_ASSERT(cfg_req->other_mod_bitmask == 0);

    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.periodical.purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);

    switch (meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval)
    {
      case lte_rrc_osys_ReportInterval_ms120:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 120);
        break;

      case lte_rrc_osys_ReportInterval_ms240:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 240);
        break;

      case lte_rrc_osys_ReportInterval_ms480:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 480);
        break;

      case lte_rrc_osys_ReportInterval_ms640:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 640);
        break;

      case lte_rrc_osys_ReportInterval_ms1024:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 1024);
        break;

      case lte_rrc_osys_ReportInterval_ms2048:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 2048);
        break;

      case lte_rrc_osys_ReportInterval_ms5120:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);
        break;

      case lte_rrc_osys_ReportInterval_ms10240:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 10240);
        break;

      case lte_rrc_osys_ReportInterval_min1:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 60000);
        break;

      case lte_rrc_osys_ReportInterval_min6:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 360000);
        break;

      case lte_rrc_osys_ReportInterval_min12:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 720000);
        break;

      case lte_rrc_osys_ReportInterval_min30:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 1800000);
        break;

      case lte_rrc_osys_ReportInterval_min60:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 3600000);
        break;

      default:
        TF_ASSERT(0);
    }

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Send successful config cnf from CPHY
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the meas cfg cnf
    lte_rrc_utf_wait_for_done();

    // We expect RRC to be in the connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // We expect meas sm to have thrown the internal meas_cfg_cnfi
    lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
    TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
    TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  }

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraReportInterval::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraReportIntervalInvalid);

void LteRrcMeasConnReportCfgEutraReportIntervalInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraReportIntervalInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = (lte_rrc_osys_ReportInterval) 16;

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

void LteRrcMeasConnReportCfgEutraReportIntervalInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraReportAmount);

void LteRrcMeasConnReportCfgEutraReportAmount::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraReportAmount::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  int i;
  lte_rrc_osys_ReportConfigEUTRA_reportAmount test_values[] = 
  {
    lte_rrc_osys_ReportConfigEUTRA_reportAmount_r1,
    lte_rrc_osys_ReportConfigEUTRA_reportAmount_r2,
    lte_rrc_osys_ReportConfigEUTRA_reportAmount_r4,
    lte_rrc_osys_ReportConfigEUTRA_reportAmount_r8,
    lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16,
    lte_rrc_osys_ReportConfigEUTRA_reportAmount_r32,
    lte_rrc_osys_ReportConfigEUTRA_reportAmount_r64,
    lte_rrc_osys_ReportConfigEUTRA_reportAmount_infinity,
  };

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;

  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = test_values[i];

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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
    TF_ASSERT(cfg_req->other_mod_bitmask == 0);

    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.periodical.purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

    switch (meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount)
    {
      case lte_rrc_osys_ReportConfigEUTRA_reportAmount_r1:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_amt == 1);
        break;

      case lte_rrc_osys_ReportConfigEUTRA_reportAmount_r2:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_amt == 2);
        break;

      case lte_rrc_osys_ReportConfigEUTRA_reportAmount_r4:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_amt == 4);
        break;

      case lte_rrc_osys_ReportConfigEUTRA_reportAmount_r8:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_amt == 8);
        break;

      case lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_amt == 16);
        break;

      case lte_rrc_osys_ReportConfigEUTRA_reportAmount_r32:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_amt == 32);
        break;

      case lte_rrc_osys_ReportConfigEUTRA_reportAmount_r64:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_amt == 64);
        break;

      case lte_rrc_osys_ReportConfigEUTRA_reportAmount_infinity:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_amt == 0xFF);
        break;

      default:
        TF_ASSERT(0);
    }

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Send successful config cnf from CPHY
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the meas cfg cnf
    lte_rrc_utf_wait_for_done();

    // We expect RRC to be in the connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // We expect meas sm to have thrown the internal meas_cfg_cnfi
    lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
    TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
    TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  }

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraReportAmount::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraReportAmountInvalid);

void LteRrcMeasConnReportCfgEutraReportAmountInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraReportAmountInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = (lte_rrc_osys_ReportConfigEUTRA_reportAmount) 8;

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

void LteRrcMeasConnReportCfgEutraReportAmountInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraHystMin);

void LteRrcMeasConnReportCfgEutraHystMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraHystMin::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.t = 
    T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.t =
    T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.u.threshold_RSRP = 97;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->hysteresis = 0;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 1);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a1.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].hyst == 0);

            
  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraHystMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraHystMax);

void LteRrcMeasConnReportCfgEutraHystMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraHystMax::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.t = 
    T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.t =
    T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.u.threshold_RSRP = 97;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->hysteresis = 30;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 1);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a1.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].hyst == 30);

            
  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraHystMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraHystInvalid);

void LteRrcMeasConnReportCfgEutraHystInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraHystInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.t = 
    T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.t =
    T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.u.threshold_RSRP = 97;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->hysteresis = 31;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraHystInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraTimeToTrig);

void LteRrcMeasConnReportCfgEutraTimeToTrig::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraTimeToTrig::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  int i;
  lte_rrc_osys_TimeToTrigger test_values[] = 
  {
    lte_rrc_osys_TimeToTrigger_ms0,
    lte_rrc_osys_TimeToTrigger_ms40,
    lte_rrc_osys_TimeToTrigger_ms64,
    lte_rrc_osys_TimeToTrigger_ms80,
    lte_rrc_osys_TimeToTrigger_ms100,
    lte_rrc_osys_TimeToTrigger_ms128,
    lte_rrc_osys_TimeToTrigger_ms160,
    lte_rrc_osys_TimeToTrigger_ms256,
    lte_rrc_osys_TimeToTrigger_ms320,
    lte_rrc_osys_TimeToTrigger_ms480,
    lte_rrc_osys_TimeToTrigger_ms512,
    lte_rrc_osys_TimeToTrigger_ms640,
    lte_rrc_osys_TimeToTrigger_ms1024,
    lte_rrc_osys_TimeToTrigger_ms1280,
    lte_rrc_osys_TimeToTrigger_ms2560,
    lte_rrc_osys_TimeToTrigger_ms5120,
  };

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.t = 
    T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.t =
    T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.u.threshold_RSRP = 97;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->hysteresis = 30;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
      reportConfigEUTRA->triggerType.u.event->timeToTrigger = test_values[i];

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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
    TF_ASSERT(cfg_req->other_mod_bitmask == 0);

    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_amt == 16);

    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A1);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a1.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX);

    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].hyst == 30);

    switch (meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
            reportConfigEUTRA->triggerType.u.event->timeToTrigger)
    {
      case lte_rrc_osys_TimeToTrigger_ms0:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 0);
        break;

      case lte_rrc_osys_TimeToTrigger_ms40:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 40);
        break;

      case lte_rrc_osys_TimeToTrigger_ms64:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 64);
        break;

      case lte_rrc_osys_TimeToTrigger_ms80:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 80);
        break;

      case lte_rrc_osys_TimeToTrigger_ms100:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 100);
        break;

      case lte_rrc_osys_TimeToTrigger_ms128:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 128);
        break;

      case lte_rrc_osys_TimeToTrigger_ms160:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 160);
        break;

      case lte_rrc_osys_TimeToTrigger_ms256:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 256);
        break;

      case lte_rrc_osys_TimeToTrigger_ms320:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 320);
        break;

      case lte_rrc_osys_TimeToTrigger_ms480:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 480);
        break;

      case lte_rrc_osys_TimeToTrigger_ms512:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 512);
        break;

      case lte_rrc_osys_TimeToTrigger_ms640:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 640);
        break;

      case lte_rrc_osys_TimeToTrigger_ms1024:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 1024);
        break;

      case lte_rrc_osys_TimeToTrigger_ms1280:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 1280);
        break;

      case lte_rrc_osys_TimeToTrigger_ms2560:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 2560);
        break;

      case lte_rrc_osys_TimeToTrigger_ms5120:
        TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].time_to_trig == 5120);
        break;

      default:
        TF_ASSERT(0);
    }

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Send successful config cnf from CPHY
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the meas cfg cnf
    lte_rrc_utf_wait_for_done();

    // We expect RRC to be in the connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // We expect meas sm to have thrown the internal meas_cfg_cnfi
    lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
    TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
    TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  }

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraTimeToTrig::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraTimeToTrigInvalid);

void LteRrcMeasConnReportCfgEutraTimeToTrigInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraTimeToTrigInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.t = 
    T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA1));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.t =
    T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.u.threshold_RSRP = 97;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->hysteresis = 30;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->timeToTrigger = (lte_rrc_osys_TimeToTrigger) 16;

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

void LteRrcMeasConnReportCfgEutraTimeToTrigInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA1ThreshRsrpMin);

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrpMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrpMin::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_MIN;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a1.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRP_MIN);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrpMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA1ThreshRsrpMax);

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrpMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrpMax::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a1.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrpMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA1ThreshRsrpInvalid);

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrpInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrpInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_INVALID;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrpInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA1ThreshRsrqMin);

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrqMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrqMin::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MIN;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a1.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRQ_MIN);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrqMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA1ThreshRsrqMax);

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrqMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrqMax::Test()
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a1.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrqMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA1ThreshRsrqInvalid);

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrqInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrqInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_INVALID;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA1ThreshRsrqInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA1ThreshDiffFromTrigQtyInvalid);

void LteRrcMeasConnReportCfgEutraEventA1ThreshDiffFromTrigQtyInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA1ThreshDiffFromTrigQtyInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA1->a1_Threshold.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX;

  // Set trigger Qty to RSRQ, different from A1 threshold Qty
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA1ThreshDiffFromTrigQtyInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA2ThreshDiffFromTrigQtyInvalid);

void LteRrcMeasConnReportCfgEutraEventA2ThreshDiffFromTrigQtyInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA2ThreshDiffFromTrigQtyInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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

  // Set trigger Qty to RSRP, different from A2 threshold Qty
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA2ThreshDiffFromTrigQtyInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA2ThreshRsrpMin);

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrpMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrpMin::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA2->a2_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA2->a2_Threshold.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_MIN;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a2.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRP_MIN);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrpMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA2ThreshRsrpMax);

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrpMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrpMax::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA2->a2_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA2->a2_Threshold.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a2.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrpMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA2ThreshRsrpInvalid);

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrpInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrpInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA2->a2_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA2->a2_Threshold.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_INVALID;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrpInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA2ThreshRsrqMin);

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrqMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrqMin::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA2->a2_Threshold.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MIN;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a2.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRQ_MIN);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrqMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA2ThreshRsrqMax);

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrqMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrqMax::Test()
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a2.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrqMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA2ThreshRsrqInvalid);

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrqInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrqInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA2->a2_Threshold.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_INVALID;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA2ThreshRsrqInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA4ThreshRsrpMin);

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrpMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrpMin::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4->a4_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4->a4_Threshold.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_MIN;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A4);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a4.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRP_MIN);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrpMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA4ThreshRsrpMax);

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrpMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrpMax::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4->a4_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4->a4_Threshold.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A4);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a4.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrpMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA4ThreshRsrpInvalid);

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrpInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrpInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4->a4_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4->a4_Threshold.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_INVALID;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrpInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA4ThreshRsrqMin);

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrqMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrqMin::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4->a4_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4->a4_Threshold.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MIN;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A4);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a4.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRQ_MIN);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrqMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA4ThreshRsrqMax);

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrqMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrqMax::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4->a4_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4->a4_Threshold.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A4);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a4.thresh == LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrqMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA4ThreshRsrqInvalid);

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrqInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrqInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA4));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4->a4_Threshold.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA4->a4_Threshold.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_INVALID;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA4ThreshRsrqInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA5ThreshRsrpMin);

void LteRrcMeasConnReportCfgEutraEventA5ThreshRsrpMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA5ThreshRsrpMin::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_MIN;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_MIN;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A5);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a5.thresh1 == LTE_RRC_MEAS_TEST_THRESH_RSRP_MIN);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a5.thresh2 == LTE_RRC_MEAS_TEST_THRESH_RSRP_MIN);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA5ThreshRsrpMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA5ThreshRsrpMax);

void LteRrcMeasConnReportCfgEutraEventA5ThreshRsrpMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA5ThreshRsrpMax::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a5.thresh1 == LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a5.thresh2 == LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA5ThreshRsrpMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA5Thresh1RsrpInvalid);

void LteRrcMeasConnReportCfgEutraEventA5Thresh1RsrpInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA5Thresh1RsrpInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_INVALID;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.u.threshold_RSRP = 10;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA5Thresh1RsrpInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA5Thresh2RsrpInvalid);

void LteRrcMeasConnReportCfgEutraEventA5Thresh2RsrpInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA5Thresh2RsrpInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.u.threshold_RSRP = 10;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_INVALID;;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA5Thresh2RsrpInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA5ThreshRsrqMin);

void LteRrcMeasConnReportCfgEutraEventA5ThreshRsrqMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA5ThreshRsrqMin::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MIN;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MIN;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a5.thresh1 == LTE_RRC_MEAS_TEST_THRESH_RSRQ_MIN);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a5.thresh2 == LTE_RRC_MEAS_TEST_THRESH_RSRQ_MIN);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA5ThreshRsrqMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA5ThreshRsrqMax);

void LteRrcMeasConnReportCfgEutraEventA5ThreshRsrqMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA5ThreshRsrqMax::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a5.thresh1 == LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a5.thresh2 == LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA5ThreshRsrqMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA5ThreshDiffFromTrigQtyInvalid);

void LteRrcMeasConnReportCfgEutraEventA5ThreshDiffFromTrigQtyInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA5ThreshDiffFromTrigQtyInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX;

  // Set trigger quantity to RSRP, different from threshold Qty
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA5ThreshDiffFromTrigQtyInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA5Thresh1QtyDiffFromThresh2Invalid);

void LteRrcMeasConnReportCfgEutraEventA5Thresh1QtyDiffFromThresh2Invalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA5Thresh1QtyDiffFromThresh2Invalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRP;
  // Set threshold2 Qty to be different from threshold1 Qty 
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.u.threshold_RSRP = LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_MAX;

  // Set trigger quantity to RSRP
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA5Thresh1QtyDiffFromThresh2Invalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA5Thresh1RsrqInvalid);

void LteRrcMeasConnReportCfgEutraEventA5Thresh1RsrqInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA5Thresh1RsrqInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_INVALID;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.u.threshold_RSRQ = 10;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA5Thresh1RsrqInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA5Thresh2RsrqInvalid);

void LteRrcMeasConnReportCfgEutraEventA5Thresh2RsrqInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA5Thresh2RsrqInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA5));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold1.u.threshold_RSRQ = 10;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.t = T_lte_rrc_osys_ThresholdEUTRA_threshold_RSRQ;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA5->a5_Threshold2.u.threshold_RSRQ = LTE_RRC_MEAS_TEST_THRESH_RSRQ_INVALID;;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA5Thresh2RsrqInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA3OffsetMin);

void LteRrcMeasConnReportCfgEutraEventA3OffsetMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA3OffsetMin::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA3 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA3->a3_Offset = -30;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a3.offset == -30);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA3OffsetMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA3OffsetMax);

void LteRrcMeasConnReportCfgEutraEventA3OffsetMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA3OffsetMax::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA3 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA3->a3_Offset = 30;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a3.offset == 30);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA3OffsetMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA3OffsetInvalid1);

void LteRrcMeasConnReportCfgEutraEventA3OffsetInvalid1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA3OffsetInvalid1::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA3 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA3->a3_Offset = -31;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA3OffsetInvalid1::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA3OffsetInvalid2);

void LteRrcMeasConnReportCfgEutraEventA3OffsetInvalid2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA3OffsetInvalid2::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA3 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA3->a3_Offset = 31;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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

void LteRrcMeasConnReportCfgEutraEventA3OffsetInvalid2::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA3ReportOnLeaveTrue);

void LteRrcMeasConnReportCfgEutraEventA3ReportOnLeaveTrue::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA3ReportOnLeaveTrue::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA3 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA3->a3_Offset = 30;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA3->reportOnLeave = TRUE;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a3.offset == 30);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a3.report_on_leave == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA3ReportOnLeaveTrue::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnReportCfgEutraEventA3ReportOnLeaveFalse);

void LteRrcMeasConnReportCfgEutraEventA3ReportOnLeaveFalse::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnReportCfgEutraEventA3ReportOnLeaveFalse::Test()
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
    reportConfigEUTRA->triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA3 = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_event_eventId_eventA3));

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA3->a3_Offset = 30;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->triggerType.u.event->eventId.u.eventA3->reportOnLeave = FALSE;

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 6;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = lte_rrc_osys_ReportInterval_ms5120;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r16;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 5120);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a3.offset == 30);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.a3.report_on_leave == FALSE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);

  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 6);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnReportCfgEutraEventA3ReportOnLeaveFalse::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnQtyCfgEutraRsrpNotPresent);

void LteRrcMeasConnQtyCfgEutraRsrpNotPresent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnQtyCfgEutraRsrpNotPresent::Test()
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

  // Enable quantity cfg
  meas_cfg->m.quantityConfigPresent = 1;
  
  // Set eutra qty cfg params
  meas_cfg->quantityConfig.m.quantityConfigCDMA2000Present = 0;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigGERANPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigUTRAPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRQ = lte_rrc_osys_FilterCoefficient_fc1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP = lte_rrc_osys_FilterCoefficient_fc4;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // RSRP should be set to default
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 4);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 1);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnQtyCfgEutraRsrpNotPresent::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnQtyCfgEutraRsrqNotPresent);

void LteRrcMeasConnQtyCfgEutraRsrqNotPresent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnQtyCfgEutraRsrqNotPresent::Test()
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

  // Enable quantity cfg
  meas_cfg->m.quantityConfigPresent = 1;
  
  // Set eutra qty cfg params
  meas_cfg->quantityConfig.m.quantityConfigCDMA2000Present = 0;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigGERANPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigUTRAPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP = lte_rrc_osys_FilterCoefficient_fc1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRQ = lte_rrc_osys_FilterCoefficient_fc4;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // RSRQ should be set to default
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 4);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 1);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnQtyCfgEutraRsrqNotPresent::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnQtyCfgEutraRsrpRsrqNotPresent);

void LteRrcMeasConnQtyCfgEutraRsrpRsrqNotPresent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnQtyCfgEutraRsrpRsrqNotPresent::Test()
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

  // Enable quantity cfg
  meas_cfg->m.quantityConfigPresent = 1;
  
  // Set eutra qty cfg params
  meas_cfg->quantityConfig.m.quantityConfigCDMA2000Present = 0;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigGERANPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigUTRAPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP = lte_rrc_osys_FilterCoefficient_fc4;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRQ = lte_rrc_osys_FilterCoefficient_fc4;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // RSRP and RSRQ should be set to default
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 4);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 4);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnQtyCfgEutraRsrpRsrqNotPresent::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnQtyCfgEutraRsrp);

void LteRrcMeasConnQtyCfgEutraRsrp::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnQtyCfgEutraRsrp::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;

  int i;
  lte_rrc_osys_FilterCoefficient test_values[] = 
  {
    lte_rrc_osys_FilterCoefficient_fc0,
    lte_rrc_osys_FilterCoefficient_fc1,
    lte_rrc_osys_FilterCoefficient_fc2,
    lte_rrc_osys_FilterCoefficient_fc3,
    lte_rrc_osys_FilterCoefficient_fc4,
    lte_rrc_osys_FilterCoefficient_fc5,
    lte_rrc_osys_FilterCoefficient_fc6,
    lte_rrc_osys_FilterCoefficient_fc7,
    lte_rrc_osys_FilterCoefficient_fc8,
    lte_rrc_osys_FilterCoefficient_fc9,
    lte_rrc_osys_FilterCoefficient_fc11,
    lte_rrc_osys_FilterCoefficient_fc13,
    lte_rrc_osys_FilterCoefficient_fc15,
    lte_rrc_osys_FilterCoefficient_fc17,
    lte_rrc_osys_FilterCoefficient_fc19,
  };

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Enable quantity cfg
  meas_cfg->m.quantityConfigPresent = 1;
  
  // Set eutra qty cfg params
  meas_cfg->quantityConfig.m.quantityConfigCDMA2000Present = 0;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigGERANPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigUTRAPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;

  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP = test_values[i];
    meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRQ = lte_rrc_osys_FilterCoefficient_fc4;
  
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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
    TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

    // RSRQ should be set to default
    TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 4);

    switch (meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP)
    {
      case lte_rrc_osys_FilterCoefficient_fc0:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 0);
        break;

      case lte_rrc_osys_FilterCoefficient_fc1:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 1);
        break;

      case lte_rrc_osys_FilterCoefficient_fc2:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
        break;

      case lte_rrc_osys_FilterCoefficient_fc3:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 3);
        break;

      case lte_rrc_osys_FilterCoefficient_fc4:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 4);
        break;

      case lte_rrc_osys_FilterCoefficient_fc5:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 5);
        break;

      case lte_rrc_osys_FilterCoefficient_fc6:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 6);
        break;

      case lte_rrc_osys_FilterCoefficient_fc7:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 7);
        break;

      case lte_rrc_osys_FilterCoefficient_fc8:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 8);
        break;

      case lte_rrc_osys_FilterCoefficient_fc9:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 9);
        break;

      case lte_rrc_osys_FilterCoefficient_fc11:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 11);
        break;

      case lte_rrc_osys_FilterCoefficient_fc13:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 13);
        break;

      case lte_rrc_osys_FilterCoefficient_fc15:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 15);
        break;

      case lte_rrc_osys_FilterCoefficient_fc17:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 17);
        break;

      case lte_rrc_osys_FilterCoefficient_fc19:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 19);
        break;

      default:
        TF_ASSERT(0);
    }

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Send successful config cnf from CPHY
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the meas cfg cnf
    lte_rrc_utf_wait_for_done();

    // We expect RRC to be in the connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // We expect meas sm to have thrown the internal meas_cfg_cnfi
    lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
    TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
    TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  }

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnQtyCfgEutraRsrp::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnQtyCfgEutraRsrq);

void LteRrcMeasConnQtyCfgEutraRsrq::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnQtyCfgEutraRsrq::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;

  int i;
  lte_rrc_osys_FilterCoefficient test_values[] =
  {
    lte_rrc_osys_FilterCoefficient_fc0,
    lte_rrc_osys_FilterCoefficient_fc1,
    lte_rrc_osys_FilterCoefficient_fc2,
    lte_rrc_osys_FilterCoefficient_fc3,
    lte_rrc_osys_FilterCoefficient_fc4,
    lte_rrc_osys_FilterCoefficient_fc5,
    lte_rrc_osys_FilterCoefficient_fc6,
    lte_rrc_osys_FilterCoefficient_fc7,
    lte_rrc_osys_FilterCoefficient_fc8,
    lte_rrc_osys_FilterCoefficient_fc9,
    lte_rrc_osys_FilterCoefficient_fc11,
    lte_rrc_osys_FilterCoefficient_fc13,
    lte_rrc_osys_FilterCoefficient_fc15,
    lte_rrc_osys_FilterCoefficient_fc17,
    lte_rrc_osys_FilterCoefficient_fc19,
  };

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Enable quantity cfg
  meas_cfg->m.quantityConfigPresent = 1;
  
  // Set eutra qty cfg params
  meas_cfg->quantityConfig.m.quantityConfigCDMA2000Present = 0;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigGERANPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigUTRAPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;

  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRQ = test_values[i];
    meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP = lte_rrc_osys_FilterCoefficient_fc4;
  
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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
    TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

    // RSRQ should be set to default
    TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 4);

    switch (meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRQ)
    {
      case lte_rrc_osys_FilterCoefficient_fc0:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 0);
        break;

      case lte_rrc_osys_FilterCoefficient_fc1:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 1);
        break;

      case lte_rrc_osys_FilterCoefficient_fc2:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);
        break;

      case lte_rrc_osys_FilterCoefficient_fc3:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 3);
        break;

      case lte_rrc_osys_FilterCoefficient_fc4:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 4);
        break;

      case lte_rrc_osys_FilterCoefficient_fc5:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 5);
        break;

      case lte_rrc_osys_FilterCoefficient_fc6:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 6);
        break;

      case lte_rrc_osys_FilterCoefficient_fc7:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 7);
        break;

      case lte_rrc_osys_FilterCoefficient_fc8:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 8);
        break;

      case lte_rrc_osys_FilterCoefficient_fc9:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 9);
        break;

      case lte_rrc_osys_FilterCoefficient_fc11:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 11);
        break;

      case lte_rrc_osys_FilterCoefficient_fc13:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 13);
        break;

      case lte_rrc_osys_FilterCoefficient_fc15:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 15);
        break;

      case lte_rrc_osys_FilterCoefficient_fc17:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 17);
        break;

      case lte_rrc_osys_FilterCoefficient_fc19:
        TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 19);
        break;

      default:
        TF_ASSERT(0);
    }

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Send successful config cnf from CPHY
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the meas cfg cnf
    lte_rrc_utf_wait_for_done();

    // We expect RRC to be in the connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // We expect meas sm to have thrown the internal meas_cfg_cnfi
    lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
    TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
    TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  }

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnQtyCfgEutraRsrq::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnQtyCfgEutraRsrpInvalid);

void LteRrcMeasConnQtyCfgEutraRsrpInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnQtyCfgEutraRsrpInvalid::Test()
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

  // Enable quantity cfg
  meas_cfg->m.quantityConfigPresent = 1;
  
  // Set eutra qty cfg params
  meas_cfg->quantityConfig.m.quantityConfigCDMA2000Present = 0;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigGERANPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigUTRAPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRP = (lte_rrc_osys_FilterCoefficient) 16;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == LTE_RRC_ASN_FC_RSRP_DEFAULT);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnQtyCfgEutraRsrpInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnQtyCfgEutraRsrqInvalid);

void LteRrcMeasConnQtyCfgEutraRsrqInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnQtyCfgEutraRsrqInvalid::Test()
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

  // Enable quantity cfg
  meas_cfg->m.quantityConfigPresent = 1;
  
  // Set eutra qty cfg params
  meas_cfg->quantityConfig.m.quantityConfigCDMA2000Present = 0;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigGERANPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigUTRAPresent = 0;
  meas_cfg->quantityConfig.m.quantityConfigEUTRAPresent = 1;
  meas_cfg->quantityConfig.quantityConfigEUTRA.filterCoefficientRSRQ = (lte_rrc_osys_FilterCoefficient) 16;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == LTE_RRC_ASN_FC_RSRQ_DEFAULT);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnQtyCfgEutraRsrqInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnMeasGapSetupandRelease);

void LteRrcMeasConnMeasGapSetupandRelease::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnMeasGapSetupandRelease::Test()
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

  // Enable s-measure
  meas_cfg->m.measGapConfigPresent = 1;
  
  meas_cfg->measGapConfig.t = T_lte_rrc_osys_MeasGapConfig_setup;
  meas_cfg->measGapConfig.u.setup = (lte_rrc_osys_MeasGapConfig_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasGapConfig_setup));
  meas_cfg->measGapConfig.u.setup->gapOffset.t = T_lte_rrc_osys_MeasGapConfig_setup_gapOffset_gp1;
  meas_cfg->measGapConfig.u.setup->gapOffset.u.gp0 = 79;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_GAP_CFG);

  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.gap_cfg_enabled == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.pattern_id == LTE_CPHY_MEAS_GAP_PATTERN_ID_1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.gap_offset == 79);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // Populate the next measConfig that removes neighbor cells that were added previously
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Enable s-measure
  meas_cfg->m.measGapConfigPresent = 1;
  
  meas_cfg->measGapConfig.t = T_lte_rrc_osys_MeasGapConfig_release;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_GAP_CFG);

  // Verify if gap config is disabled
  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.gap_cfg_enabled == FALSE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnMeasGapSetupandRelease::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnMeasGapSetupandHandover);

void LteRrcMeasConnMeasGapSetupandHandover::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnMeasGapSetupandHandover::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;

  uint32 buf_len;

  // Configure connected mode measurements in all RATs
  //LteRrcMeasConnAllRatMeasConfig__Test();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Enable s-measure
  meas_cfg->m.measGapConfigPresent = 1;
  
  meas_cfg->measGapConfig.t = T_lte_rrc_osys_MeasGapConfig_setup;
  meas_cfg->measGapConfig.u.setup = (lte_rrc_osys_MeasGapConfig_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasGapConfig_setup));
  meas_cfg->measGapConfig.u.setup->gapOffset.t = T_lte_rrc_osys_MeasGapConfig_setup_gapOffset_gp0;
  meas_cfg->measGapConfig.u.setup->gapOffset.u.gp0 = 39;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x3FD);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x3FF);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x7F);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_GAP_CFG);

  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.gap_cfg_enabled == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.pattern_id == LTE_CPHY_MEAS_GAP_PATTERN_ID_0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.gap_offset == 39);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

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
  meas_cfg_reqi.cell_id.phy_cell_id = 0x10 ;
  meas_cfg_reqi.cell_id.freq = 0x100;
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x37D);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x37F);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x7F);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x80);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x80);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_GAP_CFG);

  // Verify if gap config is disabled
  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.gap_cfg_enabled == FALSE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnMeasGapSetupandHandover::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnMeasGapSetupandReestablishment);

void LteRrcMeasConnMeasGapSetupandReestablishment::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnMeasGapSetupandReestablishment::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  uint32 buf_len;

  // Configure connected mode measurements in all RATs
  //LteRrcMeasConnAllRatMeasConfig__Test();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Enable s-measure
  meas_cfg->m.measGapConfigPresent = 1;
  
  meas_cfg->measGapConfig.t = T_lte_rrc_osys_MeasGapConfig_setup;
  meas_cfg->measGapConfig.u.setup = (lte_rrc_osys_MeasGapConfig_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasGapConfig_setup));
  meas_cfg->measGapConfig.u.setup->gapOffset.t = T_lte_rrc_osys_MeasGapConfig_setup_gapOffset_gp1;
  meas_cfg->measGapConfig.u.setup->gapOffset.u.gp0 = 50;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x3FD);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x3FF);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x7F);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_GAP_CFG);

  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.gap_cfg_enabled == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.pattern_id == LTE_CPHY_MEAS_GAP_PATTERN_ID_1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.gap_offset == 50);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // Send the meas config req to meas sm with handover cause
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = NULL;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_REAPPLY_CONFIG;
  meas_cfg_reqi.cell_id.phy_cell_id = 0x10 ;
  meas_cfg_reqi.cell_id.freq = 0x100;
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x37D);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x37F);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x7F);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x37D);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x37F);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x7F);
  TF_ASSERT(cfg_req->other_mod_bitmask == (LTE_CPHY_MEAS_CFG_BIT_QTY_CFG |
                                           LTE_CPHY_MEAS_CFG_BIT_GAP_CFG));

  // Verify if gap config is disabled
  TF_ASSERT(cfg_req->meas_cfg_ptr->gap_cfg.gap_cfg_enabled == FALSE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnMeasGapSetupandReestablishment::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSMeasureMin);

void LteRrcMeasConnSMeasureMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSMeasureMin::Test()
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

  // Enable s-measure
  meas_cfg->m.s_MeasurePresent = 1;
  
  meas_cfg->s_Measure = 1;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_S_MEASURE);

  TF_ASSERT(cfg_req->meas_cfg_ptr->s_measure == -140);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnSMeasureMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}




TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSMeasureMax);

void LteRrcMeasConnSMeasureMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSMeasureMax::Test()
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

  // Enable s-measure
  meas_cfg->m.s_MeasurePresent = 1;
  
  meas_cfg->s_Measure = LTE_RRC_MEAS_TEST_THRESH_RSRP_MAX;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_S_MEASURE);

  TF_ASSERT(cfg_req->meas_cfg_ptr->s_measure == -44);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnSMeasureMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}




TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSMeasureInvalid);

void LteRrcMeasConnSMeasureInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSMeasureInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Enable s-measure
  meas_cfg->m.s_MeasurePresent = 1;
  
  meas_cfg->s_Measure = LTE_RRC_MEAS_TEST_THRESH_RSRP_INVALID;
  
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

void LteRrcMeasConnSMeasureInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingDisabled);

void LteRrcMeasConnSpeedScalingDisabled::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingDisabled::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_meas_cfg_cnfi_s *meas_cfg_cnfi;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // Disable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_release;
  
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

void LteRrcMeasConnSpeedScalingDisabled::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingTEval);

void LteRrcMeasConnSpeedScalingTEval::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingTEval::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;

  uint32 buf_len;

  int i;
  lte_rrc_osys_MobilityStateParameters_t_Evaluation test_values[] =
  {
    lte_rrc_osys_MobilityStateParameters_t_Evaluation_s30,
    lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60,
    lte_rrc_osys_MobilityStateParameters_t_Evaluation_s120,
    lte_rrc_osys_MobilityStateParameters_t_Evaluation_s180,
    lte_rrc_osys_MobilityStateParameters_t_Evaluation_s240,
  };

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // Enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = 1;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;

  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = test_values[i];
  
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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
    TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_SPEED_SCALING);

    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.enabled == TRUE);

    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_hyst_normal == 60);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_medium == 1);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_high == 1);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

    switch (meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation)
    {
      case lte_rrc_osys_MobilityStateParameters_t_Evaluation_s30:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_eval == 30);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_eval == 60);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_Evaluation_s120:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_eval == 120);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_Evaluation_s180:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_eval == 180);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_Evaluation_s240:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_eval == 240);
        break;

      default:
        TF_ASSERT(0);
    }

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Send successful config cnf from CPHY
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the meas cfg cnf
    lte_rrc_utf_wait_for_done();

    // We expect RRC to be in the connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // We expect meas sm to have thrown the internal meas_cfg_cnfi
    lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
    TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
    TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  }

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnSpeedScalingTEval::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}




TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingTEvalInvalid);

void LteRrcMeasConnSpeedScalingTEvalInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingTEvalInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = 1;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;

  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = (lte_rrc_osys_MobilityStateParameters_t_Evaluation) 8;
  
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

void LteRrcMeasConnSpeedScalingTEvalInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}




TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingTHystNormal);

void LteRrcMeasConnSpeedScalingTHystNormal::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingTHystNormal::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;

  uint32 buf_len;

  int i;
  lte_rrc_osys_MobilityStateParameters_t_HystNormal test_values[] =
  {
    lte_rrc_osys_MobilityStateParameters_t_HystNormal_s30,
    lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60,
    lte_rrc_osys_MobilityStateParameters_t_HystNormal_s120,
    lte_rrc_osys_MobilityStateParameters_t_HystNormal_s180,
    lte_rrc_osys_MobilityStateParameters_t_HystNormal_s240,
  };

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // Enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = 1;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;

  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = test_values[i];
  
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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
    TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_SPEED_SCALING);

    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.enabled == TRUE);

    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_eval == 60);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_medium == 1);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_high == 1);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

    switch (meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal)
    {
      case lte_rrc_osys_MobilityStateParameters_t_HystNormal_s30:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_hyst_normal == 30);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_hyst_normal == 60);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_HystNormal_s120:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_hyst_normal == 120);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_HystNormal_s180:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_hyst_normal == 180);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_HystNormal_s240:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_hyst_normal == 240);
        break;

      default:
        TF_ASSERT(0);
    }

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Send successful config cnf from CPHY
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the meas cfg cnf
    lte_rrc_utf_wait_for_done();

    // We expect RRC to be in the connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // We expect meas sm to have thrown the internal meas_cfg_cnfi
    lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
    TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
    TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  }

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnSpeedScalingTHystNormal::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingTHystNormalInvalid);

void LteRrcMeasConnSpeedScalingTHystNormalInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingTHystNormalInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = 1;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;

  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = (lte_rrc_osys_MobilityStateParameters_t_HystNormal) 8;
  
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

void LteRrcMeasConnSpeedScalingTHystNormalInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingNCellChangeMediumMin);

void LteRrcMeasConnSpeedScalingNCellChangeMediumMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingNCellChangeMediumMin::Test()
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

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));

  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = LTE_RRC_MEAS_TEST_N_CELL_CHANGE_MIN;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_SPEED_SCALING);

  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.enabled == TRUE);

  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_medium == LTE_RRC_MEAS_TEST_N_CELL_CHANGE_MIN);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_high == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnSpeedScalingNCellChangeMediumMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingNCellChangeMediumMax);

void LteRrcMeasConnSpeedScalingNCellChangeMediumMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingNCellChangeMediumMax::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;

  uint32 buf_len;

  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));

  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = LTE_RRC_MEAS_TEST_N_CELL_CHANGE_MAX;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_SPEED_SCALING);

  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.enabled == TRUE);

  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_medium == LTE_RRC_MEAS_TEST_N_CELL_CHANGE_MAX);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_high == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnSpeedScalingNCellChangeMediumMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingNCellChangeMediumInvalid1);

void LteRrcMeasConnSpeedScalingNCellChangeMediumInvalid1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingNCellChangeMediumInvalid1::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));

  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = 0;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;
  
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

void LteRrcMeasConnSpeedScalingNCellChangeMediumInvalid1::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}




TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingNCellChangeMediumInvalid2);

void LteRrcMeasConnSpeedScalingNCellChangeMediumInvalid2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingNCellChangeMediumInvalid2::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));

  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = 17;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;
  
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

void LteRrcMeasConnSpeedScalingNCellChangeMediumInvalid2::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingNCellChangeHighMin);

void LteRrcMeasConnSpeedScalingNCellChangeHighMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingNCellChangeHighMin::Test()
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

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));

  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = LTE_RRC_MEAS_TEST_N_CELL_CHANGE_MIN;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_SPEED_SCALING);

  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.enabled == TRUE);

  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_high == LTE_RRC_MEAS_TEST_N_CELL_CHANGE_MIN);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_medium == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnSpeedScalingNCellChangeHighMin::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingNCellChangeHighMax);

void LteRrcMeasConnSpeedScalingNCellChangeHighMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingNCellChangeHighMax::Test()
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

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));

  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = LTE_RRC_MEAS_TEST_N_CELL_CHANGE_MAX;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_SPEED_SCALING);

  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.enabled == TRUE);

  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_high == LTE_RRC_MEAS_TEST_N_CELL_CHANGE_MAX);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_medium == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnSpeedScalingNCellChangeHighMax::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingNCellChangeHighInvalid1);

void LteRrcMeasConnSpeedScalingNCellChangeHighInvalid1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingNCellChangeHighInvalid1::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));

  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = 0;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;
  
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

void LteRrcMeasConnSpeedScalingNCellChangeHighInvalid1::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}




TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingNCellChangeHighInvalid2);

void LteRrcMeasConnSpeedScalingNCellChangeHighInvalid2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingNCellChangeHighInvalid2::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));

  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = 17;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;
  
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

void LteRrcMeasConnSpeedScalingNCellChangeHighInvalid2::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingTimeToTrigMedium);

void LteRrcMeasConnSpeedScalingTimeToTrigMedium::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingTimeToTrigMedium::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;

  uint32 buf_len;

  int i;
  lte_rrc_osys_SpeedStateScaleFactors_sf_Medium test_values[] =
  {
    lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot25,
    lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5,
    lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot75,
    lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_lDot0
  };

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // Enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = 1;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;

  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = test_values[i];
  
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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
    TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_SPEED_SCALING);

    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.enabled == TRUE);

    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_eval == 60);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_hyst_normal == 60);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_medium == 1);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_high == 1);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

    switch (meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium)
    {
      case lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot25:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot75:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT75);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_lDot0:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_medium == LTE_CPHY_MEAS_TIME_SF_1DOT0);
        break;

      default:
        TF_ASSERT(0);
    }

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Send successful config cnf from CPHY
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the meas cfg cnf
    lte_rrc_utf_wait_for_done();

    // We expect RRC to be in the connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // We expect meas sm to have thrown the internal meas_cfg_cnfi
    lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
    TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
    TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  }

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnSpeedScalingTimeToTrigMedium::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingTimeToTrigMediumInvalid);

void LteRrcMeasConnSpeedScalingTimeToTrigMediumInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingTimeToTrigMediumInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = 1;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;

  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = (lte_rrc_osys_SpeedStateScaleFactors_sf_Medium) 4;
  
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

void LteRrcMeasConnSpeedScalingTimeToTrigMediumInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingTimeToTrigHigh);

void LteRrcMeasConnSpeedScalingTimeToTrigHigh::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingTimeToTrigHigh::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;

  uint32 buf_len;

  int i;
  lte_rrc_osys_SpeedStateScaleFactors_sf_High test_values[] =
  {
    lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot25,
    lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5,
    lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot75,
    lte_rrc_osys_SpeedStateScaleFactors_sf_High_lDot0
  };

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // Enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = 1;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5;

  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = test_values[i];
  
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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
    TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_SPEED_SCALING);

    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.enabled == TRUE);

    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_eval == 60);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.t_hyst_normal == 60);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_medium == 1);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.config_params.n_cell_change_high == 1);
    TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);

    switch (meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High)
    {
      case lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot25:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot75:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_high == LTE_CPHY_MEAS_TIME_SF_0DOT75);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_High_lDot0:
        TF_ASSERT(cfg_req->meas_cfg_ptr->sc_cfg.time_to_trig_sf_high == LTE_CPHY_MEAS_TIME_SF_1DOT0);
        break;

      default:
        TF_ASSERT(0);
    }

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Send successful config cnf from CPHY
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the meas cfg cnf
    lte_rrc_utf_wait_for_done();

    // We expect RRC to be in the connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // We expect meas sm to have thrown the internal meas_cfg_cnfi
    lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
    TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
    TF_ASSERT(cfg_cnfi->status == E_SUCCESS);
  }

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnSpeedScalingTimeToTrigHigh::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnSpeedScalingTimeToTrigHighInvalid);

void LteRrcMeasConnSpeedScalingTimeToTrigHighInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnSpeedScalingTimeToTrigHighInvalid::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Include speed scaling
  meas_cfg->m.speedStateParsPresent = 1;
  
  // enable speed scaling
  meas_cfg->speedStatePars.t = T_lte_rrc_osys_MeasConfig_speedStatePars_setup;
  meas_cfg->speedStatePars.u.setup = (lte_rrc_osys_MeasConfig_speedStatePars_setup *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasConfig_speedStatePars_setup));
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_Evaluation = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.t_HystNormal = lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeMedium = 1;
  meas_cfg->speedStatePars.u.setup->mobilityStateParameters.n_CellChangeHigh = 1;
  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_Medium = lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5;

  meas_cfg->speedStatePars.u.setup->timeToTrigger_SF.sf_High = (lte_rrc_osys_SpeedStateScaleFactors_sf_High) 4;
  
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

void LteRrcMeasConnSpeedScalingTimeToTrigHighInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqHandoverFailureWithMeas);

void LteRrcMeasConnIntraFreqHandoverFailureWithMeas::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqHandoverFailureWithMeas::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));

  // Free the allocated memory
  lte_rrc_asn1_osys_IE_free(meas_cfg->measObjectToAddModList.elem);
  meas_cfg->measObjectToAddModList.n = 0;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // Add an item to the meas obj to add/mod list now with handover
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod));
  meas_cfg->measObjectToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 2;
  meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 101;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  // We are now doing handover... with meas
  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    TRUE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x3);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x2);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.carrier_freq == 101);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[1].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // Handover Failure happended. CRE will send meas_cfg_reqi to take
  // ML back to meas from the source cell

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_REVERT_CONFIG;
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqHandoverFailureWithMeas::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqHandoverFailureWithoutMeas);

void LteRrcMeasConnIntraFreqHandoverFailureWithoutMeas::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqHandoverFailureWithoutMeas::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  // Add a item to the meas report 
  meas_cfg->m.reportConfigToAddModListPresent = 1;

  meas_cfg->reportConfigToAddModList.elem = 
    (lte_rrc_osys_ReportConfigToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigToAddMod));

  meas_cfg->reportConfigToAddModList.n = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfigId = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.t = T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA = 
    (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.t = 
    T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.u.periodical = 
    (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.u.periodical->purpose = 
    lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 3;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAmount = 
    lte_rrc_osys_ReportConfigEUTRA_reportAmount_r32;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval = 
    lte_rrc_osys_ReportInterval_ms1024;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity = 
    lte_rrc_osys_ReportConfigEUTRA_reportQuantity_sameAsTriggerQuantity;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity = 
    lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrq;

  // Add an item for measurement id
  meas_cfg->m.measIdToAddModListPresent = 1;
  meas_cfg->measIdToAddModList.elem = 
    (lte_rrc_osys_MeasIdToAddMod*)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod));
  meas_cfg->measIdToAddModList.n = 1;

  meas_cfg->measIdToAddModList.elem[0].measId = 1;
  meas_cfg->measIdToAddModList.elem[0].measObjectId = 1;
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Validate meas objects
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // Validate meas report config
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.periodical.purpose == 
            LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ );
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_amt == 32);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 1024);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_SAME);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ);

  // Validate meas id
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].meas_obj_id == 1);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[0].report_cfg_id == 1);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

   // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_empty_reconfig_msg,
                                       sizeof(lte_rrc_meas_test_empty_reconfig_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    TRUE) == E_SUCCESS);

  // We are now doing handover... with no meas
  // Send the meas config req to meas sm indicating handover with no meas
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_HANDOVER;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Expect that all meas id that have periodic trigger to be deleted
  
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send CPHY_MEAS_CFG_CNF
  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Handover Failure happended. CRE will send meas_cfg_reqi to take
  // ML back to meas from the source cell

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_REAPPLY_CONFIG;
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Validate meas objects
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // Validate meas report config
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.periodical.purpose == 
            LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ );
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_amt == 32);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 1024);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_SAME);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ);


  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqHandoverFailureWithoutMeas::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqRlfWithMeas);

void LteRrcMeasConnIntraFreqRlfWithMeas::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqRlfWithMeas::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;

  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

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
  TF_ASSERT(cfg_req != NULL);
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

  // RLF happended
  // CRE will send meas_cfg_reqi to reapply the meas cfg to ML

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_REAPPLY_CONFIG;
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqRlfWithMeas::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqRlfWithoutMeas);

void LteRrcMeasConnIntraFreqRlfWithoutMeas::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqRlfWithoutMeas::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_meas_cfg_cnfi_s *meas_cfg_cnfi_ptr = NULL;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // RLF happended
  // CRE will send meas_cfg_reqi to reapply the meas cfg to ML

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_REAPPLY_CONFIG;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi_ptr
  lte_rrc_utf_get_next_int_msg((byte**)&meas_cfg_cnfi_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(meas_cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(meas_cfg_cnfi_ptr->status == E_SUCCESS);

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
}

void LteRrcMeasConnIntraFreqRlfWithoutMeas::Teardown()
{
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig with WCDMA measObj, when WCDMA 
  measurements are not supported */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIratWCDMANotSupported);

void LteRrcMeasConnIratWCDMANotSupported::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, 0, 0);
}

void LteRrcMeasConnIratWCDMANotSupported::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  /* Make sure capabilities returns not supported for the IRAT measurements */
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_meas_test_stub__get_fgi_bit_not_supported;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_wcdma_msg,
                                       sizeof(lte_rrc_meas_test_meas_config_wcdma_msg),
                                       &meas_cfg);

  // Verify if meas cfg validation fails
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);

  // We expect Meas to reamin in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIratWCDMANotSupported::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig with GSM measObj, when GSM 
  measurements are not supported */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIratGSMNotSupported);

void LteRrcMeasConnIratGSMNotSupported::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, 0, 0);
}

void LteRrcMeasConnIratGSMNotSupported::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  /* Make sure capabilities returns not supported for the IRAT measurements */
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_meas_test_stub__get_fgi_bit_not_supported;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_gsm_msg,
                                       sizeof(lte_rrc_meas_test_meas_config_gsm_msg),
                                       &meas_cfg);

  // Verify if meas cfg validation fails
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);

  // We expect Meas to reamin in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIratGSMNotSupported::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig with CDMA measObj, when CDMA 
  measurements are not supported */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIratCDMANotSupported);

void LteRrcMeasConnIratCDMANotSupported::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, 0, 0);
}

void LteRrcMeasConnIratCDMANotSupported::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  /* Make sure capabilities returns not supported for the IRAT measurements */
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_meas_test_stub__get_fgi_bit_not_supported;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_cdma_msg,
                                       sizeof(lte_rrc_meas_test_meas_config_cdma_msg),
                                       &meas_cfg);

  // Verify if meas cfg validation fails
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);

  // We expect Meas to reamin in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIratCDMANotSupported::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig with EUTRA periodical measurements, 
  when it is not supported */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraPeriodicalMeasNotSupported);

void LteRrcMeasConnEutraPeriodicalMeasNotSupported::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, 0, 0);
}

void LteRrcMeasConnEutraPeriodicalMeasNotSupported::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  /* Make sure capabilities returns not supported for the IRAT measurements */
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_meas_test_stub__get_fgi_bit_not_supported;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_periodical_msg,
                                       sizeof(lte_rrc_meas_test_eutra_periodical_msg),
                                       &meas_cfg);

  // Verify if meas cfg validation fails
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);

  // We expect Meas to reamin in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnEutraPeriodicalMeasNotSupported::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig with IRAT periodical measurements, 
  when it is not supported */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIratPeriodicalMeasNotSupported);

void LteRrcMeasConnIratPeriodicalMeasNotSupported::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, 0, 0);
}

void LteRrcMeasConnIratPeriodicalMeasNotSupported::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  /* Make sure capabilities returns not supported for the IRAT measurements */
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_meas_test_stub__get_fgi_bit_not_supported;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_irat_periodical_msg,
                                       sizeof(lte_rrc_meas_test_irat_periodical_msg),
                                       &meas_cfg);

  // Verify if meas cfg validation fails
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);

  // We expect Meas to reamin in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIratPeriodicalMeasNotSupported::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig with IRAT event B1 measurements, 
  when it is not supported */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIratEventB1MeasNotSupported);

void LteRrcMeasConnIratEventB1MeasNotSupported::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, 0, 0);
}

void LteRrcMeasConnIratEventB1MeasNotSupported::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  /* Make sure capabilities returns not supported for the IRAT measurements */
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_meas_test_stub__get_fgi_bit_not_supported;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_irat_event_b1_msg,
                                       sizeof(lte_rrc_meas_test_irat_event_b1_msg),
                                       &meas_cfg);

  // Verify if meas cfg validation fails
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);

  // We expect Meas to reamin in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIratEventB1MeasNotSupported::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig with EUTRA A4 measurements, 
  when it is not supported */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraEventA4MeasNotSupported);

void LteRrcMeasConnEutraEventA4MeasNotSupported::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, 0, 0);
}

void LteRrcMeasConnEutraEventA4MeasNotSupported::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  /* Make sure capabilities returns not supported for the IRAT measurements */
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_meas_test_stub__get_fgi_bit_not_supported;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_event_a4_msg,
                                       sizeof(lte_rrc_meas_test_eutra_event_a4_msg),
                                       &meas_cfg);

  // Verify if meas cfg validation fails
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);

  // We expect Meas to reamin in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnEutraEventA4MeasNotSupported::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig with EUTRA A5 measurements, 
  when it is not supported */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraEventA5MeasNotSupported);

void LteRrcMeasConnEutraEventA5MeasNotSupported::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, 0, 0);
}

void LteRrcMeasConnEutraEventA5MeasNotSupported::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  /* Make sure capabilities returns not supported for the IRAT measurements */
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_meas_test_stub__get_fgi_bit_not_supported;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_event_a5_msg,
                                       sizeof(lte_rrc_meas_test_eutra_event_a5_msg),
                                       &meas_cfg);

  // Verify if meas cfg validation fails
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);

  // We expect Meas to reamin in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnEutraEventA5MeasNotSupported::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

#if 0
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqB1ReportConfiguredInvalid);

void LteRrcMeasConnIntraFreqB1ReportConfiguredInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqB1ReportConfiguredInvalid::Test()
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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 0x100;
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
    triggerType.u.event->eventId.t = T_lte_rrc_osys_ReportConfigInterRAT_triggerType_event_eventId_eventB1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.event->eventId.u.eventB1 = (lte_rrc_osys_ReportConfigInterRAT_triggerType_event_eventId_eventB1 *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigInterRAT_triggerType_event_eventId_eventB1));
  // Set the B1 threshold to CDMA 2000
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.event->eventId.u.eventB1->b1_Threshold.t = T_lte_rrc_osys_ReportConfigInterRAT_triggerType_event_eventId_eventB1_b1_Threshold_b1_ThresholdCDMA2000;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.event->eventId.u.eventB1->b1_Threshold.u.b1_ThresholdCDMA2000 = 50;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.event->hysteresis = 14;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.event->timeToTrigger = lte_rrc_osys_TimeToTrigger_ms480;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->maxReportCells = 2;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    reportAmount = lte_rrc_osys_ReportConfigInterRAT_reportAmount_r8;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    reportInterval = lte_rrc_osys_ReportInterval_ms640;

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

void LteRrcMeasConnIntraFreqB1ReportConfiguredInvalid::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}
#endif

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjDuplicateDeletion);

void LteRrcMeasConnIntraFreqMeasObjDuplicateDeletion::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjDuplicateDeletion::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);


  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to remove list
  meas_cfg->m.measObjectToRemoveListPresent = 1;
  
  meas_cfg->measObjectToRemoveList.elem = 
    (lte_rrc_osys_MeasObjectId*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectId) * 2);
  meas_cfg->measObjectToRemoveList.n = 2;
  // Send a message that tries to delete the same measObj id twice
  meas_cfg->measObjectToRemoveList.elem[0] = 1;
  meas_cfg->measObjectToRemoveList.elem[1] = 1;

  // Validate meas cfg, trying to delete a non-existant meas obj should be ignored
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->msg_hdr.id == LTE_RRC_MEAS_CFG_CNFI);
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);



  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjDuplicateDeletion::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjWithDupListedCells);

void LteRrcMeasConnIntraFreqMeasObjWithDupListedCells::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjWithDupListedCells::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;

  uint32 buf_len, i;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Iteration 1, verify handling of duplicate listed cells within the same OTA message
  // Iteration 2, verify handling of duplicate listed cells across OTA messages
  for (i = 0; i < 2; i++)
  {
    // Point the meas_config to be returned to the typical meas_config
    lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                         sizeof(lte_rrc_meas_test_meas_config_msg1),
                                         &meas_cfg);

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
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 1;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 100;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

      // Add listed cells with duplicate entries
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem = 
      (lte_rrc_osys_CellsToAddMod*)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CellsToAddMod) * 3);
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.n = 3;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[0].cellIndex = 1;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[0].physCellId = 100;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[0].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_22;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[1].cellIndex = 2;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[1].physCellId = 120;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[1].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_22;

    if (i == 0)
    {
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
        elem[2].cellIndex = 3;
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
        elem[2].physCellId = 100;
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
        elem[2].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_22;
    }
    else
    {
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
        elem[2].cellIndex = 3;
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
        elem[2].physCellId = 130;
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
        elem[2].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_22;
    }

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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
    TF_ASSERT(cfg_req->other_mod_bitmask == 0);

    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.presence_antenna_port1 == TRUE);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);

    if (i == 0)
    {
      // Only cell index 1 and 2 are valid, the index 3 is a duplicate and should be skipped
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 3);
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[0].pci == 100);
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[1].pci == 120);
      
    }
    else
    {
      // Only cell index 1,2 and 3 are valid, the rest are duplicates and should be skipped
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 7);
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[0].pci == 100);
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[1].pci == 120);
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[2].pci == 130);
    }
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Test Meas cfg cnf processing
    lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

    // Check for no further messages from meas
    lte_rrc_utf_check_no_more_msgs();
    // Free allocated memory for reconfig message
    lte_rrc_meas_test_free_reconfig_msg_ptr();

  }

}

void LteRrcMeasConnIntraFreqMeasObjWithDupListedCells::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjMaxListedCellsWithDupEntries);

void LteRrcMeasConnIntraFreqMeasObjMaxListedCellsWithDupEntries::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjMaxListedCellsWithDupEntries::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;

  uint32 buf_len, i;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 100;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

    // Add listed cells with duplicate entries
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem = 
    (lte_rrc_osys_CellsToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CellsToAddMod) * 32);
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.n = 32;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  for (i = 0; i < 32; i++)
  {
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[i].cellIndex = (i + 1);
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[i].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_22;

    /* Add listed cells */
    if (i == 31)
    {
      // Make the last listed cell entry a duplicate
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[i].physCellId = (i - 1);
    }
    else
    {
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[i].physCellId = i;
    }
  }

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);

  // All indexes except cell index 32 should be valid
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0x7FFFFFFF);
  for (i = 0; i < 31; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[i].pci == i);
  }

  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Test Meas cfg cnf processing
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjMaxListedCellsWithDupEntries::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqReconfurationOfListedCells);

void LteRrcMeasConnIntraFreqReconfurationOfListedCells::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqReconfurationOfListedCells::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;

  uint32 buf_len, i;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // In the first iteration, listed cells are configured for the first time
  // In the second iteration, configured listed cells are reconfigured with different offsets
  for (i = 0; i < 2; i++)
  {
    // Point the meas_config to be returned to the typical meas_config
    lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                         sizeof(lte_rrc_meas_test_meas_config_msg1),
                                         &meas_cfg);

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
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 1;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 100;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

      // Add listed cells with duplicate entries
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem = 
      (lte_rrc_osys_CellsToAddMod*)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CellsToAddMod) * 2);
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.n = 2;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[0].cellIndex = 1;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[0].physCellId = 100;
    if (i == 0)
    {
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
        elem[0].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_22;
    }
    else
    {
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
        elem[0].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_18;
    }
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[1].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_22;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[1].cellIndex = 2;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[1].physCellId = 120;

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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
    TF_ASSERT(cfg_req->other_mod_bitmask == 0);

    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.presence_antenna_port1 == TRUE);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
    // Only cell index 1 and 2 are configured
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 3);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[0].pci == 100);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[1].pci == 120);

    if (i == 0)
    {
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[0].cell_offset == -22); 
    }
    else
    {
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[0].cell_offset == -18);
    }    
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[1].cell_offset == -22);

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Test Meas cfg cnf processing
    lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

    // Check for no further messages from meas
    lte_rrc_utf_check_no_more_msgs();
    // Free allocated memory for reconfig message
    lte_rrc_meas_test_free_reconfig_msg_ptr();

  }

}

void LteRrcMeasConnIntraFreqReconfurationOfListedCells::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqReconfurationOfCellIndexInListedCells);

void LteRrcMeasConnIntraFreqReconfurationOfCellIndexInListedCells::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqReconfurationOfCellIndexInListedCells::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;

  uint32 buf_len, i;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // In the first iteration, listed cells are configured for the first time
  // In the second iteration, configured listed cells are reconfigured with different cell index
  for (i = 0; i < 2; i++)
  {
    // Point the meas_config to be returned to the typical meas_config
    lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                         sizeof(lte_rrc_meas_test_meas_config_msg1),
                                         &meas_cfg);

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
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 1;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 100;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

      // Add listed cells with duplicate entries
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.elem = 
      (lte_rrc_osys_CellsToAddMod*)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CellsToAddMod) * 2);
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.n = 2;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[0].physCellId = 100;

    if (i == 0)
    {
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
        elem[0].cellIndex = 1;
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
        elem[0].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_22;
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
        elem[1].cellIndex = 2;
    }
    else
    {
      // Reconfigure pci 100 to be in cell index 2
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
        elem[0].cellIndex = 2;
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
        elem[0].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_18;
      // Reconfigure pci 120 to be in cell index 3
      meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
        elem[1].cellIndex = 3;
    }
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[1].cellIndividualOffset = lte_rrc_osys_Q_OffsetRange_dB_22;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellsToAddModList.
      elem[1].physCellId = 120;

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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
    TF_ASSERT(cfg_req->other_mod_bitmask == 0);

    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.presence_antenna_port1 == TRUE);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);

    if (i == 0)
    {
      // Only cell index 1 and 2 are configured
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 3);
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[0].pci == 100);
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[1].pci == 120);
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[0].cell_offset == -22); 
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[1].cell_offset == -22);
    }
    else
    {
      // Now cell index 2 and 3 are configured, cell index one should be disabled
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 5);
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[0].pci == 100);
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[0].cell_offset == -22);
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[2].pci == 120);
      TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.cell_info[2].cell_offset == -22);
    }    

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Test Meas cfg cnf processing
    lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

    // Check for no further messages from meas
    lte_rrc_utf_check_no_more_msgs();
    // Free allocated memory for reconfig message
    lte_rrc_meas_test_free_reconfig_msg_ptr();

  }

}

void LteRrcMeasConnIntraFreqReconfurationOfCellIndexInListedCells::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasObjAddInLoopAcrossConnections);

void LteRrcMeasConnIntraFreqMeasObjAddInLoopAcrossConnections::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasObjAddInLoopAcrossConnections::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_conn_released_indi_s conn_release_indi;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_connected_indi_s connected_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  uint32 buf_len, i = 0;

  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Do Connected->Idle->Connected transition 256 times, each time adding a measObj
  for (i = 0; i < 256; i++)
  {

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

    TF_MSG("Sending Connected indication");
    lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
    lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

    lte_rrc_utf_wait_for_done();

    // Point the meas_config to be returned to the typical meas_config
    lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                         sizeof(lte_rrc_meas_test_meas_config_msg1),
                                         &meas_cfg);

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
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
    meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

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
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
    TF_ASSERT(cfg_req->other_mod_bitmask == 0);

    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 100);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.presence_antenna_port1 == TRUE);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Test Meas cfg cnf processing
    lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

    // For the last iteration, skip connection release so that the MeasObj can be removed explicitly 
    // that would cause LTE_ASSERT(i_ptr->dd_ptr->conn_meas_freq_layer_cnt != 0) to fail
    if (i != 255)
    {
      TF_MSG("Sending Connection released indication");
      lte_rrc_init_default_hdr(&conn_release_indi.msg_hdr, LTE_RRC_CONN_RELEASED_INDI);
      lte_rrc_utf_send_msg((byte*)&conn_release_indi, sizeof(conn_release_indi));
      lte_rrc_utf_wait_for_done();
    }
  }

  // At this point the conn meas layer cnt if uninitialized will be 0
  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to remove list
  meas_cfg->m.measObjectToRemoveListPresent = 1;
  
  meas_cfg->measObjectToRemoveList.elem = 
    (lte_rrc_osys_MeasObjectId*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectId));
  meas_cfg->measObjectToRemoveList.n = 1;

  meas_cfg->measObjectToRemoveList.elem[0] = 1;

  // Validate meas cfg, trying to delete a non-existant meas id should be ignored
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);
  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasConnIntraFreqMeasObjAddInLoopAcrossConnections::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqMeasTestFreqLayerCntAcrossRLF);

void LteRrcMeasConnIntraFreqMeasTestFreqLayerCntAcrossRLF::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqMeasTestFreqLayerCntAcrossRLF::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;

  uint32 buf_len, i;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod) * 6);
  meas_cfg->measObjectToAddModList.n = 6;

  for (i = 0; i < 6; i++)
  {
    meas_cfg->measObjectToAddModList.elem[i].measObjectId = i + 1;
    meas_cfg->measObjectToAddModList.elem[i].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
    meas_cfg->measObjectToAddModList.elem[i].measObject.u.measObjectEUTRA = 
      (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
    meas_cfg->measObjectToAddModList.elem[i].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[i].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[i].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
    meas_cfg->measObjectToAddModList.elem[i].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[i].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
    meas_cfg->measObjectToAddModList.elem[i].measObject.u.measObjectEUTRA->carrierFreq = 100 + i;
    meas_cfg->measObjectToAddModList.elem[i].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
    meas_cfg->measObjectToAddModList.elem[i].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
    meas_cfg->measObjectToAddModList.elem[i].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;
  }

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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x3F);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x3F);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  for (i = 0; i < 6; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.carrier_freq == (100 + i));
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.presence_antenna_port1 == TRUE);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.freq_offset == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.neigh_cells.active_bitmask == 0);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[i].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  }

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Test Meas cfg cnf processing
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // Trigger a meas config with HO that removes all the configured measObjs
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to remove list
  meas_cfg->m.measObjectToRemoveListPresent = 1;
  
  meas_cfg->measObjectToRemoveList.elem = 
    (lte_rrc_osys_MeasObjectId*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectId) * 6);
  meas_cfg->measObjectToRemoveList.n = 6;

  for (i = 0; i < 6; i++)
  {
    meas_cfg->measObjectToRemoveList.elem[i] = i+1;
  }
  

  // Validate meas cfg, trying to delete a non-existant meas obj should be ignored
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    TRUE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
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
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x3F);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Test Meas cfg cnf processing
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // Send a meas_cfg_reqi for reverting to old config
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_REVERT_CONFIG;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req to 
  // revert back to meas config that existed prior to HO
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x3F);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x3F);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Test Meas cfg cnf processing
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Send a meas config to remove a measObj, this is part of the reconfig that comes after reestablishment
  // Trigger a meas config with HO that removes all the configured measObjs
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to remove list
  meas_cfg->m.measObjectToRemoveListPresent = 1;
  
  meas_cfg->measObjectToRemoveList.elem = 
    (lte_rrc_osys_MeasObjectId*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectId));
  meas_cfg->measObjectToRemoveList.n = 1;

  meas_cfg->measObjectToRemoveList.elem[0] = 1;

  // Validate meas cfg, trying to delete a non-existant meas obj should be ignored
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

  // Verify valid and mod bitmasks
  // Just MeasObj 1 should be removed
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x3E);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Test Meas cfg cnf processing
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
}

void LteRrcMeasConnIntraFreqMeasTestFreqLayerCntAcrossRLF::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIntraFreqRxTxTimeDiffReport);

void LteRrcMeasConnIntraFreqRxTxTimeDiffReport::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIntraFreqRxTxTimeDiffReport::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_cphy_conn_meas_report_ind_s cphy_meas_report_ind;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_osys_UL_DCCH_MessageType *ul_dcch_msg_ptr;
  lte_rrc_osys_MeasResults measResults;
  lte_errno_e status;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Set UE to rel9
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel9;

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
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 256;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;

    // Add an item to the report cfg to add/mod list
  meas_cfg->m.reportConfigToAddModListPresent = 1;
  
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
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->
    reportAmount = lte_rrc_osys_ReportConfigEUTRA_reportAmount_r4;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerQuantity 
    = lte_rrc_osys_ReportConfigEUTRA_triggerQuantity_rsrp;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportInterval 
    = lte_rrc_osys_ReportInterval_min1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportQuantity 
    = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_sameAsTriggerQuantity;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->m.
    ue_RxTxTimeDiffPeriodical_r9Present = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.
    reportConfigEUTRA->ue_RxTxTimeDiffPeriodical_r9 = lte_rrc_osys_ReportConfigEUTRA_ue_RxTxTimeDiffPeriodical_r9_setup;

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
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_int == 60000);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].trig_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_qty == LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_SAME);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].max_cells == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].report_amt == 4);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[0].ue_rxtx_time_diff_periodical_enabled == TRUE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&cphy_meas_report_ind.msgr_hdr, LTE_CPHY_MEAS_REPORT_IND);
  cphy_meas_report_ind.meas_id = 1;
  cphy_meas_report_ind.auto_delete_meas_id = FALSE;
  cphy_meas_report_ind.serving_rsrp = 40;
  cphy_meas_report_ind.serving_rsrq = 34;
  cphy_meas_report_ind.neigh_results.size = 0;
  cphy_meas_report_ind.rxtx_timediff_included = TRUE;
  cphy_meas_report_ind.rxtx_result.current_sfn = 500;
  cphy_meas_report_ind.rxtx_result.ue_rxtx_timediff_result = 2000;
  
  lte_rrc_utf_send_msg((byte*)&cphy_meas_report_ind, sizeof(cphy_meas_report_ind));

  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the ul msg reqi containing the meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_MessageType *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.t 
            == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->m.nonCriticalExtensionPresent == 0);

  measResults = ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults;

  // Verify the actual measResults for WCDMA
  TF_ASSERT(measResults.m.measResultNeighCellsPresent == 0);
  TF_ASSERT(measResults.measId == 1);
  TF_ASSERT(measResults.measResultPCell.rsrpResult == 40);
  TF_ASSERT(measResults.measResultPCell.rsrqResult == 34);
  TF_ASSERT(measResults.m.measResultForECID_r9Present == 1);

  TF_ASSERT(lte_rrc_osys_asn1_bitstring_to_int(sizeof(measResults.measResultForECID_r9.
                                                      currentSFN_r9),
                                               &measResults.measResultForECID_r9.
                                                      currentSFN_r9) == 500);
  TF_ASSERT(measResults.measResultForECID_r9.ue_RxTxTimeDiffResult_r9 == 2000);

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();

}

void LteRrcMeasConnIntraFreqRxTxTimeDiffReport::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasDuplicateMeasObjForSameFreqErrorIgnored);

void LteRrcMeasDuplicateMeasObjForSameFreqErrorIgnored::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasDuplicateMeasObjForSameFreqErrorIgnored::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_osys_MeasObjectToAddModList *cur_ptr;
  int i;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod) * 2);
  meas_cfg->measObjectToAddModList.n = 2;

  cur_ptr = &meas_cfg->measObjectToAddModList;

  for (i = 1; i <= 2; i++)
  {
    cur_ptr->elem[i-1].measObjectId = i;
    cur_ptr->elem[i-1].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
    // Configure two MeasObj for the same freq
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->carrierFreq = 0x100;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
    cur_ptr->elem[i-1].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  }

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
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x1);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].
            type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.neigh_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[0].obj_info.eutra.blacklisted_cells.active_bitmask == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

}

void LteRrcMeasDuplicateMeasObjForSameFreqErrorIgnored::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

#endif /* #ifdef 0 */
