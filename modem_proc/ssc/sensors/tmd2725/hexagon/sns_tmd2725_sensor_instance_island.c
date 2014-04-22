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
#include "sns_tmd2725_sensor.h"
#include "sns_tmd2725_sensor_instance.h"

// Unencoded batch sample
typedef struct
{
  /* Batch Sample type as defined in sns_diag.pb.h */
  sns_diag_batch_sample_type sample_type;
  /* Timestamp of the sensor state data sample */
  sns_time timestamp;
  /*Raw sensor state data sample*/
  float sample[ALS_PROTO_STD_ARRAY_LEN];
  /* Data status.*/
  sns_std_sensor_sample_status status;
} als_batch_sample;

// Unencoded batch sample
typedef struct
{
  /* Batch Sample type as defined in sns_diag.pb.h */
  sns_diag_batch_sample_type sample_type;
  /* Timestamp of the sensor state data sample */
  sns_time timestamp;
  /*Raw sensor state data sample*/
  float sample[PROX_PROTO_STD_ARRAY_LEN];
  /* Data status.*/
  sns_std_sensor_sample_status status;
} prox_batch_sample;

/** See sns_sensor_instance_api::set_client_config */
sns_rc tmd2725_inst_set_client_config(sns_sensor_instance *const this,
    sns_request const *client_request);

static void _tmd2725_wrap_up_factory_test(sns_sensor_instance *instance,
                                          bool test_result);
static void _tmd2725_check_and_remove_stream(sns_sensor_instance *this,
    ams_mode_index oc_strm, sns_data_stream *stream);

void tmd2725_als_send_config_update(sns_sensor_instance *const this, ams_mode_index mode)
{
  ams_instance_state *state =
     (ams_instance_state*)this->state->state;
  sns_std_sensor_physical_config_event phy_sensor_config =
     sns_std_sensor_physical_config_event_init_default;
  ams_deviceCtx_t *devCtx = (ams_deviceCtx_t *)(state->priv);
  // TODO: Use appropriate op_mode selected by driver.
  char operating_mode[] = "NORMAL";
  pb_buffer_arg op_mode_args;
  sns_time timestamp_val;

  AMS_DEV_INST_PRINTF(LOW, this
                  , "AMS: tmd2725_als_send_config_update: enter: inst_this %p, .publish_sensors=0x%04x"
                  , this
                  , state->publish_sensors
                  );
  op_mode_args.buf = &operating_mode[0];
  op_mode_args.buf_len = sizeof(operating_mode);

  ams_deviceInfo_t info;
  AMS_INST_PRINTF(HIGH, this, "AMS: tmd2725_als_send_config_update calling tmd2725_getDeviceInfo with hwid = %d" , devCtx->hwid);
  tmd2725_getDeviceInfo(&info, devCtx->hwid);

  AMS_INST_PRINTF(LOW, this
              , "AMS: tmd2725_als_send_config_update: mode = %d new mMaxLux = %d"
              , mode
              , devCtx->ccbAlsCtx.ctxAlgAls.results.mMaxLux
              );

  phy_sensor_config.has_sample_rate = true; /* required by SEE for both STRM and OC */
  if (mode & AMS_STRM_IDX)
  {
    phy_sensor_config.sample_rate = state->sampling_rate_hz;
    timestamp_val = state->als_stream_config_timestamp;
  }
  else {
    phy_sensor_config.sample_rate = 0;
    timestamp_val = state->als_oc_config_timestamp;
  }

  phy_sensor_config.has_water_mark = false;
  phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
  phy_sensor_config.operation_mode.arg = &op_mode_args;
  phy_sensor_config.has_active_current = true;
  phy_sensor_config.active_current = info.alsSensor.activeCurrent_uA;
  phy_sensor_config.has_resolution = true;
  phy_sensor_config.resolution = devCtx->ccbAlsCtx.ctxAlgAls.luxResolution;
  phy_sensor_config.range_count = 2;
  phy_sensor_config.range[0] = 0;
  phy_sensor_config.range[1] = devCtx->ccbAlsCtx.ctxAlgAls.maxLux;;
  phy_sensor_config.has_stream_is_synchronous = false;
  phy_sensor_config.has_dri_enabled = true;
  phy_sensor_config.dri_enabled = (mode == AMS_STRM_IDX ? false : true);
  phy_sensor_config.has_DAE_watermark = false;
  pb_send_event(this,
                sns_std_sensor_physical_config_event_fields,
                &phy_sensor_config,
                timestamp_val,
                SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                &state->als_info.suid[mode]);
}

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
void tmd2725_prox_send_config_update(sns_sensor_instance *const instance, ams_mode_index mode)
{
    ams_instance_state *state =
        (ams_instance_state*)instance->state->state;
    ams_deviceCtx_t *devCtx = (ams_deviceCtx_t *)(state->priv);
    sns_time timestamp_val = 0;

    AMS_DEV_INST_PRINTF(LOW, instance
                  , "AMS: tmd2725_prox_send_config_update: enter: inst_this %p, .publish_sensors=0x%04x"
                  , instance
                  , state->publish_sensors
                  );

    sns_std_sensor_physical_config_event phy_sensor_config =
       sns_std_sensor_physical_config_event_init_default;
    // TODO: Use appropriate op_mode selected by driver.
    char operating_mode[] = "NORMAL";
    pb_buffer_arg op_mode_args;
    op_mode_args.buf = &operating_mode[0];
    op_mode_args.buf_len = sizeof(operating_mode);

    ams_deviceInfo_t info;
    AMS_DEV_INST_PRINTF(HIGH, instance, "AMS: tmd2725_prox_send_config_update calling tmd2725_getDeviceInfo with hwid = %d" , devCtx->hwid);
    tmd2725_getDeviceInfo(&info, devCtx->hwid);

    phy_sensor_config.has_sample_rate = true;
    if(mode & AMS_OC_IDX)
    {
      phy_sensor_config.sample_rate = 0;
      timestamp_val = state->prox_oc_config_timestamp;
    }
    else
    {
      phy_sensor_config.sample_rate = state->sampling_rate_hz;
      timestamp_val = state->prox_stream_config_timestamp;
    }
    phy_sensor_config.has_dri_enabled = true;
    phy_sensor_config.dri_enabled = (mode == AMS_STRM_IDX ? false : true);
    phy_sensor_config.has_water_mark = false;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
    phy_sensor_config.operation_mode.arg = &op_mode_args;
    phy_sensor_config.active_current = info.proxSensor.activeCurrent_uA;
    phy_sensor_config.range_count = 2;
    phy_sensor_config.range[0] = info.proxSensor.rangeMin;
    phy_sensor_config.range[1] = info.proxSensor.rangeMax;
    phy_sensor_config.has_stream_is_synchronous = false;
    phy_sensor_config.has_DAE_watermark = false;

    AMS_DEV_INST_PRINTF(LOW, instance, "tmd2725_prox_send_config_update: send PROX cfg evt");
    pb_send_event(instance,
                  sns_std_sensor_physical_config_event_fields,
                  &phy_sensor_config,
                  timestamp_val,
                  SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                  &state->prox_info.suid[mode]);
}
#endif

/**
 * Allocate Sensor State Raw Log Packet
 *
 * @param[i] diag       Pointer to diag service
 * @param[i] log_size   Optional size of log packet to
 *    be allocated. If not provided by setting to 0, will
 *    default to using maximum supported log packet size
 */
static void _tmd2725_log_sensor_state_raw_alloc(
  log_sensor_state_raw_info *log_raw_info,
  uint32_t log_size)
{
  uint32_t max_log_size = 0;

  if(NULL == log_raw_info || NULL == log_raw_info->diag ||
     NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid)
  {
    return;
  }

  // allocate memory for sensor state - raw sensor log packet
  max_log_size = log_raw_info->diag->api->get_max_log_size(
       log_raw_info->diag);

  if(0 == log_size)
  {
    // log size not specified.
    // Use max supported log packet size
    log_raw_info->log_size = max_log_size;
  }
  else if(log_size <= max_log_size)
  {
    log_raw_info->log_size = log_size;
  }
  else
  {
    return;
  }

  // Commenting out for debug of out of memory issue, pbnt 1/29/19
#ifdef TMD2725_FOR_SDM855_AND_LATER
   log_raw_info->log = log_raw_info->diag->api->alloc_log(
     log_raw_info->diag,
     log_raw_info->instance,
     log_raw_info->sensor_uid,
     log_raw_info->log_size,
     SNS_DIAG_SENSOR_STATE_LOG_RAW);
#endif

  log_raw_info->log_sample_cnt = 0;
  log_raw_info->bytes_written = 0;
}

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
/**
 * Encode als log sensor state raw packet
 *
 * @param[i] log Pointer to log packet information
 * @param[i] log_size Size of log packet information
 * @param[i] encoded_log_size Maximum permitted encoded size of
 *                            the log
 * @param[o] encoded_log Pointer to location where encoded
 *                       log should be generated
 * @param[o] bytes_written Pointer to actual bytes written
 *       during encode
 *
 * @return sns_rc
 * SNS_RC_SUCCESS if encoding was succesful
 * SNS_RC_FAILED otherwise
 */
