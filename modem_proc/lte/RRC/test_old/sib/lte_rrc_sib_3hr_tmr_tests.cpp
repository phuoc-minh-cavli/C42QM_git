/*!
  @file lte_rrc_sib_3hr_tmr_tests.cpp

  @brief
  3hr timer related unit test cases
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_3hr_tmr_tests.cpp#1 $

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
#include "lte_rrc_sibi.h"
#include "__lte_rrc_controller.h"
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
DECLARE_SIB1_2345_MCC_8_5_CELL2;
DECLARE_SIB1_2345_MCC_8_5_CELL2_VALUE_TAG_MATCH_SIB1_DIFFER;
DECLARE_SIB1_2345_MCC_8_5_CELL2_VALUE_TAG_CHANGES;
DECLARE_SIB2_SIB3_8_5;
DECLARE_SIB4_SIB5_8_5;

extern "C"
{
extern lte_rrc_sib_s lte_rrc_sib;
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern boolean lte_rrc_sib_test_camped;
extern boolean lte_rrc_sib_test_wcdma_supported;
extern boolean lte_rrc_sib_test_gsm_supported;
extern boolean lte_rrc_sib_test_cdma_1x_supported;
extern boolean lte_rrc_sib_test_cdma_do_supported;
extern boolean lte_rrc_sib_test_ho_in_progress;
extern stm_state_t lte_rrc_sib_test_controller_state;
extern lte_rrc_revoke_priority_e lte_rrc_sib_test_revoke_priority;
extern void lte_rrc_sib_test_resel(int cell_index, boolean send_sched_req_for_old_cell);
extern void lte_rrc_sib_test_ho(int cell_index);
extern void lte_rrc_sib_test_resel1(boolean resel_from_basic_cell1);

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/* sib1_changes indicates if SIB1 changes when read after 3 hours. If it is FALSE, 
   value_tag_changes is a no-op. If it is TRUE, value_tag_changes indicates if 
   value tag has been changed */
