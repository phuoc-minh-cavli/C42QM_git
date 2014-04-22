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

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/lm/src/lm_dmm.c#2 $
  $DateTime: 2020/09/09 05:58:16 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/32/20   sai    added changes to not start MARB min wait timer during GNSS Unload
02/26/20   katta  Changed the info passed to LM from marb cb
02/18/20   ska     Reducing proc_duration when last fix EPH is valid
08/21/19   py    Ignore Load over LOAD request to DMM. Removed delibrate crash.
05/23/19   vg      Implement system unc specific session duration
04/04/19   nath    Cache CPI when MGP is not LOADED.
12/30/18   ap      Added code for DMM feature support.
======================================================================*/
#include "tm_api.h"
#include "lm_dmm.h"
#include "lm_data.h"
#include "tm_lm_iface.h"
#include "lm_mgp.h"
#include "aries_os_api.h"
#include "lm_tm.h"
#include "fs_public.h"

//#define TEST_UNLOAD_TRIGGER_FROM_INTERNAL_CODE_LOCATIONS

#ifdef FEATURE_DYNAMIC_LOADING_GNSS
uint8 lm_unload_mask = 255;
Arb_ResultStructType                  z_lm_dmm_arb_payload;
static DalDeviceHandle                *lm_dmm_dal_handle;
#endif
static lm_dmm_context_info_s_type     z_lm_dmm_context_info;

void lm_dmm_proceed_to_unload();

#ifdef FEATURE_DYNAMIC_LOADING_GNSS
#ifdef TEST_UNLOAD_TRIGGER_FROM_INTERNAL_CODE_LOCATIONS
/*
 ******************************************************************************
 * trigger_gnss_unload
 *
 * Function description:
 *
 *  This function triggeres MARB_UNLOAD_CB as if it is being called from MARB/WWAN. 
 *  Using this function we can inject unload in many possible sceanrio which otherwise 
 *  may be difficult because of unpredictablity of unload trigger coming from MARB/WWAN.
 *   
 *
 * Parameters: event. this is used control the trigger. 
 * Only when event passed matches the efs value, unload is triggered. Only exception is when unload is triggred from qxdm.
 * when unload is triggered from qxdm (event =99), then efs value is do not care.
 *
 * Return value: 
 ******************************************************************************
*/
void trigger_gnss_unload(uint8 event)
{
  marb_async_callback_input_t input;
  //#ifdef FEATURE_CGPS_FILE_SYSTEM
  int         fd;
  int  q_BytesRead;
  static boolean nv_read = FALSE;
  static boolean ignore_trigger_count = TRUE;
  static int16 trigger_count = 0;
  
  if (nv_read == FALSE)
  {
    fd = efs_open( FILEPATH_GNSS_UNLOAD, O_RDONLY );

    if( fd < 0 )
    {
      /* set default */
      lm_unload_mask = 255;
      LM_MSG_HIGH("LM_DMM:nv read failed ..no internal unload supported except through diag command", 0, 0, 0);
      
    }
    else
    {
      q_BytesRead = efs_read( fd, &lm_unload_mask, sizeof(lm_unload_mask) );
      (void) efs_close( fd );
      LM_MSG_HIGH("LM_DMM:NV read succeeded lm_unload_mask = %d", lm_unload_mask, 0, 0);
    }
    nv_read=TRUE;
  }
//#endif
  LM_MSG_HIGH("LM_DMM:trigger gnss_marb_cb to unload event = %d, lm_unload_mask = %d, trigger_count = %d", event, lm_unload_mask, trigger_count);

  if (lm_unload_mask >= 100)
  {
    lm_unload_mask = lm_unload_mask % 100;
    ignore_trigger_count = FALSE;
  }
  
  input.client = MARB_CLIENT_GPS;
  input.cb_type = MARB_UNLOAD_CB;
  input.cb_data.unload.wait_times.min_wait = 4*19200000;
  input.cb_data.unload.wait_times.load_wait = 19200000;
  
  if (((lm_unload_mask == event) && ((ignore_trigger_count == TRUE)||(trigger_count > 0))) ||(event ==99))
  {
    lm_dmm_marb_cb(&input);
    
  }
  else
  {
    LM_MSG_MED("LM_DMM:lm_unload_mask is not set as per event %d or trigger_count %d...can not trigger unload", event , trigger_count, 0);
  }
  if (lm_unload_mask == event)
  {
    trigger_count++;
  }
}
#endif /*TEST_UNLOAD_TRIGGER_FROM_INTERNAL_CODE_LOCATIONS */


