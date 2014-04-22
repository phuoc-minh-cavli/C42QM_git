/*
===========================================================================
*/
/**
  @file ClockApps.c

  Main entry point for the application processor clock driver.
*/
/*
  ====================================================================
  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR

  ====================================================================
  $Header: //components/rel/core.tx/6.0/systemdrivers/clock/hw/mdm9205/apps/src/ClockApps.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $
 
  when       who     what, where, why
  --------   ---     -------------------------------------------------
  01/21/19   shm     Changes for passing RPM assisted PC flag to kernel
  01/08/19   shm     Featurizing stub and test framework code for memory
                     optimization
  10/02/18   shm     First version for 9205

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "DALDeviceId.h"
#include "ClockDriver.h"
#include "ClockApps.h"
#include "ClockSWEVT.h"
#include "busywait.h"
#include "qurt_system.h"
#include "npa_resource.h"


/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/

#ifdef CLOCK_STUB_FWK_ENABLE
static const char *ClockNPANodeName[] =
{
  CLOCK_NPA_NODE_NAME_CPU,
  CLOCK_NPA_NODE_NAME_CPU_TEST,
  CLOCK_NPA_NODE_NAME_PCNOC,
  CLOCK_NPA_NODE_NAME_CXO,
  CLOCK_NPA_NODE_NAME_QDSS,
  CLOCK_NPA_NODE_NAME_BIMC,
  CLOCK_NPA_NODE_NAME_QPIC,
};
#endif

static ClockImageCtxtType Clock_ImageCtxt;


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : Clock_DetectBSPVersion
** =========================================================================*/
/**
  Detects which BSP configuration to use for the current HW.

  @param *pDrvCtxt [in] -- Pointer to driver context.

  @return
  DAL_ERROR if a valid configuration was not found, other DAL_SUCCESS.

  @dependencies
  None.
*/

static DALResult Clock_DetectBSPVersion
(
  ClockDrvCtxt *pDrvCtxt
)
{
  ClockImageCtxtType      *pImageCtxt;
  ClockCPUPerfConfigType  *pCPUPerfConfig;
  uint32                  i;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Detect which CPU BSP data to use for this HW version.                 */
  /*-----------------------------------------------------------------------*/

  pCPUPerfConfig = pImageCtxt->pBSPConfig->pCPUPerfConfig;
  for (i = 0; i < pImageCtxt->pBSPConfig->nNumCPUPerfLevelConfigs; i++)
  {
    if (Clock_IsBSPSupported(&pCPUPerfConfig[i].HWVersion) == TRUE)
    {
      pImageCtxt->CPUCtxt.PerfConfig.HWVersion = pCPUPerfConfig[i].HWVersion;
      pImageCtxt->CPUCtxt.PerfConfig.anPerfLevel = pCPUPerfConfig[i].anPerfLevel;
      pImageCtxt->CPUCtxt.PerfConfig.nMaxPerfLevel = pCPUPerfConfig[i].nMaxPerfLevel;
      pImageCtxt->CPUCtxt.PerfConfig.nMinPerfLevel = pCPUPerfConfig[i].nMinPerfLevel;
      pImageCtxt->CPUCtxt.PerfConfig.nNumPerfLevels = pCPUPerfConfig[i].nNumPerfLevels;

      break;
    }
  }

  if (i == pImageCtxt->pBSPConfig->nNumCPUPerfLevelConfigs)
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;

} /* END of Clock_DetectBSPVersion */


