/*============================================================================
  FILE:         AdcScalingUtil.c

  OVERVIEW:  This file provides utility functions for the Analog-to-Digital
            Converter driver Board Support Package.

  DEPENDENCIES: None
 
                Copyright (c) 2008-2012, 2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.10/hwengines/adc/devices/common/src/AdcScalingUtil.c#2 $$DateTime: 2018/12/28 03:55:29 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2012-06-11  jjo  Added BSP scaling functions.
  2008-03-16  jdt  Initial revision.
============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalVAdc.h"
#include "AdcScalingUtil.h"

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

/*===========================================================================

  FUNCTION        AdcDivideWithRounding

  DESCRIPTION     Performs a division with rounding.

  DEPENDENCIES    None

  PARAMETERS      nNum [in] Numerator
                  nDen [in] Denominator

  RETURN VALUE    Result

  SIDE EFFECTS    None

===========================================================================*/
int32
AdcDivideWithRounding(
   int32 nNum,
   int32 nDen
   )
{
   int32 nOffset = nDen / 2;

   if (nNum < 0)
   {
      nOffset *= -1;
   }

   return (nNum + nOffset) / nDen;
}

/*===========================================================================

  FUNCTION        VAdc_WeightedAverage

  DESCRIPTION     Calculates a weighted average

  DEPENDENCIES    None

  PARAMETERS      nW1   [in] weight 1
                  nVal1 [in] value 1
                  nW2   [in] weight 2
                  nVal2 [in] value 2

  RETURN VALUE    Result

  SIDE EFFECTS    None

===========================================================================*/
static int32
VAdc_WeightedAverage(
   int32 nW1,
   int32 nVal1,
   int32 nW2,
   int32 nVal2
   )
{
   int32 nNum;
   int32 nDen;

   nNum = (nW1 * nVal1) + (nW2 * nVal2);
   nDen = nW1 + nW2;

   return AdcDivideWithRounding(nNum, nDen);
}

/*===========================================================================

  FUNCTION        VAdcScaleDieTemp

  DESCRIPTION     Scales the ADC result from millivolts to 0.001 degrees
                  Celsius using the PMIC thermistor conversion equation.

  DEPENDENCIES    None

  PARAMETERS      uMicrovolts [in]
                  pnPhyiscal [out]

  RETURN VALUE    AdcDeviceResultStatusType

  SIDE EFFECTS    None

===========================================================================*/
AdcDeviceResultStatusType
VAdcScaleDieTemp(
   uint32 uMicrovolts,
   int32 *pnPhysical
   )
{
   /*
    * Divide by two to convert from microvolt reading to micro-Kelvin.
    *
    * Subtract 273150 to convert the temperature from Kelvin to
    * 0.001 degrees Celsius.
    */
   *pnPhysical = AdcDivideWithRounding((int32)uMicrovolts, 2) - 273150;

   return ADC_DEVICE_RESULT_VALID;
}

/*===========================================================================

  FUNCTION        VAdcScaleChgTemp

  DESCRIPTION     Scales the ADC result from millivolts to 0.001 degrees
                  Celsius using the PMIC thermistor conversion equation.

  DEPENDENCIES    None

  PARAMETERS      uMicrovolts [in]
                  pnPhyiscal [out]

  RETURN VALUE    AdcDeviceResultStatusType

  SIDE EFFECTS    None

===========================================================================*/
AdcDeviceResultStatusType
VAdcScaleChgTemp(
   uint32 uMicrovolts,
   int32 *pnPhysical
   )
{
   *pnPhysical = 377500 - AdcDivideWithRounding((int32)uMicrovolts, 4);

   return ADC_DEVICE_RESULT_VALID;
}


/*===========================================================================

  FUNCTION        AdcMapLinearInt32toInt32

  DESCRIPTION     This function uses linear interpolation to calculate an
                  output value given an input value and a map table that maps
                  input values to output values.

  DEPENDENCIES  	Requires the ADC DAL

  PARAMETERS      paPts      [in] Mapping between sensor output voltage and the
                                  actual temp (must be ascending or descending)
                  uTableSize [in] Number of entries in paPts
                  nInput     [in] Input value
                  pnOutput   [out] Result (y) interpolated using input value and table

  RETURN VALUE   	ADC_DEVICE_RESULT_INVALID if there is an error with
                  the parameters or in performing the calculation.

                  ADC_DEVICE_RESULT_VALID if successful.

  SIDE EFFECTS   	None

===========================================================================*/
AdcDeviceResultStatusType AdcMapLinearInt32toInt32(
   const AdcMapPtInt32toInt32Type *paPts,
   uint32 uTableSize,
   int32 nInput,
   int32 *pnOutput
)
{
   DALBOOL bDescending = TRUE;
   uint32 uSearchIdx = 0;

   if ((paPts == NULL) || (pnOutput == NULL))
   {
      return ADC_DEVICE_RESULT_INVALID;
   }

   /* Check if table is descending or ascending */
   if (uTableSize > 1)
   {
      if (paPts[0].x < paPts[1].x)
      {
         bDescending = FALSE;
      }
   }

   while (uSearchIdx < uTableSize)
   {
      if ( (bDescending == TRUE) && (paPts[uSearchIdx].x < nInput) )
      {
        /* table entry is less than measured value and table is descending, stop */
        break;
      }
      else if ( (bDescending == FALSE) && (paPts[uSearchIdx].x > nInput) )
      {
        /* table entry is greater than measured value and table is ascending, stop */
        break;
      }
      else
      {
        uSearchIdx++;
      }
   }

   if (uSearchIdx == 0)
   {
      *pnOutput = paPts[0].y;
   }
   else if (uSearchIdx == uTableSize)
   {
      *pnOutput = paPts[uTableSize-1].y;
   }
   else
   {
      /* result is between search_index and search_index-1 */
      /* interpolate linearly */
      *pnOutput = (
               ( (int32)
                   (
                    (paPts[uSearchIdx].y - paPts[uSearchIdx-1].y)
                     * (nInput - paPts[uSearchIdx-1].x)
                   )
                   / (paPts[uSearchIdx].x - paPts[uSearchIdx-1].x)
               )
               + paPts[uSearchIdx-1].y
             );
   }
   return ADC_DEVICE_RESULT_VALID;
}

