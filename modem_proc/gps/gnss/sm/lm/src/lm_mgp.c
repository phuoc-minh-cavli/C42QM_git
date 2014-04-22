/*======================================================================

                        Location Manager MGP interaction file

GENERAL DESCRIPTION
 This file contains functions by which Location Manager(LM) handles all 
 the MGP related processing. 

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2021 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/lm/src/lm_mgp.c#5 $
  $DateTime: 2021/03/18 02:36:13 $
  $Author: pwbldsvc $

 when      who   what, where, why
 ----      ----  -------------------------------------------------------
02/26/21   sai   Change to address honour MGP OFF Indication unconditionally
08/11/20   sai   Added support for deletion of cell DB postion support from QMI
01/08/20   sai   Workaround to handle Random MGP RCVR ACK OFF sent by MGP in ON state
09/01/20   sai   Changes done to support Geofence GTP Enhancement requirements
08/11/20   sai   Added support for deletion of cell DB postion support from QMI
05/08/20   ska     Smart compilation issues
22/03/20   ska   Changes done to support delete aiding data when MGP unload
10/01/20   ska   Changes done for MGP state to OFF when TBF>=5sec
08/21/19   py    Ignore Load over LOAD request to DMM. Removed delibrate crash.
07/04/19   py    Retry again If single shot session fails due to MGP load fail
04/04/19   nath  Cache CPI when MGP is not LOADED.
12/14/17   mj    Block sessions during SARF 
11/03/17   py    E911 MO standalone low priority will not run to time out
07/19/17   ak    Use NV value for INS/DR fixes HEPE Threshold
04/26/17   mj    Add missing changes for INS/DR fixes
02/17/17   mj    Changes to not allow final fixes when RefLoc is not received for E911
10/11/16   gk    send i/f fixes to TM in all cases
05/26/16   muk   Update the Poly reporting irrespective of LM Session State
04/02/16   mj    GERA Support
01/16/15   mj    Update function comments for lm_mgp_is_fix_source_reportable 
02/17/16   mj    Changes to report CPI fixes for E911 only if reliability is medium or better
02/17/16   mj    Added NV support to enable/disable CPI injections and early exit logic 
02/27/16   mj    Handle CPI fixes and Early exit scenarios for E911 NILR MSB  
09/23/15   mj    Added changes to report CPI fixes with lower than medium reliablity as intemediate fixes
08/28/15   mj    Add support for INS only fixes for SAP changes
05/14/15   jyw   For standalone App tracking, modified position reporting to be prompt instead 
                 of aligned to Integer second per OEM requirement
04/29/15   jv    QZSS Support added.
03/09/15   rk    Update ME with new timeout value during fix session start when the receiver is already On.
10/24/14   jv    Added Galileo Support
09/26/14   rk    Update ME with correct session timeout in the MSA to stop  probational scanning close to session timeout.
08/25/14   jv    16-bit SVID Support
07/18/14   jyw   Added support for CPE measurements
01/15/14   rk    Resolve magnetic variation of GPRMC is zero by removing duplicate variable.
10/08/13   ah    Moved Tunnel Detection enablement bit to resolve bit conflict
08/22/13   mj    Reinstate Dynamic Hepe logic for session continue for MSB apptracking for tunnel exit scenario
06/27/13   rk    Fix the malformed message output.
06/07/13   rh    Removed NO_FIX report handling (now BestAvailPos)
09/12/12   rh    Added SV freshness filtering featurized by FEATURE_GNSS_SV_FRESHNESS_FILTERING
11/08/11   rh    Converts SBAS SVID from internal(120~151) to external(33~64)
11/08/11   rh    Tracking decommissioned SVs
06/22/11   rh    Added PRM/PPM/SA/AA data in OEM layer
05/18/11   rc    Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
02/13/11   gk    eph throttling rework. 
06/18/10   gk    For single shot fixes, no need to align to sec. 
05/21/10   gk    Reset dynamic QoS thresolds if TBF >=5 to match behavior of DPO off case
02/10/10   vp    OEM DR Data Enablement
02/17/10   lt    Populated slow clk source of injected time (GPS / GLO)
01/12/10   gk    Force receiver off when end session is called
06/10/09   gk    ODP 2.0 support
08/13/08   gk    Added force receiver off
07/24/08   gk    Added a field for on-demand in 1370 log
07/21/08   ank   Apply stricter Hepe limit than 250m at qos timeout for tunnel-exit scenario.
06/24/08   ank   Reinstate dynamic hepe logic for tunnel-exit scenario.
06/12/08   lt    Support for external coarse position injection.
01/03/08   lt    If receiver reports the same state, it is no longer processed.
11/14/07   ank   For late ramp, double the ramp slope to reach Hmax at (Tmax+Tr/2)
08/01/07   rw    Add support for XTRA feature, get GPS time info
07/31/07   ank   Added Dynamic QoS and Override logic.
07/17/07   ank   Corrected the logic to qualify final fix.
05/11/07   mr    Removed call to mgp_ComputeGpsFix() to strobe MGP for fix
11/16/06   mr    Added generation of PD_API GPS_BEGIN/END events to TM 
6/20/06    mr    Initial creation of header file.
======================================================================*/
#include <stdio.h>

#include "gps_variation.h"
#include "gnss_common.h"
#include "gts_loc_api.h"
#include "lm_api.h"
#include "lm_iface.h"
#include "lm_data.h"
#include "mgp_api.h"
#include "aries_os_api.h"
#include "lm_mgp.h"
#ifdef FEATURE_CGPS_USES_CDMA
#include "lm_mgp_aflt.h"
#endif /* FEATURE_CGPS_USES_CDMA */
#include "lm_tm.h"
#include "sm_log.h"
#include "event.h"
#include "time_svc.h"
#include "math.h"
#include "tm_api.h"
#include "sm_util.h"
#include "sm_nv.h"
#include "sm_oemdre.h"
#include "sm_oemdrsync.h"
#include "sm_oemdata.h"
#include "mgp_api.h"
#include "sm_api.h"
#include "sm_util.h"
#include "lm_dmm.h"
#include "lm_diag.h"

#ifdef TEST_UNLOAD_TRIGGER_FROM_INTERNAL_CODE_LOCATIONS
void trigger_gnss_unload(uint8 event);
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                        GLOBAL DATA DECLARATIONS                                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Holds the current SARF (RF testing) state */
static boolean v_lm_sarf_state;
uint8 u_rcvr_random_off_counter = 0;

time_type cur_timestamp_CMD_ON;
time_type cur_timestamp_CMD_IDLE;
time_type cur_timestamp_CMD_OFF;
time_type cur_timestamp_ACK_ON;
time_type cur_timestamp_ACK_IDLE;
time_type cur_timestamp_ACK_OFF;
time_type cur_timestamp_random_off;

/****************** Function Definitions**************************************/
static void lm_mgp_generate_log_packet(const sm_GpsFixRptStructType *p_fixReport);
static void lm_mgp_send_intermediate_report(const sm_GpsFixRptStructType *p_fixReport);

/*
******************************************************************************
* lm_is_mgp_on
*
* Function description:
*  This function returns if MGP is On.
**
* Parameters: 
*  void
*
* Return value:
*  none
******************************************************************************
*/
boolean lm_is_mgp_on(void)
{
  if (LM_MGP_STATE_ON == z_lmControl.e_currentMgpState)
  {
    return TRUE;
  }
  return FALSE;
}
/*

******************************************************************************
* lm_is_mgp_active
*
* Function description:
*  This function returns if MGP is active. if MGP is not off or going off
*   then it is considered as active
**
* Parameters: 
*  void
*
* Return value:
*  none
******************************************************************************
*/
boolean lm_is_mgp_active(void)
{
  /*If receiver is not off or going off then it can be considered 
   that MGP is active*/
  if((z_lmControl.e_currentMgpState != LM_MGP_STATE_OFF) &&
    (z_lmControl.e_currentMgpState != LM_MGP_STATE_GOING_OFF ))
    {
       return TRUE;
    }
    else
    {
      return FALSE;
    }
}

/*
******************************************************************************
* sm_log_event_report_ex 
*  #9x05 TODO [an][del] Sdp related
* Function description:
*  Helper event generation function allowing to add a place to add in hooks
*  for notification generation to other GPS modules.
*
* Parameters: 
*  u_EventId   - enum lm_log_event_id
*  u_EventData - enum lm_log_event_data_mgp_state
*
* Return value:
*  none
******************************************************************************
*/
void sm_log_event_report_ex(uint8 u_EventId, uint8 u_EventData)
{

  sm_log_event_report(u_EventId, u_EventData);
}

/*
 ******************************************************************************
 * lm_set_current_mgp_state 
 *
 * Function description:
 *  This function is to set the current mgp state
 *
 * Parameters: 
 *  e_mgp_state: MGP State enum
 *
 * Return value:
 *  none
 ******************************************************************************
*/
static void lm_set_current_mgp_state(lm_mgp_state_e_type e_mgp_state)
{
  static lm_mgp_state_e_type e_prev_mgp_state = LM_MGP_STATE_OFF;

  if (e_prev_mgp_state != z_lmControl.e_currentMgpState)
  {
    LM_MSG_ERROR("MGP State was set outside the function state prev known %d present %d",
                 e_prev_mgp_state, z_lmControl.e_currentMgpState, 0);
  }

  if (z_lmControl.e_currentMgpState != e_mgp_state)
  {
    z_lmControl.e_currentMgpState = e_mgp_state;

    LM_MSG_MED("Current MGP State %d Desired MGP state %d",
               z_lmControl.e_currentMgpState, z_lmControl.e_desiredMgpState, 0);

  }
  e_prev_mgp_state = z_lmControl.e_currentMgpState;
}

/*
 ******************************************************************************
 * lm_mgp_set_sess_best_fix_report 
 *
 * Function description:
 *  This function saves the lowest HEPE fix for a session
 *
 * Parameters: 
 *  pFixReport - fix report received from PE
 *
 * Return value:
 *  none
 ******************************************************************************
*/
static void lm_mgp_set_sess_best_fix_report(sm_FixInfoStructType *pFixReport)
{
  FLT f_hepe;
  char b_Buffer2[250];
  if (z_lmControl.z_sessBestFixRpt==NULL)
  {
   z_lmControl.z_sessBestFixRpt = (sm_FixInfoStructType*)os_MemAlloc(sizeof(sm_FixInfoStructType), OS_MEM_SCOPE_TASK);
  }
  else
  {
     memset(z_lmControl.z_sessBestFixRpt, 0 , sizeof(*(z_lmControl.z_sessBestFixRpt)));
  // os_MemFree((void * *) &z_lmControl.z_sessBestFixRpt);
  }
  LM_MSG_HIGH("Set Best Sess Fix: [%d]", z_lmControl.u_sessBestFixValid, 0, 0);

  if (NULL == pFixReport)
  {
    LM_MSG_HIGH("Fix with no data, ignored for session best", 0, 0, 0);
    return;
  }

  if (C_GPS_WEEK_UNKNOWN == pFixReport->z_NavPos.w_GpsWeek)
  {
    LM_MSG_HIGH("Fix with no GPS time, ignored for session best", 0, 0, 0);
    return;
  }

  LM_MSG_HIGH("Fix Report received, PositionFlags: IsBest %d, isCPI %d",
              pFixReport->z_NavPos.z_SmPosFlags.b_IsPosBestAvail,
              pFixReport->z_NavPos.z_SmPosFlags.b_IsCoarsePos, 0);

  /*Cache only fixes which are CPI and reportable as final OR GNSS sourced. This will avoid sending RefLoc position back to the network */
  if (!lm_mgp_is_fix_source_reportable(pFixReport, TRUE))
  {
    LM_MSG_HIGH("Fix not CPI or GNSS, ignored for session best", 0, 0, 0);
    return;
  }

  if (!z_lmControl.u_sessBestFixValid && z_lmControl.z_sessBestFixRpt!=NULL)
  {
    memscpy(z_lmControl.z_sessBestFixRpt, 
		    sizeof(*(z_lmControl.z_sessBestFixRpt)),
			pFixReport,
			sizeof(*pFixReport));
    z_lmControl.u_sessBestFixValid = TRUE;

    z_lmControl.f_sessBestHepe = (FLT)sqrt((pFixReport->z_NavPos.f_ErrorEllipse[1] *
                                            pFixReport->z_NavPos.f_ErrorEllipse[1]) +
                                           (pFixReport->z_NavPos.f_ErrorEllipse[2] *
                                            pFixReport->z_NavPos.f_ErrorEllipse[2]));

    if (msg_status(MSG_SSID_GPSSM, MSG_LEGACY_MED))
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "New fix saved as best. hepe: %f", z_lmControl.f_sessBestHepe);
    }

    if (pFixReport->z_NavPos.z_SmPosFlags.b_IsPosBestAvail)
    {
      LM_MSG_HIGH("New Best Pos fix saved as best", 0, 0, 0);
    }

    return;
  }
  if (z_lmControl.z_sessBestFixRpt==NULL)
  {
   MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory");
   return;
  }

  f_hepe = (FLT)sqrt((pFixReport->z_NavPos.f_ErrorEllipse[1] *
                      pFixReport->z_NavPos.f_ErrorEllipse[1]) +
                     (pFixReport->z_NavPos.f_ErrorEllipse[2] *
                      pFixReport->z_NavPos.f_ErrorEllipse[2]));

  (void)snprintf(b_Buffer2, sizeof(b_Buffer2), "New HEPE =%f Sess best=%f \r\n",
                   f_hepe, z_lmControl.f_sessBestHepe);
  MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", b_Buffer2);

  if (f_hepe < z_lmControl.f_sessBestHepe)
  {
    if (msg_status(MSG_SSID_GPSSM, MSG_LEGACY_MED))
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "New fix saved as best. hepe: %f", f_hepe);
    }

	memscpy(z_lmControl.z_sessBestFixRpt, 
		    sizeof(*(z_lmControl.z_sessBestFixRpt)),
			pFixReport,
			sizeof(*pFixReport));
    z_lmControl.f_sessBestHepe = f_hepe;

    if (pFixReport->z_NavPos.z_SmPosFlags.b_IsPosBestAvail)
    {
      LM_MSG_HIGH("New Best Pos fix saved as best", 0, 0, 0);
    }
  }
}

/*
 ******************************************************************************
 * lm_mgp_get_mgp_operation_mode 
 *
 * Function description:
 *
 * This function gets corresponding mgp_ConfigOperationEnumType for a given
 * lm_gps_mode_e_type
 *
 * Parameters: 
 * lm_gps_mode_e_type - operation mode enumeration in lm_gps_mode_e_type 
 *
 * Return value: 
 *
 * mgp_ConfigOperationEnumType - operation mode enumeration in 
 * mgp_ConfigOperationEnumType
 *
 ******************************************************************************
*/
mgp_ConfigOperationEnumType lm_mgp_get_mgp_operation_mode(lm_gps_mode_e_type e_lm_mode)
{
  switch (e_lm_mode)
  {
    case LM_GPS_MODE_SA_RF:
      return MGP_OPERATION_STANDALONE_RF_VERIF;
    case LM_GPS_MODE_STANDALONE:
      return MGP_OPERATION_STANDALONE;
    default:
      return MGP_OPERATION_MAX;
  }
}

/*
 ******************************************************************************
 * lm_mgp_get_mgp_state_type 
 *
 * Function description:
 *
 * This function gets corresponding lm_mgp_state_e_type for a given
 * gps_RcvrStateEnumType
 *
 * Parameters: 
 * gps_RcvrStateEnumType - MGP state enumeration in gps_RcvrStateEnumType 
 *
 * Return value: 
 *
 * lm_mgp_state_e_type - MGP state enumeration in 
 * lm_mgp_state_e_type
 *
 ******************************************************************************
*/
lm_mgp_state_e_type lm_mgp_get_mgp_state_type(sm_RcvrStateChangeStructType *pz_rcvrState)
{
  if (NULL == pz_rcvrState)
  {
    LM_MSG_ERROR("Null pointer in rcvr state update", 0, 0, 0);
    return LM_MGP_STATE_INVALID;
  }
  switch (pz_rcvrState->e_CurrentRcvrState)
  {
    case C_GPS_RCVR_STATE_OFF:
      return LM_MGP_STATE_OFF;

    case C_GPS_RCVR_STATE_ON:
    case C_GPS_RCVR_STATE_DPO_SLEEP:            /* DPO Sleep is a transition state for Rcvr ON */
      {   //  TODO [an][del] deleted LPPM  #9x05
          return LM_MGP_STATE_ON;
      }

    case C_GPS_RCVR_STATE_IDLE:
      return LM_MGP_STATE_IDLE;
    default:
      return LM_MGP_STATE_INVALID; /*Invalid state*/
  }

}


