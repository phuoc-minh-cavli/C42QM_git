/******************************************************************************
  @file:  loc_task.c
  @brief: Location Middleware task

  DESCRIPTION
  This module handles loc middleware taks startup and processes the command queue
  for the loc mw task

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015 - 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
02/10/20   ska      sensor disablement on task_stop
09/19/19   gvv      Reorganizing Slim Timer and MessageID Handlers
08/21/19   ska      Added support for NMEA GSV
02/17/17   skm      BLE and Barometer support for LPPE Phase 2 
01/11/16   yh       LocMW refactoring
10/21/15   sudj     Fix Crash caused by bad AON configuration
06/11/15   sjk      LB 2.0 Integration
04/02/15   ssu      Batch Dwell notification for geofence.
02/08/13   ssu      Move to the DOG Heartbeat mechanism on DIME and beyond
04/12/10   ns       change loc_mw task to dynamically register with dog
04/07/10   ns       Set the loc_mw_task_started flag to indicate that
                    loc mw task is up
04/06/19   ap       9x05 QWES implementation
$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_task.c#2 $
$DateTime: 2020/07/29 06:06:03 $
$Author: pwbldsvc $

======================================================================*/


#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "task.h"
#include "msg.h"

#include "queue.h"
#include "aries_os_api.h"
#include "pdapi_lite.h"
#include "pdsm_atl.h"
#include "loc_api_2.h"

#include "loc_client.h"
#include "loc_conn.h"
#include "loc_xtra.h"
#include "loc_sm_interface.h"
#include "loc_ni.h"
#include "loc_wifi.h"
#include "loc_api_internal.h"
#include "loc_qmi_shim.h"
#include "loc_geofence.h"
#include "slim_task.h"
#include "qmislim_qmi_service_api.h"
// Mac: sdp message handler
#include "location_service_iot_v02.h"

#include "loc_aon.h"
//#include "loc_gdt.h"
#include "loc_gtp.h"
#include "loc_nv_efs.h"
#include "loc_qmi_shim.h"
#include "loc_utils.h"
#include "loc_for_gtp.h"
#include "loc_constrained_tunc_client.h"
#ifdef FEATURE_REF_TRACKER_LOC
#include "gnss_msgr.h"
#include "cm_v.h"
#include "cm_msgr_msg.h"
#endif


#ifdef __GRIFFON__
#include "mqcsi_log.h"
#endif

#ifdef FEATURE_REF_TRACKER_LOC
#define MSGR_LOCTECH           MSGR_TECH_MODULE( MSGR_TECH_GPS, THREAD_ID_LOC_MIDDLEWARE)
#endif

typedef struct {
    uint32  q_FeatureID;
    boolean v_Status;
} LocFM_FeatureStatusStructT;

static os_MutexBlockType cs_memory_record = {0};

static int highest_water_mark = 0;
#define SIZE_MEMORY_RECORD_ARRAY 64
static void * array_allocated_blocks[SIZE_MEMORY_RECORD_ARRAY] = {0};

/*
 *  e_mw_task_state -
 *      This variable is used to stop a task from running inside a Griffon build.
 *      A new API allows an external thread to change the task state from RUN to STOP.
 *      The LocMW task will poll this state variable each iteration through its
 *      task loop and will exit when it is set to "STOP".  This variable is "volatile"
 *      because it can be changed by another thread.
*/
static volatile enum { MW_TASK_STATE_RUN, MW_TASK_STATE_STOP } e_mw_task_state;

typedef boolean(*LocFM_FeatureStatusCb)
(              
  const LocFM_FeatureStatusStructT *pz_FeatureStatus
);


extern uint32 LocFM_RegisterFeature
(
  uint32                 q_FeatureID,
  LocFM_FeatureStatusCb  p_FeatureStatusCb
);
extern void locFM_Init(void);
extern uint32 locFM_getGnssServiceFeatureId(void);
extern uint32 locFM_getGtpWwanServiceFeatureId(void);

boolean loc_middleware_update_loc_feature_status_cb(const LocFM_FeatureStatusStructT *pz_FeatureStatus);

#ifdef FEATURE_REF_TRACKER_LOC
void loc_middleware_send_psm_entry_rsp(void);
boolean loc_middleware_handle_psm_entry_indication(void);
boolean loc_geofence_sensor_stop_amd(void);
boolean loc_geofence_sensor_stop_smd(void);
#endif

