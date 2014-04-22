/*!
  @file lte_rrc_meas_test_conn_meas_report.cpp

  @brief
  Unit test code for the meas procedure that tests the measurement reports
  sent by Meas.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/meas/lte_rrc_meas_test_conn_meas_report.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/10   mm     Initial Revision

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "lte_static_rrc_mem_util.h"
#include "__lte_rrc_meas.h"
#include "lte_rrc_meas.h"
#include "lte_rrc_measi.h"
#include "lte_rrc_osys_asn1util.h"
#include <lte_rrc_controller.h>
}

#include <tf_stub.h>
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_meas_test_msg.h"

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

extern "C"
{
  extern lte_rrc_meas_s lte_rrc_meas_data;
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
#if 0
/* Test to verify the processing of LTE_CPHY_REPORT_IND with EUTRA measResults */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIratEUTRAMeasReport);

void LteRrcMeasConnIratEUTRAMeasReport::Setup()
{
  uint32 umid_list[] = 
  { 
    LTE_CPHY_CONN_MEAS_CFG_REQ,
  };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIratEUTRAMeasReport::Test()
{
  lte_cphy_conn_meas_report_ind_s cphy_meas_report_ind;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_osys_UL_DCCH_MessageType *ul_dcch_msg_ptr;
  lte_rrc_osys_MeasResults measResults;
  uint32 buf_len;

  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel11;
  // Configure connected mode measurements in all RATs
  //LteRrcMeasConnAllRatMeasConfig__Test();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  memset(&cphy_meas_report_ind, 0, sizeof(lte_cphy_conn_meas_report_ind_s));

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&cphy_meas_report_ind.msgr_hdr, LTE_CPHY_MEAS_REPORT_IND);
  cphy_meas_report_ind.meas_id = 2;
  cphy_meas_report_ind.auto_delete_meas_id = TRUE;
  cphy_meas_report_ind.serving_rsrp = 40;
  cphy_meas_report_ind.serving_rsrq = 34;
  cphy_meas_report_ind.neigh_results.size = 5;
  cphy_meas_report_ind.neigh_results.type = LTE_CPHY_CONN_MEAS_REPORT_TYPE_EUTRA;
  cphy_meas_report_ind.neigh_results.list.eutra[0].pci = 100;
  cphy_meas_report_ind.neigh_results.list.eutra[0].rsrp = 97;
  cphy_meas_report_ind.neigh_results.list.eutra[0].rsrq = 34;
  cphy_meas_report_ind.neigh_results.list.eutra[1].pci = 150;
  cphy_meas_report_ind.neigh_results.list.eutra[1].rsrp = 30;
  cphy_meas_report_ind.neigh_results.list.eutra[1].rsrq = 0;
  cphy_meas_report_ind.neigh_results.list.eutra[2].pci = 200;
  cphy_meas_report_ind.neigh_results.list.eutra[2].rsrp = 97;
  cphy_meas_report_ind.neigh_results.list.eutra[2].rsrq = 25;
  cphy_meas_report_ind.neigh_results.list.eutra[3].pci = 250;
  cphy_meas_report_ind.neigh_results.list.eutra[3].rsrp = 85;
  cphy_meas_report_ind.neigh_results.list.eutra[3].rsrq = -1;
  cphy_meas_report_ind.neigh_results.list.eutra[4].pci = 300;
  cphy_meas_report_ind.neigh_results.list.eutra[4].rsrp = -5;
  cphy_meas_report_ind.neigh_results.list.eutra[4].rsrq = 34;
  
  lte_rrc_utf_send_msg((byte*)&cphy_meas_report_ind, sizeof(cphy_meas_report_ind));

  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the ul msg reqi containing the meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_MessageType *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.t 
            == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->m.nonCriticalExtensionPresent == 0);

  measResults = ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults;

  // Verify the actual measResults for WCDMA
  TF_ASSERT(measResults.m.measResultNeighCellsPresent);
  TF_ASSERT(measResults.measId == 2);
  TF_ASSERT(measResults.measResultPCell.rsrpResult == 40);
  TF_ASSERT(measResults.measResultPCell.rsrqResult == 34);

  // Verify neighbor cells results
  TF_ASSERT(measResults.measResultNeighCells.t == T_lte_rrc_osys_MeasResults_measResultNeighCells_measResultListEUTRA);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].physCellId == 100);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.m.rsrpResultPresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.m.rsrqResultPresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.
            rsrpResult == 97);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.
            rsrqResult == 34);

  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[1].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[1].
            physCellId == 150);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[1].measResult.m.rsrpResultPresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[1].measResult.m.rsrqResultPresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[1].
            measResult.rsrpResult == 30);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[1].
            measResult.rsrqResult == 0);

  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[2].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[2].
            physCellId == 200);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[2].measResult.m.rsrpResultPresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[2].measResult.m.rsrqResultPresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[2].
            measResult.rsrpResult == 97);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[2].
            measResult.rsrqResult == 25);

  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[3].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[3].
            physCellId == 250);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[3].measResult.m.rsrpResultPresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[3].measResult.m.rsrqResultPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[3].
            measResult.rsrpResult == 85);

  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[4].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[4].
            physCellId == 300);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[4].measResult.m.rsrpResultPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[4].measResult.m.rsrqResultPresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[4].
            measResult.rsrqResult == 34);

  // Verify valid and mod bitmasks
  // Valid bitmask 111 111111111 
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->src_vmc_ptr->meas_obj_valid_bitmask == 0x3FF);

  // Valid bitmask 111 111111101 - Meas id 2 alone should be deleted
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->src_vmc_ptr->meas_id_valid_bitmask == 0x3FD);

  // Valid bitmask 1111111
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->src_vmc_ptr->report_cfg_valid_bitmask == 0x7F);

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

}

