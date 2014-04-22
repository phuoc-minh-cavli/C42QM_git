/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        I M S _ I O T _ Q M I _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the ims_iot service Data structures.

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.tx/1.0/ims_iot/src/ims_iot_qmi_service_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Mon Jan  7 2019 (Spin 0)
   From IDL File: ims_iot_qmi_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "ims_iot_qmi_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t ims_iot_uri_s_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_iot_uri_s_type_v01, ims_uri),
  IMS_IOT_URI_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ims_iot_pdn_failure_reason_s_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_iot_pdn_failure_reason_s_type_v01, pdn_fail_reason_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_iot_pdn_failure_reason_s_type_v01, pdn_fail_reason),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t qmi_ims_iot_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_indication_register_req_msg_v01, config_change_ind) - QMI_IDL_OFFSET8(qmi_ims_iot_indication_register_req_msg_v01, config_change_ind_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_indication_register_req_msg_v01, config_change_ind),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_indication_register_req_msg_v01, srv_status_change_ind) - QMI_IDL_OFFSET8(qmi_ims_iot_indication_register_req_msg_v01, srv_status_change_ind_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_indication_register_req_msg_v01, srv_status_change_ind)
};

static const uint8_t qmi_ims_iot_indication_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qmi_ims_iot_get_fw_config_req_msg is empty
 * static const uint8_t qmi_ims_iot_get_fw_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_ims_iot_get_fw_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_fw_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_fw_config_resp_msg_v01, User_Agent) - QMI_IDL_OFFSET8(qmi_ims_iot_get_fw_config_resp_msg_v01, User_Agent_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_fw_config_resp_msg_v01, User_Agent),
  IMS_IOT_USER_AGENT_LEN_V01
};

static const uint8_t qmi_ims_iot_set_fw_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_fw_config_req_msg_v01, User_Agent) - QMI_IDL_OFFSET8(qmi_ims_iot_set_fw_config_req_msg_v01, User_Agent_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_fw_config_req_msg_v01, User_Agent),
  IMS_IOT_USER_AGENT_LEN_V01
};

static const uint8_t qmi_ims_iot_set_fw_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_fw_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qmi_ims_iot_fw_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_fw_config_ind_msg_v01, User_Agent) - QMI_IDL_OFFSET8(qmi_ims_iot_fw_config_ind_msg_v01, User_Agent_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_ims_iot_fw_config_ind_msg_v01, User_Agent),
  IMS_IOT_USER_AGENT_LEN_V01
};

