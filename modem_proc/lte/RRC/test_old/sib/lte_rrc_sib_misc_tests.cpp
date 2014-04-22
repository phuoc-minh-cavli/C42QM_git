/*!
  @file lte_rrc_sib_misc_tests.cpp

  @brief
  Miscellaneous SIB unit test cases
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_misc_tests.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/12   ap      Added a test case to validate RRC behavior when unsupported SIBs are present in SI message
02/05/10   amit    Initial Version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "__lte_rrc_sib.h"
#include "lte_rrc_sibi.h"
#include "__lte_rrc_controller.h"
#include "lte_rrc_utils.h"
#include <fs_public.h>
#include "lte_misc.h"
#include "lte_rrc_llci.h"
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
DECLARE_SIB1_2_3_4_MCC_8_5;
DECLARE_SIB1_2345_MCC_8_5_VALUE_TAG_CHANGE;
DECLARE_SIB1_234567_MCC_8_5;
DECLARE_SIB1_2345_8_MCC_8_5;
DECLARE_SIB1_2345_MCC_8_5_CELL2;
DECLARE_SIB1_2345_8_MCC_8_5_CELL2;
DECLARE_SIB2_8_5;
DECLARE_SIB2_8_5_MOD_PRD_512;
DECLARE_SIB2_8_5_MOD_PRD_2048;
DECLARE_SIB2_SIB3_8_5;
DECLARE_SIB2_SIB3_8_5_MOD_PRD_512;
DECLARE_SIB2_SIB3_MOD_PRD_CHANGE_8_5;
DECLARE_SIB2_SIB3_DIFF_8_5;
DECLARE_SIB2_SIB3_8_5_MOD_PRD_128;
DECLARE_SIB3_8_5;
DECLARE_SIB345_8_5_LONG;
DECLARE_SIB4_8_5;
DECLARE_SIB4_SIB5_8_5;
DECLARE_SIB6_8_5;
DECLARE_SIB1_WITH_SIB13_8_5;
DECLARE_SIB13_8_5;
DECLARE_SIB8_8_5;
DECLARE_SIB1_2_MCC_8_5;
DECLARE_SI_NONCOMPREHENDED_8_5;
DECLARE_SIB_NOT_SUPPORTED_8_5;
DECLARE_SIB1_WITH_SIB13_8_VT2;
DECLARE_SIB1_2345_MCC_8_5_V1;
DECLARE_SIB1_2310_45_11_MCC_8_5;
DECLARE_SIB1_2345_10_11_MCC_8_5;
DECLARE_SIB2_SIB3_SIB10_8_5;
DECLARE_SIB11_8_5;
DECLARE_SIB1_2310_45_11_MCC_8_5_V1;
DECLARE_SIB11_SEG1OF2_8_5;
DECLARE_SIB11_SEG2OF2_8_5;
DECLARE_SIB10_8_5;
DECLARE_SIB1_234567_MCC_8_5_V1;

static unsigned char sib1_invalid[] = {0x40, 0x40, 0x04, 0x03, 0x00, 0x02, 0x00, 0x00, 0x82, 0xa8, 0x64,
                                       0x01, 0xa4, 0x10, 0x20, 0x85, 0x04, 0x54, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


extern "C"
{
extern lte_rrc_sib_s lte_rrc_sib;

extern void lte_rrc_llc_validate_and_update_mcch_schdl_cfg
(
  lte_rrc_cfg_db_type* cfg_db_ptr,
  lte_l1_mbsfn_allocation_period_e max_alloc_period
);

}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern boolean lte_rrc_sib_test_camped;
extern stm_state_t lte_rrc_sib_test_controller_state;
extern boolean lte_rrc_sib_test_wcdma_supported;
extern boolean lte_rrc_sib_test_gsm_supported;
extern boolean lte_rrc_sib_test_cdma_1x_supported;
extern boolean lte_rrc_sib_test_cdma_do_supported;
extern boolean lte_rrc_sib_test_ho_in_progress;
extern lte_rrc_revoke_priority_e lte_rrc_sib_test_revoke_priority;
extern int lte_rrc_sib_test_sib_db_key;
extern lte_phy_cell_id_t lte_rrc_sib_test_serv_cell_phy_cell_id;
extern lte_earfcn_t lte_rrc_sib_test_serv_cell_freq;

extern void lte_rrc_sib_test_resel1(boolean resel_from_basic_cell1);
extern void lte_rrc_sib_test_3hr_tmr(boolean sib1_changes, boolean value_tag_changes, 
                                     boolean controller_suspended, boolean higher_priority);
extern boolean lte_rrc_sib_test_embms_is_enabled;

static uint64 time_since_power_up_ms = 0;

extern boolean lte_rrc_sib_test_wcdma_supported;
extern boolean lte_rrc_sib_test_gsm_supported;
extern boolean lte_rrc_sib_test_auto_gap_is_active;

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/* First SI empty - valid case - in the sib_sched_req, the first SI should contain 
   SIB2 */
void lte_rrc_sib_test_sched_req()
{
  lte_cphy_sib_sched_req_s *sib_sched_req;
  
  uint32 buf_len;

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

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

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

  // send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_8_5,
                                        sizeof(sib2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  //---------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2_3_4_mcc_8_5,
                                        sizeof(sib1_2_3_4_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

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

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK));
  
  //---------------------------------------------------------------

  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  
  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  //---------------------------------------------------------------

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;

  lte_rrc_camped_indi_s camped_indi;

  // Prepare camped_indi to send to SIB
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_indi.camped_cell.freq = FREQ_1;
  camped_indi.camped_cell.phy_cell_id = PHY_CELL_ID_1;

  // Send camped_indi
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  lte_rrc_utf_wait_for_done();

  //---------------------------------------------------------------

  // send SIB3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib3_8_5,
                                        sizeof(sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
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

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK));

  //---------------------------------------------------------------

  // send SIB4 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_8_5,
                                        sizeof(sib4_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        5);

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
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK));

  //---------------------------------------------------------------

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  //---------------------------------------------------------------

  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL, TRUE, 
                                LTE_RRC_NULL_SFN);

  //---------------------------------------------------------------

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

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK));

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
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK |
                                                     LTE_RRC_SIB2_BITMASK |
                                                     LTE_RRC_SIB3_BITMASK |
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;

  // Send camped_indi
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  lte_rrc_utf_wait_for_done();

  //---------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2_3_4_mcc_8_5,
                                        sizeof(sib1_2_3_4_mcc_8_5),
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

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 128);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK));

  //---------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2_3_4_mcc_8_5,
                                        sizeof(sib1_2_3_4_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        128);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == LTE_RRC_MIB_BITMASK);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                     LTE_RRC_SIB1_BITMASK | 
                                                     LTE_RRC_SIB2_BITMASK | 
                                                     LTE_RRC_SIB3_BITMASK | 
                                                     LTE_RRC_SIB4_BITMASK));

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 192);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK));

  //---------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            129);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 192);

  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK));

}

void lte_rrc_sib_test_camp_phase4_mod_prd_change()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  uint32 buf_len;

  // Send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_mod_prd_change_8_5,
                                        sizeof(sib2_sib3_mod_prd_change_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  // We expect a get_sibs_cnf from SIB
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
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
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
}

void lte_rrc_sib_test_camp_phase5_mod_prd_change()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  
  uint32 buf_len;

  // Send SIB4_5 to RRC
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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
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
}

void lte_rrc_sib_test_camp_phase6_mod_prd_change()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  
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
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

}

/* Test to check SIB behavior when SIB reading for serving cell is aborted at a 
   time when mandatory SIBs are current but some non mandatory SIBs are not. 
   sibs_consistent flag should be true but non mandatory SIBs should be removed 
   from the db */
void lte_rrc_sib_test_non_mand_sibs_inconsistent()
{
  lte_rrc_get_sibs_reqi_s get_sibs_req;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  /* get sibs req - paging */
  lte_rrc_init_default_hdr(&get_sibs_req.msg_hdr, LTE_RRC_GET_SIBS_REQI);
  get_sibs_req.cause = LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE;
  get_sibs_req.cell_id.freq = FREQ_1;
  get_sibs_req.cell_id.phy_cell_id = PHY_CELL_ID_1;
  get_sibs_req.cnf_needed = FALSE;
  get_sibs_req.req_proc = LTE_RRC_PROC_PG;
  get_sibs_req.sfn.sys_fn = 67;

  lte_rrc_utf_send_msg((byte*)&get_sibs_req, sizeof(get_sibs_req));

  lte_rrc_utf_wait_for_done();

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
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));

  // ----------------------------------------------------------------------

  /* SIB1 */
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_value_tag_change,
                                        sizeof(sib1_2345_mcc_8_5_value_tag_change),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        129);

  // ----------------------------------------------------------------------
  
  //sib_sched_req
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));

  // ----------------------------------------------------------------------

  //mib_ind
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            130);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        131);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK |
                                         LTE_RRC_SIB2_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));

  // ----------------------------------------------------------------------

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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_db_index == 0);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == 130);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[1] == 129);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[2] == 131);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[3] == 131);
  for(int i = 4; i < LTE_RRC_SIB_NUM_OF_MIB_N_SIBS; i++)
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
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB3_BITMASK |
                                                                         LTE_RRC_SIB4_BITMASK |
                                                                         LTE_RRC_SIB5_BITMASK));

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 128);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 191);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 192);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK |
                                                                               LTE_RRC_SIB2_BITMASK |
                                                                               LTE_RRC_SIB3_BITMASK |
                                                                               LTE_RRC_SIB4_BITMASK |
                                                                               LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 131);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 2);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
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
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5_value_tag_change));
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

  // -----------------------------------------------------------------

  /* abort */
  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s))
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB3_BITMASK));

  // ----------------------------------------------------------------------

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
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 3);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5_value_tag_change));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                               LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK |
                                                                        LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_sib3_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] != NULL);
  for(int i = 1; i < LTE_RRC_SIB_MAX_SIS; i++)
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
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib4_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib5_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
  //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);

}

/* Test to check that SIB should not request SIBs from ML1 that are not supported 
   even though they are being transmitted, and also those that are supported but 
   not transmitted*/
