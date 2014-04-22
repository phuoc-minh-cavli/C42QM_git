/*============================================================================
  FILE:         DalAdc.c

  OVERVIEW:     Implementation of the ADC DAL

  DEPENDENCIES: None

                Copyright (c) 2009-2015, 2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.tx/6.0/hwengines/adc/dal/src/DalAdc.c#1 $$DateTime: 2020/01/30 22:49:35 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2018-12-28  ay   updated for MDM9205
  2015-03-27  jjo  Added tolerance API.
  2013-05-22  jjo  Moved the work loop to the DAL ADC layer; one shared
                   device queue; added synchronous read support.
  2013-02-26  jjo  Added TM APIs.
  2012-10-18  jjo  Added a result type for recalibration.
  2012-06-13  jjo  Updated to use DAL device ID string names.
  2012-04-19  jdt  Updated for 8974: Removed remote support. Removed batch APIs.
  2009-03-16  jdt  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalAdc.h"
#include "DDIAdcDevice.h"
#include "DDITimetick.h"
#include <qurt_memory.h>
#include <qurt_mutex.h>
#include <stdlib.h>
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ADC_DEFAULT_STACK_SIZE 0x2000
#define ADC_LOG_MAX 30

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
qurt_mutex_t ghAdcDebugSync;
//DALSYS_SYNC_OBJECT(ghAdcDebugSyncObject);

struct
{
   struct
   {
      enum
      {
         ADC_LOG_EVENT_REQUEST_CONVERSION = 1,
         ADC_LOG_EVENT_REQUEST_RECALIBRATION,
         ADC_LOG_EVENT_CONVERSION_COMPLETE,
         ADC_LOG_EVENT_CALIBRATION_COMPLETE
      } eEvent;
      uint64 uTimestamp;
      uint32 uDeviceIdx;
      uint32 uChannelIdx;
      DALSYSEventHandle hClientEvent;
   } aLog[ADC_LOG_MAX];
   DalDeviceHandle *phTimetickDev;
   uint32 uCurIdx;
} gAdcDebug;




/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
/*======================================================================

  FUNCTION        Adc_GetTimetick

  DESCRIPTION     Gets the timetick

  DEPENDENCIES    None

  PARAMETERS      None

  RETURN VALUE    64-bit timetick

  SIDE EFFECTS    None

======================================================================*/
static uint64
Adc_GetTimetick(void)
{
   DALResult result;
   DalTimetickTime64Type u64Tick;

   result = DalTimetick_GetTimetick64(gAdcDebug.phTimetickDev, &u64Tick);
   if (result != DAL_SUCCESS)
   {
      return 0;
   }

   return (uint64)u64Tick;
}

