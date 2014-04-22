/*============================================================================
  FILE:         DalVAdc.c

  OVERVIEW:     Implementation of a Physical ADC device DAL for the
                PMIC VADC peripheral.

  DEPENDENCIES: None

                Copyright (c) 2009-2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.tx/6.0/hwengines/adc/devices/vadc/src/DalVAdc.c#1 $$DateTime: 2020/01/30 22:49:35 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2018-12-28  ay   updated for MDM9205
  2017-05-01  Ks   Work around for UE crashing while camping to LTE N/W
  2015-01-12  jjo  Updated scaling methods.
  2014-01-27  jjo  Temperature correction update.
  2013-11-21  jjo  Enable NPA latency request.
  2013-07-17  jjo  Support for multiple VADCs.
  2013-05-22  jjo  Remove work loop.
  2013-02-26  jjo  Added TM APIs.
  2013-02-19  jjo  Added inverse scaling functions.
  2012-10-16  jjo  Updated SPMI APIs.
  2012-10-02  jjo  Added MPP support.
  2012-07-31  jjo  Now checks the ADC type
  2012-07-09  jjo  ADC conversions are now performed entirely in a workloop.
  2012-06-11  jjo  VADC support.
  2012-04-20  jdt  Support for VADC peripheral.
  2012-03-05  gps  Fixed scaling bug in ScaleResult3
  2012-02-09  gps  Added Qxdm logging
  2011-10-26  gps  Requests using sequencer now actually make request
  2010-10-19  jjo  Resolved integral promotion problem by using a cast in
                   VAdc_ScaleResult
  2010-05-25  jdt  Initial revision based on DALVAdc.c#10

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalVAdc.h"
#include "DALDeviceId.h"
#include "VAdcDiag.h"
#include <qurt_timer.h>
#include <stdlib.h>
#include "timer.h"

#ifdef VADC_UNIT_TEST
#include "VAdcSim.h"
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define VADC_DEFAULT_STACK_SIZE 0x2000
#define VADC_SPMI_READ_NUM_RETRIES 5
#define ADC_UV_PER_MV 1000

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*======================================================================

  FUNCTION        VAdcScaleCodeToPercent

  DESCRIPTION     Scales the ADC result from code to percent (0 to
                  0xffff).

  DEPENDENCIES    None

  PARAMETERS
      uFullScale_code   [in]  full-scale ADC code
      uCode        [in]  the raw ADC code
      puPercent    [out] the value in percent (0 to 0xffff)

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
static void
VAdcScaleCodeToPercent(
   uint32 uFullScale_code,
   uint32 uCode,
   uint32 *puPercent
   )
{
   /* Note: max ADC code before overflow is 0x10000, which is far beyond
      the full scale reading of 0x4000 */
   *puPercent = (uCode * 0xffff) / uFullScale_code;

   return;
}

/*======================================================================

  FUNCTION        VAdcScaleCodeToMicrovolts

  DESCRIPTION     Scales the ADC result from code to microvolts.

  DEPENDENCIES    None

  PARAMETERS
      pScaling         [in] AMUX scaling
      uFullScale_code  [in] full-scale ADC code
      uFullScale_uV    [in] full-scale uV value
      uCode        [in]  the raw ADC code
      puMicrovolts [out] the value in microvolts

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
static void
VAdcScaleCodeToMicrovolts(
   const VAdcChannelScalingFactor *pScaling,
   uint32 uFullScale_code,
   uint32 uFullScale_uV,
   uint32 uCode,
   uint32 *puMicrovolts
   )
{
   uint64 u64Code = uCode;
   uint64 u64Microvolts;

   u64Microvolts = (u64Code * uFullScale_uV) / uFullScale_code;

   *puMicrovolts = (uint32)u64Microvolts;

   /*
    * Since PMIC AMUX channels have a prescalar gain applied to the input,
    * we need to scale input by inverse of the channel prescalar gain to
    * obtain the actual input voltage.
    */
   *puMicrovolts *= pScaling->uDenominator;
   *puMicrovolts /= pScaling->uNumerator;

   return;
}


static AdcDeviceResultStatusType
VAdcScaleCodeToResistance(
   uint32 uFullScale_code,
   uint32 uPullUp,
   uint32 uCode,
   int32 *pnResistance
   )
{
   int64 n64FS = uFullScale_code;
   int64 n64PU = uPullUp;
   int64 n64Code = uCode;
   int64 n64R;

   if (uCode == uFullScale_code)
   {
      /* Avoid division by zero */
      *pnResistance = 0x7fffffff;
      return ADC_DEVICE_RESULT_VALID;
   }

   /* Apply voltage divider equation */
   n64R = (n64PU * n64Code) / (n64FS - n64Code);

   if (n64R > 0x7fffffff)
   {
      /* Avoid overflow */
      *pnResistance = 0x7fffffff;
      return ADC_DEVICE_RESULT_VALID;
   }

   *pnResistance = (int32)n64R;

   return ADC_DEVICE_RESULT_VALID;
}
/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
/*======================================================================

  FUNCTION        VAdc_ReadBytes

  DESCRIPTION     This function reads from the VAdc peripheral

  DEPENDENCIES    Need to call DalDeviceOpen for SPMI first

  PARAMETERS
      pCtxt            [in]  pointer to the HAL interface context
      uOffset          [in]  the starting address
      pucData          [out] the bytes read
      uDataLen         [in]  the number of bytes to read
      puTotalBytesRead [out] the number of bytes read

  RETURN VALUE    VADC_HAL_SUCCESS or an error code

  SIDE EFFECTS    None

======================================================================*/
static VAdcHalResultType
VAdc_ReadBytes(
   void *pCtxt,
   uint32 uOffset,
   unsigned char *pucData,
   uint32 uDataLen,
   uint32 *puTotalBytesRead
   )
{
   VAdcHalInterfaceCtxtType *pVAdcHalInterfaceCtxt;
   VAdcDevCtxt *pDevCtxt;
   uint32 uRegisterAddress;
   uint32 i;
#ifndef VADC_UNIT_TEST
   DALResult result;
#endif

   if (pCtxt == NULL)
   {
      return VADC_HAL_ERROR;
   }

   pVAdcHalInterfaceCtxt = (VAdcHalInterfaceCtxtType *)pCtxt;
   pDevCtxt = pVAdcHalInterfaceCtxt->pDevCtxt;

   uRegisterAddress = uOffset & 0xFF;
   uRegisterAddress |= (pDevCtxt->pBsp->uPeripheralID << 8) & 0xFF00;

   for (i = 0; i < VADC_SPMI_READ_NUM_RETRIES; i++)
	  {

          result = DalSpmi_ReadLong(pDevCtxt->phSpmiDev, 
                                    pDevCtxt->pBsp->uSlaveId,
                                    pDevCtxt->pBsp->eAccessPriority,
                                    uRegisterAddress,  
                                    pucData,  
                                    uDataLen,  
                                    puTotalBytesRead);

		  if (result != DAL_SUCCESS)
          {
             VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - SPMI read failed, try again",
                    FALSE);
             continue;
          }

		  return VADC_HAL_SUCCESS;
   	  }

   
      VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - SPMI read failed repeatedly",
                    TRUE);
      return VADC_HAL_ERROR;
  

}

