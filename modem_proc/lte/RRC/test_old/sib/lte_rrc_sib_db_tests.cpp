
/*!
  @file lte_rrc_sib_db_tests.cpp

  @brief
  SIB db related test cases
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_db_tests.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/09   amit    Added test for long SIBs
04/24/09   amit    Sending just one sib_updated_ind if all sibs have been 
                   received
03/08/09   amit    Modified the test for the new SIB design
11/04/08   amit    Removed sibs_needed bitmask from get_sibs_req
09/24/08   amit    Initial Version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "__lte_rrc_sib.h"
#include "lte_rrc_sibi.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_mac_ind.h"
#include "lte_mac_msg.h"
#include "dsm_item.h"
#include "dsm_init.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_sib_test_common.h"
#include <tf_stub.h>


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/




/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

DECLARE_SIB1_2345_MCC_8_5;
DECLARE_SIB1_2345_MCC_8_5_VALUE_TAG_CHANGE;
DECLARE_SIB1_2_3_4_MCC_8_5;
DECLARE_SIB1_CELLS_8_5;
DECLARE_SIB1_INFEASIBLE_SCHEDULE_8_5;
DECLARE_SIB1_INVALID_EMPTY_SI_8_5;
DECLARE_SIB1_INVALID_MAPPING_8_5;
DECLARE_SIB1_MAPPING_SPARE_VALUE_8_5;
DECLARE_SIB1_MEM_TEST_8_5;
DECLARE_SIB1_NO_MCC_8_5;
DECLARE_SIB2_8_5_MOD_PRD_2048;
DECLARE_SIB2_8_5_MOD_PRD_1024;
DECLARE_SIB2_8_5_MOD_PRD_512;
DECLARE_SIB2_SIB3_8_5;
DECLARE_SIB2_SIB3_8_5_MOD_PRD_128;
DECLARE_SIB2_SIB3_8_5_MOD_PRD_512;
DECLARE_SIB3_8_5;
DECLARE_SIB4_8_5;
DECLARE_SIB4_SIB5_8_5;
DECLARE_SIB1_2345_MCC_8_5_VALUE_TAG_MATCH_SIB1_DIFFER;
DECLARE_SIB1_2_345_MCC_8_5;
DECLARE_SIB2_8_5;
DECLARE_SIB345_8_5_LONG;

extern "C"
{
extern lte_rrc_sib_s lte_rrc_sib;
extern void lte_rrc_sib_db_clear_cell(uint8 db_index);

}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern lte_errno_e lte_rrc_sib_test_get_serving_cell_params(lte_rrc_cell_info_s *cell_info_ptr);
extern stm_state_t lte_rrc_sib_test_controller_get_state();
extern boolean lte_rrc_sib_test_camped;
extern stm_state_t lte_rrc_sib_test_controller_state;
extern boolean lte_rrc_utf_check_sib_mem;
extern boolean lte_rrc_sib_test_wcdma_supported;
extern boolean lte_rrc_sib_test_gsm_supported;
extern boolean lte_rrc_sib_test_cdma_1x_supported;
extern boolean lte_rrc_sib_test_cdma_do_supported;
extern boolean lte_rrc_sib_test_lte_band_is_supported;
extern boolean lte_rrc_sib_test_ho_in_progress;

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/* Populates db with one cell, with that cell as serving cell */
void lte_rrc_sib_test_populate_db_with_one_cell()
{
  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();
}

/* Reselection function */
/* i is 0 onwards; 
   sib1 for cell i = sib1_cells_8_5[i]; 
   cell id = i + 1; 
   phy cell id = i + 21;
  lte_rrc_sib_test_sib_db_key = i;
   send_sched_req_for_old_cell TRUE implies event active for old cell and on 
   rcving abort for that cell sched req has to be sent to stop sib reading. 
   False implies abort is ignored */
void lte_rrc_sib_test_resel(int cell_index, boolean send_sched_req_for_old_cell)
{
  int phy_cell_id = cell_index + 21;

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_cphy_mib_ind_s mib_ind;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_sib_abort_reqi_s abort_reqi;

  uint32 buf_len;

  // ---------------------------------------------

  // Prepare abort_reqi to send to SIB
  lte_rrc_init_default_hdr(&abort_reqi.msg_hdr, LTE_RRC_SIB_ABORT_REQI);
  abort_reqi.cell_id.freq = FREQ_1;
  abort_reqi.cell_id.phy_cell_id = phy_cell_id - 1;

  // Send abort_reqi
  lte_rrc_utf_send_msg((byte*)&abort_reqi, sizeof(abort_reqi));
  lte_rrc_utf_wait_for_done();

  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(phy_cell_id, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL, TRUE, 
                                LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

  // ---------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == cell_index - 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  for(int i = 0; i < cell_index - 1; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == TRUE);
  }
  for(int i = cell_index; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_bitmask == 0);
  for(int i = 0; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == TRUE);
  for(int i = 0; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == LTE_RRC_SIB_INVALID_TIME);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                       LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == 0);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB_UPDATE_NEEDED_BITMASK &
                                                                         LTE_RRC_SIB_NON_MANDATORY_SIBS_BITMASK));
  
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == LTE_RRC_SIB_INVALID_WINDOW_LENGTH);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == LTE_RRC_NULL_SFN);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 0);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == );
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */
  for(int k = 1; k < cell_index - 2; k++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].index_rank == cell_index - (k + 1));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].index_counter == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.plmn.mcc[0] == 1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.plmn.mcc[1] == 2);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.plmn.mcc[2] == 3);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.plmn.num_mnc_digits == 3);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.plmn.mnc[0] == 4);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.plmn.mnc[1] == 5);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.plmn.mnc[2] == 6);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.global_cell_id == 0x400000);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.cell_id.phy_cell_id == k + 21);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.cell_id.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].value_tag == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].mib.num_tx_antennas == 1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].mib.dl_bandwidth == LTE_BW_NRB_100);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].encoded_sib1.encoded_si_size == 16);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib1_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_sib3_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.decoded_si_ptr[0] == NULL);
    for(int i = 1; i < LTE_RRC_SIB_MAX_SIS; i++)
    {
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.sibs_in_si_bitmask[i] == 0);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.encoded_si_info[i].encoded_si_size == 0);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.decoded_si_ptr[i] == NULL);
    }
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.si_decoded == FALSE);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib2_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib3_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib4_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib5_ptr == NULL);
   // TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib6_ptr == NULL);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib11_ptr == NULL);
  }

  // ----------------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);

  mib_ind.mib.num_tx_antennas = 1;
  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_100;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_NORMAL;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;

  mib_ind.freq = FREQ_1;
  mib_ind.phy_cell_id = phy_cell_id;

  mib_ind.sfn = 1;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_cells_8_5[cell_index],
                                        sizeof(sib1_cells_8_5[cell_index]),
                                        phy_cell_id,
                                        FREQ_1,
                                        2);
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  // Prepare bcch_dl_data_ind to send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        phy_cell_id,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].index_rank == 1);

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == cell_index);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[lte_rrc_sib.sd_ptr->serv_cell_index].sib_cell_id.cell_id.phy_cell_id == phy_cell_id - 1);

  if(send_sched_req_for_old_cell == TRUE)
  {
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
    TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

    TF_ASSERT(sib_sched_req->freq == FREQ_1);
    TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id - 1);

    TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
    TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);
  }

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = cell_index;
  // Prepare camped_indi to send to SIB
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_indi.camped_cell.freq = FREQ_1;
  camped_indi.camped_cell.phy_cell_id = phy_cell_id;
  
  // Send camped_indi
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index - 1].all_si.si_decoded == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == cell_index);
  
  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(phy_cell_id,
                                        FREQ_1,
                                        (LTE_RRC_SIB3_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  //---------------------------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == cell_index);
  for(int i = 0; i <= cell_index; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == TRUE);
  }
  for(int i = cell_index + 1; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_SIB);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_NEXT_MOD_PRD_VALUE_TAG_CHECK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == cell_index);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == 16);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == 0);
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[2] == 3);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[3] == 3);
  for(int i = 4; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == 
            (uint32)(LTE_RRC_SIB_MIB_SIB1_WT_TMR + 
                     10*(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end + 1 - 
                         lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp)));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                       LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                 LTE_RRC_SIB1_BITMASK |
                                                                 LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                      LTE_RRC_SIB1_BITMASK |
                                                                      LTE_RRC_SIB2_BITMASK |
                                                                      LTE_RRC_SIB3_BITMASK));

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == 0);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (0));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 3);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].index_counter == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mcc[0] == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mcc[1] == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mcc[2] == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.num_mnc_digits == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mnc[0] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mnc[1] == 5);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mnc[2] == 6);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.global_cell_id == 0x400000);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].value_tag == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].encoded_sib1.encoded_si_size == 16);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib1_ptr != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_sib3_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.decoded_si_ptr[0] != NULL);
  for(int i = 1; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib2_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib3_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib4_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib5_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib6_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib11_ptr == NULL);

  // ----------------------------------------------------------------------

  mib_ind.sfn = 64;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_cells_8_5[cell_index],
                                        sizeof(sib1_cells_8_5[cell_index]),
                                        phy_cell_id,
                                        FREQ_1,
                                        65);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
}

