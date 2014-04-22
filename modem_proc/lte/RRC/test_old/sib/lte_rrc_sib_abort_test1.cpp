/*!
  @file lte_rrc_sib_abort_test1.cpp

  @brief
  sib_abort_req received during mandatory SIBs collection
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_abort_test1.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/





/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

extern "C"
{
extern lte_rrc_sib_s lte_rrc_sib;
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern boolean lte_rrc_sib_test_wcdma_supported;
extern boolean lte_rrc_sib_test_gsm_supported;
extern boolean lte_rrc_sib_test_cdma_1x_supported;
extern boolean lte_rrc_sib_test_cdma_do_supported;
extern lte_rrc_rf_status_e lte_rrc_sib_test_rf_status;
extern uint64 lte_rrc_sib_test_rf_status_time;
extern lte_l1_sfn_t lte_rrc_sib_test_rf_status_sfn;
extern uint64 lte_rrc_sib_test_wt_tmr_before_rf_unavailable;
extern uint64 lte_rrc_sib_test_wt_tmr_after_rf_available;


/*===========================================================================

                                FUNCTIONS

===========================================================================*/






/*===========================================================================

                                UNIT TEST
sib_abort_req received during mandatory SIBs collection

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibAbortTest1);

void LteRrcSibAbortTest1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibAbortTest1::Test()
{
  lte_rrc_sib_test_wcdma_supported = TRUE;
  lte_rrc_sib_test_gsm_supported = TRUE;
  lte_rrc_sib_test_cdma_1x_supported = TRUE;
  lte_rrc_sib_test_cdma_do_supported = TRUE;

  lte_rrc_get_sibs_reqi_s get_sibs_req;
  
  // ----------------------------------------------------------------------

  lte_cphy_mib_ind_s mib_ind;
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

  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  // ----------------------------------------------------------------------

  //sib_sched_req
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  // ----------------------------------------------------------------------

  /* SIB sched req sent */
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == );
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == );
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == );
  
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == 42);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == 2000);
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == 0);
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                         LTE_RRC_SIB4_BITMASK |
                                                                         LTE_RRC_SIB5_BITMASK |
                                                                         LTE_RRC_SIB6_BITMASK |
                                                                         LTE_RRC_SIB7_BITMASK |
                                                                         LTE_RRC_SIB8_BITMASK |
                                                                         LTE_RRC_SIB9_BITMASK |
                                                                         LTE_RRC_SIB_EMBMS_SIBS_BITMASK|
                                                                         LTE_RRC_SIB16_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == LTE_RRC_SIB_INVALID_WINDOW_LENGTH);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == LTE_RRC_NULL_SFN);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == 42);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == 2000);
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

  // ----------------------------------------------------------------------

  /* mib_ind */
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);
  mib_ind.freq = 2000;
  mib_ind.phy_cell_id = 42;
  mib_ind.sfn = 1;
  mib_ind.mib.num_tx_antennas = 1;
  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_25;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_EXTENDED;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  // ----------------------------------------------------------------------

  /* SIB sched req sent */
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == );
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == );
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == );
  
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == 42);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == 2000);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);

  // ----------------------------------------------------------------------
  
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_25);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_EXTENDED);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_bitmask == 0);
  for(int i = 0; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == 1);
  for(int i = 1; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == LTE_RRC_SIB_MIB_SIB1_WT_TMR);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                       LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == LTE_RRC_MIB_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == LTE_RRC_MIB_BITMASK);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB_UPDATE_NEEDED_BITMASK &
                                                                         LTE_RRC_SIB_NON_MANDATORY_SIBS_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == 42);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == 2000);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 1);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 0);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == );
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */

  // ----------------------------------------------------------------------

  //abort req
  lte_rrc_sib_abort_reqi_s abort_req;
  lte_rrc_init_default_hdr(&abort_req.msg_hdr, LTE_RRC_SIB_ABORT_REQI);
  abort_req.cell_id.phy_cell_id = 42;
  abort_req.cell_id.freq = 2000;

  lte_rrc_utf_send_msg((byte*)&abort_req, sizeof(abort_req));
  lte_rrc_utf_wait_for_done();

  // ----------------------------------------------------------------------

  /* SIB sched req sent */
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == );
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == );
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == );
  
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


void LteRrcSibAbortTest1::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibAbortTest1WithDSDS);

