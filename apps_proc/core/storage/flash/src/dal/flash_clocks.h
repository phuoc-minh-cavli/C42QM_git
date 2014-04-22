#ifndef FLASH_CLOCKS_H
#define FLASH_CLOCKS_H

/**********************************************************************
 * flash_clocks.h
 *
 * SERVICES:    Header file for flash operation clocks used in NAND/SPI NOR
 *
 * DESCRIPTION: Header file for flash operation clocks used in NAND/SPI NOR
 * 
 * Copyright (c) 2016 QUALCOMM Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/flash/src/dal/flash_clocks.h#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
09/22/16     sb      Initial Version
===========================================================================*/

/* Flash Clocks Init */
void flash_clocks_init(void);

/* Flash Clocks Deinit */
void flash_clocks_deinit(void);    

/* Flash Clocks Request */
void flash_clocks_request(void);

/* Flash Clocks Request Complete */
void flash_clocks_request_complete(void);

#endif /* FLASH_CLOCKS_H */
