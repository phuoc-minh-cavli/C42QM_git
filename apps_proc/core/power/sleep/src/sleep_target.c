/*============================================================================
  FILE:         sleep_target.c
 
  OVERVIEW:     This file provides target-specific functionality for the 8960
                Riva processor.
 
  DEPENDENCIES: None
 
                Copyright (c) 2010-2019 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

  $Header: //components/rel/core.tx/6.0/power/sleep/src/sleep_target.c#1 $
============================================================================*/

#include <stdint.h>
#include "sleep_target.h"
#include "DALDeviceId.h"
#include "CoreTime.h"
#include "CoreVerify.h"
#include "sleep_log.h"
#include "com_dtypes.h"
#include "npa_resource.h"
#include "sleep_lpri.h"
#include "sleepi.h"
#include "timer.h"
#include "DalDevice.h"
#include "sleep_log.h"
#include "DDIInterruptController.h"
#include "ClockDefs.h"
#include "DDIClock.h"
#include "dog.h"
#include "mpmint.h"

/* ==================================================================
                     INTERNAL MACRO DEFINITIONS
   ================================================================== */
/**
 * @brief SLEEP_ENTRY_TICKS
 *
 * Time required from ramping up the frequency to just before entering
 * low power modes. This will change based on current frequency.
 */
#define SLEEP_ENTRY_TICKS 2072

/**
 * @brief SLEEP_EXIT_TIME
 *
 * Time required from removing backoff time to logging wakeup event while
 * exiting from sleep. Like entry time, this will change based on frequency
 * before entering sleep.
 */
#define SLEEP_EXIT_TICKS 1224

/**
 * Timer used to holdoff sleep for a certain amount of time at initial
 * boot-up.
 */
#define SLEEP_INIT_DELAY_MS 10000

/**
 * SLEEP_FREQ_FLOOR_KHZ
 *
 * Indicates the frequency in KHz at which sleep is expected to run
 */
#define SLEEP_FREQ_FLOOR_KHZ 300000

/* ==================================================================
                     GLOBAL DATA DECLARATIONS
   ================================================================== */

/**
 * @brief g_mode_is_rpm_assisted
 *
 * Variable for indication of entry to RPM assisted modes
 */
static boolean g_mode_is_rpm_assisted = FALSE;

/**
 * @brief sleep_rpm_mode_req
 *
 * Flag for bringup purpose to enable/disable rpm communication from
 * debugger w/o recompiling build.
 */
static unsigned int sleep_rpm_mode_req = SLEEP_ALL_LPRMS;

/* 
 * Sleep Target configuration parameters
 */
static sleep_target_config target_cfg = {
  SLEEP_ENTRY_TICKS,
  SLEEP_EXIT_TICKS,
  SLEEP_INIT_DELAY_MS,
  SLEEP_FREQ_FLOOR_KHZ,
  DALCHIPINFO_FAMILY_UNKNOWN,
  0
};

/* Handle for sleep Interrupt Controller */
DalDeviceHandle *sleepIntHandle;

/* DAL handle for performing clkregime related operations */
static DalDeviceHandle *sleepClkRegimHandle;

/* ==================================================================
                     FORWARD DECLARATIONS
   ================================================================== */

#define SLPTIMER_INTERRUPT_ID        116+32 /*qtmr_qgic2_phy_irq[0]*/

/* List of deferrable timers group */
//extern timer_group_type rex_timer_group;
extern timer_group_type timer_null_group;

/* 
 * Pull in the target-specific nodes that are implemented and 
 * initialized in this module.
 */       
extern sleep_lpr_synth SleepLPR_synth;

/* NPA nodes */
extern npa_node_definition sleep_cpu_vdd_node;
extern npa_node_definition sleep_vdd_dig_node;

/* Debug timer code */
void sleep_register_debug_timer(void);
static struct timer_group_struct sleepDebugTimerGroup;
static timer_type sleepDebugTimer;
static int sleep_enable_debug_timer = 0;
static int dbg_timer_period_sec = 0;

/* ==================================================================
                INTERNAL FUNCTION DECLARATIONS
   ================================================================== */              
   
/**
 * @brief sleep_target_register_lprs
 *  
 * Register the LPRs & nodes that are owned by sleep. 
 */
static void sleep_target_register_lprs( void         *context,
                                        unsigned int  event_type, 
                                        void         *data,       
                                        unsigned int  data_size )
{
  npa_resource_state initial_state = 0;
  npa_client_handle sleep_rpm_lpr_client_handle = NULL;
  
  /* Registering NPA nodes for the LPRs which are exposed outside
   * of sleep. These NPA nodes will eventually register LPRs they
   * control with the sleep subsystem. */
  npa_define_node(&sleep_cpu_vdd_node, &initial_state, NULL);
  npa_define_node(&sleep_vdd_dig_node, &initial_state, NULL);

  /* Creating npa handles for registering/handling various LPRs */
  CORE_VERIFY_PTR( sleep_rpm_lpr_client_handle = 
                   npa_create_sync_client( SLEEP_LPR_NODE_NAME,
                                           "core/power/sleep",
                                           NPA_CLIENT_REQUIRED ) );

  /* Registering lprs */
  sleep_define_lpr_str( "rpm", sleep_rpm_lpr_client_handle );
  npa_issue_required_request( sleep_rpm_lpr_client_handle, sleep_rpm_mode_req );
}

static void sleep_debug_timer_callback(timer_cb_data_type data)
{
  extern boolean sleep_allow_low_power_modes;

  sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 0,
                    "Enabling from Sleep from debug timer callback" );

  sleep_allow_low_power_modes = TRUE;
}

