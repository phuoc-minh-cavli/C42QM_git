/******************************************************************************
  @file:  loc_client.c
  @brief: Location Middleware client

  DESCRIPTION
  This module defines the loc API routines for a client that connects to 
  loc-mw

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2008-2012 by Qualcomm Technologies Incorporated.
  All Rights Reserved.QUALCOMM Proprietary and Confidential.
  Copyright (c) 2013-2015 QUALCOMM Atheros Incorporated.
  All Rights Reserved.QUALCOMM Atheros Proprietary and Confidential.
  Copyright (c) 2015-2023 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ------------------------------------------------------- 
12/1/23    dy       Fix for invalid client handle
09/17/20   sai      Made chanage to report TUNC along with GPS time in system info
01/21/20   ska      Tuning wait time in case of NHz
04/03/20   ska      Added support for System Info 
12/10/19   nath     Reject back to back session requests from the same client.
08/21/19   ska      Added support for NMEA GSV
04/01/19   nath     Added debug NMEA related code.
05/04/17   yh       Debug NMEA
03/31/17   yh       Loc_Conn and Loc_NI clean up
10/28/16   yh       Set t_LastFixReportTime for every QMI_LOC_START call
10/25/16   yh       LocMW was not sending out final position report in Single shot MO location session
08/26/16   yh       Initialize t_LastFixReportTime to the MAX value.
08/17/16   yh       Fix "1HZ client reports position at TBF 1.1s rather than 1s"
06/10/16   yh       Fix QMI_LOC Ind message not sent out bug
04/26/16   yh       Single shot fix criteria bug fix
04/01/16   yh       Schedule after interval expired for > 1HZ clients
03/23/16   yh       Multiple client position report
03/14/16   yh       Fix QMI_LOC Ind message not sent out bug
01/28/16   yh       Broadcast intermediate fix
12/10/15   yh       Remove timer when fix interval is one second
09/03/15   yh       Add loc_client_set_fix_min_interval
06/11/15   sjk      LB 2.0 Integration 
04/27/11   spn      Updated for Loc-Api 2.0

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_client.c#3 $
$DateTime: 2023/02/01 03:43:38 $
$Author: pwbldsvc $
======================================================================*/


/*===================================================================
This module manages location api client.
====================================================================*/

#include "comdef.h"
//#include "customer.h"
#include "target.h"

#include "limits.h"
#include "msg.h"
#include "queue.h"
#include "time_svc.h"

#include "aries_os_api.h"
#include "pdapi_lite.h"
#include "pdsm_atl.h"
#include "loc_api_2.h"

#include "loc_api_internal.h"
#include "loc_client.h"
#include "loc_conn.h"
#include "loc_xtra.h"
#include "loc_sm_interface.h"
#include "loc_ni.h"
#include "loc_wifi.h"
#include "loc_batching.h"
#include "loc_utils.h"
#include "loc_geofence.h"
#include "gm_api.h"
#include "loc_nv_efs.h"
#include "loc_qmi_shim.h"
#include "gts_api.h"


#define LOC_CLIENT_MAX_NUM                   (50)
#define DEFAULT_MODE                         eQMI_LOC_OPER_MODE_STANDALONE_V02
#define MAXIMUM_OVERHEAD_TIME                (400)
#define LOC_CLIENT_POS_OFFSET                (50)
#define LOC_CLIENT_MAX_BACKOFF_IN_SECS        0xFFFF

// Function declaration

extern void tm_util_set_nmea_type(void); 
  
static int locClnt_SetFixCriteria
(
   locQmiShimCltInfoStructT*         pz_ClientInfo,
   const qmiLocStartReqMsgT_v02*   pz_StartReq
);

/* Function to process the event registration mask set request for this location client. */
static int locClnt_SetEventRegMaskLocked
(
  locQmiShimCltInfoStructT*           pz_ClientInfo,
  const qmiLocRegEventsReqMsgT_v02* pz_RegEventsReq
);

static int locClnt_GetFixCriteria
(
  const locQmiShimCltInfoStructT* pz_ClientInfo
);

/* Function to process the operation mode set request for this location client. */

static uint32 locClnt_GetFixInterval
(
   const locClnt_FixCriteriaType* pz_FixCriteria
);
static int locClnt_GetLocationBackoffPeriod
(
  locQmiShimCltInfoStructT*           pz_ClientInfo,
  const qmiLocGetLocationBackoffPeriodReqMsgT_v02* pz_GetBackoffReq
);

static boolean locClnt_GetSystemInfo
(
  locQmiShimCltInfoStructT*           pz_ClientInfo
);


/** @brief Supported TBF interval for 20Hz */
#define LOC_CLIENT_INTERVAL_20HZ_MS (50)
/** @brief Supported TBF interval for 10Hz */
#define LOC_CLIENT_INTERVAL_10HZ_MS (100)
/** @brief Supported TBF interval for 5Hz */
#define LOC_CLIENT_INTERVAL_5HZ_MS (200)
/** @brief Supported TBF interval for 2Hz */
#define LOC_CLIENT_INTERVAL_2HZ_MS (500)
/** @brief Supported TBF interval for 1Hz */
#define LOC_CLIENT_INTERVAL_1HZ_MS (1000)

/*============================================================================
 *                    U T I L I T Y    F U N C T I O N S 
 * =========================================================================*/
/*---------------------------------------------------------------------------
@brief
   This function reads the Supported Minimum Interval millisecond from
   the NV item.

@param[in] None

@return    Time interval in milliseconds set by NV.
---------------------------------------------------------------------------*/
uint32 locClnt_ReadNvMinInterval (void)
{
   uint32  q_SupportedMinInterval = GNSS_NV_EFS_LOC_SUPPORTED_MIN_INTERVAL_DEFAULT; //default value;

   /* read the default NV's */
   if ( FALSE == locNvEfsRead(GNSS_NV_EFS_LOC_SUPPORTED_MIN_INTERVAL, 
                             (void *)&q_SupportedMinInterval,
                              sizeof(q_SupportedMinInterval)))
   {
      q_SupportedMinInterval = GNSS_NV_EFS_LOC_SUPPORTED_MIN_INTERVAL_DEFAULT; //default value
      LOC_MSG_HIGH("locClnt_ReadNvMinInterval could not read nv for SpportedMinInterval, defaulting it to %u ms", 
                     q_SupportedMinInterval, 0, 0 );
   }
   else
   {
      LOC_MSG_HIGH("locClnt_ReadNvMinInterval read nv for SpportedMinInterval Success %u ms", q_SupportedMinInterval, 0, 0 );
   }

   return q_SupportedMinInterval;
}

/*---------------------------------------------------------------------------
@brief
   Select closest supported time interval for TBF

   The method selects appropriate time interval in milliseconds for time between
   fixes. All intervals are rounded up to the next supported value if they are
   lower, than 1000ms.

   Values over 1000ms are rounded up to the next integer second interval.

   When NHz feature is enabled, the following values are allowed:
   - 250 milliseconds - 4Hz rate;
   - 500 milliseconds - 2Hz rate;
   - N*1000 millseconds - 1Hz rate and lower with one second boundaries.

@param[in] q_IntervalMs Time interval in milliseconds to round up.

@return Time interval in milliseconds supported by engine.
---------------------------------------------------------------------------*/
static uint32 locClnt_MatchFixInterval
(
  uint32 q_IntervalMs
)
{
  /**
   * @brief Table of supported intervals.
   *
   * This table has to be monotonic with growing values up to 1000ms. When
   * NHz feature is not enabled, only 1000ms is supported.
   */
   static const uint32 pq_IntervalsMs[] =
   {
#ifdef FEATURE_LOCTECH_NHZ
      LOC_CLIENT_INTERVAL_20HZ_MS,
      LOC_CLIENT_INTERVAL_10HZ_MS,
      LOC_CLIENT_INTERVAL_5HZ_MS,
      LOC_CLIENT_INTERVAL_2HZ_MS,
#endif /* FEATURE_LOCTECH_NHZ */
      LOC_CLIENT_INTERVAL_1HZ_MS
   };

   static boolean v_LocNvInit = FALSE;
   static uint32  q_SupportedMinInterval = LOC_CLIENT_INTERVAL_1HZ_MS; // 1 Sec
   uint32         q_RetVal = q_IntervalMs;

   /* Only read NV supported min interval once */
   if(FALSE == v_LocNvInit)
   {
      if(TRUE == locNvEfsInit())
      {
         q_SupportedMinInterval = locClnt_ReadNvMinInterval();
         v_LocNvInit = TRUE;
      }
      else
      {
         LOC_MSG_ERROR("locClnt_MatchFixInterval: locNvEfsInit Failed!",0,0,0);
      }
   }

   /* Restrict the interval to the MIN supported value */
   if ( q_RetVal < q_SupportedMinInterval )
   {
     q_RetVal = q_SupportedMinInterval;
   }

   if (q_IntervalMs >= LOC_CLIENT_INTERVAL_1HZ_MS)
   {
      if (0 != q_IntervalMs % LOC_CLIENT_INTERVAL_1HZ_MS)
      {
         q_RetVal = (q_IntervalMs / LOC_CLIENT_INTERVAL_1HZ_MS + 1) *
                     LOC_CLIENT_INTERVAL_1HZ_MS;
      }
   }
   else
   {
      uint32 q_Idx = 0;

      for( q_Idx = ARR_SIZE(pq_IntervalsMs); q_Idx ; q_Idx--)
      {
         if( q_RetVal >= pq_IntervalsMs[q_Idx-1] )
         {
            q_RetVal = pq_IntervalsMs[q_Idx-1];
            break;
         }
      }
   }

   LOC_MSG_MED("locClnt_MatchFixInterval: input=%ums output=%ums",
               q_IntervalMs,
               q_RetVal,
               0);

   return q_RetVal;

}

/*---------------------------------------------------------------------------
@brief
   This function compares l_ClientHandle

@param[in] p_ItemPtr    loc_client_info_s_type pointer.
           p_CompareVal l_ClientHandle pointer

@return TRUE if loc_client_info_s_type pointer contains l_ClientHandle value
---------------------------------------------------------------------------*/
static int locClnt_CompareHandle
( 
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
   locQmiShimCltInfoStructT  *pz_Client = (locQmiShimCltInfoStructT *)p_ItemPtr;

   /* item_ptr contains an element of the Client Q i.e. Client Handle, 
     and compare val is also a Client Handle */
   return ( (pz_Client->l_LocClientHandle == *((int *)p_CompareVal) ) ? 1 : 0 );
}

