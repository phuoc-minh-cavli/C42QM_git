/*============================================================================
@file loc_aon.c

loc MW AlwaysOn service module implementation

GENERAL DESCRIPTION

This file contains the AlwaysOn service module implementation.

Copyright (c) 2014 - 2015 QUALCOMM Atheros, Inc.
All Rights Reserved.
QCA Proprietary and Confidential.
Copyright (c) 2015 - 2019 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*============================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.

 $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_aon.c#1 $
 $DateTime: 2020/07/21 21:23:44 $
 $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/04/17   sum   Deregister OTB client when QMI_LOC client connection closed.
10/21/15   sudj  Fix Crash caused by bad AON configuration 
09/30/15    sj    Added hysterisis for unsolicited status indications 
08/13/15   sj    Added default configuration of GM from NV 
04/06/15   sj    LB 2.0 integration 
02/25/15   sj    Fixed status indications for client specfic cases
02/19/15   sj    Added reqId to status indications sent back by DBT
01/26/15   sj    Added correct transalation for positionType field
01/19/15   sj    Set default for maxLatency to 1 sec 
09/22/14   sj    Initial version

=============================================================================*/

#include "msg.h"
#include "comdef.h"
#include "location_service_iot_v02.h"
#include "loc_api_internal.h"
#include "loc_qmi_shim.h"
#include "loc_batching.h"
#include "aon_api.h"
#include "loc_conv_locEng_qmiLoc.h"
#include "loc_aon.h"
#include "loc_api_2.h"
#include "loc_client.h"
#include "sm_nv_efs.h"
#include "time_svc.h"
#include "qw.h"

#define LOC_AON_MAX_REQUESTS (10)

#define LOC_AON_INVALID_INDEX  (-1)

#define LOC_AON_BATCHING_DEFAULT_REQ_ID (0)

#define LOC_AON_OTB_DEFAULT_REQ_ID  (256) //only one OTB client, the client ID is predefined

static boolean locSendDbtPositionInd
(
   loc_client_handle_type                    l_ClientHandle,
   qmiLocEventDbtPositionReportIndMsgT_v02  *pz_dbtPosInd
);


//Request specific status indications
static void locAonReqStatusCallback
(
   uint32 reqId,
   aonSessionStatusType sessionStatus
);

static locAonStatusEnumT locAonOtbStopInternal
(
   loc_client_handle_type l_ClientHandle,
   uint32 q_LocReqId,
   boolean v_ForceStop
);

/* Map a locClientHandle and req Id to a unique Aon reqID */
static struct
{
  loc_client_handle_type locClientHandle;
  uint32 locReqId;
  uint32 aonReqId;
  boolean used;
  locAonReqTypeEnumT reqType;
  boolean startPending;
  locAonBatchingConfig  batchCfg;
}g_AonClientReqMap[LOC_AON_MAX_REQUESTS];


/* to generate  aon req IDs */
static uint32 g_AonReqCtr = 0;

/*GF radius in meters used for batching */
static uint32 g_AonBatchingGfRadius = 0;

/* Batching config masks */
static uint32 g_AonBatchingConfigMask = 0;

/* Data Received from AON in Status CB, process in LocMW context */
typedef struct
{
  uint32            aonReqID;
  aonSessionStatusType sessionStatus;
}locAonStatusCBData;

/* Data Received from AON in Location CB, process in LocMW context */
typedef struct
{
  uint32                       aonReqID;
  aonPositionType              positionType;
  locEngPositionReportMsgT_v01 locPosRpt;
}locAonLocationCBData;

