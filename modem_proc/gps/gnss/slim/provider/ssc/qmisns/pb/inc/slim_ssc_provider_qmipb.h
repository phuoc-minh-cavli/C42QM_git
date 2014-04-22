#ifndef SLIM_SSC_PROVIDER_QMI_PB_H
#define SLIM_SSC_PROVIDER_QMI_PB_H
/**
  @file
  @brief Util functions needed in SSC connection handling.

  This module provides functions that are needed in for establishing
  QMI-connection to the SSC.

  @ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
                      All Rights Reserved.
           Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/provider/ssc/qmisns/pb/inc/slim_ssc_provider_qmipb.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_ssc_provider_data.h"
#include "sns_std_type.pb.h"

#include "sns_client_api_v01.h"

//! @addtogroup slim_SSCProvider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define SLIM_TEST_TODO_SUID_DECODE_IN_QMI_CONTEXT
//#define SLIM_TEST_REVERT_RESAMPLER_SENSOR


/* 
* Sting names for SSC sensors 
*/
#define ACCEL_SERVICE_NAME       "accel"
#define GYRO_SERVICE_NAME        "gyro"
#define MAG_UNCAL_SERVICE_NAME   "mag"
#define SMD_SERVICE_NAME        "sig_motion"
#define BARO_SERVICE_NAME        "pressure"
#define AMD_SERVICE_NAME         "amd"
#define RMD_SERVICE_NAME         "rmd"
#define PEDO_SERVICE_NAME        "pedometer"
#define DB_SERVICE_NAME          "distance_bound"
#define RESAMPLER_SERVICE_NAME   "resampler"

/** Constants for SSC SNS Services */
#define SLIM_SSC_SNS_CLIENT_SERVICE (0)   /** SSC SNS Client service */

/** Total number of SSC SNS services accessed by SLIM, see slim_SscSnsServiceEnumType */
#define SLIM_SSC_SNS_SERVICE_COUNT  (1)

/* 
* This transaction internally triggered by slim provider 
* i.e request SUID from SSC 
*/
#define SLIM_SSC_SUID_REQ_TRANSACTION_ID    (999)

/*
* Default session key , session key is not used in PB connection
*/
#define SLIM_PB_SESSION_KEY (1)

/**
* SSC Time Macros
* To convert to a uint64 timestamp in 19.2MHz clock,
* the following computation is used:
*
* timestamp_ns = (current_seconds - sns_time_start_time_seconds) * 10^9 + current_nanoseconds
*
* timestamp_ticks = (19200000/1000000000) * timestamp_ns = (12/625)*timestamp_ns
*
* and
* timestamp_ns = (timestamp_ticks/192000000)*1000000000 = (625/12)*timestamp_ticks
*/
#define NANOSECONDS_IN_MILLISECOND 1000000ULL

/* Convert nanoseconds to QTimer ticks */
#define SLIM_NSEC_TO_QTIMER_TICKS( nanoseconds ) ((12ULL*(uint64_t)(nanoseconds))/625ULL)

/* Convert QTimer ticks to nanoseconds */
#define SLIM_QTIMER_TICKS_TO_NSEC( ticks ) (((uint64_t)(ticks) * 625ULL)/12ULL)


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* Structure for PB decoded SNS sensor data */
typedef struct
{
  uint64 t_TimestampMs; /*!< time stamp of sample */
  float  f_Sample[3];      /*!< 3-axis Sample */
  uint8  u_Quality;        /*!< Sample quality for usability */
}slim_SscSensorDataType;

typedef struct
{
  uint8 u_SampleLen;  /*!< Number of samples */
  slim_SscSensorDataType z_Data[SLIM_SENSOR_MAX_SAMPLE_SETS]; /*!< Sensor Data samples*/
} slim_SscSensorEventPayloadArgType;

