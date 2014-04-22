/*!
  @file lte_rrc_sib_sib8_tests.cpp

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_sib8_tests.cpp#1 $

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
#include "lte_misc.h"
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
DECLARE_SIB1_2345_MCC_8_5_CELL2;
DECLARE_SIB1_2345_8_MCC_8_5_CELL2;

extern "C"
{
extern lte_rrc_sib_s lte_rrc_sib;
extern lte_l1_sfn_t lte_rrc_sib_get_sibx_si_win_bnd(lte_rrc_sib_event_s *event_ptr, lte_l1_sfn_t sibx_sfn, lte_rrc_sib_bitmask_t sib_mask);
}

static uint64 time_since_power_up_ms = 0;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern boolean lte_rrc_sib_test_camped;
extern stm_state_t lte_rrc_sib_test_controller_state;
extern void lte_rrc_sib_test_resel1(boolean resel_from_basic_cell1);

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/* enum used to indicate which sib8 to use */
typedef enum
{
  LTE_RRC_SIB_TEST_SIB8_SYS_TIME_DISABLED,
  LTE_RRC_SIB_TEST_SIB8_SYS_TIME_TYPE_SYNC,
  LTE_RRC_SIB_TEST_SIB8_SYS_TIME_TYPE_ASYNC
} lte_rrc_sib_test_sib8_e;

/* Camp on cell1 that has SIB8 */
void lte_rrc_sib_test_camp_cell1_with_sib8(lte_rrc_sib_test_sib8_e sib8_type)
{
  lte_cphy_irat_cdma_system_time_chg_req_s *sys_time_chg_req_ptr;

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();

  // ----------------------------------------------------------------------

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_8_mcc_8_5,
                                        sizeof(sib1_2345_8_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB8_BITMASK));

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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));

  // ----------------------------------------------------------------------

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));

  // ----------------------------------------------------------------------

  if(sib8_type == LTE_RRC_SIB_TEST_SIB8_SYS_TIME_DISABLED)
  {
    // send SIB8 to RRC
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib8_8_5,
                                          sizeof(sib8_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          0);
  
    // We expect a sys_time_chg_req from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sys_time_chg_req_ptr, &buf_len);
  
    TF_ASSERT(sys_time_chg_req_ptr->msgr_hdr.id == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ);
    TF_ASSERT(sys_time_chg_req_ptr->cell_info.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sys_time_chg_req_ptr->cell_info.dl_cell_frequency == FREQ_1);
    TF_ASSERT(sys_time_chg_req_ptr->irat_cdma_system_time_info.sib8.sib8_sfn_boundary == 0);
    TF_ASSERT(sys_time_chg_req_ptr->enabled == FALSE);
    TF_ASSERT(sys_time_chg_req_ptr->source_type == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_SOURCE_SIB8);
    TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == FALSE);
  }
  else if(sib8_type == LTE_RRC_SIB_TEST_SIB8_SYS_TIME_TYPE_SYNC)
  {
    // send SIB8 to RRC
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib8_sync_8_5,
                                          sizeof(sib8_sync_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          0);

    // We expect a sys_time_chg_req from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sys_time_chg_req_ptr, &buf_len);

    TF_ASSERT(sys_time_chg_req_ptr->msgr_hdr.id == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ);
    TF_ASSERT(sys_time_chg_req_ptr->cell_info.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sys_time_chg_req_ptr->cell_info.dl_cell_frequency == FREQ_1);
    TF_ASSERT(sys_time_chg_req_ptr->irat_cdma_system_time_info.sib8.sib8_sfn_boundary == 0);
    TF_ASSERT(sys_time_chg_req_ptr->enabled == TRUE);
    TF_ASSERT(sys_time_chg_req_ptr->source_type == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_SOURCE_SIB8);
    TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == FALSE);
  }
  else if(sib8_type == LTE_RRC_SIB_TEST_SIB8_SYS_TIME_TYPE_ASYNC)
  {
    // send SIB8 to RRC
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib8_async_8_5,
                                          sizeof(sib8_async_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          0);

    // We expect a sys_time_chg_req from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sys_time_chg_req_ptr, &buf_len);

    TF_ASSERT(sys_time_chg_req_ptr->msgr_hdr.id == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ);
    TF_ASSERT(sys_time_chg_req_ptr->cell_info.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sys_time_chg_req_ptr->cell_info.dl_cell_frequency == FREQ_1);
    TF_ASSERT(sys_time_chg_req_ptr->irat_cdma_system_time_info.sib8.sib8_sfn_boundary == 0);
    TF_ASSERT(sys_time_chg_req_ptr->enabled == TRUE);
    TF_ASSERT(sys_time_chg_req_ptr->source_type == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_SOURCE_SIB8);
    TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == TRUE);
    TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == 
              LTE_RRC_SIB_PERIODIC_SIB8_TMR_DURATION);
  }

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
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));

  // ----------------------------------------------------------------------

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1, 
                                        FREQ_1, 
                                        (LTE_RRC_SIB3_BITMASK |
                                         LTE_RRC_SIB4_BITMASK | 
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB8_BITMASK), 
                                        (LTE_RRC_MIB_BITMASK |
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK |
                                         LTE_RRC_SIB3_BITMASK |
                                         LTE_RRC_SIB4_BITMASK | 
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB8_BITMASK));

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
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_8_mcc_8_5,
                                        sizeof(sib1_2345_8_mcc_8_5),
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
}