/*
 * qmi_ims_iot_get_sip_config_req_msg is empty
 * static const uint8_t qmi_ims_iot_get_sip_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_ims_iot_get_sip_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_T1) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_T1_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_T1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_T2) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_T2_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_T2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_T4) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_T4_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_T4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TA) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TA_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TA),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TB) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TB_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TB),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TC) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TC_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TC),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TD) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TD_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TD),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TE) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TE_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TE),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TF) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TF_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TF),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TG) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TG_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TG),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TI) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TI_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TI),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TJ) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TJ_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TJ),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TK) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TK_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_TK),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_RelResponse_Retry) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_RelResponse_Retry_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Timer_RelResponse_Retry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Stop_TimerB_On1xx) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Stop_TimerB_On1xx_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Stop_TimerB_On1xx),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Listening_Port) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Listening_Port_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Listening_Port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_TCP_MaxBackOff_Timer) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_TCP_MaxBackOff_Timer_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_TCP_MaxBackOff_Timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_TCP_Threshold) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_TCP_Threshold_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_TCP_Threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_TCP_Threshold) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_TCP_Threshold_valid)),
  0x22,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_TCP_Threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Transport_Switch) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Transport_Switch_valid)),
  0x23,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Transport_Switch),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_IPSec_Int_Scheme) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_IPSec_Int_Scheme_valid)),
  0x24,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_IPSec_Int_Scheme),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_IPSec_Enc_Algo) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_IPSec_Enc_Algo_valid)),
  0x25,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_IPSec_Enc_Algo),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Compact_Header) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Compact_Header_valid)),
  0x26,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Compact_Header),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Sip_Instance) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Sip_Instance_valid)),
  0x27,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Sip_Instance),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Route_Header) - QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Route_Header_valid)),
  0x28,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Route_Header),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Imsi_In_Contact) - QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Imsi_In_Contact_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Imsi_In_Contact),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Keep_Alive) - QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Keep_Alive_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_ims_iot_get_sip_config_resp_msg_v01, SIP_Enable_Keep_Alive)
};

static const uint8_t qmi_ims_iot_set_sip_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_T1) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_T1_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_T1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_T2) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_T2_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_T2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_T4) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_T4_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_T4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TA) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TA_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TA),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TB) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TB_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TB),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TC) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TC_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TC),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TD) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TD_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TD),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TE) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TE_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TE),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TF) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TF_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TF),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TG) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TG_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TG),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TI) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TI_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TI),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TJ) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TJ_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TJ),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TK) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TK_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_TK),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_RelResponse_Retry) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_RelResponse_Retry_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Timer_RelResponse_Retry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Stop_TimerB_On1xx) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Stop_TimerB_On1xx_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Stop_TimerB_On1xx),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Listening_Port) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Listening_Port_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Listening_Port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_TCP_MaxBackOff_Timer) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_TCP_MaxBackOff_Timer_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_TCP_MaxBackOff_Timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_TCP_Threshold) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_TCP_Threshold_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_TCP_Threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_TCP_Threshold) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_TCP_Threshold_valid)),
  0x22,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_TCP_Threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Transport_Switch) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Transport_Switch_valid)),
  0x23,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Transport_Switch),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_IPSec_Int_Scheme) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_IPSec_Int_Scheme_valid)),
  0x24,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_IPSec_Int_Scheme),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_IPSec_Enc_Algo) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_IPSec_Enc_Algo_valid)),
  0x25,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_IPSec_Enc_Algo),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Compact_Header) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Compact_Header_valid)),
  0x26,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Compact_Header),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Sip_Instance) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Sip_Instance_valid)),
  0x27,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Sip_Instance),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Route_Header) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Route_Header_valid)),
  0x28,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Route_Header),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Imsi_In_Contact) - QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Imsi_In_Contact_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Imsi_In_Contact),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Keep_Alive) - QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Keep_Alive_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_ims_iot_set_sip_config_req_msg_v01, SIP_Enable_Keep_Alive)
};

static const uint8_t qmi_ims_iot_set_sip_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sip_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qmi_ims_iot_sip_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_T1) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_T1_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_T1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_T2) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_T2_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_T2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_T4) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_T4_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_T4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TA) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TA_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TA),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TB) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TB_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TB),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TC) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TC_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TC),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TD) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TD_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TD),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TE) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TE_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TE),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TF) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TF_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TF),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TG) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TG_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TG),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TI) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TI_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TI),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TJ) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TJ_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TJ),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TK) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TK_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_TK),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_RelResponse_Retry) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_RelResponse_Retry_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Timer_RelResponse_Retry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Stop_TimerB_On1xx) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Stop_TimerB_On1xx_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Stop_TimerB_On1xx),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Listening_Port) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Listening_Port_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Listening_Port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_TCP_MaxBackOff_Timer) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_TCP_MaxBackOff_Timer_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_TCP_MaxBackOff_Timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_TCP_Threshold) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_TCP_Threshold_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_TCP_Threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_TCP_Threshold) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_TCP_Threshold_valid)),
  0x22,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_TCP_Threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Transport_Switch) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Transport_Switch_valid)),
  0x23,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Transport_Switch),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_IPSec_Int_Scheme) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_IPSec_Int_Scheme_valid)),
  0x24,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_IPSec_Int_Scheme),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_IPSec_Enc_Algo) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_IPSec_Enc_Algo_valid)),
  0x25,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_IPSec_Enc_Algo),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Compact_Header) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Compact_Header_valid)),
  0x26,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Compact_Header),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Sip_Instance) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Sip_Instance_valid)),
  0x27,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Sip_Instance),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Route_Header) - QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Route_Header_valid)),
  0x28,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Route_Header),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Imsi_In_Contact) - QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Imsi_In_Contact_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Imsi_In_Contact),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Keep_Alive) - QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Keep_Alive_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_ims_iot_sip_config_ind_msg_v01, SIP_Enable_Keep_Alive)
};

/*
 * qmi_ims_iot_get_reg_mgr_config_req_msg is empty
 * static const uint8_t qmi_ims_iot_get_reg_mgr_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_ims_iot_get_reg_mgr_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, Auth_Reg_mode) - QMI_IDL_OFFSET8(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, Auth_Reg_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, Auth_Reg_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, Reg_Retry_Base_Time) - QMI_IDL_OFFSET8(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, Reg_Retry_Base_Time_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, Reg_Retry_Base_Time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, Reg_Retry_Max_Time) - QMI_IDL_OFFSET8(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, Reg_Retry_Max_Time_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, Reg_Retry_Max_Time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, PCSCF_Server_Address) - QMI_IDL_OFFSET8(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, PCSCF_Server_Address_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, PCSCF_Server_Address),
  ((IMS_IOT_PCSCF_ADDR_LEN_V01) & 0xFF), ((IMS_IOT_PCSCF_ADDR_LEN_V01) >> 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, PCSCF_Server_Port) - QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, PCSCF_Server_Port_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01, PCSCF_Server_Port)
};

static const uint8_t qmi_ims_iot_set_reg_mgr_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, Auth_Reg_mode) - QMI_IDL_OFFSET8(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, Auth_Reg_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, Auth_Reg_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, Reg_Retry_Base_Time) - QMI_IDL_OFFSET8(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, Reg_Retry_Base_Time_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, Reg_Retry_Base_Time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, Reg_Retry_Max_Time) - QMI_IDL_OFFSET8(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, Reg_Retry_Max_Time_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, Reg_Retry_Max_Time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, PCSCF_Server_Address) - QMI_IDL_OFFSET8(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, PCSCF_Server_Address_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, PCSCF_Server_Address),
  ((IMS_IOT_PCSCF_ADDR_LEN_V01) & 0xFF), ((IMS_IOT_PCSCF_ADDR_LEN_V01) >> 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, PCSCF_Server_Port) - QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, PCSCF_Server_Port_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_ims_iot_set_reg_mgr_config_req_msg_v01, PCSCF_Server_Port)
};

static const uint8_t qmi_ims_iot_set_reg_mgr_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_reg_mgr_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qmi_ims_iot_reg_mgr_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_reg_mgr_config_ind_msg_v01, Auth_Reg_mode) - QMI_IDL_OFFSET8(qmi_ims_iot_reg_mgr_config_ind_msg_v01, Auth_Reg_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_reg_mgr_config_ind_msg_v01, Auth_Reg_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_reg_mgr_config_ind_msg_v01, Reg_Retry_Base_Time) - QMI_IDL_OFFSET8(qmi_ims_iot_reg_mgr_config_ind_msg_v01, Reg_Retry_Base_Time_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_reg_mgr_config_ind_msg_v01, Reg_Retry_Base_Time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_reg_mgr_config_ind_msg_v01, Reg_Retry_Max_Time) - QMI_IDL_OFFSET8(qmi_ims_iot_reg_mgr_config_ind_msg_v01, Reg_Retry_Max_Time_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_reg_mgr_config_ind_msg_v01, Reg_Retry_Max_Time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_reg_mgr_config_ind_msg_v01, PCSCF_Server_Address) - QMI_IDL_OFFSET8(qmi_ims_iot_reg_mgr_config_ind_msg_v01, PCSCF_Server_Address_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_ims_iot_reg_mgr_config_ind_msg_v01, PCSCF_Server_Address),
  ((IMS_IOT_PCSCF_ADDR_LEN_V01) & 0xFF), ((IMS_IOT_PCSCF_ADDR_LEN_V01) >> 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_reg_mgr_config_ind_msg_v01, PCSCF_Server_Port) - QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_reg_mgr_config_ind_msg_v01, PCSCF_Server_Port_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_ims_iot_reg_mgr_config_ind_msg_v01, PCSCF_Server_Port)
};

/*
 * qmi_ims_iot_get_service_status_config_req_msg is empty
 * static const uint8_t qmi_ims_iot_get_service_status_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_ims_iot_get_service_status_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_service_status_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_service_status_config_resp_msg_v01, VOLTE_Enable) - QMI_IDL_OFFSET8(qmi_ims_iot_get_service_status_config_resp_msg_v01, VOLTE_Enable_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_service_status_config_resp_msg_v01, VOLTE_Enable),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_service_status_config_resp_msg_v01, SMS_Over_IP_Enable) - QMI_IDL_OFFSET8(qmi_ims_iot_get_service_status_config_resp_msg_v01, SMS_Over_IP_Enable_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_service_status_config_resp_msg_v01, SMS_Over_IP_Enable)
};

static const uint8_t qmi_ims_iot_set_service_status_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_service_status_config_req_msg_v01, VOLTE_Enable) - QMI_IDL_OFFSET8(qmi_ims_iot_set_service_status_config_req_msg_v01, VOLTE_Enable_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_service_status_config_req_msg_v01, VOLTE_Enable),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_service_status_config_req_msg_v01, SMS_Over_IP_Enable) - QMI_IDL_OFFSET8(qmi_ims_iot_set_service_status_config_req_msg_v01, SMS_Over_IP_Enable_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_service_status_config_req_msg_v01, SMS_Over_IP_Enable)
};

static const uint8_t qmi_ims_iot_set_service_status_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_service_status_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qmi_ims_iot_service_status_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_service_status_config_ind_msg_v01, VOLTE_Enable) - QMI_IDL_OFFSET8(qmi_ims_iot_service_status_config_ind_msg_v01, VOLTE_Enable_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_service_status_config_ind_msg_v01, VOLTE_Enable),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_service_status_config_ind_msg_v01, SMS_Over_IP_Enable) - QMI_IDL_OFFSET8(qmi_ims_iot_service_status_config_ind_msg_v01, SMS_Over_IP_Enable_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_service_status_config_ind_msg_v01, SMS_Over_IP_Enable)
};

/*
 * qmi_ims_iot_set_apcs_config_complete_req_msg is empty
 * static const uint8_t qmi_ims_iot_set_apcs_config_complete_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_ims_iot_set_apcs_config_complete_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_apcs_config_complete_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qmi_ims_iot_get_sms_config_req_msg is empty
 * static const uint8_t qmi_ims_iot_get_sms_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_ims_iot_get_sms_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sms_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sms_config_resp_msg_v01, SMS_Format) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sms_config_resp_msg_v01, SMS_Format_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sms_config_resp_msg_v01, SMS_Format),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_sms_config_resp_msg_v01, Psi_Value) - QMI_IDL_OFFSET8(qmi_ims_iot_get_sms_config_resp_msg_v01, Psi_Value_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_sms_config_resp_msg_v01, Psi_Value),
  IMS_IOT_SMS_PSI_LEN_V01
};

static const uint8_t qmi_ims_iot_set_sms_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sms_config_req_msg_v01, SMS_Format) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sms_config_req_msg_v01, SMS_Format_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sms_config_req_msg_v01, SMS_Format),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_sms_config_req_msg_v01, Psi_Value) - QMI_IDL_OFFSET8(qmi_ims_iot_set_sms_config_req_msg_v01, Psi_Value_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sms_config_req_msg_v01, Psi_Value),
  IMS_IOT_SMS_PSI_LEN_V01
};

static const uint8_t qmi_ims_iot_set_sms_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_sms_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qmi_ims_iot_sms_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sms_config_ind_msg_v01, SMS_Format) - QMI_IDL_OFFSET8(qmi_ims_iot_sms_config_ind_msg_v01, SMS_Format_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_sms_config_ind_msg_v01, SMS_Format),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_sms_config_ind_msg_v01, Psi_Value) - QMI_IDL_OFFSET8(qmi_ims_iot_sms_config_ind_msg_v01, Psi_Value_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_ims_iot_sms_config_ind_msg_v01, Psi_Value),
  IMS_IOT_SMS_PSI_LEN_V01
};

/*
 * qmi_ims_iot_get_voice_config_req_msg is empty
 * static const uint8_t qmi_ims_iot_get_voice_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_ims_iot_get_voice_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Display_Text_Priority) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Display_Text_Priority_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Display_Text_Priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, CallerId_Mode) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, CallerId_Mode_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, CallerId_Mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, T_CALL) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, T_CALL_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, T_CALL),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Ringing_Timer) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Ringing_Timer_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Ringing_Timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, RingBack_Timer) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, RingBack_Timer_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, RingBack_Timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Session_Timer_Enabled) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Session_Timer_Enabled_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Session_Timer_Enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Session_Expires) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Session_Expires_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Session_Expires),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Min_Session_Expires) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Min_Session_Expires_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Min_Session_Expires),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Rtcp_Link_Aliveness_Timer) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Rtcp_Link_Aliveness_Timer_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Rtcp_Link_Aliveness_Timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Amr_OctetAligned_DynamicPt) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Amr_OctetAligned_DynamicPt_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Amr_OctetAligned_DynamicPt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Amr_BandWidthEfficient_DynamicPt) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Amr_BandWidthEfficient_DynamicPt_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Amr_BandWidthEfficient_DynamicPt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Dtmf_Nb_DynamicPt) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Dtmf_Nb_DynamicPt_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Dtmf_Nb_DynamicPt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Amr_ModeSet) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Amr_ModeSet_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Amr_ModeSet),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Scr_AMR_Enable) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Scr_AMR_Enable_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Scr_AMR_Enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Speech_Start_Port) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Speech_Start_Port_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Speech_Start_Port),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Speech_Stop_Port) - QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Speech_Stop_Port_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_voice_config_resp_msg_v01, Speech_Stop_Port)
};

static const uint8_t qmi_ims_iot_set_voice_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Display_Text_Priority) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Display_Text_Priority_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Display_Text_Priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, CallerId_Mode) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, CallerId_Mode_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, CallerId_Mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, T_CALL) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, T_CALL_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, T_CALL),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Ringing_Timer) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Ringing_Timer_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Ringing_Timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, RingBack_Timer) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, RingBack_Timer_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, RingBack_Timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Session_Timer_Enabled) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Session_Timer_Enabled_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Session_Timer_Enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Session_Expires) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Session_Expires_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Session_Expires),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Min_Session_Expires) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Min_Session_Expires_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Min_Session_Expires),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Rtcp_Link_Aliveness_Timer) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Rtcp_Link_Aliveness_Timer_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Rtcp_Link_Aliveness_Timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Amr_OctetAligned_DynamicPt) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Amr_OctetAligned_DynamicPt_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Amr_OctetAligned_DynamicPt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Amr_BandWidthEfficient_DynamicPt) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Amr_BandWidthEfficient_DynamicPt_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Amr_BandWidthEfficient_DynamicPt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Dtmf_Nb_DynamicPt) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Dtmf_Nb_DynamicPt_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Dtmf_Nb_DynamicPt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Amr_ModeSet) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Amr_ModeSet_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Amr_ModeSet),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Scr_AMR_Enable) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Scr_AMR_Enable_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Scr_AMR_Enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Speech_Start_Port) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Speech_Start_Port_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Speech_Start_Port),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Speech_Stop_Port) - QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Speech_Stop_Port_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_req_msg_v01, Speech_Stop_Port)
};

static const uint8_t qmi_ims_iot_set_voice_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_set_voice_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qmi_ims_iot_voice_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Display_Text_Priority) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Display_Text_Priority_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Display_Text_Priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, CallerId_Mode) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, CallerId_Mode_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, CallerId_Mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, T_CALL) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, T_CALL_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, T_CALL),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Ringing_Timer) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Ringing_Timer_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Ringing_Timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, RingBack_Timer) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, RingBack_Timer_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, RingBack_Timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Session_Timer_Enabled) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Session_Timer_Enabled_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Session_Timer_Enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Session_Expires) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Session_Expires_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Session_Expires),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Min_Session_Expires) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Min_Session_Expires_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Min_Session_Expires),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Rtcp_Link_Aliveness_Timer) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Rtcp_Link_Aliveness_Timer_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Rtcp_Link_Aliveness_Timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Amr_OctetAligned_DynamicPt) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Amr_OctetAligned_DynamicPt_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Amr_OctetAligned_DynamicPt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Amr_BandWidthEfficient_DynamicPt) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Amr_BandWidthEfficient_DynamicPt_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Amr_BandWidthEfficient_DynamicPt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Dtmf_Nb_DynamicPt) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Dtmf_Nb_DynamicPt_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Dtmf_Nb_DynamicPt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Amr_ModeSet) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Amr_ModeSet_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Amr_ModeSet),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Scr_AMR_Enable) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Scr_AMR_Enable_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Scr_AMR_Enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Speech_Start_Port) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Speech_Start_Port_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Speech_Start_Port),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Speech_Stop_Port) - QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Speech_Stop_Port_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_voice_config_ind_msg_v01, Speech_Stop_Port)
};

/*
 * qmi_ims_iot_get_registration_service_status_req_msg is empty
 * static const uint8_t qmi_ims_iot_get_registration_service_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_ims_iot_get_registration_service_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, registration_status) - QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, registration_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, registration_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, sms_service_status) - QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, sms_service_status_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, sms_service_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, voice_service_status) - QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, voice_service_status_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, voice_service_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, registration_failure_reason_code) - QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, registration_failure_reason_code_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, registration_failure_reason_code),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, registration_failure_reason) - QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, registration_failure_reason_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_service_status_resp_msg_v01, registration_failure_reason),
  IMS_IOT_REGISTRATION_FAILURE_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_get_registration_service_status_resp_msg_v01, uri_list) - QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_get_registration_service_status_resp_msg_v01, uri_list_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmi_ims_iot_get_registration_service_status_resp_msg_v01, uri_list),
  IMS_IOT_MAX_URI_LIST_V01,
  QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_get_registration_service_status_resp_msg_v01, uri_list) - QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_get_registration_service_status_resp_msg_v01, uri_list_len),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t qmi_ims_iot_registration_service_status_ind_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, registration_status) - QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, registration_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, registration_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, sms_service_status) - QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, sms_service_status_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, sms_service_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, voice_service_status) - QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, voice_service_status_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, voice_service_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, registration_failure_reason_code) - QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, registration_failure_reason_code_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, registration_failure_reason_code),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, registration_failure_reason) - QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, registration_failure_reason_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_ims_iot_registration_service_status_ind_msg_v01, registration_failure_reason),
  IMS_IOT_REGISTRATION_FAILURE_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_registration_service_status_ind_msg_v01, uri_list) - QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_registration_service_status_ind_msg_v01, uri_list_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmi_ims_iot_registration_service_status_ind_msg_v01, uri_list),
  IMS_IOT_MAX_URI_LIST_V01,
  QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_registration_service_status_ind_msg_v01, uri_list) - QMI_IDL_OFFSET16RELATIVE(qmi_ims_iot_registration_service_status_ind_msg_v01, uri_list_len),
  QMI_IDL_TYPE88(0, 0)
};

/*
 * qmi_ims_iot_get_pdn_status_req_msg is empty
 * static const uint8_t qmi_ims_iot_get_pdn_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_ims_iot_get_pdn_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_pdn_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_pdn_status_resp_msg_v01, pdn_status) - QMI_IDL_OFFSET8(qmi_ims_iot_get_pdn_status_resp_msg_v01, pdn_status_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_pdn_status_resp_msg_v01, pdn_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_pdn_status_resp_msg_v01, pdn_failure_reason) - QMI_IDL_OFFSET8(qmi_ims_iot_get_pdn_status_resp_msg_v01, pdn_failure_reason_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_pdn_status_resp_msg_v01, pdn_failure_reason),
  IMS_IOT_PDN_FAILURE_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_pdn_status_resp_msg_v01, pdn_fail_code) - QMI_IDL_OFFSET8(qmi_ims_iot_get_pdn_status_resp_msg_v01, pdn_fail_code_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_pdn_status_resp_msg_v01, pdn_fail_code),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_pdn_status_resp_msg_v01, pdn_fail_cause) - QMI_IDL_OFFSET8(qmi_ims_iot_get_pdn_status_resp_msg_v01, pdn_fail_cause_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_pdn_status_resp_msg_v01, pdn_fail_cause),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t qmi_ims_iot_pdn_status_ind_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_pdn_status_ind_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_pdn_status_ind_msg_v01, pdn_status) - QMI_IDL_OFFSET8(qmi_ims_iot_pdn_status_ind_msg_v01, pdn_status_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_pdn_status_ind_msg_v01, pdn_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_pdn_status_ind_msg_v01, pdn_failure_reason) - QMI_IDL_OFFSET8(qmi_ims_iot_pdn_status_ind_msg_v01, pdn_failure_reason_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_ims_iot_pdn_status_ind_msg_v01, pdn_failure_reason),
  IMS_IOT_PDN_FAILURE_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_pdn_status_ind_msg_v01, pdn_fail_code) - QMI_IDL_OFFSET8(qmi_ims_iot_pdn_status_ind_msg_v01, pdn_fail_code_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_pdn_status_ind_msg_v01, pdn_fail_code),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_pdn_status_ind_msg_v01, pdn_fail_cause) - QMI_IDL_OFFSET8(qmi_ims_iot_pdn_status_ind_msg_v01, pdn_fail_cause_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_pdn_status_ind_msg_v01, pdn_fail_cause),
  QMI_IDL_TYPE88(0, 1)
};

/*
 * qmi_ims_iot_get_registration_block_status_req_msg is empty
 * static const uint8_t qmi_ims_iot_get_registration_block_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_ims_iot_get_registration_block_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_block_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_block_status_resp_msg_v01, block_status) - QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_block_status_resp_msg_v01, block_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_registration_block_status_resp_msg_v01, block_status)
};

static const uint8_t qmi_ims_iot_registration_block_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_registration_block_status_ind_msg_v01, block_status) - QMI_IDL_OFFSET8(qmi_ims_iot_registration_block_status_ind_msg_v01, block_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_registration_block_status_ind_msg_v01, block_status)
};

static const uint8_t qmi_ims_iot_registration_unblock_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_registration_unblock_req_msg_v01, pdn_unblock),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_registration_unblock_req_msg_v01, registration_unblock)
};

static const uint8_t qmi_ims_iot_registration_unblock_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_registration_unblock_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qmi_ims_iot_get_lte_attach_parameters_req_msg is empty
 * static const uint8_t qmi_ims_iot_get_lte_attach_parameters_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_ims_iot_get_lte_attach_parameters_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_lte_attach_parameters_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ims_iot_get_lte_attach_parameters_resp_msg_v01, operator_reserved_pco) - QMI_IDL_OFFSET8(qmi_ims_iot_get_lte_attach_parameters_resp_msg_v01, operator_reserved_pco_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_get_lte_attach_parameters_resp_msg_v01, operator_reserved_pco)
};

static const uint8_t qmi_ims_iot_lte_attach_parameters_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_ims_iot_lte_attach_parameters_ind_msg_v01, operator_reserved_pco)
};

/* Type Table */
static const qmi_idl_type_table_entry  ims_iot_type_table_v01[] = {
  {sizeof(ims_iot_uri_s_type_v01), ims_iot_uri_s_type_data_v01},
  {sizeof(ims_iot_pdn_failure_reason_s_type_v01), ims_iot_pdn_failure_reason_s_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry ims_iot_message_table_v01[] = {
  {sizeof(qmi_ims_iot_indication_register_req_msg_v01), qmi_ims_iot_indication_register_req_msg_data_v01},
  {sizeof(qmi_ims_iot_indication_register_resp_msg_v01), qmi_ims_iot_indication_register_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_get_fw_config_req_msg_v01), 0},
  {sizeof(qmi_ims_iot_get_fw_config_resp_msg_v01), qmi_ims_iot_get_fw_config_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_set_fw_config_req_msg_v01), qmi_ims_iot_set_fw_config_req_msg_data_v01},
  {sizeof(qmi_ims_iot_set_fw_config_resp_msg_v01), qmi_ims_iot_set_fw_config_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_fw_config_ind_msg_v01), qmi_ims_iot_fw_config_ind_msg_data_v01},
  {sizeof(qmi_ims_iot_get_sip_config_req_msg_v01), 0},
  {sizeof(qmi_ims_iot_get_sip_config_resp_msg_v01), qmi_ims_iot_get_sip_config_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_set_sip_config_req_msg_v01), qmi_ims_iot_set_sip_config_req_msg_data_v01},
  {sizeof(qmi_ims_iot_set_sip_config_resp_msg_v01), qmi_ims_iot_set_sip_config_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_sip_config_ind_msg_v01), qmi_ims_iot_sip_config_ind_msg_data_v01},
  {sizeof(qmi_ims_iot_get_reg_mgr_config_req_msg_v01), 0},
  {sizeof(qmi_ims_iot_get_reg_mgr_config_resp_msg_v01), qmi_ims_iot_get_reg_mgr_config_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_set_reg_mgr_config_req_msg_v01), qmi_ims_iot_set_reg_mgr_config_req_msg_data_v01},
  {sizeof(qmi_ims_iot_set_reg_mgr_config_resp_msg_v01), qmi_ims_iot_set_reg_mgr_config_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_reg_mgr_config_ind_msg_v01), qmi_ims_iot_reg_mgr_config_ind_msg_data_v01},
  {sizeof(qmi_ims_iot_get_service_status_config_req_msg_v01), 0},
  {sizeof(qmi_ims_iot_get_service_status_config_resp_msg_v01), qmi_ims_iot_get_service_status_config_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_set_service_status_config_req_msg_v01), qmi_ims_iot_set_service_status_config_req_msg_data_v01},
  {sizeof(qmi_ims_iot_set_service_status_config_resp_msg_v01), qmi_ims_iot_set_service_status_config_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_service_status_config_ind_msg_v01), qmi_ims_iot_service_status_config_ind_msg_data_v01},
  {sizeof(qmi_ims_iot_set_apcs_config_complete_req_msg_v01), 0},
  {sizeof(qmi_ims_iot_set_apcs_config_complete_resp_msg_v01), qmi_ims_iot_set_apcs_config_complete_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_get_sms_config_req_msg_v01), 0},
  {sizeof(qmi_ims_iot_get_sms_config_resp_msg_v01), qmi_ims_iot_get_sms_config_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_set_sms_config_req_msg_v01), qmi_ims_iot_set_sms_config_req_msg_data_v01},
  {sizeof(qmi_ims_iot_set_sms_config_resp_msg_v01), qmi_ims_iot_set_sms_config_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_sms_config_ind_msg_v01), qmi_ims_iot_sms_config_ind_msg_data_v01},
  {sizeof(qmi_ims_iot_get_voice_config_req_msg_v01), 0},
  {sizeof(qmi_ims_iot_get_voice_config_resp_msg_v01), qmi_ims_iot_get_voice_config_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_set_voice_config_req_msg_v01), qmi_ims_iot_set_voice_config_req_msg_data_v01},
  {sizeof(qmi_ims_iot_set_voice_config_resp_msg_v01), qmi_ims_iot_set_voice_config_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_voice_config_ind_msg_v01), qmi_ims_iot_voice_config_ind_msg_data_v01},
  {sizeof(qmi_ims_iot_get_registration_service_status_req_msg_v01), 0},
  {sizeof(qmi_ims_iot_get_registration_service_status_resp_msg_v01), qmi_ims_iot_get_registration_service_status_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_registration_service_status_ind_msg_v01), qmi_ims_iot_registration_service_status_ind_msg_data_v01},
  {sizeof(qmi_ims_iot_get_pdn_status_req_msg_v01), 0},
  {sizeof(qmi_ims_iot_get_pdn_status_resp_msg_v01), qmi_ims_iot_get_pdn_status_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_pdn_status_ind_msg_v01), qmi_ims_iot_pdn_status_ind_msg_data_v01},
  {sizeof(qmi_ims_iot_get_registration_block_status_req_msg_v01), 0},
  {sizeof(qmi_ims_iot_get_registration_block_status_resp_msg_v01), qmi_ims_iot_get_registration_block_status_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_registration_block_status_ind_msg_v01), qmi_ims_iot_registration_block_status_ind_msg_data_v01},
  {sizeof(qmi_ims_iot_registration_unblock_req_msg_v01), qmi_ims_iot_registration_unblock_req_msg_data_v01},
  {sizeof(qmi_ims_iot_registration_unblock_resp_msg_v01), qmi_ims_iot_registration_unblock_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_get_lte_attach_parameters_req_msg_v01), 0},
  {sizeof(qmi_ims_iot_get_lte_attach_parameters_resp_msg_v01), qmi_ims_iot_get_lte_attach_parameters_resp_msg_data_v01},
  {sizeof(qmi_ims_iot_lte_attach_parameters_ind_msg_v01), qmi_ims_iot_lte_attach_parameters_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object ims_iot_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *ims_iot_qmi_idl_type_table_object_referenced_tables_v01[] =
{&ims_iot_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object ims_iot_qmi_idl_type_table_object_v01 = {
  sizeof(ims_iot_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(ims_iot_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  ims_iot_type_table_v01,
  ims_iot_message_table_v01,
  ims_iot_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry ims_iot_service_command_messages_v01[] = {
  {QMI_IMS_IOT_GET_SUPPORTED_MSGS_REQ_V01, QMI_IDL_TYPE16(1, 0), 0},
  {QMI_IMS_IOT_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 0), 8},
  {QMI_IMS_IOT_GET_FW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 2), 0},
  {QMI_IMS_IOT_SET_FW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 4), 131},
  {QMI_IMS_IOT_GET_SIP_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 7), 0},
  {QMI_IMS_IOT_SET_SIP_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 9), 189},
  {QMI_IMS_IOT_GET_REG_MGR_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 12), 0},
  {QMI_IMS_IOT_SET_REG_MGR_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 14), 287},
  {QMI_IMS_IOT_GET_SERVICE_STATUS_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 17), 0},
  {QMI_IMS_IOT_SET_SERVICE_STATUS_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 19), 14},
  {QMI_IMS_IOT_SET_APCS_CONFIG_COMPLETE_REQ_V01, QMI_IDL_TYPE16(0, 22), 0},
  {QMI_IMS_IOT_GET_SMS_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 24), 0},
  {QMI_IMS_IOT_SET_SMS_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 26), 138},
  {QMI_IMS_IOT_GET_VOICE_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 29), 0},
  {QMI_IMS_IOT_SET_VOICE_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 31), 112},
  {QMI_IMS_IOT_GET_REGISTRATION_SERVICE_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 34), 0},
  {QMI_IMS_IOT_GET_PDN_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 37), 0},
  {QMI_IMS_IOT_GET_REGISTRATION_BLOCK_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 40), 0},
  {QMI_IMS_IOT_REGISTRATION_UNBLOCK_REQ_V01, QMI_IDL_TYPE16(0, 43), 8},
  {QMI_IMS_IOT_GET_LTE_ATTACH_PARAMETERS_REQ_V01, QMI_IDL_TYPE16(0, 45), 0}
};