boolean lm_dmm_is_marb_req_needed(void)
{
   /*If the receiver state is off it implies that LM 
     has already informed marb about GNSS inactivity
     and so it should inform marb that GNSS is going 
     active again before trying to turn ON receiver
   */
   if(LM_MGP_STATE_OFF == lm_mgp_get_gnns_rcvr_state())
   {
     return TRUE;
   }
   else
   {
     return FALSE;
   }
}
marb_procedure_t lm_dmm_get_marb_procedure_type(void)
{
   if(LM_MGP_LOAD_FOR_SARF_TEST == lm_mgp_get_mgp_load_reason())
   {
      return MARB_PROC_HIGH;
   }
   else
   {
     return MARB_PROC_ACTIVE;
   }
}


void lm_dmm_marb_unload(uint64 min_wait, uint64 load_wait)
{
  z_lm_dmm_arb_payload.min_wait = min_wait;
  z_lm_dmm_arb_payload.load_wait = load_wait;
}

void lm_dmm_dal_init(void)
{
  DALResult result = DalTimetick_Attach("SystemTimer", &lm_dmm_dal_handle);
  ASSERT(result == DAL_SUCCESS);
}

uint64 lm_dmm_timeticks_to_ms(uint64 tt)
{
  uint64 return_ms;
  if (NULL == lm_dmm_dal_handle)
  {
    lm_dmm_dal_init();
  }
  DalTimetick_CvtFromTimetick64(lm_dmm_dal_handle, tt, T_MSEC, &return_ms);
  return return_ms;
}

uint64 lm_dmm_ms_to_timeticks(uint64 ms)
{
  uint64 return_tt;
  if (NULL == lm_dmm_dal_handle)
  {
    lm_dmm_dal_init();
  }
  DalTimetick_CvtToTimetick64(lm_dmm_dal_handle, ms, T_MSEC, &return_tt);
  return return_tt;
}

/*
 ******************************************************************************
 * lm_dmm_marb_deregister
 *
 * Function description:
 *
 *  DeRegisters LM DMM as a client for MARB
 *   
 *
 * Parameters: 
 *
 * Return value: 
 ******************************************************************************
*/
void lm_dmm_marb_deregister(void)
{
  return;
}

/*
 ******************************************************************************
 * lm_dmm_marb_deregister
 *
 * Function description:
 *
 *  Registers LM DMM as a client for MARB
 *   
 *
 * Parameters: 
 *
 * Return value: 
 ******************************************************************************
*/

void lm_dmm_marb_register(void)
{
/* REV 12/04:Variable naming as per coding guideline*/
  boolean status;
  marb_register_input_t params;
 
#ifdef FEATURE_CGPS_FILE_SYSTEM
  int         fd;
  int  q_BytesRead;
  /*REV 12/04: Is this the right way to call efs_read same for efs_read and close*/
  /*REV_12/04: Chnage name FILEPATH_GNSS_UNLOAD as per coding guide lines  */
  fd = efs_open( FILEPATH_GNSS_UNLOAD, O_RDONLY );

  if( fd < 0 )
  {
    /* set default */
    lm_unload_mask = 255;
  }
  else
  {
    q_BytesRead = efs_read( fd, &lm_unload_mask, sizeof(lm_unload_mask) );
    (void) efs_close( fd );
  }
#endif
  params.cb_bitmask = (1 << MARB_EVENT_NOTIFY_CB); /* rest  of the two events are default and bitmask is not required for those */
  params.cb_ptr = lm_dmm_marb_cb;
 
  status = marb_register(MARB_CLIENT_GPS, MARB_CLI_GROUP_GNSS, params);
 
  if (status != TRUE)
  {
    ERR_FATAL("MARB register failed",0,0,0);
  }

  return;
}


/*
 ******************************************************************************
 * lm_dmm_marb_req
 *
 * Function description:
 *
 *  Requests MARB to load MGP
 *   
 *
 * Parameters: 
 *    
 *   duration for loading is required,
 *   time for which cleanup after unload
 * Return value: 
 *   Arb_ResultStructType : return result of the req
 ******************************************************************************
*/

