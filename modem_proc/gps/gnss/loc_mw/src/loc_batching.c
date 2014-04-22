/*============================================================================
 @file loc_batching.c

 loc MW Batching module implementation

 GENERAL DESCRIPTION

 This file contains the loc middleware Batching module implementation.

  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_batching.c#1 $
 $DateTime: 2020/07/21 21:23:44 $
 $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 04/13/16   yh      loc_batching_array  NULL pointer check
 09/22/15   sjk     Fixed a bug in fixId transalation
 06/11/15   sjk     LB 2.0 Integration
 07/25/13   bnk     Initial version

=============================================================================*/

#include "customer.h"
#include "comdef.h"
#include "msg.h"
#include "time_svc.h"
#include "qw.h"

#include "loc_api_2.h"
#include "aon_api.h"
#include "mgp_api.h"
#include "loc_batching.h"
#include "location_service_iot_v02.h"
#include "loc_api_internal.h"
#include "loc_qmi_shim.h"
#include "gnss_calendar_util.h"
#include "loc_client.h"
#include "loc_aon.h"
#include "location_common.h"

/* -----------------------------------------------------------------------*//**
@brief
  Function to free the allocated buffer memory and reset the client's
  outdoor trip batching related fields. 

@param[in]   pz_LocClient            Loc Client Info

@retval    TRUE           Buffer memory release success
@retval    FALSE          Buffer memory release failed
                                  
*//* ------------------------------------------------------------------------*/
extern boolean locOtb_FreeAndReset
(
   locQmiShimCltInfoStructT* pz_LocClient
);


/* -----------------------------------------------------------------------*//**
@brief
  Function to handle read operation from the client.
  
@param[in]   pz_LocClient          Loc Client Info
@param[in]   pz_ReadFromBatchReq   QMI_LOC request message
@param[out]  pz_ReadFromBatchInd   Indication with batched position reports


@retval    TRUE           number of reads field is valid
@retval    FALSE          number of reads field is not valid
                                  
*//* ------------------------------------------------------------------------*/
extern boolean locOtb_BtchingRead
(
   locQmiShimCltInfoStructT*                   pz_LocClient,
   const  qmiLocReadFromBatchReqMsgT_v02*    pz_ReadFromBatchReq,
   qmiLocReadFromBatchIndMsgT_v02*           pz_ReadFromBatchInd
);


/* -----------------------------------------------------------------------*//**
@brief
  Function to allocate the buffer memory based on availability and control
   points request.
  
@param[in]   pz_Client             Loc Client Info
@param[out]  p_GetBatchSizeInd     Get Batch Indication message to control point
@param[in]   q_RequestBatchSize    Batch size requested by control point 
@param[in]   q_ReqTransactionId    Transaction Id in request message by control point 

@retval    TRUE           Buffer memory alloc success
@retval    FALSE          Buffer memory alloc failed
                                  
*//* ------------------------------------------------------------------------*/
extern boolean locOtb_HandleGetBatchSize
(
   locQmiShimCltInfoStructT*                pz_LocClient,
   qmiLocGetBatchSizeIndMsgT_v02*         pz_GetBatchSizeInd,
   uint32                                 q_RequestBatchSize,
   uint32                                 q_ReqTransactionId
);


/* -----------------------------------------------------------------------*//**
@brief
 Untility function to send Response mesasge to Control point 
 
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                  to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the response message
@param[out]  pResp                 Pointer to send the response
@param[in]   resp_value            Response status being sent to control point
@param[in]   e_QmiErr              QMI Error type being sent to control point


@retval    TRUE           Sending Response to control point success
@retval    FALSE          Sending Response to control point fail
*/ /* ------------------------------------------------------------------------*/

extern boolean locBatchingOperationSendResp(locQmiShimCltInfoStructT    *pz_CltInfo,
                                           qmi_req_handle              z_ReqHandle,
                                           locQmiShimMsgInfoStructT    *pz_MsgInfo,
                                           boolean                     resp_value,
                                           qmi_error_type_v01          e_QmiErr);

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to push the position report to LIFO buffer
  
@param[in]   client_ptr            Client Info Handle 
@param[in]   batching_element      Batched position report 


@retval    TRUE           adding position report to LIFO success
@retval    FALSE          adding position report to LIFO fail
*//* ------------------------------------------------------------------------*/
static boolean loc_batching_push_to_lifo(
   locQmiShimCltInfoStructT           *const client_ptr,
   qmiLocBatchedReportStructT_v02   *const batching_element,
   locAonBatchingConfig             *const batchCfg
);


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to retrieve the position report from LIFO buffer
  
@param[in]   pz_LocClient          Loc Client Info
@param[in]   number_of_reads       Number of reports to retrieve
@param[out]  pz_ReadFromBatchInd   QMI_LOC Indication to store the reports

@retval    uint32           number of reports actually retrieved from buffer
*//* ------------------------------------------------------------------------*/
static uint32 loc_batching_pull_from_lifo
(
  locQmiShimCltInfoStructT         *pz_LocClient,
  uint32                          number_of_reads,
  qmiLocReadFromBatchIndMsgT_v02 *pz_ReadFromBatchInd
);

static boolean loc_batching_sufficient_time_elapsed_to_batch_fix
(
   locQmiShimCltInfoStructT           *const client_ptr,
   qmiLocBatchedReportStructT_v02   *const batching_element,
   locAonBatchingConfig             *const batchCfg
)
{
   uint64 curr_time_utc       = 0;
   uint32 modified_min_interval = 0;

   if((client_ptr == NULL) || (batching_element == NULL) || (batchCfg == NULL))
   {
      LOC_MSG_ERROR("Null pointer!", 0, 0, 0);
      return TRUE;
   }
  
   /* round off timestamp to nearest second */
   curr_time_utc = ((batching_element->timestampUtc + 500ULL) / 1000ULL) * 1000ULL;

   
   /* If the time gap between the current fix and the last fix is > minInterval, 
    * keep both fixes */
   if((curr_time_utc - client_ptr->batching_info.last_batched_fix_utc) 
        >= batchCfg->minInterval)
   {
      LOC_MSG_HIGH("Time bet new & last fix: (%d) > min interval == %d. Keep both", 
          (curr_time_utc - client_ptr->batching_info.last_batched_fix_utc), 
          batchCfg->minInterval, 0);
      return TRUE;
   }
   /* Time Gap between current fix and the last fix is < minInterval. */

   /* If the time gap between the current fix and the last_last fix is 
    * >= MIN_INTERVAL_GRACE_FACTOR * minInterval, keep both fixes. If we 
    * overwrite the last fix, our time gap will increase to > minInterval.
    *
    * We will have an opportunity to overwrite the current fix if we get a 
    * new fix soon in < minInterval time */
#define MIN_INTERVAL_GRACE_FACTOR            (1.10f)
#define MIN_INTERVAL_FIX_TIME_GAP_THRESHOLD  (6000)
#define LARGE_BATCHING_INTERVAL              (5*60*1000)
#define SMALL_BATCHING_INTERVAL              (30*1000)
   if(batchCfg->minInterval >= LARGE_BATCHING_INTERVAL)
   {
     modified_min_interval = batchCfg->minInterval + 30*1000;
   }
   else if(batchCfg->minInterval >= SMALL_BATCHING_INTERVAL)
   {
     modified_min_interval = MIN_INTERVAL_GRACE_FACTOR*batchCfg->minInterval;
   }
   else
   {
     modified_min_interval = batchCfg->minInterval + 3*1000;
   }

   if((curr_time_utc - client_ptr->batching_info.last_last_batched_fix_utc) 
        >= modified_min_interval)
   {
     LOC_MSG_HIGH("Time bet new & last_last fix: (%d) > min interval == %d. Keep both",
         (curr_time_utc - client_ptr->batching_info.last_last_batched_fix_utc), 
         batchCfg->minInterval, 0);	    
      return TRUE;
   }

   /* Time Gap between current fix and the last last fix is 
    *      < modified_min_interval, but 
    *      > minInterval
    * AND Time Gap between current fix and the last fix is 
    *      < MIN_INTERVAL_FIX_TIME_GAP_THRESHOLD seconds
    * We can afford to keep just one of the current and the last fixes.  */
   
   if(((curr_time_utc - client_ptr->batching_info.last_last_batched_fix_utc) 
         >= batchCfg->minInterval))
   {
     if((curr_time_utc - client_ptr->batching_info.last_batched_fix_utc) 
         < MIN_INTERVAL_FIX_TIME_GAP_THRESHOLD)
     {
       LOC_MSG_HIGH("Time bet new & last fix: (%d) > min interval == %d. Keep One",
           (curr_time_utc - client_ptr->batching_info.last_batched_fix_utc), 
           batchCfg->minInterval, 0);	     
       return FALSE;
     }
     else
     {
       LOC_MSG_HIGH("Time bet new & last_last fix: (%d) > min interval == %d. Keep both",
           (curr_time_utc - client_ptr->batching_info.last_last_batched_fix_utc), 
           batchCfg->minInterval, 0);
       return TRUE;       
     }
   }

   LOC_MSG_HIGH("UTC NewTime %d LastFix %d LastLastFix %d",
         curr_time_utc, client_ptr->batching_info.last_batched_fix_utc, 
         client_ptr->batching_info.last_last_batched_fix_utc);
   return FALSE;
}

