#ifndef LOC_IOT_SERVICE_02_H
#define LOC_IOT_SERVICE_02_H
/**
  @file location_service_iot_v02.h

  @brief This is the public header file which defines the loc_iot service Data structures.

  This header file defines the types and structures that were defined in
  loc_iot. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2010-2023 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.tx/1.0/loc_iot/api/location_service_iot_v02.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Mon Apr 10 2023 (Spin 0)
   From IDL File: location_service_iot_v02.idl */

/** @defgroup loc_iot_qmi_consts Constant values defined in the IDL */
/** @defgroup loc_iot_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup loc_iot_qmi_enums Enumerated types used in QMI messages */
/** @defgroup loc_iot_qmi_messages Structures sent as QMI messages */
/** @defgroup loc_iot_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup loc_iot_qmi_accessor Accessor for QMI service object */
/** @defgroup loc_iot_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup loc_iot_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define LOC_IOT_V02_IDL_MAJOR_VERS 0x02
/** Revision Number of the IDL used to generate this file */
#define LOC_IOT_V02_IDL_MINOR_VERS 0x0E
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define LOC_IOT_V02_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define LOC_IOT_V02_MAX_MESSAGE_ID 0x006A
/**
    @}
  */


/** @addtogroup loc_iot_qmi_consts
    @{
  */

/**  Maximum string length for the client string ID  */
#define QMI_LOC_MAX_CLIENT_ID_STRING_LENGTH_V02 4

/**  Maximum length of the list containing the SVs that were used to generate
     a position report.  */
#define QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02 80

/**  Maximum expanded length of the list containing the SVs that were used to generate
     a position report.  */
#define QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02 176

/**  Maximum number of GNSS signal type.  */
#define QMI_LOC_MAX_GNSS_SIGNAL_TYPE_V02 19

/**  East, north, up array length.  */
#define QMI_LOC_ENU_ARRAY_LENGTH_V02 3

/**  Maximum length of the list containing station IDs providing DGNSS 
     correction.  */
#define QMI_LOC_DGNSS_STATION_ID_ARRAY_LENGTH_V02 3

/**  Maximum number of NTP servers sent out with this event. */
#define QMI_LOC_MAX_NTP_SERVERS_V02 3

/**  Maximum URL length accepted by the location engine.  */
#define QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 255

/**  Maximum number of predicted orbits servers supported in the location
     engine.  */
#define QMI_LOC_MAX_PREDICTED_ORBITS_SERVERS_V02 3

/**  Maximum GNSS measurement engine firmware version string length.  */
#define QMI_LOC_GNSS_ME_VERSION_STRING_MAX_LENGTH_V02 127

/**  Maximum GNSS measurement engine hosted software version string length.  */
#define QMI_LOC_GNSS_HOSTED_SW_VERSION_STRING_MAX_LENGTH_V02 127

/**  Maximum GNSS measurement engine full version string length.  */
#define QMI_LOC_GNSS_SW_VERSION_STRING_MAX_LENGTH_V02 255

/**  APS buffer data max length  */
#define QMI_LOC_APS_BUFFER_DATA_MAX_LENGTH_V02 1024

/**  Maximum number of motion states used by the geofence engine.  */
#define QMI_LOC_GEOFENCE_MAX_MOTION_STATES_V02 20

/**  Maximum number of entries returned from a batch in each indication.  */
#define QMI_LOC_READ_FROM_BATCH_MAX_SIZE_V02 5

/**  Maximum length that can be injected.   */
#define QMI_LOC_MAX_GTP_WWAN_CLIENT_DOWNLOADED_DATA_LEN_V02 512

/**  Maximum part length that can be injected; the client must select
     the minimum of either this constant or the maxPartSize field in
     the QMI_LOC_EVENT_INJECT_PREDICTED_ORBITS_REQ_IND indication.  */
#define QMI_LOC_MAX_XTRA_PART_LEN_V02 1024
#define QMI_LOC_SUPPORTED_FEATURE_LENGTH_V02 100

/**  Maximum number of bands in LTE.  */
#define QMI_LOC_RF_BAND_LENGTH_V02 16

/**  Maximum number of BS information in the BS list. 
  */
#define QMI_LOC_FDCL_BS_LIST_MAX_SIZE_V02 10

/**  Maximum bytes of BS OBS data scan data. 
  */
#define QMI_LOC_SCAN_DATA_MAX_SIZE_V02 4000

/**  FDCL cell-position list length.  */
#define QMI_LOC_FDCL_CELL_POS_LIST_LENGTH_V02 10

/**  Restricted region list length.  */
#define QMI_LOC_RESTRICTED_REGIONS_LIST_MAX_SIZE_V02 20
#define QMI_LOC_INJ_DATA_MAX_SIZE_V02 3500
#define QMI_LOC_INJECT_FDCL_DATA_ERROR_MSG_LEN_V02 255

/**  Maximum NMEA string length.  */
#define QMI_LOC_NMEA_STRING_MAX_LENGTH_V02 200

/**  Maximum expanded NMEA string length.  */
#define QMI_LOC_EXPANDED_NMEA_STRING_MAX_LENGTH_V02 4095

/**  Maximum number of satellites in the satellite report.  */
#define QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02 80

/**  Maximum bytes that buffer can have. */
#define QMI_LOC_DATA_BUFF_MAX_SIZE_V02 4000
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCSTATUSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SUCCESS_V02 = 0, /**<  Completed successfully \n  */
  eQMI_LOC_GENERAL_FAILURE_V02 = 1, /**<  Failed because of a general failure \n  */
  eQMI_LOC_UNSUPPORTED_V02 = 2, /**<  Failed because it is not supported \n  */
  eQMI_LOC_INVALID_PARAMETER_V02 = 3, /**<  Failed because it contained invalid parameters \n  */
  eQMI_LOC_ENGINE_BUSY_V02 = 4, /**<  Failed because the engine is busy \n  */
  eQMI_LOC_PHONE_OFFLINE_V02 = 5, /**<  Failed because the phone is offline \n  */
  eQMI_LOC_TIMEOUT_V02 = 6, /**<  Failed because it has timed out \n  */
  eQMI_LOC_CONFIG_NOT_SUPPORTED_V02 = 7, /**<  Failed because an undefined configuration was requested \n  */
  eQMI_LOC_INSUFFICIENT_MEMORY_V02 = 8, /**<  Failed because the engine could not allocate sufficient memory for the request \n  */
  eQMI_LOC_MAX_GEOFENCE_PROGRAMMED_V02 = 9, /**<  Failed because the maximum number of geofences are already programmed \n  */
  eQMI_LOC_XTRA_VERSION_CHECK_FAILURE_V02 = 10, /**<  Location service failed because of an XTRA version-based file format check failure \n */
  eQMI_LOC_GNSS_DISABLED_V02 = 11, /**<  Failed because the location service is disabled \n  */
  eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE_V02 = 12, /**<  Failed because the GNSS engine was not loaded when the request was made  */
  QMILOCSTATUSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocStatusEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Generic response definition; informs
                    clients whether their message was accepted for further 
                    processing or rejected. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmiLocGenReqMsgT_v02;

  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Response Message; Generic response definition; informs
                    clients whether their message was accepted for further 
                    processing or rejected. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qmiLocGenRespMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Generic response definition; informs
                    clients whether their message was accepted for further 
                    processing or rejected. */
typedef struct {

  /* Mandatory */
  /*  Success/Failure Status of QMI Request */
  qmiLocStatusEnumT_v02 status;
  /**<   Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */
}qmiLocGenReqStatusIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Informs the service of the minor revision of the interface
                    definition that the control point implements. */
typedef struct {

  /* Mandatory */
  /*  Revision */
  uint32_t revision;
  /**<   Revision that the control point is using. */
}qmiLocInformClientRevisionReqMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint64_t qmiLocEventRegMaskT_v02;
#define QMI_LOC_EVENT_MASK_POSITION_REPORT_V02 ((qmiLocEventRegMaskT_v02)0x00000001ull) /**<  Position report event indications. \n  */
#define QMI_LOC_EVENT_MASK_GNSS_SV_INFO_V02 ((qmiLocEventRegMaskT_v02)0x00000002ull) /**<  Satellite report event indications; these reports are sent at a 1 Hz rate.\n  */
#define QMI_LOC_EVENT_MASK_NMEA_V02 ((qmiLocEventRegMaskT_v02)0x00000004ull) /**<  NMEA reports for position and satellites in view; the report is at a 1 Hz rate. \n */
#define QMI_LOC_EVENT_MASK_INJECT_TIME_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000008ull) /**<  Time injection request event indications.\n  */
#define QMI_LOC_EVENT_MASK_INJECT_PREDICTED_ORBITS_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000010ull) /**<  Predicted orbits request event indications. \n */
#define QMI_LOC_EVENT_MASK_INJECT_POSITION_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000020ull) /**<  Control point must enable this mask to receive position injection request
       event indications. \n */
#define QMI_LOC_EVENT_MASK_ENGINE_STATE_V02 ((qmiLocEventRegMaskT_v02)0x00000040ull) /**<  Engine state report event indications. \n */
#define QMI_LOC_EVENT_MASK_FIX_SESSION_STATE_V02 ((qmiLocEventRegMaskT_v02)0x00000080ull) /**<  Fix session status report event indications.\n  */
#define QMI_LOC_EVENT_MASK_GEOFENCE_GEN_ALERT_V02 ((qmiLocEventRegMaskT_v02)0x00000100ull) /**<  Geofence alerts to inform the client of the changes that might
       affect a geofence, for example, if GPS is turned off or if the network is
       unavailable.\n  */
#define QMI_LOC_EVENT_MASK_GEOFENCE_BREACH_NOTIFICATION_V02 ((qmiLocEventRegMaskT_v02)0x00000200ull) /**<  Geofence is breached; generated when UE enters
       or leaves the perimeter of a geofence. This breach report is for a single
       geofence.\n  */
#define QMI_LOC_EVENT_MASK_BATCH_FULL_NOTIFICATION_V02 ((qmiLocEventRegMaskT_v02)0x00000400ull) /**<  Batch is full; the location engine sends this event to notify of batch full
       for ongoing batching session.\n  */
#define QMI_LOC_EVENT_MASK_LIVE_BATCHED_POSITION_REPORT_V02 ((qmiLocEventRegMaskT_v02)0x00000800ull) /**<  Position report indications along with an ongoing batching session. The location engine sends
       this event to notify the batched position report while a batching session
       is ongoing.\n  */
#define QMI_LOC_EVENT_MASK_GEOFENCE_BATCH_BREACH_NOTIFICATION_V02 ((qmiLocEventRegMaskT_v02)0x00001000ull) /**<  Geofence is breached; generated when UE enters
       or leaves the perimeter of a geofence. This breach notification is for
       multiple geofences. Breaches from multiple geofences are batched and
       sent in the same notification. \n   */
#define QMI_LOC_EVENT_MASK_GEOFENCE_BATCH_DWELL_NOTIFICATION_V02 ((qmiLocEventRegMaskT_v02)0x00002000ull) /**<  Geofence is dwelled; generated when a UE enters
       or leaves the perimeter of a geofence and dwells inside or outside for a specified time.
       This dwell notification is for multiple geofences. Dwells from multiple geofences are batched and
       sent in the same notification.  */
#define QMI_LOC_EVENT_MASK_BATCHING_STATUS_V02 ((qmiLocEventRegMaskT_v02)0x00004000ull) /**<  Asynchronous events related to batching. \n */
#define QMI_LOC_EVENT_MASK_GNSS_ONLY_POSITION_REPORT_V02 ((qmiLocEventRegMaskT_v02)0x000008000ull) /**<  Position report event indications that contain a GNSS only position.\n  */
#define QMI_LOC_EVENT_MASK_FDCL_SERVICE_REQ_V02 ((qmiLocEventRegMaskT_v02)0x000010000ull) /**<  FDCL service request. \n */
#define QMI_LOC_EVENT_MASK_BS_OBS_DATA_SERVICE_REQ_V02 ((qmiLocEventRegMaskT_v02)0x0000020000ull) /**<  Base station observed data service request.  */
#define QMI_LOC_EVENT_MASK_INITIATE_POSITION_REQ_V02 ((qmiLocEventRegMaskT_v02)0x0000040000ull) /**<  Control point must enable this mask to get position 
  intialization request from service \n */
/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Registers for events from the location subsystem. */
typedef struct {

  /* Mandatory */
  /*  Event Registration Mask */
  qmiLocEventRegMaskT_v02 eventRegMask;
  /**<   Events that the control point is interested in receiving.
 Valid bitmasks: \n
      - QMI_LOC_EVENT_MASK_POSITION_REPORT (0x00000001) --  Position report event indications. \n 
      - QMI_LOC_EVENT_MASK_GNSS_SV_INFO (0x00000002) --  Satellite report event indications; these reports are sent at a 1 Hz rate.\n 
      - QMI_LOC_EVENT_MASK_NMEA (0x00000004) --  NMEA reports for position and satellites in view; the report is at a 1 Hz rate. \n
      - QMI_LOC_EVENT_MASK_INJECT_TIME_REQ (0x00000008) --  Time injection request event indications.\n 
      - QMI_LOC_EVENT_MASK_INJECT_PREDICTED_ORBITS_REQ (0x00000010) --  Predicted orbits request event indications. \n
      - QMI_LOC_EVENT_MASK_INJECT_POSITION_REQ (0x00000020) --  Control point must enable this mask to receive position injection request
       event indications. \n
      - QMI_LOC_EVENT_MASK_ENGINE_STATE (0x00000040) --  Engine state report event indications. \n
      - QMI_LOC_EVENT_MASK_FIX_SESSION_STATE (0x00000080) --  Fix session status report event indications.\n 
      - QMI_LOC_EVENT_MASK_GEOFENCE_GEN_ALERT (0x00000100) --  Geofence alerts to inform the client of the changes that might
       affect a geofence, for example, if GPS is turned off or if the network is
       unavailable.\n 
      - QMI_LOC_EVENT_MASK_GEOFENCE_BREACH_NOTIFICATION (0x00000200) --  Geofence is breached; generated when UE enters
       or leaves the perimeter of a geofence. This breach report is for a single
       geofence.\n 
      - QMI_LOC_EVENT_MASK_BATCH_FULL_NOTIFICATION (0x00000400) --  Batch is full; the location engine sends this event to notify of batch full
       for ongoing batching session.\n 
      - QMI_LOC_EVENT_MASK_LIVE_BATCHED_POSITION_REPORT (0x00000800) --  Position report indications along with an ongoing batching session. The location engine sends
       this event to notify the batched position report while a batching session
       is ongoing.\n 
      - QMI_LOC_EVENT_MASK_GEOFENCE_BATCH_BREACH_NOTIFICATION (0x00001000) --  Geofence is breached; generated when UE enters
       or leaves the perimeter of a geofence. This breach notification is for
       multiple geofences. Breaches from multiple geofences are batched and
       sent in the same notification. \n  
      - QMI_LOC_EVENT_MASK_GEOFENCE_BATCH_DWELL_NOTIFICATION (0x00002000) --  Geofence is dwelled; generated when a UE enters
       or leaves the perimeter of a geofence and dwells inside or outside for a specified time.
       This dwell notification is for multiple geofences. Dwells from multiple geofences are batched and
       sent in the same notification. 
      - QMI_LOC_EVENT_MASK_BATCHING_STATUS (0x00004000) --  Asynchronous events related to batching. \n
      - QMI_LOC_EVENT_MASK_GNSS_ONLY_POSITION_REPORT (0x000008000) --  Position report event indications that contain a GNSS only position.\n 
      - QMI_LOC_EVENT_MASK_FDCL_SERVICE_REQ (0x000010000) --  FDCL service request. \n
      - QMI_LOC_EVENT_MASK_BS_OBS_DATA_SERVICE_REQ (0x0000020000) --  Base station observed data service request. 
      - QMI_LOC_EVENT_MASK_INITIATE_POSITION_REQ (0x0000040000) --  Control point must enable this mask to get position 
  intialization request from service \n

 Register multiple events by OR'ing the individual masks and
 sending them in this TLV. All unused bits in this mask must be set to 0.
 */

  /* Optional */
  /*  Client Identification String */
  uint8_t clientStrId_valid;  /**< Must be set to true if clientStrId is being passed */
  char clientStrId[QMI_LOC_MAX_CLIENT_ID_STRING_LENGTH_V02 + 1];
  /**<   String identification of this client, used internally for debugging purposes.
       If not provided, nothing is printed for client identification.\n
       - Type -- NULL-terminated string \n
       - Maximum string length (including NULL-terminator) -- 5
       */
}qmiLocRegEventsReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCFIXRECURRENCEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_RECURRENCE_PERIODIC_V02 = 1, /**<  Request periodic position fixes \n  */
  eQMI_LOC_RECURRENCE_SINGLE_V02 = 2, /**<  Request a single position fix  */
  QMILOCFIXRECURRENCEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocFixRecurrenceEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCACCURACYLEVELENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_ACCURACY_LOW_V02 = 1, /**<  Low accuracy \n */
  eQMI_LOC_ACCURACY_MED_V02 = 2, /**<  Medium accuracy \n */
  eQMI_LOC_ACCURACY_HIGH_V02 = 3, /**<  High accuracy  */
  QMILOCACCURACYLEVELENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocAccuracyLevelEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCINTERMEDIATEREPORTSTATEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_INTERMEDIATE_REPORTS_ON_V02 = 1, /**<  Intermediate reports are turned on \n  */
  eQMI_LOC_INTERMEDIATE_REPORTS_OFF_V02 = 2, /**<  Intermediate reports are turned off  */
  QMILOCINTERMEDIATEREPORTSTATEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocIntermediateReportStateEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCCONFIGINCLUDEALTITUDEASSUMEDINGNSSSVINFOENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_ALTITUDE_ASSUMED_IN_GNSS_SV_INFO_ENABLED_V02 = 1, /**<  Enable altitude assumed information in GNSS SV Info event. \n */
  eQMI_LOC_ALTITUDE_ASSUMED_IN_GNSS_SV_INFO_DISABLED_V02 = 2, /**<  Disable altitude assumed information in GNSS SV Info event.  */
  QMILOCCONFIGINCLUDEALTITUDEASSUMEDINGNSSSVINFOENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocConfigIncludeAltitudeAssumedInGnssSvInfoEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCPOWERMODEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_POWER_MODE_IMPROVED_ACCURACY_V02 = 1, /**<  GNSS receiver operates in full power mode (non-DPO) \n */
  eQMI_LOC_POWER_MODE_NORMAL_V02 = 2, /**<  GNSS receiver operates with GNSS receiver autonomously
       duty cycling while meeting position criteria. \n  */
  eQMI_LOC_POWER_MODE_BACKGROUND_DEFINED_POWER_V02 = 3, /**<  GNSS receiver duty cycles at a variable rate to meet
       target power budget. The power budget is defined as 1/N
       of the GNSS full power during 1 Hz navigation.
       N = timeBetweenMeasurement/1000 \n  */
  eQMI_LOC_POWER_MODE_BACKGROUND_DEFINED_TIME_V02 = 4, /**<  GNSS receiver duty cycles at a fixed time interval. \n  */
  eQMI_LOC_POWER_MODE_BACKGROUND_KEEP_WARM_V02 = 5, /**<  GNSS receiver operates in very-low power (less than 1mA)
       duty cycling mode, to keep GNSS receiver warm for faster signal
       acquisition and tracking.  */
  QMILOCPOWERMODEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocPowerModeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocPowerModeEnumT_v02 powerMode;
  /**<   GNSS power mode option to let the client choose the power-accuracy trade-off.\n
 Values: \n
      - eQMI_LOC_POWER_MODE_IMPROVED_ACCURACY (1) --  GNSS receiver operates in full power mode (non-DPO) \n
      - eQMI_LOC_POWER_MODE_NORMAL (2) --  GNSS receiver operates with GNSS receiver autonomously
       duty cycling while meeting position criteria. \n 
      - eQMI_LOC_POWER_MODE_BACKGROUND_DEFINED_POWER (3) --  GNSS receiver duty cycles at a variable rate to meet
       target power budget. The power budget is defined as 1/N
       of the GNSS full power during 1 Hz navigation.
       N = timeBetweenMeasurement/1000 \n 
      - eQMI_LOC_POWER_MODE_BACKGROUND_DEFINED_TIME (4) --  GNSS receiver duty cycles at a fixed time interval. \n 
      - eQMI_LOC_POWER_MODE_BACKGROUND_KEEP_WARM (5) --  GNSS receiver operates in very-low power (less than 1mA)
       duty cycling mode, to keep GNSS receiver warm for faster signal
       acquisition and tracking. 
 */

  uint32_t timeBetweenMeasurement;
  /**<   Maximum time interval between measurements,
       applies to power mode BACKGROUND_DEFINED_POWER and BACKGROUND_DEFINED_TIME,
       For BACKGROUND_DEFINED_POWER, set timeBetweenMeasurement to 1000 * N; 
       timeBetweenMeasurement is ignored for the other power modes. \n
       - Units -- Milliseconds 
  */
}qmiLocPowerModeStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Initiates a GPS session. */
typedef struct {

  /* Mandatory */
  /*  Session ID */
  uint8_t sessionId;
  /**<   ID of the session as identified by the control point; the session ID
       is reported back in the position reports. The control point must
       specify the same session ID in the QMI_LOC_STOP_REQ request. \n
       - Range -- 0 to 255
  */

  /* Optional */
  /*  Recurrence Type */
  uint8_t fixRecurrence_valid;  /**< Must be set to true if fixRecurrence is being passed */
  qmiLocFixRecurrenceEnumT_v02 fixRecurrence;
  /**<   Type of session in which the control point is interested.
 If this TLV is not specified, recurrence defaults to single.
 Valid values: \n
      - eQMI_LOC_RECURRENCE_PERIODIC (1) --  Request periodic position fixes \n 
      - eQMI_LOC_RECURRENCE_SINGLE (2) --  Request a single position fix 
 */

  /* Optional */
  /*  Horizontal Accuracy Level */
  uint8_t horizontalAccuracyLevel_valid;  /**< Must be set to true if horizontalAccuracyLevel is being passed */
  qmiLocAccuracyLevelEnumT_v02 horizontalAccuracyLevel;
  /**<   Horizontal accuracy level required by the control point.
 If not specified, accuracy defaults to low.
 Valid values: \n
      - eQMI_LOC_ACCURACY_LOW (1) --  Low accuracy \n
      - eQMI_LOC_ACCURACY_MED (2) --  Medium accuracy \n
      - eQMI_LOC_ACCURACY_HIGH (3) --  High accuracy 
 */

  /* Optional */
  /*  Enable/Disable Intermediate Reports */
  uint8_t intermediateReportState_valid;  /**< Must be set to true if intermediateReportState is being passed */
  qmiLocIntermediateReportStateEnumT_v02 intermediateReportState;
  /**<   Specifies whether the control point is interested in receiving intermediate
 reports. The control point must explicitly set this field to off if it
 does not wish to receive intermediate position reports. Intermediate
 position reports are generated at 1 Hz and are on by default. If
 intermediate reports are turned on, the client receives position reports
 even if the accuracy criteria are not met. The status in such a position
 report is set to IN_PROGRESS for the control point to identify
 intermediate reports.
 Valid values: \n
      - eQMI_LOC_INTERMEDIATE_REPORTS_ON (1) --  Intermediate reports are turned on \n 
      - eQMI_LOC_INTERMEDIATE_REPORTS_OFF (2) --  Intermediate reports are turned off 
 */

  /* Optional */
  /*  Minimum Interval Between Final Position Reports */
  uint8_t minInterval_valid;  /**< Must be set to true if minInterval is being passed */
  uint32_t minInterval;
  /**<   Minimum time interval, specified by the control point, that must elapse between
       final position reports. \n
       - Units -- Milliseconds \n
       - Default -- 1000 ms
  */

  /* Optional */
  /*  Configuration for Altitude Assumed Info in GNSS SV Info Event */
  uint8_t configAltitudeAssumed_valid;  /**< Must be set to true if configAltitudeAssumed is being passed */
  qmiLocConfigIncludeAltitudeAssumedInGnssSvInfoEnumT_v02 configAltitudeAssumed;
  /**<   Configuration to include altitude assumed information in the GNSS SV Info event.
 When enabled, an additional QMI_LOC_EVENT_GNSS_SV_INFO_IND indication is sent to the control
 point that also includes the altitude assumed information.
 If not specified, the configuration defaults to enabled.
 Valid values: \n
      - eQMI_LOC_ALTITUDE_ASSUMED_IN_GNSS_SV_INFO_ENABLED (1) --  Enable altitude assumed information in GNSS SV Info event. \n
      - eQMI_LOC_ALTITUDE_ASSUMED_IN_GNSS_SV_INFO_DISABLED (2) --  Disable altitude assumed information in GNSS SV Info event. 
 */

  /* Optional */
  /*  Minimum Interval Between Intermediate Position Reports */
  uint8_t minIntermediatePositionReportInterval_valid;  /**< Must be set to true if minIntermediatePositionReportInterval is being passed */
  uint32_t minIntermediatePositionReportInterval;
  /**<   Minimum time interval (specified by the control point) for intermediate position reports,
       that between the position reports elapsed time must be longer than the interval time.
       If this optional value is not set or set to the default value (0), the intermediate position
       is reported when it is ready. \n
       - Units -- Milliseconds \n
       - Default -- 0 ms
  */

  /* Optional */
  /*  Maximum Wait Time to Get a Position Report */
  uint8_t positionReportTimeout_valid;  /**< Must be set to true if positionReportTimeout is being passed */
  uint32_t positionReportTimeout;
  /**<   Maximum time to work on each fix, specified by the control point.
       The GPS engine returns QMI_ERR_INTERNAL if a position cannot be obtained
       within the positionReportTimeout value. \n
       - Units -- Milliseconds \n
       - Default -- 255*1000 ms \n
       - Range -- 1000 - 255*1000 ms
  */

  /* Optional */
  /*  Share Position Report with Other Clients */
  uint8_t sharePosition_valid;  /**< Must be set to true if sharePosition is being passed */
  uint8_t sharePosition;
  /**<   Indicates whether to share the position report with the other QMI_LOC clients: \n
       - 0x00 (FALSE) -- Do not share  \n
       - 0x01 (TRUE) -- Share  \n
       If this optional TLV is not set, the GPS engine allows the position sharing.
  */

  /* Optional */
  /*  Report GNSS Only Position */
  uint8_t reportGnssOnlyPosition_valid;  /**< Must be set to true if reportGnssOnlyPosition is being passed */
  uint8_t reportGnssOnlyPosition;
  /**<   Requests the GPS engine to report positions that can be GNSS only or
       combined with other technologies, such as sensors. Values: \n
       - 0x00 (FALSE) -- Report GNSS only positions is disabled (default) \n
       - 0x01 (TRUE)  -- Report GNSS only positions is enabled
  */

  /* Optional */
  /*  GNSS Power Mode */
  uint8_t powerMode_valid;  /**< Must be set to true if powerMode is being passed */
  qmiLocPowerModeStructT_v02 powerMode;
  /**<   \n 
       eQMI_LOC_POWER_MODE_NORMAL is the default value when the power mode is not set by the client.
       When there are other clients with different powerMode settings, the GNSS receiver is 
	   configured with powerMode with the highest power consumption among the requesting clients.
  */

  /* Optional */
  /*  Horizontal Accuracy */
  uint8_t horizontalAccuracy_valid;  /**< Must be set to true if horizontalAccuracy is being passed */
  uint16_t horizontalAccuracy;
  /**<   Horizontal accuracy required by the control point. \n
        - Range -- 0 to 65535 \n
        - Units -- Meters */
}qmiLocStartReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Stops a GPS session. */
typedef struct {

  /* Mandatory */
  /*  Session ID */
  uint8_t sessionId;
  /**<   ID of the session that was specified in the QMI_LOC_START_REQ request.\n
       - Range -- 0 to 255 */
}qmiLocStopReqMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocPosTechMaskT_v02;
#define QMI_LOC_POS_TECH_MASK_SATELLITE_V02 ((qmiLocPosTechMaskT_v02)0x00000001) /**<  Satellites are used to generate the fix \n */
#define QMI_LOC_POS_TECH_MASK_CELLID_V02 ((qmiLocPosTechMaskT_v02)0x00000002) /**<  Cell towers are used to generate the fix \n  */
#define QMI_LOC_POS_TECH_MASK_WIFI_V02 ((qmiLocPosTechMaskT_v02)0x00000004) /**<  Wi-Fi access points are used to generate the fix \n */
#define QMI_LOC_POS_TECH_MASK_SENSORS_V02 ((qmiLocPosTechMaskT_v02)0x00000008) /**<  Sensors are used to generate the fix \n  */
#define QMI_LOC_POS_TECH_MASK_REFERENCE_LOCATION_V02 ((qmiLocPosTechMaskT_v02)0x00000010) /**<  Reference location is used to generate the fix \n */
#define QMI_LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION_V02 ((qmiLocPosTechMaskT_v02)0x00000020) /**<  Coarse position injected into the location engine is used to
        generate the fix \n */
#define QMI_LOC_POS_TECH_MASK_AFLT_V02 ((qmiLocPosTechMaskT_v02)0x00000040) /**<  AFLT is used to generate the fix \n */
#define QMI_LOC_POS_TECH_MASK_HYBRID_V02 ((qmiLocPosTechMaskT_v02)0x00000080) /**<  GNSS and network-provided measurements are used to
        generate the fix  */
/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCSESSIONSTATUSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SESS_STATUS_SUCCESS_V02 = 0, /**<  Session was successful \n */
  eQMI_LOC_SESS_STATUS_IN_PROGRESS_V02 = 1, /**<  Session is still in progress; further position reports are generated
       until either the fix criteria specified by the client are met or the
       client response timeout occurs \n */
  eQMI_LOC_SESS_STATUS_GENERAL_FAILURE_V02 = 2, /**<  Session failed \n */
  eQMI_LOC_SESS_STATUS_TIMEOUT_V02 = 3, /**<  Fix request failed because the session timed out \n */
  eQMI_LOC_SESS_STATUS_USER_END_V02 = 4, /**<  Fix request failed because the session was ended by the user \n  */
  eQMI_LOC_SESS_STATUS_BAD_PARAMETER_V02 = 5, /**<  Fix request failed due to bad parameters in the request \n */
  eQMI_LOC_SESS_STATUS_PHONE_OFFLINE_V02 = 6, /**<  Fix request failed because the phone is offline \n */
  eQMI_LOC_SESS_STATUS_ENGINE_LOCKED_V02 = 7, /**<  Fix request failed because the engine is locked \n */
  eQMI_LOC_SESS_STATUS_GNSS_SERVICE_NOT_AVAILABLE_V02 = 8, /**<  Fix request failed because the GNSS engine was not available for service   */
  QMILOCSESSIONSTATUSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSessionStatusEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t gpsWeek;
  /**<   Current GPS week as calculated from midnight, Jan. 6, 1980. \n
       - Units -- Weeks */

  uint32_t gpsTimeOfWeekMs;
  /**<   Amount of time into the current GPS week. \n
       - Units -- Milliseconds */
}qmiLocGPSTimeStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  float PDOP;
  /**<   Position dilution of precision.\n
       - Range -- 1 (highest accuracy) to 50 (lowest accuracy) \n
       - PDOP -- square root of (HDOP^2 + VDOP^2)
        */

  float HDOP;
  /**<   Horizontal dilution of precision. \n
       - Range -- 1 (highest accuracy) to 50 (lowest accuracy)
        */

  float VDOP;
  /**<   Vertical dilution of precision. \n
       - Range -- 1 (highest accuracy) to 50 (lowest accuracy)
       */
}qmiLocDOPStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  float PDOP;
  /**<   Position dilution of precision.\n
       - Range -- 1 (highest accuracy) to 50 (lowest accuracy) \n
       - PDOP -- square root of (HDOP\textsuperscript{2} + VDOP\textsuperscript{2})
        */

  float HDOP;
  /**<   Horizontal dilution of precision. \n
       - Range -- 1 (highest accuracy) to 50 (lowest accuracy)
        */

  float VDOP;
  /**<   Vertical dilution of precision. \n       
       - Range -- 1 (highest accuracy) to 50 (lowest accuracy)
        */

  float GDOP;
  /**<   Geometric  dilution of precision. \n       
       - Range -- 1 (highest accuracy) to 50 (lowest accuracy)
        */

  float TDOP;
  /**<   Time dilution of precision. \n       
       - Range -- 1 (highest accuracy) to 50 (lowest accuracy)
       */
}qmiLocExtDOPStructT_v02;  /* Type */
/**
    @}
  */

typedef uint32_t qmiLocSensorUsageMaskT_v02;
#define QMI_LOC_SENSOR_MASK_USED_ACCEL_V02 ((qmiLocSensorUsageMaskT_v02)0x00000001) /**<  Bitmask specifying whether an accelerometer was used. \n */
#define QMI_LOC_SENSOR_MASK_USED_GYRO_V02 ((qmiLocSensorUsageMaskT_v02)0x00000002) /**<  Bitmask specifying whether a gyroscope was used.  */
typedef uint32_t qmiLocSensorAidedMaskT_v02;
#define QMI_LOC_SENSOR_AIDED_MASK_HEADING_V02 ((qmiLocSensorAidedMaskT_v02)0x00000001) /**<  Specifies whether a sensor was used to calculate heading. \n */
#define QMI_LOC_SENSOR_AIDED_MASK_SPEED_V02 ((qmiLocSensorAidedMaskT_v02)0x00000002) /**<  Specifies whether a sensor was used to calculate speed.\n  */
#define QMI_LOC_SENSOR_AIDED_MASK_POSITION_V02 ((qmiLocSensorAidedMaskT_v02)0x00000004) /**<  Specifies whether a sensor was used to calculate position.\n  */
#define QMI_LOC_SENSOR_AIDED_MASK_VELOCITY_V02 ((qmiLocSensorAidedMaskT_v02)0x00000008) /**<  Specifies whether a sensor was used to calculate velocity.  */
/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocSensorUsageMaskT_v02 usageMask;
  /**<   Specifies which sensors were used in calculating the position in the
 position report.
 Valid bitmasks: \n
      - QMI_LOC_SENSOR_MASK_USED_ACCEL (0x00000001) --  Bitmask specifying whether an accelerometer was used. \n
      - QMI_LOC_SENSOR_MASK_USED_GYRO (0x00000002) --  Bitmask specifying whether a gyroscope was used.  */

  qmiLocSensorAidedMaskT_v02 aidingIndicatorMask;
  /**<   Specifies which results were aided by sensors.
 Valid bitmasks: \n
      - QMI_LOC_SENSOR_AIDED_MASK_HEADING (0x00000001) --  Specifies whether a sensor was used to calculate heading. \n
      - QMI_LOC_SENSOR_AIDED_MASK_SPEED (0x00000002) --  Specifies whether a sensor was used to calculate speed.\n 
      - QMI_LOC_SENSOR_AIDED_MASK_POSITION (0x00000004) --  Specifies whether a sensor was used to calculate position.\n 
      - QMI_LOC_SENSOR_AIDED_MASK_VELOCITY (0x00000008) --  Specifies whether a sensor was used to calculate velocity.  */
}qmiLocSensorUsageIndicatorStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCTIMESOURCEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_TIME_SRC_INVALID_V02 = 0, /**<  Invalid time \n */
  eQMI_LOC_TIME_SRC_NETWORK_TIME_TRANSFER_V02 = 1, /**<  Time is set by the 1X system \n  */
  eQMI_LOC_TIME_SRC_NETWORK_TIME_TAGGING_V02 = 2, /**<  Time is set by WCDMA/GSM time tagging (that is,
       associating network time with GPS time) \n */
  eQMI_LOC_TIME_SRC_EXTERNAL_INPUT_V02 = 3, /**<  Time is set by an external injection \n */
  eQMI_LOC_TIME_SRC_TOW_DECODE_V02 = 4, /**<  Time is set after decoding over-the-air GPS navigation data
       from one GPS satellite \n */
  eQMI_LOC_TIME_SRC_TOW_CONFIRMED_V02 = 5, /**<  Time is set after decoding over-the-air GPS navigation data
       from multiple satellites \n */
  eQMI_LOC_TIME_SRC_TOW_AND_WEEK_CONFIRMED_V02 = 6, /**<  Both time of the week and the GPS week number are known \n */
  eQMI_LOC_TIME_SRC_NAV_SOLUTION_V02 = 7, /**<  Time is set by the position engine after the fix is obtained \n */
  eQMI_LOC_TIME_SRC_SOLVE_FOR_TIME_V02 = 8, /**<  Time is set by the position engine after performing SFT;
       this is done when the clock time uncertainty is large \n */
  eQMI_LOC_TIME_SRC_GLO_TOW_DECODE_V02 = 9, /**<  Time is set after decoding GLONASS satellites \n */
  eQMI_LOC_TIME_SRC_TIME_TRANSFORM_V02 = 10, /**<  Time is set after transforming the GPS to GLONASS time \n */
  eQMI_LOC_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING_V02 = 11, /**<  Time is set by the sleep time tag provided by the WCDMA network \n */
  eQMI_LOC_TIME_SRC_GSM_SLEEP_TIME_TAGGING_V02 = 12, /**<  Time is set by the sleep time tag provided by the GSM network \n */
  eQMI_LOC_TIME_SRC_UNKNOWN_V02 = 13, /**<  Source of the time is unknown \n */
  eQMI_LOC_TIME_SRC_SYSTEM_TIMETICK_V02 = 14, /**<  Time is derived from the system clock (better known as the slow clock);
       GNSS time is maintained irrespective of the GNSS receiver state \n  */
  eQMI_LOC_TIME_SRC_QZSS_TOW_DECODE_V02 = 15, /**<  Time is set after decoding QZSS satellites \n  */
  eQMI_LOC_TIME_SRC_BDS_TOW_DECODE_V02 = 16, /**<  Time is set after decoding BDS satellites \n  */
  eQMI_LOC_TIME_SRC_GAL_TOW_DECODE_V02 = 17, /**<  Time is set after decoding Galielo satellites   */
  QMILOCTIMESOURCEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocTimeSourceEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCRELIABILITYENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_RELIABILITY_NOT_SET_V02 = 0, /**<  Not set \n  */
  eQMI_LOC_RELIABILITY_VERY_LOW_V02 = 1, /**<  Very low; use it at your own risk \n  */
  eQMI_LOC_RELIABILITY_LOW_V02 = 2, /**<  Low; little or no cross-checking is possible \n  */
  eQMI_LOC_RELIABILITY_MEDIUM_V02 = 3, /**<  Medium; limited cross-check passed \n  */
  eQMI_LOC_RELIABILITY_HIGH_V02 = 4, /**<  High; strong cross-check passed  */
  QMILOCRELIABILITYENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocReliabilityEnumT_v02;
/**
    @}
  */

typedef uint64_t qmiLocNavSolutionMaskT_v02;
#define QMI_LOC_NAV_MASK_SBAS_CORRECTION_IONO_V02 ((qmiLocNavSolutionMaskT_v02)0x00000001ull) /**<  Specifies whether SBAS ionospheric correction is used \n  */
#define QMI_LOC_NAV_MASK_SBAS_CORRECTION_FAST_V02 ((qmiLocNavSolutionMaskT_v02)0x00000002ull) /**<  Specifies whether SBAS fast correction is used \n  */
#define QMI_LOC_NAV_MASK_SBAS_CORRECTION_LONG_V02 ((qmiLocNavSolutionMaskT_v02)0x00000004ull) /**<  Specifies whether SBAS long-tem correction is used \n */
#define QMI_LOC_NAV_MASK_SBAS_INTEGRITY_V02 ((qmiLocNavSolutionMaskT_v02)0x00000008ull) /**<  Specifies whether SBAS integrity information is used \n */
#define QMI_LOC_NAV_MASK_CORRECTION_DGNSS_V02 ((qmiLocNavSolutionMaskT_v02)0x00000010ull) /**<  Specifies whether DGNSS information is used  */
typedef uint32_t qmiLocSensorSubTechnologyMaskT_v02;
#define QMI_LOC_SENSOR_SUB_MASK_PDR_ENABLED_V02 ((qmiLocSensorSubTechnologyMaskT_v02)0x00000001) /**<  Specifies whether PDR is enabled or disabled \n  */
#define QMI_LOC_SENSOR_SUB_MASK_PEDOMETER_ENABLED_V02 ((qmiLocSensorSubTechnologyMaskT_v02)0x00000002) /**<  Specifies whether a pedometer was used \n  */
#define QMI_LOC_SENSOR_SUB_MASK_VEHICULAR_ENABLED_V02 ((qmiLocSensorSubTechnologyMaskT_v02)0x00000004) /**<  Specifies whether vehicular sensor assistance is enabled or disabled  */
typedef uint64_t qmiLocSpoofMaskT_v02;
#define QMI_LOC_POSITION_SPOOFED_V02 ((qmiLocSpoofMaskT_v02)0x00000001ull) /**<  Set bit indicates suspected spoofing in position \n  */
#define QMI_LOC_TIME_SPOOFED_V02 ((qmiLocSpoofMaskT_v02)0x00000002ull) /**<  Set bit indicates suspected spoofing in time \n  */
#define QMI_LOC_NAVIGATION_DATA_SPOOFED_V02 ((qmiLocSpoofMaskT_v02)0x00000004ull) /**<  Set bit indicates suspected spoofing in navigation data  */
typedef uint64_t qmiLocGnssSignalTypeMaskT_v02;
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L1CA_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00000001ull) /**<  GPS L1CA RF band \n */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L1C_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00000002ull) /**<  GPS L1C RF band \n */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L2C_L_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00000004ull) /**<  GPS L2C_L RF band \n  */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L5_Q_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00000008ull) /**<  GPS L5_Q RF band \n */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GLONASS_G1_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00000010ull) /**<  GLONASS G1 (L1OF) RF band \n */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GLONASS_G2_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00000020ull) /**<  GLONASS G2 (L2OF) RF band\n  */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E1_C_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00000040ull) /**<  Galileo E1_C RF band \n  */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E5A_Q_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00000080ull) /**<  Galileo E5A_Q RF band \n */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E5B_Q_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00000100ull) /**<  Galileo E5B_Q RF band \n */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B1_I_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00000200ull) /**<  BeiDou B1_I RF band \n */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B1C_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00000400ull) /**<  BeiDou B1C RF band \n */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B2_I_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00000800ull) /**<  BeiDou B2_I RF band \n  */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B2A_I_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00001000ull) /**<  BeiDou B2A_I RF band \n  */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L1CA_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00002000ull) /**<  QZSS L1CA RF band \n */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L1S_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00004000ull) /**<  QZSS L1S RF band \n */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L2C_L_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00008000ull) /**<  QZSS L2C_L RF band \n */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L5_Q_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00010000ull) /**<  QZSS L5_Q RF band \n */
#define QMI_LOC_MASK_GNSS_SIGNAL_TYPE_SBAS_L1_CA_V02 ((qmiLocGnssSignalTypeMaskT_v02)0x00020000ull) /**<  SBAS L1_CA RF band  */
/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCGNSSSIGNALTYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_GNSS_SIGNAL_TYPE_INVALID_V02 = 0, /**<  Invalid RF band \n */
  eQMI_LOC_GNSS_SIGNAL_TYPE_GPS_L1CA_V02 = 1, /**<  GPS L1CA RF band \n */
  eQMI_LOC_GNSS_SIGNAL_TYPE_GPS_L1C_V02 = 2, /**<  GPS L1C RF band \n */
  eQMI_LOC_GNSS_SIGNAL_TYPE_GPS_L2C_L_V02 = 3, /**<  GPS L2C_L RF band \n */
  eQMI_LOC_GNSS_SIGNAL_TYPE_GPS_L5_Q_V02 = 4, /**<  GPS L5_Q RF band \n */
  eQMI_LOC_GNSS_SIGNAL_TYPE_GLONASS_G1_V02 = 5, /**<  GLONASS G1 (L1OF) RF band \n */
  eQMI_LOC_GNSS_SIGNAL_TYPE_GLONASS_G2_V02 = 6, /**<  GLONASS G2 (L2OF) RF band \n */
  eQMI_LOC_GNSS_SIGNAL_TYPE_GALILEO_E1_C_V02 = 7, /**<  Galileo E1_C RF band \n */
  eQMI_LOC_GNSS_SIGNAL_TYPE_GALILEO_E5A_Q_V02 = 8, /**<  Galileo E5A_Q RF band \n */
  eQMI_LOC_GNSS_SIGNAL_TYPE_GALILEO_E5B_Q_V02 = 9, /**<  Galileo E5B_Q RF band \n */
  eQMI_LOC_GNSS_SIGNAL_TYPE_BEIDOU_B1_I_V02 = 10, /**<  BeiDou B1_I RF band \n */
  eQMI_LOC_GNSS_SIGNAL_TYPE_BEIDOU_B1C_V02 = 11, /**<  BeiDou B1C RF band \n */
  eQMI_LOC_GNSS_SIGNAL_TYPE_BEIDOU_B2_I_V02 = 12, /**<  BeiDou B2_I RF band \n */
  eQMI_LOC_GNSS_SIGNAL_TYPE_BEIDOU_B2A_I_V02 = 13, /**<  BeiDou B2A_I RF band \n  */
  eQMI_LOC_GNSS_SIGNAL_TYPE_QZSS_L1CA_V02 = 14, /**<  QZSS L1CA RF band\n  */
  eQMI_LOC_GNSS_SIGNAL_TYPE_QZSS_L1S_V02 = 15, /**<  QZSS L1S RF band \n */
  eQMI_LOC_GNSS_SIGNAL_TYPE_QZSS_L2C_L_V02 = 16, /**<  QZSS L2C_L RF band \n  */
  eQMI_LOC_GNSS_SIGNAL_TYPE_QZSS_L5_Q_V02 = 17, /**<  QZSS L5_Q RF band \n  */
  eQMI_LOC_GNSS_SIGNAL_TYPE_SBAS_L1_CA_V02 = 18, /**<  SBAS L1_CA RF band  */
  QMILOCGNSSSIGNALTYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocGnssSignalTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  int32_t agcMetricDb;
  /**<   AGC metric in 0.01 dB. */

  int32_t bpMetricDb;
  /**<   BP metric in 0.01 dB. */
}qmiLocJammerIndicatorStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Sends the position report to the control point. */
typedef struct {

  /* Mandatory */
  /*  Session Status */
  qmiLocSessionStatusEnumT_v02 sessionStatus;
  /**<   Session status.
 Valid values: \n
      - eQMI_LOC_SESS_STATUS_SUCCESS (0) --  Session was successful \n
      - eQMI_LOC_SESS_STATUS_IN_PROGRESS (1) --  Session is still in progress; further position reports are generated
       until either the fix criteria specified by the client are met or the
       client response timeout occurs \n
      - eQMI_LOC_SESS_STATUS_GENERAL_FAILURE (2) --  Session failed \n
      - eQMI_LOC_SESS_STATUS_TIMEOUT (3) --  Fix request failed because the session timed out \n
      - eQMI_LOC_SESS_STATUS_USER_END (4) --  Fix request failed because the session was ended by the user \n 
      - eQMI_LOC_SESS_STATUS_BAD_PARAMETER (5) --  Fix request failed due to bad parameters in the request \n
      - eQMI_LOC_SESS_STATUS_PHONE_OFFLINE (6) --  Fix request failed because the phone is offline \n
      - eQMI_LOC_SESS_STATUS_ENGINE_LOCKED (7) --  Fix request failed because the engine is locked \n
      - eQMI_LOC_SESS_STATUS_GNSS_SERVICE_NOT_AVAILABLE (8) --  Fix request failed because the GNSS engine was not available for service  
 */

  /* Mandatory */
  /*  Session ID */
  uint8_t sessionId;
  /**<    ID of the session that was specified in the  
        QMI_LOC_START_REQ request. \n
        - Range -- 0 to 255 */

  /* Optional */
  /*  Latitude */
  uint8_t latitude_valid;  /**< Must be set to true if latitude is being passed */
  double latitude;
  /**<   Latitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -90.0 to 90.0 (ositive values indicate northern latitude,
         (negative values indicate southern latitude)
      */

  /* Optional */
  /*  Longitude */
  uint8_t longitude_valid;  /**< Must be set to true if longitude is being passed */
  double longitude;
  /**<   Longitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -180.0 to 180.0 (positive values indicate eastern longitude,
         negative values indicate western longitude)
        */

  /* Optional */
  /*  Circular Horizontal Position Uncertainty */
  uint8_t horUncCircular_valid;  /**< Must be set to true if horUncCircular is being passed */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
       - Units -- Meters */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
  uint8_t horUncEllipseSemiMinor_valid;  /**< Must be set to true if horUncEllipseSemiMinor is being passed */
  float horUncEllipseSemiMinor;
  /**<   Semi-minor axis of horizontal elliptical uncertainty.\n
       - Units -- Meters */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
  uint8_t horUncEllipseSemiMajor_valid;  /**< Must be set to true if horUncEllipseSemiMajor is being passed */
  float horUncEllipseSemiMajor;
  /**<   Semi-major axis of horizontal elliptical uncertainty.\n
       - Units -- Meters */

  /* Optional */
  /*  Elliptical Horizontal Uncertainty Azimuth */
  uint8_t horUncEllipseOrientAzimuth_valid;  /**< Must be set to true if horUncEllipseOrientAzimuth is being passed */
  float horUncEllipseOrientAzimuth;
  /**<   Elliptical horizontal uncertainty azimuth of orientation.\n
       - Units -- Decimal degrees \n
       - Range -- 0 to 180 */

  /* Optional */
  /*  Horizontal Confidence */
  uint8_t horConfidence_valid;  /**< Must be set to true if horConfidence is being passed */
  uint8_t horConfidence;
  /**<   Horizontal uncertainty confidence.
       If both elliptical and horizontal uncertainties are specified in this message,
       the confidence corresponds to the elliptical uncertainty. \n
       - Units -- Percent \n
       - Range -- 0 to 99 */

  /* Optional */
  /*  Horizontal Reliability */
  uint8_t horReliability_valid;  /**< Must be set to true if horReliability is being passed */
  qmiLocReliabilityEnumT_v02 horReliability;
  /**<   Reliability of the horizontal position.
 Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Not set \n 
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Very low; use it at your own risk \n 
      - eQMI_LOC_RELIABILITY_LOW (2) --  Low; little or no cross-checking is possible \n 
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Medium; limited cross-check passed \n 
      - eQMI_LOC_RELIABILITY_HIGH (4) --  High; strong cross-check passed 
 */

  /* Optional */
  /*  Horizontal Speed */
  uint8_t speedHorizontal_valid;  /**< Must be set to true if speedHorizontal is being passed */
  float speedHorizontal;
  /**<   Horizontal speed.\n
       - Units -- Meters per second */

  /* Optional */
  /*  Speed Uncertainty */
  uint8_t speedUnc_valid;  /**< Must be set to true if speedUnc is being passed */
  float speedUnc;
  /**<   3D speed uncertainty.\n
       - Units -- Meters per second */

  /* Optional */
  /*  Altitude With Respect to Ellipsoid */
  uint8_t altitudeWrtEllipsoid_valid;  /**< Must be set to true if altitudeWrtEllipsoid is being passed */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
       - Units -- Meters \n
       - Range -- -500 to 15883 */

  /* Optional */
  /*  Altitude With Respect to Sea Level */
  uint8_t altitudeWrtMeanSeaLevel_valid;  /**< Must be set to true if altitudeWrtMeanSeaLevel is being passed */
  float altitudeWrtMeanSeaLevel;
  /**<   Altitude with respect to mean sea level.\n
       - Units -- Meters */

  /* Optional */
  /*  Vertical Uncertainty */
  uint8_t vertUnc_valid;  /**< Must be set to true if vertUnc is being passed */
  float vertUnc;
  /**<   Vertical uncertainty.\n
       - Units -- Meters */

  /* Optional */
  /*  Vertical Confidence */
  uint8_t vertConfidence_valid;  /**< Must be set to true if vertConfidence is being passed */
  uint8_t vertConfidence;
  /**<   Vertical uncertainty confidence.\n
       - Units -- Percent \n
       - Range -- 0 to 99 */

  /* Optional */
  /*  Vertical Reliability */
  uint8_t vertReliability_valid;  /**< Must be set to true if vertReliability is being passed */
  qmiLocReliabilityEnumT_v02 vertReliability;
  /**<   Reliability of the vertical position.
 Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Not set \n 
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Very low; use it at your own risk \n 
      - eQMI_LOC_RELIABILITY_LOW (2) --  Low; little or no cross-checking is possible \n 
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Medium; limited cross-check passed \n 
      - eQMI_LOC_RELIABILITY_HIGH (4) --  High; strong cross-check passed 
 */

  /* Optional */
  /*  Vertical Speed */
  uint8_t speedVertical_valid;  /**< Must be set to true if speedVertical is being passed */
  float speedVertical;
  /**<   Vertical speed.\n
         - Units -- Meters per second */

  /* Optional */
  /*  Heading */
  uint8_t heading_valid;  /**< Must be set to true if heading is being passed */
  float heading;
  /**<   Heading.\n
         - Units -- Degrees \n
         - Range -- 0 to 359.999  */

  /* Optional */
  /*  Heading Uncertainty */
  uint8_t headingUnc_valid;  /**< Must be set to true if headingUnc is being passed */
  float headingUnc;
  /**<   Heading uncertainty.\n
       - Units -- Degrees \n
       - Range -- 0 to 359.999 */

  /* Optional */
  /*  Magnetic Deviation */
  uint8_t magneticDeviation_valid;  /**< Must be set to true if magneticDeviation is being passed */
  float magneticDeviation;
  /**<   Difference between the bearing to true north and the bearing shown
      on a magnetic compass. The deviation is positive when the magnetic
      north is east of true north. */

  /* Optional */
  /*  Technology Used */
  uint8_t technologyMask_valid;  /**< Must be set to true if technologyMask is being passed */
  qmiLocPosTechMaskT_v02 technologyMask;
  /**<   Technology used in computing this fix.
 Valid bitmasks: \n
      - QMI_LOC_POS_TECH_MASK_SATELLITE (0x00000001) --  Satellites are used to generate the fix \n
      - QMI_LOC_POS_TECH_MASK_CELLID (0x00000002) --  Cell towers are used to generate the fix \n 
      - QMI_LOC_POS_TECH_MASK_WIFI (0x00000004) --  Wi-Fi access points are used to generate the fix \n
      - QMI_LOC_POS_TECH_MASK_SENSORS (0x00000008) --  Sensors are used to generate the fix \n 
      - QMI_LOC_POS_TECH_MASK_REFERENCE_LOCATION (0x00000010) --  Reference location is used to generate the fix \n
      - QMI_LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION (0x00000020) --  Coarse position injected into the location engine is used to
        generate the fix \n
      - QMI_LOC_POS_TECH_MASK_AFLT (0x00000040) --  AFLT is used to generate the fix \n
      - QMI_LOC_POS_TECH_MASK_HYBRID (0x00000080) --  GNSS and network-provided measurements are used to
        generate the fix 
 */

  /* Optional */
  /*  Dilution of Precision */
  uint8_t DOP_valid;  /**< Must be set to true if DOP is being passed */
  qmiLocDOPStructT_v02 DOP;
  /**<    */

  /* Optional */
  /*  UTC Timestamp */
  uint8_t timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64_t timestampUtc;
  /**<   UTC timestamp. \n
       - Units -- Milliseconds since Jan. 1, 1970 */

  /* Optional */
  /*  Leap Seconds */
  uint8_t leapSeconds_valid;  /**< Must be set to true if leapSeconds is being passed */
  uint8_t leapSeconds;
  /**<   Leap second information; if leapSeconds is not available,
         timestampUtc is calculated based on a hard-coded value
         for leap seconds. \n
         - Units -- Seconds */

  /* Optional */
  /*  GPS Time */
  uint8_t gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  qmiLocGPSTimeStructT_v02 gpsTime;
  /**<   \vspace{0.06in} \n Number of weeks since Jan. 5, 1980, and
       milliseconds into the current week. */

  /* Optional */
  /*  Time Uncertainty */
  uint8_t timeUnc_valid;  /**< Must be set to true if timeUnc is being passed */
  float timeUnc;
  /**<   Time uncertainty. \n
       - Units -- Milliseconds */

  /* Optional */
  /*  Time Source */
  uint8_t timeSrc_valid;  /**< Must be set to true if timeSrc is being passed */
  qmiLocTimeSourceEnumT_v02 timeSrc;
  /**<   Time source. Valid values: \n
      - eQMI_LOC_TIME_SRC_INVALID (0) --  Invalid time \n
      - eQMI_LOC_TIME_SRC_NETWORK_TIME_TRANSFER (1) --  Time is set by the 1X system \n 
      - eQMI_LOC_TIME_SRC_NETWORK_TIME_TAGGING (2) --  Time is set by WCDMA/GSM time tagging (that is,
       associating network time with GPS time) \n
      - eQMI_LOC_TIME_SRC_EXTERNAL_INPUT (3) --  Time is set by an external injection \n
      - eQMI_LOC_TIME_SRC_TOW_DECODE (4) --  Time is set after decoding over-the-air GPS navigation data
       from one GPS satellite \n
      - eQMI_LOC_TIME_SRC_TOW_CONFIRMED (5) --  Time is set after decoding over-the-air GPS navigation data
       from multiple satellites \n
      - eQMI_LOC_TIME_SRC_TOW_AND_WEEK_CONFIRMED (6) --  Both time of the week and the GPS week number are known \n
      - eQMI_LOC_TIME_SRC_NAV_SOLUTION (7) --  Time is set by the position engine after the fix is obtained \n
      - eQMI_LOC_TIME_SRC_SOLVE_FOR_TIME (8) --  Time is set by the position engine after performing SFT;
       this is done when the clock time uncertainty is large \n
      - eQMI_LOC_TIME_SRC_GLO_TOW_DECODE (9) --  Time is set after decoding GLONASS satellites \n
      - eQMI_LOC_TIME_SRC_TIME_TRANSFORM (10) --  Time is set after transforming the GPS to GLONASS time \n
      - eQMI_LOC_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING (11) --  Time is set by the sleep time tag provided by the WCDMA network \n
      - eQMI_LOC_TIME_SRC_GSM_SLEEP_TIME_TAGGING (12) --  Time is set by the sleep time tag provided by the GSM network \n
      - eQMI_LOC_TIME_SRC_UNKNOWN (13) --  Source of the time is unknown \n
      - eQMI_LOC_TIME_SRC_SYSTEM_TIMETICK (14) --  Time is derived from the system clock (better known as the slow clock);
       GNSS time is maintained irrespective of the GNSS receiver state \n 
      - eQMI_LOC_TIME_SRC_QZSS_TOW_DECODE (15) --  Time is set after decoding QZSS satellites \n 
      - eQMI_LOC_TIME_SRC_BDS_TOW_DECODE (16) --  Time is set after decoding BDS satellites \n 
      - eQMI_LOC_TIME_SRC_GAL_TOW_DECODE (17) --  Time is set after decoding Galielo satellites   */

  /* Optional */
  /*  Sensor Data Usage */
  uint8_t sensorDataUsage_valid;  /**< Must be set to true if sensorDataUsage is being passed */
  qmiLocSensorUsageIndicatorStructT_v02 sensorDataUsage;
  /**<   \vspace{0.06in} \n Indicates whether sensor data was used in computing the position in this
       position report. */

  /* Optional */
  /*  Fix Count for This Session */
  uint8_t fixId_valid;  /**< Must be set to true if fixId is being passed */
  uint32_t fixId;
  /**<   Fix count for the session; starts with 0 and increments by one
       for each successive position report for a session. */

  /* Optional */
  /*  SVs Used to Calculate the Fix */
  uint8_t gnssSvUsedList_valid;  /**< Must be set to true if gnssSvUsedList is being passed */
  uint32_t gnssSvUsedList_len;  /**< Must be set to # of elements in gnssSvUsedList */
  uint16_t gnssSvUsedList[QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02];
  /**<   Each entry in the list contains the SV ID of a satellite
       used for calculating this position report. The following
       information is associated with each SV ID. \n
       Range: \n
       - GPS --     1 to 32 \n
       - GLONASS -- 65 to 96 \n
       - QZSS --    193 to 197 \n
       - BDS --     201 to 237 \n
       - Galileo -- 301 to 336
      */

  /* Optional */
  /*  Altitude Assumed */
  uint8_t altitudeAssumed_valid;  /**< Must be set to true if altitudeAssumed is being passed */
  uint8_t altitudeAssumed;
  /**<   Indicates whether altitude is assumed or calculated: \n
       - 0x00 (FALSE) -- Calculated \n
       - 0x01 (TRUE) -- Assumed; there might not be enough
                                 satellites to determine the precise altitude
        */

  /* Optional */
  /*  Velocity ENU (East, North, Up) */
  uint8_t velEnu_valid;  /**< Must be set to true if velEnu is being passed */
  float velEnu[QMI_LOC_ENU_ARRAY_LENGTH_V02];
  /**<   East, north, up velocity.\n
       - Units -- Meters per second */

  /* Optional */
  /*  Velocity Uncertainty ENU */
  uint8_t velUncEnu_valid;  /**< Must be set to true if velUncEnu is being passed */
  float velUncEnu[QMI_LOC_ENU_ARRAY_LENGTH_V02];
  /**<   East, north, up velocity uncertainty.\n
       - Units -- Meters per second */

  /* Optional */
  /*  Navigation solution */
  uint8_t navSolutionMask_valid;  /**< Must be set to true if navSolutionMask is being passed */
  qmiLocNavSolutionMaskT_v02 navSolutionMask;
  /**<   Navigation solutions that are used to calculate
 the GNSS position report.
 Valid bitmasks: \n
      - QMI_LOC_NAV_MASK_SBAS_CORRECTION_IONO (0x00000001) --  Specifies whether SBAS ionospheric correction is used \n 
      - QMI_LOC_NAV_MASK_SBAS_CORRECTION_FAST (0x00000002) --  Specifies whether SBAS fast correction is used \n 
      - QMI_LOC_NAV_MASK_SBAS_CORRECTION_LONG (0x00000004) --  Specifies whether SBAS long-tem correction is used \n
      - QMI_LOC_NAV_MASK_SBAS_INTEGRITY (0x00000008) --  Specifies whether SBAS integrity information is used \n
      - QMI_LOC_NAV_MASK_CORRECTION_DGNSS (0x00000010) --  Specifies whether DGNSS information is used 
 */

  /* Optional */
  /*  Sensor Subtechnology Information */
  uint8_t sensorSubTechnologyMask_valid;  /**< Must be set to true if sensorSubTechnologyMask is being passed */
  qmiLocSensorSubTechnologyMaskT_v02 sensorSubTechnologyMask;
  /**<   Sensor subtechnology information.
 Valid bitmasks: \n
      - QMI_LOC_SENSOR_SUB_MASK_PDR_ENABLED (0x00000001) --  Specifies whether PDR is enabled or disabled \n 
      - QMI_LOC_SENSOR_SUB_MASK_PEDOMETER_ENABLED (0x00000002) --  Specifies whether a pedometer was used \n 
      - QMI_LOC_SENSOR_SUB_MASK_VEHICULAR_ENABLED (0x00000004) --  Specifies whether vehicular sensor assistance is enabled or disabled 
 */

  /* Optional */
  /*  GNSS Only Position Report */
  uint8_t gnssOnlyPosition_valid;  /**< Must be set to true if gnssOnlyPosition is being passed */
  uint8_t gnssOnlyPosition;
  /**<   Indicates if this position report is generated from GNSS only technology.
       Values: \n
       - 0x00 (FALSE) -- Position is generated with other technologies (default) \n
       - 0x01 (TRUE)  -- Position is generated from GNSS technology only
  */

  /* Optional */
  /*  Extended Dilution of Precision */
  uint8_t extDOP_valid;  /**< Must be set to true if extDOP is being passed */
  qmiLocExtDOPStructT_v02 extDOP;
  /**<   \vspace{0.06in} \n Dilution of precision associated with this position. */

  /* Optional */
  /*  Differential correction source ID */
  uint8_t dgnssStationId_valid;  /**< Must be set to true if dgnssStationId is being passed */
  uint32_t dgnssStationId_len;  /**< Must be set to # of elements in dgnssStationId */
  uint16_t dgnssStationId[QMI_LOC_DGNSS_STATION_ID_ARRAY_LENGTH_V02];
  /**<   List of DGNSS station IDs providing corrections. \n
       Range:    \n
       - SBAS --  120 to 158 and 183 to 191 \n
       - Monitoring station -- 1000-2023 (Station ID biased by 1000) \n
         Other values reserved 
  */

  /* Optional */
  /*  Spoof Report */
  uint8_t spoofReportMask_valid;  /**< Must be set to true if spoofReportMask is being passed */
  qmiLocSpoofMaskT_v02 spoofReportMask;
  /**<   Spoof report.
 Valid bitmasks: \n
      - QMI_LOC_POSITION_SPOOFED (0x00000001) --  Set bit indicates suspected spoofing in position \n 
      - QMI_LOC_TIME_SPOOFED (0x00000002) --  Set bit indicates suspected spoofing in time \n 
      - QMI_LOC_NAVIGATION_DATA_SPOOFED (0x00000004) --  Set bit indicates suspected spoofing in navigation data 
 */

  /* Optional */
  /*  Expanded SVs Used to Calculate the Fix */
  uint8_t expandedGnssSvUsedList_valid;  /**< Must be set to true if expandedGnssSvUsedList is being passed */
  uint32_t expandedGnssSvUsedList_len;  /**< Must be set to # of elements in expandedGnssSvUsedList */
  uint16_t expandedGnssSvUsedList[QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02];
  /**<   If the service reports expandedGnssSvUsedList, gnssSvUsedList is
       not reported. Each entry in the list contains the SV ID of a satellite
       used to calculate this position report. The following
       information is associated with each SV ID.
       Range: \n
      - GPS --     1 to 32 \n
      - GLONASS -- 65 to 96 \n
      - QZSS --    193 to 197 \n
      - BDS --     201 to 237 \n
      - Galileo -- 301 to 336
      */

  /* Optional */
  /*  SVs Signal Types in the SVs Used List */
  uint8_t gnssSvUsedSignalTypeList_valid;  /**< Must be set to true if gnssSvUsedSignalTypeList is being passed */
  uint32_t gnssSvUsedSignalTypeList_len;  /**< Must be set to # of elements in gnssSvUsedSignalTypeList */
  qmiLocGnssSignalTypeMaskT_v02 gnssSvUsedSignalTypeList[QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02];
  /**<   Indicates the signal type of each satellite in expandedGnssSvUsedList. The
 signal type list shall be aligned with the SVs in expandedGnssSvUsedList. Value of 0
 means invalid.
 Valid bitmasks: \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L1CA (0x00000001) --  GPS L1CA RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L1C (0x00000002) --  GPS L1C RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L2C_L (0x00000004) --  GPS L2C_L RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L5_Q (0x00000008) --  GPS L5_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GLONASS_G1 (0x00000010) --  GLONASS G1 (L1OF) RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GLONASS_G2 (0x00000020) --  GLONASS G2 (L2OF) RF band\n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E1_C (0x00000040) --  Galileo E1_C RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E5A_Q (0x00000080) --  Galileo E5A_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E5B_Q (0x00000100) --  Galileo E5B_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B1_I (0x00000200) --  BeiDou B1_I RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B1C (0x00000400) --  BeiDou B1C RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B2_I (0x00000800) --  BeiDou B2_I RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B2A_I (0x00001000) --  BeiDou B2A_I RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L1CA (0x00002000) --  QZSS L1CA RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L1S (0x00004000) --  QZSS L1S RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L2C_L (0x00008000) --  QZSS L2C_L RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L5_Q (0x00010000) --  QZSS L5_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_SBAS_L1_CA (0x00020000) --  SBAS L1_CA RF band 
 */

  /* Optional */
  /*  Jammer Indicator of each GNSS Signal */
  uint8_t jammerIndicatorList_valid;  /**< Must be set to true if jammerIndicatorList is being passed */
  uint32_t jammerIndicatorList_len;  /**< Must be set to # of elements in jammerIndicatorList */
  qmiLocJammerIndicatorStructT_v02 jammerIndicatorList[QMI_LOC_MAX_GNSS_SIGNAL_TYPE_V02];
  /**<   
  */

  /* Optional */
  /*  Conformity Index */
  uint8_t conformityIndex_valid;  /**< Must be set to true if conformityIndex is being passed */
  float conformityIndex;
  /**<   Indicates how well the various input data considered for navigation solution conforms to expectations. \n
       - Range -- 0 (least conforming) to 1 (most conforming)
  */
}qmiLocEventPositionReportIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  char serverUrl[QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 + 1];
  /**<   Assistance server URL. \n
       - Type -- NULL-terminated string \n
       - Maximum string length (including NULL terminator) -- 256
       */
}qmiLocAssistanceServerUrlStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t delayThreshold;
  /**<   Time server is skipped if a one-way delay to the server
       exceeds this threshold. \n
       - Units -- Milliseconds */

  uint32_t timeServerList_len;  /**< Must be set to # of elements in timeServerList */
  qmiLocAssistanceServerUrlStructT_v02 timeServerList[QMI_LOC_MAX_NTP_SERVERS_V02];
  /**<   Ordered list of Time Server URLs that are recommended by the service for time
       information. The client must use the first
       server specified in the list as the primary URL to fetch NTP time,
       the second one as secondary, and so on. \n
       - Maximum server list items -- 3 */
}qmiLocTimeServerListStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Requests the control point to inject time information. */
typedef struct {

  /* Optional */
  /*  Time Server Information */
  uint8_t timeServerInfo_valid;  /**< Must be set to true if timeServerInfo is being passed */
  qmiLocTimeServerListStructT_v02 timeServerInfo;
  /**<   \vspace{0.06in} \n Contains information about the time servers recommended by the
       location service for NTP time. */
}qmiLocEventInjectTimeReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t maxFileSizeInBytes;
  /**<   Maximum allowable predicted orbits file size (in bytes). */

  uint32_t maxPartSize;
  /**<   Maximum allowable predicted orbits file chunk size (in bytes). */
}qmiLocPredictedOrbitsAllowedSizesStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t serverList_len;  /**< Must be set to # of elements in serverList */
  qmiLocAssistanceServerUrlStructT_v02 serverList[QMI_LOC_MAX_PREDICTED_ORBITS_SERVERS_V02];
  /**<   Ordered list of predicted orbits URLs; the client
       must use the first server specified in the list as the primary URL
       from which to download predicted orbits data, the second one as
       secondary, and so on. \n
       - Maximum number of servers that can be specified -- 3 */
}qmiLocPredictedOrbitsServerListStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Requests the control point to inject predicted orbits data. */
typedef struct {

  /* Mandatory */
  /*  Allowed Sizes */
  qmiLocPredictedOrbitsAllowedSizesStructT_v02 allowedSizes;
  /**<   \vspace{0.06in} \n Maximum part and file size allowed to inject in the engine. */

  /* Optional */
  /*  Server List */
  uint8_t serverList_valid;  /**< Must be set to true if serverList is being passed */
  qmiLocPredictedOrbitsServerListStructT_v02 serverList;
  /**<   \vspace{0.06in} \n List of servers that the client can use to download
       predicted orbits data. */
}qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCENGINESTATEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_ENGINE_STATE_ON_V02 = 1, /**<  Location engine is on \n  */
  eQMI_LOC_ENGINE_STATE_OFF_V02 = 2, /**<  Location engine is off  */
  QMILOCENGINESTATEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocEngineStateEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Sends the engine state to the control point. */
typedef struct {

  /* Mandatory */
  /*  Engine State */
  qmiLocEngineStateEnumT_v02 engineState;
  /**<   Location engine state.
 Valid values: \n
      - eQMI_LOC_ENGINE_STATE_ON (1) --  Location engine is on \n 
      - eQMI_LOC_ENGINE_STATE_OFF (2) --  Location engine is off 
 */
}qmiLocEventEngineStateIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCFIXSESSIONSTATEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_FIX_SESSION_STARTED_V02 = 1, /**<  Location fix session has started \n */
  eQMI_LOC_FIX_SESSION_FINISHED_V02 = 2, /**<  Location fix session has ended  */
  QMILOCFIXSESSIONSTATEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocFixSessionStateEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Sends the fix session state to the control point. */
typedef struct {

  /* Mandatory */
  /*  Session State */
  qmiLocFixSessionStateEnumT_v02 sessionState;
  /**<   LOC fix session state.
 Valid values: \n
      - eQMI_LOC_FIX_SESSION_STARTED (1) --  Location fix session has started \n
      - eQMI_LOC_FIX_SESSION_FINISHED (2) --  Location fix session has ended 
 */

  /* Optional */
  /*  Session ID */
  uint8_t sessionId_valid;  /**< Must be set to true if sessionId is being passed */
  uint8_t sessionId;
  /**<   ID of the session specified in the Start request.
       This might not be specified for a fix session corresponding to
       a network-initiated request. \n
       - Range -- 0 to 255 */
}qmiLocEventFixSessionStateIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCGEOFENCEGENALERTENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_GEOFENCE_GEN_ALERT_GNSS_UNAVAILABLE_V02 = 1, /**<  GNSS is unavailable and GNSS position fixes
       cannot be used to monitor geofences \n */
  eQMI_LOC_GEOFENCE_GEN_ALERT_GNSS_AVAILABLE_V02 = 2, /**<  GNSS is available and GNSS postion fixes can
       be used to monitor geofences \n  */
  eQMI_LOC_GEOFENCE_GEN_ALERT_OOS_V02 = 3, /**<  Engine is out of service and no cell ID coverage
       information is available \n */
  eQMI_LOC_GEOFENCE_GEN_ALERT_TIME_INVALID_V02 = 4, /**<  Engine has an invalid time \n  */
  eQMI_LOC_GEOFENCE_GEN_ALERT_MGP_UNAVAILABLE_V02 = 5, /**<  MGP is unloaded or unavailable  */
  QMILOCGEOFENCEGENALERTENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocGeofenceGenAlertEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Notifies the control point of the
                    geofence status. */
typedef struct {

  /* Mandatory */
  /*  Geofence General Alert */
  qmiLocGeofenceGenAlertEnumT_v02 geofenceAlert;
  /**<   Geofence general alert type.
 Valid values: \n
      - eQMI_LOC_GEOFENCE_GEN_ALERT_GNSS_UNAVAILABLE (1) --  GNSS is unavailable and GNSS position fixes
       cannot be used to monitor geofences \n
      - eQMI_LOC_GEOFENCE_GEN_ALERT_GNSS_AVAILABLE (2) --  GNSS is available and GNSS postion fixes can
       be used to monitor geofences \n 
      - eQMI_LOC_GEOFENCE_GEN_ALERT_OOS (3) --  Engine is out of service and no cell ID coverage
       information is available \n
      - eQMI_LOC_GEOFENCE_GEN_ALERT_TIME_INVALID (4) --  Engine has an invalid time \n 
      - eQMI_LOC_GEOFENCE_GEN_ALERT_MGP_UNAVAILABLE (5) --  MGP is unloaded or unavailable 
 */
}qmiLocEventGeofenceGenAlertIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCGEOFENCEBREACHTYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_GEOFENCE_BREACH_TYPE_ENTERING_V02 = 1, /**<  Client entered the geofence \n  */
  eQMI_LOC_GEOFENCE_BREACH_TYPE_LEAVING_V02 = 2, /**<  Client left the geofence  */
  QMILOCGEOFENCEBREACHTYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocGeofenceBreachTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  /*  UTC Timestamp */
  uint64_t timestampUtc;
  /**<   UTC timestamp. \n
       - Units -- Milliseconds since Jan. 1, 1970
        */

  /*  Latitude */
  double latitude;
  /**<   Latitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -90.0 to 90.0 (positive values indicate northern latitude, 
         negative values indicate southern latitude)
   */

  /*   Longitude */
  double longitude;
  /**<   Longitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -180.0 to 180.0  (positive values indicate eastern longitude,
         negative values indicate western longitude)
     */

  /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
  float horUncEllipseSemiMinor;
  /**<   Semi-minor axis of horizontal elliptical uncertainty.\n
       - Units -- Meters */

  /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
  float horUncEllipseSemiMajor;
  /**<   Semi-major axis of horizontal elliptical uncertainty.\n
       - Units -- Meters */

  float horUncEllipseOrientAzimuth;
  /**<   Elliptical horizontal uncertainty azimuth of orientation.\n
       - Units -- Decimal degrees \n
       - Range -- 0 to 180 */

  uint8_t speedHorizontal_valid;
  /**<   Indicates whether the horizontal speed field contains valid
       information. \n
       - 0x01 (TRUE)  -- Valid \n
       - 0x00 (FALSE) -- Invalid and is ignored
        */

  float speedHorizontal;
  /**<   Horizontal speed.\n
       - Units -- Meters per second */

  uint8_t altitudeWrtEllipsoid_valid;
  /**<   Indicates whether the altitude field contains valid
       information. \n
       - 0x01 (TRUE)  -- Valid \n
       - 0x00 (FALSE) -- Invalid and is ignored       
       */

  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
       - Units -- Meters \n
       - Range -- -500 to 15883 */

  uint8_t vertUnc_valid;
  /**<   Indicates whether the Vertical Uncertainty field contains valid
       information.  \n
       - 0x01 (TRUE)  --  Valid \n
       - 0x00 (FALSE) --  Invalid and is ignored
        */

  float vertUnc;
  /**<   Vertical uncertainty.\n
       - Units -- Meters */

  uint8_t speedVertical_valid;
  /**<   Indicates whether the Vertical Speed field contains valid
       information.   \n
       - 0x01 (TRUE)  -- Vertical speed field is valid \n
       - 0x00 (FALSE) -- Vertical speed field is invalid
                                 and is ignored
        */

  float speedVertical;
  /**<   Vertical speed.\n
       - Units -- Meters per second */

  uint8_t heading_valid;
  /**<   Indicates whether the Heading field contains valid
       information. \n
       - 0x01 (TRUE)  --  Heading field is valid \n
       - 0x00 (FALSE) --  Heading field is invalid
                                 and is ignored
        */

  float heading;
  /**<   Heading.\n
        - Units -- Degrees \n
        - Range -- 0 to 359.999  */
}qmiLocGeofencePositionStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCGEOFENCECONFIDENCEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_GEOFENCE_CONFIDENCE_LOW_V02 = 0x01, /**<  Geofence engine indicates a breach with
       low confidence; this setting results in lower
       power usage, and it can impact the yield because
       incorrect breach events might be sent \n */
  eQMI_LOC_GEOFENCE_CONFIDENCE_MED_V02 = 0x02, /**<  (Default) geofence engine indicates a breach with
       medium confidence \n */
  eQMI_LOC_GEOFENCE_CONFIDENCE_HIGH_V02 = 0x03, /**<  Geofence engine indicates a breach with
       high confidence; this setting results in higher
       power usage  */
  QMILOCGEOFENCECONFIDENCEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocGeofenceConfidenceEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Notifies the control point of
                    a geofence breach event. */
typedef struct {

  /* Mandatory */
  /*  Geofence ID */
  uint32_t geofenceId;
  /**<   ID of the geofence for which this
       notification was generated. */

  /* Mandatory */
  /*  Geofence Breach Type */
  qmiLocGeofenceBreachTypeEnumT_v02 breachType;
  /**<   Type of breach that generated this event.
 Valid values: \n
      - eQMI_LOC_GEOFENCE_BREACH_TYPE_ENTERING (1) --  Client entered the geofence \n 
      - eQMI_LOC_GEOFENCE_BREACH_TYPE_LEAVING (2) --  Client left the geofence 
 */

  /* Optional */
  /*  Geofence Position */
  uint8_t geofencePosition_valid;  /**< Must be set to true if geofencePosition is being passed */
  qmiLocGeofencePositionStructT_v02 geofencePosition;
  /**<   \vspace{0.06in} \n Position of the client when it breached the geofence.
       This TLV is included if the client configures the
       geofence to report position. The position is reported
       at the same confidence level specified in the
       QMI_LOC_ADD_CIRCULAR_GEOFENCE_REQ request. */

  /* Optional */
  /*  Geofence Breach Confidence */
  uint8_t breachConfidence_valid;  /**< Must be set to true if breachConfidence is being passed */
  qmiLocGeofenceConfidenceEnumT_v02 breachConfidence;
  /**<   Given a breach event, the confidence determines the probability
 that the breach happened at the geofence boundary.
 Valid values: \n
      - eQMI_LOC_GEOFENCE_CONFIDENCE_LOW (0x01) --  Geofence engine indicates a breach with
       low confidence; this setting results in lower
       power usage, and it can impact the yield because
       incorrect breach events might be sent \n
      - eQMI_LOC_GEOFENCE_CONFIDENCE_MED (0x02) --  (Default) geofence engine indicates a breach with
       medium confidence \n
      - eQMI_LOC_GEOFENCE_CONFIDENCE_HIGH (0x03) --  Geofence engine indicates a breach with
       high confidence; this setting results in higher
       power usage 
 */
}qmiLocEventGeofenceBreachIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Queries the service revision. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmiLocGetServiceRevisionReqMsgT_v02;

  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Queries the service revision. */
typedef struct {

  /* Mandatory */
  /*  Get Revision Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_GET_SERVICE_REVISION_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Mandatory */
  /*  Interface Definition Minor Revision */
  uint32_t revision;
  /**<   Revision of the service. This is the minor revision of the interface that
       the service implements. Minor revision updates of the service are always
       backward compatible. */

  /* Optional */
  /*  GNSS Measurement Engine Firmware Version String */
  uint8_t gnssMeFWVerString_valid;  /**< Must be set to true if gnssMeFWVerString is being passed */
  char gnssMeFWVerString[QMI_LOC_GNSS_ME_VERSION_STRING_MAX_LENGTH_V02 + 1];
  /**<   Version of the GNSS measurement engine software running under the LOC API.\n
       - Type -- NULL-terminated string \n
       - Maximum string length (including NULL terminator) -- 128 \n
       \textbf{Note:} This string is only provided on platforms that have
       a measurement engine that supports this version string. On all other
       platforms, this optional TLV is not provided. */

  /* Optional */
  /*  GNSS Hosted Software Version String */
  uint8_t gnssHostSWVerString_valid;  /**< Must be set to true if gnssHostSWVerString is being passed */
  char gnssHostSWVerString[QMI_LOC_GNSS_HOSTED_SW_VERSION_STRING_MAX_LENGTH_V02 + 1];
  /**<   Version of the GNSS hosted software running under the LOC API.\n
       - Type -- NULL-terminated string \n
       - Maximum string length (including NULL terminator) -- 128 \n
       \textbf{Note:} This string is only provided on hosted architectures
       (measurement and position engine running on different processors) that
       support this version string. On all other platforms, this optional TLV
       is not provided. */

  /* Optional */
  /*  GNSS Software Version String */
  uint8_t gnssSWVerString_valid;  /**< Must be set to true if gnssSWVerString is being passed */
  char gnssSWVerString[QMI_LOC_GNSS_SW_VERSION_STRING_MAX_LENGTH_V02 + 1];
  /**<   Aggregate version of the GNSS software.\n
       - Type -- NULL-terminated string \n
       - Maximum string length (including NULL terminator) -- 256
        */
}qmiLocGetServiceRevisionIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Gets the fix criteria from the location engine. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmiLocGetFixCriteriaReqMsgT_v02;

  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Gets the fix criteria from the location engine. */
typedef struct {

  /* Mandatory */
  /*  Get Fix Criteria Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_GET_FIX_CRITERIA_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Horizontal Accuracy Level */
  uint8_t horizontalAccuracyLevel_valid;  /**< Must be set to true if horizontalAccuracyLevel is being passed */
  qmiLocAccuracyLevelEnumT_v02 horizontalAccuracyLevel;
  /**<   Horizontal accuracy level.
 Valid values: \n
      - eQMI_LOC_ACCURACY_LOW (1) --  Low accuracy \n
      - eQMI_LOC_ACCURACY_MED (2) --  Medium accuracy \n
      - eQMI_LOC_ACCURACY_HIGH (3) --  High accuracy 
 */

  /* Optional */
  /*  Enable/Disable Intermediate Fixes */
  uint8_t intermediateReportState_valid;  /**< Must be set to true if intermediateReportState is being passed */
  qmiLocIntermediateReportStateEnumT_v02 intermediateReportState;
  /**<   Intermediate report state (ON, OFF).\n
 The client must explicitly set this field to OFF to stop receiving
 intermediate position reports. Intermediate position reports are
 generated at 1 Hz and are ON by default. If intermediate reports
 are turned ON, the client receives position reports even if the
 accuracy criteria is not met. The status in the position report is
 set to IN_PROGRESS for intermediate reports.
 Valid values: \n
      - eQMI_LOC_INTERMEDIATE_REPORTS_ON (1) --  Intermediate reports are turned on \n 
      - eQMI_LOC_INTERMEDIATE_REPORTS_OFF (2) --  Intermediate reports are turned off 
 */

  /* Optional */
  /*  Minimum Interval Between Fixes */
  uint8_t minInterval_valid;  /**< Must be set to true if minInterval is being passed */
  uint32_t minInterval;
  /**<   Time that must elapse before alerting the client. \n
       - Units -- Milliseconds */

  /* Optional */
  /*  Configuration for Altitude Assumed Info in GNSS SV Info Event */
  uint8_t configAltitudeAssumed_valid;  /**< Must be set to true if configAltitudeAssumed is being passed */
  qmiLocConfigIncludeAltitudeAssumedInGnssSvInfoEnumT_v02 configAltitudeAssumed;
  /**<   Configuration to include altitude assumed information in the GNSS SV Info event.
 When enabled, an additional GNSS SV Info event indication is sent to the control
 point that also includes the altitude assumed information.
 If not specified, the configuration defaults to ENABLED.
 Valid values: \n
      - eQMI_LOC_ALTITUDE_ASSUMED_IN_GNSS_SV_INFO_ENABLED (1) --  Enable altitude assumed information in GNSS SV Info event. \n
      - eQMI_LOC_ALTITUDE_ASSUMED_IN_GNSS_SV_INFO_DISABLED (2) --  Disable altitude assumed information in GNSS SV Info event. 
 */

  /* Optional */
  /*  Minimum Interval Between Intermediate Position Reports */
  uint8_t minIntermediatePositionReportInterval_valid;  /**< Must be set to true if minIntermediatePositionReportInterval is being passed */
  uint32_t minIntermediatePositionReportInterval;
  /**<   Minimum time interval for intermediate position reports, specified by the control point,
       that, between the position reports elapsed time, must be longer than the interval time.
       If this optional value is not set or set to the default value (0), the intermediate position
       is reported when it is ready. \n
       - Units -- Milliseconds \n
       - Default -- 0 ms
  */

  /* Optional */
  /*  Maximum Wait Time to Get a Position Report */
  uint8_t positionReportTimeout_valid;  /**< Must be set to true if positionReportTimeout is being passed */
  uint32_t positionReportTimeout;
  /**<   Maximum time to work on each fix, specified by the control point.
       The GPS engine returns QMI_ERR_INTERNAL if a position cannot be obtained
       within the positionReportTimeout value. \n
       - Units -- Milliseconds \n
       - Default -- 255*1000 milliseconds \n
       - Range -- 1000 - 255*1000 milliseconds
  */

  /* Optional */
  /*  Share Position Report with Other Clients */
  uint8_t sharePosition_valid;  /**< Must be set to true if sharePosition is being passed */
  uint8_t sharePosition;
  /**<   Indicates whether to share the position report with the other QMI_LOC clients. \n
       - 0x00 (FALSE) -- Do not share the position report \n
       - 0x01 (TRUE) -- Share the position report
  */

  /* Optional */
  /*  GNSS Power Mode */
  uint8_t powerMode_valid;  /**< Must be set to true if powerMode is being passed */
  qmiLocPowerModeStructT_v02 powerMode;
  /**<   \n Power mode requested by the client.
  */
}qmiLocGetFixCriteriaIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Gets the predicted orbits data source. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmiLocGetPredictedOrbitsDataSourceReqMsgT_v02;

  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Gets the predicted orbits data source. */
typedef struct {

  /* Mandatory */
  /*  Predicted Orbits Data Source Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the request for a predicted orbits data source.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Allowed Sizes */
  uint8_t allowedSizes_valid;  /**< Must be set to true if allowedSizes is being passed */
  qmiLocPredictedOrbitsAllowedSizesStructT_v02 allowedSizes;
  /**<   \n Maximum part and file size allowed to inject in the engine. */

  /* Optional */
  /*  Server List */
  uint8_t serverList_valid;  /**< Must be set to true if serverList is being passed */
  qmiLocPredictedOrbitsServerListStructT_v02 serverList;
  /**<   \n List of servers that the cient can use to download
       predicted orbits data. */
}qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Gets the predicted orbits data validity. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmiLocGetPredictedOrbitsDataValidityReqMsgT_v02;

  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t startTimeInUTC;
  /**<   Predicted orbits data is valid starting from this time. \n
       - Units -- Seconds (since Jan. 1, 1970)
        */

  uint16_t durationHours;
  /**<   Duration from the start time for which the data is valid.\n
       - Units -- Hours
        */
}qmiLocPredictedOrbitsDataValidityStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Gets the predicted orbits data validity. */
typedef struct {

  /* Mandatory */
  /*  Predicted Orbits Data Validity Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the request for predicted orbits data validity.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Validity Information */
  uint8_t validityInfo_valid;  /**< Must be set to true if validityInfo is being passed */
  qmiLocPredictedOrbitsDataValidityStructT_v02 validityInfo;
}qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Injects UTC time in the location engine. */
typedef struct {

  /* Mandatory */
  /*  UTC Time */
  uint64_t timeUtc;
  /**<   UTC time since Jan. 1, 1970.\n
       - Units -- Milliseconds */

  /* Mandatory */
  /*  Time Uncertainty */
  uint32_t timeUnc;
  /**<   Time uncertainty.\n
       - Units -- Milliseconds */
}qmiLocInjectUtcTimeReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Injects UTC time in the location engine. */
typedef struct {

  /* Mandatory */
  /*  UTC Time Injection Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_INJECT_UTC_TIME_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */
}qmiLocInjectUtcTimeIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCALTSRCENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_ALT_SRC_UNKNOWN_V02 = 0, /**<  Source is unknown \n */
  eQMI_LOC_ALT_SRC_GPS_V02 = 1, /**<  GPS \n */
  eQMI_LOC_ALT_SRC_CELL_ID_V02 = 2, /**<  Cell ID  \n */
  eQMI_LOC_ALT_SRC_ENHANCED_CELL_ID_V02 = 3, /**<  Enhanced cell ID \n */
  eQMI_LOC_ALT_SRC_WIFI_V02 = 4, /**<  Wi-Fi  \n */
  eQMI_LOC_ALT_SRC_TERRESTRIAL_V02 = 5, /**<  Terrestrial  \n */
  eQMI_LOC_ALT_SRC_TERRESTRIAL_HYBRID_V02 = 6, /**<  Hybrid terrestrial  \n */
  eQMI_LOC_ALT_SRC_ALTITUDE_DATABASE_V02 = 7, /**<  Altitude database  \n */
  eQMI_LOC_ALT_SRC_BAROMETRIC_ALTIMETER_V02 = 8, /**<  Barometric altimeter  \n */
  eQMI_LOC_ALT_SRC_OTHER_V02 = 9, /**<  Other sources  */
  QMILOCALTSRCENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocAltSrcEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCALTSRCLINKAGEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_ALT_SRC_LINKAGE_NOT_SPECIFIED_V02 = 0, /**<  Not specified \n */
  eQMI_LOC_ALT_SRC_LINKAGE_FULLY_INTERDEPENDENT_V02 = 1, /**<  Fully interdependent \n */
  eQMI_LOC_ALT_SRC_LINKAGE_DEPENDS_ON_LAT_LONG_V02 = 2, /**<  Depends on latitude and longitude \n */
  eQMI_LOC_ALT_SRC_LINKAGE_FULLY_INDEPENDENT_V02 = 3, /**<  Fully independent  */
  QMILOCALTSRCLINKAGEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocAltSrcLinkageEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCALTSRCUNCERTAINTYCOVERAGEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_ALT_UNCERTAINTY_NOT_SPECIFIED_V02 = 0, /**<  Not specified \n */
  eQMI_LOC_ALT_UNCERTAINTY_POINT_V02 = 1, /**<  Altitude uncertainty is valid at the injected horizontal
       position coordinates only \n */
  eQMI_LOC_ALT_UNCERTAINTY_FULL_V02 = 2, /**<  Altitude uncertainty applies to the position of the device
       regardless of horizontal position (within the horizontal
       uncertainty region, if provided)  */
  QMILOCALTSRCUNCERTAINTYCOVERAGEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocAltSrcUncertaintyCoverageEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocAltSrcEnumT_v02 source;
  /**<   Source of the altitude.
 Valid values: \n
      - eQMI_LOC_ALT_SRC_UNKNOWN (0) --  Source is unknown \n
      - eQMI_LOC_ALT_SRC_GPS (1) --  GPS \n
      - eQMI_LOC_ALT_SRC_CELL_ID (2) --  Cell ID  \n
      - eQMI_LOC_ALT_SRC_ENHANCED_CELL_ID (3) --  Enhanced cell ID \n
      - eQMI_LOC_ALT_SRC_WIFI (4) --  Wi-Fi  \n
      - eQMI_LOC_ALT_SRC_TERRESTRIAL (5) --  Terrestrial  \n
      - eQMI_LOC_ALT_SRC_TERRESTRIAL_HYBRID (6) --  Hybrid terrestrial  \n
      - eQMI_LOC_ALT_SRC_ALTITUDE_DATABASE (7) --  Altitude database  \n
      - eQMI_LOC_ALT_SRC_BAROMETRIC_ALTIMETER (8) --  Barometric altimeter  \n
      - eQMI_LOC_ALT_SRC_OTHER (9) --  Other sources 
 */

  qmiLocAltSrcLinkageEnumT_v02 linkage;
  /**<   Dependency between the horizontal and
 altitude position components.
 Valid values: \n
      - eQMI_LOC_ALT_SRC_LINKAGE_NOT_SPECIFIED (0) --  Not specified \n
      - eQMI_LOC_ALT_SRC_LINKAGE_FULLY_INTERDEPENDENT (1) --  Fully interdependent \n
      - eQMI_LOC_ALT_SRC_LINKAGE_DEPENDS_ON_LAT_LONG (2) --  Depends on latitude and longitude \n
      - eQMI_LOC_ALT_SRC_LINKAGE_FULLY_INDEPENDENT (3) --  Fully independent 
 */

  qmiLocAltSrcUncertaintyCoverageEnumT_v02 coverage;
  /**<   Region of uncertainty.
 Valid values: \n
      - eQMI_LOC_ALT_UNCERTAINTY_NOT_SPECIFIED (0) --  Not specified \n
      - eQMI_LOC_ALT_UNCERTAINTY_POINT (1) --  Altitude uncertainty is valid at the injected horizontal
       position coordinates only \n
      - eQMI_LOC_ALT_UNCERTAINTY_FULL (2) --  Altitude uncertainty applies to the position of the device
       regardless of horizontal position (within the horizontal
       uncertainty region, if provided) 
 */
}qmiLocAltitudeSrcInfoStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCPOSITIONSRCENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_POSITION_SRC_GNSS_V02 = 0, /**<  Position source is GNSS \n */
  eQMI_LOC_POSITION_SRC_CELLID_V02 = 1, /**<  Position source is cell ID \n */
  eQMI_LOC_POSITION_SRC_ENH_CELLID_V02 = 2, /**<  Position source is enhanced cell ID \n */
  eQMI_LOC_POSITION_SRC_WIFI_V02 = 3, /**<  Position source is Wi-Fi \n */
  eQMI_LOC_POSITION_SRC_TERRESTRIAL_V02 = 4, /**<  Position source is terrestrial \n */
  eQMI_LOC_POSITION_SRC_GNSS_TERRESTRIAL_HYBRID_V02 = 5, /**<  Position source is GNSS terrestrial hybrid \n */
  eQMI_LOC_POSITION_SRC_OTHER_V02 = 6, /**<  Other sources \n */
  eQMI_LOC_POSITION_SRC_DRE_V02 = 7, /**<  Position source is dead reckoning engine  */
  QMILOCPOSITIONSRCENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocPositionSrcEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCPOSITIONSRCPROVIDERENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_POSITION_SRC_PROVIDER_EXTERNAL_V02 = 0, /**<  Position is sourced from an external module \n */
  eQMI_LOC_POSITION_SRC_PROVIDER_INTERNAL_V02 = 1, /**<  Position is sourced from an internal module  */
  QMILOCPOSITIONSRCPROVIDERENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocPositionSrcProviderEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Injects a position to the location engine. */
typedef struct {

  /* Optional */
  /*  Latitude */
  uint8_t latitude_valid;  /**< Must be set to true if latitude is being passed */
  double latitude;
  /**<   Latitude (specified in WGS84 datum).\n      
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -90.0 to 90.0 (positive values indicate northern latitude,
         negative values indicate southern latitude)
    */

  /* Optional */
  /*  Longitude */
  uint8_t longitude_valid;  /**< Must be set to true if longitude is being passed */
  double longitude;
  /**<   Longitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -180.0 to 180.0 (positive values indicate eastern longitude,
         negative values indicate western longitude)
       */

  /* Optional */
  /*  Circular Horizontal Uncertainty */
  uint8_t horUncCircular_valid;  /**< Must be set to true if horUncCircular is being passed */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
        - Units -- Meters */

  /* Optional */
  /*  Horizontal Confidence */
  uint8_t horConfidence_valid;  /**< Must be set to true if horConfidence is being passed */
  uint8_t horConfidence;
  /**<   Horizontal confidence, as defined by \hyperref[TS0332]{TS 03.32}.\n
        - Units -- Percent (1 to 99)  \n
        - 0, 101 to 255 -- invalid value \n
        - If 100 is received, reinterpret to 99 \n        
        This field must be specified together with horizontal uncertainty.
        If not specified when horUncCircular is set, the default value is 50. */

  /* Optional */
  /*  Horizontal Reliability */
  uint8_t horReliability_valid;  /**< Must be set to true if horReliability is being passed */
  qmiLocReliabilityEnumT_v02 horReliability;
  /**<   Reliability of the horizontal position.
 Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Not set \n 
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Very low; use it at your own risk \n 
      - eQMI_LOC_RELIABILITY_LOW (2) --  Low; little or no cross-checking is possible \n 
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Medium; limited cross-check passed \n 
      - eQMI_LOC_RELIABILITY_HIGH (4) --  High; strong cross-check passed 
 */

  /* Optional */
  /*  Altitude With Respect to Ellipsoid */
  uint8_t altitudeWrtEllipsoid_valid;  /**< Must be set to true if altitudeWrtEllipsoid is being passed */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid. \n
        - Units -- Meters  \n
        - Positive -- height \n
        - Negative -- depth
     */

  /* Optional */
  /*  Altitude With Respect to Sea Level */
  uint8_t altitudeWrtMeanSeaLevel_valid;  /**< Must be set to true if altitudeWrtMeanSeaLevel is being passed */
  float altitudeWrtMeanSeaLevel;
  /**<   Altitude with respect to mean sea level.\n
       - Units -- Meters */

  /* Optional */
  /*  Vertical Uncertainty */
  uint8_t vertUnc_valid;  /**< Must be set to true if vertUnc is being passed */
  float vertUnc;
  /**<   Vertical uncertainty; mandatory when either altitudeWrtEllipsoid
        or altitudeWrtMeanSeaLevel is specified.\n
        - Units -- Meters */

  /* Optional */
  /*  Vertical Confidence */
  uint8_t vertConfidence_valid;  /**< Must be set to true if vertConfidence is being passed */
  uint8_t vertConfidence;
  /**<   Vertical confidence, as defined by \hyperref[TS0332]{TS 03.32}.\n
        - Units -- Percent (0-99) \n
        - 0 -- invalid value \n
        - 100 to 256 -- not used \n
        - If 100 is received, reinterpret to 99  \n      
        This field must be specified together with the vertical uncertainty.
        If not specified, the default value is 50. */

  /* Optional */
  /*  Vertical Reliability */
  uint8_t vertReliability_valid;  /**< Must be set to true if vertReliability is being passed */
  qmiLocReliabilityEnumT_v02 vertReliability;
  /**<   Reliability of the vertical position.
 Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Not set \n 
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Very low; use it at your own risk \n 
      - eQMI_LOC_RELIABILITY_LOW (2) --  Low; little or no cross-checking is possible \n 
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Medium; limited cross-check passed \n 
      - eQMI_LOC_RELIABILITY_HIGH (4) --  High; strong cross-check passed 
 */

  /* Optional */
  /*  Altitude Source Information */
  uint8_t altSourceInfo_valid;  /**< Must be set to true if altSourceInfo is being passed */
  qmiLocAltitudeSrcInfoStructT_v02 altSourceInfo;

  /* Optional */
  /*  UTC Timestamp */
  uint8_t timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64_t timestampUtc;
  /**<   - Units -- Milliseconds (since Jan. 1, 1970) */

  /* Optional */
  /*  Position Age */
  uint8_t timestampAge_valid;  /**< Must be set to true if timestampAge is being passed */
  int32_t timestampAge;
  /**<   Position age, which is an estimate of how long ago this fix was made. \n
        - Units -- Milliseconds */

  /* Optional */
  /*  Position Source */
  uint8_t positionSrc_valid;  /**< Must be set to true if positionSrc is being passed */
  qmiLocPositionSrcEnumT_v02 positionSrc;
  /**<   Source from which this position was obtained.
 Valid values: \n
      - eQMI_LOC_POSITION_SRC_GNSS (0) --  Position source is GNSS \n
      - eQMI_LOC_POSITION_SRC_CELLID (1) --  Position source is cell ID \n
      - eQMI_LOC_POSITION_SRC_ENH_CELLID (2) --  Position source is enhanced cell ID \n
      - eQMI_LOC_POSITION_SRC_WIFI (3) --  Position source is Wi-Fi \n
      - eQMI_LOC_POSITION_SRC_TERRESTRIAL (4) --  Position source is terrestrial \n
      - eQMI_LOC_POSITION_SRC_GNSS_TERRESTRIAL_HYBRID (5) --  Position source is GNSS terrestrial hybrid \n
      - eQMI_LOC_POSITION_SRC_OTHER (6) --  Other sources \n
      - eQMI_LOC_POSITION_SRC_DRE (7) --  Position source is dead reckoning engine  \n
 If altitude is specified and the altitude source is not specified, the engine
 assumes that the altitude was obtained using the specified position source. \n
 If both altitude and altitude source are specified, the engine assumes
 that only latitude and longitude were obtained using the specified position
 source.
 */

  /* Optional */
  /*  Raw Circular Horizontal Uncertainty */
  uint8_t rawHorUncCircular_valid;  /**< Must be set to true if rawHorUncCircular is being passed */
  float rawHorUncCircular;
  /**<   Horizontal position uncertainty (circular) without any optimization.\n
        - Units -- Meters */

  /* Optional */
  /*  Raw Horizontal Confidence */
  uint8_t rawHorConfidence_valid;  /**< Must be set to true if rawHorConfidence is being passed */
  uint8_t rawHorConfidence;
  /**<   Horizontal confidence associated with raw horizontal uncertainty,
        as defined by \hyperref[TS0332]{TS 03.32}. \n
        - Units -- Percent (1 to 99) \n
        - 0, 101 to 255 -- invalid value \n
        - If 100 is received, reinterpret to 99 \n
        This field must be specified together with raw horizontal uncertainty.
        If not specified when rawHorUncCircular is set, the default value is 50. */

  /* Optional */
  /*  Free CPI or On-Demand CPI */
  uint8_t onDemandCpi_valid;  /**< Must be set to true if onDemandCpi is being passed */
  uint8_t onDemandCpi;
  /**<   Indicates whether the modem requested this position injection.
        Valid values: \n
       - 0x00 (FALSE) -- Position injection was not requested by the modem (free CPI) \n
       - 0x01 (TRUE) -- Position injection was requested by the modem (on-demand CPI) */

  /* Optional */
  /*  Position Source Provider  */
  uint8_t positionSrcProvider_valid;  /**< Must be set to true if positionSrcProvider is being passed */
  qmiLocPositionSrcProviderEnumT_v02 positionSrcProvider;
  /**<   Source provider from which this position was obtained.
 Valid values: \n
      - eQMI_LOC_POSITION_SRC_PROVIDER_EXTERNAL (0) --  Position is sourced from an external module \n
      - eQMI_LOC_POSITION_SRC_PROVIDER_INTERNAL (1) --  Position is sourced from an internal module 
 */

  /* Optional */
  /*  GPS Time */
  uint8_t gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  qmiLocGPSTimeStructT_v02 gpsTime;
  /**<   \vspace{0.06in} \n Number of weeks since Jan. 5, 1980, and
       milliseconds into the current week. This is the GPS time stamp
       for this injected position. */

  /* Optional */
  /*  Time Uncertainty */
  uint8_t timeUnc_valid;  /**< Must be set to true if timeUnc is being passed */
  float timeUnc;
  /**<   Time uncertainty associated with this injected position. \n
       - Units -- Milliseconds */

  /* Optional */
  /*  Velocity ENU (East, North, Up) */
  uint8_t velEnu_valid;  /**< Must be set to true if velEnu is being passed */
  float velEnu[QMI_LOC_ENU_ARRAY_LENGTH_V02];
  /**<   East, north, up velocity.\n
       - Units -- Meters per second */

  /* Optional */
  /*  Velocity Uncertainty ENU */
  uint8_t velUncEnu_valid;  /**< Must be set to true if velUncEnu is being passed */
  float velUncEnu[QMI_LOC_ENU_ARRAY_LENGTH_V02];
  /**<   East, north, up velocity uncertainty.\n
       - Units -- Meters per second */

  /* Optional */
  /*  Expanded SVs Used to Calculate the Fix */
  uint8_t expandedGnssSvUsedList_valid;  /**< Must be set to true if expandedGnssSvUsedList is being passed */
  uint32_t expandedGnssSvUsedList_len;  /**< Must be set to # of elements in expandedGnssSvUsedList */
  uint16_t expandedGnssSvUsedList[QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02];
  /**<   Each entry in the list contains the SV ID of a satellite
       used for calculating this position report. The following
       information is associated with each SV ID.
       Range: \n
       - GPS --     1 to 32 \n
       - GLONASS -- 65 to 96 \n
       - QZSS --    193 to 197 \n
       - BDS --     201 to 237 \n
       - Galileo -- 301 to 336
      */

  /* Optional */
  /*  Number of SVs used to calculate the fix */
  uint8_t numSvInFix_valid;  /**< Must be set to true if numSvInFix is being passed */
  uint8_t numSvInFix;
  /**<   Number of SVs used to calculate the fix. 
       
  */
}qmiLocInjectPositionReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Injects a position to the location engine. */
typedef struct {

  /* Mandatory */
  /*  UTC Position Injection Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_INJECT_POSITION_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */
}qmiLocInjectPositionIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Enables/disables XTRA-T session control. */
typedef struct {

  /* Mandatory */
  /*  Enable XTRA-T */
  uint8_t xtraTSessionControl;
  /**<   Indicates whether to enable XTRA-T:\n
       - 0x01 (TRUE) -- Enable XTRA-T 
       - 0x00 (FALSE) -- Disable XTRA-T */
}qmiLocSetXtraTSessionControlReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Enables/disables XTRA-T session control. */
typedef struct {

  /* Mandatory */
  /*  Set XTRA-T Session Control Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */
}qmiLocSetXtraTSessionControlIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Gets the XTRA-T session control value from the location
                    engine. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmiLocGetXtraTSessionControlReqMsgT_v02;

  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Gets the XTRA-T session control value from the location
                    engine. */
typedef struct {

  /* Mandatory */
  /*  Get XTRA-T Session Control Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_GET_XTRA_T_SESSION_CONTROL_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Enable/Disable XTRA-T */
  uint8_t xtraTSessionControl_valid;  /**< Must be set to true if xtraTSessionControl is being passed */
  uint8_t xtraTSessionControl;
  /**<   Indicates whether to enable XTRA-T:\n
       - 0x01 (TRUE) -- Enable XTRA-T \n
       - 0x00 (FALSE) -- Disable XTRA-T */
}qmiLocGetXtraTSessionControlIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCCHANNELESTIMATEBANDWIDTHENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_CHANNEL_ESTIMATE_BW_6RB_V02 = 0, /**<  6 resource blocks of channel estimate bandwidth
  in the location output data. \n  */
  eQMI_LOC_CHANNEL_ESTIMATE_BW_20RB_V02 = 1, /**<  20 resource blocks of channel estimate bandwidth
  in the location output data. \n   */
  eQMI_LOC_CHANNEL_ESTIMATE_BW_25RB_V02 = 2, /**<  25 resource blocks of channel estimate bandwidth
  in the location output data. \n   */
  eQMI_LOC_CHANNEL_ESTIMATE_BW_50RB_V02 = 3, /**<  50 resource blocks of channel estimate bandwidth
  in the location output data. \n   */
  eQMI_LOC_CHANNEL_ESTIMATE_BW_75RB_V02 = 4, /**<  75 resource blocks of channel estimate bandwidth
  in the location output data. \n  */
  eQMI_LOC_CHANNEL_ESTIMATE_BW_100RB_V02 = 5, /**<  100 resource blocks of channel estimate bandwidth
  in the location output data.    */
  QMILOCCHANNELESTIMATEBANDWIDTHENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocChannelEstimateBandwidthEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCAPSREQUESTTYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_APS_CRS_REQUEST_V02 = 0, /**<  Using this value, the application software may make
  a CRS data request to the APS task \n    */
  eQMI_LOC_APS_AUX_REQUEST_V02 = 1, /**<  Using this value, the application software may make
  a AUX data request to the APS task.    */
  QMILOCAPSREQUESTTYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocApsRequestTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Used by the control point to request APS data.  */
typedef struct {

  /* Mandatory */
  /*  APS Request Type */
  qmiLocApsRequestTypeEnumT_v02 requestType;
  /**<   Specifies the type of APS request. \n
 Values: \n
      - eQMI_LOC_APS_CRS_REQUEST (0) --  Using this value, the application software may make
  a CRS data request to the APS task \n   
      - eQMI_LOC_APS_AUX_REQUEST (1) --  Using this value, the application software may make
  a AUX data request to the APS task.   
 */

  /* Optional */
  /*  Timeout in milliseconds  */
  uint8_t timeOut_valid;  /**< Must be set to true if timeOut is being passed */
  uint16_t timeOut;
  /**<   CRS capture and Compression Engine scan timeout in milliseconds. \n
  */

  /* Optional */
  /*  Max number of cells */
  uint8_t maxNumOfCells_valid;  /**< Must be set to true if maxNumOfCells is being passed */
  uint8_t maxNumOfCells;
  /**<   Compression Engine scan max number of cells to report. \n
   */

  /* Optional */
  /*  Channel estimate bandwidth  */
  uint8_t channelEstimateBW_valid;  /**< Must be set to true if channelEstimateBW is being passed */
  qmiLocChannelEstimateBandwidthEnumT_v02 channelEstimateBW;
  /**<   Values: \n
      - eQMI_LOC_CHANNEL_ESTIMATE_BW_6RB (0) --  6 resource blocks of channel estimate bandwidth
  in the location output data. \n 
      - eQMI_LOC_CHANNEL_ESTIMATE_BW_20RB (1) --  20 resource blocks of channel estimate bandwidth
  in the location output data. \n  
      - eQMI_LOC_CHANNEL_ESTIMATE_BW_25RB (2) --  25 resource blocks of channel estimate bandwidth
  in the location output data. \n  
      - eQMI_LOC_CHANNEL_ESTIMATE_BW_50RB (3) --  50 resource blocks of channel estimate bandwidth
  in the location output data. \n  
      - eQMI_LOC_CHANNEL_ESTIMATE_BW_75RB (4) --  75 resource blocks of channel estimate bandwidth
  in the location output data. \n 
      - eQMI_LOC_CHANNEL_ESTIMATE_BW_100RB (5) --  100 resource blocks of channel estimate bandwidth
  in the location output data.   
 */

  /* Optional */
  /*  Number of CRS subframes */
  uint8_t numCrsSubframes_valid;  /**< Must be set to true if numCrsSubframes is being passed */
  uint8_t numCrsSubframes;
  /**<   Number of CRS subframes to capture. \n
  */
}qmiLocGetApsDataReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to request APS data.  */
typedef struct {

  /* Mandatory */
  /*  Result Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the get APS data indication:
		- SUCCESS \n
		- FAILURE \n
  */

  /* Mandatory */
  /*  More data to follow  */
  uint8_t moreData;
  /**<   More Data:
			- TRUE \n
			- FALSE \n
   */

  /* Mandatory */
  /*  Buffer Data */
  uint32_t bufferData_len;  /**< Must be set to # of elements in bufferData */
  uint8_t bufferData[QMI_LOC_APS_BUFFER_DATA_MAX_LENGTH_V02];
  /**<   APS data buffer. 
  */
}qmiLocGetApsDataIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Used by the control point to cancel a previous APS Get Data Request.  */
typedef struct {

  /* Mandatory */
  /*  Requester */
  uint8_t requester;
  /**<   Requester asking to cancel. 
	   Values: \n
		- 0 : Location Framework \n
		- 1 : QAPI LOC \n
  */
}qmiLocCancelApsDataReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Gets the mask of the events for which a client has
                    registered. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmiLocGetRegisteredEventsReqMsgT_v02;

  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Gets the mask of the events for which a client has
                    registered. */
typedef struct {

  /* Mandatory */
  /*  Get Registered Events Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_GET_REGISTERED_EVENTS_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Event Registration Mask */
  uint8_t eventRegMask_valid;  /**< Must be set to true if eventRegMask is being passed */
  qmiLocEventRegMaskT_v02 eventRegMask;
  /**<   Event registration mask.
 Valid bitmasks: \n
      - QMI_LOC_EVENT_MASK_POSITION_REPORT (0x00000001) --  Position report event indications. \n 
      - QMI_LOC_EVENT_MASK_GNSS_SV_INFO (0x00000002) --  Satellite report event indications; these reports are sent at a 1 Hz rate.\n 
      - QMI_LOC_EVENT_MASK_NMEA (0x00000004) --  NMEA reports for position and satellites in view; the report is at a 1 Hz rate. \n
      - QMI_LOC_EVENT_MASK_INJECT_TIME_REQ (0x00000008) --  Time injection request event indications.\n 
      - QMI_LOC_EVENT_MASK_INJECT_PREDICTED_ORBITS_REQ (0x00000010) --  Predicted orbits request event indications. \n
      - QMI_LOC_EVENT_MASK_INJECT_POSITION_REQ (0x00000020) --  Control point must enable this mask to receive position injection request
       event indications. \n
      - QMI_LOC_EVENT_MASK_ENGINE_STATE (0x00000040) --  Engine state report event indications. \n
      - QMI_LOC_EVENT_MASK_FIX_SESSION_STATE (0x00000080) --  Fix session status report event indications.\n 
      - QMI_LOC_EVENT_MASK_GEOFENCE_GEN_ALERT (0x00000100) --  Geofence alerts to inform the client of the changes that might
       affect a geofence, for example, if GPS is turned off or if the network is
       unavailable.\n 
      - QMI_LOC_EVENT_MASK_GEOFENCE_BREACH_NOTIFICATION (0x00000200) --  Geofence is breached; generated when UE enters
       or leaves the perimeter of a geofence. This breach report is for a single
       geofence.\n 
      - QMI_LOC_EVENT_MASK_BATCH_FULL_NOTIFICATION (0x00000400) --  Batch is full; the location engine sends this event to notify of batch full
       for ongoing batching session.\n 
      - QMI_LOC_EVENT_MASK_LIVE_BATCHED_POSITION_REPORT (0x00000800) --  Position report indications along with an ongoing batching session. The location engine sends
       this event to notify the batched position report while a batching session
       is ongoing.\n 
      - QMI_LOC_EVENT_MASK_GEOFENCE_BATCH_BREACH_NOTIFICATION (0x00001000) --  Geofence is breached; generated when UE enters
       or leaves the perimeter of a geofence. This breach notification is for
       multiple geofences. Breaches from multiple geofences are batched and
       sent in the same notification. \n  
      - QMI_LOC_EVENT_MASK_GEOFENCE_BATCH_DWELL_NOTIFICATION (0x00002000) --  Geofence is dwelled; generated when a UE enters
       or leaves the perimeter of a geofence and dwells inside or outside for a specified time.
       This dwell notification is for multiple geofences. Dwells from multiple geofences are batched and
       sent in the same notification. 
      - QMI_LOC_EVENT_MASK_BATCHING_STATUS (0x00004000) --  Asynchronous events related to batching. \n
      - QMI_LOC_EVENT_MASK_GNSS_ONLY_POSITION_REPORT (0x000008000) --  Position report event indications that contain a GNSS only position.\n 
      - QMI_LOC_EVENT_MASK_FDCL_SERVICE_REQ (0x000010000) --  FDCL service request. \n
      - QMI_LOC_EVENT_MASK_BS_OBS_DATA_SERVICE_REQ (0x0000020000) --  Base station observed data service request. 
      - QMI_LOC_EVENT_MASK_INITIATE_POSITION_REQ (0x0000040000) --  Control point must enable this mask to get position 
  intialization request from service \n
 */
}qmiLocGetRegisteredEventsIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocPositionEngineConfigParamMaskT_v02;
#define QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_INJECTED_POSITION_CONTROL_V02 ((qmiLocPositionEngineConfigParamMaskT_v02)0x00000001) /**<  Denotes whether the position engine uses the
       injected position in a direct position calculation.\n  */
#define QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_FILTER_SV_USAGE_V02 ((qmiLocPositionEngineConfigParamMaskT_v02)0x00000002) /**<  Denotes whether the position engine filters the
       SV usage in the fix. \n */
#define QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_STORE_ASSIST_DATA_V02 ((qmiLocPositionEngineConfigParamMaskT_v02)0x00000004) /**<  Denotes whether the position engine stores assistance data
       in persistent memory. \n */
#define QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_ENABLE_FASTER_TTFF_V02 ((qmiLocPositionEngineConfigParamMaskT_v02)0x00000008) /**<  Denotes whether the position engine stays on to optimize
       the TTFF for the subsequent position fix.  */
/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Configures position engine functionality. */
typedef struct {

  /* Optional */
  /*  Injected Position Control */
  uint8_t injectedPositionControl_valid;  /**< Must be set to true if injectedPositionControl is being passed */
  uint8_t injectedPositionControl;
  /**<   Controls how the injected position is used in the position engine.
       Valid values: \n
       - 0x01 (TRUE) -- Use the injected position in a direct position
                               calculation (default) \n
       - 0x00 (FALSE) -- Do not use the injected position in a direct
                                position calculation
       
  */

  /* Optional */
  /*  Filter SV Usage */
  uint8_t filterSvUsage_valid;  /**< Must be set to true if filterSvUsage is being passed */
  uint8_t filterSvUsage;
  /**<   Controls whether SV usage is filtered in a position fix.
       Valid values: \n
       - 0x01 (TRUE) -- Filter the usage of SVs in the fix \n
       - 0x00 (FALSE) -- Do not filter the usage of SVs in the fix (default)
      
  */

  /* Optional */
  /*  Store Assist Data */
  uint8_t storeAssistData_valid;  /**< Must be set to true if storeAssistData is being passed */
  uint8_t storeAssistData;
  /**<   Controls whether assistance data is stored in
       persistent memory.
       Valid values: \n
       - 0x01 (TRUE) -- Store assistance data in persistent memory (default) \n
       - 0x00 (FALSE) -- Do not store assistance data in persistent memory
       
  */

  /* Optional */
  /*  Enable Faster TTFF */
  uint8_t enableFasterTTFF_valid;  /**< Must be set to true if enableFasterTTFF is being passed */
  uint8_t enableFasterTTFF;
  /**<   Allows the receiver to stay on after a position session to
       collect information that helps to reduce the Time To First Fix (TTFF)
       when the next position request is made. The receiver stays
       on only if the engine determines that it must collect some
       information. The receiver stays on for the duration needed to
       collect the information.
       If enabled, the clients might see a delay in receiving the Engine Off
       event after the position session ends.
       Valid values: \n
       - 0x01 (TRUE) -- Allow the engine to stay on for reduced TTFF (default)\n
       - 0x00 (FALSE) -- Do not allow the engine to stay on for reduced
                               TTFF
       */
}qmiLocSetPositionEngineConfigParametersReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Configures position engine functionality. */
typedef struct {

  /* Mandatory */
  /*  Set Position Engine Configuration Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Failed Parameters */
  uint8_t failedPositionEngineConfigParamMask_valid;  /**< Must be set to true if failedPositionEngineConfigParamMask is being passed */
  qmiLocPositionEngineConfigParamMaskT_v02 failedPositionEngineConfigParamMask;
  /**<   Identifies the parameters that were not set successfully.
 This field is sent only if the status is other than SUCCESS.
 Valid bitmasks:\n
      - QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_INJECTED_POSITION_CONTROL (0x00000001) --  Denotes whether the position engine uses the
       injected position in a direct position calculation.\n 
      - QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_FILTER_SV_USAGE (0x00000002) --  Denotes whether the position engine filters the
       SV usage in the fix. \n
      - QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_STORE_ASSIST_DATA (0x00000004) --  Denotes whether the position engine stores assistance data
       in persistent memory. \n
      - QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_ENABLE_FASTER_TTFF (0x00000008) --  Denotes whether the position engine stays on to optimize
       the TTFF for the subsequent position fix. 
 */
}qmiLocSetPositionEngineConfigParametersIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Gets the position engine configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Configuration Parameters */
  qmiLocPositionEngineConfigParamMaskT_v02 getPositionEngineConfigParamMask;
  /**<   Mask denoting the configuration parameters to retrieve.
 Valid bitmasks:
      - QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_INJECTED_POSITION_CONTROL (0x00000001) --  Denotes whether the position engine uses the
       injected position in a direct position calculation.\n 
      - QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_FILTER_SV_USAGE (0x00000002) --  Denotes whether the position engine filters the
       SV usage in the fix. \n
      - QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_STORE_ASSIST_DATA (0x00000004) --  Denotes whether the position engine stores assistance data
       in persistent memory. \n
      - QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_ENABLE_FASTER_TTFF (0x00000008) --  Denotes whether the position engine stays on to optimize
       the TTFF for the subsequent position fix. 
 */
}qmiLocGetPositionEngineConfigParametersReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Gets the position engine configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Get Position Engine Configuration Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Injected Position Control */
  uint8_t injectedPositionControl_valid;  /**< Must be set to true if injectedPositionControl is being passed */
  uint8_t injectedPositionControl;
  /**<   Specifies whether the injected position is used for a direct calculation
       in the position engine.
       Valid values: \n
       - 0x01 (TRUE) -- Injected position is used in a direct
                               position calculation (default) \n
       - 0x00 (FALSE) -- Injected position is not used in a direct
                                position calculation
        */

  /* Optional */
  /*  Filter SV Usage */
  uint8_t filterSvUsage_valid;  /**< Must be set to true if filterSvUsage is being passed */
  uint8_t filterSvUsage;
  /**<   Specifies whether SV usage is filtered in a position fix.
       Valid values: \n
       - 0x01 (TRUE) -- SV usage is filtered in the fix \n
       - 0x00 (FALSE) -- SV usage is not filtered in the fix (default)
       
  */

  /* Optional */
  /*  Store Assist Data */
  uint8_t storeAssistData_valid;  /**< Must be set to true if storeAssistData is being passed */
  uint8_t storeAssistData;
  /**<   Specifies whether assistance data is stored in persistent memory.
       Valid values: \n
       - 0x01 (TRUE) -- Assistance data is stored in persistent memory (true) \n
       - 0x00 (FALSE) -- Assistance data is not stored in persistent
                                memory       
  */

  /* Optional */
  /*  Enable Faster TTFF */
  uint8_t enableFasterTTFF_valid;  /**< Must be set to true if enableFasterTTFF is being passed */
  uint8_t enableFasterTTFF;
  /**<   Allows the receiver to stay on after a position session to
       collect information that helps reduce the TTFF
       when the next position request is made. The receiver stays
       on only if the engine determines that it must collect some
       information. The receiver stays on for the duration needed to
       collect the information.
       If enabled, the clients might see a delay in receiving the Engine Off
       event after the position session ends.
       Valid values: \n
       - 0x01 (TRUE) -- Allow the engine to stay on for reduced TTFF (default) \n
       - 0x00 (FALSE) -- Do not allow the engine to stay on for reduced
                               TTFF
       */
}qmiLocGetPositionEngineConfigParametersIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint8_t qmiLocGeofenceBreachMaskT_v02;
#define QMI_LOC_GEOFENCE_BREACH_ENTERING_MASK_V02 ((qmiLocGeofenceBreachMaskT_v02)0x01) /**<  If this mask is set, a breach event is reported
       when the geofence is entered \n  */
#define QMI_LOC_GEOFENCE_BREACH_LEAVING_MASK_V02 ((qmiLocGeofenceBreachMaskT_v02)0x02) /**<  If this mask is set, a breach event is reported
       when the geofence is exited  */
/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCGEOFENCERESPONSIVENESSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_GEOFENCE_RESPONSIVENESS_LOW_V02 = 0x01, /**<  Geofence is monitored for a breach at a
       low rate of 15 minutes. The gap between the actual breach and
       the time it is reported is higher. This
       setting results in lower power usage. \n  */
  eQMI_LOC_GEOFENCE_RESPONSIVENESS_MED_V02 = 0x02, /**<  Geofence is monitored for a breach at a
       medium rate of 2 minutes. This is the default setting. \n  */
  eQMI_LOC_GEOFENCE_RESPONSIVENESS_HIGH_V02 = 0x03, /**<  Geofence is monitored for a breach at a
       high rate of 10 seconds. The gap between the actual breach and
       the time it is reported is low. This results
       in higher power usage. \n  */
  eQMI_LOC_GEOFENCE_RESPONSIVENESS_ULTRA_HIGH_V02 = 0x04, /**<  Geofence is monitored for a breach at a
       very high rate of 1 second. The gap between the actual breach and
       the time it is reported is very low, resulting
       in very high power usage. Avoid this setting whenever
       possible because of the drastic power implications.\n   */
  eQMI_LOC_GEOFENCE_RESPONSIVENESS_CUSTOM_V02 = 0x05, /**<  Geofence is monitored for a breach at a
       user defined rate. The gap between the actual breach and
       the time it is reported depends on the user setting. The power implication
       is inversely proportional to the responsiveness value set by the user.
       The higher the responsiveness value, the lower the power implications, and vice-versa.  */
  QMILOCGEOFENCERESPONSIVENESSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocGeofenceResponsivenessEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  double latitude;
  /**<   Latitude of the center of the geofence.*/

  double longitude;
  /**<   Longitude of the center of the geofence.*/

  uint32_t radius;
  /**<   Radius of the circular geofence in meters. */
}qmiLocCircularGeofenceArgsStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCGEOFENCEPOSITIONENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_GEOFENCE_POSITION_INSIDE_V02 = 0x01, /**<  Position is inside a geofence \n  */
  eQMI_LOC_GEOFENCE_POSITION_OUTSIDE_V02 = 0x02, /**<  Position is outside a geofence  */
  QMILOCGEOFENCEPOSITIONENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocGeofencePositionEnumT_v02;
/**
    @}
  */

typedef uint8_t qmiLocGeofenceDwellTypeMaskT_v02;
#define QMI_LOC_GEOFENCE_DWELL_TYPE_INSIDE_MASK_V02 ((qmiLocGeofenceDwellTypeMaskT_v02)0x01) /**<  If this mask is set, a dwell event is reported
       when a user dwells inside the geofence for a specified time \n  */
#define QMI_LOC_GEOFENCE_DWELL_TYPE_OUTSIDE_MASK_V02 ((qmiLocGeofenceDwellTypeMaskT_v02)0x02) /**<  If this mask is set, a dwell event is reported
       when a user dwells outside the geofence for a specified time  */
/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Adds a circular geofence. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction; the transaction ID
       is returned in the QMI_LOC_ADD_CIRCULAR_GEOFENCE_IND
       indication. */

  /* Mandatory */
  /*  Circular Geofence Arguments */
  qmiLocCircularGeofenceArgsStructT_v02 circularGeofenceArgs;
  /**<   \n Latitude, longitude, and radius that define
       this geofence. */

  /* Mandatory */
  /*  Breach Event Mask */
  qmiLocGeofenceBreachMaskT_v02 breachMask;
  /**<   Breach events in which the client is interested.
 Valid values: \n 
      - QMI_LOC_GEOFENCE_BREACH_ENTERING_MASK (0x01) --  If this mask is set, a breach event is reported
       when the geofence is entered \n 
      - QMI_LOC_GEOFENCE_BREACH_LEAVING_MASK (0x02) --  If this mask is set, a breach event is reported
       when the geofence is exited 
 */

  /* Mandatory */
  /*  Include Position in Breach Event */
  uint8_t includePosition;
  /**<   Specifies whether the geofence engine is to include the position
       in a breach event.
       Valid values: \n
       - 0x01 (TRUE) -- Position is reported with the breach event \n
       - 0x00 (FALSE) -- Position is not reported with the breach
                                event
       */

  /* Optional */
  /*  Responsiveness */
  uint8_t responsiveness_valid;  /**< Must be set to true if responsiveness is being passed */
  qmiLocGeofenceResponsivenessEnumT_v02 responsiveness;
  /**<   Rate of detection for a geofence breach.
 This can impact the time lag between the actual breach event and
 when it is reported. This parameter has power implications
 and is fine-tuned to optimize power savings.
 Valid values: \n
      - eQMI_LOC_GEOFENCE_RESPONSIVENESS_LOW (0x01) --  Geofence is monitored for a breach at a
       low rate of 15 minutes. The gap between the actual breach and
       the time it is reported is higher. This
       setting results in lower power usage. \n 
      - eQMI_LOC_GEOFENCE_RESPONSIVENESS_MED (0x02) --  Geofence is monitored for a breach at a
       medium rate of 2 minutes. This is the default setting. \n 
      - eQMI_LOC_GEOFENCE_RESPONSIVENESS_HIGH (0x03) --  Geofence is monitored for a breach at a
       high rate of 10 seconds. The gap between the actual breach and
       the time it is reported is low. This results
       in higher power usage. \n 
      - eQMI_LOC_GEOFENCE_RESPONSIVENESS_ULTRA_HIGH (0x04) --  Geofence is monitored for a breach at a
       very high rate of 1 second. The gap between the actual breach and
       the time it is reported is very low, resulting
       in very high power usage. Avoid this setting whenever
       possible because of the drastic power implications.\n  
      - eQMI_LOC_GEOFENCE_RESPONSIVENESS_CUSTOM (0x05) --  Geofence is monitored for a breach at a
       user defined rate. The gap between the actual breach and
       the time it is reported depends on the user setting. The power implication
       is inversely proportional to the responsiveness value set by the user.
       The higher the responsiveness value, the lower the power implications, and vice-versa. 
 */

  /* Optional */
  /*  Confidence */
  uint8_t confidence_valid;  /**< Must be set to true if confidence is being passed */
  qmiLocGeofenceConfidenceEnumT_v02 confidence;
  /**<   Given a breach event, the confidence determines the probability
 that the breach happened at the geofence boundary.
 This parameter has power implications and
 is fine-tuned to optimize power savings.
 Valid values: \n
      - eQMI_LOC_GEOFENCE_CONFIDENCE_LOW (0x01) --  Geofence engine indicates a breach with
       low confidence; this setting results in lower
       power usage, and it can impact the yield because
       incorrect breach events might be sent \n
      - eQMI_LOC_GEOFENCE_CONFIDENCE_MED (0x02) --  (Default) geofence engine indicates a breach with
       medium confidence \n
      - eQMI_LOC_GEOFENCE_CONFIDENCE_HIGH (0x03) --  Geofence engine indicates a breach with
       high confidence; this setting results in higher
       power usage 
 */

  /* Optional */
  /*  Custom Responsiveness Value */
  uint8_t customResponsivenessValue_valid;  /**< Must be set to true if customResponsivenessValue is being passed */
  uint32_t customResponsivenessValue;
  /**<   Specifies in seconds the user-defined rate of detection for a geofence breach.
       This can impact the time lag between the actual breach event and
       when it is reported. The gap between the actual breach and
       the time it is reported depends on the user setting. The power implication
       is inversely proportional to the responsiveness value set by the user.
       The higher the responsiveness value, the lower the power implications, and vice-versa.
       If this field is set, the responsiveness is always treated
       as eQMI_LOC_GEOFENCE_ RESPONSIVENESS_CUSTOM.
       The minimum value supported in this field is one second, and the maximum value is 65535 seconds.
       An error is returned if an attempt is made to set this to an unsupported value.
       If this field is set, the responsiveness is always treated
       as eQMI_LOC_GEOFENCE_ RESPONSIVENESS_CUSTOM, which means that the other responsiveness
       types, such as eQMI_LOC_GEOFENCE _RESPONSIVENESS_LOW, eQMI_LOC_GEOFENCE_ RESPONSIVENESS_MEDIUM,
       eQMI_LOC_GEOFENCE_ RESPONSIVENESS_HIGH, and eQMI_LOC_GEOFENCE_ RESPONSIVENESS_ULTRA_HIGH are all
       disregarded.
       If this field is not set, the responsiveness is treated as
       eQMI_LOC_GEOFENCE_ RESPONSIVENESS_LOW, eQMI_LOC_GEOFENCE_RESPONSIVENESS_MEDIUM,
       eQMI_LOC_GEOFENCE_RESPONSIVENESS_HIGH, or eQMI_LOC_GEOFENCE_RESPONSIVENESS_ULTRA_HIGH.
  */

  /* Optional */
  /*  Dwell Time of Geofence */
  uint8_t dwellTime_valid;  /**< Must be set to true if dwellTime is being passed */
  uint32_t dwellTime;
  /**<   Dwell time is the time in seconds a user spends in the geofence before a dwell
       event is sent.
  */

  /* Optional */
  /*  Geofence Dwell Type */
  uint8_t dwellTypeMask_valid;  /**< Must be set to true if dwellTypeMask is being passed */
  qmiLocGeofenceDwellTypeMaskT_v02 dwellTypeMask;
  /**<   Type of dwell event in which the user is interested.
 Valid values: \n
      - QMI_LOC_GEOFENCE_DWELL_TYPE_INSIDE_MASK (0x01) --  If this mask is set, a dwell event is reported
       when a user dwells inside the geofence for a specified time \n 
      - QMI_LOC_GEOFENCE_DWELL_TYPE_OUTSIDE_MASK (0x02) --  If this mask is set, a dwell event is reported
       when a user dwells outside the geofence for a specified time 
 */
}qmiLocAddCircularGeofenceReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Adds a circular geofence. */
typedef struct {

  /* Mandatory */
  /*  Add Circular Geofence Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_ADD_CIRCULAR_GEOFENCE_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Transaction ID */
  uint8_t transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32_t transactionId;
  /**<   Transaction ID specified in the QMI_LOC_ADD_CIRCULAR_GEOFENCE_REQ request. 
       This parameter is always present if the status field is set to SUCCESS. */

  /* Optional */
  /*  Geofence ID */
  uint8_t geofenceId_valid;  /**< Must be set to true if geofenceId is being passed */
  uint32_t geofenceId;
  /**<   Geofence identifier allocated by the engine.
       The client must include this identifier in all transactions
       pertaining to this geofence. */
}qmiLocAddCircularGeofenceIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Deletes a geofence. */
typedef struct {

  /* Mandatory */
  /*  Geofence ID */
  uint32_t geofenceId;
  /**<   Identifier for the geofence to delete. */

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction. The transaction ID
       is returned in the QMI_LOC_DELETE_GEOFENCE_IND
       indication. */
}qmiLocDeleteGeofenceReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Deletes a geofence. */
typedef struct {

  /* Mandatory */
  /*  Delete Geofence Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_DELETE_GEOFENCE_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Geofence ID */
  uint8_t geofenceId_valid;  /**< Must be set to true if geofenceId is being passed */
  uint32_t geofenceId;
  /**<   Identifier for the deleted geofence. */

  /* Optional */
  /*  Transaction ID */
  uint8_t transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32_t transactionId;
  /**<   Transaction ID specified in the QMI_LOC_DELETE_GEOFENCE_REQ request.
       This parameter is always present if the status field is set to SUCCESS. */
}qmiLocDeleteGeofenceIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocMotionDetectionSourceMaskT_v02;
#define QMI_LOC_MOTION_DETECTION_SOURCE_SENSORS_V02 ((qmiLocMotionDetectionSourceMaskT_v02)0x00000001) /**<  Sensors are used for motion detection \n  */
#define QMI_LOC_MOTION_DETECTION_SOURCE_WIFI_V02 ((qmiLocMotionDetectionSourceMaskT_v02)0x00000002) /**<  Wi-Fi is used for motion detection \n */
#define QMI_LOC_MOTION_DETECTION_SOURCE_WWAN_V02 ((qmiLocMotionDetectionSourceMaskT_v02)0x00000004) /**<  Wireless WAN is used for motion detection  */
/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCGEOFENCEMOTIONSTATESENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_GEOFENCE_MOTION_STATE_STATIONARY_V02 = 0, /**<  Motion state Stationary -- Handset is completely stationary \n  */
  eQMI_LOC_GEOFENCE_MOTION_STATE_FIDDLE_V02 = 1, /**<  Motion state Fiddle -- Handset is not in motion but is being "fiddled" with \n  */
  eQMI_LOC_GEOFENCE_MOTION_STATE_WALK_V02 = 2, /**<  Motion state Walk -- User is walking with the handset \n  */
  eQMI_LOC_GEOFENCE_MOTION_STATE_RUN_V02 = 3, /**<  Motion state Run -- User is running with the handset \n  */
  eQMI_LOC_GEOFENCE_MOTION_STATE_DRIVE_V02 = 4, /**<  Motion state Drive -- User is driving with the handset    */
  QMILOCGEOFENCEMOTIONSTATESENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocGeofenceMotionStatesEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocGeofenceMotionStatesEnumT_v02 motionState;
  /**<   Motion state for which information is being configured. */

  float motionStateSpeed;
  /**<    Motion state speed in milliseconds; positive floating values.
        The state speed must be configured carefully. Very low speed
        configuration for a state can result in missing geofence
        breaches in some scenarios.

        Typical motion state speeds: \n
        - Stationary speed -- 0 meters per secpnd
        - Fiddle speed -- 0 meters per second \n
        - Walk speed -- 3 meters per second    \n
        - Run speed -- 8 meters per second \n
        - Drive speed -- 56 meters per second
  */
}qmiLocGeofenceMotionStateConfigStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Sets the geofence engine configuration. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction. The transaction ID
       is returned with the QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_IND
       indication. */

  /* Optional */
  /*  GNSS Unavailable Indication Timeout */
  uint8_t gnssUnavailableIndicationTimeout_valid;  /**< Must be set to true if gnssUnavailableIndicationTimeout is being passed */
  uint32_t gnssUnavailableIndicationTimeout;
  /**<   In a bad GNSS environment, this is the timeout after which the geofence
       engine sends out a GNSS Unavailable indication. The GNSS Unavailable
       indication is sent under the following conditions: \n
       - If gnssUnavailableIndicationTimeout is less than
             gnssPositionSessionTimeout, the GNSS
             Unavailable timeout indication is sent after
             gnssPositionSessionTimeout expires \n
       - If gnssPositionSessionTimeout is less than
             gnssUnavailableIndicationTimeout, the
             GNSS Unavailable timeout indication is sent after
             gnssUnavailableIndicationTimeout expires
         */

  /* Optional */
  /*  Max Geofences */
  uint8_t maxGeofences_valid;  /**< Must be set to true if maxGeofences is being passed */
  uint32_t maxGeofences;
  /**<   Maximum number of geofences that the geofence engine can support.
       If this number is less than the currently deployed
       geofences, this command fails.
       If the command succeeds, the engine supports the maximum number of
       geofences requested, provided there is enough memory to support that
       many geofences. Increasing this value to a very large number in a
       constrained memory environment might affect other modules negatively.
       This value is determined by phone manufacturers. The default value
       is 200. */

  /* Optional */
  /*  Enable Motion Detection Sources */
  uint8_t enableMotionDetectionSources_valid;  /**< Must be set to true if enableMotionDetectionSources is being passed */
  qmiLocMotionDetectionSourceMaskT_v02 enableMotionDetectionSources;
  /**<   Sources that the geofence enginer can enable for motion detection.
 The sources of motion detection that are enabled
 by the geofence engine are dependent on the platform.
 These sources can only be set once at boot time and they are not expected to change after that.
 Any attempt to set the value of the motion detection sources at runtime results in an undefined behavior.
 Valid values: \n
      - QMI_LOC_MOTION_DETECTION_SOURCE_SENSORS (0x00000001) --  Sensors are used for motion detection \n 
      - QMI_LOC_MOTION_DETECTION_SOURCE_WIFI (0x00000002) --  Wi-Fi is used for motion detection \n
      - QMI_LOC_MOTION_DETECTION_SOURCE_WWAN (0x00000004) --  Wireless WAN is used for motion detection  */

  /* Optional */
  /*  Enable Coarse Position Injection Usage */
  uint8_t enableCpiUsage_valid;  /**< Must be set to true if enableCpiUsage is being passed */
  uint8_t enableCpiUsage;
  /**<   Indicates whether external Coarse Position Injection (CPI) is used
       by the geofence engine. \n
       - 0x01 (TRUE)  -- CPI is enabled (default) \n
       - 0x00 (FALSE) -- CPI is disabled
        */

  /* Optional */
  /*  GNSS Position QOS Session Timeout */
  uint8_t gnssPositionSessionTimeout_valid;  /**< Must be set to true if gnssPositionSessionTimeout is being passed */
  uint32_t gnssPositionSessionTimeout;
  /**<   Session timeout value (in seconds) for requesting a
         position in a bad GNSS environment.
         Valid values: \n
         - If the gnssUnavailableIndicationTimeout value is less than
         gnssPositionSessionTimeout, in a bad GNSS environment, the GNSS
         Unavailable timeout indication is sent after
         gnssPositionSessionTimeout expires. \n
         - If gnssPositionSessionTimeout is less than gnssUnavailableIndicationTimeout,
         in a bad GNSS environment, the GNSS Unavailable timeout indication
         is sent after gnssUnavailableIndicationTimeout expires. 
    */

  /* Optional */
  /*  GNSS Position Maximum Position Uncertainity Acceptable */
  uint8_t gnssPositionMaxPuncAcceptable_valid;  /**< Must be set to true if gnssPositionMaxPuncAcceptable is being passed */
  uint32_t gnssPositionMaxPuncAcceptable;
  /**<   GNSS maximum position uncertainity in meters acceptable by
         the geofence engine.
         Valid values: \n
         - All positive values
    */

  /* Optional */
  /*  Medium Responsiveness Value */
  uint8_t mediumResponsivenessValue_valid;  /**< Must be set to true if mediumResponsivenessValue is being passed */
  uint32_t mediumResponsivenessValue;
  /**<   Medium responsiveness value in seconds that the geofence engine
         uses for all medium responsiveness Geofences in the geofence engine.    
         Valid values: \n
         - Positive values (in seconds) \n
         - If the value is configured for less than 30 seconds, the value is 
               set at 30 seconds \n
         - If the value is configured for more than 600 seconds, the value is 
               set at 600 seconds \n
         - Default -- Geofence engine uses 120 seconds as the medium 
                          responsiveness value \n         
         If the medium responsiveness value is changed, the responsiveness 
         of the existing medium responsiveness geofence does not change until the next 
         position fix, which is based on the previous medium responsiveness 
         setting.
    */

  /* Optional */
  /*  Challenging GNSS Environment Minimum CPI Wait Interval */
  uint8_t chalGnssEnvMinCpiWaitInterval_valid;  /**< Must be set to true if chalGnssEnvMinCpiWaitInterval is being passed */
  uint32_t chalGnssEnvMinCpiWaitInterval;
  /**<   Number of seconds that the geofence engine must wait between 
         CPI requests in challenging a GNSS environment.
         Valid values: \n
         - Positive values (in seconds)
     */

  /* Optional */
  /*  Geofence Motion State Information */
  uint8_t motionStateInfo_valid;  /**< Must be set to true if motionStateInfo is being passed */
  uint32_t motionStateInfo_len;  /**< Must be set to # of elements in motionStateInfo */
  qmiLocGeofenceMotionStateConfigStructT_v02 motionStateInfo[QMI_LOC_GEOFENCE_MAX_MOTION_STATES_V02];
  /**<   \vspace{4pt} \n  Motion state information (for example, motion state speed) that the
         geofence engine is to use.   
   */
}qmiLocSetGeofenceEngineConfigReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Sets the geofence engine configuration. */
typedef struct {

  /* Mandatory */
  /*  Set Geofence Engine Configuration Status. */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made  */

  /* Optional */
  /*  Transaction ID */
  uint8_t transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32_t transactionId;
  /**<   Transaction ID specified in the QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_REQ
       request. This parameter is always present if the status
       field is set to SUCCESS. */
}qmiLocSetGeofenceEngineConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Gets the geofence engine configuration. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction; the transaction ID
       is returned with the QMI_LOC_GET_GEOFENCE_ENGINE_CONFIG_IND
       indication. */
}qmiLocGetGeofenceEngineConfigReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Gets the geofence engine configuration. */
typedef struct {

  /* Mandatory */
  /*  Get Geofence Engine Configuration Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_GET_GEOFENCE_ENGINE_CONFIG_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made  */

  /* Optional */
  /*  Transaction ID */
  uint8_t transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32_t transactionId;
  /**<   Transaction ID specified in the QMI_LOC_GET_GEOFENCE_ENGINE_CONFIG_REQ
       request. This parameter is always present
       if the status field is set to SUCCESS. */

  /* Optional */
  /*  GPS Unavailable Indication Timeout */
  uint8_t gnssUnavailableIndicationTimeout_valid;  /**< Must be set to true if gnssUnavailableIndicationTimeout is being passed */
  uint32_t gnssUnavailableIndicationTimeout;
  /**<   In a bad GNSS environment, the timeout after which the geofence engine
       sends out a GNSS unavailable indication. */

  /* Optional */
  /*  Max Geofences */
  uint8_t maxGeofences_valid;  /**< Must be set to true if maxGeofences is being passed */
  uint32_t maxGeofences;
  /**<   Identifies the maximum number of Geofences that are currently supported
       in the geofence engine.  */

  /* Optional */
  /*  Enabled Motion Detection Sources */
  uint8_t enabledMotionDetectionSources_valid;  /**< Must be set to true if enabledMotionDetectionSources is being passed */
  qmiLocMotionDetectionSourceMaskT_v02 enabledMotionDetectionSources;
  /**<   Identifies the sources that are currently enabled for motion detection
 by the geofence engine.
 Valid values: \n
      - QMI_LOC_MOTION_DETECTION_SOURCE_SENSORS (0x00000001) --  Sensors are used for motion detection \n 
      - QMI_LOC_MOTION_DETECTION_SOURCE_WIFI (0x00000002) --  Wi-Fi is used for motion detection \n
      - QMI_LOC_MOTION_DETECTION_SOURCE_WWAN (0x00000004) --  Wireless WAN is used for motion detection  */

  /* Optional */
  /*  Enabled for CPI Position Injection Usage */
  uint8_t enabledCpiUsage_valid;  /**< Must be set to true if enabledCpiUsage is being passed */
  uint8_t enabledCpiUsage;
  /**<   Indicates whether CPI usage is enabled.\n
       - 0x01 (TRUE)  -- Enabled \n
       - 0x00 (FALSE) -- Disabled
       */
}qmiLocGetGeofenceEngineConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Gets the best available
                    position estimate from the location engine. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction; the transaction ID
       is returned in the QMI_LOC_GET_BEST_AVAILABLE_POSITION_IND indication. */
}qmiLocGetBestAvailablePositionReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Gets the best available
                    position estimate from the location engine. */
typedef struct {

  /* Mandatory */
  /*  Get Best Available Position Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_GET_BEST_AVAILABLE_POSITION_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made  */

  /* Optional */
  /*  Transaction ID */
  uint8_t transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32_t transactionId;
  /**<   Transaction ID specified in the QMI_LOC_GET_BEST_AVAILABLE_POSITION_REQ request. This parameter is
       always present if the status field is set to SUCCESS. */

  /* Optional */
  /*  Latitude */
  uint8_t latitude_valid;  /**< Must be set to true if latitude is being passed */
  double latitude;
  /**<   Latitude (specified in WGS84 datum).\n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -90.0 to 90.0 (positive values indicate northern latitude,
         negative values indicate southern latitude)
    */

  /* Optional */
  /*  Longitude */
  uint8_t longitude_valid;  /**< Must be set to true if longitude is being passed */
  double longitude;
  /**<   Longitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -180.0 to 180.0 (positive values indicate eastern longitude,
         negative values indicate western longitude)
    */

  /* Optional */
  /*  Circular Horizontal Position Uncertainty */
  uint8_t horUncCircular_valid;  /**< Must be set to true if horUncCircular is being passed */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
       - Units -- Meters */

  /* Optional */
  /*  Altitude With Respect to Ellipsoid */
  uint8_t altitudeWrtEllipsoid_valid;  /**< Must be set to true if altitudeWrtEllipsoid is being passed */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
       - Units -- Meters \n
       - Range -- -500 to 15883 */

  /* Optional */
  /*  Vertical Uncertainty */
  uint8_t vertUnc_valid;  /**< Must be set to true if vertUnc is being passed */
  float vertUnc;
  /**<   Vertical uncertainty.\n
       - Units -- Meters */

  /* Optional */
  /*  UTC Timestamp */
  uint8_t timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64_t timestampUtc;
  /**<   UTC timestamp.\n
       - Units -- Milliseconds since Jan. 1, 1970
        */

  /* Optional */
  /*  Time Uncertainty */
  uint8_t timeUnc_valid;  /**< Must be set to true if timeUnc is being passed */
  float timeUnc;
  /**<   Time uncertainty. \n
       - Units -- Milliseconds */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty Semi-Minor Axis */
  uint8_t horUncEllipseSemiMinor_valid;  /**< Must be set to true if horUncEllipseSemiMinor is being passed */
  float horUncEllipseSemiMinor;
  /**<   Semi-minor axis of horizontal elliptical uncertainty. \n
       - Units -- Meters */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty Semi-Major Axis */
  uint8_t horUncEllipseSemiMajor_valid;  /**< Must be set to true if horUncEllipseSemiMajor is being passed */
  float horUncEllipseSemiMajor;
  /**<   Semi-major axis of horizontal elliptical uncertainty. \n
       - Units -- Meters */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty Azimuth */
  uint8_t horUncEllipseOrientAzimuth_valid;  /**< Must be set to true if horUncEllipseOrientAzimuth is being passed */
  float horUncEllipseOrientAzimuth;
  /**<   Elliptical horizontal uncertainty azimuth of orientation. \n
       - Units -- Decimal degrees \n
       - Range -- 0 to 180 */

  /* Optional */
  /*  Horizontal Circular Confidence */
  uint8_t horCircularConfidence_valid;  /**< Must be set to true if horCircularConfidence is being passed */
  uint8_t horCircularConfidence;
  /**<   Horizontal circular uncertainty confidence. \n
       - Units -- Percent \n
       - Range -- 0 to 99 */

  /* Optional */
  /*  Horizontal Elliptical Confidence */
  uint8_t horEllipticalConfidence_valid;  /**< Must be set to true if horEllipticalConfidence is being passed */
  uint8_t horEllipticalConfidence;
  /**<   Horizontal elliptical uncertainty confidence. \n
       - Units -- Percent \n
       - Range -- 0 to 99 */

  /* Optional */
  /*  Horizontal Reliability */
  uint8_t horReliability_valid;  /**< Must be set to true if horReliability is being passed */
  qmiLocReliabilityEnumT_v02 horReliability;
  /**<   Reliability of the horizontal position.
 Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Not set \n 
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Very low; use it at your own risk \n 
      - eQMI_LOC_RELIABILITY_LOW (2) --  Low; little or no cross-checking is possible \n 
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Medium; limited cross-check passed \n 
      - eQMI_LOC_RELIABILITY_HIGH (4) --  High; strong cross-check passed 
 */

  /* Optional */
  /*  Horizontal Speed */
  uint8_t horSpeed_valid;  /**< Must be set to true if horSpeed is being passed */
  float horSpeed;
  /**<   Horizontal speed. \n
       - Units -- Meters per second */

  /* Optional */
  /*  Horizontal Speed Uncertainty */
  uint8_t horSpeedUnc_valid;  /**< Must be set to true if horSpeedUnc is being passed */
  float horSpeedUnc;
  /**<   Horizontal speed uncertainty. \n
       - Units -- Meters per second */

  /* Optional */
  /*  Altitude With Respect to Sea Level */
  uint8_t altitudeWrtMeanSeaLevel_valid;  /**< Must be set to true if altitudeWrtMeanSeaLevel is being passed */
  float altitudeWrtMeanSeaLevel;
  /**<   Altitude with respect to mean sea level. \n
       - Units -- Meters */

  /* Optional */
  /*  Vertical Confidence */
  uint8_t vertConfidence_valid;  /**< Must be set to true if vertConfidence is being passed */
  uint8_t vertConfidence;
  /**<   Vertical uncertainty confidence. \n
       - Units -- Percent \n
       - Range -- 0 to 99 */

  /* Optional */
  /*  Vertical Reliability */
  uint8_t vertReliability_valid;  /**< Must be set to true if vertReliability is being passed */
  qmiLocReliabilityEnumT_v02 vertReliability;
  /**<   Reliability of the vertical position.
 Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Not set \n 
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Very low; use it at your own risk \n 
      - eQMI_LOC_RELIABILITY_LOW (2) --  Low; little or no cross-checking is possible \n 
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Medium; limited cross-check passed \n 
      - eQMI_LOC_RELIABILITY_HIGH (4) --  High; strong cross-check passed 
 */

  /* Optional */
  /*  Vertical Speed */
  uint8_t vertSpeed_valid;  /**< Must be set to true if vertSpeed is being passed */
  float vertSpeed;
  /**<   - Units -- Meters per second */

  /* Optional */
  /*  Vertical Speed Uncertainty */
  uint8_t vertSpeedUnc_valid;  /**< Must be set to true if vertSpeedUnc is being passed */
  float vertSpeedUnc;
  /**<   - Units -- Meters per second */

  /* Optional */
  /*  Heading */
  uint8_t heading_valid;  /**< Must be set to true if heading is being passed */
  float heading;
  /**<   - Units -- Degrees \n
         - Range -- 0 to 359.999  */

  /* Optional */
  /*  Heading Uncertainty */
  uint8_t headingUnc_valid;  /**< Must be set to true if headingUnc is being passed */
  float headingUnc;
  /**<   - Type -- Floating point \n
       - Range -- 0 to 359.999 */

  /* Optional */
  /*  Magnetic Deviation */
  uint8_t magneticDeviation_valid;  /**< Must be set to true if magneticDeviation is being passed */
  float magneticDeviation;
  /**<   Difference between the bearing to true north and the bearing shown
      on a magnetic compass. The deviation is positive when the magnetic
      north is east of true north. */

  /* Optional */
  /*  Technology Used Mask */
  uint8_t technologyMask_valid;  /**< Must be set to true if technologyMask is being passed */
  qmiLocPosTechMaskT_v02 technologyMask;
  /**<   Technology used in computing this fix.
 Valid bitmasks: \n
      - QMI_LOC_POS_TECH_MASK_SATELLITE (0x00000001) --  Satellites are used to generate the fix \n
      - QMI_LOC_POS_TECH_MASK_CELLID (0x00000002) --  Cell towers are used to generate the fix \n 
      - QMI_LOC_POS_TECH_MASK_WIFI (0x00000004) --  Wi-Fi access points are used to generate the fix \n
      - QMI_LOC_POS_TECH_MASK_SENSORS (0x00000008) --  Sensors are used to generate the fix \n 
      - QMI_LOC_POS_TECH_MASK_REFERENCE_LOCATION (0x00000010) --  Reference location is used to generate the fix \n
      - QMI_LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION (0x00000020) --  Coarse position injected into the location engine is used to
        generate the fix \n
      - QMI_LOC_POS_TECH_MASK_AFLT (0x00000040) --  AFLT is used to generate the fix \n
      - QMI_LOC_POS_TECH_MASK_HYBRID (0x00000080) --  GNSS and network-provided measurements are used to
        generate the fix 
 */

  /* Optional */
  /*  Dilution of Precision */
  uint8_t DOP_valid;  /**< Must be set to true if DOP is being passed */
  qmiLocDOPStructT_v02 DOP;
  /**<   Dilution of precision associated with this position. */

  /* Optional */
  /*  GPS Time */
  uint8_t gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  qmiLocGPSTimeStructT_v02 gpsTime;
  /**<   \n Number of weeks since Jan. 5, 1980, and
       milliseconds into the current week. */

  /* Optional */
  /*  Time Source */
  uint8_t timeSrc_valid;  /**< Must be set to true if timeSrc is being passed */
  qmiLocTimeSourceEnumT_v02 timeSrc;
  /**<   Valid values: \n
      - eQMI_LOC_TIME_SRC_INVALID (0) --  Invalid time \n
      - eQMI_LOC_TIME_SRC_NETWORK_TIME_TRANSFER (1) --  Time is set by the 1X system \n 
      - eQMI_LOC_TIME_SRC_NETWORK_TIME_TAGGING (2) --  Time is set by WCDMA/GSM time tagging (that is,
       associating network time with GPS time) \n
      - eQMI_LOC_TIME_SRC_EXTERNAL_INPUT (3) --  Time is set by an external injection \n
      - eQMI_LOC_TIME_SRC_TOW_DECODE (4) --  Time is set after decoding over-the-air GPS navigation data
       from one GPS satellite \n
      - eQMI_LOC_TIME_SRC_TOW_CONFIRMED (5) --  Time is set after decoding over-the-air GPS navigation data
       from multiple satellites \n
      - eQMI_LOC_TIME_SRC_TOW_AND_WEEK_CONFIRMED (6) --  Both time of the week and the GPS week number are known \n
      - eQMI_LOC_TIME_SRC_NAV_SOLUTION (7) --  Time is set by the position engine after the fix is obtained \n
      - eQMI_LOC_TIME_SRC_SOLVE_FOR_TIME (8) --  Time is set by the position engine after performing SFT;
       this is done when the clock time uncertainty is large \n
      - eQMI_LOC_TIME_SRC_GLO_TOW_DECODE (9) --  Time is set after decoding GLONASS satellites \n
      - eQMI_LOC_TIME_SRC_TIME_TRANSFORM (10) --  Time is set after transforming the GPS to GLONASS time \n
      - eQMI_LOC_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING (11) --  Time is set by the sleep time tag provided by the WCDMA network \n
      - eQMI_LOC_TIME_SRC_GSM_SLEEP_TIME_TAGGING (12) --  Time is set by the sleep time tag provided by the GSM network \n
      - eQMI_LOC_TIME_SRC_UNKNOWN (13) --  Source of the time is unknown \n
      - eQMI_LOC_TIME_SRC_SYSTEM_TIMETICK (14) --  Time is derived from the system clock (better known as the slow clock);
       GNSS time is maintained irrespective of the GNSS receiver state \n 
      - eQMI_LOC_TIME_SRC_QZSS_TOW_DECODE (15) --  Time is set after decoding QZSS satellites \n 
      - eQMI_LOC_TIME_SRC_BDS_TOW_DECODE (16) --  Time is set after decoding BDS satellites \n 
      - eQMI_LOC_TIME_SRC_GAL_TOW_DECODE (17) --  Time is set after decoding Galielo satellites  
 */

  /* Optional */
  /*  Sensor Data Usage */
  uint8_t sensorDataUsage_valid;  /**< Must be set to true if sensorDataUsage is being passed */
  qmiLocSensorUsageIndicatorStructT_v02 sensorDataUsage;
  /**<   Indicates whether sensor data was used in computing the position in this
       position report. */

  /* Optional */
  /*  SVs Used to Calculate the Fix */
  uint8_t gnssSvUsedList_valid;  /**< Must be set to true if gnssSvUsedList is being passed */
  uint32_t gnssSvUsedList_len;  /**< Must be set to # of elements in gnssSvUsedList */
  uint16_t gnssSvUsedList[QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02];
  /**<   Each entry in the list contains the SV ID of a satellite
       used for calculating this position report. The following
       information is associated with each SV ID. \n
       Range: \n
       - GPS --     1 to 32  \n
       - GLONASS -- 65 to 96  \n
       - SBAS --    120 to 158 and 183 to 191   \n
       - QZSS --    193 to 197  \n
       - BDS --     201 to 237  \n
       - Galileo --  301 to 336 
       */

  /* Optional */
  /*  Extended Dilution of Precision */
  uint8_t extDOP_valid;  /**< Must be set to true if extDOP is being passed */
  qmiLocExtDOPStructT_v02 extDOP;
  /**<   \vspace{0.06in} \n Dilution of precision associated with this position. */

  /* Optional */
  /*  Spoof Report */
  uint8_t spoofReportMask_valid;  /**< Must be set to true if spoofReportMask is being passed */
  qmiLocSpoofMaskT_v02 spoofReportMask;
  /**<   Valid bitmasks: \n
      - QMI_LOC_POSITION_SPOOFED (0x00000001) --  Set bit indicates suspected spoofing in position \n 
      - QMI_LOC_TIME_SPOOFED (0x00000002) --  Set bit indicates suspected spoofing in time \n 
      - QMI_LOC_NAVIGATION_DATA_SPOOFED (0x00000004) --  Set bit indicates suspected spoofing in navigation data 
 */

  /* Optional */
  /*  Expanded SVs Used to Calculate the Fix */
  uint8_t expandedGnssSvUsedList_valid;  /**< Must be set to true if expandedGnssSvUsedList is being passed */
  uint32_t expandedGnssSvUsedList_len;  /**< Must be set to # of elements in expandedGnssSvUsedList */
  uint16_t expandedGnssSvUsedList[QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02];
  /**<   If the service reports expandedGnssSvUsedList, gnssSvUsedList is
       not reported. Each entry in the list contains the SV ID of a satellite
       used to calculate this position report. The following
       information is associated with each SV ID. 
       Range:  \n
       - GPS --     1 to 32  \n
       - GLONASS -- 65 to 96  \n
       - QZSS --    193 to 197  \n
       - BDS --     201 to 237  \n
       - Galileo --   301 to 336 
      */

  /* Optional */
  /*  Satellite Signal Types in the SVs Used List */
  uint8_t gnssSvUsedSignalTypeList_valid;  /**< Must be set to true if gnssSvUsedSignalTypeList is being passed */
  uint32_t gnssSvUsedSignalTypeList_len;  /**< Must be set to # of elements in gnssSvUsedSignalTypeList */
  qmiLocGnssSignalTypeMaskT_v02 gnssSvUsedSignalTypeList[QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02];
  /**<   Indicates the signal type of each satellite in expandedGnssSvUsedList. The
 signal type list shall be aligned with the SVs in expandedGnssSvUsedList. A value of 0
 means invalid. \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L1CA (0x00000001) --  GPS L1CA RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L1C (0x00000002) --  GPS L1C RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L2C_L (0x00000004) --  GPS L2C_L RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L5_Q (0x00000008) --  GPS L5_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GLONASS_G1 (0x00000010) --  GLONASS G1 (L1OF) RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GLONASS_G2 (0x00000020) --  GLONASS G2 (L2OF) RF band\n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E1_C (0x00000040) --  Galileo E1_C RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E5A_Q (0x00000080) --  Galileo E5A_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E5B_Q (0x00000100) --  Galileo E5B_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B1_I (0x00000200) --  BeiDou B1_I RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B1C (0x00000400) --  BeiDou B1C RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B2_I (0x00000800) --  BeiDou B2_I RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B2A_I (0x00001000) --  BeiDou B2A_I RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L1CA (0x00002000) --  QZSS L1CA RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L1S (0x00004000) --  QZSS L1S RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L2C_L (0x00008000) --  QZSS L2C_L RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L5_Q (0x00010000) --  QZSS L5_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_SBAS_L1_CA (0x00020000) --  SBAS L1_CA RF band  */

  /* Optional */
  /*  Conformity Index */
  uint8_t conformityIndex_valid;  /**< Must be set to true if conformityIndex is being passed */
  float conformityIndex;
  /**<   Indicates how well the various input data considered for navigation solution conforms to expectations. \n
       - Range -- 0 (least conforming) to 1 (most conforming)
  */
}qmiLocGetBestAvailablePositionIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t MCC;
  /**<   GSM mobile country code. Refer to \hyperref[ITUTE212]{ITU-T E.212}. */

  uint32_t MNC;
  /**<   GSM mobile network code. Refer to \hyperref[ITUTE212]{ITU-T E.212}. */

  uint32_t LAC;
  /**<   GSM location area code. Refer to \hyperref[ITUTE212]{ITU-T E.212}. */

  uint32_t CID;
  /**<   GSM cell identification. Refer to \hyperref[ITUTE212]{ITU-T E.212}. */
}qmiLocGSMCellIdStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Injects GSM cell information into the location
                    engine. */
typedef struct {

  /* Mandatory */
  /*  GSM Cell ID */
  qmiLocGSMCellIdStructT_v02 gsmCellId;
  /**<   \vspace{0.06in} \n Identifies the GSM cell on which the device is currently camped. */

  /* Mandatory */
  /*  Roaming Status */
  uint8_t roamingStatus;
  /**<   Indicates whether the device is roaming. \n
       - 0x01 (TRUE)  -- Device is roaming \n
       - 0x00 (FALSE) -- Device is not roaming
	*/

  /* Optional */
  /*  Timing Advance */
  uint8_t timingAdvance_valid;  /**< Must be set to true if timingAdvance is being passed */
  uint32_t timingAdvance;
  /**<   Round trip delay between the MS and the BS, in units of 3.69 microseconds.
       Refer to \hyperref[TS0510]{TS 05.10} and \hyperref[TS45010]{TS 45.010}. */
}qmiLocInjectGSMCellInfoReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Injects GSM cell information into the location
                    engine. */
typedef struct {

  /* Mandatory */
  /*  Inject GSM Cell Information Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_INJECT_GSM_CELL_INFO_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made  */
}qmiLocInjectGSMCellInfoIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCBATCHINGTYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_LOCATION_BATCHING_V02 = 1, /**<  Location batching (default) \n  */
  eQMI_LOC_OUTDOOR_TRIP_BATCHING_V02 = 2, /**<  Outdoor trip batching  */
  QMILOCBATCHINGTYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocBatchingTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Gets the batching size. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction. The transaction ID is returned in the 
       QMI_LOC_GET_BATCH_SIZE_IND indication. */

  /* Mandatory */
  /*  Requested Batch Size */
  uint32_t batchSize;
  /**<   Request the service with the number of location fixes to batch. */

  /* Optional */
  /*  Batching Type */
  uint8_t batchType_valid;  /**< Must be set to true if batchType is being passed */
  qmiLocBatchingTypeEnumT_v02 batchType;
  /**<   Identifies the batching type.
 Valid values: \n
      - eQMI_LOC_LOCATION_BATCHING (1) --  Location batching (default) \n 
      - eQMI_LOC_OUTDOOR_TRIP_BATCHING (2) --  Outdoor trip batching 
 */
}qmiLocGetBatchSizeReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Gets the batching size. */
typedef struct {

  /* Mandatory */
  /*  Get Batch Size Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_GET_BATCH_SIZE_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Transaction ID specified in the QMI_LOC_GET_BATCH_SIZE_REQ request.
       */

  /* Mandatory */
  /*  Batch Size Supported */
  uint32_t batchSize;
  /**<   Number of location fixes that the service is able to batch.
       The batch size value is returned as 0 in the case of a failure status.
  */
}qmiLocGetBatchSizeIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Initiates a location batching session. */
typedef struct {

  /* Optional */
  /*  Minimum Interval Between Position Reports */
  uint8_t minInterval_valid;  /**< Must be set to true if minInterval is being passed */
  uint32_t minInterval;
  /**<   Minimum time interval, specified by the control point, that must elapse between
       position reports. \n
       - Units -- milliseconds \n
       - Default -- 60000 ms
  */

  /* Optional */
  /*  Horizontal Accuracy Level */
  uint8_t horizontalAccuracyLevel_valid;  /**< Must be set to true if horizontalAccuracyLevel is being passed */
  qmiLocAccuracyLevelEnumT_v02 horizontalAccuracyLevel;
  /**<   Horizontal accuracy level required by the control point.
 If not specified, accuracy defaults to LOW.
 Valid values: \n
      - eQMI_LOC_ACCURACY_LOW (1) --  Low accuracy \n
      - eQMI_LOC_ACCURACY_MED (2) --  Medium accuracy \n
      - eQMI_LOC_ACCURACY_HIGH (3) --  High accuracy 
 */

  /* Optional */
  /*  Fix Session Timeout Period */
  uint8_t fixSessionTimeout_valid;  /**< Must be set to true if fixSessionTimeout is being passed */
  uint32_t fixSessionTimeout;
  /**<   Configures the fix session timeout duration. \n
       - Units -- Milliseconds \n
       - Default -- 20,000 ms
  */

  /* Optional */
  /*  Minimum Distance */
  uint8_t minDistance_valid;  /**< Must be set to true if minDistance is being passed */
  uint32_t minDistance;
  /**<   Minimum distance (in meters) to traverse before a 
       position is batched. 
       If no distance is specified, the positions are batched after 
       the minInterval period expires. If both minInterval and minDistance are 
       specified, the position are batched only after minInterval has 
       expired AND minDistance has been traversed. 
  */

  /* Optional */
  /*  Batch All Positions */
  uint8_t batchAllPos_valid;  /**< Must be set to true if batchAllPos is being passed */
  uint8_t batchAllPos;
  /**<   Values: \n
       - TRUE -- All positions that are available must be batched. For example,
              if any other type of positioning is active (such as 1 Hz tracking), all 
              positions computed for that use case are also batched. This might
              result in generating the QMI_LOC_EVENT_BATCH_FULL_NOTIFICATION_IND indication earlier. \n
       - FALSE -- Only positions that meet the time and/or distance criteria are batched
                  (default).
  */

  /* Optional */
  /*  Request ID */
  uint8_t requestId_valid;  /**< Must be set to true if requestId is being passed */
  uint32_t requestId;
  /**<   Identifies the request. A batching client can start multiple batching
       requests with different batching parameters, 
       however, positions corresponding to all requests from the same client are
       batched in the same buffer. A request ID value of 0 is considered invalid. \n
       Valid values 0x01 - 0xFFFFFFFF
  */
}qmiLocStartBatchingReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Initiates a location batching session. */
typedef struct {

  /* Mandatory */
  /*  Start Batching Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_START_BATCHING_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Request ID */
  uint8_t requestId_valid;  /**< Must be set to true if requestId is being passed */
  uint32_t requestId;
  /**<   Identifies the request. A batching client can start multiple batching
       requests with different batching parameters, however, positions 
       corresponding to all requests from the same client are
       batched in the same buffer. \n
       Valid values 0x01 - 0xFFFFFFFF
  */
}qmiLocStartBatchingIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Used to notify the control point that the batched buffer is full. */
typedef struct {

  /* Mandatory */
  /*  Number of Entries in the Batch During Full Event */
  uint32_t batchCount;
  /**<   Number of entries in the batch during a full event.
  */

  /* Optional */
  /*  Batching Type */
  uint8_t batchType_valid;  /**< Must be set to true if batchType is being passed */
  qmiLocBatchingTypeEnumT_v02 batchType;
  /**<   Identifies the batching type.
 Valid values: \n
      - eQMI_LOC_LOCATION_BATCHING (1) --  Location batching (default) \n 
      - eQMI_LOC_OUTDOOR_TRIP_BATCHING (2) --  Outdoor trip batching 
 */

  /* Optional */
  /*  Accumulated OTB Distance */
  uint8_t accumulatedDistance_valid;  /**< Must be set to true if accumulatedDistance is being passed */
  uint32_t accumulatedDistance;
  /**<   Accumulated outdoor trip batching (OTB) distance if the batchType is Outdoor Trip. \n
       - Units -- Meters
  */

  /* Optional */
  /*  Number of Batched Position Reports */
  uint8_t batchedPosition_valid;  /**< Must be set to true if batchedPosition is being passed */
  uint32_t batchedPosition;
  /**<   Number of position reports that have been batched from the last
       QMI_LOC_START_OUTDOOR_TRIP_BATCHING_REQ.
  */
}qmiLocEventBatchFullIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint64_t qmiLocBatchedReportValidFieldsMaskT_v02;
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_LATITUDE_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00000001ull) /**<  Latitude field is valid for this fix \n  */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_LONGITUDE_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00000002ull) /**<  Longitude field is valid for this fix \n */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_HOR_CIR_UNC_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00000004ull) /**<  Horizontal circular uncertainty field is valid for this fix \n */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_SPEED_HOR_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00000008ull) /**<  Horizontal speed field is valid for this fix \n */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_SPEED_UNC_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00000010ull) /**<  Speed uncertainty field is valid for this fix \n */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_ALT_WRT_ELP_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00000020ull) /**<  Altitude with respect to ellipsoid field is valid for this fix \n  */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_SPEED_VER_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00000040ull) /**<  Vertical speed field is valid for this fix \n  */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_HEADING_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00000080ull) /**<  Heading field is valid for this fix \n  */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_HEADING_UNC_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00000100ull) /**<  Heading uncertainty field is valid for this fix \n */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_TECH_MASK_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00000200ull) /**<  Technology source mask field is valid for this fix \n  */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_TIMESTAMP_UTC_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00000400ull) /**<  UTC timestamp field is valid for this fix \n  */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_TIME_UNC_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00000800ull) /**<  Time uncertainty field is valid for this fix \n  */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_MAGNETIC_DEV_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00001000ull) /**<  Magnetic deviation field is valid for this fix \n  */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_VERT_UNC_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00002000ull) /**<  Vertical uncertainty field is valid for this fix \n  */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_HOR_CONF_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00004000ull) /**<  Horizontal confidence field is valid for this fix \n  */
#define QMI_LOC_BATCHED_REPORT_MASK_VALID_TIMESTAMP_GPS_V02 ((qmiLocBatchedReportValidFieldsMaskT_v02)0x00008000ull) /**<  GPS timestamp field is valid for this fix  */
/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t fixId;
  /**<   Fix count for the session. The count starts at 0 and increments by one for
       each successive batched position report for a particular session. */

  qmiLocBatchedReportValidFieldsMaskT_v02 validFields;
  /**<   Mask of all valid fields for this fix.
 Valid bitmasks: \n
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_LATITUDE (0x00000001) --  Latitude field is valid for this fix \n 
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_LONGITUDE (0x00000002) --  Longitude field is valid for this fix \n
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_HOR_CIR_UNC (0x00000004) --  Horizontal circular uncertainty field is valid for this fix \n
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_SPEED_HOR (0x00000008) --  Horizontal speed field is valid for this fix \n
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_SPEED_UNC (0x00000010) --  Speed uncertainty field is valid for this fix \n
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_ALT_WRT_ELP (0x00000020) --  Altitude with respect to ellipsoid field is valid for this fix \n 
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_SPEED_VER (0x00000040) --  Vertical speed field is valid for this fix \n 
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_HEADING (0x00000080) --  Heading field is valid for this fix \n 
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_HEADING_UNC (0x00000100) --  Heading uncertainty field is valid for this fix \n
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_TECH_MASK (0x00000200) --  Technology source mask field is valid for this fix \n 
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_TIMESTAMP_UTC (0x00000400) --  UTC timestamp field is valid for this fix \n 
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_TIME_UNC (0x00000800) --  Time uncertainty field is valid for this fix \n 
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_MAGNETIC_DEV (0x00001000) --  Magnetic deviation field is valid for this fix \n 
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_VERT_UNC (0x00002000) --  Vertical uncertainty field is valid for this fix \n 
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_HOR_CONF (0x00004000) --  Horizontal confidence field is valid for this fix \n 
      - QMI_LOC_BATCHED_REPORT_MASK_VALID_TIMESTAMP_GPS (0x00008000) --  GPS timestamp field is valid for this fix 
 */

  double latitude;
  /**<   Latitude (specified in WGS84 datum).\n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -90.0 to 90.0 (positive values indicate northern latitude,
         negative values indicate southern latitude)
    */

  double longitude;
  /**<   Longitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -180.0 to 180.0 (positive values indicate eastern longitude,
         negative values indicate western longitude)
      */

  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
       - Units -- Meters */

  float speedHorizontal;
  /**<   Horizontal speed.\n
       - Units -- Meters per second */

  float speedUnc;
  /**<   3D speed uncertainty.\n
       - Units: Meters/second */

  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
       - Units -- Meters \n
       - Range -- -500 to 15883 */

  float speedVertical;
  /**<   Vertical speed.\n
         - Units -- Meters per second */

  float heading;
  /**<   Heading.\n
         - Units -- Degrees \n
         - Range -- 0 to 359.999  */

  float headingUnc;
  /**<   Heading uncertainty.\n
       - Units -- Degrees \n
       - Range -- 0 to 359.999 */

  qmiLocPosTechMaskT_v02 technologyMask;
  /**<   Technology used in computing this fix.
 Valid bitmasks: \n
      - QMI_LOC_POS_TECH_MASK_SATELLITE (0x00000001) --  Satellites are used to generate the fix \n
      - QMI_LOC_POS_TECH_MASK_CELLID (0x00000002) --  Cell towers are used to generate the fix \n 
      - QMI_LOC_POS_TECH_MASK_WIFI (0x00000004) --  Wi-Fi access points are used to generate the fix \n
      - QMI_LOC_POS_TECH_MASK_SENSORS (0x00000008) --  Sensors are used to generate the fix \n 
      - QMI_LOC_POS_TECH_MASK_REFERENCE_LOCATION (0x00000010) --  Reference location is used to generate the fix \n
      - QMI_LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION (0x00000020) --  Coarse position injected into the location engine is used to
        generate the fix \n
      - QMI_LOC_POS_TECH_MASK_AFLT (0x00000040) --  AFLT is used to generate the fix \n
      - QMI_LOC_POS_TECH_MASK_HYBRID (0x00000080) --  GNSS and network-provided measurements are used to
        generate the fix 
 */

  uint64_t timestampUtc;
  /**<   UTC timestamp. \n
       - Units -- Milliseconds since Jan. 1, 1970 */

  float timeUnc;
  /**<   Time uncertainty. \n
       - Units: Milliseconds */

  float magneticDeviation;
  /**<   Difference between the bearing to true north and the bearing shown
      on a magnetic compass. The deviation is positive when the magnetic
      north is east of true north. */

  float vertUnc;
  /**<   Vertical uncertainty.\n
       - Units -- Meters */

  uint8_t horConfidence;
  /**<   Horizontal confidence. \n
       - Units -- Percent \n
       - Range -- 0 to 99 */

  qmiLocGPSTimeStructT_v02 gpsTime;
  /**<   Number of weeks since Jan. 5, 1980, and milliseconds into the current week. */
}qmiLocBatchedReportStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Used to notify the control point with the live batched
                    position report. */
typedef struct {

  /* Mandatory */
  /*  Batched Position Report */
  qmiLocBatchedReportStructT_v02 liveBatchedReport;
  /**<   \n Live position report that is also batched. */
}qmiLocEventLiveBatchedPositionReportIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Retrieves fixes from the batch. */
typedef struct {

  /* Mandatory */
  /*  Number of Fix Entries to Retrieve from Batch */
  uint32_t numberOfEntries;
  /**<   Number of fix entries to retrieve from the batch. \n
       Maximum limit -- QMI_LOC_READ_FROM_BATCH_MAX_SIZE.
  */

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction. The transaction ID is returned in the 
       QMI_LOC_READ_FROM_BATCH_IND indication. */

  /* Optional */
  /*  Batching Type */
  uint8_t batchType_valid;  /**< Must be set to true if batchType is being passed */
  qmiLocBatchingTypeEnumT_v02 batchType;
  /**<   Identifies the batching type.
 Valid values: \n
      - eQMI_LOC_LOCATION_BATCHING (1) --  Location batching (default) \n 
      - eQMI_LOC_OUTDOOR_TRIP_BATCHING (2) --  Outdoor trip batching 
 */
}qmiLocReadFromBatchReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Retrieves fixes from the batch. */
typedef struct {

  /* Mandatory */
  /*  Read from Batch Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_READ_FROM_BATCH_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Transaction ID specified in the QMI_LOC_READ_FROM_BATCH_REQ
       request.
       */

  /* Optional */
  /*  Number of Fix Entries Returned from the Batch */
  uint8_t numberOfEntries_valid;  /**< Must be set to true if numberOfEntries is being passed */
  uint32_t numberOfEntries;
  /**<   Number of fix entries returned from the batch. */

  /* Optional */
  /*  List of Batched Position Reports Returned */
  uint8_t batchedReportList_valid;  /**< Must be set to true if batchedReportList is being passed */
  uint32_t batchedReportList_len;  /**< Must be set to # of elements in batchedReportList */
  qmiLocBatchedReportStructT_v02 batchedReportList[QMI_LOC_READ_FROM_BATCH_MAX_SIZE_V02];
  /**<    */
}qmiLocReadFromBatchIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Stops an ongoing batching session. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Transaction ID of the request. */

  /* Optional */
  /*  Request ID */
  uint8_t requestId_valid;  /**< Must be set to true if requestId is being passed */
  uint32_t requestId;
  /**<   Identifies the location batching request that must be stopped. 
       A location batching client can start multiple batching requests. \n 
       Valid values: \n
       - 0x01 -- 0xFFFFFFFF
  */

  /* Optional */
  /*  Batching Type */
  uint8_t batchType_valid;  /**< Must be set to true if batchType is being passed */
  qmiLocBatchingTypeEnumT_v02 batchType;
  /**<   Identifies the batching type.
 Valid values: \n
      - eQMI_LOC_LOCATION_BATCHING (1) --  Location batching (default) \n 
      - eQMI_LOC_OUTDOOR_TRIP_BATCHING (2) --  Outdoor trip batching 
 */
}qmiLocStopBatchingReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Stops an ongoing batching session. */
typedef struct {

  /* Mandatory */
  /*  Stop Batching Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_STOP_BATCHING_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Transaction ID that was specified in the QMI_LOC_STOP_BATCHING_REQ request.
  */

  /* Optional */
  /*  Request ID */
  uint8_t requestId_valid;  /**< Must be set to true if requestId is being passed */
  uint32_t requestId;
  /**<   Identifies the location batching request that was stopped. 
       A location batching client can start multiple batching requests. \n
       Valid values: \n
       - 0x01 -- 0xFFFFFFFF
  */
}qmiLocStopBatchingIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Releases the batching buffer. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction. */

  /* Optional */
  /*  Batching Type */
  uint8_t batchType_valid;  /**< Must be set to true if batchType is being passed */
  qmiLocBatchingTypeEnumT_v02 batchType;
  /**<   Identifies the batching type.
 Valid values: \n
      - eQMI_LOC_LOCATION_BATCHING (1) --  Location batching (default) \n 
      - eQMI_LOC_OUTDOOR_TRIP_BATCHING (2) --  Outdoor trip batching 
 */
}qmiLocReleaseBatchReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Releases the batching buffer. */
typedef struct {

  /* Mandatory */
  /*  Release Batch Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_RELEASE_BATCH_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Transaction ID that was specified in the QMI_LOC_RELEASE_BATCH_REQ request.
  */
}qmiLocReleaseBatchIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint64_t qmiLocGtpLocationTypeMaskT_v02;
#define QMI_LOC_GTP_BASIC_MASK_V02 ((qmiLocGtpLocationTypeMaskT_v02)0x00000001ull) /**<  When set, this mask indicates basic GTP 
	   is requested \n  */
#define QMI_LOC_GTP_ENHANCED_MASK_V02 ((qmiLocGtpLocationTypeMaskT_v02)0x00000002ull) /**<  When set, this mask indicates enhanced GTP 
	   is requested  */
/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Gets the first available WWAN position from the location engine. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction; the transaction ID
       is returned in the QMI_LOC_GET_AVAILABLE_WWAN_POSITION_IND indication. */

  /* Optional */
  /*  GTP Location Mask */
  uint8_t gtpLocMask_valid;  /**< Must be set to true if gtpLocMask is being passed */
  qmiLocGtpLocationTypeMaskT_v02 gtpLocMask;
  /**<   Identifies the GTP request type 
 Valid bitmasks: \n
      - QMI_LOC_GTP_BASIC_MASK (0x00000001) --  When set, this mask indicates basic GTP 
	   is requested \n 
      - QMI_LOC_GTP_ENHANCED_MASK (0x00000002) --  When set, this mask indicates enhanced GTP 
	   is requested 
 */

  /* Optional */
  /*  Time Out */
  uint8_t timeOut_valid;  /**< Must be set to true if timeOut is being passed */
  uint8_t timeOut;
  /**<   Indicates the current request timeout. \n
       - Units -- Seconds */
}qmiLocGetAvailWwanPositionReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Gets the first available WWAN position from the location engine. */
typedef struct {

  /* Mandatory */
  /*  Get Available WWAN Position Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_GET_AVAILABLE_WWAN_POSITION_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made  */

  /* Optional */
  /*  Transaction ID */
  uint8_t transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32_t transactionId;
  /**<   Transaction ID that was specified in the QMI_LOC_GET_AVAILABLE_WWAN_POSITION_REQ
       request. This parameter is always present when the status field is set to
       SUCCESS. */

  /* Optional */
  /*  Latitude */
  uint8_t latitude_valid;  /**< Must be set to true if latitude is being passed */
  double latitude;
  /**<   Latitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -90.0 to 90.0 (positive values indicate northern latitude,
         negative values indicate southern latitude)
    */

  /* Optional */
  /*  Longitude */
  uint8_t longitude_valid;  /**< Must be set to true if longitude is being passed */
  double longitude;
  /**<   Longitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -180.0 to 180.0 (positive values indicate eastern longitude,
         negative values indicate western longitude)
      */

  /* Optional */
  /*  Circular Horizontal Position Uncertainty */
  uint8_t horUncCircular_valid;  /**< Must be set to true if horUncCircular is being passed */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
       - Units -- Meters */

  /* Optional */
  /*  Altitude With Respect to Ellipsoid */
  uint8_t altitudeWrtEllipsoid_valid;  /**< Must be set to true if altitudeWrtEllipsoid is being passed */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
       - Units -- Meters \n
       - Range -- -500 to 15883 */

  /* Optional */
  /*  Vertical Uncertainty */
  uint8_t vertUnc_valid;  /**< Must be set to true if vertUnc is being passed */
  float vertUnc;
  /**<   - Units -- Meters */

  /* Optional */
  /*  UTC Timestamp */
  uint8_t timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64_t timestampUtc;
  /**<   - Units -- Milliseconds since Jan. 1, 1970 */

  /* Optional */
  /*  Time Uncertainty */
  uint8_t timeUnc_valid;  /**< Must be set to true if timeUnc is being passed */
  float timeUnc;
  /**<   - Units -- Milliseconds */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty Semi-Minor Axis */
  uint8_t horUncEllipseSemiMinor_valid;  /**< Must be set to true if horUncEllipseSemiMinor is being passed */
  float horUncEllipseSemiMinor;
  /**<   Semi-minor axis of horizontal elliptical uncertainty. \n
       - Units -- Meters */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty Semi-Major Axis */
  uint8_t horUncEllipseSemiMajor_valid;  /**< Must be set to true if horUncEllipseSemiMajor is being passed */
  float horUncEllipseSemiMajor;
  /**<   Semi-major axis of horizontal elliptical uncertainty. \n
       - Units: Meters */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty Azimuth */
  uint8_t horUncEllipseOrientAzimuth_valid;  /**< Must be set to true if horUncEllipseOrientAzimuth is being passed */
  float horUncEllipseOrientAzimuth;
  /**<   Elliptical horizontal uncertainty azimuth of orientation. \n
       - Units -- Decimal degrees \n
       - Range -- 0 to 180 */

  /* Optional */
  /*  Horizontal Circular Confidence */
  uint8_t horCircularConfidence_valid;  /**< Must be set to true if horCircularConfidence is being passed */
  uint8_t horCircularConfidence;
  /**<   Horizontal circular uncertainty confidence. \n
       - Units -- Percent \n
       - Range -- 0 to 99 */

  /* Optional */
  /*  Horizontal Elliptical Confidence */
  uint8_t horEllipticalConfidence_valid;  /**< Must be set to true if horEllipticalConfidence is being passed */
  uint8_t horEllipticalConfidence;
  /**<   Horizontal elliptical uncertainty confidence. \n
       - Units -- Percent \n
       - Range -- 0 to 99 */

  /* Optional */
  /*  Horizontal Reliability */
  uint8_t horReliability_valid;  /**< Must be set to true if horReliability is being passed */
  qmiLocReliabilityEnumT_v02 horReliability;
  /**<   Reliability of the horizontal position.
 Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Not set \n 
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Very low; use it at your own risk \n 
      - eQMI_LOC_RELIABILITY_LOW (2) --  Low; little or no cross-checking is possible \n 
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Medium; limited cross-check passed \n 
      - eQMI_LOC_RELIABILITY_HIGH (4) --  High; strong cross-check passed 
 */

  /* Optional */
  /*  Altitude With Respect to Sea Level */
  uint8_t altitudeWrtMeanSeaLevel_valid;  /**< Must be set to true if altitudeWrtMeanSeaLevel is being passed */
  float altitudeWrtMeanSeaLevel;
  /**<   Altitude with respect to mean sea level. \n
       - Units -- Meters */

  /* Optional */
  /*  Vertical Confidence */
  uint8_t vertConfidence_valid;  /**< Must be set to true if vertConfidence is being passed */
  uint8_t vertConfidence;
  /**<   Vertical uncertainty confidence. \n
       - Units -- Percent \n
       - Range -- 0 to 99 */

  /* Optional */
  /*  Vertical Reliability */
  uint8_t vertReliability_valid;  /**< Must be set to true if vertReliability is being passed */
  qmiLocReliabilityEnumT_v02 vertReliability;
  /**<   Reliability of the vertical position.
 Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Not set \n 
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Very low; use it at your own risk \n 
      - eQMI_LOC_RELIABILITY_LOW (2) --  Low; little or no cross-checking is possible \n 
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Medium; limited cross-check passed \n 
      - eQMI_LOC_RELIABILITY_HIGH (4) --  High; strong cross-check passed 
 */

  /* Optional */
  /*  GPS Time */
  uint8_t gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  qmiLocGPSTimeStructT_v02 gpsTime;
  /**<   \n Number of weeks since Jan. 5, 1980, and
       milliseconds into the current week. */

  /* Optional */
  /*  Time Source */
  uint8_t timeSrc_valid;  /**< Must be set to true if timeSrc is being passed */
  qmiLocTimeSourceEnumT_v02 timeSrc;
  /**<   Time source.
 Valid values: \n
      - eQMI_LOC_TIME_SRC_INVALID (0) --  Invalid time \n
      - eQMI_LOC_TIME_SRC_NETWORK_TIME_TRANSFER (1) --  Time is set by the 1X system \n 
      - eQMI_LOC_TIME_SRC_NETWORK_TIME_TAGGING (2) --  Time is set by WCDMA/GSM time tagging (that is,
       associating network time with GPS time) \n
      - eQMI_LOC_TIME_SRC_EXTERNAL_INPUT (3) --  Time is set by an external injection \n
      - eQMI_LOC_TIME_SRC_TOW_DECODE (4) --  Time is set after decoding over-the-air GPS navigation data
       from one GPS satellite \n
      - eQMI_LOC_TIME_SRC_TOW_CONFIRMED (5) --  Time is set after decoding over-the-air GPS navigation data
       from multiple satellites \n
      - eQMI_LOC_TIME_SRC_TOW_AND_WEEK_CONFIRMED (6) --  Both time of the week and the GPS week number are known \n
      - eQMI_LOC_TIME_SRC_NAV_SOLUTION (7) --  Time is set by the position engine after the fix is obtained \n
      - eQMI_LOC_TIME_SRC_SOLVE_FOR_TIME (8) --  Time is set by the position engine after performing SFT;
       this is done when the clock time uncertainty is large \n
      - eQMI_LOC_TIME_SRC_GLO_TOW_DECODE (9) --  Time is set after decoding GLONASS satellites \n
      - eQMI_LOC_TIME_SRC_TIME_TRANSFORM (10) --  Time is set after transforming the GPS to GLONASS time \n
      - eQMI_LOC_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING (11) --  Time is set by the sleep time tag provided by the WCDMA network \n
      - eQMI_LOC_TIME_SRC_GSM_SLEEP_TIME_TAGGING (12) --  Time is set by the sleep time tag provided by the GSM network \n
      - eQMI_LOC_TIME_SRC_UNKNOWN (13) --  Source of the time is unknown \n
      - eQMI_LOC_TIME_SRC_SYSTEM_TIMETICK (14) --  Time is derived from the system clock (better known as the slow clock);
       GNSS time is maintained irrespective of the GNSS receiver state \n 
      - eQMI_LOC_TIME_SRC_QZSS_TOW_DECODE (15) --  Time is set after decoding QZSS satellites \n 
      - eQMI_LOC_TIME_SRC_BDS_TOW_DECODE (16) --  Time is set after decoding BDS satellites \n 
      - eQMI_LOC_TIME_SRC_GAL_TOW_DECODE (17) --  Time is set after decoding Galielo satellites  
 */
}qmiLocGetAvailWwanPositionIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCPREMIUMSERVICEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_PREMIUM_SERVICE_GTP_CELL_V02 = 0, /**<  Premium service -- Global terrestrial positioning for the cell \n  */
  eQMI_LOC_PREMIUM_SERVICE_SAP_V02 = 1, /**<  Premium service -- Sensor-assisted positioning \n  */
  eQMI_LOC_PREMIUM_SERVICE_GTP_ENH_CELL_V02 = 2, /**<  Premium service -- Global terrestrial positioning enhanced cell \n  */
  eQMI_LOC_PREMIUM_SERVICE_GTP_WIFI_V02 = 3, /**<  Premium service -- Global terrestrial positioning for Wi-Fi  */
  QMILOCPREMIUMSERVICEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocPremiumServiceEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCPREMIUMSERVICECFGENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_PREMIUM_SERVICE_DISABLED_V02 = 0, /**<  Premium service disabled  \n */
  eQMI_LOC_PREMIUM_SERVICE_ENABLED_BASIC_V02 = 1, /**<  Premium service enabled for basic \n  */
  eQMI_LOC_PREMIUM_SERVICE_ENABLED_PREMIUM_V02 = 2, /**<  Premium service enabled for premium \n  */
  eQMI_LOC_PREMIUM_SERVICE_ENABLED_THIRDPARTY_V02 = 3, /**<  Premium service enabled for third party integration  */
  QMILOCPREMIUMSERVICECFGENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocPremiumServiceCfgEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Sets the configuration information for all premium services to the location
                    engine. */
typedef struct {

  /* Mandatory */
  /*  Set Premium Service Type */
  qmiLocPremiumServiceEnumT_v02 premiumServiceType;
  /**<   Premium service to configure.
 Valid values: \n
      - eQMI_LOC_PREMIUM_SERVICE_GTP_CELL (0) --  Premium service -- Global terrestrial positioning for the cell \n 
      - eQMI_LOC_PREMIUM_SERVICE_SAP (1) --  Premium service -- Sensor-assisted positioning \n 
      - eQMI_LOC_PREMIUM_SERVICE_GTP_ENH_CELL (2) --  Premium service -- Global terrestrial positioning enhanced cell \n 
      - eQMI_LOC_PREMIUM_SERVICE_GTP_WIFI (3) --  Premium service -- Global terrestrial positioning for Wi-Fi 
 */

  /* Mandatory */
  /*  Set Premium Service Configuration */
  qmiLocPremiumServiceCfgEnumT_v02 premiumServiceCfg;
  /**<   Premium service configuration mode.
 Valid values: \n
      - eQMI_LOC_PREMIUM_SERVICE_DISABLED (0) --  Premium service disabled  \n
      - eQMI_LOC_PREMIUM_SERVICE_ENABLED_BASIC (1) --  Premium service enabled for basic \n 
      - eQMI_LOC_PREMIUM_SERVICE_ENABLED_PREMIUM (2) --  Premium service enabled for premium \n 
      - eQMI_LOC_PREMIUM_SERVICE_ENABLED_THIRDPARTY (3) --  Premium service enabled for third party integration 
 */
}qmiLocSetPremiumServicesCfgReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Sets the configuration information for all premium services to the location
                    engine. */
typedef struct {

  /* Mandatory */
  /*  Set Premium Service Configuration Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_SET_PREMIUM_SERVICES_CONFIG_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made  */
}qmiLocSetPremiumServicesCfgIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCXTRAVERSIONCHECKENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_XTRA_VERSION_CHECK_DISABLE_V02 = 0, /**<  XTRA file version check is not required \n  */
  eQMI_LOC_XTRA_VERSION_CHECK_AUTO_V02 = 1, /**<  XTRA file version check is required; the Location service decides the 'expected version' based on the preprovisioned XTRA version configuration  \n */
  eQMI_LOC_XTRA_VERSION_CHECK_XTRA2_V02 = 2, /**<  Check the XTRA file against XTRA2 format  \n  */
  eQMI_LOC_XTRA_VERSION_CHECK_XTRA3_V02 = 3, /**<  Check the XTRA file against XTRA3 format   \n */
  eQMI_LOC_XTRA_VERSION_CHECK_XTRA3_1_V02 = 4, /**<  Check the XTRA file against XTRA3.1 format  */
  QMILOCXTRAVERSIONCHECKENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocXtraVersionCheckEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Enables or disables XTRA version verification. */
typedef struct {

  /* Mandatory */
  /*  Set XTRA Version Check Mode */
  qmiLocXtraVersionCheckEnumT_v02 xtraVersionCheckMode;
  /**<   Specifies XTRA version check mode.
 Valid values: \n
      - eQMI_LOC_XTRA_VERSION_CHECK_DISABLE (0) --  XTRA file version check is not required \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_AUTO (1) --  XTRA file version check is required; the Location service decides the 'expected version' based on the preprovisioned XTRA version configuration  \n
      - eQMI_LOC_XTRA_VERSION_CHECK_XTRA2 (2) --  Check the XTRA file against XTRA2 format  \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_XTRA3 (3) --  Check the XTRA file against XTRA3 format   \n
      - eQMI_LOC_XTRA_VERSION_CHECK_XTRA3_1 (4) --  Check the XTRA file against XTRA3.1 format 
 */
}qmiLocSetXtraVersionCheckReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Enables or disables XTRA version verification. */
typedef struct {

  /* Mandatory */
  /*  Set XTRA Version Check Mode Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_SET_XTRA_VERSION_CHECK_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made  */
}qmiLocSetXtraVersionCheckIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint64_t qmiLocGNSSConstellEnumT_v02;
#define eQMI_SYSTEM_GPS_V02 ((qmiLocGNSSConstellEnumT_v02)0x01ull) /**<  Enable GPS \n  */
#define eQMI_SYSTEM_GLO_V02 ((qmiLocGNSSConstellEnumT_v02)0x02ull) /**<  Enable GLONASS \n  */
#define eQMI_SYSTEM_BDS_V02 ((qmiLocGNSSConstellEnumT_v02)0x04ull) /**<  Enable BDS \n  */
#define eQMI_SYSTEM_GAL_V02 ((qmiLocGNSSConstellEnumT_v02)0x08ull) /**<  Enable Galileo \n  */
#define eQMI_SYSTEM_QZSS_V02 ((qmiLocGNSSConstellEnumT_v02)0x10ull) /**<  Enable QZSS  */
/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Sets satellite constellations of interest for reporting. */
typedef struct {

  /* Optional */
  /*  GNSS Measurement Report Constellation Control */
  uint8_t measReportConfig_valid;  /**< Must be set to true if measReportConfig is being passed */
  qmiLocGNSSConstellEnumT_v02 measReportConfig;
  /**<   Valid values: \n
      - eQMI_SYSTEM_GPS (0x01) --  Enable GPS \n 
      - eQMI_SYSTEM_GLO (0x02) --  Enable GLONASS \n 
      - eQMI_SYSTEM_BDS (0x04) --  Enable BDS \n 
      - eQMI_SYSTEM_GAL (0x08) --  Enable Galileo \n 
      - eQMI_SYSTEM_QZSS (0x10) --  Enable QZSS 
 */

  /* Optional */
  /*  SV Polynomial Report Constellation Control */
  uint8_t svPolyReportConfig_valid;  /**< Must be set to true if svPolyReportConfig is being passed */
  qmiLocGNSSConstellEnumT_v02 svPolyReportConfig;
  /**<   Valid values: \n
      - eQMI_SYSTEM_GPS (0x01) --  Enable GPS \n 
      - eQMI_SYSTEM_GLO (0x02) --  Enable GLONASS \n 
      - eQMI_SYSTEM_BDS (0x04) --  Enable BDS \n 
      - eQMI_SYSTEM_GAL (0x08) --  Enable Galileo \n 
      - eQMI_SYSTEM_QZSS (0x10) --  Enable QZSS 
 */

  /* Optional */
  /*  Request Full Ephemeris Data */
  uint8_t reportFullEphemerisDb_valid;  /**< Must be set to true if reportFullEphemerisDb is being passed */
  uint8_t reportFullEphemerisDb;
  /**<   Request indicating that the client must complete ephemeris. */

  /* Optional */
  /*  Request Complete SV Polynomial Data */
  uint8_t reportFullSvPolyDb_valid;  /**< Must be set to true if reportFullSvPolyDb is being passed */
  uint8_t reportFullSvPolyDb;
  /**<   Request indicating that the client must complete SV polynomials. */
}qmiLocSetGNSSConstRepConfigReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Sets satellite constellations of interest for reporting. */
typedef struct {

  /* Mandatory */
  /*  Set GNSS Constellation Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the GNSS constellation.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */
}qmiLocSetGNSSConstRepConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Injects Global Terrestrial Positioning (GTP) WWAN client downloaded data. */
typedef struct {

  /* Mandatory */
  /*  Data */
  uint32_t ClientDownloadedData_len;  /**< Must be set to # of elements in ClientDownloadedData */
  char ClientDownloadedData[QMI_LOC_MAX_GTP_WWAN_CLIENT_DOWNLOADED_DATA_LEN_V02];
  /**<   WWAN client downloaded data. \n
         - Type -- Array of bytes \n 
         - Maximum length of the array -- 512  
    */
}qmiLocInjectGtpClientDownloadedDataReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Injects Global Terrestrial Positioning (GTP) WWAN client downloaded data. */
typedef struct {

  /* Mandatory */
  /*  GTP Client Downloaded Data Injection Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the GTP client downloaded data injection.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */
}qmiLocInjectGtpClientDownloadedDataIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCDBTUSAGEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_DBT_USAGE_NAVIGATION_V02 = 1, /**<  Navigation usage type  */
  QMILOCDBTUSAGEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocDbtUsageEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCDBDISTANCETYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_DBT_DISTANCE_TYPE_STRAIGHT_LINE_V02 = 1, /**<  Straight line distance between 
       location updates   */
  QMILOCDBDISTANCETYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocDbDistanceTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Initiates a Distance Based Tracking (DBT) session. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint8_t reqId;
  /**<   ID of the request as identified by the control point; the request ID
       is reported back in the position reports. The control point must
       specify the same request ID in the QMI_LOC_STOP_DBT_REQ request. \n
       - Range -- 0 to 255
  */

  /* Mandatory */
  /*  Minimum Distance Between Position Reports */
  uint32_t minDistance;
  /**<   Minimum distance, specified by the control point, 
       that must be traversed between position reports. \n
       - Units -- Meters
  */

  /* Mandatory */
  /*  Type of Distance to Track */
  qmiLocDbDistanceTypeEnumT_v02 distanceType;
  /**<   Straight line distance or accumulated distance.
 Valid values: \n
      - eQMI_LOC_DBT_DISTANCE_TYPE_STRAIGHT_LINE (1) --  Straight line distance between 
       location updates  
 */

  /* Mandatory */
  /*  Need Origin Location */
  uint8_t needOriginLocation;
  /**<   Indicates whether the control point wants the position
       corresponding to the origin. \n
	   - 0x01 (TRUE)  -- Control point is requesting origin
                                location \n
       - 0x00 (FALSE) -- Control point is not requesting origin 
                                location
     
  */

  /* Optional */
  /*  Maximum Latency Threshold for Position Reports */
  uint8_t maxLatency_valid;  /**< Must be set to true if maxLatency is being passed */
  uint32_t maxLatency;
  /**<   Maximum time period, specified by the control point, after the minimum
       distance criteria has been met within which a location update must
       be provided. If not specified, an ideal value is assumed by the
       engine.  \n
       - Units -- seconds
  */

  /* Optional */
  /*  Usage Type */
  uint8_t usageType_valid;  /**< Must be set to true if usageType is being passed */
  qmiLocDbtUsageEnumT_v02 usageType;
  /**<   Type of usage by the control point -- specifically
 the use case category of the client. For example, a navigation client should
 set this to QMI_LOC_USAGE_NAVIGATION for better performance in difficult
 signal conditions, such as tunnels.

 If not specified, the service uses default algorithms to provide an ideal
 performance.

 Valid values: \n
      - eQMI_LOC_DBT_USAGE_NAVIGATION (1) --  Navigation usage type 
 */
}qmiLocStartDbtReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Initiates a Distance Based Tracking (DBT) session. */
typedef struct {

  /* Mandatory */
  /*  Start DBT Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_START_DBT_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Request ID */
  uint8_t reqId_valid;  /**< Must be set to true if reqId is being passed */
  uint8_t reqId;
  /**<   ID of the QMI_LOC_START_DBT_REQ request for which this
       indication was generated. */
}qmiLocStartDbtIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Stops a DBT session. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint8_t reqId;
  /**<   ID of the request specified in the QMI_LOC_START_DBT_REQ 
        request.\n
       - Range: 0 to 255 */
}qmiLocStopDbtReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Stops a DBT session. */
typedef struct {

  /* Mandatory */
  /*  Stop DBT Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_STOP_DBT_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Request ID */
  uint8_t reqId_valid;  /**< Must be set to true if reqId is being passed */
  uint8_t reqId;
  /**<   ID of the QMI_LOC_STOP_DBT_REQ request for which this
       indication was generated. */
}qmiLocStopDbtIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCDBTPOSITIONTYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_DBT_POSITION_TYPE_ORIGIN_V02 = 1, /**<  Position reported is at the origin \n */
  eQMI_LOC_DBT_POSITION_TYPE_TRACKING_V02 = 2, /**<  Position reported is a tracking type
       where the origin location has already
       been reported  */
  QMILOCDBTPOSITIONTYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocDbtPositionTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  /*  UTC Timestamp */
  uint64_t timestampUtc;
  /**<   UTC timestamp. \n
       - Units -- Milliseconds since Jan. 1, 1970
  */

  /*  Latitude */
  double latitude;
  /**<   Latitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -90.0 to 90.0 (positive values indicate northern latitude,
         negative values indicate southern latitude)
    */

  /*   Longitude */
  double longitude;
  /**<   Longitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -180.0 to 180.0 (positive values indicate eastern longitude,
         negative values indicate western longitude)
    */

  /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
  float horUncEllipseSemiMinor;
  /**<   Semi-minor axis of horizontal elliptical uncertainty.\n
       - Units -- Meters */

  /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
  float horUncEllipseSemiMajor;
  /**<   Semi-major axis of horizontal elliptical uncertainty.\n
       - Units -- Meters */

  /*  Elliptical Horizontal Uncertainty Azimuth */
  float horUncEllipseOrientAzimuth;
  /**<   Elliptical horizontal uncertainty azimuth of orientation.\n
       - Units -- Decimal degrees \n
       - Range -- 0 to 180 */

  uint8_t speedHorizontal_valid;
  /**<   Indicates whether the horizontal speed field contains valid
       information.        \n
       - 0x01 (TRUE)  --  Valid \n
       - 0x00 (FALSE) --  Invalid and is ignored 
  */

  float speedHorizontal;
  /**<   Horizontal speed.\n
       - Units -- Meters per second 
	*/

  uint8_t altitudeWrtEllipsoid_valid;
  /**<   Indicates whether the altitude field contains valid
       information.        \n
       - 0x01 (TRUE)  --  Valid \n
       - 0x00 (FALSE) --  Invalid and is ignored 
       */

  /*  Altitude With Respect to Ellipsoid */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
       - Units -- Meters \n
       - Range -- -500 to 15883
	 */

  /*  Vertical Uncertainty Validity Bit */
  uint8_t vertUnc_valid;
  /**<   Indicates whether the vertical uncertainty field contains valid
       information. \n
       - 0x01 (TRUE)  --  Vertical uncertainty field is valid \n
       - 0x00 (FALSE) --  Vertical uncertainty field is invalid
                                 and is ignored 
  */

  /*  Vertical Uncertainty */
  float vertUnc;
  /**<   Vertical uncertainty.\n
       - Units -- Meters 
	*/

  /*  Vertical Speed Validity Bit */
  uint8_t speedVertical_valid;
  /**<   Indicates whether the vertical speed field contains valid
       information. \n
       - 0x01 (TRUE)  --  Vertical speed field is valid \n
       - 0x00 (FALSE) --  Vertical speed field is invalid
                                 and is ignored 
  */

  /*  Vertical Speed */
  float speedVertical;
  /**<   Vertical speed.\n
       - Units -- Meters per second 
	*/

  /*  Heading Validity Bit */
  uint8_t heading_valid;
  /**<   Indicates whether the heading field contains valid
       information.        \n
       - 0x01 (TRUE)  --  Heading field is valid \n
       - 0x00 (FALSE) --  Heading field is invalid
                                 and is ignored 
  */

  /*  Heading */
  float heading;
  /**<   Heading.\n
        - Units -- Degrees \n
        - Range -- 0 to 359.999  
    */
}qmiLocDbtPositionStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Notifies the control point of
                    a DBT position report. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint8_t reqId;
  /**<   ID of the DBT request for which this
       report was generated. */

  /* Mandatory */
  /*  DBT Position */
  qmiLocDbtPositionStructT_v02 dbtPosition;
  /**<   \n Position of the client when it has traversed the 
       specified distance. 
       */

  /* Mandatory */
  /*  DBT Position Type */
  qmiLocDbtPositionTypeEnumT_v02 positionType;
  /**<   Specifies whether the position reported is at the
 origin of the DBT session or during the tracking
 duration of the session. Values: \n
      - eQMI_LOC_DBT_POSITION_TYPE_ORIGIN (1) --  Position reported is at the origin \n
      - eQMI_LOC_DBT_POSITION_TYPE_TRACKING (2) --  Position reported is a tracking type
       where the origin location has already
       been reported  */

  /* Optional */
  /*  Heading Uncertainty */
  uint8_t headingUnc_valid;  /**< Must be set to true if headingUnc is being passed */
  float headingUnc;
  /**<   - Units -- Degrees \n
       - Range -- 0 to 359.999 */

  /* Optional */
  /*  Speed Uncertainty */
  uint8_t speedUnc_valid;  /**< Must be set to true if speedUnc is being passed */
  float speedUnc;
  /**<   3D speed uncertainty.\n
       - Units -- Meters per second */

  /* Optional */
  /*  Horizontal Confidence */
  uint8_t horConfidence_valid;  /**< Must be set to true if horConfidence is being passed */
  uint8_t horConfidence;
  /**<   Horizontal uncertainty confidence.\n
       - Units -- Percent \n
       - Range -- 0 to 99 */

  /* Optional */
  /*  Vertical Confidence */
  uint8_t vertConfidence_valid;  /**< Must be set to true if vertConfidence is being passed */
  uint8_t vertConfidence;
  /**<   Vertical uncertainty confidence.\n
       - Units -- Percent \n
       - Range -- 0 to 99 */

  /* Optional */
  /*  Dilution of Precision */
  uint8_t DOP_valid;  /**< Must be set to true if DOP is being passed */
  qmiLocDOPStructT_v02 DOP;
  /**<   \vspace{0.06in} \n Dilution of precision associated with this position. */

  /* Optional */
  /*  SVs Used to Calculate the Fix */
  uint8_t gnssSvUsedList_valid;  /**< Must be set to true if gnssSvUsedList is being passed */
  uint32_t gnssSvUsedList_len;  /**< Must be set to # of elements in gnssSvUsedList */
  uint16_t gnssSvUsedList[QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02];
  /**<   Each entry in the list contains the SV ID of a satellite
       used for calculating this position report. The following
       information is associated with each SV ID: \n
        Range:    \n
       - GPS --     1 to 32 \n
       - GLONASS -- 65 to 96 \n
       - SBAS --    120 to 158 and 183 to 191 \n
       - QZSS --    193 to 197 \n
       - BDS --     201 to 237 \n
       - Galileo --   301 to 336
  */

  /* Optional */
  /*  Position Source */
  uint8_t positionSrc_valid;  /**< Must be set to true if positionSrc is being passed */
  qmiLocPositionSrcEnumT_v02 positionSrc;
  /**<   Source from which this position was obtained.
 Valid values: \n
      - eQMI_LOC_POSITION_SRC_GNSS (0) --  Position source is GNSS \n
      - eQMI_LOC_POSITION_SRC_CELLID (1) --  Position source is cell ID \n
      - eQMI_LOC_POSITION_SRC_ENH_CELLID (2) --  Position source is enhanced cell ID \n
      - eQMI_LOC_POSITION_SRC_WIFI (3) --  Position source is Wi-Fi \n
      - eQMI_LOC_POSITION_SRC_TERRESTRIAL (4) --  Position source is terrestrial \n
      - eQMI_LOC_POSITION_SRC_GNSS_TERRESTRIAL_HYBRID (5) --  Position source is GNSS terrestrial hybrid \n
      - eQMI_LOC_POSITION_SRC_OTHER (6) --  Other sources \n
      - eQMI_LOC_POSITION_SRC_DRE (7) --  Position source is dead reckoning engine 
 */

  /* Optional */
  /*  Extended Dilution of Precision */
  uint8_t extDOP_valid;  /**< Must be set to true if extDOP is being passed */
  qmiLocExtDOPStructT_v02 extDOP;
  /**<   \vspace{0.06in} \n Dilution of precision associated with this position. */

  /* Optional */
  /*  Expanded SVs Used to Calculate the Fix */
  uint8_t expandedGnssSvUsedList_valid;  /**< Must be set to true if expandedGnssSvUsedList is being passed */
  uint32_t expandedGnssSvUsedList_len;  /**< Must be set to # of elements in expandedGnssSvUsedList */
  uint16_t expandedGnssSvUsedList[QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02];
  /**<   If the service reports expandedGnssSvUsedList, gnssSvUsedList is
       not reported. Each entry in the list contains the SV ID of a satellite
       used to calculate this position report. The following
       information is associated with each SV ID.
       Range: \n
      - GPS --     1 to 32 \n
      - GLONASS -- 65 to 96 \n
      - QZSS --    193 to 197 \n
      - BDS --     201 to 237 \n
      - Galileo -- 301 to 336
      */

  /* Optional */
  /*  Satellite Signal Types in the SVs Used List */
  uint8_t gnssSvUsedSignalTypeList_valid;  /**< Must be set to true if gnssSvUsedSignalTypeList is being passed */
  uint32_t gnssSvUsedSignalTypeList_len;  /**< Must be set to # of elements in gnssSvUsedSignalTypeList */
  qmiLocGnssSignalTypeMaskT_v02 gnssSvUsedSignalTypeList[QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02];
  /**<   Indicates the signal type of each satellite in expandedGnssSvUsedList. The
 signal type list shall be aligned with the SVs in expandedGnssSvUsedList. Value of 0
 means invalid.
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L1CA (0x00000001) --  GPS L1CA RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L1C (0x00000002) --  GPS L1C RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L2C_L (0x00000004) --  GPS L2C_L RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L5_Q (0x00000008) --  GPS L5_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GLONASS_G1 (0x00000010) --  GLONASS G1 (L1OF) RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GLONASS_G2 (0x00000020) --  GLONASS G2 (L2OF) RF band\n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E1_C (0x00000040) --  Galileo E1_C RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E5A_Q (0x00000080) --  Galileo E5A_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E5B_Q (0x00000100) --  Galileo E5B_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B1_I (0x00000200) --  BeiDou B1_I RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B1C (0x00000400) --  BeiDou B1C RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B2_I (0x00000800) --  BeiDou B2_I RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B2A_I (0x00001000) --  BeiDou B2A_I RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L1CA (0x00002000) --  QZSS L1CA RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L1S (0x00004000) --  QZSS L1S RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L2C_L (0x00008000) --  QZSS L2C_L RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L5_Q (0x00010000) --  QZSS L5_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_SBAS_L1_CA (0x00020000) --  SBAS L1_CA RF band  */
}qmiLocEventDbtPositionReportIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCDBTSESSIONSTATUSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_DBT_UNABLE_TO_TRACK_V02 = 1, /**<  Distance based tracking is unavailable and DBT fixes
       cannot currently be obtained \n  */
  eQMI_LOC_DBT_ABLE_TO_TRACK_V02 = 2, /**<  Distance based tracking is available and DBT fixes
       can currently be obtained  */
  QMILOCDBTSESSIONSTATUSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocDbtSessionStatusEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Notifies the control point of the
                    DBT session status. */
typedef struct {

  /* Mandatory */
  /*  DBT Session Status */
  qmiLocDbtSessionStatusEnumT_v02 dbtSessionStatus;
  /**<   Specifies the DBT session status type.
 Valid values: \n
      - eQMI_LOC_DBT_UNABLE_TO_TRACK (1) --  Distance based tracking is unavailable and DBT fixes
       cannot currently be obtained \n 
      - eQMI_LOC_DBT_ABLE_TO_TRACK (2) --  Distance based tracking is available and DBT fixes
       can currently be obtained 
 */

  /* Optional */
  /*  Request ID */
  uint8_t reqId_valid;  /**< Must be set to true if reqId is being passed */
  uint8_t reqId;
  /**<   ID of the DBT request for which this
       status was generated. */
}qmiLocEventDbtSessionStatusIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCPOSITIONSOURCEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_POS_SRC_GNSS_V02 = 1, /**<  Source of the position is GNSS  */
  QMILOCPOSITIONSOURCEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocPositionSourceEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCPOSITIONRPTPROPAGATIONENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_POS_REPORT_PROPAGATED_TO_CURRENT_UTC_V02 = 1, /**<  Reported position is propagated to the current UTC  */
  QMILOCPOSITIONRPTPROPAGATIONENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocPositionRptPropagationEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCSECURELOCDATAMODEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SECURE_LOC_DATA_ENCRYPTED_V02 = 1, /**<  Encrypt data in the indication \n  */
  eQMI_LOC_SECURE_LOC_DATA_UNENCRYPTED_V02 = 2, /**<  Unencrypt data in the indication  */
  QMILOCSECURELOCDATAMODEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSecureLocDataModeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCSECUREGETAVAILABLEPOSPARAMETERIDENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SECURE_GET_AVAILABLE_POS_PARAM_TRANSACTION_ID_V02 = 1, /**<  Parameter ID for the Transaction ID field; mandatory field.
       Identifies the transaction. The transaction ID is returned in the 
       QMI_LOC_GET_AVAILABLE_WWAN_POSITION_IND indication. \n
       - Parameter type -- uint32 
   */
  eQMI_LOC_SECURE_GET_AVAILABLE_POS_PARAM_NONCE_V02 = 2, /**<  Parameter ID for the Nonce field. Optional field. 
       - Parameter type -- uint64 
   */
  eQMI_LOC_SECURE_GET_AVAILABLE_POS_PARAM_SOURCE_V02 = 3, /**<  Parameter ID for the position source field. Optional field. 
       Specifies the source of the position in which the control point is interested.
       If not included, the value defaults to GNSS.        \n
       - Parameter type -- int32 \n 
       Parameter valid values: \n
       - eQMI_LOC_POS_SRC_GNSS (1) --  Source of the position is GNSS
   */
  eQMI_LOC_SECURE_GET_AVAILABLE_POS_PARAM_REPORT_DATA_SECURITY_MODE_V02 = 4, /**<  Secured position report data security mode. Optional field.
       If this TLV is not sent, the position report is encrypted by default. \n
       - Parameter type -- int32 \n
       Parameter valid values: \n
       - eQMI_LOC_SECURE_LOC_DATA_ENCRYPTED (1)   --  Position reports are encrypted \n
       - eQMI_LOC_SECURE_LOC_DATA_UNENCRYPTED (2) --  Position reports are not encrypted 
    */
  eQMI_LOC_SECURE_GET_AVAILABLE_POS_PARAM_REPORT_DATA_TIME_PROPAGATION_V02 = 5, /**<  Secured position report data propagation. Optional field.
        If this TLV is not sent, the position report is propagated to the current UTC time by default. \n
        - Parameter type -- int32 \n
        Parameter valid values: \n
        - eQMI_LOC_POS_REPORT_PROPAGATED_TO_CURRENT_UTC (1) -- Position reports are propagated to the current UTC. \n
    */
  QMILOCSECUREGETAVAILABLEPOSPARAMETERIDENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSecureGetAvailablePosParameterIDEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCSECUREMESSAGEDATATYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SECURE_DATA_TYPE_BYTE_8_V02 = 1, /**<  Data type: Byte (8 bits)  \n */
  eQMI_LOC_SECURE_DATA_TYPE_CHAR_8_V02 = 2, /**<  Data type: Char (8 bits) \n  */
  eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_CHAR_8_V02 = 3, /**<  Data type: Unsigned char (8 bits) \n  */
  eQMI_LOC_SECURE_DATA_TYPE_INT_8_V02 = 4, /**<  Data type: Int (8 bits) \n  */
  eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_8_V02 = 5, /**<  Data type: Unsigned int (8 bits)  \n */
  eQMI_LOC_SECURE_DATA_TYPE_INT_16_V02 = 6, /**<  Data type: Int (16 bits)  \n */
  eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_16_V02 = 7, /**<  Data type: Unsigned int (16 bits) \n  */
  eQMI_LOC_SECURE_DATA_TYPE_INT_32_V02 = 8, /**<  Data type: Int (32 bits)  \n */
  eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_32_V02 = 9, /**<  Data type: Unsigned int (32 bits)  \n */
  eQMI_LOC_SECURE_DATA_TYPE_INT_64_V02 = 10, /**<  Data type: Int (64 bits) \n  */
  eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_64_V02 = 11, /**<  Data type: Unsigned int (64 bits) \n  */
  eQMI_LOC_SECURE_DATA_TYPE_BOOL_8_V02 = 12, /**<  Data type: Boolean (8 bits) \n  */
  eQMI_LOC_SECURE_DATA_TYPE_DOUBLE_64_V02 = 13, /**<  Data type: Double (64 bits) \n  */
  eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02 = 14, /**<  Data type: Float (32 bits)  */
  QMILOCSECUREMESSAGEDATATYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSecureMessageDataTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCBATCHINGSTATUSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_BATCH_POS_UNAVAILABLE_V02 = 1, /**<  Service is unable to compute the positions for batching \n */
  eQMI_LOC_BATCH_POS_AVAILABLE_V02 = 2, /**<  Service is able to compute the positions for batching  */
  QMILOCBATCHINGSTATUSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocBatchingStatusEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Notifies the control point of the
                    batching status. */
typedef struct {

  /* Mandatory */
  /*  Batching Status */
  qmiLocBatchingStatusEnumT_v02 batchingStatus;
  /**<   Batching status.
 Valid values: \n
      - eQMI_LOC_BATCH_POS_UNAVAILABLE (1) --  Service is unable to compute the positions for batching \n
      - eQMI_LOC_BATCH_POS_AVAILABLE (2) --  Service is able to compute the positions for batching 
 */
}qmiLocEventBatchingStatusIndMsgT_v02;  /* Message */
/**
    @}
  */

/**  Identifies the always-on service capabilities.  */
typedef uint32_t qmiLocAonCapabilityMaskT_v02;
#define QMI_LOC_MASK_AON_AUTO_BATCHING_SUPPORTED_V02 ((qmiLocAonCapabilityMaskT_v02)0x00000001) /**<  Service supports auto batching; the client can enable auto
       batching by setting the distance parameter to 0 in the START_BATCHING request \n */
#define QMI_LOC_MASK_AON_DISTANCE_BASED_BATCHING_SUPPORTED_V02 ((qmiLocAonCapabilityMaskT_v02)0x00000002) /**<  Service supports distance-based batching \n */
#define QMI_LOC_MASK_AON_TIME_BASED_BATCHING_SUPPORTED_V02 ((qmiLocAonCapabilityMaskT_v02)0x00000004) /**<  Service supports time-based batching \n */
#define QMI_LOC_MASK_AON_DISTANCE_BASED_TRACKING_SUPPORTED_V02 ((qmiLocAonCapabilityMaskT_v02)0x00000008) /**<  Service supports distance-based tracking \n */
#define QMI_LOC_MASK_AON_UPDATE_TBF_SUPPORTED_V02 ((qmiLocAonCapabilityMaskT_v02)0x00000010) /**<  Service supports changing TBF dynamically\n */
#define QMI_LOC_MASK_AON_OUTDOOR_TRIP_BATCHING_SUPPORTED_V02 ((qmiLocAonCapabilityMaskT_v02)0x00000020) /**<  Service supports outdoor trip batching  */
/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Used by the clients to get always-on (AON) service settings. 
                      */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction; the same transaction ID
       is returned in the QMI_LOC_QUERY_AON_CONFIG_IND indication. */
}qmiLocQueryAonConfigReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Used by the clients to get always-on (AON) service settings. 
                      */
typedef struct {

  /* Mandatory */
  /*  Always-On Config Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_QUERY_AON_CONFIG_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Transaction ID */
  uint8_t transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32_t transactionId;
  /**<   Identifies the transaction. It is the same transaction 
       ID that was passed in the QUERY_AON_CONFIG request. */

  /* Optional */
  /*  Always-On Capability  */
  uint8_t aonCapability_valid;  /**< Must be set to true if aonCapability is being passed */
  qmiLocAonCapabilityMaskT_v02 aonCapability;
  /**<   Always-on capabilities supported by the service. \n
 Valid values: \n 
      - QMI_LOC_MASK_AON_AUTO_BATCHING_SUPPORTED (0x00000001) --  Service supports auto batching; the client can enable auto
       batching by setting the distance parameter to 0 in the START_BATCHING request \n
      - QMI_LOC_MASK_AON_DISTANCE_BASED_BATCHING_SUPPORTED (0x00000002) --  Service supports distance-based batching \n
      - QMI_LOC_MASK_AON_TIME_BASED_BATCHING_SUPPORTED (0x00000004) --  Service supports time-based batching \n
      - QMI_LOC_MASK_AON_DISTANCE_BASED_TRACKING_SUPPORTED (0x00000008) --  Service supports distance-based tracking \n
      - QMI_LOC_MASK_AON_UPDATE_TBF_SUPPORTED (0x00000010) --  Service supports changing TBF dynamically\n
      - QMI_LOC_MASK_AON_OUTDOOR_TRIP_BATCHING_SUPPORTED (0x00000020) --  Service supports outdoor trip batching */
}qmiLocQueryAonConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocDeleteCommonDataMaskT_v02;
#define QMI_LOC_DELETE_COMMON_MASK_POS_V02 ((qmiLocDeleteCommonDataMaskT_v02)0x00000001) /**<  Position estimate; common for all GNSS types \n */
#define QMI_LOC_DELETE_COMMON_MASK_TIME_V02 ((qmiLocDeleteCommonDataMaskT_v02)0x00000002) /**<  Reset all CLOCK_INFO mask \n */
#define QMI_LOC_DELETE_COMMON_MASK_UTC_V02 ((qmiLocDeleteCommonDataMaskT_v02)0x00000004) /**<  UTC estimate \n */
#define QMI_LOC_DELETE_COMMON_MASK_RTI_V02 ((qmiLocDeleteCommonDataMaskT_v02)0x00000008) /**<  RTI \n */
#define QMI_LOC_DELETE_COMMON_MASK_FREQ_BIAS_EST_V02 ((qmiLocDeleteCommonDataMaskT_v02)0x00000010) /**<  Frequency bias estimate; common for all GNSS types  */
typedef uint32_t qmiLocDeleteSatelliteDataMaskT_v02;
#define QMI_LOC_DELETE_DATA_MASK_EPHEMERIS_V02 ((qmiLocDeleteSatelliteDataMaskT_v02)0x00000001) /**<  Ephemeris \n */
#define QMI_LOC_DELETE_DATA_MASK_ALMANAC_V02 ((qmiLocDeleteSatelliteDataMaskT_v02)0x00000002) /**<  Almanac \n */
#define QMI_LOC_DELETE_DATA_MASK_SVHEALTH_V02 ((qmiLocDeleteSatelliteDataMaskT_v02)0x00000004) /**<  SV health \n */
#define QMI_LOC_DELETE_DATA_MASK_SVDIR_V02 ((qmiLocDeleteSatelliteDataMaskT_v02)0x00000008) /**<  SV direction \n */
#define QMI_LOC_DELETE_DATA_MASK_SVSTEER_V02 ((qmiLocDeleteSatelliteDataMaskT_v02)0x00000010) /**<  SV steer \n */
#define QMI_LOC_DELETE_DATA_MASK_ALM_CORR_V02 ((qmiLocDeleteSatelliteDataMaskT_v02)0x00000020) /**<  Almanac correction \n */
#define QMI_LOC_DELETE_DATA_MASK_BLACKLIST_V02 ((qmiLocDeleteSatelliteDataMaskT_v02)0x00000040) /**<  Blacklist SVs \n */
#define QMI_LOC_DELETE_DATA_MASK_SA_DATA_V02 ((qmiLocDeleteSatelliteDataMaskT_v02)0x00000080) /**<  Sensitivity assistance data \n */
#define QMI_LOC_DELETE_DATA_MASK_SV_NO_EXIST_V02 ((qmiLocDeleteSatelliteDataMaskT_v02)0x00000100) /**<  SV does not exist \n */
#define QMI_LOC_DELETE_DATA_MASK_IONO_V02 ((qmiLocDeleteSatelliteDataMaskT_v02)0x00000200) /**<  Ionosphere correction \n */
#define QMI_LOC_DELETE_DATA_MASK_TIME_V02 ((qmiLocDeleteSatelliteDataMaskT_v02)0x00000400) /**<  Reset satellite time \n */
#define QMI_LOC_DELETE_DATA_MASK_MB_DATA_V02 ((qmiLocDeleteSatelliteDataMaskT_v02)0x00000800) /**<  Delete Multiband data \n */
#define QMI_LOC_DELETE_DATA_MASK_TGD_DATA_V02 ((qmiLocDeleteSatelliteDataMaskT_v02)0x00001000) /**<  Reset TGD (group delay) data  */
typedef uint32_t qmiLocGNSSConstellMaskT_v02;
#define QMI_LOC_SYSTEM_GPS_V02 ((qmiLocGNSSConstellMaskT_v02)0x00000001) /**<  System GPS data \n */
#define QMI_LOC_SYSTEM_GLO_V02 ((qmiLocGNSSConstellMaskT_v02)0x00000002) /**<  System GLONASS data \n */
#define QMI_LOC_SYSTEM_BDS_V02 ((qmiLocGNSSConstellMaskT_v02)0x00000004) /**<  System BDS data \n */
#define QMI_LOC_SYSTEM_GAL_V02 ((qmiLocGNSSConstellMaskT_v02)0x00000008) /**<  System Galileo data \n */
#define QMI_LOC_SYSTEM_QZSS_V02 ((qmiLocGNSSConstellMaskT_v02)0x00000010) /**<  System QZSS data  */
/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocGNSSConstellMaskT_v02 system;
  /**<   Indicates which satellite system's data to delete.
 The control point can delete multiple systems at a time.
 Valid values: \n
      - QMI_LOC_SYSTEM_GPS (0x00000001) --  System GPS data \n
      - QMI_LOC_SYSTEM_GLO (0x00000002) --  System GLONASS data \n
      - QMI_LOC_SYSTEM_BDS (0x00000004) --  System BDS data \n
      - QMI_LOC_SYSTEM_GAL (0x00000008) --  System Galileo data \n
      - QMI_LOC_SYSTEM_QZSS (0x00000010) --  System QZSS data 
 */

  qmiLocDeleteSatelliteDataMaskT_v02 deleteSatelliteDataMask;
  /**<   Requested bitmask of data to delete for the specified satellite system. \n
 Valid values: \n
      - QMI_LOC_DELETE_DATA_MASK_EPHEMERIS (0x00000001) --  Ephemeris \n
      - QMI_LOC_DELETE_DATA_MASK_ALMANAC (0x00000002) --  Almanac \n
      - QMI_LOC_DELETE_DATA_MASK_SVHEALTH (0x00000004) --  SV health \n
      - QMI_LOC_DELETE_DATA_MASK_SVDIR (0x00000008) --  SV direction \n
      - QMI_LOC_DELETE_DATA_MASK_SVSTEER (0x00000010) --  SV steer \n
      - QMI_LOC_DELETE_DATA_MASK_ALM_CORR (0x00000020) --  Almanac correction \n
      - QMI_LOC_DELETE_DATA_MASK_BLACKLIST (0x00000040) --  Blacklist SVs \n
      - QMI_LOC_DELETE_DATA_MASK_SA_DATA (0x00000080) --  Sensitivity assistance data \n
      - QMI_LOC_DELETE_DATA_MASK_SV_NO_EXIST (0x00000100) --  SV does not exist \n
      - QMI_LOC_DELETE_DATA_MASK_IONO (0x00000200) --  Ionosphere correction \n
      - QMI_LOC_DELETE_DATA_MASK_TIME (0x00000400) --  Reset satellite time \n
      - QMI_LOC_DELETE_DATA_MASK_MB_DATA (0x00000800) --  Delete Multiband data \n
      - QMI_LOC_DELETE_DATA_MASK_TGD_DATA (0x00001000) --  Reset TGD (group delay) data 
 */
}qmiLocDeleteSatelliteDataStructT_v02;  /* Type */
/**
    @}
  */

typedef uint32_t qmiLocDeleteCelldbDataMaskT_v02;
#define QMI_LOC_MASK_DELETE_CELLDB_POS_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000001) /**<  Mask to delete cell database position \n */
#define QMI_LOC_MASK_DELETE_CELLDB_LATEST_GPS_POS_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000002) /**<  Mask to delete cell database latest GPS position \n */
#define QMI_LOC_MASK_DELETE_CELLDB_OTA_POS_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000004) /**<  Mask to delete cell database OTA position \n */
#define QMI_LOC_MASK_DELETE_CELLDB_EXT_REF_POS_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000008) /**<  Mask to delete cell database external reference position \n */
#define QMI_LOC_MASK_DELETE_CELLDB_TIMETAG_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000010) /**<  Mask to delete cell database time tag \n  */
#define QMI_LOC_MASK_DELETE_CELLDB_CELLID_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000020) /**<  Mask to delete cell database cell ID \n */
#define QMI_LOC_MASK_DELETE_CELLDB_CACHED_CELLID_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000040) /**<  Mask to delete cell database cached cell ID \n */
#define QMI_LOC_MASK_DELETE_CELLDB_LAST_SRV_CELL_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000080) /**<  Mask to delete cell database last service cell \n */
#define QMI_LOC_MASK_DELETE_CELLDB_CUR_SRV_CELL_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000100) /**<  Mask to delete cell database current service cell \n */
#define QMI_LOC_MASK_DELETE_CELLDB_NEIGHBOR_INFO_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000200) /**<  Mask to delete cell database neighbor information  */
typedef uint32_t qmiLocDeleteClockInfoMaskT_v02;
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_TIME_EST_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000001) /**<  Mask to delete time estimate from clock information \n  */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_FREQ_EST_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000002) /**<  Mask to delete frequency estimate from clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_WEEK_NUMBER_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000004) /**<  Mask to delete week number from clock information \n  */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_RTC_TIME_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000008) /**<  Mask to delete RTC time from clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_TIME_TRANSFER_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000010) /**<  Mask to delete time transfer from clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GPSTIME_EST_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000020) /**<  Mask to delete GPS time estimate from clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GLOTIME_EST_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000040) /**<  Mask to delete GLONASS time estimate from clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GLODAY_NUMBER_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000080) /**<  Mask to delete GLONASS day number from clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GLO4YEAR_NUMBER_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000100) /**<  Mask to delete GLONASS four year number from clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000200) /**<  Mask to delete GLONASS RF GRP delay from clock information \n  */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_DISABLE_TT_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000400) /**<  Mask to delete disable TT from clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GG_LEAPSEC_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000800) /**<  Mask to delete a BDS time estimate from the clock information \n  */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GG_GGTB_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00001000) /**<  Mask to delete a BDS time estimate from the clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_BDSTIME_EST_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00002000) /**<  Mask to delete a BDS time estimate from the clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GB_GBTB_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00004000) /**<  Mask to delete Glonass-to-BDS time bias-related information from the
      clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_BG_BGTB_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00008000) /**<  Mask to delete BDS-to-GLONASS time bias-related information from the
       clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_BDSWEEK_NUMBER_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00010000) /**<  Mask to delete the BDS week number from the clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_BDS_RF_GRP_DELAY_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00020000) /**<  Mask to delete the BDS RF GRP delay from the clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTIME_EST_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00040000) /**<  Mask to delete a Galileo time estimate from the clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTOGPS_TB_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00080000) /**<  Mask to delete Galileo-to-GPS time bias-related information from the
      clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTOGLO_TB_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00100000) /**<  Mask to delete Galileo-to-GLONASS time bias-related information from the
      clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTOBDS_TB_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00200000) /**<  Mask to delete Galileo-to-BDS time bias-related information from the
      clock information \n  */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GALWEEK_NUMBER_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00400000) /**<  Mask to delete the Galileo week number from the clock information \n */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GAL_RF_GRP_DELAY_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00800000) /**<  Mask to delete the Galileo RF GRP delay from the clock information  */
/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Deletes the location engine
                    service data from memory. */
typedef struct {

  /* Mandatory */
  /*  Reset All */
  uint8_t deleteAllFlag;
  /**<   Indicates whether to delete all GNSS service data.
       Values: \n
       - 0x01 (TRUE)  -- All constellations' service data is reset;
                       if this flag is set, all the other information
                       contained in the optional fields for this
                       message are ignored \n
       - 0x00 (FALSE) -- Optional fields in the message are 
                       used to determine which data to delete.
  */

  /* Optional */
  /*  Requested Bitmask of Clock Info Data to Delete */
  uint8_t deleteClockInfoMask_valid;  /**< Must be set to true if deleteClockInfoMask is being passed */
  qmiLocDeleteClockInfoMaskT_v02 deleteClockInfoMask;
  /**<   Mask for the clock information service data to delete.
 If QMI_LOC_DELETE_DATA_MASK_TIME is set in deleteServiceDataMask,
 deleteClockInfoMask is ignored.
 Valid values: \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_TIME_EST (0x00000001) --  Mask to delete time estimate from clock information \n 
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_FREQ_EST (0x00000002) --  Mask to delete frequency estimate from clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_WEEK_NUMBER (0x00000004) --  Mask to delete week number from clock information \n 
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_RTC_TIME (0x00000008) --  Mask to delete RTC time from clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_TIME_TRANSFER (0x00000010) --  Mask to delete time transfer from clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_GPSTIME_EST (0x00000020) --  Mask to delete GPS time estimate from clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_GLOTIME_EST (0x00000040) --  Mask to delete GLONASS time estimate from clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_GLODAY_NUMBER (0x00000080) --  Mask to delete GLONASS day number from clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_GLO4YEAR_NUMBER (0x00000100) --  Mask to delete GLONASS four year number from clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY (0x00000200) --  Mask to delete GLONASS RF GRP delay from clock information \n 
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_DISABLE_TT (0x00000400) --  Mask to delete disable TT from clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_GG_LEAPSEC (0x00000800) --  Mask to delete a BDS time estimate from the clock information \n 
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_GG_GGTB (0x00001000) --  Mask to delete a BDS time estimate from the clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_BDSTIME_EST (0x00002000) --  Mask to delete a BDS time estimate from the clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_GB_GBTB (0x00004000) --  Mask to delete Glonass-to-BDS time bias-related information from the
      clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_BG_BGTB (0x00008000) --  Mask to delete BDS-to-GLONASS time bias-related information from the
       clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_BDSWEEK_NUMBER (0x00010000) --  Mask to delete the BDS week number from the clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_BDS_RF_GRP_DELAY (0x00020000) --  Mask to delete the BDS RF GRP delay from the clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTIME_EST (0x00040000) --  Mask to delete a Galileo time estimate from the clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTOGPS_TB (0x00080000) --  Mask to delete Galileo-to-GPS time bias-related information from the
      clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTOGLO_TB (0x00100000) --  Mask to delete Galileo-to-GLONASS time bias-related information from the
      clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTOBDS_TB (0x00200000) --  Mask to delete Galileo-to-BDS time bias-related information from the
      clock information \n 
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_GALWEEK_NUMBER (0x00400000) --  Mask to delete the Galileo week number from the clock information \n
      - QMI_LOC_MASK_DELETE_CLOCK_INFO_GAL_RF_GRP_DELAY (0x00800000) --  Mask to delete the Galileo RF GRP delay from the clock information 
 */

  /* Optional */
  /*  Requested Bitmask of Cell DB Data to Delete */
  uint8_t deleteCellDbDataMask_valid;  /**< Must be set to true if deleteCellDbDataMask is being passed */
  qmiLocDeleteCelldbDataMaskT_v02 deleteCellDbDataMask;
  /**<   Mask for the cell database service data to delete;
 common for all GNSS types. 
 Valid values: \n
      - QMI_LOC_MASK_DELETE_CELLDB_POS (0x00000001) --  Mask to delete cell database position \n
      - QMI_LOC_MASK_DELETE_CELLDB_LATEST_GPS_POS (0x00000002) --  Mask to delete cell database latest GPS position \n
      - QMI_LOC_MASK_DELETE_CELLDB_OTA_POS (0x00000004) --  Mask to delete cell database OTA position \n
      - QMI_LOC_MASK_DELETE_CELLDB_EXT_REF_POS (0x00000008) --  Mask to delete cell database external reference position \n
      - QMI_LOC_MASK_DELETE_CELLDB_TIMETAG (0x00000010) --  Mask to delete cell database time tag \n 
      - QMI_LOC_MASK_DELETE_CELLDB_CELLID (0x00000020) --  Mask to delete cell database cell ID \n
      - QMI_LOC_MASK_DELETE_CELLDB_CACHED_CELLID (0x00000040) --  Mask to delete cell database cached cell ID \n
      - QMI_LOC_MASK_DELETE_CELLDB_LAST_SRV_CELL (0x00000080) --  Mask to delete cell database last service cell \n
      - QMI_LOC_MASK_DELETE_CELLDB_CUR_SRV_CELL (0x00000100) --  Mask to delete cell database current service cell \n
      - QMI_LOC_MASK_DELETE_CELLDB_NEIGHBOR_INFO (0x00000200) --  Mask to delete cell database neighbor information 
 */

  /* Optional */
  /*  Requested Bitmask of Common Data to Delete */
  uint8_t deleteCommonDataMask_valid;  /**< Must be set to true if deleteCommonDataMask is being passed */
  qmiLocDeleteCommonDataMaskT_v02 deleteCommonDataMask;
  /**<   Mask for the common service data to delete.
 Valid values: \n
      - QMI_LOC_DELETE_COMMON_MASK_POS (0x00000001) --  Position estimate; common for all GNSS types \n
      - QMI_LOC_DELETE_COMMON_MASK_TIME (0x00000002) --  Reset all CLOCK_INFO mask \n
      - QMI_LOC_DELETE_COMMON_MASK_UTC (0x00000004) --  UTC estimate \n
      - QMI_LOC_DELETE_COMMON_MASK_RTI (0x00000008) --  RTI \n
      - QMI_LOC_DELETE_COMMON_MASK_FREQ_BIAS_EST (0x00000010) --  Frequency bias estimate; common for all GNSS types 
 */

  /* Optional */
  /*  GNSS Service Data to Delete */
  uint8_t deleteSatelliteData_valid;  /**< Must be set to true if deleteSatelliteData is being passed */
  qmiLocDeleteSatelliteDataStructT_v02 deleteSatelliteData;
  /**<   \n Request to delete the GNSS service data.*/
}qmiLocDeleteGNSSServiceDataReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Deletes the location engine
                    service data from memory. */
typedef struct {

  /* Mandatory */
  /*  Delete GNSS Service Data Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_DELETE_GNSS_SERVICE_DATA_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */
}qmiLocDeleteGNSSServiceDataIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCXTRADATAFORMATENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_XTRA_DATA_V02 = 0, /**<  Default is QCOM-XTRA format.  */
  QMILOCXTRADATAFORMATENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocXtraDataFormatEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Injects XTRA data. */
typedef struct {

  /* Mandatory */
  /*  Total Size */
  uint32_t totalSize;
  /**<   Total size of the XTRA data to inject. \n
        - Units -- Bytes */

  /* Mandatory */
  /*  Total Parts */
  uint16_t totalParts;
  /**<   Total number of parts into which the XTRA data is divided. */

  /* Mandatory */
  /*  Part Number */
  uint16_t partNum;
  /**<   Number of the current XTRA data part; starts at 1. */

  /* Mandatory */
  /*  Data */
  uint32_t partData_len;  /**< Must be set to # of elements in partData */
  uint8_t partData[QMI_LOC_MAX_XTRA_PART_LEN_V02];
  /**<   XTRA data. \n
         - Type -- Array of bytes \n
         - Maximum length of the array -- 1024
    */

  /* Optional */
  /*  Format Type */
  uint8_t formatType_valid;  /**< Must be set to true if formatType is being passed */
  qmiLocXtraDataFormatEnumT_v02 formatType;
  /**<   XTRA data format. \n
 Valid values: \n
      - eQMI_LOC_XTRA_DATA (0) --  Default is QCOM-XTRA format. 
 */
}qmiLocInjectXtraDataReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Injects XTRA data. */
typedef struct {

  /* Mandatory */
  /*  Data Injection Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_INJECT_XTRA_DATA_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Part Number */
  uint8_t partNum_valid;  /**< Must be set to true if partNum is being passed */
  uint16_t partNum;
  /**<   Number of the XTRA data part for which this indication
      is sent; starts at 1. */

  /* Optional */
  /*  Success Constellation Mask */
  uint8_t ConstellationMask_valid;  /**< Must be set to true if ConstellationMask is being passed */
  qmiLocGNSSConstellEnumT_v02 ConstellationMask;
  /**<   Constellation mask is set when the XTRA data is accepted by the modem GNSS engine.
 This optional TLV is only available when at least one constellation injection is successful,
 and it is only available in the last QMI_LOC_INJECT_XTRA_DATA_IND indication.\n
      - eQMI_SYSTEM_GPS (0x01) --  Enable GPS \n 
      - eQMI_SYSTEM_GLO (0x02) --  Enable GLONASS \n 
      - eQMI_SYSTEM_BDS (0x04) --  Enable BDS \n 
      - eQMI_SYSTEM_GAL (0x08) --  Enable Galileo \n 
      - eQMI_SYSTEM_QZSS (0x10) --  Enable QZSS  */
}qmiLocInjectXtraDataIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Injects PCID, which is used by XTRA service. */
typedef struct {

  /* Mandatory */
  /*  XTRA PCID */
  uint64_t xtraPcid;
  /**<   XTRA PCID data.  */
}qmiLocInjectXtraPcidReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Injects PCID, which is used by XTRA service. */
typedef struct {

  /* Mandatory */
  /*  Inject XTRA PCID Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the inject XTRA PCID command.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made  */
}qmiLocInjectXtraPcidIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Queries the QMI_LOC service supported features. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmiLocGetSupportedFeatureReqMsgT_v02;

  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCSUPPORTEDFEATUREENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SUPPORTED_FEATURE_ODCPI_2_V02 = 0, /**<  Support the ODCPI version 2 feature \n  */
  eQMI_LOC_SUPPORTED_FEATURE_WIFI_AP_DATA_INJECT_2_V02 = 1, /**<  Support the Wi-Fi AP data inject version 2 feature \n  */
  eQMI_LOC_SUPPORTED_FEATURE_DEBUG_NMEA_V02 = 2, /**<  Support the debug NMEA feature \n  */
  eQMI_LOC_SUPPORTED_FEATURE_GNSS_ONLY_POSITION_REPORT_V02 = 3, /**<  Support the GNSS only position report feature  \n */
  eQMI_LOC_SUPPORTED_FEATURE_FDCL_V02 = 4, /**<  Support the FDCL feature \n  */
  eQMI_LOC_SUPPORTED_FEATURE_CONSTELLATION_ENABLEMENT_V02 = 5, /**<  Support the GNSS constellation enablement feature \n  */
  eQMI_LOC_SUPPORTED_FEATURE_AGPM_V02 = 6, /**<  Support the advanced GNSS power management feature \n  */
  eQMI_LOC_SUPPORTED_FEATURE_XTRA_INTEGRITY_V02 = 7, /**<  Support the XTRA integrity feature \n  */
  eQMI_LOC_SUPPORTED_FEATURE_FDCL_2_V02 = 8, /**<  Support the FDCL version 2 feature  */
  eQMI_LOC_SUPPORTED_FEATURE_ROBUST_LOCATION_V02 = 9, /**<  Support Robust Location feature  */
  QMILOCSUPPORTEDFEATUREENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSupportedFeatureEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Queries the QMI_LOC service supported features. */
typedef struct {

  /* Mandatory */
  /*  Supported Feature Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_GET_SUPPORTED_FEATURE_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Mandatory */
  /*  Supported Features */
  uint32_t feature_len;  /**< Must be set to # of elements in feature */
  uint8_t feature[QMI_LOC_SUPPORTED_FEATURE_LENGTH_V02];
  /**<   Indicates which features are supported in the running
       QMI_LOC service. The array of unit8 is the bitmask where each bit
       represents a feature enum. Bit 0 represents feature enum ID 0,
       bit 1 represents feature enum ID 1, and so on.
       For example, if QMI_LOC supports feature enum 0,1,2,8,
       feature_len is 2, and feature array is [7,1]. \n
       - Type -- Array of uint8 \n
       - Maximum array length -- 100
  */
}qmiLocGetSupportedFeatureIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocXtraConfigMaskT_v02;
#define QMI_LOC_XTRA_CONFIG_DISABLE_AUTO_DOWNLOAD_TIMER_V02 ((qmiLocXtraConfigMaskT_v02)0x00000001) /**<  Request the engine to disable the XTRA auto download timer  */
typedef uint32_t qmiLocXtraInfoMaskT_v02;
#define QMI_LOC_XTRA_INFO_MASK_ABS_AGE_V02 ((qmiLocXtraInfoMaskT_v02)0x00000001) /**<  Number of hours for which the current XTRA information is valid \n */
#define QMI_LOC_XTRA_INFO_MASK_REL_AGE_V02 ((qmiLocXtraInfoMaskT_v02)0x00000002) /**<  Last XTRA data download time \n  */
#define QMI_LOC_XTRA_INFO_MASK_XTRA_SERVER_V02 ((qmiLocXtraInfoMaskT_v02)0x00000004) /**<  XTRA server URLs \n  */
#define QMI_LOC_XTRA_INFO_MASK_NTP_SERVER_V02 ((qmiLocXtraInfoMaskT_v02)0x00000008) /**<  Network Time Protocol (NTP) server URLs \n  */
#define QMI_LOC_XTRA_INFO_MASK_TIME_REQUEST_V02 ((qmiLocXtraInfoMaskT_v02)0x00000010) /**<  Requests the control point to send QMI_LOC_INJECT_UTC_TIME_REQ
       to the engine \n  */
#define QMI_LOC_XTRA_INFO_MASK_PREF_VALID_AGE_V02 ((qmiLocXtraInfoMaskT_v02)0x00000020) /**<  Preferred valid age  */
/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocXtraInfoMaskT_v02 reportMask;
  /**<   Bitmask indicating which of the fields in this TLV are reported: \n
      - QMI_LOC_XTRA_INFO_MASK_ABS_AGE (0x00000001) --  Number of hours for which the current XTRA information is valid \n
      - QMI_LOC_XTRA_INFO_MASK_REL_AGE (0x00000002) --  Last XTRA data download time \n 
      - QMI_LOC_XTRA_INFO_MASK_XTRA_SERVER (0x00000004) --  XTRA server URLs \n 
      - QMI_LOC_XTRA_INFO_MASK_NTP_SERVER (0x00000008) --  Network Time Protocol (NTP) server URLs \n 
      - QMI_LOC_XTRA_INFO_MASK_TIME_REQUEST (0x00000010) --  Requests the control point to send QMI_LOC_INJECT_UTC_TIME_REQ
       to the engine \n 
      - QMI_LOC_XTRA_INFO_MASK_PREF_VALID_AGE (0x00000020) --  Preferred valid age  */

  uint16_t absAgeHrs;
  /**<   Current age of XTRA file in hours. \n
       - Units -- Hours */

  uint64_t relAgeInUTC;
  /**<   Last XTRA data download time in UTC. \n
       - Units -- Milliseconds */

  qmiLocPredictedOrbitsServerListStructT_v02 xtraServerInfo;
  /**<   Contains information about the XTRA servers that the control point can use
       to download XTRA data. */

  qmiLocTimeServerListStructT_v02 ntpServerInfo;
  /**<   Contains information about the time servers recommended by the
       location service for the UTC time. */

  uint8_t timeRequest;
  /**<    Requests the control point to send the QMI_LOC_INJECT_UTC_TIME_REQ
        request to the engine. \n
       - 0x00 (FALSE) -- Engine has the UTC time \n
       - 0x01 (TRUE) -- Requests the control point to inject the UTC time to the engine */

  uint16_t preferedValidAgeHrs;
  /**<   Preferred valid age. \n
       - Units -- Hours */
}qmiLocXtraInfoStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Queries the XTRA information. */
typedef struct {

  /* Mandatory */
  /*  XTRA Configure */
  qmiLocXtraConfigMaskT_v02 xtraConfig;
  /**<   XTRA configuration.
 Valid values: \n
      - QMI_LOC_XTRA_CONFIG_DISABLE_AUTO_DOWNLOAD_TIMER (0x00000001) --  Request the engine to disable the XTRA auto download timer 
 */
}qmiLocQueryXtraInfoReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Queries the XTRA information. */
typedef struct {

  /* Mandatory */
  /*  Query XTRA Information Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of QMI_LOC_QUERY_XTRA_INFO.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Mandatory */
  /*  XTRA Information */
  qmiLocXtraInfoStructT_v02 xtraInfo;
  /**<   \n XTRA information returned from the engine. */

  /* Optional */
  /*  XTRA File Generation Time */
  uint8_t xtraFileGenerationTime_valid;  /**< Must be set to true if xtraFileGenerationTime is being passed */
  uint64_t xtraFileGenerationTime;
  /**<   Indicates XTRA file generation time on the server in UTC.
      This value will be decoded after a successful XTRA Download \n
      - Units -- Milliseconds (since Jan. 1, 1970) */

  /* Optional */
  /*  XTRA File Valid Duration */
  uint8_t xtraFileDurationHrs_valid;  /**< Must be set to true if xtraFileDurationHrs is being passed */
  uint32_t xtraFileDurationHrs;
  /**<   Indicates XTRA file total valid duration since start time till its expiry. \n
       - Units -- Hours */
}qmiLocQueryXtraInfoIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Initiates an outdoor trip batching session. */
typedef struct {

  /* Mandatory */
  /*  Batch Distance */
  uint32_t batchDistance;
  /**<   Trip distance from the start of outdoor trip batching,
       which triggers the QMI_LOC_EVENT_BATCH_FULL_NOTIFICATION_IND indication
       to report to the control point. \n
       - Units -- Meters
  */

  /* Mandatory */
  /*  Minimum Interval Between Position Reports */
  uint32_t minTimeInterval;
  /**<   Minimum time interval, specified by the control point, that must elapse between
       position reports. \n
       - Units -- Milliseconds
  */

  /* Optional */
  /*  Fix Session Timeout Period */
  uint8_t fixSessionTimeout_valid;  /**< Must be set to true if fixSessionTimeout is being passed */
  uint32_t fixSessionTimeout;
  /**<   Configures the fix session timeout duration. \n
       - Units -- Milliseconds \n
       - Default -- 20,000 ms
  */

  /* Optional */
  /*  Batch All Positions */
  uint8_t batchAllPos_valid;  /**< Must be set to true if batchAllPos is being passed */
  uint8_t batchAllPos;
  /**<   Values: \n
       - TRUE -- All available positions must be batched. For example,
              if any other type of positioning is active (such as 1 Hz tracking), all 
              positions computed for that use case are also batched. This might
              result in generating the BATCH_FULL indication earlier. \n
       - FALSE -- Only positions that meet the time and/or distance criteria are batched
                  (default).
  */
}qmiLocStartOutdoorTripBatchingReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Initiates an outdoor trip batching session. */
typedef struct {

  /* Mandatory */
  /*  Start Distance-Based Batching Session Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_START_OUTDOOR_TRIP_BATCHING_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */
}qmiLocStartOutdoorTripBatchingIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Queries the accumulated distance
                    of an outdoor trip batching session. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmiLocQueryOTBAccumulatedDistanceReqMsgT_v02;

  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Queries the accumulated distance
                    of an outdoor trip batching session. */
typedef struct {

  /* Mandatory */
  /*  Query Outdoor Trip Batching Session Accumulated Distance Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_QUERY_OTB_ACCUMULATED_DISTANCE_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Mandatory */
  /*  Accumulated OTB Distance */
  uint32_t accumulatedDistance;
  /**<   Accumulated distance from the last QMI_LOC_START_OUTDOOR_TRIP_BATCHING_REQ request. \n
       - Units -- Meters
  */

  /* Mandatory */
  /*  Number of Batched Position Reports */
  uint32_t batchedPosition;
  /**<   Number of the position reports that have been batched from the last 
       QMI_LOC_START_OUTDOOR_TRIP_BATCHING_REQ request.
  */
}qmiLocQueryOTBAccumulatedDistanceIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t gtpClientSwMajorVersion;
  /**<   Major version number from 0 to 255. */

  uint8_t gtpClientSwMinorVersion;
  /**<   Minor version number from 0 to 255. */
}qmiLocGtpClientSwVerStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t homeMncValid;
  /**<   Boolean indicating whether homeMnc is valid. */

  uint32_t homeMcc;
  /**<   MCC of home carrier. */

  uint32_t homeMnc;
  /**<   MNC of home carrier. */
}qmiLocHomeCarrierIdStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t unspecifiedInternal;
  /**<   Unspecified internal contribution. \n
       - Range -- 0 to 100 percent */

  uint8_t unspecifiedExternal;
  /**<   Unspecified external contribution. \n
       - Range -- 0 to 100 percent */

  uint8_t gnss;
  /**<   GNSS contribution. \n
       - Range -- 0 to 100 percent */

  uint8_t sensors;
  /**<   Sensors contribution. \n
       - Range -- 0 to 100 percent */

  uint8_t wifi;
  /**<   Wi-Fi contribution. \n
       - Range --  0 to 100 percent */

  uint8_t wwan;
  /**<   WWAN contribution. \n
       - Range -- 0 to 100 percent */
}qmiLocPositionSourceStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  double latitude;
  /**<   Latitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -90.0 to 90.0 (positive values indicate northern latitude,
         negative values indicate southern latitude)
     */

  double longitude;
  /**<   Longitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -180.0 to 180.0 (positive values indicate eastern longitude,
         negative values indicate western longitude)
    */

  float horizontalUnc;
  /**<   Horizontal circular uncertainty. \n
       - Units -- Meters */

  float altitude;
  /**<   Altitude with respect to mean sea level. \n
       - Units -- Meters */

  float altitudeUnc;
  /**<   Vertical uncertainty. \n
       - Units -- Meters */

  qmiLocPositionSourceStructT_v02 latestPositionSources;
  /**<   Percentage contributions from all sources for position fix (sum should be ~100%). */
}qmiLocBsBestPositionStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCFDCLAIRINTERFACETYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_CDMA_V02 = 0, /**<  FDCL CDMA cell  \n */
  eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_GSM_V02 = 1, /**<  FDCL GSM cell \n */
  eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_WCDMA_V02 = 2, /**<  FDCL WCDMA cell \n */
  eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_V02 = 3, /**<  FDCL LTE cell \n */
  eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_M1_V02 = 4, /**<  FDCL LTE-M1 cell \n  */
  eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_NB1_V02 = 5, /**<  FDCL LTE-NB1 cell \n  */
  eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_M1_MODE_A_V02 = 6, /**<  FDCL LTE-M1 ModeA cell \n  */
  eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_M1_MODE_B_V02 = 7, /**<  FDCL LTE-M1 ModeB cell \n  */
  eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_UNKNOWN_V02 = 8, /**<  FDCL LTE-unknown cell  */
  QMILOCFDCLAIRINTERFACETYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocFdclAirInterfaceTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocFdclAirInterfaceTypeEnumT_v02 airInterfaceType;
  /**<   Cell type for this record. */

  uint32_t id1;
  /**<   ID1. \n
       - For all air interface -- MCC
   */

  uint32_t id2;
  /**<   ID2. \n
       - GSM -- WCDMA \n
       - LTE -- MNC
   */

  uint32_t id3;
  /**<   ID3. \n
       - CDMA -- NID \n
       - GSM and WCDMA -- LAC \n
       - LTE -- TAC
   */

  uint32_t id4;
  /**<   ID4. \n
       - CDMA -- BSID \n
       - GSM -- CI \n
       - WCDMA -- U_CI \n
       - LTE -- G_CI
   */
}qmiLocFdclCellIdStructT_v02;  /* Type */
/**
    @}
  */

typedef uint64_t qmiLocFdclCellIdOptionalValidMaskT_v02;
#define QMI_LOC_FDCL_CELL_ID_MASK_VALID_CHANNEL_NUMBER_V02 ((qmiLocFdclCellIdOptionalValidMaskT_v02)0x00000001ull) /**<  Cell channel number is valid. \n  */
#define QMI_LOC_FDCL_CELL_ID_MASK_VALID_PLID_V02 ((qmiLocFdclCellIdOptionalValidMaskT_v02)0x00000002ull) /**<  Cell channel plid field is valid. \n */
#define QMI_LOC_FDCL_CELL_ID_MASK_VALID_NB_CARRIER_FREQ_OFFSET_V02 ((qmiLocFdclCellIdOptionalValidMaskT_v02)0x00000004ull) /**<  Cell narrow band carrier freq offset is valid. \n */
#define QMI_LOC_FDCL_CELL_ID_MASK_VALID_NB_CHANNEL_RASTER_OFFSET_V02 ((qmiLocFdclCellIdOptionalValidMaskT_v02)0x00000008ull) /**<  Cell narrow band channel raster offset is valid.  */
/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocFdclAirInterfaceTypeEnumT_v02 airInterfaceType;
  /**<   Cell type for this record. */

  uint32_t id1;
  /**<   ID1. \n
       - For all air interfaces -- MCC
   */

  uint32_t id2;
  /**<   ID2. \n
       - GSM -- WCDMA \n
       - LTE-- MNC
   */

  uint32_t id3;
  /**<   ID3. \n
       - CDMA -- NID \n
       - GSM and WCDMA -- LAC \n
       - LTE -- TAC
   */

  uint32_t id4;
  /**<   ID4. \n
       - CDMA -- BSID \n
       - GSM -- CI \n
       - WCDMA -- U_CI \n
       - LTE -- G_CI
     */

  qmiLocFdclCellIdOptionalValidMaskT_v02 validMask;
  /**<   Additional parameters. 
 Valid bitmasks: \n
      - QMI_LOC_FDCL_CELL_ID_MASK_VALID_CHANNEL_NUMBER (0x00000001) --  Cell channel number is valid. \n 
      - QMI_LOC_FDCL_CELL_ID_MASK_VALID_PLID (0x00000002) --  Cell channel plid field is valid. \n
      - QMI_LOC_FDCL_CELL_ID_MASK_VALID_NB_CARRIER_FREQ_OFFSET (0x00000004) --  Cell narrow band carrier freq offset is valid. \n
      - QMI_LOC_FDCL_CELL_ID_MASK_VALID_NB_CHANNEL_RASTER_OFFSET (0x00000008) --  Cell narrow band channel raster offset is valid. */

  uint32_t channelNumber;
  /**<   Channel number. */

  uint16_t plid;
  /**<   PLID of cell. */

  uint8_t nbCarrierFreqOff;
  /**<   Carrier frequency offset.*/

  uint8_t nbChannelRasterOff;
  /**<   Channel raster offset.*/
}qmiLocFdclCellIdExtStructT_v02;  /* Type */
/**
    @}
  */

typedef uint64_t qmiLocCapabilitiesMaskT_v02;
#define QMI_LOC_CAP_MASK_GTP_V02 ((qmiLocCapabilitiesMaskT_v02)0x00000001ull) /**<  GTP capability of this device is enabled  \n  */
#define QMI_LOC_CAP_MASK_WIFI_V02 ((qmiLocCapabilitiesMaskT_v02)0x00000002ull) /**<  GNSS capability of this device is enabled \n */
#define QMI_LOC_CAP_MASK_GNSS_V02 ((qmiLocCapabilitiesMaskT_v02)0x00000004ull) /**<  GNSS capability of this device is enabled \n */
#define QMI_LOC_CAP_MASK_GTP_CACHE_V02 ((qmiLocCapabilitiesMaskT_v02)0x00000008ull) /**<  GTP cache fix of this device is enabled  */
typedef uint64_t qmiLTEBandMaskT_v02;
#define QMI_LOC_EVENT_MASK_LTE_BANDS_1_16_V02 ((qmiLTEBandMaskT_v02)0x00000001ull) /**<  LTE bands from 1 to 16  \n */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_17_32_V02 ((qmiLTEBandMaskT_v02)0x00000002ull) /**<  LTE bands from 17 to 32 \n  */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_33_48_V02 ((qmiLTEBandMaskT_v02)0x00000004ull) /**<  LTE bands from 33 to 48 \n */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_49_64_V02 ((qmiLTEBandMaskT_v02)0x00000008ull) /**<  LTE bands from 49 to 64 \n  */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_65_80_V02 ((qmiLTEBandMaskT_v02)0x00000010ull) /**<  LTE bands from 65 to 80  \n */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_81_96_V02 ((qmiLTEBandMaskT_v02)0x00000020ull) /**<  LTE bands from 81 to 96  \n */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_97_112_V02 ((qmiLTEBandMaskT_v02)0x00000040ull) /**<  LTE bands from 97 to 112 \n  */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_113_128_V02 ((qmiLTEBandMaskT_v02)0x00000080ull) /**<  LTE bands from 113 to 128 \n */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_129_144_V02 ((qmiLTEBandMaskT_v02)0x00000100ull) /**<  LTE bands from 129 to 144 \n */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_145_160_V02 ((qmiLTEBandMaskT_v02)0x00000200ull) /**<  LTE bands from 145 to 160 \n */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_161_176_V02 ((qmiLTEBandMaskT_v02)0x00000400ull) /**<   LTE bands from 161 to 176 \n */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_177_192_V02 ((qmiLTEBandMaskT_v02)0x00000800ull) /**<  LTE bands from 177 to 192  \n */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_193_208_V02 ((qmiLTEBandMaskT_v02)0x00001000ull) /**<  LTE bands from 193 to 208 \n */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_209_224_V02 ((qmiLTEBandMaskT_v02)0x00002000ull) /**<  LTE bands from 209 to 224 \n  */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_225_240_V02 ((qmiLTEBandMaskT_v02)0x00004000ull) /**<  LTE bands from 225 to 240 \n  */
#define QMI_LOC_EVENT_MASK_LTE_BANDS_241_256_V02 ((qmiLTEBandMaskT_v02)0x00008000ull) /**<   LTE bands from 241 to 256  */
/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  qmiLTEBandMaskT_v02 bandMask;
  /**<   Supported sub-band fields: \n
      - QMI_LOC_EVENT_MASK_LTE_BANDS_1_16 (0x00000001) --  LTE bands from 1 to 16  \n
      - QMI_LOC_EVENT_MASK_LTE_BANDS_17_32 (0x00000002) --  LTE bands from 17 to 32 \n 
      - QMI_LOC_EVENT_MASK_LTE_BANDS_33_48 (0x00000004) --  LTE bands from 33 to 48 \n
      - QMI_LOC_EVENT_MASK_LTE_BANDS_49_64 (0x00000008) --  LTE bands from 49 to 64 \n 
      - QMI_LOC_EVENT_MASK_LTE_BANDS_65_80 (0x00000010) --  LTE bands from 65 to 80  \n
      - QMI_LOC_EVENT_MASK_LTE_BANDS_81_96 (0x00000020) --  LTE bands from 81 to 96  \n
      - QMI_LOC_EVENT_MASK_LTE_BANDS_97_112 (0x00000040) --  LTE bands from 97 to 112 \n 
      - QMI_LOC_EVENT_MASK_LTE_BANDS_113_128 (0x00000080) --  LTE bands from 113 to 128 \n
      - QMI_LOC_EVENT_MASK_LTE_BANDS_129_144 (0x00000100) --  LTE bands from 129 to 144 \n
      - QMI_LOC_EVENT_MASK_LTE_BANDS_145_160 (0x00000200) --  LTE bands from 145 to 160 \n
      - QMI_LOC_EVENT_MASK_LTE_BANDS_161_176 (0x00000400) --   LTE bands from 161 to 176 \n
      - QMI_LOC_EVENT_MASK_LTE_BANDS_177_192 (0x00000800) --  LTE bands from 177 to 192  \n
      - QMI_LOC_EVENT_MASK_LTE_BANDS_193_208 (0x00001000) --  LTE bands from 193 to 208 \n
      - QMI_LOC_EVENT_MASK_LTE_BANDS_209_224 (0x00002000) --  LTE bands from 209 to 224 \n 
      - QMI_LOC_EVENT_MASK_LTE_BANDS_225_240 (0x00004000) --  LTE bands from 225 to 240 \n 
      - QMI_LOC_EVENT_MASK_LTE_BANDS_241_256 (0x00008000) --   LTE bands from 241 to 256  */

  uint16_t rfBandsLTE[QMI_LOC_RF_BAND_LENGTH_V02];
  /**<   Indicates which bands are supported by the device.  */
}qmiLTEBandStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Sends an FDCL request to the control point. */
typedef struct {

  /* Optional */
  /*  GTP Client Software Version */
  uint8_t gtpClientSwVer_valid;  /**< Must be set to true if gtpClientSwVer is being passed */
  qmiLocGtpClientSwVerStructT_v02 gtpClientSwVer;

  /* Optional */
  /*  Modem Build String (Non-NULL Terminated) */
  uint8_t modemSwVersion_valid;  /**< Must be set to true if modemSwVersion is being passed */
  char modemSwVersion[QMI_LOC_GNSS_SW_VERSION_STRING_MAX_LENGTH_V02 + 1];
  /**<   First 255 characters of the modem build string. \n
         - Type -- character string \n 
         - Maximum length of the array -- 255
    */

  /* Optional */
  /*  Home Carrier ID */
  uint8_t homeCarrierId_valid;  /**< Must be set to true if homeCarrierId is being passed */
  qmiLocHomeCarrierIdStructT_v02 homeCarrierId;
  /**<   Home carrier ID (only for GSM, WCDMA, or LTE carrier). */

  /* Optional */
  /*  CDMA Realm (Non-NULL Terminated) */
  uint8_t cdmaRealm_valid;  /**< Must be set to true if cdmaRealm is being passed */
  char cdmaRealm[QMI_LOC_GNSS_SW_VERSION_STRING_MAX_LENGTH_V02 + 1];
  /**<   CDMA realm (for home network on CDMA only). \n
         First 255 characters of the CDMA realm. \n
         - Type -- character string \n 
         - Maximum length of the array -- 255  
    */

  /* Optional */
  /*  Maximum Number of Records Accepted */
  uint8_t gtpMaxNumRecordsAccepted_valid;  /**< Must be set to true if gtpMaxNumRecordsAccepted is being passed */
  uint16_t gtpMaxNumRecordsAccepted;
  /**<   Range -- 1 to 65,535 (defaults to 650). */

  /* Optional */
  /*  GPS Week */
  uint8_t gpsWeek_valid;  /**< Must be set to true if gpsWeek is being passed */
  uint16_t gpsWeek;
  /**<   Units -- Week number (since Jan. 1, 1970). */

  /* Optional */
  /*  GPS Milliseconds */
  uint8_t gpsMilliseconds_valid;  /**< Must be set to true if gpsMilliseconds is being passed */
  uint32_t gpsMilliseconds;
  /**<   Units -- Milliseconds (since Jan. 1, 1970). */

  /* Optional */
  /*  Base Station Best Position */
  uint8_t bsBestPosition_valid;  /**< Must be set to true if bsBestPosition is being passed */
  qmiLocBsBestPositionStructT_v02 bsBestPosition;

  /* Optional */
  /*  Current Cell ID */
  uint8_t currentCellId_valid;  /**< Must be set to true if currentCellId is being passed */
  qmiLocFdclCellIdStructT_v02 currentCellId;

  /* Optional */
  /*  Requested Data Types */
  uint8_t requestedDataTypes_valid;  /**< Must be set to true if requestedDataTypes is being passed */
  uint8_t requestedDataTypes;
  /**<   Requested data types. */

  /* Optional */
  /*  Restricted Region Version */
  uint8_t restrictedRegionVersion_valid;  /**< Must be set to true if restrictedRegionVersion is being passed */
  uint8_t restrictedRegionVersion;

  /* Optional */
  /*  RF Bands LTE */
  uint8_t rfBandsLte_valid;  /**< Must be set to true if rfBandsLte is being passed */
  qmiLTEBandStructT_v02 rfBandsLte;
  /**<   \n Indicates which LTE bands are supported. */

  /* Optional */
  /*  RF Bands GSM */
  uint8_t rfBandsGSM_valid;  /**< Must be set to true if rfBandsGSM is being passed */
  uint16_t rfBandsGSM;
  /**<   Indicates which GSM bands are supported. */

  /* Optional */
  /*  Location Capabilities */
  uint8_t capabilitiesEnabled_valid;  /**< Must be set to true if capabilitiesEnabled is being passed */
  qmiLocCapabilitiesMaskT_v02 capabilitiesEnabled;
  /**<   Mask indicating capabilities.
 Valid bitmasks: \n
      - QMI_LOC_CAP_MASK_GTP (0x00000001) --  GTP capability of this device is enabled  \n 
      - QMI_LOC_CAP_MASK_WIFI (0x00000002) --  GNSS capability of this device is enabled \n
      - QMI_LOC_CAP_MASK_GNSS (0x00000004) --  GNSS capability of this device is enabled \n
      - QMI_LOC_CAP_MASK_GTP_CACHE (0x00000008) --  GTP cache fix of this device is enabled  */

  /* Optional */
  /*  Scan Assistance ID  */
  uint8_t scanAssistId_valid;  /**< Must be set to true if scanAssistId is being passed */
  uint64_t scanAssistId;
  /**<   Scan assistance ID. */

  /* Optional */
  /*  Configuration ID */
  uint8_t configId_valid;  /**< Must be set to true if configId is being passed */
  uint8_t configId;
  /**<   Configuration ID. */
}qmiLocEventFdclServiceReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Requests the FDCL BS (Base station) list from the TLE. */
typedef struct {

  /* Optional */
  /*  Maximum number of BS to send */
  uint8_t maxNumBs_valid;  /**< Must be set to true if maxNumBs is being passed */
  uint16_t maxNumBs;
  /**<   Maximum number of BS to send by the TLE. 
        All the available BS are sent when this optional field is not included. \n
        - Range -- 1 to 65,535 */
}qmiLocGetFdclBsListReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCCELLRECORDTYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_CELL_RECORD_TYPE_INVALID_V02 = 0, /**<  Invalid value \n */
  eQMI_LOC_CELL_RECORD_TYPE_L1_REGION_WITH_POSITION_V02 = 1, /**<  L1 region with position \n */
  eQMI_LOC_CELL_RECORD_TYPE_L2_REGION_WITH_POSITION_V02 = 2, /**<  L2 region with position \n */
  eQMI_LOC_CELL_RECORD_TYPE_L3_REGION_WITH_POSITION_V02 = 3, /**<  L3 region with position \n  */
  eQMI_LOC_CELL_RECORD_TYPE_L4_CELL_RECORD_WITH_POSITION_V02 = 4, /**<  L4 cell record with position \n  */
  eQMI_LOC_CELL_RECORD_TYPE_L4_CELL_RECORD_NOT_RESOLVED_V02 = 5, /**<  L4 cell record not resolved \n */
  eQMI_LOC_CELL_RECORD_TYPE_L4_CELL_RECORD_NO_INFO_V02 = 6, /**<  L4 cell record no information  */
  QMILOCCELLRECORDTYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocCellRecordTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Requests the FDCL BS (Base station) list from the TLE. */
typedef struct {

  /* Mandatory */
  /*  Get FDCL BS List Request Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_GET_FDCL_BS_LIST_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Mandatory */
  /*  Base Station List */
  uint32_t BsList_len;  /**< Must be set to # of elements in BsList */
  qmiLocFdclCellIdStructT_v02 BsList[QMI_LOC_FDCL_BS_LIST_MAX_SIZE_V02];
  /**<   \vspace{4pt} \n A list of base station IDs for FDCL request. */

  /* Optional */
  /*  More BS Lists are Available */
  uint8_t moreBsAvailable_valid;  /**< Must be set to true if moreBsAvailable is being passed */
  uint8_t moreBsAvailable;
  /**<   Indicates whether more base station lists are available. \n
       - 0x00 (FALSE) -- No more base station lists are available\n
       - 0x01 (TRUE) -- More base station lists are available
       If not specified, moreBsAvailable defaults to FALSE.
  */

  /* Optional */
  /*  Base Station Best Position */
  uint8_t bsBestPosition_valid;  /**< Must be set to true if bsBestPosition is being passed */
  qmiLocBsBestPositionStructT_v02 bsBestPosition;

  /* Optional */
  /*  Data Buffer  */
  uint8_t dataBuffer_valid;  /**< Must be set to true if dataBuffer is being passed */
  uint32_t dataBuffer_len;  /**< Must be set to # of elements in dataBuffer */
  uint8_t dataBuffer[QMI_LOC_SCAN_DATA_MAX_SIZE_V02];
  /**<   Data buffer.*/

  /* Optional */
  /*  Serving Cell Information  */
  uint8_t servCellId_valid;  /**< Must be set to true if servCellId is being passed */
  qmiLocFdclCellIdExtStructT_v02 servCellId;
}qmiLocGetFdclBsListIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocFdclCellPosValidMaskT_v02;
#define QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALTITUDE_V02 ((qmiLocFdclCellPosValidMaskT_v02)0x00000001) /**<  Altitude field is valid in cell position \n  */
#define QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_UNC_V02 ((qmiLocFdclCellPosValidMaskT_v02)0x00000002) /**<  Altitude uncertainty field is valid in cell position \n */
#define QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_CONFIDENCE_V02 ((qmiLocFdclCellPosValidMaskT_v02)0x00000004) /**<  Altitude confidence is valid in cell position \n */
#define QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_RELIABILITY_V02 ((qmiLocFdclCellPosValidMaskT_v02)0x00000008) /**<  Altitude reliability field is valid in cell position  */
/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocFdclCellPosValidMaskT_v02 validMask;
  /**<   Bitmask indicating which of the fields in this TLV are valid: \n
      - QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALTITUDE (0x00000001) --  Altitude field is valid in cell position \n 
      - QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_UNC (0x00000002) --  Altitude uncertainty field is valid in cell position \n
      - QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_CONFIDENCE (0x00000004) --  Altitude confidence is valid in cell position \n
      - QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_RELIABILITY (0x00000008) --  Altitude reliability field is valid in cell position 
 */

  qmiLocFdclCellIdStructT_v02 cellId;
  /**<   \vspace{4pt} \n Cell ID for this record. */

  double latitude;
  /**<   Latitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -90.0 to 90.0 (positive values indicate northern latitude,
         negative values indicate southern latitude)
 */

  double longitude;
  /**<   Longitude (specified in WGS84 datum).\n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -180.0 to 180.0 (positive values indicate eastern longitude,
                  negative values indicate western longitude)
      */

  float horCoverageRadius;
  /**<   Horizontal coverage radius (circular).\n
        - Units -- Meters */

  uint8_t horConfidence;
  /**<   Horizontal confidence, as defined by \hyperref[TS0332]{TS 03.32}. \n
        - Units: Percent (1 to 99) \n
        - 0, 101 to 255 -- invalid value \n
        - If 100 is received, reinterpret to 99        
    */

  qmiLocReliabilityEnumT_v02 horReliability;
  /**<   Reliability of the horizontal position.
 Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Not set \n 
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Very low; use it at your own risk \n 
      - eQMI_LOC_RELIABILITY_LOW (2) --  Low; little or no cross-checking is possible \n 
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Medium; limited cross-check passed \n 
      - eQMI_LOC_RELIABILITY_HIGH (4) --  High; strong cross-check passed 
 */

  float altitude;
  /**<   Altitude with respect to mean sea level.\n
       - Units -- Meters */

  float altUnc;
  /**<   Vertical uncertainty; mandatory when either altitudeWrtEllipsoid
        or altitudeWrtMeanSeaLevel is specified.\n
        - Units -- Meters */

  uint8_t altConfidence;
  /**<   Vertical confidence, as defined by \hyperref[TS0332]{TS 03.32}. \n
        - Units: Percent (0-99) \n
        - 0 -- invalid value \n
        - 100 to 256 -- not used \n
        - If 100 is received, reinterpret to 99        
     */

  qmiLocReliabilityEnumT_v02 altReliability;
  /**<   Reliability of the vertical position.
 Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Not set \n 
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Very low; use it at your own risk \n 
      - eQMI_LOC_RELIABILITY_LOW (2) --  Low; little or no cross-checking is possible \n 
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Medium; limited cross-check passed \n 
      - eQMI_LOC_RELIABILITY_HIGH (4) --  High; strong cross-check passed 
 */
}qmiLocFdclCellPosStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocFdclCellPosValidMaskT_v02 validMask;
  /**<   Bitmask indicating which of the fields in this TLV are valid: \n
      - QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALTITUDE (0x00000001) --  Altitude field is valid in cell position \n 
      - QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_UNC (0x00000002) --  Altitude uncertainty field is valid in cell position \n
      - QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_CONFIDENCE (0x00000004) --  Altitude confidence is valid in cell position \n
      - QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_RELIABILITY (0x00000008) --  Altitude reliability field is valid in cell position 
 */

  double latitude;
  /**<   Latitude (specified in WGS84 datum). \n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -90.0 to 90.0 (positive values indicate northern latitude,
       negative values indicate southern latitude)
    */

  double longitude;
  /**<   Longitude (specified in WGS84 datum).\n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -180.0 to 180.0 (positive values indicate eastern longitude,
       negative values indicate western longitude)
     */

  float horCoverageRadius;
  /**<   Horizontal coverage radius (circular).\n
        - Units -- Meters */

  uint8_t horConfidence;
  /**<   Horizontal confidence, as defined by \hyperref[TS0332]{TS 03.32}. \n
        - Units -- Percent (1 to 99) \n
        - 0, 101 to 255 -- invalid value \n
        - If 100 is received, reinterpret to 99.        
    */

  qmiLocReliabilityEnumT_v02 horReliability;
  /**<   Reliability of the horizontal position.
 Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Not set \n 
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Very low; use it at your own risk \n 
      - eQMI_LOC_RELIABILITY_LOW (2) --  Low; little or no cross-checking is possible \n 
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Medium; limited cross-check passed \n 
      - eQMI_LOC_RELIABILITY_HIGH (4) --  High; strong cross-check passed 
 */

  float altitude;
  /**<   Altitude with respect to mean sea level.\n
       - Units -- Meters */

  float altUnc;
  /**<   Vertical uncertainty; mandatory if either altitudeWrtEllipsoid
        or altitudeWrtMeanSeaLevel is specified.\n
        - Units -- Meters */

  uint8_t altConfidence;
  /**<   Vertical confidence, as defined by \hyperref[TS0332]{TS 03.32}. \n
        - Units -- Percent (0-99) \n
        - 0 -- invalid value \n
        - 100 to 256 -- not used \n
        - If 100 is received, reinterpret to 99.        
       */

  qmiLocReliabilityEnumT_v02 altReliability;
  /**<   Reliability of the vertical position.
 Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Not set \n 
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Very low; use it at your own risk \n 
      - eQMI_LOC_RELIABILITY_LOW (2) --  Low; little or no cross-checking is possible \n 
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Medium; limited cross-check passed \n 
      - eQMI_LOC_RELIABILITY_HIGH (4) --  High; strong cross-check passed 
 */
}qmiLocFdclPosStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMIGTPSTATUSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_GTPWWAN_DOWNLOAD_SUCCESS_V02 = 0, /**<  Download success \n */
  eQMI_GTPWWAN_QMI_INTIALIZATION_FAIL_V02 = 1, /**<  Initialization failure \n */
  eQMI_GTPWWAN_QMI_FDCL_LIST_FAIL_V02 = 2, /**<  Failure getting FDCL list from QMI \n */
  eQMI_GTPWWAN_ENCODE_FAIL_V02 = 3, /**< Failure during binary encoding \n */
  eQMI_GTPWWAN_DATA_SERVICE_FAIL_V02 = 4, /**<  Data Service not available \n */
  eQMI_GTPWWAN_SSL_FAIL_V02 = 5, /**<  Failure in SSL \n */
  eQMI_GTPWWAN_CONNECTION_FAIL_V02 = 6, /**<  Failure in HTTP connection \n  */
  eQMI_GTPWWAN_HTTP_REQUEST_FAIL_V02 = 7, /**<  Failure in sending request to server \n  */
  eQMI_GTPWWAN_HTTP_DOWNLOAD_ERROR_V02 = 8, /**<  Failure in HTTP download \n */
  eQMI_GTPWWAN_SERVER_REJECT_V02 = 9, /**<  Request rejected by server \n */
  eQMI_GTPWWAN_DECODE_FAIL_V02 = 10, /**<  Failure during decode \n */
  eQMI_GTPWWAN_UNSPECIFIED_FAILURE_V02 = 11, /**<  Unspecified failure */
  QMIGTPSTATUSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiGtpStatusEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Injects FDCL data to the TLE. */
typedef struct {

  /* Mandatory */
  /*  FDCL Cell Position List */
  uint32_t cellPosList_len;  /**< Must be set to # of elements in cellPosList */
  qmiLocFdclCellPosStructT_v02 cellPosList[QMI_LOC_FDCL_CELL_POS_LIST_LENGTH_V02];

  /* Mandatory */
  /*  Days Valid */
  uint32_t daysValid;
  /**<   Number of days for which this FDCL data is valid. */

  /* Mandatory */
  /*  GTP Status  */
  qmiGtpStatusEnumT_v02 status;
  /**<   Valid values: \n
      - eQMI_GTPWWAN_DOWNLOAD_SUCCESS (0) --  Download success \n
      - eQMI_GTPWWAN_QMI_INTIALIZATION_FAIL (1) --  Initialization failure \n
      - eQMI_GTPWWAN_QMI_FDCL_LIST_FAIL (2) --  Failure getting FDCL list from QMI \n
      - eQMI_GTPWWAN_ENCODE_FAIL (3) -- Failure during binary encoding \n
      - eQMI_GTPWWAN_DATA_SERVICE_FAIL (4) --  Data Service not available \n
      - eQMI_GTPWWAN_SSL_FAIL (5) --  Failure in SSL \n
      - eQMI_GTPWWAN_CONNECTION_FAIL (6) --  Failure in HTTP connection \n 
      - eQMI_GTPWWAN_HTTP_REQUEST_FAIL (7) --  Failure in sending request to server \n 
      - eQMI_GTPWWAN_HTTP_DOWNLOAD_ERROR (8) --  Failure in HTTP download \n
      - eQMI_GTPWWAN_SERVER_REJECT (9) --  Request rejected by server \n
      - eQMI_GTPWWAN_DECODE_FAIL (10) --  Failure during decode \n
      - eQMI_GTPWWAN_UNSPECIFIED_FAILURE (11) --  Unspecified failure
 */

  /* Optional */
  /*  UTC Timestamp */
  uint8_t timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64_t timestampUtc;
  /**<   - Units -- Milliseconds (since Jan. 1, 1970) */

  /* Optional */
  /*  Cell Record Type List */
  uint8_t cellRecordTypeList_valid;  /**< Must be set to true if cellRecordTypeList is being passed */
  uint32_t cellRecordTypeList_len;  /**< Must be set to # of elements in cellRecordTypeList */
  qmiLocCellRecordTypeEnumT_v02 cellRecordTypeList[QMI_LOC_FDCL_CELL_POS_LIST_LENGTH_V02];
  /**<   A list of cell record types for FDCL request. \n
 The cell record type list shall be aligned with the records in cellPosList. \n
 Valid values: \n
      - eQMI_LOC_CELL_RECORD_TYPE_INVALID (0) --  Invalid value \n
      - eQMI_LOC_CELL_RECORD_TYPE_L1_REGION_WITH_POSITION (1) --  L1 region with position \n
      - eQMI_LOC_CELL_RECORD_TYPE_L2_REGION_WITH_POSITION (2) --  L2 region with position \n
      - eQMI_LOC_CELL_RECORD_TYPE_L3_REGION_WITH_POSITION (3) --  L3 region with position \n 
      - eQMI_LOC_CELL_RECORD_TYPE_L4_CELL_RECORD_WITH_POSITION (4) --  L4 cell record with position \n 
      - eQMI_LOC_CELL_RECORD_TYPE_L4_CELL_RECORD_NOT_RESOLVED (5) --  L4 cell record not resolved \n
      - eQMI_LOC_CELL_RECORD_TYPE_L4_CELL_RECORD_NO_INFO (6) --  L4 cell record no information 
 */

  /* Optional */
  /*  More FDCL Data Available */
  uint8_t moreBsAvailable_valid;  /**< Must be set to true if moreBsAvailable is being passed */
  uint8_t moreBsAvailable;
  /**<   Indicates whether more FDCL data is available. \n
       - 0x00 (FALSE) -- No more FDCL data is available. \n
       - 0x01 (TRUE)  -- More FDCL data is available. \n
       If not specified, moreBsAvailable defaults to FALSE.
  */

  /* Optional */
  /*  Client Controls */
  uint8_t clientControls_valid;  /**< Must be set to true if clientControls is being passed */
  uint16_t clientControls;
  /**<   Client controls. */

  /* Optional */
  /*  Maximum Number of Cells to Upload */
  uint8_t maxNumCellsToUpload_valid;  /**< Must be set to true if maxNumCellsToUpload is being passed */
  uint16_t maxNumCellsToUpload;
  /**<   Maximum number of cells to upload. */

  /* Optional */
  /*  Minimum Seconds Between Requests */
  uint8_t minSecsBtwRequests_valid;  /**< Must be set to true if minSecsBtwRequests is being passed */
  uint16_t minSecsBtwRequests;
  /**<   Minimum seconds between requests. */

  /* Optional */
  /*  Restricted Region Version */
  uint8_t restrictedRegionVersion_valid;  /**< Must be set to true if restrictedRegionVersion is being passed */
  uint8_t restrictedRegionVersion;
  /**<   Restricted region version. */

  /* Optional */
  /*  Maximum Number of Cells to Download */
  uint8_t maxNumCellsToDownload_valid;  /**< Must be set to true if maxNumCellsToDownload is being passed */
  uint16_t maxNumCellsToDownload;
  /**<   Maximum number of cells to download. */

  /* Optional */
  /*  Maximum Number of GTP Cell Requests with Server Access per Day */
  uint8_t maxGtpCellRequestsPerDay_valid;  /**< Must be set to true if maxGtpCellRequestsPerDay is being passed */
  uint16_t maxGtpCellRequestsPerDay;
  /**<   Maximum Number of GTP cell requests with server access per day. */

  /* Optional */
  /*  Maximum Value of Delta Time in Seconds to Crowdsource Data.   */
  uint8_t posDeltaTimeThreshold_valid;  /**< Must be set to true if posDeltaTimeThreshold is being passed */
  uint32_t posDeltaTimeThreshold;
  /**<   Maximum value of delta time in seconds to crowdsource data. \n
       - Units -- Seconds */

  /* Optional */
  /*  List of Restricted Regions */
  uint8_t restrictedRegions_valid;  /**< Must be set to true if restrictedRegions is being passed */
  uint32_t restrictedRegions_len;  /**< Must be set to # of elements in restrictedRegions */
  qmiLocHomeCarrierIdStructT_v02 restrictedRegions[QMI_LOC_RESTRICTED_REGIONS_LIST_MAX_SIZE_V02];
  /**<    */

  /* Optional */
  /*  Response Buffer */
  uint8_t responseBuffer_valid;  /**< Must be set to true if responseBuffer is being passed */
  uint32_t responseBuffer_len;  /**< Must be set to # of elements in responseBuffer */
  uint8_t responseBuffer[QMI_LOC_INJ_DATA_MAX_SIZE_V02];
  /**<   Response buffer. */

  /* Optional */
  /*  Position Information */
  uint8_t responsePos_valid;  /**< Must be set to true if responsePos is being passed */
  qmiLocFdclPosStructT_v02 responsePos;
}qmiLocInjectFdclDataReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCINJECTFDCLDATASTATUSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_INJECT_FDCL_DATA_SUCCESS_V02 = 0, /**<  Completed successfully \n  */
  eQMI_LOC_INJECT_FDCL_DATA_FAILURE_GENERAL_V02 = 1, /**<  Failed \n  */
  eQMI_LOC_INJECT_FDCL_DATA_FAILURE_NO_CELLS_INJECTED_V02 = 2, /**<  Failed because no cells were injected   */
  QMILOCINJECTFDCLDATASTATUSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocInjectFdclDataStatusEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Injects FDCL data to the TLE. */
typedef struct {

  /* Mandatory */
  /*  Inject FDCL Data Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_INJECT_FDCL_DATA_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Mandatory */
  /*  FDCL Data Injection Status */
  qmiLocInjectFdclDataStatusEnumT_v02 inject_status;
  /**<   Status of the FDCL data injection request.
 Valid values: \n
      - eQMI_LOC_INJECT_FDCL_DATA_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_INJECT_FDCL_DATA_FAILURE_GENERAL (1) --  Failed \n 
      - eQMI_LOC_INJECT_FDCL_DATA_FAILURE_NO_CELLS_INJECTED (2) --  Failed because no cells were injected  
 */

  /* Optional */
  /*  Error Message (NULL-Terminated) */
  uint8_t errorMsg_valid;  /**< Must be set to true if errorMsg is being passed */
  char errorMsg[QMI_LOC_INJECT_FDCL_DATA_ERROR_MSG_LEN_V02 + 1];
  /**<   Error message. \n
        - Type -- NULL-terminated string \n
        - Maximum string length (including NULL terminator) -- 256
     */
}qmiLocInjectFdclDataIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Receives position reports. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmiLocStartListenPositionReqMsgT_v02;

  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Stops receiving position reports. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmiLocStopListenPositionReqMsgT_v02;

  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCSYSTEMINFOENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_UNDEFINED_INFO_V02 = 0, /**<  Undefined \n  */
  eQMI_LOC_NEXT_LEAP_SECOND_INFO_V02 = 1, /**<  Information about the next (upcoming) leap second  */
  QMILOCSYSTEMINFOENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSystemInfoEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t gpsTimeCurrent_valid;
  /**<   Specifies whether the field gpsTimeCurrent contains
        a valid value*/

  qmiLocGPSTimeStructT_v02 gpsTimeCurrent;
  /**<   \vspace{0.06in} \n Number of weeks since Jan. 5, 1980, and
         milliseconds into the current week. */

  uint8_t leapSecondsCurrent_valid;
  /**<   Specifies whether the field leapSecondsCurrent contains
        a valid value*/

  uint8_t leapSecondsCurrent;
  /**<   Current leap second information.\n
       - Units -- Seconds */

  uint8_t gpsTimeNextLsEvent_valid;
  /**<   Specifies whether the gpsTimeNextLsEvent field contains
        a valid value*/

  qmiLocGPSTimeStructT_v02 gpsTimeNextLsEvent;
  /**<   \vspace{0.06in} \n Number of weeks since Jan. 5, 1980, and
         milliseconds into the week for next leap-second change event.
         Reported only when receiver has information
         on an upcoming change event. */

  uint8_t leapSecondsNext_valid;
  /**<   Specifies whether the field leapSecondsNext contains
        a valid value */

  uint8_t leapSecondsNext;
  /**<   Upcoming leap second information. 
         Reported only when the receiver has information
         on an upcoming change event. \n
        - Units -- Seconds */
}qmiLocNextLeapSecondInfoStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Generic request, queries about system information. */
typedef struct {

  /* Mandatory */
  /*  System Information */
  qmiLocSystemInfoEnumT_v02 systemInfo;
  /**<   System information being requested.
 Valid values: \n
      - eQMI_LOC_UNDEFINED_INFO (0) --  Undefined \n 
      - eQMI_LOC_NEXT_LEAP_SECOND_INFO (1) --  Information about the next (upcoming) leap second 
 */
}qmiLocSystemInfoReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Generic request, queries about system information. */
typedef struct {

  /* Mandatory */
  /*  System Information */
  qmiLocSystemInfoEnumT_v02 systemInfo;
  /**<   System information being requested.
 Valid values: \n
      - eQMI_LOC_UNDEFINED_INFO (0) --  Undefined \n 
      - eQMI_LOC_NEXT_LEAP_SECOND_INFO (1) --  Information about the next (upcoming) leap second 
 */

  /* Optional */
  /*  Next (Upcoming) Leap Second Information */
  uint8_t nextLeapSecondInfo_valid;  /**< Must be set to true if nextLeapSecondInfo is being passed */
  qmiLocNextLeapSecondInfoStructT_v02 nextLeapSecondInfo;
  /**<   \n Reported only when the receiver has information
       on an upcoming leap second change event.*/

  /* Optional */
  /*  Uncertainty of GPS time */
  uint8_t gpsTimeUncMs_valid;  /**< Must be set to true if gpsTimeUncMs is being passed */
  uint32_t gpsTimeUncMs;
  /**<    Uncertainty of GPS time in milliseconds            
        Consider this field as the part of the qmiLocNextLeapSecondInfoStructT
        structure that cannot be modified.*/
}qmiLocSystemInfoIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCCONSTELLATIONSTATUSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_CONSTELLATION_ENABLED_MANDATORY_V02 = 0, /**<  Mandatory constellation; always enabled \n  */
  eQMI_LOC_CONSTELLATION_ENABLED_INTERNALLY_V02 = 1, /**<  Enabled internally by the system software \n */
  eQMI_LOC_CONSTELLATION_ENABLED_BY_CLIENT_V02 = 2, /**<  Enabled by the external client \n  */
  eQMI_LOC_CONSTELLATION_DISABLED_NOT_SUPPORTED_V02 = 100, /**<  Constellation not supported \n  */
  eQMI_LOC_CONSTELLATION_DISABLED_INTERNALLY_V02 = 101, /**<  Disabled internally by the system software \n */
  eQMI_LOC_CONSTELLATION_DISABLED_BY_CLIENT_V02 = 102, /**<  Disabled by the external client \n  */
  eQMI_LOC_CONSTELLATION_DISABLED_NO_MEMORY_V02 = 103, /**<  Could not be enabled due to memory allocation failure  */
  QMILOCCONSTELLATIONSTATUSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocConstellationStatusEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Queries the GNSS constellation configuration. */
typedef struct {

  /* Mandatory */
  /*  Get Constellation Configuration Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the constellation configuration query request. \n
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  GPS Constellation Status */
  uint8_t gps_status_valid;  /**< Must be set to true if gps_status is being passed */
  qmiLocConstellationStatusEnumT_v02 gps_status;
  /**<   Enablement status of GPS. \n
 Valid values: \n
      - eQMI_LOC_CONSTELLATION_ENABLED_MANDATORY (0) --  Mandatory constellation; always enabled \n 
      - eQMI_LOC_CONSTELLATION_ENABLED_INTERNALLY (1) --  Enabled internally by the system software \n
      - eQMI_LOC_CONSTELLATION_ENABLED_BY_CLIENT (2) --  Enabled by the external client \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_NOT_SUPPORTED (100) --  Constellation not supported \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_INTERNALLY (101) --  Disabled internally by the system software \n
      - eQMI_LOC_CONSTELLATION_DISABLED_BY_CLIENT (102) --  Disabled by the external client \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_NO_MEMORY (103) --  Could not be enabled due to memory allocation failure 
 */

  /* Optional */
  /*  GLONASS Constellation Status */
  uint8_t glonass_status_valid;  /**< Must be set to true if glonass_status is being passed */
  qmiLocConstellationStatusEnumT_v02 glonass_status;
  /**<   Enablement status of GLONASS. \n
 Valid values: \n
      - eQMI_LOC_CONSTELLATION_ENABLED_MANDATORY (0) --  Mandatory constellation; always enabled \n 
      - eQMI_LOC_CONSTELLATION_ENABLED_INTERNALLY (1) --  Enabled internally by the system software \n
      - eQMI_LOC_CONSTELLATION_ENABLED_BY_CLIENT (2) --  Enabled by the external client \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_NOT_SUPPORTED (100) --  Constellation not supported \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_INTERNALLY (101) --  Disabled internally by the system software \n
      - eQMI_LOC_CONSTELLATION_DISABLED_BY_CLIENT (102) --  Disabled by the external client \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_NO_MEMORY (103) --  Could not be enabled due to memory allocation failure 
 */

  /* Optional */
  /*  BDS Constellation Status */
  uint8_t bds_status_valid;  /**< Must be set to true if bds_status is being passed */
  qmiLocConstellationStatusEnumT_v02 bds_status;
  /**<   Enablement status of BDS. \n
 Valid values: \n
      - eQMI_LOC_CONSTELLATION_ENABLED_MANDATORY (0) --  Mandatory constellation; always enabled \n 
      - eQMI_LOC_CONSTELLATION_ENABLED_INTERNALLY (1) --  Enabled internally by the system software \n
      - eQMI_LOC_CONSTELLATION_ENABLED_BY_CLIENT (2) --  Enabled by the external client \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_NOT_SUPPORTED (100) --  Constellation not supported \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_INTERNALLY (101) --  Disabled internally by the system software \n
      - eQMI_LOC_CONSTELLATION_DISABLED_BY_CLIENT (102) --  Disabled by the external client \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_NO_MEMORY (103) --  Could not be enabled due to memory allocation failure 
 */

  /* Optional */
  /*  QZSS Constellation Status */
  uint8_t qzss_status_valid;  /**< Must be set to true if qzss_status is being passed */
  qmiLocConstellationStatusEnumT_v02 qzss_status;
  /**<   Enablement status of QZSS. \n
 Valid values: \n
      - eQMI_LOC_CONSTELLATION_ENABLED_MANDATORY (0) --  Mandatory constellation; always enabled \n 
      - eQMI_LOC_CONSTELLATION_ENABLED_INTERNALLY (1) --  Enabled internally by the system software \n
      - eQMI_LOC_CONSTELLATION_ENABLED_BY_CLIENT (2) --  Enabled by the external client \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_NOT_SUPPORTED (100) --  Constellation not supported \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_INTERNALLY (101) --  Disabled internally by the system software \n
      - eQMI_LOC_CONSTELLATION_DISABLED_BY_CLIENT (102) --  Disabled by the external client \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_NO_MEMORY (103) --  Could not be enabled due to memory allocation failure 
 */

  /* Optional */
  /*  Galileo Constellation Status */
  uint8_t galileo_status_valid;  /**< Must be set to true if galileo_status is being passed */
  qmiLocConstellationStatusEnumT_v02 galileo_status;
  /**<   Enablement status of Galileo. \n
 Valid values: \n
      - eQMI_LOC_CONSTELLATION_ENABLED_MANDATORY (0) --  Mandatory constellation; always enabled \n 
      - eQMI_LOC_CONSTELLATION_ENABLED_INTERNALLY (1) --  Enabled internally by the system software \n
      - eQMI_LOC_CONSTELLATION_ENABLED_BY_CLIENT (2) --  Enabled by the external client \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_NOT_SUPPORTED (100) --  Constellation not supported \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_INTERNALLY (101) --  Disabled internally by the system software \n
      - eQMI_LOC_CONSTELLATION_DISABLED_BY_CLIENT (102) --  Disabled by the external client \n 
      - eQMI_LOC_CONSTELLATION_DISABLED_NO_MEMORY (103) --  Could not be enabled due to memory allocation failure 
 */
}qmiLocGetConstellationConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint64_t qmiLocConstellationMaskT_v02;
#define QMI_LOC_CONSTELLATION_GLO_V02 ((qmiLocConstellationMaskT_v02)0x00000001ull) /**<  Enable GLONASS. \n  */
#define QMI_LOC_CONSTELLATION_BDS_V02 ((qmiLocConstellationMaskT_v02)0x00000002ull) /**<  Enable BDS.\n  */
#define QMI_LOC_CONSTELLATION_QZSS_V02 ((qmiLocConstellationMaskT_v02)0x00000004ull) /**<  Enable QZSS. \n */
#define QMI_LOC_CONSTELLATION_GAL_V02 ((qmiLocConstellationMaskT_v02)0x00000008ull) /**<  Enable Galileo. \n */
#define QMI_LOC_CONSTELLATION_NAVIC_V02 ((qmiLocConstellationMaskT_v02)0x00000010ull) /**<  Enable NavIC.  */
/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Used by the control point to enable/disable
                      GNSS constellations */
typedef struct {

  /* Mandatory */
  /*  Reset GNSS Constellation Control */
  uint8_t resetConstellations;
  /**<   Reset GNSS Constellation Control
       Values: \n
       - TRUE --  Resets the GNSS constellation control to NV default. \n
                  Optional TLVs are disregarded.\n
       - FALSE -- Does not reset GNSS constellation control.
  */

  /* Optional */
  /*  GNSS Constellations to Enable  */
  uint8_t enableMask_valid;  /**< Must be set to true if enableMask is being passed */
  qmiLocConstellationMaskT_v02 enableMask;
  /**<   Specifies which GNSS constellations to enable.
 GPS is always enabled.
 Valid only when resetConstellations is FALSE.
      - QMI_LOC_CONSTELLATION_GLO (0x00000001) --  Enable GLONASS. \n 
      - QMI_LOC_CONSTELLATION_BDS (0x00000002) --  Enable BDS.\n 
      - QMI_LOC_CONSTELLATION_QZSS (0x00000004) --  Enable QZSS. \n
      - QMI_LOC_CONSTELLATION_GAL (0x00000008) --  Enable Galileo. \n
      - QMI_LOC_CONSTELLATION_NAVIC (0x00000010) --  Enable NavIC. 
 */

  /* Optional */
  /*  GNSS Constellations to Disable */
  uint8_t disableMask_valid;  /**< Must be set to true if disableMask is being passed */
  qmiLocConstellationMaskT_v02 disableMask;
  /**<   Specifies which GNSS constellations to disable.
 GPS cannot be disabled.
 Valid only when resetConstellations is FALSE. \n
      - QMI_LOC_CONSTELLATION_GLO (0x00000001) --  Enable GLONASS. \n 
      - QMI_LOC_CONSTELLATION_BDS (0x00000002) --  Enable BDS.\n 
      - QMI_LOC_CONSTELLATION_QZSS (0x00000004) --  Enable QZSS. \n
      - QMI_LOC_CONSTELLATION_GAL (0x00000008) --  Enable Galileo. \n
      - QMI_LOC_CONSTELLATION_NAVIC (0x00000010) --  Enable NavIC. 
 */
}qmiLocSetConstellationConfigReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Enables position injection in test mode. */
typedef struct {

  /* Mandatory */
  /*  Enable/Disable Test Mode */
  uint8_t enableTestMode;
  /**<   Specifies whether the test mode is enabled. 
       If the control point has already enabled the test mode,
       this command shall have no effect */
}qmiLocEnablePositionInjectionTestModeReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCENABLEPOSITIONINJECTIONTESTMODEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_TEST_MODE_UNKNOWN_V02 = 0, /**<  Test mode in unknown state \n  */
  eQMI_LOC_TEST_MODE_ENABLED_V02 = 1, /**<  Test mode is enabled \n  */
  eQMI_LOC_TEST_MODE_DISABLED_V02 = 2, /**<  Test mode is disbled \n  */
  eQMI_LOC_TEST_MODE_ALREADY_ENABLED_V02 = 3, /**<  Test mode is already enabled \n  */
  eQMI_LOC_TEST_MODE_ALREADY_DISABLED_V02 = 4, /**<  Test mode is already disabled  */
  QMILOCENABLEPOSITIONINJECTIONTESTMODEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocEnablePositionInjectionTestModeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Enables position injection in test mode. */
typedef struct {

  /* Mandatory */
  /*  Position Injection Test Mode Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of enable position injection test mode request. \n
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Test Mode State */
  uint8_t testModeState_valid;  /**< Must be set to true if testModeState is being passed */
  qmiLocEnablePositionInjectionTestModeEnumT_v02 testModeState;
  /**<   State of the test mode. \n
 Valid values: \n
      - eQMI_LOC_TEST_MODE_UNKNOWN (0) --  Test mode in unknown state \n 
      - eQMI_LOC_TEST_MODE_ENABLED (1) --  Test mode is enabled \n 
      - eQMI_LOC_TEST_MODE_DISABLED (2) --  Test mode is disbled \n 
      - eQMI_LOC_TEST_MODE_ALREADY_ENABLED (3) --  Test mode is already enabled \n 
      - eQMI_LOC_TEST_MODE_ALREADY_DISABLED (4) --  Test mode is already disabled 
 */
}qmiLocEnablePositionInjectionTestModeIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Injects a test position. */
typedef struct {

  /* Mandatory */
  /*  Latitude */
  double latitude;
  /**<   Latitude (specified in WGS84 datum).\n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -90.0 to 90.0 (positive values indicate northern latitude,
         negative values indicate southern latitude)
    */

  /* Mandatory */
  /*  Longitude */
  double longitude;
  /**<   Longitude (specified in WGS84 datum).\n
       - Type -- Floating point \n
       - Units -- Degrees \n
       - Range -- -180.0 to 180.0 (positive values indicate eastern longitude,
         negative values indicate western longitude)
     */

  /* Mandatory */
  /*  Circular Horizontal Uncertainty */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
       - Units -- Meters */

  /* Mandatory */
  /*  UTC Timestamp */
  uint64_t timestampUtc;
  /**<   UTC timestamp. \n
       - Units -- Milliseconds since Jan. 1, 1970
       */

  /* Optional */
  /*  Position Source */
  uint8_t positionSource_valid;  /**< Must be set to true if positionSource is being passed */
  qmiLocPositionSourceEnumT_v02 positionSource;
  /**<   Position source \n
 Valid values: \n
      - eQMI_LOC_POS_SRC_GNSS (1) --  Source of the position is GNSS 
 */
}qmiLocInjectTestPositionReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Injects a test position. */
typedef struct {

  /* Mandatory */
  /*  Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the injected test position.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */
}qmiLocInjectTestPositionIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Requests the control point to upload observed BS data. */
typedef struct {

  /* Mandatory */
  /*  Number of Cells Available */
  uint16_t numCellsAvailable;
  /**<    BS observed data cells available. \n
        - Range -- 1 to 65535 */
}qmiLocEventBsObsDataServiceReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Requests the BS observed location data from the TLE. */
typedef struct {

  /* Optional */
  /*  Number of Cells Requested to Upload */
  uint8_t numCellsUpload_valid;  /**< Must be set to true if numCellsUpload is being passed */
  uint16_t numCellsUpload;
  /**<    - Range -- 1 to 65535 \n
        Defaults to upload all cells when this TLV is not included. */
}qmiLocGetBsObsDataReqMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint64_t qmiLocBsObservedDataValidMaskT_v02;
#define QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_GPSWEEK_V02 ((qmiLocBsObservedDataValidMaskT_v02)0x00000001ull) /**<  GPS week field. \n */
#define QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_GPSMSEC_V02 ((qmiLocBsObservedDataValidMaskT_v02)0x00000002ull) /**<  GPS milliseconds field. \n */
#define QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_LATITUDE_V02 ((qmiLocBsObservedDataValidMaskT_v02)0x00000004ull) /**<  Latitude field. \n */
#define QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_LONGITUDE_V02 ((qmiLocBsObservedDataValidMaskT_v02)0x00000008ull) /**<  Longitude field. \n */
#define QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_HOR_UNC_V02 ((qmiLocBsObservedDataValidMaskT_v02)0x00000010ull) /**<  Horizontal uncertainty field. \n  */
#define QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_ALTITUDE_V02 ((qmiLocBsObservedDataValidMaskT_v02)0x00000020ull) /**<  Altitude field. \n */
#define QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_ALT_UNC_V02 ((qmiLocBsObservedDataValidMaskT_v02)0x00000040ull) /**<  Altitude uncertainty field. \n */
#define QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_SPEED_V02 ((qmiLocBsObservedDataValidMaskT_v02)0x00000080ull) /**<  Speed field. \n */
#define QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_SPEED_UNC_V02 ((qmiLocBsObservedDataValidMaskT_v02)0x00000100ull) /**<  Speed uncertainty field.\n  */
#define QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_HEADING_V02 ((qmiLocBsObservedDataValidMaskT_v02)0x00000200ull) /**<  Heading field. \n */
#define QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_HEADING_UNC_V02 ((qmiLocBsObservedDataValidMaskT_v02)0x00000400ull) /**<  Heading uncertainty field.  */
/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocBsObservedDataValidMaskT_v02 validMask;
  /**<   Bitmask indicating which of the fields in this TLV are valid: \n
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_GPSWEEK (0x00000001) --  GPS week field. \n
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_GPSMSEC (0x00000002) --  GPS milliseconds field. \n
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_LATITUDE (0x00000004) --  Latitude field. \n
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_LONGITUDE (0x00000008) --  Longitude field. \n
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_HOR_UNC (0x00000010) --  Horizontal uncertainty field. \n 
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_ALTITUDE (0x00000020) --  Altitude field. \n
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_ALT_UNC (0x00000040) --  Altitude uncertainty field. \n
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_SPEED (0x00000080) --  Speed field. \n
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_SPEED_UNC (0x00000100) --  Speed uncertainty field.\n 
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_HEADING (0x00000200) --  Heading field. \n
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_HEADING_UNC (0x00000400) --  Heading uncertainty field. 
 */

  qmiLocFdclCellIdStructT_v02 cellId;
  /**<   Cell ID for this record. */

  uint16_t gpsWeek;
  /**<   GPS week. \n
       - Units -- Weeks (since Jan. 1, 1970) */

  uint32_t gpsMilliseconds;
  /**<   GPS milliseconds since Jan. 1, 1970 */

  uint32_t bsLocalTimestamp;
  /**<   Last observed local timestamp for the cell. */

  int32_t posDeltaTime;
  /**<   Delta time between position and cell ID detection. \n
       - Units -- Milliseconds */

  qmiLocBsBestPositionStructT_v02 bsBestPosition;
  /**<   Base station best position. */
}qmiLocBsObservedDataStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Requests the BS observed location data from the TLE. */
typedef struct {

  /* Mandatory */
  /*  Get BS Observed Data Request Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the BS observed data request. \n
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Mandatory */
  /*  Base Station List */
  uint32_t bsData_len;  /**< Must be set to # of elements in bsData */
  qmiLocBsObservedDataStructT_v02 bsData[QMI_LOC_FDCL_BS_LIST_MAX_SIZE_V02];
  /**<   \n A list of base station IDs for FDCL request. */

  /* Optional */
  /*  More BS Data is Available */
  uint8_t moreBsAvailable_valid;  /**< Must be set to true if moreBsAvailable is being passed */
  uint8_t moreBsAvailable;
  /**<   Indicates whether more base station data is available. \n
       - 0x00 (FALSE) -- No more base station data is available. \n
       - 0x01 (TRUE)  -- More base station data is available. \n
       If not specified, moreBsAvailable defaults to FALSE.
  */
}qmiLocGetBsObsDataIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint64_t qmiLocEventReportMaskT_v02;
#define QMI_LOC_DELETE_GPS_EPHEMERIS_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00000001ull) /**<  GPS ephemeris parameters have been deleted. \n  */
#define QMI_LOC_DELETE_GLO_EPHEMERIS_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00000002ull) /**<  GLONASS ephemeris parameters have been deleted. \n */
#define QMI_LOC_DELETE_BDS_EPHEMERIS_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00000004ull) /**<  BDS ephemeris parameters have been deleted. \n */
#define QMI_LOC_DELETE_GAL_EPHEMERIS_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00000008ull) /**<  Galileo ephemeris parameters have been deleted. \n */
#define QMI_LOC_DELETE_QZSS_EPHEMERIS_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00000010ull) /**<  QZSS ephemeris parameters have been deleted. \n */
#define QMI_LOC_DELETE_RESERVED_EPHEMERIS_1_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00000020ull) /**<  Reserved delete ephemeris bit 1.\n  */
#define QMI_LOC_DELETE_RESERVED_EPHEMERIS_2_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00000040ull) /**<  Reserved delete ephemeris bit 2. \n */
#define QMI_LOC_DELETE_RESERVED_EPHEMERIS_3_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00000080ull) /**<  Reserved delete ephemeris bit 3. \n */
#define QMI_LOC_DELETE_RESERVED_EPHEMERIS_4_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00000100ull) /**<  Reserved delete ephemeris bit 4. \n */
#define QMI_LOC_DELETE_RESERVED_EPHEMERIS_5_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00000200ull) /**<  Reserved delete ephemeris bit 5. \n */
#define QMI_LOC_DELETE_RESERVED_EPHEMERIS_6_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00000400ull) /**<  Reserved delete ephemeris bit 6. \n */
#define QMI_LOC_DELETE_RESERVED_EPHEMERIS_7_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00000800ull) /**<  Reserved delete ephemeris bit 7.\n  */
#define QMI_LOC_DELETE_GPS_SV_POLY_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00001000ull) /**<  GPS SV polynomials have been deleted.\n  */
#define QMI_LOC_DELETE_GLO_SV_POLY_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00002000ull) /**<  GLONASS SV polynomials have been deleted. \n  */
#define QMI_LOC_DELETE_BDS_SV_POLY_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00004000ull) /**<  BDS SV polynomials have been deleted. \n */
#define QMI_LOC_DELETE_GAL_SV_POLY_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00008000ull) /**<  Galileo SV polynomials have been deleted. \n */
#define QMI_LOC_DELETE_QZSS_SV_POLY_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00010000ull) /**<  QZSS SV polynomials have been deleted. \n */
#define QMI_LOC_DELETE_RESERVED_SV_POLY_1_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00020000ull) /**<  Reserved delete SV poly bit 1. \n */
#define QMI_LOC_DELETE_RESERVED_SV_POLY_2_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00040000ull) /**<  Reserved delete SV poly bit 2. \n */
#define QMI_LOC_DELETE_RESERVED_SV_POLY_3_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00080000ull) /**<  Reserved delete SV poly bit 3.\n  */
#define QMI_LOC_DELETE_RESERVED_SV_POLY_4_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00100000ull) /**<  Reserved delete SV poly bit 4. \n */
#define QMI_LOC_DELETE_RESERVED_SV_POLY_5_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00200000ull) /**<  Reserved delete SV poly bit 5. \n */
#define QMI_LOC_DELETE_RESERVED_SV_POLY_6_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00400000ull) /**<  Reserved delete SV poly bit 6. \n */
#define QMI_LOC_DELETE_RESERVED_SV_POLY_7_ALL_V02 ((qmiLocEventReportMaskT_v02)0x00800000ull) /**<  Reserved delete SV poly bit 7.  */
/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Sends a report of modem events to the control point. */
typedef struct {

  /* Mandatory */
  /*  Event Report */
  qmiLocEventReportMaskT_v02 eventReport;
  /**<   Bitmask indicating the modem event: \n
      - QMI_LOC_DELETE_GPS_EPHEMERIS_ALL (0x00000001) --  GPS ephemeris parameters have been deleted. \n 
      - QMI_LOC_DELETE_GLO_EPHEMERIS_ALL (0x00000002) --  GLONASS ephemeris parameters have been deleted. \n
      - QMI_LOC_DELETE_BDS_EPHEMERIS_ALL (0x00000004) --  BDS ephemeris parameters have been deleted. \n
      - QMI_LOC_DELETE_GAL_EPHEMERIS_ALL (0x00000008) --  Galileo ephemeris parameters have been deleted. \n
      - QMI_LOC_DELETE_QZSS_EPHEMERIS_ALL (0x00000010) --  QZSS ephemeris parameters have been deleted. \n
      - QMI_LOC_DELETE_RESERVED_EPHEMERIS_1_ALL (0x00000020) --  Reserved delete ephemeris bit 1.\n 
      - QMI_LOC_DELETE_RESERVED_EPHEMERIS_2_ALL (0x00000040) --  Reserved delete ephemeris bit 2. \n
      - QMI_LOC_DELETE_RESERVED_EPHEMERIS_3_ALL (0x00000080) --  Reserved delete ephemeris bit 3. \n
      - QMI_LOC_DELETE_RESERVED_EPHEMERIS_4_ALL (0x00000100) --  Reserved delete ephemeris bit 4. \n
      - QMI_LOC_DELETE_RESERVED_EPHEMERIS_5_ALL (0x00000200) --  Reserved delete ephemeris bit 5. \n
      - QMI_LOC_DELETE_RESERVED_EPHEMERIS_6_ALL (0x00000400) --  Reserved delete ephemeris bit 6. \n
      - QMI_LOC_DELETE_RESERVED_EPHEMERIS_7_ALL (0x00000800) --  Reserved delete ephemeris bit 7.\n 
      - QMI_LOC_DELETE_GPS_SV_POLY_ALL (0x00001000) --  GPS SV polynomials have been deleted.\n 
      - QMI_LOC_DELETE_GLO_SV_POLY_ALL (0x00002000) --  GLONASS SV polynomials have been deleted. \n 
      - QMI_LOC_DELETE_BDS_SV_POLY_ALL (0x00004000) --  BDS SV polynomials have been deleted. \n
      - QMI_LOC_DELETE_GAL_SV_POLY_ALL (0x00008000) --  Galileo SV polynomials have been deleted. \n
      - QMI_LOC_DELETE_QZSS_SV_POLY_ALL (0x00010000) --  QZSS SV polynomials have been deleted. \n
      - QMI_LOC_DELETE_RESERVED_SV_POLY_1_ALL (0x00020000) --  Reserved delete SV poly bit 1. \n
      - QMI_LOC_DELETE_RESERVED_SV_POLY_2_ALL (0x00040000) --  Reserved delete SV poly bit 2. \n
      - QMI_LOC_DELETE_RESERVED_SV_POLY_3_ALL (0x00080000) --  Reserved delete SV poly bit 3.\n 
      - QMI_LOC_DELETE_RESERVED_SV_POLY_4_ALL (0x00100000) --  Reserved delete SV poly bit 4. \n
      - QMI_LOC_DELETE_RESERVED_SV_POLY_5_ALL (0x00200000) --  Reserved delete SV poly bit 5. \n
      - QMI_LOC_DELETE_RESERVED_SV_POLY_6_ALL (0x00400000) --  Reserved delete SV poly bit 6. \n
      - QMI_LOC_DELETE_RESERVED_SV_POLY_7_ALL (0x00800000) --  Reserved delete SV poly bit 7. 
 */
}qmiLocEventReportIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Sends NMEA sentences to the control point. */
typedef struct {

  /* Mandatory */
  /*  NMEA String */
  char nmea[QMI_LOC_NMEA_STRING_MAX_LENGTH_V02 + 1];
  /**<     NULL-terminated string \n
       - Maximum string length (including NULL-terminator) -- 201
       */

  /* Optional */
  /*  Expanded NMEA String */
  uint8_t expandedNmea_valid;  /**< Must be set to true if expandedNmea is being passed */
  char expandedNmea[QMI_LOC_EXPANDED_NMEA_STRING_MAX_LENGTH_V02 + 1];
  /**<   If the service reports expandedNmea, the
       mandatory NMEA string is empty.\n
       - Type -- NULL-terminated string \n
       - Expanded maximum string length (including NULL terminator) -- 4096
       */
}qmiLocEventNmeaIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Obtains the GNSS location request backoff period.  
                    Back-to-back location requests in the modem must be spaced 
                    at an interval that is greater than or equal to this 
                    backoff period.
   */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction; the transaction ID
       is returned in the QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_IND
       indication. */
}qmiLocGetLocationBackoffPeriodReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Obtains the GNSS location request backoff period.  
                    Back-to-back location requests in the modem must be spaced 
                    at an interval that is greater than or equal to this 
                    backoff period.
   */
typedef struct {

  /* Mandatory */
  /*  Get Location Backoff Period Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_REQ request.
 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made  */

  /* Optional */
  /*  Transaction ID */
  uint8_t transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32_t transactionId;
  /**<   Transaction ID that was specified in the get QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_REQ
       request. This parameter is always present
       when the status field is set to SUCCESS. */

  /* Optional */
  /*  Get Location Backoff Period  */
  uint8_t locationBackoffPeriodMs_valid;  /**< Must be set to true if locationBackoffPeriodMs is being passed */
  uint32_t locationBackoffPeriodMs;
  /**<   Back to back location request in the modem must be spaced at 
       an interval that is greater than or equal to this backoff value
       (in milliseconds).
       This parameter is always present if the status field is 
       set to SUCCESS.*/

  /* Optional */
  /*  Distance to Nearest Geofence */
  uint8_t distanceToNearestGeofence_valid;  /**< Must be set to true if distanceToNearestGeofence is being passed */
  uint32_t distanceToNearestGeofence;
  /**<   Distance (in meters) of the UE from the nearest geofence (if present).
       If there are no geofences in the system, this field is not populated.
       */
}qmiLocGetLocationBackoffPeriodIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Used by the control point to query the energy consumed by 
                     the GNSS engine.  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmiLocQueryGNSSEnergyConsumedReqMsgT_v02;

  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to query the energy consumed by 
                     the GNSS engine.  */
typedef struct {

  /* Mandatory */
  /*  Energy Consumed Since Last Boot */
  uint64_t energyConsumedSinceLastBoot;
  /**<   Energy consumed by the GNSS engine since bootup in units of 0.1 milliwatt seconds.
       A value of 0xffffffffffffffff indicates an invalid reading.
       */

  /* Mandatory */
  /*  Energy Consumed Since First Boot */
  uint64_t energyConsumedSinceFirstBoot;
  /**<   Energy consumed by the GNSS engine since the first bootup in units of 0.1 milliwatt seconds.
       A value of 0xffffffffffffffff indicates an invalid reading.
       */
}qmiLocQueryGNSSEnergyConsumedIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCSVSYSTEMENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SV_SYSTEM_GPS_V02 = 1, /**<  GPS satellite \n */
  eQMI_LOC_SV_SYSTEM_GALILEO_V02 = 2, /**<  Galileo satellite \n */
  eQMI_LOC_SV_SYSTEM_SBAS_V02 = 3, /**<  SBAS satellite \n */
  eQMI_LOC_SV_SYSTEM_COMPASS_V02 = 4, /**<  COMPASS satellite (Deprecated) \n */
  eQMI_LOC_SV_SYSTEM_GLONASS_V02 = 5, /**<  GLONASS satellite \n */
  eQMI_LOC_SV_SYSTEM_BDS_V02 = 6, /**<  BDS satellite \n */
  eQMI_LOC_SV_SYSTEM_QZSS_V02 = 7, /**<  QZSS satellite  */
  QMILOCSVSYSTEMENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSvSystemEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCSVSTATUSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SV_STATUS_IDLE_V02 = 1, /**<  SV is not being actively processed \n  */
  eQMI_LOC_SV_STATUS_SEARCH_V02 = 2, /**<  System is searching for this SV \n */
  eQMI_LOC_SV_STATUS_TRACK_V02 = 3, /**<  SV is being tracked  */
  QMILOCSVSTATUSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSvStatusEnumT_v02;
/**
    @}
  */

typedef uint64_t qmiLocSvInfoValidMaskT_v02;
#define QMI_LOC_SV_INFO_MASK_VALID_SYSTEM_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000001ull) /**<  System field is valid in SV information \n  */
#define QMI_LOC_SV_INFO_MASK_VALID_GNSS_SVID_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000002ull) /**<  gnssSvId field is valid in SV information \n  */
#define QMI_LOC_SV_INFO_MASK_VALID_HEALTH_STATUS_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000004ull) /**<  healthStatus field is valid in SV information \n  */
#define QMI_LOC_SV_INFO_MASK_VALID_PROCESS_STATUS_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000008ull) /**<  processStatus field is valid in SV information \n  */
#define QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000010ull) /**<  svInfoMask field is valid in SV information \n  */
#define QMI_LOC_SV_INFO_MASK_VALID_ELEVATION_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000020ull) /**<  Elevation field is valid in SV information \n  */
#define QMI_LOC_SV_INFO_MASK_VALID_AZIMUTH_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000040ull) /**<  Azimuth field is valid in SV information \n  */
#define QMI_LOC_SV_INFO_MASK_VALID_SNR_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000080ull) /**<  SNR field is valid in SV information  */
typedef uint64_t qmiLocSvInfoMaskT_v02;
#define QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02 ((qmiLocSvInfoMaskT_v02)0x01ull) /**<  Ephemeris is available for this SV \n  */
#define QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02 ((qmiLocSvInfoMaskT_v02)0x02ull) /**<  Almanac is available for this SV  */
/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocSvInfoValidMaskT_v02 validMask;
  /**<   Bitmask indicating which of the fields in this TLV are valid: \n
      - QMI_LOC_SV_INFO_MASK_VALID_SYSTEM (0x00000001) --  System field is valid in SV information \n 
      - QMI_LOC_SV_INFO_MASK_VALID_GNSS_SVID (0x00000002) --  gnssSvId field is valid in SV information \n 
      - QMI_LOC_SV_INFO_MASK_VALID_HEALTH_STATUS (0x00000004) --  healthStatus field is valid in SV information \n 
      - QMI_LOC_SV_INFO_MASK_VALID_PROCESS_STATUS (0x00000008) --  processStatus field is valid in SV information \n 
      - QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK (0x00000010) --  svInfoMask field is valid in SV information \n 
      - QMI_LOC_SV_INFO_MASK_VALID_ELEVATION (0x00000020) --  Elevation field is valid in SV information \n 
      - QMI_LOC_SV_INFO_MASK_VALID_AZIMUTH (0x00000040) --  Azimuth field is valid in SV information \n 
      - QMI_LOC_SV_INFO_MASK_VALID_SNR (0x00000080) --  SNR field is valid in SV information 
 */

  qmiLocSvSystemEnumT_v02 system;
  /**<   Indicates to which constellation this SV belongs.
 Valid values: \n
      - eQMI_LOC_SV_SYSTEM_GPS (1) --  GPS satellite \n
      - eQMI_LOC_SV_SYSTEM_GALILEO (2) --  Galileo satellite \n
      - eQMI_LOC_SV_SYSTEM_SBAS (3) --  SBAS satellite \n
      - eQMI_LOC_SV_SYSTEM_COMPASS (4) --  COMPASS satellite (Deprecated) \n
      - eQMI_LOC_SV_SYSTEM_GLONASS (5) --  GLONASS satellite \n
      - eQMI_LOC_SV_SYSTEM_BDS (6) --  BDS satellite \n
      - eQMI_LOC_SV_SYSTEM_QZSS (7) --  QZSS satellite 
 */

  uint16_t gnssSvId;
  /**<   GNSS SV ID.
         Range:  \n
           - GPS --      1 to 32 \n
           - GLONASS --  1 to 32 \n
           - SBAS --     120 to 158 and 183 to 191 \n
           - QZSS --     193 to 197 \n
           - BDS --      201 to 237 \n
           - Galileo --  301 to 336 \n
           - NavIC --    401 to 414
       */

  uint8_t healthStatus;
  /**<   Health status. 
         Values: \n
         - 0 -- unhealthy \n
         - 1 -- healthy
       */

  qmiLocSvStatusEnumT_v02 svStatus;
  /**<   SV processing status.
 Valid values: \n
      - eQMI_LOC_SV_STATUS_IDLE (1) --  SV is not being actively processed \n 
      - eQMI_LOC_SV_STATUS_SEARCH (2) --  System is searching for this SV \n
      - eQMI_LOC_SV_STATUS_TRACK (3) --  SV is being tracked 
 */

  qmiLocSvInfoMaskT_v02 svInfoMask;
  /**<   Indicates whether almanac and ephemeris information is available. \n
 Valid bitmasks: \n
      - QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS (0x01) --  Ephemeris is available for this SV \n 
      - QMI_LOC_SVINFO_MASK_HAS_ALMANAC (0x02) --  Almanac is available for this SV 
 */

  float elevation;
  /**<   SV elevation angle.\n
         - Units -- Degrees \n
         - Range -- 0 to 90 */

  float azimuth;
  /**<   SV azimuth angle.\n
         - Units -- Degrees \n
         - Range -- 0 to 360 */

  float snr;
  /**<   SV signal-to-noise ratio at antenna. \n
         - Units -- dB-Hz */

  qmiLocGnssSignalTypeMaskT_v02 gnssSignalType;
  /**<   Indicates the signal type of satellite. 
 The value 0 means invalid.
 Valid bitmasks: \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L1CA (0x00000001) --  GPS L1CA RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L1C (0x00000002) --  GPS L1C RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L2C_L (0x00000004) --  GPS L2C_L RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L5_Q (0x00000008) --  GPS L5_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GLONASS_G1 (0x00000010) --  GLONASS G1 (L1OF) RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GLONASS_G2 (0x00000020) --  GLONASS G2 (L2OF) RF band\n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E1_C (0x00000040) --  Galileo E1_C RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E5A_Q (0x00000080) --  Galileo E5A_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E5B_Q (0x00000100) --  Galileo E5B_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B1_I (0x00000200) --  BeiDou B1_I RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B1C (0x00000400) --  BeiDou B1C RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B2_I (0x00000800) --  BeiDou B2_I RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B2A_I (0x00001000) --  BeiDou B2A_I RF band \n 
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L1CA (0x00002000) --  QZSS L1CA RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L1S (0x00004000) --  QZSS L1S RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L2C_L (0x00008000) --  QZSS L2C_L RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L5_Q (0x00010000) --  QZSS L5_Q RF band \n
      - QMI_LOC_MASK_GNSS_SIGNAL_TYPE_SBAS_L1_CA (0x00020000) --  SBAS L1_CA RF band 
 */
}qmiLocSvInfoStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Sends a satellite report to the control point. */
typedef struct {

  /* Mandatory */
  /*  Altitude Assumed */
  uint8_t altitudeAssumed;
  /**<   Indicates whether altitude is assumed or calculated: \n
         - 0x00 (FALSE) -- Calculated \n
         - 0x01 (TRUE) -- Assumed; there might not be
                          enough satellites to determine the precise altitude
      */

  /* Optional */
  /*  Satellite Information */
  uint8_t svList_valid;  /**< Must be set to true if svList is being passed */
  uint32_t svList_len;  /**< Must be set to # of elements in svList */
  qmiLocSvInfoStructT_v02 svList[QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02];
  /**<   \n SV information list. */
}qmiLocEventGnssSvInfoIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Used by the control point to query WWAN data from the modem.  */
typedef struct {

  /* Mandatory */
  /*  Number of Cells */
  uint16_t numOfCells;
  /**<   Number of cells. */

  /* Mandatory */
  /*  Assisted */
  uint8_t isAssisted;
  /**<   Is assisted. */

  /* Mandatory */
  /*  Air Interface  */
  qmiLocFdclAirInterfaceTypeEnumT_v02 airInterface;
  /**<   Air interface. */

  /* Optional */
  /*  Request Type */
  uint8_t reqType_valid;  /**< Must be set to true if reqType is being passed */
  uint16_t reqType;
  /**<   Request Type*/
}qmiLocGetWwanDataReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to query WWAN data from the modem.  */
typedef struct {

  /* Mandatory */
  /*  Success/Failure Status of QMI Request */
  qmiLocStatusEnumT_v02 status;
  /**<   Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  More BS Data is Available */
  uint8_t moreBsAvailable_valid;  /**< Must be set to true if moreBsAvailable is being passed */
  uint8_t moreBsAvailable;
  /**<   Indicates whether more base station data is available. \n
       - 0x00 (FALSE) -- No more base station data is available. \n
       - 0x01 (TRUE)  -- More base station data is available. \n
       If not specified, moreBsAvailable defaults to FALSE.
  */

  /* Optional */
  /*  Data Buffer  */
  uint8_t dataBuffer_valid;  /**< Must be set to true if dataBuffer is being passed */
  uint32_t dataBuffer_len;  /**< Must be set to # of elements in dataBuffer */
  uint8_t dataBuffer[QMI_LOC_DATA_BUFF_MAX_SIZE_V02];
  /**<   dataBuffer. */

  /* Optional */
  /*  Request Type */
  uint8_t reqType_valid;  /**< Must be set to true if reqType is being passed */
  uint16_t reqType;
  /**<   request Type. */

  /* Optional */
  /*  Request Status */
  uint8_t reqStatus_valid;  /**< Must be set to true if reqStatus is being passed */
  uint16_t reqStatus;
  /**<   Request Status */
}qmiLocGetWwanDataIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Request Message; Configures the robust location. */
typedef struct {

  /* Mandatory */
  /*  Enable */
  uint8_t enable;
  /**<   Specifies the robust location configuration. \n 
       - 0x00 (FALSE) -- Disable \n
       - 0x01 (TRUE)  -- Enable  
  */

  /* Optional */
  /*  Enable for E911 */
  uint8_t enableForE911_valid;  /**< Must be set to true if enableForE911 is being passed */
  uint8_t enableForE911;
  /**<    Specifies the robust location configuration during e911 state. \n 
       - 0x00 (FALSE) -- Disable \n 
       - 0x01 (TRUE)  -- Enable   
  */
}qmiLocSetRobustLocationReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t major;
  /**<   Major version number. */

  uint16_t minor;
  /**<   Minor version number. */
}qmiLocRobustLocationVersionT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Queries the robust location configuration. */
typedef struct {

  /* Mandatory */
  /*  Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the request.
 Values: \n
      - eQMI_LOC_SUCCESS (0) --  Completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Failed because it has timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Failed because the maximum number of geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure \n
      - eQMI_LOC_GNSS_DISABLED (11) --  Failed because the location service is disabled \n 
      - eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE (12) --  Failed because the GNSS engine was not loaded when the request was made 
 */

  /* Optional */
  /*  Enable */
  uint8_t isEnabled_valid;  /**< Must be set to true if isEnabled is being passed */
  uint8_t isEnabled;
  /**<   Configuration of robust location.  \n
       - 0x00 (FALSE) -- Disabled  \n
       - 0x01 (TRUE)  -- Enabled   
  */

  /* Optional */
  /*  Enable for E911 */
  uint8_t isEnabledForE911_valid;  /**< Must be set to true if isEnabledForE911 is being passed */
  uint8_t isEnabledForE911;
  /**<   Configuration of robust location during e911 state. \n
       - 0x00 (FALSE) -- Disabled \n
       - 0x01 (TRUE)  -- Enabled  
  */

  /* Optional */
  /*  Robust Location Version */
  uint8_t robustLocationVersion_valid;  /**< Must be set to true if robustLocationVersion is being passed */
  qmiLocRobustLocationVersionT_v02 robustLocationVersion;
  /**<   \n Major and minor version of the supported robust location feature. */
}qmiLocGetRobustLocationConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCREQUESTEDPOSTYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_REQUESTED_POS_TYPE_GNSS_V02 = 1, /**<   GNSS position requested \n */
  QMILOCREQUESTEDPOSTYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocRequestedPosTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_enums
    @{
  */
typedef enum {
  QMILOCPOSREQUESTERENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_POS_REQUESTER_GEOFENCE_V02 = 1, /**<   Geofence requested the position \n    */
  QMILOCPOSREQUESTERENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocPosRequesterEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_iot_qmi_messages
    @{
  */
/** Indication Message; Request the control point to initiate a position request. */
typedef struct {

  /* Optional */
  /*  Requested Position Source */
  uint8_t posRequester_valid;  /**< Must be set to true if posRequester is being passed */
  qmiLocPosRequesterEnumT_v02 posRequester;
  /**<   Client who is requesting the fix \n
 Valid values: \n
      - eQMI_LOC_POS_REQUESTER_GEOFENCE (1) --   Geofence requested the position \n   
 */

  /* Optional */
  /*  Requested Position Type */
  uint8_t requestedPosType_valid;  /**< Must be set to true if requestedPosType is being passed */
  qmiLocRequestedPosTypeEnumT_v02 requestedPosType;
  /**<   Type of position requested \n
 Valid values: \n
      - eQMI_LOC_REQUESTED_POS_TYPE_GNSS (1) --   GNSS position requested \n
 */

  /* Optional */
  /*  Horizontal Accuracy Level */
  uint8_t horizontalAccuracyLevel_valid;  /**< Must be set to true if horizontalAccuracyLevel is being passed */
  qmiLocAccuracyLevelEnumT_v02 horizontalAccuracyLevel;
  /**<   Horizontal accuracy level required.
 If not specified, accuracy defaults to low.\n
 Valid values: \n
      - eQMI_LOC_ACCURACY_LOW (1) --  Low accuracy \n
      - eQMI_LOC_ACCURACY_MED (2) --  Medium accuracy \n
      - eQMI_LOC_ACCURACY_HIGH (3) --  High accuracy 
 */

  /* Optional */
  /*  Maximum Wait Time to Get a Position Report */
  uint8_t positionReportTimeout_valid;  /**< Must be set to true if positionReportTimeout is being passed */
  uint32_t positionReportTimeout;
  /**<   Maximum time to work on each fix.
       The GPS engine returns QMI_ERR_INTERNAL if a position cannot be obtained
       within the positionReportTimeout value. \n
       - Units -- Milliseconds \n
       - Default -- 255*1000 ms \n
       - Range -- 1000 - 255*1000 ms
  */

  /* Optional */
  /*  Minimum Interval Between Final Position Reports */
  uint8_t minInterval_valid;  /**< Must be set to true if minInterval is being passed */
  uint32_t minInterval;
  /**<   Minimum time interval, that must elapse between final position reports. \n
       - Units -- Milliseconds \n
       - Default -- 1000 ms
  */

  /* Optional */
  /*  Horizontal Accuracy */
  uint8_t horizontalAccuracy_valid;  /**< Must be set to true if horizontalAccuracy is being passed */
  uint16_t horizontalAccuracy;
  /**<   Horizontal accuracy required by the control point. \n
        - Range -- 0 to 65535 \n
        - Units -- Meters   */
}qmiLocEventInitiatePosReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_LOC_ADD_CIRCULAR_GEOFENCE_V02
//#define REMOVE_QMI_LOC_CANCEL_APS_DATA_V02
//#define REMOVE_QMI_LOC_DELETE_GEOFENCE_V02
//#define REMOVE_QMI_LOC_DELETE_GNSS_SERVICE_DATA_V02
//#define REMOVE_QMI_LOC_ENABLE_POSITION_INJECTION_TEST_MODE_V02
//#define REMOVE_QMI_LOC_EVENT_BATCHING_STATUS_V02
//#define REMOVE_QMI_LOC_EVENT_BATCH_FULL_NOTIFICATION_V02
//#define REMOVE_QMI_LOC_EVENT_BS_OBS_DATA_SERVICE_REQ_V02
//#define REMOVE_QMI_LOC_EVENT_DBT_POSITION_REPORT_V02
//#define REMOVE_QMI_LOC_EVENT_DBT_SESSION_STATUS_V02
//#define REMOVE_QMI_LOC_EVENT_ENGINE_STATE_V02
//#define REMOVE_QMI_LOC_EVENT_FDCL_SERVICE_REQ_V02
//#define REMOVE_QMI_LOC_EVENT_FIX_SESSION_STATE_V02
//#define REMOVE_QMI_LOC_EVENT_GEOFENCE_BREACH_NOTIFICATION_V02
//#define REMOVE_QMI_LOC_EVENT_GEOFENCE_GEN_ALERT_V02
//#define REMOVE_QMI_LOC_EVENT_GNSS_SV_INFO_V02
//#define REMOVE_QMI_LOC_EVENT_INITIATE_POS_REQ_V02
//#define REMOVE_QMI_LOC_EVENT_INJECT_PREDICTED_ORBITS_REQ_V02
//#define REMOVE_QMI_LOC_EVENT_INJECT_TIME_REQ_V02
//#define REMOVE_QMI_LOC_EVENT_NMEA_V02
//#define REMOVE_QMI_LOC_EVENT_POSITION_REPORT_V02
//#define REMOVE_QMI_LOC_EVENT_REPORT_V02
//#define REMOVE_QMI_LOC_GET_APS_DATA_V02
//#define REMOVE_QMI_LOC_GET_AVAILABLE_WWAN_POSITION_V02
//#define REMOVE_QMI_LOC_GET_BATCH_SIZE_V02
//#define REMOVE_QMI_LOC_GET_BEST_AVAILABLE_POSITION_V02
//#define REMOVE_QMI_LOC_GET_BS_OBS_DATA_V02
//#define REMOVE_QMI_LOC_GET_CONSTELLATION_CONTROL_V02
//#define REMOVE_QMI_LOC_GET_FDCL_BS_LIST_V02
//#define REMOVE_QMI_LOC_GET_FIX_CRITERIA_V02
//#define REMOVE_QMI_LOC_GET_GEOFENCE_ENGINE_CONFIG_V02
//#define REMOVE_QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_V02
//#define REMOVE_QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_V02
//#define REMOVE_QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_V02
//#define REMOVE_QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_V02
//#define REMOVE_QMI_LOC_GET_REGISTERED_EVENTS_V02
//#define REMOVE_QMI_LOC_GET_ROBUST_LOCATION_CONFIG_V02
//#define REMOVE_QMI_LOC_GET_SERVICE_REVISION_V02
//#define REMOVE_QMI_LOC_GET_SUPPORTED_FEATURE_V02
//#define REMOVE_QMI_LOC_GET_WWAN_DATA_V02
//#define REMOVE_QMI_LOC_GET_XTRA_T_SESSION_CONTROL_V02
//#define REMOVE_QMI_LOC_INFORM_CLIENT_REVISION_V02
//#define REMOVE_QMI_LOC_INJECT_FDCL_DATA_V02
//#define REMOVE_QMI_LOC_INJECT_GSM_CELL_INFO_V02
//#define REMOVE_QMI_LOC_INJECT_GTP_CLIENT_DOWNLOADED_DATA_V02
//#define REMOVE_QMI_LOC_INJECT_POSITION_V02
//#define REMOVE_QMI_LOC_INJECT_TEST_POSITION_V02
//#define REMOVE_QMI_LOC_INJECT_UTC_TIME_V02
//#define REMOVE_QMI_LOC_INJECT_XTRA_DATA_V02
//#define REMOVE_QMI_LOC_INJECT_XTRA_PCID_V02
//#define REMOVE_QMI_LOC_QUERY_AON_CONFIG_V02
//#define REMOVE_QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_V02
//#define REMOVE_QMI_LOC_QUERY_OTB_ACCUMULATED_DISTANCE_V02
//#define REMOVE_QMI_LOC_QUERY_XTRA_INFO_V02
//#define REMOVE_QMI_LOC_READ_FROM_BATCH_V02
//#define REMOVE_QMI_LOC_REG_EVENTS_V02
//#define REMOVE_QMI_LOC_RELEASE_BATCH_V02
//#define REMOVE_QMI_LOC_SET_CONSTELLATION_CONTROL_V02
//#define REMOVE_QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_V02
//#define REMOVE_QMI_LOC_SET_GNSS_CONSTELL_REPORT_CONFIG_V02
//#define REMOVE_QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_V02
//#define REMOVE_QMI_LOC_SET_PREMIUM_SERVICES_CONFIG_V02
//#define REMOVE_QMI_LOC_SET_ROBUST_LOCATION_CONFIG_V02
//#define REMOVE_QMI_LOC_SET_XTRA_T_SESSION_CONTROL_V02
//#define REMOVE_QMI_LOC_SET_XTRA_VERSION_CHECK_V02
//#define REMOVE_QMI_LOC_START_V02
//#define REMOVE_QMI_LOC_START_BATCHING_V02
//#define REMOVE_QMI_LOC_START_DBT_V02
//#define REMOVE_QMI_LOC_START_LISTEN_POSITION_V02
//#define REMOVE_QMI_LOC_START_OUTDOOR_TRIP_BATCHING_V02
//#define REMOVE_QMI_LOC_STOP_V02
//#define REMOVE_QMI_LOC_STOP_BATCHING_V02
//#define REMOVE_QMI_LOC_STOP_DBT_V02
//#define REMOVE_QMI_LOC_STOP_LISTEN_POSITION_V02
//#define REMOVE_QMI_LOC_SYSTEM_INFO_V02

/*Service Message Definition*/
/** @addtogroup loc_iot_qmi_msg_ids
    @{
  */
#define QMI_LOC_INFORM_CLIENT_REVISION_REQ_V02 0x0020
#define QMI_LOC_INFORM_CLIENT_REVISION_RESP_V02 0x0020
#define QMI_LOC_REG_EVENTS_REQ_V02 0x0021
#define QMI_LOC_REG_EVENTS_RESP_V02 0x0021
#define QMI_LOC_START_REQ_V02 0x0022
#define QMI_LOC_START_RESP_V02 0x0022
#define QMI_LOC_STOP_REQ_V02 0x0023
#define QMI_LOC_STOP_RESP_V02 0x0023
#define QMI_LOC_EVENT_POSITION_REPORT_IND_V02 0x0024
#define QMI_LOC_EVENT_INJECT_TIME_REQ_IND_V02 0x0025
#define QMI_LOC_EVENT_INJECT_PREDICTED_ORBITS_REQ_IND_V02 0x0026
#define QMI_LOC_EVENT_ENGINE_STATE_IND_V02 0x0027
#define QMI_LOC_EVENT_FIX_SESSION_STATE_IND_V02 0x0028
#define QMI_LOC_GET_SERVICE_REVISION_REQ_V02 0x0029
#define QMI_LOC_GET_SERVICE_REVISION_RESP_V02 0x0029
#define QMI_LOC_GET_SERVICE_REVISION_IND_V02 0x0029
#define QMI_LOC_GET_FIX_CRITERIA_REQ_V02 0x002A
#define QMI_LOC_GET_FIX_CRITERIA_RESP_V02 0x002A
#define QMI_LOC_GET_FIX_CRITERIA_IND_V02 0x002A
#define QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02 0x002B
#define QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_RESP_V02 0x002B
#define QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_IND_V02 0x002B
#define QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_REQ_V02 0x002C
#define QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_RESP_V02 0x002C
#define QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_IND_V02 0x002C
#define QMI_LOC_INJECT_UTC_TIME_REQ_V02 0x002D
#define QMI_LOC_INJECT_UTC_TIME_RESP_V02 0x002D
#define QMI_LOC_INJECT_UTC_TIME_IND_V02 0x002D
#define QMI_LOC_INJECT_POSITION_REQ_V02 0x002E
#define QMI_LOC_INJECT_POSITION_RESP_V02 0x002E
#define QMI_LOC_INJECT_POSITION_IND_V02 0x002E
#define QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ_V02 0x002F
#define QMI_LOC_SET_XTRA_T_SESSION_CONTROL_RESP_V02 0x002F
#define QMI_LOC_SET_XTRA_T_SESSION_CONTROL_IND_V02 0x002F
#define QMI_LOC_GET_XTRA_T_SESSION_CONTROL_REQ_V02 0x0030
#define QMI_LOC_GET_XTRA_T_SESSION_CONTROL_RESP_V02 0x0030
#define QMI_LOC_GET_XTRA_T_SESSION_CONTROL_IND_V02 0x0030
#define QMI_LOC_GET_REGISTERED_EVENTS_REQ_V02 0x0031
#define QMI_LOC_GET_REGISTERED_EVENTS_RESP_V02 0x0031
#define QMI_LOC_GET_REGISTERED_EVENTS_IND_V02 0x0031
#define QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02 0x0032
#define QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_RESP_V02 0x0032
#define QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02 0x0032
#define QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02 0x0033
#define QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_RESP_V02 0x0033
#define QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02 0x0033
#define QMI_LOC_EVENT_GEOFENCE_GEN_ALERT_IND_V02 0x0034
#define QMI_LOC_EVENT_GEOFENCE_BREACH_NOTIFICATION_IND_V02 0x0035
#define QMI_LOC_ADD_CIRCULAR_GEOFENCE_REQ_V02 0x0036
#define QMI_LOC_ADD_CIRCULAR_GEOFENCE_RESP_V02 0x0036
#define QMI_LOC_ADD_CIRCULAR_GEOFENCE_IND_V02 0x0036
#define QMI_LOC_DELETE_GEOFENCE_REQ_V02 0x0037
#define QMI_LOC_DELETE_GEOFENCE_RESP_V02 0x0037
#define QMI_LOC_DELETE_GEOFENCE_IND_V02 0x0037
#define QMI_LOC_GET_BEST_AVAILABLE_POSITION_REQ_V02 0x0038
#define QMI_LOC_GET_BEST_AVAILABLE_POSITION_RESP_V02 0x0038
#define QMI_LOC_GET_BEST_AVAILABLE_POSITION_IND_V02 0x0038
#define QMI_LOC_INJECT_GSM_CELL_INFO_REQ_V02 0x0039
#define QMI_LOC_INJECT_GSM_CELL_INFO_RESP_V02 0x0039
#define QMI_LOC_INJECT_GSM_CELL_INFO_IND_V02 0x0039
#define QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_REQ_V02 0x003A
#define QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_RESP_V02 0x003A
#define QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_IND_V02 0x003A
#define QMI_LOC_GET_GEOFENCE_ENGINE_CONFIG_REQ_V02 0x003B
#define QMI_LOC_GET_GEOFENCE_ENGINE_CONFIG_RESP_V02 0x003B
#define QMI_LOC_GET_GEOFENCE_ENGINE_CONFIG_IND_V02 0x003B
#define QMI_LOC_GET_BATCH_SIZE_REQ_V02 0x003C
#define QMI_LOC_GET_BATCH_SIZE_RESP_V02 0x003C
#define QMI_LOC_GET_BATCH_SIZE_IND_V02 0x003C
#define QMI_LOC_START_BATCHING_REQ_V02 0x003D
#define QMI_LOC_START_BATCHING_RESP_V02 0x003D
#define QMI_LOC_START_BATCHING_IND_V02 0x003D
#define QMI_LOC_EVENT_BATCH_FULL_NOTIFICATION_IND_V02 0x003E
#define QMI_LOC_READ_FROM_BATCH_REQ_V02 0x003F
#define QMI_LOC_READ_FROM_BATCH_RESP_V02 0x003F
#define QMI_LOC_READ_FROM_BATCH_IND_V02 0x003F
#define QMI_LOC_STOP_BATCHING_REQ_V02 0x0040
#define QMI_LOC_STOP_BATCHING_RESP_V02 0x0040
#define QMI_LOC_STOP_BATCHING_IND_V02 0x0040
#define QMI_LOC_RELEASE_BATCH_REQ_V02 0x0041
#define QMI_LOC_RELEASE_BATCH_RESP_V02 0x0041
#define QMI_LOC_RELEASE_BATCH_IND_V02 0x0041
#define QMI_LOC_GET_AVAILABLE_WWAN_POSITION_REQ_V02 0x0042
#define QMI_LOC_GET_AVAILABLE_WWAN_POSITION_RESP_V02 0x0042
#define QMI_LOC_GET_AVAILABLE_WWAN_POSITION_IND_V02 0x0042
#define QMI_LOC_SET_PREMIUM_SERVICES_CONFIG_REQ_V02 0x0043
#define QMI_LOC_SET_PREMIUM_SERVICES_CONFIG_RESP_V02 0x0043
#define QMI_LOC_SET_PREMIUM_SERVICES_CONFIG_IND_V02 0x0043
#define QMI_LOC_SET_XTRA_VERSION_CHECK_REQ_V02 0x0044
#define QMI_LOC_SET_XTRA_VERSION_CHECK_RESP_V02 0x0044
#define QMI_LOC_SET_XTRA_VERSION_CHECK_IND_V02 0x0044
#define QMI_LOC_SET_GNSS_CONSTELL_REPORT_CONFIG_V02 0x0045
#define QMI_LOC_SET_GNSS_CONSTELL_REPORT_CONFIG_RESP_V02 0x0045
#define QMI_LOC_SET_GNSS_CONSTELL_REPORT_CONFIG_IND_V02 0x0045
#define QMI_LOC_INJECT_GTP_CLIENT_DOWNLOADED_DATA_REQ_V02 0x0046
#define QMI_LOC_INJECT_GTP_CLIENT_DOWNLOADED_DATA_RESP_V02 0x0046
#define QMI_LOC_INJECT_GTP_CLIENT_DOWNLOADED_DATA_IND_V02 0x0046
#define QMI_LOC_START_DBT_REQ_V02 0x0047
#define QMI_LOC_START_DBT_RESP_V02 0x0047
#define QMI_LOC_START_DBT_IND_V02 0x0047
#define QMI_LOC_EVENT_DBT_POSITION_REPORT_IND_V02 0x0048
#define QMI_LOC_EVENT_DBT_SESSION_STATUS_IND_V02 0x0049
#define QMI_LOC_STOP_DBT_REQ_V02 0x004A
#define QMI_LOC_STOP_DBT_RESP_V02 0x004A
#define QMI_LOC_STOP_DBT_IND_V02 0x004A
#define QMI_LOC_EVENT_BATCHING_STATUS_IND_V02 0x004B
#define QMI_LOC_QUERY_AON_CONFIG_REQ_V02 0x004C
#define QMI_LOC_QUERY_AON_CONFIG_RESP_V02 0x004C
#define QMI_LOC_QUERY_AON_CONFIG_IND_V02 0x004C
#define QMI_LOC_DELETE_GNSS_SERVICE_DATA_REQ_V02 0x004D
#define QMI_LOC_DELETE_GNSS_SERVICE_DATA_RESP_V02 0x004D
#define QMI_LOC_DELETE_GNSS_SERVICE_DATA_IND_V02 0x004D
#define QMI_LOC_INJECT_XTRA_DATA_REQ_V02 0x004E
#define QMI_LOC_INJECT_XTRA_DATA_RESP_V02 0x004E
#define QMI_LOC_INJECT_XTRA_DATA_IND_V02 0x004E
#define QMI_LOC_INJECT_XTRA_PCID_REQ_V02 0x004F
#define QMI_LOC_INJECT_XTRA_PCID_RESP_V02 0x004F
#define QMI_LOC_INJECT_XTRA_PCID_IND_V02 0x004F
#define QMI_LOC_GET_SUPPORTED_FEATURE_REQ_V02 0x0050
#define QMI_LOC_GET_SUPPORTED_FEATURE_RESP_V02 0x0050
#define QMI_LOC_GET_SUPPORTED_FEATURE_IND_V02 0x0050
#define QMI_LOC_QUERY_XTRA_INFO_REQ_V02 0x0051
#define QMI_LOC_QUERY_XTRA_INFO_RESP_V02 0x0051
#define QMI_LOC_QUERY_XTRA_INFO_IND_V02 0x0051
#define QMI_LOC_START_OUTDOOR_TRIP_BATCHING_REQ_V02 0x0052
#define QMI_LOC_START_OUTDOOR_TRIP_BATCHING_RESP_V02 0x0052
#define QMI_LOC_START_OUTDOOR_TRIP_BATCHING_IND_V02 0x0052
#define QMI_LOC_QUERY_OTB_ACCUMULATED_DISTANCE_REQ_V02 0x0053
#define QMI_LOC_QUERY_OTB_ACCUMULATED_DISTANCE_RESP_V02 0x0053
#define QMI_LOC_QUERY_OTB_ACCUMULATED_DISTANCE_IND_V02 0x0053
#define QMI_LOC_EVENT_FDCL_SERVICE_REQ_IND_V02 0x0054
#define QMI_LOC_GET_FDCL_BS_LIST_REQ_V02 0x0055
#define QMI_LOC_GET_FDCL_BS_LIST_RESP_V02 0x0055
#define QMI_LOC_GET_FDCL_BS_LIST_IND_V02 0x0055
#define QMI_LOC_INJECT_FDCL_DATA_REQ_V02 0x0056
#define QMI_LOC_INJECT_FDCL_DATA_RESP_V02 0x0056
#define QMI_LOC_INJECT_FDCL_DATA_IND_V02 0x0056
#define QMI_LOC_START_LISTEN_POSITION_REQ_V02 0x0057
#define QMI_LOC_START_LISTEN_POSITION_RESP_V02 0x0057
#define QMI_LOC_STOP_LISTEN_POSITION_REQ_V02 0x0058
#define QMI_LOC_STOP_LISTEN_POSITION_RESP_V02 0x0058
#define QMI_LOC_SYSTEM_INFO_REQ_V02 0x0059
#define QMI_LOC_SYSTEM_INFO_RESP_V02 0x0059
#define QMI_LOC_SYSTEM_INFO_IND_V02 0x0059
#define QMI_LOC_GET_CONSTELLATION_CONTROL_REQ_V02 0x005A
#define QMI_LOC_GET_CONSTELLATION_CONTROL_RESP_V02 0x005A
#define QMI_LOC_GET_CONSTELLATION_CONTROL_IND_V02 0x005A
#define QMI_LOC_ENABLE_POSITION_INJECTION_TEST_MODE_REQ_V02 0x005B
#define QMI_LOC_ENABLE_POSITION_INJECTION_TEST_MODE_RESP_V02 0x005B
#define QMI_LOC_ENABLE_POSITION_INJECTION_TEST_MODE_IND_V02 0x005B
#define QMI_LOC_INJECT_TEST_POSITION_REQ_V02 0x005C
#define QMI_LOC_INJECT_TEST_POSITION_RESP_V02 0x005C
#define QMI_LOC_INJECT_TEST_POSITION_IND_V02 0x005C
#define QMI_LOC_EVENT_BS_OBS_DATA_SERVICE_REQ_IND_V02 0x005D
#define QMI_LOC_GET_BS_OBS_DATA_REQ_V02 0x005E
#define QMI_LOC_GET_BS_OBS_DATA_RESP_V02 0x005E
#define QMI_LOC_GET_BS_OBS_DATA_IND_V02 0x005E
#define QMI_LOC_EVENT_REPORT_IND_V02 0x005F
#define QMI_LOC_EVENT_NMEA_IND_V02 0x0060
#define QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_REQ_V02 0x0061
#define QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_RESP_V02 0x0061
#define QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_IND_V02 0x0061
#define QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_REQ_V02 0x0062
#define QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_RESP_V02 0x0062
#define QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_IND_V02 0x0062
#define QMI_LOC_EVENT_GNSS_SV_INFO_IND_V02 0x0063
#define QMI_LOC_GET_WWAN_DATA_REQ_V02 0x0064
#define QMI_LOC_GET_WWAN_DATA_RESP_V02 0x0064
#define QMI_LOC_GET_WWAN_DATA_IND_V02 0x0064
#define QMI_LOC_SET_CONSTELLATION_CONTROL_REQ_V02 0x0065
#define QMI_LOC_SET_CONSTELLATION_CONTROL_RESP_V02 0x0065
#define QMI_LOC_SET_CONSTELLATION_CONTROL_IND_V02 0x0065
#define QMI_LOC_SET_ROBUST_LOCATION_CONFIG_REQ_V02 0x0066
#define QMI_LOC_SET_ROBUST_LOCATION_CONFIG_RESP_V02 0x0066
#define QMI_LOC_SET_ROBUST_LOCATION_CONFIG_IND_V02 0x0066
#define QMI_LOC_GET_ROBUST_LOCATION_CONFIG_REQ_V02 0x0067
#define QMI_LOC_GET_ROBUST_LOCATION_CONFIG_RESP_V02 0x0067
#define QMI_LOC_GET_ROBUST_LOCATION_CONFIG_IND_V02 0x0067
#define QMI_LOC_EVENT_INITIATE_POS_REQ_IND_V02 0x0068
#define QMI_LOC_GET_APS_DATA_REQ_V02 0x0069
#define QMI_LOC_GET_APS_DATA_RESP_V02 0x0069
#define QMI_LOC_GET_APS_DATA_IND_V02 0x0069
#define QMI_LOC_CANCEL_APS_DATA_REQ_V02 0x006A
#define QMI_LOC_CANCEL_APS_DATA_RESP_V02 0x006A
#define QMI_LOC_CANCEL_APS_DATA_IND_V02 0x006A
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro loc_iot_get_service_object_v02( ) that takes in no arguments. */
qmi_idl_service_object_type loc_iot_get_service_object_internal_v02
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define loc_iot_get_service_object_v02( ) \
          loc_iot_get_service_object_internal_v02( \
            LOC_IOT_V02_IDL_MAJOR_VERS, LOC_IOT_V02_IDL_MINOR_VERS, \
            LOC_IOT_V02_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

