/*
 * Copyright (c) 2019-2020, ams AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include "sns_tmd2725_sensor.h"


/* ------------------------------------------------------------------------------------ */
/* Registry section identifiers */
/* ------------------------------------------------------------------------------------ */
static char *registry_als_fac_cal_names[TMD2725_NUM_DEVICES]           = { "tmd2725_0_platform.als.fac_cal", "tmd2725_1_platform.als.fac_cal"};
static char *registry_als_custom_cal_names[TMD2725_NUM_DEVICES]        = { "tmd2725_0_platform.als.custom_cal", "tmd2725_1_platform.als.custom_cal"};
static char *registry_als_default_config_names[TMD2725_NUM_DEVICES]    = { "tmd2725_0_platform.als.def_config", "tmd2725_1_platform.als.def_config"};
static char *registry_als_cal_target_names[TMD2725_NUM_DEVICES]        = { "tmd2725_0_platform.als.cal_target", "tmd2725_1_platform.als.cal_target"};

static char *registry_prox_fac_cal_names[TMD2725_NUM_DEVICES]          = { "tmd2725_0_platform.prox.fac_cal", "tmd2725_1_platform.prox.fac_cal"};
static char *registry_prox_proxOffset_hWcal_names[TMD2725_NUM_DEVICES] = { "tmd2725_0_platform.prox.proxOffset_hWcal", "tmd2725_1_platform.prox.proxOffset_hWcal"};
static char *registry_prox_default_config_names[TMD2725_NUM_DEVICES]   = { "tmd2725_0_platform.prox.def_config", "tmd2725_1_platform.prox.def_config"};

static char *registry_platform_config_names[TMD2725_NUM_DEVICES]       = { "tmd2725_0_platform.config", "tmd2725_1_platform.config"};
static char *registry_platform_placement_names[TMD2725_NUM_DEVICES]    = { "tmd2725_0_platform.placement", "tmd2725_1_platform.placement"};

static char *registry_als_config_names[TMD2725_NUM_DEVICES]            = { "tmd2725_0.als.config", "tmd2725_1.als.config"};
static char *registry_prox_config_names[TMD2725_NUM_DEVICES]           = { "tmd2725_0.prox.config", "tmd2725_1.prox.config"};


const ams_calibrationData_t default_common_cal_data_tmd2725 = {
  .timeBase_us = AMS_USEC_PER_TICK,
  .adcMaxCount = AMS_ALS_MAX_COUNTS_PER_TICK,
  .alsThresholdHigh = 5,
  .alsThresholdLow = 5,
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
  .alsCalibrationLuxTargetHigh = 100,
  .alsCalibrationLuxTargetLow = 100,
  .alsCalibrationLuxTarget = CONFIG_ALS_CAL_TARGET,
  .alsCalibrationLuxTargetError = CONFIG_ALS_CAL_TARGET_TOLERANCE,
  .alsCalibrationFactor = 1123,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  .proxAdaptiveThresholdFactor = 100,
  .proxAdaptiveThreshold       = 100,
  .proxHardThreshold           = AMS_PRX_DEFAULT_NEARBY_HARD_THRESHOLD,
  .proxMinThreshold            = AMS_PRX_DEFAULT_MIN_THRESHOLD,
  .proxHardThresholdResetValue = AMS_PRX_DEFAULT_NEARBY_HARD_THRESHOLD,
  .proxMinThresholdResetValue  = AMS_PRX_DEFAULT_MIN_THRESHOLD,
#endif
  .D_factor = AMS_ALS_D_FACTOR,
  .L0_factor = AMS_ALS_L0_FACTOR,
  .L1_factor = AMS_ALS_L1_FACTOR,
  .L2_factor = AMS_ALS_L2_FACTOR,
  .L3_factor = AMS_ALS_L3_FACTOR
};

const ams_default_als_config_data_t default_als_config_data = {
    .aTime = AMS_ALS_ATIME,
    .wTime = AMS_DEFAULT_WAIT_TIME,
    .wLong = true,
    .aGain = AMS_ALS_AGAIN,
    .aPers = AMS_ALS_PERS,
    .aUpperThrs = AMS_ALS_THRESHOLD_HIGH,
    .aLowerThrs = AMS_ALS_THRESHOLD_LOW,
    .aThrsWidth = AMS_ALS_THRESHOLD_HALF_WIDTH
};

const ams_default_prox_config_data_t default_prox_config_data = {
    .pTime = AMS_PRX_DEFAULT_PTIME,
    .pwTime_us = AMS_DEFAULT_WAIT_TIME,
    .pPulseLen = AMS_PRX_DEFAULT_PULSE_LENGTH,
    .pPulse = AMS_PRX_DEFAULT_PULSE_COUNT,
    .pGain = AMS_PRX_DEFAULT_GAIN,
    .pLdrive = AMS_PRX_DEFAULT_LED_CURRENT,
    .pPers = AMS_PRX_DEFAULT_PERS,
    .pUpperThrs = AMS_PRX_DEFAULT_NEARBY_HARD_THRESHOLD,
    .pLowerThrs = AMS_PRX_DEFAULT_MIN_THRESHOLD,
    .pOffsetSign = AMS_PRX_POFFSET_SIGN,
    .proxOffset = AMS_PRX_POFFSET_VALUE,
    .pBinSearch = AMS_PRX_DEFAULT_BIN_SEARCH
};

static void process_cal_reset(sns_sensor *const this, sns_sensor_instance *const instance);

static void tmd2725_send_fifo_flush_done(sns_sensor_instance *const instance,
                              sns_sensor_uid *suid)
{
  sns_service_manager *mgr = instance->cb->get_service_manager(instance);
  sns_event_service *e_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
  sns_sensor_event *event = e_service->api->alloc_event(e_service, instance, 0);

  if(NULL != event)
  {
    event->message_id = SNS_STD_MSGID_SNS_STD_FLUSH_EVENT;
    event->event_len = 0;
    event->timestamp = sns_get_system_time();

    e_service->api->publish_event(e_service, instance, event, suid);
  }
}

/**
 * Returns decoded request message for type
 * sns_std_sensor_config.
 *
 * @param[in] in_request   Request as sotred in client_requests
 *                         list.
 * @param decoded_request  Standard decoded message.
 * @param decoded_payload  Decoded stream request payload.
 *
 * @return bool true if decode is successful else false
 */
static bool ams_get_decoded_std_request(sns_sensor const *this, sns_request const *in_request,
                                            sns_std_request *decoded_request,
                                            sns_std_sensor_config *decoded_payload)
{
  pb_istream_t stream;
  pb_simple_cb_arg arg = { .decoded_struct = decoded_payload,
                           .fields = sns_std_sensor_config_fields };

  decoded_request->payload = (struct pb_callback_s) { .funcs.decode = &pb_decode_simple_cb,
                                                      .arg = &arg };

  stream = pb_istream_from_buffer(in_request->request, in_request->request_len);

  if(!pb_decode(&stream, sns_std_request_fields, decoded_request))
  {
    SNS_PRINTF(ERROR, this, "AMS: pb_decode error in ams_get_decoded_std_request");
    return false;
  }
  return true;
}

static void ams_get_device_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   ams_sensor_type sensor_type,
                                   float *chosen_sample_rate,
                                   float *chosen_report_rate,
                                   bool *client_present,
                                   int32_t *num_clients)
{
  sns_sensor_uid suid;
  sns_request const *request;
  ams_state *state = (ams_state*)this->state->state;
  sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid){{0}}), sizeof(sns_sensor_uid));
  sns_sensor_uid * p_suid;
  int32_t hwid = this->cb->get_registration_index(this);

  *num_clients = 0;
  *client_present = false;


  if ((sensor_type == AMS_ALS_OC)   ||
      (sensor_type == AMS_ALS_STRM) ||
      (sensor_type == AMS_PROX_OC)   ||
      (sensor_type == AMS_PROX_STRM))
  {
      p_suid = (sns_sensor_uid *)tmd2725_get_suid_with_sensortype_and_hwid(sensor_type, hwid);
      sns_memscpy(&suid, sizeof(suid), p_suid, sizeof(sns_sensor_uid));
  }
  else
  {
    return;
  }

  if (instance->cb->get_client_request(instance, &suid, true) != NULL)
  {
    *chosen_report_rate = 0;
    *chosen_sample_rate = 0;
  }
  /** Parse through existing requests and get fastest sample
   *  rate and report rate requests. */
  for(request = instance->cb->get_client_request(instance, &suid, true);
      NULL != request;
      request = instance->cb->get_client_request(instance, &suid, false))
  {
    sns_std_request decoded_request;
    sns_std_sensor_config decoded_payload = {0};

    AMS_PRINTF(MED, this, "ams_get_device_config: this %p, instance %p, sensor_type %d, msgId=%d"
                      , this
                      , instance
                      , sensor_type
                      , request->message_id
                      );

    if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
    {
      if(ams_get_decoded_std_request(this, request, &decoded_request, &decoded_payload))
      {
        AMS_PRINTF(MED, this, "AMS: ams_get_device_config: sensor_type %d, curr SR %d, recvd SR %d",
                                sensor_type,
                                (int32_t)*chosen_sample_rate,
                                (int32_t)decoded_payload.sample_rate);
        *chosen_sample_rate = SNS_MAX(*chosen_sample_rate,
                                       decoded_payload.sample_rate);
        *chosen_report_rate = *chosen_sample_rate;
        *client_present = true;
        (*num_clients)++;
      }
    }
    else if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG)
    {
      AMS_PRINTF(MED, this, "AMS: ams_get_device_config: sensor_type %d (ON_CHANGE)",
                              sensor_type
                              );
      *chosen_sample_rate = 0;
      *chosen_report_rate = 0;
      *client_present = true;
      (*num_clients)++;
    }
    else if(request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
    {
      AMS_PRINTF(MED, this, "AMS: ams_get_device_config: sensor_type %d (SELF TEST)",
                              sensor_type
                              );
      *chosen_sample_rate = state->odr_for_selftest;
      *chosen_report_rate = state->odr_for_selftest;
      *client_present = true;
    }
  }
}

static void ams_set_inst_config(sns_sensor *this,
                                    sns_sensor_instance *instance,
                                    float chosen_report_rate,
                                    float chosen_sample_rate,
                                    uint32_t message_id)
{
  sns_std_req new_client_config;
  sns_request config;

  new_client_config.desired_report_rate = chosen_report_rate;
  new_client_config.desired_sample_rate = chosen_sample_rate;

  config.message_id = message_id;
  config.request_len = sizeof(sns_std_req);
  config.request = &new_client_config;

  this->instance_api->set_client_config(instance, &config);
}

static void ams_turn_rails_off(sns_sensor *this)
{
  sns_sensor *sensor;

  for(sensor = this->cb->get_library_sensor(this, true);
      NULL != sensor;
      sensor = this->cb->get_library_sensor(this, false))
  {
    ams_state *sensor_state = (ams_state*)sensor->state->state;
    if(sensor_state->rail_config.rail_vote != SNS_RAIL_OFF)
    {
      sns_time timeout = sns_convert_ns_to_ticks(AMS_POWER_RAIL_OFF_TIMEOUT_NS);
      tmd2725_start_power_rail_timer( sensor, timeout, AMS_POWER_RAIL_PENDING_OFF );
    }
  }
}

void tmd2725_start_power_rail_timer(sns_sensor *const this,
                                sns_time timeout_ticks,
                                ams_power_rail_pending_state pwr_rail_pend_state)
{
  ams_state *state = (ams_state*)this->state->state;

  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  size_t req_len;
  uint8_t buffer[20];
  sns_memset(buffer, 0, sizeof(buffer));
  req_payload.is_periodic = false;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = timeout_ticks;

  req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                              sns_timer_sensor_config_fields, NULL);
  if(req_len > 0)
  {
    sns_request timer_req =
      {  .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
         .request = buffer, .request_len = req_len  };
    state->timer_stream->api->send_request(state->timer_stream, &timer_req);
    state->power_rail_pend_state = pwr_rail_pend_state;
    AMS_DEV_PRINTF(MED, this, "AMS: tmd2725_start_power_rail_timer enter: timer_stream %p",\
                        state->timer_stream);
  }
  else
  {
    AMS_PRINTF(HIGH, this, "AMS timer req encode error");
  }
}

/**
 * Publish attributes read from registry
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void
ams_publish_registry_attributes(sns_sensor *const this)
{
  int32_t hwid = this->cb->get_registration_index(this);
  ams_state *state = (ams_state*)this->state->state;
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = state->supports_sync_stream;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_STREAM_SYNC, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = hwid;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_HW_ID, &value, 1, false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR,
      SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
    for(uint8_t i = 0; i < 12; i++)
    {
      values[i].has_flt = true;
      values[i].flt = state->placement[i];
    }
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PLACEMENT,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->registry_pf_cfg.rigid_body_type;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1, true);
  }
}

void tmd2725_reval_instance_config(sns_sensor *this,
                              sns_sensor_instance *instance,
                              ams_sensor_type sensor_type)
{
  /**
   * 1. Get best ALS Config.
   * 2. Get best Proximity Config.
   * 3. Decide best Instance Config based on above outputs.
   */
  UNUSED_VAR(sensor_type);
  float chosen_sample_rate = 0;
  float chosen_report_rate = 0;
  float sample_rate = 0;
  float report_rate = 0;
  bool client_present = false;
  int32_t num_clients;

  ams_state *state = (ams_state*)this->state->state;
  ams_instance_state *inst_state = (ams_instance_state*)instance->state->state;
  inst_state->publish_sensors_new = 0;

  SNS_PRINTF(MED, this, "ams_reval_instance_config: this %p, instance %p, sensor_type %d", this,
                          instance,
                          state->sensor_type);
  SNS_PRINTF(MED, this, "ams_reval_instance_config enter: publish_sensors=0x%04x, publish_sensors_prev=0x%04x",
               inst_state->publish_sensors, inst_state->publish_sensors_prev );

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
    ams_get_device_config(this,
                           instance,
                           AMS_ALS_STRM,
                           &sample_rate,
                           &report_rate,
                           &client_present,
                           &num_clients);

    SNS_PRINTF(MED, this, "ams_reval_instance_config: AMS_ALS_STRM client_present %d, curr SR %d, recvd SR %d",
                            client_present,
                            (int32_t)sample_rate,
                            (int32_t)chosen_sample_rate);
    if(client_present)
    {
      chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
      chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);
      inst_state->publish_sensors |= AMS_ALS_STRM;
      inst_state->publish_sensors_new |= AMS_ALS_STRM;
    }
    else
    {
      inst_state->publish_sensors &= ~(AMS_ALS_STRM);
    }

    ams_get_device_config(this,
                           instance,
                           AMS_ALS_OC,
                           &sample_rate,
                           &report_rate,
                           &client_present,
                           &num_clients);
    SNS_PRINTF(MED, this, "ams_reval_instance_config: AMS_ALS_OC client_present %d, curr SR %d, recvd SR %d",
                            client_present,
                            (int32_t)sample_rate,
                            (int32_t)chosen_sample_rate);
    if (num_clients > inst_state->als_info.client_cfg.num_oc_clients_prev
        &&
        num_clients > 1)
    {
      inst_state->als_info.client_cfg.oc_send_last_event = true;
    }
    else
    {
      inst_state->als_info.client_cfg.oc_send_last_event = false;
    }
    inst_state->als_info.client_cfg.num_oc_clients_prev = num_clients;

    if(client_present)
    {
      chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
      chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);
      inst_state->publish_sensors |= AMS_ALS_OC;
      inst_state->publish_sensors_new |= AMS_ALS_OC;
    }
    else
    {
      inst_state->publish_sensors &= ~(AMS_ALS_OC);
    }
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    ams_get_device_config(this,
                           instance,
                           AMS_PROX_STRM,
                           &sample_rate,
                           &report_rate,
                           &client_present,
                           &num_clients);
    SNS_PRINTF(MED, this, "ams_reval_instance_config: AMS_PROX_STRM client_present %d, curr SR %d, recvd SR %d",
                            client_present,
                            (int32_t)chosen_sample_rate,
                            (int32_t)sample_rate);

    if(client_present)
    {
      chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
      chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);
      inst_state->publish_sensors |= AMS_PROX_STRM;
      inst_state->publish_sensors_new |= AMS_PROX_STRM;
    }
    else
    {
      inst_state->publish_sensors &= ~(AMS_PROX_STRM);
    }

    ams_get_device_config(this,
                           instance,
                           AMS_PROX_OC,
                           &sample_rate,
                           &report_rate,
                           &client_present,
                           &num_clients);
    SNS_PRINTF(MED, this, "ams_reval_instance_config: AMS_PROX_OC client_present %d, curr SR %d, recvd SR %d",
                            client_present,
                            (int32_t)chosen_sample_rate,
                            (int32_t)sample_rate);

    if (num_clients > inst_state->prox_info.client_cfg.num_oc_clients_prev
        &&
        num_clients > 1)
    {
      inst_state->prox_info.client_cfg.oc_send_last_event = true;
    }
    else
    {
      inst_state->prox_info.client_cfg.oc_send_last_event = false;
    }
    inst_state->prox_info.client_cfg.num_oc_clients_prev = num_clients;

    if(client_present)
    {
      chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
      chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);
      inst_state->publish_sensors |= AMS_PROX_OC;
      inst_state->publish_sensors_new |= AMS_PROX_OC;
    }
    else
    {
      inst_state->publish_sensors &= ~(AMS_PROX_OC);
    }
