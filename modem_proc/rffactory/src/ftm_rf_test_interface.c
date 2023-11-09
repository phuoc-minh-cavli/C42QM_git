 /*!
  @file
  ftm_rf_test_interface.c

  @brief
  Common framework to perform radio test in FTM mode
*/

/*======================================================================================================================

  Copyright (c) 2015 - 2022 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

  $Header: //components/rel/rffactory.mpss/1.0/src/ftm_rf_test_interface.c#136 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------------------------------------------
12/21/22   waq     Add new properties in RRA and Radio Config for NV/Bandwidth Availability check-up
04/15/22   yanwa   [FR77441] Add new FTM XPT internal test framework.
03/30/22   pwj     Add new FTM command and properties for Verify Debug Config
03/30/22   yanwa   [FR77647] RFLTE time profiling for FTM automation
03/14/22   yanwa   Add new Radio Config properties for UL Switch
02/14/22   yka     Add new property for UL Tx Switch Type in RRA
12/09/21   adawn   Sleep Control Response Property for command exec time
12/06/21   pwj     Add new properties in RadioConfig and RRA for supporting 8Rx
08/12/21   adawn   Sleep Control Command Support
08/19/21   prabrai Add mmw HPM DPM power mode
04/16/21   dj      Add Input and output Properties to support SRS CS in RRA
02/24/21   dj      Add new FTM RRA property FTM_TRM_RRA_PROP_V2_CARRIER_CONTROL
07/02/20   fxd     add new property for GSM selftest PVT absolute value
05/15/20   pwj     Add a new property type for configuring droop compensation dynamically
05/12/20   hd      Fetching alternate Tx antenna paths during RRA query
05/07/20   hep     Added placeholder for Tx Gain Adjustment (Radio Configure) for GNB
04/08/20   hep     Added placeholder for Waveform Immediate Trigger for GNB
04/07/20   dpk     Add new RRA and Radio Config Properties for NB_ID and LANE_ID
02/27/20   dj      Add new FTM RRA property FTM_TRM_RRA_PROP_V2_MSIM_TUNE_AWAY
12/26/19   dpk     FR60431:Enable UE power class setting for NR
12/14/19   JC      CR2585642: QDTA via QRCT IRAT panel
12/04/19   hep     Merged gNodeB specific RF Test Rx Measure property enums
11/09/19   lzh     FR57112: Multi-tones Estimated Sensitivity
09/06/19   dpk     Sending NS value in Radio Config for Sub6
07/10/19   gz      Adding the Technology property for Radio Config. command
07/01/19   JC      mmW IRAT support
06/06/19   sr      Adding Command Capability Command
06/04/19   ud      Added NDR State for Radio Config
04/18/19   hp      Added Sub Carrier Spacing support
03/20/19   hp      Added Rx Measure property for measurement type
01/14/19   sb      Added power block symbol info for Tx control command
11/01/18   sb      Added MPE Control prop for DFT Distance
10/17/18   sb      Added RX Measure prop types for RSB freq and power dbc
09/10/18   sb      Added TECH_MODE property type in Radio Config
09/06/18   sb      Adding BWP_START_LOC property type for RRA
09/03/18   dpk     Adding FTM Sawless Support to RRA and Radio Config for LTE
07/12/18   sb      Add Radio Config property Bandwidth part location
07/08/18   pvsr    Add Support for Sub Tech V2X
07/02/18   xj      add new property to hold Tx measure detailed error code
05/07/18   sb      Add MPE Control sub command
05/02/18   vs      Add Tx control prop for FR50490
04/03/18   cth     Add I/Q Capture Type to Rx Measure
03/29/18   jps     Add SEM and OBW measurement properties to TX Measure
03/13/18   jps     New RX_MEASURE properties for averaging
02/27/18   cth     Included Sub-Tech prop/value in FTM RF Test Radio Config Command.
01/12/18   kc      [Selftest]Changes for ACLR Noise floor reporting
01/09/18   cth     Included IF Frequency for return prop/value in FTM RF Test Radio Config Command.
11/12/17   xsh     Add vswr lse property for FTM RF SELF TEST
10/17/17   zhw     Added time profiling response support for FTM RF Test command
10/11/17   dbz     Add TX measure prop for FR 44645
10/05/17   pkg     Added UE Power Class support for UTI query in FTM.
08/16/17   cth     Add Beam Id property(s) for Rx Measure and Tx Control (nr5g)
07/30/17   fz      Add property to support RFVFS LTE TDD bursted mode
05/10/17   zhw     Add PLL ID support to FTM RF TEST framework
04/20/17   ld      Add extended ORFS modulation freq offset
04/05/17   zhw     Added support for returning antenna number as part of RRA
08/29/16   dyx     Add expected_agc/override lna state for iq capture
02/08/17   stl     Add support for RX Meas No-tone Est Sensitivity spur dynamic update
01/05/17   stl     Add support for RX Meas No-tone Est Sensitivity update
11/23/16   dbz     Add support for RX Meas No-tone Est Sensitivity
10/18/16   jps     Add support for TX Meas FBRX switchpoint char
09/13/16   ld      Add GSM responses: PVT num passed/requested
09/02/16   ld      Added GSM responses: PVT pass/fail flag, PVT pass ratio, ORFS in dBm, phase error peak
                   Add GSM OOS action, response
08/05/16   jfc     Add support for Rx Measure - Rx gain control type property
08/04/16   jfc     Add override LNA gain state property to Rx Meas
08/01/16   ld      Correct the GSM ORFS modulation and switching definition
07/21/16   ld      Add GSM PVT test support
07/20/16   jfc     Merge PRx and DRx device properties into one (radio cfg)
06/24/16   jfc     Rename rx_mode continuous_mode and add burst_pattern property
06/21/16   jfc     Add wait_trigger subcommand to FTM RF Test
06/02/16   jps     Use DEBUG_DATA property for blob data
05/25/16   jps     TX Measure - Remove Sig Path and Antenna Path properties
05/16/16   chm     Added new property type for rx measure cmd
05/06/16   jfc     Added support for freq adj in radio config
04/28/16   chm     Return the IQ samples used to compute the IQ reports
04/04/16   zhw     Addeds support for New FTM irat Framework
04/01/15   zhw     Initial Release

======================================================================================================================*/

#include "comdef.h"
#include "rflm_defs.h"
#include "ftm_rf_test_interface.h"
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Radio Config Property Name Display Info
*/
const char *radio_config_property_names[] =
{
  "UNASSIGNED",                  /* FTM_RF_TEST_RADIO_CFG_PROP_UNASSIGNED */
  "RX_CARRIER",                  /* FTM_RF_TEST_RADIO_CFG_PROP_RX_CARRIER */
  "TX_CARRIER",                  /* FTM_RF_TEST_RADIO_CFG_PROP_TX_CARRIER */
  "RFM_DEVICE",                  /* FTM_RF_TEST_RADIO_CFG_PROP_RFM_DEVICE */
  "DEPRECATED",                  /* DEPRECATED property. Used to be FTM_RF_TEST_RADIO_CFG_PROP_RFM_DEVICE_DIV */
  "BAND",                        /* FTM_RF_TEST_RADIO_CFG_PROP_BAND */
  "CHANNEL",                     /* FTM_RF_TEST_RADIO_CFG_PROP_CHANNEL */
  "BANDWIDTH",                   /* FTM_RF_TEST_RADIO_CFG_PROP_BANDWIDTH */
  "CONT_MODE",                   /* FTM_RF_TEST_RADIO_CFG_PROP_CONTTINUOUS_MODE */
  "SIG_PATH",                    /* FTM_RF_TEST_RADIO_CFG_PROP_SIG_PATH */
  "ANT_PATH",                    /* FTM_RF_TEST_RADIO_CFG_PROP_ANT_PATH */
  "USER_ADJ",                    /* FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_USER_ADJUST */
  "CENTER_FREQ",                 /* FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_CENTER_FREQ */
  "ENABLE_XO",                   /* FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_ENABLE_XO */
  "TOTAL_ADJ",                   /* FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_TOTAL_ADJUST */
  "BURST_PATTERN",               /* FTM_RF_TEST_RADIO_CFG_BURST_PATTERN */
  "BEAM_ID ",                    /* FTM_RF_TEST_RADIO_CFG_PROP_BEAM_ID */
  "SUB_FRAME_CONFIG",            /* FTM_RF_TEST_RADIO_CFG_PROP_SUB_FRAME_CONFIG */
  "PLL_ID",                      /* FTM_RF_TEST_RADIO_CFG_PLL_ID */
  "TIME_US",                     /* FTM_RF_TEST_RADIO_CFG_PROP_EXEC_TIME_US */
  "INTER_FREQ",                  /* FTM_RF_TEST_RADIO_CFG_PROP_INTER_FREQ */
  "CENTER_FREQ",                 /* FTM_RF_TEST_RADIO_CFG_PROP_CENTER_FREQ */
  "SUB_TECH",                    /* FTM_RF_TEST_RADIO_CFG_PROP_SUB_TECH */
  "BWP_START_LOC",               /* FTM_RF_TEST_RADIO_CFG_PROP_BWP_START_LOCATION */
  "PATH_FILTER_TYPE",            /* FTM_RF_TEST_RADIO_CFG_PROP_PATH_FILTER_TYPE */
  "TECH_MODE",                   /* FTM_RF_TEST_RADIO_CFG_PROP_TECH_MODE */
  "SCS",                         /* FTM_RF_TEST_RADIO_CFG_PROP_SCS */
  "LOAD_CODEBOOK",               /* FTM_RF_TEST_RADIO_CFG_LOAD_CODEBOOK */
  "NDR_STATE",                   /* FTM_RF_TEST_RADIO_CFG_NDR_STATE */
  "TECHNOLOGY",                  /* FTM_RF_TEST_RADIO_CFG_PROP_TECHNOLOGY */
  "NETWORK_SIGNAL",              /* FTM_RF_TEST_RADIO_CFG_PROP_NETWORK_SIGNAL_VAL */
  "RESERVED2",                   /* FTM_RF_TEST_RADIO_CFG_PROP_RESERVED_2 */
  "SELFTEST_TYPE",               /* FTM_RF_TEST_RADIO_CFG_PROP_SELFTEST_TYPE */
  "UE_POWER_CLASS",              /* FTM_RF_TEST_RADIO_CFG_PROP_UE_POWER_CLASS */
  "RESERVED3",                   /* FTM_RF_TEST_RADIO_CFG_PROP_RESERVED3 */
  "NB_ID",                       /* FTM_RF_TEST_RADIO_CFG_PROP_NB_ID */
  "LANE_ID",                     /* FTM_RF_TEST_RADIO_CFG_PROP_LANE_ID */
  "TX_GAIN_ADJUSTMENT",          /* FTM_RF_TEST_RADIO_CFG_PROP_TX_GAIN_ADJUSTMENT */
  "SRS_CS_TYPE",                 /* FTM_RF_TEST_RADIO_CFG_PROP_SRS_CS_TYPE */
  "SRS_SOURCE_CARRIER",          /* FTM_RF_TEST_RADIO_CFG_PROP_SRS_SOURCE_CARRIER */
  "IF_PLL_UNLOCK_STATUS",        /* FTM_RF_TEST_RADIO_CFG_PROP_IF_PLL_UNLOCKED */
  "RF_PLL_UNLOCK_STATUS",        /* FTM_RF_TEST_RADIO_CFG_PROP_RF_PLL_UNLOCKED_MODULE_BITMASK */
  "SUBSCRIPTION_INDEX",          /* FTM_RF_TEST_RADIO_CFG_PROP_SUBCRIPTION_INDEX  */
  "TX_SHARING",                  /* FTM_RF_TEST_RADIO_CFG_PROP_TX_SHARING */
  "TX_PRORITY",                  /* FTM_RF_TEST_RADIO_CFG_PROP_TX_PRIORITY */
  "BWP_ID",                      /* FTM_RF_TEST_RADIO_CFG_BWP_ID */
  "BWP_BW",                      /* FTM_RF_TEST_RADIO_CFG_BWP_BW */
  "TARGET_BWP_ID",               /* FTM_RF_TEST_RADIO_CFG_TARGET_BWP_ID */
  "BWP_CENTER_FREQ",             /* FTM_RF_TEST_RADIO_CFG_BWP_CENTER_KFREQ */
  "BWP_PRIORITY",                /* FTM_RF_TEST_RADIO_CFG_BWP_PRIORITY */
  "BWP_CUSTOM",                  /* FTM_RF_TEST_RADIO_CFG_BWP_CUSTOM */
  "SUB_CFG_ID",                  /* FTM_RF_TEST_RADIO_CFG_SUB_CFG_ID */
  "TARGET_SUB_CFG",              /* FTM_RF_TEST_RADIO_CFG_TARGET_SUB_CFG */
  "ANT_NUM",                     /* FTM_RF_TEST_RADIO_CFG_ANT_NUM */
  "UE_COMBO_POWER_CLASS",        /* FTM_RF_TEST_RADIO_CFG_PROP_UE_COMBO_POWER_CLASS */
  "UL_TX_SWITCH_TYPE",           /* FTM_RF_TEST_RADIO_CFG_PROP_UL_TX_SWITCH_TYPE */
  "TX_SWITCH_SOURCE_CARRIE",     /* FTM_RF_TEST_RADIO_CFG_PROP_TX_SWITCH_SOURCE_CARRIER */
  "TUNE_BUILD_SCRIPT_TIME",       /* FTM_RF_TEST_RADIO_CFG_TUNE_BUILD_SCRIPT_TIME */ 
  "TX_CELL_ID",                   /* FTM_RF_TEST_RADIO_CFG_PROP_TX_CELL_ID */
  "NS_VAL_TYPE",       			 /* FTM_RF_TEST_RADIO_CFG_PROP_NS_VAL_TYPE */
  "NV_BANDWIDTH_AVAILABILITY",    /* FTM_RF_TEST_RADIO_CFG_PROP_NV_BW_AVAILABILITY */  
  "ANT_ISOLATION_CHAR_TUNE_TYPE", /* FTM_RF_TEST_RADIO_CFG_PROP_ANT_ISOLATION_TUNE_TYPE */
  "ANT_ISOLATION_AGGRESSOR_BAND",	/* FTM_RF_TEST_RADIO_CFG_PROP_ANT_ISOLATION_AGGRESSOR_BAND */
  "ANT_ISOLATION_AGGRESSOR_TECH",	/* FTM_RF_TEST_RADIO_CFG_PROP_ANT_ISOLATION_AGGRESSOR_TECH */
  "ANT_ISOLATION_AGGRESSOR_FREQ"	/* FTM_RF_TEST_RADIO_CFG_PROP_ANT_ISOLATION_AGGRESSOR_FREQ */
};

