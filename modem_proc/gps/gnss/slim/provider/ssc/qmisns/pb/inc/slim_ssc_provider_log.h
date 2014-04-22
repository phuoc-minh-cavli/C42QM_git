#ifndef SLIM_SSC_PROVIDER_LOG_H
#define SLIM_SSC_PROVIDER_LOG_H
/**
@file
@brief SLIM diagnostic logging API

@ingroup slim_CoreDiag
*/
/*============================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

               Copyright (c) 2017 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/provider/ssc/qmisns/pb/inc/slim_ssc_provider_log.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_ssc_common.h"
#include "slim_ssc_pb_parser.h"
#include "sns_client.pb.h"
#include "sns_suid.pb.h"
#include "sns_client.pb.h"
#include "sns_distance_bound.pb.h"
#include "sns_resampler.pb.h"

//! @addtogroup slim_CoreDiag
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*! Struct for PED Data Event Received at SLIM provider log payload */
typedef PACKED struct PACKED_POST
{
  sns_suid_req z_SuidReq;         
  slimServiceEnumT e_ServiceType;
} slim_LogSuidReqDataStructType;


/*! Struct for PED Data Event Received at SLIM provider log payload */
typedef PACKED struct PACKED_POST
{
  U32 q_StepCount;                /**< current step count */
  FLT f_StepRateHzFiltered;       /**< filtered step rat, in Hz */
  FLT f_StepRateHzUnfiltered;     /**< unfiltered step rat, in Hz */

  U64 t_LastStepEventTimestampMs; /**< last step event timestamp, in msec */
  U32 q_LastStepCount;            /**< last step count */
} slim_LogPedDataStructType;

/* Structure for passing funciton and data as argument during decoded of SNS sensor data */
typedef struct 
{
  sns_std_suid z_Suid;         /*!< Sensor SUID */
  uint32 q_MsgId;            /*!< Event Type */
  uint32 q_TotalLen;
  uint64 t_TimestampMs;
  uint32 q_Handle;
  uint32 q_ServiceId; 
  uint8  u_DecodeRetVal;
} slim_LogSensorDataEventStructType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief logs SUID Request sent from SSC Provider.

Function logs SUID Request sent from SSC Provider.

@param[in] q_QmiConnHandle  QMI Client Handle.
@param[in] q_SlimTxnID  Slim Transaction ID.
@param[in] q_PayloadLen  Request message Payload Length.
@param[in] e_ServiceType  Request Service Type.
@param[in] pz_PBReqMsg  Request messsage information.
@param[in] pz_Request  Request message data specific to SUID.
*/
void slim_LogSnsSuidReq 
(
  U32 q_QmiConnHandle,
  U32 q_SlimTxnID,
  U32 q_PayloadLen,
  slimServiceEnumT e_ServiceType,
  const sns_client_request_msg* pz_PBReqMsg,
  const slim_LogSuidReqDataStructType* pz_Request
);

/**
@brief logs Sensor Request sent from SSC Provider.

Function logs Sensor Request sent from SSC Provider.

@param[in] q_QmiConnHandle  QMI Client Handle.
@param[in] q_SlimTxnID  Slim Transaction ID.
@param[in] q_PayloadLen  Request message Payload Length.
@param[in] e_ServiceType  Request Service Type.
@param[in] pz_PBReqMsg  Request messsage information.
@param[in] pz_Request  Request message data specific to Sensor.
*/
void slim_LogSnsSensorDataReq 
(
  U32 q_QmiConnHandle,
  U32 q_SlimTxnID,
  U32 q_PayloadLen,
  slimServiceEnumT e_ServiceType,
  const sns_client_request_msg* pz_PBReqMsg,
  const slimEnableSensorDataRequestStructT* pz_Request
);

/**
@brief logs ReSampler Request sent from SSC Provider.

Function logs ReSampler Request sent from SSC Provider.

@param[in] q_QmiConnHandle  QMI Client Handle.
@param[in] q_SlimTxnID  Slim Transaction ID.
@param[in] q_PayloadLen  Request message Payload Length.
@param[in] e_ServiceType  Request Service Type.
@param[in] pz_PBReqMsg  Request messsage information.
@param[in] pz_Request  Request message data specific to ReSampler.
*/
void slim_LogSnsResamplerDataReq 
(
  U32 q_QmiConnHandle,
  U32 q_SlimTxnID,
  U32 q_PayloadLen,
  slimServiceEnumT e_ServiceType,
  const sns_client_request_msg* pz_PBReqMsg,
  const sns_resampler_config * pz_Request
);

