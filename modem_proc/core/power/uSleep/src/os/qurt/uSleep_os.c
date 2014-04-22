/*==============================================================================
  FILE:         uSleep_os.c

  OVERVIEW:     This file provides the main uSleep functions that reside in
                island memory space

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/os/qurt/uSleep_os.c#2 $
$DateTime: 2019/05/28 10:11:02 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "CoreTime.h"
#include "uCoreTime.h"
#include "qurt.h"
#include "timer.h"
#include "uATS.h"
#include "uSleep_log.h"
#include "uSleep_os.h"
#include "uSleep_util.h"
#include "uSleep_target.h"
#include "rpmclient.h"
#include "uSleep_solver.h"
#include "HALcomdef.h"
#include "q6_lpm_config.h"

#ifdef USLEEP_SUPPORT_TRANSITIONS
#include "uSleep_os_trans.h"
#include "dog_hal.h"
#include "uSleep_transi.h"
#include "sleep_os.h"
#endif

/*==============================================================================
                             INTERNAL VARIABLES
 =============================================================================*/
/* Flag to indicate if exit routines should be run */
boolean g_uSleepRunExit = FALSE;



/*==============================================================================
                           EXTERNAL GLOBAL VARIABLES
 =============================================================================*/
extern uSleep_idle_entry_ptr_type                      g_uSleepIdleEntryTable[];
/* State of Solver Inputs */
extern uSleep_solver_input g_uSolverInput;

extern uSleep_solver_output   g_uSolverOutput;
extern uSleep_solver_output   g_lastuSolverOutput;
extern volatile uSleep_idle_entry_ptr_type * volatile  g_uIdleEntryPtr;

extern volatile boolean g_isRPMAssisted;
/*==============================================================================
                            INTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
 /**
 * uSleepOS_prepareForSleep 
 *  
 * @brief Common logic that will verify it is safe to enter a low power mode. 
 *  
 *        If there is an entry failure status, the appropriate action will have
 *        already occured, the caller will not need to take further action.
 *  
 *        If the function returns success, it will have already entered STM
 *        and the caller should continue to enter a low power mode.
 *  
 *
 *  
 * @return Time of STM entry (in ticks) or 0 if failure 
 */
static uint64 uSleepOS_prepareForSleep(uSleepOS_idleModeType  idleModeCfg)
{
  uint64 uSleepStart = 0;
  static uint32 rpm_cnt=0;

  /* Wait for all other HW threads to go idle. If the override setting is 
   * disabled, this function will return and the system will already be in
   * STM. */
  if(0 != qurt_power_wait_for_idle())
  {
    return uSleepStart;
  }	

  uSleepLog_printf(0,
                       "Check to ensure that no rpm msgs are in-flight");
  /*Check to ensure that no rpm msgs are in-flight */ 
  if(FALSE == rpm_is_mq_empty())
  {
    if(0 == (rpm_cnt++ % 24))
    {
      uSleepLog_printf(1,
                       "Short SWFI (reason: RPM message in flight) "
                       "(Count %d)",
                       rpm_cnt);
    }
    
    qurt_power_exit();
    /* Configure the Q6 for Clock Gating when exiting any LPRM */
    q6LPMConfig_setupModeConfig(RSC_MODE_CLK_GATE, FALSE);
	uSleepTarget_enterIdle();
	
    return uSleepStart;
  } 
  
  /* Mark start of this sleep cycle */
  uSleepStart = CoreTimetick_Get64();
  uSleepLog_printf(0, "uSleep cycle start");
  
  /* Ensure that idle plugin has not changed 
  else if(FALSE == uSleepOS_verifyIdleModeConfig(idleModeCfg))
  {
    /*sleepLog_QDSSEvent(SLEEP_EARLY_EXIT_STM_NUM_ARGS,
                       SLEEP_EARLY_EXIT_STM); 

    uSleepLog_printf(0, 
                    "Sleep cycle skipped "
                    "(reason: Idle Plugin state changed)");

    uSleepStart = 0;
    qurt_power_exit();
  }  */

  return uSleepStart;
}

 
/* 
 * uSleepOS_performLPMExit
 */
