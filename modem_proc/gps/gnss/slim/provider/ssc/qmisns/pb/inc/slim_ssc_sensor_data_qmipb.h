#ifndef SLIM_SSC_SENSOR_DATA_QMI_PB_H
#define SLIM_SSC_SENSOR_DATA_QMI_PB_H
/**
  @file
  @brief Util functions needed in SSC connection handling.

  This module provides functions that are needed in for establishing
  QMI-connection to the SSC.

  @ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2017 Qualcomm Technologies, Inc.
                      All Rights Reserved.
           Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/provider/ssc/qmisns/pb/inc/slim_ssc_sensor_data_qmipb.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_ssc_provider_data.h"
#include "slim_ssc_msg_types.h"
#include "slim_ssc_provider_qmipb.h"


//! @addtogroup slim_SSCProvider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/


/*!
 * @brief Constant for converting Q16 to double by multiplication
 *
 * - Type is double to minimize rounding errors on multiplication.
 */
#define UNIT_CONVERT_Q16             (1.0 / 65536.0)
/*! @brief Constant for converting Q16 uTesla to float by multiplication
 *
 * This constant equals to (100. * UNIT_CONVERT_Q16)
 *
 * - Units: uTesla.
 * - Type is double to minimize rounding errors on multiplication.
*/
#define UNIT_CONVERT_MAGNETIC_FIELD  (100. / 65536.)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
@brief Compose Sensor Enable request

This function Composes Sensor Enable request

@param[in] pz_SscData           pointer to SSC data
@param[in] pz_Txn               pointer to Transaction data structure
@param[in] v_ConsolidatedReq    falg indicating if this Consolicated request for all sensors
@param[in] q_ReportId           Report ID
@param[0]  ppz_Request          bouble pointer to formatted requtest 
@return SSC_SERVICE_SUCCESS if request sent else Slim Service error code 
*/
slim_SscServiceErrorEnumType slim_SscComposeSensorDataEnableRequest
(
  slim_SscDeviceSensorConnectionDataStructType *pz_SscData,
  const slim_EnableSensorDataTxnStructType *pz_Txn,
  boolean v_ConsolidatedReq,
  uint32 q_ReportId,
  void **ppz_Request
);

/**
@brief Send Sensor Enable Request

This function sends Sensor Enable Request

@param[in] pz_Request           pointer to Request message
@param[in] pz_SscData           pointer to SSC data
@param[in] l_ServiceTxnId       Transaction ID from Client
@param[in] fn_ResponseCallback  Call back function to handle response
@return SSC_SERVICE_SUCCESS if request sent else Slim Service error code 
*/
slim_SscServiceErrorEnumType slim_SscSendSensorDataEnable
(
  void *pz_Request,
  slim_SscConnectionDataStructType *pz_SscConnection,
  int32 l_ServiceTxnId,
  qmi_client_recv_msg_async_cb fn_ResponseCallback
);

/**
@brief Handles response data for SMGR request

This function handles SMGR responses.

@param[in] p_UserHandle QMI client handle
@param[in] q_MsgId      Response mssage ID
@param[in] p_Buf        pointer to QMI reponse data
@param[in] q_Len        size of response message data
@param[in] p_RespCbData Call back fucntion for parsing
@param[in] e_TranspErr  QMI Transport error type
*/
void slim_SensorDataResponseCallback
(
  qmi_client_type p_UserHandle,
  unsigned int q_MsgId,
  void *p_Buf,
  unsigned int q_Len,
  void *p_RespCbData,
  qmi_client_error_type e_TranspErr
);

/**
@brief Callback function for sensor data indications.

Function is called when SSC is sending data indications. Indication is
forwarded to SSC provider and the actual data conversion is done in callback
function.

@param[in] p_UserHandle QMI-client handle.
@param[in] q_MsgId      SSC message id.
@param[in] p_Buf        Pointer to message.
@param[in] q_Len        Message length.
@param[in] p_IndCbData  Callback data (client handle).
*/
void slim_SensorDataIndicationCallback
(
  qmi_client_type p_UserHandle,
  unsigned int q_MsgId,
  void *p_Buf,
  unsigned int q_Len,
  void *p_IndCbData
);

/**
@brief Convert SSC Sensor data to SLIM structure.

Function converts SSC sensor data to SLIM structures. Data
is dispatched to client.

@param[in] sensorSamples sensor data
@param[in] SensorId      sensor type
@param[in] p_Handle      client handle for forwarding data

*/
void slim_PBConvertAndSendSensorData
(
  const slim_SscSensorEventPayloadArgType* pz_SensorDataIn, 
  slimSensorTypeEnumT e_SensorType,
  slimClientHandleT p_Handle
);

/**
@brief retrive throttle ID corresponding to client_id

This function retrive throttle ID corresponding to client_id

@param[in] client_id  Slim Client ID
*/
uint8 slim_SscMatchThrottleIdForClientId 
(
  uint64 t_ClientId
);

#endif /* #ifndef SLIM_SSC_SENSOR_DATA_QMI_PB_H */
