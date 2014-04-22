#ifndef SLIM_AMD_QMI_PB_H
#define SLIM_AMD_QMI_PB_H
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
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/provider/ssc/qmisns/pb/inc/slim_ssc_amd_qmipb.h#1 $ */

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

/**
@brief Decodes AMD PB event payloads

@param[in] pz_stream  Pointer to PB stream structure
@param[in] pz_QmdData Pointer to QMD data structure

*/
boolean PBDecodeAmdPayload(pb_istream_t *pz_stream, slimQmdDataStructT* pz_QmdData);

#endif