/*
 ******************************************************************************
 * lm_reset_dynamic_hepe_threshold 
 *
 * Function description:
 *
 * This function resets the dynamic hepe threshold limits 
 *
 * Parameters: 
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/
void lm_reset_dynamic_hepe_thresold(void)
{
  LM_MSG_MED("Reset Dynamic Hepe thresholds", 0, 0, 0);

  /* Reset time of first reportable fix */
  z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn = 0;
  z_lmControl.z_early_exit_decision.u_FirstValidFixAfterEngineOn = FALSE;
  (void)time_get(z_lmControl.z_early_exit_decision.d_TimeEngOn);
  z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn = FALSE;

  z_lmControl.z_early_exit_decision.f_LastValidFixHepe = LM_LAST_VALID_FIX_HEPE_MAX;
  z_lmControl.z_early_exit_decision.q_NullFixReportCount = 0;
  z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe = FALSE;
  z_lmControl.z_early_exit_decision.u_TimeoutAfterReinstate = FALSE;
  z_lmControl.z_early_exit_decision.u_OverrideTimeOfFirstValidFixAfterEngineOn = FALSE;
}
/*
 ******************************************************************************
 * lm_get_time_from_start 
 *
 * Function description:
 *
 * This function gets corresponding time from Engine ON/ Fix start depending on enum
 *
 * Parameters: 
 *
 * Return value: Time in ms
 *
 *
 ******************************************************************************
*/
uint32 lm_get_time_from_start(lm_get_time_e_type e_StartType)
{
  qword d_CurrentTime = { 0 }, d_TimeDiff = { 0 }, d_TimeDiffMs = { 0 };

  (void)time_get(d_CurrentTime);

  switch (e_StartType)
  {
    case LM_TIME_FROM_ENGINE_ON:

      qw_sub(d_TimeDiff, d_CurrentTime, z_lmControl.z_early_exit_decision.d_TimeEngOn);
      break;

    case LM_TIME_FROM_SESSION_START:

      qw_sub(d_TimeDiff, d_CurrentTime, z_lmControl.z_early_exit_decision.d_TimeSessionStart);
      break;

    default:
      break;
  }

  /* Convert time diff to milliseconds */
  gps_total_msec(d_TimeDiff, d_TimeDiffMs);
  return d_TimeDiffMs[0];
}

/*
 ******************************************************************************
 * lm_mgp_reinstate_dynamic_hepe
 *
 * Function description:
 * restart the dynamic HEPE limit algorithm if the following conditions are both true:
 * (1) If ((MGP is ON) and (dynamic HEPE limit == MAX(250m, QoS Accuracy Threshold)))
 * (2)     IF ( [(NF reported 2 consecutive null fixes) and (HEPE of last valid fix from NF < 50m)]  OR 
 *              [NF reported 10 consecutive null fixes] )
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *  None
 *
 ******************************************************************************
*/
void lm_mgp_reinstate_dynamic_hepe(void)
{
  if (msg_status(MSG_SSID_GPSSM, MSG_LEGACY_MED))
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Check reinstate dynamic hepe: Null Fix report count=%lu LastValidFixHepe=%f\r\n",
                   z_lmControl.z_early_exit_decision.q_NullFixReportCount, z_lmControl.z_early_exit_decision.f_LastValidFixHepe);
  }

  /* If NF reported 2 null fixes and hepe of last valid fix is <= 50m, reinstate dynamic Qos */
  if (((z_lmControl.z_early_exit_decision.q_NullFixReportCount >= LM_MIN_NULL_FIXES_WITH_HEPE_LIMIT) &&
       (z_lmControl.z_early_exit_decision.f_LastValidFixHepe <= LM_LAST_VALID_FIX_HEPE_LIMIT)) ||
      (z_lmControl.z_early_exit_decision.q_NullFixReportCount >= LM_MIN_NULL_FIXES_WITHOUT_HEPE_LIMIT))
  {
    LM_MSG_MED("Reinstate dynamic hepe logic", 0, 0, 0);
    z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe = TRUE;
  }
}

/*
 ******************************************************************************
 * lm_mgp_position_accuracy_check
 *
 * Function description:
 *
 * This function checks if the position fix reported by MGP qualifies the 
 * QoS accuracy threshold provided by TM or not.
 *
 * Parameters: 
 * p_fixReport - Position fix from MGP.
 *
 * Return value: 
 *
 * TRUE - if accuracy threshold check passed
 * FALSE - if accuracy threshold check failed
 *
 ******************************************************************************
*/
boolean lm_mgp_position_accuracy_check(const sm_FixInfoStructType *const p_fixReport)
{
  FLT f_hepe = 0;
  char b_Buffer[250];
  uint32 q_TimeFromEngOn, q_TimeFromSessStart;
  uint32 q_current_hepe_limit = z_lmControl.z_fix_request.q_qos_accuracy_threshold;
  uint32 q_hepe_max = z_lmControl.z_fix_request.q_qos_accuracy_threshold;

  if ((p_fixReport == NULL) || (z_lmControl.z_fix_request.q_qos_accuracy_threshold == 0))
  {
    return FALSE;
  }

  f_hepe = (FLT)sqrt((p_fixReport->z_NavPos.f_ErrorEllipse[1] *
                      p_fixReport->z_NavPos.f_ErrorEllipse[1]) +
                     (p_fixReport->z_NavPos.f_ErrorEllipse[2] *
                      p_fixReport->z_NavPos.f_ErrorEllipse[2]));

  if (z_lmControl.z_fix_request.u_dynamic_qos_enabled)
  {
    /* Calculate time from Session Start */
    q_TimeFromSessStart = lm_get_time_from_start(LM_TIME_FROM_SESSION_START);
    LM_MSG_MED("Time from Sess Start = %lu ms", q_TimeFromSessStart, 0, 0);

    /* IF QoS_override_time is > 0, this should be interpreted as the number of seconds during which the QoS_meters_override 
    ** should be used instead of the injected value from the PD-API. After QoS_override_time seconds, the injected 
    ** QoS-meters value should apply.
    ** If the QoS_override_time > injected-QoS-timeout, the injected QoS-Timeout will still apply to end the session.
    */
    if (z_lmControl.z_early_exit_decision.u_UseQoSOverride)
    {
      if (q_TimeFromSessStart < z_lmControl.z_early_exit_decision.q_QoSTimeoutOverride)
      {
        q_current_hepe_limit = z_lmControl.z_early_exit_decision.w_QoSThresholdOverride;
      }
      else
      {
        q_current_hepe_limit = z_lmControl.q_HepeThreshold;
      }
    }

    if ((z_lmControl.z_early_exit_decision.u_UseQoSOverride) ||
        (z_lmControl.z_fix_request.q_qos_accuracy_threshold < z_lmControl.q_HepeThreshold))
    {
      q_hepe_max = z_lmControl.q_HepeThreshold;
    }

    LM_MSG_MED("Current Hepe Limit=%lu Hepe Max=%lu", q_current_hepe_limit, q_hepe_max, 0);

    /* Calculate time from which MGP engine is ON */
    q_TimeFromEngOn = lm_get_time_from_start(LM_TIME_FROM_ENGINE_ON);
    LM_MSG_MED("Time from Engine ON = %lu ms", q_TimeFromEngOn, 0, 0);

    /* If this is first fix for which f_hepe < Hmax, store Time of first reportable fix */
    if ((z_lmControl.z_early_exit_decision.u_FirstValidFixAfterEngineOn == FALSE) && (f_hepe <= q_hepe_max))
    {
      z_lmControl.z_early_exit_decision.u_FirstValidFixAfterEngineOn = TRUE;
      z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn = q_TimeFromEngOn;
      LM_MSG_MED("Time of first valid fix after Engine ON %lu ms", z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn, 0, 0);
    }

    /* If this is first fix for which f_hepe < Hmax, store Time of first reportable fix */
    if ((z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart == FALSE) && (f_hepe <= q_hepe_max))
    {
      z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart = TRUE;
      z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart = q_TimeFromSessStart;
      LM_MSG_MED("Time of first valid fix after Sess Start %lu ms", z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart, 0, 0);
    }

    /* Check if we have hit tunnel-exit scenario */
    if ((z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn == TRUE) &&
        ((z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit == q_hepe_max) ||
         (z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit == z_lmControl.z_fix_request.q_qos_accuracy_threshold)))
    {
      LM_MSG_MED("Hit tunnel exit scenario", 0, 0, 0);
      lm_mgp_reinstate_dynamic_hepe();
    }

    /* Store the hepe as last valid fix hepe */
    z_lmControl.z_early_exit_decision.f_LastValidFixHepe = f_hepe;

    /* In tunnel-exit scenario, reinstate only after first fix w/ hepe < 250m */
    if ((z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe == TRUE) &&
        (z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart == TRUE) &&
        (z_lmControl.z_early_exit_decision.u_OverrideTimeOfFirstValidFixAfterEngineOn == FALSE))
    {
      /* Force early ramp by overriding Ter */
      z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn = q_TimeFromEngOn;
      z_lmControl.z_early_exit_decision.u_OverrideTimeOfFirstValidFixAfterEngineOn = TRUE;
    }

    /* If first fix since EngOn is not reported OR reinstate after tunnel-exit is enabled 
    ** AND the fix has hepe < 250m; then use ramp logic */
    if (((z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn == FALSE) ||
         (z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe == TRUE)) &&
        (z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart == TRUE))
    {
      /* Stricter for 4 seconds */
      if ((q_TimeFromEngOn - z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn) < LM_EARLY_RAMP_TIME_MS)
      {
        z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = (FLT)((q_TimeFromEngOn - z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn + LM_EARLY_RAMP_ADDER_MS) * q_current_hepe_limit);
        z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit /= LM_EARLY_RAMP_DIVIDER_MS;
        LM_MSG_MED("early ramp", 0, 0, 0);
      }
      /* wait up to 7 sec in session to try to reach target HEPE */
      else if ((q_TimeFromSessStart - z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart) < LM_FLAT_SECTION_TIME_MS)
      {
        z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = (FLT)q_current_hepe_limit;
        LM_MSG_MED("flat section", 0, 0, 0);
      }
      /* late-ramp for better TTF in difficult environments  */
      else if ((q_TimeFromSessStart <= z_lmControl.z_fix_request.q_timeout) &&
               ((z_lmControl.z_fix_request.q_timeout - z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart - LM_FLAT_SECTION_TIME_MS) > 0))
      {
        /* Hd = MIN( Hmax, Hc + 2*(Hmax-Hc)*(t-Tr-7)/(Tmax-Tr-7) )*/
        z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = (FLT)(2 * (q_hepe_max - q_current_hepe_limit) * (q_TimeFromSessStart - z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart - LM_FLAT_SECTION_TIME_MS));
        z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit /= (z_lmControl.z_fix_request.q_timeout - z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart - LM_FLAT_SECTION_TIME_MS);
        z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit += q_current_hepe_limit;
        if (z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit > q_hepe_max)
        {
          z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = (FLT)q_hepe_max;
        }
        LM_MSG_MED("late ramp", 0, 0, 0);
      }
      else
      {
        LM_MSG_ERROR("Dynamic QoS logic hit session timeout before timer expiry. Timeout %d, CurrSessTime %d, SessTimeOfFirstValidFIx %lu",
                     z_lmControl.z_fix_request.q_timeout, q_TimeFromSessStart, z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart);
        z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = (FLT)q_hepe_max;
      }
    }

    (void)snprintf(b_Buffer, sizeof(b_Buffer), "Override=%d Hepe=%f Dynamic Hepe Limit=%f\r\n",
                   z_lmControl.z_early_exit_decision.u_UseQoSOverride, f_hepe, z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit);
    MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", b_Buffer);

    if (f_hepe <= z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit)
    {
      if (z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn == FALSE)
      {
        z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn = TRUE;
      }
      z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe = FALSE;
      z_lmControl.z_early_exit_decision.u_TimeoutAfterReinstate = FALSE;
      z_lmControl.z_early_exit_decision.u_OverrideTimeOfFirstValidFixAfterEngineOn = FALSE;
      z_lmControl.z_early_exit_decision.e_decision = LM_EARLY_EXIT_DECISION_REPORT_FIX;
      lm_log_early_exit_decision(q_TimeFromEngOn, q_TimeFromSessStart, p_fixReport->z_NavPos.q_RefFCount, (uint16)q_current_hepe_limit, (uint16)z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit, (uint16)q_hepe_max, (uint16)f_hepe, TRUE);
      /*lint -e506 */
      return TRUE;
    }
    else
    {
      z_lmControl.z_early_exit_decision.e_decision = LM_EARLY_EXIT_DECISION_WAIT_FIX;
      lm_log_early_exit_decision(q_TimeFromEngOn, q_TimeFromSessStart, p_fixReport->z_NavPos.q_RefFCount, (uint16)q_current_hepe_limit, (uint16)z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit, (uint16)q_hepe_max, (uint16)f_hepe, TRUE);
      return FALSE;
    }
  }
  else /*if !u_dynamic_qos_enabled*/
  {
      char b_Buffer[250];
      (void)snprintf(b_Buffer, sizeof(b_Buffer), "Hepe=%f Current Hepe Limit=%ld\r\n",
                     f_hepe, q_current_hepe_limit);

      MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", b_Buffer);

    if (f_hepe <= q_current_hepe_limit)
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
}


/*
 ******************************************************************************
 * lm_mgp_config_periodic_fix_align 
 *
 * Function description:
 *
 * This function configures the PE periodical fix time and alignment of
 * position time stamp
 *
 * Parameters: 
 * void
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
static void lm_mgp_config_periodic_fix_align(void)
{
  uint32 q_Tbf_ms = z_lmControl.z_session_request.q_tbf;
  mgp_PeriodicFixAndAlignEnumType e_alignType = C_MGP_FIX_TIMER_OFF_ALIGN_NOT_REQ;

  if (z_lmControl.q_GnssCpeControl)
  {
    if (q_Tbf_ms < 1000)
    {
      e_alignType = C_MGP_FIX_TIMER_ON_ALIGN_EXP_FIRST;
    }
    else
    {
      /* This is new CPE timing control so don't align to integer second
         but report after measurements are sent from ME */
      e_alignType = C_MGP_FIX_TIMER_OFF_ALIGN_NOT_REQ;
    }
  }
  else
  {

    /* Determine the type of Fix Alignment needed based on TBF and Num Fixes requested */
    if ((q_Tbf_ms < lm_get_rc_on_threshold_period()) ||
        (z_lmControl.z_session_request.q_num_fixes == 1))
    {
      e_alignType = C_MGP_FIX_TIMER_ON_ALIGN_EXP_FIRST;
    }
    else if (0 != q_Tbf_ms)
    {
      e_alignType = C_MGP_FIX_TIMER_ON_ALIGN_ALWAYS;
    }
  }

  LM_MSG_MED("CPE Control %d, Align %d", z_lmControl.q_GnssCpeControl, e_alignType, 0);

  if (!mgp_ConfigPeriodicFixAndAlignment(e_alignType, q_Tbf_ms))
  {
    LM_MSG_ERROR("Failed to send set the requested for periodic fix; tbf=%u",
                 q_Tbf_ms, 0, 0);
  }
}
/*
 ******************************************************************************
 * lm_mgp_start_position_fix_processing 
 *
 * Function description:
 *
 * This function turns MGP engine to ON for a MS-based session.
 *
 * Parameters: 
 * p_FixReq - Pointer to structure containing Fix request from TM
 *
 * Return value: 
 *
 * boolean - MGP fix processng successfully started or not
 *
 ******************************************************************************
*/
boolean lm_mgp_start_position_fix_processing(const lm_session_request_fix_s_type *const p_FixReq)
{
  uint32  q_hepe_max            = z_lmControl.z_fix_request.q_qos_accuracy_threshold;
  uint32  q_prevShutOffTimeLeft = 0;
  uint32  q_SessionTimeRemainingMSec = 0;
  uint32  q_mgp_fix_timeout = 0;

  /* Get the current MGP operation mode before start processing the fix*/
  mgp_ConfigOperationEnumType const e_MgpCurrentOperationMode = z_lmControl.z_MgpONParams.e_OperationMode;

  /*When calling lm_mgp_update_mgp_state() to turn ON MGP, MGP might be in a transition
    state and ON command has to be delayed until MGP comes out of the transition
    state. Save the the MGP on parameters in z_lmControl so that they are readily
    available when needed to turn ON MGP*/
  mgp_ConfigStructType *pz_MgpConfig = &z_lmControl.z_MgpONParams;

  if (p_FixReq == NULL)
  {
    return FALSE;
  }

  q_SessionTimeRemainingMSec  = os_TimerCheck(z_lmControl.p_sessionTimer);
  q_mgp_fix_timeout = q_SessionTimeRemainingMSec;
  
  LM_MSG_HIGH("Session timer value left = %d ; Requested fix timer = %d", q_SessionTimeRemainingMSec, p_FixReq->q_timeout, 0);

  if (!os_TimerStart(z_lmControl.p_fixTimer, q_mgp_fix_timeout, 0))
  {
    LM_MSG_ERROR("Failed starting Fix timer", 0, 0, 0);
    return FALSE;
  }
  os_TimerStop(z_lmControl.p_sessionTimer);  
  
  (void)time_get(z_lmControl.z_early_exit_decision.d_TimeSessionStart);
  z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart = FALSE;
  z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart = 0;
  /* If first reportable fix after engine ON has been recorded */

  if ((z_lmControl.z_early_exit_decision.u_UseQoSOverride) ||
      (z_lmControl.z_fix_request.q_qos_accuracy_threshold < z_lmControl.q_HepeThreshold))
  {
    q_hepe_max = z_lmControl.q_HepeThreshold;
  }
  if (z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn)
  {
    /* Set Dynamic QoS value (Hd) to Hmax for the duration of the session */
    z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = (FLT)q_hepe_max;
  }

  /* 
   * If dynamic QoS is enabled AND
   * If its been more than 12 seconds since we started the shutOffTimer, reset the dynamic 
   * hepe limit
   */
  if (z_lmControl.z_fix_request.u_dynamic_qos_enabled)
  {
    q_prevShutOffTimeLeft = os_TimerCheck(z_lmControl.p_mgpShutOffTimer);

    if (((FLT)(z_lmControl.z_early_exit_decision.q_ShutOffTimerInitValue - (int32)q_prevShutOffTimeLeft)) >
           C_MAX_TIME_LIMIT_FOR_DPO_OPN * 1000)
    {
      LM_MSG_MED("Reinstate dynamic Hepe logic %d %d %d",
                 z_lmControl.z_early_exit_decision.q_ShutOffTimerInitValue,
                 q_prevShutOffTimeLeft,
                 (z_lmControl.z_early_exit_decision.q_ShutOffTimerInitValue - q_prevShutOffTimeLeft));
      z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe  = TRUE;
      z_lmControl.z_early_exit_decision.q_ShutOffTimerInitValue = 0;
    }
  }

  /*Stop the RC_ON timer*/
  LM_MSG_MED("Stopping RC_ON timer", 0, 0, 0);
  os_TimerStop(z_lmControl.p_rcOnTimer); /*lint !e534 */

  pz_MgpConfig->e_OperationMode     = lm_mgp_get_mgp_operation_mode(z_lmControl.z_session_request.e_mode);

  LM_ASSERT(pz_MgpConfig->e_OperationMode != (uint8)MGP_OPERATION_MAX);

  if (pz_MgpConfig->e_OperationMode == (uint8)MGP_OPERATION_MAX)
  {
    LM_MSG_ERROR("Invalid GPS operation mode received", 0, 0, 0);
    return FALSE;
  }

  pz_MgpConfig->q_SADataAvailability  = p_FixReq->q_data_availability;
  pz_MgpConfig->q_RoundTripDelayMs  = p_FixReq->q_rtd;
  pz_MgpConfig->u_Qos               = 0;  /* Not currently used by MGP. Set zero */
  pz_MgpConfig->q_SessionTimeoutMs  = q_mgp_fix_timeout;
  pz_MgpConfig->u_SmMeasRptPeriod   = 1; /*Unknown?? Measurement report period in units of GPS_MEAS_TICK */
  pz_MgpConfig->u_EnablePpmRpt      = FALSE; /*lint -e506 */
  pz_MgpConfig->u_EnablePosRpt      = TRUE;
  pz_MgpConfig->u_EnableUtcRpt      = TRUE;
  pz_MgpConfig->e_NetPlane          = (mgp_ConfigNetPlaneEnumType)z_lmControl.z_session_request.e_up_cp;
  pz_MgpConfig->u_DataDemodFlag     = z_lmControl.z_session_request.u_do_data_demod;
  pz_MgpConfig->q_QosAccuracyThreshold = p_FixReq->q_qos_accuracy_threshold;
  pz_MgpConfig->q_tbf  = z_lmControl.z_session_request.q_tbf;

  z_lmControl.u_MgpONParamsValid = TRUE;
  pz_MgpConfig->u_OdpSession        = p_FixReq->u_on_demand;
  pz_MgpConfig->q_Punc              = p_FixReq->q_Punc;
  pz_MgpConfig->q_Tunc              = p_FixReq->q_Tunc;

  MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED,
        "=LM TASK= START FIX processing with timeout[%d] old timeout[%d] SA_availability[%d] MGP opermode[%d]",
        q_mgp_fix_timeout,
        z_lmControl.z_fix_request.q_timeout,
        p_FixReq->q_data_availability,
        e_MgpCurrentOperationMode);

  if (z_lmControl.z_early_exit_decision.u_UseQoSOverride)
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED,
          "=LM TASK= Qos_accuracy[%ld]", z_lmControl.z_early_exit_decision.w_QoSThresholdOverride);
  }
  else
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED,
          "=LM TASK= Qos_accuracy[%lu]", p_FixReq->q_qos_accuracy_threshold);
  }

  LM_MSG_MED("Dynamic QoS Enabled: %d", p_FixReq->u_dynamic_qos_enabled, 0, 0);

  if (!lm_mgp_update_mgp_state(LM_MGP_STATE_ON, pz_MgpConfig, LM_UPDATE_MGP_STATE_FLAG_NONE))
  {
    os_TimerStop(z_lmControl.p_fixTimer); /*lint !e534 */
    return FALSE;
  }

  /* If ME in MSB receiver On mode & any change to timeout value then update ME.
  *  Caution : z_lmControl.z_fix_request shouldn't be cleaned between sessions.
  */
  if ((lm_is_mgp_on() == TRUE) &&
      (MGP_OPERATION_MSBASED == e_MgpCurrentOperationMode) &&
      (q_mgp_fix_timeout != z_lmControl.z_fix_request.q_timeout)
      )
  {
    mgp_UpDateAgpsAgnssSessionTimeOutMsecs(q_mgp_fix_timeout);
  }

  /*Send Fix Start Indication to MGP*/
  lm_mgp_send_fix_start_ind(z_lmControl.z_session_request.q_tbf);
  z_lmControl.v_FixStartIndPending = TRUE;
  /*Update fix session related state*/
  lm_update_session_state(LM_SESSION_STATE_IN_SESSION_REPORT_PENDING);
  z_lmControl.q_reportsPending |= (uint32)LM_SESSION_REQUEST_FIX;

  /*Report event*/
  event_report(EVENT_GPS_LM_FIX_REQUEST_START);

  /* Copy fix request params internally */
  z_lmControl.z_fix_request = *p_FixReq;

  return TRUE;
}

