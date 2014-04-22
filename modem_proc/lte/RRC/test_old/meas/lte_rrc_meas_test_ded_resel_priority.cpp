/*!
  @file lte_rrc_meas_test_ded_resel_priority.cpp

  @brief
  Unit test code for the meas procedure.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/meas/lte_rrc_meas_test_ded_resel_priority.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/10   svk     Modified the name from UTRA to WCDMA
02/26/10   mm      Added test cases for testing dedicated priorities when 
                   SIB6, SIB7 and SIB8 are signaled
11/20/09   da      Initial Revision
           
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
#include "lte_rrc_controller.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_meas_test_msg.h"
#include <TestFramework.h>
#include <tf_stub.h>

extern "C"
{
  TF_PROTO_STUB(lte_rrc_revoke_priority_e, lte_rrc_get_revoke_priority, lte_rrc_revoke_trigger_e);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);
}

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_meas_test_ded_resel_priority_s.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */

} lte_rrc_meas_test_ded_resel_priority_s;


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_meas_test_idle_intra.cpp
*/
DECLARE_LTE_RRC_MEAS_TEST_SIB3_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB4_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB5_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB5_MSG2;
DECLARE_LTE_RRC_MEAS_TEST_SIB6_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB7_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB8_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB8_MSG2;

extern "C"
{
  extern lte_rrc_meas_s lte_rrc_meas_data;
}

extern "C"
{
  //TF_PROTO_STUB(sys_sband_e_type, lte_rrc_get_gsm_band_from_arfcn, geran_grr_band_ind_e, uint16, boolean *);
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
#if 0
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasDedReselPriorityInterFreq);

void LteRrcMeasDedReselPriorityInterFreq::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasDedReselPriorityInterFreq::Setup() */

void LteRrcMeasDedReselPriorityInterFreq::Test()
{
  lte_rrc_camped_indi_s         camped_indi; 
  lte_rrc_sib_updated_indi_s    sib_updated_indi; 
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf; 
  lte_rrc_ded_priority_list_indi_s priority_list_ind;
  lte_rrc_conn_released_indi_s  conn_rel_indi;
  lte_rrc_connected_indi_s      connected_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  const lte_irat_dedicated_priority_list_s *freq_list_ptr = NULL;
  uint64                        t_320_value        = 0;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  memset(&priority_list_ind, 0, sizeof(lte_rrc_ded_priority_list_indi_s));
  memset(&conn_rel_indi, 0, sizeof(lte_rrc_conn_released_indi_s));

  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_init_default_hdr(&priority_list_ind.msg_hdr, LTE_RRC_DED_PRIORITY_LIST_INDI);

  priority_list_ind.t_320_is_present = TRUE;
  priority_list_ind.t_320_ms = 300000;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
  priority_list_ind.freq_list.num_hrdp_band_class = 0;
  priority_list_ind.freq_list.num_1x_band_class = 0;
  priority_list_ind.freq_list.num_eutra_freq = 2;

  // Freq in Sib 5
  priority_list_ind.freq_list.eutra_freq_list[0].arfcn = 5180;
  priority_list_ind.freq_list.eutra_freq_list[0].priority = 5;
  priority_list_ind.freq_list.eutra_freq_list[1].arfcn = 1000;
  priority_list_ind.freq_list.eutra_freq_list[1].priority = 0;

  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));

  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg2,
                                sizeof(lte_rrc_meas_test_sib5_msg2));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
	                                      LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
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

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].dl_carrier_freq == 1000);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].cell_resel_priority == 0);

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

  t_320_value = lte_rrc_meas_get_dedicated_priority_info(&freq_list_ptr);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[0].arfcn == 5180);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[0].priority == 5);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[1].arfcn == 1000);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[1].priority == 0);
  TF_MSG("Current T320 value in ms is %d", t_320_value);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasDedReselPriorityInterFreq::Test() */

void LteRrcMeasDedReselPriorityInterFreq::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasDedReselPriorityInterFreq::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasDedReselPriorityIntraInterFreq);

void LteRrcMeasDedReselPriorityIntraInterFreq::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasDedReselPriorityIntraInterFreq::Setup() */

void LteRrcMeasDedReselPriorityIntraInterFreq::Test()
{
  lte_rrc_camped_indi_s         camped_indi;
  lte_rrc_sib_updated_indi_s    sib_updated_indi;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf;
  lte_rrc_ded_priority_list_indi_s priority_list_ind;
  lte_rrc_conn_released_indi_s  conn_rel_indi;
  lte_rrc_connected_indi_s      connected_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  const lte_irat_dedicated_priority_list_s *freq_list_ptr = NULL;
  uint64                        t_320_value        = 0;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  memset(&priority_list_ind, 0, sizeof(lte_rrc_ded_priority_list_indi_s));
  memset(&conn_rel_indi, 0, sizeof(lte_rrc_conn_released_indi_s));

  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_init_default_hdr(&priority_list_ind.msg_hdr, LTE_RRC_DED_PRIORITY_LIST_INDI);

  priority_list_ind.t_320_is_present = TRUE;
  priority_list_ind.t_320_ms = 300000;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
  priority_list_ind.freq_list.num_hrdp_band_class = 0;
  priority_list_ind.freq_list.num_1x_band_class = 0;
  priority_list_ind.freq_list.num_eutra_freq = 2;
  // Serving freq info
  priority_list_ind.freq_list.eutra_freq_list[0].arfcn = 0x100;
  priority_list_ind.freq_list.eutra_freq_list[0].priority = 4;
  // Freq in Sib 5
  priority_list_ind.freq_list.eutra_freq_list[1].arfcn = 5180;
  priority_list_ind.freq_list.eutra_freq_list[1].priority = 5;

  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));

  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  // Verify cell resel priority for serving is according to the dedicated priority
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 4);


  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 5);

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

  t_320_value = lte_rrc_meas_get_dedicated_priority_info(&freq_list_ptr);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[0].arfcn == 0x100);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[0].priority == 4);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[1].arfcn == 5180);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[1].priority == 5);
  TF_MSG("Current T320 value in ms is %d", t_320_value);

  //----------------------------------------------------------------------------    


  // Take meas to conneted state, and try dedicated priority list indication 
  // as though it was comming in rrc connection release

  TF_MSG("Sending Connected indication");
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));
  
  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Make sure T320 is stopped on connected mode transition. 
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_STOPPED); 

  // Make sure owner of dedicated priorities is NONE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_NONE);

  priority_list_ind.t_320_ms = 300000;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
  priority_list_ind.freq_list.num_hrdp_band_class = 0;
  priority_list_ind.freq_list.num_1x_band_class = 0;
  priority_list_ind.freq_list.num_eutra_freq = 2;
  // Serving freq info
  priority_list_ind.freq_list.eutra_freq_list[0].arfcn = 0x100;
  priority_list_ind.freq_list.eutra_freq_list[0].priority = 1;
  // Freq in Sib 5
  priority_list_ind.freq_list.eutra_freq_list[1].arfcn = 5180;
  priority_list_ind.freq_list.eutra_freq_list[1].priority = 3;

  // Send dedicated priority info
  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));
  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();
  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

  // Transition to IDLE
  TF_MSG("Sending the ConnReleased indication");
  lte_rrc_init_default_hdr(&conn_rel_indi.msg_hdr, LTE_RRC_CONN_RELEASED_INDI);
  lte_rrc_utf_send_msg((byte*)&conn_rel_indi, sizeof(conn_rel_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);


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

  // Verify cell resel priority for serving is according to the dedicated priority
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 1);


  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 3);

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

}/* LteRrcMeasDedReselPriorityIntraInterFreq::Test() */

