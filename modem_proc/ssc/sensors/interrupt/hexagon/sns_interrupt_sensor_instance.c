/**
 * @file sns_interrupt_sensor_instance.c
 *
 * The Interrupt virtual Sensor Instance implementation
 *
 * Copyright (c) 2016-2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/5.1/sensors/interrupt/hexagon/sns_interrupt_sensor_instance.c#3 $
 * $DateTime: 2019/09/27 03:18:37 $
 * $Change: 20732958 $
 *
 **/
#include <stdio.h>

#include "sns_gpio_service_internal.h"
#include "sns_interrupt_sensor.h"
#include "sns_interrupt_sensor_instance.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_list.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_printf.h"
#include "sns_profiler.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_signal.h"
#include "sns_stream_service.h"
#include "sns_time.h"
#include "sns_types.h"

#include "DALDeviceId.h"
#include "DDITlmm.h"
#include "DDIGPIOInt.h"
#include "sns_osa_thread.h"


#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_pb_util.h"
#include "sns_interrupt.pb.h"
#include "sns_std.pb.h"

#ifndef SSC_TARGET_NO_DIRECT_GPIO_INT_SUPPORT
#include "GPIOInt.h"
#include "Tlmm.h"
#endif

#ifndef SSC_TARGET_NO_DIRECT_GPIO_INT_SUPPORT
static const uGPIOIntTriggerType dal_trigger_map[_sns_interrupt_trigger_type_ARRAYSIZE] =
                                                   {
                                                     UGPIOINT_TRIGGER_RISING,
                                                     UGPIOINT_TRIGGER_FALLING,
                                                     UGPIOINT_TRIGGER_DUAL_EDGE,
                                                     UGPIOINT_TRIGGER_HIGH,
                                                     UGPIOINT_TRIGGER_LOW,
                                                   };
/**
 * Return trigger type value from uGPIOIntTriggerType enum.
 *
 * @param[i] trigger  sns_irq_trigger_type type
 *
 * @return uGPIOIntTriggerType
 */
static uGPIOIntTriggerType get_dal_trigger_type(sns_interrupt_trigger_type trigger)
{
  /** uTLMM and dalTLMM trigger type enum values are identical. */
   return dal_trigger_map[trigger];
}

#else
DalDeviceHandle *g_gpio_int_handle = NULL;

static const GPIOIntTriggerType dal_trigger_map[_sns_interrupt_trigger_type_ARRAYSIZE] =
                                                   {
                                                     GPIOINT_TRIGGER_RISING,
                                                     GPIOINT_TRIGGER_FALLING,
                                                     GPIOINT_TRIGGER_DUAL_EDGE,
                                                     GPIOINT_TRIGGER_HIGH,
                                                     GPIOINT_TRIGGER_LOW,
                                                   };

/**
 * Return trigger type value from GPIOIntTriggerType enum.
 *
 * @param[i] trigger  sns_irq_trigger_type type
 *
 * @return GPIOIntTriggerType
 */
static GPIOIntTriggerType get_dal_trigger_type(sns_interrupt_trigger_type trigger)
{
  /** uTLMM and dalTLMM trigger type enum values are identical. */
   return dal_trigger_map[trigger];
}

#endif


/**
 * Generate and send an sns_interrupt_event for an interrupt
 */
