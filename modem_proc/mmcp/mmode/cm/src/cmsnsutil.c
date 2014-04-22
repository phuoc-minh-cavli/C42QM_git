#include "cmsnsutil.h"
#include "cmpsm.h"
#include "cm_psm_uart.h"
#include "cmefs.h"

#ifdef FEATURE_MMODE_TX_21
#ifndef CM_DEBUG 
#include "stdbool.h"
#endif
#include "sns_tracker_log.h"
#endif

#ifdef FEATURE_REF_TRACKER 
static boolean context_change = FALSE;
#ifdef FEATURE_MMODE_TX_21

static config_init_output algo_output[CM_MAX_SNS_ENABLED];
cm_sns_reg_config_s_type reg_config_global;
static float als_previous_value = -1;

config_init_output* cm_send_algo_output()
{
     return algo_output;  
}
cm_sns_reg_config_s_type *get_reg_config()
{
  return &reg_config_global;
}

#endif

cm_psm_info_s_type *cmpsm_ptr(void);
extern cm_sns_motion_event_e_type event_debug ;

extern  cm_snsmgr_state_e_type next_sns_state_g ;

#ifndef FEATURE_MMODE_TX_21
/*===========================================================================
FUNCTION cm_sns_get_fmd_config

DESCRIPTION
  get the FMD config to send the sensor

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/


void cm_sns_get_fmd_config(byte *state,uint16 *pres_thres,byte *accel_freq,byte *accel_sample_len)
{

 cm_sns_data_collection_s_type*	 sns_ptr = cmsns_get_data_info_ptr();

 CM_ASSERT(sns_ptr != NULL);

 if( sns_ptr->sns_odr_data.fmd_data.is_enable == TRUE )
 {
   *state = sns_ptr->sns_odr_data.fmd_data.flight_status;
   *pres_thres = sns_ptr->sns_odr_data.fmd_data.pressure_threshold;
   *accel_freq = sns_ptr->sns_odr_data.fmd_data.accel_sample_rate;
   *accel_sample_len = sns_ptr->sns_odr_data.fmd_data.accel_sample_length;
 }

}
 /*===========================================================================
 FUNCTION cm_sns_get_fmd_config
 
 DESCRIPTION
   get the FMD config to send the sensor
 
 DEPENDENCIES
   none
 
 RETURN VALUE
   boolean
 
 SIDE EFFECTS
   none
 
 ===========================================================================*/

 boolean cm_sns_get_tilt_config(

  cm_sns_tilt_tracker_transportation_type *transportation_type,
  boolean *has_redo_gravity_init,
  boolean *redo_gravity_init, 
  boolean *has_iteration_delay,
  uint32 *iteration_delay,
  boolean *has_max_init_trial_num,
  uint8 *max_init_trial_num,
  boolean *has_max_update_trial_num,    
  uint8 *max_update_trial_num, 
  boolean *has_init_gravity_threshold, 
  float *init_gravity_threshold,
  boolean *has_update_gravity_threshold,
  float *update_gravity_threshold
)
{
  cm_sns_data_collection_s_type*	 sns_ptr = cmsns_get_data_info_ptr();

  CM_ASSERT(sns_ptr != NULL);
  if(sns_ptr->sns_intr_data.tilt_data.has_config_change == FALSE || sns_ptr->sns_intr_data.tilt_data.is_enable == FALSE)
  {
    return FALSE;
  }
  *transportation_type = sns_ptr->sns_intr_data.tilt_data.config_data.transportation_type;
  *has_redo_gravity_init= sns_ptr->sns_intr_data.tilt_data.config_data.has_redo_gravity_init;
  *redo_gravity_init= sns_ptr->sns_intr_data.tilt_data.config_data.redo_gravity_init;
  *has_iteration_delay= sns_ptr->sns_intr_data.tilt_data.config_data.has_iteration_delay;
  *iteration_delay= sns_ptr->sns_intr_data.tilt_data.config_data.iteration_delay;
  *has_max_init_trial_num= sns_ptr->sns_intr_data.tilt_data.config_data.has_max_init_trial_num;
  *max_init_trial_num= sns_ptr->sns_intr_data.tilt_data.config_data.max_init_trial_num;
  *has_max_update_trial_num= sns_ptr->sns_intr_data.tilt_data.config_data.has_max_update_trial_num; 
  *max_update_trial_num= sns_ptr->sns_intr_data.tilt_data.config_data.max_update_trial_num;
  *has_init_gravity_threshold= sns_ptr->sns_intr_data.tilt_data.config_data.has_init_gravity_threshold; 
  *init_gravity_threshold= sns_ptr->sns_intr_data.tilt_data.config_data.init_gravity_threshold;
  *has_update_gravity_threshold= sns_ptr->sns_intr_data.tilt_data.config_data.has_update_gravity_threshold;
  *update_gravity_threshold = sns_ptr->sns_intr_data.tilt_data.config_data.update_gravity_threshold;

  return TRUE;

}
 
 /*===========================================================================
 FUNCTION cm_sns_get_fmd_config
 
 DESCRIPTION
   get the FMD config to send the sensor
 
 DEPENDENCIES
   none
 
 RETURN VALUE
   boolean
 
 SIDE EFFECTS
   none
 
 ===========================================================================*/

boolean cm_sns_get_tilt_gravity_vector(boolean *init_success,  float *gravity_init_vector_x,float *gravity_init_vector_y,float *gravity_init_vector_z)
{
  cm_sns_data_collection_s_type*	 sns_ptr = cmsns_get_data_info_ptr();

  CM_ASSERT(sns_ptr != NULL);

  if(sns_ptr->sns_intr_data.tilt_data.has_gravity_updated== FALSE || sns_ptr->sns_intr_data.tilt_data.is_enable == FALSE)
  {
    return FALSE;
  }
  *init_success = sns_ptr->sns_intr_data.tilt_data.tilt_gravity_data.gravity_init_success;
  *gravity_init_vector_x  = sns_ptr->sns_intr_data.tilt_data.tilt_gravity_data.gravity_init_vector_x;
  *gravity_init_vector_y = sns_ptr->sns_intr_data.tilt_data.tilt_gravity_data.gravity_init_vector_y;
  *gravity_init_vector_z = sns_ptr->sns_intr_data.tilt_data.tilt_gravity_data.gravity_init_vector_z;

  return TRUE;
}
 
 /*===========================================================================
FUNCTION cm_sns_get_fmd_config

DESCRIPTION
  get the FMD config to send the sensor

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
void cm_sns_set_tilt_gravity_vector(boolean init_success,  float gravity_init_vector_x,float gravity_init_vector_y,float gravity_init_vector_z)
{
  cm_sns_data_collection_s_type*	 sns_ptr = cmsns_get_data_info_ptr();
  
  CM_ASSERT(sns_ptr != NULL);
  sns_ptr->sns_intr_data.tilt_data.tilt_gravity_data.gravity_init_success = init_success;;
  sns_ptr->sns_intr_data.tilt_data.tilt_gravity_data.gravity_init_vector_x = gravity_init_vector_x ;
  sns_ptr->sns_intr_data.tilt_data.tilt_gravity_data.gravity_init_vector_y = gravity_init_vector_y;
  sns_ptr->sns_intr_data.tilt_data.tilt_gravity_data.gravity_init_vector_z =  gravity_init_vector_z;
  sns_ptr->sns_intr_data.tilt_data.has_gravity_updated = TRUE;
}
#endif

/*===========================================================================
FUNCTION cmpsm_btw_edrx_active

DESCRIPTION
  Check if PSM btw eDRX is active

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

extern boolean cm_send_sns_req(cm_sns_id_e_type sensor_type);




/*===========================================================================
FUNCTION cm_sns_update_sample_buffer_per_sid

DESCRIPTION
  Call by the sensor callback, to process the sensors sample data in CM context

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_update_sample_buffer_per_sid(cm_sns_id_e_type sensor_id,float sample_arr[],int len,uint64 timestamp)
{
  CM_MSG_HIGH_2("  Sample for sensor = %d and len is %d",sensor_id,len);
  cmsns_snsmod_send_sns_sample_rsp(sensor_id,sample_arr,len,timestamp);
}

#ifndef FEATURE_MMODE_TX_21
/*===========================================================================
FUNCTION cm_sns_enable_motion_detect_intr

DESCRIPTION
 Handle the motion detect intr

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/


void cm_sns_enable_motion_detect_intr(void)
{
    cm_sns_data_collection_s_type*   sns_ptr = cmsns_get_data_info_ptr();
   if(cm_sns_check_if_motion_possible(sns_ptr) == TRUE)
    {
      cm_handle_md_interrupt(TRUE);
    }

    else
    {
      cm_handle_md_interrupt(FALSE);
    }

}
#endif
/*===========================================================================
FUNCTION cm_sns_update_motion_rsp

DESCRIPTION
  call by sensor callback API, to send response in CM context

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_update_motion_rsp(cm_sns_motion_event_e_type event)
{
  event_debug = event;
  CM_MSG_HIGH_1("  MOTION sensor response type = %d ",event);
  cmsns_snsmod_send_motion_sns_rsp(event,0);

}

/*===========================================================================
FUNCTION cm_sns_update_mcd_rsp

DESCRIPTION
  call by mcd sensor callback API, to send response in CM context

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cm_sns_update_mcd_rsp(cm_sns_motion_event_e_type event, uint32 psm_time)

{
  event_debug = event;
  cmsns_snsmod_send_motion_sns_rsp(event,psm_time);
  
}


/*===========================================================================
FUNCTION cm_sns_copy_sns_sample_data

DESCRIPTION
  copy the sample data from sensors global to the send the response to the app

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/



void cm_sns_copy_sns_sample_data(cm_qmi_sensor_data_rsp_s_type dest_sample_arr[],uint8 *len)
{
  cm_sns_data_collection_s_type*   sns_ptr = cmsns_get_data_info_ptr();
  int index = 0;
  uint32 size=0;
  int temp_len=0;
  CM_ASSERT(sns_ptr != NULL);
  /* copy the sample data from global to dest array */
  for(index = 0; index < sns_ptr->no_of_sns_enabled; index++)
  {
    if(sns_ptr->sns_data_arr[index].current_sns_state == CM_SNS_STATE_APP_SENT_DATA_REQ)
    {
      CM_MSG_HIGH_1("[DEBUG] Sensor id = %d",sns_ptr->sns_data_arr[index].sns_config_data.sns_id);
      dest_sample_arr[temp_len].batch_count = sns_ptr->sns_data_arr[index].current_batch_count;
      dest_sample_arr[temp_len].sensor_id = sns_ptr->sns_data_arr[index].sns_config_data.sns_id;
      size = sizeof(cm_sns_sample_data_s_type)  * sns_ptr->sns_data_arr[index].current_batch_count;
      memscpy(dest_sample_arr[temp_len].sample_batch,size,sns_ptr->sns_data_arr[index].sns_sample_data,size);
#ifdef FEATURE_MMODE_TX_21
      dest_sample_arr[temp_len].event_type = sns_ptr->sns_data_arr[index].event_type;
	  dest_sample_arr[temp_len].sensors_state = sns_ptr->sns_data_arr[index].algorithm_state;
      dest_sample_arr[temp_len].TTC = sns_ptr->sns_data_arr[index].TTC;
      dest_sample_arr[temp_len].slope = sns_ptr->sns_data_arr[index].slope;
	  dest_sample_arr[temp_len].high_perf_measurement_period = sns_ptr->sns_data_arr[index].sns_config_data.sns_high_perf_interval;
#endif
      temp_len++;    
    }  
  }
  // update the actual length
  *len = temp_len;

}