/*---------------------------------------------------------------------------
@brief
   This function is used to check whether a client with the specific handle exists or not.

@param[in] l_ClientHandle  l_ClientHandle value

@return A pointer to the client.
        If it does not exist, NULL will be returned.
---------------------------------------------------------------------------*/
locQmiShimCltInfoStructT* locClnt_FindHandle 
(
   loc_client_handle_type l_ClientHandle
)
{
   locQmiShimCltInfoStructT  *pz_Client = NULL;

   pz_Client = (locQmiShimCltInfoStructT *)q_linear_search(
                                   &z_LocQmiShim.z_ClientQ,
                                   locClnt_CompareHandle,
                                   &l_ClientHandle );

   return pz_Client;
}

/*---------------------------------------------------------------------------
@brief
   This function is used to process loc_open

@param[in] p_EventCb       When loc_client receives client interested event,
                           this is the function to callback to loc_qmi_shim.c

@return A client handle (>=0) when successful.
        -1 if failed
---------------------------------------------------------------------------*/
void locClnt_Init
(
  loc_event_cb_f_type         *p_EventCb ,
  locQmiShimCltInfoStructT    *pz_Client
)
{
   locClnt_ModuleDataType*        p_LocClntData = &(loc_middleware_data.z_locClnt_Data);
   

   if( LOC_CLIENT_MAX_NUM == p_LocClntData->l_MaxNumClients )
   {
      LOC_MSG_ERROR("locClnt_Init l_MaxNumClients = %d",
                    p_LocClntData->l_MaxNumClients, 0, 0);

      pz_Client->l_LocClientHandle = LOC_CLIENT_HANDLE_INVALID; 
      return ;
   }

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.client_crit_sec);

   
   if ( pz_Client != NULL ) 
   {
      p_LocClntData->l_MaxNumClients++;

      pz_Client->v_InUse = TRUE;
      pz_Client->l_LocClientHandle = p_LocClntData->l_GlobalClientHandle++;

      // Client handle wrapped around
      if (pz_Client->l_LocClientHandle < 0)
      {
         p_LocClntData->l_GlobalClientHandle = 0;
         pz_Client->l_LocClientHandle = p_LocClntData->l_GlobalClientHandle++;
      }

      pz_Client->e_ClientState = CLIENT_STATE_INACTIVE;
      pz_Client->t_EventMask = 0;
      pz_Client->p_EventCb = p_EventCb;
      pz_Client->v_FirstFix = TRUE;
      pz_Client->v_SingleShotInProgress = FALSE;

     //set the default valid bits in the fix criteria
      pz_Client->z_FixCriteria.q_ValidMask =
         (LOC_FIX_CRIT_VALID_INTERMEDIATE_POS_REPORT_ENABLED |
          LOC_FIX_CRIT_VALID_MIN_DIST_SAMPLE_INTERVAL |
          LOC_FIX_CRIT_VALID_MIN_DISTANCE |
          LOC_FIX_CRIT_VALID_MIN_INTERVAL |
          LOC_FIX_CRIT_VALID_NOTIFY_TYPE |
          LOC_FIX_CRIT_VALID_PREFERRED_ACCURACY |
          LOC_FIX_CRIT_VALID_PREFERRED_RESPONSE_TIME |
          LOC_FIX_CRIT_VALID_RECURRENCE_TYPE);

      // set the default values for each field in the fix criteria
      pz_Client->z_FixCriteria.e_RecurrenceType                 = LOC_CLIENT_FIX_SINGLE;
      pz_Client->z_FixCriteria.v_IsIntermPosRptOn               = TRUE;

      pz_Client->z_FixCriteria.q_TbfMs                          = LOC_CLIENT_FIX_INTERVAL_MS_DEFAULT;
      pz_Client->z_FixCriteria.q_PreferredAccuracy              = LOC_CLIENT_FIX_ACCURACY_LOW;
      pz_Client->z_FixCriteria.q_PreferredResponseTime          = LOC_CLIENT_FIX_RESPONSE_TIME_MS_DEFAULT;
      pz_Client->z_FixCriteria.e_NotifyType                     = LOC_CLIENT_NOTIFY_ON_INTERVAL;
      pz_Client->z_FixCriteria.q_MinIntervalIntermediate        = 0;



      /* Initialize to use default server. Client will use custom server
       * if eQMI_LOC_SERVER_TYPE_CUSTOM_PDE is being called */
      /*9x05: doesnt support A-GNSS*/

      /* Initialize it to maximum Value, so first final FIX will always be reported */
      pz_Client->t_LastFixReportTime        = ~0;
#if 0
       pz_Client->v_PositionListener = FALSE;
#endif
      LOC_MSG_MED ("locClnt_Init returned l_ClientHandle = %d, pz_Client = 0x%x, l_MaxNumClients = %d",
                    pz_Client->l_LocClientHandle, pz_Client, p_LocClntData->l_MaxNumClients);
   }
   else
   {
      LOC_MSG_ERROR ("locClnt_Init failed to alloate loc_client_info_s_type", 0, 0, 0);
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.client_crit_sec);

   return;
}



/*---------------------------------------------------------------------------
@brief
   This function will go through all the active and "willing to share" clients,
   Loc client decide the best FIX criteria and set the value in
   loc_middleware_data.z_locPd_Data.loc_fix_criteria_rule

@param[in] NULL

@return NULL

---------------------------------------------------------------------------*/
void locClnt_SetCommonPosCriteria(void)
{
   locQmiShimCltInfoStructT            *pz_Client= NULL;
   boolean                            v_FirstClient = TRUE;
   uint32                             q_MinFixInterval = LOC_CLIENT_FIX_INTERVAL_MS_DEFAULT;   /* 1 second */
   uint32                             q_PreferredAccuracy = LOC_CLIENT_FIX_ACCURACY_HIGH;
   uint32                             q_PreferredResponseTime = LOC_CLIENT_FIX_RESPONSE_TIME_MS_DEFAULT;
   locClnt_FixRecurrenceEnumType      e_PreferredRecurrence = LOC_CLIENT_FIX_SINGLE;

   for (pz_Client = (locQmiShimCltInfoStructT *)q_check(&z_LocQmiShim.z_ClientQ);
        pz_Client != NULL;
        pz_Client = (locQmiShimCltInfoStructT *)q_next(&z_LocQmiShim.z_ClientQ, &pz_Client->z_Link) )
   {
      if (TRUE == pz_Client->v_InUse && 
          CLIENT_STATE_INACTIVE != pz_Client->e_ClientState &&
          TRUE == pz_Client->z_FixCriteria.v_SharePosition )
      {

         /* 1. Set q_MinFixInterval to the first client's q_TbfMs
            2. Compare q_MinFixInterval with the next client's q_TbfMs, set to the smaller value */
         if( ( TRUE == v_FirstClient ) ||
             ( pz_Client->z_FixCriteria.q_TbfMs < q_MinFixInterval) )
         {
            q_MinFixInterval = pz_Client->z_FixCriteria.q_TbfMs;
         }

         /* Accuracy and ResponseTime */
         if( ( TRUE == v_FirstClient ) ||
           pz_Client->z_FixCriteria.q_PreferredAccuracy < q_PreferredAccuracy)
         {
           q_PreferredAccuracy = pz_Client->z_FixCriteria.q_PreferredAccuracy;
           q_PreferredResponseTime = pz_Client->z_FixCriteria.q_PreferredResponseTime;
         }

         /* e_RecurrenceType */
         if( ( TRUE == v_FirstClient ) ||
           pz_Client->z_FixCriteria.e_RecurrenceType < e_PreferredRecurrence)
         {
           e_PreferredRecurrence = pz_Client->z_FixCriteria.e_RecurrenceType;
         }

         v_FirstClient = FALSE;
      }
   }

   loc_middleware_data.z_locPd_Data.loc_fix_criteria_rule.q_TbfMs = q_MinFixInterval;
   loc_middleware_data.z_locPd_Data.loc_fix_criteria_rule.q_PreferredAccuracy = q_PreferredAccuracy;
   loc_middleware_data.z_locPd_Data.loc_fix_criteria_rule.q_PreferredResponseTime = q_PreferredResponseTime;
   loc_middleware_data.z_locPd_Data.loc_fix_criteria_rule.e_RecurrenceType = e_PreferredRecurrence;
   LOC_MSG_MED("locClnt_SetCommonPosCriteria,q_TbfMs = %d,q_PreferredAccuracy=%d",
                q_MinFixInterval, q_PreferredAccuracy,0);

   return;
}

