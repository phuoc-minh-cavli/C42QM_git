#ifndef FLASH_EXTDRV_I2C_9205_H
#define FLASH_EXTDRV_I2C_9205_H

/*=======================================================================
 * FILE:        flash_extdrv_i2c_9205.h
 *
 * SERVICES:
 *
 * DESCRIPTION: Header file for I2C related registers
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2015, 2016, 2018, 2019 Qualcomm Technologies Incorporated.
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
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 22/01/19     sl      Updated EEPROM i2c address for 9205
 * 11/01/18     sb      Updated for 9205
 * 08/24/18     svl     Update EEPROM dev id for VT
 * 02/23/18     svl     Update to support VT
 * 07/11/16     eo      Update custome EEPROM support
 * 03/17/15     sb      Update to support 9x65
 * 02/17/15     sb      Update to support 9x55
 * 08/04/14     sb      Fix to support on Virtio
 * 06/25/14     sb      Updated to support 9x45
 * 06/09/14     whc     accurate i2c clock support
 * 11/25/13     eo      Replace busy loop cntr with busywait()
 * 10/18/13     whc     Intial 9x35 changes

 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "msmhwiobase.h"
#include "flash_extdrv_i2c.h"

/* Enable I2C multi-byte support */
#define FLASH_I2C_ENABLE_MULTIBYTE_READ
#define FLASH_I2C_ENABLE_MULTIBYTE_WRITE

/* EEPROM device address */
#ifndef BOOT_PRE_SILICON
  #define FLASH_I2C_DEVICE_ADDR               0x52
#else
  #define FLASH_I2C_DEVICE_ADDR               0x51
#endif

#define BLSP1_QUP0                          (0x078B5000)
#define BLSP1_QUP1                          (0x078B6000)
#define BLSP1_QUP2                          (0x078B7000)
#define BLSP1_QUP3                          (0x078B8000)

#define TLMM_CSR_REG_BASE                   (TLMM_BASE + 00000000)
#define HWIO_GPIO_CFGn_ADDR(c) \
  (TLMM_CSR_REG_BASE + 0x00000000 + 0x1000 * (c))

#ifndef BOOT_PRE_SILICON
  #define I2C_GSBI_QUP_REG_BASE              BLSP1_QUP1
  #define I2C_GSBI_REG_BASE                  0x0 //NOP: Not Appblicable
  #define I2C_GSBI_QUP_APPS_MD_ADDR          0x0 //NOP: Not Applicable
  #define I2C_GSBI_QUP_APPS_NS_ADDR          0x0 //NOP: Not Applicable
  #define I2C_GSBI_HCLK_CTL_ADDR             0x0 //NOP: Not Applicable
  /* GPIO pins for EEPROM */
  /* Temp use RUMI GPIO config */
  #define I2C_GPIO_CFG_SDA_ADDR              HWIO_GPIO_CFGn_ADDR(6)
  #define I2C_GPIO_CFG_CLK_ADDR              HWIO_GPIO_CFGn_ADDR(7)
#else
  #define I2C_GSBI_QUP_REG_BASE              BLSP1_QUP1
  #define I2C_GSBI_REG_BASE                  0x0
  #define I2C_GSBI_QUP_APPS_MD_ADDR          0x0
  #define I2C_GSBI_QUP_APPS_NS_ADDR          0x0
  #define I2C_GSBI_HCLK_CTL_ADDR             0x0
  /* GPIO pins for EEPROM */
  #define I2C_GPIO_CFG_SDA_ADDR              HWIO_GPIO_CFGn_ADDR(6)
  #define I2C_GPIO_CFG_CLK_ADDR              HWIO_GPIO_CFGn_ADDR(7)
#endif

/* GPIO pull up, 0mA drive mode (NOTE this might need adjusted for CDP/MTP/IDP) */
#ifndef BOOT_PRE_SILICON
#define I2C_SDA_GPIO_CFG_VAL               0xF
#define I2C_CLK_GPIO_CFG_VAL               0xF
#else
#define I2C_SDA_GPIO_CFG_VAL               0xF
#define I2C_CLK_GPIO_CFG_VAL               0xF
#endif

#ifndef BOOT_PRE_SILICON
  #define I2C_RW_CHECK_LOOP_COUNT          500000
#else
  #define I2C_RW_CHECK_LOOP_COUNT          5000
#endif

#define I2C_GSBI_QUP_CONFIG_ADDR           (I2C_GSBI_QUP_REG_BASE + 0x00000000)
#define I2C_GSBI_QUP_STATE_ADDR            (I2C_GSBI_QUP_REG_BASE + 0x00000004)
#define I2C_GSBI_QUP_IO_MODES_ADDR         (I2C_GSBI_QUP_REG_BASE + 0x00000008)
#define I2C_GSBI_QUP_SW_RESET_ADDR         (I2C_GSBI_QUP_REG_BASE + 0x0000000c)
#define I2C_GSBI_QUP_OPERATIONAL_ADDR      (I2C_GSBI_QUP_REG_BASE + 0x00000018)
#define I2C_GSBI_QUP_ERROR_FLAGS_ADDR      (I2C_GSBI_QUP_REG_BASE + 0x0000001c)
#define I2C_GSBI_QUP_ERROR_FLAGS_EN_ADDR   (I2C_GSBI_QUP_REG_BASE + 0x00000020)
#define I2C_GSBI_QUP_MX_OUTPUT_COUNT_ADDR  (I2C_GSBI_QUP_REG_BASE + 0x00000100)
#define I2C_GSBI_QUP_MX_WRITE_COUNT_ADDR   (I2C_GSBI_QUP_REG_BASE + 0x00000150)
#define I2C_GSBI_QUP_MX_INPUT_COUNT_ADDR   (I2C_GSBI_QUP_REG_BASE + 0x00000200)
#define I2C_GSBI_QUP_MX_READ_COUNT_ADDR    (I2C_GSBI_QUP_REG_BASE + 0x00000208)
#define I2C_GSBI_I2C_MASTER_CLK_CTL_ADDR   (I2C_GSBI_QUP_REG_BASE + 0x00000400)

