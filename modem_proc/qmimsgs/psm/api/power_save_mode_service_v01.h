#ifndef PSM_SERVICE_01_H
#define PSM_SERVICE_01_H
/**
  @file power_save_mode_service_v01.h

  @brief This is the public header file which defines the psm service Data structures.

  This header file defines the types and structures that were defined in
  psm. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2013-2023 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.


  $Header: //components/rel/qmimsgs.mpss/4.3.4/psm/api/power_save_mode_service_v01.h#12 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Fri Mar 31 2023 (Spin 0)
   From IDL File: power_save_mode_service_v01.idl */

/** @defgroup psm_qmi_consts Constant values defined in the IDL */
/** @defgroup psm_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup psm_qmi_enums Enumerated types used in QMI messages */
/** @defgroup psm_qmi_messages Structures sent as QMI messages */
/** @defgroup psm_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup psm_qmi_accessor Accessor for QMI service object */
/** @defgroup psm_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup psm_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define PSM_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define PSM_V01_IDL_MINOR_VERS 0x0F
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define PSM_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define PSM_V01_MAX_MESSAGE_ID 0x0014
/**
    @}
  */


/** @addtogroup psm_qmi_consts
    @{
  */
#define QMI_PSM_MAX_BATCH_SUPPORTED_V01 25
#define QMI_PSM_SENSOR_TYPE_MAX_V01 10
#define QMI_PSM_SENSOR_CONFIG_MAX_V01 10
#define PERCEPTRON_WEIGHT_INDEX_V01 20
#define PERCEPTRON_MAX_MIN_FEATURES_INDEX_V01 4
#define QMI_PSM_SENSOR_SERIAL_NUMBER_MAX_LENGTH_V01 20
/**
    @}
  */

/** @addtogroup psm_qmi_enums
    @{
  */
typedef enum {
  PSM_VERSION_CAPABILITY_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PSM_NOT_SUPPORTED_V01 = 0, /**<  PSM is not supported on device  */
  PSM_VERSION_1_V01 = 1, /**<  Version1: AP decides the time that the modem goes into PSM \n */
  PSM_VERSION_2_V01 = 2, /**<  Version 2: Modem calculates and decides the time that it goes into PSM \n */
  PSM_VERSION_UNSPEC_V01 = 0xFF, /**<  PSM version unspecified, e.g., the modem either does not support the PSM feature or version 1 is 
           supported on an older modem that does not support negotiating PSM version (default value). */
  PSM_VERSION_CAPABILITY_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}psm_version_capability_v01;
/**
    @}
  */

/** @addtogroup psm_qmi_enums
    @{
  */
typedef enum {
  PSM_SENSOR_TYPE_ID_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PSM_SENSOR_TYPE_ALS_V01 = 1, /**<  Sensor information about ambient light detection \n   */
  PSM_SENSOR_TYPE_PRESSURE_V01 = 2, /**<  Sensor information about pressure \n   */
  PSM_SENSOR_TYPE_HUMIDITY_V01 = 3, /**<  Sensor information about humidity \n   */
  PSM_SENSOR_TYPE_TEMPERATURE_V01 = 4, /**<  Sensor information about temperature \n   */
  PSM_SENSOR_TYPE_AMD_V01 = 5, /**<  Sensor information about absolute motion detection used to enable different                                           motion algorithms based on the use case \n   */
  PSM_SENSOR_TYPE_SMD_V01 = 6, /**<  Sensor information about significant motion detection. \n   */
  PSM_SENSOR_TYPE_MOTION_DETECT_V01 = 7, /**<  Sensor information about physical motion detection\n  */
  PSM_SENSOR_TYPE_FMD_V01 = 8, /**<  Sensor information about flight motion detection\n  */
  PSM_SENSOR_TYPE_FALL_DETECT_V01 = 9, /**<  Sensor information about fall detection\n  */
  PSM_SENSOR_TYPE_TILT_DETECT_V01 = 10, /**<  Sensor information about tilt detection\n  */
  PSM_SENSOR_TYPE_ACCEL_V01 = 11, /**<  Accelerometer sensor information \n  */
  PSM_SENSOR_TYPE_MCD_V01 = 12, /**<  Sensor information about motion change detection\n  */
  PSM_SENSOR_TYPE_ALL_V01 = 255, /**<  Sensor information about multiple sensors   */
  PSM_SENSOR_TYPE_ID_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}psm_sensor_type_id_v01;
/**
    @}
  */

/** @addtogroup psm_qmi_enums
    @{
  */
typedef enum {
  PSM_SENSOR_TRACKER_LOG_LEVEL_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SNS_TRACKER_LOG_DISABLE_V01 = 0, /**<  Logging is disabled \n  */
  SNS_TRACKER_LOG_LEVEL1_V01 = 1, /**<  Logging is enabled and logging level is 1 \n  */
  SNS_TRACKER_LOG_LEVEL2_V01 = 2, /**<  Logging is enabled and logging level is 2  */
  PSM_SENSOR_TRACKER_LOG_LEVEL_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}psm_sensor_tracker_log_level_v01;
/**
    @}
  */

/** @addtogroup psm_qmi_enums
    @{
  */
typedef enum {
  PSM_SENSOR_FLIGHT_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PSM_SNS_FMD_STATUS_NONE_V01 = 0, 
  PSM_SNS_FMD_NOT_IN_FLIGHT_V01 = 1, 
  PSM_SNS_FMD_IN_FLIGHT_V01 = 2, 
  PSM_SNS_FMD_MAX_V01 = 255, 
  PSM_SENSOR_FLIGHT_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}psm_sensor_flight_status_enum_v01;
/**
    @}
  */

/** @addtogroup psm_qmi_enums
    @{
  */
typedef enum {
  PSM_SENSOR_MCD_STATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SNS_MCD_TRACKER_STATIONARY_V01 = 1, /**<  Stationary \n */
  SNS_MCD_TRACKER_SIG_MOTION_V01 = 2, /**<  Significant motion  */
  PSM_SENSOR_MCD_STATE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}psm_sensor_mcd_state_enum_v01;
/**
    @}
  */

/** @addtogroup psm_qmi_aggregates
    @{
  */
typedef struct {

  float greater_value;
  /**<   Any value greater than this is treated as policy met.
    0XFFFFFFFF means this field is not considered for policy matching.*/

  float lower_value;
  /**<   Any value lower than this is treated as policy met.
   0XFFFFFFFF means this field is not considered for policy matching.*/

  float delta_value;
  /**<   Minimum change in sensor values which needs to be treated as policy 
   met. 0XFFFFFFFF means this field is not considered for policy matching.*/
}psm_sensor_policy_cond_v01;  /* Type */
/**
    @}
  */

/** @addtogroup psm_qmi_aggregates
    @{
  */
typedef struct {

  float alert_threshold_high;
  /**<   Any value greater than this is treated as Alert threashold breach.
    0XFFFFFFFF means this field is not considered for Alert threashold breach.*/

  float alert_threshold_low;
  /**<   Any value lower than this is treated as Alert threashold breach.
   0XFFFFFFFF means this field is not considered for Alert threashold breach.*/

  float pre_alert_threshold_high;
  /**<   Any value greater than this is treated as Pre Alert threashold breach.
    0XFFFFFFFF means this field is not considered for Pre Alert threashold breach.*/

  float pre_alert_threshold_low;
  /**<   Any value lower than this is treated as Pre Alert threashold breach.
   0XFFFFFFFF means this field is not considered for Pre Alert threashold breach.*/
}psm_sensor_policy_cond_ext_v01;  /* Type */
/**
    @}
  */

/** @addtogroup psm_qmi_aggregates
    @{
  */
typedef struct {

  psm_sensor_type_id_v01 sensor_id;
  /**<   Values: \n
      - PSM_SENSOR_TYPE_ALS (1) --  Sensor information about ambient light detection \n  
      - PSM_SENSOR_TYPE_PRESSURE (2) --  Sensor information about pressure \n  
      - PSM_SENSOR_TYPE_HUMIDITY (3) --  Sensor information about humidity \n  
      - PSM_SENSOR_TYPE_TEMPERATURE (4) --  Sensor information about temperature \n  
      - PSM_SENSOR_TYPE_AMD (5) --  Sensor information about absolute motion detection used to enable different                                           motion algorithms based on the use case \n  
      - PSM_SENSOR_TYPE_SMD (6) --  Sensor information about significant motion detection. \n  
      - PSM_SENSOR_TYPE_MOTION_DETECT (7) --  Sensor information about physical motion detection\n 
      - PSM_SENSOR_TYPE_FMD (8) --  Sensor information about flight motion detection\n 
      - PSM_SENSOR_TYPE_FALL_DETECT (9) --  Sensor information about fall detection\n 
      - PSM_SENSOR_TYPE_TILT_DETECT (10) --  Sensor information about tilt detection\n 
      - PSM_SENSOR_TYPE_ACCEL (11) --  Accelerometer sensor information \n 
      - PSM_SENSOR_TYPE_MCD (12) --  Sensor information about motion change detection\n 
      - PSM_SENSOR_TYPE_ALL (255) --  Sensor information about multiple sensors   */

  uint32_t sensor_policy_len;  /**< Must be set to # of elements in sensor_policy */
  psm_sensor_policy_cond_v01 sensor_policy[QMI_PSM_SENSOR_CONFIG_MAX_V01];
  /**<   Policy list for the sensor type.
  */

  uint8_t sensor_batch_cnt;
  /**<    Batch count for the sensor type
  */

  uint32_t reporting_interval;
  /**<    Reporting interval for the sensor type
  */
}psm_sensor_policy_config_v01;  /* Type */
/**
    @}
  */

/** @addtogroup psm_qmi_aggregates
    @{
  */
