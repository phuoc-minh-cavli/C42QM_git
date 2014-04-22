/******************************************************************************
  @file:  loc_api_internal.h
  @brief: Location Middleware internal API header

  DESCRIPTION
      Declares the internal data types and macros
      used by loc middleware

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2010 - 2015 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014, 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015 - 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ------------------------------------------------------- 
08/21/19   ska  Added support for NMEA GSV
03/31/17   yh    Loc_NI cleanup
02/17/17   skm   BLE support for LPPE Phase 2 
05/19/16   yh    LocMW refactoring
01/11/16   yh    Loc_Wifi cleanup
10/21/15   sudj  Fix Crash caused by bad AON configuration
09/30/15   sj    Added hysterisis for unsolicited status indications 
06/11/15   sj    LB 2.0 integration
04/02/15   ssu   Batch Dwell notification for geofence.
03/02/15   sj    Use replaceGf() API from GM instead of Del/Add seq  
02/08/13   ssu   Move to the DOG Heartbeat mechanism on DIME and beyond   
04/07/10   ns    Added loc_mw_task_started flag to denote that loc mw
                 task has started

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_api_internal.h#1 $
$Author: pwbldsvc $
$DateTime: 2020/07/21 21:23:44 $
======================================================================*/

#ifndef FEATURE_LOC_API_INTERNAL_H
#define FEATURE_LOC_API_INTERNAL_H

#include "tm_api.h"
#include "gps_variation.h"
#include "queue.h"
#include "loc_api_2.h"
#include "pdapi_lite.h"
#include "loc_conn.h"
#include "loc_xtra.h"
#include "loc_client.h"
#include "loc_sm_interface.h"
#include "loc_ni.h"
#include "loc_wifi.h"
#include "msg.h"
#include "aries_os_globals.h"
#include "aries_os_api.h"

/*Include to get the data-type definitions for QMI-LOC */
#include "location_service_iot_v02.h"

#include "slim_task.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define LOC_HANDLE_REGISTERED_CLIENTS (-2)

#define LOC_MSG_LOW_0( str )           MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_LOW, str )
#define LOC_MSG_LOW_1( str, a )        MSG_1(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_LOW, str, a )
#define LOC_MSG_LOW_2( str, a, b )     MSG_2(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_LOW, str, a, b )
#define LOC_MSG_LOW( str, a, b, c )       MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_LOW, str, a, b, c )

#define LOC_MSG_MED_0( str )           MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, str )
#define LOC_MSG_MED_1( str, a )        MSG_1(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, str, a )
#define LOC_MSG_MED_2( str, a, b )     MSG_2(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, str, a, b )
#define LOC_MSG_MED( str, a, b, c )       MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, str, a, b, c )

#define LOC_MSG_HIGH_0( str )                     MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH, str )
#define LOC_MSG_HIGH_1( str, a )                  MSG_1(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH, str, a )
#define LOC_MSG_HIGH_2( str, a, b )               MSG_2(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH, str, a, b )
#define LOC_MSG_HIGH( str, a, b, c )      MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH, str, a, b, c )
#define LOC_MSG_HIGH_4( str, a, b, c, d )         MSG_4(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH, str, a, b, c, d )
#define LOC_MSG_HIGH_5( str, a, b, c, d, e )      MSG_5(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH, str, a, b, c, d, e )
#define LOC_MSG_HIGH_6( str, a, b, c, d, e, f )   MSG_6(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH, str, a, b, c, d, e, f )


#define LOC_MSG_ERROR_0( str )         MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR, str )
#define LOC_MSG_ERROR_1( str, a )      MSG_1(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR, str, a )
#define LOC_MSG_ERROR_2( str, a, b )   MSG_2(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR, str, a, b )
#define LOC_MSG_ERROR( str, a, b, c )     MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR, str, a, b, c )

#define LOC_MSG_FATAL_0( str )         MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_FATAL, str )
#define LOC_MSG_FATAL_1( str, a )      MSG_1(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_FATAL, str, a )
#define LOC_MSG_FATAL_2( str, a, b )   MSG_2(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_FATAL, str, a, b )
#define LOC_ERR_FATAL( str, a, b, c )     MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_FATAL, str, a, b, c )

// MACRO Initialize Critical Section
#define LOC_MW_INIT_CRIT_SECTION( mutex_ctrl_block )                     \
  { os_MutexInit( mutex_ctrl_block, MUTEX_DATA_ONLY_CONTEXT ); }

// MACRO Enter Critical Section
#define LOC_MW_ENTER_CRIT_SECTION( mutex_ctrl_block )                    \
  { os_MutexLock( mutex_ctrl_block ); }

// MACRO Leave Critical Section
#define LOC_MW_LEAVE_CRIT_SECTION( mutex_ctrl_block )                    \
  { os_MutexUnlock( mutex_ctrl_block ); }

/* This Marco is only used to check the sanity of uint8_t defined in location_service_v02.h
   TRUE is "1" and FALSE is "0", ie the valid values are less than "2"*/
