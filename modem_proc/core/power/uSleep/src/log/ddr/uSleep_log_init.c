/*==============================================================================
  FILE:         uSleep_log_init.c
  
  OVERVIEW:     This file initializes the uSleep logging system.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/log/ddr/uSleep_log_init.c#2 $
$DateTime: 2019/05/28 10:11:02 $
==============================================================================*/
#ifdef USLEEP_ENABLE_ULOGS
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "CoreString.h"
#include "comdef.h"
#include "CoreVerify.h"
#include "ULog.h"
#include "ULogFront.h"
#include "uSleep_log.h"

#ifdef USLEEP_USE_DEVCFG
#include "DALFramework.h"
#endif

/*==============================================================================
                           LOCAL TYPES AND DEFS
 =============================================================================*/
#define USLEEP_ULOG_BUFFER_MIN  8110
#define USLEEP_ULOG_BUFFER_MAX  8192
/**
 * @brief MAX_BUF_LEN
 *
 * Maximum lenght of ulog header string
 */
#define MAX_BUF_LEN 80

/*==============================================================================
                           EXTERNAL VARIABLES
 =============================================================================*/
extern char *g_uSleepLogBuffer;

/*==============================================================================
                       GLOBAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleepLog_initialize
 */
void uSleepLog_initialize(void)
{

  uint32            logSize = USLEEP_ULOG_BUFFER_MIN;
   /* ULog Header substring. */
  char uLogBaseHeader[MAX_BUF_LEN];

#ifdef USLEEP_USE_DEVCFG
  DALSYSPropertyVar propVal;
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp); 
#endif

  /* Init uSleep Island logs */
  if(NULL == g_uSleepLogHandle)
  {
#ifdef USLEEP_USE_DEVCFG
    /* Use devcfg value if provided */
    if(DAL_SUCCESS == DALSYS_GetDALPropertyHandleStr("/core/power/uLog", hProp))
    {
      if(DAL_SUCCESS == DALSYS_GetPropertyValue(hProp, "usleep_log_size", 0, &propVal))
      {
        if(propVal.Val.dwVal > USLEEP_ULOG_BUFFER_MAX)
        {
          logSize = USLEEP_ULOG_BUFFER_MAX;
        }
        else if(propVal.Val.dwVal > USLEEP_ULOG_BUFFER_MIN)
        {
          logSize = propVal.Val.dwVal;
        }
      }
    }
#endif

    
	CORE_VERIFY(ULOG_ERR_INITINCOMPLETE == 
                            ULogFront_RealTimeInit(&g_uSleepLogHandle,
                                            "uSleep",
                                                    0,
                                                    ULOG_MEMORY_LOCAL,
                                                     ULOG_LOCK_NONE));

    /* Preparing ULog Header messages */
    core_snprintf(uLogBaseHeader, MAX_BUF_LEN, 
                     "Content-Type: text/dyn-sleep-log-1.0; title=:%s",
                     "uSleep");

    /* Setting ULog header */
    ULogCore_HeaderSet(g_uSleepLogHandle, uLogBaseHeader);			

    /* Allocating the required log memory */
    ULogCore_Allocate(g_uSleepLogHandle, logSize);

    /* Enabling the disabled log */
    ULogCore_Enable(g_uSleepLogHandle);	  

  }
  
  return;
}
#endif /* USLEEP_ENABLE_ULOGS */

