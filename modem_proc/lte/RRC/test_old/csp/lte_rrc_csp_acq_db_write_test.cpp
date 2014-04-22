/*!
  @file lte_rrc_csp_acq_db_write_test.cpp

  @brief
  Write acq_db structure to EFS

*/

/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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
05/14/10   vatsac  Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include <lte_rrc_csp.h>
#include <lte_rrc_utils.h>
#include <fs_public.h>
}

#include <TestFramework.h>

typedef struct acq_db_entry
{
    lte_earfcn_t earfcn;
    sys_sband_lte_e_type band;
    lte_rrc_plmn_s plmn;
} lte_rrc_csp_test_acq_db_entry_s;

static lte_rrc_csp_test_acq_db_entry_s entry[] =
         {{200, SYS_SBAND_LTE_EUTRAN_BAND1, {0,0,1,2,0,1,0}}, 
          {250, SYS_SBAND_LTE_EUTRAN_BAND1, {0,0,1,2,0,1,0}},
          {300, SYS_SBAND_LTE_EUTRAN_BAND1, {0,0,1,2,0,1,0}},
          {348, SYS_SBAND_LTE_EUTRAN_BAND1, {0,0,1,2,0,1,0}},
          {400, SYS_SBAND_LTE_EUTRAN_BAND1, {0,0,1,2,0,1,0}},
          {450, SYS_SBAND_LTE_EUTRAN_BAND1, {0,0,1,2,0,1,0}},
          {0, SYS_SBAND_LTE_EUTRAN_BAND1, {1,2,3,3,4,5,6}},
          {0, SYS_SBAND_LTE_EUTRAN_BAND1, {1,2,3,3,4,5,6}},
          {0, SYS_SBAND_LTE_EUTRAN_BAND1, {1,2,3,3,4,5,6}},
          {0, SYS_SBAND_LTE_EUTRAN_BAND1, {1,2,3,3,4,5,6}}};

//structures
static lte_rrc_csp_acq_db_s acq_db;
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCspAcqDbWriteTest);

void LteRrcCspAcqDbWriteTest::Setup()
{
  return;
}

void LteRrcCspAcqDbWriteTest::Test()
{
  uint32 i;
  int result;
  char str_buf[256];

  TF_MSG("Start of test");

  for (i = 0; i < LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES; i++)
  {
      acq_db.db[i].band = entry[i].band;
      acq_db.db[i].bandwidth = LTE_BW_NRB_6;
      acq_db.db[i].dl_earfcn = entry[i].earfcn;
      acq_db.db[i].phy_cell_id = i+1;
      acq_db.db[i].cell_identity = i+1;
      acq_db.db[i].tac = 0;
      acq_db.db[i].plmn_list.num_plmns = 1;
      acq_db.db[i].plmn_list.plmn[0] = entry[i].plmn;
      acq_db.db[i].sib_db_key = 1;
      acq_db.db[i].selected_plmn = entry[i].plmn;
      acq_db.db[i].selected_plmn_index = 0;

      acq_db.acq_db_index[i] = i;
      acq_db.acq_db_count++;

      if (entry[i].earfcn)
      {
          acq_db.entry_is_occupied[i] = TRUE;
      }
      else
      {
          acq_db.entry_is_occupied[i] = FALSE;
      }

      TF_MSG("Index array Acq DB Index = %d, DL EARFCN = %d BAND = %d PLMN = %s", 
             i, acq_db.db[i].dl_earfcn, acq_db.db[i].band,
             lte_rrc_get_plmn_as_str(&acq_db.db[i].selected_plmn,
                                     str_buf, sizeof(str_buf)));
  }

  TF_MSG("The # of AcqDB entries = %d", acq_db.acq_db_count);

  TF_MSG("Acq DB size = %d", sizeof(acq_db));

  /* write acq db to EFS, erasing previous contents */
  result = efs_put("./acq_db" /*"/nv/reg_files/modem/lte/rrc/csp/acq_db" */, 
                   &acq_db,
                   sizeof(acq_db),
                   O_RDWR|O_AUTODIR|O_TRUNC,
                   0777);

  TF_ASSERT(0 == result);

  TF_MSG("CSP: Wrote acq db to EFS %d bytes", sizeof(acq_db));

  TF_MSG("End of test");
}

void LteRrcCspAcqDbWriteTest::Teardown()
{
  return;
}