#endif


    if(inst_state->publish_sensors)
    {
      ams_set_inst_config(this,
                          instance,
                          chosen_report_rate,
                          chosen_sample_rate,
                          SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);
    }

  SNS_PRINTF(MED, this, "ams_reval_instance_config exit: publish_sensors=0x%04x, publish_sensors_prev=0x%04x",
               inst_state->publish_sensors, inst_state->publish_sensors_prev );

  if((!inst_state->publish_sensors) && ( inst_state-> client_present <= 0))
  {
    SNS_PRINTF(MED, this, "ams_reval_instance_config: .publish_sensors == 0, turn rails off");
    ams_turn_rails_off(this);
    inst_state->instance_is_ready_to_configure = false;
    AMS_DEV_PRINTF(MED,this, "AMS: ams_reval_instance_config: set instance_is_ready_to_configure to false");
  }
}

/**
 * Sets instance config to run a self test.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 *
 * @return none
 */
void tmd2725_set_self_test_inst_config(sns_sensor *this,
                              sns_sensor_instance *instance)
{

  sns_request config;

  config.message_id = SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG;
  config.request_len = 0;
  config.request = NULL;

  this->instance_api->set_client_config(instance, &config);
}

/**
 * It extracts each element's version as well as it's value.
 *
 * @return bool True if decoding is successfull else false.
 */
bool
tmd2725_custom_parse_registry_float_array(sns_registry_data_item *reg_item,
                                      pb_buffer_arg  *item_name,
                                      pb_buffer_arg  *item_str_val,
                                      void *parsed_buffer)
{
  bool rv = true;
  pb_custom_float_parse_arg *arg = (pb_custom_float_parse_arg*) parsed_buffer;

  if(NULL == reg_item || NULL == item_name || NULL == item_str_val ||
     NULL == parsed_buffer)
  {
    rv = false;
  }
  else if(reg_item->has_flt && arg->size > *arg->index)
  {
    {
      arg->data_array[*arg->index] = reg_item->flt;
      if (reg_item->has_version) {
        arg->version_array[*arg->index] = reg_item->version;
      }
      (*arg->index)++;
    }
  }
  else
  {
    rv = false;
  }

  return rv;
}

bool
tmd2725_custom_parse_registry_custom_cal_array(sns_registry_data_item *reg_item,
                                      pb_buffer_arg  *item_name,
                                      pb_buffer_arg  *item_str_val,
                                      void *parsed_buffer)
{
  bool rv = true;
  pb_custom_calibration_values *arg = (pb_custom_calibration_values*) parsed_buffer;

  if(NULL == reg_item || NULL == item_name || NULL == item_str_val ||
     NULL == parsed_buffer)
  {
    rv = false;
  }
  else if(reg_item->has_flt && arg->size > *arg->index)
  {
      arg->data_array[*arg->index] = reg_item->flt;
      if (reg_item->has_version) {
        arg->version_array[*arg->index] = reg_item->version;
      }

      pb_custom_calibration_values *has_values =
      (pb_custom_calibration_values *)parsed_buffer;

      if(0 == strncmp((char*)item_name->buf, "l0_factor", item_name->buf_len)){
        has_values->has_L0factor = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "l1_factor", item_name->buf_len)){
        has_values->has_L1factor = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "l2_factor", item_name->buf_len)){
        has_values->has_L2factor = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "l3_factor", item_name->buf_len)){
        has_values->has_L3factor = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "d_factor", item_name->buf_len)){
        has_values->has_Dfactor = true;
      }
      (*arg->index)++;
  }
  else
  {
    rv = false;
  }

  return rv;
}

bool
tmd2725_parse_registry_als_cal_target_array(sns_registry_data_item *reg_item,
                                            pb_buffer_arg  *item_name,
                                            pb_buffer_arg  *item_str_val,
                                            void *parsed_buffer)
{
  bool rv = true;
  pb_als_calibration_target_values *arg = (pb_als_calibration_target_values*) parsed_buffer;

  if(NULL == reg_item || NULL == item_name || NULL == item_str_val ||
     NULL == parsed_buffer)
  {
    rv = false;
  }
  else if(reg_item->has_flt && arg->size > *arg->index)
  {
      arg->data_array[*arg->index] = reg_item->flt;
      if (reg_item->has_version) {
        arg->version_array[*arg->index] = reg_item->version;
      }

      pb_als_calibration_target_values *has_values = (pb_als_calibration_target_values *)parsed_buffer;

      if(0 == strncmp((char*)item_name->buf, "lux_cal_target", item_name->buf_len)){
        has_values->has_luxTarget = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "lux_cal_target_tolerance", item_name->buf_len)){
        has_values->has_luxTargetTolerance = true;
      }
      (*arg->index)++;
  }
  else
  {
    rv = false;
  }

  return rv;
}


bool
tmd2725_als_default_config_data_array(sns_registry_data_item *reg_item,
                                      pb_buffer_arg  *item_name,
                                      pb_buffer_arg  *item_str_val,
                                      void *parsed_buffer)
{
  bool rv = true;
  pb_als_default_config_values *arg = (pb_als_default_config_values*) parsed_buffer;

  if(NULL == reg_item || NULL == item_name || NULL == item_str_val ||
     NULL == parsed_buffer)
  {
    rv = false;
  }
  else if(reg_item->has_flt && arg->size > *arg->index)
  {
      arg->data_array[*arg->index] = reg_item->flt;
      if (reg_item->has_version) {
        arg->version_array[*arg->index] = reg_item->version;
      }

      pb_als_default_config_values *has_values =
      (pb_als_default_config_values *)parsed_buffer;

      if(0 == strncmp((char*)item_name->buf, "a_time", item_name->buf_len)){
        has_values->hasAtime = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "w_time", item_name->buf_len)){
        has_values->hasWtime = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "w_long", item_name->buf_len)){
        has_values->hasWlong = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "a_gain", item_name->buf_len)){
        has_values->hasAgain = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "a_pers", item_name->buf_len)){
        has_values->hasApers = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "a_upperThrs", item_name->buf_len)){
        has_values->hasUpperThrs = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "a_lowerThrs", item_name->buf_len)){
        has_values->hasLowerThrs = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "a_thrsWidth", item_name->buf_len)){
        has_values->hasThrsWidth = true;
      }
      (*arg->index)++;
  }
  else
  {
    rv = false;
  }

  return rv;
}

bool
tmd2725_prox_default_config_data_array(sns_registry_data_item *reg_item,
                                      pb_buffer_arg  *item_name,
                                      pb_buffer_arg  *item_str_val,
                                      void *parsed_buffer)
{
  bool rv = true;
  pb_prox_default_config_values *arg = (pb_prox_default_config_values*) parsed_buffer;

  if(NULL == reg_item || NULL == item_name || NULL == item_str_val ||
     NULL == parsed_buffer)
  {
    rv = false;
  }
  else if(reg_item->has_flt && arg->size > *arg->index)
  {
      arg->data_array[*arg->index] = reg_item->flt;
      if (reg_item->has_version) {
        arg->version_array[*arg->index] = reg_item->version;
      }

      pb_prox_default_config_values *has_values =
      (pb_prox_default_config_values *)parsed_buffer;

      if(0 == strncmp((char*)item_name->buf, "ptime", item_name->buf_len)){
        has_values->hasPtime = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "pwait_time", item_name->buf_len)){
        has_values->hasPwtime = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "ppulse_len", item_name->buf_len)){
        has_values->hasPpulseLen = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "ppulse", item_name->buf_len)){
        has_values->hasPpulse = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "pgain", item_name->buf_len)){
        has_values->hasPpGain = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "pldrive", item_name->buf_len)){
        has_values->hasPldrive = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "ppers", item_name->buf_len)){
        has_values->hasPpers = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "pupper_thrs", item_name->buf_len)){
        has_values->hasPupperThrs = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "plower_thrs", item_name->buf_len)){
        has_values->hasPlowerThrs = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "poffset_sign", item_name->buf_len)){
        has_values->hasPoffsetSign = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "poffset_L", item_name->buf_len)){
        has_values->hasProxOffset = true;
      }
      else if(0 == strncmp((char*)item_name->buf, "pBinSearch", item_name->buf_len)){
        has_values->hasPbinSearch = true;
      }
      (*arg->index)++;
  }
  else
  {
    rv = false;
  }

  return rv;
}

