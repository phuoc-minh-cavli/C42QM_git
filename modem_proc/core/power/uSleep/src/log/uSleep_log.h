#ifndef _USLEEP_LOG_H_
#define _USLEEP_LOG_H_
/*==============================================================================
  FILE:         uSleep_log.h
  
  OVERVIEW:     Contains internal variables and functions declarations for the
                uSleep logging system
 
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/log/uSleep_log.h#2 $
$DateTime: 2019/05/28 10:11:02 $
==============================================================================*/
#ifdef USLEEP_ENABLE_ULOGS

#include <stdarg.h>
#include "comdef.h"
#include "ULog.h"
#include "ULogFront.h"

/*==============================================================================
                             DATA DECLARATIONS
 =============================================================================*/
extern ULogHandle g_uSleepLogHandle;

/*==============================================================================
                              INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * uSleepLog_initialize
 *
 * @brief Initializes the uSleep log system
 */
void uSleepLog_initialize(void);

/**
 * uSleepLog_printf
 *
 * @brief Function that will log the messages.
 *
 * @param arg_count: Number of parameteres that are being logged.
 * @param format: Format string for logging function.
 */
void uSleepLog_printf(int arg_count, const char *format, ...);

#else

#define uSleepLog_initialize()
#define uSleepLog_printf(arg_count, format, ...)

#endif /* USLEEP_ENABLE_ULOGS */

/**
 * uSleepLog_QDSSPrintf
 *
 * @brief This function is used to log an event both to QDSS, 
 *        as well as to the usleep log indicated in the first
 *        parameter.
 *  
 * @note If QDSS is not enabled, the normal log function will be called instead, 
 *       to minimize timeline impact.
 *
 * @param numArg:   Number of parameters that are being logged.
 * @param str:      The format string for printing log.
 * @param eventID:  QDSS tracer ID to use.
 */
#ifdef USLEEP_ENABLE_QDSS
#include "uSleep_tracer_event_ids.h"
#include "tracer_micro.h"

#define uSleepLog_QDSSEvent(numArg, eventID, ...)                         \
            UTRACER_EVENT((tracer_event_id_t)eventID, ## __VA_ARGS__)

#define uSleepLog_QDSSPrintf(numArg, str, eventID, ...)           \
            do                                                            \
            {                                                             \
              UTRACER_EVENT((tracer_event_id_t)eventID, ## __VA_ARGS__);  \
              uSleepLog_printf(numArg, str, ## __VA_ARGS__);      \
            } while (0)

#else /* QDSS ENABLED */

#define uSleepLog_QDSSEvent(numArg, eventID, ...)
#define uSleepLog_QDSSPrintf(numArg, str, eventID, ...)           \
            uSleepLog_printf(numArg, str, ## __VA_ARGS__ )

#endif /* USLEEP_ENABLE_QDSS */

#endif  /* _USLEEP_LOG_H_ */