/*---------------------------------------------------------------------------
@brief
   This function is called to proces loc_start_fix.

@param[in] client handle

@return LOC_CLIENT_HANDLE_INVALID if the passed-in client handle is not valid
        LOC_API_SUCCESS if success
---------------------------------------------------------------------------*/
int locClnt_StartFix (loc_client_handle_type  l_ClientHandle)
{
   locQmiShimCltInfoStructT* pz_Client = NULL;
   time_type               z_TimeMs = {0};
   uint32                  q_RetVal = LOC_API_SUCCESS;
   boolean                 v_ClientStateChanged = FALSE;

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.client_crit_sec);

   LOC_MSG_MED ("locClnt_StartFix for l_ClientHandle = %d", l_ClientHandle, 0, 0);

   pz_Client = locClnt_FindHandle (l_ClientHandle);
   if (pz_Client == NULL)
   {
      LOC_MSG_ERROR ("locClnt_StartFix, invalid client handle", l_ClientHandle, 0, 0);
      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.client_crit_sec);
      return LOC_API_INVALID_HANDLE;
   }

  if(pz_Client->e_ClientState != CLIENT_STATE_INACTIVE)
   {
   /*If the client has already requested for a fix(single/apptrack) and the fix is in progress, then the client state will not be INACTIVE. 
   This check is to  reject any consecutive fix requests from the same client*/
     q_RetVal = LOC_API_ENGINE_BUSY;
   }
  else
  {
     if (pz_Client->z_FixCriteria.e_RecurrenceType == LOC_CLIENT_FIX_PERIODIC)
     {
       // First fix has not been requested yet when client state is either
       // INACTIVE or TRANSIENT, in this case, set up the necessary information
       if (pz_Client->e_ClientState != CLIENT_STATE_TRANSIENT_WAIT_FOR_POSITION) 
       {
         pz_Client->v_FirstFix = TRUE;
         time_get_uptime_ms (z_TimeMs);
         pz_Client->t_LastFixRequestTime = QW_CVT_Q2N(z_TimeMs);
         pz_Client->t_LastIntermediateFixReportTime = QW_CVT_Q2N(z_TimeMs);
         /* Initialize it to maximum Value, so first final FIX will always be reported */
         pz_Client->t_LastFixReportTime = ~0;
         LOC_MSG_MED("locClnt_StartFix, t_LastIntermediateFixReportTime = %u:%u", 
                      (uint32)(pz_Client->t_LastIntermediateFixReportTime >> 32),
                      (uint32)(pz_Client->t_LastIntermediateFixReportTime & CGPS_MAX_UINT32), 0);
       }

       //clear the single shot started but not stopped flag
       pz_Client->v_SingleShotInProgress = FALSE;
       LOC_MSG_MED("locClnt_StartFix: periodic fix req. clearing v_SingleShotInProgress flag", 
                     0, 0, 0);
       pz_Client->e_ClientState = CLIENT_STATE_ACTIVE;
       v_ClientStateChanged = TRUE;

     }
     // Single shot
     else
     {
        if ((pz_Client->e_ClientState == CLIENT_STATE_INACTIVE) ||
             (pz_Client->e_ClientState == CLIENT_STATE_TRANSIENT_WAIT_FOR_POSITION))
        {
            pz_Client->e_ClientState = CLIENT_STATE_TRANSIENT;
            time_get_uptime_ms(z_TimeMs);
            pz_Client->t_LastFixRequestTime = QW_CVT_Q2N(z_TimeMs);
            /* Initialize it to maximum Value, so first final FIX will always be reported */
            pz_Client->t_LastFixReportTime = ~0;

            //set the single shot started but not stopped flag
            pz_Client->v_SingleShotInProgress = TRUE;
            LOC_MSG_MED("locClnt_StartFix: single shot req. setting v_SingleShotInProgress flag", 
                         0, 0, 0);
            v_ClientStateChanged = TRUE;

        }
        // Position being requested when client is active, ignore the request
        else
        {
           LOC_MSG_MED ("locClnt_StartFix: client is already requesting fix", 0, 0, 0);
        }
     }
  }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.client_crit_sec);

   if (v_ClientStateChanged == TRUE) 
   {
      // Set fix criteria for all clients
      locClnt_SetCommonPosCriteria();
      // run the fix scheduler
      loc_pd_process_fix_request(eLOC_PD_START_REQUEST);
   }

   LOC_MSG_MED ("locClnt_StartFix for client = %d, return value = %d", l_ClientHandle, q_RetVal, 0);

   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
   This function is called to proces loc_stop_fix.

@param[in] client handle

@return TRUE if the client state changed, eg: from requesting position to idle
---------------------------------------------------------------------------*/
int locClnt_StopFix(loc_client_handle_type  l_ClientHandle)
{
   uint32                  q_RetVal = LOC_API_SUCCESS;
   locQmiShimCltInfoStructT* pz_Client = NULL;

   pz_Client = locClnt_FindHandle (l_ClientHandle);

   do
   {
      if (pz_Client == NULL)
      {
         LOC_MSG_ERROR ("locClnt_StopFix, invalid client handle", l_ClientHandle, 0, 0);
         q_RetVal = LOC_API_INVALID_HANDLE;
         break;
      }

      LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.client_crit_sec);

      /* for tracking clients drop the stop if state is inactive
         but for single shot clients allow the stop to go through
         if a stop has not been received. Single shot clients become
         INACTIVE after the position report is generated, but the engine
         will be kept on unless explicitly stopped. */

      if (pz_Client->e_ClientState == CLIENT_STATE_INACTIVE &&
          FALSE == pz_Client->v_SingleShotInProgress )
      {
         LOC_MSG_MED("locClnt_StopFix client %d is already inactive,"
                     " not doing anything for the stop request", 
                     l_ClientHandle,0, 0);

         LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.client_crit_sec);

         break;
      }
      
      LOC_MSG_MED("locClnt_StopFix client = %d, state is %d ss flag is %d  "
                  "scheduling a stop", pz_Client->l_LocClientHandle, 
                  pz_Client->e_ClientState, pz_Client->v_SingleShotInProgress);

      /* When client state becomes inactive, no more position requests will
          be processed on behalf of this client */

      pz_Client->e_ClientState = CLIENT_STATE_INACTIVE;

      //clear the single shot started but not stopped flag
      pz_Client->v_SingleShotInProgress = FALSE;

      LOC_MSG_HIGH("locClnt_StopFix: clearing v_SingleShotInProgress flag", 0, 0, 0);
      /*9x05: doesnt support A-GNSS*/

      loc_middleware_data.z_locClnt_Data.l_StopClientHandle = pz_Client->l_LocClientHandle;
      
      // Recalculate fix criteria for all clients
      locClnt_SetCommonPosCriteria();

      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.client_crit_sec);
         
      //run the fix scheduler
      loc_pd_process_fix_request(eLOC_PD_STOP_REQUEST);
      
   }while (0); 

   LOC_MSG_MED ("locClnt_StopFix for client = %d, return value = %d", l_ClientHandle, q_RetVal, 0);
   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
   This function goes through the queue and determines which client is ready to schedule fix next.
   The client with the minimal value of (last request time + q_FixInterval) will be returned.

@param[in/out] loc client info double pointer, this is the client will be scheduled next

@return The minimum wait time in millisecond among all the clients
---------------------------------------------------------------------------*/
int locClnt_GetMinWaitTime(locQmiShimCltInfoStructT** ppz_MinTimeClient)
{
   locQmiShimCltInfoStructT* pz_Client = NULL;
   time_type               z_TimeMs = {0};
   uint64                  t_CurrentTimeMs;
   uint64                  t_TimeDeltaMs;
   dword                   q_LastFixTtf;
   double                  d_TempWaitTime;

   // initialize variable to over range so error can be easily detected
   double                  d_WaitTime = (double)INT_MAX + 1.0;
   uint32                  q_FixInterval;
   boolean                 v_FirstClientPtr = TRUE;

   time_get_uptime_ms(z_TimeMs);
   t_CurrentTimeMs = QW_CVT_Q2N(z_TimeMs);

   LOC_MSG_MED ("locClnt_GetMinWaitTime t_CurrentTimeMs = %u", t_CurrentTimeMs, 0, 0);
   if(NULL != ppz_MinTimeClient)
   {
   *ppz_MinTimeClient = NULL;
   }
   else
   {
      LOC_MSG_ERROR ("locClnt_GetMinWaitTime NULL ppz_MinTimeClient", 0, 0, 0);
      return 0;
   }
      
   /* CRIT_SECTION taken care in the caller function */
   for (pz_Client = (locQmiShimCltInfoStructT *)q_check(&z_LocQmiShim.z_ClientQ);
        pz_Client != NULL;
        pz_Client = (locQmiShimCltInfoStructT *)q_next(&z_LocQmiShim.z_ClientQ, &pz_Client->z_Link) )
   {
      if (TRUE == pz_Client->v_InUse)
      {
         if (CLIENT_STATE_ACTIVE == pz_Client->e_ClientState ||
             CLIENT_STATE_TRANSIENT == pz_Client->e_ClientState)
         {
            // Calculate wait time based on last fix request time and current time
            t_TimeDeltaMs = t_CurrentTimeMs - pz_Client->t_LastFixRequestTime;

            d_TempWaitTime = 0 - (double) t_TimeDeltaMs;

            if((d_TempWaitTime <= INT_MAX) && (d_TempWaitTime >= INT_MIN))
            {
               LOC_MSG_MED ("locClnt_GetMinWaitTime, client %d d_TempWaitTime %d, t_LastFixRequestTime %u", 
                             pz_Client->l_LocClientHandle, (int32)d_TempWaitTime, pz_Client->t_LastFixRequestTime);
            }
            else
            {
                LOC_MSG_MED ("locClnt_GetMinWaitTime: temp wait time is over range", 0, 0, 0);
            }

            // Adjust for fix interval
            if ((CLIENT_STATE_ACTIVE == pz_Client->e_ClientState) &&
                (FALSE == pz_Client->v_FirstFix))
            {
               q_FixInterval = locClnt_GetFixInterval (&(pz_Client->z_FixCriteria));
                 // to avoid overflow issue
               d_TempWaitTime += q_FixInterval;			   
            }

            if ( (TRUE == v_FirstClientPtr) ||
                 (d_TempWaitTime < d_WaitTime) )

            {
               d_WaitTime    = d_TempWaitTime;
               *ppz_MinTimeClient = pz_Client;
               v_FirstClientPtr = FALSE;
            }
         }
      }
   }

      // there is no active client, wait forever
   if (TRUE == v_FirstClientPtr)
   {
      return d_WaitTime;
   }

   // If there is no expired client, we need to set waitTime to 0
   // when the client TBF < 1s(LOC_CLIENT_FIX_INTERVAL_MS_DEFAULT)
   q_FixInterval = locClnt_GetFixInterval (&((*ppz_MinTimeClient)->z_FixCriteria));
   if( (LOC_CLIENT_FIX_INTERVAL_MS_DEFAULT >= q_FixInterval) &&
       (d_WaitTime > 0) &&
       ((q_FixInterval-d_WaitTime) > LOC_CLIENT_POS_OFFSET))
   {
       LOC_MSG_MED("locClnt_GetMinWaitTime, client %d interval %d less than default, d_WaitTime %d",
                    (*ppz_MinTimeClient)->l_LocClientHandle,q_FixInterval,d_WaitTime);
       d_WaitTime = 0;
   }

   // only apply to TBF = 1s as already we are having the right wait time for NHz case
   if (LOC_CLIENT_FIX_INTERVAL_MS_DEFAULT == q_FixInterval) {
      // Retrieve info regarding how long it takes to process the last fix
      q_LastFixTtf = loc_middleware_data.z_locPd_Data.last_fix_ttf;

      // Shorten the wait interval to avoid the case we missed the fix from the engine,
      if ((q_LastFixTtf != 0) && (q_LastFixTtf < MAXIMUM_OVERHEAD_TIME))
      {
         d_WaitTime -= (MAXIMUM_OVERHEAD_TIME - q_LastFixTtf);
      }
   }

   LOC_MSG_MED("locClnt_GetMinWaitTime, set active client %d adjusted d_WaitTime %d \n", 
               (*ppz_MinTimeClient)->l_LocClientHandle, (int32)d_WaitTime, 0);
   
   return d_WaitTime; 
}

