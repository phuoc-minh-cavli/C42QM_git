/*!
  @file lte_rrc_sib_mand_sibs_test1.cpp

  @brief
  SIB1 received for a cell, event active for that cell
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_mand_sibs_test1.cpp#1 $

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

DECLARE_SIB1_MCC_8_5;
DECLARE_SIB2_8_5;

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

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibMandSibsTest1);

void LteRrcSibMandSibsTest1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ,
      LTE_RRC_GET_SIBS_CNFI
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 2);
  lte_rrc_sib_test_stub_init();
}

void LteRrcSibMandSibsTest1::Test()
{
  lte_rrc_get_sibs_reqi_s get_sibs_req;
  
  // ----------------------------------------------------------------------

  lte_cphy_mib_ind_s mib_ind;
  // ----------------------------------------------------------------------

  dsm_item_type *dsm_ptr=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr;           /* Message attachment pointer */
  lte_mac_rrc_dl_data_ind_s dl_data_ind;
  
  lte_mac_rrc_dl_data_ind_s dl_data_ind_sib2;
  dsm_item_type *dsm_ptr_sib2=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr_sib2;           /* Message attachment pointer */
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

  lte_rrc_sib_test_send_rf_unavail_ind();
  TF_SLEEP(100);
  lte_rrc_sib_test_send_rf_avail_ind();

  // ----------------------------------------------------------------------

  /* SIB1 */
  msgr_init_hdr_attach(&dl_data_ind.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_BCCH_DL_DATA_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr, sib1_mcc_8_5, sizeof(sib1_mcc_8_5), DSM_DS_SMALL_ITEM_POOL) == sizeof(sib1_mcc_8_5));

  TF_ASSERT(dsm_ptr != NULL);

  att_ptr = msgr_get_attach(&dl_data_ind.hdr, 0);
  msgr_set_dsm_attach(att_ptr, dsm_ptr);

  dl_data_ind.freq = 2000;
  dl_data_ind.phy_cell_id = 42;
  dl_data_ind.sfn.sys_fn = 3;


  lte_rrc_utf_send_msg((byte*)&dl_data_ind, sizeof(dl_data_ind));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  // ----------------------------------------------------------------------

  /* SIB2 */
  msgr_init_hdr_attach(&dl_data_ind_sib2.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_BCCH_DL_DATA_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr_sib2, sib2_8_5, sizeof(sib2_8_5), DSM_DS_SMALL_ITEM_POOL) == sizeof(sib2_8_5));

  TF_ASSERT(dsm_ptr_sib2 != NULL);

  att_ptr_sib2 = msgr_get_attach(&dl_data_ind_sib2.hdr, 0);
  msgr_set_dsm_attach(att_ptr_sib2, dsm_ptr_sib2);

  dl_data_ind_sib2.freq = 2000;
  dl_data_ind_sib2.phy_cell_id = 42;
  dl_data_ind_sib2.sfn.sys_fn = 5;


  lte_rrc_utf_send_msg((byte*)&dl_data_ind_sib2, sizeof(dl_data_ind_sib2));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__ACTIVE);

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.event[0].sibs_rcvd_bitmask == 7);
  TF_ASSERT(lte_rrc_sib.sd_ptr->sib_db_ptr->cell[0].all_si.sibs_bitmask == 4);

  lte_rrc_sib_test_send_stopped_indi();
  lte_rrc_utf_reset();
}


void LteRrcSibMandSibsTest1::Teardown()
{
  lte_rrc_utf_teardown();
}