void LteRrcSibAbortTest1WithDSDS::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibAbortTest1WithDSDS::Test()
{
  lte_rrc_sib_test_wcdma_supported = TRUE;
  lte_rrc_sib_test_gsm_supported = TRUE;
  lte_rrc_sib_test_cdma_1x_supported = TRUE;
  lte_rrc_sib_test_cdma_do_supported = TRUE;

  lte_rrc_get_sibs_reqi_s get_sibs_req;
  
  // ----------------------------------------------------------------------

  lte_cphy_mib_ind_s mib_ind;
  // ----------------------------------------------------------------------

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_wt_tmr_before_rf_unavailable = lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr);
  TF_MSG("Sending RF_UNAVAILABLE_IND");
  lte_rrc_sib_test_rf_status_sfn = 1;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_UNAVAILABLE;
  lte_rrc_sib_test_send_rf_unavail_ind();

  lte_rrc_utf_wait_for_done();

  /* get sibs req */
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
  lte_rrc_sib_test_rf_status_sfn = 21;
  lte_rrc_sib_test_rf_status = LTE_RRC_RF_AVAILABLE;
  lte_rrc_sib_test_send_rf_avail_ind();
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  // ----------------------------------------------------------------------

  //sib_sched_req
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  // ----------------------------------------------------------------------

  /* SIB sched req sent */
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
														LTE_RRC_SIB1_BITMASK));  //PLEASE CHECK IF THIS IS EXPECTED BEHAVIOR FOR NEXT_SIB_MASK
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == );
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == );
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == );
  
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == 42);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == 2000);
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == 0);
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                         LTE_RRC_SIB4_BITMASK |
                                                                         LTE_RRC_SIB5_BITMASK |
                                                                         LTE_RRC_SIB6_BITMASK |
                                                                         LTE_RRC_SIB7_BITMASK |
                                                                         LTE_RRC_SIB8_BITMASK |
                                                                         LTE_RRC_SIB9_BITMASK |
                                                                         LTE_RRC_SIB_EMBMS_SIBS_BITMASK|
                                                                         LTE_RRC_SIB16_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == LTE_RRC_SIB_INVALID_WINDOW_LENGTH);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == LTE_RRC_NULL_SFN);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == 42);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == 2000);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 0);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == );
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == );

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1); //PLEASE CHECK WHY IS IT NOT LTE_RRC_SIB_EVENT_TMR_MIB_SIB1

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */

  // ----------------------------------------------------------------------

  /* mib_ind */
  msgr_init_hdr_attach(&mib_ind.msgr_hdr, MSGR_LTE_CPHY, LTE_CPHY_MIB_IND, 0, 0);
  mib_ind.freq = 2000;
  mib_ind.phy_cell_id = 42;
  mib_ind.sfn = 1;
  mib_ind.mib.num_tx_antennas = 1;
  mib_ind.mib.dl_bandwidth = LTE_BW_NRB_25;
  mib_ind.mib.phich_info.phich_duration = LTE_L1_PHICH_DURATION_EXTENDED;
  mib_ind.mib.phich_info.phich_resource = LTE_L1_PHICH_RESOURCE_ONE;
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
  lte_rrc_utf_wait_for_done();

  // ----------------------------------------------------------------------

  /* SIB sched req sent */
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == );
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == );
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == );
  
  // ----------------------------------------------------------------------

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


    /* SIB sched req sent */
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));


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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.phy_cell_id == 42);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cell_id.freq == 2000);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].req_proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == LTE_RRC_SIB_DB_MAX_CELLS);

  // ----------------------------------------------------------------------
  
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_25);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_EXTENDED);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_bitmask == 0);
  for(int i = 0; i < LTE_RRC_SIB_MAX_SIS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[i] == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_size == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.decoded_si_ptr[i] == NULL);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == 1);
  for(int i = 1; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
  {
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[i] == LTE_RRC_NULL_SFN);
  }
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].last_sfn_rem_time == LTE_RRC_SIB_MIB_SIB1_WT_TMR);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == LTE_RRC_SIB_ALL_SIBS_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK |
                                                                       LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == LTE_RRC_MIB_BITMASK);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_curr_bitmask == LTE_RRC_MIB_BITMASK);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cnf_needed == TRUE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].get_sibs_cnf_sent == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_serv_cell == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB_UPDATE_NEEDED_BITMASK &
                                                                         LTE_RRC_SIB_NON_MANDATORY_SIBS_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == 42);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == 2000);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 1);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 0);
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == );
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == );
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);

  // ----------------------------------------------------------------------

  /* Asserts on valid cells */

  // ----------------------------------------------------------------------

  //abort req
  lte_rrc_sib_abort_reqi_s abort_req;
  lte_rrc_init_default_hdr(&abort_req.msg_hdr, LTE_RRC_SIB_ABORT_REQI);
  abort_req.cell_id.phy_cell_id = 42;
  abort_req.cell_id.freq = 2000;

  lte_rrc_utf_send_msg((byte*)&abort_req, sizeof(abort_req));
  lte_rrc_utf_wait_for_done();

  // ----------------------------------------------------------------------

  /* SIB sched req sent */
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  //TF_ASSERT(sib_sched_req_ptr->si_window_length == );
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == );
  //TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == );
  
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


void LteRrcSibAbortTest1WithDSDS::Teardown()
{
  lte_rrc_utf_teardown();
}
