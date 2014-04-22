/*============================================================================
  FILE:         TsensLoader.c

  OVERVIEW:     Implementation of TSENS supporting XBL loader.

  DEPENDENCIES: None

                Copyright (c) 2014-2016, 2018 Qualcomm Technologies, Inc.
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
  2018-12-14  AY   updated to support MDM9205
  2017-02-17  PC   Disabling WD, Cycle monitor interrupts
  2016-12-14  jjo  Add watchdog support.
  2016-04-12  jjo  Add support for truncated offsets.
  2016-03-08  jjo  Memory enhancements.
  2016-03-03  jjo  Add TBCB mapping.
  2015-06-18  jjo  Add PWM enable.
  2014-07-21  jjo  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "TsensLib.h"
#include "HALtsens.h"
#include "TsensBsp.h"
#include "DALSys.h"
#include "DDIChipInfo.h"
#include "TsensiConversion.h"
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
   const TsensBspType *pTsensBsp;
   int32 nLastTempDegC;
   uint32 uLastSensor;
   int32 nThresholdMax;
   int32 nThresholdMin;
} TsensDevCtxtType;

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
int32 gnTsensLastTemp = 0x7FFFFFFF;
uint32 guTsensLastSensor = 0;
TsensDevCtxtType gTsensDevCtxt = {0};

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static boolean bTsensInitialized = FALSE;   /* Flag to determine if Tsens is initialized */
static boolean bFirstReadDone = FALSE;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/* ============================================================================
**
**  Tsens_MTC_Init
**
**  Description:
**    Initializes TSENS MTC in the boot.
**
**  Parameters:
**    None
**
**  Return:
**    TsensResultType
**
**  Dependencies:
**    None
**
** ========================================================================= */
static TsensResultType Tsens_MTC_Init()
{
   uint32 i, uTh1Code = 0, uTh2Code = 0;
   uint32 uTh1Margin, uTh2Margin;
   uint32 uTh1MaxMargin = 0, uTh2MaxMargin = 0;
   uint32 uSens, uSensMask;
   const TsensMTCConfigType *pMTCConfig;
   const TsensSensorCfgType *pSensor;
   TsensResultType result;
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp; 

   // Set the threshold temperature for all sensors
   for (i = 0; i < pBsp->uNumSensors; ++i)
   {
      pSensor = &pBsp->paSensorCfgs[i];
      result = Tsensi_ConvertFromDegC(i, pSensor->uMTCThreshold1Temp, &uTh1Code);
      if (result != TSENS_SUCCESS)
      {
         return result;
      }
      result = Tsensi_ConvertFromDegC(i, pSensor->uMTCThreshold2Temp, &uTh2Code);
      if (result != TSENS_SUCCESS)
      {
         return result;
      }
      HAL_tsens_MTC_SetThresholds(i, uTh1Code, uTh2Code);
   }

   //
   // Calculate highest code among all the sensors for the threshold margins...this is to avoid
   // cases where some senors may have low code value for the threshold margin and so could
   // result in applying the zone change (from red to yellow or yellow to cool) action too soon.
   //

   for (i = 0; i < pBsp->uNumMTCZones; ++i)
   {
      pMTCConfig = &pBsp->paMTCConfig[i];
      uSensMask = pMTCConfig->uSensorMask;
      uSens = 0;

      while (uSensMask)
      {
         if (uSensMask & 0x1)
         {
            pSensor = &pBsp->paSensorCfgs[uSens];
            result = Tsensi_ConvertFromDegC(uSens, pSensor->uMTCThreshold1Temp-pBsp->uMTCThreshold1Margin, &uTh1Margin);
            if (result != TSENS_SUCCESS)
            {
               return result;
            }
            result = Tsensi_ConvertFromDegC(uSens, pSensor->uMTCThreshold1Temp, &uTh1Code);
            if (result != TSENS_SUCCESS)
            {
               return result;
            }
            if (uTh1MaxMargin < uTh1Code-uTh1Margin)
            {
               uTh1MaxMargin = uTh1Code-uTh1Margin;
            }

            result = Tsensi_ConvertFromDegC(uSens, pSensor->uMTCThreshold2Temp-pBsp->uMTCThreshold2Margin, &uTh2Margin);
            if (result != TSENS_SUCCESS)
            {
               return result;
            }
            result = Tsensi_ConvertFromDegC(uSens, pSensor->uMTCThreshold2Temp, &uTh2Code);
            if (result != TSENS_SUCCESS)
            {
               return result;
            }
            if (uTh2MaxMargin < uTh2Code-uTh2Margin)
            {
               uTh2MaxMargin = uTh2Code-uTh2Margin;
            }
         }
         uSens++;
         uSensMask = (uSensMask >> 1);
      }
   }
   uTh2Code=0;
   uTh1Code=0;
   for (i = 0; i < pBsp->uNumMTCZones; ++i)
   {
      pMTCConfig = &pBsp->paMTCConfig[i];
      uSensMask = pMTCConfig->uSensorMask;
      uSens = 0;

      while (uSensMask)
      {
         if (uSensMask & 0x1)
         {
            pSensor = &pBsp->paSensorCfgs[uSens];

            result = Tsensi_ConvertFromDegC(uSens, pSensor->uMTCThreshold1Temp, &uTh1Code);
            if (result != TSENS_SUCCESS)
            {
               return result;
            }
            result = Tsensi_ConvertFromDegC(uSens, pSensor->uMTCThreshold2Temp, &uTh2Code);
            if (result != TSENS_SUCCESS)
            {
               return result;
            }
            if (uTh2Code-uTh2MaxMargin <= uTh1Code-uTh1MaxMargin)
            {
               uTh1MaxMargin = 0;
               uTh2MaxMargin = 0;
               goto SET_MARGIN;
            }
         }
         uSens++;
         uSensMask = (uSensMask >> 1);
      }
   }

SET_MARGIN:
   // Set margins
   HAL_tsens_MTC_SetMargins(uTh1MaxMargin, uTh2MaxMargin);

   // Configure and enable MTC zones
   for (i = 0; i < pBsp->uNumMTCZones; ++i)
   {
      pMTCConfig = &pBsp->paMTCConfig[i];
      HAL_tsens_MTC_ConfigZone(i, pMTCConfig->uPSCommandTh2Viol, pMTCConfig->uPSCommandTh1Viol, pMTCConfig->uPSCommandCool, pMTCConfig->uSensorMask);
   }

   HAL_tsens_SetMTCState(HAL_TSENS_ENABLE_MTC);
   for (i = 0; i < pBsp->uNumMTCZones; ++i)
   {
      pMTCConfig = &pBsp->paMTCConfig[i];
      HAL_tsens_MTC_ConfigZoneSwMask(i, pMTCConfig->bIsTH1Enabled, pMTCConfig->bIsTH2Enabled);	  
   }

   return TSENS_SUCCESS;
}


