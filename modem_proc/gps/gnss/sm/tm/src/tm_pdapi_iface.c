/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM - PD API Interface

General Description
  This file contains implementations for TM Diag Interface
  Copyright (c) 2002 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2017 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2020 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History
  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/src/tm_pdapi_iface.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/21   sai     Change to correct sys uncertainity call and also to handle UNC Invalid state  
10/01/20   ska     Changes done for MGP state to OFF when TBF>=5sec
10/15/19   ska     Moved os_MemAlloc for event_payload_ptr to if case where it used in tm_core_handle_set_param()
10/14/19   ska     Added fix to force MGP to off when there is explicit loc stop request
08/06/19   py      set TM session state early to not allow multiple internal sessions.
04/01/19   nath    Added debug NMEA related code.
12/14/17   mj      Block sessions during SARF 
08/03/17   ss       Dont override E911 session if internal session starts
05/22/17   ak      Force standalone mode for MSB apptracking comes when in concurrent mode.
03/21/17   gk      Allow Xtra injection during 911 
01/11/17   kpn     Allow end session request while Emergency call is ongoing for NV1930 value of 2
12/09/16   mj      Update PD event report to "PDSM_LITE_PD_CMD_EMERGENCY_CALL_IN_PROGRESS_S" during E911
10/26/16   nl      SET/ GET mismatch fix for nmea_ext_sentence_mask 
04/27/16   rn      Updated tm_core_efs_file_resp_handler to include filename for EFS Quota.                   
04/02/16   mj      GERA support
03/16/16   gk      LPPe changes
02/17/16   mj      Added NV support to enable/disable CPI injections and early exit logic 
12/08/15   gk      Allow pdsm_lite_get_best_pos during a E911 to support VoWiFi Calling
09/16/15   skm     Xtra Version check fix 
07/03/15   jv      Added QZSS Support
06/29/15   rk      Remove unwanted protocol callback upon PDSM_LITE_PA_ASST_GLONASS_POS_PROTOCOL PA event change.
05/28/15   skm     Allow SET command for Premium services during ongoing session
05/26/15   jv      SBAS user preference changed from bool to uint8
04/06/15   skm     Modify "tm_core_param_check" to make it a general PARAM check (SET and GET) function  
03/14/15   rh      Added support for MultiSIM NV 
01/14/15   rk      Added LPP RSTD >24 measurement processing support 
01/08/15   skm     Return End event (NO_SESS) when session end called, when there is not active session.  
05/12/14   ah      Added support for Set XTRA Version Check
04/28/14   ah      Added support for handling premium service SAP disabled
04/17/14   skm     NV Async Read changes 
03/10/14   ah      Added support for Set Premium Services Cfg
01/30/14   ss      Allowing Set Param when Geofencing Session is running
11/13/13   rk      Missing initialization of pd_event_type causes sending garbage data to LBS.
10/03/12   rh      Added a few PA commands for SUPL configuration  
06/15/12   rk      Added NV set parameter support for PDSM_LITE_PA_ASST_GLONASS_POS_PROTOCOL PA event.
05/29/12   ssu     Removed tm_core_handle_best_avail_pos  
04/18/12   ssu     Added tm_core_handle_best_avail_pos  
06/02/12   gk      LPP config support
06/03/11   gk      Clear cached position  when delete param or gps lock is sent
06/02/11   rh      Added PDSM_LITE_LCS_CMD_SET_VX_CONFIG and Get/Set SUPL security
12/17/10   rc      Changed PDSM_LITE_PA_EFS_DATA to support root certificate injection w/o reboot
09/30/10   rh      Changed ERR_OTHER to more meaningful error codes
04/09/10   atien   enable standalone in E911 when camped on G/W for Ecall
04/09/10   atien   Correct possible null pointer dereference
03/03/10   ns      DSDS Support. Force supl cert read from efs if pdapi injects
                   new certificate
1/12/10    gk      send the efs write pa event to pdapi
09/30/09   gk     Support ODP 2.0
09/03/09   gk      validate the EFS file name length etc
08/11/09   gk      set mt flag correctly
07/26/09   atien   debug message update


07/23/09   gk      Support ODP 2.0
07/13/09   gk      process end session command even when no TM session
06/30/09   atien   Allow PDSM_LITE_CLIENT_TYPE_OEM to abort active session
6/29/09    gk      Send PD_EVENT call back when supl certificate is written.
2/18/09    gk      Client Id is getting reset in some cases.
03/06/09   atien   add missing case for NV write callback for MOLR vs AD Call Flow Select mask update
03/04/09   ns      Modified input param to tm_core_get_gps_state
03/02/09   jd      Added support for Wiper logging over RPC.
02/18/09   gk      Client Id is getting reset in some cases.
02/04/09   atien   MOLR vs AD Call Flow Select support
01/13/08   atien   XTRA-T Support
11/07/08   ank     For QoS=0, reject MS-B and Standalone only fix requests.
09/18/08   cl      Only allow dload/fix session overlapping 1x systems
09/3/08    gk      Fixed compiler warning
08/26/08   gk      On-Demand GPS Lock changes
08/13/08   gk      On-Demand force receiver off changes
08/08/08   cl      Enable non-encrypted lat/long when security is activated
07/25/08   gk      On-Demand Changes
04/25/08   gk      On-Demand Changes
04/09/08   cl      Allow Opt mode when phone is out of services
03/14/08   lt      END and DONE events added if protocol modules reject START.
03/13/08   cl      allow standalone to bypass security check
01/21/08   cl      Allow overlapping dload and fix (fix and dload) sessions
11/13/07   gk      Add support for SUPL ceritificate injection thorugh PDAPI
11/12/07   cl      Reject AGPS request while MS is OoS
09/14/07   gk      NMEA CONFIG set parameters support
09/06/07   gk      undo previous checkin. not valid for this branch
09/06/07   gk      Save the MO method when set via PDAPI
08/30/07   cl      Allow optimal mode when MS is in OoS
08/01/07   rw      Add support of XTRA feature
07/30/07   gk      Add handling for last position
05/27/07   rw      Add handling for app info from client
05/23/07   gk      added checks for end session
04/25/07   gk      Fixed the pa event type to match the new structure definition
04/03/07   gk      Add function for Time injection handling
01/31/07   cl      Modified function for LCS handling
12/07/06   cl      Added set/get parameter handling
12/06/06   gk      renamed set parm struct
09/15/06   cl      Initail Release
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE
// rev1
===========================================================================*/

#include <fs_errno.h>
#include "gps_variation.h"
#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */
#include <math.h>
#include <string.h>
#include "msg.h"
#include "aries_os_api.h"
#include "tm_pdapi_iface.h"
#include "tm_pdapi_client.h"
#include "tm_cm_iface.h"
#include "tm_data.h"
#include "sm_nv.h"
#include "aries_os_globals.h"
#include "gps_fs_api.h"
#include "pdapibuf.h"
#include "mgp_api.h"
#include "tm_nv.h"
#include "sm_oemdre.h"
#include "lm_data.h"
#ifdef FEATURE_CGPS_XTRA
#include "tm_xtra.h"
#endif /* FEATURE_CGPS_XTRA */
#ifdef FEATURE_CGPS_XTRA_T
#include "tle_api.h"
#include "tm_xtra_t.h"
#endif /* FEATURE_CGPS_XTRA_T */

#include "gm_api.h"
#include "tm_datamanager_api.h"

#ifdef FEATURE_CGPS_XTRA_T
#define XTRA_T_HANDSHAKE_KEY_0 0x41
#define XTRA_T_HANDSHAKE_KEY_1 0x52
#define XTRA_T_HANDSHAKE_KEY_2 0x54
#define XTRA_T_HANDSHAKE_KEY_3 0x58
#define tm_core_get_active_prtl_module TM_PRTL_TYPE_NONE
uint8   xtra_t_key_idx = 0;
#define XTRA_T_RCVD_KEY_MAX (4)
uint8   xtra_t_rcvd_keys[XTRA_T_RCVD_KEY_MAX+1];
#endif /* FEATURE_CGPS_XTRA_T */
extern pdsm_lite_pd_event_type tm_util_xlate_fixt_report_to_qmi_loc_pos_rpt_Ind(sm_FixInfoStructType *lr_data,
                                                        qmiLocEventPositionReportIndMsgT_v02* pz_PositionReportIndMsg , 
                                                        prtl_sess_info_e_type orig_sess_info_type );

extern boolean lm_is_mgp_active(void);
extern boolean tm_core_start_gnss_session(void);
extern boolean tm_core_propogate_cached_gnss_position(void);
extern boolean tm_core_req_gtp_wwan_fix_from_cache();

extern lm_dmm_state_e_type lm_get_dmm_state(void);
extern tm_core_system_unc_state tm_core_req_gts_for_sys_unc(boolean is_first_attempt, boolean* v_is_tunc_valid);
/*===========================================================================
                 Data Definition
===========================================================================*/

/* Flag to check if SM-NV init has been called */
boolean tm_nv_initialized = FALSE;