void LteRrcMeasConnIratEUTRAMeasReport::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of LTE_CPHY_REPORT_IND with WCDMA measResults */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIratWCDMAMeasReport);

void LteRrcMeasConnIratWCDMAMeasReport::Setup()
{
  uint32 umid_list[] = 
  { 
    LTE_CPHY_CONN_MEAS_CFG_REQ,
  };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIratWCDMAMeasReport::Test()
{
  lte_cphy_conn_meas_report_ind_s cphy_meas_report_ind;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_osys_UL_DCCH_MessageType *ul_dcch_msg_ptr;
  lte_rrc_osys_MeasResults measResults;
  uint32 buf_len;

  // Configure connected mode measurements in all RATs
  //LteRrcMeasConnAllRatMeasConfig__Test();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  memset(&cphy_meas_report_ind, 0, sizeof(lte_cphy_conn_meas_report_ind_s));
  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&cphy_meas_report_ind.msgr_hdr, LTE_CPHY_MEAS_REPORT_IND);
  cphy_meas_report_ind.meas_id = 3;
  cphy_meas_report_ind.auto_delete_meas_id = FALSE;
  cphy_meas_report_ind.serving_rsrp = 40;
  cphy_meas_report_ind.serving_rsrq = 34;
  cphy_meas_report_ind.neigh_results.size = 4;
  cphy_meas_report_ind.neigh_results.type = LTE_CPHY_CONN_MEAS_REPORT_TYPE_WCDMA;
  cphy_meas_report_ind.neigh_results.list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  cphy_meas_report_ind.neigh_results.list.wcdma[0].psc = 100;
  cphy_meas_report_ind.neigh_results.list.wcdma[0].cpich_rscp = 87;
  cphy_meas_report_ind.neigh_results.list.wcdma[0].cpich_ecno = 45;
  cphy_meas_report_ind.neigh_results.list.wcdma[1].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  cphy_meas_report_ind.neigh_results.list.wcdma[1].psc = 150;
  cphy_meas_report_ind.neigh_results.list.wcdma[1].cpich_rscp = 87;
  cphy_meas_report_ind.neigh_results.list.wcdma[1].cpich_ecno = -1;
  cphy_meas_report_ind.neigh_results.list.wcdma[2].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  cphy_meas_report_ind.neigh_results.list.wcdma[2].psc = 170;
  cphy_meas_report_ind.neigh_results.list.wcdma[2].cpich_rscp = -5;
  cphy_meas_report_ind.neigh_results.list.wcdma[2].cpich_ecno = 49;
  cphy_meas_report_ind.neigh_results.list.wcdma[3].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  cphy_meas_report_ind.neigh_results.list.wcdma[3].psc = 190;
  cphy_meas_report_ind.neigh_results.list.wcdma[3].cpich_rscp = -128;
  cphy_meas_report_ind.neigh_results.list.wcdma[3].cpich_ecno = 25;
  
  lte_rrc_utf_send_msg((byte*)&cphy_meas_report_ind, sizeof(cphy_meas_report_ind));

  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the ul msg reqi containing the meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_MessageType *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.t 
            == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->m.nonCriticalExtensionPresent == 0);

  measResults = ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults;

  // Verify the actual measResults for WCDMA
  TF_ASSERT(measResults.m.measResultNeighCellsPresent);
  TF_ASSERT(measResults.measId == 3);
  TF_ASSERT(measResults.measResultPCell.rsrpResult == 40);
  TF_ASSERT(measResults.measResultPCell.rsrqResult == 34);

  // Verify neighbor cells results
  TF_ASSERT(measResults.measResultNeighCells.t == T_lte_rrc_osys_MeasResults_measResultNeighCells_measResultListUTRA);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[0].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[0].physCellId.
            t == T_lte_rrc_osys_MeasResultUTRA_physCellId_fdd);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[0].physCellId.
            u.fdd == 100);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[0].measResult.m.utra_EcN0Present);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[0].measResult.m.utra_RSCPPresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[0].measResult.
            utra_RSCP == 87);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[0].measResult.
            utra_EcN0 == 45);

  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[1].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[1].physCellId.
            t == T_lte_rrc_osys_MeasResultUTRA_physCellId_fdd);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[1].physCellId.
            u.fdd == 150);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[1].measResult.m.utra_EcN0Present == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[1].measResult.m.utra_RSCPPresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[1].measResult.
            utra_RSCP == 87);

  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[2].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[2].
            physCellId.t == T_lte_rrc_osys_MeasResultUTRA_physCellId_fdd);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[2].
            physCellId.u.fdd == 170);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[2].measResult.m.utra_EcN0Present);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[2].measResult.m.utra_RSCPPresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[2].
            measResult.utra_RSCP == -5);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[2].
            measResult.utra_EcN0 == 49);
  
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[3].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[3].
            physCellId.t == T_lte_rrc_osys_MeasResultUTRA_physCellId_fdd);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[3].
            physCellId.u.fdd == 190);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[3].measResult.m.utra_EcN0Present);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[3].measResult.m.utra_RSCPPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListUTRA->elem[3].
            measResult.utra_EcN0 == 25);

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

}

