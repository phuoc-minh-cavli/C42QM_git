/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P O W E R _ S A V E _ M O D E _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the psm service Data structures.

  Copyright (c) 2013-2023 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.


  $Header: //components/rel/qmimsgs.tx/1.0/psm/src/power_save_mode_service_v01.c#12 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Fri Jun 23 2023 (Spin 0)
   From IDL File: power_save_mode_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "power_save_mode_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t psm_sensor_policy_cond_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_cond_v01, greater_value),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_cond_v01, lower_value),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_cond_v01, delta_value),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t psm_sensor_policy_cond_ext_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_cond_ext_v01, alert_threshold_high),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_cond_ext_v01, alert_threshold_low),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_cond_ext_v01, pre_alert_threshold_high),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_cond_ext_v01, pre_alert_threshold_low),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t psm_sensor_policy_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_config_v01, sensor_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_sensor_policy_config_v01, sensor_policy),
  QMI_PSM_SENSOR_CONFIG_MAX_V01,
  QMI_IDL_OFFSET8(psm_sensor_policy_config_v01, sensor_policy) - QMI_IDL_OFFSET8(psm_sensor_policy_config_v01, sensor_policy_len),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_config_v01, sensor_batch_cnt),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_config_v01, reporting_interval),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t psm_sensor_policy_config_ext_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_config_ext_v01, sensor_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_sensor_policy_config_ext_v01, sensor_policy),
  QMI_PSM_SENSOR_CONFIG_MAX_V01,
  QMI_IDL_OFFSET8(psm_sensor_policy_config_ext_v01, sensor_policy) - QMI_IDL_OFFSET8(psm_sensor_policy_config_ext_v01, sensor_policy_len),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_config_ext_v01, operating_mode),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_config_ext_v01, performance_mode),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_config_ext_v01, hysteresis_duration),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_config_ext_v01, high_perf_measurement_period),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_config_ext_v01, measurement_period),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sensor_error_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sensor_error_type_v01, sns_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sensor_error_type_v01, error_config),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sensor_error_type_v01, error_type),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t psm_sensor_sample_data_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_sample_data_v01, sensor_dimension_data),
  QMI_PSM_SENSOR_CONFIG_MAX_V01,
  QMI_IDL_OFFSET8(psm_sensor_sample_data_v01, sensor_dimension_data) - QMI_IDL_OFFSET8(psm_sensor_sample_data_v01, sensor_dimension_data_len),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_sample_data_v01, timestamp),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t psm_sensor_info_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_info_v01, sensor_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_sensor_info_v01, sample_list),
  QMI_PSM_MAX_BATCH_SUPPORTED_V01,
  QMI_IDL_OFFSET8(psm_sensor_info_v01, sample_list) - QMI_IDL_OFFSET8(psm_sensor_info_v01, sample_list_len),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t psm_sensor_algo_info_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_algo_info_v01, event_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_algo_info_v01, sensor_state),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_algo_info_v01, ttc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_algo_info_v01, slope),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_algo_info_v01, high_perf_measurement_period),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t psm_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_psm_status) - QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_psm_status_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_psm_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_psm_cfg_change) - QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_psm_cfg_change_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_psm_cfg_change),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_sensor_status) - QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_sensor_status_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_sensor_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_sns_phy_test_ind) - QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_sns_phy_test_ind_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_sns_phy_test_ind),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_sns_phy_test_get_data_ind) - QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_sns_phy_test_get_data_ind_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_indication_register_req_msg_v01, report_sns_phy_test_get_data_ind)
};

static const uint8_t psm_indication_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * psm_get_cfg_params_req_msg is empty
 * static const uint8_t psm_get_cfg_params_req_msg_data_v01[] = {
 * };
 */

static const uint8_t psm_get_cfg_params_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, psm_enabled) - QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, psm_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, psm_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, duration_threshold) - QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, duration_threshold_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, duration_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, duration_due_to_oos) - QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, duration_due_to_oos_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, duration_due_to_oos),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, randomization_window) - QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, randomization_window_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, randomization_window),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, active_timer) - QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, active_timer_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, active_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, periodic_update_timer) - QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, periodic_update_timer_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, periodic_update_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, early_wakeup_time) - QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, early_wakeup_time_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_get_cfg_params_resp_msg_v01, early_wakeup_time)
};