static void flush_memory_records (void)
{
    LOC_MW_ENTER_CRIT_SECTION (&cs_memory_record);
    {
        int i = 0;
        for(i = 0; i < SIZE_MEMORY_RECORD_ARRAY; i++)
        {
            array_allocated_blocks[i] = NULL;
        }
    }
    highest_water_mark = 0;
    LOC_MW_LEAVE_CRIT_SECTION (&cs_memory_record);
}

static void memory_watermark_check (void)
{
    int count = 0;
    int highest_copy = 0;
    LOC_MW_ENTER_CRIT_SECTION (&cs_memory_record);
    {
        int i = 0;
        for(i = 0; i < SIZE_MEMORY_RECORD_ARRAY; i++)
        {
            if(NULL != array_allocated_blocks[i])
            {
                ++count;
            }
        }
        if( count > highest_water_mark)
        {
            highest_water_mark = count;
        }
        highest_copy = highest_water_mark;
    }
    LOC_MW_LEAVE_CRIT_SECTION (&cs_memory_record);
    LOC_MSG_HIGH ( "memory_watermark_check: %d memory blocks allocated, highest %d", count, highest_copy, 0 );
}

void * loc_calloc (size_t block_size)
{
    void * ptr_return = NULL;
    int cause = 0;

    if(block_size > 0)
    {
        cause = 1;
        LOC_MW_ENTER_CRIT_SECTION (&cs_memory_record);
        {
            boolean fg_continue = TRUE;
            int i = 0;
            for(i = 0; (i < SIZE_MEMORY_RECORD_ARRAY) && (TRUE == fg_continue); i++)
            {
                if(NULL == array_allocated_blocks[i])
                {
                    cause = 2;
                    // asked to switch to a specific heap manager
                    ptr_return = os_MemAlloc (block_size, OS_MEM_SCOPE_TASK);
                    if(NULL != ptr_return)
                    {
                        memset(ptr_return, 0, block_size);
                    }
                    array_allocated_blocks[i] = ptr_return;
                    fg_continue = FALSE;
                }
            }
        }
        LOC_MW_LEAVE_CRIT_SECTION (&cs_memory_record);
    }
    else
    {
        cause = 4;
    }

    if(NULL == ptr_return)
    {
        LOC_MSG_ERROR ( "loc_calloc: %d bytes, failed, cause %d", block_size, cause, 0 );
    }
    else
    {
        LOC_MSG_HIGH ( "loc_calloc: %d bytes, 0x%p returned.", block_size, ptr_return, 0 );
    }

    return ptr_return;
}

void loc_free (void * ptr)
{
    int cause = 0;
    if(NULL != ptr)
    {
        cause = 1;
        LOC_MW_ENTER_CRIT_SECTION (&cs_memory_record);
        {
            int i = 0;
            for(i = 0; i < SIZE_MEMORY_RECORD_ARRAY; i++)
            {
                if(ptr == array_allocated_blocks[i])
                {
                    void * ptr_for_modify = (void *)ptr;
                    cause = 2;
                    // asked to switch to a specific heap manager
                    // os_MemFree would set the pointer to NULL
                    os_MemFree(&ptr_for_modify);
                    array_allocated_blocks[i] = NULL;
                }
            }
        }
        LOC_MW_LEAVE_CRIT_SECTION (&cs_memory_record);
    }
    else
    {
        cause = 4;
    }

    if(2 != cause)
    {
        LOC_MSG_ERROR ( "loc_free: 0x%p failed, cause %d", ptr, cause, 0 );
    }
    else
    {
        LOC_MSG_HIGH ( "loc_free: 0x%p freed okay", ptr, 0, 0 );
    }
}

boolean loc_middleware_initialized (void)
{

    if(FALSE == loc_middleware_data.loc_mw_task_started)
    {
       // failed
       LOC_MSG_ERROR ( "loc_middleware is not initialized: FAIL", 0,0,0 );
    }
    else
    {
       LOC_MSG_HIGH ( "loc_middleware is initialized", 0,0,0 );
    }

    return loc_middleware_data.loc_mw_task_started;
}

static void loc_middleware_start_up(void)
{
  (void)os_IpcStart((uint32)THREAD_ID_LOC_MIDDLEWARE);

} /* End tm_StartUp() */

void loc_middleware_offline_handler( void )
{

  LOC_MSG_HIGH("TASK OFFLINE received",0,0,0);

}

