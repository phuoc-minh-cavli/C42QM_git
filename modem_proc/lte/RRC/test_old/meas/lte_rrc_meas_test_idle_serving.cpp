/*!
  @file lte_rrc_meas_test_idle_serving.cpp

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/meas/lte_rrc_meas_test_idle_serving.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/12/09   sk      March 09 spec migration for SIB4,SIB4,SIB5 and conn meas
04/17/09   sk      Updated meas state names. Also added support for scheduled
                   bitmask.
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
//#include "lte_rrc_meas.h"
#include "lte_rrc_measi.h"
#include "lte_rrc_osys_asn1util.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_meas_test_msg.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_meas_test_idle_serving.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */

} lte_rrc_meas_test_idle_serving_s;


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_meas_test_idle_serving.cpp
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


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselServingSNonIntraNotPresent);

void LteRrcMeasReselServingSNonIntraNotPresent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselServingSNonIntraNotPresent::Test()
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

  // Disable s_non_intra parameter
  lte_rrc_meas_test_sib3->cellReselectionServingFreqInfo.m.s_NonIntraSearchPresent = 0;

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

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == LTE_CPHY_MEAS_SRCH_THRESHOLD_INVALID);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
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

void LteRrcMeasReselServingSNonIntraNotPresent::Teardown()
{
  lte_rrc_utf_teardown();
}