static void ams_sensor_process_registry_event(sns_sensor *const this,
                                              sns_sensor_event *event)
{
  bool rv = true;
  bool b_need_to_pub_prox_attr = false;
  bool b_need_to_pub_als_resolution = false;
  ams_state *state = (ams_state*)this->state->state;
  float_t parsed_float_array[ALS_CUSTOM_CAL_NUM] = {0.0, 0.0, 0.0, 0.0, 0.0};
  float_t parsed_als_default_config_array[ALS_DEFAULT_CONFIG_VAL] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  float_t parsed_prox_default_config_array[PROX_DEFAULT_CONFIG_VAL] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  pb_istream_t stream = pb_istream_from_buffer((void*)event->event,
      event->event_len);
  int32_t hwid = this->cb->get_registration_index(this);

  if(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
  {
    sns_registry_read_event read_event = sns_registry_read_event_init_default;
    pb_buffer_arg group_name = {0,0};
    read_event.name.arg = &group_name;
    read_event.name.funcs.decode = pb_decode_string_cb;

    if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
    {
      SNS_PRINTF(ERROR, this, "Error decoding registry event");
    }
    else
    {
      stream = pb_istream_from_buffer((void*)event->event, event->event_len);

      if(0 == strncmp((char*)group_name.buf, AMS_REGISTRY_ENTRY_NAME("_0.als.config"),
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, AMS_REGISTRY_ENTRY_NAME("_0.prox.config"),
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, AMS_REGISTRY_ENTRY_NAME("_1.als.config"),
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, AMS_REGISTRY_ENTRY_NAME("_1.prox.config"),
                      group_name.buf_len))
      {
        {
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = "config",
              .parse_func = sns_registry_parse_phy_sensor_cfg,
              .parsed_buffer = &state->registry_cfg
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->registry_cfg_received = true;
          state->is_dri = state->registry_cfg.is_dri;
          state->hardware_id = state->registry_cfg.hw_id;
          state->resolution_idx = state->registry_cfg.res_idx;
          state->supports_sync_stream = state->registry_cfg.sync_stream;
          AMS_DEV_PRINTF(HIGH, this, "Registry read event received hardware_id:%d, is_dri: %d",
                     state->hardware_id, state->is_dri);
          AMS_DEV_PRINTF(HIGH, this, "resolution_idx:%d, supports_sync_stream:%d ",
                     state->resolution_idx, state->supports_sync_stream);
        }
      }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
      else if(0 == strncmp((char*)group_name.buf, registry_als_fac_cal_names[hwid],
                           group_name.buf_len))
      {
        float parsed_float_array[ALS_FAC_CAL_NUM] = {0};
        uint32_t version_array[ALS_FAC_CAL_NUM] = {0};
        {
          uint8_t arr_index = 0;

          pb_custom_float_parse_arg arr_arg = {
            .data_array = parsed_float_array,
            .version_array = version_array,
            .index = &arr_index,
            .size = ALS_FAC_CAL_NUM
          };
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .parse_func = tmd2725_custom_parse_registry_float_array,
              .parsed_buffer = &arr_arg
            }
          };
          AMS_DEV_PRINTF(LOW, this, " AMS: ams_sensor_process_registry_event() hwid %d", hwid);
          sns_strlcpy(arg.parse_info[0].group_name, registry_als_fac_cal_names[hwid], SNS_REGISTRY_MAX_NAME_LEN);

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }
        if(rv)
        {
          state->calibration_data_tmd2725.alsCalibrationFactor = (uint16_t)parsed_float_array[0];
          state->cal_version[ALSCALIBRATIONFACTOR_IDX] = version_array[0];

          AMS_DEV_PRINTF(MED, this, "Registry read event for ALS fac_cal calibration received ");
          sns_memscpy(state->calibration_data_tmd2725.deviceName, sizeof(state->calibration_data_tmd2725.deviceName),
              STRINGIFY(CONFIG_AMS_PART_NAME), sizeof(state->calibration_data_tmd2725.deviceName));

          AMS_DEV_PRINTF(MED, this, "Als Scale: %d, Als Bias: %d",
                               (int32_t)parsed_float_array[0],
                               (int32_t)parsed_float_array[1]);

        }
      }
      /**
       * THIS SECTION IS TO READ THE CUSTOM CALLIBRATION DATA FOR ALS FROM THE
       * JSON FILE
       */
      else if(0 == strncmp((char*)group_name.buf, registry_als_custom_cal_names[hwid],
                           group_name.buf_len))
      {
        uint32_t version_array[ALS_CUSTOM_CAL_NUM] = {0, 0, 0, 0, 0};
        struct pb_custom_calibration_values *pb_custom_cal_value = NULL;

        {
          uint8_t arr_index = 0;

          struct pb_custom_calibration_values arr_arg = {
            .L0factor = 0.0,
            .L1factor = 0.0,
            .L2factor = 0.0,
            .L3factor = 0.0,
            .Dfactor = 0.0,
            .data_array = &parsed_float_array[0],
            .version_array = version_array,
            .index = &arr_index,
            .size = ALS_CUSTOM_CAL_NUM,
            .has_L0factor = false,
            .has_L1factor = false,
            .has_L2factor = false,
            .has_L3factor = false,
            .has_Dfactor = false
          };

          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .parse_func = tmd2725_custom_parse_registry_custom_cal_array,
              .parsed_buffer = &arr_arg
            }
          };
          sns_strlcpy(arg.parse_info[0].group_name, registry_als_custom_cal_names[hwid], SNS_REGISTRY_MAX_NAME_LEN);

          pb_custom_cal_value = &arr_arg;

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }
        if(rv)
        {
          /**
           * READ THE CUSTOM VALUES OF L0_FACTOR, L1_FACTOR, L2_FACTOR,
           * L3_FACTOR AND DFG FROM THE JSON FILE. IF THE JSON FILE HAS NO VALUE
           * THEN READ THE CORRESPONDING VALUES FROM THE default_common_cal_data
           * STRUCTURE. IF THE VALUE IS DIFFERENT FROM THE default_common_cal_data
           * THEN READ THE VALUES FROM THE JSON FILE.
           */
          if(true == pb_custom_cal_value->has_L0factor){
            state->calibration_data_tmd2725.L0_factor = (int32_t)parsed_float_array[0];
            AMS_DEV_PRINTF(LOW, this, "calibration_data.L0_factor read from json %d", (int32_t)parsed_float_array[0]);
          }else{
            state->calibration_data_tmd2725.L0_factor = default_common_cal_data_tmd2725.L0_factor;
            AMS_DEV_PRINTF(LOW, this, "calibration_data.L0_factor read from default_common_cal_data_tmd2725 %d", default_common_cal_data_tmd2725.L0_factor);
          }

          if(true == pb_custom_cal_value->has_L1factor){
            state->calibration_data_tmd2725.L1_factor = (int32_t)parsed_float_array[1];
            AMS_DEV_PRINTF(LOW, this, "calibration_data.L1_factor read from json %d", (int32_t)parsed_float_array[1]);
          }else{
            state->calibration_data_tmd2725.L1_factor = default_common_cal_data_tmd2725.L1_factor;
            AMS_DEV_PRINTF(LOW, this, "calibration_data.L1_factor read from default_common_cal_data %d", default_common_cal_data_tmd2725.L1_factor);
          }

          if(true == pb_custom_cal_value->has_L2factor){
            state->calibration_data_tmd2725.L2_factor = (int32_t)parsed_float_array[2];
            AMS_DEV_PRINTF(LOW, this, "calibration_data.L2_factor read from json %d", (int32_t)parsed_float_array[2]);
          }else{
            state->calibration_data_tmd2725.L2_factor = default_common_cal_data_tmd2725.L2_factor;
            AMS_DEV_PRINTF(LOW, this, "calibration_data.L2_factor read from default_common_cal_data %d", default_common_cal_data_tmd2725.L2_factor);
          }

          if(true == pb_custom_cal_value->has_L3factor){
            state->calibration_data_tmd2725.L3_factor = (int32_t)parsed_float_array[3];
            AMS_DEV_PRINTF(LOW, this, "calibration_data.L3_factor read from json %d", (int32_t)parsed_float_array[3]);
          }else{
            state->calibration_data_tmd2725.L3_factor = default_common_cal_data_tmd2725.L3_factor;
            AMS_DEV_PRINTF(LOW, this, "calibration_data.L3_factor read from default_common_cal_data %d", default_common_cal_data_tmd2725.L3_factor);
          }

          if(true == pb_custom_cal_value->has_Dfactor){
            state->calibration_data_tmd2725.D_factor = (int32_t)parsed_float_array[4];
            AMS_DEV_PRINTF(LOW, this, "calibration_data.D_factor read from json %d", (int32_t)parsed_float_array[4]);
          }else{
            state->calibration_data_tmd2725.D_factor = default_common_cal_data_tmd2725.D_factor;
            AMS_DEV_PRINTF(LOW, this, "calibration_data.D_factor read from default_common_cal_data %d", default_common_cal_data_tmd2725.D_factor);
          }

          AMS_DEV_PRINTF(LOW, this, "Registry read event for ALS custom_cal calibration received ");
          sns_memscpy(state->calibration_data_tmd2725.deviceName, sizeof(state->calibration_data_tmd2725.deviceName),
              STRINGIFY(CONFIG_AMS_PART_NAME), sizeof(state->calibration_data_tmd2725.deviceName));

          AMS_DEV_PRINTF(LOW, this, "Als l0_factor: %d, Als l1_factor: %d, Als l2_factor: %d, Als l3_factor: %d, Als d_factor: %d",
                                (int32_t)parsed_float_array[0],
                                (int32_t)parsed_float_array[1],
                                (int32_t)parsed_float_array[2],
                                (int32_t)parsed_float_array[3],
                                (int32_t)parsed_float_array[4]);
        }
      }
       /**
       * THIS SECTION IS TO READ THE ALS DEFAULT CONFIG DATA FOR ALS FROM THE
       * JSON FILE
       */
      else if(0 == strncmp((char*)group_name.buf, registry_als_default_config_names[hwid],
                           group_name.buf_len))
      {
        uint32_t version_array[ALS_DEFAULT_CONFIG_VAL] = {0, 0, 0, 0, 0, 0, 0, 0};
        struct pb_als_default_config_values *pb_als_default_config_value = NULL;
        {
          uint8_t arr_index = 0;

          struct pb_als_default_config_values arr_arg = {
            .aTime = 0.0,
            .wTime = 0.0,
            .wLong = 0.0,
            .aGain = 0.0,
            .aPers = 0.0,
            .aUpperThrs = 0.0,
            .aLowerThrs = 0.0,
            .aThrsWidth = 0.0,
            .data_array = &parsed_als_default_config_array[0],
            .version_array = version_array,
            .index = &arr_index,
            .size = ALS_DEFAULT_CONFIG_VAL,
            .hasAgain = false,
            .hasApers = false,
            .hasAtime = false,
            .hasUpperThrs = false,
            .hasLowerThrs = false,
            .hasWlong = false,
            .hasWtime = false,
            .hasThrsWidth = false
          };

          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .parse_func = tmd2725_als_default_config_data_array,
              .parsed_buffer = &arr_arg
            }
          };
          sns_strlcpy(arg.parse_info[0].group_name, registry_als_default_config_names[hwid], SNS_REGISTRY_MAX_NAME_LEN);

          pb_als_default_config_value = &arr_arg;

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }
        if(rv)
        {
          /**
           * READ THE ALS DEFAULT CONFIG VALUES FROM THE JSON FILE
           */
          if(true == pb_als_default_config_value->hasAtime){
            state->als_default_config_data.aTime = (uint32_t)parsed_als_default_config_array[0];
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.aTime read from json %d", (int32_t)parsed_als_default_config_array[0]);
          }
          else{
            state->als_default_config_data.aTime = default_als_config_data.aTime;
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.aTime %d", default_als_config_data.aTime);
          }
		  
		  b_need_to_pub_als_resolution = true;

          if(true == pb_als_default_config_value->hasWtime){
            state->als_default_config_data.wTime = (int32_t)parsed_als_default_config_array[1]*1000;
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.wTime read from json %d", (int32_t)parsed_als_default_config_array[1]);
          }
          else{
            state->als_default_config_data.wTime = default_als_config_data.wTime;
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.wTime %d", default_als_config_data.wTime);
          }

          if(true == pb_als_default_config_value->hasWlong){
            state->als_default_config_data.wLong = (int32_t)parsed_als_default_config_array[2];
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.wLong read from json %d", (int32_t)parsed_als_default_config_array[2]);
          }
          else{
            state->als_default_config_data.wLong = default_als_config_data.wLong;
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.wLong %d", default_als_config_data.wLong);
          }

          if(true == pb_als_default_config_value->hasAgain){
            state->als_default_config_data.aGain = (int32_t)parsed_als_default_config_array[3];
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.aGain read from json %d", (int32_t)parsed_als_default_config_array[3]);
          }
          else{
            state->als_default_config_data.aGain = default_als_config_data.aGain;
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.aGain %d", default_als_config_data.aGain);
          }

          if(true == pb_als_default_config_value->hasApers){
            state->als_default_config_data.aPers = (int32_t)parsed_als_default_config_array[4];
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.aPers read from json %d", (int32_t)parsed_als_default_config_array[4]);
          }
          else{
            state->als_default_config_data.aPers = default_als_config_data.aPers;
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.aPers %d", default_als_config_data.aPers);
          }

          if(true == pb_als_default_config_value->hasLowerThrs){
            state->als_default_config_data.aLowerThrs = (int32_t)parsed_als_default_config_array[5];
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.aLowerThrs read from json %d", (int32_t)parsed_als_default_config_array[5]);
          }
          else{
            state->als_default_config_data.aLowerThrs = default_als_config_data.aLowerThrs;
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.aLowerThrs %d", default_als_config_data.aLowerThrs);
          }

          if(true == pb_als_default_config_value->hasUpperThrs){
            state->als_default_config_data.aUpperThrs = (int32_t)parsed_als_default_config_array[6];
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.aUpperThrs read from json %d", (int32_t)parsed_als_default_config_array[6]);
          }
          else{
            state->als_default_config_data.aUpperThrs = default_als_config_data.aUpperThrs;
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.aUpperThrs %d", default_als_config_data.aUpperThrs);
          }

          if(true == pb_als_default_config_value->hasThrsWidth){
            state->als_default_config_data.aThrsWidth = (int32_t)parsed_als_default_config_array[7];
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.aThrsWidth read from json %d", (int32_t)parsed_als_default_config_array[7]);
          }
          else{
            state->als_default_config_data.aThrsWidth = default_als_config_data.aThrsWidth;
            AMS_DEV_PRINTF(LOW, this, "als_default_config_data.aThrsWidth %d", default_als_config_data.aThrsWidth);
          }

          AMS_DEV_PRINTF(LOW, this, "Registry read event for ALS default config received ");
          sns_memscpy(state->als_default_config_data.deviceName, sizeof(state->als_default_config_data.deviceName),
              STRINGIFY(CONFIG_AMS_PART_NAME), sizeof(state->als_default_config_data.deviceName));

          AMS_DEV_PRINTF(LOW, this, "Als ATIME: %d, Als WTIME: %d, Als WLONG: %d, Als AGAIN: %d, Als APERS: %d, Als LOWER THRS: %d, Als UPPER THRS: %d, \
                                 Als Thrs Width: %d", \
                                (int32_t)parsed_als_default_config_array[0],
                                (int32_t)parsed_als_default_config_array[1],
                                (int32_t)parsed_als_default_config_array[2],
                                (int32_t)parsed_als_default_config_array[3],
                                (int32_t)parsed_als_default_config_array[4],
                                (int32_t)parsed_als_default_config_array[5],
                                (int32_t)parsed_als_default_config_array[6],
                                (int32_t)parsed_als_default_config_array[7]);
        }
      }
      /**
       * THIS SECTION IS TO READ THE ALS CALLIBRATION TARGET DATA FROM THE
       * JSON FILE
       */
      else if(0 == strncmp((char*)group_name.buf, registry_als_cal_target_names[hwid],
                           group_name.buf_len))
      {
          float parsed_als_cal_target_config_array[ALS_CAL_TARGET_NUM] = {0.0, 0.0};
          uint32_t version_array[ALS_CAL_TARGET_NUM] = {0, 0};
          struct pb_als_calibration_target_values *pb_als_cal_target_value = NULL;

          AMS_DEV_PRINTF(LOW, this, "Registry read event for ALS calibration target received ");

          uint8_t arr_index = 0;

          struct pb_als_calibration_target_values arr_arg = {
              .luxTarget = 0.0,
              .luxTargetTolerance = 0.0,
              .data_array = &parsed_als_cal_target_config_array[0],
              .version_array = version_array,
              .index = &arr_index,
              .size = ALS_CAL_TARGET_NUM,
              .has_luxTarget = false,
              .has_luxTargetTolerance = false
          };

          sns_registry_decode_arg arg = {
              .item_group_name = &group_name,
              .parse_info_len = 1,
              .parse_info[0] = {
                  .parse_func = tmd2725_parse_registry_als_cal_target_array,
                  .parsed_buffer = &arr_arg
              }
          };
          sns_strlcpy(arg.parse_info[0].group_name, registry_als_cal_target_names[hwid], SNS_REGISTRY_MAX_NAME_LEN);

          pb_als_cal_target_value = &arr_arg;

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);

          if(rv)
          {
              AMS_DEV_PRINTF(LOW, this, "entering assignment block");

              /**
               * READ THE CUSTOM VALUES OF LUX_TARGET, LUX_TARGET_TOLERANCE
               * FROM THE JSON FILE. IF THE JSON FILE HAS NO VALUE
               * THEN READ THE CORRESPONDING VALUES FROM THE default_common_cal_data
               * STRUCTURE. IF THE VALUE IS DIFFERENT FROM THE default_common_cal_data
               * THEN READ THE VALUES FROM THE JSON FILE.
               */
              if(true == pb_als_cal_target_value->has_luxTarget){
                  state->calibration_data_tmd2725.alsCalibrationLuxTarget = (uint16_t)parsed_als_cal_target_config_array[0];
                  if (state->priv != NULL)
                  {
                      ams_deviceCtx_t *devCtx = (ams_deviceCtx_t *)(state->priv);
                      devCtx->ccbAlsCtx.initData.calibrationData.luxTarget = (uint16_t)parsed_als_cal_target_config_array[0];
                  }
                  else
                  {
                      AMS_DEV_PRINTF(LOW, this, "lux target: state->priv is NULL!!!!!!!!!!!!!!!!");
                  }
                  AMS_DEV_PRINTF(LOW, this, "calibration_data.alsCalibrationLuxTarget read from json %d", (uint16_t)parsed_als_cal_target_config_array[0]);
              }else{
                  state->calibration_data_tmd2725.alsCalibrationLuxTarget = default_common_cal_data_tmd2725.alsCalibrationLuxTarget;
                  AMS_DEV_PRINTF(LOW, this, "calibration_data.alsCalibrationLuxTarget read from default_common_cal_data_tmd2725 %d", default_common_cal_data_tmd2725.alsCalibrationLuxTarget);
              }

              if(true == pb_als_cal_target_value->has_luxTargetTolerance){
                  state->calibration_data_tmd2725.alsCalibrationLuxTargetError = (uint16_t)parsed_als_cal_target_config_array[1];
                  if (state->priv != NULL)
                  {
                      ams_deviceCtx_t *devCtx = (ams_deviceCtx_t *)(state->priv);
                      devCtx->ccbAlsCtx.initData.calibrationData.luxTargetError = (uint16_t)parsed_als_cal_target_config_array[1];
                  }
                  else
                  {
                      AMS_DEV_PRINTF(LOW, this, "lux target tolerance: state->priv is NULL!!!!!!!!!!!!!!!!");
                  }
                  AMS_DEV_PRINTF(LOW, this, "calibration_data.alsCalibrationLuxTargetError read from json %d", (uint16_t)parsed_als_cal_target_config_array[1]);
              }else{
                  state->calibration_data_tmd2725.alsCalibrationLuxTargetError = default_common_cal_data_tmd2725.alsCalibrationLuxTargetError;
                  AMS_DEV_PRINTF(LOW, this, "calibration_data.alsCalibrationLuxTargetError read from default_common_cal_data %d", default_common_cal_data_tmd2725.alsCalibrationLuxTargetError);
              }

              AMS_DEV_PRINTF(LOW, this, "Als cal target: %d, Als cal target tolerance: %d",
                             (uint16_t)parsed_als_cal_target_config_array[0],
                             (uint16_t)parsed_als_cal_target_config_array[1]);
          }
          else
          {
              SNS_PRINTF(ERROR, this, "Error decoding als.cal_target from registry %d ", rv);
          }
      }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
      else if(0 == strncmp((char*)group_name.buf, registry_prox_fac_cal_names[hwid],
                           group_name.buf_len))
      {
        float parsed_float_array[PROX_FAC_CAL_NUM] = {0};
        uint32_t version_array[PROX_FAC_CAL_NUM] = {0};
        {
          uint8_t arr_index = 0;

          pb_custom_float_parse_arg arr_arg = {
            .data_array = parsed_float_array,
            .version_array = version_array,
            .index = &arr_index,
            .size = PROX_FAC_CAL_NUM
          };
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .parse_func = tmd2725_custom_parse_registry_float_array,
              .parsed_buffer = &arr_arg
            }
          };
          sns_strlcpy(arg.parse_info[0].group_name, registry_prox_fac_cal_names[hwid], SNS_REGISTRY_MAX_NAME_LEN);

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }
        if(rv)
        {
            /* threshold data is stored in the registry as 8-bit values but the driver
             * operates on 16-bit values.
             */
            state->calibration_data_tmd2725.proxHardThreshold = (uint16_t)(((uint8_t)parsed_float_array[0]) << 8);
            state->calibration_data_tmd2725.proxMinThreshold = (uint16_t)(((uint8_t)parsed_float_array[1]) << 8);

            /* Save the proximity threshold values from the registry. These values will be used for resetting
             * in the scenario where a client wishes to reset the proximity calibrataion. This decision was
             * agreed with Qualcomm on 3/26/19.
             */
            state->calibration_data_tmd2725.proxHardThresholdResetValue = state->calibration_data_tmd2725.proxHardThreshold;
            state->calibration_data_tmd2725.proxMinThresholdResetValue = state->calibration_data_tmd2725.proxMinThreshold;
          state->cal_version[PROXHARDTHRESHOLD_IDX] = version_array[0];

          AMS_DEV_PRINTF(LOW, this, "Registry read event for PROX fac_cal calibration received ");
          sns_memscpy(state->calibration_data_tmd2725.deviceName, sizeof(state->calibration_data_tmd2725.deviceName),
              STRINGIFY(CONFIG_AMS_PART_NAME), sizeof(state->calibration_data_tmd2725.deviceName));

          AMS_DEV_PRINTF(LOW, this, "Prox near_threshold: %d, Prox far_threshold: %d",
                               (int32_t)parsed_float_array[0],
                               (int32_t)parsed_float_array[1]);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, registry_prox_proxOffset_hWcal_names[hwid],
                           group_name.buf_len))
      {
        float parsed_float_array[PROX_OFFSET_HW_CAL_NUM] = {0};
        uint32_t version_array[PROX_OFFSET_HW_CAL_NUM] = {0};
        {
          uint8_t arr_index = 0;

          pb_custom_float_parse_arg arr_arg = {
            .data_array = parsed_float_array,
            .version_array = version_array,
            .index = &arr_index,
            .size = PROX_OFFSET_HW_CAL_NUM
          };
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .parse_func = tmd2725_custom_parse_registry_float_array,
              .parsed_buffer = &arr_arg
            }
          };
          sns_strlcpy(arg.parse_info[0].group_name, registry_prox_proxOffset_hWcal_names[hwid], SNS_REGISTRY_MAX_NAME_LEN);

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }
        if(rv)
        {
          state->calibration_data_tmd2725.proxOffset = (uint8_t)parsed_float_array[0];
          state->calibration_data_tmd2725.proxOffsetSign = (uint8_t)parsed_float_array[1];
          state->cal_version[PROXOFFSET_IDX] = version_array[0];
          state->cal_version[PROXOFFSETSIGN_IDX] = version_array[0];

          AMS_DEV_PRINTF(LOW, this, "Registry read event for PROX proxOffset_hWcal calibration received ");
          sns_memscpy(state->calibration_data_tmd2725.deviceName, sizeof(state->calibration_data_tmd2725.deviceName),
              STRINGIFY(CONFIG_AMS_PART_NAME), sizeof(state->calibration_data_tmd2725.deviceName));

          AMS_DEV_PRINTF(LOW, this, "Prox proxOffset: %d, Prox proxOffsetSign: %d",
                               (int32_t)parsed_float_array[0],
                               (int32_t)parsed_float_array[1]);
        }
      }
      /**
       * THIS SECTION IS TO READ THE PROX DEFAULT CONFIG DATA FOR PROX FROM THE
       * JSON FILE
       */
      else if(0 == strncmp((char*)group_name.buf, registry_prox_default_config_names[hwid],
                           group_name.buf_len))
      {
        uint32_t version_array[PROX_DEFAULT_CONFIG_VAL] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        struct pb_prox_default_config_values *pb_prox_default_config_value = NULL;
        {
          uint8_t arr_index = 0;

          struct pb_prox_default_config_values arr_arg = {
            .pTime = 0.0,
            .pwTime_us = 0.0,
            .pPulseLen = 0.0,
            .pPulse = 0.0,
            .pGain = 0.0,
            .pLdrive = 0.0,
            .pPers = 0.0,
            .pUpperThrs = 0.0,
            .pLowerThrs = 0.0,
            .pOffsetSign = 0.0,
            .pProxOffset = 0.0,
            .pBinSearch = 0.0,
            .data_array = &parsed_prox_default_config_array[0],
            .version_array = version_array,
            .index = &arr_index,
            .size = PROX_DEFAULT_CONFIG_VAL,
            .hasPtime = false,
            .hasPwtime = false,
            .hasPpulseLen = false,
            .hasPpulse = false,
            .hasPpGain = false,
            .hasPldrive = false,
            .hasPpers = false,
            .hasPupperThrs = false,
            .hasPlowerThrs = false,
            .hasPoffsetSign = false,
            .hasProxOffset = false,
            .hasPbinSearch = false
          };

          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .parse_func = tmd2725_prox_default_config_data_array,
              .parsed_buffer = &arr_arg
            }
          };
          sns_strlcpy(arg.parse_info[0].group_name, registry_prox_default_config_names[hwid], SNS_REGISTRY_MAX_NAME_LEN);

          pb_prox_default_config_value = &arr_arg;

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }
        if(rv)
        {
          /**
           * READ THE PROX DEFAULT CONFIG VALUES FROM THE JSON FILE
           */
          if(true == pb_prox_default_config_value->hasPtime){
            state->prox_default_config_data.pTime = (uint32_t)parsed_prox_default_config_array[0];
            AMS_DEV_PRINTF(LOW, this, "AMS: prox_default_config_data.pTime read from json %d", (int32_t)parsed_prox_default_config_array[0]);
          }
          else{
            state->prox_default_config_data.pTime = default_prox_config_data.pTime;
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pTime %d", default_prox_config_data.pTime);
          }

          if(true == pb_prox_default_config_value->hasPwtime){
            state->prox_default_config_data.pwTime_us = (uint32_t)parsed_prox_default_config_array[1]*1000;
            AMS_DEV_PRINTF(LOW, this, "AMS: prox_default_config_data.pwTime_us read from json %d", (int32_t)parsed_prox_default_config_array[1]);
          }
          else{
            state->prox_default_config_data.pwTime_us = default_prox_config_data.pwTime_us;
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pwTime_us %d", default_prox_config_data.pwTime_us);
          }

          if(true == pb_prox_default_config_value->hasPpulseLen){
            state->prox_default_config_data.pPulseLen = (int16_t)parsed_prox_default_config_array[2];
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pPulseLen read from json %d", (int32_t)parsed_prox_default_config_array[2]);
          }
          else{
            state->prox_default_config_data.pPulseLen = default_prox_config_data.pPulseLen;
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pPulseLen %d", default_prox_config_data.pPulseLen);
          }

          if(true == pb_prox_default_config_value->hasPpulse){
            state->prox_default_config_data.pPulse = (int8_t)parsed_prox_default_config_array[3];
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pPulse read from json %d", (int32_t)parsed_prox_default_config_array[3]);
          }
          else{
            state->prox_default_config_data.pPulse = default_prox_config_data.pPulse;
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pPulse %d", default_prox_config_data.pPulse);
          }

          if(true == pb_prox_default_config_value->hasPpGain){
            state->prox_default_config_data.pGain = (int16_t)parsed_prox_default_config_array[4];
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pGain read from json %d", (int32_t)parsed_prox_default_config_array[4]);
          }
          else{
            state->prox_default_config_data.pGain = default_prox_config_data.pGain;
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pGain %d", default_prox_config_data.pGain);
          }

          if(true == pb_prox_default_config_value->hasPldrive){
            state->prox_default_config_data.pLdrive = (int8_t)parsed_prox_default_config_array[5];
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pLdrive read from json %d", (int32_t)parsed_prox_default_config_array[5]);
          }
          else{
            state->prox_default_config_data.pLdrive = default_prox_config_data.pLdrive;
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pLdrive %d", default_prox_config_data.pLdrive);
          }

          if(true == pb_prox_default_config_value->hasPpers){
            state->prox_default_config_data.pPers = (int8_t)parsed_prox_default_config_array[6];
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pPers read from json %d", (int32_t)parsed_prox_default_config_array[6]);
          }
          else{
            state->prox_default_config_data.pPers = default_prox_config_data.pPers;
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pPers %d", default_prox_config_data.pPers);
          }

          if(true == pb_prox_default_config_value->hasPupperThrs){
            state->prox_default_config_data.pUpperThrs = (int16_t)parsed_prox_default_config_array[7];
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pUpperThrs read from json %d", (int32_t)parsed_prox_default_config_array[7]);
          }
          else{
            state->prox_default_config_data.pUpperThrs = default_prox_config_data.pUpperThrs;
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pUpperThrs %d", default_prox_config_data.pUpperThrs);
          }

          if(true == pb_prox_default_config_value->hasPlowerThrs){
            state->prox_default_config_data.pLowerThrs = (int16_t)parsed_prox_default_config_array[8];
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pLowerThrs read from json %d", (int32_t)parsed_prox_default_config_array[8]);
          }
          else{
            state->prox_default_config_data.pLowerThrs = default_prox_config_data.pLowerThrs;
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pLowerThrs %d", default_prox_config_data.pLowerThrs);
          }

          if(true == pb_prox_default_config_value->hasPoffsetSign){
            state->prox_default_config_data.pOffsetSign = (bool)parsed_prox_default_config_array[9];
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pUpperThrs read from json %d", (int32_t)parsed_prox_default_config_array[9]);
          }
          else{
            state->prox_default_config_data.pOffsetSign = default_prox_config_data.pOffsetSign;
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pOffsetSign %d", default_prox_config_data.pOffsetSign);
          }

          if(true == pb_prox_default_config_value->hasProxOffset){
            state->prox_default_config_data.proxOffset = (int8_t)parsed_prox_default_config_array[10];
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.proxOffset read from json %d", (int32_t)parsed_prox_default_config_array[10]);
          }
          else{
            state->prox_default_config_data.proxOffset = default_prox_config_data.proxOffset;
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.proxOffset %d", default_prox_config_data.proxOffset);
          }

          if(true == pb_prox_default_config_value->hasPbinSearch){
            state->prox_default_config_data.pBinSearch = (int8_t)parsed_prox_default_config_array[11];
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pBinSearch read from json %d", (int32_t)parsed_prox_default_config_array[11]);
          }
          else{
            state->prox_default_config_data.pBinSearch = default_prox_config_data.pBinSearch;
            AMS_DEV_PRINTF(LOW, this, "prox_default_config_data.pBinSearch %d", default_prox_config_data.pBinSearch);
          }

          AMS_DEV_PRINTF(LOW, this, "Registry read event for PRX default config received ");
          sns_memscpy(state->prox_default_config_data.deviceName, sizeof(state->prox_default_config_data.deviceName),
              STRINGIFY(CONFIG_AMS_PART_NAME), sizeof(state->prox_default_config_data.deviceName));

          AMS_DEV_PRINTF(LOW, this, "PRX PTIME: %d, PRX WTIME: %d, PRX pulseLen: %d, PRX Pulse: %d, PRX Pgain: %d, PRX LDrive: %d, PRX Pers: %d, \
                                 PRX upperThrs: %d, PRX lowerThrs: %d, PRX offsetSign: %d, PRX proxOffset: %d, PRX binSearch: %d",
                                (int32_t)parsed_prox_default_config_array[0],
                                (int32_t)parsed_prox_default_config_array[1],
                                (int32_t)parsed_prox_default_config_array[2],
                                (int32_t)parsed_prox_default_config_array[3],
                                (int32_t)parsed_prox_default_config_array[4],
                                (int32_t)parsed_prox_default_config_array[5],
                                (int32_t)parsed_prox_default_config_array[6],
                                (int32_t)parsed_prox_default_config_array[7],
                                (int32_t)parsed_prox_default_config_array[8],
                                (int32_t)parsed_prox_default_config_array[9],
                                (int32_t)parsed_prox_default_config_array[10],
                                (int32_t)parsed_prox_default_config_array[11]);

          b_need_to_pub_prox_attr = true;
        }
      }
