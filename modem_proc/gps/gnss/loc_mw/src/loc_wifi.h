/******************************************************************************
  @file: loc_wifi.h
  @brief: Location API WiFi positioning module

  DESCRIPTION
   Qualcomm Location API WiFi Positioning Module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_wifi.h#1 $
$DateTime: 2020/07/21 21:23:44 $

******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
01/11/16   yh       Loc_Wifi cleanup
07/24/09   dx       Initial version
======================================================================*/

#ifndef LOC_WIFI_H
#define LOC_WIFI_H

#include "loc_api_2.h"
#include "aries_os_api.h"

typedef struct
{
   loc_client_handle_type      l_ClientHandle;
   uint32                      q_QmiLocMsgId;
}locWifi_ModuleDataType;

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC request from client that belongs to WIFI module

@param[in] l_ClientHandle  : Loc API client handle
@param[in] q_QmiLocMsgId   : QMI_LOC Request Message ID
@param[in] p_QmiLocMsgData : QMI_LOC Request Message Data
@param[in] q_QmiLocMsgLen  : QMI_LOC Request Message Data Length

@retval    TRUE   Called successfully
@retval    FALSE  Called unsuccessfully
---------------------------------------------------------------------------*/
boolean locWifi_ProcessQmiRequest
(
   loc_client_handle_type l_ClientHandle,
   uint32                 q_QmiLocMsgId,
   const void*            p_QmiLocMsgData,
   uint32                 q_QmiLocMsgLen
);

boolean locWifi_Init(void);
#endif /* LOC_WIFI_H */
