#ifndef __NOR_UTILS_H__
#define __NOR_UTILS_H__

/*=======================================================================
 * FILE:        nor_utils.h
 *
 * SERVICES:    Various utility functions used in NOR.
 *
 * GENERAL DESCRIPTION
 *    Various utility functions used in NOR.
 *
 * Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/NorJtagTools/nor_utils.h#1 $
 *   $DateTime: 2018/08/01 23:51:05 $
 *   $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 04/06/11     jz      Added support for reading platform ID from EEPROM
 * 09/23/10     sv      Initial version
 *==================================================================*/

uint32 flash_utils_erase_block(uint32 block);
uint32 flash_utils_read_page(void);
uint32 flash_utils_fill_page(void);
uint32 flash_utils_read_id (void);
uint32 flash_utils_read_platform_id(void);

#endif /* __NOR_UTILS_H__ */
