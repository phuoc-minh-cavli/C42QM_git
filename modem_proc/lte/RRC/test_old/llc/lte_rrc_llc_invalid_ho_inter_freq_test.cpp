/*!
  @file lte_rrc_llc_invalid_ho_inter_freq_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of HO related
  configuration when a UE receives a Reconfig message containing mobility 
  information for inter-freq handover. To start with UE receives a Connection Setup 
  followed by Connection Reconfiguration message that sets up a UM Bi-directional 
  DRB, before handover. Inter-freq handovers are not supported in Rel 1.6.

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
lte_rrc_llc_invalid_ho_inter_freq_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/02/09   mm     Initial Revision
           
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
  TF_PROTO_STUB(boolean, lte_rrc_cap_inter_freq_is_supported, void);
}

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcInvalidHOInterFreqTest);

void LteRrcLlcInvalidHOInterFreqTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  /* Set inter-freq as not supported */
  TF_STUB(lte_rrc_cap_inter_freq_is_supported) = lte_rrc_llc_test_cap_inter_freq_is_not_supported;
}

void LteRrcLlcInvalidHOInterFreqTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update(); 
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_invalid_ho_inter_freq_msg);
  encoded_pdu.value = llc_invalid_ho_inter_freq_msg;
  TF_MSG("\nConnection Reconfiguration Invalid HO - Inter Freq Test Begins \n");
  lte_rrc_llc_test_ho_validation_failure(encoded_pdu);
  TF_MSG("\nConnection Reconfiguration Invalid HO - Inter Freq Test Test Ends \n");
  lte_rrc_utf_reset(); 
}

void LteRrcLlcInvalidHOInterFreqTest::Teardown()
{
  lte_rrc_utf_teardown();
}











