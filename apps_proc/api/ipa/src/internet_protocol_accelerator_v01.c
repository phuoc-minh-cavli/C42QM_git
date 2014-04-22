/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        I N T E R N E T _ P R O T O C O L _ A C C E L E R A T O R _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the ipa service Data structures.

  Copyright (c) 2013-2021 Qualcomm Technologies, Inc. All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header: //components/rel/qmimsgs.tx/1.0/ipa/src/internet_protocol_accelerator_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Thu Apr 29 2021 (Spin 0)
   From IDL File: internet_protocol_accelerator_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "internet_protocol_accelerator_v01.h"
#include "common_v01.h"
#include "data_common_v01.h"


/*Type Definitions*/
static const uint8_t ipa_hdr_tbl_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_hdr_tbl_info_type_v01, modem_offset_start),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_hdr_tbl_info_type_v01, modem_offset_end),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_hdr_proc_ctx_tbl_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_hdr_proc_ctx_tbl_info_type_v01, modem_offset_start),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_hdr_proc_ctx_tbl_info_type_v01, modem_offset_end),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_zip_tbl_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_zip_tbl_info_type_v01, modem_offset_start),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_zip_tbl_info_type_v01, modem_offset_end),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_route_tbl_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_route_tbl_info_type_v01, route_tbl_start_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_route_tbl_info_type_v01, num_indices),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_modem_mem_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_modem_mem_info_type_v01, block_start_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_modem_mem_info_type_v01, size),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_ipfltr_range_eq_16_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_range_eq_16_type_v01, offset),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_range_eq_16_type_v01, range_low),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_range_eq_16_type_v01, range_high),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_ipfltr_mask_eq_32_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_mask_eq_32_type_v01, offset),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_mask_eq_32_type_v01, mask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_mask_eq_32_type_v01, value),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_ipfltr_eq_16_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_eq_16_type_v01, offset),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_eq_16_type_v01, value),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_ipfltr_eq_32_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_eq_32_type_v01, offset),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_eq_32_type_v01, value),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_ipfltr_mask_eq_128_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_mask_eq_128_type_v01, offset),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_mask_eq_128_type_v01, mask),
  16,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_mask_eq_128_type_v01, value),
  16,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_filter_rule_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, rule_eq_bitmap),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, tos_eq_present),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, tos_eq),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, protocol_eq_present),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, protocol_eq),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, num_ihl_offset_range_16),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, ihl_offset_range_16),
  QMI_IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS_V01,
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, num_offset_meq_32),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, offset_meq_32),
  QMI_IPA_IPFLTR_NUM_MEQ_32_EQNS_V01,
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, tc_eq_present),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, tc_eq),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, flow_eq_present),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, flow_eq),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, ihl_offset_eq_16_present),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, ihl_offset_eq_16),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, ihl_offset_eq_32_present),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, ihl_offset_eq_32),
  QMI_IDL_TYPE88(0, 8),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, num_ihl_offset_meq_32),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, ihl_offset_meq_32),
  QMI_IPA_IPFLTR_NUM_IHL_MEQ_32_EQNS_V01,
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, num_offset_meq_128),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, offset_meq_128),
  QMI_IPA_IPFLTR_NUM_MEQ_128_EQNS_V01,
  QMI_IDL_TYPE88(0, 9),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, metadata_meq32_present),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, metadata_meq32),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, ipv4_frag_eq_present),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_filter_spec_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_type_v01, filter_spec_identifier),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_type_v01, ip_type),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_spec_type_v01, filter_rule),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_type_v01, filter_action),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_type_v01, is_routing_table_index_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_type_v01, route_table_index),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_filter_spec_type_v01, is_mux_id_valid),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_filter_spec_type_v01, mux_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_filter_spec_ex_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_ex_type_v01, ip_type),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_spec_ex_type_v01, filter_rule),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_ex_type_v01, filter_action),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_ex_type_v01, is_routing_table_index_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_ex_type_v01, route_table_index),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_ex_type_v01, is_mux_id_valid),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_filter_spec_ex_type_v01, mux_id),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_filter_spec_ex_type_v01, rule_id),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_filter_spec_ex_type_v01, is_rule_hashable),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_filter_rule_req2_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, rule_eq_bitmap),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, pure_ack_eq_present),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, pure_ack_eq),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, protocol_eq_present),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, protocol_eq),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, num_ihl_offset_range_16),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, ihl_offset_range_16),
  QMI_IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS_V01,
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, num_offset_meq_32),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, offset_meq_32),
  QMI_IPA_IPFLTR_NUM_MEQ_32_EQNS_V01,
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, tc_eq_present),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, tc_eq),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, flow_eq_present),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, flow_eq),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, ihl_offset_eq_16_present),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, ihl_offset_eq_16),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, ihl_offset_eq_32_present),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, ihl_offset_eq_32),
  QMI_IDL_TYPE88(0, 8),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, num_ihl_offset_meq_32),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, ihl_offset_meq_32),
  QMI_IPA_IPFLTR_NUM_IHL_MEQ_32_EQNS_V01,
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, num_offset_meq_128),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, offset_meq_128),
  QMI_IPA_IPFLTR_NUM_MEQ_128_EQNS_V01,
  QMI_IDL_TYPE88(0, 9),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, metadata_meq32_present),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, metadata_meq32),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_req2_type_v01, ipv4_frag_eq_present),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_filter_spec_ex2_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_ex2_type_v01, ip_type),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_spec_ex2_type_v01, filter_rule),
  QMI_IDL_TYPE88(0, 13),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_ex2_type_v01, filter_action),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_ex2_type_v01, is_routing_table_index_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_ex2_type_v01, route_table_index),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_ex2_type_v01, is_mux_id_valid),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_filter_spec_ex2_type_v01, mux_id),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_filter_spec_ex2_type_v01, rule_id),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_filter_spec_ex2_type_v01, is_rule_hashable),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_filter_rule_identifier_to_handle_map_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_identifier_to_handle_map_v01, filter_spec_identifier),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_identifier_to_handle_map_v01, filter_handle),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_filter_handle_to_index_map_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_handle_to_index_map_v01, filter_handle),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_handle_to_index_map_v01, filter_index),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_pipe_stats_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_pipe_stats_info_type_v01, pipe_index),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_pipe_stats_info_type_v01, num_ipv4_packets),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_pipe_stats_info_type_v01, num_ipv4_bytes),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_pipe_stats_info_type_v01, num_ipv6_packets),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_pipe_stats_info_type_v01, num_ipv6_bytes),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_stats_type_filter_rule_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_stats_type_filter_rule_v01, filter_rule_index),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_stats_type_filter_rule_v01, num_packets),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_apn_data_stats_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_apn_data_stats_info_type_v01, mux_id),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_apn_data_stats_info_type_v01, num_ul_packets),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_apn_data_stats_info_type_v01, num_ul_bytes),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_apn_data_stats_info_type_v01, num_dl_packets),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_apn_data_stats_info_type_v01, num_dl_bytes),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_data_usage_quota_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_data_usage_quota_info_type_v01, mux_id),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_data_usage_quota_info_type_v01, num_Mbytes),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_per_client_stats_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_per_client_stats_info_type_v01, client_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_per_client_stats_info_type_v01, src_pipe_id),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_per_client_stats_info_type_v01, num_ul_ipv4_bytes),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_per_client_stats_info_type_v01, num_ul_ipv6_bytes),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_per_client_stats_info_type_v01, num_dl_ipv4_bytes),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_per_client_stats_info_type_v01, num_dl_ipv6_bytes),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_per_client_stats_info_type_v01, num_ul_ipv4_pkts),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_per_client_stats_info_type_v01, num_ul_ipv6_pkts),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_per_client_stats_info_type_v01, num_dl_ipv4_pkts),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_per_client_stats_info_type_v01, num_dl_ipv6_pkts),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_ul_firewall_rule_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_ul_firewall_rule_type_v01, ip_type),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_ul_firewall_rule_type_v01, filter_rule),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_ul_firewall_config_result_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_ul_firewall_config_result_type_v01, is_success),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_ul_firewall_config_result_type_v01, mux_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_mhi_ch_init_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_ch_init_info_type_v01, ch_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_ch_init_info_type_v01, er_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_ch_init_info_type_v01, ch_doorbell_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_ch_init_info_type_v01, er_doorbell_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_ch_init_info_type_v01, direction_type),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_mhi_smmu_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_smmu_info_type_v01, iova_ctl_base_addr),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_smmu_info_type_v01, iova_ctl_size),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_smmu_info_type_v01, iova_data_base_addr),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_smmu_info_type_v01, iova_data_size),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_mhi_mem_addr_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_mem_addr_info_type_v01, pa),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_mem_addr_info_type_v01, iova),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_mem_addr_info_type_v01, size),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_mhi_tr_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_tr_info_type_v01, ch_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_tr_info_type_v01, poll_cfg),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_tr_info_type_v01, brst_mode_type),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_tr_info_type_v01, ring_iova),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_tr_info_type_v01, ring_len),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_tr_info_type_v01, rp),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_tr_info_type_v01, wp),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_mhi_er_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_er_info_type_v01, er_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_er_info_type_v01, intmod_cycles),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_er_info_type_v01, intmod_count),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_er_info_type_v01, msi_addr),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_er_info_type_v01, ring_iova),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_er_info_type_v01, ring_len),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_er_info_type_v01, rp),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_er_info_type_v01, wp),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_mhi_ch_alloc_resp_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_ch_alloc_resp_type_v01, ch_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_ch_alloc_resp_type_v01, is_success),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_ep_id_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_ep_id_type_v01, ic_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_ep_id_type_v01, ep_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_ep_id_type_v01, ep_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_ep_id_type_v01, ep_status),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_mhi_prime_aggr_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_prime_aggr_info_type_v01, ic_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_prime_aggr_info_type_v01, ep_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_prime_aggr_info_type_v01, bytes_count),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_prime_aggr_info_type_v01, pkt_count),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_prime_aggr_info_type_v01, aggr_type),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_filter_stats_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_stats_info_type_v01, hw_filter_stats_start_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_stats_info_type_v01, hw_filter_stats_size),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_stats_info_type_v01, hw_filter_stats_start_index),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_stats_info_type_v01, hw_filter_stats_end_index),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t ipa_indication_reg_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, master_driver_init_complete) - QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, master_driver_init_complete_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, master_driver_init_complete),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, data_usage_quota_reached) - QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, data_usage_quota_reached_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, data_usage_quota_reached),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, ipa_mhi_ready_ind) - QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, ipa_mhi_ready_ind_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, ipa_mhi_ready_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, endpoint_desc_ind) - QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, endpoint_desc_ind_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, endpoint_desc_ind),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, bw_change_ind) - QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, bw_change_ind_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, bw_change_ind)
};