/* Handle used for SM NV write */
uint32  tm_nv_rdrw_handle = SM_NV_CLIENT_HANDLE_ERROR;
nv_item_type  *tm_pa_nv_write_buffer=NULL;
nv_item_type  *tm_pa_nv_read_buffer=NULL;

static gps_fs_u_cmd_type z_GpsFsCmd;

/* Temp variable to support API changes */
static boolean v_gnss_monitor_cfg = FALSE;

static tm_pdapi_pd_event_s_type  pd_event_type = { 0 };


/*===========================================================================

FUNCTION tm_core_get_pos_para_check 
  This function is called before starting a standalone or a UP session to get
  a GPS position. We need to check whether the get_position request can be
  handled by the position engine by checking the parameters passed and the 
  state of the GPS engine.

DESCRIPTION

DEPENDENCIES

RETURN VALUE  Return error code or success

SIDE EFFECTS

===========================================================================*/
pdsm_lite_pd_cmd_err_e_type tm_core_get_pos_para_check
(
   const tm_pdapi_get_pos_param_s_type *get_pos_param_ptr
   )
{
  tm_cm_phone_state_info_s_type phone_state_info;

  /* Perform necessary check to see if incoming get pos request should be granted */

  /* First check if the task is active */
  if (tm_core_info.tm_task_info.task_active == FALSE)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Task is not active", 0, 0, 0);
    return PDSM_LITE_PD_CMD_ERR_OFFLINE_S;
  }

  /***************************************************************************
   * Perform E911 related checks (E911 call orgination, E911 CBW, ...etc     *
   ***************************************************************************/
  tm_cm_iface_get_phone_state(&phone_state_info);

   if ((TM_CORE_CONFIG_DD(gps_lock) == PDSM_LITE_GPS_LOCK_MI)
      || (TM_CORE_CONFIG_DD(gps_lock) == PDSM_LITE_GPS_LOCK_ALL))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "MI GPS is locked", 0, 0, 0);
    return PDSM_LITE_PD_CMD_ERR_GPS_LOCK_S;
  }

  /* ayt_todo - make sure no code change is need to prevent MT-LR in LPP CP */

  /* [LPP CP FRS] LPP CP supports only NI-LR but not MO-LR 
   * 
   * If in LTE, LPP CP feature enabled then
   *    Block MO Get Postion request with the following conditions:
   *      1. if MO Method is Control Plane
   *      2. if Operation Mode NOT Standalone
   *
   *   (ayt_todo - skip this restriction for now)
   *    If MO-CP and Standalone and
   *       Make sure of all GNSSs , only GPS is supported. 
   * Endif
   */
 #if 0//Todo: tm_core_get_lpp_cp_enable not supported for 9x05:
  if ((phone_state_info.srv_system == CGPS_SRV_SYS_LTE) &&
      (tm_core_get_lpp_cp_enable() == TRUE))
  {
    if ((get_pos_param_ptr != NULL) &&
        (get_pos_param_ptr->option_ptr != NULL))
    {
      if (get_pos_param_ptr->option_ptr->operation_mode != PDSM_LITE_SESSION_OPERATION_STANDALONE_ONLY)
      {
        return PDSM_LITE_PD_CMD_ERR_FEATURE_NOT_ENABLED;
      }
      /*  skip this restriction for now
              if (get_pos_param_ptr->option_ptr->mode_info.mode != PDSM_LITE_SESS_JGPS_TYPE_GPS)
              {
                return PDSM_LITE_PD_CMD_ERR_FEATURE_NOT_ENABLED;
              }
      */
    }
  }
 #endif
  /* Block any session while RF testing in progress */
  if (TRUE == tm_core_get_sarf_status())
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "SARF (RF testing) in progress, reject position request");
    return PDSM_LITE_PD_CMD_ERR_BUSY_S;
  }

  /* MO service interaction checking */
  if ((get_pos_param_ptr != NULL)
      && (get_pos_param_ptr->option_ptr != NULL)
      && (get_pos_param_ptr->qos_ptr != NULL))
  {
      if (tm_core_session_active()
          && (tm_core_is_internal_session_on() == FALSE)
          )
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Fix session is in progress", 0, 0, 0);
        return PDSM_LITE_PD_CMD_ERR_BUSY_S;
      }
    if ((get_pos_param_ptr->option_ptr->operation_mode == PDSM_LITE_SESSION_OPERATION_STANDALONE_ONLY) &&
        (get_pos_param_ptr->qos_ptr->gps_session_timeout == 0))
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "QoS timeout 0 for MS-B/Standalone", 0, 0, 0);
      return PDSM_LITE_PD_CMD_ERR_QOS_P;
    }
  }
  else /* NI service interaction checking */
  {
    if (tm_core_session_active()
        && (tm_core_info.internal_session == FALSE)
        )
    {
      return PDSM_LITE_PD_CMD_ERR_BUSY_S;
    }
  }

  /* Need CM interface to complete this check, for now always allow incoming request */
  return PDSM_LITE_PD_CMD_ERR_NOERR;
}

/*===========================================================================

FUNCTION tm_core_resume_get_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_resume_get_pos(void)
{
  int                               prtl_tbl_idx = -1;
  tm_prtl_type                      active_prtl;
  tm_prtl_type                      orig_active_prtl;
  prtl_start_sess_req_result_e_type reqResult;
  tm_sess_req_start_param_s_type    start_actions;
  boolean                           retVal = FALSE;
  tm_sess_handle_type               mo_sess_handle;
  tm_pdapi_get_pos_param_s_type     get_pos_param;

  /* Init */
  memset(&pd_event_type, 0, sizeof(pd_event_type));


  tm_core_info.session_info.queued_request = FALSE;
  get_pos_param.client_id   = tm_core_info.session_info.queued_get_pos_param.client_id;
  get_pos_param.client_type = tm_core_info.session_info.queued_get_pos_param.client_type;
  get_pos_param.option_ptr  = &(tm_core_info.session_info.queued_get_pos_param.option);
  get_pos_param.qos_ptr     = &(tm_core_info.session_info.queued_get_pos_param.qos);
  tm_core_info.internal_session = tm_core_info.session_info.queued_get_pos_param.internal_session;

  /* if the get pos request is last position, return a fix if availalbe.
    otherwise return error
    */
  /* If a get pos request is
     1. Fix session
     2. MSB or Standalone
     Let standalone handles it first */
  if (get_pos_param.option_ptr->operation_mode == PDSM_LITE_SESSION_OPERATION_STANDALONE_ONLY)
    {
      {
        active_prtl = TM_PRTL_TYPE_STANDALONE;
        prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);
      }

    orig_active_prtl = active_prtl; /* Save original prtl as it may get overwritten later. */
  }
  else
  {
    active_prtl =TM_PRTL_TYPE_NONE;//  TODO:[an][modify] check tm_core_get_active_prtl_module def at top;
    orig_active_prtl = active_prtl; /* Save original prtl as it may get overwritten later. */

    prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);

  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core dispatches get_pos req to [%d]", active_prtl, 0, 0);

  if ((prtl_tbl_idx >= 0) && (prtl_tbl_idx < TM_PRTL_NUM))
  {
    mo_sess_handle = tm_core_new_mo_sess_handle();

    if (tm_core_info.prtl_func_cb_table[prtl_tbl_idx].start_sess_req_fp)
    {
        tm_core_info.session_info.pd_param.client_id   = get_pos_param.client_id;
        tm_core_info.session_info.pd_param.client_type = get_pos_param.client_type;
        memscpy((void *)&(tm_core_info.session_info.pd_param.pd_option),
                sizeof(tm_core_info.session_info.pd_param.pd_option),
                (void *)get_pos_param.option_ptr, sizeof(*get_pos_param.option_ptr));
        memscpy((void *)&(tm_core_info.session_info.pd_param.pd_qos), sizeof(tm_core_info.session_info.pd_param.pd_qos),
                (void *)get_pos_param.qos_ptr, sizeof(*get_pos_param.qos_ptr));

        reqResult = tm_core_info.prtl_func_cb_table[prtl_tbl_idx].start_sess_req_fp(
           mo_sess_handle,
           &(tm_core_info.session_info.pd_param),
           &start_actions);

      if (reqResult == PRTL_START_SESS_REQ_ALLOWED)
      {
        /* If prototocol return PRTL_START_SESS_REQ_ALLOWED, call tmCore_SessReqStart */
        retVal = tm_core_sess_req_start(active_prtl,
                                        mo_sess_handle,
                                        start_actions);

       /* tech sel not supported
        tm_core_report_sess_start_to_techsel(&start_actions, mo_sess_handle);
       */
      }
      else
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Protocol sub-module rejects start session request", 0, 0, 0);
        retVal = FALSE;
      }
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid prtlTableIndex", 0, 0, 0);
      retVal = FALSE;
    }
  }
  else
  {

    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid prtlTableIndex", 0, 0, 0);
    retVal = FALSE;
  }

  if (retVal == FALSE)
  {
      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506  !e730 */
    }

  return retVal;
  }