static const uint8_t psm_enter_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_enter_req_msg_v01, duration_in_psm) - QMI_IDL_OFFSET8(psm_enter_req_msg_v01, duration_in_psm_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_enter_req_msg_v01, duration_in_psm),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_enter_req_msg_v01, status) - QMI_IDL_OFFSET8(psm_enter_req_msg_v01, status_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_enter_req_msg_v01, status)
};

static const uint8_t psm_enter_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_enter_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t psm_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_status_ind_msg_v01, psm_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_status_ind_msg_v01, reject_reason) - QMI_IDL_OFFSET8(psm_status_ind_msg_v01, reject_reason_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_status_ind_msg_v01, reject_reason),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_status_ind_msg_v01, periodic_update_timer) - QMI_IDL_OFFSET8(psm_status_ind_msg_v01, periodic_update_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_status_ind_msg_v01, periodic_update_timer)
};

static const uint8_t psm_get_capability_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_capability_req_msg_v01, te_psm_capability) - QMI_IDL_OFFSET8(psm_get_capability_req_msg_v01, te_psm_capability_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_get_capability_req_msg_v01, te_psm_capability)
};

static const uint8_t psm_get_capability_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_get_capability_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_capability_resp_msg_v01, psm_capability) - QMI_IDL_OFFSET8(psm_get_capability_resp_msg_v01, psm_capability_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_get_capability_resp_msg_v01, psm_capability)
};

static const uint8_t psm_set_cfg_params_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_set_cfg_params_req_msg_v01, psm_enabled) - QMI_IDL_OFFSET8(psm_set_cfg_params_req_msg_v01, psm_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_set_cfg_params_req_msg_v01, psm_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_set_cfg_params_req_msg_v01, active_timer) - QMI_IDL_OFFSET8(psm_set_cfg_params_req_msg_v01, active_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_set_cfg_params_req_msg_v01, active_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_set_cfg_params_req_msg_v01, periodic_update_timer) - QMI_IDL_OFFSET8(psm_set_cfg_params_req_msg_v01, periodic_update_timer_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_set_cfg_params_req_msg_v01, periodic_update_timer)
};

static const uint8_t psm_set_cfg_params_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_set_cfg_params_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t psm_cfg_params_change_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_cfg_params_change_ind_msg_v01, psm_enabled) - QMI_IDL_OFFSET8(psm_cfg_params_change_ind_msg_v01, psm_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_cfg_params_change_ind_msg_v01, psm_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_cfg_params_change_ind_msg_v01, active_timer) - QMI_IDL_OFFSET8(psm_cfg_params_change_ind_msg_v01, active_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_cfg_params_change_ind_msg_v01, active_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_cfg_params_change_ind_msg_v01, periodic_update_timer) - QMI_IDL_OFFSET8(psm_cfg_params_change_ind_msg_v01, periodic_update_timer_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_cfg_params_change_ind_msg_v01, periodic_update_timer)
};

