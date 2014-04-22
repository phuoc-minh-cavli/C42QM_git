/*!
  @file lte_rrc_llc_invalid_ho_before_drb_setup_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of HO related
  configuration when a UE receives a Reconfig message containing mobility 
  information before a DRB has been setup. To start with UE receives a Connection Setup 
  followed by Connection Reconfiguration message that sets up SRB2. 

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
lte_rrc_llc_invalid_ho_before_drb_setup_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/28/09   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_sib.h"
#include "lte_rrc_csp.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h>  
/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidHObeforeDRBSetupTest);

void LteRrcLlcInvalidHObeforeDRBSetupTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcInvalidHObeforeDRBSetupTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update(); 
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb1_setup_srb2();
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg);
  encoded_pdu.value = llc_reconfig_ho_msg;
  TF_MSG("\nConnection Reconfiguration with mobility before DRB setup Test Begins \n");
  lte_rrc_llc_test_ho_validation_failure(encoded_pdu);
  TF_MSG("\nConnection Reconfiguration with mobility before DRB setup Test Ends \n");
  lte_rrc_utf_reset(); 
}

void LteRrcLlcInvalidHObeforeDRBSetupTest::Teardown()
{
  lte_rrc_utf_teardown();
}













