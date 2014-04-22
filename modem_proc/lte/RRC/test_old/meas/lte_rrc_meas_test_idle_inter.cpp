/*!
  @file lte_rrc_meas_test_idle_inter.cpp

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/meas/lte_rrc_meas_test_idle_inter.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/10   svk     Modified the name from UTRA to WCDMA
08/12/09   sk      March 09 spec migration for SIB4,SIB4,SIB5 and conn meas
04/17/09   sk      Updated meas state names. Also added support for scheduled
                   bitmask.
03/31/09   sk      Added support for Generic Error handling as per spec
03/19/09   sk      Added support for rxLevMin and pMax for inter and intra 
                   freq meas
03/13/09   sk      Fixed eutra dl values
02/23/09   sk      Initial Revision
           
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
#include "lte_rrc_csp.h"
#include "lte_rrc_osys_asn1util.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_meas_test_msg.h"
#include <tf_stub.h>

extern "C"
{
  TF_PROTO_STUB(lte_rrc_div_duplex_e, lte_rrc_csp_get_last_camped_mode, void);
}
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_meas_test_idle_intra.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */

} lte_rrc_meas_test_idle_inter_s;


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_meas_test_idle_inter.cpp
*/
DECLARE_LTE_RRC_MEAS_TEST_SIB3_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB4_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB5_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB5_MSG2;
DECLARE_LTE_RRC_MEAS_TEST_SIB5_MSG3;
DECLARE_LTE_RRC_MEAS_TEST_SIB5_FDD_AND_TDD_MSG;
DECLARE_LTE_RRC_MEAS_TEST_SIB5_WITH_DUP_NEIGH_CELLS;
DECLARE_LTE_RRC_MEAS_TEST_SIB5_WITH_DUP_FREQ;

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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqTyp);

void LteRrcMeasReselInterFreqTyp::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqTyp::Setup() */

void LteRrcMeasReselInterFreqTyp::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas SM should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // Wait for Meas SM to process the camped indi
  lte_rrc_utf_wait_for_done();

  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
	                                      LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max== -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
			LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);
  
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqTyp::Test() */

void LteRrcMeasReselInterFreqTyp::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqTyp::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqZeroQOffset);

void LteRrcMeasReselInterFreqZeroQOffset::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqZeroQOffset::Setup() */

void LteRrcMeasReselInterFreqZeroQOffset::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; 
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas SM should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // Wait for Meas SM to process the camped indi
  lte_rrc_utf_wait_for_done();

  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg3,
                                sizeof(lte_rrc_meas_test_sib5_msg3));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
	                                      LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max== -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
			LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 0);
  
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqZeroQOffset::Test() */

void LteRrcMeasReselInterFreqZeroQOffset::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqZeroQOffset::Teardown() */


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqTDDPruningOnFDD);

void LteRrcMeasReselInterFreqTDDPruningOnFDD::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqTDDPruningOnFDD::Setup() */

void LteRrcMeasReselInterFreqTDDPruningOnFDD::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  
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

  // Point the SIBs to be returned to the typical SIB3 , SIB4 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_fdd_and_tdd_msg,
                                sizeof(lte_rrc_meas_test_sib5_fdd_and_tdd_msg));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4, SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                      LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  // The neighbor cell with invalid q_rxlevmin value should be ignored and the other
  // neighbors should be processed 
  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].dl_carrier_freq == 36000);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].q_rxlevmin == -50);

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

}/* LteRrcMeasReselInterFreqTDDPruningOnFDD::Test() */

void LteRrcMeasReselInterFreqTDDPruningOnFDD::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqTDDPruningOnFDD::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqFDDPruningOnTDD);

void LteRrcMeasReselInterFreqFDDPruningOnTDD::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqTDDPruningOnFDD::Setup() */

void LteRrcMeasReselInterFreqFDDPruningOnTDD::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  // Set TDD for camped cell
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_meas_test_stub__get_mode_tdd_for_last_camped_cell;
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  lte_rrc_meas_test_cell_id.freq = 36100;
  lte_rrc_meas_test_cell_id.phy_cell_id = 2;

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 , SIB4 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_fdd_and_tdd_msg,
                                sizeof(lte_rrc_meas_test_sib5_fdd_and_tdd_msg));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4, SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                      LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 2;
  sib_updated_indi.cell_id.freq = 36100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  // The neighbor cell with invalid q_rxlevmin value should be ignored and the other
  // neighbors should be processed 
  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].dl_carrier_freq == 36000);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].q_rxlevmin == -50);

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

}/* LteRrcMeasReselInterFreqFDDPruningOnTDD::Test() */

void LteRrcMeasReselInterFreqFDDPruningOnTDD::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqFDDPruningOnTDD::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreq_p_maxNotPresent);

void LteRrcMeasReselInterFreq_p_maxNotPresent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreq_p_maxNotPresent::Setup() */

void LteRrcMeasReselInterFreq_p_maxNotPresent::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ; 
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
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

  // Point the SIBs to be returned to the typical SIB3 , SIB4 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Disable p_max
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].m.p_MaxPresent = 0;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Indicate that SIB3, SIB4, SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);
    

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == LTE_DEFAULT_PMAX);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
            LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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
  
}/* LteRrcMeasReselInterFreq_p_maxNotPresent::Test() */

void LteRrcMeasReselInterFreq_p_maxNotPresent::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreq_p_maxNotPresent::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqTReselEutran);

void LteRrcMeasReselInterFreqTReselEutran::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqTReselEutran::Setup() */