void uSleepOS_performLPMExit(void)
{
  uint64                STMExit;
  unsigned int          interrupt;
  uSleep_solver_output  *solverOutput;

  interrupt    = qurt_system_vid_get();
  solverOutput = uATS_getSolverOutput();
  const char *wake_reason;
  RPM_master_stats        *rpmStats;
  rpmStats = (RPM_master_stats        *) uSleepTarget_getRPMStatisticsAddress(); 
  CORE_VERIFY_PTR(rpmStats);

  CORE_VERIFY(TRUE == g_uSleepRunExit);

  /* Verify that an exit funtion needs to be called */
  if(NULL != solverOutput->mode)
  {
    /* Run exit function */
    uSleepLPR_runLPRFunctions(FALSE, 0, FALSE, solverOutput->mode);
  }
  
  /* Clear the need to run the exit function */
  g_uSleepRunExit = FALSE;
    
  /* Record STM exit time */
  STMExit = uCoreTimetick_Get64();

  /* Exit STM */
  qurt_power_exit();
  qurt_timer_group_enable( 0 );
  timer_undefer_match_interrupt();

  uSleepLog_QDSSPrintf(USLEEP_MASTER_WAKEUP_NUM_ARGS,
                       USLEEP_MASTER_WAKEUP_STR, 
                       USLEEP_MASTER_WAKEUP,
                       interrupt,
                       ULOG64_DATA(uSleep_getLastSleepWakeupInternal()),
                       ULOG64_DATA(STMExit));

    if(g_isRPMAssisted == TRUE)
    {
	  
	  if(rpmStats->wakeup_reason)
	  {
        wake_reason = "Timer";
      }
      else
	  {
	    wake_reason = "Rude";
	  }	
	
	  
    /* Log RPM bringup info */
    uSleepLog_printf( 3 + (3*2),
                     "RPM stats (wakeup_ind: 0x%llx) (bringup_req: 0x%llx) "
                     "(bringup_ack: 0x%llx) (sleep trans time: %d) "
                     "(wake trans time: %d)""(reason:%s)",
                     ULOG64_DATA(inp64(&rpmStats->wakeup_ind)),
                     ULOG64_DATA(inp64(&rpmStats->bringup_req)),
                     ULOG64_DATA(inp64(&rpmStats->bringup_ack)),
                     rpmStats->last_sleep_transition_duration,
                     rpmStats->last_wake_transition_duration,
					 wake_reason);
    g_isRPMAssisted = FALSE;					 
  }

  return;
}

/*==============================================================================
                            EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/* 
 * uSleepOS_performLPM
 */
