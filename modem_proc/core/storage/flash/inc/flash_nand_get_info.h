#ifndef FLASH_NAND_INFO_H
#define FLASH_NAND_INFO_H

/**********************************************************************
 * flash_nand_get_info.h
 *
 * SERVICES:    Header file for flash operations
 *
 * DESCRIPTION: Header file for flash operations
 * 
 * Copyright (c) 2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/storage/flash/inc/flash_nand_get_info.h#1 $ $DateTime: 2018/06/26 03:18:17 $ $Author: pwbldsvc $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
10/15/12     sb      Initial Revision
===========================================================================*/
#include "comdef.h"

/* Flash operation to get MIBIB partition table info */
void flash_ops_get_parti_tbl(uint32 *buffer, uint32 *size_in_bytes);

#endif /* FLASH_NAND_INFO_H */
