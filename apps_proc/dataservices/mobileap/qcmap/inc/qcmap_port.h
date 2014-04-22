/******************************************************************************
  @file    qcmap_port.h
           The QCMAP port Header file.

  DESCRIPTION
  Header files and function declaration of Mobile AP.

  ---------------------------------------------------------------------------
  Copyright (c) 2019 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/

#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"

#define INVALID_FILE_HANDLE              (-1)

extern FILE * ds_log;
FILE * ds_log = NULL;


#define qcmap_platform_abort()     ASSERT(0)

#define qcmap_abort()  qcmap_platform_abort()

#define ds_assert(a)                                            \
        if (!(a)) {                                             \
            fprintf(stderr, "%s, %d: assertion (a) failed!",    \
                    __FILE__,                                   \
                    __LINE__);                                  \
            qcmap_abort();                                         \
        }


