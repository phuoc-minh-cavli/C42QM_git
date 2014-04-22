/*
===========================================================================

FILE:   SpiDeviceOsSvc.c

DESCRIPTION:
    This file contains the implementation for the QUP OS Services 
    Interface: IQupSpiDevice 

===========================================================================

        Edit History


When     Who    What, where, why
-------- ---    -----------------------------------------------------------
09/11/17 vg     Added spi DAM changes
10/18/16 ska    Added changes to phy mem alloc for BAM mode support
10/04/16 ska    Added Fixes for KW issues Threadx
09/20/16 dpk    Ported for ThreadX
02/29/16 dpk    Added ISR un-registering API
04/20/12 ddk    Added updates to enable logging.
04/19/12 ddk    Added changes for avoiding symbol match with I2C library.
03/27/12 DK     Added Physical memory request functions.
09/26/11 LK     Created

===========================================================================
Copyright (c) 2017 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

===========================================================================
*/

#include "SpiDeviceOsSvc.h"
#include "DALStdDef.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "SpiLog.h"
#include "DALStdDef.h"
#include "qurt.h"
#include "dsr.h"
#include "qurt_signal.h"
#include "qurt_memory.h"
#include "busywait.h"
#include <stringl.h>




#define QUPBUSOSSVC_NUM_DEVICES_MAX       12
extern int32  aQupDalDevIds[];
extern uint32 IBUSPLAT_GetQupNum(void *QupHandle);

typedef enum SpiDeviceOs_HandleType
{
   SpiDeviceOs_EventHandleType,
   SpiDeviceOs_CritSecHandleType,
} SpiDeviceOs_HandleType;

typedef struct SpiDeviceOs_EventHandle
{
   SpiDeviceOs_HandleType hType;
   qurt_signal_t hQupEvents[2];
} SpiDeviceOs_EventHandle;

typedef struct SpiDeviceOs_SyncHandle
{
   SpiDeviceOs_HandleType hType;
} SpiDeviceOs_SyncHandle;

typedef struct SpiDeviceOs_Handle
{
   SpiDeviceOs_HandleType hType;
} SpiDeviceOs_Handle;

typedef struct SpiDeviceOs_IrqHandle
{
   ISR_HOOK IsrHook;
   IST_HOOK IstHook;
   void *pIsrData;
   void *pIstData;
   uint32 uInterruptId;
   uint32 uIrqTriggerCfg;
   dsr_obj_t *pIstHook;
} SpiDeviceOs_IrqHandle;

static struct SpiDeviceOs_InterruptController
{
   SpiDeviceOs_IrqHandle *aIrqHandles[QUPBUSOSSVC_NUM_DEVICES_MAX];
   DalDeviceHandle *hInterruptCtrl;
} qupIrqCtrl;

uint32 IBUSOS_CreateEvent(IBUSOS_EVENT_HANDLE *phEvt)
{
   uint32 res;
   SpiDeviceOs_EventHandle *pEvt;

   *phEvt = NULL;
   
   res = IBUSOS_Malloc((void **)&pEvt, sizeof(SpiDeviceOs_EventHandle));
   if (res != SPIDEVICE_RES_SUCCESS)
   {
      SPIDEVICE_LOG(ERROR, "IBUSOS_CreateEvent: Malloc failure\n");
      return res;
   }
   IBUSOS_Memset((void *)pEvt, 0, sizeof(SpiDeviceOs_EventHandle));

   /* Create a timeout object used to timeout operations.			   */
   qurt_signal_init(&pEvt->hQupEvents[0]);

   /* Create a wait object on which the client will wait.              */
   qurt_signal_init(&pEvt->hQupEvents[1]);

   pEvt->hType = SpiDeviceOs_EventHandleType;

   *phEvt = (IBUSOS_EVENT_HANDLE)pEvt;

   return res;
}

uint32 IBUSOS_DestroyEvent(SpiDeviceOs_EventHandle *pEvt)
{
   qurt_signal_destroy(&pEvt->hQupEvents[0]);
   qurt_signal_destroy(&pEvt->hQupEvents[1]);
   IBUSOS_Free(pEvt);
   pEvt = NULL;
   return SPIDEVICE_RES_SUCCESS;
}

