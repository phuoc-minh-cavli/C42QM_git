/*======================================================================
rev1
               LM task main file

 GENERAL DESCRIPTION
  This file contains the implementation of Location Manager(LM) task.
  This task handles requests from various tasks like TM, MGP, GPS DIAG. 

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2020 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/lm/src/lm_tm.c#3 $$DateTime: 2021/02/11 01:36:31 $$Author: pwbldsvc $

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
02/04/21   sai   Reverting MARB SLEEP start related ARB notify handling logic 
09/01/20   sai   Changes done to support Geofence GTP Enhancement requirements
05/12/20   ska	   Addressing SMART compilation issues and KW compilation warnings
02/26/20   katta  Changed the info passed to LM from marb cb
08/21/19   ska     Added support for NMEA GSV
07/04/19   py    Retry again If single shot session fails due to MGP load fail
05/24/19   vg      Implement system unc specific session duration
05/20/19   py      Unload MGP , if MARB request unload when no active session on MGP
02/17/17   mj    Changes to not allow final fixes when RefLoc is not received for E911
07/21/16   mj    Update "lm_mpg_handle_gera_rcvr_off" to send Force RCVR OFF option for GERA
05/26/15   mj    Do not send FORCE IDLE for MSB to SA transitions and vice-versa within MO sessions  
04/02/16   mj    GERA Support
02/17/16   mj    Added new function lm_mgp_is_fix_source_reportable to indicate if the fix is reportable
02/17/16   mj    Changes to handle b_e911_session flag to indicate e911 session  
10/22/15   mj    Modify function lm_mgp_is_fix_source_reportable to add new param
03/20/15   mj    Send FORCE RECVR IDLE during mode change OR tbf change OR apptracking to single shot and vice versa
03/09/15   rk    Update ME with new timeout value during fix session start when the receiver is already On.
01/07/15   rk    Fix the print qualifier.
08/21/14   rk    Fix MGP receiver wasn't set correctly when SI MSA session is followed by Standalone/MSB.
06/07/13   rh    Removed NO_FIX report handling (now BestAvailPos) 
01/22/13   ah    Support extraction of MPG event from union tm_info_s_type
05/18/11   rc    Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
03/03/10   gk    For MSB session, PPM does not need to turn MGP to IDLE
01/12/10   gk    Force receiver off when end session is called
07/24/08   gk    Added on-demand flag for log 1370
07/21/08   ank   Apply stricter Hepe limit than 250m at qos timeout for tunnel-exit scenario.
06/24/08   ank   Reinstate dynamic hepe logic for tunnel-exit scenario.
06/12/08   lt    Support for external coarse position injection.
06/09/08   gk    Add a flag for background fixes
02/08/08   lt    Added support for featurized "allow staled GPS position"
08/01/07   rw    Add support for GPS time info request/response
07/31/07   ank   Added Dynamic QoS and Override logic.
04/13/07   jd    Updated cgps_logGpsFix call to pass NULL ptr for p_Meas parameter
03/27/07   mr    Added Code to save a GET_PPM req, while AFLT_ON is pending
03/27/07   mr    Added 0x1370 position fix log
11/16/06   mr    Added function to send  PD_API GPS_BEGIN/END events to TM 
04/04/06   mr    Initial creation of header file.
======================================================================*/

#include "gps_variation.h"
#include <stdio.h>
#include "task.h"
#include "msg.h"
#include "aries_os_api.h"

#include "lm_data.h"
#include "tm_lm_iface.h"
#include "lm_mgp.h"
#ifdef FEATURE_CGPS_USES_CDMA
#include "lm_mgp_aflt.h"
#endif /* FEATURE_CGPS_USES_CDMA */
#include "sm_api.h"
#include "lm_tm.h"
#include "event.h"
#include "gps_common.h"
#include "gnss_common.h"
#include "lm_diag.h"
#include "math.h"
#include "sm_util.h"
#include "tm_api.h"
#include "lm_dmm.h"
#include "tle_api.h"
/******************************************************************************/
  //  TODO  [an][del]  dummy apis
  

extern boolean locMW_SendIpcMsg
(
  tm_ipc_msg_id_type           e_MsgId,
  uint32                       q_PayloadSize,
  const void *                 p_PayloadData
);


boolean lm_tm_send_ppm_report_to_tm(const srch_ppm_rpt_type* const p_ppmReport)
{
return TRUE;
}
/******************************************************************************/ 
static lm_info_s_type   z_lm_info_s;
/*
******************************************************************************
 * sarf_cleanup 
 *
 * Function description:
 *
 * Cleans up SARF state machine vars. Sets them to init.
 *
 * Parameters: 
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/
void sarf_cleanup( void )
{
  z_lm_sarf_state_db.e_sarf_state = LM_SARF_MODE_SLEEP;  
  event_report( EVENT_GPS_LM_ERROR_SA_RF_VERIF );
  lm_update_session_state(LM_SESSION_STATE_NO_SESSION);
  z_lmControl.z_session_request.e_mode = LM_GPS_MODE_NONE;
}

/*
 ******************************************************************************
 * lm_send_tm_session_update_ack 
 *
 * Function description:
 *
 * This function sends a STATUS REPORT session update to TM  with a ACK
 *
 * Parameters: 
 * q_handle - LM-TM session handle for which update has to be sent
 * e_Cause  - The cause value to be sent with the update
 *
 * Return value: 
 *
 * boolean - session update successfully sent to TM or not
 *
 ******************************************************************************
*/

boolean lm_send_tm_session_update_ack(uint32 q_handle, tm_status_cause_e_type e_Cause)
{
  tm_session_update_info_s_type z_sessUpdate;
  LM_MSG_MED("Sending session update ACK to TM",0,0,0);

  z_sessUpdate.q_handle = q_handle;
  z_sessUpdate.e_update_type = TM_INFO_STATUS_REPORT;
  z_sessUpdate.z_update.z_status_rpt.e_status = TM_STATUS_ACK;
  z_sessUpdate.z_update.z_status_rpt.e_cause = e_Cause;

   return tm_session_update(&z_sessUpdate);

}

/*
 ******************************************************************************
 * lm_send_tm_session_update_nack 
 *
 * Function description:
 *
 * This function sends a STATUS REPORT session update to TM  with a NACK
 *
 * Parameters: 
 * q_handle - LM-TM session handle for which update has to be sent
 * e_Cause  - The cause value to be sent with the update
 *
 * Return value: 
 *
 * boolean - session update successfully sent to TM or not
 *
 ******************************************************************************
*/

boolean lm_send_tm_session_update_nack(uint32 q_handle, tm_status_cause_e_type e_Cause)
{
  tm_session_update_info_s_type z_sessUpdate;

  LM_MSG_MED("Sending session update NACK to TM",0,0,0);

  z_sessUpdate.q_handle = q_handle;
  z_sessUpdate.e_update_type = TM_INFO_STATUS_REPORT;
  z_sessUpdate.z_update.z_status_rpt.e_status = TM_STATUS_NACK;
  z_sessUpdate.z_update.z_status_rpt.e_cause = e_Cause;

   return tm_session_update(&z_sessUpdate);

}

