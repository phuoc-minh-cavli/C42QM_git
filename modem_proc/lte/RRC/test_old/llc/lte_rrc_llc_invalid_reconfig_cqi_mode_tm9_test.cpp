
/*!
  @file lte_rrc_llc_invalid_reconfig_cqi_mode_tm9_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing TM9 related configurations

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
#include "lte_rrc_llci.h"
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
TF_PROTO_STUB(uint8, lte_rrc_cap_get_fgi_bit, uint8);
}

static lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
static lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
static uint32 buf_len;

/*===========================================================================

                                UNIT TEST

===========================================================================*/


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI105Enabled);

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI105Enabled::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI105Enabled::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_pmi_ri_enabled);
  encoded_pdu.value = llc_reconfig_tm9_pmi_ri_enabled;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

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

  TF_MSG("\nValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI105Enabled::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI110Disabled);

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI110Disabled::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;


   TF_STUB(lte_rrc_cap_get_fgi_bit) = 
     lte_rrc_llc_test_get_fgi_bit_disable_110;

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI110Disabled::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_fgi_110_config);
  encoded_pdu.value = llc_reconfig_tm9_fgi_110_config;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI110Disabled::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI109Disabled);

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI109Disabled::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;


   TF_STUB(lte_rrc_cap_get_fgi_bit) = 
     lte_rrc_llc_test_get_fgi_bit_disable_109;

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI109Disabled::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_fgi_109_config);
  encoded_pdu.value = llc_reconfig_tm9_fgi_109_config;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI109Disabled::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI110Enabled);

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI110Enabled::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI110Enabled::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_fgi_110_config);
  encoded_pdu.value = llc_reconfig_tm9_fgi_110_config;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

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

  TF_MSG("\nValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI110Enabled::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI109Enabled);

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI109Enabled::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI109Enabled::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_fgi_109_config);
  encoded_pdu.value = llc_reconfig_tm9_fgi_109_config;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

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

  TF_MSG("\nValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI109Enabled::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI105Disabled);

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI105Disabled::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

   TF_STUB(lte_rrc_cap_get_fgi_bit) = 
     lte_rrc_llc_test_get_fgi_bit_disable_105;
}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI105Disabled::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_pmi_ri_enabled);
  encoded_pdu.value = llc_reconfig_tm9_pmi_ri_enabled;
  /* Send LTE_RRC_CFG_REQI to LLC */
 
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI105Disabled::Teardown()
{
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI105Disabled);

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI105Disabled::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

   TF_STUB(lte_rrc_cap_get_fgi_bit) = 
     lte_rrc_llc_test_get_fgi_bit_disable_105;
}

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI105Disabled::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_pmi_ri_enabled_ports_2);
  encoded_pdu.value = llc_reconfig_tm9_pmi_ri_enabled_ports_2;
  /* Send LTE_RRC_CFG_REQI to LLC */
 
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

 
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI105Disabled::Teardown()
{
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI105Disabled2);

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI105Disabled2::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

   TF_STUB(lte_rrc_cap_get_fgi_bit) = 
     lte_rrc_llc_test_get_fgi_bit_disable_105;

}

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI105Disabled2::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_pmi_ri_disabled_ports_1);
  encoded_pdu.value = llc_reconfig_tm9_pmi_ri_disabled_ports_1;
  /* Send LTE_RRC_CFG_REQI to LLC */
 
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

 
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI105Disabled2::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcReconfigDedConfigCqiModeTM9TestForFGI106);

void LteRrcLlcReconfigDedConfigCqiModeTM9TestForFGI106::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

   TF_STUB(lte_rrc_cap_get_fgi_bit) = 
     lte_rrc_llc_test_get_fgi_bit_1;	

}

void LteRrcLlcReconfigDedConfigCqiModeTM9TestForFGI106::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_pmi_ri_enabled_ports_8);
  encoded_pdu.value = llc_reconfig_tm9_pmi_ri_enabled_ports_8;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

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

  TF_MSG("\nValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr(); 
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcReconfigDedConfigCqiModeTM9TestForFGI106::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcReconfigDedConfigCqiModeTM9TestForFGI106Disabled);

void LteRrcLlcReconfigDedConfigCqiModeTM9TestForFGI106Disabled::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

   TF_STUB(lte_rrc_cap_get_fgi_bit) = 
     lte_rrc_llc_test_get_fgi_bit_disable_106;	 

}

