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

#include "sns_tmd2725_sensor.h"

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS // entire file

extern const ams_calibrationData_t default_common_cal_data_tmd2725;
extern const ams_default_als_config_data_t default_als_config_data;
extern const ams_default_prox_config_data_t default_prox_config_data;
/**
 * Publish all Sensor attributes
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void
tmd2725_publish_als_attributes(sns_sensor *const this, sns_std_sensor_stream_type stream_type)
{
  ams_deviceInfo_t info;
  int32_t hwid = this->cb->get_registration_index(this);
  if (stream_type == SNS_STD_SENSOR_STREAM_TYPE_STREAMING)
  {
      AMS_DEV_PRINTF(LOW, this, "AMS: tmd2725_publish_als_attributes, hwid: %d, streaming", hwid);
  }
  else if (stream_type == SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE)
  {
      AMS_DEV_PRINTF(LOW, this, "AMS: tmd2725_publish_als_attributes, hwid: %d, on-change", hwid);
  }

  tmd2725_getDeviceInfo(&info, hwid);

  ams_state *state = (ams_state*)this->state->state;
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};

    sns_std_attr_value_data range1[] = {SNS_ATTR, SNS_ATTR};
    range1[0].has_flt = true;
    range1[0].flt = info.alsSensor.rangeMin;
    range1[1].has_flt = true;
    range1[1].flt = info.alsSensor.rangeMax;
    values[0].has_subtype = true;
    values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
    values[0].subtype.values.arg =
      &((pb_buffer_arg){ .buf = range1, .buf_len = ARR_SIZE(range1) });
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RANGES,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};

    sns_std_attr_value_data range1[] = {SNS_ATTR, SNS_ATTR};
    range1[0].has_flt = true;
    range1[0].flt = info.alsSensor.rangeMin;
    range1[1].has_flt = true;
    range1[1].flt = info.alsSensor.rangeMax;
    values[0].has_subtype = true;
    values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
    values[0].subtype.values.arg =
      &((pb_buffer_arg){ .buf = range1, .buf_len = ARR_SIZE(range1) });
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_SELECTED_RANGE,
      values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    values[0].has_sint = true;
    values[0].sint = stream_type;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE,
        values, ARR_SIZE(values), false);
  }
  {
   sns_std_attr_value_data values[] = {SNS_ATTR};
   values[0].has_sint = true;
   values[0].sint = info.alsSensor.activeCurrent_uA;
   sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT,
       values, ARR_SIZE(values), false);
  }
  {
   sns_std_attr_value_data values[] = {SNS_ATTR};
   values[0].has_sint = true;
   values[0].sint = info.alsSensor.standbyCurrent_uA;
   sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT,
       values, ARR_SIZE(values), false);
  }

  if (stream_type == SNS_STD_SENSOR_STREAM_TYPE_STREAMING)
  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
    values[0].has_flt = true;
    values[0].flt = AMS_DEVICE_ODR_1;
    values[1].has_flt = true;
    values[1].flt = AMS_DEVICE_ODR_2;
    values[2].has_flt = true;
    values[2].flt = AMS_DEVICE_ODR_5;
    values[3].has_flt = true;
    values[3].flt = AMS_DEVICE_ODR_10;

    state->odr_for_selftest = AMS_DEVICE_ODR_10;

    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RATES,
        values, ARR_SIZE(values), false);
  } else {
    /* _ATTRID_RATES not supported; this is an on-change sensor */
    state->odr_for_selftest = 0.0;
  }

  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    values[0].has_flt = true;
    values[0].flt = info.alsSensor.alsResolution;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RESOLUTIONS,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    values[0].has_flt = true;
    values[0].flt = info.alsSensor.alsResolution;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    char const op_mode1[] = AMS_OPMODE_NORMAL;

    values[0].str.funcs.encode = pb_encode_string_cb;
    values[0].str.arg = &((pb_buffer_arg)
        { .buf = op_mode1, .buf_len = sizeof(op_mode1) });
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_OP_MODES,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    char const proto1[] = "sns_ambient_light.proto";

    values[0].str.funcs.encode = pb_encode_string_cb;
    values[0].str.arg = &((pb_buffer_arg)
        { .buf = proto1, .buf_len = sizeof(proto1) });
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_API,
        values, ARR_SIZE(values), false);
  }
  {
    char const name[] = "TMD2725ALSPRX";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
      /*
       * NOTE: Any attributes that are strings must not be null terminated
       * because the at some point in the SEE framework this data is
       * converted to JSON and the JSON parser will throw an error if
       * it encounters NULL characters in a string.
       */
#ifdef CONFIG_AMS_USE_TEST_SENSORS
    if (stream_type == SNS_STD_SENSOR_STREAM_TYPE_STREAMING){
      if(this->cb->get_registration_index(this) == 1){
          char const type[] = "ambient_light_strm_1";
          sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
          value.str.funcs.encode = pb_encode_string_cb;
          value.str.arg = &((pb_buffer_arg)
              { .buf = type, .buf_len = sizeof(type) });
          sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
      } else {
          char const type[] = "ambient_light_strm";
          sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
          value.str.funcs.encode = pb_encode_string_cb;
          value.str.arg = &((pb_buffer_arg)
              { .buf = type, .buf_len = sizeof(type) });
          sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
      }
    } else {
        if(this->cb->get_registration_index(this) == 1){
            char const type[] = "ambient_light_1";
            sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
            value.str.funcs.encode = pb_encode_string_cb;
            value.str.arg = &((pb_buffer_arg)
                { .buf = type, .buf_len = sizeof(type) });
            sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
        } else {
            char const type[] = "ambient_light";
            sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
            value.str.funcs.encode = pb_encode_string_cb;
            value.str.arg = &((pb_buffer_arg)
                { .buf = type, .buf_len = sizeof(type) });
            sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
        }
    }
#else
    char const type[] = "ambient_light";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = type, .buf_len = sizeof(type) });
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
#endif
  }
  {
    char const vendor[] = "amsAG";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = vendor, .buf_len = sizeof(vendor) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VENDOR, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = false;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = false;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_DYNAMIC, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = (stream_type == SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE) ? true : false;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_DRI, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = false;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_STREAM_SYNC, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = AMS_DRIVER_VERSION;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = SNS_STD_SENSOR_RIGID_BODY_TYPE_DISPLAY;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = hwid;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_HW_ID, &value, 1, false);
  }
  {
    float data[ALS_PROTO_STD_ARRAY_LEN] = {0};
    state->encoded_event_len =
        pb_get_encoded_size_sensor_stream_event(data, ARR_SIZE(data));
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->encoded_event_len;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_EVENT_SIZE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = true;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR, &value, 1, true);
  }
}

