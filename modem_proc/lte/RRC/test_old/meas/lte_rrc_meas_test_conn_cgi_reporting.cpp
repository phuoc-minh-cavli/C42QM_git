/*!
  @file lte_rrc_meas_test_conn_cgi_reporting.cpp

  @brief
  Unit test code for the meas procedure that tests connected mode measurements
  configuration for IRAT GSM.

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

$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/meas/
lte_rrc_meas_test_conn_cgi_reporting.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/10   mm     Initial Revision

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
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_meas_test_msg.h"
#include "lte_rrc_cep_test_common.h"
#include <tf_stub.h>
/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_meas_test_conn_cgi_reporting.cpp
*/
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_MSG;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_LOC_INFO_MSG;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_MSG_ON_B29;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_RPT_CFG_UNMODIFIED;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_RPT_CFG_MODIFIED_TO_CGI;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_MSG_WO_CGI_CELL;
DECLARE_LTE_RRC_MEAS_TEST_EMPTY_RECFG_MSG;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_WITH_SIHOREQ_MSG;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_RECONFIG_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_RECONFIG_MSG2;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_SERVING_CGI_MSG;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_SIB1_MSG;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_SIB1_BARRED_MSG;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_SIB1_RESERVED_PLMN_MSG;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_SIB1_MSG2;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_SIB1_MSG3;
DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_EUTRA_HO_WITH_CGI;

extern "C"
{
  extern lte_rrc_meas_s lte_rrc_meas_data;
}

extern "C"
{
  TF_PROTO_STUB(uint8, lte_rrc_cap_get_fgi_bit, uint8);
#ifdef FEATURE_FEMTO_CSG
  TF_PROTO_STUB(boolean, lte_rrc_csg_asf_check_csg_in_whitelist, const lte_rrc_plmn_s *, sys_csg_id_type, lte_earfcn_t);
  #endif
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
#ifdef FEATURE_LTE_MDT
  TF_PROTO_STUB(void, lte_rrc_loc_services_get_loc_info, lte_rrc_osys_LocationInfo_r10 *);  
#endif
}
/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf

===========================================================================*/
/*!
    @brief
    This function tests the sending of LTE_CPHY_SERV_CELL_MEAS_REQ and CNF.

    @return
    void

*/
/*=========================================================================*/
void lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf
(
  void
)
{
  lte_cphy_serv_cell_meas_req_s *serv_cell_meas_req;
  lte_cphy_serv_cell_meas_cnf_s serv_cell_meas_cnf;
  uint32 buf_len = 0;

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting
  // and trigger report indication from ML1
  lte_rrc_utf_get_next_ext_msg((byte**)&serv_cell_meas_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_serv_cell_meas_req_s));
  TF_ASSERT(serv_cell_meas_req->msgr_hdr.id == LTE_CPHY_SERV_CELL_MEAS_REQ);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send serving cell measurement results to Meas
  lte_rrc_init_default_hdr(&serv_cell_meas_cnf.msgr_hdr, LTE_CPHY_SERV_CELL_MEAS_CNF);
  serv_cell_meas_cnf.serving_rsrp = 80;
  serv_cell_meas_cnf.serving_rsrq = 28;
  serv_cell_meas_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&serv_cell_meas_cnf, sizeof(serv_cell_meas_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();

} /* lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal

===========================================================================*/
/*!
    @brief
    This function tests the processing of unsolicited SIB indication by Meas.

    @return
    void

*/
/*=========================================================================*/
void lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal
(
  void
)
{
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  uint32 buf_len = 0;

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting
  // and trigger report indication from ML1
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in the onnected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

} /* lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal
 */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_meas_report_for_cgi

===========================================================================*/
/*!
    @brief
    This function tests the CGI meas report sent by Meas SM

    @return
    void

*/
/*=========================================================================*/
void lte_rrc_meas_test_meas_report_for_cgi
(
  uint16 cgi_pci,
  boolean csg_included,
  boolean csg_member_status_included,
  boolean csg_ind,
  boolean band_29_present
)
{
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_osys_MeasResults measResults;
  uint32 buf_len = 0;
  lte_rrc_cgi_success_indi_s *cgi_success_reqi = NULL;

  // We expect meas sm to have thrown the ul msg reqi containing the meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->message.t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.t 
            == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->m.nonCriticalExtensionPresent == 0);

  if (csg_ind)
  {
    lte_rrc_utf_get_next_int_msg ( (byte**) &cgi_success_reqi, &buf_len );
    TF_ASSERT ( buf_len == sizeof ( lte_rrc_cgi_success_indi_s ) );
    TF_ASSERT ( cgi_success_reqi->msg_hdr.id == LTE_RRC_CGI_SUCCESS_INDI );
    TF_ASSERT ( cgi_success_reqi->target_cell_info.rat == SYS_RAT_LTE_RADIO_ACCESS );
    TF_ASSERT ( cgi_success_reqi->target_cell_info.phy_cell_id == cgi_pci);
    if (band_29_present)
    {
      TF_ASSERT ( cgi_success_reqi->target_cell_info.frequency == 9710);
    }
    else
    {
      TF_ASSERT ( cgi_success_reqi->target_cell_info.frequency == 0x100);
    }
    TF_ASSERT ( cgi_success_reqi->target_cell_info.plmn.identity [0] == 0x53);
    TF_ASSERT ( cgi_success_reqi->target_cell_info.plmn.identity [1] == 0x67);
    TF_ASSERT ( cgi_success_reqi->target_cell_info.plmn.identity [2] == 0x54);
    TF_ASSERT ( cgi_success_reqi->target_cell_info.cell_identity == 0xA05040);
  }

  measResults = ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults;

  // Verify the actual measResults
  TF_ASSERT(measResults.m.measResultNeighCellsPresent);
  TF_ASSERT(measResults.measId == 5);
  TF_ASSERT(measResults.measResultPCell.rsrpResult == 80);
  TF_ASSERT(measResults.measResultPCell.rsrqResult == 28);

  // Verify CGI info
  TF_ASSERT(measResults.measResultNeighCells.t == T_lte_rrc_osys_MeasResults_measResultNeighCells_measResultListEUTRA);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].m.cgi_InfoPresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].physCellId == cgi_pci);
  
  // Verify PLMN identity
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
            cellGlobalId.plmn_Identity.m.mccPresent);

  // Verify MCC
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
            cellGlobalId.plmn_Identity.mcc.elem[0] == 3);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
            cellGlobalId.plmn_Identity.mcc.elem[1] == 5);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
            cellGlobalId.plmn_Identity.mcc.elem[2] == 7);

  // Verify MNC
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
            cellGlobalId.plmn_Identity.mnc.elem[0] == 4);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
            cellGlobalId.plmn_Identity.mnc.elem[1] == 5);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
            cellGlobalId.plmn_Identity.mnc.elem[2] == 6);

  // Verify cell identity
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
            cellGlobalId.cellIdentity.numbits == 28);
  TF_ASSERT(lte_rrc_osys_asn1_bitstring_to_int(sizeof(measResults.measResultNeighCells.u.
                                                 measResultListEUTRA->elem[0].cgi_Info.
                                                 cellGlobalId.cellIdentity),
                                          &measResults.measResultNeighCells.u.
                                          measResultListEUTRA->elem[0].cgi_Info.
                                          cellGlobalId.cellIdentity) == 0xA05040);  

  // Verify tracking area code
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
            trackingAreaCode.numbits == 16);
  TF_ASSERT(lte_rrc_osys_asn1_bitstring_to_int(sizeof(measResults.measResultNeighCells.u.
                                                 measResultListEUTRA->elem[0].cgi_Info.
                                                 trackingAreaCode),
                                          &measResults.measResultNeighCells.u.
                                          measResultListEUTRA->elem[0].cgi_Info.
                                          trackingAreaCode) == 0x0F0F); 
  // Verify measResults 
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.
            m.rsrpResultPresent == 0); 
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.
            m.rsrqResultPresent == 0);

  if (csg_included)
  {
    TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.
                m.additionalSI_Info_r9Present == 1);
    TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.
                m._v2ExtPresent == 1);
    TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.
                additionalSI_Info_r9.m.csg_Identity_r9Present == 1);
    TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.
                additionalSI_Info_r9.csg_Identity_r9.numbits == 27);
    TF_ASSERT(lte_rrc_osys_asn1_bitstring_to_int(
                sizeof(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].
                  measResult.additionalSI_Info_r9.csg_Identity_r9),
                &measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.
                  additionalSI_Info_r9.csg_Identity_r9) 
                == 0x505860); 
    if (csg_ind)
    TF_ASSERT ( cgi_success_reqi->target_cell_info.csg_id == 0x505860);
  }
  else
  {
    if (csg_ind)
    TF_ASSERT ( cgi_success_reqi->target_cell_info.csg_id == SYS_CSG_ID_INVALID);
  }
  
  if (csg_member_status_included)
  {
     TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.
                 additionalSI_Info_r9.m.csg_MemberStatus_r9Present == 1);
    
     TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.
                 additionalSI_Info_r9.csg_MemberStatus_r9 == 0);
  }
  if (measResults.measResultServFreqList_r10.n > 1)
  {
    TF_ASSERT(measResults.measResultServFreqList_r10.n == 5);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[0].m.measResultSCell_r10Present);
    TF_ASSERT(!measResults.measResultServFreqList_r10.elem[0].m.measResultBestNeighCell_r10Present);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[0].servFreqId_r10 == 1);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[0].measResultSCell_r10.rsrpResultSCell_r10 == 97);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[0].measResultSCell_r10.rsrqResultSCell_r10 == 34);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[1].m.measResultSCell_r10Present);
    TF_ASSERT(!measResults.measResultServFreqList_r10.elem[1].m.measResultBestNeighCell_r10Present);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[1].servFreqId_r10 == 2);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[1].measResultSCell_r10.rsrpResultSCell_r10 == 96);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[1].measResultSCell_r10.rsrqResultSCell_r10 == 33);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[2].m.measResultSCell_r10Present);
    TF_ASSERT(!measResults.measResultServFreqList_r10.elem[2].m.measResultBestNeighCell_r10Present);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[2].servFreqId_r10 == 3);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[2].measResultSCell_r10.rsrpResultSCell_r10 == 95);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[2].measResultSCell_r10.rsrqResultSCell_r10 == 32);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[3].m.measResultSCell_r10Present);
    TF_ASSERT(!measResults.measResultServFreqList_r10.elem[3].m.measResultBestNeighCell_r10Present);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[3].servFreqId_r10 == 4);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[3].measResultSCell_r10.rsrpResultSCell_r10 == 94);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[3].measResultSCell_r10.rsrqResultSCell_r10 == 31);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[4].m.measResultSCell_r10Present);
    TF_ASSERT(!measResults.measResultServFreqList_r10.elem[4].m.measResultBestNeighCell_r10Present);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[4].servFreqId_r10 == 5);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[4].measResultSCell_r10.rsrpResultSCell_r10 == 93);
    TF_ASSERT(measResults.measResultServFreqList_r10.elem[4].measResultSCell_r10.rsrqResultSCell_r10 == 30);
  }

} /* lte_rrc_meas_test_meas_report_for_cgi */