SNS_SECTION(".text.sns") static void gen_irq_events(sns_sensor_instance *const this)
{
  SNS_PROFILE(SNS_INTERRUPT_GENERATE_EVENT_START, 0);
  sns_interrupt_instance_state *state =
    (sns_interrupt_instance_state*)this->state->state;

  for(;state->events_processed
    < state->read_only_buffer.events_occured;
    state->events_processed++)
  {

    sns_interrupt_event irq_event = sns_interrupt_event_init_default;
    uint32_t index = state->events_processed % SNS_INTERRUPTS_CIRCULAR_BUFFER_DEPTH;
    state->irq_info.timestamp =
      state->read_only_buffer.circular_buffer[index].timestamp;
    if(!state->irq_info.is_registered)
    {
       state->irq_info.warning_cnt++;
       SNS_INST_PRINTF(LOW, this, "Unregistered IRQ %d fired count %d",
                  state->irq_info.irq_num,
                  state->irq_info.warning_cnt++);
    }

    // PEND: Publish the timestamp status so that the clients can handle corner
    //       cases instead of the interrupt sensor having to make all of the
    //       hard decisions.
    if(state->read_only_buffer.circular_buffer[index].ts_status != SNS_INTERRUPT_TIMESTAMP_VALID)
    {
      // Overflow happens if the timestamp is updated in the middle of reading
      // it. That could mean a spurious interrupt happened or the interrupts
      // are coming in too fast for us to handle them in a timely manner
      if(state->read_only_buffer.circular_buffer[index].ts_status == SNS_INTERRUPT_TIMESTAMP_VALID_OVERFLOW)
      {
        SNS_INST_PRINTF(HIGH, this, "Timestamp 0x%x%08x overflowed",
            (uint32_t)(state->irq_info.timestamp >> 32),
            (uint32_t)(state->irq_info.timestamp));
      }
      else
      {
        // If the timestamp has an invalid status, just print out a warning
        SNS_INST_PRINTF(HIGH, this, "Timestamp 0x%x%08x has invalid status 0x%x",
            (uint32_t)(state->irq_info.timestamp >> 32),
            (uint32_t)(state->irq_info.timestamp),
            state->read_only_buffer.circular_buffer[index].ts_status);
      }
    }

    irq_event.interrupt_num = state->irq_info.irq_num;
    irq_event.timestamp = state->irq_info.timestamp;

    SNS_PROFILE(SNS_DDF_SIGNAL_POST, 3, sns_osa_thread_get_thread_id(), irq_event.interrupt_num, (uint32_t)irq_event.timestamp);

    if(!pb_send_event(this, sns_interrupt_event_fields, &irq_event,
                  sns_get_system_time(), SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT,
                  NULL))
    {
       SNS_INST_PRINTF(ERROR, this, "Failed to send Interrupt event for irq %d",
                  state->irq_info.irq_num);
    }

  }
  SNS_PROFILE(SNS_INTERRUPT_GENERATE_EVENT_END, 0);
}

/**
 * After a Physical sensor driver registers the interrupt,
 * Interrupt Sensor Instance registers this call back with
 * uGPIOInt. ISR calls sns_ddf_smgr_notify_irq(). Do minimal
 * operation in this function because it is in IST
 * context.
 *
 * @param param   context provided to uGPIOInt. It is pointer to
 *                the Sensor Instance in this case.
 *
 */
