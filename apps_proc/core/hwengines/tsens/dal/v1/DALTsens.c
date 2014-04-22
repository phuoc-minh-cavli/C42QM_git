/*============================================================================
  FILE:         TsensDevice.c

  OVERVIEW:     Implementation of the TSENS device library

  DEPENDENCIES: None

                Copyright (c) 2012-2015, 2019 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.tx/6.0/hwengines/tsens/dal/v1/DALTsens.c#1 $$DateTime: 2020/01/30 22:49:35 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2019-05-08  ay   CR:2440855 changes
  2015-01-29  jjo  Remove critical thresholds.
  2014-07-02  jjo  Added parameter check for uSensor.
  2014-06-10  jjo  Get base addresses from HWIO.
  2013-10-08  jjo  Ported to 9x35.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALTsens.h"
#include "HALtsens.h"
#include "TsensiConversion.h"



/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
extern uint8 *gpuMpm2MpmBase;
extern uint8 *gpuSecCtrlBase;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static TsensDeviceDataType tsensDeviceData;
TsensDeviceDataType *pDeviceData = NULL;
static uint32 tsensWorkloopPriority = 0;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static int32 minval(int32 a, int32 b)
{
   return (a < b) ? a: b;
}

static int32 maxval(int32 a, int32 b)
{
   return (a > b) ? a: b;
}

static void TsensLock(void)
{
   (void)qurt_mutex_lock(&pDeviceData->hSync);
}

static void TsensUnLock(void)
{
   (void)qurt_mutex_unlock(&pDeviceData->hSync);
}

static DALResult Tsens_MapHWIORegion(const char *szBase,
                                     uint8 **ppuVirtAddr)
{
   DALResult status;

   status = DalHWIO_MapRegion(pDeviceData->phHWIO,
                              szBase,
                              ppuVirtAddr);

   return status;
}

static DALResult Tsens_UpdateThresholds(void)
{
   HAL_tsens_ThresholdLevel halThresholdType;
   uint32 uClient;
   uint32 uAdcCode;
   int32 nDegC;
   int32 thresholdTemp;
   DALBOOL bThresholdEnabled;
   DALResult status = DAL_SUCCESS;
   uint32 uSensor;
   uint32 uThresholdIdx;

   if (pDeviceData->eDeviceState != TSENS_DEVICE_STATE_ISR)
   {
      HAL_tsens_DisableUpperLowerInterrupt();
   }

   for (uSensor = 0; uSensor < pDeviceData->uNumSensors; uSensor++)
   {
      // Disable interrupts (also clears them)
      HAL_tsens_DisableInterrupt(HAL_TSENS_LOWER_LIMIT_TH, uSensor);
      HAL_tsens_DisableInterrupt(HAL_TSENS_UPPER_LIMIT_TH, uSensor);
   }

   for (uThresholdIdx = 0; uThresholdIdx < TSENS_NUM_THRESHOLD_IDXS; uThresholdIdx++)
   {
      switch (uThresholdIdx)
      {
         case TSENS_THRESHOLD_UPPER_IDX:
            halThresholdType = HAL_TSENS_UPPER_LIMIT_TH;
            break;

         case TSENS_THRESHOLD_LOWER_IDX:
            halThresholdType = HAL_TSENS_LOWER_LIMIT_TH;
            break;

         default:
            return TSENS_ERROR_INVALID_PARAMETER;
      }

      for (uSensor = 0; uSensor < pDeviceData->uNumSensors; uSensor++)
      {
         bThresholdEnabled = FALSE;
         nDegC = TSENS_INVALID_THRESHOLD;

         for (uClient = 0; uClient < _TSENS_NUM_CLIENTS; uClient++)
         {
            if (pDeviceData->clients[uClient].bRegistered == TRUE)
            {
               if ((pDeviceData->clients[uClient].eThresholdState[uSensor][uThresholdIdx] == TSENS_THRESHOLD_STATE_ENABLED)
                  && ((pDeviceData->clients[uClient].bEnableThresholds == TRUE) || uThresholdIdx >= _TSENS_NUM_THRESHOLDS))
               {
                  bThresholdEnabled = TRUE;
                  thresholdTemp = pDeviceData->clients[uClient].nThresholdTemp[uSensor][uThresholdIdx];
                  if (nDegC == TSENS_INVALID_THRESHOLD)
                  {
                     nDegC = thresholdTemp;
                  }
                  else if (uThresholdIdx == TSENS_THRESHOLD_UPPER)
                  {
                     nDegC = minval(nDegC, thresholdTemp);
                  }
                  else
                  {
                     nDegC = maxval(nDegC, thresholdTemp);
                  }
               }
            }
         }

         if (bThresholdEnabled)
         {
            status = Tsensi_ConvertFromDegC(uSensor, nDegC, &uAdcCode);
            HAL_tsens_SetThreshold(halThresholdType, uSensor, uAdcCode);
            HAL_tsens_EnableInterrupt(halThresholdType, uSensor);
         }
      }
   }

   if (pDeviceData->eDeviceState != TSENS_DEVICE_STATE_ISR)
   {
      HAL_tsens_EnableUpperLowerInterrupt();
   }

   return status;
}

static DALResult Tsens_GetTempAndCodeNoLock(uint32 uSensor, int32 *pnTempDegC, uint32 *puCode)
{
   DALResult status;
   uint32 uAdcCode;
   uint32 uAdcCodeTry1;
   uint32 uAdcCodeTry2;
   uint32 uAdcCodeTry3;
   boolean bValid;

   if ((1 << uSensor) & pDeviceData->uSensorsEnabledMask)
   {
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

      *puCode = uAdcCode;

      /* Scale the ADC code to physical units */
      status = Tsensi_ConvertToDegC(uSensor, uAdcCode, pnTempDegC);
   }
   else
   {
      status = TSENS_ERROR_UNSUPPORTED;
   }

   return status;
}