/*
 ******************************************************************************
 * lm_tm_handle_final_fix_debug_info
 *
 * Function description:
 * 
 * This function generates the information needed to generate the debug NMEA
 * string for final fix report.
 *
 * Parameters: 
 * *p_fixReport - Final position fix report
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
static void lm_tm_handle_final_fix_debug_info(const sm_FixInfoStructType* const p_fixReport)
{
   tm_info_s_type z_fix_info = {0};
   FLT f_hepe = 0;
   qword           timestamp;
   uint32          q_UtcFixTimeMsec, q_GpsLeapSec, q_GpsTimeMs;
   uint16          w_GpsWeek;

   /*Set TM_INFO type*/
   z_fix_info.e_info_type = TM_INFO_FIX_DEBUG;

   /*Get the current HEPE threshold in LM*/
   if(z_lmControl.z_fix_request.q_qos_accuracy_threshold < z_lmControl.q_HepeThreshold)
   {
     z_fix_info.z_info.z_fix_debug_info.q_HepeLimit = z_lmControl.q_HepeThreshold;
   }
   else
   {
     z_fix_info.z_info.z_fix_debug_info.q_HepeLimit =
                                               z_lmControl.z_fix_request.q_qos_accuracy_threshold;
   }

   /*clear this flag initially (FIX_UNSUCCESSFUL (0)), this will be populated 
     in the following steps for the error reason. If fix report was generated
     and no Failure is seen, FIX will be reported as SUCCESS(1)*/
   z_fix_info.z_info.z_fix_debug_info.w_FixInfoMask = PDSM_LITE_PD_FIX_STATUS_DEBUG_FAIL;


   /* NULL report is generated at timeout. Need to calculate time.*/
   if( NULL == p_fixReport ) 
   {
     LM_MSG_MED("debug Nmea at time out. No FixRpt",0,0,0);

     /* generate GPS time from system time, since we do not save MGP report for
        intermediate fixes*/
     time_get_ms(timestamp);

     gps_week_ms( timestamp, &w_GpsWeek, &q_UtcFixTimeMsec ); 

     q_GpsLeapSec =   cgps_GetNvUtcOffset(); /* See the above warning */
     
     q_GpsTimeMs = q_UtcFixTimeMsec + q_GpsLeapSec*1000L ;

     /* Check for week rollover */
     gps_MsecWeekLimit((int32 *) &q_GpsTimeMs,&w_GpsWeek );
     

     LM_MSG_MED("debug Nmea time: Week:%hu, GPSMsec:%lu, UTCMsec: %lu",w_GpsWeek,q_GpsTimeMs,q_UtcFixTimeMsec);
     LM_MSG_MED("debug Nmea time: LeapSec:%lu",q_GpsLeapSec,0,0);

     z_fix_info.z_info.z_fix_debug_info.w_GpsWeek   = w_GpsWeek;
     z_fix_info.z_info.z_fix_debug_info.q_GpsTimeMs = q_GpsTimeMs;
     z_fix_info.z_info.z_fix_debug_info.q_UtcTimeMs = q_UtcFixTimeMsec;

     if ( z_lmControl.w_lastNoFixReportRsn == PDSM_LITE_PD_FIX_STATUS_DEBUG_HEPE_FAIL )
     {
        z_fix_info.z_info.z_fix_debug_info.w_FixInfoMask |= PDSM_LITE_PD_FIX_STATUS_DEBUG_HEPE_FAIL;
     }
     else
     {
        z_fix_info.z_info.z_fix_debug_info.w_FixInfoMask |= PDSM_LITE_PD_FIX_STATUS_DEBUG_LOW_SV; 
     }
   }
   else
   {
     LM_MSG_MED("debug Nmea at time out. with FixRpt",0,0,0);
     
     /*Capture GPS time from fix report. This will be used to construct DEBUG NMEA*/
     z_fix_info.z_info.z_fix_debug_info.w_GpsWeek   = p_fixReport->z_NavPos.w_GpsWeek;
     z_fix_info.z_info.z_fix_debug_info.q_GpsTimeMs = p_fixReport->z_NavPos.q_GpsTimeMs;
     z_fix_info.z_info.z_fix_debug_info.q_UtcTimeMs = p_fixReport->z_NavPos.q_UtcTimeMs;

     /*Determine the Fix fail reason*/
     if ( TRUE == p_fixReport->z_NavPos.z_SmPosFlags.b_IsPosBestAvail )
     {
        z_fix_info.z_info.z_fix_debug_info.w_FixInfoMask |= PDSM_LITE_PD_FIX_STATUS_DEBUG_LOW_SV; 
     }  
     else if ( PDSM_LITE_POSITION_RELIABILITY_LOW >= p_fixReport->z_NavPos.e_HoriRelIndicator )
     {
        z_fix_info.z_info.z_fix_debug_info.w_FixInfoMask |= PDSM_LITE_PD_FIX_STATUS_DEBUG_LOW_RELIABILITY;
     }
     else
     {
       /* calculate fix HEPE from Report*/
       f_hepe = (FLT) sqrt((p_fixReport->z_NavPos.f_ErrorEllipse[1] * 
                            p_fixReport->z_NavPos.f_ErrorEllipse[1]) + 
                           (p_fixReport->z_NavPos.f_ErrorEllipse[2] * 
                            p_fixReport->z_NavPos.f_ErrorEllipse[2]));
       if ( f_hepe > z_fix_info.z_info.z_fix_debug_info.q_HepeLimit )
       {
         z_fix_info.z_info.z_fix_debug_info.w_FixInfoMask |= PDSM_LITE_PD_FIX_STATUS_DEBUG_HEPE_FAIL;
       }
     }
     /*If no error reason set then the FIX was successful*/
     if ( 0 == z_fix_info.z_info.z_fix_debug_info.w_FixInfoMask) 
     {
        z_fix_info.z_info.z_fix_debug_info.w_FixInfoMask |= PDSM_LITE_PD_FIX_STATUS_DEBUG_SUCCESS;
     }
   }
   
   /*send this information to TM*/
   tm_info(&z_fix_info);
}

/*
 ******************************************************************************
 * lm_send_tm_session_request_abort 
 *
 * Function description:
 *
 * This function sends a abort request to TM when a error occurs
 *
 * Parameters: 
 * q_handle - LM-TM session handle for which abort request has to be sent
 * e_Cause  - The cause value for abort
 *
 * Return value: 
 *
 * boolean - session request successfully sent to TM or not
 *
 ******************************************************************************
*/

boolean lm_send_tm_session_request_error_notify(uint32 q_handle, tm_session_error_cause_e_type e_Cause)
{
  tm_session_request_s_type z_sessReq;
  LM_MSG_MED("Sending session ERROR NOTIFICATION to TM. Error Cause [%d]",(uint32)e_Cause,0,0);

  z_sessReq.q_handle = q_handle;
  z_sessReq.e_request_type = TM_REQUEST_ERROR_NOTIFY;
  z_sessReq.z_request.e_errorCause = e_Cause;

   return tm_session_request(&z_sessReq);

}

/*
 ******************************************************************************
 * lm_tm_send_position_report_to_tm
 *
 * Function description:
 *
 * This function sends position report to TM
 *
 * Parameters: 
 *
 * p_fixReport   - fix report to be sent to TM
 * u_finalReport - if the report is a final report for session or an intermediate
 *                 diag report.
 *
 * Return value: 
 *
 * boolean - update succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_position_report_to_tm(const sm_FixInfoStructType* const p_fixReport,
                                         boolean u_finalReport)
{
  tm_session_update_info_s_type z_update;

  if(p_fixReport == NULL)
  {
    LM_MSG_ERROR("NULL fix report received",0,0,0);
    return FALSE;
  }
  
  z_update.q_handle = z_lmControl.q_sessHandle;

  memscpy( &z_update.z_update.z_fix_rpt.z_GpsFixRpt, sizeof(z_update.z_update.z_fix_rpt.z_GpsFixRpt),
           p_fixReport, sizeof(*p_fixReport)
         );

  /*Populate the time reamining to the fix timeout*/
  z_update.z_update.z_fix_rpt.q_FixTimeRemainingMSec = 
  os_TimerCheck(z_lmControl.p_fixTimer); 
    

  if(u_finalReport)
  {
    /*Send final position fix report*/
    LM_MSG_HIGH("Sending FINAL_FIX report to TM.Time remaining to fix timeout: %lu",
         z_update.z_update.z_fix_rpt.q_FixTimeRemainingMSec,0,0);
    z_update.e_update_type = TM_INFO_FINAL_FIX_REPORT;
  }
  else
  {
    /*Send intermediate QoS qualified fix*/
    LM_MSG_HIGH("Sending INTERMEDIATE_FIX report to TM. Time remaining to fix timeout: %lu",
         z_update.z_update.z_fix_rpt.q_FixTimeRemainingMSec,0,0);
    z_update.e_update_type = TM_INFO_INTERMEDIATE_FIX_REPORT;
  }

  return tm_session_update(&z_update);
  
}

