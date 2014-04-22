/*!
  @file lte_rrc_sib_wt_tmr_tests.cpp

  @brief
  SIB wait timer related test cases
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_wt_tmr_tests.cpp#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/24/11   zz      Added no RLF should be declared when non-mandantory SIBs       
                   are read in CONNECTED state 
02/23/10   amit    Initial Version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "__lte_rrc_sib.h"
#include "lte_rrc_sibi.h"
#include "lte_rrc_irat_to_1x_mgr.h"
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
DECLARE_SIB1_23_16_MCC_8_5;
DECLARE_SIB1_2345_MCC_8_5_SIB2_PER_32;
DECLARE_SIB2_SIB3_8_5;
DECLARE_SIB4_SIB5_8_5;
DECLARE_SIB1_236_MCC_8_5;
DECLARE_SIB6_8_5;
DECLARE_SIB1_2345_10_11w128_MCC_8_5;
DECLARE_SIB10_8_5;
DECLARE_SIB11_SEG1OF2_8_5;
DECLARE_SIB11_SEG2OF2_8_5;
DECLARE_SIB1_23_8_MCC_8_5;
DECLARE_SIB1_2345_10_11_MCC_8_5;
DECLARE_SIB2_8_5;
DECLARE_SIB1_2345_10_11w64_MCC_8_5;

extern "C"
{
extern lte_rrc_sib_s lte_rrc_sib;
TF_PROTO_STUB(void, lte_rrc_irat_to_1x_mgr_send_sib8_unavailable_to_1x, void);

}
/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern boolean lte_rrc_sib_test_camped;
extern boolean lte_rrc_sib_test_wcdma_supported;
extern boolean lte_rrc_sib_test_gsm_supported;
extern boolean lte_rrc_sib_test_cdma_1x_supported;
extern boolean lte_rrc_sib_test_cdma_do_supported;
extern boolean lte_rrc_sib_test_embms_is_preferred;
extern boolean lte_rrc_sib_test_embms_is_enabled;
extern int lte_rrc_sib_test_sib_db_key;
extern lte_phy_cell_id_t lte_rrc_sib_test_serv_cell_phy_cell_id;
extern lte_earfcn_t lte_rrc_sib_test_serv_cell_freq;

/*===========================================================================

                                FUNCTIONS

===========================================================================*/


/*===========================================================================

                                UNIT TEST
Test to check sib timer expiry while mandatory SIBs are being read

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibWtTmrTest1);

void LteRrcSibWtTmrTest1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibWtTmrTest1::Test()
{
  lte_rrc_sib_test_camped = FALSE;
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
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);

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

  //tmr expiry
  TF_SLEEP(LTE_RRC_SIB_MIB_SIB1_WT_TMR);

  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));
  TF_ASSERT(read_error_ind_ptr->cause == LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT);
  TF_ASSERT(read_error_ind_ptr->csg_indication == FALSE);
  TF_ASSERT(read_error_ind_ptr->intra_freq_resel_allowed == TRUE);


  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);
  
  // ----------------------------------------------------------------------

  lte_rrc_utf_reset();
}


void LteRrcSibWtTmrTest1::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check SIB timer expiry while non-mandatory SIBs are being read and UE 
has not camped on the cell yet

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibWtTmrTest2);

void LteRrcSibWtTmrTest2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibWtTmrTest2::Test()
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
  
  lte_mac_rrc_dl_data_ind_s dl_data_ind_sib2_sib3;
  dsm_item_type *dsm_ptr_sib2_sib3=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr_sib2_sib3;           /* Message attachment pointer */

  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
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
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
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
  dl_data_ind_sib2_sib3.sfn.sys_fn = 5;


  lte_rrc_utf_send_msg((byte*)&dl_data_ind_sib2_sib3, sizeof(dl_data_ind_sib2_sib3));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == 2000);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == 42);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
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

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));

  // ----------------------------------------------------------------------

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

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == FALSE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == LTE_RRC_SIB_DB_MAX_CELLS);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);

  // ----------------------------------------------------------------------

  lte_rrc_utf_reset();
}


void LteRrcSibWtTmrTest2::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check SIB timer expiry while non-mandatory SIBs are being read but UE 
has already camped on the cell

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibWtTmrTest3);