/*===========================================================================

FUNCTION tm_core_handle_get_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_get_pos(const tm_pdapi_get_pos_param_s_type *get_pos_param_ptr)
{
  int                               prtl_tbl_idx = -1;
  tm_prtl_type                      active_prtl;
  tm_prtl_type                      orig_active_prtl;
  prtl_start_sess_req_result_e_type reqResult;
  tm_sess_req_start_param_s_type    start_actions;
  boolean                           retVal = FALSE;
  tm_sess_handle_type               mo_sess_handle;
  boolean                           v_is_tunc_valid = TRUE;

  /* Init */
  memset(&pd_event_type, 0, sizeof(pd_event_type));

  if(TM_CORE_CONFIG_DD(gps_lock) != PDSM_LITE_GPS_LOCK_NONE)
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "GPS is locked, Status : %d. Session not allowed", (TM_CORE_CONFIG_DD(gps_lock)));
    return FALSE;
  }

  if ((get_pos_param_ptr == NULL)
      || (get_pos_param_ptr->option_ptr == NULL)
      || (get_pos_param_ptr->qos_ptr == NULL))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null get_pos_param_ptr", 0, 0, 0);
    return FALSE;
  }
  if (TRUE == tm_core_get_sarf_status())
  {
    if (TRUE == get_pos_param_ptr->internal_session)
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Internal sessions not allowed during SARF(RF testing)");
      return FALSE;
    }
  }

  if (tm_core_is_internal_session_on() == TRUE)
  {
    /* For following case,Abort the ongoing internal session
         Case: Incoming sess. req type:External, ongoing session type:internal;
         Case: Incoming sess. req type:Internal, ongoing session type:internal;
    */
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Aborting on-going internal session", 0, 0, 0);
    tm_core_abort_recover(TRUE, TM_STOP_REASON_ABORT_INTERNAL_SESSION);
  }
  else if(tm_core_info.session_info.session_state != TM_CORE_SESS_STATE_INIT)
  {
    /*For following case, donot process incoming sess req as already session going on TM.

              Incoming ses req type:Internal, ongoing session type:external;
              Incoming ses req type:External, ongoing session type:external;
     */
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Overlapping session", 0, 0, 0);
      return FALSE;
  }

  /*[9x05][APP] iff the requested session is not an internal session. Indicate PDAPI that
  session is starting*/
  if( (get_pos_param_ptr->internal_session == FALSE))
  {
    /* Fire Begin event */
    pd_event_type.e_pd_event = PDSM_PD_EVENT_BEGIN;
    pd_event_type.pd_event_data.pd_info_type.client_id = get_pos_param_ptr->client_id;
    pd_event_type.pd_event_data.pd_get_pos_data = *get_pos_param_ptr;
    
    tm_pdapi_pd_event_callback(&pd_event_type, TM_PDAPI_SESS_TYPE_MI);
    }

  /*[9x05][APP] Copy all the parameters from requested session*/
  tm_core_info.session_info.pd_param.client_id   = get_pos_param_ptr->client_id;
  tm_core_info.session_info.pd_param.client_type = get_pos_param_ptr->client_type;

  memscpy((void *)&(tm_core_info.session_info.pd_param.pd_option),
          sizeof(tm_core_info.session_info.pd_param.pd_option),
          (void *)get_pos_param_ptr->option_ptr, sizeof(*get_pos_param_ptr->option_ptr));
  memscpy((void *)&(tm_core_info.session_info.pd_param.pd_qos),
          sizeof(tm_core_info.session_info.pd_param.pd_qos),
          (void *)get_pos_param_ptr->qos_ptr, sizeof(*get_pos_param_ptr->qos_ptr));

  if(get_pos_param_ptr->option_ptr->operation_mode == PDSM_LITE_SESSION_OPERATION_STANDALONE_ONLY)
  {
    tm_core_info.session_info.op_mode = TM_SESS_OPERATION_MODE_STANDALONE;
  }

  /* All the params check are passed now to start a session, so now we can update TM session state*/
  tm_core_info.internal_session = get_pos_param_ptr->internal_session;
  tm_core_update_session_state(TM_CORE_SESS_STATE_ACCESS_GRANTED);        
  
  if(FALSE == lm_is_mgp_active())
  {
    /*Try propogating cached Gnss fix*/
    if(TRUE   == tm_core_propogate_cached_gnss_position())
    {
      retVal = TRUE;
    }
    else if(TM_CORE_CONFIG_IS_GTP_WWAN_AVAIL())
    {
      /*Request GTP WWAn for cached fix*/
      tm_core_req_gtp_wwan_fix_from_cache();
      retVal  = TRUE;
    }
    else
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "GTP WWAN no available, Proceed with Sys Unc check");

      /* if sync unc doesnt succeed, a retry will happen after sync unc retry timer expired*/
      if(TM_CORE_SYSTEM_UNC_VALID == tm_core_req_gts_for_sys_unc(TRUE, &v_is_tunc_valid))
      {
        tm_core_handover_to_xtra(v_is_tunc_valid);
        retVal  = TRUE;
      }
    }
  }
  /*Starting GNSS session*/
  else if(tm_core_start_gnss_session())
  {
    retVal = TRUE;
  }
  return retVal;
}
/*===========================================================================

FUNCTION tm_core_handle_use_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

/*===========================================================================

FUNCTION tm_core_end_session_para_check

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_lite_pd_cmd_err_e_type tm_core_end_session_para_check
(
   const tm_pdapi_end_session_param_s_type  *end_session_param_ptr
   )
{
  tm_cm_phone_state_info_s_type phone_state_info;
  (void)end_session_param_ptr;

  /* Perform necessary check to see if incoming end pos request should be granted */

  /* First check if the task is active */
  if (tm_core_info.tm_task_info.task_active == FALSE)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Task is not active", 0, 0, 0);
    return PDSM_LITE_PD_CMD_ERR_OFFLINE_S;
  }
  /* Perform E911 related checks (E911 call orgination, E911 CBW, ...etc */
  tm_cm_iface_get_phone_state(&phone_state_info);

  /* Then check if any there is any active PD session */
  if (!tm_core_session_active())
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "No Fix session is in progress. Send cancellation to the protocols.", 0, 0, 0);
    //return PDSM_LITE_PD_CMD_ERR_NO_SESS_S;
  }

  return PDSM_LITE_PD_CMD_ERR_NOERR;
}


/*===========================================================================

FUNCTION tm_core_handle_end_session

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_end_session
(
   const tm_pdapi_end_session_param_s_type  *end_session_param_ptr
   )
{
  boolean                           retVal = FALSE;
  pdsm_lite_client_type_e_type           client_type = PDSM_LITE_CLIENT_TYPE_NONE;

  if (end_session_param_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null end_session_param_ptr", 0, 0, 0);
    return FALSE;
  }

  /* Init */
  memset(&pd_event_type, 0, sizeof(pd_event_type));

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,
        "End Session Request Client_id = %d, Session type = %d Receiver off = %d",
        end_session_param_ptr->client_id,0,
        end_session_param_ptr->receiver_off);

  client_type = pdsmclient_lite_get_client_type_map(end_session_param_ptr->client_id);
//TODO: [an][del] if OEM not needed, del
  if ((end_session_param_ptr->client_id == tm_core_info.session_info.pd_param.client_id) ||
	  (client_type == PDSM_LITE_CLIENT_TYPE_OEM) ||
	  (end_session_param_ptr->receiver_off == TRUE))
  {
	  /* Check if the receiver off is needed */
	  if (end_session_param_ptr->receiver_off == TRUE)
	  {
		 tm_core_info.session_info.receiver_off_needed = TRUE;
	  }
	  tm_core_info.session_info.pd_param.client_id = end_session_param_ptr->client_id;
	  tm_core_abort_recover(TRUE, TM_STOP_REASON_USER_TERMINATED);
	  retVal = TRUE; /*lint !e506 */
  }
  else
  {
	  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Client (%d) does not have an Active session, Send END Event", end_session_param_ptr->client_id, 0, 0);
	  pd_event_type.pd_event_data.pd_info_type.client_id = end_session_param_ptr->client_id;
	  pd_event_type.e_pd_event = PDSM_LITE_PD_EVENT_END;
	  pd_event_type.pd_event_data.pd_info_type.pd_info.end_status = PDSM_LITE_PD_END_CLIENT_HAS_NO_SESS;
	  tm_pdapi_pd_event_callback(&pd_event_type, TM_PDAPI_SESS_TYPE_NONE);
	  retVal = FALSE;
  }
  return retVal;
}

/*===========================================================================

FUNCTION tm_core_handle_inject_ext_time

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_inject_ext_time
(
   pdsm_lite_pd_external_time_info_type *ext_time_ptr
   )
{
  if (ext_time_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null input pointer", 0, 0, 0);
    return FALSE;
  }

  tm_core_inject_ext_time(ext_time_ptr);
  return TRUE; /*lint !e506 */
}


