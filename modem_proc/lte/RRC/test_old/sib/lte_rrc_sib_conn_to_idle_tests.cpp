/*!
  @file lte_rrc_sib_conn_to_idle_tests.cpp

  @brief
  Connected to idle test cases
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_conn_to_idle_tests.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/10   amit    Initial Version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "__lte_rrc_sib.h"
#include "__lte_rrc_controller.h"
#include "lte_rrc_sibi.h"
#include "lte_mac_ind.h"
#include "lte_mac_msg.h"
#include "dsm_item.h"
#include "dsm_init.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_sib_test_common.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/




/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

DECLARE_SIB1_2345_MCC_8_5;
DECLARE_SIB1_2345_MCC_8_5_pad_bytes;
DECLARE_SIB1_2345_MCC_8_5_VALUE_TAG_CHANGE_PAD_BYTES;
DECLARE_SIB1_2345_MCC_8_5_VALUE_TAG_CHANGE;
DECLARE_SIB1_2345_MCC_8_5_CELL2;
DECLARE_SIB2_SIB3_8_5;
DECLARE_SIB4_SIB5_8_5;
DECLARE_SIB1_234513_MCC_8_5;
DECLARE_SIB1_WITH_SIB13_8_5;
DECLARE_SIB2_8_5;
DECLARE_SIB13_8_5;

extern "C"
{
extern lte_rrc_sib_s lte_rrc_sib;
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern boolean lte_rrc_sib_test_camped;
extern stm_state_t lte_rrc_sib_test_controller_state;
extern boolean lte_rrc_sib_test_wcdma_supported;
extern boolean lte_rrc_sib_test_gsm_supported;
extern boolean lte_rrc_sib_test_cdma_1x_supported;
extern boolean lte_rrc_sib_test_cdma_do_supported;
extern boolean lte_rrc_sib_test_ho_in_progress;
extern boolean lte_rrc_sib_test_embms_is_enabled;
extern lte_rrc_rf_status_e lte_rrc_sib_test_rf_status;
extern uint64 lte_rrc_sib_test_rf_status_time;
extern lte_l1_sfn_t lte_rrc_sib_test_rf_status_sfn;
extern uint64 lte_rrc_sib_test_wt_tmr_before_rf_unavailable;
extern uint64 lte_rrc_sib_test_wt_tmr_after_rf_available;

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/* Read SIBs for conn to idle when all SIBs are already present in db and event 
   not already active for cell. Event should be created, and get_sibs_cnf sent 
   right away. On receiving camped_indi, event should be cleared but no 
   sib_sched_req sent */
void lte_rrc_sib_test_conn_to_idle()
{
  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  //---------------------------------------------------------------

  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL, TRUE, 
                                LTE_RRC_NULL_SFN);

  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  uint32 buf_len;

  // ---------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // ---------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);

  // ---------------------------------------------

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;
  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // ---------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK));

  // ---------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);

  //---------------------------------------------------------------

  lte_rrc_utf_wait_for_done();
  lte_rrc_utf_check_no_more_msgs();

  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);

}

void lte_rrc_sib_test_conn_to_idle2()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  uint32 buf_len;

  // ---------------------------------------------

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;
  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // ---------------------------------------------

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  //---------------------------------------------------------------
/*
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB4_BITMASK);

  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);

  //---------------------------------------------------------------
*/
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL, TRUE, 
                                LTE_RRC_NULL_SFN);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  lte_rrc_sib_test_camped = TRUE;

  // ---------------------------------------------
/*
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                     LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));


  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  // --------------------------------------------------------------------
*/
  // Send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // --------------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));


  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  // --------------------------------------------------------------------

  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // --------------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK));


  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  // --------------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            64);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB1_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  // ----------------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

}

void lte_rrc_sib_test_conn_to_idle3()
{
  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  //---------------------------------------------------------------

  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL, TRUE, 
                                LTE_RRC_NULL_SFN);

  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_cphy_sib_sched_req_s *sib_sched_req;

  uint32 buf_len;

  // ---------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE, FALSE, 
                                1);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK |
                                                     LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  // ---------------------------------------------

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;
  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // ---------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK));

  // ---------------------------------------------

  //---------------------------------------------------------------

  lte_rrc_utf_wait_for_done();
  lte_rrc_utf_check_no_more_msgs();

  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);
}

typedef enum
{
  LTE_RRC_SIB_TEST_CONN_TO_IDLE5_TEST_SIBS_CONSISTENT_VALUE_TAGS_MATCH,
  LTE_RRC_SIB_TEST_CONN_TO_IDLE5_TEST_SIBS_CONSISTENT_VALUE_TAGS_DO_NOT_MATCH
} lte_rrc_sib_test_conn_to_idle5_test_e;

