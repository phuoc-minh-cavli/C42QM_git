/*****************************************************************************
***
*** TITLE
***
*** 
***
***
*** DESCRIPTION
***
***  
*** 
*** Copyright (c) 2001-2018 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //components/rel/geran.mpss/5.2.0/gcommon/static/src/geran_static_dmm.c#10 $
*** $DateTime: 2023/01/17 00:05:41 $ $Author: pwbldsvc $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 17/01/23   sastryk Do not de-register with MARB when the previous request is in pending accept state
*** 09/07/18   rrp     Added file to have DMM related functionalities in static code.
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
#include "dmm.h"
#include "marb.h"
#include "DDITimetick.h"
#include "geran_dmm.h"
#include "gtmrs_g.h"
#include "gpl1_dual_sim.h"
extern boolean L1_ftm_mode_enabled(void);
extern idle_data_T           l1_idle_data_store;


typedef struct
{
  DalDeviceHandle  *dal_handle;
  geran_dyn_state_e dyn_state;
  proc_type_e       curr_proc;
  boolean           mm_high_priority;
  boolean           arb_pending_accept;
} geran_static_data_t;

geran_static_data_t geran_static_data;

boolean geran_dmm_load(void)
{
  dm_status_e_type status = dmm_open_module(DM_ID_GERAN, NULL);
  MSG_GERAN_HIGH_1("GERAN load completed (status=%d)", status);
  return (status == DM_STATUS_DONE);
}

boolean geran_dmm_unload(void)
{
  dm_status_e_type status = dmm_close_module(DM_ID_GERAN, NULL);
  MSG_GERAN_HIGH_1("GERAN unload completed (status=%d)", status);
  return (status == DM_STATUS_DONE);
}

boolean geran_dmm_suspend(void)
{
  dm_status_e_type status = dmm_suspend_module(DM_ID_GERAN, NULL);
  MSG_GERAN_HIGH_1("GERAN suspend completed (status=%d)", status);
  return (status == DM_STATUS_DONE);
}

boolean geran_dmm_resume(void)
{
  dm_status_e_type status = dmm_resume_module(DM_ID_GERAN, NULL);
  MSG_GERAN_HIGH_1("GERAN resume completed (status=%d)", status);

  /* Start post resumption activities */
  //geran_gsn_start_post_resumption_activity();
  
  return (status == DM_STATUS_DONE);
}

void geran_set_dyn_state(geran_dyn_state_e state)
{
  MSG_GERAN_HIGH_2("GERAN dynamic state change (%d -> %d)", geran_static_data.dyn_state, state);
  geran_static_data.dyn_state = state;
}

geran_dyn_state_e geran_get_dyn_state(void)
{
  return geran_static_data.dyn_state;
}

/* External API, used by GRF */
boolean geran_dmm_set_loaded(void)
{
  int ftm_mode = ftm_get_mode();
  dm_state_e_type module_state = dmm_get_module_state(DM_ID_GERAN);

  MSG_GERAN_HIGH_2("geran_dmm_set_loaded (ftm_mode=%d, module_state=%d)", ftm_mode, module_state);

  if (ftm_mode == FTM_MODE && module_state == DM_STATE_ACTIVE)
  {
    geran_set_dyn_state(GERAN_LOADED);
    return TRUE;
  }

  return FALSE;
}

boolean geran_dmm_set_inactive(void)
{
  int ftm_mode = ftm_get_mode();
  dm_state_e_type module_state = dmm_get_module_state(DM_ID_GERAN);

  MSG_GERAN_HIGH_2("geran_dmm_set_inactive (ftm_mode=%d, module_state=%d)", ftm_mode, module_state);

  if (ftm_mode == FTM_MODE && module_state != DM_STATE_ACTIVE)
  {
    geran_set_dyn_state(GERAN_INACTIVE);
    return TRUE;
  }

  return FALSE;
}

void geran_set_curr_proc(proc_type_e proc)
{
  geran_static_data.curr_proc = proc;
}

proc_type_e geran_get_curr_proc(void)
{
  return geran_static_data.curr_proc;
}

void geran_set_mm_high_priority(boolean priority)
{
  MSG_GERAN_HIGH_2("GERAN mm_high_priority change (%d -> %d)", geran_static_data.mm_high_priority, priority);
  geran_static_data.mm_high_priority = priority;
  return;
}

boolean geran_get_mm_high_priority(void)
{
  return geran_static_data.mm_high_priority;
}

boolean is_geran_loaded(void)
{
  return (geran_get_dyn_state() == GERAN_LOADED || geran_get_dyn_state() == GERAN_LOADED_SLEEP 
#ifdef FEATURE_FSK_SUPPORT
  || geran_get_dyn_state() == GERAN_FSK_LOADED
#endif
  );

}