/*===========================================================================

FUNCTION tm_core_handle_inject_ext_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_inject_ext_pos
(
   pdsm_lite_cmd_s_type *p_cmd_type
   )
{
  if (p_cmd_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null input pointer", 0, 0, 0);
    return FALSE;
  }
  tm_core_inject_ext_pos(p_cmd_type);

  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_core_get_gps_state_para_check

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_lite_pd_cmd_err_e_type tm_core_get_gps_state_err_check
(
   )
{
  tm_cm_phone_state_info_s_type phone_state_info;

  /* Perform necessary check to see if incoming end pos request should be granted */

  /* First check if the task is active */
  if (tm_core_info.tm_task_info.task_active == FALSE)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Task is not active", 0, 0, 0);
    return PDSM_LITE_PD_CMD_ERR_OFFLINE_S;
  }

  /* Perform E911 related checks (E911 call orgination, E911 CBW, ...etc */
  tm_cm_iface_get_phone_state(&phone_state_info);

  /* Then check if any there is any active PD session */
  if (tm_core_session_active())
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Fix session is in progress", 0, 0, 0);
  }


  if (tm_core_info.e_cached_rcvr_state != C_GPS_RCVR_STATE_OFF)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Fix session is in progress. RCVR is not OFF", 0, 0, 0);
  }
  return PDSM_LITE_PD_CMD_ERR_NOERR;
}



/*===========================================================================

FUNCTION tm_core_get_best_avail_pos_err_check

DESCRIPTION 
This function does error checks to see if the command can be processed 

DEPENDENCIES
None 
 
RETURN VALUE
PD CMD ERROR if the command cannot be processed. NO_ERR otherwise. 
 
SIDE EFFECTS

===========================================================================*/
pdsm_lite_pd_cmd_err_e_type tm_core_get_best_avail_pos_err_check(void)
{
  /* First check if the task is active */
  if (tm_core_info.tm_task_info.task_active == FALSE)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Task is not active", 0, 0, 0);
    return PDSM_LITE_PD_CMD_ERR_OFFLINE_S;
  }
  return PDSM_LITE_PD_CMD_ERR_NOERR;
}


/*===========================================================================

FUNCTION tm_core_handle_get_gps_state

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_get_gps_state
(
   pdsm_lite_cmd_s_type *p_cmd_type
   )
{
  tm_core_get_gps_state_param_s_type get_gps_state_param;

  if (p_cmd_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null input pointer", 0, 0, 0);
    return FALSE;
  }

  get_gps_state_param.source = SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE;
  get_gps_state_param.cmd.p_cmd_type = p_cmd_type;
  tm_core_get_gps_state(&get_gps_state_param);
  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_core_param_always_allowed

DESCRIPTION: Allow commands to go through based on type of command and on-going sessio
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static boolean tm_core_param_always_allowed(pdsm_lite_pa_e_type e_pa_type, 
                                                      pdsm_lite_pa_cmd_e_type pa_cmd,
                                                      tm_pdapi_param_s_type *param_ptr)
{
  boolean ret_val =  FALSE;

  /* Allow following set command to go through regardless session state */
  if ((e_pa_type == PDSM_LITE_PA_XTRAT_CLIENT_TOKEN) ||
      (e_pa_type == PDSM_LITE_PA_XTRAT_USER_SESSION_CONTROL) ||
      (e_pa_type == PDSM_LITE_PA_DPO_CONTROL) ||
      (e_pa_type == PDSM_LITE_PA_XTRA_APN_PROFILE) ||
      (e_pa_type == PDSM_LITE_PA_UAX3_CONFIG)
      )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Always allow Set parameter for %d", e_pa_type, 0, 0);
    ret_val = TRUE;
  }
  /* Always let an UNLOCK ALL, or UNLOCK MO through. All other lock commands need to 
   * pass other requirements of no E911 and no active session */
  else if((e_pa_type == PDSM_PA_GPS_LOCK) &&
          ( ((param_ptr->pa_info_type_ptr->gps_lock == PDSM_GPS_LOCK_NONE) &&
             (TM_CORE_CONFIG_DD( gps_lock ) != PDSM_GPS_LOCK_NONE)) 
            || 
            ((param_ptr->pa_info_type_ptr->gps_lock == PDSM_GPS_LOCK_MT) &&
             (TM_CORE_CONFIG_DD( gps_lock ) != PDSM_GPS_LOCK_MT))
          )
     )
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Always allow Unlock(%d) from lock state (%d)", 
          param_ptr->pa_info_type_ptr->gps_lock, TM_CORE_CONFIG_DD( gps_lock ));
    ret_val = TRUE;
  }

  return ret_val;
}

/*===========================================================================

FUNCTION tm_core_param_check

DESCRIPTION
  This function decides if a PA command is allowed or not based on the type of command and 
  if there is an ongoing session in progress 
DEPENDENCIES
 
PARAMETERS
 set_param_ptr: Pointer to SET/GET PARAM command structure
 pa_cmd       : PA command Type

RETURN VALUE
  TRUE: The requested PA command is allowed to execute
  FALSE: The requested PA command is denied
  
SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_param_check(tm_pdapi_param_s_type *param_ptr, pdsm_lite_pa_cmd_e_type pa_cmd)
{
  boolean retVal = FALSE;
  tm_cm_phone_state_info_s_type  phone_state_info = { 0 };

  tm_cm_iface_get_phone_state(&phone_state_info);

  if (param_ptr == NULL)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL ptr passed to tm_core_param_check");
    return FALSE;
  }

  /* First check if the task is active */
  if (tm_core_info.tm_task_info.task_active == FALSE)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Task is not active");
  }
  else if (tm_core_param_always_allowed(param_ptr->e_pa_type, pa_cmd, param_ptr))
  {
    retVal = TRUE;
  }
  else
  {
    retVal = TRUE; /*lint !e506 */
  }

return retVal;
}

/*===========================================================================

FUNCTION tm_core_handle_get_param

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_get_param(tm_pdapi_param_s_type *get_param_ptr)
{
  dword             portID             = 0;
  uint32            uConfigInfo        = 0;
  uint8             bConfigByte        = 0;

  if (get_param_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null set_param_ptr", 0, 0, 0);
    return (FALSE);
  }
  else
  {
    switch (get_param_ptr->e_pa_type)
    {
      case PDSM_LITE_PA_GPS_LOCK:

        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Getting GPS Lock Setting", 0, 0, 0);
      tm_pa_nv_read_buffer = (nv_item_type*)os_MemAlloc(sizeof(nv_item_type), OS_MEM_SCOPE_TASK); 
       if (tm_pa_nv_read_buffer == NULL)
        {
       MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory");
       return FALSE;
        }
       (void)sm_nv_read_ex(NV_GPS1_LOCK_I, (void *)tm_pa_nv_read_buffer);
        get_param_ptr->pa_info_type_ptr->gps_lock = (pdsm_lite_gps_lock_e_type)tm_pa_nv_read_buffer->gps1_lock;
      os_MemFree((void * *) &tm_pa_nv_read_buffer);
        return (TRUE); /*lint !e506 */

      case PDSM_LITE_PA_DPO_CONTROL:
        {
          uint8 u_DpoControl = 0;

          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Getting DPO Control Setting", 0, 0, 0);
          if(LM_DMM_STATE_LOADED == lm_get_dmm_state())
          {
            mgp_GetDpoControlStatus(&u_DpoControl, THREAD_ID_SM_TM);
          }
          get_param_ptr->pa_info_type_ptr->dpo_control = u_DpoControl;
          return (TRUE); /*lint !e506 */
        }

        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "ON Demand Deprecated", 0, 0, 0);
        return (TRUE);

      case PDSM_LITE_PA_XTRA_APN_PROFILE:
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Getting APN profile", 0, 0, 0);
        return (TRUE);

      case PDSM_LITE_PA_PE_CONFIG_PARAM:
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Getting PE config param not supported", 0, 0, 0);
        get_param_ptr->pa_info_type_ptr->pe_config_info.peConfigMask = 0;  // mark all as failure
        return (TRUE); /*lint !e506 */

      case PDSM_LITE_PA_XTRA_VERSION_CHECK:
        {
#ifndef FEATURE_GNSS_LEAN_BUILD
          get_param_ptr->pa_info_type_ptr->xtra_version_check = tm_xtra_get_version_check();
          if (PDSM_LITE_PA_XTRA_VERSION_CHECK_MAX == get_param_ptr->pa_info_type_ptr->xtra_version_check)
          {
            MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "GetParam: PDSM_LITE_PA_XTRA_VERSION_CHECK %u", (uint32)get_param_ptr->pa_info_type_ptr->xtra_version_check);
            return (FALSE);
          }
          MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "GetParam: PDSM_LITE_PA_XTRA_VERSION_CHECK %u", (uint32)get_param_ptr->pa_info_type_ptr->xtra_version_check);
