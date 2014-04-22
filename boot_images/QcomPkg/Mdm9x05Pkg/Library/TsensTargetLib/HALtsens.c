/*============================================================================
  FILE:         HALTsens.c

  OVERVIEW:     Implementation of the TSENS HAL for 8952

  DEPENDENCIES: None

                Copyright (c) 2015, 2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.


  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2018-12-14  AY   Updated to support MDM9205
  2015-02-12  SA   Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "HALhwio.h"
#include "HALhwioTsens.h"
#include "HALtsens.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Get temperature measurement
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_GetSensorPrevTemp
**
**  Description:
**    Gets the adc result from the previous output.
**
**  Parameters:
**    uSensor - which of the sensors
**    puCode  - TSENS ADC code
**
**  Return: true if reading is valid, else false
**
**  Dependencies: adc result must be generated already
**
** ========================================================================= */
boolean HAL_tsens_GetSensorPrevTemp(uint32 uSensor, uint32 *puCode)
{
   uint32 uTsensSnStatus;
   //uint32 uValidBit;

   uTsensSnStatus = HWIO_INI(MPM2_TSENS_Sn_STATUS, uSensor);

   *puCode = (uTsensSnStatus & HWIO_FMSK(MPM2_TSENS_Sn_STATUS, LAST_TEMP)) >> HWIO_SHFT(MPM2_TSENS_Sn_STATUS, LAST_TEMP);

   return TRUE;

}

/* ============================================================================
**
**  HAL_tsens_TempMeasurementIsComplete
**
**  Description:
**    Finds out if the adc result of a given sensor is ready to read.
**
**  Parameters:
**    None
**
**  Return: whether temperature measurement has been completed w/ the associated
**          adc result in the register as a boolean (T/F).
**
**  Dependencies: tsens is enabled
**
** ========================================================================= */
boolean HAL_tsens_TempMeasurementIsComplete(void)
{
   uint32 uTempReady;

   uTempReady = HWIO_INF(MPM2_TSENS_TRDY, TRDY);

   if (uTempReady == 0)
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}