/* SIB8 should be read on timer expiry - timer should be restarted on reading SIB8 */
void lte_rrc_sib_test_periodic_sib8_async()
{
  lte_rrc_tmri_s *tmr_ptr;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_cphy_irat_cdma_system_time_chg_req_s *sys_time_chg_req_ptr;
  uint32 buf_len;

  TF_SLEEP(LTE_RRC_SIB_PERIODIC_SIB8_TMR_DURATION - 1);
  lte_rrc_utf_check_no_more_msgs();
  TF_SLEEP(1);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_PERIODIC_SIB8_TMRI);

  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == FALSE);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

  //---------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_8_mcc_8_5,
                                        sizeof(sib1_2345_8_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        1);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK | 
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK | 
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));

  //---------------------------------------------------------------

  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == FALSE);

  // send SIB8 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib8_async_8_5,
                                        sizeof(sib8_async_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        0);

  //---------------------------------------------------------------

  // We expect a sys_time_chg_req from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sys_time_chg_req_ptr, &buf_len);

  TF_ASSERT(sys_time_chg_req_ptr->msgr_hdr.id == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ);
  TF_ASSERT(sys_time_chg_req_ptr->cell_info.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sys_time_chg_req_ptr->cell_info.dl_cell_frequency == FREQ_1);
  TF_ASSERT(sys_time_chg_req_ptr->irat_cdma_system_time_info.sib8.sib8_sfn_boundary == 0);
  TF_ASSERT(sys_time_chg_req_ptr->enabled == TRUE);
  TF_ASSERT(sys_time_chg_req_ptr->source_type == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_SOURCE_SIB8);
  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == TRUE);
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == 
            LTE_RRC_SIB_PERIODIC_SIB8_TMR_DURATION);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
}

