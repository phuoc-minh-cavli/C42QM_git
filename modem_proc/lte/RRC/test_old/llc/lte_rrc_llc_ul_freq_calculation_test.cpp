/*!
  @file lte_rrc_llc_ul_freq_calculation_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the calculation of UL freq
  from DL freq.
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
11/16/09   mm     Initial Revision
           
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

/*===========================================================================

                                INTERNAL DEFINITIONS

===========================================================================*/
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcULFreqCalculationTest);

void LteRrcLlcULFreqCalculationTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL,0);
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcULFreqCalculationTest::Test()
{

  lte_earfcn_t dl_earfcn = 0, ul_earfcn = 0;

  TF_MSG("\nLLC: UL freq calculation test begins \n");

  dl_earfcn = 45590;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));

  dl_earfcn = 500;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 18500);

  dl_earfcn = 1000;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 19000);

  dl_earfcn = 1500;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 19500);

  dl_earfcn = 2000;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 20000);

  dl_earfcn = 2500;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 20500);

  dl_earfcn = 2700;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 20700);

  dl_earfcn = 3000;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 21000);

  dl_earfcn = 3500;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 21500);

  dl_earfcn = 4000;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 22000);

  dl_earfcn = 4500;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 22500);

  dl_earfcn = 4800;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 22800);

  dl_earfcn = 5010;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 23010);

  dl_earfcn = 5200;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 23200);

  dl_earfcn = 5300;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 23300);

  dl_earfcn = 5800;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 23800);

  dl_earfcn = 5900;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 23900);

  dl_earfcn = 6000;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 24000);

  dl_earfcn = 6200;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 24200);

  dl_earfcn = 6500;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 24500);

  dl_earfcn = 7800;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 25800);

  dl_earfcn = 8100;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 26100);

  dl_earfcn = 38000;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  TF_ASSERT(ul_earfcn == 38000);

  dl_earfcn = 30000;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_ul_earfcn(dl_earfcn,&ul_earfcn));
  
  TF_MSG("\nLLC: UL freq calculation test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcULFreqCalculationTest::Teardown()
{
  lte_rrc_utf_teardown();
}