void LteRrcMeasReselInterFreqTReselEutran::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ; 
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  int                           i                  = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
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

  // Point the SIBs to be returned to the typical SIB3, SIB4and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Check for all the valid values of t_reselection_EUTRAN only
  for (i = 0; i <= 7; i++)
  {
    lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].t_ReselectionEUTRA = i;

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3, SIB4 and SIB5 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
		                                    LTE_RRC_SIB4_BITMASK |
		                                    LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                              LTE_RRC_SIB4_BITMASK |
	                                      LTE_RRC_SIB5_BITMASK;
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
    lte_rrc_verify_serving_cell_info(cfg_req);


    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

    // Verify reselection common info
    lte_rrc_verify_common_resel_info(cfg_req);

    // Verify serving freq cfg
    lte_rrc_verify_serving_freq_info(cfg_req);

    // Verify intra frequency info
    lte_rrc_verify_intra_freq_info(cfg_req);
    

	//Verify inter-freq info
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == i);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
			  LTE_CPHY_MEAS_BANDWIDTH_15);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);

	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqTReselEutran::Test() */

void LteRrcMeasReselInterFreqTReselEutran::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqTReselEutran::Teardown() */


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqTReselEutranSFMed);

void LteRrcMeasReselInterFreqTReselEutranSFMed::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqTReselEutranSFMed::Setup() */

void LteRrcMeasReselInterFreqTReselEutranSFMed::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ; 
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  int                           i                  = 0;
  lte_rrc_osys_SpeedStateScaleFactors_sf_Medium test_values[] = 
  {
    lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot25,
    lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5,
    lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot75,
    lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_lDot0
  };

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
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

 lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Check for all the valid values of t_reselection_EUTRANSFMed only
 for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].t_ReselectionEUTRA_SF.sf_Medium =
		test_values[i];

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3, SIB4 and SIB5 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
		                                    LTE_RRC_SIB4_BITMASK |
		                                    LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                              LTE_RRC_SIB4_BITMASK |
	                                      LTE_RRC_SIB5_BITMASK;
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
    lte_rrc_verify_serving_cell_info(cfg_req);


    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

    // Verify reselection common info
    lte_rrc_verify_common_resel_info(cfg_req);

    // Verify serving freq cfg
    lte_rrc_verify_serving_freq_info(cfg_req);

    // Verify intra frequency info
    lte_rrc_verify_intra_freq_info(cfg_req);
    

	//Verify inter-freq info
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
    switch (lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].t_ReselectionEUTRA_SF.sf_Medium)
    {
      case lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot25:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot75:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT75);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_lDot0:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_1DOT0);
        break;
    }
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
			  LTE_CPHY_MEAS_BANDWIDTH_15);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);

	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqTReselEutranSFMed::Test() */

void LteRrcMeasReselInterFreqTReselEutranSFMed::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqTReselEutranSFMed::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqTReselEutranSFHigh);

void LteRrcMeasReselInterFreqTReselEutranSFHigh::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqTReselEutranSFHigh::Setup() */

void LteRrcMeasReselInterFreqTReselEutranSFHigh::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ; 
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  int                           i                  = 0;
  lte_rrc_osys_SpeedStateScaleFactors_sf_High test_values[] = 
  {
    lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot25,
    lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5,
    lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot75,
    lte_rrc_osys_SpeedStateScaleFactors_sf_High_lDot0
  };
  
  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
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

 lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Check for all the valid values of t_reselection_EUTRANSFMed only
 for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].t_ReselectionEUTRA_SF.sf_High =
		test_values[i];

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3, SIB4 and SIB5 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
		                                    LTE_RRC_SIB4_BITMASK |
		                                    LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                              LTE_RRC_SIB4_BITMASK |
	                                      LTE_RRC_SIB5_BITMASK;
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
    lte_rrc_verify_serving_cell_info(cfg_req);


    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

    // Verify reselection common info
    lte_rrc_verify_common_resel_info(cfg_req);

    // Verify serving freq cfg
    lte_rrc_verify_serving_freq_info(cfg_req);

    // Verify intra frequency info
    lte_rrc_verify_intra_freq_info(cfg_req);
    

	//Verify inter-freq info
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
    switch (lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].t_ReselectionEUTRA_SF.sf_High)
    {
      case lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot25:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot75:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT75);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_High_lDot0:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_1DOT0);
        break;
    }
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
			  LTE_CPHY_MEAS_BANDWIDTH_15);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);

	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqTReselEutranSFHigh::Test() */

void LteRrcMeasReselInterFreqTReselEutranSFHigh::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqTReselEutranSFHigh::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqSpeedScalingDisabled);

void LteRrcMeasReselInterFreqSpeedScalingDisabled::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqSpeedScalingDisabled::Setup() */

void LteRrcMeasReselInterFreqSpeedScalingDisabled::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ; 
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Disable speed dependant scaling params
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].m.t_ReselectionEUTRA_SFPresent = 0;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
		                                  LTE_RRC_SIB4_BITMASK |
		                                  LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA  |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_1DOT0);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
            LTE_CPHY_MEAS_TIME_SF_1DOT0);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
            LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqSpeedScalingDisabled::Test() */

void LteRrcMeasReselInterFreqSpeedScalingDisabled::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqSpeedScalingDisabled::Teardown() */


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqThreshX);

void LteRrcMeasReselInterFreqThreshX::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqThreshX::Setup() */