#endif
      else if(0 == strncmp((char*)group_name.buf, registry_platform_config_names[hwid],
                           group_name.buf_len))
      {
        {
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = "config",
              .parse_func = sns_registry_parse_phy_sensor_pf_cfg,
              .parsed_buffer = &state->registry_pf_cfg
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->registry_pf_cfg_received = true;

          state->com_port_info.com_config.bus_type = state->registry_pf_cfg.bus_type;
          state->com_port_info.com_config.bus_instance = state->registry_pf_cfg.bus_instance;
          state->com_port_info.com_config.slave_control = state->registry_pf_cfg.slave_config;
          state->com_port_info.com_config.min_bus_speed_KHz = state->registry_pf_cfg.min_bus_speed_khz;
          state->com_port_info.com_config.max_bus_speed_KHz = state->registry_pf_cfg.max_bus_speed_khz;
          state->com_port_info.com_config.reg_addr_type = state->registry_pf_cfg.reg_addr_type;
          state->irq_config.interrupt_num = state->registry_pf_cfg.dri_irq_num;
          state->irq_config.interrupt_pull_type = state->registry_pf_cfg.irq_pull_type;
          state->irq_config.is_chip_pin = state->registry_pf_cfg.irq_is_chip_pin;
          state->irq_config.interrupt_drive_strength = state->registry_pf_cfg.irq_drive_strength;
          state->irq_config.interrupt_trigger_type = state->registry_pf_cfg.irq_trigger_type;
          state->rail_config.num_of_rails = state->registry_pf_cfg.num_rail;
          state->registry_rail_on_state = state->registry_pf_cfg.rail_on_state;
          sns_strlcpy(state->rail_config.rails[0].name,
                      state->registry_pf_cfg.vddio_rail,
                      sizeof(state->rail_config.rails[0].name));
          sns_strlcpy(state->rail_config.rails[1].name,
                      state->registry_pf_cfg.vdd_rail,
                      sizeof(state->rail_config.rails[1].name));

          AMS_DEV_PRINTF(LOW, this, "Registry read event received "
                     "bus_type:%d bus_instance:%d slave_control:%d",
                     state->com_port_info.com_config.bus_type,
                     state->com_port_info.com_config.bus_instance,
                     state->com_port_info.com_config.slave_control);

          AMS_DEV_PRINTF(LOW, this, "min_bus_speed_KHz :%d max_bus_speed_KHz:%d reg_addr_type:%d",
                               state->com_port_info.com_config.min_bus_speed_KHz,
                               state->com_port_info.com_config.max_bus_speed_KHz,
                               state->com_port_info.com_config.reg_addr_type);

          AMS_DEV_PRINTF(LOW, this, "interrupt_num:%d interrupt_pull_type:%d is_chip_pin:%d",
                               state->irq_config.interrupt_num,
                               state->irq_config.interrupt_pull_type,
                               state->irq_config.is_chip_pin);

          AMS_DEV_PRINTF(LOW, this, "interrupt_drive_strength:%d interrupt_trigger_type:%d"
             " rigid body type:%d",
             state->irq_config.interrupt_drive_strength,
             state->irq_config.interrupt_trigger_type,
             state->registry_pf_cfg.rigid_body_type);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, registry_platform_placement_names[hwid],
                           group_name.buf_len))
      {
        {
          uint8_t arr_index = 0;
          pb_float_arr_arg arr_arg = {
            .arr = state->placement,
            .arr_index = &arr_index,
            .arr_len = 12
          };

          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = "placement",
              .parse_func = sns_registry_parse_float_arr,
              .parsed_buffer = &arr_arg
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->registry_placement_received = true;
          AMS_DEV_PRINTF(LOW, this, "Registry read event for placement received "
                 "p[0]:%d p[6]:%d p[11]:%d",
                 (uint32_t)state->placement[0], (uint32_t)state->placement[6],
                 (uint32_t)state->placement[11]);
        }
      }
      else
      {
        rv = false;
      }

      if(!rv)
      {
        //SENSOR_PRINTF_ERROR_FULL(this, "Error decoding registry group %s due to %s", (char*)group_name.buf,
        //                         PB_GET_ERROR(&stream));
      }

      if (b_need_to_pub_prox_attr == true)
      {
          /* The active current for proximity will possibly be changed
           * now that the proximity configuration data has been read
           * from the registry. Therefore, we have to publish attributes
           * again.
           */
          //calculate active current before publish it
          int32_t ac;
          ac = tmd2725_calculate_prox_active_current_from_setting(this);
          tmd2725_publish_prox_active_current_attribute(this, ac);
      }
	  if(b_need_to_pub_als_resolution)
	  {
		  float res;
		  
		  res = 300.0/(getAlsAdcMaxCount(state->als_default_config_data.aTime));
		  tmd2725_publish_als_resolution_attribute(this, res);
		  
	  }
    }
  }
