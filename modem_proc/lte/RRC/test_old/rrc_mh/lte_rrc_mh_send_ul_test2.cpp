/*!
  @file
  lte_rrc_mh_test.cpp

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/rrc_mh/lte_rrc_mh_send_ul_test2.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/08   amit    Removed the resend field from ul_msg_req
09/15/08   amit    Freeing memory allocated to mu_id and encode_status
09/15/08   amit    Initial Version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "lte_rrc_mh_asn1_test_util.h"
#include "__lte_rrc_mh.h"
#include "__lte_rrc_controller.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_pdcp_msg.h"
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


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/



extern "C"
{
//TF_DECLARE_STUB( stm_state_t, lte_rrc_controller_get_state, void);
TF_PROTO_STUB( stm_state_t, lte_rrc_controller_get_state, void);
}

extern stm_state_t lte_rrc_mh_test_stub_controller_get_state();


/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(NO_RUN, LteRrcMHSendUlTest2);

void LteRrcMHSendUlTest2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_PDCPUL_SDU_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MH_SM, umid_list, 1);

  TF_STUB(lte_rrc_controller_get_state) = 
    lte_rrc_mh_test_stub_controller_get_state;
}

void LteRrcMHSendUlTest2::Test()
{
  lte_rrc_send_ul_msg_reqi_s ul_msg_reqi;
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  // ----------------------------------------------------------------------

  lte_rrc_init_default_hdr(&ul_msg_reqi.msg_hdr, LTE_RRC_SEND_UL_MSG_REQI);

  ul_msg_reqi.ack_needed = FALSE;
  ul_msg_reqi.rb_id = 1;
  ul_msg_reqi.mu_id = (uint8 *)lte_rrc_mem_calloc(sizeof(uint8));
  ul_msg_reqi.encode_status = (uint8 *)lte_rrc_mem_calloc(sizeof(uint8));
  *(ul_msg_reqi.encode_status) = 0;

  /* Allocate memory to and create a decoded message. The function is in .c 
     file */
  ul_msg_reqi.ul_msg_ptr = lte_rrc_mh_test_get_ul_msg_enc_fail();

  lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);
  
  /* Encoding should be successful */
  TF_ASSERT(*(ul_msg_reqi.encode_status) != 0);
  /* For the first message, mu_id should be 0 */
  //TF_ASSERT(*(ul_msg_reqi.mu_id) == 0);

  lte_rrc_mem_free(ul_msg_reqi.mu_id);
  lte_rrc_mem_free(ul_msg_reqi.encode_status);
  
  lte_rrc_utf_reset();
}


void LteRrcMHSendUlTest2::Teardown()
{
  lte_rrc_utf_teardown();
}