void LteRrcMeasDedReselPriorityIntraInterFreq::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasDedReselPriorityIntraInterFreq::Teardown() */


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasDedReselPrioritySIB3456);

void LteRrcMeasDedReselPrioritySIB3456::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasDedReselPrioritySIB3456::Setup() */

void LteRrcMeasDedReselPrioritySIB3456::Test()
{
  lte_rrc_camped_indi_s         camped_indi;
  lte_rrc_sib_updated_indi_s    sib_updated_indi;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf;
  lte_rrc_ded_priority_list_indi_s priority_list_ind;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  memset(&priority_list_ind, 0, sizeof(lte_rrc_ded_priority_list_indi_s));


  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_init_default_hdr(&priority_list_ind.msg_hdr, LTE_RRC_DED_PRIORITY_LIST_INDI);

  priority_list_ind.t_320_is_present = FALSE;
  priority_list_ind.t_320_ms = 0;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
  priority_list_ind.freq_list.num_1x_band_class = 0;
  priority_list_ind.freq_list.num_eutra_freq = 3;
  // Serving freq info
  priority_list_ind.freq_list.eutra_freq_list[0].arfcn = 0x100;
  priority_list_ind.freq_list.eutra_freq_list[0].priority = 4;
  // Freq in Sib 5
  priority_list_ind.freq_list.eutra_freq_list[1].arfcn = 5180;
  priority_list_ind.freq_list.eutra_freq_list[1].priority = 5;
  // Include extra freq not included in sib.  It shall be ingored.
  priority_list_ind.freq_list.eutra_freq_list[2].arfcn = 2000;
  priority_list_ind.freq_list.eutra_freq_list[2].priority = 5;

  // Populate UTRA FDD list
  priority_list_ind.freq_list.num_utra_fdd_freq = 3;
  priority_list_ind.freq_list.utra_fdd_freq_list[0].arfcn = 100;
  priority_list_ind.freq_list.utra_fdd_freq_list[0].priority = 2;
  priority_list_ind.freq_list.utra_fdd_freq_list[1].arfcn = 200;
  priority_list_ind.freq_list.utra_fdd_freq_list[1].priority = 2;
  priority_list_ind.freq_list.utra_fdd_freq_list[2].arfcn = 300;
  priority_list_ind.freq_list.utra_fdd_freq_list[2].priority = 2;
  
  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));

  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_INFINITY);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  lte_rrc_meas_test_decode_sib6((void*) lte_rrc_meas_test_sib6_msg1,
                                sizeof(lte_rrc_meas_test_sib6_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB6_BITMASK ;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                          LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB6_BITMASK;
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
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS ));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify cell resel priority for serving is according to the dedicated priority
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 4);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 5);
  
  // Verify cell reselection priority for WCDMA FDD freqs
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].freq == 100);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].cell_resel_priority == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].q_rx_lev_min_actual == -59);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].q_qual_min == -10);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].p_max == 5);

  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].freq == 200);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].cell_resel_priority == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].thresh_Xhigh == 50);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].thresh_Xlow == 20);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].q_rx_lev_min_actual == -59);

  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[2].freq == 300);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[2].cell_resel_priority == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[2].thresh_Xhigh == 50);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[2].thresh_Xlow == 24);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[2].q_rx_lev_min_actual == -79);
  
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

}/* LteRrcMeasDedReselPrioritySIB3456::Test() */

void LteRrcMeasDedReselPrioritySIB3456::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasDedReselPrioritySIB3456::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasDedReselPrioritySIB3457);

void LteRrcMeasDedReselPrioritySIB3457::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasDedReselPrioritySIB3457::Setup() */

void LteRrcMeasDedReselPrioritySIB3457::Test()
{
  lte_rrc_camped_indi_s         camped_indi;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf;
  lte_rrc_ded_priority_list_indi_s priority_list_ind;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  memset(&priority_list_ind, 0, sizeof(lte_rrc_ded_priority_list_indi_s));


  // Initialize the proto stubs
  lte_rrc_meas_test_init();

  //TF_STUB(lte_rrc_get_gsm_band_from_arfcn) = lte_rrc_meas_test_stub__get_gsm_band_from_arfcn;
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_init_default_hdr(&priority_list_ind.msg_hdr, LTE_RRC_DED_PRIORITY_LIST_INDI);

  priority_list_ind.t_320_is_present = TRUE;
  priority_list_ind.t_320_ms = 300000;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
  priority_list_ind.freq_list.num_1x_band_class = 0;
  priority_list_ind.freq_list.num_eutra_freq = 3;
  // Serving freq info
  priority_list_ind.freq_list.eutra_freq_list[0].arfcn = 0x100;
  priority_list_ind.freq_list.eutra_freq_list[0].priority = 4;
  // Freq in Sib 5
  priority_list_ind.freq_list.eutra_freq_list[1].arfcn = 5180;
  priority_list_ind.freq_list.eutra_freq_list[1].priority = 5;
  // Include extra freq not included in sib.  It shall be ingored.
  priority_list_ind.freq_list.eutra_freq_list[2].arfcn = 2000;
  priority_list_ind.freq_list.eutra_freq_list[2].priority = 5;

  // Populate UTRA FDD list
  priority_list_ind.freq_list.num_geran_freq = 2;
  priority_list_ind.freq_list.geran_freq_list[0].arfcn = 85;
  priority_list_ind.freq_list.geran_freq_list[0].priority = 7;
  priority_list_ind.freq_list.geran_freq_list[1].arfcn = 175;
  priority_list_ind.freq_list.geran_freq_list[1].priority = 6;
  
  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));

  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  lte_rrc_meas_test_decode_sib7((void*) lte_rrc_meas_test_sib7_msg1,
                                sizeof(lte_rrc_meas_test_sib7_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB7_BITMASK ;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                          LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
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
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify cell resel priority for serving is according to the dedicated priority
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 4);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 5);

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

}/* LteRrcMeasDedReselPrioritySIB3457::Test() */

void LteRrcMeasDedReselPrioritySIB3457::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasDedReselPrioritySIB3457::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasDedReselPriorityComparingGsmBand);

void LteRrcMeasDedReselPriorityComparingGsmBand::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasDedReselPrioritySIB3457::Setup() */

void LteRrcMeasDedReselPriorityComparingGsmBand::Test()
{
  lte_rrc_camped_indi_s         camped_indi;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf;
  lte_rrc_ded_priority_list_indi_s priority_list_ind;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  memset(&priority_list_ind, 0, sizeof(lte_rrc_ded_priority_list_indi_s));


  // Initialize the proto stubs
  lte_rrc_meas_test_init();

  //TF_STUB(lte_rrc_get_gsm_band_from_arfcn) = lte_rrc_meas_test_stub__get_gsm_band_from_arfcn;
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_init_default_hdr(&priority_list_ind.msg_hdr, LTE_RRC_DED_PRIORITY_LIST_INDI);

  priority_list_ind.t_320_is_present = TRUE;
  priority_list_ind.t_320_ms = 300000;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
 
  priority_list_ind.freq_list.num_geran_freq = 3;
  priority_list_ind.freq_list.geran_freq_list[0].arfcn = 55;
  priority_list_ind.freq_list.geran_freq_list[0].priority = 7;
  priority_list_ind.freq_list.geran_freq_list[0].band = SYS_SBAND_GSM_DCS_1800;
  
  priority_list_ind.freq_list.geran_freq_list[1].arfcn = 60;
  priority_list_ind.freq_list.geran_freq_list[1].priority = 7;
  priority_list_ind.freq_list.geran_freq_list[1].band = SYS_SBAND_GSM_DCS_1800;
  
  priority_list_ind.freq_list.geran_freq_list[2].arfcn = 150;
  priority_list_ind.freq_list.geran_freq_list[2].priority = 7;
  priority_list_ind.freq_list.geran_freq_list[2].band = SYS_SBAND_GSM_DCS_1800;
  
  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));

  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  lte_rrc_meas_test_decode_sib7((void*) lte_rrc_meas_test_sib7_msg1,
                                sizeof(lte_rrc_meas_test_sib7_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB7_BITMASK ;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                          LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
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
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ));

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify cell resel priority for serving is according to the dedicated priority
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == LTE_CPHY_MEAS_RESEL_PRIORITY_NONE);
  
  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  // Send a camped indication, Meas SM should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_indi.reason = LTE_RRC_CAMPED_CELL_RESELECTION;
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

  lte_rrc_meas_test_decode_sib7((void*) lte_rrc_meas_test_sib7_msg1,
                                sizeof(lte_rrc_meas_test_sib7_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB7_BITMASK ;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                          LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;
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
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ));

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify cell resel priority for serving is according to the dedicated priority
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == LTE_CPHY_MEAS_RESEL_PRIORITY_NONE);
  
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

}/* LteRrcMeasDedReselPrioritySIB3457::Test() */

void LteRrcMeasDedReselPriorityComparingGsmBand::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasDedReselPrioritySIB3457::Teardown() */



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasDedReselPrioritySIB3458);

void LteRrcMeasDedReselPrioritySIB3458::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasDedReselPrioritySIB3458::Setup() */

void LteRrcMeasDedReselPrioritySIB3458::Test()
{
  lte_rrc_camped_indi_s         camped_indi;
  lte_rrc_sib_updated_indi_s    sib_updated_indi;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf;
  lte_rrc_ded_priority_list_indi_s priority_list_ind;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  memset(&priority_list_ind, 0, sizeof(lte_rrc_ded_priority_list_indi_s));


  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_init_default_hdr(&priority_list_ind.msg_hdr, LTE_RRC_DED_PRIORITY_LIST_INDI);


  priority_list_ind.t_320_is_present = TRUE;
  priority_list_ind.t_320_ms = 300000;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
  priority_list_ind.freq_list.num_1x_band_class = 0;
  priority_list_ind.freq_list.num_eutra_freq = 3;
  // Serving freq info
  priority_list_ind.freq_list.eutra_freq_list[0].arfcn = 0x100;
  priority_list_ind.freq_list.eutra_freq_list[0].priority = 4;
  // Freq in Sib 5
  priority_list_ind.freq_list.eutra_freq_list[1].arfcn = 5180;
  priority_list_ind.freq_list.eutra_freq_list[1].priority = 5;
  // Include extra freq not included in sib.  It shall be ingored.
  priority_list_ind.freq_list.eutra_freq_list[2].arfcn = 2000;
  priority_list_ind.freq_list.eutra_freq_list[2].priority = 5;

  // Populate EHRPD list
  priority_list_ind.freq_list.num_hrdp_band_class = 3;
  priority_list_ind.freq_list.cdma_hrpd_list[0].band_class = SYS_BAND_BC0;
  priority_list_ind.freq_list.cdma_hrpd_list[0].cell_resel_priority = 0;
  priority_list_ind.freq_list.cdma_hrpd_list[1].band_class = SYS_BAND_BC1;
  priority_list_ind.freq_list.cdma_hrpd_list[1].cell_resel_priority = 0;
  priority_list_ind.freq_list.cdma_hrpd_list[2].band_class = SYS_BAND_BC3;
  priority_list_ind.freq_list.cdma_hrpd_list[2].cell_resel_priority = 0;

  // Populate 1x list - give less that the bands listed in sib
  priority_list_ind.freq_list.num_1x_band_class = 1;
  priority_list_ind.freq_list.cdma_1x_list[0].band_class = SYS_BAND_BC16;
  priority_list_ind.freq_list.cdma_1x_list[0].cell_resel_priority = 4;


  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));

  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  lte_rrc_meas_test_decode_sib8((void*) lte_rrc_meas_test_sib8_msg1,
                                sizeof(lte_rrc_meas_test_sib8_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB5_BITMASK |
                                          LTE_RRC_SIB8_BITMASK ;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                          LTE_RRC_SIB5_BITMASK |
                                            LTE_RRC_SIB8_BITMASK;
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
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify cell resel priority for serving is according to the dedicated priority
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 4);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 5);

  // Verify bandclass list for eHRPD
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_ehrpd_resel_enabled == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list_size == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[0].band_class == SYS_BAND_BC0);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[0].cell_resel_priority == 0);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[0].threshX_high == 60);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[0].threshX_low == 10);

  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[1].band_class == SYS_BAND_BC3);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[1].cell_resel_priority == 0);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[1].threshX_high == 50);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[1].threshX_low == 30);
  
  // Verify bandclass list for 1X
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_enabled == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list_size = 1);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list[0].band_class == SYS_BAND_BC16);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list[0].cell_resel_priority == 4);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list[0].threshX_high == 40);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list[0].threshX_low == 20);

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

}/* LteRrcMeasDedReselPrioritySIB3458::Test() */

void LteRrcMeasDedReselPrioritySIB3458::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasDedReselPrioritySIB3458::Teardown() */