/*
 ******************************************************************************
 * lm_tm_send_sa_data_request_to_tm 
 *
 * Function description:
 *
 * This function sends a SA Data request from MGP to TM
 *
 * Parameters: 
 *
 * Return value: 
 *
 * boolean - session request successfully sent to TM or not
 *
 ******************************************************************************
*/

boolean lm_tm_send_sa_data_request_to_tm(void)
{
  tm_session_request_s_type z_sessReq;
  LM_MSG_MED("Sending SA Data request to TM.",0,0,0);

  z_sessReq.q_handle = z_lmControl.q_sessHandle;
  z_sessReq.e_request_type = TM_REQUEST_SA_DATA;

   return tm_session_request(&z_sessReq);
}

/*
 ******************************************************************************
 * lm_tm_send_pd_api_event_to_tm
 *
 * Function description:
 *
 * This function sends PD_API event info to TM
 *
 * Parameters: 
 *
 * e_pd_api_event   - PD API event to be sent to TM
 *
 * Return value: 
 *
 * boolean - info succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_pd_api_event_to_tm
(tm_pd_api_event_e_type  e_pd_api_event)
{
  tm_info_s_type   *z_info=NULL;
  boolean retval = FALSE;
  
  z_info = (tm_info_s_type*)os_MemAlloc(sizeof(tm_info_s_type), OS_MEM_SCOPE_TASK);
  
  if (z_info==NULL)
  {
	MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory");
	return FALSE;
  }
  LM_MSG_MED("Sending PD_API event %d to TM",e_pd_api_event,0,0);
  
  z_info->e_info_type = TM_INFO_PD_API_EVENT;
  z_info->z_info.e_pdapi_event = e_pd_api_event;

  retval= tm_info(z_info);
  os_MemFree((void * *) &z_info);
  return retval;
}
/*
 ******************************************************************************
 * lm_tm_send_gps_time_to_tm
 *
 * Function description:
 *
 * This function sends GPS time to TM
 *
 * Parameters: 
 *
 * p_rf_info   - RF INFO to be sent to TM
 *
 * Return value: 
 *
 * boolean - info succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_gps_time_to_tm
(
  sm_ReportGpsTimeStructType *p_gps_time_report
)
{
  tm_info_s_type   *z_info=NULL;
  
  boolean retval = FALSE;
  z_info = (tm_info_s_type*)os_MemAlloc(sizeof(tm_info_s_type), OS_MEM_SCOPE_TASK);
  if (z_info==NULL)
  {
	MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory");
	return FALSE;
  }

  if(p_gps_time_report == NULL)
  {
    return FALSE;
  }
  
  LM_MSG_MED("Sending GPS time to TM",0,0,0);
  
  z_info->e_info_type = TM_GPS_TIME;
  z_info->z_info.z_gps_time_info.z_gps_time_info_struct = *p_gps_time_report;

  retval= tm_info(z_info);
  os_MemFree((void * *) &z_info);
  return retval;
  
}

/*
 ******************************************************************************
 * lm_tm_send_mgp_event_to_tm
 *
 * Function description:
 *
 * This function sends Events from MGP to TM
 *
 * Parameters: 
 *
 * p_mgp_event - MGP event info to be sent to TM
 *
 * Return value: 
 *
 * boolean - info succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_mgp_event_to_tm (sm_ReportEventStructType* p_mgp_event)
{
  tm_info_mgp_event_s_type   *z_info=NULL;
  
  boolean retval = FALSE;
  z_info = (tm_info_mgp_event_s_type*)os_MemAlloc(sizeof(tm_info_mgp_event_s_type), OS_MEM_SCOPE_TASK);
  if (z_info==NULL)
  {
	MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory");
	return FALSE;
  }

  if (p_mgp_event == NULL)
  {
    return FALSE;
  }

  LM_MSG_LOW("Sending MGP EVENT %d to TM", (uint32)p_mgp_event->e_EventType, 0, 0);

  z_info->e_info_type = TM_INFO_MGP_EVENT;
  z_info->z_mgp_event = *p_mgp_event;
  retval = tm_info_mgp_event(z_info);
  os_MemFree((void * *) &z_info);
  return retval;
}

/*
 ******************************************************************************
 * lm_tm_clear_session 
 *
 * Function description:
 *
 * This function clears the session state related variables
 *
 * Parameters: 
 *
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_clear_session()
{
  LM_MSG_MED("Clearing LM-TM sesion with handle[%d]",z_lmControl.q_sessHandle,0,0);
  z_lmControl.q_reportsPending = 0;
  z_lmControl.q_sessHandle = 0;
  lm_update_session_state(LM_SESSION_STATE_NO_SESSION);
  z_lmControl.u_sessBestFixValid = FALSE;
  z_lmControl.f_sessBestHepe     = 0;
  os_MemFree((void * *) &z_lmControl.z_sessBestFixRpt);
  //memset((void*)&z_lmControl.z_sessBestFixRpt, 0, sizeof(sm_FixInfoStructType));
  z_lmControl.u_lastFixRptValid  = FALSE;
  z_lmControl.u_lastFixWasNoFixRpt = FALSE;
  os_MemFree((void * *) &z_lmControl.z_lastFixRpt);
  z_lmControl.u_MgpONParamsValid = FALSE;
  z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart = FALSE;
  z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart = 0;
  qw_set(z_lmControl.z_early_exit_decision.d_TimeSessionStart,0,0);
  z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = 0;
  z_lmControl.b_gnssFixReceivedThisSession = FALSE;	
  z_lmControl.v_gnssRefLocReceivedThisSession = FALSE;
  z_lmControl.z_session_request.e_gnss_activation_reason = LM_MPG_LOAD_FOR_INVALID_REASON;
  z_lmControl.v_LoadRetryRequired = FALSE;
}

/*
 ******************************************************************************
 * lm_tm_start_new_session 
 *
 * Function description:
 *
 * This function handles the session request  for starting a session, fix, PPM
 * or PRM in LM.
 *
 * Parameters: 
 * p_startReq - Pointer to structure containing start request
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_start_new_session(const lm_session_request_info_s_type* const p_startReq)
{  
  boolean u_send_force_idle = FALSE;
  
  if(NULL == p_startReq)
  {
    LM_MSG_ERROR("NULL start request received",0,0,0);
    return;
  }  
     
  /* When the receiver is ON, and there is an incoming E911 session OR mode change OR single shot to apptracking OR vice versa
  	 the ME may go into DPO sleep. During this time, SM still believe the engine is ON and does not send another ON command.
  	 Due to this the RCVR may not wake up and may cause delay in handling the session.
  	 To facilitate ME, we are changing receiver state from ON to IDLE(if already ON). And then later turn it ON again with the operation mode
	 in its parameter. This will help ME wake up to handle the incoming session and also have the correct mode update.
   */
  /* Send FORCE IDLE only when from SM's perspective the receiver is ON*/ 
  if ((lm_is_mgp_on() == TRUE) ||
      (LM_MGP_STATE_GOING_ON == z_lmControl.e_currentMgpState)
     )
  {
    /* If mode has changed, issue a FORCE IDLE */ //  TODO  [an][doubt][del]  will mode ? #9x05
	/* If apptracking to single shot session, issue a FORCE IDLE */
	if ((z_lmControl.z_session_request.q_num_fixes > 1) && 
			 (1 == p_startReq->z_request.z_session_request.q_num_fixes))
	{	  
	  u_send_force_idle = TRUE;
	  MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Apptracking to Single Shot transition");
	}
	/* If single shot session to apptracking, issue a FORCE IDLE */
	else if ((1 == z_lmControl.z_session_request.q_num_fixes) && 
			 (p_startReq->z_request.z_session_request.q_num_fixes > 1))
	{
	  u_send_force_idle = TRUE;
	  MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Single Shot to Apptracking transition");
	}
	/* If apptracking to apptracking with TBF change, issue a FORCE IDLE */
	else if ((z_lmControl.z_session_request.q_num_fixes > 1) && 
			 (p_startReq->z_request.z_session_request.q_num_fixes > 1) &&
			 (z_lmControl.z_session_request.q_tbf != p_startReq->z_request.z_session_request.q_tbf))
	{
	  u_send_force_idle = TRUE;
	  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Apptracking to Apptracking transition with TBF change from %u to %u", 
	  	z_lmControl.z_session_request.q_tbf,p_startReq->z_request.z_session_request.q_tbf);
	}
	else
	{
	  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "RCVR in ON state, but no FORCE IDLE sent. On-going session's mode=%d, No. of fixes=%u, TBF=%u", 
	  	z_lmControl.z_session_request.e_mode,z_lmControl.z_session_request.q_num_fixes,z_lmControl.z_session_request.q_tbf);
	}
  }
	
  if (u_send_force_idle)
  {
	MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Force RCVR IDLE, current RCVR state %u",z_lmControl.e_currentMgpState); 
    /* Put MGP in forced IDLE mode if not in this mode already */
    if ( !lm_mgp_update_mgp_state(LM_MGP_STATE_IDLE, NULL, LM_UPDATE_MGP_STATE_FLAG_FORCED) )
    {
  	  MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Failed to put MGP in forced IDLE state. But continuing the session anyway");
    }
  }  
    
  LM_MSG_MED("Starting new LM-TM sesion with mode[%d] TBF[%lu] num_fixes[%lu]",
             p_startReq->z_request.z_session_request.e_mode,
             p_startReq->z_request.z_session_request.q_tbf,
             p_startReq->z_request.z_session_request.q_num_fixes);
     
  z_lmControl.q_sessHandle = p_startReq->q_handle;
  lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
  z_lmControl.z_session_request = p_startReq->z_request.z_session_request;

  if (!os_TimerStart(z_lmControl.p_sessionTimer, p_startReq->z_request.z_fix_request.q_timeout, 0))
  {
    LM_MSG_ERROR("Failed starting Session timer", 0, 0, 0);
    return;
  }
  LM_MSG_HIGH("Session timer started with = %d", p_startReq->z_request.z_fix_request.q_timeout, 0, 0);
  
  /*Report event*/
  event_report_payload(EVENT_GPS_LM_SESSION_START,
                       sizeof(lm_gps_mode_e_type),
                       (void*)&p_startReq->z_request.z_session_request.e_mode);  
  
}

