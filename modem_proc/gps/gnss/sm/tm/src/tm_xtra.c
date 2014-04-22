/*===========================================================================

                  TM - XTRA Core Module

DESCRIPTION
  This file contains implementations for TM XTRA Core.

  Responsible for handling XTRA related functionality. 

  

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2017 Qualcomm Technologies, Inc. 
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*
Copyright (c) 2013-2014 Qualcomm Atheros, Inc.
All Rights Reserved.
Qualcomm Atheros Confidential and Proprietary.
*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/src/tm_xtra.c#4 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/21   vag     Remove NTP DL code when XTRA is valid but NV74194 is set to 1.
05/23/19   vg      Implement system unc specific session duration
10/17/17   rn      Added Xtra Integrity support.
06/13/17   sg      Added support to use GTS time while filling XTRA config
05/11/17   sg      Added support to query XTRA config information
09/16/15   skm     Xtra Version check fix  
05/12/14   ah      Added support for Set XTRA Version Check
12/06/13   ah      Support for non-deferrable timers
09/24/13   rh      Allow one-part XTRA injection for XTRA3c.bin
09/30/10   rh      Changed ERR_OTHER to more meaningful error codes
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec
04/01/08   rw      Add gpsOneXTRA events
06/21/07   rw      Initial Release
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"   /* Definition for basic types and macros */
#include "customer.h"
#include "gts_api.h"
#include "aries_os_api.h"
//#include <string.h>
/*---------------------------------------------------------------------------

   This section contain code which is only built when 
   FEATURE_GPSONE_XTRA is defined.
   
----------------------------------------------------------------------------*/
#ifdef FEATURE_CGPS_XTRA
#include "pdapiclient_lite.h"  //adding to make sm work
#include "pdapi_lite.h"
#include "time_svc.h"
#include  <stdio.h>
#include  <stdlib.h>
#include "gps_common.h"
#include "tm_cm_iface.h"
#include "tm_common.h"
#include "tm_nv.h"

#include "lm_api.h"
#include "tm_api.h"
#include "tm_xtra.h"
#include "tm_data.h"
#include "mgp_api.h"

#include "sm_dm_event.h"

#ifdef FEATURE_CGPS_XTRA_T
#include "tle_api.h"
#endif

#include "aries_os_api.h"

/*------------------------- Constants ------------------------------------*/
#define TM_XTRA_UINT64_MAX 0xFFFFFFFFFFFFFFFF

/*------------------------- Global Variables ------------------------------------*/
#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
tm_xtra_state_type*  p_xtra_state_info_ptr = NULL;  /* TM XTRA state information */
#else
tm_xtra_state_type  z_xtra_state_info;  /* TM XTRA state information */
#endif /*FEATURE_CGPS_XTRA_DYNAMIC_ALLOC*/

static uint64 t_LastNtpInjectGpsTimeMs = TM_XTRA_UINT64_MAX;
static boolean b_ntp_download_needed = FALSE;

#ifdef FEATURE_CGPS_XTRA_DEBUG
/* Used for debugging */
static char pchAsciiOutput[XTRA_TEMP_ASCII_STRING_SIZE];
#endif /* FEATURE_CGPS_XTRA_DEBUG */

#ifdef FEATURE_GNSS_XTRA3 
/* since tm_decode_xtra3_data.h is not shiped, declare it here */
void tm_xtra3_set_download_url(uint8 **Xtra3Url_ptr, uint32 NumUrl, uint32 MaxUrlLen);
#endif /* FEATURE_GNSS_XTRA3 */
#include "gts_loc_api.h"
#include "aries_os_api.h"

extern void tm_core_notify_xtra_process_done (boolean status);
/*===========================================================================
=============================================================================

   XTRA Module functions
   
=============================================================================
===========================================================================*/



 /*
 ******************************************************************************
 * tm_xtra_request_download
 *
 * Function description:
 *
 * This function is used to request for XTRA download from the HLOS. Note that 
 * this function does not do an existing XTRA validity check before requesting the 
 * download. It is the caller's responsibility to do the same . 
 * 
 * Parameters:
 *   p_ret : The data check validation status of the existing XTRA data 
 *
 * Return value: TRUE if the XTRA data was requested , FALSE otherwise.  
 *
 ******************************************************************************
*/
boolean tm_xtra_request_download(tm_xtra_data_check_result_e_type *p_ret);


 /*
 ******************************************************************************
 * tm_xtra_determine_data_age
 *
 * Function description:
 *
 * This function is used to determine the age of the  downloaded XTRA data . 
 * 
 * Parameters:
 *   w_GpsWeek : The GPS week 
 *   u_GpsMsec : GPS msec
 *   b_IsDataOld : Determines if the XTRA data is too old . 
 * Return value: TRUE if the APIis succesful , FALSE otherwise.  
 *
 ******************************************************************************
*/
boolean tm_xtra_determine_data_age(uint16 w_GpsWeek, uint32 u_GpsMsec, boolean *pb_IsDataOld);


 /*
 ******************************************************************************
 * tm_xtra_set_data_server_url_for_constel
 *
 * Function description:
 *
 * This function is used to set the server URLs for the 2 selected constellations
 * The first constellation is always GPS . 
 * Parameters:
 *   None
 *
 * Return value: TRUE if the XTRA data was requested , FALSE otherwise.  
 *
 ******************************************************************************
*/
boolean tm_xtra_set_data_server_url_for_constel(sm_XtraConstelEnumType e_ConstelType);

/*===========================================================================
FUNCTION tm_xtra_req_allow

DESCRIPTION
Allow XTRA data requests.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
static void tm_xtra_req_allow
(
  void
)
{
  tm_xtra_state_type *xtra_state_ptr =  tm_xtra_get_state_ptr();

  if(xtra_state_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
    return;
  }
  
  xtra_state_ptr->time_of_throttle_end = 0;
  xtra_state_ptr->dl_attempts = TM_XTRA_INIT_RETRY_ATTEMPTS;
  xtra_state_ptr->continue_blocking = FALSE;
  TM_XTRA_MSG_MED("XTRA data requests unblocked",0, 0,0); 
   
  return;
}

/*===========================================================================
FUNCTION tm_xtra_is_req_blocked

DESCRIPTION
Check XTRA data request throttling status based on timestamp.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
Throttle status
===========================================================================*/
static boolean tm_xtra_is_req_blocked
(
  void
)
{
  uint32 now;
  tm_xtra_state_type *xtra_state_ptr =  tm_xtra_get_state_ptr();
  

  if(xtra_state_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
    return TRUE;
  }
  
  now = time_get_secs();

  TM_XTRA_MSG_MED("now %lu, end %lu ",now,xtra_state_ptr->time_of_throttle_end,0 );
  
  if (now >= xtra_state_ptr->time_of_throttle_end)
  {
    TM_XTRA_MSG_MED("tm_xtra_is_req_blocked == FALSE",0,0,0);
    return FALSE;
  }
  
  TM_XTRA_MSG_MED("tm_xtra_is_req_blocked == TRUE",0,0,0);
  return TRUE;
}


/*===========================================================================
FUNCTION tm_xtra_req_block

DESCRIPTION
Block XTRA requests until retry timeout.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
static void tm_xtra_req_block
(
  void
)
{
  uint32 time_between_attempts = 0; 
  tm_xtra_state_type *xtra_state_ptr =  tm_xtra_get_state_ptr();

  if(xtra_state_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
    return;
  }

  xtra_state_ptr->dl_attempts++;
  if (xtra_state_ptr->dl_attempts <= xtra_state_ptr->num_download_attempts)
  {  
     time_between_attempts = xtra_state_ptr->time_between_attempts*(TM_XTRA_MSEC_IN_MIN/TM_XTRA_SEC_TO_MSEC);       
     TM_XTRA_MSG_MED("XTRA BLOCKED for %lu seconds, attempt = %d",time_between_attempts, xtra_state_ptr->dl_attempts,xtra_state_ptr->num_download_attempts);
  }
  else
  {
    time_between_attempts = TM_XTRA_LAST_RETRY_TIMEOUT; //seconds
    TM_XTRA_MSG_MED("XTRA BLOCKED for %lu seconds, attempt = %d",time_between_attempts, xtra_state_ptr->dl_attempts,0);
  }
  
  xtra_state_ptr->time_of_throttle_end = time_get_secs() + time_between_attempts;

  return;
}

/*===========================================================================
FUNCTION tm_xtra_init_state_ptr

DESCRIPTION
Dynamically allocate memory for the XTRA state pointer

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
Memory allocation status
===========================================================================*/
boolean tm_xtra_init_state_ptr(void)
{
#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
  if(p_xtra_state_info_ptr != NULL)
  {
    TM_XTRA_MSG_MED("XTRA state pointer mem allocation alredy done",0,0,0);   
    return TRUE;
  }
  p_xtra_state_info_ptr = (tm_xtra_state_type*)os_MemAlloc(sizeof(tm_xtra_state_type), OS_MEM_SCOPE_TASK);

  if(p_xtra_state_info_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("XTRA state pointer mem allocation failed",0,0,0);	  
	return FALSE;
  }
  else
  {
	memset((void *)p_xtra_state_info_ptr, 0x0, sizeof(tm_xtra_state_type));
    TM_XTRA_MSG_MED("XTRA state pointer mem allocation successful",0,0,0);    
	return TRUE;
  }
#else
  return TRUE;   
#endif /*FEATURE_CGPS_XTRA_DYNAMIC_ALLOC*/
}

/*===========================================================================
FUNCTION tm_xtra_init

DESCRIPTION
Initialize the XTRA module

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
void tm_xtra_init
(
  void
)
{
  /* Local Variables */
  tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();
    
  if(xtra_state_ptr == NULL)
  {
	MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
	return;
  }

  /*----------------------------------------------------------------------
     Initialize all XTRA session data
  ----------------------------------------------------------------------*/  

  /* Memory buffer to store XTRA.BIN data from XTRA Client */
#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
  if (xtra_state_ptr->xtraTempBuf != NULL)
  {
    TM_XTRA_MSG_ERROR("XTRA buffer ptr not NULL, possible memory leak!",0,0,0);
    xtra_state_ptr->xtraTempBuf = NULL;
  }
#else
  memset((void *)xtra_state_ptr->xtraTempBuf, 
         0x0,
         TM_XTRA_MAX_FILE_SIZE);
#endif

  /* Used to manage data read into XtraTempBuf */
  xtra_state_ptr->q_XtraDataLen = 0;

  /* GPS time structure */
  memset((void*)&xtra_state_ptr->gps_time,
         0x0,
         sizeof(gps_TimeStructType) );

/* Without mutex, we cannot support XTRA, disable this feature */
MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Init XTRA mutex");
  /* Initialize the XTRA almanac corrections data mutex */
if (FALSE == gnss_InitXtraMutexForDynLoading())
{
  /* Without mutex, we cannot support XTRA, disable this feature */
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Failiure in XTRA mutex init");
}
gnss_ResetCommonXtraInfo();


  /*----------------------------------------------------------------------
     XTRA NV item local settings
  ----------------------------------------------------------------------*/  

  /* Number of hours between automatic downloads [hours] */
  if(xtra_state_ptr->download_interval < TM_XTRA_MIN_DOWNLOAD_INTERVAL
     || xtra_state_ptr->download_interval > TM_XTRA_MAX_DOWNLOAD_INTERVAL)
  {
    /* Set Default value */
    xtra_state_ptr->download_interval 
                   = TM_XTRA_DEFAULT_DOWNLOAD_INTERVAL;
  }

  /* Number of download attempts (0...10) */
  if (xtra_state_ptr->num_download_attempts > TM_XTRA_MAX_NUM_DOWNLOAD_ATTEMPTS )
  {
    /* Set Default value */
    xtra_state_ptr->num_download_attempts 
                   = TM_XTRA_DEFAULT_NUM_DOWNLOAD_ATTEMPTS;
  }

  /* Number of minutes between download attempts */
  if(xtra_state_ptr->time_between_attempts < TM_XTRA_MIN_TIME_BETWEEN_ATTEMPTS 
     || xtra_state_ptr->time_between_attempts > TM_XTRA_MAX_TIME_BETWEEN_ATTEMPTS)
  {
    /* Set Default value */
    xtra_state_ptr->time_between_attempts 
                   = TM_XTRA_DEFAULT_TIME_BETWEEN_ATTEMPTS;    
  }

  /* One-way delay failover threshold */
  if(xtra_state_ptr->time_info_oneway_delay_thresh < TM_XTRA_MIN_TIME_INFO_DELAY_THRESH 
     || xtra_state_ptr->time_info_oneway_delay_thresh > TM_XTRA_MAX_TIME_INFO_DELAY_THRESH)
  {
    xtra_state_ptr->time_info_oneway_delay_thresh
                 = TM_XTRA_DEFAULT_TIME_INFO_DELAY_THRESH;
  }

  /* Time uncertainty threshold */
  if(xtra_state_ptr->time_info_uncertainty_thresh < TM_XTRA_MIN_TIME_INFO_UNC_THRESH 
     || xtra_state_ptr->time_info_uncertainty_thresh > TM_XTRA_MAX_TIME_INFO_UNC_THRESH)
  {
    xtra_state_ptr->time_info_uncertainty_thresh
                 = TM_XTRA_DEFAULT_TIME_INFO_UNC_THRESH;
  }

  /*----------------------------------------------------------------------
     Initialize TM-XTRA module session information 
  ----------------------------------------------------------------------*/
  /* data/time injection handler state */
  xtra_state_ptr->e_state = TM_XTRA_STATE_IDLE;

  /* Number of dload request retry attempts */
  xtra_state_ptr->retry_attempts =  TM_XTRA_INIT_RETRY_ATTEMPTS;

  /* Allow On Demand requests */
  tm_xtra_req_allow();
 
  /* Source of Time Information request */
  xtra_state_ptr->time_info_req_src = GPS_TIME_CHECK_SOURCE_NONE;

  /* XTRA Version Check */
  xtra_state_ptr->version_check = PDSM_LITE_PA_XTRA_VERSION_CHECK_DISABLED;

  /*----------------------------------------------------------------------
     Initialize TM-XTRA module Timers if not already created
  ----------------------------------------------------------------------*/
  if(xtra_state_ptr->auto_dload_timer == NULL)
  {
    xtra_state_ptr->auto_dload_timer =
        os_TimerCreateInCgpsTimerGroup( TM_XTRA_TIMER_ID_AUTO_DLOAD,
                                        (uint32)THREAD_ID_SM_TM );
  }
  if(xtra_state_ptr->retry_dload_timer == NULL)
  {
    xtra_state_ptr->retry_dload_timer =
        os_TimerCreateInCgpsTimerGroup( TM_XTRA_TIMER_ID_DLOAD_REQ_RETRY,
                                        (uint32)THREAD_ID_SM_TM );
  }

  /* Start the Auto download timer */
  if(xtra_state_ptr->auto_download_enabled)
  {
    /* Start the timer */
    (void)os_TimerStart(xtra_state_ptr->auto_dload_timer,
                        TM_XTRA_AUTO_DLOAD_CHECK_TIME, 
                        0);
  }

  if(NULL == xtra_state_ptr->xtra_avail_request_timer)
  {
    xtra_state_ptr->xtra_avail_request_timer =
        os_TimerCreateInCgpsTimerGroup( TM_XTRA_TIMER_ID_AVAIL_REQUEST,
                                        (uint32)THREAD_ID_SM_TM );
  }


  xtra_state_ptr->xtra_efs_read_timer =
        os_TimerCreateInCgpsTimerGroup( TM_XTRA_TIMER_ID_EFS_READ,
                                        (uint32)THREAD_ID_SM_TM );
  /* Start the timer */
  (void)os_TimerStart(xtra_state_ptr->xtra_efs_read_timer,
                      TM_XTRA_EFS_READ_DELAY_TIME_MS, 
                      0);

  xtra_state_ptr->b_xtraint_saved = FALSE;

  xtra_state_ptr->b_xtra_process_avail_req = FALSE;

  xtra_state_ptr->q_xtra_decoded_mask = 0;

  xtra_state_ptr->b_xtradata_saved = FALSE;

  memset(&xtra_state_ptr->z_xtraint_struct, 0, sizeof(xtra_state_ptr->z_xtraint_struct));

} /* End tm_xtra_init()*/

/*===========================================================================
FUNCTION tm_xtra_reset_data_buffer

DESCRIPTION
This functions reset XTRA data download buffer if SET_DATA command fails.
It'll reset XTRA's state to IDLE as well

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
void tm_xtra_reset_data_buffer(void)
{
  tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();

  if (xtra_state_ptr == NULL)
  {
	MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
	return;
  }

  /*----------------------------------------------------------------------
     Clear the variables used to load data from XTRA.BIN
  ----------------------------------------------------------------------*/  
  /* Memory buffer to store XTRA.BIN data from XTRA Client */
#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
  if (xtra_state_ptr->xtraTempBuf != NULL)
  {
  os_MemFree((void**)&xtra_state_ptr->xtraTempBuf);
  xtra_state_ptr->xtraTempBuf = NULL;
  }
#else
  memset((void *)xtra_state_ptr->xtraTempBuf, 
         0x0,
         TM_XTRA_MAX_FILE_SIZE);
#endif

  /* Used to manage data read into XtraTempBuf */
  xtra_state_ptr->q_XtraDataLen = 0;

  /*----------------------------------------------------------------------
     Initialize XTRA Session state
  ----------------------------------------------------------------------*/  
  xtra_state_ptr->e_state = TM_XTRA_STATE_IDLE;
}

/*===========================================================================
FUNCTION tm_xtra_abort_and_recover

DESCRIPTION
This is the cleanup function for TM-XTRA module.  It will initialize the 
current state and variables.  Then triggers a status event to the client, if
the end_status is valid

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
void tm_xtra_abort_and_recover
(
  pdsm_lite_xtra_status_e_type end_status
)
{
  /* Local Variables */
  tm_xtra_event_payload_u_type  status_event_payload;
  
  tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();

  TM_XTRA_MSG_ERROR("Abort and Recover (end_status=%d)",end_status,0,0);

  if(xtra_state_ptr == NULL)
  {
	MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
	return;
  }

  /*----------------------------------------------------------------------
     Clear the variables used to load data from XTRA.BIN
  ----------------------------------------------------------------------*/  
  /* continue blocking flag is set in in case XTRA file was rejected because of stale content */
  if (xtra_state_ptr->continue_blocking == FALSE)
  { 
     tm_xtra_req_allow();
  }
  else
  {
     xtra_state_ptr->continue_blocking = FALSE;
  }
  
  /* Memory buffer to store XTRA.BIN data from XTRA Client */
#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
  if (xtra_state_ptr->xtraTempBuf != NULL)
  {
  os_MemFree((void**)&xtra_state_ptr->xtraTempBuf);
  xtra_state_ptr->xtraTempBuf = NULL;
  }
#else
  memset((void *)xtra_state_ptr->xtraTempBuf, 
         0x0,
         TM_XTRA_MAX_FILE_SIZE);
#endif

  /* Used to manage data read into XtraTempBuf */
  xtra_state_ptr->q_XtraDataLen = 0;

  /*----------------------------------------------------------------------
     Initialize XTRA Session state
  ----------------------------------------------------------------------*/  
  xtra_state_ptr->e_state = TM_XTRA_STATE_IDLE;

  /* Number of dload request retry attempts */
  xtra_state_ptr->retry_attempts =  TM_XTRA_INIT_RETRY_ATTEMPTS;

  /* Source of Time Information request */
  xtra_state_ptr->time_info_req_src = GPS_TIME_CHECK_SOURCE_NONE;

  /*----------------------------------------------------------------------
     Stop Timers
  ----------------------------------------------------------------------*/  
  (void)os_TimerStop(xtra_state_ptr->auto_dload_timer);
  (void)os_TimerStop(xtra_state_ptr->retry_dload_timer);
  
  /*----------------------------------------------------------------------
     Send the Status event to the XTRA clients
  ----------------------------------------------------------------------*/
  if( end_status != XTRA_DOWNLOAD_STATUS_MAX )
  {
    status_event_payload.status_payload.status = end_status;
    
    (void)tm_xtra_trigger_event(TM_XTRA_EVENT_TYPE_STATUS, status_event_payload);

    /* Send gpsOneXTRA dload DM event */
    sm_report_event_xtra_end_dload(end_status);
  }

  /* let's restart auto-download timer if enabled */
    /* Start the Auto download timer */
  if(xtra_state_ptr->auto_download_enabled && xtra_state_ptr->auto_dload_timer)
  {
    /* Start the timer */
    (void)os_TimerStart(xtra_state_ptr->auto_dload_timer,
                        TM_XTRA_AUTO_DLOAD_CHECK_TIME, 
                        0);
  }
#ifdef FEATURE_CGPS_XTRA_T
  /* will not trigger XTRA-T if XTRA is triggered by AGPS(SUPL/V2) */
  if (tm_core_info.session_info.req_xtra_trig_by_agps == FALSE)
  {
    /* Let TLE know a XTRA download session completes. TLE will trigger
       a XTRA-T upload/download session.
     */
    (void) tle_RequestSessionByXtra(tm_core_is_standalone_active());
  }
  else
  {
    /* reset the "XTRA triggered by AGPS" flag */
    tm_core_info.session_info.req_xtra_trig_by_agps = FALSE;
  }
#endif  
  return;  
} /* End tm_xtra_abort_and_recover()*/


/*===========================================================================
FUNCTION tm_xtra_translate_end_status

DESCRIPTION
Handle the abort and recover request from TM-Core.  Only certain abort reasons
should end TM-XTRA module processing.  For example, E911 call origination
or TM-Core task stop.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_lite_xtra_status_e_type tm_xtra_translate_end_status
(
  const tm_sess_stop_reason_e_type reason
)
{
  /* Local Variables */
  pdsm_lite_xtra_status_e_type ret_val = XTRA_DOWNLOAD_STATUS_MAX;

  /* Translate the TM-Core stop reason to an XTRA stop reason */
  switch(reason)
  {
    case TM_STOP_REASON_TASK_OFFLINE:
    case TM_STOP_REASON_TASK_STOP:
      ret_val = XTRA_DOWNLOAD_STATUS_FAILURE;
      break;

    default:
      TM_XTRA_MSG_MED("TM-Core stop reason (%d), not valid for XTRA", reason,0,0);
      break;  
  }

  return ret_val;
} /* End tm_xtra_translate_end_status() */


/*===========================================================================
FUNCTION tm_xtra_handle_core_abort_recover

DESCRIPTION
Handle the abort and recover request from TM-Core.  Only certain abort reasons
should end TM-XTRA module processing.  For example, E911 call origination
or TM-Core task stop.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
void tm_xtra_handle_core_abort_recover
(
  const tm_sess_stop_reason_e_type abort_reason
)
{
  /* Local Variables */
  pdsm_lite_xtra_status_e_type xtra_end_status = XTRA_DOWNLOAD_STATUS_MAX;

  /* Translate the abort reason from TM-Core */
  xtra_end_status = tm_xtra_translate_end_status(abort_reason);

  /* Abort the XTRA session, only if this is a valid XTRA end reason */
  if(xtra_end_status != XTRA_DOWNLOAD_STATUS_MAX)
  {
    tm_xtra_abort_and_recover(xtra_end_status);
  }

  return;
} /* End tm_xtra_handle_core_abort_recover() */


/*===========================================================================
FUNCTION tm_xtra_is_enable

DESCRIPTION
Check if the XTRA feature is enabled

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_is_enable
(
  void
)
{
  /* Local Variable */
  tm_xtra_state_type *xtra_info_ptr = tm_xtra_get_state_ptr();
  
  if(xtra_info_ptr == NULL)
  {
    return FALSE;
  }
  
  return xtra_info_ptr->xtra_enabled;
} /* End tm_xtra_is_enable()*/

/*===========================================================================
FUNCTION tm_xtra_is_time_info_enable

DESCRIPTION
Check if the XTRA time info feature is enabled

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_is_time_info_enable
(
  void
)
{
  /* Local Variable */
  tm_xtra_state_type *xtra_info_ptr = tm_xtra_get_state_ptr();
  
  if(xtra_info_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
    return FALSE;
  }
  
  return xtra_info_ptr->time_info_enabled;
} /* End tm_xtra_is_time_info_enable()*/