static const uint8_t ipa_indication_reg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_indication_reg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_init_modem_driver_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, platform_type) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, platform_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, platform_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hdr_tbl_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hdr_tbl_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hdr_tbl_info),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_route_tbl_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_route_tbl_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_route_tbl_info),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_route_tbl_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_route_tbl_info_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_route_tbl_info),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_filter_tbl_start_addr) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_filter_tbl_start_addr_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_filter_tbl_start_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_filter_tbl_start_addr) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_filter_tbl_start_addr_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_filter_tbl_start_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, modem_mem_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, modem_mem_info_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, modem_mem_info),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, ctrl_comm_dest_end_pt) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, ctrl_comm_dest_end_pt_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, ctrl_comm_dest_end_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, is_ssr_bootup) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, is_ssr_bootup_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, is_ssr_bootup),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hdr_proc_ctx_tbl_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hdr_proc_ctx_tbl_info_valid)),
  0x19,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hdr_proc_ctx_tbl_info),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, zip_tbl_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, zip_tbl_info_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, zip_tbl_info),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_hash_route_tbl_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_hash_route_tbl_info_valid)),
  0x1B,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_hash_route_tbl_info),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_hash_route_tbl_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_hash_route_tbl_info_valid)),
  0x1C,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_hash_route_tbl_info),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_hash_filter_tbl_start_addr) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_hash_filter_tbl_start_addr_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_hash_filter_tbl_start_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_hash_filter_tbl_start_addr) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_hash_filter_tbl_start_addr_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_hash_filter_tbl_start_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hw_stats_quota_base_addr) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hw_stats_quota_base_addr_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hw_stats_quota_base_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hw_stats_quota_size) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hw_stats_quota_size_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hw_stats_quota_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hw_drop_stats_base_addr) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hw_drop_stats_base_addr_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hw_drop_stats_base_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hw_drop_stats_table_size) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hw_drop_stats_table_size_valid)),
  0x22,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hw_drop_stats_table_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, filter_stats_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, filter_stats_info_valid)),
  0x23,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, filter_stats_info),
  QMI_IDL_TYPE88(0, 32),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, smem_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, smem_info_valid)),
  0x24,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, smem_info),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_init_modem_driver_req_msg_v01, per_stats_smem_info) - QMI_IDL_OFFSET16RELATIVE(ipa_init_modem_driver_req_msg_v01, per_stats_smem_info_valid)),
  0x25,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_init_modem_driver_req_msg_v01, per_stats_smem_info),
  QMI_IDL_TYPE88(0, 4)
};

static const uint8_t ipa_init_modem_driver_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, ctrl_comm_dest_end_pt) - QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, ctrl_comm_dest_end_pt_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, ctrl_comm_dest_end_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, default_end_pt) - QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, default_end_pt_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, default_end_pt),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, modem_driver_init_pending) - QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, modem_driver_init_pending_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, modem_driver_init_pending)
};

static const uint8_t ipa_init_modem_driver_cmplt_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_cmplt_req_msg_v01, status)
};

static const uint8_t ipa_init_modem_driver_cmplt_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_cmplt_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_master_driver_init_complt_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_master_driver_init_complt_ind_msg_v01, master_driver_init_status),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_install_fltr_rule_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_msg_v01, filter_spec_list) - QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_msg_v01, filter_spec_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_msg_v01, filter_spec_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_msg_v01, filter_spec_list) - QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_msg_v01, filter_spec_list_len),
  QMI_IDL_TYPE88(0, 11),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, source_pipe_index) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, source_pipe_index_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_msg_v01, source_pipe_index),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, num_ipv4_filters) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, num_ipv4_filters_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_msg_v01, num_ipv4_filters),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, num_ipv6_filters) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, num_ipv6_filters_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_msg_v01, num_ipv6_filters),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, xlat_filter_indices_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, xlat_filter_indices_list_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_msg_v01, xlat_filter_indices_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, xlat_filter_indices_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, xlat_filter_indices_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, filter_spec_ex_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, filter_spec_ex_list_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_msg_v01, filter_spec_ex_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, filter_spec_ex_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, filter_spec_ex_list_len),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, filter_spec_ex2_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, filter_spec_ex2_list_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_msg_v01, filter_spec_ex2_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, filter_spec_ex2_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, filter_spec_ex2_list_len),
  QMI_IDL_TYPE88(0, 14),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, ul_firewall_indices_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, ul_firewall_indices_list_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_msg_v01, ul_firewall_indices_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, ul_firewall_indices_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, ul_firewall_indices_list_len)
};

static const uint8_t ipa_install_fltr_rule_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_msg_v01, filter_handle_list) - QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_msg_v01, filter_handle_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_msg_v01, filter_handle_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_msg_v01, filter_handle_list) - QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_msg_v01, filter_handle_list_len),
  QMI_IDL_TYPE88(0, 15),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_resp_msg_v01, rule_id) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_resp_msg_v01, rule_id_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_resp_msg_v01, rule_id),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_resp_msg_v01, rule_id) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_resp_msg_v01, rule_id_len)
};

