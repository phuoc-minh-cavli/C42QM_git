/******************************************************************************
@file:  loc_client.h
@brief: Location Middleware client

DESCRIPTION
This module defines the loc API routines for a client to initiate a connection

INITIALIZATION AND SEQUENCING REQUIREMENTS

-----------------------------------------------------------------------------
Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
All Rights Reserved. QUALCOMM Proprietary and Confidential. 
Copyright (c) 2013 - 2015 QUALCOMM Atheros Incorporated.
All Rights Reserved.QUALCOMM Atheros Proprietary and Confidential.
Copyright (c) 2015 - 2018 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------------
******************************************************************************/

/*=====================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ------------------------------------------------------- 
06/11/15   sjk      LB 2.0 Integration 
04/27/11   spn      Updated for Loc-Api 2.0

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_client.h#1 $
$DateTime: 2020/07/21 21:23:44 $
$Author: pwbldsvc $
======================================================================*/

#ifndef LOC_CLIENT_H
#define LOC_CLIENT_H

#include "loc_api_2.h"
#include "aries_os_api.h"
#include "queue.h"
#include "qmi_sap.h"
#include "location_service_iot_v02.h"
/******************************************************************************
 * For GET/SET_FIX_CRITERIA
 * The status will be sent back to the client via the callback passed in loc_open
 * with the event set to LOC_EVENT_IOCTL_REPORT.
 * When status indicates that the call is successful, the fix criteria will be
 * available in the event payload.
 *****************************************************************************/
#define LOC_FIX_CRIT_VALID_RECURRENCE_TYPE                  0x00000001

#define LOC_FIX_CRIT_VALID_PREFERRED_ACCURACY               0x00000004
#define LOC_FIX_CRIT_VALID_PREFERRED_RESPONSE_TIME          0x00000008
#define LOC_FIX_CRIT_VALID_INTERMEDIATE_POS_REPORT_ENABLED  0x00000010
#define LOC_FIX_CRIT_VALID_NOTIFY_TYPE                      0x00000020
#define LOC_FIX_CRIT_VALID_MIN_INTERVAL                     0x00000040
#define LOC_FIX_CRIT_VALID_MIN_DISTANCE                     0x00000080
#define LOC_FIX_CRIT_VALID_MIN_DIST_SAMPLE_INTERVAL         0x00000100
#define LOC_FIX_CRIT_ALTITUDE_ASSUMED                       0x00000400

typedef enum
{
  LOC_CLIENT_FIX_PERIODIC  = 1,
  LOC_CLIENT_FIX_SINGLE    = 2,
  LOC_CLIENT_FIX_SESSION_TYPE_MAX = 0x10000000, /* Force enum to be 32-bit */
} locClnt_FixRecurrenceEnumType;

/* Horizontal Accuracy Levels in Meters */
#define LOC_CLIENT_FIX_ACCURACY_HIGH         (50)
#define LOC_CLIENT_FIX_ACCURACY_MED          (500)
#define LOC_CLIENT_FIX_ACCURACY_LOW          (1000)

#define LOC_CLIENT_FIX_INTERVAL_MS_DEFAULT           (1000)       /*   1 second */
#define LOC_CLIENT_FIX_RESPONSE_TIME_MS_DEFAULT      (255*1000)   /* 255 seconds */
#define LOC_CLIENT_MO_RESPONSE_TIME                  (30*1000)    /* 30 seconds */
#define LOC_CLIENT_FIX_MODE_DEFAULT              eQMI_LOC_OPER_MODE_STANDALONE_V02

/* Notification on distance may not be supported on phase 1 */
typedef enum
{
  LOC_CLIENT_NOTIFY_ON_INTERVAL = 1,    /* Notify when min time interval has elapsed, distance do not care */
  LOC_CLIENT_NOTIFY_ON_DISTANCE = 2,    /* Notify when min distance has moved, interval do not care */
  LOC_CLIENT_NOTIFY_ON_ANY      = 3,    /* Notify when either min internal has elapsed or min distance has moved */
  LOC_CLIENT_NOTIFY_ON_ALL      = 4,    /* Notify only when both the min interval has elapsed and min distance has moved */
  LOC_CLIENT_NOTIFY_TYPE_MAX    = 0x10000000, /* Force enum to be 32 bit */
} locClnt_NotifyEnumType;

