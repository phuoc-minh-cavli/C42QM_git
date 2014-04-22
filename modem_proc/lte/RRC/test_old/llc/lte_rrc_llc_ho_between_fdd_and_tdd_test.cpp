/*!
  @file lte_rrc_llc_ho_between_fdd_and_tdd_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the handling of HO between
  FDD and TDD cells. The test includes handover from FDD to TDD, TDD to TDD and
  TDD to FDD.

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
lte_rrc_llc_ho_between_fdd_and_tdd_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/18/09   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_sib.h"
#include "lte_rrc_utils.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h>  	

extern "C"
{
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s*);
  TF_PROTO_STUB(void *, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
  TF_PROTO_STUB(boolean, lte_rrc_cap_get_r11_cap_bit, uint8);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcHOBetweenFDDandTDDTest);

void LteRrcLlcHOBetweenFDDandTDDTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
}

void LteRrcLlcHOBetweenFDDandTDDTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_common_cfg_mask_t common_cfg_mask;
  
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg2);
  encoded_pdu.value = llc_reconfig_ho_msg2;

  /* Set the serving cell parameters return the same DL freq as the target cell
    This is needed till inter-freq HO gets supported */
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params;

  /* Trigger FDD to TDD handover */
  lte_rrc_llc_test_ho_fdd_to_tdd(encoded_pdu, FALSE);

  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  common_cfg_mask = 
    (
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_DRX_CFG |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH |
      LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG
      //LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM
    );
  lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
  lte_rrc_llc_test_basic_sib2_update();

  encoded_pdu.length = sizeof(llc_reconfig_ho_delta_config_msg);
  encoded_pdu.value = llc_reconfig_ho_delta_config_msg;
  /* Trigger TDD to TDD handover */
  lte_rrc_llc_test_ho_tdd_to_tdd(encoded_pdu);

  /* Set the serving cell parameters return the same DL freq as the target cell
    This is needed till inter-freq HO gets supported */
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_get_serving_cell_params;

  /* Trigger TDD to FDD handover */
  /* REVISIT - Uncomment after inter-freq HO gets supported */
  // lte_rrc_llc_test_ho_tdd_to_fdd(encoded_pdu);
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_utf_reset(); 
}

void LteRrcLlcHOBetweenFDDandTDDTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcHOBetweenFDDandTDDR11SSF9Test);

void LteRrcLlcHOBetweenFDDandTDDR11SSF9Test::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel11;
  TF_STUB(lte_rrc_cap_get_r11_cap_bit) = lte_rrc_llc_test_get_r11_cap_bit_ssf9_enabled;
}

void LteRrcLlcHOBetweenFDDandTDDR11SSF9Test::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_common_cfg_mask_t common_cfg_mask;
  
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg2);
  encoded_pdu.value = llc_reconfig_ho_msg2;

  /* Set the serving cell parameters return the same DL freq as the target cell
    This is needed till inter-freq HO gets supported */
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params;

  /* Trigger FDD to TDD handover */
  lte_rrc_llc_test_ho_fdd_to_tdd(encoded_pdu, TRUE);

  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  common_cfg_mask = 
    (
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_DRX_CFG |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH |
      LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG
      //LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM
    );
  lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
  lte_rrc_llc_test_basic_sib2_update();

  encoded_pdu.length = sizeof(llc_reconfig_ho_delta_config_msg);
  encoded_pdu.value = llc_reconfig_ho_delta_config_msg;
  /* Trigger TDD to TDD handover */
  lte_rrc_llc_test_ho_tdd_to_tdd(encoded_pdu);

  /*Set the serving cell parameters return the same DL freq as the target cell
    This is needed till inter-freq HO gets supported */
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_get_serving_cell_params;

  /* Trigger TDD to FDD handover */
  /* REVISIT - Uncomment after inter-freq HO gets supported */
  // lte_rrc_llc_test_ho_tdd_to_fdd(encoded_pdu);
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_utf_reset(); 
}

