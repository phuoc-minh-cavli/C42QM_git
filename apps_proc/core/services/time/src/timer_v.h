#ifndef TIMER_V_H
#define TIMER_V_H
/*=============================================================================

 T I M E R  S E R V I C E  I N T E R N A L

GENERAL DESCRIPTION
 Provides internal structures and function used by timer. 

EXTERNALIZED FUNCTIONS

<WRITE THIS PART>

Copyright (c) 2009 - 2017, 2019 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/09/15   ab      Added this file while Memory reduction changes

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "timer.h"
#include "queue.h"
#include "qapi_timer.h"

/*=============================================================================
                           CONFIGURATION MACROS
=============================================================================*/
/* Freq of the timertick counter */
#define TIMER_FREQ_VALUE          19200000

/* The range (in timeticks) of each timer bin 
    - This must be (2^n - 1) to work (for bitwise optimizations)
    - 8191 = 0.25 sec bins */
#define TIMER_BIN_RANGE           TIMER_FREQ_VALUE/4

/* Factor to divide 19.2MHz clock into 32kHz */
#define XO_TO_SCLK_CONV_FACTOR 586

/* This range is a result of the following calculation...
In general we can say that the oscillator has a +/- 100% swing in freq
Therefore lower bound on Latency(Ticks/ms) = 2*f/1000 
If 5ms is max then TIMER_SCLK_LATENCY = (2*f/1000)*5 = f/100 
*/
#define TIMER_SCLK_LATENCY        TIMER_FREQ_VALUE/100

/*-----------------------------------------------------------------------------
  TIMER STRUCTURE SIZE CONFIGURATION
-----------------------------------------------------------------------------*/
/* Number of bins available. This must be large enough to accomodate all the
   timers in the system, or an err_fatal will be reached */ 
#define TIMER_BIN_NUM             64 

/* Max elements in the timer array. */
/* Note: Changing this number will effect number of active timers
   at a time in the system. Hence should be careful before doing
   any change */
#define MAX_TIMER_BUFFER_NUMBER   64
#define MIN_TIMER_BUFFER_NUMBER    -1

/* Number of non-defer expiry notification clients */
#define TIMER_NON_DEFER_EXPIRY_NOTIFY_CLIENTS_MAX_CNT 4

/*-----------------------------------------------------------------------------
  TIMER DEBUG STRUCTURE SIZE CONFIGURATION
-----------------------------------------------------------------------------*/
#define TIMER_TRACE_LENGTH                 64
#define MAX_TIMER_API_CALLS                64
#define MAX_MATCH_REC                      32
#define MAX_TIMER_SET                      64
#define MAX_TIMER_CLR_LOG                  64
#define MAX_TIMER_EXPIRED_DEBUG            128
#define MAX_TIMER_EXPIRED                  32
#define MAX_TIMER_REMOTE_EXPIRED           8
#define MAX_TIMER_ISOLATED                 64
#define MAX_TIMER_ASSERT_COMPARE_LIST      16
#define MAX_TIMER_REFERNCE_COUNT_LOG       8
#define MAX_TIMER_MULTI_NOTIFY             4
#define MAX_TIMER_SET_PARAMS_LOG           8

/*-----------------------------------------------------------------------------
  MACRO for locking/unlocking the pimutex.
-----------------------------------------------------------------------------*/
/*For ARM9*/
#ifdef FEATURE_TIMER_REX_IS_AVAILABLE	/* Todo: Change from FEATURE_QDSP6 in scons as well */
#define ATS_INTLOCK() INTLOCK()
#define ATS_INTFREE() INTFREE()

#define ATS_INTLOCK_SAV(x) INTLOCK_SAV(x)
#define ATS_INTFREE_SAV(x) INTFREE_SAV(x)      