SNS_SECTION(".text.sns") void sns_interrupt_sensor_notify(uint32_t param)
{
  // PEND: In Napali, only 5 interrupts can utilize the HW timestamping
  //       unit at a time. So we will continue to grab the system time
  //       here to support the other interrupts. Once the HW timestamping
  //       unit can support all interrupts at the same time, then we can
  //       remove this.
  sns_time sig_timestamp = sns_get_system_time();
  sns_interrupt_instance_state *state;
  uint32_t index;
  sns_sensor_instance * this = (sns_sensor_instance *) param;
  sns_interrupt_timestamp_status ts_status = SNS_INTERRUPT_TIMESTAMP_VALID;

  if(NULL == this)
  {
    // If input parameter is NULL then the instance is most likely destroyed.
    return;
  }

  if(!sns_island_is_island_ptr((intptr_t)this))
  {
    SNS_ISLAND_EXIT();
  }

  state = (sns_interrupt_instance_state*)this->state->state;

  SNS_PROFILE(SNS_INTERRUPT_CALLBACK_START, 0);
  #ifdef SSC_TARGET_NO_DIRECT_GPIO_INT_SUPPORT
  if((state->irq_info.trigger_type == GPIOINT_TRIGGER_HIGH) ||
    (state->irq_info.trigger_type == GPIOINT_TRIGGER_LOW))
  {
    sns_gpio_interrupt_disable(g_gpio_int_handle, state->irq_info.irq_num);
  }  
  #endif
  // If this interrupt uses the hardware timestamping unit, then go out and
  // read the timestamp.
  if (state->irq_info.uses_hw_timestamp_unit)
  {
    sns_rc ret;
    sns_time read_hw_timestamp;
    ret = read_timestamp(this, state->irq_info.irq_num, &read_hw_timestamp, &ts_status);
    // If the timestamp was successfully read, then use it. Otherwise keep
    // using the timestamp that was read at the beginning of this function.
    if (SNS_RC_SUCCESS == ret)
    {
      sig_timestamp = read_hw_timestamp;
//      SNS_INST_PRINTF(HIGH, this, "read_timestamp succeeded, timestamp %u", (uint32_t)sig_timestamp);
    }
//    else
//    {
//      SNS_INST_PRINTF(ERROR, this, "read_timestamp failed %u", ret);
//    }
  }

  index = state->read_only_buffer.events_occured % SNS_INTERRUPTS_CIRCULAR_BUFFER_DEPTH;
  state->read_only_buffer.circular_buffer[index].timestamp = sig_timestamp;
  state->read_only_buffer.circular_buffer[index].ts_status = ts_status;
  state->read_only_buffer.events_occured += 1;
  SNS_PROFILE(SNS_INTERRUPT_SET_SIGNAL, 2, state->irq_info.irq_num, state->sig_flag);
  sns_osa_thread_sigs_set(state->sig_thread->thread, state->sig_flag);

  SNS_PROFILE(SNS_INTERRUPT_CALLBACK_END, 0);
}

/**
 * Checks whether current INT line need to set INACTIVE CFG
 *
 * @param[i] state      current instance state
 *
 * @return
 * True if inactive_cfg is to be set for this INT line, otherwise FALSE
 */
static bool is_inactive_cfg_required(sns_interrupt_instance_state *const state)
{
  int irq_line_index = 0;

  for(irq_line_index = 0 ;
      irq_line_index < state->irq_config.inact_gpio_count &&
      state->irq_config.inact_gpio_count < MAX_INACTIVE_GPIOs;
      irq_line_index++)
  {
    if(state->irq_config.inact_gpio_list[irq_line_index] ==
      state->irq_info.irq_num)
    {
      SNS_PRINTF(HIGH, sns_fw_printf, "INACTIVE Mode CFG enabled for GPIO Pin = %d",
                 state->irq_info.irq_num);
      return true;
    }

  }
  return false;
}

#ifdef SSC_TARGET_NO_DIRECT_GPIO_INT_SUPPORT

/**
 * Register interrupt signal with GPIOInt
 *
 * @param[i] this      current instance
 *
 * @return
 * SNS_RC_SUCCESS if registration is successful, otherwise SNS_RC_FAILURE
 */
static sns_rc sns_GPIO_int_register(sns_sensor_instance *const this)
{
  DALResult err;
  sns_rc rc = SNS_RC_SUCCESS;
  sns_interrupt_instance_state* state =
    (sns_interrupt_instance_state*)this->state->state;

  if (g_gpio_int_handle == NULL)
  {
    err = DAL_DeviceAttach(DALDEVICEID_GPIOINT, &g_gpio_int_handle);
    if (err != DAL_SUCCESS)
    {
	   return SNS_RC_FAILED;
	}
  }
  err = GPIOInt_RegisterIsr(g_gpio_int_handle, state->irq_info.irq_num,
                            (GPIOIntTriggerType)state->irq_info.trigger_type,
                            (GPIOINTISR)sns_interrupt_sensor_notify,
							(GPIOINTISRCtx)this);
  if (err != DAL_SUCCESS)
  {
    rc = SNS_RC_FAILED;
  }
  return rc;
}
/**
 * Deregister interrupt signal with GPIOInt
 *
 * @param[i] state      current instance state
 *
 * @return
 * SNS_RC_SUCCESS if deregistration is successful, otherwise SNS_RC_FAILURE
 */
