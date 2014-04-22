#ifndef SLIM_SSC_PROVIDER_DATA_H
#define SLIM_SSC_PROVIDER_DATA_H
/**
  @file slim_ssc_provider_data.h
  @brief Structures and definitions to store SSC provider data.

  @ingroup slim_SSCProvider
*/
/*============================================================================
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Confidential and Proprietary - Qualcomm Technologies, Inc.

               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/provider/ssc/inc/slim_ssc_provider_data.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qmi_client.h"
#include "slim_provider_data.h"
//! @addtogroup slim_SSCProvider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define SLIM_SSC_SNS_SERVICE_NONE  (-1)    /**< Invalid service */


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef int32 slim_SscSnsServiceEnumType;

/** Enum for supported SSC services with time data.
*/
typedef enum
{
  SLIM_SSC_TS_SERVICE_NONE = -1,             /**< Invalid */
  SLIM_SSC_TS_SERVICE_ACCEL = 0,             /**< Accelerometer */
  SLIM_SSC_TS_SERVICE_ACCEL_TEMP,            /**< Accelerometer temperature */
  SLIM_SSC_TS_SERVICE_GYRO,                  /**< Gyro */
  SLIM_SSC_TS_SERVICE_GYRO_TEMP,             /**< Gyro temperature */
  SLIM_SSC_TS_SERVICE_AMD,                   /**< AMD */
  SLIM_SSC_TS_SERVICE_RMD,                   /**< RMD */
  SLIM_SSC_TS_SERVICE_SMD,                   /**< SMD */
  SLIM_SSC_TS_SERVICE_MOTION_DATA,           /**< Motion data */
  SLIM_SSC_TS_SERVICE_PEDOMETER,             /**< Pedometer */
  SLIM_SSC_TS_SERVICE_DISTANCE_BOUND,        /**< Distance bound */
  SLIM_SSC_TS_SERVICE_MAG_CALIB,             /**< Calibrated magnetometer */
  SLIM_SSC_TS_SERVICE_MAG_UNCALIB,           /**< Uncalibrated magnetometer */
  SLIM_SSC_TS_SERVICE_PED_ALIGNMENT,         /**< Pedestrian alignment */
  SLIM_SSC_TS_SERVICE_BARO,                  /**< Barometer */
  SLIM_SSC_TS_SERVICE_COUNT,
  //! @cond
  SLIM_SSC_TS_SERVICE_MAX     = 2147483647 /* To force a 32 bit signed enum.
                                              Do not change or use */
  //! @endcond
} slim_SscTsServiceEnumType;

/** Enum for supported SSC device sensor services.
*/
typedef enum
{
  eSLIM_SSC_DEVICE_SENSOR_NONE = -1,             /**< Invalid */
  eSLIM_SSC_DEVICE_SENSOR_ACCEL = 0,             /**< Accelerometer */
  eSLIM_SSC_DEVICE_SENSOR_ACCEL_TEMP,            /**< Accelerometer temperature */
  eSLIM_SSC_DEVICE_SENSOR_GYRO,                  /**< Gyro */
  eSLIM_SSC_DEVICE_SENSOR_GYRO_TEMP,             /**< Gyro temperature */
  eSLIM_SSC_DEVICE_SENSOR_MAG_CALIB,             /**< Calibrated magnetometer */
  eSLIM_SSC_DEVICE_SENSOR_MAG_UNCALIB,           /**< Uncalibrated magnetometer */
  eSLIM_SSC_DEVICE_SENSOR_BARO,                  /**< Barometer */
  eSLIM_SSC_DEVICE_SENSOR_COUNT,
  //! @cond
  eSLIM_SSC_DEVICE_SENSOR_MAX     = 2147483647 /* To force a 32 bit signed enum.
                                                  Do not change or use */
  //! @endcond
} slim_SscDeviceSensorEnumType;

/** Enum for SSC QMD connection types */
typedef enum
{
  SSC_QMD_AMD,
  SSC_QMD_RMD,
  SSC_QMD_MOTION_DATA_AMD,
  SSC_QMD_MOTION_DATA_RMD,

  //! @cond
  SSC_QMD_MAX = 2147483647 /* To force a 32 bit enum. Do not change or use */
  //! @endcond
} slim_SscQmdConnectionType;

/** Enum for supported SSC SMGR Report Ids.
*/
typedef enum
{
  eSLIM_SSC_SNS_REPORT_NONE = -1,         /**< Invalid */
  eSLIM_SSC_SNS_REPORT_ACCEL = 0,         /**< Accelerometer */
  eSLIM_SSC_SNS_REPORT_GYRO,              /**< Gyro */
  eSLIM_SSC_SNS_REPORT_MAG_UNCAL,         /**< Uncal magnetometer */
  eSLIM_SSC_SNS_REPORT_ACCEL_GYRO_MAG_UNCAL,  /**< Accelerometer, Gyro, Uncal magnetometer */
  eSLIM_SSC_SNS_REPORT_ACCEL_TEMP,        /**< Accelerometer temperature */
  eSLIM_SSC_SNS_REPORT_GYRO_TEMP,         /**< Gyro temperature */
  eSLIM_SSC_SNS_REPORT_MAG_CALIB,         /**< Calibrated magnetometer */
  eSLIM_SSC_SNS_REPORT_BARO,              /**< Barometer */
  eSLIM_SSC_SNS_REPORT_COUNT,
  //! @cond
  eSLIM_SSC_SNS_REPORT_MAX     = 2147483647 /* To force a 32 bit signed enum.
                                                  Do not change or use */
  //! @endcond
} slim_SscSnsServiceReportEnumType;

