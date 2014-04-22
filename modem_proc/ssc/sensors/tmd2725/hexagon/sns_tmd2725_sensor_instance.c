/*
 * Copyright (c) 2018-2020, ams AG
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

#include <stdio.h>

#include "sns_tmd2725_sensor.h"
#include "sns_tmd2725_sensor_instance.h"

static void tmd2725_inst_cleanup(ams_instance_state *state, sns_stream_service *stream_mgr)
{
    if(NULL != state->interrupt_data_stream)
    {
        stream_mgr->api->remove_stream(stream_mgr, state->interrupt_data_stream);
        state->interrupt_data_stream = NULL;
    }
    if(NULL != state->timer_data_stream)
    {
        stream_mgr->api->remove_stream(stream_mgr, state->timer_data_stream);
        state->timer_data_stream = NULL;
    }
    if(NULL != state->scp_service)
    {
        state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
        state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
        state->scp_service = NULL;
    }
}

static void tmd2725_init_diag_log_size(sns_sensor_instance *const this)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;

#ifdef CONFIG_ALS_INCLUDE_IR_RESULTS
  float als_temp[ALS_PROTO_STD_ARRAY_LEN] = {0.0, 0.0, 0.0};
#else
  float als_temp[ALS_PROTO_STD_ARRAY_LEN] = {0.0, 0.0};
#endif
  float prox_temp[PROX_PROTO_STD_ARRAY_LEN] = {0.0, 0.0};

  uint8_t arr_index = 0;
  pb_float_arr_arg als_arg = {.arr = als_temp, .arr_len = ALS_PROTO_STD_ARRAY_LEN,
    .arr_index = &arr_index};
  pb_float_arr_arg prox_arg = {.arr = (float*)prox_temp, .arr_len = PROX_PROTO_STD_ARRAY_LEN,
    .arr_index = &arr_index};
  uint64_t buffer[50];
  sns_memset(buffer, 0, sizeof(buffer));
  pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;

  batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
  batch_sample.sample.arg = &als_arg;
  if(pb_encode_tag(&stream, PB_WT_STRING,
                    sns_diag_sensor_state_raw_sample_tag))
  {
    if(pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                           &batch_sample))
    {
      state->als_enc_raw_log_size = stream.bytes_written;
    }
  }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX
  batch_sample.sample.arg = &prox_arg;
  if(pb_encode_tag(&stream, PB_WT_STRING,
                    sns_diag_sensor_state_raw_sample_tag))
  {
    if(pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                           &batch_sample))
    {
      state->prox_enc_raw_log_size = stream.bytes_written;
    }
  }
#endif
}

/** See sns_sensor_instance_api::init */
sns_rc tmd2725_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sstate)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;
  ams_state *sensor_state = (ams_state*)sstate->state;
  float data[20];
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
  int64_t hwid = sensor_state->hardware_id;
  sns_time ts_val;

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  sns_sensor_uid * p_als_oc_suid = (sns_sensor_uid *)tmd2725_get_suid_with_sensortype_and_hwid(AMS_ALS_OC, hwid);
  sns_sensor_uid * p_als_st_suid = (sns_sensor_uid *)tmd2725_get_suid_with_sensortype_and_hwid(AMS_ALS_STRM, hwid);
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX
  sns_sensor_uid * p_prox_oc_suid = (sns_sensor_uid *)tmd2725_get_suid_with_sensortype_and_hwid(AMS_PROX_OC, hwid);
  sns_sensor_uid * p_prox_st_suid = (sns_sensor_uid *)tmd2725_get_suid_with_sensortype_and_hwid(AMS_PROX_STRM, hwid);
#endif
  sns_rc rv = 0;

  AMS_DEV_INST_PRINTF(LOW, this
                    , "AMS: tmd2725_inst_init: enter: this %p, sensor_state %p, sensor_type=%d"
                    , this
                    , sensor_state
                    , sensor_state->sensor_type
              );

  state->priv = sensor_state->priv; // local copy of our deviceCtx ptr
  state->sensor_type = sensor_state->sensor_type;
  state->publish_sensors = 0;
  state->publish_sensors_prev = 0;

  state->scp_service = (sns_sync_com_port_service*)
              service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

  state->diag_service =  (sns_diag_service*)
      service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);


  sns_memscpy (&state->irq_suid, sizeof(sns_sensor_uid),
        &sensor_state->irq_suid, sizeof(sns_sensor_uid));

  sns_memscpy (&state->timer_suid, sizeof(sns_sensor_uid),
        &sensor_state->timer_suid, sizeof(sns_sensor_uid));

  state->irq_info.irq_config = sensor_state->irq_config;


  /** Initialize COM port to be used by the Instance */
  sns_memscpy(&state->com_port_info.com_config,
              sizeof(state->com_port_info.com_config),
              &sensor_state->com_port_info.com_config,
              sizeof(sensor_state->com_port_info.com_config));

  rv = state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                                                          &state->com_port_info.port_handle);
  AMS_INST_PRINTF(LOW, this
                      , "tmd2725_inst_init:  data_stream_:  scp handle %p, sns_scp_register_com_port rv = %d"
                      , state->com_port_info.port_handle, rv);

  if(NULL == state->com_port_info.port_handle)
  {
    tmd2725_inst_cleanup(state, stream_mgr);
    return SNS_RC_FAILED;
  }

  rv = state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);
  AMS_INST_PRINTF(LOW, this, "tmd2725_inst_init:  sns_scp_open rv = %d", rv);

  /* Fill out the instance's portHndl */
  state->port_handle.scp_service = state->scp_service;
  state->port_handle.scp_handle = state->com_port_info.port_handle;
  state->port_handle.diag = state->diag_service;
  state->port_handle_ptr = &state->port_handle;
  /**----------- Copy all Sensor UIDs in instance state -------------*/
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  sns_memscpy(&state->als_info.suid[AMS_OC_IDX],
              sizeof(state->als_info.suid[AMS_OC_IDX]),
              p_als_oc_suid,
              sizeof(state->als_info.suid[AMS_OC_IDX]));
  sns_memscpy(&state->als_info.suid[AMS_STRM_IDX],
              sizeof(state->als_info.suid[AMS_STRM_IDX]),
              p_als_st_suid,
              sizeof(state->als_info.suid[AMS_STRM_IDX]));
  state->encoded_als_event_len = pb_get_encoded_size_sensor_stream_event(data, ALS_PROTO_STD_ARRAY_LEN);
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  sns_memscpy(&state->prox_info.suid[AMS_OC_IDX],
              sizeof(state->prox_info.suid[AMS_OC_IDX]),
              p_prox_oc_suid,
              sizeof(state->prox_info.suid[AMS_OC_IDX]));
  sns_memscpy(&state->prox_info.suid[AMS_STRM_IDX],
              sizeof(state->prox_info.suid[AMS_STRM_IDX]),
              p_prox_st_suid,
              sizeof(state->prox_info.suid[AMS_STRM_IDX]));
