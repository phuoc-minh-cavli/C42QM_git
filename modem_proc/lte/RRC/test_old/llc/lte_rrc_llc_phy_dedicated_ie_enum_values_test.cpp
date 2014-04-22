/*!
  @file lte_rrc_llc_phy_dedicated_ie_enum_values_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of all PHY 
  dedicated config IEs with enum values, for all possible values and other range 
  value checks.

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
07/16/09   mm     Initial Revision
           
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
#include "lte_l1_types.h"
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
  TF_PROTO_STUB(void *, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
  TF_PROTO_STUB(uint8, lte_rrc_csp_get_num_tx_antennas, void);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
  TF_PROTO_STUB(lte_rrc_div_duplex_e, lte_rrc_csp_get_last_camped_mode, void);
  TF_PROTO_STUB(uint8, lte_rrc_cap_get_fgi_bit, uint8);	
  TF_PROTO_STUB(boolean, lte_rrc_cell_is_tdd, lte_earfcn_t);
}

/*===========================================================================

  FUNCTION:  lte_rrc_test_efs_item_write_CA

===========================================================================*/
/*!
    @brief
    This writes to lte_rrc_release efs item to set the release to LTE_RRC_REL_2_0.
  
    @return
    void.

*/
/*=========================================================================*/
void lte_rrc_test_efs_item_write_CA
(
  void
)
{
  int oflag, mode;

  lte_rrc_release_e rel = LTE_RRC_REL_2_0;
  uint8 disable_ca_bandwidth_check = 1;

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/cap/disable_ca_bandwidth_combo", 
                    &disable_ca_bandwidth_check, sizeof(uint8), oflag, mode) >= 0);
  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/cap/disable_ca_intra_band_validation", 
                    &disable_ca_bandwidth_check, sizeof(uint8), oflag, mode) >= 0);
	  
} /*lte_rrc_test_efs_item_write*/


/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPhyDedicatedIEEnumValuesTest);

void LteRrcLlcPhyDedicatedIEEnumValuesTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL,0);
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  lte_rrc_llc_test_stub_init();
  /* Set the release version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;
}

void LteRrcLlcPhyDedicatedIEEnumValuesTest::Test()
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
  lte_rrc_osys_CQI_ReportConfig_r10 *cqi_rep_ptr_r10 = NULL;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: PHY Dedicated Config IE Enum values test begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg);
  encoded_pdu.value = llc_reconfig_ho_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* Validating all Antenna info Dedicated IE values */
  phy_ded_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    physicalConfigDedicated;

  /* ---------------------------------------------------------------------- */

  /* Validating PDSCH Dedicated Config IE values */
  pdsch_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    physicalConfigDedicated.pdsch_ConfigDedicated;

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_P_A; i++)
  {
    pdsch_cfg_ptr->p_a = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_pdsch_cfg_dedicated(cfg_db_ptr,pdsch_cfg_ptr));
  }

  /* Check for invalid p_a value */
  pdsch_cfg_ptr->p_a = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_pdsch_cfg_dedicated(cfg_db_ptr,pdsch_cfg_ptr));

  /* ---------------------------------------------------------------------- */

  /* Validating PUCCH Dedicated Config IE values */
  pucch_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    physicalConfigDedicated.pucch_ConfigDedicated;

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_DELTA_SHIFT; i++)
  {
    pucch_cfg_ptr->ackNackRepetition.u.setup->repetitionFactor = i;
    pucch_cfg_ptr->tdd_AckNackFeedbackMode 
      = lte_rrc_osys_PUCCH_ConfigDedicated_tdd_AckNackFeedbackMode_multiplexing;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_pucch_cfg_dedicated(cfg_db_ptr,phy_ded_cfg_ptr,1));
  }

  /* Check for the spare value handling */
  pucch_cfg_ptr->ackNackRepetition.u.setup->repetitionFactor = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_pucch_cfg_dedicated(cfg_db_ptr,phy_ded_cfg_ptr,1));

  /* ---------------------------------------------------------------------- */

  /* PUSCH Dedicated Config IE doesn't have any enum values */

  /* ---------------------------------------------------------------------- */

  /* Validating all SRS Config Dedicated IE values */
  srs_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    physicalConfigDedicated.soundingRS_UL_ConfigDedicated;
  cfg_db_ptr->srs_cfg_common.enable = TRUE;

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_SRS_CYCLIC_SHIFT; i++)
  {
    if (i <= LTE_RRC_LLC_TEST_MAX_SRS_BANDWIDTH)
    {
      srs_cfg_ptr->u.setup->srs_Bandwidth = i;
    }
    if (i <= LTE_RRC_LLC_TEST_MAX_SRS_HOPPING_BANDWIDTH)
    {
      srs_cfg_ptr->u.setup->srs_HoppingBandwidth = i;
    }
    srs_cfg_ptr->u.setup->cyclicShift = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_srs_cfg_dedicated(cfg_db_ptr,srs_cfg_ptr,1));
  }

  /* Maintain the order of the assignment statements/checks below, since it 
    directly determines the value being tested*/

  /* Check for SRS Config Disable choice */
  srs_cfg_ptr->t = T_lte_rrc_osys_SoundingRS_UL_ConfigDedicated_release;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_srs_cfg_dedicated(cfg_db_ptr,srs_cfg_ptr,1));

  /* Check for invalid srsBandwidth value */
  srs_cfg_ptr->t = T_lte_rrc_osys_SoundingRS_UL_ConfigDedicated_setup;
  srs_cfg_ptr->u.setup->srs_Bandwidth = (LTE_RRC_LLC_TEST_MAX_SRS_BANDWIDTH + 1);
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_srs_cfg_dedicated(cfg_db_ptr,srs_cfg_ptr,1));

  /* Check for invalid srsHoppingBandwidth value */
  srs_cfg_ptr->u.setup->srs_Bandwidth =  LTE_RRC_LLC_TEST_MAX_SRS_BANDWIDTH;
  srs_cfg_ptr->u.setup->srs_HoppingBandwidth =  (LTE_RRC_LLC_TEST_MAX_SRS_HOPPING_BANDWIDTH + 1);
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_srs_cfg_dedicated(cfg_db_ptr,srs_cfg_ptr,1));

  /* Check for invalid frequencyDomainPosition value */
  srs_cfg_ptr->u.setup->freqDomainPosition = LTE_RRC_LLC_MAX_FREQ_DOMAIN_POS + 1;
  srs_cfg_ptr->u.setup->srs_HoppingBandwidth = LTE_RRC_LLC_TEST_MAX_SRS_HOPPING_BANDWIDTH;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_srs_cfg_dedicated(cfg_db_ptr,srs_cfg_ptr,1));

  /* Check for invalid SRS cyclicShift value */
  srs_cfg_ptr->u.setup->cyclicShift = i;
  i = 0;
  srs_cfg_ptr->u.setup->freqDomainPosition = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_srs_cfg_dedicated(cfg_db_ptr,srs_cfg_ptr,1));

  /* Check for reserved SRS config index value for FDD */
  srs_cfg_ptr->u.setup->cyclicShift = i;
  srs_cfg_ptr->u.setup->srs_ConfigIndex = LTE_RRC_LLC_FDD_MAX_SRS_CFG_INDEX + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_srs_cfg_dedicated(cfg_db_ptr,srs_cfg_ptr,1));

  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;
  #ifdef FEATURE_LTE_TDD
  /* Check for reserved SRS config index value for TDD */
  srs_cfg_ptr->u.setup->cyclicShift = i;
  srs_cfg_ptr->u.setup->srs_ConfigIndex = LTE_RRC_LLC_TDD_MAX_SRS_CFG_INDEX + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_srs_cfg_dedicated(cfg_db_ptr,srs_cfg_ptr,1));
  #endif
  /* ---------------------------------------------------------------------- */

  /* Validating all UL Power Ctrl Dedicated IE values */
  ul_pwr_ctrl_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    physicalConfigDedicated.uplinkPowerControlDedicated;
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_FILTER_COEFFICIENT; i++)
  {
    ul_pwr_ctrl_ptr->filterCoefficient = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_ul_power_ctrl_dedicated(cfg_db_ptr,ul_pwr_ctrl_ptr));
  }
  /* Checking the spare value */
  ul_pwr_ctrl_ptr->filterCoefficient = i;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_ul_power_ctrl_dedicated(cfg_db_ptr,ul_pwr_ctrl_ptr));
  /* filter coefficient should be set to a default value */
  TF_ASSERT(cfg_db_ptr->ul_power_ctrl_dedicated.
            filter_coefficient == LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_4);

  /* ---------------------------------------------------------------------- */
  /* Validating all tpc_PDCCH_ConfigPUCCH IE values */
  tpc_pdcch_cfg_pucch_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    physicalConfigDedicated.tpc_PDCCH_ConfigPUCCH;

  tpc_pdcch_cfg_pucch_ptr->u.setup->tpc_Index.u.indexOfFormat3 = LTE_RRC_LLC_MIN_TPC_INDEX - 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_tpc_pdcch_cfg_pucch(cfg_db_ptr,tpc_pdcch_cfg_pucch_ptr));
  tpc_pdcch_cfg_pucch_ptr->u.setup->tpc_Index.u.indexOfFormat3 = LTE_RRC_LLC_MAX_TPC_INDEX_FORMAT3 + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_tpc_pdcch_cfg_pucch(cfg_db_ptr,tpc_pdcch_cfg_pucch_ptr));
  tpc_pdcch_cfg_pucch_ptr->u.setup->tpc_Index.u.indexOfFormat3 = LTE_RRC_LLC_MIN_TPC_INDEX;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_tpc_pdcch_cfg_pucch(cfg_db_ptr,tpc_pdcch_cfg_pucch_ptr));

  tpc_pdcch_cfg_pucch_ptr->u.setup->tpc_Index.u.indexOfFormat3A = LTE_RRC_LLC_MIN_TPC_INDEX - 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_tpc_pdcch_cfg_pucch(cfg_db_ptr,tpc_pdcch_cfg_pucch_ptr));
  tpc_pdcch_cfg_pucch_ptr->u.setup->tpc_Index.u.indexOfFormat3A = LTE_RRC_LLC_MAX_TPC_INDEX_FORMAT3A + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_tpc_pdcch_cfg_pucch(cfg_db_ptr,tpc_pdcch_cfg_pucch_ptr));
  tpc_pdcch_cfg_pucch_ptr->u.setup->tpc_Index.u.indexOfFormat3A = LTE_RRC_LLC_MIN_TPC_INDEX;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_tpc_pdcch_cfg_pucch(cfg_db_ptr,tpc_pdcch_cfg_pucch_ptr));

  /* ---------------------------------------------------------------------- */

  /* Validating all tpc_PDCCH_ConfigPUSCH IE values */
  tpc_pdcch_cfg_pusch_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    physicalConfigDedicated.tpc_PDCCH_ConfigPUSCH;

  tpc_pdcch_cfg_pusch_ptr->u.setup->tpc_Index.u.indexOfFormat3 = LTE_RRC_LLC_MIN_TPC_INDEX - 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_tpc_pdcch_cfg_pusch(cfg_db_ptr,tpc_pdcch_cfg_pusch_ptr));
  tpc_pdcch_cfg_pusch_ptr->u.setup->tpc_Index.u.indexOfFormat3 = LTE_RRC_LLC_MAX_TPC_INDEX_FORMAT3 + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_tpc_pdcch_cfg_pusch(cfg_db_ptr,tpc_pdcch_cfg_pusch_ptr));
  tpc_pdcch_cfg_pusch_ptr->u.setup->tpc_Index.u.indexOfFormat3 = LTE_RRC_LLC_MIN_TPC_INDEX;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_tpc_pdcch_cfg_pusch(cfg_db_ptr,tpc_pdcch_cfg_pusch_ptr));

  tpc_pdcch_cfg_pusch_ptr->u.setup->tpc_Index.u.indexOfFormat3A = LTE_RRC_LLC_MIN_TPC_INDEX - 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_tpc_pdcch_cfg_pusch(cfg_db_ptr,tpc_pdcch_cfg_pusch_ptr));
  tpc_pdcch_cfg_pusch_ptr->u.setup->tpc_Index.u.indexOfFormat3A = LTE_RRC_LLC_MAX_TPC_INDEX_FORMAT3A + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_tpc_pdcch_cfg_pusch(cfg_db_ptr,tpc_pdcch_cfg_pusch_ptr));
  tpc_pdcch_cfg_pusch_ptr->u.setup->tpc_Index.u.indexOfFormat3A = LTE_RRC_LLC_MIN_TPC_INDEX;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_tpc_pdcch_cfg_pusch(cfg_db_ptr,tpc_pdcch_cfg_pusch_ptr));

  /* ---------------------------------------------------------------------- */

  /* Validating all CQI Reporting Config IE values */
  cqi_rep_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    physicalConfigDedicated.cqi_ReportConfig;
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_CQI_REPORTING_MODE_APERIODIC; i++)
  {
    cqi_rep_ptr->cqi_ReportModeAperiodic = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_cqi_reporting(cfg_db_ptr,cqi_rep_ptr,1, FALSE));
  }
  /* Checking the spare value */
  cqi_rep_ptr->cqi_ReportModeAperiodic = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_cqi_reporting(cfg_db_ptr,cqi_rep_ptr,1, FALSE));

  /* Check for CQI reporting disable handling */
  cqi_rep_ptr->cqi_ReportPeriodic.t = T_lte_rrc_osys_CQI_ReportPeriodic_release;
  i = 0;
  cqi_rep_ptr->cqi_ReportModeAperiodic = i;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_cqi_reporting(cfg_db_ptr,cqi_rep_ptr,1, FALSE));

  /* Check for invalid CQI_PUCCH_ResourceIndex value */
  cqi_rep_ptr->cqi_ReportPeriodic.t = T_lte_rrc_osys_CQI_ReportPeriodic_setup;
  cqi_rep_ptr->cqi_ReportPeriodic.u.setup->
    cqi_PUCCH_ResourceIndex = LTE_RRC_LLC_MAX_CQI_PUCCH_RESOURCE_INDEX + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_cqi_reporting(cfg_db_ptr,cqi_rep_ptr,1, FALSE));

  /* Check for invalid k value for subbandCQI */
  cqi_rep_ptr->cqi_ReportPeriodic.u.setup->
    cqi_PUCCH_ResourceIndex = LTE_RRC_LLC_MAX_CQI_PUCCH_RESOURCE_INDEX;
  cqi_rep_ptr->cqi_ReportPeriodic.u.setup->cqi_FormatIndicatorPeriodic.u.
    subbandCQI->k = LTE_RRC_LLC_MAX_CQI_SUBBAND_K + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_cqi_reporting(cfg_db_ptr,cqi_rep_ptr,1, FALSE));
                                            
  /* Check for wideband formatIndicator handling */
  cqi_rep_ptr->cqi_ReportPeriodic.u.setup->cqi_FormatIndicatorPeriodic.
    t = T_lte_rrc_osys_CQI_ReportPeriodic_setup_cqi_FormatIndicatorPeriodic_widebandCQI;
  cqi_rep_ptr->cqi_ReportPeriodic.u.setup->cqi_FormatIndicatorPeriodic.u.
    subbandCQI->k = LTE_RRC_LLC_MAX_CQI_SUBBAND_K;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_cqi_reporting(cfg_db_ptr,cqi_rep_ptr,1, FALSE));