#endif /* ! FEATURE_GNSS_LEAN_BUILD */
          return (TRUE);
        }
        break;


      default:
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unknown PA get type %d", get_param_ptr->e_pa_type, 0, 0);
        return (FALSE);
    }

    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "EFS NV Item %d get failure", get_param_ptr->e_pa_type, 0, 0);
    return (FALSE);
  }
} /* tm_core_handle_get_param */

/*===========================================================================

FUNCTION tm_core_nv_write_cb

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_nv_write_cb
(
   nv_items_enum_type    item,
   void *data,
   nv_stat_enum_type     status
   )
{
  pdsm_lite_pa_info_s_type pa_info_data;

  if (status != NV_DONE_S)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NV Write Failed", 0, 0, 0);
    return;
  }

  switch (item)
  {
    case NV_GPS1_LOCK_I:
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "GPS LOCK NV Write Status %d", status, 0, 0);
      pa_info_data.client_id        = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event         = PDSM_LITE_PA_GPS_LOCK;
      pa_info_data.pa_info.gps_lock = TM_CORE_CONFIG_DD(gps_lock);
      tm_pdapi_pa_event_callback(&pa_info_data,PDSM_LITE_PA_CMD_ERR_NOERR);
      break;

    case NV_CGPS_ON_DEMAND_ENABLED_I: /* deprecated */
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "ON Demand Deprecated", 0, 0, 0);
      break;

    case NV_CGPS_UTC_GPS_TIME_OFFSET_I:
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "UTC Time Offset write status %d", status, 0, 0);
      break;

    default:
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unknown nv write callback", 0, 0, 0);
      break;
  }
}