/* =========================================================================
**  Function : Clock_InitImage
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_InitImage
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult              eResult;
  ClockPropertyValueType PropVal;

  /*-----------------------------------------------------------------------*/
  /* Assign the image context.                                             */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->pImageCtxt = &Clock_ImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Initialize the SW Events for Clocks.                                  */
  /*-----------------------------------------------------------------------*/

  Clock_SWEvent(CLOCK_EVENT_INIT, 0);

  /*-----------------------------------------------------------------------*/
  /* Get the CPU BSP Configuration.                                        */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_GetPropertyValue("ClockImageBSP", &PropVal);
  if (eResult != DAL_SUCCESS)
  {
    ULOG_RT_PRINTF_0(
      pDrvCtxt->hClockLog,
      "Unable to get Clock BSP Property: ClockImageBSP.");

    return eResult;
  }

  Clock_ImageCtxt.pBSPConfig = (ClockImageBSPConfigType *)PropVal;

  /*-----------------------------------------------------------------------*/
  /* Detect the BSP version to use.                                        */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectBSPVersion(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "Failed to detect the BSP version.",0,0,0);

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the XO module.                                             */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitXO(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "Unable to init XO.",0,0,0);

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the PLL resources.                                         */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitPLL(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "Unable to init the PLL.",0,0,0);

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize CPU core clock frequency configuration.                    */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitDCVS(pDrvCtxt);

  if (eResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "Unable to initialize CPU DCVS.",0,0,0);

    return eResult;
  }

  #ifdef CLOCK_BIST_FWK_ENABLE
  /*-----------------------------------------------------------------------*/
  /* Run the NPA BIST (if enabled) for Q6 & bus clocks switching           */
  /*-----------------------------------------------------------------------*/

  if (Clock_ImageCtxt.CPUCtxt.bNPABISTEnabled)
  {
    if (Clock_ImageCtxt.nNPABISTLogSize == 0)
    {
      Clock_ImageCtxt.nNPABISTLogSize = CLOCK_NPA_BIST_DEFAULT_LOG_SIZE;
    }
    Clock_NPA_BIST(pDrvCtxt);
  }
  #endif

  /*-----------------------------------------------------------------------*/
  /* Good to go.                                                           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitImage */


#ifdef CLOCK_STUB_FWK_ENABLE
/* =========================================================================
**  Function : ClockStub_InitImage
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult ClockStub_InitImage
(
  ClockDrvCtxt *pDrvCtxt
)
{
  uint32 i;

  /*-----------------------------------------------------------------------*/
  /* Initialize the SW Events for Clocks.                                  */
  /*-----------------------------------------------------------------------*/

  Clock_SWEvent(CLOCK_EVENT_INIT, 0);

  /*-----------------------------------------------------------------------*/
  /* Create stubs for each NPA node.                                       */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < ARR_SIZE(ClockNPANodeName); i++)
  {
    npa_stub_resource(ClockNPANodeName[i]);
  }

  /*-----------------------------------------------------------------------*/
  /* Good to go.                                                           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END ClockStub_InitImage */
#endif

