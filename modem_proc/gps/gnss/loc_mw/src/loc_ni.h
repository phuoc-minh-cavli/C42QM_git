/******************************************************************************
@file:  loc_ni.h
@brief: Location Middleware NI-module

DESCRIPTION
This module defines the loc API routines for a client to handle NI

INITIALIZATION AND SEQUENCING REQUIREMENTS

-----------------------------------------------------------------------------
  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc. 
  Qualcomm Technologies, Inc. All Rights Reserved.
-----------------------------------------------------------------------------
******************************************************************************/

/*=====================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
03/31/17   yh       Loc_Ni clean up
04/27/11   spn      Updated for Loc-Api 2.0

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_ni.h#1 $
$DateTime: 2020/07/21 21:23:44 $
$Author: pwbldsvc $
======================================================================*/

#ifndef LOC_NI_H
#define LOC_NI_H

#include "loc_api_2.h"
#include "pdapi.h"



/*---------------------------------------------------------------------------
@brief
  Function to initialize location NI module
  This function will be called when first location client registers for NI event.

@retval    TRUE   Called successfully
@retval    FALSE  Called unsuccessfully
---------------------------------------------------------------------------*/
boolean locNi_Init(void);

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC request from client that belongs to NI module

@param[in] l_ClientHandle  : Loc API client handle
@param[in] q_QmiLocMsgId   : QMI_LOC Request Message ID
@param[in] p_QmiLocMsgData : QMI_LOC Request Message Data
@param[in] q_QmiLocMsgLen  : QMI_LOC Request Message Data Length

@retval    TRUE   Called successfully
@retval    FALSE  Called unsuccessfully
---------------------------------------------------------------------------*/
boolean locNi_ProcessQmiRequest
(
  loc_client_handle_type l_ClientHandle,
  uint32                 q_QmiLocMsgId,
  const void*            p_QmiLocMsgData,
  uint32                 q_QmiLocMsgLen
);

/*---------------------------------------------------------------------------
@brief
  Function to process cmd request coming from the location middleware task

@param[in] pz_Cmd : LocMW command data

@retval    LOC_API_SUCCESS if success
           other value if failed
---------------------------------------------------------------------------*/
int locNi_ProcessCmdRequest(const LocMW_LocCmdStructT* pz_Cmd);
#endif // LOC_NI_H