#define TIMER_TRACE_MUTEX_LOCK() INTLOCK()
#define TIMER_TRACE_MUTEX_UNLOCK() INTFREE()
#define TIMER_PC_MASK_MUTEX_LOCK() INTLOCK()
#define TIMER_PC_MASK_MUTEX_UNLOCK() INTFREE()
#define TIMER_OS_EARLY_TOL 0
#define TIMER_MAX_EXPIRY 0xFFFFFF00000000uLL

#else /*else for Qurt*/

/* Extern for the timer mutex objects */
extern qurt_mutex_t timer_qemu_mutex;
extern qurt_mutex_t timer_trace_mutex;
extern qurt_mutex_t timer_pc_mask_mutex;

/*MACRO for locking/unlocking the mutex*/
#define ATS_INTLOCK()  \
  qurt_mutex_lock(&timer_qemu_mutex); \
  
#define ATS_INTFREE()  \
  qurt_mutex_unlock(&timer_qemu_mutex); \

#define ATS_INTLOCK_SAV(X) \
  qurt_mutex_lock(&timer_qemu_mutex); \
  
#define ATS_INTFREE_SAV(X)   \
  qurt_mutex_unlock(&timer_qemu_mutex); \

#define TIMER_TRACE_MUTEX_LOCK()  \
  qurt_mutex_lock(&timer_trace_mutex); \
  
#define TIMER_TRACE_MUTEX_UNLOCK()  \
  qurt_mutex_unlock(&timer_trace_mutex); \

#define TIMER_PC_MASK_MUTEX_LOCK()  \
  qurt_mutex_lock(&timer_pc_mask_mutex); \

#define TIMER_PC_MASK_MUTEX_UNLOCK()  \
  qurt_mutex_unlock(&timer_pc_mask_mutex); \

#define TIMER_OS_EARLY_TOL 1920

/* MAX timer expiry duration */
#define TIMER_MAX_EXPIRY 0xFFFFFF00000000uLL
//#define TIMER_MAX_EXPIRY QURT_TIMER_MAX_DURATION_TICKS

#endif /*FEATURE_QDSP6*/

/*-----------------------------------------------------------------------------
  Signals used by the timer task.
-----------------------------------------------------------------------------*/
/* Signal for a timer expiring */
#define TIMER_EXPIRY_SIG          0x00000001

/* Signal for a change in the active timer list */
#define TIMER_CHANGE_SIG          0x00000002

/* Signals which must cause the active timer list to be processed */
#define TIMER_PROCESS_SIGS        (TIMER_EXPIRY_SIG | TIMER_CHANGE_SIG)

/* Signals mask for timer_sleep API Stub  */
#define TIMER_SLEEP_CB_SIG_MASK 0x00000008

/* Timer signal registered with QURT */
#define TIMER_QURT_SIG_MASK 0x2

/*-----------------------------------------------------------------------------
  Other timer configuration params.
-----------------------------------------------------------------------------*/
/* Array index for timers in the PC mask count */
#define TIMER_PC_MASK_INDEX                0

/* Number of entries to manage for PC count */
#define TIMER_PC_DEFER_UNDEFER_CNT_INDEX   4

/* Invalid Handle */
#define TIMER_BUFFER_INVALID_HANDLE   -1

#define INDEX_TO_HANDLE(x)   ( (x) +1 )
#define HANDLE_TO_INDEX(x)   ( (x) -1 )
#define TIMER_HANDLE_CONSTANT 0xC3
#define GROUP_HANDLE_CONSTANT 0xC3C3

#define GROUP_INVALID_HANDLE 0
#define TIMER_INVALID_HANDLE 0

#define MAGIC_INITIALIZER         0xDEADBEEF
#define MAGIC_INITIALIZER_64      0xDEADBEEFD00DFEEDuLL

#define TIMER_NON_DEFER_NOTIFICATION_MOCK_PTR ((timer_ptr_type)0x88888888)

#define TIMER_UNDEFINED_DURATION 0xFFFFFFFFFFFFFFFFuLL

/*=============================================================================

                           TYPEDEF's DEFINITIONS

=============================================================================*/

