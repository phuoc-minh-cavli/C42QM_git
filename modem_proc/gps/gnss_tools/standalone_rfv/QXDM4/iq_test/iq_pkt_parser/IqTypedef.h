/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               IqTest Driver (IQ) Typedefs

GENERAL DESCRIPTION
  This module contains:-
  
  Typedefs to be used in Iq Test driver code

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2005, 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss_tools/standalone_rfv/QXDM4/iq_test/iq_pkt_parser/IqTypedef.h#1 $ 
  $DateTime: 2020/07/21 21:23:44 $ 
  $Author: pwbldsvc $ 


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

 
/*
 * Include files
*/


typedef  unsigned char      boolean;     /* Boolean value type. */

typedef unsigned __int64    uint64;
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */

typedef  signed long int    int32;       /* Signed 32 bit value */
typedef  signed short       int16;       /* Signed 16 bit value */
typedef  signed char        int8;        /* Signed 8  bit value */

/* This group are the deprecated types.  Their use should be
** discontinued and new code should use the types above
*/
typedef  unsigned char     byte;         /* Unsigned 8  bit value type. */
typedef  unsigned short    word;         /* Unsinged 16 bit value type. */
typedef  unsigned long     dword;        /* Unsigned 32 bit value type. */

typedef  unsigned char     uint1;        /* Unsigned 8  bit value type. */
typedef  unsigned short    uint2;        /* Unsigned 16 bit value type. */
typedef  unsigned long     uint4;        /* Unsigned 32 bit value type. */

typedef  signed char       int1;         /* Signed 8  bit value type. */
typedef  signed short      int2;         /* Signed 16 bit value type. */
typedef  long int          int4;         /* Signed 32 bit value type. */

typedef  signed long       sint31;       /* Signed 32 bit value */
typedef  signed short      sint15;       /* Signed 16 bit value */
typedef  signed char       sint7;        /* Signed 8  bit value */

#define FALSE   0
#define TRUE    1