void LteRrcLlcHOBetweenFDDandTDDR11SSF9Test::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcHOBetweenFDDandTDDWithoutAckNakFbTest);

void LteRrcLlcHOBetweenFDDandTDDWithoutAckNakFbTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
}

void LteRrcLlcHOBetweenFDDandTDDWithoutAckNakFbTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_common_cfg_mask_t common_cfg_mask;
  
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg3);
  encoded_pdu.value = llc_reconfig_ho_msg3;

  /* Set the serving cell parameters return the same DL freq as the target cell
    This is needed till inter-freq HO gets supported */
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params;

  /* Trigger FDD to TDD handover */
  lte_rrc_llc_test_ho_fdd_to_tdd(encoded_pdu, FALSE);

  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  common_cfg_mask = 
    (
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_DRX_CFG |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH |
      LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG
      //LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM
    );
  lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
  lte_rrc_llc_test_basic_sib2_update();

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_utf_reset(); 
}

void LteRrcLlcHOBetweenFDDandTDDWithoutAckNakFbTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcHOBetweenFDDandTDDWOPucchTest);

void LteRrcLlcHOBetweenFDDandTDDWOPucchTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
}

void LteRrcLlcHOBetweenFDDandTDDWOPucchTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_common_cfg_mask_t common_cfg_mask;
  
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg41);
  encoded_pdu.value = llc_reconfig_ho_msg41;

  /* Set the serving cell parameters return the same DL freq as the target cell
    This is needed till inter-freq HO gets supported */
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params;

  /* Trigger FDD to TDD handover */
  lte_rrc_llc_test_ho_fdd_to_tdd(encoded_pdu, FALSE);

  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  common_cfg_mask = 
    (
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_DRX_CFG |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH |
      LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG
      //LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM
    );
  lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
  lte_rrc_llc_test_basic_sib2_update();

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_utf_reset();
  
}

void LteRrcLlcHOBetweenFDDandTDDWOPucchTest::Teardown()
{
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcHOBetweenFDDandTDDWithAckNakFbAndPucchR10IETest);

void LteRrcLlcHOBetweenFDDandTDDWithAckNakFbAndPucchR10IETest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
}

void LteRrcLlcHOBetweenFDDandTDDWithAckNakFbAndPucchR10IETest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_common_cfg_mask_t common_cfg_mask;
  
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg4);
  encoded_pdu.value = llc_reconfig_ho_msg4;

  /* Set the serving cell parameters return the same DL freq as the target cell
    This is needed till inter-freq HO gets supported */
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params;

  /* Trigger FDD to TDD handover */
  lte_rrc_llc_test_ho_fdd_to_tdd(encoded_pdu, FALSE);

  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  common_cfg_mask = 
    (
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_DRX_CFG |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH |
      LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG
      //LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM
    );
  lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
  lte_rrc_llc_test_basic_sib2_update();

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_utf_reset(); 
}

void LteRrcLlcHOBetweenFDDandTDDWithAckNakFbAndPucchR10IETest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcHOBetweenFDDandTDDWithNoAckNakFbAndPucchR10IETest);

void LteRrcLlcHOBetweenFDDandTDDWithNoAckNakFbAndPucchR10IETest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcHOBetweenFDDandTDDWithNoAckNakFbAndPucchR10IETest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_common_cfg_mask_t common_cfg_mask;
  
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg3);
  encoded_pdu.value = llc_reconfig_ho_msg3;

  /* Set the serving cell parameters return the same DL freq as the target cell
    This is needed till inter-freq HO gets supported */
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params;

  /* Trigger FDD to TDD handover */
  lte_rrc_llc_test_ho_fdd_to_tdd_without_ack_nak_fb_mode(encoded_pdu);

  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  common_cfg_mask = 
    (
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_DRX_CFG |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH |
      LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG
      //LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG |
      //LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM
    );

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_utf_reset(); 
}

void LteRrcLlcHOBetweenFDDandTDDWithNoAckNakFbAndPucchR10IETest::Teardown()
{
  lte_rrc_utf_teardown();
}


