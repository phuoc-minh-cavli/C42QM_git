/*!
  @file lte_rrc_sib_etws_tests.cpp

  @brief
  ETWS related SIB unit test cases
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_etws_tests.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/22/10   amit    Initial Version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "__lte_rrc_sib.h"
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
DECLARE_SIB1_2345_10_11_MCC_8_5;
DECLARE_SIB10_8_5;
DECLARE_SIB11_SEG1OF2_8_5;
DECLARE_SIB11_SEG2OF2_8_5;
DECLARE_SIB11_SEG1OF2_NEW_MSG_ID_8_5;
DECLARE_SIB11_SEG2OF2_NEW_MSG_ID_8_5;
DECLARE_SIB11_8_5;
DECLARE_SIB11_SEG3EXTRA_8_5;
DECLARE_SIB10_SIB3_8_5;
DECLARE_SIB2_SIB11_SEG1OF2_8_5;
DECLARE_SIB2_SIB11_SEG2OF2_8_5;
DECLARE_SIB11_SEG2OF2_1246_8_5;
DECLARE_SIB11_SEG2OF2_1247_8_5;
DECLARE_SIB11_SEG_LEN_1246_8_5;
DECLARE_SIB11_SEG_LEN_1247_8_5;
DECLARE_SIB11_MULTIPLE_SEG_8_5;
DECLARE_SIB11_8_NULL_MSGSEG;

extern "C"
{
extern lte_rrc_sib_s lte_rrc_sib;
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern boolean lte_rrc_sib_test_camped;
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

/* Test case to check basic ETWS operation where cell is camped on a cell, page 
   for ETWS is rcvd, SIB1 is read which schedules SIB10 and 11, both are read and 
   indications are sent to upper layers */
/* Boolean mod_prd_crossed indicates if mod prd boundary crossed while reading 
   SIB11 */
void lte_rrc_sib_test_basic_etws(boolean mod_bnd_crossed)
{

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[10];

  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;

  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 1;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 3000;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 5000;
  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
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
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
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

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);
  
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
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
  
    // ----------------------------------------------------------------------

  }

  // Send SIB1 that schedules SIB10 and SIB11
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
                                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 3);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);
  
    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);
  
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
  
    // ----------------------------------------------------------------------

  }

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[10] == 3);
    for(int i = 2; i < 10; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
    }
    for(int i = 11; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
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
                                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                   LTE_RRC_SIB1_BITMASK |
                                                                   LTE_RRC_SIB2_BITMASK |
                                                                   LTE_RRC_SIB3_BITMASK |
                                                                   LTE_RRC_SIB4_BITMASK |
                                                                   LTE_RRC_SIB5_BITMASK |
                                                                   LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                        LTE_RRC_SIB1_BITMASK |
                                                                        LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK |
                                                                        LTE_RRC_SIB4_BITMASK |
                                                                        LTE_RRC_SIB5_BITMASK |
                                                                        LTE_RRC_SIB10_BITMASK));
  
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 3);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);
  
    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);

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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK |
                                                                 LTE_RRC_SIB10_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib10_8_5));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib10);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
  
    // ----------------------------------------------------------------------

  }

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // Send same segment multiple times - there should be no memory leak and no messages sent
  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[10] == 3);
    for(int i = 2; i < 10; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
    }
    for(int i = 11; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
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
                                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                   LTE_RRC_SIB1_BITMASK |
                                                                   LTE_RRC_SIB2_BITMASK |
                                                                   LTE_RRC_SIB3_BITMASK |
                                                                   LTE_RRC_SIB4_BITMASK |
                                                                   LTE_RRC_SIB5_BITMASK |
                                                                   LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                        LTE_RRC_SIB1_BITMASK |
                                                                        LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK |
                                                                        LTE_RRC_SIB4_BITMASK |
                                                                        LTE_RRC_SIB5_BITMASK |
                                                                        LTE_RRC_SIB10_BITMASK));
  
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);
  
    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0x1100);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_ptr != NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_ptr[0] == 0x11);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_ptr[1] == 0x11);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_len == 2);
    for(int i = 1; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);

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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK |
                                                                 LTE_RRC_SIB10_BITMASK |
                                                                 LTE_RRC_SIB11_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib10_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2] != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[3] == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_size == sizeof(sib11_seg1of2_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3] != NULL);
    for(int i = 4; i < LTE_RRC_SIB_MAX_SIS; i++)
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib10);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib11);
  
    // ----------------------------------------------------------------------

  }

  if(!mod_bnd_crossed)
  {
    // Send SIB11 segment 2 (last segment)
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_8_5,
                                          sizeof(sib11_seg2of2_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          5);

    // We expect a etws_sec_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
    TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);
  
    att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
    msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
    TF_ASSERT(rx_dsm_ptr != NULL);
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
    TF_ASSERT(dsm_packet_length == 5);
    TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
    TF_ASSERT(rx_dsm_ptr == NULL);
  
    TF_ASSERT(sib11_warn_msg[0] == 0x11);
    TF_ASSERT(sib11_warn_msg[1] == 0x11);
    TF_ASSERT(sib11_warn_msg[2] == 0x22);
    TF_ASSERT(sib11_warn_msg[3] == 0x22);
    TF_ASSERT(sib11_warn_msg[4] == 0x22);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
    TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
    TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);
  
    // We expect a sib_sched_req message from RRC (to stop SIB reading)
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 5);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
  
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_1);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 2);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                   LTE_RRC_SIB3_BITMASK |
                                                                   LTE_RRC_SIB4_BITMASK |
                                                                   LTE_RRC_SIB5_BITMASK |
                                                                   LTE_RRC_SIB10_BITMASK |
                                                                   LTE_RRC_SIB11_BITMASK));
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB10_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib10_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2] != NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[3] == (LTE_RRC_SIB11_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_size == sizeof(sib11_seg2of2_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3] != NULL);
      for(int i = 4; i < LTE_RRC_SIB_MAX_SIS; i++)
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == 
                lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib10);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == 
                lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib11);
    
      // ----------------------------------------------------------------------
  
    }
  } // Mod bnd not crossed
  else // Mod bnd crossed
  {
    // Send SIB11 segment 2 (last segment)
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_8_5,
                                          sizeof(sib11_seg2of2_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          64);

    // We expect a etws_sec_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
    TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);
  
    att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
    msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
    TF_ASSERT(rx_dsm_ptr != NULL);
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
    TF_ASSERT(dsm_packet_length == 5);
    TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
    TF_ASSERT(rx_dsm_ptr == NULL);
  
    TF_ASSERT(sib11_warn_msg[0] == 0x11);
    TF_ASSERT(sib11_warn_msg[1] == 0x11);
    TF_ASSERT(sib11_warn_msg[2] == 0x22);
    TF_ASSERT(sib11_warn_msg[3] == 0x22);
    TF_ASSERT(sib11_warn_msg[4] == 0x22);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
    TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
    TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);
  
    // We expect a sib_sched_req message from RRC (to stop SIB reading)
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
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
  
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
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);

      // ----------------------------------------------------------------------

      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[10] == 3);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[11] == 64);
      for(int i = 2; i < 10; i++)
      {
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
      }
      for(int i = 12; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
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
                                                                           LTE_RRC_SIB_ETWS_SIBS_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                     LTE_RRC_SIB1_BITMASK |
                                                                     LTE_RRC_SIB2_BITMASK |
                                                                     LTE_RRC_SIB3_BITMASK |
                                                                     LTE_RRC_SIB4_BITMASK |
                                                                     LTE_RRC_SIB5_BITMASK |
                                                                     LTE_RRC_SIB10_BITMASK |
                                                                     LTE_RRC_SIB11_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_SIB10_BITMASK |
                                                                          LTE_RRC_SIB11_BITMASK));

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
                                                                                   LTE_RRC_SIB5_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 128);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                   LTE_RRC_SIB1_BITMASK |
                                                                                   LTE_RRC_SIB2_BITMASK |
                                                                                   LTE_RRC_SIB3_BITMASK |
                                                                                   LTE_RRC_SIB4_BITMASK |
                                                                                   LTE_RRC_SIB5_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 64);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 4);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                            LTE_RRC_SIB3_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                            LTE_RRC_SIB5_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);

      // ----------------------------------------------------------------------

      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == FALSE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
      for(int i = 0; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
      {
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
      }
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);

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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                   LTE_RRC_SIB3_BITMASK |
                                                                   LTE_RRC_SIB4_BITMASK |
                                                                   LTE_RRC_SIB5_BITMASK |
                                                                   LTE_RRC_SIB10_BITMASK |
                                                                   LTE_RRC_SIB11_BITMASK));
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB10_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib10_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2] != NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[3] == (LTE_RRC_SIB11_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_size == sizeof(sib11_seg2of2_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3] != NULL);
      for(int i = 4; i < LTE_RRC_SIB_MAX_SIS; i++)
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == 
                lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib10);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == 
                lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib11);
    
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
                                                           LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
  
    // send SIB1 to RRC
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                          sizeof(sib1_2345_10_11_mcc_8_5),
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
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  } // Mod bnd crossed

}

