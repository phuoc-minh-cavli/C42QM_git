/*==============================================================================
  FILE:         uATS.c
  
  OVERVIEW:     This file implements the micro-Active Time Solver thread and the 
                interfaces associated with it.
  
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/uATS/uATS.c#3 $
$DateTime: 2019/05/28 10:11:02 $
===============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "uCoreTime.h"
#include "uATS.h"
#include "qurt.h"
#include "qurt_mutex.h"
#include "uSleep_solver.h"
#include "uSleep_log.h"
#include "uSleep_os.h"
#include "timer.h"

#ifdef USLEEP_SUPPORT_TRANSITIONS
#include "sleepActive.h"
#include "uSleepi.h"
#include "uSleep_util.h"
#endif

#ifdef UNPA_SUPPORT
#include "uSleep_npa.h"
#include "unpa_event.h"
#include "unpa.h"
#endif

/*==============================================================================
                              GLOBAL DEFINITIONS
  =============================================================================*/
#ifdef UNPA_SUPPORT
/* Query handles for the current state of latency nodes */
unpa_query_handle   g_latencyQueryHandle;
#endif

 
/* ATS thread stack */
uint8  g_uSleepSolverTaskStack[USLEEP_SOLVER_STACK_SIZE];

/* ATS signal set/clear locking */
qurt_mutex_t        g_uATSSignalMutex;

/* Signal Object used for communicating with Active Time solver */
qurt_signal_t       g_uActiveSignal;
qurt_signal_t       g_uTimerSignal;

/* State of Solver Inputs */
uSleep_solver_input g_uSolverInput;

/* State of Solver Outputs */
uSleep_solver_output   g_uSolverOutput;

/* Last solver output selected by Active Time solver */
uSleep_solver_output   g_lastuSolverOutput;

/* Threshold timer object. */
timer_type          g_uATSThresholdTimer;
/*==============================================================================
                              INTERNAL DEFINITIONS
  =============================================================================*/
/* Indicates if uATS is active or not */
static boolean                g_uATSisActive = FALSE;


/*==============================================================================
                           STATIC FUNCTION DEFINITIONS
  =============================================================================*/
/**
 * uATS_getCurrentSignal 
 * 
 * @brief Returns any currently pending signals 
 *
 * @param sig_mask: Mask used to check for specific currently pending signals
 */
static uint32 uATS_getCurrentSignal(void)
{
  return (qurt_signal_get(&g_uActiveSignal));
}

/** 
 * uATS_setThresholdTimer 
 *  
 * @brief Sets the threshold timer which will expire at the deadline provided
 *        by solver. It is expected to be called after solver has finished.
 *
 * @param output: Output object used while calling solver.
 */
static void uATS_setThresholdTimer(uSleep_solver_output *output)
{
  if(output->threshold_deadline > uCoreTimetick_Get64())
  {
    /* We have valid threshold deadline and there is a possibility of 
     * different mode becoming optimal as duration reduces. */
    timer_set_absolute(&g_uATSThresholdTimer, output->threshold_deadline);

    uSleepLog_printf((1*2),
                     "Threshold set (Deadline: 0x%llx)",
                     ULOG64_DATA(output->threshold_deadline));
  }
  else
  {
    uATS_stopThreasholdTimer();

    uSleepLog_printf((1 * 2),
                     "Threshold not programmed (Value: 0x%llx)",
                     ULOG64_DATA(output->threshold_deadline));
  }

  return;
}

/** 
 * uATS_TriggerSolver 
 *  
 * @brief Execute the Active Time solver and determine whether mode selection
 *        has changed
 *
 *        Performs the following operations:
 *        1. Execute solver based on last updated solver input
 *        2. If mode selection results in the same mode,  configure reentrant
 *           component modes.
 *        3. If mode selection results in a Cached Mode then unconfigure
 *           previously configured cached modes and configure the newly selected
 *           cached ones.
 *        4. If mode selection results in Uncached Mode then 
 *           unconfigure previous configured cached modes
 *        5. Calls sleepOS_ConfigIdleMode to set the appropriate entry 
 *           depending on whether the selected mode is cached or uncached
 * 
 */