/*
 ******************************************************************************
 * lm_tm_stop_session
 *
 * Function description:
 *
 * This function handles the stopping of a session request
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_stop_session(void)
{

if((z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_FIX) ||
   (z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_GPS))
{
  LM_MSG_MED("Stopping FIX,GPS and PPM timers",0,0,0);  
  /*lint -e534 */
  os_TimerStop(z_lmControl.p_sessionTimer); 
  os_TimerStop(z_lmControl.p_fixTimer);  
  os_TimerStop(z_lmControl.p_gpsTimer); 
  
  /*lint +e534 */

  /* If SARF is in progress, then do not turn the RCVR OFF */
  if (TRUE == lm_mgp_get_SARF_status())
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "SARF (RF testing) in progress. NOT turning RCVR OFF");	
  }
  else
  {
    lm_mgp_update_mgp_state_after_final_fix(z_lmControl.z_session_request.q_num_fixes,
                                            z_lmControl.z_session_request.q_tbf);
  }	
}
lm_tm_clear_session();

/*Report event*/
event_report(EVENT_GPS_LM_SESSION_END);
  
}


/*
 ******************************************************************************
 * lm_tm_continue_session
 *
 * Function description:
 *
 * This function updated parameters of a an ongoing LM-TM session. Currently only
 * the operation_mode (MSA,MSB etc.) parameter is updated
 *
 * Parameters: 
 * p_contReq - continue request from TM 
 *
 * Return value: 
 *
 * TRUE/FALSE: operation successful or not
 *
 ******************************************************************************
*/
boolean lm_tm_continue_session(const lm_session_request_session_s_type* const p_contReq)
{
  if(p_contReq == NULL)
  {
    LM_MSG_ERROR("NULL continue request received",0,0,0);
    return FALSE;
  }  

  LM_MSG_HIGH("Updating session operation mode from %d to %d",
               z_lmControl.z_session_request.e_mode,p_contReq->e_mode,0);    
  
  z_lmControl.z_session_request.e_mode = p_contReq->e_mode; 

  /*Report event*/
  event_report_payload(EVENT_GPS_LM_SESSION_CONTINUE,
                       sizeof(lm_gps_mode_e_type),
                       (void*)&p_contReq->e_mode);
  
  return TRUE;
}