/**
@brief logs AMD Request sent from SSC Provider.

Function logs AMD Request sent from SSC Provider.

@param[in] q_QmiConnHandle  QMI Client Handle.
@param[in] q_SlimTxnID  Slim Transaction ID.
@param[in] q_PayloadLen  Request message Payload Length.
@param[in] e_ServiceType  Request Service Type.
@param[in] pz_PBReqMsg  Request messsage information.
@param[in] pz_Request  Request message data specific to AMD.
*/
void slim_LogSnsAmdRmdReq 
(
  U32 q_QmiConnHandle,
  U32 q_SlimTxnID,
  U32 q_PayloadLen,
  slimServiceEnumT e_ServiceType,
  const sns_client_request_msg* pz_PBReqMsg,
  const slimEnableQmdDataRequestStructT* pz_Request
);
/**
@brief logs Pedometer Request sent from SSC Provider.

Function logs Pedometer Request sent from SSC Provider.

@param[in] q_QmiConnHandle  QMI Client Handle.
@param[in] q_SlimTxnID  Slim Transaction ID.
@param[in] q_PayloadLen  Request message Payload Length.
@param[in] e_ServiceType  Request Service Type.
@param[in] pz_PBReqMsg  Request messsage information.
@param[in] pz_Request  Request message data specific to Pedometer.
*/
void slim_LogSnsPedoDataReq 
(
  U32 q_QmiConnHandle,
  U32 q_SlimTxnID,
  U32 q_PayloadLen,
  slimServiceEnumT e_ServiceType,
  const sns_client_request_msg* pz_PBReqMsg,
  const slimEnablePedometerRequestStructT * pz_Request
);

/**
@brief logs DistanceBound Request sent from SSC Provider.

Function logs DistanceBound Request sent from SSC Provider.

@param[in] q_QmiConnHandle  QMI Client Handle.
@param[in] q_SlimTxnID  Slim Transaction ID.
@param[in] q_PayloadLen  Request message Payload Length.
@param[in] e_ServiceType  Request Service Type.
@param[in] pz_PBReqMsg  Request messsage information.
@param[in] pz_Request  Request message data specific to DistanceBound.
*/
void slim_LogSnsDBDataReq 
(
  U32 q_QmiConnHandle,
  U32 q_SlimTxnID,
  U32 q_PayloadLen,
  slimServiceEnumT e_ServiceType,
  const sns_client_request_msg* pz_PBReqMsg,
  const sns_set_distance_bound * pz_DistRequestBounds,
  const sns_set_distance_bound_speeds * pz_SpeedRequestBounds
);

/**
@brief Logs Respnse received at SSC provider corresponding to request.

Function Logs Respnse received at SSC provider corresponding to request.

@param[in] q_QmiClientHandle  QMI Client Handle.
@param[in] q_MsgId  SUID of sensor.
@param[in] q_QmiTranspErrorCode  SUID of sensor.
@param[in] pz_RespMsg  Response details.
*/
void slim_LogSnsClinetApiRespEvent (
  U32 q_QmiClientHandle,
  U32 q_MsgId,
  U32 q_QmiTranspErrorCode,
  const slim_SscClientRespMsgT *pz_RespMsg
);

/**
@brief logs Event received at SSC Provider.

Function logs Event received at SSC Provider.

@param[in] t_ModemTTMS  Time stamp of received Event.
@param[in] q_QmiClientHandle  QMI Client Handle.
@param[in] q_SlimClientHandle  Slim Client Handle.
@param[in] q_MsgId  Event messages ID.
@param[in] u_IndValid  TRUE if event is valid.
@param[in] z_Ind  Event data if event is valid.
*/
void slim_LogSnsClinetApiIndEvent (
  U64 t_ModemTTMS,
  U32 q_QmiClientHandle,
  U32 q_SlimClientHandle,
  U32 q_MsgId,
  U8 u_IndValid,
  const sns_client_report_ind_msg_v01* pz_IndData,   /**< Indication data */
  const sns_std_suid* pz_Suid   /**< SUID for the Indication data */
);