static sns_rc tmd2725_encode_als_log_sensor_state_raw(
  void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
  size_t *bytes_written)
{
  sns_rc rc = SNS_RC_SUCCESS;
  uint32_t i = 0;
  size_t encoded_sample_size = 0;
  size_t parsed_log_size = 0;
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float temp[ALS_PROTO_STD_ARRAY_LEN];
  pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = ALS_PROTO_STD_ARRAY_LEN,
    .arr_index = &arr_index};

  if(NULL == encoded_log || NULL == log || NULL == bytes_written)
  {
    return SNS_RC_FAILED;
  }

  batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
  batch_sample.sample.arg = &arg;

  if(!pb_get_encoded_size(&encoded_sample_size, sns_diag_batch_sample_fields,
                          &batch_sample))
  {
    return SNS_RC_FAILED;
  }

  pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);
  als_batch_sample *raw_sample = (als_batch_sample *)log;

  while(parsed_log_size < log_size &&
        (stream.bytes_written + encoded_sample_size)<= encoded_log_size &&
        i < (uint32_t)(-1))
  {
    arr_index = 0;
    arg.arr = (float *)raw_sample[i].sample;

    batch_sample.sample_type = raw_sample[i].sample_type;
    batch_sample.status = raw_sample[i].status;
    batch_sample.timestamp = raw_sample[i].timestamp;

    if(!pb_encode_tag(&stream, PB_WT_STRING,
                      sns_diag_sensor_state_raw_sample_tag))
    {
      rc = SNS_RC_FAILED;
      break;
    }
    else if(!pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                                 &batch_sample))
    {
      rc = SNS_RC_FAILED;
      break;
    }

    parsed_log_size += sizeof(als_batch_sample);
    i++;
  }

  if (SNS_RC_SUCCESS == rc)
  {
    *bytes_written = stream.bytes_written;
  }

  return rc;
}
#endif


#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX
static sns_rc tmd2725_encode_prox_log_sensor_state_raw(
  void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
  size_t *bytes_written)
{
  sns_rc rc = SNS_RC_SUCCESS;
  uint32_t i = 0;
  size_t encoded_sample_size = 0;
  size_t parsed_log_size = 0;
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float temp[PROX_PROTO_STD_ARRAY_LEN];
  pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = PROX_PROTO_STD_ARRAY_LEN,
    .arr_index = &arr_index};

  if(NULL == encoded_log || NULL == log || NULL == bytes_written)
  {
    return SNS_RC_FAILED;
  }

  batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
  batch_sample.sample.arg = &arg;

  if(!pb_get_encoded_size(&encoded_sample_size, sns_diag_batch_sample_fields,
                          &batch_sample))
  {
    return SNS_RC_FAILED;
  }

  pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);
  prox_batch_sample *raw_sample = (prox_batch_sample *)log;

  while(parsed_log_size < log_size &&
        (stream.bytes_written + encoded_sample_size)<= encoded_log_size &&
        i < (uint32_t)(-1))
  {
    arr_index = 0;
    arg.arr = (float *)raw_sample[i].sample;

    batch_sample.sample_type = raw_sample[i].sample_type;
    batch_sample.status = raw_sample[i].status;
    batch_sample.timestamp = raw_sample[i].timestamp;

    if(!pb_encode_tag(&stream, PB_WT_STRING,
                      sns_diag_sensor_state_raw_sample_tag))
    {
      rc = SNS_RC_FAILED;
      break;
    }
    else if(!pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                                 &batch_sample))
    {
      rc = SNS_RC_FAILED;
      break;
    }

    parsed_log_size += sizeof(prox_batch_sample);
    i++;
  }

  if (SNS_RC_SUCCESS == rc)
  {
    *bytes_written = stream.bytes_written;
  }

  return rc;
}
#endif

/**
 *
 * Add raw uncalibrated sensor data to Sensor State Raw log
 * packet
 *
 * @param[i] log_raw_info Pointer to logging information
 *                        pertaining to the sensor
 * @param[i] raw_data     Uncalibrated sensor data to be logged
 * @param[i] timestamp    Timestamp of the sensor data
 * @param[i] status       Status of the sensor data
 *
 * * @return sns_rc,
 * SNS_RC_SUCCESS if encoding was succesful
 * SNS_RC_FAILED otherwise
 */