/* Reselection function */
/* i is 0 onwards; 
   sib1 for cell i = sib1_cells_8_5[i]; 
   cell id = i + 1; 
   phy cell id = i + 21;
  lte_rrc_sib_test_sib_db_key = j
   old serving cell = k */
void lte_rrc_sib_test_resel_without_asserts(int i, int j, int k)
{
  int phy_cell_id = i + 21;

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_cphy_mib_ind_s mib_ind;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_sib_abort_reqi_s abort_reqi;

  uint32 buf_len;

  // ---------------------------------------------

  // Prepare abort_reqi to send to SIB
  lte_rrc_init_default_hdr(&abort_reqi.msg_hdr, LTE_RRC_SIB_ABORT_REQI);
  abort_reqi.cell_id.freq = FREQ_1;
  abort_reqi.cell_id.phy_cell_id = k + 21;

  // Send abort_reqi - ignored
  lte_rrc_utf_send_msg((byte*)&abort_reqi, sizeof(abort_reqi));
  lte_rrc_utf_wait_for_done();

  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(phy_cell_id, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL, TRUE, 
                                LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

  // Prepare mib_ind to send to RRC
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);

  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_100;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_NORMAL;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;

  mib_ind.freq = FREQ_1;
  mib_ind.phy_cell_id = phy_cell_id;

  mib_ind.sfn = 1;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_cells_8_5[i],
                                        sizeof(sib1_cells_8_5[i]),
                                        phy_cell_id,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  // Prepare bcch_dl_data_ind to send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        phy_cell_id,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[j].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[j].index_rank == 1);

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == j);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = j;
  // Prepare camped_indi to send to SIB
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_indi.camped_cell.freq = FREQ_1;
  camped_indi.camped_cell.phy_cell_id = phy_cell_id;

  // Send camped_indi
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.si_decoded == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == j);

  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(phy_cell_id,
                                        FREQ_1,
                                        (LTE_RRC_SIB3_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  //---------------------------------------------------------------

  mib_ind.sfn = 64;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_cells_8_5[i],
                                        sizeof(sib1_cells_8_5[i]),
                                        phy_cell_id,
                                        FREQ_1,
                                        65);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
}

/* Handover function */
/* i is 0 onwards; 
   sib1 for cell i = sib1_cells_8_5[i]; 
   cell id = i + 1; 
   phy cell id = i + 21;
  lte_rrc_sib_test_sib_db_key = i;
   send_sched_req_for_old_cell TRUE implies event active for old cell and on 
   rcving abort for that cell sched req has to be sent to stop sib reading. 
   False implies abort is ignored */
void lte_rrc_sib_test_ho(int cell_index)
{
  int phy_cell_id = cell_index + 21;
  lte_rrc_sib_test_send_ho_started_indi();
  lte_rrc_sib_test_ho_in_progress = FALSE;

  lte_rrc_sib_test_send_get_sibs_reqi(phy_cell_id, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE, TRUE, 
                                LTE_RRC_NULL_SFN);

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_cphy_mib_ind_s mib_ind;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  
  uint32 buf_len;

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

  // ---------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == cell_index - 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  for(int i = 0; i < cell_index - 1; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == TRUE);
  }
  for(int i = cell_index; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_bitmask == 0);
  for(int i = 0; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == TRUE);
  for(int i = 0; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == LTE_RRC_SIB_INVALID_TIME);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                             LTE_RRC_SIB1_BITMASK |
                                                                             LTE_RRC_SIB2_BITMASK |
                                                                             LTE_RRC_SIB3_BITMASK |
                                                                             LTE_RRC_SIB4_BITMASK |
                                                                             LTE_RRC_SIB5_BITMASK |
                                                                             LTE_RRC_SIB6_BITMASK |
                                                                             LTE_RRC_SIB7_BITMASK |
                                                                             LTE_RRC_SIB8_BITMASK |
                                                                             LTE_RRC_SIB9_BITMASK |
                                                                             LTE_RRC_SIB_ETWS_SIBS_BITMASK |
                                                                             LTE_RRC_SIB12_BITMASK |
                                                                             LTE_RRC_SIB_EMBMS_SIBS_BITMASK|
                                                                             LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                       LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == 0);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB_UPDATE_NEEDED_BITMASK &
                                                                                 LTE_RRC_SIB_NON_MANDATORY_SIBS_BITMASK &
                                                                                 ~(LTE_RRC_SIB_SIB3_SIB4_BITMASK)));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == LTE_RRC_SIB_INVALID_WINDOW_LENGTH);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == LTE_RRC_NULL_SFN);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 0);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == );
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */
  for(int k = 1; k < cell_index - 2; k++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].index_rank == cell_index - (k + 1));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].index_counter == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.plmn.mcc[0] == 1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.plmn.mcc[1] == 2);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.plmn.mcc[2] == 3);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.plmn.num_mnc_digits == 3);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.plmn.mnc[0] == 4);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.plmn.mnc[1] == 5);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.plmn.mnc[2] == 6);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.global_cell_id == 0x400000);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.cell_id.phy_cell_id == k + 21);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib_cell_id.cell_id.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].value_tag == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].mib.num_tx_antennas == 1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].mib.dl_bandwidth == LTE_BW_NRB_100);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].encoded_sib1.encoded_si_size == 16);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].sib1_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                          LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_sib3_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.decoded_si_ptr[0] == NULL);
    for(int i = 1; i < LTE_RRC_SIB_MAX_SIS; i++)
    {
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.sibs_in_si_bitmask[i] == 0);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.encoded_si_info[i].encoded_si_size == 0);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
      TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.decoded_si_ptr[i] == NULL);
    }
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_si.si_decoded == FALSE);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib2_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib3_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib4_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib5_ptr == NULL);
   // TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib6_ptr == NULL);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[k].all_decoded_sibs.sib11_ptr == NULL);
  }

  // ----------------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);

  mib_ind.mib.num_tx_antennas = 1;
  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_100;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_NORMAL;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;

  mib_ind.freq = FREQ_1;
  mib_ind.phy_cell_id = phy_cell_id;

  mib_ind.sfn = 1;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_cells_8_5[cell_index],
                                        sizeof(sib1_cells_8_5[cell_index]),
                                        phy_cell_id,
                                        FREQ_1,
                                        2);
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
  	                                                 LTE_RRC_SIB3_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  // Prepare bcch_dl_data_ind to send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        phy_cell_id,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == cell_index);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].index_rank == 0);

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = cell_index;
  
  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[lte_rrc_sib.sd_ptr->serv_cell_index].sib_cell_id.cell_id.phy_cell_id == phy_cell_id);

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index - 1].all_si.si_decoded == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == cell_index);
  
  //---------------------------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == cell_index);
  for(int i = 0; i <= cell_index; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == TRUE);
  }
  for(int i = cell_index + 1; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_SIB);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_NEXT_MOD_PRD_VALUE_TAG_CHECK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == cell_index);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == 16);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == 0);
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[2] == 3);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[3] == 3);
  for(int i = 4; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == 
            (uint32)(LTE_RRC_SIB_MIB_SIB1_WT_TMR + 
                     10*(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end + 1 - 
                         lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp)));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                             LTE_RRC_SIB1_BITMASK |
                                                                             LTE_RRC_SIB2_BITMASK |
                                                                             LTE_RRC_SIB3_BITMASK |
                                                                             LTE_RRC_SIB4_BITMASK |
                                                                             LTE_RRC_SIB5_BITMASK |
                                                                             LTE_RRC_SIB6_BITMASK |
                                                                             LTE_RRC_SIB7_BITMASK |
                                                                             LTE_RRC_SIB8_BITMASK |
                                                                             LTE_RRC_SIB9_BITMASK |
                                                                             LTE_RRC_SIB_ETWS_SIBS_BITMASK |
                                                                             LTE_RRC_SIB12_BITMASK |
                                                                             LTE_RRC_SIB_EMBMS_SIBS_BITMASK|
                                                                             LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                       LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                 LTE_RRC_SIB1_BITMASK |
                                                                 LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                      LTE_RRC_SIB1_BITMASK |
                                                                      LTE_RRC_SIB2_BITMASK |
                                                                      LTE_RRC_SIB3_BITMASK));

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == 0);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (0));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 3);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].index_counter == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mcc[0] == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mcc[1] == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mcc[2] == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.num_mnc_digits == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mnc[0] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mnc[1] == 5);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mnc[2] == 6);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.global_cell_id == 0x400000);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].value_tag == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].encoded_sib1.encoded_si_size == 16);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib1_ptr != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_sib3_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.decoded_si_ptr[0] != NULL);
  for(int i = 1; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib2_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib3_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib4_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib5_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib6_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib11_ptr == NULL);

  // ----------------------------------------------------------------------

  mib_ind.sfn = 64;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_cells_8_5[cell_index],
                                        sizeof(sib1_cells_8_5[cell_index]),
                                        phy_cell_id,
                                        FREQ_1,
                                        65);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
}

