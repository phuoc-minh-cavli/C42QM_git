/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A _ S Y S T E M _ D E T E R M I N A T I O N _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the dsd service Data structures.

  Copyright (c) 2012-2015 Qualcomm Technologies, Inc. All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.mpss/4.3.4/dsd/src/data_system_determination_v01.c#2 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Wed Feb  6 2019 (Spin 0)
   From IDL File: data_system_determination_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "data_system_determination_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t dsd_system_status_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_system_status_info_type_v01, technology),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_system_status_info_type_v01, rat_value),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_system_status_info_type_v01, so_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_apn_avail_sys_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_apn_avail_sys_info_type_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_apn_avail_sys_info_type_v01, apn_avail_sys),
  QMI_DSD_MAX_AVAIL_SYS_V01,
  QMI_IDL_OFFSET8(dsd_apn_avail_sys_info_type_v01, apn_avail_sys) - QMI_IDL_OFFSET8(dsd_apn_avail_sys_info_type_v01, apn_avail_sys_len),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_apn_pref_sys_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_apn_pref_sys_info_type_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01,

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_apn_pref_sys_info_type_v01, curr_pref_sys),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_apn_pref_sys_info_type_v01, recommended_pref_sys),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_global_pref_sys_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_global_pref_sys_info_type_v01, curr_pref_sys),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_global_pref_sys_info_type_v01, recommended_pref_sys),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_sys_ui_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_sys_ui_info_type_v01, technology),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_sys_ui_info_type_v01, rat_value),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_sys_ui_info_type_v01, so_mask),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_sys_ui_info_type_v01, ui_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_sys_apn_ui_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_sys_apn_ui_info_type_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01,

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_sys_apn_ui_info_type_v01, apn_type_mask),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_sys_apn_ui_info_type_v01, apn_ui_info),
  QMI_IDL_TYPE88(0, 4),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_sys_apn_ui_info_type_v01, is_internet),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_bss_load_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_bss_load_type_v01, station_count),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_bss_load_type_v01, channel_utilization),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_bss_load_type_v01, available_admission_capacity),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_phy_rate_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_phy_rate_type_v01, downlink_phy_rate),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_phy_rate_type_v01, uplink_phy_rate),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_packet_error_rate_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_packet_error_rate_type_v01, downlink_packet_error_rate),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_packet_error_rate_type_v01, uplink_packet_error_rate),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_lte_cell_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_lte_cell_info_type_v01, serving_cell_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_lte_cell_info_type_v01, pci),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_lte_cell_info_type_v01, freq),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_lte_cell_info_type_v01, dl_bandwidth),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_lte_cell_info_type_v01, band_info),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/*
 * dsd_get_system_status_req_msg is empty
 * static const uint8_t dsd_get_system_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t dsd_get_system_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_system_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_system_status_resp_msg_v01, avail_sys) - QMI_IDL_OFFSET8(dsd_get_system_status_resp_msg_v01, avail_sys_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_system_status_resp_msg_v01, avail_sys),
  QMI_DSD_MAX_AVAIL_SYS_V01,
  QMI_IDL_OFFSET8(dsd_get_system_status_resp_msg_v01, avail_sys) - QMI_IDL_OFFSET8(dsd_get_system_status_resp_msg_v01, avail_sys_len),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_avail_sys_info) - QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_avail_sys_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_get_system_status_resp_msg_v01, apn_avail_sys_info),
  QMI_DSD_MAX_APNS_V01,
  QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_avail_sys_info) - QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_avail_sys_info_len),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, global_pref_sys) - QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, global_pref_sys_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_get_system_status_resp_msg_v01, global_pref_sys),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_pref_sys) - QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_pref_sys_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_get_system_status_resp_msg_v01, apn_pref_sys),
  QMI_DSD_MAX_APNS_V01,
  QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_pref_sys) - QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_pref_sys_len),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, null_bearer_reason) - QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, null_bearer_reason_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(dsd_get_system_status_resp_msg_v01, null_bearer_reason)
};

static const uint8_t dsd_system_status_change_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, limit_so_mask_change_ind) - QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, limit_so_mask_change_ind_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, limit_so_mask_change_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_data_system_status_changes) - QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_data_system_status_changes_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_data_system_status_changes),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_pref_tech_change_only) - QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_pref_tech_change_only_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_pref_tech_change_only),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_null_bearer_reason) - QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_null_bearer_reason_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_null_bearer_reason)
};

static const uint8_t dsd_system_status_change_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_system_status_change_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_system_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_system_status_ind_msg_v01, avail_sys) - QMI_IDL_OFFSET8(dsd_system_status_ind_msg_v01, avail_sys_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_system_status_ind_msg_v01, avail_sys),
  QMI_DSD_MAX_AVAIL_SYS_V01,
  QMI_IDL_OFFSET8(dsd_system_status_ind_msg_v01, avail_sys) - QMI_IDL_OFFSET8(dsd_system_status_ind_msg_v01, avail_sys_len),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_avail_sys_info) - QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_avail_sys_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_system_status_ind_msg_v01, apn_avail_sys_info),
  QMI_DSD_MAX_APNS_V01,
  QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_avail_sys_info) - QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_avail_sys_info_len),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, global_pref_sys) - QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, global_pref_sys_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_system_status_ind_msg_v01, global_pref_sys),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_pref_sys) - QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_pref_sys_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_system_status_ind_msg_v01, apn_pref_sys),
  QMI_DSD_MAX_APNS_V01,
  QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_pref_sys) - QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_pref_sys_len),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, null_bearer_reason) - QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, null_bearer_reason_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(dsd_system_status_ind_msg_v01, null_bearer_reason)
};

/*
 * dsd_get_ui_info_req_msg is empty
 * static const uint8_t dsd_get_ui_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t dsd_get_ui_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_ui_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_ui_info_resp_msg_v01, global_ui_info) - QMI_IDL_OFFSET8(dsd_get_ui_info_resp_msg_v01, global_ui_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_ui_info_resp_msg_v01, global_ui_info),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_ui_info_resp_msg_v01, apn_info) - QMI_IDL_OFFSET8(dsd_get_ui_info_resp_msg_v01, apn_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_ui_info_resp_msg_v01, apn_info),
  QMI_DSD_MAX_APNS_V01,
  QMI_IDL_OFFSET8(dsd_get_ui_info_resp_msg_v01, apn_info) - QMI_IDL_OFFSET8(dsd_get_ui_info_resp_msg_v01, apn_info_len),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_get_ui_info_resp_msg_v01, null_bearer_reason) - QMI_IDL_OFFSET16RELATIVE(dsd_get_ui_info_resp_msg_v01, null_bearer_reason_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(dsd_get_ui_info_resp_msg_v01, null_bearer_reason)
};

static const uint8_t dsd_register_ui_info_change_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_register_ui_info_change_req_msg_v01, report_ui_changes) - QMI_IDL_OFFSET8(dsd_register_ui_info_change_req_msg_v01, report_ui_changes_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_register_ui_info_change_req_msg_v01, report_ui_changes),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_register_ui_info_change_req_msg_v01, suppress_so_change) - QMI_IDL_OFFSET8(dsd_register_ui_info_change_req_msg_v01, suppress_so_change_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_register_ui_info_change_req_msg_v01, suppress_so_change),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_register_ui_info_change_req_msg_v01, suppress_null_bearer_reason) - QMI_IDL_OFFSET8(dsd_register_ui_info_change_req_msg_v01, suppress_null_bearer_reason_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_register_ui_info_change_req_msg_v01, suppress_null_bearer_reason)
};

static const uint8_t dsd_register_ui_info_change_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_register_ui_info_change_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_ui_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_ui_info_ind_msg_v01, global_ui_info) - QMI_IDL_OFFSET8(dsd_ui_info_ind_msg_v01, global_ui_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_ui_info_ind_msg_v01, global_ui_info),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_ui_info_ind_msg_v01, apn_info) - QMI_IDL_OFFSET8(dsd_ui_info_ind_msg_v01, apn_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_ui_info_ind_msg_v01, apn_info),
  QMI_DSD_MAX_APNS_V01,
  QMI_IDL_OFFSET8(dsd_ui_info_ind_msg_v01, apn_info) - QMI_IDL_OFFSET8(dsd_ui_info_ind_msg_v01, apn_info_len),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_ui_info_ind_msg_v01, null_bearer_reason) - QMI_IDL_OFFSET16RELATIVE(dsd_ui_info_ind_msg_v01, null_bearer_reason_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(dsd_ui_info_ind_msg_v01, null_bearer_reason)
};

/* Type Table */
static const qmi_idl_type_table_entry  dsd_type_table_v01[] = {
  {sizeof(dsd_system_status_info_type_v01), dsd_system_status_info_type_data_v01},
  {sizeof(dsd_apn_avail_sys_info_type_v01), dsd_apn_avail_sys_info_type_data_v01},
  {sizeof(dsd_apn_pref_sys_info_type_v01), dsd_apn_pref_sys_info_type_data_v01},
  {sizeof(dsd_global_pref_sys_info_type_v01), dsd_global_pref_sys_info_type_data_v01},
  {sizeof(dsd_sys_ui_info_type_v01), dsd_sys_ui_info_type_data_v01},
  {sizeof(dsd_sys_apn_ui_info_type_v01), dsd_sys_apn_ui_info_type_data_v01},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {sizeof(dsd_wifi_bss_load_type_v01), dsd_wifi_bss_load_type_data_v01},
  {sizeof(dsd_wifi_phy_rate_type_v01), dsd_wifi_phy_rate_type_data_v01},
  {sizeof(dsd_wifi_packet_error_rate_type_v01), dsd_wifi_packet_error_rate_type_data_v01},
  {0, 0},
  {0, 0},
  {0, 0},
  {sizeof(dsd_lte_cell_info_type_v01), dsd_lte_cell_info_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry dsd_message_table_v01[] = {
  {sizeof(dsd_get_system_status_req_msg_v01), 0},
  {sizeof(dsd_get_system_status_resp_msg_v01), dsd_get_system_status_resp_msg_data_v01},
  {sizeof(dsd_system_status_change_req_msg_v01), dsd_system_status_change_req_msg_data_v01},
  {sizeof(dsd_system_status_change_resp_msg_v01), dsd_system_status_change_resp_msg_data_v01},
  {sizeof(dsd_system_status_ind_msg_v01), dsd_system_status_ind_msg_data_v01},
  {sizeof(dsd_get_ui_info_req_msg_v01), 0},
  {sizeof(dsd_get_ui_info_resp_msg_v01), dsd_get_ui_info_resp_msg_data_v01},
  {sizeof(dsd_register_ui_info_change_req_msg_v01), dsd_register_ui_info_change_req_msg_data_v01},
  {sizeof(dsd_register_ui_info_change_resp_msg_v01), dsd_register_ui_info_change_resp_msg_data_v01},
  {sizeof(dsd_ui_info_ind_msg_v01), dsd_ui_info_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object dsd_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *dsd_qmi_idl_type_table_object_referenced_tables_v01[] =
{&dsd_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object dsd_qmi_idl_type_table_object_v01 = {
  sizeof(dsd_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(dsd_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  dsd_type_table_v01,
  dsd_message_table_v01,
  dsd_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry dsd_service_command_messages_v01[] = {
  {QMI_DSD_GET_SYSTEM_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_DSD_SYSTEM_STATUS_CHANGE_REQ_V01, QMI_IDL_TYPE16(0, 2), 16},
  {QMI_DSD_GET_UI_INFO_REQ_V01, QMI_IDL_TYPE16(0, 5), 0},
  {QMI_DSD_REGISTER_UI_INFO_CHANGE_REQ_V01, QMI_IDL_TYPE16(0, 7), 12}
};

static const qmi_idl_service_message_table_entry dsd_service_response_messages_v01[] = {
  {QMI_DSD_GET_SYSTEM_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 1), 7430},
  {QMI_DSD_SYSTEM_STATUS_CHANGE_RESP_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_DSD_GET_UI_INFO_RESP_V01, QMI_IDL_TYPE16(0, 6), 2059},
  {QMI_DSD_REGISTER_UI_INFO_CHANGE_RESP_V01, QMI_IDL_TYPE16(0, 8), 7}
};

static const qmi_idl_service_message_table_entry dsd_service_indication_messages_v01[] = {
  {QMI_DSD_SYSTEM_STATUS_IND_V01, QMI_IDL_TYPE16(0, 4), 7423},
  {QMI_DSD_UI_INFO_IND_V01, QMI_IDL_TYPE16(0, 9), 2052}
};

/*Service Object*/
struct qmi_idl_service_object dsd_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x2A,
  7430,
  { sizeof(dsd_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(dsd_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(dsd_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { dsd_service_command_messages_v01, dsd_service_response_messages_v01, dsd_service_indication_messages_v01},
  &dsd_qmi_idl_type_table_object_v01,
  0x35,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type dsd_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( DSD_V01_IDL_MAJOR_VERS != idl_maj_version || DSD_V01_IDL_MINOR_VERS != idl_min_version
       || DSD_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&dsd_qmi_idl_service_object_v01;
}