#ifdef FEATURE_LTE_TDD
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;

  cqi_rep_ptr->cqi_ReportPeriodic.u.setup->cqi_pmi_ConfigIndex=LTE_RRC_LLC_MAX_CQI_PMI_CFG_INDEX_TDD+1;

  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_cqi_reporting(cfg_db_ptr,cqi_rep_ptr,1, FALSE));
#endif
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_cqi_reporting(cfg_db_ptr,cqi_rep_ptr,1, FALSE));

  /* Check for reserved CQI_PMI_ConfigIndex value */
  cqi_rep_ptr->cqi_ReportPeriodic.u.setup->cqi_pmi_ConfigIndex = LTE_RRC_LLC_RESERVED_CQI_PMI_CFG_INDEX;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_cqi_reporting(cfg_db_ptr,cqi_rep_ptr,1, FALSE));

  cqi_rep_ptr->cqi_ReportPeriodic.u.setup->cqi_pmi_ConfigIndex = LTE_RRC_LLC_MAX_CQI_PMI_CFG_INDEX + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_cqi_reporting(cfg_db_ptr,cqi_rep_ptr,1, FALSE));

  /* Check for reserved RI_ConfigIndex value */
  cqi_rep_ptr->cqi_ReportPeriodic.u.setup->cqi_pmi_ConfigIndex = 0;
  cqi_rep_ptr->cqi_ReportPeriodic.u.setup->ri_ConfigIndex = LTE_RRC_LLC_MAX_RI_CFG_INDEX + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_cqi_reporting(cfg_db_ptr,cqi_rep_ptr,1, FALSE));
  
  /* ---------------------------------------------------------------------- */

  /* Validating all CQI Reporting Config Rel10 CQI PMI CFG IDX*/
  cqi_rep_ptr_r10 = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    physicalConfigDedicated.cqi_ReportConfig_r10;
#ifdef FEATURE_LTE_TDD
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;

  cqi_rep_ptr_r10->cqi_ReportPeriodic_r10.u.setup->cqi_pmi_ConfigIndex = LTE_RRC_LLC_MAX_CQI_PMI_CFG_INDEX_TDD+1;

  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_cqi_reporting_rel10(llc_ptr, &cfg_db_ptr->cqi_cfg,cqi_rep_ptr_r10));

  cqi_rep_ptr_r10->cqi_ReportPeriodic_r10.u.setup->cqi_pmi_ConfigIndex = LTE_RRC_LLC_MAX_CQI_PMI_CFG_INDEX_TDD;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_cqi_reporting_rel10(llc_ptr, &cfg_db_ptr->cqi_cfg,cqi_rep_ptr_r10));
#endif

  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;

  cqi_rep_ptr_r10->cqi_ReportPeriodic_r10.u.setup->cqi_pmi_ConfigIndex = LTE_RRC_LLC_RESERVED_CQI_PMI_CFG_INDEX;

  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_cqi_reporting_rel10(llc_ptr, &cfg_db_ptr->cqi_cfg,cqi_rep_ptr_r10));

  cqi_rep_ptr_r10->cqi_ReportPeriodic_r10.u.setup->cqi_pmi_ConfigIndex = LTE_RRC_LLC_RESERVED_CQI_PMI_CFG_INDEX-1;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_cqi_reporting_rel10(llc_ptr, &cfg_db_ptr->cqi_cfg,cqi_rep_ptr_r10));
  

  /* Validating all SR Config IE values */
  sr_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    physicalConfigDedicated.schedulingRequestConfig;
  for (i = 0;i <= LTE_RRC_LLC_TEST_DSR_TRANS_MAX; i++)
  {
    sr_cfg_ptr->u.setup->dsr_TransMax = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sr_cfg(cfg_db_ptr,sr_cfg_ptr,1));
  }
  /* Checking the spare value */
  sr_cfg_ptr->u.setup->dsr_TransMax = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sr_cfg(cfg_db_ptr,sr_cfg_ptr,1));

  /* Check for SR Config disable handling */
  sr_cfg_ptr->t = T_lte_rrc_osys_SchedulingRequestConfig_release;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_sr_cfg(cfg_db_ptr,sr_cfg_ptr,1));

  /* Check for invalid sr_ConfigurationIndex value */
  sr_cfg_ptr->t = T_lte_rrc_osys_SchedulingRequestConfig_setup;
  sr_cfg_ptr->u.setup->sr_ConfigIndex = LTE_RRC_LLC_MAX_SR_CONFIG_INDEX + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_sr_cfg(cfg_db_ptr,sr_cfg_ptr,1));
