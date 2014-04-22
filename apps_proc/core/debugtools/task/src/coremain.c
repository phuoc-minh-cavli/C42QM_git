/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M A I N   M O D U L E

GENERAL DESCRIPTION
  This module contains the Main procedure for the AMSS Mobile software.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 1992-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtools/task/src/coremain.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#include "qurt.h"
#include "task.h"
#include "err.h"
#include "qurt.h"

#define RCINIT_TASK_STACK_SIZE   6144


void test_posix_timer(void); // bringup only
void err_exception_handler(void);

extern void rcinit_task(void * arg_v);                                            // QURT API
/*===========================================================================
FUNCTION MAIN

DESCRIPTION
  This procedure uses the init procedure to call the mc_task task.

DEPENDENCIES
  None

RETURN VALUE
  None, this routine does not return

SIDE EFFECTS
  None
===========================================================================*/
int main(void)
{
   unsigned long i;
   int ret;

   qurt_signal_t coremain_sig_obj;

   { // kernel emulation lib should init

#ifdef FEATURE_QUARTZ_3X
      //__amss_maintask_init();
#endif

#ifdef FEATURE_QUARTZ_3X
      //thread_declare_ex_handler(L4_Myself());
#endif
   }

   { // bootstrap_rcinit_task
		static qurt_thread_attr_t qurt_attr;
		static qurt_thread_t qurt_tid;
   
		qurt_thread_attr_init(&qurt_attr);
		qurt_thread_attr_set_name(&qurt_attr, (char*)"RCINIT");
		qurt_thread_attr_set_stack_size(&qurt_attr, RCINIT_TASK_STACK_SIZE);
		qurt_thread_attr_set_priority(&qurt_attr, rcinit_lookup_prio("rcinit"));
   
		ret = qurt_thread_create((qurt_thread_t*)(&qurt_tid), &qurt_attr, rcinit_task, (void*)0);

		if (QURT_EOK != ret)
			ERR_FATAL("qurt_thread_create failed for RCINIT task",0,0,0);
   }

   /* Nominal execution path requires ERR */

   err_exception_handler();    /* Call exception handler, does not return */

   // protection from stub versions of err_exception_handler() that return
   qurt_signal_init(&coremain_sig_obj);

   for (i = 0; i < 0xffffffff; i++)
   {
	  qurt_signal_clear(&coremain_sig_obj, qurt_signal_wait(&coremain_sig_obj, 1, QURT_SIGNAL_ATTR_WAIT_ANY));
   }

   return (0);

} /* END main */
