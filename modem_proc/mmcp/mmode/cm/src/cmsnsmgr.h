#ifndef CM_SNS_MGR_H
#define CM_SNS_MGR_H
#include "cmi.h"
#include "comdef.h"
#include  "cmidbg.h"
#include  "cmtaski.h"

#ifdef FEATURE_MMODE_TX_21
#ifndef CM_DEBUG 
#include "stdbool.h"
#endif
#include "sns_tracker_log.h"
#endif

//#include "cm_msgr_msg.h"
//#include "cm_v.h"

#ifdef FEATURE_REF_TRACKER 


#ifdef FEATURE_MMODE_TX_21

#define OPERATION_RANGE 5
#define ALERT_HIGH 4
#define ALERT_LOW 3
#define PRE_ALERT_HIGH 2
#define PRE_ALERT_LOW 1
#define LOGGING 0 // LOGGING is also INITIALISATION.

#define ACK_SUCCESS 1
#define ACK_ERROR 2

typedef struct cm_sns_phy_test_config_s{
  cm_sns_id_e_type sensor_type;
  boolean is_enabled;
  boolean cm_sns_phy_test_req;
  boolean cm_sns_phy_test_get_data_req;
  uint8 cm_sns_test_type;
  float sample_rate;
  uint32 report_rate;
  uint32 next_report_time;
  boolean serial_number_req;
 }cm_sns_phy_test_config_s_type;

extern cm_sns_phy_test_config_s_type cm_sns_test_data_global;

struct config_init_input_msg {
  int sensor_type;
  int operating_mode;
  float alarm_thr_high;
  float alarm_thr_low;
  float pre_alert_thr_high;
  float pre_alert_thr_low;
  int performance_mode; // Low performance mode = 0, high performance mode = 1.
  int alarm_hysteresis_duration; // Customer configured hysteresis duration.
  unsigned long HPM_sampling_period; // High performance mode sampling period, in seconds.
  unsigned long measurement_period; // Normal operation/low performance mode measurement period, in seconds.
  };

struct config_init_output_msg {
  int sensor_type;
  int ACK;
  int ACK_error_config;
  int ACK_error_type;
  };
    
void sns_env_aware_config_init_interface(struct config_init_input_msg input, struct config_init_output_msg *output);

struct algo_init_input_msg {
  int sensor_type;
  int operating_mode;
  float alarm_thr_high;
  float alarm_thr_low;
  float pre_alert_thr_high;
  float pre_alert_thr_low;
  int performance_mode; // Low performance mode = 0, high performance mode = 1.
  int alarm_hysteresis_duration; // Customer configured hysteresis duration.
  unsigned long HPM_sampling_period; // High performance mode sampling period, in seconds.
  unsigned long measurement_period; // Normal operation/low performance mode measurement period, in seconds.
  float *data_buffer; // Array for slope estimation.
  unsigned long long *time_buffer; // Array for slope estimation.
  int hysteresis_counter; // Tracks amount of time remaining in hysteresis.
  int algorithm_state; // Indicates previous event.
  int current_count;
  int max_count;
  int valid_buffer_count;
  };

struct measurement_input_msg {
  int sensor_type;
  float sensor_sample;
  unsigned long long timestamp;
  };

struct measurement_output_msg {
  int sensor_type;
  int event_type;
  float TTC;
  float slope;
  unsigned long long timestamp;
  float sensor_sample;
  unsigned long requested_measurement_period; // In seconds
  int algorithm_state;
  float *data_buffer;
  unsigned long long *time_buffer;
  int hysteresis_counter;
  int valid_buffer_count;
  };

void sns_env_aware_algo_init_interface(struct algo_init_input_msg input);
    
void sns_env_aware_measurement_interface(struct measurement_input_msg input, struct measurement_output_msg *output);


typedef struct config_init_output_msg config_init_output;
typedef struct config_init_input_msg config_init_input;
typedef struct algo_init_input_msg algo_init_input;
typedef struct measurement_output_msg meas_output;
typedef struct measurement_input_msg meas_input;

extern config_init_output* cm_send_algo_output();
extern cm_sns_phy_test_config_s_type* cm_sns_get_phy_test_config(void);
#endif