uint32 IBUSOS_SetEvent(IBUSOS_EVENT_HANDLE hEvent)
{
   SpiDeviceOs_EventHandle *pEvt = (SpiDeviceOs_EventHandle *)hEvent;

   qurt_signal_set(&pEvt->hQupEvents[1], SPI_EVENT0_MASK);

   return SPIDEVICE_RES_SUCCESS;
}

uint32 IBUSOS_ClearEvent(IBUSOS_EVENT_HANDLE hEvt)
{
   uint32 res = SPIDEVICE_RES_SUCCESS;
   SpiDeviceOs_EventHandle *pEvt = (SpiDeviceOs_EventHandle *)hEvt;

   qurt_signal_clear(&pEvt->hQupEvents[0], SPI_EVENT0_MASK);

   qurt_signal_clear(&pEvt->hQupEvents[1], SPI_EVENT0_MASK);

   return res;
}

uint32 IBUSOS_WaitEvent(IBUSOS_EVENT_HANDLE hEvent, uint32 dwMilliseconds)
{
   SpiDeviceOs_EventHandle *pEvt = (SpiDeviceOs_EventHandle *)hEvent;
   uint32 timer_event = 0,res = SPIDEVICE_RES_SUCCESS;
   qurt_signal_wait(&(pEvt->hQupEvents[1]),SPI_EVENT0_MASK,QURT_SIGNAL_ATTR_CLEAR_MASK);
   return res;
}

uint32 IBUSOS_DestroyCriticalSection(SpiDeviceOs_SyncHandle *pCritSec)
{
   return SPIDEVICE_RES_SUCCESS;
}

uint32 IBUSOS_CloseHandle(IBUSOS_EVENT_HANDLE hEvt)
{
   SpiDeviceOs_Handle *hOsEvt = (SpiDeviceOs_Handle *)hEvt;

   if (SpiDeviceOs_EventHandleType == hOsEvt->hType)
   {
      return IBUSOS_DestroyEvent((SpiDeviceOs_EventHandle *)hEvt);
   } else if (SpiDeviceOs_CritSecHandleType == hOsEvt->hType)
   {
      return IBUSOS_DestroyCriticalSection((SpiDeviceOs_SyncHandle *)hEvt);
   } else
   {
      return IBUSOS_ERROR_INVALID_HANDLE_TYPE;
   }
}

uint32 IBUSOS_Malloc(void **pBuffer, uint32 size)
{
   if(NULL == pBuffer)
   {
      return IBUSOS_ERROR_MEM_MALLOC_FAILED;
   }
   *pBuffer = malloc(size);
   if ((NULL == *pBuffer))
   {
      return IBUSOS_ERROR_MEM_MALLOC_FAILED;
   }
   return SPIDEVICE_RES_SUCCESS;
}

uint32 IBUSOS_Memset(void *buffer, int32 val, uint32 size)
{
   memset(buffer, val, size);
   return SPIDEVICE_RES_SUCCESS;
}

uint32 IBUSOS_Free(void *pBuffer)
{
   free(pBuffer);
   pBuffer = NULL;
   return SPIDEVICE_RES_SUCCESS;
}

static void* IBUSOS_Isr (void *pCtxt)
{
   SpiDeviceOs_IrqHandle *pIrqHandle = (SpiDeviceOs_IrqHandle *)pCtxt;

   if (NULL == pIrqHandle)
   {
      return NULL;
   }
   pIrqHandle->IsrHook(pIrqHandle->pIsrData);

   if(!dsr_enqueue(pIrqHandle->pIstHook,NULL))
   {
      //SPIDEVICE_LOG(ERROR, "IBUSOS_Isr(): DSR Enqueue failed\n");
   }
   
   return NULL;
}

static void IBUSOS_Ist (dsr_obj_t *dsr_obj, void *pCtxt, void *payload)
{
   SpiDeviceOs_IrqHandle *pIrqHandle = (SpiDeviceOs_IrqHandle *)pCtxt;
	
   if (NULL == pIrqHandle)
   {
      return;
   }
   pIrqHandle->IstHook(pIrqHandle->pIstData);
}