void lte_rrc_sib_test_3hr_tmr(boolean sib1_changes, boolean value_tag_changes, 
                              boolean controller_suspended, boolean higher_priority)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_get_sibs_reqi_s *get_sibs_reqi_ptr;
  uint32 buf_len;

  if(controller_suspended)
  {
    lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__SUSPENDED;
    if(!higher_priority)
    {
      lte_rrc_sib_test_revoke_priority = LTE_RRC_REVOKE_PRIORITY_NOT_REQD;
    }
    else
    {
      lte_rrc_sib_test_revoke_priority = LTE_RRC_REVOKE_PRIORITY_NOW;
    }
  }

  TF_SLEEP(LTE_RRC_SIB_CELL_INFO_AGE_3HR * LTE_RRC_SIB_3HR_TMR_TICK_VALUE);

  for(int i = 1; i <= LTE_RRC_SIB_CELL_INFO_AGE_3HR; i++)
  {
    // We expect 3hr timer ticks
    lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
    TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_3HR_CLOCK_TICK_TMRI);
  }

  // If controller is suspended, the last timer timer expiry should be added to 
  // pending queue
  if(controller_suspended)
  {
    if(higher_priority)
    {
      lte_rrc_sib_test_rcv_revoke_reqi();
    }

    // Since timer expiry is pended, no msg/req should be sent
    lte_rrc_utf_check_no_more_msgs();

    lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;

    // Send a message that SIB does not process
    lte_rrc_sib_test_send_service_ind();

    // get_sibs_reqi should be dequeued
    lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_reqi_ptr, &buf_len);
    TF_ASSERT(get_sibs_reqi_ptr->msg_hdr.id == LTE_RRC_GET_SIBS_REQI);
  }

  // -------------------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_RRC_NULL_SFN);

  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ---------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_valid == TRUE);
  for(int i = 2; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_SIB);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_3HR_TMR);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 1);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5_cell2));
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
                                                                       LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                 LTE_RRC_SIB1_BITMASK |
                                                                 LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == 0);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == LTE_RRC_SIB_MIB_SIB1_BITMASK);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == LTE_RRC_NULL_SFN);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                        LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_counter == 18);
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
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[0] != NULL);
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

  // ----------------------------------------------------------------------

  /* MIB */
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            1);

  // ----------------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_valid == TRUE);
  for(int i = 2; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_SIB);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_3HR_TMR);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 1);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5_cell2));
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == 1);
  for(int i = 1; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == LTE_RRC_SIB_MIB_SIB1_WT_TMR);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                       LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                 LTE_RRC_SIB1_BITMASK |
                                                                 LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == LTE_RRC_MIB_BITMASK);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == LTE_RRC_SIB_MIB_SIB1_BITMASK);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                        LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_counter == 18);
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
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[0] != NULL);
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

  // ----------------------------------------------------------------------

  if(!sib1_changes) /* SIB1 does not change */
  {
    /* SIB1 */
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_cell2,
                                          sizeof(sib1_2345_mcc_8_5_cell2),
                                          PHY_CELL_ID_2,
                                          FREQ_2,
                                          2);
  
    // ----------------------------------------------------------------------
  
    // We expect a sib_sched_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  
    TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  
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
  
    /* Asserts on SIB internal data */
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
    TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == FALSE);
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[0] != NULL);
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
  } /* SIB1 does not change */
  else /* SIB1 changes */
  {
    if(!value_tag_changes) /* Value tag does not change */
    {
      /* SIB1 */
      lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_cell2_value_tag_match_sib1_differ,
                                            sizeof(sib1_2345_mcc_8_5_cell2_value_tag_match_sib1_differ),
                                            PHY_CELL_ID_2,
                                            FREQ_2,
                                            2);
    
      // ----------------------------------------------------------------------

      lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_2,
                                            FREQ_2,
                                            LTE_RRC_SIB_MIB_SIB1_BITMASK,
                                            (LTE_RRC_MIB_BITMASK |
                                             LTE_RRC_SIB1_BITMASK |
                                             LTE_RRC_SIB2_BITMASK |
                                             LTE_RRC_SIB3_BITMASK |
                                             LTE_RRC_SIB4_BITMASK |
                                             LTE_RRC_SIB5_BITMASK));

      // ----------------------------------------------------------------------

      // We expect a sib_sched_req message from RRC
      lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    
      TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
      TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
    
      TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
      TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
    
      TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
    
      TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);
    
      TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_5);
      TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
      TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
      TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                      LTE_RRC_SIB3_BITMASK));
      TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
      TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                      LTE_RRC_SIB5_BITMASK));
    
      // ----------------------------------------------------------------------
    
      /* Asserts on SIB internal data */
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == FALSE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
      TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 1);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == FALSE);
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[0] != NULL);
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
    } /* Value tag does not change */
    else /* Value tag changes */
    {
      /* SIB1 */
      lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_cell2_value_tag_changes,
                                            sizeof(sib1_2345_mcc_8_5_cell2_value_tag_changes),
                                            PHY_CELL_ID_2,
                                            FREQ_2,
                                            2);
  
      // ----------------------------------------------------------------------
  
      // We expect a sib_sched_req message from RRC
      lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  
      TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
      TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
      TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
      TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  
      TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                             LTE_RRC_SIB3_BITMASK |
                                                             LTE_RRC_SIB4_BITMASK |
                                                             LTE_RRC_SIB5_BITMASK));
  
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
  
      // ----------------------------------------------------------------------
  
      //Asserts
      {
        /* Asserts on SIB internal data */
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
        TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 1);
        TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == FALSE);
        TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_valid == TRUE);
        for(int i = 2; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
        {
          TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
        }
        TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);
    
        // ----------------------------------------------------------------------
    
        /* Asserts on active Events */
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_2);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_2);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_SIB);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 1);
  
        // ----------------------------------------------------------------------
  
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_100);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5_cell2_value_tag_changes));
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
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == 1);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[1] == 2);
        for(int i = 2; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
        {
          TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
        }
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == LTE_RRC_SIB_SIB2_WT_TMR_FACTOR * 160);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                             LTE_RRC_SIB1_BITMASK |
                                                                             LTE_RRC_SIB2_BITMASK |
                                                                             LTE_RRC_SIB3_BITMASK |
                                                                             LTE_RRC_SIB4_BITMASK |
                                                                             LTE_RRC_SIB5_BITMASK));
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                       LTE_RRC_SIB1_BITMASK));
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                            LTE_RRC_SIB1_BITMASK));
  
        // ----------------------------------------------------------------------
  
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == FALSE);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                               LTE_RRC_SIB1_BITMASK |
                                                                               LTE_RRC_SIB2_BITMASK |
                                                                               LTE_RRC_SIB3_BITMASK |
                                                                               LTE_RRC_SIB4_BITMASK |
                                                                               LTE_RRC_SIB5_BITMASK));
  
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);
  
        // ----------------------------------------------------------------------
  
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_2);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_2);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                                                     LTE_RRC_SIB3_BITMASK |
                                                                                     LTE_RRC_SIB4_BITMASK |
                                                                                     LTE_RRC_SIB5_BITMASK));
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                     LTE_RRC_SIB1_BITMASK |
                                                                                     LTE_RRC_SIB2_BITMASK |
                                                                                     LTE_RRC_SIB3_BITMASK |
                                                                                     LTE_RRC_SIB4_BITMASK |
                                                                                     LTE_RRC_SIB5_BITMASK));
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 2);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 2);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                              LTE_RRC_SIB3_BITMASK));
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                              LTE_RRC_SIB5_BITMASK));
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_SIB2);
    
        // ----------------------------------------------------------------------
    
        /* Asserts on valid cells */
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
        TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].value_tag == 4);
        TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].mib.num_tx_antennas == 1);
        TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].mib.dl_bandwidth == LTE_BW_NRB_100);
        TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
        TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
        TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5_cell2_value_tag_changes));
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
        TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].all_si.decoded_si_ptr[0] != NULL);
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

      // ----------------------------------------------------------------------

      // send SIB2_3 to RRC
      lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                            sizeof(sib2_sib3_8_5),
                                            PHY_CELL_ID_2,
                                            FREQ_2,
                                            3);
    
      //---------------------------------------------------------------
    
      lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_2,
                                            FREQ_2,
                                            LTE_RRC_SIB_MANDATORY_SIBS_BITMASK,
                                            (LTE_RRC_MIB_BITMASK |
                                             LTE_RRC_SIB1_BITMASK |
                                             LTE_RRC_SIB2_BITMASK |
                                             LTE_RRC_SIB3_BITMASK |
                                             LTE_RRC_SIB4_BITMASK |
                                             LTE_RRC_SIB5_BITMASK));
    
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

      // ----------------------------------------------------------------------

      lte_rrc_sib_test_send_mib(1,
                                LTE_BW_NRB_100,
                                LTE_L1_PHICH_DURATION_NORMAL,
                                LTE_L1_PHICH_RESOURCE_ONE,
                                PHY_CELL_ID_2,
                                FREQ_2,
                                64);

      // ---------------------------------------------

      // We expect a sib_sched_req message from RRC
      lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
      TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

      TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
      TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
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


      // ----------------------------------------------------------------------

      // send SIB1 to RRC
      lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_cell2_value_tag_changes,
                                            sizeof(sib1_2345_mcc_8_5_cell2_value_tag_changes),
                                            PHY_CELL_ID_2,
                                            FREQ_2,
                                            65);
    
      // ---------------------------------------------
    
      // We expect a sib_sched_req message from RRC
      lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
      TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
    
      TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
      TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
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

    } /* Value tag changes*/
  } /* SIB1 changes */
  
  // ----------------------------------------------------------------------
}