/* Populates db with all cells */
void lte_rrc_sib_test_populate_db_with_all_cells()
{
  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  /* Reselect LTE_RRC_SIB_DB_MAX_CELLS - 1 times to populate full db */
  for(int i = 1; i <= LTE_RRC_SIB_DB_MAX_CELLS - 1; i++)
  {
    lte_rrc_sib_test_resel(i, FALSE);
  }

  for(int i = 0; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 1);
  }
}

/* Populates db with all cells in connected mode */
void lte_rrc_sib_test_populate_db_with_all_cells_ho()
{
  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  /* Connected_indi */
  lte_rrc_sib_test_send_connected_indi();

  /* Reselect LTE_RRC_SIB_DB_MAX_CELLS - 1 times to populate full db */
  for(int i = 1; i <= LTE_RRC_SIB_DB_MAX_CELLS - 1; i++)
  {
    lte_rrc_sib_test_ho(i);
  }

  for(int i = 0; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 1);
  }
}

void lte_rrc_sib_test_db4(boolean mib_rcvd)
{
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  uint32 buf_len;

  // ----------------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2_3_4_mcc_8_5,
                                        sizeof(sib1_2_3_4_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == LTE_RRC_SIB2_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == LTE_RRC_SIB3_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == LTE_RRC_SIB4_BITMASK);
  
  // ----------------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_8_5_mod_prd_1024,
                                        sizeof(sib2_8_5_mod_prd_1024),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 0);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == LTE_RRC_SIB2_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == LTE_RRC_SIB3_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == LTE_RRC_SIB4_BITMASK);
  
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                 LTE_RRC_SIB1_BITMASK |
                                                                 LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                      LTE_RRC_SIB1_BITMASK |
                                                                      LTE_RRC_SIB2_BITMASK));

  // ------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib3_8_5,
                                        sizeof(sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 0);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == LTE_RRC_SIB2_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == LTE_RRC_SIB3_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == LTE_RRC_SIB4_BITMASK);
  
  // ------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();

  // ------------------------------------------------------

  lte_rrc_sib_test_camped = FALSE;
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_phase1();
  if(mib_rcvd == TRUE)
  {
    lte_rrc_sib_test_camp_phase2();
  }

  // Prepare bcch_dl_data_ind to send SIB3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib3_8_5,
                                        sizeof(sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  if(mib_rcvd == FALSE)
  {
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);
    TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
    TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
    TF_ASSERT(sib_sched_req->freq == FREQ_1);
    TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
    TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
    TF_ASSERT(sib_sched_req->num_si_msgs == 0);
    TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);
  }

  // ------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB4 to RRC - should be ignored
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_8_5,
                                        sizeof(sib4_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        5);

  // ------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2_3_4_mcc_8_5,
                                        sizeof(sib1_2_3_4_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  if(mib_rcvd == TRUE)
  {
    TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (0));
  }
  else
  {
    TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK));
  }

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 0);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == LTE_RRC_SIB2_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == LTE_RRC_SIB3_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == LTE_RRC_SIB4_BITMASK);
  
  // ----------------------------------------------------------------------

  if(mib_rcvd == TRUE)
  {
    lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
    TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
    TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
    TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
    TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                   LTE_RRC_SIB1_BITMASK |
                                                                   LTE_RRC_SIB2_BITMASK |
                                                                   LTE_RRC_SIB3_BITMASK |
                                                                   LTE_RRC_SIB4_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                        LTE_RRC_SIB1_BITMASK |
                                                                        LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK |
                                                                        LTE_RRC_SIB4_BITMASK));
  }
  else
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_SIB1_BITMASK |
                                                                   LTE_RRC_SIB2_BITMASK |
                                                                   LTE_RRC_SIB3_BITMASK |
                                                                   LTE_RRC_SIB4_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_SIB1_BITMASK |
                                                                        LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK |
                                                                        LTE_RRC_SIB4_BITMASK));
  }
}

/* Test case to check the correct value of mod prd boundary is used: default 
   when event is started, from SIB2 in db when cell is found in db, from new 
   SIB2 when it is rcvd */
