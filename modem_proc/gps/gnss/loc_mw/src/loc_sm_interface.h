/******************************************************************************
  @file:  loc_pd.h
  @brief: Location Middleware PD module header

  DESCRIPTION
      This module handles the interaction with the PD-engine of GPS.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
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
05/04/17   yh       Debug NMEA
05/19/16   yh       LocMW refactoring
09/03/15   yh       Add loc_fix_criteria_rule to hold the best fix criteria for multiple client
03/24/12   ss       Supporting operation mode settting to effect Geofencing operation mode
03/10/14   ah       Added support for Get Avail WWAN Pos
03/14/13   ssu      Adding support for SUPL MT ES requests.
04/27/11   spn      Updated for Loc-Api 2.0
$Id: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_sm_interface.h#1 $
$DateTime: 2020/07/21 21:23:44 $
======================================================================*/


#ifndef LOC_SM_IFACE_H
#define LOC_SM_IFACE_H

#include "loc_api_2.h"
#include "pdapi_lite.h"
#include "loc_client.h"
#include "aries_os_api.h"

// sometimes the engine would burst at least two measurement reports within very short time
// as every measurement report is converted into a set of NMEA sentences, we have to be able
// to buffer at least 2 sets
#define NMEA_BUFFER_MAX_SIZE 20

/********************************loc_pd array & structs-start********************/


/* Defines the payload of the LOC_CMD_TYPE_REQUEST_FIX IPC
   It enumerates the different conditions when the locMW
   process_fix_request can be called.
*/
typedef enum
{
   eLOC_PD_START_REQUEST,
   eLOC_PD_STOP_REQUEST,
   eLOC_PD_SCHEDULE,
   eLOC_PD_FIX_TIMEOUT,
   eLOC_PD_OPPORTUNISTIC_SCHEDULE,
}loc_pd_fix_request_e_type;



typedef enum
{
   LOC_ENGINE_STATE_ON  = 1,
   LOC_ENGINE_STATE_OFF = 2,
   LOC_ENGINE_STATE_MAX = 0x10000000 /* used to force enum to 32-bit */
} loc_engine_state_e_type;

typedef struct
{
  loc_client_handle_type l_ClientHandle; /* Client which requested the Inject Position */
  boolean                v_InProgress; /* Variable to serialize the Inject Pos request
                                            (current implementation does not support concurrent requests) */
} locPd_InjectPosType;


typedef struct
{
  loc_client_handle_type l_ClientHandle; /* Client which requested the Best Available Position */
  boolean                v_InProgress; /* Variable to serialize the Best Avail Pos request
                                            (current implementation does not support concurrent requests) */
  uint32                 q_TxnId;        /* Transaction Id */
} locPd_GetBestAvailPosType;

typedef struct
{
  loc_client_handle_type l_ClientHandle; /* Client which requested the Available WWAN Position */
  boolean                v_InProgress; /* Variable to serialize the Avail WWAN Pos request
                                            (current implementation does not support concurrent requests) */
  uint32                 q_TxnId;        /* Transaction Id */
} locPd_GetAvailWwanPosType;

typedef struct
{
   boolean                 fix_in_progress;
   boolean                 mo_fix_in_progress;
   uint64                  last_get_position_time;
   uint64                  last_fix_arrival_time;
   uint32                  last_fix_ttf;
   pdsm_lite_pd_option_s_type       pdsm_lite_option;
   pdsm_lite_pd_qos_type            pdsm_lite_qos;
   boolean                     new_pd_info_received;
   boolean                     new_ext_status_info_received;
   boolean                     new_interm_pos_info_received;
   int32                       new_nmea_messages_counter;
   boolean                     new_nmea_info_received;
   boolean                     new_pd_sess_end_status_received;
   boolean                     new_pd_cmd_err_received;
   pdsm_lite_pd_info_s_type         pd_info, pd_info_copy;
   pdsm_lite_pd_end_e_type          pd_sess_end_status, pd_sess_end_status_copy;
   pdsm_lite_pd_cmd_err_e_type      pd_cmd_err, pd_cmd_err_copy;
   pdsm_lite_pd_event_f_type       *pd_event_cb_hook;

   os_TimerMsgType *           fix_process_timer;

   loc_client_handle_type      l_ActiveClientHandle;
   loc_engine_state_e_type     engine_state;

  // qmiLocEventGnssSvInfoIndMsgT_v02        gnss_payload;
   // Cached cell id position or GPS fix


   // timeout value used by loc mw layer, if a position report is not received
   // within the timeout value, loc mw will reset its state and allow subsequent
   // pdsm_lite_get_position calls
   uint32                     session_time_out_ms;

   locPd_InjectPosType        z_InjPos;
   locPd_GetBestAvailPosType  z_GetBestAvailPos;
   locPd_GetAvailWwanPosType  z_GetAvailWwanPos;

   /* client which requested the secure available  position */




   /* hold the best fix criteria for multiple clients */
   locClnt_FixCriteriaType    loc_fix_criteria_rule;
   boolean v_any_victimized_client;


} locPd_ModuleDataType;
/********************************loc_pd array & structs-end********************/

/********************************loc_pa array & structs-start********************/


