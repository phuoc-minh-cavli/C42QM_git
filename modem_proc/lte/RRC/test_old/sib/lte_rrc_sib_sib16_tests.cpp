/*!
  @file lte_rrc_sib_sib16_tests.cpp

  @brief
  SIB8 related test cases
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

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "__lte_rrc_sib.h"
#include "__lte_rrc_controller.h"
#include "lte_rrc_sibi.h"
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

DECLARE_SIB1_2345_8_MCC_8_5;
DECLARE_SIB1_2345_MCC_8_5;
DECLARE_SIB1_VALUETAGCHANGE_MCC_8_5;
DECLARE_SIB2_SIB3_8_5;
DECLARE_SIB4_SIB5_8_5;
DECLARE_SIB8_8_5;
DECLARE_SIB8_SYNC_8_5;
DECLARE_SIB8_ASYNC_8_5;
DECLARE_SIB1_8_16_sched;
DECLARE_SIB16_SAMPLE_1;
DECLARE_SIB1_id2_16_sched;
DECLARE_SIB16_NULL_INFO;

extern "C"
{
extern lte_rrc_sib_s lte_rrc_sib;
extern lte_l1_sfn_t lte_rrc_sib_get_sibx_si_win_bnd(lte_rrc_sib_event_s *event_ptr, lte_l1_sfn_t sibx_sfn, lte_rrc_sib_bitmask_t sib_mask);
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern boolean lte_rrc_sib_test_camped;
extern stm_state_t lte_rrc_sib_test_controller_state;
extern void lte_rrc_sib_test_resel6(boolean resel_from_basic_cell1);
extern boolean lte_rrc_sib_test_ho_in_progress;
extern void lte_rrc_sib_test_ho6();

/*===========================================================================

                                FUNCTIONS

===========================================================================*/


/*Function to camp on a cell with SIB 16 */
void lte_rrc_sib_test_camp_cell1_with_sib16
(
  uint16 phy_cell_id,
  uint16 freq
)
{
  lte_cphy_irat_cdma_system_time_chg_req_s *sys_time_chg_req_ptr;
  lte_cphy_utc_time_update_req_s *utc_time_update_req;
  lte_cphy_utc_time_update_cnf_s utc_time_update_cnf;
  lte_rrc_utc_time_update_ind_s *utc_time_update_nas_ind_ptr;

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();

  // ----------------------------------------------------------------------

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_8_16_sched,
                                        sizeof(sib1_8_16_sched),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB16_BITMASK));
  
  // ----------------------------------------------------------------------

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  // this will make CSP return this cell as serving cell, though camped_indi as 
  // not been sent for the cell
  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;

  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  // send SIB16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib16_sample_1,
                                        sizeof(sib16_sample_1),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        0);

  // ----------------------------------------------------------------------

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  //---------------------------------------------------------------

  // send SIB16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib16_sample_1,
                                        sizeof(sib16_sample_1),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        0);

  // We expect a LTE_CPHY_UTC_TIME_UPDATE_REQ from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_req, &buf_len);

  TF_ASSERT(utc_time_update_req->msgr_hdr.id == LTE_CPHY_UTC_TIME_UPDATE_REQ);
  TF_ASSERT(utc_time_update_req->cell_info.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(utc_time_update_req->cell_info.dl_cell_frequency == FREQ_1);
  TF_ASSERT(utc_time_update_req->enabled == TRUE);
  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == TRUE);
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == 
            LTE_RRC_SIB_PERIODIC_SIB16_TMR_DURATION);

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1, 
                                        FREQ_1, 
                                        (LTE_RRC_SIB16_BITMASK), 
                                        (LTE_RRC_MIB_BITMASK |
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK |
                                         LTE_RRC_SIB16_BITMASK));

  //---------------------------------------------------------------

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

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_8_16_sched,
                                        sizeof(sib1_8_16_sched),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

  //Send lte_cphy_utc_time_update_cnf
  lte_rrc_sib_test_send_utc_time_update_cnf(E_SUCCESS);

  //Get UTC_NAS_INDI with all the needed Information
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_nas_ind_ptr, &buf_len);
  TF_ASSERT(utc_time_update_nas_ind_ptr->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND);
  TF_ASSERT(utc_time_update_nas_ind_ptr->utc_sib16_acquired == TRUE);
  
  lte_rrc_utf_check_no_more_msgs();
}