/*----------------------------------------------------------------------------
 * Interrupts
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_EnableUpperLowerInterrupt
**
**  Description:
**    Main enable for upper / lower thresholds
**
**  Parameters:
**    None
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_EnableUpperLowerInterrupt(void)
{
   HWIO_OUTV(MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL, INTERRUPT_EN, ENABLED);
}

/* ============================================================================
**
**  HAL_tsens_DisableUpperLowerInterrupt
**
**  Description:
**    Main disable for upper / lower thresholds
**
**  Parameters:
**    None
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_DisableUpperLowerInterrupt(void)
{
   HWIO_OUTV(MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL, INTERRUPT_EN, DISABLED);
}
/* ============================================================================
**
**  HAL_tsens_EnableInterrupt
**
**  Description:
**    Enable tsens for a given threshold level
**
**  Parameters:
**    eThresholdLevel
**    uSensor
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_EnableInterrupt(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor)
{
   switch(eThresholdLevel)
   {
      case HAL_TSENS_MIN_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MIN_STATUS_MASK, NORMAL_OPERATION);
         break;
      case HAL_TSENS_LOWER_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, LOWER_STATUS_CLR, NORMAL_OPERATION);
         break;
      case HAL_TSENS_UPPER_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, UPPER_STATUS_CLR, NORMAL_OPERATION);
         break;
      case HAL_TSENS_MAX_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MAX_STATUS_MASK, NORMAL_OPERATION);
         break;
      default:
         break;
   }
}

/* ============================================================================
**
**  HAL_tsens_DisableInterrupt
**
**  Description:
**    Disable tsens for a given threshold level
**
**  Parameters:
**    eThresholdLevel
**    uSensor
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_DisableInterrupt(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor)
{
   switch(eThresholdLevel)
   {
      case HAL_TSENS_MIN_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MIN_STATUS_MASK, MASK_OFF_MIN_STATUS);
         break;
      case HAL_TSENS_LOWER_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, LOWER_STATUS_CLR, CLEAR_LOWER_STATUS);
         break;
      case HAL_TSENS_UPPER_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, UPPER_STATUS_CLR, CLEAR_UPPER_STATUS);
         break;
      case HAL_TSENS_MAX_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MAX_STATUS_MASK, MASK_OFF_MAX_STATUS);
         break;
      default:
         break;
   }
}

/* ============================================================================
**
**  HAL_tsens_ClearInterrupt
**
**  Description:
**    Clears tsens interrupt for a given threshold level
**
**  Parameters:
**    eThresholdLevel
**    uSensor
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_ClearInterrupt(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor)
{
   uint32 uStatusClr;

   switch(eThresholdLevel)
   {
      case HAL_TSENS_MIN_LIMIT_TH:
         break;
      case HAL_TSENS_LOWER_LIMIT_TH:
         uStatusClr = HWIO_INFI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, LOWER_STATUS_CLR);
         if (uStatusClr == 1)
         {
            // Clear bit already set - do nothing
            return;
         }

         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, LOWER_STATUS_CLR, CLEAR_LOWER_STATUS);
         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, LOWER_STATUS_CLR, NORMAL_OPERATION);

         break;
      case HAL_TSENS_UPPER_LIMIT_TH:
         uStatusClr = HWIO_INFI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, UPPER_STATUS_CLR);
         if (uStatusClr == 1)
         {
            // Clear bit already set - do nothing
            return;
         }

         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, UPPER_STATUS_CLR, CLEAR_UPPER_STATUS);
         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, UPPER_STATUS_CLR, NORMAL_OPERATION);

         break;
      case HAL_TSENS_MAX_LIMIT_TH:
         break;
      default:
         break;
   }
}

/*----------------------------------------------------------------------------
 * Thresholds
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_CurrentlyBeyondThreshold
**
**  Description:
**    Finds out if a given threshold has crossed
**
**  Parameters:
**    eThresholdLevel
**    uSensor
**
**  Return: T/F
**
**  Dependencies: None
**
** ========================================================================= */
boolean HAL_tsens_CurrentlyBeyondThreshold(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor)
{
   uint32 uThreshSet = 0;

   switch(eThresholdLevel)
   {
      case HAL_TSENS_MIN_LIMIT_TH:
         uThreshSet = HWIO_INFI(MPM2_TSENS_Sn_STATUS, uSensor, MIN_STATUS);
         break;
      case HAL_TSENS_LOWER_LIMIT_TH:
         uThreshSet = HWIO_INFI(MPM2_TSENS_Sn_STATUS, uSensor, LOWER_STATUS);
         break;
      case HAL_TSENS_UPPER_LIMIT_TH:
         uThreshSet = HWIO_INFI(MPM2_TSENS_Sn_STATUS, uSensor, UPPER_STATUS);
         break;
      case HAL_TSENS_MAX_LIMIT_TH:
         uThreshSet = HWIO_INFI(MPM2_TSENS_Sn_STATUS, uSensor, MAX_STATUS);
         break;
      default:
         break;
   }

   if (uThreshSet == 0)
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}