/* 3hr timer expiry when reselection is in progress */
void lte_rrc_sib_test_3hr_tmr_resel_in_progress()
{
  lte_rrc_tmri_s *tmr_ptr;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  TF_SLEEP((LTE_RRC_SIB_CELL_INFO_AGE_3HR * LTE_RRC_SIB_3HR_TMR_TICK_VALUE) - 1);

  for(int i = 0; i < LTE_RRC_SIB_CELL_INFO_AGE_3HR - 1; i++)
  {
    lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  }

  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_sib_test_send_abort_reqi(23, FREQ_1);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_CPHY_NULL_SFN);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
  //                                                                LTE_RRC_SIB3_BITMASK));
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
  //                                                                LTE_RRC_SIB5_BITMASK));

  /* SIB1 */
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_SLEEP(1);
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);

  /* Age of both cells reaches 18, but since event is active for cell1 no action 
     is taken for it. And since reselection is active no action is taken for the 
     serving cell */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_counter == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[2].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[2].index_counter == LTE_RRC_SIB_CELL_INFO_AGE_3HR);

  /* The third cell will be removed from the db */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_valid == FALSE);

  lte_rrc_utf_check_no_more_msgs();

}

/* 3hr timer expiry when reselection is in progress */
void lte_rrc_sib_test_3hr_tmr_resel_in_progress2()
{
  lte_rrc_tmri_s *tmr_ptr;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  TF_SLEEP((LTE_RRC_SIB_CELL_INFO_AGE_3HR * LTE_RRC_SIB_3HR_TMR_TICK_VALUE) - 1);

  for(int i = 0; i < LTE_RRC_SIB_CELL_INFO_AGE_3HR - 1; i++)
  {
    lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  }

  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_sib_test_send_abort_reqi(23, FREQ_1);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_CPHY_NULL_SFN);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
  //                                                                LTE_RRC_SIB3_BITMASK));
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
  //                                                                LTE_RRC_SIB5_BITMASK));

  TF_SLEEP(1);
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);

  /* Age of both cells reaches 18, but since event is active for cell1 no action 
     is taken for it. And since reselection is active no action is taken for the 
     serving cell */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[2].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[2].index_counter == LTE_RRC_SIB_CELL_INFO_AGE_3HR);

  /* The third cell will be removed from the db */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == FALSE)
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_valid == FALSE);

  /* SIB1 */
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK | 
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

  lte_rrc_utf_check_no_more_msgs();

}

