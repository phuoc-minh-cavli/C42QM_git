/*============================================================================
@file lpr_definition_npa_scheduler.c
 
Provides the sleep LPR definition for the /npa/scheduler node.
 
Copyright (c) 2010-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.mpss/3.10/power/npa/src/scheduler/lpr_definition_npa_scheduler.c#1 $
============================================================================*/
#ifdef NPA_SCHEDULER_SLEEP_LPR_SUPPORTED
/* Only needed if Sleep LPR is defined */

#include "npa_scheduler_internal.h"
#include "npa_remote_rpm_protocol.h"
#include "CoreThread.h"
#include "CoreMacros.h"
#include "timer.h"
        
/* NPA_SCHEDULER_USES_SLEEP_OS_H is defined in Sconscript */        
#ifdef NPA_SCHEDULER_USES_SLEEP_OS_H
#include "sleep_os.h"
#define GET_HARD_DEADLINE sleepOS_getHardDeadline()
#else
#include "sleep_v.h"
#define GET_HARD_DEADLINE sleep_get_hard_deadline()
#endif
        
#define NPA_SCHEDULER_LPR_NODE_NAME "npa_scheduler"

/* LPR Definitions generated by Sleep via NPA Scheduler XML file */
extern sleep_lprm SleepLPR_npa_scheduler_modes[];
extern sleep_lpr  SleepLPR_npa_scheduler;
   
/**
 * @brief npa_scheduler_calc_wakeup
 * 
 * Calculate the expected sleep absolute wakeup time.
 *
 * Note that the hard time is calculated as when Sleep will be
 * finished whereas soft time is when Sleep will wake up.  This
 * should not be an issue though as Scheduler's desired wakeup
 * will always be at or after the hard wakeup.
 * 
 * @param start_time: Time to use as base for absolute calculations.
 * @ soft_time_ptr: Passback of calculated soft time.
 * @ hard_time_ptr: Passback of calculated hard time.
 *
 * @return: Earliest expected wakeup time.
 */
static uint64 npa_scheduler_calc_wakeup(uint64 start_time,
                                        uint64 *soft_time_ptr,
                                        uint64 *hard_time_ptr)
{
  uint64 wakeup; 
  uint64 soft = UINT64_MAX; /* soft wakeup time not set */
  uint32 soft_duration; /* soft wakeup duration from now */
  npa_query_type query_result;
  
  /* assume hard is nearest */
  *hard_time_ptr = wakeup = GET_HARD_DEADLINE;

  /* Get duration until soft time */
  CORE_VERIFY( NPA_QUERY_SUCCESS 
                 == npa_query( npa_scheduler.sleep.cpu_wakeup_query_handle,
                               SLEEP_QUERY_WAKEUP_TIME,
                               &query_result ) );
  soft_duration = query_result.data.value;

  /* Check to see if sleep's max duration resource is available */
  if( npa_scheduler.sleep.max_duration_query_handle )
  {
    CORE_VERIFY( NPA_QUERY_SUCCESS 
                   == npa_query( npa_scheduler.sleep.max_duration_query_handle,
                                 NPA_QUERY_CURRENT_STATE,
                                 &query_result ) );
    soft_duration = MIN( soft_duration, query_result.data.value );
  }

  /* see if soft time should be used */
  if (NPA_MAX_STATE != soft_duration)
  { /* have a valid soft time and not a "negative" number */
    soft = start_time + (uint64) soft_duration; /* calculate absolute time */

    if (npa_compare_times(soft, wakeup) < 0 )
    { /* soft time is sooner than hard time so use it */
      wakeup = soft;
      ++npa_scheduler.stats.soft_wakes_used;
    }
  }
  *soft_time_ptr = soft;

  return(wakeup);
}
   
/**
 * @brief npa_scheduler_lpr_enter
 * Called when entering sleep. We issue NAS requests here
 */
void npa_scheduler_lpr_enter( uint64 duration )
{
  uint64 start_time = CoreTimetick_Get64();
  uint64 func_duration;
  uint64 wakeup_time;
  uint64 soft_time;
  uint64 hard_time;

  ++npa_scheduler.stats.lpr_enter;

  /* Made it to sleep, clear the implied_completes_planB_list */
  npa_list_Clear( &npa_scheduler.implied_completes_planB_list );
  
  npa_scheduler.sleep.enter = 1;

  /* Calc wakeup Time */
  wakeup_time = npa_scheduler_calc_wakeup(start_time,
                                          &soft_time,
                                          &hard_time);
  npa_scheduler.sleep.duration = duration;
  npa_scheduler.sleep.wakeup_time_ticks = wakeup_time;

  /* fork all pending scheduled requests with respect to the
   * expected sleep exit time and do implied cancel if
   * any are expected.
   */
  npa_run_scheduler_forks(wakeup_time);

  /* calculate function duration and record max */
  func_duration = npa_calc_duration(start_time,
                                    CoreTimetick_Get64(),
                                    &npa_scheduler.stats.lpr_entry_duration);

  ULOG_RT_PRINTF_7(npa_scheduler.log, 
                   "DONE npa_scheduler_lpr_enter"
                   " (wakeup_time: 0x%016llx)"
                   " (soft_time: 0x%016llx)"
                   " (hard_time: 0x%016llx)"
                   " (function duration: 0x%08x)",
                   ULOG64_LOWWORD(wakeup_time),
                   ULOG64_HIGHWORD(wakeup_time),
                   ULOG64_LOWWORD(soft_time),
                   ULOG64_HIGHWORD(soft_time),
                   ULOG64_LOWWORD(hard_time),
                   ULOG64_HIGHWORD(hard_time),
                   (uint32)func_duration);
}
             
