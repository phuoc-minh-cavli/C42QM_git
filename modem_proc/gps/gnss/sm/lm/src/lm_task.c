/*======================================================================

               LM task main file

 GENERAL DESCRIPTION
  This file contains the implementation of Location Manager(LM) task.
  This task handles requests from various tasks like TM, MGP, GPS DIAG. 

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2020 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/lm/src/lm_task.c#2 $
  $DateTime: 2021/02/11 01:36:31 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/08/20   ska     Smart compilation issues
02/26/20   katta   Changed the info passed to LM from marb cb
22/03/20   ska     Changes done to support delete aiding data when MGP unload
08/21/19   ska     Added support for NMEA GSV
07/04/19   py      Retry again If single shot session fails due to MGP load fail
04/17/19   nath    Added code for CPI cache
10/17/17   rn      Added Xtra Integrity support.
07/19/17   ak      Add support to read NV value for HEPE threshold to be used for DR/INS fixes
09/14/16   rn      Add ULog support for SM.
06/04/15   jyw     Added support for CPE Timing control
07/18/14   jyw     Added support for CPE measurements
08/11/14   jv      Added Constellation configuration support
10/28/13   hm      Added Rf dev test memory release handler call at the end of RF dev test event report
06/07/13   rh      Removed NO_FIX report handling (now BestAvailPos) 
02/08/13   ssu     Move to the DOG Heartbeat mechanism on DIME and beyond. 
02/10/10   vp      OEM DR Data Enablement
06/24/08   ank     Reinstate dynamic hepe logic for tunnel-exit scenario.
06/18/08   jlp     Added messaging support so tm_diag can set the Sv No Exist
                    list in cd.
08/01/07   rw      Add GPS time info support
07/31/07   ank     Added Dynamic QoS and Override logic.
07/23/07   jw      By default should not disable Jammer Detect in MGP.
12/19/06   br      Clean-up.
10/11/06   br/mr   Removed include pdsmi.h.
04/04/06   mr      Initial creation of header file.
======================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "msg.h"
#include "task.h"
#define LM_DATA_DECLARE 
#include "lm_data.h"  /*Don't move this declaration down*/
#undef LM_DATA_DECLARE
#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "lm_iface.h"
#include "lm_tm.h"
#include "lm_mgp.h"
#ifdef FEATURE_CGPS_USES_CDMA
#include "lm_mgp_aflt.h"
#endif /* FEATURE_CGPS_USES_CDMA */
#include "lm_diag.h"
#include "sm_api.h"
#include "queue.h"
#include "event.h"
#include "sm_nv.h"
#include "sm_oemdrsync.h"
#include "sm_nv_efs.h"
#include "mgp_api.h"
#include "sm_util.h"
#include "tm_api.h"
#ifdef FEATURE_DYNAMIC_LOADING_GNSS
#include "marb.h"
#endif /* FEATURE_DYNAMIC_LOADING_GNSS */
#include "fs_public.h"
#include "lm_dmm.h"
#include "loc_utils.h"

typedef enum
{
  LM_GNSS_ACTIVE,
  LM_GNSS_INACTIVE
} LmGnssProcEnumType;


typedef struct 
{
  uint32 dummy;
} LmCommonInfoStructType;

#ifdef SM_LM_DMM_UNLOAD_UT_TRIGGER
uint8 lm_unload_mask = 255;
#endif

static boolean b_lm_gnss_arb_unload_started = FALSE;

static volatile enum { LM_TASK_STATE_RUN, LM_TASK_STATE_STOP } e_lm_task_state;
uint32 lmq_DogPatCnt, lmq_StartGpsMs = 1000;

#ifdef FEATURE_GNSS_DYNAMIC_SP_MEM_ALLOCATION
  extern void mgp_IqTestCompleteMemRelHandler(void);
#endif

/****************** Function Definitions**************************************/

boolean lm_dmm_send_ipc_to_lm(const void *p_Data, uint32 q_Size)
{
  os_IpcMsgType* p_ipc_ptr = NULL;  
  const marb_async_callback_input_t * p_input_ptr;
  boolean result = TRUE;  
  LmCommonInfoStructType *p_hdr_ptr;
  if ( ( q_Size > 0 ) && ( NULL == p_Data ) )
  {
  	
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Cannot generate LM msg with data ptr uninitialized");
	return FALSE;
  }

  p_ipc_ptr = os_IpcCreate(sizeof(marb_async_callback_input_t),
                             IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_LM); 
  
  p_input_ptr = (const marb_async_callback_input_t *)p_Data;

  if(p_ipc_ptr == NULL)
  {
    result = FALSE;
  }
  else
  {
	switch (p_input_ptr->cb_type)
    {
      case MARB_EVENT_NOTIFY_CB:
      {
		  p_ipc_ptr->q_MsgId = C_SM_LM_MSG_ARB_NOTIFY_IND;

        break;
      }
      case MARB_ACCEPT_CB:
      {
		p_ipc_ptr->q_MsgId = C_SM_LM_MSG_ARB_LOAD_ACCEPT;
        break;
      }
      case MARB_UNLOAD_CB:
      {
		  p_ipc_ptr->q_MsgId = C_SM_LM_MSG_ARB_UNLOAD_IND;
        break;
      }
      default:
        ERR_FATAL("MARB unknown cb_type=%d", p_input_ptr->cb_type,0,0);
    }
    p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_LM;
    p_ipc_ptr->q_Size           = q_Size;

	memscpy(p_ipc_ptr->p_Data, q_Size, p_Data, q_Size);


    if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_LM))
	{
       result = FALSE;
       (void)os_IpcDelete(p_ipc_ptr);
    }

  }
  return result;
}