typedef enum cm_sns_state_e{
 /* Default state when sensor is up   */
 CM_SNS_STATE_NONE,
 /* State when request for sensor in queue, to be sent */
 CM_SNS_STATE_REQ_QUEUE,
 /*State when request for sensor is in progress */
 CM_SNS_STATE_REQ_IN_PROGRESS,
  /*State when Indication sent to app and waiting for query */
 CM_SNS_STATE_IND_SENT_TO_APP,
/* state when app query for the data of that sensor,
   APP can ask for all indicated sensor, can ask
   for less sens data or more sens data */
 CM_SNS_STATE_APP_SENT_DATA_REQ,
/* state when response has been received for this 
   also it will be use for debugging while crashing
   to differentiate powr up case and request has beeen
   processed*/
 CM_SNS_STATE_DONE

}cm_sns_state_e_type;


typedef enum cm_sns_psm_state_e
{
  /* In this state we can send auto ready,init state */
  CM_SNS_PSM_READY_RSP_NONE,
  /* auto ready start procedure */
  CM_SNS_PSM_AUTO_READY_STARTED,
  /*  In this  state we can loop back the request         */
  CM_SNS_PSM_AUTO_READY_SENT,
  /*  CM_SNS PSM_READY_STARTED  */
  CM_SNS_PSM_READY_RSP_STARTED,
  /* In this state no need to do any thing,we can't do anything */
  CM_SNS_PSM_READY_RSP_SEND,
  /*In this state we need to send ready rsp,as it was rejected earlier  */
  CM_SNS_PSM_READY_RSP_REJECTED,
  /* In this state we have startd processing of PSM enter for SNS mgr  */
  CM_SNS_PSM_ENTER_REQ_STARTED,
  CM_SNS_PSM_ENTER_RSP_SENT

}cm_sns_psm_state_e_type;

typedef enum cm_sns_hw_intrpt_state_e
{
  CM_SNS_HW_INTRPT_TYPE_NONE,
  CM_SNS_HW_INTRPT_TYPE_USB_DETECTED,
  CM_SNS_HW_INTRPT_TYPE_KEY_PRESSED,
  CM_SNS_HW_INTRPT_TYPE_IN_PROCESS,
  CM_SNS_HW_INTRPT_TYPE_PROCESSED,
  CM_SNS_HW_INTRPT_TYPE_MAX
}cm_sns_hw_intrpt_state_e_type;

/*  write comment  */ 

typedef struct cm_sns_data_s
{
  cm_sns_config_data_s_type sns_config_data;
  /* currently going with array we con think of link if required  */
  cm_sns_sample_data_s_type     sns_sample_data[CM_MAX_BATCH_SUPPORTED];
  int                       current_batch_count;
  int                       max_count;
  cm_sns_state_e_type       current_sns_state;
  uint32                    sns_uptime;
#ifdef FEATURE_MMODE_TX_21
  uint8 valid_buffer_count;
  uint8 algorithm_state;
  uint8 event_type;
  float TTC;
  float slope;
#endif
}cm_sns_data_s_type;

typedef struct cm_sns_motion_sns_data_s
{
  boolean mcd_enable;
  uint32 mcd_psm_time;

  #ifndef FEATURE_MMODE_TX_21
   /*this indicate if motion is configure to read   */
  boolean motion_detect_enable;

  /* No of time(sec) to wait for response from sensor  */
  uint32 motion_sns_uptime;

  /* current sensor state  */
  cm_sns_state_e_type  motion_sns_state;

  /* current wait count for sensor */
  cm_sns_motion_event_e_type event;
  /* current motion event */
  boolean   no_motion_timer_expired;

  /* will let us know how much time UE should wait for SMD before moving to 
    PSM*/
  uint8 sns_smd_wait_time;  
  #endif
}cm_sns_motion_sns_data_s_type;
typedef struct cm_sns_intr_sns_als_data_s
{
  boolean is_enable;
  uint32  threshold;
  uint32  interval;
  uint32  als_sns_uptime;
  cm_sns_state_e_type state;
}cm_sns_intr_sns_als_data_s_type;

typedef enum cm_sns_freefall_state
{
  CM_SNS_FREEFALL_STATUS_NONE,
  CM_SNS_FREEFALL_NOT_DETECT = 1, 
  CM_SNS_FREEFALL_DETECT = 2,
  CM_SNS_FREEFALL_MAX
}cm_sns_freefall_state_e_type;