/*===========================================================================
FUNCTION cm_sns_copy_sns_logging_data

DESCRIPTION
  copy the sample data from sensors global to the send the response to the app

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_MMODE_TX_21
void cm_sns_copy_sns_logging_data(cm_qmi_sensor_data_rsp_s_type dest_sample_arr[],uint8 *len, cm_sns_data_req_info_s_type *  data_req_ptr)
{
  cm_sns_data_collection_s_type*   sns_ptr = cmsns_get_data_info_ptr();
  int index = 0;
  uint32 size=0;
  int temp_len=0;
  CM_ASSERT(sns_ptr != NULL);
  /* copy the sample data from global to dest array */

  for(index = 0; index < sns_ptr->no_of_sns_enabled; index++)
  {
    if((sns_ptr->sns_data_arr[index].sns_config_data.operating_mode == TRUE) && (data_req_ptr->sensor_id_mask & (1 << sns_ptr->sns_data_arr[index].sns_config_data.sns_id )))
    {
      CM_MSG_HIGH_1("[DEBUG] Sensor id = %d",sns_ptr->sns_data_arr[index].sns_config_data.sns_id);
      dest_sample_arr[temp_len].batch_count = sns_ptr->sns_data_arr[index].current_batch_count;
      dest_sample_arr[temp_len].sensor_id = sns_ptr->sns_data_arr[index].sns_config_data.sns_id;
      size = sizeof(cm_sns_sample_data_s_type)  * sns_ptr->sns_data_arr[index].current_batch_count;
      memscpy(dest_sample_arr[temp_len].sample_batch,size,sns_ptr->sns_data_arr[index].sns_sample_data,size);

      dest_sample_arr[temp_len].event_type = sns_ptr->sns_data_arr[index].event_type;
	  dest_sample_arr[temp_len].sensors_state = sns_ptr->sns_data_arr[index].algorithm_state;
      dest_sample_arr[temp_len].TTC = sns_ptr->sns_data_arr[index].TTC;
      dest_sample_arr[temp_len].slope = sns_ptr->sns_data_arr[index].slope;
	  dest_sample_arr[temp_len].high_perf_measurement_period = sns_ptr->sns_data_arr[index].sns_config_data.sns_high_perf_interval;

      temp_len++;    
    }  
  }
  // update the actual length
  *len = temp_len;
}
#endif
/*===========================================================================
FUNCTION cm_sns_flush_sensor_data

DESCRIPTION
   it flush the sensors data, which has been queried

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_flush_sensor_data(cm_sns_data_collection_s_type* sns_ptr)
{
  int index = 0;
  CM_ASSERT(sns_ptr != NULL);
  
  for(index =0;index < sns_ptr->no_of_sns_enabled;index++)
  {
    if(sns_ptr->sns_data_arr[index].current_sns_state == CM_SNS_STATE_APP_SENT_DATA_REQ)   
    {
      memset(sns_ptr->sns_data_arr[index].sns_sample_data,0,CM_MAX_BATCH_SUPPORTED * sizeof(cm_sns_sample_data_s_type));
      sns_ptr->sns_data_arr[index].current_batch_count = 0;
    }
  }
}



/*===========================================================================
FUNCTION cm_sns_reset_cond_met_sensors_state

DESCRIPTION
  update the sesnors state if APP has queried or indicated move to none

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cm_sns_reset_cond_met_sensors_state(cm_sns_data_collection_s_type* sns_ptr)
{

  int index = 0;
  CM_ASSERT(sns_ptr != NULL);

  
  for(index =0;index < sns_ptr->no_of_sns_enabled;index++)
  {
    if(sns_ptr->sns_data_arr[index].current_sns_state == CM_SNS_STATE_APP_SENT_DATA_REQ ||
           sns_ptr->sns_data_arr[index].current_sns_state == CM_SNS_STATE_IND_SENT_TO_APP)
    {
      cm_sns_update_sns_current_state(sns_ptr,CM_SNS_STATE_NONE,index);
      sns_ptr->sns_data_arr[index].current_batch_count = 0;
       /*It is safe to set poilcy mask now  */
  
    }
  }
  sns_ptr->policy_met_mask = 0;
  /*It is safe to set poilcy mask now  */
  sns_ptr->batch_full_mask = 0;
  sns_ptr->sns_fail_mask = 0;
  sns_ptr->sns_odr_data.fmd_data.flight_status = CM_SNS_FMD_STATUS_NONE;

}

void cm_sns_log_config_element(cm_sns_config_data_s_type  data)
{
   CM_MSG_HIGH_3("sns id = %d sns_interval = %d batch cnt = %d ",data.sns_id,data.sns_interval,data.sns_max_batch_cnt);
}

void cm_sns_log_cm_sns_config(void)
{
   int index = 0;
   cm_sns_data_collection_s_type*   sns_ptr = cmsns_get_data_info_ptr();
   CM_ASSERT(sns_ptr != NULL);

   for(index = 0; index < sns_ptr->no_of_sns_enabled;index++)
   {
     cm_sns_log_config_element(sns_ptr->sns_data_arr[index].sns_config_data);
   }
}

#ifndef FEATURE_BUILD_SENSORONLY_IMAGE

/*===========================================================================
FUNCTION cm_sns_process_data_request_cmd

DESCRIPTION
  Process sensors data request command 

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_process_data_request_cmd(cm_sns_cmd_s_type* sns_cmd_ptr)
{
  int index = 0;
  boolean res  = TRUE;
  cm_sns_id_e_type temp_sns_id;
  cm_sns_data_req_info_s_type *  data_req_ptr = &(sns_cmd_ptr->info.cmd_s.sns_data_req);
  cm_sns_data_collection_s_type*   sns_ptr = cmsns_get_data_info_ptr();

  CM_ASSERT(data_req_ptr != NULL);
  CM_ASSERT(sns_ptr != NULL);
  CM_ASSERT(sns_cmd_ptr != NULL);
  CM_ASSERT(sns_cmd_ptr->cmd == CM_SNS_CMD_DATA_REQ );
  
#ifdef FEATURE_MMODE_TX_21
  if(data_req_ptr->is_logging_mode == TRUE)
  {
    res = cmsns_send_app_logging_data_rsp(data_req_ptr);
    if(res == FALSE)
    {
       sns_cmd_ptr->cmd_cb_func(sns_cmd_ptr->cmd ,CM_SNS_CMD_ERR_NO_MEMORY);
    }
    return ;
  }
#endif   
  for(temp_sns_id = CM_SNS_TYPE_ALS;temp_sns_id <= CM_SNS_TYPE_ENV_SENSOR_MAX ; temp_sns_id++)
  {
    if((data_req_ptr->sensor_id_mask & (1 << temp_sns_id )))
    {
      for(index =0;index < sns_ptr->no_of_sns_enabled; index++)
      {
        if( sns_ptr->sns_data_arr[index].sns_config_data.sns_id == temp_sns_id )
        {
          CM_MSG_HIGH_1("  APP sent data request for sns id = %d ",temp_sns_id);
          cm_sns_update_sns_current_state(sns_ptr,CM_SNS_STATE_APP_SENT_DATA_REQ,index);
          break;
        }
      }
    }
  } 
  /* send sns data rsp  */
  res = cmsns_send_app_sample_data_rsp();
  
  if(res == FALSE)
  {
    sns_cmd_ptr->cmd_cb_func(sns_cmd_ptr->cmd ,CM_SNS_CMD_ERR_NO_MEMORY);
  }

  
    /* flush all the data that batch is met or policy met 
    or it has been queried by APP*/
    cm_sns_flush_sensor_data(sns_ptr);
    /*  mark it as begining of all sensor which has been flushed
    */
    cm_sns_reset_cond_met_sensors_state(sns_ptr);
	  /* Now run state machine   */
    cmsns_mgr_run_state_machine();

 
}


/*===========================================================================
FUNCTION cm_sns_process_config_cmd

DESCRIPTION
  process sesnors configuration command

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_process_config_cmd(cm_sns_cmd_s_type* sns_cmd_ptr)
{

  int index = 0;
  uint32 current_time = 0;
#ifdef FEATURE_MMODE_TX_21
  uint8 sensor_id;
#endif
  cm_sns_config_s_type* sns_config_req_ptr = &(sns_cmd_ptr->info.cmd_s.sns_config_req);
  cm_sns_data_collection_s_type*   sns_ptr = cmsns_get_data_info_ptr();
  #ifdef FEATURE_MMODE_TX_21
   cm_sns_reg_config_s_type *config_ptr = get_reg_config();
  #endif
  CM_ASSERT(sns_cmd_ptr != NULL);
  CM_ASSERT(sns_cmd_ptr->cmd == CM_SNS_CMD_SET_CONFIG);
  CM_ASSERT(sns_config_req_ptr != NULL);

  CM_ASSERT(sns_ptr != NULL);
  
#ifdef FEATURE_MMODE_TX_21
 
  if(((sns_config_req_ptr->non_env_sns_mask & (1<<CM_SNS_TYPE_ALS)) !=0) ||
 	 ((sns_config_req_ptr->non_env_sns_mask & (1<<CM_SNS_TYPE_FMD)) !=0)||
	 ((sns_config_req_ptr->non_env_sns_mask & (1<<CM_SNS_TYPE_FALL_DETECT)) !=0) ||
 ((sns_config_req_ptr->non_env_sns_mask & (1<<CM_SNS_TYPE_TILT)) !=0) ||
 ((sns_config_req_ptr->non_env_sns_mask & (1<<CM_SNS_TYPE_SMD)) !=0)  ||
  ((sns_config_req_ptr->non_env_sns_mask & (1<<CM_SNS_TYPE_MOTION_DETECT)) !=0) ||
  ((sns_config_req_ptr->non_env_sns_mask & (1<<CM_SNS_TYPE_MCD)) !=0)
	 )
  {
    
     if(((sns_config_req_ptr->non_env_sns_mask & (1<<CM_SNS_TYPE_ALS)) !=0))
     {
        current_time =  time_get_secs(); 
	if(sns_ptr->sns_intr_data.als_data.is_enable == TRUE && 
	   	   sns_config_req_ptr->als_config.is_enable == FALSE)
       {
         cm_disable_sns_stream_by_sensor_id(CM_SNS_TYPE_ALS);
       }
       sns_ptr->sns_intr_data.als_data.is_enable = sns_config_req_ptr->als_config.is_enable;
       if(sns_config_req_ptr->als_config.threshold> 3)
       {
         sns_ptr->sns_intr_data.als_data.threshold = sns_config_req_ptr->als_config.threshold;
	}
	else 
        {
	  sns_ptr->sns_intr_data.als_data.threshold = 3;
	}
	   sns_ptr->sns_intr_data.als_data.interval  = sns_config_req_ptr->als_config.interval;
	   sns_ptr->sns_intr_data.als_data.als_sns_uptime = current_time  + sns_config_req_ptr->als_config.interval;
	   
      }

	 if(((sns_config_req_ptr->non_env_sns_mask & (1<<CM_SNS_TYPE_FALL_DETECT)) !=0))
	 {
		 
	   if(sns_ptr->sns_intr_data.fall_data.is_enable == TRUE && 
	   	  sns_config_req_ptr->fall_config.is_enable == FALSE)
       {
         cm_disable_sns_stream_by_sensor_id(CM_SNS_TYPE_FALL_DETECT);
       }
	    sns_ptr->sns_intr_data.fall_data.is_enable = sns_config_req_ptr->fall_config.is_enable;
	 }

	 if(((sns_config_req_ptr->non_env_sns_mask & (1<<CM_SNS_TYPE_FMD)) !=0))
	 {
	   sns_ptr->sns_odr_data.fmd_data.is_enable = sns_config_req_ptr->fmd_config.is_enable;
   if(sns_config_req_ptr->fmd_config.accel_buffer_count != 0xFFFFFFFF
    || sns_config_req_ptr->fmd_config.run_duration != 0xFFFFFFFF 
    || sns_config_req_ptr->fmd_config.accel_threshold != 0xFFFFFFFF)
    {
       sns_ptr->sns_registry_mask =  sns_ptr->sns_registry_mask | ((1<<CM_SNS_TYPE_FMD));
       config_ptr->fmd_config_reg.accel_buffer_count = sns_config_req_ptr->fmd_config.accel_buffer_count ; 
       config_ptr->fmd_config_reg.threshold_accel = sns_config_req_ptr->fmd_config.accel_threshold ;
       config_ptr->fmd_config_reg.run_period = sns_config_req_ptr->fmd_config.run_duration ;
       cm_send_sns_registry_req(CM_SNS_TYPE_FMD);
    }
	 }

	 if(((sns_config_req_ptr->non_env_sns_mask & (1<<CM_SNS_TYPE_TILT)) !=0))
	 {
	   if(sns_ptr->sns_intr_data.tilt_data.is_enable == TRUE && 
	   	  sns_config_req_ptr->tilt_config.is_enable == FALSE)
       {
         cm_disable_sns_stream_by_sensor_id(CM_SNS_TYPE_TILT);
       }
       sns_ptr->sns_intr_data.tilt_data.is_enable = sns_config_req_ptr->tilt_config.is_enable;	   
	   sns_ptr->sns_intr_data.tilt_data.tilt_sns_uptime = sns_config_req_ptr->tilt_config.iteration_delay + time_get_secs(); 
   if(sns_config_req_ptr->tilt_config.iteration_delay != 0xFFFFFFFF
      || sns_config_req_ptr->tilt_config.update_gravity_threshold != 0xFFFFFFFF
      || sns_config_req_ptr->tilt_config.max_init_trial_num != 0xFF
      || sns_config_req_ptr->tilt_config.max_update_trial_num != 0xFF
      || sns_config_req_ptr->tilt_config.init_gravity_threshold != 0xFFFFFFFF
      || sns_config_req_ptr->tilt_config.transportation_type != 0
       )
    {
        sns_ptr->sns_registry_mask =  sns_ptr->sns_registry_mask | ((1<<CM_SNS_TYPE_TILT));
        config_ptr->tilt_tracker_config_reg.iteration_delay = sns_config_req_ptr->tilt_config.iteration_delay;
        config_ptr->tilt_tracker_config_reg.update_gravity_threshold = sns_config_req_ptr->tilt_config.update_gravity_threshold;
        config_ptr->tilt_tracker_config_reg.max_init_trial_num = sns_config_req_ptr->tilt_config.max_init_trial_num ; 
        config_ptr->tilt_tracker_config_reg.max_update_trial_num    = sns_config_req_ptr->tilt_config.max_update_trial_num ;
        config_ptr->tilt_tracker_config_reg.init_gravity_threshold = sns_config_req_ptr->tilt_config.init_gravity_threshold;
        config_ptr->tilt_tracker_config_reg.gravity_init_success = sns_config_req_ptr->tilt_config.redo_gravity_init;
        config_ptr->tilt_tracker_config_reg.transportation_type = (cm_sns_tilt_tracker_config_transportation_type)sns_config_req_ptr->tilt_config.transportation_type;
         cm_send_sns_registry_req(CM_SNS_TYPE_TILT);
    }         
	 }

if(((sns_config_req_ptr->non_env_sns_mask & (1<<CM_SNS_TYPE_MCD)) !=0))
{
sns_ptr->motion_sns_data.mcd_enable = sns_config_req_ptr->mcd_config.is_enable;
if(sns_config_req_ptr->mcd_config.mcd_motion_toggle_counter_threshold != 0xFF
  || sns_config_req_ptr->mcd_config.mcd_motion_toggle_delay_threshold != 0xFFFF
  || sns_config_req_ptr->mcd_config.mcd_psm_timer_config != 0xFF)
{
  sns_ptr->sns_registry_mask =  sns_ptr->sns_registry_mask | ((1<<CM_SNS_TYPE_MCD));
  config_ptr->mcd_config_reg.mcd_motion_toggle_counter_threshold = sns_config_req_ptr->mcd_config.mcd_motion_toggle_counter_threshold;
  config_ptr->mcd_config_reg.mcd_motion_toggle_delay_threshold = sns_config_req_ptr->mcd_config.mcd_motion_toggle_delay_threshold;
config_ptr->mcd_config_reg.mcd_psm_timer_config = sns_config_req_ptr->mcd_config.mcd_psm_timer_config ;
  CM_MSG_HIGH_1("  toggle delay threshold sns_config= %d ",sns_cmd_ptr->info.cmd_s.sns_config_req.mcd_config.mcd_motion_toggle_delay_threshold );
   cm_send_sns_registry_req(CM_SNS_TYPE_MCD);
}
}

if(((sns_config_req_ptr->non_env_sns_mask & (1<<CM_SNS_TYPE_MOTION_DETECT)) !=0))
{
if(sns_config_req_ptr->md_config.md_tuning_threshold != 0xFFFFFFFF)
{
  sns_ptr->sns_registry_mask =  sns_ptr->sns_registry_mask | ((1<<CM_SNS_TYPE_MOTION_DETECT));
  config_ptr->md_config_reg.thresh = sns_config_req_ptr->md_config.md_tuning_threshold;
   cm_send_sns_registry_req(CM_SNS_TYPE_MOTION_DETECT);
}
}

if(((sns_config_req_ptr->non_env_sns_mask & (1<<CM_SNS_TYPE_SMD)) !=0))
{
if(sns_config_req_ptr->smd_config.smd_weight[0] != 0xFFFFFFFF
  || sns_config_req_ptr->smd_config.smd_perceptron_max_weight[0] != 0xFFFFFFFF
  || sns_config_req_ptr->smd_config.smd_perceptron_min_weight[0] != 0xFFFFFFFF
  || sns_config_req_ptr->smd_config.smd_sum_score_threshold != 0xFF)
{
  sns_ptr->sns_registry_mask =  sns_ptr->sns_registry_mask | ((1<<CM_SNS_TYPE_SMD));
  config_ptr->smd_config_reg.smd_sum_score_threshold = sns_config_req_ptr->smd_config.smd_sum_score_threshold;
  memcpy( config_ptr->smd_config_reg.smd_perceptron_min_weight, sns_config_req_ptr->smd_config.smd_perceptron_min_weight, 4*sizeof(float));
  memcpy( config_ptr->smd_config_reg.smd_perceptron_weight, sns_config_req_ptr->smd_config.smd_weight, 4*sizeof(float));
  memcpy( config_ptr->smd_config_reg.smd_perceptron_max_weight, sns_config_req_ptr->smd_config.smd_perceptron_max_weight, 4*sizeof(float));
  cm_send_sns_registry_req(CM_SNS_TYPE_SMD);
}
}


}

#endif
  if(sns_config_req_ptr->len !=0)
  {
   CM_ASSERT(INRANGE(sns_config_req_ptr->len,CM_SNS_TYPE_ALS,CM_SNS_TYPE_ENV_SENSOR_MAX));
  current_time =  time_get_secs(); 

#ifndef FEATURE_MMODE_TX_21
  /********** RESET OLD DATA if any *****************************/
  cm_sns_reset_current_config(sns_ptr);
  cm_sns_update_psm_state(sns_ptr,CM_SNS_PSM_READY_RSP_NONE);
  cm_sns_set_context_change(TRUE);
  /****** now we can allocate ******/
  
  sns_ptr->sns_data_arr = (cm_sns_data_s_type *)cm_mem_malloc(sns_config_req_ptr->len * sizeof(cm_sns_data_s_type) );
  CM_ASSERT(sns_ptr->sns_data_arr != NULL);
  memset(sns_ptr->sns_data_arr,0,sns_config_req_ptr->len * sizeof(cm_sns_data_s_type) );
  sns_ptr->no_of_sns_enabled = sns_config_req_ptr->len;
  for(index = 0;index < sns_config_req_ptr->len ; index++)
  {
    CM_ASSERT(INRANGE(sns_config_req_ptr->sns_arr[index].sns_id,CM_SNS_TYPE_ALS,CM_SNS_TYPE_ENV_SENSOR_MAX));
      
   /* In case the the batch count is zero consider it max */
    if(sns_config_req_ptr->sns_arr[index].sns_max_batch_cnt == 0)
    {
        sns_config_req_ptr->sns_arr[index].sns_max_batch_cnt = CM_MAX_BATCH_SUPPORTED;
    }
  
    CM_ASSERT(INRANGE(sns_config_req_ptr->sns_arr[index].sns_max_batch_cnt,1,CM_MAX_BATCH_SUPPORTED));
     /*  copy the whole config data  */
    sns_ptr->sns_data_arr[index].sns_config_data = sns_config_req_ptr->sns_arr[index];
     /* Update the uptime  */
    sns_ptr->sns_data_arr[index].sns_uptime =  current_time + sns_ptr->sns_data_arr[index].sns_config_data.sns_interval;
     /* Update the sensor state  */
    sns_ptr->sns_data_arr[index].current_sns_state = CM_SNS_STATE_NONE;
	CM_MSG_HIGH_1("config passed for index = %d ",index);
	cm_sns_log_config_element(sns_config_req_ptr->sns_arr[index]);
    
  }