void LteRrcMeasReselInterFreqThreshX::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  int                           i                  = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                 sizeof(lte_rrc_meas_test_sib5_msg1));

  // Check for all the valid values of threshX_low and threshX_high only
  for (i = 0; i < 31; i++)
  {
    lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].threshX_Low = i;
    lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].threshX_High = i;

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3, SIB4 and SIB5 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
		                                    LTE_RRC_SIB4_BITMASK |
		                                    LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                              LTE_RRC_SIB4_BITMASK |
	                                      LTE_RRC_SIB5_BITMASK;
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
    lte_rrc_verify_serving_cell_info(cfg_req);


    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

    // Verify reselection common info
    lte_rrc_verify_common_resel_info(cfg_req);

    // Verify serving freq cfg
    lte_rrc_verify_serving_freq_info(cfg_req);

    // Verify intra frequency info
    lte_rrc_verify_intra_freq_info(cfg_req);
    

	//Verify inter-freq info
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == (i * 2));
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == (i * 2));
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
			  LTE_CPHY_MEAS_BANDWIDTH_15);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);

	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqThreshX::Test() */

void LteRrcMeasReselInterFreqThreshX::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqThreshX::Teardown() */


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqMeasBandwidth);

void LteRrcMeasReselInterFreqMeasBandwidth::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqMeasBandwidth::Setup() */

void LteRrcMeasReselInterFreqMeasBandwidth::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  int                           i                  = 0;
  lte_rrc_osys_AllowedMeasBandwidth  test_values[] = 
  {
    lte_rrc_osys_AllowedMeasBandwidth_mbw6,
    lte_rrc_osys_AllowedMeasBandwidth_mbw15,
    lte_rrc_osys_AllowedMeasBandwidth_mbw25,
    lte_rrc_osys_AllowedMeasBandwidth_mbw50,
    lte_rrc_osys_AllowedMeasBandwidth_mbw75,
    lte_rrc_osys_AllowedMeasBandwidth_mbw100,
  };

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                 sizeof(lte_rrc_meas_test_sib5_msg1));

  // Check for all the valid values of t_reselection_EUTRAN only
  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].allowedMeasBandwidth = 
		test_values[i];

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3, SIB4 and SIB5 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
		                                    LTE_RRC_SIB4_BITMASK |
		                                    LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                              LTE_RRC_SIB4_BITMASK |
	                                      LTE_RRC_SIB5_BITMASK;
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
    lte_rrc_verify_serving_cell_info(cfg_req);


    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS)); 
    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

    // Verify reselection common info
    lte_rrc_verify_common_resel_info(cfg_req);

    // Verify serving freq cfg
    lte_rrc_verify_serving_freq_info(cfg_req);

    // Verify intra frequency info
    lte_rrc_verify_intra_freq_info(cfg_req);
    

	//Verify inter-freq info
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
    switch (lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].allowedMeasBandwidth)
    {
      case lte_rrc_osys_AllowedMeasBandwidth_mbw6:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_6);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw15:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw25:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth== LTE_CPHY_MEAS_BANDWIDTH_25);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw50:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_50);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw75:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_75);
        break;

      case lte_rrc_osys_AllowedMeasBandwidth_mbw100:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_100);
        break;
    }
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);

	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqMeasBandwidth::Test() */

void LteRrcMeasReselInterFreqMeasBandwidth::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqMeasBandwidth::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqMeasBandwidthInvalid);

void LteRrcMeasReselInterFreqMeasBandwidthInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqMeasBandwidthInvalid::Setup() */

void LteRrcMeasReselInterFreqMeasBandwidthInvalid::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg2,
                                sizeof(lte_rrc_meas_test_sib5_msg2));

  // set the bandwidth to an invalid value
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].allowedMeasBandwidth = 
		(lte_rrc_osys_AllowedMeasBandwidth) 6;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
		                                  LTE_RRC_SIB4_BITMASK |
		                                  LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  // The neighbor cell with invalid MeasBandwidth value should be ignored and the other
  // neighbors should be processed 
  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 1000);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -100);

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

}/* LteRrcMeasReselInterFreqMeasBandwidthInvalid::Test() */

void LteRrcMeasReselInterFreqMeasBandwidthInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqMeasBandwidthInvalid::Teardown() */


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqMeasBandInvalid);

void LteRrcMeasReselInterFreqMeasBandInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqMeasBandInvalid::Setup() */

void LteRrcMeasReselInterFreqMeasBandInvalid::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg2,
                                sizeof(lte_rrc_meas_test_sib5_msg2));

  // set the freq to a value that doesn't belong to any band
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].dl_CarrierFreq = 64555; 

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
		                                  LTE_RRC_SIB4_BITMASK |
		                                  LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  // The neighbor cell with invalid band should be ignored and the other
  // neighbors should be processed 
  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 1000);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -100);

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

}/* LteRrcMeasReselInterFreqMeasBandInvalid::Test() */

void LteRrcMeasReselInterFreqMeasBandInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqMeasBandInvalid::Teardown() */


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqMeasBandUnsupported);

void LteRrcMeasReselInterFreqMeasBandUnsupported::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqMeasBandUnsupported::Setup() */

void LteRrcMeasReselInterFreqMeasBandUnsupported::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg2,
                                sizeof(lte_rrc_meas_test_sib5_msg2));

  // set band supported flag to false
  lte_rrc_meas_test_lte_band_is_supported = FALSE; 

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
		                                  LTE_RRC_SIB4_BITMASK |
		                                  LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  // The neighbor cell with unsupported band value should be ignored and the other
  // neighbors should be processed 
  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |          
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ|
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));

  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 0);

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

}/* LteRrcMeasReselInterFreqMeasBandUnsupported::Test() */

void LteRrcMeasReselInterFreqMeasBandUnsupported::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqMeasBandUnsupported::Teardown() */


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqCellReselPriority);

void LteRrcMeasReselInterFreqCellReselPriority::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqCellReselPriority::Setup() */

