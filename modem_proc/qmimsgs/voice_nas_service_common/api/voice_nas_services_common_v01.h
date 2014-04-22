#ifndef VOICE_NAS_SERVICES_COMMON_SERVICE_01_H
#define VOICE_NAS_SERVICES_COMMON_SERVICE_01_H
/**
  @file voice_nas_services_common_v01.h

  @brief This is the public header file which defines the voice_nas_services_common service Data structures.

  This header file defines the types and structures that were defined in
  voice_nas_services_common. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.mpss/4.3.4/voice_nas_service_common/api/voice_nas_services_common_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Tue Jun 27 2017 (Spin 0)
   From IDL File: voice_nas_services_common_v01.idl */

/** @defgroup voice_nas_services_common_qmi_consts Constant values defined in the IDL */
/** @defgroup voice_nas_services_common_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup voice_nas_services_common_qmi_enums Enumerated types used in QMI messages */
/** @defgroup voice_nas_services_common_qmi_messages Structures sent as QMI messages */
/** @defgroup voice_nas_services_common_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup voice_nas_services_common_qmi_accessor Accessor for QMI service object */
/** @defgroup voice_nas_services_common_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup voice_nas_services_common_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define VOICE_NAS_SERVICES_COMMON_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define VOICE_NAS_SERVICES_COMMON_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define VOICE_NAS_SERVICES_COMMON_V01_IDL_TOOL_VERS 0x06

/**
    @}
  */

/** @addtogroup voice_nas_services_common_qmi_aggregates
    @{
  */
/** 
 */
typedef struct {

  /*  MCC */
  uint16_t mcc;
  /**<   A 16-bit integer representation of MCC. Range: 0 to 999.
  */

  /*  MNC */
  uint16_t mnc;
  /**<   A 16-bit integer representation of MNC. Range: 0 to 999.
  */
}nas_plmn_id_type_v01;  /* Type */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 

/*Extern Definition of Type Table Object*/
/*THIS IS AN INTERNAL OBJECT AND SHOULD ONLY*/
/*BE ACCESSED BY AUTOGENERATED FILES*/
extern const qmi_idl_type_table_object voice_nas_services_common_qmi_idl_type_table_object_v01;


#ifdef __cplusplus
}
#endif
#endif

