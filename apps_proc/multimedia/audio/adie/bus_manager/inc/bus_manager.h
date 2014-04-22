#ifndef WCD_BUS_MANAGER_INTF_
#define WCD_BUS_MANAGER_INTF_

/**
@file bus_manager.h
@brief Bus manager for WCD
*/
/*-------------------------------------------------------------------------------
Copyright (c) 2013 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/bus_manager/inc/bus_manager.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/11/14   OA      Added digital core collapse
04/01/14   AG      Header update
03/02/13   KNM,SP  Refactored bus manager read/write interface to suit both
I2C & Slimbus.
12/15/10   SK      Initial revision
===============================================================================*/


#include "wcd_types.h"
#include "wcd_osal.h"

/**
* \brief The number of bits in each codec register
*/
#define WCD_REGISTER_DEPTH      8

/**
* \brief Returns the device handle for the bus.
*
* @param[reg] : Only used for I2C/I2S
*
*/
void* wcd_get_dev_handle_from_bus_handle(uint16* reg);

/**
* \brief Initializes the bus with the bus handle passed in.
* @param slimbus_used Refer to wcd_init options in wcd_intf.h
*/
wcd_result wcd_bus_init(wcd_bus_type bus_type);

/**
* \breif Deinitializes the bus.
*/
wcd_result wcd_bus_deinit(void);

/**
* \brief this api performs a register write to the codec
* @param ctrl_bus_intf
*  for slimbus:
*    ctrl_bus_intf = E_WCD_INTF if writing to SLIMBUS CODEC INTF (synchronous call)
*    ctrl_bus_intf = E_WCD_CODEC_MAIN is writing to SLIMBUS CODEC (PGD) (async call) or I2C,
*           the usage of wcd_bus_flush is required.
*  for I2C:
*    ctrl_bus_intf parameter is ignored.
* @param reg refers to the codec register to be written to
* @param mask Mask to be applied to the register before write
* @param shift shift to be applied to the data
* @param value Value to be written to the masked register, shift is applied to the value.
*/
wcd_result wcd_register_write(wcd_ctrl_bus_interface ctrl_bus_intf, uint16 reg, uint8 mask, uint8 shift, uint8 value);

/**
* \brief this api performs blocking a register write to the codec, this call flushes the previous register writes.
* @param ctrl_bus_intf
*  for slimbus:
*    ctrl_bus_intf = E_WCD_INTF if writing to SLIMBUS CODEC INTF (synchronous call)
*    ctrl_bus_intf = E_WCD_CODEC_MAIN is writing to SLIMBUS CODEC (PGD) (synchronous call) or I2C,
*           the usage of wcd_bus_flush is required.
*  for I2C:
*    ctrl_bus_intf parameter is ignored.
* @param reg refers to the codec register to be written to
* @param mask Mask to be applied to the register before write
* @param shift shift to be applied to the data
* @param value Value to be written to the masked register, shift is applied to the value.
*/
wcd_result wcd_register_write_sync(wcd_ctrl_bus_interface ctrl_bus_intf, uint16 reg, uint8 mask, uint8 shift, uint8 value);

/**
* \brief Flushes the bus, this function does not have efect on I2S
*/
wcd_result wcd_bus_flush(wcd_ctrl_bus_interface ctrl_bus_intf);

/**
* \brief Register read
*
*/
wcd_result wcd_register_read(wcd_ctrl_bus_interface ctrl_bus_intf, uint16 reg, uint8 mask, uint8 shift, uint8 *value);

/**
* \brief Obtains the logical address used by slimbus, no effect on I2C
*/
wcd_result wcd_bus_get_logical_address(uint8* logical_addr_pgd, uint8* logical_addr_intf);

/**
* \brief Initializes the shadow register with the default values.
*/
wcd_result wcd_bus_init_shadow_register(wcd_codec_info codec_info);


/**
* \brief Activates the bus for any bus reads/writes
*
*/
wcd_result wcd_activate_bus(void);

/**
* \brief Deactivates the bus preventing any bus reads/writes
*
*/
wcd_result wcd_deactivate_bus(void);

/**
* \brief Dumps the writable registers current values to the log.
*
*/
wcd_result wcd_dump_registers(void);

#endif
