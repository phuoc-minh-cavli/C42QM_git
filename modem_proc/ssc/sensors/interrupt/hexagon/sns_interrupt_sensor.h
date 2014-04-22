#pragma once
/**
 * @file sns_interrupt_sensor.h
 *
 * The Interrupt virtual Sensor.
 *
 * Copyright (c) 2016-2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/5.1/sensors/interrupt/hexagon/sns_interrupt_sensor.h#2 $
 * $DateTime: 2019/05/18 23:41:41 $
 * $Change: 19237683 $
 *
 **/

#include "sns_sensor.h"
#include "sns_signal.h"
#include "sns_suid_util.h"
#include "sns_registry.pb.h"

/* Registry will specify GPIO Numbers for which Inactive Mode should be set
 * using this TAG */
#define IRQ_GPIO_TAG "irq_"

/*Max number of IRQ lines which uses software timestamp on specified platform.*/
/*Worst case limit. Generally we won't reach this point*/
#define MAX_SW_IRQ_LINES 20

/* Registry will specify GPIO Numbers for which Inactive Mode should be set
 * using this TAG */
#define INACTIVE_GPIO_TAG "inact_"

/*Max number of GPIOs which can have Inactive Mode configured.*/
#define MAX_INACTIVE_GPIOs 20

/*Interrupt sensor config details as specified in registry*/
typedef struct {
//List of irq numbers which uses s/w ts
  uint16_t sw_ts_irq_lines[MAX_SW_IRQ_LINES];
//List of GPIOs which require Inactive Mode configuration
  uint16_t inact_gpio_list[MAX_INACTIVE_GPIOs];
// Number of of irq lines specified in registry
  uint8_t sw_ts_irq_len;
// Number of of Inactive GPIOs specified in registry
  uint8_t inact_gpio_count;
} sns_irq_config;

/** Forward Declaration of Sensor API */
extern sns_sensor_api interrupt_sensor_api;

/** Interrupt Sensor State. */
typedef struct interrupt_state
{
  sns_signal_thread *signal_thread;
  //Used to search for the dependent sensors.
  //Dep sensors:registry.
  SNS_SUID_LOOKUP_DATA(1) suid_lookup_data;

  // Required for Registry stream
  sns_data_stream   *registry_stream;

  sns_irq_config config;
} interrupt_state;