/*===========================================================================

                                UNIT TEST

===========================================================================*/

/* Test to verify the processing of a MeasConfig that adds a cell for CGI reporting,
  handling of unsolicited SIB for the CGI cell from SIB module and report indication
  from ML1 containing the measurement results for CGI reporting */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIReportSuccess);

void LteRrcMeasConnEutraCGIReportSuccess::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIReportSuccess::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_unsolicited_sib_indi_s unsolicited_sib_ind;
  lte_rrc_osys_PLMN_IdentityList2 *plmn_IdentityList;
  uint16 cgi_pci = 150;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Set the release to Rel 9
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel9;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);


  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send unsolicited sib ind containing SIB1 of the CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);

  // set phy cell id to same as that of the CGI cell
  unsolicited_sib_ind.cell_id.phy_cell_id = 150;
  unsolicited_sib_ind.cell_id.freq = 0x100;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Check if T321 is stopped
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Test Meas SM sending LTE_CPHY_SERV_CELL_MEAS_REQ and receiving the CNF
  lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf();

  // Test the meas report sent by Meas containing CGI Info
  lte_rrc_meas_test_meas_report_for_cgi(cgi_pci, TRUE, TRUE,TRUE, FALSE);

  plmn_IdentityList = &ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
    plmn_IdentityList;

  // Verify secondary PLMN identity list
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].
            cgi_Info.m.plmn_IdentityListPresent);

  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.m.locationInfo_r10Present == 0);

  // Verify MCC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList->elem[0].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[0] == 1);
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[2] == 3);

  // Verify MCC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList->elem[1].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[0] == 0);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[1] == 9);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[2] == 1);

  // Verify MNC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[2] == 1);

  // Verify MCC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList->elem[2].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[0] == 0);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[1] == 9);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[2] == 1);

  // Verify MNC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[0] == 5);
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[1] == 1);
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[2] == 0);

  lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal();

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

}

void LteRrcMeasConnEutraCGIReportSuccess::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIReportLocInfoSuccess);

void LteRrcMeasConnEutraCGIReportLocInfoSuccess::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIReportLocInfoSuccess::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_unsolicited_sib_indi_s unsolicited_sib_ind;
  lte_rrc_osys_PLMN_IdentityList2 *plmn_IdentityList;
  uint16 cgi_pci = 150;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Set the release to Rel 9
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel11;

#ifdef FEATURE_LTE_MDT
     TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
#endif

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_loc_info_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_loc_info_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);


  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send unsolicited sib ind containing SIB1 of the CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);

  // set phy cell id to same as that of the CGI cell
  unsolicited_sib_ind.cell_id.phy_cell_id = 150;
  unsolicited_sib_ind.cell_id.freq = 0x100;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Check if T321 is stopped
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Test Meas SM sending LTE_CPHY_SERV_CELL_MEAS_REQ and receiving the CNF
  lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf();

  // Test the meas report sent by Meas containing CGI Info
  lte_rrc_meas_test_meas_report_for_cgi(cgi_pci, TRUE, TRUE,TRUE, FALSE);

  plmn_IdentityList = &ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
    plmn_IdentityList;

  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.m.locationInfo_r10Present == 0);
  lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal();

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_loc_info_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_loc_info_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);


  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send unsolicited sib ind containing SIB1 of the CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);

  // set phy cell id to same as that of the CGI cell
  unsolicited_sib_ind.cell_id.phy_cell_id = 150;
  unsolicited_sib_ind.cell_id.freq = 0x100;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Check if T321 is stopped
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Test Meas SM sending LTE_CPHY_SERV_CELL_MEAS_REQ and receiving the CNF
  lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf();

  // Test the meas report sent by Meas containing CGI Info
  lte_rrc_meas_test_meas_report_for_cgi(cgi_pci, TRUE, TRUE,TRUE, FALSE);

  plmn_IdentityList = &ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
    plmn_IdentityList;

  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.m._v3ExtPresent);

  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.m.locationInfo_r10Present);

#if 0
    TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.locationInfo_r10.locationCoordinates_r10.t == 
              T_lte_rrc_osys_LocationInfo_r10_locationCoordinates_r10_ellipsoid_Point_r10);
#endif

  lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal();

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);
}

void LteRrcMeasConnEutraCGIReportLocInfoSuccess::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig that adds a cell for CGI reporting (on B29),
  handling of unsolicited SIB for the CGI cell from SIB module and report indication
  from ML1 containing the measurement results for CGI reporting */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIReportSuccessOnB29);

void LteRrcMeasConnEutraCGIReportSuccessOnB29::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIReportSuccessOnB29::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_unsolicited_sib_indi_s unsolicited_sib_ind;
  lte_rrc_osys_PLMN_IdentityList2 *plmn_IdentityList;
  uint16 cgi_pci = 150;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Set the release to Rel 9
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel10;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg_on_b29,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg_on_b29),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 9710);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_50);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);


  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send unsolicited sib ind containing SIB1 of the CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);

  // set phy cell id to same as that of the CGI cell
  unsolicited_sib_ind.cell_id.phy_cell_id = 150;
  unsolicited_sib_ind.cell_id.freq = 9710;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Check if T321 is stopped
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Test Meas SM sending LTE_CPHY_SERV_CELL_MEAS_REQ and receiving the CNF
  lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf();

  // Test the meas report sent by Meas containing CGI Info
  lte_rrc_meas_test_meas_report_for_cgi(cgi_pci, TRUE, TRUE, TRUE, TRUE);

  plmn_IdentityList = &ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
    plmn_IdentityList;

  // Verify secondary PLMN identity list
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].
            cgi_Info.m.plmn_IdentityListPresent);

  // Verify MCC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList->elem[0].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[0] == 1);
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[2] == 3);

  // Verify MCC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList->elem[1].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[0] == 0);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[1] == 9);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[2] == 1);

  // Verify MNC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[2] == 1);

  // Verify MCC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList->elem[2].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[0] == 0);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[1] == 9);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[2] == 1);

  // Verify MNC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[0] == 5);
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[1] == 1);
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[2] == 0);

  lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal();

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

}

void LteRrcMeasConnEutraCGIReportSuccessOnB29::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIReportSuccessSib1Barred);

void LteRrcMeasConnEutraCGIReportSuccessSib1Barred::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIReportSuccessSib1Barred::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_unsolicited_sib_indi_s unsolicited_sib_ind;
  lte_rrc_osys_PLMN_IdentityList2 *plmn_IdentityList;
  uint16 cgi_pci = 150;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Set the release to Rel 9
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel9;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);


  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send unsolicited sib ind containing SIB1 of the CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);

  // set phy cell id to same as that of the CGI cell
  unsolicited_sib_ind.cell_id.phy_cell_id = 150;
  unsolicited_sib_ind.cell_id.freq = 0x100;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_barred_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_barred_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_barred_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_barred_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Check if T321 is stopped
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Test Meas SM sending LTE_CPHY_SERV_CELL_MEAS_REQ and receiving the CNF
  lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf();

  // Test the meas report sent by Meas containing CGI Info
  lte_rrc_meas_test_meas_report_for_cgi(cgi_pci, TRUE, TRUE,FALSE, FALSE);

  plmn_IdentityList = &ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
    plmn_IdentityList;

  // Verify secondary PLMN identity list
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].
            cgi_Info.m.plmn_IdentityListPresent);

  // Verify MCC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList->elem[0].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[0] == 1);
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[2] == 3);

  // Verify MCC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList->elem[1].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[0] == 0);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[1] == 9);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[2] == 1);

  // Verify MNC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[2] == 1);

  // Verify MCC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList->elem[2].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[0] == 0);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[1] == 9);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[2] == 1);

  // Verify MNC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[0] == 5);
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[1] == 1);
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[2] == 0);

  lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal();

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

}