void lte_rrc_sib_test_periodic_sib16
(
   void
)
{
  lte_rrc_tmri_s *tmr_ptr;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_cphy_utc_time_update_req_s *utc_time_update_req;
  lte_cphy_utc_time_update_cnf_s utc_time_update_cnf;
  lte_rrc_utc_time_update_ind_s *utc_time_update_nas_ind_ptr;
  uint32 buf_len;

  TF_SLEEP(LTE_RRC_SIB_PERIODIC_SIB16_TMR_DURATION + 150);
  //Catch 10 min 3hr-timer ticks
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_3HR_CLOCK_TICK_TMRI);
  
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_3HR_CLOCK_TICK_TMRI ||
            tmr_ptr->msg_hdr.id == LTE_RRC_PERIODIC_SIB16_TMRI);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_PERIODIC_SIB16_TMRI ||
            tmr_ptr->msg_hdr.id == LTE_RRC_SIB_3HR_CLOCK_TICK_TMRI);

  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == FALSE);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

  //---------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_8_16_sched,
                                        sizeof(sib1_8_16_sched),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        1);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  //---------------------------------------------------------------

  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == FALSE);

  // send SIB16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib16_sample_1,
                                        sizeof(sib16_sample_1),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        0);

  // We expect a LTE_CPHY_UTC_TIME_UPDATE_REQ from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_req, &buf_len);

  TF_ASSERT(utc_time_update_req->msgr_hdr.id == LTE_CPHY_UTC_TIME_UPDATE_REQ);
  TF_ASSERT(utc_time_update_req->cell_info.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(utc_time_update_req->cell_info.dl_cell_frequency == FREQ_1);
  TF_ASSERT(utc_time_update_req->enabled == TRUE);
  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == TRUE);
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == 
            LTE_RRC_SIB_PERIODIC_SIB16_TMR_DURATION);
  

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

  //Send lte_cphy_utc_time_update_cnf
  lte_rrc_sib_test_send_utc_time_update_cnf(E_SUCCESS);

  //Get UTC_NAS_INDI with all the needed Information
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_nas_ind_ptr, &buf_len);
  TF_ASSERT(utc_time_update_nas_ind_ptr->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND);
  TF_ASSERT(utc_time_update_nas_ind_ptr->utc_sib16_acquired == TRUE);

  lte_rrc_utf_check_no_more_msgs();
}

void lte_rrc_sib_test_wt_tmr_exp_sib16
(
   void
)
{
  lte_rrc_get_sibs_reqi_s get_sibs_req;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;
  lte_rrc_utc_time_update_ind_s *utc_time_update_nas_ind_ptr;
  lte_cphy_mib_ind_s mib_ind;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_8_16_sched,
                                        sizeof(sib1_8_16_sched),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB16_BITMASK));
  
  // ----------------------------------------------------------------------

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  // this will make CSP return this cell as serving cell, though camped_indi as 
  // not been sent for the cell
  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  // ----------------------------------------------------------------------

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  //tmr expiry
  TF_SLEEP(LTE_RRC_SIB_NON_MAND_SIBS_WT_TMR);

  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));
  TF_ASSERT(read_error_ind_ptr->cause == LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT);
  TF_ASSERT(read_error_ind_ptr->csg_indication == TRUE);
  TF_ASSERT(read_error_ind_ptr->intra_freq_resel_allowed == TRUE);


  //Get UTC_NAS_INDI with all the needed Information                                
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_nas_ind_ptr, &buf_len);     
  TF_ASSERT(utc_time_update_nas_ind_ptr->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND);
  TF_ASSERT(utc_time_update_nas_ind_ptr->utc_sib16_acquired == FALSE);              


  // We expect a sib_sched_req message from RRC to Stop SIB reading
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);

  // ----------------------------------------------------------------------
  lte_rrc_utf_check_no_more_msgs();
}

void lte_rrc_sib_test_wt_tmr_exp_in_conn_sib16
(
   void
)
{
  lte_rrc_get_sibs_reqi_s get_sibs_req;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;
  lte_rrc_utc_time_update_ind_s *utc_time_update_nas_ind_ptr;
  lte_cphy_mib_ind_s mib_ind;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;

   // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_8_16_sched,
                                        sizeof(sib1_8_16_sched),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB16_BITMASK));
  
  // ----------------------------------------------------------------------

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  // this will make CSP return this cell as serving cell, though camped_indi as 
  // not been sent for the cell
  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  // ----------------------------------------------------------------------

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  lte_rrc_sib_test_send_connected_indi();

