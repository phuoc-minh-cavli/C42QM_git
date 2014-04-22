/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        W I R E L E S S _ D A T A _ S E R V I C E _ E X T _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the wds_ext service Data structures.

  Copyright (c) 2006-2015 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.


  $Header: //components/rel/qmimsgs.mpss/4.3.4/wds_ext/src/wireless_data_service_ext_v01.c#2 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Thu Jan 17 2019 (Spin 0)
   From IDL File: wireless_data_service_ext_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "wireless_data_service_ext_v01.h"
#include "common_v01.h"
#include "data_common_v01.h"
#include "wireless_data_service_common_v01.h"


/*Type Definitions*/
static const uint8_t wds_iface_handle_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wds_iface_handle_type_v01, iface_handle),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wds_iface_handle_type_v01, priority),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(wds_iface_handle_type_v01, technology_name),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wds_arb_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wds_arb_info_type_v01, iface_priority_mask),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wds_arb_info_type_v01, is_iface_arb_candiate),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wds_delay_metrics_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wds_delay_metrics_type_v01, max),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wds_delay_metrics_type_v01, min),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wds_delay_metrics_type_v01, avg),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wds_delay_metrics_type_v01, num_samples),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wds_bearer_delay_metrics_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wds_bearer_delay_metrics_type_v01, bearer_id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wds_bearer_delay_metrics_type_v01, rm_l2_delay),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wds_bearer_delay_metrics_type_v01, l2_tx_delay),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t wds_ext_route_look_up_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, route_look_up_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, apn_name) - QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, apn_name_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, apn_name),
  QMI_WDS_APN_NAME_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, ip_family_pref) - QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, ip_family_pref_valid)),
  0x19,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, ip_family_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, umts_profile) - QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, umts_profile_valid)),
  0x31,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, umts_profile),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, cdma_profile) - QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, cdma_profile_valid)),
  0x32,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, cdma_profile),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, ext_tech_pref) - QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, ext_tech_pref_valid)),
  0x34,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, ext_tech_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, mo_exceptional_data_capability) - QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, mo_exceptional_data_capability_valid)),
  0x35,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, mo_exceptional_data_capability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, apn_type) - QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, apn_type_valid)),
  0x38,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, apn_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, bring_up_by_apn_name) - QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, bring_up_by_apn_name_valid)),
  0x3C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, bring_up_by_apn_name),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, bring_up_by_apn_type) - QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, bring_up_by_apn_type_valid)),
  0x3D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_req_msg_v01, bring_up_by_apn_type)
};

static const uint8_t wds_ext_route_look_up_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_resp_msg_v01, iface_handle),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ext_route_look_up_resp_msg_v01, qmi_instance) - QMI_IDL_OFFSET8(wds_ext_route_look_up_resp_msg_v01, qmi_instance_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_resp_msg_v01, qmi_instance),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ext_route_look_up_resp_msg_v01, arb_info) - QMI_IDL_OFFSET8(wds_ext_route_look_up_resp_msg_v01, arb_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_resp_msg_v01, arb_info),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ext_route_look_up_resp_msg_v01, ep_id) - QMI_IDL_OFFSET8(wds_ext_route_look_up_resp_msg_v01, ep_id_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_resp_msg_v01, ep_id),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ext_route_look_up_resp_msg_v01, mux_id) - QMI_IDL_OFFSET8(wds_ext_route_look_up_resp_msg_v01, mux_id_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wds_ext_route_look_up_resp_msg_v01, mux_id)
};

/*
 * wds_get_ul_delay_metrics_req_msg is empty
 * static const uint8_t wds_get_ul_delay_metrics_req_msg_data_v01[] = {
 * };
 */

static const uint8_t wds_get_ul_delay_metrics_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wds_get_ul_delay_metrics_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_get_ul_delay_metrics_resp_msg_v01, ul_delay_metrics) - QMI_IDL_OFFSET8(wds_get_ul_delay_metrics_resp_msg_v01, ul_delay_metrics_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wds_get_ul_delay_metrics_resp_msg_v01, ul_delay_metrics),
  QMI_WDS_MAX_BEARER_CONTEXT_V01,
  QMI_IDL_OFFSET8(wds_get_ul_delay_metrics_resp_msg_v01, ul_delay_metrics) - QMI_IDL_OFFSET8(wds_get_ul_delay_metrics_resp_msg_v01, ul_delay_metrics_len),
  QMI_IDL_TYPE88(0, 3)
};

/*
 * wds_reset_ul_delay_metrics_req_msg is empty
 * static const uint8_t wds_reset_ul_delay_metrics_req_msg_data_v01[] = {
 * };
 */

static const uint8_t wds_reset_ul_delay_metrics_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wds_reset_ul_delay_metrics_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wds_ul_dl_throughput_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ul_dl_throughput_ind_register_req_msg_v01, ul_dl_throughput_interval) - QMI_IDL_OFFSET8(wds_ul_dl_throughput_ind_register_req_msg_v01, ul_dl_throughput_interval_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wds_ul_dl_throughput_ind_register_req_msg_v01, ul_dl_throughput_interval)
};

