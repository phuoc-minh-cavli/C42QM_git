/* Copyright (c) 2016-2019, 2021 Qualcomm Technologies, Inc. and/or its subsidiaries.
   All rights reserved.
   Confidential - Qualcomm Technologies, Inc. - May Contain Trade Secrets
*/

/*
!! IMPORTANT NOTE: "DATA SERVICES" VERSION CATEGORY. 
*/

/**
 * @file qapi_types.h
 *
 * QAPI base type definitions
 *
 * @details This file provides the base type definitions used by the QAPI.
 *          This includes the basic integer types (based on stdint.h and
 *          stddef.h) and a basic boolean type.
 */

#ifndef __QAPI_TYPES_H__ // [
#define __QAPI_TYPES_H__

#include <stdint.h>
#include <stddef.h>

typedef uint32_t qbool_t;

#ifndef false
   #define false     (0)
#endif

#ifndef true
   #define true      (1)
#endif

#define A_TRUE          ((A_BOOL)1) //(QOSAL_TRUE)
#define A_FALSE         ((A_BOOL)0) //(QOSAL_FALSE)

typedef  unsigned char      boolean;
#define TRUE   1   /**< Boolean TRUE value. */
#define FALSE  0   /**< Boolean FALSE value. */


#endif // ] #ifndef __QAPI_TYPES_H__

