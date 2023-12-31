/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_ex_qurt.c
@brief This file contains the API for the Run Control Framework, API 2.1
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.tx/6.0/debugtools/example/src/rcinit_ex_qurt.c#1 $
$DateTime: 2020/01/30 22:49:35 $
$Change: 22400605 $
$Author: pwbldsvc $
===============================================================================*/

#include "err.h"
#include "rcinit_qurt.h"

/*===========================================================================

 FUNCTION rcinit_example_qurttask

 DESCRIPTION
 example task boiler plate

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none
 ===========================================================================*/

#define RCINIT_EXAMPLE_QURTTASK_MASK0  0x00000001
#define RCINIT_EXAMPLE_QURTTASK_MASK1  0x00000002
#define RCINIT_EXAMPLE_QURTTASK_MASKS  (RCINIT_EXAMPLE_QURTTASK_MASK0|RCINIT_EXAMPLE_QURTTASK_MASK1)

/* static */ struct                                                              // clients do not need to access this, expose the communications infrastructure by API
{
   boolean init;
   qurt_signal_t Events;

} rcinit_example_qurttask_events;

void rcinit_example_qurttask_client_signal_event0(void)                          // called by other task
{
   if (TRUE != rcinit_example_qurttask_events.init)
   {
      ERR_FATAL("event trigger", 0, 0, 0);
   }
   else
   {
      qurt_signal_set(&rcinit_example_qurttask_events.Events, RCINIT_EXAMPLE_QURTTASK_MASK0);
   }
}

void rcinit_example_qurttask_client_signal_event1(void)                          // called by other task
{
   if (TRUE != rcinit_example_qurttask_events.init)
   {
      ERR_FATAL("event trigger", 0, 0, 0);
   }
   else
   {
      qurt_signal_set(&rcinit_example_qurttask_events.Events, RCINIT_EXAMPLE_QURTTASK_MASK1);
   }
}

void rcinit_example_qurttask(void* arg_p)
{
   qurt_signal_init(&rcinit_example_qurttask_events.Events);

   rcinit_example_qurttask_events.init = TRUE;

   rcinit_handshake_startup();                                                   // *REQUIRED*

   for (;;)
   {
      unsigned long mask = qurt_signal_wait(&rcinit_example_qurttask_events.Events,
	  						RCINIT_EXAMPLE_QURTTASK_MASKS,
	  						QURT_SIGNAL_ATTR_WAIT_ANY);

      if (RCINIT_EXAMPLE_QURTTASK_MASK0 & mask)
      {
         qurt_signal_clear(&rcinit_example_qurttask_events.Events, RCINIT_EXAMPLE_QURTTASK_MASK0);
         mask &= ~RCINIT_EXAMPLE_QURTTASK_MASK0;
      }

      if (RCINIT_EXAMPLE_QURTTASK_MASK1 & mask)
      {
         qurt_signal_clear(&rcinit_example_qurttask_events.Events, RCINIT_EXAMPLE_QURTTASK_MASK1);
         mask &= ~RCINIT_EXAMPLE_QURTTASK_MASK1;
      }

      // ...

      if (0 != mask)
      {
         ERR_FATAL("unknown event mask", 0, 0, 0);
      }
   }

   /* NOTREACHED */

   rcinit_example_qurttask_events.init = FALSE;

   qurt_signal_destroy(&rcinit_example_qurttask_events.Events);

   if (arg_p) {;}                                                                // unused
}
