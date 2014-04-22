/*!
  @file lte_rrc_mh_send_ul_test4.cpp

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/rrc_mh/lte_rrc_mh_sdu_cnf_test1.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/08   amit    Removed the resend field from ul_msg_req
11/05/08   amit    Initial Version
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
#include "__lte_rrc_controller.h"
#include "lte_rrc_mhi.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_pdcp_msg.h"
#include "dsm_item.h"
#include "dsm_init.h"
#include "lte_rrc_osys_asn1util.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "tf_stub.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

extern "C"
{
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);
}




/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/
extern "C"
{
extern lte_rrc_mh_s lte_rrc_mh;
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/



extern "C"
{
  //TF_DECLARE_STUB( stm_state_t, lte_rrc_controller_get_state, void);
  TF_PROTO_STUB(stm_state_t, lte_rrc_controller_get_state, void);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);
}

extern stm_state_t lte_rrc_mh_test_stub_controller_get_state();
extern lte_errno_e lte_rrc_mh_test_stub_csp_get_serving_cell_params(lte_rrc_cell_info_s *);


/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSduCnfTest1);

void LteRrcSduCnfTest1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_PDCPUL_SDU_REQ,
      LTE_RRC_MEASUREMENT_REPORT_CNFI
    };

  lte_rrc_utf_setup(LTE_RRC_MH_SM, umid_list, 2);

  TF_STUB(lte_rrc_controller_get_state) = 
    lte_rrc_mh_test_stub_controller_get_state;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) = 
    lte_rrc_mh_test_stub_csp_get_serving_cell_params;
}

void LteRrcSduCnfTest1::Test()
{
  lte_rrc_send_ul_msg_reqi_s ul_msg_reqi;
  int decode_status;

  
  
  unsigned char enc_ptr[5] = {0x08, 0x00, 0x00, 0x20};
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  // ----------------------------------------------------------------------

  lte_rrc_init_default_hdr(&ul_msg_reqi.msg_hdr, LTE_RRC_SEND_UL_MSG_REQI);

  ul_msg_reqi.ack_needed = TRUE;
  ul_msg_reqi.rb_id = 1;
  ul_msg_reqi.mu_id = (uint8 *)lte_rrc_mem_calloc(sizeof(uint8));
  ul_msg_reqi.encode_status = (uint8 *)lte_rrc_mem_calloc(sizeof(uint8));

  /* First send 3 ul messages needing ack */
  for(int i = 0; i < 3; i++)
  {
    ul_msg_reqi.ul_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(enc_ptr,
                                 4,
                                 UL_DCCH_Message,
                                 &decode_status);
    lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
    lte_rrc_utf_wait_for_done();
    /* Encoding should be successful */
    TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
    /* For the first message, mu_id should be 0 */
    TF_ASSERT(*(ul_msg_reqi.mu_id) == i);
  }

  /* Now send 3 ul messages not needing ack */
  ul_msg_reqi.ack_needed = FALSE;
  for(int i = 3; i < 6; i++)
  {
    ul_msg_reqi.ul_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(enc_ptr,
                                 4,
                                 UL_DCCH_Message,
                                 &decode_status);
    lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
    lte_rrc_utf_wait_for_done();
    /* Encoding should be successful */
    TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
    /* For the first message, mu_id should be 0 */
    //TF_ASSERT(*(ul_msg_reqi.mu_id) == i);
  }

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  /* For the one that was sent first, ack needed - not in hist now */
  for(int k = 0; k < 1; k++)
  {
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[k].valid == TRUE);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[k].ack_needed == TRUE);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[k].cnf_umid == LTE_RRC_MEASUREMENT_REPORT_CNFI);
    //TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[k].pdu_buf.value == NULL);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[k].pdu_buf.length == 4);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[k].rb_id == 1);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[k].save_cause == LTE_RRC_MH_SAVE_CAUSE_WT_CNF);
  
    //TF_ASSERT(lte_rrc_mh.dd_ptr->ul_hist[k].bk_index == 0);
  }

  /* For the one sent second and third, ack needed for both, both in hist */
  for(int k = 1; k < 3; k++)
  {
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[k].valid == TRUE);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[k].ack_needed == TRUE);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[k].cnf_umid == LTE_RRC_MEASUREMENT_REPORT_CNFI);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[k].pdu_buf.value == lte_rrc_mh.dd_ptr->ul_hist[k % 
              LTE_RRC_MH_MAX_NUM_UL_MSG_SAVED].encoded_msg);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[k].pdu_buf.length == 4);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[k].rb_id == 1);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[k].save_cause == LTE_RRC_MH_SAVE_CAUSE_BOTH);
  
    //TF_ASSERT(lte_rrc_mh.dd_ptr->ul_hist[k].bk_index == 0);
  }

  /* For fourth, fifth, sixth: ack not needed */
  for(int j = 3; j < 6; j++)
  {
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[j].valid == TRUE);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[j].ack_needed == FALSE);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[j].cnf_umid == LTE_RRC_MEASUREMENT_REPORT_CNFI);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[j].pdu_buf.value == lte_rrc_mh.dd_ptr->ul_hist[j % 
              LTE_RRC_MH_MAX_NUM_UL_MSG_SAVED].encoded_msg);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[j].rb_id == 1);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[j].save_cause == LTE_RRC_MH_SAVE_CAUSE_BOTH);
  
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_hist[j % LTE_RRC_MH_MAX_NUM_UL_MSG_SAVED].bk_index == j);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_hist[j % LTE_RRC_MH_MAX_NUM_UL_MSG_SAVED].length == 4);
  }

  TF_ASSERT(lte_rrc_mh.dd_ptr->next_ul_hist_index == 1);

  //pdcpul_sdu_cnf
  lte_pdcpul_sdu_cnf_msg_s sdu_cnf;
  msgr_init_hdr(&sdu_cnf.hdr, MSGR_LTE_PDCPUL , LTE_PDCPUL_SDU_CNF);
  sdu_cnf.num_ack_muid = 2;
  sdu_cnf.num_nack_muid = 2;
  sdu_cnf.num_maybe_muid = 2;
  sdu_cnf.ack_muid[0] = 0;
  sdu_cnf.ack_muid[1] = 3;
  sdu_cnf.nack_muid[0] = 1;
  sdu_cnf.nack_muid[1] = 4;
  sdu_cnf.maybe_muid[0] = 2;
  sdu_cnf.maybe_muid[1] = 5;

  lte_rrc_utf_send_msg((byte*)&sdu_cnf, sizeof(sdu_cnf));
  lte_rrc_utf_wait_for_done();

  //ul_msg_cnfi
  lte_rrc_ul_msg_cnfi_s *ul_msg_cnfi_ptr;
  uint32 buf_len;
  lte_rrc_utf_get_next_ext_msg((byte **)&ul_msg_cnfi_ptr, &buf_len);
  lte_rrc_utf_get_next_ext_msg((byte **)&ul_msg_cnfi_ptr, &buf_len);
  lte_rrc_utf_get_next_ext_msg((byte **)&ul_msg_cnfi_ptr, &buf_len);
  lte_rrc_utf_get_next_ext_msg((byte **)&ul_msg_cnfi_ptr, &buf_len);
  lte_rrc_utf_get_next_ext_msg((byte **)&ul_msg_cnfi_ptr, &buf_len);
  lte_rrc_utf_get_next_ext_msg((byte **)&ul_msg_cnfi_ptr, &buf_len);
  lte_rrc_utf_get_next_ext_msg((byte **)&ul_msg_cnfi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_ul_msg_cnfi_s));
  TF_ASSERT(ul_msg_cnfi_ptr->msg_hdr.id == LTE_RRC_MEASUREMENT_REPORT_CNFI);
  TF_ASSERT(ul_msg_cnfi_ptr->mu_id == 0);
  TF_ASSERT(ul_msg_cnfi_ptr->status == LTE_RRC_UL_MSG_ACK);

  lte_rrc_utf_get_next_ext_msg((byte **)&ul_msg_cnfi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_ul_msg_cnfi_s));
  TF_ASSERT(ul_msg_cnfi_ptr->msg_hdr.id == LTE_RRC_MEASUREMENT_REPORT_CNFI);
  TF_ASSERT(ul_msg_cnfi_ptr->mu_id == 1);
  TF_ASSERT(ul_msg_cnfi_ptr->status == LTE_RRC_UL_MSG_NACK);

  lte_rrc_utf_get_next_ext_msg((byte **)&ul_msg_cnfi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_ul_msg_cnfi_s));
  TF_ASSERT(ul_msg_cnfi_ptr->msg_hdr.id == LTE_RRC_MEASUREMENT_REPORT_CNFI);
  TF_ASSERT(ul_msg_cnfi_ptr->mu_id == 2);
  TF_ASSERT(ul_msg_cnfi_ptr->status == LTE_RRC_UL_MSG_MAYBE);

  for(int i = 0; i < 6; i++)
  {
    lte_rrc_mh.dd_ptr->ul_msgs_bk[i].valid = FALSE;
  }


  lte_rrc_mem_free(ul_msg_reqi.mu_id);
  lte_rrc_mem_free(ul_msg_reqi.encode_status);

  
  lte_rrc_utf_reset();
}


void LteRrcSduCnfTest1::Teardown()
{
  lte_rrc_utf_teardown();
}