#ifdef CONFIG_PROX_INCLUDE_PROXIMITY_RESULTS
  state->encoded_prox_event_len = pb_get_encoded_size_sensor_stream_event(data, PROX_PROTO_STD_ARRAY_LEN);
#endif
#endif

  /* reset entire chip */
  state->calibration_data_tmd2725 = sensor_state->calibration_data_tmd2725;
  sns_memscpy(state->cal_version,
              sizeof(state->cal_version),
              sensor_state->cal_version,
              sizeof(sensor_state->cal_version));
  tmd2725_deviceInit(state->priv, state->port_handle_ptr, &sensor_state->calibration_data_tmd2725, \
                     &sensor_state->als_default_config_data, &sensor_state->prox_default_config_data, hwid);

  rv = state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
  AMS_INST_PRINTF(LOW, this, "tmd2725_inst_init:  sns_scp_update_bus_power rv = %d", rv);
  tmd2725_init_diag_log_size(this);
  ts_val = sns_get_system_time();
  state->als_cal_timestamp = ts_val;
  state->prox_fac_cal_timestamp = ts_val;
  state->prox_hw_cal_timestamp = ts_val;
  state->als_stream_config_timestamp = ts_val;
  state->prox_stream_config_timestamp = ts_val;
  state->als_oc_config_timestamp = ts_val;
  state->prox_oc_config_timestamp = ts_val;
  return SNS_RC_SUCCESS;
}

bool tmd2725_validate_device_odr(sns_sensor_instance *const instance, float *sample_rate)
{
  ams_instance_state *state = (ams_instance_state*)instance->state->state;
  bool change_in_sr = false;
//#if CONFIG_AMS_USE_DRI - the test sensor is hardcoded to request 1 Hz so
//                         let's support it even in DRI mode
  if(*sample_rate > 0.0
     && *sample_rate <= AMS_DEVICE_ODR_1)
  {
    *sample_rate = AMS_DEVICE_ODR_1;
  }
  else if(*sample_rate > AMS_DEVICE_ODR_1
          && *sample_rate <= AMS_DEVICE_ODR_2)
  {
    *sample_rate = AMS_DEVICE_ODR_2;
  }
  else if(*sample_rate > AMS_DEVICE_ODR_2
          && *sample_rate <= AMS_DEVICE_ODR_5)
  {
    *sample_rate = AMS_DEVICE_ODR_5;
  }
  else if(*sample_rate > AMS_DEVICE_ODR_5
          && *sample_rate <= AMS_DEVICE_ODR_10)
  {
    *sample_rate = AMS_DEVICE_ODR_10;
  }
  else if(*sample_rate > AMS_DEVICE_ODR_10)
  {
    *sample_rate = AMS_DEVICE_ODR_20;
  }
  else
  {
    state->sampling_rate_hz = 0;
    state->sampling_intvl = 0;
  }

  if(state->sampling_rate_hz != *sample_rate)
  {
    change_in_sr = true;
    union {
        uint32_t  dwds[2];
        uint64_t qwd;
    } ticks;

    if(*sample_rate > 0)
    {
      state->sampling_intvl =
        sns_convert_ns_to_ticks((uint64_t)(1000000000.0 / *sample_rate));
      state->sampling_rate_hz = *sample_rate;
    }
    else
    {
      state->sampling_intvl = 0;
      state->sampling_rate_hz = 0.0f;
    }

    ticks.qwd = state->sampling_intvl;
    AMS_DEV_INST_PRINTF(LOW, instance
                    , "AMS: tmd2725_validate_device_odr: sampling_intvl=(%08x:%08x)"
                    , ticks.dwds[1]
                    , ticks.dwds[0]
                    );
  }

  return change_in_sr;
}

/**
 * Runs a communication test - verfies WHO_AM_I, publishes self
 * test event.
 *
 * @param[i] instance  Instance reference
 * @param[i] uid       Sensor UID
 *
 * @return none
 */
static void _tmd2725_send_com_test_event(sns_sensor_instance *instance,
                                     sns_sensor_uid *uid, bool test_result)
{
  uint8_t data[1] = {0};
  pb_buffer_arg buff_arg = (pb_buffer_arg)
      { .buf = &data, .buf_len = sizeof(data) };
  sns_physical_sensor_test_event test_event =
     sns_physical_sensor_test_event_init_default;

  test_event.test_passed = test_result;
  test_event.test_type = SNS_PHYSICAL_SENSOR_TEST_TYPE_COM;
  test_event.test_data.funcs.encode = &pb_encode_string_cb;
  test_event.test_data.arg = &buff_arg;

  pb_send_event(instance,
                sns_physical_sensor_test_event_fields,
                &test_event,
                sns_get_system_time(),
                SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                uid);
}

