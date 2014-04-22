/*!
  @file lte_rrc_llc_cqi_fmt_indi_validate_test.cpp

  @brief
  Unit test code for the LLC procedure that tests to validate CQI report periodic Format Indicator against BW.

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

$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/llc/lte_rrc_llc_cqi_fmt_indi_validate_test.cpp

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/10   md     Initial Revision
           
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
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_camped_cell_bw, lte_bandwidth_e *);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcCQIFmtIndiValidateTest);

void LteRrcLlcCQIFmtIndiValidateTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcCQIFmtIndiValidateTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_drx_opt_req_s *drx_opt_req_ptr = NULL;

  TF_MSG("\nLLC CQI config test begins \n");
  lte_rrc_llc_test_basic_sib2_update();

  /* Sending a Conn setup that configures periodic CQI */
  encoded_pdu.length = sizeof(llc_conn_setup_msg);
  encoded_pdu.value = llc_conn_setup_msg;

  lte_rrc_llc_test_rrc_connection_setup(encoded_pdu);

  TF_STUB(lte_rrc_csp_get_camped_cell_bw) = lte_rrc_llc_test_csp_get_camped_cell_bw1;

  /* Send Reconfig messages that doesn't include CQIConfig */
  lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_dl();

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_phy_mac_drb3_rlc_msg);
  encoded_pdu.value = llc_reconfig_phy_mac_drb3_rlc_msg;  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);

  TF_MSG("\nLLC CQI config test ends \n");
  lte_rrc_utf_reset(); 

}

void LteRrcLlcCQIFmtIndiValidateTest::Teardown()
{
  lte_rrc_utf_teardown();
}

