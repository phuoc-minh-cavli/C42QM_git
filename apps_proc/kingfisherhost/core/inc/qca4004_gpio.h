/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/


#ifndef __QCA4004_GPIO_H__
#define __QCA4004_GPIO_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qapi_types.h"


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief This function will try to get the control of GPIO pins for QCA4004
   		and configure the GPIO pin with default.

   @return
    - 0 	if GPIO pins got and configure successfully.
    - 1 	if GPIO operation fail
*/
uint8_t qca4004_get_gpio_all(void);

/**
   @brief This function allows a client to relinquish the lock on the GPIO pins
   		for QCA4004

   @return
    - 0 	if GPIO pins were released successfully.
    - 1 	if one of GPIO pins could not be released
*/
uint8_t qca4004_release_gpio_all(void);

/**
   @brief This function drives the output of GPIO pins for QCA4004 to a specified value..

   @param value is Output value.

   @return
    - 0 	if GPIO operation successfully.
    - 1 	if GPIO operation fail
*/
uint8_t qca4004_set_gpio_all(uint8_t value);

#endif

