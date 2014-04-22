/*!
  @file lte_rrc_llc_rohc_reset_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of HO ROHC reset
  R11 OTA field

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
lte_rrc_llc_rohc_reset_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/19/14   md     Initial Revision
           
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
  TF_PROTO_STUB(boolean, lte_rrc_cap_get_r11_cap_bit, uint8);
  TF_PROTO_STUB(boolean, lte_rrc_config_ho_in_progress, void);
}
extern "C"
{
extern lte_rrc_llc_s lte_rrc_llc_data;
}


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcIRohcResetR11Test);

void LteRrcLlcIRohcResetR11Test::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_cap_get_r11_cap_bit) =
     lte_rrc_llc_test_get_rohc_enabled;
  TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_llc_test_get_ho_in_progress;
}

void LteRrcLlcIRohcResetR11Test::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update(); 
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi(); 
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->rohc_reset == TRUE);

  // Trigger HO with ROHC reset
  encoded_pdu.length = sizeof(llc_rohc_reset_ho_inter_freq_msg);
  encoded_pdu.value = llc_rohc_reset_ho_inter_freq_msg;
  lte_rrc_llc_test_rohc_reset(encoded_pdu);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->rohc_reset == FALSE);

  // Trigger HO without ROHC Reset
  encoded_pdu.length = sizeof(llc_wo_rohc_reset_ho_inter_freq_msg);
  encoded_pdu.value = llc_wo_rohc_reset_ho_inter_freq_msg;
  lte_rrc_llc_test_rohc_reset(encoded_pdu);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->rohc_reset == TRUE);

  // Trigger RLF
  lte_rrc_llc_test_rohc_reset_config_failure();
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->rohc_reset == TRUE);
  lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(LTE_CPHY_COMMON_CFG_CURRENT_VALID_MASK_BITS);
  lte_rrc_llc_test_basic_sib2_update(); 
  lte_rrc_llc_test_rrc_connection_reestablishment();
  
  lte_rrc_utf_reset(); 
}

void LteRrcLlcIRohcResetR11Test::Teardown()
{
  lte_rrc_utf_teardown();
}