static void _tmd2725_setup_clean_irq_stream (sns_sensor_instance *this, bool on_of)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

  if (on_of) {
    stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                   this,
                                                   state->irq_suid,
                                                   &state->interrupt_data_stream);

    /** Initialize IRQ info to be used by the Instance */
    sns_data_stream* data_stream = state->interrupt_data_stream;
    uint8_t buffer[20];
    sns_request irq_req =
    {
      .message_id = SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ,
      .request    = buffer
    };

    sns_memset(buffer, 0, sizeof(buffer));
    irq_req.request_len = pb_encode_request(buffer,
                                            sizeof(buffer),
                                            &state->irq_info.irq_config,
                                            sns_interrupt_req_fields,
                                            NULL);
    if(irq_req.request_len > 0)
    {
      data_stream->api->send_request(data_stream, &irq_req);
    }

    AMS_DEV_INST_PRINTF(LOW, this, "AMS: tmd2725_inst_init: sent IRQ rqst");

  } else {
    if(NULL != state->interrupt_data_stream)
    {
      tmd2725_deviceSetConfig(this, state->priv, AMS_CONFIG_GLOBAL, AMS_CONFIG_ALLOW_INTS, 0);
      stream_mgr->api->remove_stream(stream_mgr, state->interrupt_data_stream);
      state->interrupt_data_stream = NULL;
    }
  }
}

/**
 * Starts/restarts polling timer
 *
 * @param instance   Instance reference
 */
static void _tmd2725_start_polling_timer(sns_sensor_instance *this)
{
    ams_instance_state *state = (ams_instance_state*)this->state->state;
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_mgr = (sns_stream_service*)
        service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

    sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
    uint8_t buffer[50];
    sns_request timer_req = {
        .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
        .request    = buffer
    };

    if (NULL == state->timer_data_stream)
    {
        stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                       this,
                                                       state->timer_suid,
                                                       &state->timer_data_stream);
    }

    req_payload.is_periodic = true;
    req_payload.start_time = sns_get_system_time() - state->sampling_intvl + SNS_TIMER_LATENCY;
    req_payload.timeout_period = state->sampling_intvl;
    req_payload.start_config.early_start_delta = 0;

    timer_req.request_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                                              sns_timer_sensor_config_fields, NULL);
    if(timer_req.request_len > 0)
    {
        union {
            uint32_t dwds[2];
            uint64_t qwd;
        } ticks;
        ticks.qwd = state->sampling_intvl;

        SNS_INST_PRINTF(LOW, this, "_tmd2725_start_polling_timer: sampling_intvl=(%08x:%08x)"
                        , ticks.dwds[1], ticks.dwds[0]
            );

        state->timer_data_stream->api->send_request(state->timer_data_stream, &timer_req);
        state->timer_is_active = true;
    }
    else
    {
        //diag->api->sensor_printf(diag, this, SNS_ERROR, __FILENAME__, __LINE__,
        //                         "LSM timer req encode error");
    }
}

