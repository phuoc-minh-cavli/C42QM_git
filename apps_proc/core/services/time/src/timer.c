/*=============================================================================

                  T I M E R   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using the slow clock.

  Slow Clock Frequency          Granularity     Timer Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs


REGIONAL FUNCTIONS 

  timer_init()
    Initializes the timer subsytem.  Call only from time_init( ).


EXTERNALIZED FUNCTIONS

  timer_def(timer, group, tcb, sigs, func, data)
    Defines and initializes a timer.
    Timers may also be staticly declared via the TIMER_DEF() macro

  timer_def2(timer, group, func, data)
    Defines and initializes a timer, with the clock callback semantics.
    Timers may also be staticly declared via the TIMER_DEF2() macro

  timer_set(timer, ticks, reload, unit)
    Sets an inactive timer to expire after a given period of time, or changes
    an active timer to expire after a given period of time.
    Optionally, specifies the timer to repeatly expire with a given period.

  timer_get(timer, unit)
    Get number of ticks before timer expires

  timer_pause(timer)
    Suspends an active timer

  timer_resume(timer)
    Resumes a previously "paused" active timer.

  timer_clr(timer, unit)
    Stops an active timer

  timer_clr_task_timers(tcb)
    Stops all active timers which belong to the specified task.

  timer_group_enable(timer_group)
    Enables a timer group.  Timers in the timer group which expired during the
    diabled period will have their expiry processing executed.

  timer_group_disable(timer_group)
    Disables a timer group.  Timers in the timer group will continue to count
    down, but they will not expire.


INITIALIZATION AND SEQUENCING REQUIREMENTS

    timer_init() must be called once before any other timer functions.

    timer_def() must be called, or the TIMER_DEF( ) macro used, for each
      timer, before other timer functions are used with that timer.

    A valid sclk estimate is needed before timers can be set.


Copyright (c) 2003 - 2017, 2019-2020 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/services/time/src/timer.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/09/15   ab      Memory reduction changes
04/03/14   psu     Update diag macros using optimized versions.
04/26/11   din     Fixed compiler warnings.
03/11/11   ab      Removed compiler warnings.
03/10/11   ab      Fix a corner case when a small expiry timer is inserted 
                   immediately after a large expiry timer.
11/12/10   din     Added function to get time to next occuring non-deferrable 
                   timer expiry.
08/25/10   din     Removed call to timetick_get_sclk_offset in timer_expires_in.
04/27/10   din     Fixed compiler warnings.
12/18/09   din     Adding Timer List debugging feature.
12/10/09   din     Timer expiry now caters to changing sclk_delta
09/30/09   sri     Enhanced QXDM strings on timer_group NULL
05/22/09   din     Added missing refernce of zero in timer_defer_match_interrupt 
05/22/09   din     Moved implementation of timer_set_next_interrupt to 
                   timetick_set_next_interrupt.
05/22/09   din     Removed FEATURE_TIMER_TASK. Removed Function timer_isr.
01/16/09   pbi     Moved MIN_REARM_SCLK definition to timetick.c
01/07/09   pbi     sclk_delta removed from timers_type
01/07/09   pbi     Removed timer_set_sclk_offset
03/25/09   din     In timer_expire(), cached callback functions from timer 
                   structure before first callback is made.
03/04/09   din     timer_defer_match_interrupt returns zero when expiry of first 
                   non deferrable timers is in past.
02/23/09   din     Consoldation of time.
01/21/09   din     Forced timer_undefer_match_interrupt() to update MATCH_VAL
                   register. Updated timer_process_active_timers() to prevent
                   overflow in timer expiry past the active timer list zero.
01/16/09   din     Added missing reset of last_set_time in timer_restart().
01/13/09   din     Removed timer_delay_expiry_excluding_groups() and
                   timer_undelay_expiry().
01/12/09   din     Added deferrable attribute to timer_group. Also added 
                   lightweight API's to enable/ disable timer groups. API's
                   timer_defer_match_interrupt(), timer_undefer_match_interrupt
                   & timer_group_set_deferrable().
01/05/09   din     Consolidation of time sources. Added 
                   FEATURE_TIME_UMTS_TARGETS. timer_set_next_interrupt was
           made uniform acroos different targets. Also Ensured that 
           timer_get() doesnot return 0.
12/23/08   tev     Remove timer_v.h inclusion.  No longer required now that
                   timer_v.h is obsolete.
12/15/08   tev     Explicitly include newly created private header file
                   timer_v.h
11/20/08   din     Always set set_time when match_value is updated in 
                   timer_set_next_interrupt(). Every attempt to set is recorded 
                   in last_set_time.
11/14/08   tev     Amended control for timer_restart to
                   defined(FEATURE_TIME_POWER_COLLAPSE) &&
                   !defined(FEATURE_TIME_REMOVE_TIMER_RESTART)
11/10/08   tev     Tentatively restored timer_restart under control of
                   FEATURE_TIME_POWER_COLLAPSE symbol definition.
10/17/08   din     Fixed initialization of do_reprogram_isr
10/16/08   din     Added lightweight mechanism to enable and disable timer groups 
                   during sleep. For use by the sleep subsystem only! APIs are
                   timer_delay_expiry_excluding_groups(), and timer_undelay_expiry()
                   (renamed timer_restart().) 
09/19/08   trc     timer_bin_find_first_exceeding() returns NULL upon failure
06/10/08   trc     Upon re-enabling a timer group, only nudge the active timer
                   list zero point as far back as the earliest expired timer
11/13/07   jhs     Removed timer_always_on_group.
08/31/07   jhs     Support for AMSS RESTART
05/29/07   jhs     Added in support to rewrite MATCH when writing match over a
                   tick boundary if FEATURE_TIME_REQUIRES_ODD_WRITE is defined.
04/19/07   gfr     Support for timer tracing.
01/12/07   trc     Increase MIN_REARM_SCLK from 6 to 9 slow clock cycles
12/04/06   trc     Bump up the # of timer bins from 64 to 128
10/11/06   trc     Reset match_value and set_value when sclk_delta is set
                   coming out of power collapse since they are no longer valid.
08/19/06   gb      Changed FEATURE_POWER_COLLAPSE to FEATURE_TIME_POWER_COLLAPSE. Made
                   changes for modem power collapse code.
08/15/06   jhs     Clear out match_value and set_value when sclk_delta is set
                   because they are no longer valid.
04/10/06   ajn     Prevent rounding nudge from creating a negative runtime.
04/07/06   gfr     Remove unneeded header file (msm_drv.h)
03/01/06   kap     Only unbin when timer->bin field is non-null
02/22/06   ajn     Add offset btw modem & MPM sclk counter for power collapse
02/10/06   kap/ajn Added Timer Optimization Changes (double linking/binning)
12/20/05   ejv     Added T_QSC60X0.
11/03/05   ajn     Zero-initialize the timer.cache fields in timer_define()
10/27/05   ajn     Always set set_time in timer_set_next_interrupt().
08/12/05   jhs     Moved apps processor to its own caching assembly function
                   for writing to MATCH_VAL.
08/05/05   ajn     MATCH_VAL written from caching assembly function.
07/29/05   ajn     Repeat MATCH_VAL writes until the write latches.
07/22/05   ajn     Don't write new MATCH_VAL until last write has latched.
07/06/05   ajn     Added timer_set_modem_app_sclk_offset (7500 APPS)
06/24/05   ajn     Include partial 7500 support
06/09/05   ajn     Corrected non-timer_task initialization.
05/19/05   ajn     Timer op's don't signal task or call process_active_timers.
05/09/05   ajn     Don't write to TIMETICK_MATCH_VAL if value is unchanged.
                   Fixed O(N^2) list insertion in timer_group_enable().
05/06/05   ajn     Cache time/reload values to avoid recalculating if unchanged
03/29/05   ajn     Replaced timer_ptr with timer_ptr_type, for L4 project.
03/16/05   ajn     Update next timer expiry interrupt w/o processing timers
                   when timers are removed from the active list.
03/16/05   ajn     Add ms rounding to following expiry in repeating timers.
12/06/04   lcc     In timer_process_active_timers, added reading of timetick
                   count to make sure it's non-zero before existing.  This is
                   needed for QUASar's timetick hw problem.
10/20/04   ajn     Moved timer expiry out of ISR context into a Timer task.
09/28/04   ajn     Catch use of timers w/o call to timer_define().
09/27/04   ajn     Force timer in the NULL timer group into an actual group.
08/30/04   ajn     Added "get next interrupt" and "interrupt early" functions
                   for TCXO shutdown.
07/06/04   ajn     timer_define() ERR's (not ERR_FATAL's) on active timer.
06/23/04   ajn     Corrected re-entrancy when processing active timers.
01/29/04   ajn     Corrected possible NULL pointer dereference.
01/09/04   ajn     Optimized the expiry-time advance of auto-reloading timers.
11/14/03   ajn     Added sclk timestamps for profiling timer_isr latency.
11/11/03   ajn     Added checks to detect timer_def( ) calls on active timers
10/03/03   ajn     Call added to process_active_timers() in group_disable, to
                   ensure next active timer's interrupt occurs at proper time.
08/11/03   ajn     Added timer_pause/timer_resume for rex_set_timer replacement
08/08/03   ajn     Updated for coding standards
08/06/03   ajn     Added timer_def2(), for clk_reg() replacement
07/23/03   ajn     Moved _{to,from}_sclk functions to timetick.c
07/17/03   ajn     File created.

=============================================================================*/



/*=============================================================================


  *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE ***

  
  All comparisons in this module are done with respect to a moving origin,
  or "zero" point, to handle wrap-around of the 32-bit slow clock counter.
  
  For example, the conditional statement "if ( now >= expiry )" is wrong,
  since "now" could be 0xFFFFFF00, and "expiry" could be "0x00000100", ...
  "0x200" counts in the "future".
  
  The correct comparison would be "if ( now - zero >= expiry - zero )".
  This compares the current time (measured from some point in the past) to
  the expiry time (measured with respect to the same point in the past).
  If the "zero" origin was "0xFFFFFE00", with the above values, "now - zero"
  would be "0x100" and "expiry - zero" would be "0x300", making the entire
  expression FALSE; the expiry point has not been reached.

  If the expression "now - zero >= expiry - zero" returns TRUE, then it is
  safe to subtract "now - expiry" to determine how much time has passed since
  the expiry point.  If "now - zero >= expiry - zero" returns FALSE, the
  reverse subtraction is valid: "expiry - now" indicates how much time
  remains until the expiry point.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "ats_v.h"
#include "timer.h"
#include "err.h"
#include "assert.h"
#include "stdint.h"
#ifndef FEATURE_QDSP6
#include "DDIInterruptController.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#else /*FEATURE_QDSP6*/   
#include "qurt.h"
#endif /*FEATURE_QDSP6*/

#if defined(FEATURE_TIMER_APQ8064)
#define TRAMP_SLP_TICK_ISR TRAMP_GP_TIMER_IRQ
#endif

#include <stringl/stringl.h>
#include "timer_v.h"
#include "timer_fn_map.h"
#include "rcevt.h"
#include "rcinit_qurt.h"
#include "queue.h"

#include "timer_slaves.h"
#include "DDITimetick.h"
#include "osal.h"

/* timer debug variables can be mainlined based on profiling studies
 done. The latency introduced due to these features are less than a 
 tick on an average */
// Note: timer debug features needs to be enabled in scons


/*=============================================================================

                             CONFIGURATION

=============================================================================*/


/*-----------------------------------------------------------------------------
 Select edge-triggered timer interrupt, if supported
-----------------------------------------------------------------------------*/

#if defined( T_MSM7500 ) || defined( T_QSC60X0 ) || defined (FEATURE_TIMER_APQ8064)


  #define TIMETICK_INT_EDGE_TRIGGERED

  /* All interrupts are already edge-triggered */
  #define TIMETICK_INT_EDGE_TRIGGERED_INIT() /* No-op */


#elif defined( T_MSM6700 ) || defined( T_MSM6275 )


  #define TIMETICK_INT_EDGE_TRIGGERED

  #define TIMETICK_INT_EDGE_TRIGGERED_INIT() \
    HWIO_INT_DET_REG_1_OUTM( \
      HWIO_INT_DET_REG_1_SLEEP_TIME_TICK_INT_BMSK, \
      HWIO_INT_DET_REG_1_SLEEP_TIME_TICK_INT_BMSK )


#elif defined( T_MSM6550 )


  #define TIMETICK_INT_EDGE_TRIGGERED

  /*lint -emacro(570,TIMETICK_INT_EDGE_TRIGGERED_INIT) */
  #define TIMETICK_INT_EDGE_TRIGGERED_INIT() \
    HWIO_INT_DETECT_CTL_0_OUTM( \
      HWIO_INT_DETECT_CTL_0_SLP_XTAL_TIMETICK_INT_BMSK, \
      HWIO_INT_DETECT_CTL_0_SLP_XTAL_TIMETICK_INT_BMSK )

#endif /* T_MSMxxxx */


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

#ifdef FEATURE_OS_MEMORY_PROTECTION
extern byte * A11S_CSR_BASE;
#endif
extern uint32 timer_root_handle;
extern osal_mutex_arg_t qapi_timer_mutex;
/*-----------------------------------------------------------------------------
  Variables required for Handling timers of user module.
-----------------------------------------------------------------------------*/
static timer_qapi_notification_handler_type timer_qapi_module_notification_handler = NULL;

/*-----------------------------------------------------------------------------
  Pool of timer bins
-----------------------------------------------------------------------------*/

timer_bin_type              timer_bin_pool[ TIMER_BIN_NUM ];



static timers_type                timers =
{
  /* Faux timer group contain active timers & list of timer groups */
  {
    /* Active timers */
    {
      /* Slow clock "zero" base count for active timers list */
      0,

      /* Linked list of active timers */
      NULL
    },

    /* Disabled flag for the Faux Active Timer group.
       This group is always enabled.  When timers are disabled, they are moved
       off of this list */
    FALSE,

    /* Deferrable Flag */
    FALSE,

    /* Linked list of timer groups which have been disabled at some time. */
    &timers.active
  },

  /* Faux timer group contain p0 active timers & list of timer groups */
  {
    /* Active timers */
    {
      /* Slow clock "zero" base count for active timers list */
      0,

      /* Linked list of p0 active timers */
      NULL
    },

    /* Disabled flag for the Faux Active Timer group.
       This group is always enabled.  When timers are disabled, they are moved
       off of this list */
    FALSE,

    /* Deferrable Flag */
    FALSE,

    /* Linked list of timer groups which have been disabled at some time. */
    &timers.p0_active
  },

  /* Timestamp when call to _set_next_interrupt updates match_value */
  0,

  /* Timestamp of last attempt to update match value */
  0,

  /* Value passed to _set_next_interrupt */
  0,

  /* Value written to match count */
  0,

  /* Timestamp when timer isr occurred at */
  0,

  /* timers_process_active_timers is not executing */
  FALSE,

  /* Allow timer_isr to be set */
  TRUE,

  /* Active timer bin list */
  {
    /* First bin in active bin list */
    NULL,

    /* Last bin in active bin list */
    NULL,

    /* Number of bins in list */
    0
  },

  /* Free timer bin list */
  {
    /* First bin in free bin list */
    NULL,

    /* Last bin in free bin list */
    NULL,

    /* Number of bins in list */
    0
  },

  /* First Non-Deferrable Timer Pointer */
  {
    /* Timer Internal Pointer */
    NULL
  },
  #ifndef FEATURE_QDSP6
  /* Dal Device Handle */
  {
    /* Dal Interrupt Handle */
    NULL
  }
  #endif /*FEATURE_QDSP6*/

};

/* Buffer of timer_internal_type */
static timer_internal_type timer_buffer[MAX_TIMER_BUFFER_NUMBER];

/* Queue to store free timer internal strcuture */
static q_type                     timer_free_q;

/* Indicated if timer_free_q in initialized */ 
static uint8 timer_q_init_status = 0;
static uint32 timer_while_cnt = 0;

static DalDeviceHandle *hTimerHandle = NULL;

static volatile qurt_thread_t timer_thread_id;
static qurt_signal_t timer_qurt_signal;

static timer_non_defer_notification_client_type timer_non_defer_expiry_notify_clients[TIMER_NON_DEFER_EXPIRY_NOTIFY_CLIENTS_MAX_CNT];
static uint32 timer_non_defer_expiry_notify_clients_cnt = 0;


/*-----------------------------------------------------------------------------
  The NULL timer group, for timers not in any other group
-----------------------------------------------------------------------------*/

/* Timers declared to be in the NULL group are reparented to this group */
timer_group_type                  timer_null_group;

/* Global Non-deferrable group for various clients use */
/* Note: This group shouldn't be called with disable/enable group functions as
   this will be used by other clients too. */
timer_group_type  				  timer_non_defer_group;

timer_group_type                  timer_prio0_group;
/*-----------------------------------------------------------------------------
  Timer trace data
-----------------------------------------------------------------------------*/

/* Stores number of timers present currently in  the active list */
static uint32 timer_active_timers_count = 0;

/* Stores number of timers present in timer_buffer*/
static uint32 timer_count_buffer_allocated = 0;

/*Stores number of paused timers*/
static uint32 timer_count_paused_timers = 0;

/* Stores the static function pointers for timer dependencies */
static timer_fn_map_fp_type timer_fn_map;

#ifdef FEATURE_TIMER_TRACE


/* Declare the log structure */
static timer_trace_type timer_trace;

#endif /* FEATURE_TIMER_TRACE */

#ifdef FEATURE_TIMER_API_LOG
static timer_api_log_type timer_api_calls[MAX_TIMER_API_CALLS];
static int timer_api_log_cnt = 0;
static void timer_api_log
(
  timer_api_type          event, 
  timer_ptr_type          timer,  
  time_timetick_type           timetick
);

#endif /* FEATURE_TIMER_API_LOG */

/* Macros to perform logging */
#ifdef FEATURE_TIMER_TRACE
  #define TIMER_TRACE(event)        timer_trace_event(TIMER_TRACE_##event)
#else
  #define TIMER_TRACE(event)
#endif

#ifdef FEATURE_TIMER_API_LOG
  #define TIMER_API_LOG(api,timer,timetick) timer_api_log(api,timer,timetick)
#else
  #define TIMER_API_LOG(api,timer,timetick)  
#endif /* FEATURE_TIMER_API_LOG */


static void timer_active_list_check(void);

/**
Post initialization routine for timer
*/
void timer_post_init(void);

/* Pointer to the mutex object for protecting critical-section of timer 
   code*/
qurt_mutex_t timer_qemu_mutex;
qurt_mutex_t timer_trace_mutex;
qurt_mutex_t timer_pc_mask_mutex;

/*function protect the critical section of timer code*/
void timer_mutex_init(void);
void timer_trace_mutex_init(void);
void timer_pc_mutex_init(void);


/*For ARM9*/
#ifndef FEATURE_QDSP6

#else /*else for Q6*/

/*registration id obtained for timer signal registered with QURT. This
 id will be used by timetick to program the match values for timers*/
static int timer_qurt_sig_registration_id = -1;

/*Timer signal registered with QURT. QURT will set this signal to
 TIMER_QURT_SIG_MASK when the match value == timetick value*/
qurt_signal_t        timer_q6_signal;


#endif /*FEATURE_QDSP6*/

#define timer_dalsys_sig_set(h)

#define timer_dalsys_sig_wait(h)

#define timer_dalsys_sig_reset(h)

/*These are timer debug variables that were useful to catch many 
 bugs. As of now they are featurized, so save space*/
#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES

static timer_match_value_record_type record_match_val[MAX_MATCH_REC];
static int record_match_val_counter = 0;

static timer_set_type timer_sets[MAX_TIMER_SET];
static int timer_set_cnt = 0;
static int timer_set_cnt_global = 0;

#ifdef FEATURE_ENABLE_TIMER_CLR_DEBUG_VARIABLES

static timer_clr_log_type timer_clr_log[MAX_TIMER_CLR_LOG];
static uint32 timer_clr_log_cnt = 0;

#endif /* FEATURE_ENABLE_TIMER_CLR_DEBUG_VARIABLES */

static timer_expired_debug_type timer_expired_debug[MAX_TIMER_EXPIRED_DEBUG];
static uint32 timer_expired_debug_cnt = 0;

static timer_expired_type timers_expired[MAX_TIMER_EXPIRED];
static int timer_expired_cnt = 0;
static int timer_expired_cnt_global = 0;

static timer_expired_type timers_remote_expired[MAX_TIMER_REMOTE_EXPIRED];
static int timer_remote_expired_cnt = 0;
static int timer_remote_expired_cnt_global = 0;

#endif /* FEATURE_ENABLE_TIMER_DEBUG_VARIABLES*/

#ifdef FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION

static uint32 timers_cnt_assert_threshold = 10;
static timer_isolate_type timers_isolated[MAX_TIMER_ISOLATED];
static int timer_isolated_cnt = 0;
static int timer_isolated_cnt_global = 0;

uint32 timer_assert_compare_value_count;
uint32 timer_assert_compare_value_list[MAX_TIMER_ASSERT_COMPARE_LIST];

#endif /* FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION */

/*These values are set high to avoid the annoying F3 messages
 and an error fatal in case cb processing time is high. 
 1 sec = 32768 ticks. 
 Therefore, 32 ticks = 1 msec 
 */
volatile static uint64 timer_processing_time_warning_threshold = 19200;

#ifndef TIMER_ERROR_FATAL_THRESHOLD_ZEROED_OUT
volatile static uint64 timer_processing_time_err_fatal_threshold=38400000;
#else
volatile static uint64 timer_processing_time_err_fatal_threshold=0;
#endif

/*this function will be called in Q6 for updating the timer
 variables*/
static void timer_update_isr_time
(
  void
);

#ifdef FEATURE_QDSP6
static void timer_init_qurt_signal(void);

#endif /* FEATURE_QDSP6 */

static void timer_get_timetick_handle(void);

static timer_undefer_match_interrupt_log_type timer_undefer_match_interrupt_log; 

#ifdef FEATURE_ENABLE_TIMER_SET_PARAM_LOG

/* Log for the API timer_set_params */
static uint32 timer_set_params_log_cnt = 0;
static timer_set_params_log_type  timer_set_params_log[MAX_TIMER_SET_PARAMS_LOG];

#endif /* FEATURE_ENABLE_TIMER_SET_PARAM_LOG */

static boolean timer_slave_processing_enabled = FALSE; 

static int timer_after_pc_mask[4]={0,0,0,0};
static int timer_cnt_cpu_max=0;
static int timer_cnt_cpu_down=0;

typedef struct 
{
  int reference_count;
  timer_match_interrupt_setter_type function_called;
  time_timetick_type ts;

}timer_defer_undefer_reference_count_log_type;

static int timer_defer_undefer_reference_count =0;

#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
static timer_defer_undefer_reference_count_log_type timer_defer_undefer_reference_count_log[MAX_TIMER_REFERNCE_COUNT_LOG];
static int timer_defer_undefer_reference_count_log_cnt = 0;
#endif /*FEATURE_ENABLE_TIMER_DEBUG_VARIABLES*/

/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/

void timer_reset_check_pc_mask_if_set(unsigned int index, unsigned int decrease_count)
{

  TIMER_PC_MASK_MUTEX_LOCK();
 
  if( timer_after_pc_mask[index] > 0)
  {
    timer_after_pc_mask[index]= timer_after_pc_mask[index]-decrease_count;
    if( (timer_after_pc_mask[0] <= 0) &&
        (timer_after_pc_mask[1] <= 0) &&
        (timer_after_pc_mask[2] <= 0) &&
        (timer_after_pc_mask[3] <= 0) )
    {
      // Release the cpu, expected to be executed once per powercollapse.
      if( timer_fn_map.timer_npa_clk_client )
      { /* client/resource exists so use can make MAX requests to the clock */
        if(timer_fn_map.timer_npa_complete_request)
        {  
          timer_fn_map.timer_npa_complete_request(timer_fn_map.timer_npa_clk_client);
        }
        timer_cnt_cpu_down++;
      }
    }
  }
  TIMER_PC_MASK_MUTEX_UNLOCK();
}

static void timer_reset_check_pc_mask(unsigned int index)
{

  TIMER_PC_MASK_MUTEX_LOCK();
  timer_after_pc_mask[index]--;
  if( (timer_after_pc_mask[0] <= 0) &&
      (timer_after_pc_mask[1] <= 0) &&
      (timer_after_pc_mask[2] <= 0) &&
      (timer_after_pc_mask[3] <= 0) )
  {
    // Release the cpu, expected to be executed once per powercollapse.
    if( timer_fn_map.timer_npa_clk_client )
    { /* client/resource exists so use can make MAX requests to the clock */
      if( timer_fn_map.timer_npa_complete_request )
      {
        timer_fn_map.timer_npa_complete_request(timer_fn_map.timer_npa_clk_client);
      }
      timer_cnt_cpu_down++;
    }
  }
  
  TIMER_PC_MASK_MUTEX_UNLOCK();
}

void timer_clk_cpu_callback( void *context,
                             unsigned int event_type, 
                             void *data,       
                             unsigned int data_size)
{
  if (NULL != timer_fn_map.timer_npa_clk_client)
  { /* client already created */
    return;
  }

  if( timer_fn_map.timer_npa_create_sync_client )
  {
    timer_fn_map.timer_npa_clk_client = 
        timer_fn_map.timer_npa_create_sync_client( "/clk/cpu", 
                                                   "timer_clk_client",
                                                   timer_fn_map.timer_npa_client_required,
                                                   0,
                                                   NULL );
  }

  if (NULL == timer_fn_map.timer_npa_clk_client)
  { /* client creation failed */
    ATS_ERR("NPA failed creation of timer_clk_client for Timer");
  }
}