//==============================================================================

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasDedReselPriorityTimerExpiryDuringOos);

void LteRrcMeasDedReselPriorityTimerExpiryDuringOos::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasDedReselPriorityOnAcceptableCell::Setup() */

void LteRrcMeasDedReselPriorityTimerExpiryDuringOos::Test()
{
  lte_rrc_camped_indi_s         camped_indi;
  lte_rrc_sib_updated_indi_s    sib_updated_indi;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf;
  lte_rrc_ded_priority_list_indi_s priority_list_ind;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  lte_rrc_tmri_s *t320_tmri_ptr                    = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  memset(&priority_list_ind, 0, sizeof(lte_rrc_ded_priority_list_indi_s));


  // Initialize the proto stubs
  lte_rrc_meas_test_init();


  // Changed the camped status to acceptable
  lte_rrc_meas_test_camped_status = LTE_RRC_STATUS_CAMPED_ACCEPTABLE;

  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_init_default_hdr(&priority_list_ind.msg_hdr, LTE_RRC_DED_PRIORITY_LIST_INDI);


  priority_list_ind.t_320_is_present = TRUE;
  priority_list_ind.t_320_ms = 5;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
  priority_list_ind.freq_list.num_hrdp_band_class = 0;
  priority_list_ind.freq_list.num_1x_band_class = 0;
  priority_list_ind.freq_list.num_eutra_freq = 2;
  // Serving freq info
  priority_list_ind.freq_list.eutra_freq_list[0].arfcn = 0x100;
  priority_list_ind.freq_list.eutra_freq_list[0].priority = 4;
  // Freq in Sib 5
  priority_list_ind.freq_list.eutra_freq_list[1].arfcn = 5180;
  priority_list_ind.freq_list.eutra_freq_list[1].priority = 5;

  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));

  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  // Verify cell resel priority for serving is according to sibs
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);

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

  //----------------------------------------------------------------------------    


  // Now change the cell camping status back to suitable and send SIB update.
  // Changed the camped status to acceptable
  lte_rrc_meas_test_camped_status = LTE_RRC_STATUS_CAMPED_SUITABLE;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  // Verify cell resel priority for serving is according to dedicated cell priority info
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 4);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 5);

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

  //----------------------------------------------------------------------------

  // Change the camped status back to acceptable and expire T320.

  lte_rrc_meas_test_camped_status = LTE_RRC_STATUS_CAMPED_ACCEPTABLE;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);
  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  // Verify cell resel priority for serving is according to sibs
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);

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

  // Set csp_get_serving_cell_params fn to return E_FAILURE
  TF_STUB(lte_rrc_csp_get_serving_cell_params) =
    lte_rrc_meas_test_stub__csp_get_serving_cell_params_failure;

  //Let T320 expire.  Expect idle meas cfg req with SIB parameters.
  TF_SLEEP(5);
  TF_MSG("T320 timer expires");

  // Catch the T320 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t320_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);

  // We expect meas sm to stay in the same state and not send anything to ML1
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_STOPPED);

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasDedReselPriorityOnAcceptableCell::Test() */

void LteRrcMeasDedReselPriorityTimerExpiryDuringOos::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasDedReselPriorityOnAcceptableCell::Teardown() */


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasDedReselPriorityOnAcceptableCell);

void LteRrcMeasDedReselPriorityOnAcceptableCell::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasDedReselPriorityOnAcceptableCell::Setup() */

void LteRrcMeasDedReselPriorityOnAcceptableCell::Test()
{
  lte_rrc_camped_indi_s         camped_indi;
  lte_rrc_sib_updated_indi_s    sib_updated_indi;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf;
  lte_rrc_ded_priority_list_indi_s priority_list_ind;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  lte_rrc_tmri_s *t320_tmri_ptr                    = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  memset(&priority_list_ind, 0, sizeof(lte_rrc_ded_priority_list_indi_s));


  // Initialize the proto stubs
  lte_rrc_meas_test_init();


  // Changed the camped status to acceptable
  lte_rrc_meas_test_camped_status = LTE_RRC_STATUS_CAMPED_ACCEPTABLE;

  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_init_default_hdr(&priority_list_ind.msg_hdr, LTE_RRC_DED_PRIORITY_LIST_INDI);


  priority_list_ind.t_320_is_present = TRUE;
  priority_list_ind.t_320_ms = 5;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
  priority_list_ind.freq_list.num_hrdp_band_class = 0;
  priority_list_ind.freq_list.num_1x_band_class = 0;
  priority_list_ind.freq_list.num_eutra_freq = 2;
  // Serving freq info
  priority_list_ind.freq_list.eutra_freq_list[0].arfcn = 0x100;
  priority_list_ind.freq_list.eutra_freq_list[0].priority = 4;
  // Freq in Sib 5
  priority_list_ind.freq_list.eutra_freq_list[1].arfcn = 5180;
  priority_list_ind.freq_list.eutra_freq_list[1].priority = 5;

  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));

  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  // Verify cell resel priority for serving is according to sibs
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);

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

  //----------------------------------------------------------------------------    


  // Now change the cell camping status back to suitable and send SIB update.
  // Changed the camped status to acceptable
  lte_rrc_meas_test_camped_status = LTE_RRC_STATUS_CAMPED_SUITABLE;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  // Verify cell resel priority for serving is according to dedicated cell priority info
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 4);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 5);

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

  //----------------------------------------------------------------------------

  // Change the camped status back to acceptable and expire T320.

  lte_rrc_meas_test_camped_status = LTE_RRC_STATUS_CAMPED_ACCEPTABLE;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);
  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  // Verify cell resel priority for serving is according to sibs
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);

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

  //Let T320 expire.  Expect idle meas cfg req with SIB parameters.
  TF_SLEEP(5);
  TF_MSG("T320 timer expires");

  // Catch the T320 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t320_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);


  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);
  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_STOPPED);

  // Make sure owner of dedicated priorities is NONE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_NONE);

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

  // Verify cell resel priority for serving is according to sib info
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);


  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to stay in the same state and not send anything to ML1
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_STOPPED);

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasDedReselPriorityOnAcceptableCell::Test() */

void LteRrcMeasDedReselPriorityOnAcceptableCell::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasDedReselPriorityOnAcceptableCell::Teardown() */



//==============================================================================


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasDedReselPriorityIntraInterFreqT320Expire);

void LteRrcMeasDedReselPriorityIntraInterFreqT320Expire::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasDedReselPriorityIntraInterFreq::Setup() */