static sns_rc _tmd2725_log_sensor_state_raw_add(
  log_sensor_state_raw_info *log_raw_info,
  float *raw_data,
  sns_time timestamp,
  sns_std_sensor_sample_status status)
{
  sns_rc rc = SNS_RC_SUCCESS;

  if(NULL == log_raw_info || NULL == log_raw_info->diag ||
     NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
     NULL == raw_data || NULL == log_raw_info->log)
  {
    return SNS_RC_FAILED;
  }

  if(0)
  { ; }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  else if(log_raw_info->sensor_type == AMS_ALS_OC
     ||
     log_raw_info->sensor_type == AMS_ALS_STRM)
  {
    als_batch_sample *sample =
        (als_batch_sample *)log_raw_info->log;

    if(0 == log_raw_info->batch_sample_cnt)
    {
      sample[log_raw_info->log_sample_cnt].sample_type =
        SNS_DIAG_BATCH_SAMPLE_TYPE_FIRST;
    }
    else
    {
      sample[log_raw_info->log_sample_cnt].sample_type =
        SNS_DIAG_BATCH_SAMPLE_TYPE_INTERMEDIATE;
    }

    sample[log_raw_info->log_sample_cnt].timestamp = timestamp;

    sns_memscpy(sample[log_raw_info->log_sample_cnt].sample,
                sizeof(sample[log_raw_info->log_sample_cnt].sample),
                raw_data,
                sizeof(sample[log_raw_info->log_sample_cnt].sample));

    sample[log_raw_info->log_sample_cnt].status = status;

    log_raw_info->bytes_written += sizeof(als_batch_sample);

    log_raw_info->log_sample_cnt++;
    log_raw_info->batch_sample_cnt++;
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX
  else if(log_raw_info->sensor_type == AMS_PROX_OC
     ||
     log_raw_info->sensor_type == AMS_PROX_STRM)
  {
    prox_batch_sample *sample =
        (prox_batch_sample *)log_raw_info->log;

    if(0 == log_raw_info->batch_sample_cnt)
    {
      sample[log_raw_info->log_sample_cnt].sample_type =
        SNS_DIAG_BATCH_SAMPLE_TYPE_FIRST;
    }
    else
    {
      sample[log_raw_info->log_sample_cnt].sample_type =
        SNS_DIAG_BATCH_SAMPLE_TYPE_INTERMEDIATE;
    }

    sample[log_raw_info->log_sample_cnt].timestamp = timestamp;

    sns_memscpy(sample[log_raw_info->log_sample_cnt].sample,
                sizeof(sample[log_raw_info->log_sample_cnt].sample),
                raw_data,
                sizeof(sample[log_raw_info->log_sample_cnt].sample));

    sample[log_raw_info->log_sample_cnt].status = status;

    log_raw_info->bytes_written += sizeof(prox_batch_sample);

    log_raw_info->log_sample_cnt++;
    log_raw_info->batch_sample_cnt++;
  }
#endif
  return rc;
}

/**
 * Submit the Sensor State Raw Log Packet
 *
 * @param[i] log_raw_info   Pointer to logging information
 *       pertaining to the sensor
 * @param[i] batch_complete true if submit request is for end
 *       of batch
 *  */
static void _tmd2725_log_sensor_state_raw_submit(
  log_sensor_state_raw_info *log_raw_info,
  bool batch_complete)
{
  sns_diag_encode_log_cb cb;

  if(NULL == log_raw_info || NULL == log_raw_info->diag ||
     NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
     NULL == log_raw_info->log)
  {
    return;
  }

  if(0)
  { ; }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  else if(log_raw_info->sensor_type == AMS_ALS_OC
     ||
     log_raw_info->sensor_type == AMS_ALS_STRM)
  {
    als_batch_sample *sample = (als_batch_sample *)log_raw_info->log;

    if(batch_complete)
    {
      // overwriting previously sample_type for last sample
      if(1 == log_raw_info->batch_sample_cnt)
      {
        sample[0].sample_type =
          SNS_DIAG_BATCH_SAMPLE_TYPE_ONLY;
      }
      else if(1 < log_raw_info->batch_sample_cnt)
      {
        sample[log_raw_info->log_sample_cnt - 1].sample_type =
          SNS_DIAG_BATCH_SAMPLE_TYPE_LAST;
      }
    }
    cb = tmd2725_encode_als_log_sensor_state_raw;
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX
  else if(log_raw_info->sensor_type == AMS_PROX_OC
     ||
     log_raw_info->sensor_type == AMS_PROX_STRM)
  {
    prox_batch_sample *sample = (prox_batch_sample *)log_raw_info->log;

    if(batch_complete)
    {
      // overwriting previously sample_type for last sample
      if(1 == log_raw_info->batch_sample_cnt)
      {
        sample[0].sample_type =
          SNS_DIAG_BATCH_SAMPLE_TYPE_ONLY;
      }
      else if(1 < log_raw_info->batch_sample_cnt)
      {
        sample[log_raw_info->log_sample_cnt - 1].sample_type =
          SNS_DIAG_BATCH_SAMPLE_TYPE_LAST;
      }
    }
    cb = tmd2725_encode_prox_log_sensor_state_raw;
  }
#endif
  else
  {
    return;
  }

  log_raw_info->diag->api->submit_log(
        log_raw_info->diag,
        log_raw_info->instance,
        log_raw_info->sensor_uid,
        log_raw_info->bytes_written,
        log_raw_info->log,
        SNS_DIAG_SENSOR_STATE_LOG_RAW,
        log_raw_info->log_sample_cnt * log_raw_info->encoded_sample_size,
        cb);

  log_raw_info->log = NULL;
}

int32_t tmd2725_getCurrentLuxResult(ams_instance_state *state, sns_std_sensor_sample_status *sample_stat)
{ 
	ams_apiAls_t outData;
	int32_t lux = 0;
	
	tmd2725_deviceGetAls(state->priv, &outData);
	
	lux = (int32_t)(outData.mLux/1000.0);
	
	if((outData.quality & ALS_QUALITY_LUX_MASK) == ALS_QUALITY_LUX_GOOD)
    {
		*sample_stat = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
    }
	else
	{
		*sample_stat = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
	}
	
	return lux;
}
/**
 * Wrap up an ALS sample and generate an event.
 *
 * @param[i] state              The state of the sensor instance
 * @param[i] diag               The instance's diag_service
 * @param[i] timestamp          Timestamp to be used for this sample
 * @param[i] instance           The current sensor instance
 * @param[i] log_als_state_raw_info  Info on the log packet
 */
void tmd2725_generateAlsResponse(ams_instance_state *state,
                             sns_diag_service* diag,
                             sns_time timestamp,
                             sns_sensor_instance *const instance,
                             ams_reportType_t report_type)
{
  UNUSED_VAR(diag);
  ams_apiAls_t outData;
  float data[ALS_PROTO_STD_ARRAY_LEN];
  sns_std_sensor_sample_status status;
  log_sensor_state_raw_info log_als_state_raw_info;
  float lux = 0.0;
  int32_t lux_int;
  sns_rc rc;

  sns_memzero(&log_als_state_raw_info, sizeof(log_als_state_raw_info));
  log_als_state_raw_info.encoded_sample_size = state->als_enc_raw_log_size;
  log_als_state_raw_info.diag = diag;
  log_als_state_raw_info.instance = instance;

  tmd2725_deviceGetAls(state->priv, &outData);
  lux = (float)(outData.mLux)/1000.0;
  lux_int = (int32_t)lux;
  AMS_INST_PRINTF(HIGH, instance, "AMS:tmd2725_generateAlsResponse lux = %d", (uint32_t)lux);

  if((outData.quality & ALS_QUALITY_LUX_MASK) == ALS_QUALITY_LUX_GOOD)
  {
    status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
  }
  else
  {
    status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
  }
  state->als_info.status = status;
  data[0] = lux;
  data[1] = (float)(outData.rawClear);
#ifdef CONFIG_ALS_INCLUDE_IR_RESULTS
  data[2] = (float)(outData.rawIR);
#endif

  if((SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE != status) &&
     (state->publish_sensors & AMS_ALS_OC) &&
     (report_type & AMS_REPORT_ON_CHANGE))
  {
	// comparing 2 integer values is preferred
    if ((lux_int != state->als_info.prev_OC_report_int[0]) || (state->als_info.prev_OC_status[0] != status))
    {
      pb_send_sensor_stream_event(instance,
                                 &state->als_info.suid[AMS_OC_IDX],
                                 timestamp,
                                 SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                 status,
                                 data,
                                 ARR_SIZE(data),
                                 state->encoded_als_event_len);

      log_als_state_raw_info.sensor_uid = &state->als_info.suid[AMS_OC_IDX];
      log_als_state_raw_info.sensor_type = AMS_ALS_OC;

      _tmd2725_log_sensor_state_raw_alloc(&log_als_state_raw_info, sizeof(als_batch_sample));

      rc = _tmd2725_log_sensor_state_raw_add(
          &log_als_state_raw_info,
          data,
          timestamp,
          status);
      if (rc == SNS_RC_SUCCESS) {
          _tmd2725_log_sensor_state_raw_submit(&log_als_state_raw_info, true);
          AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: OC SENT RAW LOG");
      } else {
#ifdef TMD2725_FOR_SDM855_AND_LATER
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: rc=%d OC RAW LOG FAILURE!!!!!!!!!!!!!!", rc);
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: &log_als_state_raw_info = %x", &log_als_state_raw_info);
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: &log_als_state_raw_info->diag = %x", log_als_state_raw_info.diag);
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: &log_als_state_raw_info->instance = %x", log_als_state_raw_info.instance);
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: &log_als_state_raw_info->sensor_uid = %x", log_als_state_raw_info.sensor_uid);
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: &log_als_state_raw_info->log = %x", log_als_state_raw_info.log);
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: data = %x", data);
#endif

    }
	  state->als_info.prev_OC_status[0] = status;
	  state->als_info.prev_OC_report_int[0] = lux_int;
	
      if (status == SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH) {
          /* For on-change only, cache result data in the instance state.
           * This is so the framework can publish the cached data
           * to multiple clients.
           */
        state->als_info.timestamp = timestamp;
        state->als_info.prev_OC_report[0] = data[0];
        state->als_info.prev_OC_report[1] = data[1];
#ifdef CONFIG_ALS_INCLUDE_IR_RESULTS
        state->als_info.prev_OC_report[2] = data[2];
#endif
      }
    }
  }

  if((state->publish_sensors & AMS_ALS_STRM) &&
     (report_type & AMS_REPORT_STREAMING))
  {
    pb_send_sensor_stream_event(instance,
                               &state->als_info.suid[AMS_STRM_IDX],
                               timestamp,
                               SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                               status,
                               data,
                               ARR_SIZE(data),
                               state->encoded_als_event_len);

    log_als_state_raw_info.sensor_uid = &state->als_info.suid[AMS_STRM_IDX];
    log_als_state_raw_info.sensor_type = AMS_ALS_STRM;
    _tmd2725_log_sensor_state_raw_alloc(&log_als_state_raw_info, sizeof(als_batch_sample));
    rc = _tmd2725_log_sensor_state_raw_add(
        &log_als_state_raw_info,
        data,
        timestamp,
        status);
    if (rc == SNS_RC_SUCCESS) {
        _tmd2725_log_sensor_state_raw_submit(&log_als_state_raw_info, true);
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: STRM SENT RAW LOG");
    } else {
#ifdef TMD2725_FOR_SDM855_AND_LATER
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: rc=%d STRM RAW LOG FAILURE!!!!!!!!!!!!!!", rc);
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: &log_als_state_raw_info = %x", &log_als_state_raw_info);
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: &log_als_state_raw_info->diag = %x", log_als_state_raw_info.diag);
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: &log_als_state_raw_info->instance = %x", log_als_state_raw_info.instance);
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: &log_als_state_raw_info->sensor_uid = %x", log_als_state_raw_info.sensor_uid);
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: &log_als_state_raw_info->log = %x", log_als_state_raw_info.log);
        AMS_INST_PRINTF(HIGH, instance, "AMS: ALS: data = %x", data);
#endif

    }

  }
}

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
sns_std_sensor_sample_status _tmd2725_getProxAccuracy(ams_apiPrx_t prox_result )
{
    sns_std_sensor_sample_status accuracy;

    /*
    SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE = 0,
    SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_LOW = 1,
    SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_MEDIUM = 2,
    SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH = 3
    */

    switch (prox_result.accuracy)
    {
    case PRX_ACCURACY_LOW:
        accuracy = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_LOW;
        break;

    case PRX_ACCURACY_MED:
        accuracy = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_MEDIUM;
        break;

    case PRX_ACCURACY_HIGH:
        accuracy = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
        break;

    case PRX_ACCURACY_UNRELIABLE:
    default:
        accuracy = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
        break;
    }

    return accuracy;
}

ams_proximity_event_type tmd2725_getCurrentProxResult(ams_instance_state *state, sns_std_sensor_sample_status *sample_stat)
{
  ams_apiPrx_t outData;
  ams_proximity_event_type ams_newNearBy;

  tmd2725_deviceGetPrx(state->priv, &outData);
  *sample_stat = _tmd2725_getProxAccuracy(outData);
  ams_newNearBy = outData.nearBy ? AMS_PROXIMITY_EVENT_TYPE_NEAR : AMS_PROXIMITY_EVENT_TYPE_FAR;
  return  ams_newNearBy;
}

/**
 * Wrap up a Proximity sample and generate an event.
 *
 * @param[i] state              The state of the sensor instance
 * @param[i] diag               The instance's diag_service
 * @param[i] timestamp          Timestamp to be used for this sample
 * @param[i] instance           The current sensor instance
 * @param[i] log_prox_state_raw_info  Info on the log packet
 */
void tmd2725_generateProxResponse(ams_instance_state *state,
                              sns_diag_service* diag,
                              sns_time timestamp,
                              sns_sensor_instance *const instance,
                              log_sensor_state_raw_info *log_prox_state_raw_info,
                              ams_reportType_t report_type)
{
  UNUSED_VAR(diag);
  log_prox_state_raw_info->encoded_sample_size = state->prox_enc_raw_log_size;
  log_prox_state_raw_info->diag = diag;
  log_prox_state_raw_info->instance = instance;

  ams_apiPrx_t outData;
  ams_proximity_event_type ams_newNearBy;
  sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
  float data[PROX_PROTO_STD_ARRAY_LEN];

  tmd2725_deviceGetPrx(state->priv, &outData);
  status = _tmd2725_getProxAccuracy(outData);

  AMS_INST_PRINTF(HIGH, instance, "tmd2725_generateProxResponse, pdata=%2x, nearBy=%d, status = %d", outData.pData, outData.nearBy, status);

  state->prox_info.prox_state.raw_adc = outData.pData;
  state->prox_info.prox_state.status = status;
  /* The event type code conveys the binary nearBy status */
  state->prox_info.prox_state.proximity_event_type =
         (outData.nearBy ? SNS_PROXIMITY_EVENT_TYPE_NEAR : SNS_PROXIMITY_EVENT_TYPE_FAR);
  ams_newNearBy = outData.nearBy ? AMS_PROXIMITY_EVENT_TYPE_NEAR : AMS_PROXIMITY_EVENT_TYPE_FAR;

  data[0] = (float)(ams_newNearBy);
#ifdef CONFIG_PROX_INCLUDE_PROXIMITY_RESULTS
  data[1] = (float)((outData.pData + outData.proxOffset ) + 255);   // Keep prox adc value as a positive number
#endif

  if((SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE != status) &&
     (state->publish_sensors & AMS_PROX_OC) &&
     ((report_type == AMS_REPORT_ON_CHANGE) || (report_type == AMS_REPORT_OC_AND_STRM)))
  {
    if((ams_newNearBy != state->prox_info.prev_OC_report[0]) || (state->prox_info.prev_OC_status[0] != status))
    {
      pb_send_event(instance,
                    sns_proximity_event_fields,
                    &state->prox_info.prox_state,
                    timestamp,
                    SNS_PROXIMITY_MSGID_SNS_PROXIMITY_EVENT,
                    &state->prox_info.suid[AMS_OC_IDX]);

      /* For on-change only, cache result data in the instance state.
       * This is so the framework can publish the cached data
       * to multiple clients.
       */
      state->prox_info.prev_OC_report[0] = ams_newNearBy;
	  state->prox_info.prev_OC_status[0] = status;
      state->prox_info.timestamp = timestamp;

#ifdef CONFIG_PROX_INCLUDE_PROXIMITY_RESULTS
      pb_send_sensor_stream_event(instance,
                                &state->prox_info.suid[AMS_OC_IDX],
                                timestamp,
                                SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                status,
                                data,
                                ARR_SIZE(data),
                                state->encoded_prox_event_len);
#endif
/** Raw log packets are now required for Proximity, to achieve
 *  standardization of data post-processing tools. QC
 *  04Jun2019.*/
#ifdef TMD2725_FOR_SDM855_AND_LATER
        sns_rc rc;
        log_prox_state_raw_info->sensor_uid =
        &state->prox_info.suid[AMS_OC_IDX];
        log_prox_state_raw_info->sensor_type = AMS_PROX_OC;

        _tmd2725_log_sensor_state_raw_alloc(log_prox_state_raw_info, sizeof(prox_batch_sample));
        rc = _tmd2725_log_sensor_state_raw_add(
            log_prox_state_raw_info,
            data,
            timestamp,
            status);
      if (rc == SNS_RC_SUCCESS) {
          _tmd2725_log_sensor_state_raw_submit(log_prox_state_raw_info, true);
          AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: OC SENT RAW LOG");
      } else {
        AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: rc=%d OC RAW LOG FAILURE!!!!!!!!!!!!!!", rc);
        AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: &log_prox_state_raw_info = %x", log_prox_state_raw_info);
        AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: &log_prox_state_raw_info->diag = %x", log_prox_state_raw_info->diag);
        AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: &log_prox_state_raw_info->instance = %x", log_prox_state_raw_info->instance);
        AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: &log_prox_state_raw_info->sensor_uid = %x", log_prox_state_raw_info->sensor_uid);
        AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: &log_prox_state_raw_info->log = %x", log_prox_state_raw_info->log);
        AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: data = %x", data);
    }
#endif
    }
  }
  if((state->publish_sensors & AMS_PROX_STRM) &&
     ((report_type == AMS_REPORT_STREAMING) || (report_type == AMS_REPORT_OC_AND_STRM)))
  {
    pb_send_event(instance,
                  sns_proximity_event_fields,
                  &state->prox_info.prox_state,
                  timestamp,
#ifdef TMD2725_FOR_SDM855_AND_LATER
                  SNS_PROXIMITY_MSGID_SNS_PROXIMITY_EVENT_RECURRENT,
#else
                  SNS_PROXIMITY_MSGID_SNS_PROXIMITY_EVENT,
#endif

                  &state->prox_info.suid[AMS_STRM_IDX]);

#ifdef CONFIG_PROX_INCLUDE_PROXIMITY_RESULTS
    pb_send_sensor_stream_event(instance,
                                 &state->prox_info.suid[AMS_STRM_IDX],
                                 timestamp,
                                 SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                 status,
                                 data,
                                 ARR_SIZE(data),
                                 state->encoded_prox_event_len);
#endif
/** Raw log packets are not required for Proximity.
    QC 09May2019.*/
#ifdef TMD2725_FOR_SDM855_AND_LATER
    sns_rc rc;
    log_prox_state_raw_info->sensor_uid = &state->prox_info.suid[AMS_STRM_IDX];
    log_prox_state_raw_info->sensor_type = AMS_PROX_STRM;

    _tmd2725_log_sensor_state_raw_alloc(log_prox_state_raw_info, sizeof(prox_batch_sample));
    rc = _tmd2725_log_sensor_state_raw_add(
        log_prox_state_raw_info,
        data,
        timestamp,
        status);
      if (rc == SNS_RC_SUCCESS) {
          _tmd2725_log_sensor_state_raw_submit(log_prox_state_raw_info, true);
          AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: STRM SENT RAW LOG");
      } else {
        AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: rc=%d STRM RAW LOG FAILURE!!!!!!!!!!!!!!", rc);
        AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: &log_prox_state_raw_info = %x", log_prox_state_raw_info);
        AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: &log_prox_state_raw_info->diag = %x", log_prox_state_raw_info->diag);
        AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: &log_prox_state_raw_info->instance = %x", log_prox_state_raw_info->instance);
        AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: &log_prox_state_raw_info->sensor_uid = %x", log_prox_state_raw_info->sensor_uid);
        AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: &log_prox_state_raw_info->log = %x", log_prox_state_raw_info->log);
        AMS_INST_PRINTF(HIGH, instance, "AMS: PROX: data = %x", data);
    }
#endif
  }
}
#endif //CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB

/** See sns_sensor_instance_api::notify_event */
static sns_rc tmd2725_inst_notify_event(sns_sensor_instance *const this)
{
    ams_instance_state *state = (ams_instance_state*)this->state->state;
    sns_interrupt_event irq_event = sns_interrupt_event_init_zero;
    sns_sensor_event *event;
    sns_diag_service* diag = state->diag_service;
    ams_deviceCtx_t *devCtx = (ams_deviceCtx_t *)(state->priv);
    ams_ccb_als_ctx_t *alsCcbCtx = &((ams_deviceCtx_t*)devCtx)->ccbAlsCtx;
    ams_ccb_proximity_ctx_t *prxCcbCtx = &((ams_deviceCtx_t *)devCtx)->ccbProxCtx;
    int32_t updateAvailableFlags;
    bool handlerIsReady = (devCtx && devCtx->portHndl && devCtx->portHndl->scp_handle);
    sns_rc rv = SNS_RC_SUCCESS;
    bool intreg = false;
    bool interrupt_stream_valid = false;
    bool timer_stream_valid = false;

    AMS_DEV_INST_PRINTF(LOW, this
                        , "AMS: tmd2725_inst_notify_event: enter, data_streams: int %p, timer %p"
                        , state->interrupt_data_stream
                        , state->timer_data_stream
        );

    // Turn COM port ON
    rv = state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                           true);

    AMS_INST_PRINTF(LOW, this, "AMS: tmd2725_inst_notify_event: sns_scp_update_bus_power %d", rv);

    if(NULL != state->interrupt_data_stream)
    {
        interrupt_stream_valid = true;
    }

    if(NULL != state->timer_data_stream)
    {
        timer_stream_valid = true;
    }

    /*-------------------------------------------------------------------------------------------
     * Handle interrupt registration event
     *-------------------------------------------------------------------------------------------*/
    if (true == interrupt_stream_valid)
    {
        event = state->interrupt_data_stream->api->peek_input(state->interrupt_data_stream);
        if (NULL != event)
        {
            if (event->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT)
            {
                intreg = true;
                if (state->priv)
                {
                    SNS_INST_PRINTF(MED, this, "tmd2725_inst_notify_event: received SNS_INTERRUPT_REG_EVENT");
                    tmd2725_deviceSetConfig(this, state->priv, AMS_CONFIG_GLOBAL, AMS_CONFIG_ALLOW_INTS, 1);
                }
            }
        }
    }


    /*-------------------------------------------------------------------------------------------
     * Handle sensor HW
     *-------------------------------------------------------------------------------------------*/
    updateAvailableFlags = 0;
    if (handlerIsReady)
    {
        AMS_DEV_INST_PRINTF(LOW, this, "tmd2725_inst_notify_event: devCtx->ccbAlsCtx.initData.calibrationData.luxTarget %d", devCtx->ccbAlsCtx.initData.calibrationData.luxTarget);
        AMS_DEV_INST_PRINTF(LOW, this, "tmd2725_inst_notify_event: devCtx->ccbAlsCtx.initData.calibrationData.luxTargetError %d", devCtx->ccbAlsCtx.initData.calibrationData.luxTargetError);
        tmd2725_deviceEventHandler(this, devCtx);
        updateAvailableFlags = tmd2725_getResult(devCtx);
        AMS_DEV_INST_PRINTF(LOW, this, "AMS: updateAvailableFlags = %x", updateAvailableFlags);
    }
    else
    {
        return SNS_RC_SUCCESS;
    }

    if (state->new_self_test_request)
    {
        if ((updateAvailableFlags == AMS_CALIBRATION_DONE) ||
            (updateAvailableFlags == AMS_CALIBRATION_DONE_BUT_FAILED))
        {
            bool test_result = false;

            if (updateAvailableFlags == AMS_CALIBRATION_DONE)
            {
                test_result = true;
            }
            _tmd2725_wrap_up_factory_test(this, test_result);
            _tmd2725_check_and_remove_stream(this, AMS_OC_IDX, state->interrupt_data_stream);
            _tmd2725_check_and_remove_stream(this, AMS_STRM_IDX, state->timer_data_stream);

            return SNS_RC_SUCCESS;
        }
    }

    /*-------------------------------------------------------------------------------------------
     * Handle interrupts
     *-------------------------------------------------------------------------------------------*/
    if(true == interrupt_stream_valid)
    {
        if (NULL != event)
        {
            if (intreg == true)
            {
                /* Already handled the interrupt registration event
                 * pop it and get the next event if there is one
                 */
                event = state->interrupt_data_stream->api->get_next_input(state->interrupt_data_stream);
            }

            while(NULL != event)
            {
                AMS_DEV_INST_PRINTF(LOW, this, "AMS: tmd2725_inst_notify_event: next interrupt event:  id=%d", event->message_id);

                if (event->message_id != SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT)
                {
                    AMS_DEV_INST_PRINTF(LOW, this, "tmd2725_inst_notify_event: received non-interrupt INT event id=%d", event->message_id);
                }
                else
                {
                    pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                                 event->event_len);
                    if(state->publish_sensors & AMS_ON_CHANGERS)
                    {
                        AMS_DEV_INST_PRINTF(LOW, this, "AMS: Received valid INT event");
                        if(pb_decode(&stream, sns_interrupt_event_fields, &irq_event))
                        {
                            if (irq_event.timestamp == 0)
                            {
                                state->interrupt_timestamp = sns_get_system_time();
                            }
                            else
                            {
                                state->interrupt_timestamp = irq_event.timestamp;
                            }

                            sns_std_sensor_sample_status  prox_loc_st;
                            ams_proximity_event_type prox_loc_ev;
                            int32_t als_loc_lux;
                            sns_std_sensor_sample_status als_loc_st;

                            /* No regular reports if self testing */
                            if (!state->new_self_test_request)
                            {
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS

                                als_loc_lux = tmd2725_getCurrentLuxResult(state, &als_loc_st);

                                AMS_DEV_INST_PRINTF(LOW, this, "AMS: als_loc_lux =%d, als prev_OC_report_int = %d, als_loc_st = %d, als prev_OC_status = %d",
                                                    (uint32_t)als_loc_lux, (uint32_t)state->als_info.prev_OC_report_int[0],
                                                    als_loc_st,state->als_info.prev_OC_status[0]);
                                if ((updateAvailableFlags & (1 << AMS_AMBIENT_SENSOR)) ||
                                    (state->als_info.prev_OC_report_int[0] != als_loc_lux) ||
                                    (state->als_info.prev_OC_status[0] != als_loc_st))
                                {
                                    AMS_DEV_INST_PRINTF(LOW, this, "AMS: updateAvailable => AMS_AMBIENT_SENSOR");
                                    if(state->publish_sensors & AMS_ALS_OC)
                                    {
                                        tmd2725_generateAlsResponse(state, diag, state->interrupt_timestamp, this, AMS_REPORT_ON_CHANGE);
                                        if (updateAvailableFlags & (1 << AMS_ALS_GAIN_CHANGED))
                                        {
                                            AMS_DEV_INST_PRINTF(LOW, this, "AMS: updateAvailable => AMS_ALS_GAIN_CHANGED");
                                            tmd2725_als_send_config_update(this, AMS_OC_IDX);
                                        }
                                    }
                                } //AMS_AMBIENT_SENSOR update available
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
                                prox_loc_ev = tmd2725_getCurrentProxResult(state, &prox_loc_st);
                                AMS_DEV_INST_PRINTF(LOW, this, "AMS: prox_loc_ev =%d, prox_info.prev_OC_report = %d, prox_loc_st = %d, prox_info.prev_OC_status = %d",
                                                    (uint32_t)prox_loc_ev, (uint32_t)state->prox_info.prev_OC_report[0],
                                                    prox_loc_st,state->prox_info.prev_OC_status[0]);
                                if ((updateAvailableFlags & (1 << AMS_PROXIMITY_SENSOR)) ||
                                    (state->prox_info.prev_OC_report[0] != prox_loc_ev) ||
                                    (state->prox_info.prev_OC_status[0] != prox_loc_st))
                                {
                                    AMS_DEV_INST_PRINTF(HIGH, this, "AMS: updateAvailable => AMS_PROXIMITY_SENSOR");
                                    if(state->publish_sensors & AMS_PROX_OC)
                                    {
                                        log_sensor_state_raw_info log_prox_state_raw_info;
                                        AMS_DEV_INST_PRINTF(HIGH, this, "AMS: Interrupt Prox ON-CHANGE reporting");
                                        // Allocate Sensor State Raw log packets
                                        sns_memzero(&log_prox_state_raw_info, sizeof(log_prox_state_raw_info));
                                        tmd2725_generateProxResponse(state, diag, state->interrupt_timestamp,
                                                                     this, &log_prox_state_raw_info, AMS_REPORT_ON_CHANGE);
                                    }
                                }
#endif
                            }
                        }
                        else
                        {
                            SNS_INST_PRINTF(ERROR, this, "AMS: pb_decode error");
                        }
                    }
                }
                event = state->interrupt_data_stream->api->get_next_input(state->interrupt_data_stream);
            }
        }
        AMS_DEV_INST_PRINTF(LOW, this, "AMS: tmd2725_inst_notify_event: no more interrupt events");
    }

    /*-------------------------------------------------------------------------------------------
     * Handle timer events
     *-------------------------------------------------------------------------------------------*/
    AMS_DEV_INST_PRINTF(LOW, this, "AMS: tmd2725_inst_notify_event: start of timer handler");


    if(true == timer_stream_valid)
    {
        event = state->timer_data_stream->api->peek_input(state->timer_data_stream);
        while(NULL != event)
        {
            pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                         event->event_len);
            sns_timer_sensor_event timer_event;

            AMS_DEV_INST_PRINTF(LOW, this
                                , "AMS: tmd2725_inst_notify_event: next timer event:  id=%d; timer_is_active=%d"
                                , event->message_id
                                , state->timer_is_active
                );

            if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
            {
                if ((event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT) &&
                    (state->timer_is_active == true )                             &&
                    (state->sampling_intvl > 0))
                {
                    AMS_DEV_INST_PRINTF(LOW, this, "AMS: tmd2725_inst_notify_event: poll timer: publish_sensors 0x%04x", state->publish_sensors);

                    /* No regular reports if self testing */
                    if (!state->new_self_test_request)
                    {
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
                        if(state->publish_sensors & AMS_ALS_STRM)
                        {
                            if(!alsCcbCtx->wait_for_clear)
                            {
                                tmd2725_generateAlsResponse(state, diag, timer_event.timeout_time, this, AMS_REPORT_STREAMING);
                                if (updateAvailableFlags & (1 << AMS_ALS_GAIN_CHANGED))
                                {
                                    tmd2725_als_send_config_update(this, AMS_STRM_IDX);
                                }
                            }
                        }
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
                        if(state->publish_sensors & AMS_PROX_STRM)
                        {
                            if(!prxCcbCtx->wait_for_clear)
                            {
                                log_sensor_state_raw_info log_prox_state_raw_info;
                                sns_memzero(&log_prox_state_raw_info, sizeof(log_prox_state_raw_info));
                                if (timer_event.timeout_time == 0) {
                                    AMS_DEV_INST_PRINTF(HIGH, this, "AMS: tmd2725_inst_notify_event: TIMESTAMP IS ZERO!!!!!!!!!!!!!!!!!!");
                                }
                                AMS_DEV_INST_PRINTF(HIGH, this, "AMS: tmd2725_inst_notify_event: PROX streaming reporting ");
                                tmd2725_generateProxResponse(state, diag, timer_event.timeout_time,
                                                             this, &log_prox_state_raw_info, AMS_REPORT_STREAMING);
                            }
                        }
#endif
                    }
                }
                else
                {
                    AMS_DEV_INST_PRINTF(LOW, this
                                        , "tmd2725_inst_notify_event: ignoring timer evt; timer_is_active=%d, S.I. > 0? _%d_"
                                        , state->timer_is_active
                                        , (state->sampling_intvl > 0)
                        );
                }
            }
            else
            {
                SNS_INST_PRINTF(ERROR, this, "tmd2725_inst_notify_event: ERROR decoding timer_data_stream event");
            }
            event = state->timer_data_stream->api->get_next_input(state->timer_data_stream);
        }
        AMS_DEV_INST_PRINTF(LOW, this, "AMS: tmd2725_inst_notify_event: no more timer_data_stream events");
    }  /* End handle timer events */

    // Turn COM port OFF
    rv = state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
    AMS_INST_PRINTF(LOW, this, "AMS: tmd2725_inst_notify_event: sns_scp_update_bus_power %d", rv);

    return SNS_RC_SUCCESS;
}