void LteRrcMeasReselInterFreqCellReselPriority::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  int                           i                  = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                 sizeof(lte_rrc_meas_test_sib5_msg1));

  // Check for all the valid values of cell priority only
  for (i = 0; i <= 7; i++)
  {
    lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].cellReselectionPriority = i;

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3, SIB4 and SIB5 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
		                                    LTE_RRC_SIB4_BITMASK |
		                                    LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                              LTE_RRC_SIB4_BITMASK |
	                                      LTE_RRC_SIB5_BITMASK;
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
    lte_rrc_verify_serving_cell_info(cfg_req);


    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
    // Verify reselection common info
    lte_rrc_verify_common_resel_info(cfg_req);

    // Verify serving freq cfg
    lte_rrc_verify_serving_freq_info(cfg_req);

    // Verify intra frequency info
    lte_rrc_verify_intra_freq_info(cfg_req);
    

	//Verify inter-freq info
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
			  LTE_CPHY_MEAS_BANDWIDTH_15);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == i);
	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);

	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqCellReselPriority::Test() */

void LteRrcMeasReselInterFreqCellReselPriority::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqCellReselPriority::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqCellReselPriorityNotPresent);

void LteRrcMeasReselInterFreqCellReselPriorityNotPresent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqCellReselPriorityNotPresent::Setup() */

void LteRrcMeasReselInterFreqCellReselPriorityNotPresent::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Disable cell priority
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].m.cellReselectionPriorityPresent = 0;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
		                                  LTE_RRC_SIB4_BITMASK |
		                                  LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ  |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);
    
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

}/* LteRrcMeasReselInterFreqCellReselPriorityNotPresent::Test() */

void LteRrcMeasReselInterFreqCellReselPriorityNotPresent::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqCellReselPriorityNotPresent::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqOffsetFreq);

void LteRrcMeasReselInterFreqOffsetFreq::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqOffsetFreq::Setup() */

void LteRrcMeasReselInterFreqOffsetFreq::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  int                           i                  = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
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
    lte_rrc_osys_Q_OffsetRange_dB24
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                 sizeof(lte_rrc_meas_test_sib5_msg1));

  // Check for all the valid values of t_reselection_EUTRAN only
  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0]) - 1); i++)
  {
    lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].q_OffsetFreq = 
      test_values[i];

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3, SIB4 and SIB5 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
		                                    LTE_RRC_SIB4_BITMASK |
		                                    LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                              LTE_RRC_SIB4_BITMASK |
	                                      LTE_RRC_SIB5_BITMASK;
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
    lte_rrc_verify_serving_cell_info(cfg_req);


    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

    // Verify reselection common info
    lte_rrc_verify_common_resel_info(cfg_req);

    // Verify serving freq cfg
    lte_rrc_verify_serving_freq_info(cfg_req);

    // Verify intra frequency info
    lte_rrc_verify_intra_freq_info(cfg_req);
    

    //Verify inter-freq info
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
              LTE_CPHY_MEAS_TIME_SF_0DOT75);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
              LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
    switch (lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].q_OffsetFreq)
    {
      case lte_rrc_osys_Q_OffsetRange_dB_24:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -24);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB_22:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -22);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB_20:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -20);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB_18:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -18);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB_16:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -16);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB_14:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -14);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB_12:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -12);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB_10:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -10);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB_8:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -8);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB_6:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB_5:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -5);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB_4:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -4);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB_3:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -3);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB_2:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -2);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB_1:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -1);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB0:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 0);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB1:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 1);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB2:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 2);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB3:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 3);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB4:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 4);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB5:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 5);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB6:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 6);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB8:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 8);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB10:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 10);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB12:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 12);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB14:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 14);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB16:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 16);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB18:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 18);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB20:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 20);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB22:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 22);
		break;
      case lte_rrc_osys_Q_OffsetRange_dB24:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 24);
        break;
      default:
        TF_ASSERT(0);
        break;
    }
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqOffsetFreq::Test() */

void LteRrcMeasReselInterFreqOffsetFreq::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqOffsetFreq::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqOffsetFreqInvalid);

void LteRrcMeasReselInterFreqOffsetFreqInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqOffsetFreqInvalid::Setup() */

void LteRrcMeasReselInterFreqOffsetFreqInvalid::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Assign invalid value to q_offset_freq
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].q_OffsetFreq = (lte_rrc_osys_Q_OffsetRange)32;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
		                                    LTE_RRC_SIB4_BITMASK |
		                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  // The invalid q_offset should be replaced by the default offset of 0
  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == 0);

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

}/* LteRrcMeasReselInterFreqOffsetFreqInvalid::Test() */

void LteRrcMeasReselInterFreqOffsetFreqInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqOffsetFreqInvalid::Teardown() */


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqNeighCellListNotPresent);

void LteRrcMeasReselInterFreqNeighCellListNotPresent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqNeighCellListNotPresent::Setup() */

void LteRrcMeasReselInterFreqNeighCellListNotPresent::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; 
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));

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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Disable neighbour cell list
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].m.interFreqNeighCellListPresent = 0;
  lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqNeighCellList.elem);
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqNeighCellList.n = 0;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);

  // Verify inter-freq info
  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
            LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqNeighCellListNotPresent::Test() */

void LteRrcMeasReselInterFreqNeighCellListNotPresent::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqNeighCellListNotPresent::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqBlackCellListNotPresent);

void LteRrcMeasReselInterFreqBlackCellListNotPresent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqBlackCellListNotPresent::Setup() */

void LteRrcMeasReselInterFreqBlackCellListNotPresent::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));

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

  // Point the SIBs to be returned to the typical SIB3, SIB4 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Disable neighbour cell list
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].m.interFreqBlackCellListPresent = 0;
  lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqBlackCellList.elem);
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqBlackCellList.n = 0;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);

  // Verify inter-freq info
  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
            LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqBlackCellListNotPresent::Test() */

