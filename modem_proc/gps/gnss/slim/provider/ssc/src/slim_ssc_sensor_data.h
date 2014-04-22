#ifndef SLIM_SSC_SENSOR_DATA_H
#define SLIM_SSC_SENSOR_DATA_H
/**
@file
@brief Module implementing the sensor data request to sensor core.

  SLIM can use this module to start or stop the sensor data
  streaming from SSC.

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
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/provider/ssc/src/slim_ssc_sensor_data.h#1 $ */

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
@brief Initiates sensor data request to SSC.

Function initiates sensor data request to SSC. If request is successfull,
SSC sensor data streaming is either started or stopped.

@param[in] pz_SscData     Pointer to the client SSC sensor data.
@param[in] pz_Txn         Pointer to sensor data request transaction data.
@param[in] l_ServiceTxnId Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscSensorDataEnableSensorReporting
(
  slim_SscDeviceSensorConnectionDataStructType *pz_SscData,
  const slim_EnableSensorDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Handles response data for SMGR request

This function handles SMGR responses. Response is converted and forwarded to
SLIM client.

@param[in] q_MsgLength size of response message data
@param[in] p_MsgPayload Pointer to the response message data
*/
void slim_SscSensorDataHandleResponse
(
  uint32 q_MsgLength,
  const void *p_MsgPayload
);

/**
@brief Handles SMGR indication data

This function handles SMGR indications. Data is converted and forwarded to
SLIM client.

@param[in] q_MsgLength  size of indication message data
@param[in] p_MsgPayload Pointer to the indication message data
*/
void slim_SscSensorDataHandleIndication
(
  uint32 q_MsgLength,
  const void *p_MsgPayload
);

/**
@brief Retrives the last txn-id associated with the input report-id.

Function retrives the last txn-id associated with the input report-id.

@param[in] pz_SscData      Pointer to the client device sensor data.
@param[in] e_ReportId      SSC SMGR report id.

@retval last-txn-id associated with the report id, -1 if not found.
*/
int32 slim_GetSscSensorDataLastTxnIdForReportId
(
   slim_SscDeviceSensorConnectionDataStructType* pz_SscData,
   slim_SscSnsServiceReportEnumType e_ReportId
);

/**
@brief Sets the flag to consolidate Accel/Gyro/uncalMag data request.

Function sets the flag to consolidate Accel/Gyro/uncalMag data request.

@param[in] v_ConsolidatedReq      input flag

@return None
*/
void slim_SetConsolidatedAGMRequestFlag
(
   boolean v_ConsolidatedReq
);


//! @}

#endif /* #ifndef SLIM_SSC_SENSOR_DATA_H */
