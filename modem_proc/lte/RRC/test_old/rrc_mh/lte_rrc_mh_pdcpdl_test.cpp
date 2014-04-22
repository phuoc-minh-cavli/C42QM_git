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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/rrc_mh/lte_rrc_mh_pdcpdl_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_mh_asn1_test_util.h"
#include "__lte_rrc_mh.h"
#include "lte_rrc_mh.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_pdcp_msg.h"
#include "dsm_item.h"
#include "dsm_init.h"
#include "lte_rrc_llc.h"
#include "lte_rrc_mhi.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "tf_stub.h"
#include "lte_rrc_mh_test_common.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

DECLARE_RRC_CONN_RECONFIG_8_5;
DECLARE_SECURITY_IS_CONFIGURED;
DECLARE_SEC_MODE_COMMAND;
DECLARE_SEC_MODE_COMPLETE;

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
  TF_PROTO_STUB(boolean, lte_rrc_sec_security_is_configured);
  extern lte_rrc_mh_s lte_rrc_mh;
}

extern stm_state_t lte_rrc_mh_test_stub_controller_get_state();

extern lte_errno_e lte_rrc_mh_test_stub_csp_get_serving_cell_params(lte_rrc_cell_info_s *);

static boolean lte_rrc_mh_test_stub_sec_security_is_configured()
{
  return security_is_configured;
}

void lte_rrc_mh_test_send_sec_mode_command()
{
  lte_pdcpdl_sdu_ind_msg_s dl_msg_ind;
  dsm_item_type *dsm_ptr=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr;           /* Message attachment pointer */
  lte_rrc_mh_dlm_s *dlm_ptr;
  uint32 buf_len;

  msgr_init_hdr_attach(&dl_msg_ind.hdr, MSGR_LTE_PDCPDL, LTE_PDCPDL_SDU_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr, sec_mode_command, sizeof(sec_mode_command), DSM_DS_SMALL_ITEM_POOL) == sizeof(sec_mode_command));
  TF_ASSERT(dsm_ptr != NULL);

  att_ptr = msgr_get_attach(&dl_msg_ind.hdr, 0);
  msgr_set_dsm_attach(att_ptr, dsm_ptr);

  dl_msg_ind.rb_cfg_idx = 33;
  dl_msg_ind.count = 123;
  dl_msg_ind.integrity_status = LTE_PDCPDL_INTEGRITY_VERIFY_NA;
  dl_msg_ind.mac_i[0] = 12;
  dl_msg_ind.mac_i[1] = 13;
  dl_msg_ind.mac_i[2] = 14;
  dl_msg_ind.mac_i[3] = 15;

  lte_rrc_utf_send_msg((byte*)&dl_msg_ind, sizeof(dl_msg_ind));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte **)&dlm_ptr, &buf_len);
  TF_ASSERT(dlm_ptr->msg_hdr.id == LTE_RRC_SECURITY_MODE_COMMAND_DLM);
  TF_ASSERT(dlm_ptr->dl_msg_ptr != NULL);
  TF_ASSERT(dlm_ptr->pdu_num == lte_rrc_osys_DL_DCCH_Message_PDU);
  TF_ASSERT(dlm_ptr->count == 123);
  TF_ASSERT(dlm_ptr->mac_i[0] == 12);
  TF_ASSERT(dlm_ptr->mac_i[1] == 13);
  TF_ASSERT(dlm_ptr->mac_i[2] == 14);
  TF_ASSERT(dlm_ptr->mac_i[3] == 15);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMHPdcpdlTest);

void LteRrcMHPdcpdlTest::Setup()
{

  lte_rrc_utf_setup(LTE_RRC_MH_SM, NULL, 0);

  TF_STUB(lte_rrc_controller_get_state) = 
    lte_rrc_mh_test_stub_controller_get_state;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) = 
    lte_rrc_mh_test_stub_csp_get_serving_cell_params;
}

void LteRrcMHPdcpdlTest::Test()
{
  lte_pdcpdl_sdu_ind_msg_s dl_msg_ind;
  
  uint8 int_queue_count;
  uint8 ext_queue_count;
  dsm_item_type *dsm_ptr=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr;           /* Message attachment pointer */
  unsigned char *encoded_msg;
  long length;
  lte_rrc_mh_dlm_s *dlm_ptr;
  uint32 buf_len;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

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
  (void)lte_rrc_osys_asn1_free_buf(encoded_msg);
  lte_rrc_utf_get_next_int_msg((byte **)&dlm_ptr, &buf_len);
  TF_ASSERT(dlm_ptr->msg_hdr.id == LTE_RRC_CSFB_PARAMETERS_RESPONSE_CDMA2000_DLM);
  TF_ASSERT(dlm_ptr->dl_msg_ptr != NULL);
  TF_ASSERT(dlm_ptr->pdu_num == lte_rrc_osys_DL_DCCH_Message_PDU);

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);

  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  
  lte_rrc_utf_reset();
}


void LteRrcMHPdcpdlTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMHPdcpdlTest2);