void LteRrcMeasConnIratWCDMAMeasReport::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of LTE_CPHY_REPORT_IND with GERAN measResults */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIratGERANMeasReport);

void LteRrcMeasConnIratGERANMeasReport::Setup()
{
  uint32 umid_list[] = 
  { 
    LTE_CPHY_CONN_MEAS_CFG_REQ,
  };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIratGERANMeasReport::Test()
{
  lte_cphy_conn_meas_report_ind_s cphy_meas_report_ind;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_osys_UL_DCCH_MessageType *ul_dcch_msg_ptr;
  lte_rrc_osys_MeasResults measResults;
  uint32 buf_len;

  // Configure connected mode measurements in all RATs
  //LteRrcMeasConnAllRatMeasConfig__Test();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  memset(&cphy_meas_report_ind, 0, sizeof(lte_cphy_conn_meas_report_ind_s));
  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&cphy_meas_report_ind.msgr_hdr, LTE_CPHY_MEAS_REPORT_IND);
  cphy_meas_report_ind.meas_id = 7;
  cphy_meas_report_ind.auto_delete_meas_id = FALSE;
  cphy_meas_report_ind.serving_rsrp = 40;
  cphy_meas_report_ind.serving_rsrq = 34;
  cphy_meas_report_ind.neigh_results.size = 3;
  cphy_meas_report_ind.neigh_results.type = LTE_CPHY_CONN_MEAS_REPORT_TYPE_GERAN;
  cphy_meas_report_ind.neigh_results.list.gsm[0].freq = 100;
  cphy_meas_report_ind.neigh_results.list.gsm[0].gsm_band = LTE_L1_GSM_BAND_DCS1800;
  cphy_meas_report_ind.neigh_results.list.gsm[0].phy_cell_id.bcc = 5;
  cphy_meas_report_ind.neigh_results.list.gsm[0].phy_cell_id.ncc = 7;
  cphy_meas_report_ind.neigh_results.list.gsm[0].rssi = 55;

  cphy_meas_report_ind.neigh_results.list.gsm[1].freq = 100;
  cphy_meas_report_ind.neigh_results.list.gsm[1].gsm_band = LTE_L1_GSM_BAND_PCS1900;
  cphy_meas_report_ind.neigh_results.list.gsm[1].phy_cell_id.bcc = 5;
  cphy_meas_report_ind.neigh_results.list.gsm[1].phy_cell_id.ncc = 5;
  cphy_meas_report_ind.neigh_results.list.gsm[1].rssi = 63;

  cphy_meas_report_ind.neigh_results.list.gsm[2].freq = 100;
  cphy_meas_report_ind.neigh_results.list.gsm[2].gsm_band = LTE_L1_GSM_BAND_PCS1900;
  cphy_meas_report_ind.neigh_results.list.gsm[2].phy_cell_id.bcc = 7;
  cphy_meas_report_ind.neigh_results.list.gsm[2].phy_cell_id.ncc = 5;
  cphy_meas_report_ind.neigh_results.list.gsm[2].rssi = 0;
  
  lte_rrc_utf_send_msg((byte*)&cphy_meas_report_ind, sizeof(cphy_meas_report_ind));

  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the ul msg reqi containing the meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_MessageType *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.t 
            == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->m.nonCriticalExtensionPresent == 0);

  measResults = ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults;

  // Verify the actual measResults for WCDMA
  TF_ASSERT(measResults.m.measResultNeighCellsPresent);
  TF_ASSERT(measResults.measId == 7);
  TF_ASSERT(measResults.measResultPCell.rsrpResult == 40);
  TF_ASSERT(measResults.measResultPCell.rsrqResult == 34);

  // Verify neighbor cells results
  TF_ASSERT(measResults.measResultNeighCells.t == T_lte_rrc_osys_MeasResults_measResultNeighCells_measResultListGERAN);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListGERAN->elem[0].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListGERAN->elem[0].carrierFreq.
            arfcn == 100);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListGERAN->elem[0].carrierFreq.
            bandIndicator == lte_rrc_osys_BandIndicatorGERAN_dcs1800);
  TF_ASSERT(lte_rrc_osys_asn1_bitstring_to_int(sizeof(measResults.measResultNeighCells.u.
                                                 measResultListGERAN->elem[0].physCellId.
                                                 baseStationColourCode),
                                          &measResults.measResultNeighCells.u.
                                          measResultListGERAN->elem[0].physCellId.
                                          baseStationColourCode) == 5);
  TF_ASSERT(lte_rrc_osys_asn1_bitstring_to_int(sizeof(measResults.measResultNeighCells.u.
                                                 measResultListGERAN->elem[0].physCellId.
                                                 networkColourCode),
                                          &measResults.measResultNeighCells.u.
                                          measResultListGERAN->elem[0].physCellId.
                                          networkColourCode) == 7);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListGERAN->elem[0].measResult.rssi == 55);

  TF_ASSERT(measResults.measResultNeighCells.u.measResultListGERAN->elem[1].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListGERAN->elem[1].carrierFreq.
            arfcn == 100);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListGERAN->elem[1].carrierFreq.
            bandIndicator == lte_rrc_osys_BandIndicatorGERAN_pcs1900);
  TF_ASSERT(lte_rrc_osys_asn1_bitstring_to_int(sizeof(measResults.measResultNeighCells.u.
                                                 measResultListGERAN->elem[1].physCellId.
                                                 baseStationColourCode),
                                          &measResults.measResultNeighCells.u.
                                          measResultListGERAN->elem[1].physCellId.
                                          baseStationColourCode) == 5);
  TF_ASSERT(lte_rrc_osys_asn1_bitstring_to_int(sizeof(measResults.measResultNeighCells.u.
                                                 measResultListGERAN->elem[1].physCellId.
                                                 networkColourCode),
                                          &measResults.measResultNeighCells.u.
                                          measResultListGERAN->elem[1].physCellId.
                                          networkColourCode) == 5);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListGERAN->elem[1].
            measResult.rssi == 63);

  TF_ASSERT(measResults.measResultNeighCells.u.measResultListGERAN->elem[2].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListGERAN->elem[2].carrierFreq.
            arfcn == 100);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListGERAN->elem[2].carrierFreq.
            bandIndicator == lte_rrc_osys_BandIndicatorGERAN_pcs1900);
  TF_ASSERT(lte_rrc_osys_asn1_bitstring_to_int(sizeof(measResults.measResultNeighCells.u.
                                                 measResultListGERAN->elem[2].physCellId.
                                                 baseStationColourCode),
                                          &measResults.measResultNeighCells.u.
                                          measResultListGERAN->elem[2].physCellId.
                                          baseStationColourCode) == 7);
  TF_ASSERT(lte_rrc_osys_asn1_bitstring_to_int(sizeof(measResults.measResultNeighCells.u.
                                                 measResultListGERAN->elem[2].physCellId.
                                                 networkColourCode),
                                          &measResults.measResultNeighCells.u.
                                          measResultListGERAN->elem[2].physCellId.
                                          networkColourCode) == 5);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListGERAN->elem[2].
            measResult.rssi == 0);

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

}

