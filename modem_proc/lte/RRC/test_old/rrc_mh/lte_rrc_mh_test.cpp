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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/rrc_mh/lte_rrc_mh_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/08   amit    Removed the resend field from ul_msg_req
10/08/08   amit    Stubbed lte_rrc_controller_get_state
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
#include "__lte_rrc_controller.h"
#include "lte_rrc_controller.h"
#include "__lte_rrc_mh.h"
#include "lte_static_rrc_mem_util.h"
#include "dsm_item.h"
#include "dsm_init.h"
#include "lte_pdcp_msg.h"
#include "lte_rrc_llc.h"
#include "lte_rrc_mhi.h"
#include "lte_static_rrc_pendq.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "tf_stub.h"
#include "lte_rrc_mh_test_common.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

extern "C"
{
}




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
  TF_PROTO_STUB(stm_state_t, lte_rrc_controller_get_state, void);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);
}

stm_state_t lte_rrc_mh_test_stub_controller_get_state()
{
  return LTE_RRC_CONTROLLER_SM__CONNECTED;
}

lte_errno_e lte_rrc_mh_test_stub_csp_get_serving_cell_params(lte_rrc_cell_info_s *cell_info_ptr)
{
  return E_SUCCESS;
}

DECLARE_RRC_CONN_RECONFIG_8_5;

/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMHTest1);

void LteRrcMHTest1::Setup()
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

void LteRrcMHTest1::Test()
{
  lte_rrc_send_ul_msg_reqi_s ul_msg_reqi;
  
  uint8 int_queue_count;
  uint8 ext_queue_count;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  // ----------------------------------------------------------------------

  lte_rrc_init_default_hdr(&ul_msg_reqi.msg_hdr, LTE_RRC_SEND_UL_MSG_REQI);

  ul_msg_reqi.ack_needed = FALSE;
  ul_msg_reqi.rb_id = 1;
  ul_msg_reqi.mu_id = (uint8 *)lte_rrc_mem_calloc(sizeof(uint8));
  ul_msg_reqi.encode_status = (uint8 *)lte_rrc_mem_calloc(sizeof(uint8));

  /* Allocate memory to and create a decoded message. The function is in .c 
     file */
  ul_msg_reqi.ul_msg_ptr = lte_rrc_mh_test_get_CDMA2000_CSFBParametersRequest();

  lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);

  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  /* Encoding should be successful */
  TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
  /* For the first message, mu_id should be 0 */
  TF_ASSERT(*(ul_msg_reqi.mu_id) == 0);

  lte_rrc_mem_free(ul_msg_reqi.mu_id);
  lte_rrc_mem_free(ul_msg_reqi.encode_status);

  lte_rrc_osys_asn1_free_pdu(ul_msg_reqi.ul_msg_ptr,
                           lte_rrc_osys_UL_DCCH_Message_PDU);
  
  lte_rrc_utf_reset();
}


void LteRrcMHTest1::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMHClearPendQTest);

void LteRrcMHClearPendQTest::Setup()
{

  lte_rrc_utf_setup(LTE_RRC_MH_SM, NULL, 0);

  TF_STUB(lte_rrc_controller_get_state) = 
    lte_rrc_mh_test_stub_controller_get_state;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) = 
    lte_rrc_mh_test_stub_csp_get_serving_cell_params;
}