/* =========================================================================
**  Function : Clock_ProcessorSleep
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ProcessorSleep
(
  ClockDrvCtxt       *pDrvCtxt,
  ClockSleepModeType  eMode,
  uint32              nFlags
)
{
  ClockImageCtxtType      *pImageCtxt;
  ClockDomainNodeType     *pDomain;
  ClockSourceNodeType     *pSourcePrevious;
  ClockSourceNodeType     *pSourceNew;
  ClockCPUConfigType      *pCPUConfig, *pSleepCPUConfig;
  ClockCPUPerfConfigType  *pCPUPerfConfig;
  HAL_clk_SourceType       eSource;
  DALResult                eResult;
  uint32                   nPL, nConfig, nSourceIndex, nKernelResult;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Determine if we should ramp down the Sparrow core clock frequency.    */ 
  /*-----------------------------------------------------------------------*/

  if ((nFlags & CLOCK_SLEEP_FLAG_RAMP_DOWN_CPU) != 0)
  {
    /*
     * Thread Safety - Critical Section [Start]
     */
    DALCLOCK_LOCK(pDrvCtxt);

    /*
     * Get pointers to the CPU config data.
     */
    pCPUConfig = pImageCtxt->CPUCtxt.pConfig;
    pCPUPerfConfig = &pImageCtxt->CPUCtxt.PerfConfig;

    /*
     * Get pointer to the sleep CPU config.
     */
    nPL = pImageCtxt->pBSPConfig->nSleepPerfLevel;
    nConfig = pCPUPerfConfig->anPerfLevel[nPL];
    pSleepCPUConfig = &pImageCtxt->pBSPConfig->pCPUConfig[nConfig];

    if ((pSleepCPUConfig == NULL) || (pCPUConfig == NULL))
    {
      DALCLOCK_FREE(pDrvCtxt);
      return DAL_ERROR_INTERNAL;
    }

    /*
     * Get the current and new source pointers.
     */
    eSource = pCPUConfig->Mux.HALConfig.eSource;
    nSourceIndex = pDrvCtxt->anSourceIndex[eSource];
    pSourcePrevious = &pDrvCtxt->aSources[nSourceIndex];

    eSource = pSleepCPUConfig->Mux.HALConfig.eSource;
    nSourceIndex = pDrvCtxt->anSourceIndex[eSource];
    pSourceNew = &pDrvCtxt->aSources[nSourceIndex];

    /*
     * Short-circuit check if the sleep CPU config is already applied.
     */
    if (pSleepCPUConfig != pCPUConfig)
    {
      /*
       * Enable the source required by the sleep configuration. We assume none
       * of the PLLs that source the Sparrow have voltage requirements, i.e.
       * they all run at low voltage. This guarantees we won't make any voltage
       * requests at this point when the path to the RPM is shut down.
       */
      if (pSourceNew != pSourcePrevious)
      {
        eResult = Clock_EnableSourceInternal(pDrvCtxt, pSourceNew, TRUE);
        if (eResult != DAL_SUCCESS)
        {
          DALCLOCK_FREE(pDrvCtxt);
          return DAL_ERROR_INTERNAL;
        }
      }

      /*
       * Switch the Sparrow core clock to the sleep configuration.
       *
       * NOTE: We use a HAL call instead of going through the driver so that
       *       we don't trigger a voltage update. The sleep configuration must
       *       run at SVS.
       */
      pDomain = pImageCtxt->CPUCtxt.pClock->pDomain;
      HAL_clk_ConfigClockMux(pDomain->HALHandle, &pSleepCPUConfig->Mux.HALConfig);
      pDomain->pActiveMuxConfig = &pSleepCPUConfig->Mux;

      /*
       * If we had switched to a different source, manually remove our vote on the
       * previous source since we've bypassed the driver.
       */
      if (pSourceNew != pSourcePrevious)
      {
        eResult = Clock_DisableSourceInternal(pDrvCtxt, pSourcePrevious, TRUE, FALSE);
        if (eResult != DAL_SUCCESS)
        {
          DALCLOCK_FREE(pDrvCtxt);
          return DAL_ERROR_INTERNAL;
        }

        /*
         * Link the new source node to the clock domain. pSourceNew should already
         * point to the correct source.
         */
        pDomain->pSource = pSourceNew;
      }

      /*
       * Update the saved and active clock configs.
       */
      pImageCtxt->CPUCtxt.pSavedConfig = pCPUConfig;
      pImageCtxt->CPUCtxt.pConfig = pSleepCPUConfig;
    }

    /*
     * Thread Safety - Critical Section [End]
     */
    DALCLOCK_FREE(pDrvCtxt);
  }
  else
  {
    pImageCtxt->CPUCtxt.pSavedConfig = NULL;
  }

  /*-----------------------------------------------------------------------*/
  /* Determine if we should halt or power collapse.                        */ 
  /*-----------------------------------------------------------------------*/

  switch (eMode)
  {
    case CLOCK_SLEEP_MODE_HALT:
      /*
       * Make a kernel call for Standby and Wait for Interrupt (SWFI).  The
       * CPU core clock is halted and waits for any interrupt to occur.
       */
      nKernelResult =
        qurt_power_shutdown_enter(
          QURT_POWER_SHUTDOWN_TYPE_WFI,
          nFlags & CLOCK_SLEEP_FLAG_RPM_ASSISTED_PC);
      if (nKernelResult != QURT_EOK)
      {
        return DAL_ERROR_INTERNAL;
      }
      break;

    case CLOCK_SLEEP_MODE_POWER_COLLAPSE:
      /*
       * Make a kernel call for power collapse.
       */
      if ((nFlags & CLOCK_SLEEP_FLAG_L2CACHE_FLUSH) != 0)
      {
        /*
         * Flush the contents of L2 cache during power collapse.
         */
        nKernelResult =
          qurt_power_shutdown_enter(
          QURT_POWER_SHUTDOWN_TYPE_L2NORET,
          nFlags & CLOCK_SLEEP_FLAG_RPM_ASSISTED_PC);
        if (nKernelResult != QURT_EOK)
        {
          return DAL_ERROR_INTERNAL;
        }
      }
      else
      {
        /*
         * Preserve the contents of L2 cache during power collapse.
         */
        nKernelResult =
          qurt_power_shutdown_enter(
          QURT_POWER_SHUTDOWN_TYPE_L2RET,
          nFlags & CLOCK_SLEEP_FLAG_RPM_ASSISTED_PC);
        if (nKernelResult != QURT_EOK)
        {
          return DAL_ERROR_INTERNAL;
        }
      }
      break;

    default:
      break;
  }

  /*-----------------------------------------------------------------------*/
  /* We're back up and running now.                                        */ 
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_ProcessorSleep */


