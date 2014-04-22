/*==============================================================================
  FILE:         uATS.c
  
  OVERVIEW:     This file implements the micro-Active Time Solver DDR functions
  
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/uATS/ddr/uATS.c#4 $
$DateTime: 2019/05/28 10:11:02 $
===============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "uATS.h"
#include "qurt.h"
#include "qurt_mutex.h"
#include "uSleep_log.h"
#include "timer.h"
#include "uSleep_solver.h"

#ifdef USLEEP_SUPPORT_TRANSITIONS
//#include "uSleep_node.h"
#endif

#ifdef UNPA_SUPPORT
#include "uSleep_npa.h"
#include "uSleep_npai.h"
#include "unpa.h"
#include "unpa_event.h"
#endif

/*==============================================================================
                              EXTERANL REFERENCES
  =============================================================================*/
/* Signal Object used for communicating with Active Time solver */
extern qurt_signal_t        g_uTimerSignal;
extern qurt_signal_t        g_uActiveSignal;
extern uSleep_solver_input  g_uSolverInput;
extern timer_type           g_uATSThresholdTimer;
extern qurt_mutex_t         g_uATSSignalMutex;


/* ATS thread stack */
extern uint8  g_uSleepSolverTaskStack[USLEEP_SOLVER_STACK_SIZE];

#ifdef UNPA_SUPPORT
extern unpa_query_handle    g_latencyQueryHandle;

void uATS_nodeChangeCB(unpa_event_type  eventType,
                       void             *eventData, 
                       void             *cbdata);
#endif /* UNPA_SUPPORT */

void uSleepNPA_initialize(void);

/*==============================================================================
                            GLOBAL FUNCTION DEFINITIONS
  =============================================================================*/
/*
 * uATS_init 
 */
void uATS_init(void)
{
  /*int                 status;
  qurt_thread_t       uSolverHandlerThread;
  qurt_thread_attr_t  uSolverHandlerAttr;	 */
	
#ifdef UNPA_SUPPORT
  unpa_event_handle latencyEventHandle;
  unpa_event_handle cpuVddEventHandle;

  /* Setup Solver Input Defaults */
  g_uSolverInput.latency_budget  = UINT32_MAX;
#endif

  g_uSolverInput.wakeup_deadline = UINT64_MAX;

  /* Initialize signal type */
  qurt_signal_init(&g_uActiveSignal);
  qurt_signal_init(&g_uTimerSignal);

  /* Init the signal mutex */
  qurt_mutex_init(&g_uATSSignalMutex);

  /* Defining threshold timer and a non-deferrable timer change event
   * Function type is used because uATS is not it's own thread and is invoked in
   * sleep context which needs special handling. */
  CORE_VERIFY(TE_SUCCESS == 
               timer_def_osal(&g_uATSThresholdTimer,       /* Timer obj */
                              &timer_null_group,           /* Timer group NULL - deferrable */
                              TIMER_FUNC1_CB_TYPE,         /* Notify type */
                              uATS_timerThreshHandlerCB,   /* Callback fcn */
                              0));                         /* User Context */
#if 0 
  CORE_VERIFY(TE_SUCCESS == timer_register_for_non_defer_expiry_change(
                              TIMER_NATIVE_OS_SIGNAL_TYPE,
                              &g_uTimerSignal,
                              USLEEP_SIGNAL_HARD_DURATION));

#ifdef USLEEP_SUPPORT_TRANSITIONS
  /* Setup uSleep nodes.  This needs to be done after the ATS signal init, and before any
   * query handles are created */
// uSleepNode_initialize();
#endif

#ifdef UNPA_SUPPORT
  uSleepNPA_initialize();

  /* This assumes that NPA initilization is done, and the node is already created */
  CORE_VERIFY_PTR(g_latencyQueryHandle = unpa_getQueryHandle(USLEEP_LATENCY_NODE_NAME));

  /* Register for a change event on the various nodes */
  CORE_VERIFY_PTR(latencyEventHandle = unpaEvent_addCallback(USLEEP_LATENCY_NODE_NAME,
                                                             UNPA_POST_CHANGE_EVENT,
                                                             uATS_nodeChangeCB,
                                                             (void*)USLEEP_SIGNAL_LATENCY));

  CORE_VERIFY_PTR(cpuVddEventHandle = unpaEvent_addCallback(USLEEP_CPUVDD_LPR_NODE_NAME,
                                                            UNPA_POST_CHANGE_EVENT,
                                                            uATS_nodeChangeCB,
                                                            (void*)USLEEP_SIGNAL_REGISTRY));
#endif /* UNPA_SUPPORT */

   /* Start the uATS thread */  
  qurt_thread_attr_init(&uSolverHandlerAttr);
  

  /* Stack memory has to be statically allocated due to island section
   * location requirement */
  qurt_thread_attr_set_stack_size(&uSolverHandlerAttr, USLEEP_SOLVER_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&uSolverHandlerAttr, (void *)g_uSleepSolverTaskStack); 

  /* Fill in the stack with a marker to try and find overflows */
  memset(g_uSleepSolverTaskStack, 0xF8, sizeof(g_uSleepSolverTaskStack));

  qurt_thread_attr_set_priority(&uSolverHandlerAttr, USLEEP_SOLVER_TASK_PRIORITY);
  qurt_thread_attr_set_name(&uSolverHandlerAttr, "uSleep_Solver");

  /* Create the DDR timer handler task in uImage section memory */
  status = qurt_thread_create(&uSolverHandlerThread, &uSolverHandlerAttr, uATS_SignalTask, NULL);

  CORE_LOG_VERIFY(status == QURT_EOK,
                  uSleepLog_printf(0, "uATS_init: creation failed"));

#endif
  uSleepLog_printf(0, "ATS Boot complete");

  /* Trigger the inputs corresponding to the available nodes and signal
   * ATS boot as completed */
  uATS_SetSignal(USLEEP_SIGNAL_HARD_DURATION  |
                 USLEEP_SIGNAL_LATENCY        |
                 USLEEP_SIGNAL_REGISTRY);
  return;
}
