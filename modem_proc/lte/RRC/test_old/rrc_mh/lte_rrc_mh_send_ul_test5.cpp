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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/rrc_mh/lte_rrc_mh_send_ul_test5.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/08   amit    Removed the resend field from ul_msg_req
11/10/08   amit    Dequeueing all messages sent to avoid overflow
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
  TF_PROTO_STUB(stm_state_t, lte_rrc_controller_get_state, void);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);
  extern lte_rrc_mh_s lte_rrc_mh;
}

extern stm_state_t lte_rrc_mh_test_stub_controller_get_state();
extern lte_errno_e lte_rrc_mh_test_stub_csp_get_serving_cell_params(lte_rrc_cell_info_s *);


/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMHSendUlTest5);

void LteRrcMHSendUlTest5::Setup()
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

void LteRrcMHSendUlTest5::Test()
{
  lte_rrc_send_ul_msg_reqi_s ul_msg_reqi;
  uint32 buf_len;
  lte_pdcpul_sdu_req_msg_s *pdcpul_sdu_req_ptr;
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

  for(int i = 0; i < LTE_RRC_MH_MAX_BOOK_KEEPING_MSG; i++)
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
    lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_sdu_req_ptr, &buf_len);
  }

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  //pdcpul_sdu_cnf
  lte_pdcpul_sdu_cnf_msg_s sdu_cnf;
  msgr_init_hdr(&sdu_cnf.hdr, MSGR_LTE_PDCPUL , LTE_PDCPUL_SDU_CNF);
  sdu_cnf.num_ack_muid = 2;
  sdu_cnf.num_nack_muid = 0;
  sdu_cnf.num_maybe_muid = 0;
  sdu_cnf.ack_muid[0] = 2;
  sdu_cnf.ack_muid[1] = 31;
  lte_rrc_utf_send_msg((byte*)&sdu_cnf, sizeof(sdu_cnf));
  lte_rrc_utf_wait_for_done();

  //send_ul_msg
  ul_msg_reqi.ul_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(enc_ptr,
                               4,
                               UL_DCCH_Message,
                               &decode_status);
  lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
  lte_rrc_utf_wait_for_done();
  /* Encoding should be successful */
  TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
  /* For the first message, mu_id should be 0 */
  TF_ASSERT(*(ul_msg_reqi.mu_id) == 32);
  TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[2].mu_id == 32);

  //pdcpul_sdu_cnf
  msgr_init_hdr(&sdu_cnf.hdr, MSGR_LTE_PDCPUL , LTE_PDCPUL_SDU_CNF);
  sdu_cnf.num_ack_muid = 3;
  sdu_cnf.num_nack_muid = 0;
  sdu_cnf.num_maybe_muid = 0;
  sdu_cnf.ack_muid[0] = 0;
  sdu_cnf.ack_muid[1] = 1;
  sdu_cnf.ack_muid[2] = 4;
  lte_rrc_utf_send_msg((byte*)&sdu_cnf, sizeof(sdu_cnf));
  lte_rrc_utf_wait_for_done();

  //send_ul_msg
  ul_msg_reqi.ul_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(enc_ptr,
                               4,
                               UL_DCCH_Message,
                               &decode_status);
  lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
  lte_rrc_utf_wait_for_done();
  /* Encoding should be successful */
  TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
  /* For the first message, mu_id should be 0 */
  TF_ASSERT(*(ul_msg_reqi.mu_id) == 33);
  TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[0].mu_id == 33);

  ul_msg_reqi.ul_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(enc_ptr,
                               4,
                               UL_DCCH_Message,
                               &decode_status);
  lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
  lte_rrc_utf_wait_for_done();
  /* Encoding should be successful */
  TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
  /* For the first message, mu_id should be 0 */
  TF_ASSERT(*(ul_msg_reqi.mu_id) == 34);
  TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[1].mu_id == 34);

  ul_msg_reqi.ul_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(enc_ptr,
                               4,
                               UL_DCCH_Message,
                               &decode_status);
  lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
  lte_rrc_utf_wait_for_done();
  /* Encoding should be successful */
  TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
  /* For the first message, mu_id should be 0 */
  TF_ASSERT(*(ul_msg_reqi.mu_id) == 35);
  TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[4].mu_id == 35);

  ul_msg_reqi.ul_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(enc_ptr,
                               4,
                               UL_DCCH_Message,
                               &decode_status);
  lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
  lte_rrc_utf_wait_for_done();
  /* Encoding should be successful */
  TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
  /* For the first message, mu_id should be 0 */
  TF_ASSERT(*(ul_msg_reqi.mu_id) == 36);
  TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[31].mu_id == 36);

  lte_rrc_mem_free(ul_msg_reqi.mu_id);
  lte_rrc_mem_free(ul_msg_reqi.encode_status);
  
  lte_rrc_utf_reset();
}


void LteRrcMHSendUlTest5::Teardown()
{
  lte_rrc_utf_teardown();
}



/* Test to check that in case of overflow of bk array, messages for which ack is 
   not needed and are not part of history buffer are removed. */
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMHSendUlTest6);