typedef struct {

  psm_sensor_type_id_v01 sensor_id;
  /**<   Values: \n
      - PSM_SENSOR_TYPE_ALS (1) --  Sensor information about ambient light detection \n  
      - PSM_SENSOR_TYPE_PRESSURE (2) --  Sensor information about pressure \n  
      - PSM_SENSOR_TYPE_HUMIDITY (3) --  Sensor information about humidity \n  
      - PSM_SENSOR_TYPE_TEMPERATURE (4) --  Sensor information about temperature \n  
      - PSM_SENSOR_TYPE_AMD (5) --  Sensor information about absolute motion detection used to enable different                                           motion algorithms based on the use case \n  
      - PSM_SENSOR_TYPE_SMD (6) --  Sensor information about significant motion detection. \n  
      - PSM_SENSOR_TYPE_MOTION_DETECT (7) --  Sensor information about physical motion detection\n 
      - PSM_SENSOR_TYPE_FMD (8) --  Sensor information about flight motion detection\n 
      - PSM_SENSOR_TYPE_FALL_DETECT (9) --  Sensor information about fall detection\n 
      - PSM_SENSOR_TYPE_TILT_DETECT (10) --  Sensor information about tilt detection\n 
      - PSM_SENSOR_TYPE_ACCEL (11) --  Accelerometer sensor information \n 
      - PSM_SENSOR_TYPE_MCD (12) --  Sensor information about motion change detection\n 
      - PSM_SENSOR_TYPE_ALL (255) --  Sensor information about multiple sensors   */

  uint32_t sensor_policy_len;  /**< Must be set to # of elements in sensor_policy */
  psm_sensor_policy_cond_ext_v01 sensor_policy[QMI_PSM_SENSOR_CONFIG_MAX_V01];
  /**<   Policy list for the sensor type.
  */

  uint8_t operating_mode;
  /**<  Indicates whether a particular sensor is enabled or disabled.
      - 0 – disabled
      - 1 – enabled
  */

  uint8_t performance_mode;

  /*  <High performance mode determines
      slope computation, dynamic measurement period. 
      Values: \n
      -0-Default mode
      -1-High Performance/ Premium
   */
  uint8_t hysteresis_duration;

  /*  <Duration of Alarm Breach before alarm interrupt generated.
       Min: 0 samples 
       Max: 15 samples
    */
  uint32_t high_perf_measurement_period;
  /**<  Polling time of sensors during high performance mode
      Min: 60 secs
      Max: 900 secs 
      Resolution: 30 secs
  */

  uint32_t measurement_period;
}psm_sensor_policy_config_ext_v01;  /* Type */
/**
    @}
  */

typedef uint16_t psm_sensor_id_mask_v01;
#define PSM_MASK_SENSOR_ID_ALS_V01 ((psm_sensor_id_mask_v01)0x0001) /**<  ALS \n  */
#define PSM_MASK_SENSOR_ID_PRESSURE_V01 ((psm_sensor_id_mask_v01)0x0002) /**<  Pressure \n */
#define PSM_MASK_SENSOR_ID_HUMIDITY_V01 ((psm_sensor_id_mask_v01)0x0004) /**<  Humidity \n */
#define PSM_MASK_SENSOR_ID_TEMPERATURE_V01 ((psm_sensor_id_mask_v01)0x0008) /**<  Temperature \n */
#define PSM_MASK_SENSOR_ID_AMD_V01 ((psm_sensor_id_mask_v01)0x0010) /**<  AMD \n */
#define PSM_MASK_SENSOR_ID_SMD_V01 ((psm_sensor_id_mask_v01)0x0020) /**<  SMD \n */
#define PSM_MASK_SENSOR_ID_MOTION_DETECT_V01 ((psm_sensor_id_mask_v01)0x0040) /**<  Motion detection \n */
#define PSM_MASK_SENSOR_ID_FMD_V01 ((psm_sensor_id_mask_v01)0x0080) /**<  FMD \n */
#define PSM_MASK_SENSOR_ID_FALL_DETECT_V01 ((psm_sensor_id_mask_v01)0x0100) /**<  Fall detection \n */
#define PSM_MASK_SENSOR_ID_TILT_DETECT_V01 ((psm_sensor_id_mask_v01)0x0200) /**<  Tilt detection \n  */
#define PSM_MASK_SENSOR_ID_MCD_V01 ((psm_sensor_id_mask_v01)0x0400) /**<  Motion change detector  */
/** @addtogroup psm_qmi_enums
    @{
  */
typedef enum {
  SENSOR_STATE_V01_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  MEASUREMENT_V01 = 0, /**<   Measurement \n   */
  PRE_ALERT_LOW_V01 = 1, /**<   Pre Alert Low  \n   */
  PRE_ALERT_HIGH_V01 = 2, /**<   Pre Alert High \n   */
  OUT_OF_RANGE_LOW_V01 = 3, /**<   Out of range low \n   */
  OUT_OF_RANGE_HIGH_V01 = 4, /**<   Out of range high\n   */
  OPERATION_RANGE_V01 = 5, /**<   Operation range \n   */
  SENSOR_STATE_V01_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}sensor_state_v01_v01;
/**
    @}
  */

/** @addtogroup psm_qmi_enums
    @{
  */
typedef enum {
  SENSOR_ERROR_CONFIG_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  ALARM_THRESHOLD_V01 = 1, /**<   Alarm Threshold \n   */
  PRE_ALERT_THRESHOLD_V01 = 2, /**<   Pre Alert Threshold \n   */
  MEASUREMENT_PERIOD_V01 = 3, /**<   Measurement Period\n   */
  ALARM_HYSTERESIS_DURATION_V01 = 4, /**<   Alarm Hysteresis Duration \n   */
  HIGH_PERF_MEASUREMENT_PERIOD_V01 = 5, /**<   High Performance Measurement Period \n   */
  SENSOR_ERROR_CONFIG_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}sensor_error_config_v01;
/**
    @}
  */

/** @addtogroup psm_qmi_enums
    @{
  */
typedef enum {
  SENSORS_TILT_TRACKER_TRANSPORTATION_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PSM_SNS_TILT_TRACKER_TRANSPORTATION_TYPE_UNKNOWN_V01 = 0, 
  PSM_SNS_TILT_TRACKER_NO_TRANSPORTATION_OR_STATIONARY_V01 = 1, 
  PSM_SNS_TILT_TRACKER_TRANSPORTATION_TYPE_TRAIN_V01 = 2, 
  PSM_SNS_TILT_TRACKER_TRANSPORTATION_TYPE_AIRPLANE_V01 = 3, 
  PSM_SNS_TILT_TRACKER_TRANSPORTATION_TYPE_TRUCK_V01 = 4, 
  PSM_SNS_TILT_TRACKER_TRANSPORTATION_TYPE_WORST_CASE_V01 = 5, /**<  Car Cup Holder \n  */
  SENSORS_TILT_TRACKER_TRANSPORTATION_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}sensors_tilt_tracker_transportation_type_v01;
/**
    @}
  */

/** @addtogroup psm_qmi_enums
    @{
  */
typedef enum {
  PHY_SNS_TEST_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PSM_SNS_PHYSICAL_SENSOR_TEST_TYPE_SW_V01 = 0, /**<  Software test \n  */
  PSM_SNS_PHYSICAL_SENSOR_TEST_TYPE_HW_V01 = 1, /**<  Sensor hardware test \n  */
  PSM_SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY_V01 = 2, /**<  Factory test used for sensor calibration \n  */
  PSM_SNS_PHYSICAL_SENSOR_TEST_TYPE_COM_V01 = 3, /**<  Communication bus test \n  */
  PHY_SNS_TEST_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}phy_sns_test_type_v01;
/**
    @}
  */

/** @addtogroup psm_qmi_aggregates
    @{
  */
typedef struct {

  psm_sensor_type_id_v01 sns_id;

  /*  Sensor type: \n */
  sensor_error_config_v01 error_config;

  /*  Sensor error config: \n */
  uint8_t error_type;
}sensor_error_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup psm_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t sensor_dimension_data_len;  /**< Must be set to # of elements in sensor_dimension_data */
  float sensor_dimension_data[QMI_PSM_SENSOR_CONFIG_MAX_V01];
  /**<   Time in milliseconds that has
       elapsed from the start of GPS Epoch time
       (Jan 6, 1980).
       */

  uint64_t timestamp;
  /**<   Time stamp. */
}psm_sensor_sample_data_v01;  /* Type */
/**
    @}
  */

/** @addtogroup psm_qmi_aggregates
    @{
  */
typedef struct {

  psm_sensor_type_id_v01 sensor_id;
  /**<  List of samples for this sensor ID. Values: \n
      - PSM_SENSOR_TYPE_ALS (1) --  Sensor information about ambient light detection \n  
      - PSM_SENSOR_TYPE_PRESSURE (2) --  Sensor information about pressure \n  
      - PSM_SENSOR_TYPE_HUMIDITY (3) --  Sensor information about humidity \n  
      - PSM_SENSOR_TYPE_TEMPERATURE (4) --  Sensor information about temperature \n  
      - PSM_SENSOR_TYPE_AMD (5) --  Sensor information about absolute motion detection used to enable different                                           motion algorithms based on the use case \n  
      - PSM_SENSOR_TYPE_SMD (6) --  Sensor information about significant motion detection. \n  
      - PSM_SENSOR_TYPE_MOTION_DETECT (7) --  Sensor information about physical motion detection\n 
      - PSM_SENSOR_TYPE_FMD (8) --  Sensor information about flight motion detection\n 
      - PSM_SENSOR_TYPE_FALL_DETECT (9) --  Sensor information about fall detection\n 
      - PSM_SENSOR_TYPE_TILT_DETECT (10) --  Sensor information about tilt detection\n 
      - PSM_SENSOR_TYPE_ACCEL (11) --  Accelerometer sensor information \n 
      - PSM_SENSOR_TYPE_MCD (12) --  Sensor information about motion change detection\n 
      - PSM_SENSOR_TYPE_ALL (255) --  Sensor information about multiple sensors  */

  uint32_t sample_list_len;  /**< Must be set to # of elements in sample_list */
  psm_sensor_sample_data_v01 sample_list[QMI_PSM_MAX_BATCH_SUPPORTED_V01];
}psm_sensor_info_v01;  /* Type */
/**
    @}
  */

/** @addtogroup psm_qmi_aggregates
    @{
  */