static const qmi_idl_service_message_table_entry ims_iot_service_response_messages_v01[] = {
  {QMI_IMS_IOT_GET_SUPPORTED_MSGS_RESP_V01, QMI_IDL_TYPE16(1, 1), 8204},
  {QMI_IMS_IOT_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_IMS_IOT_GET_FW_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 3), 138},
  {QMI_IMS_IOT_SET_FW_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_IMS_IOT_GET_SIP_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 8), 196},
  {QMI_IMS_IOT_SET_SIP_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 10), 7},
  {QMI_IMS_IOT_GET_REG_MGR_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 13), 294},
  {QMI_IMS_IOT_SET_REG_MGR_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 15), 7},
  {QMI_IMS_IOT_GET_SERVICE_STATUS_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 18), 21},
  {QMI_IMS_IOT_SET_SERVICE_STATUS_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 20), 7},
  {QMI_IMS_IOT_SET_APCS_CONFIG_COMPLETE_RESP_V01, QMI_IDL_TYPE16(0, 23), 7},
  {QMI_IMS_IOT_GET_SMS_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 25), 145},
  {QMI_IMS_IOT_SET_SMS_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 27), 7},
  {QMI_IMS_IOT_GET_VOICE_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 30), 119},
  {QMI_IMS_IOT_SET_VOICE_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 32), 7},
  {QMI_IMS_IOT_GET_REGISTRATION_SERVICE_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 35), 1577},
  {QMI_IMS_IOT_GET_PDN_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 38), 160},
  {QMI_IMS_IOT_GET_REGISTRATION_BLOCK_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 41), 14},
  {QMI_IMS_IOT_REGISTRATION_UNBLOCK_RESP_V01, QMI_IDL_TYPE16(0, 44), 7},
  {QMI_IMS_IOT_GET_LTE_ATTACH_PARAMETERS_RESP_V01, QMI_IDL_TYPE16(0, 46), 11}
};