static void _tmd2725_check_and_remove_stream(sns_sensor_instance *this,
                                             ams_mode_index oc_strm,
                                             sns_data_stream *stream)
{
    ams_instance_state *state;
    bool test_client_present = false;
    sns_rc rv;

    AMS_INST_PRINTF(MED, this, "_tmd2725_check_and_remove_stream: enter, oc_strm=%d", oc_strm);

    if (NULL == stream) {
        return;
    }
    state = (ams_instance_state *) this->state->state;

    if (state->als_info.test_info[oc_strm].test_client_present)
    {
        AMS_INST_PRINTF(MED, this, "_tmd2725_check_and_remove_stream: als test client for %d", oc_strm);
        state->als_info.test_info[oc_strm].test_client_present = false;
        test_client_present = true;
    }
    else
    {
        AMS_INST_PRINTF(MED, this, "_tmd2725_check_and_remove_stream: no als test client for %d", oc_strm);
    }


    if (state->prox_info.test_info[oc_strm].test_client_present)
    {
        AMS_INST_PRINTF(MED, this, "_tmd2725_check_and_remove_stream: prox test client for %d", oc_strm);
        state->prox_info.test_info[oc_strm].test_client_present = false;
        test_client_present = true;
    }
    else
    {
        AMS_INST_PRINTF(MED, this, "_tmd2725_check_and_remove_stream: no prox test client for %d", oc_strm);
    }


    if ((test_client_present == true) &&
        (state->client_present == 0))
    {
        AMS_INST_PRINTF(MED, this, "_tmd2725_check_and_remove_stream: removing state->timer_data_stream %x", state->timer_data_stream);

        sns_service_manager *service_mgr = this->cb->get_service_manager(this);
        sns_stream_service *stream_mgr = (sns_stream_service*)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
        rv = stream_mgr->api->remove_stream(stream_mgr, stream);
        AMS_INST_PRINTF(MED, this, "_tmd2725_check_and_remove_stream: stream_mgr->api->remove_stream rv = %d ", rv);

        if (oc_strm == AMS_STRM_IDX)
        {
            state->timer_is_active = false;
            state->timer_data_stream = NULL;
        }
        else
        {
            state->interrupt_data_stream = NULL;
        }
    }
    else
    {
        AMS_INST_PRINTF(MED, this, "_tmd2725_check_and_remove_stream: cannot remove timer stream, tc=%d, cp=%d", test_client_present, state->client_present);
    }
}

