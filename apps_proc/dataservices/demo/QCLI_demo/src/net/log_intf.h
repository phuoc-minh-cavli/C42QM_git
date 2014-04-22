/******************************************************************************
 * Copyright © 2017 Qualcomm Technologies, Inc.
 * @file    log_intf.h
 * @author  Qualcomm Technologies, Inc.
 * @brief   Contains the logging info interface
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "qapi_diag.h"

void m2m_stub_log
(
  const char* file_name, uint32_t line_number, uint32_t mask,
  const char *fmt, 
  ...
);

#define LOG_INFO(format, ...)  m2m_stub_log(__MODULE__, __LINE__, MSG_LEGACY_HIGH,  format, ##__VA_ARGS__);
#define LOG_DEBUG(format, ...) m2m_stub_log(__MODULE__, __LINE__, MSG_LEGACY_HIGH,  format, ##__VA_ARGS__);
#define LOG_ERROR(format, ...) m2m_stub_log(__MODULE__, __LINE__, MSG_LEGACY_ERROR, format, ##__VA_ARGS__);

