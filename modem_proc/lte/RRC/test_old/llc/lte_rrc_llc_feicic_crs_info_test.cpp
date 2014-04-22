/*!
  @file lte_rrc_llc_feicic_crs_info_test.cpp

  @brief
  Unit test code for the LLC procedure that tests FeICIC CRS assistance Info IEs

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
lte_rrc_llc_feicic_crs_info_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/14   md     Initial Revision
           
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
extern lte_rrc_llc_s lte_rrc_llc_data;
}


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(DISABLE, LteRrcLlcIFeICICCRSInfoTest);

void LteRrcLlcIFeICICCRSInfoTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcIFeICICCRSInfoTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update(); 
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi(); 
  
  // Trigger HO with ROHC reset
  encoded_pdu.length = sizeof(llc_feicic_crs_info_msg);
  encoded_pdu.value = llc_feicic_crs_info_msg;
  lte_rrc_llc_test_feicic_crs_info(encoded_pdu);

  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.enable == TRUE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.num_crs_rpt == 8);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[0].cell_id == 10);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[0].antenna_ports_count == LTE_L1_ANTENNA_INFO_PORTS_COUNT_2);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[0].mbsfn_subframe_cfg.enable == TRUE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[0].mbsfn_subframe_cfg.num_mbms_allocations == 1);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[0].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_offset == 2);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[0].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_period == LTE_L1_MBSFN_ALLOCATION_PERIOD_2);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[0].mbsfn_subframe_cfg.mbsfn_alloacation[0].sfn_allocation == LTE_L1_MBSFN_ALLOCATION_ONE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[1].cell_id == 20);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[1].antenna_ports_count == LTE_L1_ANTENNA_INFO_PORTS_COUNT_4);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[1].mbsfn_subframe_cfg.enable == TRUE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[1].mbsfn_subframe_cfg.num_mbms_allocations == 1);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[1].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_offset == 5);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[1].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_period == LTE_L1_MBSFN_ALLOCATION_PERIOD_16);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[1].mbsfn_subframe_cfg.mbsfn_alloacation[0].sfn_allocation == LTE_L1_MBSFN_ALLOCATION_ONE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[2].cell_id == 14);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[2].antenna_ports_count == LTE_L1_ANTENNA_INFO_PORTS_COUNT_4);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[2].mbsfn_subframe_cfg.enable == TRUE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[2].mbsfn_subframe_cfg.num_mbms_allocations == 1);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[2].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_offset == 6);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[2].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_period == LTE_L1_MBSFN_ALLOCATION_PERIOD_8);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[2].mbsfn_subframe_cfg.mbsfn_alloacation[0].sfn_allocation == LTE_L1_MBSFN_ALLOCATION_ONE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[3].cell_id == 16);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[3].antenna_ports_count == LTE_L1_ANTENNA_INFO_PORTS_COUNT_1);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[3].mbsfn_subframe_cfg.enable == TRUE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[3].mbsfn_subframe_cfg.num_mbms_allocations == 1);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[3].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_offset == 3);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[3].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_period == LTE_L1_MBSFN_ALLOCATION_PERIOD_16);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[3].mbsfn_subframe_cfg.mbsfn_alloacation[0].sfn_allocation == LTE_L1_MBSFN_ALLOCATION_ONE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[4].cell_id == 18);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[4].antenna_ports_count == LTE_L1_ANTENNA_INFO_PORTS_COUNT_4);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[4].mbsfn_subframe_cfg.enable == TRUE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[4].mbsfn_subframe_cfg.num_mbms_allocations == 1);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[4].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_offset == 2);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[4].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_period == LTE_L1_MBSFN_ALLOCATION_PERIOD_32);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[4].mbsfn_subframe_cfg.mbsfn_alloacation[0].sfn_allocation == LTE_L1_MBSFN_ALLOCATION_FOUR);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[5].cell_id == 25);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[5].antenna_ports_count == LTE_L1_ANTENNA_INFO_PORTS_COUNT_2);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[5].mbsfn_subframe_cfg.enable == TRUE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[5].mbsfn_subframe_cfg.num_mbms_allocations == 1);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[5].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_offset == 3);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[5].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_period == LTE_L1_MBSFN_ALLOCATION_PERIOD_8);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[5].mbsfn_subframe_cfg.mbsfn_alloacation[0].sfn_allocation == LTE_L1_MBSFN_ALLOCATION_ONE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[6].cell_id == 56);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[6].antenna_ports_count == LTE_L1_ANTENNA_INFO_PORTS_COUNT_4);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[6].mbsfn_subframe_cfg.enable == TRUE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[6].mbsfn_subframe_cfg.num_mbms_allocations == 1);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[6].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_offset == 3);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[6].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_period == LTE_L1_MBSFN_ALLOCATION_PERIOD_8);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[6].mbsfn_subframe_cfg.mbsfn_alloacation[0].sfn_allocation == LTE_L1_MBSFN_ALLOCATION_ONE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[7].cell_id == 19);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[7].antenna_ports_count == LTE_L1_ANTENNA_INFO_PORTS_COUNT_4);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[7].mbsfn_subframe_cfg.enable == TRUE);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[7].mbsfn_subframe_cfg.num_mbms_allocations == 1);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[7].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_offset == 2);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[7].mbsfn_subframe_cfg.mbsfn_alloacation[0].allocation_period == LTE_L1_MBSFN_ALLOCATION_PERIOD_1);
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.crs_assistance_info[7].mbsfn_subframe_cfg.mbsfn_alloacation[0].sfn_allocation == LTE_L1_MBSFN_ALLOCATION_ONE);

  // Trigger HO with ROHC reset
  encoded_pdu.length = sizeof(llc_feicic_crs_info_rel_msg);
  encoded_pdu.value = llc_feicic_crs_info_rel_msg;
  lte_rrc_llc_test_feicic_crs_info(encoded_pdu);

  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->neigh_cell_crs_cfg.enable == FALSE);
  lte_rrc_utf_reset(); 
}

void LteRrcLlcIFeICICCRSInfoTest::Teardown()
{
  lte_rrc_utf_teardown();
}