/*! Struct for SSC connection QMI objects */
typedef struct slim_SscConnectionDataStructType
{
  boolean b_SscOpen;                            /*!< SSC connection status */
  qmi_client_type p_ServerHandle;               /*!< SSC QMI server handle */
  qmi_idl_service_object_type p_ServiceObject;  /*!< SSC QMI service object */
  qmi_client_os_params z_OsParams;              /*!< SSC QMI OS params */
} slim_SscConnectionDataStructType;

/*! Struct for SSC SAM connection */
typedef struct slim_SscSamConnectionDataStructType
{
  slim_SscConnectionDataStructType z_SscConnection; /*!< SSC connection data */
  uint8 u_InstanceId;                               /*!< Algorithm instance id */
} slim_SscSamConnectionDataStructType;

/*! Struct for SSC Motion data connection */
typedef struct slim_SscMotionDataConnectionDataStructType
{
  slim_SscSamConnectionDataStructType z_AmdData;  /*!< SSC connection data for AMD data service */
  slimQmdMotionStateEnumT e_AmdState;             /*!< Last received AMD state */
  slim_SscSamConnectionDataStructType z_RmdData;  /*!< SSC connection data for RMD data service */
  slimQmdMotionStateEnumT e_RmdState;             /*!< Last received RMD state */
} slim_SscMotionDataConnectionDataStructType;

/*! Struct for SSC Pedometer connection */
typedef struct slim_SscPedometerConnectionDataStructType
{
  slim_SscSamConnectionDataStructType z_SamData;  /*!< SAM connection data for pedometer service */
  uint64 t_ResetTime;                             /*!< Last pedometer reset time in milliseconds */
} slim_SscPedometerConnectionDataStructType;

/*! Struct for SSC SMGR device sensor session data */
typedef struct
{
  boolean v_ServiceEnable;     /*!< Flag to indicate if service is enabled */
  uint16 w_SamplingRateHz;     /*!< Sampling rate in Hz */
  uint16 w_ReportingRateHz;    /*!< Reporting rate in Hz */
} slim_SscSnsDataStructType;

/*! Struct for SSC SMGR connection */
typedef struct
{
  slim_SscConnectionDataStructType      z_SscConnections[eSLIM_SSC_DEVICE_SENSOR_COUNT];              /*!< SSC connection data */
  slim_ProviderThrottleStatusStructType z_ThrottleData;               /*!< SSC Throttle data */
  slim_SscSnsDataStructType z_SnsData[eSLIM_SSC_DEVICE_SENSOR_COUNT]; /*!< SSC device sensor session data */
} slim_SscDeviceSensorConnectionDataStructType;

/*! Struct for client SSC data */
typedef struct slim_SscClientStructType
{
  slimClientHandleT p_Handle; /*!< SLIM client handle */
  boolean b_Reserved;         /*!< Flag if the entry is in use */
  slim_ServiceMtDataStructType z_ServiceMtData[SLIM_SSC_TS_SERVICE_COUNT]; /*!< Timesync data */
  slim_SscDeviceSensorConnectionDataStructType z_SensorData;        /*!< SSC connection data for sensor data service */
  slim_SscSamConnectionDataStructType z_AmdData;            /*!< SSC connection data for AMD data service */
  slim_SscSamConnectionDataStructType z_RmdData;            /*!< SSC connection data for RMD data service */
  slim_SscSamConnectionDataStructType z_SmdData;            /*!< SSC connection data for SMD data service */
  slim_SscPedometerConnectionDataStructType z_PedometerData;/*!< SSC connection data for pedometer data service */
  slim_SscMotionDataConnectionDataStructType z_MotionData;  /*!< SSC connection data for motion data service */
  slim_SscSamConnectionDataStructType z_DbData;             /*!< SSC connection data for Distance Bound service */
  slim_SscSamConnectionDataStructType z_PedAlignmentData;   /*!< SSC connection data for pedestrian alignment service */
} slim_SscClientStructType;


/*! Struct for SLIM service provider entry (SSC) */
typedef struct slim_SscProviderStructType
{
  /*! Provider interface function table */
  const slim_ServiceProviderInterfaceStructType *pz_Vtbl;

  /*! Mask of found SNS services */
  uint32 q_FoundSnsServicesMask;

  /*! Notifiers for the SNS service availability */
//  slim_SscServiceNotifierDataStructType z_Notifiers[SLIM_SSC_SNS_SERVICE_COUNT];

  /*! Rollover count of SSC time */
  uint32 q_TimeTickRolloverCount;
  /*! Timestamp of latest time update */
  uint32 q_LatestTimeTick;

  slim_SscConnectionDataStructType z_Time;
  /*!< SSC connection data for time service */
  slim_SscClientStructType z_SscConnection[SLIM_CLIENT_MAX_COUNT];
  /*!< SSC connection structures */

  slim_MutexStructType z_Mutex;
  /*!< SSC Provider mutex */
} slim_SscProviderStructType;
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


/**
@brief Returns pointer to SSC SMGR sensor session data.

Function returns pointer to SSC SMGR sensor session data.

@param[in] pz_SscData Pointer to the client device sensor data.
@param[in] e_Service  SLIM service.

@return Pointer to SSC SMGR sensor session data.
@retval NULL if not found.
*/
slim_SscSnsDataStructType *slim_GetSscSnsData
(
   const slim_SscDeviceSensorConnectionDataStructType* pz_SscData,
   slimServiceEnumT e_Service
);

/**
@brief Returns SLIM service type for input SSC Device Sensor type.

Function returns SLIM service type for input SSC Device Sensor type.

@param[in] e_DeviceService  SSC Device Sensor type.

@return SLIM service type
*/
slimServiceEnumT slim_GetSlimServiceForSnsDeviceSensor
(
   slim_SscDeviceSensorEnumType e_DeviceSensor
);

//! @}

#endif /* #ifndef SLIM_PROVIDER_DATA_H */
