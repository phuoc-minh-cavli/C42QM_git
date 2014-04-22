/*!
  @file lte_rrc_llc_full_cycle_release_drb3_drb4_drb5_test.cpp

  @brief
  Full Call cycle prcessing - 
  Unit test code for the LLC procedure that tests the processing of SIB2 update,
  Connection Setup, Connection Reconfiguration message that sets up a DRB3, 
  followed by another Reconfiguration message that reconfigures SRB2 and DRB3 
  and sets up DRB4 and DRB5. Another Reconfiguration message releases DRB3, DRB4
  and DRB5. Finally connection release message that releases the RRC connection.

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
lte_rrc_llc_full_cycle_release_drb3_drb4_drb5_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/10/08   mm     Initial Revision
           
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

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcFullCycleReleaseDrb3Drb4Drb5Test);

void LteRrcLlcFullCycleReleaseDrb3Drb4Drb5Test::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcFullCycleReleaseDrb3Drb4Drb5Test::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nFull Cycle - Connection Reconfiguration with DRB3, DRB4 and DRB5 Release Test Begins \n");
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  lte_rrc_llc_test_reconfig_drb3_setup_drb4_drb5_um_bi();
  /* Releasing the DRB3, DRB4 and DRB5 */
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_drb3_drb4_drb5_rel_msg);
  encoded_pdu.value = llc_reconfig_drb3_drb4_drb5_rel_msg;

  /* On receiving a Config Req from Config, LLC should be in the INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  lte_rrc_llc_test_release(LTE_RRC_PROC_CONFIG, FALSE, 0);
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);
  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_conn_release(TRUE);
  TF_MSG("\nFull Cycle - Connection Reconfiguration with DRB3, DRB4 and DRB5 Release Test Ends \n");
  lte_rrc_utf_reset(); 
}

void LteRrcLlcFullCycleReleaseDrb3Drb4Drb5Test::Teardown()
{
  lte_rrc_utf_teardown();
}










