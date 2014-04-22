/*======================================================================================
                                cpu_monitor.c

GENERAL DESCRIPTION
  This file contains the implementation of the CPU monitor which is used to obtain the
  CPU statistics over intervals of time.

Copyright (c) 2011-2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
=========================================================================================*/
/*===========================================================================
                             Edit History

$Header: //components/rel/core.tx/6.0/power/dcvs/src/common/cpu_monitor.c#1 $ 
$DateTime: 2020/01/30 22:49:35 $ 
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/24/12   sg      Log events on QDSS tracer.
03/27/12   nj      Replaced DALSYS_SyncEnter with npa_resource_lock and 
                   DALSYS_SyncExit with npa_resource_unlock. These APIs use 
                   the correct/required argument signatures
09/28/11   sg      Adding DCVS_PARAMS_MONITOR_AUTO_ENABLE as an enum to 
                   correspond to the default CPUMonitor input parameters. 
                   Set non-zero default ulog buffer size; enable logging by
                   default.
09/25/11   sg      Modified to use strlcat instead of banned API strcat.
02/18/11   ss      Adding context based processing for DCVS.
01/24/11   ss      Initial.
===========================================================================*/
#include "BuildFeatures.h"
#ifndef DEFEATURE_CPU_MON_ARM
#include "cpu_monitor.h"
#include "dcvs_nv.h"
#include "dcvs_npa.h"
#include "dcvs_kernel.h"
#include "dcvs_trace.h"
#include "CoreVerify.h"
#include "ULogFront.h"
#include "stringl/stringl.h"

/* Invalid value used for logging */
#define INVALID_QUERY_STATE 0xFFFF

typedef enum
{
  DCVS_PARAMS_MONITOR_SAMPLE_PERIOD = 0,
  DCVS_PARAMS_MONITOR_AUTO_ENABLE,
  DCVS_PARAMS_MONITOR_LOG_SIZE,
} DCVS_PARAMS_MONITOR;


/* Data type to maintain the common/generic states of this layer of DCVS. These
   states are used by all the cores. */
typedef struct
{
  /* Flag to indicate that the NV items are initialized. */
  DALBOOL nvDataInit;

}CPU_MONITOR_GENERIC_DATA;

static CPU_MONITOR_GENERIC_DATA cpuMonGeneric;

/* Structure to maintain the data for the CPU Monitor for ARM */
typedef struct
{
  /* DCVS idle monitoring flag */
  uint32 enabled;

  /* Sampling period for cpu monitoring */
  uint32 samplePeriod;

  /* Log handle for cpu monitor log */
  ULogHandle logHandle;

  /* System start tick and stop tick for 
     sampling period during CPU monitoring. */
  uint64 sysStartTick, sysStopTick;

  /* Sleep start tick and stop tick for 
     sampling period during CPU monitoring. */
  uint64 sleepStartTick, sleepStopTick;

  /* System idle percentage for the sampling
     period during CPU monitoring. */
  int32 sysIdlePct;

  /* System idle override information. */
  uint32 idleOverride;

  /* System mips without idle override information. */
  uint32 mipsWithoutIdleOverride;

  /* System mips with idle override information.*/
  uint32 mipsWithIdleOverride;

  /* State of the system clock */
  uint32 clkState;

  /* Timer handle for the Cpu Monitor. */
  void* timerHandle;
  
}CPU_MON_DATA;


// Default Monitor NV Parameter set
//
static const DCVS_NV_PARAM_ENTRY dcvsParamsMonitor[] =
{
  {"SamplePeriod", DCVS_NV_PARAM_UINT32, (void*)50},
  {"AutoEnable", DCVS_NV_PARAM_UINT32, (void*)1},
  {"LogSize", DCVS_NV_PARAM_UINT32, (void*)16384},
  DCVS_NV_PARAM_LIST_END
};