void lte_rrc_sib_test_conn_to_idle5(lte_rrc_sib_test_conn_to_idle5_test_e test_type)
{
  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  uint32 buf_len;

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;
  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // ---------------------------------------------

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  //---------------------------------------------------------------
/*
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == LTE_RRC_SIB4_BITMASK);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);


  //---------------------------------------------------------------
*/
  lte_rrc_sib_test_send_ho_started_indi();
  lte_rrc_sib_test_ho_in_progress = FALSE;

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE, TRUE, 
                                LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // ---------------------------------------------

  lte_rrc_sib_test_send_ho_started_indi();
  lte_rrc_sib_test_ho_in_progress = FALSE;

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE, FALSE, 
                                LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL, TRUE, 
                                1);

  // ---------------------------------------------

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  
  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            1);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB1_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);

  // ----------------------------------------------------------

  // send SIB1 to RRC
  switch(test_type)
  {
    case LTE_RRC_SIB_TEST_CONN_TO_IDLE5_TEST_SIBS_CONSISTENT_VALUE_TAGS_MATCH:
      /*lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                            sizeof(sib1_2345_mcc_8_5),
                                            PHY_CELL_ID_1,
                                            FREQ_1,
                                            2); */
      lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_pad_bytes,
                                            sizeof(sib1_2345_mcc_8_5_pad_bytes),
                                            PHY_CELL_ID_1,
                                            FREQ_1,
                                            2);
      break;

    case LTE_RRC_SIB_TEST_CONN_TO_IDLE5_TEST_SIBS_CONSISTENT_VALUE_TAGS_DO_NOT_MATCH:
      /* lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_value_tag_change,
                                            sizeof(sib1_2345_mcc_8_5_value_tag_change),
                                            PHY_CELL_ID_1,
                                            FREQ_1,
                                            2);*/
      lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_value_tag_change_pad_bytes,
                                            sizeof(sib1_2345_mcc_8_5_value_tag_change_pad_bytes),
                                            PHY_CELL_ID_1,
                                            FREQ_1,
                                            2);
      break;

    default:
      TF_ASSERT(0);
  }
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  switch(test_type)
  {
    case LTE_RRC_SIB_TEST_CONN_TO_IDLE5_TEST_SIBS_CONSISTENT_VALUE_TAGS_MATCH:
      TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));

      lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
      TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
      TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
      TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
      TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
      TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

      break;

    case LTE_RRC_SIB_TEST_CONN_TO_IDLE5_TEST_SIBS_CONSISTENT_VALUE_TAGS_DO_NOT_MATCH:
      TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
      break;

    default:
      TF_ASSERT(0);
  }

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK |
                                                     LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  lte_rrc_utf_check_no_more_msgs();
}

/* Sib reading active but mandatory sibs not read then connected to idle on 
   same cell */
void lte_rrc_sib_test_conn_to_idle7()
{
  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_ho_started_indi();
  lte_rrc_sib_test_ho_in_progress = FALSE;

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE, TRUE, 
                                      LTE_RRC_NULL_SFN);

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  uint32 buf_len;

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

  //---------------------------------------------------------------

  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  //---------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            1);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_cell2,
                                        sizeof(sib1_2345_mcc_8_5_cell2),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK | 
                                                     LTE_RRC_SIB3_BITMASK | 
                                                     LTE_RRC_SIB4_BITMASK | 
                                                     LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        3);

  //---------------------------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_2);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 1);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 1;

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK | 
                                                     LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK | 
                                                     LTE_RRC_SIB3_BITMASK | 
                                                     LTE_RRC_SIB4_BITMASK | 
                                                     LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);

  //---------------------------------------------------------------

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_2, FREQ_2);

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        4);

  //---------------------------------------------------------------

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr = 
    lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_2, FREQ_2,
                                       0,
                                       64,
                                       (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK),
                                       4,
                                       LTE_CPHY_SI_WINDOW_LENGTH_MS_2,
                                       2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            64);

  //---------------------------------------------------------------

  sib_sched_req_ptr = 
    lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_2, FREQ_2,
                                       LTE_RRC_SIB1_BITMASK,
                                       128,
                                       (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK),
                                       64,
                                       LTE_CPHY_SI_WINDOW_LENGTH_MS_2,
                                       2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_cell2,
                                        sizeof(sib1_2345_mcc_8_5_cell2),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        65);

  //---------------------------------------------------------------

  sib_sched_req_ptr = 
    lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_2, FREQ_2,
                                       0,
                                       128,
                                       0,
                                       65,
                                       LTE_CPHY_SI_WINDOW_LENGTH_MS_2,
                                       2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_valid == TRUE);
  for(int i = 2; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_rank == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_counter == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[0] == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[1] == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[2] == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.num_mnc_digits == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[0] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[1] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[2] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.global_cell_id == 0x400000);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                               LTE_RRC_SIB3_BITMASK |
                                                               LTE_RRC_SIB4_BITMASK |
                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_sib3_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[1] == (LTE_RRC_SIB4_BITMASK |
                                                                        LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_size == sizeof(sib4_sib5_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1] == NULL);
  for(int i = 2; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib3_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib4_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib5_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
 // TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_counter == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.plmn.mcc[0] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.plmn.mcc[1] == 5);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.plmn.mcc[2] == 6);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.plmn.num_mnc_digits == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.plmn.mnc[0] == 6);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.plmn.mnc[1] == 7);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.plmn.mnc[2] == 8);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.global_cell_id == 0x1100000);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.cell_id.freq == FREQ_2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].value_tag == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5_cell2));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib1_ptr != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                               LTE_RRC_SIB3_BITMASK |
                                                               LTE_RRC_SIB4_BITMASK |
                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_sib3_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.sibs_in_si_bitmask[1] == (LTE_RRC_SIB4_BITMASK |
                                                                        LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.encoded_si_info[1].encoded_si_size == sizeof(sib4_sib5_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.encoded_si_info[1].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[1] != NULL);
  for(int i = 2; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib2_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib3_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib4_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib5_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib5);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib6_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib11_ptr == NULL);
}

/* Get sibs req rcvd for conn to idle after some failure in connected mode such 
   that the new cell and SIB's serving cell do not match - the case should be 
   treated as cell selection and all SIBs should be read for the new cell */