void lte_rrc_sib_test_db6()
{
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  uint32 buf_len;

  // ----------------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2_3_4_mcc_8_5,
                                        sizeof(sib1_2_3_4_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == LTE_RRC_SIB2_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == LTE_RRC_SIB3_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == LTE_RRC_SIB4_BITMASK);
  
  // ----------------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_8_5_mod_prd_1024,
                                        sizeof(sib2_8_5_mod_prd_1024),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 0);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == LTE_RRC_SIB2_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == LTE_RRC_SIB3_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == LTE_RRC_SIB4_BITMASK);
  
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                 LTE_RRC_SIB1_BITMASK |
                                                                 LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                      LTE_RRC_SIB1_BITMASK |
                                                                      LTE_RRC_SIB2_BITMASK));

  // ------------------------------------------------------

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;
  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // ------------------------------------------------------

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB3_BITMASK |
  	                                                 LTE_RRC_SIB4_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
  	                                                 LTE_RRC_SIB4_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_ho_started_indi();
  lte_rrc_sib_test_ho_in_progress = FALSE;

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE, TRUE, 
                                LTE_RRC_NULL_SFN);


  // ---------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

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

  lte_cphy_mib_ind_s mib_ind;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  
  // Prepare mib_ind to send to RRC
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);

  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_100;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_NORMAL;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;

  mib_ind.freq = FREQ_1;
  mib_ind.phy_cell_id = PHY_CELL_ID_1;

  mib_ind.sfn = 1;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

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

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_value_tag_change,
                                        sizeof(sib1_2345_mcc_8_5_value_tag_change),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
  	                                                 LTE_RRC_SIB3_BITMASK |
  	                                                 LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
  	                                                 LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 0);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  // --------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5_mod_prd_512,
                                        sizeof(sib2_sib3_8_5_mod_prd_512),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
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

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 512);

  TF_ASSERT(sib_sched_req->num_si_msgs == 2);

}

/* SIBs inconsistent - SIB2 not present in the db. Mod prd length should be 64 */
void lte_rrc_sib_test_db7()
{
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  uint32 buf_len;

  // Prepare bcch_dl_data_ind to send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5_mod_prd_512,
                                        sizeof(sib2_sib3_8_5_mod_prd_512),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

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

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 512);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;
  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // ------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib3_8_5,
                                        sizeof(sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        513);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK |
                                                     LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK |
                                                     LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 0);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

/*
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));

  //---------------------------------------------------------------
*/

  lte_rrc_sib_test_send_ho_started_indi();
  lte_rrc_sib_test_ho_in_progress = FALSE;

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE, TRUE, 
                                LTE_RRC_NULL_SFN);


  // ---------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == FALSE);

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

  lte_cphy_mib_ind_s mib_ind;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;

  // Prepare mib_ind to send to RRC
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);

  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_100;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_NORMAL;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;

  mib_ind.freq = FREQ_1;
  mib_ind.phy_cell_id = PHY_CELL_ID_1;

  mib_ind.sfn = 1;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

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

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
  	                                                 LTE_RRC_SIB3_BITMASK |
  	                                                 LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
  	                                                 LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == LTE_RRC_SIB3_BITMASK);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

}

void lte_rrc_sib_test_db9()
{
  int cell_index = 1;
  int phy_cell_id = cell_index + 21;
  lte_rrc_sib_test_send_get_sibs_reqi(phy_cell_id, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                LTE_RRC_NULL_SFN);

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_cphy_mib_ind_s mib_ind;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  uint32 buf_len;

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

  // ---------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_bitmask == 0);
  for(int i = 0; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == TRUE);
  for(int i = 0; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == LTE_RRC_SIB_INVALID_TIME);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                       LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == 0);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB_UPDATE_NEEDED_BITMASK &
                                                                         LTE_RRC_SIB_NON_MANDATORY_SIBS_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == LTE_RRC_SIB_INVALID_WINDOW_LENGTH);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == LTE_RRC_NULL_SFN);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 0);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == );
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

  // ----------------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);

  mib_ind.mib.num_tx_antennas = 1;
  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_100;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_NORMAL;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;

  mib_ind.freq = FREQ_1;
  mib_ind.phy_cell_id = phy_cell_id;

  mib_ind.sfn = 1;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_cells_8_5[cell_index],
                                        sizeof(sib1_cells_8_5[cell_index]),
                                        phy_cell_id,
                                        FREQ_1,
                                        2);

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == cell_index);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK));

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  // Prepare camped_indi to send to SIB
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_indi.camped_cell.freq = FREQ_1;
  camped_indi.camped_cell.phy_cell_id = phy_cell_id;
  
  // Send camped_indi
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index - 1].all_si.si_decoded == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == cell_index);
  
  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(phy_cell_id,
                                        FREQ_1,
                                        (LTE_RRC_SIB3_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  //---------------------------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_SIB);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_NEXT_MOD_PRD_VALUE_TAG_CHECK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == cell_index);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == 16);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == 0);
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == 
            (uint32)(LTE_RRC_SIB_MIB_SIB1_WT_TMR + 
                     10*(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end + 1 - 
                         lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp)));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                       LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                 LTE_RRC_SIB1_BITMASK |
                                                                 LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                      LTE_RRC_SIB1_BITMASK |
                                                                      LTE_RRC_SIB2_BITMASK |
                                                                      LTE_RRC_SIB3_BITMASK));

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == 0);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (0));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

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
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].index_counter == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mcc[0] == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mcc[1] == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mcc[2] == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.num_mnc_digits == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mnc[0] == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mnc[1] == 5);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.plmn.mnc[2] == 6);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.global_cell_id == 0x400000);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib_cell_id.cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].value_tag == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].encoded_sib1.encoded_si_size == 16);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].sib1_ptr != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_sib3_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.decoded_si_ptr[0] != NULL);
  for(int i = 1; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib2_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib3_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib4_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib5_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib6_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index].all_decoded_sibs.sib11_ptr == NULL);

  // ----------------------------------------------------------------------

  mib_ind.sfn = 64;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_cells_8_5[cell_index],
                                        sizeof(sib1_cells_8_5[cell_index]),
                                        phy_cell_id,
                                        FREQ_1,
                                        65);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
}

typedef enum
{
  LTE_RRC_SIB_TEST_NEXT_MOD_PRD_VALUE_TAGS_DIFF,
  LTE_RRC_SIB_TEST_NEXT_MOD_PRD_VALUE_TAGS_SAME_SIB1_DIFF
} lte_rrc_sib_test_conn_to_idle5_test_e;

void lte_rrc_sib_test_next_mod_prd(lte_rrc_sib_test_conn_to_idle5_test_e test_type)
{
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_cphy_mib_ind_s mib_ind;

  uint32 buf_len;

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;
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
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);

  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_100;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_NORMAL;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;

  mib_ind.freq = FREQ_1;
  mib_ind.phy_cell_id = PHY_CELL_ID_1;

  mib_ind.sfn = 64;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  if(test_type == LTE_RRC_SIB_TEST_NEXT_MOD_PRD_VALUE_TAGS_DIFF)
  {
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_value_tag_change,
                                          sizeof(sib1_2345_mcc_8_5_value_tag_change),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          65);
  }
  else if(test_type == LTE_RRC_SIB_TEST_NEXT_MOD_PRD_VALUE_TAGS_SAME_SIB1_DIFF)
  {
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_value_tag_match_sib1_differ,
                                          sizeof(sib1_2345_mcc_8_5_value_tag_match_sib1_differ),
                                          PHY_CELL_ID_1,
                                          FREQ_1,
                                          65);
  }

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  if(test_type == LTE_RRC_SIB_TEST_NEXT_MOD_PRD_VALUE_TAGS_DIFF)
  {
    TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                       LTE_RRC_SIB3_BITMASK |
                                                       LTE_RRC_SIB4_BITMASK |
                                                       LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                       LTE_RRC_SIB1_BITMASK |
                                                       LTE_RRC_SIB2_BITMASK |
                                                       LTE_RRC_SIB3_BITMASK |
                                                       LTE_RRC_SIB4_BITMASK |
                                                       LTE_RRC_SIB5_BITMASK));

    TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  }
  else if(test_type == LTE_RRC_SIB_TEST_NEXT_MOD_PRD_VALUE_TAGS_SAME_SIB1_DIFF)
  {
    TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
    TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_10);

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

    TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);

  }

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);
  
  
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibDbTest1);