/*
 ******************************************************************************
 * lm_mgp_update_mgp_state 
 *
 * Function description:
 *
 * This function gets corresponding mgp_ConfigOperationEnumType for a given
 * lm_gps_mode_e_type
 *
 * Parameters: 
 * e_desiredMgpState - desired MGP state
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
boolean lm_mgp_update_mgp_state(lm_mgp_state_e_type e_desiredMgpState,
                                const mgp_ConfigStructType *p_OnParams,
                                lm_update_mgp_state_req_flag e_update_flag)
{
  boolean u_Forced = (e_update_flag & LM_UPDATE_MGP_STATE_FLAG_FORCED);
  boolean u_Transition = (e_update_flag & LM_UPDATE_MGP_STATE_FLAG_TRANSITION);
  
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Requested State = %d Current Desired State = %d Current State = %d",
        e_desiredMgpState, z_lmControl.e_desiredMgpState, z_lmControl.e_currentMgpState);

  switch (e_desiredMgpState)
  {
    case LM_MGP_STATE_ON:
      { 

         lm_mgp_config_periodic_fix_align();
         
/*REV 12/05: If the dmm_state is not loded call dmm state machine with event load(Rework on envent enum for state machine) and return*/
        if (((z_lmControl.e_desiredMgpState == LM_MGP_STATE_OFF) ||
             (z_lmControl.e_desiredMgpState == LM_MGP_STATE_IDLE)) &&
            ((z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_OFF) ||
             (z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_IDLE))
            )
        {
          /*This check is done in-order to handle the extended ON mode in MGP.
          MGP may deferr the OFF/IDLE command in extended ON mode. We may get
          another ON request and want to take MGP out of the extended ON mode.*/
  
          if (z_lmControl.e_desiredMgpState == LM_MGP_STATE_IDLE)
          {
            (void)os_TimerStop(z_lmControl.p_goingIDLETimer);
            LM_MSG_MED("Stopped GOING_IDLE Timer", 0, 0, 0);
          }
          else
          {
            (void)os_TimerStop(z_lmControl.p_goingOFFTimer);
            LM_MSG_MED("Stopped GOING_OFF Timer", 0, 0, 0);
          }
  
          z_lmControl.e_desiredMgpState = LM_MGP_STATE_ON;
  
          /*Start a timer for getting the ON notification from MGP*/
          if (!os_TimerStart(z_lmControl.p_goingONTimer, LM_GOING_ON_TIMEOUT, 0))
          {
            LM_MSG_ERROR("Failed starting GOING_ON timer", 0, 0, 0);
            return FALSE;
          }
          LM_MSG_MED("Turning MGP to ON(MGP could be in extended ON mode)", 0, 0, 0);
          
          time_get_ms(cur_timestamp_CMD_ON); // get current timestamp in secs
          LM_MSG_HIGH("DEBUG: Turning MGP to ON(MGP could be in extended ON mode), curr_time rcvr cmd ON %d %d", qw_lo(cur_timestamp_CMD_ON), qw_hi(cur_timestamp_CMD_ON), 0);
          
          mgp_ReceiverUpdate(MGP_RCVR_COMMAND_ON, p_OnParams);
          /*Update MGP state*/
          lm_set_current_mgp_state(LM_MGP_STATE_GOING_ON);
          return TRUE;
        }
  
        z_lmControl.e_desiredMgpState = LM_MGP_STATE_ON;
  
        if ((z_lmControl.e_currentMgpState == LM_MGP_STATE_OFF) ||
            (z_lmControl.e_currentMgpState == LM_MGP_STATE_IDLE) ||
            (z_lmControl.z_session_request.e_mode == LM_GPS_MODE_SA_RF))
        {
          /*Start a timer for getting the ON notification from MGP*/
          if (!os_TimerStart(z_lmControl.p_goingONTimer, LM_GOING_ON_TIMEOUT, 0))
          {
            LM_MSG_ERROR("Failed starting GOING_ON timer", 0, 0, 0);
            return FALSE;
          }
          LM_MSG_MED("Turning MGP to ON", 0, 0, 0);
          
          time_get_ms(cur_timestamp_CMD_ON); // get current timestamp in secs
          LM_MSG_HIGH("DEBUG: Turning MGP to ON, curr_time rcvr cmd ON %d %d", qw_lo(cur_timestamp_CMD_ON), qw_hi(cur_timestamp_CMD_ON), 0);
          
          mgp_ReceiverUpdate(MGP_RCVR_COMMAND_ON, p_OnParams);
          /*Update MGP state*/
          lm_set_current_mgp_state(LM_MGP_STATE_GOING_ON);
        }
        return TRUE;
      }

    case LM_MGP_STATE_OFF:
      {
      
      /*REV 12/05: if dmm state is not loaded crash*/
        z_lmControl.e_desiredMgpState = LM_MGP_STATE_OFF;
    
        /*Unsubscribe 1Hz periodic fix reporting from PE*/
        if (!mgp_ConfigPeriodicFixAndAlignment(C_MGP_FIX_TIMER_OFF_ALIGN_NOT_REQ, 0))
        {
          LM_MSG_ERROR("Failed to send the disable periodic fixes request to the PE.", 0, 0, 0);
        }
    
        if ((lm_is_mgp_on() == TRUE) ||
            (z_lmControl.e_currentMgpState == LM_MGP_STATE_IDLE))
        {
          /*Start a timer for getting the OFF notification from MGP*/
          if (!os_TimerStart(z_lmControl.p_goingOFFTimer, LM_GOING_OFF_TIMEOUT, 0))
          {
            LM_MSG_ERROR("Failed starting GOING_OFF timer", 0, 0, 0);
            return FALSE;
          }
          LM_MSG_MED("Turning MGP to OFF", 0, 0, 0);
          if (u_Forced)
          {
            time_get_ms(cur_timestamp_CMD_OFF); // get current timestamp in secs
            LM_MSG_HIGH("DEBUG: Turning MGP to Forced OFF, curr_time rcvr cmd OFF %d %d", qw_lo(cur_timestamp_CMD_OFF), qw_hi(cur_timestamp_CMD_OFF), 0);
            
            mgp_ReceiverUpdate(MGP_RCVR_COMMAND_FORCED_OFF, NULL);
          }
          else
          {
            time_get_ms(cur_timestamp_CMD_OFF); // get current timestamp in secs
            LM_MSG_HIGH("DEBUG: Turning MGP to OFF, curr_time rcvr cmd OFF %d %d", qw_lo(cur_timestamp_CMD_OFF), qw_hi(cur_timestamp_CMD_OFF), 0);
            
            mgp_ReceiverUpdate(MGP_RCVR_COMMAND_OFF, NULL);
          }
          /*Update MGP state*/
          lm_set_current_mgp_state(LM_MGP_STATE_GOING_OFF);
        }
    
        if (((z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_OFF) ||
             (z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_IDLE) ||
             (z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_ON)) &&
            (u_Forced == TRUE))
        {
          /*Start a timer for getting the OFF notification from MGP*/
          if (!os_TimerStart(z_lmControl.p_goingOFFTimer, LM_GOING_OFF_TIMEOUT, 0))
          {
            LM_MSG_ERROR("Failed starting GOING_OFF timer", 0, 0, 0);
            return FALSE;
          }
          LM_MSG_MED("Turning MGP to OFF", 0, 0, 0);
          
          time_get_ms(cur_timestamp_CMD_OFF); // get current timestamp in secs
          LM_MSG_HIGH("DEBUG: Turning MGP to Forced OFF, curr_time rcvr cmd OFF %d %d", qw_lo(cur_timestamp_CMD_OFF), qw_hi(cur_timestamp_CMD_OFF), 0);
          
          mgp_ReceiverUpdate(MGP_RCVR_COMMAND_FORCED_OFF, NULL);
    
          lm_set_current_mgp_state(LM_MGP_STATE_GOING_OFF);
        }
       /*lint -e506 */
        return TRUE;
      }

    case LM_MGP_STATE_IDLE:
      {
        /*REV 12/05: if dmm state is not loaded crash*/
          /* Unsubscribe periodic fix reporting from PE */
        if (!mgp_ConfigPeriodicFixAndAlignment(C_MGP_FIX_TIMER_OFF_ALIGN_NOT_REQ, 0))
        {
          LM_MSG_ERROR("Failed to send the disable periodic fixes request to the PE.", 0, 0, 0);
        }
        if (((z_lmControl.e_desiredMgpState == LM_MGP_STATE_OFF) ||
             (z_lmControl.e_desiredMgpState == LM_MGP_STATE_ON) ||
             (z_lmControl.e_desiredMgpState == LM_MGP_STATE_IDLE)) &&
            ((z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_OFF) ||
             (z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_ON) ||
             (z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_IDLE)
             ) &&
            (u_Forced | u_Transition))
        {
          /*This check is done in-order to handle the extended ON mode in MGP.
          MGP may deferr the OFF/IDLE command in extended ON mode. We may get
          another forced IDLE request and want to take MGP out of the extended ON mode.*/
  
          if (z_lmControl.e_desiredMgpState == LM_MGP_STATE_IDLE)
          {
            (void)os_TimerStop(z_lmControl.p_goingIDLETimer);
            LM_MSG_MED("Stopped GOING_IDLE Timer", 0, 0, 0);
          }
          else
          {
            (void)os_TimerStop(z_lmControl.p_goingOFFTimer);
            LM_MSG_MED("Stopped GOING_OFF Timer", 0, 0, 0);
          }
  
          z_lmControl.e_desiredMgpState = LM_MGP_STATE_IDLE;
  
          /*Dont Start IDLE timer for IDLE TRANSITIONING. It is just an intermediate state*/
          if (FALSE == u_Transition)
          {
            /*Start a timer for getting the IDLE notification from MGP*/
            if (!os_TimerStart(z_lmControl.p_goingIDLETimer, LM_GOING_IDLE_TIMEOUT, 0))
            {
              LM_MSG_ERROR("Failed starting GOING_IDLE timer", 0, 0, 0);
              return FALSE;
            }
          }
  
          if (u_Forced)
          {
            LM_MSG_MED("Turning MGP to IDLE(Forced)", 0, 0, 0);
            
            time_get_ms(cur_timestamp_CMD_IDLE); // get current timestamp in secs
            LM_MSG_HIGH("DEBUG: Turning MGP to forced IDLE, curr_time rcvr cmd IDLE %d %d", qw_lo(cur_timestamp_CMD_IDLE), qw_hi(cur_timestamp_CMD_IDLE), 0);
            
            mgp_ReceiverUpdate(MGP_RCVR_COMMAND_FORCED_IDLE, NULL);
          }
          else if (u_Transition)
          {
            LM_MSG_MED("Turning MGP to IDLE(Transition)", 0, 0, 0);
            
            time_get_ms(cur_timestamp_CMD_IDLE); // get current timestamp in secs
            LM_MSG_HIGH("DEBUG: Turning MGP to transition IDLE, curr_time rcvr cmd IDLE %d %d", qw_lo(cur_timestamp_CMD_IDLE), qw_hi(cur_timestamp_CMD_IDLE), 0);
            
            mgp_ReceiverUpdate(MGP_RCVR_COMMAND_IDLE_TRANSITIONING, NULL);
          }
          else
          {
            LM_MSG_MED("Turning MGP to IDLE", 0, 0, 0);
            
            time_get_ms(cur_timestamp_CMD_IDLE); // get current timestamp in secs
            LM_MSG_HIGH("DEBUG: Turning MGP to IDLE, curr_time rcvr cmd IDLE %d %d", qw_lo(cur_timestamp_CMD_IDLE), qw_hi(cur_timestamp_CMD_IDLE), 0);

            mgp_ReceiverUpdate(MGP_RCVR_COMMAND_IDLE, NULL);
          }
  
          /*Update MGP state*/
          lm_set_current_mgp_state(LM_MGP_STATE_GOING_IDLE);
  
          return TRUE;
        }
  
        z_lmControl.e_desiredMgpState = LM_MGP_STATE_IDLE;
  
        LM_MSG_MED("LM Desired State = %d, Current State = %d", z_lmControl.e_desiredMgpState, z_lmControl.e_currentMgpState, 0);
  
        if ((z_lmControl.e_currentMgpState == LM_MGP_STATE_ON) ||
            (z_lmControl.e_currentMgpState == LM_MGP_STATE_OFF) ||
            (z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_ON)
            )
        {
          /*Dont Start IDLE timer for IDLE TRANSITIONING. It is just an intermediate state*/
          if (FALSE == u_Transition)
          {
            /*Start a timer for getting the IDLE notification from MGP*/
            if (!os_TimerStart(z_lmControl.p_goingIDLETimer, LM_GOING_IDLE_TIMEOUT, 0))
            {
              LM_MSG_ERROR("Failed starting GOING_IDLE timer", 0, 0, 0);
              return FALSE;
            }
          }
  
          if (u_Forced)
          {
            LM_MSG_MED("Turning MGP to IDLE(Forced)", 0, 0, 0);
            
            time_get_ms(cur_timestamp_CMD_IDLE); // get current timestamp in secs
            LM_MSG_HIGH("DEBUG: Turning MGP to forced IDLE, curr_time rcvr cmd IDLE %d %d", qw_lo(cur_timestamp_CMD_IDLE), qw_hi(cur_timestamp_CMD_IDLE), 0);
            
            mgp_ReceiverUpdate(MGP_RCVR_COMMAND_FORCED_IDLE, NULL);
          }
          else if (u_Transition)
          {
            LM_MSG_MED("Turning MGP to IDLE(Transition)", 0, 0, 0);
            
            time_get_ms(cur_timestamp_CMD_IDLE); // get current timestamp in secs
            LM_MSG_HIGH("DEBUG: Turning MGP to transition IDLE, curr_time rcvr cmd IDLE %d %d", qw_lo(cur_timestamp_CMD_IDLE), qw_hi(cur_timestamp_CMD_IDLE), 0);
            
            mgp_ReceiverUpdate(MGP_RCVR_COMMAND_IDLE_TRANSITIONING, NULL);
          }
          else
          {
            LM_MSG_MED("Turning MGP to IDLE", 0, 0, 0);
            
            time_get_ms(cur_timestamp_CMD_IDLE); // get current timestamp in secs
            LM_MSG_HIGH("DEBUG: Turning MGP to IDLE, curr_time rcvr cmd IDLE %d %d", qw_lo(cur_timestamp_CMD_IDLE), qw_hi(cur_timestamp_CMD_IDLE), 0);
            
            mgp_ReceiverUpdate(MGP_RCVR_COMMAND_IDLE, NULL);
          }
  
          /*Update MGP state*/
          lm_set_current_mgp_state(LM_MGP_STATE_GOING_IDLE);
        }
        return TRUE;
      }
    default:
      {
        return FALSE;
      }
  } /*switch(e_desiredMgpState) ends*/

}