/* SIB8 timer expires when UE reselect to another cell */
void lte_rrc_sib_test_resel_to_cell2(boolean sib8_present)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_tmri_s *tmr_ptr;
  uint32 buf_len;

  // ----------------------------------------------------------------------

  TF_SLEEP(LTE_RRC_SIB_PERIODIC_SIB8_TMR_DURATION - 1);
  lte_rrc_utf_check_no_more_msgs();

  // ----------------------------------------------------------------------

  /* get sibs req - cell selection */
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE,
                                      120);


  // ----------------------------------------------------------------------

  //sib_sched_req
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);

  // ----------------------------------------------------------------------

  // send MIB to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            130);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));

  // ----------------------------------------------------------------------
  if(sib8_present)
  {
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_8_mcc_8_5_cell2, 
                                          sizeof(sib1_2345_8_mcc_8_5_cell2),
                                          PHY_CELL_ID_2,
                                          FREQ_2,
                                          130);


    // ----------------------------------------------------------------------

    //sib_sched_req
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK | 
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK | 
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK | 
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK | 
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);

    // ----------------------------------------------------------------------

    // send SIB2_3 to RRC
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                          sizeof(sib2_sib3_8_5),
                                          PHY_CELL_ID_2,
                                          FREQ_2,
                                          132);

    //---------------------------------------------------------------

    /* Serving cell changes */
    lte_rrc_sib.sd_ptr->serv_cell_index = 1;
    lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid = FALSE;

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
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB8_BITMASK));
  }
  else
  {
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_cell2, 
                                          sizeof(sib1_2345_mcc_8_5_cell2),
                                          PHY_CELL_ID_2,
                                          FREQ_2,
                                          130);


    // ----------------------------------------------------------------------

    //sib_sched_req
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK | 
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK | 
                                                           LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK | 
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK | 
                                                           LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);

    // ----------------------------------------------------------------------

    // send SIB2_3 to RRC
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                          sizeof(sib2_sib3_8_5),
                                          PHY_CELL_ID_2,
                                          FREQ_2,
                                          132);

    //---------------------------------------------------------------

    /* Serving cell changes */
    lte_rrc_sib.sd_ptr->serv_cell_index = 1;
    lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid = FALSE;

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
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK));
  }

  // ----------------------------------------------------------------------
  //sib8 timer expires
  TF_SLEEP(1);
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_PERIODIC_SIB8_TMRI);

  if(sib8_present)
  {
    // We expect a sib_sched_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB8_BITMASK));
  }
}

/* SIB8 timer expires when event is active for SI update */
void lte_rrc_sib_test_periodic_sib8_si_update(boolean sib8_removed)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_tmri_s *tmr_ptr;
  lte_cphy_irat_cdma_system_time_chg_req_s *sys_time_chg_req_ptr;
  uint32 buf_len;

  // ----------------------------------------------------------------------

  TF_SLEEP(LTE_RRC_SIB_PERIODIC_SIB8_TMR_DURATION - 1);
  lte_rrc_utf_check_no_more_msgs();

  // ----------------------------------------------------------------------

  /* get sibs req - paging */
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE, FALSE,
                                      120);


  // ----------------------------------------------------------------------

  //sib_sched_req
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK | 
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK | 
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);

  // ----------------------------------------------------------------------

  //sib8 timer expires
  TF_SLEEP(1);
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_PERIODIC_SIB8_TMRI);

  // ----------------------------------------------------------------------

  //sib_sched_req
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK | 
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK | 
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);

  // ----------------------------------------------------------------------

  if(!sib8_removed)
  {
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_8_mcc_8_5, 
                                          sizeof(sib1_2345_8_mcc_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          120);

    // ----------------------------------------------------------------------

    //sib_sched_req
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK | 
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK | 
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  
    // ----------------------------------------------------------------------
  
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib8_async_8_5, 
                                          sizeof(sib8_async_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          120);
  
    // ----------------------------------------------------------------------

    // We expect a sys_time_chg_req from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sys_time_chg_req_ptr, &buf_len);

    TF_ASSERT(sys_time_chg_req_ptr->msgr_hdr.id == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ);
    TF_ASSERT(sys_time_chg_req_ptr->cell_info.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sys_time_chg_req_ptr->cell_info.dl_cell_frequency == FREQ_1);
    TF_ASSERT(sys_time_chg_req_ptr->irat_cdma_system_time_info.sib8.sib8_sfn_boundary == 120);
    TF_ASSERT(sys_time_chg_req_ptr->enabled == TRUE);
    TF_ASSERT(sys_time_chg_req_ptr->source_type == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_SOURCE_SIB8);
    TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == TRUE);
    TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == 
              LTE_RRC_SIB_PERIODIC_SIB8_TMR_DURATION);

    // ----------------------------------------------------------------------

    //sib_sched_req
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK | 
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK | 
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);

  }
  else //sib8_removed
  {
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5, 
                                          sizeof(sib1_2345_mcc_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          120);


    // ----------------------------------------------------------------------

    //sib_sched_req
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK | 
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK | 
                                                           LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);

  }


}