void lte_rrc_sib_test_supported_rats()
{
  lte_rrc_sib_test_camp_phase1();

  //-----------------------------------------------------------------

  /* SIB7 should not be a required SIB since GSM is not supported */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == (LTE_RRC_SIB_ALL_SIBS_BITMASK & 
                                                                     ~LTE_RRC_SIB7_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK | 
                                                                       LTE_RRC_SIB1_BITMASK));

  //-----------------------------------------------------------------

  lte_rrc_sib_test_camp_phase2();

  //-----------------------------------------------------------------

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_cphy_irat_cdma_system_time_chg_req_s *sys_time_chg_req;
  uint32 buf_len;

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_234567_mcc_8_5,
                                        sizeof(sib1_234567_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  //-----------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == (LTE_RRC_SIB_ALL_SIBS_BITMASK & 
                                                                     ~LTE_RRC_SIB7_BITMASK));
  /* SIB7 should not be acquired because GSM is not supported, 
     SIB8 should not be acquired because it is not transmitted */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK | 
                                                                       LTE_RRC_SIB1_BITMASK |
                                                                       LTE_RRC_SIB2_BITMASK |
                                                                       LTE_RRC_SIB3_BITMASK |
                                                                       LTE_RRC_SIB4_BITMASK |
                                                                       LTE_RRC_SIB5_BITMASK |
                                                                       LTE_RRC_SIB6_BITMASK));

  //-----------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB6_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB6_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 4);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB6_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[3].sib_map_bitmask == (LTE_RRC_SIB7_BITMASK));

  //-----------------------------------------------------------------

  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

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

  //-----------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB6_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB6_BITMASK));

  //-----------------------------------------------------------------

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  //-----------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB6_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB6_BITMASK));

  //-----------------------------------------------------------------

  // send SIB6 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib6_8_5,
                                        sizeof(sib6_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  //-----------------------------------------------------------------

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
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB6_BITMASK));

  //-----------------------------------------------------------------

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);
  
  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB6_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB6_BITMASK));

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

  /* TO CHECK THE SAME THING IN CONNECTED MODE, HO TO A CELL THAT TRANSMITS SIB8, 
     BUT IT SHOULD NOT BE REQUESTED IF CDMA IS NOT SUPPORTED */

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_connected_indi();

  //---------------------------------------------------------------

  lte_rrc_sib_test_cdma_1x_supported = FALSE;
  lte_rrc_sib_test_cdma_do_supported = FALSE;

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_ho_started_indi();
  lte_rrc_sib_test_ho_in_progress = FALSE;

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE, TRUE, 
                                      LTE_RRC_NULL_SFN);

  //---------------------------------------------------------------

  // We expect a SIB8 to be included, as we need system time info, whether CDMA is supported or not
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == (LTE_RRC_MIB_BITMASK | 
                                                                             LTE_RRC_SIB1_BITMASK |
                                                                             LTE_RRC_SIB2_BITMASK |
                                                                             LTE_RRC_SIB3_BITMASK |
                                                                             LTE_RRC_SIB4_BITMASK |
                                                                             LTE_RRC_SIB5_BITMASK |
                                                                             LTE_RRC_SIB6_BITMASK |
                                                                             LTE_RRC_SIB8_BITMASK |
                                                                             LTE_RRC_SIB9_BITMASK |
                                                                             LTE_RRC_SIB_ETWS_SIBS_BITMASK |
                                                                             LTE_RRC_SIB12_BITMASK |
                                                                             LTE_RRC_SIB_EMBMS_SIBS_BITMASK|
                                                                             LTE_RRC_SIB16_BITMASK));
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK | 
                                                                       LTE_RRC_SIB1_BITMASK));

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 
            (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK));

  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);

  //---------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            1);

  //---------------------------------------------------------------

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

  //---------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_8_mcc_8_5_cell2,
                                        sizeof(sib1_2345_8_mcc_8_5_cell2),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        2);

  //---------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == (LTE_RRC_MIB_BITMASK | 
                                                                             LTE_RRC_SIB1_BITMASK |
                                                                             LTE_RRC_SIB2_BITMASK |
                                                                             LTE_RRC_SIB3_BITMASK |
                                                                             LTE_RRC_SIB4_BITMASK |
                                                                             LTE_RRC_SIB5_BITMASK |
                                                                             LTE_RRC_SIB6_BITMASK |
                                                                             LTE_RRC_SIB8_BITMASK |
                                                                             LTE_RRC_SIB9_BITMASK |
                                                                             LTE_RRC_SIB_ETWS_SIBS_BITMASK |
                                                                             LTE_RRC_SIB12_BITMASK |
                                                                             LTE_RRC_SIB_EMBMS_SIBS_BITMASK|
                                                                             LTE_RRC_SIB16_BITMASK));
  /*  */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK |
                                                                               LTE_RRC_SIB2_BITMASK |
                                                                               LTE_RRC_SIB3_BITMASK |
                                                                               LTE_RRC_SIB4_BITMASK |
                                                                               LTE_RRC_SIB5_BITMASK |
                                                                               LTE_RRC_SIB8_BITMASK ));

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  // SIB8 should not be requested even though it is being transmitted
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK| 
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK | 
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB8_BITMASK));

  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);

  //---------------------------------------------------------------

  /* SAME TEST ON GOING FROM CONNECTED TO IDLE WHEN EVENT IS ALREADY ACTIVE 
     (SINCE SIBS REQUIRED IS COMPUTED AGAIN). CDMA IS NOT SUPPORTED SO SIB8 SHOULD 
     NOT BE REQUESTED */
  
  //---------------------------------------------------------------

  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  lte_rrc_sib_test_camped = FALSE;

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  //---------------------------------------------------------------

  /* */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == (LTE_RRC_SIB_ALL_SIBS_BITMASK & 
                                                                     ~(LTE_RRC_SIB7_BITMASK)));
  /* SIB6 should not be acquired since it is not transmitted */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK | 
                                                                       LTE_RRC_SIB1_BITMASK |
                                                                       LTE_RRC_SIB2_BITMASK |
                                                                       LTE_RRC_SIB3_BITMASK |
                                                                       LTE_RRC_SIB4_BITMASK |
                                                                       LTE_RRC_SIB5_BITMASK |
                                                                       LTE_RRC_SIB8_BITMASK ));

  //---------------------------------------------------------------
/*
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);

  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK | 
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB8_BITMASK));
*/
  //---------------------------------------------------------------

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        3);

  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_get_sibs_cnfi(PHY_CELL_ID_2,
                                     FREQ_2,
                                     1,
                                     LTE_RRC_GET_SIBS_SUCCESS);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB8_BITMASK));

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib8_8_5,
										  sizeof(sib8_8_5),
										  PHY_CELL_ID_2,
										  FREQ_2,
										  0);
  
  // We expect a sys_time_chg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sys_time_chg_req, &buf_len);
  TF_ASSERT(sys_time_chg_req->msgr_hdr.id == LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ);

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
                                                         LTE_RRC_SIB5_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));
  
  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 1;
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_2,
                                    FREQ_2);

  //---------------------------------------------------------------

  /* SAME TEST ON GOING FROM IDLE TO CONNECTED WHEN EVENT IS ALREADY ACTIVE 
     (SINCE SIBS REQUIRED IS COMPUTED AGAIN). CDMA IS NOT SUPPORTED SO SIB8 SHOULD 
     NOT BE REQUESTED */

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_connected_indi();

  //---------------------------------------------------------------

  // Since MIB, SIB1, SIB2 are already present, event should be converted to next_mod_prd_value_tag_check
  // Since in connected mode SIB3, 4 are addtional read, event wouldn't be updated to NXT MOD PRD VAL TAF CHK,
  //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].cause == LTE_RRC_GET_SIBS_CAUSE_NEXT_MOD_PRD_VALUE_TAG_CHECK);

  //---------------------------------------------------------------

  /* */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_required_bitmask == (LTE_RRC_MIB_BITMASK | 
                                                                             LTE_RRC_SIB1_BITMASK |
                                                                             LTE_RRC_SIB2_BITMASK |
                                                                             LTE_RRC_SIB3_BITMASK |
                                                                             LTE_RRC_SIB4_BITMASK |
                                                                             LTE_RRC_SIB5_BITMASK |
                                                                             LTE_RRC_SIB6_BITMASK |
                                                                             LTE_RRC_SIB8_BITMASK |
                                                                             LTE_RRC_SIB9_BITMASK |
                                                                             LTE_RRC_SIB_ETWS_SIBS_BITMASK |
                                                                             LTE_RRC_SIB12_BITMASK |
                                                                             LTE_RRC_SIB_EMBMS_SIBS_BITMASK|
                                                                             LTE_RRC_SIB16_BITMASK));
  /* */
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_to_acquire_bitmask == (LTE_RRC_MIB_BITMASK | 
                                                                               LTE_RRC_SIB1_BITMASK |
                                                                               LTE_RRC_SIB2_BITMASK |
                                                                               LTE_RRC_SIB3_BITMASK |
                                                                               LTE_RRC_SIB4_BITMASK |
                                                                               LTE_RRC_SIB5_BITMASK |
                                                                               LTE_RRC_SIB8_BITMASK));

  //---------------------------------------------------------------
/*
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_2);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB4_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB8_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 3);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                              LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[2].sib_map_bitmask == (LTE_RRC_SIB8_BITMASK));
  */
}

/* Test to check feature FEATURE_LTE_RRC_USE_CANNED_SIB1 */
void lte_rrc_sib_test_use_canned_sib1()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  uint32 buf_len;

  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_CPHY_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_20);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));

  // ---------------------------------------------

  // send MIB to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            1);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_20);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));

  // ----------------------------------------------------------------------

  // send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_8_5,
                                        sizeof(sib2_8_5),
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
  //TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_20);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));

  //Asserts
  {
    /* Asserts on SIB internal data */
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == TRUE);
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
    //TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib1_ptr != NULL);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_bitmask == 0);
    for(int i = 0; i < LTE_RRC_SIB_MAX_SIS; i++)
    {
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.sibs_in_si_bitmask[i] == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_size == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.encoded_si_info[i].encoded_si_ptr == 0);
      TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.decoded_si_ptr[i] == NULL);
    }
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].all_si.si_decoded == FALSE);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[0] == 1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_sfn[1] == 3);
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
                                                                         LTE_RRC_SIB3_BITMASK));
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
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sib_update_needed_bitmask == (LTE_RRC_SIB3_BITMASK));
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].mod_period_window_length == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_start == 0);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].curr_window_end == 63);
  
    // ----------------------------------------------------------------------
  
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].next_mod_prd_sfn == LTE_RRC_NULL_SFN);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.freq == FREQ_1);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.curr_mod_prd_sib_mask == (LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.mod_period_boundary_sfn == 64);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                                                 LTE_RRC_SIB2_BITMASK |
                                                                                 LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.last_sfn_in_sib_rsp == 3);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_20);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.num_si_msgs == 2);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sched_req.si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_SIB3_SIB4);
  
    // ----------------------------------------------------------------------
  
    /* Asserts on valid cells */
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_rank == 0);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].index_counter == 0);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[0] == 1);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[1] == 2);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[2] == 3);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.num_mnc_digits == 3);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[0] == 4);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[1] == 4);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[2] == 4);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.global_cell_id == 0x400000);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_1);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 2);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
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
  }

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  //---------------------------------------------------------------

  // send SIB3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib3_8_5,
                                        sizeof(sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_SIB3_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK));

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  //TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_20);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB3_BITMASK));

  //Asserts
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
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[0] == 1);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[1] == 2);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mcc[2] == 3);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.num_mnc_digits == 3);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[0] == 4);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[1] == 4);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.plmn.mnc[2] == 4);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.global_cell_id == 0x400000);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.phy_cell_id == PHY_CELL_ID_1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_1);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].value_tag == 2);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.num_tx_antennas == 1);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.dl_bandwidth == LTE_BW_NRB_100);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_duration == LTE_L1_PHICH_DURATION_NORMAL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].mib.phich_info.phich_resource == LTE_L1_PHICH_RESOURCE_ONE);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_size == sizeof(sib1_2345_mcc_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                 LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] != NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[1] == (LTE_RRC_SIB3_BITMASK));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_size == sizeof(sib3_8_5));
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_ptr == NULL);
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
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib4_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib5_ptr == NULL);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib6_ptr == NULL);
    //TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib7_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib8_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib9_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib10_ptr == NULL);
    TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib11_ptr == NULL);
  }
}

/* Test to check feature FEATURE_LTE_RRC_USE_CANNED_SIBS */
void lte_rrc_sib_test_use_canned_sibs()
{
	lte_cphy_sib_sched_req_s *sib_sched_req_ptr;

  uint32 buf_len;

	lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                      LTE_RRC_NULL_SFN);

	lte_rrc_sib_test_rcv_get_sibs_cnfi(PHY_CELL_ID_1,
																		 FREQ_1,
																		 0,
																		 LTE_RRC_GET_SIBS_SUCCESS);

	// We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));

	TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].encoded_sib1.encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib1_ptr != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == (LTE_RRC_SIB2_BITMASK |
																																			 LTE_RRC_SIB13_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[0] == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_size == sizeof(sib2_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[0].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[0] != NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_in_si_bitmask[1] == (LTE_RRC_SIB13_BITMASK));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_size == sizeof(sib13_8_5));
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.encoded_si_info[1].encoded_si_ptr == NULL);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.decoded_si_ptr[1] != NULL);
}

/* Test to check revoke functionality */
void lte_rrc_sib_test_get_sibs_reqi_suspended(lte_rrc_get_sibs_cause_e get_sibs_cause,
                                              boolean higher_priority)
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

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      get_sibs_cause, FALSE, 
                                      0);

  if(higher_priority)
  {
    lte_rrc_sib_test_rcv_revoke_reqi();
  }

  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;

  // Send a message that SIB does not process
  lte_rrc_sib_test_send_service_ind();

  lte_rrc_sib_test_rcv_get_sibs_reqi();

  if(get_sibs_cause == LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE)
  {
    lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_1,
                                       FREQ_1,
                                       0,
                                       64,
                                       (LTE_RRC_MIB_BITMASK |
                                        LTE_RRC_SIB1_BITMASK |
                                        LTE_RRC_SIB2_BITMASK |
                                        LTE_RRC_SIB3_BITMASK |
                                        LTE_RRC_SIB4_BITMASK |
                                        LTE_RRC_SIB5_BITMASK),
                                       0,
                                       LTE_CPHY_SI_WINDOW_LENGTH_MS_2,
                                       2);
  }
  else
  {
    lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_1,
                                       FREQ_1,
                                       LTE_RRC_SIB1_BITMASK,
                                       64,
                                       (LTE_RRC_MIB_BITMASK |
                                        LTE_RRC_SIB1_BITMASK |
                                        LTE_RRC_SIB2_BITMASK |
                                        LTE_RRC_SIB3_BITMASK |
                                        LTE_RRC_SIB4_BITMASK |
                                        LTE_RRC_SIB5_BITMASK),
                                       0,
                                       LTE_CPHY_SI_WINDOW_LENGTH_MS_2,
                                       2);
  }
}

