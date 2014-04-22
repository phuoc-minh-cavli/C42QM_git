#ifndef SLIM_UTILS_H
#define SLIM_UTILS_H
/**
  @file
  @brief Utility functions and types for different SLIM modules.

  @ingroup slim_CoreUtils
*/

/*============================================================================
               Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Proprietary and Confidential.

               Copyright (c) 2014-2017 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/common/core/inc/slim_utils.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stringl.h>
#include "slim_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_CoreUtils
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
//! @name Preprocessor Definitions and Constants
//! @{

/*! Sensor service count */
#define SLIM_SERVICE_COUNT (eSLIM_SERVICE_LAST + 1)

/*! Get client id value */
#define SLIM_CLIENT_ID(p_Handle) ((uint8_t)((uintptr_t)(p_Handle) & 0xFF))

/*! Get client instance value */
#define SLIM_CLIENT_INSTANCE(p_Handle) \
  ((uint16_t)(((uintptr_t)(p_Handle) >> 16) & 0xFF))

/*! Construct client handle from client and instance */
#define SLIM_CLIENT_MAKE_HANDLE(q_ClientId, q_InstanceId) \
  (((uint32_t)(q_ClientId) & 0xFF) | ((uint32_t)(q_InstanceId) << 16))

/* Max number of throttle data for each provider */
#define SLIM_MAX_THROTTLE_INSTANCE_NUM       SLIM_SERVICE_COUNT

/* Types of throttling thresholds */
#define SLIM_THROTTLE_THRESHOLD_TYPE_FIXED   (0)  /* Fixed thresholding */
#define SLIM_THROTTLE_THRESHOLD_TYPE_RATE    (1)  /* Rate-based thresholding */


//! @}
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
//! @name Type Declarations
//! @{

/*! Struct for SLIM client transaction data */
typedef struct slim_ClientTxnDataStructType
{
  slimClientHandleT p_Handle; /*!< Client handle */
  uint8 u_ClientTxnId;        /*!< Transaction id provided by client */
  slimNotifyCallbackFunctionT fn_Callback; /*!< Callback function */
  uint64 t_CallbackData;                   /*!< Callback function parameter */
} slim_ClientTxnDataStructType;

/*! Struct for throttle Report data */
typedef struct
{
  uint64 t_LastReportTimestampMs;      /*!< Last timestamp data was received. */
  uint64 t_ReportIntervalThresholdMs;  /*!< Threshold value for data dispatching. */
  uint64 t_AvgReportIntervalMs;        /*!< Moving average for report interval. */
  int32  l_ServiceTxnId;               /*!< Most recent SLIM Service transaction id. */
} slim_ProviderThrottleDataStructType;

/*! Struct for throttle data array */
typedef struct 
{
  slim_ProviderThrottleDataStructType z_Data[SLIM_MAX_THROTTLE_INSTANCE_NUM];
} slim_ProviderThrottleStatusStructType;

/*! Struct for SLIM API open request */
typedef struct
{
  slimClientHandleT p_Handle;              /*!< Client handle */
  slimNotifyCallbackFunctionT fn_Callback; /*!< Client callback */
  slimOpenFlagsT q_OpenFlags;              /*!< Client flags */
  uint64 t_CallbackData;                   /*!< Client callback data */
} slim_OpenTxnStructType;

/*! Struct for SLIM API close request */
typedef struct
{
  slim_ClientTxnDataStructType z_TxnData; /*!< Transaction data */
} slim_CloseTxnStructType;

/*! Struct for SLIM enable sensor data transaction */
typedef struct slim_EnableSensorDataTxnStructType
{
  slim_ClientTxnDataStructType z_TxnData;       /*!< Transaction data */
  slimEnableSensorDataRequestStructT z_Request; /*!< Sensor configuration data */
} slim_EnableSensorDataTxnStructType;

/*! Struct for SLIM enable motion data transaction */
typedef struct slim_EnableMotionDataTxnStructType
{
  slim_ClientTxnDataStructType z_TxnData;        /*!< Transaction data */
  slimEnableMotionDataRequestStructT z_Request;  /*!< Motion data configuration */
} slim_EnableMotionDataTxnStructType;

