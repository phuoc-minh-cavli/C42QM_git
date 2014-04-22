#ifndef CM_SNS_UTIL_H
#define CM_SNS_UTIL_H
#include "cmsnsmgr.h"

#ifdef FEATURE_REF_TRACKER 
extern void cm_sns_copy_sns_sample_data(cm_qmi_sensor_data_rsp_s_type dest_sample_arr[],uint8 *len);
#ifdef FEATURE_MMODE_TX_21
extern void cm_sns_copy_sns_logging_data(cm_qmi_sensor_data_rsp_s_type dest_sample_arr[],uint8 *len, cm_sns_data_req_info_s_type *  data_req_ptr);
#endif
extern void cmsns_send_sns_sample_per_sid(cm_sns_id_e_type sensorid);
extern boolean cm_send_sns_access_req(cm_sns_id_e_type sensor_type);
extern boolean cm_sns_config_allowed(cm_sns_cmd_s_type* sns_cmd_ptr) ;
extern boolean cm_sns_data_req_allowed(cm_sns_cmd_s_type* sns_cmd_ptr);
boolean cm_sns_od_sns_req_allowed(cm_sns_cmd_s_type* sns_cmd_ptr);

extern boolean cm_sns_check_if_sensors_are_idle(cm_sns_data_collection_s_type*   sns_ptr);
extern void cm_sns_update_psm_state(cm_sns_data_collection_s_type*   sns_ptr,cm_sns_psm_state_e_type rsp_state);
extern void cm_sns_update_sns_current_state(cm_sns_data_collection_s_type*   sns_ptr,cm_sns_state_e_type state,int index);
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

boolean cm_sns_ready_to_send_psm_ready_rsp(void);

extern boolean cm_send_sns_req(cm_sns_id_e_type sensor_type);

/*===========================================================================
FUNCTION cmsns_snsmod_send_sns_sample_rsp

DESCRIPTION
  It send the environment sensor sample rsp to CM task

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_snsmod_send_sns_sample_rsp(cm_sns_id_e_type sns_id,float arr[],int len,uint64 timestamp);


/*===========================================================================
FUNCTION cmsns_snsmod_send_motion_sns_rsp

DESCRIPTION
  it send the motion sensor rsp to CM task

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cmsns_snsmod_send_motion_sns_rsp(cm_sns_motion_event_e_type event,uint32 psm_time);

/*===========================================================================
FUNCTION cmsns_send_app_sample_data_rsp

DESCRIPTION

 The function will send the response to app for sns data request

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/



boolean cmsns_send_app_sample_data_rsp(void);

#ifdef FEATURE_MMODE_TX_21

/*===========================================================================
FUNCTION cmsns_send_app_logging_data_rsp

DESCRIPTION

 The function will send the response to app for sns data request

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/



boolean cmsns_send_app_logging_data_rsp(cm_sns_data_req_info_s_type *  data_req_ptr);

#endif
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

static void cm_sns_reset_current_config( cm_sns_data_collection_s_type*   sns_ptr );


/*===========================================================================
FUNCTION cmsns_send_psm_auto_ready_ext_ind

DESCRIPTION
 It will send the auto ready indication to CM

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cmsns_send_psm_auto_ready_ext_ind(uint32 active_timer,uint8 psm_enter_type);

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

void cm_sns_check_for_batch_full_cond_met(cm_sns_data_collection_s_type*	 sns_ptr,int index);

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

void cm_sns_set_context_change(boolean val);

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

static void cm_sns_reset_current_config( cm_sns_data_collection_s_type*   sns_ptr );


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


boolean cm_sns_check_if_next_batch_cond_met(cm_sns_data_collection_s_type* sns_ptr);


/*===========================================================================
FUNCTION cmsns_send_psm_ready_ind

DESCRIPTION
 It will send the ready ind to CM.
 it is the case where sensor manger rejected the ready request,
 but now ok to proceed. 

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_send_psm_ready_ind(uint32 activate_timer);

/*===========================================================================
FUNCTION cmsns_send_psm_auto_ready_ind

DESCRIPTION
 It will send the auto ready indication to CM

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cmsns_send_psm_auto_ready_ind(uint32 active_timer);

/*===========================================================================
FUNCTION cm_handle_md_interrupt

DESCRIPTION
enable the motion detect interrupt

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/

void cm_handle_md_interrupt(boolean is_enable);



/*===========================================================================
FUNCTION cmsns_process_motion_detect_enable

DESCRIPTION
  it will send enter rsp

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_process_motion_detect_enable(void);


/*===========================================================================
FUNCTION cm_send_sns_registry_req

DESCRIPTION

Send registry req to sns

DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none

===========================================================================*/

boolean cm_send_sns_registry_req(cm_sns_id_e_type sensor_type  );



typedef struct cm_sns_fmd_config_s
{
  float         threshold_pressure;
  float         threshold_accel;      // default: -0.4, no range
  uint32      accel_buffer_count;   // default: 200, range [200, 1000]
  uint32      run_period;    // default: 600 sec, range [0, 3600]
} cm_sns_fmd_config_s_type;

///cm_sns_fmd_config_s_type fmd_config;

/*
 * motion_detect registry write
 */
typedef struct cm_sns_md_config_s
{
  float         thresh;   // default: 0.0766f, no range
  uint32      disable;  // default: 0, 0/1
  float         win;      // default: 0.0f, no range
} cm_sns_md_config_s_type;

///cm_sns_md_config_s_type md_config;

/*
 * tilt_tracker registry write
 */
typedef struct cm_sns_tilt_tracker_config_s
{
  uint8   max_init_trial_num;
  uint8   max_update_trial_num;
  float      init_gravity_threshold;
  float      update_gravity_threshold;
  uint32   iteration_delay;
   boolean gravity_init_success;
    uint8 transportation_type;
} cm_sns_tilt_tracker_config_s_type;

///cm_sns_tilt_tracker_config_s_type tilt_tracker_config;

typedef struct cm_sns_mcd_config_s
{
   uint16 mcd_motion_toggle_delay_threshold;
   uint8 mcd_motion_toggle_counter_threshold;
   uint8 mcd_psm_timer_config;
} cm_sns_mcd_config_s_type;

///cm_sns_mcd_config_s_type mcd_config;

typedef struct cm_sns_smd_config_s
{
   float smd_perceptron_weight[20];
   float smd_perceptron_min_weight[4];
   float smd_perceptron_max_weight[4];
   uint8 smd_sum_score_threshold;
} cm_sns_smd_config_s_type;

//cm_sns_smd_config_s_type smd_config;

typedef struct cm_sns_reg_config_s

{

 cm_sns_smd_config_s_type smd_config_reg;
 cm_sns_mcd_config_s_type mcd_config_reg;
 cm_sns_tilt_tracker_config_s_type tilt_tracker_config_reg;
 cm_sns_md_config_s_type md_config_reg;
 cm_sns_fmd_config_s_type fmd_config_reg;
 
}cm_sns_reg_config_s_type;


extern cm_sns_reg_config_s_type reg_config_global;

extern cm_sns_reg_config_s_type *get_reg_config(void);

#endif
#endif
