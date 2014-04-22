/**
 * sns_interrupt_sensor_configure.c
 *
 * Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "stdint.h"
#include "sns_sensor_instance.h"
#include "sns_request.h"
#include "sns_interrupt.pb.h"
#include "sns_island.h"
#include "sns_types.h"
#include "sns_time.h"
#include "sns_interrupt_sensor_configure.h"
#include "sns_interrupt_sensor_instance.h"

extern DalDeviceHandle *g_gpio_int_handle;
/**
 * Set flag to register sensors interrupts for island mode operation
 */
uint32_t sns_interrupt_sensor_get_island_flag(void)
{
  return 0;
}
/**
 * Set flag to register level triggered interrupts
 */
uint32_t sns_interrupt_sensor_get_level_trigger_flag(void)
{
  return 0;
}
/**
 * Get flag to use the hardware timestamping unit
 */
uint32_t sns_interrupt_sensor_get_hw_timestamp_flag(void)
{
  // This feature is NOT supported on this target
  return 0;
}

SNS_SECTION(".text.sns") sns_rc send_interrupt_done(sns_sensor_instance *const this,
                           uint16_t irq_num)
{
  sns_rc rv = SNS_RC_SUCCESS;
  sns_interrupt_instance_state *state =
    (sns_interrupt_instance_state*)this->state->state;
  if(SNS_RC_SUCCESS != sns_gpio_interrupt_enable(g_gpio_int_handle,state->irq_info.irq_num))
  {
    SNS_INST_PRINTF(ERROR, this, "InterruptDone failed");
    rv = SNS_RC_FAILED;
  }
  return rv;
}

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
SNS_SECTION(".text.sns") sns_rc read_timestamp(sns_sensor_instance *const this,
    uint32_t gpio, sns_time* timestamp, sns_interrupt_timestamp_status* ts_status)
{
  UNUSED_VAR(this);
  UNUSED_VAR(gpio);
  UNUSED_VAR(timestamp);
  UNUSED_VAR(ts_status);
  // This feature is NOT supported on this target
  return SNS_RC_FAILED;
}

sns_rc sns_irq_inst_process_ibi_req(sns_sensor_instance *const this,
                                    sns_request const *client_request)
{
  UNUSED_VAR(this);
  UNUSED_VAR(client_request);
  return SNS_RC_INVALID_VALUE;
}

/**
 * Disable interrupt
 */
sns_rc sns_gpio_interrupt_disable(DalDeviceHandle *handle, uint32_t gpio)
{
  DALResult err = DAL_ERROR;
  if(NULL != handle)
  {
    err = GPIOInt_DisableInterrupt(handle,gpio);
  }
  return ((DAL_SUCCESS == err)?SNS_RC_SUCCESS:SNS_RC_FAILED);
}
/**
 * Enable interrupt
 */
sns_rc sns_gpio_interrupt_enable(DalDeviceHandle *handle, uint32_t gpio)
{
  DALResult err = DAL_ERROR;
  if(NULL != handle)
  {
    err = GPIOInt_EnableInterrupt(handle,gpio);
  }
  return ((DAL_SUCCESS == err)?SNS_RC_SUCCESS:SNS_RC_FAILED);
}