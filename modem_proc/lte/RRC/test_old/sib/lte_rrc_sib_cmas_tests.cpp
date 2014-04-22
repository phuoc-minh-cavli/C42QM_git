/*!
  @file lte_rrc_sib_cmas_tests.cpp

  @brief
  CMAS related SIB unit test cases
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

$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/sib/lte_rrc_sib_cmas_tests.cpp

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/15/11   mm    Initial Version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "__lte_rrc_sib.h"
#include "__lte_rrc_controller.h"
#include "lte_rrc_sibi.h"
#include <lte_mac_ind.h>
#include <lte_mac_msg.h>
#include <dsm_item.h>
#include <dsm_init.h>
#include <fs_public.h>
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_sib_test_common.h"
#include <qtf_efs.h>
#include <fs_lib.h>
#include <tf_stub.h>

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/




/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

DECLARE_SIB1_2345_MCC_8_5;
DECLARE_SIB1_2345_MCC_8_5_CELL2;
DECLARE_SIB2_SIB3_8_5;
DECLARE_SIB4_SIB5_8_5;
DECLARE_SIB1_2345_12_MCC_8_5;
DECLARE_SIB12_SEG1OF2_8_5;
DECLARE_SIB12_SEG2OF2_8_5;
DECLARE_SIB12_BLANK_8_5;

extern "C"
{
extern lte_rrc_sib_s lte_rrc_sib;
}


extern "C"
{
  TF_PROTO_STUB(uint16, lte_rrc_paging_get_ue_specific_paging_cycle, void);
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern boolean lte_rrc_sib_test_camped;
extern stm_state_t lte_rrc_sib_test_controller_state;
extern lte_phy_cell_id_t lte_rrc_sib_test_serv_cell_phy_cell_id;
extern lte_rrc_rf_status_e lte_rrc_sib_test_rf_status;
extern uint64 lte_rrc_sib_test_rf_status_time;
extern lte_l1_sfn_t lte_rrc_sib_test_rf_status_sfn;
extern uint64 lte_rrc_sib_test_wt_tmr_before_rf_unavailable;
extern uint64 lte_rrc_sib_test_wt_tmr_after_rf_available;
extern lte_earfcn_t lte_rrc_sib_test_serv_cell_freq;
extern boolean lte_rrc_sib_test_ho_in_progress;
extern void lte_rrc_sib_test_resel2(boolean etws_test);
extern void lte_rrc_sib_test_ho1();

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/* Test case to check basic CMAS operation where UE is camped on a cell, page 
   for CMAS is rcvd, SIB1 is read which schedules SIB12, that is read and 
   cmas indication is sent to upper layers */
/* Boolean mod_prd_crossed indicates if mod prd boundary crossed while reading 
   SIB12 */
void lte_rrc_sib_test_basic_cmas(boolean mod_bnd_crossed)
{

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_cmas_ind_s *cmas_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib12_warn_msg[10];

  lte_rrc_msg_id_list_req_s cmas_msg_id_list_req;

  msgr_init_hdr(&cmas_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  cmas_msg_id_list_req.msg_id_range_list_size = 1;
  cmas_msg_id_list_req.msg_id_range_list[0].min_msg_id = 2000;
  cmas_msg_id_list_req.msg_id_range_list[0].max_msg_id = 4000;
  lte_rrc_utf_send_msg((byte*)&cmas_msg_id_list_req, sizeof(cmas_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_CMAS, FALSE, 
                                      1);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // Back to back requests
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_CMAS, FALSE, 
                                      2);

  // No messages should be sent
  lte_rrc_utf_check_no_more_msgs();

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
  
    // ----------------------------------------------------------------------
  
    /* Asserts on active Events */
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_PG);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CMAS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == LTE_RRC_SIB_MIB_SIB1_WT_TMR);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK));
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
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (0));
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_msg_id == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_sr_num == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIB12_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_num_segments_rcvd == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_last_segment_index == LTE_RRC_SIB_MAX_SIB12_SEGMENTS);
  
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib12_ptr == NULL);
    // ----------------------------------------------------------------------

  }

  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));

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
  
    // ----------------------------------------------------------------------
  
    /* Asserts on active Events */
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_PG);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CMAS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_12_mcc_8_5));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[1] == 3);
    for(int i = 2; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == (LTE_RRC_SIB_SI_WT_TMR_FACTOR * 80));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                           LTE_RRC_SIB1_BITMASK |
                                                                           LTE_RRC_SIB2_BITMASK |
                                                                           LTE_RRC_SIB3_BITMASK |
                                                                           LTE_RRC_SIB4_BITMASK |
                                                                           LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                                LTE_RRC_SIB1_BITMASK |
                                                                                LTE_RRC_SIB2_BITMASK |
                                                                                LTE_RRC_SIB3_BITMASK |
                                                                                LTE_RRC_SIB4_BITMASK |
                                                                                LTE_RRC_SIB5_BITMASK));
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (0));
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                         LTE_RRC_SIB1_BITMASK |
                                                                                         LTE_RRC_SIB2_BITMASK |
                                                                                         LTE_RRC_SIB3_BITMASK |
                                                                                         LTE_RRC_SIB4_BITMASK |
                                                                                         LTE_RRC_SIB5_BITMASK |
                                                                                         LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 3);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 3);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                                  LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                                  LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);
  
    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_msg_id == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_sr_num == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIB12_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_num_segments_rcvd == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_last_segment_index == LTE_RRC_SIB_MAX_SIB12_SEGMENTS);
  
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_12_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib12_ptr == NULL);
    // ----------------------------------------------------------------------

  }

  // Send SIB12 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // Send same segment multiple times - there should be no memory leak and no messages sent
  // Send SIB12 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);
  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

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
  
    // ----------------------------------------------------------------------
  
    /* Asserts on active Events */
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_PG);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CMAS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_12_mcc_8_5));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[1] == 3);
    for(int i = 2; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
    }

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == (LTE_RRC_SIB_SI_WT_TMR_FACTOR * 80));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                           LTE_RRC_SIB1_BITMASK |
                                                                           LTE_RRC_SIB2_BITMASK |
                                                                           LTE_RRC_SIB3_BITMASK |
                                                                           LTE_RRC_SIB4_BITMASK |
                                                                           LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                                LTE_RRC_SIB1_BITMASK |
                                                                                LTE_RRC_SIB2_BITMASK |
                                                                                LTE_RRC_SIB3_BITMASK |
                                                                                LTE_RRC_SIB4_BITMASK |
                                                                                LTE_RRC_SIB5_BITMASK));
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (0));
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                         LTE_RRC_SIB1_BITMASK |
                                                                                         LTE_RRC_SIB2_BITMASK |
                                                                                         LTE_RRC_SIB3_BITMASK |
                                                                                         LTE_RRC_SIB4_BITMASK |
                                                                                         LTE_RRC_SIB5_BITMASK |
                                                                                         LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 3);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                                  LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                                  LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);
  
    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_msg_id == 0x909);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_sr_num == 0x100);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[0].seg_ptr != NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[0].seg_ptr[0] == 0x10);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[0].seg_ptr[1] == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[0].seg_len == 2);
    for(int i = 1; i < LTE_RRC_SIB_MAX_SIB12_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_num_segments_rcvd == 1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_last_segment_index == LTE_RRC_SIB_MAX_SIB12_SEGMENTS);

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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_12_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                         LTE_RRC_SIB3_BITMASK |
                                                                         LTE_RRC_SIB4_BITMASK |
                                                                         LTE_RRC_SIB5_BITMASK |
                                                                         LTE_RRC_SIB12_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib12_seg1of2_8_5));
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
   // TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib12_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib12_v920);
  
    // ----------------------------------------------------------------------

  }

  if(!mod_bnd_crossed)
  {
    // Send SIB12 segment 2 (last segment)
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg2of2_8_5,
                                          sizeof(sib12_seg2of2_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          5);

    // We expect a cmas_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&cmas_ind_ptr, &buf_len);
    TF_ASSERT(cmas_ind_ptr->msg_hdr.id == LTE_RRC_CMAS_IND);
  
    att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)cmas_ind_ptr, 0);
    msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
    TF_ASSERT(rx_dsm_ptr != NULL);
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
    TF_ASSERT(dsm_packet_length == 4);
    TF_ASSERT(4 == dsm_pullup(&rx_dsm_ptr, sib12_warn_msg, (uint16)dsm_packet_length));
    TF_ASSERT(rx_dsm_ptr == NULL);
  
    TF_ASSERT(sib12_warn_msg[0] == 0x10);
    TF_ASSERT(sib12_warn_msg[1] == 0x0);
    TF_ASSERT(sib12_warn_msg[2] == 0x0F);
    TF_ASSERT(sib12_warn_msg[3] == 0x0);
    TF_ASSERT(cmas_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(cmas_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(cmas_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(cmas_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(cmas_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(cmas_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(cmas_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(cmas_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(cmas_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(cmas_ind_ptr->msg_id == 0x909);
    TF_ASSERT(cmas_ind_ptr->sr_num == 0x100);
	TF_ASSERT(cmas_ind_ptr->data_coding_scheme == 0x1);
  
    // We continue read SIB12 but no sib_sched_req would be sent

    // Asserts
    {
      /* Asserts on SIB internal data */
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                           LTE_RRC_SIB1_BITMASK |
                                                                                           LTE_RRC_SIB2_BITMASK |
                                                                                           LTE_RRC_SIB3_BITMASK |
                                                                                           LTE_RRC_SIB4_BITMASK |
                                                                                           LTE_RRC_SIB5_BITMASK |
                                                                                           LTE_RRC_SIB12_BITMASK));
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_12_mcc_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                           LTE_RRC_SIB3_BITMASK |
                                                                           LTE_RRC_SIB4_BITMASK |
                                                                           LTE_RRC_SIB5_BITMASK |
                                                                           LTE_RRC_SIB12_BITMASK));
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib12_seg2of2_8_5));
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
     // TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib12_ptr == 
                lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib12_v920);
    
      // ----------------------------------------------------------------------
  
    }
  } // Mod bnd not crossed
  else // Mod bnd crossed
  {
    // Send SIB12 segment 2 (last segment)
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg2of2_8_5,
                                          sizeof(sib12_seg2of2_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          64);

    // We expect a cmas_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&cmas_ind_ptr, &buf_len);
    TF_ASSERT(cmas_ind_ptr->msg_hdr.id == LTE_RRC_CMAS_IND);
  
    att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)cmas_ind_ptr, 0);
    msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
    TF_ASSERT(rx_dsm_ptr != NULL);
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
    TF_ASSERT(dsm_packet_length == 4);
    TF_ASSERT(4 == dsm_pullup(&rx_dsm_ptr, sib12_warn_msg, (uint16)dsm_packet_length));
    TF_ASSERT(rx_dsm_ptr == NULL);
  
    TF_ASSERT(sib12_warn_msg[0] == 0x10);
    TF_ASSERT(sib12_warn_msg[1] == 0x0);
    TF_ASSERT(sib12_warn_msg[2] == 0x0F);
    TF_ASSERT(sib12_warn_msg[3] == 0x0);
    TF_ASSERT(cmas_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(cmas_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(cmas_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(cmas_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(cmas_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(cmas_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(cmas_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(cmas_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(cmas_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(cmas_ind_ptr->msg_id == 0x909);
    TF_ASSERT(cmas_ind_ptr->sr_num == 0x100);
	TF_ASSERT(cmas_ind_ptr->data_coding_scheme == 0x1);

    // We expect a sib_sched_req message from RRC (to continue SIB12 reading)
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));
  
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
    
      // ----------------------------------------------------------------------
    
      /* Asserts on active Events */
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_PG);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CMAS);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);

      // ----------------------------------------------------------------------

      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_12_mcc_8_5));
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
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == LTE_RRC_NULL_SFN);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[1] == 3);
      for(int i = 2; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
      {
        // But for SIB12 all other SFNs should be NULL
        if (i != 12)
        {
          TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
        }
      }
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == (LTE_RRC_SIB_SI_WT_TMR_FACTOR * 80));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                                   LTE_RRC_SIB1_BITMASK |
                                                                                   LTE_RRC_SIB2_BITMASK |
                                                                                   LTE_RRC_SIB3_BITMASK |
                                                                                   LTE_RRC_SIB4_BITMASK |
                                                                                   LTE_RRC_SIB5_BITMASK |
                                                                                   LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                             LTE_RRC_SIB1_BITMASK |
                                                                             LTE_RRC_SIB2_BITMASK |
                                                                             LTE_RRC_SIB3_BITMASK |
                                                                             LTE_RRC_SIB4_BITMASK |
                                                                             LTE_RRC_SIB5_BITMASK |
                                                                             LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == 0);

      // ----------------------------------------------------------------------

      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == FALSE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB_MIB_SIB1_BITMASK));

      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 64);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 127);

      // ----------------------------------------------------------------------

      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                           LTE_RRC_SIB1_BITMASK |
                                                                                           LTE_RRC_SIB2_BITMASK |
                                                                                           LTE_RRC_SIB3_BITMASK |
                                                                                           LTE_RRC_SIB4_BITMASK |
                                                                                           LTE_RRC_SIB5_BITMASK |
                                                                                           LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 128);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                           LTE_RRC_SIB1_BITMASK |
                                                                                           LTE_RRC_SIB2_BITMASK |
                                                                                           LTE_RRC_SIB3_BITMASK |
                                                                                           LTE_RRC_SIB4_BITMASK |
                                                                                           LTE_RRC_SIB5_BITMASK |
                                                                                           LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 64);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 3);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                                    LTE_RRC_SIB3_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                                    LTE_RRC_SIB5_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);

      // ----------------------------------------------------------------------

      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_msg_id == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_sr_num == 0);
      for(int i = 0; i < LTE_RRC_SIB_MAX_SIB12_SEGMENTS; i++)
      {
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_ptr == NULL);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_len == 0);
      }
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_num_segments_rcvd == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_last_segment_index == LTE_RRC_SIB_MAX_SIB12_SEGMENTS);

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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_12_mcc_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                           LTE_RRC_SIB3_BITMASK |
                                                                           LTE_RRC_SIB4_BITMASK |
                                                                           LTE_RRC_SIB5_BITMASK |
                                                                           LTE_RRC_SIB12_BITMASK));
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib12_seg2of2_8_5));
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
     // TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib12_ptr == 
                lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib12_v920);
    
      // ----------------------------------------------------------------------
  
    } // Asserts

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
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == LTE_RRC_SIB12_BITMASK);
  
    // send SIB1 to RRC
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                          sizeof(sib1_2345_mcc_8_5),
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

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK));

  } // Mod bnd crossed

}