void LteRrcSibDbTest1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibDbTest1::Test()
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

  /* Serving cell */
  lte_rrc_sib_test_populate_db_with_one_cell();

  lte_rrc_sib_db_clear_cell(0);
  lte_rrc_sib.sd_ptr->serv_cell_index = LTE_RRC_SIB_DB_MAX_CELLS;
  lte_rrc_sib_test_camped = FALSE;

  // ---------------------------------------------

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

  /* Non serving cell */
  lte_rrc_sib_test_populate_db_with_one_cell();

  lte_rrc_sib_test_send_stopped_indi();

  // ----------------------------------------------------------------------

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
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);

  // ----------------------------------------------------------------------

  lte_rrc_sib_db_clear_cell(0);

  // ----------------------------------------------------------------------

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

  /* Populate db with all cells - clear rank 0 when serving cell valid */
  lte_rrc_sib_test_populate_db_with_all_cells();

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 9);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].all_si.si_decoded == TRUE);

  lte_rrc_sib_db_clear_cell(9);
  lte_rrc_sib.sd_ptr->serv_cell_index = LTE_RRC_SIB_DB_MAX_CELLS;
  lte_rrc_sib_test_camped = FALSE;

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_valid == FALSE);

  for(int i = 0; i < LTE_RRC_SIB_DB_MAX_CELLS - 1; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 2);
  }

  // ----------------------------------------------------------------------

  /* i cells in db - serving not valid - clear rank 0 */
  lte_rrc_sib_db_clear_cell(8);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[8].index_valid == FALSE);
  for(int i = 0; i < LTE_RRC_SIB_DB_MAX_CELLS - 2; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 3);
  }

  // ----------------------------------------------------------------------

  /* i cells in db - serving not valid - clear max rank */
  lte_rrc_sib_db_clear_cell(0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == FALSE);
  for(int i = 1; i < LTE_RRC_SIB_DB_MAX_CELLS - 2; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 3);
  }

  // ----------------------------------------------------------------------

  /* i cells in db - serving not valid - clear rank i */
  lte_rrc_sib_db_clear_cell(4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[4].index_valid == FALSE);
  for(int i = 1; i < 4; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 4);
  }

  for(int i = 5; i < LTE_RRC_SIB_DB_MAX_CELLS - 2; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 3);
  }

  /* Populate db with all cells - clear rank 0 when serving cell not valid */
  /*lte_rrc_sib_test_resel(9);
  for(int i = 0; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 1);
  }

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 9);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].all_si.si_decoded == TRUE);

  lte_rrc_sib_test_send_stopped_indi();

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == LTE_RRC_SIB_DB_MAX_CELLS);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].all_si.si_decoded == FALSE);

  lte_rrc_sib_db_clear_cell(9);

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_valid == FALSE);

  for(int i = 0; i < LTE_RRC_SIB_DB_MAX_CELLS - 1; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 2);
  }*/

  // ----------------------------------------------------------------------

  /* Populate db with all cells - clear rank 0 when serving cell valid */
  /*lte_rrc_sib_test_resel(9);
  for(int i = 0; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 1);
  }

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 9);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].all_si.si_decoded == TRUE);

  lte_rrc_sib_db_clear_cell(5);

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[5].index_valid == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 9);

  for(int i = 0; i < 5; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 2);
  }

  for(int i = 6; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 1);
  }*/

  // ----------------------------------------------------------------------

  /* Populate db with all cells - clear rank i when serving cell not valid */
  /*lte_rrc_sib_test_resel(9);
  for(int i = 0; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 1);
  }

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 9);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].all_si.si_decoded == TRUE);

  lte_rrc_sib_test_send_stopped_indi();

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == LTE_RRC_SIB_DB_MAX_CELLS);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].all_si.si_decoded == FALSE);

  lte_rrc_sib_db_clear_cell(5);

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[5].index_valid == FALSE);

  for(int i = 0; i < 5; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 2);
  }

  for(int i = 6; i < LTE_RRC_SIB_DB_MAX_CELLS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_rank == LTE_RRC_SIB_DB_MAX_CELLS - i - 1);
  }*/

  // ----------------------------------------------------------------------

  // ----------------------------------------------------------------------

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_sib_test_send_deact_req();
  lte_rrc_utf_reset();
}

void LteRrcSibDbTest1::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibDbTest2);

void LteRrcSibDbTest2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibDbTest2::Test()
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

  /* Populate db with all cells - clear rank 0 when serving cell valid */
  lte_rrc_sib_test_populate_db_with_all_cells();

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 9);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].all_si.si_decoded == TRUE);

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_rank == 9);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_rank == 8);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[2].index_rank == 7);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[3].index_rank == 6);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[4].index_rank == 5);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[5].index_rank == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[6].index_rank == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[7].index_rank == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[8].index_rank == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_rank == 0);


  /* Add eleventh cell to db - will remove cell1 */
  lte_rrc_sib_test_resel_without_asserts(10, 0, 9);

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == TRUE);

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_rank == 9);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[2].index_rank == 8);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[3].index_rank == 7);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[4].index_rank == 6);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[5].index_rank == 5);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[6].index_rank == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[7].index_rank == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[8].index_rank == 2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_rank == 1);
  // ----------------------------------------------------------------------

  int phy_cell_id = 22;

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_cphy_mib_ind_s mib_ind;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_sib_abort_reqi_s abort_reqi;

  uint32 buf_len;
  
  // ---------------------------------------------

  // Prepare abort_reqi to send to SIB
  lte_rrc_init_default_hdr(&abort_reqi.msg_hdr, LTE_RRC_SIB_ABORT_REQI);
  abort_reqi.cell_id.freq = FREQ_1;
  abort_reqi.cell_id.phy_cell_id = 30;

  // Send abort_reqi - ignored
  lte_rrc_utf_send_msg((byte*)&abort_reqi, sizeof(abort_reqi));
  lte_rrc_utf_wait_for_done();

  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(phy_cell_id, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL, TRUE, 
                                LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

  // Prepare mib_ind to send to RRC
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);

  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_100;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_NORMAL;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;

  mib_ind.freq = FREQ_1;
  mib_ind.phy_cell_id = phy_cell_id;

  mib_ind.sfn = 1;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_cells_8_5[1],
                                        sizeof(sib1_cells_8_5[1]),
                                        phy_cell_id,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 1);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_rank == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[2].index_rank == 9);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[3].index_rank == 8);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[4].index_rank == 7);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[5].index_rank == 6);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[6].index_rank == 5);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[7].index_rank == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[8].index_rank == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_rank == 2);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 1;
  // Prepare camped_indi to send to SIB
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_indi.camped_cell.freq = FREQ_1;
  camped_indi.camped_cell.phy_cell_id = phy_cell_id;

  // Send camped_indi
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 1);

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_rank == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[1].index_rank == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[2].index_rank == 9);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[3].index_rank == 8);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[4].index_rank == 7);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[5].index_rank == 6);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[6].index_rank == 5);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[7].index_rank == 4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[8].index_rank == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[9].index_rank == 2);

  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(phy_cell_id,
                                        FREQ_1,
                                        (LTE_RRC_SIB3_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  //---------------------------------------------------------------

  mib_ind.sfn = 64;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_cells_8_5[1],
                                        sizeof(sib1_cells_8_5[1]),
                                        phy_cell_id,
                                        FREQ_1,
                                        65);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibDbTest2::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibDbTest3);