/* ==================================================================
              EXTERNAL FUNCTION DECLARATIONS
   ================================================================== */              
/*
 * sleep_target_lpr_init 
 *  
*/
void sleep_target_lpr_init( void )
{
  /* Register the pre-synthesized LPRMs here.  This must be done
   * before any of the component LPRMs are registered, so we do
   * it when the /sleep/lpr node is being initialized. */
  sleep_define_lpr_synth( &SleepLPR_synth );
}

/*
 * Dependencies to wait on before registering any LPRs.
 */
static const char *lpr_dependencies[] =
{
  /* "/init/rpm", */
  "/sleep/lpr"
};

/*
 * sleep_target_init
*/
sleep_target_config * sleep_target_init( void )
{
  /* Create a handle to check for interrupts */
  DALResult eResult;

  /* Attaching a handle to interrupt controller to query pending interrupts */
  eResult = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &sleepIntHandle);
  CORE_VERIFY(eResult == DAL_SUCCESS);

  /* Obtaining handle to clock regime for swfi */
  eResult = DAL_DeviceAttach(DALDEVICEID_CLOCK, &sleepClkRegimHandle);
  CORE_VERIFY(eResult == DAL_SUCCESS);

  /* Set up timer groups that should be ignored when calculating sleep time. */
  timer_group_set_deferrable( &timer_null_group, TRUE );
  //timer_group_set_deferrable( &rex_timer_group,  TRUE );

  /* Register a callback for when the necessary dependencies are available,
   * in order to register the nodes and LPRs that sleep owns. */
  npa_resources_available_cb( NPA_ARRAY( lpr_dependencies ), 
                                sleep_target_register_lprs, NULL );

  sleep_register_debug_timer();

  return &target_cfg;
}

/*
 * sleep_target_get_CPU_arch
 *
*/
void sleep_target_get_CPU_arch(DalChipInfoFamilyType *chipFamily,
                               DalChipInfoVersionType *chipVer)
{
  static uint32 init = 0;

  if(init == 0)
  {
    DalDeviceHandle *hChipInfo;

    /* Get global chip architecture  */
    if( DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &hChipInfo) == DAL_SUCCESS )
    {
      DalDevice_Open(hChipInfo, DAL_OPEN_SHARED);
      DalChipInfo_GetChipFamily(hChipInfo, &target_cfg.chip_family);
      DalChipInfo_GetChipVersion(hChipInfo, &target_cfg.chip_ver);
      DalDevice_Close(hChipInfo);
      init = 1;
    }
  }

  if(chipFamily != NULL)
    *chipFamily = target_cfg.chip_family;

  if(chipVer != NULL)
    *chipVer = target_cfg.chip_ver;

  return;
}

/**
 * @brief sleep function to disable deferrable timers
 */
void sleep_target_defer_timers( void )
{
  timer_defer_match_interrupt_64();
}

/**
 * @brief sleep function to enable deferrable timers
 */
void sleep_target_undefer_timers( void )
{
  timer_undefer_match_interrupt();
}

/**
 * @brief Wrapper function to enable watch dog.
 */
void sleep_target_enable_dog( void )
{
  dog_autokick( FALSE );
}

/**
 * @brief Wrapper function to disable watch dog.
 */
void sleep_target_disable_dog( void )
{
  dog_autokick( TRUE );
}

/**
 * @brief Function to set flag for rpm assisted low power modes.
 */
void sleep_target_set_rpm_assisted_mode( boolean value )
{
  g_mode_is_rpm_assisted = value;
}

/**
 * @brief Function to query if we performed rpm assisted low power modes
 *        in current sleep cycle.
 */
boolean sleep_target_get_rpm_assisted_mode( void )
{
  return(g_mode_is_rpm_assisted);
}

/*
 * sleep_target_get_rpm_stat_ptr
 */
void* sleep_target_get_rpm_statistics_addr(void)
{
  uint32 rpm_stats_va = 0;

  /* RPM Stats Virtual Address is 128 bytes above
   * the mpm interrupt base virtual address */
  rpm_stats_va = (mpm_get_proc_mpm_base_va() - 128);

  return ((void*) rpm_stats_va);
}

/**
 * SWFI without any ramping. Usually called while short SWFI (i.e. we don't
 * have enough time to go to lower power modes).
 */
void sleep_target_swfi( void )
{
  /*-----------------------------------------------------------------------*/
  /* Standby and Wait for Interrupt (SWFI)                                 */
  /* The CPU clock is halted and waits for any interrupt to occur          */
  /*-----------------------------------------------------------------------*/

  DalClock_ProcessorSleep( sleepClkRegimHandle, CLOCK_SLEEP_MODE_HALT, 0 ); 
}

void sleep_register_debug_timer(void)
{
  if( sleep_enable_debug_timer )
  {
    /* Setting our timer group as non deferrable */
    timer_group_set_deferrable(&sleepDebugTimerGroup, FALSE);

    /* Creating a timer with a call back function in debug group */
#if 0
    timer_def( &sleepDebugTimer,
               &sleepDebugTimerGroup,
               NULL,
               0,
               sleep_debug_timer_callback,
               (timer_cb_data_type) 0 );
#endif
	timer_def_osal( &sleepDebugTimer,
	                &sleepDebugTimerGroup,
	                TIMER_FUNC1_CB_TYPE,
	                sleep_debug_timer_callback,
	                (timer_cb_data_type) 0 ); 

    /* Set timer to fire every 1 second */
    timer_set( &sleepDebugTimer,
               10,
               dbg_timer_period_sec,     /* Non periodic by default */
               T_SEC );
  }

}