#ifdef FEATURE_LTE_SPS
  /* ---------------------------------------------------------------------- */

  /* Validating all SPS Config IE values */
  sps_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                 criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
                 radioResourceConfigDedicated.sps_Config;
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
#endif
  /* ---------------------------------------------------------------------- */

  phy_ded_cfg_ptr->antennaInfo.t = T_lte_rrc_osys_PhysicalConfigDedicated_antennaInfo_explicitValue;

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_TRANSMISSION_MODE; i++)
  {
    phy_ded_cfg_ptr->antennaInfo.u.explicitValue = 
      (lte_rrc_osys_AntennaInfoDedicated *) lte_rrc_asn1_osys_IE_malloc(
        sizeof(lte_rrc_osys_AntennaInfoDedicated));
    phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = i;
    if ((i > 1) && (i < 6))
    {
      phy_ded_cfg_ptr->antennaInfo.u.explicitValue->m.codebookSubsetRestrictionPresent = 1;
	  TF_STUB(lte_rrc_csp_get_num_tx_antennas) = lte_rrc_llc_test_csp_get_num_tx_antennas_mimo;
    }
    else
    {
      phy_ded_cfg_ptr->antennaInfo.u.explicitValue->m.codebookSubsetRestrictionPresent = 0;
    }

    switch(i)
    {
      case 0:
        TF_STUB(lte_rrc_csp_get_num_tx_antennas) = lte_rrc_llc_test_csp_get_num_tx_antennas_mimo;
        /* TM1 is an invalid transmissionMode for MIMO */
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));
        break;
      case 1:
        TF_STUB(lte_rrc_csp_get_num_tx_antennas) = lte_rrc_llc_test_csp_get_num_tx_antennas_simo;
        /* TM2 is an invalid transmissionMode for SIMO */
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));
        break;
      case 2:
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          t = T_lte_rrc_osys_AntennaInfoDedicated_codebookSubsetRestriction_n2TxAntenna_tm3;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n2TxAntenna_tm3 = (ASN1BitStr32 *) lte_rrc_asn1_osys_IE_malloc(sizeof(ASN1BitStr32));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n2TxAntenna_tm3->numbits = 2;
        osys_bitstring_ptr = phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
         u.n2TxAntenna_tm3;
        *(osys_bitstring_ptr->data) = 1;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_antenna_info(cfg_db_ptr,
                                                                phy_ded_cfg_ptr));

        /* Check for invalid codebookSubsetRestriction for TM3 */
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = (i + 1);
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));

        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = i;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n2TxAntenna_tm3->numbits = 3;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));
        
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.t = 2;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n4TxAntenna_tm3 = (ASN1BitStr32 *) lte_rrc_asn1_osys_IE_malloc(sizeof(ASN1BitStr32));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n4TxAntenna_tm3->numbits = 4;
        osys_bitstring_ptr = phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
         u.n4TxAntenna_tm3;
        *(osys_bitstring_ptr->data) = 1;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                phy_ded_cfg_ptr));

        /* Check for invalid codebookSubsetRestriction for TM3 */
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = (i + 1);
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));

        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = i;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n4TxAntenna_tm3->numbits = 5;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr)); 
        
        /* Check for absence of codebookSubsetRestriction when required */
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->m.codebookSubsetRestrictionPresent = 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->m.codebookSubsetRestrictionPresent = 0;
	 	
		/* Check for invalid TM for SIMO config */
        TF_STUB(lte_rrc_csp_get_num_tx_antennas) = lte_rrc_llc_test_csp_get_num_tx_antennas_simo;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));
        break;
      case 3:
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.t = 3;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n2TxAntenna_tm4 = (ASN1BitStr32 *) lte_rrc_asn1_osys_IE_malloc(sizeof(ASN1BitStr32));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n2TxAntenna_tm4->numbits = 6;
        osys_bitstring_ptr = phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
         u.n2TxAntenna_tm4;
        *(osys_bitstring_ptr->data) = 1;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                phy_ded_cfg_ptr));
        /* Check for invalid codebookSubsetRestriction for TM4 */
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = (i + 1);
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = i;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n2TxAntenna_tm4->numbits = 5;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));

        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.t = 4;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n4TxAntenna_tm4 = 
          (lte_rrc_osys_AntennaInfoDedicated_codebookSubsetRestriction_n4TxAntenna_tm4 *) 
        lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_AntennaInfoDedicated_codebookSubsetRestriction_n4TxAntenna_tm4));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n4TxAntenna_tm4->numbits = 64;
        cbsr_n4_tm4_ptr = phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
         u.n4TxAntenna_tm4;
        *(cbsr_n4_tm4_ptr->data) = 1;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                phy_ded_cfg_ptr));
        /* Check for invalid codebookSubsetRestriction for TM4 */
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = (i + 1);
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = i;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n4TxAntenna_tm4->numbits = 60;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));       
        /* Check for invalid TM for SIMO config */
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->
          codebookSubsetRestriction.u.n4TxAntenna_tm4->numbits = 64;
        TF_STUB(lte_rrc_csp_get_num_tx_antennas) = lte_rrc_llc_test_csp_get_num_tx_antennas_simo;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));
        break;
      case 4:
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.t = 5;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n2TxAntenna_tm5 = (ASN1BitStr32 *) lte_rrc_asn1_osys_IE_malloc(sizeof(ASN1BitStr32));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n2TxAntenna_tm5->numbits = 4;
        osys_bitstring_ptr = phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
         u.n2TxAntenna_tm5;
        *(osys_bitstring_ptr->data) = 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                phy_ded_cfg_ptr));
        /* Check for invalid codebookSubsetRestriction for TM5 */
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = (i + 1);
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = i;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n2TxAntenna_tm5->numbits = 2;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));       

        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.t = 6;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n4TxAntenna_tm5 = (ASN1BitStr32 *) lte_rrc_asn1_osys_IE_malloc(sizeof(ASN1BitStr32));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n4TxAntenna_tm5->numbits = 16;
        osys_bitstring_ptr = phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
         u.n4TxAntenna_tm5;
        *(osys_bitstring_ptr->data) = 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                phy_ded_cfg_ptr));
        /* Check for invalid codebookSubsetRestriction for TM5 */
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = (i + 1);
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = i;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n4TxAntenna_tm5->numbits = 8;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));  
        /* Check for invalid TM for SIMO config */
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->
          codebookSubsetRestriction.u.n4TxAntenna_tm5->numbits = 16;
        TF_STUB(lte_rrc_csp_get_num_tx_antennas) = lte_rrc_llc_test_csp_get_num_tx_antennas_simo;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));
        break;
      case 5:
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.t = 7;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n2TxAntenna_tm6 = (ASN1BitStr32 *) lte_rrc_asn1_osys_IE_malloc(sizeof(ASN1BitStr32));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n2TxAntenna_tm6->numbits = 4;
        osys_bitstring_ptr = phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
         u.n2TxAntenna_tm6;
        *(osys_bitstring_ptr->data) = 1;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                phy_ded_cfg_ptr));
        /* Check for invalid codebookSubsetRestriction for TM6 */
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = (i - 1);
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = i;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n2TxAntenna_tm6->numbits = 2;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));       

        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.t = 8;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n4TxAntenna_tm6 = (ASN1BitStr32 *) lte_rrc_asn1_osys_IE_malloc(sizeof(ASN1BitStr32));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
          u.n4TxAntenna_tm6->numbits = 16;
        osys_bitstring_ptr = phy_ded_cfg_ptr->antennaInfo.u.explicitValue->codebookSubsetRestriction.
         u.n4TxAntenna_tm6;
        *(osys_bitstring_ptr->data) = 1;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                phy_ded_cfg_ptr));
        /* Check for invalid codebookSubsetRestriction for TM6 */
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->transmissionMode = (i - 1);
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->
          transmissionMode = (lte_rrc_osys_AntennaInfoDedicated_transmissionMode) i;
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->
          codebookSubsetRestriction.u.n4TxAntenna_tm6->numbits = 32;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));  
        /* Check for invalid TM for SIMO config */
        phy_ded_cfg_ptr->antennaInfo.u.explicitValue->
          codebookSubsetRestriction.u.n4TxAntenna_tm6->numbits = 16;
        TF_STUB(lte_rrc_csp_get_num_tx_antennas) = lte_rrc_llc_test_csp_get_num_tx_antennas_simo;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                     phy_ded_cfg_ptr));       
        break;
			case 6:
				
				TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
				
				TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                phy_ded_cfg_ptr));
				
				TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_false;

				TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                phy_ded_cfg_ptr));
				break;
      default:
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_antenna_info(cfg_db_ptr, 
                                                                phy_ded_cfg_ptr));
    }
  }



  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_llc_test_get_fgi_bit_0;

  cfg_db_ptr->antenna_cfg_dedicated.transmission_mode = LTE_L1_DL_TM9_REL10;
  cfg_db_ptr->cqi_cfg.cqi_rel9_param_present = TRUE;
  cfg_db_ptr->cqi_cfg.cqi_rel9_param.pmi_ri_report_configured = TRUE;
  cfg_db_ptr->csi_rs_cfg.csi_rs.csi_rs_enabled = TRUE;
  cfg_db_ptr->csi_rs_cfg.csi_rs.antenna_port = LTE_L1_ANTENNA_INFO_PORTS_COUNT_1;
  cfg_db_ptr->cqi_cfg.cqi_reporting_mode_aperiodic_enable = FALSE;
  cfg_db_ptr->cqi_cfg.cqi_reporting_periodic_present = TRUE;

  cfg_db_ptr->cqi_cfg.cqi_reporting_periodic.format_indicator = LTE_L1_CQI_FORMAT_INDICATOR_PERIODIC_WIDEBAND;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_validate_cqi_mode_against_tm9(&cfg_db_ptr->cqi_cfg, 
                                                                                 &cfg_db_ptr->csi_rs_cfg.csi_rs));

  cfg_db_ptr->cqi_cfg.cqi_reporting_periodic.format_indicator = LTE_L1_CQI_FORMAT_INDICATOR_PERIODIC_SUBBAND;  
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_llc_test_get_fgi_bit_1;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_validate_cqi_mode_against_tm9(&cfg_db_ptr->cqi_cfg, 
                                                                                 &cfg_db_ptr->csi_rs_cfg.csi_rs));

  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_llc_test_get_fgi_bit_0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_cqi_mode_against_tm9(&cfg_db_ptr->cqi_cfg, 
                                                                                 &cfg_db_ptr->csi_rs_cfg.csi_rs));


  cfg_db_ptr->csi_rs_cfg.csi_rs.antenna_port = LTE_L1_ANTENNA_INFO_PORTS_COUNT_4;


  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_cqi_mode_against_tm9(&cfg_db_ptr->cqi_cfg, 
                                                                                 &cfg_db_ptr->csi_rs_cfg.csi_rs));

  cfg_db_ptr->cqi_cfg.cqi_reporting_mode_aperiodic_enable = TRUE;
  cfg_db_ptr->cqi_cfg.cqi_reporting_periodic_present = FALSE;

  cfg_db_ptr->cqi_cfg.cqi_reporting_mode_aperiodic = LTE_L1_CQI_MODE_APERIODIC_RM20;
  cfg_db_ptr->cqi_cfg.cqi_rel9_param.pmi_ri_report_configured = FALSE;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_cqi_mode_against_tm9(&cfg_db_ptr->cqi_cfg, 
                                                                                 &cfg_db_ptr->csi_rs_cfg.csi_rs));

  cfg_db_ptr->cqi_cfg.cqi_reporting_mode_aperiodic = LTE_L1_CQI_MODE_APERIODIC_RM22;
  cfg_db_ptr->cqi_cfg.cqi_rel9_param.pmi_ri_report_configured = TRUE;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_cqi_mode_against_tm9(&cfg_db_ptr->cqi_cfg, 
                                                                                 &cfg_db_ptr->csi_rs_cfg.csi_rs));



  /* -----------------------------------------------------------------------*/
    TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_ptr, lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  
  TF_MSG("\nLLC: PHY Dedicated Config IE Enum values test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcPhyDedicatedIEEnumValuesTest::Teardown()
{
  lte_rrc_utf_teardown();
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
#ifdef FEATURE_LTE_CA
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPhyCommonScellIEEnumValuesTest);

void LteRrcLlcPhyCommonScellIEEnumValuesTest::Setup()
{
	lte_rrc_test_efs_item_write_CA();
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL,0);
  lte_rrc_llc_test_stub_init();
  /* Set the release version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
}

void LteRrcLlcPhyCommonScellIEEnumValuesTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_SCellToAddModList_r10 *scell_add_mod_list = NULL;
  lte_rrc_osys_RadioResourceConfigCommonSCell_r10 *cfg_commom_ptr = NULL;
  lte_rrc_osys_MBSFN_SubframeConfigList *ota_mbsfn_area_cfg_ptr = NULL;
  lte_cphy_mbsfn_cfg_s *mbsfn_area_cfg_ptr = NULL;
  lte_rrc_osys_PHICH_Config phich_cfg;
  lte_l1_phich_cfg_param_s *phich_cfg_ptr = NULL;

  lte_rrc_llc_s *llc_ptr;
  lte_rrc_cfg_db_type *cfg_db_ptr;
  uint16 i = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: ScellRadioResource common Config IE Enum values test begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
#ifdef FEATURE_LTE_CA
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ca_scell_add_msg);
  encoded_pdu.value = llc_reconfig_ca_scell_add_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* ---------------------------------------------------------------------- */

  /*Assign values*/
  scell_add_mod_list = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10;

  scell_add_mod_list->n = 1;
  scell_add_mod_list->elem[0].m.cellIdentification_r10Present = 1;
  scell_add_mod_list->elem[0].cellIdentification_r10.physCellId_r10 = 0;
  scell_add_mod_list->elem[0].cellIdentification_r10.dl_CarrierFreq_r10 = 0;
  /* Validating Scell Common Config IE values */
  cfg_commom_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10.elem->radioResourceConfigCommonSCell_r10;

  /* PROCESS NON UL*/
  /*dl_bandwidth_r_10*/
  /*Invalid*/
  /*Check Needed*/
  TF_ASSERT(LTE_BW_NRB_100 == lte_rrc_llc_update_scell_common_cfg_dl_bwth(lte_rrc_osys_RadioResourceConfigCommonSCell_r10_nonUL_Configuration_r10_dl_Bandwidth_r10_n100));
  /*Valid*/
  cfg_commom_ptr->nonUL_Configuration_r10.dl_Bandwidth_r10 = lte_rrc_osys_RadioResourceConfigCommonSCell_r10_nonUL_Configuration_r10_dl_Bandwidth_r10_n6;
  TF_ASSERT(LTE_BW_NRB_100 == lte_rrc_llc_update_scell_common_cfg_dl_bwth(lte_rrc_osys_RadioResourceConfigCommonSCell_r10_nonUL_Configuration_r10_dl_Bandwidth_r10_n100));
  TF_ASSERT(LTE_BW_NRB_75 == lte_rrc_llc_update_scell_common_cfg_dl_bwth(lte_rrc_osys_RadioResourceConfigCommonSCell_r10_nonUL_Configuration_r10_dl_Bandwidth_r10_n75));
  TF_ASSERT(LTE_BW_NRB_50 == lte_rrc_llc_update_scell_common_cfg_dl_bwth(lte_rrc_osys_RadioResourceConfigCommonSCell_r10_nonUL_Configuration_r10_dl_Bandwidth_r10_n50));
  TF_ASSERT(LTE_BW_NRB_25 == lte_rrc_llc_update_scell_common_cfg_dl_bwth(lte_rrc_osys_RadioResourceConfigCommonSCell_r10_nonUL_Configuration_r10_dl_Bandwidth_r10_n25));
  TF_ASSERT(LTE_BW_NRB_15 == lte_rrc_llc_update_scell_common_cfg_dl_bwth(lte_rrc_osys_RadioResourceConfigCommonSCell_r10_nonUL_Configuration_r10_dl_Bandwidth_r10_n15));
  TF_ASSERT(LTE_BW_NRB_6 == lte_rrc_llc_update_scell_common_cfg_dl_bwth(lte_rrc_osys_RadioResourceConfigCommonSCell_r10_nonUL_Configuration_r10_dl_Bandwidth_r10_n6));

  /*Antenna Ports*/
  /*Invalid*/
  /*Check Needed*/
  TF_ASSERT(LTE_L1_ANTENNA_INFO_PORTS_COUNT_4 == lte_rrc_llc_update_scell_common_cfg_antenna_ports(lte_rrc_osys_AntennaInfoCommon_antennaPortsCount_an4));
  /*Valid*/
  cfg_commom_ptr->nonUL_Configuration_r10.antennaInfoCommon_r10.antennaPortsCount = lte_rrc_osys_AntennaInfoCommon_antennaPortsCount_an1;
  TF_ASSERT(LTE_L1_ANTENNA_INFO_PORTS_COUNT_4 == lte_rrc_llc_update_scell_common_cfg_antenna_ports(lte_rrc_osys_AntennaInfoCommon_antennaPortsCount_an4));
  TF_ASSERT(LTE_L1_ANTENNA_INFO_PORTS_COUNT_2 == lte_rrc_llc_update_scell_common_cfg_antenna_ports(lte_rrc_osys_AntennaInfoCommon_antennaPortsCount_an2));
  TF_ASSERT(LTE_L1_ANTENNA_INFO_PORTS_COUNT_1 == lte_rrc_llc_update_scell_common_cfg_antenna_ports(lte_rrc_osys_AntennaInfoCommon_antennaPortsCount_an1));

  /* MBSFN Present*/
  /*Set Bitmask*/
  cfg_commom_ptr->nonUL_Configuration_r10.m.mbsfn_SubframeConfigList_r10Present = 1;
  ota_mbsfn_area_cfg_ptr = &cfg_commom_ptr->nonUL_Configuration_r10.mbsfn_SubframeConfigList_r10;
  mbsfn_area_cfg_ptr = &llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_common_cfg.scell_dl_cfg.mbsfn_subframe_cfg;
  /*Out of bounds subfn allocations*/
  ota_mbsfn_area_cfg_ptr->n = LTE_RRC_LLC_MAX_MBSFN_SUBF_ALLOCATION+1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_mbsfn_sf_cfg(ota_mbsfn_area_cfg_ptr, mbsfn_area_cfg_ptr));
  /*In bounds subfcn allocations and Valid Allocation perriod*/
  ota_mbsfn_area_cfg_ptr->n = LTE_RRC_LLC_MIN_MBSFN_SUBF_ALLOCATION;
  ota_mbsfn_area_cfg_ptr->elem = (lte_rrc_osys_MBSFN_SubframeConfig *) lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MBSFN_SubframeConfig));
  for (i=0; i<6; i++)
  {
    switch (i)
    {
       case 0:
         ota_mbsfn_area_cfg_ptr->elem[0].radioframeAllocationPeriod = lte_rrc_osys_MBSFN_SubframeConfig_radioframeAllocationPeriod_n1;
         break; 
       case 1:
         ota_mbsfn_area_cfg_ptr->elem[0].radioframeAllocationPeriod = lte_rrc_osys_MBSFN_SubframeConfig_radioframeAllocationPeriod_n2;
         break;
       case 2:
         ota_mbsfn_area_cfg_ptr->elem[0].radioframeAllocationPeriod = lte_rrc_osys_MBSFN_SubframeConfig_radioframeAllocationPeriod_n4;
         break;
       case 3:
         ota_mbsfn_area_cfg_ptr->elem[0].radioframeAllocationPeriod = lte_rrc_osys_MBSFN_SubframeConfig_radioframeAllocationPeriod_n8;
         break;
       case 4:
         ota_mbsfn_area_cfg_ptr->elem[0].radioframeAllocationPeriod = lte_rrc_osys_MBSFN_SubframeConfig_radioframeAllocationPeriod_n16;
         break;
       case 5:
         ota_mbsfn_area_cfg_ptr->elem[0].radioframeAllocationPeriod = lte_rrc_osys_MBSFN_SubframeConfig_radioframeAllocationPeriod_n32;
         break;     
    }
    ota_mbsfn_area_cfg_ptr->elem[0].subframeAllocation.t = T_lte_rrc_osys_MBSFN_SubframeConfig_subframeAllocation_fourFrames+1;
    /*Check Needed*/
    //TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_mbsfn_sf_alloc_pattern_list_from_scell(ota_mbsfn_area_cfg_ptr, mbsfn_area_cfg_ptr));
    ota_mbsfn_area_cfg_ptr->elem[0].subframeAllocation.t = T_lte_rrc_osys_MBSFN_SubframeConfig_subframeAllocation_oneFrame;
    ota_mbsfn_area_cfg_ptr->elem[0].subframeAllocation.u.oneFrame = (ASN1BitStr32 *) lte_rrc_asn1_osys_IE_malloc(sizeof(ASN1BitStr32));
    ota_mbsfn_area_cfg_ptr->elem[0].subframeAllocation.u.oneFrame->numbits = 2;
    ota_mbsfn_area_cfg_ptr->elem[0].subframeAllocation.u.oneFrame->data[0] = 3;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_mbsfn_sf_cfg(ota_mbsfn_area_cfg_ptr, mbsfn_area_cfg_ptr));
    ota_mbsfn_area_cfg_ptr->elem[0].subframeAllocation.t = T_lte_rrc_osys_MBSFN_SubframeConfig_subframeAllocation_fourFrames;
    ota_mbsfn_area_cfg_ptr->elem[0].subframeAllocation.u.fourFrames = (ASN1BitStr32 *) lte_rrc_asn1_osys_IE_malloc(sizeof(ASN1BitStr32));
    ota_mbsfn_area_cfg_ptr->elem[0].subframeAllocation.u.fourFrames->numbits = 2;
    ota_mbsfn_area_cfg_ptr->elem[0].subframeAllocation.u.fourFrames->data[0] = 3;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_mbsfn_sf_cfg(ota_mbsfn_area_cfg_ptr, mbsfn_area_cfg_ptr));    
  }
  /*Invalid Allocation period*/
  ota_mbsfn_area_cfg_ptr->elem[0].radioframeAllocationPeriod = lte_rrc_osys_MBSFN_SubframeConfig_radioframeAllocationPeriod_n32+1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_mbsfn_sf_cfg(ota_mbsfn_area_cfg_ptr, mbsfn_area_cfg_ptr));
  ota_mbsfn_area_cfg_ptr->elem[0].radioframeAllocationPeriod = lte_rrc_osys_MBSFN_SubframeConfig_radioframeAllocationPeriod_n1;

  /* Phich config info*/
  phich_cfg = cfg_commom_ptr->nonUL_Configuration_r10.phich_Config_r10;
  phich_cfg_ptr = &llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_common_cfg.scell_dl_cfg.phich_cfg;
  /*Valid-invalid Phich duration and valid phich resource*/
  for (i=0; i<4; i++)
  {
    switch (i)
    {
       case 0:
         phich_cfg.phich_Resource = lte_rrc_osys_PHICH_Config_phich_Resource_oneSixth;
         break;
       case 1:
         phich_cfg.phich_Resource = lte_rrc_osys_PHICH_Config_phich_Resource_half;
         break;
       case 2:
         phich_cfg.phich_Resource = lte_rrc_osys_PHICH_Config_phich_Resource_one;
         break;
       case 3:
         phich_cfg.phich_Resource = lte_rrc_osys_PHICH_Config_phich_Resource_two;
         break;
    }
    phich_cfg.phich_Duration = lte_rrc_osys_PHICH_Config_phich_Duration_extended+1;
    TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_phich_cfg(phich_cfg, phich_cfg_ptr, FALSE));
    phich_cfg.phich_Duration = lte_rrc_osys_PHICH_Config_phich_Duration_extended;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_phich_cfg(phich_cfg, phich_cfg_ptr, FALSE));
    phich_cfg.phich_Duration = lte_rrc_osys_PHICH_Config_phich_Duration_normal;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_phich_cfg(phich_cfg, phich_cfg_ptr, FALSE));    
  }
  /*Invalid phich resource*/
  phich_cfg.phich_Resource = lte_rrc_osys_PHICH_Config_phich_Resource_two+1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_phich_cfg(phich_cfg, phich_cfg_ptr, FALSE));
  phich_cfg.phich_Resource = lte_rrc_osys_PHICH_Config_phich_Resource_oneSixth;

  /*Pdsch Config*/
  /*Invalid reference signal power*/
  cfg_commom_ptr->nonUL_Configuration_r10.pdsch_ConfigCommon_r10.referenceSignalPower = LTE_RRC_LLC_MAX_PDSCH_REG_SIG_PWR+1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_radio_resource_scell_cfg_common(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_commom_ptr));
  cfg_commom_ptr->nonUL_Configuration_r10.pdsch_ConfigCommon_r10.referenceSignalPower = LTE_RRC_LLC_MIN_PDSCH_REG_SIG_PWR;

  /*PROCRESS UL*/
  /*We dont support UL as of now. We can disable UL checks till we support uplink and bypass TC */
    cfg_commom_ptr->m.ul_Configuration_r10Present = 0;
