/*============================================================================
  FILE:         VAdcSettings.c

  OVERVIEW:     Settings for VADC.

  DEPENDENCIES: None

                Copyright (c) 2016, 2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Settings/ADC/core/VAdcSettings.c#2 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-03-04  jjo  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalVAdc.h"
#include "AdcInputs.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ARRAY_LENGTH(a) (sizeof(a) / sizeof((a)[0]))

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
/*
 * System Thermistor Table
 *
 * The first column in the table is thermistor resistance R_T in ohms
 * and the second column is the temperature in degrees C.
 *
 *               VDD ___
 *                      |
 *                      >
 *                P_PU  <
 *                      >
 *                      |
 *                      |
 *                      |- - - V_T
 *                      |
 *                      >
 *                R_T   <   100 kOhms (NTCG104EF104FB)
 *                      >
 *                      |
 *                      |
 *                     Gnd
 *
 */
static const AdcMapPtInt32toInt32Type adcMap_NTCG104EF104FB[] =
{
   { 4251000, -40 },
   { 3004900, -35 },
   { 2148900, -30 },
   { 1553800, -25 },
   { 1135300, -20 },
   {  837800, -15 },
   {  624100, -10 },
   {  469100, -5 },
   {  355600, 0 },
   {  271800, 5 },
   {  209400, 10 },
   {  162500, 15 },
   {  127000, 20 },
   {  100000, 25 },
   {   79200, 30 },
   {   63200, 35 },
   {   50700, 40 },
   {   40900, 45 },
   {   33200, 50 },
   {   27100, 55 },
   {   22200, 60 },
   {   18300, 65 },
   {   15200, 70 },
   {   12600, 75 },
   {   10600, 80 },
   {    8890, 85 },
   {    7500, 90 },
   {    6360, 95 },
   {    5410, 100 },
   {    4620, 105 },
   {    3970, 110 },
   {    3420, 115 },
   {    2950, 120 },
   {    2560, 125 }
};

const AdcIntTableType gVAdcSysThermTable =
{
   .pTable  = adcMap_NTCG104EF104FB,
   .uLength = ARRAY_LENGTH(adcMap_NTCG104EF104FB),
};

 /*----------------------------------------------------------------------------
 * 	PME9205_YODA Settings
 * -------------------------------------------------------------------------*/
/*
 * VAdc channel configuration.
 */