static uSolver_status uATS_TriggerSolver(void)
{
  uSleep_solver_output  solverOutput;
  uSolver_status        solverStatus;
  uSleep_LPR            *lastSynthMode = g_lastuSolverOutput.mode;
  
  /* Run the main solver to select a mode to enter */
  solverStatus = uSleepSolver_solve(&g_uSolverInput, &solverOutput);

  if(USOLVER_DEADLINE_IN_PAST == solverStatus)
  {
    /* Current deadline is in the past, signal to get the latest deadlines and rerun solver* 
     * Set the signal directly here, since uATS will process it immediately */
    qurt_signal_set(&g_uActiveSignal, USLEEP_SIGNAL_HARD_DURATION);
    return solverStatus;
  }

  /* Mode must always be chosen */
  CORE_VERIFY(USOLVER_SUCCESS == solverStatus);

  /* Update the last selected solver output data */
  g_lastuSolverOutput = solverOutput;
  
  /* Set a threshold timer in case this selected mode becomes stale. */
  uATS_setThresholdTimer(&solverOutput);
  
  /* Nothing further to do if solver chose the same mode */
  if(lastSynthMode == solverOutput.mode)
  {
    /* All cacheable modes are re-entrant - call mode enter function again */
    if(TRUE == solverOutput.mode->cached)
    {
      uSleepLPR_runLPRFunctions(TRUE, (g_uSolverInput.wakeup_deadline - solverOutput.mode->backoff),
                                TRUE, solverOutput.mode);
    }

    return solverStatus;
  }

  /* Transition across Synth Modes if solver output has changed */
  if((NULL != lastSynthMode) &&
     (TRUE == lastSynthMode->cached))
  {
    /* Unconfigure previously configured Cached Synth Mode */
    uSleepLPR_runLPRFunctions(FALSE, 0, FALSE, lastSynthMode);
  }

  /* Configure the sleep functionality for the new mode */
  if((NULL != solverOutput.mode) && 
     (TRUE == solverOutput.mode->cached))
  {
    /* If synth is cacheable, run its enter functions */
    uSleepLog_printf(0, "Configuring cacheable mode" );

    /* Enter the cacheable modes */
    uSleepLPR_runLPRFunctions(TRUE, (g_uSolverInput.wakeup_deadline - solverOutput.mode->backoff),
                              FALSE, solverOutput.mode);

    /* Configure OS Idle Mode for Cached LPR mode */
    uSleepOS_configIdleMode(USLEEP_OS_IDLE_MODE_OVERRIDE);
  }
  else
  {
    /* Configure OS Idle Mode for Uncached Synth Mode */
    uSleepOS_configIdleMode(USLEEP_OS_IDLE_MODE_DEFAULT);
  }

  return solverStatus;
}

/**
 * uATS_ClearSignal
 *
 * @brief Clear a signal or mask of signals on Active Time solver thread
 * 
 * @param signals: signal or signal mask to be cleared on uATS
 */
INLINE void uATS_ClearSignal(uint32 signals)
{
  qurt_signal_clear(&g_uActiveSignal, signals);
}

/**
 * @brief Updates solver input based on set signals
 *
 * @param signals: Data signals that are set on Active Time Solver
 */