static void * Tsens_ThresholdInterruptIsr(void *pArg)
{
   //TsensDevCtxt *pDevCtxt = (TsensDevCtxt *)pArg;

   // TODO: Trigger DSR
   //(void)DALSYS_EventCtrlEx(pDeviceData->hTsensInterruptEvent,
   //                         DALSYS_EVENT_CTRL_TRIGGER,
   //                         0,
   //                         NULL,
   //                         0);

   dsr_enqueue(pDeviceData->pDsr, NULL);

// Remove this call on ThreadX as calling this from interrupt context is not supported.
   //DalInterruptController_InterruptDone(((DALDevCtxt *)pDevCtxt)->hDALInterrupt,
   //                                     pDevCtxt->InterruptId);

   return DAL_SUCCESS;
}

//void bamIsrDsr(dsr_obj_t *pDsr, void *ctxt, void *pPayload)

void Tsens_ThresholdEventHandler(dsr_obj_t *pDsr, void *ctxt, void *pPayload)
{
   uint32 uClient;
   uint32 uSensor;
   DALResult status;
   int32 sensorTemps[TSENS_MAX_NUM_SENSORS];
   uint32 sensorCodes[TSENS_MAX_NUM_SENSORS];
   int32 nSensorTemp;
   uint32 uSensorCode;
   TsensCallbackPayloadType tsensCallbackPayload;

   //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_INFO, "+Tsens_ThresholdEventHandler\n");

   TsensLock();
   pDeviceData->eDeviceState = TSENS_DEVICE_STATE_ISR;
   HAL_tsens_DisableUpperLowerInterrupt();

   for (uSensor = 0; uSensor < pDeviceData->uNumSensors; uSensor++)
   {
      /* Get the sensor temperatures */
      status = Tsens_GetTempAndCodeNoLock(uSensor, &sensorTemps[uSensor], &sensorCodes[uSensor]);
      if (status != DAL_SUCCESS)
      {
         sensorTemps[uSensor] = TSENS_INVALID_THRESHOLD;
      }
   }

   /* Check to see if any thresholds are crossed */
   for (uClient = 0; uClient < _TSENS_NUM_CLIENTS; uClient++)
   {
      if (pDeviceData->clients[uClient].bEnableThresholds == TRUE && pDeviceData->clients[uClient].bRegistered == TRUE)
      {
         for (uSensor = 0; uSensor < pDeviceData->uNumSensors; uSensor++)
         {
            nSensorTemp = sensorTemps[uSensor];
            uSensorCode = sensorCodes[uSensor];
            if (nSensorTemp != TSENS_INVALID_THRESHOLD)
            {
               if ((pDeviceData->clients[uClient].eThresholdState[uSensor][TSENS_THRESHOLD_UPPER] == TSENS_THRESHOLD_STATE_ENABLED)
                     && (uSensorCode >= pDeviceData->clients[uClient].uThresholdCode[uSensor][TSENS_THRESHOLD_UPPER]))
               {
                  pDeviceData->clients[uClient].eThresholdState[uSensor][TSENS_THRESHOLD_UPPER] = TSENS_THRESHOLD_STATE_TRIGGERED;

                  tsensCallbackPayload.eThreshold = TSENS_THRESHOLD_UPPER;
                  tsensCallbackPayload.uSensor = uSensor;
                  tsensCallbackPayload.nTriggeredDegC = nSensorTemp;

                  TsensUnLock();
                  // TODO
                  //DALSYS_EventCtrlEx(pDeviceData->clients[uClient].hEvent[uSensor][TSENS_THRESHOLD_UPPER],
                  //                   DALSYS_EVENT_CTRL_TRIGGER,
                  //                   TSENS_THRESHOLD_UPPER,
                  //                   (void *)&tsensCallbackPayload,
                  //                   sizeof(tsensCallbackPayload));
                  if (pDeviceData->clients[uClient].pfnCb[uSensor][TSENS_THRESHOLD_UPPER] != NULL)
				  	
                  {
                     if(pDeviceData->clients[uClient].tsensCbPayload[uSensor][TSENS_THRESHOLD_UPPER] == NULL)   
                     	{
                          (*pDeviceData->clients[uClient].pfnCb[uSensor][TSENS_THRESHOLD_UPPER])(pDeviceData->clients[uClient].pCbCtxt[uSensor][TSENS_THRESHOLD_UPPER], &tsensCallbackPayload);
                     	}
					 else
					 	{
					 	  memcpy(pDeviceData->clients[uClient].tsensCbPayload[uSensor][TSENS_THRESHOLD_UPPER], &tsensCallbackPayload , sizeof(TsensCallbackPayloadType));
						  (*pDeviceData->clients[uClient].pfnCb[uSensor][TSENS_THRESHOLD_UPPER])(pDeviceData->clients[uClient].pCbCtxt[uSensor][TSENS_THRESHOLD_UPPER], pDeviceData->clients[uClient].tsensCbPayload[uSensor][TSENS_THRESHOLD_UPPER]);
					 	}
                  }
                  TsensLock();

               }

               if ((pDeviceData->clients[uClient].eThresholdState[uSensor][TSENS_THRESHOLD_LOWER] == TSENS_THRESHOLD_STATE_ENABLED)
                     && (uSensorCode <= pDeviceData->clients[uClient].uThresholdCode[uSensor][TSENS_THRESHOLD_LOWER]))
               {
                  pDeviceData->clients[uClient].eThresholdState[uSensor][TSENS_THRESHOLD_LOWER] = TSENS_THRESHOLD_STATE_TRIGGERED;

                  tsensCallbackPayload.eThreshold = TSENS_THRESHOLD_LOWER;
                  tsensCallbackPayload.uSensor = uSensor;
                  tsensCallbackPayload.nTriggeredDegC = nSensorTemp;

                  TsensUnLock();
                  // TODO
                  //DALSYS_EventCtrlEx(pDeviceData->clients[uClient].hEvent[uSensor][TSENS_THRESHOLD_LOWER],
                  //                   DALSYS_EVENT_CTRL_TRIGGER,
                  //                   TSENS_THRESHOLD_LOWER,
                  //                   (void *)&tsensCallbackPayload,
                  //                   sizeof(tsensCallbackPayload));
                  if (pDeviceData->clients[uClient].pfnCb[uSensor][TSENS_THRESHOLD_LOWER] != NULL)
                  {
                     if(pDeviceData->clients[uClient].tsensCbPayload[uSensor][TSENS_THRESHOLD_LOWER] == NULL)
                     	{
                          (*pDeviceData->clients[uClient].pfnCb[uSensor][TSENS_THRESHOLD_LOWER])(pDeviceData->clients[uClient].pCbCtxt[uSensor][TSENS_THRESHOLD_LOWER], &tsensCallbackPayload);
                     	}
					 else
					 	{
					 	  memcpy(pDeviceData->clients[uClient].tsensCbPayload[uSensor][TSENS_THRESHOLD_LOWER],&tsensCallbackPayload , sizeof(TsensCallbackPayloadType));
						  (*pDeviceData->clients[uClient].pfnCb[uSensor][TSENS_THRESHOLD_LOWER])(pDeviceData->clients[uClient].pCbCtxt[uSensor][TSENS_THRESHOLD_LOWER], pDeviceData->clients[uClient].tsensCbPayload[uSensor][TSENS_THRESHOLD_LOWER]);
					 	}
					 
                  }
                  TsensLock();
               }
            }
         }
      }
   }

   Tsens_UpdateThresholds();

   HAL_tsens_EnableUpperLowerInterrupt();

   pDeviceData->eDeviceState = TSENS_DEVICE_STATE_READY;

   TsensUnLock();

   // return DAL_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
 * Functions specific to the DalTsens interface
 * ----------------------------------------------------------------------*/
DALResult Tsens_SetThreshold(TsensClientCtxt *pCtxt, uint32 uSensor, TsensThresholdType eThreshold, int32 nDegC, TsensThresholdCbType pfnCallback, void *pCbCtxt)
{
   DALResult status = DAL_SUCCESS;
   uint32 uAdcCode = 0;

   if (pCtxt == NULL)
   {
      //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_WARNING,
      //                "Tsens_SetThreshold : NULL pCtxt\n");

      return TSENS_ERROR_INVALID_PARAMETER;
   }

   //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_INFO,
   //                "+Tsens_SetThreshold : nClient=%u uSensor=%u eThreshold=%u nDegC=%d hEvent=%08x\n",
   //                (unsigned int)pCtxt->nClient,
   //                (unsigned int)uSensor,
   //                (unsigned int)eThreshold,
   //                (int)nDegC,
   //                (unsigned int)hEvent);

   if (pCtxt->nClient >= _TSENS_NUM_CLIENTS)
   {
      //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_WARNING,
      //                "Tsens_SetThreshold : invalid client index: %u\n",
      //                (unsigned int)pCtxt->nClient);

      return TSENS_ERROR_INVALID_PARAMETER;
   }

   //if (hEvent == NULL)
   //{
   //   return TSENS_ERROR_INVALID_PARAMETER;
   //}

   if (uSensor >= pDeviceData->uNumSensors)
   {
      return TSENS_ERROR_UNSUPPORTED;
   }

   if (((1 << uSensor) & pDeviceData->pBsp->uSensorEnableMask) != (1 << uSensor))
   {
      return TSENS_ERROR_UNSUPPORTED;
   }

   if ((eThreshold != TSENS_THRESHOLD_UPPER) && (eThreshold != TSENS_THRESHOLD_LOWER))
   {
      //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_WARNING,
      //                "Tsens_SetThreshold : threshold type: %u\n",
      //                (unsigned int)eThreshold);

      return TSENS_ERROR_INVALID_PARAMETER;
   }

   if ((nDegC > pDeviceData->tempRange[uSensor].nMaxDegC || nDegC < pDeviceData->tempRange[uSensor].nMinDegC) && nDegC != TSENS_INVALID_THRESHOLD)
   {
      //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_WARNING,
      //                "Tsens_SetThreshold : invalid threshold value passed for nDegC\n");

      return TSENS_ERROR_INVALID_PARAMETER;
   }

   if (!Tsensi_IsCalibrated(uSensor))
   {
      return TSENS_ERROR_INVALID_CALIBRATION;
   }

   TsensLock();

   if (pDeviceData->clients[pCtxt->nClient].bRegistered == TRUE)
   {
      if (nDegC == TSENS_INVALID_THRESHOLD)
      {
         pDeviceData->clients[pCtxt->nClient].eThresholdState[uSensor][eThreshold] = TSENS_THRESHOLD_STATE_DISABLED;
      }
      else
      {
         pDeviceData->clients[pCtxt->nClient].eThresholdState[uSensor][eThreshold] = TSENS_THRESHOLD_STATE_ENABLED;
         pDeviceData->clients[pCtxt->nClient].nThresholdTemp[uSensor][eThreshold] = nDegC;

         (void)Tsensi_ConvertFromDegC(uSensor, nDegC, &uAdcCode);
         pDeviceData->clients[pCtxt->nClient].uThresholdCode[uSensor][eThreshold] = uAdcCode;
      }
      pDeviceData->clients[pCtxt->nClient].pfnCb[uSensor][eThreshold] = pfnCallback;
      pDeviceData->clients[pCtxt->nClient].pCbCtxt[uSensor][eThreshold] = pCbCtxt;

      status = Tsens_UpdateThresholds();
   }
   else
   {
      status = TSENS_ERROR_INVALID_PARAMETER;
   }

   TsensUnLock();

   return status;
}

DALResult Tsens_GetTemp(TsensClientCtxt *pCtxt, uint32 uSensor, TsensTempType *pTemp)
{
   DALResult status;
   uint32 uAdcCode = 0;
   int32 nDelay_us;

   if (uSensor >= (sizeof(uint32) * 8))
   {
      return TSENS_ERROR_INVALID_PARAMETER;
   }

   if (uSensor >= pDeviceData->uNumSensors)
   {
      return TSENS_ERROR_INVALID_PARAMETER;
   }

   if (pTemp == NULL)
   {
      return TSENS_ERROR_INVALID_PARAMETER;
   }

   if (pDeviceData->bDataReady == FALSE)
   {
      // Worst case wait is 1.5 ms. After that, data is ready unless TSENS gets disabled
      nDelay_us = (int32)(pDeviceData->uNumSensors * pDeviceData->pBsp->uSensorConvTime_us);

      do
      {
         if (HAL_tsens_TempMeasurementIsComplete() == FALSE)
         {
            // TODO
            //DALSYS_BusyWait(TSENS_BUSY_WAIT_TIME);
            nDelay_us -= TSENS_BUSY_WAIT_TIME;
         }
         else
         {
            break;
         }

      } while (nDelay_us > 0);

      pDeviceData->bDataReady = TRUE;
   }

   TsensLock();
   status = Tsens_GetTempAndCodeNoLock(uSensor, &pTemp->nDegC, &uAdcCode);
   TsensUnLock();


   //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_INFO,
   //                "+Tsens_GetTemp : status=0x%08x, temperature=%d degrees C\n",
   //                (unsigned int)status,
   //                (int)pTemp->nDegC);

   return status;
}

DALResult Tsens_GetTempRange(TsensClientCtxt *pCtxt, uint32 uSensor, TsensTempRangeType *pTempRange)
{
   DALResult status;

   if (pTempRange == NULL || pTempRange == NULL)
   {
      return TSENS_ERROR_INVALID_PARAMETER;
   }

   //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_INFO,
   //                "+Tsens_GetTempRange : uSensor=%u, pTempRange=%08x",
   //                (unsigned int)uSensor,
   //                (unsigned int)pTempRange);

   if (uSensor >= (sizeof(uint32) * 8))
   {
      return TSENS_ERROR_INVALID_PARAMETER;
   }

   if (uSensor >= pDeviceData->uNumSensors)
   {
      return TSENS_ERROR_INVALID_PARAMETER;
   }

   if ((1 << uSensor) & pDeviceData->pBsp->uSensorEnableMask)
   {
      status = Tsensi_ConvertToDegC(uSensor,
                                    0,
                                    &pTempRange->nMinDegC);

      if (status == DAL_SUCCESS)
      {
         status = Tsensi_ConvertToDegC(uSensor,
                                       HAL_tsens_GetMaxCode(),
                                       &pTempRange->nMaxDegC);
      }
   }
   else
   {
      status = TSENS_ERROR_UNSUPPORTED;
   }

   return status;
}

DALResult Tsens_DeviceInit(TsensClientCtxt *pCtxt)
{
   uint32 uClient, uThresholdIdx, uSensor, uMaxAdcCode;
   DALResult status = DAL_SUCCESS;
   DALSYSPropertyVar propertyVar;
   TsensDevCtxt *pDevCtxt = pCtxt->pTsensDevCtxt;
   TsensBspType *pBsp;
   boolean bDsrResult = FALSE;

   //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_INFO, "+Tsens_Init\n");

   pDeviceData = &tsensDeviceData;
   pDeviceData->eDeviceState = TSENS_DEVICE_STATE_INIT;
   pDeviceData->bDataReady = FALSE;

   /* Get the BSP */
   status = DALSYS_GetPropertyValue(pDevCtxt->hProp,
                                    "TSENS_BSP",
                                    0,
                                    &propertyVar);
   if (status != DAL_SUCCESS)
   {
      //DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
      //                "Tsens_DeviceInit : Device failed to get tsens_bsp property");

      goto error;
   }

   pDeviceData->pBsp = (TsensBspType *)propertyVar.Val.pStruct;

   pBsp = pDeviceData->pBsp;
   pDeviceData->uNumSensors = pBsp->uNumSensors;
   pDevCtxt->InterruptId = pBsp->uInterruptId;

   //DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
   //                "Tsens_DeviceInit : interrupt_id=%u",
   //                (unsigned int)pDevCtxt->InterruptId);

   /* Initialize synchronization object */
   //status = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
   //                           &pDeviceData->hSync,
   //                           &pDeviceData->syncObject);
   qurt_mutex_init(&pDeviceData->hSync);
   //if (status != DAL_SUCCESS)
   //{
   //   DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_FATAL_ERROR,
   //                   "Tsens_DeviceInit : Adc could not create device synchronization object\n");
   //   goto error;
   //}

   /* Attach to the HWIO DAL and map the memory regions */
   status = DAL_DeviceAttach(DALDEVICEID_HWIO, &pDeviceData->phHWIO);
   if (status != DAL_SUCCESS)
   {
      //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_FATAL_ERROR,
      //                "Tsens_DeviceInit : Failed to attach to HWIO with status=0x%08x\n", (unsigned int)status);
      goto error;
   }

   status = Tsens_MapHWIORegion("SECURITY_CONTROL", &gpuSecCtrlBase);
   if (status != DAL_SUCCESS)
   {
      //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_FATAL_ERROR,
      //                "Tsens_DeviceInit : Failed to map memory for TSENS QFPROM registers with status=0x%08x\n", (unsigned int)status);
      goto error;
   }

   status = Tsens_MapHWIORegion("MPM2_MPM", &gpuMpm2MpmBase);
   if (status != DAL_SUCCESS)
   {
      //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_FATAL_ERROR,
      //                "Tsens_Init : Failed to map memory for TSENS control registers with status=0x%08x\n", (unsigned int)status);
      goto error;
   }

   /* Configure the interrupt */
   /* Obtain an handle to the interrupt controller */
   status = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER,
                             &(((DALDevCtxt *)pDevCtxt)->hDALInterrupt));
   if (status != DAL_SUCCESS)
   {
      //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_FATAL_ERROR,
      //                 "Tsens_DeviceInit : Device failed to attach to Interrupt Controller");

      goto error;
   }


   /* Per DAL protocol invoke Open on the DAL Driver component. On success
    * this is ready to use
    */
   status = DalDevice_Open(((DALDevCtxt *)pDevCtxt)->hDALInterrupt, DAL_OPEN_SHARED);
   if (status != DAL_SUCCESS)
   {
      DAL_DeviceDetach(((DALDevCtxt *)pDevCtxt)->hDALInterrupt);

      //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_FATAL_ERROR,
      //                "Tsens_DeviceInit : Device failed to open Interrupt Controller");
      goto error;
   }

   /* If we have interrupts available attach now */
   /* Register an ISR with the DAL Interrupt Controller for this Int Id */
   status = DalInterruptController_RegisterISR(((DALDevCtxt *)pDevCtxt)->hDALInterrupt,
                                               pDevCtxt->InterruptId,
                                               Tsens_ThresholdInterruptIsr,
                                               (DALISRCtx)pDevCtxt,
                                               DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER);
   if (status != DAL_SUCCESS)
   {
      DAL_DeviceDetach(((DALDevCtxt *)pDevCtxt)->hDALInterrupt);
      //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_FATAL_ERROR,
      //                "Tsens_DeviceInit : Device failed to register ISR");
      goto error;
   }

   /*
    * Create Work-Loop.
    */
   //status = DALSYS_RegisterWorkLoop(tsensWorkloopPriority,
   //                                 10,
   //                                 &pDeviceData->hTsensWorkLoop,
   //                                 NULL);
        bDsrResult = dsr_create(&pDeviceData->pDsr,
                                Tsens_ThresholdEventHandler,
                                (void *)pCtxt,
                                DSR_PRI_HIGH,
                                DSR_CONTEXT_ISR);
        if (!bDsrResult)
        {
            return DAL_ERROR;
        }
   //if(status != DAL_SUCCESS)
   //{
   //   //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_FATAL_ERROR,
   //   //                "Tsens_DeviceInit : failed to create workloop\n");
   //   return status;
   //}

   ///*
   // * Create Async Object dedicated for workloop
   // */
   //status = DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
   //                            &pDeviceData->hTsensInterruptEvent, NULL);
   //if(status != DAL_SUCCESS)
   //{
   //  //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_FATAL_ERROR,
   //  //                "Tsens_DeviceInit : failed to create workloop event\n");
   //  return status;
   //}

   ///*
   // * Add the event to the workloop
   // */
   //status = DALSYS_AddEventToWorkLoop(pDeviceData->hTsensWorkLoop,
   //                                   Tsens_ThresholdEventHandler, NULL,
   //                                   pDeviceData->hTsensInterruptEvent, NULL);
   //if(status != DAL_SUCCESS)
   //{
   //   //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_FATAL_ERROR,
   //   //                "Tsens_DeviceInit : failed to create workloop event\n");
   //   return status;
   //}

   for(uClient = 0; uClient < _TSENS_NUM_CLIENTS; uClient++)
   {
      pDeviceData->clients[uClient].bEnableThresholds = TRUE;
      pDeviceData->clients[uClient].bRegistered = FALSE;

      for(uSensor = 0; uSensor < pDeviceData->uNumSensors; uSensor++)
      {
         for(uThresholdIdx = 0; uThresholdIdx < TSENS_NUM_THRESHOLD_IDXS; uThresholdIdx++)
         {
            pDeviceData->clients[uClient].nThresholdTemp[uSensor][uThresholdIdx] = TSENS_INVALID_THRESHOLD;
         }
      }
   }

   /* The first client needs registered in Init because for the first call to
    * Tsens_RegisterClient the ctxt is not yet initialized */
   pDeviceData->clients[0].bRegistered = TRUE;
   pCtxt->nClient = 0;

   /* Read the QFPROM registers and calculate the calibration parameters */
   Tsensi_CalibrationInit(pBsp->uNumSensors, pBsp->panX1_default, pBsp->panM_default, pBsp->nY1, pBsp->nY2);

   /* Record the min & max temperatures for the thresholds */
   for (uSensor = 0; uSensor < pDeviceData->uNumSensors; uSensor++)
   {
      if(Tsensi_IsCalibrated(uSensor))
      {
         status = Tsens_GetTempRange(pCtxt,
                                     uSensor,
                                     &pDeviceData->tempRange[uSensor]);

         if(status != DAL_SUCCESS)
         {
            goto error;
         }
      }
   }

   HAL_tsens_DisableUpperLowerInterrupt();
   uMaxAdcCode = HAL_tsens_GetMaxCode();
   for (uSensor = 0; uSensor < pDeviceData->uNumSensors; uSensor++)
   {
      HAL_tsens_SetThreshold(HAL_TSENS_LOWER_LIMIT_TH, uSensor, 0);
      HAL_tsens_SetThreshold(HAL_TSENS_UPPER_LIMIT_TH, uSensor, uMaxAdcCode);

      HAL_tsens_DisableInterrupt(HAL_TSENS_LOWER_LIMIT_TH, uSensor);
      HAL_tsens_DisableInterrupt(HAL_TSENS_UPPER_LIMIT_TH, uSensor);
   }

   pDeviceData->uSensorsEnabledMask = pDeviceData->pBsp->uSensorEnableMask;

   pDeviceData->eDeviceState = TSENS_DEVICE_STATE_READY;

   return DAL_SUCCESS;

error:
   pDeviceData->eDeviceState = TSENS_DEVICE_STATE_ERROR;
   return status;
}

