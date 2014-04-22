/*=============================================================================

  FILE:   I2cDriver.c

  OVERVIEW:     This file contains the implementation of QUPI2CDriver API
  DEPENDENCIES: I2CDRV_AddDevice must be called first then other api's.
 
    Copyright (c) 2011-2017,2019 Qualcomm Technologies, Incorporated.
                        All rights reserved.
    Qualcomm Technologies, Confidential and Proprietary.
  ===========================================================================*/

/*=========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/buses/i2c/src/drv/I2cDriver.c#1 $
  $DateTime: 2020/01/30 22:49:35 $$Author: pwbldsvc $

  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  03/04/19 SS     Added I2C client callback issue fix.
  02/22/17 VG     Added changes to support async apis
  09/15/16 PR	  removed dsr_enque for polling mode
  09/01/16 PR     Replace DalSys based events to qurt based events 
  09/26/11 LK     Created
 

  ===========================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cDriver.h"
#include "I2cError.h"
#include "I2cDevice.h"
#include "I2cSys.h"
#include "I2cLog.h"
#include "qurt_signal.h"
#include "qurt_mutex.h"
#include "dsr.h"
/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

#define QUI2CDRV_REQUEST_QUEUE_SIZE            16
#define FIRST_TIME   						   1
#define I2C_SIG_MASK                           0x00000001


#define I2CDRV_VALIDATE_SYNC_TRAN(hDev,pTransfer,pClntCfg,puNumCompleted)\
     ( (NULL == hDev)?           I2CDRV_ERR_INVALID_DEV_HANDLE:           \
       (NULL == pTransfer)?      I2CDRV_ERR_INVALID_TRANSFER_POINTER:     \
       (NULL == puNumCompleted)? I2CDRV_ERR_INVALID_NUMCOMPLETED_POINTER: \
       I2C_RES_SUCCESS                                                    \
     )

#define I2CDRV_VALIDATE_SYNC_SEQ(hDev,pSequence,pClntCfg,puNumCompleted) \
     ( (NULL == hDev)?           I2CDRV_ERR_INVALID_DEV_HANDLE:           \
       (NULL == pSequence)?      I2CDRV_ERR_INVALID_SEQUENCE_POINTER:     \
       (NULL == puNumCompleted)? I2CDRV_ERR_INVALID_NUMCOMPLETED_POINTER: \
       I2C_RES_SUCCESS                                                    \
     )

#define I2CDRV_VALIDATE_ASYNC_TRAN(hDev,pTransfer,pClntCfg,pIoRes,pCallbackFn,pArg)\
     ( (NULL == hDev)?           I2CDRV_ERR_INVALID_DEV_HANDLE:           \
       (NULL == pTransfer)?      I2CDRV_ERR_INVALID_TRANSFER_POINTER:     \
       (NULL == pIoRes)?       I2CDRV_ERR_INVALID_TRAN_RESULT_POINTER:  \
       (NULL == pCallbackFn)?    I2CDRV_ERR_INVALID_CALLBACK_FN_POINTER:  \
       (NULL == pArg)?           I2CDRV_ERR_INVALID_CALLBACK_ARG_POINTER:  \
       I2C_RES_SUCCESS                                                    \
     )
#define I2CDRV_VALIDATE_ASYNC_SEQ(hDev,pSequence,pClntCfg,pIoRes,pCallbackFn,pArg)\
     ( (NULL == hDev)?           I2CDRV_ERR_INVALID_DEV_HANDLE:           \
       (NULL == pSequence)?      I2CDRV_ERR_INVALID_SEQUENCE_POINTER:     \
       (NULL == pIoRes)?       I2CDRV_ERR_INVALID_TRAN_RESULT_POINTER:  \
       (NULL == pCallbackFn)?    I2CDRV_ERR_INVALID_CALLBACK_FN_POINTER:  \
       (NULL == pArg)?           I2CDRV_ERR_INVALID_CALLBACK_ARG_POINTER:  \
       I2C_RES_SUCCESS                                                    \
     )

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

enum  I2CDRV_Error
{
   I2CDRV_ERR_BASE = I2C_RES_ERROR_CLS_I2C_DRV,

   I2CDRV_ERR_FAILED_TO_ALLOCATE_DEVICE,
   I2CDRV_ERR_INVALID_DEVICE_ID,
   I2CDRV_ERR_FAILED_TO_REGISTER_WORKLOOP,
   I2CDRV_ERR_FAILED_TO_ENQUE_REQUEST,

   I2CDRV_ERR_FAILED_TO_CREATE_DRV_SYNC_HANDLE,
   I2CDRV_ERR_FAILED_TO_CREATE_WORKLOOP_HANDLE,
   I2CDRV_ERR_FAILED_TO_CREATE_NOT_FULL_EVENT,
   I2CDRV_ERR_FAILED_TO_CREATE_QUE_SYNC_HANDLE,
   I2CDRV_ERR_FAILED_TO_CREATE_FREE_REQ_LIST_SYNC_HANDLE,
   I2CDRV_ERR_FAILED_TO_CREATE_ENQ_REQ_LIST_SYNC_HANDLE,
   I2CDRV_ERR_FAILED_TO_CREATE_ENQ_REQ_LIST_EVENT_HANDLE,
   I2CDRV_ERR_FAILED_TO_CREATE_ASYNC_WAIT_EVENT_HANDLE,
   I2CDRV_ERR_FAILED_TO_CREATE_REQ_EVENT,
   I2CDRV_ERR_FAILED_TO_CLEAR_REQ_EVENT,
   I2CDRV_ERR_FAILED_TO_DESTROY_REQ_EVENT,
   I2CDRV_ERR_FAILED_TO_WAIT_ON_REQ_EVENT,
   I2CDRV_ERR_FAILED_TO_CLEAR_DAL_EVENT,
   I2CDRV_ERR_FAILED_DAL_EVENT_WAIT,


   I2CDRV_ERR_FAILED_TO_ASSOCIATE_WORKLOOP_EVENT,
   
   I2CDRV_ERR_FAILED_TO_INIT_QUE_INVALID_STATE,
   I2CDRV_ERR_FAILED_TO_CREATE_DRVDEV_SYNC_HANDLE,
   I2CDRV_ERR_FAILED_TO_CLEAR_WAIT_ASYNC_DONE_EVENT,

   I2CDRV_ERR_FREE_REQ_LIST_EMPTY,

   I2CDRV_ERR_INVALID_DEV_HANDLE,
   I2CDRV_ERR_INVALID_TRANSFER_POINTER,
   I2CDRV_ERR_INVALID_SEQUENCE_POINTER,
   I2CDRV_ERR_INVALID_NUMCOMPLETED_POINTER,
   I2CDRV_ERR_INVALID_TRAN_RESULT_POINTER,
   I2CDRV_ERR_INVALID_CALLBACK_FN_POINTER,
   I2CDRV_ERR_INVALID_CALLBACK_ARG_POINTER,
};


typedef struct I2CDRV_DalEvt
{
   qurt_signal_t                hEvt;
   dsr_obj_t                  *objEvt;
} I2CDRV_DalEvt;


typedef struct I2CDRV_DalSync
{
   qurt_mutex_t             hSync;
} I2CDRV_DalSync;


typedef struct I2CDRV_Request I2CDRV_Request;
struct I2CDRV_Request
{
   enum I2CDRV_ReqApiEnumType
   {
      I2CDRV_ReqApiEnumRead,
      I2CDRV_ReqApiEnumWrite,
      I2CDRV_ReqApiEnumBatch
   } eReqApiEnum;

   enum I2CDRV_ReqSyncEnumType
   {
      I2CDRV_ReqSyncEnumSynchronous,
      I2CDRV_ReqSyncEnumAsynchronous
   } eReqSyncEnum;

   struct I2CDRV_ReqVariant   
   {
	  I2cTransfer					 pTransfer;
	  I2cClientConfig				 pClntCfg;
	  uint32						 puNumCompleted;
	  I2cSequence					 pSequence;
	  int32 						 retVal;
	  I2cIoResult					 pIoRes;
	  DRV_ASYNC_CB					 pCallbackFn;
	  void							 *pArg;	
   }reqVariant;

   I2CDRV_DalEvt                    *pEvtReqComplete;
   I2CDRV_Request                   *pNext;
   boolean                          IsAsync;
};

typedef enum I2CDRV_ReqQueueInitState
{
      I2CDRV_EnqReqListState_WorkLoopNotCreated = 0,
      I2CDRV_EnqReqListState_WorkLoopCreated,
      I2CDRV_EnqReqListState_InitDone,

}I2CDRV_ReqQueueInitState;



typedef struct I2CDRV_FreeRequestList I2CDRV_FreeRequestList;
struct I2CDRV_FreeRequestList
{
   I2CDRV_Request           *pHead;
   I2CDRV_DalSync            syncFreeReqList;
   uint32                    uNumFreeReqs;
};

typedef struct I2CDRV_EnqueuedRequestList I2CDRV_EnqueuedRequestList;
struct I2CDRV_EnqueuedRequestList
{
   I2CDRV_Request           *pHead; /* remove from head. */
   I2CDRV_Request           *pTail; /* add to tail. */
   I2CDRV_DalEvt             evtEnqReqListNotEmpty;
   dsr_obj_t                 *wlReqProcessing;
   I2CDRV_DalSync            syncEnqReqList;
   uint32                    uNumEnqReqs;
   I2CDRV_ReqQueueInitState eInitState;
};