/*
 ******************************************************************************
 * lm_tm_handle_session_request_start 
 *
 * Function description:
 *
 * This function handles the session request  for starting a session, fix, PPM
 * or PRM in LM.
 *
 * Parameters: 
 * p_startReq - Pointer to structure containing start request
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/

void lm_tm_handle_session_request_start(const lm_session_request_info_s_type* const p_startReq)
{

  if(p_startReq == NULL)
  {
    LM_MSG_ERROR("NULL start request received",0,0,0);
    return;
  }
  /*REV 12/05: Remove this and all associated globals*/
  if(p_startReq->q_req_mask & (uint32)LM_SESSION_REQUEST_SESSION)
  {

    LM_MSG_MED("Received SESSION start request from TM",0,0,0);
    if(z_lmControl.e_sessionState != LM_SESSION_STATE_NO_SESSION)
    {
      /*There is a session already active. send a NACK for new session request to TM*/
      if(!lm_send_tm_session_update_nack(p_startReq->q_handle,TM_STATUS_NACK_PREVIOUS_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
      /*Start a new session*/
      lm_tm_start_new_session(p_startReq);
    }
  } /*if (LM_SESSION_REQUEST_SESSION) ends*/


  if(p_startReq->q_req_mask & (uint32)LM_SESSION_REQUEST_FIX)
  {

    LM_MSG_MED("Received FIX start request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_startReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_startReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
    /*Check if this report is already pending. If yes, then no need to start it again. If not, 
      start the report related processing*/
      if( (z_lmControl.e_sessionState == LM_SESSION_STATE_IN_SESSION_REPORT_PENDING) &&
          (z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_FIX))
      {
        /*Report processing already in progress. Do nothing*/
        LM_MSG_ERROR("A previous FIX request already pending",0,0,0);    
      }      
	  else
	  {
	  /*Start position fix procesing*/
        lm_mgp_process_fix_request(p_startReq);
		  /*lm_cleanup_session();*/
			return;
		  }

    }

  } /*if (LM_SESSION_REQUEST_FIX) ends*/

/*REV 12/05: Remove this if subroutine as it is only for meas reports and not supported*/
   /*if (LM_SESSION_REQUEST_GPS) ends*/
  /*If we reached here that means everything went OK with the request processing.
    Send an ACK back to TM*/
  if(!lm_send_tm_session_update_ack(p_startReq->q_handle,TM_STATUS_ACK_OK))
  {
    LM_MSG_ERROR("Unable to send session update ACK to TM. Stopping the entire session",0,0,0);
    /*Stop a session*/
    lm_tm_stop_session();
  }
}

/*
 ******************************************************************************
 * lm_tm_handle_session_request_stop 
 *
 * Function description:
 *
 * This function handles the session request  for stopping a session, fix, PPM
 * or PRM in LM.
 *
 * Parameters: 
 * p_stopReq - Pointer to structure containing stop request
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/

void lm_tm_handle_session_request_stop(const lm_session_request_info_s_type* const p_stopReq)
{

  if(p_stopReq == NULL)
  {
    LM_MSG_ERROR("NULL stop request received",0,0,0);
    return;
  }
  
  if(p_stopReq->q_req_mask & (uint32)LM_SESSION_REQUEST_FIX)
  {

    LM_MSG_MED("Received FIX stop request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_stopReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_stopReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {

      /*Stop position fix procesing*/
      lm_mgp_stop_position_fix_processing(p_stopReq->z_request.z_fix_request.u_force_mgp_off);

      if(z_lmControl.q_reportsPending == 0)
      {
        lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
      }

    }

  } /*if (LM_SESSION_REQUEST_FIX) ends*/


  if(p_stopReq->q_req_mask & (uint32)LM_SESSION_REQUEST_GPS)
  {

    LM_MSG_MED("Received GPS stop request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_stopReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_stopReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
      if(z_lmControl.q_reportsPending == 0)
      {
        lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
      }

    }

  } /*if (LM_SESSION_REQUEST_GPS) ends*/
                    
  if(p_stopReq->q_req_mask & (uint32)LM_SESSION_REQUEST_SESSION)
  {

    LM_MSG_MED("Received SESSION stop request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)
    {
      /*There is no session already active to be stopped. 
        Send a NACK to TM*/
      if(!lm_send_tm_session_update_nack(p_stopReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
      /*Stop a session*/
      lm_tm_stop_session();
      //lm_tm_clear_session();
      //lm_send_tm_session_update_ack(p_stopReq->q_handle,TM_STATUS_ACK_OK);
    }

  } /*if (LM_SESSION_REQUEST_SESSION) ends*/  

  /*If we reached here that means everything went OK with the request processing.
    Send an ACK back to TM*/
  if(!lm_send_tm_session_update_ack(p_stopReq->q_handle,TM_STATUS_ACK_OK))
  {
    LM_MSG_ERROR("Unable to send session update ACK to TM. Stopping the entire session",0,0,0);
    /*Stop a session*/
    lm_tm_stop_session();
  }
}

/*
 ******************************************************************************
 * lm_tm_handle_session_request_continue 
 *
 * Function description:
 *
 * This function handles the session request  for updating params of fix, PPM
 * or PRM processing in LM.
 *
 * Parameters: 
 * p_continueReq - Pointer to structure containing continue request
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/

void lm_tm_handle_session_request_continue(const lm_session_request_info_s_type* const p_continueReq)
{
  if(p_continueReq == NULL)
  {
    LM_MSG_ERROR("NULL continue request received",0,0,0);
    return;
  }

  z_lmControl.q_sessHandle = p_continueReq->q_handle;
  if(p_continueReq->q_req_mask & (uint32)LM_SESSION_REQUEST_SESSION)
  {

    LM_MSG_MED("Received SESSION continue request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_continueReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_continueReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
      /*Update session procesing params*/
      if(!lm_tm_continue_session(&p_continueReq->z_request.z_session_request))
      {
        if(!lm_send_tm_session_update_nack(p_continueReq->q_handle,TM_STATUS_NACK_CAUSE_UNKNOWN_ERROR))
        {
          LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
        }
        return;
      }

    }

  } /*if (LM_SESSION_REQUEST_SESSION) ends*/  

    
  if(p_continueReq->q_req_mask & (uint32)LM_SESSION_REQUEST_FIX)
  {

    LM_MSG_MED("Received FIX continue request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_continueReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_continueReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
      if(!(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_FIX))
      {
        /*No position fix report pending. Do nothing*/
      }
      else
      {

        z_lmControl.z_fix_request.q_qos_accuracy_threshold = p_continueReq->z_request.z_fix_request.q_qos_accuracy_threshold;
        LM_MSG_MED("SessionContinueReq updating Qos_accuracy to %d", z_lmControl.z_fix_request.q_qos_accuracy_threshold, 0, 0 );

      }

    }

  } /*if (LM_SESSION_REQUEST_FIX) ends*/


  if(p_continueReq->q_req_mask & (uint32)LM_SESSION_REQUEST_GPS)
  {

    LM_MSG_MED("Received GPS continue request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_continueReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_continueReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }

  } /*if (LM_SESSION_REQUEST_GPS) ends*/
  /*If we reached here that means everything went OK with the request processing.
    Send an ACK back to TM*/
  if(!lm_send_tm_session_update_ack(p_continueReq->q_handle,TM_STATUS_ACK_OK))
  {
    LM_MSG_ERROR("Unable to send session update ACK to TM. Stopping the entire session",0,0,0);
    /*Stop a session*/
    lm_tm_stop_session();
  }
}
/*
 ******************************************************************************
 * lm_tm_handle_session_request 
 *
 * Function description:
 *
 * This function handles the session request message from TM.
 * This message is sent by TM for:
 * 1.  starting/stopping/updating a session with LM
 * 2.  To request a PPM report, PRM report or position report
 *     for an already established session.
 *
 * Parameters: 
 * p_msg - Pointer to structure containing received message
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_tm_handle_session_request(const os_IpcMsgType* const p_msg)
{
  lm_session_request_info_s_type *p_sessReq;

  if(p_msg == NULL)
  {
    LM_MSG_ERROR("NULL OS message received",0,0,0);
    return;
  }

  /*lint -e826 */
  p_sessReq = (lm_session_request_info_s_type*) p_msg->p_Data;
  /*lint +e826 */
  
  switch(p_sessReq->e_req_type)
  {

  case LM_SESSION_REQUEST_START:
    {
      lm_tm_handle_session_request_start(p_sessReq);
      break;
    }       
  case LM_SESSION_REQUEST_STOP:
    {
      lm_tm_handle_session_request_stop(p_sessReq);
      break;
    }
  case LM_SESSION_REQUEST_CONTINUE:
    {
      lm_tm_handle_session_request_continue(p_sessReq);
      break;
    }

  }/*switch(p_sessReq->e_req_type) ends*/ 

}


/*
 ******************************************************************************
 * lm_tm_handle_set_parameter
 *
 * Function description:
 * This function handles set parameter request from TM
 * 
 *
 * Parameters: 
 * *p_Req - set parameter request from TM
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_set_parameter(const lm_request_set_param_s_type* const p_Req)
{

   if(p_Req == NULL)
   {
     LM_MSG_ERROR("Received NULL Set parameter request from TM",0,0,0);
     return;
   }
    
   LM_MSG_MED("Received SET PARAMETER request from TM",0,0,0);
   switch (p_Req->e_param_type)
   {
   case   LM_SET_PARAM_APP_TRK_GPS_ON_THRESHOLD:
       {
         lm_set_rc_on_threshold_period(p_Req->z_param_value.q_gpsOnThreshold);
         break;
       }
   case LM_SET_PARAM_APP_TRK_GPS_IDLE_THRESHOLD:
       {
         lm_set_rc_idle_threshold(p_Req->z_param_value.q_gpsIdleThreshold);
         break;
       }
   case LM_SET_PARAM_APP_TRK_DELAY_MARGIN:
       {
         lm_set_rc_shut_off_delay_margin(p_Req->z_param_value.q_delayMargin);
         break;
       }
   }
}
/*
 ******************************************************************************
 * lm_tm_handle_request 
 *
 * Function description:
 *
 * This function handles the session independent request message from TM.
 * This message is sent by TM to send a LM-TM session independent request to LM.
 * The type of request could be:
 * 1.  Turn keep warm processing ON/OFF
 * 2.  Turn NMEA sentence generation ON/OFF
 * 3.  Get MGP's Aiding data wishlist
 * 4.  Delete certain parameters in MGP database
 *
 * Parameters: 
 * p_msg - Pointer to structure containing received message
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_tm_handle_request(const os_IpcMsgType* const p_msg)
{
  lm_request_info_s_type *p_Req;

  if(p_msg == NULL)
  {
    LM_MSG_ERROR("NULL request recieved from TM",0,0,0);
    return;
  }

  /*lint -e826 */
  p_Req = (lm_request_info_s_type*) p_msg->p_Data;
  /*lint +e826 */
  
  switch(p_Req->e_req_type)
  {       
  case LM_REQUEST_DELETE_MGP_INFO:
    {
      lm_mgp_handle_delete_mgp_info(&p_Req->z_request.z_delete_request);
      break;
    }
  case LM_REQUEST_SET_PARAM:
    {
      lm_tm_handle_set_parameter(&p_Req->z_request.z_set_param_request);
      break;
    }

  case LM_REQUEST_SLOW_CLK_TIME_INJECT:
    {
      lm_mgp_handle_slow_clk_time_inject_request(&p_Req->z_request.z_slow_clk_inject_request);
      break;  
    }

  case LM_REQUEST_EXTERNAL_COARSE_POS_INJECT:    //  TODO  [an][doubt]#9x05 do we need CPI?
    {
      lm_mgp_handle_external_coarse_pos_inject_request(&p_Req->z_request.z_extern_coarse_pos_inject_request);
      break;  
    }

  case LM_REQUEST_GNSS_TIME:
    {
      lm_mgp_handle_gps_time_request(&p_Req->z_request.z_gps_time_request);
      break;  
    } 
  case LM_REQUEST_FORCE_RECVR_OFF:
    {
      lm_mpg_handle_force_rcvr_off();
      break;
  }

  case LM_REQUEST_RECVR_OFF:
    {
      lm_mpg_handle_rcvr_off();
      break;    
  }
  default:
    {
      break;
    }
  }/*switch(p_Req->e_req_type) ends*/ 

}

