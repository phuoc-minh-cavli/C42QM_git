#ifndef NAND_BSP_H
#define NAND_BSP_H  /*{*/
/*===========================================================================

                NAND BSP API

DESCRIPTION
  This header file defines target specific APIS for NAND driver.

  Copyright 2018 Qualcomm Technologies Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/21/18   sl      Created.

===========================================================================*/

/* Used to differentiate NAND vs SPI NAND boot */
enum flash_boot_dev_type
{
  FLASH_BOOT_DEV_NAND_SPI=0,  /**< SPI NAND device */
  FLASH_BOOT_DEV_NAND,        /**< Regular NAND device */
  FLASH_BOOT_DEV_NONE,        /**< No device */
};

/*
 * Returns if the boot device type is parallel or SPI NAND
 */
enum flash_boot_dev_type flash_get_boot_dev_type (void);


#endif /* NAND_BSP_H }*/