static const uint8_t psm_set_sensor_configuration_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_set_sensor_configuration_req_msg_v01, sensor_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_set_sensor_configuration_req_msg_v01, sensor_config_list) - QMI_IDL_OFFSET8(psm_set_sensor_configuration_req_msg_v01, sensor_config_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_set_sensor_configuration_req_msg_v01, sensor_config_list),
  QMI_PSM_SENSOR_CONFIG_MAX_V01,
  QMI_IDL_OFFSET8(psm_set_sensor_configuration_req_msg_v01, sensor_config_list) - QMI_IDL_OFFSET8(psm_set_sensor_configuration_req_msg_v01, sensor_config_list_len),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t psm_set_sensor_configuration_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_set_sensor_configuration_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t psm_set_sensor_configuration_ext_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_set_sensor_configuration_ext_req_msg_v01, sensor_config_list) - QMI_IDL_OFFSET8(psm_set_sensor_configuration_ext_req_msg_v01, sensor_config_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_set_sensor_configuration_ext_req_msg_v01, sensor_config_list),
  QMI_PSM_SENSOR_CONFIG_MAX_V01,
  QMI_IDL_OFFSET8(psm_set_sensor_configuration_ext_req_msg_v01, sensor_config_list) - QMI_IDL_OFFSET8(psm_set_sensor_configuration_ext_req_msg_v01, sensor_config_list_len),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, als_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, als_threshold_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, als_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, flight_mode_pressure_enable) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, flight_mode_pressure_enable_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, flight_mode_pressure_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, flight_mode_pressure_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, flight_mode_pressure_threshold_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, flight_mode_pressure_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, flight_mode_accelerometer_sample_rate) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, flight_mode_accelerometer_sample_rate_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, flight_mode_accelerometer_sample_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, flight_mode_accelerometer_sample_length) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, flight_mode_accelerometer_sample_length_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, flight_mode_accelerometer_sample_length),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, transportation_type) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, transportation_type_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, transportation_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, redo_gravity_init) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, redo_gravity_init_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, redo_gravity_init),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, iteration_delay) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, iteration_delay_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, iteration_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, init_max_trial_num) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, init_max_trial_num_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, init_max_trial_num),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, update_max_trial_num) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, update_max_trial_num_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, update_max_trial_num),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, init_gravity_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, init_gravity_threshold_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, init_gravity_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, update_gravity_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, update_gravity_threshold_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, update_gravity_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_weights) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_weights_valid)),
  0x1D,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_weights),
  PERCEPTRON_WEIGHT_INDEX_V01,
  QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_weights) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_weights_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_min_features) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_min_features_valid)),
  0x1E,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_min_features),
  PERCEPTRON_MAX_MIN_FEATURES_INDEX_V01,
  QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_min_features) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_min_features_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_max_features) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_max_features_valid)),
  0x1F,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_max_features),
  PERCEPTRON_MAX_MIN_FEATURES_INDEX_V01,
  QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_max_features) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_perceptron_max_features_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_sum_score_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_sum_score_threshold_valid)),
  0x20,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, smd_sum_score_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, md_tuning_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, md_tuning_threshold_valid)),
  0x21,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, md_tuning_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, mcd_motion_toggle_delay_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, mcd_motion_toggle_delay_threshold_valid)),
  0x22,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, mcd_motion_toggle_delay_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, mcd_motion_toggle_counter_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, mcd_motion_toggle_counter_threshold_valid)),
  0x23,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, mcd_motion_toggle_counter_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, mcd_psm_timer_config) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, mcd_psm_timer_config_valid)),
  0x24,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, mcd_psm_timer_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, fmd_accel_thres) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, fmd_accel_thres_valid)),
  0x25,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, fmd_accel_thres),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, fmd_accel_buffer_count) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, fmd_accel_buffer_count_valid)),
  0x26,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, fmd_accel_buffer_count),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, fmd_run_period) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, fmd_run_period_valid)),
  0x27,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, fmd_run_period),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_sld_win_sz) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_sld_win_sz_valid)),
  0x28,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, smd_sld_win_sz),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_sld_win_s_sz) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, smd_sld_win_s_sz_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, smd_sld_win_s_sz),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, shock_detection_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_set_sensor_configuration_ext_req_msg_v01, shock_detection_threshold_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_set_sensor_configuration_ext_req_msg_v01, shock_detection_threshold)
};

static const uint8_t psm_set_sensor_configuration_ext_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_set_sensor_configuration_ext_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_set_sensor_configuration_ext_resp_msg_v01, sensor_error_resp) - QMI_IDL_OFFSET8(psm_set_sensor_configuration_ext_resp_msg_v01, sensor_error_resp_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_set_sensor_configuration_ext_resp_msg_v01, sensor_error_resp),
  QMI_PSM_SENSOR_CONFIG_MAX_V01,
  QMI_IDL_OFFSET8(psm_set_sensor_configuration_ext_resp_msg_v01, sensor_error_resp) - QMI_IDL_OFFSET8(psm_set_sensor_configuration_ext_resp_msg_v01, sensor_error_resp_len),
  QMI_IDL_TYPE88(0, 4)
};

static const uint8_t psm_get_sensor_configuration_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_get_sensor_configuration_req_msg_v01, sensor_id)
};

