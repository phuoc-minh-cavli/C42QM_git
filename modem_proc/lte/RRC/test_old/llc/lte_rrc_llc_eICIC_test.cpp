/*!
  @file lte_rrc_llc_eICIC_test.cpp

  @brief
  Unit test code for checking eICIC RRC-ML1 interface.

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
lte_rrc_llc_conn_setup_reconfig_phy_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/13   md     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_sib.h"
#include "lte_rrc_llci.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"

#include <tf_stub.h>  
/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
extern "C"
{
  TF_PROTO_STUB(lte_rrc_div_duplex_e, lte_rrc_csp_get_last_camped_mode, void);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlceICICRLMTest);

void LteRrcLlceICICRLMTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlceICICRLMTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_errno_e status;
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_ded_cfg_ptr = NULL;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_llc_s *llc_ptr;
  int decode_status = 0;
  int i = 0;

  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

  encoded_pdu.length = sizeof(llc_eicic_release_msg);
  
  encoded_pdu.value = llc_eicic_release_msg;

  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                   (encoded_pdu.value,encoded_pdu.length,
                                                    DL_DCCH_Message, &decode_status);

  radio_resrc_ded_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                              criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                              radioResourceConfigDedicated;
  
  /*This test eICIC RLM interface for release message*/

  status = lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr,  radio_resrc_ded_cfg_ptr);

  TF_ASSERT(status == E_SUCCESS);

  //Verify eICIC data
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.enable == FALSE);

  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

  encoded_pdu.length = sizeof(llc_eicic_msg);
  
  encoded_pdu.value = llc_eicic_msg;

  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                    (encoded_pdu.value,encoded_pdu.length,
                                                     DL_DCCH_Message, &decode_status);

  radio_resrc_ded_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                               criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                               radioResourceConfigDedicated;
  
  /*This test eICIC RLM interface for FDD*/

  status = lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr,  radio_resrc_ded_cfg_ptr);

  TF_ASSERT(status == E_SUCCESS);

  //Verify eICIC data
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.enable == TRUE);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.type == 
              LTE_L1_MEAS_SUBFM_PATTERN_FDD);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_num_bits == 
              LTE_RRC_SUBFRAME_PATTERN_FDD_NUMBITS);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_pattern[0] == 
              0x41424344);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_pattern[1] == 
              0x45000000);

  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;

  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

  encoded_pdu.length = sizeof(llc_eicic_TDD_msg);
  
  encoded_pdu.value = llc_eicic_TDD_msg;

  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                    (encoded_pdu.value,encoded_pdu.length,
                                                     DL_DCCH_Message, &decode_status);

  radio_resrc_ded_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                              criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                              radioResourceConfigDedicated;
  
  /*This test eICIC RLM interface for TDD (20 bits)*/

  status = lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr,  radio_resrc_ded_cfg_ptr);

  TF_ASSERT(status == E_SUCCESS);

  //Verify eICIC data
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.enable == TRUE);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.type == 
              LTE_L1_MEAS_SUBFM_PATTERN_TDD);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_num_bits == 
              LTE_RRC_SUBFRAME_PATTERN_TDD_SUBFRAME_CONFIG_1_5_NUMBITS);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_pattern[0] == 
              0x99999000);

  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

  encoded_pdu.length = sizeof(llc_eicic_TDD1_msg);
  
  encoded_pdu.value = llc_eicic_TDD1_msg;

  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                    (encoded_pdu.value,encoded_pdu.length,
                                                     DL_DCCH_Message, &decode_status);

  radio_resrc_ded_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                              criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                              radioResourceConfigDedicated;
  
  /*This test eICIC RLM interface for TDD (70 bits)*/

  status = lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr,  radio_resrc_ded_cfg_ptr);

  TF_ASSERT(status == E_SUCCESS);

  //Verify eICIC data
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.enable == TRUE);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.type == 
              LTE_L1_MEAS_SUBFM_PATTERN_TDD);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_num_bits == 
              LTE_RRC_SUBFRAME_PATTERN_TDD_SUBFRAME_CONFIG_0_NUMBITS);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_pattern[0] == 
              0x41424344);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_pattern[1] == 
              0x45464748);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_pattern[2] == 
              0x48000000);

  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

  encoded_pdu.length = sizeof(llc_eicic_TDD2_msg);
  
  encoded_pdu.value = llc_eicic_TDD2_msg;

  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                    (encoded_pdu.value,encoded_pdu.length,
                                                     DL_DCCH_Message, &decode_status);

  radio_resrc_ded_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                              criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                              radioResourceConfigDedicated;
  
  /*This test eICIC RLM interface for TDD (60 bits)*/

  status = lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr,  radio_resrc_ded_cfg_ptr);

  TF_ASSERT(status == E_SUCCESS);

  //Verify eICIC data
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.enable == TRUE);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.type == 
              LTE_L1_MEAS_SUBFM_PATTERN_TDD);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_num_bits == 
              LTE_RRC_SUBFRAME_PATTERN_TDD_SUBFRAME_CONFIG_6_NUMBITS);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_pattern[0] == 
              0x41424344);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_pattern[1] == 
              0x45464740);

  for (i = 0; i < 20; i++)
  {
  
    llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

    encoded_pdu.length = sizeof(llc_eicic_release_msg);
  
    encoded_pdu.value = llc_eicic_release_msg;

    dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                      (encoded_pdu.value,encoded_pdu.length,
                                                       DL_DCCH_Message, &decode_status);

    radio_resrc_ded_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                                criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                                radioResourceConfigDedicated;
  
    /*This test eICIC RLM interface for release message*/

    status = lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr,  radio_resrc_ded_cfg_ptr);

    TF_ASSERT(status == E_SUCCESS);

    //Verify eICIC data
    TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.enable == FALSE);

    if ((i % 2) == 0)
    {

      TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;
      
      llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

      encoded_pdu.length = sizeof(llc_eicic_msg);
  
      encoded_pdu.value = llc_eicic_msg;

      dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                        (encoded_pdu.value,encoded_pdu.length,
                                                         DL_DCCH_Message, &decode_status);

      radio_resrc_ded_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                                  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                                  radioResourceConfigDedicated;
  
      /*This test eICIC RLM interface for FDD*/

      status = lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr,  radio_resrc_ded_cfg_ptr);

      TF_ASSERT(status == E_SUCCESS);

      //Verify eICIC data
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.enable == TRUE);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.type == 
                  LTE_L1_MEAS_SUBFM_PATTERN_FDD);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_num_bits == 
                  LTE_RRC_SUBFRAME_PATTERN_FDD_NUMBITS);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_pattern[0] == 
                  0x41424344);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_pattern[1] == 
                  0x45000000);
      
    }

    else
    {

      TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;
      
      llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

      encoded_pdu.length = sizeof(llc_eicic_TDD1_msg);
  
      encoded_pdu.value = llc_eicic_TDD1_msg;

      dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                        (encoded_pdu.value,encoded_pdu.length,
                                                         DL_DCCH_Message, &decode_status);

      radio_resrc_ded_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                                  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                                  radioResourceConfigDedicated;
  
      /*This test eICIC RLM interface for TDD (70 bits)*/

      status = lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr,  radio_resrc_ded_cfg_ptr);

      TF_ASSERT(status == E_SUCCESS);

      //Verify eICIC data
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.enable == TRUE);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.type == 
                  LTE_L1_MEAS_SUBFM_PATTERN_TDD);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_num_bits == 
                  LTE_RRC_SUBFRAME_PATTERN_TDD_SUBFRAME_CONFIG_0_NUMBITS);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_pattern[0] == 
                  0x41424344);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_pattern[1] == 
                  0x45464748);
      TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->meas_subfm_pattern_pcell.meas_subfm.subfm_pattern[2] == 
                  0x48000000);
      
      
    }

  }
  
  lte_rrc_utf_reset(); 
  
}

void LteRrcLlceICICRLMTest::Teardown()
{
  lte_rrc_utf_teardown();
}

