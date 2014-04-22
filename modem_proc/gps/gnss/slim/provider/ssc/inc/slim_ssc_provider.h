#ifndef SLIM_SSC_PROVIDER_H
#define SLIM_SSC_PROVIDER_H
/**
  @file
  @brief Implementation of SSC-based sensor data requests.

 This module implements the sensor data request handling when sensor provider
 is SSC.


  @ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Proprietary and Confidential.

               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/provider/ssc/inc/slim_ssc_provider.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_provider_data.h"
#include "slim_ssc_provider_data.h"

//! @addtogroup slim_SSCProvider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*! Struct for SLIM service provider entry (SSC) */
typedef struct slim_SscServiceNotifierDataStructType
{
  /*! @brief QCCI OS-specific parameters */
  qmi_client_os_params z_OsParams;
  /*! @brief QCCI client connection handle */
  qmi_client_type p_UserHandle;
} slim_SscServiceNotifierDataStructType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief Initializes SSC provider module.

Function initializes SSC provider module.

@return Pointer to the SSC provider implementation.
*/
slim_ServiceProviderInterfaceType *slim_SscInitialize
(
  void
);

/**
@brief Converts sample time base to milliseconds.

Function converts sample time base to milliseconds.

@param[in] q_TimeInClockTicks Slow clock value in ticks.
@return Clock value in milliseconds.
*/
uint64 slim_SscTimeBaseToMilliseconds
(
  uint32 q_TimeInClockTicks
);

/**
@brief Converts sample timestamp to milliseconds.

Function converts sample timestamp to milliseconds.

@param[in] q_TimeInClockTicks Slow clock value in ticks.
@return Clock value in milliseconds.
*/
uint64 slim_SscTimestampToMilliseconds
(
  uint32 q_TimeInClockTicks
);

/**
@brief Notify function for time request response

SLIM providers should use this function to notify of time request results.

@param[in] l_TransactionId           Transaction id for SLIM core.
@param[in] q_RemoteTimeTicks         Remote time in clock ticks.
@param[in] q_RemoteTimeRolloverCount Rollover count of remote time.
*/
void slim_SscNotifyTimeResponse
(
  int32 l_TransactionId,
  uint32 q_RemoteTimeTicks,
  uint32 q_RemoteTimeRolloverCount
);

/**
@brief Checks if SSC data should be throttled.

Function checks if SSC data should be throttled.

@param[in] p_Handle        SLIM client handle.
@param[in] e_SscReportId   SSC SMGR report id.
@param[in] t_DataTimestamp Current timestamp in milliseconds.

@retval TRUE if SSC data should be throttled.
@retval FALSE if data should be forwarded to client.
*/
boolean slim_SscThrottleData
(
  slimClientHandleT p_Handle,
  slim_SscSnsServiceReportEnumType e_SscReportId,
  uint64 t_DataTimestamp
);

/**
@brief Retrives the last txn-id associated with the input report-id.

Function retrives the last txn-id associated with the input report-id.

@param[in] e_SscReportId    SSC SMGR report id.
@param[in] l_TxnId          current txn-id.

@retval last-txn-id associated with the report id, -1 if not found.
*/
int32 slim_SscGetLastTxnIdForReportId
(
   slim_SscSnsServiceReportEnumType e_SscReportId,
   int32 l_TxnId
);

/**
@brief Retrives the report-id associated with the input txn-id.

Function retrives the report-id associated with the input txn-id.

@param[in] l_TxnId          current txn-id.

@retval SSC throttle report id.
*/
uint8 slim_SscGetThrottleIdForTxnId
(
   int32 l_TxnId
);

/**
@brief Start SSC provider module including initial service discovery.

Function starts the SSC provider module including initial service discovery.

@retval TRUE if service discovery was success,
        FALSE otherwise
*/
boolean slim_SscInitialServiceDiscovery (void);

/**
@brief Maps SLIM service enum to SSC SMGR report Id.

Function maps SLIM service enum to SSC SMGR report Id.

@param[in] e_ServiceType: SLIM service enum

@return SSC SMGR report id.
*/
slim_SscSnsServiceReportEnumType slim_SscMapSlimServiceToReportId
(
  slimServiceEnumT e_ServiceType,
  boolean v_ConsolidatedReq
);

/**
@brief Maps SSC SMGR report Id to SLIM service enum.

Function maps SSC SMGR report Id to SLIM service enum.

@param[in] SscReportId: SSC SMGR report id
@param[in] e_SensorType: SLIM sensor type

@return SLIM service type
*/
slimServiceEnumT slim_SscMapReportIdToSlimService
(
  slim_SscSnsServiceReportEnumType SscReportId,
  slimSensorTypeEnumT e_SensorType
);

/**
@brief Provides SSC client connection data structure for a txn id.

Function provides SSC client connection data structure for a txn id.

@param  pz_SscData: Pointer to the SSC provider data structure.
@param  l_TxnId: Transaction id.
@return Pointer to SSC client connection structure if successfull. Otherwise
NULL.
*/
slim_SscClientStructType *slim_SscGetClientDataForTxnId
(
  slim_SscProviderStructType *pz_SscData,
  int32 l_TxnId
);

/**
@brief Returns SSC provider data structure.

Function returns SSC provider data structure.

@return Pointer to the SSC provider data structure.
*/
slim_SscProviderStructType *slim_SscGetProviderData(void);

/**
@brief Provides SSC client connection data structure for a client handle.

Function provides SSC client connection data structure for a client handle.

@param  pz_SscData: Pointer to the SSC provider data structure.
@param  p_Handle: Pointer to the SLIM client handle.
@return Pointer to SSC client connection structure if successfull. Otherwise
NULL.
*/
slim_SscClientStructType *slim_SscGetClientData
(
  slim_SscProviderStructType *pz_SscData,
  slimClientHandleT p_Handle
);

/**
@brief Provides the QMI service object for the given service.

Function provides the QMI service object for the given service.

@param[in] e_SnsService QMI_SNS service type
@return QMI_SNS service object for the given service. NULL, if not found.
*/
qmi_idl_service_object_type slim_SscGetServiceObject
(
  slim_SscSnsServiceEnumType e_SnsService
);

/**
@brief Provides the SNS service type for the given SLIM service.

Function provides the SNS service type for the given SLIM service.

@param[in] e_Service SLIM service
@return QMI_SNS service type for the given service.
*/
slim_SscSnsServiceEnumType slim_SscGetSnsServiceForSlimService
(
  slimServiceEnumT e_Service
);

/**
@brief handler function for SSC connection close.

Function is called whenever some SSC connection is closed.
This is triggered from internal message handler to avoid locks in callbacks
It will complete the client closing by clearing client data if no open
SSC connections are found.

@param  p_Handle: client handle( callback data ).
*/
void slim_SscCloseConnectionHandler
(
  slimClientHandleT  p_Handle
);
//! @}

#endif /* #ifndef SLIM_SSC_PROVIDER_H */