static const uint8_t psm_get_sensor_configuration_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_get_sensor_configuration_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_sensor_configuration_resp_msg_v01, sensor_config_list) - QMI_IDL_OFFSET8(psm_get_sensor_configuration_resp_msg_v01, sensor_config_list_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_get_sensor_configuration_resp_msg_v01, sensor_config_list),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_sensor_configuration_resp_msg_v01, als_threshold) - QMI_IDL_OFFSET8(psm_get_sensor_configuration_resp_msg_v01, als_threshold_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_get_sensor_configuration_resp_msg_v01, als_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, flight_mode_pressure_enable) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, flight_mode_pressure_enable_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, flight_mode_pressure_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, flight_mode_pressure_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, flight_mode_pressure_threshold_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, flight_mode_pressure_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, flight_mode_accelerometer_sample_rate) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, flight_mode_accelerometer_sample_rate_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, flight_mode_accelerometer_sample_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, flight_mode_accelerometer_sample_length) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, flight_mode_accelerometer_sample_length_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, flight_mode_accelerometer_sample_length),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, transportation_type) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, transportation_type_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, transportation_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, redo_gravity_init) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, redo_gravity_init_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, redo_gravity_init),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, iteration_delay) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, iteration_delay_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, iteration_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, init_max_trial_num) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, init_max_trial_num_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, init_max_trial_num),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, update_max_trial_num) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, update_max_trial_num_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, update_max_trial_num),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, init_gravity_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, init_gravity_threshold_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, init_gravity_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, update_gravity_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, update_gravity_threshold_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, update_gravity_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_weights) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_weights_valid)),
  0x1D,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_weights),
  PERCEPTRON_WEIGHT_INDEX_V01,
  QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_weights) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_weights_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_min_features) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_min_features_valid)),
  0x1E,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_min_features),
  PERCEPTRON_MAX_MIN_FEATURES_INDEX_V01,
  QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_min_features) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_min_features_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_max_features) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_max_features_valid)),
  0x1F,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_max_features),
  PERCEPTRON_MAX_MIN_FEATURES_INDEX_V01,
  QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_max_features) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_perceptron_max_features_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_sum_score_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_sum_score_threshold_valid)),
  0x20,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, smd_sum_score_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, md_tuning_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, md_tuning_threshold_valid)),
  0x21,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, md_tuning_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, mcd_motion_toggle_delay_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, mcd_motion_toggle_delay_threshold_valid)),
  0x22,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, mcd_motion_toggle_delay_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, mcd_motion_toggle_counter_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, mcd_motion_toggle_counter_threshold_valid)),
  0x23,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, mcd_motion_toggle_counter_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, mcd_psm_timer_config) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, mcd_psm_timer_config_valid)),
  0x24,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, mcd_psm_timer_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, fmd_accel_thres) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, fmd_accel_thres_valid)),
  0x25,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, fmd_accel_thres),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, fmd_accel_buffer_count) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, fmd_accel_buffer_count_valid)),
  0x26,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, fmd_accel_buffer_count),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, fmd_run_period) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, fmd_run_period_valid)),
  0x27,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, fmd_run_period),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_sld_win_sz) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_sld_win_sz_valid)),
  0x28,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, smd_sld_win_sz),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_sld_win_s_sz) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, smd_sld_win_s_sz_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, smd_sld_win_s_sz),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, shock_detection_threshold) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_configuration_resp_msg_v01, shock_detection_threshold_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_configuration_resp_msg_v01, shock_detection_threshold)
};

static const uint8_t psm_get_sensor_data_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(psm_get_sensor_data_req_msg_v01, sensor_id_mask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_sensor_data_req_msg_v01, is_logging_mode) - QMI_IDL_OFFSET8(psm_get_sensor_data_req_msg_v01, is_logging_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_get_sensor_data_req_msg_v01, is_logging_mode)
};

static const uint8_t psm_get_sensor_data_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_get_sensor_data_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_sensor_data_resp_msg_v01, sensor_info) - QMI_IDL_OFFSET8(psm_get_sensor_data_resp_msg_v01, sensor_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_get_sensor_data_resp_msg_v01, sensor_info),
  QMI_PSM_SENSOR_TYPE_MAX_V01,
  QMI_IDL_OFFSET8(psm_get_sensor_data_resp_msg_v01, sensor_info) - QMI_IDL_OFFSET8(psm_get_sensor_data_resp_msg_v01, sensor_info_len),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, algo_info) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, algo_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_data_resp_msg_v01, algo_info),
  QMI_PSM_SENSOR_TYPE_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, algo_info) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, algo_info_len),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, sensor_flight_status) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, sensor_flight_status_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_data_resp_msg_v01, sensor_flight_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, sensor_tilt_angle) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, sensor_tilt_angle_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_data_resp_msg_v01, sensor_tilt_angle),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, mcd_curr_state) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, mcd_curr_state_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_data_resp_msg_v01, mcd_curr_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, mcd_prev_state) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, mcd_prev_state_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_data_resp_msg_v01, mcd_prev_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, high_shock_duration) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, high_shock_duration_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_data_resp_msg_v01, high_shock_duration),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, free_fall_height) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, free_fall_height_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_data_resp_msg_v01, free_fall_height),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, fmd_data_timestamp) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, fmd_data_timestamp_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_data_resp_msg_v01, fmd_data_timestamp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, tilt_data_timestamp) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, tilt_data_timestamp_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_data_resp_msg_v01, tilt_data_timestamp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, mcd_data_timestamp) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, mcd_data_timestamp_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_data_resp_msg_v01, mcd_data_timestamp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, free_fall_data_timestamp) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, free_fall_data_timestamp_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_data_resp_msg_v01, free_fall_data_timestamp),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, high_shock_data_timestamp) - QMI_IDL_OFFSET16RELATIVE(psm_get_sensor_data_resp_msg_v01, high_shock_data_timestamp_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(psm_get_sensor_data_resp_msg_v01, high_shock_data_timestamp)
};