typedef struct {

  sensor_state_v01_v01 event_type;

  /*  Event type: \n */
  sensor_state_v01_v01 sensor_state;

  /*  Event type: \n */
  float ttc;

  /*  <High Performance -Indicates time remaining (in mins) until sensor measurements breach threshold. 
       Maximum value   – Restore measurement period to default. 
   */
  float slope;

  /*  < High Performance: Indicates rate of change of sensor measurements
   */
  uint32_t high_perf_measurement_period;
}psm_sensor_algo_info_v01;  /* Type */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Registers the requesting control point for various indications. */
typedef struct {

  /* Optional */
  /*  Power Save Mode Status */
  uint8_t report_psm_status_valid;  /**< Must be set to true if report_psm_status is being passed */
  uint8_t report_psm_status;
  /**<   Values: \n
	  - 0 -- Do not report \n
	  - 1 -- Report PSM status 
  */

  /* Optional */
  /*  Power Save Mode Configuration Change Information  */
  uint8_t report_psm_cfg_change_valid;  /**< Must be set to true if report_psm_cfg_change is being passed */
  uint8_t report_psm_cfg_change;
  /**<   Values: \n
	  - 0 -- Do not report \n
	  - 1 -- Report Power Save Mode configuration changes 
  */

  /* Optional */
  /*  Report Sensor Status  */
  uint8_t report_sensor_status_valid;  /**< Must be set to true if report_sensor_status is being passed */
  uint8_t report_sensor_status;
  /**<   Values: \n 
    - 0 -- Do not report \n 
    - 1 -- Report sensor status  
    */

  /* Optional */
  /*  Report Physical Sensor Test Result */
  uint8_t report_sns_phy_test_ind_valid;  /**< Must be set to true if report_sns_phy_test_ind is being passed */
  uint8_t report_sns_phy_test_ind;
  /**<   Indicates whether to report the results of the physical sensor test: \n
        Values: \n 
      - 0 -- Do not report \n 
      - 1 -- Report 
   */

  /* Optional */
  /*  Report Physical Sensor Test Data  */
  uint8_t report_sns_phy_test_get_data_ind_valid;  /**< Must be set to true if report_sns_phy_test_get_data_ind is being passed */
  uint8_t report_sns_phy_test_get_data_ind;
  /**<   Indicates whether to report the data of the physical sensor test: \n
        Values: \n 
      - 0 -- Do not report \n
      - 1 -- Report 
   */
}psm_indication_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Registers the requesting control point for various indications. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}psm_indication_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Queries the Power Save Mode (PSM) configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}psm_get_cfg_params_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Queries the Power Save Mode (PSM) configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Power Save Mode Enable State  */
  uint8_t psm_enabled_valid;  /**< Must be set to true if psm_enabled is being passed */
  uint8_t psm_enabled;
  /**<   Values: \n
       - 0 -- PSM is not enabled \n
       - 1 -- PSM is enabled
  */

  /* Optional */
  /*  Duration Threshold */
  uint8_t duration_threshold_valid;  /**< Must be set to true if duration_threshold is being passed */
  uint32_t duration_threshold;
  /**<   Minimum duration for the device to benefit by entering PSM (in seconds).
  */

  /* Optional */
  /*  Duration Due to OOS */
  uint8_t duration_due_to_oos_valid;  /**< Must be set to true if duration_due_to_oos is being passed */
  uint32_t duration_due_to_oos;
  /**<   Power Save mode duration due to an outage (in seconds). 
  */

  /* Optional */
  /*  Randomization Window */
  uint8_t randomization_window_valid;  /**< Must be set to true if randomization_window is being passed */
  uint32_t randomization_window;
  /**<   Power Save mode wakeup randomization window (in seconds). 
  */

  /* Optional */
  /*  Active Timer */
  uint8_t active_timer_valid;  /**< Must be set to true if active_timer is being passed */
  uint32_t active_timer;
  /**<   Power Save mode active timer value (in seconds). 
  */

  /* Optional */
  /*  Periodic Update Timer */
  uint8_t periodic_update_timer_valid;  /**< Must be set to true if periodic_update_timer is being passed */
  uint32_t periodic_update_timer;
  /**<   Power Save mode periodic update timer value (in seconds). 
  */

  /* Optional */
  /*  Early Wakeup Time */
  uint8_t early_wakeup_time_valid;  /**< Must be set to true if early_wakeup_time is being passed */
  uint32_t early_wakeup_time;
  /**<   Power Save Mode early wakeup time (in seconds) indicating
       how early the device should exit PSM to offset for bootup and 
       acquisition delay.
  */
}psm_get_cfg_params_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Requests to enter Power Save Mode (PSM). */
typedef struct {

  /* Optional */
  /*  Duration in PSM */
  uint8_t duration_in_psm_valid;  /**< Must be set to true if duration_in_psm is being passed */
  uint32_t duration_in_psm;
  /**<   Duration to stay in PSM (in seconds).
  */

  /* Optional */
  /*  Status  */
  uint8_t status_valid;  /**< Must be set to true if status is being passed */
  uint8_t status;
  /**<   Flag to accept or reject the PSM request.\n
         - 0 -- Reject \n
         - 1 -- Accept 
   */
}psm_enter_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Requests to enter Power Save Mode (PSM). */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}psm_enter_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_enums
    @{
  */
typedef enum {
  PSM_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PSM_STATUS_COMPLETED_V01 = 0, /**<  Entering into PSM is complete.  */
  PSM_STATUS_REJECTED_V01 = 1, /**<  Entering PSM is rejected.  */
  PSM_STATUS_READY_V01 = 2, /**<  Modem is ready to enter PSM.   */
  PSM_STATUS_OOS_V01 = 3, /**<  UE is unable to acquire service, even after a few full scans.  */
  PSM_STATUS_LIMITED_SERVICE_V01 = 4, /**<  UE has acquired limited service but not full service.  */
  PSM_STATUS_AUTO_READY_V01 = 5, /**<  Modem is autonomously ready to enter into PSM.  */
  PSM_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}psm_status_enum_v01;
/**
    @}
  */

/** @addtogroup psm_qmi_enums
    @{
  */
typedef enum {
  PSM_REJECT_REASON_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PSM_REJECT_REASON_NO_ERROR_V01 = 0, /**<  No error.  */
  PSM_REJECT_REASON_NOT_ENABLED_V01 = 1, /**<  PSM is not enabled.  */
  PSM_REJECT_REASON_MODEM_NOT_READY_V01 = 2, /**<  Modem is not in a ready state; for example, an MT call is in progress.  */
  PSM_REJECT_REASON_DURATION_TOO_SHORT_V01 = 3, /**<  Duration set by the control point is less than the PSM threshold.  */
  PSM_REJECT_REASON_PSM_DURATION_CHANGED_V01 = 4, /**<  Duration set by the control point is greater than the PSM timer duration 
       on the modem; the control point can retry with a shorter duration.
   */
  PSM_REJECT_REASON_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}psm_reject_reason_enum_v01;
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Indication Message; Indicates PSM status. */
typedef struct {

  /* Mandatory */
  /*  PSM Status */
  psm_status_enum_v01 psm_status;
  /**<   PSM status. Values: \n
      - PSM_STATUS_COMPLETED (0) --  Entering into PSM is complete. 
      - PSM_STATUS_REJECTED (1) --  Entering PSM is rejected. 
      - PSM_STATUS_READY (2) --  Modem is ready to enter PSM.  
      - PSM_STATUS_OOS (3) --  UE is unable to acquire service, even after a few full scans. 
      - PSM_STATUS_LIMITED_SERVICE (4) --  UE has acquired limited service but not full service. 
      - PSM_STATUS_AUTO_READY (5) --  Modem is autonomously ready to enter into PSM. 
 */

  /* Optional */
  /*  PSM Reject Reason */
  uint8_t reject_reason_valid;  /**< Must be set to true if reject_reason is being passed */
  psm_reject_reason_enum_v01 reject_reason;
  /**<   PSM reject status. Values: \n
      - PSM_REJECT_REASON_NO_ERROR (0) --  No error. 
      - PSM_REJECT_REASON_NOT_ENABLED (1) --  PSM is not enabled. 
      - PSM_REJECT_REASON_MODEM_NOT_READY (2) --  Modem is not in a ready state; for example, an MT call is in progress. 
      - PSM_REJECT_REASON_DURATION_TOO_SHORT (3) --  Duration set by the control point is less than the PSM threshold. 
      - PSM_REJECT_REASON_PSM_DURATION_CHANGED (4) --  Duration set by the control point is greater than the PSM timer duration 
       on the modem; the control point can retry with a shorter duration.
  
 */

  /* Optional */
  /*  Periodic Update Timer */
  uint8_t periodic_update_timer_valid;  /**< Must be set to true if periodic_update_timer is being passed */
  uint32_t periodic_update_timer;
  /**<   PSM periodic update timer value (in seconds). 
  */
}psm_status_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Requests to get the PSM capability. */
typedef struct {

  /* Optional */
  /*  TE PSM Version */
  uint8_t te_psm_capability_valid;  /**< Must be set to true if te_psm_capability is being passed */
  psm_version_capability_v01 te_psm_capability;
  /**<   PSM version that the control point supports Values : \n
      - PSM_NOT_SUPPORTED (0) --  PSM is not supported on device 
      - PSM_VERSION_1 (1) --  Version1: AP decides the time that the modem goes into PSM \n
      - PSM_VERSION_2 (2) --  Version 2: Modem calculates and decides the time that it goes into PSM \n
      - PSM_VERSION_UNSPEC (0xFF) --  PSM version unspecified, e.g., the modem either does not support the PSM feature or version 1 is 
           supported on an older modem that does not support negotiating PSM version (default value).
 */
}psm_get_capability_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Requests to get the PSM capability. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  PSM Version Support */
  uint8_t psm_capability_valid;  /**< Must be set to true if psm_capability is being passed */
  psm_version_capability_v01 psm_capability;
  /**<   PSM version honored by the modem Values : \n
      - PSM_NOT_SUPPORTED (0) --  PSM is not supported on device 
      - PSM_VERSION_1 (1) --  Version1: AP decides the time that the modem goes into PSM \n
      - PSM_VERSION_2 (2) --  Version 2: Modem calculates and decides the time that it goes into PSM \n
      - PSM_VERSION_UNSPEC (0xFF) --  PSM version unspecified, e.g., the modem either does not support the PSM feature or version 1 is 
           supported on an older modem that does not support negotiating PSM version (default value).
 */
}psm_get_capability_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Sets PSM (Power Save Mode) configuration parameters */
typedef struct {

  /* Optional */
  /*  PSM Enable State  */
  uint8_t psm_enabled_valid;  /**< Must be set to true if psm_enabled is being passed */
  uint8_t psm_enabled;
  /**<   Values: \n
       - 0 -- PSM is not enabled \n
       - 1 -- PSM is enabled
  */

  /* Optional */
  /*  Active Timer */
  uint8_t active_timer_valid;  /**< Must be set to true if active_timer is being passed */
  uint32_t active_timer;
  /**<   PSM active timer value (in seconds).
  */

  /* Optional */
  /*  Periodic Update Timer */
  uint8_t periodic_update_timer_valid;  /**< Must be set to true if periodic_update_timer is being passed */
  uint32_t periodic_update_timer;
  /**<   PSM periodic update timer value (in seconds). 
  */
}psm_set_cfg_params_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Sets PSM (Power Save Mode) configuration parameters */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}psm_set_cfg_params_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Indication Message; Indicates PSM status. */
typedef struct {

  /* Optional */
  /*  PSM Enable State  */
  uint8_t psm_enabled_valid;  /**< Must be set to true if psm_enabled is being passed */
  uint8_t psm_enabled;
  /**<   Values: \n
       - 0 -- PSM is not enabled \n
       - 1 -- PSM is enabled
  */

  /* Optional */
  /*  Active Timer */
  uint8_t active_timer_valid;  /**< Must be set to true if active_timer is being passed */
  uint32_t active_timer;
  /**<   PSM active timer value (in seconds). 
  */

  /* Optional */
  /*  Periodic Update Timer */
  uint8_t periodic_update_timer_valid;  /**< Must be set to true if periodic_update_timer is being passed */
  uint32_t periodic_update_timer;
  /**<   PSM periodic update timer value (in seconds). 
  */
}psm_cfg_params_change_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Sends the sensor configuration parameters to the modem. */
typedef struct {

  /* Mandatory */
  /*  Sensor ID */
  psm_sensor_type_id_v01 sensor_id;
  /**<   Sensor type for which the configuration is sent. Values:\n
      - PSM_SENSOR_TYPE_ALS (1) --  Sensor information about ambient light detection \n  
      - PSM_SENSOR_TYPE_PRESSURE (2) --  Sensor information about pressure \n  
      - PSM_SENSOR_TYPE_HUMIDITY (3) --  Sensor information about humidity \n  
      - PSM_SENSOR_TYPE_TEMPERATURE (4) --  Sensor information about temperature \n  
      - PSM_SENSOR_TYPE_AMD (5) --  Sensor information about absolute motion detection used to enable different                                           motion algorithms based on the use case \n  
      - PSM_SENSOR_TYPE_SMD (6) --  Sensor information about significant motion detection. \n  
      - PSM_SENSOR_TYPE_MOTION_DETECT (7) --  Sensor information about physical motion detection\n 
      - PSM_SENSOR_TYPE_FMD (8) --  Sensor information about flight motion detection\n 
      - PSM_SENSOR_TYPE_FALL_DETECT (9) --  Sensor information about fall detection\n 
      - PSM_SENSOR_TYPE_TILT_DETECT (10) --  Sensor information about tilt detection\n 
      - PSM_SENSOR_TYPE_ACCEL (11) --  Accelerometer sensor information \n 
      - PSM_SENSOR_TYPE_MCD (12) --  Sensor information about motion change detection\n 
      - PSM_SENSOR_TYPE_ALL (255) --  Sensor information about multiple sensors  
 */

  /* Optional */
  /*  Sensor Configuration List */
  uint8_t sensor_config_list_valid;  /**< Must be set to true if sensor_config_list is being passed */
  uint32_t sensor_config_list_len;  /**< Must be set to # of elements in sensor_config_list */
  psm_sensor_policy_config_v01 sensor_config_list[QMI_PSM_SENSOR_CONFIG_MAX_V01];
  /**<   \n Sensor configuration for multiple sensors.
        */
}psm_set_sensor_configuration_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Sends the sensor configuration parameters to the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}psm_set_sensor_configuration_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Sends the sensor configuration parameters to the modem. */
typedef struct {

  /* Optional */
  /*  Sensor Configuration List */
  uint8_t sensor_config_list_valid;  /**< Must be set to true if sensor_config_list is being passed */
  uint32_t sensor_config_list_len;  /**< Must be set to # of elements in sensor_config_list */
  psm_sensor_policy_config_ext_v01 sensor_config_list[QMI_PSM_SENSOR_CONFIG_MAX_V01];
  /**<   \n Sensor configuration for multiple sensors.
        */

  /* Optional */
  /*  Ambient Light Sensor Threshold */
  uint8_t als_threshold_valid;  /**< Must be set to true if als_threshold is being passed */
  uint32_t als_threshold;

  /* Optional */
  /*  Flight Mode Pressure Enable */
  uint8_t flight_mode_pressure_enable_valid;  /**< Must be set to true if flight_mode_pressure_enable is being passed */
  uint8_t flight_mode_pressure_enable;

  /* Optional */
  /*  Flight Mode Pressure Threshold */
  uint8_t flight_mode_pressure_threshold_valid;  /**< Must be set to true if flight_mode_pressure_threshold is being passed */
  uint16_t flight_mode_pressure_threshold;

  /* Optional */
  /*  Flight Mode Accelerometer Sample Rate */
  uint8_t flight_mode_accelerometer_sample_rate_valid;  /**< Must be set to true if flight_mode_accelerometer_sample_rate is being passed */
  uint8_t flight_mode_accelerometer_sample_rate;

  /* Optional */
  /*  Flight Mode Accelerometer Sample Length */
  uint8_t flight_mode_accelerometer_sample_length_valid;  /**< Must be set to true if flight_mode_accelerometer_sample_length is being passed */
  uint8_t flight_mode_accelerometer_sample_length;

  /* Optional */
  /*  Sensor Tilt Transportation Type */
  uint8_t transportation_type_valid;  /**< Must be set to true if transportation_type is being passed */
  sensors_tilt_tracker_transportation_type_v01 transportation_type;
  /**<   Transportation type of the vehicle where sensor is kept 
  */

  /* Optional */
  /*  Redo Initial Gravity */
  uint8_t redo_gravity_init_valid;  /**< Must be set to true if redo_gravity_init is being passed */
  uint8_t redo_gravity_init;
  /**<   Tilt reconfiguration of initial gravity.
     */

  /* Optional */
  /*  Iteration Delay */
  uint8_t iteration_delay_valid;  /**< Must be set to true if iteration_delay is being passed */
  uint32_t iteration_delay;
  /**<   Time delay to send the configuration to the tilt sensor.
  */

  /* Optional */
  /*  Initial Maximum Trial Number */
  uint8_t init_max_trial_num_valid;  /**< Must be set to true if init_max_trial_num is being passed */
  uint8_t init_max_trial_num;
  /**<   Initial maximum number of times to collect sample for the tilt sensor.
  */

  /* Optional */
  /*  Updated Maximum Trial Number */
  uint8_t update_max_trial_num_valid;  /**< Must be set to true if update_max_trial_num is being passed */
  uint8_t update_max_trial_num;
  /**<   Updated maximum number of times to collect sample for the tilt sensor.
  */

  /* Optional */
  /*  Initial Gravity Threshold */
  uint8_t init_gravity_threshold_valid;  /**< Must be set to true if init_gravity_threshold is being passed */
  float init_gravity_threshold;
  /**<   Initial maximum gravity value for sensor for policy met for the tilt sensor.
    */

  /* Optional */
  /*  Updated Gravity Threshold */
  uint8_t update_gravity_threshold_valid;  /**< Must be set to true if update_gravity_threshold is being passed */
  float update_gravity_threshold;
  /**<   Updated maximum gravity value for sensor for policy met for the tilt sensor.
  */

  /* Optional */
  /*  Perceptron Weights */
  uint8_t smd_perceptron_weights_valid;  /**< Must be set to true if smd_perceptron_weights is being passed */
  uint32_t smd_perceptron_weights_len;  /**< Must be set to # of elements in smd_perceptron_weights */
  float smd_perceptron_weights[PERCEPTRON_WEIGHT_INDEX_V01];
  /**<   Weights for the perceptron machine learning algorithm.
  */

  /* Optional */
  /*  Perceptron Min Feature */
  uint8_t smd_perceptron_min_features_valid;  /**< Must be set to true if smd_perceptron_min_features is being passed */
  uint32_t smd_perceptron_min_features_len;  /**< Must be set to # of elements in smd_perceptron_min_features */
  float smd_perceptron_min_features[PERCEPTRON_MAX_MIN_FEATURES_INDEX_V01];
  /**<   Multiply minimum weights with input features in the perceptron machine learning algorithm.
  */

  /* Optional */
  /*  Perceptron Max Feature */
  uint8_t smd_perceptron_max_features_valid;  /**< Must be set to true if smd_perceptron_max_features is being passed */
  uint32_t smd_perceptron_max_features_len;  /**< Must be set to # of elements in smd_perceptron_max_features */
  float smd_perceptron_max_features[PERCEPTRON_MAX_MIN_FEATURES_INDEX_V01];
  /**<   Multiply maximum weights with input features in the perceptron machine learning algorithm.
  */

  /* Optional */
  /*  Sum Score Threshold */
  uint8_t smd_sum_score_threshold_valid;  /**< Must be set to true if smd_sum_score_threshold is being passed */
  uint8_t smd_sum_score_threshold;
  /**<   Threshold after computing the sum throughput sliding window. 
  */

  /* Optional */
  /*  Motion Detection Tuning Threshold */
  uint8_t md_tuning_threshold_valid;  /**< Must be set to true if md_tuning_threshold is being passed */
  float md_tuning_threshold;
  /**<   Threshold for hardware motion detection tuning. 
  */

  /* Optional */
  /*  Motion Toggle Delay Threshold */
  uint8_t mcd_motion_toggle_delay_threshold_valid;  /**< Must be set to true if mcd_motion_toggle_delay_threshold is being passed */
  uint16_t mcd_motion_toggle_delay_threshold;
  /**<   Threshold for hardware motion detection tuning. 
  */

  /* Optional */
  /*  Motion Toggle Counter Threshold */
  uint8_t mcd_motion_toggle_counter_threshold_valid;  /**< Must be set to true if mcd_motion_toggle_counter_threshold is being passed */
  uint8_t mcd_motion_toggle_counter_threshold;
  /**<   Threshold for the toggle counter value to go to timer state. 
  */

  /* Optional */
  /*  PSM Timer Config */
  uint8_t mcd_psm_timer_config_valid;  /**< Must be set to true if mcd_psm_timer_config is being passed */
  uint8_t mcd_psm_timer_config;
  /**<   Time to sleep after significant motion or motion detected. 
 */

  /* Optional */
  /*  Flight mode Accelerometer Threshold  */
  uint8_t fmd_accel_thres_valid;  /**< Must be set to true if fmd_accel_thres is being passed */
  float fmd_accel_thres;
  /**<   Threshold for accelerometer. 
  */

  /* Optional */
  /*  Flight Mode Accelerometer Buffer Count */
  uint8_t fmd_accel_buffer_count_valid;  /**< Must be set to true if fmd_accel_buffer_count is being passed */
  uint32_t fmd_accel_buffer_count;
  /**<   COunter for accelerator buffer. 
  */

  /* Optional */
  /*  Flight mode Accelerometerr Threshold */
  uint8_t fmd_run_period_valid;  /**< Must be set to true if fmd_run_period is being passed */
  uint32_t fmd_run_period;
  /**<   Delay between the next iteration. 
  */

  /* Optional */
  /*  Sliding Window Size */
  uint8_t smd_sld_win_sz_valid;  /**< Must be set to true if smd_sld_win_sz is being passed */
  float smd_sld_win_sz;
  /**<   Sliding window size is a config parameter for significant motion detection. 
  */

  /* Optional */
  /*  Sliding Window Shift Size */
  uint8_t smd_sld_win_s_sz_valid;  /**< Must be set to true if smd_sld_win_s_sz is being passed */
  float smd_sld_win_s_sz;
  /**<   Sliding window shift size is a config parameter for significant motion detection. 
  */

  /* Optional */
  /*  Shock Detection Threshold */
  uint8_t shock_detection_threshold_valid;  /**< Must be set to true if shock_detection_threshold is being passed */
  float shock_detection_threshold;
  /**<   Threshold for shock detection. 
  */
}psm_set_sensor_configuration_ext_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Sends the sensor configuration parameters to the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Sensor Error Resp */
  uint8_t sensor_error_resp_valid;  /**< Must be set to true if sensor_error_resp is being passed */
  uint32_t sensor_error_resp_len;  /**< Must be set to # of elements in sensor_error_resp */
  sensor_error_type_v01 sensor_error_resp[QMI_PSM_SENSOR_CONFIG_MAX_V01];
}psm_set_sensor_configuration_ext_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Gets the sensor configuration parameters from the modem. */
typedef struct {

  /* Mandatory */
  /*  Sensor ID */
  psm_sensor_type_id_v01 sensor_id;
  /**<   Sensor type for which the configuration is sent. Values:\n
      - PSM_SENSOR_TYPE_ALS (1) --  Sensor information about ambient light detection \n  
      - PSM_SENSOR_TYPE_PRESSURE (2) --  Sensor information about pressure \n  
      - PSM_SENSOR_TYPE_HUMIDITY (3) --  Sensor information about humidity \n  
      - PSM_SENSOR_TYPE_TEMPERATURE (4) --  Sensor information about temperature \n  
      - PSM_SENSOR_TYPE_AMD (5) --  Sensor information about absolute motion detection used to enable different                                           motion algorithms based on the use case \n  
      - PSM_SENSOR_TYPE_SMD (6) --  Sensor information about significant motion detection. \n  
      - PSM_SENSOR_TYPE_MOTION_DETECT (7) --  Sensor information about physical motion detection\n 
      - PSM_SENSOR_TYPE_FMD (8) --  Sensor information about flight motion detection\n 
      - PSM_SENSOR_TYPE_FALL_DETECT (9) --  Sensor information about fall detection\n 
      - PSM_SENSOR_TYPE_TILT_DETECT (10) --  Sensor information about tilt detection\n 
      - PSM_SENSOR_TYPE_ACCEL (11) --  Accelerometer sensor information \n 
      - PSM_SENSOR_TYPE_MCD (12) --  Sensor information about motion change detection\n 
      - PSM_SENSOR_TYPE_ALL (255) --  Sensor information about multiple sensors  
 */
}psm_get_sensor_configuration_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Gets the sensor configuration parameters from the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Sensor Configuration List */
  uint8_t sensor_config_list_valid;  /**< Must be set to true if sensor_config_list is being passed */
  psm_sensor_policy_config_ext_v01 sensor_config_list;

  /* Optional */
  /*  Ambient Light Sensor Threshold */
  uint8_t als_threshold_valid;  /**< Must be set to true if als_threshold is being passed */
  uint32_t als_threshold;

  /* Optional */
  /*  Flight Mode Pressure Enable */
  uint8_t flight_mode_pressure_enable_valid;  /**< Must be set to true if flight_mode_pressure_enable is being passed */
  uint8_t flight_mode_pressure_enable;

  /* Optional */
  /*  Flight Mode Pressure Threshold */
  uint8_t flight_mode_pressure_threshold_valid;  /**< Must be set to true if flight_mode_pressure_threshold is being passed */
  uint16_t flight_mode_pressure_threshold;

  /* Optional */
  /*  Flight Mode Accelerometer Sample Rate */
  uint8_t flight_mode_accelerometer_sample_rate_valid;  /**< Must be set to true if flight_mode_accelerometer_sample_rate is being passed */
  uint8_t flight_mode_accelerometer_sample_rate;

  /* Optional */
  /*  Flight Mode Accelerometer Sample Length */
  uint8_t flight_mode_accelerometer_sample_length_valid;  /**< Must be set to true if flight_mode_accelerometer_sample_length is being passed */
  uint8_t flight_mode_accelerometer_sample_length;

  /* Optional */
  /*  Sensor Tilt Transportation Type */
  uint8_t transportation_type_valid;  /**< Must be set to true if transportation_type is being passed */
  sensors_tilt_tracker_transportation_type_v01 transportation_type;
  /**<   Transportation type of the vehicle where sensor is kept 
  */

  /* Optional */
  /*  Redo Initial Gravity */
  uint8_t redo_gravity_init_valid;  /**< Must be set to true if redo_gravity_init is being passed */
  uint8_t redo_gravity_init;
  /**<   Tilt reconfiguration of initial gravity.
     */

  /* Optional */
  /*  Iteration Delay */
  uint8_t iteration_delay_valid;  /**< Must be set to true if iteration_delay is being passed */
  uint32_t iteration_delay;
  /**<   Time delay to send the configuration to the tilt sensor.
  */

  /* Optional */
  /*  Initial Maximum Trial Number */
  uint8_t init_max_trial_num_valid;  /**< Must be set to true if init_max_trial_num is being passed */
  uint8_t init_max_trial_num;
  /**<   Initial maximum number of times to collect sample for the tilt sensor.
  */

  /* Optional */
  /*  Updated Maximum Trial Number */
  uint8_t update_max_trial_num_valid;  /**< Must be set to true if update_max_trial_num is being passed */
  uint8_t update_max_trial_num;
  /**<   Updated maximum number of times to collect sample for the tilt sensor.
  */

  /* Optional */
  /*  Initial Gravity Threshold */
  uint8_t init_gravity_threshold_valid;  /**< Must be set to true if init_gravity_threshold is being passed */
  float init_gravity_threshold;
  /**<   Initial maximum gravity value for sensor for policy met for the tilt sensor.
    */

  /* Optional */
  /*  Updated Gravity Threshold */
  uint8_t update_gravity_threshold_valid;  /**< Must be set to true if update_gravity_threshold is being passed */
  float update_gravity_threshold;
  /**<   Updated maximum gravity value for sensor for policy met for the tilt sensor.
  */

  /* Optional */
  /*  Perceptron Weights */
  uint8_t smd_perceptron_weights_valid;  /**< Must be set to true if smd_perceptron_weights is being passed */
  uint32_t smd_perceptron_weights_len;  /**< Must be set to # of elements in smd_perceptron_weights */
  float smd_perceptron_weights[PERCEPTRON_WEIGHT_INDEX_V01];
  /**<   Weights for the perceptron machine learning algorithm.
  */

  /* Optional */
  /*  Perceptron Min Feature */
  uint8_t smd_perceptron_min_features_valid;  /**< Must be set to true if smd_perceptron_min_features is being passed */
  uint32_t smd_perceptron_min_features_len;  /**< Must be set to # of elements in smd_perceptron_min_features */
  float smd_perceptron_min_features[PERCEPTRON_MAX_MIN_FEATURES_INDEX_V01];
  /**<   Multiply minimum weights with input features in the perceptron machine learning algorithm.
  */

  /* Optional */
  /*  Perceptron Max Feature */
  uint8_t smd_perceptron_max_features_valid;  /**< Must be set to true if smd_perceptron_max_features is being passed */
  uint32_t smd_perceptron_max_features_len;  /**< Must be set to # of elements in smd_perceptron_max_features */
  float smd_perceptron_max_features[PERCEPTRON_MAX_MIN_FEATURES_INDEX_V01];
  /**<   Multiply maximum weights with input features in the perceptron machine learning algorithm.
  */

  /* Optional */
  /*  Sum Score Threshold */
  uint8_t smd_sum_score_threshold_valid;  /**< Must be set to true if smd_sum_score_threshold is being passed */
  uint8_t smd_sum_score_threshold;
  /**<   Threshold after computing the sum throughput sliding window. 
  */

  /* Optional */
  /*  Motion Detection Tuning Threshold */
  uint8_t md_tuning_threshold_valid;  /**< Must be set to true if md_tuning_threshold is being passed */
  float md_tuning_threshold;
  /**<   Threshold for hardware motion detection tuning. 
  */

  /* Optional */
  /*  Motion Toggle Delay Threshold */
  uint8_t mcd_motion_toggle_delay_threshold_valid;  /**< Must be set to true if mcd_motion_toggle_delay_threshold is being passed */
  uint16_t mcd_motion_toggle_delay_threshold;
  /**<   Threshold for hardware motion detection tuning. 
  */

  /* Optional */
  /*  Motion Toggle Counter Threshold */
  uint8_t mcd_motion_toggle_counter_threshold_valid;  /**< Must be set to true if mcd_motion_toggle_counter_threshold is being passed */
  uint8_t mcd_motion_toggle_counter_threshold;
  /**<   Threshold for the toggle counter value to go to timer state. 
  */

  /* Optional */
  /*  PSM Timer Config */
  uint8_t mcd_psm_timer_config_valid;  /**< Must be set to true if mcd_psm_timer_config is being passed */
  uint8_t mcd_psm_timer_config;
  /**<   Time to sleep after significant motion or motion detected. 
  */

  /* Optional */
  /*  Flight mode Accelerometer Threshold  */
  uint8_t fmd_accel_thres_valid;  /**< Must be set to true if fmd_accel_thres is being passed */
  float fmd_accel_thres;
  /**<   Threshold for accelerometer. 
  */

  /* Optional */
  /*  Flight Mode Accelerometer Buffer Count */
  uint8_t fmd_accel_buffer_count_valid;  /**< Must be set to true if fmd_accel_buffer_count is being passed */
  uint32_t fmd_accel_buffer_count;
  /**<   COunter for accelerator buffer. 
  */

  /* Optional */
  /*  Flight mode Accelerometerr Threshold */
  uint8_t fmd_run_period_valid;  /**< Must be set to true if fmd_run_period is being passed */
  uint32_t fmd_run_period;
  /**<   Delay between the next iteration. 
  */

  /* Optional */
  /*  Sliding Window Size */
  uint8_t smd_sld_win_sz_valid;  /**< Must be set to true if smd_sld_win_sz is being passed */
  float smd_sld_win_sz;
  /**<   Sliding window size is a config parameter for significant motion detection. 
  */

  /* Optional */
  /*  Sliding Window Shift Size */
  uint8_t smd_sld_win_s_sz_valid;  /**< Must be set to true if smd_sld_win_s_sz is being passed */
  float smd_sld_win_s_sz;
  /**<   Sliding window shift size is a config parameter for significant motion detection. 
  */

  /* Optional */
  /*  Shock Detection Threshold */
  uint8_t shock_detection_threshold_valid;  /**< Must be set to true if shock_detection_threshold is being passed */
  float shock_detection_threshold;
  /**<   Threshold for shock detection. 
  */
}psm_get_sensor_configuration_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Queries sensor data snapshot from the modem. */
typedef struct {

  /* Mandatory */
  /*  Sensor ID Mask  */
  psm_sensor_id_mask_v01 sensor_id_mask;
  /**<   Values: \n 
      - PSM_MASK_SENSOR_ID_ALS (0x0001) --  ALS \n 
      - PSM_MASK_SENSOR_ID_PRESSURE (0x0002) --  Pressure \n
      - PSM_MASK_SENSOR_ID_HUMIDITY (0x0004) --  Humidity \n
      - PSM_MASK_SENSOR_ID_TEMPERATURE (0x0008) --  Temperature \n
      - PSM_MASK_SENSOR_ID_AMD (0x0010) --  AMD \n
      - PSM_MASK_SENSOR_ID_SMD (0x0020) --  SMD \n
      - PSM_MASK_SENSOR_ID_MOTION_DETECT (0x0040) --  Motion detection \n
      - PSM_MASK_SENSOR_ID_FMD (0x0080) --  FMD \n
      - PSM_MASK_SENSOR_ID_FALL_DETECT (0x0100) --  Fall detection \n
      - PSM_MASK_SENSOR_ID_TILT_DETECT (0x0200) --  Tilt detection \n 
      - PSM_MASK_SENSOR_ID_MCD (0x0400) --  Motion change detector 
 */

  /* Optional */
  /*  Is Logging Mode */
  uint8_t is_logging_mode_valid;  /**< Must be set to true if is_logging_mode is being passed */
  uint8_t is_logging_mode;
}psm_get_sensor_data_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Queries sensor data snapshot from the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Sensor Information */
  uint8_t sensor_info_valid;  /**< Must be set to true if sensor_info is being passed */
  uint32_t sensor_info_len;  /**< Must be set to # of elements in sensor_info */
  psm_sensor_info_v01 sensor_info[QMI_PSM_SENSOR_TYPE_MAX_V01];

  /* Optional */
  /*  Sensor Algo Information */
  uint8_t algo_info_valid;  /**< Must be set to true if algo_info is being passed */
  uint32_t algo_info_len;  /**< Must be set to # of elements in algo_info */
  psm_sensor_algo_info_v01 algo_info[QMI_PSM_SENSOR_TYPE_MAX_V01];

  /* Optional */
  /*  Sensor Flight Status */
  uint8_t sensor_flight_status_valid;  /**< Must be set to true if sensor_flight_status is being passed */
  psm_sensor_flight_status_enum_v01 sensor_flight_status;

  /* Optional */
  /*  Sensor Tilt Angle */
  uint8_t sensor_tilt_angle_valid;  /**< Must be set to true if sensor_tilt_angle is being passed */
  float sensor_tilt_angle;

  /* Optional */
  /*  Motion Change Detector Current State */
  uint8_t mcd_curr_state_valid;  /**< Must be set to true if mcd_curr_state is being passed */
  psm_sensor_mcd_state_enum_v01 mcd_curr_state;
  /**<   Values: \n 
      - SNS_MCD_TRACKER_STATIONARY (1) --  Stationary \n
      - SNS_MCD_TRACKER_SIG_MOTION (2) --  Significant motion 
 */

  /* Optional */
  /*  Motion Change Detector Previous State */
  uint8_t mcd_prev_state_valid;  /**< Must be set to true if mcd_prev_state is being passed */
  psm_sensor_mcd_state_enum_v01 mcd_prev_state;
  /**<   Values: \n 
      - SNS_MCD_TRACKER_STATIONARY (1) --  Stationary \n
      - SNS_MCD_TRACKER_SIG_MOTION (2) --  Significant motion 
 */
}psm_get_sensor_data_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Resets the sensor configuration on modem */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}psm_reset_sensor_config_data_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Resets the sensor configuration on modem */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}psm_reset_sensor_config_data_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Queries the information reported by the last sensor policy met indication. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}psm_get_last_policy_met_info_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Queries the information reported by the last sensor policy met indication. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Policy Met Mask */
  uint8_t policy_met_sensor_mask_valid;  /**< Must be set to true if policy_met_sensor_mask is being passed */
  psm_sensor_id_mask_v01 policy_met_sensor_mask;
  /**<   Values: \n
      - PSM_MASK_SENSOR_ID_ALS (0x0001) --  ALS \n 
      - PSM_MASK_SENSOR_ID_PRESSURE (0x0002) --  Pressure \n
      - PSM_MASK_SENSOR_ID_HUMIDITY (0x0004) --  Humidity \n
      - PSM_MASK_SENSOR_ID_TEMPERATURE (0x0008) --  Temperature \n
      - PSM_MASK_SENSOR_ID_AMD (0x0010) --  AMD \n
      - PSM_MASK_SENSOR_ID_SMD (0x0020) --  SMD \n
      - PSM_MASK_SENSOR_ID_MOTION_DETECT (0x0040) --  Motion detection \n
      - PSM_MASK_SENSOR_ID_FMD (0x0080) --  FMD \n
      - PSM_MASK_SENSOR_ID_FALL_DETECT (0x0100) --  Fall detection \n
      - PSM_MASK_SENSOR_ID_TILT_DETECT (0x0200) --  Tilt detection \n 
      - PSM_MASK_SENSOR_ID_MCD (0x0400) --  Motion change detector 
 */

  /* Optional */
  /*  Batch Full Mask */
  uint8_t batch_full_sensor_mask_valid;  /**< Must be set to true if batch_full_sensor_mask is being passed */
  psm_sensor_id_mask_v01 batch_full_sensor_mask;
  /**<   Values: \n
      - PSM_MASK_SENSOR_ID_ALS (0x0001) --  ALS \n 
      - PSM_MASK_SENSOR_ID_PRESSURE (0x0002) --  Pressure \n
      - PSM_MASK_SENSOR_ID_HUMIDITY (0x0004) --  Humidity \n
      - PSM_MASK_SENSOR_ID_TEMPERATURE (0x0008) --  Temperature \n
      - PSM_MASK_SENSOR_ID_AMD (0x0010) --  AMD \n
      - PSM_MASK_SENSOR_ID_SMD (0x0020) --  SMD \n
      - PSM_MASK_SENSOR_ID_MOTION_DETECT (0x0040) --  Motion detection \n
      - PSM_MASK_SENSOR_ID_FMD (0x0080) --  FMD \n
      - PSM_MASK_SENSOR_ID_FALL_DETECT (0x0100) --  Fall detection \n
      - PSM_MASK_SENSOR_ID_TILT_DETECT (0x0200) --  Tilt detection \n 
      - PSM_MASK_SENSOR_ID_MCD (0x0400) --  Motion change detector */

  /* Optional */
  /*  Sensor Fail Cond Met Mask */
  uint8_t sensor_fail_cond_met_mask_valid;  /**< Must be set to true if sensor_fail_cond_met_mask is being passed */
  psm_sensor_id_mask_v01 sensor_fail_cond_met_mask;
  /**<   Values: \n
      - PSM_MASK_SENSOR_ID_ALS (0x0001) --  ALS \n 
      - PSM_MASK_SENSOR_ID_PRESSURE (0x0002) --  Pressure \n
      - PSM_MASK_SENSOR_ID_HUMIDITY (0x0004) --  Humidity \n
      - PSM_MASK_SENSOR_ID_TEMPERATURE (0x0008) --  Temperature \n
      - PSM_MASK_SENSOR_ID_AMD (0x0010) --  AMD \n
      - PSM_MASK_SENSOR_ID_SMD (0x0020) --  SMD \n
      - PSM_MASK_SENSOR_ID_MOTION_DETECT (0x0040) --  Motion detection \n
      - PSM_MASK_SENSOR_ID_FMD (0x0080) --  FMD \n
      - PSM_MASK_SENSOR_ID_FALL_DETECT (0x0100) --  Fall detection \n
      - PSM_MASK_SENSOR_ID_TILT_DETECT (0x0200) --  Tilt detection \n 
      - PSM_MASK_SENSOR_ID_MCD (0x0400) --  Motion change detector */
}psm_get_last_policy_met_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Indication Message; Notifies the control point whenever the sensor policy is met or whether the batch is full. */
typedef struct {

  /* Optional */
  /*  Policy Met Mask */
  uint8_t policy_met_sensor_mask_valid;  /**< Must be set to true if policy_met_sensor_mask is being passed */
  psm_sensor_id_mask_v01 policy_met_sensor_mask;
  /**<   Values: \n
      - PSM_MASK_SENSOR_ID_ALS (0x0001) --  ALS \n 
      - PSM_MASK_SENSOR_ID_PRESSURE (0x0002) --  Pressure \n
      - PSM_MASK_SENSOR_ID_HUMIDITY (0x0004) --  Humidity \n
      - PSM_MASK_SENSOR_ID_TEMPERATURE (0x0008) --  Temperature \n
      - PSM_MASK_SENSOR_ID_AMD (0x0010) --  AMD \n
      - PSM_MASK_SENSOR_ID_SMD (0x0020) --  SMD \n
      - PSM_MASK_SENSOR_ID_MOTION_DETECT (0x0040) --  Motion detection \n
      - PSM_MASK_SENSOR_ID_FMD (0x0080) --  FMD \n
      - PSM_MASK_SENSOR_ID_FALL_DETECT (0x0100) --  Fall detection \n
      - PSM_MASK_SENSOR_ID_TILT_DETECT (0x0200) --  Tilt detection \n 
      - PSM_MASK_SENSOR_ID_MCD (0x0400) --  Motion change detector 
 */

  /* Optional */
  /*  Batch Full Mask */
  uint8_t batch_full_sensor_mask_valid;  /**< Must be set to true if batch_full_sensor_mask is being passed */
  psm_sensor_id_mask_v01 batch_full_sensor_mask;
  /**<   Values: \n
      - PSM_MASK_SENSOR_ID_ALS (0x0001) --  ALS \n 
      - PSM_MASK_SENSOR_ID_PRESSURE (0x0002) --  Pressure \n
      - PSM_MASK_SENSOR_ID_HUMIDITY (0x0004) --  Humidity \n
      - PSM_MASK_SENSOR_ID_TEMPERATURE (0x0008) --  Temperature \n
      - PSM_MASK_SENSOR_ID_AMD (0x0010) --  AMD \n
      - PSM_MASK_SENSOR_ID_SMD (0x0020) --  SMD \n
      - PSM_MASK_SENSOR_ID_MOTION_DETECT (0x0040) --  Motion detection \n
      - PSM_MASK_SENSOR_ID_FMD (0x0080) --  FMD \n
      - PSM_MASK_SENSOR_ID_FALL_DETECT (0x0100) --  Fall detection \n
      - PSM_MASK_SENSOR_ID_TILT_DETECT (0x0200) --  Tilt detection \n 
      - PSM_MASK_SENSOR_ID_MCD (0x0400) --  Motion change detector 
 */

  /* Optional */
  /*  Sensor Fail Cond Met Mask */
  uint8_t sensor_fail_cond_met_mask_valid;  /**< Must be set to true if sensor_fail_cond_met_mask is being passed */
  psm_sensor_id_mask_v01 sensor_fail_cond_met_mask;
  /**<   Values: \n
      - PSM_MASK_SENSOR_ID_ALS (0x0001) --  ALS \n 
      - PSM_MASK_SENSOR_ID_PRESSURE (0x0002) --  Pressure \n
      - PSM_MASK_SENSOR_ID_HUMIDITY (0x0004) --  Humidity \n
      - PSM_MASK_SENSOR_ID_TEMPERATURE (0x0008) --  Temperature \n
      - PSM_MASK_SENSOR_ID_AMD (0x0010) --  AMD \n
      - PSM_MASK_SENSOR_ID_SMD (0x0020) --  SMD \n
      - PSM_MASK_SENSOR_ID_MOTION_DETECT (0x0040) --  Motion detection \n
      - PSM_MASK_SENSOR_ID_FMD (0x0080) --  FMD \n
      - PSM_MASK_SENSOR_ID_FALL_DETECT (0x0100) --  Fall detection \n
      - PSM_MASK_SENSOR_ID_TILT_DETECT (0x0200) --  Tilt detection \n 
      - PSM_MASK_SENSOR_ID_MCD (0x0400) --  Motion change detector 
 */

  /* Optional */
  /*  Tracker Database Status */
  uint8_t is_tracker_db_full_valid;  /**< Must be set to true if is_tracker_db_full is being passed */
  uint8_t is_tracker_db_full;
  /**<   Indicates whether the tracker database (tracker_db) is full or not. \n
        - 0x00 -- FALSE -- Not full \n
        - 0x01 -- TRUE -- Full
  */

  /* Optional */
  /*  Timestamp  */
  uint8_t timestamp_valid;  /**< Must be set to true if timestamp is being passed */
  uint64_t timestamp;
  /**<   Timestamp at which the policy is met.
  */
}psm_sensor_policy_met_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Trigger the FMD sensor to start collect data. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}psm_get_fmd_sns_sample_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Trigger the FMD sensor to start collect data. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}psm_get_fmd_sns_sample_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Triggers the sensor framework to test the physical sensor. */
typedef struct {

  /* Mandatory */
  /*  Sensor ID */
  psm_sensor_type_id_v01 sensor_id;
  /**<   Sensor type for which the configuration is sent. Values:\n
      - PSM_SENSOR_TYPE_ALS (1) --  Sensor information about ambient light detection \n  
      - PSM_SENSOR_TYPE_PRESSURE (2) --  Sensor information about pressure \n  
      - PSM_SENSOR_TYPE_HUMIDITY (3) --  Sensor information about humidity \n  
      - PSM_SENSOR_TYPE_TEMPERATURE (4) --  Sensor information about temperature \n  
      - PSM_SENSOR_TYPE_AMD (5) --  Sensor information about absolute motion detection used to enable different                                           motion algorithms based on the use case \n  
      - PSM_SENSOR_TYPE_SMD (6) --  Sensor information about significant motion detection. \n  
      - PSM_SENSOR_TYPE_MOTION_DETECT (7) --  Sensor information about physical motion detection\n 
      - PSM_SENSOR_TYPE_FMD (8) --  Sensor information about flight motion detection\n 
      - PSM_SENSOR_TYPE_FALL_DETECT (9) --  Sensor information about fall detection\n 
      - PSM_SENSOR_TYPE_TILT_DETECT (10) --  Sensor information about tilt detection\n 
      - PSM_SENSOR_TYPE_ACCEL (11) --  Accelerometer sensor information \n 
      - PSM_SENSOR_TYPE_MCD (12) --  Sensor information about motion change detection\n 
      - PSM_SENSOR_TYPE_ALL (255) --  Sensor information about multiple sensors  
 */

  /* Optional */
  /*  Physical Sensor Test Type  */
  uint8_t physical_sns_test_valid;  /**< Must be set to true if physical_sns_test is being passed */
  phy_sns_test_type_v01 physical_sns_test;
  /**<   Defines the test type. Values:\n
      - PSM_SNS_PHYSICAL_SENSOR_TEST_TYPE_SW (0) --  Software test \n 
      - PSM_SNS_PHYSICAL_SENSOR_TEST_TYPE_HW (1) --  Sensor hardware test \n 
      - PSM_SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY (2) --  Factory test used for sensor calibration \n 
      - PSM_SNS_PHYSICAL_SENSOR_TEST_TYPE_COM (3) --  Communication bus test \n 
 */
}psm_phy_sns_test_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Triggers the sensor framework to test the physical sensor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}psm_phy_sns_test_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Indication Message; Triggers the sensor framework to test the physical sensor. */
typedef struct {

  /* Optional */
  /*  Result of Physical Sensor Test */
  uint8_t test_passed_valid;  /**< Must be set to true if test_passed is being passed */
  uint8_t test_passed;
  /**<  Defines the result of the test. Values:\n
    - 0 -- Test failure \n 
    - 1 -- Test success
  */

  /* Optional */
  /*  Physical Sensor Test Type  */
  uint8_t physical_sns_test_valid;  /**< Must be set to true if physical_sns_test is being passed */
  phy_sns_test_type_v01 physical_sns_test;
  /**<   Defines the test type. Values:\n
      - PSM_SNS_PHYSICAL_SENSOR_TEST_TYPE_SW (0) --  Software test \n 
      - PSM_SNS_PHYSICAL_SENSOR_TEST_TYPE_HW (1) --  Sensor hardware test \n 
      - PSM_SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY (2) --  Factory test used for sensor calibration \n 
      - PSM_SNS_PHYSICAL_SENSOR_TEST_TYPE_COM (3) --  Communication bus test \n 
 */
}psm_phy_sns_test_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Collect test data */
typedef struct {

  /* Mandatory */
  /*  Sensor ID */
  psm_sensor_type_id_v01 sensor_id;
  /**<   Sensor type for which the configuration is sent. Values:\n
      - PSM_SENSOR_TYPE_ALS (1) --  Sensor information about ambient light detection \n  
      - PSM_SENSOR_TYPE_PRESSURE (2) --  Sensor information about pressure \n  
      - PSM_SENSOR_TYPE_HUMIDITY (3) --  Sensor information about humidity \n  
      - PSM_SENSOR_TYPE_TEMPERATURE (4) --  Sensor information about temperature \n  
      - PSM_SENSOR_TYPE_AMD (5) --  Sensor information about absolute motion detection used to enable different                                           motion algorithms based on the use case \n  
      - PSM_SENSOR_TYPE_SMD (6) --  Sensor information about significant motion detection. \n  
      - PSM_SENSOR_TYPE_MOTION_DETECT (7) --  Sensor information about physical motion detection\n 
      - PSM_SENSOR_TYPE_FMD (8) --  Sensor information about flight motion detection\n 
      - PSM_SENSOR_TYPE_FALL_DETECT (9) --  Sensor information about fall detection\n 
      - PSM_SENSOR_TYPE_TILT_DETECT (10) --  Sensor information about tilt detection\n 
      - PSM_SENSOR_TYPE_ACCEL (11) --  Accelerometer sensor information \n 
      - PSM_SENSOR_TYPE_MCD (12) --  Sensor information about motion change detection\n 
      - PSM_SENSOR_TYPE_ALL (255) --  Sensor information about multiple sensors  
 */

  /* Mandatory */
  /*  Enable Sensor */
  uint8_t is_enabled;
  /**<   Enable or disable the sensor. values:\n
    - 0 -- Disable \n 
    - 1 -- Enable
     */

  /* Optional */
  /*  Sample Rate */
  uint8_t sample_rate_valid;  /**< Must be set to true if sample_rate is being passed */
  float sample_rate;
  /**<   Rate at which the physical sensor is queried. 
  */

  /* Optional */
  /*  Report Rate */
  uint8_t report_rate_valid;  /**< Must be set to true if report_rate is being passed */
  uint32_t report_rate;
  /**<   Rate at which report indications are sent to applications. \n
        Minimum rate is one second.  
    */
}psm_phy_sns_test_get_data_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Collect test data */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}psm_phy_sns_test_get_data_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Indication Message; Collect test data */
typedef struct {

  /* Mandatory */
  /*  Sensor ID */
  psm_sensor_type_id_v01 sensor_id;
  /**<   Sensor type for which the configuration is sent. Values:\n
      - PSM_SENSOR_TYPE_ALS (1) --  Sensor information about ambient light detection \n  
      - PSM_SENSOR_TYPE_PRESSURE (2) --  Sensor information about pressure \n  
      - PSM_SENSOR_TYPE_HUMIDITY (3) --  Sensor information about humidity \n  
      - PSM_SENSOR_TYPE_TEMPERATURE (4) --  Sensor information about temperature \n  
      - PSM_SENSOR_TYPE_AMD (5) --  Sensor information about absolute motion detection used to enable different                                           motion algorithms based on the use case \n  
      - PSM_SENSOR_TYPE_SMD (6) --  Sensor information about significant motion detection. \n  
      - PSM_SENSOR_TYPE_MOTION_DETECT (7) --  Sensor information about physical motion detection\n 
      - PSM_SENSOR_TYPE_FMD (8) --  Sensor information about flight motion detection\n 
      - PSM_SENSOR_TYPE_FALL_DETECT (9) --  Sensor information about fall detection\n 
      - PSM_SENSOR_TYPE_TILT_DETECT (10) --  Sensor information about tilt detection\n 
      - PSM_SENSOR_TYPE_ACCEL (11) --  Accelerometer sensor information \n 
      - PSM_SENSOR_TYPE_MCD (12) --  Sensor information about motion change detection\n 
      - PSM_SENSOR_TYPE_ALL (255) --  Sensor information about multiple sensors  
 */

  /* Optional */
  /*  Sensor Data Response */
  uint8_t sensor_data_rsp_valid;  /**< Must be set to true if sensor_data_rsp is being passed */
  uint32_t sensor_data_rsp_len;  /**< Must be set to # of elements in sensor_data_rsp */
  float sensor_data_rsp[QMI_PSM_SENSOR_CONFIG_MAX_V01];
  /**<   Sensor data response.
  */

  /* Optional */
  /*  Timestamp Of Data */
  uint8_t timestamp_valid;  /**< Must be set to true if timestamp is being passed */
  uint64_t timestamp;
  /**<   Timestamp of the sample.
  */
}psm_phy_sns_test_get_data_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Sends the request to the modem to set the logging level. */
typedef struct {

  /* Mandatory */
  /*  Sensor Logging Level */
  psm_sensor_tracker_log_level_v01 log_enable_level;
  /**<   Logging level for the sensor. Values:\n
      - SNS_TRACKER_LOG_DISABLE (0) --  Logging is disabled \n 
      - SNS_TRACKER_LOG_LEVEL1 (1) --  Logging is enabled and logging level is 1 \n 
      - SNS_TRACKER_LOG_LEVEL2 (2) --  Logging is enabled and logging level is 2 
 */
}psm_sensor_logging_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Sends the request to the modem to set the logging level. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}psm_sensor_logging_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Triggers the sensor framework to get the serial number of the physical sensor. */
typedef struct {

  /* Mandatory */
  /*  Sensor ID */
  psm_sensor_type_id_v01 sensor_id;
  /**<   Sensor type for which the request is sent. Values:\n
      - PSM_SENSOR_TYPE_ALS (1) --  Sensor information about ambient light detection \n  
      - PSM_SENSOR_TYPE_PRESSURE (2) --  Sensor information about pressure \n  
      - PSM_SENSOR_TYPE_HUMIDITY (3) --  Sensor information about humidity \n  
      - PSM_SENSOR_TYPE_TEMPERATURE (4) --  Sensor information about temperature \n  
      - PSM_SENSOR_TYPE_AMD (5) --  Sensor information about absolute motion detection used to enable different                                           motion algorithms based on the use case \n  
      - PSM_SENSOR_TYPE_SMD (6) --  Sensor information about significant motion detection. \n  
      - PSM_SENSOR_TYPE_MOTION_DETECT (7) --  Sensor information about physical motion detection\n 
      - PSM_SENSOR_TYPE_FMD (8) --  Sensor information about flight motion detection\n 
      - PSM_SENSOR_TYPE_FALL_DETECT (9) --  Sensor information about fall detection\n 
      - PSM_SENSOR_TYPE_TILT_DETECT (10) --  Sensor information about tilt detection\n 
      - PSM_SENSOR_TYPE_ACCEL (11) --  Accelerometer sensor information \n 
      - PSM_SENSOR_TYPE_MCD (12) --  Sensor information about motion change detection\n 
      - PSM_SENSOR_TYPE_ALL (255) --  Sensor information about multiple sensors  
 */
}psm_get_sns_serial_number_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Triggers the sensor framework to get the serial number of the physical sensor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Serial number   */
  uint8_t serial_number_valid;  /**< Must be set to true if serial_number is being passed */
  uint32_t serial_number_len;  /**< Must be set to # of elements in serial_number */
  char serial_number[QMI_PSM_SENSOR_SERIAL_NUMBER_MAX_LENGTH_V01];
  /**<   Serial number from the 0x89 register of the physical sensor.
  */
}psm_get_sns_serial_number_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Request Message; Sends a request to the modem to reset the tracker database. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}psm_sensors_reset_tracker_db_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup psm_qmi_messages
    @{
  */
/** Response Message; Sends a request to the modem to reset the tracker database. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}psm_sensors_reset_tracker_db_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_PSM_CFG_PARAMS_CHANGE_IND_V01
//#define REMOVE_QMI_PSM_ENTER_V01
//#define REMOVE_QMI_PSM_GET_CAPABILITY_V01
//#define REMOVE_QMI_PSM_GET_CFG_PARAMS_V01
//#define REMOVE_QMI_PSM_GET_FMD_SNS_SAMPLE_V01
//#define REMOVE_QMI_PSM_GET_LAST_POLICY_MET_INFO_V01
//#define REMOVE_QMI_PSM_GET_SENSOR_CONFIGURATION_V01
//#define REMOVE_QMI_PSM_GET_SENSOR_DATA_V01
//#define REMOVE_QMI_PSM_GET_SNS_SERIAL_NUMBER_V01
//#define REMOVE_QMI_PSM_INDICATION_REGISTER_V01
//#define REMOVE_QMI_PSM_PHY_SNS_TEST_V01
//#define REMOVE_QMI_PSM_PHY_SNS_TEST_GET_DATA_V01
//#define REMOVE_QMI_PSM_RESET_SENSOR_CONFIG_DATA_V01
//#define REMOVE_QMI_PSM_SENSORS_RESET_TRACKER_DB_V01
//#define REMOVE_QMI_PSM_SENSOR_LOGGING_V01
//#define REMOVE_QMI_PSM_SENSOR_POLICY_MET_IND_V01
//#define REMOVE_QMI_PSM_SET_CFG_PARAMS_V01
//#define REMOVE_QMI_PSM_SET_SENSOR_CONFIGURATION_V01
//#define REMOVE_QMI_PSM_SET_SENSOR_CONFIGURATION_EXT_V01
//#define REMOVE_QMI_PSM_STATUS_IND_V01

/*Service Message Definition*/
/** @addtogroup psm_qmi_msg_ids
    @{
  */
#define QMI_PSM_INDICATION_REGISTER_REQ_V01 0x0001
#define QMI_PSM_INDICATION_REGISTER_RESP_V01 0x0001
#define QMI_PSM_GET_CFG_PARAMS_REQ_V01 0x0002
#define QMI_PSM_GET_CFG_PARAMS_RESP_V01 0x0002
#define QMI_PSM_ENTER_REQ_V01 0x0003
#define QMI_PSM_ENTER_RESP_V01 0x0003
#define QMI_PSM_STATUS_IND_V01 0x0004
#define QMI_PSM_SET_CFG_PARAMS_REQ_V01 0x0005
#define QMI_PSM_SET_CFG_PARAMS_RESP_V01 0x0005
#define QMI_PSM_CFG_PARAMS_CHANGE_IND_V01 0x0006
#define QMI_PSM_GET_CAPABILITY_REQ_V01 0x0007
#define QMI_PSM_GET_CAPABILITY_RESP_V01 0x0007
#define QMI_PSM_SET_SENSOR_CONFIGURATION_REQ_V01 0x0008
#define QMI_PSM_SET_SENSOR_CONFIGURATION_RESP_V01 0x0008
#define QMI_PSM_GET_SENSOR_DATA_REQ_V01 0x0009
#define QMI_PSM_GET_SENSOR_DATA_RESP_V01 0x0009
#define QMI_PSM_RESET_SENSOR_CONFIG_DATA_REQ_V01 0x000A
#define QMI_PSM_RESET_SENSOR_CONFIG_DATA_RESP_V01 0x000A
#define QMI_PSM_GET_LAST_SENSOR_POLICY_MET_INFO_REQ_V01 0x000B
#define QMI_PSM_GET_LAST_SENSOR_POLICY_MET_INFO_RESP_V01 0x000B
#define QMI_PSM_SENSOR_POLICY_MET_IND_V01 0x000C
#define QMI_PSM_SET_SENSOR_CONFIGURATION_EXT_REQ_V01 0x000D
#define QMI_PSM_SET_SENSOR_CONFIGURATION_EXT_RESP_V01 0x000D
#define QMI_PSM_GET_SENSOR_CONFIGURATION_REQ_V01 0x000E
#define QMI_PSM_GET_SENSOR_CONFIGURATION_RESP_V01 0x000E
#define QMI_PSM_GET_FMD_SNS_SAMPLE_REQ_V01 0x000F
#define QMI_PSM_GET_FMD_SNS_SAMPLE_RESP_V01 0x000F
#define QMI_PSM_PHY_SNS_TEST_REQ_V01 0x0010
#define QMI_PSM_PHY_SNS_TEST_RESP_V01 0x0010
#define QMI_PSM_PHY_SNS_TEST_IND_V01 0x0010
#define QMI_PSM_PHY_SNS_TEST_GET_DATA_REQ_V01 0x0011
#define QMI_PSM_PHY_SNS_TEST_GET_DATA_RESP_V01 0x0011
#define QMI_PSM_PHY_SNS_TEST_GET_DATA_IND_V01 0x0011
#define QMI_PSM_SENSOR_LOGGING_REQ_V01 0x0012
#define QMI_PSM_SENSOR_LOGGING_RESP_V01 0x0012
#define QMI_PSM_GET_SNS_SERIAL_NUMBER_REQ_V01 0x0013
#define QMI_PSM_GET_SNS_SERIAL_NUMBER_RESP_V01 0x0013
#define QMI_PSM_SENSORS_RESET_TRACKER_DB_REQ_V01 0x0014
#define QMI_PSM_SENSORS_RESET_TRACKER_DB_RESP_V01 0x0014
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro psm_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type psm_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define psm_get_service_object_v01( ) \
          psm_get_service_object_internal_v01( \
            PSM_V01_IDL_MAJOR_VERS, PSM_V01_IDL_MINOR_VERS, \
            PSM_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