/*======================================================================

  FUNCTION        VAdc_WriteBytes

  DESCRIPTION     This function writes to the VAdc peripheral

  DEPENDENCIES    Need to call DalDeviceOpen for SPMI first

  PARAMETERS
      pCtxt            [in] pointer to the HAL interface context
      uOffset          [in] the starting address
      pucData          [in] the bytes to write
      uDataLen         [in] the number of bytes to write

  RETURN VALUE    VADC_HAL_SUCCESS or an error code

  SIDE EFFECTS    None

======================================================================*/
static VAdcHalResultType
VAdc_WriteBytes(
   void *pCtxt,
   uint32 uOffset,
   unsigned char *pucData,
   uint32 uDataLen
   )
{
   VAdcHalInterfaceCtxtType *pVAdcHalInterfaceCtxt;
   VAdcDevCtxt *pDevCtxt;
   uint32 uRegisterAddress;
   DALResult result;


   if (pCtxt == NULL)
   {
      return VADC_HAL_ERROR;
   }

   pVAdcHalInterfaceCtxt = (VAdcHalInterfaceCtxtType *)pCtxt;
   pDevCtxt = pVAdcHalInterfaceCtxt->pDevCtxt;

   uRegisterAddress = uOffset & 0xFF;
   uRegisterAddress |= (pDevCtxt->pBsp->uPeripheralID << 8) & 0xFF00;


   result = DalSpmi_WriteLong(pDevCtxt->phSpmiDev, 
                              pDevCtxt->pBsp->uSlaveId,
                              pDevCtxt->pBsp->eAccessPriority,
                              uRegisterAddress,
                              pucData,
                              uDataLen);

   if (result != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - SPMI write failed", TRUE);
      return VADC_HAL_ERROR;
   }

   return VADC_HAL_SUCCESS;
}