void LteRrcSibWtTmrTest3::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibWtTmrTest3::Test()
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
  dl_data_ind_sib2_sib3.sfn.sys_fn = 5;


  lte_rrc_utf_send_msg((byte*)&dl_data_ind_sib2_sib3, sizeof(dl_data_ind_sib2_sib3));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == 2000);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == 42);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
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

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));

  // ----------------------------------------------------------------------

  //camped_ind
  lte_rrc_camped_indi_s camped_ind;

  lte_rrc_init_default_hdr(&camped_ind.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_ind.camped_cell.freq = 2000;
  camped_ind.camped_cell.phy_cell_id = 42;

  lte_rrc_utf_send_msg((byte*)&camped_ind, sizeof(camped_ind));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);

  // ----------------------------------------------------------------------

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

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->freq == 2000);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == 42);

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.si_decoded == TRUE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->serv_cell_index == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);

  // ----------------------------------------------------------------------


  /* SIB4_SIB5 */
  msgr_init_hdr_attach(&dl_data_ind_sib4_sib5.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_BCCH_DL_DATA_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr_sib4_sib5, sib4_sib5_8_5, sizeof(sib4_sib5_8_5), DSM_DS_SMALL_ITEM_POOL) == sizeof(sib4_sib5_8_5));

  TF_ASSERT(dsm_ptr_sib4_sib5 != NULL);

  att_ptr_sib4_sib5 = msgr_get_attach(&dl_data_ind_sib4_sib5.hdr, 0);
  msgr_set_dsm_attach(att_ptr_sib4_sib5, dsm_ptr_sib4_sib5);

  dl_data_ind_sib4_sib5.freq = 2000;
  dl_data_ind_sib4_sib5.phy_cell_id = 42;
  dl_data_ind_sib4_sib5.sfn.sys_fn = 7;


  lte_rrc_utf_send_msg((byte*)&dl_data_ind_sib4_sib5, sizeof(dl_data_ind_sib4_sib5));
  lte_rrc_utf_wait_for_done();

  //sib_updated_ind

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibWtTmrTest3::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Timer for SIB2 expires while reading SIBs

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSib2TimerExpiryTest);

void LteRrcSibSib2TimerExpiryTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSib2TimerExpiryTest::Test()
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

  /* SIB1 saying that SIB2 periodicity is something other than 160 */
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;

  uint32 buf_len;

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_sib2_per_32,
                                        sizeof(sib1_2345_mcc_8_5_sib2_per_32),
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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_32);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  /* SIB2 timer expiry */
  TF_SLEEP((320 * LTE_RRC_SIB_SIB2_WT_TMR_FACTOR) - 1);
  lte_rrc_utf_check_no_more_msgs();
  TF_SLEEP(1);

  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;

  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
  TF_ASSERT(read_error_ind_ptr->msg_hdr.id == LTE_RRC_SIB_READ_ERROR_INDI);
  TF_ASSERT(read_error_ind_ptr->cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(read_error_ind_ptr->cell_id.freq == FREQ_1);
  TF_ASSERT(read_error_ind_ptr->cause == LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT);
  TF_ASSERT(read_error_ind_ptr->sibs_rcvd_curr_bitmask == LTE_RRC_SIB_MIB_SIB1_BITMASK);
  TF_ASSERT(read_error_ind_ptr->csg_indication == TRUE);
  TF_ASSERT(read_error_ind_ptr->intra_freq_resel_allowed == TRUE);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibSib2TimerExpiryTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check SIB timer expiry while unnecessary non-mandatory SIBs (SIB6 or 
SIB7) are being read in CONNECTED state

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSib6or7TimeoutIgnoreWhenConnected);
void LteRrcSibSib6or7TimeoutIgnoreWhenConnected::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSib6or7TimeoutIgnoreWhenConnected::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf;
  lte_rrc_tmri_s *tmr_ptr;

  uint32 buf_len;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  
  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_236_mcc_8_5,
                                        sizeof(sib1_236_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB6_BITMASK ));
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB6_BITMASK ));
  TF_ASSERT(sib_sched_req->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_64);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_128);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB6_BITMASK));
  
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

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB6_BITMASK));
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB6_BITMASK ));
  TF_ASSERT(sib_sched_req->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_64);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_128);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB6_BITMASK));

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // Send sib_updated_indi
  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        LTE_RRC_SIB3_BITMASK,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB6_BITMASK ));

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