/* ============================================================================
**
**  HAL_tsens_SetThreshold
**
**  Description:
**    Sets a given threshold
**
**  Parameters:
**    eThresholdLevel
**    uSensor
**    uThresholdCode - Value for that threshold
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_SetThreshold(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor, uint32 uThresholdCode)
{
   switch(eThresholdLevel)
   {
      case HAL_TSENS_MIN_LIMIT_TH:
         HWIO_OUTFI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MIN_THRESHOLD, uThresholdCode);
         break;
      case HAL_TSENS_LOWER_LIMIT_TH:
         HWIO_OUTFI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, LOWER_THRESHOLD, uThresholdCode);
         break;
      case HAL_TSENS_UPPER_LIMIT_TH:
         HWIO_OUTFI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, UPPER_THRESHOLD, uThresholdCode);
         break;
      case HAL_TSENS_MAX_LIMIT_TH:
         HWIO_OUTFI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MAX_THRESHOLD, uThresholdCode);
         break;
      default:
         break;
   }
}

/* ============================================================================
**
**  HAL_tsens_GetThreshold
**
**  Description:
**    Sets a given threshold
**
**  Parameters:
**    eThresholdLevel
**    uSensor
**
**  Return: Value for that threshold
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetThreshold(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor)
{
   uint32 uAdcCode = 0;

   switch(eThresholdLevel)
   {
      case HAL_TSENS_MIN_LIMIT_TH:
         uAdcCode = HWIO_INFI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MIN_THRESHOLD);
         break;
      case HAL_TSENS_LOWER_LIMIT_TH:
         uAdcCode = HWIO_INFI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, LOWER_THRESHOLD);
         break;
      case HAL_TSENS_UPPER_LIMIT_TH:
         uAdcCode = HWIO_INFI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, UPPER_THRESHOLD);
         break;
      case HAL_TSENS_MAX_LIMIT_TH:
         uAdcCode = HWIO_INFI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MAX_THRESHOLD);
         break;
      default:
         break;
   }

   return uAdcCode;
}

/*----------------------------------------------------------------------------
 * Measurement period
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_SetPeriod
**
**  Description:
**    Sets the re-sampling rate
**
**  Parameters:
**    0x00 measures temp once
**    0x01 62.5 ms
**    0xFF 63.5625 s
**    There is a 0.25 sec increment between 0x01 - 0xFF
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_SetPeriod(uint32 uTime)
{
   HWIO_OUTF(MPM2_TSENS_CTRL, MEASURE_PERIOD, uTime);
}
/* ============================================================================
**
**  HAL_tsens_SetPeriodSleep
**
**  Description:
**    Sets the period for the sleep case
**
**  Parameters:
**    0x00 measures continiously
**    0x01-0xFE defines the length of sleep between two measurements
**              approx equal to (upper 2bits) * 250ms + (lower 6bits) * 1.95ms
**    0xFF measures once
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_SetPeriodSleep(uint32 uTime)
{
   //HWIO_OUTF(MPM2_TSENS_POWERDOWN_MEASURE_PERIOD, POWERDOWN_MEASURE_PERIOD, uTime);
   return;
}

/* ============================================================================
**
**  HAL_tsens_SetAutoAdjustPeriod
**
**  Description:
**    Sets whether or not TSENS will auto-adjust the period going in and
**    out of sleep
**
**  Parameters:
**    bEnable - TRUE / FALSE
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_SetAutoAdjustPeriod(boolean bEnable)
{ /*
   if (bEnable == TRUE)
   {
      HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_AUTO_ADJUST_PERIOD_EN, ENABLED);
   }
   else
   {
      HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_AUTO_ADJUST_PERIOD_EN, DISABLED);
   }
   */
   return;
}
/* ============================================================================
**
**  HAL_tsens_GetPeriod
**
**  Description:
**    Gets the re-sampling rate
**
**  Parameters:
**    None
**
**  Return:
**    0x00 measures temp once
**    0x01 62.5 ms
**    0xFF 63.5625 s
**    There is a 0.25 sec increment between 0x01 - 0xFF
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetPeriod(void)
{
   return HWIO_INF(MPM2_TSENS_CTRL, MEASURE_PERIOD);
}

/* ============================================================================
**
**  HAL_tsens_GetLongestPeriod
**
**  Description:
**    Provides the longest measurement period
**
**  Parameters:
**    None
**
**  Return: Highest value supported
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetLongestPeriod(void)
{
   return HWIO_FMSK(MPM2_TSENS_CTRL, MEASURE_PERIOD) >> HWIO_SHFT(MPM2_TSENS_CTRL, MEASURE_PERIOD);
}

/*----------------------------------------------------------------------------
 * Sensor enable / disable
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_SetSensorsEnabled
**
**  Description:
**    Enable a set of sensors based on bitmask settings
**
**  Parameters:
**    uSensorMask - bit mask settings
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_SetSensorsEnabled(uint32 uSensorMask)
{
   uint32 uReg;

   uReg = HWIO_IN(MPM2_TSENS_CTRL);

   uReg &= ~(HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR0_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR1_EN) |
             HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR2_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR3_EN) |
             HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR4_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR5_EN) |
             HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR6_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR7_EN) |
             HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR8_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR9_EN) |
             HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR10_EN));

   uReg |= uSensorMask << HWIO_SHFT(MPM2_TSENS_CTRL, SENSOR0_EN) &
           (HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR0_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR1_EN) |
            HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR2_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR3_EN) |
            HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR4_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR5_EN) |
            HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR6_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR7_EN) |
            HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR8_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR9_EN) |
            HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR10_EN));

   HWIO_OUT(MPM2_TSENS_CTRL , uReg);
}

/* ============================================================================
**
**  HAL_tsens_GetEnabledSensors
**
**  Description:
**    Gets a bit mask of all enabled TSENS sensors.
**
**  Parameters:
**    None
**
**  Return: bit mask settings of enabled sensors;
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetEnabledSensors(void)
{
   uint32 uSensorMask;

   uSensorMask = HWIO_IN(MPM2_TSENS_CTRL);

   uSensorMask &= (HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR0_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR1_EN) |
                   HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR2_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR3_EN) |
                   HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR4_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR5_EN) |
                   HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR6_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR7_EN) |
                   HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR8_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR9_EN) |
                   HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR10_EN));

   uSensorMask = uSensorMask >> HWIO_SHFT(MPM2_TSENS_CTRL, SENSOR0_EN);

   return uSensorMask;
}

/*----------------------------------------------------------------------------
 * Main enable / disable
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_SetState
**
**  Description:
**    Enable or disable tsens and returned the previous state
**
**  Parameters:
**    eTsensState - HAL_TSENS_ENABLE/HAL_TSENS_DISABLE
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_SetState(HAL_tsens_State eTsensState)
{
   switch(eTsensState)
   {
      case HAL_TSENS_ENABLE:
         HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_EN, ENABLED);
         break;
      case HAL_TSENS_DISABLE:
         HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_EN, DISABLED);
         break;
      default:
         break;
   }
}

/*----------------------------------------------------------------------------
 * MTC enable / disable
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_SetMTCState
**
**  Description:
**    Enable or disable tsens MTC functionality.
**
**  Parameters:
**    eTsensState - HAL_TSENS_ENABLE_MTC/HAL_TSENS_DISABLE_MTC
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_SetMTCState(HAL_tsens_State eTsensState)
{
/*
   switch(eTsensState)
   {
      case HAL_TSENS_ENABLE_MTC:
         HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_MTC_EN, ENABLED);
         break;
      case HAL_TSENS_DISABLE_MTC:
         HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_MTC_EN, DISABLED);
         break;
      default:
         break;
   }
 */
 return;
}

