

/**********************************************************************
 * Qdsp_classic.c
 *
 * This module implements the qdsp_qdsp_classic algorithm for
 * Dynamic Clock and Voltage Scaling(DCVS).
 *
 * EXTERNALIZED FUNCTIONS
 *
 * Copyright (C) 2009 - 2012 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.tx/6.0/power/dcvs/src/common/algorithms/qdsp_classic.c#1 $
$DateTime: 2020/01/30 22:49:35 $

when       who     what, where, why
--------   ----    --------------------------------------------------- 
11/28/12   nj      Fixed a ULog printing bug
10/01/12   nj      Fixed ULog printing format for 64 bits
09/06/12   nj      Detecting PMU available and restarting sampling window 
09/26/12   nj      Fixed a bug in calculating OneThreadBusy percentage 
09/15/12   nj      Added code to deal with concurrent PMU usage
09/10/12   nj      INTLOCK/INTFREE sections when reading PMU 
09/10/12   nj      Corrected ULOG printing for 64 bit 
08/30/12   nj      Added New Scheme based Profiling on top of existing scheme 
                   Added ClkChgCB
08/29/12   nj      Fixed bug in 'old code' where timer is restarted from ClkChgCB 
                   w/o the updated timer interval value (always SAMPLE PERIOD was used) 
08/23/12   nj      Added PMU Profiling code 
03/29/12   nj      Added header macros that include target specific include files 
03/23/12   nj      Changed include files to be compatible with 8974
09/24/11   sg      Declare extern requestStateVector.
09/19/11   ss      Removing the notifications into the algorithm when the state of the 
                   clock changes. 
09/15/11   ss      Restarting the timers so that DCVS fires every sample period interval
                   taking into account the clock changes.
09/12/11   ss      Correcting a bug in identifying dedicated and non dedicated threads.
                   Adding additional logging for vector requests.
04/26/11   ss      Fixing Q6 compiler warnings.
04/14/11   ss      Fixing the system profiling functionality.
02/28/11   bjs     Initial check in.
========================================================================*/
#include "BuildFeatures.h"
#ifndef DEFEATURE_DCVS_ALGO_QDSP_CLASSIC

#include "dcvs_core.h"
#include "dcvs_kernel.h"
#include "dcvs_utils.h"
#include "dcvs_clkstub.h"
#include "dcvs_algorithm.h"
#include "classic.h"
#include "dcvs_npa.h"
#include "dcvs_nv.h"
#include "ULogFront.h"
#include "npa.h"
#include "CoreVerify.h"
#include "npa_resource.h"
#include "max_stub.h"
#include "blast.h"
#include "msg.h"
#include "CoreTime.h"

#include DCVS_OS_HEADER1_H
#include DCVS_OS_HEADER2_H


typedef enum
{
  DCVS_PARAMS_ALG_QDSP_CLASSIC_SAMPLE_PERIOD = 0,
  DCVS_PARAMS_ALG_QDSP_CLASSIC_HIGH_THRESHOLD_BUSY_PCT,
} DCVS_PARAMS_ALG_QDSP_CLASSIC;

static const DCVS_NV_PARAM_ENTRY dcvsParamsAlgDefault[] =
{
  {"SamplePeriod", DCVS_NV_PARAM_UINT32, {(void*)50}},
  {"HighThresholdBusyPct", DCVS_NV_PARAM_UINT32, {(void *)90}},
  DCVS_NV_PARAM_LIST_END
};

#define DCVS_ALGO_QDSP_CLASSIC_HIGH_WATERMARK_SAFETY (10)

/* Macro for multiplication by 10 .
   Multiply the number by 8 and 2 and add the sum . */
#define MUL_TEN(x) \
  ((x<<3)+(x<<1))

/* Macro for PMU Event config register value */
#define PMU_EVT_CONFIG_VAL 0x003d3c3b

/* Macro for PMU config register value */
#define PMU_CONFIG_VAL 7


/* Structure to maintain the low and high thresholds for each clock level in OAL. */
typedef struct
{
  /* The low threshold for a clock level in terms of busy percentage */
  uint32 lowThresholdBusyPct;

}DCVS_ALGO_QDSP_CLASSIC_CLK_THRESHOLD;

extern uint32 *requestStateVector;

/*Static variable to store the current status of PMU usage 
  TRUE==not available for DCVS use; FALSE==available for DCVS use.*/
static uint32 isPMUBusy = FALSE; 

/* Static variable to store the context handle */
static DCVS_CONTEXT* localCoreContext = NULL;