/*===========================================================================

FUNCTION   tm_xtra_update_sess_state

DESCRIPTION
  This function is used by TM-XTRA module to update the current session state.

RETURN VALUE
  TRUE - State transition is successful
  FALSE - otherwise
============================================================================*/
boolean tm_xtra_update_sess_state
(
  tm_xtra_state_e_type new_state
)
{
  /* Local Variables */
  boolean                      state_tran = FALSE;
  tm_xtra_state_e_type         curr_state = TM_XTRA_STATE_IDLE;
  
  tm_xtra_state_type           *xtra_state_ptr =  tm_xtra_get_state_ptr();

  if(xtra_state_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
    return FALSE;
  }

  /*------------------------------------------------------------------------
     Check the TM-JGPS Core session state transition
  -------------------------------------------------------------------------*/
 
  /* Current Session State */
  curr_state = xtra_state_ptr->e_state;
  
  if( curr_state == new_state )
  {
    TM_XTRA_MSG_MED("current state [%d], is same as new state [%d]",
                    curr_state, 
                    new_state,
                    0 );
    return TRUE;
  }
  
  /* Determine if the session state can be modified*/
  switch(new_state)
  {  
    case TM_XTRA_STATE_IDLE:
      if(   curr_state == TM_XTRA_STATE_CLIENT_REQ
         || curr_state == TM_XTRA_STATE_WAIT_DATA
         || curr_state == TM_XTRA_STATE_INJECT_DATA )
      {
        state_tran = TRUE;
#ifdef FEATURE_CGPS_XTRA_T
        TM_XTRA_MSG_HIGH("Notify TLE XTRA Session Stop",0,0,0);
        tle_XTRAEventNotify(TLE_EVENT_XTRA_STOP);
#endif
      }
      break;
    case TM_XTRA_STATE_CLIENT_REQ:
      if( curr_state == TM_XTRA_STATE_IDLE )
      {
        state_tran = TRUE;
#ifdef FEATURE_CGPS_XTRA_T
        TM_XTRA_MSG_HIGH("Notify TLE XTRA Session Start",0,0,0);
        tle_XTRAEventNotify(TLE_EVENT_XTRA_START);
#endif
      }
      break;
    case TM_XTRA_STATE_WAIT_DATA:
      if( curr_state == TM_XTRA_STATE_CLIENT_REQ )
      {
        state_tran = TRUE;
      }
      break;
    case TM_XTRA_STATE_INJECT_DATA:
      if( curr_state == TM_XTRA_STATE_WAIT_DATA ||
          curr_state == TM_XTRA_STATE_CLIENT_REQ )
      {
        state_tran = TRUE;
      }
      break;
  
    default:
      state_tran = FALSE;
      break;    
  }

  /*------------------------------------------------------------------------
     Update the TM-JGPS Core session state
  -------------------------------------------------------------------------*/
  if(state_tran)
  {
    xtra_state_ptr->e_state = new_state;
    TM_XTRA_MSG_HIGH("state transition [%d] -> [%d]",
                    curr_state, 
                    xtra_state_ptr->e_state,
                    0 );
  }
  else
  {
    TM_XTRA_MSG_HIGH( "state transition failed cur state [%d], req state [%d]",
                      curr_state, 
                      new_state,
                      0 );
  }
  
  return state_tran;

} /* tm_xtra_update_sess_state */


/*===========================================================================
FUNCTION tm_xtra_is_download_in_progress

DESCRIPTION
Determine if the current download is in progress

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_is_download_in_progress
(
  void
)
{
  /* Local Variables */
  boolean             ret_val = FALSE;
  tm_xtra_state_type  *xtra_state_ptr =  tm_xtra_get_state_ptr();

  if(xtra_state_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
    return FALSE;
  }

  if(    xtra_state_ptr->e_state == TM_XTRA_STATE_WAIT_DATA
      || xtra_state_ptr->e_state == TM_XTRA_STATE_INJECT_DATA )
  {
    ret_val = TRUE;
  }

  return ret_val; 
} /* End tm_xtra_is_download_in_progress()*/


/*===========================================================================
FUNCTION tm_xtra_is_download_req_pending

DESCRIPTION
Determine if a download requst is pending

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_is_download_req_pending
(
  void
)
{
  /* Local Variables */
  boolean             ret_val = FALSE;
  tm_xtra_state_type  *xtra_state_ptr =  tm_xtra_get_state_ptr();

  if(xtra_state_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
    return FALSE;
  }

  if( xtra_state_ptr->e_state == TM_XTRA_STATE_CLIENT_REQ )
  {
    ret_val = TRUE;
  }

  return ret_val;
} /* End tm_xtra_is_download_req_pending()*/

/*===========================================================================

FUNCTION tm_xtra_handle_auto_dload_timer_expiry

DESCRIPTION
  This function handles the automatic download timer expiration. 
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_xtra_handle_auto_dload_timer_expiry
(
  void
)
{
   tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();
    
   if(xtra_state_ptr == NULL)
   {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
      return;
   }
  /* check if GNSS lock is enabled */
   if( (TM_CORE_CONFIG_DD(gps_lock) == PDSM_LITE_GPS_LOCK_MI)
      ||(TM_CORE_CONFIG_DD(gps_lock) == PDSM_LITE_GPS_LOCK_ALL))
   {
      uint32       q_IntervalMsec = 0;
      q_IntervalMsec = (uint32)TM_XTRA_CONVERT_HOURS_TO_MS(xtra_state_ptr->download_interval);
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Trigger auto Dnld timer(GPS lock) %dms ",q_IntervalMsec);
      /* Re-start the auto dload timer */
      (void)os_TimerStart(xtra_state_ptr->auto_dload_timer,
                          q_IntervalMsec, 
                          0);  
      return;
   }

    
   /*check if time of last download is non-zero (valid)
   If an XTRA file has never been downloaded, then don't do anything.
   The very first download should be done manually or during a positioning
   session before the automatic downloading functionality kicks in to 
   prevent the phone trying to retrieve XTRA data as soon as it is turned 
   on for the first time*/
  if(tm_xtra_data_valid())
  {
    (void)tm_xtra_data_validity_check( GPS_TIME_CHECK_SOURCE_PERIODIC );
  }
  else
  {
     /* Re-start the auto dload timer */
     (void)os_TimerStart(xtra_state_ptr->auto_dload_timer,
                         TM_XTRA_AUTO_DLOAD_CHECK_TIME, 
                         0);  
  }
} /* End tm_xtra_handle_auto_dload_timer_expiry()*/

/*===========================================================================

FUNCTION tm_xtra_handle_dload_req_retry_timer_expiry

DESCRIPTION
  This function handles the automatic download timer expiration. 
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_xtra_handle_dload_req_retry_timer_expiry
(
  void
)
{
  /* Local Variables */
  boolean                       ret_val = FALSE;
  tm_xtra_event_payload_u_type  event_payload;
  tm_xtra_state_type            *xtra_state_ptr =  tm_xtra_get_state_ptr();

  if(xtra_state_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
    return;
  }
  /* check if GNSS lock is enabled */
  if( (TM_CORE_CONFIG_DD(gps_lock) == PDSM_LITE_GPS_LOCK_MI)
      ||(TM_CORE_CONFIG_DD(gps_lock) == PDSM_LITE_GPS_LOCK_ALL))
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA retry but GPS is locked");
    tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_GPS_LOCK);
    return;
  }

  /* Update the retry attempts counter */
  xtra_state_ptr->retry_attempts++;

  /* If the download attempt exceeds some threshold terminate the session */
  if(xtra_state_ptr->retry_attempts >= xtra_state_ptr->num_download_attempts)
  {
    tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_RETRY_EXCEEDED);
  }
  else
  {
    /*--------------------------------------------------------------------
       Trigger the download request processing
    --------------------------------------------------------------------*/
    TM_XTRA_MSG_LOW("Processing initiate_xtra_download command. ",0,0,0);
  
    event_payload.data_dload_payload.force_data_dload = TRUE;
      
    ret_val = tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_DATA_DOWNLOAD, 
                                     event_payload );
      
    /*--------------------------------------------------------------------
       Based on the trigger event and current state determine if timer 
       should be re-started.
    --------------------------------------------------------------------*/
    if(ret_val)
    {
      /* Send gpsOneXTRA dload DM event */
      sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_RETRY);
      
      if( tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ) )
      {
        /* Start the recover timer */
        (void)os_TimerStart(xtra_state_ptr->retry_dload_timer,
                            (xtra_state_ptr->time_between_attempts*TM_XTRA_MSEC_IN_MIN), 
                            0);
      }
    }
    else if ( tm_xtra_is_download_in_progress() 
             || tm_xtra_is_download_req_pending() )
    {
      TM_XTRA_MSG_MED("Download currently in progress or pending - restart dload retry timer (state=%d)",
                       xtra_state_ptr->e_state,0,0);

      /* Start the recover timer */
      (void)os_TimerStart(xtra_state_ptr->retry_dload_timer,
                          (xtra_state_ptr->time_between_attempts*TM_XTRA_MSEC_IN_MIN), 
                          0);
    }
    else
    {
      /* Abort and recover the session */
      tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_FAILURE);
    } /* end else */
  } /* end else */

  return;
} /* End tm_xtra_handle_dload_req_retry_timer_expiry()*/



/*===========================================================================

FUNCTION tm_xtra_handle_avail_request_timer_expiry

DESCRIPTION
  This function handles the XTRA avail request timer expiry
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_xtra_handle_avail_request_timer_expiry(void)
{
  /* The timer for downloading of XTRA from HLOS has expired. */
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  TM_XTRA_MSG_ERROR("tm_xtra_handle_avail_request_timer_expiry",0,0,0);
  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/  
  if(xtra_state_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
	return;
  }

  if(TRUE == xtra_state_ptr->b_xtra_process_avail_req)
  {
    tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_FAILURE);

    /* Inform SM client that the XTRA download failed */
    xtra_state_ptr->b_xtra_process_avail_req = FALSE;
    
    TM_XTRA_MSG_HIGH("XTRA validity check bypassed = %d and ntp_download needed = %d", b_bypass_xtra_validity_check, b_ntp_download_needed,0);
    /* Load MGP: IF  xtra validity check is bypassed (74231==1) AND 
       NTP download is not needed (NTP already downloaded in last 3 hours or tunc NV bypassed "NV 74194 == 1") */
    if((TRUE == b_bypass_xtra_validity_check) && ((FALSE == b_ntp_download_needed) || (tm_core_info.config_info.bypass_gnss_aiding_check & TM_CORE_GTS_SYS_TUNC_DL_FAILURE_ALLOW_SESS)))
    {
      tm_xtra_process_avail_request_done(TRUE);
    }
    else 
    {
      tm_xtra_process_avail_request_done(FALSE);
    }
  }

}


/*===========================================================================

FUNCTION tm_xtra_handle_timer_expiry

DESCRIPTION
  This function is called by TM-core to inform XTRA module about timer
  expiry event.
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_xtra_handle_timer_expiry(const os_TimerExpiryType *timer_data)
{
  os_TimerExpiryType *timer_param = (os_TimerExpiryType *)timer_data;
  
  if(timer_param != NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"timerId 0x%x expired", timer_param->q_TimerId, 0, 0);
    
    switch(timer_param->q_TimerId)
    {
      case TM_XTRA_TIMER_ID_AUTO_DLOAD:
        tm_xtra_handle_auto_dload_timer_expiry();
      break;
      
      case TM_XTRA_TIMER_ID_DLOAD_REQ_RETRY:
        tm_xtra_handle_dload_req_retry_timer_expiry();
      break;
      
     case TM_XTRA_TIMER_ID_AVAIL_REQUEST:
        tm_xtra_handle_avail_request_timer_expiry();
		break;
     case TM_XTRA_TIMER_ID_EFS_READ:
        tm_xtra_efs_read();
		break;

      break;
      default:
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Uknown timer expired", 0, 0, 0);
      break;
    }
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Can not proess null timer msg", 0, 0, 0);  
  }
} /* End tm_xtra_handle_timer_expiry() */


/*===========================================================================
FUNCTION tm_xtra_randomize_and_set_urls

DESCRIPTION
Randomize order of input URLs and output.  This is for load-balancing 
purposes to aid XTRA clients accessing different XTRA servers.  
(Assumption that XTRA clients do NOT perform internal load-balancing
techniques)


INPUT PARAMS
U8 **InUrl_ptr

OUTPUT PARAMS
U8 **OutUrl_ptr

RETURN VALUE 
void
===========================================================================*/
void tm_xtra_randomize_and_set_urls
(
  uint8 **InUrl_ptr,
  uint8 **OutUrl_ptr
)
{
  /* Local Variables */
  uint8 i, k, indOut = 0;
  uint8 initUrl = 0;

  qword current_time;

  /* Check for null pointers */
  if ((InUrl_ptr == NULL) || (OutUrl_ptr == NULL))
  {
    TM_XTRA_MSG_HIGH("Received NULL pointer parameter %lx %lx",
                     InUrl_ptr,OutUrl_ptr,0);
    return;
  }

  /* Get current system time
  */
  (void) time_get(current_time);
  
  /* Generate a random integer btwn 0 and 2 */
  initUrl = (uint8)(current_time[0] % TM_XTRA_MAX_NUMBER_URLS);

  for ( k = 0; k < TM_XTRA_MAX_NUMBER_URLS; k++ )
  {
    if ( InUrl_ptr[initUrl] != NULL && InUrl_ptr[initUrl][0] != '\0' )
    {
      for (i = 0; i < TM_XTRA_MAX_SERVER_URL_SIZE; i++)
      {
        OutUrl_ptr[indOut][i] = InUrl_ptr[initUrl][i];

        if ( InUrl_ptr[initUrl][i] == '\0' )
          break;
      }

      indOut++;
    }

    initUrl++;
    initUrl %= TM_XTRA_MAX_NUMBER_URLS;
  }

  /* Fill in remaining output URLs with NULL terminated strings */
  for ( k = indOut; k < TM_XTRA_MAX_NUMBER_URLS; k++ )
  {
    OutUrl_ptr[k][0] = '\0';
  }

  return;
} /* End tm_xtra_randomize_and_set_urls()*/


/*===========================================================================
=============================================================================

   TM-XTRA: PD API Requst Check and Process 
   
=============================================================================
===========================================================================*/

/*===========================================================================
FUNCTION tm_xtra_check_set_data_para

DESCRIPTION
Check the input parameters for XTRA set data client request.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_lite_xtra_cmd_err_e_type tm_xtra_check_set_data_para
(
  const pdsm_lite_xtra_cmd_s_type *req_ptr
)
{
  /* Local Variables */
  pdsm_lite_xtra_cmd_err_e_type  xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_NOERR;

   
  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
  if (req_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("null ptr", 0, 0, 0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_INVALID_PARAM;
  }     
  /*----------------------------------------------------------------------
      Determine if NV item enables XTRA feature
  ----------------------------------------------------------------------*/
  else if ( FALSE == tm_xtra_is_enable() )
  {
    TM_XTRA_MSG_ERROR("XTRA: feature is disabled.",0,0,0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_FEATURE_NOT_ENABLED;
  }   
  /*-----------------------------------------------------------------------
     Check if exceeding max local buffer size 
  -----------------------------------------------------------------------*/
  else if (req_ptr->info.xtra_data_len > TM_XTRA_MAX_FILE_SIZE)
  {
    TM_XTRA_MSG_ERROR("Exceeding max buffer size %ld", 
                      req_ptr->info.xtra_data_len, 0, 0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_NO_BUF_L;
  }
  /*-----------------------------------------------------------------------
     Check the xtra data ptr
  -----------------------------------------------------------------------*/
  else if (req_ptr->info.xtra_data_ptr == NULL )
  {
    TM_XTRA_MSG_ERROR("NULL ptr from XTRA client", 0, 0, 0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_INVALID_PARAM;
  }
  /*-----------------------------------------------------------------------
     Verify the part number
  -----------------------------------------------------------------------*/
  else if (req_ptr->info.part_number > req_ptr->info.total_parts)
  {
    TM_XTRA_MSG_ERROR("part_number %d > total_parts %d",
                      req_ptr->info.part_number, 
                      req_ptr->info.total_parts, 0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_INVALID_PARAM;
  }
  /*-----------------------------------------------------------------------
     Check the XTRA client Download status
  -----------------------------------------------------------------------*/
  else if( (req_ptr->info.part_number < req_ptr->info.total_parts )
     && (PDSM_LITE_XTRA_STATUS_FAILURE == req_ptr->info.xtra_dc_status) )
  {
    TM_XTRA_MSG_ERROR("Failed to Recv complete XTRA file "
                      "(part num %d < total %d, status: %d",
                      req_ptr->info.part_number, 
                      req_ptr->info.total_parts, 
                      req_ptr->info.xtra_dc_status);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_INVALID_PARAM;
  }
  /*----------------------------------------------------------------------
     If assisted (MS-B dload, MS-A) mode operation is ongoing, block
     client request
  ----------------------------------------------------------------------*/
  else if ( FALSE == tm_core_is_xtra_operation_allowed() )
  {
    TM_XTRA_MSG_ERROR("XTRA: Assisted GPS session in progress",0,0,0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_GPS_SUBSYSTEM_BUSY;
  }
  
  return xtra_cmd_err;

} /* End tm_xtra_check_set_data_para() */


/*===========================================================================
FUNCTION tm_xtra_check_initiate_download_para

DESCRIPTION
Check the input parameters for XTRA check init download parameters.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_lite_xtra_cmd_err_e_type tm_xtra_check_initiate_download_para
(
  const pdsm_lite_xtra_cmd_s_type *req_ptr
)
{
  /* Local Varaibles*/
  pdsm_lite_xtra_cmd_err_e_type  xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_NOERR;
  
  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
  if (req_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("null ptr", 0, 0, 0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_INVALID_PARAM;
  }
  /*----------------------------------------------------------------------
     Determine if NV item enables XTRA feature
  ----------------------------------------------------------------------*/
  else if ( FALSE == tm_xtra_is_enable() )
  {
    TM_XTRA_MSG_ERROR("XTRA: feature is disabled.",0,0,0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_FEATURE_NOT_ENABLED;
  } 
  /*----------------------------------------------------------------------
     If assisted (MS-B dload, MS-A) mode operation is ongoing, block
     client request
  ----------------------------------------------------------------------*/
  else if ( FALSE == tm_core_is_xtra_operation_allowed() )
  {
    TM_XTRA_MSG_ERROR("XTRA: Assisted GPS session in progress",0,0,0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_GPS_SUBSYSTEM_BUSY;
  } 

  return xtra_cmd_err;
} /* End tm_xtra_check_initiate_download_para() */


/*===========================================================================
FUNCTION tm_xtra_check_auto_download_para

DESCRIPTION
Check the input parameters for XTRA check auto download parameters.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_lite_xtra_cmd_err_e_type tm_xtra_check_auto_download_para
(
  const pdsm_lite_xtra_cmd_s_type *req_ptr
)
{
  /* Local Varaibles*/
  pdsm_lite_xtra_cmd_err_e_type  xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_NOERR;

  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
  if (req_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("null ptr", 0, 0, 0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_INVALID_PARAM;
  }
  /*----------------------------------------------------------------------
     Determine if NV item enables XTRA feature
  ----------------------------------------------------------------------*/
  else if ( FALSE == tm_xtra_is_enable() )
  {
    TM_XTRA_MSG_ERROR("XTRA: feature is disabled.",0,0,0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_FEATURE_NOT_ENABLED;
  } 
  /*-----------------------------------------------------------------------
     Check Auto Download interval
  -----------------------------------------------------------------------*/
  else if (   (req_ptr->info.download_interval < TM_XTRA_MIN_DOWNLOAD_INTERVAL) 
           || (req_ptr->info.download_interval > TM_XTRA_MAX_DOWNLOAD_INTERVAL) )
  {
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_DOWNLOAD_INTRVL_OUT_OF_RANGE;
  }


  return xtra_cmd_err;

} /* End tm_xtra_check_auto_download_para() */

/*===========================================================================
FUNCTION tm_xtra_check_query_data_validity_para

DESCRIPTION
Check the input parameters for XTRA check data validy request parameters.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_lite_xtra_cmd_err_e_type tm_xtra_check_query_data_validity_para
(
  const pdsm_lite_xtra_cmd_s_type *req_ptr
)
{
  /* Local Varaibles*/
  pdsm_lite_xtra_cmd_err_e_type  xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_NOERR;
  
  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
  if (req_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("null ptr", 0, 0, 0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_INVALID_PARAM;
  }
  /*----------------------------------------------------------------------
     Determine if NV item enables XTRA feature
  ----------------------------------------------------------------------*/
  else if ( FALSE == tm_xtra_is_enable() )
  {
    TM_XTRA_MSG_ERROR("XTRA: feature is disabled.",0,0,0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_FEATURE_NOT_ENABLED;
  } 

  return xtra_cmd_err;
} /* End tm_xtra_check_query_data_validity_para() */

/*===========================================================================
FUNCTION tm_xtra_copy_data

DESCRIPTION
This function is called by PD API at client's context. Be sure only to perform
simple operations in this function.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_lite_xtra_cmd_err_e_type tm_xtra_copy_data(uint8 part_num, byte *xtra_part_ptr, uint32 xtra_part_length)
{
  /* Call this macro to obtain gpsOneXTRA tag pointer */
  tm_xtra_state_type *p_xtra_state_info = tm_xtra_get_state_ptr();

  if(p_xtra_state_info == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
    return PDSM_LITE_XTRA_CMD_ERR_INVALID_PARAM;
  }

  // check XTRA data pointer
  if(xtra_part_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("Received NULL XTRA data pointer in set_data",0,0,0);
    return PDSM_LITE_XTRA_CMD_ERR_INVALID_PARAM;
  }
  
  if (part_num == 1 && p_xtra_state_info->q_XtraDataLen != 0)
  {
    TM_XTRA_MSG_ERROR("Received XTRA download 1st part while buffer not empty!",0,0,0);
    p_xtra_state_info->q_XtraDataLen = 0; //Reset to 0, recovery from bad case
  }

  // check if we've exceeded the local buffer size
  if ( (p_xtra_state_info->q_XtraDataLen + xtra_part_length)
                                                > TM_XTRA_MAX_FILE_SIZE )
  {
    TM_XTRA_MSG_ERROR("Total data %d exceeds max allowed size %d",
      p_xtra_state_info->q_XtraDataLen,
      TM_XTRA_MAX_FILE_SIZE,0);
    /*Since this function doesn't have any recovery mechanism, we need to somehow indicate TM_XTRA
      that a buffer space issue has occured and session abort is needed. We set the XTRADataLen to 
      the MAX file size +1 so that when tm_xtra_process_set_data_cmd() is called, it will abort the
      session and recover*/
    p_xtra_state_info->q_XtraDataLen = TM_XTRA_MAX_FILE_SIZE + 1;

    return PDSM_LITE_XTRA_CMD_ERR_NOERR;
  }

#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
  /* Only allocate if this is first call to copy data */
  if (p_xtra_state_info->xtraTempBuf == NULL)
  {
    if (part_num == 1 && p_xtra_state_info->q_XtraDataLen == 0)
    {
      /*  Allocate temporary buffer for gpsOneXTRA data */
      p_xtra_state_info->xtraTempBuf = (uint8*)os_MemAlloc(TM_XTRA_MAX_FILE_SIZE, OS_MEM_SCOPE_TASK);
      
      if (p_xtra_state_info->xtraTempBuf == NULL)
      {
        TM_XTRA_MSG_ERROR("os_MemAlloc failed - out of memory",0,0,0);
        return PDSM_LITE_XTRA_CMD_ERR_NO_BUF_L;
      }
    }
    else
    {
      TM_XTRA_MSG_ERROR("Unexpected part # (%d) or data length (%d)",
        part_num, p_xtra_state_info->q_XtraDataLen,0);
      return PDSM_LITE_XTRA_CMD_ERR_NO_BUF_L;
    }
  }
#endif

  // copy the client data into the xtra buffer
  memscpy( p_xtra_state_info->xtraTempBuf + p_xtra_state_info->q_XtraDataLen, 
    xtra_part_length, xtra_part_ptr, xtra_part_length);

  // total amount of data received from client thus far
  p_xtra_state_info->q_XtraDataLen += xtra_part_length;   

  return PDSM_LITE_XTRA_CMD_ERR_NOERR;
}

/*===========================================================================
FUNCTION tm_xtra_check_query_data_validity_para

DESCRIPTION
Check the input parameters for XTRA check data validy request parameters.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_lite_xtra_cmd_err_e_type tm_xtra_check_time_info_inject_para
(
  const pdsm_lite_xtra_cmd_s_type *req_ptr
)
{
  /* Local Varaibles*/
  pdsm_lite_xtra_cmd_err_e_type  xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_NOERR;
  
  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
  if (req_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("null ptr", 0, 0, 0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_INVALID_PARAM;
  }
  /*----------------------------------------------------------------------
     Determine if NV item enables XTRA feature
  ----------------------------------------------------------------------*/
  else if ( FALSE == tm_xtra_is_time_info_enable() )
  {
    TM_XTRA_MSG_ERROR("XTRA: feature xtra timeinfo is disabled.",0,0,0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_FEATURE_NOT_ENABLED;
  }

  return xtra_cmd_err;
} /* End tm_xtra_check_time_info_inject_para() */

/*===========================================================================
FUNCTION tm_xtra_check_xtra_config_info_para

DESCRIPTION
Check the input parameters for XTRA query Xtra config info request .

INPUT PARAMS
pdsm_lite_xtra_cmd_s_type *req_ptr : input xtra request pointer

OUTPUT PARAMS
None

RETURN VALUE
XTRA command status
===========================================================================*/
pdsm_lite_xtra_cmd_err_e_type tm_xtra_check_xtra_config_info_para
(
  const pdsm_lite_xtra_cmd_s_type *req_ptr
)
{
  /* Local Varaibles*/
  pdsm_lite_xtra_cmd_err_e_type  xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_INVALID_PARAM;
  
  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
  if (req_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("null ptr", 0, 0, 0);
  }
  /*----------------------------------------------------------------------
     Determine if NV item enables XTRA feature
  ----------------------------------------------------------------------*/

  else if ( FALSE == tm_xtra_is_enable() )
  {
    TM_XTRA_MSG_ERROR("XTRA: feature is disabled.",0,0,0);
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_FEATURE_NOT_ENABLED;
  }
  else
  {
    xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_NOERR;
  }

  return xtra_cmd_err;
} /* End tm_xtra_check_xtra_config_info_para() */

/*===========================================================================
FUNCTION tm_xtra_process_set_data_cmd

DESCRIPTION
Process the set data command request from XTRA client

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_process_set_data_cmd
(
  pdsm_lite_xtra_cmd_s_type *cmd_ptr
)
{
   /* Local Variables */
   boolean                    ret_val = FALSE;
   tm_xtra_event_payload_u_type  event_payload;
   qword d_CurrentTime = {0}, d_TimeDiff = {0}, d_TimeDiffMs = {0};

   /* Call this macro to obtain gpsOneXTRA tag pointer */
   tm_xtra_state_type *p_xtra_state_info = tm_xtra_get_state_ptr();

   if(p_xtra_state_info == NULL)
   {
     MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  

     event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;    
     (void)tm_xtra_trigger_event(TM_XTRA_EVENT_TYPE_STATUS, event_payload);

     return FALSE;
   }

  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
   if (cmd_ptr == NULL)
   {
     TM_XTRA_MSG_ERROR("Null cmd_ptr",0,0,0);

     event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;    
     (void)tm_xtra_trigger_event(TM_XTRA_EVENT_TYPE_STATUS, event_payload);

     return FALSE;
   }
   if (cmd_ptr->info.xtra_data_ptr != NULL)
   {
     TM_XTRA_MSG_HIGH("tm_xtra_process_set_data_cmd: part_num =%d, total_parts= %d, data_len= %d ",
                                    cmd_ptr->info.part_number,cmd_ptr->info.total_parts,cmd_ptr->info.xtra_data_len);
     TM_XTRA_MSG_HIGH("tm_xtra_process_set_data_cmd: xtra_data_ptr[0] =%d, xtra_data_ptr[1]= %d",
                                    cmd_ptr->info.xtra_data_ptr[0],cmd_ptr->info.xtra_data_ptr[1],0);
        
     if ( (1 == cmd_ptr->info.part_number) && 
          (1 == cmd_ptr->info.total_parts) && 
          (1 == cmd_ptr->info.xtra_data_len ) &&
          (1 == cmd_ptr->info.xtra_data_ptr[0])
        )
     {
     
       TM_XTRA_MSG_HIGH("XTRA validity check bypassed = %d and ntp_download needed = %d", b_bypass_xtra_validity_check, b_ntp_download_needed,0);
         
       /* If we are here that means Xtra download is failed and HLOS has sent dummy XTRA message indicating XTRA download failire.
          XTRA avail request timer can be stopped and session can start if XTRA validity check bypass is allowed from NV
          Handle XTRA failure only when NTP phase is already completed or not required
        */
       if((TRUE == p_xtra_state_info->b_xtra_process_avail_req) && (FALSE == b_ntp_download_needed))
       {
         tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_FAILURE);
     
         /* Stop the xtra avail request timer if running */
         os_TimerStop(p_xtra_state_info->xtra_avail_request_timer);
         
         p_xtra_state_info->b_xtra_process_avail_req = FALSE;
         
         
         /* Load MGP: IF  xtra validity check is bypassed (74231==1) AND 
                   NTP download is not needed (NTP already downloaded in last 3 hours or tunc NV bypassed "NV 74194 == 1") */
         if((TRUE == b_bypass_xtra_validity_check))
         {
           tm_xtra_process_avail_request_done(TRUE);
         }
         else 
         {
           tm_xtra_process_avail_request_done(FALSE);
         }
       } 
       return TRUE;
     }	 
   }


   /*-----------------------------------------------------------------------
      Process the incoming gpsOneXTRA data from client
   -----------------------------------------------------------------------*/
   do{
     /* Update the session state, to client request, if this is the first 
     ** XTRA data file part
     */
     if(p_xtra_state_info->e_state == TM_XTRA_STATE_IDLE)
     {
       /* Update the session state, IDLE->CLIENT_REQ should succeed */
       (void)tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ);
     }
     else if ( p_xtra_state_info->e_state == TM_XTRA_STATE_INJECT_DATA )
     {
       (void)time_get(d_CurrentTime);
       qw_sub(d_TimeDiff,d_CurrentTime,p_xtra_state_info->time_of_last_download);
       gps_total_msec(d_TimeDiff,d_TimeDiffMs);
       if ( (p_xtra_state_info->time_of_last_download > 0) && 
          ((d_TimeDiffMs[0]) > 120000) ) 
       {
         //allow xtra injection if its been more than 2 minutes since last 
         //time of download
         TM_XTRA_MSG_MED("tm_xtra_process_set_data_cmd: Xtra injection already going on. ",0,0,0);
         /* Clear the earlier xtra injection request status */
         /* Clear the time of last download */
         qw_set(p_xtra_state_info->time_of_last_download, 0, 0);
         (void)tm_xtra_update_sess_state(TM_XTRA_STATE_IDLE);
         /* Number of dload request retry attempts */
         p_xtra_state_info->retry_attempts =  TM_XTRA_INIT_RETRY_ATTEMPTS;

         /* Source of Time Information request */
         p_xtra_state_info->time_info_req_src = GPS_TIME_CHECK_SOURCE_NONE;

         /*----------------------------------------------------------------------
         Stop Timers
         ----------------------------------------------------------------------*/  
         (void)os_TimerStop(p_xtra_state_info->auto_dload_timer);
         (void)os_TimerStop(p_xtra_state_info->retry_dload_timer);
         if(p_xtra_state_info->auto_download_enabled 
            && p_xtra_state_info->auto_dload_timer)
         {
           /* Start the timer */
           (void)os_TimerStart(p_xtra_state_info->auto_dload_timer,
                        TM_XTRA_AUTO_DLOAD_CHECK_TIME, 
                        0);
         }
         if(!tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ))
         {
            TM_XTRA_MSG_ERROR("tm_xtra_process_set_data_cmd: Unable to change xtra inj state after time-out. ",0,0,0);
            tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_FAILURE);  // -> IDLE
            break;
         }
       }
       else
       {
         TM_XTRA_MSG_MED("Injecting data, process data rejected (state:%d)", p_xtra_state_info->e_state, 0, 0);
           event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_GPS_BUSY;    
           (void)tm_xtra_trigger_event(TM_XTRA_EVENT_TYPE_STATUS, event_payload);
         /* Send gpsOneXTRA dload DM event */
         sm_report_event_xtra_end_dload(XTRA_DOWNLOAD_STATUS_GPS_BUSY);
         break;
       }
     }
     
     TM_XTRA_MSG_HIGH( "Processing set_xtra_data command. status=%d, part=%d, total parts=%d",
                       cmd_ptr->info.xtra_dc_status,
                       cmd_ptr->info.part_number,
                       cmd_ptr->info.total_parts);
  
     // Clear the wait data timer
     (void)os_TimerStop(p_xtra_state_info->retry_dload_timer);  
  
     // make sure file was downloaded successfully
     if (cmd_ptr->info.xtra_dc_status != PDSM_LITE_XTRA_STATUS_SUCCESS)
     {
       TM_XTRA_MSG_MED(" ERROR - client cannot download data successfully. Abort ",0,0,0);
       tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_FAILURE);  // -> IDLE
       break;
     }
     
     // check XTRA data pointer
     if(cmd_ptr->info.xtra_data_ptr == NULL)
     {
       TM_XTRA_MSG_ERROR("Received NULL XTRA data pointer in set_data",0,0,0);
       tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_FAILURE);
       break;
     }

     // check if we've exceeded the local buffer size
     if ( p_xtra_state_info->q_XtraDataLen > TM_XTRA_MAX_FILE_SIZE )
     {
       TM_XTRA_MSG_ERROR("Total data %d exceeds max allowed size %d",
                          p_xtra_state_info->q_XtraDataLen,
                          TM_XTRA_MAX_FILE_SIZE,0);
       
       tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_BAD_BUFFER_LENGTH);
       break;
     }

     // if we have all of the parts, process the data now
     if (cmd_ptr->info.part_number == cmd_ptr->info.total_parts)
     {
       /* Update the session state, inject data state */
       if( !tm_xtra_update_sess_state(TM_XTRA_STATE_INJECT_DATA) )
       {
         tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_FAILURE);  // -> IDLE
         break;
       }
 
       /* Request GPS time, once GPS time is available process the XTRA data */
       ret_val = tm_xtra_data_validity_check(GPS_TIME_CHECK_SOURCE_DATA_INJECT);
       
     }
     else
     {
       /* Send status event to XTRA client */
       event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_SUCCESS;
    
       (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS, event_payload );
       
       /* Update the session state, wait for additional data */
       if( !tm_xtra_update_sess_state(TM_XTRA_STATE_WAIT_DATA) )
       {
         break;
       }
     }

     ret_val = TRUE;
   } while (0);

   return ret_val;
} /* End tm_xtra_process_set_data_cmd() */