/*
 ******************************************************************************
 * lm_tm_handle_info 
 *
 * Function description:
 *
 * This function handles the session independent info message from TM. 
 * This message contains LM-TM session independent info from TM.
 * The information type can be:
 * 1.  Aiding data to be injected in MGP.
 * 2.  Expected network delays for getting aiding data.
 *
 * Parameters: 
 * p_info - Pointer to structure containing received message
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_tm_handle_info(const os_IpcMsgType* const p_msg)
{
  lm_info_s_type *p_Info = &z_lm_info_s;
  
  if(p_msg == NULL)
  {
    LM_MSG_ERROR("NULL info recieved from TM",0,0,0);
    return;
  }

  memset((void*)p_Info, 0, sizeof(lm_info_s_type));
  

  /*lint -e826 */
  if(lm_translate_lm_info_ipc_type_to_lm_info((lm_info_s_type_ipc_type*) p_msg->p_Data,
                                            p_Info) == FALSE)
  {
    LM_MSG_ERROR("LM Info Ipc transalation Failure",0,0,0);
    return;
  }
  
  /*lint +e826 */
  
  switch(p_Info->e_info_type)
  {      
  case LM_SESSION_INFO_POS_UNC:  
    {
      LM_MSG_LOW("Received New POS UNC [%f] from TM",p_Info->z_info.f_new_pos_unc
                 ,0,0);
      lm_mgp_handle_pos_unc(p_Info->z_info.f_new_pos_unc);
      break;    
    }
  case LM_SESSION_INFO_TIME_UNC:
    {
      LM_MSG_LOW("Received New TIME UNC[%f] from TM",p_Info->z_info.f_new_time_unc
                 ,0,0);
      lm_mgp_handle_time_unc(p_Info->z_info.f_new_time_unc);
      break;    
    }
  case LM_SESSION_INFO_TIME_OFFSET:   
    {
      LM_MSG_LOW("Received TIME Offset[%f] from TM",p_Info->z_info.f_time_offset
                 ,0,0);
      lm_mgp_handle_time_offset(p_Info->z_info.f_time_offset);
      break;    
    }

  }/*switch(p_Info->e_info_mask) ends*/ 

}
/*
 ******************************************************************************
 * lm_tm_handle_final_position_report
 *
 * Function description:
 * 
 * This function handles the final position fix report from MGP. It stops the 
 * session related timers and sets appropriate session state. The final fix
 * is sent to TM. MGP engine state after the position fix is decided. 
 *
 * Parameters: 
 * *p_fixReport - Final position fix report
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_final_position_report(const sm_FixInfoStructType* const p_fixReport)
{
  if(p_fixReport == NULL)
  {
    LM_MSG_ERROR("NULL fix report received",0,0,0);
    return;
  }
  
  /*Stop the fix timer*/
  /*lint -e534 */
  os_TimerStop(z_lmControl.p_fixTimer);

  /*lint +e534 */
  
  /*Send position report to TM and generate NMEA if enabled*/
  /*lint -e506 */
  if(!lm_tm_send_position_report_to_tm(p_fixReport,(boolean)TRUE))
  {
    LM_MSG_ERROR("Unable to send position report to TM. Clearing entire session",0,0,0);
    lm_tm_stop_session();
    return;
  }
  /*lint +e506 */
  
  /*Update fix session related state*/
  z_lmControl.q_reportsPending &= ~((uint32)LM_SESSION_REQUEST_FIX); 

  if(z_lmControl.q_reportsPending == 0)
  {
    lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
  }
  /*Generate 0x1370 log*/

  /* translation from sm_GnssNavSolution to gps_NavSolution */
  do{ /* to create new stack */
     gnss_NavSolutionStructType z_NavSolution;
   gnss_NavSolutionStructType *pz_NavSolution = NULL;
   
   memset((void*)&z_NavSolution,0,sizeof(z_NavSolution));
   pz_NavSolution = &z_NavSolution;
   
     if(sm_translate_sm_fix_report_to_gnss_nav_solution(pz_NavSolution, p_fixReport, TRUE,
       z_lmControl.z_fix_request.u_on_demand))
     {
        /* log the 0x1476 log packet from LM. */
        gnss_LogPositionReport(pz_NavSolution, NULL);
     }
  } while(0);

  /*Maintain proper MGP state*/
  lm_mgp_update_mgp_state_after_final_fix(z_lmControl.z_session_request.q_num_fixes,
                                          z_lmControl.z_session_request.q_tbf);
}

/*
 ******************************************************************************
 * lm_tm_send_prm_report_to_tm
 *
 * Function description:
 *
 * This function sends GPS PRM report to TM
 *
 * Parameters: 
 *
 * p_gpsReport   - PRM report to be sent to TM
 * u_delayed     - Is this a late report?(Arrived after PRM timer timedout.
 *
 * Return value: 
 *
 * boolean - update succesfully sent or not
 *
 ******************************************************************************
*/

boolean lm_tm_send_prm_report_to_tm(const sm_GpsMeasRptStructType* const p_measReport, boolean u_late)
{
  tm_session_update_info_s_type p_update;
  
  if(p_measReport == NULL)
  {
    LM_MSG_ERROR("NULL measurement report received",0,0,0);
    return FALSE;
  }

  if(u_late)
  {
    LM_MSG_HIGH("LATE PRM not supported",0,0,0);
  }
  else
  {
  LM_MSG_HIGH("Sending PRM  report to TM",0,0,0);
  p_update.e_update_type = TM_INFO_PRM_REPORT;
  }

  p_update.q_handle = z_lmControl.q_sessHandle;


  p_update.z_update.z_measure_rpt.z_GpsMeasRpt = *p_measReport;



  return tm_session_update(&p_update);
  
}

