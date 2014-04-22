/** @file FlashTargetLib.c
  
  Target Specific functions for FlashTargetLib

  Copyright (c) 2018-2019 Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


when       who       what, where, why
--------   ---       -----------------------------------------------------------
19/07/15   sl        default boot dec type to SPI NAND.
18/11/21   svl       Initial revision.


=============================================================================*/

#include "HALhwio.h"
#include "HALbootHWIO.h"
#include "NandBsp.h"

/*!
 * Returns if the boot device type is parallel or SPI NAND
 */
enum flash_boot_dev_type flash_get_boot_dev_type (void)
{
  /* remove option to initialize parallel/spi nand based on fastboot fuse
  */
  return FLASH_BOOT_DEV_NAND_SPI;

#if 0  //{
  /* Read Fastboot fuse for dev type */
  enum flash_boot_dev_type nand_dev_type = FLASH_BOOT_DEV_NONE;

  nand_dev_type = HWIO_BOOT_CONFIG_INM(HWIO_BOOT_CONFIG_FAST_BOOT_BMSK) >> HWIO_BOOT_CONFIG_FAST_BOOT_SHFT;

  /*
  * as per BOOT_CONFIG:FAST_BOOT register
  * will change from target to target
  * 0x0 - SPI-NAND
  * 0x1 - USB
  * 0x2 - Parallel NAND
  * 0x3 – Reserved
  * TODO: find a way to remove the magic numbers
  */
  switch(nand_dev_type)
  {
    case 0:
      return FLASH_BOOT_DEV_NAND_SPI;
    case 2:
      return FLASH_BOOT_DEV_NAND;
    default:
      return FLASH_BOOT_DEV_NONE;
  }
#endif  //}
}