/*===========================================================================
FUNCTION tm_xtra_process_initiate_download_cmd

DESCRIPTION
Process the initiate download command request from XTRA client

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_process_initiate_download_cmd
(
  pdsm_lite_xtra_cmd_s_type *cmd_ptr
)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  tm_xtra_event_payload_u_type  event_payload;

  tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();
  
  if(xtra_state_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  

    event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;
    
    (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS, 
                                 event_payload );

    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
      Check input parameters
  -----------------------------------------------------------------------*/
  if (cmd_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("Received NULL cmd_ptr pointer parameter",0,0,0);
    
    event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;
    
    ret_val = tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS, 
                                     event_payload );
  }
  else
  {
    /*--------------------------------------------------------------------
      Trigger the download request processing
    --------------------------------------------------------------------*/
    TM_XTRA_MSG_LOW("Processing initiate_xtra_download command. ",0,0,0);

    event_payload.data_dload_payload.force_data_dload = TRUE;
    
    ret_val = tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_DATA_DOWNLOAD, 
                                     event_payload );
    
    /* Update the session state */
    if(ret_val)
    {
      /* Send gpsOneXTRA dload DM event */
      sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_XTRA_CLIENT_REQ);

      if( tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ) )
      {
        /* Start the recover timer */
        (void)os_TimerStart(xtra_state_ptr->retry_dload_timer,
                            (xtra_state_ptr->time_between_attempts*TM_XTRA_MSEC_IN_MIN), 
                            0);
      }
    }
    
  } /* end else */

  return ret_val;

} /* End tm_xtra_process_initiate_download_cmd() */


/*===========================================================================
FUNCTION tm_xtra_process_set_auto_download_cmd

DESCRIPTION
Process the set auto download command request from XTRA client

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_process_set_auto_download_cmd
(
  pdsm_lite_xtra_cmd_s_type *cmd_ptr
)
{
   /* Local Variables */
   boolean                       ret_val = FALSE;
   tm_xtra_event_payload_u_type  event_payload;
   nv_item_type                  nv_write_buffer;

   tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();

   if(xtra_state_ptr == NULL)
   {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  

      event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;

      (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS, 
         event_payload );

      return FALSE;
   }

   /*-----------------------------------------------------------------------
   Check input parameters
   -----------------------------------------------------------------------*/
   if (cmd_ptr == NULL)
   {
      TM_XTRA_MSG_ERROR("Received NULL cmd_ptr pointer parameter",0,0,0);

      event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;

      ret_val = tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS, 
         event_payload );
   }
   else
   {
      /*--------------------------------------------------------------------
      Set the auto download parameters
      --------------------------------------------------------------------*/
      TM_XTRA_MSG_LOW("Processing set_auto_download command. ",0,0,0);

      /* set download interval */
      xtra_state_ptr->download_interval  
         = cmd_ptr->info.download_interval;

      nv_write_buffer.gps1_xtra_download_interval 
         = cmd_ptr->info.download_interval;

      (void)tm_core_nv_write( NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I, 
         (void*)&nv_write_buffer );

      /* enable/disable auto downloads */
      xtra_state_ptr->auto_download_enabled
         = cmd_ptr->info.enable_auto_download;

      nv_write_buffer.gps1_xtra_auto_download_enabled 
         = cmd_ptr->info.enable_auto_download;

      (void)tm_core_nv_write( NV_GPS1_XTRA_AUTO_DOWNLOAD_ENABLED_I, 
         (void*)&nv_write_buffer );


      /* Start the Auto download timer */
      if(xtra_state_ptr->auto_download_enabled)
      {
         /* Start the timer */
         (void)os_TimerStart(xtra_state_ptr->auto_dload_timer,
            TM_XTRA_AUTO_DLOAD_CHECK_TIME, 
            0);
      }
      else
      {
         /* Stop the timer */
         (void)os_TimerStop(xtra_state_ptr->auto_dload_timer);
      }

      event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_SUCCESS;

      (void) tm_xtra_trigger_event(TM_XTRA_EVENT_TYPE_STATUS, event_payload);
      ret_val =  TRUE;
   } /* end else */

   return ret_val;
} /* End tm_xtra_process_set_auto_download_cmd() */


/*===========================================================================
FUNCTION tm_xtra_process_query_data_validity_cmd

DESCRIPTION
Process the query data validity command request from XTRA client

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_process_query_data_validity_cmd
(
  pdsm_lite_xtra_cmd_s_type *cmd_ptr
)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  tm_xtra_event_payload_u_type  event_payload;

  /*-----------------------------------------------------------------------
      Check input parameters
  -----------------------------------------------------------------------*/
  if (cmd_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("Received NULL cmd_ptr pointer parameter",0,0,0);

    event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;

    ret_val = tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS, 
                                     event_payload );
  }
  else
  {
    /*--------------------------------------------------------------------
      Trigger the download request processing
    --------------------------------------------------------------------*/
    TM_XTRA_MSG_LOW("Processing query_data_validity command. ",0,0,0);

    event_payload.data_query_payload.status = XTRA_DOWNLOAD_STATUS_SUCCESS;
    
    ret_val = tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_QUERY_DATA_VALID, 
                                     event_payload );       
  } /* end else */

  return ret_val;    
} /* End tm_xtra_process_query_data_validity_cmd() */


/*===========================================================================
FUNCTION tm_xtra_process_inject_time_info

DESCRIPTION
Process the inject time info command request from XTRA client

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_process_inject_time_info
(
  pdsm_lite_xtra_cmd_s_type *cmd_ptr
)
{
  /* Local Variables */
  tm_xtra_event_payload_u_type  event_payload;
  lm_request_info_s_type    lm_info_req_param;
  pdsm_lite_xtra_cmd_info_s_type *p_xtra_info = NULL;
  uint16 w_GpsWeek;
  uint32 u_GpsMsec;
  boolean ret_val = FALSE;
  boolean b_is_data_old = FALSE;
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  tm_xtra_data_check_result_e_type e_data_check = TM_XTRA_DATA_CHECK_ERROR;
  gts_QtimeType z_GtsTime = {0};
  

  p_xtra_info = &cmd_ptr->info;

  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
  if (cmd_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("Null cmd_ptr",0,0,0);
    event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;
    (void) tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS,event_payload );
    return FALSE;
  }

  if (p_xtra_info->cmd_err != PDSM_LITE_XTRA_CMD_ERR_NOERR)
  {
    TM_XTRA_MSG_ERROR("Time injection command error found", 0, 0, 0);
    event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;
    (void) tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS,event_payload );
    return FALSE;
  }

  TM_XTRA_MSG_HIGH("Processing inject_time_info command. force %d",
                  p_xtra_info->b_ForceFlag,0,0);
  w_GpsWeek = (uint16) ( p_xtra_info->d_TimeMsec / ( TM_XTRA_GPS_SECONDS_IN_WEEK * 1000.0) );
  u_GpsMsec = (uint32) ( p_xtra_info->d_TimeMsec - w_GpsWeek* ( TM_XTRA_GPS_SECONDS_IN_WEEK * 1000.0));
  
  if( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
  {
    char u_string[100];
    (void) snprintf(u_string, sizeof(u_string), "=TM XTRA= Time injection: %0.3lf(Week: %d,Msec: %ld), unc %0.3f, utc %d",
                                          p_xtra_info->d_TimeMsec,
                                          w_GpsWeek,
                                          u_GpsMsec,
                                          p_xtra_info->f_TimeUncMsec, 
                                          p_xtra_info->b_RefToUtcTime);
    MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", u_string);
  }

  if (!p_xtra_info->b_ForceFlag && (w_GpsWeek < TM_XTRA_MIN_ACCEPTABLE_GPS_WEEK_FOR_INJECTION) )
  {
     TM_XTRA_MSG_ERROR("Invalid GPS week (%u) in XTRA time injection",w_GpsWeek,0,0);
     event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;
     (void) tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS,event_payload );

     if (0 == w_GpsWeek )
     {
       /* GpsWeek ==0 indicates NTP download failure.
          Handle it as if avail_request_timer expired.
       */
       os_TimerStop(xtra_state_ptr->xtra_avail_request_timer);

       tm_xtra_handle_avail_request_timer_expiry();
     }
     return FALSE;
  }
  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/  
  if( NULL != xtra_state_ptr)
  {
   /* Update the XTRA common buffer of the injected time to be used in later validity checks. */
   xtra_state_ptr->gps_time.w_GpsWeek = w_GpsWeek;
   xtra_state_ptr->gps_time.q_GpsMsec = u_GpsMsec;
   xtra_state_ptr->gps_time.f_ClkTimeUncMs = p_xtra_info->f_TimeUncMsec;
   xtra_state_ptr->gps_time.e_SourceOfTime = MGP_SLOW_CLK_SRC_XTRA;
  }

  /* Use the GPS time from HLOS to calculate the age of XTRA data ( if present ) and inform SM
     via callback */
  if(TRUE == tm_xtra_determine_data_age(w_GpsWeek, u_GpsMsec, &b_is_data_old))
  {
    if(NULL != xtra_state_ptr) 
    {
      /* Stop the xtra avail request timer if running */
      os_TimerStop(xtra_state_ptr->xtra_avail_request_timer);

      /* If the data is old , check whether XTRA was already requested before or if download is in progress */
      if(TRUE == b_is_data_old)
      {
        /* XTRA data is too old . Request for an XTRA download if there is an xtra process availability request  */
        if(TRUE == xtra_state_ptr->b_xtra_process_avail_req)
        {
          TM_XTRA_MSG_HIGH("XTRA data too old as determined from NTP time, trying for download",0,0,0);		
          if(FALSE == tm_xtra_request_download(&e_data_check))
          {
            TM_XTRA_MSG_ERROR("XTRA data download not requested due to check value %d",e_data_check,0,0);	            
            xtra_state_ptr->b_xtra_process_avail_req = FALSE;
            if(TM_XTRA_DATA_CHECK_SUCCESS == e_data_check)
            {
              /* Not sure if we can get into this code path . Since XTRA age is already checked before. */
              tm_xtra_process_avail_request_done(TRUE);
            }
            else
            {
              /* Inform SM about the status of XTRA avilability */
              tm_xtra_process_avail_request_done(FALSE);			
            }
          }
          else
          {
            /* restart the XTRA process avail request timer . */
            TM_XTRA_MSG_HIGH("Restart xtra avail request timer for %d secs",TM_XTRA_AVAIL_REQUEST_TIME_WAIT_MSEC/1000,0,0);
            (void)os_TimerStart(xtra_state_ptr->xtra_avail_request_timer,
                                TM_XTRA_AVAIL_REQUEST_TIME_WAIT_MSEC, 
                                0);
          }
        }// end of if(TRUE == xtra_state_ptr->b_xtra_process_avail_req)
      }
      else
      {
        /* XTRA data is valid . Notify SM with if there is an xtra process availability request */
        if(TRUE == xtra_state_ptr->b_xtra_process_avail_req)
        {
          xtra_state_ptr->b_xtra_process_avail_req = FALSE;
          /* Inform SM about the status of XTRA avilability */
          tm_xtra_process_avail_request_done(TRUE);     
        }
      }// end if(TRUE == b_is_data_old)
    }// end if(NULL != xtra_state_ptr) 
  }
  
  lm_info_req_param.e_req_type = LM_REQUEST_SLOW_CLK_TIME_INJECT;

  lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.t_Milliseconds = (uint64) p_xtra_info->d_TimeMsec; 
  /*Increase the uncertainty to 3.5sec if less than 3.5sec is injected in-order to prevent pattern-match in MGP*/
  if(p_xtra_info->f_TimeUncMsec < TM_XTRA_MIN_INJ_TIME_UNC)
  {
    lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.q_Uncertainty = (uint32)TM_XTRA_MIN_INJ_TIME_UNC;
  }
  else
  {
    lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.q_Uncertainty = (uint32)p_xtra_info->f_TimeUncMsec;
  }
  
  lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.e_TimeSource = MGP_SLOW_CLK_SRC_XTRA; 
  if(!p_xtra_info->b_RefToUtcTime)
  {
    lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.e_SlowClkSrc = GNSS_SLOW_CLOCK_SRC_GPS;
  }
  else
  {
    lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.e_SlowClkSrc = GNSS_SLOW_CLOCK_SRC_UTC;
  }
  lm_info_req_param.z_request.z_slow_clk_inject_request.e_source = 
        p_xtra_info->b_RefToUtcTime ? GNSS_SLOW_CLOCK_SRC_UTC 
                                    : GNSS_SLOW_CLOCK_SRC_GPS;
  lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.u_DiscontinuityCnt = 0; 
  lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.b_ForceFlag = p_xtra_info->b_ForceFlag; 

  /* Inject time to MGP through LM */
  ret_val = lm_request(&lm_info_req_param);
  /* As NTP download is done set b_ntp_download_needed to FALSE 
     even if inject to MGP is failed due to some internal issue. 
     This will allow MGP load even when injecion to MGP failed 
     on avail request timer expiry. This is in line with the case when XTRA
     is successful downloaded while time inejection failed before.
 */
  b_ntp_download_needed = FALSE;
  
  if(ret_val)
  {
    /* store current time so that age of NTP time can be calculated*/
    gts_GetQtime(&z_GtsTime);
    if ( TRUE == z_GtsTime.b_QtimeValid )
    {
      t_LastNtpInjectGpsTimeMs = z_GtsTime.t_Qtime / GTS_USTMR_PER_MS;
    }
    else
    {
      t_LastNtpInjectGpsTimeMs = TM_XTRA_UINT64_MAX;
    }

    TM_XTRA_MSG_HIGH("Time injection to LM/MGP successful",0,0,0); 
    event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_SUCCESS;
    (void) tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS,event_payload );

  }
  else
  {
     TM_XTRA_MSG_ERROR("Time injection to LM/MGP failed", 0, 0, 0);  
     event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;
     (void) tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS,event_payload );
  }

  return ret_val;
} /* End tm_xtra_process_inject_time_info() */

/*===========================================================================
FUNCTION tm_xtra_process_query_xtra_cfg_info_cmd

DESCRIPTION
Process the query xtra config info command request from XTRA client

INPUT PARAMS
pdsm_lite_xtra_cmd_s_type  *cmd_ptr : Input xtra cmd .

OUTPUT PARAMS
None

RETURN VALUE
XTRA command status
===========================================================================*/
boolean tm_xtra_process_query_xtra_cfg_info_cmd
(
  pdsm_lite_xtra_cmd_s_type  *cmd_ptr
)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  tm_xtra_event_payload_u_type  event_payload = {0};
  tm_xtra_state_type *p_xtra_state_info = tm_xtra_get_state_ptr();

  /*-----------------------------------------------------------------------
      Check input parameters
  -----------------------------------------------------------------------*/
  if (cmd_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("Received NULL cmd_ptr pointer parameter",0,0,0);
    // send failure in indication
    event_payload.cfg_info_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;
    tm_xtra_trigger_event( TX_XTRA_EVENT_TYPE_CFG_INFO, event_payload ) ;
  }
  else
  {
    p_xtra_state_info->auto_download_enabled = cmd_ptr->info.enable_auto_download ;
    if (!p_xtra_state_info->auto_download_enabled)
    {
        /* Stop the Auto dload timer */
        (void)os_TimerStop(p_xtra_state_info->auto_dload_timer);
    }


    TM_XTRA_MSG_MED("Query_config_info command. AutoDownload:%d ",cmd_ptr->info.enable_auto_download,0,0);
    /* Request the time from MGP*/
     ret_val = tm_xtra_data_validity_check(GPS_TIME_CHECK_SOURCE_CFG_REQ);
  } /* end else */

  return ret_val;
} /* End tm_xtra_process_query_xtra_cfg_info_cmd() */


