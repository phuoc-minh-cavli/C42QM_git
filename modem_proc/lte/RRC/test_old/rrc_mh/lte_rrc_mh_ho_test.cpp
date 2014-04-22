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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/rrc_mh/lte_rrc_mh_ho_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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
#include "lte_rrc_mh_asn1_test_util.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "tf_stub.h"
#include "lte_rrc_mh_test_common.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

DECLARE_UL_DCCH_UL_INFO_TX;
DECLARE_UL_DCCH_RRC_RECONFIG_COMP;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

void lte_rrc_mh_test_pdcpul_sdu_req(int mu_id)
{
  uint32 buf_len;
  lte_pdcpul_sdu_req_msg_s *pdcpul_sdu_req_ptr;

  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_sdu_req_ptr, &buf_len);
  TF_ASSERT(pdcpul_sdu_req_ptr->rb_cfg_idx == LTE_MAX_RB + 1);
  TF_ASSERT(pdcpul_sdu_req_ptr->mu_id == mu_id);
}

void lte_rrc_mh_test_pdcpdl_sdu_ind()
{
  lte_pdcpdl_sdu_ind_msg_s dl_msg_ind;
  
  dsm_item_type *dsm_ptr=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr;           /* Message attachment pointer */
  unsigned char *encoded_msg;
  long length;
  lte_rrc_mh_dlm_s *dlm_ptr;
  uint32 buf_len;

  // ----------------------------------------------------------------------

  msgr_init_hdr_attach(&dl_msg_ind.hdr, MSGR_LTE_PDCPDL, LTE_PDCPDL_SDU_IND, 0, 1);

  length = lte_rrc_mh_test_get_CDMA2000_CSFBParametersResponse(&encoded_msg);

  TF_ASSERT(dsm_pushdown(&dsm_ptr, encoded_msg, (uint16)length, DSM_DS_SMALL_ITEM_POOL) == (uint16)length);
  TF_ASSERT(dsm_ptr != NULL);

  att_ptr = msgr_get_attach(&dl_msg_ind.hdr, 0);
  msgr_set_dsm_attach(att_ptr, dsm_ptr);

  dl_msg_ind.rb_cfg_idx = 33;

  lte_rrc_utf_send_msg((byte*)&dl_msg_ind, sizeof(dl_msg_ind));
  lte_rrc_utf_wait_for_done();
  lte_rrc_osys_asn1_free_buf(encoded_msg);
  lte_rrc_utf_get_next_int_msg((byte **)&dlm_ptr, &buf_len);
  TF_ASSERT(dlm_ptr->msg_hdr.id == LTE_RRC_CSFB_PARAMETERS_RESPONSE_CDMA2000_DLM);
  TF_ASSERT(dlm_ptr->dl_msg_ptr != NULL);
  TF_ASSERT(dlm_ptr->pdu_num == lte_rrc_osys_DL_DCCH_Message_PDU);

}

void lte_rrc_mh_test_send_ho_started()
{
  lte_rrc_handover_started_indi_s ho_started_indi;

  lte_rrc_init_default_hdr(&ho_started_indi.msg_hdr, LTE_RRC_HANDOVER_STARTED_INDI);

  lte_rrc_utf_send_msg((byte*)&ho_started_indi, sizeof(ho_started_indi));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__HANDOVER);
}

void lte_rrc_mh_test_send_ho_completed()
{
  lte_rrc_handover_completed_ind_s ho_completed_indi;

  lte_rrc_init_default_hdr(&ho_completed_indi.msg_hdr, LTE_RRC_HANDOVER_COMPLETED_IND);

  lte_rrc_utf_send_msg((byte*)&ho_completed_indi, sizeof(ho_completed_indi));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);
}

void lte_rrc_mh_test_send_conn_released_indi()
{
  lte_rrc_conn_released_indi_s conn_released_indi;

  lte_rrc_init_default_hdr(&conn_released_indi.msg_hdr, LTE_RRC_CONN_RELEASED_INDI);

  lte_rrc_utf_send_msg((byte*)&conn_released_indi, sizeof(conn_released_indi));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);
}

