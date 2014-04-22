
/*!
  @file lte_rrc_llc_invalid_reconfig_duplicate_drb_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a Reconfig message containing invalid duplicate DRB configuration.

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

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/04/09   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_sib.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h>  	

extern "C"
{
TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
TF_PROTO_STUB(boolean, lte_rrc_cell_is_tdd, lte_earfcn_t);
TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s*);
TF_PROTO_STUB(void *, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
#ifdef FEATURE_LTE_CA
TF_PROTO_STUB(boolean, lte_rrc_cap_is_band_combo_valid, sys_sband_lte_e_type, uint8 , 
	const sys_sband_lte_e_type *, const lte_bandwidth_e *, const uint8 *, boolean);
	#endif
}

static lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
static lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
static uint32 buf_len;

/*===========================================================================

                                UNIT TEST

===========================================================================*/


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidReconfigDedConfigFddSrsApTest);

void LteRrcLlcInvalidReconfigDedConfigFddSrsApTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

}

void LteRrcLlcInvalidReconfigDedConfigFddSrsApTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Reconfig with Ded Config FDD SRS AP Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_default();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_invalid_ap_srs_msg);
  encoded_pdu.value = llc_invalid_ap_srs_msg;
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nInvalid Reconfig with Ded Config FDD SRS AP Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInvalidReconfigDedConfigFddSrsApTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidReconfigDedConfigApSrsAntennaSelection);

void LteRrcLlcInvalidReconfigDedConfigApSrsAntennaSelection::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

}

void LteRrcLlcInvalidReconfigDedConfigApSrsAntennaSelection::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Reconfig with Ded Config FDD SRS AP Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_with_r10_antenna_selection();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_valid_srs_msg);
  encoded_pdu.value = llc_valid_srs_msg;
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nInvalid Reconfig with Ded Config FDD SRS AP Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInvalidReconfigDedConfigApSrsAntennaSelection::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidReconfigDedConfigApSrsAntennaSelectionScell);

void LteRrcLlcInvalidReconfigDedConfigApSrsAntennaSelectionScell::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

#ifdef FEATURE_LTE_CA
  TF_STUB(lte_rrc_cap_is_band_combo_valid) = lte_rrc_llc_test_get_band_combo_bitmask_FF;
  #endif
}

void LteRrcLlcInvalidReconfigDedConfigApSrsAntennaSelectionScell::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Reconfig with Ded Config FDD SRS AP Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_with_apsrs();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_ca_scell_add_with_apsrs_msg);
  encoded_pdu.value = llc_reconfig_ca_scell_add_with_apsrs_msg;
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nInvalid Reconfig with Ded Config FDD SRS AP Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInvalidReconfigDedConfigApSrsAntennaSelectionScell::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidReconfigDedConfigApSrsAntennaSelection2);

void LteRrcLlcInvalidReconfigDedConfigApSrsAntennaSelection2::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

  

}

void LteRrcLlcInvalidReconfigDedConfigApSrsAntennaSelection2::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Reconfig with Ded Config FDD SRS AP Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_with_r8_antenna_selection();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_valid_srs_msg);
  encoded_pdu.value = llc_valid_srs_msg;
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nInvalid Reconfig with Ded Config FDD SRS AP Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInvalidReconfigDedConfigApSrsAntennaSelection2::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcvalidReconfigDedConfigApSrsAntennaSelection);

void LteRrcLlcvalidReconfigDedConfigApSrsAntennaSelection::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

}

void LteRrcLlcvalidReconfigDedConfigApSrsAntennaSelection::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Reconfig with Ded Config FDD SRS AP Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_with_r8_antenna_selection();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_valid_apsrs_antenna_selection_rel_msg);
  encoded_pdu.value = llc_valid_apsrs_antenna_selection_rel_msg;
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);
  TF_MSG("\nLLC: Rcvd CPHY_DEDICATED_CFG_REQ \n");

  /* After CPHY Config req is sent, assert LLC is in WAIT_FOR_CPHY_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  TF_MSG("\nCPHY_DEDICATED_CFG_REQ rcvd and LLC Module in WAIT_FOR_CPHY_CFG_CNF state\n");

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  /* Setting the fields in the LTE_CPHY_DEDICATED_CFG_CNF sent by CPHY, The bits
    set in dedicated_cfg_validity field reflect the channel config that were set
    in the llc_conn_setup_msg in the lte_rrc_llc_ota_msg.h */
  cphy_dedicated_cfg_cnf.dedicated_cfg_validity = cphy_dedicated_cfg_req_ptr->dedicated_cfg_param.cfg_mask;
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();


  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);


  TF_MSG("\nInvalid Reconfig with Ded Config FDD SRS AP Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcvalidReconfigDedConfigApSrsAntennaSelection::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidReconfigDedConfigTddSrsApTest);

void LteRrcLlcInvalidReconfigDedConfigTddSrsApTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params;
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
}