void LteRrcMHSendUlTest6::Setup()
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

void LteRrcMHSendUlTest6::Test()
{
  lte_rrc_send_ul_msg_reqi_s ul_msg_reqi;
  uint32 buf_len;
  uint8 mu_id = 0;
  lte_pdcpul_sdu_req_msg_s *pdcpul_sdu_req_ptr;
  int decode_status;
  lte_rrc_conn_released_indi_s conn_rel_indi;
  lte_rrc_conn_mode_failure_indi_s *config_failure_ind_msg_ptr = NULL;

  
  unsigned char enc_ptr[5] = {0x08, 0x00, 0x00, 0x20};
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  // ----------------------------------------------------------------------

  lte_rrc_init_default_hdr(&ul_msg_reqi.msg_hdr, LTE_RRC_SEND_UL_MSG_REQI);

  ul_msg_reqi.ack_needed = FALSE;
  ul_msg_reqi.rb_id = 1;
  ul_msg_reqi.mu_id = (uint8 *)lte_rrc_mem_calloc(sizeof(uint8));
  ul_msg_reqi.encode_status = (uint8 *)lte_rrc_mem_calloc(sizeof(uint8));

  ul_msg_reqi.ul_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)
  lte_rrc_osys_asn1_decode_pdu(enc_ptr,
                               4,
                               UL_DCCH_Message,
                               &decode_status);
  
  for(int i = 0; i < LTE_RRC_MH_MAX_BOOK_KEEPING_MSG; i++)
  {

    ul_msg_reqi.ul_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)
    lte_rrc_osys_asn1_decode_pdu(enc_ptr,
                                 4,
                                 UL_DCCH_Message,
                                 &decode_status);  

    lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
    lte_rrc_utf_wait_for_done();
    /* Encoding should be successful */
    TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[i % LTE_RRC_MH_MAX_BOOK_KEEPING_MSG].mu_id == mu_id);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_hist[i % LTE_RRC_MH_MAX_NUM_UL_MSG_SAVED].bk_index == (i % LTE_RRC_MH_MAX_BOOK_KEEPING_MSG));
    mu_id++;
    lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_sdu_req_ptr, &buf_len);
  }

  for(int i = LTE_RRC_MH_MAX_BOOK_KEEPING_MSG; i < 256; i++)
  {
    lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
    lte_rrc_utf_wait_for_done();
    /* Encoding should be successful */
    TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[27 + ((i - 27) % 5)].mu_id == mu_id);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_hist[i % LTE_RRC_MH_MAX_NUM_UL_MSG_SAVED].bk_index == (27 + ((i - 27) % 5)));
    mu_id++;
    lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_sdu_req_ptr, &buf_len);
  }

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  /* At this point, there are 256 unaccounted MU_IDs.. Send one more so that MH declares RLF*/
  lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
  lte_rrc_utf_wait_for_done();
  
  /* Trap connected mode failure indi*/
  lte_rrc_utf_get_next_int_msg((byte **)&config_failure_ind_msg_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_mode_failure_indi_s) == buf_len);


  /*On receiving ConnReleased indication, CEP should transition to inactive*/
  TF_MSG("Sending the ConnReleased indication");
  lte_rrc_init_default_hdr(&conn_rel_indi.msg_hdr, LTE_RRC_CONN_RELEASED_INDI);
  
  lte_rrc_utf_send_msg((byte*)&conn_rel_indi, sizeof(conn_rel_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_mh.dd_ptr->num_cnf_due == 0);

  /* Send some more messages to make sure that clean up has taken effect */
  for(int i = 0; i < LTE_RRC_MH_MAX_BOOK_KEEPING_MSG; i++)
  {

    lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
    lte_rrc_utf_wait_for_done();
    /* Encoding should be successful */
    TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[i % LTE_RRC_MH_MAX_BOOK_KEEPING_MSG].mu_id == mu_id);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_hist[i % LTE_RRC_MH_MAX_NUM_UL_MSG_SAVED].bk_index == (i % LTE_RRC_MH_MAX_BOOK_KEEPING_MSG));
    mu_id++;
    lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_sdu_req_ptr, &buf_len);
  }




  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_mem_free(ul_msg_reqi.mu_id);
  lte_rrc_mem_free(ul_msg_reqi.encode_status);
  
  lte_rrc_utf_reset();
}


void LteRrcMHSendUlTest6::Teardown()
{
  lte_rrc_utf_teardown();
}


/* Test to check that in case of overflow of bk array, messages for which ack is 
   not needed and are not part of history buffer are removed. Here some messages 
   need ack, so test is to make sure those for which ack are needed are not 
   removed from the bk array */
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMHSendUlTest7);

void LteRrcMHSendUlTest7::Setup()
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