void loc_mw_process_task_stop_after_sensors_disabled(void)
{
   /* Deregister with the Dog Heartbeat before stopping task */
   os_DogHeartbeatDeregister();

   os_TaskStopAck();
}

/*
 ******************************************************************************
 * loc_middleware_task_stop_handler
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
void loc_middleware_stop_handler( void )
{
  LOC_MSG_HIGH("TASK STOP received",0,0,0);
  #ifdef FEATURE_REF_TRACKER_LOC
    slim_QSQmi_disable_sensor(loc_mw_process_task_stop_after_sensors_disabled);
  #else
       /* Deregister with the Dog Heartbeat before stopping task */
   os_DogHeartbeatDeregister();

   os_TaskStopAck();
  #endif
}


/*===========================================================================
FUNCTION loc_middleware_proc

DESCRIPTION

RETURN VALUE

DEPENDENCIES
=============================================================================*/
void loc_middleware_proc (os_IpcMsgType* p_msg)
{
   // lm_task calls this function whenever it recieves a message
   if( p_msg == NULL )
   {
      LOC_MSG_ERROR("Received NULL IPC Message ",0,0,0);
      return;
   }

   LOC_MSG_MED("Received IPC Message with id %d. Loc enabled %d",p_msg->q_MsgId, loc_middleware_data.qmi_loc_enabled, 0);

   switch (p_msg->q_MsgId)
   {
   /*Handle task maintenance messages*/
   case C_OS_MSG_ID_OFFLINE:
      loc_middleware_offline_handler();
      break;

   case C_OS_MSG_ID_STOP:
      loc_middleware_stop_handler();
      break;

   case C_OS_MSG_ID_DOG_HB:
   {
     // Respond to the dog heart beat. Pat the dog.
     os_DogHeartbeatReport();
     break;
   }
   /* Handle timer expiry messages*/
   case C_OS_MSG_ID_TIMER_EXPIRY:
   {
      os_TimerExpiryType z_TimerMsg;
      memscpy(&z_TimerMsg, sizeof(z_TimerMsg),
        (void*)&p_msg->p_Data[0], p_msg->q_Size);

      /*lint -e826 */
      switch(z_TimerMsg.q_TimerId)
      {
        case LOC_MIDDLEWARE_TIMER_ID_FIX:
        {
           loc_pd_process_fix_request(eLOC_PD_FIX_TIMEOUT);
           break;
        }
        case LOC_MIDDLEWARE_TIMER_ID_XTRA:
        {
           locXtra_ProcessTimerCb();
           break;
        }
        case LOC_MIDDLEWARE_TIMER_ID_PA:
        {
           locPa_ProcessTimerCb();
           break;
        }
        case C_OS_MSG_ID_DOG_HB:
        {
          // Respond to the dog heart beat. Pat the dog.
          os_DogHeartbeatReport();
          break;
        }
        default:
        {
          if (LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_MIN <= z_TimerMsg.q_TimerId &&
             LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_MAX >= z_TimerMsg.q_TimerId)
          {
            locAonTimerHandler(z_TimerMsg.q_TimerId, z_TimerMsg.q_Param);
          }
#ifdef FEATURE_REF_TRACKER_LOC
          else if (LOC_MIDDLEWARE_TIMER_ID_SLIM_MIN <= z_TimerMsg.q_TimerId &&
              LOC_MIDDLEWARE_TIMER_ID_SLIM_MAX >= z_TimerMsg.q_TimerId)
          {
            LOC_MSG_HIGH_1("Calling slim_TaskTimerHandler timer id = %d", z_TimerMsg.q_TimerId);
            slim_TaskTimerHandler(z_TimerMsg.q_TimerId, z_TimerMsg.q_Param);
          }
#endif

          break;
        }
      }
      break;
   }

   case LM_MIDDLEWARE_MSG_ID_PDAPI:
   	    (void)locMW_HandleLocCmd(p_msg);
        break;

   /*#9x05: Call to all functions in loc_sdp.c removed 
     for lean build.
   */

   case LM_MIDDLEWARE_MSG_ID_GEOFENCE_GEN_ALERT_IND :
       locGeofenceHandleGenAlertInd(p_msg);
       break;

   case LM_MIDDLEWARE_MSG_ID_GEOFENCE_BREACH_IND :
       locGeofenceHandleBreachInd(p_msg);
       break;

   case LM_MIDDLEWARE_MSG_ID_GEOFENCE_ADD_IND :
       locGeofenceHandleAddInd(p_msg);
       break;

   case LM_MIDDLEWARE_MSG_ID_GEOFENCE_DELETE_IND :
       locGeofenceHandleDeleteInd(p_msg);
        break;




   /*9x05: LM_MIDDLEWARE_MSG_ID_PEDOMTER_REPORT_IND	not supported*/

    /*9x05: LM_MIDDLEWARE_MSG_ID_INJECT_MOTION_DATA_IND  not supported*/

   case LM_MIDDLEWARE_MSG_ID_GEOFENCE_SET_ENGINE_CONFIG_IND:
       locGeofenceHandleSetEngineConfigInd(p_msg);
       break;

   /*9x05: LM_MIDDLEWARE_MSG_ID_SDP_INJECT_VEHICLE_SENSOR_DATA_IND not supported*/

   /*#9x05: locGeofenceHandleBatchBreachInd not supported*/

   /*9x05: Wifi not supported*/



   case LM_MIDDLEWARE_MSG_ID_GTP_CLIENT_DLD_DATA_IND:
       (void) loc_gtp_WwanClientDloadedDataIndHandler(p_msg);
       break;

   
   /*#9x05: loc_gdt not supported. Removing all calls to functions in loc_gdt*/




   /*#9x05: loc_wle not supported. Removing all calls to functions in loc_wle*/

   case LM_MIDDLEWARE_MSG_ID_QMI_LOC_MW_IND:
      locMW_HandleQmiIndication(p_msg);
       break;

   case LM_MIDDLEWARE_MSG_ID_QMI_LOC_PA_IND:
       locPa_SendQmiIndication(p_msg);
       break;

   /*#9x05: Call to all functions in loc_gdt.c removed 
     for lean build.
   */

   /*#9x05: Call to all functions in loc_csm.c removed 
     for lean build.
   */

   case LM_MIDDLEWARE_MSG_ID_SEND_FDCL_SERVICE_REQ_IND:
       loc_tle_sendFDCLServiceReqInd(p_msg);
       break;

   case LM_MIDDLEWARE_MSG_ID_SEND_GET_FDCL_BS_LIST_IND:
       loc_tle_sendGetFdclBsListInd(p_msg);
       break;

   case LM_MIDDLEWARE_MSG_ID_SEND_INJECT_FDCL_DATA_IND:
       loc_tle_sendInjectFdclDataInd(p_msg);
       break;

   case LM_MIDDLEWARE_MSG_ID_SEND_FDCL2_SERVICE_REQ_IND:
       loc_tle_Fdcl2_SendBsObsDataServiceReqInd(p_msg);
     break;

   case LM_MIDDLEWARE_MSG_ID_SEND_GET_FDCL2_BS_OBS_DATA_IND:
       loc_tle_Fdcl2_sendGetBsObsDataInd(p_msg);
     break;
   
   case LM_MIDDLEWARE_MSG_ID_QUERY_GNSS_ENERGY_CONSUMED:
       loc_mw_handle_query_gnss_energy_consumed(*((loc_client_handle_type*) p_msg->p_Data));
     break;

#ifdef FEATURE_REF_TRACKER_LOC
   case LM_MIDDLEWARE_MSG_ID_PSM_ENTRY_IND:
        loc_middleware_handle_psm_entry_indication();
        break;
#endif

   case LM_MIDDLEWARE_MSG_ID_OPPORTUNISTIC_SCHEDULE_IND:
        
        if(loc_middleware_data.z_locPd_Data.v_any_victimized_client)
        { /*Only schedule a sesion immediately, if in past some client didnt get fix from MGP*/
          /*Also vitimized flag will never be set for single shot client, only for apptrack client */
          loc_pd_process_fix_request (eLOC_PD_OPPORTUNISTIC_SCHEDULE);
        }
     break;

   case LM_MIDDLEWARE_MSG_ID_SEND_GET_WWAN_DATA_IND:
        loc_tle_sendGetWwanDataInd(p_msg);
     break;
   
   default:
       /* Forward IPC messages targeted to SLIM */
      /*#9x05: Call to all slim functions  removed 
        for lean build.
       */


       /* Forward IPC messages targeted to AON */
       if(((p_msg->q_MsgId >= LM_MIDDLEWARE_MSG_ID_AON_MIN &&
             p_msg->q_MsgId <= LM_MIDDLEWARE_MSG_ID_AON_MAX)) ||
           ((p_msg->q_MsgId == LM_MIDDLEWARE_MSG_ID_AON_STATUS_CB   ||
             p_msg->q_MsgId == LM_MIDDLEWARE_MSG_ID_AON_LOCATION_CB)))
       {
          locAonIpcHandler(p_msg);
       }
#ifdef FEATURE_REF_TRACKER_LOC
       else if ((p_msg->q_MsgId >=  LM_MIDDLEWARE_MSG_ID_SLIM_MIN) &&
                (p_msg->q_MsgId <=  LM_MIDDLEWARE_MSG_ID_SLIM_MAX) )
       {
         MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Calling slim_TaskIpcHandler for msg id = %d", p_msg->q_MsgId, 0, 0);
         slim_TaskIpcHandler(p_msg);
       }
#endif     

       break;
   }

   memory_watermark_check();
}

