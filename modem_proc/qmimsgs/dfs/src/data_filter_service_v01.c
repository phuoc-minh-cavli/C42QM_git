/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A _ F I L T E R _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the dfs service Data structures.

  Copyright (c) 2013-2017 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
  


  $Header: //components/rel/qmimsgs.mpss/4.3.4/dfs/src/data_filter_service_v01.c#3 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Wed Feb 12 2020 (Spin 0)
   From IDL File: data_filter_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "data_filter_service_v01.h"
#include "common_v01.h"
#include "data_common_v01.h"


/*Type Definitions*/
static const uint8_t dfs_ipv4_addr_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dfs_ipv4_addr_type_v01, ipv4_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dfs_ipv4_addr_type_v01, subnet_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dfs_ipv4_tos_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dfs_ipv4_tos_type_v01, val),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dfs_ipv4_tos_type_v01, mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dfs_ipv4_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dfs_ipv4_info_type_v01, valid_params),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_ipv4_info_type_v01, src_addr),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_ipv4_info_type_v01, dest_addr),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_ipv4_info_type_v01, tos),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dfs_ipv6_trf_cls_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dfs_ipv6_trf_cls_type_v01, val),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dfs_ipv6_trf_cls_type_v01, mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dfs_ipv6_addr_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dfs_ipv6_addr_type_v01, ipv6_address),
  QMI_DFS_IPV6_ADDR_LEN_V01,

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dfs_ipv6_addr_type_v01, prefix_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dfs_ipv6_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dfs_ipv6_info_type_v01, valid_params),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_ipv6_info_type_v01, src_addr),
  QMI_IDL_TYPE88(0, 4),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_ipv6_info_type_v01, dest_addr),
  QMI_IDL_TYPE88(0, 4),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_ipv6_info_type_v01, trf_cls),
  QMI_IDL_TYPE88(0, 3),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dfs_ipv6_info_type_v01, flow_label),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dfs_ip_header_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(dfs_ip_header_type_v01, ip_version),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_ip_header_type_v01, v4_info),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_ip_header_type_v01, v6_info),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dfs_port_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dfs_port_type_v01, port),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dfs_port_type_v01, range),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dfs_port_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dfs_port_info_type_v01, valid_params),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_port_info_type_v01, src_port_info),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_port_info_type_v01, dest_port_info),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dfs_icmp_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dfs_icmp_info_type_v01, valid_params),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dfs_icmp_info_type_v01, type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dfs_icmp_info_type_v01, code),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dfs_ipsec_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dfs_ipsec_info_type_v01, valid_params),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dfs_ipsec_info_type_v01, spi),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dfs_xport_header_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dfs_xport_header_type_v01, xport_protocol),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_xport_header_type_v01, tcp_info),
  QMI_IDL_TYPE88(0, 8),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_xport_header_type_v01, udp_info),
  QMI_IDL_TYPE88(0, 8),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_xport_header_type_v01, icmp_info),
  QMI_IDL_TYPE88(0, 9),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_xport_header_type_v01, esp_info),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_xport_header_type_v01, ah_info),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dfs_filter_rule_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_filter_rule_type_v01, ip_info),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_filter_rule_type_v01, xport_info),
  QMI_IDL_TYPE88(0, 11),
  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t dfs_bind_client_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, data_port) - QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, data_port_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, data_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, ip_preference) - QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, ip_preference_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, ip_preference),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, ep_id) - QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, ep_id_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, ep_id),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, mux_id) - QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, mux_id_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, mux_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, bind_subs) - QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, bind_subs_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dfs_bind_client_req_msg_v01, bind_subs)
};

static const uint8_t dfs_bind_client_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_bind_client_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * dfs_get_client_binding_req_msg is empty
 * static const uint8_t dfs_get_client_binding_req_msg_data_v01[] = {
 * };
 */

static const uint8_t dfs_get_client_binding_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, data_port) - QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, data_port_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, data_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, ip_preference) - QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, ip_preference_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, ip_preference),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, bound_ep_id) - QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, bound_ep_id_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, bound_ep_id),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, bound_mux_id) - QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, bound_mux_id_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, bound_mux_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, bound_subs) - QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, bound_subs_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dfs_get_client_binding_resp_msg_v01, bound_subs)
};

static const uint8_t dfs_add_pdn_sharing_filters_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dfs_add_pdn_sharing_filters_req_msg_v01, filter_rules) - QMI_IDL_OFFSET8(dfs_add_pdn_sharing_filters_req_msg_v01, filter_rules_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_add_pdn_sharing_filters_req_msg_v01, filter_rules),
  QMI_DFS_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(dfs_add_pdn_sharing_filters_req_msg_v01, filter_rules) - QMI_IDL_OFFSET8(dfs_add_pdn_sharing_filters_req_msg_v01, filter_rules_len),
  QMI_IDL_TYPE88(0, 12)
};

