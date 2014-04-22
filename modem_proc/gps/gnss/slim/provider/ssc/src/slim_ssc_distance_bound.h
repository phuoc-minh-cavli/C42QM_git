#ifndef SLIM_SSC_DISTANCE_BOUND_H
#define SLIM_SSC_DISTANCE_BOUND_H
/**
  @file slim_ssc_distance_bound.h
  @brief Module implementing the distance bound request to sensor core.
 
  SLIM can use this module to start or stop the distance bound service
  streaming from SSC.

  @ingroup slim_SSCProvider
*/
/*============================================================================
                Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.

               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/provider/ssc/src/slim_ssc_distance_bound.h#1 $ */

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
@brief Initiates enable distance bound request to SSC.

Function initiates enable distance bound request to SSC.

@param[in] pz_SscData     Pointer to the SSC data.
@param[in] pz_Txn         Pointer to the distance bound data request transaction
                          data.
@param[in] l_ServiceTxnId Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscDistanceBoundMakeEnableRequest
(
  slim_SscSamConnectionDataStructType *pz_SscData,
  const slim_EnableDistanceBoundTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Initiates set distance bound request to SSC.

Function initiates set distance bound request to SSC.

@param[in] pz_SscData     Pointer to the SSC data.
@param[in] pz_Txn         Pointer to the distance bound data request transaction
                          data.
@param[in] l_ServiceTxnId Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscDistanceBoundMakeSetRequest
(
  slim_SscSamConnectionDataStructType *pz_SscData,
  const slim_SetDistanceBoundTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Initiates get distance bound request to SSC.

Function initiates get distance bound request to SSC.

@param[in] pz_SscData     Pointer to the SSC data.
@param[in] l_ServiceTxnId Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscDistanceBoundMakeGetRequest
(
  slim_SscSamConnectionDataStructType *pz_SscData,
  int32 l_ServiceTxnId
);

#ifdef FEATURE_LOCTECH_SLIM_LEGACY_QMI_SNS
/**
@brief Handles enable response data for distance bound request

This function handles enable response. Response is converted and forwarded to
SLIM client.

@param[in] q_MsgLength size of response message data
@param[in] p_MsgPayload Pointer to the response message data
@param[in] pz_DbData  Pointer to the client distance bound data
*/
void slim_SscDistanceBoundHandleEnableResponse
(
  uint32 q_MsgLength,
  const void *p_MsgPayload,
  slim_SscSamConnectionDataStructType *pz_DbData
);

/**
@brief Handles disable response data for distance bound request

This function handles disable response. Response is converted and forwarded to
SLIM client.

@param[in] pz_DbData  Pointer to the client distance bound data
@param[in] pz_RespMsg Pointer to the response message data
*/
void slim_SscDistanceBoundHandleDisableResponse
(
  slim_SscSamConnectionDataStructType *pz_DbData,
  const slim_SscDisableDbRespMsgT *pz_RespMsg
);

/**
@brief Handles set response data for distance bound request

This function handles set response. Response is converted and forwarded to
SLIM client.

@param[in] pz_RespMsg Pointer to the response message data
*/
void slim_SscDistanceBoundHandleSetResponse
(
  const slim_SscSetDbRespMsgT *pz_RespMsg
);

/**
@brief Handles get response data for distance bound request

This function handles get response. Response is converted and forwarded to
SLIM client.

@param[in] pz_RespMsg Pointer to the response message data
*/
void slim_SscDistanceBoundHandleGetResponse
(
  const slim_SscGetDbRespMsgT *pz_RespMsg
);

/**
@brief Handles distance bound data indication

This function handles distance bound data indication. Data is converted and
forwarded to SLIM client.

@param[in] q_MsgLength  size of input message
@param[in] p_MsgPayload Pointer to the indication message
*/
void slim_SscDistanceBoundHandleIndication
(
  uint32 q_MsgLength,
  const void *p_MsgPayload
);
#endif

/**
@brief Compose Enable request specific to connection

Funciton Composes Enable request specific to connection

@param[in] pz_SscData pointer to ssc data
@param[in] pz_Txn     pointer to transaction details
@param[o] ppz_Request double pointer to generated encoded request
@return SSC_SERVICE_SUCCESS or SSC service Error code
*/
slim_SscServiceErrorEnumType slim_SscComposeDistanceBoundEnableRequest
(
  slim_SscConnectionDataStructType *pz_SscData,
  const slim_EnableDistanceBoundTxnStructType *pz_Txn,
  void **ppz_Request
);

/**
@brief Compose Disable request specific to connection

Funciton Composes Disable request specific to connection

@param[in] pz_SscData pointer to ssc data
@param[in] pz_Txn           pointer to Transaction details 
@param[o] ppz_Request double pointer to generated encoded request
@param[in] u_InstanceId QMI connection instance ID
@return SSC_SERVICE_SUCCESS or SSC service Error code
*/
slim_SscServiceErrorEnumType slim_SscComposeDistanceBoundDisableRequest
(
  slim_SscConnectionDataStructType *pz_SscData,
  const slim_EnableDistanceBoundTxnStructType *pz_Txn,
  void **ppz_Request,
  uint8 u_InstanceId
);

/**
@brief Send enable request specific to connection

Funciton Snd enable request specific to connection

@param[in] pz_Request          pointer to encoded request
@param[in] pz_SscData          pointer to ssc data
@param[in] l_ServiceTxnId      Transaction ID used for request
@param[in] fn_ResponseCallback callback function to be used for response
@return SSC_SERVICE_SUCCESS or SSC service Error code
*/
slim_SscServiceErrorEnumType slim_SscSendDistanceBoundEnable
(
  void *pz_Request,
  slim_SscConnectionDataStructType *pz_SscData,
  int32 l_ServiceTxnId,
  qmi_client_recv_msg_async_cb fn_ResponseCallback
);

/**
@brief Send Disable request specific to connection

Funciton Snd Disable request specific to connection

@param[in] pz_Request          pointer to encoded request
@param[in] pz_SscData          pointer to ssc data
@param[in] l_ServiceTxnId      Transaction ID used for request
@param[in] fn_ResponseCallback callback function to be used for response
@return SSC_SERVICE_SUCCESS or SSC service Error code
*/
slim_SscServiceErrorEnumType slim_SscSendDistanceBoundDisable
(
  void *pz_Request,
  slim_SscConnectionDataStructType *pz_SscData,
  int32 l_ServiceTxnId,
  qmi_client_recv_msg_async_cb fn_ResponseCallback
);

/**
@brief Sends get request for distance bound service.

Function sends get request for distance bound service.

@param[in] u_InstanceId   Distance bound service instance id.
@param[in] pz_SscData     Pointer to client SSC data.
@param[in] l_ServiceTxnId Transaction id.

@retval SSC_SERVICE_SUCCESS if request was sent successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_QmiGetDistanceBoundReport
(
  uint8 u_InstanceId,
  slim_SscConnectionDataStructType *pz_SscData,
  int32 l_ServiceTxnId
);
/**
@brief Callback function for distance bound request.

Function is called when SSC is sending the response for distance bound-request.

@param[in] p_UserHandle  QMI-client handle.
@param[in] q_MsgId       SSC message id.
@param[in] p_Buf         Pointer to message.
@param[in] q_Len         Message length.
@param[in] p_RespCbData  Callback data (transaction id).
@param[in] e_TranspErr   QMI error.
*/
void slim_DistanceBoundResponseCallback
(
  qmi_client_type p_UserHandle,
  unsigned int q_MsgId,
  void *p_Buf,
  unsigned int q_Len,
  void *p_RespCbData,
  qmi_client_error_type e_TranspErr
);

/**
@brief set request for distance bound service.

Function set request for distance bound service.
slimf_distance global vairable is updated with user specific value 
which will be used to generate enable request

@param[in] pz_SscData      Pointer to client SSC data.
@param[in] l_ServiceTxnId  Transaction id.
@param[in] u_InstanceId    Distance bound service instance id.
@param[in] f_DistanceBound Bound to set.
@param[in] u_SessionKey    Session key.

@retval SSC_SERVICE_SUCCESS if request was sent successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_QmiSetBound
(
  slim_SscConnectionDataStructType *pz_SscData,
  int32 l_ServiceTxnId,
  uint8 u_InstanceId,
  float f_DistanceBound,
  uint8 u_SessionKey
);
/**
@brief Callback function for distance bound indications.

Function is called when SSC is sending data indications. Indication is
forwarded to SSC provider and the actual data conversion is done in callback
function.

@param[in] p_UserHandle QMI-client handle.
@param[in] q_MsgId      SSC message id.
@param[in] p_Buf        Pointer to message.
@param[in] q_Len        Message length.
@param[in] p_IndCbData  Callback data (client handle).
*/
void slim_DistanceBoundIndicationCallback
(
  qmi_client_type p_UserHandle,
  unsigned int q_MsgId,
  void *p_Buf,
  unsigned int q_Len,
  void *p_IndCbData
);


//! @}

#endif /* #ifndef SLIM_SSC_DISTANCE_BOUND_H */
