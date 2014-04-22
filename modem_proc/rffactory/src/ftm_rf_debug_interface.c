/*!
  @file
  ftm_rf_debug_interface.c

  @brief
  Common framework to perform radio test in FTM mode
*/

/*======================================================================================================================

  Copyright (c) 2015 - 2021 Qualcomm Technologies Incorporated. 
  All Rights Reserved

  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential 
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are 
  expressly reserved. By accepting this material the recipient agrees that this material and the 
  information contained therein are held in confidence and in trust and will not be used, copied, 
  reproduced in whole or in part, nor its contents revealed in any manner to others without the 
  express written permission of Qualcomm Technologies Incorporated.

======================================================================================================================*/

/*======================================================================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rffactory.mpss/1.0/src/ftm_rf_debug_interface.c#79 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------------------------------------------
02/09/21   lzh     Added new properties(2 thru 5) for device cal
01/15/21   hep     Added placeholder VDPD Cal properties 15 thru 20 for GNB
04/08/20   hep     Added placeholder for Waveform Immediate Trigger for GNB
01/30/20   hep     Added placeholder for Tx Override IQ Compensation properties (36,37) for GNB
01/09/20   hep     Added placeholder for Tx Override Tx DCOC properties (34,35) for GNB
09/12/18   mpt     Updated Tx Override with SPR Action property
09/07/18   mpt     Add tech enter/exit functionality
08/02/18   mz      support VSWR Measure in CAL mode
04/04/18   jps     Add self test EVM measurement support in calibration mode
10/25/17   abr     Define codebook load property type in radio config command
10/02/17   abr     Add Action type and Antenna Element ID properties for codebook override command
07/21/17   mpt     Add PLL_ID property for radio config
06/05/17   dj      Add codebook index as a parameter for codebook override
05/22/17   dj      Add property types for beam lut override
05/17/17   br      Add command for DPD Debug mode override
05/15/17   dj      Initial support for codebook override
04/14/17   br      Add property types for VDPD Conversion
04/11/17   rsr     Add property types for 5G TX overide.
12/01/17   br      Add property enums  for Load Unity DPD and VDPD cal
08/22/16   br      Update property names for Radio config
07/27/16   br      Update property names for Radio config
07/25/16   mpt     Initial Release

======================================================================================================================*/

#include "comdef.h"
#include "rflm_defs.h"
#include "ftm_rf_debug_interface.h"



/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM RF DEBUG RADIO CONFIG Properties
  used to validate whether critical properties are present in the incoming packet
*/
const uint8 ftm_rf_debug_radio_config_prop_attrib_list[] = 
{
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*0 :  FTM_RF_DEBUG_RADIO_CFG_PROP_UNASSIGNED*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*1 :  FTM_RF_DEBUG_RADIO_CFG_PROP_IS_TEARDOWN*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*2 :  FTM_RF_DEBUG_RADIO_CFG_PROP_RADIO_SETUP_TYPE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*3 :  FTM_RF_DEBUG_RADIO_CFG_PROP_RFM_DEVICE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*4 :  FTM_RF_DEBUG_RADIO_CFG_PROP_SIG_PATH*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*5 :  FTM_RF_DEBUG_RADIO_CFG_PROP_ANT_PATH*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*6 :  FTM_RF_DEBUG_RADIO_CFG_PROP_RFM_PATH_TYPE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*7 :  FTM_RF_DEBUG_RADIO_CFG_PROP_BAND*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*8 :  FTM_RF_DEBUG_RADIO_CFG_PROP_SUBBAND*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*9 :  FTM_RF_DEBUG_RADIO_CFG_PROP_RESERVED*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,         /*10 : FTM_RF_DEBUG_RADIO_CFG_PROP_CHANNEL*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*11 : FTM_RF_DEBUG_RADIO_CFG_PROP_WAVEFORM */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*12 : FTM_RF_DEBUG_RADIO_CFG_PROP_BANDWIDTH*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*13 : FTM_RF_DEBUG_RADIO_CFG_PROP_NUM_RB*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*14 : FTM_RF_DEBUG_RADIO_CFG_PROP_START_RB*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*15 : FTM_RF_DEBUG_RADIO_CFG_PROP_CW_OFFSET*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*16 : FTM_RF_DEBUG_RADIO_CFG_PROP_IS_DC*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*17 : FTM_RF_DEBUG_RADIO_CFG_PROP_MOD_TYPE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*18 : FTM_RF_DEBUG_RADIO_CFG_PROP_LOOPBACK_TYPE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*19 : FTM_RF_DEBUG_RADIO_CFG_PROP_BEAM_ID*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*20 : FTM_RF_DEBUG_RADIO_CFG_PROP_CC_INDEX */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*21 : FTM_RF_DEBUG_RADIO_CFG_PROP_CC_START_RB*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*22 : FTM_RF_DEBUG_RADIO_CFG_PROP_CC_NUM_RB*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*23 : FTM_RF_DEBUG_RADIO_CFG_PROP_CC_BANDWIDTH*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*24 : TM_RF_DEBUG_RADIO_CFG_PROP_LOOPBACK_RFM_DEVICE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*25 : FTM_RF_DEBUG_RADIO_CFG_PROP_PLL_ID*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*26 : FTM_RF_DEBUG_RADIO_CFG_PROP_TUNE_TX_TO_RX_FREQ*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,      /*27 : FTM_RF_DEBUG_RADIO_CFG_PROP_LOAD_CODEBOOK*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,          /*28 :FTM_RF_DEBUG_RADIO_CFG_PROP_FREQUENCY*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,          /*29 :FTM_RF_DEBUG_RADIO_CFG_PROP_VERSION*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,          /*30 :FTM_RF_DEBUG_RADIO_CFG_PROP_SWITCH_TDSCDMA_WAVEFORM*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,         /*31 :FTM_RF_DEBUG_RADIO_CFG_PROP_SET_MOD*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,         /*32 :FTM_RF_DEBUG_RADIO_CFG_PROP_CC_SCS*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,         /*33 :FTM_RF_DEBUG_RADIO_CFG_PROP_WAVEFORM_ORIGIN*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,         /*34 :FTM_RF_DEBUG_RADIO_CFG_PROP_NDR_STATE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,         /*35 : FTM_RF_DEBUG_RADIO_CFG_PROP_NR5G_MOD_TYPE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,         /*36 :FTM_RF_DEBUG_RADIO_CFG_PROP_WAVEFORM_IMMEDIATE_TRIGGER*/  
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,         /*37 :FTM_RF_DEBUG_RADIO_CFG_PROP_VPA_MAX_MV*/  
};
// ensure the attribute list size is same as number of properies in property enum 
COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_radio_config_prop_attrib_list) == FTM_RF_DEBUG_RADIO_CFG_PROP_NUM);
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM RF DEBUG TX OVERRIDE Properties
	used to validate whether critical properties are present in the incoming packet
