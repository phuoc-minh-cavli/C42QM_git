/******************************************************************************
  @file:  loc_conn.c
  @brief: Location Middleware User Plane Data Connection Manager

  DESCRIPTION
  This module contains routines used to manage User Plane Data Connections
 
 
  -----------------------------------------------------------------------------
  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
  All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
 ******************************************************************************/


/*=============================================================================
$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_conn.c#1 $
$DateTime: 2020/07/21 21:23:44 $
$Author: pwbldsvc $
=============================================================================*/

#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */
#include "msg.h"

#include "msg.h"
#include "queue.h"

#include "aries_os_api.h"
#include "pdapi_lite.h"
#include "pdsm_atl.h"

#include "loc_api_2.h"
#include "loc_client.h"
#include "loc_conn.h"
#include "loc_xtra.h"
#include "loc_sm_interface.h"
#include "loc_ni.h"
#include "loc_wifi.h"
#include "loc_api_internal.h"
#include "loc_qmi_shim.h"
#include "loc_utils.h"






// Routine to initialize the location middleware conn module.


// Routine to Deinitialize the location middleware conn module.

// Callback function when GPS engine needs a data data session to be brought up on A-proc


// callback function when GPS engine is done with the data session