typedef struct I2CDRV_RequestQueue
{
   /* TODO: request quesize to be definet in properties, req que allocated dynamically. */
   I2CDRV_Request                aRequests[QUI2CDRV_REQUEST_QUEUE_SIZE];
   I2CDRV_FreeRequestList        freeReqList;
   I2CDRV_EnqueuedRequestList    enqReqList;

   /* timed event wait implemented in os layer. */
   I2CDRV_DalEvt                 waitForAsyncDoneEvt;
}I2CDRV_RequestQueue;


typedef struct I2CDRV_DevType I2CDRV_DevType;
typedef struct I2CDRV_DevType* I2CDRV_PDevType;

struct I2CDRV_DevType
{
   I2CDRV_RequestQueue  devReqQueue;

   I2CDEV_HANDLE     hDev;
   uint32            uDevId;
   uint32            uOpenedClientCount;

   I2CLOG_DevType   *pDevLog;

   I2CDRV_DalSync    syncDrvDev;
   I2CDRV_DevType*   pNext;
};

/*-------------------------------------------------------------------------
* Static Variable Definitions
* ----------------------------------------------------------------------*/

static I2CDRV_DevType* pDrvDevHead;


/*-------------------------------------------------------------------------
* Static Function Declarations and Definitions
* ----------------------------------------------------------------------*/
static void I2CDRV_ProcessNextTransaction( dsr_obj_t *dsr_obj, void *pInDevCtxt, void *payload);


/** @brief Searches for a device driver struct in the list.
  
    @param[in] uDevId    Platform id for the device.
    
    @return             Pointer to dev if found, otherwise NULL.
  */
static I2CDRV_DevType*
I2CDRV_SearchDevice
(
   uint32          uDevId 
)
{
   I2CDRV_DevType * pDevDrv = pDrvDevHead;
   while ( pDevDrv ) {
      if ( pDevDrv->uDevId == uDevId ) {
         return pDevDrv;
      }
      pDevDrv = pDevDrv->pNext;
   }

   return NULL;
}

/** @brief Links a device driver structure in the list.
  
    @param[in] pDevDrv  Pointer to device structure.
    
    @return             Nothing.
  */
static void
I2CDRV_LinkDevice
(
   I2CDRV_DevType* pDevDrv
)
{
   pDevDrv->pNext = pDrvDevHead;
   pDrvDevHead = pDevDrv;
}