#ifdef TMD2725_FOR_SDM855_AND_LATER
  else  if(SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_EVENT == event->message_id)
  {
    // it's an acknowledgement event from registry due to a registry write request
    // no-op
  }
#endif
  else
  {
    SNS_PRINTF(MED, this, "Received unsupported registry event msg id %u",
                             event->message_id);
  }
}

void tmd2725_publish_available(sns_sensor *const this)
{
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  value.has_boolean = true;
  value.boolean = true;
  sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
}

/** See sns_ams_sensor.h */
void tmd2725_process_suid_events(sns_sensor *const this)
{
  ams_state *state = (ams_state*)this->state->state;

  for(;
      0 != state->fw_stream->api->get_input_cnt(state->fw_stream);
      state->fw_stream->api->get_next_input(state->fw_stream))
  {
    sns_sensor_event *event =
      state->fw_stream->api->peek_input(state->fw_stream);

    if(SNS_SUID_MSGID_SNS_SUID_EVENT == event->message_id)
    {
      pb_istream_t stream = pb_istream_from_buffer((void*)event->event, event->event_len);
      sns_suid_event suid_event = sns_suid_event_init_default;
      pb_buffer_arg data_type_arg = { .buf = NULL, .buf_len = 0 };
      sns_sensor_uid uid_list;
      sns_suid_search suid_search;
      suid_search.suid = &uid_list;
      suid_search.num_of_suids = 0;

      suid_event.data_type.funcs.decode = &pb_decode_string_cb;
      suid_event.data_type.arg = &data_type_arg;
      suid_event.suid.funcs.decode = &pb_decode_suid_event;
      suid_event.suid.arg = &suid_search;

      if(!pb_decode(&stream, sns_suid_event_fields, &suid_event)) {
         SNS_PRINTF(ERROR, this, "AMS: ERROR, ams_process_suid_events: SUID Decode failed");
         continue;
       }

      /* if no suids found, ignore the event */
      if(suid_search.num_of_suids == 0)
      {
        continue;
      }

      /* save suid based on incoming data type name */
      if(0 == strncmp(data_type_arg.buf, "interrupt", data_type_arg.buf_len))
      {
        state->irq_suid = uid_list;
      }
      else if(0 == strncmp(data_type_arg.buf, "timer", data_type_arg.buf_len))
      {
        state->timer_suid = uid_list;
      }
      else if (0 == strncmp(data_type_arg.buf, "registry", data_type_arg.buf_len))
      {
        state->reg_suid = uid_list;
      }
      else if(0 == strncmp(data_type_arg.buf, "async_com_port", data_type_arg.buf_len))
      {
        AMS_PRINTF(LOW, this, "AMS: ams_process_suid_events: received ASCP SUID");
      }
      else
      {
        SNS_PRINTF(ERROR, this, "AMS: ERROR, ams_process_suid_events: invalid datatype_name");
      }
    }
  }
}

void tmd2725_sensor_send_registry_request(sns_sensor *const this,
                                             char *reg_group_name)
{
  ams_state *state = (ams_state*)this->state->state;
  uint8_t buffer[100];
  int32_t encoded_len;
  sns_memset(buffer, 0, sizeof(buffer));
  sns_rc rc = SNS_RC_SUCCESS;

  sns_registry_read_req read_request;
  pb_buffer_arg data = (pb_buffer_arg){ .buf = reg_group_name,
    .buf_len = (strlen(reg_group_name) + 1) };

  read_request.name.arg = &data;
  read_request.name.funcs.encode = pb_encode_string_cb;

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
      &read_request, sns_registry_read_req_fields, NULL);
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
      .request_len = encoded_len, .request = buffer,
      .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ };
    rc = state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
  }
}



sns_rc tmd2725_sensor_notify_event(sns_sensor *const this)
{
  sns_sensor_event *event = NULL;

  ams_state *state = (ams_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_svc = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
  sns_time on_timestamp;
  sns_rc rv = SNS_RC_SUCCESS;
  ams_deviceCtx_t *amsLibraryTmd2725[TMD2725_NUM_DEVICES] = { NULL, NULL};
  int32_t hwid = this->cb->get_registration_index(this);

  AMS_DEV_PRINTF(MED, this, " AMS: ams_sensor_notify_event: enter: this %p, state %p, fw_stream %p, timer_stream %p, pwr_rail_service %p", this, state,\
                            state->fw_stream, state->timer_stream, state->pwr_rail_service);

  if(state->fw_stream)
  {
    if((0 == sns_memcmp(&state->irq_suid, &((sns_sensor_uid){{0}}), sizeof(state->irq_suid)))
     || (0 == sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid)))
     || (0 == sns_memcmp(&state->reg_suid, &((sns_sensor_uid){{0}}), sizeof(state->reg_suid)))
    )
    {
      tmd2725_process_suid_events(this);

      // place a request to registry sensor
      if((0 != sns_memcmp(&state->reg_suid, &((sns_sensor_uid){{0}}), sizeof(state->reg_suid))))
      {
          AMS_DEV_PRINTF(MED, this, "on change: Registry SUID not null");
          if(state->reg_data_stream == NULL)
          {
              AMS_DEV_PRINTF(MED, this, "on change: create_sensor_stream for registry");
              stream_svc->api->create_sensor_stream(stream_svc,
                                                    this, state->reg_suid , &state->reg_data_stream);

              tmd2725_sensor_send_registry_request(this, registry_platform_config_names[hwid]);
              tmd2725_sensor_send_registry_request(this, registry_platform_placement_names[hwid]);
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
              tmd2725_sensor_send_registry_request(this, registry_als_config_names[hwid]);
              tmd2725_sensor_send_registry_request(this, registry_als_fac_cal_names[hwid]);
              tmd2725_sensor_send_registry_request(this, registry_als_custom_cal_names[hwid]);
              tmd2725_sensor_send_registry_request(this, registry_als_default_config_names[hwid]);
              tmd2725_sensor_send_registry_request(this, registry_als_cal_target_names[hwid]);
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
              tmd2725_sensor_send_registry_request(this, registry_prox_config_names[hwid]);
              tmd2725_sensor_send_registry_request(this, registry_prox_fac_cal_names[hwid]);
              tmd2725_sensor_send_registry_request(this, registry_prox_proxOffset_hWcal_names[hwid]);
              tmd2725_sensor_send_registry_request(this, registry_prox_default_config_names[hwid]);
#endif
          }
      }
    }
  }

  /**----------------------Handle a Timer Sensor event.-------------------*/
  if(NULL != state->timer_stream)
  {
    AMS_DEV_PRINTF(LOW, this, "AMS: ams_sensor_notify_event: timer_stream");

    event = state->timer_stream->api->peek_input(state->timer_stream);

    while(NULL != event)
    {
      pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                   event->event_len);
      sns_timer_sensor_event timer_event;
      if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
      {
        if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {
          if(state->power_rail_pend_state == AMS_POWER_RAIL_PENDING_INIT)
          {
            /**-------------------Read and Confirm WHO-AM-I------------------------*/
            AMS_DEV_PRINTF(LOW, this, "AMS: ams_sensor_notify_event: try amsLibrary_open_tmd2725...");

            state->priv = amsLibrary_open_tmd2725(hwid,
                                                  state->port_handle_ptr,
                                                  &state->calibration_data_tmd2725,
                                                  amsLibraryTmd2725[hwid],
                                                  &state->als_default_config_data,
                                                  &state->prox_default_config_data);

            if (state->priv != NULL){
              AMS_DEV_PRINTF(LOW, this, "AMS: ams_sensor_notify_event: amsLibrary_open_tmd2725 SUCCESS");
              state->tmd2725_hw_is_present = true;
              tmd2725_publish_available(this); // mlora: moving line here to publish asap
            } else {
              SNS_PRINTF(ERROR, this, "AMS: ams_sensor_notify_event: amsLibrary_open_tmd2725 ERROR");
              rv = SNS_RC_INVALID_LIBRARY_STATE;
            }

            /**------------------Power Down and Close COM Port--------------------*/
            state->scp_service->api->sns_scp_update_bus_power(
                                                        state->com_port_info.port_handle,
                                                        false);

            state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
            state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
            /* mustn't use the com port any more so make our handles etc. invalid */
            AMS_DEV_PRINTF(LOW, this, "AMS: clear AMS_PORT_portHndl");
            state->com_port_info.port_handle = NULL;
            state->port_handle.scp_handle = NULL;
            state->port_handle_ptr = NULL;

            /**----------------------Turn Power Rail OFF--------------------------*/
            state->rail_config.rail_vote = SNS_RAIL_OFF;
            state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                                     this,
                                                                     &state->rail_config,
                                                                     NULL);

            if(state->tmd2725_hw_is_present)
            {
                SNS_PRINTF(MED, this, "AMS: HW present ");
                //tmd2725_publish_available(this);
            }
            else
            {
              rv = SNS_RC_INVALID_LIBRARY_STATE;
              SNS_PRINTF(MED, this, "AMS: HW absent");
            }
            state->power_rail_pend_state = AMS_POWER_RAIL_PENDING_NONE;
          }
          else if(state->power_rail_pend_state == AMS_POWER_RAIL_PENDING_SET_CLIENT_REQ)
          {
            AMS_DEV_PRINTF(MED, this, "AMS: ams_sensor_notify_event: _POWER_RAIL_PENDING_SET_CLIENT_REQ");

            sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
            AMS_DEV_PRINTF(MED,this, "AMS: ams_sensor_notify_event: shared instance %p", instance);
            if(NULL != instance)
            {
              ams_instance_state *inst_state = (ams_instance_state*) instance->state->state;
              inst_state->instance_is_ready_to_configure = true;
              AMS_DEV_PRINTF(MED,this, "AMS: ams_sensor_notify_event: set instance_is_ready_to_configure to true");

              if(inst_state->new_cal_reset_request)
              {
                  process_cal_reset(this, instance);
                  inst_state->client_present++;
                  inst_state->new_cal_reset_request = false;
              }
              else
              {
                  tmd2725_reval_instance_config(this, instance, state->sensor_type);
                  if(inst_state->new_self_test_request)
                  {
                    tmd2725_set_self_test_inst_config(this, instance);
                  }
              }
            }
            state->power_rail_pend_state = AMS_POWER_RAIL_PENDING_NONE;
          }
          else if(AMS_POWER_RAIL_PENDING_OFF == state->power_rail_pend_state)
          {
            sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
            /*Not required to turn OFF power rail, If instance is present*/
            if(NULL == instance)
            {
              if(SNS_RAIL_OFF != state->rail_config.rail_vote)
              {
                state->rail_config.rail_vote = SNS_RAIL_OFF;
                state->pwr_rail_service->api->
                sns_vote_power_rail_update(state->pwr_rail_service, this,
                                           &state->rail_config, NULL);
              }
            }
            state->power_rail_pend_state = AMS_POWER_RAIL_PENDING_NONE;
          }
        }
        else
        {
          AMS_DEV_PRINTF(HIGH, this, "pb_decode error");
        }
      }

      event = state->timer_stream->api->get_next_input(state->timer_stream);
    }
  }

  if(NULL != state->reg_data_stream)
  {
    event = state->reg_data_stream->api->peek_input(state->reg_data_stream);
    while(NULL != event)
    {
      ams_sensor_process_registry_event(this, event);
      if(state->registry_cfg_received && state->registry_placement_received)
      {
        ams_publish_registry_attributes(this);
      }

      event = state->reg_data_stream->api->get_next_input(state->reg_data_stream);
    }
  }

  /**---------------------Register Power Rails --------------------------*/
  if(0 != sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid))
     && state->registry_pf_cfg_received
     && state->registry_cfg_received
     && state->registry_placement_received)
  {
    state->registry_pf_cfg_received = false;
    /**-----------------Register and Open COM Port-------------------------*/
    if(NULL == state->com_port_info.port_handle)
    {
      AMS_DEV_PRINTF(HIGH, this, "AMS: ams_sensor_notify_event: .port_handle is null");
      rv = state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                                                              &state->com_port_info.port_handle);
      AMS_DEV_PRINTF(HIGH, this, "AMS: ams_sensor_notify_event: .port_handle %p, set AMS_PORT_portHndl", state->com_port_info.port_handle);

      if(SNS_RC_SUCCESS == rv)
      {
        rv = state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);
        if(SNS_RC_SUCCESS == rv)
        {
          AMS_DEV_PRINTF(HIGH, this, "AMS: tmd2725_sensor_notify_event: sns_scp_open SUCCESS");
          /* set our wrapped-up handle for this port */
          state->port_handle.scp_handle = state->com_port_info.port_handle;
          state->port_handle_ptr = &state->port_handle;
        }
      }
    }
    AMS_DEV_PRINTF(HIGH, this, "AMS: ams_sensor_notify_event: register power rails");
    state->rail_config.rail_vote = SNS_RAIL_OFF;

    state->pwr_rail_service =
     (sns_pwr_rail_service*)service_mgr->get_service(service_mgr,
                                                     SNS_POWER_RAIL_SERVICE);

    state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service,
                                                           &state->rail_config);

    /**---------------------Turn Power Rails ON----------------------------*/
    AMS_DEV_PRINTF(HIGH, this, "AMS: ams_sensor_notify_event: state->tmd2725_hw_is_present %d", state->tmd2725_hw_is_present);
    state->rail_config.rail_vote = SNS_RAIL_ON_NPM;
    AMS_DEV_PRINTF(HIGH, this, "AMS: ams_sensor_notify_event: .rail_vote %d", state->rail_config.rail_vote);
    state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                             this,
                                                             &state->rail_config,
                                                             &on_timestamp);

    /**-------------Create a Timer stream for Power Rail ON timeout.---------*/
    AMS_DEV_PRINTF(HIGH, this, "AMS: ams_sensor_notify_event: state->tmd2725_hw_is_present %d", state->tmd2725_hw_is_present);
    if(NULL == state->timer_stream)
    {
      AMS_DEV_PRINTF(HIGH, this, "AMS: ams_sensor_notify_event: .timer_stream is null");
      stream_svc->api->create_sensor_stream(stream_svc, this, state->timer_suid,
                                            &state->timer_stream);
      AMS_DEV_PRINTF(HIGH, this, "AMS: ams_sensor_notify_event: .timer_stream %p", state->timer_stream);
      if(NULL != state->timer_stream)
      {
        AMS_DEV_PRINTF(HIGH, this, "AMS: ams_sensor_notify_event: power rail timer start");

        tmd2725_start_power_rail_timer(this,
                                  sns_convert_ns_to_ticks(AMS_DEVICE_OFF_TO_IDLE_MS * 1000 * 1000),
                                  AMS_POWER_RAIL_PENDING_INIT);
      }
    }
  }
  return rv;
}

