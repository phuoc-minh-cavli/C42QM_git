/*!
  @file lte_rrc_sib_mand_sibs_test3.cpp

  @brief
  SIB collection over multiple modification periods without value tag changing
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_mand_sibs_test3.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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

DECLARE_SIB1_2345_MCC_8_5;
DECLARE_SIB2_SIB3_8_5;
DECLARE_SIB4_SIB5_8_5;

extern "C"
{
extern lte_rrc_sib_s lte_rrc_sib;
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/






/*===========================================================================

                                UNIT TEST
Receive get_sibs_req for cell selection, receive SIBs for the cell, mandatory 
SIBs + some optional SIBs are transmitted for the cell, cell not already in db, 
spans multiple modification periods but value tag does not change

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibMandSibsTest3);

void LteRrcSibMandSibsTest3::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibMandSibsTest3::Test()
{
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

  lte_mac_rrc_dl_data_ind_s dl_data_ind_sib2_sib3_st;
  dsm_item_type *dsm_ptr_sib2_sib3_st=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr_sib2_sib3_st;           /* Message attachment pointer */
  
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
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == 0);

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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_MIB_BITMASK);
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
  TF_ASSERT(event_ptr->all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(event_ptr->cause == LTE_RRC_GET_SIBS_CAUSE_CELL_SEL);
  
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  // ----------------------------------------------------------------------

  mib_ind.sfn = 67;
  lte_rrc_utf_send_msg((byte*)&mib_ind, sizeof(mib_ind));
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

  // ----------------------------------------------------------------------

  // should be discarded as already present
  msgr_init_hdr_attach(&dl_data_ind_sib2_sib3_st.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_BCCH_DL_DATA_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr_sib2_sib3_st, sib2_sib3_8_5, sizeof(sib2_sib3_8_5), DSM_DS_SMALL_ITEM_POOL) == sizeof(sib2_sib3_8_5));

  TF_ASSERT(dsm_ptr_sib2_sib3_st != NULL);

  att_ptr_sib2_sib3_st = msgr_get_attach(&dl_data_ind_sib2_sib3_st.hdr, 0);
  msgr_set_dsm_attach(att_ptr_sib2_sib3_st, dsm_ptr_sib2_sib3_st);

  dl_data_ind_sib2_sib3_st.freq = 2000;
  dl_data_ind_sib2_sib3_st.phy_cell_id = 42;
  dl_data_ind_sib2_sib3_st.sfn.sys_fn = 64;
  
  lte_rrc_utf_send_msg((byte*)&dl_data_ind_sib2_sib3_st, sizeof(dl_data_ind_sib2_sib3_st));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == 2000);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == 42);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}


void LteRrcSibMandSibsTest3::Teardown()
{
  lte_rrc_utf_teardown();
}