/** @brief Initializes the request array.
  
    @param[in] pReqArr  Pointer to request array.
    @param[in] size     Array size.
    
    @return             Nothing.
  */
static void 
I2CDRV_InitRequestArray
(
   I2CDRV_Request *pReqArr,
   int32           size
)
{
   int32     arrIndex;
   
   if ( 0 == size ) {
      return;
   }

   pReqArr[size-1].pNext = NULL;
   for (arrIndex = size-2; arrIndex >= 0; arrIndex--) {
      pReqArr[arrIndex].pNext = &pReqArr[arrIndex+1];/* link the free request arrays. */
   }
}

/** @brief Initializes the free request array.
  
    @param[in] pDevDrv  Pointer to device structure.
    
    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_InitFreeRequestList
(
   I2CDRV_PDevType pDevDrv
)
{
   I2CDRV_RequestQueue     *pReqQueue;
   I2CDRV_FreeRequestList  *pFreeReqList;

   
   pReqQueue = &pDevDrv->devReqQueue;
   pFreeReqList = &pReqQueue->freeReqList;
   qurt_mutex_init(&(pFreeReqList->syncFreeReqList.hSync));
   I2CDRV_InitRequestArray(pReqQueue->aRequests, QUI2CDRV_REQUEST_QUEUE_SIZE);
   
   pFreeReqList->pHead = pReqQueue->aRequests;
   pFreeReqList->uNumFreeReqs = QUI2CDRV_REQUEST_QUEUE_SIZE;

   return I2C_RES_SUCCESS;
}

/** @brief Get a request from the free request array.
  
    @param[in]  pDevDrv Pointer to device structure.
    @param[out] ppReq   Pointer to pointer to request structure.
    
    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_GetFreeRequest
(
   I2CDRV_PDevType  pDevDrv,
   I2CDRV_Request **ppReq
)
{
   I2CDRV_FreeRequestList  *pFreeReqList;
   int32                    res;
   
   pFreeReqList = &pDevDrv->devReqQueue.freeReqList;

   qurt_mutex_lock(&(pFreeReqList->syncFreeReqList.hSync));

   if ( pFreeReqList->uNumFreeReqs > 0 ) { /* remove from list. */
      *ppReq = pFreeReqList->pHead;
      pFreeReqList->pHead = pFreeReqList->pHead->pNext;
      pFreeReqList->uNumFreeReqs--;
      res = I2C_RES_SUCCESS;
   }
   else {
      res = I2CDRV_ERR_FREE_REQ_LIST_EMPTY;
   }
  
   qurt_mutex_unlock(&(pFreeReqList->syncFreeReqList.hSync));

   return res;
}

/** @brief Returns a request to the free request array.
  
    @param[in]  pDevDrv  Pointer to device structure.
    @param[in]  pReq     Pointer to request structure.
    
    @return              Nothing.
  */
void
I2CDRV_ReleaseFreeRequest
(
   I2CDRV_PDevType pDevDrv,
   I2CDRV_Request *pReq
)
{
   I2CDRV_FreeRequestList  *pFreeReqList;
   pFreeReqList = &pDevDrv->devReqQueue.freeReqList;
   qurt_mutex_lock(&(pFreeReqList->syncFreeReqList.hSync));
   pReq->pNext = pFreeReqList->pHead;
   pFreeReqList->pHead = pReq;
   pFreeReqList->uNumFreeReqs++;
   qurt_mutex_unlock(&(pFreeReqList->syncFreeReqList.hSync));
}

/** @brief Initializes the enqueued request list.
  
    @param[in]  pDevDrv  Pointer to device structure.
    
    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_InitEnqueuedRequestList
(
   I2CDRV_PDevType pDevDrv
)
{
   int32 dalRes;
   I2CDRV_RequestQueue *pReqQueue = &pDevDrv->devReqQueue;
   I2CDRV_EnqueuedRequestList     *pEnqReqList = &pReqQueue->enqReqList;
   
   
   qurt_mutex_init(&(pEnqReqList->syncEnqReqList.hSync));
   
   qurt_signal_init(&(pEnqReqList->evtEnqReqListNotEmpty.hEvt));
  
   dalRes = (dsr_create(&(pEnqReqList->wlReqProcessing), (dsr_hdlr_t)I2CDRV_ProcessNextTransaction, (void *) pDevDrv, DSR_PRI_MEDIUM, DSR_CONTEXT_WORKLOOP) == 1 ? 0 : 1);
   if ( I2C_SUCCESS != dalRes ) {
      return I2CDRV_ERR_FAILED_TO_REGISTER_WORKLOOP;
   }

   pEnqReqList->uNumEnqReqs  = 0;

   return I2C_RES_SUCCESS;
}

/** @brief Initializes the request queue.
  
    @param[in]  pDevDrv  Pointer to device structure.
    
    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_InitRequestQueue
(
   I2CDRV_PDevType pDevDrv
)
{
   int32 res;

   I2CDRV_RequestQueue *pReqQueue = &pDevDrv->devReqQueue;

   qurt_signal_init(&(pReqQueue->waitForAsyncDoneEvt.hEvt));

   res = I2CDRV_InitFreeRequestList(pDevDrv);
   if (I2C_RES_SUCCESS != res) {
     return res;
   }
   
   res = I2CDRV_InitEnqueuedRequestList(pDevDrv);
   return res;
}

/** @brief Enqueues the request.
  
    @param[in]  pDevDrv  Pointer to device structure.
    @param[in]  pReq     Pointer to request structure.
    
    @return             Nothing.
  */ 