static const uint8_t ipa_fltr_installed_notif_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_fltr_installed_notif_req_msg_v01, source_pipe_index),

  0x02,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(ipa_fltr_installed_notif_req_msg_v01, install_status),

  0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_fltr_installed_notif_req_msg_v01, filter_index_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(ipa_fltr_installed_notif_req_msg_v01, filter_index_list) - QMI_IDL_OFFSET8(ipa_fltr_installed_notif_req_msg_v01, filter_index_list_len),
  QMI_IDL_TYPE88(0, 16),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, embedded_pipe_index) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, embedded_pipe_index_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, embedded_pipe_index),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, retain_header) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, retain_header_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, retain_header),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, embedded_call_mux_id) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, embedded_call_mux_id_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, embedded_call_mux_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, num_ipv4_filters) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, num_ipv4_filters_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, num_ipv4_filters),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, num_ipv6_filters) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, num_ipv6_filters_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, num_ipv6_filters),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, start_ipv4_filter_idx) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, start_ipv4_filter_idx_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, start_ipv4_filter_idx),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, start_ipv6_filter_idx) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, start_ipv6_filter_idx_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, start_ipv6_filter_idx),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, rule_id) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, rule_id_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, rule_id),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, rule_id) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, rule_id_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, dst_pipe_id) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, dst_pipe_id_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, dst_pipe_id),
  QMI_IPA_MAX_CLIENT_DST_PIPES_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, dst_pipe_id) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, dst_pipe_id_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, rule_id_ex) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, rule_id_ex_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, rule_id_ex),
  ((QMI_IPA_MAX_FILTERS_EX2_V01) & 0xFF), ((QMI_IPA_MAX_FILTERS_EX2_V01) >> 8),
  QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, rule_id_ex) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, rule_id_ex_len)
};

static const uint8_t ipa_fltr_installed_notif_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_fltr_installed_notif_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_enable_force_clear_datapath_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_req_msg_v01, source_pipe_bitmask),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_req_msg_v01, request_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_req_msg_v01, throttle_source) - QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_req_msg_v01, throttle_source_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_req_msg_v01, throttle_source),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_req_msg_v01, source_pipe_bitmask_ext) - QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_req_msg_v01, source_pipe_bitmask_ext_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_req_msg_v01, source_pipe_bitmask_ext),
  4
};

static const uint8_t ipa_enable_force_clear_datapath_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_disable_force_clear_datapath_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_disable_force_clear_datapath_req_msg_v01, request_id)
};

static const uint8_t ipa_disable_force_clear_datapath_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_disable_force_clear_datapath_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, peripheral_type) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, peripheral_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, peripheral_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, hw_deaggr_supported) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, hw_deaggr_supported_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, hw_deaggr_supported),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, max_aggr_frame_size) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, max_aggr_frame_size_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, max_aggr_frame_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ipa_ingress_pipe_mode) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ipa_ingress_pipe_mode_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ipa_ingress_pipe_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, peripheral_speed_info) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, peripheral_speed_info_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, peripheral_speed_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_time_limit) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_time_limit_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_time_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_pkt_limit) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_pkt_limit_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_pkt_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_byte_limit) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_byte_limit_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_byte_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_accumulation_time_limit) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_accumulation_time_limit_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_accumulation_time_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, hw_control_flags) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, hw_control_flags_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, hw_control_flags),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_msi_event_threshold) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_msi_event_threshold_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_msi_event_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_msi_event_threshold) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_msi_event_threshold_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_msi_event_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_fifo_size) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_fifo_size_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_fifo_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_fifo_size) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_fifo_size_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_fifo_size),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_buf_size) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_buf_size_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_buf_size)
};

static const uint8_t ipa_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_disable_link_low_pwr_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_disable_link_low_pwr_state_req_msg_v01, request_id) - QMI_IDL_OFFSET8(ipa_disable_link_low_pwr_state_req_msg_v01, request_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_disable_link_low_pwr_state_req_msg_v01, request_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_disable_link_low_pwr_state_req_msg_v01, link_ep_id) - QMI_IDL_OFFSET8(ipa_disable_link_low_pwr_state_req_msg_v01, link_ep_id_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_disable_link_low_pwr_state_req_msg_v01, link_ep_id),
  QMI_IDL_TYPE88(2, 0)
};

static const uint8_t ipa_disable_link_low_pwr_state_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_disable_link_low_pwr_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_enable_link_low_pwr_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_enable_link_low_pwr_state_req_msg_v01, request_id) - QMI_IDL_OFFSET8(ipa_enable_link_low_pwr_state_req_msg_v01, request_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_enable_link_low_pwr_state_req_msg_v01, request_id)
};

static const uint8_t ipa_enable_link_low_pwr_state_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_enable_link_low_pwr_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_get_data_stats_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_get_data_stats_req_msg_v01, ipa_stats_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_get_data_stats_req_msg_v01, reset_stats) - QMI_IDL_OFFSET8(ipa_get_data_stats_req_msg_v01, reset_stats_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_get_data_stats_req_msg_v01, reset_stats)
};

static const uint8_t ipa_get_data_stats_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_get_data_stats_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_get_data_stats_resp_msg_v01, ipa_stats_type) - QMI_IDL_OFFSET8(ipa_get_data_stats_resp_msg_v01, ipa_stats_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_get_data_stats_resp_msg_v01, ipa_stats_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_get_data_stats_resp_msg_v01, ul_src_pipe_stats_list) - QMI_IDL_OFFSET8(ipa_get_data_stats_resp_msg_v01, ul_src_pipe_stats_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_get_data_stats_resp_msg_v01, ul_src_pipe_stats_list),
  QMI_IPA_MAX_PIPES_V01,
  QMI_IDL_OFFSET8(ipa_get_data_stats_resp_msg_v01, ul_src_pipe_stats_list) - QMI_IDL_OFFSET8(ipa_get_data_stats_resp_msg_v01, ul_src_pipe_stats_list_len),
  QMI_IDL_TYPE88(0, 17),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_get_data_stats_resp_msg_v01, dl_dst_pipe_stats_list) - QMI_IDL_OFFSET16RELATIVE(ipa_get_data_stats_resp_msg_v01, dl_dst_pipe_stats_list_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_get_data_stats_resp_msg_v01, dl_dst_pipe_stats_list),
  QMI_IPA_MAX_PIPES_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_get_data_stats_resp_msg_v01, dl_dst_pipe_stats_list) - QMI_IDL_OFFSET16RELATIVE(ipa_get_data_stats_resp_msg_v01, dl_dst_pipe_stats_list_len),
  QMI_IDL_TYPE88(0, 17),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_get_data_stats_resp_msg_v01, dl_filter_rule_stats_list) - QMI_IDL_OFFSET16RELATIVE(ipa_get_data_stats_resp_msg_v01, dl_filter_rule_stats_list_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_get_data_stats_resp_msg_v01, dl_filter_rule_stats_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_get_data_stats_resp_msg_v01, dl_filter_rule_stats_list) - QMI_IDL_OFFSET16RELATIVE(ipa_get_data_stats_resp_msg_v01, dl_filter_rule_stats_list_len),
  QMI_IDL_TYPE88(0, 18)
};

static const uint8_t ipa_get_apn_data_stats_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_get_apn_data_stats_req_msg_v01, mux_id_list) - QMI_IDL_OFFSET8(ipa_get_apn_data_stats_req_msg_v01, mux_id_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_get_apn_data_stats_req_msg_v01, mux_id_list),
  QMI_IPA_MAX_APN_V01,
  QMI_IDL_OFFSET8(ipa_get_apn_data_stats_req_msg_v01, mux_id_list) - QMI_IDL_OFFSET8(ipa_get_apn_data_stats_req_msg_v01, mux_id_list_len)
};

static const uint8_t ipa_get_apn_data_stats_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_get_apn_data_stats_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_get_apn_data_stats_resp_msg_v01, apn_data_stats_list) - QMI_IDL_OFFSET8(ipa_get_apn_data_stats_resp_msg_v01, apn_data_stats_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_get_apn_data_stats_resp_msg_v01, apn_data_stats_list),
  QMI_IPA_MAX_APN_V01,
  QMI_IDL_OFFSET8(ipa_get_apn_data_stats_resp_msg_v01, apn_data_stats_list) - QMI_IDL_OFFSET8(ipa_get_apn_data_stats_resp_msg_v01, apn_data_stats_list_len),
  QMI_IDL_TYPE88(0, 19)
};