*/
const uint8 ftm_rf_debug_tx_override_prop_attrib_list[] = 
{
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_UNASSIGNED*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_TX_ENABLE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_PATH_INDEX*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_SIG_PATH*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_RFM_DEVICE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_ANT_PATH*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_XPT_MODE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_IQ_GAIN_ACTION*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_IQ_GAIN*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_TXFE_GAIN_ACTION*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_TXFE_GAIN*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_ENV_SCALE_ACTION*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_ENV_SCALE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_RGI*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_PA_BIAS*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_PA_STATE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_PA_CURRENT*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_DELAY_ACTION*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_DELAY*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_DATA_TYPE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_SLOT_NUMBER*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_DATA_SOURCE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_MOD_TYPE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_ENV_GAIN*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_BEAM_ID*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_PRE_COMB_IQ_GAIN_ACTION*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_CARR_IDX*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_PRE_COMB_IQ_GAIN*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_RF_TRX_IDX*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_PLL_ID*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_SPR_ACTION*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_TX_MOD_TYPE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_MIXER_GAIN*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_PHASE_SHIFTER*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_DCOC_I_COMP*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_DCOC_Q_COMP*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_IQ_COMP_GAIN*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_IQ_COMP_CROSS*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*RESERVED*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_ET_VMIN*/
};

COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_tx_override_prop_attrib_list) == FTM_RF_DEBUG_TX_OVERRIDE_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM RF DEBUG RX OVERRIDE Properties
	used to validate whether critical properties are present in the incoming packet
*/
const uint8 ftm_rf_debug_rx_override_prop_attrib_list[] = 
{
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK, /* Unassigned property */
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,  /* RF path index */
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,  /* Signal Path for AT forward */ 
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK, /* RFM device for this measurement*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK, /* RFM device for this measurement*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK, /* Expected AGC Value */
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK, /* LNA Gain State */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK, /* RxAGC value */ 
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK, /* NBEE value */
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK, /* Signal Type */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK, /* GSM only:Data type  */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK, /* GSM only: Measured RSSI */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /* GSM only: Rx Enable */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /* PLL ID */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /*RESERVED_1*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /*RESERVED_2*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /*RESERVED_3*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,   /*RESERVED_4*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,   /*RESERVED*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /*RESERVED_5*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /*RESERVED_6*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /*RESERVED_7*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK   /*RESERVED_8*/
  //FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /* (13) 5GNR only: Beam Id */
  //FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /* (14) 5GNR only: Set Rx Gain State Action */
  //FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /* (15) 5GNR only: Set DVGA Offset Action */
  //FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /* (16) 5GNR only: CC Index for DVGA Offset */
  //FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK   /* (17) 5GNR only: DVGA Offset Value */

};

COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_rx_override_prop_attrib_list) == FTM_RF_DEBUG_RX_OVERRIDE_PROP_NUM_UPDATED);
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM RF DEBUG COMMAND DISCOVERY Properties
	used to validate whether critical properties are present in the incoming packet
*/
const uint8 ftm_rf_debug_command_discovery_prop_attrib_list[] = 
{
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK, /* Unassigned property */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /* Command bitmask output */ 
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK  /* Command bitmask output */

};

COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_command_discovery_prop_attrib_list) == FTM_RF_DEBUG_COMMAND_DISCOVERY_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM RF DEBUG Tx Measure Properties
	used to validate whether critical properties are present in the incoming packet
*/
const uint8 ftm_rf_debug_tx_measure_prop_attrib_list[] = 
{
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_UNASSIGNED*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_MEASURE_PROP_PATH_INDEX*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_MEASURE_PROP_SIG_PATH*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_MEASURE_PROP_RFM_DEVICE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_MEASURE_PROP_ANT_PATH*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_MEASURE_PROP_EXPECTED_PWR */
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_MEASURE_PROP_FBRX_GAIN_STATE */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_REF_TXPWR */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_MEASURED_PWR */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_EXPECTED_HW_GAIN */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_HW_GAIN */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_HW_GAIN_DELTA */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_TRUNC_BIT  */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_LS_ERROR */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_PREDET_INDEX */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_FW_GAIN  */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,      /*FTM_RF_DEBUG_TX_MEASURE_PROP_SS_REF_TXPWR */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_FBRX_MODE */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_MEASUREMENT_TYPE */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_UTRA1_POS   */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_UTRA1_NEG   */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_UTRA2_POS  */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_UTRA2_NEG  */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_EUTRA_POS */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_EUTRA_NEG */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,          /*FTM_RF_DEBUG_TX_MEASURE_PROP_IQ_GAIN_ACTION */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,          /*FTM_RF_DEBUG_TX_MEASURE_PROP_IQ_GAIN */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,          /*FTM_RF_DEBUG_TX_MEASURE_PROP_TXFE_GAIN_ACTION */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,          /*FTM_RF_DEBUG_TX_MEASURE_PROP_TXFE_GAIN */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,          /*FTM_RF_DEBUG_TX_MEASURE_PROP_ENV_SCALE_ACTION*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,          /*FTM_RF_DEBUG_TX_MEASURE_PROP_ENV_SCALE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,          /*FTM_RF_DEBUG_TX_MEASURE_PROP_RGI*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,          /*FTM_RF_DEBUG_TX_MEASURE_PROP_PA_BIAS*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,          /*FTM_RF_DEBUG_TX_MEASURE_PROP_PA_STATE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,         /*FTM_RF_DEBUG_TX_MEASURE_PROP_PA_CURRENT*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,          /*FTM_RF_DEBUG_TX_MEASURE_PROP_DELAY_ACTION*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,          /*FTM_RF_DEBUG_TX_MEASURE_PROP_DELAY*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_PLL_ID */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_EVM */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_XPT_MODE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_VSWR_S11_MAG*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_CONVERGED_IQ_GAIN*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_TX_MEASURE_PROP_ET_VMIN */
};

COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_tx_measure_prop_attrib_list) == FTM_RF_DEBUG_TX_MEASURE_PROP_NUM);
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM RF DEBUG LOAD UNITY DPD Properties
	used to validate whether critical properties are present in the incoming packet
*/
const uint8 ftm_rf_debug_load_unity_dpd_prop_attrib_list[] = 
{
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_UNASSIGNED*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_PATH_INDEX*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_SIG_PATH*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_RFM_DEVICE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_XPT_MODE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_ETVD_MAX*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_ETVD_MIN*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,    /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_DET*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,    /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_QPOET_VMIN*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_QPOET_VMAX*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_PLL_ID*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_SCALE_DET*/

};

COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_load_unity_dpd_prop_attrib_list) == FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_NUM);
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM RF DEBUG VDPD Cal Properties
	used to validate whether critical properties are present in the incoming packet
*/
const uint8 ftm_rf_debug_vdpd_cal_prop_attrib_list[] = 
{
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_VDPD_CAL_PROP_UNASSIGNED*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_VDPD_CAL_PROP_PATH_INDEX*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_VDPD_CAL_PROP_RFM_DEVICE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_VDPD_CAL_PROP_SIG_PATH*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_VDPD_CAL_PROP_XPT_MODE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_VDPD_CAL_PROP_SNR*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_VDPD_CAL_PROP_EPT_COMPRESSION*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,      /*FTM_RF_DEBUG_VDPD_CAL_PROP_KERNEL_MASK*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,      /*FTM_RF_DEBUG_VDPD_CAL_PROP_QFACTOR */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*FTM_RF_DEBUG_VDPD_CAL_PROP_NUM_KERNEL_WEIGHTS*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*FTM_RF_DEBUG_VDPD_CAL_PROP_KERNEL_WEIGHT_I*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*FTM_RF_DEBUG_VDPD_CAL_PROP_KERNEL_WEIGHT_Q */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*FTM_RF_DEBUG_VDPD_CAL_PROP_RESERVED */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*FTM_RF_DEBUG_VDPD_CAL_PROP_PROC_ONLY */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*FTM_RF_DEBUG_VDPD_CAL_PROP_PLL_ID */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*FTM_RF_DEBUG_VDPD_CAL_PROP_DBL_PRECISION */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*FTM_RF_DEBUG_VDPD_CAL_PROP_ACLR_RIGHT */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*FTM_RF_DEBUG_VDPD_CAL_PROP_ACLR_LEFT */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*FTM_RF_DEBUG_VDPD_CAL_PROP_ACLR_MET */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*FTM_RF_DEBUG_VDPD_CAL_PROP_SNR_MET */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*FTM_RF_DEBUG_VDPD_CAL_PROP_FBRX_PWR_DBM10 */            
};

COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_vdpd_cal_prop_attrib_list) == FTM_RF_DEBUG_VDPD_CAL_PROP_NUM);
/*--------------------------------------------------------------------------------------------------------------------*/
/*! 
  @brief
  mandatory attribute for FTM RF DEBUG VDPD Conversion Properties
	used to validate whether critical properties are present in the incoming packet
*/
const uint8 ftm_rf_debug_vdpd_conversion_prop_attrib_list[] = 
{
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_UNASSIGNED*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,      /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_KERNEL_MASK*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,      /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_QFACTOR */
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,       /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_KERNEL_WEIGHT_I*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,       /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_KERNEL_WEIGHT_Q */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,   /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_NUM_LUT_VALUES */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,   /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_AMAM */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,   /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_AMPM*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,   /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_GAIN*/
};

COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_vdpd_conversion_prop_attrib_list) == FTM_RF_DEBUG_VDPD_CONVERSION_PROP_NUM);
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM RF DEBUG Load DPD Properties
	used to validate whether critical properties are present in the incoming packet
*/
const uint8 ftm_rf_debug_load_dpd_prop_attrib_list[] = 
{
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_LOAD_DPD_PROP_UNASSIGNED*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_LOAD_DPD_PROP_RFM_DEVICE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_LOAD_DPD_PROP_SIG_PATH*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_LOAD_DPD_PROP_XPT_MODE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,      /*FTM_RF_DEBUG_LOAD_DPD_PROP_KERNEL_MASK*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,      /*FTM_RF_DEBUG_LOAD_DPD_PROP_QFACTOR */
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,       /*FTM_RF_DEBUG_LOAD_DPD_PROP_KERNEL_WEIGHT_I*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,       /*FTM_RF_DEBUG_LOAD_DPD_PROP_KERNEL_WEIGHT_Q */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*FTM_RF_DEBUG_LOAD_DPD_PROP_PLL_ID */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*  FTM_RF_DEBUG_LOAD_DPD_PROP_ETVD_MAX*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,       /*FTM_RF_DEBUG_LOAD_DPD_PROP_ETVD_MIN*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_LOAD_DPD_PROP_DET */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_LOAD_DPD_PROP_SCALE_DET */
};

COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_load_dpd_prop_attrib_list) == FTM_RF_DEBUG_LOAD_DPD_PROP_NUM);
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM RF DEBUG Set DPD Debug Mode Properties
	used to validate whether critical properties are present in the incoming packet
*/
const uint8 ftm_rf_debug_set_dpd_debug_mode_prop_attrib_list[] = 
{
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_UNASSIGNED*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_RFM_DEVICE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_SIG_PATH*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_DEBUG_MODE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,         /*FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_PLL_ID*/

};

COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_set_dpd_debug_mode_prop_attrib_list) == FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM RF DEBUG VDPD Cal Properties
	used to validate whether critical properties are present in the incoming packet
*/
const uint8 ftm_rf_debug_idc_cal_prop_attrib_list[] = 
{
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_IDC_CAL_PROP_UNASSIGNED*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_IDC_CAL_PROP_RFM_DEVICE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_IDC_CAL_PROP_SIG_PATH*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_IDC_CAL_PROP_PLL_ID*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_TX_OFFSET*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_IDC_CAL_PROP_I_CODE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_IDC_CAL_PROP_Q_CODE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_IM2_POWER_REF_CODE*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_IM2_BEST_POWER*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_POWER_BEST*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_POWER_DELTA*/
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_IDC_IIP2_CAL_THERM_ADC*/
};

COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_idc_cal_prop_attrib_list) == FTM_RF_DEBUG_IDC_CAL_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM RF DEBUG TECH ENTER/EXIT Properties
	used to validate whether critical properties are present in the incoming packet
*/
const uint8 ftm_rf_debug_tech_enter_exit_prop_attrib_list[] = 
{
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /* FTM_RF_DEBUG_TECH_ENTER_EXIT_UNASSIGNED */
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,      /* FTM_RF_DEBUG_TECH_ENTER_EXIT_MODE */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /* FTM_RF_DEBUG_TECH_ENTER_EXIT_ENABLE_COOKBOOK_LOAD */
};

COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_command_discovery_prop_attrib_list) == FTM_RF_DEBUG_COMMAND_DISCOVERY_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM RF DEBUG DEVICE CAL Properties
	used to validate whether critical properties are present in the incoming packet
