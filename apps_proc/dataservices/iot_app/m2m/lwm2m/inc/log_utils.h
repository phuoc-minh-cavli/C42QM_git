/******************************************************************************

  @file    log_utils.h
  @brief   Log Utitilities Interface File
           This file declares the interfaces and data structures if any, used for 
           logging messages on to remote host 

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

#ifndef _LOG_UTILS_H
#define _LOG_UTILS_H

#include <stdarg.h>
#include <stdbool.h>
#include "iotapp_log_util.h"
#include "qapi_diag.h"


//! Log macro for debug logs
#ifdef M2M_DEBUG
#define LOG_DEBUG(format, ...)  ds_iot_log_strf_low(format, ...)
#define LOG_URI(uriP)   PRINT_URI(uriP)


#else
#define LOG_DEBUG(...)
#define LOG_URI(URI)

#endif


#define PRINT_SIZE 256  // added 

bool lwm2m_isprint(char c);
int m2m_log_printf(const char *fmt,...);

#endif // End of #ifndef _LOG_UTILS_H