/**
@brief Logs SUID received for sensor.

Function logs SUID received for sensor.

@param[in] pz_Suid  SUID of sensor.
@param[in] q_MsgId  Event message ID.
@param[in] q_MsgLen  Event message Size.
@param[in] t_SnsTimestampMs  Event message Time stamp.
@param[in] q_slimClientHandle  Client ID to send this Event.
@param[in] v_DecodeReturnVal  Event message parse status.
@param[in] q_SlimServiceType  Event message service type corresponding to SUID.
*/
void slim_LogSnsSuidEvent
(
  const sns_std_suid* pz_Suid,
  U32 q_MsgId,
  U32 q_MsgLen,
  U64 t_SnsTimestampMs,
  U32 q_slimClientHandle,
  boolean v_DecodeReturnVal,
  U32 q_SlimServiceType
);

/**
@brief Logs Sensor Data Event received for sensor.

Function logs Sensor Data Event received for sensor.

@param[in] pz_Suid  SUID of sensor.
@param[in] q_MsgId  Event message ID.
@param[in] q_MsgLen  Event message Size.
@param[in] t_SnsTimestampMs  Event message Time stamp.
@param[in] q_slimClientHandle  Client ID to send this Event.
@param[in] v_DecodeReturnVal  Event message parse status.
@param[in] q_SlimServiceType  Service type of event.
@param[in] u_SampleCount  Total number of samples in event.
@param[in] t_FirstSampleTimestampMs  Time stamp of first sample in Ms.
@param[in] q_LastSampleTimestampOffetMs  Offset from From first sample in Ms.
*/
void slim_LogSnsSensorDataEvent
(
  const sns_std_suid*  pz_Suid,
  U32 q_MsgId,
  U32 q_MsgLen,
  U64 t_SnsTimestampMs,
  U32 q_slimClientHandle,
  boolean v_DecodeReturnVal,
  U32 q_SlimServiceType,
  U8 u_SampleCount,      
  U64 t_FirstSampleTimestampMs,
  U32 q_LastSampleTimestampOffetMs
);

/**
@brief Logs Sensor Data Event received for sensor.

Function logs Sensor Data Event received for sensor.

@param[in] pz_Suid  SUID of sensor.
@param[in] q_MsgId  Event message ID.
@param[in] q_MsgLen  Event message Size.
@param[in] t_SnsTimestampMs  Event message Time stamp.
@param[in] q_slimClientHandle  Client ID to send this Event.
@param[in] v_DecodeReturnVal  Event message parse status.
@param[in] q_AmdRMD_Event  Event Data AMD/RMD
*/
void slim_LogSnsAmdRmdDataEvent
(
  const sns_std_suid*  pz_Suid,
  U32 q_MsgId,
  U32 q_MsgLen,
  U64 t_SnsTimestampMs,
  U32 q_slimClientHandle,
  boolean v_DecodeReturnVal,
  U32 q_AmdRMD_Event
);

/**
@brief Logs Pedometer Data Event received for sensor.

Function logs Pedometer Data Event received for sensor.

@param[in] pz_Suid  SUID of sensor.
@param[in] q_MsgId  Event message ID.
@param[in] q_MsgLen  Event message Size.
@param[in] t_SnsTimestampMs  Event message Time stamp.
@param[in] q_slimClientHandle  Client ID to send this Event.
@param[in] v_DecodeReturnVal  Event message parse status.
@param[in] slim_LogPedDataStructType  PED Event data.
*/
void slim_LogSnsPedDataEvent
(
  const sns_std_suid*  pz_Suid,
  U32 q_MsgId,
  U32 q_MsgLen,
  U64 t_SnsTimestampMs,
  U32 q_slimClientHandle,
  boolean v_DecodeReturnVal,
  const slim_LogPedDataStructType *z_EventData
);

/**
@brief Logs DB Data Event received for sensor.

Function logs DB Data Event received for sensor.

@param[in] pz_Suid  SUID of sensor.
@param[in] q_MsgId  Event message ID.
@param[in] q_MsgLen  Event message Size.
@param[in] t_SnsTimestampMs  Event message Time stamp.
@param[in] q_slimClientHandle  Client ID to send this Event.
@param[in] v_DecodeReturnVal  Event message parse status.
@param[in] FLT Distance accumilated
@param[in] U8 u_BreachType State of system if breach happend
*/
void slim_LogSnsDBDataEvent
(
  const sns_std_suid*  pz_Suid,
  U32 q_MsgId,
  U32 q_MsgLen,
  U64 t_SnsTimestampMs,
  U32 q_slimClientHandle,
  boolean v_DecodeReturnVal,
  FLT f_DistanceAccumilated,
  U8 u_BreachType
);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_SSC_PROVIDER_LOG_H */