// // We expect a sib_sched_req message from RRC
//lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
//TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
//
//TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
//TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
//TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
//TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
//TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
//                                                       LTE_RRC_SIB1_BITMASK));

  //---------------------------------------------------------------

  /* Next mod prd value tag check */
  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            65);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_8_16_sched,
                                        sizeof(sib1_8_16_sched),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        66);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 66);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB16_BITMASK));
  
  //tmr expiry
  TF_SLEEP(LTE_RRC_SIB_NON_MAND_SIBS_WT_TMR);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));
  TF_ASSERT(read_error_ind_ptr->cause == LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT);
  TF_ASSERT(read_error_ind_ptr->csg_indication == TRUE);
  TF_ASSERT(read_error_ind_ptr->intra_freq_resel_allowed == TRUE);


  //Get UTC_NAS_INDI with all the needed Information                                
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_nas_ind_ptr, &buf_len);     
  TF_ASSERT(utc_time_update_nas_ind_ptr->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND);
  TF_ASSERT(utc_time_update_nas_ind_ptr->utc_sib16_acquired == FALSE);              


  // We expect a sib_sched_req message from RRC to Stop SIB reading
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

  //---------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  
}

void lte_rrc_sib_test_back_to_back_resel
(
   void
)
{
  lte_rrc_get_sibs_reqi_s get_sibs_req;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;
  lte_rrc_utc_time_update_ind_s *utc_time_update_nas_ind_ptr;
  lte_cphy_mib_ind_s mib_ind;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_cphy_utc_time_update_req_s *utc_time_update_req;
  lte_cphy_utc_time_update_cnf_s utc_time_update_cnf;

  
  /* Reselect to another cell2 */
  lte_rrc_sib_test_resel6(FALSE);

  //Try to reselect to same cell1
  // Prepare abort_reqi to send to SIB
  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_2,
                                   FREQ_2);

  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL, TRUE, 
                                      LTE_RRC_NULL_SFN);
 
  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_8_16_sched,
                                        sizeof(sib1_8_16_sched),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB16_BITMASK));

  // this will make CSP return this cell as serving cell, though camped_indi as 
  // not been sent for the cell
  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);


  // send SIB16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib16_sample_1,
                                        sizeof(sib16_sample_1),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        32);

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // send SIB16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib16_sample_1,
                                        sizeof(sib16_sample_1),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        32);
                                        
  // We expect a LTE_CPHY_UTC_TIME_UPDATE_REQ from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_req, &buf_len);

  TF_ASSERT(utc_time_update_req->msgr_hdr.id == LTE_CPHY_UTC_TIME_UPDATE_REQ);
  TF_ASSERT(utc_time_update_req->cell_info.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(utc_time_update_req->cell_info.dl_cell_frequency == FREQ_1);
  TF_ASSERT(utc_time_update_req->enabled == TRUE);
  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == TRUE);
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == 
            LTE_RRC_SIB_PERIODIC_SIB16_TMR_DURATION);

   //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
                                                         
  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1, 
                                        FREQ_1, 
                                        (LTE_RRC_SIB16_BITMASK), 
                                        (LTE_RRC_MIB_BITMASK |
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK |
                                         LTE_RRC_SIB16_BITMASK));

  //---------------------------------------------------------------
  //Send lte_cphy_utc_time_update_cnf
  lte_rrc_sib_test_send_utc_time_update_cnf(E_SUCCESS);

  //Get UTC_NAS_INDI with all the needed Information
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_nas_ind_ptr, &buf_len);
  TF_ASSERT(utc_time_update_nas_ind_ptr->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND);
  TF_ASSERT(utc_time_update_nas_ind_ptr->utc_sib16_acquired == TRUE);

  //---------------------------------------------------------------
  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            132);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_8_16_sched,
                                        sizeof(sib1_8_16_sched),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        135);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

  lte_rrc_utf_check_no_more_msgs();
}

void lte_rrc_sib_test_resel_to_cell2
(
   void
)
{
  lte_rrc_get_sibs_reqi_s get_sibs_req;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;
  lte_rrc_utc_time_update_ind_s *utc_time_update_nas_ind_ptr;
  lte_cphy_mib_ind_s mib_ind;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_cphy_utc_time_update_req_s *utc_time_update_req;
  lte_cphy_utc_time_update_cnf_s utc_time_update_cnf;

  
  //Try to reselect to same cell1
  // Prepare abort_reqi to send to SIB
  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1,
                                   FREQ_1);

  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL, TRUE, 
                                      LTE_RRC_NULL_SFN);
 
  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_id2_16_sched,
                                        sizeof(sib1_id2_16_sched),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        4);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB16_BITMASK));

  // ----------------------------------------------------------------------

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        6);

  //---------------------------------------------------------------

  // this will make CSP return this cell as serving cell, though camped_indi as 
  // not been sent for the cell
  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_2);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 1);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
