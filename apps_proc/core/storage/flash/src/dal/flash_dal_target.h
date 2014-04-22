#ifndef FLASH_DAL_TARGET_H
#define FLASH_DAL_TARGET_H
/*=============================================================================
 *
 * FILE:      flash_dal_target.h
 *
 * DESCRIPTION: Flash dal target specific config file for 9X15
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2011-2013, 2018 QUALCOMM Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 12/21/18     sl      Add compile time SNAND/PNAND driver support
 * 01/18/13     eo      Add run time NAND/SPI NOR driver support
 * 08/21/12     sb      Add SPI NOR tools support for 9x25
 * 07/27/12     sv      Remove Data Mover specific codes 
 * 01/13/12     sb      Allocate max page size DAL WS buffer 
 * 09/16/11     jz      ehostdl image size reduction
 * 08/26/11     sv      Set correct SPI Device ID for 9x15
 * 07/22/11     sv      Add watchdog config for NOR tools
 * 06/22/11     eo      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/



//#include "core_variation.h"
#include "flash_nand_entry.h"
#include "DALDeviceId.h"

/*===================================================================
 *
 *             FLASH  BUILD ENVIRONMENT CONFIGURATION
 *
 ====================================================================*/
#if defined (IMAGE_APPS_PROC) || defined(IMAGE_QDSP6_PROC)
  #define FLASH_BUILD_ENV    FLASH_BUILD_APPS
#else
  #define FLASH_BUILD_ENV    FLASH_BUILD_MODEM
#endif

/*===================================================================
 *
 *             FLASH PROBE CONFIG
 *
 ====================================================================*/

#if defined(FLASH_ENABLE_NAND)
  #define FLASH_PRIMARY_DEV_NAND_PROBE   nand_entry_device_probe

  /* Used to differentiate NAND vs SPI NAND boot */
  enum flash_boot_dev_type
  {
    FLASH_BOOT_DEV_NAND_SPI=0,  /**< SPI NAND device */
    FLASH_BOOT_DEV_NAND,        /**< Regular NAND device */
    FLASH_BOOT_DEV_NONE,        /**< No device */
  };

  inline enum flash_boot_dev_type flash_get_boot_dev_type(void)
  {
    #ifdef FLASH_ENABLE_NAND_SPI
      return FLASH_BOOT_DEV_NAND_SPI;
    #else
      return FLASH_BOOT_DEV_NAND;
    #endif
  }

#endif

/*===================================================================
 *
 *             WATCHDOG CONFIG
 *
 ====================================================================*/


/*===================================================================
 *
 *            FLASH  DAL/HAL MEMORY CONFIGURATION
 *
 ====================================================================*/

/* Number of pages supported in a multi page operation.

   IMPORTANT : SET THIS TO 1 WHEN DEMAND PAGING ENABLED AS
               FLASH DRIVER IS SUPPOSED TO RELEASE LOCKS
               AFTER EVERY PAGE OPERATION.
*/
#if defined(FLASH_ENABLE_NAND)
  #define FLASH_MULTI_RW_SUPPORT_COUNT    1

  #if defined(FLASH_ENABLE_DMA)
    #if (FLASH_BUILD_ENV == FLASH_BUILD_HOSTDL)
      #define FLASH_HAL_WS_BUFF_SIZE_BYTES    2048  /* No OTP required */
    #else
      /* HAL Memory allocated */
      #define FLASH_HAL_WS_BUFF_SIZE_BYTES    3072  /* OTP Support needed */
    #endif
  #else
    #define FLASH_HAL_WS_BUFF_SIZE_BYTES      0     /* No HAL buffer needed for non-DM */
  #endif

  /* Flash DAL workspace buffer size
   *   Main page Bytes + Spare Bytes + 64 driver miscellaneous
   * NOTE: DAL WS buffer is not re-adjusted in non DM path
   * as per runtime detected page size
   */

  #define FLASH_DRV_WS_BUFF_SIZE_BYTES    (4096+160+64)
#endif


/*===================================================================
 *
 *            MEMCPY CONFIGURATION
 *
 ====================================================================*/

/* Memory util api's to be used by HAL */
#ifdef FEATURE_QDSP6
  /* Currently Q6 has issues with standard memory utils when trying to do
   * memory operations on uncached memory. So use the memutil api's developed
   * in HAL to handle this. This setting says the HAL to use internal memutil
   * APIs instead of standard APIs
   */
  #define FLASH_HAL_MEM_UTIL_TYPE   FLASH_HAL_USE_INT_MEMUTIL
#endif

#endif /* #ifndef FLASH_DAL_TARGET_H */
