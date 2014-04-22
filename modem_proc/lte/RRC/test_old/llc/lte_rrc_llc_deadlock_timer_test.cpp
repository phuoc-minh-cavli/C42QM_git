/*!
  @file lte_rrc_llc_deadlock_timer_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a deadlock timer expiry handling while processing a Reconfig message.

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

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/08   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "stm2.h"
#include "__lte_rrc_llc.h"
#include "lte_rrc_sib.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h> 

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

extern "C"
{
  TF_PROTO_STUB(void, lte_rrc_llc_deadlock_tmri_ef, void);
}

/*===========================================================================

                                FUNCTIONS

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_deadlock_tmri_ef

===========================================================================*/
/*!
    @brief
    This function simulates the LLC API that calls error fatal.

    @return
    void
*/
/*=========================================================================*/
void lte_rrc_llc_test_deadlock_tmri_ef
(
  void
)
{
  TF_MSG("\nDeadlock timer expired and Handler called successfully\n");
} /* lte_rrc_llc_test_deadlock_tmri_ef() */

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcDeadlockTimerTest);

void LteRrcLlcDeadlockTimerTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init(); 
  TF_STUB(lte_rrc_llc_deadlock_tmri_ef) = 
	lte_rrc_llc_test_deadlock_tmri_ef;
}

void LteRrcLlcDeadlockTimerTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_dedicated_cfg_req_s *cphy_cfg_req_ptr = NULL;
  uint32 buf_len = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nDeadlock Timer Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_msg);
  encoded_pdu.value = llc_reconfig_msg;
  
  /* On receiving a Config Req from Config, LLC should be in the INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);
  TF_MSG("Deadlock timer expires");
  TF_SLEEP(LTE_RRC_LLC_DEADLOCK_TMRI_VAL);
  /* Deadlock timer expiry indication would call the stub function for 
    lte_rrc_llc_deadlock_tmri_handler */

  TF_MSG("\nDeadlock Timer Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcDeadlockTimerTest::Teardown()
{
  lte_rrc_utf_teardown();
}