/*
 ******************************************************************************
 * lm_tm_hepe_check
 *
 * Function description:
 *
 * This function checks if the position fix reported by MGP qualifies the 
 * Max(250m, QoS-HEPE-Requested) provided by TM or not.
 *
 * Parameters: 
 * p_fixReport - Last good Position fix from MGP.
 *
 * Return value: 
 *
 * TRUE - if HEPE check passed
 * FALSE - if HEPE check failed
 *
 ******************************************************************************
*/
boolean lm_tm_hepe_check(const sm_FixInfoStructType* const p_fixReport)
{
   FLT f_hepe = 0;
   char b_Buffer[250];
   uint32 q_TimeFromEngOn,q_TimeFromSessStart;
   uint32 q_hepe_max = z_lmControl.z_fix_request.q_qos_accuracy_threshold;

   if(p_fixReport == NULL)
   {
     return FALSE;
   }

   f_hepe = (FLT) sqrt((p_fixReport->z_NavPos.f_ErrorEllipse[1] * 
                        p_fixReport->z_NavPos.f_ErrorEllipse[1]) + 
                       (p_fixReport->z_NavPos.f_ErrorEllipse[2] * 
                        p_fixReport->z_NavPos.f_ErrorEllipse[2]));

   if(z_lmControl.z_early_exit_decision.u_UseQoSOverride)
   {
     q_hepe_max = z_lmControl.q_HepeThreshold;
   }
   else
   {
     if(z_lmControl.z_fix_request.q_qos_accuracy_threshold < z_lmControl.q_HepeThreshold)
     {
       q_hepe_max = z_lmControl.q_HepeThreshold;
     }
   }

   /* Override Hepe limit at timeout after tunnel-exit esp to handle low qos setting */
   if((z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe == TRUE) && 
      (z_lmControl.z_early_exit_decision.u_TimeoutAfterReinstate == FALSE) &&
      (z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart == TRUE))
   {
     LM_MSG_MED("Timeout hit after reinstate!",0,0,0);
     q_hepe_max = LM_REINSTATE_TIMEOUT_HEPE_LIMIT;
     z_lmControl.z_early_exit_decision.u_TimeoutAfterReinstate = TRUE;
   }

    (void)snprintf(b_Buffer, sizeof(b_Buffer), "Override=%d Hepe=%f HepeMax= %lu \r\n",
             z_lmControl.z_early_exit_decision.u_UseQoSOverride,f_hepe, q_hepe_max);
    MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", b_Buffer);


   /* Calculate time from Session Start */
   q_TimeFromSessStart = lm_get_time_from_start(LM_TIME_FROM_SESSION_START);
   /* Calculate time from which MGP engine is ON */
   q_TimeFromEngOn = lm_get_time_from_start(LM_TIME_FROM_ENGINE_ON);

   if(f_hepe <= q_hepe_max)
   {
     z_lmControl.z_early_exit_decision.e_decision = LM_EARLY_EXIT_DECISION_REPORT_FIX_TIMEOUT;
     z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe = FALSE;
     z_lmControl.z_early_exit_decision.u_OverrideTimeOfFirstValidFixAfterEngineOn = FALSE;
     lm_log_early_exit_decision(q_TimeFromEngOn, q_TimeFromSessStart,p_fixReport->z_NavPos.q_RefFCount, 0, 0, (uint16) q_hepe_max, (uint16) f_hepe, TRUE);
     /*lint -e506 */
     return TRUE;
   }
   else
   {
     z_lmControl.z_early_exit_decision.e_decision = LM_EARLY_EXIT_DECISION_REPORT_NO_FIX_TIMEOUT;
     z_lmControl.w_lastNoFixReportRsn = PDSM_PD_FIX_STATUS_DEBUG_HEPE_FAIL;
     lm_log_early_exit_decision(q_TimeFromEngOn, q_TimeFromSessStart,p_fixReport->z_NavPos.q_RefFCount, 0, 0, (uint16) q_hepe_max, (uint16) f_hepe, TRUE);
     return FALSE;
   }
}
/*
 ******************************************************************************
 * lm_tm_handle_session_timer_timeout
 *
 * Function description:
 * This function handles session timer timeout. If a session is not able to start on MGP,
 * send the error to TM. 
 * 
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_session_timer_timeout(void)
{
  if(LM_DMM_STATE_LOADING == lm_get_dmm_state())
  {
    ERR_FATAL("DMM in Unexpected state = %d",lm_get_dmm_state(),0,0);
  }
  else
  {
    lm_tm_handle_gnss_failure(TM_ERROR_MGP_LOADING_ERROR);    
  }
}

/*
 ******************************************************************************
 * lm_tm_handle_fix_timer_timeout
 *
 * Function description:
 * This function handles positon fix timer timeout. If a fix that didn't pass
 * QoS accuracy check is available from this session, send it over to TM, else
 * send the error to TM. 
 * 
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_fix_timer_timeout(void)
{
  LM_MSG_HIGH("Fix timer timedout for handle[%d]",z_lmControl.q_sessHandle,0,0);

  /*If there is no FIX report pending, this timeout was queued just when
  fix timer was stopped. Do nothing in this case*/
  if(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_FIX)
  {
    /* means stale position is not generally allowed. This is the usually 
       way as this feature is not by default included */
    if( (z_lmControl.u_lastFixRptValid) && (lm_tm_hepe_check(z_lmControl.z_lastFixRpt)) &&
        /*Do not send RefLoc position back as final fix for E911 NILR MSB. Only CPI/GNSS fix should be sent */
        lm_mgp_is_fix_source_reportable(z_lmControl.z_lastFixRpt, TRUE) &&
        (!z_lmControl.u_lastFixWasNoFixRpt) )
    {
      /* If a non-qualified fix available & HEPE is acceptable report it only for E911 calls*/
      lm_tm_handle_final_position_report(z_lmControl.z_lastFixRpt);
      z_lmControl.u_lastFixRptValid = FALSE; /*This fix report should not be used again
                                               once it is reported*/

      os_MemFree((void * *) &z_lmControl.z_lastFixRpt);
      z_lmControl.u_lastFixWasNoFixRpt = FALSE;
      /* session has ended, so reset this flag */
      z_lmControl.b_gnssFixReceivedThisSession = FALSE;	
    }
    else
    {
      /*Send Fix related debug info, to generate DEBUG NMEA string*/
      lm_tm_handle_final_fix_debug_info(NULL);

      /*Update fix session related state*/
      z_lmControl.q_reportsPending &= ~((uint32)LM_SESSION_REQUEST_FIX); 

      if(z_lmControl.q_reportsPending == 0)
      {
        lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
      }

      /*Log QoS Timeout NO-FIX log*/  
      do 
      { /* create new stack */
         gnss_NavSolutionStructType z_navSolution;
         memset((void*)&z_navSolution,0,sizeof(z_navSolution));
     
         z_navSolution.z_NavSolutionFlags.b_IsBackground = SM_UTIL_MAKE_BOOL(z_lmControl.z_fix_request.u_on_demand);
         z_navSolution.z_NavSolutionFlags.b_IsFinal = TRUE;

         /* log the 0x1476 log packet from LM. */
         gnss_LogPositionReport(&z_navSolution, NULL); 
         /* count = NULL because we're not keeping track of how many messages we've sent out */
      } while(0);

      if(!lm_send_tm_session_request_error_notify(z_lmControl.q_sessHandle, TM_ERROR_CAUSE_FIX_TIMEOUT))
      {
        LM_MSG_ERROR("Failed to send error notification to TM. Clearing entire session",0,0,0);  
        lm_tm_stop_session();
        return;
      }

      /*If this is a cold start, initial fixes may fail. Keep MGP engine in proper state for
       application based tracking*/
      lm_mgp_update_mgp_state_after_final_fix(z_lmControl.z_session_request.q_num_fixes,
                                              z_lmControl.z_session_request.q_tbf);

    }

  }

}


/*
 ******************************************************************************
 * lm_tm_handle_gnss_failure
 *
 * Function description:
 * This function handles any error in turning gnss receiver. 
 * It sends the error to TM. 
 * 
 *
 * Parameters: 
 * Going on errors
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_gnss_failure(tm_session_error_cause_e_type e_error)
{
  LM_MSG_ERROR("Handle GNSS failure while in state %d error %d",z_lmControl.e_sessionState,e_error,0);

  /*Turn receiver off in any case*/
  if(TM_ERROR_MGP_STATUS_TIMEOUT == e_error)
  {
    /*This mgp state update should happen only
      when going on timer timeout has happened 
      other wise time receiver gets turned off
      now and so mgpdeinit gets triggered. But
      latter for TBF< RC_IDLE_TIME, the receiver
      is turned to idle because of which mgp deinit
      fails. The receiver should be turned off
      only  once after clearing all session 
      parameters.
  */
    lm_mgp_update_mgp_state(LM_MGP_STATE_OFF,NULL,LM_UPDATE_MGP_STATE_FLAG_NONE);
  }
  if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)
  {
    /* If No session on LM, Turn receiver off. Internally it will led to MGP unload.*/
    lm_mgp_update_mgp_state(LM_MGP_STATE_OFF,NULL,LM_UPDATE_MGP_STATE_FLAG_NONE);
  }
  else
  {
    /*Report abort state to TM if not already reported a previous non-recoverable error*/
    if(z_lmControl.e_sessionState != LM_SESSION_STATE_IN_SESSION_ERROR_REPORTED)
    {
      if(!lm_send_tm_session_request_error_notify(z_lmControl.q_sessHandle,e_error))
      {
        LM_MSG_ERROR("Failed to send error notification to TM. Clearing entire session",0,0,0);  
        if(TM_ERROR_MGP_STATUS_TIMEOUT != e_error)
        {
           /*
             If this function is called for any reason 
             other going on timer time out then the
             receiver needs to be turned off and so
           */
           lm_mgp_stop_position_fix_processing(TRUE);
        }
        lm_tm_stop_session();
        return;
      }
    }
  }
  if( z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_GOING_ON )
  {
    sarf_cleanup();
  }
}