/* =========================================================================
**  Function : Clock_ProcessorRestore
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ProcessorRestore
(
  ClockDrvCtxt       *pDrvCtxt,
  ClockSleepModeType  eMode,
  uint32              nFlags
)
{
  ClockImageCtxtType      *pImageCtxt;
  ClockDomainNodeType     *pDomain;
  ClockSourceNodeType     *pSourcePrevious;
  ClockSourceNodeType     *pSourceNew;
  ClockCPUConfigType      *pCPUConfig, *pSavedCPUConfig;
  HAL_clk_SourceType       eSource;
  DALResult                eResult;
  uint32                   nSourceIndex;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Determine if we need to restore the Sparrow core clock frequency.     */ 
  /*-----------------------------------------------------------------------*/

  if (pImageCtxt->CPUCtxt.pSavedConfig != NULL)
  {
    /*
     * Thread Safety - Critical Section [Start]
     */
    DALCLOCK_LOCK(pDrvCtxt);

    /*
     * Get pointers to the CPU config data.
     */
    pCPUConfig = pImageCtxt->CPUCtxt.pConfig;
    pSavedCPUConfig = pImageCtxt->CPUCtxt.pSavedConfig;

    if ((pSavedCPUConfig == NULL) || (pCPUConfig == NULL))
    {
      DALCLOCK_FREE(pDrvCtxt);
      return DAL_ERROR_INTERNAL;
    }

    /*
     * Get the current and new source pointers.
     */
    eSource = pCPUConfig->Mux.HALConfig.eSource;
    nSourceIndex = pDrvCtxt->anSourceIndex[eSource];
    pSourcePrevious = &pDrvCtxt->aSources[nSourceIndex];

    eSource = pSavedCPUConfig->Mux.HALConfig.eSource;
    nSourceIndex = pDrvCtxt->anSourceIndex[eSource];
    pSourceNew = &pDrvCtxt->aSources[nSourceIndex];

    /*
     * Enable the source required by the saved configuration. We assume none
     * of the PLLs that source the Sparrow have voltage requirements, i.e.
     * they all run at low voltage. This guarantees we won't make any voltage
     * requests at this point when the path to the RPM is shut down.
     */
    if (pSourceNew != pSourcePrevious)
    {
      eResult = Clock_EnableSourceInternal(pDrvCtxt, pSourceNew, TRUE);
      if (eResult != DAL_SUCCESS)
      {
        DALCLOCK_FREE(pDrvCtxt);
        return DAL_ERROR_INTERNAL;
      }
    }

    /*
     * Restore the Sparrow core clock to its original configuration.
     *
     * NOTE: We use a HAL call instead of going through the driver so that
     *       we don't trigger a voltage update. The sleep configuration must
     *       run at SVS.
     */
    pDomain = pImageCtxt->CPUCtxt.pClock->pDomain;
    HAL_clk_ConfigClockMux(pDomain->HALHandle, &pSavedCPUConfig->Mux.HALConfig);
    pDomain->pActiveMuxConfig = &pSavedCPUConfig->Mux;

    /*
     * If we had switched to a different source, manually remove our vote on the
     * previous source since we've bypassed the driver.
     */
    if (pSourceNew != pSourcePrevious)
    {
      eResult = Clock_DisableSourceInternal(pDrvCtxt, pSourcePrevious, TRUE, FALSE);
      if (eResult != DAL_SUCCESS)
      {
        DALCLOCK_FREE(pDrvCtxt);
        return DAL_ERROR_INTERNAL;
      }

      /*
       * Link the new source node to the clock domain. pSourceNew should already
       * point to the correct source.
       */
      pDomain->pSource = pSourceNew;
    }

    /*
     * Update the driver state.
     */
    pImageCtxt->CPUCtxt.pConfig = pSavedCPUConfig;
    pImageCtxt->CPUCtxt.pSavedConfig = NULL;

    /*
     * Thread Safety - Critical Section [End]
     */
    DALCLOCK_FREE(pDrvCtxt);
  }

  return DAL_SUCCESS;

} /* END Clock_ProcessorRestore */