static sns_rc sns_GPIO_int_deregister(sns_interrupt_instance_state* state)
{

  DALResult err;
  sns_rc rc = SNS_RC_SUCCESS;

  if (g_gpio_int_handle == NULL)
  {
    return  SNS_RC_FAILED;
  }
  err = GPIOInt_DeregisterIsr(g_gpio_int_handle, state->irq_info.irq_num,
                              (GPIOINTISR)sns_interrupt_sensor_notify);
  if (err != DAL_SUCCESS)
  {
    rc = SNS_RC_FAILED;
  }
  return rc;
}

#else /* SSC_TARGET_NO_DIRECT_GPIO_INT_SUPPORT */

/**
 * Checks whether current INT line need to use hw or sw Timestamp
 *
 * @param[i] state      current instance state
 *
 * @return
 * True if hw_ts is going to use for this INT line, otherwise FALSE
 */
static bool is_hw_ts_required(sns_interrupt_instance_state *const state)
{
  int irq_line_index = 0;

  for(irq_line_index = 0 ;
      irq_line_index < state->irq_config.sw_ts_irq_len &&
      state->irq_config.sw_ts_irq_len < MAX_SW_IRQ_LINES;
      irq_line_index++)
  {
    if(state->irq_config.sw_ts_irq_lines[irq_line_index] == 
      state->irq_info.irq_num)
    {
      SNS_PRINTF(HIGH, sns_fw_printf, "HW TS disabled for GPIO Pin = %d",
                 state->irq_info.irq_num);
      return false;
    }
  
  }
  return true;
}


/**
 * Register interrupt signal with uGPIOInt
 *
 * @param[i] this      current instance
 *
 * @return
 * SNS_RC_SUCCESS if registration is successful, otherwise SNS_RC_FAILURE
 */
static sns_rc sns_GPIO_int_register(sns_sensor_instance *const this)
{
  int32 err;
  sns_rc rc = SNS_RC_SUCCESS;
  uint32_t flags = 0;
  sns_interrupt_instance_state* state =
    (sns_interrupt_instance_state*)this->state->state;

  flags |= sns_interrupt_sensor_get_island_flag();
  if((state->irq_info.trigger_type == UGPIOINT_TRIGGER_HIGH) ||
     (state->irq_info.trigger_type == UGPIOINT_TRIGGER_LOW))
  {
      flags |= sns_interrupt_sensor_get_level_trigger_flag();
  }

  state->irq_info.uses_hw_timestamp_unit = is_hw_ts_required(state);

  if (state->irq_info.uses_hw_timestamp_unit)
  {
    flags |= sns_interrupt_sensor_get_hw_timestamp_flag();
  }

  err = uGPIOInt_RegisterInterrupt(state->irq_info.irq_num,
                                  (uGPIOIntTriggerType)state->irq_info.trigger_type,
                                  (uGPIOINTISR)sns_interrupt_sensor_notify,
                                  (uGPIOINTISRCtx)this,
                                  flags);
  if (err != UGPIOINT_SUCCESS)
  {
    rc = SNS_RC_FAILED;
  }
  return rc;
}
/**
 * Deregister interrupt signal with uGPIOInt
 *
 * @param[i] state      current instance state
 *
 * @return
 * SNS_RC_SUCCESS if deregistration is successful, otherwise SNS_RC_FAILURE
 */

static sns_rc sns_GPIO_int_deregister(sns_interrupt_instance_state* state)
{

  DALResult err;
  sns_rc rc = SNS_RC_SUCCESS;

  err = uGPIOInt_DeregisterInterrupt(state->irq_info.irq_num);
  if (err != UGPIOINT_SUCCESS)
  {
    rc = SNS_RC_FAILED;
  }
  return rc;
}