/*
 ******************************************************************************
 * lm_mgp_stop_position_fix_processing 
 *
 * Function description:
 *
 * This function stops a position fix processing. It will stop the position fix
 * timer and turn off MGP. 
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
void lm_mgp_stop_position_fix_processing(boolean u_forceMgpOff)
{
  lm_update_mgp_state_req_flag e_update_flag = LM_UPDATE_MGP_STATE_FLAG_NONE;

  LM_MSG_MED("Stopping FIX timer", 0, 0, 0);
  os_TimerStop(z_lmControl.p_fixTimer); /*lint !e534 */
  /*Stop all the MGP engine maintenance related timers to avoid un-neccessary
    timeout events*/
  LM_MSG_MED("Stopping RC_ON and SHUT_OFF timers", 0, 0, 0);
  os_TimerStop(z_lmControl.p_mgpShutOffTimer); /*lint !e534 */
  os_TimerStop(z_lmControl.p_rcOnTimer); /*lint !e534 */

  z_lmControl.q_reportsPending &= ~((uint32)LM_SESSION_REQUEST_FIX);
  /*Report event only if a session is active */

  qw_set(z_lmControl.z_early_exit_decision.d_TimeSessionStart, 0, 0);

  if (TRUE == u_forceMgpOff)
  {
    e_update_flag = LM_UPDATE_MGP_STATE_FLAG_FORCED;
  }

  /* If SARF is in progress, then do not turn the RCVR OFF */
  if (TRUE == lm_mgp_get_SARF_status())
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "SARF (RF testing) in progress. NOT turning RCVR OFF");
    if (z_lmControl.e_sessionState != LM_SESSION_STATE_NO_SESSION)
    {
      event_report(EVENT_GPS_LM_FIX_REQUEST_END);
    }
    return;
  }

  if (!lm_mgp_update_mgp_state(LM_MGP_STATE_OFF, NULL, e_update_flag))
  {
    LM_MSG_HIGH("State update to LM_MGP_STATE_OFF failed ", 0, 0, 0);
  }

  if (z_lmControl.e_sessionState != LM_SESSION_STATE_NO_SESSION)
  {
    event_report(EVENT_GPS_LM_FIX_REQUEST_END);
  }

}

/*
 ******************************************************************************
 * lm_mgp_calculate_mag_deviation
 *
 * Function description:
 *
 * This function calculated the Mag deviation from Fix report
 *
 * Parameters: 
 * p_fixReport - Pointer to Fix Report Structure
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
static void lm_mgp_calculate_mag_deviation(sm_FixInfoStructType *p_fixReport)
{
  FLT   f_MagDev = 0.0;

  if (p_fixReport->z_NavPos.u_MagDevGood)
  {
    f_MagDev = p_fixReport->z_NavPos.f_MagDeviation;
  }
  else
  {
    /* If Mag Dev is not available, attempt to compute it */
    if (cgps_MagDeviation(p_fixReport->z_NavPos.d_PosLla[0], p_fixReport->z_NavPos.d_PosLla[1], &f_MagDev) == TRUE)
    {
      /*Calculation of Mag Dev Succesfull*/
      p_fixReport->z_NavPos.u_MagDevGood = TRUE;
    }
    else
    {
      p_fixReport->z_NavPos.u_MagDevGood = FALSE;
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Mag Dev was not present and could not be calculated", 0, 0, 0);
    }
  }

  p_fixReport->z_NavPos.f_MagDeviation = f_MagDev;
}


/*
 ******************************************************************************
 * lm_mgp_generate_log_packet
 *
 * Function description:
 *   This function generates 1370 and 1476 log packets from LM
 *
 * Parameters:
 *   p_fixReport - Pointer to structure containing the fix report
 *
 * Return value:
 *   None
 *
 ******************************************************************************
*/
static void lm_mgp_generate_log_packet(const sm_GpsFixRptStructType *p_fixReport)
{
#ifdef FEATURE_GEN7_SM_PE_API
  #error code not present
#else
  gnss_NavSolutionStructType z_navSolution = { 0 };
#endif

  if (p_fixReport == NULL)
  {
    LM_MSG_ERROR("NULL p_fixReport", 0, 0, 0);
    return;
  }

#ifdef FEATURE_GEN7_SM_PE_API
  #error code not present
#else /* FEATURE_GEN7_SM_PE_API */
  /* log the 0x1476 log packet from LM. */
  memset((void *)&z_navSolution, 0, sizeof(z_navSolution));
  if (sm_translate_sm_fix_report_to_gnss_nav_solution(&z_navSolution, p_fixReport, FALSE,
                                                      z_lmControl.z_fix_request.u_on_demand))
  {
    gnss_LogPositionReport(&z_navSolution, NULL);
    /* count = NULL because we're not keeping track of how many messages we've sent out */
  }
#endif /* FEATURE_GEN7_SM_PE_API */
}

/*
 ******************************************************************************
* lm_mgp_is_fix_source_reportable
*
* Function description:
*   This function decides if a fix generated from PE is reportable as intermediate/final to SM or not.
*   All GNSS fixes are reportable. CPI fixes are reportable as final fixes, if NV for injecting CPIs is enabled AND
*   if in E911 session AND if reliabilty is medium or better.  For intermediate fixes, reliability is not checked.
*
* Parameters:
*   p_fixReport - Pointer to structure containing the fix report from PE
*   v_reportableFinalfix:
*       TRUE: This fix is considered for final fix
*       FALSE: This fix is considered for intermediate fix
*
* Return value:
*   TRUE: If the fix is reportable as intermediate/final fix (based on input param v_reportableFinalfix)
*   FALSE: If the fix is NOT reportable as intermediate/final (based on input param v_reportableFinalfix)
 *
 ******************************************************************************
*/
boolean lm_mgp_is_fix_source_reportable(const sm_GpsFixRptStructType *p_fixReport, boolean v_reportableFinalfix)
{
  boolean retVal = FALSE;

  if (p_fixReport == NULL)
  {
    LM_MSG_ERROR("NULL p_fixReport", 0, 0, 0);
    return FALSE;
  }

  if (!(p_fixReport->z_NavPos.z_SmPosFlags.b_IsPosBestAvail)) /*Checks if its GNSS fix */
  {
    retVal = TRUE;
  }
  /* CPI fixes are reportable if NV for injecting CPI is enabled and if in E911 session. */
 
  

  return retVal;

}

/*
******************************************************************************
* lm_mgp_send_intermediate_report
*
* Function description:
*   This function sends a fix report as intermediate to TM
*
* Parameters:
*   p_fixReport - Pointer to structure containing the fix report from PE
*
* Return value:
*   None
*
******************************************************************************
*/
static void lm_mgp_send_intermediate_report(const sm_GpsFixRptStructType *p_fixReport)
{
  if (NULL == p_fixReport)
  {
    LM_MSG_ERROR("NULL Pointer received at lm_mgp_send_intermediate_report", 0, 0, 0);
    return;
  }
  lm_mgp_generate_log_packet(p_fixReport);
  /*Send diag position report to TM and generate NMEA if enabled*/
  if (!lm_tm_send_position_report_to_tm(p_fixReport, FALSE))
  {
    LM_MSG_ERROR("Unable to send position report to TM", 0, 0, 0);
  }
}

/*
 ******************************************************************************
 * lm_mgp_handle_fix_report
 *
 * Function description:
 *
 * This function handles the position fix report from MGP
 *
 * Parameters: 
 * p_msg - Pointer to structure containing IPC message with position fix
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_fix_report(os_IpcMsgType *p_msg)
{
  sm_FixReportStructType *p_PosReport_from_mgp;
  sm_FixInfoStructType *p_fixReport;
  boolean bBestAvailPos = FALSE;
  boolean bSensorPropogatedGNSSFix = FALSE;
  boolean bOEMDREReport = FALSE;
  float INSOnlyFixHepe = 0;
  os_TimerMsgType* gtp_wwan_fix_timer = NULL;
  
  
  if (NULL == p_msg)
  {
    LM_MSG_ERROR("NULL Pointer received", 0, 0, 0);
    return;
  }

  if (z_lmControl.z_lastFixRpt==NULL)
  {
   z_lmControl.z_lastFixRpt = (sm_FixInfoStructType*)os_MemAlloc(sizeof(sm_FixInfoStructType), OS_MEM_SCOPE_TASK);
  }

  p_PosReport_from_mgp = (sm_FixReportStructType *)p_msg->p_Data;
  if (NULL == p_PosReport_from_mgp)
  {
    LM_MSG_ERROR("NULL POS Pointer received from MGP", 0, 0, 0);
    return;
  }

  p_fixReport = &p_PosReport_from_mgp->z_FixInfo;
  if (NULL == p_fixReport)
  {
    LM_MSG_ERROR("NULL Fix Report Pointer received from MGP", 0, 0, 0);
    return;
  }

  /*If this is a N Hz session, check if the fix report recieved is OEM DRE report*/
  if ((z_lmControl.z_session_request.q_tbf < 1000) &&
      (p_fixReport->z_NavPos.z_PosFlags.b_IsOemDreFix))
  {
    bOEMDREReport = TRUE;
  }

  if (SM_REQ_TYPE_GET_POS_EST == p_PosReport_from_mgp->b_ReqType)
  {
    return;
  }

  /*If its is a GPS state info request, send the state info to TM */
  if ((p_PosReport_from_mgp->b_ReqType >= SM_GPS_STATE_INFO_REQ_TYPE_MIN) &&
      (p_PosReport_from_mgp->b_ReqType <= SM_GPS_STATE_INFO_REQ_TYPE_MAX)
      )
  {
    tm_api_update_gps_state_info(p_PosReport_from_mgp);
    return;
  }
   /*if MGP is off and we are getting a fix that means its a GTP WWAN fix*/

  /*If its is a GPS state info request, send the state info to TM */
  if ((p_PosReport_from_mgp->b_ReqType >= SM_REQ_TYPE_WIFI_POS_FOR_MIN) &&
      (p_PosReport_from_mgp->b_ReqType <=  SM_REQ_TYPE_WIFI_POS_FOR_MAX)
      )
  {
    /*TODO: Need to take out hardcoding and send all wifi pos to */
    
    return;
  }

  /*If its is a GNSS LE only fix. Send to TM directly */
  
  if (p_PosReport_from_mgp->b_ReqType != SM_GPS_STATE_INFO_REQ_TYPE_NONE)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unsupported Request type %d", p_PosReport_from_mgp->b_ReqType, 0, 0);
    return;
  }

  /*If we are here, then its a position fix report*/
  if (p_fixReport->z_NavPos.z_SmPosFlags.b_IsPosBestAvail)
  {
    bBestAvailPos = TRUE; /* original NO_FIX report: CellDB fix, WiFi fix etc non-GNSS fix */
  }
  else
  {
    z_lmControl.b_gnssFixReceivedThisSession = TRUE;
  }

  LM_MSG_HIGH("Received FIX REPORT from MGP, Best Position = %d, gnss_fix_received_this_session = %d",
             (uint8)bBestAvailPos, (uint8)z_lmControl.b_gnssFixReceivedThisSession, 0);

  /* If NV is set to enabled to inject CPI fixes */

  LM_MSG_MED("Received FIX REPORT from MGP (BestAvailPos=%d, INSOnlyFix=%d)", (uint32)bBestAvailPos, (uint32)bSensorPropogatedGNSSFix, 0);
  
  if (z_lmControl.z_lastFixRpt==NULL)
  {
	MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory");
	return;
  }

  if ((z_lmControl.e_sessionState == LM_SESSION_STATE_IN_SESSION_REPORT_PENDING) &&
      (z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_FIX) &&
      (bOEMDREReport != TRUE)) /*For NHz session, dont forward OEMDRE fix report to QMI iface*/
  {
    if (lm_mgp_is_fix_source_reportable(p_fixReport, FALSE))
    {
      /*Save the fix report to be used when session timesout*/
     // z_lmControl.z_lastFixRpt = p_fixReport; /*lint -e506 */
	  memscpy(z_lmControl.z_lastFixRpt, 
		      sizeof(*(z_lmControl.z_lastFixRpt)),
		      p_fixReport, sizeof(*p_fixReport));
      z_lmControl.u_lastFixRptValid = TRUE;
      z_lmControl.u_lastFixWasNoFixRpt = FALSE;

      /*Calculate Mag Deviation*/
      lm_mgp_calculate_mag_deviation(p_fixReport);

      /* Save the lowest HEPE fix for a session */
      lm_mgp_set_sess_best_fix_report(p_fixReport);

      /* If E911 NILR MSB or E911 Standalone */

      
      if ((p_fixReport->z_NavPos.e_HoriRelIndicator != PDSM_POSITION_RELIABILITY_VERY_LOW) &&
               /* Verify position accuracy check only for non E911 cases */
               (lm_mgp_is_fix_source_reportable(p_fixReport, TRUE))
               )
      {
        if (lm_mgp_position_accuracy_check(p_fixReport))
        {
          /* handle final report (will do 0x1476 logging and do post-fix handling there) */
          lm_tm_handle_final_position_report(p_fixReport);
          /* Once final position is reported, reset this flag for next session */
          z_lmControl.b_gnssFixReceivedThisSession = FALSE;
        }
        else if (TRUE == bSensorPropogatedGNSSFix)
        {
          /* Calculate HEPE for INS ONLY Fix */
          INSOnlyFixHepe = (FLT)sqrt((p_fixReport->z_NavPos.f_ErrorEllipse[1] *
                                      p_fixReport->z_NavPos.f_ErrorEllipse[1]) +
                                     (p_fixReport->z_NavPos.f_ErrorEllipse[2] *
                                      p_fixReport->z_NavPos.f_ErrorEllipse[2]));

          LM_MSG_MED("INS ONLY fix HEPE %d and INS/DR HEPE threshold %d", INSOnlyFixHepe, z_lmControl.q_HepeThresholdForDRFix, 0);
          /* If INS ONLY fix and HEPE <= 250m, send it out as final fix, even if it does not meet the dynamic HEPE check */
          if (INSOnlyFixHepe <= (float)z_lmControl.q_HepeThresholdForDRFix)
          {
            LM_MSG_MED("Reporting INS ONLY fix as FINAL Fix with HEPE %d, even if INS HEPE does not meet dynamic HEPE accuracy", INSOnlyFixHepe, 0, 0);
            /* handle final report (will do 0x1476 logging and do post-fix handling there) */
            lm_tm_handle_final_position_report(p_fixReport);
            /* Once final position is reported, reset this flag for next session */
            z_lmControl.b_gnssFixReceivedThisSession = FALSE;
          }
          else
          {
            lm_mgp_send_intermediate_report(p_fixReport);
          }
        }
        else
        {
          lm_mgp_send_intermediate_report(p_fixReport);
        }
      }
      else
      {
        lm_mgp_send_intermediate_report(p_fixReport);
      }
    }
    else
    {
      /* original NO_FIX report: Increment null fix report counter */
      z_lmControl.z_early_exit_decision.q_NullFixReportCount++;
      LM_MSG_LOW("NULL fix report counter incremented to %d", z_lmControl.z_early_exit_decision.q_NullFixReportCount, 0, 0);
      lm_mgp_send_intermediate_report(p_fixReport);
    }


    if (bBestAvailPos == FALSE)
    {
      /* Reset null fix report counter */
      z_lmControl.z_early_exit_decision.q_NullFixReportCount = 0;
      LM_MSG_LOW("NULL fix report counter reset done", 0, 0, 0);

    }
    /*For 1Hz session, we dont recieve a seperate OEMDRE report.
      Hence forward the fix report to OEM DRE interface also*/
    if (z_lmControl.z_session_request.q_tbf >= 1000)
    {
      bOEMDREReport = TRUE;
    }
  }
  else
  {
    lm_mgp_generate_log_packet(p_fixReport);
  }

}

