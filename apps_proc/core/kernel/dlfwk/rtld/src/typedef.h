/*==============================================================================

Dynamic Loading Support Libs

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

--------------------------------------------------------------------------------
 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when               who         what, where, why
 --------           ---         -----------------------------------------------
 1/22/15          amitkulk      created 
==============================================================================*/

#ifndef TYPEDEF_H
#define TYPEDEF_H

/*==============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

// File Specification **********************************************************
//!
//! @file typedef.h
//!
//! @brief Common type definitions used by all files of this application.
//!
//******************************************************************************

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Types
//------------------------------------------------------------------------------
typedef char               int8;
typedef unsigned char      uint8;

typedef short              int16;
typedef unsigned short int uint16;

typedef int                int32;
typedef unsigned int       uint32;

#ifdef VCPP
typedef _int64             int64;
typedef unsigned _int64    uint64;
#else
typedef long long int          int64;
typedef unsigned long long int uint64;
#endif

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
#define MAX_HW_THREADS 6
#define MAX_SW_THREAD_NAME_LEN 32
#define MAX_INTERRUPTS 32

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#ifdef VCPP
#define F64  "I64"
#else
#define F64  "ll"
#endif

#define BIT_7(n)     ((n >> 7) & 0x1)
#define BIT_6(n)     ((n >> 6) & 0x1)
#define BIT_5(n)     ((n >> 5) & 0x1)
#define BIT_4(n)     ((n >> 4) & 0x1)
#define BIT_3(n)     ((n >> 3) & 0x1)
#define BIT_2(n)     ((n >> 2) & 0x1)
#define BIT_1(n)     ((n >> 1) & 0x1)
#define BIT_0(n)     ((n)      & 0x1)

#define BITS_7to6(n) ((n >> 6) & 0x3)
#define BITS_7to5(n) ((n >> 5) & 0x7)
#define BITS_7to4(n) ((n >> 4) & 0xf)
#define BITS_7to2(n) ((n >> 2) & 0x3f)
#define BITS_6to4(n) ((n >> 4) & 0x7)
#define BITS_5to4(n) ((n >> 4) & 0x3)
#define BITS_5to2(n) ((n >> 2) & 0xf)
#define BITS_3to2(n) ((n >> 2) & 0x3)
#define BITS_3to0(n) ((n)      & 0xf)
#define BITS_2to0(n) ((n)      & 0x7)
#define BITS_1to0(n) ((n)      & 0x3)

//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function Declarations
//------------------------------------------------------------------------------

#endif

