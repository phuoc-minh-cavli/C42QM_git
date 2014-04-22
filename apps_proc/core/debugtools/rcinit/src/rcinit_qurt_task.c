/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_qurt_task.c
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
Copyright (c) 2015, 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.tx/6.0/debugtools/rcinit/src/rcinit_qurt_task.c#2 $
$DateTime: 2020/07/21 10:05:31 $
$Change: 25009440 $
$Author: pwbldsvc $
===============================================================================*/

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
#include "tracer.h"
#include "rcinit_tracer_swe.h"
#endif

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
#include "msg.h"
#endif

#include "rcinit_qurt.h"
#include "tms_rcinit.h"
#include "sys_m_messages.h"

//static DALSYSEventHandle hEventCoreStarted;

#define RCINIT_SHUTDOWN_SIG_MASK   0x00000001

/*===========================================================================

 FUNCTION rcinit_task

 DESCRIPTION
 hlos task thread to process sequnece groups

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none

 ===========================================================================*/

void rcinit_task(void * arg_v)                                            // QURT API
{
   RCEVT_SIGEX_SIGQURT sig;
   qurt_signal_t rcinit_task_sig;
   uint32 sig_mask;

   rcinit_internal.process_state = rcinit_process_init;

   /* Initialize the signal object */
   qurt_signal_init(&rcinit_task_sig);

   // rcinit init
   rcinit_init();

   // register as client to system monitor
   sig.signal = &rcinit_task_sig;
   sig.mask = RCINIT_SHUTDOWN_SIG_MASK;
   if (rcevt_register_sigex_name(SYS_M_SHUTDOWN, RCEVT_SIGEX_TYPE_SIGQURT, &sig)) {;}

   // DAL DEPENDS ON QURT
   //if (DAL_SUCCESS != DALSYS_EventCtrl(hEventCoreStarted, DALSYS_EVENT_CTRL_TRIGGER))
   //{
   //ERR_FATAL("fail corestarted trigger", 0, 0, 0);
   //}

#if defined(RCINIT_TRACER_SWEVT)
   tracer_event_simple(RCINIT_SWE_INITGROUPS);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
   MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH, "group initializations end");
#endif

   // signal for observers
   if (rcecb_signal_name(RCINIT_RCECB_INITGROUPS)) {;}
   if (rcevt_signal_name(RCINIT_RCEVT_INITGROUPS)) {;}

   rcinit_internal.process_state = rcinit_process_blocking;

   // block for system monitor notification
   sig_mask = qurt_signal_wait(&rcinit_task_sig, RCINIT_SHUTDOWN_SIG_MASK, QURT_SIGNAL_ATTR_WAIT_ANY);
   if (sig_mask & RCINIT_SHUTDOWN_SIG_MASK)
   	{
   		qurt_signal_clear(&rcinit_task_sig, RCINIT_SHUTDOWN_SIG_MASK);
		
		// unregister further system monitor notifications
		if (rcevt_unregister_sigex_name(SYS_M_SHUTDOWN, RCEVT_SIGEX_TYPE_SIGQURT, &sig)) {;}
   	}

   rcinit_internal.process_state = rcinit_process_running; 

#if defined(RCINIT_TRACER_SWEVT)
   tracer_event_simple(RCINIT_SWE_TERMGROUPS);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
   MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH, "group terminations begin");
#endif

   // signal for observers
   if (rcevt_signal_name(RCINIT_RCEVT_TERMGROUPS)) {;}
   if (rcecb_signal_name(RCINIT_RCECB_TERMGROUPS)) {;}

   // rcinit termination
   rcinit_term();

   // client ack to system monitor
   if (rcevt_signal_name(SYS_M_SHUTDOWN_ACK)) {;}

   // termination
   rcinit_internal.process_state = rcinit_process_term;
   
   /* Destroy the signal object */
   qurt_signal_destroy(&rcinit_task_sig);
   
   if (arg_v) {;}
}

void rcinit_fail(void)
{
   static unsigned long unused = 0;

   for (;;)                                                                      // punt, no exception handler
   {
   }

   /* NOTREACHED */

   if (unused) {;}
}

void bootstrap_rcinit_task(void)
{
   int ret;

   // Initialize NHLOS Layers Needed

   // Must initialize the kernel REX API Layer; its not done by kernel
   // even on PL where the interface is available, and we are already
   // multi-tasking at this point by the NHLOS.

   { // bootstrap_rcinit_task
		static qurt_thread_attr_t qurt_attr;
		static qurt_thread_t qurt_tid;
   
		qurt_thread_attr_init(&qurt_attr);
		qurt_thread_attr_set_name(&qurt_attr, (char*)"RCINIT");
		qurt_thread_attr_set_stack_size(&qurt_attr, 6144);
		qurt_thread_attr_set_priority(&qurt_attr, rcinit_lookup_prio("rcinit"));
   
		ret = qurt_thread_create((qurt_thread_t*)(&qurt_tid), &qurt_attr, rcinit_task, (void*)0);

		if (QURT_EOK != ret)
			ERR_FATAL("qurt_thread_create failed for RCINIT task",0,0,0);
   }

   // Block for RCINIT Framework Startup to Complete

   //if ((DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &hEventCoreStarted, NULL)) ||
   //(DAL_SUCCESS != DALSYS_EventWait(hEventCoreStarted)))
   //{
   //ERR_FATAL("task wait event shutdown", 0, 0, 0);
   //}
}