#endif /* SSC_TARGET_NO_DIRECT_GPIO_INT_SUPPORT */

/**
 * Register the interrupt signal with uGPIOInt or GPIOInt.

 * @param[i] this    current instance
 * @param[i] req     IRQ config request
 *
 * @return
 * SNS_RC_INVALID_VALUE - invalid request
 * SNS_RC_SUCCESS
 */
static sns_rc register_interrupt(sns_sensor_instance *const this,
                                 sns_request const *client_request)
{
  sns_interrupt_req const *new_req = (sns_interrupt_req*)client_request->request;
  sns_rc err;
  sns_interrupt_instance_state* state =
    (sns_interrupt_instance_state*)this->state->state;
  bool irq_is_enabled = false, set_inactive_cfg = false;
  sns_service_manager *service_manager = this->cb->get_service_manager(this);

  if(state->irq_info.is_registered)
  {
    /* If the irq_pin is registered then proceed only if
       the incoming request is diffrent from current config.*/
    if(0 == sns_memcmp(new_req, &state->irq_client_req, sizeof(*new_req)))
    {
       /* new_req is same as current config. No-op.*/
       return SNS_RC_SUCCESS;
    }
  }

  state->event_service =
     (sns_event_service*)service_manager->get_service(service_manager,
                                                      SNS_EVENT_SERVICE);
  state->irq_info.irq_num = new_req->interrupt_num;
  state->irq_info.warning_cnt = 0;
  state->irq_info.timestamp = 0;
  state->irq_info.trigger_type = get_dal_trigger_type(new_req->interrupt_trigger_type);
  state->irq_info.pin_tlmm_info.is_chip_pin = new_req->is_chip_pin;

  set_inactive_cfg = is_inactive_cfg_required(state);
  irq_is_enabled = sns_gpio_service_update_irq_pin(new_req, true, set_inactive_cfg);
  if(!irq_is_enabled)
  {
    return SNS_RC_INVALID_VALUE;
  }

  err = sns_GPIO_int_register(this);
  if(err == SNS_RC_SUCCESS)
  {
      /* Copy the new irq client config in private state of this Instance.*/
      sns_memscpy(&state->irq_client_req,
                  sizeof(state->irq_client_req),
                  new_req,
                  sizeof(*new_req));

     if(!state->irq_info.is_registered)
     {
       state->irq_info.is_registered = true;
     }
  }
  else
  {
    sns_gpio_service_update_irq_pin(new_req, false, false);
    SNS_INST_PRINTF(ERROR, this, "Failed to register gpio err = %d pin = %d", err,
               state->irq_info.irq_num);
    return SNS_RC_INVALID_VALUE;
  }

  return SNS_RC_SUCCESS;
}

/**
 * De-Register the interrupt signal with uGPIOInt or GPIOInt.

 * @param[i] this    current instance
 *
 * @return
 * SNS_RC_INVALID_VALUE - invalid request
 * SNS_RC_SUCCESS
 */
static sns_rc deregister_interrupt(sns_sensor_instance *const this)
{
  sns_rc err;
  sns_interrupt_instance_state* state =
    (sns_interrupt_instance_state*)this->state->state;

  if(state->irq_info.is_registered)
  {
    err = sns_GPIO_int_deregister(state);
    if (err == SNS_RC_SUCCESS)
    {
      state->irq_info.is_registered = false;
      sns_gpio_service_update_irq_pin(&state->irq_client_req, false, false);
    }
    else
    {
      SNS_INST_PRINTF(ERROR, this, "Failed to de-register gpio err = %d pin = %d", err,
               state->irq_info.irq_num);
      return SNS_RC_INVALID_VALUE;
    }
  }
  return SNS_RC_SUCCESS;
}

