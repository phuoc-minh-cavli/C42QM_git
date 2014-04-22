/*!
  @file lte_rrc_sib_mib_test2.cpp

  @brief
  MIB & SIB1 received for a cell for which event is not active
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_mib_test2.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/08/09   amit    Modified the test for the new SIB design
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
MIB & SIB1 received for a cell for which event is not active

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSibMibTest2);

void LteRrcSibMibTest2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_SIB_SCHED_REQ
    };
  lte_rrc_utf_setup(LTE_RRC_SIB_SM, umid_list, 1);
}

void LteRrcSibMibTest2::Test()
{
  // ----------------------------------------------------------------------
  lte_cphy_mib_ind_s mib_ind;
  // ----------------------------------------------------------------------

  dsm_item_type *dsm_ptr=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr;           /* Message attachment pointer */
  lte_mac_rrc_dl_data_ind_s dl_data_ind;
  lte_rrc_unsolicited_sib_indi_s *unsolicited_sib_ind_ptr;
  uint32 buf_len;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

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

  TF_ASSERT(lte_rrc_sib.dd_ptr->events_db.num_active_events == 0);

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

  TF_ASSERT(stm_get_state(LTE_RRC_SIB_SM) == LTE_RRC_SIB_SM__INACTIVE);

  // We expect a unsolicited_sib_ind from SIB
  lte_rrc_utf_get_next_int_msg((byte**)&unsolicited_sib_ind_ptr, &buf_len);
  TF_ASSERT(unsolicited_sib_ind_ptr->msg_hdr.id == LTE_RRC_UNSOLICITED_SIB_INDI);
  TF_ASSERT(unsolicited_sib_ind_ptr->cell_id.phy_cell_id == 42);
  TF_ASSERT(unsolicited_sib_ind_ptr->cell_id.freq == 2000);
  TF_ASSERT(unsolicited_sib_ind_ptr->encoded_si_size == sizeof(sib1_mcc_8_5));


  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset();
}


void LteRrcSibMibTest2::Teardown()
{
  lte_rrc_utf_teardown();
}
