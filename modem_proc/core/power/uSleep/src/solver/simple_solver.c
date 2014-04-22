/*==============================================================================
  FILE:         simple_solver.c

  OVERVIEW:     This file provides the support for the simple sover

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/solver/simple_solver.c#2 $
$DateTime: 2019/05/28 10:11:02 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "uCoreTime.h"
#include "uSleep_solver.h"
#include "uSleep_log.h"
#include "uSleep_lpr.h"
#include "qurt.h"

/*==============================================================================
                          EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleepSolver_solve
 */
uSolver_status simpleSolver_solve(uSleep_solver_input   *input, 
                                  uSleep_solver_output  *output)
{
  uSleep_LPR  *mode;
  uint32      modeLatency;
  uint64      duration;
  int32       idx             = USLEEP_NUMBER_LOW_POWER_MODES - 1;
  uint64      wakeupDeadline  = input->wakeup_deadline;

  CORE_VERIFY_PTR(input);
  CORE_VERIFY_PTR(output);

  /* Set default values for solver output */
  memset(output, 0, sizeof(uSleep_solver_output));

  if(FALSE == g_uSleepAllowLowPowerModes)
  {
    /* Return the default low power mode which must always be valid and enabled at all times */
    uSleepLog_printf(0, " Power modes disabled, selecting default");
    output->mode                = &g_uSleepDefaultLPR;
    output->threshold_deadline  = 0;  /* No need to re-solve based on threshold timer */
    return USOLVER_SUCCESS;
  }
  
  duration = wakeupDeadline - uCoreTimetick_Get64();

#ifdef UNPA_SUPPORT
  uSleepLog_printf(1 + (1 * 2),
                   " Simple Solver (Sleep duration: 0x%llx) "
                   "(Latency budget 0x%x)",
                   ULOG64_DATA(duration),
                   input->latency_budget);
#else
  uSleepLog_printf((1 * 2),
                   " Simple Solver (Sleep duration: 0x%llx)",
                   ULOG64_DATA(duration));
#endif

  /* Start at highest index (most power savings) and work backwards */
  while(idx >= 0)
  {
    mode        = &g_uSleepLPR[idx];
    modeLatency = mode->working_latency.enter + mode->working_latency.exit;

    if(wakeupDeadline <= uCoreTimetick_Get64())
    {
      uSleepLog_printf((1 * 2), 
                       " Solver bypass "
                       "(Hard Deadline in past: 0x%llx)",
                       ULOG64_DATA(wakeupDeadline));

      return USOLVER_DEADLINE_IN_PAST;
    }

    /* Both user and internal enablement flags must be set for LPR to truly be enabled */
    if((USLEEP_LPR_ENABLED == mode->enabled_flags)  && 
       (duration > modeLatency)
#ifdef UNPA_SUPPORT
       && (input->latency_budget > modeLatency)
#endif
      )
    {
      output->threshold_deadline = wakeupDeadline - modeLatency;

      /* If threashold will expire 'relativly' soon, it's just not worth selecting this mode 
       * so try to find the next one. */
      if((uCoreTimetick_Get64() >= output->threshold_deadline) ||
         ((uCoreTimetick_Get64() - output->threshold_deadline) < (192 * 2))
        )
      {
        uSleepLog_printf(1,
                       " Mode skipped "
                       "(Name: %s) "
                       "(Reason: Close to threshold)",
                       mode->name);

        output->threshold_deadline = 0;
      }
      else
      {
        output->mode = mode;
        break;
      }
    }
    /* Only print if internally enabled from compile time */
    else if(USLEEP_LPR_INTERNAL_ENABLE_MASK == (mode->enabled_flags & USLEEP_LPR_INTERNAL_ENABLE_MASK)) 
    {
      uSleepLog_printf(3,
                       " Mode skipped "
                       "(Name: %s) "
                       "(Enabled: %s) "
                       "(Latency: 0x%x)",
                       mode->name,
                       (mode->enabled_flags & USLEEP_LPR_ENABLE_MASK) ? "True" : "False",
                       modeLatency);
    }

    idx--;
  }
 
  if(NULL == output->mode)
  {
    output->mode                = &g_uSleepDefaultLPR;
    output->threshold_deadline  = 0;  /* No need to re-solve based on threshold timer */
  }

  uSleepLog_printf(1,
                   "Solver mode chosen (Mode: %s)",
                   output->mode->name);

  return USOLVER_SUCCESS;
}