COMPILE_ASSERT( sizeof(radio_config_property_names)/sizeof( radio_config_property_names[0] )== FTM_RF_TEST_RADIO_CFG_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Rx Measure Property Name Display Info
*/
const char *rx_measure_property_names[] =
{
  "UNASSIGNED",           /* FTM_RF_MEASURE_PROP_UNASSIGNED */
  "RX_CARRIER",           /* FTM_RX_MEASURE_PROP_RX_CARRIER */
  "RFM_DEVICE",           /* FTM_RX_MEASURE_PROP_RFM_DEVICE */
  "EXPECTED_AGC",         /* FTM_RX_MEASURE_PROP_EXPECTED_AGC */
  "RX_AGC",               /* FTM_RX_MEASURE_PROP_AGC */
  "LNA_GS",               /* FTM_RX_MEASURE_PROP_LNA_GAIN_STATE */
  "SIG_PATH",             /* FTM_RX_MEASURE_PROP_SIG_PATH */
  "RX_MODE",              /* FTM_RX_MEASURE_PROP_RX_MODE */
  "RX_SLOT",              /* FTM_RX_MEASURE_PROP_RX_SLOT */
  "NUM_OF_BURST",         /* FTM_RX_MEASURE_PROP_NUM_OF_BURST */
  "SENSITIVITY",          /* FTM_RX_MEASURE_PROP_SENSITIVITY */
  "CTON",                 /* FTM_RX_MEASURE_PROP_CTON */
  "PEAK FREQ",            /* FTM_RX_MEASURE_PROP_PEAK_FREQ */
  "FETCH_IQ",             /* FTM_RX_MEASURE_PROP_FETCH_RAW_IQ_SAMPLES */
  "NUM_OF_SAMPLES",       /* FTM_RX_MEASURE_PROP_NUM_OF_SAMPLES */
  "IQ_DATA_FORMAT",       /* FTM_RX_MEASURE_PROP_IQ_DATA_FORMAT */
  "SAMP_FREQ",            /* FTM_RX_MEASURE_PROP_SAMP_FREQ */
  "OVERRIDE_LNA",         /* FTM_RX_MEASURE_PROP_OVERRIDE_LNA_GAIN_STATE */
  "RX_GAIN_CTL_TYPE",     /* FTM_RX_MEASURE_PROP_RX_GAIN_CONTRL_TYPE */
  "NO_TONE_SENSITIVITY",   /* FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY */
  "NO_TONE_SENSITIVITY_SPUR_NUM",           /* FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_SPUR_NUM */
  "NO_TONE_SENSITIVITY_SPUR_FERQ_OFFSET",   /* FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_SPUR_1_FERQ_OFFSET */
  "NO_TONE_SENSITIVITY_SPUR_LEVEL",         /* FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_SPUR_1_LEVEL */
  "BEAM_ID",              /* FTM_RX_MEASURE_PROP_BEAM_ID */
  "TIME_US",              /* FTM_RX_MEASURE_PROP_EXEC_TIME_US */
  "SUB_FRAME_CONFIG",     /* FTM_RX_MEASURE_PROP_SUB_FRAME_CONFIG */
  "NUM_AVERAGES",         /* FTM_RX_MEASURE_PROP_NUM_AVERAGES */
  "RX_AGC_MIN",           /* FTM_RX_MEASURE_PROP_AGC_MIN */
  "RX_AGC_MAX",           /* FTM_RX_MEASURE_PROP_AGC_MAX */
  "RX_AGC_STDDEV",        /* FTM_RX_MEASURE_PROP_AGC_STDDEV */
  "CTON_MIN",             /* FTM_RX_MEASURE_PROP_CTON_MIN */
  "CTON_MAX",             /* FTM_RX_MEASURE_PROP_CTON_MAX */
  "CTON_STDDEV",          /* FTM_RX_MEASURE_PROP_CTON_STDDEV */
  "SENSITIVITY_MIN",      /* FTM_RX_MEASURE_PROP_SENSITIVITY_MIN */
  "SENSITIVITY_MAX",      /* FTM_RX_MEASURE_PROP_SENSITIVITY_MAX */
  "SENSITIVITY_STDDEV",   /* FTM_RX_MEASURE_PROP_SENSITIVITY_STDDEV */
  "NO_TONE_SENSITIVITY_MIN",                /* FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_MIN */
  "NO_TONE_SENSITIVITY_MAX",                /* FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_MAX */
  "NO_TONE_SENSITIVITY_STDDEV",             /* FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_STDDEV */
  "IQ_CAPTURE_TYPE",      /* FTM_RX_MEASURE_PROP_IQ_CAPTURE_TYPE */
  "MEAS_FREQUENCY_ERROR", /*FTM_RX_MEASURE_PROP_MEAS_FREQUENCY_ERROR*/
  "INPUT_CW_TONE_FREQUENCY", /*FTM_RX_MEASURE_PROP_INPUT_CW_TONE_FREQUENCY*/
  "RX_FREQUENCY_ERROR", /*FTM_RX_MEASURE_PROP_RX_FREQUENCY_ERROR*/
  "APPLY_FREQUENCY_ERROR_TX", /*FTM_RX_MEASURE_PROP_APPLY_FREQUENCY_ERROR_TX*/
  "NUM_OF_CAPTURES",      /* FTM_RX_MEASURE_PROP_NUM_OF_CAPTURES */
  "IQ_SAMPLE_DEBUG_INFO_0", /* FTM_RX_MEASURE_PROP_IQ_SAMPLE_DEBUG_INFO_0 */
  "IQ_SAMPLE_DEBUG_INFO_1", /* FTM_RX_MEASURE_PROP_IQ_SAMPLE_DEBUG_INFO_1 */
  "IQ_SAMPLE_DEBUG_INFO_2", /* FTM_RX_MEASURE_PROP_IQ_SAMPLE_DEBUG_INFO_2 */
  "SUB_TECH",              /* FTM_RX_MEASURE_PROP_SUB_TECH */
  "RSB_FREQ",              /* FTM_RX_MEASURE_PROP_RSB_FREQ */
  "RSB_PWR_DBC",           /* FTM_RX_MEASURE_PROP_RSB_POWER_DBC */
  "RSB_MEAS_INT",          /* FTM_RX_MEASURE_PROP_RSB_MEAS_INTEGRITY */
  "MEAS_TYPE",        /* FTM_RX_MEASURE_PROP_MEAS_TYPE */
  "NUM_OF_TONES",      /* FTM_RX_MEASURE_PROP_NUM_TONES */
  "MULTI_TONES_SENSITIVITY",  /* FTM_RX_MEASURE_PROP_MULTI_TONES_SENSITIVITY */
  "MULTI_TONES_PEAK_FREQ",  /* FTM_RX_MEASURE_PROP_MULTI_TONES_PEAK_FREQ */
  "MULTI_TONES_CTON",  /* FTM_RX_MEASURE_PROP_MULTI_TONES_CTON */
  "WAVE_TYPE", /* FTM_RX_MEASURE_PROP_WAVE_TYPE */
  "RB_OFFSET", /* FTM_RX_MEASURE_PROP_RB_OFFSET */
  "MEAS_BER", /* FTM_RX_MEASURE_PROP_MEAS_BER */
  "XPORT_BLKS_ERR", /* FTM_RX_MEASURE_PROP_XPORT_BLKS_ERR */
  "XPORT_BLKS_TOT", /* FTM_RX_MEASURE_PROP_XPORT_BLKS_TOT */
  "WAVE_ACTION", /*FTM_RX_MEASURE_PROP_WAVE_ACTION */
  "AGC_DBFS", /*FTM_RX_MEASURE_PROP_AGC_DBFS */
  "WVFM_IMMED_TRIGGER", /* FTM_RX_MEASURE_PROP_WAVEFORM_IMMED_TRIGGER */
  "DROOP_COMPENSATION", /*FTM_RX_MEASURE_PROP_DROOP_COMPENSATION */
  "RX_IF_BACKOFF",     /*FTM_RX_MEASURE_PROP_IF_BACKOFF */
  "RX_AGC_OUTER_LOOP", /*FTM_RX_MEASURE_PROP_AGC_OUTER_LOOP */
  "DL_WAVEFORM_TYPE",    /* FTM_RX_MEASURE_PROP_DL_WAVEFORM_TYPE*/
  "WBEE_DBFS10",         /* FTM_RX_MEASURE_PROP_WBEE_DBFS10 */
  "FFTEE_DBFS10",         /* FTM_RX_MEASURE_PROP_FFTEE_DBFS10 */
  "POWER_MODE_TYPE",    /* FTM_RX_MEASURE_POWER_MODE*/  
  "PATH_SWITCH",    /* FTM_RX_MEASURE_PROP_PATH_SWITCH */
  "PATH_STATE",    /* FTM_RX_MEASURE_PROP_PATH_STATE */
  "GAIN_DELTA",    /* FTM_RX_MEASURE_PROP_GAIN_DELTA */
  "LNA_OFFSET",	  /* FTM_RX_MEASURE_PROP_LNA_OFFSET */
  "ANT_ISO_MEAS_VALID"     /*FTM_RX_MEASURE_PROP_ISO_MEAS_VALID*/
};

COMPILE_ASSERT( sizeof(rx_measure_property_names)/sizeof( rx_measure_property_names[0] )== FTM_RX_MEASURE_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Rx Measure Property Critical and blob_type info table
*/
const ftm_rf_test_rx_meas_property_info_t ftm_rftest_rx_meas_prop_info_list[] =
{
  { 0, FALSE },            /*FTM_RF_MEASURE_PROP_UNASSIGNED*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_RX_CARRIER*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_RFM_DEVICE*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_EXPECTED_AGC*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_AGC*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_LNA_GAIN_STATE*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_SIG_PATH*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_RX_MODE*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_RX_SLOT*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_NUM_OF_BURST*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_SENSITIVITY*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_CTON*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_PEAK_FREQ*/
  { 0, TRUE },             /*FTM_RX_MEASURE_PROP_FETCH_RAW_IQ_SAMPLES*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_NUM_OF_SAMPLES*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_IQ_DATA_FORMAT*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_SAMP_FREQ*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_OVERRIDE_LNA_GAIN_STATE*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_RX_GAIN_CONTRL_TYPE*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_SPUR_NUM*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_SPUR_FERQ_OFFSET*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_SPUR_LEVEL*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_BEAM_ID*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_EXEC_TIME_US*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_SUB_FRAME_CONFIG*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_NUM_AVERAGES*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_AGC_MIN*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_AGC_MAX*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_AGC_STDDEV*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_CTON_MIN*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_CTON_MAX*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_CTON_STDDEV*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_SENSITIVITY_MIN*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_SENSITIVITY_MAX*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_SENSITIVITY_STDDEV*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_MIN*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_MAX*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_STDDEV*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_IQ_CAPTURE_TYPE*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_MEAS_FREQUENCY_ERROR*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_INPUT_CW_TONE_FREQUENCY*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_RX_FREQUENCY_ERROR*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_APPLY_FREQUENCY_ERROR_TX*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_NUM_OF_CAPTURES*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_IQ_SAMPLE_DEBUG_INFO_0*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_IQ_SAMPLE_DEBUG_INFO_1*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_IQ_SAMPLE_DEBUG_INFO_2*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_SUB_TECH */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_RSB_FREQ */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_RSB_POWER_DBC */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_RSB_MEAS_INTEGRITY */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_MEAS_TYPE */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_NUM_TONES */
  { 0, TRUE },             /* FTM_RX_MEASURE_PROP_MULTI_TONES_SENSITIVITY */
  { 0, TRUE },             /* FTM_RX_MEASURE_PROP_MULTI_TONES_PEAK_FREQ */
  { 0, TRUE },             /* FTM_RX_MEASURE_PROP_MULTI_TONES_CTON */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_WAVE_TYPE */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_RB_OFFSET */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_MEAS_BER */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_XPORT_BLKS_ERR */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_XPORT_BLKS_TOT */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_WAVE_ACTION */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_AGC_DBFS */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_WAVEFORM_IMMED_TRIGGER*/
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_DROOP_COMPENSATION */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_IF_BACKOFF */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_AGC_OUTER_LOOP */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_DL_WAVEFORM_TYPE */
  { 0, FALSE },            /* FTM_RX_MEASURE_PROP_WBEE_DBFS10 */
  { 0, FALSE },            /* FTM_RX_MEASURE_PROP_FFTEE_DBFS10 */
  { 0, FALSE },            /*FTM_RX_MEASURE_POWER_MODE */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_PATH_SWITCH */
  { 0, FALSE },            /*FTM_RX_MEASURE_PROP_PATH_STATE */
  { 0, FALSE },             /*FTM_RX_MEASURE_PROP_GAIN_DELTA */
  { 0, FALSE },			   /* FTM_RX_MEASURE_PROP_LNA_OFFSET */
  { 0, FALSE }             /*FTM_RX_MEASURE_PROP_ISO_MEAS_VALID*/
};

COMPILE_ASSERT( ARR_SIZE(ftm_rftest_rx_meas_prop_info_list)== FTM_RX_MEASURE_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Tx Control Property Name Display Info
*/
const char *tx_control_property_names[] =
{
  "UNASSIGNED",           /* FTM_TX_CONTROL_PROP_UNASSIGNED */
  "TX_CARRIER",           /* FTM_TX_CONTROL_PROP_TX_CARRIER */
  "RFM_DEVICE",           /* FTM_TX_CONTROL_PROP_RFM_DEVICE */
  "TX_ACTION",            /* FTM_TX_CONTROL_PROP_TX_ACTION */
  "TX_POWER",             /* FTM_TX_CONTROL_PROP_TX_POWER */
  "RB_CONFIG",            /* FTM_TX_CONTROL_PROP_RB_CONFIG */
  "SIG_PATH",             /* FTM_TX_CONTROL_PROP_SIG_PATH */
  "NUM_OF_RB",            /* FTM_TX_CONTROL_PROP_NUM_OF_RB */
  "TX_WAVEFORM",          /* FTM_TX_CONTROL_PROP_TX_WAVEFORM */
  "NETWORK_SIGNAL",       /* FTM_TX_CONTROL_PROP_NETWORK_SIGNAL */
  "TX_SLOT",              /* FTM_TX_CONTROL_PROP_TX_SLOT */
  "MODULATION_TYPE",      /* FTM_TX_CONTROL_PROP_MODULATION_TYPE */
  "TX_ON_DURATION",       /* FTM_TX_CONTROL_PROP_TX_ON_DURATION */
  "UL_BURST_PATTRN",      /* FTM_TX_CONTROL_PROP_UL_BURST_PATTERN */
  "BEAM_ID",              /* FTM_TX_CONTROL_PROP_BEAM_ID */
  "UE_POWER_CLASS",       /* FTM_TX_CONTROL_PROP_OVERRIDE_UE_POWER_CLASS */
  "TIME_US",              /* FTM_TX_CONTROL_PROP_TIME_US */
  "SUB_FRAME_CONFIG",     /* FTM_TX_CONTROL_PROP_SUB_FRAME_CONFIG */
  "TX_POWER_SWEEP_STOP",  /* FTM_TX_CONTROL_PROP_TX_POWER_SWEEP_STOP */
  "TX_POWER_SWEEP_STEP",  /* FTM_TX_CONTROL_PROP_TX_POWER_SWEEP_STEP */
  "SUB_TECH",             /* FTM_TX_CONTROL_PROP_SUB_TECH */
  "NUM_POWER_BLOCK_TR",   /* FTM_TX_CONTROL_PROP_NUM_POWER_BLOCK_TRANSITIONS */
  "TX_PWR_PER_PWR_BLK",   /* FTM_TX_CONTROL_PROP_TX_POWER_PER_POWER_BLOCK */
  "START_SYMBOL",         /* FTM_TX_CONTROL_PROP_START_SYMBOL */
  "STOP_SYMBOL",          /* FTM_TX_CONTROL_PROP_STOP_SYMBOL */
  "TIMING_ADVANCE",       /* FTM_TX_CONTROL_PROP_TIMING_ADVANCE_NANOSECONDS */
  "RESERVED",             /* RESERVED */
  "RESERVED",             /* RESERVED */
  "RESERVED1",            /*FTM_TX_CONTROL_PROP_RESERVED1*/
  "WAVEFORM_IMMEDIATE_TRIGGER", /* FTM_TX_CONTROL_PROP_WAVEFORM_IMMED_TRIGGER */
  "MIMO_ENABLED", /*FTM_TX_CONTROL_PROP_MIMO_ENABLED*/
  "IS_RA_TYPE_0",          /*FTM_TX_CONTROL_PROP_IS_RA_TYPE_0*/
  "IF_BACKOFF", /*FTM_TX_CONTROL_IF_BACKOFF */
  "C1_RATYPE0_START_RB", /*Cluster1_Ra0_START_RB*/
  "C1_RATYPE0_NRB_ALLOC",/*Cluster1_Ra0_NRB_ALLOC*/
  "C2_RATYPE0_START_RB", /*Cluster2_Ra0_START_RB*/
  "C2_RATYPE0_NRB_ALLOC",/*Cluster2_Ra0_NRB_ALLOC (37)*/
  "DPD_ITERATIONS",      /* FTM_TX_CONTROL_PROP_DPD_ITERATIONS */
  "MTPL"                 /* FTM_TX_CONTROL_PROP_MTPL */
};

COMPILE_ASSERT( sizeof(tx_control_property_names)/sizeof( tx_control_property_names[0] )== FTM_TX_CONTROL_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  MPE control blob_type info table
*/
const ftm_rftest_mpe_control_prop_info_t ftm_rftest_mpe_control_prop_info_list[] =
{
  { FALSE },               /* FTM_MPE_CONTROL_PROP_UNASSIGNED */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_MPE_STATE */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_BEAM_ID_A */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_BEAM_ID_B */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE1_I_H_OS */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE1_I_V_OS */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE1_Q_H_OS */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE1_Q_V_OS */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE1_DETECTION */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE2_MUSIC_DISTANCE */
  { TRUE },                /* FTM_MPE_CONTROL_PROP_ACTION_GET_TYPE2_IQ_DATA */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE2_DFT_DISTANCE */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE1_I_K_OS */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE1_Q_K_OS */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE1_SCENARIO */
  { TRUE },                /* FTM_MPE_CONTROL_PROP_TYPE2_MUSIC_SPECTRUM */
  { TRUE },                /* FTM_MPE_CONTROL_PROP_TYPE2_DFT_SPECTRUM */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE2_FAR_FIELD_CANC_DEPTH */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE2_DFT_SNR */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE1_ACTIVE_COVER */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE1_COVER_DET_MODE */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE1_BEAM_GROUP_ID */
  { TRUE },                /* FTM_MPE_CONTROL_PROP_TYPE2_WIN_DFT_SPECTRUM */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE2_MC_CANC_WEIGHTS */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE2_TRANSIENT_FLAG */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_TYPE2_MC_CANC_WEIGHT_Q */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_MPE_GROUP_BEAM_SET_INDEX */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_PLATFORM_TYPE */
  { TRUE },                /* FTM_MPE_CONTROL_PROP_TYPE2_IQ_SAMPLES */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_WIN_DFT_PEAK */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_WIN_DFT_SNR */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_DFT_THRESHOLD */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_NOISE_EST */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_PHASE_BAD_CAPTURE */
  { TRUE },                /* FTM_MPE_CONTROL_PROP_DFT_FREQ_DEPENDENT_THRESHOLD */
  { TRUE },                /* FTM_MPE_CONTROL_PROP_ACCUM_DFT_LINEAR_H */
  { TRUE },                /* FTM_MPE_CONTROL_PROP_ACCUM_DFT_LINEAR_V*/
  { FALSE },               /* FTM_MPE_CONTROL_PROP_WIN_DFT_DISTANCE*/
  { FALSE },               /* FTM_MPE_CONTROL_PROP_DFT_DUAL_THRESHOLD_OFFSET */
  { TRUE },                /* FTM_MPE_CONTROL_PROP_NOISE_EST_VEC_OUT */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_NOISE_BINS_AVG_SIZE */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_NOISE_EST_MODE */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_DUAL_LAYER_MODE */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_LOG_LIKELYHOOD_RATIO */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_JAMMER_DETECTION */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_NF_DETECTION */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_NF_RAW_ROT */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_NF_INST_ROT */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_NF_FILTER_ROT */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_NF_ROT_ADAPT_IIR_ALPHA */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_NF_ROT_BIAS_EST */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_NF_FILTER_ROT_VAR */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_NF_XLAYER_NORM */
  { FALSE },               /* FTM_MPE_CONTROL_PROP_FW_RESET */
  { FALSE }                /* FTM_MPE_CONTROL_PROP_INTERNAL_RESP */
};
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  MPE Control Property Name Display Info
*/
const char *mpe_control_property_names[] =
{
  "UNASSIGNED",                 /* FTM_MPE_CONTROL_PROP_UNASSIGNED */
  "MPE_STATE",                  /* FTM_MPE_CONTROL_PROP_MPE_STATE */
  "BEAM_ID_PAIR_A",             /* FTM_MPE_CONTROL_PROP_BEAM_ID_A */
  "BEAM_ID_PAIR_B",             /* FTM_MPE_CONTROL_PROP_BEAM_ID_B */
  "TYPE1_CHAR_I_H",             /* FTM_MPE_CONTROL_PROP_TYPE1_I_H_OS */
  "TYPE1_CHAR_I_V",             /* FTM_MPE_CONTROL_PROP_TYPE1_I_V_OS */
  "TYPE1_CHAR_Q_H",             /* FTM_MPE_CONTROL_PROP_TYPE1_Q_H_OS */
  "TYPE1_CHAR_Q_V",             /* FTM_MPE_CONTROL_PROP_TYPE1_Q_V_OS */
  "TYPE1_DETECTION",            /* FTM_MPE_CONTROL_PROP_TYPE1_DETECTION */
  "TYPE2_M_DISTANCE",           /* FTM_MPE_CONTROL_PROP_TYPE2_MUSIC_DISTANCE */
  "ACTION_GET_TYPE2_DATA",      /* FTM_MPE_CONTROL_PROP_ACTION_GET_TYPE2_IQ_DATA */
  "TYPE2_D_DISTANCE",           /* FTM_MPE_CONTROL_PROP_TYPE2_DFT_DISTANCE */
  "TYPE1_CHAR_I_K",             /* FTM_MPE_CONTROL_PROP_TYPE1_I_K_OS */
  "TYPE1_CHAR_Q_K",             /* FTM_MPE_CONTROL_PROP_TYPE1_Q_K_OS */
  "TYPE1_SCENARIO",             /* FTM_MPE_CONTROL_PROP_TYPE1_Q_SCENARIO */
  "TYPE2_MUSIC_SPECTRUM",       /* FTM_MPE_CONTROL_PROP_TYPE2_MUSIC_SPECTRUM */
  "TYPE2_DFT_SPECTRUM",         /* FTM_MPE_CONTROL_PROP_TYPE2_DFT_SPECTRUM */
  "TYPE2_FAR_FIELD_CANC_DEPTH", /* FTM_MPE_CONTROL_PROP_TYPE2_FAR_FIELD_CANC_DEPTH */
  "TYPE2_DFT_SNR",              /* FTM_MPE_CONTROL_PROP_TYPE2_DFT_SNR */
  "TYPE1_ACTIVE_COVER",         /* FTM_MPE_CONTROL_PROP_TYPE1_ACTIVE_COVER */
  "TYPE1_COVER_DET_MODE",       /* FTM_MPE_CONTROL_PROP_TYPE1_COVER_DET_MODE */
  "BEAM_GROUP_ID",              /* FTM_MPE_CONTROL_PROP_TYPE1_BEAM_GROUP_ID */
  "TYPE2_WIN_DFT_SPECTRUM",     /* FTM_MPE_CONTROL_PROP_TYPE2_WIN_DFT_SPECTRUM */
  "TYPE2_MC_CANC_WEIGHT_I",     /* FTM_MPE_CONTROL_PROP_TYPE2_MC_CANC_WEIGHTS */
  "TYPE2_NEAR_FIELD_DETECTION", /* FTM_MPE_CONTROL_PROP_TYPE2_TRANSIENT_FLAG */
  "TYPE2_MC_CANC_WEIGHT_Q",     /* FTM_MPE_CONTROL_PROP_TYPE2_MC_CANC_WEIGHT_Q */
  "MPE_GROUP_BEAM_SET_INDEX",   /* FTM_MPE_CONTROL_PROP_MPE_GROUP_BEAM_SET_INDEX */
  "PLATFORM_TYPE",              /* FTM_MPE_CONTROL_PROP_PLATFORM_TYPE */
  "IQ_SAMPLES",                 /* FTM_MPE_CONTROL_PROP_TYPE2_IQ_SAMPLES */
  "WIN_DFT_PEAK",               /* FTM_MPE_CONTROL_PROP_WIN_DFT_PEAK */
  "WIN_DFT_SNR",                /* FTM_MPE_CONTROL_PROP_WIN_DFT_SNR */
  "DFT_THRESHOLD",              /* FTM_MPE_CONTROL_PROP_DFT_THRESHOLD */
  "NOISE_EST",                  /* FTM_MPE_CONTROL_PROP_NOISE_EST */
  "PHASE_BAD_CAPTURE",          /* FTM_MPE_CONTROL_PROP_PHASE_BAD_CAPTURE */
  "DFT_FREQ_THRESHOLD",         /* FTM_MPE_CONTROL_PROP_DFT_FREQ_DEPENDENT_THRESHOLD */
  "ACCUM_DFT_LINEAR_H",         /* FTM_MPE_CONTROL_PROP_ACCUM_DFT_LINEAR_H */
  "ACCUM_DFT_LINEAR_V",         /* FTM_MPE_CONTROL_PROP_ACCUM_DFT_LINEAR_V */
  "WIN_DFT_DISTANCE",           /* FTM_MPE_CONTROL_PROP_WIN_DFT_DISTANCE */
  "DFT_THRESHOLD",              /* FTM_MPE_CONTROL_PROP_DFT_DUAL_THRESHOLD_OFFSET */
  "NOISE_EST_VEC_OUT",          /* FTM_MPE_CONTROL_PROP_NOISE_EST_VEC_OUT */
  "NOISE_BINS_AVG_SIZE",        /* FTM_MPE_CONTROL_PROP_NOISE_BINS_AVG_SIZE */
  "NOISE_EST_MODE",             /* FTM_MPE_CONTROL_PROP_NOISE_EST_MODE */
  "DUAL_LAYER_MODE",            /* FTM_MPE_CONTROL_PROP_DUAL_LAYER_MODE */
  "LOG_LIKELY_RATIO",           /* FTM_MPE_CONTROL_PROP_LOG_LIKELYHOOD_RATIO */
  "JAMMER_DETECTION",           /* FTM_MPE_CONTROL_PROP_JAMMER_DETECTION */
  "NF_DETECTION",               /* FTM_MPE_CONTROL_PROP_NF_DETECTION */
  "NF_FD_CANC_DEPTH",           /* FTM_MPE_CONTROL_PROP_NF_FD_CANC_DEPTH */
  "NF_RAW_ROT",                 /* FTM_MPE_CONTROL_PROP_NF_RAW_ROT */
  "NF_INST_ROT",                /* FTM_MPE_CONTROL_PROP_NF_INST_ROT */
  "NF_FILTER_ROT",              /* FTM_MPE_CONTROL_PROP_NF_FILTER_ROT */
  "NF_ROT_ADAPT_IIR_ALPHA",     /* FTM_MPE_CONTROL_PROP_NF_ROT_ADAPT_IIR_ALPHA */
  "NF_ROT_BIAS_EST",            /* FTM_MPE_CONTROL_PROP_NF_ROT_BIAS_EST */
  "NF_FILTER_ROT_VAR",          /* FTM_MPE_CONTROL_PROP_NF_FILTER_ROT_VAR */
  "NF_XLAYER_NORM",             /* FTM_MPE_CONTROL_PROP_NF_XLAYER_NORM */
  "FW_RESET",                   /* FTM_MPE_CONTROL_PROP_NF_FW_RESET*/
  "INTERNAL_RESP"               /* FTM_MPE_CONTROL_PROP_INTERNAL_RESP*/
};

COMPILE_ASSERT( sizeof(mpe_control_property_names)/sizeof( mpe_control_property_names[0] )== FTM_MPE_CONTROL_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  MSIM Config Property Name Display Info
*/
const char *msim_config_property_names[] =
{
  "UNASSIGNED",           /* FTM_MSIM_CONFIG_PROP_UNASSIGNED */
  "SUB",                  /* FTM_MSIM_CONFIG_PROP_SUBSCRIBER */
  "TECH",                 /* FTM_MSIM_CONFIG_PROP_TECH */
  "SCENARIO",             /* FTM_MSIM_CONFIG_PROP_SCENARIO */
};

COMPILE_ASSERT( sizeof(msim_config_property_names)/sizeof( msim_config_property_names[0] )== FTM_MSIM_CONFIG_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  IQ Capture Property Name Display Info
*/
const char *iq_capture_property_names[] =
{
  "UNASSIGNED",           /* FTM_IQ_CAPTURE_PROP_UNASSIGNED */
  "SUBSCRIBER",           /* FTM_IQ_CAPTURE_PROP_SUBSCRIBER */
  "TECHNOLOGY",           /* FTM_IQ_CAPTURE_PROP_TECHNOLOGY */
  "TX_CARRIER",           /* FTM_IQ_CAPTURE_PROP_TX_CARRIER */
  "RX_CARRIER",           /* FTM_IQ_CAPTURE_PROP_RX_CARRIER */
  "RFM_DEVICE",           /* FTM_IQ_CAPTURE_PROP_RFM_DEVICE */
  "SIG_PATH",             /* FTM_IQ_CAPTURE_PROP_SIG_PATH */
  "ACTION_GETCFG",        /* FTM_IQ_CAPTURE_PROP_ACTION_GET_CFG */
  "ACTION_ACQUIRE",       /* FTM_IQ_CAPTURE_PROP_ACTION_ACQUIRE */
  "ACTION_FETCH",         /* FTM_IQ_CAPTURE_PROP_ACTION_FETCH */
  "ACTION_EST_SENS",      /* FTM_IQ_CAPTURE_PROP_ACTION_EST_SENS */
  "ACTION_CTON",          /* FTM_IQ_CAPTURE_PROP_ACTION_CTON */
  "ACTION_PEAK_FREQ",     /* FTM_IQ_CAPTURE_PROP_ACTION_PEAK_FREQ */
  "IQ_SOURCE",            /* FTM_IQ_CAPTURE_PROP_IQ_CAPTURE_SOURCE */
  "SAMP_SIZE",            /* FTM_IQ_CAPTURE_PROP_SAMPLE_SIZE */
  "DATA_FORMAT",          /* FTM_IQ_CAPTURE_PROP_IQ_DATA_FORMAT */
  "SAMP_FREQ",            /* FTM_IQ_CAPTURE_PROP_SAMP_FREQ */
  "MAX_DIAG_SIZE",        /* FTM_IQ_CAPTURE_PROP_MAX_DIAG_SIZE */
  "SAMP_OFFSET",          /* FTM_IQ_CAPTURE_PROP_SAMPLE_OFFSET */
  "NUM_SAMP_BYTES",       /* FTM_IQ_CAPTURE_PROP_NUM_SAMPLE_BYTES */
  "EST_SENS",             /* FTM_IQ_CAPTURE_PROP_EST_SENSITIVITY */
  "CTON",                 /* FTM_IQ_CAPTURE_PROP_CTON */
  "PEAK_FREQ",            /* FTM_IQ_CAPTURE_PROP_CW_PEAK_FREQ */
  "EXPECTED_AGC ",        /*FTM_IQ_CAPTURE_PROP_EXPECTED_AGC*/
  "LNA_GAIN_STATE",       /*FTM_IQ_CAPTURE_PROP_LNA_GAIN_STATE  */
  "OVERRIDE_LNA_GAIN_STATE", /*FTM_IQ_CAPTURE_PROP_OVERRIDE_LNA_GAIN_STAT  */
  "GAIN_CONTRL",             /*FTM_IQ_CAPTURE_PROP_RX_GAIN_CONTRL_TYPE   */
  "SUB_TECH",             /* FTM_IQ_CAPTURE_PROP_SUB_TECH */
};

COMPILE_ASSERT( sizeof(iq_capture_property_names)/sizeof( iq_capture_property_names[0] )== FTM_IQ_CAPTURE_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  TX Measure Property Name Display Info
*/
const char *tx_measure_property_names[] =
{
  "UNASSIGNED",           /* FTM_IQ_CAPTURE_PROP_UNASSIGNED */
  "NUM_AVERAGES",           /* FTM_IQ_CAPTURE_PROP_SUBSCRIBER */
  "TX_CARRIER",           /* FTM_IQ_CAPTURE_PROP_TECHNOLOGY */
  "TX_DEVICE",           /* FTM_IQ_CAPTURE_PROP_TX_CARRIER */
  "ACTION_TX_POWER",           /* FTM_IQ_CAPTURE_PROP_RX_CARRIER */
  "ACTION_EVM",           /* FTM_IQ_CAPTURE_PROP_RFM_DEVICE */
  "ACTION_ACLR",             /* FTM_IQ_CAPTURE_PROP_SIG_PATH */
  "ACTION_VSWR",        /* FTM_IQ_CAPTURE_PROP_ACTION_GET_CFG */
  "ACTION_DEBUG_DATA",       /* FTM_IQ_CAPTURE_PROP_ACTION_ACQUIRE */
  "ACTION_PHASE_ERROR",         /* FTM_IQ_CAPTURE_PROP_ACTION_FETCH */
  "ACTION_SPECTRUM_FLATNESS",      /* FTM_IQ_CAPTURE_PROP_ACTION_EST_SENS */
  "TX_POWER",          /* FTM_IQ_CAPTURE_PROP_ACTION_CTON */
  "EVM",     /* FTM_IQ_CAPTURE_PROP_ACTION_PEAK_FREQ */
  "ACLR_UTRA1_POS",            /* FTM_IQ_CAPTURE_PROP_IQ_CAPTURE_SOURCE */
  "ACLR_UTRA1_NEG",            /* FTM_IQ_CAPTURE_PROP_SAMPLE_SIZE */
  "ACLR_UTRA2_POS",          /* FTM_IQ_CAPTURE_PROP_IQ_DATA_FORMAT */
  "ACLR_UTRA2_NEG",            /* FTM_IQ_CAPTURE_PROP_SAMP_FREQ */
  "ACLR_EUTRA_POS",        /* FTM_IQ_CAPTURE_PROP_MAX_DIAG_SIZE */
  "ACLR_EUTRA_NEG",          /* FTM_IQ_CAPTURE_PROP_SAMPLE_OFFSET */
  "VSWR_RATIO",       /* FTM_IQ_CAPTURE_PROP_NUM_SAMPLE_BYTES */
  "VSWR_RETURN_LOSS",             /* FTM_IQ_CAPTURE_PROP_EST_SENSITIVITY */
  "VSWR_S11_MAG",                 /* FTM_IQ_CAPTURE_PROP_CTON */
  "VSWR_S11_PHASE",            /* FTM_IQ_CAPTURE_PROP_CW_PEAK_FREQ */
  "VSWR_FWD_POWER",
  "VSWR_REV_POWER",
  "SPECTRUM_FLATNESS_R1_RIPPLE",
  "SPECTRUM_FLATNESS_R2_RIPPLE",
  "SPECTRUM_FLATNESS_R12_RIPPLE",
  "SPECTRUM_FLATNESS_R21_RIPPLE",
  "SPECTRUM_FLATNESS_R1_RIPPLE_EXTREME",
  "SPECTRUM_FLATNESS_R2_RIPPLE_EXTREME",
  "SPECTRUM_FLATNESS_R12_RIPPLE_EXTREME",
  "SPECTRUM_FLATNESS_R21_RIPPLE_EXTREME",
  "PHASE_ERROR",
  "ACTION_TX_TOTAL",
  "TOTAL_TX_POWER",
  "DEBUG_DATA",
  "ACTION_ORFS_MODULATION",
  "ACTION_ORFS_SWITCHING",
  "ACTION_PVT",
  "RESERVED1",
  "RESERVED2",
  "TECH",
  "SUBSCRIBER",
  "ORFS_SWITCHING_400KHZ_POS",
  "ORFS_SWITCHING_400KHZ_NEG",
  "ORFS_SWITCHING_600KHZ_POS",
  "ORFS_SWITCHING_600KHZ_NEG",
  "ORFS_SWITCHING_1200KHZ_POS",
  "ORFS_SWITCHING_1200KHZ_NEG",
  "ORFS_SWITCHING_1800KHZ_POS",
  "ORFS_SWITCHING_1800KHZ_NEG",
  "ORFS_MODULATION_100KHZ_POS",
  "ORFS_MODULATION_100KHZ_NEG",
  "ORFS_MODULATION_200KHZ_POS",
  "ORFS_MODULATION_200KHZ_NEG",
  "ORFS_MODULATION_250KHZ_POS",
  "ORFS_MODULATION_250KHZ_NEG",
  "ORFS_MODULATION_400KHZ_POS",
  "ORFS_MODULATION_400KHZ_NEG",
  "ORFS_MODULATION_600KHZ_POS",
  "ORFS_MODULATION_600KHZ_NEG",
  "ORFS_MODULATION_1800KHZ_POS",
  "ORFS_MODULATION_1800KHZ_NEG",
  "PVT_UPPER_MARGIN_0",
  "PVT_UPPER_MARGIN_1",
  "PVT_UPPER_MARGIN_2",
  "PVT_UPPER_MARGIN_3",
  "PVT_UPPER_MARGIN_4",
  "PVT_UPPER_MARGIN_5",
  "PVT_UPPER_MARGIN_6",
  "PVT_UPPER_MARGIN_7",
  "PVT_UPPER_MARGIN_8",
  "PVT_UPPER_MARGIN_9",
  "PVT_UPPER_MARGIN_10",
  "PVT_LOWER_MARGIN_0",
  "PVT_LOWER_MARGIN_1",
  "PVT_LOWER_MARGIN_2",
  "PVT_LOWER_MARGIN_3",
  "PVT_LOWER_MARGIN_4",
  "ORFS_SWITCHING_400KHZ_POS_DBM",
  "ORFS_SWITCHING_400KHZ_NEG_DBM",
  "ORFS_SWITCHING_600KHZ_POS_DBM",
  "ORFS_SWITCHING_600KHZ_NEG_DBM",
  "ORFS_SWITCHING_1200KHZ_POS_DBM",
  "ORFS_SWITCHING_1200KHZ_NEG_DBM",
  "ORFS_SWITCHING_1800KHZ_POS_DBM",
  "ORFS_SWITCHING_1800KHZ_NEG_DBM",
  "ORFS_MODULATION_100KHZ_POS_DBM",
  "ORFS_MODULATION_100KHZ_NEG_DBM",
  "ORFS_MODULATION_200KHZ_POS_DBM",
  "ORFS_MODULATION_200KHZ_NEG_DBM",
  "ORFS_MODULATION_250KHZ_POS_DBM",
  "ORFS_MODULATION_250KHZ_NEG_DBM",
  "ORFS_MODULATION_400KHZ_POS_DBM",
  "ORFS_MODULATION_400KHZ_NEG_DBM",
  "ORFS_MODULATION_600KHZ_POS_DBM",
  "ORFS_MODULATION_600KHZ_NEG_DBM",
  "ORFS_MODULATION_1800KHZ_POS_DBM",
  "ORFS_MODULATION_1800KHZ_NEG_DBM",
  "PVT_PASS_FAIL_FLAG",
  "PVT_PASS_RATIO",
  "PVT_PASS_NUM",
  "PVT_REQUEST_NUM",
  "PHASE_ERROR_PEAK",
  "ACTION_OOS",
  "OOS",
  "ACTION_SWPT_CHAR",
  "ORFS_MODULATION_800KHZ_POS",
  "ORFS_MODULATION_800KHZ_NEG",
  "ORFS_MODULATION_1000KHZ_POS",
  "ORFS_MODULATION_1000KHZ_NEG",
  "ORFS_MODULATION_1200KHZ_POS",
  "ORFS_MODULATION_1200KHZ_NEG",
  "ORFS_MODULATION_1400KHZ_POS",
  "ORFS_MODULATION_1400KHZ_NEG",
  "ORFS_MODULATION_1600KHZ_POS",
  "ORFS_MODULATION_1600KHZ_NEG",
  "ORFS_MODULATION_800KHZ_POS_DBM",
  "ORFS_MODULATION_800KHZ_NEG_DBM",
  "ORFS_MODULATION_1000KHZ_POS_DBM",
  "ORFS_MODULATION_1000KHZ_NEG_DBM",
  "ORFS_MODULATION_1200KHZ_POS_DBM",
  "ORFS_MODULATION_1200KHZ_NEG_DBM",
  "ORFS_MODULATION_1400KHZ_POS_DBM",
  "ORFS_MODULATION_1400KHZ_NEG_DBM",
  "ORFS_MODULATION_1600KHZ_POS_DBM",
  "ORFS_MODULATION_1600KHZ_NEG_DBM",
  "ACTION_TIME",
  "TIME",
  "VSWR_FWD_LSE",
  "REPORT_NOISE_FLOOR",
  "NOISE_FLOOR_ACLR_UTRA1_POS",
  "NOISE_FLOOR_ACLR_UTRA1_NEG",
  "NOISE_FLOOR_ACLR_UTRA2_POS",
  "NOISE_FLOOR_ACLR_UTRA2_NEG",
  "NOISE_FLOOR_ACLR_EUTRA_POS",
  "NOISE_FLOOR_ACLR_EUTRA_NEG",
  "NOISE_FLOOR_ORFS_SWITCHING_400KHZ_POS",
  "NOISE_FLOOR_ORFS_SWITCHING_400KHZ_NEG",
  "NOISE_FLOOR_ORFS_SWITCHING_600KHZ_POS",
  "NOISE_FLOOR_ORFS_SWITCHING_600KHZ_NEG",
  "NOISE_FLOOR_ORFS_SWITCHING_1200KHZ_POS",
  "NOISE_FLOOR_ORFS_SWITCHING_1200KHZ_NEG",
  "NOISE_FLOOR_ORFS_SWITCHING_1800KHZ_POS",
  "NOISE_FLOOR_ORFS_SWITCHING_1800KHZ_NEG",
  "NOISE_FLOOR_ORFS_MODULATION_100KHZ_POS",
  "NOISE_FLOOR_ORFS_MODULATION_100KHZ_NEG",
  "NOISE_FLOOR_ORFS_MODULATION_200KHZ_POS",
  "NOISE_FLOOR_ORFS_MODULATION_200KHZ_NEG",
  "NOISE_FLOOR_ORFS_MODULATION_250KHZ_POS",
  "NOISE_FLOOR_ORFS_MODULATION_250KHZ_NEG",
  "NOISE_FLOOR_ORFS_MODULATION_400KHZ_POS",
  "NOISE_FLOOR_ORFS_MODULATION_400KHZ_NEG",
  "NOISE_FLOOR_ORFS_MODULATION_600KHZ_POS",
  "NOISE_FLOOR_ORFS_MODULATION_600KHZ_NEG",
  "NOISE_FLOOR_ORFS_MODULATION_800KHZ_POS",
  "NOISE_FLOOR_ORFS_MODULATION_800KHZ_NEG",
  "NOISE_FLOOR_ORFS_MODULATION_1000KHZ_POS",
  "NOISE_FLOOR_ORFS_MODULATION_1000KHZ_NEG",
  "NOISE_FLOOR_ORFS_MODULATION_1200KHZ_POS",
  "NOISE_FLOOR_ORFS_MODULATION_1200KHZ_NEG",
  "NOISE_FLOOR_ORFS_MODULATION_1400KHZ_POS",
  "NOISE_FLOOR_ORFS_MODULATION_1400KHZ_NEG",
  "NOISE_FLOOR_ORFS_MODULATION_1600KHZ_POS",
  "NOISE_FLOOR_ORFS_MODULATION_1600KHZ_NEG",
  "NOISE_FLOOR_ORFS_MODULATION_1800KHZ_POS",
  "NOISE_FLOOR_ORFS_MODULATION_1800KHZ_NEG",
  "DELTA_LIMIT_UTRA1_POS",
  "DELTA_LIMIT_UTRA1_NEG",
  "DELTA_LIMIT_UTRA2_POS",
  "DELTA_LIMIT_UTRA2_NEG",
  "DELTA_LIMIT_EUTRA_POS",
  "DELTA_LIMIT_EUTRA_NEG",
  "DELTA_LIMIT_ORFS_M_POS",
  "DELTA_LIMIT_ORFS_M_NEG",
  "DELTA_LIMIT_ORFS_SW_POS",
  "DELTA_LIMIT_ORFS_SW_NEG",
  "ACTION_SEM",
  "SEM_PASS_FAIL",
  "SEM_OFFSET_NUM",
  "SEM_FREQ_OFFSET",
  "SEM_WORST_CASE_PWR",
  "ACTION_OBW",
  "OBW",
  "NUM_OF_CAPTURES",
  "IQ_SAMPLE_DEBUG_INFO_0",
  "IQ_SAMPLE_DEBUG_INFO_1",
  "IQ_SAMPLE_DEBUG_INFO_2",
  "DEBUG_ERROR_CODE",
  "SUB_TECH",
  "MUTUAL_COUPLING_BEAM_ID",
  "MUTUAL_COUPLING_GAIN_STATE",
  "PVT_UPPER_ABS_0",
  "PVT_UPPER_ABS_1",
  "PVT_UPPER_ABS_2",
  "PVT_UPPER_ABS_3",
  "PVT_UPPER_ABS_4",
  "PVT_UPPER_ABS_5",
  "PVT_UPPER_ABS_6",
  "PVT_UPPER_ABS_7",
  "PVT_UPPER_ABS_8",
  "PVT_UPPER_ABS_9",
  "PVT_UPPER_ABS_10",
  "PVT_LOWER_ABS_0",
  "PVT_LOWER_ABS_1",
  "PVT_LOWER_ABS_2",
  "PVT_LOWER_ABS_3",
  "PVT_LOWER_ABS_4",
  "ACTION_CARRIER_LEAKAGE",
  "CARRIER_LEAKAGE",
  "RX_GAIN_STATE_OVERRIDE",
  "ADDITIONAL_ERROR_CODE_SET_1",
  "ACLR_LEFT_GNB_SUB6",
  "ACLR_RIGHT_GNB_SUB6",
  "ACLR_MET_GNB_SUB6",
  "CARRIER_LEAKAGE_TONE_PWR",
  "ACTION_AUTO_PIN_STATUS",
  "AUTO_PIN_STATUS",
  "ACTION_AUTO_DELAY_STATUS",
  "AUTO_DELAY_STATUS",
  "FBRX_GAIN_STATE",
  "ACTION_MEASURE_PDET_POWER",
  "PDET_POWER_RESULTS",
  "ACTION_IF_LOOPBACK_TEST",
  "IF_LOOPBACK_TEST"
};

COMPILE_ASSERT( sizeof(tx_measure_property_names)/sizeof( tx_measure_property_names[0] )== FTM_TX_MEASURE_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Deprecated
*/
/*! DEPRECATION! THE USAGE OF THIS ENUM FAILED TO CONSIDER THE NON-BRANCHING
  RULE OF RFFACTORY, MAKING IT IMPOSSIBLE TO EXTEND THIS LIST. PLEASE USE V2
  FOR ALL FUTUER PURPOSES */
const char *ftm_trm_rra_property_names[] =
{
  "UNASSIGNED",
  "SUB_IDX",
  "TECH",
  "BAND",
  "CHAN",
  "BANDWIDTH",
  "RXTX",
  "CHAIN",
  "CARRIER_IDX",
  "RESOURCE",
  "REASON",
  "PATH_FUNCTIONALITY",
  "DEVICE",
  "SIGNAL_PATH",
  "ANTENNA_PATH",
};
/*! DEPRECATION! THE USAGE OF THIS ENUM FAILED TO CONSIDER THE NON-BRANCHING
  RULE OF RFFACTORY, MAKING IT IMPOSSIBLE TO EXTEND THIS LIST. PLEASE USE V2
  FOR ALL FUTUER PURPOSES */
COMPILE_ASSERT( ARR_SIZE(ftm_trm_rra_property_names)== FTM_TRM_RRA_PROP_NUM);


/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  FTM TRM RRA Property Name Display Info
*/
const char *ftm_trm_rra_property_v2_names[] =
{
  "UNASSIGNED",                     /*FTM_TRM_RRA_PROP_V2_UNASSIGNED*/
  "SUB_IDX",                        /*FTM_TRM_RRA_PROP_V2_SUB_IDX*/
  "TECH",                           /*FTM_TRM_RRA_PROP_V2_TECH*/
  "BAND",                           /*FTM_TRM_RRA_PROP_V2_BAND*/
  "CHAN",                           /*FTM_TRM_RRA_PROP_V2_CHAN*/
  "BANDWIDTH",                      /*FTM_TRM_RRA_PROP_V2_BANDWIDTH*/
  "RXTX",                           /*FTM_TRM_RRA_PROP_V2_RXTX*/
  "CHAIN",                          /*FTM_TRM_RRA_PROP_V2_CHAIN*/
  "CARRIER_IDX",                    /*FTM_TRM_RRA_PROP_V2_CARRIER_IDX*/
  "RESOURCE",                       /*FTM_TRM_RRA_PROP_V2_RESOURCE*/
  "REASON",                         /*FTM_TRM_RRA_PROP_V2_REASON*/
  "PATH_FUNCTIONALITY",             /*FTM_TRM_RRA_PROP_V2_PATH_FUNCTIONALITY*/
  "DEVICE",                         /*FTM_TRM_RRA_PROP_V2_DEVICE*/
  "SIGNAL_PATH",                    /*FTM_TRM_RRA_PROP_V2_SIGNAL_PATH*/
  "ANTENNA_PATH",                   /*FTM_TRM_RRA_PROP_V2_ANTENNA_PATH*/
  "ANTENNA_NUM",                    /*FTM_TRM_RRA_PROP_V2_ANTENNA_NUMBER*/
  "PLL_ID",                         /*FTM_TRM_RRA_PROP_V2_PLL_ID*/
  "TIME_US",                        /*FTM_TRM_RRA_PROP_V2_TIME_US*/
  "SUB_TECH",                       /*FTM_TRM_RRA_PROP_V2_SUB_TECH*/
  "RFM_CONTEXT",                    /*FTM_TRM_RRA_PROP_V2_RFM_CONTEXT*/
  "FILTER_TYPE",                    /*FTM_TRM_RRA_PROP_V2_PATH_FILTER_TYPE*/
  "BWP_START_LOC",                  /*FTM_TRM_RRA_PROP_V2_BWP_START_LOC*/
  "SCS_HZ",                         /*FTM_TRM_RRA_PROP_V2_SCS*/
  "MSIM_TUNE_AWAY",                 /*FTM_TRM_RRA_PROP_V2_MSIM_TUNE_AWAY*/
  "NB_ID",                          /*FTM_TRM_RRA_PROP_V2_NB_ID*/
  "LANE_ID",                        /*FTM_TRM_RRA_PROP_V2_LANE_ID*/
  "RESERVED1",                      /*FTM_TRM_RRA_PROP_V2_RESERVED1*/
  "RESERVED2",                      /*FTM_TRM_RRA_PROP_V2_RESERVED2*/
  "CARRIER_CONTROL",                /*FTM_TRM_RRA_PROP_V2_CARRIER_CONTROL*/
  "SRS_SOURCE_CARRIER",             /*FTM_TRM_RRA_PROP_V2_SRS_SOURCE_CARRIER*/
  "SRS_CS_STYLE",                   /*FTM_TRM_RRA_PROP_V2_SRS_CS_STYLE*/
  "SRS_CS_IMPACTED",                /*FTM_TRM_RRA_PROP_V2_SRS_CS_IMPACTED*/
  "SRS_ANTENNA_NUMBER",             /*FTM_TRM_RRA_PROP_V2_SRS_ANTENNA_NUMBER*/
  "ASDIV_CONFIG_MASK",              /*FTM_TRM_RRA_PROP_V2_ASDIV_CONFIG_MASK*/
  "FETCH_REGROUP_TABLE",            /*FTM_TRM_RRA_PROP_V2_FETCH_REGROUP_TABLE*/
  "REGROUP_COMBO_NUMBER",           /*FTM_TRM_RRA_PROP_V2_REGROUP_COMBO_NUMBER*/
  "ANT_NUMBER_PER_REGROUP_COMBO",   /*FTM_TRM_RRA_PROP_V2_ANT_NUMBER_PER_REGROUP_COMBO*/
  "TX_SWITCH_TYPE",                 /*FTM_TRM_RRA_PROP_V2_TX_SWITCH_TYPE*/
  "NV_BANDWIDTH_AVAILABILITY"       /*FTM_TRM_RRA_PROP_V2_NV_BW_AVAILABILITY*/
};

COMPILE_ASSERT( ARR_SIZE(ftm_trm_rra_property_v2_names)== FTM_TRM_RRA_PROP_V2_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mandatory attribute for FTM TRM RRA Properties
*/

const uint8 ftm_trm_rra_prop_v2_attrib_list[] =
{
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_UNASSIGNED*/
  FTM_TRM_RRA_V2_ATTRIB_CRTCL_BMSK,                /*FTM_TRM_RRA_PROP_V2_SUB_IDX*/
  FTM_TRM_RRA_V2_ATTRIB_CRTCL_BMSK,                /*FTM_TRM_RRA_PROP_V2_TECH*/
  FTM_TRM_RRA_V2_ATTRIB_CRTCL_BMSK,                /*FTM_TRM_RRA_PROP_V2_BAND*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_CHAN*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_BANDWIDTH*/
  FTM_TRM_RRA_V2_ATTRIB_CRTCL_BMSK,                /*FTM_TRM_RRA_PROP_V2_RXTX*/
  FTM_TRM_RRA_V2_ATTRIB_CRTCL_BMSK,                /*FTM_TRM_RRA_PROP_V2_CHAIN*/
  FTM_TRM_RRA_V2_ATTRIB_CRTCL_BMSK,                /*FTM_TRM_RRA_PROP_V2_CARRIER_IDX*/
  FTM_TRM_RRA_V2_ATTRIB_CRTCL_BMSK,                /*FTM_TRM_RRA_PROP_V2_RESOURCE*/
  FTM_TRM_RRA_V2_ATTRIB_CRTCL_BMSK,                /*FTM_TRM_RRA_PROP_V2_REASON*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_PATH_FUNCTIONALITY*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_DEVICE*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_SIGNAL_PATH*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_ANTENNA_PATH*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_ANTENNA_NUMBER*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_PLL_ID*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_TIME_US*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_SUB_TECH*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_RFM_CONTEXT*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_PATH_FILTER_TYPE*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_BWP_START_LOC*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_SCS*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_MSIM_TUNE_AWAY*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_NB_ID*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_LANE_ID*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_RESERVED1*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_RESERVED2*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_CARRIER_CONTROL*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_SRS_SOURCE_CARRIER*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_SRS_CS_STYLE*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_SRS_CS_IMPACTED*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_SRS_ANTENNA_NUMBER*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_ASDIV_CONFIG_MASK*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_FETCH_REGROUP_TABLE*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_REGROUP_COMBO_NUMBER*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_ANT_NUMBER_PER_REGROUP_COMBO*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK,            /*FTM_TRM_RRA_PROP_V2_TX_SWITCH_TYPE*/
  FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK             /*FTM_TRM_RRA_PROP_V2_NV_BW_AVAILABILITY*/
};

COMPILE_ASSERT( ARR_SIZE(ftm_trm_rra_prop_v2_attrib_list)== FTM_TRM_RRA_PROP_V2_NUM);

const uint8 ftm_trm_rra_repack_order_properties[] =
{
  FTM_TRM_RRA_PROP_V2_UNASSIGNED,
  FTM_TRM_RRA_PROP_V2_SUB_IDX,
  FTM_TRM_RRA_PROP_V2_TECH,
  FTM_TRM_RRA_PROP_V2_CARRIER_IDX,
  FTM_TRM_RRA_PROP_V2_BAND,
  FTM_TRM_RRA_PROP_V2_CHAN,
  FTM_TRM_RRA_PROP_V2_BANDWIDTH,
  FTM_TRM_RRA_PROP_V2_RXTX,
  FTM_TRM_RRA_PROP_V2_CHAIN,
  FTM_TRM_RRA_PROP_V2_RESOURCE,
  FTM_TRM_RRA_PROP_V2_REASON,
  FTM_TRM_RRA_PROP_V2_PATH_FUNCTIONALITY,
  FTM_TRM_RRA_PROP_V2_DEVICE,
  FTM_TRM_RRA_PROP_V2_SIGNAL_PATH,
  FTM_TRM_RRA_PROP_V2_ANTENNA_PATH,
  FTM_TRM_RRA_PROP_V2_ANTENNA_NUMBER,
  FTM_TRM_RRA_PROP_V2_PLL_ID,
  FTM_TRM_RRA_PROP_V2_NB_ID,
  FTM_TRM_RRA_PROP_V2_LANE_ID,
  FTM_TRM_RRA_PROP_V2_TIME_US,
  FTM_TRM_RRA_PROP_V2_SUB_TECH,
  FTM_TRM_RRA_PROP_V2_RFM_CONTEXT,
  FTM_TRM_RRA_PROP_V2_PATH_FILTER_TYPE,
  FTM_TRM_RRA_PROP_V2_BWP_START_LOC,
  FTM_TRM_RRA_PROP_V2_SCS,
  FTM_TRM_RRA_PROP_V2_MSIM_TUNE_AWAY,
  FTM_TRM_RRA_PROP_V2_RESERVED1,
  FTM_TRM_RRA_PROP_V2_RESERVED2,
  FTM_TRM_RRA_PROP_V2_CARRIER_CONTROL,
  FTM_TRM_RRA_PROP_V2_SRS_SOURCE_CARRIER,
  FTM_TRM_RRA_PROP_V2_SRS_CS_STYLE,
  FTM_TRM_RRA_PROP_V2_SRS_CS_IMPACTED,
  FTM_TRM_RRA_PROP_V2_SRS_ANTENNA_NUMBER,
  FTM_TRM_RRA_PROP_V2_ASDIV_CONFIG_MASK,
  FTM_TRM_RRA_PROP_V2_FETCH_REGROUP_TABLE,
  FTM_TRM_RRA_PROP_V2_REGROUP_COMBO_NUMBER,
  FTM_TRM_RRA_PROP_V2_ANT_NUMBER_PER_REGROUP_COMBO,
  FTM_TRM_RRA_PROP_V2_TX_SWITCH_TYPE,
  FTM_TRM_RRA_PROP_V2_NV_BW_AVAILABILITY
};

COMPILE_ASSERT( ARR_SIZE(ftm_trm_rra_repack_order_properties)== FTM_TRM_RRA_PROP_V2_NUM);

const char *irat_config_property_names[] =
{
  "UNASSIGNED",           /* FTM_IRAT_CONFIG_PROP_UNASSIGNED */
  "ACTION",               /* FTM_IRAT_CONFIG_PROP_TGT_ACTION */
  "SUB",                  /* FTM_IRAT_CONFIG_PROP_TGT_SUB */
  "TECH",                 /* FTM_IRAT_CONFIG_PROP_TGT_TECH */
  "CARRIER",              /* FTM_IRAT_CONFIG_PROP_TGT_CARRIER */
  "DEVICE",               /* FTM_IRAT_CONFIG_PROP_TGT_DEVICE */
  "SIG_PATH",             /* FTM_IRAT_CONFIG_PROP_TGT_SIG_PATH */
  "ANT_PATH",             /* FTM_IRAT_CONFIG_PROP_TGT_ANT_PATH */
  "BAND",                 /* FTM_IRAT_CONFIG_PROP_TGT_BAND */
  "CHANNEL",              /* FTM_IRAT_CONFIG_PROP_TGT_CHANNEL */
  "BANDWIDTH",            /* FTM_IRAT_CONFIG_PROP_TGT_BANDWIDTH */
  "PLL_ID",               /* FTM_IRAT_CONFIG_PROP_TGT_PLL_ID */
  "BEAM_CNT",             /* FTM_IRAT_CONFIG_PROP_TGT_BEAM_CNT */
  "BEAM_ID",              /* FTM_IRAT_CONFIG_PROP_TGT_BEAM_ID */
  "BEAM_CYCLE",           /* FTM_IRAT_CONFIG_PROP_TGT_BEAM_CYCLE */
    /* HI2.0 QDTA UI */
  "SRC_SUB",              /* FTM_IRAT_CONFIG_PROP_SRC_SUB */
  "SRC_TECH",             /* FTM_IRAT_CONFIG_PROP_SRC_TECH */
  "SRC CC",               /* FTM_IRAT_CONFIG_PROP_SRC_CARRIER */
  "SRC_DEVICE",           /* FTM_IRAT_CONFIG_PROP_SRC_DEVICE */
  "SRC_SIG_PATH",         /* FTM_IRAT_CONFIG_PROP_SRC_SIG_PATH */
  "SRC_ANT_PATH",         /* FTM_IRAT_CONFIG_PROP_SRC_ANT_PATH */
  "SRC_PLL_ID",           /* FTM_IRAT_CONFIG_PROP_SRC_PLL_ID */
  "NB_ID",                /* FTM_IRAT_CONFIG_PROP_TGT_NB_ID */
  "LANE_ID",              /* FTM_IRAT_CONFIG_PROP_TGT_LANE_ID */
  "SRC_NB_ID",            /* FTM_IRAT_CONFIG_PROP_SRC_NB_ID */
  "SRC_LANE_ID",          /* FTM_IRAT_CONFIG_PROP_SRC_LANE_ID */
  "EXP_RXAGC_PWR",        /* FTM_IRAT_CONFIG_PROP_EXPECTED_RXAGC_POWER */
  "TGT_PRX_EE",           /* FTM_IRAT_CONFIG_PROP_TGT_PRX_ENERGY_ESTIMATE */
  "TGT_DRX_EE",           /* FTM_IRAT_CONFIG_PROP_TGT_DRX_ENERGY_ESTIMATE */
  "BUILD_SCRIPT_TIME"     /* FTM_IRAT_CONFIG_PROP_BUILD_SCRIPT_TIME */
};

COMPILE_ASSERT( sizeof(irat_config_property_names)/sizeof( irat_config_property_names[0] )== FTM_IRAT_CONFIG_PROP_NUM);


const char *wait_trigger_property_names[] =
{
  "UNASSIGNED",           /* FTM_WAIT_TRIGGER_PROP_UNASSIGNED */
  "SUB",                  /* FTM_WAIT_TRIGGER_PROP_SUBSCRIBER */
  "TECH",                 /* FTM_WAIT_TRIGGER_PROP_TECHNOLOGY */
  "TX_CARRIER",           /* FTM_WAIT_TRIGGER_PROP_TX_CARRIER */
  "RX_CARRIER",           /* FTM_WAIT_TRIGGER_PROP_RX_CARRIER */
  "DEVICE",               /* FTM_WAIT_TRIGGER_PROP_RFM_DEVICE */
  "THRESHOLD",            /* FTM_WAIT_TRIGGER_PROP_RXWAITDBM_THRESHOLD */
  "DIRECTION",            /* FTM_WAIT_TRIGGER_PROP_RXWAITDBM_DIRECTION */
  "TIMEOUT",              /* FTM_WAIT_TRIGGER_PROP_RXWAITDBM_TIMEOUT */
  "RXAGC",                /* FTM_WAIT_TRIGGER_PROP_RXWAITDBM_RXAGC */
};

COMPILE_ASSERT(sizeof(wait_trigger_property_names)/sizeof(wait_trigger_property_names[0]) == FTM_WAIT_TRIGGER_PROP_NUM);

const char *command_capability_property_names[] =
{
  "UNASSIGNED",           /*FTM_RF_TEST_CMD_CAP_PROP_UNASSIGNED */
  "QUERY_COMMAND",        /*FTM_RF_TEST_CMD_CAP_PROP_QUERY_CMD */
  "QUERY_PROPERTY",       /*FTM_RF_TEST_CMD_CAP_PROP_QUERY_PROP */
  "CMD_MASK",             /*FTM_RF_TEST_CMD_CAP_PROP_RSP_CMD_QUERY*/
  "PROPERTY_MASK_0_63",   /*FTM_RF_TEST_CMD_CAP_PROP_RSP_PROP_QUERY_0_63*/
  "PROPERTY_MASK_64_127", /*FTM_RF_TEST_CMD_CAP_PROP_RSP_PROP_QUERY_64_127*/
  "PROPERTY_MASK_128_191",/*FTM_RF_TEST_CMD_CAP_PROP_RSP_PROP_QUERY_128_191*/
  "PROPERTY_MASK_192_255",/*FTM_RF_TEST_CMD_CAP_PROP_RSP_PROP_QUERY_192_255*/
};

COMPILE_ASSERT(sizeof(command_capability_property_names)/sizeof(command_capability_property_names[0]) == FTM_RF_TEST_CMD_CAP_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Sleep Control Property Name Display Info
*/
const char *sleep_control_property_names[] =
{
  "UNASSIGNED",                    /* FTM_RF_TEST_SLEEP_CONTROL_PROP_UNASSIGNED */
  "SUBSCRIBER",                    /* FTM_RF_TEST_SLEEP_CONTROL_PROP_SUBSCRIBER */
  "TECHNOLOGY",                    /* FTM_RF_TEST_SLEEP_CONTROL_PROP_TECHNOLOGY */
  "SLEEP_WAKEUP_INDICATOR",        /* FTM_RF_TEST_SLEEP_CONTROL_PROP_SLEEP_WAKEUP_INDICATOR */
  "SLEEP_WAKEUP_MODE",             /* FTM_RF_TEST_SLEEP_CONTROL_PROP_SLEEP_WAKEUP_MODE */
  "CARRIER_INDEX",                 /* FTM_RF_TEST_SLEEP_CONTROL_PROP_CARRIER_INDEX */
  "CHAIN",                         /* FTM_RF_TEST_SLEEP_CONTROL_PROP_CHAIN */
  "RX_TX_INDICATOR",               /* FTM_RF_TEST_SLEEP_CONTROL_PROP_RX_TX_INDICATOR */
  "TIME_US",                       /* FTM_RF_TEST_SLEEP_CONTROL_PROP_TIME_US */
  "TECH_SLEEP_WAKEUP_TOTAL_TIME"   /* FTM_RF_TEST_SLEEP_CONTROL_PROP_TECH_SLEEP_WAKEUP_TOTAL_TIME */
};

COMPILE_ASSERT(sizeof(sleep_control_property_names)/sizeof( sleep_control_property_names[0] ) == FTM_RF_TEST_SLEEP_CONTROL_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  RF Verify Debug Config Property Name Display Info
*/

const char *rf_verify_debug_config_property_names[] =
{
  "UNASSIGNED",                    /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_UNASSIGNED */
  "DEBUG_ACTION",                  /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_DEBUG_ACTION_TYPE */
  "CRASH_AFTER_CAPTURE",           /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_CRASH_AFTER_CAPTURE_DONE */
  "CRASH_IF_CAPTURE_TIMEOUT",      /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_CRASH_IF_CAPTURE_TIMEOUT */
  "LSE_MAX_LIMIT",                 /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_LSE_MAX_LIMIT */
  "EVM_MAX_LIMIT",                 /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_EVM_MAX_LIMIT */
  "SF_R1_MAX_LIMIT",               /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SF_R1_MAX_LIMIT */
  "SF_R2_MAX_LIMIT",               /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SF_R2_MAX_LIMIT */
  "SF_R12_MAX_LIMIT",              /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SF_R12_MAX_LIMIT */
  "SF_R21_MAX_LIMIT",              /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SF_R21_MAX_LIMIT */
  "ACLR_UTRA1_MIN_LIMIT",          /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_ACLR_UTRA1_MIN_LIMIT */
  "ACLR_UTRA2_MIN_LIMIT",          /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_ACLR_UTRA2_MIN_LIMIT */
  "ACLR_EUTRA_MIN_LIMIT",          /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_ACLR_EUTRA_MIN_LIMIT */
  "SEM_OFFSET1_MAX_LIMIT",         /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SEM_OFFSET1_MAX_LIMIT */
  "SEM_OFFSET2_MAX_LIMIT",         /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SEM_OFFSET2_MAX_LIMIT */
  "SEM_OFFSET3_MAX_LIMIT",         /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SEM_OFFSET3_MAX_LIMIT */
  "SEM_OFFSET4_MAX_LIMIT",         /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SEM_OFFSET4_MAX_LIMIT */
  "TX_POWER_DELTA",                /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_TX_POWER_LIMIT_DELTA */
  "CARRIER_LEAKAGE_MAX_LIMIT",     /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_CARRIER_LEAKAGE_MAX_LIMIT */
  "GSM_PVT",                       /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_GSM_PVT */
  "GSM_PEAK_PHASE_ERR_MAX_LIMIT",  /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_GSM_PEAK_PHASE_ERROR_MAX_LIMIT */
  "GSM_RMS_PHASE_ERR_MAX_LIMIT",   /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_GSM_RMS_PHASE_ERROR_MAX_LIMIT */
  "ORFS_MOD_LIMIT_DELTA",          /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_GSM_ORFS_MOD_LIMIT_DELTA */
  "ORFS_SW_LIMIT_DELTA",           /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_GSM_ORFS_SW_LIMIT_DELTA */
  "1_TONE_EST_SEN_MAX_LIMIT",      /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_ONE_TONE_EST_SEN_MAX_LIMIT */
  "NO_TONE_EST_SEN_MAX_LIMIT",     /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_NO_TONE_EST_SEN_MAX_LIMIT */
  "DLAFC_LOW_LIMIT",               /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_DLAFC_LOW_LIMIT */
  "DLAFC_UPPER_LIMIT",             /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_DLAFC_UPPER_LIMIT */
  "RXAGC_LOW_DELTA",               /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_RXAGC_LOW_DELTA */
  "RXAGC_UPPER_DELTA",             /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_RXAGC_UPPER_DELTA */
  "VSWR_LOW_LIMIT",                /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_VSWR_LOW_LIMIT */
  "VSWR_UPPER_LIMIT",              /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_VSWR_UPPER_LIMIT */
  "VSWR_LSE_MAX_LIMIT",            /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_VSWR_LSE_MAX_LIMIT */
  "FBRX_PLL_MODE",                 /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_FBRX_PLL_MODE */
  "SAVE_IQ",                       /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SAVE_IQ_DATA */
  "TIME_US"                        /* FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_TIME_US */
};

COMPILE_ASSERT(sizeof(rf_verify_debug_config_property_names)/sizeof( rf_verify_debug_config_property_names[0] ) == FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  XPT Config Property Name Display Info
*/
const char *xpt_config_property_names[] =
{
  "UNASSIGNED",            /* FTM_RF_TEST_XPT_CONFIG_PROP_UNASSIGNED */
  "XPT_MODE",              /* FTM_RF_TEST_XPT_CONFIG_PROP_XPT_MODE */
  "AUTO_PIN_OFFSET",       /* FTM_RF_TEST_XPT_CONFIG_PROP_AUTO_PIN_OFFSET */
  "AUTO_DELAY_OFFSET",     /* FTM_RF_TEST_XPT_CONFIG_PROP_AUTO_DELAY_OFFSET */
  "ONLINE_DPD_OFFSET"      /* FTM_RF_TEST_XPT_CONFIG_PROP_ONLINE_DPD_OFFSET */
};

COMPILE_ASSERT(sizeof(xpt_config_property_names)/sizeof( xpt_config_property_names[0] ) == FTM_RF_TEST_XPT_CONFIG_PROP_NUM);/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  MCS Config Property Name Display Info
*/
const char *mcs_config_property_names[] =
{
  "UNASSIGNED",            /* FTM_RF_TEST_MCS_CONFIG_PROP_UNASSIGNED */
  "COEX_ACTION",           /* FTM_RF_TEST_MCS_CONFIG_PROP_COEX_ACTION */
  "STX_ACTION",            /* FTM_RF_TEST_MCS_CONFIG_PROP_STX_ACTION */
  "COEX_MODE",             /* FTM_RF_TEST_MCS_CONFIG_PROP_COEX_MODE */
  "DESENSE_PRESENT",       /* FTM_RF_TEST_MCS_CONFIG_PROP_DESENSE_PRESENT */
  "SUBSCRIBER",            /* FTM_RF_TEST_MCS_CONFIG_PROP_SUBSCRIBER */
  "TECHNOLOGY",            /* FTM_RF_TEST_MCS_CONFIG_PROP_TECHNOLOGY */
  "CARRIER",               /* FTM_RF_TEST_MCS_CONFIG_PROP_CARRIER */
  "DESENSE_REASON",        /* FTM_RF_TEST_MCS_CONFIG_PROP_DESENSE_REASON */
  "DESENSE_ACTION",        /* FTM_RF_TEST_MCS_CONFIG_PROP_DESENSE_ACTION */
  "COEX_MTPL",             /* FTM_RF_TEST_MCS_CONFIG_PROP_COEX_MTPL */
  "MCC_INFO",              /* FTM_RF_TEST_MCS_CONFIG_PROP_MCC_INFO */
  "BYPASS_DISALLOW_TABLE"  /* FTM_RF_TEST_MCS_CONFIG_PROP_BYPASS_DISALLOW_TABLE */
};

COMPILE_ASSERT(sizeof(mcs_config_property_names)/sizeof( mcs_config_property_names[0] ) == FTM_RF_TEST_MCS_CONFIG_PROP_NUM);