#define I2C_GSBI_QUP_OUTPUT_FIFOc_ADDR(c)  \
  (I2C_GSBI_QUP_REG_BASE + 0x00000110 + 4 * (c))
#define I2C_GSBI_QUP_INPUT_FIFOc_ADDR(c)   \
  (I2C_GSBI_QUP_REG_BASE + 0x00000218 + 4 * (c))

#define I2C_GSBI_QUP_OUTPUT_FIFO_ADDR  I2C_GSBI_QUP_OUTPUT_FIFOc_ADDR(0)
#define I2C_GSBI_QUP_INPUT_FIFO_ADDR   I2C_GSBI_QUP_INPUT_FIFOc_ADDR(0)

#define I2C_GSBI_CTRL_REG_ADDR             (I2C_GSBI_REG_BASE + 00000000)
                                   
/* I2C, QUP, GSBI Bit */
#define I2C_STATE_VALID \
  I2C_GSBI_QUP_STATE_STATE_VALID_BMSK
#define I2C_OUTPUT_DONE_FLAG \
  I2C_GSBI_QUP_OPERATIONAL_MAX_OUTPUT_DONE_FLAG_BMSK
#define I2C_INPUT_DONE_FLAG \
  I2C_GSBI_QUP_OPERATIONAL_MAX_INPUT_DONE_FLAG_BMSK
#define I2C_IO_MODES_UNPACK_EN \
  I2C_GSBI_QUP_IO_MODES_UNPACK_EN_BMSK
#define I2C_IO_MODES_PACK_EN \
  I2C_GSBI_QUP_IO_MODES_PACK_EN_BMSK
#define I2C_CFG_N_BMSK \
  I2C_GSBI_QUP_CONFIG_N_BMSK
#define I2C_PROTOCOL_CODE_BMSK \
  I2C_GSBI_GSBI_CTRL_REG_PROTOCOL_CODE_BMSK
#define I2C_CFG_MINI_CORE_BMSK \
  I2C_GSBI_QUP_CONFIG_MINI_CORE_BMSK
#define I2C_INPUT_MODE_BMSK \
  I2C_GSBI_QUP_IO_MODES_INPUT_MODE_BMSK
#define I2C_OUTPUT_MODE_BMSK \
  I2C_GSBI_QUP_IO_MODES_OUTPUT_MODE_BMSK
#define I2C_GSBI_HCLK_ENABLE \
  I2C_GSBI_HCLK_CTL_CLK_BRANCH_ENA_BMSK
#define I2C_HIGH_TIME_DIVIDER_VALUE_BMSK \
  I2C_GSBI_MASTER_CLK_CTL_HIGH_TIME_DIVIDER_VALUE_BMSK
#define I2C_HIGH_TIME_DIVIDER_VALUE_SHFT    16

/* The following is extracted from msmhwioreg.h without GSBI port dependency */
#define I2C_GSBI_QUP_STATE_STATE_VALID_BMSK                    0x4
#define I2C_GSBI_QUP_OPERATIONAL_MAX_OUTPUT_DONE_FLAG_BMSK     0x400
#define I2C_GSBI_QUP_OPERATIONAL_MAX_INPUT_DONE_FLAG_BMSK      0x800
#define I2C_GSBI_QUP_IO_MODES_UNPACK_EN_BMSK                   0x4000
#define I2C_GSBI_QUP_IO_MODES_PACK_EN_BMSK                     0x8000
#define I2C_GSBI_QUP_CONFIG_N_BMSK                             0x1f
#define I2C_GSBI_GSBI_CTRL_REG_PROTOCOL_CODE_BMSK              0x70
#define I2C_GSBI_QUP_CONFIG_MINI_CORE_BMSK                     0xf00
#define I2C_GSBI_QUP_IO_MODES_INPUT_MODE_BMSK                  0x3000
#define I2C_GSBI_QUP_IO_MODES_OUTPUT_MODE_BMSK                 0xc00
#define I2C_GSBI_HCLK_CTL_CLK_BRANCH_ENA_BMSK                  0x10
#define I2C_GSBI_MASTER_CLK_CTL_HIGH_TIME_DIVIDER_VALUE_BMSK   0xff0000

#define WDOG_RESET_ADDR                                        0x00280040
#define WDOG_RESET_VAL                                         1

/* Define I2C clock setting. */
#define I2C_CLOCK_KHZ                                          I2C_CLK_400KHZ

void i2c_reconfigure(struct i2c_reg_addrs *i2c_reg_addr, 
  struct i2c_config_data* i2c_cfg_data_val) \
{}

#endif  /* FLASH_EXTDRV_I2C_9205_H */