/* Default DCVS Algorithm specific members */
typedef struct
{
  /* Members specific to the default DCVS algorithm */

  ULogHandle logHandle;

  /* mips calculated by the OAL portion of the algorithm. */
  uint32 mips;

  /* Structure which stores the clock plan for the particular target */
  DCVS_TARGET_CLOCK_PLAN_INFO_TYPE* clockPlan;

  /* Query handle to NPA resource */
  npa_query_handle queryHandle;

  /* The resource handle of the NPA resource used by this algorithm.  */
  npa_resource *resource;

  /* Previous known value of the cpu clock (kHz) */
  uint32 prevClkFrequency;

  /* Current known value of the cpu clock (kHz) */
  uint32 currClkFrequency;

  uint32 samplePeriod;
  uint32 activeFloor;
  uint32 timerHandle;

  uint32 timerBusy;

  /* The high threshold busy percentage from the NV .*/
  uint32 highThresholdBusyPct;

  /* Structure to maintain the low and high thresholds for each clock level in OAL. */
  DCVS_ALGO_QDSP_CLASSIC_CLK_THRESHOLD* thresholdTable;

  /* The current lookup index for the clock plan table as calculated by this DCVS algorithm */
  uint32 clockPlanCurrIndex;

  /* Number of ticks elapsed on BLAST system clock */
  uint64 sysTickStart;
  uint64 sysTickStop;

  /* Delta system clock ticks that have elapsed during each profiling interval */
  uint32 sclksDeltaVal;

  /* Total count of pcycles that have elapsed during each profiling interval  */
  uint64 sumPCyclesCount;

  /* Stores the total time elapsed in milli-second units*/
  uint32 totalTimeElapsed;

  /* Kernel PMU reported busy cycles for each HW-Thread */
  uint32 prevOneThreadCycleCnt;
  uint32 prevTwoThreadCycleCnt;
  uint32 prevThreeThreadCycleCnt;

}DCVS_ALGO_QDSP_CLASSIC;

/* Function to check if PMU is enabled
   Place holder for now */
static inline uint32 checkPMUEnableBit(void)
{
    return TRUE;
}

/**
  @brief DCVSAlgoQdspClassicTimerStart

  This function is used to start the DCVS sampling timer.

  @param time_ms : The time in ms for the timer.

  @param context: DCVS data context.

  @return : None

*/
static void DCVSAlgoQdspClassicTimerStart(uint32 time_ms, DCVS_CONTEXT* context)
{
  /* Obtain the algorithm data context. */
  DCVS_ALGO_QDSP_CLASSIC* algoDataContext = context->algoDataContext;

  algoDataContext->timerBusy = 1;

  /* System ticks at the start of the timing interval */
  algoDataContext->sysTickStart = blast_system_sclk_attr_gethwticks();

  /* Start the DCVS sampling timer. */
  DCVSKernelTimerStart((DCVSKernelTimerHandle)algoDataContext->timerHandle, time_ms, context);
}

/**
  @brief DCVSAlgoQdspClassicTimerStop

  This function is used to stop the DCVS sampling timer.

  @param : DCVS data context.

  @return : None

*/
static void DCVSAlgoQdspClassicTimerStop(DCVS_CONTEXT* context)
{
  /* Obtain the algorithm data context. */
  DCVS_ALGO_QDSP_CLASSIC* algoDataContext = context->algoDataContext;
  algoDataContext->timerBusy = 0;

  /* System ticks at the end of the timing interval */
  algoDataContext->sysTickStop = blast_system_sclk_attr_gethwticks();

  DCVSKernelTimerStop((DCVSKernelTimerHandle)algoDataContext->timerHandle, context);
}



static void DCVSAlgoQdspPMUInitialize(void)
{
  DCVS_ALGO_QDSP_CLASSIC* algoDataContext = localCoreContext->algoDataContext;

  blast_pmu_enable(0); //disabled

  blast_pmu_set(BLAST_PMUEVTCFG, PMU_EVT_CONFIG_VAL); // Set Event Types
  blast_pmu_set(BLAST_PMUCFG, PMU_CONFIG_VAL);        // Thread Bit Mask

  blast_pmu_set(BLAST_PMUCNT0, 0);
  blast_pmu_set(BLAST_PMUCNT1, 0);
  blast_pmu_set(BLAST_PMUCNT2, 0);
  blast_pmu_set(BLAST_PMUCNT3, 0);

  algoDataContext->prevOneThreadCycleCnt = 0;
  algoDataContext->prevTwoThreadCycleCnt = 0;
  algoDataContext->prevThreeThreadCycleCnt = 0;

  blast_pmu_enable(1); //enabled

}
  
/**
  @brief DcvsQdspPMUbusy

  This function gets called whenever somebody is 
  using the PMU. A TRUE argument indicates that PMU is indeed 
  being used and DCVS should not use it. Correspondingly, a 
  FALSE argument means that no one other than DCVS is using the 
  PMU. 

  @param isBusy
  
  @return : none

*/
void DcvsQdspPMUbusy(int isBusy)
{

  if (localCoreContext == NULL)
  {
    return;
  }

  /* Obtain the algorithm data context. */
  DCVS_ALGO_QDSP_CLASSIC* algoDataContext = 
    (DCVS_ALGO_QDSP_CLASSIC*)localCoreContext->algoDataContext;

  DCVSKernelCriticalSectionEnter(localCoreContext);

  if (isBusy)
  {
    if (blast_pmu_get(BLAST_PMUEVTCFG) != PMU_EVT_CONFIG_VAL)
    {
      isPMUBusy = TRUE;
    }
    else
    {
      isPMUBusy = FALSE;
    }
  }
  else
  {
      isPMUBusy = FALSE;
      DCVSAlgoQdspPMUInitialize();

      /* We need to restart DCVS Sampling Window, since for the time duration that the PMU was 
         busy we dont have valid PMU numbers and hence Algorithm cannot make the right decision
         hence we start sampling from now */
      DCVSAlgoQdspClassicTimerStart(algoDataContext->samplePeriod, localCoreContext);

  }

  DCVSKernelCriticalSectionExit(localCoreContext);
}


