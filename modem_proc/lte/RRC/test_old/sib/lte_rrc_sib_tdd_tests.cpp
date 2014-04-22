/*!
  @file lte_rrc_sib_tdd_tests.cpp

  @brief
  TDD related test cases
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_tdd_tests.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/29/10   amit    Initial Version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "__lte_rrc_sib.h"
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

DECLARE_SIB1_2345_MCC_8_5;
DECLARE_SIB1_2345_TDD_MCC_8_5;
DECLARE_SIB1_2345_TDD_INVALID_MCC_8_5;
DECLARE_SIB1_2345_NEW_TDD_MCC_8_5;
DECLARE_SIB2_SIB3_8_5;
DECLARE_SIB4_SIB5_8_5;

extern "C"
{
extern lte_rrc_sib_s lte_rrc_sib;
}

extern boolean lte_rrc_sib_test_camped;
extern int lte_rrc_sib_test_sib_db_key;
extern lte_phy_cell_id_t lte_rrc_sib_test_serv_cell_phy_cell_id;
extern lte_earfcn_t lte_rrc_sib_test_serv_cell_freq;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/* enum used to indicate what conditions apply */
typedef enum
{
  LTE_RRC_SIB_TEST_TDD_NEW_CFG,
  LTE_RRC_SIB_TEST_TDD_FAIL_EVENT_ACTIVE,
  LTE_RRC_SIB_TEST_TDD_FAIL_EVENT_NOT_ACTIVE,
  LTE_RRC_SIB_TEST_TDD_INVALID
} lte_rrc_sib_test_tdd_e;

/* Camp on cell1 that is TDD */
void lte_rrc_sib_test_tdd_camp(lte_rrc_sib_test_tdd_e test)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_cphy_tdd_cfg_req_s *tdd_cfg_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_sib_updated_indi_s *sib_updated_ind_ptr;
  uint32 buf_len;

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_TDD_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_TDD_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_CPHY_NULL_SFN);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);

  // ---------------------------------------------

  // send MIB to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_TDD_1,
                            1);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_TDD_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);

  // ----------------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_tdd_mcc_8_5,
                                        sizeof(sib1_2345_tdd_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_TDD_1,
                                        2);

  //---------------------------------------------------------------

  // We expect a tdd_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&tdd_cfg_req_ptr, &buf_len);
  TF_ASSERT(tdd_cfg_req_ptr->msgr_hdr.id == LTE_CPHY_TDD_CFG_REQ);

  TF_ASSERT(tdd_cfg_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(tdd_cfg_req_ptr->freq == FREQ_TDD_1);
  TF_ASSERT(tdd_cfg_req_ptr->cfg.subframe_assign == LTE_L1_TDD_UL_DL_CFG_2);
  TF_ASSERT(tdd_cfg_req_ptr->cfg.special_sub_fn_patterns == LTE_L1_TDD_SPECIAL_SUBFRAME_PATTERN_2);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_TDD_1);
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

  // ----------------------------------------------------------------------

  if(test == LTE_RRC_SIB_TEST_TDD_FAIL_EVENT_ACTIVE)
  {
    //tdd_cfg_cnf with failure - SIB read error should be sent and sib reading stopped
    lte_rrc_sib_test_send_tdd_cfg_cnf(PHY_CELL_ID_1,
                                      FREQ_TDD_1,
                                      E_FAILURE);
  
    // ----------------------------------------------------------------------
  
    //SIB read error
    lte_rrc_sib_test_rcv_sib_read_error_indi(PHY_CELL_ID_1,
                                             FREQ_TDD_1,
                                             LTE_RRC_SIB_READ_ERROR_CAUSE_INVALID_SIBS,
                                             LTE_RRC_SIB_MIB_SIB1_BITMASK,
                                             TRUE,
                                             TRUE);
  
    // ----------------------------------------------------------------------
  
    // We expect a sib_sched_req message from RRC to stop SIB reading
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_TDD_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
  
    // ----------------------------------------------------------------------
  
    // Asserts
    {
      /* Asserts on SIB internal data */
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == FALSE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
      TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == LTE_RRC_SIB_DB_MAX_CELLS);
      for(int i = 0; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
      {
        TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
      }
      TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);
  
      // ----------------------------------------------------------------------
  
      /* Asserts on active Events */
  
      // ----------------------------------------------------------------------
  
      /* Asserts on valid cells */
  
      // ----------------------------------------------------------------------
  
    } // Asserts

    return;
  } // test == LTE_RRC_SIB_TEST_TDD_FAIL_EVENT_ACTIVE
  else
  {
    //tdd_cfg_cnf with success - should be ignored
    lte_rrc_sib_test_send_tdd_cfg_cnf(PHY_CELL_ID_1,
                                      FREQ_TDD_1,
                                      E_SUCCESS);
  
    lte_rrc_utf_check_no_more_msgs();
  }

  // ----------------------------------------------------------------------

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_TDD_1,
                                        3);

  //---------------------------------------------------------------

  // this will make CSP return this cell as serving cell, though camped_indi as 
  // not been sent for the cell
  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;
  lte_rrc_sib_test_serv_cell_phy_cell_id = PHY_CELL_ID_1;
  lte_rrc_sib_test_serv_cell_freq = FREQ_TDD_1;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_TDD_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_TDD_1);
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

  //---------------------------------------------------------------

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_TDD_1,
                                        4);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_TDD_1);
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

  //---------------------------------------------------------------

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_TDD_1);
  
  //---------------------------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&sib_updated_ind_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_updated_indi_s));
  TF_ASSERT(sib_updated_ind_ptr->sibs_changed_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                          LTE_RRC_SIB4_BITMASK | 
                                                          LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_TDD_1);
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

  //---------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_TDD_1,
                            64);
  
  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_TDD_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  if(test == LTE_RRC_SIB_TEST_TDD_NEW_CFG)
  {
    // send SIB1 with new tdd cfg to RRC
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_new_tdd_mcc_8_5,
                                          sizeof(sib1_2345_new_tdd_mcc_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_TDD_1,
                                          65);

    // We expect a tdd_cfg_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&tdd_cfg_req_ptr, &buf_len);
    TF_ASSERT(tdd_cfg_req_ptr->msgr_hdr.id == LTE_CPHY_TDD_CFG_REQ);
  
    TF_ASSERT(tdd_cfg_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(tdd_cfg_req_ptr->freq == FREQ_TDD_1);
    TF_ASSERT(tdd_cfg_req_ptr->cfg.subframe_assign == LTE_L1_TDD_UL_DL_CFG_3);
    TF_ASSERT(tdd_cfg_req_ptr->cfg.special_sub_fn_patterns == LTE_L1_TDD_SPECIAL_SUBFRAME_PATTERN_3);

    // Since SIB1 has changed, sib_updated_indi should be sent for MIB and SIB1
    lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                          FREQ_TDD_1,
                                          LTE_RRC_SIB_MIB_SIB1_BITMASK,
                                          (LTE_RRC_MIB_BITMASK | 
                                           LTE_RRC_SIB1_BITMASK |
                                           LTE_RRC_SIB2_BITMASK |
                                           LTE_RRC_SIB3_BITMASK |
                                           LTE_RRC_SIB4_BITMASK |
                                           LTE_RRC_SIB5_BITMASK));
  }
  else
  {
    // send SIB1 to RRC
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_tdd_mcc_8_5,
                                          sizeof(sib1_2345_tdd_mcc_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_TDD_1,
                                          65);
  }
  
  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_TDD_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  // Asserts
  {
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_TDD_1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 2);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
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
   // TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
   // TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
  
    // ----------------------------------------------------------------------

  }

  // ----------------------------------------------------------------------

  if(test == LTE_RRC_SIB_TEST_TDD_FAIL_EVENT_NOT_ACTIVE)
  {
    //tdd_cfg_cnf with failure - SIB read error should be sent
    lte_rrc_sib_test_send_tdd_cfg_cnf(PHY_CELL_ID_1,
                                      FREQ_TDD_1,
                                      E_FAILURE);

    // ----------------------------------------------------------------------

    //SIB read error
    lte_rrc_sib_test_rcv_sib_read_error_indi(PHY_CELL_ID_1,
                                             FREQ_TDD_1,
                                             LTE_RRC_SIB_READ_ERROR_CAUSE_INVALID_SIBS,
                                             (LTE_RRC_SIB_MIB_SIB1_BITMASK |
                                              LTE_RRC_SIB2_BITMASK |
                                              LTE_RRC_SIB3_BITMASK |
                                              LTE_RRC_SIB4_BITMASK |
                                              LTE_RRC_SIB5_BITMASK),
                                             TRUE,
                                             TRUE);

    // ----------------------------------------------------------------------
  }
}