Arb_ResultStructType lm_dmm_marb_req(marb_procedure_t proc)
{
 /*REV 12/04: Use proper naming convention*/
  marb_result_enum_t      result;
  marb_request_input_t    input;
  marb_wait_time_output_t output;
  Arb_ResultStructType z_result;
  uint64 q_proc_duration;

  switch (lm_get_sys_status_info())
  {
    case C_SYS_STATUS_VALID_NONE:
    {
      q_proc_duration = LM_DMM_EXPECTED_SESS_DURATION_SYS_STATUS_VALID_NONE;
      break;
    }
    case C_SYS_STATUS_VALID_ONLY_XTRA:
    {
      q_proc_duration = LM_DMM_EXPECTED_SESS_DURATION_SYS_STATUS_VALID_ONLY_XTRA;
      break;
    }
    case C_SYS_STATUS_VALID_ONLY_EPH:
    {
      q_proc_duration = LM_DMM_EXPECTED_SESS_DURATION_SYS_STATUS_VALID_EPH;
      break;
    }
    default:
    {
      q_proc_duration = z_lm_dmm_context_info.u_gnss_proc_duration;
      break;
    }
  }
  
  input.procedure = proc;
  input.duration  = lm_dmm_ms_to_timeticks(q_proc_duration);
  input.cleanup_duration = lm_dmm_ms_to_timeticks(z_lm_dmm_context_info.u_gnss_cleanup_time);
  
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"marb_debug: MARB request (proc=%d, duration in msec=%d, cleanup_duration in msec =%d)",
    input.procedure, q_proc_duration, z_lm_dmm_context_info.u_gnss_cleanup_time);

  result = marb_request(MARB_CLIENT_GPS, &input, &output);
  

  z_result.e_result = result;
  z_result.load_wait = lm_dmm_timeticks_to_ms(output.load_wait);
  z_result.min_wait= lm_dmm_timeticks_to_ms(output.min_wait);
  
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"marb_debug: MARB request results (load_wait=%d, min_wait=%d, result=%d)",
    z_result.load_wait, z_result.min_wait, z_result.e_result);

  return (z_result);
}

/*
 ******************************************************************************
 * lm_dmm_mgp_load_backoff_timer_timeout
 *
 * Function description:
 *
 *  Handles load backoff timer time out
 * 
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_mgp_load_backoff_timer_timeout(void)
{
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"LM_DMM:min_wait_timer expired ",0 ,0 ,0);
  os_TimerStop(z_lm_dmm_context_info.p_MgpLoadAttemptBackOffTimer); /*lint !e534 */
  return;
}

/*
 ******************************************************************************
 * lm_dmm_load_timeout
 *
 * Function description:
 *
 *  Handles load timer timeout
 * 
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_load_timeout(void)
{
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"LM_DMM:load_timer expired ",0 ,0 ,0);
  (void)lm_dmm_state_machine(LM_DMM_EVENT_ABORT_MGP_LOAD_IND);
  return;
}

/*
 ******************************************************************************
 * lm_dmm_unlLoad_timeout
 *
 * Function description:
 *
 *  Handles unload timer timeout
 * 
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_unlLoad_timeout(void)
{
  ERR_FATAL("LM_DMM: Forced crash as MGP did not respond to deinit request",0 ,0 ,0);
  (void)lm_dmm_state_machine(LM_DMM_EVENT_MGP_UNLOAD_FAIL_IND);
  return;
}

/*
 ******************************************************************************
 * lm_dmm_marb_indication
 *
 * Function description:
 *
 *  Sends out indication to MARB
 * 
 *
 * Parameters: 
 *   marb_mem_ind_enum_t indication
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_marb_indication(marb_mem_ind_enum_t indication)
{
  marb_indication_input_t input;

  input.indication = indication;
  LM_MSG_HIGH_1("marb_debug: MARB indication (indication=%d)", indication);
  marb_indication(MARB_CLIENT_GPS, &input);
}

/*
 ******************************************************************************
 * lm_dmm_marb_cb
 *
 * Function description:
 *
 *  Callback called by MARB to provide inputs to DMM 
 * 
 *
 * Parameters: 
 *   marb_async_callback_input_t *input
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_marb_cb(marb_async_callback_input_t *input)
{
  if (input && input->client == MARB_CLIENT_GPS)
  {
    switch (input->cb_type)
    {
      case MARB_EVENT_NOTIFY_CB:
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"MARB notify rcvd (mem_ind=%d, actv_grp=%d)", 
          input->cb_data.notify.mem_indication, input->cb_data.notify.active_group,0);
        /* no need for mem_indication check or active gorup check. this event means laoded tech 
                  has priority reduced so restriction from GNSS can be lifted*/
        lm_dmm_send_ipc_to_lm(input, sizeof(*input));
        break;
      }

      case MARB_ACCEPT_CB:
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"MARB accept rcvd (result=%d)", input->cb_data.accept.result,0,0);
        if (input->cb_data.accept.result == MARB_RESULT_ACCEPT)
        {
          lm_dmm_send_ipc_to_lm(input, sizeof(*input));
        }
        else
        {
          ERR_FATAL("MARB unknown accept",0,0,0);
        }
        break;
      }

      case MARB_UNLOAD_CB:
      {
        uint64 min_wait, load_wait;

        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"MARB unload rcvd (min_wait=%d, load_wait=%d)", 
        input->cb_data.unload.wait_times.min_wait, input->cb_data.unload.wait_times.load_wait,0);
        min_wait  = lm_dmm_timeticks_to_ms(input->cb_data.unload.wait_times.min_wait);
        load_wait = lm_dmm_timeticks_to_ms(input->cb_data.unload.wait_times.load_wait);
        (void)lm_dmm_marb_unload(min_wait, load_wait);
		lm_dmm_send_ipc_to_lm(input, sizeof(*input));
        break;
      }

      default:
        ERR_FATAL("MARB unknown cb_type=%d", input->cb_type,0,0);
    }
  }
}
#endif