/*---------------------------------------------------------------------------
@brief
   This function goes through the queue and determines which client is ready to schedule fix next.
   And then translate QMI_LOC FIX criteria to pdsm_get_position FIX criteria

@param[in/out] bool v_opportunistic_schedule 

@return The minimum wait time in millisecond among all the clients
---------------------------------------------------------------------------*/
int locClnt_SetupRequestConfig (boolean v_opportunistic_schedule)
{
   locQmiShimCltInfoStructT* pz_Client = NULL;
   time_type               z_TimeMs = {0};
   uint64                  t_CurrentTimeMs;
   locPd_ModuleDataType* pz_LocPdData = &(loc_middleware_data.z_locPd_Data);
   // initialize variable to over range so error can be easily detected
   double                  d_WaitTime = (double)INT_MAX + 1.0;

   time_get_uptime_ms(z_TimeMs);
   t_CurrentTimeMs = QW_CVT_Q2N(z_TimeMs);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.client_crit_sec);



   // After cell id fix requests have been serviced from cached position report
   // every fix request will need to be serviced by calling pdsm_get_position

   d_WaitTime = locClnt_GetMinWaitTime(&pz_Client);

   if(v_opportunistic_schedule)
   {
     /* Changing wait time to 0 , will immediatly schedule session for active client. 
        No need to wait, if MARB indicates that GNSS can be loaded immediately.
     */
     d_WaitTime = 0;
   }
   
   /*If boolean immediate scheduling is set, we don't care about client's wait time till next session and start right away*/
   if((NULL != pz_Client) && (d_WaitTime <= 0))
   {
      // there is one active client whose request need to be serviced, save the
      // configure from that application into the global setting
      loc_middleware_data.z_locClnt_Data.l_ActiveClientHandle = pz_Client->l_LocClientHandle;

      pz_Client->t_LastFixRequestTime = t_CurrentTimeMs;
      pz_Client->v_FirstFix = FALSE;

      // change this client state to CLIENT_STATE_TRANSIENT_WAIT_FOR_POSITION
      if (pz_Client->e_ClientState == CLIENT_STATE_TRANSIENT)
      {
         pz_Client->e_ClientState = CLIENT_STATE_TRANSIENT_WAIT_FOR_POSITION;
      }

      // Translate pd fix config(fix criteria and server information)
      // from loc API to PD API
      locPd_ConvertFixCfg2Pdapi(pz_Client);
   } 

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.client_crit_sec);

   if (NULL == pz_Client) 
   {
      LOC_MSG_HIGH("locClnt_SetupRequestConfig, no client is waiting, d_WaitTime %d \n", 
                     (int32)d_WaitTime, 0, 0);
      return INT32_MAX;
   }
   else if (d_WaitTime >= (INT32_MAX-1))
   {
      LOC_MSG_MED("locClnt_SetupRequestConfig, d_WaitTime %d l_ClientHandle = %d \n", 
                     (int32)d_WaitTime, pz_Client->l_LocClientHandle, 0);
      return (INT32_MAX-1);
   }
   else
   {
      LOC_MSG_MED("locClnt_SetupRequestConfig, d_WaitTime %d l_ClientHandle = %d \n", 
                     (int32)d_WaitTime, pz_Client->l_LocClientHandle, 0);
      return (int32) d_WaitTime;
   }
}

/*---------------------------------------------------------------------------
@brief
   This is a helper function to tell if there is an active client or not

@param[in]  None

@return True if any of the location client is active
---------------------------------------------------------------------------*/
boolean locClnt_IsAnyRequestingFix()
{
   locQmiShimCltInfoStructT* pz_Client = NULL;
   boolean                 v_RetVal = FALSE;

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.client_crit_sec);
   for (pz_Client = (locQmiShimCltInfoStructT *)q_check(&z_LocQmiShim.z_ClientQ);
        pz_Client != NULL;
        pz_Client = (locQmiShimCltInfoStructT *)q_next(&z_LocQmiShim.z_ClientQ, &pz_Client->z_Link) )
   {
      if ((pz_Client->v_InUse == TRUE) &&
          ((pz_Client->e_ClientState == CLIENT_STATE_ACTIVE) ||
           (pz_Client->e_ClientState == CLIENT_STATE_TRANSIENT_WAIT_FOR_POSITION) ||
           (pz_Client->e_ClientState == CLIENT_STATE_TRANSIENT)))
      {
         v_RetVal = TRUE;
         break;
      }
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.client_crit_sec);

   LOC_MSG_MED ("locClnt_IsAnyRequestingFix %d", v_RetVal, 0, 0);

   return v_RetVal;
}

/*---------------------------------------------------------------------------
@brief
   Update the client state after a fix is done. If a client is requesting
   a single-shot fix, after the single shot fix is serviced, the client state
   will be updated to INACTIVE.

@param[in]  None

@return TRUE
---------------------------------------------------------------------------*/
boolean locClnt_UpdateStateAfterFix (void)
{
   locQmiShimCltInfoStructT* pz_Client = NULL;

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.client_crit_sec);
   pz_Client = locClnt_FindHandle (loc_middleware_data.z_locClnt_Data.l_ActiveClientHandle);
   if (pz_Client != NULL)
   {
      if (pz_Client->e_ClientState == CLIENT_STATE_TRANSIENT_WAIT_FOR_POSITION)
      {
         LOC_MSG_MED ("locClnt_UpdateStateAfterFix client = %d requested single shot"
                      "changing state from transient wait for positin to inactive ", 
                      pz_Client->l_LocClientHandle, 0, 0);
         pz_Client->e_ClientState = CLIENT_STATE_INACTIVE;
         /*9x05: doesnt support A-GNSS*/
         loc_middleware_data.z_locClnt_Data.l_ActiveClientHandle = LOC_CLIENT_HANDLE_INVALID;
         locClnt_SetCommonPosCriteria();
      }
      else if (pz_Client->e_ClientState == CLIENT_STATE_INACTIVE)
      {
         /* Set active client handle to INVALID for clients that are inactive */
         LOC_MSG_MED ("locClnt_UpdateStateAfterFix client = %d was in inactive state", 
                      pz_Client->l_LocClientHandle, 0, 0);
         loc_middleware_data.z_locClnt_Data.l_ActiveClientHandle = LOC_CLIENT_HANDLE_INVALID;
      }
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.client_crit_sec);

   return TRUE;
}

/*---------------------------------------------------------------------------
@brief
   Returns the client that is requesting the fix

@param[in]  None

@return Active client handle
---------------------------------------------------------------------------*/
loc_client_handle_type locClnt_GetActiveClient(void)
{
   return loc_middleware_data.z_locClnt_Data.l_ActiveClientHandle;
}

