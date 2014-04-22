
/*!
  @file lte_rrc_llc_full_config_ho_validation_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the validation LLC does for
  HO to LTE or Full Config HO.

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
03/20/12   mm     Initial Revision
           
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

/*===========================================================================

                                INTERNAL DEFINITIONS

===========================================================================*/
/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

extern "C"
{
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
 // TF_PROTO_STUB(boolean, lte_rrc_config_ho_to_eutra_in_progress, void);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcFullConfigHOValidationTest);

void LteRrcLlcFullConfigHOValidationTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL,0);
  lte_rrc_llc_test_stub_init();
  /* Set the release version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
 // TF_STUB(lte_rrc_config_ho_to_eutra_in_progress) = lte_rrc_llc_test_ho_to_eutra_in_progress_true;
}

void LteRrcLlcFullConfigHOValidationTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs* conn_recfg_ptr = NULL;
  lte_rrc_llc_s *llc_ptr;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: Full Config HO validation test begins \n");

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  
  /* Set encoed msg to Rel 9 Full Config HO */ 
  encoded_pdu.length = sizeof(llc_reconfig_rel9_ho_full_config_msg);
  encoded_pdu.value = llc_reconfig_rel9_ho_full_config_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  conn_recfg_ptr = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8;

  /* ---------------------------------------------------------------------- */
  /* Since original message has all required IEs, the validation should pass */
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_validate_reconfig_msg_for_full_config(conn_recfg_ptr));

  /* Mask off RACH common config and see if the validation fails */
  conn_recfg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.rach_ConfigCommonPresent = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_reconfig_msg_for_full_config(conn_recfg_ptr));

  /* Mask off PDSCH common config and see if the validation fails */
  conn_recfg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.rach_ConfigCommonPresent = 1;
  conn_recfg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.pdsch_ConfigCommonPresent = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_reconfig_msg_for_full_config(conn_recfg_ptr));

  /* Mask off PUCCH common config and see if the validation fails */
  conn_recfg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.pdsch_ConfigCommonPresent = 1;
  conn_recfg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.pucch_ConfigCommonPresent = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_reconfig_msg_for_full_config(conn_recfg_ptr));
  
  /* Mask off PHICH common config and see if the validation fails */
  conn_recfg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.pucch_ConfigCommonPresent = 1;
  conn_recfg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.phich_ConfigPresent = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_reconfig_msg_for_full_config(conn_recfg_ptr));

  /* Mask off AntennaInfo common config and see if the validation fails */
  conn_recfg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.phich_ConfigPresent = 1;
  conn_recfg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.antennaInfoCommonPresent = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_reconfig_msg_for_full_config(conn_recfg_ptr));
  
  /* Mask off SRS UL common config and see if the validation fails */
  conn_recfg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.antennaInfoCommonPresent = 1;
  conn_recfg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.soundingRS_UL_ConfigCommonPresent = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_reconfig_msg_for_full_config(conn_recfg_ptr));
  
  /* Mask off UL power common config and see if the validation fails */
  conn_recfg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.soundingRS_UL_ConfigCommonPresent = 1;
  conn_recfg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.uplinkPowerControlCommonPresent = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_reconfig_msg_for_full_config(conn_recfg_ptr));

  /* Mask off MAC main config and see if the validation fails */
  conn_recfg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.uplinkPowerControlCommonPresent = 0;
  conn_recfg_ptr->radioResourceConfigDedicated.m.mac_MainConfigPresent = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_reconfig_msg_for_full_config(conn_recfg_ptr));

  /* Mask off SRBToAddModList and see if the validation fails */
  conn_recfg_ptr->radioResourceConfigDedicated.m.mac_MainConfigPresent = 1;
  conn_recfg_ptr->radioResourceConfigDedicated.m.srb_ToAddModListPresent = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_reconfig_msg_for_full_config(conn_recfg_ptr));

  /* Mask off DRBToAddModList and see if the validation fails */
  conn_recfg_ptr->radioResourceConfigDedicated.m.srb_ToAddModListPresent = 1;
  conn_recfg_ptr->radioResourceConfigDedicated.m.drb_ToAddModListPresent = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_reconfig_msg_for_full_config(conn_recfg_ptr));

  /* Mask off carrierFreq IE and see if the validation fails */
  conn_recfg_ptr->radioResourceConfigDedicated.m.drb_ToAddModListPresent = 1;
  conn_recfg_ptr->mobilityControlInfo.m.carrierFreqPresent = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_target_cell_info(&conn_recfg_ptr->mobilityControlInfo, llc_ptr));

  /* Mask off carrierBandwidth IE and see if the validation fails */
  conn_recfg_ptr->mobilityControlInfo.m.carrierFreqPresent = 1;
  conn_recfg_ptr->mobilityControlInfo.m.carrierBandwidthPresent = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_target_cell_info(&conn_recfg_ptr->mobilityControlInfo, llc_ptr));

  /* Mask off additionalSpectrumEmission IE and see if the validation fails */
  conn_recfg_ptr->mobilityControlInfo.m.carrierBandwidthPresent = 1;
  conn_recfg_ptr->mobilityControlInfo.m.additionalSpectrumEmissionPresent = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_target_cell_info(&conn_recfg_ptr->mobilityControlInfo, llc_ptr));

  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_ptr, lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  
  TF_MSG("\nLLC: Full Config HO validation test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcFullConfigHOValidationTest::Teardown()
{
  lte_rrc_utf_teardown();
}
