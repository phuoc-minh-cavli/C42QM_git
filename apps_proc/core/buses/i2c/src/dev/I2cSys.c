/*=============================================================================

  FILE:   I2cSys.c

  OVERVIEW: This file contains the implementation for the QUP OS Services 
            Interface: IQupI2cDevice 
 
    Copyright (c) 2011-2017 Qualcomm Technologies, Incorporated.
                        All rights reserved.
    Qualcomm Technologies, Confidential and Proprietary.
  ===========================================================================*/
/*=========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/buses/i2c/src/dev/I2cSys.c#1 $
  $DateTime: 2020/01/30 22:49:35 $$Author: pwbldsvc $

  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  09/11/17 VG     Added I2C DAM changes
  02/22/17 VG     Added changes to support memscpy
  06/10/16 PR	  Initialize qurt_mem_pool_t to default memory pool
  09/01/16 PR     Changed DALSyswait to Busywait in I2CSYS_BusyWait
  02/22/13 LK     Enabled timeout.
  09/26/11 LK     Created

  ===========================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cSys.h"
#include "DALStdDef.h"
#include "DALDeviceId.h"
#include "qurt.h"
#include "dsr.h"
#include "qurt_signal.h"
#include "qurt_memory.h"
#include "busywait.h"
#include <string.h>


/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/


typedef enum I2CSYS_Error
{
   I2CSYS_ERROR_BASE = I2CSYS_RESULT_FIRST_SECTION,

   I2CSYS_ERROR_TIMEOUT_EVT_CREATE_FAILED,
   I2CSYS_ERROR_CLIENT_EVT_CREATE_FAILED,
   I2CSYS_ERROR_MEM_FREE_FAILED,
   I2CSYS_ERROR_MEM_MALLOC_FAILED,
   I2CSYS_ERROR_PHYS_MEM_MALLOC_FAILED,
   I2CSYS_ERROR_PHYS_MEM_INFO_FAILED,
   I2CSYS_ERROR_MEM_SET_FAILED,
   I2CSYS_ERROR_TIMEOUT_EVT_CLEAR_FAILED,
   I2CSYS_ERROR_CLIENT_EVT_CLEAR_FAILED,
   I2CSYS_ERROR_EVT_CTRL_FAILED,
   I2CSYS_ERROR_EVT_WAIT_FAILED,
   I2CSYS_ERROR_SYNC_CREATE_FAILED,
   I2CSYS_ERROR_SYNC_DESTROY_FAILED,
   I2CSYS_ERROR_INVALID_HANDLE_TYPE,
   I2CSYS_ERROR_NULL_PTR,
   I2CSYS_ERROR_GET_MEM_REGION_FAILED,
   I2CSYS_ERROR_CREATE_MEM_REGION_FAILED,
   I2CSYS_ERROR_MEM_REGION_DEL_FAILED,
} I2CSYS_Error;


typedef enum I2cOs_HandleType
{
   I2cOs_EventHandleType,
   I2cOs_CritSecHandleType,
} I2cOs_HandleType;

typedef struct I2cOs_EventHandle
{
   I2cOs_HandleType    hType;
   qurt_signal_t        hQupEvents[2];
   qurt_mutex_t         aQupEventObj[2];

} I2cOs_EventHandle;

typedef struct I2cOs_SyncHandle
{
   I2cOs_HandleType    hType;
   qurt_mutex_t  hDeviceSynchronization;
} I2cOs_SyncHandle;


typedef struct I2cOs_Handle
{
   I2cOs_HandleType    hType;
} I2cOs_Handle;


/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Creates an event object and sets handle pointer.

    @param[out] pEventHandle  Pointer to event handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_CreateEvent(I2CSYS_EVENT_HANDLE *phEvt)
{
   int32 res;
   I2cOs_EventHandle *pEvt;
   
   do {
      res = I2CSYS_Malloc((void **)&pEvt, sizeof(I2cOs_EventHandle));
      if ( I2CSYS_RESULT_OK != res ) {
         return res;
      }

      qurt_signal_init(&(pEvt->hQupEvents[0]));
      qurt_signal_init(&(pEvt->hQupEvents[1]));

      pEvt->hType = I2cOs_EventHandleType;

   } while ( 0 );

   *phEvt = (I2CSYS_EVENT_HANDLE) pEvt;
 

   return res;
}



int32
I2CSYS_DestroyEvent(I2cOs_EventHandle *pEvt)
{
   int32 res;

   res = I2CSYS_Free(pEvt);
   return res;
}


/** @brief Signals the event object pointed to by the handle.

    @param[in] hEvent  Event handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_SetEvent
(
   I2CSYS_EVENT_HANDLE hEvent
)
{
  
   I2cOs_EventHandle *pEvt = (I2cOs_EventHandle *)hEvent;

   qurt_signal_set(&(pEvt->hQupEvents[1]),I2C_EVENT1_MASK);	  
   
   return I2CSYS_RESULT_OK;
}

/** @brief Clears outstanding signals on the event object.

    @param[in] hEvent  Event handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_ClearEvent
(
   I2CSYS_EVENT_HANDLE hEvt
)
{
   
   int32 res = I2CSYS_RESULT_OK;
   I2cOs_EventHandle *pEvt = (I2cOs_EventHandle *)hEvt;

   qurt_signal_clear(&(pEvt->hQupEvents[0]),I2C_EVENT0_MASK);

   qurt_signal_clear(&(pEvt->hQupEvents[1]),I2C_EVENT1_MASK);

   return res;
}

/** @brief Wait with a timeout on the event object.

    @param[in] hEvent  Event handle.
    @param[in] dwMilliseconds  Event wait timeout.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_Wait
(
   I2CSYS_EVENT_HANDLE hEvent,
   uint32 dwMilliseconds
)
{

   int32  dalRes;
   uint32 timer_event = 0;
   I2cOs_EventHandle *pEvt = (I2cOs_EventHandle *)hEvent;
   
    
   
   dalRes = qurt_signal_wait_timed_ext(&(pEvt->hQupEvents[1]),I2C_EVENT1_MASK,QURT_SIGNAL_ATTR_WAIT_ANY,
   	                      &timer_event,dwMilliseconds*1000);
   

   return (dalRes == QURT_EOK)       ? (I2C_SUCCESS) :
          (dalRes == QURT_EFAILED_TIMEOUT) ? (I2C_TIMEOUT) :
                                          (I2C_ERROR);
}


int32
I2CSYS_DestroyCriticalSection(I2cOs_SyncHandle *pCritSec)
{
   int32 res = I2CSYS_RESULT_OK;

   qurt_mutex_destroy(&(pCritSec->hDeviceSynchronization));

   I2CSYS_Free(pCritSec);


   return res;
}


/** @brief Closes a handle destroying the object associated with it.

    @param[in] hEvent  Event handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_CloseHandle(I2CSYS_EVENT_HANDLE hEvt)
{
   I2cOs_Handle *hOsEvt = (I2cOs_Handle *)hEvt;

   if (I2cOs_EventHandleType == hOsEvt->hType) {
      return I2CSYS_DestroyEvent((I2cOs_EventHandle *)hEvt);
   }
   else if (I2cOs_CritSecHandleType == hOsEvt->hType) {
      return I2CSYS_DestroyCriticalSection((I2cOs_SyncHandle *)hEvt);
   }
   else {
      return I2CSYS_ERROR_INVALID_HANDLE_TYPE;
   }
}

/** @brief Creates a critical section object and sets the handle pointer.

    @param[out] pCritSecHandle  Pointer to critical section handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_CreateCriticalSection
(
   I2CSYS_CRITSECTION_HANDLE *pCritSecHandle
)
{
   I2cOs_SyncHandle *pCritSec ;
   int32 res = I2CSYS_RESULT_OK;

   res = I2CSYS_Malloc((void **)&pCritSec,sizeof(I2cOs_SyncHandle));
   if ( I2CSYS_RESULT_OK != res ) {
      return res;
   }
   qurt_mutex_init(&(pCritSec->hDeviceSynchronization));

   return res;
}

/** @brief Enters the critical section.

    @param[in] hCriticalSection  Critical section handle handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_EnterCriticalSection
(
   I2CSYS_CRITSECTION_HANDLE hCriticalSection
)
{
   I2cOs_SyncHandle *pCritSec = (I2cOs_SyncHandle *)hCriticalSection;
   /* Enter the Critical Section now */
   qurt_mutex_lock(&(pCritSec->hDeviceSynchronization));
   
   return I2CSYS_RESULT_OK;
}