sns_rc tmd2725_streaming_sensor_notify_event(sns_sensor *const this)
{
  ams_state *state = (ams_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_svc = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
  sns_time on_timestamp;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_sensor_event *event;
  ams_deviceCtx_t *amsLibraryTmd2725[TMD2725_NUM_DEVICES] = { NULL, NULL};
  int32_t hwid = this->cb->get_registration_index(this);

  AMS_DEV_PRINTF(HIGH, this, "AMS: ams_streaming_sensor_notify_event: enter: this %p, state %p, fw_stream %p, timer_stream %p, pwr_rail_service %p",
             this, state, state->fw_stream, state->timer_stream, state->pwr_rail_service);

  if(state->fw_stream)
  {
    if((0 == sns_memcmp(&state->irq_suid, &((sns_sensor_uid){{0}}), sizeof(state->irq_suid)))
     || (0 == sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid)))
     || (0 == sns_memcmp(&state->reg_suid, &((sns_sensor_uid){{0}}), sizeof(state->reg_suid)))
    )
    {
      tmd2725_process_suid_events(this);
            // place a request to registry sensor
      if((0 != sns_memcmp(&state->reg_suid, &((sns_sensor_uid){{0}}), sizeof(state->reg_suid))))
      {
          AMS_DEV_PRINTF(MED, this, "streaming: Registry SUID not null");
        if(state->reg_data_stream == NULL)
        {
            AMS_DEV_PRINTF(MED, this, "streaming: create_sensor_stream for registry");

          stream_svc->api->create_sensor_stream(stream_svc,
              this, state->reg_suid , &state->reg_data_stream);

          tmd2725_sensor_send_registry_request(this, registry_platform_config_names[hwid]);
          tmd2725_sensor_send_registry_request(this, registry_platform_placement_names[hwid]);
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
          tmd2725_sensor_send_registry_request(this, registry_als_config_names[hwid]);
          tmd2725_sensor_send_registry_request(this, registry_als_fac_cal_names[hwid]);
          tmd2725_sensor_send_registry_request(this, registry_als_custom_cal_names[hwid]);
          tmd2725_sensor_send_registry_request(this, registry_als_default_config_names[hwid]);
          tmd2725_sensor_send_registry_request(this, registry_als_cal_target_names[hwid]);
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
          tmd2725_sensor_send_registry_request(this, registry_prox_config_names[hwid]);
          tmd2725_sensor_send_registry_request(this, registry_prox_fac_cal_names[hwid]);
          tmd2725_sensor_send_registry_request(this, registry_prox_proxOffset_hWcal_names[hwid]);
          tmd2725_sensor_send_registry_request(this, registry_prox_default_config_names[hwid]);
#endif
        }
      }
    }
  }

  /**----------------------Handle a Timer Sensor event.-------------------*/
  if(NULL != state->timer_stream)
  {
    AMS_DEV_PRINTF(MED, this, "AMS: ams_streaming_sensor_notify_event: timer_stream");

    event = state->timer_stream->api->peek_input(state->timer_stream);

    while(NULL != event)
    {
      pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                   event->event_len);
      sns_timer_sensor_event timer_event;
      if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
      {
        if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {
          if(state->power_rail_pend_state == AMS_POWER_RAIL_PENDING_INIT)
          {
            /**-------------------Read and Confirm WHO-AM-I------------------------*/
            AMS_DEV_PRINTF(MED, this, "AMS: ams_streaming_sensor_notify_event: try amsLibrary_open_tmd2725...");

            state->priv = amsLibrary_open_tmd2725(hwid,
                                                  state->port_handle_ptr,
                                                  &state->calibration_data_tmd2725,
                                                  amsLibraryTmd2725[hwid],
                                                  &state->als_default_config_data,
                                                  &state->prox_default_config_data);


            if (state->priv != NULL){
              AMS_DEV_PRINTF(MED, this, "AMS: ams_streaming_sensor_notify_event: amsLibrary_open_tmd2725 SUCCESS");
              state->tmd2725_hw_is_present = true;
              tmd2725_publish_available(this); // mlora: moving line here to publish asap //
              AMS_DEV_PRINTF(HIGH, this, "AMS: ams_sensor_notify_event: state->tmd2725_hw_is_present %d", state->tmd2725_hw_is_present);
            } else {
              AMS_DEV_PRINTF(HIGH, this, "AMS: ams_streaming_sensor_notify_event: amsLibrary_open_tmd2725 HIGH");
              /* TODO: we need to free some things here */
              rv = SNS_RC_INVALID_LIBRARY_STATE;
              //free(state->priv);
            }
            /**------------------Power Down and Close COM Port--------------------*/
            state->scp_service->api->sns_scp_update_bus_power(
                                                        state->com_port_info.port_handle,
                                                        false);

            state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
            state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
            /* mustn't use the com port any more so make our handles etc. invalid */
            AMS_DEV_PRINTF(HIGH, this, "AMS: clear AMS_PORT_portHndl");
            state->com_port_info.port_handle = NULL;
            state->port_handle.scp_handle = NULL;
            state->port_handle_ptr = NULL;

            /**----------------------Turn Power Rail OFF--------------------------*/
            AMS_DEV_PRINTF(HIGH, this, "AMS: ams_streaming_sensor_notify_event: _RAIL_PENDING_INIT timer:  vote for SNS_RAIL_OFF");
            state->rail_config.rail_vote = SNS_RAIL_OFF;
            state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                                     this,
                                                                     &state->rail_config,
                                                                     NULL);

            if(state->tmd2725_hw_is_present)
            {
              AMS_DEV_PRINTF(HIGH, this, "AMS: HW present");
              //tmd2725_publish_available(this);
            }
            else
            {
              rv = SNS_RC_INVALID_LIBRARY_STATE;
              AMS_DEV_PRINTF(HIGH, this, "AMS: HW absent");
            }
            state->power_rail_pend_state = AMS_POWER_RAIL_PENDING_NONE;
          }
          else if(state->power_rail_pend_state == AMS_POWER_RAIL_PENDING_SET_CLIENT_REQ)
          {
            AMS_DEV_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: _POWER_RAIL_PENDING_SET_CLIENT_REQ");

            sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
            AMS_DEV_PRINTF(MED,this, " AMS: ams_streaming_sensor_notify_event: shared instance %p", instance);
            if(NULL != instance)
            {
              ams_instance_state *inst_state =
                 (ams_instance_state*) instance->state->state;
              inst_state->instance_is_ready_to_configure = true;
              AMS_DEV_PRINTF(MED,this, "AMS: ams_sensor_notify_event: set instance_is_ready_to_configure to true");
              if(inst_state->new_cal_reset_request)
              {
                  process_cal_reset(this, instance);
                  inst_state->client_present++;
                  inst_state->new_cal_reset_request = false;
              }
              else
              {
                  tmd2725_reval_instance_config(this, instance, state->sensor_type);
                  if(inst_state->new_self_test_request)
                  {
                    tmd2725_set_self_test_inst_config(this, instance);
                  }
              }
            }
            state->power_rail_pend_state = AMS_POWER_RAIL_PENDING_NONE;
          }
          else if(AMS_POWER_RAIL_PENDING_OFF == state->power_rail_pend_state)
          {
            sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
            /*Not required to turn OFF power rail, If instance is present*/
            if(NULL == instance)
            {
              if(SNS_RAIL_OFF != state->rail_config.rail_vote)
              {
                state->rail_config.rail_vote = SNS_RAIL_OFF;
                state->pwr_rail_service->api->
                sns_vote_power_rail_update(state->pwr_rail_service, this,
                                           &state->rail_config, NULL);
              }
            }
            state->power_rail_pend_state = AMS_POWER_RAIL_PENDING_NONE;
          }
        }
        else
        {
          AMS_DEV_PRINTF(HIGH, this, "pb_decode error");
        }
      }
      event = state->timer_stream->api->get_next_input(state->timer_stream);
    }
  }
  if(NULL != state->reg_data_stream)
  {
    event = state->reg_data_stream->api->peek_input(state->reg_data_stream);
    while(NULL != event)
    {
      ams_sensor_process_registry_event(this, event);
      if(state->registry_cfg_received && state->registry_placement_received)
      {
        ams_publish_registry_attributes(this);
      }

      event = state->reg_data_stream->api->get_next_input(state->reg_data_stream);
    }
  }

  /**---------------------Register Power Rails --------------------------*/
  if(0 != sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid))
     && state->registry_pf_cfg_received
     && state->registry_cfg_received
     && state->registry_placement_received)
  {
    state->registry_pf_cfg_received = false;

    /**-----------------Register and Open COM Port-------------------------*/
    AMS_DEV_PRINTF(HIGH, this, " AMS: ams_sensor_notify_event: state->tmd2725_hw_is_present %d", state->tmd2725_hw_is_present);
    if((NULL == state->com_port_info.port_handle)/*  && (state->tmd2725_hw_is_present)*/)
    {
      AMS_DEV_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: .port_handle is null");
      rv = state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                                                              &state->com_port_info.port_handle);
      AMS_DEV_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: .port_handle %p, set AMS_PORT_portHndl", state->com_port_info.port_handle);

      if(rv == SNS_RC_SUCCESS)
      {
        rv = state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);
        /* set our wrapped-up handle for this port */
        state->port_handle.scp_handle = state->com_port_info.port_handle;
        state->port_handle_ptr = &state->port_handle;
      }
    }
    AMS_DEV_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: register power rails");
    state->rail_config.rail_vote = SNS_RAIL_OFF;

    state->pwr_rail_service =
     (sns_pwr_rail_service*)service_mgr->get_service(service_mgr,
                                                     SNS_POWER_RAIL_SERVICE);

    state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service,
                                                           &state->rail_config);

    /**---------------------Turn Power Rails ON----------------------------*/
    AMS_DEV_PRINTF(HIGH, this, " AMS: ams_sensor_notify_event: state->tmd2725_hw_is_present %d", state->tmd2725_hw_is_present);
    AMS_DEV_PRINTF(HIGH, this, "ams_streaming_sensor_notify_event: registry cfg sections rcvd:  vote for SNS_RAIL_ON_NPM");
    state->rail_config.rail_vote = SNS_RAIL_ON_NPM;
    AMS_DEV_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: .rail_vote %d", state->rail_config.rail_vote);
    state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                             this,
                                                             &state->rail_config,
                                                             &on_timestamp);

    /**-------------Create a Timer stream for Power Rail ON timeout.---------*/
    AMS_DEV_PRINTF(HIGH, this, " AMS: ams_sensor_notify_event: state->tmd2725_hw_is_present %d", state->tmd2725_hw_is_present);

    if(NULL == state->timer_stream)
    {
      AMS_DEV_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: .timer_stream is null");
      stream_svc->api->create_sensor_stream(stream_svc, this, state->timer_suid,
                                            &state->timer_stream);
      AMS_DEV_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: .timer_stream %p", state->timer_stream);
      if(NULL != state->timer_stream)
      {
        AMS_DEV_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: power rail timer start");
        tmd2725_start_power_rail_timer(this,
                                  sns_convert_ns_to_ticks(AMS_DEVICE_OFF_TO_IDLE_MS * 1000 * 1000),
                                  AMS_POWER_RAIL_PENDING_INIT);
      }
    }
  }
  return rv;
}

static bool
tmd2725_write_to_prox_fac_cal(struct pb_ostream_s *stream,
    struct pb_field_s const *field, void *const *arg)
{
  sns_sensor *this = (sns_sensor*) *arg;
  ams_state *state = (ams_state *)this->state->state;
  ams_calibrationData_t *cfg = &state->calibration_data_tmd2725;
  uint32_t *new_version = state->cal_version;

  char const *names[] = {"near_threshold", "far_threshold"};

  for(int i = 0; i < sizeof(names)/sizeof(names[0]) ; i++)
  {
    pb_buffer_arg name_data = (pb_buffer_arg)
        { .buf = names[i], .buf_len = strlen(names[i]) + 1 };

    sns_registry_data_item pb_item = sns_registry_data_item_init_default;
    pb_item.name.funcs.encode = &pb_encode_string_cb;
    pb_item.name.arg = &name_data;

    /* ---------------------------------------------------------------------------- */
    /* Near threshold */
    /* ---------------------------------------------------------------------------- */
    if (0==strncmp(names[i], "near_threshold", sizeof("near_threshold")))
    {
      pb_item.has_version = true;
      pb_item.version = new_version[PROXHARDTHRESHOLD_IDX];
      pb_item.has_flt = true;
      /* threshold data is stored in the registry as an 8-bit value */
      pb_item.flt = (float)(cfg->proxHardThreshold >> 8);
      AMS_PRINTF(HIGH, this,"cfg->proxHardThreshold: %d", cfg->proxHardThreshold);

    }

    /* ---------------------------------------------------------------------------- */
    /* Far threshold */
    /* ---------------------------------------------------------------------------- */
    else if (0==strncmp(names[i], "far_threshold", sizeof("far_threshold")))
    {
      pb_item.has_version = true;
      pb_item.version = new_version[PROXHARDTHRESHOLD_IDX];
      pb_item.has_flt = true;
      /* threshold data is stored in the registry as an 8-bit value */
      pb_item.flt = (float)(cfg->proxMinThreshold >> 8);
      AMS_PRINTF(HIGH, this,"cfg->proxMinThreshold: %d", cfg->proxMinThreshold);
    }

    if(!pb_encode_tag_for_field(stream, field))
    {
      SNS_PRINTF(ERROR, this,"Error encoding tag item: %d", i);

      return false;
    }

    if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
    {
      SNS_PRINTF(ERROR, this,"Error encoding item: %d", i);
      return false;
    }
  }

  AMS_DEV_PRINTF(HIGH, this, "tmd2725_write_to_prox_fac_cal: done");

  return true;
}

static bool
tmd2725_write_to_prox_proxOffset_hWcal(struct pb_ostream_s *stream,
                                       struct pb_field_s const *field, void *const *arg)
{
    sns_sensor *this = (sns_sensor*) *arg;

    ams_state *state = (ams_state *)this->state->state;
    ams_calibrationData_t *cfg = &state->calibration_data_tmd2725;
    uint32_t *new_version = state->cal_version;

    char const *names[] = {"proxOffset",
                           "proxOffsetSign"
    };

