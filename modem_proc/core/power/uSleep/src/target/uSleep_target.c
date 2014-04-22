/*==============================================================================
  FILE:         uSleep_target.c

  OVERVIEW:     This file provides uSleep target specfic functions while in
                island mode

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/target/uSleep_target.c#3 $
$DateTime: 2019/05/28 10:11:02 $
==============================================================================*/
#include "comdef.h"
#include "DDITimetick.h"
#include "CoreVerify.h"
#include "uCoreTime.h"
#include "uSleep_target.h"
#include "qurt.h"
#include "uSleep_hwio.h"
#include "uSleep_log.h"
#include "uSleep_util.h"
#include "rsc.h"
#include "uATS.h"
#include "q6_lpm_config.h"
#include "uSleepi.h"

/*==============================================================================
                              GLOBAL VARIABLES
 =============================================================================*/
/* Handle to the DAL "AonTimer" & "WakeupTimer" timetick driver, for calculating the
 * wakeup time from sleep. This timer is used by the DAL and timer services
 * to provide "sleep time". */
DalDeviceHandle *g_DALWakeTimerHandle;      /* Local qTimer */

/*
 * Sleep Target configuration parameters
 *
 */
uSleep_target_config g_uSleepTargetConfig = 
{
  .chip_family    = DALCHIPINFO_FAMILY_UNKNOWN,
  .chip_ver       = 0,
  .l2_mask        = 0x1,
  .tcm_mask       = 0x1,
};
/*==============================================================================
                        INTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
#ifdef DYNAMIC_LPR_CALLBACKS
/* 
 * uSleepTarget_callRegisteredCB
 * 
 * @brief Internal helper function used to call the user registered LPR callbacks
 *        when entering/exiting the power mode
 */
static void uSleepTarget_callRegisteredCB(uSleep_lpr_callback *lpr,
                                          uint64              wakeupTime)
{
  while(NULL != lpr)
  {
    uSleepLog_printf(1,
                     " Calling registered LPR: %s",
                     lpr->name);

    lpr->func(wakeupTime, lpr->userData);
    lpr = lpr->next;
  }
  return;
}
#endif /* DYNAMIC_LPR_CALLBACKS */

/*==============================================================================
                        EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/* 
 * uSleepTarget_getCurrentModeLatency 
 */
const uSleep_lpr_latency *uSleepTarget_getCurrentModeLatency(void)
{
  uSleep_solver_output  *solverOutput = uATS_getSolverOutput();
  return &solverOutput->mode->working_latency;
}

/* 
 * uSleepTarget_enterIdle 
 */
void uSleepTarget_enterIdle(void)
{
  /* Wait for wakeup interrupt */ 
  qurt_power_wait_for_active();
  return;
}

/* 
 * uSleepTarget_configBasePowerMode
 */
void uSleepTarget_configBasePowerMode(void)
{
  /* Initilize the low power mode base settings */
  q6LPMConfig_setupBase();

  /* Set the initial default low power mode setup.  Flag param doesn't matter* 
   * since it will be selected based on the real mode chosen, just need to
   * setup the base default mode here. */
  q6LPMConfig_setupModeConfig(RSC_MODE_PWRC, FALSE);

  /* Disable handshake by default */
  uSleepTarget_setRpmHandshake(FALSE);

  return;
}

/*
 * sleepTarget_wakeupHandler
 */
void sleepTarget_wakeupHandler(uint32 param)
{
  return;
}

/* 
 * uSleepTarget_programWakeupTimer 
 */
void uSleepTarget_programWakeupTimer(uint64 qTimerWakeup, uint64 AONWakeup)
{
  uint64  qTimerProgrammedTime  = 0;

  uSleep_setLastSleepWakeupInternal(qTimerWakeup);

  if(NULL != g_DALWakeTimerHandle)
  {
    /* Program & enable the qTimer wakeup time */
    CORE_DAL_VERIFY(DalTimetick_SetNextInterrupt64(g_DALWakeTimerHandle,
                                                   qTimerWakeup,
                                                   0, &qTimerProgrammedTime));
     
    CORE_DAL_VERIFY(DalTimetick_Enable(g_DALWakeTimerHandle, 1));
  }



  uSleepLog_printf((2*1),
                   " Set wakeup times "
                   "(qTimer Request: 0x%llx) ",
                   ULOG64_DATA(qTimerWakeup));

  /* Verify wakeup time is valid as late as possible */
  CORE_VERIFY(uCoreTimetick_Get64() < qTimerWakeup);

  return;
}

/* 
 * uSleepTarget_disableWakeupTimer 
 */
void uSleepTarget_disableWakeupTimer(void)
{
  uSleepLog_printf(0, "Disable wakeup timer");

  CORE_DAL_VERIFY(DalTimetick_Enable(g_DALWakeTimerHandle, 0));

  return;
}

/* 
 * uSleepTarget_enterLowPowerMode 
 */
void uSleepTarget_enterLowPowerMode(uSleep_LPR *mode, uint64 wakeupTime, boolean reEntrant)
{
  uSleepLog_printf(2,
                   "%s LPM: %s",
                   TRUE == reEntrant ? "Re-entering" : "Entering",
                   mode->name);

#ifdef DYNAMIC_LPR_CALLBACKS
  if(NULL != mode->registered_lprs.enter)
  {
    /* Dynamic reentrant callbacks are not supported in cacheable modes */
    CORE_VERIFY((FALSE == reEntrant) && (FALSE == mode->cached));

    /* Call any user registered CB's */
    uSleepTarget_callRegisteredCB(mode->registered_lprs.enter, wakeupTime);
  }
#endif

  /* Call the main function to enter power collapse */
  if(NULL != mode->enter_func)
  {
    mode->enter_func(wakeupTime, reEntrant);
  }

  return;
}

/* 
 * uSleepTarget_exitLowPowerMode 
 */
void uSleepTarget_exitLowPowerMode(uSleep_LPR *mode)
{
  uSleepLog_printf(0, "Exiting LPM");

  if(NULL != mode->exit_func)
  {
    mode->exit_func();
  }

#ifdef DYNAMIC_LPR_CALLBACKS
  if(NULL != mode->registered_lprs.exit)
  {
    /* Call any user registered CB's */
    uSleepTarget_callRegisteredCB(mode->registered_lprs.exit, 0);
  }
#endif

  return;
}

/*
 * uSleepTarget_setRpmHandshake
 */
rsc_result_t uSleepTarget_setRpmHandshake(boolean performHandshake)
{
  q6LPMConfig_setRpmhHandshake(performHandshake);
  return RSC_SUCCESS;
}

/*
 * uSleepTarget_setRscLowPowerMode
 */
rsc_result_t uSleepTarget_setRscLowPowerMode(rsc_type type, rsc_low_power_mode_type coreMode)
{
  return(rsc_set_low_power_mode(RSC_CHILD, coreMode));
}