typedef struct cm_sns_intr_sns_fmd_data_s
{
  boolean is_enable;
   #ifndef FEATURE_MMODE_TX_21
  boolean is_pressure_enable;
  uint16  pressure_threshold;
  byte    accel_sample_rate;
  byte    accel_sample_length;
  #endif
  cm_sns_flight_mode_state_e_type flight_status;
 }cm_sns_intr_sns_fmd_data_s_type;

typedef struct cm_sns_intr_sns_fall_data_s
{
  boolean is_enable;  
  cm_sns_freefall_state_e_type freefall_state;

}cm_sns_intr_sns_fall_data_s_type;

typedef struct cm_tilt_gravity_vector_s
{
  boolean gravity_init_success;
  float gravity_init_vector_x;
  float gravity_init_vector_y;
  float gravity_init_vector_z;

} cm_tilt_gravirty_vector_s_type;


typedef enum cm_sns_tilt_tracker_transportation_type 
{    
  CM_SNS_TILT_TRACKER_TRANSPORTATION_TYPE_UNKNOWN_0 = 0,
  CM_SNS_TILT_TRACKER_NO_TRANSPORTATION_OR_STATIONARY_1 = 1,
  CM_SNS_TILT_TRACKER_TRANSPORTATION_TYPE_TRAIN_2 = 2,
  CM_SNS_TILT_TRACKER_TRANSPORTATION_TYPE_AIRPLANE_3 = 3,
  CM_SNS_TILT_TRACKER_TRANSPORTATION_TYPE_TRUCK_4 = 4,
  CM_SNS_TILT_TRACKER_TRANSPORTATION_TYPE_WORST_CASE_5 = 5
} cm_sns_tilt_tracker_transportation_type;


typedef struct cm_tilt_config_s
{
  boolean is_enable;

  cm_sns_tilt_tracker_transportation_type transportation_type;

  boolean has_redo_gravity_init;	
  boolean redo_gravity_init;    
  boolean has_iteration_delay;	
  uint32 iteration_delay;	
  boolean has_max_init_trial_num;	
  uint8 max_init_trial_num;	
  boolean has_max_update_trial_num;    
  uint8 max_update_trial_num;	 
  boolean has_init_gravity_threshold;	 
  float init_gravity_threshold;	 
  boolean has_update_gravity_threshold;	
  float update_gravity_threshold;


}cm_tilt_config_s_type;


typedef struct cm_sns_intr_sns_tilt_data_s
{
  boolean is_enable;
  #ifndef FEATURE_MMODE_TX_21
  boolean has_gravity_updated;
  cm_tilt_gravirty_vector_s_type tilt_gravity_data;
  #endif
  boolean has_config_change;
  cm_tilt_config_s_type config_data;
  boolean need_to_enqueue;
  cm_sns_state_e_type state;
  uint32 tilt_sns_uptime;
  float tilt_angle_val;
}cm_sns_intr_sns_tilt_data_s_type;

typedef struct cm_sns_intr_sns_data_s
{
  cm_sns_intr_sns_als_data_s_type als_data;
  cm_sns_intr_sns_fall_data_s_type fall_data;
  cm_sns_intr_sns_tilt_data_s_type tilt_data;
}cm_sns_intr_sns_data_s_type;

typedef struct cm_sns_on_demand_sns_data_s
{
    cm_sns_intr_sns_fmd_data_s_type fmd_data;

}cm_sns_on_demand_sns_data_s_type;