/*===========================================================================
=============================================================================

   TM-XTRA: PD API Event Handlers
   
=============================================================================
===========================================================================*/


/*===========================================================================
FUNCTION tm_xtra_get_status_event_info

DESCRIPTION
Fill the status event parameters to be deliverd to the XTRA clients

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_get_status_event_info
(
  pdsm_lite_xtra_info_s_type          *info_ptr,
  tm_xtra_status_payload_s_type  *payload_ptr
)
{

  /* Local Variables */
  boolean ret_val = FALSE;

  /*-----------------------------------------------------------------------
      Check input parameters
  -----------------------------------------------------------------------*/
  if (info_ptr == NULL || payload_ptr == NULL )
  {
    TM_XTRA_MSG_HIGH("Received NULL pointer parameter",0,0,0);
  }
  else
  {  
    /*-----------------------------------------------------------------------
        Fill in the valid XTRA information for the STATUS event
    -----------------------------------------------------------------------*/
    info_ptr->xtra_event = PDSM_LITE_XTRA_EVENT_STATUS;
    
    /* Set the valid bits flag in download_info struct */
    info_ptr->xtra_union.xtra_download_info.flags 
      = XTRA_DOWNLOAD_INFO_FLAGS_STATUS;
  
    /* set status value */
    info_ptr->xtra_union.xtra_download_info.status = payload_ptr->status;
    if(info_ptr->xtra_union.xtra_download_info.status == XTRA_DOWNLOAD_STATUS_SUCCESS)
    {
      info_ptr->xtra_union.xtra_download_info.value  = payload_ptr->value;
    }
    else
    {
       info_ptr->xtra_union.xtra_download_info.value  = 0;
    }

    ret_val = TRUE;
  }
  
  return ret_val;
} /* End tm_xtra_get_status_event_info() */


/*===========================================================================
FUNCTION tm_xtra_get_data_download_event_info

DESCRIPTION
Fill the download request event parameters to be deliverd to the XTRA clients

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_get_data_download_event_info
(
  pdsm_lite_xtra_info_s_type              *info_ptr,
  tm_xtra_data_dload_payload_s_type  *payload_ptr
)
{
  /* Local Variables */
  boolean              ret_val = FALSE;
  uint8 *InUrl_ptr[3], *OutUrl_ptr[3];
  
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  
  if(xtra_state_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
      Check input parameters
  -----------------------------------------------------------------------*/
  if (info_ptr == NULL)
  {
    TM_XTRA_MSG_HIGH("Received NULL pointer parameter",0,0,0);
    return ret_val;
  }

  
  do{
    /*-----------------------------------------------------------------------
        Check current XTRA module state
    -----------------------------------------------------------------------*/
    if ( FALSE == tm_xtra_is_enable() )
    {
      TM_XTRA_MSG_MED("Feature disabled",0,0,0);
      break;
    }
    else if ( tm_xtra_is_download_in_progress() )
    {
      TM_XTRA_MSG_MED("Trigger download.  Download currently in progress - no download request sent to client. Force %d",
                       payload_ptr->force_data_dload,0,0);
      break;
    }
    else if ( tm_xtra_is_download_req_pending() )
    {
      /* Do not force the download request if already pending */
      if (  FALSE == payload_ptr->force_data_dload )
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Trigger download.  Download already pending - no download request sent to client. Force %d",
                 payload_ptr->force_data_dload,0,0);
        break;
      }
    }

    
    /*-----------------------------------------------------------------------
        Client is allowed start download
        Fill in the valid XTRA information for the DOWNLOAD_REQ event
    -----------------------------------------------------------------------*/
    TM_XTRA_MSG_HIGH("Trigger download - sending data download request to client. Force %d",
                      payload_ptr->force_data_dload,0,0);

    /* Send the Download request event */
    info_ptr->xtra_event = PDSM_LITE_XTRA_EVENT_DOWNLOAD_REQ;


    /*-----------------------------------------------------------------------
       Populate the payload for download reqest
    -----------------------------------------------------------------------*/
    /* Set command */
    info_ptr->xtra_union.xtra_download_info.command = XTRA_COMMAND_RETRIEVE_FILE;

    /* Set the valid bits flag in download_info struct */
    info_ptr->xtra_union.xtra_download_info.flags 
                                =   (XTRA_DOWNLOAD_INFO_FLAGS_COMMAND) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_WEEK) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_MINS) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_VALID_DURATION) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_SERVER1) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_SERVER2) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_SERVER3) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_MAX_FILE_PART_SIZE) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_MAX_FILE_SIZE);

    /* Set status value */
    info_ptr->xtra_union.xtra_download_info.status = XTRA_DOWNLOAD_STATUS_MAX;

    /* Randomize and set server URLs */
    InUrl_ptr[0] = (uint8 *)xtra_state_ptr->primary_data_server_url;
    InUrl_ptr[1] = (uint8 *)xtra_state_ptr->secondary_data_server_url;
    InUrl_ptr[2] = (uint8 *)xtra_state_ptr->tertiary_data_server_url;
  
    OutUrl_ptr[0] = (uint8 *)info_ptr->xtra_union.xtra_download_info.xtra_server_primary;
    OutUrl_ptr[1] = (uint8 *)info_ptr->xtra_union.xtra_download_info.xtra_server_secondary;
    OutUrl_ptr[2] = (uint8 *)info_ptr->xtra_union.xtra_download_info.xtra_server_tertiary;
  
    tm_xtra_randomize_and_set_urls(InUrl_ptr, OutUrl_ptr);
         
#ifdef FEATURE_GNSS_XTRA3 
    //tm_xtra3_set_download_url(OutUrl_ptr, sizeof(OutUrl_ptr)/sizeof(OutUrl_ptr[0]), TM_XTRA_MAX_SERVER_URL_SIZE);
#endif /* FEATURE_GNSS_XTRA3 */
                 
    /* Set file block transfer information */
    info_ptr->xtra_union.xtra_download_info.maxFilePartSize = TM_XTRA_MAX_FILE_PART_SIZE;
    info_ptr->xtra_union.xtra_download_info.maxFileSize = TM_XTRA_MAX_FILE_SIZE;


    /*-----------------------------------------------------------------------
        Get XTRA time and duration validy info from Almanac corrections data
    -----------------------------------------------------------------------*/
    if(FALSE == tm_xtra_set_xtra_time_and_validity(&info_ptr->xtra_union.xtra_download_info))
    {
      ret_val = FALSE;
      TM_XTRA_MSG_ERROR("Set_xtra_time_and_validity failed",0,0,0);
      break;
    }

 
#ifdef FEATURE_CGPS_XTRA_DEBUG
    /* Debug message: Print out the parameters sent to the client application. */
    snprintf(pchAsciiOutput, 
             sizeof(pchAsciiOutput), 
             "Sending XTRA download request. URLs:%s,%s,%s\n",
             info_ptr->xtra_union.xtra_download_info.xtra_server_primary,
             info_ptr->xtra_union.xtra_download_info.xtra_server_secondary,
             info_ptr->xtra_union.xtra_download_info.xtra_server_tertiary);
    TM_XTRA_MSG_LOW(pchAsciiOutput,0,0,0);

#endif /* FEATURE_CGPS_XTRA_DEBUG */

  
    ret_val = TRUE;
    
  } while (0);
  
  return ret_val;
} /* End tm_xtra_get_data_download_event_info() */


/*===========================================================================
FUNCTION tm_xtra_get_time_download_event_info

DESCRIPTION
Fill the time info download request event parameters to be deliverd to 
the XTRA clients

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_get_time_download_event_info
(
  pdsm_lite_xtra_info_s_type               *info_ptr,
  tm_xtra_time_dload_payload_s_type   *payload_ptr
)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  uint8 *InUrl_ptr[3], *OutUrl_ptr[3];
  
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  
  if(xtra_state_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
    return FALSE;
  }

  
  /*-----------------------------------------------------------------------
      Check input parameters
  -----------------------------------------------------------------------*/
  if (info_ptr == NULL)
  {
    TM_XTRA_MSG_HIGH("Received NULL pointer parameter",0,0,0);
    return ret_val;
  }

  
  do{
    /*-----------------------------------------------------------------------
        Check current XTRA module state
    -----------------------------------------------------------------------*/
    if (    !tm_xtra_is_enable() 
         || !tm_xtra_is_time_info_enable() )
    {
      TM_XTRA_MSG_MED("Feature disabled",0,0,0);
      break;
    }

    /*-----------------------------------------------------------------------
        Client is allowed start download
        Fill in the valid XTRA information for the TIME_REQ event
    -----------------------------------------------------------------------*/
    TM_XTRA_MSG_HIGH("Trigger download - sending time info request to client. Force %d",
                      payload_ptr->force_time_info_dload,0,0);

    /* Send the time info request event */
    info_ptr->xtra_event = PDSM_LITE_XTRA_EVENT_TIME_REQ;

    /*-----------------------------------------------------------------------
       Populate the payload for download reqest
    -----------------------------------------------------------------------*/
    /* Set command */
    info_ptr->xtra_union.xtra_time_info.command = XTRA_COMMAND_RETRIEVE_TIME_INFO;

    /* Randomize and set server URLs */
    InUrl_ptr[0] = (uint8 *)xtra_state_ptr->primary_sntp_server_url;
    InUrl_ptr[1] = (uint8 *)xtra_state_ptr->secondary_sntp_server_url;
    InUrl_ptr[2] = (uint8 *)xtra_state_ptr->tertiary_sntp_server_url;

    OutUrl_ptr[0] = (uint8 *)info_ptr->xtra_union.xtra_time_info.xtra_server_primary;
    OutUrl_ptr[1] = (uint8 *)info_ptr->xtra_union.xtra_time_info.xtra_server_secondary;
    OutUrl_ptr[2] = (uint8 *)info_ptr->xtra_union.xtra_time_info.xtra_server_tertiary;

    tm_xtra_randomize_and_set_urls(InUrl_ptr, OutUrl_ptr);
    
    /* Set one-way delay failover threshold */
    info_ptr->xtra_union.xtra_time_info.oneway_delay_failover_thresh
                             = xtra_state_ptr->time_info_oneway_delay_thresh;

#ifdef FEATURE_CGPS_XTRA_DEBUG
    /* Debug message: Print out the parameters sent to the client application. */
    snprintf(pchAsciiOutput, 
             sizeof(pchAsciiOutput),
             "Request time info - Thresh=%ld, URLs:%s,%s,%s\n",
             info_ptr->xtra_union.xtra_time_info.oneway_delay_failover_thresh,
             info_ptr->xtra_union.xtra_time_info.xtra_server_primary,
             info_ptr->xtra_union.xtra_time_info.xtra_server_secondary,
             info_ptr->xtra_union.xtra_time_info.xtra_server_tertiary);
    TM_XTRA_MSG_LOW(pchAsciiOutput,0,0,0);
#endif /* FEATURE_CGPS_XTRA_DEBUG */

  
    ret_val = TRUE;
    
  } while (0);
    
  return ret_val;
} /* End tm_xtra_get_time_download_event_info() */


/*===========================================================================
FUNCTION tm_xtra_get_query_data_valid_event_info

DESCRIPTION
Fill the query valid request info to be sent to XTRA clients

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_get_query_data_valid_event_info
(
  pdsm_lite_xtra_info_s_type               *info_ptr,
  tm_xtra_data_query_payload_s_type   *payload_ptr
)
{
  /* Local Variables */
  boolean              ret_val = FALSE;

  /*-----------------------------------------------------------------------
      Check input parameters
  -----------------------------------------------------------------------*/
  if (info_ptr == NULL)
  {
    TM_XTRA_MSG_HIGH("Received NULL pointer parameter",0,0,0);
    return ret_val;
  }


  do
  {
    /*-----------------------------------------------------------------------
        Check current XTRA module state
    -----------------------------------------------------------------------*/
    if ( !tm_xtra_is_enable() )
    {
      TM_XTRA_MSG_MED("Feature disabled",0,0,0);
      break;
    }

    /*-----------------------------------------------------------------------
        Client is allowed start download
        Fill in the valid XTRA information for the TIME_REQ event
    -----------------------------------------------------------------------*/
    TM_XTRA_MSG_HIGH("Trigger query data request - sending info to client(status:%d)",
                      payload_ptr->status,0,0);

    /* Send the time info request event */
    info_ptr->xtra_event = PDSM_LITE_XTRA_EVENT_STATUS;


    /*-----------------------------------------------------------------------
       Populate the payload for query data validity request
    -----------------------------------------------------------------------*/
    /* Set the valid bits flag in download_info struct */
    info_ptr->xtra_union.xtra_download_info.flags 
                               = (XTRA_DOWNLOAD_INFO_FLAGS_STATUS) \
                               | (XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_WEEK) \
                               | (XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_MINS) \
                               | (XTRA_DOWNLOAD_INFO_FLAGS_VALID_DURATION);

    /* Set status value */
    info_ptr->xtra_union.xtra_download_info.status = payload_ptr->status;

    /* Initialize the URL data */
    memset((void*)info_ptr->xtra_union.xtra_time_info.xtra_server_primary,
           0x0,
           TM_XTRA_MAX_SERVER_URL_SIZE);

    memset((void*)info_ptr->xtra_union.xtra_time_info.xtra_server_secondary,
           0x0,
           TM_XTRA_MAX_SERVER_URL_SIZE);

    memset((void*)info_ptr->xtra_union.xtra_time_info.xtra_server_tertiary,
           0x0,
           TM_XTRA_MAX_SERVER_URL_SIZE);

    /* Set file block transfer information */
    info_ptr->xtra_union.xtra_download_info.maxFilePartSize = TM_XTRA_NULL_FILE_SIZE;
    info_ptr->xtra_union.xtra_download_info.maxFileSize = TM_XTRA_NULL_FILE_SIZE;


    /*-----------------------------------------------------------------------
        Get XTRA time and duration validy info from Almanac corrections data
    -----------------------------------------------------------------------*/
    if(FALSE == tm_xtra_set_xtra_time_and_validity(&info_ptr->xtra_union.xtra_download_info))
    {
       ret_val = FALSE;
       TM_XTRA_MSG_ERROR("Set_xtra_time_and_validity failed",0,0,0);
       break;
    }
  
    ret_val = TRUE;

  } while (0);

  return ret_val;
} /* End tm_xtra_get_query_data_valid_event_info()*/

/*===========================================================================
FUNCTION tx_xtra_convert_Gps_to_utc

DESCRIPTION
Convert GPS time (Gps week and tow in sec) to UTC .

INPUT PARAMS
uint16 w_gpsWeek : Gps Week number
uint32 q_gpsTowSec : Gps Tow in sec

OUTPUT PARAMS
None

RETURN VALUE 
UTC time in sec
===========================================================================*/
uint64 tx_xtra_convert_Gps_to_utc(uint16 w_gpsWeek, uint32 q_gpsTowSec)
{
  return(( uint64) (DELTA_UTC_TO_GPS_SECONDS + w_gpsWeek * 7 * 24 * 60 * 60 + q_gpsTowSec) );
}
/*===========================================================================
FUNCTION tm_xtra_get_config_info_ind_event_info

DESCRIPTION
Fill the config indication to be sent to XTRA clients

INPUT PARAMS
tm_xtra_cfg_info_payload_s_type     *payload_ptr : input xtra config req payload

OUTPUT PARAMS
pdsm_lite_xtra_info_s_type  *info_ptr : output Xtra config indication 

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_get_config_info_ind_event_info(
  pdsm_lite_xtra_info_s_type               *info_ptr,
  tm_xtra_cfg_info_payload_s_type     *payload_ptr
)
{
  pdsm_lite_xtra_config_info_s_type *xtra_config_info;
  pdsm_lite_xtra_download_info_s_type  xtra_download_info;
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  /* Local Variables */
  boolean              ret_val = FALSE;
  
  sm_XtraConstelEnumType e_ConstelType;

  /*-----------------------------------------------------------------------
      Check input parameters
  -----------------------------------------------------------------------*/
  if(NULL == xtra_state_ptr )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
    return ret_val;
  }

  if ( NULL == info_ptr || NULL == payload_ptr )
  {
    TM_XTRA_MSG_HIGH("Received NULL pointer parameter",0,0,0);
    return ret_val;
  }

  xtra_config_info = &info_ptr->xtra_union.xtra_cfg_info;
  memset(xtra_config_info,0,sizeof(*xtra_config_info));


  do
  {
    xtra_config_info->status  = XTRA_DOWNLOAD_STATUS_SUCCESS;
    /* Send the time info request event */
    info_ptr->xtra_event = PDSM_LITE_XTRA_EVENT_CFG_REQ ;
    ret_val = TRUE;
    if ( XTRA_DOWNLOAD_STATUS_SUCCESS != payload_ptr->status )
    {
      xtra_config_info->status = XTRA_DOWNLOAD_STATUS_FAILURE ;
      break;
    }

    /*-----------------------------------------------------------------------
        Check current XTRA module state
    -----------------------------------------------------------------------*/
    if ( !tm_xtra_is_enable() )
    {
      TM_XTRA_MSG_MED("Feature disabled",0,0,0);
      // we have to still send msg 
      xtra_config_info->status = XTRA_DOWNLOAD_STATUS_GPS_LOCK; 
      
      break;
    }
    

    /* Before requesting XTRA download query the SM for the desired constellations we need to use */
    if(FALSE == sm_GetXtraConstelType(&e_ConstelType))
    {
      TM_XTRA_MSG_ERROR("Constellation info not available",0,0,0);
      return FALSE;
    }
   
   TM_XTRA_MSG_HIGH("Constellation type from SM :%d ",e_ConstelType, 0, 0);

   /* Set the server data URL for the constellations */
   tm_xtra_set_data_server_url_for_constel(e_ConstelType);

    memscpy(xtra_config_info->xtra_time_server_primary ,GNSS_ARRAY_SIZEOF(xtra_config_info->xtra_time_server_primary),
            xtra_state_ptr->primary_sntp_server_url,TM_XTRA_MAX_SERVER_URL_SIZE);
    memscpy(xtra_config_info->xtra_time_server_secondary ,GNSS_ARRAY_SIZEOF(xtra_config_info->xtra_time_server_primary),
            xtra_state_ptr->secondary_sntp_server_url,TM_XTRA_MAX_SERVER_URL_SIZE);
    memscpy(xtra_config_info->xtra_time_server_tertiary ,GNSS_ARRAY_SIZEOF(xtra_config_info->xtra_time_server_primary),
            xtra_state_ptr->tertiary_sntp_server_url,TM_XTRA_MAX_SERVER_URL_SIZE);
    xtra_config_info->report_mask |= XTRA_CFG_INFO_MASK_NTP_SERVER;

    memscpy(xtra_config_info->xtra_server_primary ,GNSS_ARRAY_SIZEOF(xtra_config_info->xtra_server_primary),
            xtra_state_ptr->primary_data_server_url,TM_XTRA_MAX_SERVER_URL_SIZE);
    memscpy(xtra_config_info->xtra_server_secondary ,GNSS_ARRAY_SIZEOF(xtra_config_info->xtra_server_secondary),
            xtra_state_ptr->secondary_data_server_url,TM_XTRA_MAX_SERVER_URL_SIZE);
    memscpy(xtra_config_info->xtra_server_tertiary ,GNSS_ARRAY_SIZEOF(xtra_config_info->xtra_server_tertiary),
            xtra_state_ptr->tertiary_data_server_url,TM_XTRA_MAX_SERVER_URL_SIZE);
    xtra_config_info->report_mask |= XTRA_CFG_INFO_MASK_XTRA_SERVER;

    tm_core_get_xtra_validity_age(&xtra_config_info->prefered_valid_age_hrs);
    
    xtra_config_info->report_mask |= XTRA_CFG_INFO_MASK_PREF_VALID_AGE;
    
    if ( (TM_CORE_CONFIG_DD(gps_lock) == PDSM_LITE_GPS_LOCK_MI)||(TM_CORE_CONFIG_DD(gps_lock) == PDSM_LITE_GPS_LOCK_ALL))
    {
      /** Even though GPS is locked let us return xtra and ntp server **/
      xtra_config_info->status = XTRA_DOWNLOAD_STATUS_GPS_LOCK; 
      break;
    }

    TM_XTRA_MSG_HIGH("config_info_ind Valid Age :%u GpsWeek:%u gpsMs:%u",xtra_config_info->prefered_valid_age_hrs,
                      xtra_state_ptr->gps_time.w_GpsWeek,xtra_state_ptr->gps_time.q_GpsMsec);
    /*-----------------------------------------------------------------------
    If the GPS time is unknown or time uncertainty is great than a threshold,
    we need time
     -----------------------------------------------------------------------*/
  if((xtra_state_ptr->gps_time.w_GpsWeek == C_GPS_WEEK_UNKNOWN) ||  
     (xtra_state_ptr->gps_time.f_ClkTimeUncMs >= xtra_state_ptr->time_info_uncertainty_thresh) )
    {
      xtra_config_info->report_mask |= XTRA_CFG_INFO_MASK_TIME_REQUEST;  /** Set request time  **/
      xtra_config_info->time_request = 1;
    }

    memset(&xtra_download_info,0,sizeof(xtra_download_info));
    if(FALSE == tm_xtra_set_xtra_time_and_validity(&xtra_download_info))
    {
      TM_XTRA_MSG_ERROR("Set_xtra_time_and_validity failed",0,0,0);
      break;
    }

    TM_XTRA_MSG_HIGH("config_info_ind xtrainfoflags [0x%x]",xtra_download_info.flags,0,0);
    if (( XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_WEEK & xtra_download_info.flags ) && 
        ( XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_MINS & xtra_download_info.flags ))
    {
      /** valid xtra file time **/
      double  d_xtraFileTime = 0 ;
      
      xtra_config_info->rel_age_utc = (uint64 ) tx_xtra_convert_Gps_to_utc(xtra_download_info.startGpsWeek,
                                                                           (xtra_download_info.startGpsMinutes * 60 ) );
      xtra_config_info->report_mask |= XTRA_CFG_INFO_MASK_REL_AGE;
      TM_XTRA_MSG_MED("Xtra file time is valid gpsWeek[%u] gpstime[%u] mins relAge[%lu] msec",xtra_download_info.startGpsWeek,
                       xtra_download_info.startGpsMinutes,xtra_config_info->rel_age_utc);
      /** if we current time is valid caluculate abs age **/
      if(xtra_state_ptr->gps_time.w_GpsWeek != C_GPS_WEEK_UNKNOWN)
      {
        double d_curGpsTime = 0 , d_xtraFileAge=0;
        
        /** we have time we can caluculate abs age **/
        
        d_curGpsTime    = TM_XTRA_CONVERT_GPS_FULL_MS(xtra_state_ptr->gps_time.w_GpsWeek ,xtra_state_ptr->gps_time.q_GpsMsec);
        d_xtraFileTime  = TM_XTRA_CONVERT_GPS_FULL_MS(xtra_download_info.startGpsWeek,
                                                      (xtra_download_info.startGpsMinutes * 60 * TM_XTRA_SEC_TO_MSEC) );

        if ( d_curGpsTime > d_xtraFileTime )
        {
          xtra_config_info->abs_age_hrs = (uint32)((d_curGpsTime - d_xtraFileTime ) / TM_XTRA_MSEC_IN_HOUR);
          xtra_config_info->report_mask |= XTRA_CFG_INFO_MASK_ABS_AGE;
        }
        TM_XTRA_MSG_MED("Current Time is valid gpsWeek[%u] gpstime[%u] msec AbsAge[%u] hrs",xtra_state_ptr->gps_time.w_GpsWeek,
                          xtra_state_ptr->gps_time.q_GpsMsec,xtra_config_info->abs_age_hrs );

      }
    }

  }while(0);

  return (ret_val);
}
/*===========================================================================
FUNCTION tm_xtra_trigger_event

DESCRIPTION
Send the specified XTRA event to the client

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_trigger_event
(
  tm_xtra_event_e_type          xtra_event,
  tm_xtra_event_payload_u_type  event_payload
)
{
  /* Local variables */
  boolean                ret_val = TRUE;
  pdsm_lite_xtra_info_s_type  u_xtra_info = {0};

  /*-----------------------------------------------------------------------
      Process the incoming XTRA event request
  -----------------------------------------------------------------------*/
  switch ( xtra_event )
  { 
    case TM_XTRA_EVENT_TYPE_STATUS:
      ret_val = tm_xtra_get_status_event_info( &u_xtra_info, 
                                               &event_payload.status_payload);
      break;
    case TM_XTRA_EVENT_TYPE_REQ_DATA_DOWNLOAD:

      ret_val = FALSE;
      /*Forced downloads are exempt from blocking */
      
      if ( (tm_xtra_is_req_blocked() == FALSE) || (event_payload.data_dload_payload.force_data_dload == TRUE) )
      {
        /* During A-GPS session do not send XTRA download request */
        if( tm_core_is_xtra_operation_allowed() )
        {
          ret_val = tm_xtra_get_data_download_event_info( &u_xtra_info, 
                                                        &event_payload.data_dload_payload );
           TM_XTRA_MSG_HIGH("Sending XTRA Download Request, force(%d)", event_payload.data_dload_payload.force_data_dload,0,0);
           /* Throttle On Demand data requests. Unblock on successful injection or retry timer */
           if( (ret_val == TRUE) && (event_payload.data_dload_payload.force_data_dload==FALSE) )
           {
              tm_xtra_req_block();
           }   
        }
        else
        {
          TM_XTRA_MSG_ERROR("Active A-GPS session, download event is blocked",0,0,0);
        }
      }
      else
      {
          TM_XTRA_MSG_HIGH("XTRA Download Request Blocked", 0,0,0);
      }
      
      break;
      
    case TM_XTRA_EVENT_TYPE_REQ_TIME:
      ret_val = tm_xtra_get_time_download_event_info( &u_xtra_info,
                                                      &event_payload.time_dload_payload );
      break;
    case TM_XTRA_EVENT_TYPE_REQ_QUERY_DATA_VALID:
      ret_val = tm_xtra_get_query_data_valid_event_info( &u_xtra_info,
                                                         &event_payload.data_query_payload );
      break;
 case TX_XTRA_EVENT_TYPE_CFG_INFO :
        ret_val = tm_xtra_get_config_info_ind_event_info( &u_xtra_info,
		                                                  &event_payload.cfg_info_payload );
      break;
    default:
      TM_XTRA_MSG_ERROR("TM xtra event (%d) not handled", xtra_event,0,0);
      ret_val = FALSE;
      break;
  }

  /* Only for known XTRA events and valid parameters, 
  ** send request to XTRA client 
  */
  if (ret_val)
  {
    TM_XTRA_MSG_MED( "Sending event %d", u_xtra_info.xtra_event,0,0);

    tm_pdapi_xtra_event_callback(u_xtra_info.xtra_event, &u_xtra_info);
  }

  return ret_val;
  
} /* End tm_xtra_trigger_event() */

