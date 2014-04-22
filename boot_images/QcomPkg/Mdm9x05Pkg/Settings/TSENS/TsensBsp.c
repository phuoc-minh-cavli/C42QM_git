/*============================================================================
  FILE:         TsensBsp.c

  OVERVIEW:     Auto-generated TSENS BSP
                * Chip: SDM660, version: all
                * Config file: TSENS_Starlord.xlsm
                * Config file format version: 2
                * Generator version: 1.2.0

                Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2018-12-14  AY   Updated to support MDM9205
  2017-02-10  PC   Updated with new Autogen file
  2016-09-29  SA   Updated for SDM660.
  2016-03-01  jjo  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "TsensBsp.h"
#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ARRAY_LENGTH(a) (sizeof(a) / sizeof((a)[0]))

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/


static const TsensSensorCfgType gaTsensSensorCfgs[] =
{
   /* Sensor 0 */
   {
	  .uTsensConfig                = 0x1C3,
      .ucController                = 0,
      .ucChannel                   = 0,
      .ucSensorID                  = 0,
      .ucFuseIdx                   = 0,
      .eCal                        = TSENS_BSP_SENSOR_CAL_NORMAL,
      .nsCalPoint1CodeDefault      = 480,
      .nsCalPoint2CodeDefault      = 714,
	  .nM_default                  = 12602,
      .nsThresholdMinDegC          = TSENS_THRESHOLD_DISABLED,
      .nsThresholdLowerDegC        = TSENS_THRESHOLD_DISABLED,
      .nsThresholdUpperDegC        = TSENS_THRESHOLD_DISABLED,
      .nsThresholdCriticalDegC     = TSENS_THRESHOLD_DISABLED,
	  .uMTCThreshold1Temp          = 100,
	  .uMTCThreshold2Temp          = 110,
      .nsThresholdMaxDegC          = 120,
   },

   /* Sensor 1 */
   {
	  .uTsensConfig                = 0x11C3,
      .ucController                = 0,
      .ucChannel                   = 1,
      .ucSensorID                  = 1,
      .ucFuseIdx                   = 1,
      .eCal                        = TSENS_BSP_SENSOR_CAL_NORMAL,
      .nsCalPoint1CodeDefault      = 480,
      .nsCalPoint2CodeDefault      = 714,
	  .nM_default                  = 12602,
      .nsThresholdMinDegC          = TSENS_THRESHOLD_DISABLED,
      .nsThresholdLowerDegC        = TSENS_THRESHOLD_DISABLED,
      .nsThresholdUpperDegC        = TSENS_THRESHOLD_DISABLED,
      .nsThresholdCriticalDegC     = TSENS_THRESHOLD_DISABLED,
	  .uMTCThreshold1Temp          = 100,
	  .uMTCThreshold2Temp          = 110,
      .nsThresholdMaxDegC          = 120,
   },

   /* Sensor 2 */
   {
	  .uTsensConfig                = 0x11C3,
      .ucController                = 0,
      .ucChannel                   = 2,
      .ucSensorID                  = 2,
      .ucFuseIdx                   = 2,
      .eCal                        = TSENS_BSP_SENSOR_CAL_NORMAL,
      .nsCalPoint1CodeDefault      = 480,
      .nsCalPoint2CodeDefault      = 714,
	  .nM_default                  = 12602,
      .nsThresholdMinDegC          = TSENS_THRESHOLD_DISABLED,
      .nsThresholdLowerDegC        = TSENS_THRESHOLD_DISABLED,
      .nsThresholdUpperDegC        = TSENS_THRESHOLD_DISABLED,
      .nsThresholdCriticalDegC     = TSENS_THRESHOLD_DISABLED,
	  .uMTCThreshold1Temp          = 100,
	  .uMTCThreshold2Temp          = 110,
      .nsThresholdMaxDegC          = 120,
   },

   /* Sensor 3 */
   {
	  .uTsensConfig                = 0x11C3,
      .ucController                = 0,
      .ucChannel                   = 3,
      .ucSensorID                  = 3,
      .ucFuseIdx                   = 3,
      .eCal                        = TSENS_BSP_SENSOR_CAL_NORMAL,
      .nsCalPoint1CodeDefault      = 480,
      .nsCalPoint2CodeDefault      = 714,
	  .nM_default                  = 12602,
      .nsThresholdMinDegC          = TSENS_THRESHOLD_DISABLED,
      .nsThresholdLowerDegC        = TSENS_THRESHOLD_DISABLED,
      .nsThresholdUpperDegC        = TSENS_THRESHOLD_DISABLED,
      .nsThresholdCriticalDegC     = TSENS_THRESHOLD_DISABLED,
	  .uMTCThreshold1Temp          = 100,
	  .uMTCThreshold2Temp          = 110,	  
      .nsThresholdMaxDegC          = 120,
   },

   /* Sensor 4 */
   {
	  .uTsensConfig                = 0x11C3,
      .ucController                = 0,
      .ucChannel                   = 4,
      .ucSensorID                  = 4,
      .ucFuseIdx                   = 4,
      .eCal                        = TSENS_BSP_SENSOR_CAL_NORMAL,
      .nsCalPoint1CodeDefault      = 480,
      .nsCalPoint2CodeDefault      = 714,
	  .nM_default                  = 12602,
      .nsThresholdMinDegC          = TSENS_THRESHOLD_DISABLED,
      .nsThresholdLowerDegC        = TSENS_THRESHOLD_DISABLED,
      .nsThresholdUpperDegC        = TSENS_THRESHOLD_DISABLED,
      .nsThresholdCriticalDegC     = TSENS_THRESHOLD_DISABLED,
	  .uMTCThreshold1Temp          = 100,
	  .uMTCThreshold2Temp          = 110,
      .nsThresholdMaxDegC          = 120,
   },

};