void LteRrcMeasDedReselPriorityIntraInterFreqT320Expire::Test()
{
  lte_rrc_camped_indi_s         camped_indi;
  lte_rrc_sib_updated_indi_s    sib_updated_indi;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf;
  lte_rrc_ded_priority_list_indi_s priority_list_ind; 
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  lte_rrc_tmri_s *t320_tmri_ptr                    = NULL;
  lte_rrc_ml1_revoke_reqi_s     *revoke_req        = NULL;
  lte_rrc_conn_released_indi_s  conn_rel_indi;
  lte_rrc_connected_indi_s      connected_indi;
  lte_rrc_not_camped_indi_s     not_camped_indi;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  memset(&priority_list_ind, 0, sizeof(lte_rrc_ded_priority_list_indi_s));



  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_init_default_hdr(&priority_list_ind.msg_hdr, LTE_RRC_DED_PRIORITY_LIST_INDI);


  priority_list_ind.t_320_is_present = TRUE;
  priority_list_ind.t_320_ms = 5;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
  priority_list_ind.freq_list.num_hrdp_band_class = 0;
  priority_list_ind.freq_list.num_1x_band_class = 0;
  priority_list_ind.freq_list.num_eutra_freq = 2;
  // Serving freq info
  priority_list_ind.freq_list.eutra_freq_list[0].arfcn = 0x100;
  priority_list_ind.freq_list.eutra_freq_list[0].priority = 4;
  // Freq in Sib 5
  priority_list_ind.freq_list.eutra_freq_list[1].arfcn = 5180;
  priority_list_ind.freq_list.eutra_freq_list[1].priority = 5;

  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));
  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  // Verify cell resel priority for serving is according to the dedicated priority
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 4);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 5);

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

  //Let T320 expire.  Expect idle meas cfg req with SIB parameters.
  TF_SLEEP(5);
  TF_MSG("T320 timer expires");

  // Catch the T320 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t320_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);
  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_STOPPED);

  // Make sure owner of dedicated priorities is NONE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_NONE);

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

  // Verify cell resel priority for serving is according to sib info
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);


  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);


  //----------------------------------------------------------------------------

  // Test T320 expiry in the middle of idle_meas_cfg_req.
  // Take meas to conneted state, and try dedicated priority list indication 
  // as though it was comming in rrc connection release

  TF_MSG("Sending Connected indication");
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Make sure T320 is stopped on connected mode transition. 
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_STOPPED); 

  // Make sure owner of dedicated priorities is NONE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_NONE);

  // Send dedicated priority info
  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));
  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();
  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

  // Transition to IDLE
  TF_MSG("Sending the ConnReleased indication");
  lte_rrc_init_default_hdr(&conn_rel_indi.msg_hdr, LTE_RRC_CONN_RELEASED_INDI);
  lte_rrc_utf_send_msg((byte*)&conn_rel_indi, sizeof(conn_rel_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Send SIB update ind
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  lte_rrc_utf_wait_for_done();
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // Expire T320
  TF_SLEEP(5);
  TF_MSG("T320 timer expires");

  // Catch the T320 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t320_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);


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

  // Verify cell resel priority for serving is according to the dedicated priority
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 4);


  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 5);

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


  // We expect meas sm to have thrown the idle meas cfg req due to T320 expiry
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_STOPPED);

  // Make sure owner of dedicated priorities is NONE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_NONE);

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

  // Verify cell resel priority for serving is according to sib info
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);


  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Catch the T320 Timer expiry - the pended T320 expiry comes again
  lte_rrc_utf_get_next_int_msg((byte **)&t320_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);


  //----------------------------------------------------------------------------
  // Test T320 expiry when controller state is suspended when BPLMN search is in
  // progress.  This test makes sure Meas modules revokes the priority of the 
  // BPLMN search and handles T320 expiry immediately 

  // Throw a not camped indication and take meas to inactive state

  // IDLE_CAMPED -> INACTIVE
  // Trigger event: LTE_RRC_NOT_CAMPED_INDI

  TF_MSG("Sending Not-Camped indication");
  lte_rrc_init_default_hdr(&not_camped_indi.msg_hdr, LTE_RRC_NOT_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&not_camped_indi, sizeof(not_camped_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);


  // Initiate ded cell resel priority indication
  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));
  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  // Take controller to suspended state
  TF_MSG("Controller state suspended");
  lte_rrc_meas_test_ctrl_state = LTE_RRC_CONTROLLER_SM__SUSPENDED;

  
  // Expire T320
  TF_SLEEP(5);
  TF_MSG("T320 timer expires");

  // Catch the T320 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t320_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);

  // Catch the revoke request sent by Meas 
  lte_rrc_utf_get_next_int_msg((byte **)&revoke_req,&buf_len);
  TF_ASSERT(revoke_req->msg_hdr.id == LTE_RRC_ML1_REVOKE_REQI);

  // Wait for Meas SM to pend the T320 expiry
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Now take controller to idle camped state
  TF_MSG("Controller state idle camped");
  lte_rrc_meas_test_ctrl_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;

  // Send camped indication
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  // Catch the pended T320 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t320_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);


  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  //----------------------------------------------------------------------------
  // Test T320 expiry when controller state is suspended and when revoke priority
  // returns NOT REQUIRED since inter-RAT reselection is in progress.  This test makes sure
  // that meas sends lower layer cmd after controller Resumes lower layers and transitions out of 
  // suspended state.
  
  // Throw a not camped indication and take meas to inactive state
  // IDLE_CAMPED -> INACTIVE
  // Trigger event: LTE_RRC_NOT_CAMPED_INDI
  
  TF_STUB(lte_rrc_get_revoke_priority) =
    lte_rrc_meas_test_stub__get_revoke_priority_failure;
  
  TF_MSG("Sending Not-Camped indication");
  lte_rrc_init_default_hdr(&not_camped_indi.msg_hdr, LTE_RRC_NOT_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&not_camped_indi, sizeof(not_camped_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));
  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  // Take controller to suspended state
  TF_MSG("Controller state suspended");
  lte_rrc_meas_test_ctrl_state = LTE_RRC_CONTROLLER_SM__SUSPENDED;
  
  // Expire T320
  TF_SLEEP(5);
  TF_MSG("T320 timer expires");

  // Catch the T320 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t320_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);

    // Take controller to IDLE_CAMPED state
  TF_MSG("Controller state idle camped");
  lte_rrc_meas_test_ctrl_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;

  // Send camped indication
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Catch the T320 Timer expiry - the pended T320 expiry comes again
  lte_rrc_utf_get_next_int_msg((byte **)&t320_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasDedReselPriorityIntraInterFreq::Test() */

void LteRrcMeasDedReselPriorityIntraInterFreqT320Expire::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasDedReselPriorityIntraInterFreqT320Expire::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasDedReselPriorityTransferToIRAT);

void LteRrcMeasDedReselPriorityTransferToIRAT::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_RRC_GET_DEDICATED_PRI_RSP,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasDedReselPriorityTransferToIRAT::Setup() */

