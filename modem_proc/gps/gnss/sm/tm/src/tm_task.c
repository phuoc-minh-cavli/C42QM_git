
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM TASK

GENERAL DESCRIPTION
  This module contains TM task setup related functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2020 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/src/tm_task.c#2 $  
  $DateTime: 2020/10/16 02:37:26 $
  $Author: pwbldsvc $ 
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
15/10/20   kat     Added IPC buffer logging logic if FEATURE_CIRC_BUF_IPC is enabled
29/04/20   ska     Added changes for asking less proc duration when EPH is valid
05/12/20   ska	   Addressing KW compilation warnings 
02/10/20   ska     sensor disablement on task_stop
09/10/19   gvv     Reorganizing slim timer and message id handlers into loc middleware
05/21/19   kat     Handle plmn cell scan request 
05/30/18   sum     DogHb sequence upfate
10/25/17   rn      Handle IPC msg from GPS FS task for XTRA EFS file operation.
05/24/17   ak      Updated LPC function names. 
02/17/17   skm     BLE and Barometer support for LPPE Phase 2
01/30/17   sum     Removed the call to handler function tm_gsdi_handle_sim_card_inserted_evt() Which was only opening redundant session and tm_gsdi_handle_session_closed_evt() whic is never true
09/14/16   rn      Add ULog support for SM.
04/02/16   mj      GERA support
10/26/15   ss       Supporting Geofence Client Config indication
07/08/15   rh      Changed NV refresh to be done on one specifc sub 
04/28/14   ah      Added handling of Set Premium Services indication status
04/17/14   skm     NV Async Read changes
12/23/13   mj      Remove FEATURE_DUAL_SIM
12/06/13   ah      Support for non-deferrable timers
10/29/13   mj      Initialize WCDMA and TDSCDMA interfaces for segment loading
03/01/13   kj      Move to the DOG Heartbeat mechanism on DIME and beyond
02/28/13   mj      Update gm_register_task_init_notifier for new param and added TM_CORE_MSG_ID_API_REGISTRATION
02/21/13   mj      Adding missed code for handling lsmp param read IPC msgs
07/31/12   mjohn   Support to handle PARAM read/write IPC msgs
03/03/10   ns      DSDS Support. Use MMGSDI for DSDS
07/30/09   gk      ODP 2.0 support
01/12/09   atien   XTRA-T support
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec
04/25/08   gk      On-demand changes
07/20/07   gk      include pdapi_client.h for pdapi_init
07/17/07   lt      L1 interface.
06/14/07   gk      fixed a compile error
06/12/07   cl      Only allow PD API access when TM task is active
12/12/06   ank     Added support for TM_CORE_CM_MSG_ID_CMD_NTFY.
12/07/06   cl      Added NV support
06/13/06   cl      Initial version

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */
#include "msg.h"
#include "aries_os_api.h"
#include "sm_nv.h"
#include "pdapibuf.h"
#include "pdapiclient_lite.h"
#define TM_DATA_DECLARE
#include "tm_data.h"    /* Definition of TM task data structure */
#undef TM_DATA_DECLARE
#include "tm_diag.h"
#include "tm_xtra_t.h"
#include "tm_pdapi_client.h"
#include "tm_cm_iface.h"
#ifdef FEATURE_RUNTIME_DEVMAP
#include "rdevmap.h"
#endif /* FEATURE_RUNTIME_DEVMAP */

#include "gm_api.h"
#include "gnss_wwan_iface.h"
#include "tm_datamanager_api.h"
#include "tm_xtra.h"
#include "sm_log.h"

/* Initialize the GPS Diag Module */
extern void gpsdiag_init(void);


extern void lm_proc(os_IpcMsgType *);
extern void loc_middleware_proc(os_IpcMsgType *);
extern void loc_middleware_register_features(void);
extern void gm_proc(os_IpcMsgType *);

