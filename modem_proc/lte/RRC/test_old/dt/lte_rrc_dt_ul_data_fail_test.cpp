/*!
  @file lte_rrc_dt_ul_data_fail_test.cpp

  @brief
  Unit test code for basic RRC DT state transitions. Sends UL Data messages
  to the DT module

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
01/08/09   vatsac  Initial Revision
           
===========================================================================*/

/*===========================================================================

INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <lte_rrc_int_msg.h>
#include <lte_rrc_dti.h>
#include <lte_rrc_utils.h>
#include <lte_rrc_osys_asn1util.h>
#include <dsm_item.h>
#include <dsm_init.h>
  
}

#include <TestFramework.h>
#include <lte_rrc_utf.h>

#include "lte_rrc_dt_test_common.h"

//external messages generated by code under test
static uint32 ext_umid_list[] = 
{ 
    LTE_RRC_UL_DATA_CNF
};

//message structures
static lte_rrc_ul_data_req_s ul_data_req;
static lte_rrc_ul_msg_cnfi_s ul_msg_cnfi;
static lte_rrc_nas_msg_container_s nas_msg_container;
/*===========================================================================

UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcDtULDataFailTest);

void LteRrcDtULDataFailTest::Setup()
{
    lte_rrc_utf_setup(LTE_RRC_DT_SM, ext_umid_list, 1);
    lte_rrc_test_dt_init();
}

void LteRrcDtULDataFailTest::Test()
{
    byte *buf_ptr;
    uint32 buf_len;
    lte_rrc_send_ul_msg_reqi_s *int_req_ptr;
    lte_rrc_ul_data_cnf_s *ext_cnf_ptr;
    dsm_item_type *dsm_ptr=NULL;
    /* Message attachment pointer */
    msgr_attach_struct_type *att_ptr;

    TF_MSG("Start of test");

    lte_rrc_test_dt_set_controller_state(LTE_RRC_CONTROLLER_SM__CONNECTED);

    TF_MSG("Checking for READY state");
    TF_ASSERT(stm_get_state(LTE_RRC_DT_SM) == LTE_RRC_DT_SM__READY);

    TF_MSG("Sending UL Information Transfer Message");
    msgr_init_hdr_attach(&ul_data_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_UL_DATA_REQ, 0, 1);
    
    // ul_data_req.priority;
    ul_data_req.cnf_is_reqd = true;
    nas_msg_container.msg_size = LTE_RRC_MAX_NAS_MSG_CONTAINER_SIZE;
    memset(nas_msg_container.msg_payload, 'A',
           nas_msg_container.msg_size);

    /* Pushdown DSM */
    LTE_ASSERT(dsm_pushdown(&dsm_ptr, nas_msg_container.msg_payload, 
														(uint16)LTE_RRC_MAX_NAS_MSG_CONTAINER_SIZE, 
														LTE_RRC_DSM_POOL((uint16)LTE_RRC_MAX_NAS_MSG_CONTAINER_SIZE)) 
        == (uint16)LTE_RRC_MAX_NAS_MSG_CONTAINER_SIZE);

    /* Set DSM attach */
    att_ptr = msgr_get_attach(&ul_data_req.msg_hdr, 0);
    LTE_ASSERT(att_ptr != NULL);
    msgr_set_dsm_attach(att_ptr, dsm_ptr);
    
    LTE_ASSERT(dsm_ptr != NULL);
    
    lte_rrc_utf_send_msg((byte*)&ul_data_req, sizeof(ul_data_req));

    lte_rrc_utf_wait_for_done();

    TF_MSG("Checking for LTE_RRC_UL_DATA_REQI");
    lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
    int_req_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
    TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == int_req_ptr->msg_hdr.id); 

    /* validate msg */

    /* free msg */
    lte_rrc_osys_asn1_free_pdu(int_req_ptr->ul_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);

    lte_rrc_utf_wait_for_done();

    TF_MSG("Sending UL Information Transfer CNFI");

    lte_rrc_init_default_hdr(&ul_msg_cnfi.msg_hdr, LTE_RRC_UL_INFORMATION_TRANSFER_CNFI);

    ul_msg_cnfi.mu_id = 0;
    ul_msg_cnfi.status = LTE_RRC_UL_MSG_NACK;
    ul_msg_cnfi.fail_cause = LTE_RRC_UL_MSG_FAIL_CAUSE_MAX_TX;

    lte_rrc_utf_send_msg((byte*)&ul_msg_cnfi, sizeof(ul_msg_cnfi));

    lte_rrc_utf_wait_for_done();

    TF_MSG("Checking for LTE_RRC_UL_DATA_CNF");

    lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_ul_data_cnf_s));
    ext_cnf_ptr = (lte_rrc_ul_data_cnf_s *)(void*) buf_ptr;
    TF_ASSERT(LTE_RRC_UL_DATA_CNF == ext_cnf_ptr->msg_hdr.id); 

    TF_ASSERT(LTE_RRC_UL_DATA_CNF_FAILURE_TXN == ext_cnf_ptr->status); 

    lte_rrc_utf_wait_for_done();

    TF_MSG("Checking for READY state");
    TF_ASSERT(stm_get_state(LTE_RRC_DT_SM) == LTE_RRC_DT_SM__READY);

    TF_MSG("End of test");
}