/*
 ******************************************************************************
 * lm_mgp_update_mgp_state_after_final_fix
 *
 * Function description:
 *
 * This function is called after the final fix(PRM or position fix) for a session is
 * reported. Depending on the value of N(number of fixes) and T(time between fixes)
 * for a application based tracking session, it will decide whether to keep the
 * MGP engine ON,IDLE or OFF. It also starts timers to eventually turn MGP OFF if 
 * the expected next position fix request doesn't arrive.
 *
 * Parameters: 
 * q_num_fixes - value of number of fixes for the session (N)
 * q_tbf       - value of time between fixes for the session (T)
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_update_mgp_state_after_final_fix(uint32 q_num_fixes, uint32 q_tbf)
{
  uint32 q_T1;

  LM_MSG_MED("lm_mgp_update_mgp_state_after_final_fix tbf %lu num_fixes %lu", q_tbf, q_num_fixes, 0);
  
  if (q_num_fixes > 1) /*Means app based tracking*/
  {
    q_T1              = lm_get_rc_on_threshold_period();

	if (q_tbf < q_T1)
	{
    	/*Keep MGP in ON state and start RC_ON timer*/
       LM_MSG_MED("Starting RC_ON timer with value %d", q_T1, 0, 0);
       (void)os_TimerStart(z_lmControl.p_rcOnTimer,q_T1,0);	   
	} 
	else /* q_tbf>=q_t1 case*/
    {
	   if (!lm_mgp_update_mgp_state(LM_MGP_STATE_OFF, NULL, LM_UPDATE_MGP_STATE_FLAG_NONE))
       {
           LM_MSG_HIGH("State update to LM_MGP_STATE_OFF failed", 0, 0, 0);
       }
	}

  } /*if(z_lmControl.z_fix_request.q_num_fixes > 1) ends*/
  else /*Else change MGP to OFF*/
  {
   if (!lm_mgp_update_mgp_state(LM_MGP_STATE_OFF, NULL, LM_UPDATE_MGP_STATE_FLAG_NONE))
    {
        LM_MSG_HIGH("State update to LM_MGP_STATE_OFF failed", 0, 0, 0);
    }
  }

}



/*
 ******************************************************************************
 * lm_mgp_rcvr_state_change
 *
 * Function description:
 *
 * This function handles the MGP status(ON/OFF/IDLE) updated coming from MGP.
 *
 * Parameters: 
 * e_mgpState - new MGP state
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_rcvr_state_change(lm_mgp_state_e_type e_mgpState)
{
  lm_mgp_state_e_type e_prevMgpState;
  if (e_mgpState == z_lmControl.e_currentMgpState)
  {
    /* current state the same as the reported new state, no action needed 
    one scenario this could happen is that since MGP always responds to
    LM's receiverON command, if by the time MGP receives receiverON it is
    already ON and has already sent an unsolicitated state change, another
    mgpState ON will be sent here */
    LM_MSG_MED("RCVR state report: same as current %u", e_mgpState, 0, 0);
    return;
  }

  if( (LM_MGP_STATE_OFF == e_mgpState) && (LM_MGP_STATE_OFF != z_lmControl.e_desiredMgpState))
  {
	time_get_ms(cur_timestamp_random_off);
    LM_MSG_HIGH("Debug: Random RCVR OFF is received at %d %d", qw_lo(cur_timestamp_random_off), qw_hi(cur_timestamp_random_off), 0);

	lm_tm_handle_gnss_failure(TM_ERROR_MGP_RANDOM_RCVR_OFF_ERROR);
	u_rcvr_random_off_counter++;
	return;
  }
  /**** proceed with the new, changed state *******************************/
  e_prevMgpState  = z_lmControl.e_currentMgpState;
  /*Update current state to the new state*/
  lm_set_current_mgp_state(e_mgpState);

  /*Stop the intermediate MGP state timer*/
  if (e_mgpState == LM_MGP_STATE_ON)
  {
    LM_MSG_HIGH("RCVR state changed to ON", 0, 0, 0);
#ifdef TEST_UNLOAD_TRIGGER_FROM_INTERNAL_CODE_LOCATIONS
    trigger_gnss_unload(2);
#endif

    os_TimerStop(z_lmControl.p_goingONTimer); /*lint !e534 */

    time_get_ms(cur_timestamp_ACK_ON);
    LM_MSG_HIGH("Debug: Ack On received and RCVR state changed to ON at %d %d", qw_lo(cur_timestamp_ACK_ON), qw_hi(cur_timestamp_ACK_ON), 0);

    /*Make sure the event goes out only first time we recieve it from MGP
    given, MGP might send this update multiple times*/
    if (LM_MGP_STATE_GOING_ON == e_prevMgpState)
    {
      /*Notify TM to generate the PD-API event*/
      (void)lm_tm_send_pd_api_event_to_tm(TM_PD_EVENT_GPS_BEGIN);


      /* Reset time of first reportable fix */
      z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn = 0;
      z_lmControl.z_early_exit_decision.u_FirstValidFixAfterEngineOn = FALSE;
      (void)time_get(z_lmControl.z_early_exit_decision.d_TimeEngOn);
      z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn = FALSE;

      z_lmControl.z_early_exit_decision.f_LastValidFixHepe = LM_LAST_VALID_FIX_HEPE_MAX;
      z_lmControl.z_early_exit_decision.q_NullFixReportCount = 0;
      z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe = FALSE;
      z_lmControl.z_early_exit_decision.u_TimeoutAfterReinstate = FALSE;
      z_lmControl.z_early_exit_decision.u_OverrideTimeOfFirstValidFixAfterEngineOn = FALSE;
      /*Generate CGPS event*/
      sm_log_event_report_ex(LM_LOG_EVENT_ID_MGP_STATE_CHANGE, LM_LOG_EVENT_DATA_MGP_STATE_ON);
      /*Report Event*/
      event_report(EVENT_GPS_LM_MGP_ON);
    }

    if (z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_GOING_ON)
    {
      z_lm_sarf_state_db.e_sarf_state = LM_SARF_MODE_ON;
      
      event_report( EVENT_GPS_LM_ENTER_SA_RF_VERIF );
    }
    /*Trigger XTRA injection if the reciever is turned on 
      for first time after MGP loaded.
    */
    if(TRUE == z_lmControl.v_XtraInjectionPending)
    {
      tm_api_request_xtra_inject_to_mgp();
      z_lmControl.v_XtraInjectionPending = FALSE;
    }
  } /*if(e_mgpState == LM_MGP_STATE_ON)*/
  else if (e_mgpState == LM_MGP_STATE_OFF)
  {
    LM_MSG_HIGH("RCVR state changed to OFF", 0, 0, 0);
#ifdef TEST_UNLOAD_TRIGGER_FROM_INTERNAL_CODE_LOCATIONS
    trigger_gnss_unload(3);
#endif
    os_TimerStop(z_lmControl.p_goingOFFTimer); /*lint !e534 */

    time_get_ms(cur_timestamp_ACK_OFF);
    LM_MSG_HIGH("Debug: Ack Off received and RCVR state changed to Off at %d %d", qw_lo(cur_timestamp_ACK_OFF), qw_hi(cur_timestamp_ACK_OFF), 0);

    /*   
       if sarf test is not going on then irrespective of GNSS session state RCVR OFF ACK should be honoured
   
       if SARF test is going on then SARF state should be going off.
     */

    if ((v_lm_sarf_state == FALSE) ||
        (z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_GOING_OFF))
    {
      (void)lm_dmm_state_machine(LM_DMM_EVENT_RCVR_OFF_IND);
    }
    /*Notify TM to generate the PD-API event*/
    (void)lm_tm_send_pd_api_event_to_tm(TM_PD_EVENT_GPS_DONE);

    /* Reset time of first reportable fix */
    lm_reset_dynamic_hepe_thresold();
    /*Report Event*/
    event_report(EVENT_GPS_LM_MGP_OFF);
    if (z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_GOING_OFF)
    {
      z_lm_sarf_state_db.e_sarf_state = LM_SARF_MODE_SLEEP;
      
      event_report(EVENT_GPS_LM_EXIT_SA_RF_VERIF);
      lm_mgp_report_SARF_status(FALSE);
    }
  } /*else if(e_mgpState == LM_MGP_STATE_OFF)*/
  else if (e_mgpState == LM_MGP_STATE_IDLE)
  {
    LM_MSG_HIGH("RCVR state changed to IDLE", 0, 0, 0);
#ifdef TEST_UNLOAD_TRIGGER_FROM_INTERNAL_CODE_LOCATIONS
    trigger_gnss_unload(4);
#endif
    os_TimerStop(z_lmControl.p_goingIDLETimer); /*lint !e534 */

    time_get_ms(cur_timestamp_ACK_IDLE);
    LM_MSG_HIGH("Debug: Ack Idle received and RCVR state changed to Idle at %d %d", qw_lo(cur_timestamp_ACK_IDLE), qw_hi(cur_timestamp_ACK_IDLE), 0);

    /*Notify TM about MGP IDLE state. This is not really a PDAPI event*/
    (void)lm_tm_send_pd_api_event_to_tm(TM_PD_EVENT_GPS_IDLE);
    /*Generate CGPS event*/
    sm_log_event_report_ex(LM_LOG_EVENT_ID_MGP_STATE_CHANGE, LM_LOG_EVENT_DATA_MGP_STATE_IDLE);
    /*Report Event*/
    event_report(EVENT_GPS_LM_MGP_IDLE);
  } /*else if(e_mgpState == LM_MGP_STATE_IDLE)    */
  /*If the new state is not the desired state, update the MGP state to the 
  desired state*/
  if (z_lmControl.e_desiredMgpState != z_lmControl.e_currentMgpState)
  {
    if (z_lmControl.e_desiredMgpState == LM_MGP_STATE_ON)
    {
      LM_ASSERT(z_lmControl.u_MgpONParamsValid);

      if (z_lmControl.u_MgpONParamsValid)
      {
        if (!lm_mgp_update_mgp_state(z_lmControl.e_desiredMgpState, &z_lmControl.z_MgpONParams, LM_UPDATE_MGP_STATE_FLAG_NONE))
        {
          LM_MSG_HIGH("State update to %d failed", z_lmControl.e_desiredMgpState, 0, 0);
        }
      }
      else
      {
        LM_MSG_ERROR("MGP ON parameters not valid for handle[%d]", z_lmControl.q_sessHandle, 0, 0);
      }
    }
    else
    {
      if (!lm_mgp_update_mgp_state(z_lmControl.e_desiredMgpState, NULL, LM_UPDATE_MGP_STATE_FLAG_NONE))
      {
        LM_MSG_HIGH("State update to %d failed", z_lmControl.e_desiredMgpState, 0, 0);
      }
    }
  }


}

/*
 ******************************************************************************
 * lm_mgp_handle_status_update
 *
 * Function description:
 *
 * This function handles the MGP status updates coming from MGP.
 *
 * Parameters: 
 * *p_msg - IPC message containing the MGP status update
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_status_update(os_IpcMsgType *p_msg)
{
  sm_RcvrStateChangeStructType *p_rcvrState = NULL;
  lm_mgp_state_e_type e_mgpState;
  lm_recvr_state_event_info rcvr_state_event_payload = { 0 };

  if (p_msg == NULL)
  {
    return;
  }
  /*lint -e826 */
  p_rcvrState = (sm_RcvrStateChangeStructType *)p_msg->p_Data;
  /*lint +e826 */

  /*Report QXDM Event on GNSS Receiver state*/
  rcvr_state_event_payload.q_GpsRtcMs = p_rcvrState->q_GpsRtcMs;
  rcvr_state_event_payload.q_receiver_state = (uint32)p_rcvrState->e_CurrentRcvrState;
  event_report_payload(EVENT_GNSS_RCVR_STATE, sizeof(rcvr_state_event_payload), (void *)&(rcvr_state_event_payload));

  /*Send the current MGP state to TechSel*/
  

  switch (p_rcvrState->e_CurrentRcvrState)
  {
    case C_GPS_RCVR_STATE_OFF:                /* Receiver State is OFF */
    case C_GPS_RCVR_STATE_IDLE:               /* Receiver State is IDLE */
    case C_GPS_RCVR_STATE_ON:                 /* Receiver State is ON */
      {
        e_mgpState = lm_mgp_get_mgp_state_type(p_rcvrState);

        LM_ASSERT(e_mgpState != LM_MGP_STATE_INVALID);
        lm_mgp_rcvr_state_change(e_mgpState);

        /*Invalidate the ON parameters since they are to be used only 
          once to call mgp ReceiverOn()*/
        if (e_mgpState == LM_MGP_STATE_ON)
        {
          z_lmControl.u_MgpONParamsValid = FALSE;
        }
        /*Validate the ON parameters if MGP in LPPM mode.
               This is done so that SM can call mgp Receiver On to put MGP out of LPPM mode later*/
        break;
      }

    case C_GPS_RCVR_STATE_TRANSITIONING:
    case C_GPS_RCVR_STATE_DPO_SLEEP:          /* DPO Sleep is a transition state for Rcvr ON */
      {
        /*Do nothing*/
        break;
      }
    default:
      break;
  } /*switch (p_rcvrState->e_CurrentRcvrState) ends*/

}


/*
 ******************************************************************************
 * lm_meas_status_bits_check
 *
 * Function description:
 *
 * Function checking SV measurement status bits to determin if the SV is
 * to be included in the measrement report to the network (this will  
 * affect both NMEA and QMI output)  
 * Also, this function can do optional filtering (featurized) based on SV's  
 * freshness status bit.
 *
 * Parameters: 
 * q_sv_meas_status - SV measurement status bits
 * w_Cno - CNo. Units of 0.1 dB
 * e_SvState - SV State
 *
 * Return value: TRUE/FALSE
 *
 ******************************************************************************
*/
boolean lm_meas_status_bits_check(uint32 q_sv_meas_status, uint16 w_Cno, gnss_SvStateEnumType e_SvState)
{
  /* SV freshness-based filtering (freshness bit is defined in gnss_common.h) */
#ifdef FEATURE_GNSS_SV_FRESHNESS_FILTERING 
  if ( !( q_sv_meas_status & ( MEAS_STATUS_GNSS_FRESH_MEAS ) ) ) 
  {
    return FALSE;
  }
#endif

  if ( q_sv_meas_status & ( MEAS_STATUS_XCORR ) ) 
  {
    return FALSE;
  }
  else /* XCORR & DONT_USE bits not set*/
  {
    if( q_sv_meas_status & ( MEAS_STATUS_FROM_RNG_DIFF | MEAS_STATUS_FROM_VE_DIFF ) )
    {
      if(e_SvState == C_GNSS_SV_STATE_DPO_TRACK)
      {
        return FALSE;
      }
    }
    else /* RNG_DIFF & VE_DIFF bits not set*/
    {
      if(w_Cno == 0)
      {
        return FALSE;
      }
    }
  }

  return TRUE;
}

