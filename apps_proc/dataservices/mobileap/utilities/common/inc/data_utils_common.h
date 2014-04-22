/***************************************************************************************************
    @file
    data_utils_common.h

    @brief
    Exports standard and custom utilities.

    Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#ifndef DATA_UTILS_COMMON
#define DATA_UTILS_COMMON

#include "data_utils_standard.h"
#include "data_util_list.h"
#include "data_util_memory.h"
#include "data_util_bit_field.h"
#include "data_util_synchronization.h"
#include "data_util_config.h"


#define INVALID_FILE_HANDLE              (-1)

#define IS_BIT_SET(value,bit) (value & (1<<bit))?TRUE:FALSE;

#define UTIL_LOG_MSG

#endif