/* SIB1 invalid - Cell is TDD but SIB1 does not have TDD info. SIB read error 
should be sent to CSP */
void lte_rrc_sib_test_tdd_sib1_invalid()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_TDD_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_TDD_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_CPHY_NULL_SFN);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);

  // ---------------------------------------------

  // send MIB to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_TDD_1,
                            1);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_TDD_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);

  // ----------------------------------------------------------------------

  // send SIB1 that does not have tdd info to RRC - SIB read error should be sent
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_tdd_invalid_mcc_8_5,
                                        sizeof(sib1_2345_tdd_invalid_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_TDD_1,
                                        2);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_read_error_indi(PHY_CELL_ID_1,
                                           FREQ_TDD_1,
                                           LTE_RRC_SIB_READ_ERROR_CAUSE_INVALID_SIBS,
                                           LTE_RRC_MIB_BITMASK,
                                           FALSE,
                                           TRUE);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_TDD_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);

}

/*===========================================================================

                                UNIT TEST
Camp on a TDD cell - tdd_cfg_req should be sent on reading SIB1

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibTddCampTest);

void LteRrcSibTddCampTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_TDD_CFG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibTddCampTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_tdd_camp(LTE_RRC_SIB_TEST_TDD_INVALID);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibTddCampTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
SIB1 received with new TDD config, new req should be sent

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibTddNewCfgTest);

void LteRrcSibTddNewCfgTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_TDD_CFG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibTddNewCfgTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_tdd_camp(LTE_RRC_SIB_TEST_TDD_NEW_CFG);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibTddNewCfgTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
CFG cnf received with failure when event is active - sib read error should be sent

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibTddFailureTest1);

void LteRrcSibTddFailureTest1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_TDD_CFG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibTddFailureTest1::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_tdd_camp(LTE_RRC_SIB_TEST_TDD_FAIL_EVENT_ACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibTddFailureTest1::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
CFG cnf received with failure when event is not active and it is for serving 
cell - sib read error should be sent

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibTddFailureTest2);

void LteRrcSibTddFailureTest2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_TDD_CFG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibTddFailureTest2::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_tdd_camp(LTE_RRC_SIB_TEST_TDD_FAIL_EVENT_NOT_ACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibTddFailureTest2::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
SIB1 invalid - Cell is TDD but SIB1 does not have TDD info. SIB read error 
should be sent to CSP

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibTddSib1InvalidTest);

void LteRrcSibTddSib1InvalidTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_CPHY_TDD_CFG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibTddSib1InvalidTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_tdd_sib1_invalid();

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibTddSib1InvalidTest::Teardown()
{
  lte_rrc_utf_teardown();
}