#else
  if(sns_ptr->sns_data_arr == NULL)
  {
     sns_ptr->sns_data_arr = (cm_sns_data_s_type *)cm_mem_malloc(CM_MAX_SNS_ENABLED * sizeof(cm_sns_data_s_type) );
	 CM_ASSERT(sns_ptr->sns_data_arr != NULL);
	 memset(sns_ptr->sns_data_arr,0,CM_MAX_SNS_ENABLED * sizeof(cm_sns_data_s_type) );
	 sns_ptr->no_of_sns_enabled = CM_MAX_SNS_ENABLED;
     sns_ptr->sns_data_arr[0].sns_config_data.sns_id = CM_SNS_TYPE_PRESSURE; //2
     sns_ptr->sns_data_arr[1].sns_config_data.sns_id = CM_SNS_TYPE_HUMIDITY; //3
     sns_ptr->sns_data_arr[2].sns_config_data.sns_id = CM_SNS_TYPE_TEMPERATURE; // 4
     sns_ptr->sns_data_arr[0].sns_uptime =  0xFFFFFFFF;
	 sns_ptr->sns_data_arr[1].sns_uptime =  0xFFFFFFFF;
	 sns_ptr->sns_data_arr[2].sns_uptime =  0xFFFFFFFF;
  }
   for(index = 0;index < sns_config_req_ptr->len ; index++)
   {
    CM_ASSERT(INRANGE(sns_config_req_ptr->sns_arr[index].sns_id,CM_SNS_TYPE_PRESSURE,CM_SNS_TYPE_ENV_SENSOR_MAX));
    for(sensor_id =0; sensor_id <CM_MAX_SNS_ENABLED;sensor_id++   )
    {
     
	  if(sns_ptr->sns_data_arr[sensor_id].sns_config_data.sns_id == 
	  	               sns_config_req_ptr->sns_arr[index].sns_id)
	  	{
	  	  if(sns_config_req_ptr->sns_arr[index].sns_max_batch_cnt == 0)
          {
             sns_config_req_ptr->sns_arr[index].sns_max_batch_cnt = CM_MAX_BATCH_SUPPORTED;
          }
  
          CM_ASSERT(INRANGE(sns_config_req_ptr->sns_arr[index].sns_max_batch_cnt,1,CM_MAX_BATCH_SUPPORTED));
          /*  copy the whole config data  */

          sns_ptr->sns_data_arr[sensor_id].sns_config_data = sns_config_req_ptr->sns_arr[index];

		  
            /* Update the uptime  */
          sns_ptr->sns_data_arr[sensor_id].sns_uptime =  current_time + sns_ptr->sns_data_arr[sensor_id].sns_config_data.sns_interval;
            /* Update the sensor state  */
          sns_ptr->sns_data_arr[sensor_id].current_sns_state = CM_SNS_STATE_NONE;
       	  CM_MSG_HIGH_2("config passed for sensor_index = %d req_index =%d",sensor_id,index);
		  CM_MSG_HIGH_1("sns_uptime %d ", sns_ptr->sns_data_arr[sensor_id].sns_uptime );
       	  cm_sns_log_config_element(sns_config_req_ptr->sns_arr[index]);
          cm_sns_log_config_element(sns_ptr->sns_data_arr[sensor_id].sns_config_data);
 
          break;
		}
	  }
   }

  
    
#endif 

  CM_MSG_HIGH_0("Global data update is ");
}
  cm_sns_log_cm_sns_config();
  cmsns_mgr_run_state_machine();

}