void lte_rrc_sib_test_basic_etws_with_dsds(boolean mod_bnd_crossed)
{

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[10];

  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;

  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 1;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 3000;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 5000;
  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
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
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
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

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);
  
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
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
  
    // ----------------------------------------------------------------------

  }

  // Send SIB1 that schedules SIB10 and SIB11
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));


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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
                                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 3);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);
  
    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);
  
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
  
    // ----------------------------------------------------------------------

  }

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_MSG("Sending RF_UNAVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 17;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
  lte_rrc_sib_test_send_rf_unavail_ind();

  lte_rrc_utf_wait_for_done();

  TF_SLEEP(100);
  TF_MSG("Sending RF_AVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 21;
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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
														 LTE_RRC_SIB10_BITMASK |  // WE DO NOT EXPECT SIB10 IN THIS SIB_SCHED_REQ. BUT IT IS STILL BEING SET
                                                         LTE_RRC_SIB11_BITMASK));


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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[10] == 3);
    for(int i = 2; i < 10; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
    }
    for(int i = 11; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
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
                                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                   LTE_RRC_SIB1_BITMASK |
                                                                   LTE_RRC_SIB2_BITMASK |
                                                                   LTE_RRC_SIB3_BITMASK |
                                                                   LTE_RRC_SIB4_BITMASK |
                                                                   LTE_RRC_SIB5_BITMASK |
                                                                   LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                        LTE_RRC_SIB1_BITMASK |
                                                                        LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK |
                                                                        LTE_RRC_SIB4_BITMASK |
                                                                        LTE_RRC_SIB5_BITMASK |
                                                                        LTE_RRC_SIB10_BITMASK));
  
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
																				 LTE_RRC_SIB10_BITMASK | //SIB10 SHOULD NOT BE SET. BUT IT IS STILL BEING SET
                                                                                 LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 3);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);
  
    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);

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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK |
                                                                 LTE_RRC_SIB10_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib10_8_5));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib10);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
  
    // ----------------------------------------------------------------------

  }

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // Send same segment multiple times - there should be no memory leak and no messages sent
  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));


  // No sched req should be sent. But currently there is a sched req being sent
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[10] == 3);
    for(int i = 2; i < 10; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
    }
    for(int i = 11; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
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
                                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                   LTE_RRC_SIB1_BITMASK |
                                                                   LTE_RRC_SIB2_BITMASK |
                                                                   LTE_RRC_SIB3_BITMASK |
                                                                   LTE_RRC_SIB4_BITMASK |
                                                                   LTE_RRC_SIB5_BITMASK |
                                                                   LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                        LTE_RRC_SIB1_BITMASK |
                                                                        LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK |
                                                                        LTE_RRC_SIB4_BITMASK |
                                                                        LTE_RRC_SIB5_BITMASK |
                                                                        LTE_RRC_SIB10_BITMASK));
  
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);
  
    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0x1100);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_ptr != NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_ptr[0] == 0x11);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_ptr[1] == 0x11);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_len == 2);
    for(int i = 1; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);

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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK |
                                                                 LTE_RRC_SIB10_BITMASK |
                                                                 LTE_RRC_SIB11_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib10_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2] != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[3] == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_size == sizeof(sib11_seg1of2_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3] != NULL);
    for(int i = 4; i < LTE_RRC_SIB_MAX_SIS; i++)
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib10);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib11);
  
    // ----------------------------------------------------------------------

  }

  if(!mod_bnd_crossed)
  {
    // Send SIB11 segment 2 (last segment)
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_8_5,
                                          sizeof(sib11_seg2of2_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          5);

    // We expect a etws_sec_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
    TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);
  
    att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
    msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
    TF_ASSERT(rx_dsm_ptr != NULL);
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
    TF_ASSERT(dsm_packet_length == 5);
    TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
    TF_ASSERT(rx_dsm_ptr == NULL);
  
    TF_ASSERT(sib11_warn_msg[0] == 0x11);
    TF_ASSERT(sib11_warn_msg[1] == 0x11);
    TF_ASSERT(sib11_warn_msg[2] == 0x22);
    TF_ASSERT(sib11_warn_msg[3] == 0x22);
    TF_ASSERT(sib11_warn_msg[4] == 0x22);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
    TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
    TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);
  
    // We expect a sib_sched_req message from RRC (to stop SIB reading)
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 5);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
  


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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_1);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 2);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                   LTE_RRC_SIB3_BITMASK |
                                                                   LTE_RRC_SIB4_BITMASK |
                                                                   LTE_RRC_SIB5_BITMASK |
                                                                   LTE_RRC_SIB10_BITMASK |
                                                                   LTE_RRC_SIB11_BITMASK));
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB10_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib10_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2] != NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[3] == (LTE_RRC_SIB11_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_size == sizeof(sib11_seg2of2_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3] != NULL);
      for(int i = 4; i < LTE_RRC_SIB_MAX_SIS; i++)
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == 
                lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib10);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == 
                lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib11);
    
      // ----------------------------------------------------------------------
  
    }
  } // Mod bnd not crossed
  else // Mod bnd crossed
  {
    // Send SIB11 segment 2 (last segment)
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_8_5,
                                          sizeof(sib11_seg2of2_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          64);

    // We expect a etws_sec_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
    TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);
  
    att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
    msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
    TF_ASSERT(rx_dsm_ptr != NULL);
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
    TF_ASSERT(dsm_packet_length == 5);
    TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
    TF_ASSERT(rx_dsm_ptr == NULL);
  
    TF_ASSERT(sib11_warn_msg[0] == 0x11);
    TF_ASSERT(sib11_warn_msg[1] == 0x11);
    TF_ASSERT(sib11_warn_msg[2] == 0x22);
    TF_ASSERT(sib11_warn_msg[3] == 0x22);
    TF_ASSERT(sib11_warn_msg[4] == 0x22);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
    TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
    TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);
  
    // We expect a sib_sched_req message from RRC (to stop SIB reading)
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
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
  

	lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
 TF_MSG("Sending RF_UNAVAILABLE_IND");
 lte_rrc_sib_test_rf_status_sfn = 25;
 lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
 lte_rrc_sib_test_send_rf_unavail_ind();

 lte_rrc_utf_wait_for_done();

 TF_SLEEP(100);
 TF_MSG("Sending RF_AVAILABLE_IND");
 lte_rrc_sib_test_rf_status_sfn = 29;
 lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
 lte_rrc_sib_test_send_rf_avail_ind();

 lte_rrc_utf_wait_for_done();

 lte_rrc_sib_test_wt_tmr_after_rf_available = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
 TF_ASSERT(lte_rrc_sib_test_wt_tmr_after_rf_available == lte_rrc_sib_test_wt_tmr_before_rf_unavailable);


     // We expect a sib_sched_req message from RRC (to stop SIB reading)
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
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);

      // ----------------------------------------------------------------------

      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[10] == 3);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[11] == 64);
      for(int i = 2; i < 10; i++)
      {
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
      }
      for(int i = 12; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
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
                                                                           LTE_RRC_SIB_ETWS_SIBS_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                     LTE_RRC_SIB1_BITMASK |
                                                                     LTE_RRC_SIB2_BITMASK |
                                                                     LTE_RRC_SIB3_BITMASK |
                                                                     LTE_RRC_SIB4_BITMASK |
                                                                     LTE_RRC_SIB5_BITMASK |
                                                                     LTE_RRC_SIB10_BITMASK |
                                                                     LTE_RRC_SIB11_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_SIB10_BITMASK |
                                                                          LTE_RRC_SIB11_BITMASK));

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
                                                                                   LTE_RRC_SIB5_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 128);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                   LTE_RRC_SIB1_BITMASK |
                                                                                   LTE_RRC_SIB2_BITMASK |
                                                                                   LTE_RRC_SIB3_BITMASK |
                                                                                   LTE_RRC_SIB4_BITMASK |
                                                                                   LTE_RRC_SIB5_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 64);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 4);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                            LTE_RRC_SIB3_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                            LTE_RRC_SIB5_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);

      // ----------------------------------------------------------------------

      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == FALSE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
      for(int i = 0; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
      {
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
        TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
      }
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);

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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                   LTE_RRC_SIB3_BITMASK |
                                                                   LTE_RRC_SIB4_BITMASK |
                                                                   LTE_RRC_SIB5_BITMASK |
                                                                   LTE_RRC_SIB10_BITMASK |
                                                                   LTE_RRC_SIB11_BITMASK));
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB10_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib10_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2] != NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[3] == (LTE_RRC_SIB11_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_size == sizeof(sib11_seg2of2_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3] != NULL);
      for(int i = 4; i < LTE_RRC_SIB_MAX_SIS; i++)
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == 
                lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib10);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == 
                lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib11);
    
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
                                                           LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
  
    // send SIB1 to RRC
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                          sizeof(sib1_2345_10_11_mcc_8_5),
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
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  } // Mod bnd crossed

}