static void _tmd2725_run_self_test(sns_sensor_instance *instance)
{
  ams_instance_state *state = (ams_instance_state*)instance->state->state;
  ams_deviceCtx_t *devCtx = (ams_deviceCtx_t *)(state->priv);
  bool who_am_i_success = 0;
  bool bret = 0;
  bool handlerIsReady = (devCtx && devCtx->portHndl);
  sns_sensor_uid *pSuid = NULL;

  if(handlerIsReady)
  {
    tmd2725_deviceIdentifier_e devId;
    devId = tmd2725_validateDevice(devCtx->portHndl);
    who_am_i_success = (devId != AMS_UNKNOWN_DEVICE) && (devId != AMS_LAST_DEVICE) ? true : false;
  }
  else
  {
    who_am_i_success = false;
  }

/* --------------------------------------------------------------------------
 * AMBIENT LIGHT
 * -------------------------------------------------------------------------- */

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  if(state->als_info.test_info[AMS_OC_IDX].test_client_present)
  {
    if(state->als_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      AMS_DEV_INST_PRINTF(HIGH, instance, "_tmd2725_run_self_test: on-change SNS_PHYSICAL_SENSOR_TEST_TYPE_COM");
      _tmd2725_send_com_test_event(instance, &state->als_info.suid[AMS_OC_IDX], who_am_i_success);
      state->new_self_test_request = false;
      state->als_info.test_info[AMS_OC_IDX].test_client_present = false;
    }
    else if(state->als_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      AMS_DEV_INST_PRINTF(HIGH, instance, "_tmd2725_run_self_test: ALS on change SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY");
      if(!who_am_i_success)
      {
          AMS_DEV_INST_PRINTF(HIGH, instance, "_tmd2725_run_self_test: No sensor found");
          _tmd2725_factory_test_no_sensor_found(instance);
          state->new_self_test_request = false;
          state->als_info.test_info[AMS_OC_IDX].test_client_present = false;
      }
      else
      {
          bret = false;
          if(handlerIsReady)
          {
            AMS_DEV_INST_PRINTF(HIGH, instance, "_tmd2725_run_self_test: calling tmd2725_deviceCalibrateLux");
            bret = tmd2725_deviceCalibrateLux(instance, devCtx, NULL, AMS_ALS_CAL_TYPE_ON_CHANGE);
          }
          if (!bret)
          {
              AMS_DEV_INST_PRINTF(HIGH, instance,"AMS: !!!! CALLING _tmd2725_send_factory_test_event with FALSE !!!! ");
             _tmd2725_send_factory_test_event(instance, &state->als_info.suid[AMS_OC_IDX], false);
          }
      }
    }
  }
  if(state->als_info.test_info[AMS_STRM_IDX].test_client_present)
  {
    if(state->als_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      AMS_DEV_INST_PRINTF(HIGH, instance, "_tmd2725_run_self_test: streaming SNS_PHYSICAL_SENSOR_TEST_TYPE_COM");
      _tmd2725_send_com_test_event(instance, &state->als_info.suid[AMS_STRM_IDX], who_am_i_success);
      state->new_self_test_request = false;
      state->als_info.test_info[AMS_STRM_IDX].test_client_present = false;
    }
    else if(state->als_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      AMS_DEV_INST_PRINTF(HIGH, instance, "_tmd2725_run_self_test: ALS stream SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY");
      bret = false;
      if(!who_am_i_success)
      {
          AMS_DEV_INST_PRINTF(HIGH, instance, "_tmd2725_run_self_test: no sensor found");
          _tmd2725_factory_test_no_sensor_found(instance);
          state->new_self_test_request = false;
          state->als_info.test_info[AMS_STRM_IDX].test_client_present = false;
      }
      else
      {
          if(handlerIsReady)
          {
            if(state->timer_is_active)
              {
                state->timer_is_active = false;

                sns_service_manager *service_mgr = instance->cb->get_service_manager(instance);
                sns_stream_service *stream_mgr = (sns_stream_service*)
                            service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
                if(NULL != state->timer_data_stream)
                {
                  stream_mgr->api->remove_stream(stream_mgr, state->timer_data_stream);
                  state->timer_data_stream = NULL;
                }
                AMS_DEV_INST_PRINTF(HIGH, instance, "_tmd2725_run_self_test: remove ALS timer");
            }
            AMS_DEV_INST_PRINTF(HIGH, instance, "_tmd2725_run_self_test: calling tmd2725_deviceCalibrateLux");
            bret = tmd2725_deviceCalibrateLux(instance, devCtx, NULL, AMS_ALS_CAL_TYPE_STREAMING);
          }
          if (bret)
          {
            _tmd2725_start_polling_timer(instance);
          }
          else
          {
              AMS_DEV_INST_PRINTF(HIGH, instance,"AMS: !!!! CALLING _tmd2725_send_factory_test_event with FALSE !!!! ");
            _tmd2725_send_factory_test_event(instance, &state->als_info.suid[AMS_STRM_IDX], false);
          }
      }
    }
  }
#endif

/* --------------------------------------------------------------------------
 * PROXIMITY
 * -------------------------------------------------------------------------- */
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  if(state->prox_info.test_info[AMS_OC_IDX].test_client_present)
  {
    if(state->prox_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      _tmd2725_send_com_test_event(instance, &state->prox_info.suid[AMS_OC_IDX], who_am_i_success);
      state->new_self_test_request = false;
      state->prox_info.test_info[AMS_OC_IDX].test_client_present = false;
    }
    else if(state->prox_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      AMS_DEV_INST_PRINTF(HIGH, instance,
                     "AMS: _tmd2725_run_self_test: PROX on change SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY");
      bret = false;

      if(!who_am_i_success)
      {
          AMS_DEV_INST_PRINTF(HIGH, instance, "AMS: _tmd2725_run_self_test: no sensor found");
          _tmd2725_factory_test_no_sensor_found(instance);
          state->new_self_test_request = false;
          state->prox_info.test_info[AMS_OC_IDX].test_client_present = false;
      }
      else
      {
          if(handlerIsReady)
          {
            AMS_DEV_INST_PRINTF(HIGH, instance,
                     "AMS: _tmd2725_run_self_test: calling tmd2725_deviceCalibrateProx");
            bret = tmd2725_deviceCalibrateProx(instance, devCtx, NULL);
          }
          if(!bret)
          {
              AMS_DEV_INST_PRINTF(HIGH, instance,"AMS: !!!! CALLING _tmd2725_send_factory_test_event with FALSE !!!! ");
            _tmd2725_send_factory_test_event(instance, &state->prox_info.suid[AMS_OC_IDX], false);
          }
      }
    }
    else if(state->prox_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_HW)
    {
      AMS_DEV_INST_PRINTF(HIGH, instance,
                     "AMS: _tmd2725_run_self_test: calling tmd2725_deviceCalibrateProxHardware");
      bret = false;
      if(handlerIsReady)
      {
        bret = tmd2725_deviceCalibrateProxHardware(instance, devCtx, NULL);
      }
      if(!bret)
      {
          AMS_DEV_INST_PRINTF(HIGH, instance,"AMS: !!!! CALLING _tmd2725_send_factory_test_event with FALSE !!!! ");
        _tmd2725_send_factory_test_event(instance, &state->prox_info.suid[AMS_OC_IDX], false);
      }

      float result = (float)((state->calibration_data_tmd2725.proxHardThreshold) >> 8);
      AMS_DEV_INST_PRINTF(HIGH, instance,"calling _tmd2725_send_cal_event 1 ");
      //update cal_time_stamp
      _tmd2725_send_cal_event (instance, pSuid, &result, 1, NULL, 0, state->prox_fac_cal_timestamp);
    }
  }

  if(state->prox_info.test_info[AMS_STRM_IDX].test_client_present)
  {
    if(state->prox_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      _tmd2725_send_com_test_event(instance, &state->prox_info.suid[AMS_STRM_IDX], who_am_i_success);
      state->new_self_test_request = false;
      state->prox_info.test_info[AMS_STRM_IDX].test_client_present = false;
    }
    else if(state->prox_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      AMS_DEV_INST_PRINTF(HIGH, instance,
                     "AMS: _tmd2725_run_self_test: PROX stream SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY");
      bret = false;
      if(!who_am_i_success)
      {
          AMS_DEV_INST_PRINTF(HIGH, instance, "AMS: _tmd2725_run_self_test: no sensor found");
          _tmd2725_factory_test_no_sensor_found(instance);
          state->new_self_test_request = false;
          state->prox_info.test_info[AMS_STRM_IDX].test_client_present = false;
      }
      else
      {
          if(handlerIsReady)
          {
            AMS_DEV_INST_PRINTF(HIGH, instance,
                     "AMS: _tmd2725_run_self_test: calling tmd2725_deviceCalibrateProx");
            bret = tmd2725_deviceCalibrateProx(instance, devCtx, NULL);
          }
          if (bret)
          {
            _tmd2725_start_polling_timer(instance);
          }
          else
          {
              AMS_DEV_INST_PRINTF(HIGH, instance,"AMS: !!!! CALLING _tmd2725_send_factory_test_event with FALSE !!!! ");
            _tmd2725_send_factory_test_event(instance, &state->prox_info.suid[AMS_STRM_IDX], false);
          }
      }
    }
    else if(state->prox_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_HW)
    {
      AMS_DEV_INST_PRINTF(HIGH, instance,
                     "AMS: _tmd2725_run_self_test: calling tmd2725_deviceCalibrateProxHardware");
      bret = false;
      if(handlerIsReady)
      {
        bret = tmd2725_deviceCalibrateProxHardware(instance, devCtx, NULL);
      }
      if (bret)
      {
        _tmd2725_start_polling_timer(instance);
      }
      else
      {
          AMS_DEV_INST_PRINTF(HIGH, instance,"AMS: !!!! CALLING _tmd2725_send_factory_test_event with FALSE !!!! ");
        _tmd2725_send_factory_hw_test_event(instance, &state->prox_info.suid[AMS_STRM_IDX], false);
      }

      float result = (float)((state->calibration_data_tmd2725.proxHardThreshold) >> 8);
      AMS_DEV_INST_PRINTF(HIGH, instance,"calling _tmd2725_send_cal_event 2 ");
      _tmd2725_send_cal_event (instance, pSuid, &result, 1, NULL, 0, state->prox_fac_cal_timestamp);
    }
  }
#endif
}