/**
  @brief DCVSAlgoClassicCurrClockStateQuery
  
  This function is used to read the current state of the clock.
 
  @param context: The DCVS data context.
 
  @return : The current mips of the clock or invalid state . 

*/
static uint32 DCVSAlgoQdspClassicCurrClockStateQuery(DCVS_CONTEXT *context)
{
  npa_query_status status;
  npa_query_type queryResult;

  /* Obtain the algorithm data context. */
  DCVS_ALGO_QDSP_CLASSIC *algoDataContext = (DCVS_ALGO_QDSP_CLASSIC *)context->algoDataContext;

  /* Query the current state of clock resource. */
  status = npa_query(algoDataContext->queryHandle, NPA_QUERY_CURRENT_STATE, &queryResult);
  if (status == NPA_QUERY_SUCCESS)
  {
    return (queryResult.data.state);
  } else
  {
    return (DCVS_ALGO_CLASSIC_INVALID_CLK_STATE);
  }
}

/**
  @brief DCVSQdspHasPMUConfigChanged

  This function checks if the PMU is either disabled or if it's 
  Register Configuration bits have changed or if somebody other 
  than DCVS is using the PMU Registers 

  @param none
   
  @return : TRUE if PMU config has changed 
            FALSE otherwise 

*/

static inline uint32 DCVSQdspHasPMUConfigChanged(void)
{
  uint32 evtCFG = blast_pmu_get(BLAST_PMUEVTCFG);

  if(evtCFG != PMU_EVT_CONFIG_VAL || !checkPMUEnableBit() || isPMUBusy)
  {
    // F3 logging
    // MSG_HIGH("DcvsQdspPMUIsBusy(BLAST_PMUEVTCFG=0X%x CurrTime=0X%x)", evtCFG, CoreTimetick_Get(), 0);
    return TRUE;
  }
  return FALSE;
}



/**
  @brief DCVSRunQdspClassic

  This function is used to run the actual qdsp_classic 
  algorithm. 
   
  This function is run every time the DCVSTimer expires 
  (ie:every SamplePeriod). We calculate the Budget PCycles that
  were available in the last sample period. then get the
  profiling data by reading the PMU Registers that tell us how
  for how many cycles did atleast one HW Thread run for. Based 
  on this, we calculate the busy percentage of the time where
  atleast one HW thread was running. Then compare this % with 
  high and low thresholds – if greater than high threshold, step 
  up the clock index (Performance Level) by 1, if lower than low 
  threshold, calculate the new clock index (Performance Level) 
  that atleast meets the clock frequency based on the busy 
  percentage. Restart the DCVSTimer with the SamplePeriod time 
  Return the mips based on the above caclulated clock plan index 


  @param activeFloor : Active floor of the system.
  @param context: The DCVS data context.

  @return : The mips calculated by the algorithm.

*/