void timer_register_npa_client(void)
{
  if( timer_fn_map.timer_npa_resource_available_cb )
  {
    timer_fn_map.timer_npa_resource_available_cb( "/clk/cpu",
                                                  timer_clk_cpu_callback,
                                                  NULL );
  }
}
#ifdef FEATURE_TIMER_TRACE
/*=============================================================================

FUNCTION TIMER_TRACE_EVENT

DESCRIPTION
  This function saved a timer trace event packet.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

static void timer_trace_event
(
  timer_trace_event_type event
)
{
  /* Pointer to the trace structure to fill in */
  timer_trace_data_type *trace;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  TIMER_TRACE_MUTEX_LOCK();
  /* Get the next trace structure */
  trace = &timer_trace.events[timer_trace.index];

  /* Fill in the common fields */
  trace->event = event,
  DalTimetick_GetTimetick64(hTimerHandle, &trace->ts);

  /*record the match value in the structure*/
  if(event == TIMER_TRACE_MATCH_VALUE_PROG)
  {
    trace->match_value_programmed = timers.match_value; 
  }
  else
  {
    trace->match_value_programmed = 0;
  }

  /* Go to the next index */
  timer_trace.index++;
  if (timer_trace.index >= TIMER_TRACE_LENGTH)
  {
    timer_trace.index = 0;
  }
  TIMER_TRACE_MUTEX_UNLOCK();
} /* timer_trace_event */

#endif /* FEATURE_TIMER_TRACE */

#ifdef FEATURE_TIMER_API_LOG
/*=============================================================================

FUNCTION TIMER_API_LOG

DESCRIPTION
  This function logs all of the api calls along with
  the thread ID.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

static void timer_api_log
(
  timer_api_type event,
  timer_ptr_type timer,
  time_timetick_type timetick  
)
{
  /* Record Event Type */
  timer_api_calls[timer_api_log_cnt].api = event;
  /* Record ptr to TCB */
  timer_api_calls[timer_api_log_cnt].tid = qurt_thread_get_id();

  /* Record timetick value */
  timer_api_calls[timer_api_log_cnt].ts = timetick;
  
  /* Record ptr to timer */
  timer_api_calls[timer_api_log_cnt].timer = timer;
  
  //timer_api_calls[timer_api_log_cnt].timer_cf =  /* Figure out how to get R14 */

  // To remove warning
  timer_api_calls[timer_api_log_cnt].api = timer_api_calls[timer_api_log_cnt].api;
  
  timer_api_log_cnt = (++timer_api_log_cnt) % MAX_TIMER_API_CALLS;
  
} /* timer_api_log */

#endif /* FEATURE_TIMER_API_LOG */

/*=============================================================================

FUNCTION TIMER_GET_INTERNAL_TIMER

DESCRIPTION
  This function returns pointer to internal timer.

DEPENDENCIES
  None.

RETURN VALUE
  Returns relevant pointer to internal timer structure. 
  If handle is invalid returns NULL.

SIDE EFFECTS
  This function should be called inside an intlock or a mutex.

=============================================================================*/

static timer_ptr_internal_type timer_get_internal_timer
(
  timer_ptr_type   timer
)
{
  int32 temp_index;
  timer_ptr_internal_type temp_int = NULL;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  temp_index = HANDLE_TO_INDEX( timer->timer_handle );

  if ( temp_index == TIMER_BUFFER_INVALID_HANDLE )
  {
    temp_int= NULL;    
  }

  if ( temp_index > MIN_TIMER_BUFFER_NUMBER
       && temp_index < MAX_TIMER_BUFFER_NUMBER )
  {
    temp_int = &(timer_buffer[temp_index]);    
  }
  return temp_int;

} /* timer_get_internal_timer */



/*=============================================================================

FUNCTION TIMER_CONV_TO_SCLK_FREQ

DESCRIPTION
  This function returns 32kHz timetick value based on 19MHz timetick value.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 32kHz timetick value. 

SIDE EFFECTS
  None.

=============================================================================*/

static inline timetick_type timer_conv_to_sclk_freq(uint64 time_in)
{
  return (timetick_type)(time_in/XO_TO_SCLK_CONV_FACTOR);
} /* timer_conv_to_sclk_freq */


/*=============================================================================

FUNCTION TIMER_CONV_TO_XO_FREQ

DESCRIPTION
  This function returns 19MHz timetick value based on 32kHz timetick value.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 19MHz timetick value. 

SIDE EFFECTS
  None.

=============================================================================*/

static inline uint64 timer_conv_to_xo_freq(timetick_type time_in)
{
  return ((uint64)time_in)*((uint64)XO_TO_SCLK_CONV_FACTOR);
} /* timer_conv_to_xo_freq */

/*=============================================================================

FUNCTION TIMER_NOTIFY_FIRST_NON_DEFER_TIMER_EXPIRY_CHANGE

DESCRIPTION
  Sets the signal to clients' sig objs to notify the changed first non-deferrable timer 
  expiry

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/

static void timer_notify_first_non_defer_timer_expiry_change(void)
{
    int i;
    timer_notify_type         notify_type;
    time_osal_notify_obj_ptr  notify_obj;
    time_osal_notify_data     notify_mask;

    for(i=0; i<timer_non_defer_expiry_notify_clients_cnt; i++)
    {
    
      notify_type = timer_non_defer_expiry_notify_clients[i].notify_type;
      notify_obj = timer_non_defer_expiry_notify_clients[i].notify_obj;
      notify_mask = timer_non_defer_expiry_notify_clients[i].notify_mask;
      
      if(notify_obj == NULL)
	     {
         ERR_FATAL("Notify_obj NULL means corruption happened; index = %d", i, 0, 0);
		 return;
		 }
      
      /* If there are signals to be set, and a task / signal object to set the signals to,
     then set those signals in that task's task control block or the passed signal object */

      if ( notify_type & TIMER_NATIVE_OS_SIGNAL_TYPE )
       {
         qurt_signal_set((qurt_signal_t *)notify_obj, notify_mask);
       }
      else if ( notify_type & TIMER_REX_TCB_SIGNAL_TYPE )
      {
        #ifdef FEATURE_TIMER_REX_IS_AVAILABLE
          (void) rex_set_sigs( notify_obj, notify_mask );
        #else
          ERR_FATAL("Signal type cannot be rex type ; index = %d",i,0,0);
        #endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
      }
      else if ( notify_type & TIMER_NATIVE_OS_SIGNAL2_TYPE )
      {
      #ifdef FEATURE_TIME_QDSP6
            qurt_signal2_set((qurt_signal2_t *)notify_obj, notify_mask);
      #else
             ERR_FATAL("Signal type cannot be NATIVE OS type ; index = %d",i,0,0);
      #endif /* FEATURE_TIME_QDSP6 */
      }
      else if ( notify_type & TIMER_DALSYS_EVENT_SIGNAL_TYPE )
      {
          timer_dalsys_sig_set(notify_obj);
      }
      else if (notify_type & ~TIMER_NOTIFICATION_CHECK_MASK)
      {
         ERR_FATAL("Invalid Notify type 0x%x, may be corruption",notify_type,0,0);
      }
    } /* for() */

    return;
} /* timer_notify_first_non_defer_timer_expiry_change */

/*=============================================================================

FUNCTION TIMER_UPDATE_FIRST_ACTIVE_NON_DEFERRABLE_TIMER

DESCRIPTION
  This function returns the first non-deferrable timer on the active
  timer list.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the pointer to the first non-deferrable internal timer.
  If there are no non-deferrable timers on the active list, the return NULL. 

SIDE EFFECTS
  None.

=============================================================================*/

static void timer_update_first_active_non_deferrable_timer(void)
{
  timer_ptr_internal_type ptimer;
  timer_ptr_internal_type ptimer_p0 = timers.p0_active.list.first;
  time_timetick_type p1_expiry = TIMER_MAX_EXPIRY;
  time_timetick_type p0_expiry = TIMER_MAX_EXPIRY;
  time_timetick_type old_expiry = TIMER_MAX_EXPIRY;
  time_timetick_type new_expiry = TIMER_MAX_EXPIRY;

  if(timers.first_non_deferrable)
     old_expiry = timers.first_non_deferrable->expiry;
  
  if (ptimer_p0)
  {
    p0_expiry = ptimer_p0->expiry;
  }

  for( ptimer = timers.active.list.first; ptimer != NULL; ptimer = ptimer->next )
  {
    /* If timer belongs to a group, which is non-deferrable */
    if (ptimer->group->deferrable == FALSE )
    {
      /* Found the first non-deferrable timer on the active list */
      p1_expiry = ptimer->expiry;
      break;
    }
  }

  /* If there was a non-deferrable timer on the active list, then check the expiry value */
  if (p0_expiry == TIMER_MAX_EXPIRY &&
      p1_expiry == TIMER_MAX_EXPIRY)
  {
    /* There are no active non-deferrable timers */
    timers.first_non_deferrable = NULL;
  }
  else if (p1_expiry < p0_expiry)
  {
    /* First non-deferrable timer is on the active list */
    timers.first_non_deferrable = ptimer;
    new_expiry = p1_expiry;
  }
  else
  {
    /* First non-deferrable timer is on the p0 active list */
    timers.first_non_deferrable = ptimer_p0;
    new_expiry = p0_expiry;
  }
  
  if(old_expiry != new_expiry)
     timer_notify_first_non_defer_timer_expiry_change();
  
} /* timer_update_first_active_non_deferrable_timer */

/*=============================================================================

FUNCTION TIMER_TASK_ISR

DESCRIPTION
  This function detects a timer interrupt, and forwards it to the timer task
  for processing.

DEPENDENCIES
  Must only be called from tramp.

RETURN VALUE
  None

SIDE EFFECTS
  Uninstalls the timer ISR.
  Signals the timer task to process the expiring timer, which can have
  numerous side effects, depending on the expiring timer.

=============================================================================*/
static const void * timer_task_isr( void* input )
{
  /* Disabling any logging since it is in 
   * ISR context and needs to execute fast */

  #ifdef TIMER_NOT_IN_ISR_CONTEXT
  /* Timer sclk time-stamp values */
  time_timetick_type                   set_time, set_value;

  /* Latency in ISR handling */
  time_timetick_type                   latency_sclk;
  #endif

  time_timetick_type                   now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  //ATS_INTLOCK( );

  /* Not locking the mutex since 
   * this will be always a linear operation */

  /* Record when timer isr actually was handled */
  DalTimetick_GetTimetick64(hTimerHandle, &now);
  timers.isr_time = now;

  #ifdef TIMER_NOT_IN_ISR_CONTEXT
  /* Capture timer set values, for messaging outside of ATS_INTLOCK. */
  set_time  = timers.set_time;
  set_value = timers.set_value;
  #endif

  #ifndef FEATURE_QDSP6
  /* Disable interrupt since QTMR irq remains high */
  DalTimetick_Enable(hTimerHandle,0);
  #else
  /* Set timer to far in the future so that interrupt doesn't fire again */
  DalTimetick_SetNextInterrupt64(hTimerHandle, TIMER_MAX_EXPIRY, now, &timers.match_value);
  #endif

  //ATS_INTFREE();

  #ifdef TIMER_NOT_IN_ISR_CONTEXT
  /* This is checked again in timer_update_isr_time() */
  if ( now - set_time >= set_value - set_time)
  {
    /* Determine the difference between when the interrupt was programmed to
       occur at, and when the interrupt was actually handled. */
    latency_sclk = now - set_value;

    if ( latency_sclk > TIMER_SCLK_LATENCY )
    {
      /* Timer interrupt was handled over btw 5 (38MHz) to 10ms (19MHz) late.
         (Timers may expire much later than 5-10ms, if they are in a disabled
         group, or if multiple timers expire due to the same interrupt.
         This is only a measure of the timer isr latency.) */

      ATS_ERR_3("Late Timer ISR: ST=%d SV=%d IT-SV=%d",
                set_time, set_value, latency_sclk);
    }
  }
  else
  {
    ATS_MSG_3("Early Timer ISR: ST=%d SV=%d SV-IT=%d",
             set_time, set_value, set_value - now);
  }
  #endif

  /* Signal the timer task of the timer interrupt event */
  qurt_signal_set(&timer_qurt_signal, TIMER_EXPIRY_SIG);

  return NULL;
} /* timer_task_isr */


/*=============================================================================

FUNCTION TIMER_SET_NEXT_INTERRUPT

DESCRIPTION
  Program the SLEEP_XTAL_TIMETICK to generate an interrupt at the given value

DEPENDENCIES
  Internal timer_ function.
  Must be called from ATS_INTLOCKED context.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt when slow clock counter reaches given value

=============================================================================*/

static void timer_set_next_interrupt
(
  /* Slow clock count at which next interrupt will occur */
  time_timetick_type                   match_count,

  /* Current slow clock count */
  time_timetick_type                   ticks_now,

  /* If TRUE, forces MATCH_VAL register to be updated */
  boolean                         force,

  uint8                           caller,

  timer_ptr_type                  head_timer
)
{                               

  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Don't re-write the same value to the MATCH_VAL register. 
    Only if the timer_undefer_reference_count is zero, it means
    that NPA is done with its job and we can start programming the interrupt.*/
  if ((force == TRUE || ( match_count != timers.set_value  &&  
         match_count != timers.match_value ))
       && timer_defer_undefer_reference_count == 0)       
  {
    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    record_match_val[record_match_val_counter].match_value_supplied = match_count;
    record_match_val[record_match_val_counter].timer_address = head_timer;
    record_match_val[record_match_val_counter].timer_match_value_actual_set= MAGIC_INITIALIZER_64;
    record_match_val[record_match_val_counter].timer_now_supplied = ticks_now;
    record_match_val[record_match_val_counter].timer_now_actual = MAGIC_INITIALIZER_64;
    record_match_val[record_match_val_counter].mv_setter = caller;
    #endif

    DalTimetick_SetNextInterrupt64(hTimerHandle,
                                   (DalTimetickTime64Type)match_count,
                                   (DalTimetickTime64Type)ticks_now,
                                   &timers.match_value);

    /* Re-enable interrupts */
    DalTimetick_Enable(hTimerHandle,1);

    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    record_match_val[record_match_val_counter].timer_match_value_actual_set = timers.match_value;
    DalTimetick_GetTimetick64(hTimerHandle, &record_match_val[record_match_val_counter].timer_now_actual);
    record_match_val_counter = (record_match_val_counter+1)%MAX_MATCH_REC;
    /* This is a dummy statement to remove compiler warning */
    record_match_val[record_match_val_counter].timer_now_supplied = \
                  record_match_val[record_match_val_counter].timer_now_supplied;
    #endif

    /* Save a trace packet with the match value */
    TIMER_TRACE(MATCH_VALUE_PROG);     
    
    /* Record when this value was actually written */
    timers.set_time    = ticks_now ;

    /* Record when next timer should expire at */
    timers.set_value = match_count;
  }
  else if(timer_defer_undefer_reference_count > 0) 
  {
    ATS_ERR_2("Programming of timer int ignored for match_val = %d, ref_count = %d",
             match_count, timer_defer_undefer_reference_count);
  }
  
  /* Record when this match value was logically written, even if we don't
     bother to write the value if it is unchanged. */
  timers.last_set_time = ticks_now ;

} /* timer_set_next_interrupt */


/*=============================================================================

FUNCTION TIMER_DETERMINE_NEXT_TIMER_EXPIRY

DESCRIPTION
  To determine which earliest timer from the two timer list.
  This will require us to iterate through the two list to check
  the earlier of the two timers and return that timer.

DEPENDENCIES
  This function should be called from critical section.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
time_timetick_type timer_determine_next_timer_expiry
(
  timer_ptr_type *head_timer
)
{
  /*initialize the next_timer_expiry to max value*/
  time_timetick_type first = TIMER_MAX_EXPIRY;
    
  if(timers.p0_active.list.first != NULL) 
  {
    first = timers.p0_active.list.first->expiry;
    *head_timer = timers.p0_active.list.first->timer_ext;
  }

  if ( timers.active.list.first != NULL )
  {
    if(timers.active.list.first->expiry < first) 
    {
      first = timers.active.list.first->expiry;
      *head_timer = timers.active.list.first->timer_ext;
    }   
  }
  return first;
}


/*=============================================================================

FUNCTION TIMER_UPDATE_TIMER_INTERRUPT

DESCRIPTION
  If the head of the timer list is modified, update the timer match value.
  Will cause an interrupt to be generated immediately if there is an
  expired timer.

DEPENDENCIES
  Internal timer_ function.
  Must be called from ATS_INTLOCKED context.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt when slow clock counter reaches the first timer's expiry point

=============================================================================*/

static void timer_update_timer_interrupt
(
  uint8 caller
)
{
  /* Current slow clock count */
  time_timetick_type                   now = 0;

  /* Time of first expiring timer */
  time_timetick_type                   first=TIMER_MAX_EXPIRY;
  timer_ptr_type                       head_timer=NULL;

  /* Minimum re-arm provided by DalTimetick call */
  DalTimetickTime32Type                minReArm;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* NOTE: This function has many early returns. */

  /* Skip updating the interrupts if not allowed */
  if ( FALSE == timers.do_reprogram_isr )
  {
    return;
  }

  /* If a timer is being altered inside "timer_process_active_timers" ... */
  if ( timers.processing )
  {
    /* ... don't bother updating the timer match value!  Caller will. */
    return;
  }



  /* The following two tests deal with timers.set_value, which is the time
     of the Already Programmed timer interrupt, which may have no relation to
     the first timer on the active timer list. */


  /* Get the current time */
  DalTimetick_GetTimetick64(hTimerHandle, &now);


  /* If the timer interrupt has already occurred ... */
  if ( timers.set_value - timers.set_time <= now - timers.set_time )
  {
    /* ... let the Timer ISR/Task update the next timer match value */
    return;
  }


  /* If the timer interrupt is just a little in the future ... */
  DalTimetick_GetMinRearmSclk(hTimerHandle,&minReArm);
  if ( timers.set_value - now <= minReArm )
  {
    /* ... we can't stop it from been generated.  Don't try to. */
    return;
  }

  first = timer_determine_next_timer_expiry(&head_timer);

  /* If the first expiring timer matches the timer.set_value, ... */
  if ( first == timers.set_value )
  {
    /* ... then it is already properly programmed in TIMETICK_MATCH */
    return;
  }

  /* If the first expiring timer is still in the past ... */
  if ( first < now  )
  {
    /* Set the timer for "as soon as possible" (eg, "now") */
    first = now;
  }


  /* Set the next interrupt match value (unless it is unchanged) */
  timer_set_next_interrupt( first, now, FALSE, caller,head_timer );

} /* timer_update_timer_interrupt */


/*=============================================================================

                           TIMER BIN FUNCTIONS

=============================================================================*/


/*=============================================================================

FUNCTION TIMER_BIN_LIST_INSERT_BETWEEN

DESCRIPTION
  Inserts a bin into the specified list between the two specified bin positions 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_list_insert_between
(
  /* Bin list to insert bin into */
  timer_bin_list_ptr              list, 

  /* Bin to insert in bin list */
  timer_bin_ptr                   bin,

  /* Which node bin should be inserted after */
  timer_bin_ptr                   ptr1,

  /* Which node bin should be inserted before */
  timer_bin_ptr                   ptr2
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update our bin's prev/next ptrs to point at correct bin structs */
  bin->prev_bin = ptr1; 
  bin->next_bin = ptr2;


  /* Update surrounding prev/next ptrs (if necessary) to point to our 
     newly inserted bin */

  if( ptr1 != NULL )
  {
    ptr1->next_bin = bin;
  }
  else
  {
    list->first = bin; /* we have a new start of list, update first ptr */
  } 

  if( ptr2 != NULL )
  {
    ptr2->prev_bin = bin;
  }
  else
  {
    list->last = bin; /* we have a new end of list, update last ptr */
  }

  list->count++;

} /* timer_bin_list_insert_between */



/*=============================================================================

FUNCTION TIMER_BIN_LIST_REMOVE

DESCRIPTION
  Removes a bin from the specified list

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The popped timer_bin_type

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_list_remove
(
  /* Bin list to removed bin from */
  timer_bin_list_ptr              list, 

  /* Bin to remove from bin list */
  timer_bin_ptr                   bin
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( bin->prev_bin != NULL )
  {
    bin->prev_bin->next_bin = bin->next_bin;
  }
  else
  {
    /* This was the head, so update first ptr */
    list->first = bin->next_bin;
  }

  if( bin->next_bin != NULL )
  {
    bin->next_bin->prev_bin = bin->prev_bin;
  }
  else
  {
    /* This was the tail, so updated last ptr */
    list->last = bin->prev_bin;
  }

  /* Update list's count */
  list->count--;

} /* timer_bin_list_remove */



/*=============================================================================

FUNCTION TIMER_BIN_LIST_POP

DESCRIPTION
  Pops a bin off the head of the specified list and returns it 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The popped timer_bin_type

SIDE EFFECTS
  None

=============================================================================*/

static timer_bin_ptr timer_bin_list_pop
(
  /* Bin list to remove the head bin from */
  timer_bin_list_ptr              list
) 
{
  /* Bin at head of bin list */
  timer_bin_ptr                   bin = list->first;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( bin != NULL )
  {
    timer_bin_list_remove( list, bin );
  }

  return bin;

} /* timer_bin_list_pop */



/*=============================================================================

FUNCTION TIMER_BIN_LIST_PUSH

DESCRIPTION
  Pushes a bin onto the head of the specified list 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_list_push
(
  /* Bin list to push bin onto */
  timer_bin_list_ptr              list, 

  /* Bin to push onto bin list */
  timer_bin_ptr                   bin
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  timer_bin_list_insert_between( list, bin, NULL, list->first );

} /* timer_bin_list_push */



/*=============================================================================

FUNCTION TIMER_BIN_CREATE_BETWEEN

DESCRIPTION
  Takes a new bin from free bin pool and adds it between the specified bins
  on the active bin list 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The created timer_bin_type

SIDE EFFECTS
  None

=============================================================================*/

static timer_bin_ptr timer_bin_create_between
(
  /* Bin to create new bin after */
  timer_bin_ptr                   bin1, 

  /* Bin to create new bin before */
  timer_bin_ptr                   bin2,

  /* Timer which will exist in the bin (used to define bin range) */
  timer_ptr_internal_type         timer
) 
{ 
  /* Newly created bin */
  timer_bin_ptr                   new_bin;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Ensure we have a free bin in the pool */
  ASSERT( timers.bins_free.count > 0 );

  /* Pop it off */
  new_bin = timer_bin_list_pop( &timers.bins_free );

  /* Initialize the bin's info */
  new_bin->first  = NULL;
  new_bin->last   = NULL;
  new_bin->count  = 0;

  /* Determine the range of this new bin */
  new_bin->lower = timer->expiry & ~(TIMER_BIN_RANGE); 
  new_bin->upper = new_bin->lower  |  (TIMER_BIN_RANGE); 

  /* finally, compare against prev and next bins to ensure no overlap */

  if( bin1 != NULL )
  {
    /* is the bottom portion of the bin overlapping? */
    if( bin1->upper >= new_bin->lower )
    {
      /* yes, use the upper bound of the prev bin as our upper bound */
      new_bin->lower = bin1->upper + 1;
    }
  }
  if( bin2 != NULL )
  {
    /* is the top portion of the bin overlapping? */
    if( bin2->lower <= new_bin->upper )
    {
      /* yes, use the lower bound of the next bin as our upper bound */
      new_bin->upper = bin2->lower - 1;
    }
  }
        
  timer_bin_list_insert_between( &timers.bins, new_bin, bin1, bin2 );  

  return new_bin;

} /* timer_bin_create_between */



/*=============================================================================

FUNCTION TIMER_BIN_FIND_CREATE

DESCRIPTION
  Searches the active bin list for a bin corresponding to the specified timer  
  If none exists, create it and link it into the bin list

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The timer_bin_type whose range the timer fits into  

SIDE EFFECTS
  None

=============================================================================*/

static timer_bin_ptr timer_bin_find_create
( 
  /* Timer for which a bin is needed */
  timer_ptr_internal_type        timer
)
{
  /* Pointer to a bin.  Used for searching the list of active timer bins */
  timer_bin_ptr                   bin;

  /* Timer's expiry. */
  time_timetick_type                   expiry;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* NOTE: This function has many early returns. */


  ASSERT( timer->list != NULL );

  bin    = timers.bins.first;
  expiry = timer->expiry; 

  while( bin != NULL )
  {
    /* Is this bin able to hold this timer? */

    if ( expiry >= bin->lower  &&  expiry <= bin->upper )
    {
      /* Timer fits in this bin, and it doesn't straddle zero, return it */
      return bin;
    }


    /* Should we create a new bin before this bin? */

    if( expiry < bin->lower )
    {
      /* If it fits between this bin and the prev, or if the prev is NULL
         then create one */

      if( bin->prev_bin == NULL  ||  expiry > bin->prev_bin->upper )
      {
        return timer_bin_create_between( bin->prev_bin, bin, timer );
      }
    }

    /* Doesnt fit in this bin, or before this bin, so iterate to next */
    bin = bin->next_bin;

  } /* while( bin != NULL ) */


  /* If we've gotten to the end, create a bin after the last bin */
  return timer_bin_create_between( timers.bins.last, NULL, timer );

} /* timer_bin_find_create */



/*=============================================================================

FUNCTION TIMER_BIN_FIND_FIRST_EXCEEDING

DESCRIPTION
  Returns the first timer on the bin list exceeding the specified 
  timer's range. If none are found, NULL is returned 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The timer meeting the description's criteria.  

SIDE EFFECTS
  None

=============================================================================*/

static timer_ptr_internal_type timer_bin_find_first_exceeding
(
  /* Timer used to determine bin range */
  timer_ptr_internal_type         timer
)
{
  /* First timer in bins */
  timer_ptr_internal_type         first;

  /* Timer bin under consideration */
  timer_bin_ptr                   bin;

  /* Expiry of timer under consideration */
  time_timetick_type                   expiry = timer->expiry;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* NOTE: This function has many early returns. */


  /* Search the bins until we find one with a timer exceeding this timer's
     expiry */

  /* Skip passed all the unbinned expired timers at the head, if any */

  /*For priority timers, do not insert the timer into the 
  bin. Just return NULL*/
  if(TIMER_PRIORITY_0 == timer->timer_priority) 
  {
    return NULL;
  }
  else
  {
    first   = timers.active.list.first;
  }

  while( first != NULL )
  {
    if( first->bin != NULL )
    {
      break;
    }
    first = first->next;
  }

  /* Start with the bin associated with the first on the active list */
  if( first == NULL )
  {
    return NULL;
  }

  for( bin = first->bin;  bin != NULL;  bin = bin->next_bin )
  {
    /* Does this timer fit before the bin's upper bound? */

    if( expiry <= bin->upper )
    {
      /* return the first pointer */ 
      return bin->first;
    }
  }

  /* now continue at the beginning of the bin list and
     progress until we hit the bin of the first timer on the active list */
  for( bin = timers.bins.first; bin != first->bin; bin = bin->next_bin )
  {
    /* Does this timer fit before the bin's upper bound? */

    if( expiry  <= bin->upper )
    {
      /* return the first pointer */ 
      return bin->first;
    }
  }
 
  /* If we've gotten here, return NULL */
  return NULL;

} /* timer_bin_find_first_exceeding */