    for(int i = 0; i < sizeof(names)/sizeof(names[0]) ; i++)
    {
        pb_buffer_arg name_data = (pb_buffer_arg)
            { .buf = names[i], .buf_len = strlen(names[i]) + 1 };


        sns_registry_data_item pb_item = sns_registry_data_item_init_default;
        pb_item.name.funcs.encode = &pb_encode_string_cb;
        pb_item.name.arg = &name_data;

        if (0==strncmp(names[i], "proxOffset", sizeof("proxOffset")))
        {
            AMS_DEV_PRINTF(HIGH, this,"cfg->proxOffset: %d", cfg->proxOffset);
            pb_item.has_version = true;
            pb_item.version = new_version[PROXOFFSET_IDX];
            pb_item.has_flt = true;
            pb_item.flt = (float)cfg->proxOffset;
        }
        else if (0==strncmp(names[i], "proxOffsetSign", sizeof("proxOffsetSign")))
        {
            AMS_DEV_PRINTF(HIGH, this,"cfg->proxOffsetSign: %d", cfg->proxOffsetSign);
            pb_item.has_version = true;
            pb_item.version = new_version[PROXOFFSETSIGN_IDX];
            pb_item.has_flt = true;
            pb_item.flt = (float)cfg->proxOffsetSign;
        }

        if(!pb_encode_tag_for_field(stream, field))
        {
            SNS_PRINTF(ERROR, this,"Error encoding tag item: %d", i);
            return false;
        }

        if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
        {
            SNS_PRINTF(ERROR, this,"Error encoding item: %d", i);
            return false;
        }
    }

    AMS_DEV_PRINTF(HIGH, this, "tmd2725_write_to_prox_proxOffset_hWcal: done");

    return true;
}


static bool
tmd2725_write_to_als_fac_cal(struct pb_ostream_s *stream,
    struct pb_field_s const *field, void *const *arg)
{
  sns_sensor *this = (sns_sensor*) *arg;

  ams_state *state = (ams_state *)this->state->state;
  ams_calibrationData_t *cfg = &state->calibration_data_tmd2725;
  uint32_t *new_version = state->cal_version;

  char const *names[] = {"scale"};

  for(int i = 0; i < sizeof(names)/sizeof(names[0]) ; i++)
  {
    pb_buffer_arg name_data = (pb_buffer_arg)
           { .buf = names[i], .buf_len = strlen(names[i]) + 1 };

    sns_registry_data_item pb_item = sns_registry_data_item_init_default;
    pb_item.name.funcs.encode = &pb_encode_string_cb;
    pb_item.name.arg = &name_data;

    if (0==strncmp(names[i], "scale", sizeof("scale")))
    {
      pb_item.has_version = true;
      pb_item.version = new_version[ALSCALIBRATIONFACTOR_IDX];
      pb_item.has_flt = true;
      pb_item.flt = (float)cfg->alsCalibrationFactor;
    }
    if(!pb_encode_tag_for_field(stream, field))
    {
      SNS_PRINTF(ERROR, this,"Error encoding tag item: %d", i);
      return false;
    }

    if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
    {
      SNS_PRINTF(ERROR, this,"Error encoding item: %d", i);
      return false;
    }
  }

  AMS_DEV_PRINTF(HIGH, this, "tmd2725_write_to_als_fac_cal: done");

  return true;
}

static void tmd2725_reset_calibration(sns_sensor_instance *const instance, sns_sensor *const sensor)
{
    ams_instance_state    *instance_state       = (ams_instance_state *)instance->state->state;
    ams_state             *sensor_state         = (ams_state *)sensor->state->state;
    ams_calibrationData_t *instance_calibration = &instance_state->calibration_data_tmd2725;

    /* The cal reset should be applied only to the type of sensor
     * for which it is intended. Therefore, we have to check the sensor type
     * and only reset the factory cal data associated with that type.
     */
    if ((sensor_state->sensor_type == AMS_PROX_OC) || (sensor_state->sensor_type == AMS_PROX_STRM))
    {
        AMS_DEV_PRINTF(HIGH, sensor, "tmd2725_reset_calibration: prox");
        instance_calibration->proxHardThreshold = instance_calibration->proxHardThresholdResetValue;      /* near threshold */
        instance_calibration->proxMinThreshold = instance_calibration->proxMinThresholdResetValue;        /* far threshold */
        instance_state->cal_version[PROXHARDTHRESHOLD_IDX]++;
    }
    if ((sensor_state->sensor_type == AMS_ALS_OC) || (sensor_state->sensor_type == AMS_ALS_STRM))
    {
        AMS_DEV_PRINTF(HIGH, sensor, "tmd2725_reset_calibration: als, instance = %x", instance);
        instance_calibration->alsCalibrationFactor = AMS_ALS_FACTOR;   /* scale */
        instance_state->cal_version[ALSCALIBRATIONFACTOR_IDX]++;
    }

    sensor_state->calibration_data_tmd2725 = instance_state->calibration_data_tmd2725;
    sns_memscpy(sensor_state->cal_version,
                sizeof(sensor_state->cal_version),
                instance_state->cal_version,
                sizeof(instance_state->cal_version));
}

static bool
tmd2725_write_calibration_to_registry(sns_sensor *const this, amsRegistryDataWriteType_t write_type)
{
    //todo how much to define for this buffer
    uint8_t buffer[1000];
    int32_t encoded_len;
    ams_state *sensor_state =  (ams_state *) this->state->state;
    sns_registry_write_req write_req = sns_registry_write_req_init_default;
    int32_t hwid = this->cb->get_registration_index(this);
    if(NULL != sensor_state->reg_data_stream)
    {
      AMS_DEV_PRINTF(HIGH, this, "AMS: enter tmd2725_write_calibration_to_registry");

      /* Should never happen, but just in case */
      if ((hwid > (TMD2725_NUM_DEVICES - 1)) || (hwid < 0))
      {
          SNS_PRINTF(ERROR, this, "AMS: ERROR, hwid %d is out of range", hwid);
          return false;
      }

      if (write_type == AMS_REGISTRY_WRITE_TYPE_NONE)
      {
          /* Shouldn't happen */
          SNS_PRINTF(HIGH, this, "ERROR: unsupported write_type = %d ", write_type);
          return false;
      }

      pb_buffer_arg als_fac_cal_name_data = (pb_buffer_arg)
          { .buf = registry_als_fac_cal_names[hwid],
            .buf_len = strlen (registry_als_fac_cal_names[hwid]) + 1 };

      pb_buffer_arg prox_fac_cal_name_data = (pb_buffer_arg)
          { .buf = registry_prox_fac_cal_names[hwid],
            .buf_len = strlen (registry_prox_fac_cal_names[hwid]) + 1 };

      pb_buffer_arg prox_hwCal_fac_cal_name_data = (pb_buffer_arg)
          { .buf = registry_prox_proxOffset_hWcal_names[hwid],
            .buf_len = strlen (registry_prox_proxOffset_hWcal_names[hwid]) + 1 };


      if (write_type == AMS_REGISTRY_WRITE_TYPE_ALS_FAC_CAL)
      {
          /* ---------------------------------------------------------------------------- */
          /* ALS Factory Cal*/
          /* ---------------------------------------------------------------------------- */
  
          AMS_DEV_PRINTF(HIGH, this,"Encoding .als.fac_cal");
          write_req.name.funcs.encode = &pb_encode_string_cb;
          write_req.name.arg = &als_fac_cal_name_data;
          write_req.data.items.arg = this;
          write_req.data.items.funcs.encode = tmd2725_write_to_als_fac_cal;
      }

      if (write_type == AMS_REGISTRY_WRITE_TYPE_PROX_FAC_CAL)
      {
          /* ---------------------------------------------------------------------------- */
          /* PROX Factory */
          /* ---------------------------------------------------------------------------- */


          AMS_DEV_PRINTF(HIGH, this,"Encoding .prox.fac_cal");
          write_req.name.funcs.encode = &pb_encode_string_cb;
          write_req.name.arg = &prox_fac_cal_name_data;
          write_req.data.items.arg = this;
          write_req.data.items.funcs.encode = tmd2725_write_to_prox_fac_cal;
      }

      if (write_type == AMS_REGISTRY_WRITE_TYPE_PROX_HW_CAL)
      {
          /* ---------------------------------------------------------------------------- */
          /* PROX hwCal */
          /* ---------------------------------------------------------------------------- */

          AMS_DEV_PRINTF(HIGH, this,"Encoding .prox.proxOffset_hWcal");
          write_req.name.funcs.encode = &pb_encode_string_cb;
          write_req.name.arg = &prox_hwCal_fac_cal_name_data;
          write_req.data.items.arg = this;
          write_req.data.items.funcs.encode = tmd2725_write_to_prox_proxOffset_hWcal;
      }

      /* Create encode request */
      encoded_len = pb_encode_request(buffer, sizeof(buffer), &write_req, sns_registry_write_req_fields, NULL);

      /* Send request to write to registry */
      if(0 < encoded_len)
      {
          sns_request request = (sns_request){
              .request_len = encoded_len,
              .request = buffer,
              .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ };
          sensor_state->reg_data_stream->api->send_request(sensor_state->reg_data_stream, &request);
      } else {
          SNS_PRINTF(ERROR, this, "AMS: error sending request");
      }
  }
  return true;
}

static void process_cal_reset(sns_sensor *const this, sns_sensor_instance *const instance)
{
  
  ams_state *state = (ams_state*)this->state->state;
  ams_instance_state *inst_state = (ams_instance_state *) instance->state->state;
  float result = 0;

    /* ------------------------------------------------------------------------------------
    * Reset calibration data
    * -----------------------------------------------------------------------------------*/
  ams_calibrationData_t *instance_calibration = &inst_state->calibration_data_tmd2725;
  SNS_PRINTF(HIGH, this, ": process_cal_reset");

  /* Reset calibration */
  tmd2725_reset_calibration(instance, this);

  /* Update Registry */
  amsRegistryDataWriteType_t write_type = AMS_REGISTRY_WRITE_TYPE_NONE;
  if ((state->sensor_type == AMS_ALS_OC) ||
      (state->sensor_type == AMS_ALS_STRM))
  {
      write_type = AMS_REGISTRY_WRITE_TYPE_ALS_FAC_CAL;
      result = (float)AMS_ALS_FACTOR;
  }
  if ((state->sensor_type == AMS_PROX_OC) ||
      (state->sensor_type == AMS_PROX_STRM))
  {
      write_type = AMS_REGISTRY_WRITE_TYPE_PROX_FAC_CAL;
      result = (float)(instance_calibration->proxHardThreshold>>8);
  }
  tmd2725_write_calibration_to_registry(this, write_type);
  AMS_DEV_PRINTF(HIGH, this, "AMS: ams_set_client_request: instance_is_ready_to_configure = %d", inst_state->instance_is_ready_to_configure);
  ams_set_inst_config(this,
          instance,
          0,
          0,
          SNS_CAL_MSGID_SNS_CAL_RESET);
}

/**
 * Decodes self test requests.
 *
 * @param[i] this              Sensor reference
 * @param[i] request           Encoded input request
 * @param[o] decoded_request   Decoded standard request
 * @param[o] test_config       decoded self test request
 *
 * @return bool True if decoding is successfull else false.
 */
static bool ams_get_decoded_self_test_request(sns_sensor const *this, sns_request const *request,
                                                  sns_std_request *decoded_request,
                                                  sns_physical_sensor_test_config *test_config)
{
  pb_istream_t stream;
  pb_simple_cb_arg arg = { .decoded_struct = test_config,
                           .fields = sns_physical_sensor_test_config_fields };

  decoded_request->payload = (struct pb_callback_s) { .funcs.decode = &pb_decode_simple_cb,
                                                      .arg = &arg };

  stream = pb_istream_from_buffer(request->request, request->request_len);
  if(!pb_decode(&stream, sns_std_request_fields, decoded_request))
  {
    SNS_PRINTF(ERROR, this, "AMS: pb_decode error in ams_get_decoded_self_test_request");
    return false;
  }
  return true;
}

/**
 * Decodes a physical sensor self test request and updates
 * instance state with request info.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 * @param[i] new_request Encoded request
 *
 * @return True if request is valid else false
 */
static bool ams_extract_self_test_info(sns_sensor *this,
                              sns_sensor_instance *instance,
                              struct sns_request const *new_request)
{
  sns_std_request decoded_request = sns_std_request_init_default;
  sns_physical_sensor_test_config test_config = sns_physical_sensor_test_config_init_default;
  ams_state *state = (ams_state*)this->state->state;
  ams_instance_state *inst_state = (ams_instance_state*)instance->state->state;
  ams_self_test_info *self_test_info;

  if (0) { }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  else if (state->sensor_type == AMS_ALS_OC) {
    self_test_info = &inst_state->als_info.test_info[AMS_OC_IDX];
  } else if (state->sensor_type == AMS_ALS_STRM) {
    self_test_info = &inst_state->als_info.test_info[AMS_STRM_IDX];
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  else if (state->sensor_type == AMS_PROX_OC) {
    self_test_info = &inst_state->prox_info.test_info[AMS_OC_IDX];
  } else if (state->sensor_type == AMS_PROX_STRM) {
    self_test_info = &inst_state->prox_info.test_info[AMS_STRM_IDX];
  }
#endif
  else
  {
    SNS_PRINTF(ERROR, this, "AMS: ams_extract_self_test_info: Error: unknown sensor type");
    return false;
  }

  if(ams_get_decoded_self_test_request(this, new_request, &decoded_request, &test_config))
  {
    self_test_info->test_type = test_config.test_type;
    self_test_info->test_client_present = true;
    return true;
  }
  else
  {
    return false;
  }
}

void tmd2725_send_suid_req(sns_sensor *this, char *const data_type,
    uint32_t data_type_len)
{
  uint8_t buffer[50];
  sns_memset(buffer, 0, sizeof(buffer));
  ams_state *state = (ams_state*)this->state->state;
  sns_service_manager *manager = this->cb->get_service_manager(this);
  sns_stream_service *stream_service = (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);
  size_t encoded_len;
  pb_buffer_arg data = (pb_buffer_arg){ .buf = data_type, .buf_len = data_type_len };

  sns_suid_req suid_req = sns_suid_req_init_default;
  suid_req.has_register_updates = true;
  suid_req.register_updates = true;
  suid_req.data_type.funcs.encode = &pb_encode_string_cb;
  suid_req.data_type.arg = &data;

  if(state->fw_stream == NULL)
  {
     stream_service->api->create_sensor_stream(stream_service,
         this, sns_get_suid_lookup(), &state->fw_stream);
  }

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
      &suid_req, sns_suid_req_fields, NULL);
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
      .request_len = encoded_len, .request = buffer, .message_id = SNS_SUID_MSGID_SNS_SUID_REQ };
    state->fw_stream->api->send_request(state->fw_stream, &request);
  }
}

static sns_rc tmd2725_add_new_request(sns_sensor * this,
                                      sns_sensor_instance *instance,
                                      ams_state *sensor_state,
                                      struct sns_request const *new_request)
{
   sns_rc rc = SNS_RC_SUCCESS;

   sns_std_request decoded_request;
   sns_std_sensor_config decoded_payload = {0};

   instance->cb->add_client_request(instance, new_request); 
   SNS_PRINTF(HIGH, this, ": tmd2725_add_new_request. new_request msg_id=%d sensor=%u", 
              new_request->message_id, sensor_state->sensor_type);