/*
 * psm_reset_sensor_config_data_req_msg is empty
 * static const uint8_t psm_reset_sensor_config_data_req_msg_data_v01[] = {
 * };
 */

static const uint8_t psm_reset_sensor_config_data_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_reset_sensor_config_data_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * psm_get_last_policy_met_info_req_msg is empty
 * static const uint8_t psm_get_last_policy_met_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t psm_get_last_policy_met_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_get_last_policy_met_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_last_policy_met_info_resp_msg_v01, policy_met_sensor_mask) - QMI_IDL_OFFSET8(psm_get_last_policy_met_info_resp_msg_v01, policy_met_sensor_mask_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(psm_get_last_policy_met_info_resp_msg_v01, policy_met_sensor_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_last_policy_met_info_resp_msg_v01, batch_full_sensor_mask) - QMI_IDL_OFFSET8(psm_get_last_policy_met_info_resp_msg_v01, batch_full_sensor_mask_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(psm_get_last_policy_met_info_resp_msg_v01, batch_full_sensor_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_last_policy_met_info_resp_msg_v01, sensor_fail_cond_met_mask) - QMI_IDL_OFFSET8(psm_get_last_policy_met_info_resp_msg_v01, sensor_fail_cond_met_mask_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(psm_get_last_policy_met_info_resp_msg_v01, sensor_fail_cond_met_mask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_last_policy_met_info_resp_msg_v01, data_ready) - QMI_IDL_OFFSET8(psm_get_last_policy_met_info_resp_msg_v01, data_ready_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_get_last_policy_met_info_resp_msg_v01, data_ready)
};

static const uint8_t psm_sensor_policy_met_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, policy_met_sensor_mask) - QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, policy_met_sensor_mask_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, policy_met_sensor_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, batch_full_sensor_mask) - QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, batch_full_sensor_mask_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, batch_full_sensor_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, sensor_fail_cond_met_mask) - QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, sensor_fail_cond_met_mask_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, sensor_fail_cond_met_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, is_tracker_db_full) - QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, is_tracker_db_full_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, is_tracker_db_full),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, timestamp) - QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, timestamp_valid)),
  0x14,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, timestamp),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, data_ready) - QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, data_ready_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_policy_met_ind_msg_v01, data_ready)
};

/*
 * psm_get_fmd_sns_sample_req_msg is empty
 * static const uint8_t psm_get_fmd_sns_sample_req_msg_data_v01[] = {
 * };
 */

static const uint8_t psm_get_fmd_sns_sample_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_get_fmd_sns_sample_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t psm_phy_sns_test_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_phy_sns_test_req_msg_v01, sensor_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_phy_sns_test_req_msg_v01, physical_sns_test) - QMI_IDL_OFFSET8(psm_phy_sns_test_req_msg_v01, physical_sns_test_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_phy_sns_test_req_msg_v01, physical_sns_test)
};

static const uint8_t psm_phy_sns_test_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_phy_sns_test_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t psm_phy_sns_test_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_phy_sns_test_ind_msg_v01, test_passed) - QMI_IDL_OFFSET8(psm_phy_sns_test_ind_msg_v01, test_passed_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_phy_sns_test_ind_msg_v01, test_passed),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_phy_sns_test_ind_msg_v01, physical_sns_test) - QMI_IDL_OFFSET8(psm_phy_sns_test_ind_msg_v01, physical_sns_test_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_phy_sns_test_ind_msg_v01, physical_sns_test)
};

static const uint8_t psm_phy_sns_test_get_data_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_req_msg_v01, sensor_id),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_req_msg_v01, is_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_req_msg_v01, sample_rate) - QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_req_msg_v01, sample_rate_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_req_msg_v01, sample_rate),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_req_msg_v01, report_rate) - QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_req_msg_v01, report_rate_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_req_msg_v01, report_rate)
};

