/*============================================================================
  FILE:         sleep_os.c
 
  OVERVIEW:     This file provides the sleep task and OS-specific 
                functionality for the processors with REX running on them.
 
  DEPENDENCIES: sleep_task() is the entry point to the lowest-priority thread
                processing.  sleep_init() should be called before using any
                other function in the sleep subsystem.
 
                Copyright (c) 2012-2019 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

  $Header: //components/rel/core.tx/6.0/power/sleep/src/os/l4/sleep_os.c#1 $
============================================================================*/

#include <stdint.h>
#include "rcinit.h"
#include "npa.h"
#include "CoreTime.h"
#include "CoreVerify.h"
#include "DDITimetick.h"
#include "timer.h"
#include "sleep_config.h"
#include "CoreConfig.h"
#include "DDIInterruptController.h"
#include "dog.h"
#include "rpmclient.h"
#include "sleepi.h"
#include "sleep_target.h"
#include "sleep_lpr.h"
#include "sleep_log.h"
#include "sleep_solver.h"
#include "sleep_v.h"
#include "sleep_npa_scheduler.h"
#include "lookup_table_solver.h"
#include "sleep_idle_plugin.h"
#include "qurt_interrupt.h"
#include "qurt_init.h"
#include "qurt_system.h"
#include "qurt_types.h"

/*=======================================================================
 *                                 DEFINES
 *======================================================================*/

/* Following three defines can go to common files as each os file is using 
 * that */
/* Convert sclk unit (32768 Hz) values into tick unit values (19.2 MHz) values
   32768 sclks = 19.2e6 ticks :: x     sclks = y      ticks  */
/* 8974 will tick at 19.2Mhz/586 during XO shutdown, so use 586 in conversion */

#define SCLK_TO_TICKS(sclks)  ((sclks)*586)
#define SAFE_TO_UINT32(val64) ((val64) > UINT32_MAX)? (UINT32_MAX) : ((uint32)(val64))

/**
 * @brief Enumeration defining bitmasks for actions Sleep tasks need to 
 *        perform outside of INT/TASK locked context.
 */
enum
{
  SLEEP_OS_ADD_FLUT = 1 << 0,          /* Add a frequency lookup table */
  SLEEP_OS_LOG_STATS = 1 << 1,         /* Log Sleep statistics */
};

/*========================================================================== 
 *                       EXTERNAL DECLARATIONS
 *=========================================================================*/

extern void sleep_target_swfi( void );
extern uint64 safe_unsigned_addition(uint64 a, uint64 b);
void sleep_holdoff_reg_rcinit_comp_event( void );

extern DalDeviceHandle *sleepIntHandle;

/* ==================================================================
                        GLOBAL VARIABLES
   ================================================================== */              
/**
 * @brief threadx_min_duration
 * 
 * Minimum sleep duration provided by Qurt 
 */
static qurt_time_t threadx_min_duration;

/**
 * @brief sleep_cfg
 * 
 * Returned by sleep_target_init()
 */
static sleep_target_config* sleep_cfg;

/**
 * @brief gSleepLatencyNode
 * 
 * Handle used to query the /core/cpu/latency node.
 */
static npa_query_handle gSleepLatencyNode;

/**
 * @brief gSleepLprNode
 * 
 * Handle used to query the /sleep/lpr node.
 */
static npa_query_handle gSleepLprNode;

/**
 * @brief gClkCpuHandle
 *
 * Client for /clk/cpu requesting desired frequency during sleep
 */
static npa_client_handle gClkCpuHandle;

/**
 * @brief gClkCpuQueryHandle
 *
 * Query handle used to know the status of /clk/cpu resource to determine
 * sleep enter and exit time.
 */
static npa_query_handle gClkCpuQueryHandle;

/** 
 *  @brief gSleepCLprmNotification
 *
 * The CLprm to notify if the sleep duration changes.  This 
 * duration could be updated by an enter function. 
 */
CLprm *gSleepCLprmNotification;

/** 
 *  @brief gSleepHardDeadline
 *
 * The hard deadline (expected sleep exit time) that is 
 * currently being used by the idle code, in absolute ticks.  It
 * only makes sense to query this value during the idle context 
 * (i.e., from an LPR). 
 */