/*! Struct for SLIM enable pedometer transaction */
typedef struct slim_EnablePedometerTxnStructType
{
  slim_ClientTxnDataStructType z_TxnData;       /*!< Transaction data */
  slimEnablePedometerRequestStructT z_Request;  /*!< Pedometer configuration */
} slim_EnablePedometerTxnStructType;

/*! Struct for SLIM enable AMD/RMD data transaction */
typedef struct slim_EnableQmdDataTxnStructType
{
  slim_ClientTxnDataStructType z_TxnData;        /*!< Transaction data */
  slimEnableQmdDataRequestStructT z_Request;     /*!< QMD data configuration */
} slim_EnableQmdDataTxnStructType;

/*! Struct for SLIM enable SMD data transaction */
typedef struct slim_EnableSmdDataTxnStructType
{
  slim_ClientTxnDataStructType z_TxnData;        /*!< Transaction data */
  slimEnableSmdDataRequestStructT z_Request;     /*!< SMD data configuration */
} slim_EnableSmdDataTxnStructType;

/*! Struct for SLIM enable distance bound data transaction */
typedef struct slim_EnableDistanceBoundTxnStructType
{
  slim_ClientTxnDataStructType z_TxnData;           /*!< Transaction data */
  slimEnableDistanceBoundRequestStructT z_Request;  /*!< DB configuration */
} slim_EnableDistanceBoundTxnStructType;

/*! Struct for SLIM set distance bound data transaction */
typedef struct slim_SetDistanceBoundTxnStructType
{
  slim_ClientTxnDataStructType z_TxnData;           /*!< Transaction data */
  slimSetDistanceBoundRequestStructT z_Request;     /*!< DB configuration */
} slim_SetDistanceBoundTxnStructType;

/*! Struct for SLIM get distance bound transaction */
typedef struct slim_GetDistanceBoundTxnStructType
{
  slim_ClientTxnDataStructType z_TxnData;          /*!< Transaction data */
} slim_GetDistanceBoundTxnStructType;

/*! Struct for SLIM enable vehicle data transaction */
typedef struct slim_EnableVehicleDataTxnStructType
{
  slim_ClientTxnDataStructType z_TxnData;        /*!< Transaction data */
  slimEnableVehicleDataRequestStructT z_Request; /*!< Vehicle data configuration */
} slim_EnableVehicleDataTxnStructType;

/*! Struct for SLIM enable pedestrian alignment transaction */
typedef struct slim_EnablePedestrianAlignmentTxnStructType
{
  slim_ClientTxnDataStructType z_TxnData;                /*!< Transaction data */
  slimEnablePedestrianAlignmentRequestStructT z_Request; /*!< Pedestrian alignment
                                                              configuration */
} slim_EnablePedestrianAlignmentTxnStructType;

/*! Struct for SLIM enable magnetic field data transaction */
typedef struct slim_EnableMagneticFieldDataTxnStructType
{
  slim_ClientTxnDataStructType z_TxnData;               /*!< Transaction data */
  slimEnableMagneticFieldDataRequestStructT z_Request;  /*!< Magnetic field data configuration */
} slim_EnableMagneticFieldDataTxnStructType;

/*! Struct for provider get time request parameters*/
typedef struct
{
  uint64 referenceTimeMs;
  /**< Client can use this field to store the local reference time in
       milliseconds. It is echoed back in the response. */

} slimGetProviderTimeRequestStructT;

/*! Struct for provider get time response */
typedef struct
{
  slimServiceEnumT         service;
  /**< SLIM service for the time response. */

  slimServiceProviderEnumT provider;
  /**< Time response provider. One of: \n
         eSLIM_SERVICE_PROVIDER_DEFAULT
         eSLIM_SERVICE_PROVIDER_SSC \n
         eSLIM_SERVICE_PROVIDER_SAMLITE \n
         eSLIM_SERVICE_PROVIDER_NATIVE */

  uint64                   referenceTimeMs;
  /**< Reference time provided by client in the request. */

  uint64                   remoteReceiveTimeMs;
  /**< Provider receive time in milliseconds. */

  uint64                   remoteTransmitTimeMs;
  /**< Provider transmit time in milliseconds. */

} slimGetProviderTimeResponseStructT;