/** Pointer to timer list structure */
typedef struct timer_list_struct  *timer_list_ptr;
/** Pointer to timer bin structure */
typedef struct timer_bin_struct   *timer_bin_ptr;

/* QAPI User module Notification Handler type */
typedef void (*timer_qapi_notification_handler_type)
(
  /* Timer pointer */
  timer_ptr_type 		timer_ptr
);


/*=============================================================================

                           ENUMERATIONS

=============================================================================*/

/** Timer Queue flags with magic number */
typedef enum timer_internal_queue
{
  TIMER_IN_FREE_Q = 0x0BABABAB,
  TIMER_NOT_IN_FREE_Q = 0x0CACACAC 
} timer_internal_q;

/* Enumerated type for Parameters */
typedef enum
{
  TIMER_IS_ACTIVE,
  TIMER_IS_PERIODIC,
  TIMER_EXPIRES_AT_PARAM,
  TIMER_GET_START_64,
  TIMER_GET_GROUP,
  TIMER_GET_EXPIRY_TYPE,
  TIMER_MAX_PARAMS
} timer_param_type;

/* Enums for timer API */
typedef enum
{
  TIMER_API_LOG_TIMER_INIT = 0x1,
  TIMER_API_LOG_TIMER_DEF,
  TIMER_API_LOG_TIMER_DEF_OSAL,
  TIMER_API_LOG_TIMER_DEF2,
  TIMER_API_LOG_TIMER_SET,
  TIMER_API_LOG_TIMER_SET_ABSOLUTE,
  TIMER_API_LOG_TIMER_GET,
  TIMER_API_LOG_TIMER_GET_PARAMS,
  TIMER_API_LOG_TIMER_SET_PARAMS,
  TIMER_API_LOG_TIMER_CLR,
  TIMER_API_LOG_TIMER_STOP,
  TIMER_API_LOG_TIMER_UNDEF,
  TIMER_API_LOG_TIMER_CLR_TASK_TIMERS,
  TIMER_API_LOG_TIMER_CLR_OSAL_THREAD_TIMERS,
  TIMER_API_LOG_TIMER_GROUP_ENABLE,
  TIMER_API_LOG_TIMER_GROUP_DISABLE,
  TIMER_API_LOG_TIMER_GROUP_SET_DEFERRABLE,
  TIMER_API_LOG_TIMER_GET_TIME_TILL_FIRST_NON_DEFERRABLE_TIMER,
  TIMER_API_LOG_TIMER_DEFER_MATCH_INTERRUPT,
  TIMER_API_LOG_TIMER_UNDEFER_MATCH_INTERRUPT,
  TIMER_API_LOG_TIMER_RESTART,
  TIMER_API_LOG_TIMER_REG,
  TIMER_API_LOG_TIMER_GET_SCLK_TILL_EXPIRY,
  TIMER_API_LOG_TIMER_SET_WAKEUP,
  TIMER_API_LOG_TIMER_GET_QURT_SIG_REGISTRATION_ID,
  TIMER_API_LOG_TIMER_INIT_QURT_SIGNAL,
  TIMER_API_LOG_TIMER_UPDATE_ISR_TIMER,
  TIMER_API_LOG_TIMER_EXPIRES_AT,
  TIMER_API_LOG_TIMER_GET_START,
  TIMER_API_LOG_TIMER_GET_GROUP,
  TIMER_API_LOG_TIMER_GET_EXPIRY_TYPE,
  TIMER_API_LOG_TIMER_PAUSE,
  TIMER_API_LOG_TIMER_RESUME,
  TIMER_API_LOG_TIMER_IS_ACTIVE,
  TIMER_API_LOG_TIMER_IS_PERIODIC,
} timer_api_type;

/*This enum will be used to specify the event
 responsible for setting the match value*/