static void loc_batching_select_best_fixes
(
   locQmiShimCltInfoStructT           *const client_ptr,
   qmiLocBatchedReportStructT_v02   *const batching_element,
   locAonBatchingConfig             *const batchCfg,
   boolean  *ignoreNewFix,
   boolean  *overWriteOldFix
)
{
  int32  last_fix_index  = 0;
  qmiLocBatchedReportStructT_v02 *last_position = NULL;
  uint32  distance_bet_positions = 0;
  uint32  hepe_sum = 0;
  uint64 curr_time_utc   = 0;

  if((client_ptr == NULL) || (batching_element == NULL) || (batchCfg == NULL) ||
     (ignoreNewFix == NULL) || (overWriteOldFix == NULL))
  {
     LOC_MSG_ERROR("Null pointer!", 0, 0, 0);
     return;
  }

  if( NULL == client_ptr->batching_info.loc_batching_array )
  {
     LOC_MSG_ERROR("Null loc_batching_array!", 0, 0, 0);
     return;
  }
  /* Default option is to keep both fixes and neither throw away a new fix, 
   * nor overwrite an old fix */
  *ignoreNewFix    = FALSE;
  *overWriteOldFix = FALSE;
  
  /* Always batch the first two fixes generated for a batching req. When we get
   * the third fix, we will decide whether to keep the second or not */
  if(client_ptr->batching_info.num_fixes_batched_lifetime < 2)
  {
    LOC_MSG_HIGH("Batched only %d fixes over lifetime! Keep new fix", 
                 client_ptr->batching_info.num_fixes_batched_lifetime, 0, 0);
    return;
  }
  /* Always batch the first fix received for an empty batch. If we get any
   * subsequent fixes, we will compare the first batched fix with the later
   * fixes to store the 'better' fix, is possible */
  if(client_ptr->batching_info.total_occupied == 0)
  {
    LOC_MSG_HIGH("Keep first fix for this batch!", 0, 0, 0);
    return;
  }
  

  if(loc_batching_sufficient_time_elapsed_to_batch_fix(
                 client_ptr, batching_element, batchCfg) == TRUE)
  {
     LOC_MSG_HIGH("Sufficient Time Elapsed to keep new fix!", 0, 0, 0);
     return;
  }
  
  /**************************************************************************** 
   * There is at least one fix in our batching buffer and at least two fixes  *
   * generated over the lifetime of batching req! Further, this new fix is    *
   * closer to the last fix than the minInterval from user. We can do some    *
   * comparisons to decide which is the better fix to keep.                   *
   * We will use the time of the latest fix, the two fixes prior to it, and   *
   * the 'quality' of the fixes                                               *
   ****************************************************************************/
  
  LOC_MSG_HIGH("Batch all fixes == %d", 
               (batchCfg->batchConfigMask & AON_BATCHING_CONFIG_MASK_BATCH_ALL_BREACHES), 0, 0);

  if((batchCfg->batchConfigMask & AON_BATCHING_CONFIG_MASK_BATCH_ALL_BREACHES) == 0x0)
  {
     /* Time Gap between current fix and the last last fix is < minInterval.
        Assume Fix 1 --------  Fix2 -------- Fix 3.
        We will keep only one of Fix2 and Fix3 as the Time difference between 
        Fix 1 and Fix 3 is < minInterval for batching cfg. We will keep the 
        better fix between Fix2 and Fix3. */ 
      
     last_fix_index  = client_ptr->batching_info.write_index - 1;      
     if(last_fix_index < 0)
     {
       last_fix_index = client_ptr->batching_info.max_array_size - 1;
     }

     last_position = &client_ptr->batching_info.loc_batching_array[last_fix_index];
      
#define GOOD_HEPE_THRESHOLD_FOR_OLD_FIXES (100L)
#define GOOD_HEPE_THRESHOLD_FOR_NEW_FIXES (125L)
#define HEPE_SCALE_FOR_NEW_FIXES         (1/2)

     /* We are biased towards keeping the latest fix. We keep the old fix only 
      *   If the old fix is reasonably accurate 
      *   AND the new fix HEPE is poor               
      *   AND the new fix has a punc that is twice that of the old fix */
     if((last_position->horUncCircular <= GOOD_HEPE_THRESHOLD_FOR_OLD_FIXES)    &&
        (batching_element->horUncCircular >= GOOD_HEPE_THRESHOLD_FOR_NEW_FIXES) &&
        (last_position->horUncCircular <= 
         (HEPE_SCALE_FOR_NEW_FIXES*batching_element->horUncCircular)))
     {
       LOC_MSG_HIGH("Last fix Hepe %d New fix HEPE %d. Drop New",
         last_position->horUncCircular, batching_element->horUncCircular, 0);
       *ignoreNewFix    = TRUE;
       *overWriteOldFix = FALSE;
     }
     else 
     {
       LOC_MSG_HIGH("Last fix Hepe %d New fix HEPE %d. Keep New",
         last_position->horUncCircular, batching_element->horUncCircular, 0);
       *ignoreNewFix    = FALSE;
       *overWriteOldFix = TRUE;
     }
     return;
  }  /* if((batchCfg->batchConfigMask & AON_BATCHING_CONFIG_MASK_BATCH_ALL_BREACHES) == 0x0) */
  else
  {
    /* round off timestamp to nearest second */
    curr_time_utc = ((batching_element->timestampUtc + 500ULL) / 1000ULL) * 1000ULL;
	  
    /* Batch ALL is set !! */
#define MIN_TBF_FOR_BATCH_ALL_TO_KEEP_ALL_FIXES  (60*1000ULL) /* 60 seconds */
    /* Batch All is set, and the latest fix comes MIN_TBF_FOR_BATCH_ALL_TO_KEEP_ALL_FIXES 
     * milli seconds after the last fix. Keep this one and the previously batched fix */
    if((curr_time_utc - client_ptr->batching_info.last_batched_fix_utc)
        >= MIN_TBF_FOR_BATCH_ALL_TO_KEEP_ALL_FIXES)
    {
      LOC_MSG_HIGH("Time bet new & last fix: (%d) > 60 secs. Keep both",
          (curr_time_utc - client_ptr->batching_info.last_batched_fix_utc), 
          batchCfg->minInterval, 0);	    	    
      return;
    }
    
    /* Time difference does not tell us which fix to keep. Select the "better
     * fix" */
    last_fix_index  = client_ptr->batching_info.write_index - 1;
    if (last_fix_index < 0)
    {
      last_fix_index = client_ptr->batching_info.max_array_size - 1;
    }

    last_position = &client_ptr->batching_info.loc_batching_array[last_fix_index];
    
    /* We are biased towards keeping both fixes!! Figure out if two successive 
     * positions have a large intersection */
    distance_bet_positions =  (uint32)(loc_CalcDistBetLatLongHaversine(
                                batching_element->latitude  * LAT_LONG_SCALE,
                                batching_element->longitude * LAT_LONG_SCALE,
                                last_position->latitude  * LAT_LONG_SCALE, 
                                last_position->longitude * LAT_LONG_SCALE));
    hepe_sum = batching_element->horUncCircular + last_position->horUncCircular;


    MSG_5(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED,
          "Dist bet posn %d NewHepe %d newTech 0x%x OldHepe %d oldTech 0x%x",
          (distance_bet_positions), 
          batching_element->horUncCircular, last_position->horUncCircular,
          batching_element->technologyMask, last_position->technologyMask);
    
    /* Comparing the Distance between the positions with the horiz unc is a 
     * crude measure of how much the two positions intersect. 
     *
     * If we have a large intersection, we can afford to keep the better fix!! */
    if(distance_bet_positions <= (0.68 * hepe_sum))
    {
      /* Keep the fix with the better HEPE */
      if(last_position->horUncCircular < batching_element->horUncCircular)
      {
        LOC_MSG_HIGH("Drop New Fix!!", 0, 0, 0);
        *ignoreNewFix    = TRUE;
        *overWriteOldFix = FALSE;
      }
      else
      {
        LOC_MSG_HIGH("Overwrite Old Fix!!", 0, 0, 0);
        *ignoreNewFix    = FALSE;
        *overWriteOldFix = TRUE;
      }
      return;	    
    }
    else if(distance_bet_positions <= (0.8 * hepe_sum))
    {
      if((last_position->horUncCircular <= GOOD_HEPE_THRESHOLD_FOR_NEW_FIXES)    &&
         (batching_element->horUncCircular <= GOOD_HEPE_THRESHOLD_FOR_NEW_FIXES))
      {
        LOC_MSG_HIGH("Both fixes have good hepe. Keep both!", 0, 0, 0);
        /* Keep both fixes if both fixes have good hepe even if they intersect */
        return;
      }
      
      /* One of the two fixes does not have a good HEPE. Keep the fix with the 
       * better HEPE */
      if(last_position->horUncCircular < batching_element->horUncCircular)
      {
        LOC_MSG_HIGH("Drop New Fix!!", 0, 0, 0);
        *ignoreNewFix    = TRUE;
        *overWriteOldFix = FALSE;
      }
      else
      {
        LOC_MSG_HIGH("Overwrite Old Fix!!", 0, 0, 0);
        *ignoreNewFix    = FALSE;
        *overWriteOldFix = TRUE;
      }
      return;
    }
    else
    {
      LOC_MSG_HIGH("Keep both fixes as intersection is small!", 0, 0, 0);
      return;
    }
  }


  LOC_MSG_ERROR("Could not pick better of two fixes. Keep both!", 0, 0, 0);
  return;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to push the position report to LIFO buffer
  
@param[in]   client_ptr            Client Info Handle 
@param[in]   batching_element      Batched position report 


@retval    TRUE           adding position report to LIFO success
@retval    FALSE          adding position report to LIFO fail
*//* ------------------------------------------------------------------------*/
static boolean loc_batching_push_to_lifo
(
   locQmiShimCltInfoStructT           *const client_ptr,
   qmiLocBatchedReportStructT_v02   *const batching_element,
   locAonBatchingConfig             *const batchCfg
)
{
  uint64 curr_time_utc   = 0;
  int32 curr_write_index = 0;
  boolean ignoreNewFix    = FALSE;
  boolean overWriteOldFix = FALSE;
  
  if((client_ptr == NULL) || (batching_element == NULL) || (batchCfg == NULL))
  {
     LOC_MSG_ERROR("Null pointer!", 0, 0, 0);
     return FALSE;
  }

  if( NULL == client_ptr->batching_info.loc_batching_array )
  {
     LOC_MSG_ERROR("Null loc_batching_array!", 0, 0, 0);
     return FALSE;
  }

  /* round off timestamp to nearest second */
  curr_time_utc = ((batching_element->timestampUtc + 500ULL) / 1000ULL) * 1000ULL;

  if(client_ptr->batching_info.total_occupied <= 0)
  {
    /* buffer is anyways empty, reset the write/read pointers */
    client_ptr->batching_info.write_index = 0;
    client_ptr->batching_info.read_index = -1;
    LOC_MSG_HIGH("PUSH: total_occupied <= 0 hence resetting write (0) & read (-1) indexes", 0, 0, 0);
  }
  curr_write_index = client_ptr->batching_info.write_index;
  
  /*****************************************************************************  
   * Figure out if we need to overwrite a fix in our buffer or throw away the  *
   * new fix, or keep the last fix as well as the latest fix                   *
   ****************************************************************************/
  loc_batching_select_best_fixes(client_ptr,
                                 batching_element,
                                 batchCfg,
                                 &ignoreNewFix,
                                 &overWriteOldFix);

  if(ignoreNewFix == TRUE)
  {
    LOC_MSG_HIGH("Ignore Latest Fix!", 0, 0, 0);
    return FALSE;
  }
  else if(overWriteOldFix == TRUE)
  {
    /* position should be written at index one behind the write index */
    curr_write_index = curr_write_index - 1;
    if(curr_write_index < 0)
    {
      curr_write_index = client_ptr->batching_info.max_array_size - 1;
    }
      
    LOC_MSG_HIGH("PUSH: Fix rcvd in %u ms interval, fix id %d overwrite fix@ idx %d",
          (uint32)(curr_time_utc - client_ptr->batching_info.last_batched_fix_utc), 
          batching_element->fixId,
          curr_write_index);
    
  }
  else
  {
    LOC_MSG_HIGH("Keep both the last and the latest fix!", 0, 0, 0);
  }

  /******************* Update the fix at curr_write_index ******************/
  /* update the fix id in the stored report */
  batching_element->fixId = client_ptr->batching_info.fix_id;  // update fixId for live report & batched pos
  
  /* copy the position into the buffer */
  client_ptr->batching_info.loc_batching_array[curr_write_index] = (*batching_element);
    
  /************************* increment the write index **********************/
  client_ptr->batching_info.write_index = curr_write_index+1;

  /* Check if the write index reached the top of buffer and reset. Mark the overwrite flag */
  if ( client_ptr->batching_info.write_index == client_ptr->batching_info.max_array_size )
  {
    client_ptr->batching_info.write_index = 0;
    client_ptr->batching_info.overwrite_flag = TRUE;
    LOC_MSG_LOW("PUSH: write_index reached max (%d) resetting to 0, and overwrite_flag is TRUE",
      client_ptr->batching_info.max_array_size, 0, 0);
  }

  if(ignoreNewFix == FALSE)
  {
    if(overWriteOldFix == FALSE)
    {
      /* If an old fix is not being overwritten, increment the total number of 
       * occupied fixes */
      (client_ptr->batching_info.total_occupied)++;
      (client_ptr->batching_info.num_fixes_batched_lifetime++);
      /* update the last last batched fix time. Subsequent positions will be checked against this */ 
      client_ptr->batching_info.last_last_batched_fix_utc = client_ptr->batching_info.last_batched_fix_utc;
    }
    client_ptr->batching_info.last_batched_fix_utc = curr_time_utc;      
  }

  /* array overwrites, the value still remains MAX-ARRAY */
  if ( client_ptr->batching_info.total_occupied > client_ptr->batching_info.max_array_size )
  {
    /* counter always remains same */
    client_ptr->batching_info.total_occupied = client_ptr->batching_info.max_array_size;
    LOC_MSG_LOW("PUSH: total_occupied peaked (%d), write index (%d)", client_ptr->batching_info.total_occupied, client_ptr->batching_info.write_index, 0);
  }

  return TRUE;
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to send QMI_LOC_EVENT_BATCH_FULL_NOTIFICATION_IND to the client
  
@param[in]   pz_LocClient   Loc Client Info

@retval    TRUE           event reporting success
@retval    FALSE          event reporting fail
*//* ------------------------------------------------------------------------*/
static boolean loc_batching_report_batch_full_event
(
   locQmiShimCltInfoStructT    *pz_LocClient
)
{
   qmiLocEventBatchFullIndMsgT_v02  z_BatchFullInd = {0};
   locQmiShimIndInfoStructT         z_ShimIndInfo = {0};

   /* Send report only if the client registered */
   if ( 0 == (pz_LocClient->t_EventMask & QMI_LOC_EVENT_MASK_BATCH_FULL_NOTIFICATION_V02) )
   {
      LOC_MSG_HIGH("Client did not register for BATCH_FULL_NOTIFICATION, registered Loc t_EventRegMask = 0x%8x",
                  pz_LocClient->t_EventMask,0,0);
      return FALSE; 
   }

   /* client registered for QMI_LOC_EVENT_MASK_BATCH_FULL_V02 event  */
   z_BatchFullInd.batchCount = pz_LocClient->batching_info.max_array_size; /* size of batch to be notified */

   z_ShimIndInfo.p_Msg = &z_BatchFullInd;
   z_ShimIndInfo.q_MsgLen = sizeof(z_BatchFullInd);
   z_ShimIndInfo.q_Id = QMI_LOC_EVENT_BATCH_FULL_NOTIFICATION_IND_V02;

   LOC_MSG_LOW("loc_batching_report_batch_full_event Ind:Clt:%u, Count:%u",
                  QMI_LOC_EVENT_BATCH_FULL_NOTIFICATION_IND_V02, z_BatchFullInd.batchCount, 0);

   return locQmiShimSendInd(pz_LocClient->l_LocClientHandle,
                           &z_ShimIndInfo) ;
}



/* -----------------------------------------------------------------------*//**
@brief
  Function to handle position report from LocAPI. The function pushes the 
  batched fix into the LIFO buffer and notifies the control point depending
  on the event registration masks.
  
@param[in]   client_ptr            Client Info Handle 
@param[in]   batching_element      Pointer to the Batched position report

@retval    TRUE           push to LIFO buffer is success
@retval    FALSE          push to LIFO buffer is fail
                                  
*//* ------------------------------------------------------------------------*/
static boolean loc_batching_handle_position_report
(
   locQmiShimCltInfoStructT           *client_ptr,
   qmiLocBatchedReportStructT_v02   *batching_element,
   locAonBatchingConfig             *batchCfg
)
{ 
  if(client_ptr->batching_info.last_fix_timestamp_utc == batching_element->timestampUtc)
  {
    /* Ignore fix with time stamp same as the last fix received */
    LOC_MSG_HIGH("Ignore fix with same timestamp as last fix reported", 0, 0, 0);
    return FALSE;
  }

  client_ptr->batching_info.last_fix_timestamp_utc = batching_element->timestampUtc;

   /* push the entry into batching buffer */
  if (FALSE == loc_batching_push_to_lifo(
                   client_ptr, batching_element, batchCfg))
  {
    return FALSE;
  }

  /* update the fix id in the position report report */
  batching_element->fixId = client_ptr->batching_info.fix_id;  
   
  /* increment the fix id counter */
  client_ptr->batching_info.fix_id++; 

  LOC_MSG_HIGH("fix Id %d batched at index %d for client %d", batching_element->fixId,
	client_ptr->batching_info.write_index-1, client_ptr->l_LocClientHandle);

  /* handle:: first report live fix, and then report batch-full */

  /* check if client registered for live fixes and report */
  /*#9x05: loc_batching_report_live_position_event not supported */
  
  /* check for buffer full */
  if((client_ptr->batching_info.total_occupied == client_ptr->batching_info.max_array_size) &&
     (client_ptr->batching_info.buffer_full_reported != TRUE))
  {
    /* check if client registered for batch full event and notify */
    loc_batching_report_batch_full_event(client_ptr);
    client_ptr->batching_info.buffer_full_reported = TRUE;
  }

  return TRUE;
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to handle position report from loc AON. The function pushes the 
  batched fix into the LIFO buffer and notifies the control point depending
  on the event registration masks.
  
@param[in]   loc_client_handle     Client Handle 
@param[in]   batching_element      Pointer to the Batched position report
@param[in]   batchCfg              Pointer to the Batch config

@retval    TRUE           push to LIFO buffer is success
@retval    FALSE          push to LIFO buffer is fail
                                  
*//* ------------------------------------------------------------------------*/
boolean loc_batching_batch_position
(
  loc_client_handle_type            loc_client_handle,
  qmiLocBatchedReportStructT_v02   *batching_element,
  locAonBatchingConfig             *batchCfg
)
{
  locQmiShimCltInfoStructT *client_info_ptr = locClnt_FindHandle(loc_client_handle);

  if (NULL == client_info_ptr)
  {
    LOC_MSG_ERROR(" loc_batching_batch_position: Invalid params",0,0,0);
    return FALSE;
  }

  return (loc_batching_handle_position_report(
          client_info_ptr, batching_element, batchCfg));
}


/* -----------------------------------------------------------------------*//**
@brief
  Function to free the allocated buffer memory and reset the client's batching
   related fields. 
  
@param[in]   client_ptr            Client Info Handle 

@retval    TRUE           Buffer memory release success
@retval    FALSE          Buffer memory release failed
                                  
*//* ------------------------------------------------------------------------*/
boolean loc_batching_free_and_reset(locQmiShimCltInfoStructT     *client_ptr)
{
   if(NULL == client_ptr)
   {
      LOC_MSG_MED("loc_batching_free_and_reset NULL client_ptr", 0, 0, 0);
      return FALSE;
   }

   /* free the buffer */
   if (NULL != client_ptr->batching_info.loc_batching_array)
   {
      os_MemFree((void **)&client_ptr->batching_info.loc_batching_array);
      LOC_MSG_HIGH("loc_batching_free_and_reset free loc batching array", 0, 0, 0);
   }

   /* reset all fields */
   memset(&(client_ptr->batching_info),0,sizeof(client_ptr->batching_info));
   client_ptr->batching_info.read_index = -1;
   client_ptr->batching_info.write_index = -1;

   LOC_MSG_HIGH("loc_batching_free_and_reset for batching client SUCCESS", 0, 0, 0);
   return TRUE;
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to allocate the buffer memory based on availability and control
   points request.
  
@param[in]   client_ptr            Client Info Handle 
@param[out]  p_GetBatchSizeInd     Get Batch Indication message to control point
@param[in]   client_batch_size_req Batch size requested by control point 
@param[in]   reqTransactionId      Transaction Id in request message by control point 

@retval    TRUE           Buffer memory alloc success
@retval    FALSE          Buffer memory alloc failed
                                  
*//* ------------------------------------------------------------------------*/
boolean locGetBatchSizeHandleAllocate
(
   locQmiShimCltInfoStructT         *client_ptr,
   qmiLocGetBatchSizeIndMsgT_v02  *p_GetBatchSizeInd,
   uint32                         client_batch_size_req,
   uint32                         reqTransactionId
)
{
  qmiLocStatusEnumT_v02 request_status = eQMI_LOC_SUCCESS_V02;
  uint32 actual_batch_size = 0;
  uint32 malloc_retry_reduce = 0;
  void* buffer_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */
  

  /* check if batch size requested is valid */
  if ( client_batch_size_req <= 0 )
  {
    client_batch_size_req = 0; /* return 0 to client in indication */
    request_status = eQMI_LOC_INVALID_PARAMETER_V02; /* return failure to client in indication */
  }
  else
  {
    actual_batch_size = client_batch_size_req + LOC_BATCHING_ADDITIONAL_SLOTS; /* add extra slots for buffer manipulation */

    LOC_MSG_HIGH ("locGetBatchSizeHandleAllocate: client handle = %d, client_batch_size_req = %d, actual_batch_size = %d with reserved",
                 client_ptr->l_LocClientHandle, client_batch_size_req, actual_batch_size);

    /* malloc retry in 4 parts */
    malloc_retry_reduce = actual_batch_size / 4;

    /* avoid the possible (32bit) integer overflow when the total bytes size is computed. */
    if ( actual_batch_size < (UINT32_MAX / sizeof(qmiLocBatchedReportStructT_v02)) )   
    {
      /* actual_batch_size is in limits to avoid buffer overflow */
      LOC_MSG_HIGH("locGetBatchSizeHandleAllocate: client handle = %d, actual_batch_size is in limits to avoid buffer overflow",
                  client_ptr->l_LocClientHandle, 0, 0);

      do
      {
        buffer_ptr = (void *)os_MemAlloc(actual_batch_size * sizeof(qmiLocBatchedReportStructT_v02), OS_MEM_SCOPE_TASK);

        if ( NULL == buffer_ptr )
        {
          /* if malloc fails, reduce the size of the batch and retry */
          actual_batch_size = actual_batch_size - malloc_retry_reduce;
        }
        else
        {
          /* malloc success */
          LOC_MSG_HIGH("locGetBatchSizeHandleAllocate: Sucess Batching malloc - client handle = %d, total batch size = %d, size of each fix = %d",
                      client_ptr->l_LocClientHandle, actual_batch_size, sizeof(qmiLocBatchedReportStructT_v02));

          break;
        }
      }
      while ( actual_batch_size > LOC_BATCHING_ADDITIONAL_SLOTS );

    }
    else
    {
      LOC_MSG_HIGH("locGetBatchSizeHandleAllocate: client handle = %d, actual_batch_size is not in limits to avoid buffer overflow ",
                  client_ptr->l_LocClientHandle, 0, 0);
    }

    if ( buffer_ptr == NULL )
    {
      /* insufficient memory - return failure IND */
      client_ptr->batching_info.is_batching_client = FALSE;
      client_ptr->batching_info.client_batch_size = 0;
      client_ptr->batching_info.total_batch_size = 0;
      client_ptr->batching_info.loc_batching_array = NULL;

      client_batch_size_req = 0; /* return 0 to client in indication */
      request_status = eQMI_LOC_INSUFFICIENT_MEMORY_V02;

      LOC_MSG_HIGH ("locGetBatchSizeHandleAllocate: Batching client handle = %d, allocation failed",
                   client_ptr->l_LocClientHandle, 0, 0);
    }
    else
    {
      client_ptr->batching_info.is_batching_client = TRUE;
      client_ptr->batching_info.client_batch_size = actual_batch_size - LOC_BATCHING_ADDITIONAL_SLOTS;
      client_ptr->batching_info.total_batch_size = actual_batch_size;
      client_ptr->batching_info.loc_batching_array = (qmiLocBatchedReportStructT_v02 *)buffer_ptr;
      client_batch_size_req = client_ptr->batching_info.client_batch_size; /* return size to client in indication */
      request_status = eQMI_LOC_SUCCESS_V02;

      LOC_MSG_HIGH ("locGetBatchSizeHandleAllocate: Batching client handle = %d, batch size = %d",
                   client_ptr->l_LocClientHandle, client_ptr->batching_info.client_batch_size, 0);

      /* prepare the batching buffer and other variables */
      //loc_batching_variables_set.loc_batching_array = (loc_batched_position_s_type *)buffer_ptr;
      client_ptr->batching_info.write_index = 0;
      client_ptr->batching_info.read_index = -1;
      client_ptr->batching_info.total_occupied = 0;
      client_ptr->batching_info.buffer_full_reported = FALSE;
      client_ptr->batching_info.overwrite_flag = FALSE;
      client_ptr->batching_info.max_array_size = client_ptr->batching_info.client_batch_size;
      client_ptr->batching_info.last_batched_fix_utc = 0;
      client_ptr->batching_info.last_last_batched_fix_utc = 0;
      client_ptr->batching_info.num_fixes_batched_lifetime = 0;
      client_ptr->batching_info.fix_id = 0;
      client_ptr->batching_info.unable_to_track_reported = FALSE;
    }
  }

  /* update the status field in indication */
  p_GetBatchSizeInd->status = request_status;

  /* update batch size in indication - mandatory field */
  p_GetBatchSizeInd->batchSize = client_batch_size_req;

  /* update transaction id in indication - mandatory field */
  p_GetBatchSizeInd->transactionId = reqTransactionId; 

  return TRUE;
}


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to retrieve the position report from LIFO buffer
  
@param[in]   pz_LocClient          Loc Client Info
@param[in]   number_of_reads       Number of reports to retrieve
@param[out]  pz_ReadFromBatchInd   QMI_LOC Indication to store the reports

@retval    uint32           number of reports actually retrieved from buffer
*//* ------------------------------------------------------------------------*/
static uint32 loc_batching_pull_from_lifo
(
  locQmiShimCltInfoStructT         *pz_LocClient,
  uint32                          number_of_reads,
  qmiLocReadFromBatchIndMsgT_v02 *pz_ReadFromBatchInd
)
{
  uint32 i = 0;

  /* pReadFromBatchInd is already memset'ed at caller, so no need to initialize each field */
  if ((pz_LocClient == NULL) || (pz_ReadFromBatchInd == NULL))
  {
     LOC_MSG_ERROR("loc_batching_pull_from_lifo Null pointer!", 0, 0, 0);
     return 0;
  }

  if (NULL == pz_LocClient->batching_info.loc_batching_array)
  {
     LOC_MSG_ERROR("loc_batching_pull_from_lifo Null loc_batching_array!", 0, 0, 0);
    return 0;
  }

  /* always read = (write - 1) */
  pz_LocClient->batching_info.read_index = pz_LocClient->batching_info.write_index - 1;
  pz_LocClient->batching_info.buffer_full_reported = FALSE;

  for (i = 0; i < number_of_reads; i++)  
  {
    if (pz_LocClient->batching_info.read_index < 0) 
    {
      if (pz_LocClient->batching_info.overwrite_flag == TRUE)
      {
        pz_LocClient->batching_info.read_index = pz_LocClient->batching_info.max_array_size - 1;
        pz_LocClient->batching_info.overwrite_flag = FALSE; /* reset the flag */
        LOC_MSG_LOW("PULL: overwrite_flag resetted to FALSE", 0, 0, 0);
      }
      else
      {
        break;
      }
    }

    if (pz_LocClient->batching_info.total_occupied <= 0)
    {
      break;
    }
    
    /* update the counter fields in the indication */
    pz_ReadFromBatchInd->numberOfEntries++;
    pz_ReadFromBatchInd->batchedReportList_len++;

    /* already memset'ed the buffer */
    /* fill the position report into the indication */
    pz_ReadFromBatchInd->batchedReportList[i] = pz_LocClient->batching_info.loc_batching_array[pz_LocClient->batching_info.read_index];
    
    LOC_MSG_LOW("PULL: Peek fixId %d from Ind at index i %d", pz_ReadFromBatchInd->batchedReportList[i].fixId, i, 0);

    /* adjust the Read/Write indexes */
    pz_LocClient->batching_info.write_index = pz_LocClient->batching_info.read_index;
    (pz_LocClient->batching_info.read_index)--;
    
    /* update the total entries remaining in the buffer */
    (pz_LocClient->batching_info.total_occupied)--;
    LOC_MSG_HIGH("PULL: updated read (%d) write (%d) indexes, total_occupied = %d", pz_LocClient->batching_info.read_index,
                pz_LocClient->batching_info.write_index, pz_LocClient->batching_info.total_occupied);
  }

  if ( i > 0 )
  {
    pz_ReadFromBatchInd->numberOfEntries_valid = TRUE;
    pz_ReadFromBatchInd->batchedReportList_valid = TRUE;
  }

  LOC_MSG_HIGH("loc_batching_pull_from_lifo: number of entries returning %d total occupied %d", i, pz_LocClient->batching_info.total_occupied, 0);

  return i;
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to handle read operation from the lifo buffer. 
  
@param[in]   pz_LocClient          Loc Client Info Handle 
@param[in]   pz_ReadFromBatchReq   QMI_LOC request message
@param[out]  pz_ReadFromBatchInd   Indication with batched position reports


@retval    TRUE           number of reads field is valid
@retval    FALSE          number of reads field is not valid
*//* ------------------------------------------------------------------------*/
boolean loc_handle_batching_read
(
   locQmiShimCltInfoStructT                    *pz_LocClient,
   const  qmiLocReadFromBatchReqMsgT_v02     *pz_ReadFromBatchReq,
   qmiLocReadFromBatchIndMsgT_v02            *pz_ReadFromBatchInd
)
{
   if ((NULL == pz_LocClient) ||
      (NULL == pz_ReadFromBatchReq) ||
      (NULL == pz_ReadFromBatchInd))
   {
      LOC_MSG_HIGH("loc_handle_batching_read: NULL pointer", 0, 0, 0);
      return FALSE;
   }
   LOC_MSG_HIGH("loc_handle_batching_read: number of entries to read %d", pz_ReadFromBatchReq->numberOfEntries, 0, 0);

   /* pull the entries from batching buffer */
   loc_batching_pull_from_lifo(pz_LocClient, pz_ReadFromBatchReq->numberOfEntries, pz_ReadFromBatchInd);

   return TRUE;
}


/* -----------------------------------------------------------------------*//**
@brief
 Function to handle GetBatchSize Request from Control point 
 
@param[in]   pz_QmiShimCltInfo     Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                  to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the get batch size request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*/ /* ------------------------------------------------------------------------*/
boolean locGetBatchSizeReq(locQmiShimCltInfoStructT    *pz_QmiShimCltInfo,
                                  qmi_req_handle              z_ReqHandle,
                                  locQmiShimMsgInfoStructT    *pz_MsgInfo)
{
  qmiLocGetBatchSizeIndMsgT_v02 z_GetBatchSizeInd = { 0 };
  boolean v_RetVal = TRUE;
  boolean                       v_RespResult = TRUE; /* default TRUE */
  qmi_error_type_v01            e_RespErr = QMI_ERR_NONE_V01; /* default NO Error */
  boolean                       v_Otb = FALSE; /* outdoor trip batching */
  locQmiShimCltInfoStructT *pz_LocClient =
     locClnt_FindHandle(pz_QmiShimCltInfo->l_LocClientHandle);
  qmiLocGetBatchSizeReqMsgT_v02 *pz_GetBatchSizeReq = (qmiLocGetBatchSizeReqMsgT_v02 *)pz_MsgInfo->p_Msg;

  if ((NULL == pz_GetBatchSizeReq) || (NULL == pz_LocClient))
  {
    LOC_MSG_ERROR("locGetBatchSizeReq: NULL param(s)\n",
                  0, 0, 0);

    v_RespResult = FALSE;
    e_RespErr = QMI_ERR_INTERNAL_V01;
  }
  else if (pz_GetBatchSizeReq->batchSize <= 0)
  {
    /* check if batch size requested is valid */
    LOC_MSG_ERROR("locGetBatchSizeReq: invalid batching size %d",
                  pz_GetBatchSizeReq->batchSize, 0, 0);
    v_RespResult = FALSE;
    e_RespErr = QMI_ERR_INVALID_ARG_V01;
  }
  else if (TRUE == pz_GetBatchSizeReq->batchType_valid)
  {
    if (eQMI_LOC_LOCATION_BATCHING_V02 == pz_GetBatchSizeReq->batchType)
    {
      if (TRUE == pz_LocClient->batching_info.is_batching_client)
      {
        /* Memory is already allocated TBB client */
        LOC_MSG_ERROR("locGetBatchSizeReq: memory already allocated for TBB client",
                      0, 0, 0);
        v_RespResult = FALSE;
        e_RespErr = QMI_ERR_DEVICE_IN_USE_V01;
      }
    }
    else 
    {
      /* not supported batching type */
      LOC_MSG_ERROR("locGetBatchSizeReq: not supported batching type %d",
                    pz_GetBatchSizeReq->batchType, 0, 0);
      v_RespResult = FALSE;
      e_RespErr = QMI_ERR_INVALID_ARG_V01;
    }
  }
  /* if batchType_valid is not set, default is TBB */
  else
  {
    if (TRUE == pz_LocClient->batching_info.is_batching_client)
    {
      /* Memory is already allocated for TBB client */
      LOC_MSG_ERROR("locGetBatchSizeReq: memory already allocated for default TBB client",
                    0, 0, 0);
      v_RespResult = FALSE;
      e_RespErr = QMI_ERR_DEVICE_IN_USE_V01;
    }
  }

  /* send response first */
  v_RetVal = locBatchingOperationSendResp(pz_QmiShimCltInfo,
                                          z_ReqHandle,
                                          pz_MsgInfo,
                                          v_RespResult,
                                          e_RespErr);

  /* process the request and send indication only if RESP is SUCCESS */
  if ((TRUE == v_RespResult) && (TRUE == v_RetVal))
  {
    locQmiShimIndInfoStructT  shimIndInfo = { 0 };

    if (FALSE == v_Otb)
    {
      locGetBatchSizeHandleAllocate(pz_LocClient,
                                    &z_GetBatchSizeInd,
                                    pz_GetBatchSizeReq->batchSize,
                                    pz_GetBatchSizeReq->transactionId);
    }
    
    /* send indication */
    shimIndInfo.p_Msg = &z_GetBatchSizeInd;
    shimIndInfo.q_MsgLen = sizeof(z_GetBatchSizeInd);
    shimIndInfo.q_Id = QMI_LOC_GET_BATCH_SIZE_IND_V02;

    v_RetVal = locQmiShimSendInd(pz_LocClient->l_LocClientHandle,
                                 &shimIndInfo);
  }

  return v_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
 Function to handle ReadFromBatch Request from Control point 
 
@param[in]   pz_QmiShimCltInfo     QmiShim Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                  to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the ReadFromBatch request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*/ /* ------------------------------------------------------------------------*/
 boolean locReadFromBatchReq(locQmiShimCltInfoStructT     *pz_QmiShimCltInfo,
                                   qmi_req_handle               z_ReqHandle,
                                   locQmiShimMsgInfoStructT     *pz_MsgInfo)
{
  qmiLocReadFromBatchIndMsgT_v02   z_ReadFromBatchInd = { 0 };
  boolean v_RetVal = TRUE;
  boolean                          v_RespResult = TRUE; /* default TRUE */
  qmi_error_type_v01               e_RespErr = QMI_ERR_NONE_V01; /* default NO Error */
  boolean                          v_Otb = FALSE; /* outdoor trip batching */
  locQmiShimCltInfoStructT *pz_LocClient =
     locClnt_FindHandle(pz_QmiShimCltInfo->l_LocClientHandle);
  qmiLocReadFromBatchReqMsgT_v02 *pz_ReadFromBatchReq =
     (qmiLocReadFromBatchReqMsgT_v02 *)(pz_MsgInfo->p_Msg);
  locQmiShimIndInfoStructT         z_ShimIndInfo = { 0 };

  if ((NULL == pz_ReadFromBatchReq) || (NULL == pz_LocClient))
  {
    LOC_MSG_ERROR("locReadFromBatchReq: NULL param(s)",
                  0, 0, 0);

    v_RespResult = FALSE;
    e_RespErr = QMI_ERR_INTERNAL_V01;
    return FALSE;
  }
  else if (pz_ReadFromBatchReq->numberOfEntries > QMI_LOC_READ_FROM_BATCH_MAX_SIZE_V02)
  {
    LOC_MSG_ERROR("locReadFromBatchReq: invalid numberOfEntries %d",
                  pz_ReadFromBatchReq->numberOfEntries, 0, 0);
    v_RespResult = FALSE;
    e_RespErr = QMI_ERR_MALFORMED_MSG_V01;
  }
  else if (TRUE == pz_ReadFromBatchReq->batchType_valid)
  {
    if (eQMI_LOC_LOCATION_BATCHING_V02 == pz_ReadFromBatchReq->batchType)
    {
      /* nothing to do */
    }
    else 
    {
      LOC_MSG_ERROR("locReadFromBatchReq: invalid batchType %d",
                    pz_ReadFromBatchReq->batchType, 0, 0);
      v_RespResult = FALSE;
      e_RespErr = QMI_ERR_MALFORMED_MSG_V01;
    }
  }

  if (TRUE == v_RespResult)
  {
    if ((FALSE == v_Otb) &&
        (FALSE == pz_LocClient->batching_info.is_batching_client))
    {
      LOC_MSG_ERROR("locReadFromBatchReq: LocClient %d is not a batching client",
                    pz_LocClient->l_LocClientHandle, 0, 0);
      v_RespResult = FALSE;
      e_RespErr = QMI_ERR_INVALID_HANDLE_V01;
    }
    else if ((TRUE == v_Otb) &&
             (FALSE == pz_LocClient->z_OtbInfo.v_OtbClient))
    {
      LOC_MSG_ERROR("locReadFromBatchReq: LocClient %d is not a outdoor trip batching client",
                    pz_LocClient->l_LocClientHandle, 0, 0);
      v_RespResult = FALSE;
      e_RespErr = QMI_ERR_INVALID_HANDLE_V01;
    }
  }

  /* send response first */
  v_RetVal = locBatchingOperationSendResp(pz_QmiShimCltInfo,
                                          z_ReqHandle,
                                          pz_MsgInfo,
                                          v_RespResult,
                                          e_RespErr);

  /* process the request and send indication only if RESP is SUCCESS */
  if ((TRUE == v_RespResult) && (TRUE == v_RetVal))
  {
    if (FALSE == v_Otb)
    {
      v_RetVal = loc_handle_batching_read(pz_LocClient, pz_ReadFromBatchReq, &z_ReadFromBatchInd);
    }
    
  }

  /* send indication */
  z_ShimIndInfo.p_Msg = &z_ReadFromBatchInd;
  z_ShimIndInfo.q_MsgLen = sizeof(z_ReadFromBatchInd);
  z_ShimIndInfo.q_Id = QMI_LOC_READ_FROM_BATCH_IND_V02;

  v_RetVal = locQmiShimSendInd(pz_LocClient->l_LocClientHandle,
                               &z_ShimIndInfo);

  return v_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
 Utility function to send release batch Indication to control point.
 
@param[in]   client_ptr            Client Info Handle 
@param[out]  p_releaseBatchInd     Release Batch Indication message to control point
@param[in]   request_status        Status from buffer free operation
@param[in]   reqTransactionId      Transaction Id in request message by control point 


@retval    TRUE           locQmiShimSendInd is success
@retval    FALSE          locQmiShimSendInd is failed
*/ /* ------------------------------------------------------------------------*/
static boolean loc_send_release_batch_ind
(
   locQmiShimCltInfoStructT            *pz_LocClient,
   qmiLocReleaseBatchIndMsgT_v02 *pz_ReleaseBatchInd,
   boolean                           v_RetVal,
   uint32                            q_TransactionId,
   qmiLocBatchingTypeEnumT_v02       e_BatchType
   )
{
  locQmiShimIndInfoStructT              shimIndInfo;

  /* invalid read size, return failure */
  if (v_RetVal == FALSE)
  {
    pz_ReleaseBatchInd->status = eQMI_LOC_GENERAL_FAILURE_V02;
  }
  else
  {
    pz_ReleaseBatchInd->status = eQMI_LOC_SUCCESS_V02;
  }

  /* update transaction id in indication - mandatory field */
  pz_ReleaseBatchInd->transactionId = q_TransactionId;

  shimIndInfo.p_Msg = pz_ReleaseBatchInd;
  shimIndInfo.q_MsgLen = sizeof(*pz_ReleaseBatchInd);
  shimIndInfo.q_Id = QMI_LOC_RELEASE_BATCH_IND_V02;

  /* send indication */
  return locQmiShimSendInd(pz_LocClient->l_LocClientHandle,
                           &shimIndInfo);
}


/* -----------------------------------------------------------------------*//**
@brief
 Function to handle ReleaseBatch Request from Control point 
 
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                  to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the ReleaseBatch request

@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*/ /* ------------------------------------------------------------------------*/
 boolean locReleaseBatchReq
(
   locQmiShimCltInfoStructT      *pz_QmiShimCltInfo,
   qmi_req_handle                z_ReqHandle,
   locQmiShimMsgInfoStructT      *pz_MsgInfo
   )
{
  qmiLocReleaseBatchIndMsgT_v02 z_ReleaseBatchInd = { 0 };
  boolean                       v_RetVal = FALSE;
  boolean                       v_RespResult = TRUE; /* default TRUE */
  qmi_error_type_v01            e_RespErr = QMI_ERR_NONE_V01; /* default NO Error */
  boolean                       v_Otb = FALSE; /* outdoor trip batching */
  locQmiShimCltInfoStructT *pz_LocClient =
     locClnt_FindHandle(pz_QmiShimCltInfo->l_LocClientHandle);

  qmiLocReleaseBatchReqMsgT_v02 *pz_ReleaseBatchReq = (qmiLocReleaseBatchReqMsgT_v02 *)(pz_MsgInfo->p_Msg);

  if ((NULL == pz_ReleaseBatchReq) || (NULL == pz_LocClient))
  {
    LOC_MSG_ERROR("locReleaseBatchReq: NULL param(s)\n",
                  0, 0, 0);

    v_RespResult = FALSE;
    e_RespErr = QMI_ERR_INTERNAL_V01;
    return v_RetVal;
  }

  if ((TRUE == pz_ReleaseBatchReq->batchType_valid) &&
      (eQMI_LOC_OUTDOOR_TRIP_BATCHING_V02 == pz_ReleaseBatchReq->batchType))
  {
    v_Otb = TRUE;
  }

  if (TRUE == v_RespResult)
  {
    if (FALSE == v_Otb)
    {
      /* check if memory is already allocated for location batching client */
      if (FALSE == pz_LocClient->batching_info.is_batching_client)
      {
        LOC_MSG_ERROR("locReleaseBatchReq: client handle %d is not a batching client",
                      pz_QmiShimCltInfo->l_LocClientHandle, 0, 0);
        v_RespResult = FALSE;
        e_RespErr = QMI_ERR_INVALID_HANDLE_V01;
      }
      else if (TRUE == locAonCheckIfLocClientPresent(pz_QmiShimCltInfo->l_LocClientHandle, eLOC_AON_REQ_BATCH))
      {
        LOC_MSG_ERROR("locReleaseBatchReq: LB call QMI_LOC_STOP_BATCHING before RELEASE_BATCHING",
                      0, 0, 0);
        v_RespResult = FALSE;
        e_RespErr = QMI_ERR_INVALID_OPERATION_V01;
      }
    }
    else
    {
      /* check if memory is already allocated for Outdoor Trip Batching client */
      if (FALSE == pz_LocClient->z_OtbInfo.v_OtbClient)
      {
        LOC_MSG_ERROR("locReleaseBatchReq: client handle %d is not a Otb client",
                      pz_QmiShimCltInfo->l_LocClientHandle, 0, 0);
        v_RespResult = FALSE;
        e_RespErr = QMI_ERR_INVALID_HANDLE_V01;
      }
      else if (TRUE == locAonCheckIfLocClientPresent(pz_QmiShimCltInfo->l_LocClientHandle, eLOC_AON_REQ_OTB))
      {
        LOC_MSG_ERROR("locReleaseBatchReq: OTB call QMI_LOC_STOP_BATCHING before RELEASE_BATCHING",
                      0, 0, 0);
        v_RespResult = FALSE;
        e_RespErr = QMI_ERR_INVALID_OPERATION_V01;
      }
    }
  }

  /* send response first */
  v_RetVal = locBatchingOperationSendResp(pz_QmiShimCltInfo,
                                          z_ReqHandle,
                                          pz_MsgInfo,
                                          v_RespResult,
                                          e_RespErr);

  /* process the request and send indication only if RESP is SUCCESS */
  if ((TRUE == v_RespResult) && (TRUE == v_RetVal))
  {
    if (FALSE == v_Otb)
    {
      /* free the buffer and reset client info */
      v_RetVal = loc_batching_free_and_reset(pz_LocClient);

      /* Queue up the indication that needs to be sent */
      v_RetVal = loc_send_release_batch_ind(pz_LocClient, &z_ReleaseBatchInd, v_RetVal,
                                            pz_ReleaseBatchReq->transactionId, eQMI_LOC_LOCATION_BATCHING_V02);
    }
    else
    {
      
      if (TRUE == pz_LocClient->z_OtbInfo.v_Active)
      {
        LOC_MSG_ERROR("locReleaseBatchReq: fail because session is active", 0, 0, 0);
        v_RetVal = FALSE;
      }

      /* Queue up the OTB indication that needs to be sent */
      v_RetVal = loc_send_release_batch_ind(pz_LocClient, &z_ReleaseBatchInd, v_RetVal,
                                            pz_ReleaseBatchReq->transactionId, eQMI_LOC_OUTDOOR_TRIP_BATCHING_V02);
    }
  }

  return v_RetVal;
}
/* -----------------------------------------------------------------------*//**
@brief
 Function to handle StopBatching Request from Control point 
 
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                  to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the StopBatching request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*/ /* ------------------------------------------------------------------------*/
 boolean locStopBatchingReq
(
   locQmiShimCltInfoStructT    *pz_CltInfo,
   qmi_req_handle              z_ReqHandle,
   locQmiShimMsgInfoStructT    *pz_MsgInfo
   )
{
  boolean v_RetVal = FALSE;
  qmiLocGenRespMsgT_v02   z_QmiLocResp = { 0 };
  qmiLocStopBatchingReqMsgT_v02 *pz_StopBatchingReq = (qmiLocStopBatchingReqMsgT_v02 *)pz_MsgInfo->p_Msg;
  locQmiShimCltInfoStructT *pz_LocClient = locClnt_FindHandle(pz_CltInfo->l_LocClientHandle);
  boolean                 v_Otb = FALSE; /* outdoor trip batching */
  boolean                 v_RespResult = FALSE;

  /* Input pointers have been checked in requestCb */
  if (TRUE == pz_StopBatchingReq->batchType_valid &&
      eQMI_LOC_OUTDOOR_TRIP_BATCHING_V02 == pz_StopBatchingReq->batchType)
  {
    v_Otb = TRUE;
  }

  if (NULL == pz_LocClient)
  {
    z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    z_QmiLocResp.resp.error = QMI_ERR_INTERNAL_V01;
    LOC_MSG_ERROR("locStopBatchingReq: could not find client info",
                  0, 0, 0);
  }
  /* check if this client is a location batching client */
  else if ((FALSE == v_Otb) &&
           (FALSE == pz_LocClient->batching_info.is_batching_client))
  {
    z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    z_QmiLocResp.resp.error = QMI_ERR_INVALID_OPERATION_V01;
    LOC_MSG_ERROR("locStopBatchingReq: NOT a location batching client", 0, 0, 0);
  }
  /* check if this client is a outdoor trip batching client */
  else if ((TRUE == v_Otb) &&
           (FALSE == pz_LocClient->z_OtbInfo.v_OtbClient))
  {
    z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    z_QmiLocResp.resp.error = QMI_ERR_INVALID_OPERATION_V01;
    LOC_MSG_ERROR("locStopBatchingReq: NOT a outdoor trip batching client", 0, 0, 0);
  }
  else
  {
    v_RespResult = TRUE;
  }

  /* send response */
  v_RetVal = locBatchingOperationSendResp(pz_CltInfo,
                                          z_ReqHandle,
                                          pz_MsgInfo,
                                          v_RespResult,
                                          z_QmiLocResp.resp.error);

  /* Send a success indication if the response was a success
     if the the response was a failure no need to send an indication*/
  if ((TRUE == v_RetVal) &&
      (TRUE == v_RespResult))
  {
    locQmiShimIndInfoStructT      z_ShimIndInfo = { 0 };
    qmiLocStopBatchingIndMsgT_v02 z_StopBatchingInd = { 0 };

    /* forward the request to AoN*/
    if (FALSE == v_Otb)
    {
      locAonBatchStopReq(pz_CltInfo->l_LocClientHandle,
                         pz_StopBatchingReq,
                         &z_QmiLocResp);
    }
    

    z_StopBatchingInd.status = eQMI_LOC_SUCCESS_V02;
    z_StopBatchingInd.transactionId = pz_StopBatchingReq->transactionId;
    z_StopBatchingInd.requestId_valid = pz_StopBatchingReq->requestId_valid;
    z_StopBatchingInd.requestId = pz_StopBatchingReq->requestId;
    z_ShimIndInfo.p_Msg = &z_StopBatchingInd;
    z_ShimIndInfo.q_MsgLen = sizeof(z_StopBatchingInd);
    z_ShimIndInfo.q_Id = QMI_LOC_STOP_BATCHING_IND_V02;

    LOC_MSG_HIGH("locStopBatchingReq: <<< Send QMI_RESP 0X%04x succeed,error:%d,result:%d ",
                pz_MsgInfo->q_Id, z_QmiLocResp.resp.error, z_QmiLocResp.resp.result);
    /* send indication */
    v_RetVal = locQmiShimSendInd(pz_CltInfo->l_LocClientHandle,
                                 &z_ShimIndInfo);
  }
  else if (TRUE != v_RetVal)
  {
    LOC_MSG_ERROR("locStopBatchingReq: <<< Send QMI_RESP 0X%04x failed for LocClient",
                  pz_MsgInfo->q_Id, pz_CltInfo->l_LocClientHandle, 0);
  }

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
 Function to handle StartBatching Request from Control point 
 
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   pStartBatchingReq     Request message from the control point


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*/ /* ------------------------------------------------------------------------*/
 boolean locStartBatchingReq(locQmiShimCltInfoStructT     *pz_CltInfo,
                                   qmi_req_handle               z_ReqHandle,
                                   locQmiShimMsgInfoStructT     *pz_MsgInfo)
{
  boolean v_RetVal = FALSE;
  boolean v_SendErrorInd = FALSE;
  qmiLocStatusEnumT_v02  e_qmiLocStatus = eQMI_LOC_GENERAL_FAILURE_V02;
  qmiLocGenRespMsgT_v02  z_QmiLocResp = { 0 };
  qmiLocStartBatchingReqMsgT_v02 *pStartBatchingReq = NULL;
  boolean v_RespResult = FALSE;

  locQmiShimCltInfoStructT *client_ptr =
     locClnt_FindHandle(pz_CltInfo->l_LocClientHandle);

  if (NULL == pz_MsgInfo)
  {
    LOC_MSG_ERROR("locStartBatchingReq: Error invalid invalid msg info 0x%x",
                  pz_MsgInfo, 0, 0);
    return v_RetVal;
  }
  else if (NULL == client_ptr)
  {
    z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    z_QmiLocResp.resp.error =  QMI_ERR_INTERNAL_V01;
    LOC_MSG_ERROR("locStartBatchingReq: Error invalid client info 0x%x",
                  client_ptr, 0, 0);
  }
  else if (TRUE != client_ptr->batching_info.is_batching_client)
  {
    z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    z_QmiLocResp.resp.error =  QMI_ERR_INVALID_OPERATION_V01;
    LOC_MSG_ERROR("locStartBatchingReq: Not a batching client",
                  0, 0, 0);
  }
  else if (NULL == client_ptr->batching_info.loc_batching_array)
  {
    z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    z_QmiLocResp.resp.error =  QMI_ERR_INVALID_OPERATION_V01;
    LOC_MSG_ERROR("locStartBatchingReq: Call QMI_LOC_GET_BATCH_SIZE_REQ_V02 to allocate batching buffer",
                  0, 0, 0);
  }
  else if (pdsm_get_gps_lock_status() != PDSM_GPS_LOCK_NONE)
  {
    e_qmiLocStatus = eQMI_LOC_GNSS_DISABLED_V02;
    v_SendErrorInd = TRUE;
    v_RespResult = TRUE;
  
    LOC_MSG_ERROR("locStartBatchingReq: GPS locked",0, 0, 0);
  }
  else
  {
    pStartBatchingReq = (qmiLocStartBatchingReqMsgT_v02 *)pz_MsgInfo->p_Msg;
    /* forward the request to AoN*/
    locAonBatchStartReq(pz_CltInfo->l_LocClientHandle,
                        pStartBatchingReq,
                        &z_QmiLocResp);
    if((z_QmiLocResp.resp.error == QMI_ERR_NONE_V01) &&
        (z_QmiLocResp.resp.result == QMI_RESULT_SUCCESS_V01)) 
    {
      v_RespResult = TRUE;
    }
  }
    /* send response */
  v_RetVal =  locBatchingOperationSendResp(pz_CltInfo,
                                           z_ReqHandle,
                                           pz_MsgInfo,
                                           v_RespResult,
                                           z_QmiLocResp.resp.error);

  if (TRUE != v_RetVal)
  {
    LOC_MSG_ERROR("locStartBatchingReq: <<< Send QMI_RESP 0X%04x failed for locClient %d",
                  pz_MsgInfo->q_Id, pz_CltInfo->l_LocClientHandle, 0);
  }
  else
  {
    LOC_MSG_HIGH("locStartBatchingReq: <<< Send QMI_RESP 0X%04x succeed,error:%d,result:%d ",
                pz_MsgInfo->q_Id, z_QmiLocResp.resp.error, z_QmiLocResp.resp.result);

    if (v_SendErrorInd)
    {
      locAonSendStartBatchingInd(pz_CltInfo->l_LocClientHandle,
                                 pz_MsgInfo->q_Id, e_qmiLocStatus, eQMI_LOC_LOCATION_BATCHING_V02);
    }
  }
  return v_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
 Function to handle Stop a DBT session from Control point 
 
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                  to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the StopDBT request

@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*/ /* ------------------------------------------------------------------------*/
 boolean locStopDbtReq(locQmiShimCltInfoStructT    *pz_CltInfo,
                             qmi_req_handle              z_ReqHandle,
                             locQmiShimMsgInfoStructT    *pz_MsgInfo)
{
  boolean v_RetVal = FALSE;
  qmiLocGenRespMsgT_v02  z_QmiLocResp;
  qmiLocStopDbtReqMsgT_v02 *pStopDbtReq = NULL;

  if (NULL == pz_MsgInfo)
  {
    LOC_MSG_ERROR("locAonDbtStopReq: NULL QmiShimMsgInfo param", 0, 0, 0);
    return FALSE;
  }

  pStopDbtReq = (qmiLocStopDbtReqMsgT_v02 *)pz_MsgInfo->p_Msg;

  /* forward the request to AoN*/
  locAonDbtStopReq(pz_CltInfo->l_LocClientHandle,
                   pStopDbtReq,
                   &z_QmiLocResp);
  
  z_QmiLocResp.resp.result = QMI_RESULT_SUCCESS_V01;
  z_QmiLocResp.resp.error  = QMI_ERR_NONE_V01;
  /* send response */
  v_RetVal = locBatchingOperationSendResp(pz_CltInfo,
                                          z_ReqHandle,
                                          pz_MsgInfo,
                                          TRUE,
                                          z_QmiLocResp.resp.error
                                          );

  /* Send a success indication if the response was a success
     if the the response was a failure no need to send an indication*/
  if ((TRUE == v_RetVal) &&
      (QMI_RESULT_SUCCESS_V01 == z_QmiLocResp.resp.result))
  {
    locQmiShimIndInfoStructT  shimIndInfo;
    qmiLocStopDbtIndMsgT_v02 stopDbtInd;
    stopDbtInd.status = eQMI_LOC_SUCCESS_V02;
    stopDbtInd.reqId_valid = TRUE;
    stopDbtInd.reqId = pStopDbtReq->reqId;
    shimIndInfo.p_Msg = &stopDbtInd;
    shimIndInfo.q_MsgLen = sizeof(stopDbtInd);
    shimIndInfo.q_Id = QMI_LOC_STOP_DBT_IND_V02;

    LOC_MSG_HIGH("locAonDbtStopReq: <<< Send QMI_RESP 0X%04x succeed,error:%d,result:%d",
                pz_MsgInfo->q_Id, z_QmiLocResp.resp.error, z_QmiLocResp.resp.result);

    /* send indication */
    v_RetVal = locQmiShimSendInd(pz_CltInfo->l_LocClientHandle,
                                 &shimIndInfo);
  }
  else if (TRUE != v_RetVal)
  {
    LOC_MSG_ERROR("locAonDbtStopReq: <<< Send QMI_RESP 0X%04x failed for LocClient %d",
                  pz_MsgInfo->q_Id, pz_CltInfo->l_LocClientHandle, 0);
  }

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
 Function to handle QueryAonConfig Request from Control point 
 
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                  to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the StopBatching request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*/ /* ------------------------------------------------------------------------*/
 boolean locQueryAonConfigReq(locQmiShimCltInfoStructT    *pz_CltInfo,
                                    qmi_req_handle              z_ReqHandle,
                                    locQmiShimMsgInfoStructT    *pz_MsgInfo)
{
  boolean v_RetVal = FALSE;
  boolean resp_value = TRUE;
  qmiLocGenRespMsgT_v02  z_QmiLocResp = { 0 };

  qmiLocQueryAonConfigReqMsgT_v02 *pQueryAonConfigReq =
     (qmiLocQueryAonConfigReqMsgT_v02 *)pz_MsgInfo->p_Msg;

  locQmiShimCltInfoStructT   *client_ptr =
     locClnt_FindHandle(pz_CltInfo->l_LocClientHandle);

  LOC_MSG_HIGH("locQueryAonConfigReq : got request to query Aon Config", 0, 0, 0);

  if (NULL == client_ptr)
  {
    z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    z_QmiLocResp.resp.error = QMI_ERR_INTERNAL_V01;
    resp_value = FALSE;
    LOC_MSG_ERROR("locQueryAonConfigReq: could not find client info",
                  0, 0, 0);
  }

  /* send response */
  v_RetVal = locBatchingOperationSendResp(pz_CltInfo,
                                          z_ReqHandle,
                                          pz_MsgInfo,
                                          resp_value,
                                          z_QmiLocResp.resp.error
                                         );

  /* Send a success indication if the response was a success
     if the the response was a failure no need to send an indication*/
  if ((TRUE == v_RetVal) &&
      (QMI_RESULT_SUCCESS_V01 == z_QmiLocResp.resp.result))
  {
    locQmiShimIndInfoStructT  shimIndInfo;
    qmiLocQueryAonConfigIndMsgT_v02 aonConfigInd;

    aonConfigInd.status = eQMI_LOC_SUCCESS_V02;
    aonConfigInd.transactionId_valid = TRUE;
    aonConfigInd.transactionId = pQueryAonConfigReq->transactionId;
    aonConfigInd.aonCapability_valid = TRUE;
    aonConfigInd.aonCapability = (
                                  QMI_LOC_MASK_AON_AUTO_BATCHING_SUPPORTED_V02 |
                                     QMI_LOC_MASK_AON_DISTANCE_BASED_BATCHING_SUPPORTED_V02 |
                                     QMI_LOC_MASK_AON_TIME_BASED_BATCHING_SUPPORTED_V02 |
                                     QMI_LOC_MASK_AON_DISTANCE_BASED_TRACKING_SUPPORTED_V02 |
                                     QMI_LOC_MASK_AON_UPDATE_TBF_SUPPORTED_V02 |
                                     QMI_LOC_MASK_AON_OUTDOOR_TRIP_BATCHING_SUPPORTED_V02);

    LOC_MSG_HIGH("locQueryAonConfigReq : returning Aon Config = %d, transaction id = %d",
                aonConfigInd.aonCapability, aonConfigInd.transactionId, 0);

    shimIndInfo.p_Msg = &aonConfigInd;
    shimIndInfo.q_MsgLen = sizeof(aonConfigInd);
    shimIndInfo.q_Id = QMI_LOC_QUERY_AON_CONFIG_IND_V02;
    /* send indication */
    v_RetVal = locQmiShimSendInd(pz_CltInfo->l_LocClientHandle,
                                 &shimIndInfo);
  }
  else if (TRUE != v_RetVal)
  {
    LOC_MSG_ERROR("locQueryAonConfigReq: <<< Send QMI_RESP 0X%04x failed for LocClient %d",
                  pz_MsgInfo->q_Id, pz_CltInfo->l_LocClientHandle, 0);
  }

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
Function to handle Start Outdoor Trip Batching Request from Control point 

@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                 to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the StartOTB request

 
                                  
@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*/ /* ------------------------------------------------------------------------*/
 boolean locStartOTBReq
(
   locQmiShimCltInfoStructT     *pz_CltInfo,
   qmi_req_handle               z_ReqHandle,
   locQmiShimMsgInfoStructT     *pz_MsgInfo
   )
{
  boolean                 v_RetVal = FALSE;
   
  return v_RetVal;
}



/* -----------------------------------------------------------------------*//**
@brief
 Function to handle a Start DBT session from Control point 
 
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                  to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the StartDBT request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*/ /* ------------------------------------------------------------------------*/
boolean locStartDbtReq(locQmiShimCltInfoStructT     *pz_CltInfo,
                              qmi_req_handle               z_ReqHandle,
                              locQmiShimMsgInfoStructT     *pz_MsgInfo)
{
  boolean v_RetVal = FALSE;
  boolean v_SendErrorInd = FALSE;
  qmiLocStatusEnumT_v02  e_qmiLocStatus = eQMI_LOC_GENERAL_FAILURE_V02;
  qmiLocGenRespMsgT_v02  z_QmiLocResp ={0};
  qmiLocStartDbtReqMsgT_v02 *pStartDbtReq = NULL;

  if (NULL == pz_MsgInfo)
  {
    LOC_MSG_ERROR("locStartDbtReq: NULL QmiShimMsgInfo param", 0, 0, 0);
    return FALSE;
  }
  else if (pdsm_get_gps_lock_status() != PDSM_GPS_LOCK_NONE)
  {
    e_qmiLocStatus = eQMI_LOC_GNSS_DISABLED_V02;
    v_SendErrorInd = TRUE;
    
    LOC_MSG_ERROR("locStartDbtReq: GPS locked",0, 0, 0);
  }
  else 
  {
  pStartDbtReq = (qmiLocStartDbtReqMsgT_v02 *)pz_MsgInfo->p_Msg;
  /* forward the request to AoN*/
  locAonDbtStartReq(pz_CltInfo->l_LocClientHandle,
                    pStartDbtReq,
                    &z_QmiLocResp);
  }

  /* send response */
  v_RetVal = locBatchingOperationSendResp(pz_CltInfo,
                                          z_ReqHandle,
                                          pz_MsgInfo,
                                          TRUE,
                                          z_QmiLocResp.resp.error
                                          );

  if (TRUE != v_RetVal)
  {
    LOC_MSG_ERROR("locStartDbtReq: <<< Send QMI_RESP 0X%04x failed for LocClient %d",
                  pz_MsgInfo->q_Id, pz_CltInfo->l_LocClientHandle, 0);
  }
  else
  {
    LOC_MSG_HIGH("locStartDbtReq: <<< Send QMI_RESP 0X%04x succeed,error:%d,result:%d ",
                pz_MsgInfo->q_Id, z_QmiLocResp.resp.error, z_QmiLocResp.resp.result);

    if (v_SendErrorInd)
    {
      locAonSendStartBatchingInd(pz_CltInfo->l_LocClientHandle,
                                 pz_MsgInfo->q_Id, e_qmiLocStatus, QMILOCBATCHINGTYPEENUMT_MAX_ENUM_VAL_V02);
    }
  }

  return v_RetVal;
}