#if 0
  /*UL info not present*/
  cfg_commom_ptr->m.ul_Configuration_r10Present = 1;
  /*Uplink bandwidth*/
  cfg_commom_ptr->ul_Configuration_r10.ul_FreqInfo_r10.m.ul_Bandwidth_r10Present = 1;
  /*Invalid*/
  cfg_commom_ptr->ul_Configuration_r10.ul_FreqInfo_r10.ul_Bandwidth_r10 = lte_rrc_osys_RadioResourceConfigCommonSCell_r10_ul_Configuration_r10_ul_FreqInfo_r10_ul_Bandwidth_r10_n100+1;
  lte_rrc_llc_process_radio_resource_scell_cfg_common(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_commom_ptr); 
  TF_ASSERT(llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_common_cfg.scell_ul_cfg.ul_bandwidth == llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_common_cfg.scell_dl_cfg.dl_bandwidth);
  /*Valid*/
  for (i=0;i<6;i++)
  {
    switch (i)
    {
       case (0):
         cfg_commom_ptr->ul_Configuration_r10.ul_FreqInfo_r10.ul_Bandwidth_r10 = lte_rrc_osys_RadioResourceConfigCommonSCell_r10_ul_Configuration_r10_ul_FreqInfo_r10_ul_Bandwidth_r10_n6;
         lte_rrc_llc_process_radio_resource_scell_cfg_common(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_commom_ptr);
         TF_ASSERT(LTE_BW_NRB_6 == llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_common_cfg.scell_ul_cfg.ul_bandwidth);
         break;
       case (1):
         cfg_commom_ptr->ul_Configuration_r10.ul_FreqInfo_r10.ul_Bandwidth_r10 = lte_rrc_osys_RadioResourceConfigCommonSCell_r10_ul_Configuration_r10_ul_FreqInfo_r10_ul_Bandwidth_r10_n15;
         lte_rrc_llc_process_radio_resource_scell_cfg_common(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_commom_ptr);
         TF_ASSERT(LTE_BW_NRB_15 == llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_common_cfg.scell_ul_cfg.ul_bandwidth);
         break;
       case (2):
         cfg_commom_ptr->ul_Configuration_r10.ul_FreqInfo_r10.ul_Bandwidth_r10 = lte_rrc_osys_RadioResourceConfigCommonSCell_r10_ul_Configuration_r10_ul_FreqInfo_r10_ul_Bandwidth_r10_n25;
         lte_rrc_llc_process_radio_resource_scell_cfg_common(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_commom_ptr);
         TF_ASSERT(LTE_BW_NRB_25 == llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_common_cfg.scell_ul_cfg.ul_bandwidth);
         break;
       case (3):
         cfg_commom_ptr->ul_Configuration_r10.ul_FreqInfo_r10.ul_Bandwidth_r10 = lte_rrc_osys_RadioResourceConfigCommonSCell_r10_ul_Configuration_r10_ul_FreqInfo_r10_ul_Bandwidth_r10_n50;
         lte_rrc_llc_process_radio_resource_scell_cfg_common(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_commom_ptr);
         TF_ASSERT(LTE_BW_NRB_50 == llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_common_cfg.scell_ul_cfg.ul_bandwidth);
         break;
       case (4):
         cfg_commom_ptr->ul_Configuration_r10.ul_FreqInfo_r10.ul_Bandwidth_r10 = lte_rrc_osys_RadioResourceConfigCommonSCell_r10_ul_Configuration_r10_ul_FreqInfo_r10_ul_Bandwidth_r10_n75;
         lte_rrc_llc_process_radio_resource_scell_cfg_common(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_commom_ptr);
         TF_ASSERT(LTE_BW_NRB_75 == llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_common_cfg.scell_ul_cfg.ul_bandwidth);
         break;
       case (5):
         cfg_commom_ptr->ul_Configuration_r10.ul_FreqInfo_r10.ul_Bandwidth_r10 = lte_rrc_osys_RadioResourceConfigCommonSCell_r10_ul_Configuration_r10_ul_FreqInfo_r10_ul_Bandwidth_r10_n100;
         lte_rrc_llc_process_radio_resource_scell_cfg_common(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_commom_ptr);
         TF_ASSERT(LTE_BW_NRB_100 == llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_common_cfg.scell_ul_cfg.ul_bandwidth);
         break;
    }    
  }