void LteRrcMeasConnIratGERANMeasReport::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of LTE_CPHY_REPORT_IND with CDMA measResults */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnIratCDMAMeasReport);

void LteRrcMeasConnIratCDMAMeasReport::Setup()
{
  uint32 umid_list[] = 
  { 
    LTE_CPHY_CONN_MEAS_CFG_REQ,
  };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnIratCDMAMeasReport::Test()
{
  lte_cphy_conn_meas_report_ind_s cphy_meas_report_ind;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_osys_UL_DCCH_MessageType *ul_dcch_msg_ptr;
  lte_rrc_osys_MeasResults measResults;
  uint32 buf_len;

  // Configure connected mode measurements in all RATs
  //LteRrcMeasConnAllRatMeasConfig__Test();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  memset(&cphy_meas_report_ind, 0, sizeof(lte_cphy_conn_meas_report_ind_s));
  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&cphy_meas_report_ind.msgr_hdr, LTE_CPHY_MEAS_REPORT_IND);
  cphy_meas_report_ind.meas_id = 9;
  cphy_meas_report_ind.auto_delete_meas_id = FALSE;
  cphy_meas_report_ind.serving_rsrp = 40;
  cphy_meas_report_ind.serving_rsrq = 34;
  cphy_meas_report_ind.neigh_results.size = 3;
  cphy_meas_report_ind.neigh_results.type = LTE_CPHY_CONN_MEAS_REPORT_TYPE_CDMA2K;
  cphy_meas_report_ind.neigh_results.list.cdma2k[0].phy_cell_id = 511;
  cphy_meas_report_ind.neigh_results.list.cdma2k[0].pilot_strength = 63;
  cphy_meas_report_ind.neigh_results.list.cdma2k[0].pilot_pn_phase = 32767;
  cphy_meas_report_ind.neigh_results.list.cdma2k[1].phy_cell_id = 350;
  cphy_meas_report_ind.neigh_results.list.cdma2k[1].pilot_strength = 5;
  cphy_meas_report_ind.neigh_results.list.cdma2k[1].pilot_pn_phase = -4;
  cphy_meas_report_ind.neigh_results.list.cdma2k[2].phy_cell_id = 250;
  cphy_meas_report_ind.neigh_results.list.cdma2k[2].pilot_strength = 0;
  cphy_meas_report_ind.neigh_results.list.cdma2k[2].pilot_pn_phase = 0;
  
  lte_rrc_utf_send_msg((byte*)&cphy_meas_report_ind, sizeof(cphy_meas_report_ind));

  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the ul msg reqi containing the meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_MessageType *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.t 
            == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->m.nonCriticalExtensionPresent == 0);

  measResults = ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults;

  // Verify the actual measResults for WCDMA
  TF_ASSERT(measResults.m.measResultNeighCellsPresent);
  TF_ASSERT(measResults.measId == 9);
  TF_ASSERT(measResults.measResultPCell.rsrpResult == 40);
  TF_ASSERT(measResults.measResultPCell.rsrqResult == 34);

  // Verify neighbor cells results
  TF_ASSERT(measResults.measResultNeighCells.t == T_lte_rrc_osys_MeasResults_measResultNeighCells_measResultsCDMA2000);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultsCDMA2000->
            measResultListCDMA2000.elem[0].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultsCDMA2000->
            measResultListCDMA2000.elem[0].physCellId == 511);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultsCDMA2000->
            measResultListCDMA2000.elem[0].measResult.m.pilotPnPhasePresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultsCDMA2000->
            measResultListCDMA2000.elem[0].measResult.pilotStrength == 63);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultsCDMA2000->
            measResultListCDMA2000.elem[0].measResult.pilotPnPhase == 32767);

  TF_ASSERT(measResults.measResultNeighCells.u.measResultsCDMA2000->
            measResultListCDMA2000.elem[1].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultsCDMA2000->
            measResultListCDMA2000.elem[1].physCellId == 350);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultsCDMA2000->
            measResultListCDMA2000.elem[1].measResult.m.pilotPnPhasePresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultsCDMA2000->
            measResultListCDMA2000.elem[1].measResult.pilotStrength == 5);

  TF_ASSERT(measResults.measResultNeighCells.u.measResultsCDMA2000->
            measResultListCDMA2000.elem[2].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultsCDMA2000->
            measResultListCDMA2000.elem[2].physCellId == 250);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultsCDMA2000->
            measResultListCDMA2000.elem[2].measResult.m.pilotPnPhasePresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultsCDMA2000->
            measResultListCDMA2000.elem[2].measResult.pilotStrength == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultsCDMA2000->
            measResultListCDMA2000.elem[2].measResult.pilotPnPhase == 0);


  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

}