////---------------------------------------------------------------
//
//lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_2,
//                                      FREQ_2,
//                                      (LTE_RRC_SIB16_BITMASK),
//                                      (LTE_RRC_MIB_BITMASK |
//                                       LTE_RRC_SIB1_BITMASK |
//                                       LTE_RRC_SIB2_BITMASK |
//                                       LTE_RRC_SIB16_BITMASK));

  //---------------------------------------------------------------

  // send SIB16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib16_sample_1,
                                        sizeof(sib16_sample_1),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        32);

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_2, FREQ_2);

  // send SIB16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib16_sample_1,
                                        sizeof(sib16_sample_1),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        48);

  // We expect a LTE_CPHY_UTC_TIME_UPDATE_REQ from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_req, &buf_len);

  TF_ASSERT(utc_time_update_req->msgr_hdr.id == LTE_CPHY_UTC_TIME_UPDATE_REQ);
  TF_ASSERT(utc_time_update_req->cell_info.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(utc_time_update_req->cell_info.dl_cell_frequency == FREQ_2);
  TF_ASSERT(utc_time_update_req->enabled == TRUE);
  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == TRUE);
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == 
            LTE_RRC_SIB_PERIODIC_SIB16_TMR_DURATION);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  //---------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            66);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_id2_16_sched,
                                        sizeof(sib1_id2_16_sched),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        68);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  //Send lte_cphy_utc_time_update_cnf
  lte_rrc_sib_test_send_utc_time_update_cnf(E_SUCCESS);

  //Get UTC_NAS_INDI with all the needed Information
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_nas_ind_ptr, &buf_len);
  TF_ASSERT(utc_time_update_nas_ind_ptr->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND);
  TF_ASSERT(utc_time_update_nas_ind_ptr->utc_sib16_acquired == TRUE);

  lte_rrc_utf_check_no_more_msgs();

}

void lte_rrc_sib_test_resel_to_cell_with_SI_containing_Sib2Sib16
(
   void
)
{
  lte_rrc_get_sibs_reqi_s get_sibs_req;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;
  lte_rrc_utc_time_update_ind_s *utc_time_update_nas_ind_ptr;
  lte_cphy_mib_ind_s mib_ind;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_cphy_utc_time_update_req_s *utc_time_update_req;
  lte_cphy_utc_time_update_cnf_s utc_time_update_cnf;

  
  //Try to reselect to same cell1
  // Prepare abort_reqi to send to SIB
  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1,
                                   FREQ_1);

  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL, TRUE, 
                                      LTE_RRC_NULL_SFN);
 
  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_id2_16_sched,
                                        sizeof(sib1_id2_16_sched),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        4);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB16_BITMASK));

  // ----------------------------------------------------------------------
  uint8 sib2_16[] = {0x00, 0x80, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xB0, 0x00, 0x80, 0x00, 0x00, 0x01, 0x08,
    0x0C, 0x80, 0x05, 0x01, 0x89, 0xE0, 0x20,};

  // send SIB2_16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_16,
                                        sizeof(sib2_16),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        16);

  //---------------------------------------------------------------

  // this will make CSP return this cell as serving cell, though camped_indi as 
  // not been sent for the cell
  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_2);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 1);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  // send SIB2_16 to RRC
  //This SI has to be ignored since SIB2 is not needed again and SIB16 cant be processed w/o camping
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_16,
                                        sizeof(sib2_16),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        32);

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_2, FREQ_2);

  // send SIB2_16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_16,
                                        sizeof(sib2_16),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        48);

  // We expect a LTE_CPHY_UTC_TIME_UPDATE_REQ from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_req, &buf_len);

  TF_ASSERT(utc_time_update_req->msgr_hdr.id == LTE_CPHY_UTC_TIME_UPDATE_REQ);
  TF_ASSERT(utc_time_update_req->cell_info.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(utc_time_update_req->cell_info.dl_cell_frequency == FREQ_2);
  TF_ASSERT(utc_time_update_req->enabled == TRUE);
  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == TRUE);
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == 
            LTE_RRC_SIB_PERIODIC_SIB16_TMR_DURATION);

    //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  //---------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            66);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_id2_16_sched,
                                        sizeof(sib1_id2_16_sched),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        68);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  //Send lte_cphy_utc_time_update_cnf
  lte_rrc_sib_test_send_utc_time_update_cnf(E_SUCCESS);

  //Get UTC_NAS_INDI with all the needed Information
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_nas_ind_ptr, &buf_len);
  TF_ASSERT(utc_time_update_nas_ind_ptr->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND);
  TF_ASSERT(utc_time_update_nas_ind_ptr->utc_sib16_acquired == TRUE);

  lte_rrc_utf_check_no_more_msgs();

}