static uint64 gSleepHardDeadline;

/**
 * @brief gLastTimerDeadline
 *
 * The deadline (in absolute ticks) of the last scheduled timer
 * wakeup that was calcaulated by sleep.
 */
static uint64 gLastTimerDeadline;

/**
 * @brief gSleepCycleID
 *
 * Indicates a particular sleep cycle. It is rather simply a counter that
 * is incremented every time latency and power function parameters are 
 * changed. This counter is somewhat different than above one. This will 
 * not be allowed to have 0 as valid value. Also, it will be incremented 
 * only when sleep is enabled on corresponding core.
 */
static uint32 gSleepCycleID = 0;

/**
 * @brief sleep_idle_entry_ptr
 *
 * Pointer to use as the entry point when the system is idle.
 * This pointer is controlled by the /sleep/idle/plugin node.
 */
static sleep_idle_entry_ptr_type sleep_idle_entry_ptr = NULL;

/**
 * @brief sleep_allow_low_power_modes
 *
 * Boolean that indicates whether we should enter any low power modes
 * or not.  This is meant to be used by developers, and set
 * through the debugger, for disabling all low power modes from
 * occurring.
 */
volatile boolean sleep_allow_low_power_modes = TRUE;

/* ==================================================================
                  INTERNAL FUNCTION DEFINITIONS
   ================================================================== */

/**
 * sleep_clk_cpu_avail_cb
 *
 * @brief Obtains the query handle for /clk/cpu node when it becomes
 *        available. 
 */
static void sleep_clk_cpu_avail_cb( void *context,
                                    unsigned int event_type,
                                    void *data,
                                    unsigned int data_size )
{
  CORE_VERIFY_PTR( gClkCpuHandle = npa_create_sync_client( 
                                      "/clk/cpu",
                                      "SleepFreqClient",
                                      NPA_CLIENT_REQUIRED) );

  /* Creating a query handle on the resource */
  CORE_VERIFY_PTR( gClkCpuQueryHandle = npa_create_query_handle("/clk/cpu") );
  
}

/**
 * @brief sleep_enabled
 * 
 * Check various conditions to determine whether any low power 
 * modes are allowed or not. 
 */
static inline boolean sleep_enabled( void )
{
  /* We may need to add other conditions eventually */
  if ( !sleep_allow_low_power_modes )
  {
    return FALSE;
  }

  return TRUE;
}

/**
 *
 * @brief sleep_os_get_durations 
 *  
 * Get the hard duration, soft duration, and latency budget to 
 * be used for idle processing. 
 *  
 * @param *hard_duration:  [output] Buffer to write the hard 
 *                         duration.
 * @param *soft_duration:  [output] Buffer to write the soft 
 *                         duration.
 * @param *latency_budget: [output] Buffer to write the latency 
 *                         budget.
 * 
*/
static void sleep_os_get_durations( uint64 *hard_duration, 
                                    uint64 *soft_duration, 
                                    uint32 *latency_budget )
{
  npa_query_type qres;
  uint64 qres64;
  
  /* Query the hard wake-up time using the os abstraction function.
   * NPA queries only support 32 bit return values for scalar resources,
   * We can switch back to using NPA once NPA provides 64-bit support */
  *hard_duration = sleep_os_get_hard_wakeup_time();

  /* Set to max. since wakeup node is not supported. */
  *soft_duration = (uint64)(-1);

  /* Get the latency budget from the latency node.  The latency node will
   * return the minimum latency budget being requested by all clients.
   * This lets us know how quickly we need to respond to an interrupt when
   * it occurs. */
  CORE_VERIFY( NPA_QUERY_SUCCESS == npa_query( gSleepLatencyNode,
                                               NPA_QUERY_CURRENT_STATE,
                                               &qres ) );

  qres64 = SCLK_TO_TICKS((uint64)qres.data.value);
  *latency_budget = SAFE_TO_UINT32(qres64);
  
}

