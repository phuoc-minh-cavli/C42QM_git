/*!
  @file lte_rrc_llc_invalid_ho_fdd_to_tdd_cdrx_config_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the handling of HO mesg from
  FDD to TDD that includes CDRX config.

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

$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/llc/
lte_rrc_llc_invalid_ho_fdd_to_tdd_no_tdd_ack_nak_feedback_mode_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/02/09   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_csp.h"
#include "lte_rrc_capabilities.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h>  	

extern "C"
{
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s*);
  TF_PROTO_STUB(uint8, lte_rrc_cap_get_fgi_bit, uint8);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidHOFDDtoTDDCDRXConfigTest);

void LteRrcLlcInvalidHOFDDtoTDDCDRXConfigTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();  
}

void LteRrcLlcInvalidHOFDDtoTDDCDRXConfigTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_RadioResourceConfigCommon *radio_resrc_common_ptr = NULL;
  int decode_status = 0;
  TF_MSG("\nConnection Reconfiguration Invalid HO - FDD to TDD with CDRX config Test Begins \n");
  lte_rrc_llc_test_basic_sib2_update(); 
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg);
  encoded_pdu.value = llc_reconfig_ho_msg;
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(lte_rrc_llc_test_common_data.dl_dcch_msg_ptr != NULL);

  /* Set the DL freq to a freq in TDD band */
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo.
    carrierFreq.dl_CarrierFreq = 36000;

  /* Set TDD Config parameters */
  radio_resrc_common_ptr = &lte_rrc_llc_test_common_data.dl_dcch_msg_ptr->message.u.c1->u.
    rrcConnectionReconfiguration->criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
    mobilityControlInfo.radioResourceConfigCommon;
  radio_resrc_common_ptr->m.tdd_ConfigPresent = 1;
  radio_resrc_common_ptr->tdd_Config.specialSubframePatterns 
    = lte_rrc_osys_TDD_Config_specialSubframePatterns_ssp4;
  radio_resrc_common_ptr->tdd_Config.subframeAssignment 
    = lte_rrc_osys_TDD_Config_subframeAssignment_sa3;

  /* Set the stub function to return the cell info for a TDD cell */
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params;
  /* Set CDRX supported to FALSE */
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_llc_test_get_fgi_bit_0;

  /* Trigger FDD to TDD handover */
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);  
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reconfiguration Invalid HO - FDD to TDD with CDRX config Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_utf_reset(); 
}

void LteRrcLlcInvalidHOFDDtoTDDCDRXConfigTest::Teardown()
{
  lte_rrc_utf_teardown();
}