static void uATS_ProcessDataSignals(uint32 signals)
{
  
 /* if(signals & USLEEP_SIGNAL_THRESH_TIMER)
  {
    uATS_ClearSignal(USLEEP_SIGNAL_RCINIT_DONE);
    uSleepOS_configIdleMode(SLEEP_OS_IDLE_MODE_DEFAULT);
    
    uSleepLog_printf(SLEEP_LOG_LEVEL_ATS, 0, "ATS: RCinit complete, allowing normal operation");
  } */
  
  if(signals & USLEEP_SIGNAL_THRESH_TIMER)
  {
    uATS_ClearSignal(USLEEP_SIGNAL_THRESH_TIMER);

    /* Fake a hard duration change to get latest sleep time.  Do this because of the limited* 
     * power modes and most likely we have woken up from sleep and the threshold timer will be
     * the first one to have expired.  This saves time of waiting for the actual hard duration 
     * change signal. */ 
    signals |= USLEEP_SIGNAL_HARD_DURATION;
  }

  if(signals & USLEEP_SIGNAL_LATENCY)
  {
    uATS_ClearSignal(USLEEP_SIGNAL_LATENCY);

#ifdef UNPA_SUPPORT
    if(NULL != g_latencyQueryHandle)
    {
      unpa_query_result qres;

      /* Get the latency budget from the latency node.  The latency node will
       * return the minimum latency budget being requested by all clients.
       * This lets us know how quickly we need to respond to an interrupt when
       * it occurs. */
      CORE_VERIFY(UNPA_QUERY_SUCCESS == 
                  unpa_query(g_latencyQueryHandle, UNPA_QUERY_ACTIVE_STATE, &qres));

      g_uSolverInput.latency_budget = (uint32)qres.val;
    }
#endif
  }

  if(signals & (USLEEP_SIGNAL_REGISTRY | 
                USLEEP_SIGNAL_XO_SETTLE_CHANGE))
  {
    /* Nothing to do other than clear it */
    uATS_ClearSignal(USLEEP_SIGNAL_REGISTRY | 
                     USLEEP_SIGNAL_XO_SETTLE_CHANGE);
  }

  /* Check last to allow a little time from the threshold signal to try and catch the real hard duration
   * chagne signal and avoid double processing for timer based wakeup. */
  if(signals & USLEEP_SIGNAL_HARD_DURATION)
  {
    uATS_ClearSignal(USLEEP_SIGNAL_HARD_DURATION); 

    /* First non deferrable timer expiry changed */
    g_uSolverInput.wakeup_deadline = timer_get_first_non_deferrable_timer_expiry();

    uSleepLog_printf((1*2), 
                     "Hard deadline (Timer: 0x%llx)",
                     ULOG64_DATA(g_uSolverInput.wakeup_deadline));
  }

  uSleepLog_printf(0, "Processed data signals");

  return;
}

/*==============================================================================
                            GLOBAL FUNCTION DEFINITIONS
  =============================================================================*/
/* 
 * uATS_GetSolverOutput
 */
uSleep_solver_output *uATS_getSolverOutput(void)
{
  return (&g_lastuSolverOutput);
}

/* 
 * uATS_GetSolverInput 
 */
uSleep_solver_input *uATS_getSolverInput(void)
{
  return (&g_uSolverInput);
}

/* 
 * uATS_SetSignal 
 */
void uATS_SetSignal(uATS_signalType signals)
{
  /* Sets a uATS signal and kicks the main sleep thread to run uATS and process the signal */
  qurt_signal_set(&g_uActiveSignal, (uint32)signals);

  /* Only cancel the wait for idle call if ATS is not already active */
  qurt_mutex_lock(&g_uATSSignalMutex);

  if(FALSE == g_uATSisActive)
  {
    qurt_power_cancel_wait_for_idle();
  }

  qurt_mutex_unlock(&g_uATSSignalMutex);

  return;
}

/*
 * uATS_performSolving
 */
