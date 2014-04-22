/*!
  @file lte_rrc_llc_phy_common_ie_enum_values_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of all PHY 
  common config IEs, including those that come as part of mobility control
  information, with enum values, for all possible values and other range value
  checks.

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
07/13/09   mm     Initial Revision
           
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
  TF_PROTO_STUB(lte_rrc_div_duplex_e, lte_rrc_csp_get_last_camped_mode, void);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
#ifdef FEATURE_LTE_SPS
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcSPSAndTTIBundlingTest);

void LteRrcLlcSPSAndTTIBundlingTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;
}

void LteRrcLlcSPSAndTTIBundlingTest::Test()
{

  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_PDSCH_ConfigDedicated *pdsch_cfg_ptr = NULL;
  lte_rrc_osys_PUCCH_ConfigDedicated *pucch_cfg_ptr = NULL;
  lte_rrc_osys_SoundingRS_UL_ConfigDedicated *srs_cfg_ptr = NULL;
  lte_rrc_osys_UplinkPowerControlDedicated *ul_pwr_ctrl_ptr = NULL;
  lte_rrc_osys_TPC_PDCCH_Config* tpc_pdcch_cfg_pucch_ptr = NULL, *tpc_pdcch_cfg_pusch_ptr = NULL;
  lte_rrc_osys_PhysicalConfigDedicated* phy_ded_cfg_ptr = NULL;
  lte_rrc_osys_CQI_ReportConfig* cqi_rep_ptr = NULL;
  lte_rrc_osys_SchedulingRequestConfig* sr_cfg_ptr = NULL;
  lte_rrc_osys_SPS_Config* sps_cfg_ptr = NULL;
  ASN1BitStr32 *osys_bitstring_ptr = NULL ;
  lte_rrc_osys_AntennaInfoDedicated_codebookSubsetRestriction_n4TxAntenna_tm4 *cbsr_n4_tm4_ptr = NULL;
  lte_rrc_llc_s *llc_ptr;
  lte_rrc_cfg_db_type *cfg_db_ptr;
  uint16 i = 0;
  int decode_status = 0;
  lte_rrc_osys_RadioResourceConfigDedicated *cfg_ded_ptr = NULL;


  /*FDD mode with TTI set to config 0*/
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));


  /* FDD mode with TTI set to config 1*/
  
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  llc_ptr->dd_ptr->oc_ptr->tdd_cfg.subframe_assign = LTE_L1_TDD_UL_DL_CFG_1;
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));

  /* FDD mode with TTI set to config 2*/
  
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  llc_ptr->dd_ptr->oc_ptr->tdd_cfg.subframe_assign = LTE_L1_TDD_UL_DL_CFG_2;
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));


  /* FDD mode with TTI set to config 6*/
  
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  llc_ptr->dd_ptr->oc_ptr->tdd_cfg.subframe_assign = LTE_L1_TDD_UL_DL_CFG_6;
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));


  /* FDD mode with reconfig1: SPS enabled, with reconfig2: TTI enabled and SPS set to release*/
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg1);
  encoded_pdu.value = llc_reconfig_ho_msg1;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  /* Validating all SPS Config IE values */
  sps_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                 criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                 radioResourceConfigDedicated.sps_Config;

  llc_ptr->dd_ptr->oc_ptr->ulsch_cfg.tti_bundling = FALSE;
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_SPS_INTERVAL; i++)
  {
    sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
    sps_cfg_ptr->sps_ConfigUL.u.setup->semiPersistSchedIntervalUL = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  }
  /* Checking the spare value */
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));

  /* Checking the invalid p0_NominalPUSCH_Persistent value */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MIN_P0_NOMINAL_PUSCH - 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  
  /* Check for SPS DL Config being not present */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH;
  i = 0;
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  /* Reset lte_rrc_sps_ConfigurationDL_present bitmask */
  sps_cfg_ptr->m.sps_ConfigDLPresent = 0;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_dl.valid == FALSE)

  /* Check SPS Config UL disable handling */
  sps_cfg_ptr->sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_ul.enable == FALSE);

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;
  
  cfg_ded_ptr->m.sps_ConfigPresent = 1;

  cfg_ded_ptr->sps_Config.m.sps_ConfigDLPresent = 1;

  cfg_ded_ptr->sps_Config.sps_ConfigDL.t = T_lte_rrc_osys_SPS_ConfigDL_release;

  cfg_ded_ptr->sps_Config.m.sps_ConfigULPresent = 1;

  cfg_ded_ptr->sps_Config.sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));


  /* FDD mode with reconfig1: SPS enabled, with reconfig2: TTI enabled and SPS set to setup*/
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg1);
  encoded_pdu.value = llc_reconfig_ho_msg1;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  /* Validating all SPS Config IE values */
  sps_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                 criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                 radioResourceConfigDedicated.sps_Config;

  llc_ptr->dd_ptr->oc_ptr->ulsch_cfg.tti_bundling = FALSE;
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_SPS_INTERVAL; i++)
  {
    sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
    sps_cfg_ptr->sps_ConfigUL.u.setup->semiPersistSchedIntervalUL = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  }
  /* Checking the spare value */
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));

  /* Checking the invalid p0_NominalPUSCH_Persistent value */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MIN_P0_NOMINAL_PUSCH - 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  
  /* Check for SPS DL Config being not present */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH;
  i = 0;
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  /* Reset lte_rrc_sps_ConfigurationDL_present bitmask */
  sps_cfg_ptr->m.sps_ConfigDLPresent = 0;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_dl.valid == FALSE)

  /* Check SPS Config UL disable handling */
  sps_cfg_ptr->sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_ul.enable == FALSE);

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));


  /* FDD mode with reconfig1: SPS enabled, with reconfig2: TTI enabled and SPS set to 0*/
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg1);
  encoded_pdu.value = llc_reconfig_ho_msg1;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  /* Validating all SPS Config IE values */
  sps_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                 criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                 radioResourceConfigDedicated.sps_Config;

  llc_ptr->dd_ptr->oc_ptr->ulsch_cfg.tti_bundling = FALSE;
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_SPS_INTERVAL; i++)
  {
    sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
    sps_cfg_ptr->sps_ConfigUL.u.setup->semiPersistSchedIntervalUL = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  }
  /* Checking the spare value */
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));

  /* Checking the invalid p0_NominalPUSCH_Persistent value */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MIN_P0_NOMINAL_PUSCH - 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  
  /* Check for SPS DL Config being not present */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH;
  i = 0;
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  /* Reset lte_rrc_sps_ConfigurationDL_present bitmask */
  sps_cfg_ptr->m.sps_ConfigDLPresent = 0;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_dl.valid == FALSE)

  /* Check SPS Config UL disable handling */
  sps_cfg_ptr->sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_ul.enable == FALSE);

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;
  
  cfg_ded_ptr->m.sps_ConfigPresent = 0;

  cfg_ded_ptr->sps_Config.m.sps_ConfigDLPresent = 0;

  cfg_ded_ptr->sps_Config.m.sps_ConfigULPresent = 0;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));

  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;


  /* TDD mode with TTI set to config 5*/
  
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  cfg_db_ptr->sps_cfg.sps_cfg_ul.enable = FALSE;
  cfg_db_ptr->sps_cfg.sps_cfg_dl.enable = FALSE;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  llc_ptr->dd_ptr->oc_ptr->tdd_cfg.subframe_assign = LTE_L1_TDD_UL_DL_CFG_5;
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;

  cfg_ded_ptr->m.sps_ConfigPresent = 1;

  cfg_ded_ptr->sps_Config.m.sps_ConfigDLPresent = 1;

  cfg_ded_ptr->sps_Config.sps_ConfigDL.t = T_lte_rrc_osys_SPS_ConfigDL_release;

  cfg_ded_ptr->sps_Config.m.sps_ConfigULPresent = 1;

  cfg_ded_ptr->sps_Config.sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;
  
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));

  /*TDD mode with TTI set to config 0 with SPS config set to 0*/
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  llc_ptr->dd_ptr->oc_ptr->tdd_cfg.subframe_assign = LTE_L1_TDD_UL_DL_CFG_0;
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;

  cfg_ded_ptr->m.sps_ConfigPresent = 0;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));


  /*TDD mode with TTI set to config 0*/
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  llc_ptr->dd_ptr->oc_ptr->tdd_cfg.subframe_assign = LTE_L1_TDD_UL_DL_CFG_0;
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;

  cfg_ded_ptr->m.sps_ConfigPresent = 1;

  cfg_ded_ptr->sps_Config.m.sps_ConfigDLPresent = 1;

  cfg_ded_ptr->sps_Config.sps_ConfigDL.t = T_lte_rrc_osys_SPS_ConfigDL_release;

  cfg_ded_ptr->sps_Config.m.sps_ConfigULPresent = 1;

  cfg_ded_ptr->sps_Config.sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;
  
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));


  /* TDD mode with TTI set to config 1*/
  
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  llc_ptr->dd_ptr->oc_ptr->tdd_cfg.subframe_assign = LTE_L1_TDD_UL_DL_CFG_1;
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;

  cfg_ded_ptr->m.sps_ConfigPresent = 1;

  cfg_ded_ptr->sps_Config.m.sps_ConfigDLPresent = 1;

  cfg_ded_ptr->sps_Config.sps_ConfigDL.t = T_lte_rrc_osys_SPS_ConfigDL_release;

  cfg_ded_ptr->sps_Config.m.sps_ConfigULPresent = 1;

  cfg_ded_ptr->sps_Config.sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;
  
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));


  /* TDD mode with TTI set to config 6*/
  
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  llc_ptr->dd_ptr->oc_ptr->tdd_cfg.subframe_assign = LTE_L1_TDD_UL_DL_CFG_6;
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;

  cfg_ded_ptr->m.sps_ConfigPresent = 1;

  cfg_ded_ptr->sps_Config.m.sps_ConfigDLPresent = 1;

  cfg_ded_ptr->sps_Config.sps_ConfigDL.t = T_lte_rrc_osys_SPS_ConfigDL_release;

  cfg_ded_ptr->sps_Config.m.sps_ConfigULPresent = 1;

  cfg_ded_ptr->sps_Config.sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;
  
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));


  /* TDD mode with reconfig1: SPS enabled, with reconfig2: TTI enabled and SPS set to release*/
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg1);
  encoded_pdu.value = llc_reconfig_ho_msg1;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  /* Validating all SPS Config IE values */
  sps_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                 criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                 radioResourceConfigDedicated.sps_Config;

  llc_ptr->dd_ptr->oc_ptr->ulsch_cfg.tti_bundling = FALSE;
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_SPS_INTERVAL; i++)
  {
    sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
    sps_cfg_ptr->sps_ConfigUL.u.setup->semiPersistSchedIntervalUL = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  }
  /* Checking the spare value */
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));

  /* Checking the invalid p0_NominalPUSCH_Persistent value */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MIN_P0_NOMINAL_PUSCH - 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  
  /* Check for SPS DL Config being not present */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH;
  i = 0;
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  /* Reset lte_rrc_sps_ConfigurationDL_present bitmask */
  sps_cfg_ptr->m.sps_ConfigDLPresent = 0;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_dl.valid == FALSE)

  /* Check SPS Config UL disable handling */
  sps_cfg_ptr->sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_ul.enable == FALSE);

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;
  
  cfg_ded_ptr->m.sps_ConfigPresent = 1;

  cfg_ded_ptr->sps_Config.m.sps_ConfigDLPresent = 1;

  cfg_ded_ptr->sps_Config.sps_ConfigDL.t = T_lte_rrc_osys_SPS_ConfigDL_release;

  cfg_ded_ptr->sps_Config.m.sps_ConfigULPresent = 1;

  cfg_ded_ptr->sps_Config.sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));


  /* TDD mode with reconfig1: SPS enabled, with reconfig2: TTI enabled and SPS set to setup*/
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg1);
  encoded_pdu.value = llc_reconfig_ho_msg1;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  /* Validating all SPS Config IE values */
  sps_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                 criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                 radioResourceConfigDedicated.sps_Config;

  llc_ptr->dd_ptr->oc_ptr->ulsch_cfg.tti_bundling = FALSE;
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_SPS_INTERVAL; i++)
  {
    sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
    sps_cfg_ptr->sps_ConfigUL.u.setup->semiPersistSchedIntervalUL = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  }
  /* Checking the spare value */
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));

  /* Checking the invalid p0_NominalPUSCH_Persistent value */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MIN_P0_NOMINAL_PUSCH - 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  
  /* Check for SPS DL Config being not present */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH;
  i = 0;
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  /* Reset lte_rrc_sps_ConfigurationDL_present bitmask */
  sps_cfg_ptr->m.sps_ConfigDLPresent = 0;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_dl.valid == FALSE)

  /* Check SPS Config UL disable handling */
  sps_cfg_ptr->sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_ul.enable == FALSE);

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;

  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));


  /* TDD mode with reconfig1: SPS enabled, with reconfig2: TTI enabled and SPS not signalled*/
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg1);
  encoded_pdu.value = llc_reconfig_ho_msg1;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  /* Validating all SPS Config IE values */
  sps_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                 criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                 radioResourceConfigDedicated.sps_Config;

  llc_ptr->dd_ptr->oc_ptr->ulsch_cfg.tti_bundling = FALSE;
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_SPS_INTERVAL; i++)
  {
    sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
    sps_cfg_ptr->sps_ConfigUL.u.setup->semiPersistSchedIntervalUL = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  }
  /* Checking the spare value */
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));

  /* Checking the invalid p0_NominalPUSCH_Persistent value */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MIN_P0_NOMINAL_PUSCH - 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  
  /* Check for SPS DL Config being not present */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH;
  i = 0;
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  /* Reset lte_rrc_sps_ConfigurationDL_present bitmask */
  sps_cfg_ptr->m.sps_ConfigDLPresent = 0;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_dl.valid == FALSE)

  /* Check SPS Config UL disable handling */
  sps_cfg_ptr->sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_ul.enable == FALSE);

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;
  
  cfg_ded_ptr->m.sps_ConfigPresent = 0;

  cfg_ded_ptr->sps_Config.m.sps_ConfigDLPresent = 0;

  cfg_ded_ptr->sps_Config.m.sps_ConfigULPresent = 0;

  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));
  
  /* ---------------------------------------------------------------------- */


  /*TDD mode: Reconfig1: TTI enabled, Reconfig2: TTI Disabled, SPS set to setup*/
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;
  
  sps_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                 criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                 radioResourceConfigDedicated.sps_Config;

  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
    radioResourceConfigDedicated.m.sps_ConfigPresent = 1;

  sps_cfg_ptr->m.sps_ConfigDLPresent = 0;
  sps_cfg_ptr->m.sps_ConfigULPresent = 0;

  cfg_db_ptr->sps_cfg.sps_cfg_ul.enable = FALSE;
  cfg_db_ptr->sps_cfg.sps_cfg_dl.enable = FALSE;
  
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg1);
  encoded_pdu.value = llc_reconfig_ho_msg1;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  /* Validating all SPS Config IE values */
  sps_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                 criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                 radioResourceConfigDedicated.sps_Config;
  
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_SPS_INTERVAL; i++)
  {
    sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
    sps_cfg_ptr->sps_ConfigUL.u.setup->semiPersistSchedIntervalUL = i;
    TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  }
  /* Checking the spare value */
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));

  /* Checking the invalid p0_NominalPUSCH_Persistent value */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MIN_P0_NOMINAL_PUSCH - 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  
  /* Check for SPS DL Config being not present */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH;
  i = 0;
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  /* Reset lte_rrc_sps_ConfigurationDL_present bitmask */
  sps_cfg_ptr->m.sps_ConfigDLPresent = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_dl.valid == FALSE)

  /* Check SPS Config UL disable handling */
  sps_cfg_ptr->sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_ul.enable == FALSE);
  /* ---------------------------------------------------------------------- */

  
  /*TDD mode: Reconfig1: TTI enabled, Reconfig2: TTI Disabled, SPS set to release*/
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;
  sps_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                 criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                 radioResourceConfigDedicated.sps_Config;
  
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
    radioResourceConfigDedicated.m.sps_ConfigPresent = 1;

  sps_cfg_ptr->m.sps_ConfigDLPresent = 0;
  sps_cfg_ptr->m.sps_ConfigULPresent = 0;

  cfg_db_ptr->sps_cfg.sps_cfg_ul.enable = FALSE;
  cfg_db_ptr->sps_cfg.sps_cfg_dl.enable = FALSE;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg1);
  encoded_pdu.value = llc_reconfig_ho_msg1;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  /* Validating all SPS Config IE values */
  sps_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                 criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                 radioResourceConfigDedicated.sps_Config;

  sps_cfg_ptr->m.sps_ConfigDLPresent = 1;
  sps_cfg_ptr->m.sps_ConfigULPresent = 1;
  sps_cfg_ptr->sps_ConfigDL.t = T_lte_rrc_osys_SPS_ConfigDL_release;
  sps_cfg_ptr->sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;
  
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_SPS_INTERVAL; i++)
  {
    sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
    sps_cfg_ptr->sps_ConfigUL.u.setup->semiPersistSchedIntervalUL = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  }
  /* Checking the spare value */
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));

  /* Checking the invalid p0_NominalPUSCH_Persistent value */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MIN_P0_NOMINAL_PUSCH - 1;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH + 1;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  
  /* Check for SPS DL Config being not present */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH;
  i = 0;
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  /* Reset lte_rrc_sps_ConfigurationDL_present bitmask */
  sps_cfg_ptr->m.sps_ConfigDLPresent = 0;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_dl.valid == FALSE)

  /* Check SPS Config UL disable handling */
  sps_cfg_ptr->sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_ul.enable == FALSE);
  /* ---------------------------------------------------------------------- */


  /*TDD mode: Reconfig1: TTI enabled, Reconfig2: TTI Disabled, SPS not signalled*/
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_TTI_msg);
  encoded_pdu.value = llc_reconfig_TTI_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;
  sps_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                 criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                 radioResourceConfigDedicated.sps_Config;

  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
    radioResourceConfigDedicated.m.sps_ConfigPresent = 1;

  sps_cfg_ptr->m.sps_ConfigDLPresent = 0;
  sps_cfg_ptr->m.sps_ConfigULPresent = 0;

  cfg_db_ptr->sps_cfg.sps_cfg_ul.enable = FALSE;
  cfg_db_ptr->sps_cfg.sps_cfg_dl.enable = FALSE;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg1);
  encoded_pdu.value = llc_reconfig_ho_msg1;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  /* Validating all SPS Config IE values */
  sps_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                 criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                 radioResourceConfigDedicated.sps_Config;

  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                 criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                 radioResourceConfigDedicated.m.sps_ConfigPresent = 0;

  sps_cfg_ptr->m.sps_ConfigDLPresent = 0;
  sps_cfg_ptr->m.sps_ConfigULPresent = 0;
  
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_SPS_INTERVAL; i++)
  {
    sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
    sps_cfg_ptr->sps_ConfigUL.u.setup->semiPersistSchedIntervalUL = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  }
  /* Checking the spare value */
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));

  /* Checking the invalid p0_NominalPUSCH_Persistent value */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MIN_P0_NOMINAL_PUSCH - 1;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH + 1;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  
  /* Check for SPS DL Config being not present */
  sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.
    p0_NominalPUSCH_Persistent = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH;
  i = 0;
  sps_cfg_ptr->sps_ConfigDL.u.setup->semiPersistSchedIntervalDL = i;
  /* Reset lte_rrc_sps_ConfigurationDL_present bitmask */
  sps_cfg_ptr->m.sps_ConfigDLPresent = 0;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_dl.valid == FALSE)

  /* Check SPS Config UL disable handling */
  sps_cfg_ptr->sps_ConfigUL.t = T_lte_rrc_osys_SPS_ConfigUL_release;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sps_cfg(cfg_db_ptr,sps_cfg_ptr,1));
  TF_ASSERT(cfg_db_ptr->sps_cfg.sps_cfg_ul.enable == FALSE);
  /* ---------------------------------------------------------------------- */
  
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_ptr, lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  
  lte_rrc_utf_reset(); 

}

void LteRrcLlcSPSAndTTIBundlingTest::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif

