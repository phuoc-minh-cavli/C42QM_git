/*!
  @file lte_rrc_llc_ho_with_pdcp_modify_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of HO related
  configuration when a UE receives a Reconfig message containing intra-freq mobility 
  information and PDCP reconfiguration changing status report required field. 
  To start receives a Connection Setup followed by Connection Reconfiguration 
  message that sets up an AM DRB, before handover.

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
lte_rrc_llc_ho_with_pdcp_modify_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/24/08   mm     Initial Revision
           
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

                         INTERNAL VARIABLES

===========================================================================*/
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcHOWithPDCPModifyTest);

void LteRrcLlcHOWithPDCPModifyTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcHOWithPDCPModifyTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_setup_srb2_drb3_am();
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */
  encoded_pdu.length = sizeof(llc_reconfig_ho_pdcp_modify_msg);
  encoded_pdu.value = llc_reconfig_ho_pdcp_modify_msg;
  lte_rrc_llc_test_intra_freq_mobility_only(encoded_pdu, TRUE);
  lte_rrc_utf_reset(); 
}

void LteRrcLlcHOWithPDCPModifyTest::Teardown()
{
  lte_rrc_utf_teardown();
}