static const uint8_t psm_phy_sns_test_get_data_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t psm_phy_sns_test_get_data_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_ind_msg_v01, sensor_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_ind_msg_v01, sensor_data_rsp) - QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_ind_msg_v01, sensor_data_rsp_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_ind_msg_v01, sensor_data_rsp),
  QMI_PSM_SENSOR_CONFIG_MAX_V01,
  QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_ind_msg_v01, sensor_data_rsp) - QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_ind_msg_v01, sensor_data_rsp_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_ind_msg_v01, timestamp) - QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_ind_msg_v01, timestamp_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(psm_phy_sns_test_get_data_ind_msg_v01, timestamp)
};

static const uint8_t psm_sensor_logging_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_sensor_logging_req_msg_v01, log_enable_level)
};

static const uint8_t psm_sensor_logging_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_sensor_logging_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t psm_get_sns_serial_number_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(psm_get_sns_serial_number_req_msg_v01, sensor_id)
};

static const uint8_t psm_get_sns_serial_number_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_get_sns_serial_number_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(psm_get_sns_serial_number_resp_msg_v01, serial_number) - QMI_IDL_OFFSET8(psm_get_sns_serial_number_resp_msg_v01, serial_number_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(psm_get_sns_serial_number_resp_msg_v01, serial_number),
  QMI_PSM_SENSOR_SERIAL_NUMBER_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(psm_get_sns_serial_number_resp_msg_v01, serial_number) - QMI_IDL_OFFSET8(psm_get_sns_serial_number_resp_msg_v01, serial_number_len)
};

/*
 * psm_sensors_reset_tracker_db_req_msg is empty
 * static const uint8_t psm_sensors_reset_tracker_db_req_msg_data_v01[] = {
 * };
 */