#endif
	scell_add_mod_list->elem[0].cellIdentification_r10.dl_CarrierFreq_r10 = 5730;

  /*Check for validity of entire scell common config*/
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_scell_cfg_common(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_commom_ptr));

  //scell_add_mod_list->elem[0].m.radioResourceConfigDedicatedSCell_r10Present = 0;
  /*Check for validity of Entire Scell Add_mod list*/
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10.elem->radioResourceConfigDedicatedSCell_r10.physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.m.antennaInfo_r10Present = 0;
  lte_rrc_llc_process_scell_add_modify_list(llc_ptr, scell_add_mod_list);
  #endif
  
  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_ptr, lte_rrc_osys_DL_DCCH_Message_PDU);
  
  TF_MSG("\nLLC: ScellRadioResource common Config IE Enum values test ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcPhyCommonScellIEEnumValuesTest::Teardown()
{
  lte_rrc_utf_teardown();
}


/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPhyDedicatedScellIEEnumValuesTest);

void LteRrcLlcPhyDedicatedScellIEEnumValuesTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL,0);
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  lte_rrc_llc_test_stub_init();
  /* Set the release version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
}

void LteRrcLlcPhyDedicatedScellIEEnumValuesTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_SCellToAddModList_r10 *scell_add_mod_list = NULL;
  lte_rrc_osys_RadioResourceConfigDedicatedSCell_r10 *cfg_ded_ptr = NULL;
  lte_cphy_antenna_cfg_dedicated_param_s* antenna_cfg_db_ptr = NULL;
  lte_rrc_osys_AntennaInfoDedicated_r10* antennaInfo_r10 = NULL;
  lte_cphy_common_scell_cfg_param_s *scell_common_cfg_db_ptr = NULL;
  lte_cphy_dedicated_csi_rs_config_s* csi_rs_cfg_db_ptr = NULL;
  lte_rrc_osys_CSI_RS_Config_r10* csi_rs_cfg_r10 = NULL;
  lte_l1_dl_tx_mode_e* transmission_mode = NULL;
  lte_rrc_osys_AntennaInfoUL_r10_transmissionModeUL_r10 transmission_mode_ul_r10 =  NULL;
  lte_rrc_osys_PhysicalConfigDedicated* phy_ded_cfg_ptr;
  ASN1DynBitStr *bit_str_ptr = NULL;

  lte_rrc_llc_s *llc_ptr;
  lte_rrc_cfg_db_type *cfg_db_ptr;
  uint16 i = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: ScellRadioResource dedicated Config IE Enum values test begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
#ifdef FEATURE_LTE_CA
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ca_scell_add_msg);
  encoded_pdu.value = llc_reconfig_ca_scell_add_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* ---------------------------------------------------------------------- */

  /*Assign values*/
  scell_add_mod_list = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10;

  phy_ded_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
    radioResourceConfigDedicated.physicalConfigDedicated;
  

  scell_add_mod_list->n = 1;
  scell_add_mod_list->elem[0].m.cellIdentification_r10Present = 1;
  scell_add_mod_list->elem[0].cellIdentification_r10.physCellId_r10 = 0;
  scell_add_mod_list->elem[0].cellIdentification_r10.dl_CarrierFreq_r10 = 0;
  /* Validating Scell Common Config IE values */
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10.elem->radioResourceConfigDedicatedSCell_r10;

  /* PROCESS NON UL*/
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.m.nonUL_Configuration_r10Present = 1;
  /*Antenna Info*/
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.m.antennaInfo_r10Present = 1;
  antenna_cfg_db_ptr =  &llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.dl_cfg.antenna_cfg_dedicated;
  antennaInfo_r10 = &cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.antennaInfo_r10;
  scell_common_cfg_db_ptr = &llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_common_cfg;
  /*Invalid*/  
  antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm9_v1020+1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency));
  /*Valid*/
  for (i=0; i<9; i++)
  {
    switch (i)
    {
       case 0:
         antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm1;
         lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency);
         TF_ASSERT(antenna_cfg_db_ptr->transmission_mode == LTE_L1_DL_TM1);
       case 1:
         antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm2;
         lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency);
         TF_ASSERT(antenna_cfg_db_ptr->transmission_mode == LTE_L1_DL_TM2);
       case 2:
         antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm3;
         lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency);
         TF_ASSERT(antenna_cfg_db_ptr->transmission_mode == LTE_L1_DL_TM3);
       case 3:
         antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm4;
         lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency);
         TF_ASSERT(antenna_cfg_db_ptr->transmission_mode == LTE_L1_DL_TM4);
       case 4:
         antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm5;        
         TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency));
       case 5:
         antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm6;
         lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency);
         TF_ASSERT(antenna_cfg_db_ptr->transmission_mode == LTE_L1_DL_TM6);
       case 6:
         antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm7;
         lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency);
         TF_ASSERT(antenna_cfg_db_ptr->transmission_mode == LTE_L1_DL_TM7);
       case 7:
         antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm8_v920;
         TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency));
       case 8:
         antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm9_v1020;
         lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency);
         TF_ASSERT(antenna_cfg_db_ptr->transmission_mode == LTE_L1_DL_TM9_REL10);
    }    
  }

  /*TM9 Validations*/
  antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm9_v1020;
  llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.ul_cfg.cqi_reporting_cfg.cqi_rel9_param.pmi_ri_report_configured = TRUE;
  llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.dl_cfg.csi_rs_cfg_dedicated.csi_rs.antenna_port = LTE_L1_ANTENNA_INFO_PORTS_COUNT_2;
  antennaInfo_r10->m.codebookSubsetRestriction_r10Present = 1;
  TF_ASSERT(E_SUCCESS ==lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency));
  TF_ASSERT(E_SUCCESS ==lte_rrc_llc_process_codebookrestriction_r10_for_scell(antenna_cfg_db_ptr, antennaInfo_r10,
                                          llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.ul_cfg.cqi_reporting_cfg.cqi_rel9_param.pmi_ri_report_configured,
                                          llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.dl_cfg.csi_rs_cfg_dedicated.csi_rs.antenna_port,
                                          llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.dl_cfg.csi_rs_cfg_dedicated.csi_rs.csi_rs_enabled));
  TF_ASSERT(antenna_cfg_db_ptr->transmission_mode == LTE_L1_DL_TM9_REL10);

  /*TM9 Validations*/
  antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm9_v1020;
  llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.ul_cfg.cqi_reporting_cfg.cqi_rel9_param.pmi_ri_report_configured = FALSE;
  llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.dl_cfg.csi_rs_cfg_dedicated.csi_rs.antenna_port = LTE_L1_ANTENNA_INFO_PORTS_COUNT_2;
  antennaInfo_r10->m.codebookSubsetRestriction_r10Present = 1;
  TF_ASSERT(E_SUCCESS ==lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency));
  TF_ASSERT(E_SUCCESS ==lte_rrc_llc_process_codebookrestriction_r10_for_scell(antenna_cfg_db_ptr, antennaInfo_r10,
                                          llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.ul_cfg.cqi_reporting_cfg.cqi_rel9_param.pmi_ri_report_configured,
                                          llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.dl_cfg.csi_rs_cfg_dedicated.csi_rs.antenna_port,
                                          llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.dl_cfg.csi_rs_cfg_dedicated.csi_rs.csi_rs_enabled));
  TF_ASSERT(antenna_cfg_db_ptr->transmission_mode == LTE_L1_DL_TM9_REL10);

  /*TM9 Validations*/
  antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm9_v1020;
  llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.ul_cfg.cqi_reporting_cfg.cqi_rel9_param.pmi_ri_report_configured = TRUE;
  llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.dl_cfg.csi_rs_cfg_dedicated.csi_rs.antenna_port = LTE_L1_ANTENNA_INFO_PORTS_COUNT_1;
  antennaInfo_r10->m.codebookSubsetRestriction_r10Present = 1;
  TF_ASSERT(E_SUCCESS ==lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency));
  TF_ASSERT(E_SUCCESS ==lte_rrc_llc_process_codebookrestriction_r10_for_scell(antenna_cfg_db_ptr, antennaInfo_r10,
                                          llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.ul_cfg.cqi_reporting_cfg.cqi_rel9_param.pmi_ri_report_configured,
                                          llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.dl_cfg.csi_rs_cfg_dedicated.csi_rs.antenna_port,
                                          llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.dl_cfg.csi_rs_cfg_dedicated.csi_rs.csi_rs_enabled));
  TF_ASSERT(antenna_cfg_db_ptr->transmission_mode == LTE_L1_DL_TM9_REL10);

  /*TM9 validation for CBR (i.e. CBR is retained from previous reconfig)*/
  cfg_db_ptr->cqi_cfg.cqi_rel9_param.pmi_ri_report_configured = TRUE;
  cfg_db_ptr->csi_rs_cfg.csi_rs.antenna_port = LTE_L1_ANTENNA_INFO_PORTS_COUNT_2;
  cfg_db_ptr->antenna_cfg_dedicated.transmission_mode = LTE_L1_DL_TM9_REL10;
  phy_ded_cfg_ptr->antennaInfo_r10.t = T_lte_rrc_osys_PhysicalConfigDedicated_antennaInfo_r10_explicitValue_r10;
  phy_ded_cfg_ptr->antennaInfo_r10.u.explicitValue_r10 = (lte_rrc_osys_AntennaInfoDedicated_r10 *)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_AntennaInfoDedicated_r10));
  phy_ded_cfg_ptr->antennaInfo_r10.u.explicitValue_r10->m.codebookSubsetRestriction_r10Present = 0;
  cfg_db_ptr->antenna_cfg_dedicated.codebook_subset_restriction = 10;
  TF_ASSERT(E_SUCCESS ==lte_rrc_llc_process_codebookrestriction_r10(cfg_db_ptr, phy_ded_cfg_ptr));
  TF_ASSERT(cfg_db_ptr->antenna_cfg_dedicated.codebook_subset_restriction == 0);

  lte_rrc_llc_process_radio_resource_scell_cfg_ded(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_ded_ptr);
    
  /*Validate TM aganist antennas*/
  scell_common_cfg_db_ptr->scell_dl_cfg.antenna_ports.antenna_ports_count = LTE_L1_ANTENNA_INFO_PORTS_COUNT_1;
  antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm2;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency));
  scell_common_cfg_db_ptr->scell_dl_cfg.antenna_ports.antenna_ports_count = LTE_L1_ANTENNA_INFO_PORTS_COUNT_2;
  antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency));
  antennaInfo_r10->m.codebookSubsetRestriction_r10Present = 0;
  antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm3;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency));
  TF_ASSERT(E_DATA_INVALID ==lte_rrc_llc_process_codebookrestriction_r10_for_scell(antenna_cfg_db_ptr, antennaInfo_r10,FALSE, LTE_L1_ANTENNA_INFO_PORTS_COUNT_1, TRUE));
 
  scell_common_cfg_db_ptr->scell_dl_cfg.antenna_ports.antenna_ports_count = LTE_L1_ANTENNA_INFO_PORTS_COUNT_1;
  antennaInfo_r10->transmissionMode_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm1;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_rel10_antenna_info_for_scell(antenna_cfg_db_ptr, antennaInfo_r10, scell_common_cfg_db_ptr, llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_id_info.dl_cell_frequency));

  /*CSI RS Config info*/
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.csi_RS_Config_r10.m.csi_RS_r10Present = 1;
  csi_rs_cfg_db_ptr = &llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.dl_cfg.csi_rs_cfg_dedicated;
  csi_rs_cfg_r10 = &cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.csi_RS_Config_r10;
  csi_rs_cfg_r10->m.csi_RS_r10Present = 1;
  csi_rs_cfg_r10->csi_RS_r10.t = 0;
  /*Invalid*/
  csi_rs_cfg_r10->csi_RS_r10.u.setup = (lte_rrc_osys_CSI_RS_Config_r10_csi_RS_r10_setup *) lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CSI_RS_Config_r10_csi_RS_r10_setup));
  csi_rs_cfg_r10->csi_RS_r10.u.setup->antennaPortsCount_r10 = lte_rrc_osys_CSI_RS_Config_r10_csi_RS_r10_setup_antennaPortsCount_r10_an8+1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_rel10_csi_rs_cfg_info(csi_rs_cfg_db_ptr, csi_rs_cfg_r10));
  for (i=0;i<4;i++)
  {
    csi_rs_cfg_r10->csi_RS_r10.u.setup->antennaPortsCount_r10 = i;
    lte_rrc_llc_update_rel10_csi_rs_cfg_info(csi_rs_cfg_db_ptr, csi_rs_cfg_r10);
    TF_ASSERT(csi_rs_cfg_db_ptr->csi_rs.antenna_port == 1<<i);
  }
  csi_rs_cfg_r10->m.zeroTxPowerCSI_RS_r10Present = 0;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_rel10_csi_rs_cfg_info(csi_rs_cfg_db_ptr, csi_rs_cfg_r10));

  /*Pdsch config dedicated*/
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.m.pdsch_ConfigDedicated_r10Present = 1;
  /*Invalid p_a*/
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.pdsch_ConfigDedicated_r10.p_a = lte_rrc_osys_PDSCH_ConfigDedicated_p_a_dB3+1;
  lte_rrc_llc_process_radio_resource_scell_cfg_ded(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_ded_ptr );
  TF_ASSERT(LTE_L1_PA_DB_0 == llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.dl_cfg.pdsch_cfg_dedicated.pa);
  /*Valid p_a*/
  for (i=0;i<8;i++)
  {
   cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.pdsch_ConfigDedicated_r10.p_a = i;
  lte_rrc_llc_process_radio_resource_scell_cfg_ded(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_ded_ptr );
  TF_ASSERT(i == llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.dl_cfg.pdsch_cfg_dedicated.pa);
  }

  /*PROCRESS UL*/
  /*Phy UL config*/
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.m.ul_Configuration_r10Present = 1;
  /*Antenna Info*/
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.m.antennaInfoUL_r10Present = 0;
  transmission_mode = &llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_dedicated_cfg.ul_cfg.scell_ul_antenna_cfg.transmission_mode;
  transmission_mode_ul_r10 = cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.antennaInfoUL_r10.transmissionModeUL_r10;
  /*Invalid*/
  transmission_mode_ul_r10 = lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm9_v1020+1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_antenna_info_ul_tran_mode_r10(transmission_mode, transmission_mode_ul_r10));
  /*Valid*/
  for (i=0;i<2;i++)
  {
    transmission_mode_ul_r10 = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_antenna_info_ul_tran_mode_r10(transmission_mode, transmission_mode_ul_r10));
    TF_ASSERT(i+1 == *transmission_mode);
  }

  // TM9 Validation for SCell
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.m.ul_Configuration_r10Present = 1;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.m.cqi_ReportConfigSCell_r10Present = 1;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.cqi_ReportConfigSCell_r10.m.cqi_ReportModeAperiodic_r10Present =1;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.cqi_ReportConfigSCell_r10.
    cqi_ReportModeAperiodic_r10  = lte_rrc_osys_CQI_ReportModeAperiodic_rm22;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.cqi_ReportConfigSCell_r10.pmi_RI_Report_r10 = 0;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.m.nonUL_Configuration_r10Present = 1;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.m.antennaInfo_r10Present = 1;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.antennaInfo_r10.transmissionMode_r10 = 
    lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm9_v1020;
  
  llc_ptr->dd_ptr->oc_ptr->scell_config.scell_cfg_list[1].scell_common_cfg.scell_dl_cfg.dl_bandwidth = LTE_BW_UNKNOWN;
  
  /*Check for validity of entire scell common config*/
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_scell_cfg_ded(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_ded_ptr));
#endif

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_ptr, lte_rrc_osys_DL_DCCH_Message_PDU);
  
  TF_MSG("\nLLC: ScellRadioResource dedicated Config IE Enum values test ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcPhyDedicatedScellIEEnumValuesTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPhyDedicatedScellIESRSULSCellTest);

void LteRrcLlcPhyDedicatedScellIESRSULSCellTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL,0);
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  lte_rrc_llc_test_stub_init();
  /* Set the release version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
}

void LteRrcLlcPhyDedicatedScellIESRSULSCellTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_SCellToAddModList_r10 *scell_add_mod_list = NULL;
  lte_rrc_osys_RadioResourceConfigDedicatedSCell_r10 *cfg_ded_ptr = NULL;
  lte_cphy_antenna_cfg_dedicated_param_s* antenna_cfg_db_ptr = NULL;
  lte_rrc_osys_AntennaInfoDedicated_r10* antennaInfo_r10 = NULL;
  lte_cphy_common_scell_cfg_param_s *scell_common_cfg_db_ptr = NULL;
  lte_cphy_dedicated_csi_rs_config_s* csi_rs_cfg_db_ptr = NULL;
  lte_rrc_osys_CSI_RS_Config_r10* csi_rs_cfg_r10 = NULL;
  lte_l1_dl_tx_mode_e* transmission_mode = NULL;
  lte_rrc_osys_AntennaInfoUL_r10_transmissionModeUL_r10 transmission_mode_ul_r10 =  NULL;

  lte_rrc_llc_s *llc_ptr;
  lte_rrc_cfg_db_type *cfg_db_ptr;
  uint16 i = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: ScellRadioResource dedicated Config IE Enum values test begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
 
#ifdef FEATURE_LTE_CA 
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ca_scell_add_msg);
  encoded_pdu.value = llc_reconfig_ca_scell_add_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* ---------------------------------------------------------------------- */

  /*Assign values*/
  scell_add_mod_list = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10;

  scell_add_mod_list->n = 1;
  scell_add_mod_list->elem[0].m.cellIdentification_r10Present = 1;
  scell_add_mod_list->elem[0].cellIdentification_r10.physCellId_r10 = 0;
  scell_add_mod_list->elem[0].cellIdentification_r10.dl_CarrierFreq_r10 = 0;
  
  /* Validating Scell Common Config IE values */
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10.elem->radioResourceConfigDedicatedSCell_r10;

  /* Validating SRS_UL_ConfigDedicated*/
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.m.antennaInfo_r10Present = 0;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.m.csi_RS_Config_r10Present = 0;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.m.pdsch_ConfigDedicated_r10Present = 0;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.m.antennaInfoUL_r10Present = 0;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.m.pusch_ConfigDedicatedSCell_r10Present = 0;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.m.uplinkPowerControlDedicatedSCell_r10Present = 0;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.m.soundingRS_UL_ConfigDedicated_r10Present = 0;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.m.soundingRS_UL_ConfigDedicated_v1020Present = 0;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.m.pdsch_ConfigDedicated_r10Present = 0;



  
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.m.ul_Configuration_r10Present = 1;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.m.soundingRS_UL_ConfigDedicatedAperiodic_r10Present = 1;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.soundingRS_UL_ConfigDedicatedAperiodic_r10.t = T_lte_rrc_osys_SoundingRS_UL_ConfigDedicatedAperiodic_r10_release;

  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_scell_cfg_ded(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_ded_ptr));

  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.m.soundingRS_UL_ConfigDedicatedAperiodic_r10Present = 1;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.ul_Configuration_r10.soundingRS_UL_ConfigDedicatedAperiodic_r10.t = T_lte_rrc_osys_SoundingRS_UL_ConfigDedicatedAperiodic_r10_setup;

  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_radio_resource_scell_cfg_ded(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_ded_ptr));
  #endif

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_ptr, lte_rrc_osys_DL_DCCH_Message_PDU);
  
  TF_MSG("\nLLC: ScellRadioResource dedicated Config IE Enum values test ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcPhyDedicatedScellIESRSULSCellTest::Teardown()
{
  lte_rrc_utf_teardown();
}

#endif

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPhyDedicatedMeasSubframePatternPCellTest);