/* SIB8 timer expires when event for conn to idle is active - SIB8 should be read 
and timer should be started */
void lte_rrc_sib_test_periodic_sib8_conn_to_idle()
{
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_tmri_s *tmr_ptr;
  lte_cphy_irat_cdma_system_time_chg_req_s *sys_time_chg_req_ptr;
  uint32 buf_len;

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  // Timer
  TF_SLEEP(LTE_RRC_SIB_PERIODIC_SIB8_TMR_DURATION - 1);

  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL, TRUE, 
                                LTE_RRC_NULL_SFN);


  // ---------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // ---------------------------------------------

  // Asserts
  {
    /* Asserts on SIB internal data */
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
    TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
    for(int i = 1; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
    {
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);
    TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == TRUE);
    TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == 1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->periodic_sib8_cell_id.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->periodic_sib8_cell_id.freq == FREQ_1);
  
    // ----------------------------------------------------------------------
  
    /* Asserts on active Events */
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_8_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr != NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_bitmask == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[i] == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_size == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.decoded_si_ptr[i] == NULL);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == FALSE);
    for(int i = 0; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == LTE_RRC_SIB_INVALID_TIME);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK | 
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                           LTE_RRC_SIB1_BITMASK |
                                                                           LTE_RRC_SIB2_BITMASK |
                                                                           LTE_RRC_SIB3_BITMASK |
                                                                           LTE_RRC_SIB4_BITMASK |
                                                                           LTE_RRC_SIB5_BITMASK |
                                                                           LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                                LTE_RRC_SIB1_BITMASK |
                                                                                LTE_RRC_SIB2_BITMASK |
                                                                                LTE_RRC_SIB3_BITMASK |
                                                                                LTE_RRC_SIB4_BITMASK |
                                                                                LTE_RRC_SIB5_BITMASK |
                                                                                LTE_RRC_SIB8_BITMASK));
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                                   LTE_RRC_SIB4_BITMASK |
                                                                                   LTE_RRC_SIB5_BITMASK |
                                                                                   LTE_RRC_SIB8_BITMASK));
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == LTE_RRC_NULL_SFN);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (0));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
    //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (0));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 3);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                                  LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                                  LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NONE);
  
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_8_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                         LTE_RRC_SIB3_BITMASK |
                                                                         LTE_RRC_SIB4_BITMASK |
                                                                         LTE_RRC_SIB5_BITMASK |
                                                                         LTE_RRC_SIB8_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib8_async_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2] != NULL);
    for(int i = 3; i < LTE_RRC_SIB_MAX_SIS; i++)
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
   // TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib8);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
  }

  // ---------------------------------------------

  // Timer
  TF_SLEEP(1);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_PERIODIC_SIB8_TMRI);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB8_BITMASK));

  // ---------------------------------------------

  // Asserts
  {
    /* Asserts on SIB internal data */
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
    TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
    for(int i = 1; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
    {
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);
    TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == FALSE);
    //TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == 1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->periodic_sib8_cell_id.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->periodic_sib8_cell_id.freq == FREQ_1);
  
    // ----------------------------------------------------------------------
  
    /* Asserts on active Events */
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_8_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr != NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_bitmask == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[i] == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_size == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.decoded_si_ptr[i] == NULL);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == FALSE);
    for(int i = 0; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == LTE_RRC_SIB_INVALID_TIME);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK | 
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                           LTE_RRC_SIB1_BITMASK |
                                                                           LTE_RRC_SIB2_BITMASK |
                                                                           LTE_RRC_SIB3_BITMASK |
                                                                           LTE_RRC_SIB4_BITMASK |
                                                                           LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                                LTE_RRC_SIB2_BITMASK |
                                                                                LTE_RRC_SIB3_BITMASK |
                                                                                LTE_RRC_SIB4_BITMASK |
                                                                                LTE_RRC_SIB5_BITMASK));
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                                   LTE_RRC_SIB4_BITMASK |
                                                                                   LTE_RRC_SIB5_BITMASK |
                                                                                   LTE_RRC_SIB8_BITMASK));
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == LTE_RRC_NULL_SFN);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                                                         LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (0));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 3);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                                  LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                                  LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);
  
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_8_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                         LTE_RRC_SIB3_BITMASK |
                                                                         LTE_RRC_SIB4_BITMASK |
                                                                         LTE_RRC_SIB5_BITMASK |
                                                                         LTE_RRC_SIB8_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB8_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib8_async_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2] != NULL);
    for(int i = 3; i < LTE_RRC_SIB_MAX_SIS; i++)
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
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib8);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
  }

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);

  //---------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_8_mcc_8_5,
                                        sizeof(sib1_2345_8_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        1);

  //---------------------------------------------------------------

  //sib_updated_indi
  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        LTE_RRC_SIB_SIB3_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK |
                                         LTE_RRC_SIB2_BITMASK |
                                         LTE_RRC_SIB3_BITMASK |
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB8_BITMASK));

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK | 
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK | 
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));

  //---------------------------------------------------------------

  // send SIB8 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib8_async_8_5,
                                        sizeof(sib8_async_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        0);

  //---------------------------------------------------------------

  //sib_updated_indi
  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        LTE_RRC_SIB8_BITMASK,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK |
                                         LTE_RRC_SIB2_BITMASK |
                                         LTE_RRC_SIB3_BITMASK |
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB8_BITMASK));

  //---------------------------------------------------------------

  // We expect a sys_time_chg_req from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sys_time_chg_req_ptr, &buf_len);

  TF_ASSERT(sys_time_chg_req_ptr->msgr_hdr.id == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ);
  TF_ASSERT(sys_time_chg_req_ptr->cell_info.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sys_time_chg_req_ptr->cell_info.dl_cell_frequency == FREQ_1);
  TF_ASSERT(sys_time_chg_req_ptr->irat_cdma_system_time_info.sib8.sib8_sfn_boundary == 0);
  TF_ASSERT(sys_time_chg_req_ptr->enabled == TRUE);
  TF_ASSERT(sys_time_chg_req_ptr->source_type == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_SOURCE_SIB8);
  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == TRUE);
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == 
            LTE_RRC_SIB_PERIODIC_SIB8_TMR_DURATION);

  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == TRUE);
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == 
            LTE_RRC_SIB_PERIODIC_SIB8_TMR_DURATION);
  TF_ASSERT(lte_rrc_sib.dd_ptr->periodic_sib8_cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->periodic_sib8_cell_id.freq == FREQ_1);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_SIB_MIB_SIB1_BITMASK));

  //---------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            64);
  
  //---------------------------------------------------------------

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
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);

  //---------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_8_mcc_8_5,
                                        sizeof(sib1_2345_8_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);
  
  //---------------------------------------------------------------

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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
}