void LteRrcMeasConnIratCDMAMeasReport::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

#endif



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConn0NeighReport);

void LteRrcMeasConn0NeighReport::Setup()
{
  uint32 umid_list[] = 
  { 
    LTE_CPHY_CONN_MEAS_CFG_REQ,
  };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConn0NeighReport::Test()
{
  lte_cphy_conn_meas_report_ind_s cphy_meas_report_ind;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_osys_UL_DCCH_MessageType *ul_dcch_msg_ptr;
  lte_rrc_osys_MeasResults measResults;
  uint32 buf_len;

  // Configure connected mode measurements in all RATs
  //LteRrcMeasConnAllRatMeasConfig__Test();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  memset(&cphy_meas_report_ind, 0, sizeof(lte_cphy_conn_meas_report_ind_s));
  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&cphy_meas_report_ind.msgr_hdr, LTE_CPHY_MEAS_REPORT_IND);
  cphy_meas_report_ind.meas_id = 9;
  cphy_meas_report_ind.auto_delete_meas_id = FALSE;
  cphy_meas_report_ind.serving_rsrp = 40;
  cphy_meas_report_ind.serving_rsrq = 34;
  cphy_meas_report_ind.neigh_results.size = 0;
  
  lte_rrc_utf_send_msg((byte*)&cphy_meas_report_ind, sizeof(cphy_meas_report_ind));

  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the ul msg reqi containing the meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_MessageType *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.t 
            == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->m.nonCriticalExtensionPresent == 0);

  measResults = ul_dcch_msg_ptr->u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults;

  // Verify the actual measResults for WCDMA
  TF_ASSERT(FALSE == measResults.m.measResultNeighCellsPresent);
  TF_ASSERT(measResults.measId == 9);
  TF_ASSERT(measResults.measResultPCell.rsrpResult == 40);
  TF_ASSERT(measResults.measResultPCell.rsrqResult == 34);


  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

}

