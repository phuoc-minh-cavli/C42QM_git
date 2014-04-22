#ifndef ARIES_IPC_DIAG_LOG_H
#define ARIES_IPC_DIAG_LOG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   IPC Diagnostic Message Header File

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for 
  the IPC-based diagnostic message routines used in the OS abstraction layer. 

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/os_api/inc/aries_ipc_diag_log.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

=============================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/28/07    ah     Fixed Lint errors.
04/03/06    jd     Initial creation of file for AMSS.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "gps_variation.h"
#include "customer.h"

#include "comdef.h"

#include "log.h"

#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "ULog.h"
#include "ULogFront.h"

#ifdef __GRIFFON__
#include "upos.h"
#endif // __GRIFFON__

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Constant definitions
*/
#define C_CGPS_IPC_VERSION_NUMBER  1

#ifndef WIN32
  #ifdef __ARMCC_VERSION
    #if ( __ARMCC_VERSION >= 120000 ) && ( __ARMCC_VERSION <= 129999 )
      #define C_CGPS_IPC_PACKING_FLAG   0  /* ADS 1.20 = 4-byte packing */
    #elif ( __ARMCC_VERSION >= 210000 ) && ( __ARMCC_VERSION <= 219999 )
      #define C_CGPS_IPC_PACKING_FLAG   1  /* RVCT 2.1 = 8-byte packing */
    #elif ( __ARMCC_VERSION >= 220000 ) && ( __ARMCC_VERSION <= 419999 )
      #define C_CGPS_IPC_PACKING_FLAG   1  /* RVCT 4.0 = 8-byte packing */
    #elif ( __ARMCC_VERSION >= 420000 ) && ( __ARMCC_VERSION <= 429999 )
      #define C_CGPS_IPC_PACKING_FLAG   1  /* RVCT 4.1 = 8-byte packing */
    #else
      #error NOTE: If you hit this build problem, please add the name of your (new) ARM compiler version to the list above add add a MACRO to define the alignment restriction for that compiler.
    #endif  /* __ARMCC_VERSION */

  #elif defined(__GNUC__) && defined(__QDSP6_ARCH__) 
    #define C_CGPS_IPC_PACKING_FLAG   1  /* qdsp6-gcc = 8-byte packing */
  #elif defined(__HEXAGON_ARCH__)
    #define C_CGPS_IPC_PACKING_FLAG   1  /* qdsp6-gcc = 8-byte packing */
  #elif _lint
    #define C_CGPS_IPC_PACKING_FLAG   1  /* Assume 8-byte packing during Lint */
  #elif defined(__GRIFFON__)
    #define C_CGPS_IPC_PACKING_FLAG   1  /* Assume 8-byte packing during Lint */
  #else /* No compiler defined and not running Lint */
    #error NOTE: If you hit this build problem, please add the name of your [new] compiler to one of the two lists above (either ADS..., or RVCT...) depending on whether the default data packing on your compiler is 4 or 8 bytes.
  #endif
#else /* defined(WIN32) */
  #define C_CGPS_IPC_PACKING_FLAG   1     /* WIN32 = 8-byte packing */
#endif /* WIN32 */

#ifdef FEATURE_GNSS_UIMAGE_SUPPORT
#define GNSS_LOG_STATUS(a)                   micro_log_status(a)
#define GNSS_LOG_SET_LENGTH(a,b)             micro_log_set_length(a,b)
#define GNSS_LOG_SET_CODE(a,b)               micro_log_set_code(a,b)
#define GNSS_LOG_SUBMIT(a,b,c)               micro_log_submit_ext(a,b,c,0,NULL)
#else
#define GNSS_LOG_STATUS(a)                   log_status(a)
#define GNSS_LOG_SET_LENGTH(a,b)             log_set_length(a,b)
#define GNSS_LOG_SET_CODE(a,b)               log_set_code(a,b)
#define GNSS_LOG_SUBMIT(a,b,c)               log_submit_ext(c,0,NULL)
#endif

/* =========================================================================
**  Macro defines for GNSS ULOG 
** =========================================================================*/
#ifndef FEATURE_GNSS_LEAN_BUILD
#define OS_ULOG_HANDLE  ULogHandle
#define OS_ULOG_RESULT  ULogResult
#define OS_ULOG_LOCK_TYPE ULOG_LOCK_TYPE
#define OS_ULOG_PRINT_0(handle, formatStr) ULOG_RT_PRINTF_0(handle, formatStr);
#define OS_ULOG_PRINT_1(handle, formatStr, a) ULOG_RT_PRINTF_1(handle, formatStr, a);
#define OS_ULOG_PRINT_2(handle, formatStr, a, b) ULOG_RT_PRINTF_2(handle, formatStr, a, b);
#define OS_ULOG_PRINT_3(handle, formatStr, a, b, c) ULOG_RT_PRINTF_3(handle, formatStr, a, b, c);
#define OS_ULOG_PRINT_4(handle, formatStr, a, b, c, d) ULOG_RT_PRINTF_4(handle, formatStr, a, b, c, d);
#define OS_ULOG_PRINT_5(handle, formatStr, a, b, c, d, e) ULOG_RT_PRINTF_5(handle, formatStr, a, b, c, d, e);
#define OS_ULOG_PRINT_6(handle, formatStr, a, b, c, d, e, f) ULOG_RT_PRINTF_6(handle, formatStr, a, b, c, d, e, f);
#define OS_ULOG_PRINT_7(handle, formatStr, a, b, c, d, e, f, g) ULOG_RT_PRINTF_7(handle, formatStr, a, b, c, d, e, f, g);
#define OS_ULOG_PRINT_8(handle, formatStr, a, b, c, d, e, f, g, h) ULOG_RT_PRINTF_8(handle, formatStr, a, b, c, d, e, f, g, h);
#define OS_ULOG_PRINT_9(handle, formatStr, a, b, c, d, e, f, g, h, i) ULOG_RT_PRINTF_9(handle, formatStr, a, b, c, d, e, f, g, h, i);
#define OS_ULOG_PRINT_10(handle, formatStr, a, b, c, d, e, f, g, h, i, j) ULOG_RT_PRINTF_10(handle, formatStr, a, b, c, d, e, f, g, h, i, j);
#else
#define NOOP (void)0 
#define OS_ULOG_HANDLE  void *
#define OS_ULOG_RESULT  void *
#define OS_ULOG_LOCK_TYPE ULOG_LOCK_TYPE void *
#define OS_ULOG_PRINT_0(handle, formatStr) NOOP
#define OS_ULOG_PRINT_1(handle, formatStr, a) NOOP
#define OS_ULOG_PRINT_2(handle, formatStr, a, b) NOOP
#define OS_ULOG_PRINT_3(handle, formatStr, a, b, c) NOOP
#define OS_ULOG_PRINT_4(handle, formatStr, a, b, c, d) NOOP
#define OS_ULOG_PRINT_5(handle, formatStr, a, b, c, d, e) NOOP
#define OS_ULOG_PRINT_6(handle, formatStr, a, b, c, d, e, f) NOOP
#define OS_ULOG_PRINT_7(handle, formatStr, a, b, c, d, e, f, g) NOOP
#define OS_ULOG_PRINT_8(handle, formatStr, a, b, c, d, e, f, g, h) NOOP
#define OS_ULOG_PRINT_9(handle, formatStr, a, b, c, d, e, f, g, h, i) NOOP
#define OS_ULOG_PRINT_10(handle, formatStr, a, b, c, d, e, f, g, h, i, j) NOOP
#endif
/*
 * Structure definitions
 */


/* Generic header struture for the DM IPC-based message.
 */

typedef struct ipc_hdr_type
{
  uint32 q_MsgId;           /* IPC message ID */
  uint32 q_DestThreadId;    /* destination thread id */
  uint32 q_SrcThreadId;     /* source thread id */
  uint32 q_SubmissionCnt;   /* count of IPC messages submitted */
  uint32 q_ConsumptionCnt;  /* count of IPC messages consumed */
  uint32 q_TotalMsgCnt;
  uint32 q_IpcMsgSize;      /* size of IPC message payload, in bytes */
} ipc_hdr_type;

/* definition of the IPC-based log
 */
typedef struct LOG_CGPS_IPC_DATA_C_type
{
  log_hdr_type xx_hdr;      /* standard DM header */
  ipc_hdr_type ipc_hdr;     /* */
  uint8 u_Data[1];          /* IPC message payload */
} LOG_CGPS_IPC_DATA_C_type;

/* definition of the non-IPC-based free format log
 */
typedef struct LOG_CGPS_NON_IPC_DATA_C_type
{
  log_hdr_type xx_hdr;      /* standard DM header */
//  ipc_hdr_type ipc_hdr;     /* */
  uint8 u_Data[1];          /* free format data payload */
} LOG_CGPS_NON_IPC_DATA_C_type;


/* A typedef for generic IPC message data structure used across the system.
 */
typedef LOG_CGPS_IPC_DATA_C_type log_ipc_data_type;

/* A typedef for generic non-IPC-based free format data structure used across the system.
 */
typedef LOG_CGPS_NON_IPC_DATA_C_type log_non_ipc_data_type;

/* Save status of msg masks related to PFF messages when system enters uimage */
extern uint32 osq_PffMsgMask;

/*
 * Function definitions
*/

/*
 ******************************************************************************
 * log_ipc_msg
 *
 * Function description:
 *
 *	log_ipc_msg is used to log a IPC diagnostic message.
 *
 * Parameters: 
 *
 *	p_IpcMsg - pointer to the IPC message to be logged
 *
 * Return value: 
 *
 *	void
 *
 ******************************************************************************
*/

void log_ipc_msg( const os_IpcMsgType *p_IpcMsg );

/*
 ******************************************************************************
 * log_pff_msg
 *
 * Function description:
 *
 *  log_pff_msg is used to log a process free-format diagnostic message
 *    (formerly called a non-IPC message, and it shares a header with the
 *     IPC message)
 *
 * Parameters: 
 *
 *  q_SrcThreadId - thread from which this message is coming
 *  q_MsgId    - a message ID for the IPC header - enumed by each thread separately
 *  q_MsgSize  - Length of message payload (can be zero)
 *  p_Msg      - pointer to beginning of message payload (can be NULL if q_MsgSize is zero)
 *
 * Return value: 
 *
 *  true/false - success
 *
 *
 ******************************************************************************
*/

boolean log_pff_msg( os_ThreadIdEnumType q_SrcThreadId, uint32 q_MsgId, uint32 q_MsgSize, const byte *p_Msg );

/*
 ******************************************************************************
 * init_ipc_msg_data
 *
 * Function description:
 *
 *	init_ipc_msg_data is used to initialize the data assoicated with IPC 
 *    diagnostic messaging.
 *
 * Parameters: 
 *
 *	q_ThreadId
 *
 * Return value: 
 *
 *	void
 *
 ******************************************************************************
*/
void init_ipc_msg_data( os_ThreadIdEnumType q_ThreadId );

/*
 ******************************************************************************
 * inc_SubmissionCnt
 *
 * Function description:
 *
 *	inc_SubmissionCnt is used to increment and return the IPC submission counter
 *  associated with the specified thread Id
 *
 * Parameters: 
 *
 *	q_ThreadId - Id of thread for which submission count needs to be incremented
 *
 * Return value: 
 *
 *	q_SubmissionCnt - count of IPC messages submitted by the specified thread Id
 *
 ******************************************************************************
*/

uint32 inc_SubmissionCnt( os_ThreadIdEnumType q_ThreadId );

/*
 ******************************************************************************
 * init_NvIpcMasks
 *
 * Function description:
 *
 *	init_NvIpcMasks is used to set the IPC logging masks
 *
 * Parameters: 
 *
 *	p_NvMaskData - pointer to the IPC logging mask NV item 
 *
 * Return value: 
 *
 *	void
 *
 ******************************************************************************
*/

void init_NvIpcMasks( const uint32 *p_NvMaskData );

/* -------------------------------------------------------------------------
 * os_PffMsgStatus
 *
 * Description: 
 *  This function does a small part of the job done by msg_status(). It had to 
 *  be implemented due to the latter not being supported in uimage mode. 
 *  Return the ON/OFF status of caller-specified msg under MSG_SSID_MGPME category.
 *  Retrieve the msg mask saved when uimage was last entered. No mutex protection 
 *  for osq_PffMsgMask since this is just diag related.
 *
 * Parameters:
 *  q_MsgMask : QXDM Msg mask under MSG_SSID_MGPME category
 *  Return value: Boolean - TRUE/FALSE - Messaged turned ON/OFF in QXDM
 * -----------------------------------------------------------------------*/
boolean os_PffMsgStatus(uint32 q_MsgMask);

/* -------------------------------------------------------------------------
 * os_ULogInit
 *
 * Description: 
 *  Initialize Ulog Handle
 * Paramters: See ULogFront_RealTimeInit in core/api/power/ULogFront.h
 * Return: OS_ULOG_RESULT
 * -----------------------------------------------------------------------*/

OS_ULOG_RESULT os_ULogInit(OS_ULOG_HANDLE *p_Handle, /* A ULogHandle.  Used for all log accesses.*/
                           const char * name,        /* This must be < 24 chars */
                           uint32 q_ULogBuffSize,    /* Buffer Size, must be a power of 2, in this case 0 means disable the log */
                           uint32 q_ULogBufMemType,   /* Local or Shared memory */
                           uint8 u_LockType /* Whether to use lock when writing Log */
                           );

/* -------------------------------------------------------------------------
 * os_ULog_PrintString
 *
 * Description: 
 *  Print a String in Ulog. Use it to print Character Array
 * Paramters: ULogHandle, Character String
 * Return: None
 * -----------------------------------------------------------------------*/
void os_ULogPrintString(OS_ULOG_HANDLE p_handle, char * cStr); 

#ifdef __cplusplus
}
#endif

#endif /* ARIES_IPC_DIAG_LOG_H */