/** locAonAddRequest
* Adds the new location request to the loc_aon request queue and 
* allocates a new equivalent AON request id to idetify this 
* locClientHandle/locReqId pair 
* @return : TRUE: Successfully added to the queue
*           FALSE: Unsuccessful due to max supported
*           transactions
*/
static boolean locAonAddRequest
(
   loc_client_handle_type locClientHandle,
   uint32 locReqId,
   locAonReqTypeEnumT reqType,
   uint32 *pAonReqId,
   locAonBatchingConfig *pBatchCfg,
   int32 *idx
)
{
   int i =0, j=0;
   if (NULL == pAonReqId)
   {
      LOC_MSG_ERROR(" locAonAddRequest:NULL aon req id input", 0, 0,0);
      return FALSE;
   }

   if (NULL == idx)
   {
      LOC_MSG_ERROR(" locAonAddRequest:NULL idx", 0, 0,0);
      return FALSE;
   }

   for (i=0; i<LOC_AON_MAX_REQUESTS; i++)
   {
      if (FALSE == g_AonClientReqMap[i].used)
      {
         //found a free entry
         break;
      }
   }

   if (i<LOC_AON_MAX_REQUESTS)
   {
      g_AonClientReqMap[i].used = TRUE;
      g_AonClientReqMap[i].locClientHandle = locClientHandle;
      g_AonClientReqMap[i].locReqId = locReqId;
      g_AonClientReqMap[i].reqType = reqType;
      g_AonClientReqMap[i].aonReqId = g_AonReqCtr++;

      if (reqType == eLOC_AON_REQ_BATCH)
      {
         if (pBatchCfg != NULL)
         {
            g_AonClientReqMap[i].batchCfg = *pBatchCfg;
         }
         else
         {
            LOC_MSG_ERROR("No Batching Config Cached!! Null pointer!", 0, 0, 0);
         }
      }

      /* ensure that the request id is already not assigned to another request */
      for (j=0; j<LOC_AON_MAX_REQUESTS && j!=i; j++)
      {
         if ((TRUE == g_AonClientReqMap[j].used) &&
            (g_AonClientReqMap[i].aonReqId == g_AonClientReqMap[j].aonReqId))
         {
            g_AonClientReqMap[i].aonReqId =  g_AonReqCtr++;
            j = 0; /* start again from first entry as after 
                  incrementing the reqId could now be equal to a previously compared value */
         }
      }

      *pAonReqId = g_AonClientReqMap[i].aonReqId;
      *idx = i;
      LOC_MSG_HIGH(" locAonAddRequest:pAonReqId %d idx %d generated for Client Handle %d",
         *pAonReqId, i, locClientHandle);
      return TRUE;
   }

   LOC_MSG_ERROR(" locAonAddRequest:Exceeded max number of concurrent" 
      "loc Aon requests, allowed %u requests", LOC_AON_MAX_REQUESTS, 0,0);
   return FALSE;
}
/** locAonDeleteRequest
* Deletes the location request in the loc_aon request queue 
* idetified by the AON reqID and frees a slot in the request 
* queue 
* @return : TRUE: Successfully removed from the queue
*           FALSE: Unsuccessful as it was not found
*/
static boolean locAonDeleteRequest
(
  uint32 aonReqId
)
{
  int32 i = 0;
  for (i=0; i<LOC_AON_MAX_REQUESTS; i++)
  {
    if ((TRUE == g_AonClientReqMap[i].used) &&
      (aonReqId == g_AonClientReqMap[i].aonReqId))
    {
      g_AonClientReqMap[i].used = FALSE;
      g_AonClientReqMap[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID;
      g_AonClientReqMap[i].locReqId = 0;
      LOC_MSG_MED("locAonDeleteRequest: for aonReqId %d",
                   aonReqId, 0, 0);
      return TRUE;
    }
  }

  LOC_MSG_ERROR("locAonDeleteRequest: Did not find the aonReqId %d",
    aonReqId, 0, 0);
  return FALSE;
}
/** locAonGetAonReqId
* Retrieve the AON request ID associated with the given 
* locClientHandle/locReqId pair 
* @return : TRUE: Successfully retrieved
*           FALSE: Unsuccessful as it was not found
*/
static boolean locAonGetAonReqId
(
   loc_client_handle_type locClientHandle,
   uint32 locReqId,
   locAonReqTypeEnumT reqType,
   uint32 *pAonReqId,
   int32 *idx
)
{
  int32 i = 0;
  if (NULL == pAonReqId)
  {
    LOC_MSG_ERROR("locAonGetAonReqId: NULL input ", 0, 0, 0);
    return FALSE;
  }
  if (NULL == idx)
  {
    LOC_MSG_ERROR("locAonGetAonReqId: NULL idx ", 0, 0, 0);
    return FALSE;
  }

  for (i=0; i<LOC_AON_MAX_REQUESTS; i++)
  {
    if ((TRUE == g_AonClientReqMap[i].used) &&
      (locClientHandle == g_AonClientReqMap[i].locClientHandle) &&
      (locReqId == g_AonClientReqMap[i].locReqId) &&
      (reqType == g_AonClientReqMap[i].reqType))
    {
      *pAonReqId = g_AonClientReqMap[i].aonReqId;
      *idx = i;
      LOC_MSG_MED("locAonGetAonReqId: Found AonReqId %d for Client Handle %d reqId %d",
       *pAonReqId, locClientHandle, locReqId);
      return TRUE;
    }
  }

  LOC_MSG_ERROR("locAonGetAonReqId: Did not find the req clnt hdl = %d, reqId = %d",
    locClientHandle, locReqId,0);
  return FALSE;
}
/** locAonGetClientReqInfo
* Retrieve the LocClientHandle/LocReqId info associated with the
* given aonReqId
* @return : TRUE: Successfully retrieved
*           FALSE: Unsuccessful as it was not found
*/
static boolean locAonGetClientReqInfo
(
   uint32 aonReqId,
   loc_client_handle_type *pLocClientHandle,
   uint32 *pLocReqId,
   locAonReqTypeEnumT *pReqType,
   int32 *idx
)
{
  int32 i = LOC_AON_INVALID_INDEX;
  if (NULL == pLocClientHandle || NULL == pLocReqId || 
      NULL == pReqType || NULL == idx)
  {
    LOC_MSG_ERROR("locAonGetClientReqInfo: NULL input ", 0, 0, 0);
    return FALSE;
  }

  for (i=0; i<LOC_AON_MAX_REQUESTS; i++)
  {
     //todo, remove later
       LOC_MSG_MED("locAonGetClientReqInfo: map[%d], used %d, locClientHandle %d",
                     i, g_AonClientReqMap[i].used, g_AonClientReqMap[i].locClientHandle);
       LOC_MSG_MED("locAonGetClientReqInfo: locReqId %d, reqType %d, aonReqId %d",
                     g_AonClientReqMap[i].locReqId, g_AonClientReqMap[i].reqType, g_AonClientReqMap[i].aonReqId);
    if ((TRUE == g_AonClientReqMap[i].used) &&
      (aonReqId == g_AonClientReqMap[i].aonReqId))
    {
      *pLocClientHandle = g_AonClientReqMap[i].locClientHandle;
      *pLocReqId = g_AonClientReqMap[i].locReqId;
      *pReqType = g_AonClientReqMap[i].reqType;
      *idx = i;
      LOC_MSG_MED("locAonGetClientReqInfo: found aonReqId %u, locClientHandle %d, ReqType %d",
                     aonReqId,*pLocClientHandle,*pReqType);
      return TRUE;
    }
  }

  LOC_MSG_ERROR("locAonGetClientReqInfo: Did not find the aonReqId = %d",
    aonReqId,0,0);
  return FALSE;
}
/** locAonCheckIfLocReqIdPresent
* Determine if the LocClientHandle/LocReqId info is present in 
* the request queue 
* @return : TRUE: Present
*           FALSE: Absent
*/
static boolean locAonCheckIfLocReqIdPresent
(
   loc_client_handle_type locClientHandle,
   uint32 locReqId,
   locAonReqTypeEnumT reqType
)
{
  int32 i = 0;
  for (i=0; i<LOC_AON_MAX_REQUESTS; i++)
  {
     //todo, remove later
       LOC_MSG_MED("locAonCheckIfLocReqIdPresent: map[%d], used %d, locClientHandle %d",
                     i, g_AonClientReqMap[i].used, g_AonClientReqMap[i].locClientHandle);
       LOC_MSG_MED("locAonCheckIfLocReqIdPresent: locReqId %d, reqType %d",
                     g_AonClientReqMap[i].locReqId, g_AonClientReqMap[i].reqType, 0);
    if ((TRUE == g_AonClientReqMap[i].used) &&
      (locClientHandle == g_AonClientReqMap[i].locClientHandle) &&
      (locReqId == g_AonClientReqMap[i].locReqId) &&
      (reqType == g_AonClientReqMap[i].reqType))
    {
      return TRUE;
    }
  }

  LOC_MSG_MED("locAonCheckIfLocReqIdPresent: Did not find the req client Handle = %d, locReqId = %d",
    locClientHandle, locReqId,0);
  return FALSE;
}

/** locAonCheckIfLocClientPresent
* Determine if the given locClientHandle is present in the 
* request queue 
* @return : TRUE: Present
*           FALSE: Absent
*/
boolean locAonCheckIfLocClientPresent
(
   loc_client_handle_type locClientHandle,
   locAonReqTypeEnumT reqType
)
{
  int32 i = 0;
  for (i=0; i<LOC_AON_MAX_REQUESTS; i++)
  {
    if ((TRUE == g_AonClientReqMap[i].used) &&
        (locClientHandle == g_AonClientReqMap[i].locClientHandle) &&
        (reqType == g_AonClientReqMap[i].reqType)  )
    {
      return TRUE;
    }
  }

  LOC_MSG_MED("locAonCheckIfLocClientPresent: Did not find the clnt hdl = %d, reqType = %d",
               locClientHandle,reqType,0);
  return FALSE;
}
/** locAonSetUnableToTrack
* Setter for unable to track state flag associted with a 
* particaul LOC client handle
* @return : TRUE: Successfully set
*           FALSE: Could not find this client
*/
static boolean locAonSetUnableToTrack
(
   loc_client_handle_type locClientHandle,
   boolean value
)
{
  locQmiShimCltInfoStructT *pClientInfo = locClnt_FindHandle(locClientHandle);

  LOC_MSG_MED("locAonSetUnableToTrack : client handle = %d, value = %d", locClientHandle, value, 0);

  if (NULL == pClientInfo)
  {
    LOC_MSG_ERROR(" locAonSetUnableToTrack: Invalid client Handle %d",locClientHandle,0,0);
    return FALSE;
  }
  pClientInfo->batching_info.unable_to_track_reported = value;
  return TRUE;
}

/** locAonGetUnableToTrack
* Getter for unable to track state flag associted with a 
* particaul LOC client handle
* @return : TRUE: Successfully got the value
*           FALSE: Could not find this client
*/
static boolean locAonGetUnableToTrack
(
   loc_client_handle_type locClientHandle,
   boolean *pv_Val
)
{
   locQmiShimCltInfoStructT *pClientInfo = locClnt_FindHandle(locClientHandle);

   if (NULL == pClientInfo)
   {
      LOC_MSG_ERROR(" locAonGetUnableToTrack: Invalid client Handle %d",locClientHandle,0,0);
      return FALSE;
   }

   if (NULL == pv_Val)
   {
      LOC_MSG_ERROR(" locAonGetUnableToTrack: NULL pv_Val", 0, 0, 0);
      return FALSE;
   }
   *pv_Val = pClientInfo->batching_info.unable_to_track_reported;

   LOC_MSG_MED("locAonGetUnableToTrack : client handle = %d, value = %d", locClientHandle, *pv_Val, 0);
   return TRUE;
}



/** locAonBatchStopInternal
* Stop a batching session and cleanup the AON request
* @return : TRUE ; loc AlwaysOn module was initialized correctly
*         : FALSE; loc AlwaysOn module could not be initialized.
*/
static locAonStatusEnumT locAonBatchStopInternal
(
   loc_client_handle_type locClientHandle,
   uint32 locReqId,
   boolean forceStop
)
{
  uint32 aonReqId = 0;
  int32 idx = 0;

  if (locAonGetAonReqId(locClientHandle, locReqId, eLOC_AON_REQ_BATCH, &aonReqId, &idx))
  {
    /* check if req came before the indication for previous start was sent */
    /* check if indication for previous start was sent */
    if ((g_AonClientReqMap[idx].startPending) && (FALSE == forceStop))
    {
      LOC_MSG_ERROR("locAonBatchStopInternal: previous start still pending !!!",0,0,0);
      return eLOC_AON_ERR_START_PENDING;
    }
    else
    {
      /* Valid reqID then delete the request */
      (void)locAonDeleteRequest(aonReqId);
      /* 
       1) If stop comes before replace then no harm
       2) If stop comes between add and add resp handler (for replace or otherwise), the add resp handler will delete
          the geofence just got added if the transaction doesn't exist
      */
      (void)aonDeleteRequest(aonReqId);

      /* If this is the last batching session from the client then reset the unable_to_track flag */
      if (FALSE == locAonCheckIfLocClientPresent(locClientHandle, eLOC_AON_REQ_BATCH))
      {
        (void)locAonSetUnableToTrack(locClientHandle, FALSE);
      }
      return eLOC_AON_SUCCESS;
    }     
  }

  LOC_MSG_ERROR("locAonStopBatchInternal did not find reqId from clientHandle  %d", 
    locClientHandle, 0, 0);
  return eLOC_AON_ERR_AON_REQ_NOT_FOUND;
}

/** locAonProcessLocationCallback
* Process data sent in Location call back invoked by AON service when available
* @return : none
*/
static void locAonProcessLocationCallback
(
   locAonLocationCBData *pz_LocationCBData
)
{
   loc_client_handle_type     l_ClientHandle = LOC_CLIENT_HANDLE_INVALID;
   locAonReqTypeEnumT         e_ReqType;
   aonPositionType            e_PositionType;
   uint32                     q_LocReqId = 0;
   int32                      l_Idx = 0;
   uint32                     q_ReqId;
   locEngPositionReportMsgT_v01 *pz_Location = NULL;

   if (pz_LocationCBData == NULL)
   {
      LOC_MSG_ERROR("locAonProcessLocationCallback: NULL pz_LocationCBData", 0, 0, 0);
      return;
   }

   q_ReqId = pz_LocationCBData->aonReqID;
   e_PositionType = pz_LocationCBData->positionType;
   pz_Location = &pz_LocationCBData->locPosRpt;

   if (FALSE == locAonGetClientReqInfo(q_ReqId, &l_ClientHandle, &q_LocReqId, &e_ReqType, &l_Idx))
   {
      LOC_MSG_ERROR("locAonProcessLocationCallback: Did not find aonReqID = %u", q_ReqId, 0, 0);
   }
   else if (eLOC_AON_REQ_DBT == e_ReqType)
   {
      qmiLocEventDbtPositionReportIndMsgT_v02 z_DbtPosInd = {0};

      if (FALSE == convertLocEngPosToDbtPosInd(pz_Location, e_PositionType, 
                                             q_LocReqId, &z_DbtPosInd))
      {
         LOC_MSG_ERROR("locAonProcessLocationCallback: Could not convert DBT position",0,0,0);
      }

      else if (FALSE == locSendDbtPositionInd(l_ClientHandle, &z_DbtPosInd))
      {
         LOC_MSG_ERROR("locAonProcessLocationCallback: Could not sendDbtPositionInd",0,0,0);
      }
   }
   else if (eLOC_AON_REQ_BATCH == e_ReqType)
   {
      qmiLocBatchedReportStructT_v02 z_BatchPos = {0};

      if (FALSE == pz_Location->gpsTime_valid)
      {
         LOC_MSG_ERROR("locAonProcessLocationCallback: Invalid GPS time",0,0,0);
      }
      else if (FALSE == convertLocEngPosToBatchedPos( pz_Location, &z_BatchPos))
      {
         LOC_MSG_ERROR("locAonProcessLocationCallback: Could not convert Batch position",0,0,0);
      }
      else if (FALSE == loc_batching_batch_position(
                              l_ClientHandle, 
                              &z_BatchPos, 
                              &g_AonClientReqMap[l_Idx].batchCfg))
      {
         LOC_MSG_ERROR("locAonProcessLocationCallback: Could not batch position",0,0,0);
      }
   }
   else
   {
      LOC_MSG_ERROR("locAonProcessLocationCallback: not a AON req", 0, 0, 0);
   }

   return;
}

/** locAonLocationCallback
* Location call back invoked by AON service when available
* @return : none
*/
static void locAonLocationCallback
(
   uint32 reqId, 
   locEngPositionReportMsgT_v01 * p_location,
   aonPositionType positionType
)
{
   locAonLocationCBData *p_LocationCBData = NULL;
   os_IpcMsgType        *p_ipc_msg_ptr = NULL;

   if (NULL == p_location)
   {
      LOC_MSG_ERROR ("LocationCallback: NULL p_location", 0, 0, 0);
      return;
   }

   p_ipc_msg_ptr = os_IpcCreate(sizeof(locAonLocationCBData),
                              IPC_ALLOCATION_DYNAMIC,
                              (uint32)THREAD_ID_LOC_MIDDLEWARE);

   if (NULL == p_ipc_msg_ptr)
   {
      LOC_MSG_ERROR ("LocationCallback:IPC allocation fail reqID %d posType %d",
                     reqId, positionType, 0);
      return;
   }

   p_ipc_msg_ptr->q_MsgId  = LM_MIDDLEWARE_MSG_ID_AON_LOCATION_CB;
   p_ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_LOC_MIDDLEWARE;
   p_ipc_msg_ptr->q_Size           = sizeof(locAonLocationCBData);

   p_LocationCBData = (locAonLocationCBData*) p_ipc_msg_ptr->p_Data; /*lint !e826 */
   p_LocationCBData->aonReqID = reqId;
   p_LocationCBData->positionType = positionType;
   p_LocationCBData->locPosRpt    = *p_location;

   if (os_IpcSend(p_ipc_msg_ptr, (uint32)THREAD_ID_LOC_MIDDLEWARE) == FALSE)
   {
      LOC_MSG_ERROR ("LocationCallback:IPC allocation fail reqID %d posType %d",
                     reqId, positionType, 0);
      // no matter whether os_IpcDelete succeeds or not, there is nothing we can do
      (void) os_IpcDelete(p_ipc_msg_ptr);
   }
}

/** locSendDbtPositionInd
* Sends a DBT position indication to the client
* @return : TRUE ; Successful
*         : FALSE; Failed
*/
static boolean locSendDbtPositionInd
(
   loc_client_handle_type l_ClientHandle,
   qmiLocEventDbtPositionReportIndMsgT_v02 *pz_dbtPosInd
)
{
   locQmiShimIndInfoStructT shimIndInfo = { 0 };

   if (NULL == pz_dbtPosInd)
   {
      LOC_MSG_ERROR("locSendDbtPositionInd NULL pz_dbtPosInd", 
                  l_ClientHandle,0,0);
      return FALSE;
   }

   LOC_MSG_HIGH("locSendDbtPositionInd handle %d reqId %d \n", 
               l_ClientHandle,pz_dbtPosInd->reqId,0);
   shimIndInfo.q_MsgLen = sizeof(*pz_dbtPosInd);

   /** send indication to the SHIM */
   shimIndInfo.p_Msg = (void *)pz_dbtPosInd;
   shimIndInfo.q_Id = QMI_LOC_EVENT_DBT_POSITION_REPORT_IND_V02;

   return locQmiShimSendInd(l_ClientHandle, &shimIndInfo);
}

/** locSendDbtStatusInd
* Sends a DBT status indication to the client
* @return : TRUE ; Successful
*         : FALSE; Failed
*/
static boolean locSendDbtStatusInd
(
   loc_client_handle_type locClienHandle,
   uint32 reqId,
   aonSessionStatusType sessionStatus
)
{
   locQmiShimIndInfoStructT shimIndInfo ={0};
   boolean  v_RetVal = FALSE;

   LOC_MSG_HIGH("locSendDbtStatusInd. sessionStatus %d reqId %d", sessionStatus, reqId, 0);

   switch(sessionStatus)
   {
   case AON_START_SUCCEEDED:
   case AON_START_FAILED:
      {
         qmiLocStartDbtIndMsgT_v02 startDbtInd = {0};
         startDbtInd.reqId_valid = TRUE;
         startDbtInd.reqId = (uint8) reqId;
         startDbtInd.status = (sessionStatus == AON_START_SUCCEEDED)?
                                                eQMI_LOC_SUCCESS_V02:
                                                eQMI_LOC_GENERAL_FAILURE_V02;

         shimIndInfo.q_MsgLen = sizeof(startDbtInd);
         /** send indication to the SHIM */
         shimIndInfo.p_Msg = (void *)&startDbtInd;
         shimIndInfo.q_Id = QMI_LOC_START_DBT_IND_V02;
         v_RetVal = locQmiShimSendInd(locClienHandle ,
                                 &shimIndInfo);
      }
      break;

   case AON_UNABLE_TO_TRACK:
   case AON_ABLE_TO_TRACK:
      {
         qmiLocEventDbtSessionStatusIndMsgT_v02 dbtSessionStatusInd = {0};
         dbtSessionStatusInd.dbtSessionStatus = (sessionStatus == AON_UNABLE_TO_TRACK)?
                                                eQMI_LOC_DBT_UNABLE_TO_TRACK_V02:
                                                eQMI_LOC_DBT_ABLE_TO_TRACK_V02;
         dbtSessionStatusInd.reqId_valid = TRUE;
         dbtSessionStatusInd.reqId = (uint8) reqId;
         shimIndInfo.q_MsgLen = sizeof(dbtSessionStatusInd);
         /** send indication to the SHIM */
         shimIndInfo.p_Msg = (void *)&dbtSessionStatusInd;
         shimIndInfo.q_Id = QMI_LOC_EVENT_DBT_SESSION_STATUS_IND_V02;
         v_RetVal = locQmiShimSendInd(locClienHandle ,
                                 &shimIndInfo);
      }
      break;

   default:
      LOC_MSG_HIGH("locSendDbtStatusInd. unhandled sessionStatus ", 0,0,0);
   }

   return v_RetVal;
}

void locAonSendStartBatchingInd
(
   loc_client_handle_type        l_ClientHandle,
   uint32                        q_LocReqId,
   qmiLocStatusEnumT_v02         e_QmiLocStatus,
   qmiLocBatchingTypeEnumT_v02   e_QmiLocBatchType
)
{
   locQmiShimIndInfoStructT         z_ShimIndInfo = {0};
   qmiLocStartBatchingIndMsgT_v02   z_StartBatchingInd = {0};
   qmiLocStartDbtIndMsgT_v02        z_StartDbtInd = {0};

   LOC_MSG_MED("locAonSendStartBatchingInd: l_ClientHandle %d batchType %d status %d",
                  l_ClientHandle, e_QmiLocBatchType,e_QmiLocStatus);

  
  switch (e_QmiLocBatchType)
  {
    case eQMI_LOC_LOCATION_BATCHING_V02:
    {
      z_StartBatchingInd.status = e_QmiLocStatus;
      if (q_LocReqId != 0)
      {
        z_StartBatchingInd.requestId_valid = TRUE;
        z_StartBatchingInd.requestId = q_LocReqId;
      }

      z_ShimIndInfo.p_Msg = &z_StartBatchingInd;
      z_ShimIndInfo.q_MsgLen = sizeof(z_StartBatchingInd);
      z_ShimIndInfo.q_Id = QMI_LOC_START_BATCHING_IND_V02;
    }
    break;
    
    case QMILOCBATCHINGTYPEENUMT_MAX_ENUM_VAL_V02:
    {
      z_StartDbtInd.status = e_QmiLocStatus;
      if (q_LocReqId != 0)
      {
        z_StartDbtInd.reqId_valid = TRUE;
        z_StartDbtInd.reqId = q_LocReqId;
      }
      z_ShimIndInfo.p_Msg = &z_StartDbtInd;
      z_ShimIndInfo.q_MsgLen = sizeof(z_StartDbtInd);
      z_ShimIndInfo.q_Id = QMI_LOC_START_DBT_IND_V02;
    }
    break;

    default:
      LOC_MSG_ERROR("locAonSendStartBatchingInd: unsupported type",0,0,0);
    break;
  }

   /* send indication */
   (void)locQmiShimSendInd(l_ClientHandle, &z_ShimIndInfo);
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to send batching status event indication to batching clients that
  registered for it
  
@param[in]   locClientHandle       Loc MW client handle 
@param[in]   status                Indication status
                                   
@retval    TRUE           success
@retval    FALSE          failure
*//* ------------------------------------------------------------------------*/
static void locAonSendBatchingStatusInd
(
   loc_client_handle_type        l_ClientHandle,
   qmiLocBatchingStatusEnumT_v02 e_QmiLocBatchingStatus,
   qmiLocBatchingTypeEnumT_v02   e_QmiLocBatchType
)
{
   qmiLocEventBatchingStatusIndMsgT_v02   z_BatchingStatusInd = { 0 };
   locQmiShimIndInfoStructT               z_ShimIndInfo = { 0 };
   locQmiShimCltInfoStructT                *pz_LocClient = locClnt_FindHandle(l_ClientHandle);

   if (NULL == pz_LocClient)
   {
      LOC_MSG_ERROR("locAonSendBatchingStatusInd: Could not find the loc client",0,0,0);
      return;
   }

   /* Send report only if the client registered */
   if (0 == (pz_LocClient->t_EventMask & QMI_LOC_EVENT_MASK_BATCHING_STATUS_V02))
   {
      LOC_MSG_MED("locAonSendBatchingStatusInd: client Handle %d not registered \
                  QMI_LOC_EVENT_MASK_BATCHING_STATUS_V02, suppressing",
                  l_ClientHandle, 0, 0);
      return; 
   }

   /* fill the position report into the indication */
   z_BatchingStatusInd.batchingStatus = e_QmiLocBatchingStatus;

   z_ShimIndInfo.p_Msg = &z_BatchingStatusInd;
   z_ShimIndInfo.q_MsgLen = sizeof(z_BatchingStatusInd);
   z_ShimIndInfo.q_Id = QMI_LOC_EVENT_BATCHING_STATUS_IND_V02;

   LOC_MSG_MED("locAonSendBatchingStatusInd for client Handle %d", l_ClientHandle, 0, 0);

   (void)locQmiShimSendInd(l_ClientHandle, &z_ShimIndInfo);

   return;
}


/** locAonBatchingHandleStatus
* Handle status callback from AON for batching clients
* @return : none
*/
static void locAonBatchingHandleStatus
(
   loc_client_handle_type locClientHandle,
   uint32 locReqId, 
   aonSessionStatusType sessionStatus,
   int32 idx
)
{
   switch(sessionStatus)
   {
      case AON_START_SUCCEEDED: 
      {
        (void)locAonSendStartBatchingInd(locClientHandle, locReqId, eQMI_LOC_SUCCESS_V02, eQMI_LOC_LOCATION_BATCHING_V02);
        g_AonClientReqMap[idx].startPending = FALSE;
        break;
      }
      case AON_START_FAILED:
      {
        //force stop the session
        (void)locAonBatchStopInternal(locClientHandle, locReqId, TRUE);
      
        (void)locAonSendStartBatchingInd(locClientHandle, locReqId, eQMI_LOC_GENERAL_FAILURE_V02, eQMI_LOC_LOCATION_BATCHING_V02);
        g_AonClientReqMap[idx].startPending = FALSE;
        break;
      }
      case AON_UNABLE_TO_TRACK:
      {
        boolean unableToTrackState = FALSE;

        LOC_MSG_MED("locAonBatchingHandleStatus: Got Unable to track,  handle = %d, loc req Id = %d",
          locClientHandle, locReqId, 0);
        
        if (TRUE == locAonGetUnableToTrack(locClientHandle, &unableToTrackState))
        {
          // if Unable to track was previously not reported then report and batch an invalid position
          if (FALSE == unableToTrackState) 
          {
            qmiLocBatchedReportStructT_v02 batchPos = {0};
            (void)loc_batching_batch_position(locClientHandle, &batchPos, &g_AonClientReqMap[idx].batchCfg);
            (void)locAonSendBatchingStatusInd(locClientHandle, 
               eQMI_LOC_BATCH_POS_UNAVAILABLE_V02, eQMI_LOC_LOCATION_BATCHING_V02);
            //set the unable to track for this client
            (void)locAonSetUnableToTrack(locClientHandle, TRUE);
          }
          else
          {
            LOC_MSG_MED("locAonBatchingHandleStatus: Already reported unavailable, suppressing unable to track",0,0,0);
          }
        }
        else
        {
          LOC_MSG_ERROR("locAonBatchingHandleStatus: Could not get the unable to track state",0,0,0);
        }
        break;
      }
      case AON_ABLE_TO_TRACK:
      {
        boolean val = FALSE;

        LOC_MSG_MED("locAonBatchingHandleStatus: Got able to track,  handle = %d, loc req Id = %d",
          locClientHandle, locReqId, 0);
        
        if (TRUE == locAonGetUnableToTrack(locClientHandle, &val))
        {
          // if Unable to track was previously reported then report able to track
          if (TRUE == val) 
          {
            (void)locAonSendBatchingStatusInd(locClientHandle, 
               eQMI_LOC_BATCH_POS_AVAILABLE_V02, eQMI_LOC_LOCATION_BATCHING_V02);

            //clear the unable to track for this client
            (void)locAonSetUnableToTrack(locClientHandle, FALSE);
          }
          else
          {
            LOC_MSG_ERROR("locAonBatchingHandleStatus: got available when no unavailable was sent",0,0,0);
          }
        }
        else
        {
          LOC_MSG_ERROR("locAonBatchingHandleStatus: Could not get the unable to track state",0,0,0);
        }
        break;
      }
      default:
      {
        LOC_MSG_HIGH("Unknown status callback for batching",0,0,0);
        break;
      }
    }
   return;
}

/** locAonReqProcessStatusCallback
* Processes the status updates specific to each request
* @return None
*/
static void locAonReqProcessStatusCallback
(
   locAonStatusCBData *p_statusCBData
)
{
   loc_client_handle_type locClientHandle = LOC_CLIENT_HANDLE_INVALID;
   locAonReqTypeEnumT reqType;
   uint32 locReqId = 0;
   int32 idx = 0;

   if (p_statusCBData == NULL)
   {
      return;
   }

   if (FALSE == locAonGetClientReqInfo(
                     p_statusCBData->aonReqID,
                     &locClientHandle,
                     &locReqId,
                     &reqType,
                     &idx))
   {
      LOC_MSG_ERROR("locAonStatusCallback: Did not find reqID = %u", locReqId, 0, 0);
   }
   else if (eLOC_AON_REQ_DBT == reqType)
   {
      if (FALSE == locSendDbtStatusInd(
                     locClientHandle,
                     locReqId,
                     p_statusCBData->sessionStatus))
      {
         LOC_MSG_ERROR("locSendDbtStatusInd: Could not DbtStatusInd",0,0,0);
      }
   }
   else if (eLOC_AON_REQ_BATCH == reqType)
   {
      /* handle the status callback for batching */
      locAonBatchingHandleStatus(
                     locClientHandle, 
                     locReqId, 
                     p_statusCBData->sessionStatus, 
                     idx);
   }
   else
   {
      LOC_MSG_ERROR("locAonSendStartBatchingInd: not a AON req",0,0,0);
   }
}

/** locAonReqStatusCallback
* Processes the status updates specific to each request
* @return None
*/
static void locAonReqStatusCallback
(
   uint32 reqId,
   aonSessionStatusType sessionStatus
)
{
  locAonStatusCBData *p_statusCBData = NULL;
  os_IpcMsgType      *p_ipc_msg_ptr = NULL;

  p_ipc_msg_ptr = os_IpcCreate(sizeof(locAonStatusCBData), 
                             IPC_ALLOCATION_DYNAMIC,
                             (uint32)THREAD_ID_LOC_MIDDLEWARE);

  if (NULL == p_ipc_msg_ptr)
  {
     LOC_MSG_ERROR ("StatusCallback:IPC allocation fail reqID %d sessStatus %d",
                     reqId, sessionStatus, 0);
     return;
  }
  
  p_ipc_msg_ptr->q_MsgId  = LM_MIDDLEWARE_MSG_ID_AON_STATUS_CB;
  p_ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_LOC_MIDDLEWARE;
  p_ipc_msg_ptr->q_Size           = sizeof(locAonStatusCBData);
  
  p_statusCBData = (locAonStatusCBData*) p_ipc_msg_ptr->p_Data; /*lint !e826 */
  p_statusCBData->aonReqID = reqId;
  p_statusCBData->sessionStatus = sessionStatus;

  if (os_IpcSend(p_ipc_msg_ptr, (uint32)THREAD_ID_LOC_MIDDLEWARE) == FALSE)
  {
     LOC_MSG_ERROR ("StatusCallback:IPC allocation fail reqID %d sessStatus %d",
                     reqId, sessionStatus, 0);
     // no matter whether os_IpcDelete succeeds or not, there is nothing we can do
     (void)os_IpcDelete(p_ipc_msg_ptr);
  }
}

/** locAonDbtStopReqInternal
* Stop a DBT session and cleanup the AON request
* @return : TRUE ; loc AlwaysOn module was initialized correctly
*         : FALSE; loc AlwaysOn module could not be initialized.
*/
boolean locAonDbtStopReqInternal
(
   loc_client_handle_type locClientHandle,
   uint32 locReqId
)
{
   uint32 aonReqId = 0; 
   int32 err = 0, idx = 0;

   if (FALSE == locAonGetAonReqId(locClientHandle, locReqId, 
      eLOC_AON_REQ_DBT, &aonReqId, &idx))
   {
      err = -1;
   }
   else if (FALSE == locAonDeleteRequest(aonReqId))
   {
      err = -2 ;
   }

   /*
         1) If stop comes before replace then no harm
         2) If stop comes between add and add resp handler (for replace or otherwise), the add resp handler will delete
            the geofence just got added if the transaction doesn't exist
      */
   (void)aonDeleteRequest(aonReqId);

   if (0 == err)
   {
      return TRUE;
   }

   LOC_MSG_ERROR("locAonDbtStopReqInternal : err = %d", err, 0,0);
   return FALSE;
}


/** locAonInit
* initialize loc MW AlwaysOn service module and register the callbacks with GM
* module.
* @return : TRUE ; loc AlwaysOn module was initialized correctly
*         : FALSE; loc AlwaysOn module could not be initialized.
*/
boolean locAonInit(void)
{
   boolean retVal = FALSE;
   aonOperationStatusType aonStatus;
   aonGmNvDefaultConfigType z_nvConfig = {0};
   gm_chal_gps_env_backoff_config_s_type z_backoffConfig = {0};
   aon_chal_gps_env_ms_dist_config_s_type z_msDistConfig = {0};

   /* read the default NV's */
   if (FALSE == sm_nv_efs_reg_item_read(
      GNSS_NV_EFS_SM_BATCHING_GF_RADIUS, 
      sizeof(uint32), (void *)&g_AonBatchingGfRadius))
   {
      g_AonBatchingGfRadius = LOC_BATCHING_DEFAULT_RADIUS; //default is 100m
   }

   if (FALSE == sm_nv_efs_reg_item_read(
      GNSS_NV_EFS_SM_BATCHING_CONFIG_MASK, 
      sizeof(uint32), (void *)&g_AonBatchingConfigMask))
   {
      g_AonBatchingConfigMask = 0;
   }

   if (FALSE == sm_nv_efs_reg_item_read(
      GNSS_NV_EFS_SM_AON_GNSS_POS_QOS_SESSION_TIMEOUT, 
      sizeof(uint32), (void *)&(z_nvConfig.q_gnssPositionQosSessionTimeout)))
   {
      LOC_MSG_ERROR("nvread falied for QosSessionTimeout", 0, 0,0);
      z_nvConfig.q_gnssPositionQosSessionTimeout = GNSS_NV_EFS_SM_AON_DEFAULT_GNSS_POS_QOS_SESSION_TIMEOUT; //default is 20 secs
   }

   if (FALSE == sm_nv_efs_reg_item_read(
      GNSS_NV_EFS_SM_AON_CPI_REQUEST_RATE , 
      sizeof(uint32), (void *)&(z_nvConfig.q_cpiReqRate)))
   {
      LOC_MSG_ERROR("nvread falied for cpiReqRate", 0, 0,0);
      z_nvConfig.q_cpiReqRate = GNSS_NV_EFS_SM_AON_DEFAULT_CPI_REQUEST_RATE; //default is 20 secs
   }

   if (FALSE == sm_nv_efs_reg_item_read(
      GNSS_NV_EFS_SM_AON_CHAL_GPS_ENV_BO_CONFIG, 
      sizeof(z_backoffConfig), (void *)&z_backoffConfig))
   {
      LOC_MSG_ERROR("nvread falied for EnvBackoffConfig", 0, 0,0);
      z_backoffConfig.q_backoff_min = GNSS_NV_EFS_SM_AON_DEFAULT_CHAL_GPS_ENV_BO_MIN;
      z_backoffConfig.q_backoff_max = GNSS_NV_EFS_SM_AON_DEFAULT_CHAL_GPS_ENV_BO_MAX;
   }

   z_nvConfig.q_backoffMin = z_backoffConfig.q_backoff_min;
   z_nvConfig.q_backoffMax = z_backoffConfig.q_backoff_max;

   LOC_MSG_HIGH("locAonInit : nvread QosSessionTimeout= %d backOffMin %d backoffMax %d ", 
               z_nvConfig.q_gnssPositionQosSessionTimeout, 
               z_nvConfig.q_backoffMin,
               z_nvConfig.q_backoffMax);

   if (FALSE == sm_nv_efs_reg_item_read(
      GNSS_NV_EFS_SM_AON_CHAL_GPS_ENV_MS_DIST_CONFIG, 
      sizeof(z_msDistConfig), (void *)&z_msDistConfig))
   {
      LOC_MSG_ERROR("nvread falied for msDistConfig", 0, 0,0);
      z_msDistConfig.q_dist_used_for_gps = GNSS_NV_EFS_SM_AON_DEFAULT_CHAL_GPS_ENV_MS_DIST_GPS;
      z_msDistConfig.q_dist_used_for_cpi = GNSS_NV_EFS_SM_AON_DEFAULT_CHAL_GPS_ENV_MS_DIST_CPI;
   }

   z_nvConfig.q_motionSensingDistGps = z_msDistConfig.q_dist_used_for_gps;
   z_nvConfig.q_motionSensingDistCpi = z_msDistConfig.q_dist_used_for_cpi;

   LOC_MSG_HIGH("locAonInit : nvread cpiReqRate= %d motionSensingDistCpi %d motionSensingDistGps %d ", 
               z_nvConfig.q_cpiReqRate, 
               z_nvConfig.q_motionSensingDistCpi,
               z_nvConfig.q_motionSensingDistGps);

   aonStatus = aonClientRegister(locAonLocationCallback, locAonReqStatusCallback, &z_nvConfig);
   retVal = (aonStatus == AON_SUCCEEDED)? TRUE:FALSE;
   return retVal;
}

/** locAonBatchStartReq
* Start batching request, stops the existing request if the same client already
  has batching ongoing. Restarts batching with new params.
* @return None
*/

void locAonBatchStartReq
(
   loc_client_handle_type locClientHandle,
   const qmiLocStartBatchingReqMsgT_v02 *  pStartBatchingReq,
   qmiLocGenRespMsgT_v02 *  pResp
)
{
   void *p_clientCookie=NULL;
   boolean retVal =  FALSE;
   uint32 aonReqId = 0;
   int32 err = 0;
   uint32 minDistance = 0;

   //Copy the NV mask configuration read during boot up
   uint32 batchingConfigMask = g_AonBatchingConfigMask;

   /* set defaults */
   uint32 fix_timeout = LOC_BATCHING_GNSS_QOS_SESSION_TIMEOUT_MS;
   uint32 accuracy = LOC_ACCURACY_LOW;
   aonBatchingMode mode = AON_BATCHING_MODE_TIME_ONLY; 
   uint32 minInterval = LOC_BATCHING_DEFAULT_INTERVAL_MS;
   uint32 reqId = LOC_AON_BATCHING_DEFAULT_REQ_ID;
   locAonBatchingConfig batchCfg = { 0 };
   int32 idx = 0;

   LOC_MSG_HIGH("entered locAonBatchStartReq", 0,0,0);

   if (NULL == pResp)
   {
      LOC_MSG_ERROR("locAonBatchStartReq: NULL resp param", 0,0,0);
      return;
   }

   if (NULL == pStartBatchingReq)
   {
      pResp->resp.error  = QMI_ERR_INVALID_ARG_V01;
      pResp->resp.result = QMI_RESULT_FAILURE_V01;
      LOC_MSG_ERROR("locAonBatchStartReq: NULL startBatching param",0,0,0);
      return;
   }

   if (pStartBatchingReq->requestId_valid)
   {
      if (0 == pStartBatchingReq->requestId)
      {
         pResp->resp.error  = QMI_ERR_INVALID_ARG_V01;
         pResp->resp.result = QMI_RESULT_FAILURE_V01;
         LOC_MSG_ERROR("locAonBatchStartReq: invalid requestId = %d", pStartBatchingReq->requestId,0,0);
      return;
      }
      reqId = pStartBatchingReq->requestId;
   }

   if (pStartBatchingReq->minInterval_valid)
   {
      if (pStartBatchingReq->minInterval < LOC_AON_BATCHING_MINIMUM_MININTERVAL)
      {
         pResp->resp.error  = QMI_ERR_INVALID_ARG_V01;
         pResp->resp.result = QMI_RESULT_FAILURE_V01;
         LOC_MSG_ERROR("locAonBatchStartReq: minInterval (%d) < 1000 msec", pStartBatchingReq->minInterval,0,0);
         return;
    }
    minInterval = pStartBatchingReq->minInterval;
  }

   if (pStartBatchingReq->minDistance_valid)
   {
      minDistance = pStartBatchingReq->minDistance;

      /* If distance specified to 0 then use the NV configured distance */
      if (0 == minDistance)
      {
         minDistance = g_AonBatchingGfRadius;
         mode = AON_BATCHING_MODE_AUTO;
      }
      else if (pStartBatchingReq->minInterval_valid) //time and distance
      {
         mode = AON_BATCHING_MODE_TIME_AND_DISTANCE;
      }
      else //distance only batching
      {
         mode = AON_BATCHING_MODE_DISTANCE_ONLY;
         minInterval = AON_BATCHING_MODE_DISTANCE_ONLY_MIN_INTERVAL_MS;
      }
   }

   if (pStartBatchingReq->batchAllPos_valid)
   {
      /* override the NV config if this TLV is specified */
      if (TRUE == pStartBatchingReq->batchAllPos)
      {
         batchingConfigMask |= AON_BATCHING_CONFIG_MASK_BATCH_ALL_BREACHES;
      }
      else
      {
         batchingConfigMask &= ~AON_BATCHING_CONFIG_MASK_BATCH_ALL_BREACHES;
      }
   }
   /* Create a copy of the config we are sending for batching req */
   batchCfg.batchConfigMask = batchingConfigMask;
   batchCfg.minInterval = minInterval;
   batchCfg.minDistance = minDistance;

   if (pStartBatchingReq->fixSessionTimeout_valid)
   {
      fix_timeout = pStartBatchingReq->fixSessionTimeout;
   }

   if (pStartBatchingReq->horizontalAccuracyLevel_valid)
   {
      if (eQMI_LOC_ACCURACY_LOW_V02 == pStartBatchingReq->horizontalAccuracyLevel)
      {
      accuracy = LOC_ACCURACY_LOW; /* in meters */
      }
      else if (eQMI_LOC_ACCURACY_MED_V02 == pStartBatchingReq->horizontalAccuracyLevel)
      {
      accuracy = LOC_ACCURACY_MED; /* in meters */
      }
      else if (eQMI_LOC_ACCURACY_HIGH_V02 == pStartBatchingReq->horizontalAccuracyLevel)
      {
      accuracy = LOC_ACCURACY_HIGH; /* in meters */
      }
   }

   /* clear existing batching request to support batching "restart";
      If prev start indication wasn't sent then return error */
   if (eLOC_AON_ERR_START_PENDING == locAonBatchStopInternal(locClientHandle, reqId, FALSE))  
   {
      pResp->resp.error  = QMI_ERR_INVALID_OPERATION_V01;
      pResp->resp.result = QMI_RESULT_FAILURE_V01;
      LOC_MSG_ERROR("locAonBatchStartReq: previous start still pending !!!",0,0,0);
      return;
   }

   if (FALSE == locAonAddRequest(locClientHandle, reqId, eLOC_AON_REQ_BATCH, 
      &aonReqId, &batchCfg, &idx))
   {
      err = -1;
   }
   else if (AON_SUCCEEDED != aonCreateBatchingRequest(aonReqId, 
   minInterval, minDistance, batchingConfigMask, fix_timeout, accuracy, mode, 
   locAonLocationCallback, locAonReqStatusCallback))
   {
      (void)locAonDeleteRequest(aonReqId);
      err = -3;
   }

   if (0 == err)
   {
      g_AonClientReqMap[idx].startPending = TRUE;
      pResp->resp.error = QMI_ERR_NONE_V01;
      pResp->resp.result = QMI_RESULT_SUCCESS_V01;
   }
   else
   {
      pResp->resp.error  = QMI_ERR_INTERNAL_V01;
      pResp->resp.result = QMI_RESULT_FAILURE_V01;
      LOC_MSG_ERROR("locAonBatchStartReq: Internal Error ",0,0,0);
   }
   return;
}

/** locAonBatchStopReq
* Stop batching request.
* @return None
*/
void locAonBatchStopReq
(
   loc_client_handle_type locClientHandle,
   const qmiLocStopBatchingReqMsgT_v02 *  pStopBatchingReq,
   qmiLocGenRespMsgT_v02 *  pResp
)
{
   uint32 reqId = LOC_AON_BATCHING_DEFAULT_REQ_ID;
   if (NULL == pResp)
   {
      LOC_MSG_ERROR("locAonBatchStopReq: NULL resp param",0,0,0);
      return;
   }

   if (NULL == pStopBatchingReq)
   {
      pResp->resp.error  = QMI_ERR_INVALID_ARG_V01;
      pResp->resp.result = QMI_RESULT_FAILURE_V01;
      LOC_MSG_ERROR("locAonBatchStopReq: NULL stopBatching param",0,0,0);
      return;
   }

   if (TRUE == pStopBatchingReq->requestId_valid)
   {
      reqId = pStopBatchingReq->requestId;
   }

   if (eLOC_AON_SUCCESS == locAonBatchStopInternal(locClientHandle, reqId, FALSE))
   {
      pResp->resp.result = QMI_RESULT_SUCCESS_V01;
      pResp->resp.error = QMI_ERR_NONE_V01;
   }
   else
   {
      pResp->resp.error  = QMI_ERR_INVALID_OPERATION_V01;
      pResp->resp.result = QMI_RESULT_FAILURE_V01;
      LOC_MSG_ERROR("locAonBatchStopReq: Err Req not found or start pending ",0,0,0);
   }
   return;
}

/** locAonForceStopAll
* Stop all batching requests from the specified client 
* @return : none.
*/
void locAonForceStopAll
(
  loc_client_handle_type locClientHandle
)
{
   int32 i = 0;
   for (i=0; i<LOC_AON_MAX_REQUESTS; i++)
   {
      if ((TRUE == g_AonClientReqMap[i].used) &&
      (locClientHandle == g_AonClientReqMap[i].locClientHandle))
      {
         if (eLOC_AON_REQ_BATCH == g_AonClientReqMap[i].reqType)
         {
            (void)locAonBatchStopInternal(locClientHandle, g_AonClientReqMap[i].locReqId , TRUE);
         }
         else if (eLOC_AON_REQ_DBT == g_AonClientReqMap[i].reqType)
         {
            (void)locAonDbtStopReqInternal(locClientHandle, g_AonClientReqMap[i].locReqId);
         }
         else if (eLOC_AON_REQ_OTB == g_AonClientReqMap[i].reqType)
         {
            (void)locAonOtbStopInternal(locClientHandle, LOC_AON_OTB_DEFAULT_REQ_ID, FALSE);
         }
      }
   }
}
/** locAonDbtStartReq
* Starts DBT request.
* @return None
*/
boolean locAonDbtStartReq
(
   loc_client_handle_type locClientHandle,
   const qmiLocStartDbtReqMsgT_v02 *  pStartDbtReq,
   qmiLocGenRespMsgT_v02 *  pResp
)
{
   aonOperationStatusType *p_errorCode=NULL;
   aonOperationStatusType aonStatus = AON_FAILED;
   void *p_clientCookie=NULL;
   boolean retVal =  FALSE;
   uint32 aonReqId = 0;
   int32 err = 0, idx = 0;

   LOC_MSG_HIGH("entered locAonDbtStartReq LocClntHdl %d ", locClientHandle,0,0);

   if (NULL == pResp)
   {
      LOC_MSG_ERROR("locAonBatchStartReq: NULL resp param", 0,0,0);
      return retVal;
   }

   if (NULL == pStartDbtReq)
   {
      pResp->resp.error  = QMI_ERR_INVALID_ARG_V01;
      pResp->resp.result = QMI_RESULT_FAILURE_V01;
      LOC_MSG_ERROR("locAonBatchStartReq: NULL startBatching param",0,0,0);
      return retVal;
   }

   if (TRUE == locAonCheckIfLocReqIdPresent(locClientHandle, pStartDbtReq->reqId, 
                                          eLOC_AON_REQ_DBT))
   {
      err = -1;
      pResp->resp.error  = QMI_ERR_INVALID_ID_V01;//requestId already present
   }
   else if (FALSE == locAonAddRequest(locClientHandle, pStartDbtReq->reqId, 
      eLOC_AON_REQ_DBT, &aonReqId, NULL /* dont care */, &idx))
   {
      err = -2;
      /*Max number of requests reached*/
      pResp->resp.error  = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
   }
   else if (AON_SUCCEEDED != aonCreateDbtRequest(aonReqId, 
                                                pStartDbtReq->minDistance, 
                                                (aonDbtDistanceType)pStartDbtReq->distanceType,
                                                pStartDbtReq->needOriginLocation,
                                                ( pStartDbtReq->maxLatency_valid? 
                                                  pStartDbtReq->maxLatency:AON_DBT_DEFAULT_LATENCY),
                                                (pStartDbtReq->usageType_valid?
                                                  (aonDbtUsageType) pStartDbtReq->usageType:
                                                  (aonDbtUsageType)AON_DBT_USAGE_NAVIGATION)))
   {
      err = -3;
      pResp->resp.error  = QMI_ERR_INTERNAL_V01;
   }

   if (0 == err)
   {
      pResp->resp.error = QMI_ERR_NONE_V01;
      pResp->resp.result = QMI_RESULT_SUCCESS_V01;
      retVal = TRUE;
   }
   else
   {        
      pResp->resp.result = QMI_RESULT_FAILURE_V01;
      LOC_MSG_ERROR("locAonDbtStartReq: Internal Error %d ",err,0,0);
      retVal = FALSE;
   }

   LOC_MSG_MED("locAonBatchStartReq: pStartDbtReq->reqId %d, aonReqId %d ",
               pStartDbtReq->reqId,aonReqId,0);
   return retVal;

}
/** locAonDbtStopReq
* Stop DBT request.
* @return None
*/
boolean locAonDbtStopReq
(
   loc_client_handle_type locClientHandle,
   const qmiLocStopDbtReqMsgT_v02 *  pStopDbtReq,
   qmiLocGenRespMsgT_v02 *  pResp
)
{
   aonOperationStatusType *p_errorCode=NULL;
   aonOperationStatusType aonStatus = AON_FAILED;
   void *p_clientCookie=NULL;
   boolean retVal =  FALSE;
   LOC_MSG_MED("AON- entered locAonDbtStopReq LocClntHdl %d ", locClientHandle,
                  0,0);
   if (NULL == pResp)
   {
      LOC_MSG_ERROR("locAonDbtStopReq: NULL resp param",0,0,0);
      return retVal;
   }

   if (NULL == pStopDbtReq)
   {
      pResp->resp.error  = QMI_ERR_INVALID_ARG_V01;
      pResp->resp.result = QMI_RESULT_FAILURE_V01;
      LOC_MSG_ERROR("locAonDbtStopReq: NULL stopDbt param",0,0,0);
      return retVal;
   }

   if (FALSE == locAonCheckIfLocReqIdPresent(locClientHandle, pStopDbtReq->reqId, 
                                          eLOC_AON_REQ_DBT))
   {
      pResp->resp.error  = QMI_ERR_INVALID_ID_V01;//requestId already present
      pResp->resp.result = QMI_RESULT_FAILURE_V01;
   } 
   else if (TRUE == locAonDbtStopReqInternal(locClientHandle,
                                       pStopDbtReq->reqId))
   {
      pResp->resp.result = QMI_RESULT_SUCCESS_V01;
      pResp->resp.error = QMI_ERR_NONE_V01;
      retVal = TRUE;
   }
   else
   {
      pResp->resp.error  = QMI_ERR_INTERNAL_V01;
      pResp->resp.result = QMI_RESULT_FAILURE_V01;
      LOC_MSG_ERROR("locAonDbtStopReq: Internal Error ",0,0,0);
   }

   LOC_MSG_MED("AON- exited locAonDbtStopReq. retVal %d", retVal,0,0);
   return retVal;
}

/** locAonIpcHandler
* Forwards the AON IPC's to aon code
* @return None
*/
boolean locAonIpcHandler(os_IpcMsgType* p_msg )
{
   aonOperationStatusType aonStatus;
   boolean retVal =  FALSE;

   //Convert message Id into GM API format here
   uint32 adjustedMsgId = 0;

   if (NULL == p_msg)
   {
      LOC_MSG_ERROR("locAonIpcHandler NULL p_msg", 0, 0, 0);
      return FALSE;
   }

   if (p_msg->q_MsgId >= LM_MIDDLEWARE_MSG_ID_AON_MIN &&
      p_msg->q_MsgId <= LM_MIDDLEWARE_MSG_ID_AON_MAX)
   {
      LOC_MSG_HIGH("locAonIpcHandler invoked MSG = %d",p_msg->q_MsgId,0,0);
      adjustedMsgId = p_msg->q_MsgId - (uint32)LM_MIDDLEWARE_MSG_ID_AON_MIN;
      aonStatus = aonGmEventHandler(adjustedMsgId, p_msg->p_Data);
      retVal = (aonStatus == AON_SUCCEEDED)? TRUE:FALSE;
   }
   else if (p_msg->q_MsgId == LM_MIDDLEWARE_MSG_ID_AON_STATUS_CB)
   {
      locAonReqProcessStatusCallback((locAonStatusCBData *)(p_msg->p_Data));
      retVal = TRUE;
   }
   else if (p_msg->q_MsgId == LM_MIDDLEWARE_MSG_ID_AON_LOCATION_CB)
   {
      locAonProcessLocationCallback((locAonLocationCBData *) p_msg->p_Data);
      retVal = TRUE;
   }
   return retVal;
}

/** locAonTimerHandler
@BRIEF handle the AON timer IPC messages
@return None 
*/
void locAonTimerHandler
(
   uint32 timerId,
   uint32 timerParam
)
{
   LOC_MSG_HIGH("locAonTimerHandler id = %d, param = %d",timerId,timerParam,0);
   aonTimerHandler(timerId);
}

/** locAonOtbStopInternal
* Stop an Outdoor trip batching session and cleanup the AON request
* @return : TRUE ; loc AlwaysOn module was initialized correctly
*         : FALSE; loc AlwaysOn module could not be initialized.
*/
static locAonStatusEnumT locAonOtbStopInternal
(
   loc_client_handle_type l_ClientHandle,
   uint32 q_LocReqId,
   boolean v_ForceStop
)
{
#if 0 /* Feature not present here */
   uint32 q_AonReqId = 0;
   int32  l_Idx = 0;
   loc_client_info_s_type *pz_LocClient = locClnt_FindHandle(l_ClientHandle);

   if (NULL == pz_LocClient)
   {
      LOC_MSG_ERROR(" locAonOtbStopInternal: Invalid client Handle %d", l_ClientHandle, 0, 0);
      return eLOC_AON_ERR_AON_REQ_NOT_FOUND;
   }

   if (locAonGetAonReqId(l_ClientHandle, q_LocReqId, eLOC_AON_REQ_OTB, &q_AonReqId, &l_Idx))
   {
      /* check if req came before the indication for previous start was sent */
      /* check if indication for previous start was sent */
      if ((g_AonClientReqMap[l_Idx].startPending) && (FALSE == v_ForceStop))
      {
         LOC_MSG_ERROR("locAonOtbStopInternal: previous start still pending !!!",0,0,0);
         return eLOC_AON_ERR_START_PENDING;
      }
      else
      {
         /* Valid reqID then delete the request */
         (void)locAonDeleteRequest(q_AonReqId);
         /* 
         1) If stop comes before replace then no harm
         2) If stop comes between add and add resp handler (for replace or otherwise), the add resp handler will delete
            the geofence just got added if the transaction doesn't exist
         */
         (void)aonDeleteRequest(q_AonReqId);
         pz_LocClient->z_OtbInfo.v_Active = FALSE;

         return eLOC_AON_SUCCESS;
      }
   }

   LOC_MSG_ERROR("locAonOtbStopInternal did not find AonReqId for client Handle %d", 
      l_ClientHandle, 0,0);
#endif /* 0 - Feature not present here */
   return eLOC_AON_ERR_AON_REQ_NOT_FOUND;
}