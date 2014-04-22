/*=============================================================================

                        Session Manager API Header File

GENERAL DESCRIPTION
  gps_types.h contains the type definitions for the data types used within 
  the GPS software. This file may require modification as part of a host
  plaform / compiler change to support variable length changes. 
 
 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


#ifndef GPS_TYPES_H
#define GPS_TYPES_H

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "loc_types.h"
#include "stdint.h"


#ifndef _UINT32_DEFINED
/** Unsigned 32-bit value.
*/
typedef  unsigned long int  uint32;      
#define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
/** Unsigned 16-bit value.
*/
typedef  unsigned short     uint16;      
#define _UINT16_DEFINED
#endif

#ifndef _UINT8_DEFINED
/** Unsigned 8-bit value. 
*/
typedef  unsigned char      uint8;       
#define _UINT8_DEFINED
#endif

#ifndef _INT32_DEFINED
/** Signed 32-bit value.
*/
typedef  signed long int    int32;
#define _INT32_DEFINED
#endif

#ifndef _INT16_DEFINED
/** Signed 16-bit value.
*/
typedef  signed short       int16;
#define _INT16_DEFINED
#endif

#ifndef _BOOLEAN_DEFINED

/** @addtogroup utils_services
@{ */
/** Boolean value type. 
*/
typedef  unsigned char      boolean;     
#define _BOOLEAN_DEFINED
#endif

typedef	uint8	  U8;
typedef uint16  U16;
typedef uint32	U32;
typedef uint64  U64;

typedef	int8    S8;
typedef int16   S16;
typedef int32   S32;
typedef int64   S64;

#ifdef FEATURE_CGPS_PLAYBACK
  /* TDP 2.0 PE playback workaround */
  #ifndef UINT8_MAX
    #define UINT8_MAX _UI8_MAX
  #endif /* ! UINT8_MAX */
  #ifndef INT16_MAX
    #define INT16_MAX _I16_MAX
  #endif /* ! INT16_MAX */
  #ifndef UINT16_MAX
    #define UINT16_MAX _UI16_MAX
  #endif /* ! UINT16_MAX */
  #ifndef UINT32_MAX
    #define UINT32_MAX _UI32_MAX
  #endif /* ! UINT32_MAX */
  #ifndef INT32_MIN
    #define INT32_MIN  _I32_MIN
  #endif /* ! INT32_MIN */
  #ifndef INT32_MAX
    #define INT32_MAX  _I32_MAX
  #endif /* ! INT32_MAX */
#endif /* FEATURE_CGPS_PLAYBACK */

#define CGPS_MAX_UINT8  UINT8_MAX
#define CGPS_MAX_UINT16 UINT16_MAX
#define CGPS_MAX_UINT32 UINT32_MAX
#define CGPS_MAX_UINT64 UINT64_MAX
#define CGPS_MAX_INT16  INT16_MAX
#define CGPS_MIN_INT16  INT16_MIN
#define CGPS_MAX_INT32  INT32_MAX
#define CGPS_MIN_INT32  INT32_MIN
#define CGPS_MAX_INT64  INT64_MAX
#define CGPS_MIN_INT64  INT64_MIN

/* Structure used to process 32 bit complex quantities */
typedef struct
{
  int32 l_I;
  int32 l_Q;
} CplxS32;

/* Structure used to process 16 bit complex quantities */
typedef struct
{
  int16 x_I;
  int16 x_Q;
} CplxS16;

/* Structure used to process 8 bit complex quantities */
typedef struct
{
  int8 b_I;
  int8 b_Q;
} CplxS8;

/* AMSS defines TRUE/FALSE as integers (1 and 0).
   Lint complains if a boolean variable is assigned with TRUE or FALSE, because
   it expect "true" and "false" to be used for boolean values. This not 
   supported by the ADS ARM Compiler. So the best solution to avoid lint
   warnings is to typecast boolean to an unsigned char (as far as lint is concerned).
*/
#ifdef _lint
  typedef unsigned char boolean;
#endif

#ifdef __cplusplus
}
#endif

#endif /* GPS_TYPES_H */

