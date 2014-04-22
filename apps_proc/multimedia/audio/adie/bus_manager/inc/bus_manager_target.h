#ifndef WCD_BUS_MANAGER_TARGET_INTF_
#define WCD_BUS_MANAGER_TARGET_INTF_

/**
@file bus_manager_target.h
@brief 
Bus manager target implementation for WCD
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2010 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/bus_manager/inc/bus_manager_target.h#4 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/11/14   OA      Added digital core collapse
12/15/10   SK      Initial revision
===============================================================================*/


#include "wcd_types.h"
#include "wcd_result.h"
#include "wcd_osal.h"

/**
 * \brief The total number of registers in the codec
 *
 */
#define WCD_NUMBER_OF_REGISTERS 1024



/**
 * \brief Initializes the shadow register to the default values upon reset
 */
wcd_result wcd_initialize_shadow_register(uint8* shadow_register, wcd_codec_info codec_info);

/**
 * \brief Initializes the cachable register array. Tells us which registers are cachable
 * or not. Each register is a bit in the array
 */
wcd_result wcd_initialize_noncachable_register(uint8* noncachable_registers, wcd_codec_info codec_info);

/**
 * \brief Sets the enumeration address for either I2C/SB PGD or SB INTF
 */
wcd_result wcd_set_enumeration_address(wcd_bus_type bus_type, uint8 bus_intf,
                                       uint8** enumeration_address, uint8* enum_addr_length);

/** 
 * \brief Dumps the registers for the target that are writable.
 */
wcd_result wcd_dump_target_registers(void);


#define WCD_SB_NUM_CORES              1
#define WCD_SLIMBUS_ROOT_CLK_FREQ     24576000
#define WCD_SLIMBUS_OFFSET            0x800
#define WCD_SB_BUS_DAL_DEVICE         DALDEVICEID_SLIMBUS_1
#define WCD_FLUSH_REGISTER            0x278


#define WCD_I2C_NUM_CORES             4
#define WCD_I2C_INVALID               0xFF
#define WCD_I2C_ADDRESS               0x0D
#define WCD_I2C_DEVICE_ID_MASK        0x0F00
#define WCD_I2C_DEVICE_ID_SHIFT       8
#define WCD_I2C_REG_MASK              0xFF
#define WCD_I2C_BUS_SPEED             100
#define WCD_TRANSFER_TIMEOUT          2500
#define WCD_I2C_BUS_DAL_DEVICE        DALDEVICEID_I2C_DEVICE_2     //changed from 3 to 4 


/******************************************************************************
 **************       SLIMBUS SHARED CHANNEL DEFINITIONS        ***************
 *****************************************************************************/
#define WCD_PGD_MAX_NUMBER_OF_TX_SLAVE_DEV_PORTS        13

#define WCD_PGD_MAX_NUMBER_OF_RX_SLAVE_DEV_PORTS        7

#define WCD_TX_PORT_ID_OFFSET                           0
#define WCD_RX_PORT_ID_OFFSET                           16

// slave port water mark level (0: 6bytes, 1: 9bytes, 2: 12 bytes, 3: 15 bytes)
#define WCD_SLAVE_PORT_WATER_MARK_VALUE                 2
#define WCD_SLAVE_PORT_WATER_MARK_SHIFT                 1 
#define WCD_SLAVE_PORT_ENABLE                           1
#define WCD_SLAVE_PORT_DISABLE                          0

#define WCD_PGD_TX_PORT_MULTI_CHANNEL_0_REG_BASE        0x100
#define WCD_PGD_TX_PORT_MULTI_CHANNEL_0_REG_OFFSET_MULT 4
#define WCD_PGD_TX_PORT_MULTI_CHANNEL_1_REG_BASE        0x101
#define WCD_PGD_TX_PORT_MULTI_CHANNEL_1_REG_OFFSET_MULT 4

#define WCD_PGD_RX_PORT_MULTI_CHANNEL_0_REG_BASE        0x180
#define WCD_PGD_RX_PORT_MULTI_CHANNEL_0_REG_OFFSET_MULT 4
#define WCD_PGD_RX_PORT_MULTI_CHANNEL_1_REG_BASE        0x181
#define WCD_PGD_RX_PORT_MULTI_CHANNEL_1_REG_OFFSET_MULT 4

#define WCD_PGD_TX_PORT_CONFIGURATION_REG_BASE          0x050
#define WCD_PGD_TX_PORT_CONFIGURATION_REG_OFFSET_MULT   1

#define WCD_PGD_RX_PORT_CONFIGURATION_REG_BASE          0x040
#define WCD_PGD_RX_PORT_CONFIGURATION_REG_OFFSET_MULT   1

#endif