/** @brief Leaves the critical section.

    @param[in] hCriticalSection  Critical section handle handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_LeaveCriticalSection
(
   I2CSYS_CRITSECTION_HANDLE hCriticalSection
)
{
   I2cOs_SyncHandle *pCritSec = (I2cOs_SyncHandle *)hCriticalSection;
   
   qurt_mutex_unlock(&(pCritSec->hDeviceSynchronization));
   return I2CSYS_RESULT_OK;
}

/** @brief Allocates a memory buffer of the given size and sets the pointer to it.

    @param[out] ppBuffer pointer to pointer to allocated memory.
    @param[in]  size     size of the memory to be allocated.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_Malloc
(
   void   **pBuffer,
   uint32   size
)
{
   void *pTmpBuffer=NULL;

   pTmpBuffer = malloc(size);
   
   if (NULL == pTmpBuffer) {
      return I2CSYS_ERROR_MEM_MALLOC_FAILED;
   }

   *pBuffer = pTmpBuffer;
   return I2CSYS_RESULT_OK;
}


/** @brief Frees memory allocated by I2CSYS_Malloc.

    @param[in] pBuffer  pointer to buffer.
    @param[in] c        character byte to init the memory.
    @param[in] size     the size of the buffer. 
    @return          I2CSYS_Result .
  */
  