void lte_rrc_sib_test_conn_to_idle_serv_cell_diff()
{
  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_ho_started_indi();
  lte_rrc_sib_test_ho_in_progress = FALSE;

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

  // ---------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            1);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // ---------------------------------------------

  TF_SLEEP(30000);

  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);

  // ---------------------------------------------

  lte_rrc_sib_read_error_indi_s *read_error_ptr;

  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ptr, &buf_len);

  TF_ASSERT(read_error_ptr->msg_hdr.id == LTE_RRC_SIB_READ_ERROR_INDI);
  TF_ASSERT(read_error_ptr->cell_id.freq == FREQ_2);
  TF_ASSERT(read_error_ptr->cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(read_error_ptr->sibs_rcvd_curr_bitmask == LTE_RRC_MIB_BITMASK);
  TF_ASSERT(read_error_ptr->csg_indication == FALSE);
  TF_ASSERT(read_error_ptr->intra_freq_resel_allowed == TRUE);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // ---------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  for(int i = 1; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_counter == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[0] == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[1] == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[2] == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.num_mnc_digits == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[0] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[1] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[2] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.global_cell_id == 0x400000);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                               LTE_RRC_SIB3_BITMASK |
                                                               LTE_RRC_SIB4_BITMASK |
                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_sib3_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[1] == (LTE_RRC_SIB4_BITMASK |
                                                                        LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_size == sizeof(sib4_sib5_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1] != NULL);
  for(int i = 2; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib3_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib4_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib5_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib5);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
 // TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  /* connected to idle when event is not active for the cell, and it is not the 
     serving cell yet - should work like cell selection. This would be the case 
     where PCI & freq of the requested cell is not the same as that of the 
     serving cell for SIB */
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

  // ---------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            1);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // ---------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_cell2,
                                        sizeof(sib1_2345_mcc_8_5_cell2),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        2);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ---------------------------------------------

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        3);

  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_get_sibs_cnfi(PHY_CELL_ID_2,
                                     FREQ_2,
                                     1,
                                     LTE_RRC_GET_SIBS_SUCCESS);

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 1;
  
  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ---------------------------------------------

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        4);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ---------------------------------------------

  // Prepare camped_indi to send to SIB
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_2,
                                    FREQ_2);

  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_2,
                                        FREQ_2,
                                        (LTE_RRC_SIB3_BITMASK |
                                         LTE_RRC_SIB4_BITMASK | 
                                         LTE_RRC_SIB5_BITMASK),
                                        (LTE_RRC_MIB_BITMASK |
                                         LTE_RRC_SIB1_BITMASK |
                                         LTE_RRC_SIB2_BITMASK |
                                         LTE_RRC_SIB3_BITMASK |
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));


}

/* Get sibs req rcvd for conn to idle after some failure in connected mode such 
   that the serving cell SIBs are inconsistent - the case should be 
   treated as cell selection and all SIBs should be read for the new cell */
void lte_rrc_sib_test_conn_to_idle_serv_cell_sibs_inconsistent()
{
  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_ho_started_indi();
  lte_rrc_sib_test_ho_in_progress = FALSE;

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE, TRUE, 
                                      LTE_RRC_NULL_SFN);

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  
  uint32 buf_len;

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            1);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_cell2,
                                        sizeof(sib1_2345_mcc_8_5_cell2),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        2);
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK | 
  	                                                 LTE_RRC_SIB3_BITMASK |
  	                                                 LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        3);

  //---------------------------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_2);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 1);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 1;

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK | 
  	                                                 LTE_RRC_SIB3_BITMASK |
  	                                                 LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 1);
  
  //---------------------------------------------------------------

  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK|
  	                                                 LTE_RRC_SIB2_BITMASK|
  	                                                 LTE_RRC_SIB3_BITMASK|
  	                                                 LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK|
                                                     LTE_RRC_SIB2_BITMASK|
  	                                                 LTE_RRC_SIB3_BITMASK|
  	                                                 LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  TF_SLEEP(35000);

  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);

  // ---------------------------------------------

  lte_rrc_sib_read_error_indi_s *read_error_ptr;

  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ptr, &buf_len);

  TF_ASSERT(read_error_ptr->msg_hdr.id == LTE_RRC_SIB_READ_ERROR_INDI);
  TF_ASSERT(read_error_ptr->cell_id.freq == FREQ_2);
  TF_ASSERT(read_error_ptr->cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(read_error_ptr->sibs_rcvd_curr_bitmask == LTE_RRC_MIB_BITMASK);
  TF_ASSERT(read_error_ptr->csg_indication == TRUE);
  TF_ASSERT(read_error_ptr->intra_freq_resel_allowed == TRUE);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);

  // ---------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_valid == TRUE);
  for(int i = 2; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == FALSE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  
  // ----------------------------------------------------------------------

  /* Asserts on valid cells */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_rank == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_counter == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[0] == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[1] == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[2] == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.num_mnc_digits == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[0] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[1] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[2] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.global_cell_id == 0x400000);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                               LTE_RRC_SIB3_BITMASK |
                                                               LTE_RRC_SIB4_BITMASK |
                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_sib3_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[1] == (LTE_RRC_SIB4_BITMASK |
                                                                        LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_size == sizeof(sib4_sib5_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1] == NULL);
  for(int i = 2; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib3_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib4_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib5_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
 // TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_counter == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.plmn.mcc[0] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.plmn.mcc[1] == 5);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.plmn.mcc[2] == 6);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.plmn.num_mnc_digits == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.plmn.mnc[0] == 6);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.plmn.mnc[1] == 7);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.plmn.mnc[2] == 8);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.global_cell_id == 0x1100000);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib_cell_id.cell_id.freq == FREQ_2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].value_tag == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5_cell2));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].sib1_ptr != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                               LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_sib3_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[0] != NULL);
  for(int i = 1; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib2_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib3_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib4_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib5_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib6_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_decoded_sibs.sib11_ptr == NULL);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  /* connected to idle when event serv cell sibs are not consistent - should 
     work like cell selection. */
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

  // ---------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            1);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_2);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // ---------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_cell2,
                                        sizeof(sib1_2345_mcc_8_5_cell2),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        2);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ---------------------------------------------

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        3);

  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_get_sibs_cnfi(PHY_CELL_ID_2,
                                     FREQ_2,
                                     1,
                                     LTE_RRC_GET_SIBS_SUCCESS);

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 1;
  
  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ---------------------------------------------

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        4);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ---------------------------------------------

  // Prepare camped_indi to send to SIB
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_2,
                                    FREQ_2);

  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_2,
                                        FREQ_2,
                                        (LTE_RRC_SIB3_BITMASK |
                                         LTE_RRC_SIB4_BITMASK | 
                                         LTE_RRC_SIB5_BITMASK),
                                        (LTE_RRC_MIB_BITMASK |
                                         LTE_RRC_SIB1_BITMASK |
                                         LTE_RRC_SIB2_BITMASK |
                                         LTE_RRC_SIB3_BITMASK |
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

}

