/*!
  @file lte_rrc_crp_stopped_conn_release_test.cpp

  @brief
  Unit test code for taking CRP from Inactive -> Wait for Timer ->
  -> Inactive

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

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/08   sureshs Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include <lte_rrc_int_msg.h>
#include <lte_rrc_ext_msg.h>
#include <__lte_rrc_crp.h>
#include <__lte_rrc_controller.h>
#include <lte_rrc_utils.h>
#include "lte_rrc_crp_test_asn.h"
}

#include <TestFramework.h>
#include <lte_rrc_utf.h>
#include "lte_rrc_crp_test_common.h"

//message structures
static lte_rrc_mh_dlm_s conn_rel_dlm;
static lte_rrc_stopped_indi_s lte_stopped_ind;

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCrpStoppedConnRelTest);

void LteRrcCrpStoppedConnRelTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CRP_SM,NULL,0);
  lte_rrc_crp_test_stub_init();
}

void LteRrcCrpStoppedConnRelTest::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  lte_rrc_conn_rel_started_indi_s *conn_rel_started_ind_ptr;

  TF_MSG("Start of test");

  TF_MSG("Checking for INACTIVE state");
  TF_ASSERT(stm_get_state(LTE_RRC_CRP_SM) == LTE_RRC_CRP_SM__INACTIVE);

  lte_rrc_crp_test_stub_controller_set_state(LTE_RRC_CONTROLLER_SM__CONNECTED);

  TF_MSG("Sending Conn Release DLM");
  lte_rrc_crp_test_construct_conn_rel_msg(&conn_rel_dlm,FALSE,FALSE,FALSE);
  lte_rrc_utf_send_msg((byte*)&conn_rel_dlm, sizeof(conn_rel_dlm));

  TF_MSG("Checking for LTE_RRC_CONN_REL_STARTED_INDI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_rel_started_indi_s) == buf_len);
  conn_rel_started_ind_ptr = (lte_rrc_conn_rel_started_indi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_CONN_REL_STARTED_INDI == 
            conn_rel_started_ind_ptr->msg_hdr.id);

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for WAIT_FOR_TIMER state");
  TF_ASSERT(stm_get_state(LTE_RRC_CRP_SM) == 
            LTE_RRC_CRP_SM__WAIT_FOR_TIMER);

  TF_MSG("Sending LTE Stopped ind");
  lte_rrc_init_default_hdr(&lte_stopped_ind.msg_hdr,LTE_RRC_STOPPED_INDI);
  lte_rrc_utf_send_msg((byte*)&lte_stopped_ind, sizeof(lte_stopped_ind));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INACTIVE state");
  TF_ASSERT(stm_get_state(LTE_RRC_CRP_SM) == LTE_RRC_CRP_SM__INACTIVE);

  TF_MSG("End of test");
}

void LteRrcCrpStoppedConnRelTest::Teardown()
{
  lte_rrc_utf_teardown();
}