/*! Struct for SLIM get provider time request transaction */
typedef struct slim_GetProviderTimeRequestTxnStructType
{
  slim_ClientTxnDataStructType z_TxnData;           /*!< Transaction data */
  slimGetProviderTimeRequestStructT z_Request;      /*!< Time request */
} slim_GetProviderTimeRequestTxnStructType;

//! Struct for SLIM provider configuration change transaction */
typedef struct slim_ProviderConfigChangeStructType
{
  slimServiceProviderMaskT q_ProviderMask;        /*!< Provider */
  slimAvailableServiceMaskT q_ServiceMask; /*!< Changed service mask */
} slim_ProviderConfigChangeStructType;

//! @}
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
//! @name Function Declarations
//! @{

/**
@brief Converts slow clock ticks to milliseconds.

Function converts given slow clock ticks value to milliseconds. Rounds the
result to the nearest integer.

@param  t_TimeInClockTicks: Slow clock value in ticks.
@return Clock value in milliseconds.
*/
uint64 slim_TimeTickToMillisecond
(
  uint64 t_TimeInClockTicks
);

/**
@brief Converts slow clock ticks to milliseconds.

Function converts given slow clock ticks value to milliseconds. Rounds the
result to the floor integer.

@param  t_TimeInClockTicks: Slow clock value in ticks.
@return Clock value in milliseconds.
*/
uint64 slim_TimeToMillisecondsFloor
(
  uint64 t_TimeInClockTicks
);

/**
@brief Converts milliseconds to slow clock ticks.

Function converts given milliseconds value to slow clock ticks.

@param  t_TimeInMSecs: Clock value in milliseconds.
@return Clock value in slow clock ticks.
*/
uint64 slim_TimeToClockTicks
(
  uint64 t_TimeInMSecs
);

/**
@brief Sends provider IPC message to SLIM owning task.

Providers can use the function to create an IPC message and send it to the SLIM
owning task.

@param[in] e_Provider      Pointer to IPC payload.
@param[in] q_ProviderMsgId Provider message id.
@param[in] p_Payload       Pointer to IPC payload.
@param[in] q_PayloadSize   Size of the IPC payload.

@return 0 if IPC is sent successfully.
*/
uint32 slim_SendProviderIpcToTask
(
  slim_ProviderEnumType e_Provider,
  uint32 q_ProviderMsgId,
  const void *p_Payload,
  uint32 q_PayloadSize
);

/**
@brief Returns the throttling config status.

Function returns the throttling config parameters for a sensor.

@param[in] pz_Status: pointer to the provider throttle status.
@param[in] u_ThrottleId: throttle-id

@return    pointer to the throttle config data for the throttle-id
*/
slim_ProviderThrottleDataStructType* slim_ProviderGetThrottleStatus
(
  slim_ProviderThrottleStatusStructType *pz_Status,
  uint8 u_ThrottleId
);

/**
@brief Checks if data should be throttled.

Function checks if slim data should be throttled.

@param[in] pz_Status       Points to the throttling status. 
@param[in] u_ThrottleId    Throttle Id
@param[in] t_DataTimestamp Timestamp of new data indication.

@retval TRUE  if slim data should be throttled.
@retval FALSE if data should be forwarded to client.
*/
boolean slim_ProviderThrottleData
(
   slim_ProviderThrottleStatusStructType *pz_Status,
   uint8 u_ThrottleId,
   uint64 t_DataTimestamp
);

/**
@brief Sets the throttling configuration.

Function Sets the throttling configuration.

@param[in] pz_ProviderStatus Points to the throttling status. 
@param[in] u_ThrottleId          Throttle Id
@param[in] u_ThrottleThreshType  Throttle threshold type
@param[in] l_ServiceTxnId        associate txn Id
@param[in] w_ReportRate sensor report rate.
*/
void slim_ProviderConfigThrottleData
(
  slim_ProviderThrottleStatusStructType *pz_ProviderStatus,
  uint8 u_ThrottleId,
  uint8 u_ThrottleThreshType,
  int32  l_ServiceTxnId,
  uint16 w_ReportRate
);

//! @}
//! @}

#ifdef __cplusplus
}
#endif

#endif /* SLIM_UTILS_H_ */
