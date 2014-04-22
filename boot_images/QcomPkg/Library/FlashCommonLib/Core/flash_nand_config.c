/*====================================================================
 *
 * FILE: flash_nand_config.c
 *
 * SERVICES: Functions and data for Flash NAND Configurations
 *
 * DESCRIPTION: This file contain initialization and interface APIs
 *              of flash config data, which is used across all NAND
 *              like devices (NAND, OneNAND)
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright 2007-2016, 2019 Qualcomm Technologies, Inc. All Rights Reserved.
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/FlashCommonLib/Core/flash_nand_config.c#2 $ $DateTime: 2019/04/23 17:39:55 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 02/27/18     eo      Use 4 byte aligned read for BBM marker 
 * 05/25/17     eo      Add support for Toshiba Nand device
 * 04/18/17     eo      Add 16bit ECC support
 * 12/01/15     sb      Clean-up
 * 09/19/14     eo      Update to support 9x45
 * 06/04/14     sb      Update to support 9x45
 * 01/08/13     sv      Add Toshiba 2K Page 8 bit ECC NAND Support
 * 12/04/12     sv      Add 4K Page Micron NAND Support
 * 10/04/12     sv      Update the transfer step values for 9x25
 * 07/27/12     sv      Remove unused devices from supported dev table
 * 12/14/11     eo      Add optimized transfer steps values
 * 05/20/11     bb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nand_config.h"

#define NUM_ARRAY_ELEMENTS(array, type)  (sizeof(array)/sizeof(type))

/* Transfer step values to use based on EBI2 clock frequency */
#define FLASH_EBI2_FREQ_CFG FLASH_EBI2_64MHZ
/*-----------------------------------------------------------------------
 *           Definitions and Constants
 *----------------------------------------------------------------------*/
/* x8, 2k page NAND devices */


/* Spare Bytes info for 2K x8 Device */
static struct flash_spare_info spare_info_x8_2048 =
{
  { {0, 2048, 4},    /* Bad Block Info - Page 0 */
    {1, 2048, 4}     /* Bad Block Info - Page 1 */
  },
  16, 0, 16,         /* udata max, udata ecc max, udata corrected bytes */
  NULL,              /* Spare udata encode APIs */
  NULL               /* Spare udata decode APIs */
};

/* Spare Bytes info for 2K x16 Device */
static struct flash_spare_info spare_info_x16_2048 =
{
  { {0, 2048, 4},     /* Bad Block Info - Page 0 */
    {1, 2048, 4}      /* Bad Block Info - Page 1 */
  },
  16, 0, 16,          /* udata max, udata ecc max, udata corrected bytes */
  NULL,               /* Spare udata encode APIs */
  NULL                /* Spare udata decode APIs */
};

/* Spare Bytes info for 4K x8 Device */
static struct flash_spare_info spare_info_x8_4096 =
{
  { {0, 4096, 4},     /* Bad Block Info - Page 0 */
    {1, 4096, 4}      /* Bad Block Info - Page 1 */
  },
  32, 0, 32,          /* udata max, udata ecc max, udata corrected bytes */
  NULL,               /* Spare udata encode APIs */
  NULL                /* Spare udata decode APIs */
};

/* Spare Bytes info for 4K x16 Device */
static struct flash_spare_info spare_info_x16_4096 =
{
  { {0, 4096, 4},     /* Bad Block Info - Page 0 */
    {1, 4096, 4}      /* Bad Block Info - Page 1 */
  },
  40, 0, 24,          /* udata max, udata ecc max, udata corrected bytes */
  NULL,               /* Spare udata encode APIs */
  NULL                /* Spare udata decode APIs */
};