/**
 * Updates polling configuration
 *
 * @param[i] instance   Sensor instance
 *
 * @return sampling interval time in ticks
 */
static void _tmd2725_set_polling_config(sns_sensor_instance *const this)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;

  {
    union {
        uint32_t dwds[2];
        uint64_t qwd;
    } ticks;
    ticks.qwd = state->sampling_intvl;
    AMS_DEV_INST_PRINTF(LOW, this
                    , "AMS: ams_set_polling_config: sampling_intvl=(%08x:%08x), timer_is_active=%d"
                    , ticks.dwds[1], ticks.dwds[0]
                    , state->timer_is_active
                    );
  }

  if((state->sampling_intvl > 0) &&
     (state->new_self_test_request == false)) // don't start the timer if self_test_request
  {
    _tmd2725_start_polling_timer(this);
  }
  else if(state->timer_is_active)
  {
    state->timer_is_active = false;

    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_mgr = (sns_stream_service*)
                service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    if(NULL != state->timer_data_stream)
    {
      stream_mgr->api->remove_stream(stream_mgr, state->timer_data_stream);
      state->timer_data_stream = NULL;
    }
  }
}

/**
 * Configures sensor with new/recomputed settings
 *
 * @param instance   Instance reference
 */
static void _tmd2725_reconfig_hw(sns_sensor_instance *this)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;

  AMS_DEV_INST_PRINTF(LOW, this
                  , "AMS: _tmd2725_reconfig_hw: publish_sensors=0x%04x"
                  , state->publish_sensors
                  );

  if(state->publish_sensors & AMS_STREAMERS)
  {
      SNS_INST_PRINTF(MED, this, "_tmd2725_reconfig_hw: starting timer ");
      _tmd2725_set_polling_config(this);
  }
  else
  {
    /* QC - the periodic timer would still be running at this point; must close timer stream to turn off this timer */
      SNS_INST_PRINTF(MED, this, "_tmd2725_reconfig_hw: closing timer stream publish_sensors=0x%04x", state->publish_sensors);
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_mgr = (sns_stream_service*)
                service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    if(NULL != state->timer_data_stream)
    {
      stream_mgr->api->remove_stream(stream_mgr, state->timer_data_stream);
      state->timer_data_stream = NULL;
    }
    state->timer_is_active = false;
  }
}

/**
 * Sends config update event for the chosen sample_rate
 *
 * @param[i] instance    reference to this Instance
 */
static void _tmd2725_send_config_event(sns_sensor_instance *const instance)
{
  ams_instance_state *state =
     (ams_instance_state*)instance->state->state;

  AMS_DEV_INST_PRINTF(LOW, instance
                  , "AMS: _tmd2725_send_config_event: enter: inst_this %p, .publish_sensors=0x%04x"
                  , instance
                  , state->publish_sensors
                  );

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
    if(state->publish_sensors & AMS_ALS_OC)
    {
      tmd2725_als_send_config_update(instance, AMS_OC_IDX);
    }
    if(state->publish_sensors & (AMS_ALS_STRM))
    {
        tmd2725_als_send_config_update(instance, AMS_STRM_IDX);
    }
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB


    if(state->publish_sensors & (AMS_PROX_STRM))
    {
        tmd2725_prox_send_config_update(instance, AMS_STRM_IDX);
    }
    if(state->publish_sensors & AMS_PROX_OC)
    {
        tmd2725_prox_send_config_update(instance, AMS_OC_IDX);
    }
#endif
}