typedef enum
{
  TIMER_MVS_TIMER_PROCESSED = 0x1,
  TIMER_MVS_TIMER_SET,
  TIMER_MVS_DEFER_MATCH_INT,
  TIMER_MVS_UNDEFER_MATCH_INT,
  TIMER_MVS_GROUP_DISABLED,
  TIMER_MVS_GROUP_ENABLED,
  TIMER_MVS_TIMER_CLEARED,
  TIMER_MVS_TIMER_PAUSED,
  TIMER_MVS_TIMER_RESUMED,
  TIMER_MVS_TIMER_CLEAR_TASK_TIMERS,
  TIMER_MVS_SET_WAKEUP,
  TIMER_MVS_TIMER_UNDEFINED,
}timer_match_interrupt_setter_type;

/*=============================================================================

                           STRUCTURE TYPEDEF's

=============================================================================*/
/** Timer cache structure type */
typedef struct
{
  /** Original set duration */
  time_timetick_type                   duration;

  /** Converted set duration */
  time_timetick_type                   duration_sclk;

  /** Original reload increment */
  time_timetick_type                   reload;

  /** Converted reload increment */
  time_timetick_type                   reload_sclk;

  /** Time units */
  timer_unit_type                      unit;
}
timer_cache_type;


/** Internal Timer struture referenced by external timer structure declared in 
timer.h through handle */
typedef struct timer_struct_internal
{
  /* Link for the next queue */
  q_link_type                     link;
  
  /* Index of timer_buffer array */
  int32                           index;
  
  /* Group to which timer is a member */
  timer_group_ptr                 group;

  /** Task to signal when timer expires, or task to use for APC */
  time_osal_notify_obj_ptr        notify_obj_ptr;

  /** Thread ID of the client using the timer */
  time_osal_thread_id			  thread_id;  

  /** Signal(s) to set in task, when timer expires */
  time_osal_notify_data           sigs;

  /** APC (Asynchronous Procedure Call) function to call when timer expires */
  timer_t1_cb_type                func1;

  /** Clock callback style function to call when timer expires */
  timer_t2_cb_type                func2;

  /** Data for APC function, when timer expires */
  timer_cb_data_type              data;

  /** Callback type for the timer */
  timer_notify_type               notify_type;   

  /** The list the timer is on.  "list" will contain:
       "&active_timers" if the timer is active;
       "&timer->group->list" if the timer is active, but in a disabled group;
       "NULL" if the timer is not active. */
  timer_list_ptr                  list;

  /** Slow clock tick count timer expires at (list != NULL),
     or remaining ticks until expiry if timer is paused (list == NULL) */
  time_timetick_type                   expiry;

  /** Reload offset when timer expires (0 = no reload at expiry) */
  time_timetick_type                   reload;

  /** Slow clock tick count value when timer was set (started) */
  time_timetick_type                   start;

  /** Slow clock delta count value when timer was set (started) */
  uint32                          sclk_delta;

  /** Durations and reload times, original units and converted to sclks */
  timer_cache_type                cache;

  /** Pointer to the next timer in the list (list != NULL) */
  timer_ptr_internal_type         next;

  /** Pointer to the previous timer in the list (list != NULL) */
  timer_ptr_internal_type         prev;

  /** Pointer to the bin for this timer */
  timer_bin_ptr                   bin;

  /* Enum depicting if timer_internal_structure is in queue */
  timer_internal_q                internal_q;          
  
  /* Backward pointer to external timer */
  timer_ptr_type                  timer_ext;

  /*to store the assignment of slave task*/
  uint8                          slave_task_assigned;

  /*to store timer priority*/
  timer_priority_enum_type       timer_priority;

  /*handle for every threadx module */
  uint32                         timer_qapi_proc_hdle;
  
  /* Proc handle for the QAPI timer */
  timer_qapi_process                timer_qapi_proc_id;
}
timer_internal_type;