typedef struct
{
   uint32                             q_ValidMask;
   locClnt_FixRecurrenceEnumType      e_RecurrenceType;          /* Default is periodic fixes */
   uint32                             q_PreferredAccuracy;       /* Preferred Accuracy Threshold (Default=50m) &
                                                                    Response Time (Default=30000ms) are both 
                                                                    used to drive location method selection */
   uint32                             q_PreferredResponseTime;   /* [milli-seconds] */
   boolean                            v_IsIntermPosRptOn;        /* Default is on (not supported in 1.0) */
   locClnt_NotifyEnumType             e_NotifyType;              /* Notify type based on distance is not supported in 1.0 */
   uint32                             q_TbfMs;                   /* TBF = minimum interval [milliseconds] */
   
   uint32                             q_MinDistSampleInterval;   /* in milli-seconds only applies when notifiation based on min_distance is used */
   uint32                             q_MinIntervalIntermediate; /* [milli-seconds] */
   boolean                            v_SharePosition;           /* The client allows to share position with other clients */
   qmiLocConfigIncludeAltitudeAssumedInGnssSvInfoEnumT_v02
                                      e_ConfigAltitudeAssumed;   /* Enable/Disable Altitude Assumed information in GNSS SV Info Event. */
} locClnt_FixCriteriaType;

/* Client state */
typedef enum
{
   CLIENT_STATE_INACTIVE = 0,
   CLIENT_STATE_ACTIVE,    /* Client requests for tracking session */
   CLIENT_STATE_TRANSIENT, /* client requests for single-shot session */
   CLIENT_STATE_TRANSIENT_WAIT_FOR_POSITION, /* Client single shot session is being processed */
} loc_client_state_e_type;

/* control block to store batching related info for a client */
typedef struct
{
   /* to mark if this is a batching client */
   boolean        is_batching_client;

   /* numbering each fix stored */
   uint32         fix_id;

   /* TimeStamp of Last fix reported, this is to avoid batching the same fix
    * from GM in two different batches */
   uint64         last_fix_timestamp_utc;


   /* write_index, read_index to push/pop entries into the batching array */
   int32          write_index;
   int32          read_index;

   /* number of fixes batched during lifetime of batching request */
   uint32         num_fixes_batched_lifetime;

   /* counter to keep track of number of entries filled in buffer */
   int32          total_occupied;

   /* flag to indicate the buffer is full and overwriting */
   /* true == overwrite on state */
   /* false == overwrite off state  */
   boolean        overwrite_flag; 

   /* maximum size of the buffer units */
   int32          max_array_size;

   /* number of fixes in the batch for client */
   int32          client_batch_size;

   /* total number of fixes in the batch = client-size + reserved size */
   int32          total_batch_size;

   /* buffer pointer */
   qmiLocBatchedReportStructT_v02 *loc_batching_array;
        
   /* UTC timestamp of position when the current batch interval begins */
   uint64 last_batched_fix_utc ;

   /* UTC timestamp of the second last position batched */
   uint64 last_last_batched_fix_utc ;

    /* A flag to track if unable to track has been reported for this client, in order to avoid
      sending duplicate indications to the same client for multiple sessions and to avoid bathing the
	  invalid position multiple times */
   boolean unable_to_track_reported;
      
    /* A flag to track if buffer full has been reported. So if we overwrite the 
     * last buffer, we dont send multiple 'batch full' messages. */
   boolean buffer_full_reported;
}loc_batching_client_info_s_type;

