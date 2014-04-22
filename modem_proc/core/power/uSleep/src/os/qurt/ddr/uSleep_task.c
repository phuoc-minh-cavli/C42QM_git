/*==============================================================================
  FILE:         uSleep_init.c

  OVERVIEW:     This file provides uSleep task setup functions for standalone use

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/os/qurt/ddr/uSleep_task.c#2 $
$DateTime: 2019/05/28 10:11:02 $
==============================================================================*/
#include "comdef.h"
#include "qurt.h"
#include "CoreVerify.h"
#include "uSleep_os.h"

/*==============================================================================
                                  DEFINITIONS
 =============================================================================*/
/* Defintions for the usleep tasks */
#define USLEEP_STACK_SIZE         1536
#define USLEEP_TASK_PRIORITY      253

/*==============================================================================
                                  INTERNAL VARIABLES
 =============================================================================*/
/* Creating static stack space for the usleep tasks */
static uint8 g_uSleepStack[USLEEP_STACK_SIZE];


/* Function pointer table for different Sleep behavior based on various
 * restrictions
 *
 *  DEFAULT   : Execute Idle-time Solver
 *  OVERRIDE  : qurt_override_wait_for_idle is used to perform lpm
 *  LPI       : Low power island function is used
 *  HOLD_OFF  : No Low-power Mode selection is performed
 */
uSleep_idle_entry_ptr_type g_uSleepIdleEntryTable[USLEEP_OS_IDLE_MODE_NUM_STATES] = 
{
  uSleepOS_performLPM,      /* USLEEP_OS_IDLE_MODE_DEFAULT */
  uSleepOS_performLPM,      /* USLEEP_OS_IDLE_MODE_OVERRIDE  
                            * Reuse same function since override is only a kernel setting */
  uSleepOS_holdoffNoMode    /* USLEEP_OS_IDLE_MODE_HOLD_OFF */
};

/* Pointer to use as the entry point when the system is idle. */
volatile uSleep_idle_entry_ptr_type * volatile   g_uIdleEntryPtr = &g_uSleepIdleEntryTable[USLEEP_OS_IDLE_MODE_HOLD_OFF];

/*==============================================================================
                               EXTERNAL FUNCTIONS
 =============================================================================*/
void uSleepOS_registerPMIHandler(void);

/*==============================================================================
                               EXTERNAL VARIABLES
 =============================================================================*/
extern boolean g_uSleepRunExit;

/*==============================================================================
                            INTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/**
 * uSleepOS_mainTask
 * 
 * @brief Main sleep task that will call various low power modes
 *        when the system goes idle
 */
static void uSleepOS_mainTask(void *ignored)
{
  uSleep_idle_entry_ptr_type uSleepFunctionPtr;
  
  /* Main sleep task processing - Enter as many low power modes as possible. */
  while(1)
  {
    uSleepFunctionPtr = *g_uIdleEntryPtr;

    if(uSleepFunctionPtr != NULL)
    {
      /* Call the idle entry point.  This function pointer may be modified
       * by the /sleep/idle/plugin resource. */
      uSleepFunctionPtr();
    }
	
    
    /* Check if we need to execute exit path from here. It might already 
     * have been executed from another context. */
    if(TRUE == g_uSleepRunExit)
    {
      /* Call exit function */
      uSleepOS_performLPMExit();
    }
  }

  /* Should never hit here */
  CORE_VERIFY(0);
}

/**
 * sleepOS_taskInit
 * 
 * @brief RCInit function that will create the main sleep task and initilize 
 *        all of the sleep data structures 
 */
static void uSleepOS_taskInit(void)
{
  qurt_thread_t       uSleepThread;
  qurt_thread_attr_t  uSleepAttr;

  /* Initialize main uSleep thread attributes */
  qurt_thread_attr_init(&uSleepAttr);

  /* setting up attributes for the sleep thread */
  qurt_thread_attr_set_name(&uSleepAttr, "uSleep");
  qurt_thread_attr_set_tcb_partition(&uSleepAttr, 0);
  qurt_thread_attr_set_priority(&uSleepAttr, USLEEP_TASK_PRIORITY);

  /* Stack memory has to be statically allocated due to island section location requirement */
  qurt_thread_attr_set_stack_size(&uSleepAttr, USLEEP_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&uSleepAttr, (void *)g_uSleepStack);
  
  /* Fill in the stack with a marker to try and find overflows */
  memset(g_uSleepStack, 0xF8, sizeof(uint8) * USLEEP_STACK_SIZE);

  /* Create the main sleep task in island section memory */
  CORE_VERIFY(QURT_EOK == qurt_thread_create(&uSleepThread, &uSleepAttr,
                                             uSleepOS_mainTask, NULL));

  return;
}


/*==============================================================================
                            EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleepOS_initializeTask
 */
void uSleepOS_initializeTask(void)
{
  /* Setup the main uSleep task */
  uSleepOS_taskInit();

#if defined(USLEEP_SUPPORT_UNCACHED_APCR)
  /* Setup the PMI interrupt handler task */
  uSleepOS_registerPMIHandler();
#endif

  return;
}