/*======================================================================

  FUNCTION        Adc_LogConversionRequest

  DESCRIPTION     This function logs a conversion request

  DEPENDENCIES    None

  PARAMETERS      uDeviceIdx  [in] the device index
                  uChannelIdx [in] the channel index

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
static void
Adc_LogConversionRequest(
   uint32 uDeviceIdx,
   uint32 uChannelIdx
   )
{
   qurt_mutex_lock(&ghAdcDebugSync);
   gAdcDebug.aLog[gAdcDebug.uCurIdx].eEvent = ADC_LOG_EVENT_REQUEST_CONVERSION;
   gAdcDebug.aLog[gAdcDebug.uCurIdx].uTimestamp = Adc_GetTimetick();
   gAdcDebug.aLog[gAdcDebug.uCurIdx].uDeviceIdx = uDeviceIdx;
   gAdcDebug.aLog[gAdcDebug.uCurIdx].uChannelIdx = uChannelIdx;

   if (gAdcDebug.uCurIdx < ADC_LOG_MAX - 1)
   {
      gAdcDebug.uCurIdx++;
   }
   else
   {
      gAdcDebug.uCurIdx = 0;
   }
   qurt_mutex_unlock(&ghAdcDebugSync);
}



/*======================================================================

  FUNCTION        Adc_LogConversionComplete

  DESCRIPTION     This function logs when a conversion completes

  DEPENDENCIES    None

  PARAMETERS      uDeviceIdx   [in] the device index
                  uChannelIdx  [in] the channel index
                  hClientEvent [in] the client event

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
static void
Adc_LogConversionComplete(
   uint32 uDeviceIdx,
   uint32 uChannelIdx,
   const DALSYSEventHandle hClientEvent
   )
{
   qurt_mutex_lock(&ghAdcDebugSync);
   gAdcDebug.aLog[gAdcDebug.uCurIdx].eEvent = ADC_LOG_EVENT_CONVERSION_COMPLETE;
   gAdcDebug.aLog[gAdcDebug.uCurIdx].uTimestamp = Adc_GetTimetick();
   gAdcDebug.aLog[gAdcDebug.uCurIdx].uDeviceIdx = uDeviceIdx;
   gAdcDebug.aLog[gAdcDebug.uCurIdx].uChannelIdx = uChannelIdx;
   gAdcDebug.aLog[gAdcDebug.uCurIdx].hClientEvent = hClientEvent;

   if (gAdcDebug.uCurIdx < ADC_LOG_MAX - 1)
   {
      gAdcDebug.uCurIdx++;
   }
   else
   {
      gAdcDebug.uCurIdx = 0;
   }
   qurt_mutex_unlock(&ghAdcDebugSync);
}



/*======================================================================

  FUNCTION        Adc_AddDeviceQueueRequest

  DESCRIPTION     This function adds a conversion or recalibration
                  request to the device queue.

  DEPENDENCIES    None

  PARAMETERS
      pDevCtxt          [in]  pointer to the ADC device context
      eAdcDeviceRequest [in]  which kind of request to add
      nDeviceIdx        [in]  index to the ADC device
      nChannelIdx       [in]  index to the ADC channel
      hClientEvent      [in]  event to signal when the request is completed
                              set hEvent = NULL to block on the completion
      pAdcRequestStatus [out] pointer to the request status; only set if
                              non-null.

  RETURN VALUE    DAL_SUCCESS if request was added to the queue
                  ADC_DEVICE_QUEUE_FULL if the queue was full

  SIDE EFFECTS    None

======================================================================*/
//static DALResult
//Adc_AddDeviceQueueRequest(
//   AdcClientCtxt *pCtxt,
//   AdcDeviceRequestType eAdcDeviceRequest,
//   uint32 uDeviceIdx,
//   uint32 uChannelIdx,
//   const DALSYSEventHandle hClientEvent,
//   AdcRequestStatusType *pAdcRequestStatus
//   )
//{
//   AdcDeviceQueueNodeType *pNode;
//   DALResult ret = DAL_SUCCESS;
//   AdcDevCtxt *pDevCtxt = pCtxt->pAdcDevCtxt;
//   uint32 uQueueIdx = pDevCtxt->pBsp->paAdcPhysicalDevices[uDeviceIdx].uQueueIdx;
//   AdcQueueType *pQueue = &pDevCtxt->paQueues[uQueueIdx];
//
//   qurt_mutex_lock(pDevCtxt->hSync);
//
//   /* Get a node from the pool of free nodes */
//   pNode = (AdcDeviceQueueNodeType *)DALFW_Q_Get(&pQueue->qFree);
//   if (pNode != NULL)
//   {
//      /*
//       * A free node is available, so copy the request data into the free node
//       * and move the node from the list of free nodes to the appropriate queue.
//       */
//      pNode->hClientEvent = hClientEvent;
//      pNode->uDeviceIdx = uDeviceIdx;
//      pNode->uChannelIdx = uChannelIdx;
//      pNode->eAdcDeviceRequest = eAdcDeviceRequest;
//
//      if (DALFW_Q_Check(&pQueue->qPending) != NULL)
//      {
//         /* Queue the node */
//         pNode->adcRequestStatus.eStatus = ADC_REQUEST_STATUS_QUEUED;
//         if (pAdcRequestStatus != NULL)
//         {
//            pAdcRequestStatus->eStatus = ADC_REQUEST_STATUS_QUEUED;
//         }
//         DALFW_Q_Put(&pQueue->qQueued, (DALFW_Q_LinkType * )pNode);
//      }
//      else
//      {
//         /* Place the node in pending */
//         pNode->adcRequestStatus.eStatus = ADC_REQUEST_STATUS_PENDING;
//         if (pAdcRequestStatus != NULL)
//         {
//            pAdcRequestStatus->eStatus = ADC_REQUEST_STATUS_PENDING;
//         }
//         DALFW_Q_Put(&pQueue->qPending, (DALFW_Q_LinkType *)pNode);
//
//         /* Queue the work loop for processing */
//         DALSYS_EventCtrlEx(pQueue->hAdcOpEvent,
//                            DALSYS_EVENT_CTRL_TRIGGER,
//                            0,
//                            NULL,
//                            0);
//      }
//   }
//   else
//   {
//      if (pAdcRequestStatus != NULL)
//      {
//         pAdcRequestStatus->eStatus = ADC_REQUEST_STATUS_ERROR;
//      }
//
//      DALSYS_LogEvent(pDevCtxt->DevId,
//                      DALSYS_LOGEVENT_INFO,
//                      "Adc device request queue is full");
//      ret = ADC_ERROR_DEVICE_QUEUE_FULL;
//   }
//
//   qurt_mutex_unlock(pDevCtxt->hSync);
//
//   return ret;
//}