/* Extra segments for SIB11 are sent and SIB11 is rcvd before SIB10 */
void lte_rrc_sib_test_etws_extra_segments()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[10];

  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;

  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 1;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 3000;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 5000;
  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
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

  // ---------------------------------------------------------------

  // Send SIB1 that schedules SIB10 and SIB11
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // ---------------------------------------------------------------

  // Send SIB11 segment 3
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg3extra_8_5,
                                        sizeof(sib11_seg3extra_8_5),
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
                                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB10_BITMASK |
                                                                                 LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB10_BITMASK |
                                                                                 LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);
  
    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0x1100);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[2].seg_ptr != NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[2].seg_ptr[0] == 0x33);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[2].seg_ptr[1] == 0x33);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[2].seg_ptr[2] == 0x33);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[2].seg_ptr[3] == 0x33);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[2].seg_len == 4);
    for(int i = 0; i < 2; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    for(int i = 3; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);

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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK |
                                                                 LTE_RRC_SIB11_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib11_seg3extra_8_5));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib11);
  
    // ----------------------------------------------------------------------

  }

  // ----------------------------------------------------------------------

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
                                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB10_BITMASK |
                                                                                 LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB10_BITMASK |
                                                                                 LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);
  
    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0x1100);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_ptr != NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_ptr[0] == 0x11);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_ptr[1] == 0x11);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_len == 2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[2].seg_ptr != NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[2].seg_ptr[0] == 0x33);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[2].seg_ptr[1] == 0x33);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[2].seg_ptr[2] == 0x33);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[2].seg_ptr[3] == 0x33);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[2].seg_len == 4);
    for(int i = 1; i < 2; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    for(int i = 3; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);

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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK |
                                                                 LTE_RRC_SIB11_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib11_seg1of2_8_5));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib11);
  
    // ----------------------------------------------------------------------

  }

  // Send SIB11 segment 2 (last segment)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_8_5,
                                        sizeof(sib11_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // We expect a etws_sec_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
  TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 5);
  TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib11_warn_msg[0] == 0x11);
  TF_ASSERT(sib11_warn_msg[1] == 0x11);
  TF_ASSERT(sib11_warn_msg[2] == 0x22);
  TF_ASSERT(sib11_warn_msg[3] == 0x22);
  TF_ASSERT(sib11_warn_msg[4] == 0x22);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_sec_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);

  // ---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // ---------------------------------------------------------------

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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[11] == 4);
    for(int i = 2; i < 11; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
    }
    for(int i = 12; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
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
                                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                   LTE_RRC_SIB1_BITMASK |
                                                                   LTE_RRC_SIB2_BITMASK |
                                                                   LTE_RRC_SIB3_BITMASK |
                                                                   LTE_RRC_SIB4_BITMASK |
                                                                   LTE_RRC_SIB5_BITMASK |
                                                                   LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                        LTE_RRC_SIB1_BITMASK |
                                                                        LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK |
                                                                        LTE_RRC_SIB4_BITMASK |
                                                                        LTE_RRC_SIB5_BITMASK |
                                                                        LTE_RRC_SIB11_BITMASK));
  
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);
  
    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);

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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK |
                                                                 LTE_RRC_SIB11_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib11_seg2of2_8_5));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib11);
  
    // ----------------------------------------------------------------------

  }

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        5);

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);

  // ----------------------------------------------------------------------

  // We expect a sib_sched_req message from RRC (to stop SIB reading)
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 5);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 2);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK |
                                                                 LTE_RRC_SIB10_BITMASK |
                                                                 LTE_RRC_SIB11_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib11_seg2of2_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2] != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[3] == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_size == sizeof(sib10_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3] != NULL);
    for(int i = 4; i < LTE_RRC_SIB_MAX_SIS; i++)
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib10);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib11);
  
    // ----------------------------------------------------------------------

  }

}

/* ETWS indication in connected mode */
void lte_rrc_sib_test_etws_connected()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[10];

  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;

  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 1;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 3000;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 5000;
  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  lte_rrc_sib_test_send_connected_indi();

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
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
                                                         LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ---------------------------------------------------------------

  // Send SIB1 that schedules SIB10 and SIB11
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // ---------------------------------------------------------------

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // ---------------------------------------------------------------

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);

  // ---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // ---------------------------------------------------------------

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // ---------------------------------------------------------------

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  // ---------------------------------------------------------------

  // Send SIB11 segment 2 (last segment)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_8_5,
                                        sizeof(sib11_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        5);

  // We expect a etws_sec_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
  TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 5);
  TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib11_warn_msg[0] == 0x11);
  TF_ASSERT(sib11_warn_msg[1] == 0x11);
  TF_ASSERT(sib11_warn_msg[2] == 0x22);
  TF_ASSERT(sib11_warn_msg[3] == 0x22);
  TF_ASSERT(sib11_warn_msg[4] == 0x22);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_sec_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);

  // We expect a sib_sched_req message from RRC (to stop SIB reading)
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 5);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
}

/* enum used to indicate what conditions apply */
typedef enum
{
  LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_INFO_PRESENT,
  LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_READING_ACTIVE_ID_CHANGES,
  LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_READING_ACTIVE_ID_SAME,
  LTE_RRC_SIB_TEST_ETWS_TIMER_EXPIRY,
  LTE_RRC_SIB_TEST_ETWS_PAGED_INVALID
} lte_rrc_sib_test_etws_paged_e;