/**
  @brief CpuMonitorSysStatCalc
  This function calculates for the past timer period of cpu monitoring 
  the following :: 
  1. The System start and Stop ticks.
  2. The CPU idle percentage.
  3. The idle override information of the system.
  4. The actual system frequency.
  5. The mips with and without idle override information. 

  @param context : The DCVS data context. 
  @return : None

*/

static void CpuMonitorSysStatCalc(DCVS_CONTEXT* context)
{
  uint32 tempMips;
  int32 idlePct;
  npa_query_status status;
 

  /* Obtain the core data context. */
  DCVS_CORE_DATA* coreDataContext = context->coreDataContext;

  /* Obtain the CPU monitor data context . */
  CPU_MON_DATA* cpuMonContext = (CPU_MON_DATA*)context->cpuMonContext;

  /* Query result of NPA resource */
  npa_query_type clockQueryResult;

  // System Tick Delta = System Tick Stop - System Tick Start
  idlePct = (uint32)( (cpuMonContext->sysStopTick - cpuMonContext->sysStartTick) & 0xFFFFFFFF);  
  if (idlePct > 0)
  {
    // (Sleep Tick Stop - Sleep Tick Start) * 100 / System Tick Delta
    idlePct = (cpuMonContext->sleepStopTick - cpuMonContext->sleepStartTick) * 100 / idlePct;  
  }
  else if (idlePct != 0)
  {
    /* No reasonable idle data. Indicate a failure */
    /* ToDo:: could something better than 0 be returned for this value? */
    idlePct = -1;
  }

  /* Record the system idle percentage */
  cpuMonContext->sysIdlePct = idlePct;

  /* Get the idle override information for the past timing interval */
  DCVSKernelIdleInfo(&cpuMonContext->idleOverride);

  /* Query the current state of clock resource . */
  status = npa_query(coreDataContext->clockQueryHandle, NPA_QUERY_CURRENT_STATE, &clockQueryResult);

  if ( status != NPA_QUERY_SUCCESS )
  {
    /* The resource query failed . Set the logging values to invalid */
    cpuMonContext->clkState = INVALID_QUERY_STATE;
  }
  else
  {
    cpuMonContext->clkState = clockQueryResult.data.state;
  }
  
  /* Use the target specific frequency conversion function. */
  tempMips = coreDataContext->clockPlan->KhzToMipsConvert(cpuMonContext->clkState);

  /* Calculate the mips without idle override */
  cpuMonContext->mipsWithoutIdleOverride = (tempMips*(100-idlePct))/100 ;

  if(idlePct < (int32)cpuMonContext->idleOverride)
  {
    /* Use the same mips without idle override infromation .*/
    cpuMonContext->mipsWithIdleOverride = cpuMonContext->mipsWithoutIdleOverride;
  }
  else
  {
    cpuMonContext->mipsWithIdleOverride = tempMips*(100-cpuMonContext->idleOverride)/100;
  } 

}/* CpuMonitorSysStatCalc*/

/**
  @brief CpuMonitorTimerStart
  This function is used to start the timer for the CPU monitor. 
  The required data is captured before the timer start. 

  @param context : The DCVS data context. 
  @return : None

*/

static void CpuMonitorTimerStart(DCVS_CONTEXT* context)
{
  /* Obtain the CPU monitor data context . */
  CPU_MON_DATA* cpuMonContext = (CPU_MON_DATA*)context->cpuMonContext;

  /* Get the idle stats at the beginning of the sample period */
	 cpuMonContext->sleepStartTick = DCVSKernelSleepTickGet(context);

	 /* Get the timer start ticks at the beginning of the sample period */
  cpuMonContext->sysStartTick = DCVSKernelSystemTickGet(context);


  /* Start the timer for Cpu Monitor */
	 DCVSKernelTimerStart(cpuMonContext->timerHandle, cpuMonContext->samplePeriod, context);

}