#if 0
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == LTE_RRC_SIB6_BITMASK);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB6_BITMASK ));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
#endif
  //---------------------------------------------------------------

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
                                                     LTE_RRC_SIB6_BITMASK ));

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB6_BITMASK ));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_236_mcc_8_5,
                                        sizeof(sib1_236_mcc_8_5),
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

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == LTE_RRC_SIB6_BITMASK);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB6_BITMASK ));

  //---------------------------------------------------------------

  //tmr expiry for SIB6
  TF_SLEEP(LTE_RRC_SIB_NON_MAND_SIBS_WT_TMR);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);

  //---------------------------------------------------------------

  // No sib_read_error_indi should be sent
  lte_rrc_utf_check_no_more_msgs();

}

void LteRrcSibSib6or7TimeoutIgnoreWhenConnected::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to check SIB8 timer expiry in CONNECTED state, sib_read_error_indi need 
to be sent in this case 

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSib8TimeoutWhenConnected);
void LteRrcSibSib8TimeoutWhenConnected::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
  TF_STUB(lte_rrc_irat_to_1x_mgr_send_sib8_unavailable_to_1x) = lte_rrc_sib_test_stub_send_sib8_unavailable_to_1x;
}

void LteRrcSibSib8TimeoutWhenConnected::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf;
  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  lte_rrc_tmri_s *tmr_ptr;

  uint32 buf_len;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  
  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_23_8_mcc_8_5,
                                        sizeof(sib1_23_8_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK ));
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK ));
  TF_ASSERT(sib_sched_req->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB8_BITMASK));
  
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

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK ));
  TF_ASSERT(sib_sched_req->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB8_BITMASK));

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // Send sib_updated_indi
  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        LTE_RRC_SIB3_BITMASK,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB8_BITMASK ));

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

#if 0
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == LTE_RRC_SIB8_BITMASK);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB8_BITMASK ));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
#endif
  //---------------------------------------------------------------

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
                                                     LTE_RRC_SIB8_BITMASK ));

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB8_BITMASK ));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_23_8_mcc_8_5,
                                        sizeof(sib1_23_8_mcc_8_5),
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

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == LTE_RRC_SIB8_BITMASK);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB8_BITMASK ));

  //---------------------------------------------------------------

  //tmr expiry for SIB8
  TF_SLEEP(LTE_RRC_SIB_NON_MAND_SIBS_WT_TMR);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));
  TF_ASSERT(read_error_ind_ptr->cause == LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT);
  TF_ASSERT(read_error_ind_ptr->csg_indication == TRUE);
  TF_ASSERT(read_error_ind_ptr->intra_freq_resel_allowed == TRUE);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);

  //---------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();

}

void LteRrcSibSib8TimeoutWhenConnected::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to check SIB timer expiry while ETWS are being read in CONNECTED state, 
sib_read_error_indi need to be sent in this case 

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibETWSTimeoutWhenConnected);
void LteRrcSibETWSTimeoutWhenConnected::Setup()
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

void LteRrcSibETWSTimeoutWhenConnected::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf;
  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  uint32 buf_len;

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
                                        3);

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
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

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

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB_ETWS_SIBS_BITMASK);
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

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

/*
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB_ETWS_SIBS_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);

  //---------------------------------------------------------------
*/
  
  TF_SLEEP(320);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

  //Make sure ETWS timer is restarted for remaing time
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr) == 
            (6080));

  /* Since ETWS timer is less than 20s in this case, this is also ETWS timeout */
  TF_SLEEP(6080);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));
  TF_ASSERT(read_error_ind_ptr->cause == LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT);
  TF_ASSERT(read_error_ind_ptr->csg_indication == TRUE);
  TF_ASSERT(read_error_ind_ptr->intra_freq_resel_allowed == TRUE);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);

  //---------------------------------------------------------------

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();
}

void LteRrcSibETWSTimeoutWhenConnected::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Non-mandatary sibs timeout in IDLE state with ETWS in sib bitmask, expect 
sib_read_error_indi

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibNonMndSibsTimeoutWithEtws);
void LteRrcSibNonMndSibsTimeoutWithEtws::Setup()
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

void LteRrcSibNonMndSibsTimeoutWithEtws::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf;
  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  uint32 buf_len;

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
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11w128_mcc_8_5,
                                        sizeof(sib1_2345_10_11w128_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

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
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_32);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_128);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

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

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_32);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_128);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  //---------------------------------------------------------------

  /* SIB4 & SIB5 timeout */
  TF_SLEEP(LTE_RRC_SIB_NON_MAND_SIBS_WT_TMR);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);
 
  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));
  TF_ASSERT(read_error_ind_ptr->cause == LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT);
  TF_ASSERT(read_error_ind_ptr->csg_indication == TRUE);
  TF_ASSERT(read_error_ind_ptr->intra_freq_resel_allowed == TRUE);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  
  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();
}