#define LOC_MW_IS_VALID_BOOLEAN(value)    ((value)<2)
#define loc_middleware_msg_id_e_type tm_ipc_msg_id_type

typedef struct
{
   // Module initialization is done when the first client registers for the service
   volatile boolean         loc_mw_task_started;
   boolean                  qmi_loc_enabled;
   boolean                  pd_module_initialized;
   /*#9x05: NMEA not supported*/
   boolean                  pa_module_initialized;
   boolean                  conn_module_initialized;
   boolean                  xtra_module_initialized;
   boolean                  ni_module_initialized;
   boolean                  geofence_module_initialized;
   boolean                  aon_module_initialized;
   boolean                  v_nv_module_initialized;
   boolean                  nmea_module_needed;    // Whether NMEA module is needed or not

   pdsm_lite_client_id_type      pdapi_client_id;       // Used for pdapi events
   pdsm_lite_client_id_type      xtra_client_id;  // Used for xtra events

   locClnt_ModuleDataType          z_locClnt_Data;
   locPd_ModuleDataType            z_locPd_Data;
   qmiLocEventGnssSvInfoIndMsgT_v02 z_locSV_Data;
   locXtra_ModuleDataType          z_locXtra_Data;
   locPa_ModuleDataType            z_locPa_Data;
   os_MutexBlockType	           crit_sec; // critical section for the module
   os_MutexBlockType               pd_crit_sec; // critical section for position module
   boolean                         wifi_module_initialized;
   pdsm_lite_client_id_type             wifi_client_id;
   os_MutexBlockType               wifi_crit_sec; // critical section for wifi module
   os_MutexBlockType               xtra_crit_sec; // critical section for xtra module
   os_MutexBlockType               client_crit_sec; // critical section for client module
} loc_middleware_data_s_type;

extern loc_middleware_data_s_type loc_middleware_data;
#if 0 
#define LOC_MIDDLEWARE_TIMER_ID_FIX  (2)
#define LOC_MIDDLEWARE_TIMER_ID_XTRA (3)
#define LOC_MIDDLEWARE_TIMER_ID_PA   (4)
#define LOC_MIDDLEWARE_TIMER_ID_SLIM_RESERVED_MIN IPC_SLIM_TIMER_ID_MIN
#define LOC_MIDDLEWARE_TIMER_ID_SLIM_RESERVED_MAX IPC_SLIM_TIMER_ID_MAX

#define LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_MIN (10000)
#define LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_HYSTERISIS (10999)
#define LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_MAX (LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_HYSTERISIS)
#endif
/*---------------------------------------------------------------------------
@brief
  Function to send IPC to LocMW task for servicing Indication to previous
  QMI_LOC request

@param[in] l_ClientHandle : Loc client handle
@param[in] q_QmiIndMsgID  : QMI_LOC Msg Id
@param[in] p_QmiIndMsg    : QMI_LOC Indication Msg Data
@param[in] q_LocApiStatus : Parameter Status from Loc API

@retval    TRUE    if IPC posted to LocMW successfully
@retval    FALSE   if IPC posted to LocMW unsuccessfully
---------------------------------------------------------------------------*/
boolean locMW_SendQmiIndicationProxy
(
   loc_client_handle_type     l_ClientHandle,
   uint32                     q_QmiIndMsgID,
   void*                      p_QmiIndMsg,
   uint32                     q_LocApiStatus
);

/*---------------------------------------------------------------------------
@brief
  Function to handle QMI_LOC indication to client for previous QMI_LOC request
  which is handled by LocMW directly.

@param[in] p_IpcMsg : IPC Message containing the QMI_LOC indication data

@retval    TRUE    if IPC message is handled successfully
@retval    FALSE   if IPC message is handled unsuccessfully
---------------------------------------------------------------------------*/
boolean locMW_HandleQmiIndication
(
  const os_IpcMsgType* p_IpcMsg
);

/*---------------------------------------------------------------------------
@brief
  Function to send IPC to LocMW task for LocMW command

@param[in] l_ClientHandle : Loc client handle
@param[in] e_LocCmdType   : LocMW command ID
@param[in] t_EventType    : LocMW event type
@param[in] p_LocCmdData   : LocMW command data

@retval    TRUE    if IPC posted to LocMW successfully
@retval    FALSE   if IPC posted to LocMW unsuccessfully
---------------------------------------------------------------------------*/
boolean locMW_SendLocCmdProxy
(
   loc_client_handle_type     l_ClientHandle,
   LocMW_LocCmdEnumT          e_LocCmdType,
   qmiLocEventRegMaskT_v02      t_EventType,
   void*                      p_LocCmdData
);

/*---------------------------------------------------------------------------
@brief
  Function to handle LocMW command

@param[in] p_IpcMsg : IPC Message containing the LocMW command data

@retval    TRUE    if IPC message is handled successfully
@retval    FALSE   if IPC message is handled unsuccessfully
---------------------------------------------------------------------------*/
boolean locMW_HandleLocCmd
(
  const os_IpcMsgType* p_IpcMsg
);

#ifdef __cplusplus
}
#endif


#endif // FEATURE_LOC_API_INTERNAL_H