/* Supported Regular NAND devices */
static struct flash_nand_params supported_nand_devices[] =
{
  /* Micron MT29F2G08Vtio flash ( x8 part) Virtio Supported
   * This device supported by Virtio 2K page, 1-plane,x8, non-ONFI.
   * page size: 2048 + 112 = 2160 bytes
   * block size: 64 pages
   * total size = 2048 blocks = 2Gbit
   */
  {
    FLASH_HAL_8_BIT_WIDTH,         /*  Device width */
    NAND_2K_8_BIT_XFER_CFG,        /*  Device transfer steps */
    FLASH_NAND_OTP_SEQUENCE_CFG3,  /*  OTP Config */
    &spare_info_x8_2048,           /*  Device Spare Info  */

    "MT29F4G08Vtio",                /*  Device name */
    {0x2C, 0xDC},                  /*  Device ID */
    2048,                          /*  Block Count */
    64,                            /*  Page Count */
    2048,                          /*  Page Size */
    64,                            /*  Spare Size */
    100,                           /*  tWB = 100 ns for Micron */
    0xff,                          /*  Operation status mask */
    0x01,                          /*  Operation success status value */
    {                              /*  Device feature Flags1: */
      NAND_1BIT_HW_ECC,            /*  Note: use NAND_MAX_SUPPORTED_HW_ECC for 16bit ECC */
      FLASH_DEFAULT_READ_MPAGES,
      FLASH_DEFAULT_WRITE_MPAGES,
      FLASH_DEFAULT_READ_WRITE_CP
    },
    {0x0},                         /* Device feature Flags2: */
  },

  /* Toshiba TC58NYG0S3HBAI6 flash (1.8V, x8 part)  
   * This device is added here because there was no way to identify the 
   * correct Spare size of this device since the Redundant Area Size 
   * specified in the Device IDs is not correct for this device.There are
   * 4 Bit ECC devices(64 byte spare area per page) in Toshiba as well with 
   * the same id(0xA1). We are hardcoding here to 8 Bit ECC part
   * (TC58NYG0S3HBAI6) for device id 0xA1. If support for 4 Bit ECC part is 
   * required, then this table needs to be updated with required values for 
   * the 4 Bit ECC part and 8 Bit ECC cannot be supported in the same build.
   * This needs to be added here to support TC58NYG0S3HBAI6 part unless there 
   * is a hardware fix from Toshiba.
   */
  {
    FLASH_HAL_8_BIT_WIDTH,         /*  Device width */
    NAND_2K_8_BIT_XFER_CFG,        /*  Device transfer steps */
    FLASH_NAND_OTP_SEQUENCE_CFG2,  /*  OTP Config */
    &spare_info_x8_2048,           /*  Device Spare Info  */
    "TC58NYG1S3HBAI6",             /*  Device name */
    {0x98, 0xAA},                  /*  Device ID */
    2048,                          /*  Block Count */
    64,                            /*  Page Count */
    2048,                          /*  Page Size */
    128,                           /*  Spare Size */
    100,                           /*  tWB = 100 ns for Toshiba */
    0xff,                          /*  Operation status mask */
    0x01,                          /*  Operation success status value */
    {                              /*  Device feature Flags1: */
      NAND_8BIT_HW_ECC,            /*  Note: use NAND_MAX_SUPPORTED_HW_ECC for 16bit ECC */
      FLASH_DEFAULT_READ_MPAGES,
      FLASH_DEFAULT_WRITE_MPAGES,
      FLASH_DEFAULT_READ_WRITE_CP
    },
    {0x0},                         /*  Device feature Flags2: */
  },
  /* Toshiba TC58NYG2S0HBA14 flash ( x8 part), non-ONFI1.0
     * This device is non-ONFI.
     * page size: 4096 + 256 = 4352 bytes
     * block size: 64 pages
     * total size = 2048 blocks = 4Gbit
     */ 
  {
    FLASH_HAL_8_BIT_WIDTH,	   /*  Device width */
    NAND_4K_8_BIT_XFER_CFG, 	   /*  Device transfer steps */
    FLASH_NAND_OTP_SEQUENCE_CFG2,  /*  OTP Config */
    &spare_info_x8_4096,	   /*  Device Spare Info  */
    "TC58NYG2S0HBAI4",		   /*  Device name */
    {0x98, 0xAC},		   /*  Device ID */
    2048,			   /*  Block Count */
    64, 			   /*  Page Count */
    4096,			   /*  Page Size */
    256,			   /*  Spare Size */
    100,			   /*  tWB = 100 ns for Toshiba */
    0xff,			   /*  Operation status mask */
    0x01,			   /*  Operation success status value */
    {				   /*  Device feature Flags1: */
      NAND_8BIT_HW_ECC,            /*  Note: use NAND_MAX_SUPPORTED_HW_ECC for 16bit ECC */
      FLASH_DEFAULT_READ_MPAGES,
      FLASH_DEFAULT_WRITE_MPAGES,
      FLASH_DEFAULT_READ_WRITE_CP
    },
    {0x0},                         /*  Device feature Flags2: */
  },

