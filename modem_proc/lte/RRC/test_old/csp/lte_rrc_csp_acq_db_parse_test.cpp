/*!
  @file lte_rrc_csp_acq_db_parse_test.cpp

  @brief
  Drop the acq db file into ./lte/RRC/test/csp/lte_acq_db (relative to
  build root). Then run qtf -conf=QOLTA -o test -r -t LteRrcCspAcqDbParseTest

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

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/23/09   dineshd Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include <lte_rrc_csp.h>
}

#include <TestFramework.h>

//structures
static lte_rrc_csp_acq_db_s acq_db;
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(NO_RUN, LteRrcCspAcqDbParseTest);

void LteRrcCspAcqDbParseTest::Setup()
{
  return;
}

void LteRrcCspAcqDbParseTest::Test()
{
  FILE *fp = NULL;
  int n;
  uint32 i;
  uint32 index;

  TF_MSG("Start of test");

  fp = fopen("./lte/RRC/test/csp/lte_acq_db", "r");
  n = fread(&acq_db,1,sizeof(acq_db),fp);

  TF_MSG("# of bytes read = %d", n);

  TF_ASSERT(sizeof(acq_db) == n);

  fclose(fp);

  TF_MSG("Acq DB size = %d", sizeof(acq_db));

  TF_MSG("The # of AcqDB entries = %d", acq_db.acq_db_count);

  TF_ASSERT(acq_db.acq_db_count < LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES);

  for (i = 0; i < acq_db.acq_db_count; i++)
  {
    index = acq_db.acq_db_index[i];

    TF_ASSERT(index < LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES);

    TF_MSG("Index array i = %d, Acq DB Index = %d, DL EARFCN = %d", 
           i,index,acq_db.db[index].dl_earfcn);
  }

  TF_MSG("End of test");
}

void LteRrcCspAcqDbParseTest::Teardown()
{
  return;
}

