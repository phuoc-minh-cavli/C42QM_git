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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPhyCommonIEEnumValuesTest);

void LteRrcLlcPhyCommonIEEnumValuesTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL,0);
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;
}

void LteRrcLlcPhyCommonIEEnumValuesTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_msg_ptr = NULL;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_MobilityControlInfo* mobility_ctrl_info_ptr = NULL;
  lte_rrc_osys_PDSCH_ConfigCommon pdsch_cfg_common;
  lte_rrc_osys_PRACH_Config prach_cfg;
  lte_rrc_osys_PRACH_ConfigSIB prach_cfg_sib;
  lte_rrc_osys_PUCCH_ConfigCommon pucch_cfg_common;
  lte_rrc_osys_PUSCH_ConfigCommon pusch_cfg_common;
  lte_rrc_osys_PHICH_Config phich_cfg;
  lte_rrc_osys_SoundingRS_UL_ConfigCommon srs_cfg_common;
  lte_rrc_osys_UplinkPowerControlCommon ul_pwr_ctrl_common;
  lte_rrc_osys_AntennaInfoCommon antenna_info_common;
  lte_rrc_osys_UE_TimersAndConstants tmr_and_const_param;
  lte_rrc_osys_MBSFN_SubframeConfigList *mbsfg_cfg_ptr;
  lte_rrc_osys_TDD_Config tdd_cfg;
  lte_rrc_llc_s *llc_ptr;
  lte_rrc_cfg_db_type *cfg_db_ptr;
  uint16 i = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: PHY Common Config IE Enum values test begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  llc_ptr->dd_ptr->cfg_ptr_in_use = CFG_PTR_IN_USE_OC;
  
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg);
  encoded_pdu.value = llc_reconfig_ho_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               sizeof(llc_reconfig_ho_msg),
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* ---------------------------------------------------------------------- */

  /* Validating PRACH Config IE values*/
  prach_cfg = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo.radioResourceConfigCommon.
    prach_Config;
  cfg_db_ptr->ho_config_in_progress = LTE_RRC_LLC_HANDOVER_FDD_INTRA_FREQ;

  cfg_db_ptr->mac_rach_cfg.l1_rach.prach_frequency_offset = 100;
  cfg_db_ptr->ul_freq_cfg.ul_bandwidth = LTE_BW_NRB_6;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_prach_offset_with_ul_bw(cfg_db_ptr));

  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;
  cfg_db_ptr->mac_rach_cfg.l1_rach.prach_cfg = 56;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_validate_prach_offset_with_ul_bw(cfg_db_ptr));

  cfg_db_ptr->ho_target_cell_info.dl_bandwidth = LTE_BW_NRB_100;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;

  prach_cfg.prach_ConfigInfo.prach_ConfigIndex = 10;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_prach_cfg(prach_cfg,cfg_db_ptr));

  prach_cfg.prach_ConfigInfo.prach_ConfigIndex = 63;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_prach_cfg(prach_cfg,cfg_db_ptr));

  prach_cfg.prach_ConfigInfo.prach_ConfigIndex = 30;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg(prach_cfg,cfg_db_ptr));

  prach_cfg.prach_ConfigInfo.prach_ConfigIndex = 46;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg(prach_cfg,cfg_db_ptr));

  prach_cfg.prach_ConfigInfo.prach_ConfigIndex = 60;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg(prach_cfg,cfg_db_ptr));

  prach_cfg.prach_ConfigInfo.prach_ConfigIndex = 61;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg(prach_cfg,cfg_db_ptr));

  prach_cfg.prach_ConfigInfo.prach_ConfigIndex = 62;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg(prach_cfg,cfg_db_ptr));
  

  cfg_db_ptr->ho_config_in_progress = LTE_RRC_LLC_HANDOVER_TDD_INTRA_FREQ;
  cfg_db_ptr->ho_target_cell_info.dl_cell_frequency = 36000;

  prach_cfg.prach_ConfigInfo.prach_ConfigIndex = 10;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_prach_cfg(prach_cfg,cfg_db_ptr));

  prach_cfg.prach_ConfigInfo.prach_ConfigIndex = 58;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg(prach_cfg,cfg_db_ptr));

  prach_cfg.prach_ConfigInfo.prach_ConfigIndex = 59;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg(prach_cfg,cfg_db_ptr));

  prach_cfg.prach_ConfigInfo.prach_ConfigIndex = 60;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg(prach_cfg,cfg_db_ptr));

  prach_cfg.prach_ConfigInfo.prach_ConfigIndex = 61;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg(prach_cfg,cfg_db_ptr));

  prach_cfg.prach_ConfigInfo.prach_ConfigIndex = 62;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg(prach_cfg,cfg_db_ptr));

  prach_cfg.prach_ConfigInfo.prach_ConfigIndex = 63;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg(prach_cfg,cfg_db_ptr));
  
  /* Validating PDSCH Common Config IE values */
  pdsch_cfg_common = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo.radioResourceConfigCommon.
    pdsch_ConfigCommon;

  pdsch_cfg_common.p_b = 1;
  pdsch_cfg_common.referenceSignalPower = 0;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_pdsch_cfg_common(pdsch_cfg_common,cfg_db_ptr));

  /* Check for invalid referenceSignalPower value */
  pdsch_cfg_common.referenceSignalPower = LTE_RRC_LLC_MAX_PDSCH_REG_SIG_PWR + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_pdsch_cfg_common(pdsch_cfg_common,cfg_db_ptr));
  /* ---------------------------------------------------------------------- */

  /* Validating PUCCH Common Config IE values */
  pucch_cfg_common = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo.radioResourceConfigCommon.
    pucch_ConfigCommon;

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_DELTA_SHIFT; i++)
  {
    pucch_cfg_common.deltaPUCCH_Shift = i;
    pucch_cfg_common.nCS_AN = ((uint8)(pucch_cfg_common.deltaPUCCH_Shift+1))*2;
    pucch_cfg_common.nRB_CQI = 0;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_pucch_cfg_common(pucch_cfg_common,cfg_db_ptr));
  }
  /* Maintain the order of the assignment statements/checks below, since it 
    directly determines the value being tested*/

  /* Check for invalid deltaPUCCH_Shift value */
  pucch_cfg_common.deltaPUCCH_Shift = i;
  pucch_cfg_common.nCS_AN = ((uint8)(pucch_cfg_common.deltaPUCCH_Shift+1))*2;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_pucch_cfg_common(pucch_cfg_common,cfg_db_ptr));

  /* check for (nCS_AN % delta_shift != 0) */
  i = 2;
  pucch_cfg_common.deltaPUCCH_Shift = i;
  pucch_cfg_common.nCS_AN = (uint8) i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_pucch_cfg_common(pucch_cfg_common,cfg_db_ptr));

  /* Check for invalid nRB_CQI value */
  pucch_cfg_common.nRB_CQI = 99;
  pucch_cfg_common.nCS_AN = (uint8) i;
  pucch_cfg_common.deltaPUCCH_Shift = 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_pucch_cfg_common(pucch_cfg_common,cfg_db_ptr));
  /* ---------------------------------------------------------------------- */

  /* Validating PUSCH Common Config IE values */
  pusch_cfg_common = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo.radioResourceConfigCommon.
    pusch_ConfigCommon;

  for (i = 0; i <= LTE_RRC_LLC_TEST_MAX_HOPPING_MODE; i++)
  {
    pusch_cfg_common.pusch_ConfigBasic.hoppingMode = i;
    pusch_cfg_common.pusch_ConfigBasic.n_SB = (uint8) i;
    pusch_cfg_common.ul_ReferenceSignalsPUSCH.groupAssignmentPUSCH = 0;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_pusch_cfg_common(pusch_cfg_common,cfg_db_ptr));
  }
  /* Maintain the order of the assignment statements/checks below, since it 
    directly determines the value being tested*/

  /* Check for invalid hoppingOffset value */
  i = 3;
  pusch_cfg_common.pusch_ConfigBasic.n_SB = (uint8) i;
  pusch_cfg_common.pusch_ConfigBasic.pusch_HoppingOffset = 99;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_pusch_cfg_common(pusch_cfg_common,cfg_db_ptr));

  /* Check for invalid hoppingMode value */
  pusch_cfg_common.pusch_ConfigBasic.hoppingMode = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_pusch_cfg_common(pusch_cfg_common,cfg_db_ptr));

  /* Check for invalid groupAssignmentPUSCH value */
  i = 0;
  pusch_cfg_common.pusch_ConfigBasic.n_SB = (uint8) i;
  pusch_cfg_common.pusch_ConfigBasic.pusch_HoppingOffset = 99;
  pusch_cfg_common.pusch_ConfigBasic.hoppingMode = i;
  pusch_cfg_common.ul_ReferenceSignalsPUSCH.
    groupAssignmentPUSCH = LTE_RRC_LLC_MAX_GROUP_ASSIGNMENT_PUSCH + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_pusch_cfg_common(pusch_cfg_common,cfg_db_ptr));
  /* ---------------------------------------------------------------------- */

  /* Validating all SRS Config Common IE values */
  srs_cfg_common = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo.radioResourceConfigCommon.
    soundingRS_UL_ConfigCommon;

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_SRS_SUBFRAME_CFG; i++)
  {
    if (i <= LTE_RRC_LLC_TEST_MAX_SRS_BW_CFG)
    {
      srs_cfg_common.u.setup->srs_BandwidthConfig = i;
    }
    srs_cfg_common.u.setup->srs_SubframeConfig = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_srs_cfg_common(srs_cfg_common,cfg_db_ptr,1));
  }
  /* Maintain the order of the assignment statements/checks below, since it 
    directly determines the value being tested*/

  /* Check for reserved srsSubframeConfiguration value */
  srs_cfg_common.u.setup->srs_SubframeConfig = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_srs_cfg_common(srs_cfg_common,cfg_db_ptr,1));

  /* Check for invalid srsBandwidthConfiguration value */
  srs_cfg_common.u.setup->srs_BandwidthConfig = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_srs_cfg_common(srs_cfg_common,cfg_db_ptr,1));

  /* Check for invalid srsSubframeConfiguration value */
  i++;
  srs_cfg_common.u.setup->srs_SubframeConfig = i;
  i = 0;
  srs_cfg_common.u.setup->srs_BandwidthConfig = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_srs_cfg_common(srs_cfg_common,cfg_db_ptr,1));
  
  /* ---------------------------------------------------------------------- */

  /* Validating all UL Power Ctrl Common IE values */
  ul_pwr_ctrl_common = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo.radioResourceConfigCommon.
    uplinkPowerControlCommon;

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_ALPHA; i++)
  {
    ul_pwr_ctrl_common.alpha = i;
    ul_pwr_ctrl_common.p0_NominalPUSCH = 0;
    
    if (i < LTE_RRC_LLC_TEST_DELTAF_PUCCH_FORMAT)
    {
      ul_pwr_ctrl_common.deltaFList_PUCCH.deltaF_PUCCH_Format1 = i;
      ul_pwr_ctrl_common.deltaFList_PUCCH.deltaF_PUCCH_Format2a = i;
      ul_pwr_ctrl_common.deltaFList_PUCCH.deltaF_PUCCH_Format2b = i;
    }

    if (i < LTE_RRC_LLC_TEST_DELTAF_PUCCH_FORMAT1B)
    {
      ul_pwr_ctrl_common.deltaFList_PUCCH.deltaF_PUCCH_Format1b = i;
    }

    if (i < LTE_RRC_LLC_TEST_DELTAF_PUCCH_FORMAT2)
    {
      ul_pwr_ctrl_common.deltaFList_PUCCH.deltaF_PUCCH_Format2 = i;
    }
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_power_ctrl_common(ul_pwr_ctrl_common,cfg_db_ptr));
  }

  /* Maintain the order of the assignment statements/checks below, since it 
    directly determines the value being tested*/
 
  /* Check for invalid alpha value */
  ul_pwr_ctrl_common.alpha = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_ul_power_ctrl_common(ul_pwr_ctrl_common,cfg_db_ptr));

  /* Check for invalid p0_NominalPUSCH value */
  i = 0;
  ul_pwr_ctrl_common.alpha = i;
  ul_pwr_ctrl_common.p0_NominalPUSCH = LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_ul_power_ctrl_common(ul_pwr_ctrl_common,cfg_db_ptr));

  /* Check for invalid deltaF_PUCCH_Format1 value */
  ul_pwr_ctrl_common.p0_NominalPUSCH = 0;
  ul_pwr_ctrl_common.deltaFList_PUCCH.
    deltaF_PUCCH_Format1 = LTE_RRC_LLC_TEST_DELTAF_PUCCH_FORMAT + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_ul_power_ctrl_common(ul_pwr_ctrl_common,cfg_db_ptr));

  /* Check for invalid deltaF_PUCCH_Format1b value */
  ul_pwr_ctrl_common.deltaFList_PUCCH.deltaF_PUCCH_Format1 = i;
  ul_pwr_ctrl_common.deltaFList_PUCCH.
    deltaF_PUCCH_Format1b = LTE_RRC_LLC_TEST_DELTAF_PUCCH_FORMAT1B + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_ul_power_ctrl_common(ul_pwr_ctrl_common,cfg_db_ptr));

  /* Check for invalid deltaF_PUCCH_Format2 value */
  ul_pwr_ctrl_common.deltaFList_PUCCH.deltaF_PUCCH_Format1b = i;
  ul_pwr_ctrl_common.deltaFList_PUCCH.
    deltaF_PUCCH_Format2 = LTE_RRC_LLC_TEST_DELTAF_PUCCH_FORMAT2 + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_ul_power_ctrl_common(ul_pwr_ctrl_common,cfg_db_ptr));

  /* Check for invalid deltaF_PUCCH_Format2a value */
  ul_pwr_ctrl_common.deltaFList_PUCCH.deltaF_PUCCH_Format2 = i;
  ul_pwr_ctrl_common.deltaFList_PUCCH.
    deltaF_PUCCH_Format2a = LTE_RRC_LLC_TEST_DELTAF_PUCCH_FORMAT + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_ul_power_ctrl_common(ul_pwr_ctrl_common,cfg_db_ptr));

  /* Check for invalid deltaF_PUCCH_Format2b value */
  ul_pwr_ctrl_common.deltaFList_PUCCH.deltaF_PUCCH_Format2a = i;
  ul_pwr_ctrl_common.deltaFList_PUCCH.
    deltaF_PUCCH_Format2b = LTE_RRC_LLC_TEST_DELTAF_PUCCH_FORMAT + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_ul_power_ctrl_common(ul_pwr_ctrl_common,cfg_db_ptr));
  /* ---------------------------------------------------------------------- */

  /* Validating all Antenna info Common IE values */
  antenna_info_common = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo.radioResourceConfigCommon.
    antennaInfoCommon;

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_ANTENNA_PORTS; i++)
  {
    antenna_info_common.antennaPortsCount = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_antenna_info_common(antenna_info_common,cfg_db_ptr));
  }
  /* Checking the spare value */
  antenna_info_common.antennaPortsCount = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_antenna_info_common(antenna_info_common,cfg_db_ptr));

  /* ---------------------------------------------------------------------- */

  /* Validating all PHICH IE values */
  phich_cfg = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo.radioResourceConfigCommon.
    phich_Config;

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_PHICH_RESOURCE; i++)
  {
    if (i <= LTE_RRC_LLC_TEST_MAX_PHICH_DURATION)
    {
      phich_cfg.phich_Duration = i;
    }
    phich_cfg.phich_Resource = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_phich_cfg(phich_cfg,&cfg_db_ptr->phich_cfg,TRUE));
  }

  /* Maintain the order of the assignment statements/checks below, since it 
    directly determines the value being tested*/

  /* Check for invalid phich_Resource value */
  phich_cfg.phich_Resource = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_phich_cfg(phich_cfg,&cfg_db_ptr->phich_cfg,TRUE));

  /* Check for invalid phich_Duration value */
  phich_cfg.phich_Duration = i;
  i = 0;
  phich_cfg.phich_Resource = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_phich_cfg(phich_cfg,&cfg_db_ptr->phich_cfg,TRUE));

  /* ---------------------------------------------------------------------- */

  /* Validating Target cell Info IE */

  mobility_ctrl_info_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo;

  /* Validating all DL and UL bandwidth values in Target cell Info IE */
  llc_ptr->dd_ptr->cmd_db.cell_info.cell_id.freq = mobility_ctrl_info_ptr->carrierFreq.dl_CarrierFreq;

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_BANDWIDTH; i++)
  {
    mobility_ctrl_info_ptr->carrierBandwidth.dl_Bandwidth = i;
    mobility_ctrl_info_ptr->carrierBandwidth.ul_Bandwidth = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_target_cell_info(mobility_ctrl_info_ptr,llc_ptr));
  }

  /* Verify spare values handling */
  /* Set UL bandwidth to a spare value */
  mobility_ctrl_info_ptr->carrierBandwidth.ul_Bandwidth = i;
  /* UL bandwidth should be set to DL bandwidth and lte_rrc_llc_update_target_cell_info
    should return E_SUCCESS*/
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_target_cell_info(mobility_ctrl_info_ptr,llc_ptr));
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->ul_freq_cfg.
            ul_bandwidth == llc_ptr->dd_ptr->oc_ptr->ho_target_cell_info.dl_bandwidth);

  /* Verifying handling when UL bandwidth is not signalled */
  /* Reset the UL bandwidth bitmask */
  mobility_ctrl_info_ptr->carrierBandwidth.m.ul_BandwidthPresent = 0;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_target_cell_info(mobility_ctrl_info_ptr,llc_ptr));
  /* UL bandwidth should be set to DL bandwidth and lte_rrc_llc_update_target_cell_info
    should return E_SUCCESS*/
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->ul_freq_cfg.
            ul_bandwidth == llc_ptr->dd_ptr->oc_ptr->ho_target_cell_info.dl_bandwidth);

  /* Now set DL bandwidth to a spare value, lte_rrc_llc_update_target_cell_info should
    return E_DATA_INVALID */
  mobility_ctrl_info_ptr->carrierBandwidth.dl_Bandwidth =i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_target_cell_info(mobility_ctrl_info_ptr,llc_ptr));

  /* Maintain the order of the assignment statements/checks below, since it 
    directly determines the value being tested*/

  /* Check for invalid targetPhysCellId value */
  mobility_ctrl_info_ptr->targetPhysCellId = LTE_RRC_LLC_MAX_VAL_PHY_CELL_ID + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_target_cell_info(mobility_ctrl_info_ptr,llc_ptr));

  /* Check for invalid DL earfcn value */
  mobility_ctrl_info_ptr->targetPhysCellId = LTE_RRC_LLC_MAX_VAL_PHY_CELL_ID;
  llc_ptr->dd_ptr->cmd_db.cell_info.cell_id.freq = LTE_RRC_LLC_TEST_MAX_DL_EARFCN + 1;
  mobility_ctrl_info_ptr->carrierFreq.dl_CarrierFreq = LTE_RRC_LLC_TEST_MAX_DL_EARFCN + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_target_cell_info(mobility_ctrl_info_ptr,llc_ptr));

  /* ---------------------------------------------------------------------- */
