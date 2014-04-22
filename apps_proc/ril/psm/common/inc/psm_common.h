/******************************************************************************
#@file    psm_common.h
#@brief   All common declaracations, utilities
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2015 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#ifndef PSM_COMMON_H
#define PSM_COMMON_H

#include <stdio.h>
#include <stdarg.h>
#include <stringl/stringl.h>
#include "ULog.h"
#include "ULogFront.h"

#include "ril_port.h"

#include "psm_client.h"

#define NIL      (0)

#define SUCCESS  (0)
#define FAIL     (-1)

static ULogHandle     psm_ulog_handle;

#define PSM_ULOG_FILE_NAME               "PSM"
#define PSM_ULOG_BUF_SIZE                 16384
#define PSM_CLIENT_HC_TIMEOUT            (5*TIMER_1S)
#define PSM_SERVER_HC_TIMEOUT            (10*TIMER_1S)

#define PSM_LOG_VERBOSE(fmt) \
            RIL_LOG_MSG_VERBOSE(fmt)
#define PSM_LOG_VERBOSE1(fmt, arg) \
            RIL_LOG_MSG_VERBOSE1(fmt, arg)
#define PSM_LOG_VERBOSE2(fmt, arg1, arg2) \
            RIL_LOG_MSG_VERBOSE2(fmt, arg1, arg2)
#define PSM_LOG_VERBOSE3(fmt, arg1, arg2, arg3) \
            RIL_LOG_MSG_VERBOSE3(fmt, arg1, arg2, arg3)
#define PSM_LOG_VERBOSE_STR(...) \
            RIL_LOG_MSG_VERBOSE_STR(__VA_ARGS__)

#define PSM_LOG_INFO(fmt) \
            RIL_LOG_MSG_INFO(fmt)
#define PSM_LOG_INFO1(fmt, arg) \
            RIL_LOG_MSG_INFO1(fmt, arg)
#define PSM_LOG_INFO2(fmt, arg1, arg2) \
            RIL_LOG_MSG_INFO2(fmt, arg1, arg2)
#define PSM_LOG_INFO3(fmt, arg1, arg2, arg3) \
            RIL_LOG_MSG_INFO3(fmt, arg1, arg2, arg3)
#define PSM_LOG_INFO4(fmt, arg1, arg2, arg3, arg4) \
            RIL_LOG_MSG_INFO4(fmt, arg1, arg2, arg3, arg4)
#define PSM_LOG_INFO_STR(...) \
            RIL_LOG_MSG_INFO_STR(__VA_ARGS__)

#define PSM_LOG_DEBUG(fmt) \
            RIL_LOG_MSG_DEBUG(fmt)
#define PSM_LOG_DEBUG1(fmt, arg) \
            RIL_LOG_MSG_DEBUG1(fmt, arg)
#define PSM_LOG_DEBUG2(fmt, arg1, arg2) \
            RIL_LOG_MSG_DEBUG2(fmt, arg1, arg2)
#define PSM_LOG_DEBUG3(fmt, arg1, arg2, arg3) \
            RIL_LOG_MSG_DEBUG3(fmt, arg1, arg2, arg3)
#define PSM_LOG_DEBUG_STR(...) \
            RIL_LOG_MSG_DEBUG_STR(__VA_ARGS__)

#define PSM_LOG_ERROR(fmt) \
            RIL_LOG_MSG_ERROR(fmt)
#define PSM_LOG_ERROR1(fmt, arg) \
            RIL_LOG_MSG_ERROR1(fmt, arg)
#define PSM_LOG_ERROR2(fmt, arg1, arg2) \
            RIL_LOG_MSG_ERROR2(fmt, arg1, arg2)
#define PSM_LOG_ERROR3(fmt, arg1, arg2, arg3) \
            RIL_LOG_MSG_ERROR3(fmt, arg1, arg2, arg3)
#define PSM_LOG_ERROR_STR(...) \
            RIL_LOG_MSG_ERROR_STR(__VA_ARGS__)

#define PSM_LOG_MSG_CRITICAL(...) \
            RIL_LOG_MSG_CRITICAL(__VA_ARGS__)

#define PSM_LOG_FUNC_ENTRY(fmt) \
            PSM_LOG_VERBOSE(fmt)

#define PSM_LOG_FUNC_RETURN(fmt) \
            PSM_LOG_VERBOSE(fmt)

#define PSM_LOG_FUNC_RETURN_WITH_RET(result) \
            PSM_LOG_VERBOSE1("Exit with return value %d", result)


#define PSM_ULOG_NUM_ARGS(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,N,...) N

#define PSM_ULOG(...) ULogFront_RealTimePrintf(psm_ulog_handle, ((PSM_ULOG_NUM_ARGS(__VA_ARGS__, 11,10,9,8,7,6,5,4,3,2,1))-1), __VA_ARGS__)

#define PSM_ULOG_MSG(msg, args...) PSM_ULOG("%s: "msg, __func__, ##args)

#define PSM_CORE_ULOG_MSG(msg, args...) PSM_ULOG("CORE %s: "msg, __func__, ##args)

#define PSM_QMI_ULOG_MSG(msg, args...) PSM_ULOG("QMI %s: "msg, __func__, ##args)

#define PSM_CLIENT_ULOG_MSG(msg, args...) PSM_ULOG("CLIENT %s: "msg, __func__, ##args)

#if defined(USES_QNX) && defined(QNP_POWERMAN_PROC)
       static ULOG_LOCK_TYPE log_lock = ULOG_LOCK_NONE;
#else
       static ULOG_LOCK_TYPE log_lock = ULOG_LOCK_OS;
#endif

#define PSM_CRITICAL_ULOG_INIT() \
{\
    if( !psm_ulog_handle )\
    {\
        ULogFront_RealTimeInit(&psm_ulog_handle, PSM_ULOG_FILE_NAME, PSM_ULOG_BUF_SIZE, ULOG_MEMORY_LOCAL, log_lock);\
        ULogCore_Enable(psm_ulog_handle);\
    }\
}

typedef enum {
    PSM_REQUEST_START,
    PSM_REQUEST_FIRST = PSM_REQUEST_START,
    PSM_REQUEST_PSM_REGISTER,
    PSM_REQUEST_PSM_READY,
    PSM_REQUEST_PSM_NOT_READY,
    PSM_REQUEST_PSM_UNREGISTER,
    PSM_REQUEST_LOAD_MODEM,
    PSM_REQUEST_HEALTH_CHECK,
    PSM_REQUEST_BACKOFF,
    PSM_REQUEST_LAST
}psm_request_type_e;

typedef struct
{
    int               nw_access;
    int               active_timer;
    int               start_timer;
    int               end_timer;
}psm_req_info_type;

typedef struct
{
    int                 client_id;
    psm_request_type_e  request;
    psm_req_info_type   req_info;
}psm_req_msg_type;

#endif // PSM_COMMON_H