/*
 ******************************************************************************
 * lm_startup
 *
 * Function description:
 *
 * Contains task initialization code.
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/
static void lm_startup( void )
{
  /*Call all the initialization code here*/
  OS_ULOG_RESULT q_ULogInitStatus;
  boolean ret = os_IpcStart((uint32)THREAD_ID_SM_LM);

  if(ret == FALSE)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"PDCOMM APP: IPC Start Failed",0,0,0);
  }

  /* Initialize ULog Handle for LM, last two arguments are default */
  q_ULogInitStatus = os_ULogInit(&lm_ULogHandle,"LM_ULOG", LM_ULOG_SIZE,0,0);
  if (q_ULogInitStatus != 0) 
  {
    /* Check ULOG_ERRORS Enum for Error Code in ULOG.H */
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"LM ULOG Handle Init Failed with Error: %d",  
          q_ULogInitStatus,0,0);
    ERR_FATAL("LM ULOG Handle Init Failed", 0, 0, 0);
  }
  else
  {
    OS_ULOG_PRINT_0(lm_ULogHandle, "LM ULOG Handle created successfully");
  }

  os_TaskReadyAck();
  os_TaskStopRegister();

  /* Register for DOG heartbeat monitoring */
  os_DogHeartbeatRegister();

#ifndef FEATURE_GNSS_NAVHW_GEN8C
  {
    static uint8 gu_StartGpsatBoot = 0;
    if(gu_StartGpsatBoot==1)
    {
      z_lmControl.p_GpsStartTimerPowerTest = os_TimerCreateInCgpsTimerGroup((uint32)LM_POWER_TEST_GPS_START_TIMER_ID,(uint32)THREAD_ID_SM_LM);
      (void) os_TimerStart(z_lmControl.p_GpsStartTimerPowerTest, lmq_StartGpsMs, NULL);
    }
  }
#endif /* ! FEATURE_GNSS_NAVHW_GEN8C */

} /*lm_startup() ends*/