/*---------------------------------------------------------------------------
@brief
   This function is used to process the cmd request from the loc middleware cmd queue.
   For example, if client issues an RPC call to retrieve information, the inforamtion
   can not be sent directly via callbacks, it will have to be put in the location
   middleware cmd queue and from there, the callback can be issued via RPC callback to
   the client.

@param[in]  pz_Cmd

@return LOC_API_SUCCESS if success
        other value if failed
---------------------------------------------------------------------------*/
uint32 locClnt_ProcessCmdRequest(const LocMW_LocCmdStructT* pz_Cmd)
{
   uint32                  q_RetVal = LOC_API_GENERAL_FAILURE;
   LocMW_LocCmdEnumT       e_CmdType;
   loc_client_handle_type  l_ClientHandle;


   if(NULL == pz_Cmd)
   {
      LOC_MSG_ERROR("locClnt_ProcessCmdRequest NULL pz_Cmd", 0, 0, 0);
      return q_RetVal;
   }

   e_CmdType = pz_Cmd->e_CmdType;
   l_ClientHandle = pz_Cmd->l_ClientHandle;

   if(LOC_CMD_TYPE_NOTIFY_CLIENT == e_CmdType)
   {
      if ( TRUE == locClnt_InvokeCallback (l_ClientHandle,
                                 pz_Cmd->t_EventType,
                                 pz_Cmd->p_QmiEventIndMsg))
      {
         q_RetVal = LOC_API_SUCCESS;
      }
   }
   else
   {
      q_RetVal = LOC_API_UNSUPPORTED;
   }

   if(LOC_API_SUCCESS != q_RetVal)
   {
      LOC_MSG_HIGH("locClnt_ProcessCmdRequest event 0X%08x not sent to the client", pz_Cmd->t_EventType, 0, 0);
   }

   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
   This function is used to invoke the callback for the client. If the
   l_ClientHandle is LOC_HANDLE_REGISTERED_CLIENTS, then all clients registered
   for this particular event will get the callback. Otherwise, only the specified
   client handle that regiteres for the callback will get the event.

@param[in]  l_ClientHandle    client handle
            t_EventType       client interested event type
            p_EventPayload    event callback data payload

@return TRUE if success
        FALSE if failed
---------------------------------------------------------------------------*/
boolean locClnt_InvokeCallback
(
   loc_client_handle_type        l_ClientHandle,
   qmiLocEventRegMaskT_v02         t_EventType,
   const void*                   p_EventPayload
)
{
   locQmiShimCltInfoStructT* pz_Client = NULL;
   loc_client_handle_type  l_TempClientHandle = LOC_CLIENT_HANDLE_INVALID;
   boolean                 v_RetVal = FALSE;

   LOC_MSG_MED("locClnt_InvokeCallback, active client = %d, t_EventType = 0X%08x, max clients = %d",
                l_ClientHandle, t_EventType, loc_middleware_data.z_locClnt_Data.l_MaxNumClients);

   if (NULL == p_EventPayload)
   {
      LOC_MSG_ERROR("locClnt_InvokeCallback, NULL pointer", 0, 0, 0);
      return v_RetVal;
   }

   for (pz_Client = (locQmiShimCltInfoStructT *)q_check(&(z_LocQmiShim.z_ClientQ));
        pz_Client != NULL; 
        pz_Client = (locQmiShimCltInfoStructT *)q_next(&(z_LocQmiShim.z_ClientQ), &pz_Client->z_Link))
   {
      /* go through the locClientQueue, find out the client which is registered for the t_EventType */
      if ((TRUE == pz_Client->v_InUse) &&
          (0 != ( pz_Client->t_EventMask & t_EventType)))
      {
         l_TempClientHandle = pz_Client->l_LocClientHandle;

         /* The event should be sent to the current client */
         /* The event should be broadcasted if l_ClientHandle == LOC_HANDLE_REGISTERED_CLIENTS */
         if ((l_TempClientHandle == l_ClientHandle) ||
             (l_ClientHandle == LOC_HANDLE_REGISTERED_CLIENTS))
         {
            boolean v_Report = TRUE;

            if ((QMI_LOC_EVENT_MASK_NMEA_V02 == t_EventType) &&
                (CLIENT_STATE_INACTIVE == pz_Client->e_ClientState))
            {
              v_Report = FALSE;
            }
            if (TRUE == v_Report)
            {

               LOC_MSG_MED ("locClnt_InvokeCallback event_type 0X%08x for client = %d",
                              t_EventType, l_TempClientHandle, 0);
               v_RetVal = pz_Client->p_EventCb(l_TempClientHandle,
                                               t_EventType,
                                               p_EventPayload);
            }
         }
      }
   }

   LOC_MSG_MED ("locClnt_InvokeCallback done", 0,0,0);

   return v_RetVal;
}
boolean locClnt_SVInfoCallback(loc_client_handle_type                 l_ClientHandle,
   qmiLocEventRegMaskT_v02                  t_EventType,
   const void*                            p_EventPayload)
{
   locQmiShimCltInfoStructT* pz_Client = NULL;
   locQmiShimCltInfoStructT *pz_ActiveClient = NULL;
   loc_client_handle_type  l_TempClientHandle = LOC_CLIENT_HANDLE_INVALID;
   LOC_MSG_HIGH("locClnt_SVInfoCallback, active client = %d, event_type = 0X%08x, max clients = %d",
                l_ClientHandle, t_EventType, loc_middleware_data.z_locClnt_Data.l_MaxNumClients);

   if (NULL == p_EventPayload)
   {
      LOC_MSG_ERROR("locClnt_SVInfoCallback, NULL pointer", 0, 0, 0);
      return FALSE;
   }
  
   if( l_ClientHandle < 0)
   {
      LOC_MSG_ERROR("locClnt_SVInfoCallback, Invalid ClientHandle %d", l_ClientHandle, 0, 0);
      return FALSE;
   }
   for (pz_Client = (locQmiShimCltInfoStructT *)q_check(&(z_LocQmiShim.z_ClientQ));
       pz_Client != NULL; 
       pz_Client = (locQmiShimCltInfoStructT *)q_next(&(z_LocQmiShim.z_ClientQ), &pz_Client->z_Link) )
   {
      l_TempClientHandle = pz_Client->l_LocClientHandle;
      if (l_TempClientHandle == l_ClientHandle)
	  {
          LOC_MSG_HIGH ("locClnt_SVInfoCallback event_type 0X%08x for client_handle = %d",
                        t_EventType, l_TempClientHandle, 0);
		  
          pz_Client->p_EventCb(l_TempClientHandle,
                               t_EventType,
                               p_EventPayload);
	  }
   }
   LOC_MSG_HIGH ("locClnt_SVInfoCallback done",0,0,0);
   return TRUE;
}
/*---------------------------------------------------------------------------
@brief
   This function is used to invoke the callback for the below events:
   LOC_EVENT_PARSED_POSITION_REPORT
   1. E911 session, POSITION will be reported to all active clients
   2. Normal session, intermediate POSITION
      Report to all active clients and update intermediate report timestamp
   3. Normal session,
      3.1 Always report to the scheduled client
      3.2 If the scheduled client willing to share, report to interval expired clients.
      Update the last fix report timestamp for the clients whom loc_client sent the final FIX
   LOC_EVENT_SATELLITE_REPORT
   Report to the active clients

@param[in]  l_ClientHandle    client handle
            t_EventType       client interested event type
            pz_EventPayload   event callback data payload

@return TRUE if success
        FALSE if failed
---------------------------------------------------------------------------*/
boolean locClnt_PositionCallback
(
   loc_client_handle_type                 l_ClientHandle,
   qmiLocEventRegMaskT_v02                  t_EventType,
   const void*                            p_EventPayload
)
{
   locQmiShimCltInfoStructT* pz_Client = NULL;
   locQmiShimCltInfoStructT* pz_ActiveClient = NULL;
   loc_client_handle_type  l_TempClientHandle = LOC_CLIENT_HANDLE_INVALID;
   boolean                 v_Report = FALSE;
   uint64                  t_TimeDeltaMs = 0ULL;
   uint64                  t_CurrentTimeMs = 0ULL;
   time_type               z_TimeMs={0};
   
   qmiLocEventPositionReportIndMsgT_v02* pz_PositionReportIndMsg = NULL;

   LOC_MSG_MED("locClnt_PositionCallback, active client = %d, event_type = 0X%08x, max clients = %d",
                l_ClientHandle, t_EventType, loc_middleware_data.z_locClnt_Data.l_MaxNumClients);

   if(NULL == p_EventPayload)
   {
      LOC_MSG_ERROR("locClnt_PositionCallback, NULL pointer", 0, 0, 0);
      return FALSE;
   }
   pz_PositionReportIndMsg = (qmiLocEventPositionReportIndMsgT_v02*)p_EventPayload;

   if( l_ClientHandle < 0)
   {
      LOC_MSG_ERROR("locClnt_PositionCallback, Invalid ClientHandle %d", l_ClientHandle, 0, 0);
      return FALSE;
   }
   pz_ActiveClient = locClnt_FindHandle(l_ClientHandle);
   if (NULL == pz_ActiveClient)
   {
     LOC_MSG_ERROR("locClnt_PositionCallback, can't find active client %d", l_ClientHandle, 0, 0);
     return FALSE;
   }
   time_get_uptime_ms (z_TimeMs);
   t_CurrentTimeMs = QW_CVT_Q2N(z_TimeMs);
   for(pz_Client = (locQmiShimCltInfoStructT *)q_check(&(z_LocQmiShim.z_ClientQ));
       pz_Client != NULL; 
       pz_Client = (locQmiShimCltInfoStructT *)q_next(&(z_LocQmiShim.z_ClientQ), &pz_Client->z_Link) )
   {
      v_Report = FALSE;
      if ( ( TRUE == pz_Client->v_InUse ) &&
           ( 0 != ( pz_Client->t_EventMask & t_EventType)) )
      {
         l_TempClientHandle = pz_Client->l_LocClientHandle;
         if(( QMI_LOC_EVENT_MASK_POSITION_REPORT_V02 == t_EventType ) &&
             ( eQMI_LOC_SESS_STATUS_IN_PROGRESS_V02 ==  pz_PositionReportIndMsg->sessionStatus ) &&
             ( CLIENT_STATE_INACTIVE != pz_Client->e_ClientState ))
         {
            /* Intermediate position report */
            if( FALSE == pz_Client->z_FixCriteria.v_IsIntermPosRptOn)
            {
               LOC_MSG_LOW("locClnt_PositionCallback, supress the intermediate position report for client %d",
                          l_TempClientHandle, 0, 0);
               continue;
            }
            else if( 0 == pz_Client->z_FixCriteria.q_MinIntervalIntermediate)
            {
               LOC_MSG_LOW("locClnt_PositionCallback, q_MinIntervalIntermediate = 0 for client %d",
                        l_TempClientHandle, 0, 0);
               v_Report = TRUE;
            }
            else
            {
               if(t_CurrentTimeMs > pz_Client->t_LastIntermediateFixReportTime)
               {
                  t_TimeDeltaMs = t_CurrentTimeMs - pz_Client->t_LastIntermediateFixReportTime;
               }
               else
               {
                  /* In case of time service reset or error case, report this fix */
                  t_TimeDeltaMs = (uint64)(pz_Client->z_FixCriteria.q_MinIntervalIntermediate);
               }

               LOC_MSG_LOW("locClnt_PositionCallback, intermediate position last_report_time=%u, current_time=%u, interval=%u",
                           pz_Client->t_LastIntermediateFixReportTime, t_CurrentTimeMs, 
                           pz_Client->z_FixCriteria.q_MinIntervalIntermediate);
               if( t_TimeDeltaMs < (uint64)(pz_Client->z_FixCriteria.q_MinIntervalIntermediate - LOC_CLIENT_POS_OFFSET) )
               {
                  LOC_MSG_LOW("locClnt_PositionCallback, drop intermediate position for client %d",
                               l_TempClientHandle, 0, 0);
                  continue;
               }
               else
               {
                  pz_Client->t_LastIntermediateFixReportTime = t_CurrentTimeMs;
                  LOC_MSG_LOW("locClnt_PositionCallback, set pz_Client->t_LastIntermediateFixReportTime %u for client %d",
                               pz_Client->t_LastIntermediateFixReportTime,l_TempClientHandle,0);
                  v_Report = TRUE;
               }
            }
         }/* end of intermediate position report */
         
         
         else
         {

             if (l_TempClientHandle == l_ClientHandle)
             {
                /* Do not check time eclapsed for the scheduled client */
                LOC_MSG_LOW("locClnt_PositionCallback, always report position for the scheduled client",0,0,0);
                v_Report = TRUE;
             }
             else if (( TRUE == pz_ActiveClient->z_FixCriteria.v_SharePosition ) &&
                      ( CLIENT_STATE_INACTIVE != pz_Client->e_ClientState))
             {
               /* Final position report need interval time check */
               if(t_CurrentTimeMs > pz_Client->t_LastFixReportTime)
               {
                  t_TimeDeltaMs = t_CurrentTimeMs - pz_Client->t_LastFixReportTime;
               }
               else
               {
                  /* In case of time service reset or error case, report this fix */
                  t_TimeDeltaMs = (uint64)(pz_Client->z_FixCriteria.q_TbfMs);
               }

               if( t_TimeDeltaMs >= (uint64)(pz_Client->z_FixCriteria.q_TbfMs - LOC_CLIENT_POS_OFFSET) )
               {
                  v_Report = TRUE;
               }
            }

            if((TRUE == v_Report) && 
               (QMI_LOC_EVENT_MASK_POSITION_REPORT_V02 == t_EventType))
            {
               /* update last position report time */
               pz_Client->t_LastFixReportTime = t_CurrentTimeMs;
               LOC_MSG_LOW("locClnt_PositionCallback, set pz_Client->t_LastFixReportTime %u:%u for client %d",
                            (uint32)(pz_Client->t_LastFixReportTime>> 32),
                            (uint32)(pz_Client->t_LastFixReportTime & CGPS_MAX_UINT32),
                            l_TempClientHandle);
            }
         }

         if ( v_Report )
         {
            LOC_MSG_MED ("locClnt_PositionCallback event_type 0X%08x for client_handle = %d",
                          t_EventType, l_TempClientHandle, 0);
            pz_Client->p_EventCb(l_TempClientHandle,
                                 t_EventType,
                                 p_EventPayload);
         }
      } /* pz_Client->v_InUse */
   }/* for loop */

   LOC_MSG_MED ("locClnt_PositionCallback done",0,0,0);
   return TRUE;
}

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC request from client that is Client-specific.

@param[in] l_ClientHandle,  : Loc API client handle
@param[in] q_QmiLocMsgId   : QMI_LOC Request Message ID
@param[in] p_QmiLocMsgData : QMI_LOC Request Message Data
@param[in] q_QmiLocMsgLen  : QMI_LOC Request Message Data Length

@return    TRUE   Called successfully
@return    FALSE  Called unsuccessfully
---------------------------------------------------------------------------*/
boolean locClnt_ProcessQmiRequest
(
   loc_client_handle_type l_ClientHandle,
   uint32                 q_QmiLocMsgId,
   const void*            p_QmiLocMsgData,
   uint32                 q_QmiLocMsgLen
)
{
   locQmiShimCltInfoStructT* pz_ClientInfo = NULL;
   uint32                  q_RetVal     = LOC_API_GENERAL_FAILURE;

   /* Function arguments sanity check */
   if (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle)
   {
      LOC_MSG_ERROR("locClnt_ProcessQmiRequest: invalid clientHandle", 0,0,0);
      return FALSE;
   }

   pz_ClientInfo = locClnt_FindHandle(l_ClientHandle);

   if (NULL == pz_ClientInfo)
   {
      LOC_MSG_ERROR("locClnt_ProcessQmiRequest: Invalid client handle=%d", l_ClientHandle, 0, 0);
      return FALSE;
   }

   switch (q_QmiLocMsgId)
   {
      case QMI_LOC_REG_EVENTS_REQ_V02:
      {
         if(NULL != p_QmiLocMsgData)
         {
            q_RetVal = locClnt_SetEventRegMaskLocked(pz_ClientInfo, (qmiLocRegEventsReqMsgT_v02*)p_QmiLocMsgData);
         }
         else
         {
            LOC_MSG_ERROR("locClnt_ProcessQmiRequest QMI_LOC_REG_EVENTS_REQ NULL p_QmiLocMsgData",0,0,0);
         }
         break;
      }

      case QMI_LOC_START_REQ_V02:
      {
         if(NULL != p_QmiLocMsgData)
         {
            q_RetVal = locClnt_SetFixCriteria(pz_ClientInfo, (qmiLocStartReqMsgT_v02*)p_QmiLocMsgData);
         }
         else
         {
            LOC_MSG_ERROR("locClnt_ProcessQmiRequest QMI_LOC_START_REQ NULL p_QmiLocMsgData",0,0,0);
         }
         break;
      }

      case QMI_LOC_GET_FIX_CRITERIA_REQ_V02:
      {
         q_RetVal = locClnt_GetFixCriteria(pz_ClientInfo);
         break;
      }
      case QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_REQ_V02:
      {
        q_RetVal = locClnt_GetLocationBackoffPeriod(pz_ClientInfo, (qmiLocGetLocationBackoffPeriodReqMsgT_v02*)p_QmiLocMsgData);
        break;
      }
	  case QMI_LOC_SYSTEM_INFO_REQ_V02:
	  {
	     q_RetVal = locClnt_GetSystemInfo(pz_ClientInfo);
		 break;
	  }
      default:
         q_RetVal = LOC_API_UNSUPPORTED;
         break;
   }

   if( LOC_API_SUCCESS == q_RetVal)
   {
      return TRUE;
   }
   else
   {
      LOC_MSG_ERROR("locClnt_ProcessQmiRequest: QMI_LOC 0X%04x returns error=%d", q_QmiLocMsgId, q_RetVal, 0);
      return FALSE;
   }
}

/*---------------------------------------------------------------------------
@brief
   Function to process the event registration mask set request for this location client
   Note: this function should be called when the client data stucture is locked from concurrent access

@param[in] pz_ClientInfo:   Loc client pointer
@param[in] pz_RegEventsReq: QMI_LOC Request Message

@return    LOC_API_SUCCESS   Called successfully
@return    other value       Called unsuccessfully
---------------------------------------------------------------------------*/
static int locClnt_SetEventRegMaskLocked
(
   locQmiShimCltInfoStructT*           pz_ClientInfo,
   const qmiLocRegEventsReqMsgT_v02* pz_RegEventsReq
)
{
   /* Check if any client has ever registered for event LOC_EVENT_LOCATION_SERVER_REQUEST */
   qmiLocEventRegMaskT_v02 t_EventMaskOld       = 0ULL;
   qmiLocEventRegMaskT_v02 t_EventMaskNew       = 0ULL;
   uint8                 u_I                  = 0;
   uint32                q_RetVal             = LOC_API_SUCCESS;

   /* Function arguments sanity check */
   if ((NULL == pz_ClientInfo) || (NULL == pz_RegEventsReq))
   {
      LOC_MSG_ERROR("locClnt_SetEventRegMaskLocked: NULL pointer", 0,0,0);
      return LOC_API_INVALID_PARAMETER;
   }

   t_EventMaskOld = pz_ClientInfo->t_EventMask;

   /* Convert Registered Events mask from QMI_LOC to LocClnt */
   

   LOC_MSG_LOW("locClnt_SetEventRegMaskLocked: t_EventMaskOld = 0X%04x, t_EventMaskNew = 0X%04x ",
                t_EventMaskOld, t_EventMaskNew, 0);
   
   pz_ClientInfo->t_EventMask = t_EventMaskNew;

   /*#9x05: locGeofenceHandleBatchBreachReq not supported*/

   if(pz_RegEventsReq->eventRegMask & QMI_LOC_EVENT_MASK_NMEA_V02)
   {
     LOC_MSG_HIGH("locClnt_SetEventRegMaskLocked: Calling tm_util_set_nmea_type for debug NMEA", 0, 0, 0);
     tm_util_set_nmea_type();
   }

   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
   Function to process the fix criteria set request for this location client

@param[in] pz_ClientInfo:   Loc client pointer
@param[in] pz_StartReq:     QMI_LOC Request Message

@return    LOC_API_SUCCESS   Called successfully
@return    other value       Called unsuccessfully
---------------------------------------------------------------------------*/
static int locClnt_SetFixCriteria
(
  locQmiShimCltInfoStructT*         pz_ClientInfo,
  const qmiLocStartReqMsgT_v02*   pz_StartReq
)
{
   locQmiShimCltInfoStructT*     pz_Client= NULL;
   uint32                      q_RetVal = LOC_API_GENERAL_FAILURE;

   /* Function arguments sanity check */
   if ((NULL == pz_ClientInfo) || (NULL == pz_StartReq))
   {
     LOC_MSG_ERROR("locClnt_SetFixCriteria: NULL pointer", 0,0,0);
     return LOC_API_INVALID_PARAMETER;
   }

   do
   {
     pz_ClientInfo->z_FixCriteria.q_ValidMask |= LOC_FIX_CRIT_VALID_RECURRENCE_TYPE;
     pz_ClientInfo->z_FixCriteria.e_RecurrenceType = LOC_CLIENT_FIX_SINGLE;
     if (pz_StartReq->fixRecurrence_valid)
     {
       switch (pz_StartReq->fixRecurrence)
       {
         case eQMI_LOC_RECURRENCE_PERIODIC_V02:
         {
           pz_ClientInfo->z_FixCriteria.e_RecurrenceType = LOC_CLIENT_FIX_PERIODIC;
           q_RetVal = LOC_API_SUCCESS;
           break;
         }
         case eQMI_LOC_RECURRENCE_SINGLE_V02:
         {
           pz_ClientInfo->z_FixCriteria.e_RecurrenceType = LOC_CLIENT_FIX_SINGLE;
           q_RetVal = LOC_API_SUCCESS;
           break;
         }
         default:
         {
           LOC_MSG_ERROR("locClnt_SetFixCriteria: Invalid RecurrenceType=%d ", pz_StartReq->fixRecurrence, 0, 0);
           q_RetVal = LOC_API_INVALID_PARAMETER;
           break;
         }
        }
      }
      else
      {
        LOC_MSG_MED("locClnt_SetFixCriteria: RecurrenceType not set, use default single", 0, 0, 0);
        q_RetVal = LOC_API_SUCCESS;
      }

      pz_ClientInfo->z_FixCriteria.q_ValidMask |= LOC_FIX_CRIT_VALID_PREFERRED_ACCURACY;
      pz_ClientInfo->z_FixCriteria.q_PreferredAccuracy = LOC_CLIENT_FIX_ACCURACY_LOW;
      if (LOC_API_SUCCESS == q_RetVal && 
         pz_StartReq->horizontalAccuracyLevel_valid)
      {
        switch (pz_StartReq->horizontalAccuracyLevel)
        {
          case eQMI_LOC_ACCURACY_LOW_V02:
          {
            pz_ClientInfo->z_FixCriteria.q_PreferredAccuracy = LOC_CLIENT_FIX_ACCURACY_LOW;
            break;
          }
          case eQMI_LOC_ACCURACY_MED_V02:
          {
            pz_ClientInfo->z_FixCriteria.q_PreferredAccuracy = LOC_CLIENT_FIX_ACCURACY_MED;
            break;
          }
          case eQMI_LOC_ACCURACY_HIGH_V02:
          {
            pz_ClientInfo->z_FixCriteria.q_PreferredAccuracy = LOC_CLIENT_FIX_ACCURACY_HIGH;
            break;
          }
          default:
          {
            LOC_MSG_ERROR("locClnt_SetFixCriteria: Invalid horizontalAccuracyLevel=%d ", pz_StartReq->horizontalAccuracyLevel, 0, 0);
            q_RetVal = LOC_API_INVALID_PARAMETER;
            break;
          }
        }
      }
		 
      /* There are other fields in the fix criteria, but are not exposed through QMI and hence not filled
         (assuming the defaults will be used by LocMW). */

      /* The control point must explicitly disable this if it does not wish to receive intermediate position reports */
      pz_ClientInfo->z_FixCriteria.q_ValidMask |= LOC_FIX_CRIT_VALID_INTERMEDIATE_POS_REPORT_ENABLED;
      pz_ClientInfo->z_FixCriteria.v_IsIntermPosRptOn = TRUE;

      if (LOC_API_SUCCESS == q_RetVal && 
         pz_StartReq->intermediateReportState_valid &&
         eQMI_LOC_INTERMEDIATE_REPORTS_OFF_V02 == pz_StartReq->intermediateReportState)
      {
        pz_ClientInfo->z_FixCriteria.v_IsIntermPosRptOn = FALSE;
      }

      if (LOC_API_SUCCESS == q_RetVal && 
          pz_StartReq->minInterval_valid &&
          eQMI_LOC_RECURRENCE_PERIODIC_V02 == pz_StartReq->fixRecurrence)
      {
        pz_ClientInfo->z_FixCriteria.q_ValidMask |= LOC_FIX_CRIT_VALID_MIN_INTERVAL;
        pz_ClientInfo->z_FixCriteria.q_TbfMs = locClnt_MatchFixInterval(pz_StartReq->minInterval);
      }

      
      /* set default for e_ConfigAltitudeAssumed */
      pz_ClientInfo->z_FixCriteria.e_ConfigAltitudeAssumed = eQMI_LOC_ALTITUDE_ASSUMED_IN_GNSS_SV_INFO_ENABLED_V02;
      if (LOC_API_SUCCESS == q_RetVal &&
         pz_StartReq->configAltitudeAssumed_valid)
      {
        pz_ClientInfo->z_FixCriteria.q_ValidMask |= LOC_FIX_CRIT_ALTITUDE_ASSUMED;
        pz_ClientInfo->z_FixCriteria.e_ConfigAltitudeAssumed = pz_StartReq->configAltitudeAssumed;
      }

      /* Between intermediate position reports, elapsed time must be longer than the interval time */
      pz_ClientInfo->z_FixCriteria.q_MinIntervalIntermediate = 0;
      if (LOC_API_SUCCESS == q_RetVal &&
          pz_StartReq->minIntermediatePositionReportInterval_valid)
      {
        pz_ClientInfo->z_FixCriteria.q_MinIntervalIntermediate = pz_StartReq->minIntermediatePositionReportInterval;
      }

      /* Always reset the session timeout to 255sec, in case the same client converts from batching to tracking client */
      pz_ClientInfo->z_FixCriteria.q_ValidMask |= LOC_FIX_CRIT_VALID_PREFERRED_RESPONSE_TIME;
      pz_ClientInfo->z_FixCriteria.q_PreferredResponseTime = LOC_CLIENT_FIX_RESPONSE_TIME_MS_DEFAULT;
      if( LOC_API_SUCCESS == q_RetVal &&
         pz_StartReq->positionReportTimeout_valid )
      {
        if ( LOC_CLIENT_FIX_INTERVAL_MS_DEFAULT > pz_StartReq->positionReportTimeout ||
              LOC_CLIENT_FIX_RESPONSE_TIME_MS_DEFAULT < pz_StartReq->positionReportTimeout)
        {
          LOC_MSG_ERROR("locClnt_SetFixCriteria invalid positionReportTimeout=%u",
                         pz_StartReq->positionReportTimeout, 0,0);
          q_RetVal = LOC_API_INVALID_PARAMETER;
        }
        else
        {
          pz_ClientInfo->z_FixCriteria.q_PreferredResponseTime = pz_StartReq->positionReportTimeout;
        }
      }

      pz_ClientInfo->z_FixCriteria.v_SharePosition = TRUE;
      if( LOC_API_SUCCESS == q_RetVal &&
         pz_StartReq->sharePosition_valid )
      {
        pz_ClientInfo->z_FixCriteria.v_SharePosition = pz_StartReq->sharePosition;
      }

      /* Send IPC message to LocMW task to handle QMI_LOC indication */
     /*Q: Why would QMI_LOC_START_REQ_V02 would send as indication also this request doesnot have a corresponding indication*/
     /*Ans: QMI_LOC_START_REQ_V02 is not send out as an indication. Instead it is used to call loc_start_fix which starts 
	        the session*/
      if (!locMW_SendQmiIndicationProxy(
                    pz_ClientInfo->l_LocClientHandle,
                    QMI_LOC_START_REQ_V02, 
                    NULL, q_RetVal))
      {
        LOC_MSG_ERROR("locClnt_SetFixCriteria: Could not send IPC to LocMW", 0, 0, 0);
        q_RetVal = LOC_API_GENERAL_FAILURE;
      }

   }while(0);

   LOC_MSG_MED("locClnt_SetFixCriteria: ValidMask=0x%x", pz_ClientInfo->z_FixCriteria.q_ValidMask, 0, 0);
   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
   Function to get the fix criteria for this location client

@param[in] pz_ClientInfo:   Loc client pointer

@return    LOC_API_SUCCESS   Called successfully
@return    other value       Called unsuccessfully
---------------------------------------------------------------------------*/
static int locClnt_GetFixCriteria
(
  const locQmiShimCltInfoStructT* pz_ClientInfo
)
{
   const locClnt_FixCriteriaType*   pz_FixCriteria   = NULL;
   qmiLocGetFixCriteriaIndMsgT_v02* pz_GetFixCritInd = NULL;
   uint32                           q_RetVal = LOC_API_GENERAL_FAILURE;

   /* Function arguments sanity check */
   if ( NULL == pz_ClientInfo )
   {
      LOC_MSG_ERROR("locClnt_GetFixCriteria: NULL pointer", 0,0,0);
      return LOC_API_INVALID_PARAMETER;
   }

   do
   {
      pz_GetFixCritInd = (qmiLocGetFixCriteriaIndMsgT_v02 *)loc_calloc(sizeof(*pz_GetFixCritInd));
      if (NULL == pz_GetFixCritInd)
      {
         LOC_MSG_ERROR("locClnt_GetFixCriteria: calloc return NULL", 0, 0, 0);
         break;
      }
      q_RetVal = LOC_API_SUCCESS;

      pz_FixCriteria   = &pz_ClientInfo->z_FixCriteria;

      /* Convert Fix Criteria structure from LocClnt to QMI_LOC */
      if (pz_FixCriteria->q_ValidMask & LOC_FIX_CRIT_VALID_PREFERRED_ACCURACY)
      {
         pz_GetFixCritInd->horizontalAccuracyLevel_valid = TRUE;
         switch (pz_FixCriteria->q_PreferredAccuracy)
         {
            case LOC_CLIENT_FIX_ACCURACY_HIGH:
            {
               pz_GetFixCritInd->horizontalAccuracyLevel = eQMI_LOC_ACCURACY_HIGH_V02;
               break;
            }
            case LOC_CLIENT_FIX_ACCURACY_MED:
            {
               pz_GetFixCritInd->horizontalAccuracyLevel = eQMI_LOC_ACCURACY_MED_V02;
               break;
            }
            case LOC_CLIENT_FIX_ACCURACY_LOW:
            {
               pz_GetFixCritInd->horizontalAccuracyLevel = eQMI_LOC_ACCURACY_LOW_V02;
               break;
            }
            default:
            {
               LOC_MSG_ERROR("locClnt_GetFixCriteria: Invalid Accuracy Threshold=%d", pz_FixCriteria->q_PreferredAccuracy, 0, 0);
               pz_GetFixCritInd->horizontalAccuracyLevel_valid = FALSE;
               break;
            }
         }
      }

      pz_GetFixCritInd->intermediateReportState_valid = TRUE;
      pz_GetFixCritInd->intermediateReportState = (pz_FixCriteria->q_ValidMask & LOC_FIX_CRIT_VALID_INTERMEDIATE_POS_REPORT_ENABLED) ?
                                                   eQMI_LOC_INTERMEDIATE_REPORTS_ON_V02 :
                                                   eQMI_LOC_INTERMEDIATE_REPORTS_OFF_V02;

      if (pz_FixCriteria->q_ValidMask & LOC_FIX_CRIT_VALID_MIN_INTERVAL)
      {
         pz_GetFixCritInd->minInterval_valid = TRUE;
         pz_GetFixCritInd->minInterval = pz_FixCriteria->q_TbfMs;
      }

      /* Copy Application Id */
    
      if (pz_FixCriteria->q_ValidMask & LOC_FIX_CRIT_ALTITUDE_ASSUMED)
      {
         pz_GetFixCritInd->configAltitudeAssumed_valid = TRUE;
         pz_GetFixCritInd->configAltitudeAssumed = pz_FixCriteria->e_ConfigAltitudeAssumed;
      }

      pz_GetFixCritInd->minIntermediatePositionReportInterval_valid = TRUE;
      pz_GetFixCritInd->minIntermediatePositionReportInterval = pz_FixCriteria->q_MinIntervalIntermediate;

      pz_GetFixCritInd->positionReportTimeout_valid = TRUE;
      pz_GetFixCritInd->positionReportTimeout = pz_FixCriteria->q_PreferredResponseTime;

      pz_GetFixCritInd->sharePosition_valid = TRUE;
      pz_GetFixCritInd->sharePosition = pz_FixCriteria->v_SharePosition;

      /* Send IPC message to LocMW task to handle QMI_LOC indication */
      if (!locMW_SendQmiIndicationProxy(
                    pz_ClientInfo->l_LocClientHandle,
                    QMI_LOC_GET_FIX_CRITERIA_IND_V02, 
                    pz_GetFixCritInd, q_RetVal))
      {
         LOC_MSG_ERROR("locClnt_GetFixCriteria: Could not send IPC to LocMW", 0, 0, 0);
         q_RetVal = LOC_API_GENERAL_FAILURE;
      }
   }while(0);

   return q_RetVal;
}






/*---------------------------------------------------------------------------
@brief
   Return fix interval in milliseconds

@param[in] pz_FixCriteria:   FIX criteria pointer

@return    Fix interval in milliseconds
---------------------------------------------------------------------------*/
uint32 locClnt_GetFixInterval (const locClnt_FixCriteriaType* pz_FixCriteria)
{
   uint32 q_FixInterval = LOC_CLIENT_FIX_INTERVAL_MS_DEFAULT;

   if(NULL == pz_FixCriteria)
   {
      LOC_MSG_ERROR("locClnt_GetFixInterval: NULL pz_FixCriteria", 0, 0, 0);
      return q_FixInterval;
   }

   // Time between fixes
   if ( LOC_CLIENT_NOTIFY_ON_DISTANCE == pz_FixCriteria->e_NotifyType )
   {
      q_FixInterval = pz_FixCriteria->q_MinDistSampleInterval;
   }
   else
   {
      q_FixInterval = pz_FixCriteria->q_TbfMs;
   }

   return q_FixInterval;
}


/*---------------------------------------------------------------------------
@brief
   Function to process the custom server address set request for this location client.
   Custom server will be used when doing MSA/MSB fixes.

@param[in] pz_ClientInfo:   Loc client pointer
@param[in] pz_StartReq:     QMI_LOC Request Message

@return    LOC_API_SUCCESS   Called successfully
@return    other value       Called unsuccessfully
---------------------------------------------------------------------------*/
static int locClnt_GetLocationBackoffPeriod
(
   locQmiShimCltInfoStructT*           pz_ClientInfo,
   const qmiLocGetLocationBackoffPeriodReqMsgT_v02* pz_GetBackoffReq
)
{
  uint32 q_RetVal = LOC_API_GENERAL_FAILURE;
  locQmiShimCltInfoStructT*   pz_NextClient = NULL;
  int32                     time_to_wait_ms = INT32_MAX;
  qmiLocGetLocationBackoffPeriodIndMsgT_v02 *pz_GetBackoffInd = NULL;
  time_type               z_TimeMs={0};
  uint64               t_CurrentTimeMs = 0ULL; 
  uint32 q_GfBackoffMs = LOC_CLIENT_MAX_BACKOFF_IN_SECS * 1000;
  uint32 q_DistToClosestGf = UINT32_MAX;
  uint64               t_DeltaGmReqTimeMs = 0ULL;
  uint32               q_DeltaGmBackoffMs = 0;

  /* Function arguments sanity check */
  if ((NULL == pz_ClientInfo) || (NULL == pz_GetBackoffReq))
  {
     LOC_MSG_ERROR("locClnt_GetLocBf: NULL pointer", 0,0,0);
	 return q_RetVal;
  }
 	

  pz_GetBackoffInd = (qmiLocGetLocationBackoffPeriodIndMsgT_v02 *)loc_calloc(sizeof(*pz_GetBackoffInd));
  if (NULL == pz_GetBackoffInd)
  {
    LOC_MSG_ERROR("locClnt_GetLocBf: calloc return NULL", 0, 0, 0);
    q_RetVal = LOC_API_GENERAL_FAILURE;
    return q_RetVal;
  }
  do
  {
    /* Query the next fix request from loc client  */
    time_to_wait_ms = locClnt_GetMinWaitTime(&pz_NextClient);
    
    if(NULL != pz_NextClient)
    {
      LOC_MSG_HIGH("locClnt_GetLocBf: time to wait %dms", time_to_wait_ms, 0, 0);
    }

    if(NULL == pz_NextClient)
    {
      /* There is no client waiting to be scheduled at all .
          Return a max backoff */
      pz_GetBackoffInd->locationBackoffPeriodMs = LOC_CLIENT_MAX_BACKOFF_IN_SECS * 1000;
    }
    else if (time_to_wait_ms <= 0)
    {
      /* There is a client waiting to be scheduled right away */
      /* Send a backoff of 0 ms as a response */
      pz_GetBackoffInd->locationBackoffPeriodMs = 0;   
    }
    else
    {
      /* There is a client to be scheduled for a fix . */
      /* Set the backoff time to the next next schedule time */
      pz_GetBackoffInd->locationBackoffPeriodMs = time_to_wait_ms;
    }
    pz_GetBackoffInd->locationBackoffPeriodMs_valid = TRUE;
    pz_GetBackoffInd->transactionId_valid = TRUE;
    pz_GetBackoffInd->transactionId = pz_GetBackoffReq->transactionId;
    q_RetVal = LOC_API_SUCCESS;
    pz_GetBackoffInd->distanceToNearestGeofence_valid = FALSE;

    if(FALSE == gm_api_get_backoff_info(&q_GfBackoffMs, &q_DistToClosestGf))
    {
      /* No geofences active . Return the backoffs calculated for loc client above*/
      LOC_MSG_ERROR("locClnt_GetLocBf No geofences active", 0,0,0);
      break;
    }// end FALSE == locGeofenceGetBackoffPeriod
    else
    {
      LOC_MSG_HIGH("locClnt_GetLocBf: GM Backoff:%d, dist to closest GF:%d", q_GfBackoffMs, q_DistToClosestGf, 0);      
      /* Geofences are active */
      pz_GetBackoffInd->distanceToNearestGeofence_valid = TRUE;
      pz_GetBackoffInd->distanceToNearestGeofence = q_DistToClosestGf;
      if(NULL == pz_NextClient)
      {
        /* There is no client waiting to be scheduled at all .Return the GM backoff  */
        pz_GetBackoffInd->locationBackoffPeriodMs = q_GfBackoffMs;
        LOC_MSG_HIGH("locClnt_GetLocBf: GM Backoff only: %d", pz_GetBackoffInd->locationBackoffPeriodMs, 0, 0);
      }
      else if ((NULL != pz_NextClient)&&(time_to_wait_ms <= 0))
      {
        /* There is a client waiting to be scheduled right away */
        /* Send a backoff of 0 ms as a response . 
          Doesnt matter what the GM backoff is since we need an immediate fix */
        pz_GetBackoffInd->locationBackoffPeriodMs = 0;  
        LOC_MSG_HIGH("locClnt_GetLocBf: Zero backoff", 0, 0, 0);
      }
      else
      {
        /* There is a client to be scheduled for a fix and a valid GM backoff time */
        /* Set the backoff time to the minimum of GM backoff and location client backoff */
        if(q_GfBackoffMs > time_to_wait_ms)
        {
          pz_GetBackoffInd->locationBackoffPeriodMs = time_to_wait_ms;
        }
        else
        {
          pz_GetBackoffInd->locationBackoffPeriodMs = q_GfBackoffMs;
        }
        LOC_MSG_HIGH("locClnt_GetLocBf:Min backoff of GF %d ms and LOC %d ms = %d ms", 
                     q_GfBackoffMs, 
                     time_to_wait_ms, 
                     pz_GetBackoffInd->locationBackoffPeriodMs);
      }
      q_RetVal = LOC_API_SUCCESS;
      break;
    }// end else

  }while(0);

  if (!locMW_SendQmiIndicationProxy(pz_ClientInfo->l_LocClientHandle,
                                    QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_IND_V02, 
                                    pz_GetBackoffInd, q_RetVal))
  {
    LOC_MSG_ERROR("locClnt_GetLocationBackoffPeriod: Could not send IPC to LocMW", 0, 0, 0);
    q_RetVal = LOC_API_GENERAL_FAILURE;
  }
  return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
   Function to process the System info request having current GPS time and GPS week.

@param[in] pz_ClientInfo:   Loc client pointer

@return    LOC_API_SUCCESS   Called successfully
@return    other value       Called unsuccessfully
---------------------------------------------------------------------------*/

static boolean locClnt_GetSystemInfo
(
   locQmiShimCltInfoStructT*           pz_ClientInfo
)
{
   boolean q_RetVal = LOC_API_GENERAL_FAILURE;
   qmiLocSystemInfoIndMsgT_v02* pz_LocSystemInfoInd;
   gts_TimeType pz_GtsTime = {0};

   /* Function arguments sanity check */
   if (NULL == pz_ClientInfo)
   {
      LOC_MSG_ERROR("locClnt_GetSystemInfo: NULL pointer", 0,0,0);
	  return q_RetVal;
   }

   pz_LocSystemInfoInd = (qmiLocSystemInfoIndMsgT_v02 *)loc_calloc(sizeof(*pz_LocSystemInfoInd));
   if (NULL == pz_LocSystemInfoInd)
   {
     LOC_MSG_ERROR("locClnt_GetSystemInfo: calloc return NULL", 0, 0, 0);
     return q_RetVal;
   }
   memset(pz_LocSystemInfoInd,0,sizeof(*pz_LocSystemInfoInd));
   pz_LocSystemInfoInd->systemInfo = eQMI_LOC_NEXT_LEAP_SECOND_INFO_V02;
   
   if ((TRUE == gts_GetTime(&pz_GtsTime)) && (TRUE == pz_GtsTime.b_Valid))
   {
	   pz_LocSystemInfoInd->nextLeapSecondInfo_valid = TRUE;

       pz_LocSystemInfoInd->nextLeapSecondInfo.gpsTimeCurrent_valid = TRUE;

       pz_LocSystemInfoInd->nextLeapSecondInfo.gpsTimeCurrent.gpsTimeOfWeekMs = pz_GtsTime.q_GpsMsec;

       pz_LocSystemInfoInd->nextLeapSecondInfo.gpsTimeCurrent.gpsWeek = pz_GtsTime.w_GpsWeek;

       pz_LocSystemInfoInd->nextLeapSecondInfo.gpsTimeNextLsEvent_valid = FALSE;

       pz_LocSystemInfoInd->nextLeapSecondInfo.leapSecondsCurrent_valid = FALSE;

       pz_LocSystemInfoInd->nextLeapSecondInfo.leapSecondsNext_valid = FALSE;

       pz_LocSystemInfoInd->gpsTimeUncMs_valid = TRUE;

      	  pz_LocSystemInfoInd->gpsTimeUncMs = (uint32)pz_GtsTime.f_ClkTimeUncMs;
   	   }

   if (!locMW_SendQmiIndicationProxy(pz_ClientInfo->l_LocClientHandle,
                                    QMI_LOC_SYSTEM_INFO_IND_V02, 
                                    pz_LocSystemInfoInd, q_RetVal))
   {
      LOC_MSG_ERROR("locClnt_GetSystemInfo: Could not send IPC to LocMW", 0, 0, 0);
      q_RetVal = LOC_API_GENERAL_FAILURE;
   }
   else
   {
      q_RetVal = LOC_API_SUCCESS;
   }
										
  return q_RetVal;
   
}
