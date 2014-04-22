/******************************************************************//**
 * @file dcvs_kernel.c
 *
 * @brief Kernel (OS) interface implementation for L4 platforms
 *
 * Copyright (C) 2009 - 2011, 2012 by Qualcomm Technologies, Inc. All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.tx/6.0/power/dcvs/src/os/tx/dcvs_kernel.c#1 $ 
$DateTime: 2020/01/30 22:49:35 $

when       who     what, where, why
--------   ----    ---------------------------------------------------
10/26/12   nj      Initialized system clock value as per target config.
03/27/12   nj      Replaced DALSYS_SyncEnter with npa_resource_lock and 
				   DALSYS_SyncExit with npa_resource_unlock. These APIs 
				   use the correct/required argument signatures
03/14/12   nj      Added a macro(instead of hardcoding) startup delay value
09/24/11   sg      Synced to 2H09; general clean up to fit in 1H10 framework.
09/13/11   ss      Initial check in.
========================================================================*/

#include "CoreVerify.h"
#include "dcvs_kernel.h"
#include "dcvs_target.h"
#include "DALSys.h"
#include "timer.h"
//#include "KxProfile.h"
//#include "rex.h"


#define MAX_NUM_DCVS_TIMERS 4
#define XO_UNITS_TO_32_76KHZ_UNITS 586

/**
 * Timer used to holdoff DCVS for a certain amount of time at 
 * initial boot-up. 
 */

/* Current this macro is defined in custacinaaaaa.h file for 6S
 so we are overriding with 1S */
#ifdef DCVS_BOOT_UP_DELAY_TIME_MS
  #undef DCVS_BOOT_UP_DELAY_TIME_MS
#endif
 
#define DCVS_BOOT_UP_DELAY_TIME_MS 1000


//extern rex_tcb_type sleep_tcb;

static uint32 gClkTicksPerSec; 

/* DCVS Target Specific callbacks */
void DCVSTargetInitialize(void);

/* Data type to maintain the initial callback for the DCVS. This call back starts the DCVS algorithm after a specified delay.
   The timer in this type is different from the running DCVS timer.  */
typedef struct
{
  timer_type timer;
  /* The callback function for the timer. */
  dcvs_timer_cb timerCb;
    
} DCVS_TIMER_TYPE;

/* Data type to maintain the initial callback for the DCVS. This call back starts the DCVS algorithm after a specified delay.
   The timer in this type is different from the running DCVS timer.  */
typedef struct 
{
  timer_type timer;
  DCVSKernelCallback timerCb;

} DCVS_INITIAL_CB_TYPE;

typedef struct 
{
  /* The callback type used to create the delay in starting the DCVS algorithm .*/
  DCVS_INITIAL_CB_TYPE initialCbData;
  
  /* Lock to protect the critical sections */
  qurt_mutex_t syncLock; 

  /* The DCVS running timers. */
  DCVS_TIMER_TYPE DCVSTimers[MAX_NUM_DCVS_TIMERS]; 

} DCVS_OS_DATA;

/* Structure to maintain the parameters for the callback of the DCVS timer. */
typedef struct
{
  /* DCVS data context */
  DCVS_CONTEXT* context;
  /* The index of the DCVS timer. */
  uint32 timerIndex;
}DCVS_TIMER_CB_PARAM_TYPE;

/* The parameters for the callback of the DCVS timer */
DCVS_TIMER_CB_PARAM_TYPE DCVSCbParams[MAX_NUM_DCVS_TIMERS];


/* The rex timer call back for the DCVS timers */
void DCVSOsTimerCb(uint32 param)
{
  DCVS_CONTEXT* context = DCVSCbParams[param].context;
  uint32 timerIndex = DCVSCbParams[param].timerIndex;
  DCVS_OS_DATA* osDataContext = context->osDataContext;

  if(osDataContext->DCVSTimers[timerIndex].timerCb != NULL)
  {
    /* Do the callback on the timerIndex */
    osDataContext->DCVSTimers[timerIndex].timerCb(context);
  }
}


/**
  @brief dcvs_initial_cb
  
  This is the callback in the kernel which is invoked after the 
  requested delay in starting the DCVS algorithm. 
  This function starts the DCVS algorithm.  

  @param param: The DCVS data context. 
   
*/

