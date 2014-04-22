#ifndef WCD_INIT_MANAGER_INTF_
#define WCD_INIT_MANAGER_INTF_

/**
@file init_manager.h
@brief 
External init manager functions for WCD.
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/init_manager.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/01/13   KNM		Added support for buck voltage selection in wcd init and
                    in reset codec.
12/15/10	SK		Initial revision
===============================================================================*/

#include "wcd_result.h"
#include "wcd_intf.h"
#include "wcd_types.h"

/*
  wcd_init param structure used to pass initialization requirements to WCD driver.
  
  The idea of the init param structure is to be able to pass different initialization options, the 
  major and minor version fields must be populated accordingly to the required configurations, say
  version 0.0 will configure only the bustype, interrutp handler will not be configured since IHGpio
  is not provided on wcd_init_param v0.0
  
  each time the minor version is modified a new member was added to the structure, if members need to
  be removed or the structure needs to change, a new version MUST be created.
*/

typedef struct
{
  uint16 MajorVersion;
  uint16 MinorVersion;  
} wcd_init_param_version;

/*
  wcd_options contains all the possible flags and values modified by the condiguration
  it needs to be initialized with the default values 
*/
typedef struct
{
  wcd_bus_type bustype; // E_WCD_SLIMBUS or E_WCD_I2C
  uint32 IHGPIO;
  wcd_mclk_speed mclk_speed; //E_WCD_9_6_MHZ or E_WCD_12_288MHZ
  wcd_vdd_buck_voltage vdd_buck; // 1.8V or 2.15V
}wcd_options;

/**
 * \brief mad global information pointer.
 */
extern wcd_configure_mad* mad_global_info;

/** 
 * \brief Options the driver was initialized with.
 */
extern wcd_options wcd_init_options;


/**
 * \brief Driver initialized is used to know if wcd_init call was successful, if not deinit/ioctls cannot be performed.
 */
extern wcd_bool wcd_driver_initialized;

/** 
 * \brief Global extern structure for Client GPIO Interface CB
 */
extern struct wcd_client_intf_cb wcd_gpio_cb_data;

/** 
 * \brief Global extern structure for Client Event CB
 */
extern struct wcd_client_intf_cb wcd_client_event_cb_data;

/**
 * \brief Executes special WCD commands 
 *  such as Save Client GPIO Interface CB or Save Client Event CB
 */
wcd_result wcd_execute_special_command(unsigned int cmd, void* params, void *client_data);

/** 
 * \brief This function returns vdd buck voltage that is set during initialization.
 *
 */
wcd_result wcd_get_vdd_buck_voltage(wcd_vdd_buck_voltage *vdd_buck);


#endif