/*======================================================================

  FUNCTION        VAdc_StartConversionInternal

  DESCRIPTION     This function starts the ADC conversion

  DEPENDENCIES    None

  PARAMETERS
      pDevCtxt          [in] pointer to the VADC device context
      pChConfig         [in] the channel configuration
      puConvTime_us     [out] conversion time

  RETURN VALUE    DAL_SUCCESS or an error code

  SIDE EFFECTS    DAL_SUCCESS if conversion was started successfully
                  Otherwise, returns a DAL error code.

======================================================================*/
static DALResult VAdc_StartConversionInternal(VAdcDevCtxt *pDevCtxt,
                                              const VAdcChannelConfigType *pChConfig,
                                              uint32 *puConvTime_us)
{
   
   VAdcHalResultType result;
   VAdcConversionParametersType conversionParams;
  

   /*
    * Initialize conversion parameter variables.
    */
   conversionParams.uChannel = pChConfig->uAdcHardwareChannel;
   conversionParams.eSettlingDelay = pChConfig->eSettlingDelay;
   conversionParams.eDecimationRatio = pChConfig->eDecimationRatio;
   conversionParams.eFastAverageMode = pChConfig->eFastAverageMode;
   conversionParams.eCalMethod = pChConfig->eCalMethod;

 
   

   /*
    * Need to reset the EOC events and claim ownership prior to starting the conversion
    */
   /* EOC signal */
   //SIVA
   //ret = DALSYS_EventCtrl(pDevCtxt->ahEocEvent[VADC_EVENT_DEFAULT], DALSYS_EVENT_CTRL_RESET);
   //if (ret != DAL_SUCCESS)
   //{
   //   return ret;
   //}

   ///* EOC timeout */
   //ret = DALSYS_EventCtrl(pDevCtxt->ahEocEvent[VADC_EVENT_TIMEOUT], DALSYS_EVENT_CTRL_RESET);
   //if (ret != DAL_SUCCESS)
   //{
   //   return ret;
   //}

   /*
    * Request an ADC conversion by asserting REQ field of the arbiter control
    * register.
    */
   result = VAdcHalRequestConversion(&pDevCtxt->iVAdcHalInterface, &conversionParams, puConvTime_us);
   if (result != VADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   return DAL_SUCCESS;
}

/*======================================================================

  FUNCTION        VAdc_WaitForEoc

  DESCRIPTION
      This function waits for the EOC and gets the conversion status.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt     [in]
      pChConfig    [in]
      uConvTime_us [in]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      Be careful not to make the wait time too small or EOC may
      time out too early.

======================================================================*/
static DALResult VAdc_WaitForEoc(VAdcDevCtxt *pDevCtxt, const VAdcChannelConfigType *pChConfig, uint32 uConvTime_us)
{
   uint32 uReadTimeout_us = pDevCtxt->pBsp->uReadTimeout_us;
   uint32 uReadTime_us;
   VAdcStatusType convStatus;
   VAdcHalResultType status;
   uint32 uTimeWaited_us;

   uReadTime_us = uConvTime_us + pDevCtxt->pBsp->uLDOSettlingTime_us;
   

   if (pDevCtxt->pBsp->bUsesInterrupts == TRUE)
   {
      /* Wait for the EOC signal */
      // SIVA
      //result = DALSYS_EventMultipleWait(pDevCtxt->ahEocEvent,
      //                                  _VADC_NUM_EVENTS,
      //                                  uReadTimeout_us,
      //                                  &uEventIdx);

      //if (result != DAL_SUCCESS && result != DAL_ERROR_TIMEOUT)
      //{
      //   return result;
      //}

      //if (result == DAL_ERROR_TIMEOUT)
      //{
      //   /* Log missed interrupt */
      //   VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - missed interrupt", FALSE);
      //   VAdc_LogDumpRegisters(&pDevCtxt->vAdcDebug, &pDevCtxt->iVAdcHalInterface);
      //}

      /* Check for EOC */
      status = VAdcHalGetStatus(&pDevCtxt->iVAdcHalInterface, &convStatus);
      if (status != VADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }
   
      if (convStatus.eConversionStatus == VADC_CONVERSION_STATUS_COMPLETE)
      {
         VAdc_LogEOC(&pDevCtxt->vAdcDebug);
         return DAL_SUCCESS;
      }
   }
   else
   {
      uTimeWaited_us = 0;

      while (uTimeWaited_us < uReadTimeout_us)
      {
         /* Wait for the EOC timeout */
         //SIVA
         //result = DALSYS_EventMultipleWait(pDevCtxt->ahEocEvent,
         //                                  _VADC_NUM_EVENTS,
         //                                  uReadTime_us,
         //                                  &uEventIdx);

         //if (result != DAL_SUCCESS && result != DAL_ERROR_TIMEOUT)
         //{
         //   return result;
         //}

        //   DALSYS_BusyWait	while (qurt_sysclock_get_hw_ticks() < currentTime + waitInterval);
		 timer_sleep(uReadTime_us, T_USEC, TRUE);

         status = VAdcHalGetStatus(&pDevCtxt->iVAdcHalInterface, &convStatus);
         if (status != VADC_HAL_SUCCESS)
         {
            return DAL_ERROR;
         }

         if (convStatus.eConversionStatus == VADC_CONVERSION_STATUS_COMPLETE)
         {
            VAdc_LogEOC(&pDevCtxt->vAdcDebug);
            return DAL_SUCCESS;
         }

         if (uTimeWaited_us == 0)
         {
            uTimeWaited_us += uReadTime_us;

            /* Only wait the full read time for the first try and
               then poll at a faster rate */
            uReadTime_us = uReadTime_us / 5;
         }
         else
         {
            uTimeWaited_us += uReadTime_us;
         }
      }
   }

   /* Conversion did not complete */
   VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - conversion timed out", FALSE);
   VAdc_LogDumpRegisters(&pDevCtxt->vAdcDebug, &pDevCtxt->iVAdcHalInterface);

   return DAL_ERROR;
}

/*======================================================================

  FUNCTION        VAdc_ReadAdcCode

  DESCRIPTION
      This function reads the ADC result.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt [in]
      puCode   [out]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None

======================================================================*/
static DALResult
VAdc_ReadAdcCode(
   VAdcDevCtxt *pDevCtxt,
   uint32 *puCode
   )
{
   VAdcHalResultType result;
   uint32 uCode;

   result = VAdcHalGetConversionCode(&pDevCtxt->iVAdcHalInterface, &uCode);
   if (result != VADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   if (uCode == VADC_INVALID_ADC_CODE)
   {
      /* After every peripheral disable the data registers are set to an invalid code */
      VAdc_LogError(&pDevCtxt->vAdcDebug,"VAdc - invalid ADC code read", FALSE);
      return DAL_ERROR;
   }
   else if (((int16)uCode) < 0)
   {
      /* Code is 16-bit signed and could read negative when close to zero */
      *puCode = 0;
   }
   else
   {
      *puCode = uCode;
   }

   return DAL_SUCCESS;
}

/*======================================================================

  FUNCTION        VAdc_ReadAdc

  DESCRIPTION
      This function reads an ADC channel.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt    [in]
      pChConfig   [in]
      puCode      [out]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None

======================================================================*/
static DALResult VAdc_ReadAdc(VAdcDevCtxt *pDevCtxt, const VAdcChannelConfigType *pChConfig, uint32 *puCode)
{
   VAdcHalResultType status;
   DALResult result;
   uint32 uConvTime_us;


   /* Start the conversion */
   result = VAdc_StartConversionInternal(pDevCtxt,
                                         pChConfig,
                                         &uConvTime_us);
   if (result != DAL_SUCCESS)
   {
      goto error;
   }

   /* Wait for the conversion to finish */
   result = VAdc_WaitForEoc(pDevCtxt, pChConfig, uConvTime_us);
   if (result != DAL_SUCCESS)
   {
      goto error;
   }

   /* Read the ADC code */
   result = VAdc_ReadAdcCode(pDevCtxt, puCode);
   if (result != DAL_SUCCESS)
   {
      goto error;
   }

#ifdef VADC_DUMP_REGISTERS_AFTER_READS
   VAdc_LogDumpRegisters(&pDevCtxt->vAdcDebug, &pDevCtxt->iVAdcHalInterface);
#endif

   /* Disable vadc */
   status = VAdcHalSetEnable(&pDevCtxt->iVAdcHalInterface, VADC_DISABLE);
   if (status != VADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   return DAL_SUCCESS;

error:
   VAdc_LogDumpRegisters(&pDevCtxt->vAdcDebug, &pDevCtxt->iVAdcHalInterface);
   (void)VAdcHalSetEnable(&pDevCtxt->iVAdcHalInterface, VADC_DISABLE);
   return result;
}



/*======================================================================

  FUNCTION        VAdc_ReadAdcChannel

  DESCRIPTION
      This function reads an ADC channel and returns the scaled result
      and status of the read. It is not meant for reading calibration
      channels.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt         [in]
      uChannel         [in]
      pAdcDeviceResult [out]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None

======================================================================*/
static DALResult
VAdc_ReadAdcChannel(
   VAdcDevCtxt *pDevCtxt,
   uint32 uChannel,
   AdcDeviceResultType *pAdcDeviceResult
   )
{
   DALResult result;
   uint32 uCode;
   const VAdcChannelConfigType *pChConfig = &pDevCtxt->pBsp->paChannels[uChannel];
   


   /* Read the ADC */
   result = VAdc_ReadAdc(pDevCtxt, 
                         pChConfig, 
                         &uCode);

   if (result != DAL_SUCCESS)
   {
      goto error;
   }

   /* Scale the result */
	VAdc_ProcessConversionResult(pDevCtxt,
                                 uChannel,
                                 pChConfig,
                                 uCode,
                                 pAdcDeviceResult);

   

   VAdc_LogConversionResult(&pDevCtxt->vAdcDebug,
                            pDevCtxt->pBsp->paChannels[uChannel].pName,
                            pAdcDeviceResult);

   vadc_diag_send_client_log(uChannel,
                             pAdcDeviceResult,
                             (uint32)pChConfig->eDecimationRatio);

   return DAL_SUCCESS;

error:
   pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_INVALID;

   return DAL_SUCCESS;
}

/*======================================================================

  FUNCTION        VAdc_InterruptCb

  DESCRIPTION
      This function is the callback to process the conversion complete
      interrupt from the arbiter. It sets the EOC signal.

  DEPENDENCIES    None

  PARAMETERS
      pArg            [in]
      uIntrStatusMask [in]

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
#ifndef VADC_UNIT_TEST
static void* VAdc_InterruptCb(
   void *pArg,
   uint32 uIntrStatusMask
   )
{
   VAdcHalResultType status;
   VAdcDevCtxt *pDevCtxt = (VAdcDevCtxt *)pArg;

   VAdc_LogInterrupt(&pDevCtxt->vAdcDebug);

   /* Clear all pending interrupts */
   status = VAdcHalClearInterrupts(&pDevCtxt->iVAdcHalInterface, VADC_INTERRUPT_MASK_BIT_EOC);
   if (status != VADC_HAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - failed to clear EOC interrupt", FALSE);
   }

   /* Set the EOC signal */
   //SIVA
   //result = DALSYS_EventCtrlEx(pDevCtxt->ahEocEvent[VADC_EVENT_DEFAULT],
   //                            DALSYS_EVENT_CTRL_TRIGGER,
   //                            0,
   //                            NULL,
   //                            0);

   return NULL;
}
#endif

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*======================================================================

  FUNCTION        VAdc_GetHardwareChannel

  DESCRIPTION     This function gets the hardware channel for a given
                  channel index.

  DEPENDENCIES    None

  PARAMETERS
      pDevCtxt     [in]  pointer to the current device context
      uChannelIdx  [in]  channel index

  RETURN VALUE    HW channel

  SIDE EFFECTS    None

======================================================================*/
uint32
VAdc_GetHardwareChannel(
   VAdcDevCtxt *pDevCtxt,
   uint32 uChannelIdx
   )
{
   return pDevCtxt->pBsp->paChannels[uChannelIdx].uAdcHardwareChannel;
}





/*======================================================================

  FUNCTION        VAdc_ProcessConversionResult

  DESCRIPTION     This function is called at the end of conversion to process
                  the result. The raw result is converted into a scaled 16-bit
                  result and then and the scaled result is converted into
                  physical units in millivolts. Further scaling may be
                  performed by the scaling function defined in the BSP.

  DEPENDENCIES    None

  PARAMETERS
      pDevCtxt   [in]  pointer to the current device context
      uChannel   [in]  current channel index
      pChannel   [in]  current channel config
      uCode      [in]  raw ADC code
      pResult    [out] ADC result

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
void VAdc_ProcessConversionResult(VAdcDevCtxt *pDevCtxt,
   uint32 uChannel,
   const VAdcChannelConfigType *pChannel,
   uint32 uCode,
   AdcDeviceResultType *pResult)
{
   
   pResult->eStatus = ADC_DEVICE_RESULT_VALID;
   pResult->uCode = uCode;
   

   VAdcScaleCodeToPercent(pDevCtxt->pBsp->uFullScale_code,
                          uCode,
                          &pResult->uPercent);

   VAdcScaleCodeToMicrovolts(&pChannel->scalingFactor,
                             pDevCtxt->pBsp->uFullScale_code,
                             pDevCtxt->pBsp->uFullScale_uV,
                             uCode,
                             &pResult->uMicrovolts);

   /* By default, scale the physical result in units of millivolts */
   pResult->nPhysical = AdcDivideWithRounding((int32)pResult->uMicrovolts, ADC_UV_PER_MV);

   /* Call the BSP scaling function (if present) */
   switch (pChannel->eScalingMethod)
   {
      case VADC_SCALE_TO_MILLIVOLTS:
         /* by default, nPhysical is in millivolts */
         break;
      case VADC_SCALE_DIE_TEMP_TO_MILLIDEGREES:
         pResult->eStatus = VAdcScaleDieTemp(pResult->uMicrovolts,
                                             &pResult->nPhysical);
         break;
      case VADC_SCALE_CHG_TEMP_TO_MILLIDEGREES:
         pResult->eStatus = VAdcScaleChgTemp(pResult->uMicrovolts,
                                             &pResult->nPhysical);
         break;
      case VADC_SCALE_THERMISTOR:
         pResult->eStatus = VAdcScaleThermistor(pResult->uCode,
                                                0,
                                                pDevCtxt->pBsp->uFullScale_code,
                                                pChannel->uPullUp,
                                                pChannel->pIntTable->pTable,
                                                pChannel->pIntTable->uLength,
                                                &pResult->nPhysical);
         break;
      case VADC_SCALE_INTERPOLATE_FROM_MILLIVOLTS:
         pResult->eStatus = AdcMapLinearInt32toInt32(pChannel->pIntTable->pTable,
                                                     pChannel->pIntTable->uLength,
                                                     pResult->nPhysical,
                                                     &pResult->nPhysical);
         break;
      case VADC_SCALE_RESISTOR_DIVIDER:
         pResult->eStatus = VAdcScaleCodeToResistance(pDevCtxt->pBsp->uFullScale_code,
                                                      pChannel->uPullUp,
                                                      uCode,
                                                      &pResult->nPhysical);
         break;
      default:
         /* By default, nPhysical is in millivolts */
         break;
   }

   return;
}



  

 
















/*----------------------------------------------------------------------------
 * The following functions are for DALDriver specific functionality
 * -------------------------------------------------------------------------*/
DALResult
VAdc_DriverInit(VAdcDrvCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

DALResult
VAdc_DriverDeInit(VAdcDrvCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

/*----------------------------------------------------------------------------
 * The following functions are declared in DalDevice Interface.
 * -------------------------------------------------------------------------*/
DALResult
VAdc_DeviceInit(VAdcClientCtxt *pCtxt)
{
   DALResult ret;
   VAdcHalResultType result;
   DALSYSPropertyVar propVar;
   VAdcInterruptsConfigType interruptConfig;
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;
   uint32 uMode;
   uint32 uChannelIdx;
   uint32 uPerphId;
   const VAdcChannelConfigType *pChConfig;
   pm_err_flag_type pmResult;
   DALBOOL bGpioFound;
   uint32 uGpioMappingIdx;
   const VAdcGpioChannelMappingType *pGpioChannelMapping;
   uint32 uGpioChannel;
   const VAdcAveragedChannelConfigType *pAveragedChannelCfg;
   VAdcAveragedChannelType *pAveragedChannel;
  

   /* Initialize VADC context */
   pDevCtxt->bHardwareSupported = TRUE;
   
   pDevCtxt->ahEocEvent[VADC_EVENT_DEFAULT] = NULL;
   pDevCtxt->ahEocEvent[VADC_EVENT_TIMEOUT] = NULL;
   pDevCtxt->pBsp = NULL;

   ret = VAdc_LogInit(&pDevCtxt->vAdcDebug, pDevCtxt->DevId);
   if (ret != DAL_SUCCESS)
   {
      return ret;
   }

   /* Read the DAL properties - hProp populated in device attach */
   ret = DALSYS_GetPropertyValue(pDevCtxt->hProp, "VADC_BSP", 0, &propVar);

   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - failed to get BSP", TRUE);
      return ret;
   }

   pDevCtxt->pBsp = (VAdcBspType *)propVar.Val.pStruct;

   /* Get the PMIC device info */
   pmResult = pm_get_pmic_info((uint8)pDevCtxt->pBsp->uPmicDevice, &pDevCtxt->pmicDeviceInfo);
   if (pmResult != PM_ERR_FLAG__SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - failed to get PMIC device info", TRUE);
      return DAL_ERROR;
   }

   /* Create NPA clients */
   pDevCtxt->hNPACpuLatency = npa_create_sync_client("/core/cpu/latency/usec", "VADC", NPA_CLIENT_REQUIRED);
   pDevCtxt->hNPAPmicHkadc = NULL;

#ifndef VADC_UNIT_TEST
   /* Attach to to the SPMI driver */
   ret = DAL_SpmiDeviceAttach("DALDEVICEID_SPMI_DEVICE", &pDevCtxt->phSpmiDev);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - failed to attach to SPMI", TRUE);
      return ret;
   }
#endif

   uMode = DAL_OPEN_SHARED | DAL_OPEN_READ | DAL_OPEN_WRITE;
   ret = VAdc_Open(pCtxt, uMode);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - failed to open VAdc in init", TRUE);
      return ret;
   }

   /* Initialize the HAL interface */
   pDevCtxt->VAdcHalInterfaceCtxt.pDevCtxt = pDevCtxt;
   pDevCtxt->iVAdcHalInterface.pCtxt = (void *)&pDevCtxt->VAdcHalInterfaceCtxt;
   pDevCtxt->iVAdcHalInterface.pfnWriteBytes = VAdc_WriteBytes;
   pDevCtxt->iVAdcHalInterface.pfnReadBytes = VAdc_ReadBytes;

   

   // Log the revision & peripheral type
   result = VAdcHalGetRevisionInfo(&pDevCtxt->iVAdcHalInterface, &pDevCtxt->revisionInfo);
   if (result != VADC_HAL_SUCCESS)
   {
      ret = DAL_ERROR;
      goto errorClose;
   }

   pDevCtxt->iVAdcHalInterface.revisionInfo = pDevCtxt->revisionInfo;

   
  

   if (pDevCtxt->bHardwareSupported == FALSE)
   {
      ret = DAL_SUCCESS;
      goto errorClose;
   }

   if (pDevCtxt->pBsp->bUsesInterrupts == TRUE)
   {
#ifndef VADC_UNIT_TEST
      /* Register for the ADC peripheral ISR */
      uPerphId = ((pDevCtxt->pBsp->uSlaveId << 8) & 0xF00) | (pDevCtxt->pBsp->uPeripheralID & 0xFF);

      ret = DalSpmi_RegisterIsr(pDevCtxt->phSpmiDev, 
                                uPerphId,
                                (uint32)VADC_INTERRUPT_MASK_BIT_EOC,
                                VAdc_InterruptCb,
                                pDevCtxt);
      if (ret != DAL_SUCCESS)
      {
         VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - failed to register ISR", TRUE);
         goto errorClose;
      }
#endif

      /* Configure peripheral interrupts */
      interruptConfig.eEocInterruptConfig = VADC_INTERRUPT_CONFIG_RISING_EDGE;
      

      result = VAdcHalConfigInterrupts(&pDevCtxt->iVAdcHalInterface, &interruptConfig);
      if (result != VADC_HAL_SUCCESS)
      {
         ret = DAL_ERROR;
         goto errorClose;
      }

      result = VAdcHalSetInterruptMid(&pDevCtxt->iVAdcHalInterface,
                                      (VAdcInterruptMid)pDevCtxt->pBsp->uMasterID);
      if (result != VADC_HAL_SUCCESS)
      {
         ret = DAL_ERROR;
         goto errorClose;
      }

      /* Clear all pending interrupts */
      result = VAdcHalClearInterrupts(&pDevCtxt->iVAdcHalInterface, VADC_INTERRUPT_MASK_BIT_EOC);
      if (result != VADC_HAL_SUCCESS)
      {
         ret = DAL_ERROR;
         goto errorClose;
      }

      result = VAdcHalEnableInterrupts(&pDevCtxt->iVAdcHalInterface, VADC_INTERRUPT_MASK_BIT_EOC);
      if (result != VADC_HAL_SUCCESS)
      {
         ret = DAL_ERROR;
         goto errorClose;
      }
   }

   /* Configure the EOC event & timeout event */
   // SIVA
   //ret = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT,
   //                         &pDevCtxt->ahEocEvent[VADC_EVENT_DEFAULT],
   //                         NULL);
   //if (ret != DAL_SUCCESS)
   //{
   //   VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - failed to create the EOC event", TRUE);
   //   goto errorClose;
   //}

   //ret = DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMEOUT_EVENT,
   //                         &pDevCtxt->ahEocEvent[VADC_EVENT_TIMEOUT],
   //                         NULL);
   //if (ret != DAL_SUCCESS)
   //{
   //   VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - failed to create the EOC timeout event", TRUE);

   //   goto errorClose;
   //}

//#ifndef VADC_UNIT_TEST
//   /* Map static MPPs */
//   for (uChannelIdx = 0; uChannelIdx < pDevCtxt->pBsp->uNumChannels; uChannelIdx++)
//   {
//      pChConfig = &pDevCtxt->pBsp->paChannels[uChannelIdx];
//
//      if (pChConfig->eMppConfig == VADC_CHANNEL_MPP_CONFIG_STATIC)
//      {
//         if (pChConfig->eMpp == PM_MPP_INVALID ||
//             pChConfig->eChSelect == PM_MPP__AIN__CH_INVALID)
//         {
//            VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - invalid MPP configuration", TRUE);
//            ret = DAL_ERROR;
//            goto errorClose;
//         }
//
//         pmResult = pm_dev_mpp_config_analog_input((uint8)pDevCtxt->pBsp->uPmicDevice,
//                                                   pChConfig->eMpp,
//                                                   pChConfig->eChSelect);
//         if (pmResult != PM_ERR_FLAG__SUCCESS)
//         {
//            VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - failed to configure static MPP", TRUE);
//            ret = DAL_ERROR;
//            goto errorClose;
//         }
//      }
//   }
//#endif

      /* Map static GPIOs */
   for (uChannelIdx = 0; uChannelIdx < pDevCtxt->pBsp->uNumChannels; uChannelIdx++)
   {
      pChConfig = &pDevCtxt->pBsp->paChannels[uChannelIdx];
      bGpioFound = FALSE;

      for (uGpioMappingIdx = 0; uGpioMappingIdx < pDevCtxt->pBsp->uNumGpioChannelMappings; uGpioMappingIdx++)
      {
         pGpioChannelMapping = &pDevCtxt->pBsp->paGpioChannelMappings[uGpioMappingIdx];

         for (uGpioChannel = 0; uGpioChannel < pGpioChannelMapping->uNumChannels; uGpioChannel++)
         {
            if (pChConfig->uAdcHardwareChannel == pGpioChannelMapping->paucChannels[uGpioChannel])
            {
               bGpioFound = TRUE;
               break;
            }
         }

         if (bGpioFound)
         {
            pm_gpio_perph_index eGpio = pGpioChannelMapping->eGpio;

            pmResult = pm_gpio_set_ext_pin_config(pDevCtxt->pBsp->uPmicDevice,
                                                  eGpio,
                                                  PM_GPIO_EXT_PIN_DISABLE);
            if (pmResult != PM_ERR_FLAG__SUCCESS)
            {
               VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - failed to configure static GPIO", TRUE);
               return DAL_ERROR;
            }

            break;
         }
      }
   }


   /* Configure averaged channels */
   pDevCtxt->uTotalNumChannels = pDevCtxt->pBsp->uNumChannels;


   if (pDevCtxt->pBsp->puNumAveragedChannels != NULL && *pDevCtxt->pBsp->puNumAveragedChannels != 0)
   {
      pDevCtxt->paAveragedChannels = malloc(sizeof(VAdcAveragedChannelType) * *pDevCtxt->pBsp->puNumAveragedChannels);
                          
      if (pDevCtxt->paAveragedChannels == NULL)
      {
         VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - could not allocate memory for averaged channels", TRUE);
         return ret;
      }

      for (uChannelIdx = 0; uChannelIdx < *pDevCtxt->pBsp->puNumAveragedChannels; uChannelIdx++)
      {
         pAveragedChannelCfg = &pDevCtxt->pBsp->paAveragedChannels[uChannelIdx];
         pAveragedChannel = &pDevCtxt->paAveragedChannels[uChannelIdx];

         pAveragedChannel->eState = VADC_AVERAGED_CHANNEL_STATE_UNKNOWN;

         ret = VAdc_GetChannel(pCtxt,
                               pAveragedChannelCfg->pszLowName,
                               &pAveragedChannel->uLowChannel);
         if (ret != DAL_SUCCESS)
         {
            VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - low averaged channel not found", TRUE);
            return ret;
         }

         ret = VAdc_GetChannel(pCtxt,
                               pAveragedChannelCfg->pszMedName,
                               &pAveragedChannel->uMedChannel);
         if (ret != DAL_SUCCESS)
         {
            VAdc_LogError(&pDevCtxt->vAdcDebug,"VAdc - med averaged channel not found", TRUE);
            return ret;
         }
         ret = VAdc_GetChannel(pCtxt,
                               pAveragedChannelCfg->pszHighName,
                               &pAveragedChannel->uHighChannel);
         if (ret != DAL_SUCCESS)
         {
            VAdc_LogError(&pDevCtxt->vAdcDebug,"VAdc - high averaged channel not found", TRUE);
            return ret;
         }
      }

      pDevCtxt->uTotalNumChannels += *pDevCtxt->pBsp->puNumAveragedChannels;
   }
   
   

   ret = VAdc_Close(pCtxt);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - could not close VADC in init", TRUE);
      return ret;
   }

   return DAL_SUCCESS;

