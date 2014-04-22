#pragma once
/** ======================================================================================
  @file sns_com_port_clk_resources.h

  @brief Manages bus clock resources

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Id: //components/rel/ssc.slpi/5.1/inc/internal/sns_com_port_clk_resources.h#1 $
  $DateTime: 2019/04/24 05:02:39 $
  $Change: 18986584 $
====================================================================================== **/


/**
*****************************************************************************************
                               Includes
*****************************************************************************************
*/
#include <stdbool.h>
#include <stdint.h>

#include "sns_com_port_types.h"

/**
*****************************************************************************************
                                  Constants & Macros
*****************************************************************************************
*/

/**
*****************************************************************************************
                               Forward Declarations
*****************************************************************************************
*/

/**
*****************************************************************************************
                                  Public Functions
*****************************************************************************************
*/

/**
 *  Enables clock resouces to operate the bus at the requested speed.
 *
 *  This must be called prior to any bus operations (like opening a bus).
 *  If the clock resources have to change, this may exit island mode.
 *  This function is reference counted per bus_instance -- release_clk_resources
 *  must be called an equal number of times for this clock resources
 *  to be released.
 *
 *  @param[i] bus_type           Bus type
 *  @param[i] bus_instance       As defined by the core APIs
 *  @param[i] new bus_speed_KHz  The bus speed for future bus operations
 *  @param[o] prev_bus_speed_KHz The bus speed previously requested (or 0)
 *
 *  @return
 *  SNS_RC_FAILED - internal failure
 *  SNS_RC_SUCCESS - action succeeded.
 */
sns_rc
sns_com_port_setup_clk_resources( sns_bus_type bus_type,
                                  uint8_t      bus_instance,
                                  int32_t      new_bus_speed_KHz,
                                  int32_t     *prev_bus_speed_KHz );

/**
 *  Releases clock resouces.
 *
 *  This will decriment the reference counter for clock resources. When the
 *  counter reaches 0, it will vote to turn off bus clocks.
 *  Note that releasing the clk resources will not "go back" to a previous
 *  bus speed.
 *
 *  @param[i] bus_type      Bus type
 *  @param[i] bus_instance  As defined by the core APIs
 *
 *  @return
 *  SNS_RC_FAILED - internal failure
 *  SNS_RC_SUCCESS - action succeeded.
 */
sns_rc
sns_com_port_release_clk_resources( sns_bus_type bus_type,
                                    uint8_t      bus_instance );