void cm_sns_get_registry_config (cm_sns_cmd_s_type* sns_cmd_ptr)
{
   cm_send_registry_read_req_wrapper(sns_cmd_ptr->info.cmd_s.sns_registry_config.sensor_id);
}
/*===========================================================================
FUNCTION cm_sns_process_reset_config_cmd

DESCRIPTION
  process sesnors reset configuration command

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_process_reset_config_cmd(cm_sns_cmd_s_type* sns_cmd_ptr)
{
 
  cm_sns_data_collection_s_type*   sns_ptr = cmsns_get_data_info_ptr();
  CM_ASSERT(sns_cmd_ptr != NULL);
  CM_ASSERT(sns_cmd_ptr->cmd == CM_SNS_CMD_SET_CONFIG);
  CM_ASSERT(sns_ptr != NULL);
  cm_sns_reset_current_config(sns_ptr);

}

/*===========================================================================
FUNCTION cm_sns_process_od_sns_req_cmd

DESCRIPTION
  process sesnors OD request command 
DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_process_od_sns_req_cmd(cm_sns_cmd_s_type* sns_cmd_ptr)
{
 
  cm_sns_data_collection_s_type*   sns_ptr = cmsns_get_data_info_ptr();
  CM_ASSERT(sns_cmd_ptr != NULL);
  CM_ASSERT(sns_cmd_ptr->cmd == CM_SNS_CMD_OD_SNS_REQ);
  CM_ASSERT(sns_ptr != NULL);
  if((sns_cmd_ptr->info.cmd_s.sns_od_sns_req.sensor_id_mask & (1<<CM_SNS_TYPE_FMD)) != 0)
  {
     if(sns_ptr->sns_odr_data.fmd_data.is_enable == TRUE)
     {
       cm_send_sns_req(CM_SNS_TYPE_FMD);
     }
  }

}
#ifdef FEATURE_MMODE_TX_21
/*===========================================================================
FUNCTION cm_sns_process_phy_test_req_cmd

DESCRIPTION
  process sesnors test request command 
DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_process_phy_test_req_cmd(cm_sns_cmd_s_type* sns_cmd_ptr)
{
 cm_sns_phy_test_config_s_type* config = cm_sns_get_phy_test_config();

 config->cm_sns_phy_test_req = TRUE;
 config->cm_sns_test_type = sns_cmd_ptr->info.cmd_s.sns_phy_test.phy_sns_test;
 cm_send_sns_req(sns_cmd_ptr->info.cmd_s.sns_phy_test.sensor_id);
}

/*===========================================================================
FUNCTION cm_sns_process_phy_test_get_data_req_cmd

DESCRIPTION
  process sesnors test request command 
DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_process_phy_test_get_data_req_cmd(cm_sns_cmd_s_type* sns_cmd_ptr)
{
 cm_sns_phy_test_config_s_type* config = cm_sns_get_phy_test_config();
 if(sns_cmd_ptr->info.cmd_s.sns_phy_test_get_data.is_enabled == TRUE)
  {
   config->cm_sns_phy_test_get_data_req = TRUE;
   config->sensor_type = sns_cmd_ptr->info.cmd_s.sns_phy_test_get_data.sensor_id;
   config->sample_rate = sns_cmd_ptr->info.cmd_s.sns_phy_test_get_data.sample_rate;
   config->report_rate = sns_cmd_ptr->info.cmd_s.sns_phy_test_get_data.report_rate;
   cm_send_sns_req(sns_cmd_ptr->info.cmd_s.sns_phy_test_get_data.sensor_id);
  }
 else
  {
   config->cm_sns_phy_test_get_data_req = FALSE;
   cm_disable_sns_stream_by_sensor_id(sns_cmd_ptr->info.cmd_s.sns_phy_test_get_data.sensor_id);
  }
}

/*===========================================================================
FUNCTION cm_sns_set_logging_level_cmd

DESCRIPTION
  Set logging level
DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_set_logging_level_cmd(cm_sns_cmd_s_type* sns_cmd_ptr)
{
 cm_sns_data_collection_s_type*   sns_ptr = cmsns_get_data_info_ptr();
 sns_ptr->cm_sns_logging_level = sns_cmd_ptr->info.cmd_s.sns_logging_level;
}

/*===========================================================================
FUNCTION cm_sns_process_get_serial_num

DESCRIPTION
  process sesnors test request command 
DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_process_get_serial_num(cm_sns_cmd_s_type* sns_cmd_ptr)
{
  cm_sns_phy_test_config_s_type* config = cm_sns_get_phy_test_config();
  CM_MSG_HIGH_1("Sending suid req for serial number of sns = %d", sns_cmd_ptr->info.cmd_s.sns_serial_num_req.sensor_id );
  config->serial_number_req = TRUE;
  cm_send_sns_req(sns_cmd_ptr->info.cmd_s.sns_serial_num_req.sensor_id );

}

#endif
#ifndef FEATURE_MMODE_TX_21
/*===========================================================================
FUNCTION cm_sns_handle_motion_sensor_config_req

DESCRIPTION
  process motion sensor config command

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void  cm_sns_handle_motion_sensor_config_req(boolean motion_enable)
{
  cm_sns_data_collection_s_type*	 sns_ptr = cmsns_get_data_info_ptr();
  CM_ASSERT(sns_ptr != NULL);
  CM_MSG_HIGH_2(" The motion config enable = %d and rsp state = %d ",
  	                                motion_enable,sns_ptr->sns_ready_rsp_state );
  if((sns_ptr->sns_ready_rsp_state == CM_SNS_PSM_ENTER_RSP_SENT) ||
  	 (sns_ptr->sns_ready_rsp_state == CM_SNS_PSM_ENTER_REQ_STARTED))
  {
    return;
  }
  
  sns_ptr->motion_sns_data.motion_detect_enable = motion_enable;
  cm_sns_set_context_change(TRUE);  
}

#endif
#endif
/*===========================================================================
FUNCTION cm_sns_check_for_policy_cond_met

DESCRIPTION
  Check if policy cond met for sensors

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

/* This will check if policy condition met for the current sample  */
boolean  cm_sns_check_for_policy_cond_met(cm_sns_data_collection_s_type*	 sns_ptr,int count,int batch_no)
{
  int index = 0;
  cm_sns_config_data_s_type * sns_config_data = &(sns_ptr->sns_data_arr[count].sns_config_data);
  cm_sns_sample_data_s_type *     current_sample = &(sns_ptr->sns_data_arr[count].sns_sample_data[batch_no]);  
  cm_sns_sample_data_s_type *     prev_sample = NULL;
  boolean result  = FALSE;
#ifdef FEATURE_MMODE_TX_21  
  meas_input* input;
algo_init_input *a_input;

int i=0;
  meas_output *output;
  uint32 current_time = time_get_secs();
   float *data = NULL;
 uint64 *time = NULL;
#endif
  CM_ASSERT(sns_ptr != NULL);
  CM_ASSERT(sns_config_data != NULL);
  CM_ASSERT(current_sample != NULL);
  
#ifndef FEATURE_MMODE_TX_21  
  for(index = 0; index < current_sample->dimension_data_len ; index++ )
  {
    /* always compare index by index value of policy to the sample values  */
    if( sns_config_data->sns_policy.sns_policy_elem[index].greater_value !=  0xFFFFFFFF &&
        current_sample->sns_dimension_data[index] >= sns_config_data->sns_policy.sns_policy_elem[index].greater_value )
    {
       sns_ptr->policy_met_mask = (sns_ptr->policy_met_mask |(1 << sns_config_data->sns_id));
       result = TRUE;
       break;
    }
    if( sns_config_data->sns_policy.sns_policy_elem[index].lower_value != 0xFFFFFFFF &&
        current_sample->sns_dimension_data[index] <= sns_config_data->sns_policy.sns_policy_elem[index].lower_value)
    {
       sns_ptr->policy_met_mask = (sns_ptr->policy_met_mask |(1 << sns_config_data->sns_id));
       result = TRUE;
       break;
    }
  }
    
  /* minimum change between two sensor data.
  For example, when the step is 10 and the first reading of temp is 65 and the second one is 80, 
  the condition is met. If the second reading is 70 the condition is not met */
  /* only go for delta if above condition won't met because these data are 
    per sensor and setting same sensor mask multiple time won't add any value*/
  if( result == FALSE &&  batch_no > 0 )
  {
    prev_sample = &(sns_ptr->sns_data_arr[count].sns_sample_data[batch_no -1]); 
    CM_ASSERT(prev_sample != NULL);
    
    for(index = 0; index < current_sample->dimension_data_len ; index++ )
    {

      if (sns_config_data->sns_policy.sns_policy_elem[index].delta_value != 0xFFFFFFFF)
      {
        /* if delta moves upper side  */
        if( current_sample->sns_dimension_data[index] >= ( prev_sample->sns_dimension_data[index] + sns_config_data->sns_policy.sns_policy_elem[index].delta_value) )
        {
          sns_ptr->policy_met_mask = (sns_ptr->policy_met_mask |(1 << sns_config_data->sns_id));
          result = TRUE;
          break;               
        }
        /* if delta moves lower side  */
        if( (current_sample->sns_dimension_data[index] + sns_config_data->sns_policy.sns_policy_elem[index].delta_value) <=  prev_sample->sns_dimension_data[index] )
        {
          sns_ptr->policy_met_mask = (sns_ptr->policy_met_mask |(1 << sns_config_data->sns_id));
          result = TRUE;
          break;               
        }    
      }
    }
  }
#else

	data = (float*)cm_mem_malloc(sizeof(float) * CM_MAX_BATCH_SUPPORTED);
	time = (uint64*)cm_mem_malloc(sizeof(uint64) * CM_MAX_BATCH_SUPPORTED);
	a_input = (algo_init_input*)cm_mem_malloc(sizeof(algo_init_input));
   for(index = 0; index < sns_ptr->no_of_sns_enabled; index++){
		memset(data,0, sizeof(float) * CM_MAX_BATCH_SUPPORTED);
		memset(time,0, sizeof(uint64) * CM_MAX_BATCH_SUPPORTED);
		for(i =0;i< sns_ptr->sns_data_arr[index].max_count; i++){
		  data[i]= sns_ptr->sns_data_arr[index].sns_sample_data[i].sns_dimension_data[0];
		  time[i] = sns_ptr->sns_data_arr[index].sns_sample_data[i].timestamp;
	  }
	 memset(a_input, 0,sizeof(algo_init_input));
	 a_input->sensor_type = sns_ptr->sns_data_arr[index].sns_config_data.sns_id;
	 a_input->operating_mode = sns_ptr->sns_data_arr[index].sns_config_data.operating_mode;
	 a_input->alarm_thr_high = sns_ptr->sns_data_arr[index].sns_config_data.sns_policy.sns_policy_elem->greater_value;
	 a_input->alarm_thr_low = sns_ptr->sns_data_arr[index].sns_config_data.sns_policy.sns_policy_elem->lower_value;
	 a_input->pre_alert_thr_high = sns_ptr->sns_data_arr[index].sns_config_data.sns_policy.sns_policy_elem->pre_greater_value;
	 a_input->pre_alert_thr_low = sns_ptr->sns_data_arr[index].sns_config_data.sns_policy.sns_policy_elem->pre_lower_value;
	 a_input->performance_mode = sns_ptr->sns_data_arr[index].sns_config_data.sns_performance_mode;
	 a_input->alarm_hysteresis_duration = sns_ptr->sns_data_arr[index].sns_config_data.sns_hysteresis_duration;
	 a_input->HPM_sampling_period = sns_ptr->sns_data_arr[index].sns_config_data.sns_high_perf_interval;
	 a_input->measurement_period = sns_ptr->sns_data_arr[index].sns_config_data.sns_interval;
	 a_input->data_buffer = data;
	 a_input->time_buffer = time;
	 a_input->hysteresis_counter = sns_ptr->sns_data_arr[index].sns_config_data.sns_hysteresis_count;
	 a_input->algorithm_state = sns_ptr->sns_data_arr[index].algorithm_state;
	 a_input->current_count = sns_ptr->sns_data_arr[index].current_batch_count - 1;
	 a_input->max_count = sns_ptr->sns_data_arr[index].max_count;
	 a_input->valid_buffer_count = sns_ptr->sns_data_arr[index].valid_buffer_count;
	 sns_env_aware_algo_init_interface(*a_input);
	 CM_MSG_HIGH_0("CMSNS sns_env_aware_algo_init_interface called");
	}
  
	cm_mem_free(data);
	data = NULL;
	cm_mem_free(time);
	time = NULL;
	cm_mem_free(a_input);
	a_input = NULL;

  input = (meas_input*)cm_mem_malloc(sizeof(meas_input));
  output = (meas_output*)cm_mem_malloc(sizeof(meas_output));

  input->sensor_type = sns_config_data->sns_id;
  input->sensor_sample = current_sample->sns_dimension_data[0];
  input->timestamp = current_sample->timestamp;  

  sns_env_aware_measurement_interface(*input,output);
  CM_MSG_HIGH_0("CMSNS sns_env_aware_measurement_interface ");
  CM_MSG_HIGH_2(" output->algorithm_state %d output->event_type %d",output->algorithm_state, output->event_type );
  CM_MSG_HIGH_1("requseted measurement period %d",output->requested_measurement_period);
  if(output ){
  if(output->algorithm_state == PRE_ALERT_LOW ||  output->algorithm_state == PRE_ALERT_HIGH)
  {
    if(output->event_type == PRE_ALERT_LOW ||  output->event_type == PRE_ALERT_HIGH)
    {
      sns_ptr->policy_met_mask = (sns_ptr->policy_met_mask |(1 << sns_config_data->sns_id));
	  CM_MSG_HIGH_0("Pre alert met, move to FULL");
      result = TRUE;
	}
	else
	{
	 CM_MSG_HIGH_0("Pre alert not met");
      result = FALSE;
	}
  }
  else if(output->algorithm_state == ALERT_LOW ||  output->algorithm_state == ALERT_HIGH)
  {
    if(output->event_type == ALERT_LOW ||  output->event_type == ALERT_HIGH)
    {
      sns_ptr->policy_met_mask = (sns_ptr->policy_met_mask |(1 << sns_config_data->sns_id));
      result = TRUE;
	   CM_MSG_HIGH_0("Alert met, move to FULL");
	}
	else
	{
	 CM_MSG_HIGH_0("alert not met");
      result = FALSE;
	}
  
  }
  else
  {
    CM_MSG_HIGH_0("no policy met");
     result = FALSE;
  }

  if(result == TRUE)
  {
    if(sns_ptr->sns_intr_data.tilt_data.is_enable == TRUE)
    {
      sns_ptr->sns_intr_data.tilt_data.need_to_enqueue = TRUE;
    }
  }

  
  for(index = 0; index < sns_ptr->no_of_sns_enabled; index++)
  { 
    if(sns_ptr->sns_data_arr[index].sns_config_data.sns_id == input->sensor_type)
    {
      sns_ptr->sns_data_arr[index].algorithm_state = output->algorithm_state;
	  sns_ptr->sns_data_arr[index].event_type = output->event_type;
  sns_config_data->sns_hysteresis_count = output->hysteresis_counter;
  sns_ptr->sns_data_arr[index].valid_buffer_count = output->valid_buffer_count;
  sns_ptr->sns_data_arr[index].sns_uptime = current_time + output->requested_measurement_period;
  sns_ptr->sns_data_arr[index].slope = output->slope;
  sns_ptr->sns_data_arr[index].TTC = output->TTC;
    }
  }
 }

  cm_mem_free(input);
  input = NULL;
  cm_mem_free(output);
  output = NULL;
#endif

  CM_MSG_HIGH_1("measurement result %d", result);

  return result;	
}





/*===========================================================================
FUNCTION cm_sns_check_for_batch_full_cond_met

DESCRIPTION
  check if batch full condition met

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_check_for_batch_full_cond_met(cm_sns_data_collection_s_type*	 sns_ptr,int index)
{

  cm_sns_config_data_s_type * sns_config_data = &(sns_ptr->sns_data_arr[index].sns_config_data);
  cm_sns_data_s_type *     sns_data_arr_ptr = &(sns_ptr->sns_data_arr[index]);   
  
  CM_ASSERT(sns_config_data != NULL);
  CM_ASSERT(sns_data_arr_ptr != NULL);
  CM_ASSERT(sns_ptr != NULL);
  CM_ASSERT(INRANGE(index,CM_SNS_TYPE_NONE,CM_SNS_TYPE_ENV_SENSOR_MAX));
  
  if( sns_data_arr_ptr->current_batch_count == (sns_config_data->sns_max_batch_cnt-1) )
  {
     sns_ptr->batch_full_mask = (sns_ptr->batch_full_mask |(1 << sns_config_data->sns_id));
  }  
  
}





/*===========================================================================
FUNCTION cm_sns_handle_motion_sensor_rsp

DESCRIPTION
  handle motion sensor response in CM context

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cm_sns_handle_motion_sensor_rsp(cm_sns_motion_event_e_type event, uint32 psm_time)
{

  cm_sns_data_collection_s_type*	 sns_ptr = cmsns_get_data_info_ptr();
  CM_ASSERT(sns_ptr != NULL);
  CM_MSG_HIGH_2(" The motion event is = %d event and rsp state = %d ",event,sns_ptr->sns_ready_rsp_state);
  CM_MSG_HIGH_1(" The MCD PSM time = %d",psm_time);
  event_debug = event;
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE

  CM_SERIAL_LOG_1(" =CM= SNS Motion RSP event = %d  \n\r ",event);
#endif
  if(sns_ptr->sns_ready_rsp_state == CM_SNS_PSM_ENTER_RSP_SENT)
  {
    return;
  } 

#ifdef FEATURE_MMODE_TX_21
#ifndef FEATURE_BUILD_SENSORONLY_IMAGE
  if(event == CM_SNS_MOTION_EVENT_TYPE_DEVICE_IN_FLIGHT || event == CM_SNS_MOTION_EVENT_TYPE_DEVICE_NOT_IN_FLIGHT  )
  {

     if(sns_ptr->sns_odr_data.fmd_data.is_enable == FALSE)
     {
       return;
	 }
     if(event == CM_SNS_MOTION_EVENT_TYPE_DEVICE_IN_FLIGHT)
     {
	   sns_ptr->sns_odr_data.fmd_data.flight_status = CM_SNS_FMD_IN_FLIGHT;
     }
	 else if(event == CM_SNS_MOTION_EVENT_TYPE_DEVICE_NOT_IN_FLIGHT)
	 {
	   sns_ptr->sns_odr_data.fmd_data.flight_status = CM_SNS_FMD_NOT_IN_FLIGHT;
  }
	 /*  FMD response need to send irrespective of the response  */
	 cm_sns_set_context_change(TRUE);
	 sns_ptr->policy_met_mask = sns_ptr->policy_met_mask | (1<<CM_SNS_TYPE_FMD);
     cmsns_mgr_run_state_machine();
	 return;
  }