/* Test to check using mandantory sibs timer value from EFS file */
void lte_rrc_sib_test_use_mand_sibs_tmr_val_from_efs()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  uint32 buf_len;

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);

  // ---------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_RESEL_FAIL, TRUE, 
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
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_CPHY_NULL_SFN);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 1);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].running_tmr == LTE_RRC_SIB_EVENT_TMR_MIB_SIB1);
  TF_ASSERT(lte_rrc_tmr_get_remaining(&lte_rrc_sib.dd_ptr->events_db.event[0].wt_tmr) == 10240);

  // ---------------------------------------------

  // send MIB to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            64);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == LTE_RRC_SIB1_BITMASK);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 64);

  // ----------------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_value_tag_change,
                                        sizeof(sib1_2345_mcc_8_5_value_tag_change),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
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
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
}

/* Test to check no crash when SI change but sib1 value tag match */
void lte_rrc_sib_si_change_but_sib1_value_tag_match(void)
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;

  uint32 buf_len;

  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  //---------------------------------------------------------------

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
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
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_CPHY_NULL_SFN);

  // ----------------------------------------------------------------------

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

  // send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_8_5,
                                        sizeof(sib2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        64);

  //---------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2_mcc_8_5,
                                        sizeof(sib1_2_mcc_8_5),
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
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK | 
                                                         LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
}

void lte_rrc_test_get_ul_dl_overlapping_bands
(
  uint16 ovrlp_band_combo[][2],
  boolean dl_only
)
{
  uint16 dl_low[45] = {
    0,
    21100 ,
    19300 ,
    18050 ,
    21100 ,
    8690  ,
    8750  ,
    26200 ,
    9250  ,
    18449 ,
    21100 ,
    14759 ,
    7290  ,
    7460  ,
    7580  ,
    0     ,
    0     ,
    7340  ,
    8600  ,
    8750  ,
    7910  ,
    14959 ,
    35100 ,
    21800 ,
    15250 ,
    19300 ,
    8590  ,
    8520  ,
    7580  ,
    7170  ,
    23500 ,
    4625  ,
    0     ,
    19000 ,
    20100 ,
    18500 ,
    19300 ,
    19100 ,
    25700 ,
    18800 ,
    23000 ,
    24960 ,
    34000 ,
    36000 ,
    7030  
    };
  uint16 ul_low[45] = {
    0,
    19200 ,
    18500 ,
    17100 ,
    17100 ,
    8240  ,
    8300  ,
    25000 ,
    8800  ,
    17499 ,
    17100 ,
    14279 ,
    6990  ,
    7770  ,
    7880  ,
    0     ,
    0     ,
    7040  ,
    8150  ,
    8300  ,
    8320  ,
    14479 ,
    34100 ,
    20000 ,
    16265 ,
    18500 ,
    8140  ,
    8070  ,
    7030  ,
    0     ,
    23050 ,
    4525  ,
    0     ,
    19000 ,
    20100 ,
    18500 ,
    19300 ,
    19100 ,
    25700 ,
    18800 ,
    23000 ,
    24960 ,
    34000 ,
    36000 ,
    7030  
    };
  uint8 bw[45] = {
    0,
    60 ,
    60 ,
    75 ,
    45 ,
    25 ,
    10 ,
    70 ,
    35 ,
    35 ,
    60 ,
    20 ,
    17 ,
    10 ,
    10 ,
    0  ,
    0  ,
    12 ,
    15 ,
    15 ,
    30 ,
    15 ,
    80 ,
    20 ,
    34 ,
    65 ,
    35 ,
    17 ,
    45 ,
    11 ,
    10 ,
    5  ,
    0  ,
    20 ,
    15 ,
    60 ,
    60 ,
    20 ,
    50 ,
    40 ,
    100,
    194,
    200,
    200,
    100
    };

  uint16 invalid_bands[] = {15, 16, 29, 32};
  uint16 max_band = 44;
  uint16 ovrlp_idx = 0;
  boolean invalid_band_found = FALSE;
  boolean dl_ovrlp, ul_ovrlp;

  if (dl_only)
  {
    TF_MSG("*** DL only overlapping bands ***");
  }
  else
  {
    TF_MSG("*** UL/DL only overlapping bands ***");
  }

  for (int i = 1; i <= max_band; i++)
  {
    //exclude reserved/NA bands
    invalid_band_found = FALSE;
    for (int inv = 0; inv < sizeof(invalid_bands)/sizeof(invalid_bands[0]); inv++)
    {
      if (i == invalid_bands[inv])
      {
        invalid_band_found = TRUE;
        break;
      }
    }
    if (invalid_band_found == TRUE)
    {
      continue;
    }
    
    for (int j = 1; j <= max_band; j++)
    {
      //exclude TDD bands from FDD bands
      if (i < 33 && j >= 33)
      {
        break;
      }
      else if (i >= 33 && j < 33)
      {
        continue;
      }
      
      //exclude same band
      if (i == j)
      {
        continue;
      }

      //exclude reserved/NA bands
      invalid_band_found = FALSE;
      for (int inv = 0; inv < sizeof(invalid_bands)/sizeof(invalid_bands[0]); inv++)
      {
        if (j == invalid_bands[inv])
        {
          invalid_band_found = TRUE;
          break;
        }
      }
      if (invalid_band_found == TRUE)
      {
        continue;
      }
    
      //Check for both UL and DL overlap or DL only based on input mode
      dl_ovrlp = ((dl_low[i] >= dl_low[j] && dl_low[i] <= (dl_low[j] + (bw[j]*10-1))) ||
                  ((dl_low[i] + (bw[i]*10-1)) >= dl_low[j] && (dl_low[i] + (bw[i]*10-1)) <= (dl_low[j] + (bw[j]*10-1)))||
                  (dl_low[i] <= dl_low[j] && (dl_low[i] + (bw[i]*10-1)) >= (dl_low[j] + (bw[j]*10-1))));
      ul_ovrlp = ((ul_low[i] >= ul_low[j] && ul_low[i] <= (ul_low[j] + (bw[j]*10-1))) ||
                  ((ul_low[i] + (bw[i]*10-1)) >= ul_low[j] && (ul_low[i] + (bw[i]*10-1)) <= (ul_low[j] + (bw[j]*10-1)))||
                  (ul_low[i] <= ul_low[j] && (ul_low[i] + (bw[i]*10-1)) >= (ul_low[j] + (bw[j]*10-1))));

      if ((dl_ovrlp && ul_ovrlp) || (dl_only && dl_ovrlp))
      {
        ovrlp_band_combo[ovrlp_idx][0] = i;
        ovrlp_band_combo[ovrlp_idx][1] = j;
        TF_MSG("ovrlp_band_combo: %d - %d", i,j);
        ovrlp_idx++;
      }
    }
  }
}

/*===========================================================================

                                UNIT TEST
First SI empty - valid case - in the sib_sched_req, the first SI should contain 
SIB2
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSchedReqTest);

void LteRrcSibSchedReqTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSchedReqTest::Test()
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

  /* Test the schedule requests */
  lte_rrc_sib_test_sched_req();
  

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibSchedReqTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test case to check the case where event is started with default 64 mod prd 
length, but SIB2 contains a diff mod prd length, subsequent requests should be 
updated with the corresponding values

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibModPrdChangeInSIB2Test);

void LteRrcSibModPrdChangeInSIB2Test::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibModPrdChangeInSIB2Test::Test()
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
  lte_rrc_sib_test_camp_phase4_mod_prd_change();
  lte_rrc_sib_test_camp_phase5_mod_prd_change();
  lte_rrc_sib_test_camp_phase6_mod_prd_change();

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibModPrdChangeInSIB2Test::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test case to check that SIB1 should not be removed from the event/db when mod 
prd is changed

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibModPrdChangeInSIB2CellInDbTest);

void LteRrcSibModPrdChangeInSIB2CellInDbTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibModPrdChangeInSIB2CellInDbTest::Test()
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

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  
  uint32 buf_len;

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5_mod_prd_512,
                                        sizeof(sib2_sib3_8_5_mod_prd_512),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        128);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 512);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 128);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  // earlier an assert was being seen on rcving sib1 here because sib1 was 
  // already removed from the event
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibModPrdChangeInSIB2CellInDbTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to check cell id related changes (if reselecting to serving cell, send 
sib_read_error; if cell is found in db and phy_cell_id and/or freq changes, 
update it)

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibCellIdTest);

void LteRrcSibCellIdTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibCellIdTest::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;
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

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send same SIB1 as serving cell
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        2);

  // sib read error should be sent
  lte_rrc_sib_test_rcv_sib_read_error_indi(PHY_CELL_ID_2,
                                           FREQ_2,
                                           LTE_RRC_SIB_READ_ERROR_CAUSE_INVALID_SIBS,
                                           LTE_RRC_SIB1_BITMASK,
                                           TRUE,
                                           TRUE);

  // We expect a sib_sched_req message from RRC - to stop SIB reading
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);

  // test done - stop
  lte_rrc_sib_test_send_stopped_indi();

  // another test - if cell is found in db, and pci/freq change they should be updated
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_2, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_SEL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_1);

  // send SIB1 thats present in db, but pci/freq has changed
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        2);

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].sib_cell_id.cell_id.freq == FREQ_2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  lte_rrc_utf_check_no_more_msgs();

  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibCellIdTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check SIB behavior when SIB reading for serving cell is aborted at a 
time when mandatory SIBs are current but some non mandatory SIBs are not. 
sibs_consistent flag should be true but non mandatory SIBs should be removed 
from the db

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibNonMandSibsInconsistentTest);

void LteRrcSibNonMandSibsInconsistentTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibNonMandSibsInconsistentTest::Test()
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

  /* Read SIBs such that non mandatory SIBs become inconsistent */
  lte_rrc_sib_test_non_mand_sibs_inconsistent();
  
  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibNonMandSibsInconsistentTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check that SIB should not request SIBs from ML1 that are not supported 
even though they are being transmitted, and also those that are supported but 
not transmitted

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSupportedRatsTest);

void LteRrcSibSupportedRatsTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
	  LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibSupportedRatsTest::Test()
{
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_wcdma_supported = TRUE;
  lte_rrc_sib_test_gsm_supported = FALSE;
  lte_rrc_sib_test_cdma_1x_supported = TRUE;
  lte_rrc_sib_test_cdma_do_supported = TRUE;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_supported_rats();

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibSupportedRatsTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
SIB read error indi should be populated from SIB1 if SIB1 is received and 
invalid SIB2 is received

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibInvalidSib2WhenSib1PresentTest);

void LteRrcSibInvalidSib2WhenSib1PresentTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibInvalidSib2WhenSib1PresentTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();

  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;

  uint32 buf_len;

  // Prepare bcch_dl_data_ind to send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_8_5_mod_prd_2048,
                                        sizeof(sib2_8_5_mod_prd_2048),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // ------------------------------------------------------------


  // No sib_read_error_indi should be sent, mod_prd_boundary_sfn is 0
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 0);
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

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibInvalidSib2WhenSib1PresentTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
An SI is received when already present in db (before cell is found in db). When 
cell is found in db (on rcving SIB1), the SI in db should be replaced (check by 
changing the SI)

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibRcvSIWhenAlreadyPresentTest);

void LteRrcSibRcvSIWhenAlreadyPresentTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibRcvSIWhenAlreadyPresentTest::Test()
{
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

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr->m.ac_BarringInfoPresent == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr->m.mbsfn_SubframeConfigListPresent == 0);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_phase1();

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_diff_8_5,
                                        sizeof(sib2_sib3_diff_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr->m.ac_BarringInfoPresent == 1);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr->m.mbsfn_SubframeConfigListPresent == 0);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibRcvSIWhenAlreadyPresentTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check that unsolicited_sib_ind should be sent whenever a BCCH_DL_SCH 