void LteRrcMeasConnEutraCGIReportSuccessSib1Barred::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIReportSuccessSib1ReservedPlmn);

void LteRrcMeasConnEutraCGIReportSuccessSib1ReservedPlmn::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIReportSuccessSib1ReservedPlmn::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_unsolicited_sib_indi_s unsolicited_sib_ind;
  lte_rrc_osys_PLMN_IdentityList2 *plmn_IdentityList;
  uint16 cgi_pci = 150;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Set the release to Rel 9
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel9;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);


  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send unsolicited sib ind containing SIB1 of the CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);

  // set phy cell id to same as that of the CGI cell
  unsolicited_sib_ind.cell_id.phy_cell_id = 150;
  unsolicited_sib_ind.cell_id.freq = 0x100;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_reserved_plmn_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_reserved_plmn_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_reserved_plmn_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_reserved_plmn_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Check if T321 is stopped
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Test Meas SM sending LTE_CPHY_SERV_CELL_MEAS_REQ and receiving the CNF
  lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf();

  // Test the meas report sent by Meas containing CGI Info
  lte_rrc_meas_test_meas_report_for_cgi(cgi_pci, TRUE, TRUE, FALSE, FALSE);

  plmn_IdentityList = &ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
    plmn_IdentityList;

  // Verify secondary PLMN identity list
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].
            cgi_Info.m.plmn_IdentityListPresent);

  // Verify MCC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList->elem[0].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[0] == 1);
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[2] == 3);

  // Verify MCC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList->elem[1].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[0] == 0);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[1] == 9);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[2] == 1);

  // Verify MNC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[2] == 1);

  // Verify MCC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList->elem[2].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[0] == 0);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[1] == 9);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[2] == 1);

  // Verify MNC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[0] == 5);
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[1] == 1);
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[2] == 0);

  lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal();

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

}

void LteRrcMeasConnEutraCGIReportSuccessSib1ReservedPlmn::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIReportSuccessWithBestNeighResults);

void LteRrcMeasConnEutraCGIReportSuccessWithBestNeighResults::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIReportSuccessWithBestNeighResults::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_unsolicited_sib_indi_s unsolicited_sib_ind;
  lte_rrc_osys_PLMN_IdentityList2 *plmn_IdentityList;
  uint16 cgi_pci = 150;
  uint32 buf_len;
  lte_cphy_serv_cell_meas_req_s *serv_cell_meas_req;
  lte_cphy_serv_cell_meas_cnf_s serv_cell_meas_cnf;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Set the release to Rel 9
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel10;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->m.reportAddNeighMeas_r10Present = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->reportAddNeighMeas_r10 = 0;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);


  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send unsolicited sib ind containing SIB1 of the CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);

  // set phy cell id to same as that of the CGI cell
  unsolicited_sib_ind.cell_id.phy_cell_id = 150;
  unsolicited_sib_ind.cell_id.freq = 0x100;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Check if T321 is stopped
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting
  // and trigger report indication from ML1
  lte_rrc_utf_get_next_ext_msg((byte**)&serv_cell_meas_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_serv_cell_meas_req_s));
  TF_ASSERT(serv_cell_meas_req->msgr_hdr.id == LTE_CPHY_SERV_CELL_MEAS_REQ);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send serving cell measurement results to Meas
  lte_rrc_init_default_hdr(&serv_cell_meas_cnf.msgr_hdr, LTE_CPHY_SERV_CELL_MEAS_CNF);
  serv_cell_meas_cnf.serving_rsrp = 80;
  serv_cell_meas_cnf.serving_rsrq = 28;
  serv_cell_meas_cnf.status = E_SUCCESS;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.num_scell_list = 5;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[0].serv_cell_index = 1;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[0].meas_report_scell_present = TRUE;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[0].meas_report_scell.serving_rsrp = 97;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[0].meas_report_scell.serving_rsrq = 34;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[0].meas_report_best_neighCell_present = FALSE;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[1].serv_cell_index = 2;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[1].meas_report_scell_present = TRUE;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[1].meas_report_scell.serving_rsrp = 96;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[1].meas_report_scell.serving_rsrq = 33;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[1].meas_report_best_neighCell_present = FALSE;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[2].serv_cell_index = 3;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[2].meas_report_scell_present = TRUE;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[2].meas_report_scell.serving_rsrp = 95;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[2].meas_report_scell.serving_rsrq = 32;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[2].meas_report_best_neighCell_present = FALSE;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[3].serv_cell_index = 4;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[3].meas_report_scell_present = TRUE;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[3].meas_report_scell.serving_rsrp = 94;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[3].meas_report_scell.serving_rsrq = 31;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[3].meas_report_best_neighCell_present = FALSE;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[4].serv_cell_index = 5;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[4].meas_report_scell_present = TRUE;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[4].meas_report_scell.serving_rsrp = 93;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[4].meas_report_scell.serving_rsrq = 30;
  serv_cell_meas_cnf.meas_report_ca_srv_freq_list.meas_report_scell_list[4].meas_report_best_neighCell_present = FALSE;
  lte_rrc_utf_send_msg((byte*)&serv_cell_meas_cnf, sizeof(serv_cell_meas_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();

  // Test the meas report sent by Meas containing CGI Info
  lte_rrc_meas_test_meas_report_for_cgi(cgi_pci, TRUE, TRUE, TRUE, FALSE);

  plmn_IdentityList = &ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
    plmn_IdentityList;

  // Verify secondary PLMN identity list
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].
            cgi_Info.m.plmn_IdentityListPresent);

  // Verify MCC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList->elem[0].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[0] == 1);
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[2] == 3);

  // Verify MCC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList->elem[1].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[0] == 0);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[1] == 9);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[2] == 1);

  // Verify MNC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[2] == 1);

  // Verify MCC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList->elem[2].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[0] == 0);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[1] == 9);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[2] == 1);

  // Verify MNC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[0] == 5);
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[1] == 1);
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[2] == 0);

  lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal();

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

}

void LteRrcMeasConnEutraCGIReportSuccessWithBestNeighResults::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraEmptyCGIRpt);

void LteRrcMeasConnEutraEmptyCGIRpt::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraEmptyCGIRpt::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_osys_MeasResults measResults;
  lte_rrc_tmri_s *t321_tmri_ptr = NULL;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg_without_cgi_cell,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg_without_cgi_cell),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == FALSE);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  //Let T321 expire
  TF_SLEEP(1000);
  TF_MSG("T321 timer expires");

  // Catch the T321 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t321_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(t321_tmri_ptr->msg_hdr.id == LTE_RRC_T321_TMRI);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Test Meas SM sending LTE_CPHY_SERV_CELL_MEAS_REQ and receiving the CNF
  lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf();

  // We expect meas sm to have thrown the ul msg reqi containing the meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->message.t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.t 
            == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->m.nonCriticalExtensionPresent == 0);

  measResults = ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults;

  // Verify the actual measResults for WCDMA
  TF_ASSERT(measResults.m.measResultNeighCellsPresent);
  TF_ASSERT(measResults.measId == 5);
  TF_ASSERT(measResults.measResultPCell.rsrpResult == 80);
  TF_ASSERT(measResults.measResultPCell.rsrqResult == 28);

  // Verify CGI info
  TF_ASSERT(measResults.measResultNeighCells.t == T_lte_rrc_osys_MeasResults_measResultNeighCells_measResultListEUTRA);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].m.cgi_InfoPresent == 0);

    // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting
  // and trigger report indication from ML1
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);
}

void LteRrcMeasConnEutraEmptyCGIRpt::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraUnsolicitedSIBafterCGIConfig);

void LteRrcMeasConnEutraUnsolicitedSIBafterCGIConfig::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraUnsolicitedSIBafterCGIConfig::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_unsolicited_sib_indi_s unsolicited_sib_ind;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send unsolicited sib ind containing SIB1 of the CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);

  // set phy cell id to same as that of the CGI cell
  unsolicited_sib_ind.cell_id.phy_cell_id = 150;
  unsolicited_sib_ind.cell_id.freq = 0x100;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Check if T321 is stopped
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

}

void LteRrcMeasConnEutraUnsolicitedSIBafterCGIConfig::Teardown()
{
  //  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


/* Test to verify the processing of a MeasConfig that configures CGI when not
  supported */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIReportNotSupported);

void LteRrcMeasConnEutraCGIReportNotSupported::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIReportNotSupported::Test()
{
  lte_rrc_osys_MeasConfig *meas_cfg;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_meas_test_stub__get_fgi_bit_cgi_not_supported;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);

  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
}

void LteRrcMeasConnEutraCGIReportNotSupported::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig that adds a cell for CGI reporting,
   followed by T321 expiry before acquiring SIB1 of the CGI cell */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIReportT321Expiry);

