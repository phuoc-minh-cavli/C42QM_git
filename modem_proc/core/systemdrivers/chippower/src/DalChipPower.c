/*============================================================================

FILE: DalChipPowerMaster.c

DESCRIPTION:
  This file contains the main implementation of the ChipPower Driver.

PUBLIC CLASSES:
  ChipPower_RegisterCallback
  ChipPower_PerformCallbacks
  ChipPower_PrepareForPowerEvent

===============================================================================
  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR

  $Header: //components/rel/core.mpss/3.10/systemdrivers/chippower/src/DalChipPower.c#2 $
  $DateTime: 2019/03/25 08:08:19 $
  $Author: pwbldsvc $
=============================================================================*/

#include "DalChipPower.h"
#include "dog.h"
#include "err.h"
#include <stdlib.h>


/*==============================================================================

                         DAL INFO FILE DEFINITIONS

==============================================================================*/

static DALDEVICEID DalChipPower_DeviceId[1] = {DALDEVICEID_CHIPPOWER};

const DALREG_DriverInfo
DALChipPower_DriverInfo = { ChipPower_DeviceAttach, 1, DalChipPower_DeviceId };


/*==============================================================================

                            GENERIC DAL FUNCTIONS

==============================================================================*/

static uint32
ChipPower_AddRef(DalChipPowerHandle* h)
{
  return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

DALResult
ChipPower_DriverInit(ChipPowerDrvCtxt *pDrvCtxt)
{
  pDrvCtxt->syncObj = (qurt_mutex_t*)malloc(sizeof(qurt_mutex_t));
  if (pDrvCtxt->syncObj == NULL)
  {
    ERR_FATAL("Failed to allocate memory to the synchronization object", 0, 0, 0);
  }
  qurt_mutex_init(pDrvCtxt->syncObj);

  return DAL_SUCCESS;
}

DALResult
ChipPower_DriverDeInit(ChipPowerDrvCtxt *pdrvCtxt)
{
  return DAL_SUCCESS;
}

static uint32
ChipPower_DeviceDetach(DalDeviceHandle* h)
{
  uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
  return dwref;
}

static DALResult
ChipPower_DeviceInit(DalDeviceHandle *h)
{
  return DAL_SUCCESS;
}

static DALResult
ChipPower_DeviceDeInit(DalDeviceHandle *h)
{
  return DAL_SUCCESS;
}

static DALResult
ChipPower_DevicePowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd,
                           DalPowerDomain PowerDomain )
{
  return DAL_SUCCESS;
}

static DALResult
ChipPower_DeviceOpen(DalDeviceHandle* h, uint32 mode)
{
  return DAL_SUCCESS;
}

static DALResult
ChipPower_DeviceClose(DalDeviceHandle* h)
{
  return DAL_SUCCESS;
}

static DALResult
ChipPower_DeviceInfo(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
  info->Version = DALCHIPPOWER_INTERFACE_VERSION;
  return DAL_SUCCESS;
}

static DALResult
ChipPower_DeviceSysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                           const void* SrcBuf, uint32 SrcBufLen,
                           void* DestBuf, uint32 DestBufLen,
                           uint32* DestBufLenReq)
{
  return DAL_SUCCESS;
}


/*==============================================================================

                          CHIPPOWER API FUNCTIONS

==============================================================================*/

/*==========================================================================

  FUNCTION      ChipPower_RegisterCallback

  DESCRIPTION   Registers a callback function to be called before chip is
                reset or powered off.

  PARAMETERS
    h         - DAL device handle.
    cbFunc    - Function to callback to.
    cbFuncCtx - Context of the callback function.

  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

==========================================================================*/

static DALResult ChipPower_RegisterCallback
(
  DalDeviceHandle         *h,
  ChipPowerCallbackFunc    cbFunc,
  ChipPowerCallbackFuncCtx cbFuncCtx
)
{
  ChipPowerDescNodeType *pNode;
  ChipPowerDrvCtxt      *pDrvCtxt;
  ChipPowerEventObj     *pChipPowerEventObj;

  pDrvCtxt = ((ChipPowerClientCtxt *)h->pClientCtxt)->
               pChipPowerDevCtxt->pChipPowerDrvCtxt;

  /* Allocate event queue entry. */
  pNode = (ChipPowerDescNodeType*)malloc(sizeof(ChipPowerDescNodeType));
  if (pNode == NULL)
  {
    return DAL_ERROR;
  }
  pNode->pEventObj = (ChipPowerEventObj*)malloc(sizeof (ChipPowerEventObj));
  if (pNode->pEventObj == NULL)
  {
    return DAL_ERROR;
  }

  pChipPowerEventObj = pNode->pEventObj;
  pChipPowerEventObj->cbFuncCtx = cbFuncCtx;
  pChipPowerEventObj->cbFunc = cbFunc;

  /* Insert event into event queue. */
  qurt_mutex_lock(pDrvCtxt->syncObj);
  ChipPowerQueueNode(&pDrvCtxt->eventQueue, pNode);
  qurt_mutex_unlock(pDrvCtxt->syncObj);

  return DAL_SUCCESS;

} /* END ChipPower_RegisterCallback */