void tmd2725_publish_als_resolution_attribute(sns_sensor *const this, float res)
{
    AMS_DEV_PRINTF(HIGH, this, "AMS: tmd2725_publish_als_resolution_attribute, %d ", (int32_t)res);

    sns_sensor_uid * p_suid_1;
    sns_sensor_uid * p_suid_2;
    int32_t hwid = this->cb->get_registration_index(this);
    ams_state *state = (ams_state*)this->state->state;


    p_suid_1 = (sns_sensor_uid *)tmd2725_get_suid_with_sensortype_and_hwid(AMS_ALS_OC, hwid);

    p_suid_2 = (sns_sensor_uid *)tmd2725_get_suid_with_sensortype_and_hwid(AMS_ALS_STRM, hwid);

    if((0 == sns_memcmp(&state->my_suid, p_suid_1, sizeof(state->my_suid))) ||
         (0 == sns_memcmp(&state->my_suid, p_suid_2, sizeof(state->my_suid))))
    {

		sns_std_attr_value_data values[] = {SNS_ATTR};
		values[0].has_flt = true;
		values[0].flt = res;
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION,
			values, ARR_SIZE(values), false);
    }
}

/* See sns_sensor::init */
sns_rc _common_tmd2725_als_init(sns_sensor *const this, sns_std_sensor_stream_type stream_type)
{
  ams_state *state = (ams_state*)this->state->state;
  struct sns_service_manager *smgr= this->cb->get_service_manager(this);
  state->diag_service = (sns_diag_service *)smgr->get_service(smgr, SNS_DIAG_SERVICE);
  state->scp_service =
     (sns_sync_com_port_service *)smgr->get_service(smgr, SNS_SYNC_COM_PORT_SERVICE);
  sns_diag_service* diag = state->diag_service;
  sns_sensor_uid * p_suid;
  int32_t hwid = this->cb->get_registration_index(this);

  state->calibration_data_tmd2725 = default_common_cal_data_tmd2725;
  state->als_default_config_data = default_als_config_data;
  state->prox_default_config_data = default_prox_config_data;
  /* our private wrapped-up copies of the com port's handles etc. */
  state->port_handle.scp_service = state->scp_service;
  state->port_handle_ptr = &state->port_handle;
  // for debug use - allows called fn to call sensor_printf
  state->port_handle.diag = diag;

  state->sensor_client_present = false;

  if (stream_type == SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE)
  {
    p_suid = (sns_sensor_uid *)tmd2725_get_suid_with_sensortype_and_hwid(AMS_ALS_OC, hwid);
    state->sensor_type = AMS_ALS_OC;
    sns_memscpy(&state->my_suid,
                sizeof(state->my_suid),
                p_suid,
                sizeof(sns_sensor_uid));
  }
  else
  {
    p_suid = (sns_sensor_uid *)tmd2725_get_suid_with_sensortype_and_hwid(AMS_ALS_STRM, hwid);
    state->sensor_type = AMS_ALS_STRM;
    sns_memscpy(&state->my_suid,
                sizeof(state->my_suid),
                p_suid,
                sizeof(sns_sensor_uid));
  }

  tmd2725_publish_als_attributes(this, stream_type);

  AMS_DEV_PRINTF(LOW, this
             , " AMS: _common_tmd2725_als_init: this %p, stream_type %d: state %p, scp_service %p"
             , this
             , stream_type
             , state
             , state->scp_service
             );

  tmd2725_send_suid_req(this, "interrupt", sizeof("interrupt"));
  tmd2725_send_suid_req(this, "timer", sizeof("timer"));

#ifdef TMD2725_ENABLE_REGISTRY_ACCESS
  tmd2725_send_suid_req(this, "registry", sizeof("registry"));
#else
  state->is_dri                = TMD2725_IS_DRI;
  state->hardware_id           = TMD2725_HW_ID;
  state->resolution_idx        = TMD2725_RES_IDX_ALS;
  state->supports_sync_stream  = TMD2725_SYNC_STREAM;

  state->com_port_info.com_config.bus_instance      = TMD2725_BUS_INSTANCE;
  state->com_port_info.com_config.bus_type          = TMD2725_BUS_TYPE;
  state->com_port_info.com_config.max_bus_speed_KHz = TMD2725_BUS_MAX_FREQ;
  state->com_port_info.com_config.min_bus_speed_KHz = TMD2725_BUS_MIN_FREQ;
  state->com_port_info.com_config.slave_control     = TMD2725_SLAVE_CONTROL;
  state->com_port_info.com_config.reg_addr_type     = TMD2725_REG_ADDRESS_TYPE;

  state->irq_config.interrupt_num            = TMD2725_INT_NUM;
  state->irq_config.interrupt_pull_type      = TMD2725_INT_PULL_TYPE;
  state->irq_config.is_chip_pin              = TMD2725_INT_IS_CHIP_PIN;
  state->irq_config.interrupt_drive_strength = TMD2725_INT_DRIVE_STRENGTH;
  state->irq_config.interrupt_trigger_type   = TMD2725_INT_TRIGGER_TYPE;

  state->rail_config.num_of_rails = TMD2725_NUM_OF_RAILS;
  state->rail_config.rail_vote    = TMD2725_RAIL_ON_STATE;
  sns_strlcpy(state->rail_config.rails[0].name, TMD2725_RAIL_1, sizeof(state->rail_config.rails[0].name));
if(2 == state->rail_config.num_of_rails)
{
  sns_strlcpy(state->rail_config.rails[1].name, TMD2725_RAIL_2, sizeof(state->rail_config.rails[1]. name));
}
  state->registry_cfg_received        = true;
  state->registry_pf_cfg_received     = true;
  state->registry_placement_received  = true;
#endif

  return SNS_RC_SUCCESS;
}

sns_rc tmd2725_als_init(sns_sensor *const this)
{
#ifdef ENABLE_DEBUG
  int32_t hwid = this->cb->get_registration_index(this);
  AMS_PRINTF(HIGH, this, "AMS: AMBIENT LIGHT on-change init, hwid: %d", hwid);
#endif
  return _common_tmd2725_als_init(this, SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE);
}

sns_rc tmd2725_als_streaming_init(sns_sensor *const this)
{
#ifdef ENABLE_DEBUG
  int32_t hwid = this->cb->get_registration_index(this);
  AMS_PRINTF(HIGH, this, "AMS: AMBIENT LIGHT streaming init, hwid: %d", hwid);
#endif
  return _common_tmd2725_als_init(this, SNS_STD_SENSOR_STREAM_TYPE_STREAMING);
}

sns_rc tmd2725_als_deinit(sns_sensor *const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

#endif //CONFIG_AMS_OPTICAL_SENSOR_ALS