void LteRrcSibNonMndSibsTimeoutWithEtws::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
20s timer timeout in IDLE state while non-mandatary sibs rcvd with ETWS in 
sib bitmask, no sib_read_error_indi should be sent, restart the timer for 
ETWS and rcvd ETWS msg in time 

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibNonMndSibsRcvdWithEtws);
void LteRrcSibNonMndSibsRcvdWithEtws::Setup()
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

void LteRrcSibNonMndSibsRcvdWithEtws::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf;
  lte_rrc_tmri_s *tmr_ptr;
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

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11w128_mcc_8_5,
                                        sizeof(sib1_2345_10_11w128_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

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
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_32);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_128);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

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

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_32);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_128);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

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

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB_ETWS_SIBS_BITMASK);
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

  //---------------------------------------------------------------

  /* 20s timeout, no sib_read_error_indi should be sent */
  TF_SLEEP(LTE_RRC_SIB_NON_MAND_SIBS_WT_TMR);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_32);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_128);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11w128_mcc_8_5,
                                        sizeof(sib1_2345_10_11w128_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB_ETWS_SIBS_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB11_BITMASK);
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
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_32);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_128);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();
}

void LteRrcSibNonMndSibsRcvdWithEtws::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
ETWS timeout in IDLE state, expect sib_read_error_indi 

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsTimeout);
void LteRrcSibEtwsTimeout::Setup()
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

void LteRrcSibEtwsTimeout::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf;
  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  uint32 buf_len;

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
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11w64_mcc_8_5,
                                        sizeof(sib1_2345_10_11w64_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

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
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_32);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_64);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

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

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_32);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_64);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

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

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB_ETWS_SIBS_BITMASK);
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

  //---------------------------------------------------------------

  /* 20s timeout, no sib_read_error_indi should be sent at this time */
  TF_SLEEP(LTE_RRC_SIB_NON_MAND_SIBS_WT_TMR);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);
  
  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();
  
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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_32);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_64);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11w64_mcc_8_5,
                                        sizeof(sib1_2345_10_11w64_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB_ETWS_SIBS_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB11_BITMASK);
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

  /* SIB11 timeout */
  TF_SLEEP(55000);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);
 
  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));
  TF_ASSERT(read_error_ind_ptr->cause == LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT);
  TF_ASSERT(read_error_ind_ptr->csg_indication == TRUE);
  TF_ASSERT(read_error_ind_ptr->intra_freq_resel_allowed == TRUE);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
}

void LteRrcSibEtwsTimeout::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
ETWS timeout in CONNECTED state with timer above 20s, expect sib_read_error_indi 

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsAbove20sTimeoutWhenConnected);
void LteRrcSibEtwsAbove20sTimeoutWhenConnected::Setup()
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

void LteRrcSibEtwsAbove20sTimeoutWhenConnected::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf;
  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  uint32 buf_len;

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
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11w64_mcc_8_5,
                                        sizeof(sib1_2345_10_11w64_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

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
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_32);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_64);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

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

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_32);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_64);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB_ETWS_SIBS_BITMASK);
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

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();
/*
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB_ETWS_SIBS_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);

  //---------------------------------------------------------------
*/
  /* 20s timeout, no sib_read_error_indi should be sent at this time */
  TF_SLEEP(LTE_RRC_SIB_NON_MAND_SIBS_WT_TMR);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);
  
  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();
  
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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB_ETWS_SIBS_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_32);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB10_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_64);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB11_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11w64_mcc_8_5,
                                        sizeof(sib1_2345_10_11w64_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB_ETWS_SIBS_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB_ETWS_SIBS_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB11_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB11_BITMASK));
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

  /* SIB11 timeout */
  TF_SLEEP(55000);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);
 
  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));
  TF_ASSERT(read_error_ind_ptr->cause == LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT);
  TF_ASSERT(read_error_ind_ptr->csg_indication == TRUE);
  TF_ASSERT(read_error_ind_ptr->intra_freq_resel_allowed == TRUE);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
}