/***********************************************
pp_OtbReportArray
 __     _______
|__|-->|       |qmiLocBatchedReportStructT_v02
|__|   |_______|
|__|
|__|
|__|
|__|
|__|
|__|
*************************************************/
/* control block to store outdoor trip batching related info for a client */
typedef struct
{
   /* TimeStamp of Last fix reported, this is to avoid batching the same fix
    * from GM in two different batches */
   uint64                           t_LastFixTimestampUtc;

   /* numbering each fix stored */
   uint32                           q_FixId;

   /* The distance from the last QMI_LOC_START_OTB */
   double                           d_AccumulatedDistance;

   uint32                           q_BatchDistance;

   /*  Minimum Interval Between Position Reports */
   uint32                           q_MinTimeInterval;

   /*  Fix Session Timeout Period */
   uint32                           q_FixSessionTimeout;

   /* buffer pointer */
   qmiLocBatchedReportStructT_v02** pp_OtbReportArray;

   /* size of pp_OtbReportArray */
   uint32                           q_ReportArraySize;

   /* number of allocated qmiLocBatchedReportStructT_v02 */
   uint32                           q_AllocatedReportSize;

   uint32                           q_Head;
   uint32                           q_Tail;

   /* counter to keep track of number of entries filled in buffer */
   uint32                           q_TotalOccupied;

   /* counter to keep track of number of entries from the start */
   uint32                           q_TotalBatchedPosition;

   /* to mark if this is a OTB(Outdoor Trip Batching) client */
   boolean                          v_OtbClient;

   /* START = TRUE, STOP = FALSE */
   boolean                          v_Active;

    /* A flag to track if unable to track has been reported for this client, in order to avoid
      sending duplicate indications to the same client for multiple sessions and to avoid batching the
      invalid position multiple times */
   boolean                          v_UnableToTrackReported;

   boolean                          v_BatchAllPos;

   /* A flag to track if buffer full has been reported.
   So we dont send multiple 'batch full' messages. */
   boolean                          v_BufferFullReported;

   boolean                          v_BatchDistanceReported;

}locClnt_OtbInfoType;



typedef struct
{
   // The location handle that will be returned to client, will be incremented per each call to loc_open */
   int                      l_GlobalClientHandle; 
   int                      l_MaxNumClients;
   int                      l_ActiveClientHandle;
   int                      l_StopClientHandle;
} locClnt_ModuleDataType;

typedef struct
{
  uint32         q_Id; /*< Message ID as allocated by the service definition */
  void*          p_Msg; /*< The message itself */
  uint32         q_MsgLen; /*< Length of the message */
} locIndInfoStructT;


/* Union of QMI_LOC request information saved for PA-specific get/set parameters */
typedef union
{
  /* Store the request to set/get PE Config */
  qmiLocSetPositionEngineConfigParametersReqMsgT_v02 z_SetPeCfg;
  qmiLocGetPositionEngineConfigParametersReqMsgT_v02 z_GetPeCfg;
} locQmi_PaReqUnionT;

/* -----------------------------------------------------------------------*//**
@brief
 Context information for each client is stored in this structure. Elements of
 this type will be stored in a queue, where context information for all 
 clients will be stored. This element also stores the mapping of handles as 
 allocated by the QMI-Loc Service and the Loc-API. An instance of this 
 structure is created whenever a new client connects, and destroyed when a 
 client deregisters. This structure is used as the connection handle that 
 will be given to the QCSI-Framework for it to identify a client to the 
 service provider.
*/ /* ------------------------------------------------------------------------*/
typedef struct
{
  q_link_type                  z_Link; /*< Data Structure required by the queue 
                                           library to add this as part of a 
                                           queue */
  void                        *p_QmiClientHandle; /*< Handle identifying the 
                                            client of QMI-Loc to identify the  
                                            client */
  loc_client_handle_type       l_LocClientHandle; /*< Handle provided by 
                                           loc-api when a new client 
                                           registers with loc-api, as a result 
                                           of a client connecting to QMI-Loc */
  uint64                       t_EventMask; /*< Save the event mask to filter 
                                            the events from loc-api. As 
                                            QMI-Loc has more granularity */
  uint32                       q_CltRev; /*< The Client Revision, this is 
                                             indicated by the client */
  uint8                        u_SessionId; /*< Current session Id specified by the client */
  uint32                       q_FixId;     /*< Running counter of fixes generated in a session */

  qmiLocConfigIncludeAltitudeAssumedInGnssSvInfoEnumT_v02
     e_CfgAltAssumed; /* Configuration to choose if Altitude assumed
                         is needed in SV report for this client */
     boolean                      v_IsTdpClient;    /*< For TDP related clients this feild
                                                     will be set to TRUE */
     locQmi_PaReqUnionT           z_PaReqUnion;     /*< PA-specific get/set parameters */

/*[#9x05][APP] these are the loc_client related parameters which moved in this structure as we are merging
  locqmishimclient and loc_client queues*/
     boolean                      v_InUse;
     loc_event_cb_f_type         *p_EventCb;
     locClnt_FixCriteriaType      z_FixCriteria;
    
     loc_client_state_e_type      e_ClientState; /* state for position request */
  
     boolean                      v_FirstFix;
     uint64                       t_LastFixRequestTime;
     
     /*9x05: doesnt support A-GNSS*/
  
     /* denotes that the client has sent a start for a single shot but 
        the corresponding stop has not been received */
     boolean                      v_SingleShotInProgress; 
  
     /* control block to store batching related info for a client */
     loc_batching_client_info_s_type  batching_info;
  
     /* control block to store outdoor trip batching related info for a client */
     locClnt_OtbInfoType           z_OtbInfo;
  
     /* last reported intermediate fix timestamp */
     uint64                       t_LastIntermediateFixReportTime;
  
     /* last reported FIX (final only) timestamp */
     uint64                       t_LastFixReportTime;
#if 0
     boolean v_PositionListener;
#endif
} locClnt_InfoStructT;
typedef locClnt_InfoStructT locQmiShimCltInfoStructT;