/*
 ******************************************************************************
 * lm_mgp_clean_meas_rpt
 *
 * Function description:
 *
 * This function cleans the PRM Meas report and removes any invalid SV's
 *
 * Parameters: 
 * p_measReport - PRM report to be sent to TM
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_clean_meas_rpt(sm_GpsMeasRptStructType* p_measReport)
{
  uint8 temp_u_NumSvs = 0; 
  int itr = 0; 

  if(p_measReport == NULL || ((p_measReport->z_MeasBlk).u_NumSvs == 0))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Measurement Report is empty.",0,0,0);
    return;
  }
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Original u_NumSvs = %d",(p_measReport->z_MeasBlk).u_NumSvs,0,0);
  if( sm_is_meas_blk_active((p_measReport->z_MeasBlk).e_MeasBlkSrc, GNSS_MEAS_BLK_SRC_GPS) )
  {
    gnss_MeasStructType temp_z_Gps[N_ACTIVE_GPS_CHAN];
    memset((void*)&temp_z_Gps,0,sizeof(temp_z_Gps));

    for(itr = 0; (itr < (p_measReport->z_MeasBlk).u_NumSvs) && (itr < N_ACTIVE_GPS_CHAN); itr++ )
    {
      if(lm_meas_status_bits_check((p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].q_SvState))
      {
        uint16 w_Sv;

        temp_z_Gps[temp_u_NumSvs] = (p_measReport->z_MeasBlk).z_Gnss[itr];

        // convert SBAS SVID from internal (120~151) to external format(33~64)
        w_Sv = temp_z_Gps[temp_u_NumSvs].w_Sv;
        if  ( (w_Sv >= C_FIRST_SBAS_SV_PRN) && ( w_Sv <= C_LAST_SBAS_SV_PRN) ) // 120~138 for SBAS
        {
          temp_z_Gps[temp_u_NumSvs].w_Sv = w_Sv - C_FIRST_SBAS_SV_PRN + PDSM_PD_SV_ID_SBAS_MIN;
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Converting SBAS SVID from %d to %d", w_Sv, temp_z_Gps[temp_u_NumSvs].w_Sv, 0);
        }

        temp_u_NumSvs++;
      }
      else
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Dropping GPS SV %d: q_MeasStatus=%x Cno=%d",
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Sv,
                (p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno);
      }
    }
    /* Copying the final list of SV's from the temp array back into the report*/
    memscpy((void *)&((p_measReport->z_MeasBlk).z_Gnss[0]), 
           sizeof(((p_measReport->z_MeasBlk).z_Gnss)),
           (void *)temp_z_Gps, 
           sizeof(gnss_MeasStructType)*temp_u_NumSvs);

    (p_measReport->z_MeasBlk).u_NumSvs = temp_u_NumSvs;
  }
  else if(sm_is_meas_blk_active((p_measReport->z_MeasBlk).e_MeasBlkSrc, GNSS_MEAS_BLK_SRC_GLO))
  {
    gnss_MeasStructType temp_z_Glo[N_ACTIVE_GLO_CHAN];
    memset((void*)&temp_z_Glo,0,sizeof(temp_z_Glo));

    for(itr = 0; (itr < (p_measReport->z_MeasBlk).u_NumSvs) && (itr < N_ACTIVE_GLO_CHAN); itr++ )
    {
      if(lm_meas_status_bits_check((p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].q_SvState))
      {
        temp_z_Glo[temp_u_NumSvs++] = (p_measReport->z_MeasBlk).z_Gnss[itr];
      }
      else
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"Dropping GLO SV %d: q_MeasStatus=%x Cno=%d",
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Sv,
                (p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno);
      }
    }
    /* Copying the final list of SV's from the temp array back into the report*/
    memscpy((void *)&((p_measReport->z_MeasBlk).z_Gnss[0]),
           sizeof(((p_measReport->z_MeasBlk).z_Gnss)),
           (void *)temp_z_Glo, 
           sizeof(gnss_MeasStructType)*temp_u_NumSvs);
    (p_measReport->z_MeasBlk).u_NumSvs = temp_u_NumSvs;
  }
  else if(sm_is_meas_blk_active((p_measReport->z_MeasBlk).e_MeasBlkSrc, GNSS_MEAS_BLK_SRC_BDS))
  {
    gnss_MeasStructType temp_z_Bds[N_ACTIVE_BDS_CHAN];
    memset((void*)&temp_z_Bds,0,sizeof(temp_z_Bds));

    for(itr = 0; (itr < (p_measReport->z_MeasBlk).u_NumSvs) && (itr < N_ACTIVE_BDS_CHAN); itr++ )
    {
      if(lm_meas_status_bits_check((p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].q_SvState))
      {
        temp_z_Bds[temp_u_NumSvs++] = (p_measReport->z_MeasBlk).z_Gnss[itr];
      }
      else
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"Dropping BDS SV %d: q_MeasStatus=%x Cno=%d",
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Sv,
                (p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno);
      }
    }
    /* Copying the final list of SV's from the temp array back into the report*/
    memscpy((void *)&((p_measReport->z_MeasBlk).z_Gnss[0]), 
           sizeof(((p_measReport->z_MeasBlk).z_Gnss)),
           (void *)temp_z_Bds, 
           sizeof(gnss_MeasStructType)*temp_u_NumSvs);
    (p_measReport->z_MeasBlk).u_NumSvs = temp_u_NumSvs;
  }
  else if(sm_is_meas_blk_active((p_measReport->z_MeasBlk).e_MeasBlkSrc, GNSS_MEAS_BLK_SRC_GAL))
  {
    gnss_MeasStructType temp_z_Gal[N_ACTIVE_GAL_CHAN];
    memset((void*)&temp_z_Gal,0,sizeof(temp_z_Gal));

    for(itr = 0; (itr < (p_measReport->z_MeasBlk).u_NumSvs) && (itr < N_ACTIVE_GAL_CHAN); itr++ )
    {
      if(lm_meas_status_bits_check((p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].q_SvState))
      {
        temp_z_Gal[temp_u_NumSvs++] = (p_measReport->z_MeasBlk).z_Gnss[itr];
      }
      else
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"Dropping GAL SV %d: q_MeasStatus=%x Cno=%d",
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Sv,
                (p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno);
      }
    }
    /* Copying the final list of SV's from the temp array back into the report*/
    memscpy((void *)&((p_measReport->z_MeasBlk).z_Gnss[0]), 
           sizeof(((p_measReport->z_MeasBlk).z_Gnss)),
           (void *)temp_z_Gal, 
           sizeof(gnss_MeasStructType)*temp_u_NumSvs);
    (p_measReport->z_MeasBlk).u_NumSvs = temp_u_NumSvs;
  }
  else if(sm_is_meas_blk_active((p_measReport->z_MeasBlk).e_MeasBlkSrc, GNSS_MEAS_BLK_SRC_QZSS_SBAS))
  {
    gnss_MeasStructType temp_z_Qzss[N_ACTIVE_CHAN];
    memset((void*)&temp_z_Qzss,0,sizeof(temp_z_Qzss));

    for(itr = 0; (itr < (p_measReport->z_MeasBlk).u_NumSvs) && (itr < N_ACTIVE_CHAN); itr++ )
    {
      if(lm_meas_status_bits_check((p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].q_SvState))
      {
        uint16 w_Sv;

        temp_z_Qzss[temp_u_NumSvs] = (p_measReport->z_MeasBlk).z_Gnss[itr];

        // convert SBAS SVID from internal (120~151) to external format(33~64)
        w_Sv = temp_z_Qzss[temp_u_NumSvs].w_Sv;
        if  ( (w_Sv >= C_FIRST_SBAS_SV_PRN) && ( w_Sv <= C_LAST_SBAS_SV_PRN) ) // 120~138 for SBAS
        {
          temp_z_Qzss[temp_u_NumSvs].w_Sv = w_Sv - C_FIRST_SBAS_SV_PRN + PDSM_PD_SV_ID_SBAS_MIN;
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Converting SBAS SVID from %d to %d", w_Sv, temp_z_Qzss[temp_u_NumSvs].w_Sv, 0);
        }

        temp_u_NumSvs++;
      }
      else
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"Dropping QZSS SV %d: q_MeasStatus=%x Cno=%d",
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Sv,
                (p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno);
      }
    }
    /* Copying the final list of SV's from the temp array back into the report*/
    memscpy((void *)&((p_measReport->z_MeasBlk).z_Gnss[0]), 
           sizeof(((p_measReport->z_MeasBlk).z_Gnss)),
           (void *)temp_z_Qzss, 
           sizeof(gnss_MeasStructType)*temp_u_NumSvs);

    (p_measReport->z_MeasBlk).u_NumSvs = temp_u_NumSvs;
  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"New u_NumSvs = %d",(p_measReport->z_MeasBlk).u_NumSvs,0,0);
}

/*
 ******************************************************************************
 * lm_mgp_handle_gps_meas
 *
 * Function description:
 *
 * This function handles the periodic PRM measurement report  from MGP
 *
 * Parameters: 
 * p_msg - Pointer to structure containing IPC message with meas report
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_gps_meas(const os_IpcMsgType* p_msg)
{
  sm_GpsMeasRptStructType* p_measReport;

  if(p_msg == NULL)
  {
    return;
  }

  /*lint -e826 */
  p_measReport = (sm_GpsMeasRptStructType*) p_msg->p_Data;
  /*lint +e826 */
  LM_MSG_HIGH("Received PERIODIC PRM report from MGP for %d (GPS/GLO/BDS/GAL/QZSS) system", p_measReport->z_MeasBlk.e_MeasBlkSrc, 0, 0);

  /* Cleaning the MEAS report */
  lm_mgp_clean_meas_rpt(p_measReport);

  /* NOTE: TM protocol modules must add code to "stitch" together per GNSS system meas reports
  from MGP */
  if(z_lmControl.e_sessionState == LM_SESSION_STATE_IN_SESSION_REPORT_PENDING)
  {
    if(!lm_tm_send_prm_report_to_tm(p_measReport,FALSE))
    {
      LM_MSG_ERROR("Unable to send periodic PRM report to TM",0,0,0);
    }
  }
}
/*
 ******************************************************************************
 * lm_mgp_xlate_gnss_type
 *
 * Function description:
 * This function translates the MGP GNSS TYPE to SM GNSS type
 * 
 *
 * Parameters: 
 * q_gnssType - MGP GNSS type 
 *
 * Return value: 
 *
 * SM GNSS TYPE
 *
 ******************************************************************************
*/

uint8 lm_mgp_xlate_gnss_type(mgp_GnssTypeVal       q_gnssType)
{
   switch (q_gnssType)
   {
     case MGP_GNSS_TYPE_GPS:
	 case MGP_GNSS_TYPE_ALL:
	   return SM_GNSS_GPS_CONSTELL;
	 case MGP_GNSS_TYPE_GLO:
		return SM_GNSS_GLO_CONSTELL;
	 case MGP_GNSS_TYPE_BDS:
		return SM_GNSS_BDS_CONSTELL;
	 case MGP_GNSS_TYPE_GAL:
		return SM_GNSS_GAL_CONSTELL;
	 case MGP_GNSS_TYPE_QZSS:
		return SM_GNSS_QZSS_CONSTELL;
     default:
	    return MGP_GNSS_TYPE_ALL;
   }  	
}
/*
 ******************************************************************************
 * lm_mgp_handle_delete_mgp_info
 *
 * Function description:
 * This function handles the MGP info delete request from TM
 * 
 *
 * Parameters: 
 * *p_Req - delete MGP info request from TM
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/

void 
lm_mgp_handle_delete_mgp_info(lm_request_delete_mgp_info_s_type *p_Req)
{
  uint8 q_Constel = 0;
  lm_request_delete_mgp_info_s_type *p_cached_del_aiding_data = z_lmControl.z_cached_del_aiding_data.p_cached_del_aiding_data[q_Constel];
   
  if (p_Req == NULL)
  {
    return;
  }

  LM_MSG_HIGH("Calling MGP delete function", 0, 0, 0);
  
  if ((LM_DMM_STATE_LOADED != lm_get_dmm_state()))
  {
    if(p_Req->z_mgp_info.q_DeletePosMask == C_DELETE_INFO_POS_INFO)
    {
       LM_MSG_HIGH("Delete CPI is requested", 0, 0, 0);
       lm_mgp_delete_cached_coarse_pos();	  
    }
	/*Checking if already delAll request is cached, if so then no need to cache subsequent req*/
	if((NULL != p_cached_del_aiding_data) && 
		(MGP_GNSS_TYPE_ALL == p_cached_del_aiding_data->z_mgp_info.e_GnssType))
	{
	   MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Already delAll is cached, so no need to cache subsequent requests");
	   return;
	}
	q_Constel = lm_mgp_xlate_gnss_type(p_Req->z_mgp_info.e_GnssType);

	if(MGP_GNSS_TYPE_ALL == q_Constel)
	{
	   MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid GNSS type is recieved");
	   return;
	}
	
	p_cached_del_aiding_data = z_lmControl.z_cached_del_aiding_data.p_cached_del_aiding_data[q_Constel];
	
    if(NULL == p_cached_del_aiding_data)
    {
       if(NULL == (p_cached_del_aiding_data = z_lmControl.z_cached_del_aiding_data.p_cached_del_aiding_data[q_Constel] = 
	   	  (lm_request_delete_mgp_info_s_type *)os_MemAlloc(sizeof(lm_request_delete_mgp_info_s_type), OS_MEM_SCOPE_TASK)))
       {
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory in lm_mgp_handle_del_aiding_data_request");
          return;
       }
    }
   	
	*p_cached_del_aiding_data = *p_Req;
	/* To avoid force off again as already MGP is off*/
	 p_cached_del_aiding_data->force_rcvr_off = FALSE;
		
  }
/*REV 12/05: Handle case for MGP unloaded, behave as done in tm_core_delete_inject_aiding_data() */
  else
  {
    if (p_Req->force_rcvr_off)
    {
      os_TimerStop(z_lmControl.p_mgpShutOffTimer); /*lint !e534 */
      os_TimerStop(z_lmControl.p_rcOnTimer); /*lint !e534 */
      if (!lm_mgp_update_mgp_state(LM_MGP_STATE_OFF, NULL, LM_UPDATE_MGP_STATE_FLAG_FORCED))
      {
        LM_MSG_HIGH("State update to %d failed", LM_MGP_STATE_OFF, 0, 0);
      }
    }
    p_Req->z_mgp_info.u_DeleteAll = p_Req->delete_all;
  
    /* SM always calls Gen8 delete function. any translation to 
    Gen7 ME API will be handled inside mgp_GnssDeleteInfo().
    */
    mgp_GnssDeleteInfo(&p_Req->z_mgp_info);
  }

}
/*
 ******************************************************************************
 * lm_mgp_get_data_src_type
 *
 * Function description:
 * This function returns gps_DataSourceType equivalent of  cgps_SrvSystemType
 *
 * Parameters: 
 * cgps_SrvSystemType 
 *
 * Return value: 
 * gps_DataSourceType* - Equivalent enumeration in gps_DataSourceType
 * boolean - Equivalent enumeration found or not
 * None
 *
 ******************************************************************************
*/
boolean lm_mgp_get_data_src_type(cgps_SrvSystemType e_srv_sys_type, gps_DataSourceType *p_data_src_type)
{

  if (p_data_src_type == NULL)
  {
    return FALSE;
  }

  switch (e_srv_sys_type)
  {
    case CGPS_SRV_SYS_NONE:          /* NO Service */
    case CGPS_SRV_SYS_GW:            /* GSM and WCDMA mode */
    case CGPS_SRV_SYS_OTHER:         /* All other modes */
      return FALSE;
    case CGPS_SRV_SYS_GSM:           /* GSM mode */
      *p_data_src_type = SOURCE_GSM;
      return TRUE;
    case CGPS_SRV_SYS_LTE:         /* LTE mode */
      *p_data_src_type = SOURCE_LTE;
      return TRUE;
    default:
      return FALSE;
  }

}