#endif

 if(event == CM_SNS_MOTION_EVENT_TYPE_DEVICE_FALL_DETECT  ||event == CM_SNS_MOTION_EVENT_TYPR_DEVICE_FALL_NOT_DETECT)
   {
     if(sns_ptr->sns_intr_data.fall_data.is_enable == FALSE)
     {
       return;
     }
     if(event == CM_SNS_MOTION_EVENT_TYPE_DEVICE_FALL_DETECT)
     {
	   sns_ptr->sns_intr_data.fall_data.freefall_state = CM_SNS_FREEFALL_DETECT; 
	   /*indicate the APP or move to full only if we have fall is detected else leave it */
	   sns_ptr->policy_met_mask = sns_ptr->policy_met_mask | (1<<CM_SNS_TYPE_FALL_DETECT);
	   cm_sns_set_context_change(TRUE);
	   if( sns_ptr->sns_intr_data.tilt_data.is_enable == TRUE )
	   {
         sns_ptr->sns_intr_data.tilt_data.need_to_enqueue = TRUE;
		 sns_ptr->sns_intr_data.tilt_data.state = CM_SNS_STATE_REQ_QUEUE;
	   }
     }
	 else if(event == CM_SNS_MOTION_EVENT_TYPR_DEVICE_FALL_NOT_DETECT)
	 {
	   sns_ptr->sns_intr_data.fall_data.freefall_state  = CM_SNS_FREEFALL_NOT_DETECT;
	 }   
      cmsns_mgr_run_state_machine();
	  return;
   }
#endif

#ifndef FEATURE_MMODE_TX_21 

  /* if response is received after the time expired 
     in that case ignore the event*/ 
  sns_ptr->motion_sns_data.event = event;
  if(event == CM_SNS_MOTION_EVENT_TYPE_MD_INTR_ENABLE)  
  {
    
    cmsns_process_motion_detect_enable();
  }
#endif
if(sns_ptr->motion_sns_data.mcd_enable == TRUE)
{
  if(event == CM_SNS_MOTION_EVENT_TYPE_STATIONARY || event == CM_SNS_MOTION_EVENT_TYPE_MOTION  )
  {
    cm_sns_set_context_change(TRUE);
    sns_ptr->policy_met_mask = sns_ptr->policy_met_mask | (1<<CM_SNS_TYPE_MCD);
    cmsns_mgr_run_state_machine();
    return;
  }
  else 
  { 
    sns_ptr->motion_sns_data.mcd_psm_time = psm_time;
    cmsns_mgr_run_state_machine();
    return;
  }
}
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
#ifndef FEATURE_MMODE_TX_21
  else if(event == CM_SNS_MOTION_EVENT_TYPE_MOTION )
  {
    /* save the policy condition for motion */
    sns_ptr->policy_met_mask = (sns_ptr->policy_met_mask |(1<<CM_SNS_TYPE_SMD));
	sns_ptr->sns_ready_rsp_state = CM_SNS_PSM_AUTO_READY_STARTED;
	cm_sns_set_context_change(TRUE);
   // cmsns_send_psm_auto_ready_ext_ind(cm_sns_get_active_time_value(),CM_PSM_TYPE_RESET);
    
    
  }
 #endif
 #endif 

}




/*===========================================================================
FUNCTION cm_sns_handle_sample_rsp

DESCRIPTION
  its handle the sample response from sensor in cm context

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_handle_sample_rsp(cm_sns_id_e_type sensor_id,cm_sns_sample_data_s_type sample_buffer)
{
  int index = 0;
  cm_sns_data_collection_s_type*	 sns_ptr = cmsns_get_data_info_ptr();
  int cur_batch_cnt = 0;
  int max_batch = 0;
  uint32 current_time = time_get_secs();
#ifdef FEATURE_MMODE_TX_21
  cm_sns_qmi_sns_phy_test_get_data_rsp_s_type sns_sample;
  cm_sns_phy_test_config_s_type* config = cm_sns_get_phy_test_config();
#endif  
CM_ASSERT(sns_ptr != NULL);

#ifdef FEATURE_MMODE_TX_21
  if(config->cm_sns_phy_test_get_data_req == TRUE)
  {
    memset((void*)&sns_sample,0,sizeof(cm_sns_qmi_sns_phy_test_get_data_rsp_s_type)); 
	
    sns_sample.msg_hdr.id = MM_CM_SNS_MGR_SNS_TEST_PHY_GET_DATA_IND;
	sns_sample.sensor_id  = sensor_id;
    sns_sample.sample.dimension_data_len = sample_buffer.dimension_data_len;
	memscpy(sns_sample.sample.sns_dimension_data,sizeof(float)*sample_buffer.dimension_data_len,sample_buffer.sns_dimension_data,sizeof(float)*sample_buffer.dimension_data_len );
    sns_sample.sample.timestamp = sample_buffer.timestamp;
	
   cm_msgr_send(MM_CM_SNS_MGR_SNS_TEST_PHY_GET_DATA_IND,MSGR_MM_CM_SNS_MGR,
	       (msgr_hdr_s*)&sns_sample,sizeof( cm_sns_qmi_sns_phy_test_get_data_rsp_s_type));     
  }
  else
#endif
  {
#ifdef FEATURE_MMODE_TX_21

  if(sensor_id == CM_SNS_TYPE_ALS )
  	
  {  
     
    CM_MSG_HIGH_3(" ALS sample len = %d sampl[0]= %d and sample[1]= %d ",sample_buffer.dimension_data_len,
                                                                      sample_buffer.sns_dimension_data[0],
                                                                     sample_buffer.sns_dimension_data[1] );
    if(sns_ptr->sns_intr_data.als_data.is_enable == FALSE)
    {
      return;
    }

    if(als_previous_value == -1)
    {
       als_previous_value = sample_buffer.sns_dimension_data[0];
       return;
    }

    if((als_previous_value < sample_buffer.sns_dimension_data[0] ) 
          &&  (((sample_buffer.sns_dimension_data[0] - als_previous_value) > sns_ptr->sns_intr_data.als_data.threshold)
    ))
    
    {
      als_previous_value = sample_buffer.sns_dimension_data[0] ;
    
      sns_ptr->policy_met_mask = sns_ptr->policy_met_mask| (1<<CM_SNS_TYPE_ALS);
      sns_ptr->sns_intr_data.als_data.state = CM_SNS_STATE_DONE;
      if(sns_ptr->sns_intr_data.tilt_data.is_enable == TRUE)
      {
         sns_ptr->sns_intr_data.tilt_data.need_to_enqueue = TRUE;
         sns_ptr->sns_intr_data.tilt_data.state =  CM_SNS_STATE_REQ_QUEUE;
    }
  }
  }
  else if(sensor_id == CM_SNS_TYPE_TILT)
  {
    sns_ptr->sns_intr_data.tilt_data.state =  CM_SNS_STATE_DONE;
     if(sns_ptr->sns_intr_data.tilt_data.is_enable == FALSE)
    {
      return;
    }
    sns_ptr->policy_met_mask = sns_ptr->policy_met_mask| (1<<CM_SNS_TYPE_TILT);
	sns_ptr->sns_intr_data.tilt_data.tilt_angle_val =  sample_buffer.sns_dimension_data[0];
    CM_MSG_HIGH_1(" TILT sample  sampl[0]= %d  ",sample_buffer.sns_dimension_data[0]);
  }
  else
 #endif
 {
  CM_ASSERT(INRANGE(sensor_id,CM_SNS_TYPE_ALS,CM_SNS_TYPE_ENV_SENSOR_MAX));
  for(index = 0; index < sns_ptr->no_of_sns_enabled; index++)
  {
    if(sns_ptr->sns_data_arr[index].sns_config_data.sns_id == sensor_id)
    {
      cur_batch_cnt = sns_ptr->sns_data_arr[index].current_batch_count;
      max_batch =     sns_ptr->sns_data_arr[index].sns_config_data.sns_max_batch_cnt;

    if(cur_batch_cnt > max_batch)
    {
      cur_batch_cnt = 0; 
	}
  
      /* copy the sample data */
	sns_ptr->sns_data_arr[index].sns_sample_data[cur_batch_cnt % max_batch ] = sample_buffer;

#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
#ifdef FEATURE_SERIAL_LOGGING_ENABLE
  CM_SERIAL_LOG_2(" =CM= sample response for sensor id = %d and sample length is %d  \n\r ", sensor_id,sample_buffer.dimension_data_len);
  CM_SERIAL_LOG_2(" =CM= sample value[0] = %d value[1] = %d  \n\r ", (int)sample_buffer.sns_dimension_data[0],
                                                                     (int)sample_buffer.sns_dimension_data[1]);
#endif
#endif
      CM_MSG_HIGH_2(" =SNSDBG= =CM= sample response for sensor id = %d and sample length is %d  \n\r ", sensor_id,sample_buffer.dimension_data_len);
       CM_MSG_HIGH_2("=SNSDBG=  =CM= sample value[0] = %d value[1] = %d  \n\r ", (int)sample_buffer.sns_dimension_data[0],
                          (int)sample_buffer.sns_dimension_data[1]);

      /* updating the uptime now even its more logical to do here */
      sns_ptr->sns_data_arr[index].sns_uptime = current_time + sns_ptr->sns_data_arr[index].sns_config_data.sns_interval;

      /* Update the sns state here */
      cm_sns_update_sns_current_state(sns_ptr,CM_SNS_STATE_DONE,index);

       /* check if sns policy met  */
      if ( cm_sns_check_for_policy_cond_met(sns_ptr,index,cur_batch_cnt) == FALSE)
      {
        /* check if sns batch full met    */
        cm_sns_check_for_batch_full_cond_met(sns_ptr,index);
      }
      sns_ptr->sns_data_arr[index].current_batch_count = cur_batch_cnt+1 ;
	  if(sns_ptr->sns_data_arr[index].max_count < max_batch)
	  {
	     sns_ptr->sns_data_arr[index].max_count = sns_ptr->sns_data_arr[index].current_batch_count;
	  }
          /* Increment the batch count after all condition check 
          because all condition will be check on current 
          sample count*/
      cm_sns_set_context_change(TRUE);
      break;
         /* make sense to break beacuse the sens id is unique 
        not going to match more than one*/
    }
  }
  
 }
  sns_ptr->sns_sensor_rsp_timer = 0xFFFFFFFF;
  
  /*Now run state machine   */
  cmsns_mgr_run_state_machine();
 	}
}

