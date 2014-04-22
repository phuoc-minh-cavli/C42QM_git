#ifndef FLASH_CLOCKS_H
#define FLASH_CLOCKS_H

/**********************************************************************
 * flash_clocks.h
 *
 * SERVICES:    Header file for flash operation clocks used in NAND/SPI NOR
 *
 * DESCRIPTION: Header file for flash operation clocks used in NAND/SPI NOR
 * 
 * Copyright (c) 2013,2016 QUALCOMM Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/storage/flash/src/dal/flash_clocks.h#1 $ $DateTime: 2018/06/26 03:18:17 $ $Author: pwbldsvc $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
10/12/16     eo      nand driver multi-client support
06/27/16     sb      Cleanup 
05/23/13     eo      Initial Revision
===========================================================================*/
#include "comdef.h"

/* Flash Clocks Init */
void flash_clocks_init(void);

/* Flash Clocks Deinit */
void flash_clocks_deinit(void);    

/* Flash Clocks Request */
void flash_clocks_request(void);

/* Flash Clocks Request Complete */
void flash_clocks_request_complete(void);

/* Flash NAND Clocks Init */
void flash_nand_clocks_init(void);

/* Flash NAND Clocks Deinit */
void flash_nand_clocks_deinit(void);    

/* Flash NAND Clocks Request */
void flash_nand_clocks_request(void);

/* Flash NAND Clocks Request Complete */
void flash_nand_clocks_request_complete(void);
#endif /* FLASH_CLOCKS_H */
