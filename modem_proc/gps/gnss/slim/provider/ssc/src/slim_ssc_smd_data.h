#ifndef SLIM_SSC_SMD_DATA_H
#define SLIM_SSC_SMD_DATA_H
/**
@file
@brief Module implementing the SMD data request to sensor core.
 
  SLIM can use this module to start or stop the SMD data streaming from SSC.

@ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2013-2019 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/provider/ssc/src/slim_ssc_smd_data.h#1 $ */

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

#ifdef FEATURE_LOCTECH_SLIM_LEGACY_QMI_SNS
/**
@brief Handle SMD indication events.

Function Handles SMD indication events.

@param  p_UserHandle:  pointer to user handle.
@param  q_MsgId:       indication message ID.
@param  p_Buf:         indication message data.
@param  q_Len:         indication message length.
@param  p_IndCbData:   Transaction id received as callback data.
*/
void slim_SmdIndicationCallback
(
  qmi_client_type p_UserHandle,
  unsigned int q_MsgId,
  void *p_Buf,
  unsigned int q_Len,
  void *p_IndCbData
);

/**
@brief Handle SMD response message.

Function Handles SMD response message.

@param  p_UserHandle:  pointer to user handle.
@param  q_MsgId:       indication message ID.
@param  p_Buf:         indication message data.
@param  q_Len:         indication message length.
@param  p_IndCbData:   Transaction id received as callback data.
@param  e_TranspErr:   QMI transport error if any.
*/
void slim_SmdResponseCallback
(
  qmi_client_type p_UserHandle,
  unsigned int q_MsgId,
  void *p_Buf,
  unsigned int q_Len,
  void *p_RespCbData,
  qmi_client_error_type e_TranspErr
);

/**
@brief Gets QMI service instance.

Function gets QMI service instance and initializes the QMI client connection.

@param  p_Handle: Pointer to the SLIM client handle.
@param  pz_SscData: Pointer to the SSC client data.
*/
slim_SscServiceErrorEnumType slim_GetSscSmdService
(
  slimClientHandleT p_Handle,
  slim_SscConnectionDataStructType *pz_SscData,
  qmi_client_ind_cb fn_IndicationCallback
);

/**
@brief Compose SMD Enable Request message.

This Function composes SMD Enable Request message.

@param[i]  pz_SscData:            pointer to SSC data.
@param[i]  pz_Txn:                Transaction ID.
@param[o]  ppz_Request:           enable request.
@param[i]  fn_IndicationCallback: Call back function for indication event handling.
@return SSC_SERVICE_SUCCESS
*/
slim_SscServiceErrorEnumType slim_SscComposeSmdEnableRequest
(
  slim_SscConnectionDataStructType *pz_SscData,
  const slim_EnableSmdDataTxnStructType *pz_Txn,
  void *ppz_Request,
  qmi_client_ind_cb fn_IndicationCallback
);

/**
@brief Send SMD Enable Request message.

This Function Sends SMD Enable Request message.

@param[i]  pz_SscData:            pointer to SSC data.
@param[i]  pz_Request:            enable request.
@param[i]  l_ServiceTxnId:        transaction ID.
@param[i]  fn_IndicationCallback: Call back function for response handling.
@return SSC_SERVICE_SUCCESS
*/
slim_SscServiceErrorEnumType slim_SscSendSmdEnable
(
  slim_SscConnectionDataStructType *pz_SscData,
  void *pz_Request,
  int32 l_ServiceTxnId,
  qmi_client_recv_msg_async_cb fn_ResponseCallback
);

/**
@brief Converts SSC SMD data to SLIM SMD data.

Function converts SSC SMD data to SLIM SMD data.

@param  pz_SmdInd: Pointer to SSC SMD data.
@param  pz_SmdData: Pointer to SLIM SMD data.
*/
void slim_ConvertSmdData
(
  const sns_sam_smd_report_ind_msg_v01 *pz_SmdInd,
  slimSmdDataStructT *pz_SmdData
);

/**
@brief Compose SMD Disable Request message.

This Function composes SMD Disable Request message.

@param[i]  pz_SscData:            pointer to SSC data.
@param[o]  ppz_Request:           Disable request.
@param[i]  u_InstanceId:          Connection Instance ID.
@return SSC_SERVICE_SUCCESS
*/
slim_SscServiceErrorEnumType slim_SscComposeSmdDisableRequest
(
  slim_SscConnectionDataStructType *pz_SscData,
  void *ppz_Request,
  uint8 u_InstanceId
);

/**
@brief Send SMD Disable Request message.

This Function Sends SMD Disable Request message.

@param[i]  pz_SscData:            pointer to SSC data.
@param[i]  pz_Request:            encoded enable request.
@param[i]  l_ServiceTxnId:        transaction ID.
@param[i]  fn_IndicationCallback: Call back function for response handling.
@return SSC_SERVICE_SUCCESS
*/
slim_SscServiceErrorEnumType slim_SscSendSmdDisable
(
  slim_SscConnectionDataStructType *pz_SscData,
  void *pz_Request,
  int32 l_ServiceTxnId,
  qmi_client_recv_msg_async_cb fn_ResponseCallback
);
/**
@brief Initiates SMD data request to SSC.

Function initiates SMD data request to SSC. If request is successfull,
SSC SMD data streaming is either started or stopped.

@param  pz_SscData: Pointer to the client SMD SSC data.
@param  pz_Txn: Pointer to SMD data request transaction data.
@param  l_ServiceTxnId : Service transaction id.
@return SSC_SERVICE_SUCCESS if request is done successfully. Otherwise SSC error
code.
*/
slim_SscServiceErrorEnumType slim_SscSmdDataEnableReporting
(
  slim_SscSamConnectionDataStructType *pz_SscData,
  const slim_EnableSmdDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Handles enable response data for SMD request

This function handles enable response. Response is converted and forwarded to
SLIM client.

@param  pz_SmdData: Pointer to the client SMD data
@param  pz_RespMsg: Pointer to the response message data
*/
void slim_SscSmdDataHandleEnableResponse
(
  slim_SscSamConnectionDataStructType *pz_SmdData,
  const slim_SscEnableSmdRespMsgT *pz_RespMsg
);

/**
@brief Handles disable response data for SMD request

This function handles disable response. Response is converted and forwarded to
SLIM client.

@param  pz_SmdData: Pointer to the client SMD data
@param  pz_RespMsg: Pointer to the response message data
*/
void slim_SscSmdDataHandleDisableResponse
(
  slim_SscSamConnectionDataStructType *pz_SmdData,
  const slim_SscDisableSmdRespMsgT *pz_RespMsg
);

/**
@brief Handles SMD indication data

This function handles SMD indication data. Data is converted and forwarded
to SLIM client.

@param  pz_IndMsg: Pointer to the response message data
*/
void slim_SscSmdDataHandleIndication
(
  const slim_SscSmdIndMsgT *pz_IndMsg
);
#else
/**
@brief Initiates SMD data request to SSC.

Function initiates SMD data request to SSC. If request is successful,
SSC SMD data streaming is either started or stopped.

@param[in] pz_SscData     Pointer to the client SMD SSC data.
@param[in] pz_Txn         Pointer to SMD data request transaction data.
@param[in] l_ServiceTxnId Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscSmdDataEnableReporting
(
  slim_SscSamConnectionDataStructType *pz_SscData,
  const slim_EnableQmdDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

#endif

//! @}

#endif /* #ifndef SLIM_SSC_SMD_DATA_H */