/* -----------------------------------------------------------------------*//**
@brief
 THIS STRUCTURE SHOULD BE INSTANTIATED ONLY ONCE. This is the service handle 
 that will be given to the framework to identify the service. And contains 
 the state information of the shim layer. 
*/ /* ------------------------------------------------------------------------*/
typedef struct
{
  q_type                 z_ClientQ; /**< Information about all the clients that
                               have connected to the service. This is queue 
                               of type locQmiShimCltInfoStructT */
  void *p_ServiceHandle;  /**< Handle that the framework
              provides to represent this service connection */

   /* position listener */ 
  qmi_sap_client_handle  z_QSAPUserHandle; /**< QSAP user handle to register with 
                                QSAP.*/
} locQmiShimInfoStructT;


extern locQmiShimInfoStructT z_LocQmiShim;

 void locClnt_Init
(
  loc_event_cb_f_type     *p_EventCb,
   locQmiShimCltInfoStructT  *pz_client
);

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC request from client that handled by client module

@param[in] l_ClientHandle  : Loc API client handle
@param[in] q_QmiLocMsgId   : QMI_LOC Request Message ID
@param[in] p_QmiLocMsgData : QMI_LOC Request Message Data
@param[in] q_QmiLocMsgLen  : QMI_LOC Request Message Data Length

@retval    TRUE   Called successfully
@retval    FALSE  Called unsuccessfully
---------------------------------------------------------------------------*/
boolean locClnt_ProcessQmiRequest
(
  loc_client_handle_type l_ClientHandle,
  uint32                 q_QmiLocMsgId,
  const void*            p_QmiLocMsgData,
  uint32                 q_QmiLocMsgLen
);

locQmiShimCltInfoStructT * locClnt_FindHandle
(
   loc_client_handle_type  l_ClientHandle
);

// Retunr LOC_API_XXX
int locClnt_StartFix
(
   loc_client_handle_type  l_ClientHandle
);

int locClnt_StopFix
(
   loc_client_handle_type  l_ClientHandle
);

uint32 locClnt_ProcessCmdRequest
(
   const LocMW_LocCmdStructT* pz_Cmd
);

boolean locClnt_InvokeCallback
(
   loc_client_handle_type        l_ClientHandle,
   qmiLocEventRegMaskT_v02         t_EventType,
   const void*                   p_EventPayload
);

boolean locClnt_PositionCallback
(
   loc_client_handle_type     l_ClientHandle,
   qmiLocEventRegMaskT_v02      t_EventType,
   const void*                p_EventPayload
);


// Interaction between client and pd module
// Set up gps configuration for requesting fixes
int     locClnt_SetupRequestConfig(boolean v_opportunistic_schedule);

// Returns TRUE if there is any client that is requesting fixes
boolean locClnt_IsAnyRequestingFix(void);

// Get active client handle
loc_client_handle_type locClnt_GetActiveClient(void);

// Update the state of the client after fix is done
boolean locClnt_UpdateStateAfterFix(void);




// retrives the time for next fix and associated client index
int locClnt_GetMinWaitTime(locQmiShimCltInfoStructT** ppz_MinTimeClient);

#endif // LOC_CLIENT_H