/*----------------------------------------------------------------------------
 * Software reset
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_Reset
**
**  Description:
**    Resets TSENS.
**
**  Parameters:
**    None
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_Reset(void)
{
   HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_SW_RST, RESET_ASSERTED);

   HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_SW_RST, RESET_DEASSERTED);
}

/*----------------------------------------------------------------------------
 * Config
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_Init
**
**  Description:
**    Initialization of tsens.
**
**  Parameters:
**    uVal
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_Init(uint32 uVal)
{
   HWIO_OUTM(MPM2_TSENS_GLOBAL_CONFIG, HWIO_RMSK(MPM2_TSENS_GLOBAL_CONFIG), uVal);
}

/* ============================================================================
**
**  HAL_tsens_ConfigSensor
**
**  Description:
**    Initialization of tsens.
**
**  Parameters:
**    uSensor
**    uVal
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_ConfigSensor(uint32 uSensor, uint32 uVal)
{
   if (uSensor == 0)
   {
      HWIO_OUTM(MPM2_TSENS_S0_MAIN_CONFIG, HWIO_RMSK(MPM2_TSENS_S0_MAIN_CONFIG), uVal);
   }
   else
   {
      HWIO_OUTMI(MPM2_TSENS_Sn_REMOTE_CONFIG,uSensor, HWIO_RMSK(MPM2_TSENS_Sn_REMOTE_CONFIG), uVal);
   }
}

/* ============================================================================
**
**  HAL_tsens_MTC_ConfigZone
**
**  Description:
**    Configures the specified MTC zone
**
**  Parameters:
**    uZone               Zone to be configured.
**    uPSCommandTh2Viol   Command to be sent by Pulse Swallower when TH2 temp is crossed.
**    uPSCommandTh1Viol   Command to be sent by Pulse Swallower when TH1 temp is crossed.
**    uPSCommandCool      Command to be sent by Pulse Swallower when temp falls to cool zone.
**    uSensorMask         Bit mask of sensors in this zone.
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_MTC_ConfigZone(uint32 uZone, uint32 uPSCommandTh2Viol, uint32 uPSCommandTh1Viol, uint32 uPSCommandCool, uint32 uSensorMask)
{
/*
   uint32 zoneCtrlVal;

   zoneCtrlVal = HWIO_FVAL(MPM2_TSENS_MTC_ZONEn_CTRL, SENSOR_MAP, uSensorMask) |
                 HWIO_FVAL(MPM2_TSENS_MTC_ZONEn_CTRL, PS_COMMAND_COOL, uPSCommandCool) |
                 HWIO_FVAL(MPM2_TSENS_MTC_ZONEn_CTRL, PS_COMMAND_TH1_VIOLATED, uPSCommandTh1Viol) |
                 HWIO_FVAL(MPM2_TSENS_MTC_ZONEn_CTRL, PS_COMMAND_TH2_VIOLATED, uPSCommandTh2Viol) |
                 HWIO_FVAL(MPM2_TSENS_MTC_ZONEn_CTRL, ZONE_MTC_EN, 0x1);

   HWIO_OUTI(MPM2_TSENS_MTC_ZONEn_CTRL, uZone, zoneCtrlVal);
   
*/
return;
}