/**
 * Publishes a calibration event
 *
 * @param[i] instance  Instance reference
 * @param[i] uid       Sensor UID
 * @param[i] scale_result
 * @param[i] scale_size
 * @param[i] bias_result
 * @param[i] bias_size
 *
 * @return none
 */
void _tmd2725_send_cal_event(sns_sensor_instance *instance,
                                sns_sensor_uid *uid,
                                float *scale_result, uint8_t scale_size,
                                float *bias_result, uint8_t bias_size,
                                sns_time timestamp)
{
  uint8_t arr_index = 0;
  float scale_arr[20];
  float bias_arr[20];
  pb_float_arr_arg scale_arg;
  pb_float_arr_arg bias_arg;
  sns_cal_event cal_event = sns_cal_event_init_default;
  cal_event.status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

  if (0 == scale_size && 0 == bias_size) {
    return;
  }


  AMS_DEV_INST_PRINTF(LOW, instance, "_tmd2725_send_cal_event\n");
  if (uid == NULL)
  {
      SNS_INST_PRINTF(ERROR, instance, "ERROR: _tmd2725_send_cal_event uid == NULL\n");
  }


  if (scale_size > 0)
  {
    sns_memscpy (scale_arr, sizeof(float) * scale_size, scale_result, sizeof(float) * scale_size);
    scale_arg.arr = scale_arr;
    scale_arg.arr_len = scale_size;
    scale_arg.arr_index = &arr_index;

    cal_event.scale_factor.funcs.encode = &pb_encode_float_arr_cb;
    cal_event.scale_factor.arg = &scale_arg;
  }
  if (bias_size > 0)
  {
    sns_memscpy (bias_arr, sizeof(float) * bias_size, bias_result, sizeof(float) * bias_size);
    bias_arg.arr = bias_arr;
    bias_arg.arr_len = bias_size;
    bias_arg.arr_index = &arr_index;

    cal_event.bias.funcs.encode = &pb_encode_float_arr_cb;
    cal_event.bias.arg = &bias_arg;
  }

  pb_send_event(instance,
                sns_cal_event_fields,
                &cal_event,
                timestamp,
                SNS_CAL_MSGID_SNS_CAL_EVENT,
                uid);

  AMS_DEV_INST_PRINTF(LOW, instance, "CALIBRATION COMPLETED\n");
}

