/*!
  @file lte_rrc_llc_mac_release_resources_ind_during_ho_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a CPHY_OUT_OF_SYNC_REQ when HO is in progress (ML1 has moved to the target cell
  but successful RACH is yet to happen) message.

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
lte_rrc_llc_mac_release_resources_ind_during_ho_test.cpp

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/10   mm     Initial Revision
           
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
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
  TF_PROTO_STUB(boolean, lte_rrc_cell_is_tdd, lte_earfcn_t);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcMacReleaseResourcesIndDuringHOTest);

void LteRrcLlcMacReleaseResourcesIndDuringHOTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
}

void LteRrcLlcMacReleaseResourcesIndDuringHOTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_mac_release_resources_ind_s mac_rel_resrc_ind;

  uint32 buf_len = 0;

  TF_MSG("\nLLC LTE_MAC_RELEASE_RESOURCES_IND during HO test begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg_tdd);
  encoded_pdu.value = llc_reconfig_ho_msg_tdd;
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;

  /* Trigger HO */
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify PDCP UL RECFG PREP REQ */
  lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

  /* Verify phy handover request parameters when handover succeeds (TRUE) */
  lte_rrc_llc_test_verify_phy_handover_cfg_req_and_send_cnf(TRUE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();

  /* Since HO is in progress, RRC will ignore the mac_release_resources_ind 
     Send an out of sync req to LLC */
  lte_rrc_init_default_hdr(&mac_rel_resrc_ind.hdr, LTE_MAC_RELEASE_RESOURCES_IND);
  lte_rrc_utf_send_msg((byte*)&mac_rel_resrc_ind, sizeof(mac_rel_resrc_ind));

  TF_MSG("\nLLC LTE_MAC_RELEASE_RESOURCES_IND during HO test ends \n");
  lte_rrc_utf_reset(); 

}

void LteRrcLlcMacReleaseResourcesIndDuringHOTest::Teardown()
{
  lte_rrc_utf_teardown();
}