void
I2CDRV_EnqueueRequest
(
   I2CDRV_PDevType pDevDrv,
   I2CDRV_Request *pReq
)
{
   I2CDRV_EnqueuedRequestList  *pEnqReqList;

   pEnqReqList = &pDevDrv->devReqQueue.enqReqList;
   pReq->pNext = NULL;

   qurt_mutex_lock(&(pEnqReqList->syncEnqReqList.hSync));
   
   pEnqReqList->uNumEnqReqs++;
   if ( FIRST_TIME == pEnqReqList->uNumEnqReqs ) { /* first request, queue not empty. */
      pEnqReqList->pHead = pEnqReqList->pTail = pReq;
      /* signal processing thread that queue is not empty. */
	  if ( I2CDEV_Ispollingmode(pDevDrv->hDev) && (!pReq->IsAsync)) {
	  	 I2CDRV_ProcessNextTransaction(pEnqReqList->wlReqProcessing,(void*)pDevDrv,NULL);	  
	  	}
	  else {
	  	  dsr_enqueue(pEnqReqList->wlReqProcessing, NULL);
	  	}
   }
   else {
      pEnqReqList->pTail->pNext = pReq;
      pEnqReqList->pTail = pReq;
   }
   
   qurt_mutex_unlock(&(pEnqReqList->syncEnqReqList.hSync));
}

/** @brief Denqueues the request and sets the pointer to it.
  
    @param[in]  pDevDrv Pointer to device structure.
    @param[out] ppReq   Pointer to pointer to request structure.
    
    @return             Nothing.
  */
void
I2CDRV_DequeueRequest
(
   I2CDRV_PDevType pDevDrv,
   I2CDRV_Request **ppReq
)
{
   I2CDRV_EnqueuedRequestList  *pEnqReqList;
   I2CDRV_Request              *pReq;

   pEnqReqList = &pDevDrv->devReqQueue.enqReqList;

   qurt_mutex_lock(&(pEnqReqList->syncEnqReqList.hSync));
   if ( pEnqReqList->uNumEnqReqs == 0 ) {
      pReq = NULL;
   }
   else {
      pReq = pEnqReqList->pHead;
      pEnqReqList->pHead = pReq->pNext;
      pReq->pNext = NULL;
      pEnqReqList->uNumEnqReqs--;
      if ( NULL == pEnqReqList->pHead ) {
         pEnqReqList->pTail =pEnqReqList->pHead;// empty queue
      } 
   }

   qurt_mutex_unlock(&(pEnqReqList->syncEnqReqList.hSync));
   *ppReq = pReq;
}

/** @brief Processes a synchronous transaction.
  
    @param[in]  pDevDrv  Pointer to device structure.
    @param[in]  pReq     Pointer to request structure.
    
    @return             Nothing.
  */
static void I2CDRV_ProcessSynchronousTransaction
(
   I2CDRV_PDevType    pDevDrv,
   I2CDRV_Request     *pReq
)
{

   I2C_CALLTRACE_LEVEL2(pDevDrv->pDevLog, 1,
                        "I2CDRV: I2CDRV_ProcessSynchronousTransaction pDev=0x%x",
                        pDevDrv);

   if ( I2CDRV_ReqApiEnumRead == pReq->eReqApiEnum ) {
      pReq->reqVariant.retVal =
         I2CDEV_Read(pDevDrv->hDev,
                     &pReq->reqVariant.pTransfer,
                     &pReq->reqVariant.pClntCfg, 
                     &pReq->reqVariant.puNumCompleted);
   }
   else if ( I2CDRV_ReqApiEnumWrite == pReq->eReqApiEnum ) {
      pReq->reqVariant.retVal =
         I2CDEV_Write(pDevDrv->hDev,
                      &pReq->reqVariant.pTransfer,
                      &pReq->reqVariant.pClntCfg, 
                      &pReq->reqVariant.puNumCompleted);
   }
   else if ( I2CDRV_ReqApiEnumBatch == pReq->eReqApiEnum ) {
      pReq->reqVariant.retVal =
         I2CDEV_BatchTransfer(pDevDrv->hDev,
                              &pReq->reqVariant.pSequence,
                              &pReq->reqVariant.pClntCfg, 
                              &pReq->reqVariant.pIoRes);
   }
    
   I2C_CALLTRACE_LEVEL2(
            pDevDrv->pDevLog, 1,
            "I2CDRV: I2CDRV_ProcessSynchronousTransaction EVTCTRL pDev=0x%x",
            pDevDrv);
			
    if( !I2CDEV_Ispollingmode(pDevDrv->hDev) ) {
	   qurt_signal_set(&(pReq->pEvtReqComplete->hEvt),I2C_EVENT1_MASK);  
    }																										
   
   I2C_CALLTRACE_LEVEL2(
         pDevDrv->pDevLog, 1,
         "I2CDRV: I2CDRV_ProcessSynchronousTransaction EXIT pDev=0x%x",
         pDevDrv);
		 
}

/** @brief Async completion callback.
  
    @param[in]  pArg  Pointer to argument.

    @return           Nothing.
  */
void
I2CDRV_ASYNC_CB
(
   uint32 status,
   void * pArg
)
{
 /*As client callback taken care at QAPI level so no need of any logic here*/
}

/** @brief Processes an asynchronous request.
  
    @param[in]  pDevDrv  Pointer to device structure.
    @param[in]  pReq     Pointer to request structure.
    
    @return              Nothing.
  */
