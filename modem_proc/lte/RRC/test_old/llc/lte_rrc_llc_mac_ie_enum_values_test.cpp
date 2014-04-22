/*!
  @file lte_rrc_llc_mac_ie_enum_values_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of all MAC 
  config IEs, including common and dedicated parameters with enum values, for 
  all possible values and other range value checks.

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
07/17/09   mm     Initial Revision
           
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

extern "C"
{ 
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
}

/*===========================================================================

                                INTERNAL DEFINITIONS

===========================================================================*/
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcMACIEEnumValuesTest);

void LteRrcLlcMACIEEnumValuesTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL,0);
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
}

void LteRrcLlcMACIEEnumValuesTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_RACH_ConfigCommon rach_cfg_common;
  lte_rrc_osys_MAC_MainConfig* trch_cfg_ptr = NULL;
  lte_rrc_osys_LogicalChannelConfig *lc_cfg_ptr = NULL;
  lte_rrc_llc_s *llc_ptr;
  lte_rrc_cfg_db_type *cfg_db_ptr;
  uint16 i = 0, bucketSizeDuration = 0;
  uint32 short_drx_cycle = 0, long_drx_cycle = 0, on_duration_tmr = 0;
  lte_lc_id_t lc_id = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: MAC Config IE Enum values test begins \n");
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
                               sizeof(llc_reconfig_ho_msg),
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* ---------------------------------------------------------------------- */
  /* Validating RACH Common Config IE values */
  /* ---------------------------------------------------------------------- */

  rach_cfg_common = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo.
    radioResourceConfigCommon.rach_ConfigCommon;

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_RA_PREAMBLES_GROUP_A; i++)
  {
    rach_cfg_common.preambleInfo.preamblesGroupAConfig.sizeOfRA_PreamblesGroupA = i;
    if (i <= LTE_RRC_LLC_TEST_MAX_MESSAGE_SIZE_GROUP_A)
    {
      rach_cfg_common.preambleInfo.preamblesGroupAConfig.messageSizeGroupA = i;
    }
    if (i <= LTE_RRC_LLC_TEST_MAX_MESSAGE_PWR_OFFSET_GROUP_B)
    {
      rach_cfg_common.preambleInfo.preamblesGroupAConfig.
        messagePowerOffsetGroupB = i;
    }
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_preambles_grp_a_config(&cfg_db_ptr->mac_rach_cfg,
                                                                   rach_cfg_common));
  }

  /*Check for invalid sizeOfRA_PreamblesGroupA value */
  rach_cfg_common.preambleInfo.preamblesGroupAConfig.sizeOfRA_PreamblesGroupA = i;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_preambles_grp_a_config(&cfg_db_ptr->mac_rach_cfg,
                                                                 rach_cfg_common));
  TF_ASSERT(cfg_db_ptr->mac_rach_cfg.ra_index1 == 36);

  /*Check for invalid messageSizeGroupA value handling */
  rach_cfg_common.preambleInfo.preamblesGroupAConfig.
   messageSizeGroupA = i;
  i = 0;
  rach_cfg_common.preambleInfo.preamblesGroupAConfig.sizeOfRA_PreamblesGroupA = i;
  rach_cfg_common.preambleInfo.preamblesGroupAConfig.messagePowerOffsetGroupB = lte_rrc_osys_RACH_ConfigCommon_preambleInfo_preamblesGroupAConfig_messagePowerOffsetGroupB_minusinfinity;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_preambles_grp_a_config(&cfg_db_ptr->mac_rach_cfg,
                                                                 rach_cfg_common));

  /* Check for invalid messagePowerOffsetGroupB value handling */
  rach_cfg_common.preambleInfo.preamblesGroupAConfig.messageSizeGroupA = i;
  rach_cfg_common.preambleInfo.preamblesGroupAConfig.
    messagePowerOffsetGroupB = (LTE_RRC_LLC_TEST_MAX_MESSAGE_PWR_OFFSET_GROUP_B + 1);
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_preambles_grp_a_config(&cfg_db_ptr->mac_rach_cfg,
                                                                 rach_cfg_common));
  TF_ASSERT(cfg_db_ptr->mac_rach_cfg.ra_index1 == 36);
  /* ---------------------------------------------------------------------- */

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_RA_PREAMBLES; i++)
  {
    rach_cfg_common.preambleInfo.numberOfRA_Preambles = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_num_ra_preambles(&cfg_db_ptr->mac_rach_cfg.ra_index2,
                                                            rach_cfg_common));
  }

  /*Check for invalid numberOfRA_Preambles value */
  rach_cfg_common.preambleInfo.numberOfRA_Preambles = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_num_ra_preambles(&cfg_db_ptr->mac_rach_cfg.ra_index2,
                                                               rach_cfg_common));  
  /* ---------------------------------------------------------------------- */

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_PREAMBLE_INITIAL_PWR; i++)
  {
    rach_cfg_common.powerRampingParameters.preambleInitialReceivedTargetPower = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_preamble_initial_power(&cfg_db_ptr->mac_rach_cfg.
                                                                  preamble_initial_power,
                                                                  rach_cfg_common));
  }

  /*Check for invalid numberOfRA_Preambles value */
  rach_cfg_common.powerRampingParameters.preambleInitialReceivedTargetPower = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_preamble_initial_power(&cfg_db_ptr->mac_rach_cfg.
                                                                     preamble_initial_power,
                                                                     rach_cfg_common));
  /* ---------------------------------------------------------------------- */

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_PWR_RAMPING_STEP; i++)
  {
    rach_cfg_common.powerRampingParameters.powerRampingStep = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_power_ramping_step(&cfg_db_ptr->mac_rach_cfg.
                                                              power_ramping_step,
                                                              rach_cfg_common));
  }

  /*Check for invalid powerRampingStep value */
  rach_cfg_common.powerRampingParameters.powerRampingStep = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_power_ramping_step(&cfg_db_ptr->mac_rach_cfg.
                                                                 power_ramping_step,
                                                                 rach_cfg_common));
  /* ---------------------------------------------------------------------- */

  for (i = 0;i <= LTE_RRC_LLC_TEST_PREAMBLE_TRANS_MAX; i++)
  {
    rach_cfg_common.ra_SupervisionInfo.preambleTransMax = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_preamble_trans_max(&cfg_db_ptr->mac_rach_cfg.
                                                              preamble_trans_max,
                                                              rach_cfg_common));
  }

  /*Check for invalid preambleTransMax value */
  rach_cfg_common.ra_SupervisionInfo.preambleTransMax = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_preamble_trans_max(&cfg_db_ptr->mac_rach_cfg.
                                                                 preamble_trans_max,
                                                                 rach_cfg_common));
  /* ---------------------------------------------------------------------- */

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_RA_RESPONSE_WIN_SIZE; i++)
  {
    rach_cfg_common.ra_SupervisionInfo.ra_ResponseWindowSize = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_ra_response_win_size(&cfg_db_ptr->mac_rach_cfg.
                                                                l1_rach.ra_rsp_win_size,
                                                                rach_cfg_common));
  }

  /*Check for invalid ra_ResponseWindowSize value */
  rach_cfg_common.ra_SupervisionInfo.ra_ResponseWindowSize = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_ra_response_win_size(&cfg_db_ptr->mac_rach_cfg.
                                                                   l1_rach.ra_rsp_win_size,
                                                                   rach_cfg_common));
  /* ---------------------------------------------------------------------- */

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_CONTENTION_RESOLUTION_TMR; i++)
  {
    rach_cfg_common.ra_SupervisionInfo.mac_ContentionResolutionTimer = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_contention_resolution_tmr(&cfg_db_ptr->mac_rach_cfg.
                                                                     contention_resolution_timer,
                                                                     rach_cfg_common));
  }

  /*Check for invalid mac_ContentionResolutionTimer value */
  rach_cfg_common.ra_SupervisionInfo.mac_ContentionResolutionTimer = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_contention_resolution_tmr(&cfg_db_ptr->mac_rach_cfg.
                                                                        contention_resolution_timer,
                                                                        rach_cfg_common));

  /* ---------------------------------------------------------------------- */
  /* Validating MAC-Main Config IE values */
  /* ---------------------------------------------------------------------- */

  /* Validating UL SCH Config IE values*/

  trch_cfg_ptr = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    mac_MainConfig.u.explicitValue;

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_HARQ_TX; i++)
  {
    trch_cfg_ptr->ul_SCH_Config.maxHARQ_Tx = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_max_harq_tx(&cfg_db_ptr->ulsch_cfg.max_harq_tx,
                                                       trch_cfg_ptr));
  }
  // Check for Spare value; Success is returned since error will be ignored
  trch_cfg_ptr->ul_SCH_Config.maxHARQ_Tx = i;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_max_harq_tx(&cfg_db_ptr->ulsch_cfg.max_harq_tx,
                                                     trch_cfg_ptr));
  /* ---------------------------------------------------------------------- */

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_PERIODIC_BSR_TMR; i++)
  {
    trch_cfg_ptr->ul_SCH_Config.periodicBSR_Timer = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_bsr_timer(trch_cfg_ptr,&cfg_db_ptr->mac_ul_cfg,
      (trch_cfg_ptr->ul_SCH_Config.m.periodicBSR_TimerPresent == 1)));
  }
  // Check for Spare value; Success is returned since error will be ignored
  trch_cfg_ptr->ul_SCH_Config.periodicBSR_Timer = i;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_bsr_timer(trch_cfg_ptr,&cfg_db_ptr->mac_ul_cfg,
    (trch_cfg_ptr->ul_SCH_Config.m.periodicBSR_TimerPresent == 1)));

  // Check for BSR not present handling
  /* Reset BSR present bitmask */
  trch_cfg_ptr->ul_SCH_Config.m.periodicBSR_TimerPresent = 0;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_bsr_timer(trch_cfg_ptr,&cfg_db_ptr->mac_ul_cfg,
    (trch_cfg_ptr->ul_SCH_Config.m.periodicBSR_TimerPresent == 1)));
  /* ---------------------------------------------------------------------- */

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_RETX_BSR_TMR; i++)
  {
    trch_cfg_ptr->ul_SCH_Config.retxBSR_Timer = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_retx_bsr_timer(trch_cfg_ptr, &cfg_db_ptr->mac_ul_cfg));
  }
  // Check for Spare value
  trch_cfg_ptr->ul_SCH_Config.retxBSR_Timer = i;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_retx_bsr_timer(trch_cfg_ptr, &cfg_db_ptr->mac_ul_cfg));

  /* ---------------------------------------------------------------------- */

  /* Validting TA timer values */
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_TA_TMR; i++)
  {
    trch_cfg_ptr->timeAlignmentTimerDedicated = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_ta_timer(trch_cfg_ptr->timeAlignmentTimerDedicated, 
                                                    &cfg_db_ptr->mac_dl_cfg));
  }

  /* Check for invalid timeAlignmentTimerDedicated value */
  trch_cfg_ptr->timeAlignmentTimerDedicated = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_ta_timer(trch_cfg_ptr->timeAlignmentTimerDedicated, 
                                                       &cfg_db_ptr->mac_dl_cfg));
  /* ---------------------------------------------------------------------- */

  /* Validating PHR Config IE values */
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_PERIODIC_PHR_TMR; i++)
  {
    trch_cfg_ptr->phr_Config.u.setup->periodicPHR_Timer = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_phr_config(cfg_db_ptr,trch_cfg_ptr));
  }

  /* Check for invalid periodicPHR_Timer value */
  trch_cfg_ptr->phr_Config.u.setup->periodicPHR_Timer = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_phr_config(cfg_db_ptr,trch_cfg_ptr));
  /* ---------------------------------------------------------------------- */

  /* Reset periodicPHR_Timer to a valid value before the next test */
  trch_cfg_ptr->phr_Config.u.setup->periodicPHR_Timer = (i - 1);
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_PROHIBIT_PHR_TMR; i++)
  {
    trch_cfg_ptr->phr_Config.u.setup->prohibitPHR_Timer = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_phr_config(cfg_db_ptr,trch_cfg_ptr));
  }

  /* Check for invalid prohibitPHR_Timer value */
  trch_cfg_ptr->phr_Config.u.setup->prohibitPHR_Timer = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_phr_config(cfg_db_ptr,trch_cfg_ptr));
  /* ---------------------------------------------------------------------- */

  /* Reset prohibitPHR_Timer to a valid value before the next test */
  trch_cfg_ptr->phr_Config.u.setup->prohibitPHR_Timer = (i - 1);
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_DL_PATHLOSS_CHANGE; i++)
  {
    trch_cfg_ptr->phr_Config.u.setup->dl_PathlossChange = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_phr_config(cfg_db_ptr,trch_cfg_ptr));
  }

  /* Check for invalid dl_PathlossChange value */
  trch_cfg_ptr->phr_Config.u.setup->dl_PathlossChange = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_phr_config(cfg_db_ptr,trch_cfg_ptr));


  /* Check for PHR Config disabled handling */
  trch_cfg_ptr->phr_Config.t = T_lte_rrc_osys_MAC_MainConfig_phr_Config_release;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_phr_config(cfg_db_ptr,trch_cfg_ptr));
  /* ---------------------------------------------------------------------- */

  /* Validating Connected DRX Config IE values */
  trch_cfg_ptr->drx_Config.t = T_lte_rrc_osys_DRX_Config_setup;
  trch_cfg_ptr->drx_Config.u.setup = 
    (lte_rrc_osys_DRX_Config_setup *) lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_DRX_Config_setup));
  trch_cfg_ptr->drx_Config.u.setup->m.shortDRXPresent = 1;

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_ON_DURATION_TMR; i++)
  {
    trch_cfg_ptr->drx_Config.u.setup->onDurationTimer = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_drx_on_duration_timer(&cfg_db_ptr->connected_drx_cfg,
                                                                    trch_cfg_ptr,&on_duration_tmr));
  }

  /* Check for invalid onDurationTimer value */
  trch_cfg_ptr->drx_Config.u.setup->onDurationTimer = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_drx_on_duration_timer(&cfg_db_ptr->connected_drx_cfg,
                                                                       trch_cfg_ptr,&on_duration_tmr));
  /* ---------------------------------------------------------------------- */

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_DRX_INACTIVITY_TMR; i++)
  {
    trch_cfg_ptr->drx_Config.u.setup->drx_InactivityTimer = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_drx_inactivity_timer(&cfg_db_ptr->connected_drx_cfg,
                                                                   trch_cfg_ptr));
  }
  // Check spare value
  trch_cfg_ptr->drx_Config.u.setup->drx_InactivityTimer = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_drx_inactivity_timer(&cfg_db_ptr->connected_drx_cfg,
                                                                      trch_cfg_ptr));
  /* ---------------------------------------------------------------------- */

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_DRX_RETX_TMR; i++)
  {
    trch_cfg_ptr->drx_Config.u.setup->drx_RetransmissionTimer = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_drx_retx_timer(&cfg_db_ptr->connected_drx_cfg,
                                                             trch_cfg_ptr));
  }

  /* Check for invalid drx_RetransmissionTimer value */
    trch_cfg_ptr->drx_Config.u.setup->drx_RetransmissionTimer = i;
    TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_drx_retx_timer(&cfg_db_ptr->connected_drx_cfg,
                                                                  trch_cfg_ptr));
  /* ---------------------------------------------------------------------- */

  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_LONG_DRX_CYCLE_START_OFFSET; i++)
  {
    trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.t = i + 1;
    switch(trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.t)
    {
      case 1:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf10 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF10;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf10 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF10 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
      case 2:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf20 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF20;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf20 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF20 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
      case 3:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf32 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF32;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf32 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF32 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
      case 4:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf40 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF40;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf40 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF40 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
      case 5:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf64 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF64;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf64 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF64 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
      case 6:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf80 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF80;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf80 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF80 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
      case 7:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf128 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF128;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf128 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF128 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
      case 8:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf160 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF160;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf160 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF160 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
      case 9:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf256 = 255;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));   
        break;
      case 10:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf320 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF320;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf320 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF320 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
      case 11:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf512 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF512;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf512 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF512 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
      case 12:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf640 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF640;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf640 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF640 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
      case 13:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf1024 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF1024;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf1024 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF1024 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
      case 14:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf1280 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF1280;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf1280 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF1280 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
      case 15:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf2048 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF2048;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf2048 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF2048 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
      case 16:
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf2560 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF2560;
        TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                              trch_cfg_ptr,
                                                                              &long_drx_cycle));
        trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.
          sf2560 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF2560 + 1;
        TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                                   trch_cfg_ptr,
                                                                                   &long_drx_cycle));
        break;
        
    }
  }

  /* Check for invalid choice for longDRX_CycleStartOffset */
  trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.t = i + 1;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                             trch_cfg_ptr,
                                                                             &long_drx_cycle));
    
  /* Validate Short DRX parameter values */
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_SHORT_DRX_CYCLE; i++)
  {
    trch_cfg_ptr->drx_Config.u.setup->shortDRX.shortDRX_Cycle = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_short_drx_cycle(&cfg_db_ptr->connected_drx_cfg,
                                                              trch_cfg_ptr,&short_drx_cycle));
  }

  /* Check for invalid short_drx_cycle value */
  trch_cfg_ptr->drx_Config.u.setup->shortDRX.shortDRX_Cycle = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_short_drx_cycle(&cfg_db_ptr->connected_drx_cfg,
                                                                 trch_cfg_ptr,&short_drx_cycle));

  /* Check for ShortDRX cycle length > LongDRX cycle length */
  i = 7;  // Corresponds to sf40
  trch_cfg_ptr->drx_Config.u.setup->shortDRX.shortDRX_Cycle = i;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_short_drx_cycle(&cfg_db_ptr->connected_drx_cfg,
                                                            trch_cfg_ptr,&short_drx_cycle));
  trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.
    t = T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf20;
  trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf20 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF20;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                        trch_cfg_ptr,
                                                                        &long_drx_cycle));
  trch_cfg_ptr->drx_Config.u.setup->onDurationTimer = i;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_drx_on_duration_timer(&cfg_db_ptr->connected_drx_cfg,
                                                                  trch_cfg_ptr,&on_duration_tmr));
  /* Validation fails because shortDRX cycle > longDRX cycle */
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_connected_drx_config(&cfg_db_ptr->connected_drx_cfg,short_drx_cycle,long_drx_cycle,on_duration_tmr));

  /* Check for OnDurationTimer > ShortDRX cycle length */
  i = 0;  // Corresponds to sf1
  trch_cfg_ptr->drx_Config.u.setup->shortDRX.shortDRX_Cycle = i;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_short_drx_cycle(&cfg_db_ptr->connected_drx_cfg,
                                                            trch_cfg_ptr,&short_drx_cycle));
  trch_cfg_ptr->drx_Config.u.setup->onDurationTimer = (i + 2);
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_drx_on_duration_timer(&cfg_db_ptr->connected_drx_cfg,
                                                                  trch_cfg_ptr,&on_duration_tmr));
  /* Validation fails because OnDurationTimer > ShortDRX cycle length */
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_connected_drx_config(&cfg_db_ptr->connected_drx_cfg,short_drx_cycle,long_drx_cycle,on_duration_tmr));

  /* Check for LongDRX cycle length % ShortDRX cycle length != 0 */
  i = 2;  // Corresponds to sf8
  trch_cfg_ptr->drx_Config.u.setup->shortDRX.shortDRX_Cycle = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_update_short_drx_cycle(&cfg_db_ptr->connected_drx_cfg,
                                                                 trch_cfg_ptr,&short_drx_cycle));
  trch_cfg_ptr->drx_Config.u.setup->onDurationTimer = i;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_drx_on_duration_timer(&cfg_db_ptr->connected_drx_cfg,
                                                                  trch_cfg_ptr,&on_duration_tmr));
  trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.
    t = T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf20;
  trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf20 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF20;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                        trch_cfg_ptr,
                                                                        &long_drx_cycle));
  /* Validation fails because LongDRX cycle length % ShortDRX cycle length != 0 */
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_connected_drx_config(&cfg_db_ptr->connected_drx_cfg,short_drx_cycle,long_drx_cycle,on_duration_tmr));

  cfg_db_ptr->connected_drx_cfg.short_drx_cycle_enable = FALSE;
  /* Check for OnDurationTimer > LongDRX cycle length */
  trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.
    t = T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf10;
  trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf10 = LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF10;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_long_drx_cycle_start_offset(&cfg_db_ptr->connected_drx_cfg,
                                                                        trch_cfg_ptr,
                                                                        &long_drx_cycle));
  i = 8;  // Corresponds to psf20
  trch_cfg_ptr->drx_Config.u.setup->onDurationTimer = i;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_drx_on_duration_timer(&cfg_db_ptr->connected_drx_cfg,
                                                                  trch_cfg_ptr,&on_duration_tmr));
  /* Validation fails because OnDurationTimer > LongDRX cycle length */
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_validate_connected_drx_config(&cfg_db_ptr->connected_drx_cfg,short_drx_cycle,long_drx_cycle,on_duration_tmr));
  
  /* ---------------------------------------------------------------------- */
  /* Validating Logical Channel Config IE values */
  /* ---------------------------------------------------------------------- */
  lc_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->logicalChannelConfig;
  lc_id = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->logicalChannelIdentity;

  /* Validating PBR */
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_PBR; i++)
  {
    lc_cfg_ptr->ul_SpecificParameters.prioritisedBitRate = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_pbr(lc_cfg_ptr,
                                               &cfg_db_ptr->mac_lc_list[lc_id].pbr,
                                               lc_id));
  }

  /* Check for spare value */
  lc_cfg_ptr->ul_SpecificParameters.prioritisedBitRate = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_pbr(lc_cfg_ptr,
                                                  &cfg_db_ptr->mac_lc_list[lc_id].pbr,
                                                  lc_id));

  /* ---------------------------------------------------------------------- */
  /* Validating Token Bucket Size*/
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_BUCKET_SIZE_DURATION; i++)
  {
    lc_cfg_ptr->ul_SpecificParameters.bucketSizeDuration = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_bucket_size_duration(lc_cfg_ptr,
                                                                &cfg_db_ptr->mac_lc_list[lc_id],
                                                                &bucketSizeDuration));
  }

  /* Check for spare value */
  lc_cfg_ptr->ul_SpecificParameters.bucketSizeDuration = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_bucket_size_duration(lc_cfg_ptr,
                                                                   &cfg_db_ptr->mac_lc_list[lc_id],
                                                                   &bucketSizeDuration));


  /* ---------------------------------------------------------------------- */

  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_ptr, lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  
  TF_MSG("\nLLC: MAC Config IE Enum values test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcMACIEEnumValuesTest::Teardown()
{
  lte_rrc_utf_teardown();
}