void LteRrcSibDbTest3::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibDbTest3::Test()
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

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;
  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  lte_rrc_sib_test_send_stopped_indi();

  // ----------------------------------------------------------------------

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_cphy_mib_ind_s mib_ind;
  
  uint32 buf_len;

  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                LTE_RRC_NULL_SFN);

  // ----------------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

  // ----------------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // ----------------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK |
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

  // ----------------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // ----------------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK));
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

  // ----------------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);

  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_100;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_NORMAL;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;

  mib_ind.freq = FREQ_1;
  mib_ind.phy_cell_id = PHY_CELL_ID_1;

  mib_ind.sfn = 1;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                LTE_RRC_NULL_SFN);

  // ----------------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

  // ----------------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5_mod_prd_512,
                                        sizeof(sib2_sib3_8_5_mod_prd_512),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        1);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 512);

  TF_ASSERT(sib_sched_req->num_si_msgs == 0);

  //-----------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_value_tag_change,
                                        sizeof(sib1_2345_mcc_8_5_value_tag_change),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK |
                                                     LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK |
                                                     LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 512);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibDbTest3::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibDbTest4);

void LteRrcSibDbTest4::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibDbTest4::Test()
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

  lte_rrc_sib_test_db4(TRUE);

  // ------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibDbTest4::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibDbTest5);

void LteRrcSibDbTest5::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibDbTest5::Test()
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

  lte_rrc_sib_test_db4(FALSE);

  // ------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibDbTest5::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test case to check the correct value of mod prd boundary is used: default 
when event is started, from SIB2 in db when cell is found in db, from new 
SIB2 when it is rcvd

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibDbTest6);

void LteRrcSibDbTest6::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibDbTest6::Test()
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

  lte_rrc_sib_test_db6();

  // ------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibDbTest6::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
SIBs inconsistent - SIB2 not present in the db. Mod prd length should be 64

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibDbTest7);

void LteRrcSibDbTest7::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibDbTest7::Test()
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

  lte_rrc_sib_test_db7();

  // ------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibDbTest7::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibDbTest8);

void LteRrcSibDbTest8::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibDbTest8::Test()
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

  lte_rrc_sib_test_populate_db_with_all_cells_ho();

  // ------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibDbTest8::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibDbTest9);

void LteRrcSibDbTest9::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibDbTest9::Test()
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

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  lte_rrc_sib_test_resel(1, FALSE);

  lte_rrc_sib_test_db9();

  // ------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibDbTest9::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to run validity checks on SIB1

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSib1ValidityTest);

void LteRrcSibSib1ValidityTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSib1ValidityTest::Test()
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

  lte_cphy_sib_sched_req_s *sib_sched_req;
  
  uint32 buf_len;

  lte_rrc_sib_test_camped = FALSE;

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, 100, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == 100);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // ---------------------------------------------

  lte_rrc_sib_test_lte_band_is_supported = FALSE;
  // Band is not supported
  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_cells_8_5[0],
                                        sizeof(sib1_cells_8_5[0]),
                                        PHY_CELL_ID_1,
                                        100,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == 100);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // ---------------------------------------------

  lte_rrc_sib_read_error_indi_s *read_error_ptr;

  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ptr, &buf_len);

  TF_ASSERT(read_error_ptr->msg_hdr.id == LTE_RRC_SIB_READ_ERROR_INDI);
  TF_ASSERT(read_error_ptr->cell_id.freq == 100);
  TF_ASSERT(read_error_ptr->cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(read_error_ptr->sibs_rcvd_curr_bitmask == 0);
  TF_ASSERT(read_error_ptr->csg_indication == FALSE);
  TF_ASSERT(read_error_ptr->intra_freq_resel_allowed == FALSE);

  // ---------------------------------------------

  lte_rrc_sib_test_lte_band_is_supported = TRUE;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, 100, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == 100);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // ---------------------------------------------

  // Freq and FreqBandIndicator do not match but changed to the right band on the fly
  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_cells_8_5[0],
                                        sizeof(sib1_cells_8_5[0]),
                                        PHY_CELL_ID_1,
                                        100,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == 100);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == 100);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // ---------------------------------------------

  // MCC not present in first PLMN in the PLMN list
  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_no_mcc_8_5,
                                        sizeof(sib1_no_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // ---------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ptr, &buf_len);

  TF_ASSERT(read_error_ptr->msg_hdr.id == LTE_RRC_SIB_READ_ERROR_INDI);
  TF_ASSERT(read_error_ptr->cell_id.freq == FREQ_1);
  TF_ASSERT(read_error_ptr->cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(read_error_ptr->sibs_rcvd_curr_bitmask == 0);
  TF_ASSERT(read_error_ptr->csg_indication == FALSE);
  TF_ASSERT(read_error_ptr->intra_freq_resel_allowed == TRUE);

  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
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

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // ---------------------------------------------

  // Invalid SIB to SI mapping
  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_invalid_mapping_8_5,
                                        sizeof(sib1_invalid_mapping_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask != 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask != 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();

  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
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

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // ---------------------------------------------

  // An SI (other than the first SI) empty
  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_invalid_empty_si_8_5,
                                        sizeof(sib1_invalid_empty_si_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask != 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask != 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();

  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
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

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // ---------------------------------------------

  // SIs scheduled in a way which is not feasible
  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_infeasible_schedule_8_5,
                                        sizeof(sib1_infeasible_schedule_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // ---------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ptr, &buf_len);

  TF_ASSERT(read_error_ptr->msg_hdr.id == LTE_RRC_SIB_READ_ERROR_INDI);
  TF_ASSERT(read_error_ptr->cell_id.freq == FREQ_1);
  TF_ASSERT(read_error_ptr->cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(read_error_ptr->sibs_rcvd_curr_bitmask == 0);
  TF_ASSERT(read_error_ptr->csg_indication == FALSE);
  TF_ASSERT(read_error_ptr->intra_freq_resel_allowed == TRUE);

  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
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

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // ---------------------------------------------

  // SI mapping info uses spare values
  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_mapping_spare_value_8_5,
                                        sizeof(sib1_mapping_spare_value_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  // this should be a normal case - spares are ignored
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask != 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask != 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();

  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibSib1ValidityTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

Receive get_sibs_reqi with cause si_update for a non-serving cell with 
cnf_needed = TRUE; cnf with failure should be sent

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibPagingWithCnfTest);

void LteRrcSibPagingWithCnfTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibPagingWithCnfTest::Test()
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

  uint32 buf_len;

  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);

  TF_ASSERT(get_sibs_cnf_ptr->msg_hdr.id == LTE_RRC_GET_SIBS_CNFI);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_FAILURE);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == LTE_RRC_SIB_DB_MAX_CELLS);

  // ----------------------------------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == LTE_RRC_SIB_DB_MAX_CELLS);
  for(int i = 0; i < LTE_RRC_SIB_DB_MAX_CELLS; i++ )
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  
  // ----------------------------------------------------------------------

  /* Asserts on valid cells */

  // ----------------------------------------------------------------------
  
  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset();
}

void LteRrcSibPagingWithCnfTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSib2Test1);

void LteRrcSibSib2Test1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSib2Test1::Test()
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

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  uint32 buf_len;

  // ----------------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2_3_4_mcc_8_5,
                                        sizeof(sib1_2_3_4_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == LTE_RRC_SIB2_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == LTE_RRC_SIB3_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == LTE_RRC_SIB4_BITMASK);
  
  // ----------------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_8_5_mod_prd_1024,
                                        sizeof(sib2_8_5_mod_prd_1024),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 0);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == LTE_RRC_SIB2_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == LTE_RRC_SIB3_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == LTE_RRC_SIB4_BITMASK);
  
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                 LTE_RRC_SIB1_BITMASK |
                                                                 LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                      LTE_RRC_SIB1_BITMASK |
                                                                      LTE_RRC_SIB2_BITMASK));

  // ------------------------------------------------------------

  //Non-mand SIB timer here is for 320ms only
  // Rollover
  TF_SLEEP(200);

  // Prepare bcch_dl_data_ind to send SIB3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib3_8_5,
                                        sizeof(sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 0);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == LTE_RRC_SIB2_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == LTE_RRC_SIB3_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == LTE_RRC_SIB4_BITMASK);
  
  // ------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                         LTE_RRC_SIB1_BITMASK |
                                                                         LTE_RRC_SIB2_BITMASK |
                                                                         LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                              LTE_RRC_SIB1_BITMASK |
                                                                              LTE_RRC_SIB2_BITMASK |
                                                                              LTE_RRC_SIB3_BITMASK));
  // ------------------------------------------------------------

  // Later part of the mod prd window
  TF_SLEEP(70);

  // ------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_8_5,
                                        sizeof(sib4_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        515);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
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
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 0);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == LTE_RRC_SIB2_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == LTE_RRC_SIB3_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == LTE_RRC_SIB4_BITMASK);
  
  // ------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                 LTE_RRC_SIB1_BITMASK |
                                                                 LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                 LTE_RRC_SIB1_BITMASK |
                                                                 LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK));
  // ------------------------------------------------------------
  //SIB not expected in this mod period so will be ignored
  // Prepare bcch_dl_data_ind to send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_8_5_mod_prd_512,
                                        sizeof(sib2_8_5_mod_prd_512),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        516);
  