void LteRrcMeasConnEutraCGIReportT321Expiry::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIReportT321Expiry::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_osys_MeasResults measResults;
  lte_rrc_tmri_s *t321_tmri_ptr = NULL;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  //Let T321 expire
  TF_SLEEP(1000);
  TF_MSG("T321 timer expires");

  // Catch the T321 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t321_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(t321_tmri_ptr->msg_hdr.id == LTE_RRC_T321_TMRI);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Test Meas SM sending LTE_CPHY_SERV_CELL_MEAS_REQ and receiving the CNF
  lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf();

  // We expect meas sm to have thrown the ul msg reqi containing the meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *) ul_msg_reqi_ptr->ul_msg_ptr;

  // Check basic validations for UL DCCH message
  TF_ASSERT(ul_dcch_msg_ptr->message.t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.t 
            == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->
            t == T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->m.nonCriticalExtensionPresent == 0);

  measResults = ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults;

  // Verify the actual measResults for WCDMA
  TF_ASSERT(measResults.m.measResultNeighCellsPresent);
  TF_ASSERT(measResults.measId == 5);
  TF_ASSERT(measResults.measResultPCell.rsrpResult == 80);
  TF_ASSERT(measResults.measResultPCell.rsrqResult == 28);

  // Verify CGI info
  TF_ASSERT(measResults.measResultNeighCells.t == T_lte_rrc_osys_MeasResults_measResultNeighCells_measResultListEUTRA);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].m.cgi_InfoPresent == 0);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].physCellId == 150);

    // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting
  // and trigger report indication from ML1
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

}

void LteRrcMeasConnEutraCGIReportT321Expiry::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig that adds a cell for CGI reporting,
   followed by another measConfig that removes the measId tied to CGI reporting */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIMeasIdRemoval);

void LteRrcMeasConnEutraCGIMeasIdRemoval::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIMeasIdRemoval::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send another measConfig that removes the measId tied to CGI reporting
  // Populate the next measConfig that removes neighbor cells that were added previously
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  meas_cfg->m.measIdToRemoveListPresent = 1;

  meas_cfg->measIdToRemoveList.elem = (lte_rrc_osys_MeasId *)
   lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasId));
  meas_cfg->measIdToRemoveList.n = 1;
  meas_cfg->measIdToRemoveList.elem[0] = 5;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();
  
  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting
  // and trigger report indication from ML1
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
}

void LteRrcMeasConnEutraCGIMeasIdRemoval::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig that adds a cell for CGI reporting,
   followed by another measConfig that removes the measObj tied to CGI reporting */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIMeasObjRemoval);

void LteRrcMeasConnEutraCGIMeasObjRemoval::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIMeasObjRemoval::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send another measConfig that removes the measId tied to CGI reporting
  // Populate the next measConfig that removes neighbor cells that were added previously
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  meas_cfg->m.measObjectToRemoveListPresent = 1;

  meas_cfg->measObjectToRemoveList.elem = (lte_rrc_osys_MeasObjectId *)
   lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectId));
  meas_cfg->measObjectToRemoveList.n = 1;
  meas_cfg->measObjectToRemoveList.elem[0] = 5;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();
  
  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting
  // and trigger report indication from ML1
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
}

void LteRrcMeasConnEutraCGIMeasObjRemoval::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig that adds a cell for CGI reporting,
   followed by another measConfig that removes the reportConfig tied to CGI reporting */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIReportConfigRemoval);

void LteRrcMeasConnEutraCGIReportConfigRemoval::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIReportConfigRemoval::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send another measConfig that removes the measId tied to CGI reporting
  // Populate the next measConfig that removes neighbor cells that were added previously
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  meas_cfg->m.reportConfigToRemoveListPresent = 1;

  meas_cfg->reportConfigToRemoveList.elem = (lte_rrc_osys_ReportConfigId *)
   lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigId));
  meas_cfg->reportConfigToRemoveList.n = 1;
  meas_cfg->reportConfigToRemoveList.elem[0] = 5;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();
  
  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting
  // and trigger report indication from ML1
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
}

void LteRrcMeasConnEutraCGIReportConfigRemoval::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig that adds a cell for CGI reporting,
   followed by another measConfig that modifies the measObj tied to CGI reporting,
   by changing the PCI of the CGI cell */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIMeasObjModification);

void LteRrcMeasConnEutraCGIMeasObjModification::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIMeasObjModification::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_unsolicited_sib_indi_s unsolicited_sib_ind;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send another measConfig that removes the measId tied to CGI reporting
  // Populate the next measConfig that removes neighbor cells that were added previously
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  meas_cfg->m.measObjectToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem = (lte_rrc_osys_MeasObjectToAddMod *)
   lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod));
  meas_cfg->measObjectToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 5;
  meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 0x100;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellForWhichToReportCGI = 120;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();
  
  // Check if T321 is stopped
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 120);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect Meas to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send unsolicited sib ind containing SIB1 of the CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);
  // set phy cell id to same as that of the CGI cell
  unsolicited_sib_ind.cell_id.phy_cell_id = 150;
  unsolicited_sib_ind.cell_id.freq = 0x100;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect Meas to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
}

void LteRrcMeasConnEutraCGIMeasObjModification::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig that adds a cell for CGI reporting,
   followed by another measConfig that modifies the reportConfig tied to CGI reporting */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIReportCfgModification);

void LteRrcMeasConnEutraCGIReportCfgModification::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIReportCfgModification::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send another measConfig that removes the measId tied to CGI reporting
  // Populate the next measConfig that removes neighbor cells that were added previously
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  meas_cfg->m.reportConfigToAddModListPresent = 1;

  meas_cfg->reportConfigToAddModList.elem = (lte_rrc_osys_ReportConfigToAddMod *)
   lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigToAddMod));
  meas_cfg->reportConfigToAddModList.n = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfigId = 5;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.t = T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA = 
    (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));
#if 0
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->
    triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
    triggerType.u.periodical = (lte_rrc_osys_ReportConfigInterRAT_triggerType_periodical * )
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigInterRAT_triggerType_periodical));
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.u.periodical->
    purpose = lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 1;
#endif  

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();
  
  // Check if T321 is stopped
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
}

void LteRrcMeasConnEutraCGIReportCfgModification::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


/* Test to verify the processing of a MeasConfig that adds a cell for CGI reporting,
   followed by another measConfig that modifies the measId tied to CGI reporting */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIMeasIdModification);

void LteRrcMeasConnEutraCGIMeasIdModification::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIMeasIdModification::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_unsolicited_sib_indi_s unsolicited_sib_ind;
  lte_cphy_conn_meas_report_ind_s cphy_meas_report_ind;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_osys_MeasResults measResults;
  uint16 cgi_pci = 150;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Change the CGI PCI to 120
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellForWhichToReportCGI = 120;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 120);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send another measConfig that removes the measId tied to CGI reporting
  // Populate the next measConfig that removes neighbor cells that were added previously
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  meas_cfg->m.measObjectToAddModListPresent = 1;
  meas_cfg->m.reportConfigToAddModListPresent = 1;
  meas_cfg->m.measIdToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem = (lte_rrc_osys_MeasObjectToAddMod *)
   lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod));
  meas_cfg->measObjectToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 5;
  meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 0x100;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellForWhichToReportCGI = 150;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  meas_cfg->reportConfigToAddModList.elem = (lte_rrc_osys_ReportConfigToAddMod *)
   lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigToAddMod));
  meas_cfg->reportConfigToAddModList.n = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfigId = 1;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.t = T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA = 
    (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->
    triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->
    reportQuantity = lte_rrc_osys_ReportConfigEUTRA_reportQuantity_both;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->
    triggerType.u.periodical = (lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical * )
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical));
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.u.periodical->
    purpose = lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;
  meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 1;

  meas_cfg->measIdToAddModList.elem = (lte_rrc_osys_MeasIdToAddMod *)
   lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod) * 2);
  meas_cfg->measIdToAddModList.n = 2;
  meas_cfg->measIdToAddModList.elem[0].measId = 5;
  meas_cfg->measIdToAddModList.elem[0].measObjectId = 5;
  meas_cfg->measIdToAddModList.elem[0].reportConfigId = 5;
  meas_cfg->measIdToAddModList.elem[1].measId = 1;
  meas_cfg->measIdToAddModList.elem[1].measObjectId = 5;
  meas_cfg->measIdToAddModList.elem[1].reportConfigId = 1;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();
  
  // Check if T321 is stopped
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x11);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x11);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x11);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x01);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect Meas to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send unsolicited sib ind containing SIB1 of the old CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);
 
  unsolicited_sib_ind.cell_id.phy_cell_id = 120;
  unsolicited_sib_ind.cell_id.freq = 0x100;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // We expect Meas to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  memset(&cphy_meas_report_ind, 0, sizeof(lte_cphy_conn_meas_report_ind_s));
  // Send the report indication for a non-CGI report
  lte_rrc_init_default_hdr(&cphy_meas_report_ind.msgr_hdr, LTE_CPHY_MEAS_REPORT_IND);
  cphy_meas_report_ind.meas_id = 1;
  cphy_meas_report_ind.auto_delete_meas_id = FALSE;
  cphy_meas_report_ind.serving_rsrp = 40;
  cphy_meas_report_ind.serving_rsrq = 34;
  cphy_meas_report_ind.neigh_results.size = 1;
  cphy_meas_report_ind.neigh_results.type = LTE_CPHY_CONN_MEAS_REPORT_TYPE_EUTRA;
  cphy_meas_report_ind.neigh_results.list.eutra[0].pci = 150;
  cphy_meas_report_ind.neigh_results.list.eutra[0].rsrp = 25;
  cphy_meas_report_ind.neigh_results.list.eutra[0].rsrq = 20;

  lte_rrc_utf_send_msg((byte*)&cphy_meas_report_ind, sizeof(cphy_meas_report_ind));

  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the ul msg reqi containing the non-CGI meas report 
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *) ul_msg_reqi_ptr->ul_msg_ptr;

  measResults = ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults;

  // Verify the actual measResults
  TF_ASSERT(measResults.m.measResultNeighCellsPresent);
  TF_ASSERT(measResults.measId == 1);
  TF_ASSERT(measResults.measResultPCell.rsrpResult == 40);
  TF_ASSERT(measResults.measResultPCell.rsrqResult == 34);
    // Verify measResults 
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.m.rsrpResultPresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.m.rsrqResultPresent);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.
            rsrpResult == 25);
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].measResult.
            rsrqResult == 20);

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  // Send unsolicited sib ind containing SIB1 of the CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);

  // set phy cell id to same as that of the CGI cell
  unsolicited_sib_ind.cell_id.phy_cell_id = 150;
  unsolicited_sib_ind.cell_id.freq = 0x100;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_one_plmn_only_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_one_plmn_only_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_one_plmn_only_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_one_plmn_only_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Check if T321 is stopped
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Test Meas SM sending LTE_CPHY_SERV_CELL_MEAS_REQ and receiving the CNF
  lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf();
  
  // Test the Meas SM handling of report ind for CGI from ML1
  lte_rrc_meas_test_meas_report_for_cgi(cgi_pci, FALSE, FALSE, TRUE, FALSE);

  measResults = ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults;

  // Verify secondary PLMN identity list
  TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].
            cgi_Info.m.plmn_IdentityListPresent == 0);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting
  // and trigger report indication from ML1
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 1);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x11);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Send a report ind that is not related to CGI, this should be ignored
  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);
  
}

