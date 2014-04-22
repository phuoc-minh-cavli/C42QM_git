#ifndef SLIM_SSC_TIME_H
#define SLIM_SSC_TIME_H
/**
@file
@brief Module implementing the time request to sensor core.

  SLIM can use this module to get the sensor time from SSC.

@ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/provider/ssc/src/slim_ssc_time.h#1 $ */

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
@brief Initiates time request to SSC.

Function initiates time request to SSC. When response to request is gotten,
offset between modem and sensor time can be calculated.

@param[in] pz_SscTimeData Pointer to the SSC data.
@param[in] l_ServiceTxnId Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscTimeGetTimeUpdate
(
  slim_SscConnectionDataStructType *pz_SscTimeData,
  int32 l_ServiceTxnId
);

/**
@brief Handles time response data

This function handles time response. Response is converted and forwarded to
SLIM core.

@param[in] pz_RespMsg Pointer to the response message data
*/
void slim_SscTimeHandleTimeResponse
(
  const slim_SscTimeRespMsgT *pz_RespMsg
);
#endif
//! @}

#endif /* #ifndef SLIM_SSC_TIME_H */