static void
I2CDRV_ProcessAsynchronousTransaction
(
   I2CDRV_PDevType     pDevDrv,
   I2CDRV_Request     *pReq
)
{
   int32              res = I2C_RES_SUCCESS;
   
   I2CDRV_RequestQueue *pReqQueue = &pDevDrv->devReqQueue;
   
   do {
   	
      qurt_signal_clear(&(pReqQueue->waitForAsyncDoneEvt.hEvt),I2C_EVENT1_MASK);    

      if ( I2CDRV_ReqApiEnumRead == pReq->eReqApiEnum ) {
         res = I2CDEV_AsyncRead(pDevDrv->hDev,
                                &pReq->reqVariant.pTransfer,
                                &pReq->reqVariant.pClntCfg, 
                                &pReq->reqVariant.pIoRes,
                                I2CDRV_ASYNC_CB, pReqQueue);
      }
      else if ( I2CDRV_ReqApiEnumWrite == pReq->eReqApiEnum ) {
         res = I2CDEV_AsyncWrite(pDevDrv->hDev,
                                 &pReq->reqVariant.pTransfer,
                                 &pReq->reqVariant.pClntCfg, 
                                 &pReq->reqVariant.pIoRes,
                                 I2CDRV_ASYNC_CB, pReqQueue);
      }
      else if ( I2CDRV_ReqApiEnumBatch == pReq->eReqApiEnum ) {
         res = I2CDEV_AsyncBatchTransfer(pDevDrv->hDev,
                                         &pReq->reqVariant.pSequence,
                                         &pReq->reqVariant.pClntCfg, 
                                         &pReq->reqVariant.pIoRes,
                                         I2CDRV_ASYNC_CB, pReqQueue);
      }
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      
   } while( 0 );
   
   if ( (I2CDRV_ReqApiEnumRead == pReq->eReqApiEnum) || 
        (I2CDRV_ReqApiEnumWrite == pReq->eReqApiEnum) ) {
       if ( I2C_RES_SUCCESS != res ) {
          pReq->reqVariant.pIoRes.nOperationResult = res;
       }
      pReq->reqVariant.pCallbackFn(pReq->reqVariant.pIoRes.nOperationResult,pReq->reqVariant.pArg);
   }
   else {
      if ( I2C_RES_SUCCESS != res ) {
          pReq->reqVariant.pIoRes.nOperationResult = res;
      }
      pReq->reqVariant.pCallbackFn(pReq->reqVariant.pIoRes.nOperationResult,pReq->reqVariant.pArg);
   }
   
   I2CDRV_ReleaseFreeRequest(pDevDrv, pReq);
}

/** @brief Dequeues and processes the transfer.
    
    This function is called from a workloop.
    
    @param[in]  hEvent     Event handle.
    @param[in]  pInDevCtxt Pointer to context.
    
    @return              DALResult.
  */
static void
I2CDRV_ProcessNextTransaction( dsr_obj_t *dsr_obj, void *pInDevCtxt, void *payload)
{
   I2CDRV_Request     *pReq;
   I2CDRV_PDevType     pDevDrv = (I2CDRV_PDevType)pInDevCtxt;
   
   I2C_CALLTRACE_LEVEL2(pDevDrv->pDevLog, 1,
                        "I2CDRV_ProcessNextTransaction pDev=0x%x",
                        pDevDrv);

   I2CDRV_DequeueRequest(pDevDrv, &pReq);
   while ( NULL != pReq ) {
      if ( I2CDRV_ReqSyncEnumSynchronous == pReq->eReqSyncEnum ) {
         I2CDRV_ProcessSynchronousTransaction(pDevDrv, pReq);
      }
      else {
         I2CDRV_ProcessAsynchronousTransaction(pDevDrv, pReq);
      }
      I2CDRV_DequeueRequest(pDevDrv, &pReq);
   } 
   
}



/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Adds a device and sets the handle.
  
    @param[in]  uDevId  Pointer to device structure.
    @param[in]  phDev   Pointer to handle.
    
    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_AddDevice
(
   uint32          uDevId,
   I2CDRV_HANDLE   *phDev
)
{
 
   int32 res = I2C_RES_SUCCESS;
   I2CDRV_PDevType pDevDrv;

   enum I2CDRV_AddDevState
   {
      I2CDRV_AddDevState_Malloc,
      I2CDRV_AddDevState_Inited,
   } eAddDevState;


   pDevDrv = I2CDRV_SearchDevice(uDevId);
   if ( pDevDrv ) {
      *phDev = (I2CDRV_HANDLE *)pDevDrv;
      return I2C_RES_SUCCESS;
   }
   
   res = (int32)I2CSYS_Malloc((void **)&pDevDrv, sizeof(I2CDRV_DevType));
   if ( (I2C_SUCCESS != res) || (NULL == pDevDrv) ) {
      return I2CDRV_ERR_FAILED_TO_ALLOCATE_DEVICE;
   }
   
   res = I2CSYS_Memset(pDevDrv, 0, sizeof(I2CDRV_DevType));  
   pDevDrv->uDevId = uDevId;
   eAddDevState = I2CDRV_AddDevState_Malloc;

   do{
 
     qurt_mutex_init(&(pDevDrv->syncDrvDev.hSync));
	  
      res = I2CDEV_Init((void *)pDevDrv->uDevId, &pDevDrv->hDev);
      if ( I2C_RES_SUCCESS != res ) {
        return res;
      }
      eAddDevState = I2CDRV_AddDevState_Inited;

      res = I2CDRV_InitRequestQueue(pDevDrv);
      if ( I2C_RES_SUCCESS != res ) {
         return res;
      }
      /* Init device logging. */
      I2cLog_Init((uint32)pDevDrv->uDevId, &pDevDrv->pDevLog);
      
      I2CDRV_LinkDevice(pDevDrv);
      *phDev = pDevDrv;
   } while ( 0 );

   if ( I2C_RES_SUCCESS != res ) {
      switch ( eAddDevState ) {
         case I2CDRV_AddDevState_Inited:
            I2CDEV_DeInit(pDevDrv->hDev);
         case I2CDRV_AddDevState_Malloc:
            I2CSYS_Free(pDevDrv);     
      }
   }
      
   return res;
}