void LteRrcMHPdcpdlTest2::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_MH_SM, NULL, 0);

  TF_STUB(lte_rrc_controller_get_state) = 
    lte_rrc_mh_test_stub_controller_get_state;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) = 
    lte_rrc_mh_test_stub_csp_get_serving_cell_params;
}

void LteRrcMHPdcpdlTest2::Test()
{
  lte_pdcpdl_sdu_ind_msg_s dl_msg_ind;
  
  dsm_item_type *dsm_ptr=NULL;                /* DSM pointer */
  msgr_attach_struct_type *att_ptr;           /* Message attachment pointer */
  lte_rrc_mh_dlm_s *dlm_ptr;
  uint32 buf_len;
  lte_rrc_dlm_processed_indi_s ind;
  lte_rrc_pdu_buf_s pdu_buf;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  // ----------------------------------------------------------------------

  msgr_init_hdr_attach(&dl_msg_ind.hdr, MSGR_LTE_PDCPDL, LTE_PDCPDL_SDU_IND, 0, 1);

  TF_ASSERT(dsm_pushdown(&dsm_ptr, rrc_conn_reconfig_8_5, (uint16)sizeof(rrc_conn_reconfig_8_5), DSM_DS_SMALL_ITEM_POOL) == (uint16)sizeof(rrc_conn_reconfig_8_5));

  TF_ASSERT(dsm_ptr != NULL);

  att_ptr = msgr_get_attach(&dl_msg_ind.hdr, 0);
  msgr_set_dsm_attach(att_ptr, dsm_ptr);

  dl_msg_ind.rb_cfg_idx = 33;


  lte_rrc_utf_send_msg((byte*)&dl_msg_ind, sizeof(dl_msg_ind));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte **)&dlm_ptr, &buf_len);
  TF_ASSERT(dlm_ptr->msg_hdr.id == LTE_RRC_RRC_CONNECTION_RECONFIGURATION_DLM);
  TF_ASSERT(dlm_ptr->dl_msg_ptr != NULL);
  TF_ASSERT(dlm_ptr->pdu_num == lte_rrc_osys_DL_DCCH_Message_PDU);
  
  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  lte_rrc_mh_get_last_encoded_message_rcvd(&pdu_buf);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  /* Heap memory used must be non-zero since last dlm was longer than
     LTE_RRC_MH_MAX_DL_ASN1_ENCODED_LENGTH */
  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == pdu_buf.length);
#endif

  /* send DLM_PROCESSED_INDI */
  lte_rrc_init_default_hdr(&ind.msg_hdr,LTE_RRC_DLM_PROCESSED_INDI);
  lte_rrc_utf_send_msg((byte*)&ind, sizeof(ind));
  lte_rrc_utf_wait_for_done();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  /* last_dl_msg_ptr should be freed on receiving dlm_processed_ind and heap
     memory used must be 0*/
  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == 0);
#endif

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset();
}


void LteRrcMHPdcpdlTest2::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMHPdcpdlPendQueueTest);

void LteRrcMHPdcpdlPendQueueTest::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_PDCPUL_SDU_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MH_SM, umid_list, 1);

  TF_STUB(lte_rrc_controller_get_state) = 
    lte_rrc_mh_test_stub_controller_get_state;
  TF_STUB(lte_rrc_sec_security_is_configured) = 
    lte_rrc_mh_test_stub_sec_security_is_configured;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) = 
    lte_rrc_mh_test_stub_csp_get_serving_cell_params;

}

void LteRrcMHPdcpdlPendQueueTest::Test()
{
  uint32 buf_len;
  lte_pdcpul_sdu_req_msg_s *pdcpul_sdu_req_ptr;
  lte_rrc_dlm_processed_indi_s ind;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  TF_ASSERT(lte_rrc_mh.dd_ptr->dlm_processing_active == FALSE);

  lte_rrc_mh_test_send_sec_mode_command();

  TF_ASSERT(lte_rrc_mh.dd_ptr->dlm_processing_active == TRUE);

  security_is_configured = TRUE;

  lte_rrc_mh_test_send_ul_dcch_msg(FALSE, 1, sec_mode_complete, sizeof(sec_mode_complete));
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_sdu_req_ptr, &buf_len);
  TF_ASSERT(pdcpul_sdu_req_ptr->rb_cfg_idx == LTE_MAX_RB + 1);
  TF_ASSERT(pdcpul_sdu_req_ptr->cipher == FALSE);
  TF_ASSERT(pdcpul_sdu_req_ptr->integrity_protect == TRUE);

  /* send DLM_PROCESSED_INDI */
  lte_rrc_init_default_hdr(&ind.msg_hdr,LTE_RRC_DLM_PROCESSED_INDI);
  lte_rrc_utf_send_msg((byte*)&ind, sizeof(ind));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_mh.dd_ptr->dlm_processing_active == FALSE);

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset();
}


void LteRrcMHPdcpdlPendQueueTest::Teardown()
{
  lte_rrc_utf_teardown();
}