void LteRrcMHSendUlTest7::Test()
{
  lte_rrc_send_ul_msg_reqi_s ul_msg_reqi;
  uint32 buf_len;
  uint8 mu_id = 0;
  lte_pdcpul_sdu_req_msg_s *pdcpul_sdu_req_ptr;
  int decode_status;

  
  
  unsigned char enc_ptr[5] = {0x08, 0x00, 0x00, 0x20};
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  // ----------------------------------------------------------------------

  lte_rrc_init_default_hdr(&ul_msg_reqi.msg_hdr, LTE_RRC_SEND_UL_MSG_REQI);

  ul_msg_reqi.ack_needed = FALSE;
  ul_msg_reqi.rb_id = 1;
  ul_msg_reqi.mu_id = (uint8 *)lte_rrc_mem_calloc(sizeof(uint8));
  ul_msg_reqi.encode_status = (uint8 *)lte_rrc_mem_calloc(sizeof(uint8));

  for(int i = 0; i < 25; i++)
  {
    ul_msg_reqi.ul_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(enc_ptr,
                                 4,
                                 UL_DCCH_Message,
                                 &decode_status);
    lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
    lte_rrc_utf_wait_for_done();
    /* Encoding should be successful */
    TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[i % LTE_RRC_MH_MAX_BOOK_KEEPING_MSG].mu_id == mu_id);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_hist[i % LTE_RRC_MH_MAX_NUM_UL_MSG_SAVED].bk_index == (i % LTE_RRC_MH_MAX_BOOK_KEEPING_MSG));
    mu_id++;
    lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_sdu_req_ptr, &buf_len);
  }

  ul_msg_reqi.ack_needed = TRUE;

  for(int i = 25; i < 40; i++)
  {
    ul_msg_reqi.ul_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(enc_ptr,
                                 4,
                                 UL_DCCH_Message,
                                 &decode_status);
    lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
    lte_rrc_utf_wait_for_done();
    /* Encoding should be successful */
    TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
    TF_ASSERT(*(ul_msg_reqi.mu_id) == mu_id);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[i % LTE_RRC_MH_MAX_BOOK_KEEPING_MSG].mu_id == mu_id);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_hist[i % LTE_RRC_MH_MAX_NUM_UL_MSG_SAVED].bk_index == (i % LTE_RRC_MH_MAX_BOOK_KEEPING_MSG));
    mu_id++;
    lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_sdu_req_ptr, &buf_len);
  }

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_mem_free(ul_msg_reqi.mu_id);
  lte_rrc_mem_free(ul_msg_reqi.encode_status);
  
  lte_rrc_utf_reset();
}


void LteRrcMHSendUlTest7::Teardown()
{
  lte_rrc_utf_teardown();
}



/* Test to reproduce scenario seen in lab */
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMHSendUlTest8);

void LteRrcMHSendUlTest8::Setup()
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

void LteRrcMHSendUlTest8::Test()
{
  lte_rrc_send_ul_msg_reqi_s ul_msg_reqi;
  uint32 buf_len;
  uint8 mu_id = 0;
  lte_pdcpul_sdu_req_msg_s *pdcpul_sdu_req_ptr;
  int decode_status;

  //pdcpul_sdu_cnf
  lte_pdcpul_sdu_cnf_msg_s sdu_cnf;
  
  unsigned char enc_ptr[5] = {0x08, 0x00, 0x00, 0x20};
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  // ----------------------------------------------------------------------

  lte_rrc_init_default_hdr(&ul_msg_reqi.msg_hdr, LTE_RRC_SEND_UL_MSG_REQI);

  ul_msg_reqi.ack_needed = FALSE;
  ul_msg_reqi.rb_id = 1;
  ul_msg_reqi.mu_id = (uint8 *)lte_rrc_mem_calloc(sizeof(uint8));
  ul_msg_reqi.encode_status = (uint8 *)lte_rrc_mem_calloc(sizeof(uint8));

  msgr_init_hdr(&sdu_cnf.hdr, MSGR_LTE_PDCPUL, LTE_PDCPUL_SDU_CNF);
  sdu_cnf.num_ack_muid = 1;
  sdu_cnf.num_nack_muid = 0;
  sdu_cnf.num_maybe_muid = 0;

  for(int i = 0; i < 25; i++)
  {
    ul_msg_reqi.ul_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(enc_ptr,
                                 4,
                                 UL_DCCH_Message,
                                 &decode_status);
    lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
    lte_rrc_utf_wait_for_done();
    /* Encoding should be successful */
    TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_msgs_bk[i % LTE_RRC_MH_MAX_NUM_UL_MSG_SAVED].mu_id == mu_id);
    TF_ASSERT(lte_rrc_mh.dd_ptr->ul_hist[i % LTE_RRC_MH_MAX_NUM_UL_MSG_SAVED].bk_index == (i % LTE_RRC_MH_MAX_NUM_UL_MSG_SAVED));

    lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_sdu_req_ptr, &buf_len);

    sdu_cnf.ack_muid[0] = mu_id;
    lte_rrc_utf_send_msg((byte*)&sdu_cnf, sizeof(sdu_cnf));
    lte_rrc_utf_wait_for_done();

    mu_id++;
  }

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_mem_free(ul_msg_reqi.mu_id);
  lte_rrc_mem_free(ul_msg_reqi.encode_status);
  
  lte_rrc_utf_reset();
}


void LteRrcMHSendUlTest8::Teardown()
{
  lte_rrc_utf_teardown();
}
