#pragma once
/*=============================================================================
  @file sns_gpio_service_internal.h

  Framework header for Sensors GPIO Service.

  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

 * $Id: //components/rel/ssc.slpi/5.1/inc/internal/sns_gpio_service_internal.h#1 $
 * $DateTime: 2019/04/24 05:02:39 $
 * $Change: 18986584 $
  ===========================================================================*/

#include <stdint.h>
#include <stdbool.h>

#include "sns_interrupt.pb.h"
#include "sns_gpio_service.h"

/*=============================================================================
  Macros and constants
  ===========================================================================*/

/* If there are more GPIOs in future targets, add their
   registry group names and adjust the MAX GPIO value */
#define SNS_MAX_GPIO 20

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/** GPIO config with GPIO number and active/sleep configs */
typedef struct sns_gpio_config
{
  /* GPIO number of the config being stored */
  uint16_t gpio_num;
  /* Active function select config */
  uint8_t active_fs;
  /* Active direction: IN/OUT */
  uint8_t active_dir;
  /* Active pull type: NO PULL/UP/DOWN/KEEPER */
  uint8_t active_pull;
  /* Active drive strength: 2mA/4mA/6mA/8mA/10mA/12mA/14mA/16mA */
  uint8_t active_drive;
  /* Sleep function select config */
  uint8_t sleep_fs;
  /* Sleep direction: IN/OUT */
  uint8_t sleep_dir;
  /* Sleep pull type: NO PULL/UP/DOWN/KEEPER */
  uint8_t sleep_pull;
  /* Sleep drive strength: 2mA/4mA/6mA/8mA/10mA/12mA/14mA/16mA */
  uint8_t sleep_drive;
  /* unique identifier for each gpio num */
  uint32_t gpio_key;
} sns_gpio_config;

/** Forward Declaration. */
typedef struct sns_fw_gpio_service sns_fw_gpio_service;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * Set active/sleep GPIO config for the SSC GPIOs
 *
 * @param[i] sleep_config true if sleep config is to be set
 *                        false if active config is to be set
 */
void sns_gpio_service_set_sns_gpio_config(bool sleep_config);

/**
 * Store the GPIO active and sleep config for the provided GPIO
 *
 * @param[i] gpio_config GPIO config with active/sleep set values
 */
void sns_gpio_service_store_gpio_config(sns_gpio_config *gpio_config);

/**
 * Initialize the GPIO service; to be used only by the Service
 * Manager.
 */
sns_fw_gpio_service* sns_gpio_service_init(void);

/**
 * Configures GPIO for interrupt operation
 *
 * Note: this function may exit island mode to configure chip pins
 *
 * @param[i] req    Interrupt request
 * @param[i] enable true to enable the GPIO, false to disable
 * @param[i] enable setting of Inactive mode configuration
 *
 * @return
 * true if configuration was successful.
 */
bool sns_gpio_service_update_irq_pin(sns_interrupt_req const *req,
                                     bool enable, bool set_inactive_cfg);
/**
 * Write a value to a general purpose output pin.
 *
 * @param[i] gpio  gpio pin to write to
 * @param[i] is_chip_pin  true if gpio is chip level TLMM pin
 *		 else false
 * @param[i]  drive_strength  drive strength config
 * @param[i] pull  pull type config
 * @param[i] state	output state to set for the gpio
 *
 * @return sns_rc
 * SNS_RC_SUCCESS if gpio write is successful
 * SNS_RC_NOT_SUPPORTED in case of unsupported GPIO input
 * SNS_RC_FAILED for other errors
 */
sns_rc sns_write_gpio(uint32_t gpio,
						  bool is_chip_pin,
						  sns_gpio_drive_strength drive_strength,
						  sns_gpio_pull_type pull,
						  sns_gpio_state state);