void LteRrcDtULDataFailTest::Teardown()
{
    lte_rrc_utf_teardown();
    lte_rrc_test_dt_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcDtULDataFailTestMax);

void LteRrcDtULDataFailTestMax::Setup()
{
    lte_rrc_utf_setup(LTE_RRC_DT_SM, ext_umid_list, 1);
    lte_rrc_test_dt_init();
}

void LteRrcDtULDataFailTestMax::Test()
{
    byte *buf_ptr;
    uint32 buf_len;
    lte_rrc_send_ul_msg_reqi_s *int_req_ptr;
    lte_rrc_ul_data_cnf_s *ext_cnf_ptr;
    uint8 i;

    TF_MSG("Start of test");

    lte_rrc_test_dt_set_controller_state(LTE_RRC_CONTROLLER_SM__CONNECTED);

    TF_MSG("Checking for READY state");
    TF_ASSERT(stm_get_state(LTE_RRC_DT_SM) == LTE_RRC_DT_SM__READY);

    for(i = 0; i <= 33; i++)
    {
    lte_rrc_ul_data_req_s ul_data_req;
    dsm_item_type *dsm_ptr=NULL;
    /* Message attachment pointer */
    msgr_attach_struct_type *att_ptr;

    TF_MSG("Sending UL Information Transfer Message");
    msgr_init_hdr_attach(&ul_data_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_UL_DATA_REQ, 0, 1);
    
    // ul_data_req.priority;
    ul_data_req.cnf_is_reqd = true;
    nas_msg_container.msg_size = 5;
    ul_data_req.trans_id = i+1;
    memset(nas_msg_container.msg_payload, 'A',
           nas_msg_container.msg_size);

    /* Pushdown DSM */
    LTE_ASSERT(dsm_pushdown(&dsm_ptr, nas_msg_container.msg_payload, 
														5, 
														LTE_RRC_DSM_POOL(5)) 
        == 5);

    /* Set DSM attach */
    att_ptr = msgr_get_attach(&ul_data_req.msg_hdr, 0);
    LTE_ASSERT(att_ptr != NULL);
    msgr_set_dsm_attach(att_ptr, dsm_ptr);
    
    LTE_ASSERT(dsm_ptr != NULL);
    
    lte_rrc_utf_send_msg((byte*)&ul_data_req, sizeof(ul_data_req));

    lte_rrc_utf_wait_for_done();

    TF_MSG("Checking for LTE_RRC_UL_DATA_REQI");
    lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
    int_req_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
    TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == int_req_ptr->msg_hdr.id); 

    /* validate msg */

    /* free msg */
    lte_rrc_osys_asn1_free_pdu(int_req_ptr->ul_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);

    lte_rrc_utf_wait_for_done();
    }

    TF_MSG("Checking for LTE_RRC_UL_DATA_CNF");

    lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_ul_data_cnf_s));
    ext_cnf_ptr = (lte_rrc_ul_data_cnf_s *)(void*) buf_ptr;
    TF_ASSERT(LTE_RRC_UL_DATA_CNF == ext_cnf_ptr->msg_hdr.id); 

    TF_ASSERT(LTE_RRC_UL_DATA_CNF_FAILURE_TXN == ext_cnf_ptr->status); 
    TF_ASSERT(ext_cnf_ptr->num_trans_ids == 1);
    TF_ASSERT(ext_cnf_ptr->trans_id[0] == 1);

    lte_rrc_utf_wait_for_done();

    TF_MSG("Sending UL Information Transfer CNFI");

    lte_rrc_init_default_hdr(&ul_msg_cnfi.msg_hdr, LTE_RRC_UL_INFORMATION_TRANSFER_CNFI);

    ul_msg_cnfi.mu_id = 0;
    ul_msg_cnfi.status = LTE_RRC_UL_MSG_NACK;
    ul_msg_cnfi.fail_cause = LTE_RRC_UL_MSG_FAIL_CAUSE_MAX_TX;

    lte_rrc_utf_send_msg((byte*)&ul_msg_cnfi, sizeof(ul_msg_cnfi));

    lte_rrc_utf_wait_for_done();
	
    lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_ul_data_cnf_s));
    ext_cnf_ptr = (lte_rrc_ul_data_cnf_s *)(void*) buf_ptr;
    TF_ASSERT(LTE_RRC_UL_DATA_CNF == ext_cnf_ptr->msg_hdr.id);
    TF_ASSERT(ext_cnf_ptr->num_trans_ids == 1);
    TF_ASSERT(ext_cnf_ptr->trans_id[0] == 34); 

    TF_ASSERT(LTE_RRC_UL_DATA_CNF_FAILURE_TXN == ext_cnf_ptr->status); 

    lte_rrc_utf_wait_for_done();

    TF_MSG("Sending UL Information Transfer CNFI");

    lte_rrc_init_default_hdr(&ul_msg_cnfi.msg_hdr, LTE_RRC_UL_INFORMATION_TRANSFER_CNFI);

    ul_msg_cnfi.mu_id = 0;
    ul_msg_cnfi.status = LTE_RRC_UL_MSG_NACK;
    ul_msg_cnfi.fail_cause = LTE_RRC_UL_MSG_FAIL_CAUSE_MAX_TX;

    lte_rrc_utf_send_msg((byte*)&ul_msg_cnfi, sizeof(ul_msg_cnfi));

    lte_rrc_utf_wait_for_done();
	
    lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_ul_data_cnf_s));
    ext_cnf_ptr = (lte_rrc_ul_data_cnf_s *)(void*) buf_ptr;
    TF_ASSERT(LTE_RRC_UL_DATA_CNF == ext_cnf_ptr->msg_hdr.id);
    TF_ASSERT(ext_cnf_ptr->num_trans_ids == 1);
    TF_ASSERT(ext_cnf_ptr->trans_id[0] == 2); 

    TF_ASSERT(LTE_RRC_UL_DATA_CNF_FAILURE_TXN == ext_cnf_ptr->status); 

    lte_rrc_utf_wait_for_done();

    TF_MSG("Sending UL Information Transfer CNFI");

    lte_rrc_init_default_hdr(&ul_msg_cnfi.msg_hdr, LTE_RRC_UL_INFORMATION_TRANSFER_CNFI);

    ul_msg_cnfi.mu_id = 0;
    ul_msg_cnfi.status = LTE_RRC_UL_MSG_NACK;
    ul_msg_cnfi.fail_cause = LTE_RRC_UL_MSG_FAIL_CAUSE_MAX_TX;

    lte_rrc_utf_send_msg((byte*)&ul_msg_cnfi, sizeof(ul_msg_cnfi));

    lte_rrc_utf_wait_for_done();
	
    lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_ul_data_cnf_s));
    ext_cnf_ptr = (lte_rrc_ul_data_cnf_s *)(void*) buf_ptr;
    TF_ASSERT(LTE_RRC_UL_DATA_CNF == ext_cnf_ptr->msg_hdr.id);
    TF_ASSERT(ext_cnf_ptr->num_trans_ids == 1);
    TF_ASSERT(ext_cnf_ptr->trans_id[0] == 3); 

    TF_ASSERT(LTE_RRC_UL_DATA_CNF_FAILURE_TXN == ext_cnf_ptr->status); 

    lte_rrc_utf_wait_for_done();

    lte_rrc_ul_data_req_s ul_data_req;
    dsm_item_type *dsm_ptr=NULL;
    /* Message attachment pointer */
    msgr_attach_struct_type *att_ptr;

    TF_MSG("Sending UL Information Transfer Message");
    msgr_init_hdr_attach(&ul_data_req.msg_hdr, MSGR_LTE_RRC, LTE_RRC_UL_DATA_REQ, 0, 1);
    
    // ul_data_req.priority;
    ul_data_req.cnf_is_reqd = true;
    nas_msg_container.msg_size = 5;
    ul_data_req.trans_id = 35;
    memset(nas_msg_container.msg_payload, 'A',
           nas_msg_container.msg_size);

    /* Pushdown DSM */
    LTE_ASSERT(dsm_pushdown(&dsm_ptr, nas_msg_container.msg_payload, 
														5, 
														LTE_RRC_DSM_POOL(5)) 
        == 5);

    /* Set DSM attach */
    att_ptr = msgr_get_attach(&ul_data_req.msg_hdr, 0);
    LTE_ASSERT(att_ptr != NULL);
    msgr_set_dsm_attach(att_ptr, dsm_ptr);
    
    LTE_ASSERT(dsm_ptr != NULL);
    
    lte_rrc_utf_send_msg((byte*)&ul_data_req, sizeof(ul_data_req));

    lte_rrc_utf_wait_for_done();

    TF_MSG("Checking for LTE_RRC_UL_DATA_REQI");
    lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);

    TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
    int_req_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
    TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == int_req_ptr->msg_hdr.id); 

    /* validate msg */

    /* free msg */
    lte_rrc_osys_asn1_free_pdu(int_req_ptr->ul_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);

    lte_rrc_utf_wait_for_done();

    TF_MSG("Checking for READY state");
    TF_ASSERT(stm_get_state(LTE_RRC_DT_SM) == LTE_RRC_DT_SM__READY);

    TF_MSG("End of test");
}

void LteRrcDtULDataFailTestMax::Teardown()
{
    lte_rrc_utf_teardown();
    lte_rrc_test_dt_teardown();
}



