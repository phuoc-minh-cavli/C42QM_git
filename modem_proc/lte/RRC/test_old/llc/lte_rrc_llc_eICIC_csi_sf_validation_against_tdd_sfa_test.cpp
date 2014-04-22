
/*!
  @file lte_rrc_llc_eICIC_csi_sf_validation_against_tdd_sfa_test.cpp

  @brief
  Unit test code for the LLC procedure that validates eICIC CSI Subframe pattern
  against Subframe assignment for TDD cell

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
07/16/14   md     Initial Revision
           
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
TF_PROTO_STUB(lte_rrc_div_duplex_e, lte_rrc_csp_get_last_camped_mode, void);

}

static lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
static lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
static uint32 buf_len;

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidCsiSubfmForTddSaTest);

void LteRrcLlcInvalidCsiSubfmForTddSaTest::Setup()
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

void LteRrcLlcInvalidCsiSubfmForTddSaTest::Test()
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

  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;

  encoded_pdu.length = sizeof(llc_eicic_TDD_invalid);
  encoded_pdu.value = llc_eicic_TDD_invalid;
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

}

void LteRrcLlcInvalidCsiSubfmForTddSaTest::Teardown()
{
  lte_rrc_utf_teardown();
}

