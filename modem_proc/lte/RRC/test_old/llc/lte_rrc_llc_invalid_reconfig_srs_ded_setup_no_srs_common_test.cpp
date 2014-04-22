/*!
  @file lte_rrc_llc_invalid_reconfig_srs_ded_setup_no_srs_common_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a reconfig message that sets up SRS dedicated config when SRS common config
  is not enabled.

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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
lte_rrc_llc_invalid_reconfig_srs_ded_setup_no_srs_common_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/24/10   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
  #include "lte_rrc_sib.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include "lte_mac_ind.h"
#include <tf_stub.h>  

extern "C"
{
  TF_PROTO_STUB(void *, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
  TF_PROTO_STUB(uint8, lte_rrc_csp_get_num_tx_antennas, void);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidReconfigSRSDedSetupNoSRSCommonTest);

void LteRrcLlcInvalidReconfigSRSDedSetupNoSRSCommonTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcInvalidReconfigSRSDedSetupNoSRSCommonTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_common_cfg_mask_t common_cfg_mask;
  
  TF_MSG("\nLLC SRS dedicated config setup without SRS config common test begins \n");

  lte_rrc_llc_test_basic_sib2_update();

  TF_STUB(lte_rrc_csp_get_num_tx_antennas) = lte_rrc_llc_test_csp_get_num_tx_antennas_mimo;
  lte_rrc_llc_test_common_data.llc_basic_mib.num_tx_antennas = 2;

  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();

  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_srs_release_get_cell_sib;

  common_cfg_mask = 
  (
    LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG
  );
  lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
  /* Send SIB2 update that releases the SRS common config */
  lte_rrc_llc_test_basic_sib2_update();

  /* Send a HO command that configures SRS config dedicated */
  encoded_pdu.length = sizeof(llc_reconfig_phyded_msg);
  encoded_pdu.value = llc_reconfig_phyded_msg;
  
  lte_rrc_llc_test_ho_validation_failure(encoded_pdu);

  TF_MSG("\nLLC SRS dedicated config setup without SRS config common test ends \n");
  lte_rrc_utf_reset(); 

}

void LteRrcLlcInvalidReconfigSRSDedSetupNoSRSCommonTest::Teardown()
{
  lte_rrc_utf_teardown();
}