/* Camping on a cell that already transmits ETWS SIBs */
void lte_rrc_sib_test_etws_camp(lte_rrc_sib_test_etws_paged_e paged)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[10];

  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;

  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 1;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 3000;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 5000;
  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();

  // Send SIB10 - Since PLMN is not known yet (SIB1 is not rcvd), this SIB should be ignored
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // No mesages should be sent
  lte_rrc_utf_check_no_more_msgs();

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
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
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

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
  lte_rrc_sib_test_serv_cell_phy_cell_id = PHY_CELL_ID_1;
  lte_rrc_sib_test_serv_cell_freq = FREQ_1;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  //---------------------------------------------------------------

  // Timer expiry test
  if(paged == LTE_RRC_SIB_TEST_ETWS_TIMER_EXPIRY)
  {
    TF_SLEEP(LTE_RRC_SIB_NON_MAND_SIBS_WT_TMR);

    //---------------------------------------------------------------

    // We expect timer expiry
    lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
    TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

    //---------------------------------------------------------------

    // sib read error
    lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
    TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));
    TF_ASSERT(read_error_ind_ptr->csg_indication == TRUE);
    TF_ASSERT(read_error_ind_ptr->intra_freq_resel_allowed == TRUE);

    //---------------------------------------------------------------

    // We expect a sib_sched_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

    //---------------------------------------------------------------

    // Asserts
    {
      /* Asserts on SIB internal data */
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == FALSE);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
      TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == LTE_RRC_SIB_DB_MAX_CELLS);
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
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                   LTE_RRC_SIB3_BITMASK |
                                                                   LTE_RRC_SIB10_BITMASK |
                                                                   LTE_RRC_SIB11_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB10_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib10_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[1] == (LTE_RRC_SIB2_BITMASK |
                                                                            LTE_RRC_SIB3_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_size == sizeof(sib2_sib3_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1] == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB11_BITMASK));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib11_seg1of2_8_5));
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2] == NULL);
      for(int i = 3; i < LTE_RRC_SIB_MAX_SIS; i++)
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
      //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
    
      // ----------------------------------------------------------------------
  
    } // Asserts

    return;

  }

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

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
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));

  //---------------------------------------------------------------

  // No sched req should be sent since SIB11 hasn't been received yet
  lte_rrc_utf_check_no_more_msgs();

  //---------------------------------------------------------------

  if(paged == LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_READING_ACTIVE_ID_SAME ||
     paged == LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_READING_ACTIVE_ID_CHANGES)
  {
    lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                        LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
                                        1);

    //---------------------------------------------------------------

    // We expect a sib_sched_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

    //---------------------------------------------------------------

    // send SIB1 to RRC
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                          sizeof(sib1_2345_10_11_mcc_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          2);

    //---------------------------------------------------------------

    // We expect a sib_sched_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

    //---------------------------------------------------------------

    // Send SIB10
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                          sizeof(sib10_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          3);

    // We expect a etws_prim_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
    TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
    TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
    TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
    TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
    TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);

    //---------------------------------------------------------------

    // We expect a sib_sched_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

    //---------------------------------------------------------------

  } // paged == LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_READING_ACTIVE_ID_SAME ||
    // paged == LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_READING_ACTIVE_ID_CHANGES

  //---------------------------------------------------------------

  if(paged != LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_READING_ACTIVE_ID_CHANGES)
  {
    // Send SIB11 segment 2 (last segment)
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_8_5,
                                          sizeof(sib11_seg2of2_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          4);
  
    // We expect a etws_sec_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
    TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);
  
    att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
    msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
    TF_ASSERT(rx_dsm_ptr != NULL);
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
    TF_ASSERT(dsm_packet_length == 5);
    TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
    TF_ASSERT(rx_dsm_ptr == NULL);
  
    TF_ASSERT(sib11_warn_msg[0] == 0x11);
    TF_ASSERT(sib11_warn_msg[1] == 0x11);
    TF_ASSERT(sib11_warn_msg[2] == 0x22);
    TF_ASSERT(sib11_warn_msg[3] == 0x22);
    TF_ASSERT(sib11_warn_msg[4] == 0x22);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
    TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
    TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);
  } // paged != LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_READING_ACTIVE_ID_CHANGES
  else
  {
    // In this case when SIB11 is rcvd with new msg id/sr num, the old segment 
    // should be discarded

    // Send SIB11 segment 1
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_new_msg_id_8_5,
                                          sizeof(sib11_seg1of2_new_msg_id_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          4);

    // No sched req should be sent
    lte_rrc_utf_check_no_more_msgs();

    //---------------------------------------------------------------

    // Send SIB11 segment 2 (last segment)
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_new_msg_id_8_5,
                                          sizeof(sib11_seg2of2_new_msg_id_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          4);

    // We expect a etws_sec_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
    TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);

    att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
    msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
    TF_ASSERT(rx_dsm_ptr != NULL);
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
    TF_ASSERT(dsm_packet_length == 5);
    TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
    TF_ASSERT(rx_dsm_ptr == NULL);

    TF_ASSERT(sib11_warn_msg[0] == 0x11);
    TF_ASSERT(sib11_warn_msg[1] == 0x11);
    TF_ASSERT(sib11_warn_msg[2] == 0x22);
    TF_ASSERT(sib11_warn_msg[3] == 0x22);
    TF_ASSERT(sib11_warn_msg[4] == 0x22);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1111);
    TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
    TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);

    //---------------------------------------------------------------
  } // paged == LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_READING_ACTIVE_ID_CHANGES

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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

  /* Timer should have been restarted for MIB and SIB1 and duration is current 
     mod prd remaining time + MIB_SIB1 timer */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == 
            (LTE_RRC_SIB_MIB_SIB1_WT_TMR + 600));
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr) == 
            (LTE_RRC_SIB_MIB_SIB1_WT_TMR + 600));
  
  //---------------------------------------------------------------

  if(paged == LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_INFO_PRESENT)
  {
    lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                        LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
                                        1);

    //---------------------------------------------------------------

    // We expect a sib_sched_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

    //---------------------------------------------------------------

    // send SIB1 to RRC
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                          sizeof(sib1_2345_10_11_mcc_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          2);

    //---------------------------------------------------------------

    // We expect a sib_sched_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
  
    //---------------------------------------------------------------
  
    // Send SIB10
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                          sizeof(sib10_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          3);
  
    // We expect a etws_prim_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
    TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
    TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
    TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
    TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
    TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);
  
    //---------------------------------------------------------------
  
    // We expect a sib_sched_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK |
                                                           LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
  
    //---------------------------------------------------------------

    // Send SIB11 segment 1
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                          sizeof(sib11_seg1of2_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          4);
  
    // No sched req should be sent
    lte_rrc_utf_check_no_more_msgs();
  
    //---------------------------------------------------------------
  
    // Send SIB11 segment 2 (last segment)
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_8_5,
                                          sizeof(sib11_seg2of2_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          4);
  
    // We expect a etws_sec_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
    TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);
  
    att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
    msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
    TF_ASSERT(rx_dsm_ptr != NULL);
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
    TF_ASSERT(dsm_packet_length == 5);
    TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
    TF_ASSERT(rx_dsm_ptr == NULL);
  
    TF_ASSERT(sib11_warn_msg[0] == 0x11);
    TF_ASSERT(sib11_warn_msg[1] == 0x11);
    TF_ASSERT(sib11_warn_msg[2] == 0x22);
    TF_ASSERT(sib11_warn_msg[3] == 0x22);
    TF_ASSERT(sib11_warn_msg[4] == 0x22);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
    TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
    TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);
  
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
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                    LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                    LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
  } // paged == LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_INFO_PRESENT

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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

}

/* ETWS info already present (in db), event not active, paged again - msg_id/sr_num 
   does not change - When event is created, it should not have any ETWS info and 
   when paged all info should be read */
void lte_rrc_sib_test_etws_db()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[10];

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
                                      1);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
                                                                         LTE_RRC_SIB5_BITMASK |
                                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);
  
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK |
                                                                 LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib10_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[1] == (LTE_RRC_SIB2_BITMASK |
                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_size == sizeof(sib2_sib3_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1] != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib11_seg2of2_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2] != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[3] == (LTE_RRC_SIB4_BITMASK |
                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_size == sizeof(sib4_sib5_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3] != NULL);
    for(int i = 4; i < LTE_RRC_SIB_MAX_SIS; i++)
    {
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[i] == 0);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_size == 0);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[i] == NULL);
    }
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == TRUE);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib2);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib3_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib3);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib4_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib4);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib5_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib5);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib10);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib11);
  
    // ----------------------------------------------------------------------

  } // Asserts

  // ----------------------------------------------------------------------

  // Send SIB1 that schedules SIB10 and SIB11
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // ----------------------------------------------------------------------

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // ----------------------------------------------------------------------

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  // ----------------------------------------------------------------------

  //---------------------------------------------------------------

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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[1] == 3);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[10] == 3);
    for(int i = 0; i < 1; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
    }
    for(int i = 2; i < 10; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
    }
    for(int i = 11; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
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
                                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                   LTE_RRC_SIB1_BITMASK |
                                                                   LTE_RRC_SIB2_BITMASK |
                                                                   LTE_RRC_SIB3_BITMASK |
                                                                   LTE_RRC_SIB4_BITMASK |
                                                                   LTE_RRC_SIB5_BITMASK |
                                                                   LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                        LTE_RRC_SIB1_BITMASK |
                                                                        LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK |
                                                                        LTE_RRC_SIB4_BITMASK |
                                                                        LTE_RRC_SIB5_BITMASK |
                                                                        LTE_RRC_SIB10_BITMASK));
  
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);

    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == TRUE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0x1100);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_ptr != NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_ptr[0] == 0x11);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_ptr[1] == 0x11);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[0].seg_len == 2);
    for(int i = 1; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);
  
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK |
                                                                 LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib10_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[1] == (LTE_RRC_SIB2_BITMASK |
                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_size == sizeof(sib2_sib3_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1] != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib11_seg1of2_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2] != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[3] == (LTE_RRC_SIB4_BITMASK |
                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_size == sizeof(sib4_sib5_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3] != NULL);
    for(int i = 4; i < LTE_RRC_SIB_MAX_SIS; i++)
    {
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[i] == 0);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_size == 0);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[i] == NULL);
    }
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == TRUE);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib2);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib3_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib3);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib4_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib4);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib5_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib5);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib10);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib11);
  
    // ----------------------------------------------------------------------

  } // Asserts

  // Send SIB11 segment 2 (last segment)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_8_5,
                                        sizeof(sib11_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        5);

  // We expect a etws_sec_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
  TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 5);
  TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib11_warn_msg[0] == 0x11);
  TF_ASSERT(sib11_warn_msg[1] == 0x11);
  TF_ASSERT(sib11_warn_msg[2] == 0x22);
  TF_ASSERT(sib11_warn_msg[3] == 0x22);
  TF_ASSERT(sib11_warn_msg[4] == 0x22);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_sec_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);

  // We expect a sib_sched_req message from RRC (to stop SIB reading)
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 5);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 2);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK |
                                                                 LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib10_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[1] == (LTE_RRC_SIB2_BITMASK |
                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_size == sizeof(sib2_sib3_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1] != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib11_seg2of2_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2] != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[3] == (LTE_RRC_SIB4_BITMASK |
                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_size == sizeof(sib4_sib5_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3] != NULL);
    for(int i = 4; i < LTE_RRC_SIB_MAX_SIS; i++)
    {
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[i] == 0);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_size == 0);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[i] == NULL);
    }
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == TRUE);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib2);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib3_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib3);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib4_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib4);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib5_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib5);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib10);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == 
              lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib11);
  
    // ----------------------------------------------------------------------

  } // Asserts

}