#ifndef FEATURE_BUILD_SENSORONLY_IMAGE
/*===========================================================================
FUNCTION cm_sns_config_allowed

DESCRIPTION
  Check if sensor configuration allowed,no error in parameters

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_sns_config_allowed(cm_sns_cmd_s_type* sns_cmd_ptr) 
{
  boolean res = TRUE;
  int index = 0;
#ifdef FEATURE_MMODE_TX_21  
  config_init_input *input;
  config_init_output *output;    
#endif
  if(sns_cmd_ptr != NULL)
  {
    if(sns_cmd_ptr->info.cmd_s.sns_config_req.len > CM_SNS_TYPE_ENV_SENSOR_MAX ||
       (cmsns_get_data_info_ptr()->sns_ready_rsp_state == CM_SNS_PSM_ENTER_RSP_SENT) ||
       (cmsns_get_data_info_ptr()->sns_ready_rsp_state == CM_SNS_PSM_ENTER_REQ_STARTED))
    {
       res = FALSE;
    }
    else
    {
      for(index = 0;index<sns_cmd_ptr->info.cmd_s.sns_config_req.len ;index++)
      {
         CM_MSG_HIGH_0("CNSNS in config allowed");
         if (sns_cmd_ptr->info.cmd_s.sns_config_req.sns_arr[index].sns_max_batch_cnt > CM_MAX_BATCH_SUPPORTED)
         {
           res = FALSE;
		   CM_MSG_HIGH_0("CNSNS Error in config allowed before calling algo 1");
           break;
         }
         if (sns_cmd_ptr->info.cmd_s.sns_config_req.sns_arr[index].sns_id > CM_SNS_TYPE_ENV_SENSOR_MAX ||
             sns_cmd_ptr->info.cmd_s.sns_config_req.sns_arr[index].sns_id < CM_SNS_TYPE_ALS)
         {
           res = FALSE;
		   CM_MSG_HIGH_0("CNSNS Error in config allowed before calling algo 2");
           break;
         }
         if((sns_cmd_ptr->info.cmd_s.sns_config_req.sns_arr[index].sns_interval == 0))
         {
           res = FALSE;
		   CM_MSG_HIGH_0("CNSNS Error in config allowed before calling algo 3");
           break;
         }
      }
    }
	
 #ifdef FEATURE_MMODE_TX_21  
	res = TRUE;
   input= (config_init_input*)cm_mem_malloc(sizeof(config_init_input));
   output = (config_init_output* )cm_mem_malloc(sizeof(config_init_output));
  
   for(index = 0;index < sns_cmd_ptr->info.cmd_s.sns_config_req.len ; index++)
   {
     memset(output,0,sizeof(config_init_output));
     input->sensor_type = sns_cmd_ptr->info.cmd_s.sns_config_req.sns_arr[index].sns_id;
     input->operating_mode = sns_cmd_ptr->info.cmd_s.sns_config_req.sns_arr[index].operating_mode;
	 input->alarm_thr_high = sns_cmd_ptr->info.cmd_s.sns_config_req.sns_arr[index].sns_policy.sns_policy_elem[0].greater_value;
	 input->alarm_thr_low = sns_cmd_ptr->info.cmd_s.sns_config_req.sns_arr[index].sns_policy.sns_policy_elem[0].lower_value;
	 input->pre_alert_thr_high = sns_cmd_ptr->info.cmd_s.sns_config_req.sns_arr[index].sns_policy.sns_policy_elem[0].pre_greater_value;
	 input->pre_alert_thr_low = sns_cmd_ptr->info.cmd_s.sns_config_req.sns_arr[index].sns_policy.sns_policy_elem[0].pre_lower_value;
	 input->performance_mode = sns_cmd_ptr->info.cmd_s.sns_config_req.sns_arr[index].sns_performance_mode;
 	 input->alarm_hysteresis_duration = sns_cmd_ptr->info.cmd_s.sns_config_req.sns_arr[index].sns_hysteresis_duration;
 	 input->HPM_sampling_period = sns_cmd_ptr->info.cmd_s.sns_config_req.sns_arr[index].sns_high_perf_interval;
	 input->measurement_period = sns_cmd_ptr->info.cmd_s.sns_config_req.sns_arr[index].sns_interval ;

	 sns_env_aware_config_init_interface(*input,output);
     CM_MSG_HIGH_4("sensor_type %d ACK %d ACK_error_config %d ACK_error_type %d ", output->sensor_type, output->ACK, output->ACK_error_config, output->ACK_error_type);
	  
     algo_output[index] = *output;
	 if(output->ACK == ACK_ERROR)
	 {
	     CM_MSG_HIGH_0("CNSNS Error in config allowed algo error");
	   	 res = FALSE;	 
   	 }
   }

   cm_mem_free(input);
   input = NULL;
   cm_mem_free(output);
   output = NULL;
#endif
    
  }
  else
  {  
    CM_MSG_HIGH_0("CNSNS Error in config allowed end");
    res = FALSE;
  }
  CM_MSG_HIGH_1("CNSNS Error in config allowed res= %d", res);
  return res;
}

/*===========================================================================
FUNCTION cm_sns_data_req_allowed

DESCRIPTION
  Check if data request is allowed

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_sns_data_req_allowed(cm_sns_cmd_s_type* sns_cmd_ptr)
{
  boolean res = TRUE;
  if(sns_cmd_ptr != NULL)
  {
    if(sns_cmd_ptr->info.cmd_s.sns_data_req.sensor_id_mask == 0 )
    {
      CM_MSG_HIGH_0("cm_sns_data_req_allowed return false snd id mask = 0");
      res = FALSE;
    }
  }
  else
  {
    res = FALSE;
  }

  return res;
}

/*===========================================================================
FUNCTION cm_sns_od_sns_req_allowed

DESCRIPTION
  Check if ondemand request is allowed

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_sns_od_sns_req_allowed(cm_sns_cmd_s_type* sns_cmd_ptr)
{
  boolean res = TRUE;
  if(sns_cmd_ptr != NULL)
  {
    if(((sns_cmd_ptr->info.cmd_s.sns_od_sns_req.sensor_id_mask & (1<<CM_SNS_TYPE_FMD))!=0 )&&
		(cmsns_get_data_info_ptr()->sns_odr_data.fmd_data.is_enable== TRUE))
    {
      CM_MSG_HIGH_0("cm_sns_od_sns_req_allowed return TRUE snd id");
      res = TRUE;
    }
	else
	{
      res = FALSE;
	}
  }
  else
  {
    res = FALSE;
  }

  return res;
}


/*===========================================================================
FUNCTION cm_sns_reset_current_config

DESCRIPTION
  Discard any old config

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

static void cm_sns_reset_current_config( cm_sns_data_collection_s_type*   sns_ptr )
{
  CM_ASSERT(sns_ptr != NULL);
  if(sns_ptr->sns_data_arr != NULL)
  {
    CM_MSG_HIGH_0("  Discarding the old sns config data ");
    cm_mem_free(sns_ptr->sns_data_arr);
    sns_ptr->sns_data_arr = NULL;
  }
  sns_ptr->batch_full_mask = 0;
  sns_ptr->no_of_sns_enabled = 0;
  sns_ptr->policy_met_mask = 0;
  sns_ptr->sns_fail_mask = 0;
  sns_ptr->sns_sensor_rsp_timer = 0xFFFFFFFF;
  sns_ptr->sns_data_arr = NULL;
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
  next_sns_state_g = CM_SNS_COND_CHECK_MOTION_STATE;
#else
  next_sns_state_g = CM_SNS_COND_CHECK_POLICY_MET_STATE;
#endif

}

#endif
/*===========================================================================
FUNCTION cm_sns_init_sns_data_collection

DESCRIPTION
  init sns data collection

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_init_sns_data_collection()
{
  cm_sns_data_collection_s_type* sns_ptr = cmsns_get_data_info_ptr();
  CM_ASSERT(sns_ptr != NULL);
  sns_ptr->batch_full_mask = 0;
  sns_ptr->no_of_sns_enabled = 0;
  sns_ptr->policy_met_mask = 0;
  sns_ptr->sns_fail_mask = 0;
  sns_ptr->sns_sensor_rsp_timer = 0xFFFFFFFF;
  sns_ptr->sns_data_arr = NULL; 
  /* These 2 fields will get the 
     value from EFS each time*/
  sns_ptr->force_motion_enable = FALSE;
  sns_ptr->sns_bootup_time = 0;
#ifndef FEATURE_MMODE_TX_21
  sns_ptr->motion_sns_data.motion_sns_uptime = 0xFFFFFFFF;
  sns_ptr->motion_sns_data.sns_smd_wait_time = CM_DEFAULT_MOTION_WAIT_TIME;
#endif
  sns_ptr->intrpt_status = CM_SNS_HW_INTRPT_TYPE_NONE;
  sns_ptr->sns_psm_full_boot_time = CM_PSM_MAX_UPTIME_64;
  sns_ptr->intr_sns_prog_mask = 0;
  #ifdef FEATURE_MMODE_TX_21
  sns_ptr->sns_registry_mask  = 0;
  #endif
  sns_ptr->motion_sns_data.mcd_psm_time = 0xFFFFFFFF;
  memset(&sns_ptr->sns_intr_data,0,sizeof(cm_sns_intr_sns_data_s_type));
}

/*===========================================================================
FUNCTION cm_sns_update_psm_state

DESCRIPTION
  update the sensors psm ready state

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_update_psm_state(cm_sns_data_collection_s_type*   sns_ptr,cm_sns_psm_state_e_type rsp_state)
{
  CM_ASSERT(sns_ptr != NULL);
  sns_ptr->sns_ready_rsp_state = rsp_state;
}

/*===========================================================================
FUNCTION cm_sns_update_sns_current_state

DESCRIPTION
  its update the sesnor current state

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_update_sns_current_state(cm_sns_data_collection_s_type*   sns_ptr,
                                                 cm_sns_state_e_type state,int index)
{

  CM_ASSERT(sns_ptr != NULL);
  CM_ASSERT(INRANGE(index,CM_SNS_TYPE_NONE,CM_SNS_TYPE_ENV_SENSOR_MAX))
  CM_MSG_HIGH_2("State change to = %d of sensor id = %d",sns_ptr->sns_data_arr[index].current_sns_state,index);
  sns_ptr->sns_data_arr[index].current_sns_state = state;

}


#ifndef FEATURE_BUILD_SENSORONLY_IMAGE
/*===========================================================================
FUNCTION cm_sns_check_if_sensors_are_idle

DESCRIPTION
  Check if sesnors are idle not request pending

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_sns_check_if_sensors_are_idle(cm_sns_data_collection_s_type*   sns_ptr) 
{
   boolean result = TRUE;
   int index = 0;
   CM_ASSERT(sns_ptr != NULL);

   for(index = 0;index<sns_ptr->no_of_sns_enabled;index++)
   {
      /* only these 2 states indicate sns mgr ready ready to send the 
          psm ready rsp else reject it */
      if(!((sns_ptr->sns_data_arr[index].current_sns_state == CM_SNS_STATE_NONE) || 
          (sns_ptr->sns_data_arr[index].current_sns_state == CM_SNS_STATE_DONE)))
      {
         result = FALSE;         
         break;
      }
   }

#ifdef FEATURE_MMODE_TX_21
   if((result == TRUE) && (sns_ptr->sns_intr_data.tilt_data.is_enable == TRUE) && 
      !(sns_ptr->sns_intr_data.tilt_data.state == CM_SNS_STATE_NONE ||
        sns_ptr->sns_intr_data.tilt_data.state == CM_SNS_STATE_DONE)
      )
    {
       result = FALSE;
    }
	  if((result == TRUE) && (sns_ptr->sns_intr_data.als_data.is_enable == TRUE) && 
      !(sns_ptr->sns_intr_data.als_data.state == CM_SNS_STATE_NONE ||
        sns_ptr->sns_intr_data.als_data.state == CM_SNS_STATE_DONE)
      )
    {
       result = FALSE;
    }
#endif   
   return result;

}


/*===========================================================================
FUNCTION cm_sns_ready_to_send_psm_ready_rsp

DESCRIPTION
  Check if it is ok to vote for ready request

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_sns_ready_to_send_psm_ready_rsp(void)
{
  
  cm_sns_data_collection_s_type*   sns_ptr = cmsns_get_data_info_ptr();
  boolean result = TRUE;
  CM_ASSERT(sns_ptr != NULL);
  if(cm_sns_check_if_sensors_are_idle(sns_ptr) == FALSE)
  {
    result = FALSE;

  }
  return result;
}


#endif

/*===========================================================================
FUNCTION cm_sns_get_min_sns_uptime

DESCRIPTION
  get minimum of all sensors uptime value

DEPENDENCIES
  none

RETURN VALUE
  uint32

SIDE EFFECTS
  none

===========================================================================*/

uint32 cm_sns_get_min_sns_uptime (void)
{

  uint32 active_time = 0xFFFFFFFF;// Max value that timer can achieve
  cm_sns_data_collection_s_type*   sns_ptr = cmsns_get_data_info_ptr();
  int index = 0;
  CM_ASSERT(sns_ptr != NULL);  

  for(index = 0; index < sns_ptr->no_of_sns_enabled;index++)
  {
 #ifdef FEATURE_MMODE_TX_21
     if( sns_ptr->sns_data_arr[index].sns_config_data.operating_mode == TRUE)
#endif
     	{
    /*  Finding minimum of all uptime sns time should be in secs
       and app should pass in seconds*/
    if( active_time > sns_ptr->sns_data_arr[index].sns_uptime )
    {
       active_time = sns_ptr->sns_data_arr[index].sns_uptime;
    }
  }
}

#ifdef FEATURE_MMODE_TX_21

  if(sns_ptr->sns_intr_data.tilt_data.is_enable == TRUE &&
    (sns_ptr->sns_intr_data.tilt_data.config_data.has_iteration_delay == TRUE) )
    {
       if( sns_ptr->sns_intr_data.tilt_data.tilt_sns_uptime < active_time)
       {
         active_time = sns_ptr->sns_intr_data.tilt_data.tilt_sns_uptime;
       }
    }
  if(sns_ptr->sns_intr_data.als_data.is_enable == TRUE)
    {
      if(sns_ptr->sns_intr_data.als_data.als_sns_uptime < active_time)
      {  
      active_time = sns_ptr->sns_intr_data.als_data.als_sns_uptime;
      }
 if(sns_ptr->motion_sns_data.mcd_enable == TRUE  )
    {
      if( sns_ptr->motion_sns_data.mcd_psm_time< active_time)
       {
         active_time = sns_ptr->motion_sns_data.mcd_psm_time;
       }
    }
    }
#endif
   
  return active_time;

}