/**
 * @brief npa_scheduler_lpr_exit
 * Called when exiting sleep.
 */
void npa_scheduler_lpr_exit( void )
{
  uint64 start_time = CoreTimetick_Get64();
  uint64 func_duration;
  int64 time_to_request;

  ++npa_scheduler.stats.lpr_exit;

  /* just a safety net */
  CORE_VERIFY(npa_scheduler.sleep.enter == 1);

  /* calculate time to next timeout */
  time_to_request = npa_compare_times(npa_scheduler.timer.current_timeout,
                                      CoreTimetick_Get64());

  if (0 != npa_scheduler.forked_queue.num_linked_clients
      || (0 != npa_scheduler.request_queue.num_linked_clients
          && time_to_request 
               <= (int) npa_scheduler.config_ptr->min_reschedule_delta)
      || !npa_list_IsEmpty(&npa_scheduler.implied_completes_on_wakeup_list))
  { /* have requests to handle or deferred implied_completes to execute */
    /* disable the timer since NPA Scheduler is signalled from here */
    npa_scheduler_disable_timer();

    /* block timer from running but make sure only do it once */
    CORE_VERIFY( ++npa_scheduler.timer.deferred_match_count <= 1 );

    timer_defer_match_interrupt_64();

#ifndef WINSIM
    /* NAS requests that didn't fit in the fifo and sleep/semi-active set
     * updates resulting from NAS requests that did fit, are deferred until
     * next wakeup. The latter because applying them inline with the NAS
     * request would update the sleep/semi-active sets based on the next,
     * rather than current active set. Apply those updates here.
     */
    ULOG_RT_PRINTF_0( npa_scheduler.log, "Applying deferred updates.." );
    npa_rpm_do_deferred_updates();
#endif
    
    /* Assign the scheduler the same priority as sleep (current thread) to
       prevent a context switch from the below signal */
    Core_SetThreadPriority(npa_scheduler.thread_id,
                           Core_GetThreadSelfPriority());

    /* Signal the scheduler */
    npa_scheduler_signal_task(NPA_SCHEDULER_ACTION_LPR_WAKEUP);
  }
  
  /* done processing */
  npa_scheduler.sleep.enter = 0;

  /* calculate function duration and record max */
  func_duration = npa_calc_duration(start_time,
                                    CoreTimetick_Get64(),
                                    &npa_scheduler.stats.lpr_exit_duration);

  /* TODO - Sleep logging may do this already.  Remove if so. */
  ULOG_RT_PRINTF_4( npa_scheduler.log,
                    "DONE npa_scheduler_lpr_exit"
                    " (desired_time: 0x%016llx)"
                    " (function duration: 0x%08x)"
                    " (time_to_request: 0x%08x)",
                    ULOG64_LOWWORD(npa_scheduler.timer.current_timeout),
                    ULOG64_HIGHWORD(npa_scheduler.timer.current_timeout),
                    (uint32)func_duration,
                    (uint32)time_to_request);
}


/**
 * @brief npa_scheduler_max_duration_available_callback
 * 
 * This callback will be invoked when sleeps max duration resource
 * becomes available. If the resource becomes available we will create
 * a query handle for determining the soft wakeup time duration.
 * 
 */
static void
npa_scheduler_max_duration_available_callback( void         *context,
                                               unsigned int  event_type, 
                                               void         *data,       
                                               unsigned int  data_size )
{
    /* Create a query handle to get max_duration. We need this to accurately
       determine the soft wakeup time */
    CORE_VERIFY_PTR( 
      npa_scheduler.sleep.max_duration_query_handle
        = npa_create_query_handle( NPA_SCHEDULER_SLEEP_MAX_DURATION_RESOURCE_NAME ) );
}

/**
 * @brief scheduler_lpr_init
 * 
 * Init function for the Scheduler LPR. 
 * 
 * Initialize the Scheduler LPR and then enables itself.
 */
void npa_scheduler_lpr_init( void )
{
  if (npa_sleep_lpr_supported)
  { /* feature has not been turned off */
    CORE_VERIFY_PTR( 
      npa_scheduler.sleep.lpr_client 
        = npa_create_sync_client( NPA_SCHEDULER_SLEEP_LPR_RESOURCE_NAME, 
                                  "/npa/scheduler/lpr",
                  NPA_CLIENT_REQUIRED ) );

    /* register with sleep */
    sleep_define_lpr_str("npa_scheduler", npa_scheduler.sleep.lpr_client);

    /* create CPU Wakeup Client for getting info to determine soft wakeup time */
    CORE_VERIFY_PTR( 
      npa_scheduler.sleep.cpu_wakeup_query_handle
        = npa_create_query_handle( NPA_SCHEDULER_SLEEP_CORE_CPU_WAKEUP_RESOURCE_NAME ) );
    
    /* Set up a callback to create a query handle if Sleep's max_duration
       resource is available */
    npa_resource_available_cb( NPA_SCHEDULER_SLEEP_MAX_DURATION_RESOURCE_NAME,
                               npa_scheduler_max_duration_available_callback,
                               NULL );
  }
}

#endif /* NPA_SCHEDULER_SLEEP_LPR_SUPPORTED */