static uint32 DCVSRunQdspClassic(uint32 activeFloor, DCVS_CONTEXT *context)
{
  // This should run every time the timer is fired.  It is run from an
  // asynchronous client, so already has the lock.
  // Never allow this to drop below activeFloor
  //
  uint32 i;

  /* Counts for Thread Busy PCycles */
  uint32 oneThreadCycles = 0;
  uint32 twoThreadCycles = 0;
  uint32 threeThreadCycles = 0;

  /* Counts for Thread Busy PCycles Deltas */
  uint32 oneThreadCyclesDel = 0;
  uint32 twoThreadCyclesDel = 0;
  uint32 threeThreadCyclesDel = 0;

  /* Obtain the algorithm data context. */
  DCVS_ALGO_QDSP_CLASSIC *algoDataContext = context->algoDataContext;

  /* Variable to hold the clock plan index before the algorithm is executed to check if the index
  changed . */
  uint32 clockPlanOldIndex = algoDataContext->clockPlanCurrIndex;

  /* time elapsed since timer fired */
  uint32 timerElapsedTime = 0;

  /* Store the system ref clock value  */
  uint32 clkTicksPerSec = context->coreDataContext->clockPlan->clkTicksPerSec;

  if (DCVSQdspHasPMUConfigChanged())
  {
    /*
    ULogFront_RealTimePrintf(algoDataContext->logHandle,
                             1,
                             "PMU Change (Config:  0X%x)",   
                             blast_pmu_get(BLAST_PMUEVTCFG));
    */

    DCVSKernelCriticalSectionEnter(context);    

    //Set Index to Max
    algoDataContext->clockPlanCurrIndex = algoDataContext->clockPlan->numLevels - 1;
    algoDataContext->sumPCyclesCount = 0;    //reset to 0
    algoDataContext->totalTimeElapsed = 0;

    /* Log the internals of the algorithm for visibility */
    ULogFront_RealTimePrintf(algoDataContext->logHandle,
                             2,
                             "QdspCLASSIC (AlgoMIPS: %d)(AlgoClockPlanIndex: %d)",
                             algoDataContext->clockPlan->clockPlanTable
                             [algoDataContext->clockPlanCurrIndex].reqMips,
                             algoDataContext->clockPlanCurrIndex
                             );
	DCVSKernelCriticalSectionExit(context);

    /* Restart the sampling timer */
    DCVSAlgoQdspClassicTimerStart(algoDataContext->samplePeriod, context);

      

    // Return current mips.
    return (algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].reqMips);
  }

  DCVSKernelCriticalSectionEnter(context);

  // F3 logging for Current Time
  // MSG_HIGH("DcvsQdspRunQDSP(CurrTime=0X%x)", CoreTimetick_Get(), 0, 0);

  // Aggregate the total time elapsed since the timer started. Time spent in
  // Power Collapse is not counted since we're using a deferable timer. Time elapsed is in msecs
  algoDataContext->sysTickStop = blast_system_sclk_attr_gethwticks();

  // We should not overflow since expecting the timer to get triggered in 10s of millisecs or
  // 100s in the worst case.
  algoDataContext->sclksDeltaVal = (uint32)(algoDataContext->sysTickStop 
                                            - algoDataContext->sysTickStart);
  
  timerElapsedTime = (algoDataContext->sclksDeltaVal * 1000) / clkTicksPerSec;

  algoDataContext->totalTimeElapsed += timerElapsedTime;
  
  algoDataContext->sumPCyclesCount += (uint64)((uint64)algoDataContext->sclksDeltaVal 
                           * (uint64)algoDataContext->currClkFrequency) / clkTicksPerSec; //kPCycles
 
  

  /* 
  Time to get some Blast Profiling data out... 
  We are reading 3 registers and this operation needs to be atomic, hence the lock 
  */
  INTLOCK();

  oneThreadCycles = blast_pmu_get(BLAST_PMUCNT0);
  twoThreadCycles = blast_pmu_get(BLAST_PMUCNT1);
  threeThreadCycles = blast_pmu_get(BLAST_PMUCNT2);

  INTFREE();
  
  oneThreadCyclesDel = oneThreadCycles - algoDataContext->prevOneThreadCycleCnt;
  twoThreadCyclesDel = twoThreadCycles - algoDataContext->prevTwoThreadCycleCnt;
  threeThreadCyclesDel = threeThreadCycles - algoDataContext->prevThreeThreadCycleCnt;

  algoDataContext->prevOneThreadCycleCnt = oneThreadCycles;
  algoDataContext->prevTwoThreadCycleCnt = twoThreadCycles;
  algoDataContext->prevThreeThreadCycleCnt = threeThreadCycles;


  /* We're interested to find the number of cycles when atleast 1 HW Thread was running, this
     should give us the minimum CPU clock requirement during the profiling interval */
  uint32 sumActiveThreadCycles = oneThreadCyclesDel + twoThreadCyclesDel + threeThreadCyclesDel;
  sumActiveThreadCycles /= (MAX_THREADS * 1000);              //kPCycles

  ULogFront_RealTimePrintf(algoDataContext->logHandle,
                           6,
                           "RAW PMU DATA(1-Th: %d) (2-Th: %d) (3-Th: %d) (Budget-KPcycles: %d%d) "
                           "(Curr-Freq: %d)", oneThreadCyclesDel, twoThreadCyclesDel, 
                           threeThreadCyclesDel,DCVS_ULOG64_DATA(algoDataContext->sumPCyclesCount),
                           algoDataContext->currClkFrequency);

  uint32 minOneThreadBusyPerct = (sumActiveThreadCycles * 100) / algoDataContext->sumPCyclesCount;

  if (minOneThreadBusyPerct >= (algoDataContext->highThresholdBusyPct * 100/ ( MAX_THREADS * 100)))
  {
    if (algoDataContext->clockPlanCurrIndex < algoDataContext->clockPlan->numLevels - 1)
    {
      algoDataContext->clockPlanCurrIndex++;
    }
  } 
  else if (minOneThreadBusyPerct <
               (algoDataContext->thresholdTable
                [algoDataContext->clockPlanCurrIndex].lowThresholdBusyPct / MAX_THREADS))
  {
    for (i = 0; i < algoDataContext->clockPlan->numLevels; i++)
    {
      if (algoDataContext->clockPlan->clockPlanTable[i].cpuFreq >=
            ((minOneThreadBusyPerct * MAX_THREADS * algoDataContext->currClkFrequency) / 100))
      {
        break;
      }
    }
    if (i >= algoDataContext->clockPlan->numLevels)
    {
      i = algoDataContext->clockPlan->numLevels - 1;
    }
    algoDataContext->clockPlanCurrIndex = i;
  }

  /* Check to see if the clock rate has changed or the active floor has changed . */
  if ((activeFloor > 0) || (clockPlanOldIndex != algoDataContext->clockPlanCurrIndex))
  {
    /* We make sure that the algorithm mips is greater than the active floor. Start from the current index
       because we are sure to be above the active floor till now. */
    for (i = algoDataContext->clockPlanCurrIndex; i < algoDataContext->clockPlan->numLevels; i++)
    {
      if (algoDataContext->clockPlan->clockPlanTable[i].reqMips >= activeFloor)
      {
        break;
      }
    }
    if (i >= algoDataContext->clockPlan->numLevels)
    {
      i = algoDataContext->clockPlan->numLevels - 1;
    }
    algoDataContext->clockPlanCurrIndex = i;
  }





  /* Log the internals of the algorithm for visibility */
 /*
  ULogFront_RealTimePrintf(algoDataContext->logHandle,
                           10,
                           "PMU_DATA(SumKPCycles Atleast One Thread Active: %d) "
                           "(Busy Perct atleast One Thread: %d) (Curr Freq: %d) (KPCycles: %d%d) "
                           "(Start_SCLKS: 0X%x%x) (Stop_SCLKS: 0X%x%x) (Timer Elapsed Time: %d) ",
                           sumActiveThreadCycles,
                           minOneThreadBusyPerct,
                           algoDataContext->currClkFrequency,
                           DCVS_ULOG64_DATA(algoDataContext->sumPCyclesCount),
                           DCVS_ULOG64_DATA(algoDataContext->sysTickStart),
                           DCVS_ULOG64_DATA(algoDataContext->sysTickStop),
                           algoDataContext->totalTimeElapsed
                           );
 */


  algoDataContext->sumPCyclesCount = 0;    //reset to 0

  /* Log the internals of the algorithm for visibility */
  ULogFront_RealTimePrintf(algoDataContext->logHandle,
                           2,
                           "QdspCLASSIC (AlgoMIPS: %d)(AlgoClockPlanIndex: %d)",
                           algoDataContext->clockPlan->clockPlanTable
                           [algoDataContext->clockPlanCurrIndex].reqMips,
                           algoDataContext->clockPlanCurrIndex
                           );

  algoDataContext->totalTimeElapsed = 0;

  DCVSKernelCriticalSectionExit(context);
  /* Restart the sampling timer */
  DCVSAlgoQdspClassicTimerStart(algoDataContext->samplePeriod, context);
  // Return current mips.
  return (algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].reqMips);

}