/*
 ******************************************************************************
 * lm_dmm_initialize_mgp
 *
 * Function description:
 *
 *  Initializes MGP after it is loaded
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_initialize_mgp()
{
  uint32 q_constel_config = 0;
  q_constel_config = lm_mgp_get_constellation_config();
  mgp_InitGnssDbOnMgpLoad(q_constel_config);
  
}
#ifdef FEATURE_DYNAMIC_LOADING_GNSS
/*
 ******************************************************************************
 * lm_dmm_loaded_state_handler
 *
 * Function description:
 *
 *  Handles all events of DMM state machine when
 *  it is unloaded state
 *
 * Parameters: 
 *   lm_dmm_state_machine_event_e_type event
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_unloaded_state_handler(lm_dmm_state_machine_event_e_type event)
{
  marb_procedure_t e_marb_proc_type = MARB_PROC_NONE;
  if(event == LM_DMM_EVENT_MGP_LOAD_REQ)
  { 
    if((os_TimerCheck(z_lm_dmm_context_info.p_MgpLoadAttemptBackOffTimer)!= 0) )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"LM_DMM:MGP load not possible as min_wait time is running",0,0,0);
      z_lm_dmm_context_info.lm_dmm_exception_handler_func(LM_DMM_EXCEPTION_LOAD_REQ_IN_INCOMPATIBLE_STATE);
    }
    else
    { 
      e_marb_proc_type = lm_dmm_get_marb_procedure_type();
      /*REV 12/04: Use constants forgnss_proc_duration and gnss_cleanup_time */
      z_lm_dmm_arb_payload = lm_dmm_marb_req(e_marb_proc_type);
      
      LM_MSG_HIGH("LM_DMM: arb_req result %d , min_wait = %x ", z_lm_dmm_arb_payload.e_result, z_lm_dmm_arb_payload.min_wait, 0);

      if (z_lm_dmm_arb_payload.e_result == MARB_RESULT_PENDING_ACCEPT)
      {
        /*REV 120/04: Change p_dmmLoadWaitGuardTimer name to as defined in state machine diagram */
        if (!os_TimerStart(z_lm_dmm_context_info.p_DmmArbReqWaitTimer, LM_DMM_LOAD_WAIT_GAURD_TIME, 0))
        {
          LM_MSG_ERROR("LM_DMM: Failed starting load wait guard timer", 0, 0, 0);
          ERR_FATAL("LM_DMM: Failed starting load wait guard timer", 0, 0, 0);
        }
        else
        {
          LM_MSG_MED("LM_DMM: p_DmmArbReqWaitTimer started with duration = %d ", LM_DMM_LOAD_WAIT_GAURD_TIME,0,0);
          /*REV 12/04: Avoid usage of this state variable lm_gnss_proc_state */
          z_lm_dmm_context_info.e_lm_dmm_state=LM_DMM_STATE_LOADING;
        }
      }
      else if (z_lm_dmm_arb_payload.e_result == MARB_RESULT_ACCEPT)
      {
        if (dmm_open_module(DM_ID_GNSS,NULL)!= DM_STATUS_DONE)
        {
          z_lm_dmm_context_info.lm_dmm_exception_handler_func(LM_DMM_EXCEPTION_MGP_MODULE_OPEN_FAILURE);
        }
        else
        {
          LM_MSG_HIGH("LM_DMM: dmm_open_module successfully loads MGP", 0, 0, 0);
          lm_dmm_initialize_mgp();
/*REV 12/04: Start a timer mgp_loadtimer as per state machine diagram*/
          if (!os_TimerStart(z_lm_dmm_context_info.p_MgpLoadTimer, LM_DMM_MGP_LOAD_TIME, 0))
          {
            LM_MSG_ERROR("LM_DMM: Failed starting min_wait timer", 0, 0, 0);
            ERR_FATAL("LM_DMM:Failed starting MGP Load timer", 0, 0, 0);
          }
          z_lm_dmm_context_info.e_lm_dmm_state=LM_DMM_STATE_LOADING;
        }
      }
      else if (z_lm_dmm_arb_payload.e_result == MARB_RESULT_FAIL)
      {
/*REV 12/04: Use name backOffTime instead of min_wait*/
        if (z_lm_dmm_arb_payload.min_wait !=0)
        {
          if (!os_TimerStart(z_lm_dmm_context_info.p_MgpLoadAttemptBackOffTimer, (uint32)z_lm_dmm_arb_payload.min_wait, 0))
          {
            LM_MSG_ERROR("LM_DMM: Failed starting min_wait timer", 0, 0, 0);
            ERR_FATAL("LM_DMM:Failed starting min_wait timer", 0, 0, 0);
          }
        }
        LM_MSG_MED("LM_DMM: p_MgpLoadAttemptBackOffTimer started with duration = %d ", z_lm_dmm_arb_payload.min_wait,0,0);
        z_lm_dmm_context_info.lm_dmm_exception_handler_func(LM_DMM_EXCEPTION_MARB_MGP_LOAD_FAILURE);
      }
      else
      {
        LM_MSG_MED("LM_DMM:arb_req returned unknown value %d", z_lm_dmm_arb_payload.e_result, 0, 0);
        z_lm_dmm_context_info.lm_dmm_exception_handler_func(LM_DMM_EXCEPTION_MARB_UNKNOWN_RESPONSE);
      }
    }
  }
  else
  {
    LM_MSG_MED("LM_DMM: Event = %d can not be handled in UNLOADED state ",event,0,0);
    z_lm_dmm_context_info.lm_dmm_exception_handler_func(LM_DMM_EXCEPTION_UNEXPECTED_EVENT);
  }
}