void geran_dal_init(void)
{
  DALResult result = DalTimetick_Attach("SystemTimer", &geran_static_data.dal_handle);
  ASSERT(result == DAL_SUCCESS);
}

uint32 geran_timeticks_to_ms(uint64 tt)
{
  uint64 return_ms;
  (void)DalTimetick_CvtFromTimetick64(geran_static_data.dal_handle, tt, T_MSEC, &return_ms);
  return (uint32)return_ms;
}

uint64 geran_ms_to_timeticks(uint32 ms)
{
  uint64 return_tt;
  (void)DalTimetick_CvtToTimetick64(geran_static_data.dal_handle, ms, T_MSEC, &return_tt);
  return return_tt;
}

uint64 geran_get_time_ms(void)
{
  uint64 ustmrNow = 0;
  uint64 return_ms;

  DalTimetick_GetTimetick64(geran_static_data.dal_handle, &ustmrNow);
  (void)DalTimetick_CvtFromTimetick64(geran_static_data.dal_handle, ustmrNow, T_MSEC, &return_ms);

  return return_ms;
}

proc_type_e geran_prioritize_proc(proc_type_e proc)
{
  /* If MM priority is HIGH then change IDLE/CONNET to HIGH */
  if (geran_static_data.mm_high_priority && (proc == PROC_IDLE || proc == PROC_CONNECT))
    return PROC_HIGH;
  else
    return proc;
}

geran_arb_result_e geran_marb_to_garb_result_map(marb_result_enum_t result)
{
  geran_arb_result_e ret_value;

  switch (result)
  {
    case MARB_RESULT_FAIL:
      ret_value = ARB_RESULT_FAIL;
      break;

    case MARB_RESULT_ACCEPT:
      ret_value = ARB_RESULT_SUCCESS;
      break;

    case MARB_RESULT_PENDING_ACCEPT:
      ret_value = ARB_RESULT_PENDING;
      break;

    default:
      ERR_GERAN_FATAL_1("Unexpected result %d", result);
  }

  return ret_value;
}

marb_procedure_t geran_garb_to_marb_proc_map(proc_type_e proc)
{
  marb_procedure_t ret_value;

  switch (proc)
  {
    case PROC_NONE:
      ret_value = MARB_PROC_NONE;
      break;

    case PROC_IDLE:
      ret_value = MARB_PROC_IDLE_AWAKE;
      break;

    case PROC_CONNECT:
      ret_value = MARB_PROC_CONNECTED;
      break;

    case PROC_HIGH:
      ret_value = MARB_PROC_HIGH;
      break;

    default:
      ERR_GERAN_FATAL_1("Unexpected proc %d", proc);
  }

  return ret_value;
}

void geran_arb_register(arb_cb_t cb)
{
  boolean status;
  marb_register_input_t params;

  params.cb_bitmask = (1 << MARB_EVENT_NOTIFY_CB);
  params.cb_ptr = cb;

  status = marb_register(MARB_CLIENT_GSM, MARB_CLI_GROUP_WWAN, params);
  MSG_GERAN_HIGH_1("MARB register (status=%d)", status);

  if (status != TRUE)
  {
    ERR_GERAN_FATAL_0("MARB register failed");
  }
}

void geran_arb_deregister(void)
{
  marb_result_enum_t status = marb_deregister(MARB_CLIENT_GSM);
  MSG_GERAN_HIGH_1("MARB deregister (status=%d)", status);

  if (status != MARB_RESULT_ACCEPT)
  {
    ERR_GERAN_FATAL_0("MARB deregister failed");
  }
	geran_static_data.arb_pending_accept = FALSE;
}

geran_arb_result_e geran_arb_request(proc_type_e proc, uint32 duration, uint32 cleanup_time)
{
  marb_result_enum_t      result;
  marb_request_input_t    input;
  marb_wait_time_output_t output;
  geran_static_data.arb_pending_accept = FALSE;

  proc = geran_prioritize_proc(proc);
  
  input.procedure = geran_garb_to_marb_proc_map(proc);
  input.duration  = geran_ms_to_timeticks(duration);
  input.cleanup_duration = geran_ms_to_timeticks(cleanup_time);

  result = marb_request(MARB_CLIENT_GSM, &input, &output);

  if( result == MARB_RESULT_PENDING_ACCEPT )
  {
    geran_static_data.arb_pending_accept = TRUE;
  }

  MSG_GERAN_HIGH_6("MARB request (proc=%d, duration=0x%x%08x, cleanup_duration=0x%x%08x, result=%d)",
    input.procedure, 
    HI_32_OF_64_BITS(input.duration), LO_32_OF_64_BITS(input.duration),
    HI_32_OF_64_BITS(input.cleanup_duration), LO_32_OF_64_BITS(input.cleanup_duration),
    result
  );

  return geran_marb_to_garb_result_map(result);
}