void LteRrcMeasReselInterFreqBlackCellListNotPresent::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqBlackCellListNotPresent::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqMinNeighborCellList);

void LteRrcMeasReselInterFreqMinNeighborCellList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqMinNeighborCellList::Setup() */

void LteRrcMeasReselInterFreqMinNeighborCellList::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));

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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqNeighCellList.n = 1;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));  
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 
 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);

  // Verify inter-freq info
  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
            LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqMinNeighborCellList::Test() */

void LteRrcMeasReselInterFreqMinNeighborCellList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqMinNeighborCellList::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqMinBlackCellList);

void LteRrcMeasReselInterFreqMinBlackCellList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqMinBlackCellList::Setup() */

void LteRrcMeasReselInterFreqMinBlackCellList::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));

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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqBlackCellList.n = 1;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);

  // Verify inter-freq info
  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
            LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqMinBlackCellList::Test() */

void LteRrcMeasReselInterFreqMinBlackCellList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqMinBlackCellList::Teardown()*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqMaxNeighborCellList);

void LteRrcMeasReselInterFreqMaxNeighborCellList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqMaxNeighborCellList::Setup() */

void LteRrcMeasReselInterFreqMaxNeighborCellList::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  int                           i                  = 0;
  lte_rrc_osys_InterFreqNeighCellList *neighbor_ptr = NULL;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));

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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Modify the current neighbour cell list and allocate a cell list of 16
  neighbor_ptr = 
	  &lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqNeighCellList;
  neighbor_ptr->elem[0].physCellId= 1;
  neighbor_ptr->elem[0].q_OffsetCell         = lte_rrc_osys_Q_OffsetRange_dB_5;

  neighbor_ptr->elem = (lte_rrc_osys_InterFreqNeighCellInfo*) 
    lte_rrc_asn1_osys_IE_realloc(neighbor_ptr->elem, sizeof(lte_rrc_osys_InterFreqNeighCellInfo) * 16);
  neighbor_ptr->n = 16;

  
  for (i = 1; i < 16; i++)
  {
    neighbor_ptr->elem[i].physCellId= i + 1;
    neighbor_ptr->elem[i].q_OffsetCell       = lte_rrc_osys_Q_OffsetRange_dB_5;
  }

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS)); 
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 
  
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);

  // Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
            LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == LTE_CPHY_MEAS_MAX_CELL_INTER );
  for (i = 0; i < 16; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[i].pci== i + 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[i].q_offset == -5);
  }

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqMaxNeighborCellList::Test() */

void LteRrcMeasReselInterFreqMaxNeighborCellList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqMaxNeighborCellList::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqRangePCIInBlackList);

void LteRrcMeasReselInterFreqRangePCIInBlackList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqRangePCIInBlackList::Setup() */

void LteRrcMeasReselInterFreqRangePCIInBlackList::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  int                           i                  = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  lte_rrc_osys_InterFreqBlackCellList *blacklist_ptr = NULL;
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
    lte_rrc_osys_PhysCellIdRange_range_n504
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                 sizeof(lte_rrc_meas_test_sib5_msg1));
  // Modify the current neighbour cell list
  blacklist_ptr = 
	  &lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqBlackCellList;
  blacklist_ptr->elem[0].m.rangePresent = 1;
  blacklist_ptr->elem[0].start       = 0;
  blacklist_ptr->n = 1;

  // Check for all the valid values of rangePCI only
  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    blacklist_ptr->elem[0].range = test_values[i];
    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3, SIB4 and SIB5 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
		                            LTE_RRC_SIB4_BITMASK |
		                            LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                              LTE_RRC_SIB4_BITMASK |
	                                      LTE_RRC_SIB5_BITMASK;
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
    lte_rrc_verify_serving_cell_info(cfg_req);


    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                          LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));  
    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 
 
    // Verify reselection common info
    lte_rrc_verify_common_resel_info(cfg_req);

    // Verify serving freq cfg
    lte_rrc_verify_serving_freq_info(cfg_req);

    // Verify intra frequency info
    lte_rrc_verify_intra_freq_info(cfg_req);
    
    //Verify inter-freq info
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
      LTE_CPHY_MEAS_TIME_SF_0DOT75);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
      LTE_CPHY_MEAS_TIME_SF_0DOT75);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
      LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 0);
    switch(test_values[i])
    {
      case lte_rrc_osys_PhysCellIdRange_range_n4:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci + 3));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n8:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci + 7));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n12:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci + 11));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n16:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci + 15));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n24:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci + 23));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n32:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci + 31));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n48:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci + 47));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n64:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci + 63));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n84:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci + 83));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n96:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci + 95));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n128:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci + 127));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n168:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci + 167));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n252:
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci + 251));
        break;
      case lte_rrc_osys_PhysCellIdRange_range_n504 :
        TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 
          (cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci + 503));
        break;
      default:
        TF_ASSERT(0);
        break;
    }

	TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqRangePCIInBlackList::Test() */

void LteRrcMeasReselInterFreqRangePCIInBlackList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqRangePCIInBlackList::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqMaxBlackCellList);

void LteRrcMeasReselInterFreqMaxBlackCellList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/*LteRrcMeasReselInterFreqMaxBlackCellList::Setup() */