message is received for a cell for which event is not active

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibBplmnTest);

void LteRrcSibBplmnTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibBplmnTest::Test()
{
  lte_rrc_unsolicited_sib_indi_s *unsolicited_sib_ind_ptr;
  uint32 buf_len;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            1);

  // No indication should be sent by SIB on rcving MIB for a cell for which event is not active
  lte_rrc_utf_check_no_more_msgs();

  // ---------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib345_8_5_long,
                                        sizeof(sib345_8_5_long),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // Unsolicited Sib indication should be sent by SIB on rcving long SI for a cell for which event is not active
  
  lte_rrc_utf_get_next_int_msg((byte**)&unsolicited_sib_ind_ptr, &buf_len);
  TF_ASSERT(unsolicited_sib_ind_ptr->msg_hdr.id == LTE_RRC_UNSOLICITED_SIB_INDI);
  TF_ASSERT(unsolicited_sib_ind_ptr->cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(unsolicited_sib_ind_ptr->cell_id.freq == FREQ_1);
  TF_ASSERT(unsolicited_sib_ind_ptr->encoded_si_size == sizeof(sib345_8_5_long));

  lte_rrc_utf_check_no_more_msgs();

  // ---------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_8_mcc_8_5,
                                        sizeof(sib1_2345_8_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // ---------------------------------------------

  // We expect a unsolicited_sib_ind from SIB
  lte_rrc_utf_get_next_int_msg((byte**)&unsolicited_sib_ind_ptr, &buf_len);
  TF_ASSERT(unsolicited_sib_ind_ptr->msg_hdr.id == LTE_RRC_UNSOLICITED_SIB_INDI);
  TF_ASSERT(unsolicited_sib_ind_ptr->cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(unsolicited_sib_ind_ptr->cell_id.freq == FREQ_1);
  TF_ASSERT(unsolicited_sib_ind_ptr->encoded_si_size == sizeof(sib1_2345_8_mcc_8_5));

  lte_rrc_utf_check_no_more_msgs();

  // ---------------------------------------------

  // Send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_mod_prd_change_8_5,
                                        sizeof(sib2_sib3_mod_prd_change_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  //---------------------------------------------------------------

  // SIB does not decode the SI when event is not active for the cell. So even 
  // if non SIB1 msg is rcvd, it will send unsolicited_sib_ind
  lte_rrc_utf_get_next_int_msg((byte**)&unsolicited_sib_ind_ptr, &buf_len);
  TF_ASSERT(unsolicited_sib_ind_ptr->msg_hdr.id == LTE_RRC_UNSOLICITED_SIB_INDI);
  TF_ASSERT(unsolicited_sib_ind_ptr->cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(unsolicited_sib_ind_ptr->cell_id.freq == FREQ_1);
  TF_ASSERT(unsolicited_sib_ind_ptr->encoded_si_size == sizeof(sib2_sib3_mod_prd_change_8_5));

  lte_rrc_utf_check_no_more_msgs();

  // ---------------------------------------------

  /* Same tests in active state */
  lte_rrc_sib_test_camp_phase1();
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  // ----------------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_2,
                            FREQ_2,
                            1);

  // No indication should be sent by SIB on rcving MIB for a cell for which event is not active
  lte_rrc_utf_check_no_more_msgs();

  // ---------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib345_8_5_long,
                                        sizeof(sib345_8_5_long),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        2);

  // Unsolicited Sib indication should be sent by SIB on rcving long SI for a cell for which event is not active
  
  lte_rrc_utf_get_next_int_msg((byte**)&unsolicited_sib_ind_ptr, &buf_len);
  TF_ASSERT(unsolicited_sib_ind_ptr->msg_hdr.id == LTE_RRC_UNSOLICITED_SIB_INDI);
  TF_ASSERT(unsolicited_sib_ind_ptr->cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(unsolicited_sib_ind_ptr->cell_id.freq == FREQ_2);
  TF_ASSERT(unsolicited_sib_ind_ptr->encoded_si_size == sizeof(sib345_8_5_long));

  lte_rrc_utf_check_no_more_msgs();

  // ---------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_8_mcc_8_5,
                                        sizeof(sib1_2345_8_mcc_8_5),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        2);

  // ---------------------------------------------

  // We expect a unsolicited_sib_ind from SIB
  lte_rrc_utf_get_next_int_msg((byte**)&unsolicited_sib_ind_ptr, &buf_len);
  TF_ASSERT(unsolicited_sib_ind_ptr->msg_hdr.id == LTE_RRC_UNSOLICITED_SIB_INDI);
  TF_ASSERT(unsolicited_sib_ind_ptr->cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(unsolicited_sib_ind_ptr->cell_id.freq == FREQ_2);
  TF_ASSERT(unsolicited_sib_ind_ptr->encoded_si_size == sizeof(sib1_2345_8_mcc_8_5));

  lte_rrc_utf_check_no_more_msgs();

  // ---------------------------------------------

  // Send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_mod_prd_change_8_5,
                                        sizeof(sib2_sib3_mod_prd_change_8_5),
                                        PHY_CELL_ID_2,
                                        FREQ_2,
                                        3);

  //---------------------------------------------------------------

  // SIB does not decode the SI when event is not active for the cell. So even 
  // if non SIB1 msg is rcvd, it will send unsolicited_sib_ind
  lte_rrc_utf_get_next_int_msg((byte**)&unsolicited_sib_ind_ptr, &buf_len);
  TF_ASSERT(unsolicited_sib_ind_ptr->msg_hdr.id == LTE_RRC_UNSOLICITED_SIB_INDI);
  TF_ASSERT(unsolicited_sib_ind_ptr->cell_id.phy_cell_id == PHY_CELL_ID_2);
  TF_ASSERT(unsolicited_sib_ind_ptr->cell_id.freq == FREQ_2);
  TF_ASSERT(unsolicited_sib_ind_ptr->encoded_si_size == sizeof(sib2_sib3_mod_prd_change_8_5));

  lte_rrc_utf_check_no_more_msgs();

  // ---------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibBplmnTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to check feature FEATURE_LTE_RRC_USE_CANNED_SIB1

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibUseCannedSib1Test);

void LteRrcSibUseCannedSib1Test::Setup()
{
  int oflag, mode, efs_ret;

  /* First byte corresponds to size of SIB 1*/
  unsigned char sib1[] = 
    { 0x40, 0x48, 0x20, 0xc9, 0x00, 0x01, 0x02, 0xf0, 0x10, 
      0x09, 0x3c, 0x98, 0x10, 0x00, 0x10, 0x56, 0x80, 0x00, 
      0x00, 0x00, 0x00, 0x00
    };
    
  unsigned char sib1_1[100] = {0};
  
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibUseCannedSib1Test");
    
  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/sib1", 
                    sib1, sizeof(sib1), oflag, mode) >= 0);

  /* Read the first byte to get the size of sib 1*/
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/sib/sib1", O_RDONLY)) != -1)
  TF_ASSERT(efs_read(efs_ret, &sib1_1, sizeof(sib1_1))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);

  TF_ASSERT(0 == memcmp(sib1, sib1_1, sizeof(sib1)));

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibUseCannedSib1Test::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_use_canned_sib1();

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibUseCannedSib1Test::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check feature FEATURE_LTE_RRC_USE_CANNED_SIBS

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibAllSibsCannedTest);

void LteRrcSibAllSibsCannedTest::Setup()
{
  int oflag, mode, efs_ret;

  /* First byte corresponds to size of SIB 1*/
  unsigned char sib1[] = 
    {0x40, 0x44, 0x8E, 0x8A, 0xC0, 0x00, 0x00, 0x40, 0x00, 
		 0x00, 0xA2, 0x83, 0x08, 0x01, 0x0A, 0x89, 0x40
    };

	unsigned char sib2[] = 
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 
		 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE1, 
		 0xB0, 0x00, 0x80, 0x00, 0x00, 0x00
		};

  unsigned char sib13[] = 
		{0x00, 0x40, 0x84, 0x84, 0xC9, 0x32, 0x7F, 0x4C, 0xAF, 
			0xBB, 0xCE, 0xB0, 0x00
	  };
    
  unsigned char sib1_buf[100] = {0};
	unsigned char sib2_buf[100] = {0};
	unsigned char sib13_buf[100] = {0};

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibUseAllCannedSibsTest");
    
  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/sib1", 
                    sib1, sizeof(sib1), oflag, mode) >= 0);
	TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/sib2", 
                    sib2, sizeof(sib2), oflag, mode) >= 0);
	TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/sib13", 
                    sib13, sizeof(sib13), oflag, mode) >= 0);

  /* Read the first byte to get the size of sib 1*/
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/sib/sib1", O_RDONLY)) != -1)
  TF_ASSERT(efs_read(efs_ret, &sib1_buf, sizeof(sib1))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);
  TF_ASSERT(0 == memcmp(sib1, sib1_buf, sizeof(sib1)));

	/* Read the first byte to get the size of sib 2*/
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/sib/sib2", O_RDONLY)) != -1)
  TF_ASSERT(efs_read(efs_ret, &sib2_buf, sizeof(sib2))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);
  TF_ASSERT(0 == memcmp(sib2, sib2_buf, sizeof(sib2)));

	/* Read the first byte to get the size of sib 13*/
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/sib/sib13", O_RDONLY)) != -1)
  TF_ASSERT(efs_read(efs_ret, &sib13_buf, sizeof(sib13))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);
  TF_ASSERT(0 == memcmp(sib13, sib13_buf, sizeof(sib13)));

	uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibAllSibsCannedTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_use_canned_sibs();

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibAllSibsCannedTest::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check that get_sibs_reqi for si_update should be added to pending queue 
when controller is suspended
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSiUpdateSuspendedTest);

void LteRrcSibSiUpdateSuspendedTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibSiUpdateSuspendedTest::Test()
{
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

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_get_sibs_reqi_suspended(LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE, FALSE);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibSiUpdateSuspendedTest::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check that get_sibs_reqi for si_update should be added to pending queue 
when controller is suspended. Revoke request should be sent if this is higher 
priority req that the one that we are suspended for
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSiUpdateSuspendedRevokeTest);

void LteRrcSibSiUpdateSuspendedRevokeTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibSiUpdateSuspendedRevokeTest::Test()
{
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

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_get_sibs_reqi_suspended(LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE, TRUE);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibSiUpdateSuspendedRevokeTest::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check that get_sibs_reqi for etws should be added to pending queue when 
controller is suspended
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsSuspendedTest);

void LteRrcSibEtwsSuspendedTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibEtwsSuspendedTest::Test()
{
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

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_get_sibs_reqi_suspended(LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsSuspendedTest::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check that get_sibs_reqi for etws should be added to pending queue when 
controller is suspended. Revoke request should be sent if this is higher priority 
req that the one that we are suspended for
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsSuspendedRevokeTest);

void LteRrcSibEtwsSuspendedRevokeTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibEtwsSuspendedRevokeTest::Test()
{
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

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_get_sibs_reqi_suspended(LTE_RRC_GET_SIBS_CAUSE_ETWS, TRUE);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsSuspendedRevokeTest::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check that get_sibs_reqi should be added to pending queue when 
controller is suspended
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSib3hrTmrSuspendedTest);

void LteRrcSib3hrTmrSuspendedTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSib3hrTmrSuspendedTest::Test()
{
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

  // ----------------------------------------------------------------------

  /* Reselection */
  lte_rrc_sib_test_resel1(TRUE);

  // ----------------------------------------------------------------------

  /* 3hr timer */
  lte_rrc_sib_test_3hr_tmr(FALSE, FALSE, TRUE, FALSE);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSib3hrTmrSuspendedTest::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Test to check that get_sibs_reqi should be added to pending queue when 
controller is suspended. Revoke request should be sent if this is higher priority 
req that the one that we are suspended for
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSib3hrTmrSuspendedRevokeTest);

void LteRrcSib3hrTmrSuspendedRevokeTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSib3hrTmrSuspendedRevokeTest::Test()
{
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

  // ----------------------------------------------------------------------

  /* Reselection */
  lte_rrc_sib_test_resel1(TRUE);

  // ----------------------------------------------------------------------

  /* 3hr timer */
  lte_rrc_sib_test_3hr_tmr(FALSE, FALSE, TRUE, TRUE);

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSib3hrTmrSuspendedRevokeTest::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
Analysis of crash: Whenever timer for SIBs other than MIB & SIB1 expires, there 
is an assert that SIB1 should already be present. This is not always the case. 
Particularly here the assert fails because on receiving SIB2; because of the 
change in modification period info, we get rid of old SIBs (including SIB1).
Resolution: The assert had to be replaced with an if condition.
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibWtTmrCrashTest);

void LteRrcSibWtTmrCrashTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibWtTmrCrashTest::Test()
{
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_tmri_s *tmr_ptr;
  uint32 buf_len;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_phase1();

  // send MIB to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            44);

  lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_1,
                                     FREQ_1,
                                     LTE_RRC_SIB1_BITMASK,
                                     64,
                                     (LTE_RRC_MIB_BITMASK | 
                                      LTE_RRC_SIB1_BITMASK),
                                     44,
                                     LTE_CPHY_SI_WINDOW_LENGTH_MS_1,
                                     0);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        48);

  // We expect a sib_sched_req message from RRC
  lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_1,
                                     FREQ_1,
                                     (LTE_RRC_SIB2_BITMASK |
                                      LTE_RRC_SIB3_BITMASK |
                                      LTE_RRC_SIB4_BITMASK |
                                      LTE_RRC_SIB5_BITMASK),
                                     64,
                                     (LTE_RRC_MIB_BITMASK | 
                                      LTE_RRC_SIB1_BITMASK |
                                      LTE_RRC_SIB2_BITMASK |
                                      LTE_RRC_SIB3_BITMASK |
                                      LTE_RRC_SIB4_BITMASK |
                                      LTE_RRC_SIB5_BITMASK),
                                     48,
                                     LTE_CPHY_SI_WINDOW_LENGTH_MS_2,
                                     2);

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5_mod_prd_128,
                                        sizeof(sib2_sib3_8_5_mod_prd_128),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_1,
                                     FREQ_1,
                                     (LTE_RRC_SIB4_BITMASK |
                                      LTE_RRC_SIB5_BITMASK),
                                     128,
                                     (LTE_RRC_MIB_BITMASK | 
                                      LTE_RRC_SIB1_BITMASK |
                                      LTE_RRC_SIB2_BITMASK |
                                      LTE_RRC_SIB3_BITMASK |
                                      LTE_RRC_SIB4_BITMASK |
                                      LTE_RRC_SIB5_BITMASK),
                                     64,
                                     LTE_CPHY_SI_WINDOW_LENGTH_MS_2,
                                     2);

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 0);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // was crashing at this earlier
  TF_SLEEP(20000);

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);

  lte_rrc_sib_test_rcv_sib_read_error_indi(PHY_CELL_ID_1,
                                           FREQ_1,
                                           LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT,
                                           (LTE_RRC_MIB_BITMASK | 
                                            LTE_RRC_SIB1_BITMASK | 
                                            LTE_RRC_SIB2_BITMASK |
                                            LTE_RRC_SIB3_BITMASK),
                                           TRUE,
                                           TRUE);

  // We expect a sib_sched_req message from RRC
  lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_1,
                                     FREQ_1,
                                     (0),
                                     128,
                                     (0),
                                     64,
                                     LTE_CPHY_SI_WINDOW_LENGTH_MS_2,
                                     2);

  lte_rrc_utf_wait_for_done();

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibWtTmrCrashTest::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
This unit test tests when UE is paged with SIB1 which contains SIB13 when 
event is active. 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibPageSib13WhenCampedWithActEventTest);

void LteRrcSibPageSib13WhenCampedWithActEventTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibPageSib13WhenCampedWithActEventTest::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  uint32 buf_len;

  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;

  lte_rrc_sib_test_embms_is_enabled = TRUE;

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK |
                                                         LTE_RRC_SIB1_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);

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
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_with_sib13_8_5,
                                        sizeof(sib1_with_sib13_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB13_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB13_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB13_BITMASK));

  // send SIB2 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_8_5,
                                        sizeof(sib2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        65);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB13_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB13_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 65);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB13_BITMASK ));

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK |
                                         LTE_RRC_SIB2_BITMASK ),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB13_BITMASK ));

  // send SIB13 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib13_8_5,
                                        sizeof(sib13_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        66);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 66);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB13_BITMASK ));

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_SIB13_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB13_BITMASK ));

  //---------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibPageSib13WhenCampedWithActEventTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
This unit test tests when get_sib_reqi has EMBMS cause and UE is camped and 
no event is active. 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibGetSib13WhenCampedWithNoActEevntTest);

void LteRrcSibGetSib13WhenCampedWithNoActEevntTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibGetSib13WhenCampedWithNoActEevntTest::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  uint32 buf_len;

  lte_rrc_sib_test_embms_is_enabled = TRUE;

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

	/* No Event should be active */
	TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);

  //---------------------------------------------------------------
  
  // Set the cause of next get_sibs_reqi to be EMBMS, event parameters will be changed
  // after this request
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1,
                                      FREQ_1,
                                      LTE_RRC_PROC_EMP,
                                      LTE_RRC_GET_SIBS_CAUSE_EMBMS,
                                      FALSE,
                                      128);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_with_sib13_8_VT2,
                                        sizeof(sib1_with_sib13_8_VT2),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        129);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB13_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB13_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 129);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB13_BITMASK));

  // send SIB13 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib13_8_5,
                                        sizeof(sib13_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        130);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 130);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB13_BITMASK ));

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_SIB13_BITMASK ),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB13_BITMASK ));

  //---------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibGetSib13WhenCampedWithNoActEevntTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
This unit test tests when get_sib_reqi has EMBMS cause and UE is in CONNECTED 
state. 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibGetSib13WhenConnectedTest);

void LteRrcSibGetSib13WhenConnectedTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibGetSib13WhenConnectedTest::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  uint32 buf_len;

  lte_rrc_sib_test_embms_is_enabled = TRUE;

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

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  //---------------------------------------------------------------
  
  // Set the cause of next get_sibs_reqi to be EMBMS, event parameters will be changed
  // after this request
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1,
                                      FREQ_1,
                                      LTE_RRC_PROC_EMP,
                                      LTE_RRC_GET_SIBS_CAUSE_EMBMS,
                                      FALSE,
                                      128);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == LTE_RRC_NULL_SFN);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == LTE_RRC_NULL_SFN);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_with_sib13_8_VT2,
                                        sizeof(sib1_with_sib13_8_VT2),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        129);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB13_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB13_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 129);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB13_BITMASK));

  // send SIB13 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib13_8_5,
                                        sizeof(sib13_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        136);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 192);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (0));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 136);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB13_BITMASK ));

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_SIB13_BITMASK ),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB13_BITMASK ));

  //---------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibGetSib13WhenConnectedTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
This unit test tests when get_sib_reqi has EMBMS cause and HO is in progress. 
The get_sibs_reqi should be ignored. 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibGetSib13WhenHoInProgressTest);

void LteRrcSibGetSib13WhenHoInProgressTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibGetSib13WhenHoInProgressTest::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  uint32 buf_len;

  lte_rrc_sib_test_embms_is_enabled = TRUE;

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

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  lte_rrc_sib_test_send_ho_started_indi();
  lte_rrc_sib_test_ho_in_progress = TRUE;

  //---------------------------------------------------------------
  
  // Set the cause of next get_sibs_reqi to be EMBMS, this request should be 
  // ignored since HO is in progress.
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1,
                                      FREQ_1,
                                      LTE_RRC_PROC_EMP,
                                      LTE_RRC_GET_SIBS_CAUSE_EMBMS,
                                      FALSE,
                                      128);

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibGetSib13WhenHoInProgressTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
This test is created to test the case when SI message has uncomprehended sib 
RRC should ignore and process other sibs inside that SI correctly. 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibUncomprehendedSibInSiTest);

void LteRrcSibUncomprehendedSibInSiTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibUncomprehendedSibInSiTest::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  
  uint32 buf_len;
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camped = FALSE;
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

  // send MIB to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            1);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);

  // ----------------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2_mcc_8_5,
                                        sizeof(sib1_2_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));

  // ----------------------------------------------------------------------

  // send SIB2 to RRC (RRC should skip the unknown sib in this SI)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(si_noncomprehended_8_5,
                                        sizeof(si_noncomprehended_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 128);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK));

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibUncomprehendedSibInSiTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
This test is created to test the case when sib2 is received in new modification 
period before reading MIB and sib1 when event is active. 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSib2RcvdInNewModPrdBeforeReadingMibSib1);

void LteRrcSibSib2RcvdInNewModPrdBeforeReadingMibSib1::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibSib2RcvdInNewModPrdBeforeReadingMibSib1::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  
  uint32 buf_len;
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

  // ----------------------------------------------------------------------

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
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);

  // ----------------------------------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            628);
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 640);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 628);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5_cell2,
                                        sizeof(sib1_2345_mcc_8_5_cell2),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        632);
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 640);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 632);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                                  LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK |
                                                                  LTE_RRC_SIB5_BITMASK));

  // ----------------------------------------------------------------------

  // send SIB2 to RRC in new mod_prd
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_8_5_mod_prd_512,
                                        sizeof(sib2_8_5_mod_prd_512),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        640);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 0);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 640);

  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs_cnf_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_cnfi_s))
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.freq == FREQ_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.cell_id.phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(get_sibs_cnf_ptr->cell_info.sib_db_key == 1);
  TF_ASSERT(get_sibs_cnf_ptr->status == LTE_RRC_GET_SIBS_SUCCESS);

  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibSib2RcvdInNewModPrdBeforeReadingMibSib1::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
This test is created to test the case when SI message has unsupported SIBs, RRC should ignore them
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibNotSupportedSibInSiTest);

void LteRrcSibNotSupportedSibInSiTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibNotSupportedSibInSiTest::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  
  uint32 buf_len;
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camped = FALSE;
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

  // send MIB to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            1);

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 0);

  // ----------------------------------------------------------------------

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2_mcc_8_5,
                                        sizeof(sib1_2_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 1);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));

  // ----------------------------------------------------------------------

  // send SIB2 to RRC (RRC should skip the unknown sib in this SI)
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib_not_supported_8_5,
                                        sizeof(sib_not_supported_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        3);

  /* SIB2 timer expiry */
  TF_SLEEP((80 * LTE_RRC_SIB_SIB2_WT_TMR_FACTOR) - 1);
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

void LteRrcSibNotSupportedSibInSiTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
This test is created to test the case when 3hr timer expiry happens when 
LTE is deactivated and got unpended before CSP reset the pendq 
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSib3HrTmrExpiryWhenLteDeactTest);

void LteRrcSib3HrTmrExpiryWhenLteDeactTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSib3HrTmrExpiryWhenLteDeactTest::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_tmri_s *tmr_ptr;
  
  uint32 buf_len;

  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  /* Controller is in SUSPENDED state before 3hr timer expiry */
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__SUSPENDED;

  // ----------------------------------------------------------------------

  /* 3hr timer expiry */
  TF_SLEEP(LTE_RRC_SIB_CELL_INFO_AGE_3HR * LTE_RRC_SIB_3HR_TMR_TICK_VALUE);

  for(int i = 0; i < LTE_RRC_SIB_CELL_INFO_AGE_3HR; i++)
  {
    lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  }

  lte_rrc_utf_wait_for_done();

  /* Controller is in INACTIVE state after 3hr timer expiry */
  lte_rrc_sib_test_controller_state = LTE_RRC_CONTROLLER_SM__INACTIVE;

  // ----------------------------------------------------------------------
;
  lte_rrc_utf_reset();
}

void LteRrcSib3HrTmrExpiryWhenLteDeactTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to check using EFS file to setup mandantory sibs timer value

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibUseEfsForMandSibsTmrTest);

void LteRrcSibUseEfsForMandSibsTmrTest::Setup()
{
  int oflag, mode, efs_ret;

  /* MIB/SIB1/SIB2 timer value */
  unsigned char mand_sibs_tmr[] = 
    { 0x00, 0x28, 0x00, 0x28};

  unsigned char buf[100] = {0};
  
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };

  /* Init EFS */
  qtf_efs_initialize("LteRrcSibUseEfsForMandSibsTmrTest");
    
  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/mand_sibs_tmr", 
                    mand_sibs_tmr, sizeof(mand_sibs_tmr), oflag, mode) >= 0);

  /* Read the first byte to get the size of mand_sibs_tmr */
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/sib/mand_sibs_tmr", O_RDONLY)) != -1)
  TF_ASSERT(efs_read(efs_ret, &buf, sizeof(buf))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);

  TF_ASSERT(0 == memcmp(mand_sibs_tmr, buf, sizeof(mand_sibs_tmr)));

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibUseEfsForMandSibsTmrTest::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);
#ifdef LTE_RRC_UNUSED_EFS 
  TF_ASSERT(lte_rrc_sib.sd_ptr->use_efs_mand_sib_tmr == TRUE);
#endif

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_use_mand_sibs_tmr_val_from_efs();

  // ---------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibUseEfsForMandSibsTmrTest::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
This unit test tests when get_sib_reqi has EMBMS cause and UE has an active sib event, SIB13 
must be sched for reading, given that previously EMBMS is not active
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibGetSib13WithActiveSibEventTest);

void LteRrcSibGetSib13WithActiveSibEventTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}
void LteRrcSibGetSib13WithActiveSibEventTest::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  uint32 buf_len;

  lte_rrc_sib_test_embms_is_enabled = FALSE;

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  //lte_rrc_sib_test_camp_phase5();

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // ---------------------------------------------
#if 0
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK |
  	                                                     LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 4);

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

	/* No Event should be active */
	TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].event_active == FALSE);
  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[1].event_active == FALSE);
#endif
  //---------------------------------------------------------------
  
  // Set the cause of next get_sibs_reqi to be EMBMS, event parameters will be changed
  // after this request
  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1,
                                      FREQ_1,
                                      LTE_RRC_PROC_EMP,
                                      LTE_RRC_GET_SIBS_CAUSE_EMBMS,
                                      FALSE,
                                      128);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK |
  	                                                     LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB5_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_with_sib13_8_VT2,
                                        sizeof(sib1_with_sib13_8_VT2),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        5);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB13_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB13_BITMASK ));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 5);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB13_BITMASK));

  // send SIB13 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib13_8_5,
                                        sizeof(sib13_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        10);

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
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 10);
  TF_ASSERT(sib_sched_req_ptr->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req_ptr->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req_ptr->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB13_BITMASK ));
                                                                  
  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,            
                                        FREQ_1,                   
                                        (LTE_RRC_SIB13_BITMASK ), 
                                        (LTE_RRC_MIB_BITMASK |    
                                         LTE_RRC_SIB1_BITMASK |   
                                         LTE_RRC_SIB2_BITMASK |   
                                         LTE_RRC_SIB13_BITMASK ));

  //---------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibGetSib13WithActiveSibEventTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================
                                UNIT TEST
This test test the case that SI change but SIB1 value tag still match
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSiChangeButSib1ValueTagMatchTest);

void LteRrcSibSiChangeButSib1ValueTagMatchTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibSiChangeButSib1ValueTagMatchTest::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_tmri_s *tmr_ptr;
  
  uint32 buf_len;

  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------
  lte_rrc_sib_si_change_but_sib1_value_tag_match();

  // ----------------------------------------------------------------------
  lte_rrc_utf_reset();
}

void LteRrcSibSiChangeButSib1ValueTagMatchTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================
                                UNIT TEST
This test test the case that SI change but SIB1 value tag still match
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibSiProcWithInvalidSib8Test);

void LteRrcSibSiProcWithInvalidSib8Test::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibSiProcWithInvalidSib8Test::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf;
  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  lte_rrc_tmri_s *tmr_ptr;

  uint32 buf_len;

  
  uint8 lte_rrc_sib_test_sib1_23_4[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x08, 0x08, 0x08, 0x42, 0x44,};
  uint8 lte_rrc_sib_test_sib48[] = {0x00, 0x88, 0x0c, 0x00,};

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  
  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(lte_rrc_sib_test_sib1_23_4,
                                        sizeof(lte_rrc_sib_test_sib1_23_4),
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
                                                     LTE_RRC_SIB4_BITMASK));
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK));
  TF_ASSERT(sib_sched_req->last_sfn_in_sib_rsp == 2);
  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK));
  
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
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB4_BITMASK));
  TF_ASSERT(sib_sched_req->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK |
                                                         LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK));
  TF_ASSERT(sib_sched_req->last_sfn_in_sib_rsp == 3);
  TF_ASSERT(sib_sched_req->si_window_length == LTE_CPHY_SI_WINDOW_LENGTH_MS_2);
  TF_ASSERT(sib_sched_req->num_si_msgs == 2);
  TF_ASSERT(sib_sched_req->si_msg_info[0].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_8);
  TF_ASSERT(sib_sched_req->si_msg_info[0].sib_map_bitmask == (LTE_RRC_SIB2_BITMASK |
                                                              LTE_RRC_SIB3_BITMASK));
  TF_ASSERT(sib_sched_req->si_msg_info[1].si_periodicity == LTE_CPHY_SI_PERIODICITY_RF_16);
  TF_ASSERT(sib_sched_req->si_msg_info[1].sib_map_bitmask == (LTE_RRC_SIB4_BITMASK));

  //camped_indi
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  lte_rrc_sib_test_send_bcch_dl_sch_msg(lte_rrc_sib_test_sib48,
                                        sizeof(lte_rrc_sib_test_sib48),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        8);

  //---------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_sib_sched_req_s));
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->freq == FREQ_1);
  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);

  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == 0);

  TF_ASSERT(sib_sched_req->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                     LTE_RRC_SIB1_BITMASK));

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_SIB3_BITMASK |
                                         LTE_RRC_SIB4_BITMASK),
                                        (LTE_RRC_MIB_BITMASK |
                                         LTE_RRC_SIB1_BITMASK |
                                         LTE_RRC_SIB2_BITMASK |
                                         LTE_RRC_SIB3_BITMASK |
                                         LTE_RRC_SIB4_BITMASK));
                                                     
  lte_rrc_utf_check_no_more_msgs();

}


void LteRrcSibSiProcWithInvalidSib8Test::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibTestSib2ProcNewModPrd);

void LteRrcSibTestSib2ProcNewModPrd::Setup()
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

void LteRrcSibTestSib2ProcNewModPrd::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  lte_rrc_sib_updated_indi_s *sib_updated_ind_ptr;
  
  uint32 buf_len;
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

  // ----------------------------------------------------------------------

  /* abort */
  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  TF_SLEEP(5000);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
                                      64);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_sib10_8_5,
                                        sizeof(sib2_sib3_sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        630);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2310_45_11_mcc_8_5,
                                        sizeof(sib1_2310_45_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        632);
                                        
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  
  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            630);

  lte_rrc_utf_get_next_int_msg((byte**)&sib_updated_ind_ptr, &buf_len);
  TF_ASSERT(sib_updated_ind_ptr->msg_hdr.id == LTE_RRC_SIB_UPDATED_INDI);


  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_8_5,
                                        sizeof(sib11_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        635);

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // ----------------------------------------------------------------------

  TF_SLEEP(100);

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_deact_req();
  lte_rrc_utf_reset();
}

void LteRrcSibTestSib2ProcNewModPrd::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibTestSib2ProcCurModPrd);

void LteRrcSibTestSib2ProcCurModPrd::Setup()
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

void LteRrcSibTestSib2ProcCurModPrd::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  lte_rrc_sib_updated_indi_s *sib_updated_ind_ptr;
  
  uint32 buf_len;
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

  // ----------------------------------------------------------------------

  /* abort */
  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  TF_SLEEP(5000);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
                                      6);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2310_45_11_mcc_8_5,
                                        sizeof(sib1_2310_45_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        10);
                                        
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_sib10_8_5,
                                        sizeof(sib2_sib3_sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        14);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_8_5,
                                        sizeof(sib11_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        16);

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // ----------------------------------------------------------------------

  TF_SLEEP(100);

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibTestSib2ProcCurModPrd::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibTestSib2ProcCurModPrdValTagChng);

void LteRrcSibTestSib2ProcCurModPrdValTagChng::Setup()
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

void LteRrcSibTestSib2ProcCurModPrdValTagChng::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  lte_rrc_sib_updated_indi_s *sib_updated_ind_ptr;
  
  uint32 buf_len;
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

  // ----------------------------------------------------------------------

  /* abort */
  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  TF_SLEEP(5000);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
                                      6);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_sib10_8_5,
                                        sizeof(sib2_sib3_sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        8);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2310_45_11_mcc_8_5_v1,
                                        sizeof(sib1_2310_45_11_mcc_8_5_v1),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        10);

  lte_rrc_utf_get_next_int_msg((byte**)&sib_updated_ind_ptr, &buf_len);
  TF_ASSERT(sib_updated_ind_ptr->msg_hdr.id == LTE_RRC_SIB_UPDATED_INDI);
                                        
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_8_5,
                                        sizeof(sib11_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        16);

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_SEC_IND);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        20);

  lte_rrc_utf_get_next_int_msg((byte**)&sib_updated_ind_ptr, &buf_len);
  TF_ASSERT(sib_updated_ind_ptr->msg_hdr.id == LTE_RRC_SIB_UPDATED_INDI);
  
  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // ----------------------------------------------------------------------

lte_rrc_utf_check_no_more_msgs();
lte_rrc_sib_test_send_deact_req();
lte_rrc_utf_reset();

}

void LteRrcSibTestSib2ProcCurModPrdValTagChng::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibNewModPrdValTagChgTest);

void LteRrcSibNewModPrdValTagChgTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibNewModPrdValTagChgTest::Test()
{
  lte_rrc_sib_test_camped = FALSE;
  lte_rrc_sib_test_wcdma_supported = TRUE;
  lte_rrc_sib_test_gsm_supported = TRUE;
  lte_rrc_sib_test_cdma_1x_supported = TRUE;
  lte_rrc_sib_test_cdma_do_supported = TRUE;
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  /* Camp */
  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_camp_phase2();
  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4_mod_prd_change();

  // this will make CSP return this cell as serving cell, though camped_indi as 
  // not been sent for the cell
  lte_rrc_sib_test_camped = TRUE;
  lte_rrc_sib_test_sib_db_key = 0;
  lte_rrc_sib_test_serv_cell_phy_cell_id = PHY_CELL_ID_1;
  lte_rrc_sib_test_serv_cell_freq = FREQ_1;
  
  lte_rrc_sib_test_send_camped_indi(PHY_CELL_ID_1, FREQ_1);

  // Send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        896);

  // We expect a sib_sched_req message from RRC
  // Telling L1 to stop sending any more si msgs for this mod period
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 0);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        898);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send MIB to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            898);

  // ----------------------------------------------------------------------

  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 0);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == 0);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibNewModPrdValTagChgTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to validate the lte_rrc_convert_dl_earfcn_by_band API

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibDLFreqConvByBandApiTest);

void LteRrcSibDLFreqConvByBandApiTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibDLFreqConvByBandApiTest::Test()
{
  uint16 ovrlp_bands[45][2];
  uint8 i,j,k;
  lte_earfcn_t dl_earfcn;
  sys_sband_lte_e_type from_band, to_band;
  boolean is_ovrlp = FALSE;
  errno_enum_type status;
  uint16 max_band = 44;

  //Get all UL/DL overlapping bands
  lte_rrc_test_get_ul_dl_overlapping_bands(ovrlp_bands,FALSE);

  for (i = 1; i <= max_band; i++)
  {
    from_band = lte_rrc_band_from_int_to_enum(i);
    if (LTE_RRC_INVALID_LTE_SYS_BAND != from_band)
    {
      for (j = 1; j <= max_band; j++)
      {
        is_ovrlp = FALSE;
        to_band = lte_rrc_band_from_int_to_enum(j);
        if (LTE_RRC_INVALID_LTE_SYS_BAND != to_band)
        {
          dl_earfcn = lte_rrc_get_earfcn_dl_offset(from_band);
          status = lte_rrc_convert_dl_earfcn_by_band(&dl_earfcn, from_band, to_band);
          
          for (k=0;k<(sizeof(ovrlp_bands)/2);k++)
          {
            if (ovrlp_bands[k][0] == i &&
                ovrlp_bands[k][1] == j)
            {
              is_ovrlp = TRUE;
              break;
            }
          }
        
          TF_ASSERT(!(is_ovrlp == TRUE && status == E_NOT_SUPPORTED));
          TF_ASSERT(!(is_ovrlp == FALSE && status != E_NOT_SUPPORTED));
          
          if (status == E_DATA_INVALID)
          {
            TF_MSG("E_DATA_INVALID: dl_earfcn: %d; from_band: %d; to_band: %d", dl_earfcn,i,j);
          }
          
          if (status == E_SUCCESS)
          {
            TF_MSG("E_SUCCESS: dl_earfcn: %d; from_band: %d; to_band: %d", dl_earfcn,i,j);
          }
        }
      }
    }
  }
  lte_rrc_utf_reset();
}

void LteRrcSibDLFreqConvByBandApiTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to validate the lte_rrc_convert_dl_earfcn_by_band API

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibULFreqConvByBandApiTest);

void LteRrcSibULFreqConvByBandApiTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibULFreqConvByBandApiTest::Test()
{
  uint16 ovrlp_bands[45][2];
  lte_earfcn_t ul_freq[45] = {
    0,
    18000,
    18600,
    19200,
    19950,
    20400,
    20650,
    20750,
    21450,
    21800,
    22150,
    22750,
    23010,
    23180,
    23280,
    0,
    0,
    23730,
    23850,
    24000,
    24150,
    24450,
    24600,
    25500,
    25700,
    26040,
    26690,
    27040,
    27210,
    0,
    27660,
    27760,
    0,
    36000,
    36200,
    36350,
    36950,
    37550,
    37750,
    38250,
    38650,
    39650,
    41590,
    43590,
    45590
    }; 
    
  uint8 i,j,k;
  lte_earfcn_t ul_earfcn;
  sys_sband_lte_e_type from_band, to_band;
  boolean is_ovrlp = FALSE;
  errno_enum_type status;
  uint16 max_band = 44;

  //Get all UL/DL overlapping bands
  lte_rrc_test_get_ul_dl_overlapping_bands(ovrlp_bands,FALSE);

  for (i = 1; i <= max_band; i++)
  {
    from_band = lte_rrc_band_from_int_to_enum(i);
    if (LTE_RRC_INVALID_LTE_SYS_BAND != from_band)
    {
      for (j = 1; j <= max_band; j++)
      {
        is_ovrlp = FALSE;
        to_band = lte_rrc_band_from_int_to_enum(j);
        if (LTE_RRC_INVALID_LTE_SYS_BAND != to_band)
        {
          ul_earfcn = ul_freq[i];
          status = lte_rrc_convert_ul_earfcn_by_band(&ul_earfcn, from_band, to_band);
          
          for (k=0;k<(sizeof(ovrlp_bands)/2);k++)
          {
            if (ovrlp_bands[k][0] == i &&
                ovrlp_bands[k][1] == j)
            {
              is_ovrlp = TRUE;
              break;
            }
          }
          
          TF_ASSERT(!(is_ovrlp == TRUE && status == E_NOT_SUPPORTED));
          TF_ASSERT(!(is_ovrlp == FALSE && status != E_NOT_SUPPORTED));
          
          if (status == E_DATA_INVALID)
          {
            TF_MSG("E_DATA_INVALID: ul_earfcn: %d; from_band: %d; to_band: %d", ul_earfcn,i,j);
          }
          
          if (status == E_SUCCESS)
          {
            TF_MSG("E_SUCCESS: ul_earfcn: %d; from_band: %d; to_band: %d", ul_earfcn,i,j);
          }
        }
      }
    }
  }
  lte_rrc_utf_reset();
}

void LteRrcSibULFreqConvByBandApiTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST
Test to validate the lte_rrc_convert_dl_earfcn_by_band_wo_mfbi API

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibDLFreqConvByBandWoMfbiApiTest);

void LteRrcSibDLFreqConvByBandWoMfbiApiTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibDLFreqConvByBandWoMfbiApiTest::Test()
{
  uint16 ovrlp_bands[64][2];
  uint8 i,j,k;
  lte_earfcn_t dl_earfcn;
  sys_sband_lte_e_type from_band, to_band;
  boolean is_ovrlp = FALSE;
  errno_enum_type status;
  uint16 max_band = 44;

  //Get all DL overlapping bands
  lte_rrc_test_get_ul_dl_overlapping_bands(ovrlp_bands,TRUE);

  for (i = 1; i <= max_band; i++)
  {
    from_band = lte_rrc_band_from_int_to_enum(i);
    if (LTE_RRC_INVALID_LTE_SYS_BAND != from_band)
    {
      for (j = 1; j <= max_band; j++)
      {
        is_ovrlp = FALSE;
        to_band = lte_rrc_band_from_int_to_enum(j);
        if (LTE_RRC_INVALID_LTE_SYS_BAND != to_band)
        {
          dl_earfcn = lte_rrc_get_earfcn_dl_offset(from_band);
          status = _lte_rrc_convert_dl_earfcn_by_band_wo_mfbi(&dl_earfcn, from_band, to_band);
          
          for (k=0;k<(sizeof(ovrlp_bands)/2);k++)
          {
            if (ovrlp_bands[k][0] == i &&
                ovrlp_bands[k][1] == j)
            {
              is_ovrlp = TRUE;
              break;
            }
          }
        
          TF_ASSERT(!(is_ovrlp == TRUE && status == E_NOT_SUPPORTED));
          TF_ASSERT(!(is_ovrlp == FALSE && status != E_NOT_SUPPORTED));
          
          if (status == E_DATA_INVALID)
          {
            TF_MSG("E_DATA_INVALID: dl_earfcn: %d; from_band: %d; to_band: %d", dl_earfcn,i,j);
          }
          
          if (status == E_SUCCESS)
          {
            TF_MSG("E_SUCCESS: dl_earfcn: %d; from_band: %d; to_band: %d", dl_earfcn,i,j);
          }
        }
      }
    }
  }
  lte_rrc_utf_reset();
}

void LteRrcSibDLFreqConvByBandWoMfbiApiTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/* After Resel Failure, SIB1 has not changed. So ETWS reading is continued */
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsAfterReselFailureTest1);

void LteRrcSibEtwsAfterReselFailureTest1::Setup()
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

void LteRrcSibEtwsAfterReselFailureTest1::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  lte_rrc_sib_updated_indi_s *sib_updated_ind_ptr;
  lte_cphy_cell_resel_ind_s resel_ind;
  lte_errno_e status;
  
  uint32 buf_len;
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

  // ----------------------------------------------------------------------

  /* abort */
  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  TF_SLEEP(5000);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
                                      6);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        10);
                                        
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        15);

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        16);

  // ---------------------------------------------

  TF_SLEEP(100);

  // Prepare cell_resel_ind to send to RRC
  lte_rrc_init_default_hdr(&resel_ind.msgr_hdr, LTE_CPHY_CELL_RESEL_IND);
  resel_ind.serv_cell_info.dl_cell_frequency = FREQ_1;
  resel_ind.serv_cell_info.phy_cell_id = PHY_CELL_ID_1;
  resel_ind.target_cell_info.rat_type = LTE_RAT_EUTRAN;
  resel_ind.target_cell_info.rat_cell_info.lte_cell_info.dl_cell_frequency = FREQ_1;
  resel_ind.target_cell_info.rat_cell_info.lte_cell_info.phy_cell_id = PHY_CELL_ID_2;
  resel_ind.csg_resel = FALSE;
  resel_ind.sfn.curr_sfn = 16;

  // Send cell_resel_ind to RRC
  lte_rrc_utf_send_msg((byte*)&resel_ind, sizeof(resel_ind));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_SLEEP(320);

  lte_set_current_time(time_since_power_up_ms += 320);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);
  
  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));
  TF_ASSERT(read_error_ind_ptr->cause == LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_RESEL_FAIL, FALSE, 
                                      LTE_RRC_NULL_SFN);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0xc02);


  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2310_45_11_mcc_8_5,
                                        sizeof(sib1_2310_45_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        50);

  lte_rrc_utf_get_next_int_msg((byte**)&sib_updated_ind_ptr, &buf_len);
  TF_ASSERT(sib_updated_ind_ptr->msg_hdr.id == LTE_RRC_SIB_UPDATED_INDI);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0xc00);
  
  // ----------------------------------------------------------------------

  TF_SLEEP(100);

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsAfterReselFailureTest1::Teardown()
{
  lte_rrc_utf_teardown();
}

/* After Resel Failure, SIB1 has changed to remove the ETWS SIBs. So ETWS reading is stopped */
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibEtwsAfterReselFailureTest2);

void LteRrcSibEtwsAfterReselFailureTest2::Setup()
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

void LteRrcSibEtwsAfterReselFailureTest2::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  lte_rrc_sib_updated_indi_s *sib_updated_ind_ptr;
  lte_cphy_cell_resel_ind_s resel_ind;
  lte_errno_e status;
  
  uint32 buf_len;
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

  // ----------------------------------------------------------------------

  /* abort */
  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  TF_SLEEP(5000);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_ETWS, FALSE, 
                                      6);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_10_11_mcc_8_5,
                                        sizeof(sib1_2345_10_11_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        10);
                                        
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // Send SIB10
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib10_8_5,
                                        sizeof(sib10_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        15);

  // We expect a etws_prim_ind from SIB
  lte_rrc_utf_get_next_ext_msg((byte**)&etws_prim_ind_ptr, &buf_len);
  TF_ASSERT(etws_prim_ind_ptr->msg_hdr.id == LTE_RRC_ETWS_PRIM_IND);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
  // Send SIB11 segment 1
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib11_seg1of2_8_5,
                                        sizeof(sib11_seg1of2_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        16);

  // ---------------------------------------------

  TF_SLEEP(100);

  // Prepare cell_resel_ind to send to RRC
  lte_rrc_init_default_hdr(&resel_ind.msgr_hdr, LTE_CPHY_CELL_RESEL_IND);
  resel_ind.serv_cell_info.dl_cell_frequency = FREQ_1;
  resel_ind.serv_cell_info.phy_cell_id = PHY_CELL_ID_1;
  resel_ind.target_cell_info.rat_type = LTE_RAT_EUTRAN;
  resel_ind.target_cell_info.rat_cell_info.lte_cell_info.dl_cell_frequency = FREQ_1;
  resel_ind.target_cell_info.rat_cell_info.lte_cell_info.phy_cell_id = PHY_CELL_ID_2;
  resel_ind.csg_resel = FALSE;
  resel_ind.sfn.curr_sfn = 16;

  // Send cell_resel_ind to RRC
  lte_rrc_utf_send_msg((byte*)&resel_ind, sizeof(resel_ind));
  lte_rrc_utf_wait_for_done();

  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);
  
  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_2, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL, TRUE, 
                                      LTE_RRC_NULL_SFN);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_SLEEP(320);

  lte_set_current_time(time_since_power_up_ms += 320);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  lte_rrc_tmri_s *tmr_ptr;
  lte_rrc_utf_get_next_int_msg((byte**)&tmr_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  TF_ASSERT(tmr_ptr->msg_hdr.id == LTE_RRC_SIB_EVENT1_WT_TMRI);
  
  lte_rrc_sib_read_error_indi_s *read_error_ind_ptr;
  lte_rrc_utf_get_next_int_msg((byte**)&read_error_ind_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_read_error_indi_s));
  TF_ASSERT(read_error_ind_ptr->cause == LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_CSP,
                                      LTE_RRC_GET_SIBS_CAUSE_RESEL_FAIL, FALSE, 
                                      LTE_RRC_NULL_SFN);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0xc02);

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        46);

  lte_rrc_utf_get_next_int_msg((byte**)&sib_updated_ind_ptr, &buf_len);
  TF_ASSERT(sib_updated_ind_ptr->msg_hdr.id == LTE_RRC_SIB_UPDATED_INDI);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == 0);
  
  // ----------------------------------------------------------------------

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibEtwsAfterReselFailureTest2::Teardown()
{
  lte_rrc_utf_teardown();
}
/*===========================================================================

                                UNIT TEST
===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibTestIdleToConnWithMandSibPending);

void LteRrcSibTestIdleToConnWithMandSibPending::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();

}

void LteRrcSibTestIdleToConnWithMandSibPending::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  lte_rrc_sib_updated_indi_s *sib_updated_ind_ptr;
  
  uint32 buf_len;
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

  // ----------------------------------------------------------------------

  /* abort */
  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  TF_SLEEP(5000);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE, FALSE, 
                                      72);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_234567_mcc_8_5_v1,
                                        sizeof(sib1_234567_mcc_8_5_v1),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        130);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        128);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  
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

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK |
                                         LTE_RRC_SIB6_BITMASK |
                                         LTE_RRC_SIB7_BITMASK));

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib2_sib3_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        192);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_234567_mcc_8_5_v1,
                                        sizeof(sib1_234567_mcc_8_5_v1),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        194);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib2_sib3_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        196);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);
  

  lte_rrc_sib_test_wcdma_supported = FALSE;
  lte_rrc_sib_test_gsm_supported = FALSE;

  // Send connected_indi
  lte_rrc_sib_test_send_connected_indi();

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            196);

  lte_rrc_sib_test_rcv_sib_updated_indi(PHY_CELL_ID_1,
                                        FREQ_1,
                                        (LTE_RRC_SIB5_BITMASK),
                                        (LTE_RRC_MIB_BITMASK | 
                                         LTE_RRC_SIB1_BITMASK | 
                                         LTE_RRC_SIB2_BITMASK | 
                                         LTE_RRC_SIB3_BITMASK | 
                                         LTE_RRC_SIB4_BITMASK |
                                         LTE_RRC_SIB5_BITMASK ));

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // ----------------------------------------------------------------------

  TF_SLEEP(100);

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_sib_test_send_deact_req();
  lte_rrc_utf_reset();
}

