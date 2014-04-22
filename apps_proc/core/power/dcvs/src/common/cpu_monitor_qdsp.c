/*======================================================================================
                                cpu_monitor.c

GENERAL DESCRIPTION
  This file contains the implementation of the CPU monitor which is used to obtain the
  CPU statistics over intervals of time.

Copyright (c) 2011-2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
=========================================================================================*/
/*===========================================================================
                             Edit History

$Header: //components/rel/core.tx/6.0/power/dcvs/src/common/cpu_monitor_qdsp.c#1 $ 
$DateTime: 2020/01/30 22:49:35 $ 
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/24/12   sg      Log events on QDSS tracer.
03/29/12   nj      Added header macros that include target specific include files 
03/23/12   nj      Removed "blast_timer.h" for compatibility with 8974
03/08/12   nj      Set Default NV Value for CPUMonitor Log Size to 0, CPUMonitor
				   Logging is disabled by default. 	 
12/13/11   nj      Fixed compiler warnings.
09/28/11   sg      Fixed compiler warnings.
09/27/11   ss      Adding DCVS_PARAMS_MONITOR_QDSP_AUTO_ENABLE as an enum to 
                   correspond to the default CPUMonitor input parameters. 
09/12/11   ss      Use the ULOG Locks for logging CPUMonitor data and enable auto logging
                   of CPUMonitor. 
09/12/11   ss      Use of /core/cpu resource locks to control synchronisation within 
                   CPU Monitor.
04/26/11   ss      Fixing Q6 compiler warnings.
04/14/11   ss      Fixing the system profiling functionality.
02/18/11   ss      Adding context based processing for DCVS.
01/24/11   ss      Initial.
===========================================================================*/
#include "BuildFeatures.h"
#ifndef DEFEATURE_CPU_MON_QDSP

#include "cpu_monitor.h"
#include "dcvs_nv.h"
#include "CoreString.h"
#include "dcvs_npa.h"
#include "dcvs_kernel.h"
#include "dcvs_trace.h"
#include "CoreVerify.h"
#include "ULogFront.h"
#include "stringl/stringl.h"

#include DCVS_OS_HEADER1_H
#include DCVS_OS_HEADER2_H

/* Invalid value used for logging */
#define INVALID_QUERY_STATE 0xFFFF

typedef enum
{
  DCVS_PARAMS_MONITOR_QDSP_SAMPLE_PERIOD = 0,
  DCVS_PARAMS_MONITOR_QDSP_AUTO_ENABLE,
  DCVS_PARAMS_MONITOR_QDSP_LOG_SIZE,
} DCVS_PARAMS_MONITOR_QDSP;


/* Data type to maintain the common/generic states of this layer of DCVS. These
   states are used by all the cores. */
typedef struct
{
  /* Flag to indicate that the NV items are initialized. */
  DALBOOL nvDataInit;

}CPU_MONITOR_QDSP_QDSP_GENERIC_DATA;

static CPU_MONITOR_QDSP_QDSP_GENERIC_DATA cpuMonGeneric;

/* Structure to maintain the data for the CPU Monitor for QDSP */
typedef struct
{
    /* DCVS idle monitoring flag */
  uint32 enabled;

  /* Sampling period for cpu monitoring */
  uint32 samplePeriod;

  /* Log handle for cpu monitor log */
  ULogHandle logHandle;

  uint32 timerHandle;

  uint32 threadTickIndex;
  /* Array to store the core processor cycles.  */
  uint32 threadTickCounts[2];
  /* Array to store the idle  processor cycles. */
  uint32 threadIdleCounts[2][MAX_THREADS];
  uint32 threadIdlePct[MAX_THREADS];
  uint32 threadMips[MAX_THREADS];

  /* Number of ticks elapsed on BLAST system clock */
  uint32 sysTickStart;
  uint32 sysTickStop;
  
}CPU_MON_QDSP_DATA;


// Default Monitor NV Parameter set
//
static const DCVS_NV_PARAM_ENTRY dcvsParamsMonitorQdsp[] =
{
  {"SamplePeriod", DCVS_NV_PARAM_UINT32, {(void*)50}},
  {"AutoEnable", DCVS_NV_PARAM_UINT32, {(void*)1}},
  {"LogSize", DCVS_NV_PARAM_UINT32, {(void*)0}},
  DCVS_NV_PARAM_LIST_END
};

/**
  @brief CpuMonQdspSysStatCalc
  This function calculates for the past timer period of cpu monitoring 
  the following :: 
  1. The System start and Stop ticks.
  2. The CPU idle percentage.
  3. The idle override information of the system. 

  @param context : The DCVS data context.

  @return : none.

*/