static const TsensMTCConfigType aMTCConfig[] =
{
  /* Zone 0 */
  {
     /* .bIsZoneEnabled    */ 0,
     /* .uPSCommandTh2Viol */ 0,
     /* .uPSCommandTh1Viol */ 0,
     /* .uPSCommandCool    */ 0,
     /* .uSensorMask       */ 0, 
     /* .bIsTH1Enabled     */ 0,
     /* .bIsTH2Enabled     */ 0,
  },
  /* Zone 1 */
  {
     /* .bIsZoneEnabled    */ 0,
     /* .uPSCommandTh2Viol */ 0,
     /* .uPSCommandTh1Viol */ 0,
     /* .uPSCommandCool    */ 0,
     /* .uSensorMask       */ 0, // S4, S5, S6, S7 and S8
     /* .bIsTH1Enabled     */ 0,
     /* .bIsTH2Enabled     */ 0,
  }
};


const TsensBspType TsensBsp[] =
{
   {
	  .paMTCConfig         = aMTCConfig,
	  .uNumMTCZones        = ARRAY_LENGTH(aMTCConfig),
      .paSensorCfgs        = gaTsensSensorCfgs,
      .uNumSensors         = ARRAY_LENGTH(gaTsensSensorCfgs),
      .uSensorConvTime_us  = 150,
      .nCalPoint1DegC      = 30,
      .nCalPoint2DegC      = 120,
      .uShift              = 10,
	  .bPSHoldResetEn      = TRUE,
      .uPeriodActive       = 2,
      .uPeriodSleep        = 0x40,
      .bAutoAdjustPeriod   = TRUE,
      .uTSControl          = 0x0,
      .uTSConfig           = 0x0,
      .bStandAlone         = FALSE,
      .bPWMEn              = FALSE,
      .bWatchdogEn         = TRUE,
      .bWatchdogResetEn    = TRUE,
      .uWatchdogLoadVal    = 9850,
	  .bIsMTCSupported     = FALSE,
	  .uMTCThreshold1Margin = 5,
	  .uMTCThreshold2Margin = 5,
	  .uNumGetTempRetries   = 5,
	  .uPeriod              = 1,  
      .uGlobalConfig        = 0x0302F16C,
   }
};