/**
 * Runs a factory test
 * test event.
 *
 * @param[i] instance  Instance reference
 * @param[i] uid       Sensor UID
 *
 * @return none
 */
void _tmd2725_send_factory_test_event(sns_sensor_instance *instance,
                                  sns_sensor_uid *uid, bool test_result)
{
  uint8_t data[1] = {0};
  pb_buffer_arg buff_arg = (pb_buffer_arg)
      { .buf = &data, .buf_len = sizeof(data) };
  sns_physical_sensor_test_event test_event =
     sns_physical_sensor_test_event_init_default;
  test_event.test_passed = test_result;
  test_event.test_type = SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY;
  test_event.test_data.funcs.encode = &pb_encode_string_cb;
  test_event.test_data.arg = &buff_arg;

  pb_send_event(instance,
                sns_physical_sensor_test_event_fields,
                &test_event,
                sns_get_system_time(),
                SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                uid);
  return;
}

void _tmd2725_send_factory_hw_test_event(sns_sensor_instance *instance,
                                  sns_sensor_uid *uid, bool test_result)
{
  uint8_t data[1] = {0};
  pb_buffer_arg buff_arg = (pb_buffer_arg)
      { .buf = &data, .buf_len = sizeof(data) };
  sns_physical_sensor_test_event test_event =
     sns_physical_sensor_test_event_init_default;
  test_event.test_passed = test_result;
  test_event.test_type = SNS_PHYSICAL_SENSOR_TEST_TYPE_HW;
  test_event.test_data.funcs.encode = &pb_encode_string_cb;
  test_event.test_data.arg = &buff_arg;

  pb_send_event(instance,
                sns_physical_sensor_test_config_fields,
                &test_event,
                sns_get_system_time(),
                SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                uid);
  return;
}