/*===========================================================================

  FUNCTION        VAdcScaleThermistor

  DESCRIPTION     Scaled an ADC code to physical units for a thermistor.

  DEPENDENCIES    None

  PARAMETERS      uCode [in] ADC reading
                  uVrefN [in] ADC code of GND
                  uVrefP [in] ADC code of VREF
                  uPullUp [in] thermistor pull-up
                  paMap [in] interpolation table
                  uTableSize [in] interpolation table size
                  pnPhysical [out] calibrated physical value

  RETURN VALUE    ADC_DEVICE_RESULT_VALID or an error

  SIDE EFFECTS    None

===========================================================================*/
AdcDeviceResultStatusType
VAdcScaleThermistor(
   uint32 uCode,
   uint32 uVrefN,
   uint32 uVrefP,
   uint32 uPullUp,
   const AdcMapPtInt32toInt32Type *paMap,
   uint32 uTableSize,
   int32 *pnPhysical
   )
{
   int64 llCode = uCode;
   int64 llVrefN = uVrefN;
   int64 llVrefP = uVrefP;
   int64 llRp = uPullUp;
   int64 llNum;
   int64 llDenom;
   int32 nRt;
   AdcDeviceResultStatusType resultStatus;

   /*
    *
    *                    C2 ___  (Vdd)
    *                          |
    *                          |
    *                          >
    *                      Rp  < (pull up)
    *                          >
    *                          |
    *                          |
    *                          |- - - Cin
    *                          |
    *                          |
    *                          >
    *                      Rt  < (thermistor)
    *                          >
    *                          |
    *                          |
    *                    C1 ---  (gnd)
    *
    * Voltage divider equation:
    *     Cin - C1 = (C2 - C1) * Rt / (Rp + Rt)
    *
    * Solving for Rt:
    *     Rt = (Cin - C1) * Rp / (C2 - Cin)
    *
    */
   if (llCode < llVrefN)
   {
      llCode = llVrefN;
   }
   else if (llCode > llVrefP)
   {
      llCode = llVrefP;
   }

   llNum = (llCode - llVrefN) * llRp;
   llDenom = llVrefP - llCode;

   if (llDenom == 0)
   {
      nRt = 0x7fffffff;
   }
   else
   {
      nRt = (int32)(llNum / llDenom);
   }

   resultStatus = AdcMapLinearInt32toInt32(paMap,
                                           uTableSize,
                                           nRt,
                                           pnPhysical);

   return resultStatus;
}


/*===========================================================================

  FUNCTION        VAdcBlendResult

  DESCRIPTION     Blends results for two channels. The physical value is
                  blended using a weighted average. The remaining values
                  are blended using averaging.

  DEPENDENCIES    None

  PARAMETERS      nPhysical   [in] neutral reading
                  pLowResult  [in] result accurate at low physical values
                  nLowLimit   [in] anything below is 100% low result
                  pHighResult [in] result accurate at high physical values
                  nHighLimit  [in] anything above is 100% high result
                  pBlendedResult [out] blended result

  RETURN VALUE    None

  SIDE EFFECTS    None

===========================================================================*/
void
VAdcBlendResult(
   int32 nPhysical,
   AdcDeviceResultType *pLowResult,
   int32 nLowLimit,
   AdcDeviceResultType *pHighResult,
   int32 nHighLimit,
   AdcDeviceResultType *pBlendedResult
   )
{
   int32 nLowWeight;
   int32 nHighWeight;
   int32 nNum;
   int32 nDen;

   pBlendedResult->eStatus = ADC_DEVICE_RESULT_VALID;

   if (nPhysical <= nLowLimit)
   {
      pBlendedResult->nPhysical = pLowResult->nPhysical;
      pBlendedResult->uMicrovolts = pLowResult->uMicrovolts;
      pBlendedResult->uPercent = pLowResult->uPercent;
      pBlendedResult->uCode = pLowResult->uCode;
      return;
   }
   else if (nPhysical >= nHighLimit)
   {
      pBlendedResult->nPhysical = pHighResult->nPhysical;
      pBlendedResult->uMicrovolts = pHighResult->uMicrovolts;
      pBlendedResult->uPercent = pHighResult->uPercent;
      pBlendedResult->uCode = pHighResult->uCode;
      return;
   }

   nNum = 1000 * (nPhysical - nLowLimit);
   nDen = nHighLimit - nLowLimit;
   nHighWeight = AdcDivideWithRounding(nNum, nDen);
   nLowWeight = 1000 - nHighWeight;

   pBlendedResult->nPhysical = VAdc_WeightedAverage(nLowWeight,
                                                    pLowResult->nPhysical,
                                                    nHighWeight,
                                                    pHighResult->nPhysical);

   /* To avoid overflows, just average these results */
   pBlendedResult->uMicrovolts = (pLowResult->uMicrovolts + pHighResult->uMicrovolts) / 2;
   pBlendedResult->uPercent = (pLowResult->uPercent + pHighResult->uPercent) / 2;
   pBlendedResult->uCode = (pLowResult->uCode + pHighResult->uCode) / 2;

   return;
}



