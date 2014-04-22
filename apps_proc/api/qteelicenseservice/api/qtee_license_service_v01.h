#ifndef QTEELICENSESERVICE_SERVICE_01_H
#define QTEELICENSESERVICE_SERVICE_01_H
/**
  @file QTEE_license_service_v01.h

  @brief This is the public header file which defines the qteelicenseservice service Data structures.

  This header file defines the types and structures that were defined in
  qteelicenseservice. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2018 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.



  $Header: //components/rel/qmimsgs.tx/1.0/qteelicenseservice/api/qtee_license_service_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Fri Jul  6 2018 (Spin 0)
   From IDL File: QTEE_license_service_v01.idl */

/** @defgroup qteelicenseservice_qmi_consts Constant values defined in the IDL */
/** @defgroup qteelicenseservice_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup qteelicenseservice_qmi_enums Enumerated types used in QMI messages */
/** @defgroup qteelicenseservice_qmi_messages Structures sent as QMI messages */
/** @defgroup qteelicenseservice_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup qteelicenseservice_qmi_accessor Accessor for QMI service object */
/** @defgroup qteelicenseservice_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup qteelicenseservice_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define QTEELICENSESERVICE_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define QTEELICENSESERVICE_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define QTEELICENSESERVICE_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define QTEELICENSESERVICE_V01_MAX_MESSAGE_ID 0x0022
/**
    @}
  */


/** @addtogroup qteelicenseservice_qmi_consts
    @{
  */

/**  Enumeration for the subsystem identifier types
 Enumeration for the request results types
 Enumeration for the request results types

 Maximum string length for license check request.  */
#define QMI_QTEELS_LICENSE_CHECK_REQ_LENGTH_V01 1024

/**  Maximum string length for license check response.  */
#define QMI_QTEELS_LICENSE_CHECK_IND_RESP_LENGTH_V01 1024

/**  Maximum string length for the feature status.  */
#define QMI_QTEELS_FEATURE_REPORT_REQ_LENGTH_V01 1024
/**
    @}
  */

/** @addtogroup qteelicenseservice_qmi_enums
    @{
  */