static const uint8_t dfs_add_pdn_sharing_filters_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_add_pdn_sharing_filters_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dfs_add_pdn_sharing_filters_resp_msg_v01, filter_handles) - QMI_IDL_OFFSET8(dfs_add_pdn_sharing_filters_resp_msg_v01, filter_handles_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dfs_add_pdn_sharing_filters_resp_msg_v01, filter_handles),
  QMI_DFS_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(dfs_add_pdn_sharing_filters_resp_msg_v01, filter_handles) - QMI_IDL_OFFSET8(dfs_add_pdn_sharing_filters_resp_msg_v01, filter_handles_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dfs_add_pdn_sharing_filters_resp_msg_v01, filter_rule_error) - QMI_IDL_OFFSET16RELATIVE(dfs_add_pdn_sharing_filters_resp_msg_v01, filter_rule_error_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(dfs_add_pdn_sharing_filters_resp_msg_v01, filter_rule_error),
  QMI_DFS_MAX_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(dfs_add_pdn_sharing_filters_resp_msg_v01, filter_rule_error) - QMI_IDL_OFFSET16RELATIVE(dfs_add_pdn_sharing_filters_resp_msg_v01, filter_rule_error_len)
};

static const uint8_t dfs_remove_filters_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dfs_remove_filters_req_msg_v01, filter_handles),
  QMI_DFS_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(dfs_remove_filters_req_msg_v01, filter_handles) - QMI_IDL_OFFSET8(dfs_remove_filters_req_msg_v01, filter_handles_len)
};

static const uint8_t dfs_remove_filters_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_remove_filters_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dfs_remove_mo_exceptional_filters_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dfs_remove_mo_exceptional_filters_req_msg_v01, filter_handles),
  QMI_DFS_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(dfs_remove_mo_exceptional_filters_req_msg_v01, filter_handles) - QMI_IDL_OFFSET8(dfs_remove_mo_exceptional_filters_req_msg_v01, filter_handles_len)
};

static const uint8_t dfs_remove_mo_exceptional_filters_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_remove_mo_exceptional_filters_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dfs_add_mo_exceptional_filters_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dfs_add_mo_exceptional_filters_req_msg_v01, filter_rules) - QMI_IDL_OFFSET8(dfs_add_mo_exceptional_filters_req_msg_v01, filter_rules_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_add_mo_exceptional_filters_req_msg_v01, filter_rules),
  QMI_DFS_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(dfs_add_mo_exceptional_filters_req_msg_v01, filter_rules) - QMI_IDL_OFFSET8(dfs_add_mo_exceptional_filters_req_msg_v01, filter_rules_len),
  QMI_IDL_TYPE88(0, 12)
};

static const uint8_t dfs_add_mo_exceptional_filters_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dfs_add_mo_exceptional_filters_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dfs_add_mo_exceptional_filters_resp_msg_v01, filter_handles) - QMI_IDL_OFFSET8(dfs_add_mo_exceptional_filters_resp_msg_v01, filter_handles_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dfs_add_mo_exceptional_filters_resp_msg_v01, filter_handles),
  QMI_DFS_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(dfs_add_mo_exceptional_filters_resp_msg_v01, filter_handles) - QMI_IDL_OFFSET8(dfs_add_mo_exceptional_filters_resp_msg_v01, filter_handles_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dfs_add_mo_exceptional_filters_resp_msg_v01, filter_rule_error) - QMI_IDL_OFFSET16RELATIVE(dfs_add_mo_exceptional_filters_resp_msg_v01, filter_rule_error_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(dfs_add_mo_exceptional_filters_resp_msg_v01, filter_rule_error),
  QMI_DFS_MAX_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(dfs_add_mo_exceptional_filters_resp_msg_v01, filter_rule_error) - QMI_IDL_OFFSET16RELATIVE(dfs_add_mo_exceptional_filters_resp_msg_v01, filter_rule_error_len)
};

