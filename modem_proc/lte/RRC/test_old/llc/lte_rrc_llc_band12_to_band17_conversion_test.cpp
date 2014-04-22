/*!
  @file lte_rrc_llc_band12_to_band17_conversion_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of a DL/UL earfcn 
  in band12. These should be converted to the appropriate band17 values.

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
06/23/10   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "__lte_rrc_llc_int.h"
#include "lte_rrc_llci.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_sib.h"
#include "lte_static_rrc_mem_util.h"
}

#include "lte_rrc_llc_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>  
#include <TestFramework.h>

extern "C"
{
  TF_PROTO_STUB(boolean, lte_rrc_sib_b12_to_b17_conversion_is_needed, void);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_camped_cell_bw, lte_bandwidth_e *);
}

/*===========================================================================

                                INTERNAL DEFINITIONS

===========================================================================*/
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcBand12ToBand17ConversionTest);

void LteRrcLlcBand12ToBand17ConversionTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL,0);
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_b12_to_b17_conversion_is_needed) 
  = lte_rrc_llc_test_sib_b12_to_b17_conversion_is_needed;
  TF_STUB(lte_rrc_csp_get_camped_cell_bw) = lte_rrc_llc_test_csp_get_camped_cell_bw2;
}

void LteRrcLlcBand12ToBand17ConversionTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_msg_ptr = NULL;
  lte_rrc_osys_SystemInformationBlockType2* sib2_ptr = NULL;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_MobilityControlInfo* mobility_ctrl_info_ptr = NULL;
  lte_rrc_cell_info_s cell_info;
  lte_rrc_llc_s *llc_ptr;
  lte_rrc_cfg_db_type *cfg_db_ptr;
  uint16 i = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: Band12 to Band17 Conversion test begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  llc_ptr->dd_ptr->cfg_ptr_in_use = CFG_PTR_IN_USE_OC;

  /* Validate the band12 to band17 conversion while reading SIB2 */

  encoded_pdu.length = sizeof(llc_basic_sib2_2);
  encoded_pdu.value = llc_basic_sib2_2;
  
  bcch_dl_sch_msg_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               BCCH_DL_SCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(bcch_dl_sch_msg_ptr != NULL);
  sib2_ptr = bcch_dl_sch_msg_ptr->message.u.c1->u.systemInformation->criticalExtensions.u.
    systemInformation_r8->sib_TypeAndInfo.elem->u.sib2;
  sib2_ptr->freqInfo.ul_CarrierFreq = 23100;
  cell_info.cell_id.freq = 5770;
  cell_info.cell_id.phy_cell_id = 1;
  llc_ptr->dd_ptr->cc_ptr->ul_freq_cfg.ul_cell_frequency = 23770;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_freq(sib2_ptr, llc_ptr->dd_ptr->oc_ptr, cell_info, llc_ptr));
  /* Make sure the UL earfcn corresponds to band 17 */
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->ul_freq_cfg.ul_cell_frequency == 23770);

  /* Validate the band12 to band17 conversion during handover */
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg);
  encoded_pdu.value = llc_reconfig_ho_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* ---------------------------------------------------------------------- */

  /* Validating Target cell Info IE */

  mobility_ctrl_info_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo;

  llc_ptr->dd_ptr->cmd_db.cell_info.cell_id.freq = 5770;
  mobility_ctrl_info_ptr->carrierFreq.dl_CarrierFreq = 5100;
  mobility_ctrl_info_ptr->carrierFreq.ul_CarrierFreq = 23100;
  llc_ptr->dd_ptr->cc_ptr->ul_freq_cfg.ul_cell_frequency = 23770;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_target_cell_info(mobility_ctrl_info_ptr, llc_ptr));
  /* Make sure the DL/UL earfcn corresponds to band 17 */
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->ho_target_cell_info.dl_cell_frequency == 5770);
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->ul_freq_cfg.ul_cell_frequency == 23770);

  TF_MSG("\nLLC: Band12 to Band17 Conversion test Ends \n");
  lte_rrc_osys_asn1_free_pdu(bcch_dl_sch_msg_ptr, 
                             lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_ptr, 
                             lte_rrc_osys_DL_DCCH_Message_PDU);

  lte_rrc_utf_reset(); 
  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcBand12ToBand17ConversionTest::Teardown()
{
  lte_rrc_utf_teardown();
}