void lte_rrc_mh_test_send_cre_started_indi()
{
  lte_rrc_cre_started_indi_s cre_started_indi;

  lte_rrc_init_default_hdr(&cre_started_indi.msg_hdr, LTE_RRC_CRE_STARTED_INDI);

  lte_rrc_utf_send_msg((byte*)&cre_started_indi, sizeof(cre_started_indi));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__HANDOVER);
}

void lte_rrc_mh_test_send_cre_completed_indi()
{
  lte_rrc_cre_completed_indi_s cre_completed_indi;

  lte_rrc_init_default_hdr(&cre_completed_indi.msg_hdr, LTE_RRC_CRE_COMPLETED_INDI);

  lte_rrc_utf_send_msg((byte*)&cre_completed_indi, sizeof(cre_completed_indi));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);
}

void lte_rrc_mh_test_send_stopped_indi()
{
  lte_rrc_stopped_indi_s stopped_indi;

  lte_rrc_init_default_hdr(&stopped_indi.msg_hdr, LTE_RRC_STOPPED_INDI);

  lte_rrc_utf_send_msg((byte*)&stopped_indi, sizeof(stopped_indi));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);
}

void lte_rrc_mh_test_send_pdcpul_sdu_cnf(lte_rrc_ul_msg_status_e status, int mu_id)
{
  lte_pdcpul_sdu_cnf_msg_s sdu_cnf;
  msgr_init_hdr(&sdu_cnf.hdr, MSGR_LTE_PDCPUL , LTE_PDCPUL_SDU_CNF);
  
  switch(status)
  {
    case LTE_RRC_UL_MSG_ACK:
      sdu_cnf.num_ack_muid = 1;
      sdu_cnf.num_nack_muid = 0;
      sdu_cnf.num_maybe_muid = 0;
      sdu_cnf.ack_muid[0] = mu_id;
      break;
    case LTE_RRC_UL_MSG_NACK:
      sdu_cnf.num_ack_muid = 0;
      sdu_cnf.num_nack_muid = 1;
      sdu_cnf.num_maybe_muid = 0;
      sdu_cnf.nack_muid[0] = mu_id;
      break;
    case LTE_RRC_UL_MSG_MAYBE:
      sdu_cnf.num_ack_muid = 0;
      sdu_cnf.num_nack_muid = 0;
      sdu_cnf.num_maybe_muid = 1;
      sdu_cnf.maybe_muid[0] = mu_id;
      break;
    default:
      TF_ASSERT(0);
      break;
  }

  lte_rrc_utf_send_msg((byte*)&sdu_cnf, sizeof(sdu_cnf));
  lte_rrc_utf_wait_for_done();
}

void lte_rrc_mh_test_dequeue_ul_info_tx_cnf(lte_rrc_ul_msg_status_e status, lte_rrc_ul_msg_fail_cause_e fail_cause, int mu_id)
{
  lte_rrc_ul_msg_cnfi_s *ul_msg_cnfi_ptr;
  uint32 buf_len;

  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_cnfi_ptr, &buf_len);
  TF_ASSERT(ul_msg_cnfi_ptr->msg_hdr.id == LTE_RRC_UL_INFORMATION_TRANSFER_CNFI);
  TF_ASSERT(ul_msg_cnfi_ptr->status == status);
  TF_ASSERT(ul_msg_cnfi_ptr->fail_cause == fail_cause);
  TF_ASSERT(ul_msg_cnfi_ptr->mu_id == mu_id);
}
/*===========================================================================

                                FUNCTIONS

===========================================================================*/

extern "C"
{
  TF_PROTO_STUB(stm_state_t, lte_rrc_controller_get_state, void);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_llc_get_target_cell_info, lte_rrc_cell_id_s *);
  extern lte_rrc_mh_s lte_rrc_mh;
}

extern stm_state_t lte_rrc_mh_test_stub_controller_get_state();
extern lte_errno_e lte_rrc_mh_test_stub_csp_get_serving_cell_params(lte_rrc_cell_info_s *);

lte_errno_e lte_rrc_mh_test_stub_llc_get_target_cell_info(lte_rrc_cell_id_s *cell_id_ptr)
{
  return E_SUCCESS;
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMHHOTest1);