/* Type Table */
static const qmi_idl_type_table_entry  dfs_type_table_v01[] = {
  {sizeof(dfs_ipv4_addr_type_v01), dfs_ipv4_addr_type_data_v01},
  {sizeof(dfs_ipv4_tos_type_v01), dfs_ipv4_tos_type_data_v01},
  {sizeof(dfs_ipv4_info_type_v01), dfs_ipv4_info_type_data_v01},
  {sizeof(dfs_ipv6_trf_cls_type_v01), dfs_ipv6_trf_cls_type_data_v01},
  {sizeof(dfs_ipv6_addr_type_v01), dfs_ipv6_addr_type_data_v01},
  {sizeof(dfs_ipv6_info_type_v01), dfs_ipv6_info_type_data_v01},
  {sizeof(dfs_ip_header_type_v01), dfs_ip_header_type_data_v01},
  {sizeof(dfs_port_type_v01), dfs_port_type_data_v01},
  {sizeof(dfs_port_info_type_v01), dfs_port_info_type_data_v01},
  {sizeof(dfs_icmp_info_type_v01), dfs_icmp_info_type_data_v01},
  {sizeof(dfs_ipsec_info_type_v01), dfs_ipsec_info_type_data_v01},
  {sizeof(dfs_xport_header_type_v01), dfs_xport_header_type_data_v01},
  {sizeof(dfs_filter_rule_type_v01), dfs_filter_rule_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry dfs_message_table_v01[] = {
  {sizeof(dfs_bind_client_req_msg_v01), dfs_bind_client_req_msg_data_v01},
  {sizeof(dfs_bind_client_resp_msg_v01), dfs_bind_client_resp_msg_data_v01},
  {sizeof(dfs_get_client_binding_req_msg_v01), 0},
  {sizeof(dfs_get_client_binding_resp_msg_v01), dfs_get_client_binding_resp_msg_data_v01},
  {sizeof(dfs_add_pdn_sharing_filters_req_msg_v01), dfs_add_pdn_sharing_filters_req_msg_data_v01},
  {sizeof(dfs_add_pdn_sharing_filters_resp_msg_v01), dfs_add_pdn_sharing_filters_resp_msg_data_v01},
  {sizeof(dfs_remove_filters_req_msg_v01), dfs_remove_filters_req_msg_data_v01},
  {sizeof(dfs_remove_filters_resp_msg_v01), dfs_remove_filters_resp_msg_data_v01},
  {sizeof(dfs_remove_mo_exceptional_filters_req_msg_v01), dfs_remove_mo_exceptional_filters_req_msg_data_v01},
  {sizeof(dfs_remove_mo_exceptional_filters_resp_msg_v01), dfs_remove_mo_exceptional_filters_resp_msg_data_v01},
  {sizeof(dfs_add_mo_exceptional_filters_req_msg_v01), dfs_add_mo_exceptional_filters_req_msg_data_v01},
  {sizeof(dfs_add_mo_exceptional_filters_resp_msg_v01), dfs_add_mo_exceptional_filters_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object dfs_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *dfs_qmi_idl_type_table_object_referenced_tables_v01[] =
{&dfs_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01, &data_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object dfs_qmi_idl_type_table_object_v01 = {
  sizeof(dfs_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(dfs_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  dfs_type_table_v01,
  dfs_message_table_v01,
  dfs_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry dfs_service_command_messages_v01[] = {
  {QMI_DFS_BIND_CLIENT_REQ_V01, QMI_IDL_TYPE16(0, 0), 31},
  {QMI_DFS_GET_CLIENT_BINDING_REQ_V01, QMI_IDL_TYPE16(0, 2), 0},
  {QMI_DFS_ADD_PDN_SHARING_FILTERS_REQ_V01, QMI_IDL_TYPE16(0, 4), 36979},
  {QMI_DFS_REMOVE_FILTERS_REQ_V01, QMI_IDL_TYPE16(0, 6), 1024},
  {QMI_DFS_REMOVE_MO_EXCEPTIONAL_FILTERS_REQ_V01, QMI_IDL_TYPE16(0, 8), 1024},
  {QMI_DFS_ADD_MO_EXCEPTIONAL_FILTERS_REQ_V01, QMI_IDL_TYPE16(0, 10), 36979}
};

static const qmi_idl_service_message_table_entry dfs_service_response_messages_v01[] = {
  {QMI_DFS_BIND_CLIENT_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_DFS_GET_CLIENT_BINDING_RESP_V01, QMI_IDL_TYPE16(0, 3), 38},
  {QMI_DFS_ADD_PDN_SHARING_FILTERS_RESP_V01, QMI_IDL_TYPE16(0, 5), 3075},
  {QMI_DFS_REMOVE_FILTERS_RESP_V01, QMI_IDL_TYPE16(0, 7), 7},
  {QMI_DFS_REMOVE_MO_EXCEPTIONAL_FILTERS_RESP_V01, QMI_IDL_TYPE16(0, 9), 7},
  {QMI_DFS_ADD_MO_EXCEPTIONAL_FILTERS_RESP_V01, QMI_IDL_TYPE16(0, 11), 3075}
};

/*Service Object*/
struct qmi_idl_service_object dfs_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x30,
  38010,
  { sizeof(dfs_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(dfs_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    0 },
  { dfs_service_command_messages_v01, dfs_service_response_messages_v01, NULL},
  &dfs_qmi_idl_type_table_object_v01,
  0x0B,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type dfs_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( DFS_V01_IDL_MAJOR_VERS != idl_maj_version || DFS_V01_IDL_MINOR_VERS != idl_min_version
       || DFS_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&dfs_qmi_idl_service_object_v01;
}