/** @brief Initializes the driver.

    This Function Initializes the driver and creates the
    necessary data structures to support other calls into the
    driver.


    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32 I2CDRV_Init(void)
{
   return I2C_RES_SUCCESS;
}

/** @brief Deinitializes the device.

    This Function Deinitializes the device and releases
    resources acquired during init.


    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_DeInit(void)
{
   return I2C_RES_SUCCESS;
}

/**
    This function opens the device handle.

    @param[in]  hDev         Device handle.
    @param[in]  dwaccessMode Access mode.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_Open
(
   I2CDRV_HANDLE hDev,
   uint32        dwaccessMode
)
{
   int32 res = I2C_RES_SUCCESS;
   I2CDRV_PDevType    pDevDrv = (I2CDRV_PDevType)hDev;

   qurt_mutex_lock(&(pDevDrv->syncDrvDev.hSync));
   
   if( 0 == pDevDrv->uOpenedClientCount ) {
      res = I2CDEV_SetPowerState(pDevDrv->hDev, I2CDEV_POWER_STATE_2);
   }
   if( I2C_RES_SUCCESS == res ) {
      pDevDrv->uOpenedClientCount++;
   }
 
   qurt_mutex_unlock(&(pDevDrv->syncDrvDev.hSync));
 
   return res;
}/* I2CDRV_Open */

/**
    This function closes the device handle.

    @param[in]  hDev    Device handle.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_Close
(
   I2CDRV_HANDLE hDev
)
{
   int32 res = I2C_RES_SUCCESS;
   I2CDRV_PDevType    pDevDrv = (I2CDRV_PDevType)hDev;

   qurt_mutex_lock(&(pDevDrv->syncDrvDev.hSync));
   
   if( 1 == pDevDrv->uOpenedClientCount ) {
      res = I2CDEV_SetPowerState(pDevDrv->hDev, I2CDEV_POWER_STATE_0);
   }
   if( I2C_RES_SUCCESS == res ) {
      pDevDrv->uOpenedClientCount--;
   }
 
   qurt_mutex_unlock(&(pDevDrv->syncDrvDev.hSync));
   
   return res;
}/* I2CDRV_Close */

/** @brief Read a buffer from i2c device.

    Read a buffer from i2c device.

    @param[in] hDev           Device handle.
    @param[in] pTransfer      Pointer to transfer.
    @param[in] pClntCfg       Pointer to Client configuration.
    @param[in] puNumCompleted Pointer to return completed reads.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_Read
(
   I2CDRV_HANDLE                         hDev     ,
   I2cTransfer                          *pTransfer,
   I2cClientConfig                      *pClntCfg , /* if null keep previous. */
   uint32                               *puNumCompleted
)
{

   I2CDRV_Request     req;
   int32                  res;
   
   
   I2CDRV_PDevType    pDevDrv = (I2CDRV_PDevType)hDev;
   I2CDRV_RequestQueue *pReqQueue = &pDevDrv->devReqQueue;
   I2CDRV_EnqueuedRequestList *pEnqReqList = &pReqQueue->enqReqList;
   
   res = I2CDRV_VALIDATE_SYNC_TRAN(hDev, pTransfer, pClntCfg, puNumCompleted);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   }
   
   qurt_mutex_lock(&(pDevDrv->syncDrvDev.hSync));
   
   req.eReqApiEnum               = I2CDRV_ReqApiEnumRead;
   req.eReqSyncEnum              = I2CDRV_ReqSyncEnumSynchronous;

   I2CSYS_MemsCpy(&(req.reqVariant.pTransfer),sizeof( I2cTransfer),pTransfer,sizeof(struct I2cTransfer));
   I2CSYS_MemsCpy(&(req.reqVariant.pClntCfg),sizeof(struct I2cClientConfig),pClntCfg,sizeof(struct I2cClientConfig));
   
   req.reqVariant.puNumCompleted = (uint32)puNumCompleted;

   req.IsAsync                   = FALSE;
   req.pEvtReqComplete = &(pEnqReqList->evtEnqReqListNotEmpty);
   
   if(!I2CDEV_Ispollingmode(pDevDrv->hDev) ) { 
	  qurt_signal_clear(&(pEnqReqList->evtEnqReqListNotEmpty.hEvt),
						  I2C_EVENT1_MASK);
   	  I2CDRV_EnqueueRequest(pDevDrv, &req);
   	  qurt_signal_wait(&(pEnqReqList->evtEnqReqListNotEmpty.hEvt),
						I2C_EVENT1_MASK,
						QURT_SIGNAL_ATTR_WAIT_ALL);
	   }
	  else {
	   I2CDRV_EnqueueRequest(pDevDrv, &req);
	   } 
   
   qurt_mutex_unlock(&(pDevDrv->syncDrvDev.hSync));
   return req.reqVariant.retVal;
} 

/** @brief Write a buffer to i2c device.

    Write a buffer to i2c device.

    @param[in] hDev            Device handle.
    @param[in] pTransfer       Pointer to transfer data.
    @param[in] pClntCfg        Pointer to Client configuration.
    @param[in] puNumCompleted  Pointer to return completed
          bytes.

    @return          I2C_RES_SUCCESS if successful, error
                     otherwise.
  */
