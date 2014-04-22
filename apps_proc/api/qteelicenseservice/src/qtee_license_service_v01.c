/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        Q T E E _ L I C E N S E _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the qteelicenseservice service Data structures.

  Copyright (c) 2018 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.



  $Header: //components/rel/qmimsgs.tx/1.0/qteelicenseservice/src/qtee_license_service_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Fri Jul  6 2018 (Spin 0)
   From IDL File: QTEE_license_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "qtee_license_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t qmi_qteelicenseservice_license_check_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_req_msg_v01, ss_client_id),

  0x02,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_req_msg_v01, transactionID),

  0x03,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_req_msg_v01, securedQuery),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_req_msg_v01, licenseCheckReq),
  ((QMI_QTEELS_LICENSE_CHECK_REQ_LENGTH_V01) & 0xFF), ((QMI_QTEELS_LICENSE_CHECK_REQ_LENGTH_V01) >> 8),
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_req_msg_v01, licenseCheckReq) - QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_req_msg_v01, licenseCheckReq_len)
};

static const uint8_t qmi_qteelicenseservice_license_check_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_resp_msg_v01, license_check_resp) - QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_resp_msg_v01, license_check_resp_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_resp_msg_v01, license_check_resp)
};

static const uint8_t qmi_qteelicenseservice_license_check_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_ind_msg_v01, status),

  0x02,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_ind_msg_v01, transactionID),

  0x03,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_ind_msg_v01, securedResponse),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_ind_msg_v01, licenseCheckIndResp),
  ((QMI_QTEELS_LICENSE_CHECK_IND_RESP_LENGTH_V01) & 0xFF), ((QMI_QTEELS_LICENSE_CHECK_IND_RESP_LENGTH_V01) >> 8),
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_ind_msg_v01, licenseCheckIndResp) - QMI_IDL_OFFSET8(qmi_qteelicenseservice_license_check_ind_msg_v01, licenseCheckIndResp_len)
};

static const uint8_t qmi_qteelicenseservice_feature_report_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_feature_report_req_msg_v01, ss_client_id),

  0x02,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_feature_report_req_msg_v01, transactionID),

  0x03,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_feature_report_req_msg_v01, securedQuery),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_feature_report_req_msg_v01, featureReport),
  ((QMI_QTEELS_FEATURE_REPORT_REQ_LENGTH_V01) & 0xFF), ((QMI_QTEELS_FEATURE_REPORT_REQ_LENGTH_V01) >> 8),
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_feature_report_req_msg_v01, featureReport) - QMI_IDL_OFFSET8(qmi_qteelicenseservice_feature_report_req_msg_v01, featureReport_len)
};

static const uint8_t qmi_qteelicenseservice_feature_report_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_feature_report_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_qteelicenseservice_feature_report_resp_msg_v01, feature_report_resp) - QMI_IDL_OFFSET8(qmi_qteelicenseservice_feature_report_resp_msg_v01, feature_report_resp_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_feature_report_resp_msg_v01, feature_report_resp)
};

static const uint8_t qmi_qteelicenseservice_feature_report_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_qteelicenseservice_feature_report_ind_msg_v01, status)
};

/*
 * qmi_qteelicenseservice_sync_completed_broadcast_ind is empty
 * static const uint8_t qmi_qteelicenseservice_sync_completed_broadcast_ind_data_v01[] = {
 * };
 */

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry qteelicenseservice_message_table_v01[] = {
  {sizeof(qmi_qteelicenseservice_license_check_req_msg_v01), qmi_qteelicenseservice_license_check_req_msg_data_v01},
  {sizeof(qmi_qteelicenseservice_license_check_resp_msg_v01), qmi_qteelicenseservice_license_check_resp_msg_data_v01},
  {sizeof(qmi_qteelicenseservice_license_check_ind_msg_v01), qmi_qteelicenseservice_license_check_ind_msg_data_v01},
  {sizeof(qmi_qteelicenseservice_feature_report_req_msg_v01), qmi_qteelicenseservice_feature_report_req_msg_data_v01},
  {sizeof(qmi_qteelicenseservice_feature_report_resp_msg_v01), qmi_qteelicenseservice_feature_report_resp_msg_data_v01},
  {sizeof(qmi_qteelicenseservice_feature_report_ind_msg_v01), qmi_qteelicenseservice_feature_report_ind_msg_data_v01},
  {sizeof(qmi_qteelicenseservice_sync_completed_broadcast_ind_v01), 0}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object qteelicenseservice_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *qteelicenseservice_qmi_idl_type_table_object_referenced_tables_v01[] =
{&qteelicenseservice_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object qteelicenseservice_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(qteelicenseservice_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  qteelicenseservice_message_table_v01,
  qteelicenseservice_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry qteelicenseservice_service_command_messages_v01[] = {
  {QMI_QTEELS_LICENSE_CHECK_REQ_V01, QMI_IDL_TYPE16(0, 0), 1045},
  {QMI_QTEELS_FEATURE_REPORT_REQ_V01, QMI_IDL_TYPE16(0, 3), 1045}
};

static const qmi_idl_service_message_table_entry qteelicenseservice_service_response_messages_v01[] = {
  {QMI_QTEELS_LICENSE_CHECK_RESP_V01, QMI_IDL_TYPE16(0, 1), 14},
  {QMI_QTEELS_FEATURE_REPORT_RESP_V01, QMI_IDL_TYPE16(0, 4), 14}
};

static const qmi_idl_service_message_table_entry qteelicenseservice_service_indication_messages_v01[] = {
  {QMI_QTEELS_LICENSE_CHECK_IND_V01, QMI_IDL_TYPE16(0, 2), 1045},
  {QMI_QTEELS_FEATURE_REPORT_IND_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_QTEELS_SYNC_COMPLETE_BROADCAST_IND_V01, QMI_IDL_TYPE16(0, 6), 0}
};

/*Service Object*/
struct qmi_idl_service_object qteelicenseservice_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x423,
  1045,
  { sizeof(qteelicenseservice_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(qteelicenseservice_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(qteelicenseservice_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { qteelicenseservice_service_command_messages_v01, qteelicenseservice_service_response_messages_v01, qteelicenseservice_service_indication_messages_v01},
  &qteelicenseservice_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type qteelicenseservice_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( QTEELICENSESERVICE_V01_IDL_MAJOR_VERS != idl_maj_version || QTEELICENSESERVICE_V01_IDL_MINOR_VERS != idl_min_version
       || QTEELICENSESERVICE_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&qteelicenseservice_qmi_idl_service_object_v01;
}