/**
 *
 * @brief sleep_os_not_enough_time
 *
 * Determine whether there is enough time to perform the entire
 * sleep processing or not.
 *
 * @param min_duration:   The expected sleep duration.
 * @param latency_budget: The latency budget for this sleep
 *                        cycle.
 *
 * @return TRUE if there is not enough time for sleep, otherwise
 *         FALSE.
 * 
*/
static boolean sleep_os_not_enough_time( uint64 duration,
                                         uint32 latency_budget,
                                         uint64 min_enter_exit_time )
{

  if( duration <= ( sleep_cfg->entry_ticks +
                    sleep_cfg->exit_ticks + 
                    min_enter_exit_time ) )
  {
    /* By the time we run through all of sleep, the timer will
     * have expired, so it's not worth it. */
    return TRUE;
  }

  if( duration <= ( sleep_cfg->entry_ticks + SLEEP_MIN_MATCH_VAL_DELTA ) )
  {
    /* By the time we enter sleep and try to set the backoff time, we'd
     * be setting it after the current match value, so it's not worth it. */
    return TRUE;
  }

  if ( latency_budget <= ( min_enter_exit_time + sleep_cfg->exit_ticks ) )
  {
    /* If an interrupt were to fire after the last check for interrupts,
     * we'd violate our latency budget, so bail out. */
    return TRUE;
  }

  return FALSE;
}

/* Hook for calling sleep functionaliy (WFI or power collapse) */
static void sleep_qurt_idle_hook_CB (qurt_time_t threadx_time) 
{

  threadx_min_duration = threadx_time;
  if ( NULL != sleep_idle_entry_ptr )
  {
    sleep_idle_entry_ptr();
  } 

  return;
}

/* ==================================================================
              EXTERNAL FUNCTION DEFINITIONS
   ================================================================== */

/**
 * 
 * @brief sleep_init
 * 
 * Initialize the sleep subsystem.
 *
*/
void sleep_init( void )
{
  npa_client_handle sleep_os_idle_plugin_client = NULL;

  /* Initialize the logging subsystem. */
  sleep_log_init();
  sleep_log_set_level( SLEEP_LOG_LEVEL_DEBUG );

  /* Initialize target-independent NPA nodes. */
  sleep_npa_init();

  /* Initialize LPR / LPRM tracking subsystem. */
  sleep_lpr_init();

  /* Initialize statistical data system */
  sleepStats_init();

  /* Set the initial solver configuration. This needs to be done before any
   * possible access to the fLUT related data */
  sleep_solver_set(&sleep_lookup_table_solver);

  /* Initialize target-dependent stuff (nodes, modes, etc.). */
  CORE_VERIFY_PTR( sleep_cfg = sleep_target_init() );

  /* Register for RCInit completion event */
  sleep_holdoff_reg_rcinit_comp_event();

  /* Try to get query handles for the resources we just created.  These should
     be ready immediately, but check them anyway. */
  CORE_VERIFY_PTR( gSleepLprNode = 
                   npa_create_query_handle( "/sleep/lpr" ) );


  CORE_VERIFY_PTR( gSleepLatencyNode = 
                   npa_create_query_handle( "/core/cpu/latency" ) );
  
  /* Registering a call back function to the resource to obtain its client
   * and query handles */  
  npa_resource_available_cb("/clk/cpu", sleep_clk_cpu_avail_cb, NULL);

  /* Register the time that the sleep code takes with the idle plugin node,
   * so it can decide whether to do SWFI-only based on the latency
   * restrictions */
  CORE_VERIFY_PTR( sleep_os_idle_plugin_client =
                   npa_create_sync_client(
                                "/sleep/idle/plugin",
                                "SleepOsLatencyClient",
                                SLEEP_IDLE_PLUGIN_CLIENT_MIN_SLEEP_LATENCY ) );

  /* Issue number of ticks the sleep code takes to execute.  This is the
   * worst-case number seen in the logs. */
  npa_issue_required_request( sleep_os_idle_plugin_client,
                              sleep_cfg->entry_ticks + 
                              sleep_cfg->exit_ticks );
							  
  /* Registering hook with qurt_idle_task which would be called
   * when system goes idle */
   qurt_power_register_idle_hook(&sleep_qurt_idle_hook_CB);
}