void LteRrcMeasReselInterFreqMaxBlackCellList::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  int                           i                  = 0;
  lte_rrc_osys_InterFreqBlackCellList *blacklist_ptr = NULL;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));

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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Modify the current neighbour cell list and allocate a cell list of 16
  blacklist_ptr = 
	  &lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqBlackCellList;
  blacklist_ptr->elem[0].m.rangePresent = 0;
  blacklist_ptr->elem[0].start     = 1;

  blacklist_ptr->elem = (lte_rrc_osys_PhysCellIdRange*) 
      lte_rrc_asn1_osys_IE_realloc(blacklist_ptr->elem, sizeof(lte_rrc_osys_PhysCellIdRange) * 16);
  blacklist_ptr->n = 16;

  for (i = 1; i < 16; i++)
  {
  	blacklist_ptr->elem[i].m.rangePresent = 0;
      blacklist_ptr->elem[i].start = i + 1;
  }

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);

  // Verify inter-freq info
  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
            LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == LTE_CPHY_MEAS_MAX_CELL_BLACK );
  for (i = 0; i < 16; i++)
  {
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[i].pci_range.start_pci == i+1);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[i].pci_range.end_pci == i+1);
  }

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqMaxBlackCellList::Test() */

void LteRrcMeasReselInterFreqMaxBlackCellList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqMaxBlackCellList::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqInvalidPCIInWhiteList);

void LteRrcMeasReselInterFreqInvalidPCIInWhiteList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqInvalidPCIInWhiteList::Setup() */

void LteRrcMeasReselInterFreqInvalidPCIInWhiteList::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  lte_rrc_osys_InterFreqNeighCellList *neighbor_ptr = NULL;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Modify the current neighbour cell list
  neighbor_ptr = 
	  &lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqNeighCellList;
  neighbor_ptr->elem[0].physCellId = 505;
  neighbor_ptr->elem[0].q_OffsetCell        = lte_rrc_osys_Q_OffsetRange_dB22;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 0);

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

}/* LteRrcMeasReselInterFreqInvalidPCIInWhiteList::Test() */

void LteRrcMeasReselInterFreqInvalidPCIInWhiteList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqInvalidPCIInWhiteList::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqInvalidPCIInBlackList);

void LteRrcMeasReselInterFreqInvalidPCIInBlackList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqInvalidPCIInBlackList::Setup() */

void LteRrcMeasReselInterFreqInvalidPCIInBlackList::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  lte_rrc_osys_InterFreqBlackCellList *blacklist_ptr = NULL;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));

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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Modify the current black cell list
  blacklist_ptr = 
	  &lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqBlackCellList;
  blacklist_ptr->elem[0].m.rangePresent = 0;
  blacklist_ptr->elem[0].start        = 505;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 0);

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

}/* LteRrcMeasReselInterFreqInvalidPCIInBlackList::Test() */

void LteRrcMeasReselInterFreqInvalidPCIInBlackList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqInvalidPCIInBlackList::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqInvalidPCIInRangeInBlackList);

void LteRrcMeasReselInterFreqInvalidPCIInRangeInBlackList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqInvalidPCIInRangeInBlackList::Setup() */

void LteRrcMeasReselInterFreqInvalidPCIInRangeInBlackList::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  lte_rrc_osys_InterFreqBlackCellList *blacklist_ptr = NULL;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Modify the current blacklisted cell list
  blacklist_ptr = 
	  &lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqBlackCellList;
  blacklist_ptr->elem[0].m.rangePresent = 1;
  blacklist_ptr->elem[0].start      = 504;
  blacklist_ptr->elem[0].range      = lte_rrc_osys_PhysCellIdRange_range_n16;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 0);

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

}/* LteRrcMeasReselInterFreqInvalidPCIInRangeInBlackList::Test() */

void LteRrcMeasReselInterFreqInvalidPCIInRangeInBlackList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqInvalidPCIInRangeInBlackList::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqInvalidRangeInBlackList);

void LteRrcMeasReselInterFreqInvalidRangeInBlackList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqInvalidRangeInBlackList::Setup() */

void LteRrcMeasReselInterFreqInvalidRangeInBlackList::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ; 
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  lte_rrc_osys_InterFreqBlackCellList *blacklist_ptr = NULL;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Modify the current black cell list
  blacklist_ptr = 
	  &lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqBlackCellList;
  blacklist_ptr->elem[0].m.rangePresent = 1;
  blacklist_ptr->elem[0].start      = 503;
  blacklist_ptr->elem[0].range      = (lte_rrc_osys_PhysCellIdRange_range)16;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 0);

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

}/* LteRrcMeasReselInterFreqInvalidRangeInBlackList::Test() */

void LteRrcMeasReselInterFreqInvalidRangeInBlackList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqInvalidRangeInBlackList::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqSpareRangeInBlackList);

void LteRrcMeasReselInterFreqSpareRangeInBlackList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqSpareRangeInBlackList::Setup() */

void LteRrcMeasReselInterFreqSpareRangeInBlackList::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; 
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  lte_rrc_osys_InterFreqBlackCellList *blacklist_ptr = NULL;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));

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

  // Point the SIBs to be returned to the typical SIB3, SIB4 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Modify the current neighbour cell list
  blacklist_ptr = 
	  &lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqBlackCellList;
  blacklist_ptr->elem[0].m.rangePresent = 1;
  blacklist_ptr->elem[0].start     = 503;
  blacklist_ptr->elem[0].range     = lte_rrc_osys_PhysCellIdRange_range_spare1;
  blacklist_ptr->n = 1;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));  
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 
 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);

  // Verify inter-freq info
  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
            LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqSpareRangeInBlackList::Test() */

void LteRrcMeasReselInterFreqSpareRangeInBlackList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqSpareRangeInBlackList::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqNeighborCellListInvalid);

void LteRrcMeasReselInterFreqNeighborCellListInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqNeighborCellListInvalid::Setup() */