/**
  @brief CpuMonitorTimerStop
  This function is used to stop the timer for the CPU Monitor.
  The required data is captured after the timer stops.  
  
  @param context : The DCVS data context. 
  @return : None

*/
static void CpuMonitorTimerStop(DCVS_CONTEXT* context)
{
  /* Obtain the CPU monitor data context . */
  CPU_MON_DATA* cpuMonContext = (CPU_MON_DATA*)context->cpuMonContext;

  /* Start the timer for Cpu Monitor */
	 DCVSKernelTimerStop(cpuMonContext->timerHandle, context);

  /* Get the idle stats at the beginning of the sample period */
	 cpuMonContext->sleepStopTick = DCVSKernelSleepTickGet(context);

	 /* Get the timer start ticks at the beginning of the sample period */
  cpuMonContext->sysStopTick = DCVSKernelSystemTickGet(context);

}



/**
  @brief CpuMonitorProcessor
  
  This function is used to log the system statistics periodically ( based on the
  time period set previously )

  @param event_id: Event which caused callback to be fired.
  @param pserver_data: optional pointer to data supplied by sleep task.
  @param pclient_data: optional pointer to data passed by client at time 
  of registration.

*/
void CpuMonitorProcessor( dcvs_npa_event event_id,
						                    void* pserver_data,
						                    void* pclient_data )
{
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)pclient_data;
  /* Obtain the core data context. */
  DCVS_CORE_DATA* coreDataContext = context->coreDataContext;
  /* Obtain the CPU monitor data context . */
  CPU_MON_DATA* cpuMonContext = (CPU_MON_DATA*)context->cpuMonContext;

  if(cpuMonContext->enabled == TRUE)
  {
    /* Get the lock from the resource */
    npa_resource_lock(coreDataContext->clockResourceHandle);

    /* Stop the active timers if any */
	   CpuMonitorTimerStop(context);
    // printf("CpuMonitor Timer Stopped\n");

    /* Get the required stats for the system for the past timing interval */
    CpuMonitorSysStatCalc(context);

    /* Log the system stats for the past timing interval */
    ULogFront_RealTimePrintf(cpuMonContext->logHandle,
                             9,
                             "CPU_MONITOR (sysStartTick: %llu) (sysStopTick: %llu) (sysIdlePct: %d) (idleOverride: %d) (clock state: %d) (mipsWithIdleOverride: %d) (mipsWithoutIdleOverride: %d)", 
                             ULOG64_LOWWORD(cpuMonContext->sysStartTick),ULOG64_HIGHWORD(cpuMonContext->sysStartTick),
                             ULOG64_LOWWORD(cpuMonContext->sysStopTick),ULOG64_HIGHWORD(cpuMonContext->sysStopTick),
                             cpuMonContext->sysIdlePct,
                             cpuMonContext->idleOverride,
                             cpuMonContext->clkState,
                             cpuMonContext->mipsWithIdleOverride,
                             cpuMonContext->mipsWithoutIdleOverride
                             );
#if 0
    DcvsTraceCpuMonitor(4,
                        cpuMonContext->clkState,
                        cpuMonContext->sysStartTick,
                        cpuMonContext->sysStopTick,
                        cpuMonContext->sysIdlePct);
#endif
    /* Restart the cpu monitoring */
    CpuMonitorTimerStart(context);
    //printf("CpuMonitor Timer Started\n");

    npa_resource_unlock(coreDataContext->clockResourceHandle);

  }

}

/* Cpu Monitor monitor timer callback */
static void CpuMonitorTimerCb(DCVS_CONTEXT* context)
{
  /* The cpu monitor timer has fired */
  CpuMonitorProcessor(DCVS_NPA_EVENT_CLK_CHANGE, NULL, context);
}

/**
  @brief CpuMonitorDataMemoryAllocateAndInitialize
  
  This function is used to allocate memory for the CPU Monitor data.

  @param context: The DCVS data context.

  @return: None.

*/
static void CpuMonitorDataMemoryAllocateAndInitialize(DCVS_CONTEXT* context)
{
  CPU_MON_DATA* cpuMonContext;
  /* Allocate memory for the CPU Monitor data . */
  cpuMonContext = (CPU_MON_DATA*)Core_Malloc(sizeof(CPU_MON_DATA));
  if( NULL == cpuMonContext ) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);   
  }
  /* Initialize the memory allocated for the CPU monitor data */
  memset(cpuMonContext, 0, sizeof(CPU_MON_DATA));
  context->cpuMonContext = cpuMonContext;

}