/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  Tsens_Init
**
**  Description:
**    Initializes TSENS in the boot.
**
**  Parameters:
**    None
**
**  Return:
**    TsensResultType
**
**  Dependencies:
**    None
**
** ========================================================================= */
TsensResultType Tsens_Init(void)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hTsensBootProperties);
   const char *pszTsensBspPropName = "TSENS_BSP";
   const TsensBspType *pBsp;
   const TsensSensorCfgType *pSensor;
   uint32 uSensor;
   uint32 uCode;
   DALSYSPropertyVar propertyVar;
   DALResult status;
   TsensResultType result;
   DalChipInfoFamilyType eChipFamily;
   uint32 uSensorEnableMask;

   if (bTsensInitialized == TRUE)
   {
      return TSENS_SUCCESS;
   }
   
   /* Get the BSP */
   eChipFamily = DalChipInfo_ChipFamily();
   
   if (eChipFamily == DALCHIPINFO_FAMILY_MSM8997)
   {
      pszTsensBspPropName = "TSENS_BSP_8997";
   }
   else if (eChipFamily == DALCHIPINFO_FAMILY_SDM630)
   {
      pszTsensBspPropName = "TSENS_BSP_SDM630";
   }

   status = DALSYS_GetDALPropertyHandleStr("/core/hwengines/tsens", hTsensBootProperties);
   if (status != DAL_SUCCESS)
   {
      return TSENS_ERROR;
   }

   status = DALSYS_GetPropertyValue(hTsensBootProperties,
                                    pszTsensBspPropName,
                                    0,
                                    &propertyVar);
   if (status != DAL_SUCCESS)
   {
      return TSENS_ERROR;
   }

   pBsp = (TsensBspType *)propertyVar.Val.pStruct;
   gTsensDevCtxt.pTsensBsp = pBsp;
   
   uSensorEnableMask = 0;
   for (uSensor = 0; uSensor < pBsp->uNumSensors; uSensor++)
   {
      uSensorEnableMask |= 1 << uSensor;
   }

   /* Initialize calibration */
   Tsensi_CalibrationInit(pBsp->paSensorCfgs,
                          pBsp->uNumSensors,
                          pBsp->nCalPoint1DegC, //should pass temp in deg c
                          pBsp->nCalPoint2DegC);

   /* Configure & start Tsens */
   HAL_tsens_SetState(HAL_TSENS_DISABLE);
   HAL_tsens_Reset();
   HAL_tsens_Init(pBsp->uGlobalConfig);

   /* Configure the sensor & set the critical thresholds */
   for (uSensor = 0; uSensor < pBsp->uNumSensors; uSensor++)
   {
      pSensor = &pBsp->paSensorCfgs[uSensor];

      /* Sensor configuration */
      HAL_tsens_ConfigSensor(uSensor, pSensor->uTsensConfig);

      /* Critical threshold configuration */
      if (pSensor->nsThresholdMinDegC != TSENS_THRESHOLD_DISABLED)
      {
         result = Tsensi_ConvertFromDegC(uSensor, pSensor->nsThresholdMinDegC, &uCode);
         if (result != TSENS_SUCCESS)
         {
            return result;
         }

         HAL_tsens_SetThreshold(HAL_TSENS_MIN_LIMIT_TH, uSensor, uCode);
         HAL_tsens_EnableInterrupt(HAL_TSENS_MIN_LIMIT_TH, uSensor);
      }
      else
      {
         HAL_tsens_DisableInterrupt(HAL_TSENS_MIN_LIMIT_TH, uSensor);
      }

      if (pSensor->nsThresholdMaxDegC != TSENS_THRESHOLD_DISABLED)
      {
         result = Tsensi_ConvertFromDegC(uSensor, pSensor->nsThresholdMaxDegC, &uCode);
         if (result != TSENS_SUCCESS)
         {
            return result;
         }

         HAL_tsens_SetThreshold(HAL_TSENS_MAX_LIMIT_TH, uSensor, uCode);
         HAL_tsens_EnableInterrupt(HAL_TSENS_MAX_LIMIT_TH, uSensor);
      }
      else
      {
         HAL_tsens_DisableInterrupt(HAL_TSENS_MAX_LIMIT_TH, uSensor);
      }
   }

   HAL_tsens_SetPeriod(pBsp->uPeriod);
   HAL_tsens_SelectADCClkSrc(HAL_TSENS_INTERNAL);
   HAL_tsens_SetSensorsEnabled(uSensorEnableMask);

   if (pBsp->bIsMTCSupported)
   {
      result = Tsens_MTC_Init();
      if (result != TSENS_SUCCESS)
      {
         return result;
      }
   }

   HAL_tsens_SetState(HAL_TSENS_ENABLE);

   bTsensInitialized = TRUE;
   bFirstReadDone = FALSE;

   return TSENS_SUCCESS;
}

