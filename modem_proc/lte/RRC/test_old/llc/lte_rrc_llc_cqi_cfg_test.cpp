/*!
  @file lte_rrc_llc_cqi_cfg_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the handling of CQI config in
  connection setup, reconfiguration, handover and out of sync scenarios.

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

$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/llc/lte_rrc_llc_cqi_cfg_test.cpp

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/10   mm     Initial Revision
           
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
#include <tf_stub.h>  

extern "C"
{
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);  
  extern lte_rrc_llc_s lte_rrc_llc_data;
}


/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcCQICfgTest);

void LteRrcLlcCQICfgTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcCQICfgTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;

  TF_MSG("\nLLC CQI config test begins \n");
  lte_rrc_llc_test_basic_sib2_update();

  /* Sending a Conn setup that configures periodic CQI */
  encoded_pdu.length = sizeof(llc_conn_setup_msg);
  encoded_pdu.value = llc_conn_setup_msg;

  lte_rrc_llc_test_rrc_connection_setup(encoded_pdu);

  /* Send Reconfig messages that doesn't include CQIConfig */
  lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_dl();
  lte_rrc_llc_test_reconfig_phy_mac_drb3_rlc();

  /* Send a Reconfig message triggering intra-freq HO */ 
  encoded_pdu.length = sizeof(llc_reconfig_intra_freq_mobility_info_only_msg);
  encoded_pdu.value = llc_reconfig_intra_freq_mobility_info_only_msg;
  lte_rrc_llc_test_intra_freq_mobility_only(encoded_pdu, TRUE);

  /* Send a Reconfig message that configures periodicCQI in the target cell */
  lte_rrc_llc_test_reconfig_phyded_cfg(FALSE);

  /* Send LTE_MAC_RELEASE_RESOURCES_IND and test the LLC handling of CQI Config */
  lte_rrc_llc_test_mac_release_resources_ind();

  TF_MSG("\nLLC CQI config test ends \n");
  lte_rrc_utf_reset(); 

}

void LteRrcLlcCQICfgTest::Teardown()
{
  lte_rrc_utf_teardown();
}

// CQI rel10 to rel8 during HO and rel8 applied
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcCQIRevertToR8DuringHO);

void LteRrcLlcCQIRevertToR8DuringHO::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel11;
}

void LteRrcLlcCQIRevertToR8DuringHO::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  int decode_status=0;

  TF_MSG("\nLLC CQI config test begins \n");
  lte_rrc_llc_test_basic_sib2_update();

  /* Sending a Conn setup that configures periodic CQI */
  lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband();

  /* Send Reconfig messages that doesn't include CQIConfig */
  lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_dl();
  lte_rrc_llc_test_reconfig_phy_mac_drb3_rlc();

  /* Send a Reconfig message triggering intra-freq HO */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg21);
  encoded_pdu.value = llc_reconfig_ho_msg21;
  
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(lte_rrc_llc_test_common_data.dl_dcch_msg_ptr != NULL);

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /* Verify PDCP UL RECFG PREP REQ */
  lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();
  
  
  TF_MSG("\nLLC CQI config test ends \n");
  lte_rrc_utf_reset(); 

}

void LteRrcLlcCQIRevertToR8DuringHO::Teardown()
{
  lte_rrc_utf_teardown();
}