/* Loc PA module data */
typedef struct
{
  boolean                v_PaCmdInProgress;
  uint32                 q_PaCmdCounterIn;
  uint32                 q_PaCmdCounterOut;
  loc_client_handle_type l_ClientHandle;    /* Loc API client handle */
  uint32                 q_QmiLocMsgId;     /* Corresponding QMI_LOC Id for PDSM Parameter */
  pdsm_lite_pa_cmd_e_type     e_PaCmdType;       /* PDSM Command: GET or SET */
  pdsm_lite_pa_e_type         e_ParamType;       /* PDSM Parameter to GET/SET */
  os_TimerMsgType*       pz_ProcessTimer;

 } locPa_ModuleDataType;

/* IPC data sent from PDAPI to LocMW for Send QMI Indication */
typedef struct
{
  loc_client_handle_type l_ClientHandle;
  pdsm_lite_pa_cmd_e_type     e_PaCmdType;    /* PDSM Command: GET or SET */
  pdsm_lite_pa_info_s_type    z_ParamInfo;    /* PDSM Parameter Data */
  pdsm_lite_pa_cmd_err_e_type e_ParamStatus;
} locPa_PdapiIndMsg;

/********************************loc_pa array & structs-end**********************/

/****************************loc_pd fucntion declaration-start**********************/


/* Initialize the location position determination module */
boolean locPd_Init(void);

/*#9x05: NME not supported*/

// Translate fix configuration from PD api to location API
void locPd_ConvertFixCfg2Pdapi (locQmiShimCltInfoStructT* pz_ClientInfo);

// Process cmd request coming from the location middleware task
int locPd_ProcessCmdRequest (const LocMW_LocCmdStructT*  pz_Cmd);

// Installs an event callback hook for PD events (including QWIP events)
pdsm_lite_client_status_e_type loc_pd_install_event_cb_hook(pdsm_lite_pd_event_f_type *hook);

// Handles the service interaction for CP/UP, NI/MO.


//processes a pd_fix_request
int loc_pd_process_fix_request (loc_pd_fix_request_e_type fix_request_type);


/**
 * Function to process QMI_LOC request from client that belongs to PD module
 * (related to PDAPI Position Determination module)
 *
 * @param[in] l_ClientHandle  : Loc API client handle
 * @param[in] q_QmiLocMsgId   : QMI_LOC Request Message ID
 * @param[in] p_QmiLocMsgData : QMI_LOC Request Message Data
 * @param[in] q_QmiLocMsgLen  : QMI_LOC Request Message Data Length
 *
 * @retval    TRUE   Called successfully
 * @retval    FALSE  Called unsuccessfully
---------------------------------------------------------------------------*/
boolean locPd_ProcessQmiRequest
(
   loc_client_handle_type l_ClientHandle,
   uint32                 q_QmiLocMsgId,
   const void*            p_QmiLocMsgData,
   uint32                 q_QmiLocMsgLen
);
/****************************loc_pd fucntion declaration-end**********************/


/****************************loc_pa fucntion declaration-start**********************/

/*---------------------------------------------------------------------------
@brief
  Function to initialize location PA module

@retval    TRUE   Called successfully
@retval    FALSE  Called unsuccessfully
---------------------------------------------------------------------------*/
boolean locPa_Init(void);

/*---------------------------------------------------------------------------
@brief
  Function to process Timer callback.

  When the timer fires, the location engine has not received the PA callback.
  Notify client of the failed status and reset the state.
---------------------------------------------------------------------------*/
void locPa_ProcessTimerCb(void);

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC request from client that belongs to PA module
  (related to PDAPI Get/Set_Parameters)

@param[in] l_ClientHandle  : Loc API client handle
@param[in] q_QmiLocMsgId   : QMI_LOC Request Message ID
@param[in] p_QmiLocMsgData : QMI_LOC Request Message Data
@param[in] q_QmiLocMsgLen  : QMI_LOC Request Message Data Length
@param[in/out] pe_QmiErr   : QMI_LOC Response error value

@retval    TRUE   Called successfully
@retval    FALSE  Called unsuccessfully
---------------------------------------------------------------------------*/
boolean locPa_ProcessQmiRequest
(
  loc_client_handle_type l_ClientHandle,
  uint32                 q_QmiLocMsgId,
  const void*            p_QmiLocMsgData,
  uint32                 q_QmiLocMsgLen,
  qmi_error_type_v01     *pe_QmiErr
);

/*---------------------------------------------------------------------------
@brief
  Function to send QMI_LOC indication to client for previous QMI_LOC request
  related to PDAPI Get/Set_Parameters

@param[in] p_IpcMsg : IPC Message containing the QMI_LOC indication data

@retval    TRUE    if IPC posted to LocMW successfully
@retval    FALSE   if IPC posted to LocMW unsuccessfully
---------------------------------------------------------------------------*/
boolean locPa_SendQmiIndication
(
  const os_IpcMsgType* p_IpcMsg
);

/*---------------------------------------------------------------------------
@brief
  Function to reset the PA module. This is called when in progress pa cmd is
  done. This is called when PA cmd error fails, or PA event arrives.

  NOTE: This function is always called within LocMW critical section.
---------------------------------------------------------------------------*/
void locPa_ResetModule(void);

/****************************loc_pa fucntion declaration-start**********************/


#endif // LOC_SM_IFACE_H