void LteRrcMHClearPendQTest::Test()
{
  lte_pdcpdl_sdu_ind_msg_s dl_msg_ind;
  lte_mac_rrc_dl_data_ind_s mac_dl_msg_ind;
  
  dsm_item_type *dsm_ptr=NULL;				  /* DSM pointer */
  msgr_attach_struct_type *att_ptr; 		  /* Message attachment pointer */
  lte_rrc_mh_dlm_s *dlm_ptr;
  uint32 buf_len;
  lte_rrc_dlm_processed_indi_s ind;
  lte_rrc_pdu_buf_s pdu_buf;
  lte_rrc_stopped_indi_s stopped_indi;

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

  for (int i = 0; i < 3; i++)
  {
    dsm_ptr = NULL;
    msgr_init_hdr_attach(&dl_msg_ind.hdr, MSGR_LTE_PDCPDL, LTE_PDCPDL_SDU_IND, 0, 1);
    TF_ASSERT(dsm_pushdown(&dsm_ptr, rrc_conn_reconfig_8_5, (uint16)sizeof(rrc_conn_reconfig_8_5), DSM_DS_SMALL_ITEM_POOL) == (uint16)sizeof(rrc_conn_reconfig_8_5));
    TF_ASSERT(dsm_ptr != NULL);
    att_ptr = msgr_get_attach(&dl_msg_ind.hdr, 0);
    msgr_set_dsm_attach(att_ptr, dsm_ptr);
    dl_msg_ind.rb_cfg_idx = 33;
    lte_rrc_utf_send_msg((byte*)&dl_msg_ind, sizeof(dl_msg_ind));
    lte_rrc_utf_wait_for_done();
  }

  for (int i = 0; i < 3; i++)
  {
    dsm_ptr = NULL;
    msgr_init_hdr_attach(&mac_dl_msg_ind.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_CCCH_DL_DATA_IND, 0, 1);
    TF_ASSERT(dsm_pushdown(&dsm_ptr, rrc_conn_reconfig_8_5, (uint16)sizeof(rrc_conn_reconfig_8_5), DSM_DS_SMALL_ITEM_POOL) == (uint16)sizeof(rrc_conn_reconfig_8_5));
    TF_ASSERT(dsm_ptr != NULL);
    att_ptr = msgr_get_attach(&mac_dl_msg_ind.hdr, 0);
    msgr_set_dsm_attach(att_ptr, dsm_ptr);
    lte_rrc_utf_send_msg((byte*)&mac_dl_msg_ind, sizeof(mac_dl_msg_ind));
    lte_rrc_utf_wait_for_done();
  }

  for (int i = 0; i < 3; i++)
  {
    dsm_ptr = NULL;
    msgr_init_hdr_attach(&mac_dl_msg_ind.hdr, MSGR_LTE_MAC, LTE_MAC_RRC_PCCH_DL_DATA_IND, 0, 1);
    TF_ASSERT(dsm_pushdown(&dsm_ptr, rrc_conn_reconfig_8_5, (uint16)sizeof(rrc_conn_reconfig_8_5), DSM_DS_SMALL_ITEM_POOL) == (uint16)sizeof(rrc_conn_reconfig_8_5));
    TF_ASSERT(dsm_ptr != NULL);
    att_ptr = msgr_get_attach(&mac_dl_msg_ind.hdr, 0);
    msgr_set_dsm_attach(att_ptr, dsm_ptr);
    lte_rrc_utf_send_msg((byte*)&mac_dl_msg_ind, sizeof(mac_dl_msg_ind));
    lte_rrc_utf_wait_for_done();
  }

  /* send LTE_RRC_STOPPED_INDI */
  lte_rrc_init_default_hdr(&stopped_indi.msg_hdr,LTE_RRC_STOPPED_INDI);
  lte_rrc_utf_send_msg((byte*)&stopped_indi, sizeof(stopped_indi));
  lte_rrc_utf_wait_for_done();

  //Stopped indi must have freed all the pended DLMs
  TF_ASSERT(lte_static_rrc_pendq_find_umid(LTE_PDCPDL_SDU_IND) == FALSE);
  TF_ASSERT(lte_static_rrc_pendq_find_umid(LTE_MAC_RRC_CCCH_DL_DATA_IND) == FALSE);
  TF_ASSERT(lte_static_rrc_pendq_find_umid(LTE_MAC_RRC_PCCH_DL_DATA_IND) == FALSE);

  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset();
}



void LteRrcMHClearPendQTest::Teardown()
{
  lte_rrc_utf_teardown();
}