void LteRrcLlcPhyDedicatedMeasSubframePatternPCellTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL,0);
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  lte_rrc_llc_test_stub_init();
  /* Set the release version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
}

void LteRrcLlcPhyDedicatedMeasSubframePatternPCellTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_SCellToAddModList_r10 *scell_add_mod_list = NULL;
  lte_rrc_osys_RadioResourceConfigDedicated *cfg_ded_ptr = NULL;
  lte_cphy_antenna_cfg_dedicated_param_s* antenna_cfg_db_ptr = NULL;
  lte_rrc_osys_AntennaInfoDedicated_r10* antennaInfo_r10 = NULL;
  lte_cphy_common_scell_cfg_param_s *scell_common_cfg_db_ptr = NULL;
  lte_cphy_dedicated_csi_rs_config_s* csi_rs_cfg_db_ptr = NULL;
  lte_rrc_osys_CSI_RS_Config_r10* csi_rs_cfg_r10 = NULL;
  lte_l1_dl_tx_mode_e* transmission_mode = NULL;
  lte_rrc_osys_AntennaInfoUL_r10_transmissionModeUL_r10 transmission_mode_ul_r10 =  NULL;

  lte_rrc_llc_s *llc_ptr;
  lte_rrc_cfg_db_type *cfg_db_ptr;
  uint16 i = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: ScellRadioResource dedicated Config IE Enum values test begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
 
#ifdef FEATURE_LTE_CA 
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ca_scell_add_msg);
  encoded_pdu.value = llc_reconfig_ca_scell_add_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* ---------------------------------------------------------------------- */

  /*Assign values*/
  scell_add_mod_list = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10;

  scell_add_mod_list->n = 1;
  scell_add_mod_list->elem[0].m.cellIdentification_r10Present = 1;
  scell_add_mod_list->elem[0].cellIdentification_r10.physCellId_r10 = 0;
  scell_add_mod_list->elem[0].cellIdentification_r10.dl_CarrierFreq_r10 = 0;
  
  /* Validating Meas Subframe Pattern PCell IE values */
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                   rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;

  cfg_ded_ptr->m.measSubframePatternPCell_r10Present = 1;
  cfg_ded_ptr->measSubframePatternPCell_r10.t = T_lte_rrc_osys_MeasSubframePatternPCell_r10_release; 
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));

  cfg_ded_ptr->measSubframePatternPCell_r10.t = T_lte_rrc_osys_MeasSubframePatternPCell_r10_setup;
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_llc_test_get_fgi_bit_0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_radio_resource_cfg_ded(llc_ptr, cfg_ded_ptr));
#endif

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_ptr, lte_rrc_osys_DL_DCCH_Message_PDU);
  
  TF_MSG("\nLLC: ScellRadioResource dedicated Config IE Enum values test ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcPhyDedicatedMeasSubframePatternPCellTest::Teardown()
{
  lte_rrc_utf_teardown();
}
#ifdef FEATURE_LTE_CA

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPhyDedicatedScellIESRSULTest);

void LteRrcLlcPhyDedicatedScellIESRSULTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL,0);
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  lte_rrc_llc_test_stub_init();
  /* Set the release version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
}

void LteRrcLlcPhyDedicatedScellIESRSULTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_SCellToAddModList_r10 *scell_add_mod_list = NULL;
  lte_rrc_osys_PhysicalConfigDedicated *cfg_ded_ptr = NULL;
  lte_cphy_antenna_cfg_dedicated_param_s* antenna_cfg_db_ptr = NULL;
  lte_rrc_osys_AntennaInfoDedicated_r10* antennaInfo_r10 = NULL;
  lte_cphy_common_scell_cfg_param_s *scell_common_cfg_db_ptr = NULL;
  lte_cphy_dedicated_csi_rs_config_s* csi_rs_cfg_db_ptr = NULL;
  lte_rrc_osys_CSI_RS_Config_r10* csi_rs_cfg_r10 = NULL;
  lte_l1_dl_tx_mode_e* transmission_mode = NULL;
  lte_rrc_osys_AntennaInfoUL_r10_transmissionModeUL_r10 transmission_mode_ul_r10 =  NULL;

  lte_rrc_llc_s *llc_ptr;
  lte_rrc_cfg_db_type *cfg_db_ptr;
  uint16 i = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: ScellRadioResource dedicated Config IE Enum values test begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
#ifdef FEATURE_LTE_CA
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_ca_scell_add_msg);
  encoded_pdu.value = llc_reconfig_ca_scell_add_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* ---------------------------------------------------------------------- */

  /*Assign values*/
  scell_add_mod_list = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10;

  scell_add_mod_list->n = 1;
  scell_add_mod_list->elem[0].m.cellIdentification_r10Present = 1;
  scell_add_mod_list->elem[0].cellIdentification_r10.physCellId_r10 = 0;
  scell_add_mod_list->elem[0].cellIdentification_r10.dl_CarrierFreq_r10 = 0;
  
  /* Validating Meas Subframe Pattern PCell IE values */
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
                   u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.physicalConfigDedicated;

  cfg_ded_ptr->m.soundingRS_UL_ConfigDedicatedAperiodic_r10Present = 1;
  cfg_ded_ptr->soundingRS_UL_ConfigDedicatedAperiodic_r10.t = T_lte_rrc_osys_SoundingRS_UL_ConfigDedicatedAperiodic_r10_release;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_pcell_rel10_param(llc_ptr->dd_ptr->oc_ptr, cfg_ded_ptr));

  cfg_ded_ptr->soundingRS_UL_ConfigDedicatedAperiodic_r10.t = T_lte_rrc_osys_SoundingRS_UL_ConfigDedicatedAperiodic_r10_setup;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_pcell_rel10_param(llc_ptr->dd_ptr->oc_ptr, cfg_ded_ptr));

  cfg_ded_ptr->m.soundingRS_UL_ConfigDedicatedAperiodic_r10Present = 0;
  cfg_ded_ptr->m.additionalSpectrumEmissionCA_r10Present = 1;
  cfg_ded_ptr->additionalSpectrumEmissionCA_r10.t = T_lte_rrc_osys_PhysicalConfigDedicated_additionalSpectrumEmissionCA_r10_release;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_pcell_rel10_param(llc_ptr->dd_ptr->oc_ptr, cfg_ded_ptr));

  cfg_ded_ptr->additionalSpectrumEmissionCA_r10.t = T_lte_rrc_osys_PhysicalConfigDedicated_additionalSpectrumEmissionCA_r10_setup;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_pcell_rel10_param(llc_ptr->dd_ptr->oc_ptr, cfg_ded_ptr));