void LteRrcMHHOTest1::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_PDCPUL_SDU_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MH_SM, umid_list, 1);

  TF_STUB(lte_rrc_controller_get_state) = 
    lte_rrc_mh_test_stub_controller_get_state;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) = 
    lte_rrc_mh_test_stub_csp_get_serving_cell_params;
  TF_STUB(lte_rrc_llc_get_target_cell_info) = 
    lte_rrc_mh_test_stub_llc_get_target_cell_info;
}

void LteRrcMHHOTest1::Test()
{
  uint32 i;
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  lte_rrc_mh_test_send_ul_dcch_msg(TRUE, 0, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(TRUE, 1, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(TRUE, 2, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(FALSE, 3, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(FALSE, 4, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(FALSE, 5, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(TRUE, 6, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(TRUE, 7, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(TRUE, 8, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(FALSE, 9, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(FALSE, 10, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(FALSE, 11, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(TRUE, 12, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(TRUE, 13, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(TRUE, 14, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(FALSE, 15, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(FALSE, 16, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_send_ul_dcch_msg(FALSE, 17, ul_dcch_ul_info_tx, 3);

  for(i = 0; i <= 17; i++)
  {
    lte_rrc_mh_test_pdcpul_sdu_req(i);
  }

  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_ACK, 0);
  lte_rrc_mh_test_dequeue_ul_info_tx_cnf(LTE_RRC_UL_MSG_ACK, LTE_RRC_UL_MSG_FAIL_CAUSE_MAX, 0);

  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_NACK, 1);
  lte_rrc_mh_test_dequeue_ul_info_tx_cnf(LTE_RRC_UL_MSG_NACK, LTE_RRC_UL_MSG_FAIL_CAUSE_MAX_TX, 1);

  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_MAYBE, 2);
  lte_rrc_mh_test_dequeue_ul_info_tx_cnf(LTE_RRC_UL_MSG_MAYBE, LTE_RRC_UL_MSG_FAIL_CAUSE_MAX_TX, 2);

  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_ACK, 3);
  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_NACK, 4);
  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_MAYBE, 5);

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  lte_rrc_mh_test_send_ho_started();

  lte_rrc_mh_test_pdcpdl_sdu_ind();

  lte_rrc_mh_test_send_ul_dcch_msg(TRUE, 18, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_dequeue_ul_info_tx_cnf(LTE_RRC_UL_MSG_NACK, LTE_RRC_UL_MSG_FAIL_CAUSE_HO, 18);

  lte_rrc_mh_test_send_ul_dcch_msg(FALSE, 19, ul_dcch_ul_info_tx, 3);

  lte_rrc_mh_test_send_ul_dcch_msg(FALSE, 20, ul_dcch_rrc_reconfig_comp, 2);
  lte_rrc_mh_test_pdcpul_sdu_req(20);

  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_ACK, 6);
  lte_rrc_mh_test_dequeue_ul_info_tx_cnf(LTE_RRC_UL_MSG_ACK, LTE_RRC_UL_MSG_FAIL_CAUSE_MAX, 6);

  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_NACK, 7);
  lte_rrc_mh_test_dequeue_ul_info_tx_cnf(LTE_RRC_UL_MSG_NACK, LTE_RRC_UL_MSG_FAIL_CAUSE_HO, 7);

  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_MAYBE, 8);
  lte_rrc_mh_test_dequeue_ul_info_tx_cnf(LTE_RRC_UL_MSG_MAYBE, LTE_RRC_UL_MSG_FAIL_CAUSE_HO, 8);

  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_ACK, 9);
  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_NACK, 10);
  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_MAYBE, 11);

  lte_rrc_mh_test_send_ho_completed();

  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_ACK, 12);
  lte_rrc_mh_test_dequeue_ul_info_tx_cnf(LTE_RRC_UL_MSG_ACK, LTE_RRC_UL_MSG_FAIL_CAUSE_MAX, 12);

  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_NACK, 13);
  lte_rrc_mh_test_dequeue_ul_info_tx_cnf(LTE_RRC_UL_MSG_NACK, LTE_RRC_UL_MSG_FAIL_CAUSE_MAX_TX, 13);

  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_MAYBE, 14);
  lte_rrc_mh_test_dequeue_ul_info_tx_cnf(LTE_RRC_UL_MSG_MAYBE, LTE_RRC_UL_MSG_FAIL_CAUSE_MAX_TX, 14);

  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_ACK, 15);
  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_NACK, 16);
  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_MAYBE, 17);

  lte_rrc_utf_reset();
}

