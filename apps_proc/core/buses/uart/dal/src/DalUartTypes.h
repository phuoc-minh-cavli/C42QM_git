#ifndef __DALUARTTYPES_H__
#define __DALUARTTYPES_H__
/*==================================================================================================

FILE: DalUartTypes.h

DESCRIPTION: Standard types used in DAL UART common layer.

                       Copyright (c) 2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     Qualcomm Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================
                                             DATATYPES
==================================================================================================*/

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef _BOOLEAN_DEFINED
typedef unsigned char  boolean;
#define _BOOLEAN_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef unsigned char  uint8;
#define _UINT8_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef unsigned short uint16;
#define _UINT16_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef unsigned long  uint32;
#define _UINT32_DEFINED
#endif

#endif  // __DALUARTTYPES_H__