/*===========================================================================

FUNCTION lm_read_nv

DESCRIPTION
  This function reads the following LM NV items:
  1. CGPS Application-Based Tracking GPS-On Threshold 
  2. CGPS Application-Based Tracking GPS-Idle Threshold
  3. CGPS Application-Based Tracking Periodic Request Delay Margin
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void lm_read_nv(void)
{
  /* NV buffer, used to read NV items */
  nv_item_type      *z_nv_read_buffer=NULL;
  uint32			hepe_threshold = 0;  
  z_nv_read_buffer = (nv_item_type*)os_MemAlloc(sizeof(nv_item_type), OS_MEM_SCOPE_TASK);
  
  if (z_nv_read_buffer==NULL)
  {
	MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory");
	return;
  }
  

  if(sm_nv_read(NV_AAGPS_APP_TRACKING_GPSON_THSLD_I, (void *)z_nv_read_buffer))
  {
    LM_MSG_MED("Read NV NV_AAGPS_APP_TRACKING_GPSON_THSLD_I(sec) = %d",
               (uint32)z_nv_read_buffer->aagps_app_tracking_gpson_thsld,0,0);  
    lm_set_rc_on_threshold_period((uint32)z_nv_read_buffer->aagps_app_tracking_gpson_thsld*1000);
  }
  else
  {
    LM_MSG_ERROR("Failed reading NV NV_AAGPS_APP_TRACKING_GPSON_THSLD_I setting default(sec) = %d",
                 LM_MAX_RC_ON_THRESHOLD/1000,0,0);
    lm_set_rc_on_threshold_period(LM_MAX_RC_ON_THRESHOLD);
  }

  /* CGPS Application-Based Tracking GPS-Idle Threshold*/
  if(sm_nv_read(NV_AAGPS_APP_TRACKING_GPSIDLE_THSLD_I, (void *)z_nv_read_buffer))
  {
    LM_MSG_MED("Read NV NV_AAGPS_APP_TRACKING_GPSIDLE_THSLD_I(sec) = %d",
               (uint32)z_nv_read_buffer->aagps_app_tracking_gpsidle_thsld,0,0);    
    lm_set_rc_idle_threshold((uint32)z_nv_read_buffer->aagps_app_tracking_gpsidle_thsld*1000);
  }
  else
  {
    LM_MSG_ERROR("Failed reading NV NV_AAGPS_APP_TRACKING_GPSIDLE_THSLD_I setting default(sec) = %d",
                 LM_MAX_RC_IDLE_THRESHOLD/1000,0,0);
    lm_set_rc_idle_threshold(LM_MAX_RC_IDLE_THRESHOLD);
  }

  /* CGPS Application-Based Tracking Periodic Request Delay Margin*/
  if(sm_nv_read(NV_AAGPS_APP_TRKG_PERIODIC_REQ_DLY_MARGIN_I, (void *)z_nv_read_buffer))
  {
    LM_MSG_MED("Read NV NV_AAGPS_APP_TRKG_PERIODIC_REQ_DLY_MARGIN_I(sec) = %d",
               (uint32)z_nv_read_buffer->aagps_app_trkg_periodic_req_dly_margin,0,0);      
    lm_set_rc_shut_off_delay_margin((uint32)z_nv_read_buffer->aagps_app_trkg_periodic_req_dly_margin*1000);
  }
  else
  {
    LM_MSG_ERROR("Failed reading NV NV_AAGPS_APP_TRKG_PERIODIC_REQ_DLY_MARGIN_I setting default(sec) = %d",
                 LM_RC_SHUT_OFF_DELAY_MARGIN_DEFAULT/1000,0,0);
    lm_set_rc_shut_off_delay_margin(LM_RC_SHUT_OFF_DELAY_MARGIN_DEFAULT);
  }
  /* CGPS QoS_Override_Threshold */
  if(sm_nv_read(NV_CGPS_QOS_OVERRIDE_ACC_THRESHOLD_I, (void *)z_nv_read_buffer))
  {
    LM_MSG_MED("Read NV NV_CGPS_QOS_OVERRIDE_ACC_THRESHOLD_I(m) = %d",
               (uint32)z_nv_read_buffer->cgps_qos_override_acc_threshold,0,0);      
    z_lmControl.z_early_exit_decision.w_QoSThresholdOverride = z_nv_read_buffer->cgps_qos_override_acc_threshold;
  }

  /* CGPS QoS_Override_Timeout */
  if(sm_nv_read(NV_CGPS_QOS_OVERRIDE_TIME_I, (void *)z_nv_read_buffer))
  {
    LM_MSG_MED("Read NV NV_CGPS_QOS_OVERRIDE_TIME_I(s) = %d",
               (uint32)z_nv_read_buffer->cgps_qos_override_time,0,0);      
    z_lmControl.z_early_exit_decision.q_QoSTimeoutOverride = z_nv_read_buffer->cgps_qos_override_time * 1000;
  }

  /* Read HEPE Max threshold*/
  sm_nv_init();
  sm_nv_efs_init();
  (void)sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD, 
                                sizeof(uint32),
                                &hepe_threshold);
  /* Validate for boundary limits*/
  if( hepe_threshold <= GNSS_NV_EFS_SM_LM_MAX_HEPE_THRESHOLD && hepe_threshold >= GNSS_NV_EFS_SM_LM_MIN_HEPE_THRESHOLD)
  {
      z_lmControl.q_HepeThreshold = hepe_threshold;
  }
  else
  {
      /* if out of boundary limits, force it to default value */
      LM_MSG_ERROR("NV item has out of limits value = %d, hence setting default NV NV_GNSS_LM_HEPE_THRESHOLD = %d ",(int32) hepe_threshold, (int32)GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD_DEFAULT,0);
      z_lmControl.q_HepeThreshold = GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD_DEFAULT;
  }

  /* Read HEPE threshold for DR/INS fixes*/
  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD_FOR_DR,
                               sizeof(uint32), &z_lmControl.q_HepeThresholdForDRFix) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    z_lmControl.q_HepeThresholdForDRFix = GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD_FOR_DR_DEFAULT;
  }

  sm_init_constellation_config();
  os_MemFree((void * *) &z_nv_read_buffer);
}