/* Go from conn to idle when SIB reading is not active but not all SIBs are 
   present, and before receiving any SIBs go back to connected - sched req was sent 
   to read SIBs that were not present so another one should be sent to stop SIB 
   reading */
void lte_rrc_sib_test_conn_to_idle_to_conn()
{
  lte_cphy_sib_sched_req_s *sib_sched_req;

  uint32 buf_len;

  // ---------------------------------------------

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;
  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // ---------------------------------------------

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  //---------------------------------------------------------------
/*
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == LTE_RRC_SIB4_BITMASK);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
*/
  //---------------------------------------------------------------

  /* Since in connected now, updates not needed (event converted to 
     next_mod_prd_value_tag_check */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == LTE_RRC_SIB5_BITMASK);

  /* Next mod prd value tag check */
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            64);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL, TRUE, 
                                LTE_RRC_NULL_SFN);

  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  // ---------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // ---------------------------------------------
/*
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                     LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK |
                                                     LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  // ---------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);

  // ---------------------------------------------
*/

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;
  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // ---------------------------------------------

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  //---------------------------------------------------------------
/*
  // Since sib reading was started, it should be stopped since no sibs needed now
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == LTE_RRC_SIB4_BITMASK);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);
*/
}


/*===========================================================================

                                UNIT TEST
Conn to idle on same cell (with all SIBs already in the db, sib reading 
already active)

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibConnToIdleTest1);

void LteRrcSibConnToIdleTest1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibConnToIdleTest1::Test()
{
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_get_sibs_reqi_s get_sibs_req;
  
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;
  // ----------------------------------------------------------------------

  lte_cphy_mib_ind_s mib_ind;
  // ----------------------------------------------------------------------

  dsm_item_type *dsm_ptr=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr;           /* Message attachment pointer */
  lte_mac_rrc_dl_data_ind_s dl_data_ind;
  
  dsm_item_type *dsm_ptr_sib1=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr_sib1;           /* Message attachment pointer */
  lte_mac_rrc_dl_data_ind_s dl_data_ind_sib1;
  
  lte_mac_rrc_dl_data_ind_s dl_data_ind_sib2_sib3;
  dsm_item_type *dsm_ptr_sib2_sib3=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr_sib2_sib3;           /* Message attachment pointer */
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  // ----------------------------------------------------------------------

  lte_mac_rrc_dl_data_ind_s dl_data_ind_sib4_sib5;
  dsm_item_type *dsm_ptr_sib4_sib5=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr_sib4_sib5;           /* Message attachment pointer */
  // ----------------------------------------------------------------------

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* get sibs req */
  lte_rrc_init_default_hdr(&get_sibs_req.msg_hdr, LTE_RRC_GET_SIBS_REQI);
  get_sibs_req.cause = LTE_RRC_GET_SIBS_CAUSE_CELL_SEL;
  get_sibs_req.cell_id.freq = 2000;
  get_sibs_req.cell_id.phy_cell_id = 42;
  get_sibs_req.cnf_needed = TRUE;
  get_sibs_req.req_proc = LTE_RRC_PROC_CSP;

  lte_rrc_utf_send_msg((byte*)&get_sibs_req, sizeof(get_sibs_req));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  // ----------------------------------------------------------------------

  /* mib_ind */
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);
  mib_ind.freq = 2000;
  mib_ind.phy_cell_id = 42;
  mib_ind.sfn = 1;
  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_25;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_EXTENDED;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == mib_ind.mib.dl_bandwidth);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == mib_ind.mib.phich_info.phich_duration);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == mib_ind.mib.phich_info.phich_resource);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB1_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);

  // ----------------------------------------------------------------------

  /* SIB1 */
  msgr_init_hdr_attach(&dl_data_ind.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_BCCH_DL_DATA_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr, sib1_2345_mcc_8_5, sizeof(sib1_2345_mcc_8_5), DSM_DS_SMALL_ITEM_POOL) == sizeof(sib1_2345_mcc_8_5));

  TF_ASSERT(dsm_ptr != NULL);

  att_ptr = msgr_get_attach(&dl_data_ind.hdr, 0);
  msgr_set_dsm_attach(att_ptr, dsm_ptr);

  dl_data_ind.freq = 2000;
  dl_data_ind.phy_cell_id = 42;
  dl_data_ind.sfn.sys_fn = 3;


  lte_rrc_utf_send_msg((byte*)&dl_data_ind, sizeof(dl_data_ind));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  // ----------------------------------------------------------------------

  /* SIB2_SIB3 */
  msgr_init_hdr_attach(&dl_data_ind_sib2_sib3.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_BCCH_DL_DATA_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr_sib2_sib3, sib2_sib3_8_5, sizeof(sib2_sib3_8_5), DSM_DS_SMALL_ITEM_POOL) == sizeof(sib2_sib3_8_5));

  TF_ASSERT(dsm_ptr_sib2_sib3 != NULL);

  att_ptr_sib2_sib3 = msgr_get_attach(&dl_data_ind_sib2_sib3.hdr, 0);
  msgr_set_dsm_attach(att_ptr_sib2_sib3, dsm_ptr_sib2_sib3);

  dl_data_ind_sib2_sib3.freq = 2000;
  dl_data_ind_sib2_sib3.phy_cell_id = 42;
  dl_data_ind_sib2_sib3.sfn.sys_fn = 64;


  lte_rrc_utf_send_msg((byte*)&dl_data_ind_sib2_sib3, sizeof(dl_data_ind_sib2_sib3));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  
  // ----------------------------------------------------------------------

  /* SIB4_SIB5 */
  msgr_init_hdr_attach(&dl_data_ind_sib4_sib5.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_BCCH_DL_DATA_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr_sib4_sib5, sib4_sib5_8_5, sizeof(sib4_sib5_8_5), DSM_DS_SMALL_ITEM_POOL) == sizeof(sib4_sib5_8_5));

  TF_ASSERT(dsm_ptr_sib4_sib5 != NULL);

  att_ptr_sib4_sib5 = msgr_get_attach(&dl_data_ind_sib4_sib5.hdr, 0);
  msgr_set_dsm_attach(att_ptr_sib4_sib5, dsm_ptr_sib4_sib5);

  dl_data_ind_sib4_sib5.freq = 2000;
  dl_data_ind_sib4_sib5.phy_cell_id = 42;
  dl_data_ind_sib4_sib5.sfn.sys_fn = 65;


  lte_rrc_utf_send_msg((byte*)&dl_data_ind_sib4_sib5, sizeof(dl_data_ind_sib4_sib5));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == 0);

  // ----------------------------------------------------------------------

  /* MIB */
  mib_ind.sfn = 66;
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB1_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  /* SIB1 */
  msgr_init_hdr_attach(&dl_data_ind_sib1.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_BCCH_DL_DATA_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr_sib1, sib1_2345_mcc_8_5, sizeof(sib1_2345_mcc_8_5), DSM_DS_SMALL_ITEM_POOL) == sizeof(sib1_2345_mcc_8_5));

  TF_ASSERT(dsm_ptr_sib1 != NULL);

  att_ptr_sib1 = msgr_get_attach(&dl_data_ind_sib1.hdr, 0);
  msgr_set_dsm_attach(att_ptr_sib1, dsm_ptr_sib1);

  dl_data_ind_sib1.freq = 2000;
  dl_data_ind_sib1.phy_cell_id = 42;
  dl_data_ind_sib1.sfn.sys_fn = 67;


  lte_rrc_utf_send_msg((byte*)&dl_data_ind_sib1, sizeof(dl_data_ind_sib1));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  lte_rrc_sib_event_s *event_ptr = &lte_rrc_sib.dd_ptr->events_db.event[0];
  TF_ASSERT(event_ptr->all_si.sibs_bitmask == 0);
  TF_ASSERT(event_ptr->cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == 2000);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == 42);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(event_ptr->sib_db_index == 0);
  
  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camped = TRUE;
 lte_rrc_sib_test_sib_db_key = 0;
  //camped_ind
  lte_rrc_camped_indi_s camped_ind;

  lte_rrc_init_default_hdr(&camped_ind.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_ind.camped_cell.freq = 2000;
  camped_ind.camped_cell.phy_cell_id = 42;

  lte_rrc_utf_send_msg((byte*)&camped_ind, sizeof(camped_ind));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);

  //sib_updated_ind
  lte_rrc_sib_test_rcv_sib_updated_indi(42,
                                        2000,
                                        (LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK));

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_connected_indi();

  /* get sibs req - conn to idle*/
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  get_sibs_req.cause = LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL;
  get_sibs_req.cell_id.freq = 2000;
  get_sibs_req.cell_id.phy_cell_id = 42;
  get_sibs_req.cnf_needed = TRUE;
  get_sibs_req.req_proc = LTE_RRC_PROC_CSP;

  lte_rrc_utf_send_msg((byte*)&get_sibs_req, sizeof(get_sibs_req));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == 2000);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == 42);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}