#ifdef CLOCK_BIST_FWK_ENABLE
/* =========================================================================
**  Function : Clock_NPA_BIST
** =========================================================================*/
/**
  See ClockMSS.h

*/

void Clock_NPA_BIST(ClockDrvCtxt *pDrvCtxt)
{
  npa_client_handle             cpu_h;
  DALResult                     nResult;
  ClockImageCtxtType           *pImageCtxt;
  ClockCPUConfigType           *pCPUConfig;
  ClockCPUPerfConfigType       *pCPUPerfConfig;
  npa_query_type                cpu_state, test_state;
  uint32                        i, nFreqHzNPA, nFreqHzCalc, nDisableDCS, nCfg;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;
  pCPUPerfConfig = pImageCtxt->pBSPConfig->pCPUPerfConfig;
  pCPUConfig = pImageCtxt->pBSPConfig->pCPUConfig;

  /*-----------------------------------------------------------------------*/
  /* Initialize the NPA BIST log.                                          */
  /*-----------------------------------------------------------------------*/

  ULogFront_RealTimeInit(
    &pImageCtxt->hClockNPABISTLog, "Clock NPA BIST Log",
    pImageCtxt->nNPABISTLogSize,
    ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
  ULogCore_HeaderSet(
    pImageCtxt->hClockNPABISTLog,
    "Content-Type: text/tagged-log-1.0;");

  /*-----------------------------------------------------------------------*/
  /* Test clocks through NPA                                               */
  /*-----------------------------------------------------------------------*/

  if (pImageCtxt->CPUCtxt.bNPABISTEnabled)
  {
    /* Enable DCS after saving current nDisableDCS flag */
    nDisableDCS = pImageCtxt->CPUCtxt.nDisableDCS;
    pImageCtxt->CPUCtxt.nDisableDCS = 0;

    cpu_h = npa_create_sync_client(
              CLOCK_NPA_NODE_NAME_CPU, "CPU_BIST", NPA_CLIENT_SUPPRESSIBLE);

    /* Save current Q6 state to restore later */
    npa_query_by_name(
      CLOCK_NPA_NODE_NAME_CPU, NPA_QUERY_CURRENT_STATE, &cpu_state);

    for (i = pCPUPerfConfig->nMinPerfLevel;
         i <= pCPUPerfConfig->nMaxPerfLevel; i++)
    {
      nCfg = pCPUPerfConfig->anPerfLevel[i];
      ULOG_RT_PRINTF_1(
        pImageCtxt->hClockNPABISTLog,
        "Testing Q6 clock for frequency: %d Hz",
        pCPUConfig[nCfg].Mux.nFreqHz);

      npa_issue_scalar_request(cpu_h, pCPUConfig[nCfg].Mux.nFreqHz/1000);
      npa_query_by_name(
        CLOCK_NPA_NODE_NAME_CPU, NPA_QUERY_CURRENT_STATE, &test_state);
      nFreqHzNPA = NPA_TO_HZ(test_state.data.state);

      ULOG_RT_PRINTF_1 (pImageCtxt->hClockNPABISTLog,
                        "CPU resource state frequency   : %d Hz", nFreqHzNPA);

      nResult = Clock_CalcClockFrequency(
                  pDrvCtxt, (ClockIdType) pImageCtxt->CPUCtxt.pClock,
                  &nFreqHzCalc);

      if (nResult == DAL_SUCCESS)
      {
        if (ABS_DIFF(nFreqHzNPA, nFreqHzCalc) >
            (CLOCK_NPA_BIST_ERROR_MARGIN_PERCENT/100)*nFreqHzNPA)
        {
          ULOG_RT_PRINTF_1(
            pImageCtxt->hClockNPABISTLog,
            "CPU calculated frequency       : %d Hz", nFreqHzCalc);
        }
        else
        {
          ULOG_RT_PRINTF_0(
            pImageCtxt->hClockNPABISTLog, "CPU clock set frequency: SUCCESS");
        }
      }
      else
      {
        ULOG_RT_PRINTF_0 (
          pImageCtxt->hClockNPABISTLog,
          "Clock_CalcClockFrequency failed for CPU clock");
      }
    }

    /* Restore CPU state & nDisableDCS flag*/
    npa_issue_scalar_request(cpu_h, cpu_state.data.state);
    pImageCtxt->CPUCtxt.nDisableDCS = nDisableDCS;
  }

  return;

} /* END Clock_NPA_BIST */
#endif