/*===========================================================================
FUNCTION tm_xtra_start_process_data_info

DESCRIPTION
Start the data injection to MGP, first process the gpsOneXTRA 
file

INPUT PARAMS
tm_xtra_data_check_source_e_type     source

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
/* Test Code */
void sm_EfsXtraWrite( gnss_XtraVersionEnumType e_Version, uint8 *pu_Data, uint32 q_FileLen );
U32 sm_EfsXtraRead( gnss_XtraVersionEnumType e_Version, uint8* pu_Data );

void tm_xtra_start_process_data_info
(
  const tm_xtra_data_check_source_e_type     source
)
{
  /* Local Variables */
  pdsm_xtra_status_e_type e_ret_val = XTRA_DOWNLOAD_STATUS_FAILURE;
  tm_xtra_version_e_type  e_XtraVersion = XTRA_VERSION_UNKNOWN;
  uint32                  q_DecodedMask = 0;
  tm_xtra_state_type      *pz_xtra_state =  tm_xtra_get_state_ptr();

  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/
  if (pz_xtra_state == NULL)
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
    return;
  }
  
  /*-----------------------------------------------------------------------
    Process and the inject the gpsOneXTRA data
  -----------------------------------------------------------------------*/

  /* Process and inject the gpsOneXTRA data file */
  e_ret_val = tm_xtra_process_and_inject_data( pz_xtra_state->xtraTempBuf, 
                                               pz_xtra_state->q_XtraDataLen,
                                               &e_XtraVersion,
                                               &q_DecodedMask);

  /*-----------------------------------------------------------------------
     Send the gpsOneXTRA data download status to client
  -----------------------------------------------------------------------*/
  if (XTRA_DOWNLOAD_STATUS_SUCCESS == e_ret_val)
  {
    //May want to add check here to only write XTRA3 and XTRA Integrity files.
    sm_EfsXtraWrite( e_XtraVersion, &pz_xtra_state->xtraTempBuf[0], pz_xtra_state->q_XtraDataLen );

    /* Memory buffer to store XTRA.BIN data from XTRA Client */
#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
    if (pz_xtra_state->xtraTempBuf != NULL)
    {
      os_MemFree((void**)&pz_xtra_state->xtraTempBuf);
      pz_xtra_state->xtraTempBuf = NULL;
    }
#else
    memset((void*)pz_xtra_state->xtraTempBuf,
           0x0,
           TM_XTRA_MAX_FILE_SIZE);
#endif
    
    /* Initialize the data length for next file injection */
    pz_xtra_state->q_XtraDataLen = 0;

    /* Save time of last download */
    (void)time_get(pz_xtra_state->time_of_last_download);

    /* Only send indication to CD when Xtra files are processed. */
    if ( (e_XtraVersion == XTRA_VERSION_1) ||
         (e_XtraVersion == XTRA_VERSION_2) ||
         (e_XtraVersion == XTRA_VERSION_3) )
    {
      /* Save the XTRA decoded mask since XTRA decode has been successful */
      TM_XTRA_MSG_HIGH("Saving the XTRA decoded mask 0x%x",q_DecodedMask,0,0);      
      pz_xtra_state->q_xtra_decoded_mask = q_DecodedMask;
      pz_xtra_state->b_xtradata_saved = TRUE;
     /* Indicate to QMI that the XTRA injection is complete and send the last indication */
     tm_xtra_inj_done(q_DecodedMask);
    }
    if (e_XtraVersion == XTRA_VERSION_51)
    {
      /* Reset the XTRA state machine to IDLE for XTRA integrity */
      /* Indicate to QMI that the XTRA injection is complete and send the last indication */
      tm_xtra_inj_done(q_DecodedMask);
    }
#ifdef FEATURE_CGPS_XTRA_T
    /* will not trigger XTRA-T if XTRA is triggered by AGPS(SUPL/V2 etc) */
    if (tm_core_info.session_info.req_xtra_trig_by_agps == FALSE)
    {
      /* Let TLE know a XTRA download session completes. TLE will trigger
         a XTRA-T upload/download session.
       */
      (void) tle_RequestSessionByXtra(tm_core_is_standalone_active());
    }
    else
    {
      /* reset the "XTRA triggered by AGPS" flag */
      tm_core_info.session_info.req_xtra_trig_by_agps = FALSE;
    }
#endif

    tm_xtra_req_allow(); 

		/* Stop the Xtra avail request timer if started */
    os_TimerStop(pz_xtra_state->xtra_avail_request_timer);

    /* XTRA is downloaded and decoded now . Inform SM if there is an XTRA avilability request pending */
    /* Note that XTRA availability request is only for XTRA3. Inform SM only when 
       there is XTRA3 download */
    if((TRUE == pz_xtra_state->b_xtra_process_avail_req)&&
       (XTRA_VERSION_3 == e_XtraVersion))
    {
      pz_xtra_state->b_xtra_process_avail_req = FALSE;
      tm_xtra_process_avail_request_done(TRUE);
    }


  }
  else
  {
    /* Cleanup TM-XTRA: Error scenario, data download failed */
    pz_xtra_state->continue_blocking = TRUE;
    pz_xtra_state->b_xtradata_saved = FALSE;
    tm_xtra_abort_and_recover(e_ret_val);
  }

  return;

} /* End tm_xtra_start_process_data_info() */


/*===========================================================================
FUNCTION tm_xtra_check_time_uncertainty

DESCRIPTION
Check the current time uncertainty and fire the time info request if 
necessary

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
boolean - if time was requested from XTRA
===========================================================================*/
boolean tm_xtra_check_time_uncertainty
(
  void
)
{
  /* Local Variables */
  tm_xtra_event_payload_u_type  event_payload;

  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();

  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/
  if (xtra_state_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Only perform the check if the NV item is enabled
  -----------------------------------------------------------------------*/
  if( !tm_xtra_is_time_info_enable() )
  {
    TM_XTRA_MSG_ERROR("Time info feature is disabled",0,0,0);
    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
    If the GPS time is unknown or time uncertainty is great than a threshold,
    send a time info request to the client
  -----------------------------------------------------------------------*/
  if((xtra_state_ptr->gps_time.w_GpsWeek == C_GPS_WEEK_UNKNOWN) ||  
   (xtra_state_ptr->gps_time.f_ClkTimeUncMs 
            >= xtra_state_ptr->time_info_uncertainty_thresh) )
  {
    /*Do not send another XTRA time request if we are already processing one*/  
    if(!(xtra_state_ptr->time_info_req_src & GPS_TIME_CHECK_SOURCE_TIME_INJECT))
    {
      /*---------------------------------------------------------------------
         Send the gpsOneXTRA time info download request
      ---------------------------------------------------------------------*/
      event_payload.time_dload_payload.force_time_info_dload = TRUE;
  
      (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_TIME, 
                                   event_payload );

      /* Send gpsOneXTRA dload DM event */
      sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_TIME_INJECTION);

      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }

} /* End tm_xtra_check_time_uncertainty() */

/*===========================================================================
FUNCTION tm_xtra_start_process_time_info

DESCRIPTION
Start the time info injection to MGP, first process the gpsOneXTRA 
time info

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
void tm_xtra_start_process_time_info
(
  const tm_xtra_data_check_source_e_type     source
)
{
  /* Local Variables */
  tm_xtra_data_check_result_e_type  ret_val = TM_XTRA_DATA_CHECK_ERROR;
  tm_xtra_event_payload_u_type      event_payload;
  
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();

  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/  
  if (xtra_state_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
    return;
  } 
  
  /*-----------------------------------------------------------------------
    Check time uncertainty, may send time info request to clients
  -----------------------------------------------------------------------*/  
  if(!tm_xtra_check_time_uncertainty())
  {
    ret_val = tm_xtra_data_check_handler(GPS_TIME_CHECK_SOURCE_TIME_INJECT);

    /*-----------------------------------------------------------------------
      Send the XTRA download request to client if data expired
    -----------------------------------------------------------------------*/
    if (ret_val == TM_XTRA_DATA_CHECK_ERROR)
    {
      TM_XTRA_MSG_HIGH("XTRA data invalid/expired at time inject, request data download",0,0,0);

      event_payload.data_dload_payload.force_data_dload = FALSE;

      /* Send the download request event, update the session state */
      if( TRUE ==
               tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_DATA_DOWNLOAD, event_payload ) 
        )
      {
        /* Send gpsOneXTRA dload DM event */
        sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_FIX_REQUEST);
        
        if( tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ) )
        {
        /* Start the recover timer */
        (void)os_TimerStart(xtra_state_ptr->retry_dload_timer,
                            (xtra_state_ptr->time_between_attempts*TM_XTRA_MSEC_IN_MIN), 
                            0);
        }
      }
    }
  }

  return;
} /* End tm_xtra_start_process_time_info() */

/*===========================================================================
FUNCTION tm_xtra_start_process_query_cfg_info

DESCRIPTION
 Trigger time req , to send config resp.

INPUT PARAMS
tm_xtra_data_check_source_e_type     source : Time check source type

OUTPUT PARAMS
None

RETURN VALUE
None
===========================================================================*/
void tm_xtra_start_process_query_cfg_info
(
  const tm_xtra_data_check_source_e_type     source
)
{
  /* Local Variables */
  tm_xtra_event_payload_u_type      event_payload = {0};
  
  //Get time so that we can caluculate age , bundle it with xtra,ntp servers and send it
  event_payload.cfg_info_payload.status = XTRA_DOWNLOAD_STATUS_SUCCESS;
  tm_xtra_trigger_event( TX_XTRA_EVENT_TYPE_CFG_INFO, event_payload ) ;

  return;
} /* End tm_xtra_start_process_query_cfg_info() */

/*===========================================================================
FUNCTION tm_xtra_start_process_auto_dload_check

DESCRIPTION
Check the automatic periodic download parameters to determine if a new 
gpsOneXTRA data download is required.  Send data download request
event to client if necessary.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
void tm_xtra_start_process_auto_dload_check
(
  const tm_xtra_data_check_source_e_type     source
)
{
  /* Local Variables */
  tm_xtra_data_check_result_e_type  ret_val = TM_XTRA_DATA_CHECK_ERROR;
  tm_xtra_event_payload_u_type      event_payload;
  
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();

  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/  
  if (xtra_state_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
    return;
  }

  /*-----------------------------------------------------------------------
    Process and the inject the gpsOneXTRA data
  -----------------------------------------------------------------------*/

  /* Check the header validity */
  ret_val = tm_xtra_data_check_handler( source );
      
  /*-----------------------------------------------------------------------
    Send the XTRA download request to client if time expired
  -----------------------------------------------------------------------*/
  if (ret_val == TM_XTRA_DATA_CHECK_ERROR)
  {
    TM_XTRA_MSG_HIGH("Automatic download period expired, request data download",0,0,0);
    
    event_payload.data_dload_payload.force_data_dload = FALSE;

    /* Send the download request event, update the session state */
    if( TRUE ==
           tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_DATA_DOWNLOAD, event_payload ) 
      )
    {
      /* Send gpsOneXTRA dload DM event */
      sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_AUTO);

      if( !tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ) )
      {
        TM_XTRA_MSG_ERROR("Automatic download period expired, session state update failed",0,0,0);
      }
      else
      {
        /* Start the recover timer */
        (void)os_TimerStart(xtra_state_ptr->retry_dload_timer,
          (xtra_state_ptr->time_between_attempts*TM_XTRA_MSEC_IN_MIN), 
          0);
      }
    }
  }
  else if(ret_val == TM_XTRA_DATA_CHECK_INVALID_TIME)
  {
    /*---------------------------------------------------------------------
    Send the gpsOneXTRA time info download request
    ---------------------------------------------------------------------*/
    event_payload.time_dload_payload.force_time_info_dload = TRUE;

    (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_TIME, 
                                 event_payload );  

    /* Start the automatic download timer */
    (void)os_TimerStart(xtra_state_ptr->auto_dload_timer,
                        TM_XTRA_AUTO_DLOAD_CHECK_TIME, 
                        0);

    /* Send gpsOneXTRA dload DM event */
    sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_TIME_INJECTION);
  }

  return;

} /* End tm_xtra_start_process_auto_dload_check()*/


/*===========================================================================
FUNCTION tm_xtra_start_process_per_fix_check

DESCRIPTION
Check the automatic periodic download parameters to determine if a new 
gpsOneXTRA data download is required.  Send data download request
event to client if necessary.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
void tm_xtra_start_process_per_fix_check
(
  const tm_xtra_data_check_source_e_type     source
)
{
  /* Local Variables */
  tm_xtra_data_check_result_e_type  ret_val = TM_XTRA_DATA_CHECK_ERROR;
  tm_xtra_event_payload_u_type      event_payload;
  
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();

  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/  
  if (xtra_state_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
    return;
  }

  /*-----------------------------------------------------------------------
    Process and the inject the gpsOneXTRA data
  -----------------------------------------------------------------------*/
  /* Check the header validity */
  ret_val = tm_xtra_data_check_handler( source );
  
  /*-----------------------------------------------------------------------
    Send the XTRA download request to client if time expired
  -----------------------------------------------------------------------*/
  if (ret_val == TM_XTRA_DATA_CHECK_ERROR)
  {
    TM_XTRA_MSG_HIGH("XTRA data invalid/expired, request data download",0,0,0);
    
    event_payload.data_dload_payload.force_data_dload = FALSE;
    
    /* Send the download request event, update the session state */
    if( TRUE ==
            tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_DATA_DOWNLOAD, event_payload ) 
      )
    {
      /* Send gpsOneXTRA dload DM event */
      sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_FIX_REQUEST);
 
      if( tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ) )
      {
        /* Start the recover timer */
        (void)os_TimerStart(xtra_state_ptr->retry_dload_timer,
                            (xtra_state_ptr->time_between_attempts*TM_XTRA_MSEC_IN_MIN), 
                            0);
      }
    }
  }
  else if(ret_val == TM_XTRA_DATA_CHECK_INVALID_TIME)
  {
    /*---------------------------------------------------------------------
    Send the gpsOneXTRA time info download request
    ---------------------------------------------------------------------*/
    event_payload.time_dload_payload.force_time_info_dload = TRUE;

    (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_TIME, 
                                 event_payload );  

    /* Send gpsOneXTRA dload DM event */
    sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_TIME_INJECTION);
  }
#ifdef FEATURE_CGPS_XTRA_T
  else if (ret_val == TM_XTRA_DATA_CHECK_SUCCESS)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"XTRA data/time valid. Ask TLE for Pos Unc for XTRA-T if necessary",0,0,0);
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Request TLE reports cell position unc",0,0,0);
    (void) tle_GetTerrPosStatus();
  }
#endif /* FEATURE_CGPS_XTRA_T */

  return;

} /* End tm_xtra_start_process_per_fix_check()*/


/*===========================================================================
FUNCTION tm_xtra_gps_time_report_handler

DESCRIPTION
Handle the GPS time info from MGPS

Starts the following processes:
1) gpsOneXTRA data or time from clients
2) automatic download file time check
3) GPS fix request to check data validity

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
void tm_xtra_gps_time_report_handler
(
  gps_TimeStructType     *p_gps_time
)
{
  /* Local Variables */
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
 
  if(xtra_state_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);  
    return;
  }
  
  /*-----------------------------------------------------------------------
    Check input parameters
  -----------------------------------------------------------------------*/  
  if (p_gps_time == NULL)
  {
    TM_XTRA_MSG_ERROR("p_gps_time pointer is NULL",0,0,0);
    return;
  }

  /*-----------------------------------------------------------------------
      Process data validity check request
  -----------------------------------------------------------------------*/
  /* Save the GPS time from MGP */
  //xtra_state_ptr->gps_time = *p_gps_time;

  memscpy(&xtra_state_ptr->gps_time, 
          sizeof(xtra_state_ptr->gps_time), 
          p_gps_time,
          sizeof(*p_gps_time));
  TM_XTRA_MSG_HIGH("Current XTRA state time GpsWeek:%u gpsMs:%u",
	            xtra_state_ptr->gps_time.w_GpsWeek,
	             xtra_state_ptr->gps_time.q_GpsMsec,
	             0);

  /*-----------------------------------------------------------------------
      Process data validity check request
  -----------------------------------------------------------------------*/
  /* Data injection check request */
  if ( xtra_state_ptr->time_info_req_src & GPS_TIME_CHECK_SOURCE_DATA_INJECT )
  {
    tm_xtra_start_process_data_info(GPS_TIME_CHECK_SOURCE_DATA_INJECT);
  }

  /* Periodic data validity check request */
  if ( xtra_state_ptr->time_info_req_src & GPS_TIME_CHECK_SOURCE_PERIODIC )
  {
    tm_xtra_start_process_auto_dload_check(GPS_TIME_CHECK_SOURCE_PERIODIC);
  }
      
  /* Per fix data validity check request */
  if ( xtra_state_ptr->time_info_req_src & GPS_TIME_CHECK_SOURCE_PER_FIX )
  {
    tm_xtra_start_process_per_fix_check(GPS_TIME_CHECK_SOURCE_PER_FIX);
  }

  /* Time injection check request */
  if ( xtra_state_ptr->time_info_req_src & GPS_TIME_CHECK_SOURCE_TIME_INJECT )
  {
    tm_xtra_start_process_time_info(GPS_TIME_CHECK_SOURCE_TIME_INJECT);
  }
  /* Xtra config request  */
  if ( xtra_state_ptr->time_info_req_src & GPS_TIME_CHECK_SOURCE_CFG_REQ )
  {
    tm_xtra_start_process_query_cfg_info(GPS_TIME_CHECK_SOURCE_TIME_INJECT);
  }
  
  /*-----------------------------------------------------------------------
     Reset the time info req mask
  -----------------------------------------------------------------------*/
  xtra_state_ptr->time_info_req_src = GPS_TIME_CHECK_SOURCE_NONE;
  
  return;
} /* End tm_xtra_gps_time_report_handler()*/


/*===========================================================================
FUNCTION tm_xtra_data_validity_check

DESCRIPTION
Request MGP for GPS time, must wait for response from MGP before validating
the gpsOneXTRA data/time. Once the MGP provides the GPS time, TM-Core will
call tm_xtra_gps_time_report_handler().


INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_data_validity_check
(
  tm_xtra_data_check_source_e_type     source
)
{
  /* Local Variables */
  boolean                   ret_val = FALSE;
  lm_request_info_s_type    lm_info_req_param;

  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
    
  /* Determine if gpsOneXTRA is enabled
  */
  if( tm_xtra_is_enable() )
  {

    /* Save the current time info request source */
    xtra_state_ptr->time_info_req_src |= source;

    if(xtra_state_ptr->time_info_req_src != GPS_TIME_CHECK_SOURCE_NONE)
    {
      /*---------------------------------------------------------------------
        Request the current GPS time from MGP, place TM-XTRA in wait state
      ---------------------------------------------------------------------*/
      lm_info_req_param.e_req_type = LM_REQUEST_GNSS_TIME;
    
      lm_info_req_param.z_request.z_gps_time_request.e_time_chk_type = source; 
     
	  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GTS time request from source %d", source);
      /* Send request to get GTS time request */
      ret_val = tm_api_xtra_gts_time_request();
    }
    else
    {
      /* Time info request already sent to MGP */
      ret_val = TRUE;
    }
  }
  
  return ret_val;
} /* End tm_xtra_data_validity_check() */


/*===========================================================================
FUNCTION tm_xtra_inj_done

DESCRIPTION
This function will mark as an indication from CD of the completion of Xtra 
injection. Once this message is received, we send a trigger out to the 
client indication "Xtra injection completed" also stop the retry download 
timer and change the state of the system to TM_XTRA_STATE_IDLE
 
INPUT PARAMS
uint32  q_AcceptedMask

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
void tm_xtra_inj_done(uint32  q_AcceptedMask)
{
  tm_xtra_event_payload_u_type  event_payload;
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  // Only if SM is in the INJECT state then reset its state on 
  //receiving this ACK. 
  /* Clear the time of last download */
  qw_set(xtra_state_ptr->time_of_last_download, 0, 0);

  if(xtra_state_ptr->e_state == TM_XTRA_STATE_INJECT_DATA)
  {
    /*----------------------------------------------------------------------
     Stop Timers
    ----------------------------------------------------------------------*/  
    (void)os_TimerStop(xtra_state_ptr->retry_dload_timer);

    /* Update the session state */
    (void) tm_xtra_update_sess_state(TM_XTRA_STATE_IDLE);
    /* Send the status event to the client */
    event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_SUCCESS;
    event_payload.status_payload.value = q_AcceptedMask;
    
    (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS, 
                                 event_payload );

    /* Send gpsOneXTRA dload DM event */
    sm_report_event_xtra_end_dload(XTRA_DOWNLOAD_STATUS_SUCCESS);
  }
}

/*===========================================================================
FUNCTION tm_xtra_invoke_xtrat

DESCRIPTION
This function: 
  1- Verifies to see if it is ok to initaite an XTRA-t session
  2- If it is ok to start an XTRA-T session, it initaites a PUNC based
     XTRA-T session (If PUNC > 25 Km) XTRA-T session will be initiated
 
INPUT PARAMS
e_cmd_err - Status of xtra injection

OUTPUT PARAMS
None

RETURN VALUE 
 TRUE  - If the request for XTRA-T session is done
 FALSE - Otherwise
===========================================================================*/
boolean tm_xtra_invoke_xtrat(pdsm_lite_xtra_cmd_err_e_type e_cmd_err)
{
  boolean b_result = FALSE;
#ifdef FEATURE_CGPS_XTRA_T
  if((e_cmd_err != PDSM_LITE_XTRA_CMD_ERR_GPS_SUBSYSTEM_BUSY))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Initiate XTRA-T session.",
            0, 0, 0);
    b_result = tle_GetTerrPosStatus();
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Can not initiate XTRA-T session while, Assisted GPS session in progress",
             0, 0, 0);
  }
#endif /* FEATURE_CGPS_XTRA_T */
  return b_result;
}