void LteRrcMeasDedReselPriorityTransferToIRAT::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_rrc_ded_priority_list_indi_s priority_list_ind;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  lte_rrc_stopped_indi_s        lte_stopped_ind    ;
  lte_rrc_get_dedicated_pri_req_s irat_ded_pri_req ;
  lte_rrc_get_dedicated_pri_rsp_s *irat_ded_pri_rsp = NULL;
  lte_rrc_clear_dedicated_pri_ind_s clear_ded_pri_ind;
  const lte_irat_dedicated_priority_list_s *freq_list_ptr = NULL;
  uint64                        t_320_value        = 0;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  memset(&priority_list_ind, 0, sizeof(lte_rrc_ded_priority_list_indi_s));
  memset(&lte_stopped_ind, 0, sizeof(lte_rrc_stopped_indi_s));
  memset(&irat_ded_pri_req, 0, sizeof(lte_rrc_get_dedicated_pri_req_s));
  memset(&clear_ded_pri_ind, 0, sizeof(lte_rrc_clear_dedicated_pri_ind_s));

  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_init_default_hdr(&priority_list_ind.msg_hdr, LTE_RRC_DED_PRIORITY_LIST_INDI);

  priority_list_ind.t_320_is_present = TRUE;
  priority_list_ind.t_320_ms = 300000;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
  priority_list_ind.freq_list.num_hrdp_band_class = 0;
  priority_list_ind.freq_list.num_1x_band_class = 0;
  priority_list_ind.freq_list.num_eutra_freq = 2;
  // Serving freq info
  priority_list_ind.freq_list.eutra_freq_list[0].arfcn = 0x100;
  priority_list_ind.freq_list.eutra_freq_list[0].priority = 4;
  // Freq in Sib 5
  priority_list_ind.freq_list.eutra_freq_list[1].arfcn = 5180;
  priority_list_ind.freq_list.eutra_freq_list[1].priority = 5;

  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));

  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  // Verify cell resel priority for serving is according to the dedicated priority
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 4);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 5);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  t_320_value = lte_rrc_meas_get_dedicated_priority_info(&freq_list_ptr);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[0].arfcn == 0x100);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[0].priority == 4);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[1].arfcn == 5180);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[1].priority == 5);
  TF_MSG("Current T320 value in ms is %d", t_320_value);

  // Send LTE_RRC_STOPPED_INDI 
  lte_rrc_init_default_hdr(&lte_stopped_ind.msg_hdr, LTE_RRC_STOPPED_INDI);
  lte_rrc_utf_send_msg((byte*)&lte_stopped_ind, sizeof(lte_stopped_ind));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Make sure T320 is still running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is still LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

  // Send LTE_RRC_GET_DEDICATED_PRI_REQ to Meas
  lte_rrc_init_default_hdr(&irat_ded_pri_req.msg_hdr, LTE_RRC_GET_DEDICATED_PRI_REQ);
  lte_rrc_utf_send_msg((byte*)&irat_ded_pri_req, sizeof(irat_ded_pri_req));
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the irat_ded_pri_rsp
  lte_rrc_utf_get_next_ext_msg((byte**)&irat_ded_pri_rsp, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_dedicated_pri_rsp_s));
  TF_ASSERT(irat_ded_pri_rsp->msg_hdr.id == LTE_RRC_GET_DEDICATED_PRI_RSP);

  /* Verify the contents of LTE_RRC_GET_DEDICATED_PRI_RSP set by Meas */
  TF_ASSERT(irat_ded_pri_rsp->dedicated_priorities_valid == TRUE);
  TF_ASSERT(irat_ded_pri_rsp->dedicated_priority_info.rem_validity_timer_is_present == TRUE);
  TF_ASSERT(irat_ded_pri_rsp->dedicated_priority_info.rem_validity_timer == t_320_value);
  TF_ASSERT(irat_ded_pri_rsp->dedicated_priority_info.priority_list.eutra_freq_list[0].arfcn == 0x100);
  TF_ASSERT(irat_ded_pri_rsp->dedicated_priority_info.priority_list.eutra_freq_list[0].priority == 4);
  TF_ASSERT(irat_ded_pri_rsp->dedicated_priority_info.priority_list.eutra_freq_list[1].arfcn == 5180);
  TF_ASSERT(irat_ded_pri_rsp->dedicated_priority_info.priority_list.eutra_freq_list[1].priority == 5);
  TF_ASSERT(irat_ded_pri_rsp->plmn.identity[0] == 144);
  TF_ASSERT(irat_ded_pri_rsp->plmn.identity[1] == 65);
  TF_ASSERT(irat_ded_pri_rsp->plmn.identity[2] == 23);

  // mimic action of the other RAT
  lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_WCDMA);

  // Send LTE_RRC_CLEAR_DEDICATED_PRI_IND to Meas
  lte_rrc_init_default_hdr(&clear_ded_pri_ind.msg_hdr, LTE_RRC_CLEAR_DEDICATED_PRI_IND);
  lte_rrc_utf_send_msg((byte*)&clear_ded_pri_ind, sizeof(clear_ded_pri_ind));
  lte_rrc_utf_wait_for_done();

  // Meas should stop T320 and clear the dedicated priorities
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_STOPPED);

  // Make sure owner of dedicated priorities is still LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_WCDMA);

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasDedReselPriorityTransferToIRAT::Test() */

void LteRrcMeasDedReselPriorityTransferToIRAT::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasDedReselPriorityTransferToIRAT::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasDedReselPriorityT320ExpiryBeforeClearFromIRAT);

void LteRrcMeasDedReselPriorityT320ExpiryBeforeClearFromIRAT::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_RRC_GET_DEDICATED_PRI_RSP,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasDedReselPriorityT320ExpiryBeforeClearFromIRAT::Setup() */

