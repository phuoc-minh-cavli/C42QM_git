#ifndef __HALTSENS_H__
#define __HALTSENS_H__
/*============================================================================
  @file HALtsens.h

  This is the internal hardware abstraction layer for Tsens

                Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.tx/6.0/hwengines/tsens/hal/common/HALtsens.h#1 $ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "com_dtypes.h"     /* Definitions for byte, word, etc.     */

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
  HAL_TSENS_MIN_LIMIT_TH,
  HAL_TSENS_LOWER_LIMIT_TH,
  HAL_TSENS_UPPER_LIMIT_TH,
  HAL_TSENS_MAX_LIMIT_TH,
} HAL_tsens_ThresholdLevel;

typedef enum
{
  HAL_TSENS_INTERNAL,
  HAL_TSENS_EXTERNAL,
} HAL_tsens_ADCClkSrc;

typedef enum
{
  HAL_TSENS_DISABLE=0,
  HAL_TSENS_ENABLE
} HAL_tsens_State;

typedef enum
{
   HAL_TSENS_NOT_CALIBRATED=0,
   HAL_TSENS_ONE_POINT_PRELIM=1,
   HAL_TSENS_ONE_POINT=2,
   HAL_TSENS_TWO_POINT=3,
   HAL_TSENS_20_ONE_POINT=4,
   HAL_TSENS_20_TWO_POINT=5,
} HAL_tsens_Calibration;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Get temperature measurement
 * -------------------------------------------------------------------------*/
boolean HAL_tsens_GetSensorPrevTemp(uint32 uSensor, uint32 *puCode);
boolean HAL_tsens_TempMeasurementIsComplete(void);

/*----------------------------------------------------------------------------
 * Interrupts
 * -------------------------------------------------------------------------*/
void HAL_tsens_EnableUpperLowerInterrupt(void);
void HAL_tsens_DisableUpperLowerInterrupt(void);
void HAL_tsens_EnableInterrupt(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor);
void HAL_tsens_DisableInterrupt(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor);
void HAL_tsens_ClearInterrupt(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor);

/*----------------------------------------------------------------------------
 * Thresholds
 * -------------------------------------------------------------------------*/
boolean HAL_tsens_CurrentlyBeyondThreshold(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor);
void HAL_tsens_SetThreshold(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor, uint32 uThresholdCode);
uint32 HAL_tsens_GetThreshold(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor);

/*----------------------------------------------------------------------------
 * Measurement period
 * -------------------------------------------------------------------------*/
void HAL_tsens_SetPeriodActive(uint32 uTime);
void HAL_tsens_SetPeriodSleep(uint32 uTime);
void HAL_tsens_SetAutoAdjustPeriod(boolean bEnable);

/*----------------------------------------------------------------------------
 * Sensor enable / disable
 * -------------------------------------------------------------------------*/
void HAL_tsens_SetSensorsEnabled(uint32 uSensorMask);
uint32 HAL_tsens_GetEnabledSensors(void);

/*----------------------------------------------------------------------------
 * Main enable / disable
 * -------------------------------------------------------------------------*/
void HAL_tsens_SetState(HAL_tsens_State eTsensState);

/*----------------------------------------------------------------------------
 * Software reset
 * -------------------------------------------------------------------------*/
void HAL_tsens_Reset(void);

/*----------------------------------------------------------------------------
 * Config
 * -------------------------------------------------------------------------*/
void HAL_tsens_Init(uint32 uVal);
void HAL_tsens_ConfigSensor(uint32 uSensor, uint32 uVal);
void HAL_tsens_SelectADCClkSrc(HAL_tsens_ADCClkSrc eADCClkSrc);
uint32 HAL_tsens_GetMaxCode(void);

/*----------------------------------------------------------------------------
 * Char data
 * -------------------------------------------------------------------------*/
boolean HAL_tsens_UseRedundant(void);
HAL_tsens_Calibration HAL_tsens_CalSelect(boolean bUseRedundant);
uint32 HAL_tsens_GetBaseX1(boolean bUseRedundant);
uint32 HAL_tsens_GetBaseX2(boolean bUseRedundant);
uint32 HAL_tsens_GetPointX1(boolean bUseRedundant, uint32 uSensor);
uint32 HAL_tsens_GetPointX2(boolean bUseRedundant, uint32 uSensor);

#endif /* #ifndef __HALTSENS_H__ */