void LteRrcMeasReselInterFreqNeighborCellListInvalid::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  int                           i                  = 0;
  lte_rrc_osys_InterFreqNeighCellList *neighbor_ptr = NULL;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  
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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Modify the current neighbour cell list and allocate a cell list of 16
  neighbor_ptr = 
	  &lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqNeighCellList;
  neighbor_ptr->elem[0].physCellId = 1;
  neighbor_ptr->elem[0].q_OffsetCell       = lte_rrc_osys_Q_OffsetRange_dB_5;
  
  neighbor_ptr->elem = (lte_rrc_osys_InterFreqNeighCellInfo*) 
    lte_rrc_asn1_osys_IE_realloc(neighbor_ptr->elem, sizeof(lte_rrc_osys_InterFreqNeighCellInfo) * 17);
  neighbor_ptr->n = 17;

  for (i = 1; i <= 16; i++)
  {
    neighbor_ptr->elem[i].physCellId= i + 1;
    neighbor_ptr->elem[i].q_OffsetCell= lte_rrc_osys_Q_OffsetRange_dB_5;
  }

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 0);

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
  
}/* LteRrcMeasReselInterFreqNeighborCellListInvalid::Test() */

void LteRrcMeasReselInterFreqNeighborCellListInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqNeighborCellListInvalid::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqBlackCellListInvalid);

void LteRrcMeasReselInterFreqBlackCellListInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqBlackCellListInvalid::Setup() */

void LteRrcMeasReselInterFreqBlackCellListInvalid::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  int                           i                  = 0;
  lte_rrc_osys_InterFreqBlackCellList *blacklist_ptr = NULL;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));

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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Modify the current black cell list and allocate a cell list of 16
  blacklist_ptr = 
	  &lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].interFreqBlackCellList;
  blacklist_ptr->elem[0].m.rangePresent = 0;
  blacklist_ptr->elem[0].start       = 1;

  blacklist_ptr->elem = (lte_rrc_osys_PhysCellIdRange*) 
      lte_rrc_asn1_osys_IE_realloc(blacklist_ptr->elem, sizeof(lte_rrc_osys_PhysCellIdRange) * 17);
  blacklist_ptr->n = 17;

  for (i = 1; i <= 16; i++)
  {
  	blacklist_ptr->elem[i].m.rangePresent = 0;
      blacklist_ptr->elem[i].start = i + 1;
  }

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  // Since the number of blacklist cells is more than the allowed value, the entire
  // blacklist IE should be treated as not present
  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 0);

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

}/* LteRrcMeasReselInterFreqBlackCellListInvalid::Test() */

void LteRrcMeasReselInterFreqBlackCellListInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqBlackCellListInvalid::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqMaxList);

void LteRrcMeasReselInterFreqMaxList::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqMaxList::Setup() */

void LteRrcMeasReselInterFreqMaxList::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ; 
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;
  int                           i                  = 0;
  lte_rrc_osys_InterFreqCarrierFreqList *curr_ptr = NULL;
  uint64 nCellConfig_integer_val;
  lte_errno_e status = E_SUCCESS;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));

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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

   // Modify the current inter-freq list.
  curr_ptr = &lte_rrc_meas_test_sib5->interFreqCarrierFreqList;
  curr_ptr->elem[0].m.cellReselectionPriorityPresent = 0;
  curr_ptr->elem[0].m.interFreqBlackCellListPresent = 0;
  curr_ptr->elem[0].m.interFreqNeighCellListPresent = 0;
  curr_ptr->elem[0].m.p_MaxPresent = 0;
  curr_ptr->elem[0].m.t_ReselectionEUTRA_SFPresent = 0;
  curr_ptr->elem[0].m.cellReselectionPriorityPresent = 1;
  curr_ptr->elem[0].dl_CarrierFreq = 5;
  curr_ptr->elem[0].allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  curr_ptr->elem[0].t_ReselectionEUTRA = 3;
  curr_ptr->elem[0].cellReselectionPriority = 0;
  curr_ptr->elem[0].q_RxLevMin = -22;
  curr_ptr->elem[0].threshX_High = 20;
  curr_ptr->elem[0].threshX_Low = 22;
  curr_ptr->elem[0].q_OffsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;
  nCellConfig_integer_val = 2;
  /* Convert crnti from int to ossBitString type */
  status = lte_rrc_osys_asn1_int_to_bitstring(nCellConfig_integer_val,&curr_ptr->elem[0].neighCellConfig);
  TF_ASSERT(status == E_SUCCESS);
  curr_ptr->elem[0].presenceAntennaPort1 = 1;
  lte_rrc_asn1_osys_IE_free(curr_ptr->elem[0].interFreqBlackCellList.elem);
  curr_ptr->elem[0].interFreqBlackCellList.n = 0;
  curr_ptr->elem[0].interFreqBlackCellList.elem = NULL;
  lte_rrc_asn1_osys_IE_free(curr_ptr->elem[0].interFreqNeighCellList.elem);
  curr_ptr->elem[0].interFreqNeighCellList.n = 0;
  curr_ptr->elem[0].interFreqNeighCellList.elem = NULL;

  curr_ptr->elem = (lte_rrc_osys_InterFreqCarrierFreqInfo*) 
      lte_rrc_asn1_osys_IE_realloc(curr_ptr->elem, sizeof(lte_rrc_osys_InterFreqCarrierFreqInfo) * 8);
  curr_ptr->n = 8;
  
  for (i = 1; i < 8; i++)
  {
    curr_ptr->elem[i].m.cellReselectionPriorityPresent = 0;
    curr_ptr->elem[i].m.interFreqBlackCellListPresent = 0;
    curr_ptr->elem[i].m.interFreqNeighCellListPresent = 0;
    curr_ptr->elem[i].m.p_MaxPresent = 0;
    curr_ptr->elem[i].m.t_ReselectionEUTRA_SFPresent = 0;
    curr_ptr->elem[i].m.cellReselectionPriorityPresent = 1;
    curr_ptr->elem[i].dl_CarrierFreq = (5 * (i+1));
    curr_ptr->elem[i].allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
    curr_ptr->elem[i].t_ReselectionEUTRA = 3;
    curr_ptr->elem[i].cellReselectionPriority = 0;
    curr_ptr->elem[i].q_RxLevMin = -22;
    curr_ptr->elem[i].threshX_High = 20;
    curr_ptr->elem[i].threshX_Low = 22;
    curr_ptr->elem[i].presenceAntennaPort1 = 1;

    nCellConfig_integer_val = 2;
    /*Fill out the neighbor cell config  value*/
    curr_ptr->elem[i].neighCellConfig.numbits = 2;

    /* Convert crnti from int to ossBitString type */
    status = lte_rrc_osys_asn1_int_to_bitstring(nCellConfig_integer_val,&curr_ptr->elem[i].neighCellConfig);
    TF_ASSERT(status == E_SUCCESS);

    curr_ptr->elem[i].interFreqBlackCellList.elem = NULL;
    curr_ptr->elem[i].interFreqBlackCellList.n = 0;
    curr_ptr->elem[i].interFreqNeighCellList.elem = NULL;
    curr_ptr->elem[i].interFreqNeighCellList.n = 0;
    curr_ptr->elem[i].q_OffsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;
  }
  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);

  // Verify inter-freq info


  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == (LTE_CPHY_MEAS_MAX_NUM_FREQ - 1));
  for (i = 0; i < (LTE_CPHY_MEAS_MAX_NUM_FREQ - 1); i++)
  {
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[i].dl_carrier_freq == (5 * (i+1)));
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[i].t_resel_eutran == 3);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[i].t_resel_eutran_medium == 
              LTE_CPHY_MEAS_TIME_SF_1DOT0);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[i].t_resel_eutran_high == 
	  		LTE_CPHY_MEAS_TIME_SF_1DOT0);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[i].threshX_high == 40);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[i].threshX_low == 44);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[i].meas_bandwidth == 
              LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[i].cell_resel_priority == 0);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[i].q_offset_freq == 0);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[i].whitelist_num_cells == 0);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[i].blacklist_num_cells == 0);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[i].antenna_port_1 == TRUE);

    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[i].neighbourCellConfig == 2);
    TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[i].inter_freq_resel_info_rel9_present == TRUE);
  }

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

}/* LteRrcMeasReselInterFreqMaxList::Test() */