/**
  @brief DCVSAlgoQdspClassicTimerCB

  This function is the callback from the DCVS sampling timer expiry.

  @param : None

  @return : None

*/
static void DCVSAlgoQdspClassicTimerCB(DCVS_CONTEXT* context)
{
  DCVS_ALGO_QDSP_CLASSIC* algoDataContext = context->algoDataContext;
  algoDataContext->timerBusy = 0;
  /* The DCVS sampling timer has fired calculate the new mips of the Classic algorithm */
  DCVSTargetTimerFire(context);
}

/**
  @brief DCVSImpulseQdspClassic

  This function is used to return the max mips of the target in response to the impulse client 
  request.

  @param  : DCVS data context.

  @return : The max mips supported by the target.

*/
static uint32 DCVSImpulseQdspClassic(DCVS_CONTEXT* context)
{
  /* Obtain the algorithm data context. */
  DCVS_ALGO_QDSP_CLASSIC* algoDataContext = (DCVS_ALGO_QDSP_CLASSIC*)context->algoDataContext;

  // This will be run from an impulse client request, so it already
  // has the lock.
  // restart the timer.
  DCVSAlgoQdspClassicTimerStop(context);

  algoDataContext->clockPlanCurrIndex = algoDataContext->clockPlan->numLevels - 1;

  DCVSAlgoQdspClassicTimerStart(algoDataContext->samplePeriod, context);
  // Return current mips.
  return (algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].reqMips);
}

/**
  @brief DCVSFloorQdspClassic

  This function is used to reset the members of the qdsp_classic algorithm.

  @param  : DCVS data context.

  @return : The max mips supported by the target.

*/
static uint32 DCVSFloorQdspClassic (DCVS_CONTEXT* context, uint32 floor)
{
/* Obtain the algorithm data context. */
  DCVS_ALGO_QDSP_CLASSIC* algoDataContext = context->algoDataContext;

  uint32 result = algoDataContext->mips;
  // This will be run from a required client request, so it already
  // has the lock.
  //
  // Keep the floor value around
  algoDataContext->activeFloor = floor;

  if (algoDataContext->mips < floor)
  {
        
    result = floor;

  }

  return (result);
}

/**
  @brief DCVSAlgoQdspClassicReset

  This function is used to reset the members of the qdsp_classic algorithm.

  @param  : DCVS data context.

  @return : The max mips supported by the target.

*/
static uint32 DCVSAlgoQdspClassicReset(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* Obtain the algorithm data context. */
  DCVS_ALGO_QDSP_CLASSIC* algoDataContext = (DCVS_ALGO_QDSP_CLASSIC*)coreContext->algoDataContext;

  /* Return max mips. */
  algoDataContext->clockPlanCurrIndex = algoDataContext->clockPlan->numLevels - 1;
  return (algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].reqMips);
}