typedef struct  cm_sns_data_collection_s
{
  /*dynamic memory allocated at time of sensor config  */
 cm_sns_data_s_type *sns_data_arr;
 /* in curent config no of sensor supported out of max supported  */
 byte      no_of_sns_enabled;
/*  mask for sensord ids, which batch are full */
  uint32      batch_full_mask;
/*  mask of sensor ids which met the contion   */
  uint32     policy_met_mask;
/*  mask of sensor ids which failed   */
  uint32 sns_fail_mask;
/* Ready rsp stat */
  cm_sns_psm_state_e_type sns_ready_rsp_state;
/*   should be read from 
    efs*/
  uint32 sns_bootup_time;

/* Motion sensor data config  */
  cm_sns_motion_sns_data_s_type motion_sns_data;


/* will tell the SOI PSM entry , wheter it is shutdown or reset  */
  uint8 soi_psm_entry_type;

  boolean force_motion_enable;

 /*  sns hw intrpt type sent by pmic */
  cm_sns_hw_intrpt_state_e_type  intrpt_status;
  uint64  sns_psm_full_boot_time;

  uint32 sns_sensor_rsp_timer;
  /**** new change  ***/
  cm_sns_intr_sns_data_s_type   sns_intr_data;
  uint32 intr_sns_prog_mask ;
  cm_sns_on_demand_sns_data_s_type sns_odr_data;
  #ifdef FEATURE_MMODE_TX_21
  sns_tracker_log_level cm_sns_logging_level;
   uint32 sns_registry_mask;
  #endif
 }cm_sns_data_collection_s_type;


typedef enum cm_snsmgr_state_e
{
  /**   change ***/
   CM_SNS_COND_INTR_CHECK_STATE = 1,
   CM_SNS_COND_CHECK_MOTION_STATE ,
   CM_SNS_COND_CHECK_POLICY_MET_STATE,
   CM_SNS_COND_CHECK_BATCH_FULL_MET_STATE,
   CM_SNS_COND_CHECK_SNS_FAIL_STATE,
   CM_SNS_EVALUATION_STATE ,
   CM_SNS_SENSOR_REQ_STATE,
   CM_SNS_SENSOE_DONE_STATE,
   CM_SNS_SENSOR_MAX_STATE
}cm_snsmgr_state_e_type;

#define CM_DEFAULT_MOTION_WAIT_TIME  60 /*60sec  for debug purpose*/
#define CM_MIN_SMD_TIME_OUT_VALUE  15 /* 15 sec min need as per sensor team  */

extern cm_snsmgr_state_e_type next_sns_state_g ;
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

extern cm_sns_data_collection_s_type* cmsns_get_data_info_ptr(void);
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

extern void cm_snsmgr_init();

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


extern void cmsns_client_cmd_proc(cm_sns_cmd_s_type * cmd);
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

extern void cmsns_msgr_cmd_proc(cm_sns_mod_sensor_cmd_u_type * sns_smpl_rsp);

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

extern void cm_snsmod_send_env_sns_sample_rsp(cm_sns_id_e_type sns_id,float arr[],int len,uint64 timestamp);

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

extern uint32 cm_sns_get_active_time_value(void);

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

extern boolean cm_sns_check_context_change(void);


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


extern void cm_sns_write_context(void);


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

extern void cm_sns_init_sns_data_collection( );


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

extern void cm_sns_process_config_cmd(cm_sns_cmd_s_type* sns_cmd_ptr);


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

extern void cm_sns_process_data_request_cmd(cm_sns_cmd_s_type* sns_cmd_ptr);

/*===========================================================================
FUNCTION cm_sns_process_reset_config_cmd

DESCRIPTION
  Process rest config command 

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cm_sns_process_reset_config_cmd(cm_sns_cmd_s_type* sns_cmd_ptr);


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

void cm_sns_process_od_sns_req_cmd(cm_sns_cmd_s_type* sns_cmd_ptr);

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

void cm_sns_process_phy_test_req_cmd(cm_sns_cmd_s_type* sns_cmd_ptr);

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

void cm_sns_process_phy_test_get_data_req_cmd(cm_sns_cmd_s_type* sns_cmd_ptr);

#ifdef FEATURE_MMODE_TX_21
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

void cm_sns_set_logging_level_cmd(cm_sns_cmd_s_type* sns_cmd_ptr);

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

void cm_sns_process_get_serial_num(cm_sns_cmd_s_type* sns_cmd_ptr);

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

void  cm_sns_handle_motion_sensor_config_req(boolean motion_enable);
#endif


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


extern void cm_sns_handle_sample_rsp(cm_sns_id_e_type sensor_id,cm_sns_sample_data_s_type sample_buffer);



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


extern void cm_sns_handle_motion_sensor_rsp(cm_sns_motion_event_e_type event,uint32 psm_time);

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

extern boolean cm_sns_find_sesnors_polling_time_reached(  cm_sns_data_collection_s_type* sns_ptr);


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


extern boolean cm_sns_send_sensor_request(cm_sns_data_collection_s_type* sns_ptr);


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

extern boolean cm_sns_evaluate_and_send_ready_rsp(cm_sns_data_collection_s_type* sns_ptr);



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

extern uint32 cm_sns_get_min_sns_uptime (void);

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

extern boolean cm_sns_is_request_in_progress(cm_sns_data_collection_s_type* sns_ptr);

/*===========================================================================
FUNCTION cmsns_get_event_info

DESCRIPTION
  Check if any sensor is still waiting for the request/response

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

extern void cmsns_get_event_info(cm_sns_event_info_s_type * sns_event_info);

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


boolean cm_sns_check_if_motion_possible(cm_sns_data_collection_s_type*  sns_ptr );

#endif
/*===========================================================================
FUNCTION cm_disable_smd_stream

DESCRIPTION
disable the SMD stream

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/


void cm_disable_smd_stream(void);

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


void cm_sns_enable_motion_detect_intr(void);


/*===========================================================================
FUNCTION cmsnsmgr_is_psm_entry_in_progress

DESCRIPTION
 It return TRUE,if SNS MGR is processing the PSM ENTER request
DEPENDENCIES
none

RETURN VALUE
boolean if SNSMGR processing PSM

SIDE EFFECTS
none

===========================================================================*/


