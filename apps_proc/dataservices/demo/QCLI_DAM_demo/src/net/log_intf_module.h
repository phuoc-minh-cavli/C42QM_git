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

#define LOG_INFO(fmt,  ...) QAPI_MSG_SPRINTF(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH,  fmt, ##__VA_ARGS__);
#define LOG_DEBUG(fmt, ...) QAPI_MSG_SPRINTF(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW,   fmt, ##__VA_ARGS__);
#define LOG_ERROR(fmt, ...) QAPI_MSG_SPRINTF(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__);