/*===========================================================================

FUNCTION tm_core_nv_async_rd_cb

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_nv_async_rd_cb
(
   nv_items_enum_type    item,
   void *data,
   nv_stat_enum_type     status
   )
{
  nv_item_type *tm_nv_read_buffer;
  int prtlTableIndex;

  if (status != NV_DONE_S)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NV Async READ Failed for Item: %d", (int)item, 0, 0);
    return;
  }

  tm_nv_read_buffer = ((nv_item_type *)data);
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "NV Async Read Status: %d Item: %d", status, (int)item, 0);

      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unknown NV Async Read Status callback", 0, 0, 0);
}

/*===========================================================================

FUNCTION tm_core_handle_set_param

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_set_param(tm_pdapi_param_s_type *set_param_ptr)
{
  boolean             retVal          = FALSE;
  int                 prtlTableIndex;
  prtl_event_u_type   *event_payload_ptr=NULL;
  uint16              i;
  pdsm_lite_pa_info_s_type pa_info_data;
  boolean             force_rcvr_off  = FALSE;
  tm_xtra_state_type  *xtra_state_ptr = NULL;

  if ((set_param_ptr == NULL) || (set_param_ptr->pa_info_type_ptr == NULL))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null input pointer", 0, 0, 0);
    return (FALSE);
  }
  
  /* PDSM_LITE_PA_DELETE_PARAMS is deprecated. use new PDAPI command PDSM_LITE_PA_DELETE_PARAMS_EX to
     handle deletion of eph / alm for individual SVs based on SV mask.
     Handling of PDSM_LITE_PA_DELETE_PARAMS has a bug in tm_core.c:tm_core_delete_inject_aiding_data()
     that deletes eph / alm for all SV, irrespective of eph / alm mask. The "feature" is retained for
     backwards compatibility and instead PDSM_LITE_PA_DELETE_PARAMS_EX should be used (when defined).
     */
  if ((PDSM_LITE_PA_DELETE_PARAMS_EX1 == set_param_ptr->e_pa_type))
  {
    event_payload_ptr = (prtl_event_u_type*)os_MemAlloc(sizeof(prtl_event_u_type), OS_MEM_SCOPE_TASK);
    if (NULL == event_payload_ptr)
    {
       MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory");
       return FALSE;
    }
    /* PDSM_LITE_PA_DELETE_PARAMS_EX and PDSM_LITE_PA_DELETE_PARAMS are a special case, no NV write is required */

    event_payload_ptr->pa_event_payload.pa_event = set_param_ptr->e_pa_type;
    event_payload_ptr->pa_event_payload.pa_info_ptr = set_param_ptr->pa_info_type_ptr;
    prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_STANDALONE);
    if ((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
    {
      if (tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
      {
        (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
           PRTL_EVENT_TYPE_PA,
           event_payload_ptr);
      }
    }
    os_MemFree((void * *) &event_payload_ptr);

    /* send the pdapi event call back since we dont need to write to NV */
    pa_info_data.client_id                  = set_param_ptr->client_id;
    pa_info_data.pa_event                   = set_param_ptr->e_pa_type;
    pa_info_data.pa_info.delete_params      = set_param_ptr->pa_info_type_ptr->delete_params;
    tm_pdapi_pa_event_callback(&pa_info_data, PDSM_LITE_PA_CMD_ERR_NOERR);

    /* Following check emulates the behavior for PDSM_LITE_PA_DELETE_PARAMS,
    using the newer implemenation for PDSM_LITE_PA_DELETE_PARAMS_EX */
    if (set_param_ptr->pa_info_type_ptr->delete_params.pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_EPH)
    {
      set_param_ptr->pa_info_type_ptr->delete_params.eph_sv_mask = (uint32)C_DELETE_INFO_GPS_EPH_INFO;
    }
    if (set_param_ptr->pa_info_type_ptr->delete_params.pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM)
    {
      set_param_ptr->pa_info_type_ptr->delete_params.alm_sv_mask = (uint32)C_DELETE_INFO_GPS_ALM_INFO;
    }
    tm_core_delete_inject_aiding_data(&(set_param_ptr->pa_info_type_ptr->delete_params), force_rcvr_off, FALSE, set_param_ptr->e_pa_type);

    return (TRUE); /*lint !e506 */
  }
  else if (set_param_ptr->e_pa_type == PDSM_LITE_PA_DPO_CONTROL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Changing DPO Control Setting %ld", set_param_ptr->pa_info_type_ptr->dpo_control, 0, 0);

    pa_info_data.client_id           = set_param_ptr->client_id;
    pa_info_data.pa_event            = set_param_ptr->e_pa_type;
    pa_info_data.pa_info.dpo_control = set_param_ptr->pa_info_type_ptr->dpo_control;

    tm_pdapi_pa_event_callback(&pa_info_data, PDSM_LITE_PA_CMD_ERR_NOERR);
    return (TRUE); /*lint !e506 */
  }
  else
  {
    if (tm_nv_initialized == FALSE)
    {
      tm_nv_rdrw_handle = sm_nv_rdrw_reg(THREAD_ID_SM_TM, tm_core_nv_write_cb,
                                         tm_core_nv_async_rd_cb);

      if (tm_nv_rdrw_handle != SM_NV_CLIENT_HANDLE_ERROR)
      {
        tm_nv_initialized = TRUE; /*lint !e506 */
      }
    }

    if (tm_nv_rdrw_handle != SM_NV_CLIENT_HANDLE_ERROR)
    {
      tm_core_info.pa_client_info.client_id = set_param_ptr->client_id;

      switch (set_param_ptr->e_pa_type)
      {
        case PDSM_LITE_PA_GPS_LOCK:
          {
            boolean lock_changed = FALSE;

            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Writing GPS Lock Setting (%d) to NV", set_param_ptr->pa_info_type_ptr->gps_lock, 0, 0); /*lint !e571 */

            /* Update RAM copy first */
            if (TM_CORE_CONFIG_DD(gps_lock) != set_param_ptr->pa_info_type_ptr->gps_lock)
            {
              lock_changed =  TRUE;
            }
            TM_CORE_CONFIG_DD(gps_lock) = set_param_ptr->pa_info_type_ptr->gps_lock;
         tm_pa_nv_write_buffer = (nv_item_type*)os_MemAlloc(sizeof(nv_item_type), OS_MEM_SCOPE_TASK);
         if (tm_pa_nv_write_buffer == NULL)
         {
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory");
          return FALSE;
         }
            /* Write to NV */
            tm_pa_nv_write_buffer->gps1_lock = TM_CORE_CONFIG_DD(gps_lock);
            (void)sm_nv_write(NV_GPS1_LOCK_I, tm_pa_nv_write_buffer, tm_nv_rdrw_handle);
         os_MemFree((void * *) &tm_pa_nv_write_buffer);
            /* Inform protocol sub-modules for PA changes */

            if (lock_changed == TRUE)
            {
              /* if GPS is locked and on-demand positioning/ready mode is enabled, turn it off*/
              if (TM_CORE_CONFIG_DD(gps_lock) != PDSM_LITE_GPS_LOCK_NONE)
              {
                /* Clear the cached position if GPS is locked */
#if 0 //#9x05: cached  pos not being used so being removed right now
                tm_core_clear_cached_pos();
#endif
              }

              /*Update GPS lock state to Geofencing Core*/
              gm_update_gps_lock_state((pdsm_lite_gps_lock_e_type)(TM_CORE_CONFIG_DD(gps_lock)));
            }
          retVal = TRUE;
            }
            retVal = TRUE;
            break;

#ifdef FEATURE_CGPS_XTRA_T

        case PDSM_LITE_PA_XTRAT_CLIENT_TOKEN:
          {
            boolean             tle_send_success;
            tle_api_info_s_type xtrat_xtmapi_client_info;

            xtrat_xtmapi_client_info.type = TLE_API_E_CLIENT_TOKEN;
            xtrat_xtmapi_client_info.data.client_token = (uint32)set_param_ptr->pa_info_type_ptr->client_cookie;
            xtrat_xtmapi_client_info.client_id = set_param_ptr->client_id;

            tle_send_success = tle_SendXTRATApiInfo(&xtrat_xtmapi_client_info);

            if (tle_send_success)
            {
              retVal = TRUE;
            }
            else
            {
              retVal = FALSE;
              MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "XTRA-T GNSS_NV_EFS_TLE_XTRAT_CLIENT_TOKEN could not be set", 0, 0, 0);
            }
          }
          break;

        case PDSM_LITE_PA_XTRAT_USER_SESSION_CONTROL:
          {
            pdsm_lite_pa_info_s_type pa_info;
            tle_api_info_s_type xtrat_xtmapi_session_info;

            xtra_t_rcvd_keys[xtra_t_key_idx++] = (uint8)set_param_ptr->pa_info_type_ptr->session_control;
            if (xtra_t_key_idx > XTRA_T_RCVD_KEY_MAX)
            {
              xtra_t_key_idx = 0;

              /* Check for matching XTRA-T Handshack */
              if ((xtra_t_rcvd_keys[0] == XTRA_T_HANDSHAKE_KEY_0) &&
                  (xtra_t_rcvd_keys[1] == XTRA_T_HANDSHAKE_KEY_1) &&
                  (xtra_t_rcvd_keys[2] == XTRA_T_HANDSHAKE_KEY_2) &&
                  (xtra_t_rcvd_keys[3] == XTRA_T_HANDSHAKE_KEY_3))
              {
                boolean tle_send_success;

                xtrat_xtmapi_session_info.type = TLE_API_E_USER_SESS_CONTROL;
                xtrat_xtmapi_session_info.data.session_control = xtra_t_rcvd_keys[XTRA_T_RCVD_KEY_MAX];
                xtrat_xtmapi_session_info.client_id = set_param_ptr->client_id;
                tle_send_success = tle_SendXTRATApiInfo(&xtrat_xtmapi_session_info);

                if (tle_send_success)
                {
                  retVal = TRUE;
                }
                else
                {
                  retVal = FALSE;
                  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "XTRA-T GNSS_NV_EFS_TLE_XTRAT_USER_SESSION_CONTROL could not be set", 0, 0, 0);
                }
              }
            }

            /* send the pdapi event call back since we dont need to write to NV */
            pa_info.client_id               = set_param_ptr->client_id;
            pa_info.pa_event                = set_param_ptr->e_pa_type;
            pa_info.pa_info.session_control = set_param_ptr->pa_info_type_ptr->session_control;
            tm_pdapi_pa_event_callback(&pa_info,PDSM_LITE_PA_CMD_ERR_NOERR);
          }
          break;
#endif /* FEATURE_CGPS_XTRA_T */

        case PDSM_LITE_PA_XTRA_APN_PROFILE:
          {
            retVal = TRUE;
          }
          break;

         case PDSM_LITE_PA_PE_CONFIG_PARAM:
          {
            boolean bPEConfig = FALSE;
            pdsm_lite_pa_cmd_err_e_type pa_cmd_err = PDSM_LITE_PA_CMD_ERR_NOERR;

            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Setting PE config param flag=0x%x", set_param_ptr->pa_info_type_ptr->pe_config_info.peConfigMask, 0, 0);

            pa_info_data.client_id              = set_param_ptr->client_id;
            pa_info_data.pa_event               = set_param_ptr->e_pa_type;
            pa_info_data.pa_info.pe_config_info = set_param_ptr->pa_info_type_ptr->pe_config_info;
            if(LM_DMM_STATE_LOADED == lm_get_dmm_state())
            {
               bPEConfig = mgp_SetPositioningMethodConfig(&(set_param_ptr->pa_info_type_ptr->pe_config_info));
            }
            else
            {
               pa_cmd_err = PDSM_LITE_PA_CMD_ERR_GNSS_SRVC_NOT_AVAILABLE;
            }
            if (bPEConfig == FALSE)
            {
              MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Failed to set PE config param", 0, 0, 0);
              pa_info_data.pa_info.pe_config_info.peConfigMask = 0; // mark all as failure
            }
            tm_pdapi_pa_event_callback(&pa_info_data, pa_cmd_err);

            retVal = TRUE;
          }
          break;

#ifndef FEATURE_GNSS_LEAN_BUILD

        case PDSM_LITE_PA_XTRA_VERSION_CHECK:
                  {
            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Writing XTRA Version Check configuration=%d to TM", set_param_ptr->pa_info_type_ptr->xtra_version_check, 0, 0);
            tm_xtra_update_version_check(set_param_ptr->pa_info_type_ptr->xtra_version_check);

            /* Send the pdapi event call back since we dont need to write to NV */
            pa_info_data.client_id                  = set_param_ptr->client_id;
            pa_info_data.pa_event                   = set_param_ptr->e_pa_type;
            pa_info_data.pa_info.xtra_version_check = set_param_ptr->pa_info_type_ptr->xtra_version_check;
            tm_pdapi_pa_event_callback(&pa_info_data,PDSM_LITE_PA_CMD_ERR_NOERR);

            retVal = TRUE;
          }
          break;

#endif

#ifdef FEATURE_CGPS_XTRA_T


#endif /* FEATURE_CGPS_XTRA_T */

        case PDSM_LITE_PA_UAX3_CONFIG:
          {
            pa_info_data.client_id              = set_param_ptr->client_id;
            pa_info_data.pa_event               = set_param_ptr->e_pa_type;
            pa_info_data.pa_info.uax3_config_info  = set_param_ptr->pa_info_type_ptr->uax3_config_info;
            if (FALSE == sm_nv_efs_reg_item_write(GNSS_NV_EFS_SM_TM_UAX3_CONFIG, sizeof(uint32),
                                                  (void *)&(set_param_ptr->pa_info_type_ptr->uax3_config_info)))
            {
              tm_pdapi_pa_event_callback_ex(&pa_info_data, PDSM_LITE_PA_CMD_ERR_NO_BUF_L);
              retVal = FALSE;
            }
            else
            {
              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "UAX3 config written to NV = %d", set_param_ptr->pa_info_type_ptr->uax3_config_info);
              xtra_state_ptr = tm_xtra_get_state_ptr();
              if (xtra_state_ptr == NULL)
              {
                TM_XTRA_MSG_ERROR("Null XTRA state ptr", 0, 0, 0);
                tm_pdapi_pa_event_callback_ex(&pa_info_data, PDSM_LITE_PA_CMD_ERR_NO_BUF_L);
                retVal = FALSE;
              }
              else
              {
                xtra_state_ptr->uax3_config = set_param_ptr->pa_info_type_ptr->uax3_config_info;
                /* Update UAX3 ConfigStatus to MGP */
              if(LM_DMM_STATE_LOADED == lm_get_dmm_state())
            {
                mgp_UpdateUax3Config(set_param_ptr->pa_info_type_ptr->uax3_config_info);
                tm_pdapi_pa_event_callback_ex(&pa_info_data, PDSM_LITE_PA_CMD_ERR_GNSS_SRVC_NOT_AVAILABLE);
            }
            else
            {
               tm_pdapi_pa_event_callback_ex(&pa_info_data, PDSM_LITE_PA_CMD_ERR_OTHER);
            }
            retVal = TRUE;
              }
            }
          }
          break;
        case PDSM_LITE_PA_GTP_WWAN_FIX_TIMER_CONFIG:
          {
            pa_info_data.client_id                 = set_param_ptr->client_id;
            pa_info_data.pa_event                  = set_param_ptr->e_pa_type;
            pa_info_data.pa_info.gtp_wwan_fix_timer_duration  = set_param_ptr->pa_info_type_ptr->gtp_wwan_fix_timer_duration;
            if (FALSE == sm_nv_efs_reg_item_write(GNSS_NV_EFS_SM_GTP_WWAN_FIX_TIMER_DURATION, sizeof(uint32),
                                                  (void *)&(set_param_ptr->pa_info_type_ptr->gtp_wwan_fix_timer_duration)))
            {
              tm_pdapi_pa_event_callback_ex(&pa_info_data, PDSM_LITE_PA_CMD_ERR_NO_BUF_L);
              retVal = FALSE;
            }
            else
            {
              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "GTP_WWAN_FIX_TIMER config written to NV = %d", set_param_ptr->pa_info_type_ptr->gtp_wwan_fix_timer_duration);
              tm_pdapi_pa_event_callback_ex(&pa_info_data, PDSM_LITE_PA_CMD_ERR_NOERR);
              retVal = TRUE;

            }
          }
          break;
        case PDSM_LITE_PA_GTP_WWAN_MIN_INTERVAL_BTWN_REQ:
          {
            pa_info_data.client_id                 = set_param_ptr->client_id;
            pa_info_data.pa_event                  = set_param_ptr->e_pa_type;
            pa_info_data.pa_info.gtp_wwan_min_interval_btwn_req  = set_param_ptr->pa_info_type_ptr->gtp_wwan_min_interval_btwn_req;
            if (FALSE == sm_nv_efs_reg_item_write(GNSS_NV_EFS_SM_GTP_WWAN_MIN_INTERVAL_BTWN_REQ, sizeof(uint16),
                                                  (void *)&(set_param_ptr->pa_info_type_ptr->gtp_wwan_min_interval_btwn_req)))
            {
              tm_pdapi_pa_event_callback_ex(&pa_info_data, PDSM_LITE_PA_CMD_ERR_NO_BUF_L);
              retVal = FALSE;
            }
            else
            {
              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "GTP_WWAN_MIN_INTERVAL_BTWN_REQ config written to NV = %d", set_param_ptr->pa_info_type_ptr->gtp_wwan_min_interval_btwn_req);
              tm_pdapi_pa_event_callback_ex(&pa_info_data, PDSM_LITE_PA_CMD_ERR_NOERR);
              retVal = TRUE;

            }
          }
          break;

        case PDSM_LITE_PA_BYPASS_SYS_UNC_CHECK:
          {
            pa_info_data.client_id = set_param_ptr->client_id;
            pa_info_data.pa_event  = set_param_ptr->e_pa_type;
            pa_info_data.pa_info.bypass_sys_unc_check  = set_param_ptr->pa_info_type_ptr->bypass_sys_unc_check;
            if (FALSE == sm_nv_efs_reg_item_write(GNSS_NV_EFS_SM_BYPASS_GNSS_AIDING_CHECK, sizeof(uint8),
                                                  (void *)&(set_param_ptr->pa_info_type_ptr->bypass_sys_unc_check)))
            {
              tm_pdapi_pa_event_callback_ex(&pa_info_data, PDSM_LITE_PA_CMD_ERR_NO_BUF_L);
              retVal = FALSE;
            }
            else
            {
              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "bypass_sys_unc config written to NV = %d", set_param_ptr->pa_info_type_ptr->bypass_sys_unc_check);
              tm_pdapi_pa_event_callback_ex(&pa_info_data, PDSM_LITE_PA_CMD_ERR_NOERR);
            retVal = TRUE;
              
            }
          }
          break;
  
        default:
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unknown set param type", 0, 0, 0);
          retVal = FALSE;
          break;
      }
    }
  }
  return (retVal); /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_core_xtra_req_check