/* SIB8 is received before SIB1. The SIB should be discarded */
void lte_rrc_sib_test_sib8_rcvd_before_sib1()
{
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();

  //send SIB8 - The main thing to check is SIB should not assert
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib8_8_5,
                                        sizeof(sib8_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        0);

  lte_rrc_utf_check_no_more_msgs();

}

/*===========================================================================

                                UNIT TEST
Test to check the funtionality of lte_rrc_sib_get_sibx_si_win_bnd()

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibGetSib8SiWinBndTest);

void LteRrcSibGetSib8SiWinBndTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibGetSib8SiWinBndTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_event_s event;

  event.sched_req.num_si_msgs = 2;
  event.sched_req.si_msg_info[1].sib_map_bitmask = LTE_RRC_SIB8_BITMASK;
  event.sched_req.si_window_length = LTE_CPHY_SI_WINDOW_LENGTH_MS_15;
  event.sched_req.si_msg_info[1].si_periodicity = LTE_CPHY_SI_PERIODICITY_RF_16;

  TF_ASSERT(lte_rrc_sib_get_sibx_si_win_bnd(&event, 2, LTE_RRC_SIB8_BITMASK) == 3);

  // ----------------------------------------------------------------------

  memset(&event, 0 , sizeof(lte_rrc_sib_event_s));

  event.sched_req.num_si_msgs = 3;
  event.sched_req.si_msg_info[2].sib_map_bitmask = LTE_RRC_SIB8_BITMASK;
  event.sched_req.si_window_length = LTE_CPHY_SI_WINDOW_LENGTH_MS_15;
  event.sched_req.si_msg_info[2].si_periodicity = LTE_CPHY_SI_PERIODICITY_RF_16;

  TF_ASSERT(lte_rrc_sib_get_sibx_si_win_bnd(&event, 3, LTE_RRC_SIB8_BITMASK) == 4);

  // ----------------------------------------------------------------------

  /* SFN rollover case */
  memset(&event, 0 , sizeof(lte_rrc_sib_event_s));

  event.sched_req.num_si_msgs = 2;
  event.sched_req.si_msg_info[1].sib_map_bitmask = LTE_RRC_SIB8_BITMASK;
  event.sched_req.si_window_length = LTE_CPHY_SI_WINDOW_LENGTH_MS_40;
  event.sched_req.si_msg_info[1].si_periodicity = LTE_CPHY_SI_PERIODICITY_RF_8;

  TF_ASSERT(lte_rrc_sib_get_sibx_si_win_bnd(&event, 1022, LTE_RRC_SIB8_BITMASK) == 0);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibGetSib8SiWinBndTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check that SIB8 should be read when going to a cell for which SIBs 