void LteRrcSibEtwsAbove20sTimeoutWhenConnected::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Mandatory sib not rcvd when non-mandatory timer expiry in CONNECTED state, 
expect sib_read_error_indi 

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibMndNotRcvdWhileNonMndTmrTimeoutWhenConnected);
void LteRrcSibMndNotRcvdWhileNonMndTmrTimeoutWhenConnected::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibMndNotRcvdWhileNonMndTmrTimeoutWhenConnected::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  uint32 buf_len;

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4(); 

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  //---------------------------------------------------------------
  
  // No sched req should be sent
  lte_rrc_utf_check_no_more_msgs();
  
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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();
/*
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK ) );
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
*/
  // send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_8_5,
                                        sizeof(sib2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
  	                                                     LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
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

  /* 20s timeout, a sib_read_error_indi should be sent at this time */
  TF_SLEEP(LTE_RRC_SIB_NON_MAND_SIBS_WT_TMR);

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

#if 0 
  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
  TF_ASSERT(read_error_ind_ptr->msg_hdr.id == LTE_RRC_SIB_READ_ERROR_INDI);
  TF_ASSERT(read_error_ind_ptr->cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(read_error_ind_ptr->cell_id.freq == FREQ_1);
  TF_ASSERT(read_error_ind_ptr->cause == LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT);
  TF_ASSERT(read_error_ind_ptr->sibs_rcvd_curr_bitmask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(read_error_ind_ptr->csg_indication == TRUE);
  TF_ASSERT(read_error_ind_ptr->intra_freq_resel_allowed == TRUE);
#endif

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0 ));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
}

void LteRrcSibMndNotRcvdWhileNonMndTmrTimeoutWhenConnected::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to check non-mandatary sib timer expiry while mandatory SIBs are being 
read when connected

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibMndNotRcvdWhileNonMndTmrTimeoutWhenConnected2);

void LteRrcSibMndNotRcvdWhileNonMndTmrTimeoutWhenConnected2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibMndNotRcvdWhileNonMndTmrTimeoutWhenConnected2::Test()
{
    lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
    lte_rrc_get_sibs_cnfi_s *get_sibs_cnf;
    lte_rrc_tmri_s *tmr_ptr;
    lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
    uint32 buf_len;

    lte_rrc_sib_test_camp_phase1();
    lte_rrc_sib_test_camp_phase2();
    lte_rrc_sib_test_camp_phase3();
    lte_rrc_sib_test_camp_phase4();

    //camped_indi
    lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

    //---------------------------------------------------------------

    // No sched req should be sent
    lte_rrc_utf_check_no_more_msgs();

    //---------------------------------------------------------------

    // Set the cause of next get_sibs_reqi to be etws, event parameters will be changed
    // after this request
    lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        LTE_RRC_PROC_SIB,
                                        LTE_RRC_GET_SIBS_CAUSE_ETWS,
                                        FALSE,
                                        4);

    // We expect a sib_sched_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
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

    // send SIB1 to RRC
    lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        5);

    // We expect a sib_sched_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK ));
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK |
                                                           LTE_RRC_SIB5_BITMASK));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 5);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);

    // Send connected_indi
    lte_rrc_sib_test_send_connected_indi();
/*
    // We expect a sib_sched_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB4_BITMASK);
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                           LTE_RRC_SIB1_BITMASK |
                                                           LTE_RRC_SIB2_BITMASK |
                                                           LTE_RRC_SIB3_BITMASK |
                                                           LTE_RRC_SIB4_BITMASK ));
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 5);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
*/
    //tmr expiry
    TF_SLEEP(LTE_RRC_SIB_NON_MAND_SIBS_WT_TMR);

    lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
    TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
    TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

#if 0
    lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
    TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));
    TF_ASSERT(read_error_ind_ptr->cause == LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT);
    TF_ASSERT(read_error_ind_ptr->csg_indication == TRUE);
    TF_ASSERT(read_error_ind_ptr->intra_freq_resel_allowed == TRUE);
#endif

    // We expect a sib_sched_req message from RRC
    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
    TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

    TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
    TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
    TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);
    TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
    TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 5);
    TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
    TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
}


void LteRrcSibMndNotRcvdWhileNonMndTmrTimeoutWhenConnected2::Teardown()
{
  lte_rrc_utf_teardown();
}