/*
 * sleep_os_get_last_timer_deadline
 *
*/
uint64 sleep_os_get_last_timer_deadline( void )
{
  return gLastTimerDeadline;
}

/*
 * sleep_os_offset_sleep_duration
 *
*/
void sleep_os_offset_sleep_duration( uint64 offset )
{
  /* Adjust the sleep duration, so that the new duration gets passed
   * to the rest of the enter functions. */
  if( NULL != gSleepCLprmNotification )
  {
    offsetSleepDuration( gSleepCLprmNotification, offset );
  }

  /* Adjust the hard deadline as well, in order to correctly calculate
   * whether we woke up late before exiting sleep. */
  gSleepHardDeadline += offset;
}

/*
 * sleep_get_hard_deadline
*/
uint64 sleep_get_hard_deadline( void )
{
  return gSleepHardDeadline;
}

/*
 * sleep_os_get_hard_wakeup_time
 *
*/
uint64 sleep_os_get_hard_wakeup_time( void )
{
  /* Time (in ticks) until the next scheduled timer expiry */
  uint64 ticks_to_timer_expiry;
  uint64 hard_duration;

  /* Retrieve the time (in ticks) until the next scheduled timer expiry.
   * This will skip timers that are deferrable, and push the timer interrupt
   * out to fire at the next non-deferrable timer. */
  ticks_to_timer_expiry = timer_get_time_till_first_non_deferrable_timer_64();

  gLastTimerDeadline = safe_unsigned_addition(CoreTimetick_Get64(),
                                              ticks_to_timer_expiry);

  sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 4,
                    "Hard wakeup durations (ATS Timer Duration: 0x%llx) (TX Timer Duration: 0x%llx)",
                    ULOG64_DATA(ticks_to_timer_expiry),
                    ULOG64_DATA(threadx_min_duration));

  hard_duration = MIN(ticks_to_timer_expiry, threadx_min_duration);
  
  return hard_duration;
}

/*
 * sleep_os_set_idle_entry
 */
void sleep_os_set_idle_entry( uint32 core_num,
                              sleep_idle_entry_ptr_type new_idle_entry_ptr )
{
  sleep_idle_entry_ptr = new_idle_entry_ptr;
}

/*
 * sleep_swfi
 */
uint32 sleep_swfi( void )
{
  /* Make sure the idle plugin has not changed, and we should proceed with halt.
   * There is a chance an interrupt could come in
   * after this, but the window is small, and the same risk is there in
   * sleep_perform_lpm. */
  if ( sleep_swfi == sleep_idle_entry_ptr )
  {
    sleep_target_swfi();
  }
  
  /* As Qurt would have already disabled interrupts when idle hook is called,
   * interrupts need to be enabled here */
  qurt_power_shutdown_exit();
	
  return 0;
}

