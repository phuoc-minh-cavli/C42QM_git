/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        T S Y N C _ C O N T R O L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the tsync service Data structures.

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved. 
 Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/qmimsgs.tx/1.0/tsync/src/tsync_control_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2 
   It was generated on: Wed Jul 24 2013 (Spin 2)
   From IDL File: tsync_control_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "tsync_control_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t time_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(time_type_v01, utc_hour),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(time_type_v01, utc_minute),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(time_type_v01, utc_second),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(time_type_v01, utc_nanosecond),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(time_type_v01, resolution_ns),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(time_type_v01, year),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(time_type_v01, month),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(time_type_v01, day),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(time_type_v01, sync_period_ms),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(time_type_v01, elapsed_ms),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(time_type_v01, timebase_correction_ns),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t cdma_network_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(cdma_network_type_v01, cdma_band_class),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(cdma_network_type_v01, cdma_channel),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(cdma_network_type_v01, cdma_sid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(cdma_network_type_v01, cdma_nid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(cdma_network_type_v01, cdma_pilot_pn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(cdma_network_type_v01, leap_seconds),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(cdma_network_type_v01, local_timezone_offset),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(cdma_network_type_v01, daylight_saving),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t qmi_tsync_enable_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_tsync_enable_req_msg_v01, tsync_enable_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_tsync_enable_req_msg_v01, enable_state_ind) - QMI_IDL_OFFSET8(qmi_tsync_enable_req_msg_v01, enable_state_ind_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_tsync_enable_req_msg_v01, enable_state_ind),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_tsync_enable_req_msg_v01, enable_event_ind) - QMI_IDL_OFFSET8(qmi_tsync_enable_req_msg_v01, enable_event_ind_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_tsync_enable_req_msg_v01, enable_event_ind)
};

static const uint8_t qmi_tsync_enable_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_tsync_enable_resp_msg_v01, qmi_result),
  QMI_IDL_TYPE88(1, 0)
};

/* 
 * qmi_tsync_get_state_req_msg is empty
 * static const uint8_t qmi_tsync_get_state_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t qmi_tsync_get_state_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_tsync_get_state_resp_msg_v01, qmi_result),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_tsync_get_state_resp_msg_v01, tsync_state) - QMI_IDL_OFFSET8(qmi_tsync_get_state_resp_msg_v01, tsync_state_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_tsync_get_state_resp_msg_v01, tsync_state),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_tsync_get_state_resp_msg_v01, sync_period_ms) - QMI_IDL_OFFSET8(qmi_tsync_get_state_resp_msg_v01, sync_period_ms_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmi_tsync_get_state_resp_msg_v01, sync_period_ms)
};

static const uint8_t qmi_tsync_state_report_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_tsync_state_report_ind_msg_v01, tsync_state),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_tsync_state_report_ind_msg_v01, sync_period_ms) - QMI_IDL_OFFSET8(qmi_tsync_state_report_ind_msg_v01, sync_period_ms_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmi_tsync_state_report_ind_msg_v01, sync_period_ms)
};

static const uint8_t qmi_tsync_event_report_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_tsync_event_report_ind_msg_v01, time_info),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_tsync_event_report_ind_msg_v01, cdma_network_info) - QMI_IDL_OFFSET8(qmi_tsync_event_report_ind_msg_v01, cdma_network_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_tsync_event_report_ind_msg_v01, cdma_network_info),
  QMI_IDL_TYPE88(0, 1)
};

/* Type Table */
static const qmi_idl_type_table_entry  tsync_type_table_v01[] = {
  {sizeof(time_type_v01), time_type_data_v01},
  {sizeof(cdma_network_type_v01), cdma_network_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry tsync_message_table_v01[] = {
  {sizeof(qmi_tsync_enable_req_msg_v01), qmi_tsync_enable_req_msg_data_v01},
  {sizeof(qmi_tsync_enable_resp_msg_v01), qmi_tsync_enable_resp_msg_data_v01},
  {sizeof(qmi_tsync_get_state_req_msg_v01), 0},
  {sizeof(qmi_tsync_get_state_resp_msg_v01), qmi_tsync_get_state_resp_msg_data_v01},
  {sizeof(qmi_tsync_state_report_ind_msg_v01), qmi_tsync_state_report_ind_msg_data_v01},
  {sizeof(qmi_tsync_event_report_ind_msg_v01), qmi_tsync_event_report_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object tsync_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *tsync_qmi_idl_type_table_object_referenced_tables_v01[] =
{&tsync_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object tsync_qmi_idl_type_table_object_v01 = {
  sizeof(tsync_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(tsync_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  tsync_type_table_v01,
  tsync_message_table_v01,
  tsync_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry tsync_service_command_messages_v01[] = {
  {QMI_TSYNC_ENABLE_REQ_V01, QMI_IDL_TYPE16(0, 0), 15},
  {QMI_TSYNC_GET_STATE_REQ_V01, QMI_IDL_TYPE16(0, 2), 0}
};

static const qmi_idl_service_message_table_entry tsync_service_response_messages_v01[] = {
  {QMI_TSYNC_ENABLE_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_TSYNC_GET_STATE_RESP_V01, QMI_IDL_TYPE16(0, 3), 19}
};

static const qmi_idl_service_message_table_entry tsync_service_indication_messages_v01[] = {
  {QMI_TSYNC_STATE_REPORT_IND_IND_V01, QMI_IDL_TYPE16(0, 4), 12},
  {QMI_TSYNC_EVENT_REPORT_IND_IND_V01, QMI_IDL_TYPE16(0, 5), 45}
};

/*Service Object*/
struct qmi_idl_service_object tsync_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x1B,
  45,
  { sizeof(tsync_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(tsync_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(tsync_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { tsync_service_command_messages_v01, tsync_service_response_messages_v01, tsync_service_indication_messages_v01},
  &tsync_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type tsync_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( TSYNC_V01_IDL_MAJOR_VERS != idl_maj_version || TSYNC_V01_IDL_MINOR_VERS != idl_min_version 
       || TSYNC_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&tsync_qmi_idl_service_object_v01;
}