    if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG == new_request->message_id)
    {
       if (AMS_ALS_STRM == sensor_state->sensor_type && 
           ams_get_decoded_std_request(this, new_request, &decoded_request, &decoded_payload)) {
           if ((decoded_payload.sample_rate <= 0.0f ) || (decoded_payload.sample_rate > AMS_DEVICE_ODR_10 )){
               rc = SNS_RC_INVALID_VALUE;
               SNS_PRINTF(ERROR, this, ": tmd2725_add_new_request. sensor=%u SR=%d",
               sensor_state->sensor_type, (int)decoded_payload.sample_rate);
           }
       }
       else if ( AMS_PROX_STRM == sensor_state->sensor_type && 
                ams_get_decoded_std_request(this, new_request, &decoded_request, &decoded_payload)){
          if ((decoded_payload.sample_rate <= 0.0f ) || (decoded_payload.sample_rate > AMS_DEVICE_ODR_20 )){
              rc = SNS_RC_INVALID_VALUE;
              SNS_PRINTF(ERROR, this, ": tmd2725_add_new_request. sensor=%u SR=%d",
              sensor_state->sensor_type, (int)decoded_payload.sample_rate);
          }
       }
       else {
           rc = SNS_RC_NOT_SUPPORTED;
       }
    }
    else if (SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG == new_request->message_id) {
       if ((AMS_ALS_OC != sensor_state->sensor_type) && (AMS_PROX_OC != sensor_state->sensor_type)) {
           rc = SNS_RC_NOT_SUPPORTED;
           SNS_PRINTF(ERROR, this, "ERROR: tmd2725_add_new_request. SNS_STD_ON_CHANGE_CONFIG. Not supported" );
       }
    }
    else if (SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG ==
          new_request->message_id) {
       if ((AMS_ALS_OC != sensor_state->sensor_type &&
            AMS_PROX_OC != sensor_state->sensor_type &&
            AMS_ALS_STRM != sensor_state->sensor_type &&
            AMS_PROX_STRM != sensor_state->sensor_type) ||
           (!ams_extract_self_test_info(this, instance, new_request)))
            {
               rc = SNS_RC_NOT_SUPPORTED;
               SNS_PRINTF(ERROR, this, "ERROR: tmd2725_add_new_request. SNS_PHYSICAL_SENSOR_TEST_CONFIG. Not supported" );
       }
    }
    else if (SNS_CAL_MSGID_SNS_CAL_RESET == new_request->message_id) {
        if (AMS_ALS_OC != sensor_state->sensor_type &&
            AMS_PROX_OC != sensor_state->sensor_type &&
            AMS_ALS_STRM != sensor_state->sensor_type &&
            AMS_PROX_STRM != sensor_state->sensor_type) {

               rc = SNS_RC_NOT_SUPPORTED;
               SNS_PRINTF(ERROR, this, "ERROR: tmd2725_add_new_request. SNS_CAL_RESET. Not supported" );
        }
    }
    else
    {
       SNS_PRINTF(ERROR, this, "ERROR: tmd2725_add_new_request. Not a supported request type" );
       rc = SNS_RC_NOT_SUPPORTED;
    }

    if ( rc != SNS_RC_SUCCESS) {
       instance->cb->remove_client_request(instance, new_request);
       SNS_PRINTF(ERROR, this, "ERROR: req rejec, rc=%d", rc);
    }
    return rc;
}



/** See sns_ams_sensor.h */
sns_sensor_instance* tmd2725_set_client_request(sns_sensor *const this,
                                                struct sns_request const *exist_request,
                                                struct sns_request const *new_request,
                                                bool remove)
{
  sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
  sns_sensor_instance *curr_instance = NULL;
  ams_state *state = (ams_state*)this->state->state;
  sns_time on_timestamp;
  sns_time delta;
  bool reval_config = false;
  int32_t hwid = this->cb->get_registration_index(this);

  /* Debug - pbnt @ Qualcomm */
  /* ams_deviceCtx_t *devCtx = (ams_deviceCtx_t *)(state->priv); */
  /* AMS_DEV_PRINTF(HIGH, this, "tmd2725_set_client_request: devCtx->ccbAlsCtx.initData.calibrationData.luxTarget %d", devCtx->ccbAlsCtx.initData.calibrationData.luxTarget); */
  /* AMS_DEV_PRINTF(HIGH, this, "tmd2725_set_client_request: devCtx->ccbAlsCtx.initData.calibrationData.luxTargetError %d", devCtx->ccbAlsCtx.initData.calibrationData.luxTargetError); */
  /* Debug - pbnt @ Qualcomm */

  if (remove)
  {
      SNS_PRINTF(HIGH, this
                 , "ams_set_client_request: enter: REMOVE: sensor_this 0x%X, exist_rq 0x%X, new_rq 0x%X; inst 0x%X"
                 , this, exist_request, new_request, instance);
  } else {
      SNS_PRINTF(HIGH, this
                 , "ams_set_client_request: enter: ADD/CHANGE: sensor_this 0x%X, exist_rq 0x%X, new_rq 0x%X; inst 0x%X"
                 , this, exist_request, new_request, instance);
  }

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  sns_sensor_uid * p_als_oc_suid = (sns_sensor_uid *)tmd2725_get_suid_with_sensortype_and_hwid(AMS_ALS_OC, hwid);
  sns_sensor_uid * p_als_st_suid = (sns_sensor_uid *)tmd2725_get_suid_with_sensortype_and_hwid(AMS_ALS_STRM, hwid);
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX
  sns_sensor_uid * p_prox_oc_suid = (sns_sensor_uid *)tmd2725_get_suid_with_sensortype_and_hwid(AMS_PROX_OC, hwid);
  sns_sensor_uid * p_prox_st_suid = (sns_sensor_uid *)tmd2725_get_suid_with_sensortype_and_hwid(AMS_PROX_STRM, hwid);
#endif

  if(remove)
  {
    if(NULL != instance)
    {
      ams_instance_state *inst_state = (ams_instance_state *) instance->state->state;
      ams_deviceCtx_t *ctx = (ams_deviceCtx_t *)(inst_state->priv);
      ams_ccb_als_ctx_t *alsCcbCtx = &((ams_deviceCtx_t*)ctx)->ccbAlsCtx;
      ams_ccb_proximity_ctx_t *prxCcbCtx = &((ams_deviceCtx_t *)ctx)->ccbProxCtx;
      instance->cb->remove_client_request(instance, exist_request);

      if (exist_request->message_id !=
          SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
      {
        inst_state->client_present--;
        SNS_PRINTF(HIGH, this, "ams_set_client_request: client_present--");
      }


      if (exist_request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
      {
        /* In this case we received a remove event from the framework.
         * If the removal was for a test, then we need to check to see
         * whether or not we should write to the registry. The test results
         * were processed in the sensor instance and stored in the instance
         * state.
         */
          SNS_PRINTF(HIGH, this, "ams_set_client_request: removing  SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG");
          AMS_DEV_PRINTF(HIGH, this, "ams_set_client_request: need_to_update_registry %d", inst_state->need_to_update_registry );

          /* Check the instance state to see if there is a need to update the registry */
          if (inst_state->need_to_update_registry == true)
          {
              sns_sensor *iter;
              ams_instance_state *inst_state = (ams_instance_state*)instance->state->state;
              for(iter = this->cb->get_library_sensor(this, true);
                  NULL != iter;
                  iter = this->cb->get_library_sensor(this, false))
              {
                  ams_state *sensor_state = (ams_state*)iter->state->state;

                  sensor_state->calibration_data_tmd2725 = inst_state->calibration_data_tmd2725;
                  sns_memscpy(sensor_state->cal_version,
                              sizeof(sensor_state->cal_version),
                              inst_state->cal_version,
                              sizeof(inst_state->cal_version));
              }

              amsRegistryDataWriteType_t write_type = inst_state->registry_write_type;
              tmd2725_write_calibration_to_registry(this, write_type);
              inst_state->need_to_update_registry = false;
          }
          if(alsCcbCtx->wait_for_clear)
          {
            ctx->shadowEnableReg |= (AEN);
            ams_setEnable(ctx);
            alsCcbCtx->wait_for_clear = false;
          }
          if(prxCcbCtx->wait_for_clear)
          {
              ctx->shadowEnableReg |= (PEN);
              ams_setEnable(ctx);
              prxCcbCtx->wait_for_clear = false;
          }
          //inst_state->publish_sensors_prev = 0;
          inst_state->resume_op = 1;
          AMS_DEV_PRINTF(HIGH, this, "ams_set_client_request: set wait_for_clear to false");
      }

      inst_state->new_or_chng_req = true;
      tmd2725_reval_instance_config(this, instance, state->sensor_type);
    }
  }
  else if(NULL != new_request)
  {
    if ((NULL == instance) && (new_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ))
    {
        // no op
        SNS_PRINTF(HIGH, this, "ams_set_client_request: SNS_STD_MSGID_SNS_STD_FLUSH_REQ, no-op for TMD2725");
    }
    else
    {
      // 1. If new request then:
      //     a. Power ON rails.
      //     b. Power ON COM port - Instance must handle COM port power.
      //     c. Create new instance.
      //     d. Re-evaluate existing requests and choose appropriate instance config.
      //     e. set_client_config for this instance.
      //     f. Add new_request to list of requests handled by the Instance.
      //     g. Power OFF COM port if not needed- Instance must handle COM port power.
      //     h. Return the Instance.
      // 2. If there is an Instance already present:
      //     a. Add new_request to list of requests handled by the Instance.
      //     b. Remove exist_request from list of requests handled by the Instance.
      //     c. Re-evaluate existing requests and choose appropriate Instance config.
      //     d. set_client_config for the Instance if not the same as current config.
      //     e. publish the updated config.
      //     f. Return the Instance.
      // 3.  If "flush" request:
      //     a. Perform flush on the instance.
      //     b. Return NULL.

      if(NULL == instance)
      {
        SNS_PRINTF(HIGH, this, "ams_set_client_request: no instance:  vote for SNS_RAIL_ON_NPM");
        state->rail_config.rail_vote = SNS_RAIL_ON_NPM;
        state->pwr_rail_service->api->sns_vote_power_rail_update(
                                             state->pwr_rail_service,
                                             this,
                                             &state->rail_config,
                                             &on_timestamp);

        delta = sns_get_system_time() - on_timestamp;

        // Use on_timestamp to determine correct Timer value.
        if(delta < sns_convert_ns_to_ticks(AMS_DEVICE_OFF_TO_IDLE_MS*1000*1000))
        {
          SNS_PRINTF(HIGH, this, "reconfig power rail timer");
          tmd2725_start_power_rail_timer(this,
                                    sns_convert_ns_to_ticks(AMS_DEVICE_OFF_TO_IDLE_MS*1000*1000) - delta,
                                    AMS_POWER_RAIL_PENDING_SET_CLIENT_REQ);
        }
        else
        {
          SNS_PRINTF(HIGH, this, "power rail timer already configed");
          // rail is already ON
          reval_config = true;
        }

        AMS_DEV_PRINTF(HIGH, this, "AMS: ams_set_client_request: null instance, call create_instance");

        /** create_instance() calls init() for the Sensor Instance */
        instance = this->cb->create_instance(this,
                                             sizeof(ams_instance_state));
        SNS_PRINTF(HIGH, this, "AMS: ams_set_client_request: create_instance SUCCESS %p", instance);
        ams_instance_state *inst_state = (ams_instance_state *) instance->state->state;
        inst_state->new_or_chng_req = true;
        /* If rail is already ON then flag instance OK to configure */
        if(reval_config)
        {
          AMS_DEV_PRINTF(HIGH, this, "ams_set_client_request: reval_config is TRUE");
          inst_state->instance_is_ready_to_configure = true;
          AMS_DEV_PRINTF(MED,this, "AMS: ams_sensor_notify_event: set instance_is_ready_to_configure to true");
        }
        else
        {
            AMS_DEV_PRINTF(HIGH, this, "ams_set_client_request: reval_config is FALSE");
            inst_state->instance_is_ready_to_configure = false;
            AMS_DEV_PRINTF(MED,this, "AMS: ams_sensor_notify_event: set instance_is_ready_to_configure to false");
        }
      }
      else
      {
        if(new_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
        {
            SNS_PRINTF(HIGH, this, "AMS: ams_set_client_request: SNS_STD_MSGID_SNS_STD_FLUSH_REQ");
            tmd2725_send_fifo_flush_done (instance, &state->my_suid);
            return instance;
        }
        else
        {
          reval_config = true;

          SNS_PRINTF(HIGH, this, "AMS: ams_set_client_request: instance %p; new or chg rqst", instance);

          ams_instance_state *inst_state = (ams_instance_state *) instance->state->state;
          inst_state->new_or_chng_req = true;

          /** An existing client is changing request*/
          if((NULL != exist_request) && (NULL != new_request))
          {
            AMS_DEV_PRINTF(HIGH, this, "AMS: ams_set_client_request: chg rqst");
            instance->cb->remove_client_request(instance, exist_request);
          }
          /** A new client sent new_request*/
          else if(NULL != new_request)
          {
            // No-op. new_request will be added to requests list below.
          }
        }
      }

      /** Add the new request to list of client_requests.*/
      if(NULL != instance)
      {
        ams_instance_state *inst_state = (ams_instance_state*)instance->state->state;
        if((NULL != new_request) && (new_request->message_id != SNS_STD_MSGID_SNS_STD_FLUSH_REQ))
        {
          SNS_PRINTF(HIGH, this, "AMS: ams_set_client_request: adding new request"); 
          sns_rc add_new_req_rc = tmd2725_add_new_request(this, instance, state, new_request);
          if(SNS_RC_SUCCESS != add_new_req_rc)
          {
             if (NULL != exist_request) {
               SNS_PRINTF(HIGH, this, "restoring existing req");
               instance->cb->add_client_request(instance, exist_request);
          }
          curr_instance = instance;
          instance = NULL; // no instance is handling this invalid request
         }
         if (NULL != instance) {

             if(new_request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
             {
                 SNS_PRINTF(HIGH, this, "ams_set_client_request: %p type SNS_PHYSICAL_SENSOR_TEST_CONFIG", new_request);
                 SNS_PRINTF(HIGH, this, "AMS: ams_set_client_request: is selftest rqst; reval_config=%d, instance_is_ready_to_configure = %d",
                            reval_config,inst_state->instance_is_ready_to_configure);
                 inst_state->new_self_test_request = true;
                 inst_state->sensor_type_under_test = state->sensor_type;
             }
             else if(new_request->message_id == SNS_CAL_MSGID_SNS_CAL_RESET)
             {
                 SNS_PRINTF(HIGH, this, "ams_set_client_request: %p type SNS_CAL_MSGID_SNS_CAL_RESET", new_request);
                 if(inst_state->instance_is_ready_to_configure)
                 {
                     process_cal_reset(this, instance);
                     reval_config = 0;
                     inst_state->client_present++;
                 }
                 else
                 {
                     inst_state->new_cal_reset_request = true;
                 }
             }
             else {
               inst_state->client_present++;
               inst_state->new_client_flag = 1;
             }
          }
        }

        if((NULL != instance) && reval_config && inst_state->instance_is_ready_to_configure)
        {
          AMS_DEV_PRINTF(HIGH, this, "AMS: ams_set_client_request: call tmd2725_reval_instance_config");
          tmd2725_reval_instance_config(this, instance, state->sensor_type);
          if(inst_state->new_self_test_request)
          {
            tmd2725_set_self_test_inst_config(this, instance);
          }
        }
      } /** End of add the new request to list of client_requests.*/
    }
  }
  else // bad request
  {
    curr_instance = instance;
    instance = NULL;
  }

  if(NULL == curr_instance)
  {
    curr_instance = instance;
  }

  /* Sensors are required to call remove_instance when clientless */
  if(NULL != curr_instance
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
     &&
     NULL == curr_instance->cb->get_client_request(curr_instance, p_als_oc_suid, true) &&
     NULL == curr_instance->cb->get_client_request(curr_instance, p_als_st_suid, true)
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
     &&
     NULL == curr_instance->cb->get_client_request(curr_instance, p_prox_oc_suid, true) &&
     NULL == curr_instance->cb->get_client_request(curr_instance, p_prox_st_suid, true)
#endif
    )
  {
      SNS_PRINTF(HIGH, this, "ams_set_client_request: clientless, removing instance");
      this->cb->remove_instance(curr_instance);
      instance = NULL;
  }

  return instance;
}
