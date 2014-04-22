/*!
  @file lte_rrc_plmn_search_guard_timer_expiry_test.cpp

  @brief
  Unit test code for taking PLMN_SEARCH to camped state.
  Controller is in Inactive state and there is nothing in the Acq DB. Band
  Scan is started

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

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/22/09   vatsac  Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include <lte_rrc_int_msg.h>
#include <lte_rrc_ext_msg.h>
#include <lte_cphy_msg.h>
#include <__lte_rrc_plmn_search.h>
#include <__lte_rrc_controller.h>
#include <lte_rrc_utils.h>
#include <lte_rrc_plmn_searchi.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

}

#include <TestFramework.h>
#include <lte_rrc_utf.h>
#include "lte_rrc_plmn_search_test_common.h"

extern "C"
{
extern lte_rrc_plmn_search_static_data_s lte_rrc_plmn_search_s_data;
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPlmnSearchEfsWrite);

void LteRrcPlmnSearchEfsWrite::Setup()
{
}

void LteRrcPlmnSearchEfsWrite::Test()
{
    size_t n;
    FILE *fp = 0;

    lte_rrc_plmn_search_s_data.control.l_to_l_disabled = 0;
#if 0    
    lte_rrc_plmn_search_s_data.control.l_to_w_disabled = 0;
    lte_rrc_plmn_search_s_data.control.l_to_tds_disabled = 0;
    lte_rrc_plmn_search_s_data.control.l_to_g_disabled = 0;
    lte_rrc_plmn_search_s_data.control.gw_to_l_disabled = 0;
#endif    
    lte_rrc_plmn_search_s_data.control.guard_tmri_val = 150000;
    lte_rrc_plmn_search_s_data.control.acq_db_tmri_val = 300000;

    TF_MSG("Acq DB size = %d\n", sizeof(lte_rrc_plmn_search_s_data.control));

    /* need the 'b' on windows, so that it doesnt put escape chars in between
       while writing to the file */
    if ((fp = fopen("./bplmn_control", "wb")) == 0)
    {
        TF_MSG("Error: Could not open file bplmn_control for writing!!!\n");
        TF_ASSERT(0);
    }

    n = fwrite(&lte_rrc_plmn_search_s_data.control, 1, sizeof(lte_rrc_plmn_search_s_data.control), fp);

    fclose(fp);

    TF_MSG("File bplmn_cotrol, bytes wrote =%d\n", n);
}

void LteRrcPlmnSearchEfsWrite::Teardown()
{
}