void LteRrcMeasConn0NeighReport::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



/*===========================================================================

                                UNIT TEST

===========================================================================*/

/* Test to verify the processing of LTE_CPHY_IFREQ_OTDOA_IND with RSTD Inter Frequency Info List */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnInterFreqRSTDMeasurementInd);

void LteRrcMeasConnInterFreqRSTDMeasurementInd::Setup()
{
  uint32 umid_list[] = 
  {
    LTE_CPHY_IFREQ_OTDOA_IND,
  };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnInterFreqRSTDMeasurementInd::Test()
{
  lte_cphy_ifreq_otdoa_ind_s cphy_ifreq_otdoa_ind;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_osys_UL_DCCH_MessageType *ul_dcch_msg_ptr;
  lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_IEs_rstd_InterFreqIndication_r10_start *rstd_interfreq_ind_start_ptr;
  uint32 buf_len;
  uint8 intqueue, extqueue;
  lte_rrc_connected_indi_s connected_indi;
  
  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();


  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send the interfreq otdoa indi to meas sm
  lte_rrc_init_default_hdr(&cphy_ifreq_otdoa_ind.msgr_hdr, LTE_CPHY_IFREQ_OTDOA_IND);

  cphy_ifreq_otdoa_ind.meas_action = LTE_CPHY_IFREQ_OTDOA_MEAS_ACTION_START;
  cphy_ifreq_otdoa_ind.meas_freqs[0].earfcn = 9871;
  cphy_ifreq_otdoa_ind.meas_freqs[0].prs_offset = 25;
  cphy_ifreq_otdoa_ind.meas_freqs[1].earfcn = 5625;
  cphy_ifreq_otdoa_ind.meas_freqs[1].prs_offset = 13;
  cphy_ifreq_otdoa_ind.meas_freqs[2].earfcn = 7411;
  cphy_ifreq_otdoa_ind.meas_freqs[2].prs_offset = 71;
  cphy_ifreq_otdoa_ind.num_freq = 3;
  lte_rrc_utf_send_msg((byte*)&cphy_ifreq_otdoa_ind, sizeof(cphy_ifreq_otdoa_ind));

  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the ul msg reqi containing the interfreq rstd meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_MessageType *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_interFreqRSTDMeasurementIndication_r10);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.t 
            == T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_criticalExtensions_c1_interFreqRSTDMeasurementIndication_r10);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->m.nonCriticalExtensionPresent == 0);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->m.lateNonCriticalExtensionPresent == 0);

  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->rstd_InterFreqIndication_r10.t ==
            T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_IEs_rstd_InterFreqIndication_r10_start);

  rstd_interfreq_ind_start_ptr = ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->rstd_InterFreqIndication_r10.u.start;

  TF_ASSERT(rstd_interfreq_ind_start_ptr -> rstd_InterFreqInfoList_r10.n == 2);
  TF_ASSERT(rstd_interfreq_ind_start_ptr->rstd_InterFreqInfoList_r10.elem[0].carrierFreq_r10 == 9871);
  TF_ASSERT(rstd_interfreq_ind_start_ptr->rstd_InterFreqInfoList_r10.elem[0].measPRS_Offset_r10 == 25);
  TF_ASSERT(rstd_interfreq_ind_start_ptr->rstd_InterFreqInfoList_r10.elem[1].carrierFreq_r10 == 5625);
  TF_ASSERT(rstd_interfreq_ind_start_ptr->rstd_InterFreqInfoList_r10.elem[1].measPRS_Offset_r10 == 13);

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  
  cphy_ifreq_otdoa_ind.meas_action = LTE_CPHY_IFREQ_OTDOA_MEAS_ACTION_STOP;
  lte_rrc_utf_send_msg((byte*)&cphy_ifreq_otdoa_ind, sizeof(cphy_ifreq_otdoa_ind));
  
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the ul msg reqi containing the interfreq rstd meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_MessageType *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_interFreqRSTDMeasurementIndication_r10);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.t 
            == T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_criticalExtensions_c1_interFreqRSTDMeasurementIndication_r10);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->m.nonCriticalExtensionPresent == 0);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->m.lateNonCriticalExtensionPresent == 0);

  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->rstd_InterFreqIndication_r10.t ==
            T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_IEs_rstd_InterFreqIndication_r10_stop);


  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  cphy_ifreq_otdoa_ind.meas_action = LTE_CPHY_IFREQ_OTDOA_MEAS_ACTION_START;
  cphy_ifreq_otdoa_ind.meas_freqs[0].earfcn = 9871;
  cphy_ifreq_otdoa_ind.meas_freqs[0].prs_offset = 41;
  cphy_ifreq_otdoa_ind.meas_freqs[1].earfcn = 5625;
  cphy_ifreq_otdoa_ind.meas_freqs[1].prs_offset = 55;
  cphy_ifreq_otdoa_ind.meas_freqs[2].earfcn = 7411;
  cphy_ifreq_otdoa_ind.meas_freqs[2].prs_offset = 71;
  cphy_ifreq_otdoa_ind.num_freq = 3;
  lte_rrc_utf_send_msg((byte*)&cphy_ifreq_otdoa_ind, sizeof(cphy_ifreq_otdoa_ind));

  lte_rrc_utf_wait_for_done();

  /* No UL OTA is sent here as all the freqency lists in the interfreq OTDOA indication are invalid*/

  lte_rrc_utf_get_queue_counts(&intqueue,&extqueue);
  TF_ASSERT(intqueue == 0);
  TF_ASSERT(extqueue == 0);


  cphy_ifreq_otdoa_ind.meas_action = LTE_CPHY_IFREQ_OTDOA_MEAS_ACTION_START;
  cphy_ifreq_otdoa_ind.meas_freqs[0].earfcn = 9871;
  cphy_ifreq_otdoa_ind.meas_freqs[0].prs_offset = 16;
  cphy_ifreq_otdoa_ind.meas_freqs[1].earfcn = 5625;
  cphy_ifreq_otdoa_ind.meas_freqs[1].prs_offset = 55;
  cphy_ifreq_otdoa_ind.meas_freqs[2].earfcn = 7411;
  cphy_ifreq_otdoa_ind.meas_freqs[2].prs_offset = 72;
  cphy_ifreq_otdoa_ind.num_freq = 25;
  lte_rrc_utf_send_msg((byte*)&cphy_ifreq_otdoa_ind, sizeof(cphy_ifreq_otdoa_ind));

  lte_rrc_utf_wait_for_done();
  
  // We expect meas sm to have thrown the ul msg reqi containing the interfreq rstd meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);
  
    ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_MessageType *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_interFreqRSTDMeasurementIndication_r10);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.t 
            == T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_criticalExtensions_c1_interFreqRSTDMeasurementIndication_r10);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->m.nonCriticalExtensionPresent == 0);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->m.lateNonCriticalExtensionPresent == 0);

  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->rstd_InterFreqIndication_r10.t ==
            T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_IEs_rstd_InterFreqIndication_r10_start);

  rstd_interfreq_ind_start_ptr = ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->rstd_InterFreqIndication_r10.u.start;

  TF_ASSERT(rstd_interfreq_ind_start_ptr->rstd_InterFreqInfoList_r10.elem[0].carrierFreq_r10 == 9871);
  TF_ASSERT(rstd_interfreq_ind_start_ptr->rstd_InterFreqInfoList_r10.elem[0].measPRS_Offset_r10 == 16);

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  
  cphy_ifreq_otdoa_ind.meas_action = LTE_CPHY_IFREQ_OTDOA_MEAS_ACTION_STOP;
  lte_rrc_utf_send_msg((byte*)&cphy_ifreq_otdoa_ind, sizeof(cphy_ifreq_otdoa_ind));
  
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the ul msg reqi containing the interfreq rstd meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_MessageType *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_interFreqRSTDMeasurementIndication_r10);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.t 
            == T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_criticalExtensions_c1_interFreqRSTDMeasurementIndication_r10);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->m.nonCriticalExtensionPresent == 0);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->m.lateNonCriticalExtensionPresent == 0);

  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->rstd_InterFreqIndication_r10.t ==
            T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_IEs_rstd_InterFreqIndication_r10_stop);


  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  // Now take controller to suspended state
  TF_MSG("Controller state Suspended");
  lte_rrc_meas_test_ctrl_state = LTE_RRC_CONTROLLER_SM__SUSPENDED;

  cphy_ifreq_otdoa_ind.meas_action = LTE_CPHY_IFREQ_OTDOA_MEAS_ACTION_START;
  cphy_ifreq_otdoa_ind.meas_freqs[0].earfcn = 9871;
  cphy_ifreq_otdoa_ind.meas_freqs[0].prs_offset = 25;
  cphy_ifreq_otdoa_ind.meas_freqs[1].earfcn = 5625;
  cphy_ifreq_otdoa_ind.meas_freqs[1].prs_offset = 13;
  cphy_ifreq_otdoa_ind.meas_freqs[2].earfcn = 7411;
  cphy_ifreq_otdoa_ind.meas_freqs[2].prs_offset = 71;
  cphy_ifreq_otdoa_ind.num_freq = 3;
  lte_rrc_utf_send_msg((byte*)&cphy_ifreq_otdoa_ind, sizeof(cphy_ifreq_otdoa_ind));

  lte_rrc_utf_wait_for_done();

  /* The ML1 indication is pended as the controller is in suspended state. Will be requeued once the controller is back to connected state*/

  lte_rrc_utf_get_queue_counts(&intqueue,&extqueue);
  TF_ASSERT(intqueue == 0);
  TF_ASSERT(extqueue == 0);

  // Now take controller to connected state
  TF_MSG("Controller state Connected");
  lte_rrc_meas_test_ctrl_state = LTE_RRC_CONTROLLER_SM__CONNECTED;

  TF_MSG("Sending Connected indication");
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the ul msg reqi containing the interfreq rstd meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_MessageType *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_interFreqRSTDMeasurementIndication_r10);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.t 
            == T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_criticalExtensions_c1_interFreqRSTDMeasurementIndication_r10);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->m.nonCriticalExtensionPresent == 0);
  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->m.lateNonCriticalExtensionPresent == 0);

  TF_ASSERT(ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->rstd_InterFreqIndication_r10.t ==
            T_lte_rrc_osys_InterFreqRSTDMeasurementIndication_r10_IEs_rstd_InterFreqIndication_r10_start);

  rstd_interfreq_ind_start_ptr = ul_dcch_msg_ptr->u.c1->u.interFreqRSTDMeasurementIndication_r10->criticalExtensions.u.c1->u.
            interFreqRSTDMeasurementIndication_r10->rstd_InterFreqIndication_r10.u.start;

  TF_ASSERT(rstd_interfreq_ind_start_ptr -> rstd_InterFreqInfoList_r10.n == 2);
  TF_ASSERT(rstd_interfreq_ind_start_ptr->rstd_InterFreqInfoList_r10.elem[0].carrierFreq_r10 == 9871);
  TF_ASSERT(rstd_interfreq_ind_start_ptr->rstd_InterFreqInfoList_r10.elem[0].measPRS_Offset_r10 == 25);
  TF_ASSERT(rstd_interfreq_ind_start_ptr->rstd_InterFreqInfoList_r10.elem[1].carrierFreq_r10 == 5625);
  TF_ASSERT(rstd_interfreq_ind_start_ptr->rstd_InterFreqInfoList_r10.elem[1].measPRS_Offset_r10 == 13);

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);
}

void LteRrcMeasConnInterFreqRSTDMeasurementInd::Teardown()
{
  lte_rrc_utf_teardown();
}