/**
  @brief DCVSAlgoQdspClassicClkChangeCB

  This is the callback for clock change. When clock changes to a 
  valid state, we need to update our internal clock reference 
  index. We then check if the DCVS Timer is about to expire 
  soon, if so we fire the timer immediately. If not, we profile 
  the system, based on the prev. interval before the clock 
  change occured and then restart the DCVS timer with the 
  remaining timer interval. 

  @param event_id: Event which caused callback to be fired.
  @param pserver_data: optional pointer to data supplied by NPA.
  @param pclient_data: optional pointer to data passed by client at time
  of registration.

*/
void DCVSAlgoQdspClassicClkChangeCB(dcvs_npa_event event_id,
                                    void *pserver_data,
                                    void *pclient_data)
{

  DCVS_CONTEXT *context = (DCVS_CONTEXT *)pclient_data;

    /* Obtain the algorithm data context. */
  DCVS_ALGO_QDSP_CLASSIC *algoDataContext = (DCVS_ALGO_QDSP_CLASSIC *)context->algoDataContext;

  DCVSKernelCriticalSectionEnter(context);    

  // First check if PMU config has changed, if so max out the clock
  if (DCVSQdspHasPMUConfigChanged())
  {
//  ULogFront_RealTimePrintf(algoDataContext->logHandle,
//                           1,
//                           "PMU Change ClkCB (Config: 0X%x)",
//                           blast_pmu_get(BLAST_PMUEVTCFG));

    //Set Index to Max
    algoDataContext->clockPlanCurrIndex = algoDataContext->clockPlan->numLevels - 1;

    /* Log the internals of the algorithm for visibility */
//  ULogFront_RealTimePrintf(algoDataContext->logHandle,
//                           2,
//                           "MAXED OUT IN CLK CB (AlgoMIPS: %d)(AlgoClockPlanIndex: %d)",
//                           algoDataContext->clockPlan->clockPlanTable
//                           [algoDataContext->clockPlanCurrIndex].reqMips,
//                           algoDataContext->clockPlanCurrIndex
//                           );
    DCVSKernelCriticalSectionExit(context);

    return;
  }


  int i;

  /* Stores the total (aggregate) time since most recent timer expiry */
  uint32 timerElapsedTime = 0;

  /* Store the system ref clock value  */
  uint32 clkTicksPerSec = context->coreDataContext->clockPlan->clkTicksPerSec;

  /* This is called from the event callback context.  A lock must be retrieved
  before continuing. */

  //save current clock freq
  algoDataContext->prevClkFrequency = algoDataContext->currClkFrequency;

  /* Obtain the current state of the clock */
  algoDataContext->currClkFrequency = DCVSAlgoQdspClassicCurrClockStateQuery(context);


  /*  To prevent re-entrant execution of the clock change callback function,
      we should enter only if the timer is currently busy & the clock has
      changed to a valid state
  */

  if (algoDataContext->timerBusy == TRUE && algoDataContext->currClkFrequency !=
        DCVS_ALGO_CLASSIC_INVALID_CLK_STATE)
  {
    // This is called from the event callback context.  A lock must be retrieved
    // before continuing.
    //

    /* The clock change events on the kernel side maybe queued up and not
    processed instantly if the system is busy. They may be queued up. And the
    system might be set at a particular mips 'X'.( after changing to mips values
    indicated by the events). When the system is not busy the events are finally
    fired off  one by one, the system would still be at the same set mips 'X'.
    The query would always return mips 'X' although there are multiple event
    callbacks owing to the events fired off by the system.Check to see if the
    previous state of the clock is not the same as the latest query state */

    if (algoDataContext->currClkFrequency != algoDataContext->prevClkFrequency)
    {

      /* Stop the DCVS timer */
      DCVSAlgoQdspClassicTimerStop(context);

      /* Calculate the time elapsed since timer was started */

      algoDataContext->sysTickStop = blast_system_sclk_attr_gethwticks();
      algoDataContext->sclksDeltaVal = algoDataContext->sysTickStop - algoDataContext->sysTickStart;

      timerElapsedTime = ((algoDataContext->sclksDeltaVal * 1000) / clkTicksPerSec); //time in ms units

      uint32 local_totalTimeElapsed = algoDataContext->totalTimeElapsed + timerElapsedTime;

      /* Update the clock index to the clock freq. index */

      for (i = 0; i < algoDataContext->clockPlan->numLevels; i++)
      {
        if (algoDataContext->clockPlan->clockPlanTable[i].cpuFreq >= algoDataContext->currClkFrequency)
        {
          break;
        }
      }

      if (i >= algoDataContext->clockPlan->numLevels)
      {
        i = algoDataContext->clockPlan->numLevels - 1;
      }
      algoDataContext->clockPlanCurrIndex = i;


      /* If the future timer expiry is less than or equal to 6.25% of the
      algorithm timer period then the timer expiry is indicated immediately
      since its very close to the actual timer expiry anyway. */

      if ((algoDataContext->samplePeriod < local_totalTimeElapsed) ||
          ((algoDataContext->samplePeriod - local_totalTimeElapsed) <=
             algoDataContext->samplePeriod >> 4))
      {

        /* Log the internals of the algorithm for visibility */
        ULogFront_RealTimePrintf(algoDataContext->logHandle,
                                 7,
                                 "QDSP_CLASSIC_CLK_CHANGE_CB_SKIP (currentClkFreq: %d)"
                                 "PrevOpFreq: %d)(Timer Elapsed Time: %d)(Start_SCLKS: 0X%x%x)"
                                 "(Stop_SCLKS: 0X%x%x)",
                                 algoDataContext->currClkFrequency,
                                 algoDataContext->prevClkFrequency,
                                 timerElapsedTime,
                                 DCVS_ULOG64_DATA(algoDataContext->sysTickStart),
                                 DCVS_ULOG64_DATA(algoDataContext->sysTickStop)
                                 );

        DCVSKernelCriticalSectionExit(context);

        DCVSRunQdspClassic(context->resDataContext->activeFloor, context);

      }
       
      else
      {

        // Update the Total Time Elapsed since timer start
        algoDataContext->totalTimeElapsed = local_totalTimeElapsed;

        // Calculate and aggregate the PCycles count that have elapsed

        algoDataContext->sumPCyclesCount += ((uint64)((uint64)algoDataContext->sclksDeltaVal * 
                                      (uint64)algoDataContext->prevClkFrequency)) / clkTicksPerSec;

        DCVSKernelCriticalSectionExit(context);


        // Log the internals of the algorithm for visibility
//      ULogFront_RealTimePrintf(algoDataContext->logHandle,
//                               7,
//                               "PMU_CALC_CLK_CHG (KPCyclesCount: %d%d)(SCLK_Start: 0X%x%x)"
//                               " (SCLK_Stop: 0X%x%x) (SCLKSDelta: %d) ",
//                               DCVS_ULOG64_DATA(algoDataContext->sumPCyclesCount),
//                               DCVS_ULOG64_DATA(algoDataContext->sysTickStart),
//                               DCVS_ULOG64_DATA(algoDataContext->sysTickStop),
//                               algoDataContext->sclksDeltaVal);




        /* Log the internals of the algorithm for visibility */
        ULogFront_RealTimePrintf(algoDataContext->logHandle,
                                 4,
                                 "QDSP_CLASSIC_CLK_CHANGE (PrevOpFreq: %d)(CurrOpFreq: %d)"
                                 "(NewClockPlanIndex: %d)(Timer Elapsed Time: %d) ",
                                 algoDataContext->prevClkFrequency,
                                 algoDataContext->currClkFrequency,
                                 algoDataContext->clockPlanCurrIndex,
                                 algoDataContext->totalTimeElapsed
                                 );

        /* Restart the DCVS timer to expire at its designated timer expiry */
        DCVSAlgoQdspClassicTimerStart((algoDataContext->samplePeriod - algoDataContext->totalTimeElapsed), context);

      }
    }
    else
    {
      DCVSKernelCriticalSectionExit(context);
    }
  }
  else
  {
    DCVSKernelCriticalSectionExit(context);
  }
  
}