static const uint8_t ipa_set_data_usage_quota_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_set_data_usage_quota_req_msg_v01, apn_quota_list) - QMI_IDL_OFFSET8(ipa_set_data_usage_quota_req_msg_v01, apn_quota_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_set_data_usage_quota_req_msg_v01, apn_quota_list),
  QMI_IPA_MAX_APN_V01,
  QMI_IDL_OFFSET8(ipa_set_data_usage_quota_req_msg_v01, apn_quota_list) - QMI_IDL_OFFSET8(ipa_set_data_usage_quota_req_msg_v01, apn_quota_list_len),
  QMI_IDL_TYPE88(0, 20),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_set_data_usage_quota_req_msg_v01, apn_warning_list) - QMI_IDL_OFFSET8(ipa_set_data_usage_quota_req_msg_v01, apn_warning_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_set_data_usage_quota_req_msg_v01, apn_warning_list),
  QMI_IPA_MAX_APN_V01,
  QMI_IDL_OFFSET8(ipa_set_data_usage_quota_req_msg_v01, apn_warning_list) - QMI_IDL_OFFSET8(ipa_set_data_usage_quota_req_msg_v01, apn_warning_list_len),
  QMI_IDL_TYPE88(0, 20)
};

static const uint8_t ipa_set_data_usage_quota_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_set_data_usage_quota_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_data_usage_quota_reached_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_data_usage_quota_reached_ind_msg_v01, apn),
  QMI_IDL_TYPE88(0, 20),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_data_usage_quota_reached_ind_msg_v01, is_warning_limit) - QMI_IDL_OFFSET8(ipa_data_usage_quota_reached_ind_msg_v01, is_warning_limit_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_data_usage_quota_reached_ind_msg_v01, is_warning_limit)
};

static const uint8_t ipa_stop_data_usage_quota_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_stop_data_usage_quota_req_msg_v01, is_quota_limit) - QMI_IDL_OFFSET8(ipa_stop_data_usage_quota_req_msg_v01, is_quota_limit_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_stop_data_usage_quota_req_msg_v01, is_quota_limit),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_stop_data_usage_quota_req_msg_v01, is_warning_limit) - QMI_IDL_OFFSET8(ipa_stop_data_usage_quota_req_msg_v01, is_warning_limit_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_stop_data_usage_quota_req_msg_v01, is_warning_limit)
};

static const uint8_t ipa_stop_data_usage_quota_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_stop_data_usage_quota_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_set_dpl_peripheral_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_set_dpl_peripheral_req_msg_v01, peripheral_type)
};

static const uint8_t ipa_set_dpl_peripheral_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_set_dpl_peripheral_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_set_dpl_peripheral_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_set_dpl_peripheral_ind_msg_v01, peripheral_type)
};

static const uint8_t ipa_install_fltr_rule_req_ex_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_ex_msg_v01, filter_spec_ex_list) - QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_ex_msg_v01, filter_spec_ex_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_ex_msg_v01, filter_spec_ex_list),
  QMI_IPA_MAX_FILTERS_EX_V01,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_ex_msg_v01, filter_spec_ex_list) - QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_ex_msg_v01, filter_spec_ex_list_len),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, source_pipe_index) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, source_pipe_index_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_ex_msg_v01, source_pipe_index),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, num_ipv4_filters) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, num_ipv4_filters_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_ex_msg_v01, num_ipv4_filters),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, num_ipv6_filters) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, num_ipv6_filters_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_ex_msg_v01, num_ipv6_filters),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, xlat_filter_indices_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, xlat_filter_indices_list_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_ex_msg_v01, xlat_filter_indices_list),
  QMI_IPA_MAX_FILTERS_EX_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, xlat_filter_indices_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, xlat_filter_indices_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, filter_spec_ex2_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, filter_spec_ex2_list_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_ex_msg_v01, filter_spec_ex2_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, filter_spec_ex2_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, filter_spec_ex2_list_len),
  QMI_IDL_TYPE88(0, 14),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, ul_firewall_indices_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, ul_firewall_indices_list_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_ex_msg_v01, ul_firewall_indices_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, ul_firewall_indices_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_ex_msg_v01, ul_firewall_indices_list_len)
};

static const uint8_t ipa_install_fltr_rule_resp_ex_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_ex_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_ex_msg_v01, rule_id) - QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_ex_msg_v01, rule_id_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_ex_msg_v01, rule_id),
  QMI_IPA_MAX_FILTERS_EX_V01,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_ex_msg_v01, rule_id) - QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_ex_msg_v01, rule_id_len)
};

static const uint8_t ipa_enable_per_client_stats_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_enable_per_client_stats_req_msg_v01, enable_per_client_stats)
};

static const uint8_t ipa_enable_per_client_stats_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_enable_per_client_stats_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_get_stats_per_client_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_get_stats_per_client_req_msg_v01, client_id),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_get_stats_per_client_req_msg_v01, src_pipe_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_get_stats_per_client_req_msg_v01, reset_stats) - QMI_IDL_OFFSET8(ipa_get_stats_per_client_req_msg_v01, reset_stats_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_get_stats_per_client_req_msg_v01, reset_stats)
};

static const uint8_t ipa_get_stats_per_client_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_get_stats_per_client_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_get_stats_per_client_resp_msg_v01, per_client_stats_list) - QMI_IDL_OFFSET8(ipa_get_stats_per_client_resp_msg_v01, per_client_stats_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_get_stats_per_client_resp_msg_v01, per_client_stats_list),
  QMI_IPA_MAX_PER_CLIENTS_V01,
  QMI_IDL_OFFSET8(ipa_get_stats_per_client_resp_msg_v01, per_client_stats_list) - QMI_IDL_OFFSET8(ipa_get_stats_per_client_resp_msg_v01, per_client_stats_list_len),
  QMI_IDL_TYPE88(0, 21)
};

static const uint8_t ipa_configure_ul_firewall_rules_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_configure_ul_firewall_rules_req_msg_v01, firewall_rules_list),
  QMI_IPA_MAX_UL_FIREWALL_RULES_V01,
  QMI_IDL_OFFSET8(ipa_configure_ul_firewall_rules_req_msg_v01, firewall_rules_list) - QMI_IDL_OFFSET8(ipa_configure_ul_firewall_rules_req_msg_v01, firewall_rules_list_len),
  QMI_IDL_TYPE88(0, 22),

  0x02,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_configure_ul_firewall_rules_req_msg_v01, mux_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_configure_ul_firewall_rules_req_msg_v01, disable) - QMI_IDL_OFFSET16RELATIVE(ipa_configure_ul_firewall_rules_req_msg_v01, disable_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_configure_ul_firewall_rules_req_msg_v01, disable),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_configure_ul_firewall_rules_req_msg_v01, are_blacklist_filters) - QMI_IDL_OFFSET16RELATIVE(ipa_configure_ul_firewall_rules_req_msg_v01, are_blacklist_filters_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_configure_ul_firewall_rules_req_msg_v01, are_blacklist_filters)
};

static const uint8_t ipa_configure_ul_firewall_rules_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_configure_ul_firewall_rules_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_configure_ul_firewall_rules_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_configure_ul_firewall_rules_ind_msg_v01, result),
  QMI_IDL_TYPE88(0, 23)
};

static const uint8_t ipa_mhi_clk_vote_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_clk_vote_req_msg_v01, mhi_vote),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_mhi_clk_vote_req_msg_v01, tput_value) - QMI_IDL_OFFSET8(ipa_mhi_clk_vote_req_msg_v01, tput_value_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_clk_vote_req_msg_v01, tput_value),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_mhi_clk_vote_req_msg_v01, clk_rate) - QMI_IDL_OFFSET8(ipa_mhi_clk_vote_req_msg_v01, clk_rate_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_clk_vote_req_msg_v01, clk_rate)
};

static const uint8_t ipa_mhi_clk_vote_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_mhi_clk_vote_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_mhi_ready_indication_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_mhi_ready_indication_msg_v01, ch_info_arr),
  QMI_IPA_REMOTE_MHI_CHANNELS_NUM_MAX_V01,
  QMI_IDL_OFFSET8(ipa_mhi_ready_indication_msg_v01, ch_info_arr) - QMI_IDL_OFFSET8(ipa_mhi_ready_indication_msg_v01, ch_info_arr_len),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_mhi_ready_indication_msg_v01, smmu_info) - QMI_IDL_OFFSET8(ipa_mhi_ready_indication_msg_v01, smmu_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_mhi_ready_indication_msg_v01, smmu_info),
  QMI_IDL_TYPE88(0, 25)
};

