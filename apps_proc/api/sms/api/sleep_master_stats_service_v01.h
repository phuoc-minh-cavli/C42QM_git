#ifndef MASTER_STATS_SERVICE_01_H
#define MASTER_STATS_SERVICE_01_H
/**
  @file sleep_master_stats_service_v01.h

  @brief This is the public header file which defines the master_stats service Data structures.

  This header file defines the types and structures that were defined in
  master_stats. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.tx/1.0/sms/api/sleep_master_stats_service_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Wed Apr 22 2020 (Spin 0)
   From IDL File: sleep_master_stats_service_v01.idl */

/** @defgroup master_stats_qmi_consts Constant values defined in the IDL */
/** @defgroup master_stats_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup master_stats_qmi_enums Enumerated types used in QMI messages */
/** @defgroup master_stats_qmi_messages Structures sent as QMI messages */
/** @defgroup master_stats_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup master_stats_qmi_accessor Accessor for QMI service object */
/** @defgroup master_stats_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup master_stats_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define MASTER_STATS_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define MASTER_STATS_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define MASTER_STATS_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define MASTER_STATS_V01_MAX_MESSAGE_ID 0x0001
/**
    @}
  */


/** @addtogroup master_stats_qmi_consts
    @{
  */
#define QMI_SLEEP_MASTER_STATS_SVC_ID_V01 1068
/**
    @}
  */

/** @addtogroup master_stats_qmi_enums
    @{
  */
typedef enum {
  SLEEPMSQMISTATSTYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SLEEP_MS_QMI_STATS_TYPE_MODEM_V01 = 0, /**<  Sleep Master Stats for Modem Subsystem.  */
  SLEEPMSQMISTATSTYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}SleepMsQmiStatsType_v01;
/**
    @}
  */

/** @addtogroup master_stats_qmi_messages
    @{
  */
/** Request Message; This command queries the sleep master stats from QMI service */
typedef struct {

  /* Mandatory */
  SleepMsQmiStatsType_v01 stats_type;
  /**<   Sleep Master Stats Type. */
}sleep_ms_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup master_stats_qmi_messages
    @{
  */
/** Response Message; This command queries the sleep master stats from QMI service */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI Response.  */

  /* Optional */
  uint8_t stats_type_valid;  /**< Must be set to true if stats_type is being passed */
  SleepMsQmiStatsType_v01 stats_type;
  /**<   Sleep Stats Type. */

  /* Optional */
  uint8_t version_id_valid;  /**< Must be set to true if version_id is being passed */
  uint32_t version_id;
  /**<   Sleep Stats Version. */

  /* Optional */
  uint8_t sleep_count_valid;  /**< Must be set to true if sleep_count is being passed */
  uint32_t sleep_count;
  /**<   Sleep Count. */

  /* Optional */
  uint8_t sleep_last_entered_at_valid;  /**< Must be set to true if sleep_last_entered_at is being passed */
  uint64_t sleep_last_entered_at;
  /**<   Qtimer timestamp of last sleep entry. */

  /* Optional */
  uint8_t sleep_last_exited_at_valid;  /**< Must be set to true if sleep_last_exited_at is being passed */
  uint64_t sleep_last_exited_at;
  /**<   Qtimer timestamp of last sleep exit. */

  /* Optional */
  uint8_t sleep_accumulated_duration_valid;  /**< Must be set to true if sleep_accumulated_duration is being passed */
  uint64_t sleep_accumulated_duration;
  /**<   Total sleep duration. */
}sleep_ms_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_SLEEP_MS_QUERY_V01

/*Service Message Definition*/
/** @addtogroup master_stats_qmi_msg_ids
    @{
  */
#define QMI_SLEEP_MS_REQ_V01 0x0001
#define QMI_SLEEP_MS_RESP_V01 0x0001
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro master_stats_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type master_stats_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define master_stats_get_service_object_v01( ) \
          master_stats_get_service_object_internal_v01( \
            MASTER_STATS_V01_IDL_MAJOR_VERS, MASTER_STATS_V01_IDL_MINOR_VERS, \
            MASTER_STATS_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

