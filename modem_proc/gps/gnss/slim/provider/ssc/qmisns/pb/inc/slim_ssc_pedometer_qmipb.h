#ifndef SLIM_PEDOMETER_QMI_PB_H
#define SLIM_PEDOMETER_QMI_PB_H
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
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/provider/ssc/qmisns/pb/inc/slim_ssc_pedometer_qmipb.h#1 $ */

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/

#include "slim_client_types.h"
/*----------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
* Type Declarations
* -------------------------------------------------------------------------*/

/*
* Decode an element of sns_client_event_msg::events.  This function will be
* called by nanopb once per each element in the array.
*
* @param[in] pz_stream input data pz_stream to decode
* @param[in] pz_field  structure of input data steam
* @param[in] ppz_arg structure containing Sensor-specific decode function along with data
*/
bool slim_PBDecodeStepEvent(
	pb_istream_t *pz_stream,
	const pb_field_t *pz_field,
	void **ppz_arg);
#endif