/*
 ******************************************************************************
 * lm_dmm_loaded_state_handler
 *
 * Function description:
 *
 *  Handles all events of DMM state machine when
 *  it is loaded state
 *
 * Parameters: 
 *   lm_dmm_state_machine_event_e_type event
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_loaded_state_handler(lm_dmm_state_machine_event_e_type event)
{
  marb_procedure_t e_marb_proc_type = MARB_PROC_NONE;
  if ((event == LM_DMM_EVENT_RCVR_OFF_IND))
  {
    LM_MSG_MED("LM_DMM:No need to unload as close_module is not triggered by unload_event from arbitrator  ",0, 0, 0);
    z_lm_dmm_arb_payload = lm_dmm_marb_req(e_marb_proc_type);
  }
  else if(event == LM_DMM_EVENT_ARB_UNLOAD_IND)
  {
    /*Note: Its is important that the DMM state is first changed to unloading
      state before.calling proceed to unload because if the receiver is 
      already when LM_DMM_EVENT_ARB_UNLOAD_IND of the dmm state machine is called 
      with receiver off event immediately.
      But the dmm state machine has not moved to unloading state till this point 
      so it will be treated as normal receiver off and the DMM state machine 
      will not start to unload MGP.
    */
    z_lm_dmm_context_info.e_lm_dmm_state = LM_DMM_STATE_UNLOADING;
    lm_dmm_proceed_to_unload();
    /* state will remain loaded yet.
       lm_proc will eventually send DMM_CLOSE_MODULE after RX-OFF ack at which state will move 
    */
  }
  else if(event == LM_DMM_EVENT_GNSS_ACTIVATE_REQ)
  {
     /*Check if marb has to requested to allow a GNSS session.  
       if it is required to req Marb get appropriate marb procedure
       type. if GNSS ativation is for SARF then marb_prcedure_type is
       HIGH other wise for a GNSS session its ACTIVE.
    */
     if(TRUE == lm_dmm_is_marb_req_needed())
     {
       e_marb_proc_type = lm_dmm_get_marb_procedure_type ();
       z_lm_dmm_arb_payload = lm_dmm_marb_req(e_marb_proc_type);
       /*
         if marb grants permission to start GNSS receiver then go ahead
         and start session else raise an exception 
       */
       if(z_lm_dmm_arb_payload.e_result == MARB_RESULT_ACCEPT)
       {
          LM_MSG_HIGH("DMM_LM:MARB allowed to start GNSS proceed to start receiver",0, 0, 0);
          z_lm_dmm_context_info.lm_dmm_gnss_rsc_grant_handler_func();
       }
       else
       {
          LM_MSG_MED("DMM_LM:MARB allowed to start GNSS clear the session",0, 0, 0);
          z_lm_dmm_context_info.lm_dmm_exception_handler_func(LM_DMM_EXCEPTION_MARB_GNSS_ACTIVATION_REJECT);
       }
     }
     else
     {
        z_lm_dmm_context_info.lm_dmm_gnss_rsc_grant_handler_func();
     }
  }
  else
  {
    LM_MSG_HIGH("LM_DMM: Unsupported event %d is recieved when loaded.", event,0,0);
    z_lm_dmm_context_info.lm_dmm_exception_handler_func(LM_DMM_EXCEPTION_UNEXPECTED_EVENT);
  }
}
/*
 ******************************************************************************
 * lm_dmm_loading_state_handler
 *
 * Function description:
 *
 *  Handles all events of DMM state machine when
 *  it is loading state
 *
 * Parameters: 
 *   lm_dmm_state_machine_event_e_type event
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_loading_state_handler(lm_dmm_state_machine_event_e_type event)
{
  //LM_MSG_MED("LM_DMM: event %d recived in state LM_DMM_STATE_LOADING, payload = %x ",event, event_payload, 0);
  if (event == LM_DMM_EVENT_ARB_ACCEPT_IND) /* Arb accept is received*/
  {
    
    os_TimerStop(z_lm_dmm_context_info.p_DmmArbReqWaitTimer); /*lint !e534 */
    LM_MSG_HIGH("LM_DMM: p_dmmArbReqWaitTimer stopped due to reception of Open Module", z_lm_dmm_arb_payload.min_wait,0,0);
/*REV 12/04: move code till mgp_InitGnssDbOnMgpLoad into one utility function name lm_dmm_load_gnss  */

    if (dmm_open_module(DM_ID_GNSS,NULL)!= DM_STATUS_DONE)
    {
      z_lm_dmm_context_info.lm_dmm_exception_handler_func(LM_DMM_EXCEPTION_MGP_MODULE_OPEN_FAILURE);
    }
    LM_MSG_HIGH("LM_DMM: dmm_open_module successfully loads MGP", 0, 0, 0);
    lm_dmm_initialize_mgp();
/*REV 12/05: Start a new timer MGP load timer as per state machine*/
    if (!os_TimerStart(z_lm_dmm_context_info.p_MgpLoadTimer, LM_DMM_MGP_LOAD_TIME, 0))
    {
      LM_MSG_ERROR("LM_DMM: Failed starting Mgp Load timer", 0, 0, 0);
      ERR_FATAL("LM_DMM:Failed starting Mgp Load timer", 0, 0, 0);
    }
    
  }
  /*REV 12/05: Put a ERR_Fatal temporarily to check why this event came now*/
 
  /*REV 12/05: Put a ERR_Fatal temporarily to check why this event came now*/

  else if (event == LM_DMM_EVENT_MGP_LOAD_DONE_IND)
  {
    os_TimerStop(z_lm_dmm_context_info.p_MgpLoadTimer);
    z_lm_dmm_context_info.e_lm_dmm_state=LM_DMM_STATE_LOADED;
    lm_dmm_marb_indication(MARB_MEM_LOADED);
#ifdef TEST_UNLOAD_TRIGGER_FROM_INTERNAL_CODE_LOCATIONS
    trigger_gnss_unload(5);
#endif

    if(LM_MPG_LOAD_FOR_INVALID_REASON != lm_mgp_get_mgp_load_reason())
    {
      /*start GNSS session now*/
      z_lm_dmm_context_info.lm_dmm_gnss_rsc_grant_handler_func();
    }
  }