void dcvs_initial_cb(uint32 param)
{
  /* Obtain the DCVS data context from the passed in parameter.  */
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)param;
  DCVS_OS_DATA* osDataContext = context->osDataContext;

  (*(DCVSKernelCallback)(osDataContext->initialCbData.timerCb))(context);
}


static void OsDataMemoryAllocateAndInitialize(DCVS_CONTEXT* context)
{
  uint32 i;
  DCVS_OS_DATA* osDataContext;
  /* Allocate the memory for the kernel data . */
  osDataContext = (DCVS_OS_DATA*)Core_Malloc(sizeof(DCVS_OS_DATA));
  if( NULL == osDataContext) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);   
  }

  /* Initialize the memory allocated for the OS data */
  memset(osDataContext, 0, sizeof(DCVS_OS_DATA));
  context->osDataContext = osDataContext;

  osDataContext = (DCVS_OS_DATA*)context->osDataContext;

  /* Create a lock for synchronization */
  qurt_mutex_init(&osDataContext->syncLock);

  for(i = 0; i < MAX_NUM_DCVS_TIMERS; i++)
  {
    /* Init the l4 timers . */
    osDataContext->DCVSTimers[i].timerCb = NULL;
    DCVSCbParams[i].context = NULL;
    DCVSCbParams[i].timerIndex = 0;
  }

}

void DCVSKernelInitialize(DCVS_CONTEXT* context)
{
  /* Obtain the OS data context. */
  DCVS_OS_DATA* osDataContext;

  /* Store the system ref clock value  */
  gClkTicksPerSec = context->coreDataContext->clockPlan->clkTicksPerSec;

  /* Allocate and initialize the OS specific data */
  OsDataMemoryAllocateAndInitialize(context);

  osDataContext = (DCVS_OS_DATA*)context->osDataContext;

  // ToDo: do we need all the critical sections?

  /* Define the Initial callback Timer. This timer starts the DCVS 
     algorithm after a specified delay. This is separate from the 
     running timers of DCVS. */
  //rex_def_timer_ex(&osDataContext->initialCbData.timer, dcvs_initial_cb, (uint32)context);
  timer_def_osal(&(osDataContext->initialCbData.timer), NULL, TIMER_FUNC1_CB_TYPE, dcvs_initial_cb, (time_osal_notify_data)context);

  /* Tell L4 to start idle/busy profiling. */
  //KxProfile_Start();
  qurt_start_profiling_Idle_thread();
}

void DCVSKernelCallbackCreate(DCVSKernelCallback cb, uint32 delay_ms, DCVS_CONTEXT *context)
{
  /* Obtain the kernel data context. */
  DCVS_OS_DATA* osDataContext = (DCVS_OS_DATA*)context->osDataContext;

  if (!cb)
    return;

  /* Serialize callback creation */
  // ToDo: do we need all the critical sections?
  qurt_mutex_lock(&osDataContext->syncLock);
  
  /* Set Callback Function. */
  osDataContext->initialCbData.timerCb = cb;

  /* Set Timer delay */
  //rex_set_timer(&osDataContext->initialCbData.timer, delay_ms);
  timer_set(&(osDataContext->initialCbData.timer), delay_ms, 0, T_MSEC);

  /* Unlock and return */
   qurt_mutex_unlock(&osDataContext->syncLock);
}

void DCVSKernelIdleInfo(uint32* idle_override)
{
  // Never override.
  *idle_override = 100;

  return;
}

DCVSKernelTimerHandle DCVSKernelTimerCreate(DCVSKernelCallback cb, DCVS_CONTEXT* context)
{
   uint32 index;
  /* Obtain the OS data context. */
  DCVS_OS_DATA* osDataContext = (DCVS_OS_DATA*)context->osDataContext;
  for(index = 0; index < MAX_NUM_DCVS_TIMERS; index ++)
  {
    if( NULL == osDataContext->DCVSTimers[index].timerCb )
    {
      /* Setthe callback function for this timer. */
      osDataContext->DCVSTimers[index].timerCb = cb;
      DCVSCbParams[index].context = context;
      DCVSCbParams[index].timerIndex = index;
          
      /* Define the rex timer */
      //rex_def_timer_ex(&osDataContext->DCVSTimers[index].timer, DCVSOsTimerCb, (uint32)index);
	  timer_def_osal(&(osDataContext->DCVSTimers[index].timer), NULL, TIMER_FUNC1_CB_TYPE, DCVSOsTimerCb, (time_osal_notify_data)index);
      return (DCVSKernelTimerHandle)(index+1);
    }
  }
  return NULL;
}

