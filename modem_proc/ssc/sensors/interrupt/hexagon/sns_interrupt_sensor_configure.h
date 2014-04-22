#pragma once
/**
 * @file sns_interrupt_sensor_configure.h
 *
 * Target-specific interrupt sensor configurations.
 *
 * Copyright (c) 2017 - 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/5.1/sensors/interrupt/hexagon/sns_interrupt_sensor_configure.h#2 $
 * $DateTime: 2019/09/27 03:18:37 $
 * $Change: 20732958 $
 *
 **/

#include "stdint.h"
#include "sns_sensor_instance.h"
#include "sns_request.h"
#include "sns_time.h"
#ifdef SSC_TARGET_NO_DIRECT_GPIO_INT_SUPPORT
#include "DDIGPIOInt.h"
#endif
/**
 * Interrupt timestampt status. This is a mirror of uGPIOIntTimestampStatus in uGPIOInt.h
 */
typedef enum {
  SNS_INTERRUPT_TIMESTAMP_INVALID          = 0x0,  /**< Timestamp is not valid */
  SNS_INTERRUPT_TIMESTAMP_VALID            = 0x1,  /**< Timestamp is valid */
  SNS_INTERRUPT_TIMESTAMP_INVALID_OVERFLOW = 0x10, /**< Timestamp is invalid, and there has been an overflow */
  SNS_INTERRUPT_TIMESTAMP_VALID_OVERFLOW   = 0x11  /**< Timestamp is valid, and there has been an overflow */
} sns_interrupt_timestamp_status;

/**
 * Get flag to register sensors interrupts for island mode operation
 */
uint32_t sns_interrupt_sensor_get_island_flag(void);

/**
 * Get flag to register level triggered interrupts
 */
uint32_t sns_interrupt_sensor_get_level_trigger_flag(void);

/**
 * Get flag to use the hardware timestamping unit
 */
uint32_t sns_interrupt_sensor_get_hw_timestamp_flag(void);

/**
 * Reports to bus driver that interrupt processing is completed
 *
 * @param[i] this        pointer to the sensor instance
 * @param[i] request     the request carrying the interrupt number
 *
 * @return sns_rc        SNS_RC_SUCCESS if successfully communicated with bus driver
 *                       SNS_RC_FAILED otherwise
 */
sns_rc send_interrupt_done(sns_sensor_instance *const this, uint16_t irq_num);

/**
 * Attempt to read the timestamp that was captured by the interrupt's HW
 * timestamping unit.
 *
 * @param[i] this        pointer to the sensor instance, for logging purposes
 * @param[i] gpio        interrupt pin to read the timestamp from
 * @param[o] timestamp   where the timestamp is placed
 * @param[o] ts_status   timestamp status
 *
 * @return sns_rc        SNS_RC_SUCCESS if the timestamp was successfully read,
 *                       SNS_RC_FAILED otherwise
 */
sns_rc read_timestamp(sns_sensor_instance *const this, uint32_t gpio,
    sns_time* timestamp, sns_interrupt_timestamp_status* ts_status);

/**
 * De-Register the IBI

 * @param[i] this    current instance
 *
 * @return
 * SNS_RC_INVALID_VALUE - invalid request
 * SNS_RC_SUCCESS
 */
sns_rc deregister_ibi(sns_sensor_instance *const this);

/**
 * Processes SNS_INTERRUPT_MSGID_SNS_IBI_REQ
 *
 * @param[i] this        pointer to the sensor instance
 * @param[i] request     the request 
 *
 * @return sns_rc        SNS_RC_SUCCESS if the request was processed successfully
 *                       SNS_RC_POLICY otherwise
 */
sns_rc sns_irq_inst_process_ibi_req(sns_sensor_instance *const this, 
                                    sns_request const *request);
#ifdef SSC_TARGET_NO_DIRECT_GPIO_INT_SUPPORT
/**
 * Disable GPIO interrupt
 *
 * @param[i] handle      The client context pointer of the GPIO interrupt controller DAL
 * @param[i] gpio        GPIO number
 *
 * @return sns_rc        SNS_RC_SUCCESS if the request was processed successfully
 *                       SNS_RC_POLICY otherwise
 */

sns_rc sns_gpio_interrupt_disable(DalDeviceHandle *handle, uint32_t gpio);
/**
 * Enable GPIO interrupt
 *
 * @param[i] handle      The client context pointer of the GPIO interrupt controller DAL
 * @param[i] gpio        GPIO number
 *
 * @return sns_rc        SNS_RC_SUCCESS if the request was processed successfully
 *                       SNS_RC_POLICY otherwise
 */
sns_rc sns_gpio_interrupt_enable(DalDeviceHandle *handle, uint32_t gpio);
#endif
