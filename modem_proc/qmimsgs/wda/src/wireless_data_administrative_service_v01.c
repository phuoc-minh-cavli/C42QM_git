/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        W I R E L E S S _ D A T A _ A D M I N I S T R A T I V E _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the wda service Data structures.

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.mpss/4.3.4/wda/src/wireless_data_administrative_service_v01.c#2 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Wed Feb  6 2019 (Spin 0)
   From IDL File: wireless_data_administrative_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "wireless_data_administrative_service_v01.h"
#include "common_v01.h"
#include "data_common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t wda_set_data_format_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, qos_format) - QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, qos_format_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, qos_format),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, link_prot) - QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, link_prot_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, link_prot),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ul_data_aggregation_protocol) - QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ul_data_aggregation_protocol_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ul_data_aggregation_protocol),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, dl_data_aggregation_protocol) - QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, dl_data_aggregation_protocol_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, dl_data_aggregation_protocol),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ndp_signature) - QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ndp_signature_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ndp_signature),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, dl_data_aggregation_max_datagrams) - QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, dl_data_aggregation_max_datagrams_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, dl_data_aggregation_max_datagrams),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, dl_data_aggregation_max_size) - QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, dl_data_aggregation_max_size_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, dl_data_aggregation_max_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ep_id) - QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ep_id_valid)),
  0x17,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ep_id),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, qos_header_format) - QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, qos_header_format_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, qos_header_format),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, dl_minimum_padding) - QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, dl_minimum_padding_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, dl_minimum_padding),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, flow_control) - QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, flow_control_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, flow_control),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ul_data_aggregation_max_datagrams) - QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ul_data_aggregation_max_datagrams_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ul_data_aggregation_max_datagrams),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ul_data_aggregation_max_size) - QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ul_data_aggregation_max_size_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_req_msg_v01, ul_data_aggregation_max_size)
};

static const uint8_t wda_set_data_format_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, qos_format) - QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, qos_format_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, qos_format),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, link_prot) - QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, link_prot_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, link_prot),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, ul_data_aggregation_protocol) - QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, ul_data_aggregation_protocol_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, ul_data_aggregation_protocol),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, dl_data_aggregation_protocol) - QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, dl_data_aggregation_protocol_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, dl_data_aggregation_protocol),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, ndp_signature) - QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, ndp_signature_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, ndp_signature),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, dl_data_aggregation_max_datagrams) - QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, dl_data_aggregation_max_datagrams_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, dl_data_aggregation_max_datagrams),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, dl_data_aggregation_max_size) - QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, dl_data_aggregation_max_size_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, dl_data_aggregation_max_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, ul_data_aggregation_max_datagrams) - QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, ul_data_aggregation_max_datagrams_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, ul_data_aggregation_max_datagrams),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, ul_data_aggregation_max_size) - QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, ul_data_aggregation_max_size_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, ul_data_aggregation_max_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, qos_header_format) - QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, qos_header_format_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, qos_header_format),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, dl_minimum_padding) - QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, dl_minimum_padding_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, dl_minimum_padding),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, flow_control) - QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, flow_control_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wda_set_data_format_resp_msg_v01, flow_control)
};

static const uint8_t wda_get_data_format_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_data_format_req_msg_v01, ep_id) - QMI_IDL_OFFSET8(wda_get_data_format_req_msg_v01, ep_id_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wda_get_data_format_req_msg_v01, ep_id),
  QMI_IDL_TYPE88(2, 0)
};

static const uint8_t wda_get_data_format_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, qos_format) - QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, qos_format_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, qos_format),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, link_prot) - QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, link_prot_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, link_prot),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, ul_data_aggregation_protocol) - QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, ul_data_aggregation_protocol_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, ul_data_aggregation_protocol),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, dl_data_aggregation_protocol) - QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, dl_data_aggregation_protocol_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, dl_data_aggregation_protocol),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, ndp_signature) - QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, ndp_signature_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, ndp_signature),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, dl_data_aggregation_max_datagrams) - QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, dl_data_aggregation_max_datagrams_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, dl_data_aggregation_max_datagrams),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, dl_data_aggregation_max_size) - QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, dl_data_aggregation_max_size_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, dl_data_aggregation_max_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, ul_data_aggregation_max_datagrams) - QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, ul_data_aggregation_max_datagrams_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, ul_data_aggregation_max_datagrams),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, ul_data_aggregation_max_size) - QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, ul_data_aggregation_max_size_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, ul_data_aggregation_max_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, qos_header_format) - QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, qos_header_format_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, qos_header_format),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, dl_minimum_padding) - QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, dl_minimum_padding_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, dl_minimum_padding),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, flow_control) - QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, flow_control_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wda_get_data_format_resp_msg_v01, flow_control)
};