/*REV 12/05: This event means relevant timers expired. Remove z_lmControl.v_FixStartIndPending= FALSE*/
  else if (event  == LM_DMM_EVENT_ABORT_MGP_LOAD_IND)
  {
    z_lm_dmm_context_info.e_lm_dmm_state=LM_DMM_STATE_UNLOADED;
    z_lm_dmm_context_info.lm_dmm_exception_handler_func(LM_DMM_EXCEPTION_ABORT_LOAD);
  }
  else if(event  == LM_DMM_EVENT_MGP_LOAD_REQ)
  {

    LM_MSG_HIGH("LM_DMM: Load req is recieved when loading.",0,0,0);
    z_lm_dmm_context_info.lm_dmm_exception_handler_func(LM_DMM_EXCEPTION_LOAD_REQ_OVER_LOADING);
  }
  else
  {
    LM_MSG_MED("LM_DMM: Unexpected event %d is recieved when loading.", event,0,0);
    z_lm_dmm_context_info.lm_dmm_exception_handler_func(LM_DMM_EXCEPTION_UNEXPECTED_EVENT);
  }
}

/*
 ******************************************************************************
 * lm_dmm_unloading_state_handler
 *
 * Function description:
 *
 *  Handles all events of DMM state machine when
 *  it is unloading state
 *
 * Parameters: 
 *   lm_dmm_state_machine_event_e_type event
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_unloading_state_handler(lm_dmm_state_machine_event_e_type event)
{
      if ((event == LM_DMM_EVENT_MGP_UNLOAD_DONE_IND) || 
           (event == LM_DMM_EVENT_MGP_UNLOAD_FAIL_IND))
      {
        os_TimerStop(z_lm_dmm_context_info.p_MgpUnLoadTimer);
        if (dmm_close_module(DM_ID_GNSS, NULL)!= DM_STATUS_DONE)
        {
          ERR_FATAL("LM_DMM:MGP dmm_close_module failed", 0, 0, 0);
        }
        else
        {
          LM_MSG_MED("LM_DMM: dmm_close_module successfully unloads MGP", 0, 0, 0);
          z_lm_dmm_context_info.e_lm_dmm_state=LM_DMM_STATE_UNLOADED;
          lm_dmm_marb_indication(MARB_MEM_UNLOADED);
        }
      }
      else if(event == LM_DMM_EVENT_RCVR_OFF_IND)
      {
        /* cleanup and RX-OFF already done so now start deinit. After deinit dmm_close_module will happen */
        if (!os_TimerStart(z_lm_dmm_context_info.p_MgpUnLoadTimer, LM_DMM_MGP_UNLOAD_TIME, 0))
        {
          LM_MSG_ERROR("LM_DMM: Failed starting min_wait timer", 0, 0, 0);
          ERR_FATAL("LM_DMM:Failed starting min_wait timer", 0, 0, 0);
        }
        mgp_DeInitMgpForUnload();
      }
      else if(event == LM_DMM_EVENT_MGP_LOAD_REQ)
      { 
        LM_MSG_MED("LM_DMM: Unsupported event %d is recieved when loading.", event,0,0);
        z_lm_dmm_context_info.lm_dmm_exception_handler_func(LM_DMM_EXCEPTION_LOAD_REQ_IN_INCOMPATIBLE_STATE);
      }
      else
      {
        LM_MSG_MED("LM_DMM: Unsupported event %d is recieved when unloading.", event,0,0);
        z_lm_dmm_context_info.lm_dmm_exception_handler_func(LM_DMM_EXCEPTION_UNEXPECTED_EVENT);
      }
}
#endif