/*#9x05: Initialization of location middleware
  will take place from tm_task
*/
extern boolean locQmiShimInit(void);
extern void loc_middlware_module_init(void);
extern void loc_middleware_stop_handler(void);
extern void gm_module_init(void);
extern void gm_init_notify(void);
extern void gm_start_up(void);
extern void lm_task_state_init(void);
extern void lm_create_timers(void);
extern void locQmiShimHandleEvent(void);
extern void lm_dmm_marb_register(void);
extern void LocFM_HandleRegisterCb(const os_IpcMsgType* p_IpcMsg);



static volatile enum
{
  TM_TASK_STATE_RUN, TM_TASK_STATE_STOP
} e_tm_task_state;


#define TM_WAIT_GM_TASK_MASK 0x1

/*===========================================================================
FUNCTION tm_start_up

DESCRIPTION
  This function sets up IPC and creates timers.
  
RETURN VALUE

DEPENDENCIES
=============================================================================*/
void tm_start_up(void)
{
  (void)os_IpcStart((uint32)THREAD_ID_SM_TM);


  os_TaskReadyAck();
  os_TaskStopRegister();

  /* Register for DOG heartbeat monitoring */
  os_DogHeartbeatRegister();

  /* First create Dynamic PDSM buffer pools.
  */
  pdsm_pool_init();

} /* End tm_StartUp() */

/*===========================================================================
FUNCTION tm_module_init

DESCRIPTION
  This function initializes all the data structure/interfaces in TM
  
RETURN VALUE

DEPENDENCIES
=============================================================================*/
void tm_module_init(void)
{
  OS_ULOG_RESULT q_ULogInitStatus;

  /* Initialize ULog Handle for TM, last two arguments are default */
  q_ULogInitStatus = os_ULogInit(&tm_ULogHandle, "TM_ULOG", TM_ULOG_SIZE, 0, 0);
  if (q_ULogInitStatus != 0)
  {
    /* Check ULOG_ERRORS Enum for Error Code in ULOG.H */
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM ULOG Handle Init Failed with Error: %d",
          q_ULogInitStatus, 0, 0);
    ERR_FATAL("TM ULOG Handle Init Failed", 0, 0, 0);
  }
  else
  {
    OS_ULOG_PRINT_0(tm_ULogHandle, "TM ULOG Handle created successfully");
  }

  /* Initialize with CM */
  (void)tm_cm_client_init();

  /* Initialize TM-Core data structures */
  tm_core_init();

  /* Initialize PD API module */
  pdapi_lite_init();

  /* Register CGPS Diag with AMSS Diag module */
  tm_diag_init();
}

/*
 ******************************************************************************
 * tm_offline_handler
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
void tm_offline_handler(void)
{

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TASK OFFLINE received", 0, 0, 0);

  tm_core_abort_recover(TRUE, TM_STOP_REASON_TASK_OFFLINE);

  tm_core_info.tm_task_info.task_active = FALSE;
}

/*
 ******************************************************************************
 * tm_task_stop_handler
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
void tm_task_stop_handler(void)
{
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TASK STOP received", 0, 0, 0);

  tm_core_abort_recover(TRUE, TM_STOP_REASON_TASK_STOP);

  tm_core_info.tm_task_info.task_active = FALSE;

  os_TimerStop(tm_core_info.tm_delete_param_throttle_timer);

  /* Taskstop functionality will be done in LocMW after ensuring the sensors are disabled*/
  loc_middleware_stop_handler();
}