/**
 * Collects factory test results and submits for replying and/or updating
 * the cal data in the registry.
 *
 * @param[i] instance     Instance reference
 * @param[i] test_result  Test pass/fail result if already known, else NULL
 *
 * @return none
 */
static void _tmd2725_wrap_up_factory_test(sns_sensor_instance *this,
                                          bool test_result)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;
  ams_deviceCtx_t *devCtx = (ams_deviceCtx_t *)(state->priv);
  ams_ccb_als_ctx_t * alsCcbCtx = &((ams_deviceCtx_t*)devCtx)->ccbAlsCtx;
  ams_ccb_proximity_ctx_t *prxCcbCtx = &((ams_deviceCtx_t *)devCtx)->ccbProxCtx;

  /* Assume that there is no reason to update the registry. */
  state->need_to_update_registry = false;
  state->registry_write_type = AMS_REGISTRY_WRITE_TYPE_NONE;

  AMS_INST_PRINTF(MED, this
                  , "AMS: _tmd2725_wrap_up_factory_test: enter: instance %p, test_result=%d"
                  , this
                  , test_result
      );

  /* Restore the enable and intenab settings the state before the self test */
  tmd2725_restore_enable_settings(devCtx);

  /* Assume no more than 1 sensor can be self-tested at a time */
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  if(state->als_info.test_info[AMS_OC_IDX].test_client_present)
  {
    if(state->als_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      float result;
      AMS_INST_PRINTF(MED, this
                      , "AMS: _tmd2725_wrap_up_factory_test: ALS_OC, test_result=%d, wait_for_clear = true"
                      , test_result
                      );
      tmd2725_als_send_config_update(this, AMS_OC_IDX);
      state->calibration_data_tmd2725.alsCalibrationFactor = devCtx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor;
      state->cal_version[ALSCALIBRATIONFACTOR_IDX]++;
      state->als_cal_timestamp = sns_get_system_time();
      result = (float) devCtx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor;
      _tmd2725_send_cal_event (this, &state->als_info.suid[AMS_OC_IDX],
         &result, 1, NULL, 0, state->als_cal_timestamp);
      _tmd2725_send_factory_test_event(this, &state->als_info.suid[AMS_OC_IDX], test_result);
      state->new_self_test_request = false;
      state->need_to_update_registry = true;
      state->registry_write_type = AMS_REGISTRY_WRITE_TYPE_ALS_FAC_CAL;
      alsCcbCtx->wait_for_clear = true;
    }
    return;
  }

  if(state->als_info.test_info[AMS_STRM_IDX].test_client_present)
  {
    if(state->als_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      float result;
      AMS_INST_PRINTF(MED, this
                      , "AMS: _tmd2725_wrap_up_factory_test: ALS_STRM, test_result=%d, wait_for_clear = true"
                      , test_result
                      );
      tmd2725_als_send_config_update(this, AMS_STRM_IDX);
      state->calibration_data_tmd2725.alsCalibrationFactor = devCtx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor;
      state->cal_version[ALSCALIBRATIONFACTOR_IDX]++;
      state->als_cal_timestamp = sns_get_system_time();
      result = (float) devCtx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor;
      _tmd2725_send_cal_event (this, &state->als_info.suid[AMS_STRM_IDX],
         &result, 1, NULL, 0, state->als_cal_timestamp);
      _tmd2725_send_factory_test_event(this, &state->als_info.suid[AMS_STRM_IDX], test_result);
      state->new_self_test_request = false;
      state->need_to_update_registry = true;
      state->registry_write_type = AMS_REGISTRY_WRITE_TYPE_ALS_FAC_CAL;
      alsCcbCtx->wait_for_clear = true;
    }
    return;
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  if(state->prox_info.test_info[AMS_OC_IDX].test_client_present)
  {
    if(state->prox_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      float result;
      AMS_INST_PRINTF(MED, this
                      , "AMS: _tmd2725_wrap_up_factory_test: PROX_OC, test_result=%d, wait_for_clear = true"
                      , test_result
                      );
      tmd2725_prox_send_config_update(this, AMS_OC_IDX);
      state->calibration_data_tmd2725.proxHardThreshold = devCtx->systemCalibrationData->proxHardThreshold;
      state->cal_version[PROXHARDTHRESHOLD_IDX]++;
      state->prox_fac_cal_timestamp = sns_get_system_time();
      result = (float)((devCtx->systemCalibrationData->proxHardThreshold) >> 8);
      _tmd2725_send_cal_event (this, &state->prox_info.suid[AMS_OC_IDX],
         &result, 1, NULL, 0, state->prox_fac_cal_timestamp);
      _tmd2725_send_factory_test_event(this, &state->prox_info.suid[AMS_OC_IDX], test_result);
      state->new_self_test_request = false;
      state->need_to_update_registry = true;
      state->registry_write_type = AMS_REGISTRY_WRITE_TYPE_PROX_FAC_CAL;
      prxCcbCtx->wait_for_clear = true;
    }
    if(state->prox_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_HW)
    {
      float result[2];
      AMS_INST_PRINTF(MED, this
                      , "AMS: _tmd2725_wrap_up_factory_test: HW CALIB PROX_OC, test_result=%d, wait_for_clear = true"
                      , test_result
                      );
      tmd2725_prox_send_config_update(this, AMS_OC_IDX);
      state->calibration_data_tmd2725.proxOffset = devCtx->systemCalibrationData->proxOffset;
      state->calibration_data_tmd2725.proxOffsetSign = devCtx->systemCalibrationData->proxOffsetSign;
      state->cal_version[PROXOFFSET_IDX]++;
      state->cal_version[PROXOFFSETSIGN_IDX]++;
      state->prox_hw_cal_timestamp = sns_get_system_time();
      result[0] = (float) devCtx->systemCalibrationData->proxOffset;
      result[1] = (float) devCtx->systemCalibrationData->proxOffsetSign;
      _tmd2725_send_cal_event (this, &state->prox_info.suid[AMS_OC_IDX],
         result, 2, NULL, 0, state->prox_hw_cal_timestamp);
      _tmd2725_send_factory_hw_test_event(this, &state->prox_info.suid[AMS_OC_IDX], test_result);
      state->new_self_test_request = false;
      state->need_to_update_registry = true;
      state->registry_write_type = AMS_REGISTRY_WRITE_TYPE_PROX_HW_CAL;
      prxCcbCtx->wait_for_clear = true;
    }
    return;
  }

  if(state->prox_info.test_info[AMS_STRM_IDX].test_client_present)
  {
    if(state->prox_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      float result;
      AMS_INST_PRINTF(MED, this
                      , "AMS: _tmd2725_wrap_up_factory_test: PROX_STRM, test_result=%d, wait_for_clear = true"
                      , test_result
                      );
      tmd2725_prox_send_config_update(this, AMS_STRM_IDX);
      state->calibration_data_tmd2725.proxHardThreshold = devCtx->systemCalibrationData->proxHardThreshold;
      state->cal_version[PROXHARDTHRESHOLD_IDX]++;
      state->prox_fac_cal_timestamp = sns_get_system_time();
      result = (float)((devCtx->systemCalibrationData->proxHardThreshold) >> 8);
      _tmd2725_send_cal_event (this, &state->prox_info.suid[AMS_STRM_IDX],
         &result, 1, NULL, 0, state->prox_fac_cal_timestamp);
      _tmd2725_send_factory_test_event(this, &state->prox_info.suid[AMS_STRM_IDX], test_result);
      state->new_self_test_request = false;
      state->need_to_update_registry = true;
      state->registry_write_type = AMS_REGISTRY_WRITE_TYPE_PROX_FAC_CAL;
      prxCcbCtx->wait_for_clear = true;
    }
    if(state->prox_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_HW)
    {
      float result[2];
      AMS_INST_PRINTF(MED, this
                      , "AMS: _tmd2725_wrap_up_factory_test: HW CALIB PROX_STRM, test_result=%d, wait_for_clear = true"
                      , test_result
                      );
      tmd2725_prox_send_config_update(this, AMS_STRM_IDX);
      state->calibration_data_tmd2725.proxOffset = devCtx->systemCalibrationData->proxOffset;
      state->calibration_data_tmd2725.proxOffsetSign = devCtx->systemCalibrationData->proxOffsetSign;
      state->cal_version[PROXOFFSET_IDX]++;
      state->cal_version[PROXOFFSETSIGN_IDX]++;
      state->prox_hw_cal_timestamp = sns_get_system_time();
      result[0] = (float) devCtx->systemCalibrationData->proxOffset;
      result[1] = (float) devCtx->systemCalibrationData->proxOffsetSign;
      _tmd2725_send_cal_event (this, &state->prox_info.suid[AMS_STRM_IDX],
         result, 2, NULL, 0, state->prox_hw_cal_timestamp);
      _tmd2725_send_factory_hw_test_event(this, &state->prox_info.suid[AMS_STRM_IDX], test_result);
      state->new_self_test_request = false;
      state->need_to_update_registry = true;
      state->registry_write_type = AMS_REGISTRY_WRITE_TYPE_PROX_HW_CAL;
      prxCcbCtx->wait_for_clear = true;
    }
    return;
  }
#endif
}