/*
 ******************************************************************************
 * lm_task_state_init
 *
 * Function description:
 *
 * Initializes the variables in z_lmControl structure
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/
void lm_task_state_init( void )
{
  lm_tm_clear_session();
  z_lmControl.e_afltState = LM_MGP_AFLT_STATE_OFF;
  z_lmControl.e_currentMgpState =  LM_MGP_STATE_OFF;
  z_lmControl.e_desiredMgpState =  LM_MGP_STATE_OFF;
  memset(&z_lmControl.z_fix_request,0,sizeof(z_lmControl.z_fix_request));
  memset(&z_lmControl.z_gps_request,0,sizeof(z_lmControl.z_gps_request));
  /*Set MGP maintenance related thresholds and timeouts to default values*/
  lm_read_nv();
  z_lmControl.u_searchMode = 0x10; /*Set default GPS SRCH mode: High */ 
  z_lmControl.u_DisableJammerDetect = FALSE; /* Default is to not disabe Jammer Detection*/    
  z_lmControl.z_utcData.u_utcModelValid = FALSE;

  z_lm_sarf_state_db.e_client_type = LM_SARF_CLIENT_DIAG;
  z_lm_sarf_state_db.e_sarf_state = LM_SARF_MODE_SLEEP;
  /*MRAWAL: read UTC data from EFS*/

  if((z_lmControl.z_early_exit_decision.w_QoSThresholdOverride) &&
     (z_lmControl.z_early_exit_decision.q_QoSTimeoutOverride))
  {
    z_lmControl.z_early_exit_decision.u_UseQoSOverride = TRUE;
  }
  else
  {
  z_lmControl.z_early_exit_decision.u_UseQoSOverride = FALSE;
  }
  z_lmControl.z_early_exit_decision.u_FirstValidFixAfterEngineOn = FALSE;
  z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart = FALSE;
  qw_set(z_lmControl.z_early_exit_decision.d_TimeEngOn,0,0);
  qw_set(z_lmControl.z_early_exit_decision.d_TimeSessionStart,0,0);
  z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn = 0;
  z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart = 0;
  z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = 0;
  z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn = FALSE;
  z_lmControl.z_early_exit_decision.f_LastValidFixHepe = LM_LAST_VALID_FIX_HEPE_MAX;
  z_lmControl.z_early_exit_decision.q_NullFixReportCount = 0;
  z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe = FALSE;
  z_lmControl.z_cached_del_aiding_data.f_cached_new_pos_unc = LM_INVALID_VALUE;
  z_lmControl.z_cached_del_aiding_data.f_cached_new_time_unc = LM_INVALID_VALUE;
  z_lmControl.z_cached_del_aiding_data.f_cached_time_offset = LM_INVALID_VALUE;

  lm_dmm_init();
  lm_dmm_register_cb(lm_mgp_catch_dmm_exception , lm_mgp_handle_gnss_rsc_grant);
#ifndef FEATURE_DYNAMIC_LOADING_GNSS
  /*
   Initialization of MGP is to be done by LM task 
   So if dynamic loading of MGP is not enabled 
   initialize MGP with lm_state_init
*/
  lm_dmm_initialize_mgp();  
#endif



}

/*
 ******************************************************************************
 * lm_create_timers
 *
 * Function description:
 *
 * Creates the CDMA, FIX and GPS session timers.
 *
 * Parameters: 
 *
 * None
 *
 * Return value: 
 *
 ******************************************************************************
*/
void lm_create_timers(void)
{
  /*Create session timers*/
  z_lmControl.p_fixTimer  = os_TimerCreateInCgpsTimerGroup((uint32)LM_FIX_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lmControl.p_sessionTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_SESSION_TIMER_ID,(uint32)THREAD_ID_SM_LM);

  /*Create MGP state timers*/
  z_lmControl.p_goingONTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_GOING_ON_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lmControl.p_goingOFFTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_GOING_OFF_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lmControl.p_goingIDLETimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_GOING_IDLE_TIMER_ID,(uint32)THREAD_ID_SM_LM);  

  /*Create App based tracking related MGP state timers*/
  z_lmControl.p_rcOnTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_RC_ON_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lmControl.p_mgpShutOffTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_MGP_SHUT_OFF_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lmControl.p_IqTestingTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_IQ_TESTING_TIMER_ID,(uint32)THREAD_ID_SM_LM);

}
/*
 ******************************************************************************
 * lm_handle_stop
 *
 * Description:
 *
 * This function is used to handle Stop IPC
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
void lm_handle_stop( void )
{
  LM_MSG_HIGH("TASK STOP received",0,0,0);

  /* Deregister with the Dog Heartbeat before stopping task */
  os_DogHeartbeatDeregister();

  os_TaskStopAck();
}