void LteRrcMeasConnEutraCGIMeasIdModification::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


/* Test to verify the processing of a MeasConfig that adds a cell for CGI reporting,
   followed by handover */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIMeasIdRemovalForHO);

void LteRrcMeasConnEutraCGIMeasIdRemovalForHO::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIMeasIdRemovalForHO::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_empty_reconfig_msg,
                                       sizeof(lte_rrc_meas_test_empty_reconfig_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    TRUE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_HANDOVER;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();
  
  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting
  // and trigger report indication from ML1
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
}

void LteRrcMeasConnEutraCGIMeasIdRemovalForHO::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig that adds a cell for CGI reporting,
   followed by RLF */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIMeasIdRemovalForRLF);

void LteRrcMeasConnEutraCGIMeasIdRemovalForRLF::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIMeasIdRemovalForRLF::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_REAPPLY_CONFIG;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();
  
  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting
  // and trigger report indication from ML1
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
}

void LteRrcMeasConnEutraCGIMeasIdRemovalForRLF::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


/* Test to verify the processing of a MeasConfig that adds a cell for CGI reporting,
  handling of unsolicited SIB for the CGI cell from SIB module just after T321 expiry
  and before getting the report ind for serving cell measurements from ML1 */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraUnsolicitedSIBafterT321Expiry);

void LteRrcMeasConnEutraUnsolicitedSIBafterT321Expiry::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraUnsolicitedSIBafterT321Expiry::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_serv_cell_meas_req_s *serv_cell_meas_req;
  lte_cphy_serv_cell_meas_cnf_s serv_cell_meas_cnf;
  lte_rrc_unsolicited_sib_indi_s unsolicited_sib_ind;
  lte_rrc_osys_PLMN_IdentityList2 *plmn_IdentityList_ptr;
  lte_rrc_tmri_s *t321_tmri_ptr = NULL;
  uint16 cgi_pci = 150;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    //Let T321 expire
  TF_SLEEP(1000);
  TF_MSG("T321 timer expires");

  // Catch the T321 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t321_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(t321_tmri_ptr->msg_hdr.id == LTE_RRC_T321_TMRI);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting
  // and trigger report indication from ML1
  lte_rrc_utf_get_next_ext_msg((byte**)&serv_cell_meas_req, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_serv_cell_meas_req_s));
  TF_ASSERT(serv_cell_meas_req->msgr_hdr.id == LTE_CPHY_SERV_CELL_MEAS_REQ);

  // Send unsolicited sib ind containing SIB1 of the CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);

  // set phy cell id to same as that of the CGI cell
  unsolicited_sib_ind.cell_id.phy_cell_id = 150;
  unsolicited_sib_ind.cell_id.freq = 0x100;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_max_plmn_id_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_max_plmn_id_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_max_plmn_id_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_max_plmn_id_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));
  
  // We wait for Meas to process the unsolicited_sib_ind
  lte_rrc_utf_wait_for_done();

  // Send serving cell measurement results to Meas
  lte_rrc_init_default_hdr(&serv_cell_meas_cnf.msgr_hdr, LTE_CPHY_SERV_CELL_MEAS_CNF);
  serv_cell_meas_cnf.serving_rsrp = 80;
  serv_cell_meas_cnf.serving_rsrq = 28;
  serv_cell_meas_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&serv_cell_meas_cnf, sizeof(serv_cell_meas_cnf));

  // Test the Meas SM handling of report ind for CGI from ML1
  lte_rrc_meas_test_meas_report_for_cgi(cgi_pci, FALSE, FALSE,TRUE, FALSE);

  plmn_IdentityList_ptr = &ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
    plmn_IdentityList;

  // Verify secondary PLMN identity list
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].
            cgi_Info.m.plmn_IdentityListPresent);

  // Verify MCC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList_ptr->elem[0].m.mccPresent);
  TF_ASSERT(plmn_IdentityList_ptr->elem[0].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList_ptr->elem[0].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList_ptr->elem[0].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList_ptr->elem[0].mnc.elem[0] == 1);
  TF_ASSERT(plmn_IdentityList_ptr->elem[0].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList_ptr->elem[0].mnc.elem[2] == 3);

  // Verify MCC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList_ptr->elem[1].m.mccPresent);
  TF_ASSERT(plmn_IdentityList_ptr->elem[1].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList_ptr->elem[1].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList_ptr->elem[1].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList_ptr->elem[1].mnc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList_ptr->elem[1].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList_ptr->elem[1].mnc.elem[2] == 1);

  // Verify MCC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList_ptr->elem[2].m.mccPresent);
  TF_ASSERT(plmn_IdentityList_ptr->elem[2].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList_ptr->elem[2].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList_ptr->elem[2].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList_ptr->elem[2].mnc.elem[0] == 5);
  TF_ASSERT(plmn_IdentityList_ptr->elem[2].mnc.elem[1] == 1);
  TF_ASSERT(plmn_IdentityList_ptr->elem[2].mnc.elem[2] == 0);

  // Verify MCC in PLMN-id 5
  TF_ASSERT(plmn_IdentityList_ptr->elem[3].m.mccPresent);
  TF_ASSERT(plmn_IdentityList_ptr->elem[3].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList_ptr->elem[3].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList_ptr->elem[3].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 5
  TF_ASSERT(plmn_IdentityList_ptr->elem[3].mnc.elem[0] == 7);
  TF_ASSERT(plmn_IdentityList_ptr->elem[3].mnc.elem[1] == 1);
  TF_ASSERT(plmn_IdentityList_ptr->elem[3].mnc.elem[2] == 4);

  // Verify MCC in PLMN-id 6
  TF_ASSERT(plmn_IdentityList_ptr->elem[4].m.mccPresent);
  TF_ASSERT(plmn_IdentityList_ptr->elem[4].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList_ptr->elem[4].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList_ptr->elem[4].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 6
  TF_ASSERT(plmn_IdentityList_ptr->elem[4].mnc.elem[0] == 8);
  TF_ASSERT(plmn_IdentityList_ptr->elem[4].mnc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList_ptr->elem[4].mnc.elem[2] == 8);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal();

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

}

void LteRrcMeasConnEutraUnsolicitedSIBafterT321Expiry::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig that adds a cell for CGI reporting,
  handling of T321 expiry just after receiving unsolicited SIB for the CGI cell */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraT321ExpiryAfterUnsolicitedSIB);