void LteRrcMeasDedReselPriorityT320ExpiryBeforeClearFromIRAT::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_ded_priority_list_indi_s priority_list_ind;
  lte_rrc_stopped_indi_s        lte_stopped_ind    ;
  lte_rrc_get_dedicated_pri_req_s irat_ded_pri_req ;
  lte_rrc_get_dedicated_pri_rsp_s *irat_ded_pri_rsp = NULL;
  lte_rrc_tmri_s *t320_tmri_ptr                    = NULL;
  const lte_irat_dedicated_priority_list_s *freq_list_ptr = NULL;
  uint64                        t_320_value        = 0;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&priority_list_ind, 0, sizeof(lte_rrc_ded_priority_list_indi_s));
  memset(&lte_stopped_ind, 0, sizeof(lte_rrc_stopped_indi_s));
  memset(&irat_ded_pri_req, 0, sizeof(lte_rrc_get_dedicated_pri_req_s));

  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_init_default_hdr(&priority_list_ind.msg_hdr, LTE_RRC_DED_PRIORITY_LIST_INDI);

  priority_list_ind.t_320_is_present = TRUE;
  priority_list_ind.t_320_ms = 5;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
  priority_list_ind.freq_list.num_hrdp_band_class = 0;
  priority_list_ind.freq_list.num_1x_band_class = 0;
  priority_list_ind.freq_list.num_eutra_freq = 2;
  // Serving freq info
  priority_list_ind.freq_list.eutra_freq_list[0].arfcn = 0x100;
  priority_list_ind.freq_list.eutra_freq_list[0].priority = 4;
  // Freq in Sib 5
  priority_list_ind.freq_list.eutra_freq_list[1].arfcn = 5180;
  priority_list_ind.freq_list.eutra_freq_list[1].priority = 5;

  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));

  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  t_320_value = lte_rrc_meas_get_dedicated_priority_info(&freq_list_ptr);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[0].arfcn == 0x100);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[0].priority == 4);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[1].arfcn == 5180);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[1].priority == 5);
  TF_MSG("Current T320 value in ms is %d", t_320_value);

  // Send LTE_RRC_STOPPED_INDI 
  lte_rrc_init_default_hdr(&lte_stopped_ind.msg_hdr, LTE_RRC_STOPPED_INDI);
  lte_rrc_utf_send_msg((byte*)&lte_stopped_ind, sizeof(lte_stopped_ind));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Make sure T320 is still running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is still LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

  // Send LTE_RRC_GET_DEDICATED_PRI_REQ to Meas
  lte_rrc_init_default_hdr(&irat_ded_pri_req.msg_hdr, LTE_RRC_GET_DEDICATED_PRI_REQ);
  lte_rrc_utf_send_msg((byte*)&irat_ded_pri_req, sizeof(irat_ded_pri_req));
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the irat_ded_pri_rsp
  lte_rrc_utf_get_next_ext_msg((byte**)&irat_ded_pri_rsp, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_dedicated_pri_rsp_s));
  TF_ASSERT(irat_ded_pri_rsp->msg_hdr.id == LTE_RRC_GET_DEDICATED_PRI_RSP);

  /* Verify the contents of LTE_RRC_GET_DEDICATED_PRI_RSP set by Meas */
  TF_ASSERT(irat_ded_pri_rsp->dedicated_priorities_valid == TRUE);
  TF_ASSERT(irat_ded_pri_rsp->dedicated_priority_info.rem_validity_timer_is_present == TRUE);
  TF_ASSERT(irat_ded_pri_rsp->dedicated_priority_info.rem_validity_timer == t_320_value);
  TF_ASSERT(irat_ded_pri_rsp->dedicated_priority_info.priority_list.eutra_freq_list[0].arfcn == 0x100);
  TF_ASSERT(irat_ded_pri_rsp->dedicated_priority_info.priority_list.eutra_freq_list[0].priority == 4);
  TF_ASSERT(irat_ded_pri_rsp->dedicated_priority_info.priority_list.eutra_freq_list[1].arfcn == 5180);
  TF_ASSERT(irat_ded_pri_rsp->dedicated_priority_info.priority_list.eutra_freq_list[1].priority == 5);
  TF_ASSERT(irat_ded_pri_rsp->plmn.identity[0] == 144);
  TF_ASSERT(irat_ded_pri_rsp->plmn.identity[1] == 65);
  TF_ASSERT(irat_ded_pri_rsp->plmn.identity[2] == 23);
  
  // T320 expires
  TF_SLEEP(5);
  TF_MSG("T320 timer expires");

  // Catch the T320 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t320_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);

  // T320 should be stopped
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_STOPPED);

  // Make sure owner of dedicated priorities is NONE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_NONE);

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

}/* LteRrcMeasDedReselPriorityT320ExpiryBeforeClearFromIRAT::Test() */

void LteRrcMeasDedReselPriorityT320ExpiryBeforeClearFromIRAT::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasDedReselPriorityT320ExpiryBeforeClearFromIRAT::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasDedReselPriorityClearFromIRAT);

void LteRrcMeasDedReselPriorityClearFromIRAT::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_RRC_GET_DEDICATED_PRI_RSP,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasDedReselPriorityClearFromIRAT::Setup() */

void LteRrcMeasDedReselPriorityClearFromIRAT::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_ded_priority_list_indi_s priority_list_ind;
  lte_rrc_stopped_indi_s        lte_stopped_ind    ;
  lte_rrc_clear_dedicated_pri_ind_s clear_ded_pri_ind;
  const lte_irat_dedicated_priority_list_s *freq_list_ptr = NULL;
  uint64                        t_320_value        = 0;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&priority_list_ind, 0, sizeof(lte_rrc_ded_priority_list_indi_s));
  memset(&lte_stopped_ind, 0, sizeof(lte_rrc_stopped_indi_s));
  memset(&clear_ded_pri_ind, 0, sizeof(lte_rrc_clear_dedicated_pri_ind_s));

  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_init_default_hdr(&priority_list_ind.msg_hdr, LTE_RRC_DED_PRIORITY_LIST_INDI);

  priority_list_ind.t_320_is_present = TRUE;
  priority_list_ind.t_320_ms = 5;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
  priority_list_ind.freq_list.num_hrdp_band_class = 0;
  priority_list_ind.freq_list.num_1x_band_class = 0;
  priority_list_ind.freq_list.num_eutra_freq = 2;
  // Serving freq info
  priority_list_ind.freq_list.eutra_freq_list[0].arfcn = 0x100;
  priority_list_ind.freq_list.eutra_freq_list[0].priority = 4;
  // Freq in Sib 5
  priority_list_ind.freq_list.eutra_freq_list[1].arfcn = 5180;
  priority_list_ind.freq_list.eutra_freq_list[1].priority = 5;

  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));

  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  t_320_value = lte_rrc_meas_get_dedicated_priority_info(&freq_list_ptr);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[0].arfcn == 0x100);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[0].priority == 4);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[1].arfcn == 5180);
  TF_ASSERT(freq_list_ptr->eutra_freq_list[1].priority == 5);
  TF_MSG("Current T320 value in ms is %d", t_320_value);

  // Send LTE_RRC_STOPPED_INDI 
  lte_rrc_init_default_hdr(&lte_stopped_ind.msg_hdr, LTE_RRC_STOPPED_INDI);
  lte_rrc_utf_send_msg((byte*)&lte_stopped_ind, sizeof(lte_stopped_ind));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Make sure T320 is still running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is still LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

  // mimic action of the other RAT
  lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_WCDMA);

  // Send LTE_RRC_CLEAR_DEDICATED_PRI_IND to Meas
  lte_rrc_init_default_hdr(&clear_ded_pri_ind.msg_hdr, LTE_RRC_CLEAR_DEDICATED_PRI_IND);
  lte_rrc_utf_send_msg((byte*)&clear_ded_pri_ind, sizeof(clear_ded_pri_ind));
  lte_rrc_utf_wait_for_done();
  
  // T320 should be stopped
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_STOPPED);

  // Make sure owner of dedicated priorities is NONE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_WCDMA);

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

}/* LteRrcMeasDedReselPriorityClearFromIRAT::Test() */