/* 3hr timer expiry when HO is in progress */
void lte_rrc_sib_test_3hr_tmr_ho_in_progress()
{
  lte_rrc_tmri_s *tmr_ptr;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  TF_SLEEP((LTE_RRC_SIB_CELL_INFO_AGE_3HR * LTE_RRC_SIB_3HR_TMR_TICK_VALUE) - 1);

  for(int i = 0; i < LTE_RRC_SIB_CELL_INFO_AGE_3HR - 1; i++)
  {
    lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  }

  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_sib_test_send_ho_started_indi();

  lte_rrc_sib_test_ho_in_progress = FALSE;

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_CPHY_NULL_SFN);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
  //                                                                LTE_RRC_SIB3_BITMASK));
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
  //                                                                LTE_RRC_SIB5_BITMASK));

  /* SIB1 */
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_SLEEP(1);
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);

  /* Age of both cells reaches 18, but since event is active for cell1 no action 
     is taken for it. And since reselection is active no action is taken for the 
     serving cell */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_counter == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[2].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[2].index_counter == LTE_RRC_SIB_CELL_INFO_AGE_3HR);

  /* The third cell will be removed from the db */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_valid == FALSE);

  lte_rrc_utf_check_no_more_msgs();

}

/* Notion of serving cell is different for SIB and CSP */
void lte_rrc_sib_test_3hr_tmr_sib_csp_diff_serv_cell()
{
  lte_rrc_tmri_s *tmr_ptr;
  uint32 buf_len;

  /* UE goes not camped, SIB is not informed about it */
  lte_rrc_sib_test_camped = FALSE;

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);

  TF_SLEEP(LTE_RRC_SIB_CELL_INFO_AGE_3HR * LTE_RRC_SIB_3HR_TMR_TICK_VALUE);

  for(int i = 0; i < LTE_RRC_SIB_CELL_INFO_AGE_3HR; i++)
  {
    lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  }

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
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_counter == 18);
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
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();


}


/*===========================================================================

                                UNIT TEST
3hr timer expiry when reselection is in progress

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSib3HrTmrReselInProgressTest);

void LteRrcSib3HrTmrReselInProgressTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSib3HrTmrReselInProgressTest::Test()
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

  /* Add 2 more cells to the db */
  lte_rrc_sib_test_resel(1, FALSE);
  lte_rrc_sib_test_resel(2, FALSE);

  /* 3hr timer expiry when reselection is in progress */
  lte_rrc_sib_test_3hr_tmr_resel_in_progress();
  
  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSib3HrTmrReselInProgressTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