void LteRrcMeasConnEutraT321ExpiryAfterUnsolicitedSIB::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraT321ExpiryAfterUnsolicitedSIB::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_cphy_serv_cell_meas_req_s *serv_cell_meas_req;
  lte_cphy_serv_cell_meas_cnf_s serv_cell_meas_cnf;
  lte_rrc_unsolicited_sib_indi_s unsolicited_sib_ind;
  lte_rrc_osys_PLMN_IdentityList2 *plmn_IdentityList_ptr;
  lte_rrc_tmri_s *t321_tmri_ptr = NULL;
  uint16 cgi_pci = 150;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send unsolicited sib ind containing SIB1 of the CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);

  // set phy cell id to same as that of the CGI cell
  unsolicited_sib_ind.cell_id.phy_cell_id = 150;
  unsolicited_sib_ind.cell_id.freq = 0x100;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_max_plmn_id_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_max_plmn_id_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_max_plmn_id_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_max_plmn_id_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));

  //Let T321 expire
  TF_SLEEP(1000);
  TF_MSG("T321 timer expires");

  // Catch the T321 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t321_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(t321_tmri_ptr->msg_hdr.id == LTE_RRC_T321_TMRI);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting
  // and trigger report indication from ML1
  lte_rrc_utf_get_next_ext_msg((byte**)&serv_cell_meas_req, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_cphy_serv_cell_meas_req_s));
  TF_ASSERT(serv_cell_meas_req->msgr_hdr.id == LTE_CPHY_SERV_CELL_MEAS_REQ);

  // Send serving cell measurement results to Meas
  lte_rrc_init_default_hdr(&serv_cell_meas_cnf.msgr_hdr, LTE_CPHY_SERV_CELL_MEAS_CNF);
  serv_cell_meas_cnf.serving_rsrp = 80;
  serv_cell_meas_cnf.serving_rsrq = 28;
  serv_cell_meas_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&serv_cell_meas_cnf, sizeof(serv_cell_meas_cnf));

  // Test the Meas SM handling of report ind for CGI from ML1
  lte_rrc_meas_test_meas_report_for_cgi(cgi_pci, FALSE, FALSE,TRUE, FALSE);

  plmn_IdentityList_ptr = &ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
    plmn_IdentityList;

  // Verify secondary PLMN identity list
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].
            cgi_Info.m.plmn_IdentityListPresent);

  // Verify MCC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList_ptr->elem[0].m.mccPresent);
  TF_ASSERT(plmn_IdentityList_ptr->elem[0].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList_ptr->elem[0].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList_ptr->elem[0].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList_ptr->elem[0].mnc.elem[0] == 1);
  TF_ASSERT(plmn_IdentityList_ptr->elem[0].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList_ptr->elem[0].mnc.elem[2] == 3);

  // Verify MCC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList_ptr->elem[1].m.mccPresent);
  TF_ASSERT(plmn_IdentityList_ptr->elem[1].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList_ptr->elem[1].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList_ptr->elem[1].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList_ptr->elem[1].mnc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList_ptr->elem[1].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList_ptr->elem[1].mnc.elem[2] == 1);

  // Verify MCC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList_ptr->elem[2].m.mccPresent);
  TF_ASSERT(plmn_IdentityList_ptr->elem[2].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList_ptr->elem[2].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList_ptr->elem[2].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList_ptr->elem[2].mnc.elem[0] == 5);
  TF_ASSERT(plmn_IdentityList_ptr->elem[2].mnc.elem[1] == 1);
  TF_ASSERT(plmn_IdentityList_ptr->elem[2].mnc.elem[2] == 0);

  // Verify MCC in PLMN-id 5
  TF_ASSERT(plmn_IdentityList_ptr->elem[3].m.mccPresent);
  TF_ASSERT(plmn_IdentityList_ptr->elem[3].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList_ptr->elem[3].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList_ptr->elem[3].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 5
  TF_ASSERT(plmn_IdentityList_ptr->elem[3].mnc.elem[0] == 7);
  TF_ASSERT(plmn_IdentityList_ptr->elem[3].mnc.elem[1] == 1);
  TF_ASSERT(plmn_IdentityList_ptr->elem[3].mnc.elem[2] == 4);

  // Verify MCC in PLMN-id 6
  TF_ASSERT(plmn_IdentityList_ptr->elem[4].m.mccPresent);
  TF_ASSERT(plmn_IdentityList_ptr->elem[4].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList_ptr->elem[4].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList_ptr->elem[4].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 6
  TF_ASSERT(plmn_IdentityList_ptr->elem[4].mnc.elem[0] == 8);
  TF_ASSERT(plmn_IdentityList_ptr->elem[4].mnc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList_ptr->elem[4].mnc.elem[2] == 8);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal();

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

}

void LteRrcMeasConnEutraT321ExpiryAfterUnsolicitedSIB::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


/* Test to verify the processing of a MeasConfig that tries to add a measId for
  CGI reporting, when CGI reporting is already in progress */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraInvalidMeasConfigMultipleMeasIdForCGI);

void LteRrcMeasConnEutraInvalidMeasConfigMultipleMeasIdForCGI::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraInvalidMeasConfigMultipleMeasIdForCGI::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // Send another measConfig that adds another measId tied to CGI reporting
  // Populate the next measConfig that removes neighbor cells that were added previously
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  meas_cfg->m.measObjectToAddModListPresent = 1;
  meas_cfg->m.measIdToAddModListPresent = 1;

  meas_cfg->measObjectToAddModList.elem = (lte_rrc_osys_MeasObjectToAddMod *)
   lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod));
  meas_cfg->measObjectToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 0x100;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->cellForWhichToReportCGI = 120;

  meas_cfg->measIdToAddModList.elem = (lte_rrc_osys_MeasIdToAddMod *)
   lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod));
  meas_cfg->measIdToAddModList.n = 1;
  meas_cfg->measIdToAddModList.elem[0].measId = 1;
  meas_cfg->measIdToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measIdToAddModList.elem[0].reportConfigId = 5;

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);  
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
}

void LteRrcMeasConnEutraInvalidMeasConfigMultipleMeasIdForCGI::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig that requests CGI info for the,
  serving cell */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraServingCellCGI);

void LteRrcMeasConnEutraServingCellCGI::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraServingCellCGI::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_osys_PLMN_IdentityList2 *plmn_IdentityList;
  lte_rrc_tmri_s *t321_tmri_ptr = NULL;
  uint32 buf_len;
  uint16 cgi_pci = 16;
  uint8 int_queue_count;
  uint8 ext_queue_count;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Set the release to Rel 9
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel9;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_serving_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_serving_cgi_msg),
                                       &meas_cfg);

  // Point the SIB1 to be returned to the SIB1 of the serving cell
  lte_rrc_meas_test_decode_sib1((void*) lte_rrc_meas_test_eutra_cgi_sib1_msg,
                                sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg));

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == cgi_pci);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Verify T321 is started
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  //Let T321 expire
  TF_SLEEP(1000);
  TF_MSG("T321 timer expires");

  // Catch the T321 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t321_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(t321_tmri_ptr->msg_hdr.id == LTE_RRC_T321_TMRI);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Test Meas SM sending LTE_CPHY_SERV_CELL_MEAS_REQ and receiving the CNF
  lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf();

  // Test the meas report sent by Meas containing CGI Info
  lte_rrc_meas_test_meas_report_for_cgi(cgi_pci, TRUE, TRUE,TRUE, FALSE);

  plmn_IdentityList = &ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
    plmn_IdentityList;

  // Verify secondary PLMN identity list
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].
            cgi_Info.m.plmn_IdentityListPresent);

  // Verify MCC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList->elem[0].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[0] == 1);
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[2] == 3);

  // Verify MCC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList->elem[1].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[0] == 0);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[1] == 9);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[2] == 1);

  // Verify MNC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[2] == 1);

  // Verify MCC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList->elem[2].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[0] == 0);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[1] == 9);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[2] == 1);

  // Verify MNC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[0] == 5);
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[1] == 1);
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[2] == 0);

  lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal();

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  // Check for no further messages from meas
  lte_rrc_utf_check_no_more_msgs();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
}

void LteRrcMeasConnEutraServingCellCGI::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the processing of a MeasConfig after T321 expiry, that 
  modifies the measId and reportConfig id that were tied to CGI reporting. This tests
  the cleanup of CGI internal data */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIMeasIdReportConfigIdModifiedAfterT321Expiry);