uint32
IBUSOS_RegisterISTHooks(void *QupHandle, ISR_HOOK IsrHook, void *pIsrData, IST_HOOK IstHook, void *pIstData)
{
   DALResult dalRes;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   SpiDeviceOs_IrqHandle *pIrqHandle;
   uint32 res = SPIDEVICE_RES_SUCCESS;
   uint32 uInterruptId;
   uint32 uIrqTriggerCfg;
   uint32 uDevId = IBUSPLAT_GetQupNum(QupHandle);

   if (NULL == qupIrqCtrl.hInterruptCtrl)
   {
      dalRes =
         DAL_InterruptControllerDeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &qupIrqCtrl.hInterruptCtrl);
      if ((DAL_SUCCESS != dalRes) || (NULL == qupIrqCtrl.hInterruptCtrl))
      {
         return IBUSOS_ERROR_INT_CTRL_ATTACH;
      }
   }

   if (NULL == qupIrqCtrl.aIrqHandles[uDevId])
   {
      if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle(aQupDalDevIds[uDevId], hProp))
      {
         return IBUSOS_ERROR_DAL_PROPERTY_FAILED;
      }
      if (DAL_SUCCESS != DALSYS_GetPropertyValue(hProp, "CHIP_IRQ_ID", 0, &PropVar))
      {
         return IBUSOS_ERROR_DAL_PROPERTY_CHIP_IRQ_ID_FAILED;
      }
      uInterruptId = (uint32)PropVar.Val.dwVal;
      if (DAL_SUCCESS != DALSYS_GetPropertyValue(hProp, "CHIP_IRQ_TRIGGER_CFG", 0, &PropVar))
      {
         return IBUSOS_ERROR_DAL_PROPERTY_CHIP_IRQ_TRIG_CFG_FAILED;
      }
      uIrqTriggerCfg = PropVar.Val.dwVal;

      res = IBUSOS_Malloc((void **)&pIrqHandle, sizeof(SpiDeviceOs_IrqHandle));
      if (SPIDEVICE_RES_SUCCESS != res)
      {
         return IBUSOS_ERROR_MEM_MALLOC_FAILED;
      }
      IBUSOS_Memset((void *)pIrqHandle, 0, sizeof(SpiDeviceOs_IrqHandle));
      pIrqHandle->uInterruptId = uInterruptId;
      pIrqHandle->uIrqTriggerCfg = uIrqTriggerCfg;
      qupIrqCtrl.aIrqHandles[uDevId] = pIrqHandle;
   } else
   {
      pIrqHandle = qupIrqCtrl.aIrqHandles[uDevId];
   }
   pIrqHandle->IsrHook = IsrHook;
   pIrqHandle->IstHook = IstHook;
   pIrqHandle->pIsrData = pIsrData;
   pIrqHandle->pIstData = pIstData;
   dalRes = DalInterruptController_RegisterISR(
      qupIrqCtrl.hInterruptCtrl,
      pIrqHandle->uInterruptId,
      IBUSOS_Isr,
      (DALISRCtx)pIrqHandle,
      pIrqHandle->uIrqTriggerCfg);
   if (dalRes != DAL_SUCCESS)
   {
      // the context (pIrqHandle) never gets deallocated it is part of the data
      return IBUSOS_ERROR_REGISTER_ISR;
   }

   /* Create DSR */
   if (!dsr_create(&pIrqHandle->pIstHook, IBUSOS_Ist, (void *)pIrqHandle, DSR_PRI_MEDIUM, DSR_CONTEXT_ISR))
   {
      return IBUSOS_ERROR_REGISTER_DSR;
   }
   	
   return SPIDEVICE_RES_SUCCESS;
}