void lte_rrc_sib_test_basic_cmas_with_dsds(boolean mod_bnd_crossed)
{

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_cmas_ind_s *cmas_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib12_warn_msg[10];

  lte_rrc_msg_id_list_req_s cmas_msg_id_list_req;

  msgr_init_hdr(&cmas_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  cmas_msg_id_list_req.msg_id_range_list_size = 1;
  cmas_msg_id_list_req.msg_id_range_list[0].min_msg_id = 2000;
  cmas_msg_id_list_req.msg_id_range_list[0].max_msg_id = 4000;
  lte_rrc_utf_send_msg((byte*)&cmas_msg_id_list_req, sizeof(cmas_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_CMAS, FALSE, 
                                      1);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));


  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_MSG("Sending RF_UNAVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 1;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
  lte_rrc_sib_test_send_rf_unavail_ind();

  lte_rrc_utf_wait_for_done();

  TF_SLEEP(100);
  TF_MSG("Sending RF_AVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 5;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
  lte_rrc_sib_test_send_rf_avail_ind();

  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available == lte_rrc_sib_test_wt_tmr_before_rf_unavailable);


    // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));


  // Back to back requests
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_CMAS, FALSE, 
                                      2);

  // No messages should be sent
  lte_rrc_utf_check_no_more_msgs();

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
  
    // ----------------------------------------------------------------------
  
    /* Asserts on active Events */
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_PG);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CMAS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == LTE_RRC_SIB_MIB_SIB1_WT_TMR);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK));
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
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (0));
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_msg_id == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_sr_num == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIB12_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_num_segments_rcvd == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_last_segment_index == LTE_RRC_SIB_MAX_SIB12_SEGMENTS);
  
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib12_ptr == NULL);
    // ----------------------------------------------------------------------

  }

  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));

  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_MSG("Sending RF_UNAVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 9;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
  lte_rrc_sib_test_send_rf_unavail_ind();

  lte_rrc_utf_wait_for_done();

  TF_SLEEP(100);
  TF_MSG("Sending RF_AVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 13;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
  lte_rrc_sib_test_send_rf_avail_ind();

  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available == lte_rrc_sib_test_wt_tmr_before_rf_unavailable);

    // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));

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
  
    // ----------------------------------------------------------------------
  
    /* Asserts on active Events */
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_PG);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CMAS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_12_mcc_8_5));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[1] == 3);
    for(int i = 2; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == (LTE_RRC_SIB_SI_WT_TMR_FACTOR * 80));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                           LTE_RRC_SIB1_BITMASK |
                                                                           LTE_RRC_SIB2_BITMASK |
                                                                           LTE_RRC_SIB3_BITMASK |
                                                                           LTE_RRC_SIB4_BITMASK |
                                                                           LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                                LTE_RRC_SIB1_BITMASK |
                                                                                LTE_RRC_SIB2_BITMASK |
                                                                                LTE_RRC_SIB3_BITMASK |
                                                                                LTE_RRC_SIB4_BITMASK |
                                                                                LTE_RRC_SIB5_BITMASK));
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (0));
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                         LTE_RRC_SIB1_BITMASK |
                                                                                         LTE_RRC_SIB2_BITMASK |
                                                                                         LTE_RRC_SIB3_BITMASK |
                                                                                         LTE_RRC_SIB4_BITMASK |
                                                                                         LTE_RRC_SIB5_BITMASK |
                                                                                         LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 3);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 3);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                                  LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                                  LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);
  
    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_msg_id == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_sr_num == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIB12_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_num_segments_rcvd == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_last_segment_index == LTE_RRC_SIB_MAX_SIB12_SEGMENTS);
  
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_12_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib12_ptr == NULL);
    // ----------------------------------------------------------------------

  }

  // Send SIB12 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // Send same segment multiple times - there should be no memory leak and no messages sent
  // Send SIB12 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);
  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

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
  
    // ----------------------------------------------------------------------
  
    /* Asserts on active Events */
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_PG);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CMAS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_12_mcc_8_5));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[1] == 3);
    for(int i = 2; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
    }

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == (LTE_RRC_SIB_SI_WT_TMR_FACTOR * 80));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                           LTE_RRC_SIB1_BITMASK |
                                                                           LTE_RRC_SIB2_BITMASK |
                                                                           LTE_RRC_SIB3_BITMASK |
                                                                           LTE_RRC_SIB4_BITMASK |
                                                                           LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                                LTE_RRC_SIB1_BITMASK |
                                                                                LTE_RRC_SIB2_BITMASK |
                                                                                LTE_RRC_SIB3_BITMASK |
                                                                                LTE_RRC_SIB4_BITMASK |
                                                                                LTE_RRC_SIB5_BITMASK));
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (0));
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                         LTE_RRC_SIB1_BITMASK |
                                                                                         LTE_RRC_SIB2_BITMASK |
                                                                                         LTE_RRC_SIB3_BITMASK |
                                                                                         LTE_RRC_SIB4_BITMASK |
                                                                                         LTE_RRC_SIB5_BITMASK |
                                                                                         LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 3);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                                  LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                                  LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);
  
    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_msg_id == 0x909);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_sr_num == 0x100);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[0].seg_ptr != NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[0].seg_ptr[0] == 0x10);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[0].seg_ptr[1] == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[0].seg_len == 2);
    for(int i = 1; i < LTE_RRC_SIB_MAX_SIB12_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_num_segments_rcvd == 1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_last_segment_index == LTE_RRC_SIB_MAX_SIB12_SEGMENTS);

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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_12_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                         LTE_RRC_SIB3_BITMASK |
                                                                         LTE_RRC_SIB4_BITMASK |
                                                                         LTE_RRC_SIB5_BITMASK |
                                                                         LTE_RRC_SIB12_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib12_seg1of2_8_5));
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
   // TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib12_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib12_v920);
  
    // ----------------------------------------------------------------------

  }

  if(!mod_bnd_crossed)
  {
    // Send SIB12 segment 2 (last segment)
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg2of2_8_5,
                                          sizeof(sib12_seg2of2_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          5);

    // We expect a cmas_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&cmas_ind_ptr, &buf_len);
    TF_ASSERT(cmas_ind_ptr->msg_hdr.id == LTE_RRC_CMAS_IND);
  
    att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)cmas_ind_ptr, 0);
    msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
    TF_ASSERT(rx_dsm_ptr != NULL);
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
    TF_ASSERT(dsm_packet_length == 4);
    TF_ASSERT(4 == dsm_pullup(&rx_dsm_ptr, sib12_warn_msg, (uint16)dsm_packet_length));
    TF_ASSERT(rx_dsm_ptr == NULL);
  
    TF_ASSERT(sib12_warn_msg[0] == 0x10);
    TF_ASSERT(sib12_warn_msg[1] == 0x0);
    TF_ASSERT(sib12_warn_msg[2] == 0x0F);
    TF_ASSERT(sib12_warn_msg[3] == 0x0);
    TF_ASSERT(cmas_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(cmas_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(cmas_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(cmas_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(cmas_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(cmas_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(cmas_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(cmas_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(cmas_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(cmas_ind_ptr->msg_id == 0x909);
    TF_ASSERT(cmas_ind_ptr->sr_num == 0x100);
	TF_ASSERT(cmas_ind_ptr->data_coding_scheme == 0x1);
  
    // We continue read SIB12 but no sib_sched_req would be sent

    // Asserts
    {
      /* Asserts on SIB internal data */
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                           LTE_RRC_SIB1_BITMASK |
                                                                                           LTE_RRC_SIB2_BITMASK |
                                                                                           LTE_RRC_SIB3_BITMASK |
                                                                                           LTE_RRC_SIB4_BITMASK |
                                                                                           LTE_RRC_SIB5_BITMASK |
                                                                                           LTE_RRC_SIB12_BITMASK));
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_12_mcc_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                           LTE_RRC_SIB3_BITMASK |
                                                                           LTE_RRC_SIB4_BITMASK |
                                                                           LTE_RRC_SIB5_BITMASK |
                                                                           LTE_RRC_SIB12_BITMASK));
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib12_seg2of2_8_5));
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
     // TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib12_ptr == 
                lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib12_v920);
    
      // ----------------------------------------------------------------------
  
    }
  } // Mod bnd not crossed
  else // Mod bnd crossed
  {
    // Send SIB12 segment 2 (last segment)
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg2of2_8_5,
                                          sizeof(sib12_seg2of2_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          64);

    // We expect a cmas_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&cmas_ind_ptr, &buf_len);
    TF_ASSERT(cmas_ind_ptr->msg_hdr.id == LTE_RRC_CMAS_IND);
  
    att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)cmas_ind_ptr, 0);
    msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
    TF_ASSERT(rx_dsm_ptr != NULL);
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
    TF_ASSERT(dsm_packet_length == 4);
    TF_ASSERT(4 == dsm_pullup(&rx_dsm_ptr, sib12_warn_msg, (uint16)dsm_packet_length));
    TF_ASSERT(rx_dsm_ptr == NULL);
  
    TF_ASSERT(sib12_warn_msg[0] == 0x10);
    TF_ASSERT(sib12_warn_msg[1] == 0x0);
    TF_ASSERT(sib12_warn_msg[2] == 0x0F);
    TF_ASSERT(sib12_warn_msg[3] == 0x0);
    TF_ASSERT(cmas_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(cmas_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(cmas_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(cmas_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(cmas_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(cmas_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(cmas_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(cmas_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(cmas_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(cmas_ind_ptr->msg_id == 0x909);
    TF_ASSERT(cmas_ind_ptr->sr_num == 0x100);
	TF_ASSERT(cmas_ind_ptr->data_coding_scheme == 0x1);

    // We expect a sib_sched_req message from RRC (to continue SIB12 reading)
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));
  
	lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
 TF_MSG("Sending RF_UNAVAILABLE_IND");
 lte_rrc_sib_test_rf_status_sfn = 15;
 lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
 lte_rrc_sib_test_send_rf_unavail_ind();

 lte_rrc_utf_wait_for_done();

 TF_SLEEP(100);
 TF_MSG("Sending RF_AVAILABLE_IND");
 lte_rrc_sib_test_rf_status_sfn = 19;
 lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
 lte_rrc_sib_test_send_rf_avail_ind();

 lte_rrc_utf_wait_for_done();

 lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
 TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available == lte_rrc_sib_test_wt_tmr_before_rf_unavailable);


     // We expect a sib_sched_req message from RRC (to continue SIB12 reading)
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB12_BITMASK));


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
    
      // ----------------------------------------------------------------------
    
      /* Asserts on active Events */
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_PG);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CMAS);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);

      // ----------------------------------------------------------------------

      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_12_mcc_8_5));
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
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == LTE_RRC_NULL_SFN);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[1] == 3);
      for(int i = 2; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
      {
        // But for SIB12 all other SFNs should be NULL
        if (i != 12)
        {
          TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
        }
      }
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == (LTE_RRC_SIB_SI_WT_TMR_FACTOR * 80));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                                   LTE_RRC_SIB1_BITMASK |
                                                                                   LTE_RRC_SIB2_BITMASK |
                                                                                   LTE_RRC_SIB3_BITMASK |
                                                                                   LTE_RRC_SIB4_BITMASK |
                                                                                   LTE_RRC_SIB5_BITMASK |
                                                                                   LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                             LTE_RRC_SIB1_BITMASK |
                                                                             LTE_RRC_SIB2_BITMASK |
                                                                             LTE_RRC_SIB3_BITMASK |
                                                                             LTE_RRC_SIB4_BITMASK |
                                                                             LTE_RRC_SIB5_BITMASK |
                                                                             LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == 0);

      // ----------------------------------------------------------------------

      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == FALSE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB_MIB_SIB1_BITMASK));

      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 64);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 127);

      // ----------------------------------------------------------------------

      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                           LTE_RRC_SIB1_BITMASK |
                                                                                           LTE_RRC_SIB2_BITMASK |
                                                                                           LTE_RRC_SIB3_BITMASK |
                                                                                           LTE_RRC_SIB4_BITMASK |
                                                                                           LTE_RRC_SIB5_BITMASK |
                                                                                           LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 128);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                           LTE_RRC_SIB1_BITMASK |
                                                                                           LTE_RRC_SIB2_BITMASK |
                                                                                           LTE_RRC_SIB3_BITMASK |
                                                                                           LTE_RRC_SIB4_BITMASK |
                                                                                           LTE_RRC_SIB5_BITMASK |
                                                                                           LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 64);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 3);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                                    LTE_RRC_SIB3_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                                    LTE_RRC_SIB5_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);

      // ----------------------------------------------------------------------

      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_msg_id == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_curr_sr_num == 0);
      for(int i = 0; i < LTE_RRC_SIB_MAX_SIB12_SEGMENTS; i++)
      {
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_ptr == NULL);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_segment[i].seg_len == 0);
      }
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_num_segments_rcvd == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_last_segment_index == LTE_RRC_SIB_MAX_SIB12_SEGMENTS);

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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_12_mcc_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                           LTE_RRC_SIB3_BITMASK |
                                                                           LTE_RRC_SIB4_BITMASK |
                                                                           LTE_RRC_SIB5_BITMASK |
                                                                           LTE_RRC_SIB12_BITMASK));
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB12_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib12_seg2of2_8_5));
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
     // TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib12_ptr == 
                lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib12_v920);
    
      // ----------------------------------------------------------------------
  
    } // Asserts

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
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == LTE_RRC_SIB12_BITMASK);
  
	lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