void LteRrcLlcReconfigDedConfigCqiModeTM9TestForFGI106Disabled::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_pmi_ri_enabled_ports_8);
  encoded_pdu.value = llc_reconfig_tm9_pmi_ri_enabled_ports_8;
  /* Send LTE_RRC_CFG_REQI to LLC */
 
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();		 
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcReconfigDedConfigCqiModeTM9TestForFGI106Disabled::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI107Enabled);

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI107Enabled::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI107Enabled::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_cqi_aperiodic_m30_antport_1();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_aperiodic_rm20);
  encoded_pdu.value = llc_reconfig_tm9_aperiodic_rm20;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

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

  TF_MSG("\nValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI107Enabled::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI107Disabled);

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI107Disabled::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

     TF_STUB(lte_rrc_cap_get_fgi_bit) = 
     lte_rrc_llc_test_get_fgi_bit_disable_107;
}

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI107Disabled::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_cqi_aperiodic_m30_antport_1();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_aperiodic_rm20);
  encoded_pdu.value = llc_reconfig_tm9_aperiodic_rm20;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nInValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI107Disabled::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI107Disabled2);

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI107Disabled2::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

     TF_STUB(lte_rrc_cap_get_fgi_bit) = 
     lte_rrc_llc_test_get_fgi_bit_disable_107;
}

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI107Disabled2::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_cqi_aperiodic_m30_antport_1();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_aperiodic_rm22_pmiri_enabled_anport_4);
  encoded_pdu.value = llc_reconfig_tm9_aperiodic_rm22_pmiri_enabled_anport_4;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nInValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI107Disabled2::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI107Disabled);

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI107Disabled::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

     TF_STUB(lte_rrc_cap_get_fgi_bit) = 
     lte_rrc_llc_test_get_fgi_bit_disable_107;
}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI107Disabled::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_cqi_aperiodic_m30_antport_1();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_pmiri_enable_aperiodic_rm30);
  encoded_pdu.value = llc_reconfig_tm9_pmiri_enable_aperiodic_rm30;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

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

  TF_MSG("\nInValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI107Disabled::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI108Enabled);

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI108Enabled::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
	
	TF_STUB(lte_rrc_cap_get_fgi_bit) = 
		lte_rrc_llc_test_get_fgi_bit_1;

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI108Enabled::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_cqi_aperiodic_m30_antport_1();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_aperiodic_rm22_pmiri_enabled_anport_8);
  encoded_pdu.value = llc_reconfig_tm9_aperiodic_rm22_pmiri_enabled_anport_8;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

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

  TF_MSG("\nValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcValidReconfigDedConfigCqiModeTM9TestFGI108Enabled::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI108Disabled);

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI108Disabled::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

     TF_STUB(lte_rrc_cap_get_fgi_bit) = 
     lte_rrc_llc_test_get_fgi_bit_disable_108;
}

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI108Disabled::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nInvalid Reconfig with Ded Config CQI Mode TM9 Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_cqi_aperiodic_m30_antport_1();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_reconfig_tm9_aperiodic_rm22_pmiri_enabled_anport_8);
  encoded_pdu.value = llc_reconfig_tm9_aperiodic_rm22_pmiri_enabled_anport_8;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nInValid Reconfig with Ded Config CQI Mode TM9 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInValidReconfigDedConfigCqiModeTM9TestFGI108Disabled::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcCqiPeriodicModeAbsentForTm9);

void LteRrcLlcCqiPeriodicModeAbsentForTm9::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();

  TF_STUB(lte_rrc_get_3gpp_release_version) =
     lte_rrc_llc_test_get_3gpp_release_version_for_rel10;


   TF_STUB(lte_rrc_cap_get_fgi_bit) = 
     lte_rrc_llc_test_get_fgi_bit_disable_109;

}

void LteRrcLlcCqiPeriodicModeAbsentForTm9::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;

  lte_rrc_llc_test_basic_sib2_update();
  /* Doing connection setup followed by the Reconfig message containing 
    duplicate DRB configuration */
  lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 

  encoded_pdu.length = sizeof(llc_invalid_tm9_csi_mode);
  encoded_pdu.value = llc_invalid_tm9_csi_mode;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */

  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcCqiPeriodicModeAbsentForTm9::Teardown()
{
  lte_rrc_utf_teardown();
}