/*-----------------------------------------------------------------------------
  Timer Bin structure type
    Represents a single bin used for fast insertion into the timer list
-----------------------------------------------------------------------------*/
#ifndef TIMER_BIN_REMOVED
typedef struct timer_bin_struct
{
  /* First timer in the bin */
  timer_ptr_internal_type         first;

  /* Last timer in the bin */
  timer_ptr_internal_type         last;

  /* Earliest expiry time in the bin */
  time_timetick_type                   lower;

  /* Latest expiry time in the bin */
  time_timetick_type                   upper;

  /* Number of timers in the bin */
  int                             count;

  /* Next bin pointer */
  timer_bin_ptr                   next_bin;

  /* Previous bin pointer */
  timer_bin_ptr                   prev_bin;
}
timer_bin_type;


/*-----------------------------------------------------------------------------
  Timer bin list type
-----------------------------------------------------------------------------*/

typedef struct timer_bin_list_struct
{
  /* First timer bin in list */
  timer_bin_ptr                   first;

  /* Last timer bin in list */
  timer_bin_ptr                   last;

  /* Number of timer bins */
  int                             count; 
}
timer_bin_list_type, *timer_bin_list_ptr;
#endif /* !TIMER_BIN_REMOVED */

/*-----------------------------------------------------------------------------
  Private timer data ...
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Faux timer group, used to hold list of active timers, as well as a list
     of timer groups.
     Active timers will never "belong" to this group, ie timer->group will
     never be &timers.active, since this would cause them to forget their
     true timer group. */
  timer_group_type                active;

  /*to hold p0 active timers*/
  timer_group_type                p0_active;

  /* Slow clock timestamp of last write to SLEEP_XTAL_TIMETICK_MATCH */
  time_timetick_type                   set_time;

  /* Slow clock timestamp of last attempt to write SLEEP_XTAL_TIMETICK_MATCH */
  time_timetick_type                   last_set_time;

  /* Next timer expiration value */
  time_timetick_type                   set_value;

  /* Last value written to SLEEP_XTAL_TIMETICK_MATCH */
  time_timetick_type                   match_value;

  /* Slow clock timestamp when timer_isr is called */
  time_timetick_type                   isr_time;

  /* Flag to indicate if timers_process_active_timers() is executing */
  boolean                         processing;

  /* Flag to indicate if timer_isr can be updated */
  boolean                         do_reprogram_isr;

  /* Active timer bin list */
  timer_bin_list_type             bins;

  /* Free timer bin list */
  timer_bin_list_type             bins_free;

  /* Pointer to the first non-deferrable timer */
  timer_ptr_internal_type         first_non_deferrable;

#ifndef FEATURE_QDSP6
  /* DAL Device Handle */
  DalDeviceHandle*                dal_int_handle;
#endif /*FEATURE_QDSP6*/

}
timers_type;

/* Notification functions for indicating non-defer timer expiry changes */
typedef struct
{
  timer_notify_type         notify_type;
  time_osal_notify_obj_ptr  notify_obj;
  time_osal_notify_data     notify_mask;
} timer_non_defer_notification_client_type;

/*Structure to store per Threadx Module Data*/
typedef struct timer_txmod_obj{
	q_type                     queue;      /*Queue to hold expired timer data*/
	qurt_signal_t              sig_obj;    /*signal object */
    uint32                     signs;	   /*Signal to be set for signal object*/
} timer_txmod_obj;

/*Structure to store expired timer data*/
typedef struct timer_expire_data{
	q_link_type                link;          /*Link to used to store data on queue*/
	qapi_TIMER_get_cbinfo_t    timer_cbinfo;  /*Structure to store expired timer callback info*/
} timer_expire_data;

/*-----------------------------------------------------------------------------
  Debug typedef's
-----------------------------------------------------------------------------*/
#ifdef FEATURE_TIMER_TRACE
/* Type of timer events to log */
typedef enum
{
  TIMER_TRACE_ISR,
  TIMER_TRACE_PROCESS_START,
  TIMER_TRACE_PROCESS_END,
  TIMER_TRACE_MATCH_VALUE_PROG,
} timer_trace_event_type;