#if 0
  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
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
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 0);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == LTE_RRC_SIB2_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == LTE_RRC_SIB3_BITMASK);
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == LTE_RRC_SIB4_BITMASK);
  
  // ------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_SIB1_BITMASK |
                                                                 LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK |
                                                                 LTE_RRC_SIB4_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                      LTE_RRC_SIB4_BITMASK));
  // ------------------------------------------------------------
#endif

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibSib2Test1::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test case to reproduce some crash that was seen in testing.

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibMemoryRelatedTest);

void LteRrcSibMemoryRelatedTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibMemoryRelatedTest::Test()
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

  uint32 buf_len;

  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_send_get_sibs_reqi(0, 5200, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  lte_cphy_sib_sched_req_s *sib_sched_req;

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // ---------------------------------------------

  lte_cphy_mib_ind_s mib_ind;

  // Prepare mib_ind to send to RRC
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);

  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_100;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_NORMAL;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;

  mib_ind.freq = 5200;
  mib_ind.phy_cell_id = 0;

  mib_ind.sfn = 184;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 192);

  // ---------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_mem_test_8_5,
                                        sizeof(sib1_mem_test_8_5),
                                        0,
                                        5200,
                                        184);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0x1c);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0x1f);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 192);

  TF_ASSERT(sib_sched_req->num_si_msgs == 2);

  // ---------------------------------------------

  mib_ind.sfn = 188;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  // ---------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5_mod_prd_128,
                                        sizeof(sib2_sib3_8_5_mod_prd_128),
                                        0,
                                        5200,
                                        192);
  
  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibMemoryRelatedTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibNextModPrdTest1);

void LteRrcSibNextModPrdTest1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibNextModPrdTest1::Test()
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

  lte_rrc_sib_test_next_mod_prd(LTE_RRC_SIB_TEST_NEXT_MOD_PRD_VALUE_TAGS_DIFF);

  // ------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibNextModPrdTest1::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibNextModPrdTest2);

void LteRrcSibNextModPrdTest2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibNextModPrdTest2::Test()
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

  lte_rrc_sib_test_next_mod_prd(LTE_RRC_SIB_TEST_NEXT_MOD_PRD_VALUE_TAGS_SAME_SIB1_DIFF);

  // ------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibNextModPrdTest2::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibModPrdTest);

void LteRrcSibModPrdTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibModPrdTest::Test()
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

  lte_rrc_sib_test_camp_phase1();

  uint32 buf_len;

  // Prepare bcch_dl_data_ind to send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_8_5_mod_prd_2048,
                                        sizeof(sib2_8_5_mod_prd_2048),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // ------------------------------------------------------------

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;

  // We expect sib_sched_req, no sib_read_error_indi should be sent
  // mod_prd_boundary_sfn should be 0 in this case
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 0);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
  //                                                                LTE_RRC_SIB3_BITMASK));
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
  //                                                                LTE_RRC_SIB5_BITMASK));

  // ------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset();
}

void LteRrcSibModPrdTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSib1NotRcvdModBndCrossedTest);

void LteRrcSibSib1NotRcvdModBndCrossedTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSib1NotRcvdModBndCrossedTest::Test()
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

  lte_rrc_sib_test_camp_phase1();

  uint32 buf_len;

  // Prepare bcch_dl_data_ind to send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // ------------------------------------------------------------

  lte_cphy_sib_sched_req_s *sib_sched_req;
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                     LTE_RRC_SIB3_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                           LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[2] == 3);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[3] == 3);

  // ------------------------------------------------------------

  lte_cphy_mib_ind_s mib_ind;
  
  // Prepare mib_ind to send to RRC
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);

  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_100;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_NORMAL;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;

  mib_ind.freq = FREQ_1;
  mib_ind.phy_cell_id = PHY_CELL_ID_1;

  mib_ind.sfn = 64;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  // ------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[0] == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[2] == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[3] == LTE_RRC_NULL_SFN);

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset();
}

void LteRrcSibSib1NotRcvdModBndCrossedTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSiRemovalTest1);

void LteRrcSibSiRemovalTest1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSiRemovalTest1::Test()
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

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[2] == 3);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[3] == 3);

  lte_cphy_sib_sched_req_s *sib_sched_req;

  uint32 buf_len;

  // Prepare bcch_dl_data_ind to send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
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

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK |
                                                     LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK |
                                                     LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[2] == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[3] == 64);

  // --------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset();
}

void LteRrcSibSiRemovalTest1::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSiRemovalTest2);

void LteRrcSibSiRemovalTest2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSiRemovalTest2::Test()
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

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();

  lte_cphy_sib_sched_req_s *sib_sched_req;

  uint32 buf_len;

  // Prepare bcch_dl_data_ind to send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK));
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


  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[4] == 4);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[5] == 4);

  // ------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK |
                                                     LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));


  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[4] == 65);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[5] == 65);

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset();
}

void LteRrcSibSiRemovalTest2::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSiRemovalTest3);

void LteRrcSibSiRemovalTest3::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSiRemovalTest3::Test()
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

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();

  lte_cphy_sib_sched_req_s *sib_sched_req;
  
  uint32 buf_len;

  // Prepare bcch_dl_data_ind to send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK));
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

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                LTE_RRC_SIB1_BITMASK |
                                                                LTE_RRC_SIB4_BITMASK |
                                                                LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                     LTE_RRC_SIB1_BITMASK |
                                                                     LTE_RRC_SIB4_BITMASK |
                                                                     LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB4_BITMASK |
                                                                           LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[1] == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[4] == 4);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[5] == 4);


  // ------------------------------------------------------------

  // Prepare bcch_dl_data_ind to send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_8_5,
                                        sizeof(sib4_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK |
                                                     LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));

  //---------------------------------------------------------

  /* As a result of receiving SIB4, SIB3 must be completely removed */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                LTE_RRC_SIB1_BITMASK |
                                                                LTE_RRC_SIB4_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[0] == LTE_RRC_SIB4_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[1] == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[4] == 65);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[5] == LTE_RRC_NULL_SFN);


  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset();
}