static const uint8_t ipa_mhi_alloc_channel_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_mhi_alloc_channel_req_msg_v01, tr_info_arr),
  QMI_IPA_REMOTE_MHI_CHANNELS_NUM_MAX_V01,
  QMI_IDL_OFFSET8(ipa_mhi_alloc_channel_req_msg_v01, tr_info_arr) - QMI_IDL_OFFSET8(ipa_mhi_alloc_channel_req_msg_v01, tr_info_arr_len),
  QMI_IDL_TYPE88(0, 27),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_mhi_alloc_channel_req_msg_v01, er_info_arr),
  QMI_IPA_REMOTE_MHI_CHANNELS_NUM_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_mhi_alloc_channel_req_msg_v01, er_info_arr) - QMI_IDL_OFFSET16RELATIVE(ipa_mhi_alloc_channel_req_msg_v01, er_info_arr_len),
  QMI_IDL_TYPE88(0, 28),

  0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_mhi_alloc_channel_req_msg_v01, ctrl_addr_map_info),
  QMI_IPA_REMOTE_MHI_MEMORY_MAPPING_NUM_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_mhi_alloc_channel_req_msg_v01, ctrl_addr_map_info) - QMI_IDL_OFFSET16RELATIVE(ipa_mhi_alloc_channel_req_msg_v01, ctrl_addr_map_info_len),
  QMI_IDL_TYPE88(0, 26),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_mhi_alloc_channel_req_msg_v01, data_addr_map_info),
  QMI_IPA_REMOTE_MHI_MEMORY_MAPPING_NUM_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_mhi_alloc_channel_req_msg_v01, data_addr_map_info) - QMI_IDL_OFFSET16RELATIVE(ipa_mhi_alloc_channel_req_msg_v01, data_addr_map_info_len),
  QMI_IDL_TYPE88(0, 26)
};

static const uint8_t ipa_mhi_alloc_channel_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_mhi_alloc_channel_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_mhi_alloc_channel_resp_msg_v01, alloc_resp_arr) - QMI_IDL_OFFSET8(ipa_mhi_alloc_channel_resp_msg_v01, alloc_resp_arr_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_mhi_alloc_channel_resp_msg_v01, alloc_resp_arr),
  QMI_IPA_REMOTE_MHI_CHANNELS_NUM_MAX_V01,
  QMI_IDL_OFFSET8(ipa_mhi_alloc_channel_resp_msg_v01, alloc_resp_arr) - QMI_IDL_OFFSET8(ipa_mhi_alloc_channel_resp_msg_v01, alloc_resp_arr_len),
  QMI_IDL_TYPE88(0, 29)
};

static const uint8_t ipa_mhi_cleanup_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_mhi_cleanup_req_msg_v01, cleanup) - QMI_IDL_OFFSET8(ipa_mhi_cleanup_req_msg_v01, cleanup_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_mhi_cleanup_req_msg_v01, cleanup)
};

static const uint8_t ipa_mhi_cleanup_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_mhi_cleanup_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_endp_desc_indication_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_endp_desc_indication_msg_v01, ep_info) - QMI_IDL_OFFSET8(ipa_endp_desc_indication_msg_v01, ep_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_endp_desc_indication_msg_v01, ep_info),
  QMI_IPA_ENDP_DESC_NUM_MAX_V01,
  QMI_IDL_OFFSET8(ipa_endp_desc_indication_msg_v01, ep_info) - QMI_IDL_OFFSET8(ipa_endp_desc_indication_msg_v01, ep_info_len),
  QMI_IDL_TYPE88(0, 30),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_endp_desc_indication_msg_v01, num_eps) - QMI_IDL_OFFSET16RELATIVE(ipa_endp_desc_indication_msg_v01, num_eps_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_endp_desc_indication_msg_v01, num_eps)
};

static const uint8_t ipa_mhi_prime_aggr_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_mhi_prime_aggr_info_req_msg_v01, aggr_info) - QMI_IDL_OFFSET8(ipa_mhi_prime_aggr_info_req_msg_v01, aggr_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_mhi_prime_aggr_info_req_msg_v01, aggr_info),
  QMI_IPA_ENDP_DESC_NUM_MAX_V01,
  QMI_IDL_OFFSET8(ipa_mhi_prime_aggr_info_req_msg_v01, aggr_info) - QMI_IDL_OFFSET8(ipa_mhi_prime_aggr_info_req_msg_v01, aggr_info_len),
  QMI_IDL_TYPE88(0, 31),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_mhi_prime_aggr_info_req_msg_v01, num_eps) - QMI_IDL_OFFSET16RELATIVE(ipa_mhi_prime_aggr_info_req_msg_v01, num_eps_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_mhi_prime_aggr_info_req_msg_v01, num_eps)
};

static const uint8_t ipa_mhi_prime_aggr_info_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_mhi_prime_aggr_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_add_offload_connection_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_add_offload_connection_req_msg_v01, num_ipv4_filters) - QMI_IDL_OFFSET8(ipa_add_offload_connection_req_msg_v01, num_ipv4_filters_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_add_offload_connection_req_msg_v01, num_ipv4_filters),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_add_offload_connection_req_msg_v01, num_ipv6_filters) - QMI_IDL_OFFSET8(ipa_add_offload_connection_req_msg_v01, num_ipv6_filters_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_add_offload_connection_req_msg_v01, num_ipv6_filters),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_add_offload_connection_req_msg_v01, xlat_filter_indices_list) - QMI_IDL_OFFSET8(ipa_add_offload_connection_req_msg_v01, xlat_filter_indices_list_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_add_offload_connection_req_msg_v01, xlat_filter_indices_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(ipa_add_offload_connection_req_msg_v01, xlat_filter_indices_list) - QMI_IDL_OFFSET8(ipa_add_offload_connection_req_msg_v01, xlat_filter_indices_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_add_offload_connection_req_msg_v01, filter_spec_ex2_list) - QMI_IDL_OFFSET16RELATIVE(ipa_add_offload_connection_req_msg_v01, filter_spec_ex2_list_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_add_offload_connection_req_msg_v01, filter_spec_ex2_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_add_offload_connection_req_msg_v01, filter_spec_ex2_list) - QMI_IDL_OFFSET16RELATIVE(ipa_add_offload_connection_req_msg_v01, filter_spec_ex2_list_len),
  QMI_IDL_TYPE88(0, 14),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_add_offload_connection_req_msg_v01, embedded_call_mux_id) - QMI_IDL_OFFSET16RELATIVE(ipa_add_offload_connection_req_msg_v01, embedded_call_mux_id_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_add_offload_connection_req_msg_v01, embedded_call_mux_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_add_offload_connection_req_msg_v01, default_mhi_path) - QMI_IDL_OFFSET16RELATIVE(ipa_add_offload_connection_req_msg_v01, default_mhi_path_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_add_offload_connection_req_msg_v01, default_mhi_path)
};

static const uint8_t ipa_add_offload_connection_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_add_offload_connection_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_add_offload_connection_resp_msg_v01, filter_handle_list) - QMI_IDL_OFFSET8(ipa_add_offload_connection_resp_msg_v01, filter_handle_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_add_offload_connection_resp_msg_v01, filter_handle_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(ipa_add_offload_connection_resp_msg_v01, filter_handle_list) - QMI_IDL_OFFSET8(ipa_add_offload_connection_resp_msg_v01, filter_handle_list_len),
  QMI_IDL_TYPE88(0, 15)
};

static const uint8_t ipa_remove_offload_connection_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_remove_offload_connection_req_msg_v01, filter_handle_list) - QMI_IDL_OFFSET8(ipa_remove_offload_connection_req_msg_v01, filter_handle_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_remove_offload_connection_req_msg_v01, filter_handle_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(ipa_remove_offload_connection_req_msg_v01, filter_handle_list) - QMI_IDL_OFFSET8(ipa_remove_offload_connection_req_msg_v01, filter_handle_list_len),
  QMI_IDL_TYPE88(0, 15),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_remove_offload_connection_req_msg_v01, clean_all_rules) - QMI_IDL_OFFSET16RELATIVE(ipa_remove_offload_connection_req_msg_v01, clean_all_rules_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_remove_offload_connection_req_msg_v01, clean_all_rules)
};