#ifdef FEATURE_LTE_TDD

  /* Validating all values in TDD IE */
  tdd_cfg = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo.radioResourceConfigCommon.
    tdd_Config;

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_SUBFRAME_PATTERN; i++)
  {
    if (i <= LTE_RRC_LLC_TEST_MAX_SUBFRAME_ASSIGN)
    {
      tdd_cfg.subframeAssignment = i;
    }
    tdd_cfg.specialSubframePatterns = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_tdd_cfg(tdd_cfg,cfg_db_ptr));
  }

  /* Check for invalid specialSubframePatterns value */
  tdd_cfg.specialSubframePatterns = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_tdd_cfg(tdd_cfg,cfg_db_ptr));

  /* Check for invalid subframeAssignment value */
  tdd_cfg.subframeAssignment = i;
  i = 0;
  tdd_cfg.specialSubframePatterns = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_tdd_cfg(tdd_cfg,cfg_db_ptr));
#endif
  /* ---------------------------------------------------------------------- */
 
  /* Validating all values in UL Cyclic Prefix */
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_UL_CYCLIC_PREFIX_LENGTH; i++)
  {
    mobility_ctrl_info_ptr->radioResourceConfigCommon.ul_CyclicPrefixLength = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_cyclic_prefix_length(
      mobility_ctrl_info_ptr->radioResourceConfigCommon.ul_CyclicPrefixLength,cfg_db_ptr));
  }

  /* Check for invalid ul_CyclicPrefixLength value */
  mobility_ctrl_info_ptr->radioResourceConfigCommon.ul_CyclicPrefixLength = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_ul_cyclic_prefix_length(
    mobility_ctrl_info_ptr->radioResourceConfigCommon.ul_CyclicPrefixLength,cfg_db_ptr));

  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_ptr, lte_rrc_osys_BCCH_DL_SCH_Message_PDU);

  /* ---------------------------------------------------------------------- */
  //                     Verify common IEs in SIB2
 /* ---------------------------------------------------------------------- */

  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_sib2_with_mbsfn);
  encoded_pdu.value = llc_sib2_with_mbsfn;
  bcch_dl_sch_msg_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               BCCH_DL_SCH_Message,
                               &decode_status);
  TF_ASSERT(bcch_dl_sch_msg_ptr != NULL);

  /* ---------------------------------------------------------------------- */

  /*Validating PRACH config index*/
  prach_cfg_sib = bcch_dl_sch_msg_ptr->message.u.c1->u.systemInformation->criticalExtensions.
    u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib2->radioResourceConfigCommon.
    prach_Config;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;

  prach_cfg_sib.prach_ConfigInfo.prach_ConfigIndex = 10;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_prach_cfg_sib(prach_cfg_sib,cfg_db_ptr));

  prach_cfg_sib.prach_ConfigInfo.prach_ConfigIndex = 63;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_prach_cfg_sib(prach_cfg_sib,cfg_db_ptr));

  prach_cfg_sib.prach_ConfigInfo.prach_ConfigIndex = 30;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg_sib(prach_cfg_sib,cfg_db_ptr));

  prach_cfg_sib.prach_ConfigInfo.prach_ConfigIndex = 46;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg_sib(prach_cfg_sib,cfg_db_ptr));

  prach_cfg_sib.prach_ConfigInfo.prach_ConfigIndex = 60;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg_sib(prach_cfg_sib,cfg_db_ptr));

  prach_cfg_sib.prach_ConfigInfo.prach_ConfigIndex = 61;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg_sib(prach_cfg_sib,cfg_db_ptr));

  prach_cfg_sib.prach_ConfigInfo.prach_ConfigIndex = 62;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg_sib(prach_cfg_sib,cfg_db_ptr));

  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;

  prach_cfg_sib.prach_ConfigInfo.prach_ConfigIndex = 10;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_prach_cfg_sib(prach_cfg_sib,cfg_db_ptr));

  prach_cfg_sib.prach_ConfigInfo.prach_ConfigIndex = 58;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg_sib(prach_cfg_sib,cfg_db_ptr));

  prach_cfg_sib.prach_ConfigInfo.prach_ConfigIndex = 59;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg_sib(prach_cfg_sib,cfg_db_ptr));

  prach_cfg_sib.prach_ConfigInfo.prach_ConfigIndex = 60;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg_sib(prach_cfg_sib,cfg_db_ptr));

  prach_cfg_sib.prach_ConfigInfo.prach_ConfigIndex = 61;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg_sib(prach_cfg_sib,cfg_db_ptr));

  prach_cfg_sib.prach_ConfigInfo.prach_ConfigIndex = 62;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg_sib(prach_cfg_sib,cfg_db_ptr));

  prach_cfg_sib.prach_ConfigInfo.prach_ConfigIndex = 63;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_prach_cfg_sib(prach_cfg_sib,cfg_db_ptr));

  /* Validating all UE timers and constants IE values */
  tmr_and_const_param = bcch_dl_sch_msg_ptr->message.u.c1->u.systemInformation->criticalExtensions.
    u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib2->ue_TimersAndConstants;
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_N310; i++)
  {
    tmr_and_const_param.n310 = i;
    tmr_and_const_param.n311 = i;
    if (i <= LTE_RRC_LLC_TEST_MAX_T310)
    {
      tmr_and_const_param.t310 = i;
    }
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_tmr_and_const_param(tmr_and_const_param,cfg_db_ptr));
  }

  /* Check for invalid t310 value */
  tmr_and_const_param.t310 = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_tmr_and_const_param(tmr_and_const_param,cfg_db_ptr));

  /* Check for invalid n310 value */
  tmr_and_const_param.n310 = i;
  i = 0;
  tmr_and_const_param.t310 = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_tmr_and_const_param(tmr_and_const_param,cfg_db_ptr));

  /* Check for invalid n311 value */
  tmr_and_const_param.n310 = i;
  tmr_and_const_param.n311 = LTE_RRC_LLC_TEST_MAX_N310 + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_tmr_and_const_param(tmr_and_const_param,cfg_db_ptr));
  
  /* ---------------------------------------------------------------------- */

  /* Validating all MBSFN IE values */
  mbsfg_cfg_ptr = &bcch_dl_sch_msg_ptr->message.u.c1->u.systemInformation->criticalExtensions.
    u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib2->mbsfn_SubframeConfigList;

  mbsfg_cfg_ptr->n = 1;
  mbsfg_cfg_ptr->elem[0].subframeAllocation.t = 
    T_lte_rrc_osys_MBSFN_SubframeConfig_subframeAllocation_oneFrame;
  mbsfg_cfg_ptr->elem[0].subframeAllocation.u.oneFrame->numbits = 
    LTE_RRC_LLC_OSS_BITSTRING_SIZE_MBSFN_ONE_FRAME;
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_RADIO_FRAME_ALLOCATION_PERIOD; i++)
  {
    mbsfg_cfg_ptr->elem[0].radioframeAllocationPeriod = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_mbsfn_cfg(mbsfg_cfg_ptr,cfg_db_ptr,1));
    TF_ASSERT(cfg_db_ptr->mbsfn_cfg.enable == TRUE);
  }

  /* Check for invalid radioframeAllocationPeriod value */
  mbsfg_cfg_ptr->elem[0].radioframeAllocationPeriod = i;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_mbsfn_cfg(mbsfg_cfg_ptr,cfg_db_ptr,1));
  TF_ASSERT(cfg_db_ptr->mbsfn_cfg.enable == FALSE);

  /* Check for invalid subframeAllocation length */
  i = 0;
  mbsfg_cfg_ptr->elem[0].radioframeAllocationPeriod = i;
  mbsfg_cfg_ptr->elem[0].subframeAllocation.u.oneFrame->numbits
     = LTE_RRC_LLC_OSS_BITSTRING_SIZE_MBSFN_FOUR_FRAME;
  TF_ASSERT(E_SUCCESS== lte_rrc_llc_update_mbsfn_cfg(mbsfg_cfg_ptr,cfg_db_ptr,1));
  TF_ASSERT(cfg_db_ptr->mbsfn_cfg.enable == FALSE);

  /* Check handling for subframeAllocation = fourFrames and invalid RadioFrameAllocationPeriod */
  mbsfg_cfg_ptr->elem[0].subframeAllocation.t = 
    T_lte_rrc_osys_MBSFN_SubframeConfig_subframeAllocation_fourFrames;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_mbsfn_cfg(mbsfg_cfg_ptr,cfg_db_ptr,1));
  TF_ASSERT(cfg_db_ptr->mbsfn_cfg.enable == FALSE);

  /* Check handling for subframeAllocation = fourFrames and valid RadioFrameAllocationPeriod */
  mbsfg_cfg_ptr->elem[0].radioframeAllocationPeriod = 2;
  mbsfg_cfg_ptr->elem[0].subframeAllocation.t = 
    T_lte_rrc_osys_MBSFN_SubframeConfig_subframeAllocation_fourFrames;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_mbsfn_cfg(mbsfg_cfg_ptr,cfg_db_ptr,1));
  TF_ASSERT(cfg_db_ptr->mbsfn_cfg.enable == TRUE);

  /* Check for invalid subframeAllocation length */
  mbsfg_cfg_ptr->elem[0].subframeAllocation.u.oneFrame->
    numbits = LTE_RRC_LLC_OSS_BITSTRING_SIZE_MBSFN_ONE_FRAME;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_mbsfn_cfg(mbsfg_cfg_ptr,cfg_db_ptr,1));
  TF_ASSERT(cfg_db_ptr->mbsfn_cfg.enable == FALSE);

  lte_rrc_osys_asn1_free_pdu(bcch_dl_sch_msg_ptr, lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  
  TF_MSG("\nLLC: PHY Common Config IE Enum values test Ends \n");
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_utf_reset(); 
  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcPhyCommonIEEnumValuesTest::Teardown()
{
  lte_rrc_utf_teardown();
}