/** See sns_sensor_instance_api::notify_event */
SNS_SECTION(".text.sns") static sns_rc sns_irq_inst_notify_event(sns_sensor_instance *const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

/** See sns_sensor_instance_api::init */
static sns_rc sns_irq_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sensor_state)
{
  sns_rc rc = SNS_RC_SUCCESS;
  interrupt_state *sstate = (interrupt_state*)sensor_state->state;
  sns_interrupt_instance_state *state =
    (sns_interrupt_instance_state*)this->state->state;

  state->sig_thread = sstate->signal_thread;

  sns_memscpy(&state->irq_config,sizeof(state->irq_config),
    &sstate->config,sizeof(sstate->config));

  return rc;
}
/* Non-island interrupt req handling */
static sns_rc __attribute__ ((noinline))
sns_handle_interrupt_req(sns_sensor_instance *const this,
                                             sns_request const *client_request)
{
  sns_interrupt_instance_state *state =
    (sns_interrupt_instance_state*)this->state->state;
  sns_rc ret_val  = SNS_RC_SUCCESS;
    state->event_fptr = gen_irq_events;
    ret_val = sns_signal_register_signal(state->sig_thread, this, &state->sig_flag);
    if (SNS_RC_SUCCESS == ret_val)
    {
      state->events_processed = 0;
      state->read_only_buffer.events_occured = 0;
	  
      ret_val = register_interrupt(this, client_request);

      if(SNS_RC_SUCCESS != ret_val ||
         !pb_send_event(this, NULL, NULL,sns_get_system_time(),
                        SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT, NULL))
      {
        SNS_INST_PRINTF(ERROR, this, "Failed to register irq %d",
                        state->irq_info.irq_num);
        ret_val = SNS_RC_INVALID_STATE;
      }
    }
    else
    {
      ret_val = SNS_RC_POLICY;
    }
	return ret_val;
}

/** See sns_sensor_instance_api::set_client_config */
SNS_SECTION(".text.sns") static sns_rc sns_irq_inst_set_client_config(sns_sensor_instance *const this,
                                             sns_request const *client_request)
{
  sns_interrupt_instance_state *state =
    (sns_interrupt_instance_state*)this->state->state;
  sns_rc ret_val  = SNS_RC_SUCCESS;
  if(client_request->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ)
  {
    SNS_ISLAND_EXIT();
    ret_val = sns_handle_interrupt_req(this,client_request);
  }
  else if(client_request->message_id == SNS_INTERRUPT_MSGID_SNS_IBI_REQ)
  {
    ret_val = sns_irq_inst_process_ibi_req(this, client_request);
  }
  else if(client_request->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_IS_CLEARED)
  {
    send_interrupt_done(this, state->irq_info.irq_num);
  }
  else
  {
    SNS_INST_PRINTF(ERROR, this, "Unsupported message ID %d",
                    client_request->message_id);
  }
  return ret_val;
}

static sns_rc sns_irq_inst_deinit(sns_sensor_instance *const this)
{
  sns_interrupt_instance_state *state =
    (sns_interrupt_instance_state*)this->state->state;

  if(gen_irq_events == state->event_fptr)
  {
    deregister_interrupt(this);
  }
  else if(gen_irq_events == state->event_fptr)
  {
    deregister_ibi(this);
  }
  sns_signal_unregister_signal(state->sig_thread, state->sig_flag);
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") void sns_irq_gen_event(sns_sensor *sensor, uint32_t signal, void const* instance)
{
  UNUSED_VAR(sensor);
  UNUSED_VAR(signal);
  sns_sensor_instance *this = (sns_sensor_instance *)instance;
  sns_interrupt_instance_state *state =
    (sns_interrupt_instance_state*)this->state->state;
  state->event_fptr(this);
}

/** Public Data Definitions. */

sns_sensor_instance_api interrupt_sensor_instance_api SNS_SECTION(".data.sns") =
{
  .struct_len        = sizeof(sns_sensor_instance_api),
  .init              = &sns_irq_inst_init,
  .deinit            = &sns_irq_inst_deinit,
  .set_client_config = &sns_irq_inst_set_client_config,
  .notify_event      = &sns_irq_inst_notify_event
};
