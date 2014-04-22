/*!
  @file lte_rrc_meas_test_idle_intra.cpp

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/meas/lte_rrc_meas_test_idle_intra.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/10   svk     Modified the name from UTRA to WCDMA
08/12/09   sk      March 09 spec migration for SIB4,SIB4,SIB5 and conn meas
04/17/09   sk      Updated meas state names. Also added support for scheduled
                   bitmask.
03/31/09   sk      Added support for Generic Error handling as per spec
03/19/09   sk      Added support for rxLevMin and pMax for inter and intra 
                   freq meas
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
#include "lte_rrc_measi.h"
#include "lte_rrc_osys_asn1util.h"
#include <lte_rrc_controller.h>
#include <lte_rrc_cep.h>
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_meas_test_msg.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_meas_test_idle_intra.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */

} lte_rrc_meas_test_idle_intra_s;


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_meas_test_idle_intra.cpp
*/
DECLARE_LTE_RRC_MEAS_TEST_SIB3_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB4_MSG1;

extern "C"
{
  extern lte_rrc_meas_s lte_rrc_meas;
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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqTyp);

void LteRrcMeasReselIntraFreqTyp::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqTyp::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);


  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

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

void LteRrcMeasReselIntraFreqTyp::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselZeroQOffsetCell);

void LteRrcMeasReselZeroQOffsetCell::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselZeroQOffsetCell::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Set qOffset-Cell to 0
  lte_rrc_meas_test_sib4->intraFreqNeighCellList.elem[0].q_OffsetCell = lte_rrc_osys_Q_OffsetRange_dB0;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

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

void LteRrcMeasReselZeroQOffsetCell::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqSIntraNotPresent);

void LteRrcMeasReselIntraFreqSIntraNotPresent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqSIntraNotPresent::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Disable s_intra parameter
  lte_rrc_meas_test_sib3->intraFreqCellReselectionInfo.m.s_IntraSearchPresent = 0;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == LTE_CPHY_MEAS_SRCH_THRESHOLD_INVALID);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

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

void LteRrcMeasReselIntraFreqSIntraNotPresent::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqMeasBandwidthNotPresent);

void LteRrcMeasReselIntraFreqMeasBandwidthNotPresent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqMeasBandwidthNotPresent::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
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

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Disable meas_bandwidth parameter
  lte_rrc_meas_test_sib3->intraFreqCellReselectionInfo.m.allowedMeasBandwidthPresent = 0;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_DL);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);
 
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

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

void LteRrcMeasReselIntraFreqMeasBandwidthNotPresent::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqMeasBandwidth);

void LteRrcMeasReselIntraFreqMeasBandwidth::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqMeasBandwidth::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  lte_rrc_osys_AllowedMeasBandwidth test_values[] = 
  {
    lte_rrc_osys_AllowedMeasBandwidth_mbw6,
    lte_rrc_osys_AllowedMeasBandwidth_mbw15,
    lte_rrc_osys_AllowedMeasBandwidth_mbw25,
    lte_rrc_osys_AllowedMeasBandwidth_mbw50,
    lte_rrc_osys_AllowedMeasBandwidth_mbw75,
    lte_rrc_osys_AllowedMeasBandwidth_mbw100
  };
  int i;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We iterate over meas_bandwidth values, leaving everything else the same
  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    lte_rrc_meas_test_sib3->intraFreqCellReselectionInfo.allowedMeasBandwidth = test_values[i];

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3 and SIB4 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
    sib_updated_indi.cell_id.phy_cell_id = 0x10;
    sib_updated_indi.cell_id.freq = 0x100;

    lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

    lte_rrc_utf_wait_for_done();

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

    // We expect meas sm to have thrown the idle meas cfg req
    lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
    TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

    // Verify cell info
    TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
    TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

    // Verify reselection common info
    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

    // Verify serving freq cfg
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

    // Verify intra freq cfg
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);

    switch (lte_rrc_meas_test_sib3->intraFreqCellReselectionInfo.allowedMeasBandwidth)
    {
      case lte_rrc_osys_AllowedMeasBandwidth_mbw6:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_6);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw15:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw25:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw50:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_50);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw75:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_75);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw100:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_100);
        break;
    }

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

	TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

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
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselIntraFreqMeasBandwidth::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqMeasBandwidthInvalid);

void LteRrcMeasReselIntraFreqMeasBandwidthInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqMeasBandwidthInvalid::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Assign an invalid value to  meas_bandwidth
  lte_rrc_meas_test_sib3->intraFreqCellReselectionInfo.allowedMeasBandwidth = 
    (lte_rrc_osys_AllowedMeasBandwidth) 6;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
  lte_rrc_utf_wait_for_done();
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_DL);

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

void LteRrcMeasReselIntraFreqMeasBandwidthInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqNeighCellListNotPresent);

void LteRrcMeasReselIntraFreqNeighCellListNotPresent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqNeighCellListNotPresent::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Disable neighbour cell list
  lte_rrc_meas_test_sib4->m.intraFreqNeighCellListPresent = 0;
  lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib4->intraFreqNeighCellList.elem);
  lte_rrc_meas_test_sib4->intraFreqNeighCellList.n = 0;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

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

void LteRrcMeasReselIntraFreqNeighCellListNotPresent::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqNeighbourCellPciInvalid);

void LteRrcMeasReselIntraFreqNeighbourCellPciInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqNeighbourCellPciInvalid::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Assign an invalid value to  neighbour cell PCI
  lte_rrc_meas_test_sib4->intraFreqNeighCellList.elem[0].physCellId = 505;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
  lte_rrc_utf_wait_for_done();
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  // The neighbor cell with invalid PCI value should be ignored and the other
  // neighbors should be processed 
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -20);
  
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

void LteRrcMeasReselIntraFreqNeighbourCellPciInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqNeighCellListMin);

void LteRrcMeasReselIntraFreqNeighCellListMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqNeighCellListMin::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Ignore the current neighbour cell list and allocate a cell list of 1
  lte_rrc_meas_test_sib4->intraFreqNeighCellList.n = 1;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
  
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

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

void LteRrcMeasReselIntraFreqNeighCellListMin::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqNeighCellListMax);

void LteRrcMeasReselIntraFreqNeighCellListMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqNeighCellListMax::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  int i;
  lte_rrc_osys_IntraFreqNeighCellList* curr_ptr;

  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Modify the current neighbour cell list
  curr_ptr = &lte_rrc_meas_test_sib4->intraFreqNeighCellList;
  curr_ptr->elem[0].physCellId = 1;
  curr_ptr->elem[0].q_OffsetCell = lte_rrc_osys_Q_OffsetRange_dB_5;

  curr_ptr->elem = 
    (lte_rrc_osys_IntraFreqNeighCellInfo*) 
    lte_rrc_asn1_osys_IE_realloc(curr_ptr->elem, sizeof(lte_rrc_osys_IntraFreqNeighCellInfo) * 16);
  curr_ptr->n = 16;

  for (i = 16; i < 32; i++)
  {
    curr_ptr->elem[i-16].physCellId = i + 1;
    curr_ptr->elem[i-16].q_OffsetCell = lte_rrc_osys_Q_OffsetRange_dB_5;
  }

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 16);
  for (i = 16; i < 32; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[i-16].pci == (i + 1));
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[i-16].q_offset == -5);
  }

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

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

void LteRrcMeasReselIntraFreqNeighCellListMax::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqNeighCellListInvalid);

void LteRrcMeasReselIntraFreqNeighCellListInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqNeighCellListInvalid::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  int i;
  lte_rrc_osys_IntraFreqNeighCellList* curr_ptr;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Modify the current neighbour cell list
  curr_ptr = &lte_rrc_meas_test_sib4->intraFreqNeighCellList;
  curr_ptr->elem[0].physCellId = 1;
  curr_ptr->elem[0].q_OffsetCell = lte_rrc_osys_Q_OffsetRange_dB_5;

  // We are including 18 PCIs because this includes the serving PCI that will be ignored
  curr_ptr->elem = 
        (lte_rrc_osys_IntraFreqNeighCellInfo*) 
        lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_IntraFreqNeighCellInfo) * 18);
  curr_ptr->n = 18;
  
  for (i = 1; i <= 17; i++)
  {
    curr_ptr->elem[i].physCellId = i+1;
    curr_ptr->elem[i].q_OffsetCell = lte_rrc_osys_Q_OffsetRange_dB_5;
  }

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
  lte_rrc_utf_wait_for_done();
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  // Since the number of neighbor cells is more than the maximum allowed value
  // entire neighbor cell list IE should be treated as invalid and hence not present
  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 0);

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