static const uint8_t wda_set_qmap_settings_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_qmap_settings_req_msg_v01, in_band_flow_control) - QMI_IDL_OFFSET8(wda_set_qmap_settings_req_msg_v01, in_band_flow_control_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wda_set_qmap_settings_req_msg_v01, in_band_flow_control),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_qmap_settings_req_msg_v01, ep_id) - QMI_IDL_OFFSET8(wda_set_qmap_settings_req_msg_v01, ep_id_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wda_set_qmap_settings_req_msg_v01, ep_id),
  QMI_IDL_TYPE88(2, 0)
};

static const uint8_t wda_set_qmap_settings_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wda_set_qmap_settings_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_set_qmap_settings_resp_msg_v01, in_band_flow_control) - QMI_IDL_OFFSET8(wda_set_qmap_settings_resp_msg_v01, in_band_flow_control_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wda_set_qmap_settings_resp_msg_v01, in_band_flow_control)
};

static const uint8_t wda_get_qmap_settings_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_qmap_settings_req_msg_v01, ep_id) - QMI_IDL_OFFSET8(wda_get_qmap_settings_req_msg_v01, ep_id_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wda_get_qmap_settings_req_msg_v01, ep_id),
  QMI_IDL_TYPE88(2, 0)
};

static const uint8_t wda_get_qmap_settings_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wda_get_qmap_settings_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wda_get_qmap_settings_resp_msg_v01, in_band_flow_control) - QMI_IDL_OFFSET8(wda_get_qmap_settings_resp_msg_v01, in_band_flow_control_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wda_get_qmap_settings_resp_msg_v01, in_band_flow_control)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry wda_message_table_v01[] = {
  {sizeof(wda_set_data_format_req_msg_v01), wda_set_data_format_req_msg_data_v01},
  {sizeof(wda_set_data_format_resp_msg_v01), wda_set_data_format_resp_msg_data_v01},
  {sizeof(wda_get_data_format_req_msg_v01), wda_get_data_format_req_msg_data_v01},
  {sizeof(wda_get_data_format_resp_msg_v01), wda_get_data_format_resp_msg_data_v01},
  {sizeof(wda_set_qmap_settings_req_msg_v01), wda_set_qmap_settings_req_msg_data_v01},
  {sizeof(wda_set_qmap_settings_resp_msg_v01), wda_set_qmap_settings_resp_msg_data_v01},
  {sizeof(wda_get_qmap_settings_req_msg_v01), wda_get_qmap_settings_req_msg_data_v01},
  {sizeof(wda_get_qmap_settings_resp_msg_v01), wda_get_qmap_settings_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object wda_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *wda_qmi_idl_type_table_object_referenced_tables_v01[] =
{&wda_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01, &data_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object wda_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(wda_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  wda_message_table_v01,
  wda_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry wda_service_command_messages_v01[] = {
  {QMI_WDA_SET_DATA_FORMAT_REQ_V01, QMI_IDL_TYPE16(0, 0), 89},
  {QMI_WDA_GET_DATA_FORMAT_REQ_V01, QMI_IDL_TYPE16(0, 2), 11},
  {QMI_WDA_SET_QMAP_SETTINGS_REQ_V01, QMI_IDL_TYPE16(0, 4), 15},
  {QMI_WDA_GET_QMAP_SETTINGS_REQ_V01, QMI_IDL_TYPE16(0, 6), 11}
};

static const qmi_idl_service_message_table_entry wda_service_response_messages_v01[] = {
  {QMI_WDA_SET_DATA_FORMAT_RESP_V01, QMI_IDL_TYPE16(0, 1), 85},
  {QMI_WDA_GET_DATA_FORMAT_RESP_V01, QMI_IDL_TYPE16(0, 3), 85},
  {QMI_WDA_SET_QMAP_SETTINGS_RESP_V01, QMI_IDL_TYPE16(0, 5), 11},
  {QMI_WDA_GET_QMAP_SETTINGS_RESP_V01, QMI_IDL_TYPE16(0, 7), 11}
};

/*Service Object*/
struct qmi_idl_service_object wda_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x1A,
  8204,
  { sizeof(wda_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(wda_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    0 },
  { wda_service_command_messages_v01, wda_service_response_messages_v01, NULL},
  &wda_qmi_idl_type_table_object_v01,
  0x14,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type wda_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( WDA_V01_IDL_MAJOR_VERS != idl_maj_version || WDA_V01_IDL_MINOR_VERS != idl_min_version
       || WDA_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&wda_qmi_idl_service_object_v01;
}