static const uint8_t psm_sensors_reset_tracker_db_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(psm_sensors_reset_tracker_db_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
static const qmi_idl_type_table_entry  psm_type_table_v01[] = {
  {sizeof(psm_sensor_policy_cond_v01), psm_sensor_policy_cond_data_v01},
  {sizeof(psm_sensor_policy_cond_ext_v01), psm_sensor_policy_cond_ext_data_v01},
  {sizeof(psm_sensor_policy_config_v01), psm_sensor_policy_config_data_v01},
  {sizeof(psm_sensor_policy_config_ext_v01), psm_sensor_policy_config_ext_data_v01},
  {sizeof(sensor_error_type_v01), sensor_error_type_data_v01},
  {sizeof(psm_sensor_sample_data_v01), psm_sensor_sample_data_data_v01},
  {sizeof(psm_sensor_info_v01), psm_sensor_info_data_v01},
  {sizeof(psm_sensor_algo_info_v01), psm_sensor_algo_info_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry psm_message_table_v01[] = {
  {sizeof(psm_indication_register_req_msg_v01), psm_indication_register_req_msg_data_v01},
  {sizeof(psm_indication_register_resp_msg_v01), psm_indication_register_resp_msg_data_v01},
  {sizeof(psm_get_cfg_params_req_msg_v01), 0},
  {sizeof(psm_get_cfg_params_resp_msg_v01), psm_get_cfg_params_resp_msg_data_v01},
  {sizeof(psm_enter_req_msg_v01), psm_enter_req_msg_data_v01},
  {sizeof(psm_enter_resp_msg_v01), psm_enter_resp_msg_data_v01},
  {sizeof(psm_status_ind_msg_v01), psm_status_ind_msg_data_v01},
  {sizeof(psm_get_capability_req_msg_v01), psm_get_capability_req_msg_data_v01},
  {sizeof(psm_get_capability_resp_msg_v01), psm_get_capability_resp_msg_data_v01},
  {sizeof(psm_set_cfg_params_req_msg_v01), psm_set_cfg_params_req_msg_data_v01},
  {sizeof(psm_set_cfg_params_resp_msg_v01), psm_set_cfg_params_resp_msg_data_v01},
  {sizeof(psm_cfg_params_change_ind_msg_v01), psm_cfg_params_change_ind_msg_data_v01},
  {sizeof(psm_set_sensor_configuration_req_msg_v01), psm_set_sensor_configuration_req_msg_data_v01},
  {sizeof(psm_set_sensor_configuration_resp_msg_v01), psm_set_sensor_configuration_resp_msg_data_v01},
  {sizeof(psm_set_sensor_configuration_ext_req_msg_v01), psm_set_sensor_configuration_ext_req_msg_data_v01},
  {sizeof(psm_set_sensor_configuration_ext_resp_msg_v01), psm_set_sensor_configuration_ext_resp_msg_data_v01},
  {sizeof(psm_get_sensor_configuration_req_msg_v01), psm_get_sensor_configuration_req_msg_data_v01},
  {sizeof(psm_get_sensor_configuration_resp_msg_v01), psm_get_sensor_configuration_resp_msg_data_v01},
  {sizeof(psm_get_sensor_data_req_msg_v01), psm_get_sensor_data_req_msg_data_v01},
  {sizeof(psm_get_sensor_data_resp_msg_v01), psm_get_sensor_data_resp_msg_data_v01},
  {sizeof(psm_reset_sensor_config_data_req_msg_v01), 0},
  {sizeof(psm_reset_sensor_config_data_resp_msg_v01), psm_reset_sensor_config_data_resp_msg_data_v01},
  {sizeof(psm_get_last_policy_met_info_req_msg_v01), 0},
  {sizeof(psm_get_last_policy_met_info_resp_msg_v01), psm_get_last_policy_met_info_resp_msg_data_v01},
  {sizeof(psm_sensor_policy_met_ind_msg_v01), psm_sensor_policy_met_ind_msg_data_v01},
  {sizeof(psm_get_fmd_sns_sample_req_msg_v01), 0},
  {sizeof(psm_get_fmd_sns_sample_resp_msg_v01), psm_get_fmd_sns_sample_resp_msg_data_v01},
  {sizeof(psm_phy_sns_test_req_msg_v01), psm_phy_sns_test_req_msg_data_v01},
  {sizeof(psm_phy_sns_test_resp_msg_v01), psm_phy_sns_test_resp_msg_data_v01},
  {sizeof(psm_phy_sns_test_ind_msg_v01), psm_phy_sns_test_ind_msg_data_v01},
  {sizeof(psm_phy_sns_test_get_data_req_msg_v01), psm_phy_sns_test_get_data_req_msg_data_v01},
  {sizeof(psm_phy_sns_test_get_data_resp_msg_v01), psm_phy_sns_test_get_data_resp_msg_data_v01},
  {sizeof(psm_phy_sns_test_get_data_ind_msg_v01), psm_phy_sns_test_get_data_ind_msg_data_v01},
  {sizeof(psm_sensor_logging_req_msg_v01), psm_sensor_logging_req_msg_data_v01},
  {sizeof(psm_sensor_logging_resp_msg_v01), psm_sensor_logging_resp_msg_data_v01},
  {sizeof(psm_get_sns_serial_number_req_msg_v01), psm_get_sns_serial_number_req_msg_data_v01},
  {sizeof(psm_get_sns_serial_number_resp_msg_v01), psm_get_sns_serial_number_resp_msg_data_v01},
  {sizeof(psm_sensors_reset_tracker_db_req_msg_v01), 0},
  {sizeof(psm_sensors_reset_tracker_db_resp_msg_v01), psm_sensors_reset_tracker_db_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object psm_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *psm_qmi_idl_type_table_object_referenced_tables_v01[] =
{&psm_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object psm_qmi_idl_type_table_object_v01 = {
  sizeof(psm_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(psm_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  psm_type_table_v01,
  psm_message_table_v01,
  psm_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry psm_service_command_messages_v01[] = {
  {QMI_PSM_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 0), 20},
  {QMI_PSM_GET_CFG_PARAMS_REQ_V01, QMI_IDL_TYPE16(0, 2), 0},
  {QMI_PSM_ENTER_REQ_V01, QMI_IDL_TYPE16(0, 4), 11},
  {QMI_PSM_SET_CFG_PARAMS_REQ_V01, QMI_IDL_TYPE16(0, 9), 18},
  {QMI_PSM_GET_CAPABILITY_REQ_V01, QMI_IDL_TYPE16(0, 7), 7},
  {QMI_PSM_SET_SENSOR_CONFIGURATION_REQ_V01, QMI_IDL_TYPE16(0, 12), 1311},
  {QMI_PSM_GET_SENSOR_DATA_REQ_V01, QMI_IDL_TYPE16(0, 18), 9},
  {QMI_PSM_RESET_SENSOR_CONFIG_DATA_REQ_V01, QMI_IDL_TYPE16(0, 20), 0},
  {QMI_PSM_GET_LAST_SENSOR_POLICY_MET_INFO_REQ_V01, QMI_IDL_TYPE16(0, 22), 0},
  {QMI_PSM_SET_SENSOR_CONFIGURATION_EXT_REQ_V01, QMI_IDL_TYPE16(0, 14), 2018},
  {QMI_PSM_GET_SENSOR_CONFIGURATION_REQ_V01, QMI_IDL_TYPE16(0, 16), 7},
  {QMI_PSM_GET_FMD_SNS_SAMPLE_REQ_V01, QMI_IDL_TYPE16(0, 25), 0},
  {QMI_PSM_PHY_SNS_TEST_REQ_V01, QMI_IDL_TYPE16(0, 27), 14},
  {QMI_PSM_PHY_SNS_TEST_GET_DATA_REQ_V01, QMI_IDL_TYPE16(0, 30), 25},
  {QMI_PSM_SENSOR_LOGGING_REQ_V01, QMI_IDL_TYPE16(0, 33), 7},
  {QMI_PSM_GET_SNS_SERIAL_NUMBER_REQ_V01, QMI_IDL_TYPE16(0, 35), 7},
  {QMI_PSM_SENSORS_RESET_TRACKER_DB_REQ_V01, QMI_IDL_TYPE16(0, 37), 0}
};

static const qmi_idl_service_message_table_entry psm_service_response_messages_v01[] = {
  {QMI_PSM_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_PSM_GET_CFG_PARAMS_RESP_V01, QMI_IDL_TYPE16(0, 3), 53},
  {QMI_PSM_ENTER_RESP_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_PSM_SET_CFG_PARAMS_RESP_V01, QMI_IDL_TYPE16(0, 10), 7},
  {QMI_PSM_GET_CAPABILITY_RESP_V01, QMI_IDL_TYPE16(0, 8), 14},
  {QMI_PSM_SET_SENSOR_CONFIGURATION_RESP_V01, QMI_IDL_TYPE16(0, 13), 7},
  {QMI_PSM_GET_SENSOR_DATA_RESP_V01, QMI_IDL_TYPE16(0, 19), 12612},
  {QMI_PSM_RESET_SENSOR_CONFIG_DATA_RESP_V01, QMI_IDL_TYPE16(0, 21), 7},
  {QMI_PSM_GET_LAST_SENSOR_POLICY_MET_INFO_RESP_V01, QMI_IDL_TYPE16(0, 23), 26},
  {QMI_PSM_SET_SENSOR_CONFIGURATION_EXT_RESP_V01, QMI_IDL_TYPE16(0, 15), 101},
  {QMI_PSM_GET_SENSOR_CONFIGURATION_RESP_V01, QMI_IDL_TYPE16(0, 17), 440},
  {QMI_PSM_GET_FMD_SNS_SAMPLE_RESP_V01, QMI_IDL_TYPE16(0, 26), 7},
  {QMI_PSM_PHY_SNS_TEST_RESP_V01, QMI_IDL_TYPE16(0, 28), 7},
  {QMI_PSM_PHY_SNS_TEST_GET_DATA_RESP_V01, QMI_IDL_TYPE16(0, 31), 7},
  {QMI_PSM_SENSOR_LOGGING_RESP_V01, QMI_IDL_TYPE16(0, 34), 7},
  {QMI_PSM_GET_SNS_SERIAL_NUMBER_RESP_V01, QMI_IDL_TYPE16(0, 36), 31},
  {QMI_PSM_SENSORS_RESET_TRACKER_DB_RESP_V01, QMI_IDL_TYPE16(0, 38), 7}
};

static const qmi_idl_service_message_table_entry psm_service_indication_messages_v01[] = {
  {QMI_PSM_STATUS_IND_V01, QMI_IDL_TYPE16(0, 6), 21},
  {QMI_PSM_CFG_PARAMS_CHANGE_IND_V01, QMI_IDL_TYPE16(0, 11), 18},
  {QMI_PSM_SENSOR_POLICY_MET_IND_V01, QMI_IDL_TYPE16(0, 24), 34},
  {QMI_PSM_PHY_SNS_TEST_IND_V01, QMI_IDL_TYPE16(0, 29), 11},
  {QMI_PSM_PHY_SNS_TEST_GET_DATA_IND_V01, QMI_IDL_TYPE16(0, 32), 62}
};

/*Service Object*/
struct qmi_idl_service_object psm_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x53,
  12612,
  { sizeof(psm_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(psm_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(psm_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { psm_service_command_messages_v01, psm_service_response_messages_v01, psm_service_indication_messages_v01},
  &psm_qmi_idl_type_table_object_v01,
  0x11,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type psm_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( PSM_V01_IDL_MAJOR_VERS != idl_maj_version || PSM_V01_IDL_MINOR_VERS != idl_min_version
       || PSM_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&psm_qmi_idl_service_object_v01;
}