#endif

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_ptr, lte_rrc_osys_DL_DCCH_Message_PDU);
  
  TF_MSG("\nLLC: ScellRadioResource dedicated Config IE Enum values test ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcPhyDedicatedScellIESRSULTest::Teardown()
{
  lte_rrc_utf_teardown();
}

#endif

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPhyDedicatedCSI_RSIEEnumValuesTest);

void LteRrcLlcPhyDedicatedCSI_RSIEEnumValuesTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL,0);
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  lte_rrc_llc_test_stub_init();
  /* Set the release version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;
}

void LteRrcLlcPhyDedicatedCSI_RSIEEnumValuesTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_CSI_RS_Config_r10* csi_rs_cfg_r10; /*!< Dedicated csi_RS_Config_r10 info from ASN.1*/ 
  lte_rrc_llc_s *llc_ptr;
  lte_rrc_cfg_db_type *cfg_db_ptr;
  uint16 i = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: PHY Dedicated Config IE Enum values test begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
#ifdef FEATURE_LTE_CA
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_csi_rs_config_msg);
  encoded_pdu.value = llc_reconfig_csi_rs_config_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* Validating all CSI_RS_r10 IE values */
  csi_rs_cfg_r10 = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    physicalConfigDedicated.csi_RS_Config_r10;
  for(i=0;i<32;i++)
  {	
    csi_rs_cfg_r10->csi_RS_r10.u.setup->resourceConfig_r10 = i;
    /* As per Table 6.10.5.2-1 of 36.211, valid range of resourceConfig_r10 for FDD is 0-19 */
    if(i<=19)
    {
      TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_rel10_csi_rs_cfg_info(&cfg_db_ptr->csi_rs_cfg, csi_rs_cfg_r10));
    }
    else
    {
      TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_rel10_csi_rs_cfg_info(&cfg_db_ptr->csi_rs_cfg, csi_rs_cfg_r10));
    }
  }
  /* During HO onto TDD*/
  llc_ptr->dd_ptr->oc_ptr->ho_config_in_progress  = LTE_RRC_LLC_HANDOVER_TDD_INTRA_FREQ; 
  llc_ptr->dd_ptr->oc_ptr->ho_target_cell_info.dl_cell_frequency = 36000;
  for(i=0;i<32;i++)
  {  
   csi_rs_cfg_r10->csi_RS_r10.u.setup->resourceConfig_r10 = i;
   /* As per Table 6.10.5.2-1 of 36.211, valid range of resourceConfig_r10 for FDD is 0-19 */   
   TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_rel10_csi_rs_cfg_info(&cfg_db_ptr->csi_rs_cfg, csi_rs_cfg_r10));   
  }
  llc_ptr->dd_ptr->oc_ptr->ho_target_cell_info.dl_cell_frequency = 100;
  for(i=0;i<32;i++)
  {	
    csi_rs_cfg_r10->csi_RS_r10.u.setup->resourceConfig_r10 = i;
    /* As per Table 6.10.5.2-1 of 36.211, valid range of resourceConfig_r10 for FDD is 0-19 */
    if(i<=19)
    {
      TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_rel10_csi_rs_cfg_info(&cfg_db_ptr->csi_rs_cfg, csi_rs_cfg_r10));
    }
    else
    {
      TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_rel10_csi_rs_cfg_info(&cfg_db_ptr->csi_rs_cfg, csi_rs_cfg_r10));
    }
  }

#endif

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_ptr, lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  
  TF_MSG("\nLLC: PHY Dedicated Config IE Enum values test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcPhyDedicatedCSI_RSIEEnumValuesTest::Teardown()
{
  lte_rrc_utf_teardown();
}








