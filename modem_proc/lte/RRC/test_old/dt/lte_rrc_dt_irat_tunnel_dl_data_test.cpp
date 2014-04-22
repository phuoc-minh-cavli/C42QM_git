/*!
  @file lte_rrc_dt_irat_tunnel_dl_data_test.cpp

  @brief
  Unit test code for basic RRC DT state transitions. Sends UL and DL 
  Data messages to the DT module

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
07/26/10   np      Initial Revision
           
===========================================================================*/

/*===========================================================================

INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <lte_rrc_int_msg.h>
#include <lte_rrc_dti.h>
#include <lte_rrc_utils.h>
#include "lte_static_rrc_mem_util.h"
#include <lte_rrc_osys_asn1.h>
#include <lte_rrc_osys_asn1util.h>
}

#include <TestFramework.h>
#include <lte_rrc_utf.h>

#include "lte_rrc_dt_test_common.h"

//external messages generated by code under test
static uint32 ext_umid_list[] = 
{ 
    LTE_RRC_IRAT_TUNNEL_DL_MSG_IND
};

//message structures
static lte_rrc_mh_dlm_s dl_msg;

/*===========================================================================

UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcDtIratTunnelDLDataTest);

void LteRrcDtIratTunnelDLDataTest::Setup()
{
    lte_rrc_utf_setup(LTE_RRC_DT_SM, ext_umid_list, 1);
    lte_rrc_test_dt_init();
}

void LteRrcDtIratTunnelDLDataTest::Test()
{
    byte *buf_ptr;
    uint32 buf_len;
    int decode_status = 0;
    lte_rrc_irat_tunnel_dl_msg_ind_s *ext_ind_ptr;
    lte_rrc_dlm_processed_indi_s *ind_ptr;
    lte_rrc_osys_DedicatedInfoCDMA2000 *cdma_di;

    TF_MSG("Start of test");

    lte_rrc_test_dt_set_controller_state(LTE_RRC_CONTROLLER_SM__CONNECTED);

    TF_MSG("Checking for READY state");
    TF_ASSERT(stm_get_state(LTE_RRC_DT_SM) == LTE_RRC_DT_SM__READY);

    TF_MSG("Sending IRAT DL Information Transfer Message");
    lte_rrc_init_default_hdr(&dl_msg.msg_hdr, LTE_RRC_DL_INFORMATION_TRANSFER_DLM);

    lte_rrc_osys_DL_DCCH_Message *dcch_msg_ptr = 
      (lte_rrc_osys_DL_DCCH_Message *)
        lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_DL_DCCH_Message));

    TF_ASSERT(dcch_msg_ptr != 0);

    dcch_msg_ptr->message.t = T_lte_rrc_osys_DL_DCCH_MessageType_c1;
    dcch_msg_ptr->message.u.c1 = 
      (lte_rrc_osys_DL_DCCH_MessageType_c1 *)
        lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_DL_DCCH_MessageType_c1));
    dcch_msg_ptr->message.u.c1->t = 
      T_lte_rrc_osys_DL_DCCH_MessageType_c1_dlInformationTransfer;
    dcch_msg_ptr->message.u.c1->u.dlInformationTransfer = 
      (lte_rrc_osys_DLInformationTransfer *)
        lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_DLInformationTransfer));
    dcch_msg_ptr->message.u.c1->
      u.dlInformationTransfer->rrc_TransactionIdentifier = 0;
    dcch_msg_ptr->message.u.c1->
      u.dlInformationTransfer->criticalExtensions.t = 
      T_lte_rrc_osys_DLInformationTransfer_criticalExtensions_c1;
    dcch_msg_ptr->message.u.c1->
      u.dlInformationTransfer->criticalExtensions.u.c1 = 
        (lte_rrc_osys_DLInformationTransfer_criticalExtensions_c1 *)
          lte_rrc_asn1_osys_IE_malloc(
            sizeof(lte_rrc_osys_DLInformationTransfer_criticalExtensions_c1));
    dcch_msg_ptr->message.u.c1->u.dlInformationTransfer->
      criticalExtensions.u.c1->t = 
      T_lte_rrc_osys_DLInformationTransfer_criticalExtensions_c1_dlInformationTransfer_r8;
    dcch_msg_ptr->message.u.c1->u.dlInformationTransfer->
      criticalExtensions.u.c1->u.dlInformationTransfer_r8 = 
        (lte_rrc_osys_DLInformationTransfer_r8_IEs *)
          lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_DLInformationTransfer_r8_IEs));
    dcch_msg_ptr->message.u.c1->u.dlInformationTransfer->
      criticalExtensions.u.c1->u.dlInformationTransfer_r8->
      m.nonCriticalExtensionPresent = 0;
    dcch_msg_ptr->message.u.c1->u.dlInformationTransfer->
      criticalExtensions.u.c1->u.dlInformationTransfer_r8->
      dedicatedInfoType.t = 
        T_lte_rrc_osys_DLInformationTransfer_r8_IEs_dedicatedInfoType_dedicatedInfoCDMA2000_1XRTT;

    dcch_msg_ptr->message.u.c1->u.dlInformationTransfer->criticalExtensions.u.c1->
      u.dlInformationTransfer_r8->dedicatedInfoType.u.dedicatedInfoCDMA2000_1XRTT = 
      (lte_rrc_osys_DedicatedInfoCDMA2000 *)
          lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_DedicatedInfoCDMA2000));
    cdma_di = dcch_msg_ptr->message.u.c1->u.dlInformationTransfer->criticalExtensions.u.c1->
      u.dlInformationTransfer_r8->dedicatedInfoType.u.dedicatedInfoCDMA2000_1XRTT;

    cdma_di->numocts = 10; //LTE_RRC_MAX_NAS_MSG_CONTAINER_SIZE;
    unsigned char *cdma_msg = (unsigned char*)lte_rrc_asn1_osys_IE_malloc(10);
    memset(cdma_msg, 'A', 10);
    cdma_di->data = cdma_msg;

    dl_msg.dl_msg_ptr = dcch_msg_ptr;
    
    /*
    unsigned char byte_str[] = {0x10, 0x01, 0x00, 0x07, 0x79, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00};
    dl_msg.dl_msg_ptr = lte_rrc_asn1_decode_pdu((void*)byte_str, 
                                                lte_rrc_DL_DCCH_Message_PDU,
                                                sizeof(byte_str));
    */

    dl_msg.pdu_num = lte_rrc_osys_DL_DCCH_Message_PDU;

    lte_rrc_utf_send_msg((byte*)&dl_msg, sizeof(dl_msg));

    TF_MSG("Checking for LTE_RRC_IRAT_TUNNEL_DL_MSG_IND");
    lte_rrc_utf_get_next_ext_msg(&buf_ptr, &buf_len);
    TF_ASSERT(buf_len == sizeof(lte_rrc_irat_tunnel_dl_msg_ind_s));
    ext_ind_ptr = (lte_rrc_irat_tunnel_dl_msg_ind_s *)(void*) buf_ptr;
    TF_ASSERT(LTE_RRC_IRAT_TUNNEL_DL_MSG_IND == ext_ind_ptr->msg_hdr.id); 
    TF_ASSERT(LTE_IRAT_TUNNEL_DL_MSG_DL_INFO_TRANSFER == ext_ind_ptr->msg.dl_msg_type);
    TF_ASSERT(LTE_IRAT_RAT_TYPE_1XRTT == ext_ind_ptr->msg.dl_msg_u.dl_info_tx.type);
    /*TF_ASSERT(cdma_di->numocts == ext_ind_ptr->msg.dl_msg_u.dl_info_tx.msg_container.msg_size );
    TF_ASSERT(0 == memcmp (cdma_di->data,
                           &ext_ind_ptr->msg.dl_msg_u.dl_info_tx.msg_container.msg_payload,
                           cdma_di->numocts) );*/


    TF_MSG("Checking for LTE_RRC_DLM_PROCESSED_INDI");
    lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
    TF_ASSERT(sizeof(lte_rrc_dlm_processed_indi_s) == buf_len);
    ind_ptr = (lte_rrc_dlm_processed_indi_s*)(void*) buf_ptr;
    TF_ASSERT(LTE_RRC_DLM_PROCESSED_INDI == ind_ptr->msg_hdr.id); 

    TF_MSG("Checking for READY state");
    TF_ASSERT(stm_get_state(LTE_RRC_DT_SM) == LTE_RRC_DT_SM__READY);

	  lte_rrc_osys_asn1_free_pdu(dcch_msg_ptr,
	                           lte_rrc_osys_DL_DCCH_Message_PDU);			

    TF_MSG("End of test");
}

void LteRrcDtIratTunnelDLDataTest::Teardown()
{
    lte_rrc_utf_teardown();
    lte_rrc_test_dt_teardown();
}