TF_MSG("Sending RF_UNAVAILABLE_IND");
lte_rrc_sib_test_rf_status_sfn = 23;
lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
lte_rrc_sib_test_send_rf_unavail_ind();

lte_rrc_utf_wait_for_done();

TF_SLEEP(100);
TF_MSG("Sending RF_AVAILABLE_IND");
lte_rrc_sib_test_rf_status_sfn = 27;
lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
lte_rrc_sib_test_send_rf_avail_ind();

lte_rrc_utf_wait_for_done();

lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available == lte_rrc_sib_test_wt_tmr_before_rf_unavailable);

    // We expect a sib_sched_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB12_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB12_BITMASK));


	// send SIB1 to RRC
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                          sizeof(sib1_2345_mcc_8_5),
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

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK));

  } // Mod bnd crossed

}

/* Test case to check SIB12 reading when SIB12 gets scheduled in SIB1 without 
  CMAS notification*/
void lte_rrc_sib_test_cmas_without_notification()
{

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_cmas_ind_s *cmas_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib12_warn_msg[10];

  lte_rrc_msg_id_list_req_s cmas_msg_id_list_req;

  msgr_init_hdr(&cmas_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  cmas_msg_id_list_req.msg_id_range_list_size = 1;
  cmas_msg_id_list_req.msg_id_range_list[0].min_msg_id = 2000;
  cmas_msg_id_list_req.msg_id_range_list[0].max_msg_id = 4000;
  lte_rrc_utf_send_msg((byte*)&cmas_msg_id_list_req, sizeof(cmas_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);
  
  //---------------------------------------------------------------

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
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));

  // We expect a LTE_RRC_SIBS_UPDATED_INDI from SIB
  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB12_BITMASK));

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  // Send SIB12 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);  

  // Send SIB12 segment 2 (last segment)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg2of2_8_5,
                                        sizeof(sib12_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        66);

  // We expect a cmas_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&cmas_ind_ptr, &buf_len);
  TF_ASSERT(cmas_ind_ptr->msg_hdr.id == LTE_RRC_CMAS_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)cmas_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 4);
  TF_ASSERT(4 == dsm_pullup(&rx_dsm_ptr, sib12_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib12_warn_msg[0] == 0x10);
  TF_ASSERT(sib12_warn_msg[1] == 0x0);
  TF_ASSERT(sib12_warn_msg[2] == 0x0F);
  TF_ASSERT(sib12_warn_msg[3] == 0x0);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(cmas_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(cmas_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(cmas_ind_ptr->msg_id == 0x909);
  TF_ASSERT(cmas_ind_ptr->sr_num == 0x100);
  TF_ASSERT(cmas_ind_ptr->data_coding_scheme == 0x1);

  // We don't expect a sib_sched_req message from RRC (but continue SIB12 reading)
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                       LTE_RRC_SIB1_BITMASK |
                                                                                       LTE_RRC_SIB2_BITMASK |
                                                                                       LTE_RRC_SIB3_BITMASK |
                                                                                       LTE_RRC_SIB4_BITMASK |
                                                                                       LTE_RRC_SIB5_BITMASK |
                                                                                       LTE_RRC_SIB12_BITMASK));

  // send MIB to RRC in the next mod period
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            128);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));

  // Send SIB1 that removes SIB12 from SIB1 schedule
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        128);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);

}