*/
const uint8 ftm_rf_debug_device_cal_prop_attrib_list[] = 
{
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /* FTM_RF_DEBUG_DEVICE_CAL_UNASSIGNED */
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,      /* FTM_RF_DEBUG_DEVICE_CAL_TYPE */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /* FTM_RF_DEBUG_DEVICE_CAL_IDC1_STAUTS */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /* FTM_RF_DEBUG_DEVICE_CAL_IDC2_STATUS */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /* FTM_RF_DEBUG_DEVICE_CAL_IDC1_DATA */
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,  /* FTM_RF_DEBUG_DEVICE_CAL_IDC2_DATA */
};

COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_device_cal_prop_attrib_list) == FTM_RF_DEBUG_DEVICE_CAL_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM RF DEBUG DEVICE CAL Properties
	used to validate whether properties blob type or not in the response packet
*/
const ftm_rf_debug_device_cal_property_info_t ftm_rf_debug_device_cal_prop_info_list[] = 
{
  { FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK, FALSE },   /* FTM_RF_DEBUG_DEVICE_CAL_UNASSIGNED */
  { FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK, FALSE },       /* FTM_RF_DEBUG_DEVICE_CAL_TYPE */
  { FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK, FALSE },   /* FTM_RF_DEBUG_DEVICE_CAL_IDC1_STAUTS */
  { FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK, FALSE },   /* FTM_RF_DEBUG_DEVICE_CAL_IDC2_STATUS */
  { FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK, TRUE },    /* FTM_RF_DEBUG_DEVICE_CAL_IDC1_DATA */
  { FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK, TRUE },    /* FTM_RF_DEBUG_DEVICE_CAL_IDC2_DATA */
};

COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_device_cal_prop_info_list) == FTM_RF_DEBUG_DEVICE_CAL_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM RF DEBUG DCOC Override Properties
	used to validate whether critical properties are present in the incoming packet
*/
const uint8 ftm_rf_debug_dcoc_override_prop_attrib_list[] = 
{
  FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK,     /*FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_UNASSIGNED*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_RFM_DEVICE*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_CC_INDEDX*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_DCOC_I*/
  FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK,         /*FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_DCOC_Q*/

};

COMPILE_ASSERT( ARR_SIZE(ftm_rf_debug_dcoc_override_prop_attrib_list) == FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_NUM);


/*--------------------------------------------------------------------------------------------------------------------*/
/*! 
 
 @brief
  Radio Config Property Name Display Info
*/
const char *rf_debug_radio_config_property_names[] = 
{
  "UNASSIGNED",         /* 0 :  FTM_RF_DEBUG_RADIO_CFG_PROP_UNASSIGNED */
  "IS_TEARDOWN",        /* 1 :  FTM_RF_DEBUG_RADIO_CFG_PROP_IS_TEARDOWN */
  "RADIO_SETUP_TYPE",   /* 2 :  FTM_RF_DEBUG_RADIO_CFG_PROP_RADIO_SETUP_TYPE */
  "RFM_DEVICE",         /* 3 :  FTM_RF_DEBUG_RADIO_CFG_PROP_RFM_DEVICE*/
  "SIG_PATH",           /* 4 :  FTM_RF_DEBUG_RADIO_CFG_PROP_SIG_PATH */
  "ANT_PATH",           /* 5 :  FTM_RF_DEBUG_RADIO_CFG_PROP_ANT_PATH */
  "RFM_PATH_TYPE",      /* 6 :  FTM_RF_DEBUG_RADIO_CFG_PROP_RFM_PATH_TYPE*/
  "BAND",               /* 7 :  FTM_RF_DEBUG_RADIO_CFG_PROP_BAND */
  "SUBBAND",            /* 8 :  FTM_RF_DEBUG_RADIO_CFG_PROP_SUBBAND */
  "RESERVED",           /* 9 :  FTM_RF_DEBUG_RADIO_CFG_PROP_RESERVED */
  "CHANNEL",            /* 10 : FTM_RF_DEBUG_RADIO_CFG_PROP_CHANNEL */
  "WAVEFORM",           /* 11 : FTM_RF_DEBUG_RADIO_CFG_PROP_WAVEFORM*/
  "BANDWIDTH",          /* 12 : FTM_RF_DEBUG_RADIO_CFG_PROP_BANDWIDTH */
  "NUM_RB",             /* 13 : FTM_RF_DEBUG_RADIO_CFG_PROP_NUM_RB */
  "START_RB",           /* 14 : FTM_RF_DEBUG_RADIO_CFG_PROP_START_RB */
  "CW_OFFSET",          /* 15 : FTM_RF_DEBUG_RADIO_CFG_PROP_CW_OFFSET */
  "IS_DC",              /* 16 : FTM_RF_DEBUG_RADIO_CFG_PROP_IS_DC*/
  "MOD_TYPE",           /* 17 : FTM_RF_DEBUG_RADIO_CFG_PROP_MOD_TYPE*/
  "LOOPBACK_TYPE",      /* 18 : FTM_RF_DEBUG_RADIO_CFG_PROP_LOOPBACK_TYPE */
  "BEAM_ID",            /* 19 : FTM_RF_DEBUG_RADIO_CFG_PROP_BEAM_ID */
  "CC_INDEX",           /* 20 : FTM_RF_DEBUG_RADIO_CFG_PROP_CC_INDEX */
  "CC_START_RB",        /* 21 : FTM_RF_DEBUG_RADIO_CFG_PROP_CC_START_RB */
  "CC_NUM_RB",          /* 22 : FTM_RF_DEBUG_RADIO_CFG_PROP_CC_NUM_RB */ 
  "CC_BANDWIDTH",       /* 23 : FTM_RF_DEBUG_RADIO_CFG_PROP_CC_BANDWIDTH*/
  "LOOPBACK_RFM_DEVICE",/* 24 : FTM_RF_DEBUG_RADIO_CFG_PROP_LOOPBACK_RFM_DEVICE*/
  "PLL_ID",             /* 25 : FTM_RF_DEBUG_RADIO_CFG_PROP_PLL_ID*/
  "TUNE_TX_TO_RX_FREQ", /* 26 : FTM_RF_DEBUG_RADIO_CFG_PROP_TUNE_TX_TO_RX_FREQ*/
  "LOAD_CODEBOOK",      /* 27 : FTM_RF_DEBUG_RADIO_CFG_PROP_LOAD_CODEBOOK*/
  "FREQUENCY",          /* 28 : FTM_RF_DEBUG_RADIO_CFG_PROP_FREQUENCY*/
  "VERSION",            /* 29 : FTM_RF_DEBUG_RADIO_CFG_PROP_VERSION*/
  "SWITCH_TDSCDMA_WAVEFORM", /*30:FTM_RF_DEBUG_RADIO_CFG_PROP_SWITCH_TDSCDMA_WAVEFORM*/
  "SET_MOD",             /*31 : FTM_RF_DEBUG_RADIO_CFG_PROP_SET_MOD*/
  "CC_SCS",              /*32 : FTM_RF_DEBUG_RADIO_CFG_PROP_CC_SCS*/
  "WFM_ORIGIN",          /*33 : FTM_RF_DEBUG_RADIO_CFG_PROP_WAVEFORM_ORIGIN*/
  "NDR_STATE",           /*34:FTM_RF_DEBUG_RADIO_CFG_PROP_SET_NDR_STATE*/
  "NR5G_MOD_TYPE",       /*35:FTM_RF_DEBUG_RADIO_CFG_PROP_NR5G_MOD_TYPE*/
  "WFM_IMMED_TRIG",      /*36:FTM_RF_DEBUG_RADIO_CFG_PROP_WAVEFORM_IMMEDIATE_TRIGGER*/
  "VPA_MAX_MV",          /*37:FTM_RF_DEBUG_RADIO_CFG_PROP_VPA_MAX_MV*/
  
};