/**
  @brief DCVSAlgoQdspClassicThresholdTableSetup

  This function is used to calculate the low and high threshold
  values in terms of busy percentage for each clock level.

  @param  context: The DCVS data context.

  @return : None

*/
static void DCVSAlgoQdspClassicThresholdTableSetup(DCVS_CONTEXT* context)
{
  uint32 currClkMips, prevClkMips, highWaterMarkIdle, lowWaterMarkBusy;
  int32 i;
  DCVS_ALGO_QDSP_CLASSIC* algoDataContext = context->algoDataContext;

  /* Allocate the memory for threshold table information for the resources */
  if (DALSYS_Malloc(sizeof(DCVS_ALGO_QDSP_CLASSIC_CLK_THRESHOLD) * (algoDataContext->clockPlan->numLevels),
                    (void **)&algoDataContext->thresholdTable)
      != DAL_SUCCESS)
  {
    /* Error fatal */
    CORE_VERIFY(NULL);
  }

  /* Precalculate the low/high thresholds for the busy percentage for different clock levels
     for the algorithm */
  for (i = algoDataContext->clockPlan->numLevels - 1; i >= 0; i--)
  {
    /* Calculate the HIGH threshold for the clock level
       High threshold busy pct is fixed according to the NV input. */

    /* Calculate the LOW threshold for the clock level
       High threshold is the current clock level is input from the NV . */

    if (i == 0)
    {
      algoDataContext->thresholdTable[i].lowThresholdBusyPct = 0;
    } 
	else
    {
      currClkMips = algoDataContext->clockPlan->clockPlanTable[i].reqMips;
      prevClkMips = algoDataContext->clockPlan->clockPlanTable[i - 1].reqMips;
      /* Calculate the high watermark percent for idle for this clock level */
      highWaterMarkIdle = (((currClkMips - prevClkMips) * 100) / currClkMips) + DCVS_ALGO_QDSP_CLASSIC_HIGH_WATERMARK_SAFETY;
      /* The low watermark percentage for busy for this clock level */
      lowWaterMarkBusy = 100 - highWaterMarkIdle;
      algoDataContext->thresholdTable[i].lowThresholdBusyPct = lowWaterMarkBusy;
    }
  }
}


/**
  @brief DCVSAlgoQdspClassicInitialize

  This function is used to initialize the qdsp_classic DCVS algorithm specific members .

  @param  context: DCVS data context.

  @return : Initial mips of the system

*/
static uint32 DCVSAlgoQdspClassicInitialize(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* Obtain the algorithm data context. */
  DCVS_ALGO_QDSP_CLASSIC* algoDataContext;

  /* Allocate the memory for the algorithm data context. */
  if (DALSYS_Malloc(sizeof(DCVS_ALGO_QDSP_CLASSIC),
                    (void **)&coreContext->algoDataContext)
      != DAL_SUCCESS)
  {
    /* Error fatal */
    CORE_VERIFY(NULL);
  }

  algoDataContext = (DCVS_ALGO_QDSP_CLASSIC*)coreContext->algoDataContext;

  algoDataContext->logHandle = DCVSCoreLogHandleGet(coreContext);

  CORE_VERIFY(algoDataContext->logHandle);

  algoDataContext->timerHandle = (uint32)DCVSKernelTimerCreate(DCVSAlgoQdspClassicTimerCB, coreContext);

  /* Initalize local variable to point to core context handle */
  localCoreContext = coreContext;

  if (!localCoreContext) 
  {
    CORE_VERIFY(NULL);
  }

  /* Get the current clock plan of the target */
  algoDataContext->clockPlan = DCVSCoreClockPlanGet(coreContext);

  // Start at the max
  algoDataContext->clockPlanCurrIndex = algoDataContext->clockPlan->numLevels - 1;

  /* Register to be notified when the state of the physical clock changes */
  dcvs_npa_events_register(DCVS_NPA_EVENT_CLK_CHANGE, DCVSAlgoQdspClassicClkChangeCB, coreContext);

  /* Set up the threshold table for each clock level. */
  DCVSAlgoQdspClassicThresholdTableSetup(coreContext);

  /* Configure PMU registers, enable Profiling */
  DCVSAlgoQdspPMUInitialize();

  /* Set the inital mips to max */
  algoDataContext->mips = DCVSAlgoQdspClassicReset(coreContext);

  algoDataContext->prevClkFrequency = algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].cpuFreq;
  algoDataContext->currClkFrequency = algoDataContext->prevClkFrequency;

  return (algoDataContext->mips);
}