/*
 ******************************************************************************
 * lm_mgp_handle_gps_time_request
 *
 * Function description:
 * This function handles the GPS time request from TM.
 * 
 *
 * Parameters: 
 * p_gps_time_request - pointer to the info request
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_gps_time_request(lm_request_gps_info_s_type *p_gps_time_request)
{
  LM_MSG_MED("Received GPS time request from TM. source:%d",
             (uint32)p_gps_time_request->e_time_chk_type, 0, 0);
  mgp_GnssReqGpsTime();
}

/*
 ******************************************************************************
 * lm_mgp_handle_rcOn_timer_timeout
 *
 * Function description:
 * This function handles the RC_ON timer timeout event. If there is no fix or 
 * PRM session going on, and MGP is still in ON state, MGP will be put in IDLE
 * state. 
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
void lm_mgp_handle_rcOn_timer_timeout(void)
{
  LM_MSG_MED("RC_ON Timer timedout", 0, 0, 0);

  if ((z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION) ||
      (!(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_FIX) &&
       !(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_GPS)))
  {
    if ((z_lmControl.e_currentMgpState == LM_MGP_STATE_ON) ||
        (z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_ON))
    {
      if (!lm_mgp_update_mgp_state(LM_MGP_STATE_OFF, NULL, LM_UPDATE_MGP_STATE_FLAG_NONE))
      {
        LM_MSG_HIGH("State update to %d failed", LM_MGP_STATE_OFF, 0, 0);
      }

    }
  }
  /*Report Event*/
  event_report(EVENT_GPS_LM_RC_ON_TIMER_TIMEOUT);
}


/*
 ******************************************************************************
 * lm_mgp_handle_shutOff_timer_timeout
 *
 * Function description:
 * This function handles the shut-Off timer timeout event. If there is no fix or 
 * PRM session going on, and MGP is not in OFF state, MGP will be put in OFF
 * state. 
 * It will request GERA to disable the enabled client before shutting off the timer
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
void lm_mgp_handle_shutOff_timer_timeout(void)
{
  /*Handle shut off timer expiry only if the timer is not re-armed*/
  if (os_TimerCheck(z_lmControl.p_mgpShutOffTimer) == 0)
  {
    LM_MSG_MED("Shut-off timer timedout", 0, 0, 0);

    if ((z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION) ||
        (!(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_FIX) &&
         !(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_GPS)))
    {

      if (!lm_mgp_update_mgp_state(LM_MGP_STATE_OFF, NULL, LM_UPDATE_MGP_STATE_FLAG_NONE))
      {
        LM_MSG_HIGH("State update to %d failed", LM_MGP_STATE_OFF, 0, 0);
      }
      /*Report Event*/
      event_report(EVENT_GPS_LM_SHUT_OFF_TIMER_TIMEOUT);
    }
  event_report(EVENT_GPS_LM_SHUT_OFF_TIMER_TIMEOUT); 

  }
  else
  {
    LM_MSG_MED("Shut-off timer timeout neglected, timer already re-armed", 0, 0, 0);
  }
}

/*
 ******************************************************************************
 * lm_mgp_handle_pos_unc
 *
 * Function description:
 * This function injects new POS UNC into MGP
 *
 * Parameters: 
 * f_new_pos_unc
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void  lm_mgp_handle_pos_unc(FLT f_new_pos_unc)
{
  if (LM_INVALID_VALUE == f_new_pos_unc)
  {
     return;
  }
  if ((LM_DMM_STATE_LOADED != lm_get_dmm_state()))
  {
    z_lmControl.z_cached_del_aiding_data.f_cached_new_pos_unc = f_new_pos_unc;
	return;
  }
  mgp_ModifyPosUnc(f_new_pos_unc);
  z_lmControl.z_cached_del_aiding_data.f_cached_new_pos_unc = LM_INVALID_VALUE;
}

/*
 ******************************************************************************
 * lm_mgp_handle_time_unc
 *
 * Function description:
 * This function injects new time UNC into MGP
 *
 * Parameters: 
 * f_new_time_unc
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void  lm_mgp_handle_time_unc(FLT f_new_time_unc)
{
  if (LM_INVALID_VALUE == f_new_time_unc)
  {
    return;
  }
  if ((LM_DMM_STATE_LOADED != lm_get_dmm_state()))
  {
    z_lmControl.z_cached_del_aiding_data.f_cached_new_time_unc = f_new_time_unc;
	return;
  }
  mgp_GnssModifyTimeUnc(MGP_GNSS_TYPE_ALL, f_new_time_unc);
  z_lmControl.z_cached_del_aiding_data.f_cached_new_time_unc = LM_INVALID_VALUE;
} /* lm_mgp_handle_time_unc */

/*
 ******************************************************************************
 * lm_mgp_handle_time_offset
 *
 * Function description:
 * This function injects new time offset into MGP
 *
 * Parameters: 
 * f_time_offset
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void  lm_mgp_handle_time_offset(FLT f_time_offset)
{
  if (LM_INVALID_VALUE == f_time_offset)
  {
    return;
  }
  if ((LM_DMM_STATE_LOADED != lm_get_dmm_state()))
  {
    z_lmControl.z_cached_del_aiding_data.f_cached_time_offset = f_time_offset;
	return;
  }
  mgp_GnssAddTimeOffset(MGP_GNSS_TYPE_ALL, f_time_offset);
  z_lmControl.z_cached_del_aiding_data.f_cached_time_offset = LM_INVALID_VALUE;
}

/*
 ******************************************************************************
 * lm_mgp_handle_slow_clk_time_inject_request
 *
 * Function description:
 * This function injects external time from sources into GTS
 *
 * Parameters: 
 * p_slow_clk_inject
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void  lm_mgp_handle_slow_clk_time_inject_request(lm_request_slow_clk_time_inject_s_type *p_slow_clk_inject)
{
  /* injecting time source to GTS module through API */
  gts_ExternalSourceInjection(p_slow_clk_inject);
  }

/*
 ******************************************************************************
 * lm_mgp_handle_external_coarse_pos_inject_request
 *
 * Function description:
 * This function injects external coarse position from external sources 
 * into MGP
 *
 * Parameters: 
 * lm_mgp_handle_external_coarse_pos_inject_request
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/

void  lm_mgp_handle_external_coarse_pos_inject_request
(lm_request_extern_coarse_pos_inject_s_type *p_extern_coarse_pos_inj)
{
  if(LM_DMM_STATE_LOADED == lm_get_dmm_state())
  {
    mgp_ExternalCoarsePosPut(&p_extern_coarse_pos_inj->z_extern_coarse_pos);
  }
  else 
  {
    if (NULL != z_lmControl.p_cached_coarse_pos)
    {
      LM_MSG_ERROR("CPI cache wasn't deleted earlier. Will overwrite existing cache anyways", 0, 0, 0);
    }
    else
    {
      if(NULL == (z_lmControl.p_cached_coarse_pos = (gnss_ExternCoarsePosInjectionType *)os_MemAlloc(sizeof(gnss_ExternCoarsePosInjectionType), OS_MEM_SCOPE_TASK)))
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory in lm_mgp_handle_external_coarse_pos_inject_request");
        return;
      }
    }
    memscpy((void*)z_lmControl.p_cached_coarse_pos, sizeof(*(z_lmControl.p_cached_coarse_pos)),
             (void*)&(p_extern_coarse_pos_inj->z_extern_coarse_pos), sizeof(gnss_ExternCoarsePosInjectionType));
	{
      char b_Buffer[250];
     (void)snprintf(b_Buffer, sizeof(b_Buffer), "lat1=%f long1=%f alt=%f\r\n",
                 z_lmControl.p_cached_coarse_pos->latitude,z_lmControl.p_cached_coarse_pos->longitude,z_lmControl.p_cached_coarse_pos->altitude);
      MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", b_Buffer);
    }
  }
}
/*
 ******************************************************************************
 * lm_mgp_delete_cached_coarse_pos
 *
 * Function description:
 *
 * This function clears the cached CPI when delete all flag is received from QMI
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
void lm_mgp_delete_cached_coarse_pos(void)
{
  if (z_lmControl.p_cached_coarse_pos != NULL)
  {
    os_MemFree((void * *) &z_lmControl.p_cached_coarse_pos);
    LM_MSG_HIGH("lm_mgp_delete_cached_coarse_pos: CPI cache deleted", 0, 0, 0);
  }
  return;
}

/*
 ******************************************************************************
 * lm_mgp_handle_wbiq_info
 *
 * Function description:
 *
 * This function handles the WBIQ info (Cno, freq) info from MGP
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
void lm_mgp_handle_wbiq_info(os_IpcMsgType *p_msg)
{
  if (p_msg == NULL)
  {
    return;
  }

  LM_MSG_HIGH("WBIQ PARAMS received from MGP.", 0, 0, 0);

  /* Notify TM of this data */
  tm_api_report_wbiq_info((sm_ReportWBIQInfoStructType *)p_msg->p_Data);

  /* This marks the termination of the msg ARIES_MSG_ID_MGP_LM_REPORT_WB_IQ_PARAMS.
    New code can be added (by the customer) to print out/use these values. */
}

/*
 ******************************************************************************
 * lm_mgp_handle_xooffset_info
 *
 * Function description:
 *
 * This function handles the XO offset info from MGP
 *
 * Parameters: 
 * Pointer to XO offset structure
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_xooffset_info(sm_ReportXOOffsetInfoStructType *pz_XOOffsetData)
{
  if (pz_XOOffsetData == NULL)
  {
    LM_MSG_ERROR_0("lm_mgp_handle_xooffset_info: NULL pointer check failed");
  }
  else
  {
    tm_api_report_xooffset_info(pz_XOOffsetData);
  }
}

/*
 ******************************************************************************
 * lm_mgp_handle_nav_config_change_resp
 *
 * Function description:
 *
 * This function handles the NAV config change response from MGP
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

void lm_mgp_handle_nav_config_change_resp(os_IpcMsgType *p_Msg)
{
  if (p_Msg == NULL)
  {
    LM_MSG_ERROR_0("Received NULL nav config change response from MGP");
  }
  else
  {
    tm_api_nav_config_change_resp(*((uint8 *)p_Msg->p_Data));
  }
}


/*
 ******************************************************************************
 * lm_mgp_handle_nbiq_info
 *
 * Function description:
 *
 * This function handles the NBIQ info from MGP
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
void lm_mgp_handle_nbiq_info(os_IpcMsgType *p_msg)
{
  sm_ReportNBIQInfoStructType *p_nbiq_info;

  if (p_msg == NULL)
  {
    return;
  }

  LM_MSG_HIGH("NBIQ PARAMS received from MGP.", 0, 0, 0);


  p_nbiq_info = (sm_ReportNBIQInfoStructType *)p_msg->p_Data;

  LM_MSG_HIGH("NBIQ data ptr 0x%x, Size %d",
              p_nbiq_info->p_NBIQDataPtr,
              p_nbiq_info->q_NBIQDataSize, 0);


  /* This marks the termination of the msg ARIES_MSG_ID_MGP_LM_REPORT_WB_IQ_PARAMS.
    New code can be added (by the customer) to print out/use these values. */

}

/*
 ******************************************************************************
 * lm_mgp_handle_gnss_rf_status_info
 *
 * Function description:
 *
 * This function handles the GNSS RF status info from MGP
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
void lm_mgp_handle_gnss_rf_status_info(os_IpcMsgType *p_msg)
{
  sm_ReportGNSSRfStatusInfoStructType *p_GnssRfInfo;

  if (p_msg == NULL)
  {
    return;
  }

  LM_MSG_HIGH("GNSS RF status info received from MGP.", 0, 0, 0);


  p_GnssRfInfo = (sm_ReportGNSSRfStatusInfoStructType *)p_msg->p_Data;

  LM_MSG_HIGH("GNSS RF status info PGA Gain %d BP1 AmpI %d AmpQ %d",
              p_GnssRfInfo->l_PGAGain,
              p_GnssRfInfo->q_Bp1LbwAmplI,
              p_GnssRfInfo->q_Bp1LbwAmplQ);

  /* This marks the termination of the msg ARIES_MSG_ID_MGP_LM_REPORT_WB_IQ_PARAMS.
    New code can be added (by the customer) to print out/use these values. */

}

/*
 ******************************************************************************
 * lm_mgp_handle_request_sa_data
 *
 * Function description:
 *
 * This function handles the SA data request from MGP
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
void lm_mgp_handle_request_sa_data(void)
{
  if (z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)
  {
    LM_MSG_ERROR("MGP requested SA data while no session is active", 0, 0, 0);
    return;
  }

  (void)lm_tm_send_sa_data_request_to_tm();
}

/*
 ******************************************************************************
 * lm_log_early_exit_decision
 *
 * Function description:
 *
 * This function generates a DM log message
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void lm_log_early_exit_decision(
   const uint32 q_TimeFromEngOn,
   const uint32 q_TimeFromSessStart,
   const uint32 q_FCount,
   const uint16 w_HepeLimitCurrent,
   const uint16 w_HepeLimitDynamic,
   const uint16 w_HepeMax,
   const uint16 w_Hepe,
   const boolean u_FixValid
   )
{
  LOG_CGPS_SESSION_EARLY_EXIT_DECISION_C_type *p_LogPkt;

  lm_early_exit_decision_info_s_type *p_EarlyExitDecisionType = &z_lmControl.z_early_exit_decision;
  if (p_EarlyExitDecisionType == NULL)
  {
    return;
  }
  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_SESSION_EARLY_EXIT_DECISION_C_type *)log_alloc(LOG_CGPS_SESSION_EARLY_EXIT_DECISION_C,
                                                                      sizeof(LOG_CGPS_SESSION_EARLY_EXIT_DECISION_C_type));

  if (p_LogPkt)
  {
    p_LogPkt->u_PacketVersion = 1;
    p_LogPkt->e_decision = p_EarlyExitDecisionType->e_decision;
    p_LogPkt->w_QoSTimeoutOverride = (uint16)p_EarlyExitDecisionType->q_QoSTimeoutOverride;
    p_LogPkt->q_TimeEngOn = q_TimeFromEngOn;
    p_LogPkt->q_TimeOfFirstValidFixAfterEngineOn = p_EarlyExitDecisionType->q_TimeOfFirstValidFixAfterEngineOn;
    p_LogPkt->q_TimeSessionStart = q_TimeFromSessStart;
    p_LogPkt->q_TimeOfFirstValidFixAfterSessStart = p_EarlyExitDecisionType->q_TimeOfFirstValidFixAfterSessStart;
    p_LogPkt->q_FCount = q_FCount;
    p_LogPkt->w_QoSHepeOverride = p_EarlyExitDecisionType->w_QoSThresholdOverride;
    p_LogPkt->w_HepeLimitCurrent = w_HepeLimitCurrent;
    p_LogPkt->w_HepeLimitDynamic = w_HepeLimitDynamic;
    p_LogPkt->w_HepeMax = w_HepeMax;
    p_LogPkt->w_HepeMaxDefault = z_lmControl.q_HepeThreshold;
    p_LogPkt->u_FixValid = u_FixValid;
    p_LogPkt->w_Hepe = w_Hepe;
    log_commit(p_LogPkt);
  }
  return;
}

/*
 ******************************************************************************
 * lm_mgp_handle_gps_time
 *
 * Function description:
 *
 * This function handles the GPS time info from MGP
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
void lm_mgp_handle_gps_time(os_IpcMsgType *p_msg)
{
  sm_ReportGpsTimeStructType *p_gps_time_report;

  if (p_msg == NULL)
  {
    return;
  }

  LM_MSG_HIGH("GPS time received from MGP. Forwarding to TM..", 0, 0, 0);

  /*lint -e826 */
  p_gps_time_report = (sm_ReportGpsTimeStructType *)p_msg->p_Data;
  /*lint +e826 */

  (void)lm_tm_send_gps_time_to_tm(p_gps_time_report);
}


/**
* @brief  LM abstraction layer around mgp_UpdateInfo. This
*         abstratcion implements a "shim" to enable backwards
*         compatibility with GEN7 MGP.
  
  @return      
  @sideeffects None
* @see mgp_GnssUpdateInfo() and mgp_UpdateInfo()

*/
boolean lm_mgp_update_info(gnss_AssistDataInfoStructType *p_assistDataInfo)
/**< GEN8 assistance data received from network */
{
  boolean b_retval = (p_assistDataInfo != NULL);
  if (b_retval)
  {
    LM_MSG_MED("Post %d (GPS/GLO/BDS) %d AD to MGP", p_assistDataInfo->e_GnssType, p_assistDataInfo->e_AssistanceType, 0);

    /* SM always calls gen 8 update info function. Any translation
    needed to Gen 7 ME will be handled inside mgp_GnssUpdateInfo() 
    */
    mgp_GnssUpdateInfo(p_assistDataInfo);
  }
  return b_retval;
}