boolean geran_arb_status_pending_accept( void )
{
 return geran_static_data.arb_pending_accept;
}

void geran_arb_reset_pending_accept()
{
  geran_static_data.arb_pending_accept = FALSE;
}

void geran_arb_sleep(proc_type_e next_proc, uint32 start_time, uint32 cleanup_time)
{
  marb_result_enum_t result;
  marb_sleep_input_t input;
  uint64 ustmrNow = 0;
  uint64 ustmrDuration = geran_ms_to_timeticks(start_time);

  DalTimetick_GetTimetick64(geran_static_data.dal_handle, &ustmrNow);
  input.procedure = geran_garb_to_marb_proc_map(next_proc);
  input.start_time = ustmrNow + ustmrDuration;
  input.cleanup_duration = geran_ms_to_timeticks(cleanup_time);

  result = marb_sleep(MARB_CLIENT_GSM, &input);
  
  MSG_GERAN_HIGH_7("MARB sleep (proc=%d, sleep_duration=%dms, start_time=0x%x%08x, cleanup_duration=0x%x%08x, result=%d)",
    input.procedure,
    start_time,
    HI_32_OF_64_BITS(input.start_time), LO_32_OF_64_BITS(input.start_time),
    HI_32_OF_64_BITS(input.cleanup_duration), LO_32_OF_64_BITS(input.cleanup_duration),
    result
  );

  if (result != MARB_RESULT_ACCEPT)
  {
    MSG_GERAN_HIGH_0("MARB sleep failed");
  }
}

void geran_arb_indication(geran_arb_ind_e indication)
{
  marb_indication_input_t input;

  input.indication = (indication == ARB_GERAN_LOADED) ? MARB_MEM_LOADED : MARB_MEM_UNLOADED;
  MSG_GERAN_HIGH_1("MARB indication (indication=%d)", input.indication);
  marb_indication(MARB_CLIENT_GSM, &input);
}

void rr_send_mph_proc_req(proc_type_e proc)
{
  mph_proc_req_t mph_proc_req;

  RR_PREPARE_L1_MESSAGE(mph_proc_req, MPH_PROC_REQ);
  mph_proc_req.proc = proc;
  (void)gs_send_message(GS_QUEUE_LAYER_1, (void*)&mph_proc_req, TRUE);

  MSG_GERAN_HIGH_1("MPH_PROC_REQ sent to GL1 (proc=%d)", proc);
}

void L1_send_MPH_PROC_CNF(proc_type_e proc, proc_status_e status)
{
  mph_proc_cnf_t mph_proc_cnf;
  gs_queue_id_T  queue_id;

  RR_PREPARE_L1_MESSAGE(mph_proc_cnf, MPH_PROC_CNF);
  mph_proc_cnf.proc = proc;
  mph_proc_cnf.status = status;
  if (L1_ftm_mode_enabled())
  {
    queue_id = GS_QUEUE_FTM_GSM;
  }
  else
  {
    queue_id = GS_QUEUE_RR;
  }
  (void)gs_send_message(queue_id, (void*)&mph_proc_cnf, TRUE);

  MSG_GERAN_HIGH_3("MPH_PROC_CNF sent to queue_id %d (proc=%d, status=%d)", queue_id, proc, status);
}

void L1_send_MPH_PROC_IND(proc_type_e proc)
{
  mph_proc_ind_t mph_proc_ind;
  gs_queue_id_T  queue_id;
  idle_data_T *l1_idle_data = &l1_idle_data_store;
  uint32 drx_period = FRAMES_TO_MSECS((l1_idle_data->campedon_cell_data.BS_PA_MFRMS)*MULTIFRAME_51);


  RR_PREPARE_L1_MESSAGE(mph_proc_ind, MPH_PROC_IND);
  mph_proc_ind.proc = proc;

  if(proc == PROC_SUSPEND)
 {
    if(drx_period < g_gps_min_wait)
    mph_proc_ind.rem_drx_time_ms = drx_period;
    else
    mph_proc_ind.rem_drx_time_ms = g_gps_min_wait;
    MSG_GERAN_HIGH_3("sending proc suspend min wait %d load wait=%d drx_period = %d", g_gps_min_wait, g_gps_load_wait,drx_period);

 }

  if (L1_ftm_mode_enabled())
  {
    queue_id = GS_QUEUE_FTM_GSM;
  }
  else
  {
    queue_id = GS_QUEUE_RR;
  }
  (void)gs_send_message(queue_id, (void*)&mph_proc_ind, TRUE);

  MSG_GERAN_HIGH_3("MPH_PROC_IND sent to queue_id %d (proc=%d) remaining_time = %d", queue_id, proc,mph_proc_ind.rem_drx_time_ms );
}

#endif