void LteRrcMeasConnEutraCGIMeasIdReportConfigIdModifiedAfterT321Expiry::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIMeasIdReportConfigIdModifiedAfterT321Expiry::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_osys_MeasResults measResults;
  lte_rrc_tmri_s *t321_tmri_ptr = NULL;
  uint32 buf_len = 0, i = 0;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  for (i = 0; i < 20; i++)
  {
    // Point the meas_config to be returned to the typical meas_config
    lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                         sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                         &meas_cfg);

    // Validate meas cfg
    TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                      FALSE) == E_SUCCESS);

    // Send the meas config req to meas sm
    lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
    meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
    meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
    lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
    
    // We wait for Meas to process the meas cfg indi
    lte_rrc_utf_wait_for_done();

    // We expect meas sm to have thrown the conn meas cfg req
    lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
    TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

    // Verify cell info
    TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
    TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

    // Verify valid and mod bitmasks
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
    TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

    // Verify meas obj info
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

    // Verify reportConfig 
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
              type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
              purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

    // Verify MeasId
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
    TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

    // Verify quantityConfig
    TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
    TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Check if T321 is running
    TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

    // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
    lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

    // Free allocated memory for reconfig message
    lte_rrc_meas_test_free_reconfig_msg_ptr();

    // We expect RRC to be in connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    //Let T321 expire
    TF_SLEEP(1000);
    TF_MSG("T321 timer expires");

    // Catch the T321 Timer expiry 
    lte_rrc_utf_get_next_int_msg((byte **)&t321_tmri_ptr,&buf_len);
    // Make sure the timer indication is valid 
    TF_ASSERT(t321_tmri_ptr->msg_hdr.id == LTE_RRC_T321_TMRI);

    // Check if T321 is running
    TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

    // Test Meas SM sending LTE_CPHY_SERV_CELL_MEAS_REQ and receiving the CNF
    lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf();

    // We expect meas sm to have thrown the ul msg reqi containing the meas report
    lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
    TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
    TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
    TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
    TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

    ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *) ul_msg_reqi_ptr->ul_msg_ptr;

    // Check basic validations for UL DCCH message
    TF_ASSERT(ul_dcch_msg_ptr->message.t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
    TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->t == T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport);

    measResults = ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
              measurementReport_r8->measResults;

    // Verify the actual measResults for WCDMA
    TF_ASSERT(measResults.m.measResultNeighCellsPresent);
    TF_ASSERT(measResults.measId == 5);
    TF_ASSERT(measResults.measResultPCell.rsrpResult == 80);
    TF_ASSERT(measResults.measResultPCell.rsrqResult == 28);

    // Verify CGI info
    TF_ASSERT(measResults.measResultNeighCells.t == T_lte_rrc_osys_MeasResults_measResultNeighCells_measResultListEUTRA);
    TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].m.cgi_InfoPresent == 0);
    TF_ASSERT(measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].physCellId == 150);

      // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // Meas should send conn meas cfg req to delete measId tied to CGI reporting
    // and trigger report indication from ML1
    lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
    TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

    // Verify cell info
    TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
    TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

    // Verify valid and mod bitmasks
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
    TF_ASSERT(cfg_req->other_mod_bitmask == 0);

    // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
    lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

    /* Free the memory assigned from RRC heap to the ASN1 PDU */
    lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                               lte_rrc_osys_UL_DCCH_Message_PDU);

    // Send another measConfig that modifies the reportConfig that was tied to CGI reporting
    lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                         sizeof(lte_rrc_meas_test_meas_config_msg1),
                                         &meas_cfg);

    meas_cfg->m.reportConfigToAddModListPresent = 1;
    meas_cfg->m.measIdToAddModListPresent = 1;

    meas_cfg->reportConfigToAddModList.elem = (lte_rrc_osys_ReportConfigToAddMod *)
     lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigToAddMod));
    meas_cfg->reportConfigToAddModList.n = 1;
    meas_cfg->reportConfigToAddModList.elem[0].reportConfigId = 5;
    meas_cfg->reportConfigToAddModList.elem[0].reportConfig.t = T_lte_rrc_osys_ReportConfigToAddMod_reportConfig_reportConfigEUTRA;
    meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA = 
      (lte_rrc_osys_ReportConfigEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigEUTRA));
#if 0
    meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->
      triggerType.t = T_lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical;
    meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigInterRAT->
      triggerType.u.periodical = (lte_rrc_osys_ReportConfigInterRAT_triggerType_periodical * )
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_ReportConfigInterRAT_triggerType_periodical));
    meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->triggerType.u.periodical->
      purpose = lte_rrc_osys_ReportConfigEUTRA_triggerType_periodical_purpose_reportStrongestCells;
    meas_cfg->reportConfigToAddModList.elem[0].reportConfig.u.reportConfigEUTRA->maxReportCells = 1;
#endif    

      meas_cfg->measIdToAddModList.elem = (lte_rrc_osys_MeasIdToAddMod *)
     lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasIdToAddMod) * 2);
    meas_cfg->measIdToAddModList.n = 1;
    meas_cfg->measIdToAddModList.elem[0].measId = 5;
    meas_cfg->measIdToAddModList.elem[0].measObjectId = 5;
    meas_cfg->measIdToAddModList.elem[0].reportConfigId = 5;

    // Validate meas cfg
    TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                      FALSE) == E_SUCCESS);

    // Send the meas config req to meas sm
    lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
    meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
    meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
    lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
    
    // We wait for Meas to process the meas cfg indi
    lte_rrc_utf_wait_for_done();
    
    // Check if T321 is stopped
    TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

    // We expect RRC to be in the wait_for_connected_config state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

    // We expect meas sm to have thrown the conn meas cfg req
    lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
    TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

    // Verify cell info
    TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
    TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

    // Verify valid and mod bitmasks
    TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
    TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
    TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
    TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
    TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
    TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
    TF_ASSERT(cfg_req->other_mod_bitmask == 0);

    // Verify reportConfig 
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
              type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
    TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
              purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL);

    // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
    lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

    // We expect RRC to be in connected state
    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

    // Free allocated memory for reconfig message
    lte_rrc_meas_test_free_reconfig_msg_ptr();

    // Check RRC does not send any more messages 
    lte_rrc_utf_check_no_more_msgs();

  }

}

void LteRrcMeasConnEutraCGIMeasIdReportConfigIdModifiedAfterT321Expiry::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

/* Test to verify the handling of race condition between the processing of 
  T321 expiry and HO */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraHODurringT321ExpiryHandling);

void LteRrcMeasConnEutraHODurringT321ExpiryHandling::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraHODurringT321ExpiryHandling::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_tmri_s *t321_tmri_ptr = NULL;
  lte_cphy_serv_cell_meas_req_s *serv_cell_meas_req;
  lte_cphy_serv_cell_meas_cnf_s serv_cell_meas_cnf;
  lte_rrc_unsolicited_sib_indi_s unsolicited_sib_ind;
  lte_rrc_osys_PLMN_IdentityList2 *plmn_IdentityList;
  uint16 cgi_pci = 150;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Set the release to Rel 9
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel9;

#ifdef FEATURE_FEMTO_CSG
  TF_STUB(lte_rrc_csg_asf_check_csg_in_whitelist) = lte_rrc_meas_test_stub__csg_not_in_whitelist;
#endif
  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  //Let T321 expire
  TF_SLEEP(1000);
  TF_MSG("T321 timer expires");

  // Catch the T321 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t321_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(t321_tmri_ptr->msg_hdr.id == LTE_RRC_T321_TMRI);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting
  // and trigger report indication from ML1
  lte_rrc_utf_get_next_ext_msg((byte**)&serv_cell_meas_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_serv_cell_meas_req_s));
  TF_ASSERT(serv_cell_meas_req->msgr_hdr.id == LTE_CPHY_SERV_CELL_MEAS_REQ);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_ho_with_cgi_msg,
                                       sizeof(lte_rrc_meas_test_eutra_ho_with_cgi_msg),
                                       &meas_cfg);

  // Send HO message that has CGI report configuration in it
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    TRUE) == E_SUCCESS);

  // Send the meas config req to meas sm for handover
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_HANDOVER;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Check if T321 is not started
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send serving cell measurement results to Meas
  lte_rrc_init_default_hdr(&serv_cell_meas_cnf.msgr_hdr, LTE_CPHY_SERV_CELL_MEAS_CNF);
  serv_cell_meas_cnf.serving_rsrp = 80;
  serv_cell_meas_cnf.serving_rsrq = 28;
  serv_cell_meas_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&serv_cell_meas_cnf, sizeof(serv_cell_meas_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Meas should send conn meas cfg req to delete measId tied to CGI reporting since
  // periodical reporting should be deleted as part of HO processing
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0);
  TF_ASSERT(cfg_req->other_mod_bitmask == 0);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send unsolicited sib ind containing SIB1 of the CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);

  // set phy cell id to same as that of the CGI cell
  unsolicited_sib_ind.cell_id.phy_cell_id = 150;
  unsolicited_sib_ind.cell_id.freq = 0x100;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Check if T321 is stopped
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Test Meas SM sending LTE_CPHY_SERV_CELL_MEAS_REQ and receiving the CNF
  lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf();

  // Test the meas report sent by Meas containing CGI Info
  lte_rrc_meas_test_meas_report_for_cgi(cgi_pci, TRUE, FALSE,TRUE, FALSE);

  plmn_IdentityList = &ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
    plmn_IdentityList;

  // Verify secondary PLMN identity list
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].
            cgi_Info.m.plmn_IdentityListPresent);

  // Verify MCC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList->elem[0].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[1] == 5);
  TF_ASSERT(plmn_IdentityList->elem[0].mcc.elem[2] == 7);

  // Verify MNC in PLMN-id 2
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[0] == 1);
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList->elem[0].mnc.elem[2] == 3);

  // Verify MCC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList->elem[1].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[0] == 0);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[1] == 9);
  TF_ASSERT(plmn_IdentityList->elem[1].mcc.elem[2] == 1);

  // Verify MNC in PLMN-id 3
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[0] == 3);
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[1] == 2);
  TF_ASSERT(plmn_IdentityList->elem[1].mnc.elem[2] == 1);

  // Verify MCC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList->elem[2].m.mccPresent);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[0] == 0);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[1] == 9);
  TF_ASSERT(plmn_IdentityList->elem[2].mcc.elem[2] == 1);

  // Verify MNC in PLMN-id 4
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[0] == 5);
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[1] == 1);
  TF_ASSERT(plmn_IdentityList->elem[2].mnc.elem[2] == 0);

  lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal();

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  lte_rrc_utf_check_no_more_msgs();

}

