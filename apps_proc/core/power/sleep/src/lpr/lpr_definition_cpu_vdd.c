/*============================================================================
  FILE: lpr_definition_cpu_vdd

  LPR definition file, autogenerated by SleepSynth.
============================================================================*/
#include <stdint.h>
#include "comdef.h"
#include "sleep_lpr.h"
#include "DALDeviceId.h"
#include "DDIClock.h"
#include "ClockDefs.h"
#include "CoreVerify.h"
#include "sleep_target.h"
#include "CoreTime.h"
#include "npa.h"

#define PSCI_POWER_STATE(reset) (reset << 30)

/**
 * If this flag is set, we will do rpm assited power collapse
 */
extern int rpm_assisted_power_collapse;

/*
 * PSCI state ID information of CPUs.
 * This is target specific. This might change from target to target
 * based on number of CPU/L2 lprms are supported.
 */
typedef enum{
  PSCI_ID_CPU_CLK_GATE                = 1,
  PSCI_ID_CPU_GDHS_L2_RET             = 3,
  PSCI_ID_CPU_GDHS_L2_NORET           = 4,
  PSCI_ID_CPU_GDHS_L2_NORET_RPM_HS    = 8
}psci_state_id;

/**
 * Handle for clk regime calls
 */
static DalDeviceHandle *cpu_clock_handle;

static void psci_state_id_encode(cpu_lprm cpu_mode, int *state_id, int reset)
{
    int power_state = PSCI_POWER_STATE(reset);
  
    /*
     * Prepare the PSCI state ID information
     */
    switch(cpu_mode)
    {
      case CPU_CLK_GATE:
        break;

      case CPU_GDFS_RET:
        *state_id |= (power_state | PSCI_ID_CPU_GDHS_L2_RET);
        break;

      case CPU_GDFS_NORET:
        if (rpm_assisted_power_collapse)
          *state_id |= (power_state | PSCI_ID_CPU_GDHS_L2_NORET_RPM_HS);
        else
          *state_id |= (power_state | PSCI_ID_CPU_GDHS_L2_NORET);
        break;
    }

    return;
}

static void cpu_vdd_pc_enter
(
  cpu_lprm  cpu_mode,
  l2_lprm l2_mode,
  uint32 pc_flags,
  uint64 wakeup_tick
)
{
  uint64 curr_tick;
  sleepStats_wakeup_reason  reason;
  int psci_state_id = 0x0;

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  uint64 enter_ticks = CoreTimetick_Get64();
#endif

  /* Programming the SPM for GDFS_NORET mode */

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  sleepStats_putMiscData( l2_mode, SLEEP_STATS_MISC_PC_L2_MODE_TYPE );

  curr_tick = CoreTimetick_Get64();
  sleepStats_putLprTimeData( curr_tick, SLEEP_STATS_TIME_MSTR_SHUTDOWN_TYPE );

  enter_ticks = curr_tick - enter_ticks;
  sleepStats_putLprLatency( (uint32)enter_ticks, 
                            SLEEP_STATS_ENTER_LATENCY_TYPE );
#endif

  /*
   * PSCI state ID encode
   */
  psci_state_id_encode(cpu_mode, &psci_state_id, 0x1);

  if( rpm_assisted_power_collapse )
  { 
    pc_flags = pc_flags | CLOCK_SLEEP_FLAG_RPM_ASSISTED_PC;
  }

  DalClock_ProcessorSleep( cpu_clock_handle, 
                           CLOCK_SLEEP_MODE_POWER_COLLAPSE,
                           pc_flags );

  /* Record the wakeup time as early as possible */
  curr_tick = CoreTimetick_Get64();

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  sleepStats_putLprTimeData( curr_tick, SLEEP_STATS_TIME_MSTR_RETURN_TYPE );
#endif

  /* Logging for standalone power collapse */
  if( !rpm_assisted_power_collapse )
  {
    /* Set standalone wakeup reason based on current time and requested 
     * scheduled wakeup time */    
    if( curr_tick > wakeup_tick )
    {
      reason = SLEEP_STATS_WAKEUP_SA_SCHEDULED;
      curr_tick = wakeup_tick;
    }
    else
    {
      reason = SLEEP_STATS_WAKEUP_SA_UNSCHEDULED;
    }

    /* Set stand alone PC wakeup reason */
    sleepStats_putMiscData( reason, SLEEP_STATS_MISC_WAKEUP_REASON_TYPE );

    /* Set wakeup time as either the qtmr value, or the earliest possible 
     * point in return to master code 
     * (RPM assisted will overwrite value in RPM assisted cases) */
    sleepStats_putLprTimeData( curr_tick, SLEEP_STATS_TIME_WAKEUP_TYPE );
  }
}

/* Client for setting RPM driver to polling mode */
static npa_client_handle sleep_rpm_polling_mode_client;

static void sleep_create_rpm_polling_mode_CB( void *context,
                                                          unsigned int event_type,
                                                          void *data,       
                                                          unsigned int data_size)
{
  sleep_rpm_polling_mode_client =  npa_create_sync_client( "/protocol/rpm/polling_mode",
                                                            "sleep_rpm_polling_mode_client",
                                                            NPA_CLIENT_REQUIRED );

  CORE_VERIFY_PTR( sleep_rpm_polling_mode_client );
}


/**
 * Initializes CPU_VDD LPR. This function must be called before any of the
 * modes from this LPR is entered.
 */
void cpu_vdd_lpr_init(void)
{
  /* create a client to the rpm polling mode resource when it is available */
  npa_resource_available_cb( "/protocol/rpm/polling_mode",
                             sleep_create_rpm_polling_mode_CB,
                             NULL );
	
  /* Attach a DAL handle to clkregime for power collapse */
  CORE_DAL_VERIFY( DAL_DeviceAttach( DALDEVICEID_CLOCK, &cpu_clock_handle ) );
}

/**
 * Enter function for power collapse with non retention mode
 */
void cpu_vdd_pc_no_retention_enter(uint64_t wakeup_tick)
{
  /*
   * These flags are still needed because NSec has to flush it caches
   * before it issue SMC system call
   */
  uint32 pc_flags = CLOCK_SLEEP_FLAG_RAMP_DOWN_CPU |
                    CLOCK_SLEEP_FLAG_L2CACHE_FLUSH ;

  cpu_vdd_pc_enter( CPU_GDFS_NORET, L2_NORET, pc_flags, wakeup_tick );
}

/**
 * Enter function for power collapse with retention mode. Right now it looks
 * like clock regime call for both power collapse modes will be same. If
 * that's true, we can simplify enter/exit functions a little bit.
 */
void cpu_vdd_pc_retention_enter(uint64_t wakeup_tick)
{
  cpu_vdd_pc_enter( CPU_GDFS_RET, L2_RET,
                    CLOCK_SLEEP_FLAG_RAMP_DOWN_CPU, wakeup_tick );
}

/**
 * Exit function for power collapse low power modes.
 */
void cpu_vdd_pc_exit(void)
{
    /* Issue request to the "/protocol/rpm/polling_mode" resource to switch the 
     NPA remoting driver into polling mode so that clock requests going to RPM
     do not call barrier since we are in INTLOCK'd context here */
  npa_issue_required_request( sleep_rpm_polling_mode_client, 1 );
  
  DalClock_ProcessorRestore( cpu_clock_handle, 
                             CLOCK_SLEEP_MODE_POWER_COLLAPSE, 0 );
							 
  npa_complete_request( sleep_rpm_polling_mode_client );							 

}