/* ============================================================================
**
**  Tsens_GetTemp
**
**  Description:
**    Gets the current temperature of the sensor.
**
**  Parameters:
**    uSensor [in]: sensor index
**    pnDegC [out]: sensor temperature in degrees C
**
**  Return:
**    TsensResultType
**
**  Dependencies:
**    Tsens_Init must be called once prior to calling this function.
**
** ========================================================================= */
TsensResultType Tsens_GetTemp(uint32 uSensor, int32 *pnDegC)
{
   DALBOOL bMeasComplete = FALSE;
   uint32 uTimerCnt;
   uint32 uAdcCode = 0;
   TsensResultType result;
   uint32 uWaitTime_us;
   uint32 uAdcCodeTry1;
   uint32 uAdcCodeTry2;
   uint32 uAdcCodeTry3;
   boolean bValid;
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;

   if (pnDegC == NULL)
   {
      return TSENS_ERROR;
   }

   if (bTsensInitialized != TRUE)
   {
      return TSENS_ERROR_NOT_INITIALIZED;
   }

   if (uSensor >= pBsp->uNumSensors)
   {
      return TSENS_ERROR;
   }
   
   /* Need to delay until TSENS performs the first reads */
   if (bFirstReadDone == FALSE)
   {
      uWaitTime_us = pBsp->uSensorConvTime_us * pBsp->uNumSensors;

      for (uTimerCnt = 0; uTimerCnt < pBsp->uNumGetTempRetries; uTimerCnt++)
      {
         if (HAL_tsens_TempMeasurementIsComplete() == TRUE)
         {
            bMeasComplete = TRUE;
            break;
         }
         DALSYS_BusyWait(uWaitTime_us);
      }

      if (bMeasComplete == FALSE)
      {
         if (HAL_tsens_TempMeasurementIsComplete() == FALSE)
         {
            return TSENS_ERROR_TIMEOUT;
         }
      }

      bFirstReadDone = TRUE;
   }

   /* Read the ADC code */
   bValid = HAL_tsens_GetSensorPrevTemp(uSensor, &uAdcCodeTry1);
   if (bValid == TRUE)
   {
      uAdcCode = uAdcCodeTry1;
   }
   else
   {
      bValid = HAL_tsens_GetSensorPrevTemp(uSensor, &uAdcCodeTry2);
      if (bValid == TRUE)
      {
         uAdcCode = uAdcCodeTry2;
      }
      else
      {
         bValid = HAL_tsens_GetSensorPrevTemp(uSensor, &uAdcCodeTry3);
         if (bValid == TRUE)
         {
            uAdcCode = uAdcCodeTry3;
         }
         else if (uAdcCodeTry1 == uAdcCodeTry2)
         {
            uAdcCode = uAdcCodeTry1;
         }
         else if (uAdcCodeTry2 == uAdcCodeTry3)
         {
            uAdcCode = uAdcCodeTry2;
         }
         else
         {
            uAdcCode = uAdcCodeTry1;
         }
      }
   }

   /* Scale the ADC code to physical units */
   result = Tsensi_ConvertToDegC(uSensor, uAdcCode, pnDegC);

   if (result == TSENS_SUCCESS)
   {
      gnTsensLastTemp = *pnDegC;
      guTsensLastSensor = uSensor;
   }

   return result;
}