void LteRrcSibSiRemovalTest3::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test case to check handling of long SI (dynamic memory is used)

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibLongSiTest1);

void LteRrcSibLongSiTest1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibLongSiTest1::Test()
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

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();

  // ----------------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2_345_mcc_8_5,
                                        sizeof(sib1_2_345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // ----------------------------------------------------------------------

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                  LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == LTE_RRC_SIB_DB_MAX_CELLS);
  for(int i = 0; i < LTE_RRC_SIB_DB_MAX_CELLS; i++ )
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2_345_mcc_8_5));
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[1] == 2);
  for(int i = 2; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == 160 * LTE_RRC_SIB_SIB2_WT_TMR_FACTOR);
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

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                         LTE_RRC_SIB4_BITMASK |
                                                                         LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                                        LTE_RRC_SIB4_BITMASK |
                                                                                        LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_SIB2);

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */

  // ----------------------------------------------------------------------

  // send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_8_5,
                                        sizeof(sib2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_rcv_get_sibs_cnfi(PHY_CELL_ID_1, 
                                     FREQ_1, 
                                     0, 
                                     LTE_RRC_GET_SIBS_SUCCESS);

  // ----------------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
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
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                  LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == LTE_RRC_SIB_DB_MAX_CELLS);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  for(int i = 1; i < LTE_RRC_SIB_DB_MAX_CELLS; i++ )
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2_345_mcc_8_5));
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[2] == 3);
  for(int i = 3; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
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
                                                                       LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                 LTE_RRC_SIB1_BITMASK |
                                                                 LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                      LTE_RRC_SIB1_BITMASK |
                                                                      LTE_RRC_SIB2_BITMASK));

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                         LTE_RRC_SIB4_BITMASK |
                                                                         LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB3_BITMASK |
                                                                               LTE_RRC_SIB4_BITMASK |
                                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK |
                                                                               LTE_RRC_SIB2_BITMASK |
                                                                               LTE_RRC_SIB3_BITMASK |
                                                                               LTE_RRC_SIB4_BITMASK |
                                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 3);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                                        LTE_RRC_SIB4_BITMASK |
                                                                                        LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_SIB3_SIB4);

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
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2_345_mcc_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] != NULL);
  for(int i = 1; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_ptr == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib2);
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

  // send SIB345 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib345_8_5_long,
                                        sizeof(sib345_8_5_long),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  // ----------------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
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
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                  LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == LTE_RRC_SIB_DB_MAX_CELLS);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_valid == TRUE);
  for(int i = 1; i < LTE_RRC_SIB_DB_MAX_CELLS; i++ )
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[i].index_valid == FALSE);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->serv_cell_sibs_consistent == TRUE);

  // ----------------------------------------------------------------------

  /* Asserts on active Events */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2_345_mcc_8_5));
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[2] == 3);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[3] == 4);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[4] == 4);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[5] == 4);
  for(int i = 6; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == 
            (uint32)(LTE_RRC_SIB_MIB_SIB1_WT_TMR + 
                     10*(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end + 1 - 
                         lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp)));
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
                                                                      LTE_RRC_SIB1_BITMASK |
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
                                                                         LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK |
                                                                               LTE_RRC_SIB2_BITMASK |
                                                                               LTE_RRC_SIB3_BITMASK |
                                                                               LTE_RRC_SIB4_BITMASK |
                                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 4);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                                        LTE_RRC_SIB4_BITMASK |
                                                                                        LTE_RRC_SIB5_BITMASK));
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
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2_345_mcc_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr == lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                               LTE_RRC_SIB3_BITMASK |
                                                               LTE_RRC_SIB4_BITMASK |
                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[1] == (LTE_RRC_SIB3_BITMASK |
                                                                        LTE_RRC_SIB4_BITMASK |
                                                                        LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_size == sizeof(sib345_8_5_long));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_ptr != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1] != NULL);
  for(int i = 2; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[i].encoded_si_ptr == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib3_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib4_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[1].u.sib4);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib5_ptr == 
            lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1]->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[2].u.sib5);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);

  // ----------------------------------------------------------------------

  if(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_size > LTE_RRC_MAX_SI_LENGTH)
  {
    lte_rrc_mem_free(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_ptr);
  }

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset();
}

void LteRrcSibLongSiTest1::Teardown()
{
  /* Cannot check for memory leak in SIB heap because the long SI is present in 
     sib db which will fail the assert that mem used in sib heap is 0 */
  lte_rrc_utf_check_sib_mem = FALSE;
  lte_rrc_utf_teardown();
}

uint16 lte_rrc_paging_test_get_paging_cycle_2(void)
{
  return 160; // Dummy value for unit testing
}

/*===========================================================================

                                UNIT TEST

Test next value tag check is not performed if sufficient paging cycles are
available in current modification period.
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibTestValueTagOptimization);

// helper
void lte_rrc_sib_test_value_tag_optimization()
{
  int cell_index = 1;
  int phy_cell_id = cell_index + 21;
  lte_rrc_sib_test_send_get_sibs_reqi(phy_cell_id, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                LTE_RRC_NULL_SFN);

  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_cphy_mib_ind_s mib_ind;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  uint32 buf_len;

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

  // ---------------------------------------------

  /* Asserts on SIB internal data */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_bitmask == 0);
  for(int i = 0; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == TRUE);
  for(int i = 0; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == LTE_RRC_SIB_INVALID_TIME);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                       LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == 0);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB_UPDATE_NEEDED_BITMASK &
                                                                         LTE_RRC_SIB_NON_MANDATORY_SIBS_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == LTE_RRC_SIB_INVALID_WINDOW_LENGTH);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == LTE_RRC_NULL_SFN);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == phy_cell_id);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 0);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == );
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

  // ----------------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);

  mib_ind.mib.num_tx_antennas = 1;
  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_100;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_NORMAL;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;

  mib_ind.freq = FREQ_1;
  mib_ind.phy_cell_id = phy_cell_id;

  mib_ind.sfn = 1;

  // Send mib_ind to RRC
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 
            (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  // Prepare bcch_dl_data_ind to send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_cells_8_5[cell_index],
                                        sizeof(sib1_cells_8_5[cell_index]),
                                        phy_cell_id,
                                        FREQ_1,
                                        2);

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == phy_cell_id);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == cell_index);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK));

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_1);
  TF_ASSERT(sib_sched_req->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));

  // Prepare camped_indi to send to SIB
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_indi.camped_cell.freq = FREQ_1;
  camped_indi.camped_cell.phy_cell_id = phy_cell_id;
  
  // Send camped_indi
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[cell_index - 1].all_si.si_decoded == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == cell_index);

  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(phy_cell_id,
                                        FREQ_1,
                                        (LTE_RRC_SIB3_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK));

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC to stop reading SIBs
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == phy_cell_id);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
}

void LteRrcSibTestValueTagOptimization::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibTestValueTagOptimization::Test()
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

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  lte_rrc_sib_test_resel(1, FALSE);

  lte_rrc_sib_test_value_tag_optimization();

  // ------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibTestValueTagOptimization::Teardown()
{
  lte_rrc_utf_teardown();
}