void LteRrcMeasReselIntraFreqNeighCellListInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqServingInNeighCellList);

void LteRrcMeasReselIntraFreqServingInNeighCellList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqServingInNeighCellList::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Include serving cell id in the neighbour cell list
  lte_rrc_meas_test_sib4->intraFreqNeighCellList.elem[0].physCellId = 0x10;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -20);
  
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

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

void LteRrcMeasReselIntraFreqServingInNeighCellList::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqBlackCellListNotPresent);

void LteRrcMeasReselIntraFreqBlackCellListNotPresent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqBlackCellListNotPresent::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Disable black cell list
  lte_rrc_meas_test_sib4->m.intraFreqBlackCellListPresent = 0;
  lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib4->intraFreqBlackCellList.elem);
  lte_rrc_meas_test_sib4->intraFreqBlackCellList.n = 0;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

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

void LteRrcMeasReselIntraFreqBlackCellListNotPresent::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqBlackCellPciInvalid);

void LteRrcMeasReselIntraFreqBlackCellPciInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqBlackCellPciInvalid::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Assign an invalid value to blacklisted cell PCI
  lte_rrc_meas_test_sib4->intraFreqBlackCellList.elem[0].m.rangePresent = 0;
  lte_rrc_meas_test_sib4->intraFreqBlackCellList.elem[0].start= 505;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
  lte_rrc_utf_wait_for_done();
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  // The black listed cell with invalid PCI value should be ignored and the other
  // neighbors should be processed 
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 11);

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

void LteRrcMeasReselIntraFreqBlackCellPciInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqBlackCellListMin);

void LteRrcMeasReselIntraFreqBlackCellListMin::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqBlackCellListMin::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Ignore the current neighbour cell list and allocate a cell list of 1
  lte_rrc_meas_test_sib4->intraFreqBlackCellList.n = 1;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

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

void LteRrcMeasReselIntraFreqBlackCellListMin::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqBlackCellListMax);

void LteRrcMeasReselIntraFreqBlackCellListMax::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqBlackCellListMax::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  int i;
  lte_rrc_osys_IntraFreqBlackCellList* curr_ptr;

  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Ignore the current neighbour cell list and allocate a cell list of 16
  curr_ptr = &lte_rrc_meas_test_sib4->intraFreqBlackCellList;
  curr_ptr->elem[0].m.rangePresent = 0;
  curr_ptr->elem[0].start = 1;
  
  curr_ptr->elem = 
    (lte_rrc_osys_PhysCellIdRange*) 
    lte_rrc_asn1_osys_IE_realloc(curr_ptr->elem, sizeof(lte_rrc_osys_PhysCellIdRange) * 16);
  curr_ptr->n = 16;

  for (i = 1; i < 16; i++)
  {
    curr_ptr->elem[i].m.rangePresent = 0;
    curr_ptr->elem[i].start = i + 1;
  }

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 16);
  for (i = 0; i < 16; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[i].pci_range.end_pci == i + 1);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[i].pci_range.start_pci == i + 1);
  }

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

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

void LteRrcMeasReselIntraFreqBlackCellListMax::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqBlackCellListInvalid);

void LteRrcMeasReselIntraFreqBlackCellListInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqBlackCellListInvalid::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  int i;
  lte_rrc_osys_IntraFreqBlackCellList* curr_ptr;

  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Ignore the current neighbour cell list and allocate a cell list of 17
  curr_ptr = &lte_rrc_meas_test_sib4->intraFreqBlackCellList;
  curr_ptr->elem[0].m.rangePresent = 0;
  curr_ptr->elem[0].start = 1;
  
  curr_ptr->elem = 
    (lte_rrc_osys_PhysCellIdRange*) 
    lte_rrc_asn1_osys_IE_realloc(curr_ptr->elem, sizeof(lte_rrc_osys_PhysCellIdRange) * 17);
  curr_ptr->n = 17;

  for (i = 1; i <= 16; i++)
  {
    curr_ptr->elem[i].m.rangePresent = 0;
    curr_ptr->elem[i].start= i+1;
  }

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
  lte_rrc_utf_wait_for_done();
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

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