COMPILE_ASSERT(sizeof(rf_debug_radio_config_property_names)/sizeof(rf_debug_radio_config_property_names[0]) == FTM_RF_DEBUG_RADIO_CFG_PROP_NUM );


/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Rx Override Property Name Display Info
*/
const char *rf_debug_rx_override_property_names[] = 
{
  "UNASSIGNED",           /* FTM_RF_DEBUG_RX_OVERRIDE_PROP_UNASSIGNED */
  "PATH_INDEX",           /* FTM_RF_DEBUG_RX_OVERRIDE_PROP_PATH_INDEX */
  "SIG_PATH",             /* FTM_RF_DEBUG_RX_OVERRIDE_PROP_SIG_PATH */
  "RFM_DEVICE",           /* FTM_RF_DEBUG_RX_OVERRIDE_PROP_RFM_DEVICE */
  "ANTENNA_PORT",         /* FTM_RF_DEBUG_RX_OVERRIDE_PROP_ANTENNA_PORT */
  "EXPECTED_AGC",         /* FTM_RF_DEBUG_RX_OVERRIDE_PROP_EXPECTED_AGC */
  "LNA_GAIN_STATE",       /* FTM_RF_DEBUG_RX_OVERRIDE_PROP_LNA_GAIN_STATE */
  "AGC",                  /* FTM_RF_DEBUG_RX_OVERRIDE_PROP_AGC */
  "NBEE",                 /* FTM_RF_DEBUG_RX_OVERRIDE_PROP_NBEE */
  "SIGNAL_TYPE",          /* FTM_RF_DEBUG_RX_OVERRIDE_PROP_SIGNAL_TYPE */
  "DATA_TYPE",            /*FTM_RF_DEBUG_RX_OVERRIDE_PROP_DATA_TYPE */
  "MEAS_RSSI",            /*FTM_RF_DEBUG_RX_OVERRIDE_PROP_MEAS_RSSI */
  "RX_ENABLE",            /*FTM_RF_DEBUG_RX_OVERRIDE_PROP_RX_ENABLE */
  "PLL_ID",               /*FTM_RF_DEBUG_RX_OVERRIDE_PROP_PLL_ID */
  "RESERVED_1",           /*FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_1*/
  "RESERVED_2",           /*FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_2*/
  "RESERVED_3",           /*FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_3*/
  "RESERVED_4",           /*FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_4*/
  "RESERVED",             /*FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED*/
  "RESERVED_5",           /*FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_5*/
  "RESERVED_6",           /*FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_6*/
  "RESERVED_7",           /*FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_7*/
  "RESERVED_8",           /*FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_8*/
  //"SET_RX_GAIN_STATE_ACTION",  /* 14 FTM_RF_DEBUG_RX_OVERRIDE_PROP_SET_GAIN_STATE_ACTION */
  //"SET_DVGA_OFFSET_ACTION",    /* 15 FTM_RF_DEBUG_RX_OVERRIDE_PROP_SET_DVGA_OFFSET_ACTION */
  //"DVGA_CC_INDEX",             /* 16 FTM_RF_DEBUG_RX_OVERRIDE_PROP_DVGA_CC_INDEX */
  //"DVGA_OFFSET_VALUE"          /* 17 FTM_RF_DEBUG_RX_OVERRIDE_PROP_DVGA_OFFSET_VALUE */
};

COMPILE_ASSERT(sizeof(rf_debug_rx_override_property_names)/sizeof(rf_debug_rx_override_property_names[0]) == FTM_RF_DEBUG_RX_OVERRIDE_PROP_NUM_UPDATED);
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Rx Override Property Name Display Info
*/
const char *rf_debug_command_discovery_property_names[] = 
{
  "UNASSIGNED",           /* FTM_RF_DEBUG_COMMAND_DISCOVERY_PROP_UNASSIGNED */
  "COMMAND_BITMASK_0",      /* FTM_RF_DEBUG_COMMAND_DISCOVERY_PROP_COMMAND_BITMASK_0 */
  "COMMAND_BITMASK_1"       /* FTM_RF_DEBUG_COMMAND_DISCOVERY_PROP_COMMAND_BITMASK_1 */
};