/* ============================================================================
**
**  HAL_tsens_MTC_SetThresholds
**
**  Description:
**    Sets the per-sensor threshold codes corresponding to TH1 and TH2 (yellow and red)
**    threshold temperatures.
**
**  Parameters:
**    uSensor      Sensor number for which threshold is to be set
**    uTh1Temp     TH1 threshold temperature (ADC code value)
**    uTh2Temp     TH2 threshold temperature (ADC code value)
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_MTC_SetThresholds(uint32 uSensor, uint32 uTh1Temp, uint32 uTh2Temp)
{
/*
   uint32 uRegVal;

   uRegVal = HWIO_INI(MPM2_TSENS_MTC_Sn_THRESHOLDS, uSensor);

   uRegVal &= ~((HWIO_FMSK(MPM2_TSENS_MTC_Sn_THRESHOLDS, TH1)) |
                (HWIO_FMSK(MPM2_TSENS_MTC_Sn_THRESHOLDS, TH2)));

   uRegVal |= HWIO_FVAL(MPM2_TSENS_MTC_Sn_THRESHOLDS, TH1, uTh1Temp) |
              HWIO_FVAL(MPM2_TSENS_MTC_Sn_THRESHOLDS, TH2, uTh2Temp);

   HWIO_OUTI(MPM2_TSENS_MTC_Sn_THRESHOLDS, uSensor, uRegVal);
*/
return;
}