/*===========================================================================

FUNCTION tm_xtra_update_version_check

DESCRIPTION
  This function is called by TM-PDAPI to inform XTRA module about XTRA version
  check type for verification purposes.

DEPENDENCIES

RETURN VALUE
  TRUE  - Version check is successful
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_xtra_update_version_check
(
  pdsm_lite_pa_xtra_ver_check_e_type xtra_version_check
  )
{
  tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();

  if (NULL == xtra_state_ptr)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);
    return FALSE;
  }

  /* Set the global variable for XTRA version check */
  xtra_state_ptr->version_check = xtra_version_check;

  return TRUE;
}
/*===========================================================================

FUNCTION tm_xtra_get_version_check

DESCRIPTION
  This function is called by TM-PDAPI to get XTRA version check enabled parameter
  for LBS.

DEPENDENCIES

RETURN VALUE
  pdsm_lite_pa_xtra_ver_check_e_type
    PDSM_LITE_PA_XTRA_VERSION_CHECK_DISABLED, XTRA Version Check Disabled
    PDSM_LITE_PA_XTRA_VERSION_CHECK_AUTO,     XTRA Version Check Automatic 
    PDSM_LITE_PA_XTRA_VERSION_CHECK_XTRA2,    XTRA Version Check with XTRA2 format 
    PDSM_LITE_PA_XTRA_VERSION_CHECK_XTRA3,    XTRA Version Check with XTRA3 format
    PDSM_LITE_PA_XTRA_VERSION_CHECK_MAX,      XTRA feature Disabled 

SIDE EFFECTS
  None

===========================================================================*/
pdsm_lite_pa_xtra_ver_check_e_type tm_xtra_get_version_check ()
{
  tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();

  if (NULL == xtra_state_ptr)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA STATE PTR found NULL",0,0,0);
    return PDSM_LITE_PA_XTRA_VERSION_CHECK_MAX;
  }

  /* return the global variable for XTRA version check */
  return (xtra_state_ptr->version_check);
}

/* Code to allow SM to write to EFS */
/* Move this code from below this line to new files named sm_efs.c and sm_efs.h   */
/* in the \modem_proc\gps\gnss\sm\common\src & \modem_proc\gps\gnss\sm\common\inc */
/* directories respectively.                                                      */
#include "fs_public.h"
#include "gps_fs_api.h"
#include "sm_api.h"

#if defined (WIN32) || defined (FEATURE_CGPS_PLAYBACK)

#include <io.h>

#ifdef O_CREAT
#error code not present
#endif
#define O_CREAT            0x100 

#ifdef S_IWRITE
#error code not present
#endif
#define S_IWRITE           0x180 

#ifdef O_BINARY
#error code not present
#endif
#define O_BINARY           0x8000 

extern char *smart_nv_location_prefix;

#endif /* end of #if defined (WIN32) || defined (FEATURE_CGPS_PLAYBACK) */

char *Xtra3RawFile = "Xtra3";
char *XtraIntRawFile = "Xtra3Int";

typedef struct
{
  uint32 q_Checksum;
  uint32 q_Version;                          /* Xtra Version */
  uint32 q_FileLen;                          /* Number of bytes pointed to by p_Data */
  uint8  *pu_Data;                            /* Pointer to a buffer containing file data. NULL if no data */
} sm_efs_xtra_file_type;

#define SM_EFS_PATH_SIZE 100

boolean sm_EfsGenerateFileIntegrityCheck( sm_efs_xtra_file_type *pz_EfsXtraFile, uint32 q_FileSize);
boolean sm_EfsFileIntegrityCheck(const U8 *pu_Data, U32 q_Size);
void sm_EfsXtraWriteNotification(sm_efs_xtra_file_type *pz_EfsXtraFile,
                                 const void *p_Data,
                                 uint32 q_Size);

/*
 ******************************************************************************
 * sm_EfsXtraWrite
 *
 * Function description:
 *
 * This function is used to write raw XTRA3/XTRA Integrity to EFS via GPS FS 
 * task. It does not actually write to EFS, but prepares the data to be posted
 * to GPS FS task.
 *
 * Parameters:
 *   e_Version (input)    - gnss_XtraVersionEnumType only supports XTRA3 and 
 *                          XTRA Integrity.
 *   pu_Data   (output)   - Pointer to the efs data.
 *
 * Return value:
 *   None
 *
 ******************************************************************************
*/
void sm_EfsXtraWrite( gnss_XtraVersionEnumType e_Version, uint8 *pu_Data, uint32 q_FileLen )
{
  sm_efs_xtra_file_type   z_EfsXtraFile = { 0 };
  char*              filename = NULL;
  char               pathname[SM_EFS_PATH_SIZE] = {0};
  struct fs_stat     file_stats;
  uint8              *pu_xtraTempbuf;

  /* Check for valid input parameters */
  if ( ( NULL == pu_Data ) || 
       ( 0 == q_FileLen ) || 
       ( (XTRA_VERSION_3 != e_Version) && (XTRA_VERSION_51 != e_Version) ) )
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: Invalid input parameters");
    return;
  }

  /* Set filename */
  if (XTRA_VERSION_3 == e_Version)
  {
    filename = Xtra3RawFile;
  }
  else if (XTRA_VERSION_51 == e_Version)
  {
    filename = XtraIntRawFile;
  }

  /* Valid filename */
#if defined (WIN32) || defined (FEATURE_CGPS_PLAYBACK)
  //(void)snprintf(pathname, sizeof(pathname), "GNSS\\SM\\%s", filename);   /* Valid filename */
  (void)snprintf(pathname, sizeof(pathname), "%s", filename);   /* Valid filename */
#else
  (void)snprintf(pathname, sizeof(pathname), "/GNSS/SM/%s", filename);   /* Valid filename */
#endif

  /* Get the file length */
  if ( efs_stat( pathname, &file_stats ) == 0 )
  {
    /* Add 4 bytes to q_FileLen to include checksum value */
    if( (q_FileLen + 4) != file_stats.st_size )
    {
#if defined (WIN32) || defined (FEATURE_CGPS_PLAYBACK)
      /* Provide full path of file. */
      memset((void*)pathname, 0, SM_EFS_PATH_SIZE);
      if (smart_nv_location_prefix != NULL)
      {
        GNSS_STRLCAT((char *)pathname, smart_nv_location_prefix, sizeof(pathname));
        GNSS_STRLCAT((char *)pathname, "\\", sizeof(pathname));
        GNSS_STRLCAT((char *)pathname, (const char *)filename, sizeof(pathname));
      }
      /* Had to add code above to allow __unlink call to work properly */
      if( _unlink(pathname) != 0 )
#else
      if( efs_unlink(pathname) != 0 )
#endif
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_EFS: File Delete Error");
      } 
    }
  }

  z_EfsXtraFile.q_Version = (uint32)e_Version;
  z_EfsXtraFile.q_FileLen = q_FileLen;
  /* May need to add more checks here */
  pu_xtraTempbuf = (uint8*)os_MemAlloc(q_FileLen + 4, OS_MEM_SCOPE_TASK);
  /* Check for memory allocation */
  if ( NULL == pu_xtraTempbuf )
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: Memory allocation failed");
    return;
  }
  memset((void *)pu_xtraTempbuf, 0x0,q_FileLen + 4);
  memscpy( (void *)(pu_xtraTempbuf+4), q_FileLen, pu_Data, q_FileLen);
  z_EfsXtraFile.pu_Data = pu_xtraTempbuf;

  /* Generate file integrity check: checksum for now */
  if (sm_EfsGenerateFileIntegrityCheck(&z_EfsXtraFile, q_FileLen))
  {
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_EFS: Checksum Successful");
     sm_EfsXtraWriteNotification( &z_EfsXtraFile,
                                 (const void *)z_EfsXtraFile.pu_Data,
                                  q_FileLen+4);
  }
  else
  {
     /* Nothing sent to GPS FS task, so make sure to free buffer */
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_EFS: Checksum Failed");
     os_MemFree((void **)&pu_xtraTempbuf);
  }
}  /* sm_EfsXtraWrite() */

/*
 ******************************************************************************
 * sm_EfsGenerateFileIntegrityCheck
 *
 * Function description:
 *
 * This functions generates integrity (now is checksum) info for an EFS file
 *
 * Parameters:
 *   pu_Data: data buffer, the first 4 bytes are reserved to store integrity
 *            check info. For now, only the 1st byte is used to store checksum
 *   q_Size: the size of data buffer.
 *
 * Return value: none
 *
 ******************************************************************************
*/
boolean sm_EfsGenerateFileIntegrityCheck( sm_efs_xtra_file_type *pz_EfsXtraFile, uint32 q_FileSize)
{
  boolean v_RetVal = FALSE;
  uint32  q_I;
  uint8   *pu_Data,*pu_StartData;

  if ((NULL == pz_EfsXtraFile) || 
      (0 == q_FileSize)        ||
      (NULL == pz_EfsXtraFile->pu_Data)) 
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: Invalid Input Parameters");
  }
  else
  {
    pu_Data = pu_StartData = pz_EfsXtraFile->pu_Data;
    pu_Data += 4;
    pz_EfsXtraFile->q_Checksum = 0;
    for (q_I=0; q_I < q_FileSize; ++q_I)
    {
      pz_EfsXtraFile->q_Checksum ^= (uint32)*pu_Data++;
    }
    memscpy((void *)pu_StartData,4,(const void *)&pz_EfsXtraFile->q_Checksum,4);
    v_RetVal = TRUE;
  }
  return v_RetVal;
}

/******************************************************************************
 * sm_EfsFileIntegrityCheck
 *
 * Function description:
 *
 * This functions checks the integrity (for now it is just checksum) info of an
 * EFS file
 *
 * Parameters:
 *   pu_Data: data buffer, the first 4 bytes are reserved to store integrity
 *            check info. For now, only the 1st byte is used to store checksum
 *   q_Size: the size of buffer, including 4-byte integrity check
 *
 * Return value:
 *   boolean: TRUE indicates check passes, FALSE indicates check failed.
 *
 ******************************************************************************
*/
boolean sm_EfsFileIntegrityCheck(const U8 *pu_Data, U32 q_Size)
{
  U32 q_I;
  U8  u_Sum;
  if ((q_Size <= 4)||(pu_Data == NULL))
  {
    return FALSE;
  }
  u_Sum = *pu_Data;
  pu_Data += 4;
  for (q_I=4; q_I < q_Size; ++q_I)
  {
    u_Sum ^= *pu_Data++;
  }
  return (u_Sum==0);
}

/*
 ******************************************************************************
 * sm_EfsXtraWriteNotification
 *
 * Function description:
 *
 * To send a message to THREAD_ID_GPS_FS that an Xtra file is to be written to 
 * EFS. The pointer to the cached data, the size of the data and the file to
 * be written are attached as the message payload. The actual write to EFS will
 * be conducted by the FS task, not this function. In playback, this function
 * will do the actual write
 *
 * Parameters: sm_efs_xtra_file_type - XTRA file type information 
 *             p_Data - pointer to data
 *             q_Size - size of data
 *
 * Return value: None
 *
 ******************************************************************************
*/
void sm_EfsXtraWriteNotification(sm_efs_xtra_file_type *pz_EfsXtraFile,
                                 const void *p_Data,
                                 uint32 q_Size)
{

#if !defined(FEATURE_CGPS_PLAYBACK)

  /* Don't try to send any message to the FS task in playback mode */
#ifdef FEATURE_CGPS_FILE_SYSTEM
  char u_string[256];
  os_IpcMsgType *pz_Msg;
  pz_Msg = os_IpcCreate( sizeof(gps_fs_u_cmd_type), IPC_ALLOCATION_DYNAMIC, THREAD_ID_GPS_FS );

  if (NULL != pz_Msg)
  {
    char* filename = NULL;
    gps_fs_u_cmd_type z_GpsFsCmd;
    memset( &z_GpsFsCmd, 0, sizeof(z_GpsFsCmd) );

    pz_Msg->q_MsgId = C_GPS_FS_MSG_SM_CMD;
    pz_Msg->q_DestThreadId = THREAD_ID_GPS_FS;
    pz_Msg->q_SrcThreadId = THREAD_ID_SM_TM;
    pz_Msg->q_Size = sizeof(gps_fs_u_cmd_type);

    /* Set filename */
    if (XTRA_VERSION_3 == pz_EfsXtraFile->q_Version)
    {
      filename = Xtra3RawFile;
    }
    else if (XTRA_VERSION_51 == pz_EfsXtraFile->q_Version)
    {
      filename = XtraIntRawFile;
    }

    /*** Check for valid DB file. ***/
    if ( filename == NULL )
    {
      if (os_IpcDelete(pz_Msg))
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: File not found");
      }
      else
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: EFS Unable to delete IPC msgs");
      }
    }
    else
    {
      z_GpsFsCmd.u_FileOP = GPS_FS_CREATE_WRITE_FILE;

      //u_Reserved fields not set by SM
#if 0 
      z_GpsFsCmd.u_Reserved[0] = 0;
      z_GpsFsCmd.u_Reserved[1] = 0;
      z_GpsFsCmd.u_Reserved[2] = 0;
      z_GpsFsCmd.u_Reserved[3] = 0;
#endif  /* #if 0 */

    #if !defined(WIN32)
      z_GpsFsCmd.u_FileNameLen = 
      snprintf((char *)(z_GpsFsCmd.p_FileName), sizeof(z_GpsFsCmd.p_FileName), "/GNSS/SM/%s", filename); 	 
    #else
      /* File path for SMART */
      z_GpsFsCmd.u_FileNameLen = 
      snprintf((char *)(z_GpsFsCmd.p_FileName), sizeof(z_GpsFsCmd.p_FileName), "CGPS_SM\\%s", filename);
    #endif

      z_GpsFsCmd.q_DataSize = q_Size;
      z_GpsFsCmd.p_DataPtr = (void *)p_Data;

      z_GpsFsCmd.gps_fs_efs_status_get = sm_GpsFsTaskNotification; //Add callback function here.

      memscpy((gps_fs_u_cmd_type *)pz_Msg->p_Data, pz_Msg->q_Size, 
               &z_GpsFsCmd, sizeof(z_GpsFsCmd));

      (void) snprintf(u_string, sizeof(u_string), "Sending SM message to GPS FS for file=%s, file size= %u",
                      z_GpsFsCmd.p_FileName,
                      (unsigned int)z_GpsFsCmd.q_DataSize);
      MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", u_string);

      if (!os_IpcSend( pz_Msg, THREAD_ID_GPS_FS ))
      {
        if(!os_IpcDelete(pz_Msg))
        {
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: EFS Unable to delete IPC msgs");
        }
      }

      /* Special debugging messages */
      if (XTRA_VERSION_3 == (gnss_XtraVersionEnumType)pz_EfsXtraFile->q_Version)
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_EFS: EFS WRITE: RAW XTRA3");
      }
      else if (XTRA_VERSION_51 == (gnss_XtraVersionEnumType)pz_EfsXtraFile->q_Version)
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_EFS: EFS WRITE: RAW XTRA Integrity");
      }

    }  /* end of else, i.e., filename != NULL */
  } /* end of if NULL != pz_Msg */
#endif /* end of #ifdef FEATURE_CGPS_FILE_SYSTEM */

#endif /* end of #if !defined (FEATURE_CGPS_PLAYBACK) */
}

/*
 ******************************************************************************
 * sm_EfsXtraRead
 *
 * Function description:
 *
 * This function reads a raw EFS Xtra file for XTRA3 and XTRA Integrity.
 *
 * Parameters:
 *   e_Version (input)    - gnss_XtraVersionEnumType only supports XTRA3 and 
 *                          XTRA Integrity.
 *   pu_Data   (output)   - Pointer to the efs data.
 *
 * Return value: Number of bytes read; 0 if reading fails
 *
 ******************************************************************************
*/
U32 sm_EfsXtraRead( gnss_XtraVersionEnumType e_Version, 
                    uint8* pu_Data)
{
  char*              filename;
  char               pathname[SM_EFS_PATH_SIZE] = {0};
  struct fs_stat     file_stats;
#if defined (WIN32) || defined (FEATURE_CGPS_PLAYBACK)
  FILE               *pFile;
  int                l_Result = EOF;
  size_t             q_Elements = 0;
#else
  int                fd = 0;
#endif
  U32                q_BytesRead = 0;
  fs_ssize_t         bytes_read_temp = 0;
  boolean            b_IsXtra = FALSE;

  //Only support XTRA3 and XTRA Integrity files
  if ((e_Version == XTRA_VERSION_3) ||
      (e_Version == XTRA_VERSION_51))
  {
    b_IsXtra    = TRUE;
  }

  if ((b_IsXtra != TRUE))
  {
    /* invalid parameters */
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: Invalid parameters prior to reading XTRA EFS file");
    return 0;
  }

  if(NULL == pu_Data)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: NULL buffer for raw XTRA EFS data passed in");
    return 0;
  }
  if (XTRA_VERSION_3 == e_Version)
  {
    filename = Xtra3RawFile;
  }
  else if (XTRA_VERSION_51 == e_Version)
  {
    filename = XtraIntRawFile;
  }

  /* Valid filename */
#if defined (WIN32) || defined (FEATURE_CGPS_PLAYBACK)
  //(void)snprintf(pathname, sizeof(pathname), "GNSS\\SM\\%s", filename);  /* Valid filename */
  (void)snprintf(pathname, sizeof(pathname), "CGPS_SM\\%s", filename);  /* Valid filename */
#else
  bytes_read_temp = snprintf(pathname, sizeof(pathname), "/GNSS/SM/%s", filename);   /* Valid filename */
  if(bytes_read_temp < 0)
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: Error (%d) formatting pathname for %s", 
      bytes_read_temp, filename);
    return 0;
  }
#endif

  /* Get the file length */
  if ( efs_stat( pathname, &file_stats ) < 0 )
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: Could not determine file length for %s",pathname);
    return 0;
  }
  /* File size is too small, then return error and possibly delete file. */
  if (file_stats.st_size <= 4) 
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: File Size (%u) is too small, so delete it",
          file_stats.st_size);
    /* Delete file */
    /* May want to delete directly instead of having GPS FS task do it */
    tm_xtra_delete_efs_data(e_Version);
    return 0;
  }

#if defined (WIN32) || defined (FEATURE_CGPS_PLAYBACK)
  memset(pathname, 0, SM_EFS_PATH_SIZE);
  /* Add path based on how efs_stat path is named. */
  if (smart_nv_location_prefix != NULL)
  {
    GNSS_STRLCAT((char *)pathname, smart_nv_location_prefix, sizeof(pathname));
    GNSS_STRLCAT((char *)pathname, "\\CGPS_SM\\", sizeof(pathname));
    GNSS_STRLCAT((char *)pathname, (const char *)filename, sizeof(pathname));
  }  
  //fd = _open (pathname,O_RDONLY|O_BINARY);
  pFile = fopen(pathname, "rb");
#else
  fd = efs_open (pathname,O_RDONLY);
#endif

#if defined (WIN32) || defined (FEATURE_CGPS_PLAYBACK)
  if (pFile == NULL)
#else
  if( fd < 0 )
#endif
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: File Open ERR for %s",pathname);
    return 0;
  }

  /* Malloc memory for pu_Data. */
  //*pu_Data = (U8 *)os_MemAlloc(file_stats.st_size, OS_MEM_SCOPE_TASK);
  if (NULL != pu_Data)
  {
#if defined (WIN32) || defined (FEATURE_CGPS_PLAYBACK)
    q_Elements = fread(pu_Data , sizeof(uint8), file_stats.st_size, pFile);
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_EFS: fread read:%u and requested:%u",
      q_Elements,file_stats.st_size);
    if (q_Elements != file_stats.st_size)
    { 
       MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_EFS: fread failed!");
       l_Result = fclose (pFile);
       /* if 0 is returned, then fclose was successful. */ 
       if (0 != l_Result)
       {
         MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: fclose failed err code:%d",l_Result);
         //os_MemFree((void **)&pu_Data);
       }
       return 0;
    }
    q_BytesRead = q_Elements;

	/* Perform integrity check (checksum) on data */
    if ( FALSE == sm_EfsFileIntegrityCheck(pu_Data, file_stats.st_size) )
    {
      /* If integrity check failed, then output debug msg */
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: EFS File %s corrupted. Integrity failed",pathname);
	  /* Reset the data buffer */
      memset(pu_Data, 0, file_stats.st_size);
	  return 0;
    }

    
#else
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_EFS: %u bytes allocated for read buffer",
      file_stats.st_size);
#endif 
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: failed to allocate read buffer");
    /* clean up on error */
#if defined (WIN32) || defined (FEATURE_CGPS_PLAYBACK)
    //if(_close(fd) !=0 )
    l_Result = fclose (pFile);
    /* if 0 is returned, then fclose was successful. */ 
    if (0 != l_Result)
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: fclose failed err code:%d",l_Result);
    }
#else
    if( efs_close(fd) !=0 )
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: File Close ERR: 0x%x.",fd);
    }
#endif
    return 0;
  }

#if !defined (WIN32) && !defined (FEATURE_CGPS_PLAYBACK)
  /* record file is readable.  File size == the buffer size */
  while ( q_BytesRead < file_stats.st_size )
  {
    bytes_read_temp = efs_read( fd, pu_Data+q_BytesRead, file_stats.st_size-q_BytesRead ); 
    if ( bytes_read_temp < 0 )
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: Unable to Read File: %s",pathname);

      if ( efs_close(fd) != 0 )
      {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: File Close ERR: 0x%x",fd);
      }
      memset(pu_Data, 0, file_stats.st_size);
      //os_MemFree((void **)&pu_Data);
      return 0;
    }

    /*lint -e{737} bytes_read_temp is ensured to be positive by the check above.
     * Can't change bytes_read to signed since it is compared to an unsigned in
     * the while loop, and that unsigned can't be changed to a signed.
    */
    q_BytesRead += (U32)bytes_read_temp;
  } // while ( bytes_read < file_size )

  /* Close file */
  if ( efs_close(fd) != 0 )
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: File Close ERR: 0x%x.",fd);
  }
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_EFS: Number of bytes read %d",
      q_BytesRead);

  if(q_BytesRead != file_stats.st_size)
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_EFS: Number of bytes read %d is nopt equal to file stat size",
    q_BytesRead, file_stats.st_size);
    return 0;
  }

  /* Perform integrity check (checksum) on data */
  if ( sm_EfsFileIntegrityCheck(pu_Data, file_stats.st_size) )
  {
    /* Data passed integrity check */
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_EFS: Integrity check passed !",
      0);
    return q_BytesRead;
  }

  /* If integrity check failed, then output debug msg */
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: EFS File %s corrupted.",pathname);

  /* Reset the data buffer */
  memset(pu_Data, 0, file_stats.st_size);

  /* Free memory */
  if(pu_Data != NULL)
  {
    //os_MemFree((void **)&pu_Data);
  }

  /* Delete the EFS file */
  /* May want to delete directly instead of having GPS FS task do it */
  //tm_xtra_delete_efs_data(e_Version);

  return 0;
#endif /* #if !defined (WIN32) && !defined (FEATURE_CGPS_PLAYBACK) */
  return q_BytesRead;
}