/** See sns_sensor_instance_api::set_client_config */
sns_rc tmd2725_inst_set_client_config(sns_sensor_instance *const this,
    sns_request const *client_request)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;
  float desired_sample_rate = 0.0;
  float desired_report_rate = 0.0;
  float ams_chosen_sample_rate = 0.0;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_std_req *payload = (sns_std_req*)client_request->request;

  AMS_DEV_INST_PRINTF(LOW, this
                  , "tmd2725_inst_set_client_config: enter: inst_this 0x%X, message_id %d"
                  , this, client_request->message_id);

  // Turn COM port ON
  rv = state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           true);
  AMS_INST_PRINTF(LOW, this, "tmd2725_inst_set_client_config: sns_scp_update_bus_power returned %d", rv);


  if(client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
  {
      SNS_INST_PRINTF(LOW, this
                      , "tmd2725_inst_set_client_config: SNS_STD_SENSOR_CONFIG, SR=%d, RR=%d"
                      , (int32_t)payload->desired_sample_rate
                      , (int32_t)payload->desired_report_rate);

    // 1. Extract sample, report rates from client_request.
    // 2. Configure sensor HW.
    // 3. sendRequest() for Timer to start/stop in case of polling using timer_data_stream.
    // 4. sendRequest() for Intrerupt register/de-register in case of DRI using interrupt_data_stream.
    // 5. Save the current config information like type, sample_rate, report_rate, etc.
    desired_sample_rate = payload->desired_sample_rate;
    desired_report_rate = payload->desired_report_rate;

    if(desired_report_rate > desired_sample_rate)
    {
        // bad request. Return error or default report_rate to sample_rate
        SNS_INST_PRINTF(LOW, this, "tmd2725_inst_set_client_config: desired report rate > desired sample rate, using desired sample rate");
        desired_report_rate = desired_sample_rate;
    }

    ams_chosen_sample_rate = desired_sample_rate; //we only put this into effect in polling mode
    SNS_INST_PRINTF(LOW, this
                    , "AMS: tmd2725_inst_set_client_config: ams_chosen_sample_rate=%d"
                    , ams_chosen_sample_rate
        );

    {
      ams_configureFeature_t feature;
      uint16_t EITHER = 0;
      bool     both_off = false, both_were_off = false;
      uint16_t newly_on = 0, newly_off = 0;
      bool     option_changed = false;
      bool     option_changed_for_generateAlsResponse = false;
      bool     option_changed_for_generateProxResponse = false;
      //uint32_t option_new_state;
      bool option_new_state = false;
      uint16_t on = state->publish_sensors;
      uint16_t p = state->publish_sensors_prev;

      newly_on = ~p & on;
      newly_off = p & ~on;

      AMS_DEV_INST_PRINTF(LOW, this
                      , "AMS: tmd2725_inst_set_client_config: on=%04x, p=%04x, newly_on=%04x, newly_off=%04x"
                      , (uint32_t)on
                      , (uint32_t)p
                      , (uint32_t)newly_on
                      , (uint32_t)newly_off
                      );

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
      feature = AMS_CONFIG_ALS_LUX;
      EITHER = AMS_ALS_OC | AMS_ALS_STRM;
      both_were_off = !(p & EITHER);
      both_off = !(on & EITHER);

      AMS_DEV_INST_PRINTF(LOW, this
                      , "AMS: tmd2725_inst_set_client_config: ALS: EITHER=%04x, both_were_off=%d, both_off=%d"
                      , EITHER
                      , (uint32_t)both_were_off
                      , (uint32_t)both_off
                      );

      option_changed = false;
      if (newly_off & AMS_ALS_STRM)
      {
        option_changed = true;
        option_new_state = 0;
      }
      else if (newly_on & AMS_ALS_STRM)
      {
        option_changed = true;
        option_new_state = 1;
      }
      if(option_changed && !state->new_self_test_request)
      {
        AMS_DEV_INST_PRINTF(LOW, this
                        , "AMS: ALS: CHANGE ALWAYS_READ STATE TO:  %d"
                        , option_new_state
                        );
        tmd2725_deviceSetConfig(this, state->priv, feature, AMS_CONFIG_ALWAYS_READ, option_new_state);
      }

      option_changed = false;
      if ((newly_on & EITHER) && both_were_off)
      {
        option_changed = true;
        option_new_state = 1;
      }
      else if((newly_off & EITHER) && both_off)
      {
        option_changed = true;
        option_new_state = 0;
      }
      option_changed_for_generateAlsResponse = option_changed;
      if(option_changed && !state->new_self_test_request)
      {
        AMS_DEV_INST_PRINTF(LOW, this
                        , "AMS: ALS: CHANGE ENABLE STATE TO:  %d"
                        , option_new_state
                        );
        tmd2725_deviceSetConfig(this, state->priv, feature, AMS_CONFIG_ENABLE, option_new_state);
      }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
      feature = AMS_CONFIG_PROX;
      EITHER = AMS_PROX_OC | AMS_PROX_STRM;
      both_were_off = !(p & EITHER);
      both_off = !(on & EITHER);

      AMS_DEV_INST_PRINTF(LOW, this
                      , "AMS: tmd2725_inst_set_client_config: PROX: EITHER=%04x, both_were_off=%d, both_off=%d"
                      , (uint32_t)EITHER
                      , (uint32_t)both_were_off
                      , (uint32_t)both_off
                      );

      option_changed = false;
      if (newly_off & AMS_PROX_STRM)
      {
        option_changed = true;
        option_new_state = 0;
      }
      else if (newly_on & AMS_PROX_STRM)
      {
        option_changed = true;
        option_new_state = 1;
      }
      if(option_changed && !state->new_self_test_request)
      {
        AMS_DEV_INST_PRINTF(LOW, this
                        , "AMS: PROX: CHANGE ALWAYS_READ STATE TO:  %d"
                        , option_new_state
                        );
        tmd2725_deviceSetConfig(this, state->priv, feature, AMS_CONFIG_ALWAYS_READ, option_new_state);
      }

      option_changed = false;
      if ((newly_on & EITHER) && both_were_off)
      {
        option_changed = true;
        option_new_state = 1;
      }
      else if((newly_off & EITHER) && both_off)
      {
        option_changed = true;
        option_new_state = 0;
      }
      option_changed_for_generateProxResponse = option_changed;
      if(option_changed && !state->new_self_test_request)
      {
        AMS_DEV_INST_PRINTF(LOW, this
                        , "AMS: PROX: CHANGE ENABLE STATE TO:  %d"
                        , option_new_state
                        );
        tmd2725_deviceSetConfig(this, state->priv, feature, AMS_CONFIG_ENABLE, option_new_state);
      }
#endif
      {
        uint16_t p_streamers = p & AMS_STREAMERS;
        uint16_t on_streamers = on & AMS_STREAMERS;

        /* Update timer if any change in current/previous streaming sensors */
        if(on_streamers || p_streamers)
        {
          if(tmd2725_validate_device_odr(this, &ams_chosen_sample_rate))
          {
            _tmd2725_reconfig_hw(this);
            //update config time stamp
            if(on_streamers & AMS_ALS_STRM)
            {
                state->als_stream_config_timestamp = sns_get_system_time();
            }
            if(on_streamers & AMS_PROX_STRM)
            {
                state->prox_stream_config_timestamp = sns_get_system_time();
            }
          }
        }
      }
      {
        uint16_t p_on_changesrs = p & AMS_ON_CHANGERS;
        uint16_t on_changesrs = on & AMS_ON_CHANGERS;
        if (!p_on_changesrs && on_changesrs)
        {
          // setup the irq stream
          _tmd2725_setup_clean_irq_stream (this, true);
        }
        if (p_on_changesrs && !on_changesrs)
        {
          // remove the irq stream
          _tmd2725_setup_clean_irq_stream (this, false);
        }
      }

      if (state->new_or_chng_req)
      {
        AMS_DEV_INST_PRINTF(LOW, this, "AMS:tmd2725_inst_set_client_config state->new_or_chng_req = %d ", state->new_or_chng_req);

        /* The config event must be sent before any cal event */
        state->publish_sensors_prev = state->publish_sensors;
        if(!state->new_self_test_request)
        {
            _tmd2725_send_config_event(this);
            if( !state->resume_op )
            {
                uint16_t sensor_val;
                if(state->new_client_flag)
                {
                    sensor_val = state->publish_sensors_new;
                    state->new_client_flag = 0;
                }
                else
                {
                    sensor_val = newly_on;
                }
                if (sensor_val & (AMS_ALS_OC | AMS_ALS_STRM))
                {
                  float result = (float)state->calibration_data_tmd2725.alsCalibrationFactor;
                  sns_sensor_uid *pSuid;
                  if (sensor_val & AMS_ALS_OC)
                  {
                    pSuid = &(state->als_info.suid[AMS_OC_IDX]);
                    _tmd2725_send_cal_event (this, pSuid, &result, 1, NULL, 0, state->als_cal_timestamp);
                  }
                  if (sensor_val & AMS_ALS_STRM)
                  {
                    pSuid = &(state->als_info.suid[AMS_STRM_IDX]);
                    _tmd2725_send_cal_event (this, pSuid, &result, 1, NULL, 0, state->als_cal_timestamp);
                  }
                }

                /* send cal_event only for a new AMS_CONFIG_PROX client*/
                if (sensor_val & (AMS_PROX_OC | AMS_PROX_STRM))
                {
                  float result = (float)((state->calibration_data_tmd2725.proxHardThreshold) >> 8);
                  sns_sensor_uid *pSuid;
                  if (sensor_val & AMS_PROX_OC)
                  {
                    pSuid = &(state->prox_info.suid[AMS_OC_IDX]);
                    _tmd2725_send_cal_event (this, pSuid, &result, 1, NULL, 0, state->prox_fac_cal_timestamp);
                  }
                  if (sensor_val & AMS_PROX_STRM)
                  {
                    pSuid = &(state->prox_info.suid[AMS_STRM_IDX]);
                    _tmd2725_send_cal_event (this, pSuid, &result, 1, NULL, 0, state->prox_fac_cal_timestamp);
                  }
                }
            }
            else
            {
                state->resume_op = 0;
            }
        }

        state->new_or_chng_req = false;
      }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
      if (newly_on & AMS_ALS_OC)
      {
        state->als_info.prev_OC_report[0] = NEGATIVE_INFINITY;
        state->als_info.prev_OC_report[1] = NEGATIVE_INFINITY;
		state->als_info.prev_OC_report_int[0] = 0x7FFFFFFF;
		state->als_info.prev_OC_status[0] = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
        /* if we're not going to _CONFIG_ENABLE it, make the 1st OC report now */
        if(!option_changed_for_generateAlsResponse)
        {
          tmd2725_generateAlsResponse(state, state->diag_service, sns_get_system_time(), this, AMS_REPORT_ON_CHANGE);
        }
      }
      else if (state->als_info.client_cfg.oc_send_last_event)
      {
          state->als_info.client_cfg.oc_send_last_event = false;
          // May register 2 clients before first IRQ, don't send invalid data
          if (state->als_info.timestamp > 0) {
              pb_send_sensor_stream_event(this,
                                          &state->als_info.suid[AMS_OC_IDX],
                                          state->als_info.timestamp,
                                          SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                          state->als_info.status,
                                          state->als_info.prev_OC_report,
                                          ARR_SIZE(state->als_info.prev_OC_report),
                                          state->encoded_als_event_len);
          }
      }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
      if (newly_on & AMS_PROX_OC)
      {
        state->prox_info.prev_OC_report[0] = AMS_PROXIMITY_EVENT_TYPE_INVALID;
		state->prox_info.prev_OC_status[0] = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
        /* if we're not going to _CONFIG_ENABLE it, make the 1st OC report now */
        if(!option_changed_for_generateProxResponse)
        {
          log_sensor_state_raw_info log_prox_state_raw_info;
          // Allocate Sensor State Raw log packets
          sns_memzero(&log_prox_state_raw_info, sizeof(log_prox_state_raw_info));

          tmd2725_generateProxResponse(state, state->diag_service, sns_get_system_time(),
                                 this, &log_prox_state_raw_info, AMS_REPORT_ON_CHANGE);
        }
      }
      else if (state->prox_info.client_cfg.oc_send_last_event) {
            state->prox_info.client_cfg.oc_send_last_event = false;
            // May register 2 clients before first IRQ, don't send invalid data
            if (state->prox_info.timestamp > 0)
            {
                AMS_DEV_INST_PRINTF(LOW, this, "_tmd2725_send_config_event: send PROX_OC last data evt");
                pb_send_event(this,
                              sns_proximity_event_fields,
                              &state->prox_info.prox_state,
                              state->prox_info.timestamp,
                              SNS_PROXIMITY_MSGID_SNS_PROXIMITY_EVENT,
                              &state->prox_info.suid[AMS_OC_IDX]);
            }
      }
#endif
    }
  }
  else if(client_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
  {
    SNS_INST_PRINTF(HIGH, this, "tmd2725_inst_set_client_config: SNS_STD_FLUSH_REQ, no action for TMD2725");
  }
  else if(client_request->message_id ==
          SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
  {
    SNS_INST_PRINTF(MED, this, "tmd2725_inst_set_client_config: SNS_PHYSICAL_SENSOR_TEST_CONFIG");

     // 1. Extract test type from client_request.
     // 2. Configure sensor HW for test type.
     // 3. send_request() for Timer Sensor in case test needs polling/waits.
     // 4. Factory test is TBD.
    _tmd2725_run_self_test(this);

  }
  else if( client_request->message_id ==SNS_CAL_MSGID_SNS_CAL_RESET)
  {
      if (state->sensor_type & (AMS_ALS_OC | AMS_ALS_STRM))
      {
          float result = (float)state->calibration_data_tmd2725.alsCalibrationFactor;
          sns_sensor_uid *pSuid;
          state->als_cal_timestamp = sns_get_system_time();
          if (state->sensor_type & AMS_ALS_OC)
          {
            pSuid = &(state->als_info.suid[AMS_OC_IDX]);
            tmd2725_als_send_config_update(this, AMS_OC_IDX);
            _tmd2725_send_cal_event (this, pSuid, &result, 1, NULL, 0, state->als_cal_timestamp);
          }
          else if (state->sensor_type & AMS_ALS_STRM)
          {
            pSuid = &(state->als_info.suid[AMS_STRM_IDX]);
            tmd2725_als_send_config_update(this, AMS_STRM_IDX);
            _tmd2725_send_cal_event (this, pSuid, &result, 1, NULL, 0, state->als_cal_timestamp);
          }
      }
      else if (state->sensor_type & (AMS_PROX_OC | AMS_PROX_STRM))
      {
          float result = (float)((state->calibration_data_tmd2725.proxHardThreshold) >> 8);
          sns_sensor_uid *pSuid;
          state->prox_fac_cal_timestamp = sns_get_system_time();
          if (state->sensor_type & AMS_PROX_OC)
          {
            pSuid = &(state->prox_info.suid[AMS_OC_IDX]);
            tmd2725_prox_send_config_update(this, AMS_OC_IDX);
            _tmd2725_send_cal_event (this, pSuid, &result, 1, NULL, 0, state->prox_fac_cal_timestamp);
          }
          else if (state->sensor_type & AMS_PROX_STRM)
          {
            pSuid = &(state->prox_info.suid[AMS_STRM_IDX]);
            tmd2725_prox_send_config_update(this, AMS_STRM_IDX);
            _tmd2725_send_cal_event (this, pSuid, &result, 1, NULL, 0, state->prox_fac_cal_timestamp);
          }
      }
  }
  else
  {
      SNS_INST_PRINTF(HIGH, this, "tmd2725_inst_set_client_config: unsupported message id %d", client_request->message_id);
  }

  // Turn COM port OFF
  rv = state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           false);

  AMS_INST_PRINTF(LOW, this, "AMS: tmd2725_inst_set_client_config: end,  sns_scp_update_bus_power %d", rv);

  return rv;
}

sns_rc tmd2725_inst_deinit(sns_sensor_instance *const this)
{
    ams_instance_state *state = (ams_instance_state*)this->state->state;
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_mgr = (sns_stream_service*)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

    SNS_INST_PRINTF(LOW, this, "AMS: tmd2725_inst_deinit, cleaning up");

    tmd2725_inst_cleanup(state, stream_mgr);

    return SNS_RC_SUCCESS;
}