/* ETWS info present in db when cell is found in db. ETWS info still being 
transmitted, should be read again. */
void lte_rrc_sib_test_etws_ho1()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[10];
  uint32 buf_len;

  // ---------------------------------------------

  // camp on a cell and receive etws info
  lte_rrc_sib_test_basic_etws(FALSE);

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
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_cell2,
                                        sizeof(sib1_2345_mcc_8_5_cell2),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK|
  	                                                     LTE_RRC_SIB3_BITMASK|
  	                                                     LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
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
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
  	                                                     LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
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
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
  	                                                     LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
  	                                                     LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_cell2,
                                        sizeof(sib1_2345_mcc_8_5_cell2),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        65);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK);

  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
  	                                                     LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  // HO back to the first cell. ETWS info present in db but should be read again
  lte_rrc_sib_test_send_ho_started_indi();
  lte_rrc_sib_test_ho_in_progress = FALSE;

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // ---------------------------------------------

  // Asserts on this cell in the db
  {
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK |
                                                                 LTE_RRC_SIB5_BITMASK |
                                                                 LTE_RRC_SIB10_BITMASK |
                                                                 LTE_RRC_SIB11_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[2] == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size == sizeof(sib10_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[2] == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[3] == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_size == sizeof(sib11_seg2of2_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[3].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[3] == NULL);
    for(int i = 4; i < LTE_RRC_SIB_MAX_SIS; i++)
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
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
  }

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

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);

  // ---------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // ---------------------------------------------

  lte_rrc_sib_test_rcv_get_sibs_cnfi(PHY_CELL_ID_1,
                                     FREQ_1,
                                     0,
                                     LTE_RRC_GET_SIBS_SUCCESS);

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
									   FREQ_1,
									  (LTE_RRC_SIB5_BITMASK),
									  (LTE_RRC_MIB_BITMASK |
                                       LTE_RRC_SIB1_BITMASK |
                                       LTE_RRC_SIB2_BITMASK |
                                       LTE_RRC_SIB3_BITMASK |
                                       LTE_RRC_SIB4_BITMASK |
                                       LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB_ETWS_SIBS_BITMASK));

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // ---------------------------------------------

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // ---------------------------------------------

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // ---------------------------------------------

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  // Send SIB11 segment 2 (last segment)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_8_5,
                                        sizeof(sib11_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        5);

  // We expect a etws_sec_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
  TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 5);
  TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib11_warn_msg[0] == 0x11);
  TF_ASSERT(sib11_warn_msg[1] == 0x11);
  TF_ASSERT(sib11_warn_msg[2] == 0x22);
  TF_ASSERT(sib11_warn_msg[3] == 0x22);
  TF_ASSERT(sib11_warn_msg[4] == 0x22);
  TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);

  // We expect a sib_sched_req message from RRC (to stop SIB reading)
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_SIB_MIB_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 5);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

}

/* Test for the case when ETWS is not supported */
void lte_rrc_sib_test_etws_not_supported()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

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
  lte_rrc_sib_test_serv_cell_phy_cell_id = PHY_CELL_ID_1;
  lte_rrc_sib_test_serv_cell_freq = FREQ_1;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  //---------------------------------------------------------------

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
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
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

  /* Timer should have been restarted for MIB and SIB1 and duration is current 
     mod prd remaining time + MIB_SIB1 timer */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == 
            (LTE_RRC_SIB_MIB_SIB1_WT_TMR + 600));
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr) == 
            (LTE_RRC_SIB_MIB_SIB1_WT_TMR + 600));
  
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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------
}

/* etws event active and rcv new get_sibs_reqi for some other cause - transition of 
event should be fine */
void lte_rrc_sib_test_etws_event_transition()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[10];

  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;

  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 1;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 3000;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 5000;
  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
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

  // Send SIB1 that schedules SIB10 and SIB11
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE, FALSE, 
                                      4);

  lte_rrc_utf_check_no_more_msgs();

  /* Asserts on active Events */
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_PG);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_10_11_mcc_8_5));
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
                                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
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
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                                 LTE_RRC_SIB1_BITMASK |
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK |
                                                                                 LTE_RRC_SIB4_BITMASK |
                                                                                 LTE_RRC_SIB5_BITMASK |
                                                                                 LTE_RRC_SIB_ETWS_SIBS_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 4);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                                          LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS);
  
    // ----------------------------------------------------------------------

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_read_active == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_msg_id == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_curr_sr_num == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIB11_SEGMENTS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_segment[i].seg_len == 0);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_num_segments_rcvd == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib11_last_segment_index == LTE_RRC_SIB_MAX_SIB11_SEGMENTS);
  }

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  // Send SIB11 segment 2 (last segment)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_8_5,
                                        sizeof(sib11_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        5);

  // We expect a etws_sec_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
  TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 5);
  TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib11_warn_msg[0] == 0x11);
  TF_ASSERT(sib11_warn_msg[1] == 0x11);
  TF_ASSERT(sib11_warn_msg[2] == 0x22);
  TF_ASSERT(sib11_warn_msg[3] == 0x22);
  TF_ASSERT(sib11_warn_msg[4] == 0x22);
  TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);

  // We expect a sib_sched_req message from RRC (to stop SIB reading)
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
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 5);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

}

/* ETWS added without notification when SIB1 is read for next_mod_prd_value_tag_check */
void lte_rrc_sib_test_etws_without_notification()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[10];

  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;

  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 1;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 3000;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 5000;
  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
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

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
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
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  // Send SIB11 segment 2 (last segment)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_8_5,
                                        sizeof(sib11_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  // We expect a etws_sec_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
  TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 5);
  TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib11_warn_msg[0] == 0x11);
  TF_ASSERT(sib11_warn_msg[1] == 0x11);
  TF_ASSERT(sib11_warn_msg[2] == 0x22);
  TF_ASSERT(sib11_warn_msg[3] == 0x22);
  TF_ASSERT(sib11_warn_msg[4] == 0x22);
  TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);

  // We expect a sib_sched_req message from RRC (to stop SIB reading)
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
}