/*===========================================================================
FUNCTION cm_sns_get_active_time_value

DESCRIPTION
  it return the active timer value,
  its delta value

DEPENDENCIES
  none

RETURN VALUE
  uint32

SIDE EFFECTS
  none

===========================================================================*/

uint32 cm_sns_get_active_time_value(void)
{

  uint32 active_time = 0xFFFFFFFF;// Max value that timer can achieve
  uint32 final_delta = 0xFFFFFFFF;
  cm_sns_data_collection_s_type*  sns_ptr = cmsns_get_data_info_ptr();  
  dword                     curr_uptime  = time_get_secs();
  CM_ASSERT(sns_ptr != NULL); 

  active_time = cm_sns_get_min_sns_uptime();
  

  if(active_time == 0xFFFFFFFF )
  {
    return active_time;
  }

  if(curr_uptime >= active_time)
  {
    final_delta = 0;
  }
  else 
  {
     final_delta = active_time - curr_uptime;
  }

  CM_MSG_HIGH_1("The min time is %d ",final_delta);

  return final_delta;
}


/*===========================================================================
FUNCTION cm_sns_set_context_change

DESCRIPTION
  set the context if it is changed

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_sns_set_context_change(boolean val)
{
  context_change = val;
}

/*===========================================================================
FUNCTION cm_sns_check_context_change

DESCRIPTION
  Check if context is changed

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_sns_check_context_change(void)
{

 /* Currently returning true default
     in future if need to come up with 
     algo we can add here*/

  return context_change;

}

#ifndef FEATURE_MMODE_TX_21
/*===========================================================================
FUNCTION cm_sns_check_if_motion_possible

DESCRIPTION
  Check if need to register for motion to detect the 
  actual motion

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/


boolean cm_sns_check_if_motion_possible(cm_sns_data_collection_s_type*  sns_ptr )
{
  boolean res = FALSE;
  CM_ASSERT(sns_ptr != NULL);

  /* if efs enable then enable this line
      */
  if(sns_ptr->motion_sns_data.motion_detect_enable == TRUE)
  {
     /* remove the check and always register */
     /* We can assume that sensor only Image is bootup 
        due to motion sensor interrupt 
        if tomorrow we have othe global include we can include
        here to make decision for motion detection*/
     res = TRUE;
    
  }
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
  CM_SERIAL_LOG_1(" =CM= SNS Motion enable = %d \n\r",sns_ptr->motion_sns_data.motion_detect_enable);
#endif
  CM_MSG_HIGH_1(" Motion possible check return = %d ",res);
  return res;

}

#endif
/*===========================================================================
FUNCTION cm_sns_find_sesnors_polling_time_reached

DESCRIPTION
  Check if sensors reahced the polling time and update it states

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_sns_find_sesnors_polling_time_reached(  cm_sns_data_collection_s_type* sns_ptr)
{
  uint32  current_time = 0;
  int index = 0;
  boolean res = FALSE;
  CM_ASSERT(sns_ptr != NULL);
  current_time = time_get_secs();
  for(index = 0;index < sns_ptr->no_of_sns_enabled;index++)
  {
  #ifdef FEATURE_MMODE_TX_21
     if( sns_ptr->sns_data_arr[index].sns_config_data.operating_mode == TRUE)
  #endif
     {
     if(
	 
	#ifndef FEATURE_BUILD_SENSORONLY_IMAGE
	      ((sns_ptr->sns_data_arr[index].sns_uptime ) <= current_time) && 
	#else
	      ((sns_ptr->sns_data_arr[index].sns_uptime - (sns_ptr->sns_bootup_time/1000)) <= current_time) && 
    #endif
           !((sns_ptr->sns_data_arr[index].current_sns_state == CM_SNS_STATE_REQ_IN_PROGRESS) ||
             (sns_ptr->sns_data_arr[index].current_sns_state == CM_SNS_STATE_REQ_QUEUE))
           )
     {
     /* These are sensors which met condition, can multiple sensor polling 
        time out at same time  or only single sensor update the state
        to queue to all that sensors */
       cm_sns_update_sns_current_state(sns_ptr,CM_SNS_STATE_REQ_QUEUE,index);
       res = TRUE;
	   CM_MSG_HIGH_3("Polling time reach  for the sns id = %d sns_uptime = %d and current uptime = %d",
	   	      sns_ptr->sns_data_arr[index].sns_config_data.sns_id,
	   	      sns_ptr->sns_data_arr[index].sns_uptime,current_time);
     }
}
  }
#ifdef FEATURE_MMODE_TX_21

  if(res == FALSE)
  {
     if(sns_ptr->sns_intr_data.tilt_data.is_enable == TRUE &&
	 	(sns_ptr->sns_intr_data.tilt_data.config_data.has_iteration_delay == TRUE) &&
#ifndef FEATURE_BUILD_SENSORONLY_IMAGE
	    (sns_ptr->sns_intr_data.tilt_data.tilt_sns_uptime <= current_time )
#else
        ((sns_ptr->sns_intr_data.tilt_data.tilt_sns_uptime - (sns_ptr->sns_bootup_time/1000)) <= current_time)
#endif
       )
       {
         sns_ptr->sns_intr_data.tilt_data.need_to_enqueue = TRUE;
         sns_ptr->sns_intr_data.tilt_data.tilt_sns_uptime =  sns_ptr->sns_intr_data.tilt_data.config_data.iteration_delay + current_time;
         res = TRUE;
         sns_ptr->sns_intr_data.tilt_data.state = CM_SNS_STATE_REQ_QUEUE;
       }
  }
    if(res == FALSE)
  {
     if(sns_ptr->sns_intr_data.als_data.is_enable == TRUE &&
#ifndef FEATURE_BUILD_SENSORONLY_IMAGE
	    (sns_ptr->sns_intr_data.als_data.als_sns_uptime <= current_time )
#else
        ((sns_ptr->sns_intr_data.als_data.als_sns_uptime - (sns_ptr->sns_bootup_time/1000)) <= current_time)
#endif
       )
       {
         res = TRUE;
         sns_ptr->sns_intr_data.als_data.state = CM_SNS_STATE_REQ_QUEUE;
       }
  }
#endif
 
 return res;
}

/*===========================================================================
FUNCTION cm_sns_send_sensor_request

DESCRIPTION
  Check if any sensor qualify for sending request
  return true if it does 

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/


boolean cm_sns_send_sensor_request(cm_sns_data_collection_s_type* sns_ptr)
{
  boolean  result = FALSE;
  int index = 0;
  CM_ASSERT(sns_ptr != NULL);

  for(index = 0;index < sns_ptr->no_of_sns_enabled;index++)
  {
    if(sns_ptr->sns_data_arr[index].current_sns_state == CM_SNS_STATE_REQ_IN_PROGRESS )
    {
      return TRUE;
    }
  }

  for(index = 0;index < sns_ptr->no_of_sns_enabled;index++)
  {
    if(sns_ptr->sns_data_arr[index].current_sns_state == CM_SNS_STATE_REQ_QUEUE )
    {
      /* actual sensor request API call */
      cm_send_sns_req(sns_ptr->sns_data_arr[index].sns_config_data.sns_id);
      result = TRUE;
      /* Update the sns_state  */
      cm_sns_update_sns_current_state(sns_ptr,CM_SNS_STATE_REQ_IN_PROGRESS,index);
      break;
    }
  }
#ifdef FEATURE_MMODE_TX_21

 if(result == FALSE)
  {
     if(sns_ptr->sns_intr_data.tilt_data.is_enable == TRUE &&
        sns_ptr->sns_intr_data.tilt_data.need_to_enqueue == TRUE)
     {
       cm_send_sns_req(CM_SNS_TYPE_TILT);
	   result = TRUE;
	   sns_ptr->sns_intr_data.tilt_data.need_to_enqueue = FALSE;
	   sns_ptr->sns_intr_data.tilt_data.state =  CM_SNS_STATE_REQ_IN_PROGRESS;
     }
  }

 if(result == FALSE)
  {
     if(sns_ptr->sns_intr_data.als_data.is_enable == TRUE &&
         sns_ptr->sns_intr_data.als_data.state == CM_SNS_STATE_REQ_QUEUE)
     {
       cm_send_sns_req(CM_SNS_TYPE_ALS);
	   result = TRUE;
	   sns_ptr->sns_intr_data.als_data.state =  CM_SNS_STATE_REQ_IN_PROGRESS;
     }
  }
#endif


 return result;
}


/*===========================================================================
FUNCTION cm_sns_is_request_in_progress

DESCRIPTION
  Check if any sensor is still waiting for the request/response

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_sns_is_request_in_progress(cm_sns_data_collection_s_type* sns_ptr)
{
  boolean  result = FALSE;
  int index = 0;
  CM_ASSERT(sns_ptr != NULL);

  for(index = 0;index < sns_ptr->no_of_sns_enabled;index++)
  {
    if(sns_ptr->sns_data_arr[index].current_sns_state == CM_SNS_STATE_REQ_QUEUE  ||
        sns_ptr->sns_data_arr[index].current_sns_state == CM_SNS_STATE_REQ_IN_PROGRESS )
    {
      result = TRUE;
      break;
    }
  }

#ifdef FEATURE_MMODE_TX_21
  if((result == FALSE) && (sns_ptr->sns_intr_data.tilt_data.is_enable == TRUE) && 
      (sns_ptr->sns_intr_data.tilt_data.state == CM_SNS_STATE_REQ_QUEUE ||
      sns_ptr->sns_intr_data.tilt_data.state == CM_SNS_STATE_REQ_IN_PROGRESS)
  )
  {
    result = TRUE;
  }
    if((result == FALSE) && (sns_ptr->sns_intr_data.als_data.is_enable == TRUE) && 
      (sns_ptr->sns_intr_data.als_data.state == CM_SNS_STATE_REQ_QUEUE ||
      sns_ptr->sns_intr_data.als_data.state == CM_SNS_STATE_REQ_IN_PROGRESS)
  )
  {
    result = TRUE;
  }
#endif   


  return result;
}


/*===========================================================================
FUNCTION cm_sns_check_if_next_batch_cond_met

DESCRIPTION
  Check if any sensors had reached the batch count

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/


boolean cm_sns_check_if_next_batch_cond_met(cm_sns_data_collection_s_type* sns_ptr)
{
  boolean  result = FALSE;
  CM_ASSERT(sns_ptr != NULL);
  if(sns_ptr->batch_full_mask != 0)
  {
    result = TRUE;
  }
  return result;

}


/*===========================================================================
FUNCTION cm_sns_evaluate_and_send_ready_rsp

DESCRIPTION
  evaluate what proper type of ready rsp need to send

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_sns_evaluate_and_send_ready_rsp(cm_sns_data_collection_s_type* sns_ptr)
{
  boolean result = TRUE;

#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
  uint64                      uptime = time_get_ms_native();
  uint8                    psm_enter_type;
  uint32                   sns_time_value;
  uint64                   fullboot_time = 0;
#endif
  CM_ASSERT(sns_ptr != NULL);
#ifndef FEATURE_BUILD_SENSORONLY_IMAGE
  if(cm_sns_check_if_sensors_are_idle(sns_ptr) == FALSE)
  {
   return FALSE;
  }
#endif

  if(sns_ptr->sns_ready_rsp_state == CM_SNS_PSM_READY_RSP_NONE)
  {
    /* It qualified for auto ready indication  */
    /* fo
     shutdown ony for policy met need to send reset*/
    if(sns_ptr->policy_met_mask == 0 && sns_ptr->batch_full_mask == 0 && sns_ptr->sns_fail_mask == 0)
    {
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
     sns_time_value = cm_sns_get_active_time_value();
	 psm_enter_type = CM_PSM_TYPE_SHUTDOWN;
     fullboot_time = sns_ptr->sns_psm_full_boot_time;
	 /*  defensive cehck */
	 if(fullboot_time == 0)
	 {
        fullboot_time = (cmpsm_ptr()->psm_cfg_info.threshold)*2;
	 }
     cm_sns_set_context_change(TRUE);
     CM_MSG_HIGH_3(" The fullboot_time is %d  , current uptime = %d ,sns is %d",fullboot_time,uptime,sns_time_value);

	 /* in sec  */
	 
     /*  collision case where just before full boot timer collapse/motion happens and SOI loaded */
     if(fullboot_time <= uptime )
     {
       psm_enter_type = CM_PSM_TYPE_RESET;
     }
     else if ((fullboot_time - uptime) < sns_ptr->sns_bootup_time  )
     {
        psm_enter_type = CM_PSM_TYPE_RESET;
      }
	 /* if no sensor enable send the full boot time so that device can agian move to PSM for full boot */
	 else if( sns_ptr->no_of_sns_enabled == 0)
	 {
      
	   sns_time_value = (uint32)(((fullboot_time - uptime))/1000);
	 }    
     #ifndef FEATURE_MMODE_TX_21
    if ( (sns_ptr->motion_sns_data.no_motion_timer_expired == TRUE) &&
	     (psm_enter_type == CM_PSM_TYPE_RESET))
	{
	   sns_ptr->motion_sns_data.event = CM_SNS_MOTION_EVENT_TYPE_NO_MOTION_TIMER_EXPIRED_RESET;
	}
	CM_SERIAL_LOG_3(" =CM= SNS eval rdy rsp event  = %d fboot delta time = %d uptime = %d \n\r",sns_ptr->motion_sns_data.event,(uint32)((fullboot_time - uptime )/1000),(uint32)(uptime/1000));
     #endif
   	cmsns_send_psm_auto_ready_ext_ind(sns_time_value,psm_enter_type);
#else
     /* NO need fo auto ready from sensor lets NAS drive that*/
      //cmsns_send_psm_auto_ready_ind(cm_sns_get_active_time_value());
#endif
      result = TRUE;
    }
    else
    {

#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
       cmsns_send_psm_auto_ready_ext_ind(cm_sns_get_active_time_value(),CM_PSM_TYPE_RESET);
#else
	   /* NO need fo auto ready from sensor lets NAS drive that*/

       //cmsns_send_psm_auto_ready_ind(cm_sns_get_active_time_value());
#endif
       result = TRUE;
    }
    //sns_ptr->sns_ready_rsp_state = CM_SNS_PSM_AUTO_READY_SENT;
  }   
