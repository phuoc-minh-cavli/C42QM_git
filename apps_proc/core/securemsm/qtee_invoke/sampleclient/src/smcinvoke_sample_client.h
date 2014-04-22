/********************************************************************
---------------------------------------------------------------------
 Copyright (c) 2019 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
----------------------------------------------------------------------
 SMCInvoke AppManager Sample/Test Common header
*********************************************************************/

#ifndef _SMCINVOKE_SAMPLE_CLIENT_H_
#define _SMCINVOKE_SAMPLE_CLIENT_H_

#include "qseecom_log.h"

#pragma once

typedef unsigned int bool;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long long uint64_t;

#define LOGD_PRINT LOGSD
#define LOGE_PRINT LOGSE

#define PRINT LOGD_PRINT

/* Macros for testing */
#define TEST_OK(xx)                                                     \
  do                                                                    \
  {                                                                     \
    if ((xx))                                                           \
    {                                                                   \
      LOGE_PRINT("[%s]::Failed!", __FUNCTION__);                        \
      return -1;                                                        \
    }                                                                   \
    LOGD_PRINT("[%s]::Passed!", __FUNCTION__);                          \
  } while(0)

#define SILENT_OK(xx)                                                   \
  do                                                                    \
  {                                                                     \
    if ((xx))                                                           \
    {                                                                   \
      LOGE_PRINT("[%s]::Failed!", __FUNCTION__);                        \
      return -1;                                                        \
    }                                                                   \
  } while(0)

#define TEST_FAIL(xx)   TEST_OK(!(xx))
#define TEST_FALSE(xx)  TEST_OK(xx)
#define TEST_TRUE(xx)   TEST_FAIL(xx)

#define SILENT_FAIL(xx)   SILENT_OK(!(xx))
#define SILENT_FALSE(xx)  SILENT_OK(xx)
#define SILENT_TRUE(xx)   SILENT_FAIL(xx)

#endif // _SMCINVOKE_SAMPLE_CLIENT_H_