void LteRrcSibConnToIdleTest1::Teardown()
{
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibConnToIdleTest1WithDSDS);

void LteRrcSibConnToIdleTest1WithDSDS::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibConnToIdleTest1WithDSDS::Test()
{
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_get_sibs_reqi_s get_sibs_req;
  
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;
  // ----------------------------------------------------------------------

  lte_cphy_mib_ind_s mib_ind;
  // ----------------------------------------------------------------------

  dsm_item_type *dsm_ptr=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr;           /* Message attachment pointer */
  lte_mac_rrc_dl_data_ind_s dl_data_ind;
  
  dsm_item_type *dsm_ptr_sib1=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr_sib1;           /* Message attachment pointer */
  lte_mac_rrc_dl_data_ind_s dl_data_ind_sib1;
  
  lte_mac_rrc_dl_data_ind_s dl_data_ind_sib2_sib3;
  dsm_item_type *dsm_ptr_sib2_sib3=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr_sib2_sib3;           /* Message attachment pointer */
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  // ----------------------------------------------------------------------

  lte_mac_rrc_dl_data_ind_s dl_data_ind_sib4_sib5;
  dsm_item_type *dsm_ptr_sib4_sib5=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr_sib4_sib5;           /* Message attachment pointer */
  // ----------------------------------------------------------------------

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* get sibs req */


  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
TF_MSG("Sending RF_UNAVAILABLE_IND");
lte_rrc_sib_test_rf_status_sfn = 5;
lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
lte_rrc_sib_test_send_rf_unavail_ind();

lte_rrc_utf_wait_for_done();

  lte_rrc_init_default_hdr(&get_sibs_req.msg_hdr, LTE_RRC_GET_SIBS_REQI);
  get_sibs_req.cause = LTE_RRC_GET_SIBS_CAUSE_CELL_SEL;
  get_sibs_req.cell_id.freq = 2000;
  get_sibs_req.cell_id.phy_cell_id = 42;
  get_sibs_req.cnf_needed = TRUE;
  get_sibs_req.req_proc = LTE_RRC_PROC_CSP;

  lte_rrc_utf_send_msg((byte*)&get_sibs_req, sizeof(get_sibs_req));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_check_no_more_msgs();

TF_SLEEP(100);
TF_MSG("Sending RF_AVAILABLE_IND");
lte_rrc_sib_test_rf_status_sfn = 7;
lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
lte_rrc_sib_test_send_rf_avail_ind();
lte_rrc_utf_wait_for_done();


  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  // ----------------------------------------------------------------------

  /* mib_ind */
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);
  mib_ind.freq = 2000;
  mib_ind.phy_cell_id = 42;
  mib_ind.sfn = 1;
  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_25;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_EXTENDED;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == mib_ind.mib.dl_bandwidth);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == mib_ind.mib.phich_info.phich_duration);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == mib_ind.mib.phich_info.phich_resource);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB1_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);

  // ----------------------------------------------------------------------

  /* SIB1 */
  msgr_init_hdr_attach(&dl_data_ind.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_BCCH_DL_DATA_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr, sib1_2345_mcc_8_5, sizeof(sib1_2345_mcc_8_5), DSM_DS_SMALL_ITEM_POOL) == sizeof(sib1_2345_mcc_8_5));

  TF_ASSERT(dsm_ptr != NULL);

  att_ptr = msgr_get_attach(&dl_data_ind.hdr, 0);
  msgr_set_dsm_attach(att_ptr, dsm_ptr);

  dl_data_ind.freq = 2000;
  dl_data_ind.phy_cell_id = 42;
  dl_data_ind.sfn.sys_fn = 3;


  lte_rrc_utf_send_msg((byte*)&dl_data_ind, sizeof(dl_data_ind));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  // ----------------------------------------------------------------------

  /* SIB2_SIB3 */
  msgr_init_hdr_attach(&dl_data_ind_sib2_sib3.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_BCCH_DL_DATA_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr_sib2_sib3, sib2_sib3_8_5, sizeof(sib2_sib3_8_5), DSM_DS_SMALL_ITEM_POOL) == sizeof(sib2_sib3_8_5));

  TF_ASSERT(dsm_ptr_sib2_sib3 != NULL);

  att_ptr_sib2_sib3 = msgr_get_attach(&dl_data_ind_sib2_sib3.hdr, 0);
  msgr_set_dsm_attach(att_ptr_sib2_sib3, dsm_ptr_sib2_sib3);

  dl_data_ind_sib2_sib3.freq = 2000;
  dl_data_ind_sib2_sib3.phy_cell_id = 42;
  dl_data_ind_sib2_sib3.sfn.sys_fn = 64;


  lte_rrc_utf_send_msg((byte*)&dl_data_ind_sib2_sib3, sizeof(dl_data_ind_sib2_sib3));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  

  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