DESCRIPTION
   Check the current TM-Core state to determine if XTRA request is allowed.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_lite_xtra_cmd_err_e_type tm_core_xtra_req_check
(
   const pdsm_lite_xtra_cmd_s_type *xtra_clnt_req_ptr
   )
{
#ifdef FEATURE_CGPS_XTRA

  /* Local Variables */
  pdsm_lite_xtra_cmd_err_e_type       xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_NONE;
  tm_cm_phone_state_info_s_type  phone_state_info;
  pdsm_lite_client_id_type            xtra_client_id;

  do
  {
    /*----------------------------------------------------------------------
       First, check if the TM task is active
    ----------------------------------------------------------------------*/
    if (tm_core_info.tm_task_info.task_active == FALSE)
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Task is not active", 0, 0, 0);
      xtra_cmd_err =  PDSM_LITE_XTRA_CMD_ERR_INTERNAL_RESOURCE_ERROR;
      break;
    }

    /*----------------------------------------------------------------------
       Check if the TM task is active
    ----------------------------------------------------------------------*/
    if (xtra_clnt_req_ptr == NULL)
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "NULL ptr", 0, 0, 0);
      xtra_cmd_err =  PDSM_LITE_XTRA_CMD_ERR_INVALID_PARAM;
      break;
    }

    /*----------------------------------------------------------------------
       Only grant XTRA client ID access
    ----------------------------------------------------------------------*/
    xtra_client_id = pdsmclient_lite_get_client_id_map(PDSM_LITE_CLIENT_TYPE_XTRA);

    if (xtra_client_id != xtra_clnt_req_ptr->client_id)
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_PDAPI: PDSM_LITE_XTRA_CMD_ERR_CLIENT_ID_P", 0, 0, 0);
      xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_CLIENT_ID_P;
      break;
    }

    /*----------------------------------------------------------------------
      E911 related checks (E911 call orgination, E911 CBW, ...etc
    ----------------------------------------------------------------------*/
    tm_cm_iface_get_phone_state(&phone_state_info);

    /*----------------------------------------------------------------------
       GPS lock is enabled. XTRA download is allowed if GPS is locked
       but UE is in 911 call.
    ----------------------------------------------------------------------*/
    if ((TM_CORE_CONFIG_DD(gps_lock) == PDSM_LITE_GPS_LOCK_MI)
         || (TM_CORE_CONFIG_DD(gps_lock) == PDSM_LITE_GPS_LOCK_ALL))
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "MI GPS is locked and not in 911");
      xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_GPS_LOCK_S;
      break;
    }

    /*----------------------------------------------------------------------
       Check for command parameter error
    ----------------------------------------------------------------------*/
    switch (xtra_clnt_req_ptr->cmd)
    {
      case PDSM_LITE_XTRA_CMD_SET_DATA:
        xtra_cmd_err = tm_xtra_check_set_data_para(xtra_clnt_req_ptr);
        if (xtra_cmd_err != PDSM_LITE_XTRA_CMD_ERR_NOERR)
        {
          // for SET_DATA command failure, need to reset buffer
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "XTRA_CMD_SET_DATA failure %d, reset XTRA buffer", xtra_cmd_err, 0, 0);
          tm_xtra_reset_data_buffer();
        }
        break;

      case PDSM_LITE_XTRA_CMD_INITIATE_DOWNLOAD:
        xtra_cmd_err = tm_xtra_check_initiate_download_para(xtra_clnt_req_ptr);
        break;

      case PDSM_LITE_XTRA_CMD_SET_AUTO_DOWNLOAD_PARAMS:
        xtra_cmd_err = tm_xtra_check_auto_download_para(xtra_clnt_req_ptr);
        break;

      case PDSM_LITE_XTRA_CMD_QUERY_DATA_VALIDITY:
        xtra_cmd_err = tm_xtra_check_query_data_validity_para(xtra_clnt_req_ptr);
        break;

      case PDSM_LITE_XTRA_CMD_TIME_INFO_INJ:
        xtra_cmd_err = tm_xtra_check_time_info_inject_para(xtra_clnt_req_ptr);
        break;
      case PDSM_LITE_XTRA_CMD_QUERY_XTRA_CFG_INFO:
        xtra_cmd_err = tm_xtra_check_xtra_config_info_para(xtra_clnt_req_ptr);
        break;

      default:
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "XTRA: Unrecognized XTRA cmd %d",
              xtra_clnt_req_ptr->cmd, 0, 0);
        xtra_cmd_err = PDSM_LITE_XTRA_CMD_ERR_UNKNOWN_CMD;
        break;
    } /* end switch */
  } while (0);


  /* XTRA request is granted */
  return xtra_cmd_err;

#else
  return PDSM_LITE_XTRA_CMD_ERR_FEATURE_NOT_ENABLED;
#endif /* FEATURE_CGPS_XTRA */
} /* End tm_core_xtra_req_check() */

/*===========================================================================

FUNCTION tm_core_handle_xtra_client_cmd

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_xtra_client_cmd
(
   pdsm_lite_xtra_cmd_s_type *cmd_ptr
   )
{
#ifdef FEATURE_CGPS_XTRA

  /* Local Variables */
  boolean ret_val = FALSE;

  /*----------------------------------------------------------------------
     Check if the TM task is active
  ----------------------------------------------------------------------*/
  if (cmd_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "NULL ptr", 0, 0, 0);
    return ret_val;
  }

  /*----------------------------------------------------------------------
     Process the incoming XTRA client request
  ----------------------------------------------------------------------*/
  switch (cmd_ptr->cmd)
  {
    case PDSM_LITE_XTRA_CMD_SET_DATA:
      ret_val = tm_xtra_process_set_data_cmd(cmd_ptr);
      break;
    case PDSM_LITE_XTRA_CMD_TIME_INFO_INJ:
      ret_val = tm_xtra_process_inject_time_info(cmd_ptr);
      break;

    case PDSM_LITE_XTRA_CMD_INITIATE_DOWNLOAD:
      ret_val = tm_xtra_process_initiate_download_cmd(cmd_ptr);
      break;

    case PDSM_LITE_XTRA_CMD_SET_AUTO_DOWNLOAD_PARAMS:
      ret_val = tm_xtra_process_set_auto_download_cmd(cmd_ptr);
      break;

    case PDSM_LITE_XTRA_CMD_QUERY_DATA_VALIDITY:
      ret_val = tm_xtra_process_query_data_validity_cmd(cmd_ptr);
      break;
    case PDSM_LITE_XTRA_CMD_QUERY_XTRA_CFG_INFO:
      ret_val = tm_xtra_process_query_xtra_cfg_info_cmd(cmd_ptr);
      break;


    default:
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "gpOneXTRA: Unrecognized XTRA cmd %d, abort processing",
            cmd_ptr->cmd, 0, 0);
      break;
  } /* end switch */

  return ret_val;