void LteRrcSibTestIdleToConnWithMandSibPending::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibInvalidSib1Test);

void LteRrcSibInvalidSib1Test::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibInvalidSib1Test::Test()
{
  lte_rrc_sib_test_camped = FALSE;
  
  /* Test the schedule requests */
  lte_cphy_sib_sched_req_s *sib_sched_req;
  
  uint32 buf_len;
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

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

  // Ignoring next_mod_prd_sib_mask, si_window_length, num_si_msgs, si_msg_info

  // ---------------------------------------------

  // Prepare mib_ind to send to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_25,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            100,
                            1);

  // ---------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req->freq == 100);
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_invalid,
                                        sizeof(sib1_invalid),
                                        PHY_CELL_ID_1,
                                        100,
                                        2);

  // ----------------------------------------------------------------------

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req, &buf_len);
  TF_ASSERT(sib_sched_req->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req->freq == 100);
  TF_ASSERT(sib_sched_req->curr_mod_prd_sib_mask == (LTE_RRC_SIB2_BITMASK |
                                                         LTE_RRC_SIB3_BITMASK |
                                                         LTE_RRC_SIB4_BITMASK |
                                                         LTE_RRC_SIB7_BITMASK));


  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibInvalidSib1Test::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibExtndSibTmrTest);

void LteRrcSibExtndSibTmrTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibExtndSibTmrTest::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  lte_rrc_get_sibs_cnfi_s *get_sibs_cnf_ptr;
  lte_rrc_etws_prim_ind_s *etws_prim_ind_ptr;
  lte_rrc_etws_sec_ind_s *etws_sec_ind_ptr;
  lte_rrc_sib_updated_indi_s *sib_updated_ind_ptr;
  
  uint32 buf_len;
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

  // ----------------------------------------------------------------------

  /* abort */
  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  TF_SLEEP(5000);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE, FALSE, 
                                      72);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  //Cross the mod period
  TF_SLEEP(660);

  // send SIB2_3 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        130);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  /* TEST1: Extension request during active SIB reading; timer must be restarted */ 
  lte_rrc_sib_test_auto_gap_is_active = TRUE;
  //Send the SIB tmr extnd reqi while the event is active
  lte_rrc_sib_test_send_extnd_tmr_reqi(150);

  TF_SLEEP(160);

  lte_rrc_sib_test_auto_gap_is_active = FALSE;

  // send MIB to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            132);

  // ----------------------------------------------------------------------
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE, FALSE, 
                                      172);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);


  /* TEST2: Sufficient time i curr mod period; so extension is not required */ 
  lte_rrc_sib_test_auto_gap_is_active = TRUE;
  //Send the SIB tmr extnd reqi while the event is active
  lte_rrc_sib_test_send_extnd_tmr_reqi(150);

  TF_SLEEP(160);

  lte_rrc_sib_test_auto_gap_is_active = FALSE;

  /* abort */
  lte_rrc_sib_test_send_abort_reqi(PHY_CELL_ID_1, FREQ_1);

  lte_rrc_sib_test_send_get_sibs_reqi(PHY_CELL_ID_1, FREQ_1, LTE_RRC_PROC_PG,
                                      LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE, FALSE, 
                                      182);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);


  /* TEST2: Sufficient time i curr mod period; so extension is not required */ 
  lte_rrc_sib_test_auto_gap_is_active = TRUE;
  //Send the SIB tmr extnd reqi while the event is active
  lte_rrc_sib_test_send_extnd_tmr_reqi(150);

  TF_SLEEP(160);

  lte_rrc_sib_test_auto_gap_is_active = FALSE;

  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        200);

  // We expect a sib_sched_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  // send MIB to RRC
  lte_rrc_sib_test_send_mib(1,
                            LTE_BW_NRB_100,
                            LTE_L1_PHICH_DURATION_NORMAL,
                            LTE_L1_PHICH_RESOURCE_ONE,
                            PHY_CELL_ID_1,
                            FREQ_1,
                            200);

  // ----------------------------------------------------------------------
  lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_SLEEP(600);

  /* TEST2: Sufficient time i curr mod period; so extension is not required */ 
  lte_rrc_sib_test_auto_gap_is_active = TRUE;
  //Send the SIB tmr extnd reqi while the event is active
  lte_rrc_sib_test_send_extnd_tmr_reqi(150);

  TF_SLEEP(160);

  lte_rrc_sib_test_auto_gap_is_active = FALSE;

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}

void LteRrcSibExtndSibTmrTest::Teardown()
{
  lte_rrc_utf_teardown();
}
/*===========================================================================

                                UNIT TEST
Test to validate the lte_rrc_llc_validate_and_update_mcch_schdl_cfg API

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcValidateAndUpdateMCCHSchdlCfgApiTest);

void LteRrcLlcValidateAndUpdateMCCHSchdlCfgApiTest::Setup()
{
  uint32 umid_list[] = 
    {
      LTE_CPHY_SIB_SCHED_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcLlcValidateAndUpdateMCCHSchdlCfgApiTest::Test()
{
  lte_cphy_mbsfn_area_cfg_s mbsfn_area_cfg;
  uint16 sf_count = 0;

#ifdef FEATURE_LTE_EMBMS
  lte_rrc_cfg_db_type cfg_db;

  cfg_db.mbsfn_cfg.num_mbms_allocations = 2;
  cfg_db.mbsfn_cfg.mbsfn_alloacation[0].allocation_period = LTE_L1_MBSFN_ALLOCATION_PERIOD_4;
  cfg_db.mbsfn_cfg.mbsfn_alloacation[0].allocation_offset = 3;
  cfg_db.mbsfn_cfg.mbsfn_alloacation[0].sfn_allocation = LTE_L1_MBSFN_ALLOCATION_FOUR;
  cfg_db.mbsfn_cfg.mbsfn_alloacation[0].sfn_allocation_mask = 0x6DB6DB;

  cfg_db.mbsfn_cfg.mbsfn_alloacation[1].allocation_period = LTE_L1_MBSFN_ALLOCATION_PERIOD_8;
  cfg_db.mbsfn_cfg.mbsfn_alloacation[1].allocation_offset = 0;
  cfg_db.mbsfn_cfg.mbsfn_alloacation[1].sfn_allocation = LTE_L1_MBSFN_ALLOCATION_ONE;
  cfg_db.mbsfn_cfg.mbsfn_alloacation[1].sfn_allocation_mask = 0x1D;

  cfg_db.mbsfn_areas_mcch_schdl_cfg.num_mbsfn_areas = 8;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[0].mcch_schdl_cfg.sf_alloc_info_bitmask = 0x30;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[0].mcch_schdl_cfg.offset = 3;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[0].mcch_schdl_cfg.rep_period = LTE_L1_MBSFN_MCCH_REP_PERIOD_32RF;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[0].area_id = 0;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[0].cfg_action = LTE_L1_CONFIG;
                                                 
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[1].mcch_schdl_cfg.sf_alloc_info_bitmask = 0x30;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[1].mcch_schdl_cfg.offset = 4;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[1].mcch_schdl_cfg.rep_period = LTE_L1_MBSFN_MCCH_REP_PERIOD_32RF;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[1].area_id = 1;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[1].cfg_action = LTE_L1_CONFIG;
                                                 
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[2].mcch_schdl_cfg.sf_alloc_info_bitmask = 0x30;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[2].mcch_schdl_cfg.offset = 5;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[2].mcch_schdl_cfg.rep_period = LTE_L1_MBSFN_MCCH_REP_PERIOD_32RF;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[2].area_id = 2;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[2].cfg_action = LTE_L1_CONFIG;
                                                 
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[3].mcch_schdl_cfg.sf_alloc_info_bitmask = 0x30;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[3].mcch_schdl_cfg.offset = 6;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[3].mcch_schdl_cfg.rep_period = LTE_L1_MBSFN_MCCH_REP_PERIOD_32RF;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[3].area_id = 3;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[3].cfg_action = LTE_L1_CONFIG;
                                                 
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[4].mcch_schdl_cfg.sf_alloc_info_bitmask = 1;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[4].mcch_schdl_cfg.offset = 0;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[4].mcch_schdl_cfg.rep_period = LTE_L1_MBSFN_MCCH_REP_PERIOD_32RF;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[4].area_id = 4;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[4].cfg_action = LTE_L1_CONFIG;
                                                 
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[5].mcch_schdl_cfg.sf_alloc_info_bitmask = 1;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[5].mcch_schdl_cfg.offset = 3;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[5].mcch_schdl_cfg.rep_period = LTE_L1_MBSFN_MCCH_REP_PERIOD_32RF;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[5].area_id = 5;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[5].cfg_action = LTE_L1_CONFIG;
                                                 
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[6].mcch_schdl_cfg.sf_alloc_info_bitmask = 1;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[6].mcch_schdl_cfg.offset = 4;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[6].mcch_schdl_cfg.rep_period = LTE_L1_MBSFN_MCCH_REP_PERIOD_32RF;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[6].area_id = 6;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[6].cfg_action = LTE_L1_CONFIG;
                                                 
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[7].mcch_schdl_cfg.sf_alloc_info_bitmask = 1;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[7].mcch_schdl_cfg.offset = 5;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[7].mcch_schdl_cfg.rep_period = LTE_L1_MBSFN_MCCH_REP_PERIOD_32RF;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[7].area_id = 7;
  cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[7].cfg_action = LTE_L1_CONFIG;

  lte_rrc_llc_validate_and_update_mcch_schdl_cfg(&cfg_db, LTE_L1_MBSFN_ALLOCATION_PERIOD_8);

  TF_ASSERT(cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[0].mcch_schdl_cfg.sf_alloc_info_bitmask == 0x10);
  TF_ASSERT(cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[1].mcch_schdl_cfg.sf_alloc_info_bitmask == 0x10);
  TF_ASSERT(cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[2].mcch_schdl_cfg.sf_alloc_info_bitmask == 0x10);
  TF_ASSERT(cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[3].mcch_schdl_cfg.sf_alloc_info_bitmask == 0x10);
  TF_ASSERT(cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[4].mcch_schdl_cfg.sf_alloc_info_bitmask == 0x1);
  TF_ASSERT(cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[5].mcch_schdl_cfg.sf_alloc_info_bitmask == 0x1);
  TF_ASSERT(cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[6].mcch_schdl_cfg.sf_alloc_info_bitmask == 0x1);
  TF_ASSERT(cfg_db.mbsfn_areas_mcch_schdl_cfg.mbsfn_area[7].mcch_schdl_cfg.sf_alloc_info_bitmask == 0x1);
#endif

  lte_rrc_utf_reset();
}

void LteRrcLlcValidateAndUpdateMCCHSchdlCfgApiTest::Teardown()
{
  lte_rrc_utf_teardown();
}


