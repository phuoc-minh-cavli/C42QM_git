/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          L 2   M A I N

GENERAL DESCRIPTION
   This module contains the entry point for the L2 task. It
   calls initialisation functions for the tasks, then sits in an event
   loop.

EXTERNALIZED FUNCTIONS
  gsm_l2_main
    L2 'task' entry point.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2017 by QUALCOMM Technologies, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L2/vcs/l2_main.c_v   1.21   17 Jul 2002 16:09:00   jachan  $
$Header: //components/rel/geran.mpss/5.2.0/gl2/static/l2_static_main.c#3 $ $DateTime: 2019/01/01 22:24:35 $ $Author: pwbldsvc $

when       who       what, where, why
--------   ---       ---------------------------------------------------------
22/11/17   RRP       Initial version.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "geran_tasks.h"
#include "gs.h"
#include "task.h"
#include "rcinit.h"
#include "rex.h"
#include "dog_hb_rex.h"
#ifdef FEATURE_GERAN_TRM_EMULATOR
#error code not present
#endif /* FEATURE_GERAN_TRM_EMULATOR */
#include "l2_main.h"
#include "l2_intf.h"
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
#include "geran_dmm.h"
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

static dog_report_type l2_dog_handle = 0;      /* GL2 watchdog reference */

/*****************************************************

 GLOBALS

******************************************************/

/*****************************************************

                         FUNCTION DECLARATIONS

******************************************************/

/*===========================================================================

FUNCTION  gsm_l2_main

DESCRIPTION
  This function is the entry point for the L2 task. It calls
  initialisation functions to perform task initialisation for the L2
  task, then sits in an event loop waiting to process messages on its
  command queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

/*lint -save -e715 */

void gsm_l2_main( dword unused )
{
  rex_sigs_type sigs;        /* This task's signals */

  geran_tcb_set(GERAN_TASK_ID_GL2, rex_self());

  /* Perform startup init here. Satisfy dependency of other tasks
     provided by this task. Do not linger here, startup latency is
     impacted. Higher priority tasks in the same group will preempt
     this task at this point. Do not rely upon priority alone as the
     sole mechanism to arbitrate resource contention with other tasks. */

  rcinit_handshake_startup(); /* required rcinit handshake */

#ifndef FEATURE_EXTERNAL_PAGING_GERAN
  gsm_l2_fn_init();
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */

  /* register with Dog Heartbeat API */
  l2_dog_handle = dog_hb_register_rex((rex_sigs_type)L2_RPT_TIMER_SIG);

  /* Perform remaining init here. This is sequenced by a start
     signal delivered at the end of the sequence group processing. */

#ifdef FEATURE_GERAN_TRM_EMULATOR
  #error code not present
#endif /* FEATURE_GERAN_TRM_EMULATOR */

  for (;;)
  {
    sigs = rex_wait( L2_RPT_TIMER_SIG | L2_CMD_Q_SIG);

    if (sigs & L2_RPT_TIMER_SIG)
    {
      (void)rex_clr_sigs(rex_self(), L2_RPT_TIMER_SIG);
      dog_hb_report(l2_dog_handle);
    }

    if (sigs & L2_CMD_Q_SIG)
    {
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
      /* Read the queue only when we are loaded */
      if (is_geran_loaded() == TRUE)
      {
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */
        gs_read_messages(GS_QUEUE_LAYER_2);
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
      }
      else
      {
        MSG_GERAN_ERROR_0("L2_CMD_Q_SIG rcvd while unloaded");
      }
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */
    }
  }

  /* deregister with Dog Heartbeat API */
  dog_hb_deregister(l2_dog_handle);

  /*UNREACHED*/

  NOTUSED(unused); /* silence compiler warnings of unused parameter */
}

/* Normalise naming */
void gsm_l2_task (dword dummy)
{
   gsm_l2_main(dummy);
}

void l2_generate_filler(byte *spare_octets, uint16 num_octets)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GL2_INTF_ID, geran_gl2_intf_s, l2_generate_filler, spare_octets, num_octets);
#else
  l2_generate_filler_dyn(spare_octets, num_octets);
#endif /* !FEATURE_EXTERNAL_PAGING_GERAN */

  return;
}

