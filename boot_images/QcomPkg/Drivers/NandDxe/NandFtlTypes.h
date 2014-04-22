#ifndef __NAND_FTL_TYPES_H
#define __NAND_FTL_TYPES_H

/**********************************************************************
 * NandFtltypes.h
 *
 * Declaration for datatypes for Flash Translation layer (FTL).
 *
 * Copyright (c) 2016 Qualcomm Technologies Incorporated.
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
 * 2016-11-11   svl     Review comments incorporated.
 * 2016-10-03   svl     Removed standar typedefs and used comdefs.h
 * 2016-08-24   svl     FTL layer support
 *=======================================================================*/

#include <comdef.h>

#define FLASH_FTL_OK                  (0)  /**< success */
#define FLASH_FTL_FAIL                (-1) /**< general failure */
#define FLASH_FTL_NOT_INIT            (-2) /**< ftl not initialized */
#define FLASH_FTL_INVALID_PARAM       (-3) /**< invalid parameters/arguments */
#define FLASH_FTL_OUT_OF_MEM          (-4) /**< malloc/any other allocation failed */
#define FLASH_FTL_OUT_OF_GOOD_BLOCKS  (-5) /**< no usable block present in partition */

typedef int32 FLASH_FTL_STATUS;

#endif /* __NAND_FTL_TYPES_H */
