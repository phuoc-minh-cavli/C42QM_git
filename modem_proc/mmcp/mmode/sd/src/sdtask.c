/*============================================================================
  FILE:       sdtask.c

  OVERVIEW:

              Copyright (c) 2012 Qualcomm Technologies Incorporated.
              All Rights Reserved.
              Qualcomm Confidential and Proprietary
============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/sd/src/sdtask.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/16/13   jvo     Added sd_is_init_complete() and sd_task_info.
??                 First version
===========================================================================*/

/*=============================================================================
  IMPORTANT:

  Do not directly use OS signal primitives in SD task code.  These have been
  abstracted out in a way that the same code can be run on multiple OSes - in
  particular, they will run in PC unit tests *and* on target.  If you need
  signals, use the sdtask_signal or sdtask_signalset functions.
=============================================================================*/


#include "sdtask.h"
#include "modem_mem.h"
#include "err.h"
#include "queue.h"
#include "atomic_ops.h"
#include "sddbg.h"
#include "sdss.h"

#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#include "sdcmd.h"
#endif

/*-----------------------------------------------------------------------------
  Map the various Windows defines to the single WIN32.
-----------------------------------------------------------------------------*/
#if !defined(WIN32) && (defined(_WIN32) || defined(WINNT))
  #define WIN32
#endif

/*-----------------------------------------------------------------------------
  Map the various target defines to the single TARGET_BUILD
-----------------------------------------------------------------------------*/
#if !defined(WIN32) && (defined(FEATURE_QDSP6) || defined(IMAGE_QDSP6_PROC))
  #define TARGET_BUILD
#endif


#if defined(WIN32)

#include "windows.h"

/*-----------------------------------------------------------------------------
  This is the signal that will be used to synchronize SUTE with the SD task
  thread.
-----------------------------------------------------------------------------*/
HANDLE  sd_start_signal;

#elif defined(TARGET_BUILD)

#include "dog_hb_qurt.h"                              /*Interface for new Watchdog services*/
#include <qurt_anysignal.h>
#include <qurt_timer.h>
#include "rcinit.h"
#include "rcevt_qurt.h"

static rex_tcb_type     *sd_tcb_ptr;

#endif


static sd_signal_t  sd_stop_signal;
static uint32       sdtask_thread_id;



#define SD_SIGNALS_MAX  32




#if defined(WIN32)

uint32 sdtask_get_current_thread_id(void)
{
  return GetCurrentThreadId();
}

#elif defined(TARGET_BUILD)

uint32 sdtask_get_current_thread_id(void)
{
  rex_tcb_type  *tcb = rex_self();
  return tcb->thread_id;
}

#endif



/*****************************************************************************
******************************************************************************

  Signal

  The sdtask_signal functions abstract the underlying OS signals in a way
  that these functions can be used both on Windows (for unit tests) and on
  target.  These are used in cases in which we need to wait only a single
  signal (as we do on the thread calling an API).  For waiting on multiple
  signals (as we do on the SD task thread), use sdtask_signalset.

******************************************************************************
*****************************************************************************/

#define NULL_SIGNAL ((sd_signal_t) 0)


#if defined(WIN32)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  Windows implementation of signal functions.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*----------------------------------------------------------------------------
  sdtask_signal_wait
    Block the calling thread until the signal passed in is set.
----------------------------------------------------------------------------*/
static sd_signal_t sdtask_signal_wait(
  sd_signal_t signal
  )
{
  if (signal != NULL_SIGNAL)
  {
    WaitForSingleObject((HANDLE) signal, INFINITE);
  }

  return signal;
}

/*----------------------------------------------------------------------------
  sdtask_signal_set
    Set the signal that is passed in, releasing any thread that is waiting
    in sdtask_signal_wait().
----------------------------------------------------------------------------*/
static void        sdtask_signal_set(
  sd_signal_t signal
  )
{
  if (signal != NULL_SIGNAL)
  {
    SetEvent((HANDLE) signal);
  }
}



#elif defined(TARGET_BUILD)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  QURT implementation of signal functions (for target).

  Since multiple threads cannot wait on different signal bits within a given
  qurt_anysignal_t, signals used to block threads while waiting for commands
  need to be distinct qurt_anysignal_t objects.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* This is the bit used in each of the signals used by commands */
#define SDTASK_QURT_SIG_BIT 0x00000001

static sd_signal_t sdtask_signal_wait(
  sd_signal_t signal
  )
{
  qurt_anysignal_t  *sig = (qurt_anysignal_t *) signal;

  if (signal != NULL_SIGNAL)
  {
    (void) qurt_anysignal_wait(sig, SDTASK_QURT_SIG_BIT);
    (void) qurt_anysignal_clear(sig, SDTASK_QURT_SIG_BIT);
  }

  return SDTASK_QURT_SIG_BIT;
}

static void        sdtask_signal_set(
  sd_signal_t signal
  )
{
  qurt_anysignal_t  *sig = (qurt_anysignal_t *) signal;

  if (signal != NULL_SIGNAL)
  {
    qurt_anysignal_set(sig, SDTASK_QURT_SIG_BIT);
  }
}


#endif
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  End of target-specific implementation of signal functions.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#if defined(TARGET_BUILD)

static dog_report_type  sd_dog_rpt = 0;

#endif


/*----------------------------------------------------------------------------
  sdtask_init_after_start
    Initialize after SD task is started.
----------------------------------------------------------------------------*/
void  sdtask_init_after_start(void)
{

  //sdss_read_policyman_config();

}

/*----------------------------------------------------------------------------
  sd_task
    The dispatch loop of the SD task.
----------------------------------------------------------------------------*/
void  sd_task(void)
{


  sdtask_init_after_start();

  /* Mark that we've completed init, so we can return valid values for
  ** utility functions called by other modules.
  */
  

}