DALResult Tsens_DeviceDeInit(TsensClientCtxt *pCtxt)
{
   //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_INFO,
   //                "+Tsens_DeviceDeInit\n");

   TsensLock();

   HAL_tsens_DisableUpperLowerInterrupt();
   pDeviceData->bDataReady = FALSE;
   pDeviceData->eDeviceState = TSENS_DEVICE_STATE_INIT;

    //Destroy the DSR object
    if (pDeviceData->pDsr)
    {
       dsr_destroy(pDeviceData->pDsr);
    }

   TsensUnLock();

   return DAL_SUCCESS;
}

DALResult Tsens_GetNumSensors(TsensClientCtxt *pCtxt, uint32 *puNumSensors)
{
   //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_INFO,
   //                "+Tsens_GetNumSensors\n");

   if (puNumSensors == NULL)
   {
      return TSENS_ERROR_INVALID_PARAMETER;
   }

   *puNumSensors = pDeviceData->uNumSensors;

   return DAL_SUCCESS;
}

DALResult Tsens_SetEnableThresholds(TsensClientCtxt *pCtxt, DALBOOL bEnableThresholds)
{
   DALResult status = DAL_SUCCESS;

   if (pCtxt == NULL)
   {
      return TSENS_ERROR_INVALID_PARAMETER;
   }

   //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_INFO,
   //                "+Tsens_SetEnableThresholds : nClient=%u bEnableThresholds=%u \n",
   //                (unsigned int)pCtxt->nClient,
   //                (unsigned int)bEnableThresholds);

   if (pCtxt->nClient >= _TSENS_NUM_CLIENTS)
   {
      return TSENS_ERROR_INVALID_PARAMETER;
   }

   if ((bEnableThresholds != TRUE) && (bEnableThresholds != FALSE))
   {
      return TSENS_ERROR_INVALID_PARAMETER;
   }

   TsensLock();

   if (pDeviceData->clients[pCtxt->nClient].bRegistered == TRUE)
   {
      pDeviceData->clients[pCtxt->nClient].bEnableThresholds = bEnableThresholds;

      status = Tsens_UpdateThresholds();
   }
   else
   {
      status = TSENS_ERROR_INVALID_PARAMETER;
   }

   TsensUnLock();

   return status;
}