errorClose:
   (void)VAdc_Close(pCtxt);
   return ret;
}

DALResult
VAdc_DeviceDeInit(VAdcClientCtxt *pCtxt)
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;
   DALResult result;
   DALResult retResult = DAL_SUCCESS;

#ifndef VADC_UNIT_TEST
   if (pDevCtxt->phSpmiDev != NULL)
   {
      if (pDevCtxt->pBsp->bUsesInterrupts == TRUE)
      {
         result = DalSpmi_UnregisterIsr(pDevCtxt->phSpmiDev,
                                        ((pDevCtxt->pBsp->uSlaveId << 8) & 0xF00) | (pDevCtxt->pBsp->uPeripheralID & 0xFF),
                                        (uint32)VADC_INTERRUPT_MASK_BIT_EOC);
         if (result != DAL_SUCCESS)
         {
            retResult = DAL_ERROR;
         }
      }

      result = DAL_DeviceDetach(pDevCtxt->phSpmiDev);
      if (result != DAL_SUCCESS)
      {
         retResult = DAL_ERROR;
      }
      pDevCtxt->phSpmiDev = NULL;
   }
#endif

   //if (pDevCtxt->ahEocEvent[VADC_EVENT_DEFAULT] != NULL)
   //{
   //   result = DALSYS_DestroyObject(&pDevCtxt->ahEocEvent[VADC_EVENT_DEFAULT]);
   //   if (result != DAL_SUCCESS)
   //   {
   //      retResult = DAL_ERROR;
   //   }
   //   pDevCtxt->ahEocEvent[VADC_EVENT_DEFAULT] = NULL;
   //}

   //if (pDevCtxt->ahEocEvent[VADC_EVENT_TIMEOUT] != NULL)
   //{
   //   result = DALSYS_DestroyObject(&pDevCtxt->ahEocEvent[VADC_EVENT_TIMEOUT]);
   //   if (result != DAL_SUCCESS)
   //   {
   //      retResult = DAL_ERROR;
   //   }
   //   pDevCtxt->ahEocEvent[VADC_EVENT_TIMEOUT] = NULL;
   //}



   return retResult;
}