void loc_middlware_module_init(void)
{

   // Zero initialize the loc_middleware data
   memset ((void*)&(loc_middleware_data), 0, sizeof (loc_middleware_data));
   loc_middleware_data.z_locClnt_Data.l_ActiveClientHandle = LOC_CLIENT_HANDLE_INVALID;

   loc_middleware_data.qmi_loc_enabled          = FALSE;
   loc_middleware_data.pd_module_initialized    = FALSE;
   loc_middleware_data.pa_module_initialized    = FALSE;
   loc_middleware_data.xtra_module_initialized  = FALSE;
   loc_middleware_data.conn_module_initialized  = FALSE;
   loc_middleware_data.ni_module_initialized    = FALSE;
   loc_middleware_data.geofence_module_initialized  = FALSE;
   loc_middleware_data.aon_module_initialized  = FALSE;
   loc_middleware_data.pdapi_client_id          = -1;
   loc_middleware_data.xtra_client_id           = -1;

   LOC_MW_INIT_CRIT_SECTION (&loc_middleware_data.crit_sec);
   LOC_MW_INIT_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
   LOC_MW_INIT_CRIT_SECTION (&loc_middleware_data.xtra_crit_sec);
   LOC_MW_INIT_CRIT_SECTION (&loc_middleware_data.client_crit_sec);

   LOC_MW_INIT_CRIT_SECTION (&cs_memory_record);
   flush_memory_records();


   //indicate that the loc middleware task is up
   loc_middleware_data.loc_mw_task_started = TRUE;
   locFM_Init();

#ifdef FEATURE_REF_TRACKER_LOC
  slim_Init();
#endif

}