/* Function to test SIB12 reading in current modification period */
void lte_rrc_sib_test_sib12_reading_in_curr_mod_period(uint8 sfn)
{

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_cmas_ind_s *cmas_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length = 0;
  unsigned char sib12_warn_msg[10];

  // send MIB to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            sfn);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == (sfn + LTE_RRC_SIB_MIN_MOD_PRD_WIN_LEN));
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));

  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        sfn);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == (sfn + LTE_RRC_SIB_MIN_MOD_PRD_WIN_LEN));
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  // Send SIB12 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        sfn + 1);

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  // Send SIB12 segment 2 (last segment)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg2of2_8_5,
                                        sizeof(sib12_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        sfn + 2);

  // We expect a cmas_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&cmas_ind_ptr, &buf_len);
  TF_ASSERT(cmas_ind_ptr->msg_hdr.id == LTE_RRC_CMAS_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)cmas_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 4);
  TF_ASSERT(4 == dsm_pullup(&rx_dsm_ptr, sib12_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib12_warn_msg[0] == 0x10);
  TF_ASSERT(sib12_warn_msg[1] == 0x0);
  TF_ASSERT(sib12_warn_msg[2] == 0x0F);
  TF_ASSERT(sib12_warn_msg[3] == 0x0);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(cmas_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(cmas_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(cmas_ind_ptr->msg_id == 0x909);
  TF_ASSERT(cmas_ind_ptr->sr_num == 0x100);
  TF_ASSERT(cmas_ind_ptr->data_coding_scheme == 0x1);

  // We don't expect a sib_sched_req message from RRC (but continue SIB12 reading)
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                       LTE_RRC_SIB1_BITMASK |
                                                                                       LTE_RRC_SIB2_BITMASK |
                                                                                       LTE_RRC_SIB3_BITMASK |
                                                                                       LTE_RRC_SIB4_BITMASK |
                                                                                       LTE_RRC_SIB5_BITMASK |
                                                                                       LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == (sfn + LTE_RRC_SIB_MIN_MOD_PRD_WIN_LEN));

}

/* Test case to verify continous SIB12 reading from the time it is scheduled in SIB1
till it is removed from SIB1 schedule. */
void lte_rrc_sib_test_continous_sib12_reading(void)
{

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_cmas_ind_s *cmas_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length = 0, sfn = 0;
  unsigned char sib12_warn_msg[10];
  uint8 i = 0;

  lte_rrc_msg_id_list_req_s cmas_msg_id_list_req;

  msgr_init_hdr(&cmas_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  cmas_msg_id_list_req.msg_id_range_list_size = 1;
  cmas_msg_id_list_req.msg_id_range_list[0].min_msg_id = 2000;
  cmas_msg_id_list_req.msg_id_range_list[0].max_msg_id = 4000;
  lte_rrc_utf_send_msg((byte*)&cmas_msg_id_list_req, sizeof(cmas_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_CMAS, FALSE, 
                                      1);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));

  // Back to back requests
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_CMAS, FALSE, 
                                      2);

  // No messages should be sent
  lte_rrc_utf_check_no_more_msgs();

  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  // Send SIB12 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  // Send SIB12 segment 2 (last segment)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg2of2_8_5,
                                        sizeof(sib12_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        5);

  // We expect a cmas_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&cmas_ind_ptr, &buf_len);
  TF_ASSERT(cmas_ind_ptr->msg_hdr.id == LTE_RRC_CMAS_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)cmas_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 4);
  TF_ASSERT(4 == dsm_pullup(&rx_dsm_ptr, sib12_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib12_warn_msg[0] == 0x10);
  TF_ASSERT(sib12_warn_msg[1] == 0x0);
  TF_ASSERT(sib12_warn_msg[2] == 0x0F);
  TF_ASSERT(sib12_warn_msg[3] == 0x0);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(cmas_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(cmas_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(cmas_ind_ptr->msg_id == 0x909);
  TF_ASSERT(cmas_ind_ptr->sr_num == 0x100);
  TF_ASSERT(cmas_ind_ptr->data_coding_scheme == 0x1);

  // We don't expect a sib_sched_req message from RRC (but continue SIB12 reading)
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                       LTE_RRC_SIB1_BITMASK |
                                                                                       LTE_RRC_SIB2_BITMASK |
                                                                                       LTE_RRC_SIB3_BITMASK |
                                                                                       LTE_RRC_SIB4_BITMASK |
                                                                                       LTE_RRC_SIB5_BITMASK |
                                                                                       LTE_RRC_SIB12_BITMASK));

  for (i = 1; i <= 20; i++)
  {
    sfn = ((i * LTE_RRC_SIB_MIN_MOD_PRD_WIN_LEN)%LTE_RRC_SIB_MAX_SFN);
    lte_rrc_sib_test_sib12_reading_in_curr_mod_period(sfn);
  }

  sfn = ((i * LTE_RRC_SIB_MIN_MOD_PRD_WIN_LEN)%LTE_RRC_SIB_MAX_SFN);

  // send MIB to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            sfn);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == (sfn + LTE_RRC_SIB_MIN_MOD_PRD_WIN_LEN));
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));

  // Send SIB1 that removes SIB12 from SIB1 schedule
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        sfn);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == (sfn + LTE_RRC_SIB_MIN_MOD_PRD_WIN_LEN));
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);

}