const VAdcChannelConfigType gVAdcChannels[] =
{
   /* VPH_PWR */
   {
      .pszName                   = ADC_INPUT_VPH_PWR,
      .uAdcHardwareChannel       = 0x83,
      .eSettlingDelay            = VADC_SETTLING_DELAY_0_US,
      .eAverageMode              = VADC_AVERAGE_1_SAMPLE,
      .eDecimationRatio          = VADC_DECIMATION_RATIO_1024,
      .eCalMethod                = VADC_CAL_METHOD_ABSOLUTE,
      .scalingFactor             = {1, 3},  /* {num, den} */
      .eScalingMethod            = VADC_SCALE_TO_MILLIVOLTS,
      .uPullUp                   = 0,
      .pIntTable                 = NULL,
   },
   
   /* PMIC_TEMP1 */
   {
      .pszName                   = ADC_INPUT_PMIC_TEMP1,
      .uAdcHardwareChannel       = 0x6,
      .eSettlingDelay            = VADC_SETTLING_DELAY_0_US,
      .eAverageMode              = VADC_AVERAGE_1_SAMPLE,
      .eDecimationRatio          = VADC_DECIMATION_RATIO_1024,
      .eCalMethod                = VADC_CAL_METHOD_ABSOLUTE,
      .scalingFactor             = {1, 1},  /* {num, den} */
      .eScalingMethod            = VADC_SCALE_DIE_TEMP_TO_MILLIDEGREES,
      .uPullUp                   = 0,
      .pIntTable                 = NULL,
   },

   /* XO_THERM_MV */
   {
      .pszName                   = ADC_INPUT_XO_THERM_MV,
      .uAdcHardwareChannel       = 0x4c,
      .eSettlingDelay            = VADC_SETTLING_DELAY_800_US,
      .eAverageMode              = VADC_AVERAGE_1_SAMPLE,
      .eDecimationRatio          = VADC_DECIMATION_RATIO_1024,
      .eCalMethod                = VADC_CAL_METHOD_RATIOMETRIC,
      .scalingFactor             = {1, 1},  /* {num, den} */
      .eScalingMethod            = VADC_SCALE_TO_MILLIVOLTS,
      .uPullUp                   = 100000,
      .pIntTable                 = NULL,
   },
   
  /* BATT_THERM (BAT_THERM pin) */
   {
      .pszName                   = ADC_INPUT_BATT_THERM,
      .uAdcHardwareChannel       = 0x4e,
      .eSettlingDelay            = VADC_SETTLING_DELAY_100_US,
      .eAverageMode              = VADC_AVERAGE_1_SAMPLE,
      .eDecimationRatio          = VADC_DECIMATION_RATIO_1024,
      .eCalMethod                = VADC_CAL_METHOD_RATIOMETRIC,
      .scalingFactor             = {1, 1},  /* {num, den} */
      .eScalingMethod            = VADC_SCALE_THERMISTOR,
      .uPullUp                   = 100000,
      .pIntTable                 = &gVAdcSysThermTable,
   },

   /* BATT_THERM_PU_30K (BAT_THERM pin) */
   {
      .pszName                   = ADC_INPUT_BATT_THERM_PU_30K,
      .uAdcHardwareChannel       = 0x2e,
      .eSettlingDelay            = VADC_SETTLING_DELAY_100_US,
      .eAverageMode              = VADC_AVERAGE_1_SAMPLE,
      .eDecimationRatio          = VADC_DECIMATION_RATIO_1024,
      .eCalMethod                = VADC_CAL_METHOD_RATIOMETRIC,
      .scalingFactor             = {1, 1},  /* {num, den} */
      .eScalingMethod            = VADC_SCALE_THERMISTOR,
      .uPullUp                   = 30000,
      .pIntTable                 = &gVAdcSysThermTable,
   },

   /* BATT_THERM_PU_400K (BAT_THERM pin) */
   {
      .pszName                   = ADC_INPUT_BATT_THERM_PU_400K,
      .uAdcHardwareChannel       = 0x6e,
      .eSettlingDelay            = VADC_SETTLING_DELAY_100_US,
      .eAverageMode              = VADC_AVERAGE_1_SAMPLE,
      .eDecimationRatio          = VADC_DECIMATION_RATIO_1024,
      .eCalMethod                = VADC_CAL_METHOD_RATIOMETRIC,
      .scalingFactor             = {1, 1},  /* {num, den} */
      .eScalingMethod            = VADC_SCALE_THERMISTOR,
      .uPullUp                   = 400000,
      .pIntTable                 = &gVAdcSysThermTable,
   },
   
   /* BATT_ID_OHMS (BAT_ID pin) */
   {
      .pszName                   = ADC_INPUT_BATT_ID_OHMS,
      .uAdcHardwareChannel       = 0x4d,
      .eSettlingDelay            = VADC_SETTLING_DELAY_100_US,
      .eAverageMode              = VADC_AVERAGE_1_SAMPLE,
      .eDecimationRatio          = VADC_DECIMATION_RATIO_1024,
      .eCalMethod                = VADC_CAL_METHOD_RATIOMETRIC,
      .scalingFactor             = {1, 1},  /* {num, den} */
      .eScalingMethod            = VADC_SCALE_RESISTOR_DIVIDER,
      .uPullUp                   = 100000,
      .pIntTable                 = NULL,
   },

   /* BATT_ID_OHMS_PU_30K (BAT_ID pin) */
   {
      .pszName                   = ADC_INPUT_BATT_ID_OHMS_PU_30K,
      .uAdcHardwareChannel       = 0x2d,
      .eSettlingDelay            = VADC_SETTLING_DELAY_100_US,
      .eAverageMode              = VADC_AVERAGE_1_SAMPLE,
      .eDecimationRatio          = VADC_DECIMATION_RATIO_1024,
      .eCalMethod                = VADC_CAL_METHOD_RATIOMETRIC,
      .scalingFactor             = {1, 1},  /* {num, den} */
      .eScalingMethod            = VADC_SCALE_RESISTOR_DIVIDER,
      .uPullUp                   = 30000,
      .pIntTable                 = NULL,
   },

   /* BATT_ID_OHMS_PU_400K (BAT_ID pin) */
   {
      .pszName                   = ADC_INPUT_BATT_ID_OHMS_PU_400K,
      .uAdcHardwareChannel       = 0x6d,
      .eSettlingDelay            = VADC_SETTLING_DELAY_100_US,
      .eAverageMode              = VADC_AVERAGE_1_SAMPLE,
      .eDecimationRatio          = VADC_DECIMATION_RATIO_1024,
      .eCalMethod                = VADC_CAL_METHOD_RATIOMETRIC,
      .scalingFactor             = {1, 1},  /* {num, den} */
      .eScalingMethod            = VADC_SCALE_RESISTOR_DIVIDER,
      .uPullUp                   = 400000,
      .pIntTable                 = NULL,
   },
   
};

const uint32 uNumVAdcChannels = ARRAY_LENGTH(gVAdcChannels);