DALResult
VAdc_Open(
   VAdcClientCtxt *pCtxt,
   uint32 dwaccessMode
   )
{
   DALResult ret;
   uint32 uMode;
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   if (pDevCtxt->bHardwareSupported == FALSE)
   {
      return DAL_SUCCESS;
   }

   if (pDevCtxt->hNPACpuLatency != NULL)
   {
      npa_issue_required_request(pDevCtxt->hNPACpuLatency, 1);
   }

   if (pDevCtxt->hNPAPmicHkadc != NULL)
   {
      npa_issue_required_request(pDevCtxt->hNPAPmicHkadc, PMIC_NPA_MODE_ID_HKADC_ACTIVE);
   }

#ifndef VADC_UNIT_TEST
   uMode = DAL_OPEN_SHARED | DAL_OPEN_READ | DAL_OPEN_WRITE;
   ret = DalDevice_Open(pDevCtxt->phSpmiDev, uMode);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - failed to open SPMI", TRUE);
      return ret;
   }
#endif

   return DAL_SUCCESS;
}

DALResult
VAdc_Close(VAdcClientCtxt *pCtxt)
{
   DALResult ret = DAL_SUCCESS;
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   if (pDevCtxt->bHardwareSupported == FALSE)
   {
      return DAL_SUCCESS;
   }

#ifndef VADC_UNIT_TEST
   ret = DalDevice_Close(pDevCtxt->phSpmiDev);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, "VAdc - warning - could not close SPMI", FALSE);
   }