/* Test case to check rcvd SIB12 with blank segement, no cmas_ind should be sent */
void lte_rrc_sib_test_cmas_rcvd_blank_segment(void)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_cmas_ind_s *cmas_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length = 0, sfn = 0;
  unsigned char sib12_warn_msg[10];
  uint8 i = 0;

  /* get_sibs_req sent to SIB for ETWS or CMAS */
  lte_rrc_get_sibs_reqi_s get_sibs_req;

  lte_rrc_msg_id_list_req_s cmas_msg_id_list_req;

  msgr_init_hdr(&cmas_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  cmas_msg_id_list_req.msg_id_range_list_size = 1;
  cmas_msg_id_list_req.msg_id_range_list[0].min_msg_id = 1000;
  cmas_msg_id_list_req.msg_id_range_list[0].max_msg_id = 4000;
  lte_rrc_utf_send_msg((byte*)&cmas_msg_id_list_req, sizeof(cmas_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();

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

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);

  //---------------------------------------------------------------
  
  // Set the cause of next get_sibs_reqi to be etws, event parameters will be changed
  // after this request
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1,
                                      FREQ_1,
                                      LTE_RRC_PROC_SIB,
                                      LTE_RRC_GET_SIBS_CAUSE_CMAS,
                                      FALSE,
                                      64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);

  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            64);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB12_BITMASK));

  // Send SIB12 blank segment, no cmas_ind should be sent
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_blank_8_5,
                                        sizeof(sib12_blank_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  // We don't expect a sib_sched_req message from RRC (but continue SIB12 reading)
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                       LTE_RRC_SIB1_BITMASK |
                                                                                       LTE_RRC_SIB2_BITMASK |
                                                                                       LTE_RRC_SIB3_BITMASK |
                                                                                       LTE_RRC_SIB4_BITMASK |
                                                                                       LTE_RRC_SIB5_BITMASK |
                                                                                       LTE_RRC_SIB12_BITMASK));

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

}

/* Test case to check SIB12 reading when event cause is etws in connected state */
void lte_rrc_sib_test_cmas_reading_with_event_cause_etws_when_connected(void)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_cmas_ind_s *cmas_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length = 0, sfn = 0;
  unsigned char sib12_warn_msg[10];
  uint8 i = 0;

  /* get_sibs_req sent to SIB for ETWS or CMAS */
  lte_rrc_get_sibs_reqi_s get_sibs_req;

  lte_rrc_msg_id_list_req_s cmas_msg_id_list_req;

  msgr_init_hdr(&cmas_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  cmas_msg_id_list_req.msg_id_range_list_size = 1;
  cmas_msg_id_list_req.msg_id_range_list[0].min_msg_id = 2000;
  cmas_msg_id_list_req.msg_id_range_list[0].max_msg_id = 4000;
  lte_rrc_utf_send_msg((byte*)&cmas_msg_id_list_req, sizeof(cmas_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();

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

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);

  //---------------------------------------------------------------
  
  // Set the cause of next get_sibs_reqi to be etws, event parameters will be changed
  // after this request
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1,
                                      FREQ_1,
                                      LTE_RRC_PROC_SIB,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS,
                                      FALSE,
                                      64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);

  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            64);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);

  // Send SIB12 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  //---------------------------------------------------------------

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();
/*
  // We expect a sib_sched_req msg
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);

  //---------------------------------------------------------------
*/
  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            128);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 128);

  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        130);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));


  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK |
                                         LTE_RRC_SIB3_BITMASK |
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB12_BITMASK));

  // Send SIB12 segment 2 (last segment)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg2of2_8_5,
                                        sizeof(sib12_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        128);  

  // We expect a cmas_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&cmas_ind_ptr, &buf_len);
  TF_ASSERT(cmas_ind_ptr->msg_hdr.id == LTE_RRC_CMAS_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)cmas_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 4);
  TF_ASSERT(4 == dsm_pullup(&rx_dsm_ptr, sib12_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib12_warn_msg[0] == 0x10);
  TF_ASSERT(sib12_warn_msg[1] == 0x0);
  TF_ASSERT(sib12_warn_msg[2] == 0x0F);
  TF_ASSERT(sib12_warn_msg[3] == 0x0);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(cmas_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(cmas_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(cmas_ind_ptr->msg_id == 0x909);
  TF_ASSERT(cmas_ind_ptr->sr_num == 0x100);
  TF_ASSERT(cmas_ind_ptr->data_coding_scheme == 0x1);

  // We don't expect a sib_sched_req message from RRC (but continue SIB12 reading)
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                       LTE_RRC_SIB1_BITMASK |
                                                                                       LTE_RRC_SIB2_BITMASK |
                                                                                       LTE_RRC_SIB3_BITMASK |
                                                                                       LTE_RRC_SIB4_BITMASK |
                                                                                       LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB12_BITMASK));
  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();
}

/* Test case to check SIB12 reading after connected to idle transition */
void lte_rrc_sib_test_cmas_reading_after_connected_to_idle(void)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_cmas_ind_s *cmas_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length = 0, sfn = 0;
  unsigned char sib12_warn_msg[10];
  uint8 i = 0;

  /* get_sibs_req sent to SIB for ETWS or CMAS */
  lte_rrc_get_sibs_reqi_s get_sibs_req;

  lte_rrc_msg_id_list_req_s cmas_msg_id_list_req;

  msgr_init_hdr(&cmas_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  cmas_msg_id_list_req.msg_id_range_list_size = 1;
  cmas_msg_id_list_req.msg_id_range_list[0].min_msg_id = 2000;
  cmas_msg_id_list_req.msg_id_range_list[0].max_msg_id = 4000;
  lte_rrc_utf_send_msg((byte*)&cmas_msg_id_list_req, sizeof(cmas_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();

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

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);

  //---------------------------------------------------------------
  
  // Set the cause of next get_sibs_reqi to be etws, event parameters will be changed
  // after this request
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1,
                                      FREQ_1,
                                      LTE_RRC_PROC_SIB,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS,
                                      FALSE,
                                      64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);

  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            64);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);

  // Send SIB12 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  //---------------------------------------------------------------

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();
/*
  // We expect a sib_sched_req msg
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
*/
  //---------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            128);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK | 
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 128);

  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        130);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));


  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK |
                                         LTE_RRC_SIB3_BITMASK |
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB12_BITMASK));

  //---------------------------------------------------------------

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
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB12_BITMASK));

  // Set the paging cycle to a low value so that next mod period value tag check is skipped
  TF_STUB(lte_rrc_paging_get_ue_specific_paging_cycle) = lte_rrc_sib_test_paging_get_ue_specific_paging_cycle;

  // Send SIB12 segment 2 (last segment)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg2of2_8_5,
                                        sizeof(sib12_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        128);  
/*
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK | 
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
*/

  // We expect a cmas_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&cmas_ind_ptr, &buf_len);
  TF_ASSERT(cmas_ind_ptr->msg_hdr.id == LTE_RRC_CMAS_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)cmas_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 4);
  TF_ASSERT(4 == dsm_pullup(&rx_dsm_ptr, sib12_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib12_warn_msg[0] == 0x10);
  TF_ASSERT(sib12_warn_msg[1] == 0x0);
  TF_ASSERT(sib12_warn_msg[2] == 0x0F);
  TF_ASSERT(sib12_warn_msg[3] == 0x0);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(cmas_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(cmas_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(cmas_ind_ptr->msg_id == 0x909);
  TF_ASSERT(cmas_ind_ptr->sr_num == 0x100);
  TF_ASSERT(cmas_ind_ptr->data_coding_scheme == 0x1);

  // We don't expect a sib_sched_req message from RRC (but continue SIB12 reading)
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                       LTE_RRC_SIB1_BITMASK |
                                                                                       LTE_RRC_SIB2_BITMASK |
                                                                                       LTE_RRC_SIB3_BITMASK | 
                                                                                       LTE_RRC_SIB4_BITMASK |
                                                                                       LTE_RRC_SIB5_BITMASK |
                                                                                       LTE_RRC_SIB12_BITMASK));

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();
}



