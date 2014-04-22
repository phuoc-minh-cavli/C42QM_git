#ifndef SLIM_SSC_PB_PARSER_H
#define SLIM_SSC_PB_PARSER_H
/**
  @file
  @brief Implementation of SSC-based sensor data requests.

 This module implements the sensor data request handling when sensor provider
 is SSC.


  @ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2017 Qualcomm Technologies, Inc.
                      All Rights Reserved.
           Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/provider/ssc/qmisns/pb/inc/slim_ssc_pb_parser.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "slim_ssc_sensor_data_qmipb.h"

//! @addtogroup slim_SSCProvider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*! Struct for SSC SMGR request response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SSC SMGR response payload is valid */

  sns_client_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscClientRespMsgT;

/*! Struct for SSC SMGR data indication */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscIndHeaderStructT z_IndHeader;
  /**< Indication header */

  qmi_client_type p_QmiConnHandle;
  /**< QMI connection handle */

  boolean b_IndData_valid;
  /**< TRUE if SSC indication payload is valid */

  sns_client_report_ind_msg_v01 z_IndData;
  /**< Indication data */

  sns_std_suid z_Suid;
  /**< SUID for the Indication data */

} slim_SscClientIndMsgT;

/*! Union for SMGR messages. */
typedef union 
{
  slim_SscClientRespMsgT z_RespMsg;
  /**< Response */

  slim_SscClientIndMsgT z_IndMsg;
  /**< Indication */
} slim_SscClientMsgUnionType;

/**
* Utility structure used to encode a string or already encoded message
* into another Protocol Buffer message.
*/
typedef struct
{
  /* Buffer to be written, or reference to read buffer */
  void const *buf;
  /* Length of buf */
  size_t buf_len;
} slim_sns_buffer_arg;

/**
* Structure to be used with pb_decode_float_arr_cb
*/
typedef struct
{
  /* Where to place the decoded float array */
  float *arr;
  /* Length of the float array */
  uint8_t arr_len;
  /* Array index used for decoding.
  Must be initialized to 0 prior to calling pb_decode_float_arr_cb*/
  uint8_t *arr_index;
} slim_pb_float_arr_arg;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
//! @}

/**
@brief Send multiple requests to the SUID Lookup Sensor for the complete list of SUIDs

Send Multiple requests to the SUID Lookup Sensor for the complete list of SUIDs

@param[in] p_qmi_handle        QMI connecton created in create_client_conn
@param[in] fn_ResponseCallback response call back

@return TRUE if request was sent successfully, FALSE otherwise
*/
boolean slim_SscGetSuidList
(
  qmi_client_type pz_qmi_handle, 
  qmi_client_recv_msg_async_cb fn_ResponseCallback
);

/**
@brief Map sensor name identified by proto files to sensor Type identified by client 

Map sensor name identified by proto files to sensor Type identified by client

@param[in] p_servName		name of sensor defined in proto files
@return Service Enumeration Type
*/
slimServiceEnumT slim_MapServiceNameToServiceType
(
  const char* p_servName
);

/**
@brief Map suid to sensor Type identified by client

Map suid to sensor Type identified by client

@param[in] suid		suid published by SSC
@return Sensor Type
*/
slimServiceEnumT slim_MapSuidToServiceType
(
  const sns_std_suid* pz_Suid
);

/**
@brief Map sensor Type identified by client to suid

Map sensor Type identified by client to suid

@param[in] e_sensorType		sensor Type
@param[out] pz_suid		suid for given sensor Type
@return ture if found else FALSE
*/
boolean slim_MapServiceTypeToSuid
(
  slimServiceEnumT e_serviceType, 
  sns_std_suid *pz_suid
);

/**
@brief Map sensor Type identified by client to suid

Map sensor Type identified by client to suid

@param[in] e_sensorType		sensor Type
@param[out] pz_suid		suid for given sensor Type
@return ture if found else FALSE
*/
boolean slim_MapServiceNameToSuid
(
  const char* service_name,
  sns_std_suid *pz_suid
);

/**
@brief Check if given SUID belongs to Lookup Service

Check if given SUID belongs to Lookup Service

@param[in] pz_suid pointer to SUID 
@return TRUE / FALSE
*/
boolean slim_SscIsLookupSuid
(
  const sns_std_suid* pz_suid
);

/**
* Copy an already encoded payload into a message field.  For the purpose of
* this function, an encoded submessage is identical to a character string.
*/
boolean slim_SscPbEncodePayload(pb_ostream_t *stream, const pb_field_t *field, void *const *arg);

boolean slim_SscIsServiceSuidAvailable(slimServiceEnumT e_Service);

/**
@brief Handle a received event message

Handle a received event message .i.e decode envent and send it to respective client

@param[i] pz_Suid      Sensor SUID
@param[i] p_Payload    pointer to event payload data
@param[i] q_PayloadLen Length (in bytes) Event payload
@param[i] p_Handle     slim Client handle to receive this event data
@param[i] p_QmiConnHandle     QMI Client handle to receive this event data
*/
void slim_SscDecodeClientEventMsg
(
  const sns_std_suid* pz_Suid,
  const void* p_Payload,
  uint32 q_PayloadLen,
  slimClientHandleT p_Handle,
  qmi_client_type p_QmiConnHandle
);

/**
@brief Decode the common msg_ids from of _SNS_STD_SENSOR type.
Decode the common msg_ids from of _SNS_STD_SENSOR type.

@param[in] pz_stream  pointer to sensor data pz_stream
@param[in] pz_field   pointer to sensor data fields
@param[in] ppz_arg    double pointer to input arguments
@param[in] q_MsgId    _SNS_STD_SENSOR msg id
@param[in] pv_DecodeRetVal  TRUE if decode successful, FALSE otherwise

@return TRUE if msg_id is _SNS_STD_SENSOR type, FALSE otherwise
*/
boolean slim_PBDecodeSnsStdSensorMsgId
(
  pb_istream_t *pz_stream,
  const pb_field_t *pz_field,
  void **ppz_arg,
  uint32 q_MsgId,
  boolean* pv_DecodeRetVal
);

/**
@brief Prints debug log for SUID.

@param[in] z_suid  SUID to be printed
*/
void slim_PBLogSUID
(
  sns_std_suid z_suid 
);

/**
@brief Checks if the SUID service discovery is complete, 
         i.e., we have SUIDs for all the services.

@return TRUE if complete, FALSE otherwise
*/
boolean slim_PBIsSUIDTableComplete(void);

/**
@brief Returns SLIM service type for this QMI service connection.

Function returns SLIM service type for this QMI service connection.

@param[in] p_QmiConnHandle  QMI connection handle.
@param[in] p_Handle         SLIM client handle.

@return SLIM service type
*/
slimServiceEnumT slim_GetServiceIdFromQmiHandle
(
  qmi_client_type p_QmiConnHandle, 
  slimClientHandleT p_Handle
);

/**
@brief Updates the pending SUID response count (slimz_PendingSuidRespCount).
  Specifically, increment when sending a SUID request, decrement when response received. 

@param[in] v_IncreaseCount        flag to indicate increment or decrement 

@return None
*/
void slim_UpdatePendingSuidRespCount(boolean v_IncreaseCount);

#endif /* #ifndef SLIM_SSC_PB_PARSER_H */
