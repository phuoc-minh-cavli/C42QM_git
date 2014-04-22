/*!
  @file lte_rrc_capabilities_basic_test.cpp

  @brief
  Inject Capability Enquiry DLM and get response

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
09/11/09   sureshs Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <lte_rrc_int_msg.h>
#include <lte_rrc_osys_asn1util.h>
#include <__lte_rrc_capabilities.h>
#include <lte_rrc_capabilities.h>
#include <lte_rrc_utils.h>
}

#include <TestFramework.h>
#include <lte_rrc_utf.h>
#include "lte_rrc_capabilities_test_common.h"

//message structures
static lte_rrc_mh_dlm_s cap_enq_dlm;
static lte_rrc_ul_msg_cnfi_s cap_ulm_cnfi;

//MU ID to be returned from UL message send
#define LTE_RRC_CAPABILITIES_TEST_MU_ID 7

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesBasicTest);

void LteRrcCapabilitiesBasicTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
}

void LteRrcCapabilitiesBasicTest::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  int result;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  
   //specify two EUTRA bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
  SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);
  TF_ASSERT(ul_msg_ptr->ul_msg_ptr != NULL);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);
   /* LTE UE capability info
  lte_ue_cap_info_s    cap_info;  

  lte_rrc_cap_get_ue_cap_info(&cap_info, 0xFFFFFFFFFFFFFFFF);

  lte_rrc_cap_get_ue_cap_info(&cap_info, 0);

  TF_ASSERT(cap_info.max_num_dl_carriers != 1);*/

  TF_MSG("End of test");
}

void LteRrcCapabilitiesBasicTest::Teardown()
{
  lte_rrc_utf_teardown();
}