void lte_rrc_sib_test_ho_to_cell2
(
  void
)
{
  lte_rrc_get_sibs_reqi_s get_sibs_req;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;
  lte_rrc_utc_time_update_ind_s *utc_time_update_nas_ind_ptr;
  lte_cphy_mib_ind_s mib_ind;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_cphy_utc_time_update_req_s *utc_time_update_req;
  lte_cphy_utc_time_update_cnf_s utc_time_update_cnf;

    // go to connected
  lte_rrc_sib_test_send_connected_indi();

  // HO to another cell

  lte_rrc_sib_test_send_ho_started_indi();
  lte_rrc_sib_test_ho_in_progress = FALSE;

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_id2_16_sched,
                                        sizeof(sib1_id2_16_sched),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        4);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB16_BITMASK));

  // ----------------------------------------------------------------------

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        6);

  //---------------------------------------------------------------

  // this will make CSP return this cell as serving cell, though camped_indi as 
  // not been sent for the cell
  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_2);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 1);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  
  //camped_indi
  //lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

////---------------------------------------------------------------
//
//lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_2,
//                                      FREQ_2,
//                                      (LTE_RRC_SIB16_BITMASK),
//                                      (LTE_RRC_MIB_BITMASK |
//                                       LTE_RRC_SIB1_BITMASK |
//                                       LTE_RRC_SIB2_BITMASK |
//                                       LTE_RRC_SIB16_BITMASK));

  //---------------------------------------------------------------

  // send SIB16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib16_sample_1,
                                        sizeof(sib16_sample_1),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        32);

  // We expect a LTE_CPHY_UTC_TIME_UPDATE_REQ from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_req, &buf_len);

  TF_ASSERT(utc_time_update_req->msgr_hdr.id == LTE_CPHY_UTC_TIME_UPDATE_REQ);
  TF_ASSERT(utc_time_update_req->cell_info.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(utc_time_update_req->cell_info.dl_cell_frequency == FREQ_2);
  TF_ASSERT(utc_time_update_req->enabled == TRUE);
  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == TRUE);
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == 
            LTE_RRC_SIB_PERIODIC_SIB16_TMR_DURATION);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));

  //---------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            66);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_id2_16_sched,
                                        sizeof(sib1_id2_16_sched),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        68);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  //Send lte_cphy_utc_time_update_cnf
  lte_rrc_sib_test_send_utc_time_update_cnf(E_SUCCESS);

  //Get UTC_NAS_INDI with all the needed Information
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_nas_ind_ptr, &buf_len);
  TF_ASSERT(utc_time_update_nas_ind_ptr->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND);
  TF_ASSERT(utc_time_update_nas_ind_ptr->utc_sib16_acquired == TRUE);

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_2, 
                                        FREQ_2, 
                                        (LTE_RRC_SIB16_BITMASK), 
                                        (LTE_RRC_MIB_BITMASK |
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK |
                                         LTE_RRC_SIB16_BITMASK));

  lte_rrc_utf_check_no_more_msgs();

}

void lte_rrc_sib_test_back_to_back_ho
(
   void
)
{
  lte_rrc_get_sibs_reqi_s get_sibs_req;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;
  lte_rrc_utc_time_update_ind_s *utc_time_update_nas_ind_ptr;
  lte_cphy_mib_ind_s mib_ind;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_cphy_utc_time_update_req_s *utc_time_update_req;
  lte_cphy_utc_time_update_cnf_s utc_time_update_cnf;

  
  /* HO to another cell2 */
  lte_rrc_sib_test_ho6();

  //Try to reselect to same cell1
   // go to connected
  lte_rrc_sib_test_send_connected_indi();
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
  // HO to another cell

  lte_rrc_sib_test_send_ho_started_indi();
  lte_rrc_sib_test_ho_in_progress = FALSE;

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE, TRUE, 
                                      LTE_RRC_NULL_SFN);
 
  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_8_16_sched,
                                        sizeof(sib1_8_16_sched),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB16_BITMASK));

  // this will make CSP return this cell as serving cell, though camped_indi as 
  // not been sent for the cell
  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);


  // send SIB16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib16_sample_1,
                                        sizeof(sib16_sample_1),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        32);

  // We expect a LTE_CPHY_UTC_TIME_UPDATE_REQ from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_req, &buf_len);

  TF_ASSERT(utc_time_update_req->msgr_hdr.id == LTE_CPHY_UTC_TIME_UPDATE_REQ);
  TF_ASSERT(utc_time_update_req->cell_info.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(utc_time_update_req->cell_info.dl_cell_frequency == FREQ_1);
  TF_ASSERT(utc_time_update_req->enabled == TRUE);
  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == TRUE);
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == 
            LTE_RRC_SIB_PERIODIC_SIB16_TMR_DURATION);

   //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));

  //---------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            132);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_8_16_sched,
                                        sizeof(sib1_8_16_sched),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        135);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

  //Send lte_cphy_utc_time_update_cnf
  lte_rrc_sib_test_send_utc_time_update_cnf(E_SUCCESS);

  //Get UTC_NAS_INDI with all the needed Information
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_nas_ind_ptr, &buf_len);
  TF_ASSERT(utc_time_update_nas_ind_ptr->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND);
  TF_ASSERT(utc_time_update_nas_ind_ptr->utc_sib16_acquired == TRUE);

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1, 
                                        FREQ_1, 
                                        (LTE_RRC_SIB16_BITMASK), 
                                        (LTE_RRC_MIB_BITMASK |
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK |
                                         LTE_RRC_SIB16_BITMASK));

  lte_rrc_utf_check_no_more_msgs();


}