static void CpuMonQdspSysStatCalc(DCVS_CONTEXT* context)
{
  uint32 oldThreadTickIndex, i, tempMips;
  unsigned long long tpcycles;
  unsigned long long ticycles[MAX_THREADS];
  uint32 clockKhz;
  npa_query_type queryResult;

  /* Variable to hold the number of system ticks on BLAST system clock per thread. */
  uint32 sysTickTotal;

  /* Obtain the core data context. */
  DCVS_CORE_DATA* coreDataContext = context->coreDataContext;

  /* Obtain the CPU monitor data context . */
  CPU_MON_QDSP_DATA* cpuMonContext = (CPU_MON_QDSP_DATA*)context->cpuMonContext;

  uint32 threadBusyPct ;

  uint32 net_hwthx_idle_pcycles, net_cpu_pcycles;
  
  uint32 busy_pcycles;

  oldThreadTickIndex = cpuMonContext->threadTickIndex;

  /* Switch the thread tick index from 1 to 0 and viceversa everytime */
  cpuMonContext->threadTickIndex ^= 1;

  /* Get the current count of idle processor cycles. */
  blast_profile_get_idle_pcycles(ticycles);

  /* Get the current count of core processor cycles executed. */
  tpcycles = blast_get_core_pcycles();

  cpuMonContext->threadTickCounts[cpuMonContext->threadTickIndex] = (uint32)tpcycles;

  for (i = 0; i < MAX_THREADS; i++)
  {    
    cpuMonContext->threadIdleCounts[cpuMonContext->threadTickIndex][i] = (uint32)ticycles[i];
  }

  /*
    Convert the sclks into processor cycles. 
    For each thread ,
    threadBusyPCycles = (threadTotalCorePCycles - threadIdlePCycles)/MAX_THREADS ;
      where,
        threadTotalCorePCycles is obtained from API blast_get_core_pcycles()
        threadIdlePCycles is obtained from API blast_profile_get_idle_pcycles()
 
    threadTotalSysPCycles = (sysFreq * sysTickTotal)/32768 ;
      where,
        sysFreq is the current operating frequency of the system in khz ( kilo hz or kilo pcycles / sec ) 
        The units of ( sysTickTotal/32768 ) is secs. So the units of threadTotalSysPCycles is pcycles . 
 
    busyPct  = busyPCycles/threadTotalSysPCycles ;
  */
  
  /* System ticks at the end of the timing interval */
  cpuMonContext->sysTickStop = blast_system_sclk_attr_gethwticks();

  /* The total ticks elapsed in this timer interval in sclks */
  sysTickTotal = cpuMonContext->sysTickStop - cpuMonContext->sysTickStart; 

  /* Query the current state of /clk/cpu resource */
  npa_query(coreDataContext->clockQueryHandle, NPA_QUERY_CURRENT_STATE, &queryResult);  
  clockKhz = queryResult.data.state;

  /* Use the target specific frequency conversion function. */
  tempMips = coreDataContext->clockPlan->KhzToMipsConvert(clockKhz);

  /*
    Delta CPU % = (delta_cpu_run_time/6 - delta_hwthx_idle_time ) / delta_system_run_time;
    delta_cpu_run_time = net_cpu_pcycles/core_clock_freq ;
    delta_hwthx_idle_time = net_hwthx_idle_pcycles / ( 6* core_clock_freq) ;
    net_hwthx_idle_pcycles = hwthx_idle_pcycles - prev_hwthx_idle_pcycles;
    delta_system_run_time = net_ticks * 1M  / sleep_clk_freq;
  */
  net_cpu_pcycles = cpuMonContext->threadTickCounts[cpuMonContext->threadTickIndex] - cpuMonContext->threadTickCounts[oldThreadTickIndex];


  /*
    Calculate the busy_pycles in terms of kilo pcycles
    busy_pcycles = (net_cpu_pcycles - net_hwthx_idle_pcycles)/1000;

where,
  net_cpu_pcycles is the delta from blast_get_core_pcycles for the sampling interval.
  net_hwthx_idle_pcycles is the delta from blast_profile_get_idle_pcycles for the sampling interval for each thread.

  threadBusyPct = (busy_pcycles*3276)/(tempMips*sysTickTotal);

where   
   sysTickTotal is in sclks obtained from blast_system_sclk_attr_gethwticks. We convert that to pcycles as well.
  Note that the denominator uses the clock freq in mips because the numerator was in Kilo PCycles.

  
  */


  // Convert to Idle Pct
  for (i = 0; i < MAX_THREADS; i++)
  {
    net_hwthx_idle_pcycles = cpuMonContext->threadIdleCounts[cpuMonContext->threadTickIndex][i] - 
                             cpuMonContext->threadIdleCounts[oldThreadTickIndex][i];

    /* Calculate the busy pcycles with resolution.Kilo PCycles  */
    if(net_cpu_pcycles > net_hwthx_idle_pcycles )
    {
      busy_pcycles = (net_cpu_pcycles - net_hwthx_idle_pcycles)/1000;
    }
    else
    {
      busy_pcycles = 0; 
    }

    threadBusyPct = (busy_pcycles*3276)/(tempMips*sysTickTotal);
    if((threadBusyPct >= 0)&&(threadBusyPct <= 100))
    {
      cpuMonContext->threadIdlePct[i] = 100 - threadBusyPct;
    }
    else
    {
      cpuMonContext->threadIdlePct[i] = 0;
    }


    //ULogFront_RealTimePrintf(cpuMonContext->logHandle,
    //                         5,
    //                         "Thread_Data (id: %d)(net_hwthx_idle_pcycles: %d)(net_cpu_pcycles: %d)(sysTickTotal: %d)(busyPct: %d)", 
    //                         i,
    //                         net_hwthx_idle_pcycles,
    //                         net_cpu_pcycles,
    //                         sysTickTotal,
    //                         threadBusyPct
    //                         );

  }
  
  for (i = 0; i < MAX_THREADS; i++)
  {
    cpuMonContext->threadMips[i] = ((100 - cpuMonContext->threadIdlePct[i])*tempMips)/100;
  }

  /* Log the system stats for the past timing interval */
  ULogFront_RealTimePrintf(cpuMonContext->logHandle,
                           3,
                           "CPU_MONITOR_QDSP (CPU khz: %d) (tick stop:%d) (tick start:%d)", 
                           clockKhz,
                           cpuMonContext->sysTickStop,
                           cpuMonContext->sysTickStart);
  
  for (i = 0; i < MAX_THREADS; i++)
  {
    ULogFront_RealTimePrintf(cpuMonContext->logHandle,
                             4,
                             "CPU_MONITOR_QDSP (Thread %d Idle Pct: %d) (Thread %d MIPS: %d)", 
                             i,
                             cpuMonContext->threadIdlePct[i],
                             i,
                             cpuMonContext->threadMips[i]);
  }
  
  DcvsTraceCpuMonitor(6,
                      clockKhz,
                      cpuMonContext->sysTickStart,
                      cpuMonContext->sysTickStop,
                      cpuMonContext->threadIdlePct[0],
                      cpuMonContext->threadIdlePct[1],
                      cpuMonContext->threadIdlePct[2]);

}/* CpuMonQdspSysStatCalc*/