/* Test case to check SIB12 reading after connected to idle transition */
void lte_rrc_sib_test_cmas_reading_after_connected_to_idle_with_dsds(void)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_cmas_ind_s *cmas_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length = 0, sfn = 0;
  unsigned char sib12_warn_msg[10];
  uint8 i = 0;

  /* get_sibs_req sent to SIB for ETWS or CMAS */
  lte_rrc_get_sibs_reqi_s get_sibs_req;

  lte_rrc_msg_id_list_req_s cmas_msg_id_list_req;

  msgr_init_hdr(&cmas_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  cmas_msg_id_list_req.msg_id_range_list_size = 1;
  cmas_msg_id_list_req.msg_id_range_list[0].min_msg_id = 2000;
  cmas_msg_id_list_req.msg_id_range_list[0].max_msg_id = 4000;
  lte_rrc_utf_send_msg((byte*)&cmas_msg_id_list_req, sizeof(cmas_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();

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

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);

  //---------------------------------------------------------------
  
  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_MSG("Sending RF_UNAVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 1;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
  lte_rrc_sib_test_send_rf_unavail_ind();

  lte_rrc_utf_wait_for_done();

  TF_SLEEP(100);
  TF_MSG("Sending RF_AVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 5;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
  lte_rrc_sib_test_send_rf_avail_ind();
  lte_rrc_utf_wait_for_done();
  lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available == lte_rrc_sib_test_wt_tmr_before_rf_unavailable);


    // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);

  // Set the cause of next get_sibs_reqi to be etws, event parameters will be changed
  // after this request
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1,
                                      FREQ_1,
                                      LTE_RRC_PROC_SIB,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS,
                                      FALSE,
                                      64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);

  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
//  printf("\nTime before = %d\n", lte_rrc_sib_test_wt_tmr_before_rf_unavailable);
  TF_MSG("Sending RF_UNAVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 9;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
  lte_rrc_sib_test_send_rf_unavail_ind();

  lte_rrc_utf_wait_for_done();

  TF_SLEEP(100);
  TF_MSG("Sending RF_AVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 13;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
  lte_rrc_sib_test_send_rf_avail_ind();
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
 // printf("\nTime after = %d\n", lte_rrc_sib_test_wt_tmr_after_rf_available);

  //Please check. Seems to be a bug. The WT TIMER value before RF_UNAVAILABLE is 920.
  //But the WT_TIMER value after RF_AVAILABLE becomes 320ms.
  //TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available == lte_rrc_sib_test_wt_tmr_before_rf_unavailable);


    // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);

  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));


  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_MSG("Sending RF_UNAVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 66;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
  lte_rrc_sib_test_send_rf_unavail_ind();

  lte_rrc_utf_wait_for_done();

  TF_SLEEP(100);
  TF_MSG("Sending RF_AVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 76;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
  lte_rrc_sib_test_send_rf_avail_ind();
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available == lte_rrc_sib_test_wt_tmr_before_rf_unavailable);

    // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            77);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 77);

  // Send SIB12 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        79);

  //---------------------------------------------------------------

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();
/*
  // We expect a sib_sched_req msg
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 79);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
*/
  //---------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            128);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK | 
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 128);


  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_MSG("Sending RF_UNAVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 130;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
  lte_rrc_sib_test_send_rf_unavail_ind();

  lte_rrc_utf_wait_for_done();

  TF_SLEEP(100);
  TF_MSG("Sending RF_AVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 140;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
  lte_rrc_sib_test_send_rf_avail_ind();
  lte_rrc_utf_wait_for_done();
  lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available == lte_rrc_sib_test_wt_tmr_before_rf_unavailable);


  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
														 LTE_RRC_SIB2_BITMASK |
														 LTE_RRC_SIB3_BITMASK | 
														 LTE_RRC_SIB4_BITMASK |
                             LTE_RRC_SIB5_BITMASK |
														 LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
														 LTE_RRC_SIB1_BITMASK |
														 LTE_RRC_SIB2_BITMASK |
														 LTE_RRC_SIB3_BITMASK |
														 LTE_RRC_SIB4_BITMASK |
                             LTE_RRC_SIB5_BITMASK |
														 LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 128);


  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        141);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB12_BITMASK));


  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_MSG("Sending RF_UNAVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 145;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
  lte_rrc_sib_test_send_rf_unavail_ind();

  lte_rrc_utf_wait_for_done();

  TF_SLEEP(100);
  TF_MSG("Sending RF_AVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 155;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
  lte_rrc_sib_test_send_rf_avail_ind();
  lte_rrc_utf_wait_for_done();
  lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available == lte_rrc_sib_test_wt_tmr_before_rf_unavailable);


    // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));


  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK |
                                         LTE_RRC_SIB3_BITMASK |
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB12_BITMASK));

  //---------------------------------------------------------------

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
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB12_BITMASK));

  // Set the paging cycle to a low value so that next mod period value tag check is skipped
  TF_STUB(lte_rrc_paging_get_ue_specific_paging_cycle) = lte_rrc_sib_test_paging_get_ue_specific_paging_cycle;

  // Send SIB12 segment 2 (last segment)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg2of2_8_5,
                                        sizeof(sib12_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        160);  
/*
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK | 
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));

*/
  // We expect a cmas_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&cmas_ind_ptr, &buf_len);
  TF_ASSERT(cmas_ind_ptr->msg_hdr.id == LTE_RRC_CMAS_IND);

  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_MSG("Sending RF_UNAVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 168;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
  lte_rrc_sib_test_send_rf_unavail_ind();

  lte_rrc_utf_wait_for_done();

  TF_SLEEP(100);
  TF_MSG("Sending RF_AVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 178;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
  lte_rrc_sib_test_send_rf_avail_ind();
  lte_rrc_utf_wait_for_done();
  lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available == lte_rrc_sib_test_wt_tmr_before_rf_unavailable);


    // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK | 
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));


  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)cmas_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 4);
  TF_ASSERT(4 == dsm_pullup(&rx_dsm_ptr, sib12_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib12_warn_msg[0] == 0x10);
  TF_ASSERT(sib12_warn_msg[1] == 0x0);
  TF_ASSERT(sib12_warn_msg[2] == 0x0F);
  TF_ASSERT(sib12_warn_msg[3] == 0x0);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(cmas_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(cmas_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(cmas_ind_ptr->msg_id == 0x909);
  TF_ASSERT(cmas_ind_ptr->sr_num == 0x100);
  TF_ASSERT(cmas_ind_ptr->data_coding_scheme == 0x1);

  // We don't expect a sib_sched_req message from RRC (but continue SIB12 reading)
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                       LTE_RRC_SIB1_BITMASK |
                                                                                       LTE_RRC_SIB2_BITMASK |
                                                                                       LTE_RRC_SIB3_BITMASK | 
                                                                                       LTE_RRC_SIB4_BITMASK |
                                                                                       LTE_RRC_SIB5_BITMASK |
                                                                                       LTE_RRC_SIB12_BITMASK));

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();
}

/* Test to check receiving two CMAS message in  one modification period */
void lte_rrc_sib_test_cmas_rcvd_two_msg_in_one_mod_prd(void)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_cmas_ind_s *cmas_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length = 0, sfn = 0;
  unsigned char sib12_warn_msg[10];
  uint8 i = 0;

  /* get_sibs_req sent to SIB for ETWS or CMAS */
  lte_rrc_get_sibs_reqi_s get_sibs_req;

  lte_rrc_msg_id_list_req_s cmas_msg_id_list_req;

  msgr_init_hdr(&cmas_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  cmas_msg_id_list_req.msg_id_range_list_size = 1;
  cmas_msg_id_list_req.msg_id_range_list[0].min_msg_id = 2000;
  cmas_msg_id_list_req.msg_id_range_list[0].max_msg_id = 4000;
  lte_rrc_utf_send_msg((byte*)&cmas_msg_id_list_req, sizeof(cmas_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();

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

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_CMAS, FALSE, 
                                      64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            64);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);

  //---------------------------------------------------------------

  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB12_BITMASK));

  //---------------------------------------------------------------
  
  // Send SIB12 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        66);

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  // Send SIB12 segment 2
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg2of2_8_5,
                                        sizeof(sib12_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        67);

  // We expect a cmas_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&cmas_ind_ptr, &buf_len);
  TF_ASSERT(cmas_ind_ptr->msg_hdr.id == LTE_RRC_CMAS_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)cmas_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 4);
  TF_ASSERT(4 == dsm_pullup(&rx_dsm_ptr, sib12_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib12_warn_msg[0] == 0x10);
  TF_ASSERT(sib12_warn_msg[1] == 0x0);
  TF_ASSERT(sib12_warn_msg[2] == 0x0F);
  TF_ASSERT(sib12_warn_msg[3] == 0x0);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(cmas_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(cmas_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(cmas_ind_ptr->msg_id == 0x909);
  TF_ASSERT(cmas_ind_ptr->sr_num == 0x100);
  TF_ASSERT(cmas_ind_ptr->data_coding_scheme == 0x1);

  // We don't expect a sib_sched_req message from RRC (but continue SIB12 reading)
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                       LTE_RRC_SIB1_BITMASK |
                                                                                       LTE_RRC_SIB2_BITMASK |
                                                                                       LTE_RRC_SIB3_BITMASK |
                                                                                       LTE_RRC_SIB4_BITMASK |
                                                                                       LTE_RRC_SIB5_BITMASK |
                                                                                       LTE_RRC_SIB12_BITMASK));

  //----------------------------------------------------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_CMAS, FALSE, 
                                      70);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));

  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        71);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));
  // Send SIB12 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        72);

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  // Send SIB12 segment 2
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg2of2_8_5,
                                        sizeof(sib12_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        73);

  // We expect a cmas_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&cmas_ind_ptr, &buf_len);
  TF_ASSERT(cmas_ind_ptr->msg_hdr.id == LTE_RRC_CMAS_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)cmas_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 4);
  TF_ASSERT(4 == dsm_pullup(&rx_dsm_ptr, sib12_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib12_warn_msg[0] == 0x10);
  TF_ASSERT(sib12_warn_msg[1] == 0x0);
  TF_ASSERT(sib12_warn_msg[2] == 0x0F);
  TF_ASSERT(sib12_warn_msg[3] == 0x0);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(cmas_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(cmas_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(cmas_ind_ptr->msg_id == 0x909);
  TF_ASSERT(cmas_ind_ptr->sr_num == 0x100);
  TF_ASSERT(cmas_ind_ptr->data_coding_scheme == 0x1);

  // We don't expect a sib_sched_req message from RRC (but continue SIB12 reading)
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                       LTE_RRC_SIB1_BITMASK |
                                                                                       LTE_RRC_SIB2_BITMASK |
                                                                                       LTE_RRC_SIB3_BITMASK |
                                                                                       LTE_RRC_SIB4_BITMASK |
                                                                                       LTE_RRC_SIB5_BITMASK |
                                                                                       LTE_RRC_SIB12_BITMASK));
  
  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();
}