void lte_rrc_sib_test_sib16_rcvd_before_sib1
(
   void
)
{
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();

  // send SIB16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib16_sample_1,
                                        sizeof(sib16_sample_1),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  lte_rrc_utf_check_no_more_msgs();
}

void lte_rrc_sib_test_camp_cell1_with_null_sib16
(
   void
)
{
  lte_rrc_utc_time_update_ind_s *utc_time_update_nas_ind_ptr;

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();

  // ----------------------------------------------------------------------

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_8_16_sched,
                                        sizeof(sib1_8_16_sched),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB16_BITMASK));
  
  // ----------------------------------------------------------------------

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  // this will make CSP return this cell as serving cell, though camped_indi as 
  // not been sent for the cell
  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;

  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  // send SIB16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib16_null_info,
                                        sizeof(sib16_null_info),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        0);

  // ----------------------------------------------------------------------

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // send SIB16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib16_null_info,
                                        sizeof(sib16_null_info),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        0);

  //Get UTC_NAS_INDI with all the needed Information
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_nas_ind_ptr, &buf_len);
  TF_ASSERT(utc_time_update_nas_ind_ptr->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND);
  TF_ASSERT(utc_time_update_nas_ind_ptr->utc_sib16_acquired == FALSE);

  //---------------------------------------------------------------

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

  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1, 
                                        FREQ_1, 
                                        (LTE_RRC_SIB16_BITMASK), 
                                        (LTE_RRC_MIB_BITMASK |
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK |
                                         LTE_RRC_SIB16_BITMASK));

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

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_8_16_sched,
                                        sizeof(sib1_8_16_sched),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

  lte_rrc_utf_check_no_more_msgs();
}

void lte_rrc_sib_test_page_for_si_update
(
  void
)
{
  lte_rrc_get_sibs_reqi_s get_sibs_req;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;
  lte_rrc_utc_time_update_ind_s *utc_time_update_nas_ind_ptr;
  lte_cphy_mib_ind_s mib_ind;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_cphy_utc_time_update_req_s *utc_time_update_req;
  lte_cphy_utc_time_update_cnf_s utc_time_update_cnf;

  
  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE, TRUE, 
                                      LTE_RRC_NULL_SFN);
 
  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 0);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_8_16_sched,
                                        sizeof(sib1_8_16_sched),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK));

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // send SIB16 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib16_sample_1,
                                        sizeof(sib16_sample_1),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        70);

  // We expect a LTE_CPHY_UTC_TIME_UPDATE_REQ from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_req, &buf_len);

  TF_ASSERT(utc_time_update_req->msgr_hdr.id == LTE_CPHY_UTC_TIME_UPDATE_REQ);
  TF_ASSERT(utc_time_update_req->cell_info.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(utc_time_update_req->cell_info.dl_cell_frequency == FREQ_1);
  TF_ASSERT(utc_time_update_req->enabled == TRUE);
  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == TRUE);
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib16_tmr) == 
            LTE_RRC_SIB_PERIODIC_SIB16_TMR_DURATION);


  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK ));


  //---------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            130);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_8_16_sched,
                                        sizeof(sib1_8_16_sched),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        132);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

  //Send lte_cphy_utc_time_update_cnf
  lte_rrc_sib_test_send_utc_time_update_cnf(E_SUCCESS);

  //Get UTC_NAS_INDI with all the needed Information
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_nas_ind_ptr, &buf_len);
  TF_ASSERT(utc_time_update_nas_ind_ptr->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND);
  TF_ASSERT(utc_time_update_nas_ind_ptr->utc_sib16_acquired == TRUE);

  lte_rrc_utf_check_no_more_msgs();

}