#endif

   if (pDevCtxt->hNPAPmicHkadc != NULL)
   {
      npa_issue_required_request(pDevCtxt->hNPAPmicHkadc, PMIC_NPA_MODE_ID_HKADC_OFF);
   }

   if (pDevCtxt->hNPACpuLatency != NULL)
   {
      npa_complete_request(pDevCtxt->hNPACpuLatency);
   }

   return ret;
}

/*----------------------------------------------------------------------------
 * The following functions are extended in DalAdcDevice Interface.
 *
 * These functions are documented in DDIAdcDevice.h
 * -------------------------------------------------------------------------*/
DALResult
VAdc_GetDeviceProperties(
   VAdcClientCtxt *pCtxt,
   AdcDeviceDevicePropertiesType *pAdcDeviceProp
   )
{
     VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;
   
	  if (pDevCtxt->bHardwareSupported)
	  {
		 pAdcDeviceProp->uNumChannels = pDevCtxt->uTotalNumChannels;
	  }
	  else
	  {
		 pAdcDeviceProp->uNumChannels = 0;
	  }

   return DAL_SUCCESS;
}

DALResult
VAdc_GetChannel(
   VAdcClientCtxt *pCtxt,
   const char *pszChannelName,
   uint32 *puChannelIdx
   )
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;
   uint32 uNumChannels;
   uint32 uChannel;
   const char *pszCurrentChannelName;

   if (pDevCtxt->bHardwareSupported == FALSE)
   {
      return DAL_ERROR;
   }

   uNumChannels = pDevCtxt->pBsp->uNumChannels;

   for (uChannel = 0; uChannel < uNumChannels; uChannel++)
   {
      pszCurrentChannelName = pDevCtxt->pBsp->paChannels[uChannel].pName;

      if (strcmp(pszChannelName, pszCurrentChannelName) == 0)
      {
         *puChannelIdx = uChannel;
         return DAL_SUCCESS;
      }
   }

   
	  if (pDevCtxt->pBsp->puNumAveragedChannels != NULL)
	  {
		 for (uChannel = 0; uChannel < *pDevCtxt->pBsp->puNumAveragedChannels; uChannel++)
		 {
			pszCurrentChannelName = pDevCtxt->pBsp->paAveragedChannels[uChannel].pszName;
   
			if (strcmp(pszChannelName, pszCurrentChannelName) == 0)
			{
			   *puChannelIdx = uChannel + uNumChannels;
			   return DAL_SUCCESS;
			}
		 }
	  }

   
   return DAL_ERROR;
}