#else
  return FALSE;
#endif /* FEATURE_CGPS_XTRA */
}

/*===========================================================================

FUNCTION tm_core_nv_write

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_nv_write
(
   nv_items_enum_type nv_item,
   void *item_ptr
   )
{
  /* Local Variables */
  boolean ret_val = FALSE;

  /*----------------------------------------------------------------------
     Check input parameters
  ----------------------------------------------------------------------*/
  if (item_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null input pointer", 0, 0, 0);
    return ret_val;
  }


  /*----------------------------------------------------------------------
     Initialize the NV handle
  ----------------------------------------------------------------------*/
  if (tm_nv_initialized == FALSE)
  {
    tm_nv_rdrw_handle = sm_nv_rdrw_reg(THREAD_ID_SM_TM, tm_core_nv_write_cb,
                                       tm_core_nv_async_rd_cb);

    if (tm_nv_rdrw_handle != SM_NV_CLIENT_HANDLE_ERROR)
    {
      tm_nv_initialized = TRUE; /*lint !e506 */
    }
  }

  /*----------------------------------------------------------------------
     Write the NV item
  ----------------------------------------------------------------------*/
  if (tm_nv_rdrw_handle != SM_NV_CLIENT_HANDLE_ERROR)
  {
    ret_val = sm_nv_write(nv_item, item_ptr, tm_nv_rdrw_handle);
  }

  return ret_val;

} /* End tm_core_nv_write() */

/*===========================================================================

FUNCTION tm_core_nv_async_read

DESCRIPTION
   Function to queue NV READ requests with SM NV module and to be processed
   Asynchromously.
   Function should not be called in the context of tm_core_nv_async_rd_cb()
   Will result in duplicate requests to NV module from sm_nv_cmd_q.
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_nv_async_read
(
   nv_items_enum_type nv_item,
   void *item_ptr
   )
{
  /* Local Variables */
  boolean ret_val = FALSE;

  /*----------------------------------------------------------------------
     Check input parameters
  ----------------------------------------------------------------------*/
  if (item_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null input pointer", 0, 0, 0);
    return ret_val;
  }


  /*----------------------------------------------------------------------
     Initialize the NV handle
  ----------------------------------------------------------------------*/
  if (tm_nv_initialized == FALSE)
  {
    tm_nv_rdrw_handle = sm_nv_rdrw_reg(THREAD_ID_SM_TM, tm_core_nv_write_cb,
                                       tm_core_nv_async_rd_cb);

    if (tm_nv_rdrw_handle != SM_NV_CLIENT_HANDLE_ERROR)
    {
      tm_nv_initialized = TRUE; /*lint !e506 */
    }
  }

  /*----------------------------------------------------------------------
     Read the NV item
  ----------------------------------------------------------------------*/
  if (tm_nv_rdrw_handle != SM_NV_CLIENT_HANDLE_ERROR)
  {
    ret_val = sm_nv_async_read(nv_item, item_ptr, tm_nv_rdrw_handle);
  }

  return ret_val;

} /* End tm_core_nv_async_read() */

/*===========================================================================

FUNCTION tm_core_xtra_t_req_check

DESCRIPTION
   Check the current TM-Core state to determine if XTRA-T request is allowed.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_lite_xtra_t_cmd_err_e_type tm_core_xtra_t_req_check
(
   const pdsm_lite_xtra_t_cmd_s_type *xtra_t_clnt_req_ptr
   )
{
#ifdef FEATURE_CGPS_XTRA_T

  /* Local Variables */
  pdsm_lite_xtra_t_cmd_err_e_type     xtra_t_cmd_err = PDSM_LITE_XTRA_T_CMD_ERR_NONE;
  tm_cm_phone_state_info_s_type  phone_state_info;
  pdsm_lite_client_id_type            xtra_t_client_id;

  /*----------------------------------------------------------------------
     First, check if the TM task is active
  ----------------------------------------------------------------------*/
  if (tm_core_info.tm_task_info.task_active == FALSE)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Task is not active", 0, 0, 0);
    xtra_t_cmd_err =  PDSM_LITE_XTRA_T_CMD_ERR_INTERNAL_RESOURCE;
  }

  /*----------------------------------------------------------------------
     Check if the XTRA-T client pointer is valid
  ----------------------------------------------------------------------*/
  if (xtra_t_clnt_req_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "NULL ptr", 0, 0, 0);
    xtra_t_cmd_err =  PDSM_LITE_XTRA_T_CMD_ERR_INVALID_PARAM;
    return xtra_t_cmd_err;
  }

  /*----------------------------------------------------------------------
     Only grant XTRA-T client ID access
  ----------------------------------------------------------------------*/
  xtra_t_client_id = pdsmclient_lite_get_client_id_map(PDSM_LITE_CLIENT_TYPE_XTRA_T);

  if (xtra_t_client_id != xtra_t_clnt_req_ptr->client_id)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_PDAPI: PDSM_LITE_XTRA_T_CMD_ERR_CLIENT_ID_P", 0, 0, 0);
    xtra_t_cmd_err = PDSM_LITE_XTRA_T_CMD_ERR_CLIENT_ID_P;
  }

  /*----------------------------------------------------------------------
    E911 related checks (E911 call orgination, E911 CBW, ...etc
  ----------------------------------------------------------------------*/
  tm_cm_iface_get_phone_state(&phone_state_info);


  /*----------------------------------------------------------------------
     GPS lock is enabled
  ----------------------------------------------------------------------*/
  if ((TM_CORE_CONFIG_DD(gps_lock) == PDSM_LITE_GPS_LOCK_MI)
      || (TM_CORE_CONFIG_DD(gps_lock) == PDSM_LITE_GPS_LOCK_ALL))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "MI GPS is locked", 0, 0, 0);
    xtra_t_cmd_err = PDSM_LITE_XTRA_T_CMD_ERR_GPS_LOCK_S;
  }

  /*----------------------------------------------------------------------
     Check for command parameter error
  ----------------------------------------------------------------------*/
  switch (xtra_t_clnt_req_ptr->cmd)
  {
    case PDSM_LITE_XTRA_T_CMD_GET_DATA:
    case PDSM_LITE_XTRA_T_CMD_SET_DATA:
    case PDSM_LITE_XTRA_T_CMD_INITIATE_DOWNLOAD:
    case PDSM_LITE_XTRA_T_CMD_INITIATE_UPLOAD:
    default:
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "XTRA-T: Unrecognized XTRA-T cmd %d",
            xtra_t_clnt_req_ptr->cmd, 0, 0);
      xtra_t_cmd_err = PDSM_LITE_XTRA_T_CMD_ERR_UNKNOWN_CMD;
      break;
  } /* end switch */

  /* XTRA request is granted */
  return xtra_t_cmd_err;

#else
  return PDSM_LITE_XTRA_T_CMD_ERR_FEATURE_NOT_ENABLED;
#endif /* FEATURE_CGPS_XTRA_T */
} /* End tm_core_xtra_t_req_check() */


/*===========================================================================

FUNCTION tm_core_handle_xtra_t_client_cmd

DESCRIPTION  XTRA-T command handler

DEPENDENCIES

RETURN VALUE  TRUE - success;  FALSE - failure

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_xtra_t_client_cmd
(
   pdsm_lite_xtra_t_cmd_s_type *cmd_ptr
   )
{
#ifdef FEATURE_CGPS_XTRA_T

  /* Local Variables */
  boolean ret_val = FALSE;

  /*----------------------------------------------------------------------
     Check if the TM task is active
  ----------------------------------------------------------------------*/
  if (cmd_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "NULL ptr", 0, 0, 0);
    return ret_val;
  }

  /*----------------------------------------------------------------------
     Process the incoming XTRA-T client request
  ----------------------------------------------------------------------*/
  switch (cmd_ptr->cmd)
  {
    case PDSM_LITE_XTRA_T_CMD_GET_DATA:
    case PDSM_LITE_XTRA_T_CMD_SET_DATA:
    case PDSM_LITE_XTRA_T_CMD_INITIATE_DOWNLOAD:
    case PDSM_LITE_XTRA_T_CMD_INITIATE_UPLOAD:
    default:
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "XTRA-T: Unrecognized XTRA-T cmd %d, abort processing",
            cmd_ptr->cmd, 0, 0);
      break;
  } /* end switch */

  return ret_val;

#else
  return FALSE;
#endif /* FEATURE_CGPS_XTRA_T */
} /* tm_core_handle_xtra_t_client_cmd */