int32
I2CDRV_Write
(
   I2CDRV_HANDLE                         hDev     ,
   I2cTransfer                          *pTransfer,
   I2cClientConfig                      *pClntCfg , /* if null keep previous. */
   uint32                               *puNumCompleted
)
{
   I2CDRV_Request     req;
   int32              res;

      
   I2CDRV_PDevType    pDevDrv = (I2CDRV_PDevType)hDev;
   I2CDRV_RequestQueue *pReqQueue = &pDevDrv->devReqQueue;
   I2CDRV_EnqueuedRequestList *pEnqReqList = &pReqQueue->enqReqList;
   

   res = I2CDRV_VALIDATE_SYNC_TRAN(hDev, pTransfer, pClntCfg, puNumCompleted);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   }

   qurt_mutex_lock(&(pDevDrv->syncDrvDev.hSync));
   
   I2C_CALLTRACE_LEVEL2(pDevDrv->pDevLog, 1,
                        "I2CDRV: I2CDRV_BatchTransfer ENTRY pDev=0x%x",
                        pDevDrv);
   req.eReqApiEnum               = I2CDRV_ReqApiEnumWrite;
   req.eReqSyncEnum              = I2CDRV_ReqSyncEnumSynchronous;
   
   I2CSYS_MemsCpy(&(req.reqVariant.pTransfer),sizeof(struct I2cTransfer),pTransfer,sizeof (struct I2cTransfer));
   I2CSYS_MemsCpy(&(req.reqVariant.pClntCfg),sizeof(struct I2cClientConfig),pClntCfg,sizeof (struct I2cClientConfig));

   req.reqVariant.puNumCompleted = (uint32)puNumCompleted;
   req.IsAsync                   = FALSE;
   req.pEvtReqComplete = &(pEnqReqList->evtEnqReqListNotEmpty);

   if(!I2CDEV_Ispollingmode(pDevDrv->hDev) ) {
  	  qurt_signal_clear(&(pEnqReqList->evtEnqReqListNotEmpty.hEvt),
						 I2C_EVENT1_MASK);
   	  I2CDRV_EnqueueRequest(pDevDrv, &req);
   	  qurt_signal_wait(&(pEnqReqList->evtEnqReqListNotEmpty.hEvt),
						I2C_EVENT1_MASK,
						QURT_SIGNAL_ATTR_WAIT_ALL);
   	}
   else {
   	
   	  I2CDRV_EnqueueRequest(pDevDrv, &req);
   	}

   I2C_CALLTRACE_LEVEL2(pDevDrv->pDevLog, 1,
                        "I2CDRV: I2CDRV_BatchTransfer()=0x%x",
                        pDevDrv);
						
   qurt_mutex_unlock(&(pDevDrv->syncDrvDev.hSync));
   return req.reqVariant.retVal;
}

/** @brief Does a batch of transfers in a sequence.

    Does a batch of transfers in a sequence.

    @param[in] hDev           Device handle.
    @param[in] pSequence      Pointer to a sequence of transfer
          data.
    @param[in] pClntCfg       Pointer to Client configuration.
    @param[in] pIoRes         Pointer to io result.

    @return          I2C_RES_SUCCESS if successful, error
                     otherwise.
  */
int32
I2CDRV_BatchTransfer
(
   I2CDRV_HANDLE                         hDev     ,
   I2cSequence                          *pSequence,
   I2cClientConfig                      *pClntCfg , /* if null keep previous. */
   I2cIoResult                          *pIoRes
)
{
   DALResult          dalRes;
   I2CDRV_Request     req;
   int32              res;

   I2CDRV_PDevType      pDevDrv = (I2CDRV_PDevType)hDev;
   I2CDRV_RequestQueue *pReqQueue = &pDevDrv->devReqQueue;
   I2CDRV_EnqueuedRequestList *pEnqReqList = &pReqQueue->enqReqList;

   res = I2CDRV_VALIDATE_SYNC_SEQ(hDev,pSequence,pClntCfg,pIoRes);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   	}
   
   qurt_mutex_lock(&(pDevDrv->syncDrvDev.hSync));
   
   I2C_CALLTRACE_LEVEL2(pDevDrv->pDevLog, 1,
                        "I2CDRV: I2CDRV_BatchTransfer ENTRY pDev=0x%x",
                        pDevDrv);  
 
   req.eReqApiEnum               = I2CDRV_ReqApiEnumBatch;
   req.eReqSyncEnum              = I2CDRV_ReqSyncEnumSynchronous;

   I2CSYS_MemsCpy(&(req.reqVariant.pSequence),sizeof(struct I2cSequence),pSequence,sizeof (struct I2cSequence));
   I2CSYS_MemsCpy(&(req.reqVariant.pClntCfg),sizeof(struct I2cClientConfig),pClntCfg,sizeof (struct I2cClientConfig));
   I2CSYS_MemsCpy(&(req.reqVariant.pIoRes),sizeof(struct I2cIoResult),pIoRes,sizeof (struct I2cIoResult));

   req.IsAsync                   = FALSE;
   req.pEvtReqComplete = &(pEnqReqList->evtEnqReqListNotEmpty);;

     if(!I2CDEV_Ispollingmode(pDevDrv->hDev) ) { 
   	  qurt_signal_clear(&(pEnqReqList->evtEnqReqListNotEmpty.hEvt),
						  I2C_EVENT1_MASK);
   	  I2CDRV_EnqueueRequest(pDevDrv, &req);
   	  qurt_signal_wait(&(pEnqReqList->evtEnqReqListNotEmpty.hEvt),
						 I2C_EVENT1_MASK,
						 QURT_SIGNAL_ATTR_WAIT_ALL);
    	}
   else {  	
   	  I2CDRV_EnqueueRequest(pDevDrv, &req);
   	} 

   I2C_CALLTRACE_LEVEL2(pDevDrv->pDevLog, 1,
                        "I2CDRV: I2CDRV_BatchTransfer EXIT pDev=0x%x",
                        pDevDrv);
						
	
   qurt_mutex_unlock(&(pDevDrv->syncDrvDev.hSync));
   return req.reqVariant.retVal;
}



/** @brief Schedules a buffer read from i2c device.

    Schedules a buffer read from i2c device.
    Once the read is complete or an error occurs
    the callback will be called.

    @param[in] hDev           Device handle.
    @param[in] pTransfer      Pointer to transfer.
    @param[in] pClntCfg       Pointer to Client configuration.
    @param[in] pIoRes         Pointer to returned result of the
                               transfer.
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return          I2C_RES_SUCCESS if queuing successful,
                     error otherwise.
  */