/*
 ******************************************************************************
 * tm_xtra_delete_efs_data
 *
 * Function description:
 *
 * This function sends a message to THREAD_ID_GPS_FS to notify that XTRA3 or
 * XTRA Integrity raw file shall be deleted from EFS. The to-be-deleted file-
 * name is attached as the message payload. This function does NOT delete any 
 * EFS data. The actual deletion is conducted by the GPS FS task.
 *
 * Parameters:
 *   e_Version (input) - gnss_XtraVersionEnumType only supports XTRA3 and 
 *                       XTRA Integrity.
 *
 * Return value: none
 *
 ******************************************************************************
*/
boolean tm_xtra_delete_efs_data(gnss_XtraVersionEnumType e_Version)
{
/* don't try to send any message to the FS task in playback mode */
#ifdef FEATURE_CGPS_FILE_SYSTEM
  boolean b_Ret = FALSE;
  char* filename = NULL;

  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  os_IpcMsgType *pz_Msg;

  if(NULL == xtra_state_ptr)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_xtra_delete_efs_data : NULL XTRA ptr");
    return FALSE;
  }


  /* Delete the decoded XTRA data from the common buffer */
  if(XTRA_VERSION_3 == e_Version)
  {    
    if(TRUE == tm_xtra_delete_decoded_xtra3_data())
    {
      xtra_state_ptr->b_xtradata_saved = FALSE;
    }
  }
  else if(XTRA_VERSION_51 == e_Version)
  {
    /* Delete the decoded XTRA integrity data in the common buffer */
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "tm_xtra_delete_efs_data : cleared away decoded XTRA integrity data");
    memset(&xtra_state_ptr->z_xtraint_struct, 0, sizeof(xtra_state_ptr->z_xtraint_struct));
    xtra_state_ptr->b_xtraint_saved = FALSE;
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_xtra_delete_efs_data : unsupported XTRA version");
    return FALSE;
  }

  pz_Msg = os_IpcCreate( sizeof(gps_fs_u_cmd_type), IPC_ALLOCATION_DYNAMIC, THREAD_ID_GPS_FS );

  if ( NULL != pz_Msg )
  {
    gps_fs_u_cmd_type z_GpsFsCmd;
    memset( &z_GpsFsCmd, 0, sizeof(z_GpsFsCmd) );

    pz_Msg->q_MsgId = C_GPS_FS_MSG_SM_CMD;
    pz_Msg->q_DestThreadId = THREAD_ID_GPS_FS;
    pz_Msg->q_SrcThreadId = THREAD_ID_SM_TM;
    pz_Msg->q_Size = sizeof(gps_fs_u_cmd_type);

    /* Set filename */
    if (XTRA_VERSION_3 == e_Version)
    {
      filename = Xtra3RawFile;
    }
    else if (XTRA_VERSION_51 == e_Version)
    {
      filename = XtraIntRawFile;
    }

    /*** Check for valid DB file. ***/
    if ( NULL == filename )
    {
      if (os_IpcDelete(pz_Msg))
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: File not found");
      }
      else
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: Unable to delete IPC msgs");
      }
      b_Ret = FALSE;
    }
    else
    {
      z_GpsFsCmd.u_FileOP = GPS_FS_DELETE_FILE;
      b_Ret = TRUE;

      #if !defined(WIN32) && !defined(FEATURE_CGPS_PLAYBACK)
      z_GpsFsCmd.u_FileNameLen = 
        snprintf((char *)(z_GpsFsCmd.p_FileName), sizeof(z_GpsFsCmd.p_FileName), "/GNSS/SM/%s", filename); 	 
      #else
      /* File path for SMART */
      z_GpsFsCmd.u_FileNameLen = 
      snprintf((char *)(z_GpsFsCmd.p_FileName), sizeof(z_GpsFsCmd.p_FileName), "CGPS_SM\\%s", filename);
      #endif

      z_GpsFsCmd.q_DataSize = 0;
      z_GpsFsCmd.p_DataPtr = NULL;

      z_GpsFsCmd.gps_fs_efs_status_get = sm_GpsFsTaskNotification; /* Add new callback function to send back notification */

      memscpy( (gps_fs_u_cmd_type *)pz_Msg->p_Data, pz_Msg->q_Size,
               &z_GpsFsCmd, sizeof(z_GpsFsCmd) );

      if (!os_IpcSend( pz_Msg, THREAD_ID_GPS_FS ))
      {
        if (!os_IpcDelete(pz_Msg))
        {
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: Unable to delete IPC msgs");
        }
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_EFS: Unable to send IPC for file deletion ");
        b_Ret = FALSE;
  }
  else
  {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_EFS: Send IPC for deletion of XTRA version %d", e_Version);
  }
    }  /* end of else, i.e. filename != NULL */
  }  /* end of if (NULL != pz_Msg) */
  return b_Ret;
#endif

  return FALSE;
}
void tm_xtra_notify_updated_gnss_constellation(uint8 constell_id)
{
 return;
}
#endif /* FEATURE_CGPS_XTRA */
void tm_xtra_efs_read(void)
{
 
  uint32 q_DataRead = 0;

  /* Local Variables */
  pdsm_xtra_status_e_type e_ret_val = XTRA_DOWNLOAD_STATUS_FAILURE;
  tm_xtra_version_e_type  e_XtraVersion = XTRA_VERSION_UNKNOWN;
  uint32                  q_DecodedMask = 0;

  tm_xtra_state_type      *pz_xtra_state =  tm_xtra_get_state_ptr();

  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/
  if (pz_xtra_state == NULL)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Cannot get XTRA state ptr during read");
    return;
  }

  if(0 != pz_xtra_state->q_XtraDataLen)
  {
    /* Temp XTRA buffer for storing RAW XTRA data is still in use and not released . So cannot read the XTRA from EFS 
	   into this temp buffer at this point of time */
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Temp XTRA buffer for storing RAW XTRA data is still in use .");
	return;
  }

    /* Memory buffer to store XTRA.BIN data from XTRA Client */
#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
  if (NULL == pz_xtra_state->xtraTempBuf)
  {
    /*  Allocate temporary buffer for gpsOneXTRA raw data */
    pz_xtra_state->xtraTempBuf = (uint8*)os_MemAlloc(TM_XTRA_MAX_FILE_SIZE, OS_MEM_SCOPE_TASK);
     
    if (NULL == pz_xtra_state->xtraTempBuf)
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"os_MemAlloc failed - out of memory");
      return;
    }
  }
  memset((void*)pz_xtra_state->xtraTempBuf,
         0x0,
         TM_XTRA_MAX_FILE_SIZE);

#else
   memset((void*)pz_xtra_state->xtraTempBuf,
           0x0,
           TM_XTRA_MAX_FILE_SIZE);
#endif  


  /* Read the XTRA data stored in EFS ( if any ) */
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Reading XTRA file from EFS");

  q_DataRead = sm_EfsXtraRead( XTRA_VERSION_3, pz_xtra_state->xtraTempBuf);
  if (q_DataRead == 0)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Failed EFS XTRA Read");
  } 
  else
  {
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,  "Reading Raw XTRA in EFS success!");

    /* Initialize the data length , the first 4 bytes are for XTRA data validity  */
    pz_xtra_state->q_XtraDataLen = q_DataRead - 4;


    /* Process and inject the gpsOneXTRA data file */
    e_ret_val = tm_xtra_process_and_inject_data( pz_xtra_state->xtraTempBuf+4, 
                                                 pz_xtra_state->q_XtraDataLen,
                                                 &e_XtraVersion,
                                                 &q_DecodedMask);


    if (XTRA_DOWNLOAD_STATUS_SUCCESS == e_ret_val)
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,  "Raw XTRA in EFS decoded succesfully! Saving decoded mask 0x%x", q_DecodedMask);    
      pz_xtra_state->q_xtra_decoded_mask = q_DecodedMask;
      pz_xtra_state->b_xtradata_saved = TRUE;
    }

  }


  /* Read the XTRA INT file stored in the EFS */

    memset((void*)pz_xtra_state->xtraTempBuf,
           0x0,
           TM_XTRA_MAX_FILE_SIZE);
  pz_xtra_state->q_XtraDataLen = 0;

  q_DataRead = sm_EfsXtraRead( XTRA_VERSION_51, pz_xtra_state->xtraTempBuf);
  if (q_DataRead == 0)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Failed EFS XTRA integrity Read");
  } 
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,  "Reading Raw XTRA Integrity in EFS success!");

	tm_xtra_debug_print_raw_data(pz_xtra_state->xtraTempBuf, q_DataRead);

    /* Initialize the data length , the first 4 bytes are for XTRA data validity  */
  pz_xtra_state->q_XtraDataLen = q_DataRead - 4;

  /* Process and inject the gpsOneXTRA data file */
  e_ret_val = tm_xtra_process_and_inject_data( pz_xtra_state->xtraTempBuf+4, 
                                               pz_xtra_state->q_XtraDataLen,
                                               &e_XtraVersion,
                                               &q_DecodedMask);

  if (XTRA_DOWNLOAD_STATUS_SUCCESS == e_ret_val)
  {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,  "Raw XTRA integrity in EFS decoded succesfully!");
    }
  }

	/* Memory buffer to store XTRA.BIN data from XTRA Client */
#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
    if (pz_xtra_state->xtraTempBuf != NULL)
    {
      os_MemFree((void**)&pz_xtra_state->xtraTempBuf);
      pz_xtra_state->xtraTempBuf = NULL;
    }
#else
    memset((void*)pz_xtra_state->xtraTempBuf,
           0x0,
           TM_XTRA_MAX_FILE_SIZE);
#endif


}


/*
 ******************************************************************************
 * tm_xtra_find_xtra_file_index_in_url
 *
 * Function description:
 *
 * This function is used to find the index of the xtra file name in the xtra file URL
 * Parameters:
 *   p_index : The index of the xtra file name 
 *   p_xtra_url : The XTRA data URL 
 *
 * Return value: TRUE if the operation succeeds , FALSE otherwise
 *
 ******************************************************************************
 */
boolean tm_xtra_find_xtra_file_index_in_url(uint32 *p_index, uint8 *xtra_url)
{
  uint32 i = 0;

  if((NULL == xtra_url)||(NULL == p_index))
  {
    return FALSE;
  }
  /* Remove the file name in the primary URL URL . This is later filled in based on the constellation info .  */
  i = GNSS_STRLEN((const char*)xtra_url);

  while((i > 0) && (xtra_url[i] != '/'))
  {
    i--;
  }

  i++;

  *p_index = i;

  return TRUE;
}
 /*
 ******************************************************************************
 * tm_xtra_set_data_server_url_for_constel
 *
 * Function description:
 *
 * This function is used to set the server URLs for the 2 selected constellations
 * The first constellation is always GPS . 
 * Parameters:
 *   None
 *
 * Return value: TRUE if the XTRA data was requested , FALSE otherwise.  
 *
 ******************************************************************************
*/
boolean tm_xtra_set_data_server_url_for_constel(sm_XtraConstelEnumType e_ConstelType)
{

  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();

  /* Primary gpsOneXTRA Server URL */
  uint8                                            primary_data_server_url[TM_XTRA_MAX_SERVER_URL_SIZE];
  /* Secondary gpsOneXTRA Server URL */
  uint8                                            secondary_data_server_url[TM_XTRA_MAX_SERVER_URL_SIZE];
  /* Tertiary gpsOneXTRA Server URL */
  uint8                                            tertiary_data_server_url[TM_XTRA_MAX_SERVER_URL_SIZE];

  uint32 i = 0;

  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/  
  if (xtra_state_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
    return FALSE;
  } 

  memset(primary_data_server_url, 0, sizeof(primary_data_server_url));

  memset(secondary_data_server_url, 0, sizeof(secondary_data_server_url));

  memset(tertiary_data_server_url, 0, sizeof(tertiary_data_server_url));

  /* Primary XTRA Server URL */
  memscpy((void *)&primary_data_server_url[0],sizeof(primary_data_server_url),
          xtra_state_ptr->primary_data_server_url, GNSS_ARRAY_SIZEOF(xtra_state_ptr->primary_data_server_url));
  /* Secondary XTRA Server URL */
  memscpy((void *)&secondary_data_server_url[0],sizeof(secondary_data_server_url),
          xtra_state_ptr->secondary_data_server_url, GNSS_ARRAY_SIZEOF(xtra_state_ptr->secondary_data_server_url));
  /* Tertiary gpsOneXTRA Server URL */
  memscpy((void *)&tertiary_data_server_url[0],sizeof(tertiary_data_server_url),
          xtra_state_ptr->tertiary_data_server_url, GNSS_ARRAY_SIZEOF(xtra_state_ptr->tertiary_data_server_url));

/*
REMOVE THIS BLOCK before promoting
  (void)sm_nv_read(NV_GPS1_XTRA_PRIMARY_SERVER_URL_I, (void *)&z_nv_read_buffer);
  memscpy((void *)&primary_data_server_url[0],
          sizeof(primary_data_server_url),
          xtra_state_ptr->primary_data_server_url,
          GNSS_ARRAY_SIZEOF(xtra_state_ptr->primary_data_server_url)));


  (void)sm_nv_read(NV_GPS1_XTRA_SECONDARY_SERVER_URL_I, (void *)&z_nv_read_buffer);
  memscpy((void *)&secondary_data_server_url[0],
          sizeof(secondary_data_server_url),
          (const void *)&z_nv_read_buffer.gps1_xtra_secondary_server_url[0],
          sizeof(z_nv_read_buffer.gps1_xtra_secondary_server_url));


  (void)sm_nv_read(NV_GPS1_XTRA_TERTIARY_SERVER_URL_I, (void *)&z_nv_read_buffer);
  memscpy((void *)&tertiary_data_server_url[0],
          sizeof(tertiary_data_server_url),
          (const void *)&z_nv_read_buffer.gps1_xtra_tertiary_server_url[0],
          sizeof(z_nv_read_buffer.gps1_xtra_tertiary_server_url));
  
*/

  if(FALSE == tm_xtra_find_xtra_file_index_in_url(&i, primary_data_server_url))
  {
    TM_XTRA_MSG_ERROR("Cannot get the primary XTRA URL",0,0,0);
    return FALSE;
  }
  /* At this point we need clear out the XTRA file name in the primary data URL . 
     It will be added when we get the constellation info */
  memset(&primary_data_server_url[i], 0, GNSS_STRLEN((const char*)primary_data_server_url)-i);

  i = 0;
  if(FALSE == tm_xtra_find_xtra_file_index_in_url(&i, secondary_data_server_url))
  {
    TM_XTRA_MSG_ERROR("Cannot get the secondary XTRA URL",0,0,0);
    return FALSE;
  }
  /* At this point we need clear out the XTRA file name in the secondary data URL . 
     It will be added when we get the constellation info */
  memset(&secondary_data_server_url[i], 0, GNSS_STRLEN((const char*)secondary_data_server_url)-i);

  i = 0;
  if(FALSE == tm_xtra_find_xtra_file_index_in_url(&i, tertiary_data_server_url))
  {
    TM_XTRA_MSG_ERROR("Cannot get the tertiary XTRA URL",0,0,0);
    return FALSE;
  }
  /* At this point we need clear out the XTRA file name in the tertiary data URL . 
     It will be added when we get the constellation info */
  memset(&tertiary_data_server_url[i], 0, GNSS_STRLEN((const char*)tertiary_data_server_url)-i);


 switch(e_ConstelType)
  {

    case SM_XTRA_CONSTEL_TYPE_GLO:
      /* Use the server data URL for GPS + GLO */

      /* Append the XTRA file name in primary data URL */
      GNSS_STRLCAT((char*)primary_data_server_url, "xtra3gr.bin", sizeof(primary_data_server_url));

      /* Append the XTRA file name in secondary data URL */
      GNSS_STRLCAT((char*)secondary_data_server_url, "xtra3gr.bin", sizeof(secondary_data_server_url));

      /* Append the XTRA file name in tertiary data URL */
      GNSS_STRLCAT((char*)tertiary_data_server_url, "xtra3gr.bin", sizeof(tertiary_data_server_url));
    break;

    case SM_XTRA_CONSTEL_TYPE_BDS:
      /* Use the server data URL for GPS + BDS */
      /* Append the XTRA file name in primary data URL */
      GNSS_STRLCAT((char*)primary_data_server_url, "xtra3gc.bin", sizeof(primary_data_server_url));

      /* Append the XTRA file name in secondary data URL */
      GNSS_STRLCAT((char*)secondary_data_server_url, "xtra3gc.bin", sizeof(secondary_data_server_url));

      /* Append the XTRA file name in tertiary data URL */
      GNSS_STRLCAT((char*)tertiary_data_server_url, "xtra3gc.bin", sizeof(tertiary_data_server_url));
    break;

    case SM_XTRA_CONSTEL_TYPE_GAL:
      /* Use the server data URL for GPS + GAL */
      /* Append the XTRA file name in primary data URL */
      GNSS_STRLCAT((char*)primary_data_server_url, "xtra3ge.bin", sizeof(primary_data_server_url));

      /* Append the XTRA file name in secondary data URL */
      GNSS_STRLCAT((char*)secondary_data_server_url, "xtra3ge.bin", sizeof(secondary_data_server_url));

      /* Append the XTRA file name in tertiary data URL */
      GNSS_STRLCAT((char*)tertiary_data_server_url, "xtra3ge.bin", sizeof(tertiary_data_server_url));
    break;

    case SM_XTRA_CONSTEL_TYPE_QZSS:
      /* Use the server data URL for GPS + QZSS */
      GNSS_STRLCAT((char*)primary_data_server_url, "xtra3gj.bin", sizeof(primary_data_server_url));

      /* Append the XTRA file name in secondary data URL */
      GNSS_STRLCAT((char*)secondary_data_server_url, "xtra3gj.bin", sizeof(secondary_data_server_url));

      /* Append the XTRA file name in tertiary data URL */
      GNSS_STRLCAT((char*)tertiary_data_server_url, "xtra3gj.bin", sizeof(tertiary_data_server_url));
    break;

    default :
      /* Use the server data URL for GPS only */
      GNSS_STRLCAT((char*)primary_data_server_url, "xtra3g.bin", sizeof(primary_data_server_url));

      /* Append the XTRA file name in secondary data URL */
      GNSS_STRLCAT((char*)secondary_data_server_url, "xtra3g.bin", sizeof(secondary_data_server_url));

      /* Append the XTRA file name in tertiary data URL */
      GNSS_STRLCAT((char*)tertiary_data_server_url, "xtra3g.bin", sizeof(tertiary_data_server_url));
	  break;
  }


  /* Copy the data server URL into the XTRA state */
  memscpy(xtra_state_ptr->primary_data_server_url,TM_XTRA_MAX_SERVER_URL_SIZE,
          primary_data_server_url, sizeof(primary_data_server_url));

  memscpy(xtra_state_ptr->secondary_data_server_url,TM_XTRA_MAX_SERVER_URL_SIZE,
          secondary_data_server_url, sizeof(secondary_data_server_url));

  memscpy(xtra_state_ptr->tertiary_data_server_url,TM_XTRA_MAX_SERVER_URL_SIZE,
          tertiary_data_server_url, sizeof(tertiary_data_server_url));

  TM_XTRA_MSG_HIGH("Modified XTRA primary data server URL ::",0,0,0);
  MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", xtra_state_ptr->primary_data_server_url);

  TM_XTRA_MSG_HIGH("Modified XTRA secondary data server URL ::",0,0,0);
  MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", xtra_state_ptr->secondary_data_server_url);

  TM_XTRA_MSG_HIGH("Modified XTRA tertiary data server URL ::",0,0,0);
  MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", xtra_state_ptr->tertiary_data_server_url);

  return TRUE;
}




 /*
 ******************************************************************************
 * tm_xtra_request_download
 *
 * Function description:
 *
 * This function is used to request for XTRA download from the HLOS. Note that 
 * this function does not do an existing XTRA validity check before requesting the 
 * download. It is the caller's responsibility to do the same . 
 * 
 * Parameters:
 *   None
 *
 * Return value: TRUE if the XTRA data was requested , FALSE otherwise.  
 *
 ******************************************************************************
*/
boolean tm_xtra_request_download(tm_xtra_data_check_result_e_type *p_ret_val)
{
  tm_xtra_event_payload_u_type      event_payload;
  
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();

  sm_XtraConstelEnumType e_ConstelType;
  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/  
  if ((xtra_state_ptr == NULL) || (NULL == p_ret_val))
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
    return FALSE;
  } 
  
  if( FALSE == tm_xtra_is_enable() )
  {
    TM_XTRA_MSG_ERROR("XTRA not enabled in NV ",0,0,0);
    return FALSE;
  }

  if ( tm_xtra_is_download_in_progress() || 
       tm_xtra_is_download_req_pending() )
  {
    TM_XTRA_MSG_HIGH("XTRA download is in progress or request is pending. No more XTRA requests ",0,0,0);
    return TRUE;
  }

  *p_ret_val = TM_XTRA_DATA_CHECK_ERROR;

  *p_ret_val = tm_xtra_data_check_handler(GPS_TIME_CHECK_SOURCE_PER_FIX);

  TM_XTRA_MSG_HIGH("tm_xtra_request_download:: tm_xtra_data_check_handler returned %d",*p_ret_val,0,0);


  /*-----------------------------------------------------------------------
    Send the XTRA download request to client if data expired
  -----------------------------------------------------------------------*/
  if ((*p_ret_val == TM_XTRA_DATA_CHECK_ERROR)||(*p_ret_val == TM_XTRA_DATA_CHECK_INVALID_TIME))
  {
    TM_XTRA_MSG_HIGH("XTRA data invalid/expired at time inject, request data download",0,0,0);

    event_payload.data_dload_payload.force_data_dload = FALSE;


    /* Before requesting XTRA download query the SM for the desired constellations we need to use */
    if(FALSE == sm_GetXtraConstelType(&e_ConstelType))
    {
      TM_XTRA_MSG_ERROR("Constellation info not available",0,0,0);
      return FALSE;
    }

    TM_XTRA_MSG_HIGH("Constellation type from SM :%d ",e_ConstelType, 0, 0);
    /* Set the server data URL for the constellations */
    tm_xtra_set_data_server_url_for_constel(e_ConstelType);

    /* Send the download request event, update the session state */
    if( TRUE ==
             tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_DATA_DOWNLOAD, event_payload ) 
      )
    {
      /* Send gpsOneXTRA dload DM event */
      sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_FIX_REQUEST);
        
      if( tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ) )
      {
        /* Start the recover timer */
        (void)os_TimerStart(xtra_state_ptr->retry_dload_timer,
                            (xtra_state_ptr->time_between_attempts*TM_XTRA_MSEC_IN_MIN), 
                            0);
      }
    }
    /* Since XTRA is requested at this point return TRUE */
    return TRUE;
  }// end if (*p_ret_val == TM_XTRA_DATA_CHECK_ERROR)

  /* XTRA was not requested */
  return FALSE;
}


 /*
 ******************************************************************************
 * tm_xtra_determine_data_age
 *
 * Function description:
 *
 * This function is used to determine the age of the  downloaded XTRA data . 
 * 
 * Parameters:
 *   w_GpsWeek : The GPS week 
 *   u_GpsMsec : GPS msec
 *   b_IsDataOld : Determines if the XTRA data is too old . 
 * Return value: TRUE if the APIis succesful , FALSE otherwise.  
 *
 ******************************************************************************
*/
boolean tm_xtra_determine_data_age(uint16 w_GpsWeek, uint32 u_GpsMsec, boolean *pb_IsDataOld)
{
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  tm_xtra_data_check_result_e_type ret_val = TM_XTRA_DATA_CHECK_ERROR; 
  double  d_xtraFileTime = 0 ;
  pdsm_lite_xtra_download_info_s_type  xtra_download_info = {0};
  uint16                            w_PrefAge = 0;
  char u_string[400];
  double                            d_EndGpsMsec = 0;
  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/  
  if ((xtra_state_ptr == NULL)||(NULL == pb_IsDataOld))
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
    return FALSE;
  }

  memset(&xtra_download_info,0,sizeof(xtra_download_info));
  if(FALSE == tm_xtra_set_xtra_time_and_validity(&xtra_download_info))
  {
    TM_XTRA_MSG_ERROR("Set_xtra_time_and_validity failed",0,0,0);
    return FALSE;
  }

  *pb_IsDataOld = TRUE;

  /* Get the preferred XTRA validity age from NV */
  tm_core_get_xtra_validity_age(&w_PrefAge);
  TM_XTRA_MSG_HIGH("Preferred Age NV Item is %d", w_PrefAge, 0, 0);

  if (( XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_WEEK & xtra_download_info.flags ) && 
      ( XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_MINS & xtra_download_info.flags ))
  {
    /** valid xtra file time **/
    double  d_xtraFileTime = 0, d_tempCurGpsTimeSec = 0;
    uint32   abs_age_hrs = 0xFFFF;

    /** if we current time is valid caluculate abs age **/
    if(w_GpsWeek != C_GPS_WEEK_UNKNOWN)
    {
      double d_curGpsTime = 0 , d_xtraFileAge=0, d_tempXtraStartTimeSec = 0;
        
      /** we have time we can caluculate abs age **/        
      d_curGpsTime    = TM_XTRA_CONVERT_GPS_FULL_MS(w_GpsWeek ,u_GpsMsec);
      d_xtraFileTime  = TM_XTRA_CONVERT_GPS_FULL_MS(xtra_download_info.startGpsWeek,
                                                    (xtra_download_info.startGpsMinutes * 60 * TM_XTRA_SEC_TO_MSEC) );

      d_EndGpsMsec    = d_xtraFileTime + (xtra_download_info.validDurationHours * TM_XTRA_MSEC_IN_HOUR);

      if ( d_curGpsTime > d_xtraFileTime )
      {
        abs_age_hrs = (uint32)((d_curGpsTime - d_xtraFileTime ) / TM_XTRA_MSEC_IN_HOUR);
      }
      d_tempCurGpsTimeSec = d_curGpsTime/1000;
      (void) snprintf(u_string, sizeof(u_string),
                      "Data age calc - curr GPS time(s): %0.3lf,XTRA file startGPSweek:%d, startGPSminutes:%d",
                       d_tempCurGpsTimeSec,
                       xtra_download_info.startGpsWeek,
                       xtra_download_info.startGpsMinutes);
      MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", u_string);

      d_tempXtraStartTimeSec = d_xtraFileTime/1000;

      (void) snprintf(u_string, sizeof(u_string),
                      "xtrafilestarttime(s):%0.3lf, xtrafileendtime(s):%0.3lf",
                       d_tempXtraStartTimeSec, d_EndGpsMsec/1000);
      MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", u_string);

      TM_XTRA_MSG_HIGH("Current Time is valid gpsWeek[%u] gpstime[%u] msec AbsAge[%u] hrs",w_GpsWeek,
                        u_GpsMsec,abs_age_hrs );

      if ((d_curGpsTime > d_EndGpsMsec) ||
          (abs_age_hrs > w_PrefAge))
      {
        /* XTRA data is very old */		
        *pb_IsDataOld = TRUE;
      }
      else
      {
        *pb_IsDataOld = FALSE;
      }
    }// end if(w_GpsWeek != C_GPS_WEEK_UNKNOWN)
  }// end if (( XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_WEEK & xtra_download_info.flags )

  return TRUE;
}



 /*
 ******************************************************************************
 * tm_xtra_process_avail_request
 *
 * Function description:
 *
 * This function is used to process XTRA availability request .
 * This function is used to check if the XTRA data is available . If available 
 * the XTRA data is validated using either GTS time or NTP time. 
 * If the XTRA data is not available or if the data is invalid , then this function requests for 
 * XTRA download .
 * 
 * Parameters:
 *   None
 *
 * Return value: None.
 *
 ******************************************************************************
*/
void tm_xtra_process_avail_request(boolean v_is_tunc_valid)
{
  tm_xtra_event_payload_u_type      event_payload;
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  tm_xtra_data_check_result_e_type ret_val = TM_XTRA_DATA_CHECK_ERROR; 
  gts_TimeType z_GtsTimeEst = {0};
  pdsm_xtra_download_info_s_type  xtra_download_info = {0};
  boolean b_is_data_old = FALSE;
  tm_xtra_data_check_result_e_type e_data_check = TM_XTRA_DATA_CHECK_ERROR;
  uint64 t_LastNtpInjectGpsTimeMs_Local = 0;
  gts_QtimeType z_GtsQTime = {0};
  boolean b_bypass_tunc = FALSE;
  gts_TimeType z_GtsTime = {0};


  TM_XTRA_MSG_HIGH("tm_xtra_process_avail_request called",0,0,0);

  if(TRUE == b_bypass_xtra_validity_check)
  {
    /* XTRA validity check is disabled by NV */
    TM_XTRA_MSG_HIGH("XTRA validity check disabled via NV", 0,0,0);

    if ( FALSE == tm_xtra_is_enable() )
    {
      tm_xtra_process_avail_request_done(TRUE);
      return;
    }
  }

  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/  
  if (xtra_state_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
	   tm_xtra_process_avail_request_done(FALSE);
    return;
  } 

  if(TRUE == xtra_state_ptr->b_xtra_process_avail_req)
  {
    TM_XTRA_MSG_ERROR("tm_xtra_process_avail_request: processing a previous request",0,0,0);
    return;
  }

    /* Find current time and then compare with last downloaded NTP time and then decide whether tunc check can be bypassed*/
  gts_GetQtime(&z_GtsQTime);
  if ( TRUE == z_GtsQTime.b_QtimeValid )
  {
    t_LastNtpInjectGpsTimeMs_Local = z_GtsQTime.t_Qtime / GTS_USTMR_PER_MS;
    
    if ((t_LastNtpInjectGpsTimeMs_Local > t_LastNtpInjectGpsTimeMs) && 
        ((t_LastNtpInjectGpsTimeMs_Local - t_LastNtpInjectGpsTimeMs) < TM_XTRA_DOWNLOADED_NTP_TIME_VALIDITY_PERIOD_IN_MS)
       )
    {
      /* make sure gts time is also available before bypassing NTP download */
      if ( (TRUE == gts_GetTime(&z_GtsTime)) && (TRUE == z_GtsTime.b_Valid) )
      {
        b_bypass_tunc = TRUE;
      }
    }
  }

  b_ntp_download_needed = FALSE;
  
  if((FALSE == v_is_tunc_valid) && (FALSE == b_bypass_tunc))
  {
    b_ntp_download_needed = TRUE;
    /* Force download the NTP time since TUNC is invalid . XTRA age is checked later
       when time is downloaded */
    TM_XTRA_MSG_HIGH("Force download the NTP time since TUNC is invalid",
    	                0,0,0);
    /*---------------------------------------------------------------------
         Send the gpsOneXTRA time info download request
    ---------------------------------------------------------------------*/      
    xtra_state_ptr->b_xtra_process_avail_req = TRUE;
    event_payload.time_dload_payload.force_time_info_dload = TRUE;

    (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_TIME, 
                                   event_payload );

    /* Send gpsOneXTRA dload DM event */
    sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_TIME_INJECTION);

    /* Start the xtra avail request timer */
    os_TimerStop(xtra_state_ptr->xtra_avail_request_timer);
    TM_XTRA_MSG_HIGH("tm_xtra_process_avail_request starting xtra avail request timer for %d secs",
	            TM_XTRA_AVAIL_REQUEST_TIME_WAIT_MSEC/1000,0,0);
    (void)os_TimerStart(xtra_state_ptr->xtra_avail_request_timer,
                       TM_XTRA_AVAIL_REQUEST_TIME_WAIT_MSEC, 
                        0);
    return;  
  }
  /* First check whether XTRA data is already available and decoded  */
  if(FALSE == tm_xtra_any_data_available())
  {
    TM_XTRA_MSG_ERROR("XTRA data is not valid . Checking if a download is needed",0,0,0);
    /* Request an XTRA download */
    xtra_state_ptr->b_xtra_process_avail_req = TRUE;
    if(FALSE == tm_xtra_request_download(&e_data_check))
    {
      xtra_state_ptr->b_xtra_process_avail_req = FALSE;
      TM_XTRA_MSG_ERROR("XTRA data cannot be requested due to check value %d",e_data_check,0,0);

      if(TM_XTRA_DATA_CHECK_SUCCESS == e_data_check)
      {
        /* XTRA data is available */
        tm_xtra_process_avail_request_done(TRUE);
      }
      else
      {
        tm_xtra_process_avail_request_done(FALSE);
      }

      /* Stop the xtra avail request timer */
      os_TimerStop(xtra_state_ptr->xtra_avail_request_timer);
      return;
    }
	/* Stop the xtra avail request timer */
	os_TimerStop(xtra_state_ptr->xtra_avail_request_timer);
    TM_XTRA_MSG_HIGH("tm_xtra_process_avail_request starting xtra avail request timer for %d secs",
		                TM_XTRA_AVAIL_REQUEST_TIME_WAIT_MSEC/1000,0,0);
	(void)os_TimerStart(xtra_state_ptr->xtra_avail_request_timer,
                       TM_XTRA_AVAIL_REQUEST_TIME_WAIT_MSEC, 
                        0);
    return;
  }

  /* Get the GTS time to calculate the age of XTRA data */
  memset(& z_GtsTimeEst, 0, sizeof(z_GtsTimeEst));
  if(TRUE == gts_GetTime(&z_GtsTimeEst))
  {
    if (TRUE == z_GtsTimeEst.b_Valid)
    {
      xtra_state_ptr->gps_time.w_GpsWeek = z_GtsTimeEst.w_GpsWeek;
      xtra_state_ptr->gps_time.q_GpsMsec = z_GtsTimeEst.q_GpsMsec;
      xtra_state_ptr->gps_time.f_ClkTimeBias = z_GtsTimeEst.f_ClkTimeBias;
      xtra_state_ptr->gps_time.f_ClkTimeUncMs = z_GtsTimeEst.f_ClkTimeUncMs;
      xtra_state_ptr->gps_time.e_SourceOfTime = z_GtsTimeEst.e_TimeSrc;
      TM_XTRA_MSG_HIGH("Got time from GTS GpsWeek:%u gpsMs:%u",z_GtsTimeEst.w_GpsWeek,z_GtsTimeEst.q_GpsMsec,0);
      TM_XTRA_MSG_HIGH("XTRA state time copied from GTS GpsWeek:%u gpsMs:%u",
                        xtra_state_ptr->gps_time.w_GpsWeek,
                        xtra_state_ptr->gps_time.q_GpsMsec,0);
      /* Calculate the age of XTRA data using the GTS time */
      /*-----------------------------------------------------------------------
        If the GPS time is unknown or time uncertainty is great than a threshold,
        we need time
       -----------------------------------------------------------------------*/
      if(TRUE == tm_xtra_determine_data_age(xtra_state_ptr->gps_time.w_GpsWeek, 
                                             xtra_state_ptr->gps_time.q_GpsMsec,
                                             &b_is_data_old))      
      {  
        if(TRUE == b_is_data_old)
        {
          /* XTRA download is requested */
          xtra_state_ptr->b_xtra_process_avail_req = TRUE;
          /* XTRA data is old and invalid */
          if(FALSE == tm_xtra_request_download(&e_data_check))
          {
             xtra_state_ptr->b_xtra_process_avail_req = FALSE;
             TM_XTRA_MSG_ERROR("XTRA data cannot be requested due to data check %d",e_data_check,0,0);
             if(TM_XTRA_DATA_CHECK_SUCCESS == e_data_check)
             {
               /* XTRA is available already */
               tm_xtra_process_avail_request_done(TRUE);
             }
             else
             {
               tm_xtra_process_avail_request_done(FALSE);
             }
          }/* end if(FALSE == tm_xtra_request_download(&e_data_check))     */
        }
        else
        {
          /* XTRA data is valid */
          tm_xtra_process_avail_request_done(TRUE);
        }
      }/* end of TRUE == tm_xtra_determine_data_age */
    }/* end if (TRUE == z_GtsTimeEst.b_Valid) */
    else
    {
      /* The time from GTS is invalid . Request the HLOS for NTP time.
         This is good because it seems time stored in GTS became invalid
         due to  passage of time. Even if aiding bypass may be set,
         allowing download of time in this case should be OK.
         This is because it should ideally never or rarely happen
         and in that case it may be good to restore valid time in GTS*/
      /*---------------------------------------------------------------------
         Send the gpsOneXTRA time info download request
      ---------------------------------------------------------------------*/      
      TM_XTRA_MSG_HIGH("GTS time invalid requesting NTP from HLOS",0,0,0);	

      xtra_state_ptr->b_xtra_process_avail_req = TRUE;
      event_payload.time_dload_payload.force_time_info_dload = TRUE;

      (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_TIME, 
                                   event_payload );

      /* Send gpsOneXTRA dload DM event */
      sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_TIME_INJECTION);
    }
  }/* if(TRUE == gts_GetTime(&z_GtsTimeEst)) */
  else
  {
    /* GTS failed to provide us the GPS time . but return success from here as both XTRA and NTP download is not needed */

    TM_XTRA_MSG_HIGH("GTS failed to provide time, but return sucesss as XTRA and time download not needed. tunc_valid= %d, bypass_tunc= %d",v_is_tunc_valid,b_bypass_tunc,0);

    tm_xtra_process_avail_request_done(TRUE);
  }

  if(TRUE == xtra_state_ptr->b_xtra_process_avail_req)
  {
    /* Start the xtra avail request timer */
    os_TimerStop(xtra_state_ptr->xtra_avail_request_timer);
    TM_XTRA_MSG_HIGH("tm_xtra_process_avail_request starting xtra avail request timer for %d secs",
	            TM_XTRA_AVAIL_REQUEST_TIME_WAIT_MSEC/1000,0,0);
    (void)os_TimerStart(xtra_state_ptr->xtra_avail_request_timer,
                       TM_XTRA_AVAIL_REQUEST_TIME_WAIT_MSEC, 
                        0);
  }
}



 /*
 ******************************************************************************
 * tm_xtra_process_avail_request_done
 *
 * Function description:
 *
 * This function is used to respond to the XTRA process availability request.
 * This informs the LM about the status off XTRA avilaibility 
 * Parameters:
 *   None
 *
 * Return value:  TRUE : XTRA is available and valid
 *                FALSE : XTRA could not be downloaded and is not available.
 *
 ******************************************************************************
*/
void tm_xtra_process_avail_request_done(boolean b_status)
{
  TM_XTRA_MSG_HIGH("tm_xtra_process_avail_request_done called with %d",b_status,0,0);
  tm_core_notify_xtra_process_done (b_status);
}




 /*
 ******************************************************************************
 * tm_xtra_handle_gts_time_request
 *
 * Function description:
 *
 * This function is used to handle GTS time request by XTRA module 
 * Parameters:
 *   None
 *
 * Return value:  None
 *
 ******************************************************************************
*/
void tm_xtra_handle_gts_time_request(void)
{
  gps_TimeStructType z_gps_time = {0};
  /* Time estimate from GTS */
  gts_TimeType z_GtsTimeEst = {0};

  z_gps_time.w_GpsWeek = C_GPS_WEEK_UNKNOWN;
  TM_XTRA_MSG_HIGH("tm_xtra_handle_gts_time_request",0,0,0);

  if(TRUE == gts_GetTime(&z_GtsTimeEst))
  {
    if (TRUE == z_GtsTimeEst.b_Valid)
    {
      z_gps_time.w_GpsWeek = z_GtsTimeEst.w_GpsWeek;
      z_gps_time.q_GpsMsec = z_GtsTimeEst.q_GpsMsec;
      z_gps_time.f_ClkTimeBias = z_GtsTimeEst.f_ClkTimeBias;
      z_gps_time.f_ClkTimeUncMs = z_GtsTimeEst.f_ClkTimeUncMs;
      z_gps_time.e_SourceOfTime = z_GtsTimeEst.e_TimeSrc;
      TM_XTRA_MSG_HIGH("Got time from GTS GpsWeek:%u gpsMs:%u",z_GtsTimeEst.w_GpsWeek,z_GtsTimeEst.q_GpsMsec,0);
    }
    else
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Invalid GPS time from GTS module");
    }
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"No GPS time from GTS module");
  }
  tm_xtra_gps_time_report_handler(&z_gps_time);
}


