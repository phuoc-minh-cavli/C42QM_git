/**
 * @file smdl_profile_rex.c
 *
 * REX-specfic implementation for SMD Lite profiling
 */

/*--------------------------------------------------------------------------
     Copyright (c) 2009-2012 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
----------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smdl_profile_rex.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/12   bt      Clean-up residual deprecated task and SMD_TEST defines.
02/21/12   bt      Added failure return info in timetick calls.
02/21/12   bt      Fixed timetick error code (can't redefine SMDL_ERROR).
02/09/12   bt      Added OS implementation for timetick calls in smdl_profile.
04/20/11   bm      smdl_profile_thread() - should not return anything.
03/10/11   bt      Compiler warning resolved for smdl_profile_thread.
04/21/10   tl      Updated to SMD Lite v2
04/17/09   tl      Initial SMD Lite test code
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "smdl_profile.h"
#include "smdl_debug.h"

#include "rex.h"
#include "task.h"
#include "DDITimetick.h"

/*===========================================================================
                      LOCAL CONSTANT DECLARATIONS
===========================================================================*/

#define PROFILE_TIMER_SIG 0x10

/*===========================================================================
                              LOCAL DATA DECLARATIONS
===========================================================================*/

/** The TCB for the profiling thread. */
static rex_tcb_type         smdl_profile_tcb;

/** The stack for the profiling thread. */
static rex_stack_word_type  smdl_profile_stack[1024];

/** DAL Timetick handle */
static DalDeviceHandle* smd_timetick_handle = NULL;

/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/**
 * The SMD Lite profiling thread.
 *
 * smdl_profile_init() creates a REX task to run this function and the SMD
 * Lite profiling code.
 *
 * This function never returns.
 *
 * @param[in]  reserved  Ignored
 *
 * @returns    Never.
 */
static void smdl_profile_thread
(
  unsigned long reserved
)
{
  smdl_profile();

  /* REX tends to get anxious if the profile thread returns, so kill it off
   * explicitly. Under normal circumstances smdl_profile() won't return, but
   * if it's running test code it may return. */
  rex_kill_task(rex_self());

  return;
}

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/**
 * Initializes the SMD Lite profiling system.
 *
 * The profiling system will remain dormant until triggered by the debugger.
 *
 * @param[in]    reserved  Must be 0
 *
 * @return       @li 0 if the port is closed
 *               @li A negative error code if an error occurred.
 * @sideeffects  Creates a thread for SMD Lite profiling, which remains
 *               dormant until triggered by the debugger.
 */
int smdl_profile_init
(
  uint32 reserved
)
{
  rex_def_task_ext2(&smdl_profile_tcb,
                    (unsigned char *) smdl_profile_stack,
                    1024 * sizeof(rex_stack_word_type),
                    TASK_PRIO(smdtask)+1,
                    smdl_profile_thread,
                    0L,
                    "SMDL_PROFILE",
                    FALSE,
                    0 );

  return 0;
}

/**
 * Blocks the running thread.
 *
 * This is OS-specific and should be called only by the profiling code.
 *
 * @param[in]    ms  The number of milliseconds to suspend execution
 *
 * @return       None
 * @sideeffects  Temporarily suspends the calling thread
 */
void smdl_profile_wait
(
  uint32 ms
)
{
  rex_timer_type timer;

  rex_clr_sigs(rex_self(), PROFILE_TIMER_SIG);
  rex_def_timer(&timer, rex_self(), PROFILE_TIMER_SIG);
  rex_set_timer(&timer, ms);
  rex_wait(PROFILE_TIMER_SIG);
}

/**
 * Initializes the OS-specific Timetick handle, if necessary.
 * 
 * @return      SMDL_FAIL on failure
                SMDL_SUCCESS on success
 */
int32 smdl_profile_timetick_init( void )
{
  int32 ret;
  if ( (DalTimetick_Attach("SystemTimer", &smd_timetick_handle) != DAL_SUCCESS)
          || (smd_timetick_handle == NULL) )
  {
    SMDL_ERROR("smdl_profile: DAL timetick handle failed to attach.", 0, 0, 0);
    ret = SMDL_FAIL;
  }
  else
  {
    ret = SMDL_SUCCESS;
  }
  return ret;
}

/**
 * Returns the current timetick value.
 * 
 * @return      The current timetick value, measured in ticks.
 *              Returns 0 if an error occurred.
 */
uint32 smdl_profile_timetick_get( void )
{
  DalTimetickTime32Type time_ret;
  if (DalTimetick_Get(smd_timetick_handle, &time_ret) != DAL_SUCCESS)
  {
    SMDL_WARN("smdl_profile: Cannot get DAL timetick value.", 0, 0, 0);
    time_ret = 0;
  }
  return (uint32)time_ret;
}

/**
 * Returns the slow-clock frequency used for measuring timeticks.
 * 
 * @return      The timetick frequency.
 *              Returns 0 if an error occurred.
 */
uint32 smdl_profile_timetick_get_freq( void )
{
  uint32 freq;
  if (DalTimetick_GetFreq(smd_timetick_handle, &freq) != DAL_SUCCESS)
  {
    SMDL_WARN("smdl_profile: Cannot get DAL timetick frequency.", 0, 0, 0);
    freq = 0;
  }
  return freq;
}