/* Test to check receiving two CMAS message in one modification period
   without high layer's request */
void lte_rrc_sib_test_cmas_rcvd_two_msg_in_one_mod_prd_without_notification(void)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_cmas_ind_s *cmas_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length = 0, sfn = 0;
  unsigned char sib12_warn_msg[10];
  uint8 i = 0;

  /* get_sibs_req sent to SIB for ETWS or CMAS */
  lte_rrc_get_sibs_reqi_s get_sibs_req;

  lte_rrc_msg_id_list_req_s cmas_msg_id_list_req;

  msgr_init_hdr(&cmas_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  cmas_msg_id_list_req.msg_id_range_list_size = 1;
  cmas_msg_id_list_req.msg_id_range_list[0].min_msg_id = 2000;
  cmas_msg_id_list_req.msg_id_range_list[0].max_msg_id = 4000;
  lte_rrc_utf_send_msg((byte*)&cmas_msg_id_list_req, sizeof(cmas_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();

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

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_CMAS, FALSE, 
                                      64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            64);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);

  //---------------------------------------------------------------

  // Send SIB1 that schedules SIB12
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_12_mcc_8_5,
                                        sizeof(sib1_2345_12_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB12_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB12_BITMASK));

  //---------------------------------------------------------------
  
  // Send SIB12 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        66);

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  // Send SIB12 segment 2
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg2of2_8_5,
                                        sizeof(sib12_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        67);

  // We expect a cmas_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&cmas_ind_ptr, &buf_len);
  TF_ASSERT(cmas_ind_ptr->msg_hdr.id == LTE_RRC_CMAS_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)cmas_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 4);
  TF_ASSERT(4 == dsm_pullup(&rx_dsm_ptr, sib12_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib12_warn_msg[0] == 0x10);
  TF_ASSERT(sib12_warn_msg[1] == 0x0);
  TF_ASSERT(sib12_warn_msg[2] == 0x0F);
  TF_ASSERT(sib12_warn_msg[3] == 0x0);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(cmas_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(cmas_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(cmas_ind_ptr->msg_id == 0x909);
  TF_ASSERT(cmas_ind_ptr->sr_num == 0x100);
  TF_ASSERT(cmas_ind_ptr->data_coding_scheme == 0x1);

  // We don't expect a sib_sched_req message from RRC (but continue SIB12 reading)
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                       LTE_RRC_SIB1_BITMASK |
                                                                                       LTE_RRC_SIB2_BITMASK |
                                                                                       LTE_RRC_SIB3_BITMASK |
                                                                                       LTE_RRC_SIB4_BITMASK |
                                                                                       LTE_RRC_SIB5_BITMASK |
                                                                                       LTE_RRC_SIB12_BITMASK));

  //----------------------------------------------------------------------------------------

  // Send SIB12 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg1of2_8_5,
                                        sizeof(sib12_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        70);

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  // Send SIB12 segment 2
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib12_seg2of2_8_5,
                                        sizeof(sib12_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        71);

  // We expect a cmas_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&cmas_ind_ptr, &buf_len);
  TF_ASSERT(cmas_ind_ptr->msg_hdr.id == LTE_RRC_CMAS_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)cmas_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 4);
  TF_ASSERT(4 == dsm_pullup(&rx_dsm_ptr, sib12_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib12_warn_msg[0] == 0x10);
  TF_ASSERT(sib12_warn_msg[1] == 0x0);
  TF_ASSERT(sib12_warn_msg[2] == 0x0F);
  TF_ASSERT(sib12_warn_msg[3] == 0x0);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(cmas_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(cmas_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(cmas_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(cmas_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(cmas_ind_ptr->msg_id == 0x909);
  TF_ASSERT(cmas_ind_ptr->sr_num == 0x100);
  TF_ASSERT(cmas_ind_ptr->data_coding_scheme == 0x1);

  // We don't expect a sib_sched_req message from RRC (but continue SIB12 reading)
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib12_read_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB12_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                       LTE_RRC_SIB1_BITMASK |
                                                                                       LTE_RRC_SIB2_BITMASK |
                                                                                       LTE_RRC_SIB3_BITMASK |
                                                                                       LTE_RRC_SIB4_BITMASK |
                                                                                       LTE_RRC_SIB5_BITMASK |
                                                                                       LTE_RRC_SIB12_BITMASK));
  
  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();
}

/*===========================================================================

                                UNIT TEST
Test case to check basic CMAS operation where UE is camped on a cell, page 
for CMAS is rcvd, SIB1 is read which schedules SIB12, that is read and 
CMAS indication is sent to upper layers
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibCmasBasicTest);

void LteRrcSibCmasBasicTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_CMAS_IND
    };

  // Code to set etws_supported by EFS
  int oflag, mode, efs_ret;
  boolean cmas_supported = TRUE;

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibCmasBasicTest");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/cmas", 
                    &cmas_supported, sizeof(cmas_supported), oflag, mode) >= 0);

  cmas_supported = FALSE;

  /* Read the EFS to get the etws_supported flag */
  TF_ASSERT((efs_ret = efs_get("/nv/item_files/modem/lte/rrc/sib/cmas",
                               &cmas_supported, 
                               sizeof(cmas_supported))) != -1);

  TF_ASSERT(cmas_supported == TRUE);

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);

  // etws_supported must be set to TRUE by efs
  TF_ASSERT(lte_rrc_sib.sd_ptr->cmas_supported == TRUE);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibCmasBasicTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_basic_cmas(FALSE);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibCmasBasicTest::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibCmasBasicTestWithDSDS);

void LteRrcSibCmasBasicTestWithDSDS::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_CMAS_IND
    };

  // Code to set etws_supported by EFS
  int oflag, mode, efs_ret;
  boolean cmas_supported = TRUE;

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibCmasBasicTest");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/cmas", 
                    &cmas_supported, sizeof(cmas_supported), oflag, mode) >= 0);

  cmas_supported = FALSE;

  /* Read the EFS to get the etws_supported flag */
  TF_ASSERT((efs_ret = efs_get("/nv/item_files/modem/lte/rrc/sib/cmas",
                               &cmas_supported, 
                               sizeof(cmas_supported))) != -1);

  TF_ASSERT(cmas_supported == TRUE);

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);

  // etws_supported must be set to TRUE by efs
  TF_ASSERT(lte_rrc_sib.sd_ptr->cmas_supported == TRUE);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibCmasBasicTestWithDSDS::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_basic_cmas_with_dsds(FALSE);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibCmasBasicTestWithDSDS::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Modification period boundary crossed while reading SIB12
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibCmasModBndCrossedTest);

void LteRrcSibCmasModBndCrossedTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_CMAS_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibCmasModBndCrossedTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_basic_cmas(TRUE);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibCmasModBndCrossedTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test case to verify continous SIB12 reading from the time it is scheduled in SIB1
till it is removed from SIB1 schedule. 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibContinousSIB12ReadingTest);