/* 
 * sleep_perform_lpm
 *
*/ 
uint32 sleep_perform_lpm(void)
{
  /* min of soft and hard sleep durations */
  uint64 min_duration; 
  uint32 num_enabled_lprs, min_latency;
  /* record of sleep statistics */
  uint64 sleep_start;
  /* sleep solver io */
  sleep_solver_input solver_input;
  sleep_solver_output solver_output;
  npa_query_type qres;
  uint32 is_interrupt_pending = 0;
  uint64 sleep_duration;
  uint64 backoff_deadline = 0;
  CLprm *solver_selected_mode = NULL;
  DALResult eResult;
  uint64 hard_duration, soft_duration, sleep_soft_deadline;
  uint64 time_til_deadline, curr_time;
  uint32 cpu_freq = 0;
  sleepStats_input stats;

  /* Indicates if any action needs to be perform outside INT/TASK locked */
  uint32 non_stm_actions = 0;
  unsigned long interrupt_flag = 0;

  /* Check if low power modes are allowed 
   * Also, check if the idle plugin has changed */
  if ( !sleep_enabled() ||
       (sleep_perform_lpm != sleep_idle_entry_ptr) )
  {
    /* As Qurt would have already disabled interrupts when idle hook is called,
     * interrupts need to be enabled here */
    qurt_power_shutdown_exit();
    return 0;
  }

  /* Tasklock & Intlock so that sleep_perform_lpm is single-threaded. */
  INTLOCK_SAVE( interrupt_flag );

  dog_autokick( TRUE );
  sleep_log_printf_qdss( SLEEP_LOG_LEVEL_PROFILING, SLEEP_ENTER_IDLE_NUM_ARGS,
                         SLEEP_ENTER_IDLE_STR, SLEEP_ENTER_IDLE );

  do                    /* Undo int/task locking */
  {
    /* If an RPM message is in flight, immediately SWFI. */
    if( FALSE == rpm_is_mq_empty() )
    {
      sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 0, 
                        "Short SWFI (reason: RPM message in flight)" );
      sleep_target_swfi();
      break;
    }

    /* Make sure that idle plugin has not changed since last checked */    
    if( sleep_perform_lpm != sleep_idle_entry_ptr )
    {
      sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 0, "Sleep cycle skipped "
                        "(reason: Idle Plugin state changed)" );
      break;
    }    

    /* Record the sleep start */
    sleep_start = CoreTimetick_Get64();
  
    /* Querying sleep node for duration and latency constraints */
    sleep_os_get_durations( &hard_duration, &soft_duration,
                            &solver_input.latency_budget );
  
    /* Calculate the expected wake up time to detect a late wakeup later */
    gSleepHardDeadline = safe_unsigned_addition( sleep_start, hard_duration );
    sleep_soft_deadline = safe_unsigned_addition( sleep_start, soft_duration );

    min_duration = MIN( hard_duration, soft_duration );
  
    eResult = DalInterruptController_IsAnyInterruptPending(sleepIntHandle, 
                                                     &is_interrupt_pending);
    CORE_VERIFY(eResult == DAL_SUCCESS);
  
    if ( min_duration == 0 || is_interrupt_pending )
    {
      /* Log and exit immediately, since a wakeup condition has already 
       * occurred. */
      sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, (2*1)+1, 
                        "Early wakeup (min_duration: 0x%llx)"
                        " (ints pending: %s)", ULOG64_DATA(min_duration),
                        is_interrupt_pending ? "true" : "false" ); 
      break;
    }
  
    /* Get the current LPR registry. */
    CORE_VERIFY(NPA_QUERY_SUCCESS == npa_query( gSleepLprNode,
                                                SLEEP_LPR_QUERY_RESOURCES,
                                                &qres ) );
  
    CORE_VERIFY( NPA_QUERY_TYPE_REFERENCE == qres.type );
    solver_input.lpr_registry = (CLprRegistry *)qres.data.reference;
  
    /* Storing sleep cycle id for caching purpose */
    if( 0 == ++gSleepCycleID )
    {
      /* Looks like roll-over happened. */
      gSleepCycleID = 1;
    }
    solver_input.cycle_id = gSleepCycleID;
  
    /* Requesting max frequency for /clk/cpu before entering into modes */
    if( gClkCpuHandle )
    {
      npa_issue_required_request( gClkCpuHandle, sleep_cfg->freq_floor_khz );
    }

    /* Get the current CPU frequency. */
    if ( gClkCpuQueryHandle )
    {
      CORE_VERIFY( NPA_QUERY_SUCCESS == npa_query( gClkCpuQueryHandle,
                                                   NPA_QUERY_CURRENT_STATE,
                                                   &qres ) );
      cpu_freq = qres.data.value;
    }


    do                  /* Undo freq request and int/task locking */
    {
      /* Update frequency table pointer to match current core frequency.
       * Either an exact fLUT match or the closest fLUT will be returned */
      if(sleep_lookup_table_get_entry(cpu_freq, &stats.pfreq_LUT) != TRUE) 
      {
        non_stm_actions |= SLEEP_OS_ADD_FLUT;

        break;
      }

      num_enabled_lprs =
        CLprRegistry_getNumberEnabledLprs( solver_input.lpr_registry, 0 );
    
      min_latency =
        CLprRegistry_getMinEnabledModeLatency( solver_input.lpr_registry,
                                               min_duration,
                                               stats.pfreq_LUT,
                                               gSleepCycleID );

      /* Perform early test for corner cases */
      if( num_enabled_lprs == 0 ||
          sleep_os_not_enough_time( min_duration,
                                    solver_input.latency_budget,
                                    min_latency ) )
      {
        /* Either no LPRMs are enabled or
         * there's not enough time to do the whole sleep process.
         * Therefore, immediately SWFI until the wakeup, and then exit. */
        sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, (2*2)+2, 
                          "Short SWFI (hard duration: 0x%llx) "
                          "(soft duration: 0x%llx) (latency budget: 0x%08x) "
                          "(Enabled LPRs: %d)",
                          ULOG64_DATA(hard_duration), ULOG64_DATA(soft_duration),
                          solver_input.latency_budget, num_enabled_lprs );
        sleep_target_swfi();
        break;
      }

      /* Maximum number of modes solver can select if executed */
      solver_input.num_of_allowed_modes = 1;

      /* set current frequency index value for solver */
      solver_input.pfreq_LUT = stats.pfreq_LUT;
    
      /* Fill in solver durations to operate on.  This takes into account the
       * amount of time that's passed since we checked for the durations. */
      curr_time = CoreTimetick_Get64();
      solver_input.hard_duration = gSleepHardDeadline - curr_time;
      solver_input.soft_duration = sleep_soft_deadline  - curr_time;
    
     /* Run the solver.  This will get back a list of all low power modes
      * that can be entered based on these parameters. */
      sleep_log_printf_qdss( SLEEP_LOG_LEVEL_INFO, SLEEP_ENTER_SOLVER_NUM_ARGS,
                             SLEEP_ENTER_SOLVER_STR, SLEEP_ENTER_SOLVER, 
                             cpu_freq, ULOG64_DATA(solver_input.hard_duration),
                             ULOG64_DATA(solver_input.soft_duration),
                             solver_input.latency_budget );

      solver_output.selected_modes = &solver_selected_mode;
      sleep_solver( &solver_input, &solver_output );

      sleep_log_printf_qdss( SLEEP_LOG_LEVEL_INFO, SLEEP_EXIT_SOLVER_NUM_ARGS,
                             SLEEP_EXIT_SOLVER_STR, SLEEP_EXIT_SOLVER );
    
      if ( 0 == solver_output.num_of_selected_modes )
      {
        /* No Modes were selected - Just do clock gating */
        sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 1+(2*2), 
                          "Short SWFI (hard duration: 0x%llx) "
                          "(soft duration: 0x%llx) (latency budget: 0x%08x)",
                          ULOG64_DATA(solver_input.hard_duration), 
                          ULOG64_DATA(solver_input.soft_duration),
                          solver_input.latency_budget );
        sleep_target_swfi();
        break;
      }
    
      /* Check for wakeup sources before entering the modes. */
      eResult = DalInterruptController_IsAnyInterruptPending(sleepIntHandle,
                                                       &is_interrupt_pending);
      CORE_VERIFY(eResult == DAL_SUCCESS);
     
      if ( is_interrupt_pending )
      {
        sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 0, 
                          "Early wakeup (ints pending: true)" );
        break;
      }
     
      /* Make sure there's still time to do the selected mode (add 5 ticks for
       * an extra guardband). */
      time_til_deadline = gSleepHardDeadline - CoreTimetick_Get64();
      if( ( solver_output.enter_time + solver_output.exit_time + 3000 ) >=
            time_til_deadline )
      {
        /* Not enough time:
         * Execute ALL WAIT until interrupt or timer expiry */
        sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 2+(1*2),
                          "Short SWFI (reason: not enough time to enter & exit)"
                          " (enter time: 0x%08x) (exit time: 0x%08x)"
                          " (time til deadline: 0x%llx)",
                          solver_output.enter_time, solver_output.exit_time,
                          ULOG64_DATA(time_til_deadline) );
        sleep_target_swfi();
        break;
      }
    
      /* Entering solver selected Low Power Mode(s) */
      CORE_VERIFY(solver_output.num_of_selected_modes == 1);
    
      /* This will skip timers that are deferrable, and push the timer interrupt
       * out to fire at the next non-deferrable timer. */
      ( void )timer_defer_match_interrupt();
      //sleep_target_defer_timers();
      
      do                /* Undo deferring, freq and locking */
      {
        /* CLprm to notify if sleep duration changes */
        gSleepCLprmNotification = solver_output.selected_modes[0];

        /* Set selected mode for later use in statistics */
        stats.mode = solver_output.selected_modes[0];
      
        /* Calculate the exact duration until the backed off deadline, and
         * pass that to the enter functions. */
        backoff_deadline = gSleepHardDeadline - solver_output.backoff_time;
        sleep_duration = backoff_deadline - CoreTimetick_Get64();
      
        /* Check for wakeup sources before entering the modes. */
        eResult = DalInterruptController_IsAnyInterruptPending(sleepIntHandle,
                                                       &is_interrupt_pending);
        CORE_VERIFY(eResult == DAL_SUCCESS);
       
        if ( is_interrupt_pending )
        {
          sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 0, 
                            "Early wakeup (ints pending: true)" );
          break;
        }

        /* Entering mode - i.e. non returning path with respect to lprms */
        non_stm_actions |= SLEEP_OS_LOG_STATS;

        sleep_log_printf( SLEEP_LOG_LEVEL_INFO, (4*2),
                          "Entering modes (hard deadline: 0x%llx) "
                          "(backoff deadline: 0x%llx) (backoff: 0x%llx) "
                          "(sleep duration: 0x%llx)",
                          ULOG64_DATA(gSleepHardDeadline), 
                          ULOG64_DATA(backoff_deadline),
                          ULOG64_DATA(solver_output.backoff_time),
                          ULOG64_DATA(sleep_duration) );
      
        CLprm_enter( solver_output.selected_modes[0], 
                     backoff_deadline,
                     stats.pfreq_LUT->data_idx );
      
        gSleepCLprmNotification = NULL;

        /* Exiting solver selected Low Power Mode(s) */
        sleep_log_printf(SLEEP_LOG_LEVEL_INFO, 0, "Exiting modes");
      
        CLprm_exit( solver_output.selected_modes[0],
                    stats.pfreq_LUT->data_idx );

      }while(0);        /* Undefer the timer */

      /* Re-enable the timers that were deferred. */
      timer_undefer_match_interrupt();   

    }while(0);          /* Release request on /clk/cpu */

    /* Request is complete */
    if( gClkCpuHandle )
    {
      npa_complete_request( gClkCpuHandle );
    }
    
  }while(0);            /* Unlocking interrupt and tasks */

  stats.sleep_exit_stm = CoreTimetick_Get64();

  sleep_log_printf_qdss( SLEEP_LOG_LEVEL_PROFILING, SLEEP_EXIT_IDLE_NUM_ARGS,
                         SLEEP_EXIT_IDLE_STR, SLEEP_EXIT_IDLE );

  /* Let the watchdog run free, as we're about to release into code. */
  dog_autokick( FALSE );

  /* Marking  */

  /* Intfree and let the great unknown execute. */
  INTFREE_RESTORE( interrupt_flag );

  /* As Qurt would have already locked interrupst when idle hook id called,
     It has to be unlocked here sincew we want the following code to be 
     exectuted outside of STM */
  qurt_power_shutdown_exit();

  if( non_stm_actions )
  {
    if( non_stm_actions & SLEEP_OS_LOG_STATS )
    {
      /* NPA scheduler can update the hard dealine as part of the enter 
       * function. Set backoff deadline to updated value while still in STM */
      stats.backoff_deadline = gSleepHardDeadline - solver_output.backoff_time;

      /* Get sleep wakeup time & reason as recorded from standalone or 
       * RPM modes */ 
      stats.master_interrupt = (uint32)(-1);
      stats.os_overhead.count = 0;
      stats.os_overhead.enter = stats.os_overhead.exit = 0;
      stats.actual_wakeup_time = 
        sleepStats_getLprTimeData( SLEEP_STATS_TIME_WAKEUP_TYPE );
      stats.master_wakeup_reason = (sleepStats_wakeup_reason)
        sleepStats_getMiscData( SLEEP_STATS_MISC_WAKEUP_REASON_TYPE );
      stats.hard_deadline = gSleepHardDeadline;

      /* Log and update any statistic data */
      sleepStats_logWakeup(&stats);
    }

    if( non_stm_actions & SLEEP_OS_ADD_FLUT )
    {
      /* we need to allocate a new fLUT and realloc all of the cache
       * & statistic arrays outside of STM */ 
      sleep_lookup_table_add_fLUT( stats.pfreq_LUT, cpu_freq, 
                                   solver_input.lpr_registry );
    }
  }

  return 0;
}