/*=============================================================================

FUNCTION TIMER_BIN_INSERT

DESCRIPTION
  Inserts the specified timer into the bin.
  This (potentially) involves updating the bin's first/last pointer.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_insert
(
  /* Bin to insert timer in */
  timer_bin_ptr                   bin,

  /* Timer to insert in bin */
  timer_ptr_internal_type         timer 
)
{
  /* Lowest expiry time in first bin */
  time_timetick_type                   bin_lowest;

  /* Highest expiry time in last bin */
  time_timetick_type                   bin_highest;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Since it is known that a bin does not straddle a counter loop,
     these compares can be done without a zero reference */


  /* Calculate the lowest timer expiry in the bin */

  if( bin->first != NULL )
  {
    bin_lowest  = bin->first->expiry;

    /* do we need to update the first pointer? */
    if( timer->expiry < bin_lowest )
    {
      /* We have a new first pointer */

      /* Note: if the expiries are equal, the first pointer remains the same
         to match the behavior of the timer list */ 
      bin->first = timer;
    } 

  }
  else
  {
    /* If no first pointer, update first pointer with new timer */
    bin->first = timer;
  } 

  /* Calculate the highest timer expiry in the bin */
  if( bin->last != NULL )
  {
    bin_highest = bin->last->expiry;

    /* Do we need to update the last pointer? */
    if( timer->expiry >= bin_highest )
    {
      /* We have a new last pointer */

      /* Note: if the expiries are equal, the last pointer is updated 
         to match the behavior of the timer list */ 
      bin->last = timer;
    }

  }
  else
  {
    /* If no last pointer, update last pointer with new timer */
    bin->last = timer;
  }

  /* Record the bin in the timer struct */
  timer->bin = bin; 

  /* Increment the bin's count */
  bin->count++;

} /* timer_bin_insert */



/*=============================================================================

FUNCTION TIMER_BIN_REMOVE

DESCRIPTION
  Remove the specified timer from the bin.
  This (potentially) involves updating the bin's first/last pointer.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_remove
(
  /* Timer to remove from bin list */
  timer_ptr_internal_type         timer
)
{
  /* Bin timer belongs to */
  timer_bin_ptr                   bin = timer->bin;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (bin == NULL)
   {
     ASSERT(0);
	 //return;
   }


  /* Do we now have an empty bin? */

  if( bin->count == 1 )
  {
    /* empty bin */
    bin->first = NULL;
    bin->last  = NULL;
  }
  else if( bin->count > 1 )
  {
    /* Check if we have a new bin first */
    if( timer == bin->first )
    { 
      bin->first = timer->next;
    }

    /* Check if we have a new bin last */ 
    if( timer == bin->last )
    {
      bin->last = timer->prev;
    }
  }

  /* Unbin the timer */ 
  timer->bin = NULL;

  /* Decrement the bin's count */
  bin->count--;  

  if( bin->count == 0 )
  {
    /* If this bin is now empty, then remove it from the active bin list
       and return it to the free list */
    timer_bin_list_remove( &timers.bins, bin );
    timer_bin_list_push( &timers.bins_free, bin);
  }

} /* timer_bin_remove */



/*=============================================================================

FUNCTION TIMER_INSERT_BETWEEN

DESCRIPTION
  Places 'timer' before 'ptr1' and after 'ptr2' in 'list'.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void timer_insert_between
(
  /* List to insert timer into */
  timer_list_ptr                  list,

  /* Timer to insert into list */
  timer_ptr_internal_type         timer, 

  /* Timer to insert after */
  timer_ptr_internal_type         ptr1,

  /* Timer to insert before */
  timer_ptr_internal_type         ptr2
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update our timer's prev/next ptrs to point at correct timer structs */

  timer->prev = ptr1; 
  timer->next = ptr2;


  /* Update surrounding prev/next ptrs (if necessary) to point to our 
     newly inserted timer */

  if( ptr1 != NULL )
  {
    ptr1->next = timer;
  }
  else
  {
    list->first = timer; /* We have a new start of list, update first ptr */
  } 

  if( ptr2 != NULL )
  {
    ptr2->prev = timer;
  }
  else
  {
    list->last = timer; /* We have a new end of list, update last ptr */
  }
 
} /* timer_insert_between */



/*=============================================================================

FUNCTION TIMER_INSERT

DESCRIPTION
  Inserts a timer into the sorted list of timers.
  Timers are sorted according to increasing expiration times

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_insert
(
  /* List timer is to be inserted into */
  timer_list_ptr                  list,

  /* Timer to be inserted into list of active timers */
  timer_ptr_internal_type         timer
)
{
  /* Pointer to a timer. Used for walking list of timers */
  timer_ptr_internal_type         ptr;

  /* Pointer to a timer. Determines where on the list to start looking */
  timer_ptr_internal_type         start=NULL;
  
  /* Current slow clock count */
  time_timetick_type                   now = 0;
  
  /* bin timetick bounds */
  time_timetick_type                   bin_lower, bin_upper;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  DalTimetick_GetTimetick64(hTimerHandle, &now);

  /* First set the list field of the timer */
  timer->list = list;

  /* Update last_start in ext_timer */
  DalTimetick_GetTimetick64(hTimerHandle, &timer->timer_ext->last_start);

  /* if this timer is to be inserted on the active list 
   For priority list, as of now there will be no binning*/
  if( timer->list == &timers.active.list)
  {

    /* If this timer is going on the active list we can use binning 
       to help us cut down on the list search time */

     /* Obtain the first timer from the bin exceeding this timer's expiry */
    start = timer_bin_find_first_exceeding( timer ); 

    /* Increment the number of timers */
    timer_active_timers_count++; 
  }
  else /*it can be disabled or p0 timer*/
  {
    /* If this is not an active timer, do not optimize the search,
       just search from the start of the list */
    start = list->first;

    /*increment the # of active timers if its a priority list*/
    if(timer->list == &timers.p0_active.list) 
    {
      timer_active_timers_count++;
    }
  }

  /*do  not attempt to find bin for priority timers*/
  if(start != NULL && start->bin != NULL)
  {
     /*Debug*/
     if(TIMER_PRIORITY_0 == timer->timer_priority) 
     {
       ERR_FATAL("1- Priority timers should not have been binned",0,0,0);
     }
     /*end debug*/

     bin_lower = start->bin->lower;
     bin_upper = start->bin->upper;
     
     /* Ignore the bin's first timer to insert new timer into timers list
        if the 'now is in between bin's upper and lower boundary' and first  
        timer of the bin is before now. In such case, Search from start of
        timers list to insert new timer. */
     if(now > bin_lower && now < bin_upper && start->expiry < now) 
        start = NULL;
  }
  
  /* Initialize the start position */
  if( start != NULL )
  {
    ptr = start;
  }
  else
  {
    ptr = list->first;
  }

  /* Search for appropriate list location to insert timer */
  while ( ptr != NULL &&  ptr->expiry <= timer->expiry )
  {
    ptr = ptr->next;
  }

  /* Insert the timer into the list */
  timer_insert_between( list, timer, ptr ? ptr->prev : list->last, ptr ); 

  /* Only bin this timer if we are inserting into the active list 
   Do not insert the priority timer into the bin*/
  if( timer->list == &timers.active.list)
  {
    /* Insert thie timer into the appropriate timer list bin */
    timer_bin_insert( timer_bin_find_create(timer), timer );
  }

  /* Update the first non-deferrable timer */
  if (timer->group->deferrable == 0 &&
      (timers.first_non_deferrable == NULL ||
       timers.first_non_deferrable->expiry > timer->expiry))
  {
    /* Set this timer as the first non-deferrable
       (regardless of if it's p0 or not) */
    timers.first_non_deferrable = timer;
  }

  /* Throw an error fatal if timer list is corrupted */
  timer_active_list_check();

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  timer_sets[timer_set_cnt].timer_to_be_set = timer->timer_ext;
  timer_sets[timer_set_cnt].ts = now;
  timer_sets[timer_set_cnt].expiry = timer->expiry;
  timer_set_cnt = (timer_set_cnt+1)%MAX_TIMER_SET;
  timer_set_cnt_global++;

  /* This is a dummy statement to remove compiler warnings */
  timer_sets[timer_set_cnt].ts = timer_sets[timer_set_cnt].ts;
  
  #endif

} /* timer_insert */



/*=============================================================================

FUNCTION TIMER_INSERT_LIST

DESCRIPTION
  Inserts a sorted list of timers into the sorted list of timers.
  Timers are sorted according to increasing expiration times

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_insert_list
(
  /* List timers are to be inserted into */
  timer_list_ptr                  dst_list,

  /* Linked list of timers to be inserted into the first list */
  timer_ptr_internal_type         src_list
)
{
  /* A timer from the second list, to be inserted in the first */
  timer_ptr_internal_type         timer;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Repeat for all timers on the source list */
  while ( src_list != NULL )
  {
    /* Retrieve a timer from the start of the source list */
    timer    = src_list;
    src_list = src_list->next;

    timer_insert( dst_list, timer );
  }

} /* timer_insert_list */



/*=============================================================================

FUNCTION TIMER_CONCATENATE_LISTS

DESCRIPTION
  Concatenates the src_list and the dst_list with the combined list taking on
  the identity of the dst_list.  src_list is cleared.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_concatenate_lists
(
  /* Linked list of timers to be inserted into the first list */
  timer_list_ptr                  src_list,

  /* List timers are to be inserted into */
  timer_list_ptr                  dst_list
)
{
  /* timer iteration pointer */
  timer_ptr_internal_type         timer;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (src_list==NULL)
  {
    ASSERT(0);
	//return;
  }

  /* Check if the source list is empty */
  if( src_list->first == NULL )
  {
    return;
  }

  /* Stitch the two lists together */

  /* Is the destination list empty? */
  if( dst_list->first != NULL )
  { 
    /* No, so update the head's previous pointer */
    dst_list->first->prev = src_list->last;

    /* Update the source list's last next pointer */
    src_list->last->next = dst_list->first; 

  }
  else
  {
    /* Yes, so update the destination list's last pointer */
    dst_list->last = src_list->last;
  }

  /* Update the destination lists's first pointer */
  dst_list->first = src_list->first;
 
  /* Now, for each of the src list items, update their structures */
  timer = src_list->first;
  while( timer != NULL )
  {
    /* Update the list membership */
    timer->list = dst_list;

    /* If this is the last of the src list, we are done */
    if( timer == src_list->last )
    {
      break;
    }

    /* Proceed to next timer */
    timer = timer->next; 
  }

  /* Finally update the source list */
  src_list->first = NULL;
  src_list->last  = NULL;
    
} /* timer_concatenate_lists */



/*=============================================================================

FUNCTION TIMER_UNLINK

DESCRIPTION
  Simply unlinks a timer from its current list of timers.
  Updates the associated list first/last ptrs

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_unlink
(
  /* Timer to remove from its current list */
  timer_ptr_internal_type        timer
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( timer->prev != NULL )
  {
    timer->prev->next = timer->next;
  }
  else
  {
    /* This was the head, so update first ptr */
    timer->list->first = timer->next;
  }

  if( timer->next != NULL )
  {
    timer->next->prev = timer->prev;
  }
  else
  {
    /* This was the tail, so updated last ptr */
    timer->list->last = timer->prev;
  }

} /* timer_unlink */

/*=============================================================================

FUNCTION TIMER_REMOVE_INTERNAL_TIMER_FROM_Q

DESCRIPTION
  1. Puts the timer back in free q
  2. Sets the external timer handle to INVALID handle
  
DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_remove_internal_timer_from_q
(
  timer_ptr_internal_type timer  
)
{
   /* Cache Address of external timer */
  timer_ptr_type                 ext_timer;

  /* Temporary index */
  int32                          temp_index;

   if ( timer->internal_q == TIMER_IN_FREE_Q )
    ERR_FATAL ( " Removing timer_buffer which is not in the list, thread_id=0x%x timer_ext=0x%x",
                timer->thread_id,timer->timer_ext,0);
      
   /* Get location of external timer */
   ext_timer = timer->timer_ext;
  
   /* Save index value in a teemp var */
   temp_index = timer->index;

   /* Clear expired timer buffer */
   memset( timer, 0, sizeof( timer_internal_type ));

   /* Restore timer index*/
   timer->index = temp_index ;

   if ( ext_timer != NULL )
   {
     ext_timer->timer_handle = TIMER_BUFFER_INVALID_HANDLE ;
   }
   else
   {
     ERR_FATAL ( " Timer must have an external timer, thread_id=0x%x timer_ext=0x%x timer_ptr=0x%x",
                timer->thread_id,timer->timer_ext,timer);
   }

   timer->internal_q = TIMER_IN_FREE_Q;
  
   /* Link timer_buffer element back to timer_free_q */
   (void) q_link( timer, 
             &(timer->link) );

   /* Enqueue node in timer_free_q */
   q_put( &timer_free_q, &(timer->link) );

   timer_count_buffer_allocated = MAX_TIMER_BUFFER_NUMBER-q_cnt(&timer_free_q);
}

/*=============================================================================

FUNCTION TIMER_REMOVE

DESCRIPTION
  Removes a timer from the list of timers.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_remove
(
  /* Timer to be removed from list of active timers */
  timer_ptr_internal_type         timer,
  
  boolean                          remove_from_q
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( timer == NULL || timer->list == NULL )
  {
    return;
  }

  if( timer->list == &timers.active.list 
      || timer->list == &timers.p0_active.list )
  {
    /* Decrement the number of timers if timer belongs to active list */
    timer_active_timers_count--; 
  }

  /* if this timer is to be removed from the active list */
  if( timer->bin != NULL )
  {

    /*debug*/
    if(TIMER_PRIORITY_0 == timer->timer_priority) 
    {
      ERR_FATAL("2- Priority timers should not have been binned",0,0,0);
    }
    /*end debug*/

    /* Unbin this timer */
    timer_bin_remove( timer );
  }


  /* Fix up links/list around this node */

  if ( timer->prev )
  {
    timer->prev->next = timer->next;
  }
  else
  {
    timer->list->first = timer->next;
  }

  if ( timer->next )
  {
    timer->next->prev = timer->prev;
  }                                 
  else
  {
    timer->list->last = timer->prev;
  }


  timer->next = NULL;
  timer->prev = NULL;
  timer->list = NULL;

  /* Donot remove if timer is to be resumed */
  if(remove_from_q == TRUE)
  {
    timer_remove_internal_timer_from_q(timer);
  } /* remove_from_q == TRUE */

  /* Update the first non-deferrable timer */
  if (timers.first_non_deferrable == timer)
  {
    /* Since the first non-deferrable timer was removed,
       we need to get the next non-deferrable timer */
    timer_update_first_active_non_deferrable_timer();
  }

  /* Throw an error fatal if timer list is corrupted */
  timer_active_list_check();

} /* timer_remove */



/*=============================================================================

FUNCTION TIMER_REMOVE_HEAD

DESCRIPTION
  Removes the head of the specified timer list.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_remove_head
(
  /* List to chop head off of */
  timer_list_ptr                  list
)
{
  /* Head of the timer list */
  timer_ptr_internal_type         head = list->first;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( list == &timers.active.list
      || list == &timers.p0_active.list )
  {
    /* Decrement the number of timers if timer belongs to active list */
    timer_active_timers_count--; 
  }

  /* New front of list */
  list->first = head->next;

  /* Fix up links/list around this node */
  if( list->first != NULL )
  {
    /* Update next node's prev ptr */
    list->first->prev = NULL;
  }
  else
  {
    /* Empty list */
    list->last = NULL;
  }


  /* NULL out deleted node's link fields */
  head->next = NULL;
  head->list = NULL;

  if ( head->internal_q == TIMER_IN_FREE_Q )
    ERR_FATAL ( " Removing timer_buffer which is not in the list, thread_id=0x%x timer_ext=0x%x timer_ptr=0x%x",
                head->thread_id,head->timer_ext,head);

  /* Update the first non-deferrable timer */
  if (timers.first_non_deferrable == head)
  {
    /* Since the first non-deferrable timer was removed,
       we need to get the next non-deferrable timer */
    timer_update_first_active_non_deferrable_timer();
  }

  /* Throw an error fatal if timer list is corrupted */
  timer_active_list_check();

} /* timer_remove_head */



/*=============================================================================

FUNCTION TIMER_APPEND

DESCRIPTION
  Appends the list with a timer
  The appended timer's list member is updated

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_append
(
  /* List to add timer to */
  timer_list_ptr                  list,

  /* Timer to add to list */
  timer_ptr_internal_type         timer
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update timer's links */
  timer->prev = list->last;
  timer->next = NULL;

  /* Update the old tail to point to this new timer */
  if( list->last != NULL )
  {
    list->last->next = timer;
  }
  else
  {
    /* Empty list, have a new head */
    list->first = timer;
  } 
  
  /* Always have a new tail */ 
  list->last = timer;
 
  /* We have a new list for timer */ 
  timer->list = list;

} /* timer_append */



/*=============================================================================

FUNCTION TIMER_IN_LIST

DESCRIPTION
  Returns whether the timer actually exists in the specified list

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static boolean timer_in_list
(
  /* Timer under test */
  timer_ptr_internal_type         timer,

  /* List timer may be a member of */
  timer_list_ptr                  list
)
{
  /* Used to walk timer list */
  timer_ptr_internal_type         ptr; 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Do the search */
  for( ptr = list->first; ptr != NULL; ptr = ptr->next )
  {
    if( ptr == timer )
    {
      /* Timer is in list */
      return TRUE;
    }
  }

  /* Timer was not in list */
  return FALSE;

} /* timer_in_list */


