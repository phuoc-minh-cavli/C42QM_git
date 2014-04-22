/*===========================================================================
                         PSM_APP_COMMON.H

DESCRIPTION
   File contains the utilities for the PSM demo application 
  
   Copyright (c) 2018 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

#include "txm_module.h"
#include "stdlib.h"
#include "stdint.h"
#include <stdarg.h>
#include "qapi_diag.h"
#include "qflog_utils.h"

/* Maximum length of log message */
#define PSM_MAX_LOG_MSG_SIZE       512

/* Log message to Diag */
#define PSM_LOG_MSG_DIAG( lvl, fmt, ... )  \
  { \
    char log_buf[ PSM_MAX_LOG_MSG_SIZE ]; \
    char log_fmt[ PSM_MAX_LOG_MSG_SIZE ]; \
    strlcpy(log_fmt, "%s: ", sizeof(log_fmt)); \
    strlcat(log_fmt, fmt, sizeof(log_fmt)); \
    /* Format message for logging */  \
    psm_dam_format_log_msg(log_buf, PSM_MAX_LOG_MSG_SIZE, log_fmt, __func__, ##__VA_ARGS__ ); \
    \
    /* Log message to Diag */  \
    QAPI_MSG_SPRINTF( MSG_SSID_ANDROID_QCRIL, lvl, "%s", log_buf );  \
    \
    /* Log message to Terminal */  \
    QFLOG_MSG(MSG_SSID_DFLT, lvl, "%s", log_buf); \
  }

#define LOG_INFO(...) PSM_LOG_MSG_DIAG(MSG_LEGACY_HIGH, __VA_ARGS__)

#define LOG_DEBUG(...) PSM_LOG_MSG_DIAG(MSG_LEGACY_MED, __VA_ARGS__)

#define LOG_ERROR(...) PSM_LOG_MSG_DIAG(MSG_LEGACY_ERROR, __VA_ARGS__)

#define LOG_FATAL(...) PSM_LOG_MSG_DIAG(MSG_LEGACY_FATAL, __VA_ARGS__)

void psm_dam_format_log_msg
(
 char *buf_ptr,
 int buf_size,
 char *fmt,
 ...
);

UINT memscpy
(
  void   *dst,
  UINT   dst_size,
  const  void  *src,
  UINT   src_size
);

