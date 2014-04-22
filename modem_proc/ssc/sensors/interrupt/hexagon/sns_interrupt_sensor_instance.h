#pragma once
/**
 * @file sns_interrupt_sensor_instance.h
 *
 * Interrupt Sensor Instance.
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/5.1/sensors/interrupt/hexagon/sns_interrupt_sensor_instance.h#2 $
 * $DateTime: 2019/05/18 23:41:41 $
 * $Change: 19237683 $
 *
 **/

#include <stdint.h>
#include "DalDevice.h"
#include "sns_data_stream.h"
#include "sns_event_service.h"
#include "sns_interrupt.pb.h"
#include "sns_sensor_instance.h"
#include "sns_signal.h"
#include "sns_time.h"
#include "sns_interrupt_sensor_configure.h"
#include "sns_interrupt_sensor.h"

/** Forward Declaration of Instance API */
extern sns_sensor_instance_api interrupt_sensor_instance_api;

typedef struct sns_pin_tlmm_info
{
  /** Chip TLMM or SSC TLMM pin. Chip level pins use DAL APIs */
  bool              is_chip_pin;

  /** GPIO DAL handle incase of Chip TLMM pin. NULL for SSC
   *  TLMM pin */
  DalDeviceHandle   *gpio_int_handle;
} sns_pin_tlmm_info;

/** Signal state maintained per gpio. */
typedef struct
{
  /** Interrupt number. */
  uint16_t                     irq_num;

  /** Number of times an unregistered interrupt fires.
   *  This is used for debug only. */
  uint16_t                     warning_cnt;

  /** Timestamp of most recent trigger event on the irq_pin. */
  sns_time                     timestamp;

  /** Pin config of the interrupt. */
  uint32_t                     irq_pin_cfg;

  /** Chip TLMM or SSC TLMM pin info   */
  sns_pin_tlmm_info            pin_tlmm_info;

  /** The trigger config of this interrupt. */
  uint32_t                     trigger_type;

  /** true if this signal is registered. */
  bool                         is_registered;

  /** true if this interrupt uses the hardware timestamping unit to capture the
   *  timestamp. */
  bool                         uses_hw_timestamp_unit;

} sns_interrupt_info;

typedef struct
{
  /** Handle to I2C/I3C bus for IBI */
  void    *i2c_handle;

  /** Timestamp of most recent trigger event of the callbackw */
  sns_time timestamp;

  /** Place to store IBI data. Must point to PRAM*/
  uint8_t*  ibi_data_buf;

  /** true if this signal is registered. */
  bool     is_registered;
} sns_ibi_info;

/**
* Ping-Pong buffer for interrupt sensor
*/
#define SNS_INTERRUPTS_CIRCULAR_BUFFER_DEPTH 5
typedef struct
{
  uint32_t events_occured;
  struct
  {
    sns_time timestamp;
    // Timestamp status, as returned by the uGPIOInt_ReadTimestamp API.
    // PEND: Publish the timestamp status so that the clients can handle corner
    //       cases instead of the interrupt sensor having to make all of the
    //       hard decisions.
    union {
      // For hardware interrupts:
      sns_interrupt_timestamp_status ts_status;

      // For IBI
      uint8_t num_ibi_bytes;
    };
  } circular_buffer[SNS_INTERRUPTS_CIRCULAR_BUFFER_DEPTH];
}read_only_outside_IST;

typedef void(*sns_interrupt_event_fptr)(sns_sensor_instance *const this);

/** Interrupt Sensor Instance State. The Interrupt Sensor
 *  uses a Sensor Instance per irq_num.
 *  Example: If Accel, Gyro and Motion Detect sensors use the
 *  same irq_num then a single Sensor instance handles this
 *  interrupt stream. Where as a Mag sensor stream will be
 *  handled by another Sensor Instance. */
typedef struct sns_interrupt_instance_state
{
  sns_interrupt_event_fptr event_fptr;
  union {
    /** Information for the irq_pin handled by this Instance. */
    sns_interrupt_info     irq_info;

    /** Information for IBI handled by this instance */
    sns_ibi_info           ibi_info;
  };

  union {
    /** Current client config of this Instance. */
    sns_interrupt_req     irq_client_req;
    sns_ibi_req           ibi_client_req;
  };

  /** Event Service handle. This is acquired during interrupt
   *  registration. */
  sns_event_service     *event_service;

  /** Thread unsafe buffer that should only be written to from
   * IST context and is read only everywhere else
   */
  read_only_outside_IST read_only_buffer;
  uint32_t events_processed;

  sns_signal_thread *sig_thread;
  uint32_t sig_flag;

  sns_irq_config irq_config;
} sns_interrupt_instance_state;

/**
 * See sns_handle_signal_event
 */
void sns_irq_gen_event(sns_sensor *sensor, uint32_t signal, void const* instance);
