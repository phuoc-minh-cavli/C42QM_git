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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_dsds_tests.cpp#1 $

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

extern "C"
{
extern lte_rrc_sib_s lte_rrc_sib;
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

extern lte_rrc_rf_status_e lte_rrc_sib_test_rf_status;
extern uint64 lte_rrc_sib_test_rf_status_time;
extern lte_l1_sfn_t lte_rrc_sib_test_rf_status_sfn;
extern uint8 lte_rrc_sib_num_po_missed_from_rf_avail;

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

              


/*===========================================================================

                                UNIT TEST
An SI is received when already present in db (before cell is found in db). When 
cell is found in db (on rcving SIB1), the SI in db should be replaced (check by 
changing the SI)

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibRcvSIWhenAlreadyPresentTestWithDSDS);

void LteRrcSibRcvSIWhenAlreadyPresentTestWithDSDS::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibRcvSIWhenAlreadyPresentTestWithDSDS::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_rf_status_sfn = 1;
  lte_rrc_sib_test_send_rf_unavail_ind();
  TF_SLEEP(100);
  lte_rrc_sib_test_rf_status_sfn = 5;
  lte_rrc_sib_test_send_rf_avail_ind();

  // We expect a sib_sched_req message from RRC

  lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_1,                   
                                       FREQ_1,                        
                                       (LTE_RRC_MIB_BITMASK |         
                                        LTE_RRC_SIB1_BITMASK),                             
                                       65535,                            
                                       (LTE_RRC_MIB_BITMASK |         
                                        LTE_RRC_SIB1_BITMASK),        
                                       65535,                             
                                       LTE_CPHY_SI_WINDOW_LENGTH_MS_1,
                                       0);                            


  lte_rrc_sib_test_camp_phase2();

  lte_rrc_sib_test_rf_status_sfn = 11;
  lte_rrc_sib_test_send_rf_unavail_ind();
  TF_SLEEP(100);
  lte_rrc_sib_test_rf_status_sfn = 15;
  lte_rrc_sib_test_send_rf_avail_ind();

  // We expect a sib_sched_req message from RRC

  lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_1,                   
                                       FREQ_1,                        
                                       (LTE_RRC_SIB1_BITMASK),                             
                                       64,                            
                                       (LTE_RRC_MIB_BITMASK |         
                                        LTE_RRC_SIB1_BITMASK),        
                                       1,                             
                                       LTE_CPHY_SI_WINDOW_LENGTH_MS_1,
                                       0); 

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

void LteRrcSibRcvSIWhenAlreadyPresentTestWithDSDS::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST
An SI is received when already present in db (before cell is found in db). When 
cell is found in db (on rcving SIB1), the SI in db should be replaced (check by 
changing the SI)

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibDSDSRfAvailAndModBndryCrossed);

void LteRrcSibDSDSRfAvailAndModBndryCrossed::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibDSDSRfAvailAndModBndryCrossed::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_rf_status_sfn = 1;
  lte_rrc_sib_test_rf_status_time = 15000000;
  lte_rrc_sib_test_send_rf_unavail_ind();
  TF_SLEEP(100);
  lte_rrc_sib_test_rf_status_sfn = 5;
  lte_rrc_sib_test_rf_status_time = 15000080;
  lte_rrc_sib_test_send_rf_avail_ind();

  // We expect a sib_sched_req message from RRC

  lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_1,                   
                                       FREQ_1,                        
                                       (LTE_RRC_MIB_BITMASK |         
                                        LTE_RRC_SIB1_BITMASK),                             
                                       65535,                            
                                       (LTE_RRC_MIB_BITMASK |         
                                        LTE_RRC_SIB1_BITMASK),        
                                       65535,                             
                                       LTE_CPHY_SI_WINDOW_LENGTH_MS_1,
                                       0);                            


  lte_rrc_sib_test_camp_phase2();

  lte_rrc_sib_test_rf_status_sfn = 11;
  lte_rrc_sib_test_send_rf_unavail_ind();
  TF_SLEEP(100);
  lte_rrc_sib_test_rf_status_sfn = 15;
  lte_rrc_sib_test_send_rf_avail_ind();

  // We expect a sib_sched_req message from RRC

  lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_1,                   
                                       FREQ_1,                        
                                       (LTE_RRC_SIB1_BITMASK),                             
                                       64,                            
                                       (LTE_RRC_MIB_BITMASK |         
                                        LTE_RRC_SIB1_BITMASK),        
                                       1,                             
                                       LTE_CPHY_SI_WINDOW_LENGTH_MS_1,
                                       0); 

  lte_rrc_sib_test_camp_phase3();
  lte_rrc_sib_test_camp_phase4();
  lte_rrc_sib_test_camp_phase5();
  lte_rrc_sib_test_camp_phase6();

  lte_rrc_sib_test_rf_status_sfn = 1;
  lte_rrc_sib_test_rf_status_time = 10000000;
  lte_rrc_sib_test_send_rf_unavail_ind();

  lte_rrc_sib_test_rf_status_sfn = 65;
  lte_rrc_sib_test_rf_status_time = 35000000;
  lte_rrc_sib_num_po_missed_from_rf_avail = 2;
  lte_rrc_sib_test_send_rf_avail_ind();

  /* No event active but RF_UNAVAIL for long time*/
  lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_1,                   
                                       FREQ_1,                        
                                       (LTE_RRC_MIB_BITMASK |
                                        LTE_RRC_SIB1_BITMASK),                             
                                       65535,                            
                                       (0),        
                                       65535,                             
                                       LTE_CPHY_SI_WINDOW_LENGTH_MS_2,
                                       2);

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
  TF_ASSERT(sib_sched_req_ptr->curr_mod_prd_sib_mask == (LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->mod_period_boundary_sfn == 64);
  TF_ASSERT(sib_sched_req_ptr->next_mod_prd_sib_mask == (LTE_RRC_MIB_BITMASK | 
                                                         LTE_RRC_SIB1_BITMASK));
  TF_ASSERT(sib_sched_req_ptr->last_sfn_in_sib_rsp == 1);

  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

    lte_rrc_utf_get_next_ext_msg((byte**)&sib_sched_req_ptr, &buf_len);
  TF_ASSERT(sib_sched_req_ptr->msgr_hdr.id == LTE_CPHY_SIB_SCHED_REQ);

  TF_ASSERT(sib_sched_req_ptr->phy_cell_id == PHY_CELL_ID_1);
  TF_ASSERT(sib_sched_req_ptr->freq == FREQ_1);

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

void LteRrcSibDSDSRfAvailAndModBndryCrossed::Teardown()
{
  lte_rrc_utf_teardown();
}



/*===========================================================================

                                UNIT TEST
An SI is received when already present in db (before cell is found in db). When 
cell is found in db (on rcving SIB1), the SI in db should be replaced (check by 
changing the SI)

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibRcvSibAfterRFUnavailInd);

void LteRrcSibRcvSibAfterRFUnavailInd::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibRcvSibAfterRFUnavailInd::Test()
{
  lte_cphy_sib_sched_req_s *sib_sched_req_ptr;
  uint32 buf_len;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_camp_phase1();
  lte_rrc_sib_test_rf_status_sfn = 1;
  lte_rrc_sib_test_send_rf_unavail_ind();
  TF_SLEEP(100);
  lte_rrc_sib_test_rf_status_sfn = 5;
  lte_rrc_sib_test_send_rf_avail_ind();
  
  // We expect a sib_sched_req message from RRC

  lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_1,                   
                                       FREQ_1,                        
                                       (LTE_RRC_MIB_BITMASK |         
                                        LTE_RRC_SIB1_BITMASK),                             
                                       65535,                            
                                       (LTE_RRC_MIB_BITMASK |         
                                        LTE_RRC_SIB1_BITMASK),        
                                       65535,                             
                                       LTE_CPHY_SI_WINDOW_LENGTH_MS_1,
                                       0);                            

  lte_rrc_sib_test_camp_phase2();

  lte_rrc_sib_test_rf_status_sfn = 11;
  lte_rrc_sib_test_send_rf_unavail_ind();
  TF_SLEEP(100);
  lte_rrc_sib_test_rf_status_sfn = 15;
  lte_rrc_sib_test_send_rf_avail_ind();

  // We expect a sib_sched_req message from RRC

  lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_1,                   
                                       FREQ_1,                        
                                       (LTE_RRC_SIB1_BITMASK),                             
                                       64,                            
                                       (LTE_RRC_MIB_BITMASK |         
                                        LTE_RRC_SIB1_BITMASK),        
                                       1,                             
                                       LTE_CPHY_SI_WINDOW_LENGTH_MS_1,
                                       0); 

  lte_rrc_sib_test_rf_status_sfn = 4;
  lte_rrc_sib_test_send_rf_unavail_ind();
  TF_SLEEP(100);
  
  // send SIB1 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib1_2345_mcc_8_5,
                                        sizeof(sib1_2345_mcc_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        2);

  lte_rrc_sib_test_rf_status_sfn = 28;
  lte_rrc_sib_test_send_rf_avail_ind();


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
                                       2,                             
                                       LTE_CPHY_SI_WINDOW_LENGTH_MS_2,
                                       2); 

  lte_rrc_sib_test_camp_phase4();

  lte_rrc_sib_test_rf_status_sfn = 4;
  lte_rrc_sib_test_send_rf_unavail_ind();
  TF_SLEEP(100);

  // send SIB4_5 to RRC
  lte_rrc_sib_test_send_bcch_dl_sch_msg(sib4_sib5_8_5,
                                        sizeof(sib4_sib5_8_5),
                                        PHY_CELL_ID_1,
                                        FREQ_1,
                                        4);

  lte_rrc_sib_test_rf_status_sfn = 10;
  lte_rrc_sib_test_send_rf_avail_ind();

  lte_rrc_sib_test_rcv_sib_sched_req(PHY_CELL_ID_1,                   
                                       FREQ_1,                        
                                       (0),                             
                                       64,                            
                                       (LTE_RRC_MIB_BITMASK |         
                                        LTE_RRC_SIB1_BITMASK |
                                        LTE_RRC_SIB2_BITMASK |
                                        LTE_RRC_SIB3_BITMASK |
                                        LTE_RRC_SIB4_BITMASK |
                                        LTE_RRC_SIB5_BITMASK),        
                                       4,                             
                                       LTE_CPHY_SI_WINDOW_LENGTH_MS_2,
                                       2);

  lte_rrc_sib_test_camp_phase6();

  // ----------------------------------------------------------------------

  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr->m.ac_BarringInfoPresent == 0);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_decoded_sibs.sib2_ptr->m.mbsfn_SubframeConfigListPresent == 0);

  // ----------------------------------------------------------------------

  lte_rrc_sib_test_send_stopped_indi();

  // ----------------------------------------------------------------------
  lte_rrc_utf_reset();
}

void LteRrcSibRcvSibAfterRFUnavailInd::Teardown()
{
  lte_rrc_utf_teardown();
}