uint32 uSleepOS_performLPM(void)
{
  uint64                now;
  volatile uint64 tmp_deadline;
  void    *context = (void *)(USLEEP_OS_IDLE_MODE_DEFAULT);
  uint32                printCnt        = 0;
  static uint32                deadline_printCnt        = 0;
  uint32                uSleepStart = 0;
  uSolver_status        solverStatus;
  //uSleep_LPR            *lastSynthMode = g_lastuSolverOutput.mode;


  uSleepOS_idleModeType  idleModeCfg = (uSleepOS_idleModeType)context;
#ifdef USLEEP_SUPPORT_TRANSITIONS
  uSleep_internal_state state;
#endif

  /* Wait for all other HW threads to go idle.
   * 1. If override is set, we will enter all-wait when all threads go idle and
   *    this function does not return.
   * 2. If override is not set, this call will return when all threads go
   *    idle and we will be in STM 
   * 3. If the call is forced to return when the HW is not idle, the override bit
   *    setting is ignored, the function will return, but we will not be in STM.  This will
   *    be the case when uATS needs to run */ 
    
   if(0 == (uSleepStart = uSleepOS_prepareForSleep(idleModeCfg)))
    {
     return 2;
    }


#ifdef USLEEP_SUPPORT_TRANSITIONS
  /* Refresh current state of uSleep to verify that an exit request has not been
   * made. */
  state = uSleep_getStateInternal();
  
  /* Check if the function pointer has changed or if an island exit has been requested and
   * simply return from this function.
   * The exit request state check should not be true, as the exit routines should have
   * forced an exit from the wait_for_idle call above, but the logic is here just in case. */
  if((uSleepOS_getIdleFunctionPtr() != uSleepOS_performLPM) ||
     (state & (uSLEEP_INTERNAL_STATE_STAGE1_EXIT | 
               uSLEEP_INTERNAL_STATE_STAGE2_EXIT)))
  {
    uSleepLog_printf(2,
                     "uSleep perform exit "
                     "(Reason: Function change or exit request) "
                     "(Fcn: 0x%x) "
                     "(state: 0x%x)",
                     (uint32)uSleepOS_getIdleFunctionPtr(),
                     state);

    /* Exit STM */
    qurt_power_exit();

    return 1;
  }
#endif


  /* First non deferrable timer expiry changed */
  tmp_deadline = timer_get_first_non_deferrable_timer_expiry();
  g_uSolverInput.wakeup_deadline = tmp_deadline;

  uSleepLog_printf((1*2), 
                     "Hard deadline (Timer: 0x%llx)",
                     ULOG64_DATA(g_uSolverInput.wakeup_deadline));

  /* Run the main solver to select a mode to enter */
  solverStatus = uSleepSolver_solve(&g_uSolverInput, &g_uSolverOutput);
  
  /* Update the last selected solver output data */
  g_lastuSolverOutput = g_uSolverOutput;
  
  if(USOLVER_DEADLINE_IN_PAST == solverStatus)
    {
	  /* Limit the message printing if we are continiously unable to choose a mode */
      if(0 == (deadline_printCnt++ % 24))
      {
        uSleepLog_printf(1,
                       "Skipping LPM - solver returned deadline in past "
                       "(Count %d)",
                       deadline_printCnt);
      }
 	

      /* Exit STM */
      qurt_power_exit();
	  uSleepTarget_enterIdle(); //added as got past deadline crash
	  
      return 0;
    }
  

  /* Mode must always be chosen */
  CORE_VERIFY(USOLVER_SUCCESS == solverStatus);

   if (NULL != g_uSolverOutput.mode)
  {
    if(printCnt)
    {
      uSleepLog_printf(1,
                       "Skipping LPM - no mode chosen "
                       "(Final count %d)",
                       printCnt);
    }

    /* Clear the mode skipped print counter when a mode is chosen */
    printCnt = 0;

    /* Defer the uImage timers */
    timer_defer_match_interrupt();
	qurt_timer_group_disable( 0 );

    /* Last check to make sure we can enter the mode still. */
    now = uCoreTimetick_Get64();
    if(g_uSolverInput.wakeup_deadline <= now)
    {
      uSleepLog_printf(0,
                       "Skipping LPM - wakeup in past");
      qurt_timer_group_enable( 0 );
      timer_undefer_match_interrupt();

      /* Exit STM */
      qurt_power_exit();
      return 0;
    }

    /* Configure the sleep functionality for the new mode */
    if(TRUE == g_uSolverOutput.mode->cached)
    {
      /* If synth is cacheable, run its enter functions */
      uSleepLog_printf(0, "Configuring cacheable mode" );

      /* Enter the cacheable modes */
      uSleepLPR_runLPRFunctions(TRUE, (g_uSolverInput.wakeup_deadline - g_uSolverOutput.mode->backoff),
                              FALSE, g_uSolverOutput.mode); 
	  /* Exit STM */
      qurt_power_exit();
	  qurt_timer_group_enable( 0 );
	  timer_undefer_match_interrupt();
	  
	  uSleepTarget_enterIdle();  
    	  
    			
      return 0;
    }
    
    /* Need to run the exit routine */
    g_uSleepRunExit = TRUE;

    /* Run the enter functions for the selected LPR that was previously chosen. */
    uSleepLPR_runLPRFunctions(TRUE, (g_uSolverInput.wakeup_deadline - g_uSolverOutput.mode->backoff), 
                              FALSE, g_uSolverOutput.mode);
  }
  else
  {
    /* Limit the message printing if we are continiously unable to choose a mode */
    if(0 == (printCnt++ % 24))
    {
      uSleepLog_printf(1,
                       "Skipping LPM - no mode chosen "
                       "(Count %d)",
                       printCnt);
    }

    /* Exit STM */
    qurt_power_exit();
  }

  return 0;
}

