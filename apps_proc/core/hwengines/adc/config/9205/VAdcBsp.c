/*============================================================================
  FILE:         VAdcBsp.c

  OVERVIEW:     Board support package for the VADC ADC Physical Device DAL.

  DEPENDENCIES: None

                Copyright (c) 2010-2015, 2019 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.tx/6.0/hwengines/adc/config/9205/VAdcBsp.c#1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2019-04-16  ay   CR:2436255 Routing all the read requests for VBATT to VPH_PWR 
  2018-12-28  ay   updated for MDM9205
  2015-04-23  jjo  Add thermistor scaling.
  2015-01-08  jjo  Add BTM support.
  2014-11-05  jjo  Remove VBATT channel (VPH_PWR is being measured).
  2014-03-31  jjo  Ported to 9x45.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalVAdc.h"
#include "VAdcHal.h"
#include "AdcInputs.h"
#include "AdcScalingUtil.h"

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

enum
{
   VADC_CONFIG_NORMAL,
   VADC_CONFIG_SLOW,
   VADC_CONFIG_FAST,
   VADC_NUM_CONFIGS
};

/*
 * System Thermistor Table
 * 
 * The first element is Voltage V_t in mV and the second element is the
 * temperature in mDegC. 
 *  
 *             V_ref ___  1800 mV
 *                      |
 *                      >
 *                 R_p  <  100 kOhms
 *                      >
 *                      |
 *                      |
 *                      |- - - V_t
 *                      |     
 *                      >     
 *                 R_t  <  100 kOhms (NTCG104EF104FB)    
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


/*
 * XO Thermistor Table
 * 
 * This lookup table is used to convert the XO thermistor reading to temperature
 * in degrees C multiplied by a factor of 1024.
 *
 * The first column in the table is the ratio of Rt' divided by R25 (multiplied
 * by a factor of 2^14).
 *
 * The second column is the temperature in degrees Celsius multiplied by a factor
 * of 1024.
 *
 * Rt' is the resistance of the thermistor at temperature t in parallel with the
 * input impedance of the ADC.
 *
 * R25 is the series resistor. The resistor value should be the same as that
 * of the thermistor at 25 degrees C (100 KOhms in the reference design).   
 *
 *             V_ref ___
 *                      |
 *                      >
 *                R_25  <   100 kOhms
 *                      >
 *                      |
 *                      |
 *                      |- - - V_t
 *                      |     
 *                      >     
 *                R_t   <   100 kOhms (NTCG104EF104FB)
 *                      >     
 *                      |     
 *                      |
 *                     Gnd
 *
 */