void tm_xtra_debug_print_raw_data(uint8 *pu_rawData, uint32 q_length)
{
  char b_Buf[100];
  uint32 i = 0, j = 0;
  uint32 q_iter = 0, q_rem = 0;

  memset(b_Buf, 0, sizeof(b_Buf));

  q_iter = q_length / 20;
  q_rem = q_length % 20;

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED,  "tm_xtra_debug_print_raw_data byte_length = %d \n", q_length);
  while ( j < q_iter )
  {
    memset(b_Buf, 0, sizeof(b_Buf));
    snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
	         pu_rawData[i],
	         pu_rawData[i+1],
	         pu_rawData[i+2],
                 pu_rawData[i+3],
		 pu_rawData[i+4],
		 pu_rawData[i+5],
		 pu_rawData[i+6],
		 pu_rawData[i+7],
		 pu_rawData[i+8],
		 pu_rawData[i+9],
                 pu_rawData[i+10],
                 pu_rawData[i+11],
                 pu_rawData[i+12],
                 pu_rawData[i+13],
                 pu_rawData[i+14],
                 pu_rawData[i+15],
                 pu_rawData[i+16],
                 pu_rawData[i+17],
                 pu_rawData[i+18],
                 pu_rawData[i+19]);	
    i = i+20;	
    /* commit the log */
    MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
    j++;
  }
  memset(b_Buf, 0, sizeof(b_Buf));
  /* Print the remaining bytes */
  switch(q_rem)
  {
    case 19: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n", 
                 pu_rawData[i],
	         pu_rawData[i+1],
                 pu_rawData[i+2],
                 pu_rawData[i+3],
                 pu_rawData[i+4],
                 pu_rawData[i+5],
                 pu_rawData[i+6],
                 pu_rawData[i+7],
                 pu_rawData[i+8],
                 pu_rawData[i+9],
                 pu_rawData[i+10],
                 pu_rawData[i+11],
                 pu_rawData[i+12],
                 pu_rawData[i+13],
                 pu_rawData[i+14],
                 pu_rawData[i+15],
                 pu_rawData[i+16],
                 pu_rawData[i+17],
                 pu_rawData[i+18]);
                /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
		 break;
    case 18: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
	         pu_rawData[i],
	         pu_rawData[i+1],
		 pu_rawData[i+2],
		 pu_rawData[i+3],
		 pu_rawData[i+4],
		 pu_rawData[i+5],
		 pu_rawData[i+6],
		 pu_rawData[i+7],
		 pu_rawData[i+8],
		 pu_rawData[i+9],
                 pu_rawData[i+10],
                 pu_rawData[i+11],
                 pu_rawData[i+12],
                 pu_rawData[i+13],
                 pu_rawData[i+14],
                 pu_rawData[i+15],
                 pu_rawData[i+16],
                 pu_rawData[i+17]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
		 break;
    case 17: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
	         pu_rawData[i],
	         pu_rawData[i+1],
		 pu_rawData[i+2],
		 pu_rawData[i+3],
		 pu_rawData[i+4],
		 pu_rawData[i+5],
		 pu_rawData[i+6],
		 pu_rawData[i+7],
		 pu_rawData[i+8],
		 pu_rawData[i+9],
                 pu_rawData[i+10],
                 pu_rawData[i+11],
                 pu_rawData[i+12],
                 pu_rawData[i+13],
                 pu_rawData[i+14],
                 pu_rawData[i+15],
                 pu_rawData[i+16]);
	         /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
		 break;
    case 16: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
	         pu_rawData[i],
	         pu_rawData[i+1],
	  	 pu_rawData[i+2],
		 pu_rawData[i+3],
		 pu_rawData[i+4],
		 pu_rawData[i+5],
		 pu_rawData[i+6],
		 pu_rawData[i+7],
		 pu_rawData[i+8],
		 pu_rawData[i+9],
                 pu_rawData[i+10],
                 pu_rawData[i+11],
                 pu_rawData[i+12],
                 pu_rawData[i+13],
                 pu_rawData[i+14],
                 pu_rawData[i+15]);
	         /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
		 break;
      case 15: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
		     pu_rawData[i],
	             pu_rawData[i+1],
	             pu_rawData[i+2],
	             pu_rawData[i+3],
                     pu_rawData[i+4],
	             pu_rawData[i+5],
	             pu_rawData[i+6],
	             pu_rawData[i+7],
	             pu_rawData[i+8],
	             pu_rawData[i+9],
                     pu_rawData[i+10],
                     pu_rawData[i+11],
                     pu_rawData[i+12],
                     pu_rawData[i+13],
                     pu_rawData[i+14]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
		 break;
      case 14: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
		     pu_rawData[i],
	         pu_rawData[i+1],
			 pu_rawData[i+2],
			 pu_rawData[i+3],
			 pu_rawData[i+4],
			 pu_rawData[i+5],
			 pu_rawData[i+6],
			 pu_rawData[i+7],
			 pu_rawData[i+8],
			 pu_rawData[i+9],
             pu_rawData[i+10],
             pu_rawData[i+11],
             pu_rawData[i+12],
             pu_rawData[i+13]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
			 break;
      case 13: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
		     pu_rawData[i],
	         pu_rawData[i+1],
			 pu_rawData[i+2],
			 pu_rawData[i+3],
			 pu_rawData[i+4],
			 pu_rawData[i+5],
			 pu_rawData[i+6],
			 pu_rawData[i+7],
			 pu_rawData[i+8],
			 pu_rawData[i+9],
             pu_rawData[i+10],
             pu_rawData[i+11],
             pu_rawData[i+12]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
			 break;
       case 12: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
		     pu_rawData[i],
	         pu_rawData[i+1],
			 pu_rawData[i+2],
			 pu_rawData[i+3],
			 pu_rawData[i+4],
			 pu_rawData[i+5],
			 pu_rawData[i+6],
			 pu_rawData[i+7],
			 pu_rawData[i+8],
			 pu_rawData[i+9],
             pu_rawData[i+10],
             pu_rawData[i+11]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
			 break;
      case 11: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
		     pu_rawData[i],
	         pu_rawData[i+1],
			 pu_rawData[i+2],
			 pu_rawData[i+3],
			 pu_rawData[i+4],
			 pu_rawData[i+5],
			 pu_rawData[i+6],
			 pu_rawData[i+7],
			 pu_rawData[i+8],
			 pu_rawData[i+9],
             pu_rawData[i+10]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
			 break;
      case 10: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
		     pu_rawData[i],
	         pu_rawData[i+1],
			 pu_rawData[i+2],
			 pu_rawData[i+3],
			 pu_rawData[i+4],
			 pu_rawData[i+5],
			 pu_rawData[i+6],
			 pu_rawData[i+7],
			 pu_rawData[i+8],
			 pu_rawData[i+9]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
			 break;
      case 9: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
		     pu_rawData[i],
	         pu_rawData[i+1],
			 pu_rawData[i+2],
			 pu_rawData[i+3],
			 pu_rawData[i+4],
			 pu_rawData[i+5],
			 pu_rawData[i+6],
			 pu_rawData[i+7],
			 pu_rawData[i+8]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
			 break;
      case 8: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x %02x %02x\n", 
		     pu_rawData[i],
	         pu_rawData[i+1],
			 pu_rawData[i+2],
			 pu_rawData[i+3],
			 pu_rawData[i+4],
			 pu_rawData[i+5],
			 pu_rawData[i+6],
			 pu_rawData[i+7]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
			 break;
      case 7: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x %02x\n", 
		     pu_rawData[i],
	         pu_rawData[i+1],
			 pu_rawData[i+2],
			 pu_rawData[i+3],
			 pu_rawData[i+4],
			 pu_rawData[i+5],
			 pu_rawData[i+6]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
			 break;
      case 6: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x %02x\n", 
		     pu_rawData[i],
	         pu_rawData[i+1],
			 pu_rawData[i+2],
			 pu_rawData[i+3],
			 pu_rawData[i+4],
			 pu_rawData[i+5]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
			 break;
      case 5: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x %02x\n", 
		     pu_rawData[i],
	         pu_rawData[i+1],
			 pu_rawData[i+2],
			 pu_rawData[i+3],
			 pu_rawData[i+4]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
			 break;
      case 4: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x %02x\n", 
		     pu_rawData[i],
	         pu_rawData[i+1],
			 pu_rawData[i+2],
			 pu_rawData[i+3]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
			 break;
      case 3: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x %02x\n", 
		     pu_rawData[i],
	         pu_rawData[i+1],
			 pu_rawData[i+2]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
			 break;
      case 2: snprintf(b_Buf, sizeof(b_Buf), "%02x %02x\n", 
		     pu_rawData[i],
	         pu_rawData[i+1]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
			 break;
      case 1: snprintf(b_Buf, sizeof(b_Buf), "%02x\n", 
		     pu_rawData[i]);
		     /* commit the log */
	         MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buf);
			 break;
			 default:
				 break;
  }



}


/*
 ******************************************************************************
 * tm_xtra_save_decoded_xtra_integrity_data
 *
 * Function description:
 *
 * This function is used to save the decoded XTRA integrity data inside TM
 *   None
 *
 * Return value:  None
 *
 ******************************************************************************
*/
void tm_xtra_save_decoded_xtra_integrity_data(gnss_XtraIntTmStructType *pz_xtraintStruct)
{
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  
  if((NULL == xtra_state_ptr)||(NULL == pz_xtraintStruct))
{
  return;
}

  memscpy(&xtra_state_ptr->z_xtraint_struct, sizeof(xtra_state_ptr->z_xtraint_struct),
	      pz_xtraintStruct, sizeof(*pz_xtraintStruct));
  TM_XTRA_MSG_HIGH("tm_xtra_save_decoded_xtra_integrity_data: Saved %d bytes",
	              sizeof(*pz_xtraintStruct),0,0);
  xtra_state_ptr->b_xtraint_saved = TRUE;
}


 /*
 ******************************************************************************
 * tm_xtra_handle_inject_to_mgp_request
 *
 * Function description:
 *
 * This function is used to handle XTRA injection to MGP request
 * Parameters:
 *   None
 *
 * Return value:  None
 *
 ******************************************************************************
*/
void tm_xtra_handle_inject_to_mgp_request(void)
{
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();

  if(NULL == xtra_state_ptr)
  {
    return;
  }

  /* Check if the saved XTRA is valid */
  
  if(TRUE == xtra_state_ptr->b_xtradata_saved)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_xtra_handle_inject_to_mgp_request: posting XTRA data ind to LM");
    lm_send_xtra_done_msg(xtra_state_ptr->q_xtra_decoded_mask);
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"tm_xtra_handle_inject_to_mgp_request: XTRA data not saved. Nothing to inject");
  }


  if(TRUE == xtra_state_ptr->b_xtraint_saved)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_xtra_handle_inject_to_mgp_request: posting Integrity data to LM");
    lm_send_xtraintegrity_msg(&xtra_state_ptr->z_xtraint_struct);
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_xtra_handle_inject_to_mgp_request: Integrity data not saved / invalid");
  }

}

 /*
 ******************************************************************************
 * tm_xtra_is_valid_xtra_available
 *
 * Function description:
 *
 * This function is used to know if valid xtra is available.
 * Parameters:
 *   None
 *
 * Return value:  boolean
 *
 ******************************************************************************
*/
boolean tm_xtra_is_valid_xtra_available(void)
{
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  return(xtra_state_ptr != NULL ? (TRUE == xtra_state_ptr->b_xtradata_saved) : FALSE);
}
