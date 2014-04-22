#ifndef __TSENS_BSP_H__
#define __TSENS_BSP_H__
/*============================================================================
  @file TsensBsp.h

  Tsens BSP file.

                Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define TSENS_THRESHOLD_DISABLED 0x7fff
#define TSENS_MAX_NUM_CHANNELS_PER_CONTROLLER 16
#define TSENS_MAX_NUM_TBCB_CLIENTS_PER_CONTROLLER 16

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALStdDef.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
   TSENS_BSP_SENSOR_CAL_NORMAL = 0,        /* Normal method, i.e. use QFPROM if
                                            * avail else use default char data */
   TSENS_BSP_SENSOR_CAL_IGNORE_DEVICE_CAL  /* Force using default char data */
} TsensBspSensorCalType;

typedef enum
{
   TSENS_BSP_MTC_SYS_PERF_100 = 0x10,
   TSENS_BSP_MTC_SYS_PERF_75  = 0x11,
   TSENS_BSP_MTC_SYS_PERF_67  = 0x12,
   TSENS_BSP_MTC_SYS_PERF_50  = 0x13,
   TSENS_BSP_MTC_SYS_PERF_33  = 0x14,
   TSENS_BSP_MTC_SYS_PERF_25  = 0x15,
} TsensBspMTCSysPerfType;

typedef struct
{
   uint32 uMTCThreshold1Temp;          /* Multi-Zone Temperature Control Threshold1 Temperature where crossover occurs between cool and yellow zones */
   uint32 uMTCThreshold2Temp;          /* Multi-Zone Temperature Control Threshold2 Temperature where crossover occurs between cool and yellow zones */
   uint32 uTsensConfig;          /* Config value for the sensor */
   TsensBspSensorCalType eCal;        /* Which cal type to use */
   int32 nM_default;             /* Default slope: factor * median slope [C/code] */
   int16 nsCalPoint1CodeDefault;      /* Default TSENS code at calibration point nCalPoint1DeciDegC */
   int16 nsCalPoint2CodeDefault;      /* Default TSENS code at calibration point nCalPoint2DeciDegC */
   int16 nsThresholdMinDegC;      /* Minimum temperature threshold for critical shutdown
                                       * or use TSENS_THRESHOLD_DISABLED to disable */
   int16 nsThresholdLowerDegC;    /* Default lower threshold for software interrupt or
                                       * or use TSENS_THRESHOLD_DISABLED to disable */
   int16 nsThresholdUpperDegC;    /* Default upper threshold for software interrupt or
                                       * or use TSENS_THRESHOLD_DISABLED to disable */
   int16 nsThresholdCriticalDegC; /* Default critical threshold for software interrupt or
                                       * use TSENS_THRESHOLD_DISABLED to disable */
   int16 nsThresholdMaxDegC;      /* Maximum temperature threshold for critical shutdown
                                       * or use TSENS_THRESHOLD_DISABLED to disable */
   uint8 ucController;                /* Controller index */
   uint8 ucChannel;                   /* Channel index */
   uint8 ucSensorID;                  /* Sensor ID for TBCB */
   uint8 ucFuseIdx;                   /* Which fuse corresponds to this sensor */
} TsensSensorCfgType;

typedef struct
{
   uint32 bIsZoneEnabled;
   TsensBspMTCSysPerfType uPSCommandTh2Viol;
   TsensBspMTCSysPerfType uPSCommandTh1Viol;
   TsensBspMTCSysPerfType uPSCommandCool;
   uint32 uSensorMask;
   DALBOOL bIsTH1Enabled;
   DALBOOL bIsTH2Enabled;
} TsensMTCConfigType;

typedef struct
{

   const TsensSensorCfgType *paSensorCfgs;          /* Array of sensor configs */
   uint32 uNumSensors;                              /* Number of sensors */
   const TsensMTCConfigType *paMTCConfig;           /* Array of sensor configs */
   uint32 uNumMTCZones;                             /* number of MTC Zones */
   uint32 uSensorConvTime_us;                       /* Sensor conversion time in us */
   int32 nCalPoint1DegC;                        /* Calibration point 1 in deci deg C */
   int32 nCalPoint2DegC;                        /* Calibration point 2 in deci deg C */
   uint32 uShift;                                   /* Shift value */
    uint32 uPeriodActive;                           /* Active period */
   uint32 uPeriodSleep;                             /* Sleep period */
   uint32 uTSControl;                               /* Analog TS control setting */
   uint32 uTSConfig;                                /* Analog TS config setting */
   uint32 uSidebandEnMask;                          /* Mask of sideband sensors to enable */
   uint32 uWatchdogLoadVal;                         /* Load value for watchdog in sleep clock ticks */
   DALBOOL bPSHoldResetEn;                          /* Whether to enable PS_HOLD reset */
   DALBOOL bAutoAdjustPeriod;                       /* Whether to adjust period on sleep */
   DALBOOL bStandAlone;                             /* Whether this is a stand alone controller */
   DALBOOL bPWMEn;                                  /* Whether to enable PWM */
   DALBOOL bWatchdogEn;                             /* Whether to enable the watchdog */
   DALBOOL bWatchdogResetEn;                        /* Whether or not watchdog can perform TSENS resets */
   uint32 bIsMTCSupported;
   uint32 uMTCThreshold1Margin;
   uint32 uMTCThreshold2Margin;
   uint32 uNumGetTempRetries;
   uint32 uPeriod;
   uint32 uGlobalConfig;
} TsensBspType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

#endif /* #ifndef __TSENS_BSP_H__ */

