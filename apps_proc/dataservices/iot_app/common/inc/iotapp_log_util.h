/******************************************************************************

  @file    iotapp_log_util.h
  @brief   header file provides utility for logging

  ---------------------------------------------------------------------------
  Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/

#ifndef IOTAPP_LOG_H
#define IOTAPP_LOG_H

#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "qapi_diag.h"

/***************************
 Function declarations
**************************/

void iotapp_write_ulog(
	uint32_t mask, 
    uint8_t argnum,
    char* buf,
    const char *fmt, 	
    va_list ap);
void iot_log_print
(
  /*const char* file_name, uint32_t line_number, */uint32_t mask,
  uint8_t argnum,   
  const char *fmt, 
  ...
);

#define iotapp_log(file, line, mask, number, format, ...) iot_log_print(mask, number, format, ##__VA_ARGS__)

#ifdef ENABLE_COMMON_ERR_LOG
#define IOTAPP_LOG_ERR(format, ...) iot_log_print(  MSG_LEGACY_ERROR, MSG_NARG(format, ## __VA_ARGS__), format, ##__VA_ARGS__)
#else
#define IOTAPP_LOG_ERR(...)
#endif

#ifdef ENABLE_COMMON_INFO_LOG
#define IOTAPP_LOG_INFO(format,...) iot_log_print( MSG_LEGACY_LOW, MSG_NARG(format, ## __VA_ARGS__), format, ##__VA_ARGS__)
#else
#define IOTAPP_LOG_INFO(...) 
#endif

#define LOG_ERR(format,...) iot_log_print(  MSG_LEGACY_ERROR, MSG_NARG(format, ## __VA_ARGS__), format, ##__VA_ARGS__)


#endif