/**
  @brief DCVSAlgoQdspClassicBuildParams

  This function is used to get the parameters for the qdsp_classic DCVS algorithm.

  @param  : DCVS data context.

  @return : None

*/
static void DCVSAlgoQdspClassicBuildParams(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* Obtain the algorithm data context. */
  DCVS_ALGO_QDSP_CLASSIC* algoDataContext = coreContext->algoDataContext;

  /* Read the timer period */
  DcvsNvParamUint32Read(0,
                        "Default",
                        &dcvsParamsAlgDefault[DCVS_PARAMS_ALG_QDSP_CLASSIC_SAMPLE_PERIOD],
                        &algoDataContext->samplePeriod);
  DcvsNvParamUint32Read(0,
                        "Default",
                        &dcvsParamsAlgDefault[DCVS_PARAMS_ALG_QDSP_CLASSIC_HIGH_THRESHOLD_BUSY_PCT],
                        &algoDataContext->highThresholdBusyPct);

  /* Create a query handle to a resource clock resource */
  CORE_VERIFY_PTR(algoDataContext->queryHandle = npa_create_query_handle(coreContext->clockDependency));

  CORE_VERIFY_PTR(algoDataContext->resource = npa_query_get_resource(algoDataContext->queryHandle));

} /* DCVSAlgoQdspClassicBuildParams */


/**
  @brief DCVSAlgoQdspClassicEnable

  This function is used to set the state of the qdsp_classic algorithm specific members
  when enabling the algorithm.

  @param context: DCVS data context.

  @return : The mips of the system after enabling the algorithm.

*/
static uint32 DCVSAlgoQdspClassicEnable(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* Obtain the algorithm data context. */
  DCVS_ALGO_QDSP_CLASSIC* algoDataContext = coreContext->algoDataContext;

  algoDataContext->mips = 
    algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlan->numLevels - 1].reqMips;

  INTLOCK();

  algoDataContext->prevOneThreadCycleCnt = blast_pmu_get(BLAST_PMUCNT0);
  algoDataContext->prevTwoThreadCycleCnt = blast_pmu_get(BLAST_PMUCNT1);
  algoDataContext->prevThreeThreadCycleCnt = blast_pmu_get(BLAST_PMUCNT2);

  INTFREE();

  DCVSAlgoQdspClassicTimerStart(algoDataContext->samplePeriod, context);

  return (algoDataContext->mips);
}


/**
  @brief DCVSAlgoQdspClassicDisable

  This function is used to reset the state of the qdsp_qdsp_classic algorithm specific members
  when disabling the algorithm.

  @param context: DCVS data context.

  @return : The mips of the system after disabling the algorithm.

*/
static uint32 DCVSAlgoQdspClassicDisable(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* Obtain the algorithm data context. */
  DCVS_ALGO_QDSP_CLASSIC* algoDataContext = coreContext->algoDataContext;

  DCVSAlgoQdspClassicTimerStop(context);

  /* Return max mips */
  return (algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlan->numLevels - 1].reqMips);
}

/**
  @brief DCVSAlgoQdspClassicDecideMips

  This function is used to incorporate external requests by the clients into the
  algorithm. The supported requests are impulse, floor etc.

  @param  context: DCVS data context.

  @return : Mips required

*/
static uint32 DCVSAlgoQdspClassicDecideMips(void* context)
{
  uint32 i;
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* Obtain the client context from the resource. */
  DCVS_RES_DATA* resDataContext = coreContext->resDataContext;

  /* Obtain the algorithm data context. */
  DCVS_ALGO_QDSP_CLASSIC* algoDataContext = (DCVS_ALGO_QDSP_CLASSIC*)coreContext->algoDataContext;

  if (resDataContext->algoMipsChangedOrTimerFired)
  {
    // Clear the timer fired flag here.
    resDataContext->algoMipsChangedOrTimerFired = FALSE;
    algoDataContext->mips = DCVSRunQdspClassic(resDataContext->activeFloor, context);
  }
  /* If there is an impulse request, send it to the OAL component. */
  else if (resDataContext->activeImpulse)
  {
    // Clear the impulse request flag here.
    resDataContext->activeImpulse = FALSE;
    algoDataContext->mips = DCVSImpulseQdspClassic(context);
  }
  // If it is not a timer, or an impulse, then the the floor is being adjusted.
  else
  {
    /* suds84. debug only . The request vector of the DCVS algorithm. */
    for (i = 0; i <= MAX_THREADS; i++)
    {
      ULogFront_RealTimePrintf(algoDataContext->logHandle,
                               2,
                               "DCVS_REQ_VEC %d :: %d",
                               i,
                               requestStateVector[i]);
    }

    algoDataContext->mips = DCVSFloorQdspClassic(context, resDataContext->activeFloor);
  }

  /* Logging for qdsp_classic algorithm */
  ULogFront_RealTimePrintf(algoDataContext->logHandle,
                           2,
                           "QDSPCLASSIC(mips: %d) (activeFloor: %d)",
                           algoDataContext->mips,
                           resDataContext->activeFloor);

  return algoDataContext->mips;
} /* DCVSAlgoQdspClassicDecideMips */




DCVS_CORE_ALGO qdspClassicAlgorithm =
{
  "qdsp_classic",
  dcvsParamsAlgDefault,
  DCVSAlgoQdspClassicInitialize,
  DCVSAlgoQdspClassicBuildParams,
  DCVSAlgoQdspClassicDecideMips,
  DCVSAlgoQdspClassicReset,
  DCVSAlgoQdspClassicEnable,
  DCVSAlgoQdspClassicDisable
};
#endif //DEFEATURE_DCVS_ALGO_QDSP_CLASSIC