(including SIB8) are already present in the db

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSib8AlreadyPresentTest);

void LteRrcSibSib8AlreadyPresentTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSib8AlreadyPresentTest::Test()
{
  lte_cphy_irat_cdma_system_time_chg_req_s *sys_time_chg_req_ptr;
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_cell1_with_sib8(LTE_RRC_SIB_TEST_SIB8_SYS_TIME_DISABLED);

  // ----------------------------------------------------------------------

  /* Reselect to another cell */
  lte_rrc_sib_test_resel1(FALSE);

  // ----------------------------------------------------------------------

  /* Back to cell 1 */
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

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

  // ---------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            1);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK));

  // ---------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_8_mcc_8_5,
                                        sizeof(sib1_2345_8_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  // SIB8 should be requested even though already present in the db
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));

  // ---------------------------------------------

  // send SIB8 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib8_8_5,
                                        sizeof(sib8_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        0);

  // We expect a sys_time_chg_req from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sys_time_chg_req_ptr, &buf_len);

  TF_ASSERT(sys_time_chg_req_ptr->msgr_hdr.id == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ);
  TF_ASSERT(sys_time_chg_req_ptr->cell_info.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sys_time_chg_req_ptr->cell_info.dl_cell_frequency == FREQ_1);
  TF_ASSERT(sys_time_chg_req_ptr->irat_cdma_system_time_info.sib8.sib8_sfn_boundary == 0);

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
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));

  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibSib8AlreadyPresentTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Camp on a cell with SIB8 - synchronous system - timer should not be started
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibPeriodicSib8SyncTest);

void LteRrcSibPeriodicSib8SyncTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibPeriodicSib8SyncTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_cell1_with_sib8(LTE_RRC_SIB_TEST_SIB8_SYS_TIME_TYPE_SYNC);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibPeriodicSib8SyncTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Camp on a cell with SIB8 - asynchronous system - timer should be started - SIB8 
should be read on timer expiry - timer should be restarted on reading SIB8
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibPeriodicSib8AsyncTest);

void LteRrcSibPeriodicSib8AsyncTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibPeriodicSib8AsyncTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_cell1_with_sib8(LTE_RRC_SIB_TEST_SIB8_SYS_TIME_TYPE_ASYNC);

  lte_rrc_sib_test_periodic_sib8_async();

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibPeriodicSib8AsyncTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
SIB8 timer expires when event is active for SI update
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibPeriodicSib8SiUpdate);

void LteRrcSibPeriodicSib8SiUpdate::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibPeriodicSib8SiUpdate::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_cell1_with_sib8(LTE_RRC_SIB_TEST_SIB8_SYS_TIME_TYPE_ASYNC);

  lte_rrc_sib_test_periodic_sib8_si_update(FALSE);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibPeriodicSib8SiUpdate::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
SIB8 timer expires when SIB8 is no longer being transmitted
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibPeriodicSib8Removed);

void LteRrcSibPeriodicSib8Removed::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibPeriodicSib8Removed::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_cell1_with_sib8(LTE_RRC_SIB_TEST_SIB8_SYS_TIME_TYPE_ASYNC);

  lte_rrc_sib_test_periodic_sib8_si_update(TRUE);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibPeriodicSib8Removed::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