int32
I2CDRV_AsyncRead
(
   I2CDRV_HANDLE                         hDev               ,
   I2cTransfer                          *pTransfer          ,
   I2cClientConfig                      *pClntCfg, /* if null keep previous. */
   I2cIoResult                          *pIoRes             ,
   DRV_ASYNC_CB                          pCallbackFn        ,
   void                                 *pArg            
)
{
   I2CDRV_Request     *pReq;
   int32               res;
   I2CDRV_PDevType     pDevDrv = (I2CDRV_PDevType)hDev;

   res = I2CDRV_VALIDATE_ASYNC_TRAN(hDev, pTransfer, pClntCfg, pIoRes, pCallbackFn, pArg);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   }
   
   res = I2CDRV_GetFreeRequest(pDevDrv, &pReq);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   }

   pReq->eReqApiEnum            = I2CDRV_ReqApiEnumRead;
   pReq->eReqSyncEnum           = I2CDRV_ReqSyncEnumAsynchronous;

   I2CSYS_MemsCpy(&(pReq->reqVariant.pTransfer),sizeof(struct I2cTransfer),pTransfer,sizeof (struct I2cTransfer));
   I2CSYS_MemsCpy(&(pReq->reqVariant.pClntCfg),sizeof(struct I2cClientConfig),pClntCfg,sizeof (struct I2cClientConfig));
   I2CSYS_MemsCpy(&(pReq->reqVariant.pIoRes),sizeof(struct I2cIoResult),pIoRes,sizeof (struct I2cIoResult));

   pReq->reqVariant.pCallbackFn = pCallbackFn;
   pReq->reqVariant.pArg        = pArg;
   pReq->IsAsync                = TRUE;

   I2CDRV_EnqueueRequest(pDevDrv, pReq);
   
   return I2C_RES_SUCCESS;
} 

/** @brief Schedules a buffer write to i2c device.

    Schedules a buffer write to i2c device. Once the write is
    complete or an error occurs the callback will be called.

    @param[in] hDev           Device handle.
    @param[in] pTransfer      Pointer to transfer.
    @param[in] pClntCfg       Pointer to Client configuration.
    @param[in] pIoRes       Pointer to returned result of the
                               transfer.
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return          I2C_RES_SUCCESS if successful, error
                     otherwise.
  */
int32
I2CDRV_AsyncWrite
(
   I2CDRV_HANDLE                         hDev               ,
   I2cTransfer                          *pTransfer          ,
   I2cClientConfig                      *pClntCfg,/* if null keep previous. */
   I2cIoResult                          *pIoRes           ,
   DRV_ASYNC_CB                          pCallbackFn        ,
   void                                 *pArg            
)
{
   I2CDRV_Request     *pReq;
   int32               res;
   I2CDRV_PDevType    pDevDrv = (I2CDRV_PDevType)hDev;

   res = I2CDRV_VALIDATE_ASYNC_TRAN(hDev,pTransfer,pClntCfg,pIoRes,pCallbackFn,pArg);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   }

   
   res = I2CDRV_GetFreeRequest(pDevDrv, &pReq);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   }

   pReq->eReqApiEnum                    = I2CDRV_ReqApiEnumWrite;
   pReq->eReqSyncEnum                   = I2CDRV_ReqSyncEnumAsynchronous;
   
   I2CSYS_MemsCpy(&(pReq->reqVariant.pTransfer),sizeof(struct I2cTransfer),pTransfer,sizeof (struct I2cTransfer));
   I2CSYS_MemsCpy(&(pReq->reqVariant.pClntCfg),sizeof(struct I2cClientConfig),pClntCfg,sizeof (struct I2cClientConfig));
   I2CSYS_MemsCpy(&(pReq->reqVariant.pIoRes),sizeof(struct I2cIoResult),pIoRes,sizeof (struct I2cIoResult));

   pReq->reqVariant.pCallbackFn = pCallbackFn;
   pReq->reqVariant.pArg        = pArg;
   pReq->IsAsync                = TRUE;

   I2CDRV_EnqueueRequest(pDevDrv, pReq); 
   
   return I2C_RES_SUCCESS;
}

/** @brief Schedules a batch of transfers in a sequence.

    Schedulest a batch of transfers in a sequence and returns.
    The callback will be called to notify transfer is done or
    has failed.

    @param[in] hDev           Device handle.
    @param[in] pSequence      Pointer to the sequence of
          transfers.
    @param[in] pClntCfg       Pointer to Client configuration.
    @param[in] pIoRes         Pointer to returned result of the
                               transfer.
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return          I2C_RES_SUCCESS if queuing successful,
                     error otherwise.
  */
int32
I2CDRV_AsyncBatchTransfer
(
   I2CDRV_HANDLE                         hDev               ,
   I2cSequence                          *pSequence          ,
   I2cClientConfig                      *pClntCfg,// if null keep previous
   I2cIoResult                          *pIoRes           ,
   DRV_ASYNC_CB                          pCallbackFn        ,
   void                                 *pArg            
)
{
   I2CDRV_Request     *pReq;
   int32               res;
   I2CDRV_PDevType     pDevDrv = (I2CDRV_PDevType)hDev;

   res = I2CDRV_VALIDATE_ASYNC_SEQ(hDev,pSequence,pClntCfg,pIoRes,pCallbackFn,pArg);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   }
   
   res = I2CDRV_GetFreeRequest(pDevDrv, &pReq);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   }

   pReq->eReqApiEnum            = I2CDRV_ReqApiEnumBatch;
   pReq->eReqSyncEnum           = I2CDRV_ReqSyncEnumAsynchronous;

   I2CSYS_MemsCpy(&(pReq->reqVariant.pSequence),sizeof(struct I2cSequence),pSequence,sizeof (struct I2cTransfer));
   I2CSYS_MemsCpy(&(pReq->reqVariant.pClntCfg),sizeof(struct I2cClientConfig),pClntCfg,sizeof (struct I2cClientConfig));
   I2CSYS_MemsCpy(&(pReq->reqVariant.pIoRes),sizeof(struct I2cIoResult),pIoRes,sizeof (struct I2cIoResult));
   	
   pReq->reqVariant.pCallbackFn = pCallbackFn;
   pReq->reqVariant.pArg        = pArg;
   pReq->IsAsync                = TRUE;
   
   I2CDRV_EnqueueRequest(pDevDrv, pReq);
   
   return I2C_RES_SUCCESS;
}





