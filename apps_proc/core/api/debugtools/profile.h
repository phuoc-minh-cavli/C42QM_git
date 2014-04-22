#ifndef PROFILE_H
#define PROFILE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     TIME PROFILE SERVICE

GENERAL DESCRIPTION
  none

EXTERNALIZED FUNCTIONS
  none

INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

Copyright (c) 2000, 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2002, 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2004, 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

// #include "timetest.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define PROFILE_DEPRECATED_ATTRIBUTE __attribute__((deprecated))

// #define PROFILE_OUT(val)  outpw(TIMETEST_PORT, (uint16) (val))

#define PROFILE_OUT(val) /* { *TIMETEST_PORT = 0; } */

#define PROFILE_VAL_MASK    0x3fff
#define PROFILE_VAL_BITS    14

typedef enum
{
   PROFILE_TAG_EXT    = 0x0000,	  /* extention tag is used to output the val[15:14] or maybe above*/
   PROFILE_TAG_COMMON = 0x4000,	  /* common software tag [15:14] = 0b01 */
   PROFILE_TAG_1X     = 0x8000,	  /* 1x tag [15:14] = 0b10 */
   PROFILE_TAG_HDR    = 0xC000,	  /* HDR tag [15:14] = 0b11 */

} profile_tag_type;

/* output the value stamped by the TAG at [15:14] */
#define PROFILE_TAG_OUT(tag,val)	PROFILE_OUT((tag) | ((val) & PROFILE_VAL_MASK))

/* output the upper part of a value stamped by TAG 0b00 */
#define PROFILE_EXT_OUT(val)		PROFILE_OUT((val) >> PROFILE_VAL_BITS) & PROFILE_VAL_MASK)

#if defined(__cplusplus)
}
#endif

#endif // PROFILE_H
