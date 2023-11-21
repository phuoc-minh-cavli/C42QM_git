/*!
  @file DsmPushdownTest.cpp

  @brief Testing dsm_pushdown()

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

$Header: //components/rel/core.mpss/3.10/memory/dsm/test/DsmPushdownTest.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/31/09   ag      Initial Revision.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "TestFramework.h"

extern "C" {
  #include "dsm.h"
  #include "tf_dsm_main.h"
}

/*===========================================================================

                      DEFINITIONS AND DECLARATIONS 

===========================================================================*/

TF_DEFINE_BASIC_TEST_CASE(COMPONENT, DsmPushdownTest);

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           FUNCTIONS DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
void DsmPushdownTest::Test()
{
  dsm_item_type * tmp1 = NULL;
  char buf[6] = "Hello"; 

  TF_MSG("dsm_init");
  dsm_init();

  TF_MSG("dsm_pushdown_packed(DSM_DS_SMALL_ITEM_POOL)");
  dsm_pushdown(&tmp1, buf, 1, DSM_DS_SMALL_ITEM_POOL);

  TF_MSG("dsm_pushdown returned:tmp1 %x",(int)tmp1);
  TF_ASSERT(tmp1 != NULL);

  TF_MSG("dsm_pushdown(DSM_DS_SMALL_ITEM_POOL)");
  dsm_pushdown(&tmp1, &(buf[1]), 1, DSM_DS_SMALL_ITEM_POOL);

  TF_MSG("dsm_pushdown returned:tmp1 %x",(int)tmp1);
  TF_ASSERT(tmp1 != NULL);

  tf_dsm_print_packet(tmp1);
  tf_dsm_print_pool(DSM_DS_SMALL_ITEM_POOL);

} /* Test */