/*======================================================================

  FUNCTION        Adc_ReadChannel

  DESCRIPTION     Reads an ADC channel.

  DEPENDENCIES    None

  PARAMETERS      pDevCtxt     [in]  device context
                  uDeviceIdx   [in]  device index
                  uChannelIdx  [in]  channel index
                  hClientEvent [in]  event to complete
                  pAdcResult   [out] conversion result

  RETURN VALUE    DAL_SUCCESS or an error.

  SIDE EFFECTS    None

======================================================================*/
static DALResult
Adc_ReadChannel(
   AdcDevCtxt *pDevCtxt,
   uint32 uDeviceIdx,
   uint32 uChannelIdx,
   const DALSYSEventHandle hClientEvent,
   AdcResultType *pAdcResult
   )
{
   DALResult result;
   AdcDeviceResultType adcDeviceReadResult;
   AdcDeviceDataType *pDeviceData;

   memset(pAdcResult, 0, sizeof(AdcResultType));
   pAdcResult->eStatus = ADC_RESULT_INVALID;
   pAdcResult->nDeviceIdx = uDeviceIdx;
   pAdcResult->nChannelIdx = uChannelIdx;

   pDeviceData = &pDevCtxt->paDeviceData[uDeviceIdx];

   DalDevice_Open(pDeviceData->phDevice, DAL_OPEN_EXCLUSIVE);

   result = DalAdcDevice_ReadChannel(pDeviceData->phDevice,
                                     uChannelIdx,
                                     &adcDeviceReadResult);

   DalDevice_Close(pDeviceData->phDevice);

   if (result != DAL_SUCCESS)
   {
      return result;
   }

   if (adcDeviceReadResult.eStatus == ADC_DEVICE_RESULT_VALID)
   {
      pAdcResult->eStatus = ADC_RESULT_VALID;
   }
   else if (adcDeviceReadResult.eStatus == ADC_DEVICE_RESULT_TIMEOUT)
   {
      pAdcResult->eStatus = ADC_RESULT_TIMEOUT;
   }
   else if (adcDeviceReadResult.eStatus == ADC_DEVICE_RESULT_FIFO_NOT_EMPTY)
   {
      pAdcResult->eStatus = ADC_RESULT_FIFO_NOT_EMPTY;
   }
   else
   {
      pAdcResult->eStatus = ADC_RESULT_INVALID;
   }

   pAdcResult->nPhysical = adcDeviceReadResult.nPhysical;
   pAdcResult->nPercent = adcDeviceReadResult.uPercent;
   pAdcResult->nMicrovolts = adcDeviceReadResult.uMicrovolts;
   pAdcResult->nCode = adcDeviceReadResult.uCode;

   Adc_LogConversionComplete(uDeviceIdx, uChannelIdx, hClientEvent);

   return DAL_SUCCESS;
}



