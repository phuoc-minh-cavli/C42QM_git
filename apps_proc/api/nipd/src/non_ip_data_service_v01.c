/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        N O N _ I P _ D A T A _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the nipd service Data structures.

  Copyright (c) 2017 -2023 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.


  $Header: //components/rel/qmimsgs.tx/1.0/nipd/src/non_ip_data_service_v01.c#2 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Thu May  4 2023 (Spin 0)
   From IDL File: non_ip_data_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "non_ip_data_service_v01.h"
#include "common_v01.h"
#include "data_common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t nipd_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nipd_indication_register_req_msg_v01, report_downlink_data) - QMI_IDL_OFFSET8(nipd_indication_register_req_msg_v01, report_downlink_data_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nipd_indication_register_req_msg_v01, report_downlink_data),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nipd_indication_register_req_msg_v01, report_data_delivery) - QMI_IDL_OFFSET8(nipd_indication_register_req_msg_v01, report_data_delivery_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nipd_indication_register_req_msg_v01, report_data_delivery)
};

static const uint8_t nipd_indication_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nipd_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nipd_bind_client_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nipd_bind_client_req_msg_v01, ep_id) - QMI_IDL_OFFSET8(nipd_bind_client_req_msg_v01, ep_id_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nipd_bind_client_req_msg_v01, ep_id),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nipd_bind_client_req_msg_v01, mux_id) - QMI_IDL_OFFSET8(nipd_bind_client_req_msg_v01, mux_id_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nipd_bind_client_req_msg_v01, mux_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nipd_bind_client_req_msg_v01, bind_subs) - QMI_IDL_OFFSET8(nipd_bind_client_req_msg_v01, bind_subs_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nipd_bind_client_req_msg_v01, bind_subs)
};

static const uint8_t nipd_bind_client_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nipd_bind_client_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nipd_get_client_binding_req_msg is empty
 * static const uint8_t nipd_get_client_binding_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nipd_get_client_binding_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nipd_get_client_binding_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nipd_get_client_binding_resp_msg_v01, bound_ep_id) - QMI_IDL_OFFSET8(nipd_get_client_binding_resp_msg_v01, bound_ep_id_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nipd_get_client_binding_resp_msg_v01, bound_ep_id),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nipd_get_client_binding_resp_msg_v01, bound_mux_id) - QMI_IDL_OFFSET8(nipd_get_client_binding_resp_msg_v01, bound_mux_id_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nipd_get_client_binding_resp_msg_v01, bound_mux_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nipd_get_client_binding_resp_msg_v01, bound_subs) - QMI_IDL_OFFSET8(nipd_get_client_binding_resp_msg_v01, bound_subs_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nipd_get_client_binding_resp_msg_v01, bound_subs)
};