DALResult Tsens_CheckCalibration(TsensClientCtxt *pCtxt, uint32 uSensor)
{
   //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_INFO,
   //                "+Tsens_CheckCalibration\n");

   if (pDeviceData == NULL)
   {
      return TSENS_ERROR_INVALID_PARAMETER;
   }

   if (uSensor >= pDeviceData->uNumSensors)
   {
      return TSENS_ERROR_UNSUPPORTED;
   }

   if (Tsensi_IsCalibrated(uSensor) == FALSE)
   {
      return TSENS_ERROR_INVALID_CALIBRATION;
   }

   return DAL_SUCCESS;
}

/*-------------------------------------------------------------------------
 * Functions specific to the DAL interface
 * ----------------------------------------------------------------------*/
DALResult
Tsens_DriverInit(TsensDrvCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

DALResult
Tsens_DriverDeInit(TsensDrvCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

DALResult
Tsens_RegisterClient(TsensClientCtxt *pCtxt)
{
   DALResult status = DAL_ERROR;
   uint32 uClient;

   if (pCtxt == NULL)
   {
      return DAL_ERROR;
   }

   if (pDeviceData == NULL)
   {
      /* The DAL calls RegisterClient before calling DeviceInit, so this is a normal case.
       * As a result, the first client is registered in Tsens_DeviceInit */
      return DAL_SUCCESS;
   }

   TsensLock();
   for (uClient = 0; uClient < _TSENS_NUM_CLIENTS; uClient++)
   {
      if (pDeviceData->clients[uClient].bRegistered == FALSE)
      {
         pDeviceData->clients[uClient].bRegistered = TRUE;
         pCtxt->nClient = uClient;
         status = DAL_SUCCESS;
         break;
      }
   }

   if (status != DAL_SUCCESS)
   {
      pCtxt->nClient = _TSENS_NUM_CLIENTS;
      //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_ERROR,
      //                "Tsens_RegisterClient : Failed to register. Ran out of client handles.\n");
   }
   TsensUnLock();

   return status;
}

DALResult
Tsens_DisableClient(uint32 uClient)
{
   DALResult status = DAL_SUCCESS;
   uint32 uSensor, uThresholdIdx;

   if (uClient < _TSENS_NUM_CLIENTS)
   {
      for (uSensor = 0; uSensor < pDeviceData->uNumSensors; uSensor++)
      {
         for (uThresholdIdx = 0; uThresholdIdx < TSENS_NUM_THRESHOLD_IDXS; uThresholdIdx++)
         {
            pDeviceData->clients[uClient].eThresholdState[uSensor][uThresholdIdx] = TSENS_THRESHOLD_STATE_DISABLED;
            pDeviceData->clients[uClient].bEnableThresholds = FALSE;
         }
      }

      status = Tsens_UpdateThresholds();
   }

   return status;
}

DALResult
Tsens_DeregisterClient(TsensClientCtxt *pCtxt)
{
   DALResult status = DAL_ERROR;

   if (pCtxt == NULL || pDeviceData == NULL || pCtxt->nClient >= _TSENS_NUM_CLIENTS)
   {
      //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_ERROR,
      //                "Failed to deregister the client");

      return DAL_ERROR;
   }

   TsensLock();

   pDeviceData->clients[pCtxt->nClient].bRegistered = FALSE;
   status = Tsens_DisableClient(pCtxt->nClient);
   if (status != DAL_SUCCESS)
   {
      //DALSYS_LogEvent(DALDEVICEID_TSENS, DALSYS_LOGEVENT_ERROR,
      //                "Failed to deregister the client");
   }

   TsensUnLock();

   return status;
}