void LteRrcMeasReselIntraFreqBlackCellListInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqPMaxNotPresent);

void LteRrcMeasReselIntraFreqPMaxNotPresent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqPMaxNotPresent::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Disable p_Max parameter
  lte_rrc_meas_test_sib3->intraFreqCellReselectionInfo.m.p_MaxPresent = 0;


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);


  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == LTE_DEFAULT_PMAX);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

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

void LteRrcMeasReselIntraFreqPMaxNotPresent::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqQRxLevMinInvalid);

void LteRrcMeasReselIntraFreqQRxLevMinInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqQRxLevMinInvalid::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  int i,j = 0;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));
  j = -71;
  for(i=0; i< 2; i++)
  {
    // We set qRxLevMin to an invalid value
    lte_rrc_meas_test_sib3->intraFreqCellReselectionInfo.q_RxLevMin = j;

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
    // Just indicate that SIB3 and SIB4 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
    sib_updated_indi.cell_id.phy_cell_id = 0x10;
    sib_updated_indi.cell_id.freq = 0x100;
  
    lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
    lte_rrc_utf_wait_for_done();
  
    // We expect meas sm to detect the error and not send any config to ML1
    lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
    TF_ASSERT(int_queue_count == 0);
    TF_ASSERT(ext_queue_count == 0);
  
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
	j = -21;
  }
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselIntraFreqQRxLevMinInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqRangePCIInBlackList);

void LteRrcMeasReselIntraFreqRangePCIInBlackList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselIntraFreqRangePCIInBlackList::Setup() */

void LteRrcMeasReselIntraFreqRangePCIInBlackList::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  int i;
  lte_rrc_osys_IntraFreqBlackCellList* curr_ptr;
  lte_rrc_osys_PhysCellIdRange_range test_values[] = 
  {
    lte_rrc_osys_PhysCellIdRange_range_n4,
    lte_rrc_osys_PhysCellIdRange_range_n8,
    lte_rrc_osys_PhysCellIdRange_range_n12,
    lte_rrc_osys_PhysCellIdRange_range_n16,
    lte_rrc_osys_PhysCellIdRange_range_n24,
    lte_rrc_osys_PhysCellIdRange_range_n32,
    lte_rrc_osys_PhysCellIdRange_range_n48,
    lte_rrc_osys_PhysCellIdRange_range_n64,
    lte_rrc_osys_PhysCellIdRange_range_n84,
    lte_rrc_osys_PhysCellIdRange_range_n96,
    lte_rrc_osys_PhysCellIdRange_range_n128,
    lte_rrc_osys_PhysCellIdRange_range_n168,
    lte_rrc_osys_PhysCellIdRange_range_n252,
    lte_rrc_osys_PhysCellIdRange_range_n504,
  };
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  curr_ptr = &lte_rrc_meas_test_sib4->intraFreqBlackCellList;
  curr_ptr->elem[0].m.rangePresent = 1;
  curr_ptr->elem[0].start = 0;
  curr_ptr->n = 1;

  // We iterate over meas_bandwidth values, leaving everything else the same
  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    curr_ptr->elem[0].range =  test_values[i];

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3 and SIB4 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
    sib_updated_indi.cell_id.phy_cell_id = 0x10;
    sib_updated_indi.cell_id.freq = 0x100;

    lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

    lte_rrc_utf_wait_for_done();

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

    // We expect meas sm to have thrown the idle meas cfg req
    lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
    TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

    // Verify cell info
    TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
    TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

    // Verify reselection common info
    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

    // Verify serving freq cfg
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

    // Verify intra freq cfg
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 1);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 0);
    switch(test_values[i])
    {
      case lte_rrc_osys_PhysCellIdRange_range_n4:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci + 3));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n8:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci + 7));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n12:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci + 11));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n16:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci + 15));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n24:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci + 23));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n32:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci + 31));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n48:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci + 47));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n64:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci + 63));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n84:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci + 83));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n96:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci + 95));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n128:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci + 127));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n168:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci + 167));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n252:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci + 251));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n504:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci + 503));
        break;
      default:
        TF_ASSERT(0);
        break;
    }

	TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

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
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasReselIntraFreqRangePCIInBlackList::Test() */

void LteRrcMeasReselIntraFreqRangePCIInBlackList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselIntraFreqRangePCIInBlackList::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqInvalidPCIInRangeInBlackList);

void LteRrcMeasReselIntraFreqInvalidPCIInRangeInBlackList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselIntraFreqInvalidPCIInRangeInBlackList::Setup() */

void LteRrcMeasReselIntraFreqInvalidPCIInRangeInBlackList::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_rrc_osys_IntraFreqBlackCellList* curr_ptr;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  // Initialize the proto stubs
  lte_rrc_meas_test_init();

  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();

  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3, SIB4 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Modify the current blacklisted cell list
  curr_ptr = &lte_rrc_meas_test_sib4->intraFreqBlackCellList;
  curr_ptr->elem[0].m.rangePresent = 0;
  curr_ptr->elem[0].start = 504;
  curr_ptr->elem[0].range = lte_rrc_osys_PhysCellIdRange_range_n16;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 11);

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

}/* LteRrcMeasReselIntraFreqInvalidPCIInRangeInBlackList::Test() */

void LteRrcMeasReselIntraFreqInvalidPCIInRangeInBlackList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselIntraFreqInvalidPCIInRangeInBlackList::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqInvalidRangeInBlackList);

void LteRrcMeasReselIntraFreqInvalidRangeInBlackList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselIntraFreqInvalidRangeInBlackList::Setup() */

void LteRrcMeasReselIntraFreqInvalidRangeInBlackList::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_rrc_osys_PhysCellIdRange* curr_ptr;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  // Initialize the proto stubs
  lte_rrc_meas_test_init();

  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();

  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3, SIB4 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Modify the current blacklisted cell list
  curr_ptr = &lte_rrc_meas_test_sib4->intraFreqBlackCellList.elem[1];
  curr_ptr->m.rangePresent = 1;
  curr_ptr->start = 1;
  curr_ptr->range = (lte_rrc_osys_PhysCellIdRange_range)16;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);

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

}/* LteRrcMeasReselIntraFreqInvalidRangeInBlackList::Test() */

void LteRrcMeasReselIntraFreqInvalidRangeInBlackList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselIntraFreqInvalidRangeInBlackList::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqSpareRangeInBlackList);

void LteRrcMeasReselIntraFreqSpareRangeInBlackList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselIntraFreqSpareRangeInBlackList::Setup() */

void LteRrcMeasReselIntraFreqSpareRangeInBlackList::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_rrc_osys_IntraFreqBlackCellList* curr_ptr;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  // Initialize the proto stubs
  lte_rrc_meas_test_init();

  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();

  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3, SIB4 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Modify the current blacklisted cell list 
  curr_ptr = &lte_rrc_meas_test_sib4->intraFreqBlackCellList;
  curr_ptr->elem[0].m.rangePresent = 1;
  curr_ptr->elem[0].start= 1;
  curr_ptr->elem[0].range = lte_rrc_osys_PhysCellIdRange_range_spare1;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 1);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);

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

}/* LteRrcMeasReselIntraFreqSpareRangeInBlackList */

void LteRrcMeasReselIntraFreqSpareRangeInBlackList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselIntraFreqSpareRangeInBlackList::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqPresenceAnternnaPort1Present);

void LteRrcMeasReselIntraFreqPresenceAnternnaPort1Present::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqPresenceAnternnaPort1Present::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Set PresenceAntennaPort1 to TRUE
  lte_rrc_meas_test_sib3->intraFreqCellReselectionInfo.presenceAntennaPort1 = 1;


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req != NULL);
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);


  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->neighbourCellConfig == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->antenna_port_1 == TRUE);

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

void LteRrcMeasReselIntraFreqPresenceAnternnaPort1Present::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselIntraFreqTypInRrcConnectingState);

void LteRrcMeasReselIntraFreqTypInRrcConnectingState::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselIntraFreqTypInRrcConnectingState::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;


  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // Change RRC state
  lte_rrc_meas_test_ctrl_state = LTE_RRC_CONTROLLER_SM__CONNECTING;
  lte_rrc_meas_test_cep_state = LTE_RRC_CEP_SM__CONFIGURING_SRB1;

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Send the cnf msg back to meas with E_NOT_ALLOWED
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_NOT_ALLOWED;
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

void LteRrcMeasReselIntraFreqTypInRrcConnectingState::Teardown()
{
  lte_rrc_utf_teardown();
}