void LteRrcMeasConnEutraHODurringT321ExpiryHandling::Teardown()
{
//  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIReportConfigIdModified);

void LteRrcMeasConnEutraCGIReportConfigIdModified::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIReportConfigIdModified::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_osys_MeasResults measResults;
  lte_rrc_tmri_s *t321_tmri_ptr = NULL;
  uint32 buf_len = 0, i = 0;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_reconfig_msg1,
                                       sizeof(lte_rrc_meas_test_eutra_reconfig_msg1),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                   FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
    
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A3);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  //send reconfig2 msg
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_reconfig_msg2,
                                       sizeof(lte_rrc_meas_test_eutra_reconfig_msg2),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                   FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
    
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == 1);

  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // Check RRC does not send any more messages 
  lte_rrc_utf_check_no_more_msgs();

}

void LteRrcMeasConnEutraCGIReportConfigIdModified::Teardown()
{
  //  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIReportWithSIHOReq);

void LteRrcMeasConnEutraCGIReportWithSIHOReq::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIReportWithSIHOReq::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_unsolicited_sib_indi_s unsolicited_sib_ind;
  lte_rrc_osys_PLMN_IdentityList2 *plmn_IdentityList;
  uint16 cgi_pci = 150;
  uint32 buf_len;
  lte_rrc_sib_extnd_tmr_reqi_s *tmr_extnd_req = NULL;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Set the release to Rel 9
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel9;

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_with_sihoreq_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_with_sihoreq_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x00);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x00);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);


  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].si_request_for_ho == TRUE);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = NULL;
  //meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_AUTO_GAP_CONFIG;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));

  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg ( (byte**) &tmr_extnd_req, &buf_len );
  TF_ASSERT ( buf_len == sizeof ( lte_rrc_sib_extnd_tmr_reqi_s ) );
  TF_ASSERT ( tmr_extnd_req->msg_hdr.id == LTE_RRC_SIB_EXTND_TMR_REQI );

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x10);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Send unsolicited sib ind containing SIB1 of the CGI cell
  lte_rrc_init_default_hdr(&unsolicited_sib_ind.msg_hdr, LTE_RRC_UNSOLICITED_SIB_INDI);

  // set phy cell id to same as that of the CGI cell
  unsolicited_sib_ind.cell_id.phy_cell_id = 150;
  unsolicited_sib_ind.cell_id.freq = 0x100;
  LTE_RRC_MEMCPY(unsolicited_sib_ind.encoded_si, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg),
                 lte_rrc_meas_test_eutra_cgi_sib1_msg, 
                 sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg));
  
  unsolicited_sib_ind.encoded_si_size = sizeof(lte_rrc_meas_test_eutra_cgi_sib1_msg);
  lte_rrc_utf_send_msg((byte*)&unsolicited_sib_ind, sizeof(unsolicited_sib_ind));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // Check if T321 is stopped
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Test Meas SM sending LTE_CPHY_SERV_CELL_MEAS_REQ and receiving the CNF
  lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf();

  // Test the meas report sent by Meas containing CGI Info
  lte_rrc_meas_test_meas_report_for_cgi(cgi_pci, TRUE, TRUE,TRUE, FALSE);

  plmn_IdentityList = &ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
    measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].cgi_Info.
    plmn_IdentityList;

  // Verify secondary PLMN identity list
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.
            measurementReport_r8->measResults.measResultNeighCells.u.measResultListEUTRA->elem[0].
            cgi_Info.m.plmn_IdentityListPresent == 0);

  lte_rrc_meas_test_cphy_meas_req_for_cgi_meas_id_removal();

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

}

void LteRrcMeasConnEutraCGIReportWithSIHOReq::Teardown()
{
  //  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraCGIReportWithSIHOReqAndT321Expiry);

void LteRrcMeasConnEutraCGIReportWithSIHOReqAndT321Expiry::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
      LTE_CPHY_SERV_CELL_MEAS_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraCGIReportWithSIHOReqAndT321Expiry::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  lte_rrc_unsolicited_sib_indi_s unsolicited_sib_ind;
  lte_rrc_osys_PLMN_IdentityList2 *plmn_IdentityList;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_reqi_ptr = NULL;
  lte_rrc_tmri_s *t321_tmri_ptr = NULL;
  uint16 cgi_pci = 150;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_cgi_with_sihoreq_msg,
                                       sizeof(lte_rrc_meas_test_eutra_cgi_with_sihoreq_msg),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x10);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify meas obj info
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].type == LTE_CPHY_MEAS_OBJ_TYPE_EUTRA);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.carrier_freq == 0x100);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_25);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.presence_antenna_port1 == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.freq_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.neigh_cells.active_bitmask == 3);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.blacklisted_cells.active_bitmask == 0);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_obj_list[4].obj_info.eutra.report_cgi_cell_id == 150);


  // Verify reportConfig 
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.
            type == LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].trig_cfg.data.periodical.
            purpose == LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI);
  TF_ASSERT(cfg_req->meas_cfg_ptr->report_cfg_list[4].si_request_for_ho == TRUE);

  // Verify MeasId
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].meas_obj_id == 5);
  TF_ASSERT(cfg_req->meas_cfg_ptr->meas_id_list[4].report_cfg_id == 5);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == TRUE);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // We expect RRC to be in connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  TF_SLEEP(150);

  // Check if T321 is running
  TF_ASSERT(lte_rrc_meas_data.dd_ptr->t321_running == FALSE);

  // Catch the T321 Timer expiry 
  lte_rrc_utf_get_next_int_msg((byte **)&t321_tmri_ptr,&buf_len);
  // Make sure the timer indication is valid 
  TF_ASSERT(t321_tmri_ptr->msg_hdr.id == LTE_RRC_T321_TMRI);

  /* Free the memory assigned from RRC heap to the ASN1 PDU */
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, 
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  // Test Meas SM sending LTE_CPHY_SERV_CELL_MEAS_REQ and receiving the CNF
  lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf();

  //TF_ASSERT(lte_rrc_meas_data.dd_ptr->cgi_cfg_is_pended == FALSE);

  // We expect meas sm to have thrown the ul msg reqi containing the meas report
  lte_rrc_utf_get_next_int_msg((byte**)&ul_msg_reqi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_send_ul_msg_reqi_s));
  TF_ASSERT(ul_msg_reqi_ptr->msg_hdr.id == LTE_RRC_SEND_UL_MSG_REQI);
  TF_ASSERT(ul_msg_reqi_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_msg_reqi_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  TF_SLEEP(100);

}

void LteRrcMeasConnEutraCGIReportWithSIHOReqAndT321Expiry::Teardown()
{
  //  lte_rrc_meas_test_free_meas_config();
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasConnEutraReportCfgChangedToCgi);

void LteRrcMeasConnEutraReportCfgChangedToCgi::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasConnEutraReportCfgChangedToCgi::Test()
{
  lte_rrc_meas_cfg_reqi_s meas_cfg_reqi;
  lte_cphy_conn_meas_cfg_req_s *cfg_req;
  lte_rrc_osys_MeasConfig *meas_cfg;
  uint32 buf_len;

  // Get Meas SM to connected state
  lte_rrc_meas_test_get_meas_sm_to_connected();

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_rpt_cfg_unmodified,
                                       sizeof(lte_rrc_meas_test_eutra_rpt_cfg_unmodified),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  // Send the meas config req to meas sm
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));
  
  // We wait for Meas to process the meas cfg indi
  lte_rrc_utf_wait_for_done();

  // We expect meas sm to have thrown the conn meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_conn_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_CONN_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify valid and mod bitmasks
  TF_ASSERT(cfg_req->meas_obj_valid_bitmask == 0x3);
  TF_ASSERT(cfg_req->meas_id_valid_bitmask == 0x3);
  TF_ASSERT(cfg_req->report_cfg_valid_bitmask == 0x1);
  TF_ASSERT(cfg_req->meas_obj_mod_bitmask == 0x3);
  TF_ASSERT(cfg_req->meas_id_mod_bitmask == 0x3);
  TF_ASSERT(cfg_req->report_cfg_mod_bitmask == 0x1);
  TF_ASSERT(cfg_req->other_mod_bitmask == LTE_CPHY_MEAS_CFG_BIT_QTY_CFG);

  // Verify quantityConfig
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrp == 2);
  TF_ASSERT(cfg_req->meas_cfg_ptr->qty_cfg.eutra.fc_rsrq == 2);

  // We expect RRC to be in the wait_for_connected_config state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // Send CPHY_MEAS_CONFIG_CNF to ML1 and verify if Meas sends RRC_MEAS_CFG_CNFI
  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi();

  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  // Send another measConfig that adds another measId tied to CGI reporting
  // Populate the next measConfig that removes neighbor cells that were added previously
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_eutra_rpt_cfg_modified_to_cgi,
                                       sizeof(lte_rrc_meas_test_eutra_rpt_cfg_modified_to_cgi),
                                       &meas_cfg);

  // Validate meas cfg
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_BAD_DATA);  
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();
  
}

void LteRrcMeasConnEutraReportCfgChangedToCgi::Teardown()
{
  lte_rrc_utf_teardown();
}