/*===========================================================================

                                UNIT TEST
Test to check SIB16 timer expiry in CONNECTED state
Ignore when eMBMS is not prefered
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSib16TimeoutWhenConnected);
void LteRrcSibSib16TimeoutWhenConnected::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
	  LTE_RRC_UTC_TIME_UPDATE_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSib16TimeoutWhenConnected::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_rrc_utc_time_update_ind_s *utc_time_update_ind;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf;
  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_sib_test_embms_is_preferred = FALSE;

  uint32 buf_len;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  
  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_23_16_mcc_8_5,
                                        sizeof(sib1_23_16_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK ));
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK ));
  TF_ASSERT(sib_sched_req->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB16_BITMASK));
  
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

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK ));
  TF_ASSERT(sib_sched_req->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB16_BITMASK));

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // Send sib_updated_indi
  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        LTE_RRC_SIB3_BITMASK,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB16_BITMASK ));

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();


  //---------------------------------------------------------------

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
                                                     LTE_RRC_SIB16_BITMASK ));

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB16_BITMASK ));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_23_16_mcc_8_5,
                                        sizeof(sib1_23_16_mcc_8_5),
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

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == LTE_RRC_SIB16_BITMASK);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB16_BITMASK ));

  //---------------------------------------------------------------

  //tmr expiry for SIB16
  
   TF_SLEEP(LTE_RRC_SIB_NON_MAND_SIBS_WT_TMR);
  
  
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC


  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_ind, &buf_len);
  TF_ASSERT(utc_time_update_ind->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND );
  TF_ASSERT(utc_time_update_ind->utc_sib16_acquired == FALSE);

  
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);  
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);  
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);

  //---------------------------------------------------------------

  //---------------------------------------------------------------

  // No sib_read_error_indi should be sent
  lte_rrc_utf_check_no_more_msgs();
  

}

void LteRrcSibSib16TimeoutWhenConnected::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check SIB16 timer expiry in CONNECTED state
Ignore when eMBMS is prefered
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSib16TimeoutWhenConnectedEmbmsPreferred);
void LteRrcSibSib16TimeoutWhenConnectedEmbmsPreferred::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
	  LTE_RRC_UTC_TIME_UPDATE_IND
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSib16TimeoutWhenConnectedEmbmsPreferred::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_rrc_utc_time_update_ind_s *utc_time_update_ind;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf;
  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  lte_rrc_tmri_s *tmr_ptr;

  uint32 buf_len;
 lte_rrc_sib_test_embms_is_preferred = TRUE;
 lte_rrc_sib_test_embms_is_enabled = TRUE;
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  
  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_23_16_mcc_8_5,
                                        sizeof(sib1_23_16_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK ));
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK ));
  TF_ASSERT(sib_sched_req->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB16_BITMASK));
  
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

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB16_BITMASK ));
  TF_ASSERT(sib_sched_req->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB16_BITMASK));

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // Send sib_updated_indi
  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        LTE_RRC_SIB3_BITMASK,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB16_BITMASK ));

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();


  //---------------------------------------------------------------

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
                                                     LTE_RRC_SIB16_BITMASK ));

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB16_BITMASK ));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_23_16_mcc_8_5,
                                        sizeof(sib1_23_16_mcc_8_5),
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

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == LTE_RRC_SIB16_BITMASK);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB16_BITMASK ));

  //---------------------------------------------------------------

  //tmr expiry for SIB16
  
   TF_SLEEP(LTE_RRC_SIB_NON_MAND_SIBS_WT_TMR);
  
  
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

  //---------------------------------------------------------------
  
    lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
	TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));
	TF_ASSERT(read_error_ind_ptr->cause == LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT);
	TF_ASSERT(read_error_ind_ptr->csg_indication == TRUE);
	TF_ASSERT(read_error_ind_ptr->intra_freq_resel_allowed == TRUE);

  // We expect a sib_sched_req message from RRC


  lte_rrc_utf_get_next_ext_msg((byte**)&utc_time_update_ind, &buf_len);
  TF_ASSERT(utc_time_update_ind->msg_hdr.id == LTE_RRC_UTC_TIME_UPDATE_IND );
  TF_ASSERT(utc_time_update_ind->utc_sib16_acquired == FALSE);

  
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);  
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);  
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);

  //---------------------------------------------------------------

  //---------------------------------------------------------------

  // No sib_read_error_indi should be sent
  lte_rrc_utf_check_no_more_msgs();
  

}

void LteRrcSibSib16TimeoutWhenConnectedEmbmsPreferred::Teardown()
{
  lte_rrc_utf_teardown();
}