static const AdcMapPtInt32toInt32Type adcMap_XO_NTCG104EF104FB[] =
{
   { 4250657, -40960 },
   { 3962085, -39936 },
   { 3694875, -38912 },
   { 3447322, -37888 },
   { 3217867, -36864 },
   { 3005082, -35840 },
   { 2807660, -34816 },
   { 2624405, -33792 },
   { 2454218, -32768 },
   { 2296094, -31744 },
   { 2149108, -30720 },
   { 2012414, -29696 },
   { 1885232, -28672 },
   { 1766846, -27648 },
   { 1656598, -26624 },
   { 1553884, -25600 },
   { 1458147, -24576 },
   { 1368873, -23552 },
   { 1285590, -22528 },
   { 1207863, -21504 },
   { 1135290, -20480 },
   { 1067501, -19456 },
   { 1004155, -18432 },
   { 944935, -17408 },
   { 889550, -16384 },
   { 837731, -15360 },
   { 789229, -14336 },
   { 743813, -13312 },
   { 701271, -12288 },
   { 661405, -11264 },
   { 624032, -10240 },
   { 588982, -9216 },
   { 556100, -8192 },
   { 525239, -7168 },
   { 496264, -6144 },
   { 469050, -5120 },
   { 443480, -4096 },
   { 419448, -3072 },
   { 396851, -2048 },
   { 375597, -1024 },
   { 355598, 0 },
   { 336775, 1024 },
   { 319052, 2048 },
   { 302359, 3072 },
   { 286630, 4096 },
   { 271806, 5120 },
   { 257829, 6144 },
   { 244646, 7168 },
   { 232209, 8192 },
   { 220471, 9216 },
   { 209390, 10240 },
   { 198926, 11264 },
   { 189040, 12288 },
   { 179698, 13312 },
   { 170868, 14336 },
   { 162519, 15360 },
   { 154622, 16384 },
   { 147150, 17408 },
   { 140079, 18432 },
   { 133385, 19456 },
   { 127046, 20480 },
   { 121042, 21504 },
   { 115352, 22528 },
   { 109960, 23552 },
   { 104848, 24576 },
   { 100000, 25600 },
   { 95402, 26624 },
   { 91038, 27648 },
   { 86897, 28672 },
   { 82965, 29696 },
   { 79232, 30720 },
   { 75686, 31744 },
   { 72316, 32768 },
   { 69114, 33792 },
   { 66070, 34816 },
   { 63176, 35840 },
   { 60423, 36864 },
   { 57804, 37888 },
   { 55312, 38912 },
   { 52940, 39936 },
   { 50681, 40960 },
   { 48531, 41984 },
   { 46482, 43008 },
   { 44530, 44032 },
   { 42670, 45056 },
   { 40897, 46080 },
   { 39207, 47104 },
   { 37595, 48128 },
   { 36057, 49152 },
   { 34590, 50176 },
   { 33190, 51200 },
   { 31853, 52224 },
   { 30577, 53248 },
   { 29358, 54272 },
   { 28194, 55296 },
   { 27082, 56320 },
   { 26020, 57344 },
   { 25004, 58368 },
   { 24033, 59392 },
   { 23104, 60416 },
   { 22216, 61440 },
   { 21367, 62464 },
   { 20554, 63488 },
   { 19776, 64512 },
   { 19031, 65536 },
   { 18318, 66560 },
   { 17636, 67584 },
   { 16982, 68608 },
   { 16355, 69632 },
   { 15755, 70656 },
   { 15180, 71680 },
   { 14628, 72704 },
   { 14099, 73728 },
   { 13592, 74752 },
   { 13106, 75776 },
   { 12640, 76800 },
   { 12192, 77824 },
   { 11762, 78848 },
   { 11350, 79872 },
   { 10954, 80896 },
   { 10574, 81920 },
   { 10209, 82944 },
   { 9858, 83968 },
   { 9521, 84992 },
   { 9197, 86016 },
   { 8886, 87040 },
   { 8587, 88064 },
   { 8299, 89088 },
   { 8023, 90112 },
   { 7757, 91136 },
   { 7501, 92160 },
   { 7254, 93184 },
   { 7017, 94208 },
   { 6789, 95232 },
   { 6570, 96256 },
   { 6358, 97280 },
   { 6155, 98304 },
   { 5959, 99328 },
   { 5770, 100352 },
   { 5588, 101376 },
   { 5412, 102400 },
   { 5243, 103424 },
   { 5080, 104448 },
   { 4923, 105472 },
   { 4771, 106496 },
   { 4625, 107520 },
   { 4484, 108544 },
   { 4348, 109568 },
   { 4217, 110592 },
   { 4090, 111616 },
   { 3968, 112640 },
   { 3850, 113664 },
   { 3736, 114688 },
   { 3626, 115712 },
   { 3519, 116736 },
   { 3417, 117760 },
   { 3317, 118784 },
   { 3221, 119808 },
   { 3129, 120832 },
   { 3039, 121856 },
   { 2952, 122880 },
   { 2868, 123904 },
   { 2787, 124928 },
   { 2709, 125952 },
   { 2633, 126976 },
   { 2560, 128000 },
   { 2489, 129024 },
   { 2420, 130048 }
};

const AdcIntTableType gVAdcSysThermTable =
{
   .pTable  = adcMap_NTCG104EF104FB,
   .uLength = ARRAY_LENGTH(adcMap_NTCG104EF104FB),
};

const AdcIntTableType gVAdcXoThermTable =
{
   .pTable  = adcMap_XO_NTCG104EF104FB,
   .uLength = ARRAY_LENGTH(adcMap_XO_NTCG104EF104FB),
};

/*
 * VAdc channel configuration.
 */
