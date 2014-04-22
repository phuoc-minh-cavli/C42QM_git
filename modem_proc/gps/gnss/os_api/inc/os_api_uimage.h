
#ifndef OS_API_UIMAGE_H
#define OS_API_UIMAGE_H

/* 
  Copyright (c) 2018 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*
 * Include files
*/

#include "gps_variation.h"
#include <stdio.h>
#include <string.h>
#include <stringl/stringl.h>

#include "comdef.h"
#include "aries_os_globals.h"
#include "aries_os_api.h"

#if defined(FEATURE_BLAST)
#include "blast.h"
#endif /* defined(FEATURE_BLAST) */

#if defined(FEATURE_QURT)
#include "qurt.h"
#endif /* defined (FEATURE_QURT) */
#include "queue.h"
#include "dog.h"


#ifdef FEATURE_CGPS_DOG_HEARTBEAT
#include "dog_hb_qurt.h"
#endif

#define  OS_QUEUE_TYPE q_type
#define  OS_QUEUE_ELEMENT q_link_type
#ifdef FEATURE_GNSS_UIMAGE_SUPPORT
#define  OS_UMUTEX_TYPE qurt_mutex_t
#define  OS_QURT_SIGNAL_TYPE qurt_anysignal_t
#define  OS_QURT_THREAD_TYPE qurt_thread_t
#endif
/* Bitmask to check the uImage Data */
#define  UIMAGE_BITMASK_MSG_TYPE_IPC                           ((uint32)1<<0)
#define  UIMAGE_BITMASK_MSG_TYPE_UIPC                          ((uint32)1<<1)
#define C_OS_QURT_FLAG_DOG_HB                                  (((uint32)1<<2))
#define UIMAGE_IPC_BUFFER_SIZE                                 150



typedef void (*funcPtrType)(unsigned long dummy);


typedef struct
{
  os_IpcMsgType *p_buffer[UIMAGE_IPC_BUFFER_SIZE];
  volatile uint16 w_head;
  volatile uint16 w_tail;
#ifdef FEATURE_GNSS_UIMAGE_SUPPORT
  OS_UMUTEX_TYPE z_uIpcQMutex;
#endif
}os_IpcRingBuffer;

typedef struct
{
  char s_threadName[8];
  uint8 u_GnssThreadId;
  uint8 u_GnssQurtThreadId;
  funcPtrType p_FuncPtr;
  void *p_arg;
} os_QThreadAttr;

#ifdef FEATURE_GNSS_UIMAGE_SUPPORT

typedef struct
{
  qurt_thread_attr_t z_QThreadAttr;
  uint8 q_GnssThreadId;
  qurt_thread_t qurt_ThreadId;
  OS_QURT_SIGNAL_TYPE z_uIpcSignal;
#ifdef FEATURE_CGPS_DOG_HEARTBEAT
  dog_report_type dogHandle;
#endif
}os_QurtThreadProps;

extern os_QurtThreadProps    osp_QurtThreadDb[MAX_QURT_THREAD_COUNT+1];

#endif

extern os_IpcRingBuffer osp_IpcBuff;



/*
 ******************************************************************************
 * os_PipeReceive()

 * This API is called to block on Pipe Recive.
 *
 * Parameters: Pointer to Pipe where the Qurt task is blocked, pointer to Pipe
 * data to receive
 * 
 * Returns:
 * Pointer to os_PipeDataType 
 ******************************************************************************
*/
os_IpcMsgType* os_uIpcReceive(void);

/*
 ******************************************************************************
 * os_PipeSend()

 * This API is called to send Pipe Data to a Qurt Task which is blocked on
 * receiving data on that specific pipe.
 *
 * Parameters: Destination Thread ID, Pointer to Ipc Message pointer (4 bytes)
 * , one additional uint32 paramter for the remaining 32 bit of the pipe data.
 * Returns:
 * None
 ******************************************************************************
*/

boolean os_uIpcSend(os_IpcMsgType *p_Msg, uint32 u_DestThId);


/*
 ******************************************************************************
 * os_SetGnssUimageReady()

 * This API is called to Set Uimage 
 *
 * Parameters: b_flag
 * 
 * Returns:
 * None
 ******************************************************************************
*/

void os_SetGnssUimageReady(boolean b_flag);

/*
 ******************************************************************************
 * os_IsGnssUimageReady()

 * This API is called to query if system/GPS is currently in uImage
 *
 * Parameters: None
 * 
 * Returns:
 * TRUE/FALSE
 ******************************************************************************
*/

boolean os_IsGnssUimageReady(void);

/*
 ******************************************************************************
 * os_PipeDelete()

 * This API is called to Delete an IPC sent via pipe
 *
 * Parameters: Pipe Data Pointer
 * 
 * Returns:
 * TRUE/FALSE
 ******************************************************************************
*/

boolean os_uIpcDelete(os_IpcMsgType *p_Msg);

/*
 ******************************************************************************
 * os_UpdatePipeAttr()
 * Stores the Pipe Pointer to the Qurt Threads Database

 * Paramters: Qurt Thread ID and Pipe Pointer to Store
 * Returns:
 *   None
 ******************************************************************************
 */

#ifdef FEATURE_GNSS_UIMAGE_SUPPORT
void os_UpdateQurtThreadAttr(uint8 u_GnssThreadId, uint8 u_GnssQurtThreadId, qurt_thread_attr_t qurt_tattr, qurt_thread_t ThreadId);


/*
 ******************************************************************************
 * os_GetQurtThreadId()

 * This API can be used to get a QURT task ID from a GNSS Osp_ThreadBlock Thread
 * ID.
 * Paramters: Osp_ThreadBlock Thread ID
 * Returns:
 *   uint8 QurtThread ID 
 ******************************************************************************
 */

uint8 os_GetQurtThreadId(uint8 u_GnssThreadId);


#endif

/*
 ******************************************************************************
 * os_SetIpcBuffElelment()

 * Insert an IPC pointer to the head of ring buffer .
 * Paramters: os_IpcMsgType *p_Msg
 * Returns:
 *   None 
 ******************************************************************************
 */
boolean os_SetIpcBuffElelment(os_IpcMsgType *p_Msg);

/*
 ******************************************************************************
 * os_GetIpcBuffElelment()

 * Gets an IPC pointer from tail of ring buffer .
 * Paramters: None 
 * Returns: os_IpcMsgType *
 *
 ******************************************************************************
*/
os_IpcMsgType* os_GetIpcBuffElelment(void);

/*
 ******************************************************************************
 * os_IsuIpc()

 * Checks if an IPC is uImage IPC or Not .
 * Paramters: os_IpcMsgType * p_MsgPtr 
 * Returns: TRUE or FALSE
 *   
 ******************************************************************************
 */
boolean os_IsuIpc(os_IpcMsgType *p_MsgPtr);

/*
 ******************************************************************************
 * os_uIpcCreate()

 * Description: Creates an IPC or uIPC Message
 * Paramters: uint32 q_MsgSize, os_IpcAllocEnumType e_MemType,
 * uint32 q_DestThreadId, boolean b_IsuIpc
 * Returns: os_IpcMsgType * p_Msg
 *   
 ******************************************************************************
 */
os_IpcMsgType* os_uIpcCreate(uint32 q_MsgSize, os_IpcAllocEnumType e_MemType, uint32 q_DestThreadId, boolean b_IsuIpc);

/*
 ******************************************************************************
 * os_GetAvailuIpcMsgCount()
 *  This function returns the total number of uIPC available in the uIPC 
 *  message buffer.
 *  
 * Parameters: None
 * Returns: Total number of uIPC messages available in teh uIPC buffer.
 ******************************************************************************
 */
uint16 os_GetAvailuIpcMsgCount();

#ifdef ARIES_USE_REX_KERNEL
/*
 ******************************************************************************
 * os_ThreadIdGet()
 *
 * This is a helper function that maps a Rex Task Control Block (TCB) pointer
 * address to a q_ThreadId (os_api context).
 *
 ******************************************************************************
 */
uint32 os_ThreadIdGet(const rex_tcb_type *p_RexTcb);
#endif

/*
 ******************************************************************************
 * os_QurtThreadCreate()

 * This API is called to create a QURT Thread
 * Paramters: Threadname, OsP_ThreadBlock Thread ID, Gnss Block Qurt Thread ID
 * (analogous to osp_Threadblock, but unlike REX task this ID is for any barebone
 * Qurt task), stacksize (16 bit), ThreadPiority (8 bit), Task Entry function and
 * any parameter to Task Entry function.
 * Returns:
 * None
 ******************************************************************************
*/

void os_QurtThreadCreate(os_QThreadAttr *osp_QThreadAttr);

/*
 *********************************************************************************
 * Function description:
 *   os_uIpcStart() initializes the Msg Queue of the QURT Thread Block.
 *   This should be called at creation for a QURT task using its own message queue
 *
 * Parameters:
 *   q_ThreadId - Thread Id for task that needs to initialize IPC
 *
 * Return value:
 *   TRUE if q_ThreadId is a valid Thread Id value
 *   FALSE otherwise
 *
 *********************************************************************************
*/
boolean os_uIpcStart(uint32 q_ThreadId);

#ifdef FEATURE_GNSS_UIMAGE_SUPPORT
/*
 ******************************************************************************
 * os_GetQurtThreadIdSelf()

 * This API can be used to get a QURT task ID from a GNSS Osp_QurtThreadDb Thread
 * ID when called by task in context.
 * Paramters: qurt_thread_id_t Thread ID
 * Returns:
 *   uint8 QurtThread ID 
 ******************************************************************************
 */
uint8 os_GetQurtThreadIdSelf(OS_QURT_THREAD_TYPE u_QThId);

/*
 ******************************************************************************
 * os_GetAvailuIpcMsgCountExtended()
 *  This function returns the total number of uIPC available in the uIPC 
 *  message buffer.
 *  
 * Paramters: None
 * Returns: Total number of uIPC available 
 ******************************************************************************
 */
uint16 os_GetAvailuIpcMsgCountExtended(uint16 *p_uIpcBuffHead, uint16 *p_uIpcBuffTail);


#endif
#endif /*OS_API_UIMAGE*/