/* ============================================================================
**
**  Tsens_GetNumSensors
**
**  Description:
**    Gets the number of sensors. Note that the sensor index is zero-based, i.e.
**    uSensor = {0 to *puNumSensors - 1}
**
**  Parameters:
**    puNumSensors [out]: number of sensors
**
**  Return:
**    TsensResultType
**
**  Dependencies:
**    Tsens_Init must be called once prior to calling this function.
**
** ========================================================================= */
TsensResultType Tsens_GetNumSensors(uint32 *puNumSensors)
{
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;
   if (puNumSensors == NULL)
   {
      return TSENS_ERROR;
   }

   if (bTsensInitialized != TRUE)
   {
      return TSENS_ERROR_NOT_INITIALIZED;
   }

   *puNumSensors = pBsp->uNumSensors;

   return TSENS_SUCCESS;
}

TsensResultType Tsens_GetMaxTemp(int32 *pnMaxDegC)
{
   //const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;
   int32 nMaxDegC = 120;
   //DALBOOL bFirst = TRUE;
  // uint8 *pucTsensTMAddr;
  // int32 nDeciDegC;
  // uint32 uSensorID;

   if (pnMaxDegC == NULL)
   {
      return TSENS_ERROR;
   }

   if (!bTsensInitialized)
   {
      return TSENS_ERROR_NOT_INITIALIZED;
   }

  /* for (uController = 0; uController < pBsp->uNumControllers; uController++)
   {
      //pucTsensTMAddr = gTsensDevCtxt.paControllers[uController].pucTsensTMAddr;

      //HAL_tsens_tm_GetMaxTemp(pucTsensTMAddr, &uSensorID, &nDeciDegC);

      if (bFirst)
      {
         nMaxDeciDegC = nDeciDegC;
         bFirst = FALSE;
      }
      else
      {
         if (nDeciDegC > nMaxDeciDegC)
         {
            nMaxDeciDegC = nDeciDegC;
         }
      }
   }*/

   *pnMaxDegC = nMaxDegC;

   return TSENS_SUCCESS;
}