/* etws_msg_id_list_req received from WMS that includes the message id transmitted 
by the network */
void lte_rrc_sib_test_etws_msg_id_requested(boolean msg_id_requested)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[10];

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  // using msgr_lte_rrc as of now, as wms not defined
  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 2;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 0;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 10;
  etws_msg_id_list_req.msg_id_range_list[1].min_msg_id = 20;
  etws_msg_id_list_req.msg_id_range_list[1].max_msg_id = 30;

  if(msg_id_requested)
  {
    etws_msg_id_list_req.msg_id_range_list_size = 3;
    etws_msg_id_list_req.msg_id_range_list[2].min_msg_id = 3000;
    etws_msg_id_list_req.msg_id_range_list[2].max_msg_id = 5000;
  }

  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
                                      1);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // Send SIB1 that schedules SIB10 and SIB11
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  if(msg_id_requested)
  {
    // We expect a etws_prim_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
    TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
    TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
    TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
    TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
    TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);
  }

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        5);

  if(msg_id_requested)
  {
    // No sched req should be sent
    lte_rrc_utf_check_no_more_msgs();

    // Send SIB11 segment 2 (last segment)
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_8_5,
                                          sizeof(sib11_seg2of2_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          5);

    // We expect a etws_sec_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
    TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);
  
    att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
    msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
    TF_ASSERT(rx_dsm_ptr != NULL);
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
    TF_ASSERT(dsm_packet_length == 5);
    TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
    TF_ASSERT(rx_dsm_ptr == NULL);
  
    TF_ASSERT(sib11_warn_msg[0] == 0x11);
    TF_ASSERT(sib11_warn_msg[1] == 0x11);
    TF_ASSERT(sib11_warn_msg[2] == 0x22);
    TF_ASSERT(sib11_warn_msg[3] == 0x22);
    TF_ASSERT(sib11_warn_msg[4] == 0x22);
    TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
    TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
    TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);
  }

  // We expect a sib_sched_req message from RRC (to stop SIB reading)
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 5);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

}

/* etws_msg_id_list_req received from WMS that includes the message id transmitted 
by the network  and checking if sib 11 is not 0x0 (NULL)*/
void lte_rrc_sib_test_etws_null_sib_11(boolean msg_id_requested)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[10];

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  // using msgr_lte_rrc as of now, as wms not defined
  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 2;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 0;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 10;
  etws_msg_id_list_req.msg_id_range_list[1].min_msg_id = 20;
  etws_msg_id_list_req.msg_id_range_list[1].max_msg_id = 30;

  if(msg_id_requested)
  {
    etws_msg_id_list_req.msg_id_range_list_size = 3;
    etws_msg_id_list_req.msg_id_range_list[2].min_msg_id = 3000;
    etws_msg_id_list_req.msg_id_range_list[2].max_msg_id = 5000;
  }

  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
                                      1);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // Send SIB1 that schedules SIB10 and SIB11
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  if(msg_id_requested)
  {
    // We expect a etws_prim_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
    TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
    TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
    TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
    TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
    TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);
  }

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // Send SIB11 segment 1 with NULL warning message segment
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_8_NULL_MSGSEG,
                                        sizeof(sib11_8_NULL_MSGSEG),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        5);

  // We expect a sib_sched_req message from RRC (to stop SIB reading)
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 5);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

}


/* etws sibs come in an SI that contains other SIBs */
void lte_rrc_sib_test_etws_sibs_with_other_sibs()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[10];

  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;

  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 1;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 3000;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 5000;
  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
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
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

  // send SIB2_11_seg1of2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib11_seg1of2_8_5,
                                        sizeof(sib2_sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  // this will make CSP return this cell as serving cell, though camped_indi as 
  // not been sent for the cell
  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;
  lte_rrc_sib_test_serv_cell_phy_cell_id = PHY_CELL_ID_1;
  lte_rrc_sib_test_serv_cell_freq = FREQ_1;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB10_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB10_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  //---------------------------------------------------------------

  // send SIB10_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_sib3_8_5,
                                        sizeof(sib10_sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

  // send SIB2_11_seg1of2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib11_seg2of2_8_5,
                                        sizeof(sib2_sib11_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  //---------------------------------------------------------------

  // We expect a etws_sec_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
  TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 5);
  TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib11_warn_msg[0] == 0x11);
  TF_ASSERT(sib11_warn_msg[1] == 0x11);
  TF_ASSERT(sib11_warn_msg[2] == 0x22);
  TF_ASSERT(sib11_warn_msg[3] == 0x22);
  TF_ASSERT(sib11_warn_msg[4] == 0x22);
  TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

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
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

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
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));

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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //---------------------------------------------------------------
}

/* etws_msg_id_list_req received from WMS when an event is already active */
void lte_rrc_sib_test_etws_msg_id_req_when_event_active()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[10];

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  // using msgr_lte_rrc as of now, as wms not defined
  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 2;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 0;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 10;
  etws_msg_id_list_req.msg_id_range_list[1].min_msg_id = 20;
  etws_msg_id_list_req.msg_id_range_list[1].max_msg_id = 30;

  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  TF_MSG("*****");

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
                                      1);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // Send SIB1 that schedules SIB10 and SIB11
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // only sib_sched_req is expected. etws_prim_ind should not be sent as the msg_id is not requested
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  lte_rrc_utf_check_no_more_msgs();

  // send another etws_msg_id_list_req - this time requesting the msg_id that is being transmitted.
  // an event is already active at this time.
  etws_msg_id_list_req.msg_id_range_list_size = 3;
  etws_msg_id_list_req.msg_id_range_list[2].min_msg_id = 3000;
  etws_msg_id_list_req.msg_id_range_list[2].max_msg_id = 5000;

  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));

  TF_MSG("+++");

  lte_rrc_utf_wait_for_done();

  
  // We expect a sib_sched_req message from RRC - requesting SIB1 & SIB10 again
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB10_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // Send SIB1 that schedules SIB10 and SIB11
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_prim_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_prim_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_prim_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_prim_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_prim_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_sec_info_present == FALSE);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[0] == 0);
  TF_ASSERT(etws_prim_ind_ptr->warning_type[1] == 0);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB11_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // No sched req should be sent
  //lte_rrc_utf_check_no_more_msgs();


  // Send SIB11 segment 2 (last segment)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_8_5,
                                        sizeof(sib11_seg2of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        5);

  // We expect a etws_sec_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
  TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 5);
  TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib11_warn_msg[0] == 0x11);
  TF_ASSERT(sib11_warn_msg[1] == 0x11);
  TF_ASSERT(sib11_warn_msg[2] == 0x22);
  TF_ASSERT(sib11_warn_msg[3] == 0x22);
  TF_ASSERT(sib11_warn_msg[4] == 0x22);
  TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);

  // We expect a sib_sched_req message from RRC (to stop SIB reading)
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 5);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));


}

/* Segment of length 1246/1247 - valid/invalid case */
void lte_rrc_sib_test_etws_seg_len(boolean valid_length)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[1246];

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  // using msgr_lte_rrc as of now, as wms not defined
  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 1;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 3000;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 5000;

  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
                                      1);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // Send SIB1 that schedules SIB10 and SIB11
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  if(valid_length)
  {
    // Send SIB11 with segment of length 1246
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg_len_1246_8_5,
                                          sizeof(sib11_seg_len_1246_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          4);

    // We expect a etws_sec_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
    TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);
  
    att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
    msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
    TF_ASSERT(rx_dsm_ptr != NULL);
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
    TF_ASSERT(dsm_packet_length == 1246);
    TF_ASSERT(1246 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
    TF_ASSERT(rx_dsm_ptr == NULL);
  
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
    TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
    TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);
  }
  else
  {
    // Send SIB11 with segment of length 1247
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg_len_1247_8_5,
                                          sizeof(sib11_seg_len_1247_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          4);

    //sec_ind should not be sent
  }
  
  // We expect a sib_sched_req message from RRC (to stop SIB reading)
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
}

/* Warning message of length 1246/1247 - valid/invalid case */
void lte_rrc_sib_test_etws_warn_msg_len(boolean valid_length)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[1246];

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  // using msgr_lte_rrc as of now, as wms not defined
  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 1;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 3000;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 5000;

  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
                                      1);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // Send SIB1 that schedules SIB10 and SIB11
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();

  if(valid_length)
  {
    // Send SIB11 with segment2 of length 1244
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_1246_8_5,
                                          sizeof(sib11_seg2of2_1246_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          4);

    // We expect a etws_sec_ind from SIB
    lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
    TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);
  
    att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
    msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
    TF_ASSERT(rx_dsm_ptr != NULL);
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
    TF_ASSERT(dsm_packet_length == 1246);
    TF_ASSERT(1246 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
    TF_ASSERT(rx_dsm_ptr == NULL);
  
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[0] == 1);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[1] == 2);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[2] == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.num_mnc_digits == 3);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[0] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[1] == 4);
    TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[2] == 4);
    TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
    TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
    TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
    TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);
  }
  else
  {
    // Send SIB11 with segment2 of length 1245
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg2of2_1247_8_5,
                                          sizeof(sib11_seg2of2_1247_8_5),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          4);

    //sec_ind should not be sent
  }
  
  // We expect a sib_sched_req message from RRC (to stop SIB reading)
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
}