DALResult
VAdc_ReadChannel(
   VAdcClientCtxt *pCtxt,
   uint32 uChannelIdx,
   AdcDeviceResultType *pAdcDeviceResult
   )
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;
   DALResult result;

   const VAdcAveragedChannelConfigType *pAveragedChannelCfg;
   VAdcAveragedChannelType *pAveragedChannel;
   AdcDeviceResultType lowResult;
   AdcDeviceResultType medResult;
   AdcDeviceResultType highResult;


   pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_INVALID;

   if (!pDevCtxt->bHardwareSupported)
   {
      return DAL_ERROR;
   }

   if (uChannelIdx >= pDevCtxt->uTotalNumChannels)
   {
      pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_INVALID_CHANNEL;
      return DAL_ERROR;
   }

   VAdc_LogConversionRequest(&pDevCtxt->vAdcDebug, uChannelIdx);


   if (uChannelIdx >= pDevCtxt->pBsp->uNumChannels)
   {
      /* Averaged channel reading */
      pAveragedChannelCfg = &pDevCtxt->pBsp->paAveragedChannels[uChannelIdx - pDevCtxt->pBsp->uNumChannels];
      pAveragedChannel = &pDevCtxt->paAveragedChannels[uChannelIdx - pDevCtxt->pBsp->uNumChannels];

      /* The medium channel is always read first. Decisions about when to transition
       * need to be made with the same channel. Otherwise, jumps can happen, as well
       * as getting different values going from low-to-high versus high-to-low */
      result = VAdc_ReadAdcChannel(pDevCtxt, pAveragedChannel->uMedChannel, &medResult);
      if (result != DAL_SUCCESS || medResult.eStatus != ADC_DEVICE_RESULT_VALID)
      {
         return DAL_ERROR;
      }

      if (medResult.nPhysical > pAveragedChannelCfg->nHighToMed)
      {
         result = VAdc_ReadAdcChannel(pDevCtxt, pAveragedChannel->uHighChannel, &highResult);
         if (result != DAL_SUCCESS || highResult.eStatus != ADC_DEVICE_RESULT_VALID)
         {
            return result;
         }

         if (medResult.nPhysical < pAveragedChannelCfg->nMedToHigh)
         {
            pAveragedChannel->eState = VADC_AVERAGED_CHANNEL_STATE_MED_HIGH;
            VAdcBlendResult(medResult.nPhysical,
                            &medResult,
                            pAveragedChannelCfg->nHighToMed,
                            &highResult,
                            pAveragedChannelCfg->nMedToHigh,
                            pAdcDeviceResult);

            VAdc_LogConversionResult(&pDevCtxt->vAdcDebug,
                                     pAveragedChannelCfg->pszName,
                                     pAdcDeviceResult);
         }
         else
         {
            pAveragedChannel->eState = VADC_AVERAGED_CHANNEL_STATE_HIGH;
            *pAdcDeviceResult = highResult;
         }
      }
      else if (medResult.nPhysical < pAveragedChannelCfg->nLowToMed)
      {
         result = VAdc_ReadAdcChannel(pDevCtxt, pAveragedChannel->uLowChannel, &lowResult);
         if (result != DAL_SUCCESS || lowResult.eStatus != ADC_DEVICE_RESULT_VALID)
         {
            return result;
         }

         if (medResult.nPhysical > pAveragedChannelCfg->nMedToLow)
         {
            pAveragedChannel->eState = VADC_AVERAGED_CHANNEL_STATE_LOW_MED;
            VAdcBlendResult(medResult.nPhysical,
                            &lowResult,
                            pAveragedChannelCfg->nMedToLow,
                            &medResult,
                            pAveragedChannelCfg->nLowToMed,
                            pAdcDeviceResult);

            VAdc_LogConversionResult(&pDevCtxt->vAdcDebug,
                                     pAveragedChannelCfg->pszName,
                                     pAdcDeviceResult);
         }
         else
         {
            pAveragedChannel->eState = VADC_AVERAGED_CHANNEL_STATE_LOW;
            *pAdcDeviceResult = lowResult;
         }
      }
      else
      {
         pAveragedChannel->eState = VADC_AVERAGED_CHANNEL_STATE_MED;
         *pAdcDeviceResult = medResult;
      }
   }
   else
   {

      /* Normal channel reading */
      result = VAdc_ReadAdcChannel(pDevCtxt, uChannelIdx, pAdcDeviceResult);
      if (result != DAL_SUCCESS)
      {
         return result;
      }

   }


   return result;
}


