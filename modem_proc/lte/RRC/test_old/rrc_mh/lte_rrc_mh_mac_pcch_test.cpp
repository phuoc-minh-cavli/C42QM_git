/*!
  @file lte_rrc_mh_mac_ccch_test.cpp

  @brief
  Unit tests for MH
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/rrc_mh/lte_rrc_mh_mac_pcch_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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
#include "__lte_rrc_mh.h"
#include "lte_rrc_mhi.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_mac_ind.h"
#include "lte_mac_msg.h"
#include "dsm_item.h"
#include "dsm_init.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "tf_stub.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/





/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

extern "C"
{
  TF_PROTO_STUB(stm_state_t, lte_rrc_controller_get_state, void);
  extern lte_rrc_mh_s lte_rrc_mh;
}

extern stm_state_t lte_rrc_mh_test_stub_controller_get_state();

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/






/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMHMacPcchTest);

void LteRrcMHMacPcchTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_MH_SM, NULL, 0);
  TF_STUB(lte_rrc_controller_get_state) = 
    lte_rrc_mh_test_stub_controller_get_state;
}

void LteRrcMHMacPcchTest::Test()
{
  lte_mac_rrc_dl_data_ind_s dl_msg_ind;
  
  dsm_item_type *dsm_ptr=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr;           /* Message attachment pointer */
  unsigned char *encoded_msg;
  long length;


  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  // ----------------------------------------------------------------------

  msgr_init_hdr_attach(&dl_msg_ind.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_PCCH_DL_DATA_IND, 0, 1);

  length = 1;
  encoded_msg = (unsigned char *)lte_rrc_mem_calloc(1);
  *encoded_msg = 0x38;

  TF_ASSERT(dsm_pushdown(&dsm_ptr, encoded_msg, (uint16)length, DSM_DS_SMALL_ITEM_POOL) == (uint16)length);

  TF_ASSERT(dsm_ptr != NULL);

  att_ptr = msgr_get_attach(&dl_msg_ind.hdr, 0);
  msgr_set_dsm_attach(att_ptr, dsm_ptr);

  dl_msg_ind.phy_cell_id = 1;
  dl_msg_ind.freq = 2;
  dl_msg_ind.sfn.sys_fn = 3;


  lte_rrc_utf_send_msg((byte*)&dl_msg_ind, sizeof(dl_msg_ind));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);
  
  TF_ASSERT(lte_rrc_mh.dd_ptr->dl_hist[0].cell_id.phy_cell_id == 1);
  TF_ASSERT(lte_rrc_mh.dd_ptr->dl_hist[0].cell_id.freq == 2);
  TF_ASSERT(lte_rrc_mh.dd_ptr->dl_hist[0].encoded_msg[0] == 0x38);
  TF_ASSERT(lte_rrc_mh.dd_ptr->dl_hist[0].length == 1);
  TF_ASSERT(lte_rrc_mh.dd_ptr->dl_hist[0].pdu_num == lte_rrc_osys_PCCH_Message_PDU);
  TF_ASSERT(lte_rrc_mh.dd_ptr->dl_hist[0].sfn.sys_fn == 3);

  lte_rrc_mem_free(encoded_msg);

  lte_rrc_mh_dlm_s *dlm;
  uint32 buf_len;
  lte_rrc_utf_get_next_int_msg((byte**)&dlm, &buf_len);

  TF_ASSERT(dlm->cell_id.freq == 2);
  TF_ASSERT(dlm->cell_id.phy_cell_id == 1);
  TF_ASSERT(dlm->sfn.sys_fn == 3);
  TF_ASSERT(dlm->pdu_num == lte_rrc_osys_PCCH_Message_PDU);
  
  lte_rrc_utf_reset();
}


void LteRrcMHMacPcchTest::Teardown()
{
  lte_rrc_utf_teardown();
}