TF_MSG("Sending RF_UNAVAILABLE_IND");

lte_rrc_sib_test_rf_status_sfn = 111;
lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
lte_rrc_sib_test_send_rf_unavail_ind();

lte_rrc_utf_wait_for_done();

TF_SLEEP(100);
TF_MSG("Sending RF_AVAILABLE_IND");
lte_rrc_sib_test_rf_status_sfn = 115;
lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
lte_rrc_sib_test_send_rf_avail_ind();
lte_rrc_utf_wait_for_done();

lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);


TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available != lte_rrc_sib_test_wt_tmr_before_rf_unavailable);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  /* SIB4_SIB5 */
  msgr_init_hdr_attach(&dl_data_ind_sib4_sib5.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_BCCH_DL_DATA_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr_sib4_sib5, sib4_sib5_8_5, sizeof(sib4_sib5_8_5), DSM_DS_SMALL_ITEM_POOL) == sizeof(sib4_sib5_8_5));

  TF_ASSERT(dsm_ptr_sib4_sib5 != NULL);

  att_ptr_sib4_sib5 = msgr_get_attach(&dl_data_ind_sib4_sib5.hdr, 0);
  msgr_set_dsm_attach(att_ptr_sib4_sib5, dsm_ptr_sib4_sib5);

  dl_data_ind_sib4_sib5.freq = 2000;
  dl_data_ind_sib4_sib5.phy_cell_id = 42;
  dl_data_ind_sib4_sib5.sfn.sys_fn = 65;


  lte_rrc_utf_send_msg((byte*)&dl_data_ind_sib4_sib5, sizeof(dl_data_ind_sib4_sib5));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == 0);



  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
TF_MSG("Sending RF_UNAVAILABLE_IND");

lte_rrc_sib_test_rf_status_sfn = 112;
lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
lte_rrc_sib_test_send_rf_unavail_ind();

lte_rrc_utf_wait_for_done();

TF_SLEEP(100);
TF_MSG("Sending RF_AVAILABLE_IND");
lte_rrc_sib_test_rf_status_sfn = 115;
lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
lte_rrc_sib_test_send_rf_avail_ind();
lte_rrc_utf_wait_for_done();

lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);


TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available == 320);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  /* MIB */
  mib_ind.sfn = 66;
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB1_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
TF_MSG("Sending RF_UNAVAILABLE_IND");

lte_rrc_sib_test_rf_status_sfn = 119;
lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
lte_rrc_sib_test_send_rf_unavail_ind();

lte_rrc_utf_wait_for_done();

TF_SLEEP(100);
TF_MSG("Sending RF_AVAILABLE_IND");
lte_rrc_sib_test_rf_status_sfn = 120;
lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
lte_rrc_sib_test_send_rf_avail_ind();
lte_rrc_utf_wait_for_done();

lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);


TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available == lte_rrc_sib_test_wt_tmr_before_rf_unavailable);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB1_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));


  /* SIB1 */
  msgr_init_hdr_attach(&dl_data_ind_sib1.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_BCCH_DL_DATA_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr_sib1, sib1_2345_mcc_8_5, sizeof(sib1_2345_mcc_8_5), DSM_DS_SMALL_ITEM_POOL) == sizeof(sib1_2345_mcc_8_5));

  TF_ASSERT(dsm_ptr_sib1 != NULL);

  att_ptr_sib1 = msgr_get_attach(&dl_data_ind_sib1.hdr, 0);
  msgr_set_dsm_attach(att_ptr_sib1, dsm_ptr_sib1);

  dl_data_ind_sib1.freq = 2000;
  dl_data_ind_sib1.phy_cell_id = 42;
  dl_data_ind_sib1.sfn.sys_fn = 67;


  lte_rrc_utf_send_msg((byte*)&dl_data_ind_sib1, sizeof(dl_data_ind_sib1));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  lte_rrc_sib_event_s *event_ptr = &lte_rrc_sib.dd_ptr->events_db.event[0];
  TF_ASSERT(event_ptr->all_si.sibs_bitmask == 0);
  TF_ASSERT(event_ptr->cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
TF_MSG("Sending RF_UNAVAILABLE_IND");

lte_rrc_sib_test_rf_status_sfn = 103;
lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
lte_rrc_sib_test_send_rf_unavail_ind();

lte_rrc_utf_wait_for_done();

TF_SLEEP(100);

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == 2000);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == 42);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);


  TF_MSG("Sending RF_AVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 107;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
  lte_rrc_sib_test_send_rf_avail_ind();
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);


  TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available == lte_rrc_sib_test_wt_tmr_before_rf_unavailable);


  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(event_ptr->sib_db_index == 0);
  
  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camped = TRUE;
 lte_rrc_sib_test_sib_db_key = 0;
  //camped_ind
  lte_rrc_camped_indi_s camped_ind;

  lte_rrc_init_default_hdr(&camped_ind.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_ind.camped_cell.freq = 2000;
  camped_ind.camped_cell.phy_cell_id = 42;

  lte_rrc_utf_send_msg((byte*)&camped_ind, sizeof(camped_ind));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);

  //sib_updated_ind
  lte_rrc_sib_test_rcv_sib_updated_indi(42,
                                        2000,
                                        (LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK));

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  //****************************************************************************************************************
  //****************************************************************************************************************
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK |  
														 LTE_RRC_SIB2_BITMASK |                      
														 LTE_RRC_SIB3_BITMASK |                      
														 LTE_RRC_SIB4_BITMASK |
														 LTE_RRC_SIB5_BITMASK ));

  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_connected_indi();

  /* get sibs req - conn to idle*/
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  get_sibs_req.cause = LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL;
  get_sibs_req.cell_id.freq = 2000;
  get_sibs_req.cell_id.phy_cell_id = 42;
  get_sibs_req.cnf_needed = TRUE;
  get_sibs_req.req_proc = LTE_RRC_PROC_CSP;

  lte_rrc_utf_send_msg((byte*)&get_sibs_req, sizeof(get_sibs_req));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == 2000);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == 42);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
 // TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  //lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}


void LteRrcSibConnToIdleTest1WithDSDS::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Read SIBs for conn to idle when all SIBs are already present in db and event 
not already active for cell. Event should be created, and get_sibs_cnf sent 
right away. On receiving camped_indi, event should be cleared but no 
sib_sched_req sent

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibConnToIdleTest);

void LteRrcSibConnToIdleTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibConnToIdleTest::Test()
{
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_wcdma_supported = TRUE;
  lte_rrc_sib_test_gsm_supported = TRUE;
  lte_rrc_sib_test_cdma_1x_supported = TRUE;
  lte_rrc_sib_test_cdma_do_supported = TRUE;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  /* Conn to idle test */
  lte_rrc_sib_test_conn_to_idle();

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibConnToIdleTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibConnToIdleTest2);

void LteRrcSibConnToIdleTest2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibConnToIdleTest2::Test()
{
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_wcdma_supported = TRUE;
  lte_rrc_sib_test_gsm_supported = TRUE;
  lte_rrc_sib_test_cdma_1x_supported = TRUE;
  lte_rrc_sib_test_cdma_do_supported = TRUE;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();

  /* Conn to idle test */
  lte_rrc_sib_test_conn_to_idle2();

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibConnToIdleTest2::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibConnToIdleTest3);

void LteRrcSibConnToIdleTest3::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibConnToIdleTest3::Test()
{
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_wcdma_supported = TRUE;
  lte_rrc_sib_test_gsm_supported = TRUE;
  lte_rrc_sib_test_cdma_1x_supported = TRUE;
  lte_rrc_sib_test_cdma_do_supported = TRUE;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  /* Conn to idle test */
  lte_rrc_sib_test_conn_to_idle3();

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibConnToIdleTest3::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibConnToIdleTest5);