/**
  @brief CpuMonQdspProcessor
  
  This function is used to log the system statistics periodically ( based on the
  time period set previously )

  @param event_id: Event which caused callback to be fired.
  @param pserver_data: optional pointer to data supplied by sleep task.
  @param pclient_data: optional pointer to data passed by client at time 
  of registration.

*/
void CpuMonQdspProcessor( dcvs_npa_event event_id,
						                    void* pserver_data,
						                    void* pclient_data )
{
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)pclient_data;
  /* Obtain the CPU monitor data context . */
  CPU_MON_QDSP_DATA* cpuMonContext = (CPU_MON_QDSP_DATA*)context->cpuMonContext;

  if(cpuMonContext->enabled == TRUE)
  {
    /* Get the lock from the resource */
    DCVSKernelCriticalSectionEnter(context);
    /* Stop the active timers if any */
	   DCVSKernelTimerStop((DCVSKernelTimerHandle)cpuMonContext->timerHandle, context);
    // printf("CpuMonQdsp Timer Stopped\n");

    /* Get the required stats for the system for the past timing interval */
    CpuMonQdspSysStatCalc(context);

    /* System ticks at the beginning of the timing interval */
    cpuMonContext->sysTickStart = blast_system_sclk_attr_gethwticks();
    
    /* Start the timer for Cpu Monitor */
    DCVSKernelTimerStart((DCVSKernelTimerHandle)cpuMonContext->timerHandle, cpuMonContext->samplePeriod, context);

    DCVSKernelCriticalSectionExit(context);

  }

}

/* Cpu Monitor monitor timer callback */
static void CpuMonQdspTimerCb(DCVS_CONTEXT* context)
{
  /* The cpu monitor timer has fired */
  CpuMonQdspProcessor(DCVS_NPA_EVENT_CLK_CHANGE, NULL, context);
}