/* Multiple segments in the same SI */
void lte_rrc_sib_test_etws_multiple_segments_in_same_si()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;
  uint32 buf_len;
  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;
  unsigned char sib11_warn_msg[10];

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  // using msgr_lte_rrc as of now, as wms not defined
  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 1;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 3000;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 5000;

  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
                                      1);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // Send SIB1 that schedules SIB10 and SIB11
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // Send SIB11 multiple segments in the same SI
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_multiple_seg_8_5,
                                        sizeof(sib11_multiple_seg_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // We expect a etws_sec_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_sec_ind_ptr, &buf_len);
  TF_ASSERT(etws_sec_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)etws_sec_ind_ptr, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  TF_ASSERT(rx_dsm_ptr != NULL);
  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 5);
  TF_ASSERT(5 == dsm_pullup(&rx_dsm_ptr, sib11_warn_msg, (uint16)dsm_packet_length));
  TF_ASSERT(rx_dsm_ptr == NULL);

  TF_ASSERT(sib11_warn_msg[0] == 0x11);
  TF_ASSERT(sib11_warn_msg[1] == 0x11);
  TF_ASSERT(sib11_warn_msg[2] == 0x22);
  TF_ASSERT(sib11_warn_msg[3] == 0x22);
  TF_ASSERT(sib11_warn_msg[4] == 0x22);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[0] == 1);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[1] == 2);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mcc[2] == 3);
  TF_ASSERT(etws_sec_ind_ptr->plmn.num_mnc_digits == 3);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[0] == 4);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[1] == 4);
  TF_ASSERT(etws_sec_ind_ptr->plmn.mnc[2] == 4);
  TF_ASSERT(etws_sec_ind_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(etws_sec_ind_ptr->earfcn == FREQ_1);
  TF_ASSERT(etws_sec_ind_ptr->msg_id == 0x1100);
  TF_ASSERT(etws_sec_ind_ptr->sr_num == 0);
  TF_ASSERT(etws_sec_ind_ptr->data_coding_scheme_present == TRUE);

  // We expect a sib_sched_req message from RRC (to stop SIB reading)
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibDsmTest);

void LteRrcSibDsmTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibDsmTest::Test()
{
  dsm_item_type *dsm_ptr = NULL;            /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
  unsigned char msg[4] = "abc";
  unsigned char msg2[5] = "defg";
  unsigned char encoded_msg_ptr[10];
  lte_mac_rrc_dl_data_ind_s bcch_dl_data_ind;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  msgr_init_hdr_attach(&bcch_dl_data_ind.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_BCCH_DL_DATA_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr, msg2, 4, DSM_DS_SMALL_ITEM_POOL) == 4);
  TF_ASSERT(dsm_pushdown(&dsm_ptr, msg, 3, DSM_DS_SMALL_ITEM_POOL) == 3);
  
  TF_ASSERT(dsm_ptr != NULL);

  att_ptr = msgr_get_attach(&bcch_dl_data_ind.hdr, 0);
  msgr_set_dsm_attach(att_ptr, dsm_ptr);

  /* Attach pointer */
  msgr_attach_struct_type *att_ptr_rcv;
  /* Received DSM pointer */
  dsm_item_type *rx_dsm_ptr;
  uint32 dsm_packet_length;

  att_ptr_rcv = msgr_get_attach((msgr_hdr_struct_type *)&bcch_dl_data_ind, 0);
  msgr_get_dsm_attach(att_ptr_rcv, &rx_dsm_ptr);
  LTE_ASSERT(rx_dsm_ptr != NULL);

  dsm_packet_length = dsm_length_packet(rx_dsm_ptr);
  TF_ASSERT(dsm_packet_length == 7);
  TF_ASSERT(7 == dsm_pullup(&rx_dsm_ptr, encoded_msg_ptr, (uint16)dsm_packet_length));

  /* Received dsm should have been freed */
  LTE_ASSERT(rx_dsm_ptr == NULL);




  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibDsmTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test case to check basic ETWS operation where cell is camped on a cell, page 
for ETWS is rcvd, SIB1 is read which schedules SIB10 and 11, both are read and 
indications are sent to upper layers
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsBasicTest);

void LteRrcSibEtwsBasicTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };

  // Code to set etws_supported by EFS
  int oflag, mode, efs_ret;
  boolean etws_supported = TRUE;

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibEtwsBasicTest");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/etws", 
                    &etws_supported, sizeof(etws_supported), oflag, mode) >= 0);

  etws_supported = FALSE;

  /* Read the EFS to get the etws_supported flag */
  TF_ASSERT((efs_ret = efs_get("/nv/item_files/modem/lte/rrc/sib/etws",
                               &etws_supported, 
                               sizeof(etws_supported))) != -1);

  TF_ASSERT(etws_supported == TRUE);

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  // etws_supported must be set to TRUE by efs
  TF_ASSERT(lte_rrc_sib.sd_ptr->etws_supported == TRUE);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsBasicTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_basic_etws(FALSE);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsBasicTest::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test case to check basic ETWS operation where cell is camped on a cell, page 
for ETWS is rcvd, SIB1 is read which schedules SIB10 and 11, both are read and 
indications are sent to upper layers
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsBasicTestWithDSDS);

void LteRrcSibEtwsBasicTestWithDSDS::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };

  // Code to set etws_supported by EFS
  int oflag, mode, efs_ret;
  boolean etws_supported = TRUE;

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibEtwsBasicTest");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/etws", 
                    &etws_supported, sizeof(etws_supported), oflag, mode) >= 0);

  etws_supported = FALSE;

  /* Read the EFS to get the etws_supported flag */
  TF_ASSERT((efs_ret = efs_get("/nv/item_files/modem/lte/rrc/sib/etws",
                               &etws_supported, 
                               sizeof(etws_supported))) != -1);

  TF_ASSERT(etws_supported == TRUE);

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  // etws_supported must be set to TRUE by efs
  TF_ASSERT(lte_rrc_sib.sd_ptr->etws_supported == TRUE);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsBasicTestWithDSDS::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_basic_etws_with_dsds(FALSE);

  // ---------------------------------------------

  printf("\nHERE\n");
  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsBasicTestWithDSDS::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Modification period boundary crossed while reading SIB11
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsModBndCrossedTest);

void LteRrcSibEtwsModBndCrossedTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsModBndCrossedTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_basic_etws(TRUE);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsModBndCrossedTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Extra segments for SIB11 are sent and SIB11 is rcvd before SIB10
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsExtraSegmentsTest);

void LteRrcSibEtwsExtraSegmentsTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsExtraSegmentsTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_etws_extra_segments();

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsExtraSegmentsTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
ETWS indication in connected mode
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsConnectedTest);

void LteRrcSibEtwsConnectedTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsConnectedTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_etws_connected();

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsConnectedTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Camping on a cell that already transmits ETWS SIBs
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsCampTest);

void LteRrcSibEtwsCampTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsCampTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_etws_camp(LTE_RRC_SIB_TEST_ETWS_PAGED_INVALID);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsCampTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
ETWS info already present, event active, paged for ETWS again - msg_id/sr_num 
does not change - ETWS SIBs should be read and primary and secondary indications 
should be sent
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsBack2BackTest1);

void LteRrcSibEtwsBack2BackTest1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsBack2BackTest1::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_etws_camp(LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_INFO_PRESENT);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsBack2BackTest1::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
ETWS reading active, paged again - msg_id/sr_num does not change - SIB10 should 
be read again and prim_ind sent, for SIB11 on rcving new segment msg_id/sr_num 
match with old segment so old should be considered current
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsBack2BackTest2);

void LteRrcSibEtwsBack2BackTest2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsBack2BackTest2::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_etws_camp(LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_READING_ACTIVE_ID_SAME);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsBack2BackTest2::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
ETWS reading active, paged again - msg_id/sr_num changes - SIB10 should be read 
again and prim_ind sent, for SIB11 on rcving new segment msg_id/sr_num do not 
match so old segments should be discarded
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsBack2BackTest3);