SIB8 timer expires when event for conn to idle is active - SIB8 should be read 
and timer should be started
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibPeriodicSib8ConnToIdleTest);

void LteRrcSibPeriodicSib8ConnToIdleTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibPeriodicSib8ConnToIdleTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_cell1_with_sib8(LTE_RRC_SIB_TEST_SIB8_SYS_TIME_TYPE_ASYNC);

  lte_rrc_sib_test_periodic_sib8_conn_to_idle();

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibPeriodicSib8ConnToIdleTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
SIB8 is received before SIB1. The SIB should be discarded
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSib8RcvdBeforeSib1Test);

void LteRrcSibSib8RcvdBeforeSib1Test::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSib8RcvdBeforeSib1Test::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* SIB8 is received before SIB1. The SIB should be discarded */
  lte_rrc_sib_test_sib8_rcvd_before_sib1();

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibSib8RcvdBeforeSib1Test::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
SIB8 timer expires when UE reselect to another cell and SIB8 is no longer 
being transmitted
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibPeriodicReselToAnotherCellSib8Removed);

void LteRrcSibPeriodicReselToAnotherCellSib8Removed::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibPeriodicReselToAnotherCellSib8Removed::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_cell1_with_sib8(LTE_RRC_SIB_TEST_SIB8_SYS_TIME_TYPE_ASYNC);

  lte_rrc_sib_test_resel_to_cell2(FALSE);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibPeriodicReselToAnotherCellSib8Removed::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
SIB8 timer expires when UE reselect to another cell and SIB8 is still present
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibPeriodicReselToAnotherCellSib8Present);

void LteRrcSibPeriodicReselToAnotherCellSib8Present::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibPeriodicReselToAnotherCellSib8Present::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_cell1_with_sib8(LTE_RRC_SIB_TEST_SIB8_SYS_TIME_TYPE_ASYNC);

  lte_rrc_sib_test_resel_to_cell2(TRUE);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibPeriodicReselToAnotherCellSib8Present::Teardown()
{
  lte_rrc_utf_teardown();
}

/* SIB8 async timer expires while suspended, SIB8 reading resumed after IRAT resel failure */
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibAsyncSIB8AfterReselFailureTest1);

void LteRrcSibAsyncSIB8AfterReselFailureTest1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibAsyncSIB8AfterReselFailureTest1::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_sib_updated_indi_s *sib_updated_ind_ptr;
  lte_cphy_cell_resel_ind_s resel_ind;
  lte_errno_e status;
  uint32 buf_len;
  lte_rrc_tmri_s *tmr_ptr;
  lte_cphy_irat_cdma_system_time_chg_req_s *sys_time_chg_req_ptr;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_cell1_with_sib8(LTE_RRC_SIB_TEST_SIB8_SYS_TIME_TYPE_ASYNC);

  //lte_rrc_sib_test_periodic_sib8_async();
  // ---------------------------------------------
  TF_SLEEP(LTE_RRC_SIB_PERIODIC_SIB8_TMR_DURATION - 240);
  lte_rrc_utf_check_no_more_msgs();
  TF_SLEEP(1);

  // Prepare cell_resel_ind to send to RRC
  lte_rrc_init_default_hdr(&resel_ind.msgr_hdr, LTE_CPHY_CELL_RESEL_IND);
  resel_ind.serv_cell_info.dl_cell_frequency = FREQ_1;
  resel_ind.serv_cell_info.phy_cell_id = PHY_CELL_ID_1;
  resel_ind.target_cell_info.rat_type = LTE_RAT_EUTRAN;
  resel_ind.target_cell_info.rat_cell_info.lte_cell_info.dl_cell_frequency = FREQ_1;
  resel_ind.target_cell_info.rat_cell_info.lte_cell_info.phy_cell_id = PHY_CELL_ID_2;
  resel_ind.csg_resel = FALSE;
  resel_ind.sfn.curr_sfn = 70;

  // Send cell_resel_ind to RRC
  lte_rrc_utf_send_msg((byte*)&resel_ind, sizeof(resel_ind));
  
  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0x3);

  TF_SLEEP(320);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_PERIODIC_SIB8_TMRI);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));

  lte_set_current_time(time_since_power_up_ms += 400); 

  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_RESEL_FAIL, FALSE, 
                                      LTE_RRC_NULL_SFN);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0x102);

  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_8_mcc_8_5,
                                        sizeof(sib1_2345_8_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        110);

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        LTE_RRC_SIB5_BITMASK,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB8_BITMASK));

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0x100);

  // send SIB8 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib8_async_8_5,
                                        sizeof(sib8_async_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        112);

  //---------------------------------------------------------------

  // We expect a sys_time_chg_req from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sys_time_chg_req_ptr, &buf_len);

  TF_ASSERT(sys_time_chg_req_ptr->msgr_hdr.id == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ);
  TF_ASSERT(sys_time_chg_req_ptr->cell_info.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sys_time_chg_req_ptr->cell_info.dl_cell_frequency == FREQ_1);
  TF_ASSERT(sys_time_chg_req_ptr->enabled == TRUE);
  TF_ASSERT(sys_time_chg_req_ptr->source_type == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_SOURCE_SIB8);
  TF_ASSERT(lte_rrc_tmr_is_running(&lte_rrc_sib.dd_ptr->periodic_sib8_tmr) == TRUE);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);

  lte_rrc_utf_get_next_int_msg((byte**)&sib_updated_ind_ptr, &buf_len);
  TF_ASSERT(sib_updated_ind_ptr->msg_hdr.id == LTE_RRC_SIB_UPDATED_INDI);
  
  // ----------------------------------------------------------------------

  TF_SLEEP(100);

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibAsyncSIB8AfterReselFailureTest1::Teardown()
{
  lte_rrc_utf_teardown();
}

