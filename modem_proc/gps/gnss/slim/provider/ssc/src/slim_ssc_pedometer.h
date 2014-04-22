#ifndef SLIM_SSC_PEDOMETER_H
#define SLIM_SSC_PEDOMETER_H
/**
  @file
  @brief Module implementing the pedometer request to sensor core.
 
  SLIM can use this module to start or stop the pedometer
  streaming from SSC.

  @ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/provider/ssc/src/slim_ssc_pedometer.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_ssc_common.h"

//! @addtogroup slim_SSCProvider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief QMI callback function for Ped indication

This function handles Ped indications. Called from QMI context

@param[in] p_UserHandle Pointer to QMI client handle
@param[in] q_MsgId      QMI message ID
@param[in] p_Buf        Pointer to the indication message data
@param[in] q_Len        Indication message length
@param[in] p_IndCbData  Pointer to the indication callback data

*/
void slim_PedometerIndicationCallback
(
  qmi_client_type p_UserHandle,
  unsigned int q_MsgId,
  void *p_Buf,
  unsigned int q_Len,
  void *p_IndCbData
);

/**
@brief QMI response callback function for Ped. Called from QMI task context

This function handles Ped responses. Response is converted and forwarded to
SLIM client.

@param[in] p_UserHandle   user Handle
@param[in] q_MsgId        QMI message ID
@param[in] p_Buf Pointer  Pointer to the response message data
@param[in] q_Len Pointer  Response message length
@param[in] p_RespCbData   Pointer to callback data
@param[in] e_TranspErr    Transport error

*/
void slim_PedometerResponseCallback
(
  qmi_client_type p_UserHandle,
  unsigned int q_MsgId,
  void *p_Buf,
  unsigned int q_Len,
  void *p_RespCbData,
  qmi_client_error_type e_TranspErr
);

/**
@brief Sends rest request for pedometer service.

Function sends reset request for pedometer service.

@param[in] u_InstanceId   Pedometer service instance id.
@param[in] pz_SscData     Pointer to client SSC data.
@param[in] l_ServiceTxnId Transaction id.

@retval SSC_SERVICE_SUCCESS if request was sent successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_ResetStepCount
(
  uint8 u_InstanceId,
  slim_SscConnectionDataStructType *pz_SscData,
  int32 l_ServiceTxnId,
  qmi_client_recv_msg_async_cb fn_Callback
);

/**
@brief Compose the Request message for enabling Pedometer service


@param[in] pz_SscData    Pointer to the SSC connection data.
@param[in] pz_Txn        Request configuration
@param[in] ppz_Request   Pointer to message buffer

@retval SSC_SERVICE_SUCCESS if the request is sent successfully.
Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscComposePedometerEnableRequest
(
	slim_SscConnectionDataStructType *pz_SscData,
	const slim_EnablePedometerTxnStructType *pz_Txn,
	void **ppz_Request
);

/**
@brief Send the request message for enabling Ped service

@param[in] pz_Request           Pointer to message buffer
@param[in] pz_SscConnection     Pointer to the SSC connection data.
@param[in] fn_ResponseCallback  Pointer to QMI callback function
@param[in] l_ServiceTxnId       Transaction ID

@retval SSC_SERVICE_SUCCESS if the request is sent successfully.
Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscSendPedometerEnable
(
  slim_SscConnectionDataStructType *pz_SscData,
  void *pz_Request,
  int32 l_ServiceTxnId,
  qmi_client_recv_msg_async_cb fn_ResponseCallback
);

/**
@brief Compose the message for enabling Pedometer service

This function Composes request message for disabling Ped service

@param[in] pz_SscData    Pointer to the SSC connection data.
@param[in] pz_Txn        Request configuration
@param[in] ppz_Request   Pointer to message buffer
@param[in] u_InstanceId  Instance ID

@retval SSC_SERVICE_SUCCESS if the request is sent successfully.
Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscComposePedometerDisableRequest
(
  slim_SscConnectionDataStructType *pz_SscData,
  const slim_EnablePedometerTxnStructType *pz_Txn,
  void *ppz_Request,
  uint8 u_InstanceId
);

/**
@brief Send the request message for disabling Ped service

This function sends request message for disabling Ped service

@param[in] pz_Request           Pointer to message buffer
@param[in] pz_SscConnection     Pointer to the SSC connection data.
@param[in] fn_ResponseCallback  Pointer to QMI callback function
@param[in] l_ServiceTxnId       Transaction ID

@retval SSC_SERVICE_SUCCESS if the request is sent successfully.
Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscSendPedometerDisable
(
  slim_SscConnectionDataStructType *pz_SscData,
  void *pz_Request,
  int32 l_ServiceTxnId,
  qmi_client_recv_msg_async_cb fn_ResponseCallback
);

/**
@brief Initiates pedometer request to SSC.

Function initiates pedometer request to SSC. If the request is successfull,
SSC pedometer streaming is either started or stopped.

@param[in] pz_SscData     Pointer to the SSC data.
@param[in] pz_Txn         Pointer to the pedometer request transaction data.
@param[in] l_ServiceTxnId Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscPedometerEnableReporting
(
  slim_SscPedometerConnectionDataStructType *pz_SscData,
  const slim_EnablePedometerTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

#ifdef FEATURE_LOCTECH_SLIM_LEGACY_QMI_SNS

/**
@brief Handles enable response data for pedometer request

This function handles enable response. Response is converted and forwarded to
SLIM client.

@param[in] pz_PedometerData Pointer to the client pedometer data
@param[in] pz_RespMsg       Pointer to the response message data
*/
void slim_SscPedometerHandleEnableResponse
(
  slim_SscPedometerConnectionDataStructType *pz_PedometerData,
  const slim_SscEnablePedRespMsgT *pz_RespMsg
);

/**
@brief Handles disable response data for pedometer request

This function handles disable response. Response is converted and forwarded to
SLIM client.

@param[in] pz_PedometerData Pointer to the client pedometer data
@param[in] pz_RespMsg       Pointer to the response message data
*/
void slim_SscPedometerHandleDisableResponse
(
  slim_SscPedometerConnectionDataStructType *pz_PedometerData,
  const slim_SscDisablePedRespMsgT *pz_RespMsg
);

/**
@brief Handles reset response data for pedometer request

This function handles reset response. Response is converted and forwarded to
SLIM client.

@param[in] pz_PedometerData Pointer to the client pedometer data
@param[in] pz_RespMsg       Pointer to the response message data
*/
void slim_SscPedometerHandleResetResponse
(
  slim_SscPedometerConnectionDataStructType *pz_PedometerData,
  const slim_SscResetPedRespMsgT *pz_RespMsg
);

/**
@brief Handles pedometer indication data

This function handles pedometer indication data. Data is converted and forwarded
to SLIM client.

@param[in] pz_PedometerData Pointer to the client pedometer data
@param[in] pz_IndMsg        Pointer to the indication message
*/
void slim_SscPedometerHandleIndication
(
  slim_SscPedometerConnectionDataStructType *pz_PedometerData,
  const slim_SscPedIndMsgT *pz_IndMsg
);
#endif

//! @}

#endif /* #ifndef SLIM_SSC_PEDOMETER_H */