void _tmd2725_factory_test_no_sensor_found(sns_sensor_instance *this)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;
  ams_calibrationData_t *instance_calibration = &state->calibration_data_tmd2725;
  float result = 0.0;

  /* Assume that there is no reason to update the registry. */
  state->need_to_update_registry = false;
  state->registry_write_type = AMS_REGISTRY_WRITE_TYPE_NONE;

  /* Assume no more than 1 sensor can be self-tested at a time */
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  if(state->als_info.test_info[AMS_OC_IDX].test_client_present)
  {
    if(state->als_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      result = (float) instance_calibration->alsCalibrationFactor;
      AMS_INST_PRINTF(MED, this, "AMS: _tmd2725_factory_test_no_sensor_found");
      _tmd2725_send_cal_event (this, &state->als_info.suid[AMS_OC_IDX],
         &result, 1, NULL, 0, state->als_cal_timestamp);
      _tmd2725_send_factory_test_event(this, &state->als_info.suid[AMS_OC_IDX],0);
      state->new_self_test_request = false;
    }
    return;
  }

  if(state->als_info.test_info[AMS_STRM_IDX].test_client_present)
  {
    if(state->als_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      result = (float) instance_calibration->alsCalibrationFactor;
      AMS_INST_PRINTF(MED, this, "AMS: _tmd2725_factory_test_no_sensor_found");
      _tmd2725_send_cal_event (this, &state->als_info.suid[AMS_STRM_IDX],
         &result, 1, NULL, 0, state->als_cal_timestamp);
      _tmd2725_send_factory_test_event(this, &state->als_info.suid[AMS_STRM_IDX], 0);
      state->new_self_test_request = false;
    }
    return;
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  if(state->prox_info.test_info[AMS_OC_IDX].test_client_present)
  {
    if(state->prox_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      result = (float) (instance_calibration->proxHardThreshold>>8);
      AMS_INST_PRINTF(MED, this, "AMS: _tmd2725_factory_test_no_sensor_found");
      _tmd2725_send_cal_event (this, &state->prox_info.suid[AMS_OC_IDX],
         &result, 1, NULL, 0, state->prox_fac_cal_timestamp);
      _tmd2725_send_factory_test_event(this, &state->prox_info.suid[AMS_OC_IDX], 0);
      state->new_self_test_request = false;
    }
    return;
  }

  if(state->prox_info.test_info[AMS_STRM_IDX].test_client_present)
  {
    if(state->prox_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      result = (float) (instance_calibration->proxHardThreshold>>8);
      AMS_INST_PRINTF(MED, this, "AMS: _tmd2725_factory_test_no_sensor_found");
      _tmd2725_send_cal_event (this, &state->prox_info.suid[AMS_STRM_IDX],
         &result, 1, NULL, 0, state->prox_fac_cal_timestamp);
      _tmd2725_send_factory_test_event(this, &state->prox_info.suid[AMS_STRM_IDX], 0);
      state->new_self_test_request = false;
    }
    return;
  }
#endif
}

/** Public Data Definitions. */

sns_sensor_instance_api tmd2725_sensor_instance_api =
{
  .struct_len        = sizeof(sns_sensor_instance_api),
  .init              = &tmd2725_inst_init,
  .deinit            = &tmd2725_inst_deinit,
  .set_client_config = &tmd2725_inst_set_client_config,
  .notify_event      = &tmd2725_inst_notify_event
};
