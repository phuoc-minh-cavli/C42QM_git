/*!
  @file lte_rrc_llc_invalid_ho_to_mimo_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of HO to MIMO
  with an invalid config for Transmission mode.

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
08/09/10   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_static_rrc_mem_util.h"

}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h>  

extern "C"
{
  TF_PROTO_STUB(uint8, lte_rrc_csp_get_num_tx_antennas, void);
}
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidHOToMIMOTest);

void LteRrcLlcInvalidHOToMIMOTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  /* Set source cell to MIMO */
  TF_STUB(lte_rrc_csp_get_num_tx_antennas) = lte_rrc_llc_test_csp_get_num_tx_antennas_simo;
}

void LteRrcLlcInvalidHOToMIMOTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\n Invalid HO to MIMO Test Begins \n");

  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();

  /* Set the num_tx_antenna to MIMO to denote HO to MIMO cell */
  TF_STUB(lte_rrc_csp_get_num_tx_antennas) = lte_rrc_llc_test_csp_get_num_tx_antennas_mimo;

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_reconfig_ho_delta_config_msg);
  encoded_pdu.value = llc_reconfig_ho_delta_config_msg;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\n Invalid HO to MIMO Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcLlcInvalidHOToMIMOTest::Teardown()
{
  lte_rrc_utf_teardown();
}