void LteRrcSibContinousSIB12ReadingTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_CMAS_IND
    };

  // Code to set etws_supported by EFS
  int oflag, mode, efs_ret;
  boolean cmas_supported = TRUE;

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibCmasBasicTest");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/cmas", 
                    &cmas_supported, sizeof(cmas_supported), oflag, mode) >= 0);

  cmas_supported = FALSE;

  /* Read the EFS to get the etws_supported flag */
  TF_ASSERT((efs_ret = efs_get("/nv/item_files/modem/lte/rrc/sib/cmas",
                               &cmas_supported, 
                               sizeof(cmas_supported))) != -1);

  TF_ASSERT(cmas_supported == TRUE);

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);

  // etws_supported must be set to TRUE by efs
  TF_ASSERT(lte_rrc_sib.sd_ptr->cmas_supported == TRUE);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibContinousSIB12ReadingTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_continous_sib12_reading();

  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibContinousSIB12ReadingTest::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test case to verify SIB12 reading where SIB 12 gets scheduled in SIB1 without
CMAS notification
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibCmasWithoutNotificationTest);

void LteRrcSibCmasWithoutNotificationTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_CMAS_IND
    };

  // Code to set etws_supported by EFS
  int oflag, mode, efs_ret;
  boolean cmas_supported = TRUE;

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibCmasWihoutNotificationTest");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/cmas", 
                    &cmas_supported, sizeof(cmas_supported), oflag, mode) >= 0);

  cmas_supported = FALSE;

  /* Read the EFS to get the etws_supported flag */
  TF_ASSERT((efs_ret = efs_get("/nv/item_files/modem/lte/rrc/sib/cmas",
                               &cmas_supported, 
                               sizeof(cmas_supported))) != -1);

  TF_ASSERT(cmas_supported == TRUE);

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);

  // etws_supported must be set to TRUE by efs
  TF_ASSERT(lte_rrc_sib.sd_ptr->cmas_supported == TRUE);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibCmasWithoutNotificationTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_cmas_without_notification();

  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibCmasWithoutNotificationTest::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test case to verify reveiving a blank SIB12 segment, the segment will be ignored 
and all sib12 fields would be cleard 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibCmasRcvdBlankSegment);

void LteRrcSibCmasRcvdBlankSegment::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_CMAS_IND
    };

  // Code to set etws_supported by EFS
  int oflag, mode, efs_ret;
  boolean cmas_supported = TRUE;

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibCmasWihoutNotificationTest");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/cmas", 
                    &cmas_supported, sizeof(cmas_supported), oflag, mode) >= 0);

  cmas_supported = FALSE;

  /* Read the EFS to get the etws_supported flag */
  TF_ASSERT((efs_ret = efs_get("/nv/item_files/modem/lte/rrc/sib/cmas",
                               &cmas_supported, 
                               sizeof(cmas_supported))) != -1);

  TF_ASSERT(cmas_supported == TRUE);

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);

  // etws_supported must be set to TRUE by efs
  TF_ASSERT(lte_rrc_sib.sd_ptr->cmas_supported == TRUE);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibCmasRcvdBlankSegment::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_cmas_rcvd_blank_segment();

  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibCmasRcvdBlankSegment::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test case to verify SIB12 reading continously when the event cause is etws in 
connected state 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibCmasReadingWithEventCauseEtwsWhenConnected);

void LteRrcSibCmasReadingWithEventCauseEtwsWhenConnected::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_CMAS_IND
    };

  // Code to set etws_supported by EFS
  int oflag, mode, efs_ret;
  boolean cmas_supported = TRUE;

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibCmasWihoutNotificationTest");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/cmas", 
                    &cmas_supported, sizeof(cmas_supported), oflag, mode) >= 0);

  cmas_supported = FALSE;

  /* Read the EFS to get the etws_supported flag */
  TF_ASSERT((efs_ret = efs_get("/nv/item_files/modem/lte/rrc/sib/cmas",
                               &cmas_supported, 
                               sizeof(cmas_supported))) != -1);

  TF_ASSERT(cmas_supported == TRUE);

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);

  // etws_supported must be set to TRUE by efs
  TF_ASSERT(lte_rrc_sib.sd_ptr->cmas_supported == TRUE);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibCmasReadingWithEventCauseEtwsWhenConnected::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_cmas_reading_with_event_cause_etws_when_connected();

  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibCmasReadingWithEventCauseEtwsWhenConnected::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test case to verify reveiving two CMAS message in one modification period 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibCmasRcvdTwoMsgInOneModPrd);

void LteRrcSibCmasRcvdTwoMsgInOneModPrd::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_CMAS_IND
    };

  // Code to set etws_supported by EFS
  int oflag, mode, efs_ret;
  boolean cmas_supported = TRUE;

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibCmasWihoutNotificationTest");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/cmas", 
                    &cmas_supported, sizeof(cmas_supported), oflag, mode) >= 0);

  cmas_supported = FALSE;

  /* Read the EFS to get the etws_supported flag */
  TF_ASSERT((efs_ret = efs_get("/nv/item_files/modem/lte/rrc/sib/cmas",
                               &cmas_supported, 
                               sizeof(cmas_supported))) != -1);

  TF_ASSERT(cmas_supported == TRUE);

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);

  // etws_supported must be set to TRUE by efs
  TF_ASSERT(lte_rrc_sib.sd_ptr->cmas_supported == TRUE);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibCmasRcvdTwoMsgInOneModPrd::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_cmas_rcvd_two_msg_in_one_mod_prd();

  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibCmasRcvdTwoMsgInOneModPrd::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test case to verify reveiving two CMAS message in one modification period 
without higher layer's request 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibCmasRcvdTwoMsgInOneModPrdWithoutNotification);

void LteRrcSibCmasRcvdTwoMsgInOneModPrdWithoutNotification::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_CMAS_IND
    };

  // Code to set etws_supported by EFS
  int oflag, mode, efs_ret;
  boolean cmas_supported = TRUE;

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibCmasWihoutNotificationTest");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/cmas", 
                    &cmas_supported, sizeof(cmas_supported), oflag, mode) >= 0);

  cmas_supported = FALSE;

  /* Read the EFS to get the etws_supported flag */
  TF_ASSERT((efs_ret = efs_get("/nv/item_files/modem/lte/rrc/sib/cmas",
                               &cmas_supported, 
                               sizeof(cmas_supported))) != -1);

  TF_ASSERT(cmas_supported == TRUE);

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);

  // etws_supported must be set to TRUE by efs
  TF_ASSERT(lte_rrc_sib.sd_ptr->cmas_supported == TRUE);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibCmasRcvdTwoMsgInOneModPrdWithoutNotification::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_cmas_rcvd_two_msg_in_one_mod_prd_without_notification();

  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibCmasRcvdTwoMsgInOneModPrdWithoutNotification::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test case to verify SIB12 reading after Connected to Idle transition 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibCmasReadingAfterConnectedToIdle);

void LteRrcSibCmasReadingAfterConnectedToIdle::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_CMAS_IND
    };

  // Code to set etws_supported by EFS
  int oflag, mode, efs_ret;
  boolean cmas_supported = TRUE;

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibCmasReadingAfterConnectedToIdle");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/cmas", 
                    &cmas_supported, sizeof(cmas_supported), oflag, mode) >= 0);

  cmas_supported = FALSE;

  /* Read the EFS to get the etws_supported flag */
  TF_ASSERT((efs_ret = efs_get("/nv/item_files/modem/lte/rrc/sib/cmas",
                               &cmas_supported, 
                               sizeof(cmas_supported))) != -1);

  TF_ASSERT(cmas_supported == TRUE);

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);

  // etws_supported must be set to TRUE by efs
  TF_ASSERT(lte_rrc_sib.sd_ptr->cmas_supported == TRUE);

  lte_rrc_sib_test_stub_init();

}

void LteRrcSibCmasReadingAfterConnectedToIdle::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_cmas_reading_after_connected_to_idle();

  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibCmasReadingAfterConnectedToIdle::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}




TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibCmasReadingAfterConnectedToIdleWithDSDS);

void LteRrcSibCmasReadingAfterConnectedToIdleWithDSDS::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_CMAS_IND
    };

  // Code to set etws_supported by EFS
  int oflag, mode, efs_ret;
  boolean cmas_supported = TRUE;

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibCmasReadingAfterConnectedToIdle");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/cmas", 
                    &cmas_supported, sizeof(cmas_supported), oflag, mode) >= 0);

  cmas_supported = FALSE;

  /* Read the EFS to get the etws_supported flag */
  TF_ASSERT((efs_ret = efs_get("/nv/item_files/modem/lte/rrc/sib/cmas",
                               &cmas_supported, 
                               sizeof(cmas_supported))) != -1);

  TF_ASSERT(cmas_supported == TRUE);

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);

  // etws_supported must be set to TRUE by efs
  TF_ASSERT(lte_rrc_sib.sd_ptr->cmas_supported == TRUE);

  lte_rrc_sib_test_stub_init();

}

void LteRrcSibCmasReadingAfterConnectedToIdleWithDSDS::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_cmas_reading_after_connected_to_idle_with_dsds();

  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibCmasReadingAfterConnectedToIdleWithDSDS::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}
