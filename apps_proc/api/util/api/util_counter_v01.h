#ifndef UTIL_SERVICE_01_H
#define UTIL_SERVICE_01_H
/**
  @file util_counter_v01.h

  @brief This is the public header file which defines the util service Data structures.

  This header file defines the types and structures that were defined in
  util. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Wed Apr  6 2022 (Spin 0)
   From IDL File: util_counter_v01.idl */

/** @defgroup util_qmi_consts Constant values defined in the IDL */
/** @defgroup util_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup util_qmi_enums Enumerated types used in QMI messages */
/** @defgroup util_qmi_messages Structures sent as QMI messages */
/** @defgroup util_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup util_qmi_accessor Accessor for QMI service object */
/** @defgroup util_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup util_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define UTIL_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define UTIL_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define UTIL_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define UTIL_V01_MAX_MESSAGE_ID 0x0001
/**
    @}
  */


/** @addtogroup util_qmi_consts
    @{
  */
/**
    @}
  */

/** @addtogroup util_qmi_messages
    @{
  */
/** Request Message; Request flash counters from modem */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_util_flash_counter_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup util_qmi_messages
    @{
  */
/** Response Message; Request flash counters from modem */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  uint8_t fs_counter_valid;  /**< Must be set to true if fs_counter is being passed */
  uint32_t fs_counter;
  /**<   This fs_counter variable will fetch the how many writes are happened to modem EFS region through storage API's >*/

  /* Optional */
  uint8_t fs_block_count_valid;  /**< Must be set to true if fs_block_count is being passed */
  uint32_t fs_block_count;
  /**<   This fs_block_count variable will fetch the how many blocks are present in modem EFS region through storage API's >*/

  /* Optional */
  uint8_t fs_page_count_valid;  /**< Must be set to true if fs_page_count is being passed */
  uint32_t fs_page_count;
  /**<   This fs_page_count variable will fetch the how many pages are present for particular block in modem EFS region through storage API's >*/
}qmi_util_flash_counter_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_UTIL_FlASH_COUNTER_V01

/*Service Message Definition*/
/** @addtogroup util_qmi_msg_ids
    @{
  */
#define QMI_UTIL_FlASH_COUNTER_REQ_V01 0x0001
#define QMI_UTIL_FlASH_COUNTER_RESP_V01 0x0001
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro util_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type util_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define util_get_service_object_v01( ) \
          util_get_service_object_internal_v01( \
            UTIL_V01_IDL_MAJOR_VERS, UTIL_V01_IDL_MINOR_VERS, \
            UTIL_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