uint32 IBUSOS_UnRegisterISTHooks(void *QupHandle)
{
   DALResult dalRes;
   uint32 res = SPIDEVICE_RES_SUCCESS;
   SpiDeviceOs_IrqHandle *pIrqHandle;
   uint32 uDevId = IBUSPLAT_GetQupNum(QupHandle);
   
   if (NULL == qupIrqCtrl.hInterruptCtrl) {
      return IBUSOS_ERROR_NULL_PTR; 
   }

   pIrqHandle = qupIrqCtrl.aIrqHandles[uDevId];
   dalRes = DalInterruptController_Unregister(qupIrqCtrl.hInterruptCtrl, pIrqHandle->uInterruptId);
   if (dalRes != DAL_SUCCESS) 
   {
      return IBUSOS_ERROR_UNREGISTER_ISR;
   }

   if (!dsr_destroy(pIrqHandle->pIstHook))
   {
      return IBUSOS_ERROR_UNREGISTER_DSR;
   }

   res = IBUSOS_Free(pIrqHandle);
   if (SPIDEVICE_RES_SUCCESS != res)
   {
      return res;
   }

   qupIrqCtrl.aIrqHandles[uDevId] = NULL;
   
   return SPIDEVICE_RES_SUCCESS;
}

void IBUSOS_BusyWait(uint32 uTimeMicrosec)
{
   busywait(uTimeMicrosec);
}

uint32 IBUSOS_PhysMemAlloc (uint32 uLen, void **phMem, void **ppVirtAddr, void **ppPhysAddr)
{
   int32 result;   
   qurt_mem_region_t mem_region;
   qurt_mem_pool_t pool = qurt_mem_default_pool;
   qurt_mem_region_attr_t pre_alloc_attr;

   
   if ((NULL == ppPhysAddr) || (NULL == ppVirtAddr))
   {
      return IBUSOS_ERROR_NULL_PTR;
   }
   
   pre_alloc_attr.physaddr      = 0xFFFFFFFF;
   pre_alloc_attr.virtaddr      = 0xFFFFFFFF;
   pre_alloc_attr.mapping_type  = QURT_MEM_MAPPING_VIRTUAL;
   pre_alloc_attr.region_type   = QURT_MEM_REGION_NORMAL;
   pre_alloc_attr.permission    = QURT_PERM_FULL;
   pre_alloc_attr.cache_attribs = QURT_MEM_CACHE_OUTER_INNER_UNCACHED;
   pre_alloc_attr.size          = 0;
   pre_alloc_attr.asid          = 0;
   
   qurt_mem_region_attr_init(&pre_alloc_attr);
   
   result = qurt_mem_region_create(&mem_region, uLen, pool, &pre_alloc_attr);
   if (result != QURT_EOK) 
   {
      return IBUSOS_ERROR_PHYS_MEM_ALLOC_FAILED; 
   }
   
   result = qurt_mem_region_attr_get(mem_region, &pre_alloc_attr);
   if (result != QURT_EOK) 
   {
      return IBUSOS_ERROR_PHYS_MEM_INFO_FAILED; 
   }   
   
   *phMem	  = (void *)mem_region;
   *ppPhysAddr = (void *)pre_alloc_attr.physaddr;
   *ppVirtAddr = (void *)pre_alloc_attr.virtaddr;
   
   return result;
}

uint32 IBUSOS_PhysMemFree(void *phMem)
{
   int32 res = SPIDEVICE_RES_SUCCESS;
   do
   {
      if ( NULL == phMem ) 
      {
         res = IBUSOS_ERROR_NULL_PTR;
		 break;
      }
      res  = qurt_mem_region_delete((qurt_mem_region_t)phMem);
      if ( res != QURT_EOK ) 
      {
         res = IBUSOS_ERROR_MEM_REGION_DEL_FAILED;
		 break;
      }
   }while(0);

   return res; 
}

uint32 IBUSOS_Memcpy(void *dst,uint32 dest_size, void *src, uint32 src_size)
{
   uint32 copySize = 0;
   copySize = memscpy(dst, dest_size, src, src_size);

   if (copySize < src_size)
   {
      return IBUSOS_ERROR_MEM_COPYS_FAILED;
   }
   else
   {
      return SPIDEVICE_RES_SUCCESS;
   }
}
