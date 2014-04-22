/***************************************************************************************************
    @file
    data_utils_standard.h

    @brief
    Exports standard utilities.

    Copyright (c) 2013 - 2019 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#ifndef DATA_UTILS_STANDARD
#define DATA_UTILS_STANDARD

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "fs_fcntl.h"
#include "fs_public.h"


#define ESUCCESS 0
#define EFAILURE 1

#ifndef NIL
#define NIL 0
#endif

#ifndef FALSE
#define FALSE 0
#endif 

#ifndef TRUE
#define TRUE 1
#endif

#define DATA_UTIL_ARR_SIZE( arr ) ( sizeof( ( arr ) ) / sizeof( ( arr[ 0 ] ) ) )

#endif