/*==========================================================================

  FUNCTION      ChipPower_PerformCallbacks

  DESCRIPTION   Executes each function that was registerd for a callback.

  PARAMETERS
    pDrvCtxt  - Pointer to the driver context.
    eventType - Type of power event (reset or power-off).

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void ChipPower_PerformCallbacks
(
  ChipPowerDrvCtxt   *pDrvCtxt,
  ChipPowerEventType  eventType
)
{
  ChipPowerDescNodeType *pNode;
  ChipPowerEventObj     *pChipPowerEventObj;

  /*
   * This is expected to be invoked in STM.
   * Not recommended to have a mutex here.
   */
  // qurt_mutex_lock(pDrvCtxt->syncObj);

  pNode = ChipPowerDequeueNode(&pDrvCtxt->eventQueue);
  if (pNode != NULL)
  {
    pChipPowerEventObj = pNode->pEventObj;
    while (pChipPowerEventObj != NULL)
    {
      pChipPowerEventObj->cbFunc(
        pChipPowerEventObj->cbFuncCtx,
        eventType, NULL, NULL);
      pNode = ChipPowerDequeueNode(&pDrvCtxt->eventQueue);
      if (pNode == NULL)
      {
        break;
      }
      pChipPowerEventObj = pNode->pEventObj;
    }
  }
  // qurt_mutex_unlock(pDrvCtxt->syncObj);

} /* END ChipPower_PerformCallbacks */


/*==========================================================================

  FUNCTION      ChipPower_PrepareForPowerEvent

  DESCRIPTION   Prepares the driver for a power event.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void ChipPower_PrepareForPowerEvent
(
  void
)
{
  /* Ensure that the dog is running. */
  dog_autokick(FALSE);

} /* END ChipPower_PrepareForPowerEvent */

static void ChipPower_InitInterface
(
  ChipPowerClientCtxt *pclientCtxt
)
{
  static const DalChipPower vtbl =
  {
    {
      ChipPower_DeviceAttach,
      ChipPower_DeviceDetach,
      ChipPower_DeviceInit,
      ChipPower_DeviceDeInit,
      ChipPower_DeviceOpen,
      ChipPower_DeviceClose,
      ChipPower_DeviceInfo,
      ChipPower_DevicePowerEvent,
      ChipPower_DeviceSysRequest
    },

    ChipPower_Reset,
    ChipPower_PowerOff,
    ChipPower_RegisterCallback
  };

  /*--------------------------------------------------------------------------
    Depending upon client type setup the vtables (entry points)
    --------------------------------------------------------------------------*/
  pclientCtxt->DalChipPowerHandle.dwDalHandleId =
    DALDEVICE_INTERFACE_HANDLE_ID;
  pclientCtxt->DalChipPowerHandle.pVtbl = &vtbl;
  pclientCtxt->DalChipPowerHandle.pClientCtxt = pclientCtxt;
}

DALResult ChipPower_DeviceAttach
(
  const char       *pszArg,
  DALDEVICEID       DeviceId,
  DalDeviceHandle **phDalDevice
)
{
  DALResult nErr;
  static ChipPowerDrvCtxt drvCtxt =
  {
    {
      ChipPower_DriverInit,
      ChipPower_DriverDeInit
    },
    1,
    sizeof(ChipPowerDevCtxt)
  };
  static ChipPowerClientCtxt clientCtxt;

  ChipPowerClientCtxt *pclientCtxt = &clientCtxt;

  *phDalDevice = NULL;

  nErr =
    DALFW_AttachToDevice(
      DeviceId,
      (DALDrvCtxt*)&drvCtxt,
      (DALClientCtxt*)pclientCtxt);

  if (DAL_SUCCESS == nErr)
  {
    ChipPower_InitInterface(pclientCtxt);
    ChipPower_AddRef(&(pclientCtxt->DalChipPowerHandle));
    *phDalDevice = (DalDeviceHandle*)&(pclientCtxt->DalChipPowerHandle);
  }

  return nErr;
}

ChipPowerDescNodeType* ChipPowerDequeueNode
(
  ChipPowerDescNodeType **ppList
)
{
  ChipPowerDescNodeType *pNode;

  if (*ppList == NULL)
  {
    return NULL;
  }

  pNode = *ppList;
  *ppList = pNode->pNext;

  return pNode;
}

void ChipPowerQueueNode
(
  ChipPowerDescNodeType **ppList,
  ChipPowerDescNodeType  *pNode
)
{
  ChipPowerDescNodeType *pQueuedNode;

  pNode->pNext = NULL;

  if (*ppList == NULL)
  {
    *ppList = pNode;
  }
  else
  {
    pQueuedNode = *ppList;
    while (pQueuedNode->pNext != NULL)
    {
      pQueuedNode = pQueuedNode->pNext;
    }
    pQueuedNode->pNext = pNode;
  }
}