/*=============================================================================

FUNCTION TIMER_REMOTE_EXPIRE

DESCRIPTION
  Processes an expiring timer, according to the timer's definition.

DEPENDENCIES
  Internal timer_ function()
  Must be called from inside ATS_INTLOCK & TASKLOCK (or interrupt) contexts

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

static void timer_remote_expire
(
  /* Expiring timer to be processed */
  timer_ptr_internal_type         timer,

  /* Duration timer ran for. */
  time_timetick_type                   run_time
)
{
  /* Flag indicating if timer has been properly processed */
  boolean                         processed = FALSE;

  /* Duration timer ran for, in milliseconds */
  time_timetick_type                   run_time_ms = 0;

  /* Duration timer ran for, in milliseconds */
  time_timetick_type                   run_time_sclk;

  /* Timer calllback type */
  timer_notify_type               notify_type;  

  /*to logs when the cb processing starts*/
  time_timetick_type cb_processing_started = 0;

  /*to log when the cb processing ends*/
  time_timetick_type cb_processing_ended = 0;

  /*to log the processing time*/
  time_timetick_type cb_processing_time = MAGIC_INITIALIZER_64;

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES  
  /*fn address*/
  uint32  fn_address_of_timer = 0;
  #endif

  #ifdef FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION
  /*add the timer to isolation list if required*/
  boolean add_timer_to_isolation_list     = FALSE;
  
  uint32 count  = 0;
  uint32 index  = 0;
  #endif

  #ifdef FEATURE_TIMER_SLAVE1
  timer_slave_post_cmd_params_type slave_cmd;
  memset(&slave_cmd, 0, sizeof(timer_slave_post_cmd_params_type));
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Cache information from timer structure since it could get deleted any
  time after the first callback is made */
  notify_type = timer->notify_type;

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  timers_remote_expired[timer_remote_expired_cnt].timer_expired = timer->timer_ext;
  timers_remote_expired[timer_remote_expired_cnt].tid = timer->thread_id;
  timers_remote_expired[timer_remote_expired_cnt].call_back_type = (uint8)notify_type ;
  timers_remote_expired[timer_remote_expired_cnt].processing_started = 0;
  timers_remote_expired[timer_remote_expired_cnt].processing_time = MAGIC_INITIALIZER_64;

  /* This is a dummy statement to remove compiler warnings */
  timers_remote_expired[timer_remote_expired_cnt].processing_time = 
  timers_remote_expired[timer_remote_expired_cnt].processing_time;
  
  #endif

  /*log the time when processing started*/
  DalTimetick_GetTimetick64(hTimerHandle, &cb_processing_started);

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  /*Log the time to indicate the start of processing of this timer*/
  timers_remote_expired[timer_remote_expired_cnt].processing_started = cb_processing_started;
  #endif

  /* If there are signals to be set, and a task / signal object to set the signals to,
     then set those signals in that task's task control block or the passed signal object */

  /* Check if this timers has no notification types or if it is invalid */
  if (notify_type == TIMER_NO_NOTIFY_TYPE)
	 {
	   // ATS_MSG_HIGH("Timer = 0x%x expired; External timer = 0x%x",timer,timer->timer_ext,0);
	 }
  else if (notify_type & ~TIMER_NOTIFICATION_CHECK_MASK)
	 {
	   ERR_FATAL("Timer = 0x%x : INVALID NOTIFICATION TYPE = %d",timer,timer->notify_type,0);
	 }

  if ( timer_qapi_module_notification_handler != NULL)
	 	timer_qapi_module_notification_handler(timer->timer_ext);

  /*log the cb ended time*/
  DalTimetick_GetTimetick64(hTimerHandle, &cb_processing_ended);
  
  /*to take care of the wrap around case*/
  if(cb_processing_ended >= cb_processing_started)
  {
    cb_processing_time = cb_processing_ended - cb_processing_started;       
  }
  else /*if it is a wrap around case*/
  {  
    cb_processing_time = 0xFFFFFFFFFFFFFFuLL - 
                        (cb_processing_started - cb_processing_ended)+1;
  }

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  timers_remote_expired[timer_remote_expired_cnt].processing_time = cb_processing_time;
  timer_remote_expired_cnt = (timer_remote_expired_cnt+1)%MAX_TIMER_REMOTE_EXPIRED;
  timer_expired_cnt_global++;
  #endif

  /*generate a warning or error fatal out if the processing time > threshold*/
  if(cb_processing_time > timer_processing_time_warning_threshold)
  {
    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    ATS_ERR_3("Processing of callback fn = 0x%x took 0x%x ticks, threshold = 0x%x ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_processing_time_warning_threshold);          
    #endif
    
  }

  if(timer_processing_time_err_fatal_threshold != 0 && 
     cb_processing_time > timer_processing_time_err_fatal_threshold)
  {
    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    ERR_FATAL("Processing of callback fn = 0x%x took 0x%x ticks, threshold = 0x%x ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_processing_time_err_fatal_threshold);          
    #endif
    
  }

} /* timer_remote_expire */



/*=============================================================================

FUNCTION TIMER_EXPIRE

DESCRIPTION
  Processes an expiring timer, according to the timer's definition.

DEPENDENCIES
  Internal timer_ function()
  Must be called from inside ATS_INTLOCK & TASKLOCK (or interrupt) contexts

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

static void timer_expire
(
  /* Expiring timer to be processed */
  timer_ptr_internal_type         timer,

  /* Duration timer ran for. */
  time_timetick_type                   run_time
)
{
  /* Flag indicating if timer has been properly processed */
  boolean                         processed = FALSE;

  /* Duration timer ran for, in milliseconds */
  time_timetick_type                   run_time_ms = 0;

  /* Duration timer ran for, in milliseconds */
  time_timetick_type                   run_time_sclk;

  /* Timer calllback type */
  timer_notify_type               notify_type;  

  /* Timer 'type 1' calllback function */
  timer_t1_cb_type                cb1;

  /* Timer 'type 2' calllback function */
  timer_t2_cb_type                cb2;

  /*to logs when the cb processing starts*/
  time_timetick_type cb_processing_started = 0;

  /*to log when the cb processing ends*/
  time_timetick_type cb_processing_ended = 0;

  /*to log the processing time*/
  time_timetick_type cb_processing_time = MAGIC_INITIALIZER_64;

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES  
  /*fn address*/
  uint32  fn_address_of_timer = 0;
  #endif

  #ifdef FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION
  /*add the timer to isolation list if required*/
  boolean add_timer_to_isolation_list     = FALSE;
  
  uint32 count  = 0;
  uint32 index  = 0;
  #endif

  #ifdef FEATURE_TIMER_SLAVE1
  timer_slave_post_cmd_params_type slave_cmd;
  memset(&slave_cmd, 0, sizeof(timer_slave_post_cmd_params_type));
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Cache information from timer structure since it could get deleted any
  time after the first callback is made */
  notify_type = timer->notify_type;
  cb1 = timer->func1;
  cb2 = timer->func2;

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  timers_expired[timer_expired_cnt].timer_expired = timer->timer_ext;
  timers_expired[timer_expired_cnt].tid = timer->thread_id;
  timers_expired[timer_expired_cnt].call_back_type = (uint8)notify_type ;
  timers_expired[timer_expired_cnt].processing_started = 0;
  timers_expired[timer_expired_cnt].processing_time = MAGIC_INITIALIZER_64;

  /* This is a dummy statement to remove compiler warnings */
  timers_expired[timer_expired_cnt].processing_time = 
  timers_expired[timer_expired_cnt].processing_time;
  
  #endif

  /* If a conversion from sclk to milliseconds is necessary, perform it now. */
  if ( cb2 != NULL)
  {
    /* Compute time (in milliseconds) timer has been running for.
       This may be larger than expected if the timer expires during sleep,
       or otherwise has its expiry processing delayed */
    DalTimetick_CvtFromTimetick64(hTimerHandle,run_time,T_MSEC,&run_time_ms);

    /* If a timer is a reloading timer, the millisecond conversion will have
       a rounding error.  We can adjust the start time to account for the
       residual.  This is done before signalling the timer expiry, since
       the timer expiry might alter the timer data */

    if ( timer->list != NULL )
    {
      /* Timer is still on the list.  It is a reloading timer. */

      /* Difference btw run_time & run_time_sclk is rounding error (+/-.5ms) */
      DalTimetick_CvtToTimetick64(hTimerHandle,run_time_ms,T_MSEC,&run_time_sclk);

      /* Nudge start time, so the next expiry includes this rounding error */
      if ( run_time >= run_time_sclk )
      {
        /* Rounded down: move start backwards, so next duration is longer */
        timer->start -= run_time - run_time_sclk;
      }
      else if ( timer->expiry - timer->start > run_time_sclk - run_time )
      {
        /* Rounded up: move start forward, so next duration is shorter */
        timer->start += run_time_sclk - run_time;
      }
      else
      {
        /* Rounded up, but next expiry is too close for rounding nudging. */
        timer->start = timer->expiry - 1;
      }
    }
  }

  /*log the time when processing started*/
  DalTimetick_GetTimetick64(hTimerHandle, &cb_processing_started);

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  /*Log the time to indicate the start of processing of this timer*/
  timers_expired[timer_expired_cnt].processing_started = cb_processing_started;
  #endif

  /* If there are signals to be set, and a task / signal object to set the signals to,
     then set those signals in that task's task control block or the passed signal object */

  if ( notify_type & TIMER_NATIVE_OS_SIGNAL_TYPE )
  {
     qurt_signal_set((qurt_signal_t *)timer->notify_obj_ptr, (time_osal_notify_data)timer->sigs);
  }
  else if ( notify_type & TIMER_REX_TCB_SIGNAL_TYPE )
  {
  #ifdef FEATURE_TIMER_REX_IS_AVAILABLE
      (void) rex_set_sigs( timer->notify_obj_ptr, timer->sigs );
  #else
	  ERR_FATAL("Signal type cannot be rex type ; timer = 0x%x",timer->timer_ext,0,0);
  #endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
  }
  else if ( notify_type & TIMER_NATIVE_OS_SIGNAL2_TYPE )
  {
  #ifdef FEATURE_TIME_QDSP6
		qurt_signal2_set((qurt_signal2_t *)timer->notify_obj_ptr, (time_osal_notify_data)timer->sigs);
  #else
		 ERR_FATAL("Signal type cannot be NATIVE OS type ; timer = 0x%x",timer->timer_ext,0,0);
  #endif /* FEATURE_TIME_QDSP6 */
  }
  else if (notify_type & TIMER_DALSYS_EVENT_SIGNAL_TYPE)
  {
      timer_dalsys_sig_set(timer->notify_obj_ptr);
  }


  /* If the timer was defined with a type-1 style function callback,
     call the function, or post the function as an asynchronous call */

  if ( notify_type & TIMER_FUNC1_CB_TYPE )
  {
    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    fn_address_of_timer = (uint32)cb1;
	#endif
    
    if ( !processed )
    {
      /*queue it to slave task only if it's not priority timer*/
      if(TIMER_PRIORITY_0 != timer->timer_priority)
      { 
        /* Can't queue APC -- call it ourselves */
        #ifdef FEATURE_TIMER_SLAVE1
        slave_cmd.timer_ext = timer->timer_ext;
        slave_cmd.cb_type = TIMER_SLAVE_CB_TYPE1;
        slave_cmd.cb1 = cb1;
        slave_cmd.data = timer->data;
        slave_cmd.timer_slave_task_num = timer->slave_task_assigned;      
        if (1==timer_after_pc_mask[TIMER_PC_MASK_INDEX]) 
        {                                                
           TIMER_PC_MASK_MUTEX_LOCK();                    
           timer_after_pc_mask[slave_cmd.timer_slave_task_num+1]++;  
           TIMER_PC_MASK_MUTEX_UNLOCK();               
        }                                                  
        timer_slave_post_cmd(&slave_cmd);
        #else
		if (cb1 != NULL)
          cb1( timer->data );
        else
          ASSERT(0);
        #endif
      }
      else
      {
        ATS_MSG("Priority timer called from timer task");
		if (cb1 != NULL)
          cb1(timer->data);
        else
          ASSERT(0);
      }
    }

    
  } /* timer->func1 != NULL */


  /* If the timer was defined with a type-2 style function callback,
     (clock callback style), call the function with the time (in
     milliseconds) between timer start and now. */

  if ( notify_type & TIMER_FUNC2_CB_TYPE )
  {
    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    fn_address_of_timer = (uint32)cb2;
	#endif

    if(TIMER_PRIORITY_0 != timer->timer_priority) 
    {
    /* Call the callback function */
    #ifdef FEATURE_TIMER_SLAVE1
    slave_cmd.timer_ext = timer->timer_ext;
    slave_cmd.cb_type = TIMER_SLAVE_CB_TYPE2;
    slave_cmd.cb2 = cb2;
    slave_cmd.run_time = (time_timetick_type)run_time_ms;
    slave_cmd.data = timer->data;
    slave_cmd.timer_slave_task_num = timer->slave_task_assigned;
    if (1==timer_after_pc_mask[TIMER_PC_MASK_INDEX]) 
    {                                                
       TIMER_PC_MASK_MUTEX_LOCK();                  
       timer_after_pc_mask[slave_cmd.timer_slave_task_num+1]++;  
       TIMER_PC_MASK_MUTEX_UNLOCK();                
    }                                                  
    timer_slave_post_cmd(&slave_cmd);
    #else
	if (cb2 != NULL)
      cb2( (time_timetick_type) run_time_ms, timer->data);
    else
      ASSERT(0);
    #endif
  }
    else
    {
      if (cb2 != NULL)
	    cb2( (int32) run_time_ms, timer->data);
      else
        ASSERT(0);
    }
  }

  /* Check if this timers has no notification types or if it is invalid */
  if (notify_type == TIMER_NO_NOTIFY_TYPE)
	 {
	   // ATS_MSG_HIGH("Timer = 0x%x expired; External timer = 0x%x",timer,timer->timer_ext,0);
	 }
  else if (notify_type & ~TIMER_NOTIFICATION_CHECK_MASK)
	 {
	   ERR_FATAL("Timer = 0x%x : INVALID NOTIFICATION TYPE = %d",timer,timer->notify_type,0);
	 }

  /*log the cb ended time*/
  DalTimetick_GetTimetick64(hTimerHandle, &cb_processing_ended);
  
  /*to take care of the wrap around case*/
  if(cb_processing_ended >= cb_processing_started)
  {
    cb_processing_time = cb_processing_ended - cb_processing_started;       
  }
  else /*if it is a wrap around case*/
  {  
    cb_processing_time = 0xFFFFFFFFFFFFFFuLL - 
                        (cb_processing_started - cb_processing_ended)+1;
  }

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  timers_expired[timer_expired_cnt].processing_time = cb_processing_time;

  /*if the timer slave processing is not enabled, then 
    unconditionally increment the counter.
    if the timer slave processing is enabled, then increment 
    only if the signals were set*/
  if((FALSE == timer_slave_processing_enabled)
     || (timer_slave_processing_enabled && ( (notify_type & TIMER_REX_TCB_SIGNAL_TYPE) ||
                                             (notify_type & TIMER_NATIVE_OS_SIGNAL2_TYPE) ||
                                             (notify_type == TIMER_NO_NOTIFY_TYPE) ||
                                             (notify_type & TIMER_DALSYS_EVENT_SIGNAL_TYPE) ||
                                             (notify_type & TIMER_NATIVE_OS_SIGNAL_TYPE) )))
  {
  timer_expired_cnt = (timer_expired_cnt+1)%MAX_TIMER_EXPIRED;
  timer_expired_cnt_global++;
  }
  #endif

  /*generate a warning or error fatal out if the processing time > threshold*/
  if(cb_processing_time > timer_processing_time_warning_threshold)
  {
    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    ATS_ERR_3("Processing of callback fn = 0x%x took 0x%x ticks, threshold = 0x%x ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_processing_time_warning_threshold);          
    #endif
    
    #ifdef FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION
    add_timer_to_isolation_list = TRUE;
    #endif /* FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION */
  }

  if(timer_processing_time_err_fatal_threshold != 0 && 
     cb_processing_time > timer_processing_time_err_fatal_threshold)
  {
    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    ERR_FATAL("Processing of callback fn = 0x%x took 0x%x ticks, threshold = 0x%x ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_processing_time_err_fatal_threshold);          
    #endif
    
    #ifdef FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION
    add_timer_to_isolation_list = TRUE;
    #endif /* FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION */
  }



  #ifdef FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION  
  if(add_timer_to_isolation_list)
  {
    /*first check whether the timer is already present in 
    the isolated timer list*/
    for(count = 0; count < timer_isolated_cnt; count++)
    {
      /*if the timer expired, callback fn and data is already present, then do not add
       to isolation list
       may be only first check is needed and remove the other .data and ,fn_address in formal check in */
      if((timers_isolated[count].timer_expired == timer)
          && (timers_isolated[count].data == timer->data)
          && (timers_isolated[count].fn_address == fn_address_of_timer))
      {
        timers_isolated[count].thresh_exceed_count++;

        /*if we found the timer that we were looking for, make sure that we do not add
         an entry in case the latest processing time is lesser than the one previously 
         recorded.*/
        if(timers_isolated[count].processing_time < cb_processing_time)
        {              
          timers_isolated[count].processing_started = cb_processing_started;
          timers_isolated[count].processing_time = cb_processing_time;
        }
        break;
      }
    }

    /*if we did not break prematurely above, it means we did not find the timer 
     in the isolated list. Therefore add it*/
    if(count == timer_isolated_cnt)
    {
      timers_isolated[count].thresh_exceed_count++;
      timers_isolated[count].timer_expired = (timer_ptr_internal_type)timer;      
      timers_isolated[count].processing_started = cb_processing_started;
      timers_isolated[count].processing_time    = cb_processing_time;
      timers_isolated[count].fn_address         = fn_address_of_timer;
      timers_isolated[count].data               = timer->data;
      timers_isolated[count].call_back_type     = (uint8)timer->notify_type;;

      /*increment the count*/
      timer_isolated_cnt++;
    }

     /*if the isolated timer count has reached the max or threshold, then 
      error fatal out*/
     if((MAX_TIMER_ISOLATED <= timer_isolated_cnt)
         || timers_cnt_assert_threshold == timer_isolated_cnt)
     {
       ATS_ERR_3(" # of timers exceeding warning count = 0x%x, threshold = 0x%x, ticks_threshold = 0x%x",
                  timer_isolated_cnt, timers_cnt_assert_threshold,timer_processing_time_warning_threshold);

       /*isolated timer count wrap around. This is done to make the for loop for isolating the timers 
        managable*/
       timer_isolated_cnt = 0;

     }

     /*if the isolated timer is found in the timer_assert_compare_value_list, then 
      assert*/
     for (index = 0; index < timer_assert_compare_value_count; index++)
     {
       if ( ((uint32)(timers_isolated[count].timer_expired)) == timer_assert_compare_value_list[index]);
       {
         ERR_FATAL("timers_isolated[%d].timer_expired == timer_assert_compare_value_list[%d]",count,index,0);
       }
     }   
  }/*end if */
  #endif /*if FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION*/

} /* timer_expire */

/*=============================================================================

FUNCTION TIMER_DETERMIN_TIMER_EXPIRY

DESCRIPTION
  Sets the timer expiry flag and determines whetehr the callback fn is to 
  be called or not

DEPENDENCIES
  Internal timer_ function()
  Must be called from inside ATS_INTLOCK & TASKLOCK (or interrupt) contexts

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static boolean timer_determine_timer_expiry
(
  timer_ptr_internal_type timer,
  time_timetick_type      timetick_now
)
{
  boolean expire_timer = FALSE;

  if ( timer->timer_ext != NULL )
       timer->timer_ext->last_expiry = timetick_now;
  else
    ERR_FATAL (" Timer must have an ext timer, thread_id=0x%x timer_ext=0x%x timer_ptr=0x%x",
                timer->thread_id,timer->timer_ext,timer);

  /*Check for the integrity of the external timer state*/
  if(NULL == timer->timer_ext->timer_state
     || timer->timer_ext->timer_state < TIMER_DEF_FLAG
     || timer->timer_ext->timer_state > TIMER_RESUMED_FLAG)    
  {
    ERR_FATAL("External timer structure corrupted, state=0x%x timer_ext=0x%x",
                timer->timer_ext->timer_state,timer->timer_ext,0);
  }

  /*Expire the timer only if the timer state is
    resumed or set*/
  if(TIMER_RESUMED_FLAG == timer->timer_ext->timer_state
     || TIMER_SET_FLAG == timer->timer_ext->timer_state )
  {
    timer->timer_ext->timer_state = TIMER_EXPIRED_FLAG;
    expire_timer = TRUE;
  }

  return expire_timer;  
}


/*=============================================================================

FUNCTION TIMER_PROCESS_ACTIVE_TIMERS

DESCRIPTION
  Process timers, decrementing elapsed time from each one.

DEPENDENCIES
  Internal timer_ function()
  Must be called from inside ATS_INTLOCK & TASKLOCK (or interrupt) contexts, to
  prevent a large delay between reading the current timetick count, and
  writing the new timetick match value.

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

static void timer_process_active_timers( void )
{
  /* Current slow clock count */
  time_timetick_type                   ticks_now = 0;

  /* Timer being processed */
  timer_ptr_internal_type         timer;

  /* Length of time the timer ran for */
  time_timetick_type                   run_time;

  /* Minimum advance required for reloading timer */
  time_timetick_type                   min_advance;


  /* Temporary value to compute the new expiry point */
  time_timetick_type                   new_expiry;

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  /* Temporary value to compute the new expiry point */
  time_timetick_type                   local_timer_expiry;
  #endif

  /* Cache Address of external timer */
  timer_ptr_type                 ext_timer;

  /* Temporary index */
  int32                          temp_index;

  /*to determine whether timer is to be expired or not*/
  boolean                        expire_timer = FALSE;

  struct timer_struct_internal          timer_temp_internal;

  time_timetick_type             next_timer_expiry =TIMER_MAX_EXPIRY;

  timer_ptr_type                 head_timer=NULL;

  timer_list_type*               current_list = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&timer_temp_internal,0,sizeof(timer_temp_internal));

  /* Lock interrupts while testing & manipulating the active timer list */
  ATS_INTLOCK();

  /* Record the fact that timers are being processed, to prevent re-entry
     into this function, and unnecessary modification of TIMETICK_MATCH. */
  timers.processing = TRUE;

  /* Save a trace packet */
  TIMER_TRACE(PROCESS_START);

  /* Get current slow clock count */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  /* Check to determine if the timer at head of the active timer list has
     past its expiry point.  If it has, remove it, expire the timer, and
     repeat with the new timer at the head of active timer list.
       
     Note: a timer's expiry can change timers.active.list.zero, so no local
     variable should hold a calculated value involving it from one iteration
     to the next. */

  timer_while_cnt = 0;

  /*start off with p0 active list*/
  current_list = &timers.p0_active.list;
 
  do
  {
    
    while ( timer = current_list->first,
          timer != NULL
            &&  timer->expiry 
                 <= ticks_now + TIMER_OS_EARLY_TOL)
  {
    timer_while_cnt++;
    if( timer->bin != NULL )
    {
      /*debug*/
      if(TIMER_PRIORITY_0 == timer->timer_priority) 
      {
        ERR_FATAL("3- Priority timers should not have been binned",0,0,0);
      }

      /* Unbin this timer */
      timer_bin_remove( timer );
    }

    /*debug*/
    if(current_list == &timers.p0_active.list) 
    {
      ATS_MSG("Priority Timer expired");
    }

    /*end debug*/


#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    timer_expired_debug[timer_expired_debug_cnt].whileloopcnt = timer_while_cnt;
    timer_expired_debug[timer_expired_debug_cnt].timer_exp_ptr = timer->timer_ext;
    DalTimetick_GetTimetick64(hTimerHandle, &timer_expired_debug[timer_expired_debug_cnt].ts);
    timer_expired_debug[timer_expired_debug_cnt].start = timer->start;
    timer_expired_debug[timer_expired_debug_cnt].expiry = timer->expiry;
    if( timer->notify_obj_ptr != NULL && timer->sigs != 0 )
    {
      timer_expired_debug[timer_expired_debug_cnt].callback_type = 0x1;
      timer_expired_debug[timer_expired_debug_cnt].tcb_or_func = (uint32)timer->notify_obj_ptr;
    }
    else if( timer->func1 )
    {
      timer_expired_debug[timer_expired_debug_cnt].callback_type = 0x2;
      timer_expired_debug[timer_expired_debug_cnt].tcb_or_func = (uint32)timer->func1;
    }
    else
    {
      timer_expired_debug[timer_expired_debug_cnt].callback_type = 0x4;
      timer_expired_debug[timer_expired_debug_cnt].tcb_or_func = (uint32)timer->func2;
    }
    timer_expired_debug[timer_expired_debug_cnt].slave_assigned = timer->slave_task_assigned;
    timer_expired_debug_cnt = (timer_expired_debug_cnt+1)%MAX_TIMER_EXPIRED_DEBUG;
#endif /*FEATURE_ENABLE_TIMER_DEBUG_VARIABLES*/

    /* Remove expiring timer from active timer list */
      timer_remove_head( current_list );
     
    /* Compute length of time the timer ran for.  This may be longer than
       programmed due to sleep or other delays in expiry processing. */
    run_time = ticks_now - timer->start;

    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    local_timer_expiry = timer->expiry;
    #endif

    /* Reactivate timer, if required */
    if ( timer->reload > 0 )
    {
      /* Determine how late the timer expired; this is the minimum
         amount the timer must be advanced by for the next expiry. */
      min_advance = ticks_now - timer->expiry;
  
      new_expiry = timer->expiry + timer->reload;
      
      if(new_expiry  <= ticks_now)
      {
        /* Temporary value to compute the new expiry point */
        new_expiry = timer->expiry;
        
        /* Timer expired 1 or more reload period ago.  This can happen if
           the timer belongs to a timer group which gets disabled, such as
           the default timer groups during sleep. */

        /* Round min_advance up to the next multiple of reload periods. */
        min_advance += timer->reload - min_advance % timer->reload;

        /* Add the rounded-up minimum advance to the timer expiry */
        new_expiry += min_advance;
      }

        /* Check to make sure that the new expiry point is further in the future
           than the old one.  This prevents the cases where overflow in the
           calculation could occur or wrap around past the active timer list
           zero. */
        
        if (new_expiry > timer->expiry )
        {
          /* New expiry point is further in the future than the old one, use it */
          timer->expiry = new_expiry;
        }
        else
        {
          /* The recurring timer’s new expiry point is at a point so far in the 
             future that it exceeds timer dynamic range.  Because of this, move  
             the expiry to just fit the available range */

          /* Move the expiry point as far out as possible */
          timer->expiry = TIMER_MAX_EXPIRY;
      
          ATS_ERR("Timer expiry set to max value. Timer may never expire !");
        }

      /* Record the new start time for the next expiry */
      timer->start = ticks_now;

      /* Update sclk_delta for the timer when timer is set */
      DalTimetick_GetOffset(hTimerHandle,&timer->sclk_delta);

      /* Insert timer back in active list. */
      timer_insert( current_list, timer );

      expire_timer = timer_determine_timer_expiry(timer, ticks_now);

      memscpy(&timer_temp_internal,sizeof(timer_temp_internal),timer,sizeof(timer_temp_internal)); 

       if ( NULL != timer->timer_ext )
       {
        timer->timer_ext->timer_state = TIMER_SET_FLAG; 
        /** Debug CR422642 */
        if (timer->timer_ext->timer_state == TIMER_SET_FLAG && 
            timer->timer_ext->timer_handle == TIMER_BUFFER_INVALID_HANDLE )
        {
          ERR_FATAL("Debug CR422642: Intentional Crash for debugging this issue",0,0,0);
        }
       }


      /* We've finished manipulating the active timer list.  Unlock
       interrupts for the duration of the timer's expiry processing */
       ATS_INTFREE();

      #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
      timers_expired[timer_expired_cnt].expiry_value =  local_timer_expiry;
      #endif

      /* Expire timer */
      if(expire_timer) 
      {
      	/* Check if the timer belongs to user module */
		if (timer_temp_internal.timer_qapi_proc_hdle != timer_root_handle)
			{
			 timer_remote_expire( &timer_temp_internal, run_time );
			}
		else
			{
			timer_expire( &timer_temp_internal, run_time ); 
			}
      }     

    }
    else
    {

      /* Get location of external timer */
      ext_timer = timer->timer_ext;
      
      if ( ext_timer != NULL )
      {
        ext_timer->timer_handle = TIMER_BUFFER_INVALID_HANDLE ;
      }
      else
      {
        ERR_FATAL ( " Timer must have an external timer, timer_ptr=0x%x timer_ext=0x%x",
                    timer,ext_timer,0);
      }


    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    timers_expired[timer_expired_cnt].expiry_value =  local_timer_expiry;
    #endif

      expire_timer = timer_determine_timer_expiry(timer, ticks_now);

      memscpy(&timer_temp_internal,sizeof(timer_temp_internal),timer,sizeof(timer_temp_internal));

      /* Save index value in a teemp var */
      temp_index = timer->index;

      /* Clear expired timer buffer */
      memset( timer, 0, sizeof( timer_internal_type ));

      /* Restore timer index*/
      timer->index = temp_index ;

      timer->internal_q = TIMER_IN_FREE_Q;
  
      /* Link timer_buffer element back to timer_free_q */
      (void) q_link( timer, 
             &(timer->link) );
    
      /* Enqueue node in timer_free_q */
      q_put( &timer_free_q, &(timer->link) );

      timer_count_buffer_allocated = MAX_TIMER_BUFFER_NUMBER-q_cnt(&timer_free_q);

      /* We've finished manipulating the active timer list.  Unlock
       interrupts for the duration of the timer's expiry processing */
      ATS_INTFREE();

      if(expire_timer) 
      {
      	/* Check if the timer belongs to user module */
		if (timer_temp_internal.timer_qapi_proc_hdle != timer_root_handle)
			{
			 timer_remote_expire( &timer_temp_internal, run_time );
			}
		else
			{
			timer_expire( &timer_temp_internal, run_time ); 
			}
      
      }

    }

    /* NOTE: The above call may run user code, which can do anything ...
       including installing and uninstall other timers.  This can cause
       the value of timers.active.list.zero to change at this point.
       We must use the new value in the next iteration, so any local
       variable which holds computations involving that value must
       be considered invalid after the above call.  */


    /* Lock interrupts in preparation of the next iteration of the loop,
       which tests & manipulates the active timer list */
    ATS_INTLOCK();

    /* Get current slow clock count */
    DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  } /* while timers on timer.active.list are expiring */
  
    /*after processing the p0 active list, switch over to 
     the normal processing list */
    if(current_list == &timers.p0_active.list) 
    {
      current_list = &timers.active.list;
    }
    else 
    {
      current_list = NULL;
    }

  }while(current_list != NULL);


  /* Timers that expire at and before "ticks_now" have been processed.
     Set interrupt for when next timer expires. */

  next_timer_expiry = timer_determine_next_timer_expiry(&head_timer);

    /* The first timer on the active list is the timer next to expire */
  timer_set_next_interrupt( next_timer_expiry,ticks_now, TRUE, 
                            TIMER_MVS_TIMER_PROCESSED, 
                            head_timer );



  /* Timer processing has completed */
  timers.processing = FALSE;

  /* Save a trace packet */
  TIMER_TRACE(PROCESS_END);
  
  // Resetting and checking should happen in a mutex
  if(timer_after_pc_mask[TIMER_PC_MASK_INDEX] == 1)
  {
    timer_reset_check_pc_mask(TIMER_PC_MASK_INDEX);
  }
  
  
  /* We've finished manipulating the active timer list.  */
  ATS_INTFREE();



  #if defined(T_QUASAR) && !defined(T_QUASAR_X)
  /* Because of a hardware problem with QUASar implementation of this
     timer, do read on the timer count until it's not zero...
     The problem is that for some reason, the SLEEP_XTAL_TIMETICK_COUNT
     will read zero for a while (maybe a 10-20 us) if you read it right
     after the ..._VAL is written to.  This is an easy workaround, instead
     having to check for the actual condition of ..._VAL being written
     (_VAL is written in timer_set_next_interrupt above). */
  while ( MSM_R( SLEEP_XTAL_TIMETICK_COUNT ) == 0 )
  {
    /* Do nothing */
  }
  #endif /* T_QUASAR */

} /* timer_process_active_timers */



/*=============================================================================

FUNCTION TIMER_TASK

DESCRIPTION
  Initializes the Timer Task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_task
(
  /* Unused parameter - required to match task init func prototype */
  dword                           ignored    /*lint -esym(715,ignored) */
)
{
  /* Signals returned from wait procedure */
  uint32                   sigs;

  /* Stores possible result from registering ISR if we own the Interrupt */
  int                             eRes;

  /* Install the timer expiry ISR */
  /* was DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER  if we own the interrupt */
  if (timer_fn_map.timer_register_isr)
  {
    eRes = timer_fn_map.timer_register_isr(timers.dal_int_handle,
                                           timer_fn_map.timer_interrupt_id,
                                           (DALISR)timer_task_isr,
                                           NULL,
                                           timer_fn_map.timer_interrupt_flag);
    if ( eRes != DAL_SUCCESS )
    {
      ERR_FATAL("Cannot register ISR",0,0,0);
    }
  }

  /* Signal the task in order to prime the timer processing mechanism */
  qurt_signal_set(&timer_qurt_signal, TIMER_CHANGE_SIG);

  rcinit_handshake_startup();

  for(;;)
  {
    /* Wait for some task signal */
    sigs = qurt_signal_wait(&timer_qurt_signal, TIMER_PROCESS_SIGS, QURT_SIGNAL_ATTR_WAIT_ANY);

    if ( sigs & TIMER_EXPIRY_SIG )
    {
      /* Clear the timer expiry signal */
	  qurt_signal_clear(&timer_qurt_signal, TIMER_EXPIRY_SIG);

      /* Save a trace packet */
      TIMER_TRACE(ISR);

	  /*record the time at which the qurt signal came*/
      timer_update_isr_time();
    }
	else if ( sigs & TIMER_CHANGE_SIG )
	{
      /* Clear the timer expiry signal */
	  qurt_signal_clear(&timer_qurt_signal, TIMER_CHANGE_SIG); 
	}

    /* Process expiring timer(s) */
    timer_process_active_timers();

  }
} /* timer_task */


/* If FEATURE_TIMER_TASK is defined, then the time tick interrupt is
** serviced in the task context.  The timer task should be started
** before enabling the interrupt so that the task is running when an
** interrupt occurs.
** */

void timer_task_init(void) /* Time services */
{
  RCINIT_INFO handle = rcinit_lookup("timer");

  if (RCINIT_NULL != handle)
  {
    rcinit_initfn_spawn_task(handle, timer_task);
  }
}

/*=============================================================================
FUNCTION TIMER_SET_QAPI_NOTIFICATION_HANDLER

Sets the func that needs to be called when a QAPI user module timer expires.
We will provide to this callback function, the timer callback details.
=============================================================================*/
void timer_set_qapi_notification_handler(timer_qapi_notification_handler_type notification_func)
{
   timer_qapi_module_notification_handler = notification_func;
} /* timer_drv_set_remote_process_cb_handler */


/*=============================================================================

FUNCTION TIMER_INIT                                                    REGIONAL

DESCRIPTION
  Initializes the 32kHz slow clock based Timer Subsytem

DEPENDENCIES
  Must only be called from time_init()

RETURN VALUE
  None

SIDE EFFECTS
  May install an ISR for the 32-bit slow clock match register

=============================================================================*/

void timer_init( void )
{
  /* Current slow clock count */
  time_timetick_type              ticks_now = 0;

  /* Loop index for timer bin initialization */
  int                             i;

  /* Timer bin */
  timer_bin_ptr                   bin;

  /* Return value of osal_mutex_init */
  int return_val;

  #ifndef FEATURE_QDSP6
  /* Dal Result */
  DALResult                       eRes;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  eRes = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &timers.dal_int_handle);
  if( eRes != DAL_SUCCESS || timers.dal_int_handle == NULL )
  {
    /* Error out, we should not start up timer task if we cannot set interrupts through DAL */
    ERR_FATAL("timer_init: Could not obtain a Dal Interrupt Handle",0,0,0);
  }
  #endif /*FEATURE_QDSP6*/

  /* Initialize the timer mutex objects */
  timer_mutex_init();
  timer_trace_mutex_init();
  timer_pc_mutex_init();

  /* initialize the timer signal object */
  qurt_signal_init(&timer_qurt_signal);

  timer_fn_map_initialize_fp(&timer_fn_map);

  /*sequence dependency - slave cmd queues have to be initialized 
   before timer_init*/
  #ifdef FEATURE_TIMER_SLAVE1
  /*the timer slave1 command queue has to be initializer here, before 
   starting the timer task*/
  timer_slave_cmd_queue_init();
  #endif

  /*initialize the signal that will be used for signalling expiry of
   the smallest timer when match value == timetick value*/
  timer_get_timetick_handle();

  #ifdef FEATURE_QDSP6
  /* Use function map here */
  timer_init_qurt_signal();
  #endif /*FEATURE_QDSP6*/

  /* Get current slow clock count */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_INIT, NULL, ticks_now);

  /* Initialize the timer_bin_pool and free list */

  for(i = 0; i < TIMER_BIN_NUM; i++)
  {
    bin = &timer_bin_pool[i]; 

    /* init the bin's data */
    bin->first    = NULL;
    bin->last     = NULL;
    bin->lower    = 0;
    bin->upper    = 0;
    bin->count    = 0;

    /* push it on the free list */
    timer_bin_list_push( &timers.bins_free, bin);
  }

  /* Initialize the list of active timers to contain no timers */
  timers.active.list.first = NULL;
  timers.active.list.last  = NULL;
  timers.p0_active.list.first = NULL;
  timers.p0_active.list.last  = NULL;  

  /* Initialize the first non-deferrable timer to NULL */
  timers.first_non_deferrable = NULL;

  /* Initialize timers.set_value, set_time values & last_set_time for tests 
     in set_next_interrupt() and update_timer_interrupt */
  timers.set_value         = ticks_now - 2;
  timers.set_time          = ticks_now - 1;
  timers.last_set_time     = ticks_now - 1;

#ifndef FEATURE_QDSP6

  #ifdef TIMETICK_INT_EDGE_TRIGGERED
  
  /* Setup edge-sensitive interrupt, instead of having the interrupt
     asserted for 30us */
  TIMETICK_INT_EDGE_TRIGGERED_INIT();

  #endif /* TIMETICK_INT_EDGE_TRIGGERED */

#endif

  #ifdef FEATURE_TIMER_SLAVE1
  timer_slave_processing_enabled = TRUE;
  #endif

  /* Initialize the qapi_timer mutex */
  qapi_timer_mutex.name = "MUTEX_QAPI_TIMER";
  return_val = osal_init_mutex(&qapi_timer_mutex);
  ASSERT(OSAL_SUCCESS == return_val);

  /* QAPI timer handler registration will be taken care statically by kernel.
     Moved this from timer_qapi_init to here, as timer_qapi_init is removed.
     It sets the function which is called when a QAPI user module timer expires.*/
  timer_set_qapi_notification_handler(timer_qapi_notification_handler);
} /* timer_init */


/*=============================================================================

FUNCTION TIMER_GROUP_DISABLE

DESCRIPTION
  Disables a timer group.  Timers in the timer group will continue to count
  down, but they will not expire until the group is enabled.

DEPENDENCIES
  Timer group must not be left disabled for more than 2^32 slow clock counts.

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

void timer_group_disable
(
  /* Timer group to be disabled */
  timer_group_ptr                 timer_group
)
{
  /* Pointer to timers, for walking through lists of timers */
  timer_ptr_internal_type          ptr;
  time_timetick_type               now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( timer_group == NULL )
  {
    ASSERT(0);
	//return;
  }


  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);
  TIMER_API_LOG(TIMER_API_LOG_TIMER_GROUP_DISABLE,NULL,now);

  /*if timer_priority0_group, return as disabling of 
   timer_prio0_group is not allowed*/
  if(timer_group == &timer_prio0_group) 
  {
    return;
  }

  /* Disable the timer group, if it is not already disabled */

  if ( !timer_group->disabled )
  {
    /* Mark timer-group as disabled, and record zero-basis */
    timer_group->disabled  = TRUE;

    /* Move given group's timers from active timer list to group timer list */
    ptr = timers.active.list.first;

    while (ptr != NULL)
    {
      /* Remember the next element on the active list, we are going
         to nuke it */
      timer_ptr_internal_type next = ptr->next;

      if ( ptr->group == timer_group )
      {
        /* This is a timer in the given timer group */

        /* if it's binned, remove it from its bin */
        if( ptr->bin != NULL ) 
        {
            timer_bin_remove( ptr );
        }
       
        /* Unlink timer from active list */
        timer_unlink( ptr );
       
        /* Once the timer has been removed from the active
         timer list, we need to decrement the num of
         timers in the active list*/
        /* Decrement the number of timers  */
        timer_active_timers_count--; 
       
        /* Append to group timer list */
        timer_append( &timer_group->list, ptr );
      }

        /* Walk to next timer node */
      ptr = next;

    }

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_GROUP_DISABLED);
  }

  /* Update the first non-deferrable timer in the active list */
  timer_update_first_active_non_deferrable_timer();

  ATS_INTFREE();

} /* timer_group_disable */



/*=============================================================================

FUNCTION TIMER_GROUP_ENABLE

DESCRIPTION
  Enables a timer group.  Timers in the timer group which expired during the
  diabled period will have their expiry processing executed.

DEPENDENCIES
  Timer group must not be left disabled for more than 2^32 slow clock counts.

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

void timer_group_enable
(
  /* Timer group to be enabled */
  timer_group_ptr                 timer_group
)
{
  /* Current slow clock count */
  time_timetick_type                   ticks_now = 0;

  /* Pointer to a timer in the list of timers */
  timer_ptr_internal_type         timer;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (timer_group == NULL)
  {
    ASSERT(0);
	//return;
  }

  ATS_INTLOCK();

  /* Get current slow clock count */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_GROUP_ENABLE,NULL,ticks_now);

  /* Enable the timer group, if it is disabled */

  if ( timer_group->disabled )
  {
    /* Walk through timers in timer group list.
       Stop when we get to a timer after the head of the active list */

    timer = timer_group->list.first;

    while( timer != NULL )
    { 
      if( timer->expiry >= ticks_now )
      {
        break; 
      }
      /* Increment the number of expired timers  */
      timer_active_timers_count++; 
      timer = timer->next;
    }

    /* Break timer list into two parts at this point */
    if( timer != NULL )
    {
      timer_group->list.last = timer->prev;

      if( timer->prev == NULL )
      {
        timer_group->list.first = NULL;
      }
      else
      {
        timer->prev->next = NULL;
      }

      timer->prev = NULL;
    }
        
    /* Expired timers are in list pointed to by "timer_group->list" */
    /* Non-expired timers are in linked list pointed to by "timer" */

    /* Updates timer_num in timer_insert for non-expired timers */
    /* Move non-expired timers into active timer list */
    timer_insert_list( &timers.active.list, timer );

    /* If there are any expired timers ... */
    if ( timer_group->list.first != NULL ) 
    {

      /* Move them to the front of the special expired list */
      timer_concatenate_lists( &timer_group->list, &timers.active.list ); 
 
    }

    /* Mark timer-group as enabled */
    timer_group->disabled = FALSE;

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_GROUP_ENABLED);
  }

  ATS_INTFREE();

} /* timer_group_enable */


/*=============================================================================

FUNCTION TIMER_GROUP_SET_DEFERRABLE

DESCRIPTION
  Set timer group deferrable status.Timer groups default to being non-deferrable.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

void timer_group_set_deferrable( timer_group_ptr pgroup, boolean condition )
{
  time_timetick_type      now = 0;
  /* Set the property of this timer group to indicate whether or not it's 
  deferrable, based on the argument passed in*/
    
  /* We don't want the groups internal to timer-services to be manipulated wrongly */
  if (pgroup == &timer_null_group && condition == FALSE)
  {
    return;
  }
  if (pgroup == &timer_non_defer_group	&& condition == TRUE)
  {
    return;
  }
  
  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_GROUP_SET_DEFERRABLE,NULL,now);
  
  if ( pgroup != NULL )
  {
    pgroup->deferrable = condition;
	
    /* Update the first non-deferrable timer in the active list */
    timer_update_first_active_non_deferrable_timer();	
  }

  ATS_INTFREE();

} /* timer_group_set_deferrable */

/*=============================================================================

FUNCTION TIMER_GET_FIRST_NON_DEFERRABLE_TIMER_EXPIRY

DESCRIPTION
  Gets the first non-deferrable timer's expiry value.

DEPENDENCIES
  None.

RETURN VALUE
  Absolute expiry of first occuring non-deferrable timer.
  Returns TIMER_MAX_EXPIRY in case there is no non-deferrable timer present.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_get_first_non_deferrable_timer_expiry( void )
{
  /* Expiry for the 1st non-deferrable timer */
  time_timetick_type              future_expiry = TIMER_MAX_EXPIRY;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  //TIMER_API_LOG(TIMER_API_LOG_TIMER_GET_FIRST_NON_DEFERRABLE_TIMER_EXPIRY,NULL,ticks_now);

  /* Return the expiry of the first non-deferrable timer */
  if (timers.first_non_deferrable != NULL)
  {
    future_expiry = timers.first_non_deferrable->expiry;
  }
 
  ATS_INTFREE();

  return future_expiry;

} /* timer_get_first_non_deferrable_timer_expiry */


/*=============================================================================

FUNCTION TIMER_GET_TIME_TILL_FIRST_NON_DEFERRABLE_TIMER

DESCRIPTION
  Gets time based on 32kHz clock to the next occurring non-deferrable  
  timer expiry value. 

DEPENDENCIES
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  Number of sclk's until the next timer expiry in a timer group which is 
  non-deferrable.

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timer_get_time_till_first_non_deferrable_timer( void )
{
  return timer_conv_to_sclk_freq(timer_get_time_till_first_non_deferrable_timer_64());
} /* timer_get_time_till_first_non_deferrable_timer */


/*=============================================================================

FUNCTION TIMER_GET_TIME_TILL_FIRST_NON_DEFERRABLE_TIMER_64

DESCRIPTION
  Gets time based on 19MHz clock to the next occurring non-deferrable  
  timer expiry value. 

DEPENDENCIES
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  Number of 19MHz ticks until the next timer expiry in a timer group which is 
  non-deferrable.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_get_time_till_first_non_deferrable_timer_64( void )
{
  /* Current slow clock count */
  time_timetick_type              ticks_now = 0;
  time_timetick_type              future_expiry;
  time_timetick_type              tt_ret;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  /* Get the current time */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_GET_TIME_TILL_FIRST_NON_DEFERRABLE_TIMER,NULL,ticks_now);

  /* Get the expiry of first non-deferrable active timer */
  future_expiry = timer_get_first_non_deferrable_timer_expiry(); 

  /* If future_expiry is in past */
  if (future_expiry  < ticks_now )
  {  
    tt_ret = 0;
  }
  else
  {  
    /* Return the difference between now and the above future expiry */
    tt_ret = (future_expiry - ticks_now);
  }

  ATS_INTFREE();

  return tt_ret;

} /* timer_get_time_till_first_non_deferrable_timer_64 */
     


/*=============================================================================

FUNCTION TIMER_DEFER_MATCH_INTERRUPT

DESCRIPTION
  Sets timer match interrupt to the next occurring non-deferrable timer expiry 
  value. Complimentary function is timer_undefer_match_interrupt(). 

DEPENDENCIES
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  Number of sclk's in 32kHz domain until the next timer expiry in a timer  
  group which is non-deferrable.

SIDE EFFECTS
  Adjusts match value and prevents any subsequent timer calls from updating the
  match value, until timer_undefer_match_interrupt() is called.

=============================================================================*/

timetick_type timer_defer_match_interrupt( void )
{
  return timer_conv_to_sclk_freq(timer_defer_match_interrupt_64());
} /* timer_defer_match_interrupt */


/*=============================================================================

FUNCTION TIMER_DEFER_MATCH_INTERRUPT_64

DESCRIPTION
  Sets timer match interrupt to the next occurring non-deferrable timer expiry 
  value. Complimentary function is timer_undefer_match_interrupt(). 

DEPENDENCIES
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  Number of ticks in 19MHz domain until the next timer expiry in a timer  
  group which is non-deferrable.

SIDE EFFECTS
  Adjusts match value and prevents any subsequent timer calls from updating the
  match value, until timer_undefer_match_interrupt() is called.

=============================================================================*/

time_timetick_type timer_defer_match_interrupt_64( void )
{
  /* Current slow clock count */
  time_timetick_type                   ticks_now = 0;
  time_timetick_type                   future_expiry;
  timer_ptr_type                  head_timer = NULL;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  /* Get the current time */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);
  
  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].ts = ticks_now;
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].reference_count= timer_defer_undefer_reference_count+1;
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].function_called= TIMER_MVS_DEFER_MATCH_INT;
  if (timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].ts > TIMER_MAX_EXPIRY)
  {
    ATS_ERR("Timer detects bad timetick value!");
  }
  timer_defer_undefer_reference_count_log_cnt = (timer_defer_undefer_reference_count_log_cnt+1)%MAX_TIMER_REFERNCE_COUNT_LOG; 
  #endif  

  TIMER_API_LOG(TIMER_API_LOG_TIMER_DEFER_MATCH_INTERRUPT,NULL,ticks_now);

  /* Assume the worst - furthest point in future ~119 years */
  future_expiry = TIMER_MAX_EXPIRY;

  /* Return the expiry of the first non-deferrable timer */
  if (timers.first_non_deferrable != NULL)
  {
    future_expiry = timers.first_non_deferrable->expiry;
    head_timer = timers.first_non_deferrable->timer_ext;
  }

  /* If future_expiry is in past */
  if (future_expiry < ticks_now )
  {
    timer_defer_undefer_reference_count++;
    ATS_INTFREE();
    return 0;
  }
  
  /* Set the timer interrupt to occur at the proper point in the future*/
  timer_set_next_interrupt( future_expiry, ticks_now, FALSE,TIMER_MVS_DEFER_MATCH_INT,head_timer );

  /*increase the reference count here AFTER calling the timer_set_next_interrupt()
   so that the first time around we program the timer interrupt. The decision to bypass 
   programming of the timer interrupt, the next time, is made inside 
   timer_set_next_interrupt() */
  timer_defer_undefer_reference_count++;

  /* From this point on timer interrupt  won’t be adjusted */
  timers.do_reprogram_isr = FALSE;

  ATS_INTFREE();

  /* Return the difference between now and the above future expiry */
  return (future_expiry - ticks_now);

} /* timer_defer_match_interrupt_64 */


/*=============================================================================

FUNCTION TIMER_UNDEFER_MATCH_INTERRUPT

DESCRIPTION
  Set the timer ISR to fire at the earliest expiring timer in the active
  list.  Meant to serve as a the complimentary function to
  timer_defer_match_interrupt()

DEPENDENCIES
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  None

SIDE EFFECTS
  Adjusts the match value.

=============================================================================*/
void timer_undefer_match_interrupt( void )
{
  /* Current slow clock count */
  time_timetick_type                   now = 0;

  /* Time of first expiring timer */
  time_timetick_type              first = TIMER_MAX_EXPIRY;

  timer_ptr_type                  head_timer=NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  timer_undefer_match_interrupt_log.timer_undefer_match_interrupt_now = MAGIC_INITIALIZER_64;
  timer_undefer_match_interrupt_log.timer_undefer_match_interrupt_first = MAGIC_INITIALIZER_64;

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_UNDEFER_MATCH_INTERRUPT,NULL,now);

  timer_defer_undefer_reference_count--;

  /*timer_defer_undefer_reference_count is never suppose to go below 0 */
  if(timer_defer_undefer_reference_count < 0 ) 
  {
    ERR_FATAL("timer_defer and undefer calls do not match, ref_cnt=0x%x",
              timer_defer_undefer_reference_count,0,0);
  }

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].ts = now;
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].reference_count= timer_defer_undefer_reference_count;
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].function_called= TIMER_MVS_UNDEFER_MATCH_INT;
  timer_defer_undefer_reference_count_log_cnt = (timer_defer_undefer_reference_count_log_cnt+1)%MAX_TIMER_REFERNCE_COUNT_LOG;
  #endif

  /*we can return safely here since we will not be programming 
   the timer interrupt anyways.*/
  if(timer_defer_undefer_reference_count > 0)
  {
    ATS_INTFREE();
    return;
  }

  timer_undefer_match_interrupt_log.timer_undefer_match_interrupt_now = MAGIC_INITIALIZER_64;
  timer_undefer_match_interrupt_log.timer_undefer_match_interrupt_first = MAGIC_INITIALIZER_64;
  timer_undefer_match_interrupt_log.timer_undefer_match_interrupt_now = now;
 
  first = timer_determine_next_timer_expiry(&head_timer);

  /* If the first expiring timer is in the past ... */
  if ( first < now  )
  {
    /* Set the timer for "as soon as possible" (eg, "now") */
    first = now;
  }
   
  timer_undefer_match_interrupt_log.timer_undefer_match_interrupt_first = first;
  
  //This is a dummy statement to remove compiler warning.
  timer_undefer_match_interrupt_log.timer_undefer_match_interrupt_first = \
           timer_undefer_match_interrupt_log.timer_undefer_match_interrupt_first;
   
  /* Set the next interrupt match value. The function is called after power 
     collapse and the hardware might not contain right values . Hence force 
     MATCH_VAL register to be updated. */
  timer_set_next_interrupt( first, now, TRUE,TIMER_MVS_UNDEFER_MATCH_INT, head_timer );

  /* Allow timer isr programming again */
  timers.do_reprogram_isr = TRUE;

  timer_after_pc_mask[TIMER_PC_MASK_INDEX]=1;
  // SET THE CPU TO MAX USING NPA CLIENT
  if( timer_fn_map.timer_npa_clk_client )
  { /* client/resource exists so use can make MAX requests to the clock */
    timer_fn_map.timer_npa_issue_suppressible_request( timer_fn_map.timer_npa_clk_client,
                                                       timer_fn_map.timer_npa_max_state );
    timer_cnt_cpu_max++;
  }

  ATS_INTFREE();

  } /* timer_undefer_match_interrupt */


/*=============================================================================

FUNCTION TIMER_RESTART

DESCRIPTION
 This function is called after waking up. The timetick counter values 
 are readjusted to sync up with the base timetick counter when waking up from
 power collapse. The match value has to be adjusted accordingly.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Adjusts the match value


=============================================================================*/

void timer_restart(void)
{
  time_timetick_type                now = 0;

  DalTimetick_GetTimetick64(hTimerHandle, &now);
  TIMER_API_LOG(TIMER_API_LOG_TIMER_RESTART,NULL,now);
  /* Calling timer_undefer_match_interrupt */
  timer_undefer_match_interrupt();
}

/*=============================================================================

FUNCTION TIMER_DEFINE

DESCRIPTION
  Defines a timer structure, initializing members to NULL, and associates the
  timer with the given group.

DEPENDENCIES
  Timer must not already be defined and on a timer list (active).

RETURN VALUE
  None

SIDE EFFECTS
  May link timer group into linked list of timer groups

=============================================================================*/

static timer_error_type timer_define
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group
)
{
  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;
  
  if (timer == NULL){
    ASSERT(0);
	//return TE_INVALID_PARAMETERS;
	}

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*if the timer is of priority 0, then it needs to be 
   associated with a static timer group that will be 
   guaranteed to be non-deferrable*/
  if(TIMER_PRIORITY_0 == timer->timer_priority) 
  {
    group = &timer_prio0_group;
  }
  else
  {
    /* Reparent timers in the NULL group to be in the timer_null_group */
    if ( group == NULL )
    {
      group = &timer_null_group;
    }
  }



  /* If this timer group is not yet in the linked list of timer groups,
     add it to the list. Also check for corruption and err fatal.*/
  if(group->next)
  {
    /* Checking for corruption */
    if((group->next->disabled != TRUE && group->next->disabled != FALSE) || (group->next->deferrable != TRUE && group->next->deferrable != FALSE))
      ATS_ERR_FATAL("Corrupted value of group and timer: group = %x, timer = %x",group,timer,0);
  }
  else
  {
    group->next = timers.active.next;
    timers.active.next = group;
  }


#define TIMER_CHECK_FOR_DEFINE_OF_ACTIVE_TIMER
#ifdef TIMER_CHECK_FOR_DEFINE_OF_ACTIVE_TIMER

  /* Check if timer is active (already on the active timer list, or a
   deactived timer group list).  If it is, we need to remove it,
   or when the structure is re-initialized, the timer linked lists
   will be corrupted. */

  timer_int = timer_get_internal_timer(timer);

  /*If the timer is found in timer_buffer and the timer state
   is PAUSED, then return with an error message that we cannot
   define a paused timer*/
  if(timer_int 
     && TIMER_PAUSED_FLAG == timer->timer_state) 
  {
     ATS_ERR_3("timer_def(%x,%x) called for paused timer (list %x) call clear or resume",
            timer, group, timer_int->list);
     return TE_INVALID_STATE_FOR_OP;;
  }
  
  if ( timer_int != NULL && timer_int->list != NULL 
       && timer_int->internal_q == TIMER_NOT_IN_FREE_Q 
       && timer_int->group == timer->group
       && timer_int->sigs  == timer->sigs
       && timer_int->func1 == timer->func1
       && timer_int->func2 == timer->func2
       && timer_int->notify_obj_ptr == timer->sigs_obj)
  {
    /* Timer might be active ... or it might be declared as a stack
     variable, and contain garbage values in all of the fields.
     Search for the list it claims to be a member of in the
     linked list of timer groups. */

    timer_group_ptr  grp = &timers.active;

    do
    {
      /* Is the timer a member of the "grp" group? */
      if ( timer_int->list == &grp->list )
      {
         /* Yes - timer claims to be a member of an existing timer group */

        ERR("timer_def(%x,%x) called for active timer (list %x)",
            timer, group, timer_int->list);

        /* Find timer in the groups's list */
        if( timer_in_list( timer_int, &grp->list ) )
        {
          /* Deactivate timer before initializing timer structure */          
          ERR_FATAL("timer_def(%x,%x) called for active timer (list %x)",
            timer, group, timer_int->list);
        }
      }

      /* Check next group */
      grp = grp->next;

    } while ( grp!= NULL && grp != &timers.active );

  }
 
  timer->timer_handle    = TIMER_BUFFER_INVALID_HANDLE;

#endif /* TIMER_CHECK_FOR_DEFINE_OF_ACTIVE_TIMER */


  /* Initialize timer structure */
  timer->group = group;

   /* For a priority timer the slave task is not assigned */
   if ( timer->timer_priority == TIMER_PRIORITY_0 )
   {
	timer->slave_task_assignment = 0xFF;	 
   }
   #ifdef FEATURE_TIMER_SLAVE1
   else
   {
	/*assign the slave task here*/
	timer->slave_task_assignment = timer_assign_slave_task_to_timer();
   }
   #endif


   timer->sigs_obj    = (time_osal_notify_obj_ptr)NULL;
   timer->sigs		  = (time_osal_notify_data)0;
   timer->func1 	  = (timer_t1_cb_type)NULL;
   timer->func2 	  = (timer_t2_cb_type)NULL;
   timer->data		  = (timer_cb_data_type)0;
   timer->notify_type = TIMER_NO_NOTIFY_TYPE; 
   timer->last_expiry = 0;
   timer->last_start  = 0;
   timer->timer_state = TIMER_DEF_FLAG;
   
   if(timer->timer_qapi_proc_id != TIMERS_QAPI_FROM_USER)
     {  
       timer->timer_qapi_proc_id   = TIMERS_QAPI_FROM_ROOT;
	   timer->timer_qapi_proc_hdle = timer_root_handle;
     }
	 
   return TE_SUCCESS;

} /* timer_define */