/* Structure for passing funciton and data as argument during decoded of SNS sensor data */
typedef struct 
{
  sns_std_suid z_Suid;         /*!< Sensor SUID */
  slimServiceEnumT e_ServiceId; 
  slimClientHandleT p_Handle;
  bool (*fn_DecodeClientEventPayload)(pb_istream_t *stream,
                                      const pb_field_t *field,
                                      void **arg);	/*!< Parser function pointer */
  void (*fn_ConvertAndSendData)(slimServiceEnumT e_ServiceId,
                                slimClientHandleT p_Handle,
                                const void* p_DecodeClientEventPayloadArg,
                                uint32 q_DecodeClientEventPayloadArgLen,
                                boolean v_FlushData);
  uint32 q_MsgId;            /*!< Event Type */
  uint64 t_TimestampMs;
  uint32 q_TotalLen;
  uint32 q_DataBufferLen;
  void*  p_DataBuffer;
} slim_SscClientEventPayloadArgType;

typedef void (*StoreSscServiceResponseCallbackType)(int32 l_TxnId, uint64 client_id); 

/*----------------------------------------------------------------------------
* Global Data Definitions
* -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* Static Variable Definitions
* -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief Updates SSC supported SLIM services mask for given SLIM service.

Function updates the status of given service to the services mask. Already
supported services are considered as supported and thus not double-checked.

@param  pz_SscData: Pointer to SSC provider data.
@param  e_Service: SLIM service to check.
@return TRUE if SLIM service is available with SSC provider.
*/
boolean slim_SscUpdateServicesMask
(
  uint32* pq_FoundSnsServicesMask,
  slimServiceEnumT e_Service
);

/**
@brief Callback function for QMI service notification

Function for QMI service notification callback.

@param p_UserHandle Handle of the client.
@param p_ServiceObj Service object
@param e_ServiceEvent Event type. (See qmi_client_notify_event_type)
@param p_NotifyCbData User data passed in qmi_client_register_notify_cb
*/
void slim_PBNotifierCallback
(
  qmi_client_notify_event_type e_ServiceEvent,
  void *p_NotifyCbData
);

/**
 * QMI Error callback.  This would be our notification if the QMI connection
 * was lost.  A typical implementation would re-establish the connection, and
 * resend any active requests.
 *
 * See qmi_client_error_cb. */
void slim_SscErrorCallback(qmi_client_type user_handle,
    qmi_client_error_type error, void *err_cb_data);

/**
* Handle an incoming response message from the Sensors Service.
*/
boolean slim_SscResponseCallback
(
  qmi_client_type user_handle, 
  unsigned int msg_id,
  void *resp_c_struct, 
  unsigned int resp_c_struct_len,
  void *resp_cb_data, 
  qmi_client_error_type transp_err,
  StoreSscServiceResponseCallbackType fn_StoreSscServiceResponseCb
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
void slim_SscIndicationCallback
(
  qmi_client_type p_UserHandle,
  unsigned int q_MsgId,
  void *p_Buf,
  unsigned int q_Len,
  void *p_IndCbData
);

/**
@brief Handles SSC Client Api response message

This function handles SSC Client Api responses. Response is converted and forwarded to
SLIM client.

@param[in] q_MsgLength size of input payload
@param[in] pz_RespMsg Pointer to the response message data
*/
void slim_SscClientApiHandleResponse
(
  uint32 q_MsgLength,
  const void *p_MsgPayload
);

/**
@brief Handles SSC Client Api indication data

This function handles SSC Client Api indications. Data is converted and forwarded to
SLIM client.

@param[in] q_MsgLength size of input payload
@param[in] pz_IndMsg Pointer to the indication message data
*/
void slim_SscClientApiHandleIndication
(
  uint32 q_MsgLength,
  const void *p_MsgPayload
);

/**
@brief Handles SSC Client Api Close Response data

This function handles SSC Client Api Close Response data

@param[in] q_MsgLength size of input payload
@param[in] pz_IndMsg Pointer to the indication message data
*/
void slim_SscClientApiHandleResponseClose
(
  uint32 q_MsgLength,
  const void *p_MsgPayload
);

/**
@brief handler for SSC connection close response.

Function is called whenever some SSC connection is asynchronously closed.
It will complete the client closing by clearing client data if no open
SSC connections are found.

@param  p_CallbackData: Pointer to the callback data (client handle).
*/
void slim_SscCloseConnectionCallback
(
  void *p_CallbackData
);
#endif /* #ifndef SLIM_SSC_PROVIDER_QMI_PB_H */