void LteRrcSibEtwsBack2BackTest3::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsBack2BackTest3::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_etws_camp(LTE_RRC_SIB_TEST_ETWS_PAGED_WHEN_READING_ACTIVE_ID_CHANGES);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsBack2BackTest3::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
ETWS info already present (in db), event not active, paged again - msg_id/sr_num 
does not change - When event is created, it should not have any ETWS info and 
when paged all info should be read
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsDbTest);

void LteRrcSibEtwsDbTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsDbTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_etws_camp(LTE_RRC_SIB_TEST_ETWS_PAGED_INVALID);

  lte_rrc_sib_test_etws_db();

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsDbTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
SIB wait timer expires while reading ETWS SIBs
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsTimerTest);

void LteRrcSibEtwsTimerTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsTimerTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_sib_test_etws_camp(LTE_RRC_SIB_TEST_ETWS_TIMER_EXPIRY);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsTimerTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
When going back on serving cell after reselection failure, ETWS info is 
transmitted on it. ETWS SIBs should be read.
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsReselFailTest);

void LteRrcSibEtwsReselFailTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsReselFailTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  lte_rrc_msg_id_list_req_s etws_msg_id_list_req;

  msgr_init_hdr(&etws_msg_id_list_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_MSG_ID_LIST_REQ);
  etws_msg_id_list_req.msg_id_range_list_size = 1;
  etws_msg_id_list_req.msg_id_range_list[0].min_msg_id = 3000;
  etws_msg_id_list_req.msg_id_range_list[0].max_msg_id = 5000;
  lte_rrc_utf_send_msg((byte*)&etws_msg_id_list_req, sizeof(etws_msg_id_list_req));
  lte_rrc_utf_wait_for_done();

  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_resel2(TRUE);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsReselFailTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
ETWS info present in db when cell is found in db. ETWS info still being 
transmitted, should be read again.
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsDbTest2);

void LteRrcSibEtwsDbTest2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsDbTest2::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  /* HO */
  lte_rrc_sib_test_etws_ho1();

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsDbTest2::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test for the case when ETWS is not supported
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsNotSupportedTest);

void LteRrcSibEtwsNotSupportedTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };

  // Code to set etws_supported = FALSE by EFS
  int oflag, mode, efs_ret;
  boolean etws_supported = FALSE;

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibEtwsNotSupportedTest");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/etws", 
                    &etws_supported, sizeof(etws_supported), oflag, mode) >= 0);

  etws_supported = TRUE;

  /* Read the EFS to get the etws_supported flag */
  TF_ASSERT((efs_ret = efs_get("/nv/item_files/modem/lte/rrc/sib/etws",
                               &etws_supported, 
                               sizeof(etws_supported))) != -1);

  TF_ASSERT(etws_supported == FALSE);

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  lte_rrc_sib_test_stub_init();

  TF_ASSERT(lte_rrc_sib.sd_ptr->etws_supported == FALSE);
}

void LteRrcSibEtwsNotSupportedTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  /* Camp */
  lte_rrc_sib_test_etws_not_supported();

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsNotSupportedTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
etws event active and rcv new get_sibs_reqi for some other cause - transition of 
event should be fine
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsEventTransitionTest);

void LteRrcSibEtwsEventTransitionTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsEventTransitionTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  /* Event transition test */
  lte_rrc_sib_test_etws_event_transition();

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsEventTransitionTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
ETWS added without notification when SIB1 is read for next_mod_prd_value_tag_check
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsWithoutNotificationTest);

void LteRrcSibEtwsWithoutNotificationTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsWithoutNotificationTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  /* ETWS without notification test */
  lte_rrc_sib_test_etws_without_notification();

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsWithoutNotificationTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
etws_msg_id_list_req received from WMS that includes the message id transmitted 
by the network
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsMsgIdRequestedTest);

void LteRrcSibEtwsMsgIdRequestedTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsMsgIdRequestedTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  /* ETWS msg id requested test */
  lte_rrc_sib_test_etws_msg_id_requested(TRUE);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsMsgIdRequestedTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
etws_msg_id_list_req received from WMS that does not include the message id transmitted 
by the network
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsMsgIdNotRequestedTest);

void LteRrcSibEtwsMsgIdNotRequestedTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsMsgIdNotRequestedTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  /* ETWS msg id requested test */
  lte_rrc_sib_test_etws_msg_id_requested(FALSE);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsMsgIdNotRequestedTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
etws_msg_id_list_req received from WMS that includes the message id transmitted 
by the network, ZBUT SIB 11 segments are NULL
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsMsgIdReqSib11NullWarnSeg);

void LteRrcSibEtwsMsgIdReqSib11NullWarnSeg::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsMsgIdReqSib11NullWarnSeg::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  /* ETWS msg id requested test */
  lte_rrc_sib_test_etws_null_sib_11(TRUE);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsMsgIdReqSib11NullWarnSeg::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
etws sibs come in an SI that contains other SIBs
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsSibsWithOtherSibsTest);

void LteRrcSibEtwsSibsWithOtherSibsTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsSibsWithOtherSibsTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  /* ETWS SIB and other SIBs in same SI */
  lte_rrc_sib_test_etws_sibs_with_other_sibs();

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsSibsWithOtherSibsTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
etws_msg_id_list_req received from WMS when an event is already active
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsMsgIdReqWhenEventActiveTest);

void LteRrcSibEtwsMsgIdReqWhenEventActiveTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND,

    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsMsgIdReqWhenEventActiveTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  /* ETWS msg id list req received when SIB reading event is active */
  lte_rrc_sib_test_etws_msg_id_req_when_event_active();

  // ----------------------------------------------------------------------

  //lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsMsgIdReqWhenEventActiveTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Segment of length 1246 - valid case
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsSib11SegLen1246Test);

void LteRrcSibEtwsSib11SegLen1246Test::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsSib11SegLen1246Test::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  /* Segment of length 1246 - valid case */
  lte_rrc_sib_test_etws_seg_len(TRUE);

  if(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size > LTE_RRC_MAX_SI_LENGTH)
  {
    lte_rrc_mem_free(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr);
  }

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsSib11SegLen1246Test::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Segment of length 1247 - invalid case
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsSib11SegLen1247Test);

void LteRrcSibEtwsSib11SegLen1247Test::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsSib11SegLen1247Test::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  /* Segment of length 1247 - invalid case */
  lte_rrc_sib_test_etws_seg_len(FALSE);

  if(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size > LTE_RRC_MAX_SI_LENGTH)
  {
    lte_rrc_mem_free(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr);
  }

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsSib11SegLen1247Test::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Warning message of length 1246 - valid case
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsSib11WarnMsgLen1246Test);

void LteRrcSibEtwsSib11WarnMsgLen1246Test::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsSib11WarnMsgLen1246Test::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  /* Warning message of length 1246 - valid case */
  lte_rrc_sib_test_etws_warn_msg_len(TRUE);

  if(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size > LTE_RRC_MAX_SI_LENGTH)
  {
    lte_rrc_mem_free(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr);
  }

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsSib11WarnMsgLen1246Test::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Warning message of length 1247 - invalid case
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsSib11WarnMsgLen1247Test);

void LteRrcSibEtwsSib11WarnMsgLen1247Test::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsSib11WarnMsgLen1247Test::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  /* Warning message of length 1247 - invalid case */
  lte_rrc_sib_test_etws_warn_msg_len(FALSE);

  if(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_size > LTE_RRC_MAX_SI_LENGTH)
  {
    lte_rrc_mem_free(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[2].encoded_si_ptr);
  }

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsSib11WarnMsgLen1247Test::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Multiple segments in the same SI
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsMultipleSegmentsInSameSiTest);

void LteRrcSibEtwsMultipleSegmentsInSameSiTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_ETWS_PRIM_IND,
      LTE_RRC_ETWS_SEC_IND
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 3);

  lte_rrc_sib_test_stub_init();
}

void LteRrcSibEtwsMultipleSegmentsInSameSiTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ---------------------------------------------

  /* Multiple segments in the same SI */
  lte_rrc_sib_test_etws_multiple_segments_in_same_si();

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsMultipleSegmentsInSameSiTest::Teardown()
{
  lte_rrc_utf_teardown();
}