/*
 ******************************************************************************
 * lm_mgp_handle_force_rcvr_off
 *
 * Function description:
 *
 * This function sends receiver off (forced) to MGP
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
void lm_mpg_handle_force_rcvr_off(void)
{
  lm_mgp_stop_position_fix_processing(TRUE);

}

/*
 ******************************************************************************
 * lm_mgp_handle_rcvr_off
 *
 * Function description:
 *
 * This function sends receiver off  to MGP
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
void lm_mpg_handle_rcvr_off(void)
{
  lm_mgp_stop_position_fix_processing(FALSE);
}

/*
 ******************************************************************************
 * lm_mgp_send_fix_start_ind
 *
 * Function description:
 *
 * This function sends FIX_START_IND to MGP
 *
 * Parameters: 
 * q_Tbf - TBF associated with the fix request.
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_send_fix_start_ind(uint32 q_Tbf)
{
  mgp_SmEventData z_SmEventData;

  z_SmEventData.e_SmEvent = C_MGP_FIX_START_IND;
  z_SmEventData.z_SmEventpayload.z_FixStartData.q_Tbf = q_Tbf;
  z_SmEventData.z_SmEventpayload.z_FixStartData.q_RcvrOnThresholdTime = lm_get_rc_on_threshold_period();
  z_SmEventData.z_SmEventpayload.z_FixStartData.q_RcvrIdleThresholdTime = lm_get_rc_idle_threshold();

  (void)mgp_ReportSmEvent(&z_SmEventData);
}

/*
 ******************************************************************************
 * lm_mgp_send_fix_end_ind
 *
 * Function description:
 *
 * This function sends FIX_END_IND to MGP
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
void lm_mgp_send_fix_end_ind(void)
{
  mgp_SmEventData z_SmEventData;

  z_SmEventData.e_SmEvent = C_MGP_FIX_END_IND;

  if (z_lmControl.z_session_request.q_tbf >= lm_get_rc_on_threshold_period())
  {
    /*Unsubscribe 1Hz periodic fix reporting from PE*/
    if (!mgp_ConfigPeriodicFixAndAlignment(C_MGP_FIX_TIMER_OFF_ALIGN_NOT_REQ, 0))
    {
      LM_MSG_ERROR("Failed to send the disable periodic fixes request to the PE.", 0, 0, 0);
    }
  }

  /*Expect MGP going to IDLE*/
  z_lmControl.e_desiredMgpState = LM_MGP_STATE_IDLE;

  (void)mgp_ReportSmEvent(&z_SmEventData);
}

/*
 ******************************************************************************
 * lm_xtraInjDone
 *
 * Function description:
 *
 * This function handles IND from CD on completion of an Xtra injection
 *
 * Parameters: 
 * gnss_XtraInjMaskType  q_AcceptedMask
 *
 * Return value: 
 *  None
 *
 ******************************************************************************
*/
void lm_xtraInjDone(gnss_XtraInjMaskType  q_AcceptedMask)
{
  tm_api_xtra_inj_done(q_AcceptedMask);

}
/*
 ******************************************************************************
 * lm_mgp_report_SARF_status
 *
 * Function description:
 * Notifies TM of SARF ENTER/EXIT indications 
 *
 * Parameters: 
 * v_sarf_status: TRUE: SARF ENTER; FALSE: SARF EXIT
 *
 * Return value: 
 * None
 *
 ******************************************************************************
*/
void lm_mgp_report_SARF_status(const boolean v_sarf_status)
{
  tm_info_s_type z_tmInfo;
  memset((void *)&z_tmInfo, 0, sizeof(z_tmInfo));

  /* Cache SARF state in LM */
  v_lm_sarf_state = v_sarf_status; 
  
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, 
        "Reporting SARF Status to TM = %d", v_sarf_status);

  /* Send IPC msg to TM */
  z_tmInfo.e_info_type = TM_INFO_REPORT_SARF_STATUS;
  z_tmInfo.z_info.v_sarf_status = v_sarf_status;
  (void)tm_info(&z_tmInfo);
}

/*
 ******************************************************************************
 * lm_mgp_get_SARF_status
 *
 * Function description:
 * Returns SARF state 
 *
 * Parameters: 
 * void
 *
 * Return value: 
 * boolean: TRUE: SARF ENTER; FALSE: SARF EXIT
 *
 ******************************************************************************
*/
boolean lm_mgp_get_SARF_status(void)
{
  return (v_lm_sarf_state);
}
/*
 ******************************************************************************
 * lm_mgp_process_fix_request
 *
 * Function description:
 * Returns  
 *                if MGP is loaded:   Whether fix processing could be 
 *                                    started
 *                if MGP is unloaded: Request to load MGP successfuly 
 *                                    made
 *
 * Parameters: 
 * void
 *
 * Return value: 
 * boolean: TRUE: if MGP is loaded:   Fix processing successfuly started
 *                if MGP is unloaded: Request to load MGP successfuly 
 *                                    made
 *         FALSE: if MGP is loaded:   Fix processing could not be started
 *                if MGP is unloaded: Request to load MGP could not be 
 *                                    made
 ******************************************************************************
*/

void lm_mgp_proceed_with_process_fix_request(void)
{

    lm_session_request_fix_s_type* p_fixReq = &z_lmControl.z_cached_fix_request.z_fix_request;	
    uint32 q_handle = z_lmControl.z_cached_fix_request.q_handle;
    if(!lm_mgp_start_position_fix_processing(p_fixReq))
    {
      LM_MSG_MED("Failed to start fix processing.Sending NACK for handle[%d]",q_handle,0,0);
      if(!lm_send_tm_session_update_nack(q_handle,TM_STATUS_NACK_CAUSE_UNKNOWN_ERROR))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
    }
}


/*
 ******************************************************************************
 * lm_mgp_process_fix_request
 *
 * Function description:
 * Returns  
 *                if MGP is loaded:   Whether fix processing could be 
 *                                    started
 *                if MGP is unloaded: Request to load MGP successfuly 
 *                                    made
 *
 * Parameters: 
 * void
 *
 * Return value: 
 * boolean: TRUE: if MGP is loaded:   Fix processing successfuly started
 *                if MGP is unloaded: Request to load MGP successfuly 
 *                                    made
 *         FALSE: if MGP is loaded:   Fix processing could not be started
 *                if MGP is unloaded: Request to load MGP could not be 
 *                                    made
 ******************************************************************************
*/

boolean lm_mgp_process_fix_request(const lm_session_request_info_s_type* const p_startReq)
{
  if(NULL == p_startReq)
  {
    return FALSE;
  }
  z_lmControl.z_cached_fix_request.q_handle = p_startReq->q_handle;
  z_lmControl.z_cached_fix_request.z_fix_request = p_startReq->z_request.z_fix_request;
  z_lmControl.z_session_request.e_gnss_activation_reason = LM_MGP_LOAD_FOR_GNSS_SESSION;

  if (LM_DMM_STATE_LOADED != lm_get_dmm_state())
  {  
    z_lmControl.v_XtraInjectionPending = TRUE;
    z_lmControl.z_session_request.e_gnss_activation_reason = LM_MGP_LOAD_FOR_GNSS_SESSION;
    LM_MSG_ERROR("MGP is not loaded yet. Reqest MARB to load. LM state = %d", lm_get_dmm_state(), 0, 0);
    lm_dmm_state_machine(LM_DMM_EVENT_MGP_LOAD_REQ);
    return TRUE;
  }
    z_lmControl.z_session_request.e_gnss_activation_reason = LM_MGP_LOAD_FOR_GNSS_SESSION;
#ifdef  FEATURE_DYNAMIC_LOADING_GNSS
    lm_dmm_state_machine(LM_DMM_EVENT_GNSS_ACTIVATE_REQ);
#else
    lm_mgp_proceed_with_process_fix_request();
#endif

  return TRUE;
}
/*
 ******************************************************************************
 * lm_mgp_get_constellation_config
 *
 * Function description:
 * Returns  
 *              constellation config
 *
 * Parameters: 
 * void
 *
 * Return value: 
 * uint32   Configuration of primary and secondary 
 *          constellation to be used by mgp for
 *          measurement
 * 
 ******************************************************************************
*/

uint32 lm_mgp_get_constellation_config (void)
{
  return tm_core_get_curr_gnss_constellation_config();
  
}
/*
 ******************************************************************************
 * lm_mgp_get_gnns_rcvr_state
 *
 * Function description:
 * Returns  
 *   Current GNSS receiver state
 *
 * Parameters: 
 * void
 *
 * Return value: 
 *  lm_mgp_state_e_type : Current GNSS receiver state 
 ******************************************************************************
*/

lm_mgp_state_e_type lm_mgp_get_gnns_rcvr_state(void)
{
  return z_lmControl.e_currentMgpState;
}

/*
 ******************************************************************************
 * lm_mgp_get_gnss_sess_state
 *
 * Function description:
 * Returns  
 *   Current GNSS session state
 *
 * Parameters: 
 * void
 *
 * Return value: 
 *  lm_mgp_state_e_type : Current GNSS session state 
 ******************************************************************************
*/

lm_session_state_e_type lm_mgp_get_gnss_sess_state(void)
{
  return z_lmControl.e_sessionState; 
}

/*
 ******************************************************************************
 * lm_mgp_get_mgp_load_reason
 *
 * Function description:
 * Returns  
 *   Reason for loading mgp
 *
 * Parameters: 
 * void
 *
 * Return value: 
 *  lm_mgp_load_reason_e_type : Reason for loading mgp
 ******************************************************************************
*/

lm_mgp_load_reason_e_type lm_mgp_get_mgp_load_reason(void)
{
  return z_lmControl.z_session_request.e_gnss_activation_reason ; 
}

/*
 ******************************************************************************
 * lm_mgp_catch_dmm_exception
 *
 * Function description:
 *
 *  Catches any exception thrown by DMM state machine
 *  pending GNSS session 
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/

void lm_mgp_catch_dmm_exception(lm_dmm_exceptions_e_type e_exception)
{ 

/*REV 12/05: This function should be renamed. It will receive a reason for 
  failure and based on that it takes the course of action to take
  like follow goin on session timer expire routine or other routine.
  It will be called by dmm_error_catch_callback

  dmm_error_catch_callback: Is the callback provided to DMM for any error it
  encounters
*/
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"LM_MGP:DMM- threw exception %d",e_exception,0,0);
  switch(e_exception)
  {
   case LM_DMM_EXCEPTION_LOAD_REQ_IN_INCOMPATIBLE_STATE:
   case LM_DMM_EXCEPTION_MARB_MGP_LOAD_FAILURE:
   case LM_DMM_EXCEPTION_ABORT_LOAD:
   case LM_DMM_EXCEPTION_MARB_GNSS_ACTIVATION_REJECT:
   	 {
   	     lm_tm_notify_gnss_availability(FALSE);
	 
         if( (LM_DMM_EXCEPTION_MARB_MGP_LOAD_FAILURE == e_exception ) &&
             (z_lmControl.z_session_request.q_num_fixes == 1) &&
          (os_TimerCheck(z_lmControl.p_sessionTimer) > LM_DMM_EXPECTED_SESS_DURATION_SYS_STATUS_VALID_ALL))
         {
            /* if MARB load request fails first time for single shot client , donot propagate the error to TM.*/
            z_lmControl.v_LoadRetryRequired = TRUE;
            LM_MSG_HIGH("MGP load failure to be ignored for Single shot session. Will try again to load later ",0,0,0);
            return;
         }
         else
         {
           lm_tm_handle_gnss_failure(TM_ERROR_MGP_LOADING_ERROR);
         }
   	 }
      break;
   case LM_DMM_EXCEPTION_MGP_MODULE_OPEN_FAILURE:
     ERR_FATAL("LM_MGP:DMM-MGP Open failed", 0, 0, 0);
     break;
   case LM_DMM_EXCEPTION_MARB_UNKNOWN_RESPONSE:
     ERR_FATAL("LM_MGP:DMM-Arb_req returned unknown value",0, 0, 0);
     break;
   case LM_DMM_EXCEPTION_UNEXPECTED_EVENT:
     ERR_FATAL("LM_MGP:DMM- Received uenxpected event",0, 0, 0);
   case LM_DMM_EXCEPTION_LOAD_REQ_OVER_LOADING:
   /*
     This situation can happen in case:
     when new MGP session request comes to LM, LM request MARB to load MGP and moves DMM to loading state. Now for some reason,
     this session gets cleared (because of several reason like: External session request comes when internal session is ON, 
     we abort existing session on LM. Another reason could be: QMI LOC STOP is sent immediately after request DMM to load).
     At this point, DMM in loading state and new session request comes to LM. So LM will send one more LOAD req, which is captured
     here. We are simply ignoring this request and waiting for DMM to come back with ACCEPT/REJECT/WAIT .
   */
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Ignore LOAD request when already in LOADING state.");
   break;
   default:
     break;
  }
  return;
}

/*
 ******************************************************************************
 * lm_mgp_handle_cached_del_aiding_data_req
 *
 * Function description:
 *
 * Handles cached delete aiding data to sent to MGP
 *
 * Parameters: 
 *   None   
 *
 * Return value: 
 *   None
 ******************************************************************************
*/

void lm_mgp_handle_cached_del_aiding_data_req(void)
{
  uint8 q_Index = 0;
  boolean v_delete = TRUE;
  lm_request_delete_mgp_info_s_type *p_cached_del_aiding_data = NULL;  
  LM_MSG_HIGH("DEbug:lm_mgp_handle_cached_del_aiding_data_req", 0, 0, 0);
  for(q_Index = SM_GNSS_GPS_CONSTELL; q_Index < SM_GNSS_MAX_CONSTELL; q_Index++)
  {
     /* Loop through all the cached data and if the cached one is delAll then no need to send further requests to MGP
        And free the memory which was allocated while caching*/
     p_cached_del_aiding_data = z_lmControl.z_cached_del_aiding_data.p_cached_del_aiding_data[q_Index];
	 
     if(v_delete)
     {       
       lm_mgp_handle_delete_mgp_info(p_cached_del_aiding_data);
	   if((NULL != p_cached_del_aiding_data) && (MGP_GNSS_TYPE_ALL == p_cached_del_aiding_data->z_mgp_info.e_GnssType))
	   {
	      v_delete = FALSE; /* No need to send the req to MGP once we handle MGP_GNSS_TYPE_ALL*/  
	   }
     }
	 os_MemFree((void **) &p_cached_del_aiding_data);
	 z_lmControl.z_cached_del_aiding_data.p_cached_del_aiding_data[q_Index] = NULL;
  }  
  lm_mgp_handle_pos_unc(z_lmControl.z_cached_del_aiding_data.f_cached_new_pos_unc);
  lm_mgp_handle_time_unc(z_lmControl.z_cached_del_aiding_data.f_cached_new_time_unc);
  lm_mgp_handle_time_offset(z_lmControl.z_cached_del_aiding_data.f_cached_time_offset);
}


/*
 ******************************************************************************
 * lm_dmm_handle_gnss_rsc_grant
 *
 * Function description:
 *
 *  Handles the MGP inited event and starts SARF test
 *  or GNSS session as required.
 *
 * Parameters: 
 *
 * Return value: 
 *   lm_dmm_state_e_type : state of DMM state machine
 ******************************************************************************
*/
void lm_mgp_handle_gnss_rsc_grant(void)
{
  /*Sending cached delete aiding data req to MGP after it is loaded*/ 
  lm_mgp_handle_cached_del_aiding_data_req();
  if(LM_MGP_LOAD_FOR_GNSS_SESSION == lm_mgp_get_mgp_load_reason())
  {    
    if (z_lmControl.p_cached_coarse_pos != NULL)
    {
  	  {
        char b_Buffer[250];
        (void)snprintf(b_Buffer, sizeof(b_Buffer), "lat1=%f long1=%f alt=%f\r\n",
                     z_lmControl.p_cached_coarse_pos->latitude,z_lmControl.p_cached_coarse_pos->longitude,z_lmControl.p_cached_coarse_pos->altitude);
        MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", b_Buffer);
      }
      mgp_ExternalCoarsePosPut(z_lmControl.p_cached_coarse_pos);
      os_MemFree((void * *) &z_lmControl.p_cached_coarse_pos);
      LM_MSG_HIGH("lm_mgp_handle_gnss_rsc_grant: Cached CPI injected", 0, 0, 0);
    }

    lm_mgp_proceed_with_process_fix_request();

  }
  else
  {
    lm_diag_proceed_with_mode_switch();
  }
}

/*
 ******************************************************************************
 * lm_mgp_handle_mgp_init_done
 *
 * Function description:
 *
 *  Handles the MGP inited event and starts SARF test
 *  or GNSS session as required.
 *
 * Parameters: 
 *
 * Return value: 
 *   lm_dmm_state_e_type : state of DMM state machine
 ******************************************************************************
*/

void lm_mgp_handle_mgp_init_done(void)
{
  (void)lm_dmm_state_machine(LM_DMM_EVENT_MGP_LOAD_DONE_IND);
  
  if(LM_MPG_LOAD_FOR_INVALID_REASON == lm_mgp_get_mgp_load_reason())
  {
    /*This is to handle the special case when the 
      session is terminated by the time MGP load is 
      completed. In that case the DMM should inform 
      MARB that GNSS is not active.
    */
    lm_dmm_state_machine(LM_DMM_EVENT_RCVR_OFF_IND);
  }
  return;
}