#ifdef USLEEP_SUPPORT_TRANSITIONS
/*
 * uSleepOS_performError
 */ 
uint32 uSleepOS_performError(void)
{
  /* Set via fatal error exit from uImage transition. This function should
   * NEVER be allowed to run from OS scheduler, but core verify just in case. */
  uSleepOS_haltOnError();

  return 1; /* Just here for compliance with prototype. */
}

/*
 * uSleepOS_setIdleFunctionPtr
 */
void uSleepOS_setIdleFunctionPtr(uSleep_idle_entry_ptr_type newIdleEntryPtr)
{
  /* Use main sleeps function pointer setting function since uSleep is
   * running in the sleep thread */
  sleepOS_setLPIEntryFunction((sleep_idle_entry_ptr_type)newIdleEntryPtr);
}

/*
 * uSleepOS_getIdleFunctionPtr
 */
uSleep_idle_entry_ptr_type uSleepOS_getIdleFunctionPtr(void)
{
  /* Use main sleeps API to retrieve the current LPI function */
  return ((uSleep_idle_entry_ptr_type)sleepOS_getLPIEntryFunction());
}

/*
 * uSleepOS_haltOnError
 */
void uSleepOS_haltOnError(void)
{
  HAL_dogForceBite();
  while(1);
}

#endif /* USLEEP_SUPPORT_TRANSITIONS */

/** 
 * uSleepOS_PMIPerformer
 * 
 * @brief Island mode version of the PMI interrupt handler routine.  
 */
void uSleepOS_PMIPerformer(void)
{
#if defined(USLEEP_SUPPORT_UNCACHED_APCR)
  /* set the current time as the wakeup */
  uSleep_setLastSleepWakeupInternal(uCoreTimetick_Get64());

  /* Call exit function */
  uSleepOS_performLPMExit();
#endif

  return;
}

/*==============================================================================
                            EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleepOS_configIdleMode
 */
void uSleepOS_configIdleMode(uSleepOS_idleModeType  newIdleMode)
{
  CORE_VERIFY(newIdleMode < USLEEP_OS_IDLE_MODE_NUM_STATES);
  
  /* Update sleep function pointer */
  g_uIdleEntryPtr = &g_uSleepIdleEntryTable[newIdleMode];
  
  uSleepLog_printf(1, 
                   "Setting sleep function (Idle Mode: %d)",
                   newIdleMode);

  /* Mode specific action - currently it is just for one mode but if this 
   * grows, preferably use switch case. */
  if(USLEEP_OS_IDLE_MODE_OVERRIDE == newIdleMode)
  {
    /* No Sleep task intervention - QuRT performs all wait directly. */
    qurt_power_override_wait_for_idle(TRUE);
  }
  else
  {
    /* Sleep task needs to be scheduled. */
    qurt_power_override_wait_for_idle(FALSE);
  }

  return;
}

/*
 * sleepOS_holdoffNoMode
 */
uint32 uSleepOS_holdoffNoMode(void)
{
  /* Due to some holdoff client, we cannot simply perform any low power modes.
   * Simply return. At some point, sleep function pointer would change when all
   * holdoff requests are released. Until then, we will keep coming here. */
  return 0;
}