/**
  @brief CpuMonQdspDataMemoryAllocateAndInitialize
  
  This function is used to allocate memory for the CPU Monitor data.

  @param context: The DCVS data context.

  @return: None.

*/
static void CpuMonQdspDataMemoryAllocateAndInitialize(DCVS_CONTEXT* context)
{
  
  CPU_MON_QDSP_DATA* cpuMonContext;
  /* Allocate memory for the CPU Monitor data . */
  if(DALSYS_Malloc(sizeof(CPU_MON_QDSP_DATA),  
                   (void**) &cpuMonContext)
                   != DAL_SUCCESS) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);   
  }
  /* Initialize the memory allocated for the CPU monitor data */
  memset(cpuMonContext, 0, sizeof(CPU_MON_QDSP_DATA));

  context->cpuMonContext = cpuMonContext;


}

void CpuMonitorInitialize(DCVS_CONTEXT* context)
{
  /* The size of the cpu monitor log in bytes. */ 
  uint32 cpuMonLogBufSize = 0, i = 0;
  
  /* The name of the log */
  char logName[128], coreIdString[2];
  CPU_MON_QDSP_DATA* cpuMonContext;
  ULogResult createResult;

  /* Initialize the CPU monitor NV data if not already done so. */
  if(cpuMonGeneric.nvDataInit == FALSE)
  {
    // Write the Default Monitor Parameters
    DCVSNvDefaultParamsInit ("Monitor", 
                             dcvsParamsMonitorQdsp);
    cpuMonGeneric.nvDataInit = TRUE;
  }

  blast_profile_enable(1);

  /* Allocate the memory for the CPU monitor */
  CpuMonQdspDataMemoryAllocateAndInitialize(context);

  cpuMonContext = (CPU_MON_QDSP_DATA*)context->cpuMonContext;

  // Determine the proper size for our Monitor Log
  DcvsNvParamUint32Read(context->coreId, 
                        "Monitor", 
                        &dcvsParamsMonitorQdsp[DCVS_PARAMS_MONITOR_QDSP_LOG_SIZE], 
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

  /* Create the name for the log. */  
  strlcpy(logName, "CPUMon", 128);
  strlcat(logName, coreIdString, 128);

  /* Create the log for CPU monitor */
  createResult = ULogFront_RealTimeInit(&cpuMonContext->logHandle, 
                                        logName,
                                        cpuMonLogBufSize,  // Initial Buffer
                                        ULOG_MEMORY_LOCAL,
                                        ULOG_LOCK_OS);

  if ((createResult != DAL_SUCCESS) &&
      (createResult != ULOG_ERR_INITINCOMPLETE))
  {
    cpuMonContext->enabled = FALSE;
  }

  ULogCore_HeaderSet(cpuMonContext->logHandle, "Content-Type: text/tagged-log-1.0; title=CPU_Monitor_QDSP");

  DcvsNvParamUint32Read(context->coreId, 
                        "Monitor", 
                        &dcvsParamsMonitorQdsp[DCVS_PARAMS_MONITOR_QDSP_SAMPLE_PERIOD], 
                        &cpuMonContext->samplePeriod);

  /* Create the timer for the Cpu Monitor */
	 cpuMonContext->timerHandle = (uint32)DCVSKernelTimerCreate(CpuMonQdspTimerCb, context);

}



void CpuMonitorStart(DCVS_CONTEXT* context)
{
  CPU_MON_QDSP_DATA* cpuMonContext;
  uint32 i;
  uint64 tpcycles;
  uint64 ticycles[MAX_THREADS];

  cpuMonContext = (CPU_MON_QDSP_DATA*)context->cpuMonContext;

  if(cpuMonContext->enabled == TRUE)
  {
    /* Register to be notified when the state of the physical clock changes */
    dcvs_npa_events_register(DCVS_NPA_EVENT_CLK_CHANGE, CpuMonQdspProcessor, context);

    /* Get the current count of idle processor cycles. */
    blast_profile_get_idle_pcycles(ticycles);

    /* Get the current count of core processor cycles executed. */
    tpcycles = blast_get_core_pcycles();

    /* Get the initial thread tick and idle counts. */
    cpuMonContext->threadTickCounts[cpuMonContext->threadTickIndex] = (uint32)tpcycles;

    for (i = 0; i < MAX_THREADS; i++)
    {    
      cpuMonContext->threadIdleCounts[cpuMonContext->threadTickIndex][i] = (uint32)ticycles[i];
    }
   
    /* System ticks at the end of the timing interval */
    cpuMonContext->sysTickStop = blast_system_sclk_attr_gethwticks();

    /* Start the timer for Cpu Monitor */
	DCVSKernelTimerStart((DCVSKernelTimerHandle)cpuMonContext->timerHandle, cpuMonContext->samplePeriod, context);

  }
}

#endif // DEFEATURE_CPU_MON_QDSP