boolean cmsnsmgr_is_psm_entry_in_progress(void);


/*===========================================================================
FUNCTION cm_disable_sns_stream

DESCRIPTION
disable the sensor stream 

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/

void cm_disable_sns_stream(cm_sns_id_e_type type);


typedef cm_snsmgr_state_e_type (* state_func_ptr_t)(cm_sns_data_collection_s_type *sns_ptr,boolean *is_cont);

typedef struct cm_snsmgr_state_mach_e
{
  cm_snsmgr_state_e_type  state_name;
  state_func_ptr_t  func_ptr;
}cm_snsmgr_state_mach_e_type;




cm_snsmgr_state_e_type cmsns_motion_state_func(cm_sns_data_collection_s_type *sns_ptr,boolean *is_cont);

cm_snsmgr_state_e_type cmsns_cond_met_state_func(cm_sns_data_collection_s_type *sns_ptr,boolean *is_cont);
cm_snsmgr_state_e_type cmsns_evaluation_state_func(cm_sns_data_collection_s_type *sns_ptr,boolean *is_cont);
cm_snsmgr_state_e_type cmsns_sensor_req_state_func(cm_sns_data_collection_s_type *sns_ptr,boolean *is_cont);
cm_snsmgr_state_e_type cmsns_sensor_done_state_func(cm_sns_data_collection_s_type *sns_ptr,boolean *is_cont);
#ifdef FEATURE_MMODE_TX_21

cm_snsmgr_state_e_type cmsns_intr_check_state_func(cm_sns_data_collection_s_type *sns_ptr,boolean *is_cont);
#endif



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

void cmsns_snsmod_send_sns_qmi_cmd_rsp(void *user_handle,
                                      unsigned int msg_id,
                                      void *ind_buf,
                                      unsigned int ind_buf_len);


/*===========================================================================
FUNCTION cm_disable_sns_stream_by_sensor_id

DESCRIPTION
disable the sensor stream by sensor id

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/

void cm_disable_sns_stream_by_sensor_id(cm_sns_id_e_type sns_id);

/*===========================================================================
FUNCTION cm_sns_process_qmi_sns_cmd_rsp

DESCRIPTION

QMI sensor call this call back, if there is any response

DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none

===========================================================================*/
void cm_sns_process_qmi_sns_cmd_rsp(void * user_handle, unsigned int msg_id,
                          void *ind_buf, unsigned int ind_buf_len);


/*===========================================================================
FUNCTION cm_send_sns_enable_req_wrapper

DESCRIPTION

enabling sensosr request

DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none

===========================================================================*/

boolean cm_send_sns_enable_req_wrapper();

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

extern void cm_sns_get_registry_config(cm_sns_cmd_s_type* sns_cmd_ptr);




/*===========================================================================
FUNCTION cm_send_registry_read_req_wrapper

DESCRIPTION
 Registry comeback

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

extern void cm_send_registry_read_req_wrapper(cm_sns_id_e_type sns_cmd_ptr);

#endif
#endif

