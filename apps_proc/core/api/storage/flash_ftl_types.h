#ifndef __FLASH_FTL_TYPES_H
#define __FLASH_FTL_TYPES_H

/**********************************************************************
 * flash_ftl_types.h
 *
 * Datatypes for Flash Translation layer (FTL).
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 **********************************************************************/
/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 2017-03-10   svl     Removed dependency on comdefs
 * 2016-11-11   svl     Review comments incorporated.
 * 2016-10-03   svl     Removed standar typedefs and used comdefs.h
 * 2016-08-24   svl     FTL layer support
 *=======================================================================*/

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

#ifndef _INT8_DEFINED
/** Signed 8-bit value.
*/
typedef  signed char        int8;        
#define _INT8_DEFINED
#endif

#define FLASH_FTL_OK                  (0)  /**< success */
#define FLASH_FTL_FAIL                (-1) /**< general failure */
#define FLASH_FTL_NOT_INIT            (-2) /**< ftl not initialized */
#define FLASH_FTL_INVALID_PARAM       (-3) /**< invalid parameters/arguments */
#define FLASH_FTL_OUT_OF_MEM          (-4) /**< malloc/any other allocation failed */
#define FLASH_FTL_OUT_OF_GOOD_BLOCKS  (-5) /**< no usable block present in partition */

typedef int32 FLASH_FTL_STATUS;

#endif /* __FLASH_FTL_TYPES_H */