/* SIB8 async timer is running, SIB8 is not read again after IRAT/Intra-LTE resel failure */
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibAsyncSIB8AfterReselFailureTest2);

void LteRrcSibAsyncSIB8AfterReselFailureTest2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibAsyncSIB8AfterReselFailureTest2::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_sib_updated_indi_s *sib_updated_ind_ptr;
  lte_cphy_cell_resel_ind_s resel_ind;
  lte_errno_e status;
  uint32 buf_len;
  lte_rrc_tmri_s *tmr_ptr;
  lte_cphy_irat_cdma_system_time_chg_req_s *sys_time_chg_req_ptr;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_cell1_with_sib8(LTE_RRC_SIB_TEST_SIB8_SYS_TIME_TYPE_ASYNC);

  //lte_rrc_sib_test_periodic_sib8_async();
  // ---------------------------------------------
  TF_SLEEP(500);
  lte_rrc_utf_check_no_more_msgs();

  // Prepare cell_resel_ind to send to RRC
  lte_rrc_init_default_hdr(&resel_ind.msgr_hdr, LTE_CPHY_CELL_RESEL_IND);
  resel_ind.serv_cell_info.dl_cell_frequency = FREQ_1;
  resel_ind.serv_cell_info.phy_cell_id = PHY_CELL_ID_1;
  resel_ind.target_cell_info.rat_type = LTE_RAT_EUTRAN;
  resel_ind.target_cell_info.rat_cell_info.lte_cell_info.dl_cell_frequency = FREQ_1;
  resel_ind.target_cell_info.rat_cell_info.lte_cell_info.phy_cell_id = PHY_CELL_ID_2;
  resel_ind.csg_resel = FALSE;
  resel_ind.sfn.curr_sfn = 70;

  // Send cell_resel_ind to RRC
  lte_rrc_utf_send_msg((byte*)&resel_ind, sizeof(resel_ind));
  
  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0x3);

  TF_SLEEP(320);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));

  lte_set_current_time(time_since_power_up_ms += 400); 

  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_RESEL_FAIL, FALSE, 
                                      LTE_RRC_NULL_SFN);

  lte_rrc_utf_get_next_int_msg((byte**)&sib_updated_ind_ptr, &buf_len);
  TF_ASSERT(sib_updated_ind_ptr->msg_hdr.id == LTE_RRC_SIB_UPDATED_INDI);
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0x0);

  // ----------------------------------------------------------------------

  TF_SLEEP(100);

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}


void LteRrcSibAsyncSIB8AfterReselFailureTest2::Teardown()
{
  lte_rrc_utf_teardown();
}

