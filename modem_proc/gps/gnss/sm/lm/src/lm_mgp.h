/*======================================================================

                        Location Manager MGP interaction file

GENERAL DESCRIPTION
 This file contains functions by which Location Manager(LM) handles all 
 the MGP related processing. 

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc. 
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc. 

 Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
 Qualcomm Confidential and Proprietary. All Rights Reserved.
 Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
 Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
 Copyright (c) 2013 - 2019 Qualcomm Technologies, Inc. 
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc. 
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/lm/src/lm_mgp.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
08/21/19   ska     Added support for NMEA GSV
04/01/19   nath    Added debug NMEA related code.
12/14/17   mj    Block sessions during SARF 
04/02/16   mj    GERA Support
09/23/15   mj    Update function lm_mgp_is_fix_source_reportable to add new parm
06/04/15   jyw   Added support for CPE Timing control
07/18/14   jyw   Added support for CPE measurements
02/17/16   mj    Update function comments for lm_mgp_is_fix_source_reportable 
02/17/16   mj    Added new function lm_mgp_is_fix_source_reportable to return if fix reportable 
06/07/13   rh    Removed NO_FIX report handling (now BestAvailPos) 
02/10/10   vp    OEM DR Data Enablement
01/12/10   gk    Force receiver off when end session is called
06/12/08   lt    Support for external coarse position injection.
08/01/07   rw    Add support for XTRA feature, GPS time info
06/20/06   mr    Initial creation of header file.
======================================================================*/
#ifndef LM_MGP_H
#define LM_MGP_H


#include "lm_api.h"
#include "mgp_api.h"
#include "lm_data.h"


/****************** Data Structures*********************************************/
typedef PACKED struct PACKED_POST
{
  uint32 q_GpsRtcMs;
  /*RTC MilliSeconds*/
  uint32 q_receiver_state;
  /*Receiver state enum*/
} lm_recvr_state_event_info;

typedef enum
{
  LM_UPDATE_MGP_STATE_FLAG_NONE            = 0x0,
  LM_UPDATE_MGP_STATE_FLAG_FORCED          = 0x1,
  LM_UPDATE_MGP_STATE_FLAG_TRANSITION      = 0x2
}lm_update_mgp_state_req_flag;



/****************** Function Definitions**************************************/

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
mgp_ConfigOperationEnumType lm_mgp_get_mgp_operation_mode(lm_gps_mode_e_type e_lm_mode);

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
                                lm_update_mgp_state_req_flag e_update_flag);
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
boolean lm_mgp_start_position_fix_processing(const lm_session_request_fix_s_type *const p_FixReq);