/*
 ******************************************************************************
 * lm_handle_offline
 *
 * Description:
 *
 *  This function is used to handle TASK_OFFLINE IPC
 *
 * Parameters: 
 *
 *  None
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void lm_handle_offline( void )
{

  LM_MSG_HIGH("TASK OFFLINE received",0,0,0);

}


/*
 ******************************************************************************
 * lm_proc
 *
 * Function description:
 *
 * This function handles the all the incoming IPC messages. It will call the
 * appropriate function based on the type of IPC message received.
 *
 * Parameters: 
 *
 * p_msg - Pointer to received IPC message 
 *
 * Return value: 
 *
 ******************************************************************************
*/
void lm_proc( os_IpcMsgType* p_msg )
{
  /*lm_task calls this function whenever it recieves a message*/

  if( p_msg == NULL )
  {
      LM_MSG_MED(" Received NULL IPC Message ",0,0,0);
      return;
  }
   
  switch(p_msg->q_MsgId)
  {
    case C_SM_LM_MSG_SESSION_REQUEST:
    {
      lm_tm_handle_session_request(p_msg);
      break;
    }
    
    case C_SM_LM_MSG_ARB_LOAD_ACCEPT:
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "DMM_SM:IPC C_SM_LM_MSG_ARB_LOAD_ACCEPT is received", 0, 0, 0);
      /*REV 12/04: Rename LM_DMM_EVENT_DMM_OPEN_MODULE to LM_DMM_EVENT_DMM_LOAD_GNSS */
      (void)lm_dmm_state_machine(LM_DMM_EVENT_ARB_ACCEPT_IND);
     break;
     
    case C_SM_LM_MSG_MGP_INITED_IND:
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "DMM_SM: IPC C_SM_LM_MSG_MGP_INITED_IND is received", 0, 0, 0);
      lm_mgp_handle_mgp_init_done();
     
     break;
     
    case C_SM_LM_MSG_MGP_DEINITED_IND:
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "DMM_SM: IPC C_SM_LM_MSG_MGP_DEINITED_IND is received.", 0, 0, 0);
      (void)lm_dmm_state_machine(LM_DMM_EVENT_MGP_UNLOAD_DONE_IND);
     
     break;
     
    case C_SM_LM_MSG_ARB_NOTIFY_IND:
	 {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "DMM_SM: IPC C_SM_LM_MSG_ARB_NOTIFY_IND is received");
      (void)lm_tm_handle_arb_notify(p_msg);
      (void)lm_notify_marb_events_to_tle(p_msg);
      break;
    } 
    case C_SM_LM_MSG_TEST_trigger_mode_switch_enter_sarf:
    {
       mode_switch(CGPS_ENTER_SARF_MODE);
      break;
    }
    
    case C_SM_LM_MSG_REQUEST:
    {
      lm_tm_handle_request(p_msg);
      break;
    }
    
    case C_SM_LM_MSG_INFO:
    {
      lm_tm_handle_info(p_msg);
      break;
    }

    /* Handle timer expiry messages*/
    case C_OS_MSG_ID_TIMER_EXPIRY:
    { 
       /*lint -e826 */              
      switch(((os_TimerExpiryType*)&p_msg->p_Data[0])->q_TimerId)
      {
        case LM_FIX_TIMER_ID:
        {
          lm_tm_handle_fix_timer_timeout();
          break;
        }
        case LM_SESSION_TIMER_ID:
        {
          lm_tm_handle_session_timer_timeout();
          break;
        }
//  TODO  [AN] [DEL] PRM timer deleted
        case LM_GOING_ON_TIMER_ID:
        {
          LM_MSG_HIGH("DMM_SM:LM_GOING_ON_TIMER_ID expiry received", 0, 0, 0);
          lm_tm_handle_gnss_failure(TM_ERROR_MGP_STATUS_TIMEOUT);
          break;
        }
        case LM_GOING_OFF_TIMER_ID:
        {
          lm_tm_handle_goingOff_timer_timeout();
          break;
        }
        case LM_GOING_IDLE_TIMER_ID:
        {
          lm_tm_handle_goingIdle_timer_timeout();
          break;
        }
          //  TODO [an][del] #9x05 lm_tm_handle_goingLppm_timer_timeout();
        case LM_RC_ON_TIMER_ID:
        {
          lm_mgp_handle_rcOn_timer_timeout();
          break;
        }
        case LM_MGP_SHUT_OFF_TIMER_ID:
        {
          lm_mgp_handle_shutOff_timer_timeout();
          break;
        }
        case LM_IQ_TESTING_TIMER_ID:
        {
          LM_MSG_HIGH("Iqtest testing timer expired", 0, 0, 0);
          break;
        }
#ifndef FEATURE_GNSS_NAVHW_GEN8C
        case LM_POWER_TEST_GPS_START_TIMER_ID:
        {
          z_lm_sarf_state_db.p_client_cb = NULL;
          z_lm_sarf_state_db.e_client_type = LM_SARF_CLIENT_DIAG;

          #include "cgps_api.h"
          mode_switch( CGPS_ENTER_SARF_MODE );

          break;
        }
#endif /* ! FEATURE_GNSS_NAVHW_GEN8C */
#ifdef FEATURE_DYNAMIC_LOADING_GNSS

        case LM_MGP_LOAD_TIMER_ID:
        {
          lm_dmm_load_timeout();
          ERR_FATAL("DMM_SM: Forced crash as MGP did not respond to init request",0 ,0 ,0);
          break;
        }
        case LM_DMM_LOAD_ARB_REQ_WAIT_TIMER_ID:
        {
          lm_dmm_load_timeout();
          ERR_FATAL("DMM_SM: Forced crash as ARB did not reply to load request",0 ,0 ,0);
          break;
        }
        case LM_MGP_LOAD_ATTEMPT_BACK_OFF_TIMER_ID:
            {
          lm_dmm_mgp_load_backoff_timer_timeout();
              break;
            }
        case LM_MGP_UNLOAD_TIMER_ID:
            {
          lm_dmm_unlLoad_timeout();/*Need to act as going on timer expired*/
              break;
            }
#endif		
        default:
        {
          LM_MSG_ERROR("Invalid timer id %d in timeout message",
                       ((os_TimerExpiryType*)&p_msg->p_Data[0])->q_TimerId,0,0); /*lint -e826 */ 
        }

      }/*switch(*((lm_timer_ids_e_type*)p_msg->p_Data) ends*/
      break;
    }
    case C_SM_LM_MSG_ARB_UNLOAD_IND:
    {
      LM_MSG_HIGH("DMM_SM:C_SM_LM_MSG_ARB_UNLOAD_IND received. Start unloading.Proceed as if going on timer expired", 0, 0, 0);
      (void)lm_dmm_state_machine(LM_DMM_EVENT_ARB_UNLOAD_IND); /* to dmm_state_machine back to unloaded state in case it is in loading state */
      break;
    }

    /*Handle MGP messages*/

      /*****************************************************
      ** MSG ID's: MGP Info reports MGP => LM
      ******************************************************/
    case ARIES_MSG_ID_MGP_LM_REPORT_FIX:
    {
      lm_mgp_handle_fix_report(p_msg);
      break;
    }  

    /*Handle MGP messages*/

      /*****************************************************
      ** MSG ID's: MGP Info reports MGP => LM
      ******************************************************/

	case ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS:
	{
	  switch(((sm_GpsMeasRptStructType*) p_msg->p_Data)->e_MeasRptReason)
      {
        case GPS_MEAS_RPT_PERIODIC:
        {
           MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Meas MGP Event %d\n",  ((sm_ReportEventStructType*)p_msg->p_Data)->e_EventType);
          
           lm_mgp_handle_gps_meas(p_msg);  
        } /* end case GPS_MEAS_RPT_PERIODIC*/
        break;
	    default:
         LM_MSG_ERROR("unhandled meas report type [%d]", 
                      ((sm_GpsMeasRptStructType*) p_msg->p_Data)->e_MeasRptReason, 0, 0);
         break;
	  }
	  break;
    } /* end case ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS */	  
    /***************************************************
    ** MSG ID's: MGP status reports MGP => LM
    ****************************************************/
    case ARIES_MSG_ID_MGP_LM_REPORT_GENERAL_STATUS:
    {
      lm_mgp_handle_status_update(p_msg);
      break;
    }
    
    case ARIES_MSG_ID_MGP_LM_REPORT_EVENT:
    {
      /*MRAWAL: need to handle event reports from MGP*/
      switch( ((sm_ReportEventStructType*)p_msg->p_Data)->e_EventType )
      {
        case SM_EVENT_ME_IQ_DONE:
         event_report( EVENT_GPS_LM_IQ_TEST_COMPLETE );  
#ifdef FEATURE_GNSS_DYNAMIC_SP_MEM_ALLOCATION
         
         /* Reported the end of  IQ test, this is a good time to call IQ test memory release handler before another
                   RF dev test or mission mode command is received,as this is converging point of  several event reports from ME*/
          mgp_IqTestCompleteMemRelHandler(); 
#endif
        break;
      
        case SM_EVENT_PE_NEW_NAVDATA:
        case SM_EVENT_PE_NEW_ALMANAC:
        case SM_EVENT_PE_DEL_NAVDATA:
      case SM_EVENT_PE_DEL_ALMANAC:
      (void)lm_tm_send_mgp_event_to_tm((sm_ReportEventStructType*)(p_msg->p_Data)); 
      break;
      
        default:
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Unknown MGP Event %d\n",  ((sm_ReportEventStructType*)p_msg->p_Data)->e_EventType , 0, 0);
          break;
      }
      break;
    }

    /***************************************************
    ** MSG ID's: MC WBIQ  Info reports MGP => LM
    ****************************************************/
    case ARIES_MSG_ID_MGP_LM_REPORT_WB_IQ_PARAMS:
    {
      lm_mgp_handle_wbiq_info(p_msg);
      break;
    }
    
    case ARIES_MSG_ID_MGP_LM_NAV_CONFIG_CHANGE_RESP:
    {
      lm_mgp_handle_nav_config_change_resp(p_msg);
      break;
    }

    /***************************************************
    ** MSG ID's: MC NBIQ info reports MGP => LM
    ****************************************************/
    case SM_MSG_ID_MGP_LM_REPORT_NBIQ_PARAMS:
    {
      lm_mgp_handle_nbiq_info(p_msg);
      break;
    }

    /***************************************************
    ** MSG ID's: GPS time reports MGP => LM
    ****************************************************/
    case ARIES_MSG_ID_MGP_LM_REPORT_GPS_TIME:
    
      lm_mgp_handle_gps_time(p_msg);  
      break;  
    

    /***************************************************
    ** MSG ID's: MGP data request MGP => LM
    ****************************************************/    
    /*Handle DIAG messages*/
    case C_SM_LM_DIAG_GSC_MSG:
    {
      lm_diag_handle_diag_gsc_msg(p_msg);
      break;
    }
    
    case C_SM_LM_SV_NO_EXIST:
    {
      const lm_sv_no_exist_cmd_type * cpz_SvNoExistCmd
       = (const lm_sv_no_exist_cmd_type *)p_msg->p_Data;
      (void) cpz_SvNoExistCmd;
      LM_MSG_MED("Sv No Exist cmd rcvd from TM diag %08X, %u",
       cpz_SvNoExistCmd->q_SvNoExistMask, cpz_SvNoExistCmd->e_Action, 0);
      lm_SvNoExistCmd_handler( p_msg ); /* Sv No Exist command handler */
      break;
    }

    case C_SM_LM_FORCE_DPO_FOR_POWER_MEAS:
    {
      const lm_force_dpo_for_power_meas_cmd_type * cpz_ForceDpoForPowerMeasCmd
       = (const lm_force_dpo_for_power_meas_cmd_type *)p_msg->p_Data;
      (void) cpz_ForceDpoForPowerMeasCmd;
      LM_MSG_MED("Force DPO for Power Meas cmd rcvd from TM diag %u",
       cpz_ForceDpoForPowerMeasCmd->u_ForceDpoForPowerMeas, 0, 0);
      lm_ForceDpoForPowerMeasCmd_handler( p_msg ); /* Force DPO for Power Meas command handler */
      break;
    }

    /*Handle task maintenance messages*/
    case C_OS_MSG_ID_OFFLINE: 
    {
      lm_handle_offline();
      break;
    }
    
    case C_OS_MSG_ID_STOP: 
    {
      lm_handle_stop();
      break;
    }

    case ARIES_MSG_ID_TM_LM_PRESC_DWELL:
    {
      LM_MSG_MED("Presc dwell cmd rcvd from TM diag",0,0,0);
      lm_presc_dwell_handler( p_msg ); /* Prescribed dwell command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_SARF_MODE_SWITCH:
    {
      LM_MSG_MED("SARF mode-switch IPC rcvd from TM diag",0,0,0);
      lm_Rf_VerifModeSwitch_handler( p_msg ); /* Prescribed dwell command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_IQ_TEST:
    {
      LM_MSG_MED("IqTest cmd rcvd from TM diag",0,0,0);
      lm_IqTestCmd( p_msg ); /* IQ Test command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_1SV_STANDALONE:
    {
      LM_MSG_MED("Standalone-1SV cmd rcvd from TM diag",0,0,0);
      lm_StandaloneTestCmd_handler( p_msg ); /* Standalone Test command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_RFDEV_TEST:
    {
      LM_MSG_MED("RF Dev Test cmd rcved from TM diag",0,0,0);
      lm_RfDevTestCmd_handler(p_msg);
      break;
    }

    case ARIES_MSG_ID_TM_LM_MEM_BW_TEST:
    {
      LM_MSG_MED("Mem BW Test cmd rcvd from TM diag",0,0,0);
      lm_MemBwTestCmd_handler( p_msg ); /* Memory bandwidth Test command handler */
      break;
    }
    #ifdef FEATURE_GNSS_LTE_CATM_COEX
    case ARIES_MSG_ID_TM_LM_LTE_CATM_TX_START:
    {
      LM_MSG_MED("GNSSCatM - LTE TX Start Test cmd rcvd from TM diag\n",0,0,0);
      lm_CatmLteTxStartCmd_handler( p_msg ); /* GNSS LTE TX StartTest command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_LTE_CATM_TX_END:
    {
      LM_MSG_MED("GNSSCatM - LTE TX End Test cmd rcvd from TM diag\n",0,0,0);
      lm_CatmLteTxEndCmd_handler( p_msg ); /* GNSS LTE TX EndTest command handler */
      break;
    }	
    #endif /* FEATURE_GNSS_LTE_CATM_COEX*/
    case ARIES_MSG_ID_TM_LM_TSG_CONFIG:
    {
      LM_MSG_MED("TSG Config cmd rcvd from TM diag",0,0,0);
      lm_DiagTsgCmd( p_msg ); /* TSG config command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_RF_CONFIG:
    {
      LM_MSG_MED("GNSS RF Config cmd rcvd from TM diag",0,0,0);
      lm_DiagGnssRfCmd( p_msg ); /* RF config command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_PRX_RF_CONFIG:
    {
      LM_MSG_MED("GNSS Prx RF Config cmd rcvd from TM diag",0,0,0);
      lm_DiagGnssPrxRfCmd( p_msg ); /* RF config command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_RF_LINEARITY_CONFIG:
    {
      LM_MSG_MED("GNSS RF Linearity Config cmd rcvd from TM diag",0,0,0);
      lm_DiagGnssRfLinearityCmd( p_msg ); /* RF Linearity config command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_SPECTRUM_ANALYZER_CMD:
    {
      LM_MSG_MED("Spectrum Analyzer cmd rcvd from TM diag",0,0,0);
      lm_spectrum_analyzer_handler( p_msg ); /* Spectrum Analyzer command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_NAV_SET_TEST_MODE_CMD:
    {
      LM_MSG_MED("Nav set test mode cmd rcvd from TM diag",0,0,0);
      lm_nav_set_test_mode_handler( p_msg ); /* Nav test set mode command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_NAV_IOCTL_CMD:
    {
      LM_MSG_MED("Nav Ioctl cmd rcvd from TM diag",0,0,0);
      lm_nav_ioctl_handler( p_msg ); /* Nav Ioctl command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_NAV_CONFIG_CMD:
    {
      LM_MSG_MED("NAV config cmd received from TM diag",0,0,0);
      lm_nav_config_handler( p_msg );
      break;
    }

    case ARIES_MSG_ID_TM_LM_BP_JAMMER_CTRL_CMD:
    {
      LM_MSG_MED("BP Jammer ctl command rcvd from TM diag",0,0,0);
      lm_DiagBPJammerCtrlCmd( p_msg );
      break;
    }

    case SM_MSG_ID_MGP_LM_REPORT_XTRA_INJ_DONE:
    { 
      lm_xtraInjDone(*((gnss_XtraInjMaskType *)(p_msg->p_Data)));
      break;
    }

    case SM_MSG_ID_LM_MGP_REPORT_XTRA_INJ_DONE:
    {
      mgp_XtraInjInd(*((gnss_XtraInjMaskType *)(p_msg->p_Data)));
      break;
    }

    case C_OS_MSG_ID_DOG_HB:
    {
      // Respond to the dog heart beat. Pat the dog. 
      os_DogHeartbeatReport();
      break;
    }
    case ARIES_MSG_ID_MGP_LM_XO_OFFSET_INFO:
    {
      lm_mgp_handle_xooffset_info((sm_ReportXOOffsetInfoStructType *)(p_msg->p_Data));
      break;
    }
    case SM_MSG_ID_LM_MGP_REPORT_XTRA_INTEGRITY:
    {
      mgp_XtraIntegrityInject((gnss_XtraIntegrityIpcStructType *)(p_msg->p_Data));
      LM_MSG_MED("LM received Xtra Integrity IPC message",0,0,0);
      break;
    }
    case SM_MSG_ID_MGP_LM_REPORT_XTRA_INTEGRITY_INJ_DONE:
    {
      gnss_XtraInjectType *pz_xtrainject = NULL;
      pz_xtrainject = (gnss_XtraInjectType *)(p_msg->p_Data);
      lm_xtraInjDone(pz_xtrainject->q_AcceptMask);
      LM_MSG_MED("LM received Xtra Integrity Inject Done IPC message",0,0,0);
      break;
    }
    default:
    {
      LM_MSG_MED("Illegal IPC message type: %d received",
                  p_msg->q_MsgId,0,0);
      break;
    }
                      
  }/* switch(p_msg->q_MsgId) ends */

}/*lm_proc() ends*/