/*=============================================================================

FUNCTION TIMER_INIT_BUFFER_QUEUE

DESCRIPTION
  Iniitialize buffer queue where free timer buffers are kept. 

DEPENDENCIES
  Must be called from timer_def or timer_def2 before the first timer is 
  defined. It is not called from timer_init as some timers are defined 
  before call to timer_init.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_init_buffer_queue(void)
{
  /* local Iterator */
  uint32 i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Initialize the timer_free_q */
  q_init( &timer_free_q );
  
  for ( i = 0 ; i < MAX_TIMER_BUFFER_NUMBER ; i++ )
  {
    timer_buffer[i].internal_q = TIMER_IN_FREE_Q;
    timer_buffer[i].index = i;
  
    /* Link the handles to the timer_free_q */
    q_link( &timer_buffer[i], &timer_buffer[i].link );
    
    /* Enqueue handles */
    q_put( &timer_free_q, &timer_buffer[i].link );

  }
}

static inline boolean timer_is_notify_type_valid(timer_notify_type notify_type)
{
  if(notify_type != TIMER_REX_TCB_SIGNAL_TYPE &&
   	 notify_type != TIMER_NATIVE_OS_SIGNAL_TYPE &&
     notify_type != TIMER_NATIVE_OS_SIGNAL2_TYPE &&
   	 notify_type != TIMER_DALSYS_EVENT_SIGNAL_TYPE &&
  	 notify_type != TIMER_FUNC1_CB_TYPE &&
  	 notify_type != TIMER_FUNC2_CB_TYPE &&
  	 notify_type != TIMER_NO_NOTIFY_TYPE)
     return FALSE;
  else
     return TRUE;
} /* timer_is_notify_type_valid */



/*=============================================================================

FUNCTION timer_def_osal

DESCRIPTION
  Defines and initializes a timer. Signal and function notifications are 
  exclusive. If both are provided at the same time, error will be returned.

  The timer expires when:\n
  - sigs is non-NULL. The signals are set to the task given by tcb.
  - func is non-NULL. The callback function with the argument data
    is called from the timer tasks.

DEPENDENCIES
  None

RETURN VALUE
  TE_SUCCESS  Operation completed successfully
  TE_INVALID_PARAMETERS  Passed params are incorrect for this API
  TE_INVALID_STATE_FOR_OP  If the timer is in paused state


SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_def_osal
(
  /* Timer to define */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL = the "NULL" timer group) */
  timer_group_ptr                 group,

  /*ats timer callback type*/
  timer_notify_type               cb_type,

  /* Task to signal and/or task to queue APC call for */
  time_osal_notify_obj_ptr        sigs_func_addr,
  
  /* Task signals to set to the tcb task when timer expires */
  time_osal_notify_data           sigs_mask_data
)
{
  return timer_def_osal_priority( timer, group, cb_type, sigs_func_addr, sigs_mask_data, TIMER_PRIORITY_1);
} /* timer_def_osal */


/*=============================================================================

FUNCTION timer_def_osal_priority

DESCRIPTION
  Defines and initializes a timer. Signal and function notifications are 
  exclusive. If both are provided at the same time, error will be returned.

  The timer expires when:\n
  - sigs is non-NULL. The signals are set to the task given by tcb.
  - func is non-NULL. The callback function with the argument data
    is called from the timer tasks.

DEPENDENCIES
  None

RETURN VALUE
  TE_SUCCESS  Operation completed successfully
  TE_INVALID_PARAMETERS  Passed params are incorrect for this API
  TE_INVALID_STATE_FOR_OP  If the timer is in paused state

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_def_osal_priority
(
  /* Timer to define */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL = the "NULL" timer group) */
  timer_group_ptr                 group,

  /*ats timer callback type*/
  timer_notify_type               notify_type,

  /* Task to signal and/or task to queue APC call for */
  time_osal_notify_obj_ptr        sigs_func_addr,
  
  /* Task signals to set to the tcb task when timer expires */
  time_osal_notify_data           sigs_mask_data,

  /* timer priority information */
  timer_priority_enum_type        timer_prio  
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  time_timetick_type			  now = 0;
  
  timer_error_type status = TE_SUCCESS;
  
  /* Input Validation */
  if(timer == NULL ||
     timer_prio > TIMER_MAX_PRIORITY
	 )
    return TE_INVALID_PARAMETERS;

  if(!timer_is_notify_type_valid(notify_type))
    return TE_INVALID_PARAMETERS;

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);    
  
  TIMER_API_LOG(TIMER_API_LOG_TIMER_DEF_OSAL, timer, now);
  
  if ( sigs_mask_data != 0  &&  sigs_func_addr == NULL )
  {
    /* Todo: Can print msgs for error conditions from timer apis? */
    //ERR_FATAL("Can't set signals to NULL futex. T=%x S=%x", timer, sigs, 0);    
    ATS_INTFREE();
    return TE_INVALID_PARAMETERS;
  }

  /* Initialize the timer buffer if it is not initialized */
  if(!timer_q_init_status)
  {
    timer_init_buffer_queue();
    timer_q_init_status = 1;
  }
  
  /* Put the timer priority information */
  timer->timer_priority = timer_prio;    
  
  /* Initialize new timer structure */
  status = timer_define( timer, group );

  if (status != TE_SUCCESS)
  	{
  	  ATS_INTFREE();
  	  return status;
  	}

  timer->notify_type = notify_type;
  
  /* Assign the below members of the external client timer
     structure only after timer_define has been completed */
  if (notify_type == TIMER_REX_TCB_SIGNAL_TYPE ||
      notify_type == TIMER_NATIVE_OS_SIGNAL_TYPE ||
      notify_type == TIMER_NATIVE_OS_SIGNAL2_TYPE ||
  	  notify_type == TIMER_DALSYS_EVENT_SIGNAL_TYPE)
  	{
  	  timer->sigs_obj      = (time_osal_notify_obj_ptr)sigs_func_addr;
	  timer->sigs          = (time_osal_notify_data)sigs_mask_data;
  	}
  else if (notify_type == TIMER_FUNC1_CB_TYPE)
    {
	  timer->func1		   = (timer_t1_cb_type)sigs_func_addr;
	  timer->data 		   = (timer_cb_data_type)sigs_mask_data;
    }
  else if (notify_type == TIMER_FUNC2_CB_TYPE)
    {
	  timer->func2		   = (timer_t2_cb_type)sigs_func_addr;
	  timer->data 		   = (timer_cb_data_type)sigs_mask_data;
    }	

  ATS_INTFREE();
  return status;
} /* timer_def_osal_priority */

#ifdef FEATURE_TIMER_REX_IS_AVAILABLE

/*=============================================================================

FUNCTION TIMER_DEF

DESCRIPTION
  Defines and initializes a timer.

  When the timer expires:
    if sigs is non-null, those signals are set to the task given by tcb;
    if func is non-null, then
       if tcb is null, then func(data) is called from interrupt context,
       otherwise, func(data) is queued as an Asynchronous Procedure Call (APC)

  Timers may also be staticly declared via the TIMER_DEF() macro

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_def
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group,

  /* Task to signal and/or task to queue APC call for */
  rex_tcb_type                    *tcb,

  /* Task signals to set to the tcb task when timer expires */
  rex_sigs_type                   sigs,

  /* APC (tcb!=NULL) or callback (tcb==NULL) to queue/call at timer expiry */
  timer_t1_cb_type                func,

  /* Arbitrary data for APC or callback */
  timer_cb_data_type              data
)
{  
  /*call the new timer define api with priority 1*/
  timer_def_priority(timer, group, tcb, sigs, func, data, TIMER_PRIORITY_1);
}
/*=============================================================================

FUNCTION TIMER_DEF_PRIORITY
  
DESCRIPTION
  Defines and initializes a timer with specified priority
  When the timer expires:
    if sigs is non-null, those signals are set to the task given by tcb;
    if func is non-null, then
       if tcb is null, then func(data) is called from interrupt context,
       otherwise, func(data) is queued as an Asynchronous Procedure Call (APC)

  Timers may also be staticly declared via the TIMER_DEF() macro

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_def_priority
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group,

  /* Task to signal and/or task to queue APC call for */
  rex_tcb_type                    *tcb,

  /* Task signals to set to the tcb task when timer expires */
  rex_sigs_type                   sigs,

  /* APC (tcb!=NULL) or callback (tcb==NULL) to queue/call at timer expiry */
  timer_t1_cb_type                func,

  /* Arbitrary data for APC or callback */
  timer_cb_data_type              data,

  timer_priority_enum_type        timer_prio
)
{
  
  timer_error_type			      status;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  time_timetick_type              now = 0;

  if (timer == NULL)
  	{
  	  ASSERT(0);
	  //return;
  	}  

  if(timer_prio > TIMER_PRIORITY_1)
  	{
	return;
	}

  if ( sigs != 0  &&  tcb == NULL )
  {
    ERR_FATAL("Can't set signals to NULL task. T=%x S=%x", timer, sigs, 0);
  }

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);
  
  TIMER_API_LOG(TIMER_API_LOG_TIMER_DEF,timer,now);

  if(qurt_thread_get_id() == timer_thread_id){
    ATS_ERR("Timer def called from Timer context.");
  }

  /* Initialize the timer buffer if it is not initialized */
  if(!timer_q_init_status)
  {
    timer_init_buffer_queue();
    timer_q_init_status = 1;
  }

  /* Put the timer priority information */
  timer->timer_priority = timer_prio;    

  /* Initialize new timer structure */
  status = timer_define( timer, group );

  if (status != TE_SUCCESS)
  	{
  	 ATS_INTFREE();
  	 return;
  	}  

  /* Copy data to timer structure */
  timer->sigs_obj    = (time_osal_notify_obj_ptr)tcb;
  timer->sigs        = sigs;
  timer->func1       = func;
  timer->func2       = NULL;
  timer->data        = data;
  timer->notify_type = TIMER_NO_NOTIFY_TYPE;

  /* Update the notification type */
  if(sigs!=0)
  	{
  	 timer->notify_type |= TIMER_REX_TCB_SIGNAL_TYPE;
  	}

  if(func!=NULL)
  	{
  	 timer->notify_type |= TIMER_FUNC1_CB_TYPE;
  	}


  ATS_INTFREE();

  return;

} /* timer_def */



/*=============================================================================

FUNCTION TIMER_DEF2

DESCRIPTION
  Defines a 'clock callback' style timer.

  When the timer expires:
    if func(time_ms, data) is called from interrupt context,

  Timers may also be staticly declared via the TIMER_DEF2() macro

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_def2
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group
)
{
  timer_def2_priority(timer, group, TIMER_PRIORITY_1);
}

/*=============================================================================

FUNCTION TIMER_DEF2_PRIORITY

DESCRIPTION
  Defines a 'clock callback' style timer and with an ability to set
  the timer priority.

  When the timer expires:
    if func(time_ms, data) is called from interrupt context,

  Timers may also be staticly declared via the TIMER_DEF2() macro

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_def2_priority
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group,

  timer_priority_enum_type        timer_prio
)
{
  time_timetick_type              now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (timer == NULL)
  	{
  	  ASSERT(0);
      //return;
  	}

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_DEF2,timer,now);

  /* Initialize the timer buffer if it is not initialized */
  if( !timer_q_init_status )
  {
    timer_init_buffer_queue();
    timer_q_init_status = 1;
  }

  timer->timer_priority = timer_prio;

  /* Initialize new timer structure */
  (void)timer_define( timer, group );

  ATS_INTFREE();

  /* timer_reg() will fill in the timer members "func2" and "data" */


} /* timer_def2 */



/*=============================================================================

FUNCTION TIMER_REG

DESCRIPTION
  Initializes a 'clock callback' style timer.

  When the timer expires:
    if func(time_ms, data) is called from interrupt context,

DEPENDENCIES
  Timer must have been defined using timer_def2() or TIMER_DEF2()

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_reg
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Function to call at timer expiry */
  timer_t2_cb_type                func,

  /* Arbitrary data for func(time_ms, data) */
  timer_cb_data_type              data,

  /* Time (in ms) until first timer expiry */
  timetick_type                   time_ms,

  /* Period (in ms) between repeated expiries (0 = not periodic) */
  timetick_type                   reload_ms
)
{
  time_timetick_type              now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( timer == NULL )
  {
    ASSERT(0);
	//return;
  }

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_REG,timer,now);

  /* Copy function & function data to timer structure */
  timer->func2       = func;
  timer->func1       = NULL;
  timer->data        = data;
  timer->sigs_obj    = NULL;
  timer->sigs        = 0;
  timer->notify_type = TIMER_FUNC2_CB_TYPE;

  /* Start the timer */
  timer_set(timer, time_ms, reload_ms, T_MSEC );

} /* timer_reg */
#endif /* FEATURE_TIMER_REX_IS_AVAILABLE */


/*=============================================================================

FUNCTION TIMER_SET

DESCRIPTION
  Sets an inactive timer to expire after a given period of time, or changes
  an active timer to expire after a given period of time.

  Optionally, specifies the timer to repeatly expire with a given period.
  If units are ticks, then ticks are supplied in 32kHz domain.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

void timer_set
(
  timer_ptr_type                  timer,
  timetick_type                   time,
  timetick_type                   reload,
  timer_unit_type                 unit
)
{
  /* If the unit type is in legacy ticks, we will convert to new timetick */
  if( unit == T_TICK || unit == T_NONE )
  {
    (void)timer_set_64(timer,
                 timer_conv_to_xo_freq(time),
                 timer_conv_to_xo_freq(reload),
                 unit);
  }
  else
  {
    (void)timer_set_64(timer,
                 time,
                 reload,
                 unit);
  }
  return;
} /* timer_set */


/*=============================================================================

FUNCTION TIMER_SET_GET_CMD_BUF

DESCRIPTION
  Gets a command buffer for a free timer and fills it out with timer info.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static inline timer_error_type timer_set_get_cmd_buf
(
  timer_ptr_type                  timer,
  timer_ptr_internal_type         *free_timer_in
)
{
#ifdef FEATURE_TIMER_DETECT_MEM_LEAKS 
  uint32 iter = 0;
#endif /*FEATURE_TIMER_DETECT_MEM_LEAKS*/
  int32  temp_index;
  timer_ptr_internal_type free_timer = NULL;
  free_timer = timer_get_internal_timer( timer );

  if(free_timer 
     && TIMER_PAUSED_FLAG == timer->timer_state) 
  {
    ATS_ERR_1("Timer 0x%x is a PAUSED timer that will reset",timer);
    timer_remove_internal_timer_from_q(free_timer);
    timer_count_paused_timers--;
  }

  /* Remove timer from timer list, if any */
  if ( free_timer != NULL ) 
  {
    if ( free_timer->internal_q == TIMER_NOT_IN_FREE_Q )
    {
      timer_remove(free_timer, TRUE);
    }
  }

#ifdef FEATURE_TIMER_DETECT_MEM_LEAKS 

  /*iterate through timer_buffer to see if we have an active timer
   with an external memory and different handle. This is an error*/
  for(iter = 0; iter < MAX_TIMER_BUFFER_NUMBER ; iter++ ) 
  {
    if(timer_buffer[iter].timer_ext == timer 
       && timer_buffer[iter].internal_q == TIMER_NOT_IN_FREE_Q
      ) 
    {
      ERR_FATAL("Trying to set the same timer with different handle mem=0x%x, handle=0x%x",timer, iter+1,0);
    }
  }

#endif


  /* Get a free timer from Queue */
  free_timer = (timer_ptr_internal_type) q_get( &timer_free_q );

  if(free_timer == NULL)
	 {
	  ERR_FATAL("Free timer depleted",0,0,0);
	  return TE_INVALID_PARAMETERS;
	 }	

  /*increase the count of timers in timer_buffer*/
  timer_count_buffer_allocated = MAX_TIMER_BUFFER_NUMBER-q_cnt(&timer_free_q);

  temp_index = free_timer->index; 
  timer->timer_handle = INDEX_TO_HANDLE( temp_index ); 
  timer->timer_state = TIMER_SET_FLAG;

  memset( free_timer, 0, sizeof( timer_internal_type ));

  free_timer->index = temp_index;

  free_timer->internal_q = TIMER_NOT_IN_FREE_Q;  
  /* Most fields are 0's or NULL's */

  if ( timer->group == NULL )
  {   
    timer->group = &timer_null_group;
  }

  /* Update the thread ID for the caller */
  free_timer->thread_id = TIME_OSAL_GET_THREAD_ID();

  /* Copy data to Internal timer structure */
  free_timer->group               = timer->group;
  free_timer->slave_task_assigned = timer->slave_task_assignment;
  free_timer->timer_priority      = timer->timer_priority;
  free_timer->notify_type         = timer->notify_type;
  free_timer->notify_obj_ptr      = timer->sigs_obj;
  free_timer->sigs                = timer->sigs;
  free_timer->func1               = timer->func1;
  free_timer->func2               = timer->func2;
  free_timer->data                = timer->data;
  free_timer->timer_qapi_proc_hdle  = timer->timer_qapi_proc_hdle;
  free_timer->timer_qapi_proc_id  = timer->timer_qapi_proc_id;  
  free_timer->timer_ext           = timer; 

  *free_timer_in = free_timer;

  return TE_SUCCESS;

} /*timer_set_get_cmd_buf*/


/*=============================================================================

FUNCTION TIMER_SET_64

DESCRIPTION
  Sets an inactive timer to expire after a given period of time, or changes
  an active timer to expire after a given period of time.

  Optionally, specifies the timer to repeatly expire with a given period.
  If units are ticks, then ticks are supplied in 19MHz domain.

DEPENDENCIES
  None

RETURN VALUE
  TE_SUCCESS always currently

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

timer_error_type timer_set_64
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Time (in units below) until first timer expiry */
  time_timetick_type                   time,

  /* Period (in units) between repeated expiries (0 = not periodic) */
  time_timetick_type                   reload,

  /* Unit to measure "ticks" in. */
  timer_unit_type                 unit
)
{
   /* Local internal timer ptr */
   timer_ptr_internal_type       free_timer;

   /* Temp Index */
   time_timetick_type           now = 0;
   timer_error_type             status;

   #ifdef FEATURE_TIMER_DETECT_MEM_LEAKS
   int                           iter=0;
   #endif /*FEATURE_TIMER_DETECT_MEM_LEAKS*/

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  

  if (timer == NULL)
  {
    ASSERT(0);
    //return TE_INVALID_PARAMETERS;
  }

  if ( timer->group == NULL )
  {
    char qurt_task_name[12];
    qurt_thread_get_name(&qurt_task_name[0], sizeof(qurt_task_name));
    ATS_MSG_SPRINTF_2("[timer_set] : timer_def was not called for timer %x, task %s",
	                  timer,qurt_task_name);   
  }

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);
  TIMER_API_LOG(TIMER_API_LOG_TIMER_SET,timer,now);

  status = timer_set_get_cmd_buf(timer,&free_timer);
  if (status != TE_SUCCESS)
  	{
      ATS_INTFREE();
	  return status;
  	}
  
  /** Debug CR422642 */
  if (timer->timer_state == TIMER_SET_FLAG && 
      timer->timer_handle == TIMER_BUFFER_INVALID_HANDLE )
  {
    ERR_FATAL("Debug CR422642: Intentional Crash for debugging this issue",0,0,0);
  }

  /* Defensive programming. Clearing the signal that will
   be set when the timer expires.*/
  if ( free_timer->notify_type & TIMER_NATIVE_OS_SIGNAL_TYPE )
	{
		qurt_signal_clear( (qurt_signal_t *)free_timer->notify_obj_ptr, (time_osal_notify_data)free_timer->sigs );
	}
  else if ( free_timer->notify_type & TIMER_REX_TCB_SIGNAL_TYPE )
  	{
    #ifdef FEATURE_TIMER_REX_IS_AVAILABLE
       (void) rex_clr_sigs( free_timer->notify_obj_ptr, free_timer->sigs );
    #else
	   ERR_FATAL("Signal cannot be of rex type. timer: 0x%x", timer, 0, 0);
    #endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
  	}
  else if ( free_timer->notify_type & TIMER_NATIVE_OS_SIGNAL2_TYPE )
	{
	#ifdef FEATURE_TIME_QDSP6
		  qurt_signal2_clear( (qurt_signal2_t *)free_timer->notify_obj_ptr, (time_osal_notify_data)free_timer->sigs );
	#else
          ERR_FATAL("Signal cannot be of NATIVE OS type. timer: 0x%x", timer, 0, 0);
	#endif /* FEATURE_TIME_QDSP6 */
	}
  else if (free_timer->notify_type & TIMER_DALSYS_EVENT_SIGNAL_TYPE)
    {
        timer_dalsys_sig_reset(free_timer->notify_obj_ptr);
    }

  /* ... convert given duration into slow clocks, and save in cache */
  free_timer->cache.duration      = time;
  DalTimetick_CvtToTimetick64(hTimerHandle,time,unit,&free_timer->cache.duration_sclk);

  /* Retrieve converted time duration from cache */
  time = free_timer->cache.duration_sclk;

      /* Convert and cache given reload interval into slow clocks */
  free_timer->cache.reload      = reload;
  DalTimetick_CvtToTimetick64(hTimerHandle,reload,unit,&free_timer->cache.reload_sclk);

    /* Retrieve converted reload interval from cache */
  reload = free_timer->cache.reload_sclk;

  /* Ensure cached unit value is correct */
  free_timer->cache.unit = unit;

  /* Determine when timer should expire, and set reload */

  DalTimetick_GetTimetick64(hTimerHandle, &free_timer->start);
  free_timer->expiry = free_timer->start + time;
  free_timer->reload = reload;
  DalTimetick_GetOffset(hTimerHandle,&free_timer->sclk_delta);

  /* Insert timer in either active timer list, or group timer list if it
     belongs to a disabled timer group */

  if ( free_timer->group->disabled )
  {
    /* Timer belongs to a disabled group - add it to that group's list */
    timer_insert( &free_timer->group->list, free_timer );
  }
  else
  {
    if(TIMER_PRIORITY_0 == free_timer->timer_priority) 
    {
      /* Timer is not a member of a disabled group - add to active list */
      timer_insert( &timers.p0_active.list, free_timer);
    }
    else
    {
    /* Timer is not a member of a disabled group - add to active list */
    timer_insert( &timers.active.list, free_timer );
    }

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_TIMER_SET);
  }

  if ( timer->timer_handle == TIMER_BUFFER_INVALID_HANDLE )
    ERR_FATAL (" Wrong timer buffer handle, timer_ptr=0x%x" , timer,0,0);

  /** Debug CR422642 */
  if (timer->timer_state == TIMER_SET_FLAG && 
      timer->timer_handle == TIMER_BUFFER_INVALID_HANDLE )
  {
    ERR_FATAL("Debug CR422642: Intentional Crash for debugging this issue",0,0,0);
  }

  ATS_INTFREE();

  return TE_SUCCESS;
} /* timer_set_64 */
 

/*=============================================================================

FUNCTION TIMER_SET_ABSOLUTE

DESCRIPTION
  Sets an inactive timer to expire at a given time, or changes
  an active timer to expire at a given time.

DEPENDENCIES
  None

RETURN VALUE
  TE_SUCCESS always currently

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

timer_error_type timer_set_absolute
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Time (in XOs) until first timer expiry */
  time_timetick_type              time

)
{
  /* Local internal timer ptr */
  timer_ptr_internal_type       free_timer = NULL;
  time_timetick_type            preintlock_val,postintlock_val;

  timer_error_type             status;

  #ifdef FEATURE_TIMER_DETECT_MEM_LEAKS
  int                           iter=0;
  #endif /*FEATURE_TIMER_DETECT_MEM_LEAKS*/

 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  if (timer == NULL)
  {
    ASSERT(0);
    //return TE_INVALID_PARAMETERS;
  }

 if ( timer->group == NULL )
 {
   char qurt_task_name[12];
   qurt_thread_get_name(&qurt_task_name[0], sizeof(qurt_task_name));
   ATS_MSG_SPRINTF_2("[timer_set_absolute] : timer_def was not called for timer %x, task %s",
					 timer,qurt_task_name);   
 }

 /* Specific to set_hard function... check to see if we pass expiry when acq lock */
 DalTimetick_GetTimetick64(hTimerHandle, &preintlock_val);

 ATS_INTLOCK();

 DalTimetick_GetTimetick64(hTimerHandle, &postintlock_val);

 TIMER_API_LOG(TIMER_API_LOG_TIMER_SET,timer,postintlock_val);

 status = timer_set_get_cmd_buf(timer,&free_timer);
 if (status != TE_SUCCESS)
  {
	ATS_INTFREE();
	return status;
  }

  /* Defensive programming. Clearing the signal that will
   be set when the timer expires.*/
  if ( free_timer->notify_type & TIMER_NATIVE_OS_SIGNAL_TYPE )
	{
		qurt_signal_clear( (qurt_signal_t *)free_timer->notify_obj_ptr, (time_osal_notify_data)free_timer->sigs );
	}
  else if ( free_timer->notify_type & TIMER_REX_TCB_SIGNAL_TYPE )
  	{
  #ifdef FEATURE_TIMER_REX_IS_AVAILABLE
       (void) rex_clr_sigs( free_timer->notify_obj_ptr, free_timer->sigs );
  #else
	   ERR_FATAL("Signal cannot be of rex type. timer: 0x%x", timer, 0, 0);
  #endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
  	}
  else if ( free_timer->notify_type & TIMER_NATIVE_OS_SIGNAL2_TYPE )
	{
  #ifdef FEATURE_TIME_QDSP6
		  qurt_signal2_clear( (qurt_signal2_t *)free_timer->notify_obj_ptr, (time_osal_notify_data)free_timer->sigs );
  #else
          ERR_FATAL("Signal cannot be of NATIVE OS type. timer: 0x%x", timer, 0, 0);
  #endif /* FEATURE_TIME_QDSP6 */
	}
  else if (free_timer->notify_type & TIMER_DALSYS_EVENT_SIGNAL_TYPE)
    {
        timer_dalsys_sig_reset(free_timer->notify_obj_ptr);
    }  
  
  if( postintlock_val - preintlock_val  > time - preintlock_val )
  {
    /* We will want to expire this timer right away */
    /* ... convert given duration into slow clocks, and save in cache */
    free_timer->cache.duration      = 0;
    free_timer->cache.duration_sclk = 0;
    
    /* No reload allowed for this */
    free_timer->cache.reload      = 0;
    free_timer->cache.reload_sclk = 0;
    
    /* Ensure cached unit value is correct */
    free_timer->cache.unit = T_TICK;
    
    /* Determine when timer should expire, and set reload */
    
    free_timer->start  = postintlock_val;
    free_timer->expiry = postintlock_val;
    free_timer->reload = 0;
    DalTimetick_GetOffset(hTimerHandle,&free_timer->sclk_delta);
  }
  else
  {  
   /* ... convert given duration into slow clocks, and save in cache */
   free_timer->cache.duration      = time - postintlock_val;
   free_timer->cache.duration_sclk = free_timer->cache.duration;
  
       /* No reload allowed for this */
   free_timer->cache.reload      = 0;
   free_timer->cache.reload_sclk = 0; /* Is this okay? */
  
   /* Ensure cached unit value is correct */
   free_timer->cache.unit = T_TICK;
  
   /* Determine when timer should expire, and set reload */
  
   free_timer->start  = postintlock_val;
   free_timer->expiry = time;
   free_timer->reload = 0;
   DalTimetick_GetOffset(hTimerHandle,&free_timer->sclk_delta);
  }
  
    /* Insert timer in either active timer list, or group timer list if it
     belongs to a disabled timer group */
  
  if ( free_timer->group->disabled )
  {
    /* Timer belongs to a disabled group - add it to that group's list */
    timer_insert( &free_timer->group->list, free_timer );
  }
  else
  {
    if(TIMER_PRIORITY_0 == free_timer->timer_priority) 
    {
      /* Timer is not a member of a disabled group - add to active list */
      timer_insert( &timers.p0_active.list, free_timer);
    }
    else
    {
      /* Timer is not a member of a disabled group - add to active list */
      timer_insert( &timers.active.list, free_timer );
    }
  
    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_TIMER_SET);
  }
  
  if ( timer->timer_handle == TIMER_BUFFER_INVALID_HANDLE )
    ERR_FATAL (" Wrong timer buffer handle, timer_ptr=0x%x" , timer,0,0);

  ATS_INTFREE();
 
  return TE_SUCCESS;
}