/* 
 * sleep_os_create_config_handle
 */ 
CoreConfigHandle sleep_os_create_config_handle( unsigned int core_num )
{
#if 0
  char path[MAX_INI_PATH_LEN];

  /* Build the path to the sleep_config.ini file */
  snprintf( path, MAX_INI_PATH_LEN,
            "/nv/item_files/sleep/core%d/sleep_config.ini", core_num );

  return( CoreIni_ConfigCreate( path ) );
#endif
  return NULL;
}

/* 
 * sleep_os_destroy_config_handle
 */ 
void sleep_os_destroy_config_handle( CoreConfigHandle config )
{
  /*CoreIni_ConfigDestroy( config );*/
}

/* 
 * Functions below are for managing locks for the LPRMs in order to serialize
 * their enter and exit function calls during idle time. However, for single
 * core environment they are just stub function and not used as of now. We 
 * have to define them as they are called in common code. Once we have 
 * CoreLock, we can probably use that instead of this.
 */

/*
 * sleep_os_create_mode_fcn_lock
 */
void sleep_os_create_mode_fcn_lock( sleep_lprm *mode )
{
}

/*
 * sleep_fcn_lock
 */
void sleep_fcn_lock( sleep_mode_fn_lock mode_lock )
{
}

/*
 * sleep_fcn_unlock
 */