/*
 ******************************************************************************
 * lm_tm_handle_goingOff_timer_timeout
 *
 * Function description:
 * This function handles GOING_OFF timer timeout. 
 * 
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_goingOff_timer_timeout(void)
{
  LM_MSG_ERROR("GOING OFF Timer timedout while in state %d",z_lmControl.e_sessionState,0,0);
  /*Turn receiver off in any case*/    
  if(LM_DMM_STATE_LOADED ==lm_get_dmm_state())
  { 
  lm_mgp_update_mgp_state(LM_MGP_STATE_OFF,NULL,LM_UPDATE_MGP_STATE_FLAG_NONE);
  if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)
  {
  }
  else
  {
    /*Report abort state to TM if not already reported a previous non-recoverable error*/
    if(z_lmControl.e_sessionState != LM_SESSION_STATE_IN_SESSION_ERROR_REPORTED)
    {
      if(!lm_send_tm_session_request_error_notify(z_lmControl.q_sessHandle,TM_ERROR_MGP_STATUS_TIMEOUT))
      {
        LM_MSG_ERROR("Failed to send error notification to TM. Clearing entire session",0,0,0);  
        lm_tm_stop_session();
        return;
      }
    }
  }

  if( z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_GOING_OFF )
  {
    sarf_cleanup();
  }
  }
  else if (LM_DMM_STATE_UNLOADING ==lm_get_dmm_state()) 
  {
    lm_dmm_state_machine(LM_DMM_EVENT_RCVR_OFF_IND);
  }


}

/*
 ******************************************************************************
 * lm_tm_handle_goingIdle_timer_timeout
 *
 * Function description:
 * This function handles GOING_IDLE timer timeout. 
 * 
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_goingIdle_timer_timeout(void)
{
  LM_MSG_ERROR("GOING IDLE Timer timedout while in state %d",z_lmControl.e_sessionState,0,0);
  /*Turn receiver off in any case*/
  lm_mgp_update_mgp_state(LM_MGP_STATE_OFF,NULL,LM_UPDATE_MGP_STATE_FLAG_NONE);

  if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)
  {
  }
  else
  {
    /*Report abort state to TM if not already reported a previous non-recoverable error*/
    if(z_lmControl.e_sessionState != LM_SESSION_STATE_IN_SESSION_ERROR_REPORTED)
    {
      if(!lm_send_tm_session_request_error_notify(z_lmControl.q_sessHandle,TM_ERROR_MGP_STATUS_TIMEOUT))
      {
        LM_MSG_ERROR("Failed to send error notification to TM. Clearing entire session",0,0,0);  
        lm_tm_stop_session();
        return;
      }
    }
  }
  if( z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_GOING_ON ||
      z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_GOING_OFF)
  {
    sarf_cleanup();
  }
}

/*===========================================================================

FUNCTION lm_get_sys_status_info()

DESCRIPTION
  calls TM LM IFACE to get system status update

DEPENDENCIES

RETURN VALUE 
  uint32 : system state constant

SIDE EFFECTS

===========================================================================*/
uint32 lm_get_sys_status_info (void)
{
  return(tm_get_sys_status_info());
}

/*===========================================================================

FUNCTION lm_tm_handle_arb_notify()

DESCRIPTION
  Handle ARB notify indication and handle dmm sate and decide to load GNSS engine

DEPENDENCIES

RETURN VALUE 
  void

SIDE EFFECTS

===========================================================================*/
void lm_tm_handle_arb_notify(os_IpcMsgType *p_msg)
{
  if(p_msg == NULL)
  {
    LM_MSG_ERROR_0("NULL OS message received");
    return;
  }

    (void)lm_dmm_state_machine(LM_DMM_EVENT_ARB_NOTIFY_IND);
    (void)lm_tm_arb_notify_handler();
  
}

/*===========================================================================

FUNCTION lm_tm_arb_notify_handler()

DESCRIPTION
  Handle ARB notify indication and decide to start a session or not

DEPENDENCIES

RETURN VALUE 
  void

SIDE EFFECTS

===========================================================================*/
void lm_tm_arb_notify_handler(void)
{
  uint32 q_session_time_left; 

  lm_tm_notify_gnss_availability(TRUE);
  
  if(z_lmControl.z_session_request.q_num_fixes > 1)
  {
     /* When an ARB notify comes in while GNSS is unloaded, it indicates there is an oppurtunity window when MGP
        can be loaded . Notify this to LocMW, to check if any client needs to be scheduled right away.
     */     
     locMW_SendIpcMsg(LM_MIDDLEWARE_MSG_ID_OPPORTUNISTIC_SCHEDULE_IND, 0, NULL);    
     return;
  }

  q_session_time_left = os_TimerCheck(z_lmControl.p_sessionTimer);
  LM_MSG_HIGH_1("Session time left =  %d", q_session_time_left);

  if((TRUE == z_lmControl.v_LoadRetryRequired))
  {
    /* For single shot session, Need to retry if it has failed before if Time left on session timer should 
       be more than 8 seconds
    */ 
    z_lmControl.v_LoadRetryRequired = FALSE;

    if(LM_DMM_EXPECTED_SESS_DURATION_SYS_STATUS_VALID_ALL < q_session_time_left)
    {
      lm_dmm_state_machine(LM_DMM_EVENT_MGP_LOAD_REQ);
    }
    else
    {
      LM_MSG_HIGH_0("Ignore ARB notify. Session time left not enough to get fix");
    }
  }
  else
  {
    LM_MSG_HIGH_0("Ignore ARB notify. No MGP load failure for the client before.");
  }  
  
  return;    
}
/*===========================================================================

FUNCTION lm_tm_arb_notify_handler()

DESCRIPTION
  Handle ARB notify indication and indicate to TLE

DEPENDENCIES

RETURN VALUE 
  void

SIDE EFFECTS

===========================================================================*/
void lm_notify_marb_events_to_tle(os_IpcMsgType *p_msg)
{
  switch(p_msg->q_MsgId)
  {
	case C_SM_LM_MSG_ARB_NOTIFY_IND:
	{
	  cgps_WwanStateInfo edrx_info;
	  const marb_async_callback_input_t * input;
	  memset(&edrx_info, 0x00, sizeof(edrx_info));
	  input = (const marb_async_callback_input_t *)p_msg->p_Data;
	  edrx_info.wwan_sleep_start = input->cb_data.notify.sleep_info.sleep_start;
      edrx_info.wwan_sleep_end = input->cb_data.notify.sleep_info.sleep_end;
	  tle_EdrxStateNotify(&edrx_info);
	  break;
	} 
  }
}
/*===========================================================================
  lm_tm_notify_gnss_availability

  Description:

     This function handles marb notification and failure indication

 Parameters:

   u_notifymarbfail:  TRUE for MARB Notify and FALSE for MGP Load failure

  Return value:
    void

=============================================================================*/
void lm_tm_notify_gnss_availability(boolean u_GnssAvail)
{
  tm_core_notify_gnss_availability(u_GnssAvail);
  return;
}