static const VAdcChannelConfigType vAdcChannels[] = {

   /*VPH_PWR */
   {
       .pName                  =      ADC_INPUT_VPH_PWR,
       .uAdcHardwareChannel      =      0x83,
       .eSettlingDelay           =      VADC_SETTLING_DELAY_0_US,
       .eFastAverageMode             =      VADC_FAST_AVERAGE_1_SAMPLE,
       .eDecimationRatio         =      VADC_DECIMATION_RATIO_1024,
       .eCalMethod               =      VADC_CAL_METHOD_ABSOLUTE,
       .scalingFactor            =      {1, 3},
       .eScalingMethod           =      VADC_SCALE_TO_MILLIVOLTS,
       .uPullUp                  =      0,
	   .pIntTable                =      NULL,
   },
   
   /*VBATT */                                      
   {
       .pName                  =      ADC_INPUT_VBATT,                //Routing all the read requests for VBATT to VPH_PWR
       .uAdcHardwareChannel      =      0x83,
       .eSettlingDelay           =      VADC_SETTLING_DELAY_0_US,
       .eFastAverageMode             =      VADC_FAST_AVERAGE_1_SAMPLE,
       .eDecimationRatio         =      VADC_DECIMATION_RATIO_1024,
       .eCalMethod               =      VADC_CAL_METHOD_ABSOLUTE,
       .scalingFactor            =      {1, 3},
       .eScalingMethod           =      VADC_SCALE_TO_MILLIVOLTS,
       .uPullUp                  =      0,
	   .pIntTable                =      NULL,
   },

   /*DIE_TEMP */
   {
      .pName                  =      ADC_INPUT_PMIC_THERM,
      .uAdcHardwareChannel      =      0x6,
      .eSettlingDelay           =      VADC_SETTLING_DELAY_0_US,
      .eFastAverageMode             =      VADC_FAST_AVERAGE_1_SAMPLE,
      .eDecimationRatio         =      VADC_DECIMATION_RATIO_1024,
      .eCalMethod               =      VADC_CAL_METHOD_ABSOLUTE,
      .scalingFactor            =      {1, 1},
      .eScalingMethod           =      VADC_SCALE_DIE_TEMP_TO_MILLIDEGREES,
      .uPullUp 			        =	     0,
      .pIntTable			    =	     NULL,

   },
   
   /* BATT_ID_OHMS (BAT_ID pin) */
   {
      .pName                   = ADC_INPUT_BATT_ID,
      .uAdcHardwareChannel       = 0x4d,
      .eSettlingDelay            = VADC_SETTLING_DELAY_100_US,
      .eFastAverageMode              = VADC_FAST_AVERAGE_1_SAMPLE,
      .eDecimationRatio          = VADC_DECIMATION_RATIO_1024,
      .eCalMethod                = VADC_CAL_METHOD_RATIOMETRIC,
      .scalingFactor             = {1, 1},  /* {num, den} */
      .eScalingMethod            = VADC_SCALE_RESISTOR_DIVIDER,
      .uPullUp                   = 100000,
      .pIntTable                 = NULL,
   },

   /*XO_THERM */
   {
      .pName                  =      ADC_INPUT_XO_THERM,
      .uAdcHardwareChannel      =      0x4c,
      .eSettlingDelay           =      VADC_SETTLING_DELAY_1_MS,
      .eFastAverageMode             =      VADC_FAST_AVERAGE_1_SAMPLE,
      .eDecimationRatio         =      VADC_DECIMATION_RATIO_1024,
      .eCalMethod               =      VADC_CAL_METHOD_RATIOMETRIC,
      .scalingFactor            =      {1, 1},
      .eScalingMethod           =      VADC_SCALE_THERMISTOR,
      .uPullUp                  =      100000,
	  .pIntTable                =      &gVAdcXoThermTable,
   },

   /* XO_THERM_GPS_LOW (XO_THERM pin) */
   {
      .pName                   =      ADC_INPUT_XO_THERM_GPS_LOW,
      .uAdcHardwareChannel       =      0x6c,
      .eSettlingDelay            =      VADC_SETTLING_DELAY_4_MS,
      .eFastAverageMode              =      VADC_FAST_AVERAGE_4_SAMPLES,
      .eDecimationRatio          =      VADC_DECIMATION_RATIO_1024,
      .eCalMethod                =      VADC_CAL_METHOD_RATIOMETRIC,
      .scalingFactor             =      {1, 1},  /* {num, den} */
      .eScalingMethod            =      VADC_SCALE_THERMISTOR,
      .uPullUp                   =      400000,
	  .pIntTable                 =      &gVAdcXoThermTable,
   },

   /* XO_THERM_GPS_MED (XO_THERM pin) */
   {
      .pName                   =      ADC_INPUT_XO_THERM_GPS_MED,
      .uAdcHardwareChannel       =      0x4c,
      .eSettlingDelay            =      VADC_SETTLING_DELAY_1_MS,
      .eFastAverageMode              =      VADC_FAST_AVERAGE_4_SAMPLES,
      .eDecimationRatio          =      VADC_DECIMATION_RATIO_1024,
      .eCalMethod                =      VADC_CAL_METHOD_RATIOMETRIC,
      .scalingFactor             =      {1, 1},  /* {num, den} */
      .eScalingMethod            =      VADC_SCALE_THERMISTOR,
      .uPullUp                   =      100000,
	  .pIntTable                 =      &gVAdcXoThermTable,
   },

   /* XO_THERM_GPS_HIGH (XO_THERM pin) */
   {
      .pName                   =     ADC_INPUT_XO_THERM_GPS_HIGH,
      .uAdcHardwareChannel       =     0x2c,
      .eSettlingDelay            =     VADC_SETTLING_DELAY_300_US,
      .eFastAverageMode              =     VADC_FAST_AVERAGE_4_SAMPLES,
      .eDecimationRatio          =     VADC_DECIMATION_RATIO_1024,
      .eCalMethod                =     VADC_CAL_METHOD_RATIOMETRIC,
      .scalingFactor             =     {1, 1},  /* {num, den} */
      .eScalingMethod            =     VADC_SCALE_THERMISTOR,
      .uPullUp                   =     30000,
	  .pIntTable                 =     &gVAdcXoThermTable,
   },
   
   /* PA_THERM (AMUX_2 pin) */
   {
      .pName                   = ADC_INPUT_PA_THERM,
      .uAdcHardwareChannel       = 0x55,
      .eSettlingDelay            = VADC_SETTLING_DELAY_100_US,
      .eFastAverageMode              = VADC_FAST_AVERAGE_1_SAMPLE,
      .eDecimationRatio          = VADC_DECIMATION_RATIO_1024,
      .eCalMethod                = VADC_CAL_METHOD_RATIOMETRIC,
      .scalingFactor             = {1, 1},  /* {num, den} */
      .eScalingMethod            = VADC_SCALE_THERMISTOR,
      .uPullUp                   = 100000,
      .pIntTable                 = &gVAdcSysThermTable,
   },
   
};