/*=============================================================================

  FUNCTION      tm_task_stop()

  DESCRIPTION   Exit TM task loop

  DEPENDENCIES  None

  PARAMETERS    None

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void tm_task_stop(void)
{
  e_tm_task_state = TM_TASK_STATE_STOP;
}

/*===========================================================================
FUNCTION tm_proc

DESCRIPTION
  
RETURN VALUE

DEPENDENCIES
=============================================================================*/
void tm_proc(void)
{
  os_IpcMsgType         *ipc_msg_ptr;

  e_tm_task_state = TM_TASK_STATE_RUN;

  while (e_tm_task_state == TM_TASK_STATE_RUN)
  {
    ipc_msg_ptr = os_IpcReceive();

    if (NULL == ipc_msg_ptr)
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL pointer rcvd from os_IpcReceive()", 0, 0, 0);
      continue;
    }
#ifdef FEATURE_CIRC_BUF_IPC 
	MSG_CIRC_BUF_IPC(ipc_msg_ptr->q_MsgId);
#endif
    switch (ipc_msg_ptr->q_MsgId)
    {
      /*Handle task maintenance messages*/
      case C_OS_MSG_ID_OFFLINE:
#if defined(FEATURE_GNSS_SA) || defined(FEATURE_GPS_LM_STANDALONE)
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM told to go OFFLINE", 0, 0, 0);
#else
        tm_offline_handler();
#endif
        break;

      case C_OS_MSG_ID_STOP:
        tm_task_stop_handler();
        break;

        /* Timer Expiry */
      case C_OS_MSG_ID_TIMER_EXPIRY:
        {
          os_TimerExpiryType *timer_param = (os_TimerExpiryType *)ipc_msg_ptr->p_Data;
          if (timer_param != NULL)
          {
            switch (timer_param->q_TimerId & TM_CORE_TIMER_DISPATCH_MASK)
            {
              case TM_LM_TIMER_ID_RANGE:
                lm_proc(ipc_msg_ptr);
                break;
              case TM_GM_TIMER_ID_RANGE:
                gm_proc(ipc_msg_ptr);
                break;
              case TM_LOCMW_TIMER_ID_RANGE:
                loc_middleware_proc(ipc_msg_ptr);
                break;
              default:
                tm_core_timer_dispatcher((void *)ipc_msg_ptr->p_Data);
                break;
            }
          }
        }
        // Both Read/Write completion handled here.
      case C_OS_MSG_ID_NV_RDRW:
        (void)sm_nv_rdrw_done();
        break;
      case C_OS_MSG_ID_QMI_EVENT:
        locQmiShimHandleEvent();
        break;
      case TM_CORE_MSG_ID_DIAG:
        tm_diag_msg_handler((void *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_SESS_REQ_API:
        tm_core_lm_sess_req_handler((void *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_SESS_UPDATE_API:
        tm_core_lm_sess_update_handler((void *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_SESS_INFO_API:
        tm_core_lm_sess_info_handler((void *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_PDAPI:
        tm_pdapi_cmd_proc((void *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_CM_MSG_ID_EVENT_NTFY:
        tm_cm_iface_event_proc((void *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_CM_MSG_ID_CMD_NTFY:
        tm_cm_iface_cmd_proc((void *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_BEST_AVAIL_POS_REPORT:
        tm_core_report_gps_state_info((void *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_XTM_CLIENT_TOKEN:
        tm_xtm_client_token_nv_write_status((void *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_XTM_SESSION_CONTROL:
        tm_xtm_session_control_nv_write_status((void *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_GM_MSG_ID_ADD_RESPONSE:
      case TM_CORE_GM_MSG_ID_DELETE_RESPONSE:
      case TM_CORE_GM_MSG_ID_PURGE_RESPONSE:
      case TM_CORE_GM_MSG_ID_BREACH_NOTIFY:
      case TM_CORE_GM_MSG_ID_POS_REQ:
      case TM_CORE_GM_MSG_ID_CLIENT_BLOB_NOTIFY:
      case TM_CORE_GM_MSG_ID_CLIENT_GEOFENCE_HANDELS_NOTIFY:
      case TM_CORE_GM_MSG_ID_GEOFENCE_HANDELS_NOTIFY:
      case TM_CORE_GM_MSG_ID_EDIT_NOTIFY:
      case TM_CORE_GM_MSG_ID_QUERY_NOTIFY:
      case TM_CORE_GM_MSG_ID_UNSOL_EVENT_NOTIFY:
      case TM_CORE_GM_MSG_ID_PRIO_POS_REQ_INDICATION_NOTIFY:
      case TM_CORE_GM_MSG_ID_SET_ENGINE_CONFIG_NOTIFY:
      case TM_CORE_GM_MSG_ID_ABORT_POS_REQ:
      case TM_CORE_GM_MSG_ID_BATCH_BREACH_NOTIFY:
      case TM_CORE_GM_MSG_ID_PROXIMITY_NOTIFY:
      case TM_CORE_GM_MSG_ID_TZ_REQ_NOTIFY:
      case TM_CORE_GM_MSG_ID_BATCH_DWELL_NOTIFY:
      case TM_CORE_GM_MSG_ID_SET_CLIENT_CONFIG_NOTIFY:
      case TM_CORE_GM_MSG_ID_CTXT_CHANGE_NOTIFY:
        tm_core_gm_evt_handler(ipc_msg_ptr->q_MsgId, (void *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MGP_WBIQ_INFO:
        tm_diag_wbiq_info_handler((sm_ReportWBIQInfoStructType *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MGP_NAV_CONFIG_CHANGE_RESP:
        tm_api_nav_config_change_resp_handler(*((uint8 *)ipc_msg_ptr->p_Data));
        break;

      case TM_CORE_MSG_ID_ENGINE_ERROR_RECOVERY_REPORT:
        (void)tm_core_handle_engine_error_recovery_report((gnss_engine_err_recovery_rpt *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_ENGINE_DPO_STATUS_REPORT:
        (void)tm_core_handle_engine_dpo_status_report((sm_DpoStatusReportStructType *)ipc_msg_ptr->p_Data);
        break;


      case C_OS_MSG_ID_DOG_HB:
        {
          // Respond to the dog heart beat. Pat the dog.
          os_DogHeartbeatReport();
		  /*Note 1:The feature registration should be done only 
		    after first dog heartbeat, as this would ensure
			that caas manager is initialized by then.
			
			Note2 :We have called a Loc_middleware function here
			because in legacy code feature registration was
			done by loc_middleware and since TM and Loc_middleware
			are in same task so heartbeat will be handled only in 
			TM. So loc_middleware function is called here.
			This may get corrected latter.
		   */
          loc_middleware_register_features();
          break;
        }

      case TM_CORE_MSG_ID_LEAP_SEC_INFO:
        tm_core_cache_leap_second_info((mgp_LeapSecondInfoStructType *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_XO_OFFSET_INFO:
        tm_diag_xo_offset_info_handler((sm_ReportXOOffsetInfoStructType *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_GPS_AS_BIT:
        tdm_HandleGpsAsBitIndication((uint8 *)ipc_msg_ptr->p_Data);
        break;
#if 0
      case TM_CORE_MSG_ID_NAV_IQ_TEST_MASK:
        tm_diag_ReportIqTestCapabilities(*((uint32 *)ipc_msg_ptr->p_Data));
        break;

      case TM_CORE_MSG_ID_RFDEV_TRACK_TEST_MASK:
        tm_diag_ReportRfDevTrackCapabilities(*((gnss_RfDevTrackConstellationMaskType *)ipc_msg_ptr->p_Data));
        break;
#endif

      case TM_CORE_MSG_ID_XTRA_EFS_GPS_FS_TASK_DONE:
        tm_core_xtra_efs_gps_fs_task_done((sm_GpsFsTaskNotificationDataType*)ipc_msg_ptr->p_Data);
        break;

	  case TM_CORE_MSG_ID_XTRA_GTS_TIME_REQUEST:
        tm_xtra_handle_gts_time_request();
        break;
#if 0
	case TM_CORE_MSG_ID_XTRA_INJECT_TO_MGP_REQUEST:
        tm_xtra_handle_inject_to_mgp_request();
        break;
#endif
    case TM_CORE_MSG_ID_DELETE_XTRA_DATA:
        tm_xtra_delete_efs_data(XTRA_VERSION_3);
        break;
    case TM_CORE_MSG_ID_DELETE_XTRA_INTEGRITY_DATA:
        tm_xtra_delete_efs_data(XTRA_VERSION_51);
        break;
    case TM_CORE_MSG_ID_UPDATE_FEATURE_STATUS:
        tm_core_handle_feature_status_update((LocFM_FeatureStatusStructT *)ipc_msg_ptr->p_Data);
        break;
    case TM_CORE_MSG_ID_LFM_REG_CB:
        LocFM_HandleRegisterCb(ipc_msg_ptr);

      case TM_CORE_MSG_ID_NMEA_DEBUG_INFO:
        tm_core_nmea_debug_info((sm_DebugNMEA *)ipc_msg_ptr->p_Data);
        break;

    case  TM_CORE_MSG_ID_TLE_PLMN_CELL_SCAN_REQ:
        tm_cm_request_plmn_search((void *)ipc_msg_ptr->p_Data);
		break;
    case TM_CORE_MSG_ID_RCVR_IN_US_STATUS:
         tm_util_update_constellation_config_if_in_US((boolean*)ipc_msg_ptr->p_Data);
        break;
	case TM_CORE_MSG_ID_EPH_EXPIRY_TIME:
		 tm_core_handle_eph_exp_time((gnss_EphExpTimeStructType *)ipc_msg_ptr->p_Data);
		 break;
      default:
#ifdef FEATURE_GEOFENCE
        if (ipc_msg_ptr->q_MsgId >= GM_CORE_ADD_GEOFENCE && ipc_msg_ptr->q_MsgId <= GM_CORE_MSG_ID_END)
        {
          gm_proc(ipc_msg_ptr);
        }
        else
#endif /* FEATURE_GEOFENCE */
          if (ipc_msg_ptr->q_MsgId >= LM_MIDDLEWARE_MSG_ID_FIRST && ipc_msg_ptr->q_MsgId <= LM_MIDDLEWARE_MSG_ID_END)
          {
            loc_middleware_proc(ipc_msg_ptr);
          }    
          /* range of LM messages encompasses non LM messages too. So either below LM message should alays be at the end or message chack shall be selective*/
          else if ((ipc_msg_ptr->q_MsgId >= LM_MSG_ID_FIRST && ipc_msg_ptr->q_MsgId <= LM_MSG_ID_END) || ipc_msg_ptr->q_MsgId == ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS_1 || ipc_msg_ptr->q_MsgId == ARIES_MSG_ID_MGP_LM_REPORT_FIX || ipc_msg_ptr->q_MsgId == ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS)
          {
            lm_proc(ipc_msg_ptr);
          }  
          else
          {
            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unknown MSG ID", 0, 0, 0);
          }
        break;
    } /* End switch MsgId */
    (void)os_IpcDelete(ipc_msg_ptr);
  } /*End while ( e_tm_task_state == TM_TASK_STATE_RUN ) */

} /* End tm_Proc() */



/*===========================================================================
FUNCTION SM_TM_TASK

DESCRIPTION
  Use pdsmsclient_task for now, change to tm_Task later
  
RETURN VALUE

DEPENDENCIES
=============================================================================*/
void sm_tm_task(
   dword dummy                     /* Rex requirement, not used */
   )
{
  (void)dummy;
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Starting TM task init...", 0, 0, 0);

  /* Set up IPC and timers */
  tm_start_up();

#ifdef FEATURE_GEOFENCE
  /*#9x05: Initialise gm module from within tm_task*/

  // gm_start_up();

  gm_module_init();

  gm_init_notify();
#endif /* FEATURE_GEOFENCE */

  /* TM module initialization */
  tm_module_init();

  /*lm module initialize*/
  lm_task_state_init();

  lm_create_timers();

#ifdef FEATURE_DYNAMIC_LOADING_GNSS
  lm_dmm_marb_register();
#endif

  /* in no modem builds, there is no pdcomms, hence we need to init gpsdiag here */
  gpsdiag_init();
  /* set global flag indicating PDAPI is initialized */

  // gm_init_notify();
  /*#9x05:Initialise loc_middleware*/
  loc_middlware_module_init();

  /*#9x05:Initialise locQmiShim*/
  locQmiShimInit();

  tm_proc();

} /* End tm_Task() */

