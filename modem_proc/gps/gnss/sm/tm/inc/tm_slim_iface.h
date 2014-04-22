#ifndef TM_SLIM_IFACE_H
#define TM_SLIM_IFACE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM - PD API Interface Module

GENERAL DESCRIPTION
  This module handles IS801 specific routines

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/inc/tm_slim_iface.h#1 $  
  
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/17/17   skm     BLE and Barometer support for LPPE Phase 2 
01/15/17   skm     Initial Version 

============================================================================*/
#include "slim_api.h"
#include "slim_client_types.h"
#include "slim_client_msg_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TM_SLIM_SENSOR_TXN_DISABLE   0
#define TM_SLIM_SENSOR_TXN_ENABLE    1


/*===========================================================================
FUNCTION  tm_auxTechSlimNotifyDataCallback

DESCRIPTION
  SLIM callback function provided with slim_open.to receive notification from
  SLIM module. TM_AUXTECH should copy over the data and send an IPC to TM thread
  since Sensor data processing should happen in client context.

DEPENDENCIES
  tm_SlimInit () function should be called before calling slim_open()
 
PARAMETERS 
  t_CallbackData:    client provided data.
  pz_MessageHeader:  pointer to message header
  p_Message       :  pointer to data
 
RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
void tm_slimNotifyDataCallback
(
  uint64 t_CallbackData,
  const slimMessageHeaderStructT *pz_MessageHeader,
  void *p_Message
);

/*=============================================================================

FUNCTION
  tm_slimHandleSensorReport

DESCRIPTION
  Sensor Baro information from SLIM to SM

DEPENDENCIES
  None.

PARAMETERS
  t_CallbackData   : Client specific callback data
  pz_MessageHeader : Message header pointer
  p_Message        : Message pointer

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/
void tm_slimHandleSensorReport(uint64 t_CallbackData,
                               const slimMessageHeaderStructT *pz_MessageHeader,
                               void *p_Message);

#ifdef __cplusplus
}
#endif

#endif /* TM_PDAPI_IFACE_H */