/* ============================================================================
**
**  HAL_tsens_MTC_SetThresholds
**
**  Description:
**    Sets the margins for TH1 and TH2 thresholds.
**
**  Parameters:
**    uTh1Margin     TH1 margin (ADC code value)
**    uTh2Margin     TH2 margin (ADC code value)
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_MTC_SetMargins(uint32 uTh1Margin, uint32 uTh2Margin)
{
/*
   uint32 uRegVal;

   uRegVal = HWIO_IN(MPM2_TSENS_MTC_MARGIN);

   uRegVal &= ~((HWIO_FMSK(MPM2_TSENS_MTC_MARGIN, TH1_MARGIN_VALUE)) |
                (HWIO_FMSK(MPM2_TSENS_MTC_MARGIN, TH2_MARGIN_VALUE)));

   uRegVal |= HWIO_FVAL(MPM2_TSENS_MTC_MARGIN, TH1_MARGIN_VALUE, uTh1Margin) |
              HWIO_FVAL(MPM2_TSENS_MTC_MARGIN, TH2_MARGIN_VALUE, uTh2Margin);

   HWIO_OUT(MPM2_TSENS_MTC_MARGIN, uRegVal);
*/
return;
}
/* ============================================================================
**
**  HAL_tsens_MTC_ConfigZoneSwMask
**
**  Description:
**    Configures the specified MTC zone SW Mask value
**
**  Parameters:
**    uZone               Zone to be configured.
**    bTH1_Enable         TRUE/FALSE
**    bTH2_Enable         TRUE/FALSE
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_MTC_ConfigZoneSwMask(uint32 uZone, boolean bTH1_Enable, boolean bTH2_Enable)
{
/*
   if (bTH1_Enable == TRUE)
   {
      HWIO_OUTFI(MPM2_TSENS_MTC_ZONEn_SW_MASK, uZone, TH1_MTC_IN_EFFECT, 0x1);
   }
   else
   {
      HWIO_OUTFI(MPM2_TSENS_MTC_ZONEn_SW_MASK, uZone, TH1_MTC_IN_EFFECT, 0x0);
   }
   if (bTH2_Enable == TRUE)
   {
      HWIO_OUTFI(MPM2_TSENS_MTC_ZONEn_SW_MASK, uZone, TH2_MTC_IN_EFFECT, 0x1);
   }
   else
   {
      HWIO_OUTFI(MPM2_TSENS_MTC_ZONEn_SW_MASK, uZone, TH2_MTC_IN_EFFECT, 0x0);
   }
*/
return;
}
/* ============================================================================
**
**  HAL_tsens_SelectADCClkSrc
**
**  Description:
**    Sets a given threshold
**
**  Parameters:
**    eADCClkSrc - internal clk or external clk.
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_SelectADCClkSrc(HAL_tsens_ADCClkSrc eADCClkSrc)
{
   switch(eADCClkSrc)
   {
      case HAL_TSENS_INTERNAL:
         HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_ADC_CLK_SEL, INTERNAL_OSCILLATOR);
         break;
      case HAL_TSENS_EXTERNAL:
         HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_ADC_CLK_SEL, EXTERNAL_CLOCK_SOURCE);
         break;
      default:
         break;
   }
}

/* ============================================================================
**
**  HAL_tsens_GetMaxCode
**
**  Description:
**    Provides the max ADC code
**
**  Parameters:
**    None
**
**  Return: adc result
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetMaxCode(void)
{
   return 0x3FF;
}

/*----------------------------------------------------------------------------
 * Char data
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_UseRedundant
**
**  Description:
**    Determines whether or not to use the redundant fuses
**
**  Parameters:
**    None
**
**  Return: TRUE: use redundant char data
**
**  Dependencies: None
**
** ========================================================================= */
boolean HAL_tsens_UseRedundant(void)
{
   return FALSE;
}