void LteRrcLlcInvalidReconfigDedConfigTddSrsApTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Reconfig with Ded Config TDD SRS AP Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  //lte_rrc_llc_test_rrc_connection_setup_default();
  encoded_pdu.length = sizeof(llc_conn_setup_default_ies);
  encoded_pdu.value = llc_conn_setup_default_ies;

  lte_rrc_llc_test_rrc_connection_setup(encoded_pdu);
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_invalid_ap_srs_msg);
  encoded_pdu.value = llc_invalid_ap_srs_msg;
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nInvalid Reconfig with Ded Config TDD SRS AP Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInvalidReconfigDedConfigTddSrsApTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidReconfigDedConfigTddSrs0ApTest);

void LteRrcLlcInvalidReconfigDedConfigTddSrs0ApTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params;
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
}

void LteRrcLlcInvalidReconfigDedConfigTddSrs0ApTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Reconfig with Ded Config TDD SRS 0 AP Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  //lte_rrc_llc_test_rrc_connection_setup_default();
  encoded_pdu.length = sizeof(llc_conn_setup_default_ies);
  encoded_pdu.value = llc_conn_setup_default_ies;

  lte_rrc_llc_test_rrc_connection_setup(encoded_pdu);
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_invalid_ap_srs_0_msg);
  encoded_pdu.value = llc_invalid_ap_srs_0_msg;
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nInvalid Reconfig with Ded Config TDD SRS 0 AP Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInvalidReconfigDedConfigTddSrs0ApTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcValidateSrsApCfgTest);

void LteRrcLlcValidateSrsApCfgTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params;
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
}

void LteRrcLlcValidateSrsApCfgTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  //lte_rrc_llc_test_rrc_connection_setup_default();
  encoded_pdu.length = sizeof(llc_conn_setup_default_ies);
  encoded_pdu.value = llc_conn_setup_default_ies;

  lte_rrc_llc_test_rrc_connection_setup(encoded_pdu);
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_invalid_srs_aperiodic_msg);
  encoded_pdu.value = llc_invalid_srs_aperiodic_msg;
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcValidateSrsApCfgTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcValidateValidSrsApCfgFromConnSetupTest);

void LteRrcLlcValidateValidSrsApCfgFromConnSetupTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params;
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
}

void LteRrcLlcValidateValidSrsApCfgFromConnSetupTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
  lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
  uint32 buf_len;
  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  //lte_rrc_llc_test_rrc_connection_setup_default();
  encoded_pdu.length = sizeof(llc_conn_setup_with_srs_ap);
  encoded_pdu.value = llc_conn_setup_with_srs_ap;

  lte_rrc_llc_test_rrc_connection_setup(encoded_pdu);
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_valid_srs_aperiodic_msg);
  encoded_pdu.value = llc_valid_srs_aperiodic_msg;
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);
  TF_MSG("\nLLC: Rcvd CPHY_DEDICATED_CFG_REQ \n");

  /* After CPHY Config req is sent, assert LLC is in WAIT_FOR_CPHY_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  TF_MSG("\nCPHY_DEDICATED_CFG_REQ rcvd and LLC Module in WAIT_FOR_CPHY_CFG_CNF state\n");

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  /* Setting the fields in the LTE_CPHY_DEDICATED_CFG_CNF sent by CPHY, The bits
    set in dedicated_cfg_validity field reflect the channel config that were set
    in the llc_conn_setup_msg in the lte_rrc_llc_ota_msg.h */
  cphy_dedicated_cfg_cnf.dedicated_cfg_validity = cphy_dedicated_cfg_req_ptr->dedicated_cfg_param.cfg_mask;
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();
  
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcValidateValidSrsApCfgFromConnSetupTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcValidateInValidSrsApCfgFromConnSetupTest);

void LteRrcLlcValidateInValidSrsApCfgFromConnSetupTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params;
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
}

void LteRrcLlcValidateInValidSrsApCfgFromConnSetupTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  //lte_rrc_llc_test_rrc_connection_setup_default();
  encoded_pdu.length = sizeof(llc_conn_setup_with_srs_ap);
  encoded_pdu.value = llc_conn_setup_with_srs_ap;

  lte_rrc_llc_test_rrc_connection_setup(encoded_pdu);
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_invalid_srs_aperiodic_msg);
  encoded_pdu.value = llc_invalid_srs_aperiodic_msg;
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcValidateInValidSrsApCfgFromConnSetupTest::Teardown()
{
  lte_rrc_utf_teardown();
}