static const qmi_idl_service_message_table_entry ims_iot_service_indication_messages_v01[] = {
  {QMI_IMS_IOT_FW_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 6), 131},
  {QMI_IMS_IOT_SIP_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 11), 189},
  {QMI_IMS_IOT_REG_MGR_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 16), 287},
  {QMI_IMS_IOT_SERVICE_STATUS_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 21), 14},
  {QMI_IMS_IOT_SMS_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 28), 138},
  {QMI_IMS_IOT_VOICE_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 33), 112},
  {QMI_IMS_IOT_REGISTRATION_SERVICE_STATUS_IND_V01, QMI_IDL_TYPE16(0, 36), 1577},
  {QMI_IMS_IOT_PDN_STATUS_IND_V01, QMI_IDL_TYPE16(0, 39), 160},
  {QMI_IMS_IOT_REGISTRATION_BLOCK_STATUS_IND_V01, QMI_IDL_TYPE16(0, 42), 7},
  {QMI_IMS_IOT_LTE_ATTACH_PARAMETERS_IND_V01, QMI_IDL_TYPE16(0, 47), 4}
};

/*Service Object*/
struct qmi_idl_service_object ims_iot_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x4F,
  8204,
  { sizeof(ims_iot_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ims_iot_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ims_iot_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { ims_iot_service_command_messages_v01, ims_iot_service_response_messages_v01, ims_iot_service_indication_messages_v01},
  &ims_iot_qmi_idl_type_table_object_v01,
  0x02,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type ims_iot_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( IMS_IOT_V01_IDL_MAJOR_VERS != idl_maj_version || IMS_IOT_V01_IDL_MINOR_VERS != idl_min_version
       || IMS_IOT_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&ims_iot_qmi_idl_service_object_v01;
}