void CpuMonitorInitialize(DCVS_CONTEXT* context)
{
  const byte logNameSizeMax = 128;
  /* The size of the cpu monitor log in bytes. */ 
  uint32 cpuMonLogBufSize = 0, i;
  /* The name of the log */
  char logName[logNameSizeMax], coreIdString[2];
  CPU_MON_DATA* cpuMonContext;
  ULogResult createResult;

  /* Initialize the CPU monitor NV data if not already done so. */
  if(cpuMonGeneric.nvDataInit == FALSE)
  {
    // Write the Default Monitor Parameters
    DCVSNvDefaultParamsInit ("Monitor", 
                             dcvsParamsMonitor);
    cpuMonGeneric.nvDataInit = TRUE;
  }

  /* Allocate the memory for the CPU monitor */
  CpuMonitorDataMemoryAllocateAndInitialize(context);

  cpuMonContext = (CPU_MON_DATA*)context->cpuMonContext;

  // Determine the proper size for our Monitor Log
  DcvsNvParamUint32Read(context->coreId, 
                        "Monitor", 
                        &dcvsParamsMonitor[DCVS_PARAMS_MONITOR_LOG_SIZE], 
                        &i);
  if (i > cpuMonLogBufSize)
  {
    cpuMonLogBufSize = i;
    /* If the size of the log is non-zero enable the logging */
    cpuMonContext->enabled = TRUE;
  }

  /* Convert the core ID to ASCII character. */
  coreIdString[0] = context->coreId+48;
  coreIdString[1] = '\0';
  
  strlcpy(logName, "CPUMon", logNameSizeMax);
  /* Append the core id to the log name string. */
  strlcat(logName, coreIdString, logNameSizeMax);

  /* Create the log for CPU monitor */
  createResult = ULogFront_RealTimeInit(&cpuMonContext->logHandle, 
                                        logName,
                                        cpuMonLogBufSize,  // Initial Buffer
                                        ULOG_MEMORY_LOCAL,
                                        ULOG_LOCK_NONE);

  if ((createResult != DAL_SUCCESS) &&
      (createResult != ULOG_ERR_INITINCOMPLETE))
  {
    cpuMonContext->enabled = FALSE;
  }

  ULogCore_HeaderSet(cpuMonContext->logHandle, "Content-Type: text/tagged-log-1.0; title=CPU Monitor");

  DcvsNvParamUint32Read(context->coreId, 
                        "Monitor", 
                        &dcvsParamsMonitor[DCVS_PARAMS_MONITOR_SAMPLE_PERIOD], 
                        &cpuMonContext->samplePeriod);

  /* Create the timer for the Cpu Monitor */
	 cpuMonContext->timerHandle = DCVSKernelTimerCreate(CpuMonitorTimerCb, context);

}



void CpuMonitorStart(DCVS_CONTEXT* context)
{
  CPU_MON_DATA* cpuMonContext;
  cpuMonContext = (CPU_MON_DATA*)context->cpuMonContext;
  if(cpuMonContext->enabled == TRUE)
  {
    /* Register to be notified when the state of the physical clock changes */
    dcvs_npa_events_register(DCVS_NPA_EVENT_CLK_CHANGE, CpuMonitorProcessor, context);

	   /* Get the idle stats at the beginning of the sample period */
	   cpuMonContext->sleepStartTick = DCVSKernelSleepTickGet(context);

	   /* Get the timer start ticks at the beginning of the sample period */
    cpuMonContext->sysStartTick = DCVSKernelSystemTickGet(context);

    /* Start the timer for Cpu Monitor */
	   DCVSKernelTimerStart(cpuMonContext->timerHandle, cpuMonContext->samplePeriod, context);

  }
}
#endif //DEFEATURE_CPU_MON_ARM