  /* Toshiba TC58NYG0S3HBAI6 flash (1.8V, x8 part)  
   * This device is added here because there was no way to identify the 
   * correct Spare size of this device since the Redundant Area Size 
   * specified in the Device IDs is not correct for this device.There are
   * 4 Bit ECC devices(64 byte spare area per page) in Toshiba as well with 
   * the same id(0xA1). We are hardcoding here to 8 Bit ECC part
   * (TC58NYG0S3HBAI6) for device id 0xA1. If support for 4 Bit ECC part is 
   * required, then this table needs to be updated with required values for 
   * the 4 Bit ECC part and 8 Bit ECC cannot be supported in the same build.
   * This needs to be added here to support TC58NYG0S3HBAI6 part unless there 
   * is a hardware fix from Toshiba.
   */
  {
    FLASH_HAL_8_BIT_WIDTH,         /*  Device width */
    NAND_2K_8_BIT_XFER_CFG,        /*  Device transfer steps */
    FLASH_NAND_OTP_SEQUENCE_CFG2,  /*  OTP Config */
    &spare_info_x8_2048,           /*  Device Spare Info  */
    "TC58NYG0S3HBAI6",             /*  Device name */
    {0x98, 0xA1},                  /*  Device ID */
    1024,                          /*  Block Count */
    64,                            /*  Page Count */
    2048,                          /*  Page Size */
    128,                           /*  Spare Size */
    100,                           /*  tWB = 100 ns for Toshiba */
    0xff,                          /*  Operation status mask */
    0x01,                          /*  Operation success status value */
    {                              /*  Device feature Flags1: */
      NAND_8BIT_HW_ECC,            /*  Note: use NAND_MAX_SUPPORTED_HW_ECC for 16bit ECC */
      FLASH_DEFAULT_READ_MPAGES,
      FLASH_DEFAULT_WRITE_MPAGES,
      FLASH_DEFAULT_READ_WRITE_CP
    },
    {0x0},                         /*  Device feature Flags2: */
  }
};

/* Supported Devices */
static struct flash_nand_cfg_data supported_nand_cfgs[] =
{
  {
    FLASH_HAL_DEVICE_NAND,
    FLASH_HAL_CS0,
    NUM_ARRAY_ELEMENTS(supported_nand_devices, flash_nand_params_type),
    supported_nand_devices,
    NULL
  },
  {
    FLASH_HAL_DEVICE_NONE,    /* TO MARK THE END -
                              Mandatory - Used in driver */
    FLASH_HAL_CS0,
    0,
    NULL,
    NULL
  }
};

/*
 * Return all supported NAND configurations
 */
void flash_nand_get_configs(struct flash_nand_cfg_data **cfg_data)
{
  (void)spare_info_x8_2048;
  (void)spare_info_x16_2048;
  (void)spare_info_x8_4096;
  (void)spare_info_x16_4096;
  *cfg_data = (struct flash_nand_cfg_data *) &supported_nand_cfgs;
}