/*=============================================================================

FUNCTION TIMER_EXPIRES_IN

DESCRIPTION
  Get number of ticks before timer expires

DEPENDENCIES
  Timer must be active (on a timer list)
  Internal function, called from ATS_INTLOCK context.

RETURN VALUE
  Number of ticks before timer expires.
  Zero is returned for inactive timers, and timers that have expired or
  are expiring.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_expires_in
(
  /* Timer to get */
  timer_ptr_internal_type         timer
)
{
  /* Current slow clock tick count */
  time_timetick_type                   now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current slow clock count value */

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  /* Determine if the timer has expired or not */

  if ( timer->expiry >=  now  )
  {
    /* Timer has not expired - compute time till expiry. */

    return ( timer->expiry - now );
  }
  else
  {
    /* Timer has expired - return zero */

    return 0;
  }

} /* timer_expires_in */



/*=============================================================================

FUNCTION TIMER_GET_DURATION

DESCRIPTION
  Get the timer duration for which it is set to

DEPENDENCIES
  None

RETURN VALUE
  Number of ticks in the passed unit.
  Zero is returned for inactive timers, and timers that have expired or
  are expiring. Fractional value which might truncate to zero 
  depending on unit passed as parameter is rounded to 1.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_get_duration
(
  timer_ptr_type                  timer,
  timer_unit_type                 unit
)
{
  /* Ticks until timer expires */
  time_timetick_type					 ticks;

  /* Pointer to internal timer */
  timer_ptr_internal_type 		timer_int;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  if (timer == NULL)
  {
    ASSERT(0);
    //return (time_timetick_type)0;
  }
  
  ATS_INTLOCK();

  timer_int = timer_get_internal_timer ( timer );
  if ( timer_int == NULL )
  {  
    ATS_INTFREE();
    return 0;
  }

  if (timer_int->cache.unit != unit)
	DalTimetick_CvtFromTimetick64(hTimerHandle,timer_int->cache.duration_sclk,unit,&ticks);
  else
	ticks = timer_int->cache.duration;

  ATS_INTFREE();
  return ticks;  

} /* timer_get_duration */



/*=============================================================================

FUNCTION TIMER_GET

DESCRIPTION
  Get number of ticks in 32kHz domain before timer expires

DEPENDENCIES
  None

RETURN VALUE
  Number of ticks in 32kHz domain before timer expires.
  Zero is returned for inactive timers, and timers that have expired or
  are expiring. Fractional value which might truncate to zero 
  depending on unit passed as parameter is rounded to 1.

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timer_get
(
  timer_ptr_type                  timer,
  timer_unit_type                 unit
)
{
  if( unit == T_TICK || unit == T_NONE )
  {
    return timer_conv_to_sclk_freq(timer_get_64(timer,unit));
  }
  else
  {
    return (timetick_type)timer_get_64(timer,unit);
  }
} /* timer_get */


/*=============================================================================

FUNCTION TIMER_GET_64

DESCRIPTION
  Get number of ticks in 19MHz domain before timer expires

DEPENDENCIES
  None

RETURN VALUE
  Number of ticks in 19MHz domain before timer expires.
  Zero is returned for inactive timers, and timers that have expired or
  are expiring. Fractional value which might truncate to zero 
  depending on unit passed as parameter is rounded to 1.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_get_64
(
  /* Timer to get */
  timer_ptr_type                  timer,

  /* Unit to measure "ticks before timer expires" in. */
  timer_unit_type                 unit
)
{
  /* Ticks until timer expires */
  time_timetick_type                   ticks;

  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (timer == NULL)
  {
    ASSERT(0);
    //return (time_timetick_type)0;
  }
  
  ATS_INTLOCK();

  timer_int = timer_get_internal_timer ( timer );
  DalTimetick_GetTimetick64(hTimerHandle, &ticks);
  /* This API LOG should be before any possible early exit of the function */
  TIMER_API_LOG(TIMER_API_LOG_TIMER_GET,timer,ticks);

  if ( timer_int == NULL )
  {  
    ATS_INTFREE();
    return 0;
  }

  if ( timer_int->list != NULL )
  {
    /* Timer is active. Ensure non-zero tick count */

    ticks = timer_expires_in( timer_int );
    if ( ticks == 0 )
    {
      ticks = 1;
    }

  }
  else
  {
    /* Timer is not active */

    ticks = 0;
  }

  ATS_INTFREE();

  /* If timer is active then ensure that return value is non-zero */
  if ( ticks != 0 )
  {
    DalTimetick_CvtFromTimetick64(hTimerHandle,ticks,unit,&ticks);
    if ( ticks == 0 )
    {
      ticks = 1;
    }
  }

  return ticks;

} /* timer_get_64 */


/*=============================================================================

FUNCTION TIMER_GET_PARAMS

DESCRIPTION
  Updates the passed structure as per the timer parameters.

DEPENDENCIES
   None

RETURN VALUE
  TE_SUCCESS if success
  TE_INVALID_PARAMETERS if passed pointers are NULL


SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_get_params
(
  timer_ptr_type                    timer,
  timer_attrib_struct_type *        timer_attrib_struct_ptr
)
{

  /* return error if the passed timer pointer is NULL */
  if (timer==NULL || timer_attrib_struct_ptr == NULL)
	  return TE_INVALID_PARAMETERS;
  
  ATS_INTLOCK();
  /* Update the passed timer attribute structure members */
  timer_attrib_struct_ptr->tcb_sigs_obj      = (void *)timer->sigs_obj;
  timer_attrib_struct_ptr->sigs              = (uint32)timer->sigs;
  timer_attrib_struct_ptr->func1             = timer->func1;
  timer_attrib_struct_ptr->func2             = timer->func2;
  timer_attrib_struct_ptr->data              = timer->data;
  timer_attrib_struct_ptr->notify_type       = timer->notify_type;
  timer_attrib_struct_ptr->timer_state       = timer->timer_state;

  ATS_INTFREE();
  return TE_SUCCESS;
} /* timer_get_params */




/*=============================================================================

FUNCTION TIMER_SET_PARAMS

DESCRIPTION
  Updates the timer as per the passed params.

DEPENDENCIES
  None

RETURN VALUE
  TE_SUCCESS if success
  TE_INVALID_PARAMETERS if passed pointers are NULL


SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_set_params
(
  timer_ptr_type                    timer,
  timer_attrib_set_type             to_set,
  timer_attrib_struct_type *        timer_attrib_struct_ptr
)
{
  #ifdef FEATURE_ENABLE_TIMER_SET_PARAM_LOG
  timer_state_struct_type  tmp_timer_state;
  #endif

  #ifdef FEATURE_ENABLE_TIMER_SET_PARAM_LOG
  time_timetick_type       ticks = 0;
  #endif /* FEATURE_ENABLE_TIMER_SET_PARAM_LOG */
  
  timer_ptr_internal_type  int_timer = NULL;


  /* return error if the passed timer pointer is NULL */
  if (timer==NULL || timer_attrib_struct_ptr == NULL)
	  return TE_INVALID_PARAMETERS;

  if ((timer_attrib_struct_ptr->notify_type & TIMER_NATIVE_OS_SIGNAL_TYPE) ||
      (timer_attrib_struct_ptr->notify_type & TIMER_NATIVE_OS_SIGNAL2_TYPE) )
      return TE_INVALID_PARAMETERS; 

  
  ATS_INTLOCK();
  /* Update the timer notification attributes */
  if (to_set == TIMER_UPDATE_NOTIFICATION_PARAMS)
  	{
  	  timer->notify_type |= timer_attrib_struct_ptr->notify_type;
	  
  	  if ( (timer_attrib_struct_ptr->notify_type & TIMER_DALSYS_EVENT_SIGNAL_TYPE) ||
		   (timer_attrib_struct_ptr->notify_type & TIMER_NATIVE_OS_SIGNAL_TYPE) ||
		   (timer_attrib_struct_ptr->notify_type & TIMER_NATIVE_OS_SIGNAL2_TYPE) ||
	  	   (timer_attrib_struct_ptr->notify_type & TIMER_REX_TCB_SIGNAL_TYPE) )
  	  	{
  	  	  timer->sigs_obj = timer_attrib_struct_ptr->tcb_sigs_obj;
		  timer->sigs     = timer_attrib_struct_ptr->sigs;
  	  	}

	  if (timer_attrib_struct_ptr->notify_type & TIMER_FUNC1_CB_TYPE)
	  	{
          timer->func1  = timer_attrib_struct_ptr->func1;
		  timer->data  = timer_attrib_struct_ptr->data;
	  	}

	  if (timer_attrib_struct_ptr->notify_type & TIMER_FUNC2_CB_TYPE)
	  	{
          timer->func2= timer_attrib_struct_ptr->func2;
		  timer->data  = timer_attrib_struct_ptr->data;
	  	}

	  if (timer_attrib_struct_ptr->notify_type == TIMER_NO_NOTIFY_TYPE)
	  	{
  	  	  timer->sigs_obj   = (time_osal_notify_obj_ptr)NULL;
     	  timer->sigs  = (time_osal_notify_data)0;
	      timer->func1  = (timer_t1_cb_type)NULL;
	      timer->func2  = (timer_t2_cb_type)NULL;
	      timer->data  = (timer_cb_data_type)0;
	  	}

 	}
  else 
  	{
	ATS_INTFREE();
  	return TE_INVALID_PARAMETERS;
  	}

  /* update the data for the corresponding internal timer as well */
  int_timer = timer_get_internal_timer(timer);

  /* Check if there is any internal timer for this external timer as well */
  if (int_timer != NULL)
  	{
     int_timer->notify_type = timer->notify_type;
	 int_timer->notify_obj_ptr= timer->sigs_obj;
	 int_timer->sigs          = timer->sigs;
	 int_timer->func1         = timer->func1;
	 int_timer->func2         = timer->func2;
	 int_timer->data          = timer->data;
  	}

  /* Get the timer's current state, for logging purpose */
  #ifdef FEATURE_ENABLE_TIMER_SET_PARAM_LOG
  tmp_timer_state = timer->timer_state ;
  #endif

  ATS_INTFREE();

  #ifdef FEATURE_ENABLE_TIMER_SET_PARAM_LOG
  
  /* Log whenever somebody updates the timer structure */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks);
  ATS_MSG_HIGH("timer_set_params called : timer = 0x%x; notification_type = 0x%x ; timetick = 0x%llx",
  	        timer,
  	        timer_attrib_struct_ptr->notify_type,
  	        ticks);
  timer_set_params_log[timer_set_params_log_cnt].timer = timer;
  timer_set_params_log[timer_set_params_log_cnt].ts = ticks;
  timer_set_params_log[timer_set_params_log_cnt].timer_state = tmp_timer_state;
  timer_set_params_log[timer_set_params_log_cnt].notification = timer_attrib_struct_ptr->notify_type;
  timer_set_params_log_cnt = (timer_set_params_log_cnt+1)%MAX_TIMER_SET_PARAMS_LOG;

  #endif /* FEATURE_ENABLE_TIMER_SET_PARAM_LOG */

  return TE_SUCCESS;
} /* timer_set_params */



/*=============================================================================

FUNCTION TIMER_CLR

DESCRIPTION
  Stops an active timer

DEPENDENCIES
  None

RETURN VALUE
  Number of ticks, in 32kHz domain, remaining before expiry.

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timer_clr
(
  timer_ptr_type                  timer,
  timer_unit_type                 unit
)
{
  if( unit == T_TICK || unit == T_NONE )
  {
    return timer_conv_to_sclk_freq(timer_clr_64(timer,unit));
  }
  else
  {
    return (timetick_type)timer_clr_64(timer,unit);
  }
} /* timer_clr */


/*=============================================================================

FUNCTION TIMER_CLR_64

DESCRIPTION
  Stops an active timer

DEPENDENCIES
  None

RETURN VALUE
  Number of ticks, in 19MHz domain, remaining before expiry.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_clr_64
(
  /* Timer to stop */
  timer_ptr_type                  timer,

  /* Unit to measure "ticks before expiry" in.  Use T_NONE if not required */
  timer_unit_type                 unit
)
{
  time_timetick_type	rem_time = 0;

  (void) timer_stop(timer,unit,&rem_time);

  return rem_time;  

} /* timer_clr_64 */


/*=============================================================================

FUNCTION timer_undef

DESCRIPTION
  Deletes an active timer

DEPENDENCIES
  None

RETURN VALUE
  TE_SUCCESS if success
  TE_INVALID_PARAMETERS if timer  pointer passed is invalid
  TE_TIMER_ALREADY_IN_SAME_STATE if timer is already deleted

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_undef
(
  timer_ptr_type                  timer
)
{
  /* For returning the status */
  timer_error_type          status;
  
  time_timetick_type        ticks = 0;

 
  /* Validate the input */
  if (timer == NULL)
  	return TE_INVALID_PARAMETERS;

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &ticks);
  
  TIMER_API_LOG(TIMER_API_LOG_TIMER_UNDEF,timer,ticks);

  /* We have already checked the input params, so no need to recheck again
        for the return status of timer_stop, as we will be doing memset on the 
        external timer in any case, whether it is inactive / already in same state. */
  status = timer_stop(timer, T_NONE, NULL);
  
  /* Zero out the timer */
  memset( timer, 0, sizeof(timer_type));
  
  timer->timer_state = TIMER_UNDEF_FLAG;
  
  ATS_INTFREE();
  return status;

} /* timer_undef */


/*=============================================================================

FUNCTION timer_stop

DESCRIPTION
  Stops an active timer

DEPENDENCIES
  None

RETURN VALUE
  TE_INVALID_PARAMETERS
  TE_TIMER_ALREADY_IN_SAME_STATE
  TE_SUCCESS

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_stop
(
  timer_ptr_type				   timer,
  timer_unit_type	               unit,
  time_timetick_type			   *rem_time
)
{

  /* Ticks until timer expires */
  time_timetick_type              ticks = 0;

  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;

  timer_group_ptr                 local_timer_group=NULL;

  timer_error_type                status = TE_SUCCESS;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &ticks);

  /* Get the corresponding internal timer */
  timer_int = timer_get_internal_timer ( timer );  

  /* Timer clr logging */
  #ifdef FEATURE_ENABLE_TIMER_CLR_DEBUG_VARIABLES
  timer_clr_log[timer_clr_log_cnt].ext_timer = timer;
  timer_clr_log[timer_clr_log_cnt].int_timer = timer_int;
  timer_clr_log[timer_clr_log_cnt].ts = ticks;
  timer_clr_log[timer_clr_log_cnt].ret_val = TE_TIMER_ALREADY_IN_SAME_STATE;
  timer_clr_log[timer_clr_log_cnt].timer_state = timer->timer_state;
  timer_clr_log[timer_clr_log_cnt].tid = qurt_thread_get_id();
  #endif /* FEATURE_ENABLE_TIMER_CLR_DEBUG_VARIABLES */
  
  TIMER_API_LOG(TIMER_API_LOG_TIMER_CLR,timer,ticks);
  if ( timer_int == NULL )
  { 
    #ifdef FEATURE_ENABLE_TIMER_CLR_DEBUG_VARIABLES
  	timer_clr_log_cnt = (timer_clr_log_cnt+1)%MAX_TIMER_CLR_LOG;    
	#endif /* FEATURE_ENABLE_TIMER_DEBUG_VARIABLES */
	
    ATS_INTFREE();
    return TE_TIMER_ALREADY_IN_SAME_STATE;
  }

  local_timer_group = timer_int->group;
  if(NULL == timer_int->group) 
  {
    timer_int->group = &timer_null_group;
  } 

  timer->last_expiry = 0;
  
  if ( timer_int->list != NULL )
  {
    /* Timer is active - record remaining time and remove timer */

    ticks = timer_expires_in( timer_int );
    timer_remove( timer_int, TRUE );
	status = TE_SUCCESS;
	
    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_TIMER_CLEARED);
  }
  /*if the timer was paused, then it was removed from the active list
   or the disabled list, but it was not returned back to the free 
   queue. Hence, if a paused timer was cleared, return it back
   to the free queue*/
  else if(timer->timer_state == TIMER_PAUSED_FLAG) 
  {
    timer_remove_internal_timer_from_q(timer_int);
	ticks = 0;
	status = TE_SUCCESS;
    timer_count_paused_timers--;
  }
  else
  {
    /* Timer was not active */
    ticks = 0;
	status = TE_TIMER_ALREADY_IN_SAME_STATE;
  }

  timer->timer_state = TIMER_CLEARED_FLAG;

  if ( local_timer_group == NULL )
  {
	char qurt_task_name[12];
	qurt_thread_get_name(&qurt_task_name[0], sizeof(qurt_task_name));
	ATS_MSG_SPRINTF_2("[timer_clr] : timer_def was not called for timer %x, task %s",
					  timer,qurt_task_name);   
  }

  if (rem_time != NULL)
  	{
	 DalTimetick_CvtFromTimetick64(hTimerHandle,ticks,unit,rem_time);
  
     /* Return at least 1 as the remaining time if the timer was cleared */
     if (status == TE_SUCCESS && *rem_time == 0)
     	*rem_time = 1;
  	}

  /* Timer clr logging */
  #ifdef FEATURE_ENABLE_TIMER_CLR_DEBUG_VARIABLES
  timer_clr_log[timer_clr_log_cnt].ret_val = status;
  timer_clr_log_cnt = (timer_clr_log_cnt+1)%MAX_TIMER_CLR_LOG; 
  #endif /* FEATURE_ENABLE_TIMER_CLR_DEBUG_VARIABLES */
  
  ATS_INTFREE();
  return status;
}/* timer_stop */


/*=============================================================================

FUNCTION TIMER_PAUSE

DESCRIPTION
  Pauses an active timer.

DEPENDENCIES
  Should only be called for a timer that is running.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_pause
(
  /* Timer to pause */
  timer_ptr_type                  timer
)
{
  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;
   time_timetick_type             now = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if (timer == NULL)
  {
    ASSERT(0);
    //return;
  }

  if ( timer->group == NULL )
  {
	char qurt_task_name[12];
	qurt_thread_get_name(&qurt_task_name[0], sizeof(qurt_task_name));
	ATS_MSG_SPRINTF_2("[timer_pause] : timer_def was not called for timer %x, task %s",
					  timer,qurt_task_name);   
  }  

  ATS_INTLOCK();

  timer_int = timer_get_internal_timer ( timer );
  DalTimetick_GetTimetick64(hTimerHandle, &now);
  TIMER_API_LOG(TIMER_API_LOG_TIMER_PAUSE,timer,now);
  if ( timer_int == NULL )
  {  
    ATS_INTFREE();
    return ;
  }  
  
  if(NULL == timer->group) 
  {
     timer->group= &timer_null_group;
  }

  if ( timer_int->list != NULL )
  {
    /* Timer is active - remove it, and record remaining time */

    timer_int->expiry = timer_expires_in( timer_int );
    
    /* Update sclk_delta */
    DalTimetick_GetOffset(hTimerHandle,&timer_int->sclk_delta);

    timer_remove( timer_int, FALSE );

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt( TIMER_MVS_TIMER_PAUSED);

    timer_count_paused_timers++;
  }

  timer->timer_state = TIMER_PAUSED_FLAG;
  ATS_INTFREE();

} /* timer_pause */



/*=============================================================================

FUNCTION TIMER_RESUME

DESCRIPTION
  Resumes a previously paused timer.

  If a timer was paused 5 seconds before its expiry, and 30 seconds later
  timer_resume( ) is called, the timer will expire 5 seconds after that point.

DEPENDENCIES
  Must only be called for a timer that has been paused.

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

void timer_resume
(
  /* Timer to resume */
  timer_ptr_type                  timer
)
{
  /* Ticks until timer expires */
  time_timetick_type                   start;
  time_timetick_type              now = 0;

  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (timer == NULL)
  {
    ASSERT(0);
    //return;
  }

  if ( timer->group == NULL )
  {
	char qurt_task_name[12];
	qurt_thread_get_name(&qurt_task_name[0], sizeof(qurt_task_name));
	ATS_MSG_SPRINTF_2("[timer_resume] : timer_def was not called for timer %x, task %s",
					  timer,qurt_task_name);   
  }
  
  ATS_INTLOCK();

  timer_int = timer_get_internal_timer ( timer );
  DalTimetick_GetTimetick64(hTimerHandle, &now);
  TIMER_API_LOG(TIMER_API_LOG_TIMER_RESUME,timer,now);
  if ( timer_int == NULL )
  {  
    ATS_INTFREE();
    return ;
  }  

  if(NULL == timer->group) 
  {
    timer->group = &timer_null_group;
  }

  if ( timer_int->list == NULL )
  {
    /* Determine when timer should expire */

    DalTimetick_GetTimetick64(hTimerHandle, &start);
    timer_int->expiry += start;

    /* Update sclk_delta */
    DalTimetick_GetOffset(hTimerHandle,&timer_int->sclk_delta);

    /* Insert timer in either active timer list, or group timer list if it
       belongs to a disabled timer group */

    if ( timer_int->group->disabled )
    {
      /* Timer belongs to a disabled group - add it to that group's list */
      timer_insert( &timer_int->group->list, timer_int );
    }
    else
    {
      if(TIMER_PRIORITY_0 == timer_int->timer_priority) 
      {
        /* Timer is not a member of a disabled group - add to active list */
        timer_insert( &timers.p0_active.list, timer_int);
      }
      else
      {
      /* Timer is not a member of a disabled group - add to active list */
      timer_insert( &timers.active.list, timer_int);
      }

      /* Active timer list has changed - ensure next timer event is correct */
      timer_update_timer_interrupt(TIMER_MVS_TIMER_RESUMED);
    }

    timer_count_paused_timers--;
  }
  timer->timer_state = TIMER_RESUMED_FLAG;

  ATS_INTFREE();

} /* timer_resume */

/*=============================================================================

FUNCTION TIMER_CLR_OSAL_THREAD_TIMERS

DESCRIPTION
  Stops active timers belonging to the given thread ID

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_clr_osal_thread_timers
(
  /* Timers belonging to this thread will be cleared (stopped). */
  time_osal_thread_id                    thread_id
)
{
  /* Pointer to a timer group.  Used for walking list of timer groups */
  timer_group_ptr                 group;

  /* Pointer to a timer.  Used for walking list of timers */
  timer_ptr_internal_type         timer;

  /* Pointer to the next timer.  Used for walking list of timers */
  timer_ptr_internal_type         next;

  /* Timetick value for logging */
  time_timetick_type              now = 0;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_CLR_TASK_TIMERS,NULL,now);

  /* Walk list of timer groups, starting with the faux active group */

  group = &timers.active;

  do
  {
    /* Walk the list of timers in this group, and
       remove timers which belong to given task */

    for( timer = group->list.first;  timer != NULL;  timer = next)
    {
      /* Save next timer in list, in case this timer's next field in NULL'd */
      next = timer->next;

      if ( timer->thread_id == thread_id)
      {
        if ( timer->timer_ext != NULL )
		 {
          timer->timer_ext->last_expiry = 0;
		  timer->timer_ext->timer_state = TIMER_CLEARED_FLAG;
		 }
        else
          {
          ATS_INTFREE();
          ERR_FATAL (" Timer must have an ext timer, int_timer=0x%x timer_ext=0x%x",
                   timer,timer->timer_ext,0);
		  return;
          }

        /* Remove the timer from the list */
        timer_remove( timer, TRUE );
      }
    }

    /* Determine the next timer group in the chain ... stopping when we
       return to the start of the list with the faux timer group */

    group = group->next;
  }
  while ( group != &timers.active );


  /* Active timer list may have changed - ensure next timer event is correct */
  timer_update_timer_interrupt(TIMER_MVS_TIMER_CLEAR_TASK_TIMERS );


  ATS_INTFREE();

  return;

} /* timer_clr_osal_thread_timers */

#ifdef FEATURE_TIMER_REX_IS_AVAILABLE