/*======================================================================

  FUNCTION        Adc_PerformOperationInWorkLoop

  DESCRIPTION     This function processes the request that is pending.

  DEPENDENCIES    None

  PARAMETERS      hEvent     [in]
                  pInDevCtxt [in] pointer to the ADC device context

  RETURN VALUE    DAL_SUCCESS or an error.

  SIDE EFFECTS    None

======================================================================*/
//static DALResult
//Adc_PerformOperationInWorkLoop(
//   DALSYSEventHandle hEvent,
//   void *pInDevCtxt
//   )
//{
//   AdcQueueType *pQueue = pInDevCtxt;
//   DALResult result;
//   AdcDevCtxt *pDevCtxt = pQueue->pAdcDevCtxt;
//   AdcDeviceQueueNodeType *pNode;
//   AdcResultType adcResult;
//   AdcRecalibrationResultType adcRecalResult;
//   void *pPayload;
//   uint32 uPayloadSize;
//
//   /* Get the pending request to be serviced */
//   pNode = (AdcDeviceQueueNodeType *)DALFW_Q_Check(&pQueue->qPending);
//   if (pNode == NULL)
//   {
//      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
//                      "ADC work loop called with an queue empty");
//      return DAL_ERROR;
//   }
//
//   do
//   {
//      /* Acquire the queue lock inside the loop so it is released after
//         each ADC operation. Otherwise, if the work loop becomes non-empty
//         it may starve the blocking reads trying to acquire the lock */
//      qurt_mutex_lock(pQueue->hSync);
//
//      switch (pNode->eAdcDeviceRequest)
//      {
//         case ADC_REQUEST_CONVERSION:
//
//            result = Adc_ReadChannel(pDevCtxt,
//                                     pNode->uDeviceIdx,
//                                     pNode->uChannelIdx,
//                                     pNode->hClientEvent,
//                                     &adcResult);
//            if (result != DAL_SUCCESS)
//            {
//               adcResult.eStatus = ADC_RESULT_INVALID;
//            }
//
//            pPayload = &adcResult;
//            uPayloadSize = sizeof(adcResult);
//
//            break;
//
//         case ADC_REQUEST_CHANNEL_RECALIBRATION:
//
//            result = Adc_RecalibrateChannel(pDevCtxt,
//                                            pNode->uDeviceIdx,
//                                            pNode->uChannelIdx,
//                                            pNode->hClientEvent,
//                                            &adcRecalResult);
//            if (result != DAL_SUCCESS)
//            {
//               adcRecalResult.eStatus = ADC_RESULT_INVALID;
//            }
//
//            pPayload = &adcRecalResult;
//            uPayloadSize = sizeof(adcRecalResult);
//
//            break;
//
//         default:
//            pPayload = NULL;
//            uPayloadSize = 0;
//
//            break;
//      }
//
//      qurt_mutex_unlock(pQueue->hSync);
//
//      if (pNode->hClientEvent != NULL)
//      {
//         /* notify the client that the operation is complete */
//         /* note: clients can use NULL for recalibration hence the check */
//         (void)DALSYS_EventCtrlEx(pNode->hClientEvent,
//                                  DALSYS_EVENT_CTRL_TRIGGER,
//                                  0,
//                                  pPayload,
//                                  uPayloadSize);
//      }
//
//      qurt_mutex_lock(pDevCtxt->hSync);
//
//      /* Remove node from pending queue and place it back into the free list */
//      pNode->adcRequestStatus.eStatus = ADC_REQUEST_STATUS_UNKNOWN;
//      DALFW_Q_Delete(&pQueue->qPending, (DALFW_Q_LinkType *)pNode);
//      DALFW_Q_Put(&pQueue->qFree, (DALFW_Q_LinkType *)pNode);
//
//      /* Ready to process the next node */
//      pNode = (AdcDeviceQueueNodeType *)DALFW_Q_Check(&pQueue->qQueued);
//      if (pNode != NULL)
//      {
//         /* Delete the node from queued */
//         DALFW_Q_Delete(&pQueue->qQueued, (DALFW_Q_LinkType*)pNode);
//
//         /* Place the node in pending */
//         DALFW_Q_Put(&pQueue->qPending, (DALFW_Q_LinkType *)pNode);
//
//         pNode->adcRequestStatus.eStatus = ADC_REQUEST_STATUS_PENDING;
//      }
//
//      qurt_mutex_unlock(pDevCtxt->hSync);
//   } while (pNode != NULL);
//
//   return DAL_SUCCESS;
//}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
Adc_DriverInit(AdcDrvCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

DALResult
Adc_DriverDeInit(AdcDrvCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are declared in DalDevice Interface.
------------------------------------------------------------------------------*/
DALResult
Adc_DeviceInit(AdcClientCtxt *pCtxt)
{
   uint32 uDeviceIdx;

   DALResult result = DAL_SUCCESS;
   DALSYSPropertyVar propVar;
   AdcDeviceDevicePropertiesType adcDeviceProp;
   AdcDeviceDataType *pDeviceData;

   AdcDevCtxt *pDevCtxt = pCtxt->pAdcDevCtxt;


   /* Initialize device context */
   //pDevCtxt->hSync = NULL;
   pDevCtxt->paDeviceData = NULL;
   pDevCtxt->paQueues = NULL;

   /* Prevent ADC from unloading */
   DALFW_MarkDeviceStatic((DALDevCtxt *)pDevCtxt);

   /* Initialize the log */
   gAdcDebug.uCurIdx = 0;

   result = DalTimetick_Attach("SystemTimer", &gAdcDebug.phTimetickDev);
   if (result != DAL_SUCCESS)
   {
     // DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
     //                 "VADC: Failed to attach to Timetick device");
      return result;
   }

   /* Read the DAL properties */
   result = DALSYS_GetDALPropertyHandle(pDevCtxt->DevId, pDevCtxt->hProp);
   if (result != DAL_SUCCESS)
   {
     // DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
     //                 "Could not open ADC DAL properties");
      return result;
   }

   result = DALSYS_GetPropertyValue(pDevCtxt->hProp, "ADC_BSP", 0, &propVar);
   if (result != DAL_SUCCESS)
   {
    //  DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
    //                  "BSP not found in ADC DAL properties");
      return result;
   }

   pDevCtxt->pBsp = (AdcBspType *)propVar.Val.pStruct;

   /* Initialize synchronization objects */
   //result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
   //                           &ghAdcDebugSync,
   //                           &ghAdcDebugSyncObject);

   //if (result != DAL_SUCCESS)
   //{
   //   DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
   //                   "Adc could not create log synchronization object");
   //   return result;
   //}

   //result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
   //                           &pDevCtxt->hSync,
   //                           &pDevCtxt->syncObject);
   //if (result != DAL_SUCCESS)
   //{
   //   DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
   //                   "Adc could not create device synchronization object");
   //   return result;
   //}

   ///* Allocate memory for the queues */
   //result = DALSYS_Malloc((sizeof(AdcQueueType) * pDevCtxt->pBsp->uNumQueues),
   //                       (void **)&pDevCtxt->paQueues);

   //if (result != DAL_SUCCESS)
   //{
   //   DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
   //                   "Adc could not allocate memory for device queues");
   //   return result;
   //}

   //memset(pDevCtxt->paQueues,
   //              0,
   //              (sizeof(AdcQueueType) * pDevCtxt->pBsp->uNumQueues));

   ///* Initialize the queues */
   //for (uQueueIdx = 0; uQueueIdx < pDevCtxt->pBsp->uNumQueues; uQueueIdx++)
   //{
   //   pQueue = &pDevCtxt->paQueues[uQueueIdx];

   //   /* initialize the queue */
   //   pQueue->pAdcDevCtxt = pDevCtxt;
   //   (void)DALFW_Q_Init(&pQueue->qFree);
   //   (void)DALFW_Q_Init(&pQueue->qQueued);
   //   (void)DALFW_Q_Init(&pQueue->qPending);

   //   /* allocate the queue nodes */
   //   uQueueSize = pDevCtxt->pBsp->paAdcQueueConfigs[uQueueIdx].uQueueSize;
   //   pQueue->uQueueSize = uQueueSize;

   //   result = DALSYS_Malloc((sizeof(AdcDeviceQueueNodeType) * uQueueSize),
   //                          (void **)&pQueue->paNodes);
   //   if (result != DAL_SUCCESS)
   //   {
   //      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
   //                      "Adc could not allocate memory for device queue nodes");
   //      return result;
   //   }

   //   for (uNodeIdx = 0; uNodeIdx < uQueueSize; uNodeIdx++)
   //   {
   //      /* Initialize node data */
   //      memset(&pQueue->paNodes[uNodeIdx],
   //                    0,
   //                    sizeof(AdcDeviceQueueNodeType));

   //      pQueue->paNodes[uNodeIdx].adcRequestStatus.eStatus = ADC_REQUEST_STATUS_UNKNOWN;

   //      DALFW_Q_Put(&pQueue->qFree,
   //                  (DALFW_Q_LinkType *)&pQueue->paNodes[uNodeIdx]);
   //   }

   //   /* Create the work-loop */
   //   result = DALSYS_RegisterWorkLoopEx(pDevCtxt->pBsp->paAdcQueueConfigs[uQueueIdx].pszWorkLoopName,
   //                                      ADC_DEFAULT_STACK_SIZE,
   //                                      gAdcWorkloopPriority,
   //                                      10,
   //                                      &pQueue->hAdcOpWorkLoop,
   //                                      NULL);
   //   if (result != DAL_SUCCESS)
   //   {
   //      return result;
   //   }

   //   /* Create a workloop event for performing ADC functions */
   //   result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
   //                               &pQueue->hAdcOpEvent,
   //                               NULL);
   //   if (result != DAL_SUCCESS)
   //   {
   //      return result;
   //   }

   //   /* Add the event to the workloop */
   //   result = DALSYS_AddEventToWorkLoop(pQueue->hAdcOpWorkLoop,
   //                                      Adc_PerformOperationInWorkLoop,
   //                                      pQueue,
   //                                      pQueue->hAdcOpEvent,
   //                                      NULL);
   //   if (result != DAL_SUCCESS)
   //   {
   //      return result;
   //   }

   //   /* Create a synchronization event to be used between work loop and blocking reads */
   //   result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
   //                              &pQueue->hSync,
   //                              &pQueue->syncObject);
   //   if (result != DAL_SUCCESS)
   //   {
   //      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
   //                      "Adc could not create work loop synchronization object");
   //      return result;
   //   }
   //}

   /* Allocate memory for the device data */
   pDevCtxt->paDeviceData = malloc(sizeof(AdcDeviceDataType) * pDevCtxt->pBsp->uNumPhysicalDevices);

   if (pDevCtxt->paDeviceData == NULL)
   {
      //DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
      //                "Adc could not allocate memory for device data");
      return result;
   }

   /*
    * Initialize the devices
    */
   for (uDeviceIdx = 0; uDeviceIdx < pDevCtxt->pBsp->uNumPhysicalDevices; uDeviceIdx++)
   {
      /* Get the pointer to the device data corresponding to this device */
      pDeviceData = &pDevCtxt->paDeviceData[uDeviceIdx];

      /* Initialize all of device data to 0 */
      memset(pDeviceData, 0, sizeof(AdcDeviceDataType));

      /* Flag to indicate device hardware resouraces are OFF. */
      pDeviceData->bDeviceIsOpen = FALSE;

      /*
       * Attach to the ADC device. The attach can fail when the device is
       * not physically present. In this case, we just try and attach to the
       * next available device.
       */
      result = DAL_AdcDeviceDeviceAttach(pDevCtxt->pBsp->paAdcPhysicalDevices[uDeviceIdx].pszDevName,
                                         &pDeviceData->phDevice);

      if (result == DAL_SUCCESS && pDeviceData->phDevice != NULL)
      {
         pDeviceData->deviceStatus = ADC_DEVICE_AVAILABLE;
      }
      else
      {
         //DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
         //                "Adc failed attaching to a physical device");

         pDeviceData->deviceStatus = ADC_DEVICE_NOT_AVAILABLE;
      }

      if (pDeviceData->phDevice == NULL)
      {
         continue;
      }

      /* Get the number of channels for each device */
      result = DalAdcDevice_GetDeviceProperties(pDeviceData->phDevice,
                                                &adcDeviceProp);
      if (result != DAL_SUCCESS)
      {
        // DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
        //                 "Error obtaining the number of ADC device channels");
         return result;
      }

      pDeviceData->uNumChannels = adcDeviceProp.uNumChannels;

   }

   qurt_mutex_init(&ghAdcDebugSync);


   return result;
}

DALResult
Adc_DeviceDeInit(AdcClientCtxt *pCtxt)
{
   uint32 uDeviceIdx;
  
   DALResult tmpResult;
   DALResult retResult = DAL_SUCCESS;
   
   AdcDevCtxt * pDevCtxt = pCtxt->pAdcDevCtxt;

   /* Detach from the ADC devices */
   for (uDeviceIdx = 0; uDeviceIdx < pDevCtxt->pBsp->uNumPhysicalDevices; uDeviceIdx++)
   {
      if (pDevCtxt->paDeviceData[uDeviceIdx].phDevice != NULL)
      {
         tmpResult = DAL_DeviceDetach(pDevCtxt->paDeviceData[uDeviceIdx].phDevice);
         if (tmpResult != DAL_SUCCESS)
         {
            retResult = DAL_ERROR;
         }
      }
   }

   /* Free device data array */
   if (pDevCtxt->paDeviceData != NULL)
   {
      free(pDevCtxt->paDeviceData);
      pDevCtxt->paDeviceData = NULL;
   }

   // if (ghAdcDebugSync != NULL)
   //{
   //   //tmpResult = DALSYS_DestroyObject(ghAdcDebugSync);
   //   if (tmpResult != DAL_SUCCESS)
   //   {
   //      retResult = DAL_ERROR;
   //   }
   //   ghAdcDebugSync = NULL;
   //}

    return retResult;
}

DALResult
Adc_PowerEvent(
   AdcClientCtxt *pCtxt,
   DalPowerCmd PowerCmd,
   DalPowerDomain PowerDomain
   )
{
   return DAL_SUCCESS;
}

DALResult
Adc_Open(
   AdcClientCtxt *pCtxt,
   uint32 dwaccessMode
   )
{
   return DAL_SUCCESS;
}

DALResult
Adc_Close(AdcClientCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

DALResult
Adc_Info(
   AdcClientCtxt *pCtxt,
   DalDeviceInfo *pdeviceInfo,
   uint32 dwSize
   )
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalAdc Interface.

These functions are documented in DDIAdc.h
------------------------------------------------------------------------------*/

DALResult
Adc_GetAdcInputProperties(
   AdcClientCtxt *pCtxt,
   const char *pInputName,
   uint32  nInputNameSize,
   AdcInputPropertiesType *pAdcInputProps
   )
{
   DALSYSPropertyVar propInput;
   DALResult result;
   uint32 uDeviceIdx, uChannelIdx;
   AdcDeviceDataType *pDeviceData;
   AdcDevCtxt *pDevCtxt = pCtxt->pAdcDevCtxt;

   if (pAdcInputProps == NULL)
   {
      return DAL_ERROR;
   }

   result = DALSYS_GetPropertyValue(pDevCtxt->hProp, pInputName, 0, &propInput);
   if (result == DAL_SUCCESS && pAdcInputProps != NULL)
   {
      pAdcInputProps->nDeviceIdx = propInput.Val.pdwVal[0];
      pAdcInputProps->nChannelIdx = propInput.Val.pdwVal[1];
      return DAL_SUCCESS;
   }

   for (uDeviceIdx = 0; uDeviceIdx < pDevCtxt->pBsp->uNumPhysicalDevices; uDeviceIdx++)
   {
      /* Get the pointer to the device data corresponding to this device index */
      pDeviceData = &pDevCtxt->paDeviceData[uDeviceIdx];

      if (pDeviceData->deviceStatus == ADC_DEVICE_NOT_AVAILABLE || pDeviceData->phDevice == NULL)
      {
         continue;
      }

      if (DalAdcDevice_GetInputProperties(pDeviceData->phDevice, pInputName, &uChannelIdx) == DAL_SUCCESS)
      {
         pAdcInputProps->nDeviceIdx  = uDeviceIdx;
         pAdcInputProps->nChannelIdx = uChannelIdx;
         return DAL_SUCCESS;
      }
   }

   return DAL_ERROR;
}

DALResult
Adc_Read(
   AdcClientCtxt *pCtxt,
   const AdcInputPropertiesType *pAdcInputProps,
   AdcResultType *pAdcResult
   )
{
   AdcDevCtxt *pDevCtxt = pCtxt->pAdcDevCtxt;
   DALResult result;

   if (pAdcInputProps == NULL || pAdcResult == NULL)
   {
      //DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
       //               "Adc read requested with a NULL pointer parameter");
      return ADC_ERROR_NULL_POINTER;
   }
   else if (pAdcInputProps->nDeviceIdx >= pDevCtxt->pBsp->uNumPhysicalDevices)
   {
     // DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
      //                "Adc read requested with invalid device index");
      return ADC_ERROR_INVALID_DEVICE_IDX;
   }
   else if (pDevCtxt->paDeviceData[pAdcInputProps->nDeviceIdx].deviceStatus ==
            ADC_DEVICE_NOT_AVAILABLE)
   {
     // DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
     //                 "ADC device not available.");
      return ADC_ERROR_INVALID_DEVICE_IDX;
   }
   else if (pAdcInputProps->nChannelIdx >=
            pDevCtxt->paDeviceData[pAdcInputProps->nDeviceIdx].uNumChannels)
   {
     // DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
     //                 "Adc read requested with invalid channel index");
      return ADC_ERROR_INVALID_CHANNEL_IDX;
   }

   Adc_LogConversionRequest(pAdcInputProps->nDeviceIdx, pAdcInputProps->nChannelIdx);

   //uQueueIdx = pDevCtxt->pBsp->paAdcPhysicalDevices[pAdcInputProps->nDeviceIdx].uQueueIdx;
   //pQueue = &pDevCtxt->paQueues[uQueueIdx];

   /* Acquiring this lock ensures this request doesn't conflict with the work loop */
   //qurt_mutex_lock(pQueue->hSync);

   result = Adc_ReadChannel(pDevCtxt,
                            pAdcInputProps->nDeviceIdx,
                            pAdcInputProps->nChannelIdx,
                            NULL,
                            pAdcResult);

   //qurt_mutex_unlock(pQueue->hSync);

   return result;
}

//DALResult
//Adc_RequestConversion(
//   AdcClientCtxt *pCtxt,
//   const AdcRequestParametersType *pAdcParams,
//   AdcRequestStatusType *pAdcRequestStatus
//   )
//{
//   DALResult ret;
//   AdcDevCtxt *pDevCtxt = pCtxt->pAdcDevCtxt;
//
//   /* Initialize request status to error */
//   if (pAdcRequestStatus != NULL)
//   {
//      pAdcRequestStatus->eStatus = ADC_REQUEST_STATUS_ERROR;
//   }
//
//   if (pAdcParams == NULL || pAdcParams->hEvent == NULL)
//   {
//      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
//                      "Adc conversion requested with a NULL pointer parameter");
//      return ADC_ERROR_NULL_POINTER;
//   }
//   else if (pAdcParams->nDeviceIdx >= pDevCtxt->pBsp->uNumPhysicalDevices)
//   {
//      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
//                      "Adc conversion requested with invalid device index");
//      return ADC_ERROR_INVALID_DEVICE_IDX;
//   }
//   else if (pDevCtxt->paDeviceData[pAdcParams->nDeviceIdx].deviceStatus ==
//            ADC_DEVICE_NOT_AVAILABLE)
//   {
//      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
//                      "ADC device not available.");
//      return ADC_ERROR_INVALID_DEVICE_IDX;
//   }
//   else if (pAdcParams->nChannelIdx >=
//            pDevCtxt->paDeviceData[pAdcParams->nDeviceIdx].uNumChannels)
//   {
//      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
//                      "Adc conversion requested with invalid channel index");
//      return ADC_ERROR_INVALID_CHANNEL_IDX;
//   }
//
//   /* Log the ADC conversion request */
//   Adc_LogConversionRequest(pAdcParams->nDeviceIdx, pAdcParams->nChannelIdx);
//
//   ret = Adc_AddDeviceQueueRequest(pCtxt,
//                                   ADC_REQUEST_CONVERSION,
//                                   pAdcParams->nDeviceIdx,
//                                   pAdcParams->nChannelIdx,
//                                   pAdcParams->hEvent,
//                                   pAdcRequestStatus);
//
//   return ret;
//}

//DALResult
//Adc_RequestRecalibration(
//   AdcClientCtxt *pCtxt,
//   const AdcRequestParametersType *pAdcParams
//   )
//{
//   DALResult ret;
//   AdcDevCtxt *pDevCtxt = pCtxt->pAdcDevCtxt;
//
//   if (pAdcParams == NULL)
//   {
//      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
//                      "Adc recalibration requested with a NULL pointer for pAdcParams");
//      return ADC_ERROR_NULL_POINTER;
//   }
//   else if (pAdcParams->nDeviceIdx >= pDevCtxt->pBsp->uNumPhysicalDevices)
//   {
//      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
//                      "Adc calibration requested with invalid device index");
//      return ADC_ERROR_INVALID_DEVICE_IDX;
//   }
//   else if (pDevCtxt->paDeviceData[pAdcParams->nDeviceIdx].deviceStatus ==
//            ADC_DEVICE_NOT_AVAILABLE)
//   {
//      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
//                      "ADC Device not available.");
//      return ADC_ERROR_INVALID_DEVICE_IDX;
//   }
//   else if (pAdcParams->nChannelIdx >=
//            pDevCtxt->paDeviceData[pAdcParams->nDeviceIdx].uNumChannels)
//   {
//      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
//                      "Adc conversion requested with invalid channel index");
//      return ADC_ERROR_INVALID_CHANNEL_IDX;
//   }
//
//   /* Log the ADC conversion request */
//   Adc_LogRecalibrationRequest(pAdcParams->nDeviceIdx, pAdcParams->nChannelIdx);
//
//   ret = Adc_AddDeviceQueueRequest(pCtxt,
//                                   ADC_REQUEST_CHANNEL_RECALIBRATION,
//                                   pAdcParams->nDeviceIdx,
//                                   pAdcParams->nChannelIdx,
//                                   pAdcParams->hEvent,
//                                   NULL);
//
//   return ret;
//}


DALResult
Adc_RegisterClient(AdcClientCtxt *pCtxt)
{
   static uint32 uClientId = 0;

   pCtxt->uClientId = uClientId;

   uClientId++;

   return DAL_SUCCESS;
}

DALResult
Adc_DeregisterClient(AdcClientCtxt *pCtxt)
{
   
   AdcDevCtxt *pDevCtxt = pCtxt->pAdcDevCtxt;
   AdcDeviceDataType *pDeviceData;
   uint32 uDeviceIdx;

   for (uDeviceIdx = 0; uDeviceIdx < pDevCtxt->pBsp->uNumPhysicalDevices; uDeviceIdx++)
   {
      /* Get the pointer to the device data corresponding to this device index */
      pDeviceData = &pDevCtxt->paDeviceData[uDeviceIdx];

      if (pDeviceData->deviceStatus == ADC_DEVICE_NOT_AVAILABLE || pDeviceData->phDevice == NULL)
      {
         continue;
      }
   }

   return DAL_SUCCESS;
}