static const uint8_t ipa_remove_offload_connection_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_remove_offload_connection_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_bw_change_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, peak_bw_ul) - QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, peak_bw_ul_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, peak_bw_ul),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, peak_bw_dl) - QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, peak_bw_dl_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, peak_bw_dl),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, burst_freq) - QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, burst_freq_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, burst_freq),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, avg_bw_ul) - QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, avg_bw_ul_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, avg_bw_ul),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, avg_bw_dl) - QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, avg_bw_dl_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_bw_change_req_msg_v01, avg_bw_dl)
};

static const uint8_t ipa_bw_change_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_bw_change_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_bw_change_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_bw_change_ind_msg_v01, peak_bw_ul) - QMI_IDL_OFFSET8(ipa_bw_change_ind_msg_v01, peak_bw_ul_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_bw_change_ind_msg_v01, peak_bw_ul),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_bw_change_ind_msg_v01, peak_bw_dl) - QMI_IDL_OFFSET8(ipa_bw_change_ind_msg_v01, peak_bw_dl_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_bw_change_ind_msg_v01, peak_bw_dl)
};

static const uint8_t ipa_low_latency_traffic_info_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_low_latency_traffic_info_req_msg_v01, info),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_low_latency_traffic_info_req_msg_v01, endp_bitmap) - QMI_IDL_OFFSET8(ipa_low_latency_traffic_info_req_msg_v01, endp_bitmap_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_low_latency_traffic_info_req_msg_v01, endp_bitmap)
};

static const uint8_t ipa_low_latency_traffic_info_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_low_latency_traffic_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_move_nat_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_move_nat_req_msg_v01, nat_move_direction)
};

