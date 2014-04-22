/*==============================================================================
  FILE:         uSleep_os_pmi.c

  OVERVIEW:     This file provides uSleep PMI functions

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/os/qurt/ddr/uSleep_os_pmi.c#2 $
$DateTime: 2019/05/28 10:11:02 $
==============================================================================*/
#if defined(USLEEP_SUPPORT_UNCACHED_APCR)
#include "comdef.h"
#include "CoreVerify.h"
#include "qurt.h"
#include "uSleep_os.h"


/*==============================================================================
                            DEFINITIONS & TYPES
 =============================================================================*/
/* PMI interupt ID and signal mask used to register the OS signal
 * that will be triggered as part of the APCR bringup sequence. */
#define SLEEP_PMI_ID          0
#define SLEEP_PMI_MASK        (1 << 0)

/* PMI thread defs */
#define PMI_HANDLER_STACK_SIZE    1024
#define PMI_HANDLER_TASK_PRIORITY 3

/*==============================================================================
                             INTERNAL VARIABLES
 =============================================================================*/
/* Signal that the PMI task waits on from the OS to begin the APCR exit process */ 
static qurt_anysignal_t g_pmiSignal;
static uint8            g_pmiHandlerStack[PMI_HANDLER_STACK_SIZE];

/*==============================================================================
                            EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
#if 0 
/*
 * sleepOS_pmiHandler
 */
void uSleepOS_pmiHandler(void *context)
{
  /* Registering for PMI and associated signal */
  qurt_anysignal_init(&g_pmiSignal);

  /* Interrupt registration must be done in the context of the handler. */
  if(QURT_EOK != qurt_interrupt_register(SLEEP_PMI_ID, &g_pmiSignal, SLEEP_PMI_MASK))
  {
    CORE_VERIFY(0);
  }

  /* PMI interrupt monitoring and handling task */
  while(1)
  {
    qurt_anysignal_wait(&g_pmiSignal, SLEEP_PMI_MASK);
   
    /* Call handler */
    uSleepOS_PMIPerformer();

    /* Signal is handled - Clear it */
    qurt_anysignal_clear(&g_pmiSignal, SLEEP_PMI_MASK);

    /* Interrupt has been handled, inform QuRT */
    qurt_interrupt_acknowledge(SLEEP_PMI_ID);
  }

  CORE_VERIFY(0);
}

/**
 * uSleepOS_registerPMIHandler
 *
 * @brief As part interrupt registration with QuRT, this function creates 
 *        a thread that will execute function to handle the Power Management
 *        Interrupt (PMI).
 *
 * @note As per pthread specifics, must be called from thread which does not terminate
 *       before the newly created thread does i.e. usually from Sleep task entry 
 *       function.
 */
void uSleepOS_registerPMIHandler(void)
{
  qurt_thread_t      pmiHandlerThread;
  qurt_thread_attr_t pmiHandlerAttr;
  
  /* Initialize attribute */
  qurt_thread_attr_init(&pmiHandlerAttr);

  /* setting up attributes for pmi handler thread */
  qurt_thread_attr_set_name(&pmiHandlerAttr, "PMI_Handler");
  qurt_thread_attr_set_tcb_partition(&pmiHandlerAttr, 0);
  qurt_thread_attr_set_priority(&pmiHandlerAttr, PMI_HANDLER_TASK_PRIORITY );
  qurt_thread_attr_set_stack_size(&pmiHandlerAttr, PMI_HANDLER_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&pmiHandlerAttr, (void *)g_pmiHandlerStack);
  
  /* Fill in the stack with a marker to try and find overflows */
  memset(g_pmiHandlerStack, 0xF8, sizeof(uint8) * PMI_HANDLER_STACK_SIZE);

  /* Creating thread to run PMI handler */
  CORE_VERIFY(QURT_EOK == qurt_thread_create(&pmiHandlerThread, &pmiHandlerAttr,
                                             uSleepOS_pmiHandler, NULL));
  return;
}
#endif

#endif /* defined(USLEEP_SUPPORT_UNCACHED_APCR) */