/*
 ******************************************************************************
 * lm_dmm_state_machine
 *
 * Function description:
 *
 *  Implementation of DMM state machine 
 *
 * Parameters: 
 *   lm_dmm_state_machine_event_e_type event
 * Return value: 
 *   lm_dmm_state_e_type : state of DMM state machine
 ******************************************************************************
*/

lm_dmm_state_e_type lm_dmm_state_machine( lm_dmm_state_machine_event_e_type event)
{
#ifndef FEATURE_DYNAMIC_LOADING_GNSS
  return LM_DMM_STATE_LOADED;
#else
  /* State independent handling */
  if (event == LM_DMM_EVENT_ARB_NOTIFY_IND)
  {
    LM_MSG_MED("LM_DMM: p_MgpLoadAttemptBackOffTimer stopped due to reception of Arb Notify", z_lm_dmm_arb_payload.min_wait,0,0);
    os_TimerStop(z_lm_dmm_context_info.p_MgpLoadAttemptBackOffTimer); /*lint !e534 */
    return (z_lm_dmm_context_info.e_lm_dmm_state);
  }

/* State dependent handling */
  LM_MSG_HIGH("LM_DMM: z_lm_dmm_context_info.e_lm_dmm_state : %d event=%d", z_lm_dmm_context_info.e_lm_dmm_state,event,0);
  
   if((LM_DMM_EVENT_ARB_UNLOAD_IND == event) && (z_lmControl.z_session_request.e_mode == LM_GPS_MODE_SA_RF))
      {
        ERR_FATAL("Debug: LM_DMM: UNLOAD req when in SA RF mode", 0, 0, 0);
      }
  switch (z_lm_dmm_context_info.e_lm_dmm_state)
  {
    case LM_DMM_STATE_UNLOADED:
      lm_dmm_unloaded_state_handler(event);
    break;
    case LM_DMM_STATE_LOADED:
      lm_dmm_loaded_state_handler(event);
    break;
    case LM_DMM_STATE_LOADING:
      lm_dmm_loading_state_handler(event);
    break;
    case LM_DMM_STATE_UNLOADING:
      lm_dmm_unloading_state_handler(event);
    break;
  }
  return (z_lm_dmm_context_info.e_lm_dmm_state);
#endif

}

/*
 ******************************************************************************
 * lm_get_dmm_state
 *
 * Function description:
 *
 *  provides state of DMM state machine 
 *
 * Parameters: 
 *
 * Return value: 
 *   lm_dmm_state_e_type : state of DMM state machine
 ******************************************************************************
*/

lm_dmm_state_e_type lm_get_dmm_state(void)
{
   return z_lm_dmm_context_info.e_lm_dmm_state;
}

