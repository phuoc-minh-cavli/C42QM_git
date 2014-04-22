#ifndef FLASH_BOOT_OPS_SETUP_H
#define FLASH_BOOT_OPS_SETUP_H

/**********************************************************************
 * flash_boot_ops_setup.h
 *
 * SERVICES:    Header file for flash operations used by Bootloader
 *
 * DESCRIPTION: Header file for flash operations used by Bootloader
 * 
 * Copyright (c) 2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/FlashCommonLib/Loader/flash_boot_ops_setup.h#1 $ $DateTime: 2018/08/01 23:51:05 $ $Author: pwbldsvc $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
07/01/11     eo      Initial Revision
===========================================================================*/
#include "comdef.h"

/* Flash boot operations to update SMEM AARM with MIBIB partition table */
void flash_boot_ops_get_parti_tbl(uintnt *buffer, uintnt *size_in_bytes);

#endif /* FLASH_BOOT_OPS_SETUP_H */