void uATS_performSolving(void)
{
  uSolver_status  solverStatus;
  uint32          signals  = 0;
 
  qurt_mutex_lock(&g_uATSSignalMutex);
  g_uATSisActive = TRUE;
  qurt_mutex_unlock(&g_uATSSignalMutex);

  signals = uATS_getCurrentSignal();

  uSleepLog_printf(1,
                   "uATS processing (signals: 0x%x)",
                   signals);

  while(0 != signals)
  {
    /* Process updated input signals */
    uATS_ProcessDataSignals(signals);

    /* Check for any pending signals that could have been asserted while ATS is running
     * and update data now to avoid running solver more than necessary. */
    signals = uATS_getCurrentSignal();

#ifdef USLEEP_SUPPORT_TRANSITIONS
    /* Make sure an exit request didn't come in to avoid wasting time solving */
    if(0 == (uSleep_getStateInternal() & uSLEEP_INTERNAL_STATE_ACTIVE))
    {
      break;
    }
#endif

    if(0 == signals)
    {
      /* If no signals have been triggerd, invoke the solver with updated input.
       * Solver will configure the appropriate idle mode if required. */
      solverStatus = uATS_TriggerSolver();
    
      /* Depending on solver behavior, certain signals may have been set, so
       * check for any pending ones before exiting processing loop. */
      signals = uATS_getCurrentSignal();
    }

    if(0 != signals)
    {
      uSleepLog_printf(1, 
                       "uATS signal bypass (signals: 0x%x)",
                       signals);
    }
  }

  qurt_mutex_lock(&g_uATSSignalMutex);
  g_uATSisActive = FALSE;
  qurt_mutex_unlock(&g_uATSSignalMutex);

  uSleepLog_printf(0, "uATS waiting for signals");

  return;
}

/*
 * uATS_timerThreshHandlerCB
 */
void uATS_timerThreshHandlerCB(timer_cb_data_type data)
{
  /* Threshold timer expired, tell uATS */
  uATS_SetSignal(USLEEP_SIGNAL_THRESH_TIMER);
  return;
}

/*
 * uATS_timerEventHandlerCB
 */
void uATS_timerEventHandlerCB(timer_cb_data_type data)
{
#ifdef USLEEP_SUPPORT_TRANSITIONS
  uSleep_internal_state state = uSleep_getStateInternal();

  /* If not in uSleep functionality, forward to normal ATS, otherwise
   * forward to uATS */
  if(uSLEEP_INTERNAL_STATE_IN_DDR == state)
  {
    /* Forward a uTimer change to normal ATS */
    sleepActive_SetSignal(SLEEP_SIGNAL_ISLAND_HARD_DURATION);
  }
  else if(uSLEEP_INTERNAL_STATE_ACTIVE != state)
  {
    /* If not in a stable state (i.e. enter/exit trasition) do not forward a uTimer
     * change as the exit process will force normal ATS to update all timers */
    return;
  }
  else
#endif /* USLEEP_SUPPORT_TRANSITIONS */
  {
    /* Forward a uTimer change to uATS */
    uATS_SetSignal(USLEEP_SIGNAL_HARD_DURATION);
  }

  return;
}

/*
 * uATS_stopThreasholdTimer
 */
void uATS_stopThreasholdTimer(void)
{
  timer_stop(&g_uATSThresholdTimer,0,NULL); 
  return;
}

#ifdef UNPA_SUPPORT
/** 
 * uATS_nodeChangeCB
 * 
 * @brief This Callback is triggered when the various uSleep nodes are updated
 *
 * @param See npa callback type
 */
void uATS_nodeChangeCB(unpa_event_type  eventType,
                       void             *eventData,
                       void             *cbdata)
{
  if(eventType == UNPA_POST_CHANGE_EVENT)
  {
    /* Forward the node assocaited signal to uATS */
    uATS_SetSignal((uATS_signalType)cbdata);
  }

  return;
}
#endif



/*==============================================================================
                       EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uATS_SignalTask
 */
void uATS_SignalTask
(
  /* Parameter received from Main Control task - ignored */
  void *ignored
)
{
  uint32 signals;

  /* Begin task loop */
  while(1)
  {
    /* Wait for any work signals */
    signals = qurt_anysignal_wait(&g_uTimerSignal, (USLEEP_SIGNAL_HARD_DURATION));
    qurt_signal_clear(&g_uTimerSignal, (USLEEP_SIGNAL_HARD_DURATION));

    /* Forward a uTimer change to uATS */
    uATS_SetSignal(USLEEP_SIGNAL_HARD_DURATION);
	
  
  }
  
  CORE_VERIFY(0);
}

