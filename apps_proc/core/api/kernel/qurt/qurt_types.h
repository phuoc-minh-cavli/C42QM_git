#ifndef QURT_TYPES_H
#define QURT_TYPES_H
/*=======================================================================

FILE:         qurt_types.h

DESCRIPTION:  definition of basic types, constants, preprocessor macros


  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

$Header: //components/rel/core.tx/6.0/api/kernel/qurt/qurt_types.h#1 $

=========================================================================*/

#include "com_dtypes.h"

/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/

#define QURT_TIME_NO_WAIT          0x00000000
#define QURT_TIME_WAIT_FOREVER     0xFFFFFFFF
#define QURT_TIME_WAIT_FOREVER_LL  0xFFFFFFFFFFFFFFFF

/* timer expiry,after which next  timer interrupt will be generated */
#define QURT_TIMER_NEXT_EXPIRY	192000				/* 10 ms for 19.2 MHz */

#if !defined(INLINE)
#define INLINE static __inline
#endif
#define FORCE_INLINE static __forceinline


/*=============================================================================
                        TYPEDEFS
=============================================================================*/

/** qurt_time_t types. */

typedef uint64 qurt_time_t;

/** qurt_time_unit_t types.*/
typedef enum {
  QURT_TIME_TICK,                  /**< -- Return time in Ticks */
  QURT_TIME_USEC,                  /**< -- Return time in Microseconds */
  QURT_TIME_MSEC,                  /**< -- Return time in Milliseconds */
  QURT_TIME_SEC,                   /**< -- Return time in Seconds */
  QURT_TIME_NONE=QURT_TIME_TICK    /**< -- Identifier to use if no particular return type is needed */ 
}qurt_time_unit_t;

typedef uint32 word_t;

#endif /* QURT_TYPES_H */