int32
I2CSYS_Memset
(
   void  *pBuffer,
   int32  c,
   uint32 size
)
{
   memset(pBuffer, c, size);
   
   return I2CSYS_RESULT_OK;
}

/** @brief copies memory from src to desc.

    @param[in] dst        pointer to buffer.
    @param[in] dst_size   character byte to init the memory.
    @param[in] src        pointer to buffer.
    @param[in] size       the size of the buffer. 
    @return               I2CSYS_Result .
  */
  
int32
I2CSYS_MemsCpy
(
   void  *dst,
   uint32 dst_size,
   const void  *src,
   uint32 src_size
)
{  
   uint32 copied_bytes = 0;
   copied_bytes = memscpy(dst, dst_size, src,src_size);
   if(copied_bytes != src_size)
   {
      return I2C_ERROR;
   }
   return I2CSYS_RESULT_OK;
}

/** @brief Frees memory allocated by I2CSYS_Malloc.

    @param[in] pBuffer  pointer to allocated memory.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_Free
(
   void *pBuffer
)
{
  
   free(pBuffer);

   pBuffer = NULL;
   return I2CSYS_RESULT_OK;
}

/** @brief Busy waits the uTimeMicrosec microseconds before returning.

  @param[in] uTimeMicrosec  wait time in microseconds.
  @return          Nothing.
  */
void
I2CSYS_BusyWait
(
   uint32 uTimeMicrosec
)
{
   
   busywait(uTimeMicrosec);
}

/**
   @brief Allocate physical memory
 
   This function allocates physical memory
  
   @param[in] pPhysMem  Pointer to physical memory structure.
 
   @return I2CSYS_RESULT_OK on success, error code on error
 */
int32
I2CSYS_PhysMemAlloc
(
   I2CSYS_PhysMemDesc *pPhysMem
)
{
   int32 result;   
   qurt_mem_region_t mem_region;
   qurt_mem_pool_t    pool = qurt_mem_default_pool;
   qurt_mem_region_attr_t pre_alloc_attr,post_alloc_attr;
   

   if ( NULL == pPhysMem ) {
      return I2CSYS_ERROR_NULL_PTR;
   }
      pre_alloc_attr.physaddr     =  0xFFFFFFFF;
      pre_alloc_attr.virtaddr     =  0xFFFFFFFF;
      pre_alloc_attr.mapping_type = QURT_MEM_MAPPING_VIRTUAL;
	  pre_alloc_attr.region_type  = QURT_MEM_REGION_NORMAL;
	  pre_alloc_attr.cache_attribs = QURT_MEM_CACHE_OUTER_INNER_UNCACHED;
	  pre_alloc_attr.permission   = QURT_PERM_FULL;
	  pre_alloc_attr.size         = 0;
	  pre_alloc_attr.asid         = 0;
	  
	  qurt_mem_region_attr_init(&pre_alloc_attr);
	  
	  result = qurt_mem_region_create(&mem_region, pPhysMem->uSize, pool, &pre_alloc_attr);
	  if (result != QURT_EOK) { return(I2CSYS_ERROR_CREATE_MEM_REGION_FAILED); }
	  
   
	  result = qurt_mem_region_attr_get(mem_region, &pre_alloc_attr);
	  if (result != QURT_EOK) { return(I2CSYS_ERROR_GET_MEM_REGION_FAILED); }   
   
	  pPhysMem = malloc(sizeof(I2CSYS_PhysMemDesc));
	  if (pPhysMem == NULL) { return(I2CSYS_ERROR_PHYS_MEM_MALLOC_FAILED); }
   
   
	  pPhysMem->pObj	     =  mem_region;
	  pPhysMem->pPhysAddress = (void *)pre_alloc_attr.physaddr;
	  pPhysMem->pVirtAddress = (void *)pre_alloc_attr.virtaddr;
	  pPhysMem->uSize	     = pre_alloc_attr.size;
	  
	  return result;
}


/**
   @brief Release physical memory
 
   This function releases physical memory
  
   @param[in] pPhysMem  Pointer to physical memory structure.
 
   @return I2CSYS_RESULT_OK on success, error code on error
 */
int32
I2CSYS_PhysMemFree
(
   I2CSYS_PhysMemDesc *pPhysMem
)
{
   int32 res;   

   if ( NULL == pPhysMem ) {
      return I2CSYS_ERROR_NULL_PTR;
   }

   do{
   res  =  I2CSYS_Free(pPhysMem);
   if(res != I2C_SUCCESS){
   	  res  = I2CSYS_ERROR_MEM_FREE_FAILED;
   }
   
   }while(0);
   
   return   res;
}