3hr timer expiry when reselection is in progress, timer expiry before receiving 
sib1 

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSib3HrTmrReselInProgressTest2);

void LteRrcSib3HrTmrReselInProgressTest2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSib3HrTmrReselInProgressTest2::Test()
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

  /* Add 2 more cells to the db */
  lte_rrc_sib_test_resel(1, FALSE);
  lte_rrc_sib_test_resel(2, FALSE);

  /* 3hr timer expiry when reselection is in progress */
  lte_rrc_sib_test_3hr_tmr_resel_in_progress2();
  
  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSib3HrTmrReselInProgressTest2::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Notion of serving cell is different for SIB and CSP

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSib3HrTmrSibCspDiffServCellTest);

void LteRrcSib3HrTmrSibCspDiffServCellTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSib3HrTmrSibCspDiffServCellTest::Test()
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

  /* 3hr timer expiry when csp and sib have diff notion of serving cell */
  lte_rrc_sib_test_3hr_tmr_sib_csp_diff_serv_cell();
  
  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSib3HrTmrSibCspDiffServCellTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
3hr timer expiry when HO is in progress

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSib3HrTmrHOInProgressTest);

void LteRrcSib3HrTmrHOInProgressTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSib3HrTmrHOInProgressTest::Test()
{
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

  /* connected_indi */
  lte_rrc_sib_test_send_connected_indi();

  /* Add 2 more cells to the db */
  lte_rrc_sib_test_ho(1);
  lte_rrc_sib_test_ho(2);

  /* 3hr timer expiry when HO is in progress */
  lte_rrc_sib_test_3hr_tmr_ho_in_progress();
  
  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSib3HrTmrHOInProgressTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Value tag changes when SIB1 is read on 3hr timer expiry

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSib3HrTmrValueTagChangeTest);

void LteRrcSib3HrTmrValueTagChangeTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSib3HrTmrValueTagChangeTest::Test()
{
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

  /* Reselection */
  lte_rrc_sib_test_resel1(TRUE);

  // ----------------------------------------------------------------------

  /* 3hr timer */
  lte_rrc_sib_test_3hr_tmr(TRUE, TRUE, FALSE, FALSE);
  
  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSib3HrTmrValueTagChangeTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Value tag does not change but SIB1 changes on 3hr timer expiry - sib_updated_indi 
should be sent for MIB & SIB1

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSib3HrTmrSib1ChangeTest);

void LteRrcSib3HrTmrSib1ChangeTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSib3HrTmrSib1ChangeTest::Test()
{
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

  /* Reselection */
  lte_rrc_sib_test_resel1(TRUE);

  // ----------------------------------------------------------------------

  /* 3hr timer */
  lte_rrc_sib_test_3hr_tmr(TRUE, FALSE, FALSE, FALSE);
  
  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSib3HrTmrSib1ChangeTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Index counter inside sib db should be reset after receiving sib1 when counter 
is not exceed LTE_RRC_SIB_CELL_INFO_AGE.

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSib3HrTmrNotExpireRcvdSib1Test);

void LteRrcSib3HrTmrNotExpireRcvdSib1Test::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSib3HrTmrNotExpireRcvdSib1Test::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_tmri_s *tmr_ptr;
  
  uint32 buf_len;
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

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);

  // ----------------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            128);
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 128);

  // ----------------------------------------------------------------------

  /* Set index counter to be non-zero value but not exceed LTE_RRC_SIB_CELL_INFO_AGE */
  lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_counter = 5;

  
  //-----------------------------------------------------------------------
  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        130);

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_counter == 0);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 130);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 130);

  // ----------------------------------------------------------------------

  TF_SLEEP(LTE_RRC_SIB_CELL_INFO_AGE_3HR * LTE_RRC_SIB_3HR_TMR_TICK_VALUE);

  for(int i = 0; i < LTE_RRC_SIB_CELL_INFO_AGE_3HR; i++)
  {
    lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  }

  lte_rrc_utf_wait_for_done();

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_CPHY_NULL_SFN);

  // ----------------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            192);
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 256);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 192);

  // ----------------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        194);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 256);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 194);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSib3HrTmrNotExpireRcvdSib1Test::Teardown()
{
  lte_rrc_utf_teardown();
}