DALBOOL DCVSKernelTimerDestroy(DCVSKernelTimerHandle Handle, DCVS_CONTEXT* context)
{
  /* Obtain the OS data context. */
  DCVS_OS_DATA* osDataContext = (DCVS_OS_DATA*)context->osDataContext;
  uint32 timerHandle = (uint32)Handle;
  if (NULL == timerHandle)
  {
    return FALSE;
  }

  /* Validate the handle */
  if( NULL == osDataContext->DCVSTimers[timerHandle-1].timerCb ) 
  {
    return FALSE;
  }
  else
  {  
    osDataContext->DCVSTimers[timerHandle-1].timerCb = NULL;
    return TRUE;
  }
}

DALBOOL DCVSKernelTimerStart(DCVSKernelTimerHandle Handle, uint32 time_ms, DCVS_CONTEXT* context)
{
  /* Obtain the OS data context. */
  DCVS_OS_DATA* osDataContext = (DCVS_OS_DATA*)context->osDataContext;

  uint32 timerHandle = (uint32)Handle;

  if ((NULL == (timerHandle)) ||
      (timerHandle > MAX_NUM_DCVS_TIMERS) ||
      (NULL == osDataContext->DCVSTimers[timerHandle-1].timerCb))
  {
    return FALSE;
  }
  else
  {
    /* Set the timer to expire at the given timeout*/
    //rex_set_timer(&osDataContext->DCVSTimers[timerHandle-1].timer, time_ms);
	timer_set(&(osDataContext->DCVSTimers[timerHandle-1].timer), time_ms, 0, T_MSEC);
	
    return TRUE;
  }
}

void DCVSKernelTimerStop(DCVSKernelTimerHandle Handle, DCVS_CONTEXT* context)
{
  uint32 timerHandle = (uint32)Handle;
  /* Obtain the OS data context. */
  DCVS_OS_DATA* osDataContext = (DCVS_OS_DATA*)context->osDataContext;

  if ((NULL == (timerHandle)) ||(timerHandle > MAX_NUM_DCVS_TIMERS))
  {
    return;
  }
  /* Stop the active DCVS timer */
  //rex_clr_timer(&osDataContext->DCVSTimers[timerHandle-1].timer);
  (void)timer_clr(&osDataContext->DCVSTimers[timerHandle-1].timer, T_MSEC );
  

}

uint32 DCVSKernelTicksToMsConvert(uint32 sysTicks)
{
  /* This is a l4 OS platform. Convert the sclks into ms */
  return( (sysTicks*1000) / gClkTicksPerSec );
}

uint32 DCVSKernelStartUpDelay(void)
{
  /* This is a l4 platform. Delay is as defined by macro */
  return(DCVS_BOOT_UP_DELAY_TIME_MS);
}

void DCVSKernelCriticalSectionEnter(DCVS_CONTEXT* context)
{
  npa_resource_lock(context->resDataContext->resHandle);
}

void DCVSKernelCriticalSectionExit(DCVS_CONTEXT* context)
{
  npa_resource_unlock(context->resDataContext->resHandle);
}

void* DCVSKernelDynamicMemoryAllocate(uint32 size)
{
  return(malloc(size));
}



uint64 DCVSKernelSleepTickGet(DCVS_CONTEXT* context)
{
  return qurt_get_profiling_time_Idle();
  //return (uint64)(qurt_get_profiling_time_Idle() / XO_UNITS_TO_32_76KHZ_UNITS );

}

uint64 DCVSKernelSystemTickGet(DCVS_CONTEXT* context)
{
  return qurt_profile_getTotalTime_IdleThread();
  //return (uint64)(qurt_profile_getTotalTime_IdleThread() / XO_UNITS_TO_32_76KHZ_UNITS);
}