/* ============================================================================
**
**  HAL_tsens_CalSelect
**
**  Description:
**    Determines whether there is 1) no cal data 2) single point cal data or
**    3) two point cal data
**
**  Parameters:
**    bUseRedundant
**
**  Return: HAL_tsens_Calibration (not calibrated, one point, or two point)
**
**  Dependencies: None
**
** ========================================================================= */
HAL_tsens_Calibration HAL_tsens_CalSelect(boolean bUseRedundant)
{
   return (HAL_tsens_Calibration) HWIO_INF(QFPROM_CORR_CALIB_ROW5_LSB, TSENS_CALIB);
}

/* ============================================================================
**
**  HAL_tsens_GetBaseX1
**
**  Description:
**    Gets the Tsens base for cal point 1
**
**  Parameters:
**    bUseRedundant
**
**  Return: adc code
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetBaseX1(boolean bUseRedundant)
{
   return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_LSB, TSENS_BASE) << 2) & 0x3fc);/* since TSENS_BASE0 is 8 Bit*/
}

/* ============================================================================
**
**  HAL_tsens_GetBaseX2
**
**  Description:
**    Gets the Tsens base for cal point 2
**
**  Parameters:
**    bUseRedundant
**
**  Return: adc code
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetBaseX2(boolean bUseRedundant)
{
   return ((HWIO_INF(QFPROM_CORR_CALIB_ROW5_LSB, TSENS_BASE1) << 2) & 0x3fc); /* since TSENS_BASE1 is 8 Bit*/
}

/* ============================================================================
**
**  HAL_tsens_GetPointX1
**
**  Description:
**    Gets Tsens uSensor point 1
**
**  Parameters:
**    bUseRedundant
**    uint32 uSensor
**
**  Return: adc code
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetPointX1(boolean bUseRedundant, uint32 uSensor)
{
   if (bUseRedundant == FALSE)
   {
      switch (uSensor)
      {
         case 0:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_LSB, TSENS0_POINT1) << 2) & 0xFC); /*offset is 6 bit value*/
         case 1:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_LSB, TSENS1_POINT1) << 2) & 0xFC);
         case 2:
            return
               ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_MSB, TSENS2_POINT1) << 2) & 0xFC);
         case 3:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_MSB, TSENS3_POINT1) << 2) & 0xFC);
         case 4:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW5_LSB, TSENS4_POINT1) << 2) & 0xFC);
         default:
            break;
      }
   }

   return 0; /* return 0 for sensor>TS10*/
}

/* ============================================================================
**
**  HAL_tsens_GetPointX2
**
**  Description:
**    Gets Tsens uSensor point 2
**
**  Parameters:
**    bUseRedundant
**    uint32 uSensor
**
**  Return: adc code
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetPointX2(boolean bUseRedundant, uint32 uSensor)
{
   if (bUseRedundant == FALSE)
   {
      switch (uSensor)
      {
         case 0:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_LSB, TSENS0_POINT2) << 2) & 0xFC); /*offset is 6 bit value*/
         case 1:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_LSB, TSENS1_POINT2) << 2) & 0xFC);
         case 2:
			return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_MSB, TSENS2_POINT2) << 2) & 0xFC);
         case 3:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_MSB, TSENS3_POINT2) << 2) & 0xFC);
         case 4:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW5_LSB, TSENS4_POINT2) << 2) & 0xFC);
         default:
            break;
      }
   }

   return 0; /* return 0 for sensor>TS10 */
}

/*----------------------------------------------------------------------------
 * LMh-Lite - NOT supported on 8952
 * -------------------------------------------------------------------------*/
uint32 HAL_tsens_GetDisabledCPUsMask()
{
   return 0;
}

void HAL_tsens_SetCpuIndexes(uint32 uCpuIndexes)
{
   return;
}

void HAL_tsens_SetCpuHighLowThreshold(uint32 uCpuIndex, uint32 uHighThresholdCode, uint32 uLowThresholdCode, boolean bHighEn, boolean bLowEn)
{
   return;
}

void HAL_tsens_SetCpuHighLowEnable(boolean bEnable)
{
   return;
}
