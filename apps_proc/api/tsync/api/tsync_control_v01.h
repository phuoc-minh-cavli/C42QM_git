#ifndef TSYNC_SERVICE_01_H
#define TSYNC_SERVICE_01_H
/**
  @file tsync_control_v01.h
  
  @brief This is the public header file which defines the tsync service Data structures.

  This header file defines the types and structures that were defined in 
  tsync. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved. 
 Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/qmimsgs.tx/1.0/tsync/api/tsync_control_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2 
   It was generated on: Wed Jul 24 2013 (Spin 2)
   From IDL File: tsync_control_v01.idl */

/** @defgroup tsync_qmi_consts Constant values defined in the IDL */
/** @defgroup tsync_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup tsync_qmi_enums Enumerated types used in QMI messages */
/** @defgroup tsync_qmi_messages Structures sent as QMI messages */
/** @defgroup tsync_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup tsync_qmi_accessor Accessor for QMI service object */
/** @defgroup tsync_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup tsync_qmi_version 
    @{ 
  */ 
/** Major Version Number of the IDL used to generate this file */
#define TSYNC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define TSYNC_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define TSYNC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define TSYNC_V01_MAX_MESSAGE_ID 0x0023;
/** 
    @} 
  */


/** @addtogroup tsync_qmi_consts 
    @{ 
  */
/**
    @}
  */

/** @addtogroup tsync_qmi_enums
    @{
  */