void sleep_fcn_unlock( sleep_mode_fn_lock mode_lock )
{
}

/**
 *
 * @brief sleep_efs_initialized
 *
 * Initialize the sleep LPR subsystem.  When this callback is
 * called, the EFS subsystem has been initialized.  We rely on
 * that in order to read the config files.
 *
 */
void sleep_efs_initialized( void )
{
#if 0
  boolean sleep_config_value = FALSE;
  unsigned int ret = FALSE;

  /* Check to see if we should do any LPRMs at all. */
  ret = sleep_config_read_bool( 0, ( char * )"all_lprms", NULL,
                                ( char * )"disable", &sleep_config_value );

  if ( ret )
  {
    sleep_allow_low_power_modes = !sleep_config_value;
  }

  if(sleep_allow_low_power_modes == FALSE)
  {
    sleep_log_printf( SLEEP_LOG_LEVEL_WARNING, 0,
                      "WARNING (message: \"All sleep modes are disabled"
                      " in the global config\")" );
  }
#endif
}

/*
 * 
 * @brief sleep_rcinit_shutdown_cb
 * 
 * Callback that gets called when RCINIT receives shutdown signal.
 * When this callback is called, all the low power modes are disabled.
 */
static void sleep_rcinit_shutdown_cb ( void )
{
  sleep_allow_low_power_modes = FALSE;
}

#if 0
/* 
 * sleep_task
 *
 */ 
void sleep_task
(
 /* Parameter received from Main Control task - ignored */
 dword unused_param
 )
{
  /* Start the task */
  rcinit_handshake_startup();
  
  /* Register with RCINIT for shutdown notification via callback */
  rcinit_register_termfn_group( RCINIT_GROUP_7, 
                                sleep_rcinit_shutdown_cb );

  /* Task processing - Enter as many low power modes as possible. */
  while( 1 )
  {
    /* Call the idle entry point.  This function pointer may be modified
     * by the /sleep/idle/plugin resource. */
    if ( NULL != sleep_idle_entry_ptr )
    {
      sleep_idle_entry_ptr();
    }
  }
}
#endif