#ifndef FEATURE_BUILD_SENSORONLY_IMAGE
  else if(sns_ptr->sns_ready_rsp_state == CM_SNS_PSM_READY_RSP_REJECTED )
  {
   /* It qualified to send ready ind can happen only in 
      full boot image*/
    cmsns_send_psm_ready_ind(cm_sns_get_active_time_value());
    sns_ptr->sns_ready_rsp_state = CM_SNS_PSM_READY_RSP_SEND;
    result = TRUE;
  }
#endif
  return result;
}


/*===========================================================================
FUNCTION cm_sns_write_context

DESCRIPTION
  it will store the sns context,it allocate memory and
  store all data

DEPENDENCIES
  none

RETURN VALUE
  uint8 *

SIDE EFFECTS
  none

===========================================================================*/


void cm_sns_write_context(void)
{

  cm_sns_data_collection_s_type*	 sns_ptr = NULL;
  int index = 0;
  uint8 *sns_mgr_context = NULL,*temp_sns_ptr=NULL;
  uint32 total_size = 0;
  sns_ptr = cmsns_get_data_info_ptr();
 
                       /*sensor number */                                        /* all sensor data  */
  total_size = sizeof(sns_ptr->no_of_sns_enabled) + (sns_ptr->no_of_sns_enabled * sizeof(cm_sns_data_s_type))+
                      /* size of batchfullmask  */    /* sizeof policymet mask  */ /* Motion detect data for AMD/SMD  */
                      sizeof(int)               +       sizeof(int)          + sizeof(cm_sns_motion_sns_data_s_type) +

					  /*  size of fullboot time  */     /*sizeof sns fail mask*/
					  sizeof(uint64)                         +sizeof(int)
#ifdef FEATURE_MMODE_TX_21
					  /*Intr data  */         /* ondemand data */
				+  sizeof(cm_sns_intr_sns_data_s_type) + sizeof(cm_sns_on_demand_sns_data_s_type)
#endif
 ;     

				/* size of current data */
  total_size += sizeof(uint32);
  CM_ASSERT(sns_ptr != NULL); 
  
  sns_mgr_context = cm_mem_malloc(total_size);
  /*sns_mgr_context always point to the begin of the 
    data chunk and the tempsns ptr keep 
    moving as pe copy of data*/
  temp_sns_ptr = sns_mgr_context;
  /* write the sizes */
  memscpy(temp_sns_ptr,sizeof(uint32),&total_size,sizeof(uint32) );
  temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(uint32));

  /*  write the fullboot time */
  memscpy(temp_sns_ptr,sizeof(uint64),&sns_ptr->sns_psm_full_boot_time,sizeof(uint64) );
  temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(uint64));
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE

  CM_SERIAL_LOG_1(" =CM= SNS SNSPSM ful boot time at write = %d \n\r",(uint32)sns_ptr->sns_psm_full_boot_time);
#endif
  /* write no of sensor  */
  memscpy(temp_sns_ptr,sizeof(sns_ptr->no_of_sns_enabled),&sns_ptr->no_of_sns_enabled,sizeof(sns_ptr->no_of_sns_enabled) );
  temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(sns_ptr->no_of_sns_enabled));

  if(sns_ptr->no_of_sns_enabled != 0 )
  {
   /* write all sensor data  */
    memscpy(temp_sns_ptr,(sns_ptr->no_of_sns_enabled * sizeof(cm_sns_data_s_type)),
                        sns_ptr->sns_data_arr,
                       (sns_ptr->no_of_sns_enabled * sizeof(cm_sns_data_s_type)));
    temp_sns_ptr = (uint8*)(temp_sns_ptr + (sns_ptr->no_of_sns_enabled * sizeof(cm_sns_data_s_type)));
  }
  /* write batchfull mask   */

  memscpy(temp_sns_ptr,sizeof(int),&sns_ptr->batch_full_mask,sizeof(int) );
  temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(int));

    /* write policymet mask   */

  memscpy(temp_sns_ptr,sizeof(int),&sns_ptr->policy_met_mask,sizeof(int) );
  temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(int));

    /* write sns fail mask   */

  memscpy(temp_sns_ptr,sizeof(int),&sns_ptr->sns_fail_mask,sizeof(int) );
  temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(int));

  /* write motion detect    */

  memscpy(temp_sns_ptr,sizeof(cm_sns_motion_sns_data_s_type),&(sns_ptr->motion_sns_data),sizeof(cm_sns_motion_sns_data_s_type) );
  temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(cm_sns_motion_sns_data_s_type));

    /* write INTR data    */
#ifdef FEATURE_MMODE_TX_21
  memscpy(temp_sns_ptr,sizeof(cm_sns_intr_sns_data_s_type),&sns_ptr->sns_intr_data,sizeof(cm_sns_intr_sns_data_s_type) );
  temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(cm_sns_intr_sns_data_s_type));

    /* write ODR data    */

  memscpy(temp_sns_ptr,sizeof(cm_sns_on_demand_sns_data_s_type),&sns_ptr->sns_odr_data,sizeof(cm_sns_on_demand_sns_data_s_type) );
  temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(cm_sns_on_demand_sns_data_s_type));

   /* Update logging level */

  memscpy(temp_sns_ptr,sizeof(sns_tracker_log_level),&sns_ptr->cm_sns_logging_level,sizeof(sns_tracker_log_level) );
  temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(sns_tracker_log_level));
#endif
  /* updating total size here and then return the pointer
     pointing to begining of memory chunk*/

  cmefs_write( CMEFS_SOI_CONTEXT, (byte* )sns_mgr_context, (int32)total_size);
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE

  CM_SERIAL_LOG_1("=CM= SNS context write  size is = %d  \n\r ",total_size);
#endif
  cm_mem_free(sns_mgr_context);

}

/*===========================================================================
FUNCTION cm_sns_reterieve_context

DESCRIPTION
  reterive the saved context after psm exit

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cm_sns_reterieve_context(void)
{
  uint8 *sns_context_ptr = NULL,*temp_sns_ptr = NULL;
  int32 context_size = 0,temp_size =0;
  cm_sns_data_collection_s_type*   sns_ptr = cmsns_get_data_info_ptr();
  byte index = 0;
  temp_size = sizeof(uint32);
  if (cmefs_read(CMEFS_SOI_CONTEXT,(byte*)&context_size,&temp_size)== FALSE)
  {
    return;
  }

  if(temp_size == 0 || context_size == 0)
  {
    return ;
  }
  sns_context_ptr = (uint8*)cm_mem_malloc(context_size);

  temp_size = context_size;
  cmefs_read(CMEFS_SOI_CONTEXT,sns_context_ptr,&temp_size);
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE

  CM_SERIAL_LOG_1("=CM= SNS retrieved context size is = %d  \n\r ",context_size);
#endif
  //context_size = cmpsm_retreive_saved_context(CM_PSM_MODULE_SNS_MGR, (uint8 **)&sns_context_ptr);
  if (sns_context_ptr != NULL && context_size != 0)
  {
     //temp_sns_ptr = sns_context_ptr;
     /*  move by the size of the uint32 */
	 temp_sns_ptr = (uint8*)(sns_context_ptr + sizeof(uint32));

     /*  read the psm_full_boot_time   */        
	 memscpy(&sns_ptr->sns_psm_full_boot_time,sizeof(uint64),temp_sns_ptr,sizeof(uint64) );
     temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(uint64));
    
    /* read no of sensor  */
    
    memscpy(&(sns_ptr->no_of_sns_enabled),sizeof(sns_ptr->no_of_sns_enabled),temp_sns_ptr,sizeof(sns_ptr->no_of_sns_enabled) );
    temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(sns_ptr->no_of_sns_enabled));

    if(sns_ptr->no_of_sns_enabled != 0 )
    {
      CM_ASSERT(INRANGE(sns_ptr->no_of_sns_enabled,CM_SNS_TYPE_ALS,CM_SNS_TYPE_ENV_SENSOR_MAX));
        sns_ptr->sns_data_arr =(cm_sns_data_s_type *) cm_mem_malloc(sns_ptr->no_of_sns_enabled * sizeof(cm_sns_data_s_type));
    
    
      /* read all sensor data  */
      memscpy(sns_ptr->sns_data_arr,(sns_ptr->no_of_sns_enabled * sizeof(cm_sns_data_s_type)),
                     temp_sns_ptr,
                    (sns_ptr->no_of_sns_enabled * sizeof(cm_sns_data_s_type)));
      temp_sns_ptr = (uint8*)(temp_sns_ptr + (sns_ptr->no_of_sns_enabled * sizeof(cm_sns_data_s_type)));
    
    }
    
      /* read batchfull mask   */
    memscpy(&sns_ptr->batch_full_mask,sizeof(int),temp_sns_ptr,sizeof(int) );
    temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(int));
    
    /* read policymet mask   */
    
    memscpy(&sns_ptr->policy_met_mask,sizeof(int),temp_sns_ptr,sizeof(int) );
    temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(int));

    /* read sns fail mask   */

    memscpy(&sns_ptr->sns_fail_mask,sizeof(int),temp_sns_ptr,sizeof(int) );
    temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(int));

#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
/*    for sensor only image below should start from reset 
        it is only for full image not resest , to indicate APP data*/
    sns_ptr->batch_full_mask = 0;
    sns_ptr->policy_met_mask = 0;
    sns_ptr->sns_fail_mask = 0;
#endif	
    /* rest the state for each sensor */
    for(index = 0;index < sns_ptr->no_of_sns_enabled ;index++)
	{
	  sns_ptr->sns_data_arr[index].current_sns_state = CM_SNS_STATE_NONE;
	}

    /* read motion detect enable   */
    
    memscpy(&(sns_ptr->motion_sns_data),sizeof(cm_sns_motion_sns_data_s_type),temp_sns_ptr,sizeof(cm_sns_motion_sns_data_s_type) );
    temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(cm_sns_motion_sns_data_s_type));

#ifdef FEATURE_MMODE_TX_21
	  /* read INTR data	*/
	
	memscpy(&sns_ptr->sns_intr_data,sizeof(cm_sns_intr_sns_data_s_type),temp_sns_ptr,sizeof(cm_sns_intr_sns_data_s_type) );
	temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(cm_sns_intr_sns_data_s_type));
	
	  /* read ODR data    */
	
	memscpy(&sns_ptr->sns_odr_data,sizeof(cm_sns_on_demand_sns_data_s_type),temp_sns_ptr,sizeof(cm_sns_on_demand_sns_data_s_type) );
	temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(cm_sns_on_demand_sns_data_s_type));

     /* read logging level    */
   
    memscpy(&sns_ptr->cm_sns_logging_level,sizeof(sns_tracker_log_level),temp_sns_ptr,sizeof(sns_tracker_log_level) );
    temp_sns_ptr = (uint8*)(temp_sns_ptr + sizeof(sns_tracker_log_level));

   // sns_ptr->sns_intr_data.fall_data.freefall_state = CM_SNS_FREEFALL_STATUS_NONE;
	sns_ptr->sns_odr_data.fmd_data.flight_status = CM_SNS_FMD_STATUS_NONE;
    sns_ptr->sns_intr_data.tilt_data.state = CM_SNS_STATE_NONE;
	sns_ptr->sns_intr_data.als_data.state = CM_SNS_STATE_NONE;
#endif	
#ifndef FEATURE_MMODE_TX_21
    sns_ptr->motion_sns_data.motion_sns_state  = CM_SNS_STATE_NONE;
	sns_ptr->motion_sns_data.event  = CM_SNS_MOTION_EVENT_TYPE_UNKNOWN;
    sns_ptr->motion_sns_data.motion_sns_uptime = 0XFFFFFFFF;
	sns_ptr->motion_sns_data.no_motion_timer_expired = FALSE;
#endif
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
#ifdef FEATURE_SERIAL_LOGGING_ENABLE
#ifndef FEATURE_MMODE_TX_21
  CM_SERIAL_LOG_2(" =CM= SNSMGR motion = %d and sns count = %d  \n\r ", sns_ptr->motion_sns_data.motion_detect_enable,sns_ptr->no_of_sns_enabled);
#endif
#endif
#endif
    CM_MSG_HIGH_0("The config from EFS");
	cm_sns_log_cm_sns_config();
    
    cm_sns_update_psm_state(sns_ptr,CM_SNS_PSM_READY_RSP_NONE);  
    cm_mem_free(sns_context_ptr);
    sns_context_ptr = NULL;
  }
 
}  


#endif