typedef enum {
  QMI_TSYNC_CMD_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_TSYNC_DISABLE_V01 = 0, 
  QMI_TSYNC_ENABLE_V01 = 1, 
  QMI_TSYNC_CMD_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_tsync_cmd_enum_type_v01;
/**
    @}
  */

/** @addtogroup tsync_qmi_messages
    @{
  */
/** Request Message; Enables/disables the TSYNC service. */
typedef struct {

  /* Mandatory */
  /*  TSYNC Command  */
  qmi_tsync_cmd_enum_type_v01 tsync_enable_state;
  /**<   Values: \n
       - Disabled \n
       - Enabled
  */

  /* Optional */
  /*  Enable State Indication */
  uint8_t enable_state_ind_valid;  /**< Must be set to true if enable_state_ind is being passed */
  uint8_t enable_state_ind;
  /**<   Enable or disable the TSYNC state change indication.*/

  /* Optional */
  /*  Enable Event Indication */
  uint8_t enable_event_ind_valid;  /**< Must be set to true if enable_event_ind is being passed */
  uint8_t enable_event_ind;
  /**<   Enable or disable the TSYNC event indication.*/
}qmi_tsync_enable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup tsync_qmi_messages
    @{
  */
/** Response Message; Enables/disables the TSYNC service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 qmi_result;
  /**<   Standard response type containing the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are
     described in the error codes section.
  */
}qmi_tsync_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_tsync_get_state_req_msg_v01;

/** @addtogroup tsync_qmi_enums
    @{
  */
typedef enum {
  QMI_TSYNC_STATE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_TSYNC_DISABLED_V01 = 0, 
  QMI_TSYNC_ENABLED_V01 = 1, 
  QMI_TSYNC_UNAVAILABLE_V01 = 2, 
  QMI_TSYNC_STATE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_tsync_state_enum_type_v01;
/**
    @}
  */

/** @addtogroup tsync_qmi_messages
    @{
  */
/** Response Message; Queries the TSYNC system state. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 qmi_result;
  /**<   Standard response type. Values: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are
     described in the error codes section.
  */

  /* Optional */
  /*  Current TSYNC State */
  uint8_t tsync_state_valid;  /**< Must be set to true if tsync_state is being passed */
  qmi_tsync_state_enum_type_v01 tsync_state;
  /**<   Values: \n
       - QMI_TSYNC_DISABLED  -- Disabled \n
       - QMI_TSYNC_ENABLED   -- Enabled \n
       - QMI_TSYNC_UNAVAILABLE  -- TSYNC service is unavailable and cannot currently be enabled, 
                                   likely due to an out of service condition
  */

  /* Optional */
  /*  Sync Period */
  uint8_t sync_period_ms_valid;  /**< Must be set to true if sync_period_ms is being passed */
  uint16_t sync_period_ms;
  /**<   Periodic rate of corrections to the TSYNC service, in milliseconds.*/
}qmi_tsync_get_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup tsync_qmi_messages
    @{
  */
/** Indication Message; Indicates a change in the TSYNC system state. */
typedef struct {

  /* Mandatory */
  /*  TSYNC State  */
  qmi_tsync_state_enum_type_v01 tsync_state;
  /**<   Values: \n
       - QMI_TSYNC_DISABLED -- Disabled \n
       - QMI_TSYNC_ENABLED  -- Enabled \n
       - QMI_TSYNC_UNAVAILABLE --TSYNC system is unavailable and does not respond to an enable
           request, likely due to an out of service condition
  */

  /* Optional */
  /*  Sync Period */
  uint8_t sync_period_ms_valid;  /**< Must be set to true if sync_period_ms is being passed */
  uint16_t sync_period_ms;
  /**<   Periodic rate of corrections to the TSYNC service, in milliseconds.*/
}qmi_tsync_state_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup tsync_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t utc_hour;
  /**<   UTC hour. Range: 0 to 23.*/

  uint8_t utc_minute;
  /**<   UTC minute. Range: 0 to 59.*/

  uint8_t utc_second;
  /**<   UTC second. Range: 0 to 59.*/

  uint32_t utc_nanosecond;
  /**<   UTC nanosecond. Range: 0-999,999,999.*/

  uint32_t resolution_ns;
  /**<   Resolution of the produced TSYNC hardware event in nanoseconds.
       For example, a value of 50 in this field means the event is accurate
       to plus or minus 50 ns.
  */

  uint16_t year;
  /**<   Year from system time.*/

  uint8_t month;
  /**<   Month from system time. Range: 1-12.*/

  uint8_t day;
  /**<   Day of the month from system time. Range: 1-31.*/

  uint32_t sync_period_ms;
  /**<   Periodic rate of corrections to the TSYNC hardware event,
       in milliseconds.
  */

  uint32_t elapsed_ms;
  /**<   Milliseconds since the last TSYNC hardware event correction. This value
       matches the sync_period_ms value unless there is a condition disallowing
       the correction.
  */

  int32_t timebase_correction_ns;
  /**<   The amount of timing correction applied to the TSYNC hardware timeline since the
       last correction. The value is a signed two's complement number in units of nanoseconds.
  */
}time_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup tsync_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t cdma_band_class;
  /**<   Band class of the CDMA base station whose
       timing is used for synchronization.
  */

  uint16_t cdma_channel;
  /**<   Channel of the CDMA base station whose
       timing is used for synchronization.
  */

  uint16_t cdma_sid;
  /**<   SID of the CDMA base station whose timing is used for synchronization.*/

  uint16_t cdma_nid;
  /**<   NID of the CDMA base station whose timing is used for synchronization.*/

  uint16_t cdma_pilot_pn;
  /**<   PilotPN of the CDMA base station whose timing is used for synchronization.
  */

  uint8_t leap_seconds;
  /**<   Leap seconds present in the UTC.*/

  int8_t local_timezone_offset;
  /**<   Offset specified by the base station between local time and UTC.
       The value is a signed two's complement number in units of 30 min.
  */

  uint8_t daylight_saving;
  /**<   Indicator provided by the base station indicating whether daylight saving 
       time is in effect. Values: \n
         - 0 -- No daylight saving time \n
         - 1 -- Daylight saving time
  */
}cdma_network_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup tsync_qmi_messages
    @{
  */
/** Indication Message; Provides timing information about the upcoming TSYNC hardware event. */
typedef struct {

  /* Mandatory */
  /*  Time Information */
  time_type_v01 time_info;

  /* Optional */
  /*  CDMA Network Information */
  uint8_t cdma_network_info_valid;  /**< Must be set to true if cdma_network_info is being passed */
  cdma_network_type_v01 cdma_network_info;
}qmi_tsync_event_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup tsync_qmi_msg_ids
    @{
  */
#define QMI_TSYNC_ENABLE_REQ_V01 0x0020
#define QMI_TSYNC_ENABLE_RESP_V01 0x0020
#define QMI_TSYNC_GET_STATE_REQ_V01 0x0021
#define QMI_TSYNC_GET_STATE_RESP_V01 0x0021
#define QMI_TSYNC_STATE_REPORT_IND_IND_V01 0x0022
#define QMI_TSYNC_EVENT_REPORT_IND_IND_V01 0x0023
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro tsync_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type tsync_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define tsync_get_service_object_v01( ) \
          tsync_get_service_object_internal_v01( \
            TSYNC_V01_IDL_MAJOR_VERS, TSYNC_V01_IDL_MINOR_VERS, \
            TSYNC_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