void LteRrcSibConnToIdleTest5::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibConnToIdleTest5::Test()
{
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_wcdma_supported = TRUE;
  lte_rrc_sib_test_gsm_supported = TRUE;
  lte_rrc_sib_test_cdma_1x_supported = TRUE;
  lte_rrc_sib_test_cdma_do_supported = TRUE;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  
  /* Conn to idle test */
  lte_rrc_sib_test_conn_to_idle5(LTE_RRC_SIB_TEST_CONN_TO_IDLE5_TEST_SIBS_CONSISTENT_VALUE_TAGS_MATCH);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibConnToIdleTest5::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibConnToIdleTest6);

void LteRrcSibConnToIdleTest6::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibConnToIdleTest6::Test()
{
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_wcdma_supported = TRUE;
  lte_rrc_sib_test_gsm_supported = TRUE;
  lte_rrc_sib_test_cdma_1x_supported = TRUE;
  lte_rrc_sib_test_cdma_do_supported = TRUE;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  
  /* Conn to idle test */
  lte_rrc_sib_test_conn_to_idle5(LTE_RRC_SIB_TEST_CONN_TO_IDLE5_TEST_SIBS_CONSISTENT_VALUE_TAGS_DO_NOT_MATCH);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibConnToIdleTest6::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Sib reading active but mandatory sibs not read then connected to idle on same 
cell

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibConnToIdleTest7);

void LteRrcSibConnToIdleTest7::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibConnToIdleTest7::Test()
{
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_wcdma_supported = TRUE;
  lte_rrc_sib_test_gsm_supported = TRUE;
  lte_rrc_sib_test_cdma_1x_supported = TRUE;
  lte_rrc_sib_test_cdma_do_supported = TRUE;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  // ----------------------------------------------------------------------

  /* HO and then connected to idle */
  lte_rrc_sib_test_conn_to_idle7();

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibConnToIdleTest7::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Get sibs req rcvd for conn to idle after some failure in connected mode such 
that the new cell and SIB's serving cell do not match - the case should be 
treated as cell selection and all SIBs should be read for the new cell

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibConnToIdleServCellDiffTest);

void LteRrcSibConnToIdleServCellDiffTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibConnToIdleServCellDiffTest::Test()
{
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_wcdma_supported = TRUE;
  lte_rrc_sib_test_gsm_supported = TRUE;
  lte_rrc_sib_test_cdma_1x_supported = TRUE;
  lte_rrc_sib_test_cdma_do_supported = TRUE;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  // ----------------------------------------------------------------------

  /* Connected to idle when SIB's serving cell has not been updated */
  lte_rrc_sib_test_conn_to_idle_serv_cell_diff();

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibConnToIdleServCellDiffTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Get sibs req rcvd for conn to idle after some failure in connected mode such 
that the serving cell SIBs are inconsistent - the case should be 
treated as cell selection and all SIBs should be read for the new cell

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibConnToIdleServCellSibsInconsistent);

void LteRrcSibConnToIdleServCellSibsInconsistent::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibConnToIdleServCellSibsInconsistent::Test()
{
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_wcdma_supported = TRUE;
  lte_rrc_sib_test_gsm_supported = TRUE;
  lte_rrc_sib_test_cdma_1x_supported = TRUE;
  lte_rrc_sib_test_cdma_do_supported = TRUE;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  // ----------------------------------------------------------------------

  /* Connected to idle when serving cell sibs are inconsistent */
  lte_rrc_sib_test_conn_to_idle_serv_cell_sibs_inconsistent();

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibConnToIdleServCellSibsInconsistent::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Go from conn to idle when SIB reading is not active but not all SIBs are 
present, and before receiving any SIBs go back to connected - sched req was sent 
to read SIBs that were not present so another one should be sent to stop SIB 
reading

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibConnToIdleToConnTest);

void LteRrcSibConnToIdleToConnTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibConnToIdleToConnTest::Test()
{
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_wcdma_supported = TRUE;
  lte_rrc_sib_test_gsm_supported = TRUE;
  lte_rrc_sib_test_cdma_1x_supported = TRUE;
  lte_rrc_sib_test_cdma_do_supported = TRUE;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();

  lte_rrc_sib_test_conn_to_idle_to_conn();

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibConnToIdleToConnTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
This unit test tests when get_sib_reqi has conn_to_idle cause and sib13 is 
present in sib1 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibConnToIdleWitheMBMS);

void LteRrcSibConnToIdleWitheMBMS::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibConnToIdleWitheMBMS::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  uint32 buf_len;

  lte_rrc_sib_test_embms_is_enabled = TRUE;

  // Set the cause of next get_sibs_reqi to be EMBMS, event parameters will be changed
  // after this request
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1,
                                      FREQ_1,
                                      LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_SEL,
                                      FALSE,
                                      0);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            1);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_with_sib13_8_5,
                                        sizeof(sib1_with_sib13_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB13_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB13_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB13_BITMASK));

  // send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_8_5,
                                        sizeof(sib2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        5);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB13_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB13_BITMASK ));

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK ),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB13_BITMASK ));

  // send SIB13 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib13_8_5,
                                        sizeof(sib13_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        9);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 9);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB13_BITMASK ));

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_SIB13_BITMASK ),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB13_BITMASK ));

  //---------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            64);
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_with_sib13_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_MIB_BITMASK |
                                         LTE_RRC_SIB1_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB13_BITMASK ));

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB13_BITMASK));

  //---------------------------------------------------------------

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL, TRUE, 
                                LTE_RRC_NULL_SFN);

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  //TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  //TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  //TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  //TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

#if 0
  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);
#endif
  //---------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibConnToIdleWitheMBMS::Teardown()
{
  lte_rrc_utf_teardown();
}