/*
 ******************************************************************************
 * lm_mgp_stop_position_fix_processing 
 *
 * Function description:
 *
 * This function stops a position fix processing. 
 *
 * Parameters: 
 * u_ForceMgpOff - Forces MGP to turn off immediately
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_stop_position_fix_processing(boolean u_forceMgpOff);


/*
 ******************************************************************************
 * lm_mgp_handle_fix_report#9x05 keep
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
void lm_mgp_handle_fix_report(os_IpcMsgType *p_msg);

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
void lm_mgp_rcvr_state_change(lm_mgp_state_e_type e_mgpState);

/*
 ******************************************************************************
 * lm_mgp_handle_status_update #9x05 keep
 *
 * Function description:
 *
 * This function handles the MGP status updated coming from MGP.
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
void lm_mgp_handle_status_update(os_IpcMsgType *p_msg);

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
void lm_mgp_update_mgp_state_after_final_fix(uint32 q_num_fixes, uint32 q_tbf);

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
void lm_mgp_handle_delete_mgp_info(lm_request_delete_mgp_info_s_type *p_Req);

/*
 ******************************************************************************
 * lm_mgp_handle_wbiq_info #9x05 keep
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
void lm_mgp_handle_wbiq_info(os_IpcMsgType *p_msg);

/*
 ******************************************************************************
 * lm_mgp_handle_nav_config_change_resp #9x05 keep
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
void lm_mgp_handle_nav_config_change_resp(os_IpcMsgType *p_Msg);

/*
 ******************************************************************************
 * lm_mgp_handle_nbiq_info  #9x05 keep
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
void lm_mgp_handle_nbiq_info(os_IpcMsgType *p_msg);

/*
 ******************************************************************************
 * lm_mgp_handle_gnss_rf_status_info  #9x05 keep
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
void lm_mgp_handle_gnss_rf_status_info(os_IpcMsgType *p_msg);

/*
 ******************************************************************************
 * lm_mgp_handle_gps_time_request
 *
 * Function description:
 * This function handles the GPS time request from TM.
 * 
 *
 * Parameters: 
 * p_rf_info_request - pointer to the info request
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_gps_time_request(lm_request_gps_info_s_type *p_gps_time_request);


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
void lm_mgp_handle_rcOn_timer_timeout(void);


/*
 ******************************************************************************
 * lm_mgp_handle_shutOff_timer_timeout
 *
 * Function description:
 * This function handles the shut-Off timer timeout event. If there is no fix or 
 * PRM session going on, and MGP is not in OFF state, MGP will be put in OFF
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
void lm_mgp_handle_shutOff_timer_timeout(void);

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
void  lm_mgp_handle_pos_unc(FLT f_new_pos_unc);

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
void  lm_mgp_handle_time_unc(FLT f_new_time_unc);

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
void  lm_mgp_handle_time_offset(FLT f_time_offset);

/*
 ******************************************************************************
 * lm_mgp_handle_slow_clk_time_inject_request
 *
 * Function description:
 * This function injects external time from sources like PC or network into MGP
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
void  lm_mgp_handle_slow_clk_time_inject_request(lm_request_slow_clk_time_inject_s_type *p_slow_clk_inject);

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
(lm_request_extern_coarse_pos_inject_s_type *p_extern_coarse_pos_inj);

/*
 ******************************************************************************
 * lm_mgp_handle_gps_time #9x05 keep
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
void lm_mgp_handle_gps_time(os_IpcMsgType *p_msg);

/*
 ******************************************************************************
 * lm_mgp_handle_request_sa_data #9x05 keep
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
void lm_mgp_handle_request_sa_data(void);

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
void lm_mpg_handle_force_rcvr_off(void);

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

void lm_mpg_handle_rcvr_off(void);
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
void lm_mgp_handle_gps_meas(const os_IpcMsgType* p_msg);


/**
* @brief  LM abstraction layer around mgp_UpdateInfo. This
*         abstratcion implements a "shim" to enable backwards
*         compatibility with GEN7 MGP.
  
  @return      
  @sideeffects None
* @see mgp_GnssUpdateInfo() and mgp_UpdateInfo()

*/
boolean lm_mgp_update_info(gnss_AssistDataInfoStructType *p_assistDataInfo
                           /**< GEN8 assistance data received from network */);
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
void lm_mgp_send_fix_start_ind(uint32 q_Tbf);

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
void lm_mgp_send_fix_end_ind(void);

/*
 ******************************************************************************
 * lm_xtraInjDone #9x05 keep
 *
 * Function description:
 *
 * This function handles IND from CD on completion of an Xtra injection
 *
 * Parameters: 
 *  gnss_XtraInjMaskType  q_AcceptedMask
 *
 * Return value: 
 *  None
 *
 ******************************************************************************
*/
void lm_xtraInjDone(gnss_XtraInjMaskType  q_AcceptedMask);

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
boolean lm_mgp_is_fix_source_reportable(const sm_GpsFixRptStructType *p_fixReport, boolean v_reportableFinalfix);

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
boolean lm_is_mgp_on(void);

/*
 ******************************************************************************
 * lm_mgp_handle_xooffset_info #9x05 keep
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
void lm_mgp_handle_xooffset_info(sm_ReportXOOffsetInfoStructType *pz_XOOffsetData);

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
void lm_mgp_report_SARF_status(const boolean v_sarf_status);

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
boolean lm_mgp_get_SARF_status(void);

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

boolean lm_mgp_process_fix_request(const lm_session_request_info_s_type* const p_startReq);

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

uint32 lm_mgp_get_constellation_config (void);

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

lm_mgp_state_e_type lm_mgp_get_gnns_rcvr_state(void);


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

lm_session_state_e_type lm_mgp_get_gnss_sess_state(void);

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

lm_mgp_load_reason_e_type lm_mgp_get_mgp_load_reason(void);
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

void lm_mgp_catch_dmm_exception(lm_dmm_exceptions_e_type e_exception);
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
void lm_mgp_handle_gnss_rsc_grant(void);

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
 ******************************************************************************
*/

void lm_mgp_handle_mgp_init_done(void);

#endif /* LM_MGP_H */