void LteRrcMeasReselInterFreqMaxList::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqMaxList::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqPresenceAntennaPort1Absent);

void LteRrcMeasReselInterFreqPresenceAntennaPort1Absent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqPresenceAntennaPort1Absent::Setup() */

void LteRrcMeasReselInterFreqPresenceAntennaPort1Absent::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ; 
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
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

  // Point the SIBs to be returned to the typical SIB3 , SIB4 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Set PresenceAntennaPort 1 to TRUE
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].presenceAntennaPort1 = 0;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Indicate that SIB3, SIB4, SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);
    

  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			  LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
            LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == FALSE);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);
	
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

}/* LteRrcMeasReselInterFreqPresenceAntennaPort1Absent::Test() */

void LteRrcMeasReselInterFreqPresenceAntennaPort1Absent::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqPresenceAntennaPort1Absent::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqDupNeighborCells);

void LteRrcMeasReselInterFreqDupNeighborCells::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqDupNeighborCells::Setup() */

void LteRrcMeasReselInterFreqDupNeighborCells::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ; 
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));

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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_with_dup_neigh_cells,
                                sizeof(lte_rrc_meas_test_sib5_with_dup_neigh_cells));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));  
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 
 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);

  // Verify inter-freq info
  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max == -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
            LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
            LTE_CPHY_MEAS_TIME_SF_0DOT75);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);  
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci == 4);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -24);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].antenna_port_1 == TRUE);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].neighbourCellConfig == 3);

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

}/* LteRrcMeasReselInterFreqDupNeighborCells::Test() */

void LteRrcMeasReselInterFreqDupNeighborCells::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqDupNeighborCells::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselInterFreqDupFreqs);

void LteRrcMeasReselInterFreqDupFreqs::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqDupFreqs::Setup() */

void LteRrcMeasReselInterFreqDupFreqs::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));

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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_with_dup_freq,
                                sizeof(lte_rrc_meas_test_sib5_with_dup_freq));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Indicate that SIB3, SIB4 and SIB5 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
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
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));  
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 
 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);

  // Verify inter-freq info
  //Verify inter-freq info
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].dl_carrier_freq == 5200);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].q_rxlevmin == -50);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[2].dl_carrier_freq == 5220);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[2].q_rxlevmin == -52);


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

}/* LteRrcMeasReselInterFreqDupFreqs::Test() */

void LteRrcMeasReselInterFreqDupFreqs::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqDupFreqs::Teardown() */


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasInterFreqListinConn);

void LteRrcMeasInterFreqListinConn::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_RRC_INTERFREQ_LIST_UPDATE_INDI
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasReselInterFreqTyp::Setup() */

void LteRrcMeasInterFreqListinConn::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_rrc_interfreq_list_update_indi_s *interFreq_list_ind;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();
  
  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();
/*
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&interFreq_list_ind, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_interfreq_list_update_indi_s));
  TF_ASSERT(interFreq_list_ind->msg_hdr.id == LTE_RRC_INTERFREQ_LIST_UPDATE_INDI);
*/
  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasReselInterFreqTyp::Test() */

void LteRrcMeasInterFreqListinConn::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasReselInterFreqTyp::Teardown() */





