/*==============================================================================
  FILE:         uSleep_log.c
  
  OVERVIEW:     This file implements the uSleep logging system.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/log/uSleep_log.c#2 $
$DateTime: 2019/05/28 10:11:02 $
==============================================================================*/
#ifdef USLEEP_ENABLE_ULOGS

#include <stdarg.h>
#include "comdef.h"
#include "uSleep_log.h"
#include "ULog.h"
#include "ULogFront.h"

/*==============================================================================
                          VARIABLE DECLARATIONS
 =============================================================================*/
/* uSleep log handle & buffer */
ULogHandle g_uSleepLogHandle  = NULL;
char       *g_uSleepLogBuffer = NULL;





/*==============================================================================
                       GLOBAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleepLog_printf
 */
void uSleepLog_printf(int arg_count, const char *format, ...)
{
  va_list args;

  if(NULL == g_uSleepLogHandle)
  {
    return;
  }

  va_start(args, format);
  ULogFront_RealTimeVprintf(g_uSleepLogHandle, arg_count, format, args); 
  va_end(args);

  return;
}



#endif /* USLEEP_ENABLE_ULOGS */