/*=============================================================================

  FUNCTION      loc_middleware_task_stop()

  DESCRIPTION   Exit loc_mw task loop

  DEPENDENCIES  None

  PARAMETERS    None

  RETURN VALUE  None

  SIDE EFFECTS  Calling this API will cause the LocMW task to "exit"

=============================================================================*/
void loc_middleware_task_stop( void )
{
    e_mw_task_state = MW_TASK_STATE_STOP;
}

/*=============================================================================

  FUNCTION      loc_middleware_update_loc_feature_status_cb()

  DESCRIPTION   This API injects feature status update to TM.
 

  DEPENDENCIES  

  PARAMETERS
    pz_FeatureStatus - Points to the location feature to be managed
    (input)                                    

 
  RETURN VALUE  
    TRUE  - The request is sent to PE successfuly
    FALSE - Otherwise
 
  SIDE EFFECTS  

=============================================================================*/
boolean loc_middleware_update_loc_feature_status_cb(const LocFM_FeatureStatusStructT *pz_FeatureStatus)
{
  if(NULL != pz_FeatureStatus )
  {
    os_IpcMsgType*       p_IpcMsg = NULL;
    p_IpcMsg = os_IpcCreate(sizeof(*pz_FeatureStatus), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);
    if(NULL == p_IpcMsg)
    {
      LOC_MSG_ERROR("loc_middleware_update_loc_feature_status_cb: os_IpcCreate failed", 0, 0, 0);
      return FALSE;
    }
    p_IpcMsg->q_MsgId = TM_CORE_MSG_ID_UPDATE_FEATURE_STATUS;

    memscpy(p_IpcMsg->p_Data,sizeof(*pz_FeatureStatus), pz_FeatureStatus, sizeof(*pz_FeatureStatus));
    if(FALSE == os_IpcSend(p_IpcMsg, THREAD_ID_LOC_MIDDLEWARE))
    {
      LOC_MSG_ERROR("loc_middleware_update_loc_feature_status_cb: Sending IPC to TM failed", 0, 0, 0);
      os_IpcDelete(p_IpcMsg);
      return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION loc_middleware_register_features

DESCRIPTION: This function register features with loc 
             feature manager


RETURN VALUE

DEPENDENCIES
=============================================================================*/

void loc_middleware_register_features(void)
{
   LocFM_RegisterFeature(locFM_getGnssServiceFeatureId(), loc_middleware_update_loc_feature_status_cb);
   LocFM_RegisterFeature(locFM_getGtpWwanServiceFeatureId(), loc_middleware_update_loc_feature_status_cb);
}

#ifdef FEATURE_REF_TRACKER_LOC

/*===========================================================================
FUNCTION loc_middleware_psm_entry_notfication

DESCRIPTION: This function is a callback which PSM module will call when it want to enter PSM mode. LocTech must respond with MM_CM_PSM_ENTER_RSP 
       after PSM entry procedure is completed

RETURN VALUE

DEPENDENCIES
=============================================================================*/

boolean loc_middleware_handle_psm_entry_indication(void)
{
  slim_QSQmi_disable_sensor(loc_middleware_send_psm_entry_rsp);
  return (TRUE);
}

/*===========================================================================
FUNCTION loc_middleware_send_psm_entry_rsp

DESCRIPTION: This function is called after PSM entry procedures are completed by loctech.

RETURN VALUE

DEPENDENCIES
=============================================================================*/

void loc_middleware_send_psm_entry_rsp(void)
{
  cm_client_psm_enter_rsp_s_type  psm_enter_rsp;
  LOC_MSG_HIGH("Sending msgr message Message MM_CM_PSM_ENTER_RSP",0, 0, 0);
  memset((void *)&psm_enter_rsp, 0x0, sizeof(cm_client_psm_enter_rsp_s_type)); 
  
  msgr_init_hdr((msgr_hdr_s *)&psm_enter_rsp.msg_hdr,
                MSGR_LOCTECH, 
                MM_CM_PSM_ENTER_RSP); 
  psm_enter_rsp.module = CM_PSM_MODULE_LOCTECH;
  if(msgr_send((msgr_hdr_s *)&psm_enter_rsp,sizeof(cm_client_psm_enter_rsp_s_type))!= E_SUCCESS)
  {
    ERR_FATAL("Failed to send umid MM_CM_PSM_ENTER_RSP %d",(msgr_hdr_s *)&psm_enter_rsp.msg_hdr.id ,0,0 );
  } 
}

#endif /* FEATURE_REF_TRACKER_LOC */

/*===========================================================================
FUNCTION LOC_MIDDLWARE_TASK

DESCRIPTION


RETURN VALUE

DEPENDENCIES
=============================================================================*/
#if 0

void loc_middleware_task
(
  dword dummy                     /* Rex requirement, not used */
)
{

   os_IpcMsgType* p_msg;

   (void)dummy;

   LOC_MSG_HIGH("Starting loc middleware task ...",0,0,0);

   /* location middleware module initialization */
   loc_middlware_module_init();

   /* Set up IPC and timers */
   loc_middleware_start_up();

   /* Register for DOG heartbeat monitoring. Make sure we do this after the IPC is set up . */
   os_DogHeartbeatRegister();

   os_TaskReadyAck();
   os_TaskStopRegister();

   /*
    * Initialize the MQSCI module on Griffon only
    * The mqcsi_powerup_init() function is handled by the MPROC team
    * in QCT modem SW builds, but the GPS team is responsible in Griffon (SOC) builds
    */
#ifdef __GRIFFON__
   mqcsi_powerup_init();
#endif

   /* Initialize QMI data structures. Dont register QMI-Loc with QMI framework
    * yet. Resgiter with SM a callback for PDAPI initialization notification */
   locQmiShimInit();
 

   e_mw_task_state = MW_TASK_STATE_RUN;

   while ( e_mw_task_state == MW_TASK_STATE_RUN )
   {
      p_msg = os_IpcReceive();

      if(p_msg == NULL)
      {
         LOC_MSG_ERROR("NULL message received",0,0,0);
      }
      else
      {
         loc_middleware_proc (p_msg);
         if(!os_IpcDelete(p_msg))
         {
            LOC_MSG_ERROR("Unable to delete received IPC message",0,0,0);
         }
      }
   }    /* while(task in the RUN state)*/

} /* End tm_Task() */
#endif