/*=============================================================================

  FUNCTION      lm_task_stop()

  DESCRIPTION 	Exit LM task loop

  DEPENDENCIES  None

  PARAMETERS    None

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void lm_task_stop( void )
{
    e_lm_task_state = LM_TASK_STATE_STOP;
}

/*
 ******************************************************************************
 * lm_task
 *
 * Function description:
 *
 * This is the main entry function of the LM task. It will initalize IPC
 * and internal data structures and will also undergo the startup procedure with
 * TMC. It then waits infintely for incoming IPC messages.
 *
 * Parameters: 
 *
 * dummy - task input parameters(currently not used)
 *
 * Return value: 
 *
 ******************************************************************************
*/
/*lint -e715 */ 
#if 0

void lm_task( dword q_dummy )
{

/*The entry point for the task and the main task loop
  are present in this function */

  os_IpcMsgType* p_msg;
  uint8 gnss_tech =0;

  lm_startup();
  lm_task_state_init();

  lm_create_timers();
  arb_register(gnss_tech);
  e_lm_task_state = LM_TASK_STATE_RUN;
      
  while ( e_lm_task_state == LM_TASK_STATE_RUN )
  {
    
    p_msg = os_IpcReceive();

    if(p_msg == NULL)
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL message received");
    }
    else
    {
      lm_proc(p_msg);
      if(!os_IpcDelete(p_msg))
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Unable to delete received IPC message");
      }
    }

  }/*while(1) ends*/

} /*lm_task() ends*/ 
#endif