const VAdcAveragedChannelConfigType gVAdcAveragedChannels[] =
{
   {
      .pszName     = ADC_INPUT_XO_THERM_GPS,
      .pszLowName  = ADC_INPUT_XO_THERM_GPS_LOW,
      .pszMedName  = ADC_INPUT_XO_THERM_GPS_MED,
      .pszHighName = ADC_INPUT_XO_THERM_GPS_HIGH,
      .nMedToLow   = 5120,
      .nLowToMed   = 15360,
      .nHighToMed  = 32768,
      .nMedToHigh  = 43008,
   },
};

const uint32 guNumVAdcChannels = ARRAY_LENGTH(vAdcChannels);
const uint32 guNumVAdcAveragedChannels = ARRAY_LENGTH(gVAdcAveragedChannels);

/*
 * This table is a mapping of ADC channels to GPIO inputs.
 */
static const uint8 aucAmux1GpioChans[] = {0x12, 0x32, 0x52, 0x72};
static const uint8 aucAmux2GpioChans[] = {0x13, 0x33, 0x53, 0x73};
static const uint8 aucAmux3GpioChans[] = {0x14, 0x34, 0x54, 0x74, 0x94};
static const uint8 aucAmux4GpioChans[] = {0x15, 0x35, 0x55, 0x75, 0x95};


static const VAdcGpioChannelMappingType vAdcGpioChannelMappings[] =
{
   {
      .paucChannels = aucAmux1GpioChans,
      .uNumChannels = ARRAY_LENGTH(aucAmux1GpioChans),
      .eGpio        = PM_GPIO_1,
   },

   {
      .paucChannels = aucAmux2GpioChans,
      .uNumChannels = ARRAY_LENGTH(aucAmux2GpioChans),
      .eGpio        = PM_GPIO_2,
   },

   {
      .paucChannels = aucAmux3GpioChans,
      .uNumChannels = ARRAY_LENGTH(aucAmux3GpioChans),
      .eGpio        = PM_GPIO_3,
   },

   {
      .paucChannels = aucAmux4GpioChans,
      .uNumChannels = ARRAY_LENGTH(aucAmux4GpioChans),
      .eGpio        = PM_GPIO_4,    
   },
};



const VAdcBspType VAdcBsp[] = {
   {
      .paChannels              = vAdcChannels,
      .uNumChannels            =  guNumVAdcChannels,
      .paAveragedChannels      = gVAdcAveragedChannels,
      .puNumAveragedChannels   = &guNumVAdcAveragedChannels,
      .paGpioChannelMappings   = vAdcGpioChannelMappings,
      .uNumGpioChannelMappings = ARRAY_LENGTH(vAdcGpioChannelMappings),
      .bUsesInterrupts         = FALSE,
      .uFullScale_code         = 0x70E4,
      .uFullScale_uV           = 1875000,
      .uReadTimeout_us         = 500000,
      .uLDOSettlingTime_us     = 17,
      .uSlaveId                = 0,
      .uPeripheralID           = 0x31,
      .uMasterID               = 0,
      .eAccessPriority         = SPMI_BUS_ACCESS_PRIORITY_LOW,
      .uPmicDevice             = 0,
   }
};