/* A single timer event trace */
typedef struct
{
  timer_trace_event_type  event;
  time_timetick_type      ts;
  time_timetick_type      match_value_programmed;
} timer_trace_data_type;

/* Structure of all timer log events */
typedef struct
{
  uint32                index;
  timer_trace_data_type events[TIMER_TRACE_LENGTH];
} timer_trace_type;

#endif /* FEATURE_TIMER_TRACE */


#ifdef FEATURE_TIMER_API_LOG

typedef struct
{
  /* API type that was called */
  timer_api_type    		api;
  /* Pointer to calling function */
  //timer_cb_data_type      *timer_cf;
  /* Pointer to the thread */
  qurt_thread_t            	tid;
  /* Time Stamp */
  time_timetick_type		ts;
  /* Timer Pointer */
  timer_ptr_type			timer;
} timer_api_log_type;

#endif /* FEATURE_TIMER_API_LOG */


#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES

typedef struct timer_debug_var_1
{
  uint64 timer_now_actual;
  uint64 timer_match_value_actual_set;
  timer_ptr_type timer_address;
  uint64 timer_now_supplied;
  uint64 match_value_supplied;
  uint8  mv_setter;  
}timer_match_value_record_type;

typedef struct timer_debug_var_2
{
  timer_ptr_type timer_to_be_set;
  time_timetick_type ts;
  time_timetick_type expiry;
}timer_set_type;

typedef struct timer_clr_dbg_var
{
  timer_ptr_type ext_timer;
  timer_ptr_internal_type int_timer;
  time_timetick_type ts;
  timer_error_type ret_val;
  timer_state_struct_type timer_state;
  qurt_thread_t tid;
}timer_clr_log_type;


typedef struct timer_expired_debug_struct
{
  uint32 whileloopcnt;
  timer_ptr_type timer_exp_ptr;
  time_timetick_type ts;
  time_timetick_type expiry;
  time_timetick_type start;
  uint32 tcb_or_func;
  uint8 slave_assigned;
  uint8 callback_type;
}timer_expired_debug_type; // 40 bytes


typedef struct timer_debug_var_3
{
  timer_ptr_type timer_expired;
  time_timetick_type expiry_value; /* copy the expiry value of the timer */
  time_timetick_type processing_started;
  time_timetick_type processing_time;
  qurt_thread_t tid;
  uint8  call_back_type;
}timer_expired_type;

#endif /* FEATURE_ENABLE_TIMER_DEBUG_VARIABLES*/

#ifdef FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION
typedef struct timer_debug_var_4
{
  time_timetick_type processing_time;
  timer_ptr_internal_type timer_expired;  
  uint8  call_back_type;  
  time_timetick_type processing_started;
  uint32 thresh_exceed_count; /* number of times we have exceeded the threshold */
  uint32 fn_address; /* duplciate - not needed */
  uint64 data; /* duplciate - not needed */
}timer_isolate_type;
#endif /* FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION */

typedef struct
{
  time_timetick_type timer_undefer_match_interrupt_now;
  time_timetick_type timer_undefer_match_interrupt_first;
}timer_undefer_match_interrupt_log_type;

/* Log for the API timer_set_params */
typedef struct {
timer_ptr_type timer;
time_timetick_type ts;
timer_state_struct_type timer_state;
timer_notify_type notification;
} timer_set_params_log_type;

/*=============================================================================
                           INTERNAL FUNCTIONS PROTOTYPE
=============================================================================*/

/* Get the duration of the timer in passed unit */
time_timetick_type timer_get_duration
(
  timer_ptr_type                  timer,
  timer_unit_type                 unit
);


/* For setting the Timer kernel driver user module callback function pointer */
void timer_set_qapi_notification_handler(timer_qapi_notification_handler_type notification_func);

void timer_qapi_notification_handler(timer_ptr_type timer_ptr);


#endif /* TIMER_V_H */