typedef enum {
  QTEELS_QMI_SS_IDENTIFIER_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QTEELS_QMI_CLIENT_INSTANCE_APSS_V01 = 0, /**<  Client is APSS  */
  QTEELS_QMI_CLIENT_INSTANCE_MPSS_V01 = 1, /**<  Client is MPSS  */
  QTEELS_QMI_CLIENT_INSTANCE_WCNSS_V01 = 2, /**<  Client is WCNSS  */
  QTEELS_QMI_CLIENT_INSTANCE_ADSP_V01 = 3, /**<  Client is ADSP  */
  QTEELS_QMI_CLIENT_INSTANCE_MDM_V01 = 4, /**<  Client is MDM  */
  QTEELS_QMI_CLIENT_INSTANCE_QSC_V01 = 5, /**<  Client is QSC  */
  QTEELS_QMI_SS_IDENTIFIER_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qteels_qmi_ss_identifier_enum_type_v01;
/**
    @}
  */

/** @addtogroup qteelicenseservice_qmi_enums
    @{
  */
typedef enum {
  QTEELS_QMI_REQUEST_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QTEELS_STATUS_REQUEST_SERVICED_V01 = 0, /**<  Request was serviced  */
  QTEELS_STATUS_HAVEN_TA_BUSY_V01 = 1, /**<  Haven TA busy, Try again  */
  QTEELS_STATUS_REQUEST_LOST_V01 = 2, /**<  Request was lost, Try again  */
  QTEELS_STATUS_REQUEST_NOT_SECURED_PROPERLY_V01 = 3, /**<  Request was not secured properly, Fix it and Try again  */
  QTEELS_STATUS_REQUEST_NOT_ENCODED_PROPERLY_V01 = 4, /**<  Request was not encoded properly, Fix it and try again  */
  QTEELS_STATUS_HAVEN_TA_NOT_AVAILABLE_V01 = 5, /**<  Haven TA not available */
  QTEELS_STATUS_REQUEST_NOT_SERVICED_V01 = 6, /**<  For all other errors */
  QTEELS_QMI_REQUEST_STATUS_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qteels_qmi_request_status_enum_type_v01;
/**
    @}
  */

/** @addtogroup qteelicenseservice_qmi_enums
    @{
  */
typedef enum {
  QTEELS_QMI_REQUEST_RESP_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  REQUEST_RECEIVED_V01 = 0, /**<  Request was received  */
  REQUEST_RECEIVED_AND_MEMORY_FULL_V01 = 1, /**<  Request received but memory is full, Try again  */
  REQUEST_NOT_SECURED_V01 = 2, /**<  Request was not secured, Fix it and Try again  */
  REQUEST_RECEIVED_AND_NOT_SERVICED_V01 = 3, /**<  For all other errors */
  QTEELS_QMI_REQUEST_RESP_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qteels_qmi_request_resp_enum_type_v01;
/**
    @}
  */

/** @addtogroup qteelicenseservice_qmi_messages
    @{
  */
/** Request Message; Check if license is available. */
typedef struct {

  /* Mandatory */
  /*  Client ID */
  qteels_qmi_ss_identifier_enum_type_v01 ss_client_id;
  /**<   Client ID. Values:\n
      - QTEELS_QMI_CLIENT_INSTANCE_APSS (0) --  Client is APSS 
      - QTEELS_QMI_CLIENT_INSTANCE_MPSS (1) --  Client is MPSS 
      - QTEELS_QMI_CLIENT_INSTANCE_WCNSS (2) --  Client is WCNSS 
      - QTEELS_QMI_CLIENT_INSTANCE_ADSP (3) --  Client is ADSP 
      - QTEELS_QMI_CLIENT_INSTANCE_MDM (4) --  Client is MDM 
      - QTEELS_QMI_CLIENT_INSTANCE_QSC (5) --  Client is QSC 
 */

  /* Mandatory */
  /*  transactionID */
  uint16_t transactionID;
  /**<   transactionID for query.*/

  /* Mandatory */
  /*  securedQuery */
  uint8_t securedQuery;
  /**<   securedQuery set it to true if query is encrypted and secured against replay attack.*/

  /* Mandatory */
  /*  LicenseCheckReq */
  uint32_t licenseCheckReq_len;  /**< Must be set to # of elements in licenseCheckReq */
  uint8_t licenseCheckReq[QMI_QTEELS_LICENSE_CHECK_REQ_LENGTH_V01];
  /**<   Encrypted License Check Request for query.*/
}qmi_qteelicenseservice_license_check_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qteelicenseservice_qmi_messages
    @{
  */
/** Response Message; Check if license is available. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  uint8_t license_check_resp_valid;  /**< Must be set to true if license_check_resp is being passed */
  qteels_qmi_request_resp_enum_type_v01 license_check_resp;
  /**<   License Check Request Response. Values:\n
      - REQUEST_RECEIVED (0) --  Request was received 
      - REQUEST_RECEIVED_AND_MEMORY_FULL (1) --  Request received but memory is full, Try again 
      - REQUEST_NOT_SECURED (2) --  Request was not secured, Fix it and Try again 
      - REQUEST_RECEIVED_AND_NOT_SERVICED (3) --  For all other errors
 */
}qmi_qteelicenseservice_license_check_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qteelicenseservice_qmi_messages
    @{
  */
/** Indication Message; Indicates whether the check license request was serviced. */
typedef struct {

  /* Mandatory */
  /*  Request Status */
  qteels_qmi_request_status_enum_type_v01 status;
  /**<   Status of the check license request. Values: \n
      - QTEELS_STATUS_REQUEST_SERVICED (0) --  Request was serviced 
      - QTEELS_STATUS_HAVEN_TA_BUSY (1) --  Haven TA busy, Try again 
      - QTEELS_STATUS_REQUEST_LOST (2) --  Request was lost, Try again 
      - QTEELS_STATUS_REQUEST_NOT_SECURED_PROPERLY (3) --  Request was not secured properly, Fix it and Try again 
      - QTEELS_STATUS_REQUEST_NOT_ENCODED_PROPERLY (4) --  Request was not encoded properly, Fix it and try again 
      - QTEELS_STATUS_HAVEN_TA_NOT_AVAILABLE (5) --  Haven TA not available
      - QTEELS_STATUS_REQUEST_NOT_SERVICED (6) --  For all other errors
 */

  /* Mandatory */
  /*  transactionID */
  uint16_t transactionID;
  /**<   transactionID for query.*/

  /* Mandatory */
  /*  securedResponse */
  uint8_t securedResponse;
  /**<   securedResponse set it to true if response is encrypted and secured against replay attack.*/

  /* Mandatory */
  /*  licenseCheckIndResp */
  uint32_t licenseCheckIndResp_len;  /**< Must be set to # of elements in licenseCheckIndResp */
  uint8_t licenseCheckIndResp[QMI_QTEELS_LICENSE_CHECK_IND_RESP_LENGTH_V01];
  /**<   Encrypted License Check response.*/
}qmi_qteelicenseservice_license_check_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qteelicenseservice_qmi_messages
    @{
  */
/** Request Message; Updates the feature status. */
typedef struct {

  /* Mandatory */
  /*  Client ID */
  qteels_qmi_ss_identifier_enum_type_v01 ss_client_id;
  /**<   Client ID. Values:\n
      - QTEELS_QMI_CLIENT_INSTANCE_APSS (0) --  Client is APSS 
      - QTEELS_QMI_CLIENT_INSTANCE_MPSS (1) --  Client is MPSS 
      - QTEELS_QMI_CLIENT_INSTANCE_WCNSS (2) --  Client is WCNSS 
      - QTEELS_QMI_CLIENT_INSTANCE_ADSP (3) --  Client is ADSP 
      - QTEELS_QMI_CLIENT_INSTANCE_MDM (4) --  Client is MDM 
      - QTEELS_QMI_CLIENT_INSTANCE_QSC (5) --  Client is QSC 
 */

  /* Mandatory */
  /*  transactionID */
  uint16_t transactionID;
  /**<   transactionID for query.*/

  /* Mandatory */
  /*  securedQuery */
  uint8_t securedQuery;
  /**<   securedQuery set it to true if query is encrypted and secured against replay attack.*/

  /* Mandatory */
  /*  Status */
  uint32_t featureReport_len;  /**< Must be set to # of elements in featureReport */
  uint8_t featureReport[QMI_QTEELS_FEATURE_REPORT_REQ_LENGTH_V01];
  /**<   Encrypted blob to update feature status */
}qmi_qteelicenseservice_feature_report_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qteelicenseservice_qmi_messages
    @{
  */
/** Response Message; Updates the feature status. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  uint8_t feature_report_resp_valid;  /**< Must be set to true if feature_report_resp is being passed */
  qteels_qmi_request_resp_enum_type_v01 feature_report_resp;
  /**<   Feature report Request Response. Values:\n
      - REQUEST_RECEIVED (0) --  Request was received 
      - REQUEST_RECEIVED_AND_MEMORY_FULL (1) --  Request received but memory is full, Try again 
      - REQUEST_NOT_SECURED (2) --  Request was not secured, Fix it and Try again 
      - REQUEST_RECEIVED_AND_NOT_SERVICED (3) --  For all other errors
 */
}qmi_qteelicenseservice_feature_report_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qteelicenseservice_qmi_messages
    @{
  */
/** Indication Message; Indicates whether the feature report update request was serviced. */
typedef struct {

  /* Mandatory */
  /*  Request Status */
  qteels_qmi_request_status_enum_type_v01 status;
  /**<   Status of the restart request. Values:\n
      - QTEELS_STATUS_REQUEST_SERVICED (0) --  Request was serviced 
      - QTEELS_STATUS_HAVEN_TA_BUSY (1) --  Haven TA busy, Try again 
      - QTEELS_STATUS_REQUEST_LOST (2) --  Request was lost, Try again 
      - QTEELS_STATUS_REQUEST_NOT_SECURED_PROPERLY (3) --  Request was not secured properly, Fix it and Try again 
      - QTEELS_STATUS_REQUEST_NOT_ENCODED_PROPERLY (4) --  Request was not encoded properly, Fix it and try again 
      - QTEELS_STATUS_HAVEN_TA_NOT_AVAILABLE (5) --  Haven TA not available
      - QTEELS_STATUS_REQUEST_NOT_SERVICED (6) --  For all other errors
 */
}qmi_qteelicenseservice_feature_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qteelicenseservice_qmi_messages
    @{
  */
/** Indication Message; Informs client that sync is completed on HLOS side. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_qteelicenseservice_sync_completed_broadcast_ind_v01;

  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_QTEELS_FEATURE_REPORT_V01 
//#define REMOVE_QMI_QTEELS_FEATURE_REPORT_IND_V01 
//#define REMOVE_QMI_QTEELS_LICENSE_CHECK_V01 
//#define REMOVE_QMI_QTEELS_LICENSE_CHECK_IND_V01 
//#define REMOVE_QMI_QTEELS_SYNC_COMPLETE_BROADCAST_IND_V01 

/*Service Message Definition*/
/** @addtogroup qteelicenseservice_qmi_msg_ids
    @{
  */
#define QMI_QTEELS_LICENSE_CHECK_REQ_V01 0x0020
#define QMI_QTEELS_LICENSE_CHECK_RESP_V01 0x0020
#define QMI_QTEELS_LICENSE_CHECK_IND_V01 0x0020
#define QMI_QTEELS_FEATURE_REPORT_REQ_V01 0x0021
#define QMI_QTEELS_FEATURE_REPORT_RESP_V01 0x0021
#define QMI_QTEELS_FEATURE_REPORT_IND_V01 0x0021
#define QMI_QTEELS_SYNC_COMPLETE_BROADCAST_IND_V01 0x0022
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro qteelicenseservice_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type qteelicenseservice_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define qteelicenseservice_get_service_object_v01( ) \
          qteelicenseservice_get_service_object_internal_v01( \
            QTEELICENSESERVICE_V01_IDL_MAJOR_VERS, QTEELICENSESERVICE_V01_IDL_MINOR_VERS, \
            QTEELICENSESERVICE_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

