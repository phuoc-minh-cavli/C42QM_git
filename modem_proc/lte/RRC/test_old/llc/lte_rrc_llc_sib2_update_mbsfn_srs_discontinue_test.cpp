/*!
  @file lte_rrc_llc_sib2_update_srs_discontinue_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a LTE_RRC_CFG_REQI with SIB2 update where MBSFN is included and SRS Common 
  Config is not present. In this case the CPHY_COMMON_REQ sent by LLC should 
  indicate the disabling of SRS Config Common so that CPHY will discontinue 
  using the config they have.

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
03/17/08   da     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_llc.h"
#include "lte_rrc_sib.h"
#include "lte_static_rrc_mem_util.h"
}

#include "lte_rrc_llc_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>  

extern "C"
{
  TF_PROTO_STUB(void *, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
}

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcSib2UpdateMBSFNSrsDiscontinueTest);

void LteRrcLlcSib2UpdateMBSFNSrsDiscontinueTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_srs_get_serv_cell_sib;
}

void LteRrcLlcSib2UpdateMBSFNSrsDiscontinueTest::Test()
{
  lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg();
  lte_rrc_utf_reset(); 
}

void LteRrcLlcSib2UpdateMBSFNSrsDiscontinueTest::Teardown()
{
  lte_rrc_utf_teardown();
}