static const uint8_t ipa_move_nat_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_move_nat_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_nat_move_table_complt_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_nat_move_table_complt_ind_msg_v01, nat_table_move_status),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
static const qmi_idl_type_table_entry  ipa_type_table_v01[] = {
  {sizeof(ipa_hdr_tbl_info_type_v01), ipa_hdr_tbl_info_type_data_v01},
  {sizeof(ipa_hdr_proc_ctx_tbl_info_type_v01), ipa_hdr_proc_ctx_tbl_info_type_data_v01},
  {sizeof(ipa_zip_tbl_info_type_v01), ipa_zip_tbl_info_type_data_v01},
  {sizeof(ipa_route_tbl_info_type_v01), ipa_route_tbl_info_type_data_v01},
  {sizeof(ipa_modem_mem_info_type_v01), ipa_modem_mem_info_type_data_v01},
  {sizeof(ipa_ipfltr_range_eq_16_type_v01), ipa_ipfltr_range_eq_16_type_data_v01},
  {sizeof(ipa_ipfltr_mask_eq_32_type_v01), ipa_ipfltr_mask_eq_32_type_data_v01},
  {sizeof(ipa_ipfltr_eq_16_type_v01), ipa_ipfltr_eq_16_type_data_v01},
  {sizeof(ipa_ipfltr_eq_32_type_v01), ipa_ipfltr_eq_32_type_data_v01},
  {sizeof(ipa_ipfltr_mask_eq_128_type_v01), ipa_ipfltr_mask_eq_128_type_data_v01},
  {sizeof(ipa_filter_rule_type_v01), ipa_filter_rule_type_data_v01},
  {sizeof(ipa_filter_spec_type_v01), ipa_filter_spec_type_data_v01},
  {sizeof(ipa_filter_spec_ex_type_v01), ipa_filter_spec_ex_type_data_v01},
  {sizeof(ipa_filter_rule_req2_type_v01), ipa_filter_rule_req2_type_data_v01},
  {sizeof(ipa_filter_spec_ex2_type_v01), ipa_filter_spec_ex2_type_data_v01},
  {sizeof(ipa_filter_rule_identifier_to_handle_map_v01), ipa_filter_rule_identifier_to_handle_map_data_v01},
  {sizeof(ipa_filter_handle_to_index_map_v01), ipa_filter_handle_to_index_map_data_v01},
  {sizeof(ipa_pipe_stats_info_type_v01), ipa_pipe_stats_info_type_data_v01},
  {sizeof(ipa_stats_type_filter_rule_v01), ipa_stats_type_filter_rule_data_v01},
  {sizeof(ipa_apn_data_stats_info_type_v01), ipa_apn_data_stats_info_type_data_v01},
  {sizeof(ipa_data_usage_quota_info_type_v01), ipa_data_usage_quota_info_type_data_v01},
  {sizeof(ipa_per_client_stats_info_type_v01), ipa_per_client_stats_info_type_data_v01},
  {sizeof(ipa_ul_firewall_rule_type_v01), ipa_ul_firewall_rule_type_data_v01},
  {sizeof(ipa_ul_firewall_config_result_type_v01), ipa_ul_firewall_config_result_type_data_v01},
  {sizeof(ipa_mhi_ch_init_info_type_v01), ipa_mhi_ch_init_info_type_data_v01},
  {sizeof(ipa_mhi_smmu_info_type_v01), ipa_mhi_smmu_info_type_data_v01},
  {sizeof(ipa_mhi_mem_addr_info_type_v01), ipa_mhi_mem_addr_info_type_data_v01},
  {sizeof(ipa_mhi_tr_info_type_v01), ipa_mhi_tr_info_type_data_v01},
  {sizeof(ipa_mhi_er_info_type_v01), ipa_mhi_er_info_type_data_v01},
  {sizeof(ipa_mhi_ch_alloc_resp_type_v01), ipa_mhi_ch_alloc_resp_type_data_v01},
  {sizeof(ipa_ep_id_type_v01), ipa_ep_id_type_data_v01},
  {sizeof(ipa_mhi_prime_aggr_info_type_v01), ipa_mhi_prime_aggr_info_type_data_v01},
  {sizeof(ipa_filter_stats_info_type_v01), ipa_filter_stats_info_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry ipa_message_table_v01[] = {
  {sizeof(ipa_indication_reg_req_msg_v01), ipa_indication_reg_req_msg_data_v01},
  {sizeof(ipa_indication_reg_resp_msg_v01), ipa_indication_reg_resp_msg_data_v01},
  {sizeof(ipa_init_modem_driver_req_msg_v01), ipa_init_modem_driver_req_msg_data_v01},
  {sizeof(ipa_init_modem_driver_resp_msg_v01), ipa_init_modem_driver_resp_msg_data_v01},
  {sizeof(ipa_init_modem_driver_cmplt_req_msg_v01), ipa_init_modem_driver_cmplt_req_msg_data_v01},
  {sizeof(ipa_init_modem_driver_cmplt_resp_msg_v01), ipa_init_modem_driver_cmplt_resp_msg_data_v01},
  {sizeof(ipa_master_driver_init_complt_ind_msg_v01), ipa_master_driver_init_complt_ind_msg_data_v01},
  {sizeof(ipa_install_fltr_rule_req_msg_v01), ipa_install_fltr_rule_req_msg_data_v01},
  {sizeof(ipa_install_fltr_rule_resp_msg_v01), ipa_install_fltr_rule_resp_msg_data_v01},
  {sizeof(ipa_fltr_installed_notif_req_msg_v01), ipa_fltr_installed_notif_req_msg_data_v01},
  {sizeof(ipa_fltr_installed_notif_resp_msg_v01), ipa_fltr_installed_notif_resp_msg_data_v01},
  {sizeof(ipa_enable_force_clear_datapath_req_msg_v01), ipa_enable_force_clear_datapath_req_msg_data_v01},
  {sizeof(ipa_enable_force_clear_datapath_resp_msg_v01), ipa_enable_force_clear_datapath_resp_msg_data_v01},
  {sizeof(ipa_disable_force_clear_datapath_req_msg_v01), ipa_disable_force_clear_datapath_req_msg_data_v01},
  {sizeof(ipa_disable_force_clear_datapath_resp_msg_v01), ipa_disable_force_clear_datapath_resp_msg_data_v01},
  {sizeof(ipa_config_req_msg_v01), ipa_config_req_msg_data_v01},
  {sizeof(ipa_config_resp_msg_v01), ipa_config_resp_msg_data_v01},
  {sizeof(ipa_disable_link_low_pwr_state_req_msg_v01), ipa_disable_link_low_pwr_state_req_msg_data_v01},
  {sizeof(ipa_disable_link_low_pwr_state_resp_msg_v01), ipa_disable_link_low_pwr_state_resp_msg_data_v01},
  {sizeof(ipa_enable_link_low_pwr_state_req_msg_v01), ipa_enable_link_low_pwr_state_req_msg_data_v01},
  {sizeof(ipa_enable_link_low_pwr_state_resp_msg_v01), ipa_enable_link_low_pwr_state_resp_msg_data_v01},
  {sizeof(ipa_get_data_stats_req_msg_v01), ipa_get_data_stats_req_msg_data_v01},
  {sizeof(ipa_get_data_stats_resp_msg_v01), ipa_get_data_stats_resp_msg_data_v01},
  {sizeof(ipa_get_apn_data_stats_req_msg_v01), ipa_get_apn_data_stats_req_msg_data_v01},
  {sizeof(ipa_get_apn_data_stats_resp_msg_v01), ipa_get_apn_data_stats_resp_msg_data_v01},
  {sizeof(ipa_set_data_usage_quota_req_msg_v01), ipa_set_data_usage_quota_req_msg_data_v01},
  {sizeof(ipa_set_data_usage_quota_resp_msg_v01), ipa_set_data_usage_quota_resp_msg_data_v01},
  {sizeof(ipa_data_usage_quota_reached_ind_msg_v01), ipa_data_usage_quota_reached_ind_msg_data_v01},
  {sizeof(ipa_stop_data_usage_quota_req_msg_v01), ipa_stop_data_usage_quota_req_msg_data_v01},
  {sizeof(ipa_stop_data_usage_quota_resp_msg_v01), ipa_stop_data_usage_quota_resp_msg_data_v01},
  {sizeof(ipa_set_dpl_peripheral_req_msg_v01), ipa_set_dpl_peripheral_req_msg_data_v01},
  {sizeof(ipa_set_dpl_peripheral_resp_msg_v01), ipa_set_dpl_peripheral_resp_msg_data_v01},
  {sizeof(ipa_set_dpl_peripheral_ind_msg_v01), ipa_set_dpl_peripheral_ind_msg_data_v01},
  {sizeof(ipa_install_fltr_rule_req_ex_msg_v01), ipa_install_fltr_rule_req_ex_msg_data_v01},
  {sizeof(ipa_install_fltr_rule_resp_ex_msg_v01), ipa_install_fltr_rule_resp_ex_msg_data_v01},
  {sizeof(ipa_enable_per_client_stats_req_msg_v01), ipa_enable_per_client_stats_req_msg_data_v01},
  {sizeof(ipa_enable_per_client_stats_resp_msg_v01), ipa_enable_per_client_stats_resp_msg_data_v01},
  {sizeof(ipa_get_stats_per_client_req_msg_v01), ipa_get_stats_per_client_req_msg_data_v01},
  {sizeof(ipa_get_stats_per_client_resp_msg_v01), ipa_get_stats_per_client_resp_msg_data_v01},
  {sizeof(ipa_configure_ul_firewall_rules_req_msg_v01), ipa_configure_ul_firewall_rules_req_msg_data_v01},
  {sizeof(ipa_configure_ul_firewall_rules_resp_msg_v01), ipa_configure_ul_firewall_rules_resp_msg_data_v01},
  {sizeof(ipa_configure_ul_firewall_rules_ind_msg_v01), ipa_configure_ul_firewall_rules_ind_msg_data_v01},
  {sizeof(ipa_mhi_clk_vote_req_msg_v01), ipa_mhi_clk_vote_req_msg_data_v01},
  {sizeof(ipa_mhi_clk_vote_resp_msg_v01), ipa_mhi_clk_vote_resp_msg_data_v01},
  {sizeof(ipa_mhi_ready_indication_msg_v01), ipa_mhi_ready_indication_msg_data_v01},
  {sizeof(ipa_mhi_alloc_channel_req_msg_v01), ipa_mhi_alloc_channel_req_msg_data_v01},
  {sizeof(ipa_mhi_alloc_channel_resp_msg_v01), ipa_mhi_alloc_channel_resp_msg_data_v01},
  {sizeof(ipa_mhi_cleanup_req_msg_v01), ipa_mhi_cleanup_req_msg_data_v01},
  {sizeof(ipa_mhi_cleanup_resp_msg_v01), ipa_mhi_cleanup_resp_msg_data_v01},
  {sizeof(ipa_endp_desc_indication_msg_v01), ipa_endp_desc_indication_msg_data_v01},
  {sizeof(ipa_mhi_prime_aggr_info_req_msg_v01), ipa_mhi_prime_aggr_info_req_msg_data_v01},
  {sizeof(ipa_mhi_prime_aggr_info_resp_msg_v01), ipa_mhi_prime_aggr_info_resp_msg_data_v01},
  {sizeof(ipa_add_offload_connection_req_msg_v01), ipa_add_offload_connection_req_msg_data_v01},
  {sizeof(ipa_add_offload_connection_resp_msg_v01), ipa_add_offload_connection_resp_msg_data_v01},
  {sizeof(ipa_remove_offload_connection_req_msg_v01), ipa_remove_offload_connection_req_msg_data_v01},
  {sizeof(ipa_remove_offload_connection_resp_msg_v01), ipa_remove_offload_connection_resp_msg_data_v01},
  {sizeof(ipa_bw_change_req_msg_v01), ipa_bw_change_req_msg_data_v01},
  {sizeof(ipa_bw_change_resp_msg_v01), ipa_bw_change_resp_msg_data_v01},
  {sizeof(ipa_bw_change_ind_msg_v01), ipa_bw_change_ind_msg_data_v01},
  {sizeof(ipa_low_latency_traffic_info_req_msg_v01), ipa_low_latency_traffic_info_req_msg_data_v01},
  {sizeof(ipa_low_latency_traffic_info_resp_msg_v01), ipa_low_latency_traffic_info_resp_msg_data_v01},
  {sizeof(ipa_move_nat_req_msg_v01), ipa_move_nat_req_msg_data_v01},
  {sizeof(ipa_move_nat_resp_msg_v01), ipa_move_nat_resp_msg_data_v01},
  {sizeof(ipa_nat_move_table_complt_ind_msg_v01), ipa_nat_move_table_complt_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object ipa_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *ipa_qmi_idl_type_table_object_referenced_tables_v01[] =
{&ipa_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01, &data_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object ipa_qmi_idl_type_table_object_v01 = {
  sizeof(ipa_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(ipa_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  ipa_type_table_v01,
  ipa_message_table_v01,
  ipa_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry ipa_service_command_messages_v01[] = {
  {QMI_IPA_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 0), 20},
  {QMI_IPA_INIT_MODEM_DRIVER_REQ_V01, QMI_IDL_TYPE16(0, 2), 197},
  {QMI_IPA_INSTALL_FILTER_RULE_REQ_V01, QMI_IDL_TYPE16(0, 7), 33705},
  {QMI_IPA_FILTER_INSTALLED_NOTIF_REQ_V01, QMI_IDL_TYPE16(0, 9), 1899},
  {QMI_IPA_ENABLE_FORCE_CLEAR_DATAPATH_REQ_V01, QMI_IDL_TYPE16(0, 11), 37},
  {QMI_IPA_DISABLE_FORCE_CLEAR_DATAPATH_REQ_V01, QMI_IDL_TYPE16(0, 13), 7},
  {QMI_IPA_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 15), 102},
  {QMI_IPA_DISABLE_LINK_LOW_PWR_STATE_REQ_V01, QMI_IDL_TYPE16(0, 17), 18},
  {QMI_IPA_ENABLE_LINK_LOW_PWR_STATE_REQ_V01, QMI_IDL_TYPE16(0, 19), 7},
  {QMI_IPA_GET_DATA_STATS_REQ_V01, QMI_IDL_TYPE16(0, 21), 11},
  {QMI_IPA_GET_APN_DATA_STATS_REQ_V01, QMI_IDL_TYPE16(0, 23), 36},
  {QMI_IPA_SET_DATA_USAGE_QUOTA_REQ_V01, QMI_IDL_TYPE16(0, 25), 200},
  {QMI_IPA_STOP_DATA_USAGE_QUOTA_REQ_V01, QMI_IDL_TYPE16(0, 28), 8},
  {QMI_IPA_INIT_MODEM_DRIVER_CMPLT_REQ_V01, QMI_IDL_TYPE16(0, 4), 4},
  {QMI_IPA_SET_DPL_PERIPHERAL_REQ_V01, QMI_IDL_TYPE16(0, 30), 7},
  {QMI_IPA_INSTALL_FILTER_RULE_EX_REQ_V01, QMI_IDL_TYPE16(0, 33), 34021},
  {QMI_IPA_ENABLE_PER_CLIENT_STATS_REQ_V01, QMI_IDL_TYPE16(0, 35), 4},
  {QMI_IPA_GET_STATS_PER_CLIENT_REQ_V01, QMI_IDL_TYPE16(0, 37), 18},
  {QMI_IPA_CONFIGURE_UL_FIREWALL_RULES_REQ_V01, QMI_IDL_TYPE16(0, 39), 9875},
  {QMI_IPA_MHI_CLK_VOTE_REQ_V01, QMI_IDL_TYPE16(0, 42), 18},
  {QMI_IPA_MHI_ALLOC_CHANNEL_REQ_V01, QMI_IDL_TYPE16(0, 45), 808},
  {QMI_IPA_MHI_CLEANUP_REQ_V01, QMI_IDL_TYPE16(0, 47), 4},
  {QMI_IPA_MHI_PRIME_AGGR_INFO_REQ_V01, QMI_IDL_TYPE16(0, 50), 631},
  {QMI_IPA_ADD_OFFLOAD_CONNECTION_REQ_V01, QMI_IDL_TYPE16(0, 52), 11361},
  {QMI_IPA_REMOVE_OFFLOAD_CONNECTION_REQ_V01, QMI_IDL_TYPE16(0, 54), 520},
  {QMI_IPA_BW_CHANGE_REQ_V01, QMI_IDL_TYPE16(0, 56), 35},
  {QMI_IPA_LOW_LATENCY_TRAFFIC_INFO_REQ_V01, QMI_IDL_TYPE16(0, 59), 14},
  {QMI_IPA_MOVE_NAT_REQ_V01, QMI_IDL_TYPE16(0, 61), 7}
};

static const qmi_idl_service_message_table_entry ipa_service_response_messages_v01[] = {
  {QMI_IPA_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_IPA_INIT_MODEM_DRIVER_RESP_V01, QMI_IDL_TYPE16(0, 3), 25},
  {QMI_IPA_INSTALL_FILTER_RULE_RESP_V01, QMI_IDL_TYPE16(0, 8), 783},
  {QMI_IPA_FILTER_INSTALLED_NOTIF_RESP_V01, QMI_IDL_TYPE16(0, 10), 7},
  {QMI_IPA_ENABLE_FORCE_CLEAR_DATAPATH_RESP_V01, QMI_IDL_TYPE16(0, 12), 7},
  {QMI_IPA_DISABLE_FORCE_CLEAR_DATAPATH_RESP_V01, QMI_IDL_TYPE16(0, 14), 7},
  {QMI_IPA_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 16), 7},
  {QMI_IPA_DISABLE_LINK_LOW_PWR_STATE_RESP_V01, QMI_IDL_TYPE16(0, 18), 7},
  {QMI_IPA_ENABLE_LINK_LOW_PWR_STATE_RESP_V01, QMI_IDL_TYPE16(0, 20), 7},
  {QMI_IPA_GET_DATA_STATS_RESP_V01, QMI_IDL_TYPE16(0, 22), 2234},
  {QMI_IPA_GET_APN_DATA_STATS_RESP_V01, QMI_IDL_TYPE16(0, 24), 299},
  {QMI_IPA_SET_DATA_USAGE_QUOTA_RESP_V01, QMI_IDL_TYPE16(0, 26), 7},
  {QMI_IPA_STOP_DATA_USAGE_QUOTA_RESP_V01, QMI_IDL_TYPE16(0, 29), 7},
  {QMI_IPA_INIT_MODEM_DRIVER_CMPLT_RESP_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_IPA_SET_DPL_PERIPHERAL_RESP_V01, QMI_IDL_TYPE16(0, 31), 7},
  {QMI_IPA_INSTALL_FILTER_RULE_EX_RESP_V01, QMI_IDL_TYPE16(0, 34), 523},
  {QMI_IPA_ENABLE_PER_CLIENT_STATS_RESP_V01, QMI_IDL_TYPE16(0, 36), 7},
  {QMI_IPA_GET_STATS_PER_CLIENT_RESP_V01, QMI_IDL_TYPE16(0, 38), 3595},
  {QMI_IPA_CONFIGURE_UL_FIREWALL_RULES_RESP_V01, QMI_IDL_TYPE16(0, 40), 7},
  {QMI_IPA_MHI_CLK_VOTE_RESP_V01, QMI_IDL_TYPE16(0, 43), 7},
  {QMI_IPA_MHI_ALLOC_CHANNEL_RESP_V01, QMI_IDL_TYPE16(0, 46), 23},
  {QMI_IPA_MHI_CLEANUP_RESP_V01, QMI_IDL_TYPE16(0, 48), 7},
  {QMI_IPA_MHI_PRIME_AGGR_INFO_RESP_V01, QMI_IDL_TYPE16(0, 51), 7},
  {QMI_IPA_ADD_OFFLOAD_CONNECTION_RESP_V01, QMI_IDL_TYPE16(0, 53), 523},
  {QMI_IPA_REMOVE_OFFLOAD_CONNECTION_RESP_V01, QMI_IDL_TYPE16(0, 55), 7},
  {QMI_IPA_BW_CHANGE_RESP_V01, QMI_IDL_TYPE16(0, 57), 7},
  {QMI_IPA_LOW_LATENCY_TRAFFIC_INFO_RESP_V01, QMI_IDL_TYPE16(0, 60), 7},
  {QMI_IPA_MOVE_NAT_RESP_V01, QMI_IDL_TYPE16(0, 62), 7}
};

static const qmi_idl_service_message_table_entry ipa_service_indication_messages_v01[] = {
  {QMI_IPA_MASTER_DRIVER_INIT_COMPLETE_IND_V01, QMI_IDL_TYPE16(0, 6), 7},
  {QMI_IPA_DATA_USAGE_QUOTA_REACHED_IND_V01, QMI_IDL_TYPE16(0, 27), 19},
  {QMI_IPA_SET_DPL_PERIPHERAL_IND_V01, QMI_IDL_TYPE16(0, 32), 7},
  {QMI_IPA_CONFIGURE_UL_FIREWALL_RULES_IND_V01, QMI_IDL_TYPE16(0, 41), 11},
  {QMI_IPA_MHI_READY_IND_V01, QMI_IDL_TYPE16(0, 44), 123},
  {QMI_IPA_ENDP_DESC_INDICATION_V01, QMI_IDL_TYPE16(0, 49), 507},
  {QMI_IPA_BW_CHANGE_INDICATION_V01, QMI_IDL_TYPE16(0, 58), 14},
  {QMI_IPA_NAT_TABLE_MOVE_COMPLT_IND_V01, QMI_IDL_TYPE16(0, 63), 7}
};

/*Service Object*/
struct qmi_idl_service_object ipa_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x31,
  34021,
  { sizeof(ipa_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ipa_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ipa_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { ipa_service_command_messages_v01, ipa_service_response_messages_v01, ipa_service_indication_messages_v01},
  &ipa_qmi_idl_type_table_object_v01,
  0x1A,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type ipa_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( IPA_V01_IDL_MAJOR_VERS != idl_maj_version || IPA_V01_IDL_MINOR_VERS != idl_min_version
       || IPA_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&ipa_qmi_idl_service_object_v01;
}