void LteRrcMHHOTest1::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMHHOTest2);

void LteRrcMHHOTest2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_PDCPUL_SDU_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MH_SM, umid_list, 1);

  TF_STUB(lte_rrc_controller_get_state) = 
    lte_rrc_mh_test_stub_controller_get_state;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) = 
    lte_rrc_mh_test_stub_csp_get_serving_cell_params;
}

void LteRrcMHHOTest2::Test()
{
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  /* HO successful then conn release */
  lte_rrc_mh_test_send_ho_started();

  lte_rrc_mh_test_send_ul_dcch_msg(TRUE, 0, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_dequeue_ul_info_tx_cnf(LTE_RRC_UL_MSG_NACK, LTE_RRC_UL_MSG_FAIL_CAUSE_HO, 0);

  lte_rrc_mh_test_send_ho_completed();

  lte_rrc_mh_test_send_conn_released_indi();

  /* HO started but conn released before HO complete */
  lte_rrc_mh_test_send_ho_started();

  lte_rrc_mh_test_send_conn_released_indi();

  /* HO successful then RLF and successful reestablishment */
  lte_rrc_mh_test_send_ho_started();

  lte_rrc_mh_test_send_ho_completed();

  lte_rrc_mh_test_send_cre_started_indi();

  lte_rrc_mh_test_send_ul_dcch_msg(TRUE, 1, ul_dcch_ul_info_tx, 3);
  lte_rrc_mh_test_dequeue_ul_info_tx_cnf(LTE_RRC_UL_MSG_NACK, LTE_RRC_UL_MSG_FAIL_CAUSE_RLF, 1);

  lte_rrc_mh_test_send_cre_completed_indi();

  /* HO successful then RLF and reestablishment not successful so conn released */
  lte_rrc_mh_test_send_ul_dcch_msg(TRUE, 2, ul_dcch_ul_info_tx, 3);

  lte_rrc_mh_test_send_ho_started();

  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_NACK, 2);
  lte_rrc_mh_test_dequeue_ul_info_tx_cnf(LTE_RRC_UL_MSG_NACK, LTE_RRC_UL_MSG_FAIL_CAUSE_HO, 2);

  lte_rrc_mh_test_send_ho_completed();

  lte_rrc_mh_test_send_cre_started_indi();

  lte_rrc_mh_test_send_conn_released_indi();

  /* HO started but not successful, RLF triggered and successful reestablishment */
  lte_rrc_mh_test_send_ul_dcch_msg(TRUE, 3, ul_dcch_ul_info_tx, 3);

  lte_rrc_mh_test_send_ho_started();

  lte_rrc_mh_test_send_cre_started_indi();

  lte_rrc_mh_test_send_pdcpul_sdu_cnf(LTE_RRC_UL_MSG_NACK, 3);
  lte_rrc_mh_test_dequeue_ul_info_tx_cnf(LTE_RRC_UL_MSG_NACK, LTE_RRC_UL_MSG_FAIL_CAUSE_RLF, 3);

  lte_rrc_mh_test_send_cre_completed_indi();

  /* HO started but not successful, RLF triggered and reestablishment not successful so conn released */
  lte_rrc_mh_test_send_ho_started();

  lte_rrc_mh_test_send_cre_started_indi();

  lte_rrc_mh_test_send_conn_released_indi();

  /* HO started but stopped_indi received before HO completion */
  lte_rrc_mh_test_send_ho_started();

  lte_rrc_mh_test_send_stopped_indi();

  /************************************/
  //lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset();
}

void LteRrcMHHOTest2::Teardown()
{
  lte_rrc_utf_teardown();
}