static const uint8_t wds_ul_dl_throughput_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wds_ul_dl_throughput_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wds_ul_dl_throughput_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ul_dl_throughput_info_ind_msg_v01, ul_achievable_throughput) - QMI_IDL_OFFSET8(wds_ul_dl_throughput_info_ind_msg_v01, ul_achievable_throughput_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wds_ul_dl_throughput_info_ind_msg_v01, ul_achievable_throughput),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ul_dl_throughput_info_ind_msg_v01, dl_achievable_throughput) - QMI_IDL_OFFSET8(wds_ul_dl_throughput_info_ind_msg_v01, dl_achievable_throughput_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wds_ul_dl_throughput_info_ind_msg_v01, dl_achievable_throughput),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ul_dl_throughput_info_ind_msg_v01, ul_actual_throughput) - QMI_IDL_OFFSET8(wds_ul_dl_throughput_info_ind_msg_v01, ul_actual_throughput_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wds_ul_dl_throughput_info_ind_msg_v01, ul_actual_throughput),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wds_ul_dl_throughput_info_ind_msg_v01, dl_actual_throughput) - QMI_IDL_OFFSET8(wds_ul_dl_throughput_info_ind_msg_v01, dl_actual_throughput_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wds_ul_dl_throughput_info_ind_msg_v01, dl_actual_throughput)
};

/* Type Table */
static const qmi_idl_type_table_entry  wds_ext_type_table_v01[] = {
  {sizeof(wds_iface_handle_type_v01), wds_iface_handle_type_data_v01},
  {sizeof(wds_arb_info_type_v01), wds_arb_info_type_data_v01},
  {sizeof(wds_delay_metrics_type_v01), wds_delay_metrics_type_data_v01},
  {sizeof(wds_bearer_delay_metrics_type_v01), wds_bearer_delay_metrics_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry wds_ext_message_table_v01[] = {
  {sizeof(wds_ext_route_look_up_req_msg_v01), wds_ext_route_look_up_req_msg_data_v01},
  {sizeof(wds_ext_route_look_up_resp_msg_v01), wds_ext_route_look_up_resp_msg_data_v01},
  {sizeof(wds_get_ul_delay_metrics_req_msg_v01), 0},
  {sizeof(wds_get_ul_delay_metrics_resp_msg_v01), wds_get_ul_delay_metrics_resp_msg_data_v01},
  {sizeof(wds_reset_ul_delay_metrics_req_msg_v01), 0},
  {sizeof(wds_reset_ul_delay_metrics_resp_msg_v01), wds_reset_ul_delay_metrics_resp_msg_data_v01},
  {sizeof(wds_ul_dl_throughput_ind_register_req_msg_v01), wds_ul_dl_throughput_ind_register_req_msg_data_v01},
  {sizeof(wds_ul_dl_throughput_ind_register_resp_msg_v01), wds_ul_dl_throughput_ind_register_resp_msg_data_v01},
  {sizeof(wds_ul_dl_throughput_info_ind_msg_v01), wds_ul_dl_throughput_info_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object wds_ext_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *wds_ext_qmi_idl_type_table_object_referenced_tables_v01[] =
{&wds_ext_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01, &data_common_qmi_idl_type_table_object_v01, &wireless_data_service_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object wds_ext_qmi_idl_type_table_object_v01 = {
  sizeof(wds_ext_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(wds_ext_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  wds_ext_type_table_v01,
  wds_ext_message_table_v01,
  wds_ext_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry wds_ext_service_command_messages_v01[] = {
  {QMI_WDS_UL_DL_THROUGHPUT_IND_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 6), 7},
  {QMI_WDS_GET_UL_DELAY_METRICS_REQ_V01, QMI_IDL_TYPE16(0, 2), 0},
  {QMI_WDS_RESET_UL_DELAY_METRICS_REQ_V01, QMI_IDL_TYPE16(0, 4), 0},
  {QMI_WDS_EXT_ROUTE_LOOK_UP_REQ_V01, QMI_IDL_TYPE16(0, 0), 193}
};

static const qmi_idl_service_message_table_entry wds_ext_service_response_messages_v01[] = {
  {QMI_WDS_UL_DL_THROUGHPUT_IND_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 7), 7},
  {QMI_WDS_GET_UL_DELAY_METRICS_RESP_V01, QMI_IDL_TYPE16(0, 3), 275},
  {QMI_WDS_RESET_UL_DELAY_METRICS_RESP_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_WDS_EXT_ROUTE_LOOK_UP_RESP_V01, QMI_IDL_TYPE16(0, 1), 47}
};

static const qmi_idl_service_message_table_entry wds_ext_service_indication_messages_v01[] = {
  {QMI_WDS_UL_DL_THROUGHPUT_INFO_IND_V01, QMI_IDL_TYPE16(0, 8), 28}
};

/*Service Object*/
struct qmi_idl_service_object wds_ext_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x01,
  275,
  { sizeof(wds_ext_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(wds_ext_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(wds_ext_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { wds_ext_service_command_messages_v01, wds_ext_service_response_messages_v01, wds_ext_service_indication_messages_v01},
  &wds_ext_qmi_idl_type_table_object_v01,
  0x05,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type wds_ext_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( WDS_EXT_V01_IDL_MAJOR_VERS != idl_maj_version || WDS_EXT_V01_IDL_MINOR_VERS != idl_min_version
       || WDS_EXT_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&wds_ext_qmi_idl_service_object_v01;
}