/*
 * nipd_get_capability_req_msg is empty
 * static const uint8_t nipd_get_capability_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nipd_get_capability_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nipd_get_capability_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nipd_get_capability_resp_msg_v01, allowed_data_payload_length) - QMI_IDL_OFFSET8(nipd_get_capability_resp_msg_v01, allowed_data_payload_length_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nipd_get_capability_resp_msg_v01, allowed_data_payload_length)
};

static const uint8_t nipd_downlink_data_ind_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nipd_downlink_data_ind_msg_v01, payload),
  ((QMI_NIPD_MAX_PAYLOAD_SIZE_V01) & 0xFF), ((QMI_NIPD_MAX_PAYLOAD_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(nipd_downlink_data_ind_msg_v01, payload) - QMI_IDL_OFFSET8(nipd_downlink_data_ind_msg_v01, payload_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nipd_downlink_data_ind_msg_v01, is_final_segment) - QMI_IDL_OFFSET16RELATIVE(nipd_downlink_data_ind_msg_v01, is_final_segment_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nipd_downlink_data_ind_msg_v01, is_final_segment),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nipd_downlink_data_ind_msg_v01, segment_number) - QMI_IDL_OFFSET16RELATIVE(nipd_downlink_data_ind_msg_v01, segment_number_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nipd_downlink_data_ind_msg_v01, segment_number)
};

static const uint8_t nipd_uplink_data_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nipd_uplink_data_req_msg_v01, payload),
  ((QMI_NIPD_MAX_PAYLOAD_SIZE_V01) & 0xFF), ((QMI_NIPD_MAX_PAYLOAD_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(nipd_uplink_data_req_msg_v01, payload) - QMI_IDL_OFFSET8(nipd_uplink_data_req_msg_v01, payload_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nipd_uplink_data_req_msg_v01, is_final_segment) - QMI_IDL_OFFSET16RELATIVE(nipd_uplink_data_req_msg_v01, is_final_segment_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nipd_uplink_data_req_msg_v01, is_final_segment),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nipd_uplink_data_req_msg_v01, segment_number) - QMI_IDL_OFFSET16RELATIVE(nipd_uplink_data_req_msg_v01, segment_number_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nipd_uplink_data_req_msg_v01, segment_number),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nipd_uplink_data_req_msg_v01, is_mo_exceptional_data) - QMI_IDL_OFFSET16RELATIVE(nipd_uplink_data_req_msg_v01, is_mo_exceptional_data_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nipd_uplink_data_req_msg_v01, is_mo_exceptional_data)
};

static const uint8_t nipd_uplink_data_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nipd_uplink_data_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nipd_ul_data_delivery_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nipd_ul_data_delivery_ind_msg_v01, delivery_status)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry nipd_message_table_v01[] = {
  {sizeof(nipd_indication_register_req_msg_v01), nipd_indication_register_req_msg_data_v01},
  {sizeof(nipd_indication_register_resp_msg_v01), nipd_indication_register_resp_msg_data_v01},
  {sizeof(nipd_bind_client_req_msg_v01), nipd_bind_client_req_msg_data_v01},
  {sizeof(nipd_bind_client_resp_msg_v01), nipd_bind_client_resp_msg_data_v01},
  {sizeof(nipd_get_client_binding_req_msg_v01), 0},
  {sizeof(nipd_get_client_binding_resp_msg_v01), nipd_get_client_binding_resp_msg_data_v01},
  {sizeof(nipd_get_capability_req_msg_v01), 0},
  {sizeof(nipd_get_capability_resp_msg_v01), nipd_get_capability_resp_msg_data_v01},
  {sizeof(nipd_downlink_data_ind_msg_v01), nipd_downlink_data_ind_msg_data_v01},
  {sizeof(nipd_uplink_data_req_msg_v01), nipd_uplink_data_req_msg_data_v01},
  {sizeof(nipd_uplink_data_resp_msg_v01), nipd_uplink_data_resp_msg_data_v01},
  {sizeof(nipd_ul_data_delivery_ind_msg_v01), nipd_ul_data_delivery_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object nipd_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *nipd_qmi_idl_type_table_object_referenced_tables_v01[] =
{&nipd_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01, &data_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object nipd_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(nipd_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  nipd_message_table_v01,
  nipd_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry nipd_service_command_messages_v01[] = {
  {QMI_NIPD_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 0), 8},
  {QMI_NIPD_BIND_CLIENT_REQ_V01, QMI_IDL_TYPE16(0, 2), 22},
  {QMI_NIPD_GET_CLIENT_BINDING_REQ_V01, QMI_IDL_TYPE16(0, 4), 0},
  {QMI_NIPD_GET_CAPABILITY_REQ_V01, QMI_IDL_TYPE16(0, 6), 0},
  {QMI_NIPD_UPLINK_DATA_REQ_V01, QMI_IDL_TYPE16(0, 9), 2065}
};

static const qmi_idl_service_message_table_entry nipd_service_response_messages_v01[] = {
  {QMI_NIPD_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_NIPD_BIND_CLIENT_RESP_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_NIPD_GET_CLIENT_BINDING_RESP_V01, QMI_IDL_TYPE16(0, 5), 29},
  {QMI_NIPD_GET_CAPABILITY_RESP_V01, QMI_IDL_TYPE16(0, 7), 14},
  {QMI_NIPD_UPLINK_DATA_RESP_V01, QMI_IDL_TYPE16(0, 10), 7}
};

static const qmi_idl_service_message_table_entry nipd_service_indication_messages_v01[] = {
  {QMI_NIPD_DOWN_LINK_DATA_IND_V01, QMI_IDL_TYPE16(0, 8), 2061},
  {QMI_NIPD_UL_DATA_DELIVERY_IND_V01, QMI_IDL_TYPE16(0, 11), 7}
};

/*Service Object*/
struct qmi_idl_service_object nipd_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x48,
  2065,
  { sizeof(nipd_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(nipd_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(nipd_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { nipd_service_command_messages_v01, nipd_service_response_messages_v01, nipd_service_indication_messages_v01},
  &nipd_qmi_idl_type_table_object_v01,
  0x02,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type nipd_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( NIPD_V01_IDL_MAJOR_VERS != idl_maj_version || NIPD_V01_IDL_MINOR_VERS != idl_min_version
       || NIPD_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&nipd_qmi_idl_service_object_v01;
}