/*===========================================================================

                                UNIT TEST
Camp on a cell with SIB16 - timer should be started -read SIB 16 properly - 
SIB16 should be read on timer expiry - timer should be restarted on reading SIB16
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibPeriodicSib16BasicTest);

void LteRrcSibPeriodicSib16BasicTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_UTC_TIME_UPDATE_IND,
      LTE_CPHY_UTC_TIME_UPDATE_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibPeriodicSib16BasicTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_cell1_with_sib16(PHY_CELL_ID_1, FREQ_1);
  lte_rrc_sib_test_periodic_sib16();

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibPeriodicSib16BasicTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to check SIB timer expiry while non-mandatory SIBs are being read but UE 
has already camped on the cell

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibWtTmrExpForSib16);

void LteRrcSibWtTmrExpForSib16::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_UTC_TIME_UPDATE_IND,
      LTE_CPHY_UTC_TIME_UPDATE_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibWtTmrExpForSib16::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  //Timer exp in idle mode
  lte_rrc_sib_test_wt_tmr_exp_sib16();

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibWtTmrExpForSib16::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to check SIB timer expiry while non-mandatory SIBs are being read but UE 
has already camped on the cell

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibWtTmrExpInConnForSib16);

void LteRrcSibWtTmrExpInConnForSib16::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_UTC_TIME_UPDATE_IND,
      LTE_CPHY_UTC_TIME_UPDATE_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibWtTmrExpInConnForSib16::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  lte_rrc_sib_test_wt_tmr_exp_in_conn_sib16();

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibWtTmrExpInConnForSib16::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to check Reselection scenarios. 
Camp on a cell1 with SIB16 
Move to cell2 without sib16 
Move back to cell 1
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibB2BReselForSib16);

void LteRrcSibB2BReselForSib16::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_UTC_TIME_UPDATE_IND,
      LTE_CPHY_UTC_TIME_UPDATE_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibB2BReselForSib16::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  lte_rrc_sib_test_camp_cell1_with_sib16(PHY_CELL_ID_1, FREQ_1);
  lte_rrc_sib_test_back_to_back_resel();
  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibB2BReselForSib16::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to check Reselection scenarios. 
Camp on a cell1 with SIB16 
Move to cell2 with sib16 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibReselToCellWithSib16);

void LteRrcSibReselToCellWithSib16::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_UTC_TIME_UPDATE_IND,
      LTE_CPHY_UTC_TIME_UPDATE_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibReselToCellWithSib16::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  lte_rrc_sib_test_camp_cell1_with_sib16(PHY_CELL_ID_1, FREQ_1);
  lte_rrc_sib_test_resel_to_cell2();
  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibReselToCellWithSib16::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to check the funtionality of lte_rrc_sib_get_sibx_si_win_bnd()

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibGetSib16SiWinBndTest);

void LteRrcSibGetSib16SiWinBndTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_UTC_TIME_UPDATE_IND,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibGetSib16SiWinBndTest::Test()
{
  lte_rrc_utc_time_update_ind_s *utc_time_update_nas_ind_ptr;
  uint32 buf_len;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_event_s event;

  event.sched_req.num_si_msgs = 2;
  event.sched_req.si_msg_info[1].sib_map_bitmask = LTE_RRC_SIB16_BITMASK;
  event.sched_req.si_window_length = LTE_CPHY_SI_WINDOW_LENGTH_MS_15;
  event.sched_req.si_msg_info[1].si_periodicity = LTE_CPHY_SI_PERIODICITY_RF_16;

  TF_ASSERT(lte_rrc_sib_get_sibx_si_win_bnd(&event, 2, LTE_RRC_SIB16_BITMASK) == 3);

  // ----------------------------------------------------------------------

  memset(&event, 0 , sizeof(lte_rrc_sib_event_s));

  event.sched_req.num_si_msgs = 3;
  event.sched_req.si_msg_info[2].sib_map_bitmask = LTE_RRC_SIB16_BITMASK;
  event.sched_req.si_window_length = LTE_CPHY_SI_WINDOW_LENGTH_MS_15;
  event.sched_req.si_msg_info[2].si_periodicity = LTE_CPHY_SI_PERIODICITY_RF_16;

  TF_ASSERT(lte_rrc_sib_get_sibx_si_win_bnd(&event, 3, LTE_RRC_SIB16_BITMASK) == 4);

  // ----------------------------------------------------------------------

  /* SFN rollover case */
  memset(&event, 0 , sizeof(lte_rrc_sib_event_s));

  event.sched_req.num_si_msgs = 2;
  event.sched_req.si_msg_info[1].sib_map_bitmask = LTE_RRC_SIB16_BITMASK;
  event.sched_req.si_window_length = LTE_CPHY_SI_WINDOW_LENGTH_MS_40;
  event.sched_req.si_msg_info[1].si_periodicity = LTE_CPHY_SI_PERIODICITY_RF_8;

  TF_ASSERT(lte_rrc_sib_get_sibx_si_win_bnd(&event, 1022, LTE_RRC_SIB16_BITMASK) == 0);

  /*Null SFN CASE*/
    memset(&event, 0 , sizeof(lte_rrc_sib_event_s));

  event.sched_req.num_si_msgs = 2;
  event.sched_req.si_msg_info[1].sib_map_bitmask = LTE_RRC_SIB8_BITMASK;
  event.sched_req.si_window_length = LTE_CPHY_SI_WINDOW_LENGTH_MS_40;
  event.sched_req.si_msg_info[1].si_periodicity = LTE_CPHY_SI_PERIODICITY_RF_8;

  TF_ASSERT(lte_rrc_sib_get_sibx_si_win_bnd(&event, 5, LTE_RRC_SIB16_BITMASK) == LTE_RRC_NULL_SFN);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();