void LteRrcMeasDedReselPriorityClearFromIRAT::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasDedReselPriorityClearFromIRAT::Teardown() */


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasT320ExpireBeforeSIBCollectionCompletes);

void LteRrcMeasT320ExpireBeforeSIBCollectionCompletes::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasT320ExpireBeforeSIBCollectionCompletes::Setup() */

void LteRrcMeasT320ExpireBeforeSIBCollectionCompletes::Test()
{
  lte_rrc_camped_indi_s         camped_indi;
  lte_rrc_sib_updated_indi_s    sib_updated_indi;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf;
  lte_rrc_ded_priority_list_indi_s priority_list_ind;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  lte_rrc_tmri_s *t320_tmri_ptr                    = NULL;
  lte_rrc_ml1_revoke_reqi_s     *revoke_req        = NULL;
  lte_rrc_conn_released_indi_s  conn_rel_indi;
  lte_rrc_connected_indi_s      connected_indi;
  lte_rrc_not_camped_indi_s     not_camped_indi;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  memset(&priority_list_ind, 0, sizeof(lte_rrc_ded_priority_list_indi_s));


  // Initialize the proto stubs
  lte_rrc_meas_test_init();

  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_init_default_hdr(&priority_list_ind.msg_hdr, LTE_RRC_DED_PRIORITY_LIST_INDI);


  priority_list_ind.t_320_is_present = TRUE;
  priority_list_ind.t_320_ms = 5;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
  priority_list_ind.freq_list.num_hrdp_band_class = 0;
  priority_list_ind.freq_list.num_1x_band_class = 0;
  priority_list_ind.freq_list.num_eutra_freq = 2;
  // Serving freq info
  priority_list_ind.freq_list.eutra_freq_list[0].arfcn = 0x100;
  priority_list_ind.freq_list.eutra_freq_list[0].priority = 4;
  // Freq in Sib 5
  priority_list_ind.freq_list.eutra_freq_list[1].arfcn = 5180;
  priority_list_ind.freq_list.eutra_freq_list[1].priority = 5;

  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));
  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // -----------------------------------------------------------------------
  // Test for case where T320 expires before all sibs are received.  In this case
  // T320 expires before Inter-freq sib is received.

  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);


  // Send SIB3, SIB4 updated indication
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
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS ));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify cell resel priority for serving is according to the dedicated priority
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 4);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 5);


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

  // Boolean for replicating scenario where SIB5 is scheduled but not yet received by UE.  The expiry 
  // has happened before all SIBs are collected on current cell.
  lte_rrc_meas_test_sib5_sched_and_rcvd = FALSE;

  //Let T320 expire.  Expect idle meas cfg req with SIB parameters.
  TF_SLEEP(5);
  TF_MSG("T320 timer expires");

  // Catch the T320 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t320_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);
  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_STOPPED);

  // Make sure owner of dedicated priorities is NONE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_NONE);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS ));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify cell resel priority for serving is according to sib info
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);


  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Boolean for replicating scenario where SIB5 is scheduled but not yet received by UE.
  lte_rrc_meas_test_sib5_sched_and_rcvd = TRUE;


  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);


  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasT320ExpireBeforeSIBCollectionCompletes::Test() */

void LteRrcMeasT320ExpireBeforeSIBCollectionCompletes::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasT320ExpireBeforeSIBCollectionCompletes */



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasT320ExpireDuringCellReselection);

void LteRrcMeasT320ExpireDuringCellReselection::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasT320ExpireDuringCellReselection::Setup() */

void LteRrcMeasT320ExpireDuringCellReselection::Test()
{
  lte_rrc_camped_indi_s         camped_indi;
  lte_rrc_sib_updated_indi_s    sib_updated_indi;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf;
  lte_rrc_ded_priority_list_indi_s priority_list_ind;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  lte_rrc_tmri_s *t320_tmri_ptr                    = NULL;
  lte_rrc_ml1_revoke_reqi_s     *revoke_req        = NULL;
  lte_rrc_conn_released_indi_s  conn_rel_indi;
  lte_rrc_connected_indi_s      connected_indi;
  lte_rrc_not_camped_indi_s     not_camped_indi;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  memset(&priority_list_ind, 0, sizeof(lte_rrc_ded_priority_list_indi_s));



  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Initiate ded cell resel priority indication
  lte_rrc_init_default_hdr(&priority_list_ind.msg_hdr, LTE_RRC_DED_PRIORITY_LIST_INDI);


  priority_list_ind.t_320_is_present = TRUE;
  priority_list_ind.t_320_ms = 5;
  priority_list_ind.source_proc = LTE_RRC_PROC_CRP;
  priority_list_ind.freq_list.num_hrdp_band_class = 0;
  priority_list_ind.freq_list.num_1x_band_class = 0;
  priority_list_ind.freq_list.num_eutra_freq = 2;
  // Serving freq info
  priority_list_ind.freq_list.eutra_freq_list[0].arfcn = 0x100;
  priority_list_ind.freq_list.eutra_freq_list[0].priority = 4;
  // Freq in Sib 5
  priority_list_ind.freq_list.eutra_freq_list[1].arfcn = 5180;
  priority_list_ind.freq_list.eutra_freq_list[1].priority = 5;

  lte_rrc_utf_send_msg((byte*)&priority_list_ind, sizeof(priority_list_ind));
  // Wait for Meas SM to process the ded priority list indi
  lte_rrc_utf_wait_for_done();

  // Make sure T320 is running
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_RUNNING);

  // Make sure owner of dedicated priorities is LTE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

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

  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  //--------------------------------------------------------------------
  // Test case where T320 expires in the middle of reselection


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

  // Verify cell resel priority for serving is according to the dedicated priority
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 4);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 5);

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

  // Change the serving cell
  lte_rrc_meas_test_cell_id.freq = 0x200;
  lte_rrc_meas_test_cell_id.phy_cell_id = 0x20;

  //Let T320 expire.  Expect idle meas cfg req with SIB parameters.
  TF_SLEEP(5);
  TF_MSG("T320 timer expires");

  // Catch the T320 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t320_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);

  // We expect meas sm to ignore the T320 expiry since UE has moved new cell

  // Make sure T320 is stopped
  TF_ASSERT(lte_rrc_meas_data.sd_ptr->t320_running == LTE_RRC_MEAS_T320_STOPPED);

  // Make sure owner of dedicated priorities is NONE
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_NONE);



  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);


  // send SIB update ind on new cell.
  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
	                                      LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x20;
  sib_updated_indi.cell_id.freq = 0x200;


  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);


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

  // Verify cell resel priority for serving is according to sib info
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);


  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);


  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasT320ExpireDuringCellReselection::Test() */

void LteRrcMeasT320ExpireDuringCellReselection::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasT320ExpireDuringCellReselection::Teardown() */
#endif