/*=============================================================================

FUNCTION TIMER_CLR_TASK_TIMERS

DESCRIPTION
  Stops active timers belonging to the given task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_clr_task_timers
(
  /* Timers belonging to this task will be cleared (stopped). */
  rex_tcb_type                    *tcb
)
{
  /* Pointer to a timer group.  Used for walking list of timer groups */
  timer_group_ptr                 group;

  /* Pointer to a timer.  Used for walking list of timers */
  timer_ptr_internal_type         timer;

  /* Pointer to the next timer.  Used for walking list of timers */
  timer_ptr_internal_type         next;

  /* Timetick value for logging */
  time_timetick_type              now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_CLR_TASK_TIMERS,NULL,now);

  /* Walk list of timer groups, starting with the faux active group */

  group = &timers.active;

  do
  {
    /* Walk the list of timers in this group, and
       remove timers which belong to given task */

    for( timer = group->list.first;  timer != NULL;  timer = next)
    {
      /* Save next timer in list, in case this timer's next field in NULL'd */
      next = timer->next;

      if ( timer->notify_obj_ptr == tcb)
      {
        if ( timer->timer_ext != NULL )
        {
          timer->timer_ext->last_expiry = 0;
          /* Set cleared flag */
          timer->timer_ext->timer_state = TIMER_CLEARED_FLAG;
        }
        else
        {
          ERR_FATAL (" Timer must have an ext timer, timer_ptr=0x%x timer_ext=0x%x",
                    timer,timer->timer_ext,0);
        }

        /* Remove the timer from the list */
        timer_remove( timer, TRUE );

      }
    }

    /* Determine the next timer group in the chain ... stopping when we
       return to the start of the list with the faux timer group */

    group = group->next;
  }
  while ( group != &timers.active );


  /* Active timer list may have changed - ensure next timer event is correct */
  timer_update_timer_interrupt(TIMER_MVS_TIMER_CLEAR_TASK_TIMERS );


  ATS_INTFREE();

} /* timer_clr_task_timers */
#endif /* FEATURE_TIMER_REX_IS_AVAILABLE */


/*=============================================================================

FUNCTION TIMER_GET_SCLK_TILL_EXPIRY

DESCRIPTION
  Returns the number of sclks, in 32kHz domain, until the expiry of
  next timer to expire.

DEPENDENCIES
  This function is provided for the exclusive use of SLEEP, for TCXO shutdown.
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  Number of sclk's, in 32kHz domain, until the next timer expiry.

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timer_get_sclk_till_expiry( void )
{
  return timer_conv_to_sclk_freq(timer_get_sclk_till_expiry_64());
} /* timer_get_sclk_till_expiry */


/*=============================================================================

FUNCTION TIMER_GET_SCLK_TILL_EXPIRY_64

DESCRIPTION
  Returns the number of ticks, in 19MHz domain, until the expiry of
  next timer to expire.

DEPENDENCIES
  This function is provided for the exclusive use of SLEEP, for TCXO shutdown.
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  Number of ticks, in 19MHz domain, until the next timer expiry.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_get_sclk_till_expiry_64( void )
{
  /* Current slow clock count */
  time_timetick_type                   ticks_now = 0;
  time_timetick_type                   tt_ret;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  /* Get the current time */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_GET_SCLK_TILL_EXPIRY,NULL,ticks_now);

  if ( timers.set_value - timers.set_time > ticks_now - timers.set_time )
  {  
    /* Compute & return the time until the timer is set to expire */
    tt_ret = timers.set_value - ticks_now;
  }
  else
  {  
    /* The next timer interrupt is already pending! */
    tt_ret = 0;
  }

  ATS_INTFREE();

  return tt_ret;
} /* timer_get_sclk_till_expiry_64 */



/*=============================================================================

FUNCTION TIMER_SET_WAKEUP

DESCRIPTION
  This function sets the timer interrupt to expire in advance of the
  the next expiring timer's expiry point, to allow for TCXO to turn back on.
  This function works in the 32kHz domain.

DEPENDENCIES
  This function is provided for the exclusive use of SLEEP, for TCXO shutdown.
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_set_wakeup
(
  timetick_type                        wakeup_sclks
)
{
  (void)timer_set_wakeup_64(timer_conv_to_xo_freq(wakeup_sclks));
} /* timer_set_wakeup */


/*=============================================================================

FUNCTION TIMER_SET_WAKEUP_64

DESCRIPTION
  This function sets the timer interrupt to expire in advance of the
  the next expiring timer's expiry point, to allow for TCXO to turn back on.
  This function works in the 19MHz domain.

DEPENDENCIES
  This function is provided for the exclusive use of SLEEP, for TCXO shutdown.
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  TE_SUCCESS always currently

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_set_wakeup_64
(
  /* Wakeup the CPU in the given number of sclk's. */
  time_timetick_type                   wakeup_sclks
)
{
  /* Current slow clock count */
  time_timetick_type                   ticks_now = 0;

  timer_error_type                     timer_error = TE_SUCCESS;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current time2 */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_SET_WAKEUP,NULL,ticks_now); 
  /* Program the timer */
  timer_set_next_interrupt( ticks_now + wakeup_sclks, ticks_now, FALSE, 
                            TIMER_MVS_SET_WAKEUP, NULL );

  return timer_error;

}

/* timer_set_wakeup_64 */


/*=============================================================================

FUNCTION TIMER_ACTIVE_LIST_CHECK

DESCRIPTION
  Throws an error fatat if timer list is corrupted

DEPENDENCIES
  FEATURE_TIMER_TRACE and FEATURE_TIMER_LIST_DEBUG should be turned on.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_active_list_check(void)
{
  
  #if defined( FEATURE_TIMER_TRACE ) && defined( FEATURE_TIMER_LIST_DEBUG )
  
  /* Pointer to traverse timer list */
  timer_ptr_internal_type          ptimer1, ptimer2;

  /* Number of timers in the list starting from first timer */
  uint32 first_num;
  
  /* Number of timers in the list starting from last timer */
  uint32 last_num;

  /* First & last timer respectively in the active list */
  timer_ptr_internal_type          first, last;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DalTimetick_GetTimetick64(hTimerHandle, &now);

  /* Return if active list is NULL */
  if ( timers.active.list.first == NULL )
    return;
  
  /* Spin thru the active timer list */
  ptimer1 = timers.active.list.first->next;
  
  if( ptimer1 != NULL )
    ptimer2 = ptimer1->next;
  else
    ptimer2 = NULL;

  /* Checks for Cirular loop in timers' list 
     Start with 2 pointers ( ptimer1  and ptimer2 ). 
     Advance ptimers1 with one node and ptimer2 with two nodes
     At any point if ptimer1 == ptimer2 then you have a loop, 
     otherwise ptimer2 will reach NULL in n/2 iterations. */
  
  while( ptimer1 != NULL && ptimer2 != NULL )
  {
    
    if (ptimer1 == ptimer2 )
    {
      ERR_FATAL(" Timer List Corrupted, ptimer1=0x%x ", ptimer1,0,0 );
    }

    ptimer1 = ptimer1->next;
    ptimer2 = ptimer2->next;

    if ( ptimer2 !=NULL )
    {
      ptimer2 = ptimer2->next; 
    }
    else
    {
      break;
    }
  }
  
  first      = timers.active.list.first;
  last       = timers.active.list.last;
  first_num  = 0;
  last_num   = 0;

  timer_abs_zero_count = 1;
  timer_top_of_list    = 1;
  timer_top_timers_cnt = 0;
  while( first != NULL )
  {
     /* To catch the timers that have not been expired though the expiry time
        has been crossed. A timer is expired if expiry - start of that timer is
        less than now - start. But if such a timer still exists in timers list,
        it means it been fired and thus need to error fatal */
    if(timers.processing == 0 && ((first->expiry - first->start) < (now - first->start)))
    {
       if(timer_top_of_list != 1)
       {
           ERR_FATAL(" timer active list corrupted due to timers not being expired, top_of_list=0x%x ",timer_top_of_list,0,0); 
       }
       else
       {
      /* Sometimes current time crosses expiry of top of the timers and
         are ready to fire. But before that if there is an active list check that
         will lead to error fatal which may not be necessary as they will be
         fired anyway. Thus, ignoring the starting timers even they have crossed now */
          timer_top_timers_cnt++;
       }
    }
    else
    {
       /* Starting timers that have crossed expiry are over */
       timer_top_of_list=0;
    }

  /* Check if the links of the doubly linked list are proper starting from the
     first timer and also count the number of timer */
    if( first->next != NULL )
    {
      if( first != first->next->prev )
      {
         ERR_FATAL(" timer active list corrupted, ptr1=0x%x ptr1->next->prev=0x%x",first,first->next->prev,0); 
      }

      /* Checking for expiries of the timers to be in sorted order for some corruption issues */
      if((first->expiry > first->next->expiry) && (timer_abs_zero_count == 1 || first->expiry > timer_max_expiry))
      {
         if(timer_abs_zero_count == 1)
         {
           /* Handling abs zero crossing. After this all the timers should be 
             less than max_expiry value which is before zero crossing */
            timer_abs_zero_count = 0;
            timer_max_expiry = first->expiry;
         }
         else
         {
            ERR_FATAL("Timer active list corrupted due to small timer being inserted after large timer, cnt=0x%x",timer_abs_zero_count,0,0); 
         }
      }
    }
    first = first->next;
    first_num++;
  }

  /* Count the number of timers in the list starting from last timer */
  while( last != NULL )
  {
    last = last->prev;
    last_num++;
  }

  /* List is corrupted if first_num is not same as last_num or is 
     not same as actual number of timers supposed to be present 
     at given point of time */

  if (first_num != last_num || first_num != timer_active_timers_count )
    ERR_FATAL(" timer active list corrupted, first_num=0x%x last_num=0x%x activecnt=0x%x ",
              first_num,last_num,timer_active_timers_count );

  #endif /* FEATURE_TIMER_TRACE && FEATURE_TIMER_LIST_DEBUG */

} /*  timer_active_list_check */

/*=============================================================================

FUNCTION TIMER_PIMUTEX_INIT

DESCRIPTION
  This function initializes the mutex that will be used for protecting
  the crit-section portion of timer code.

DEPENDENCIES
  This function is applicable only for QURT.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void timer_mutex_init(void)
  {
  qurt_mutex_init ( &timer_qemu_mutex );
  }

void timer_trace_mutex_init(void)
  {
  qurt_mutex_init ( &timer_trace_mutex );
}
void timer_pc_mutex_init(void)
  {
  qurt_mutex_init ( &timer_pc_mask_mutex );
}

/*=============================================================================

FUNCTION TIMER_GET_QURT_SIG_REGISTRATION_ID

DESCRIPTION
  This function returns the registration id that will be used to signal the 
  expiry of the first timer.

DEPENDENCIES
  This function is applicable only for QURT.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
int timer_get_qurt_sig_registration_id
(
  void
)
{
  time_timetick_type          now = 0;

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_GET_QURT_SIG_REGISTRATION_ID,NULL,now);
#ifdef FEATURE_QDSP6
  return timer_qurt_sig_registration_id;
#else
  return -1; 
#endif
}


/*=============================================================================

FUNCTION TIMER_INIT_QURT_SIGNAL

DESCRIPTION
  This function initializes QURT signal related to timer expiry. When
  the match value == timetick value, QURT will set this signal. 

  This function also initializes the mutex that will be used in crit-section
  portions of timer code.

DEPENDENCIES
  This function is applicable only for QURT.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
#ifdef FEATURE_QDSP6
static void timer_init_qurt_signal
(
  void
)
{
  time_timetick_type            now = 0;

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_INIT_QURT_SIGNAL,NULL,now);

  /*initialize the signal mask that we want to wait on*/
  qurt_signal_init (&timer_q6_signal);

  timer_qurt_sig_registration_id = 
        qurt_sysclock_register (&timer_q6_signal, TIMER_QURT_SIG_MASK);

  if (timer_qurt_sig_registration_id < 0) 
  {
    ERR_FATAL("sclk registration failed, id=0x%i",timer_qurt_sig_registration_id,0,0);
  }

  /*pass on the qurt registration id to the timetick layer*/
  DalTimetick_UpdateBlastSigId(hTimerHandle,(uint32)timer_qurt_sig_registration_id);
}
#endif

/*=============================================================================

FUNCTION TIMER_UPDATE_ISR_TIME

DESCRIPTION
  This function is called in QURT, when the timer signal is set. In Q6, 
  there is no timer interrupt, but for backward compatibility, the variables
  timer.isr_time, etc. need to be populated.

DEPENDENCIES
  This function is applicable only for QURT.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void timer_update_isr_time
(
  void
)
{
  /* Timer sclk time-stamp values */
  time_timetick_type                   set_time, set_value;

  /* Latency in ISR handling */
  time_timetick_type                   latency_sclk;
  time_timetick_type                   now = 0;

  ATS_INTLOCK( );
  /* Copy isr timer locally */
  now = timers.isr_time;

  TIMER_API_LOG(TIMER_API_LOG_TIMER_UPDATE_ISR_TIMER,NULL,now);  

  /* Capture timer set values, for messaging outside of ATS_INTLOCK. */
  set_time  = timers.set_time;
  set_value = timers.set_value;

  ATS_INTFREE();


  if ( now - set_time >= set_value - set_time)
  {
    /* Determine the difference between when the interrupt was programmed to
       occur at, and when the interrupt was actually handled. */
    latency_sclk = now - set_value;

    if ( latency_sclk > TIMER_SCLK_LATENCY )
    {
      /* Timer interrupt was handled over btw 5 (60kHz) to 10ms (30kHz) late.
         (Timers may expire much later than 5-10ms, if they are in a disabled
         group, or if multiple timers expire due to the same interrupt.
         This is only a measure of the timer isr latency.) */

      ATS_ERR_3("Late Timer ISR: ST=%d SV=%d IT-SV=%d",
                set_time, set_value, latency_sclk);
    }
  }
  else
  {
#ifndef FEATURE_QDSP6
    ATS_MSG_3("Early Timer ISR: ST=%d SV=%d SV-IT=%d",
             set_time, set_value, set_value - now);
#endif
  }

  /* Save a trace packet */
  /* Updated in timer_task itself 
   * TIMER_TRACE(ISR);
   */
}

/*=============================================================================

FUNCTION TIMER_IS_ACTIVE

DESCRIPTION
 This is function which queries whether a timer is active or not.        
 A timer which has been set to expire at a certain time - even if the   
 group it is a member of is disabled - is considered to be active.      
 A timer which is explicitly paused is not active, until it has been    
 resumed.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if the timer is active.FALSE if the timer is not active.

SIDE EFFECTS
  None

=============================================================================*/
boolean timer_is_active
(
  timer_ptr_type  timer
)
{
  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;
  boolean                         timer_active=FALSE;
  time_timetick_type              now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (timer == NULL)
  {
    ASSERT(0);
    //return FALSE;
  }

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_IS_ACTIVE,timer,now);
   
  timer_int = timer_get_internal_timer ( timer );

  if(timer_int
     && timer_int->list != NULL)
  {
    timer_active = TRUE;
  }
  ATS_INTFREE();
  
  return timer_active;

} /* timer_is_active */

/*=============================================================================

FUNCTION TIMER_EXPIRES_AT

DESCRIPTION
 This returns the sclk counter value when the timer    
 will expire at.See: timetick_get()  

DEPENDENCIES
  Timer must be running for the returned value to be meaningful.

RETURN VALUE
  timetick_type - sclk counter value corresponding to timer expiry point.

SIDE EFFECTS
  None

=============================================================================*/
timetick_type timer_expires_at
(
  timer_ptr_type  timer
)
{
  return timer_conv_to_sclk_freq(timer_expires_at_64(timer));
} /* timer_expires_at */


/*=============================================================================

FUNCTION TIMER_EXPIRES_AT_64

DESCRIPTION
 This returns the sclk counter value when the timer    
 will expire at. See: DalTimetick_GetTimetick64()  

DEPENDENCIES
  Timer must be running for the returned value to be meaningful.

RETURN VALUE
  time_timetick_type - XO counter value corresponding to timer expiry point.

SIDE EFFECTS
  None

=============================================================================*/
time_timetick_type timer_expires_at_64
(
  timer_ptr_type  timer
)
{
   /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;
  time_timetick_type                   expires_in=0;
  time_timetick_type              now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (timer == NULL)
  {
    ASSERT(0);
    //return (time_timetick_type)0;
  }

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_EXPIRES_AT,timer,now);
 
  timer_int = timer_get_internal_timer ( timer );

  if(timer_int) 
  {
    expires_in = timer_int->expiry;
  }
  ATS_INTFREE();
  
  return expires_in ;
  
} /* timer_expires_at_64 */


/*=============================================================================

FUNCTION TIMER_GET_START

DESCRIPTION
 This returns the sclk counter value in the 32kHz domain when the timer    
 was set  

DEPENDENCIES
  Timer must be running for the returned value to be meaningful.

RETURN VALUE
  timetick_type - sclk counter value corresponding to timer start point.

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timer_get_start
(
  timer_ptr_type  timer
)
{
  return timer_conv_to_sclk_freq(timer_get_start_64(timer));
}


/*=============================================================================

FUNCTION TIMER_GET_START_64

DESCRIPTION
 This returns the sclk counter value in the 19MHz domain when the timer    
 was set  

DEPENDENCIES
  Timer must be running for the returned value to be meaningful.

RETURN VALUE
  time_timetick_type - TCXO counter value corresponding to timer start point.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_get_start_64
(
  timer_ptr_type  timer
)
{
   /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;
  time_timetick_type                   timer_start=0;
  time_timetick_type              now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (timer == NULL)
  {
    ASSERT(0);
    //return (time_timetick_type)0;
  }

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_GET_START,timer,now);
 
  timer_int = timer_get_internal_timer ( timer );

  if(timer_int) 
  {
    timer_start = timer_int->start;
  }

  ATS_INTFREE();
  
  return timer_start;

} /* timer_get_start_64 */

/*=============================================================================

FUNCTION TIMER_GET_ACTIVE_TIMER_NUM

DESCRIPTION
 This returns the number of timers in 
 active list. These does NOT include 
 paused or disabled timers

DEPENDENCIES
  None

RETURN VALUE
  uint32 number of active timers

SIDE EFFECTS
  None

=============================================================================*/

uint32 timer_get_active_timer_num
(
  void
)
{
  return timer_active_timers_count;
}

/*=============================================================================

FUNCTION TIMER_GET_PAUSED_TIMER_NUM

DESCRIPTION
 This returns the number of paused timers 

DEPENDENCIES
  None

RETURN VALUE
  uint32 number of paused timers

SIDE EFFECTS
  None

=============================================================================*/
uint32 timer_get_paused_timer_num
(
  void
)
{
  return timer_count_paused_timers;
}

/*=============================================================================

FUNCTION TIMER_GET_TIMER_FREE_Q_CNT

DESCRIPTION
 This returns the timers allocated 
 in timer_buffer

DEPENDENCIES
  None

RETURN VALUE
  uint32 number of allocated timers in timer_buffer

SIDE EFFECTS
  None

=============================================================================*/
uint32 timer_get_timer_free_q_cnt
(
  void
)
{
  return timer_count_buffer_allocated;
}

/*=============================================================================

FUNCTION TIMER_GET_TIMETICK_HANDLE

DESCRIPTION
 This function is called from timer_init to pass timetick handle to timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void timer_get_timetick_handle(void)
{

#ifdef FEATURE_TIMER_SLAVE1
  /* We want slave task to return the handle since slave needs the ptr too */
  timer_slaves_get_timetick_handle((void**)&hTimerHandle);
#else /* FEATURE_TIMER_SLAVE1 */
  DalTimetick_Attach("SystemTimer", &hTimerHandle);
  ASSERT(hTimerHandle != NULL);
#endif /* FEATURE_TIMER_SLAVE1 */

  /* Enable system timer */
  DalTimetick_Enable(hTimerHandle,1);
  DalTimetick_InitTimetick64(hTimerHandle);

  /* Qurt thread ID needed for task synchronization */
  timer_thread_id = rcinit_lookup_qurttask("timer");
}


/*=============================================================================

FUNCTION TIMER_POST_INIT

DESCRIPTION
 This function is called from time_init to pass timetick handle to timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void timer_post_init(void)
{
  /*at this point, the dog task has started and hence we can register the slave tasks with
   the dog*/
  #ifdef FEATURE_DOG
  #ifdef FEATURE_TIMER_SLAVE1
  timer_slave_register_dog(TIMER_SLAVE_TASK1);
  #endif

  #ifdef FEATURE_TIMER_SLAVE2
  timer_slave_register_dog(TIMER_SLAVE_TASK2);
  #endif

  #ifdef FEATURE_TIMER_SLAVE3
  timer_slave_register_dog(TIMER_SLAVE_TASK3);
  #endif
  #endif /* FEATURE_DOG */

  timer_register_npa_client();
}

/*=============================================================================

FUNCTION TIMER_SLEEP_CB

DESCRIPTION
 Signals the task waiting on the task_sleep_timer by timer_sleep function.
 Data will be signal object for Qurt Kernel while task pointer on kernel

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void timer_sleep_cb
(
timer_cb_data_type      data
)
{
	/* Signal the task in order to resume the sleeping task */
	(void) qurt_signal_set((qurt_signal_t*)data, TIMER_SLEEP_CB_SIG_MASK);	
  
   return;
}

/*=============================================================================

FUNCTION TIMER_SLEEP

DESCRIPTION
 Suspends the client task to get suspended for the specified time.
 If non_deferrable option is True, task will surely resumed at specified time
 If non_deferrable option is False, and if Proc goes into sleep at the time of
 specified timeout, task will resume only after Proc wakesup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void timer_sleep
(
  time_timetick_type              timeout,
  timer_unit_type                 unit,
  boolean                         non_deferrable
)
{
  char qurt_task_name[12];

  /* Signal object for timer callback notification */
  qurt_signal_t        timer_sleep_cb_sig_obj;
  
  /* Timer for the client to handle the sleeping */
  timer_type		      task_sleep_timer;
  
  /* Data to send to the timer callback */
  timer_cb_data_type      cb_data;
   
  /* memset the external timer memory to zero */
  memset(&task_sleep_timer, 0, sizeof(timer_type));
  
  /* Initialize the qurt signal object */
  qurt_signal_init (&timer_sleep_cb_sig_obj);
  cb_data=(timer_cb_data_type)&timer_sleep_cb_sig_obj;
  
  if (non_deferrable != TRUE)
    {
	  /* Create a deferrable timer to wake up the calling task */
	  timer_def_osal( &task_sleep_timer, 
	             NULL, /* Null Timer Group */
				 TIMER_FUNC1_CB_TYPE, 
				 timer_sleep_cb, 
				 cb_data 
			   );
	}
  else
    {
	  /* Create a non-deferrable timer for the clinet */
	  timer_def_osal( &task_sleep_timer, 
	             &timer_non_defer_group, /* Null Timer Group */
				 TIMER_FUNC1_CB_TYPE, 
				 timer_sleep_cb, 
				 cb_data 
			   );
	}
	
  /* Set the timer to expire after the specified duration */
    (void)timer_set_64(&task_sleep_timer,
                 timeout,
                 0,
                 unit);				 

  
  qurt_thread_get_name(&qurt_task_name[0], sizeof(qurt_task_name));  
  ATS_MSG_SPRINTF("TIMER_SLEEP: Entering timer_sleep, taskname = %s; timeout=%d; unit=%d",
            qurt_task_name,
			(uint32)timeout,
			(uint32)unit);
  
  /* Wait on the Qurt signal object */
  qurt_signal_wait (&timer_sleep_cb_sig_obj, TIMER_SLEEP_CB_SIG_MASK, QURT_SIGNAL_ATTR_WAIT_ANY);

  /* Clear the signal */
  qurt_signal_clear (&timer_sleep_cb_sig_obj, TIMER_SLEEP_CB_SIG_MASK);


  ATS_MSG_SPRINTF("TIMER_SLEEP: Exiting from timer_sleep, taskname = %s; timeout=%d; unit=%d",
            qurt_task_name,
			(uint32)timeout,
			(uint32)unit);

  /* Initialize the qurt signal object */
  qurt_signal_destroy (&timer_sleep_cb_sig_obj);	
 
  return;
}

/*=============================================================================

FUNCTION TIMER_REGISTER_FOR_NON_DEFER_EXPIRY_CHANGE

DESCRIPTION
Registers for first non-deferrable timer expiry change notification.
Only signal type notifications are allowed, for others, error will be 
returned.

DEPENDENCIES
  Input objects should be initialized by client

RETURN VALUE
TE_INVALID_PARAMETERS   If passed sig obj addr is NULL or invalid notify type
TE_SUCCESS :            If the operation completed successfully

SIDE EFFECTS
  None

=============================================================================*/
timer_error_type timer_register_for_non_defer_expiry_change
(
  timer_notify_type        notify_type,
  time_osal_notify_obj_ptr notify_obj,
  time_osal_notify_data    notify_mask
)
{
   int idx;

   /* This registration is allowed only for signal type notifications */
   if(notify_type != TIMER_REX_TCB_SIGNAL_TYPE &&
	  notify_type != TIMER_NATIVE_OS_SIGNAL_TYPE &&
	  notify_type != TIMER_NATIVE_OS_SIGNAL2_TYPE &&
	  notify_type != TIMER_DALSYS_EVENT_SIGNAL_TYPE)
      {
        return TE_INVALID_PARAMETERS;
      }
   
   if(notify_obj == NULL)   
      return TE_INVALID_PARAMETERS;
      
   ATS_INTLOCK();
   
   if(timer_non_defer_expiry_notify_clients_cnt == TIMER_NON_DEFER_EXPIRY_NOTIFY_CLIENTS_MAX_CNT)
   {
      ATS_INTFREE();
      return TE_FAIL;
   }

   idx = timer_non_defer_expiry_notify_clients_cnt++;
   timer_non_defer_expiry_notify_clients[idx].notify_type = notify_type;
   timer_non_defer_expiry_notify_clients[idx].notify_obj = notify_obj;
   timer_non_defer_expiry_notify_clients[idx].notify_mask = notify_mask;
   
   ATS_INTFREE();
   
   return TE_SUCCESS;  
} /* timer_register_for_non_defer_expiry_change */