/*
  //Get UTC_NAS_INDI with all the needed Information                                
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_nas_ind_ptr, &buf_len);     
  TF_ASSERT(utc_time_update_nas_ind_ptr->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND);
  TF_ASSERT(utc_time_update_nas_ind_ptr->utc_sib16_acquired == FALSE);              
*/

  lte_rrc_utf_reset();
}

void LteRrcSibGetSib16SiWinBndTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
SIB16 is received before SIB1. The SIB should be discarded
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSib16RcvdBeforeSib1Test);

void LteRrcSibSib16RcvdBeforeSib1Test::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_UTC_TIME_UPDATE_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSib16RcvdBeforeSib1Test::Test()
{
  lte_rrc_utc_time_update_ind_s *utc_time_update_nas_ind_ptr;
  uint32 buf_len;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* SIB8 is received before SIB1. The SIB should be discarded */
  lte_rrc_sib_test_sib16_rcvd_before_sib1();

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();

/*
  //Get UTC_NAS_INDI with all the needed Information                                
  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_nas_ind_ptr, &buf_len);     
  TF_ASSERT(utc_time_update_nas_ind_ptr->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND);
  TF_ASSERT(utc_time_update_nas_ind_ptr->utc_sib16_acquired == FALSE);              
*/

  lte_rrc_utf_reset();
}

void LteRrcSibSib16RcvdBeforeSib1Test::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check Handover scenarios. 
Camp on a cell1 with SIB16 
Move to connected and HO to cell2 with sib16 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibHOToCellWithSib16);

void LteRrcSibHOToCellWithSib16::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_UTC_TIME_UPDATE_IND,
      LTE_CPHY_UTC_TIME_UPDATE_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibHOToCellWithSib16::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  lte_rrc_sib_test_camp_cell1_with_sib16(PHY_CELL_ID_1, FREQ_1);
  lte_rrc_sib_test_ho_to_cell2();
  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibHOToCellWithSib16::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to check Reselection scenarios. 
Camp on a cell1 with SIB16 
Move to cell2 without sib16 
Move back to cell 1
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibB2BHOForSib16);

void LteRrcSibB2BHOForSib16::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_UTC_TIME_UPDATE_IND,
      LTE_CPHY_UTC_TIME_UPDATE_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibB2BHOForSib16::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  lte_rrc_sib_test_camp_cell1_with_sib16(PHY_CELL_ID_1, FREQ_1);
  lte_rrc_sib_test_back_to_back_ho();
  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibB2BHOForSib16::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Receive SIB16 with NULL data
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibNullSib16Info);

void LteRrcSibNullSib16Info::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_UTC_TIME_UPDATE_IND,
      LTE_CPHY_UTC_TIME_UPDATE_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibNullSib16Info::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  lte_rrc_sib_test_camp_cell1_with_null_sib16();
  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibNullSib16Info::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to check sib16 rcvd on sib_update page
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSiUpdateForSib16);

void LteRrcSibSiUpdateForSib16::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_UTC_TIME_UPDATE_IND,
      LTE_CPHY_UTC_TIME_UPDATE_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSiUpdateForSib16::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  lte_rrc_sib_test_camp_cell1_with_sib16(PHY_CELL_ID_1, FREQ_1);
  lte_rrc_sib_test_page_for_si_update();
  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibSiUpdateForSib16::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to check Reselection scenarios. 
Camp on a cell1 with SIB16 
Move to cell2 with sib16 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibReselToCellWithSib16InSIWithSib2);

void LteRrcSibReselToCellWithSib16InSIWithSib2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_UTC_TIME_UPDATE_IND,
      LTE_CPHY_UTC_TIME_UPDATE_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibReselToCellWithSib16InSIWithSib2::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  lte_rrc_sib_test_camp_cell1_with_sib16(PHY_CELL_ID_1, FREQ_1);
  lte_rrc_sib_test_resel_to_cell_with_SI_containing_Sib2Sib16();
  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibReselToCellWithSib16InSIWithSib2::Teardown()
{
  lte_rrc_utf_teardown();
}