/*
 ******************************************************************************
 * lm_dmm_proceed_to_unload
 *
 * Function description:
 *
 *  Prepares LM for unloading MGP by clearing any ongoing or 
 *  pending GNSS session 
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_proceed_to_unload(void)
{
  /*If session is pending then it should be cleaned up before unloading
    MGP.
    Else if receiver is already off then inform DMM state machine that 
    receiver is turned off and it can proceed to unload MGP
    */

  if(LM_MGP_STATE_OFF != lm_mgp_get_gnns_rcvr_state())
  {
    lm_tm_handle_gnss_failure(TM_ERROR_MGP_LOADING_ERROR);
  }
  else
  {
    if(LM_SESSION_STATE_NO_SESSION != lm_mgp_get_gnss_sess_state())
    {
      lm_tm_handle_gnss_failure(TM_ERROR_MGP_LOADING_ERROR);
    }
    (void)lm_dmm_state_machine(LM_DMM_EVENT_RCVR_OFF_IND);
  }
}
void lm_dmm_configure_proc_duration(void)
{
    int  fd;
    int  q_BytesRead;
    uint8 proc_duration;
    uint8 cleanup_duration;

    fd = efs_open( FILEPATH_GNSS_PROC_DURATION, O_RDONLY );

    if( fd < 0 )
    {
      /* set default */
      z_lm_dmm_context_info.u_gnss_proc_duration = LM_DMM_GNSS_PROC_DURATION;
      LM_MSG_ERROR("LM_DMM:nv read failed ..setting default.%d", z_lm_dmm_context_info.u_gnss_proc_duration, 0, 0);
      
    }
    else
    {
      q_BytesRead = efs_read( fd, &proc_duration, sizeof(proc_duration) );
      (void) efs_close( fd );
      /*
        proc duration stored in EFS file is multiple of 0.5 sec so to get value 
        value in milliseconds need to multiply by 500
      */
        z_lm_dmm_context_info.u_gnss_proc_duration = proc_duration * 500;
        LM_MSG_ERROR("LM_DMM:NV read succeeded lm_unload_mask = %d", z_lm_dmm_context_info.u_gnss_proc_duration, 0, 0);
    }


    fd = efs_open( FILEPATH_GNSS_CLEANUP_DURATION, O_RDONLY );
    if( fd < 0 )
    {
      /* set default */
      z_lm_dmm_context_info.u_gnss_cleanup_time = LM_DMM_GNSS_CLEANUP_TIME;
      LM_MSG_ERROR("LM_DMM:nv read failed ..setting default.%d", z_lm_dmm_context_info.u_gnss_cleanup_time, 0, 0);
      
    }
    else
    {
      q_BytesRead = efs_read( fd, &cleanup_duration, sizeof(cleanup_duration) );
      (void) efs_close( fd );
      /*
        proc duration stored in EFS file is multiple of 0.01 sec so to get value 
        value in milliseconds need to multiply by 10
      */
        z_lm_dmm_context_info.u_gnss_cleanup_time = cleanup_duration * 10;
        LM_MSG_ERROR("LM_DMM:NV read succeeded lm_unload_mask = %d", z_lm_dmm_context_info.u_gnss_cleanup_time, 0, 0);
    }


}


/*
 ******************************************************************************
 * lm_dmm_init
 *
 * Function description:
 *
 * Initializes the variables in z_lm_dmm_context_info structure
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/
void lm_dmm_init(void)
{
#ifdef FEATURE_DYNAMIC_LOADING_GNSS
  z_lm_dmm_context_info.e_lm_dmm_state = LM_DMM_STATE_UNLOADED;
#else
  z_lm_dmm_context_info.e_lm_dmm_state = LM_DMM_STATE_LOADED;
#endif
  z_lm_dmm_context_info.p_DmmArbReqWaitTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_DMM_LOAD_ARB_REQ_WAIT_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lm_dmm_context_info.p_MgpLoadAttemptBackOffTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_MGP_LOAD_ATTEMPT_BACK_OFF_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lm_dmm_context_info.p_MgpLoadTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_MGP_LOAD_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lm_dmm_context_info.p_MgpUnLoadTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_MGP_UNLOAD_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lm_dmm_context_info.lm_dmm_exception_handler_func = NULL;
  lm_dmm_configure_proc_duration();
}

/*
 ******************************************************************************
 * lm_dmm_register_cb
 *
 * Function description:
 *
 *  Registers callback to function that handles exceptions thrown by 
 *  DMM state machine.
 *
 * Parameters: 
 *   lm_dmm_exception_handler_f_type *: pointer to function 
 *                                              that handles exceptions
 *                                              thrown by DMM state machine
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_register_cb(lm_dmm_exception_handler_f_type *p_lm_dmm_exception_handler_func, 
              lm_dmm_gnss_rsc_grant_handler_f_type *p_lm_dmm_gnss_rsc_grant_handler_func)

{
   z_lm_dmm_context_info.lm_dmm_exception_handler_func = p_lm_dmm_exception_handler_func;
   z_lm_dmm_context_info.lm_dmm_gnss_rsc_grant_handler_func = p_lm_dmm_gnss_rsc_grant_handler_func;
}

