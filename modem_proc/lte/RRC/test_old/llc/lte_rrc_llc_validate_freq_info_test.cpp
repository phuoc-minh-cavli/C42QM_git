/*!
  @file lte_rrc_llc_validate_freq_info_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the validation for freq info.
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
07/27/09   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "__lte_rrc_llc_int.h"
#include "lte_rrc_llci.h"
#include "lte_rrc_sib.h"
#include "lte_static_rrc_mem_util.h"
}

#include "lte_rrc_llc_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>  

/*===========================================================================

                                INTERNAL DEFINITIONS

===========================================================================*/
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(DISABLE, LteRrcLlcValidateFreqInfoValuesTest);

void LteRrcLlcValidateFreqInfoValuesTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL,0);
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcValidateFreqInfoValuesTest::Test()
{

  lte_earfcn_t dl_earfcn = 0, ul_earfcn = 0;
  boolean ul_earfcn_present = FALSE;

  TF_MSG("\nLLC: Validate freq info test begins \n");
  
  TF_MSG("\nLLC: Validate freq info test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcValidateFreqInfoValuesTest::Teardown()
{
  lte_rrc_utf_teardown();
}