COMPILE_ASSERT(sizeof(rf_debug_command_discovery_property_names)/sizeof(rf_debug_command_discovery_property_names[0]) == FTM_RF_DEBUG_COMMAND_DISCOVERY_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Tx Measure Property Name Display Info
*/
const char *rf_debug_tx_measure_property_names[] = 
{
  "UNASSIGNED",       /* FTM_RF_DEBUG_TX_MEASURE_PROP_UNASSIGNED */
  "PATH_INDEX",       /* FTM_RF_DEBUG_TX_MEASURE_PROP_PATH_INDEX */
  "SIG_PATH",         /* FTM_RF_DEBUG_TX_MEASURE_PROP_SIG_PATH */ 
  "RFM_DEVICE",       /* FTM_RF_DEBUG_TX_MEASURE_PROP_RFM_DEVICE */
  "ANTENNA_PORT",     /* FTM_RF_DEBUG_TX_MEASURE_PROP_ANTENNA_PORT */
  "EXPECTED_PWR",     /* FTM_RF_DEBUG_TX_MEASURE_PROP_EXPECTED_PWR */
  "FBRX_GAIN_STATE",  /* FTM_RF_DEBUG_TX_MEASURE_PROP_FBRX_GAIN_STATE */
  "REF_TXPWR",        /* FTM_RF_DEBUG_TX_MEASURE_PROP_REF_TXPW */
  "MEASURED_PWR",     /* FTM_RF_DEBUG_TX_MEASURE_PROP_MEASURED_PWR */ 
  "EXPECTED_HW_GAIN", /* FTM_RF_DEBUG_TX_MEASURE_PROP_EXPECTED_HW_GAIN */
  "HW_GAIN",          /* FTM_RF_DEBUG_TX_MEASURE_PROP_HW_GAIN */
  "HW_GAIN_DELTA",    /* FTM_RF_DEBUG_TX_MEASURE_PROP_HW_GAIN_DELTA */
  "TRUNC_BIT",        /* FTM_RF_DEBUG_TX_MEASURE_PROP_TRUNC_BIT */
  "LS_ERROR",         /* FTM_RF_DEBUG_TX_MEASURE_PROP_LS_ERROR */
  "PREDET_INDEX",     /* FTM_RF_DEBUG_TX_MEASURE_PROP_PREDET_INDEX */
  "FW_GAIN",          /* FTM_RF_DEBUG_TX_MEASURE_PROP_FW_GAIN */
  "REF_TXPWR_SS",      /* FTM_RF_DEBUG_TX_MEASURE_PROP_SS_REF_TXPW */
  "FBRX_MODE",        /*FTM_RF_DEBUG_TX_MEASURE_PROP_FBRX_MODE */
  "MEASUREMENT_TYPE", /*FTM_RF_DEBUG_TX_MEASURE_PROP_MEASUREMENT_TYPE*/
  "ACLR_UTRA1_POS",   /*FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_UTRA1_POS*/
  "ACLR_UTRA1_NEG",   /*FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_UTRA1_NEG */
  "ACLR_UTRA2_POS",   /*FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_UTRA2_POS  */
  "ACLR_UTRA2_NEG",   /*FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_UTRA2_NEG */
  "ACLR_EUTRA_POS",   /*FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_EUTRA_POS */
  "ACLR_EUTRA_NEG",    /*FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_EUTRA_NEG */
  "IQ_GAIN_ACTION",    /* FTM_RF_DEBUG_TX_MEASURE_PROP_IQ_GAIN_ACTION */      
  "IQ_GAIN",           /* FTM_RF_DEBUG_TX_MEASURE_PROP_IQ_GAIN */   
  "TXFE_GAIN_ACTION",  /* FTM_RF_DEBUG_TX_MEASURE_PROP_TXFE_GAIN_ACTION */ 
  "TXFE_GAIN",         /* FTM_RF_DEBUG_TX_MEASURE_PROP_TXFE_GAIN */
  "ENV_SCALE_ACTION",  /* FTM_RF_DEBUG_TX_MEASURE_PROP_ENV_SCALE_ACTION */
  "ENV_SCALE",         /* FTM_RF_DEBUG_TX_MEASURE_PROP_ENV_SCALE */
  "RGI",               /* FTM_RF_DEBUG_TX_MEASURE_PROP_RGI */
  "PA_BIAS",           /* FTM_RF_DEBUG_TX_MEASURE_PROP_PA_BIAS */
  "PA_STATE",          /* FTM_RF_DEBUG_TX_MEASURE_PROP_PA_STATE */
  "PA_CURRENT",          /* FTM_RF_DEBUG_TX_MEASURE_PROP_PA_CURRENT */
  "DELAY_ACTION",  /* FTM_RF_DEBUG_TX_MEASURE_PROP_DELAY_ACTION */
  "DELAY",         /* FTM_RF_DEBUG_TX_MEASURE_PROP_DELAY_SCALE */
  "PLL_ID",             /*FTM_RF_DEBUG_TX_MEASURE_PROP_PLL_ID */
  "EVM",             /*FTM_RF_DEBUG_TX_MEASURE_PROP_EVM */
  "XPT_MODE",        /*FTM_RF_DEBUG_TX_MEASURE_PROP_XPT_MODE */
  "VSWR_S11_MAG",         /* FTM_RF_DEBUG_TX_MEASURE_PROP_VSWR_S11_MAG */  
  "CONVERGED_IQ_GAIN", /*FTM_RF_DEBUG_TX_MEASURE_PROP_CONVERGED_IQ_GAIN*/
  "ET_VMIN", /* FTM_RF_DEBUG_TX_MEASURE_PROP_ET_VMIN */
};

COMPILE_ASSERT(sizeof(rf_debug_tx_measure_property_names)/sizeof(rf_debug_tx_measure_property_names[0]) == FTM_RF_DEBUG_TX_MEASURE_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Tx Override Property Name Display Info
*/
const char *rf_debug_tx_override_property_names[] = 
{
  "UNASSIGNED",               /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_UNASSIGNED */
  "TX_ENABLE",                /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_TX_ENABLE */
  "PATH_INDEX",               /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_PATH_INDEX */
  "SIG_PATH",                 /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_SIG_PATH */
  "RFM_DEVICE",               /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_RFM_DEVICE */
  "ANT_PATH",                 /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_ANT_PATH */    
  "XPT_MODE",                 /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_XPT_MODE */    
  "IQ_GAIN_ACTION",           /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_IQ_GAIN_ACTION */      
  "IQ_GAIN",                  /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_IQ_GAIN */   
  "TXFE_GAIN_ACTION",         /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_TXFE_GAIN_ACTION */ 
  "TXFE_GAIN",                /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_TXFE_GAIN */
  "ENV_SCALE_ACTION",         /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_ENV_SCALE_ACTION */
  "ENV_SCALE",                /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_ENV_SCALE */
  "RGI",                      /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_RGI */
  "PA_BIAS",                  /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_PA_BIAS */
  "PA_STATE",                 /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_PA_STATE */
  "PA_CURRENT",               /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_PA_CURRENT */
  "DELAY_ACTION",             /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_DELAY_ACTION */    
  "DELAY",                    /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_DELAY */   
  "DATA_TYPE",                /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_DATA_TYPE */
  "SLOT_NUMBER",              /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_SLOT_NUMBER*/ 
  "DATA_SOURCE",              /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_DATA_SOURCE*/
  "MOD_TYPE",                 /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_MOD_TYPE*/
  "ENV_GAIN",                 /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_ENV_GAIN*/
  "BEAM_ID",                  /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_BEAM_ID*/
  "PRE_COMB_IQ_GAIN_ACTION",  /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_PRE_COMB_IQ_GAIN_ACTION*/
  "CARR_IDX",                 /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_CARR_IDX*/
  "PRE_COMB_IQ_GAIN",         /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_PRE_COMB_IQ_GAIN*/
  "RF_TRX_IDX",               /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_RF_TRX_IDX*/
  "PLL_ID",                   /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_PLL_ID*/
  "SPR_ACTION",               /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_SPR_ACTION*/
  "TX_MOD_TYPE",              /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_TX_MOD_TYPE*/
  "MIXER_GAIN",               /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_MIXER_GAIN */
  "PHASE_SHIFTER",            /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_PHASE_SHIFTER */
  "DCOC_I_COMP",              /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_DCOC_I-COMP */
  "DCOC_Q_COMP",              /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_DCOC_Q_COMP */
  "IQ_COMP_GAIN",             /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_IQ_COMP_GAIN */
  "IQ_COMP_CROSS",            /* FTM_RF_DEBUG_TX_OVERRIDE_PROP_IQ_COMP_CROSS */
  "RESERVED",                  /* RESERVED */
  "ET_VMIN"					  /*FTM_RF_DEBUG_TX_OVERRIDE_PROP_ET_VMIN */
};

COMPILE_ASSERT(sizeof(rf_debug_tx_override_property_names)/sizeof(rf_debug_tx_override_property_names[0]) == FTM_RF_DEBUG_TX_OVERRIDE_PROP_NUM);



/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  VDPD Cal Property Name Display Info
*/
const char *rf_debug_vdpd_cal_property_names[] = 
{
  "UNASSIGNED",           /* FTM_RF_DEBUG_VDPD_CAL_PROP_UNASSIGNED */
  "PATH_INDEX",           /* FTM_RF_DEBUG_VDPD_CAL_PROP_PATH_INDEX */
  "SIG_PATH",             /* FTM_RF_DEBUG_VDPD_CAL_PROP_SIG_PATH */ 
  "RFM_DEVICE",           /* FTM_RF_DEBUG_VDPD_CAL_PROP_RFM_DEVICE */ 
  "XPT_MODE",             /* FTM_RF_DEBUG_VDPD_CAL_PROP_XPT_MODE */  
  "SNR",                  /* FTM_RF_DEBUG_VDPD_CAL_PROP_SNR */
  "EPT_COMPRESSION",      /* FTM_RF_DEBUG_VDPD_CAL_PROP_EPT_COMPRESSION */  
  "KERNEL_MASK",          /* FTM_RF_DEBUG_VDPD_CAL_PROP_KERNEL_MASK*/  
  "QFACTOR",              /* FTM_RF_DEBUG_VDPD_CAL_PROP_QFACTOR */
  "NUM_KERNEL_WEIGHTS",   /* FTM_RF_DEBUG_VDPD_CAL_PROP_NUM_KERNEL_WEIGHTS*/
  "KERNEL_WEIGHT_I",      /* FTM_RF_DEBUG_VDPD_CAL_PROP_KERNEL_WEIGHT_I*/
  "KERNEL_WEIGHT_Q",      /* FTM_RF_DEBUG_VDPD_CAL_PROP_KERNEL_WEIGHT_Q */
  "RESERVED",             /* FTM_RF_DEBUG_VDPD_CAL_PROP_RESERVED*/
  "PROC_ONLY",            /* FTM_RF_DEBUG_VDPD_CAL_PROP_PROC_ONLY */
  "PLL_ID",               /* FTM_RF_DEBUG_VDPD_CAL_PROP_PLL_ID */
  "DBL_PRECISION",        /* FTM_RF_DEBUG_VDPD_CAL_PROP_DBL_PRECISION */
  "ACLR_RIGHT",           /* FTM_RF_DEBUG_VDPD_CAL_PROP_ACLR_RIGHT */
  "ACLR_LEFT",            /* FTM_RF_DEBUG_VDPD_CAL_PROP_ACLR_LEFT */
  "ACLR_MET",             /* FTM_RF_DEBUG_VDPD_CAL_PROP_ACLR_MET */
  "SNR_MET",              /* FTM_RF_DEBUG_VDPD_CAL_PROP_SNR_MET */
  "FBRX_PWR_DBM10",       /* FTM_RF_DEBUG_VDPD_CAL_PROP_FBRX_PWR_DBM10 */            
};

COMPILE_ASSERT(sizeof(rf_debug_vdpd_cal_property_names)/sizeof(rf_debug_vdpd_cal_property_names[0]) == FTM_RF_DEBUG_VDPD_CAL_PROP_NUM );

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Load Unity DPD Property Name Display Info
*/
const char *rf_debug_load_unity_dpd_property_names[] = 
{
  "UNASSIGNED",           /* FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_UNASSIGNED */
  "PATH_INDEX",           /* FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_PATH_INDEX */
  "SIG_PATH",             /* FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_SIG_PATH */ 
  "RFM_DEVICE",           /* FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_RFM_DEVICE */ 
  "XPT_MODE",             /* FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_XPT_MODE */
  "ETVD_MAX",             /* FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_ETVD_MAX */
  "ETVD_MIN" ,            /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_ETVD_MIN */
  "DETROUGH" ,            /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_DET*/
  "QPOET_VMIN" ,          /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_QPOET_VMIN */
  "QPOET_VMAX" ,           /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_QPOET_VMAX*/
  "PLL_ID",             /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_PLL_ID */
  "SCALE_DETROUGH",      /*FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_SCALE_DET */

};

COMPILE_ASSERT(sizeof(rf_debug_load_unity_dpd_property_names)/sizeof(rf_debug_load_unity_dpd_property_names[0]) == FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_NUM);
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  VDPD Conversion Property Name Display Info
*/
const char *rf_debug_vdpd_conversion_property_names[] = 
{
  "UNASSIGNED",           /* FTM_RF_DEBUG_VDPD_CONVERSION_PROP_UNASSIGNED */
  "KERNEL_MASK",           /* FTM_RF_DEBUG_VDPD_CONVERSION_PROP_KERNEL_MASK*/  
  "QFACTOR",               /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_QFACTOR */
  "KERNEL_WEIGHT_I",       /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_KERNEL_WEIGHT_I*/
  "KERNEL_WEIGHT_Q",       /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_KERNEL_WEIGHT_Q */
  "NUM_LUT_VALUES",         /*  FTM_RF_DEBUG_VDPD_CONVERSION_PROP_NUM_LUT_VALUES */
  "AMAM",                  /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_AMAM*/
  "AMPM",                   /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_AMPM*/
  "GAIN",                   /*FTM_RF_DEBUG_VDPD_CONVERSION_PROP_GAIN*/
};

COMPILE_ASSERT(sizeof(rf_debug_vdpd_conversion_property_names)/sizeof(rf_debug_vdpd_conversion_property_names[0]) == FTM_RF_DEBUG_VDPD_CONVERSION_PROP_NUM );
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Load DPD Property Name Display Info
*/
const char *rf_debug_load_dpd_property_names[] = 
{
  "UNASSIGNED",           /* FTM_RF_DEBUG_LOAD_DPD_PROP_UNASSIGNED */
  "RFM_DEVICE",           /* FTM_RF_DEBUG_LOAD_DPD_PROP_RFM_DEVICE */ 
  "SIG_PATH",             /* FTM_RF_DEBUG_LOAD_DPD_PROP_SIG_PATH */ 
  "XPT_MODE",             /*FTM_RF_DEBUG_ATTRIB_XPT_MODE */
  "KERNEL_MASK",           /* FTM_RF_DEBUG_LOAD_DPD_PROP_KERNEL_MASK*/  
  "QFACTOR",               /*FTM_RF_DEBUG_LOAD_DPD_PROP_QFACTOR */
  "KERNEL_WEIGHT_I",       /*FTM_RF_DEBUG_LOAD_DPD_PROP_KERNEL_WEIGHT_I*/
  "KERNEL_WEIGHT_Q",       /*FTM_RF_DEBUG_LOAD_DPD_PROP_KERNEL_WEIGHT_Q */
  "PLL_ID",             /*FTM_RF_DEBUG_LOAD_DPD_PROP_PLL_ID */
  "ETVD_MAX",           /*FTM_RF_DEBUG_LOAD_DPD_PROP_ETVD_MAX*/
  "ETVD MIN",           /*FTM_RF_DEBUG_LOAD_DPD_PROP_ETVD_MIN*/
  "DETROUGH",           /*FTM_RF_DEBUG_LOAD_DPD_PROP_DET*/
  "SCALE_DETROUGH",           /*FTM_RF_DEBUG_LOAD_DPD_PROP_SCALE_DET*/
 
};

COMPILE_ASSERT(sizeof(rf_debug_load_dpd_property_names)/sizeof(rf_debug_load_dpd_property_names[0]) == FTM_RF_DEBUG_LOAD_DPD_PROP_NUM );
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Load DPD Property Name Display Info
*/
const char *rf_debug_set_dpd_debug_mode_property_names[] = 
{
  "UNASSIGNED",           /* FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_UNASSIGNED */
  "SIG_PATH",             /* FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_SIG_PATH */ 
  "RFM_DEVICE",           /* FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_RFM_DEVICE */ 
  "DEBUG_MODE",           /*FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_DEBUG_MODE */
  "PLL_ID",             /*FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_PLL_ID */
 
};

COMPILE_ASSERT(sizeof(rf_debug_set_dpd_debug_mode_property_names)/sizeof(rf_debug_set_dpd_debug_mode_property_names[0]) == FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_NUM );
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  IDC Cal Property Name Display Info
*/
const char *rf_debug_idc_cal_property_names[] = 
{
  "UNASSIGNED",           /* FTM_RF_DEBUG_IDC_CAL_PROP_UNASSIGNED */
  "CAL_TYPE",             /* FTM_RF_DEBUG_IDC_CAL_PROP_CAL_TYPE */
  "IQ_CAPTURE",           /* FTM_RF_DEBUG_IDC_CAL_PROP_GET_IQ_CAPTURE */
  "DIS_TX_LOOPBACK",      /* FTM_RF_DEBUG_IDC_CAL_PROP_DISABLE_TX_LOOPBACK */
  "IIP2_TX_OFFSET",		  /* FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_TX_OFFSET */
  "IIP2_I_CODE",	         /* FTM_RF_DEBUG_IDC_CAL_PROP_I_CODE */
  "IIP2_Q_CODE",              /* FTM_RF_DEBUG_IDC_CAL_PROP_Q_CODE */
  "IIP2_IM2_POWER_REF_CODE",  /* FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_IM2_POWER_REF_CODE */
  "IIP2_IM2_BEST_POWER",      /* FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_IM2_BEST_POWER */
  "IIP2_POWER_BEST",          /* FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_POWER_BEST */
  "IIP2_POWER_DELTA",         /* FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_POWER_DELTA */
  "IIP2_THERM_ADC",         /* FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_THERM_ADC */
};

COMPILE_ASSERT(sizeof(rf_debug_idc_cal_property_names)/sizeof(rf_debug_idc_cal_property_names[0]) == FTM_RF_DEBUG_IDC_CAL_PROP_NUM );

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Tech Enter/Exit Property Name Display Info
*/
const char *rf_debug_tech_enter_exit_property_names[] = 
{
  "UNASSIGNED",              /* FTM_RF_DEBUG_TECH_ENTER_EXIT_PROP_UNASSIGNED */
  "MODE",                    /* FTM_RF_DEBUG_TECH_ENTER_EXIT_PROP_MODE */ 
  "ENABLE_COOKBOOK_LOAD",    /* FTM_RF_DEBUG_TECH_ENTER_EXIT_ENABLE_COOKBOOK_LOAD */ 
};

COMPILE_ASSERT(sizeof(rf_debug_tech_enter_exit_property_names)/sizeof(rf_debug_tech_enter_exit_property_names[0]) == FTM_RF_DEBUG_TECH_ENTER_EXIT_PROP_NUM );

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Device Cal Property Name Display Info
*/
const char *rf_debug_device_cal_property_names[] = 
{
  "UNASSIGNED",              /* FTM_RF_DEBUG_DEVICE_CAL_PROP_UNASSIGNED */
  "CAL TYPE",                /* FTM_RF_DEBUG_DEVICE_CAL_PROP_TYPE */ 
  "IDC1 STATUS",             /* FTM_RF_DEBUG_DEVICE_CAL_PROP_IDC1_STATUS */
  "IDC2 STATUS",             /* FTM_RF_DEBUG_DEVICE_CAL_PROP_IDC2_STATUS */
  "IDC1 DATA",               /* FTM_RF_DEBUG_DEVICE_CAL_PROP_IDC1_DATA */
  "IDC2 DATA",               /* FTM_RF_DEBUG_DEVICE_CAL_PROP_IDC2_DATA */
  
};

COMPILE_ASSERT(sizeof(rf_debug_device_cal_property_names)/sizeof(rf_debug_device_cal_property_names[0]) == FTM_RF_DEBUG_DEVICE_CAL_PROP_NUM );

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  DCOC Override Property Name Display Info
*/
const char *rf_debug_dcoc_override_property_names[] = 
{
  "UNASSIGNED",         /* FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_UNASSIGNED */
  "RFM_DEVICE",         /* FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_RFM_DEVICE */ 
  "CC_INDEDX",          /* FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_CC_INDEDX */ 
  "DCOC_I",             /* FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_DCOC_I */
  "DCOC_Q",             /* FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_DCOC_Q */
};

COMPILE_ASSERT(sizeof(rf_debug_dcoc_override_property_names)/sizeof(rf_debug_dcoc_override_property_names[0]) == FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_NUM );
