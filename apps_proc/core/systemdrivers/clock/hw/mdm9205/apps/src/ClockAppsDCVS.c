/*
===========================================================================
*/
/**
  @file ClockAppsDCVS.c

  DCVS NPA node definitions for the clock driver.
*/
/*
  ====================================================================
  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR
  ====================================================================
  $Header: //components/rel/core.tx/6.0/systemdrivers/clock/hw/mdm9205/apps/src/ClockAppsDCVS.c#1 $ 
  $DateTime: 2020/01/30 22:49:35 $ 
  $Author: pwbldsvc $
 
  when       who     what, where, why
  --------   ---     -------------------------------------------------
  11/05/18   shm     Clock driver changes
  10/02/18   shm     First version for 9205

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockDriver.h"
#include "ClockApps.h"
#include "ClockSWEVT.h"
#include "busywait.h"

#include "DALSys.h"
#include "npa.h"
#include "npa_scheduler.h"
#include "npa_resource.h"
#include "npa_remote.h"
#include "npa_remote_resource.h"
#include "pmapp_npa.h"
#include "rcecb.h"
#include "rcinit.h"


/*=========================================================================
      Macros 
==========================================================================*/

/*
 * NPA Resource handles
 */
#define NPA_RESOURCE_CLK_CPU                                              \
  Clock_NPACPUResources.aResource[CLOCK_APPS_RESOURCE_CLK_CPU].handle

#define NPA_RESOURCE_CLK_CPU_TEST                                         \
  Clock_NPACPUResources.aResource[CLOCK_APPS_RESOURCE_CLK_CPU_TEST].handle

/*
 * Clock ID's
 */
#define CLOCK_ID_CPU                        "sparrow_clk_internal"


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * Enumeration of local CPU resources.
 */
enum
{
  CLOCK_APPS_RESOURCE_CLK_CPU,
  CLOCK_APPS_RESOURCE_CLK_CPU_TEST,
  CLOCK_APPS_RESOURCE_TOTAL
};


/*
 * Clock_NPACPUResourcesType
 *
 * Structure containing the NPA node and resource data for local nodes.
 *
 * aResource[CLOCK_APPS_RESOURCE_TOTAL]
 *   - resource data   - /clk/cpu
 *   - resource data   - /clk/cpu.test
 * dependency     - CPU dependency  - /vdd/cx
 * node           - CPU node data   - /node/clk/cpu
 * hClientDCVS    - NPA client      - /clk/dcvs.ena
 * hClientRail    - NPA client      - /vdd/cx
 */
typedef struct
{
  npa_resource_definition aResource[CLOCK_APPS_RESOURCE_TOTAL];
  npa_node_dependency     dependency;
  npa_node_definition     node;  
  npa_client_handle       hClientDCVS;
  npa_client_handle       hClientRail;
} Clock_NPACPUResourcesType;


/*=========================================================================
      Prototypes
==========================================================================*/


static npa_resource_state Clock_NPACPUNodeDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
);

static npa_resource_state Clock_NPACPUResourceDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
);

static npa_resource_state Clock_NPACPUTestResourceDriverFunc
(
  npa_resource        *pResource,
  npa_client_handle    hClient,
  npa_resource_state   nState
);

static void Clock_NPACPUNodeAvailableCallback
(
  void         *pContext,
  unsigned int  nEventType,
  void         *pNodeName,
  unsigned int  nNodeNameSize
);

static npa_query_status Clock_NPACPUResourceQuery
(
  npa_resource    *resource,
  unsigned int     id,
  npa_query_type  *result
);

static DALResult Clock_SetCPUFrequency
(
  ClockDrvCtxt *pDrvCtxt,
  uint32        nFreqHz
);

static void Clock_RCECBCallback
(
  void
);


/*=========================================================================
      Data
==========================================================================*/

/*
 * NPA remote resources used on apps
 */
static npa_remote_resource_definition Clock_aNPARemoteResources[] =
{
  {
    CLOCK_NPA_NODE_NAME_QDSS,
    "clk0\x01\x00\x00\x00",
    "/protocol/rpm/rpm",
    &npa_max_plugin,
    npa_remote_resource_local_aggregation_driver_fcn, 
    "STATE",
    NPA_MAX_STATE
  },
  {
    CLOCK_NPA_NODE_NAME_QPIC,
    "qpic\x00\x00\x00\x00",
    "/protocol/rpm/rpm",
    &npa_max_plugin,
    npa_remote_resource_local_aggregation_driver_fcn, 
    "KHz",
    NPA_MAX_STATE
  },
  {
    CLOCK_NPA_NODE_NAME_BIMC,
    "clk2\x00\x00\x00\x00",
    "/protocol/rpm/rpm",
    &npa_max_plugin,
    npa_remote_resource_local_aggregation_driver_fcn,
    "KHz",
    NPA_MAX_STATE
  },
  {
    CLOCK_NPA_NODE_NAME_PCNOC,
    "clk1\x00\x00\x00\x00",
    "/protocol/rpm/rpm",
    &npa_max_plugin,
    npa_remote_resource_local_aggregation_driver_fcn,
    "KHz",
    NPA_MAX_STATE
  },
  {
    CLOCK_NPA_NODE_NAME_DCVS,
    "clk0\x02\x00\x00\x00",
    "/protocol/rpm/rpm",
    &npa_max_plugin,
    npa_remote_resource_local_aggregation_driver_fcn,
    "Enable",
    NPA_MAX_STATE
  },
};


/*
 * Define the CPU clock resources.
 */
static Clock_NPACPUResourcesType Clock_NPACPUResources =
{
  /*
   * aResource
   */
  {
    /*
     * CPU
     */
    {
      CLOCK_NPA_NODE_NAME_CPU,
      "KHz",
      0,  /* Max, fill in later */
      &npa_max_plugin,
      NPA_RESOURCE_DEFAULT | NPA_RESOURCE_IMPULSE_UNCONDITIONAL,
      (npa_user_data)NULL,
      Clock_NPACPUResourceQuery
    },

    /*
     * CPU Test
     */
    {
      CLOCK_NPA_NODE_NAME_CPU_TEST,
      "KHz",
      0,  /* Max, fill in later */
      &npa_max_plugin,
      NPA_RESOURCE_SINGLE_CLIENT,
      (npa_user_data)NULL,
      Clock_NPACPUResourceQuery
    }
  },

  /*
   * dependency
   */
  {
    NULL,          // Fill in rail string name from BSP data at init
    NPA_NO_CLIENT  // No client used because NPA doesn't allow specifying
                   // additional client data as part of this node creation
                   // process.  A separate client handle is created using
                   // a different NPA API that does allow client data parameters.
  },

  /*
   * node
   */
  { 
    "/node" CLOCK_NPA_NODE_NAME_CPU,                /* name       */
    Clock_NPACPUNodeDriverFunc,                     /* driver_fcn */
    NPA_NODE_DEFAULT,                               /* attributes */
    NULL,                                           /* data       */
    1,                                              /* dep count  */
    &Clock_NPACPUResources.dependency,              /* dependency */
    ARR_SIZE(Clock_NPACPUResources.aResource),      /* res count  */
    Clock_NPACPUResources.aResource                 /* resource   */
  }
};


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : Clock_SetCPUConfig
** =========================================================================*/
/*
  See ClockApps.h
*/

DALResult Clock_SetCPUConfig
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockCPUConfigType  *pNewConfig
)
{
  ClockImageCtxtType  *pImageCtxt;
  ClockCPUConfigType  *pPreviousConfig;
  ClockNodeType       *pClock;
  ClockSourceNodeType *pSource;
  DALResult            eResult;
  uint32               nSourceIndex;

  /*-----------------------------------------------------------------------*/
  /* NOTE:                                                                 */
  /*   This function mostly resembles the sequence found in the common     */
  /*   function Clock_SetClockConfig.  The reason it was not invoked       */
  /*   directly was because the logic to reconfigure a source which had    */
  /*   been put into a special sleep mode (e.g. freeze, standby, saved)    */
  /*   would have been a one time use only implementation on 9x45 due to   */
  /*   the fact that the source architectures and the implementations to   */
  /*   manage them are changing drasticly enough on future targets.        */
  /*   Instead this image specific implementation was devised to resolve   */
  /*   the special needs of these source types when reconfiguring them.    */
  /*-----------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------*/
  /* Validate arguments.                                                   */
  /*-----------------------------------------------------------------------*/

  if (pNewConfig == NULL)
  {
    return DAL_ERROR_INVALID_POINTER;
  }

  /*-----------------------------------------------------------------------*/
  /* Get config data.                                                      */
  /*-----------------------------------------------------------------------*/

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;
  pPreviousConfig = pImageCtxt->CPUCtxt.pConfig;
  pClock = pImageCtxt->CPUCtxt.pClock;

  /*-----------------------------------------------------------------------*/
  /* Short-circuit if the configuration is already active.                 */
  /*-----------------------------------------------------------------------*/

  if (pNewConfig == pPreviousConfig)
  {
    return DAL_SUCCESS;
  }

  
  /*-----------------------------------------------------------------------*/
  /* Get the new source pointer.                                           */
  /*-----------------------------------------------------------------------*/

  nSourceIndex = pDrvCtxt->anSourceIndex[pNewConfig->Mux.HALConfig.eSource];
  if (nSourceIndex == 0xFF)
  {
    return DAL_ERROR_INTERNAL;
  }

  pSource = &pDrvCtxt->aSources[nSourceIndex];
  if (pSource == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Begin critical section.                                               */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Prepare new source if different from the current source.              */
  /*                                                                       */
  /* For a source that is off in SW but not in the completely off state in */
  /* HW (e.g. frozen, standby, etc.) we need to first enable the source    */
  /* before reconfiguring it.                                              */
  /*                                                                       */
  /* For a source that is in the full off state we can enable it here.     */
  /*-----------------------------------------------------------------------*/

  if (pSource != pClock->pDomain->pSource)
  {
    eResult = Clock_EnableSourceInternal(pDrvCtxt, pSource, TRUE);
    if (eResult != DAL_SUCCESS)
    {
      DALCLOCK_FREE(pDrvCtxt);
      return DAL_ERROR_INTERNAL;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* If we are increasing the voltage requirement, do so now.              */
  /*-----------------------------------------------------------------------*/

  if (pNewConfig->Mux.eVRegLevel > pClock->pDomain->VRegRequest.eVRegLevel)
  {
    /*
     * Verify we can satisfy the change in voltage requirement.
     */
    if (Clock_NPACPUResources.hClientRail == NULL)
    {
      ERR_FATAL(
      "CPU handle to voltage rail NPA resource is NULL - unable to satisfy change in voltage requirement.",0,0,0);
    }


    npa_issue_scalar_request(
      Clock_NPACPUResources.hClientRail,
      pNewConfig->Mux.eVRegLevel);
  }
   /*-----------------------------------------------------------------------*/
  /* Reconfigure the source if required for the CPU configuration.         */
  /*-----------------------------------------------------------------------*/

  if (pNewConfig->Mux.pSourceFreqConfig != NULL)
  {
    eResult =
      Clock_ConfigSource(
        pDrvCtxt,
        pSource,
        pNewConfig->Mux.pSourceFreqConfig);
    if (eResult != DAL_SUCCESS)
    {
      DALCLOCK_FREE(pDrvCtxt);
      return DAL_ERROR_INTERNAL;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Switch CPU core clock.                                                */
  /*-----------------------------------------------------------------------*/

  HAL_clk_ConfigClockMux(pClock->pDomain->HALHandle, &pNewConfig->Mux.HALConfig);

  /*-----------------------------------------------------------------------*/
  /* If we are decreasing the voltage requirement, do so now.              */
  /*-----------------------------------------------------------------------*/

  if (pNewConfig->Mux.eVRegLevel < pClock->pDomain->VRegRequest.eVRegLevel)
  {
    /*
     * Verify we can satisfy the change in voltage requirement.
     */
    if (Clock_NPACPUResources.hClientRail == NULL)
    {
      ERR_FATAL(
        "CPU handle to voltage rail NPA resource is NULL - unable to satisfy change in voltage requirement.",0,0,0);
    }

    npa_issue_scalar_request(
      Clock_NPACPUResources.hClientRail,
      pNewConfig->Mux.eVRegLevel);
  }

  /*-----------------------------------------------------------------------*/
  /* Disable previous source if different from the new active source.      */
  /*-----------------------------------------------------------------------*/

  if (pSource != pClock->pDomain->pSource)
  {
    eResult =
      Clock_DisableSourceInternal(
        pDrvCtxt,
        pClock->pDomain->pSource,
        TRUE,
        FALSE);
    if (eResult != DAL_SUCCESS)
    {
      DALCLOCK_FREE(pDrvCtxt);
      return DAL_ERROR_INTERNAL;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update state.                                                         */
  /*-----------------------------------------------------------------------*/

  pClock->pDomain->pSource = pSource;
  pClock->pDomain->pActiveMuxConfig = &pNewConfig->Mux;
  pImageCtxt->CPUCtxt.pConfig = pNewConfig;

  /*-----------------------------------------------------------------------*/
  /* Update CPU domain VRegRequest so that ClockDriver.cmm knows how to    */
  /* interpret the voltage requirement for this domain.                    */
  /*-----------------------------------------------------------------------*/

  pClock->pDomain->VRegRequest.eVRegLevel = pNewConfig->Mux.eVRegLevel;

  /*-----------------------------------------------------------------------*/
  /* End critical section.                                                 */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_SetCPUConfig */


/* =========================================================================
**  Function : Clock_SetCPUFrequency
** =========================================================================*/
/**
  Sets CPU frequency

  This function sets CPU frequency based on input parameters.

  @param *pDrvCtxt [in] -- Pointer to driver context.
  @param  nFreqHz  [in] -- New frequency.

  @return
  DAL_SUCCESS -- CPU clock set successfully.
  DAL_ERROR  --  CPU clock not set successfully.

  @dependencies
  None.
*/

static DALResult Clock_SetCPUFrequency
(
  ClockDrvCtxt *pDrvCtxt,
  uint32        nFreqHz
)
{
  ClockImageCtxtType     *pImageCtxt;
  ClockCPUPerfConfigType *pCPUPerfConfig;
  ClockCPUConfigType     *pCPUConfig;
  DALResult               eResult;
  uint32                  nCfg, nPL;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;
  pCPUPerfConfig = &pImageCtxt->CPUCtxt.PerfConfig;
  pCPUConfig  = pImageCtxt->pBSPConfig->pCPUConfig;

  /*-----------------------------------------------------------------------*/
  /* Find minimum performance level.                                       */
  /*-----------------------------------------------------------------------*/

  nPL = pCPUPerfConfig->nMinPerfLevel;
  for ( ; nPL < pCPUPerfConfig->nMaxPerfLevel; nPL++)
  {
    nCfg = pCPUPerfConfig->anPerfLevel[nPL];
    if (pCPUConfig[nCfg].Mux.nFreqHz >= nFreqHz)
    {
      break;
    }
  }

  nCfg = pCPUPerfConfig->anPerfLevel[nPL];

  /*-----------------------------------------------------------------------*/
  /* Switch the CPU clock.                                                 */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_SetCPUConfig(pDrvCtxt, &pCPUConfig[nCfg]);

  return eResult;

} /* END of Clock_SetCPUFrequency */


/* =========================================================================
**  Function : Clock_FindCPUMaxConfigAtVoltage
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_FindCPUMaxConfigAtVoltage
(
  ClockDrvCtxt         *pDrvCtxt,
  ClockCPUConfigType  **pConfig,
  uint32                nCorner
)
{
  ClockImageCtxtType *pImageCtxt;
  ClockCPUConfigType *pCPUConfig, *pTempConfig;
  uint32              nMinPL, nMaxPL, nPL, nCfg;

  /*-----------------------------------------------------------------------*/
  /* Get config data.                                                      */
  /*-----------------------------------------------------------------------*/

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Validate argument.                                                    */
  /*-----------------------------------------------------------------------*/

  if (pConfig == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Find the highest frequency configuration.                             */
  /*-----------------------------------------------------------------------*/

  nMinPL = pImageCtxt->CPUCtxt.PerfConfig.nMinPerfLevel;
  nMaxPL = pImageCtxt->CPUCtxt.PerfConfig.nMaxPerfLevel;
  pCPUConfig = pImageCtxt->pBSPConfig->pCPUConfig;
  pTempConfig = NULL;

  for (nPL = nMinPL; nPL <= nMaxPL; nPL++)
  {
    nCfg = pImageCtxt->CPUCtxt.PerfConfig.anPerfLevel[nPL];

    if (pCPUConfig[nCfg].Mux.eVRegLevel > nCorner)
    {
      break;
    }
    else
    {
      pTempConfig = &pCPUConfig[nCfg];
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Validate config was found.                                            */
  /*-----------------------------------------------------------------------*/

  if (pTempConfig == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Store config for caller.                                              */
  /*-----------------------------------------------------------------------*/

  *pConfig = pTempConfig;

  return DAL_SUCCESS;

} /* END Clock_FindCPUMaxConfigAtVoltage */


/* =========================================================================
**  Function : Clock_SetCPUMaxFrequencyAtCurrentVoltage
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_SetCPUMaxFrequencyAtCurrentVoltage
(
  ClockDrvCtxt *pDrvCtxt,
  boolean       bRequired,
  uint32       *pnResultFreqHz
)
{
  DALResult           eResult;
  ClockImageCtxtType *pImageCtxt;
  ClockCPUConfigType *pConfig;
  uint32              nCorner;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Find max config @ current voltage if required.                        */
  /*-----------------------------------------------------------------------*/

  if (bRequired == TRUE)
  {
    /*
     * Find the max CPU config at the current voltage.
     */
    nCorner = pImageCtxt->CPUCtxt.pConfig->Mux.eVRegLevel;
    eResult = Clock_FindCPUMaxConfigAtVoltage(pDrvCtxt, &pConfig, nCorner);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }

    /*
     * Configure the CPU to the max supported frequency at the current voltage.
     */
    eResult = Clock_SetCPUFrequency(pDrvCtxt, pConfig->Mux.nFreqHz);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }

    /*
     * Overwrite the active state (in KHz) of the NPA node.
     */
    pConfig = pImageCtxt->CPUCtxt.pConfig;
    npa_assign_resource_state(NPA_RESOURCE_CLK_CPU, pConfig->Mux.nFreqHz / 1000);

    /*
     * Update result for caller.
     */
    if (pnResultFreqHz != NULL)
    {
      *pnResultFreqHz = pConfig->Mux.nFreqHz;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Restore CPU to requested state.                                       */
  /*-----------------------------------------------------------------------*/

  else
  {
    /*
     * Impulse request results in the resource restoring to its prior value.
     */
    npa_issue_impulse_request(pImageCtxt->CPUCtxt.hNPAClkCPUImpulse);

    /*
     * Update result for caller.
     */
    if (pnResultFreqHz != NULL)
    {
      *pnResultFreqHz = pImageCtxt->CPUCtxt.pConfig->Mux.nFreqHz;
    }
  }

  return DAL_SUCCESS;

} /* END Clock_SetCPUMaxFrequencyAtCurrentVoltage */


/* =========================================================================
**  Function : Clock_InitDCVS
** =========================================================================*/
/*
  See ClockDriver.h
*/ 

DALResult Clock_InitDCVS
(
  ClockDrvCtxt *pDrvCtxt
)
{
  uint32              i, nPL, nConfig;
  npa_resource_state  nInitialStates[CLOCK_APPS_RESOURCE_TOTAL];
  ClockImageCtxtType *pImageCtxt;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* We must disable scaling until:                                        */
  /*  - EFS init has complete.                                             */
  /*  NO EFS ON APCS                                                       */
  /*-----------------------------------------------------------------------*/

 // pImageCtxt->CPUCtxt.nDisableDCS = CLOCK_FLAG_DISABLED_BY_EFS;

  /*-----------------------------------------------------------------------*/
  /* Disable DCS if specified in the BSP.                                  */
  /*-----------------------------------------------------------------------*/
  
  if (pImageCtxt->pBSPConfig->bEnableDCS != TRUE)
  {
    pImageCtxt->CPUCtxt.nDisableDCS |= CLOCK_FLAG_DISABLED_BY_BSP;
  }

  /*-----------------------------------------------------------------------*/
  /* Create callback for end of RCINIT to lower BIMC vote.                 */
  /*-----------------------------------------------------------------------*/
  rcecb_register_name(
    RCINIT_RCECB_INITGROUPS,
    (RCECB_FNSIG_VOID_VOID)Clock_RCECBCallback);

  /*-----------------------------------------------------------------------*/
  /* Define this set of remote nodes with no initial state.                */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < ARR_SIZE(Clock_aNPARemoteResources); i++)
  {
    npa_remote_define_resource(
      &Clock_aNPARemoteResources[i],
      (npa_resource_state)0,
      NULL);
  }

  /*-----------------------------------------------------------------------*/
  /* Define this set of remote nodes with some initial state.              */
  /*-----------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------*/
  /* Init the NPA CPU resource/node to the max supported perf level.       */
  /*-----------------------------------------------------------------------*/
  
  Clock_NPACPUResources.node.data = (npa_user_data)pDrvCtxt;
  Clock_NPACPUResources.dependency.name = pImageCtxt->pBSPConfig->szRailName;

  nPL     = pImageCtxt->CPUCtxt.PerfConfig.nMaxPerfLevel;
  nConfig = pImageCtxt->CPUCtxt.PerfConfig.anPerfLevel[nPL];

  for (i = 0; i < Clock_NPACPUResources.node.resource_count; i++)
  {
    Clock_NPACPUResources.aResource[i].max =
      pImageCtxt->pBSPConfig->pCPUConfig[nConfig].Mux.nFreqHz / 1000;
  }

  nInitialStates[CLOCK_APPS_RESOURCE_CLK_CPU] =
    Clock_NPACPUResources.aResource[CLOCK_APPS_RESOURCE_CLK_CPU].max;
  nInitialStates[CLOCK_APPS_RESOURCE_CLK_CPU_TEST] = 0;

  npa_define_node_cb(
    &Clock_NPACPUResources.node,
    nInitialStates,
    Clock_NPACPUNodeAvailableCallback,
    pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Good to go.                                                           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitDCVS */


/* =========================================================================
**  Function : Clock_EnableDCVS
** =========================================================================*/
/*
  See DDIClock.h
*/ 

DALResult Clock_EnableDCVS
(
  ClockDrvCtxt *pDrvCtxt
) 
{
  return DAL_SUCCESS;

} /* END Clock_EnableDCVS */


/* =========================================================================
**  Function : Clock_InitializeCPUMax
** =========================================================================*/
/**
  Explicitly set the CPU to the maximum supported performance level in order
  to minimize the latency of SW boot up.

  This function is only called once by the CPU resource driver function
  when the CPU resource is being created.  We're guaranteed to have rail voting
  available at this time which allows the CPU clock to be scaled to any
  performance level regardless of how the BSP min/max may have been configured.

  This function avoids explicitly switching to TURBO to avoid potential issues
  with going to the highest voltage which has not always been possible for all
  HW versions and even more so during the bring-up phase of a target.

  @param *pDrvCtxt [in] -- Pointer to driver context.

  @return
  DAL_SUCCESS -- CPU clock initialized successfully.
  DAL_ERROR  --  CPU clock not initialized successfully.

  @dependencies
  None.
*/

static DALResult Clock_InitializeCPUMax
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult            eResult;
  ClockImageCtxtType  *pImageCtxt;
  ClockNodeType       *pClock;
  ClockSourceNodeType *pSource;
  ClockCPUConfigType  *pCPUConfig;
  uint32               nPL, nConfig, nSourceIndex;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Get CPU clock ID.                                                     */
  /*-----------------------------------------------------------------------*/

  eResult =
    Clock_GetClockId(
      pDrvCtxt, CLOCK_ID_CPU,
      (ClockIdType *)&pImageCtxt->CPUCtxt.pClock);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR_INTERNAL;
  }

  pClock = pImageCtxt->CPUCtxt.pClock;

  /*-----------------------------------------------------------------------*/
  /* Switch to a performance level that's not sourced by the APCS PLL so   */
  /* that the APCS PLL can be disabled and re-calibrated for the TN APPS   */
  /* clock plan. The performance level to use during this process is       */
  /* specified in the BSP and uses GPLL0 as a source.                      */
  /*-----------------------------------------------------------------------*/

  nPL = pImageCtxt->pBSPConfig->nPLLCalibrationPerfLevel;
  nConfig = pImageCtxt->pBSPConfig->pCPUPerfConfig->anPerfLevel[nPL];
  pCPUConfig = &pImageCtxt->pBSPConfig->pCPUConfig[nConfig];

  /*-----------------------------------------------------------------------*/
  /* Enable GPLL0 using an FSM vote.                                       */
  /*-----------------------------------------------------------------------*/

  nSourceIndex = pDrvCtxt->anSourceIndex[pCPUConfig->Mux.HALConfig.eSource];
  if (nSourceIndex == 0xFF)
  {
    return DAL_ERROR_INTERNAL;
  }

  pSource = &pDrvCtxt->aSources[nSourceIndex];
  if (pSource == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  HAL_clk_EnableSourceVote(pCPUConfig->Mux.HALConfig.eSource);

  /*-----------------------------------------------------------------------*/
  /* Config the CPU to use GPLL0 at NOMINAL voltage or lower.              */
  /*-----------------------------------------------------------------------*/

  HAL_clk_ConfigClockMux(
    pClock->pDomain->HALHandle,
    &pCPUConfig->Mux.HALConfig);

  /*-----------------------------------------------------------------------*/
  /* Vote for current voltage requirement to initialize the CPU vote.      */
  /*-----------------------------------------------------------------------*/

  npa_issue_scalar_request(
    Clock_NPACPUResources.hClientRail,
    pCPUConfig->Mux.eVRegLevel);

  /*-----------------------------------------------------------------------*/
  /* Disable APCS PLL without saving the calibration since we are not      */
  /* guaranteed to have valid calibration data coming out of boot init.    */
  /*-----------------------------------------------------------------------*/

  HAL_clk_DisableSource(
    HAL_CLK_SOURCE_APCSPLL0,
    HAL_CLK_SOURCE_DISABLE_MODE_NORMAL,
    NULL);

  /*-----------------------------------------------------------------------*/
  /* Update CPU state.                                                     */
  /*-----------------------------------------------------------------------*/

  pClock->pDomain->pSource = pSource;
  pClock->pDomain->pActiveMuxConfig = &pCPUConfig->Mux;
  pImageCtxt->CPUCtxt.pConfig = pCPUConfig;
  /*-----------------------------------------------------------------------*/
  /* Ensure that the CPU core clock/domain/source reference counts are 1.  */
  /*-----------------------------------------------------------------------*/

  eResult =
    Clock_EnableClock(
      pDrvCtxt,
      (ClockIdType)pImageCtxt->CPUCtxt.pClock);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR_INTERNAL;
  }
  
  /*-----------------------------------------------------------------------*/
  /* Scale CPU to initial performance level.                               */
  /*-----------------------------------------------------------------------*/
  
  nPL = pImageCtxt->pBSPConfig->nInitCPUPerfLevel ;
  nConfig = pImageCtxt->pBSPConfig->pCPUPerfConfig->anPerfLevel[nPL];
  pCPUConfig = &pImageCtxt->pBSPConfig->pCPUConfig[nConfig];
  
  eResult = Clock_SetCPUConfig(pDrvCtxt, pCPUConfig); 
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR_INTERNAL;
  }
  return DAL_SUCCESS;

} /* END Clock_InitializeCPUMax */


/* =========================================================================
**  Function : Clock_NPACPUResourceDriverFunc
** =========================================================================*/
/**
  NPA node driver function for the CPU resource.

  This function handles minimum frequency requests for the CPU NPA resource.

  @param *pResource [in] -- Pointer to resource whose state changed.
  @param hClient    [in] -- Client handle that triggered the change.
  @param nState     [in] -- New request state.

  @return
  npa_resource_state -- The new active state of the resource.

  @dependencies
  None.
*/

static npa_resource_state Clock_NPACPUResourceDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{
  ClockDrvCtxt       *pDrvCtxt;
  ClockImageCtxtType *pImageCtxt;
  DALResult           eResult;
  uint32              nDisableDCS, nFreqHz, nPL, nCfg;
  npa_resource_state  nActiveState;

  pDrvCtxt = (ClockDrvCtxt *)pResource->node->data;
  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;
  nDisableDCS = pImageCtxt->CPUCtxt.nDisableDCS;
  nFreqHz = NPA_TO_HZ(nState);

  /*-----------------------------------------------------------------------*/
  /* Perform necessary setup during resource creation.                     */
  /*-----------------------------------------------------------------------*/

  if (hClient->type == NPA_CLIENT_INITIALIZE)
  {
    /*
     * Create client to the voltage rail resource and specify the CPU id
     * that the voltage requirement must be satisfied for.
     */
    Clock_NPACPUResources.hClientRail =
      npa_create_sync_client_ex(
        pImageCtxt->pBSPConfig->szRailName,
        "CPU",
        NPA_CLIENT_SUPPRESSIBLE,
        (unsigned int)TRUE,
        (void *)&pImageCtxt->pBSPConfig->eCPU);

    if (Clock_NPACPUResources.hClientRail == NULL)
    {
      ERR_FATAL(
        "Unable to create NPA sync client %s->%s.",
        pImageCtxt->pBSPConfig->szRailName,
        "CPU",0);

      return 0;
    }

    /*
     * Initialize the CPU to the maximum performance level.
     */
    eResult = Clock_InitializeCPUMax(pDrvCtxt);
    if (eResult != DAL_SUCCESS)
    {
      ERR_FATAL(
        "Unable to initialize CPU during NPA resource creation.",0,0,0);

      return 0;
    }

    /*
     * Return the current configuration as the active state.
     */
    return pImageCtxt->CPUCtxt.pConfig->Mux.nFreqHz / 1000;
  }

  /*-----------------------------------------------------------------------*/
  /* Short-circuit if DCS is disabled by the CPU test.                     */
  /* This is because we don't want to modify the active state of /clk/cpu  */
  /* when a request has been made on /clk/cpu.test.                        */
  /*-----------------------------------------------------------------------*/

  if (nDisableDCS & CLOCK_FLAG_DISABLED_BY_CPU_TEST)
  {
    return pImageCtxt->CPUCtxt.pConfig->Mux.nFreqHz / 1000;
  }

  /*-----------------------------------------------------------------------*/
  /* If any other non-HW bit is set in the disable DCS flag then set the   */
  /* CPU to the maximum performance level.                                 */
  /*-----------------------------------------------------------------------*/

  nDisableDCS &= ~CLOCK_FLAG_DISABLED_BY_CPU_TEST;
  if (nDisableDCS & ~CLOCK_FLAG_DISABLED_HW_LOGIC)
  {
    nPL = pImageCtxt->CPUCtxt.PerfConfig.nMaxPerfLevel;
    nCfg = pImageCtxt->CPUCtxt.PerfConfig.anPerfLevel[nPL];
    nFreqHz = pImageCtxt->pBSPConfig->pCPUConfig[nCfg].Mux.nFreqHz;
  }

  /*-----------------------------------------------------------------------*/
  /* Update clock frequency.                                               */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_SetCPUFrequency(pDrvCtxt, nFreqHz);
  if (eResult != DAL_SUCCESS)
  {
    ERR_FATAL("Unable to set CPU clock to %lu",
      nFreqHz,0,0);
  }


  /*-----------------------------------------------------------------------*/
  /* Return active state of this resource.                                 */
  /*-----------------------------------------------------------------------*/

    nActiveState = pImageCtxt->CPUCtxt.pConfig->Mux.nFreqHz / 1000;
  

  return nActiveState;

} /* END Clock_NPACPUResourceDriverFunc */


/* =========================================================================
**  Function : Clock_NPACPUTestResourceDriverFunc
** =========================================================================*/
/**
  NPA node driver function for the CPU test resource.

  This function handles minimum frequency requests for the CPU test NPA resource.

  @param *pResource [in] -- Pointer to resource whose state changed.
  @param hClient    [in] -- Client handle that triggered the change.
  @param nState     [in] -- New request state.

  @return
  npa_resource_state -- The new active state of the resource.

  @dependencies
  None.
*/

static npa_resource_state Clock_NPACPUTestResourceDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{
  ClockDrvCtxt       *pDrvCtxt;
  ClockImageCtxtType *pImageCtxt;
  DALResult           eResult;

  pDrvCtxt = (ClockDrvCtxt *)pResource->node->data;
  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Nothing to do for initialization.                                     */
  /*-----------------------------------------------------------------------*/

  if (hClient->type == NPA_CLIENT_INITIALIZE)
  {
    return pResource->active_state;
  }

  /*-----------------------------------------------------------------------*/
  /* Satisfy CPU test request frequency.                                   */
  /*-----------------------------------------------------------------------*/

  if (nState)
  {
    /*
     * Set CPU test disable bit in clock scaling flag.
     */
    pImageCtxt->CPUCtxt.nDisableDCS |= CLOCK_FLAG_DISABLED_BY_CPU_TEST;

    /*
     * Set CPU to frequency requested by the test client.
     */
    eResult = Clock_SetCPUFrequency(pDrvCtxt, NPA_TO_HZ(nState));
    if (eResult != DAL_SUCCESS)
    {
      ERR_FATAL(
        "Unable to set test request CPU clock to %l",
        NPA_TO_HZ(nState),0,0);
    }
    /*
     * Update the active state of the /clk/cpu resource.
     */
    else
    {
      npa_assign_resource_state(
        NPA_RESOURCE_CLK_CPU,
        pImageCtxt->CPUCtxt.pConfig->Mux.nFreqHz / 1000);
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Clear the disable flag when no client request present.                */
  /*-----------------------------------------------------------------------*/

  else
  {
    /*
     * Clear CPU test disable bit in clock scaling flag.
     */
    pImageCtxt->CPUCtxt.nDisableDCS &= ~CLOCK_FLAG_DISABLED_BY_CPU_TEST;

    /*
     * Set CPU to requested state.
     */
    npa_issue_impulse_request(pImageCtxt->CPUCtxt.hNPAClkCPUImpulse);
  }

  return nState;

} /* END of Clock_NPACPUTestResourceDriverFunc */


/* =========================================================================
**  Function : Clock_NPACPUNodeDriverFunc
** =========================================================================*/
/**
  NPA node driver function for the CPU node.

  This function handles minimum frequency requests for the CPU NPA node.

  @param *pResource [in] -- Pointer to resource whose state changed.
  @param hClient    [in] -- Client handle that triggered the change.
  @param nState     [in] -- New request state.

  @return
  npa_resource_state -- The new active state of the resource.

  @dependencies
  None.
*/

static npa_resource_state Clock_NPACPUNodeDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{
  npa_resource_state nActiveState;

  if (pResource == NPA_RESOURCE_CLK_CPU)
  {
    nActiveState = Clock_NPACPUResourceDriverFunc(pResource, hClient, nState);
  }
  else if (pResource == NPA_RESOURCE_CLK_CPU_TEST)
  {
    nActiveState = Clock_NPACPUTestResourceDriverFunc(pResource, hClient, nState);
  }
  else
  {
    nActiveState = 0;
  }

  return nActiveState;

} /* END of Clock_NPACPUNodeDriverFunc */


/* =========================================================================
**  Function : Clock_NPACPUNodeAvailableCallback
** =========================================================================*/
/**
  Callback when a /clk/cpu node is created.

  This function is called by the NPA framework when the given /clk/cpu node
  is created.  The creation is delayed until all dependencies are also
  created.

  @param *pContext     [in] -- Context passed in npa_define_node_cb
  @param nEventType    [in] -- Zero.
  @param *pNodeName    [in] -- Name of the node being created.
  @param nNodeNameSize [in] -- Length of the name.

  @return
  None.

  @dependencies
  None.
*/

static void Clock_NPACPUNodeAvailableCallback
(
  void        *pContext,
  unsigned int nEventType,
  void        *pNodeName,
  unsigned int nNodeNameSize
)
{
  ClockDrvCtxt       *pDrvCtxt;
  ClockImageCtxtType *pImageCtxt;

  pDrvCtxt = (ClockDrvCtxt *)pContext;
  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Create an impulse client for triggering re-aggregation on the CPU     */
  /* resource.                                                             */
  /*-----------------------------------------------------------------------*/

  pImageCtxt->CPUCtxt.hNPAClkCPUImpulse =
    npa_create_sync_client(
      Clock_NPACPUResources.aResource[CLOCK_APPS_RESOURCE_CLK_CPU].name,
      CLOCK_NPA_CLIENT_NAME_CLK_CPU_IMPULSE,
      NPA_CLIENT_IMPULSE);
  if (pImageCtxt->CPUCtxt.hNPAClkCPUImpulse == NULL)
  {
    ERR_FATAL(
      "Unable to create NPA sync client %s->%s.",
      CLOCK_NPA_CLIENT_NAME_CLK_CPU_IMPULSE,
      CLOCK_NPA_NODE_NAME_CPU,0);
  }

} /* END Clock_NPACPUNodeAvailableCallback */




/* =========================================================================
**  Function : Clock_NPACPUResourceQuery
** =========================================================================*/
/**
  NPA CPU resource query function.
 
  This function is called by NPA query nodes to get a list of supported CPU
  frequencies, or the CPU's vote on its voltage rail.
 
  @param *pResource  [in]  -- Pointer to the resource in question
  @param  id         [in]  -- ID of the query.
  @param *pResult    [out] -- Pointer to the data to be filled by this function.

  @return
  npa_query_status - NPA_QUERY_SUCCESS, if query supported.
                   - NPA_QUERY_UNSUPPORTED_QUERY_ID, if query not supported.

  @dependencies
  None.
*/ 

static npa_query_status Clock_NPACPUResourceQuery
( 
  npa_resource    *pResource,
  unsigned int     id,
  npa_query_type  *pResult
)
{
  ClockDrvCtxt             *pDrvCtxt;
  ClockImageCtxtType       *pImageCtxt;
  ClockCPUConfigType       *pCPUConfig;
  ClockCPUPerfConfigType   *pCPUPerfConfig;
  uint32                    nMinPL, nMaxPL, nConfig, nIndex;
  uint32                    nNumPerfLevels;
  npa_query_status          nStatus;
  DALResult                 eResult;

  pDrvCtxt = (ClockDrvCtxt *)pResource->node->data;
  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;
  pCPUConfig = pImageCtxt->pBSPConfig->pCPUConfig;
  pCPUPerfConfig = NULL;
  nNumPerfLevels = 0;
  nStatus  = NPA_QUERY_UNSUPPORTED_QUERY_ID;
  
  /*-----------------------------------------------------------------------*/
  /* Get a handle to CPU performance data.                                 */
  /*-----------------------------------------------------------------------*/

  pCPUPerfConfig = &pImageCtxt->CPUCtxt.PerfConfig;

  /*
   * Get min, max, and number of CPU performance levels.
   */
  nMinPL = pCPUPerfConfig->nMinPerfLevel;
  nMaxPL = pCPUPerfConfig->nMaxPerfLevel;
  nNumPerfLevels = nMaxPL - nMinPL + 1;
  
  /*-----------------------------------------------------------------------*/
  /* If this is a performance level indexing request then return the       */
  /* frequency corresponding to this performance level.                    */
  /*-----------------------------------------------------------------------*/

  if((id >= CLOCK_NPA_QUERY_PERF_LEVEL_KHZ) &&
     (id < CLOCK_NPA_QUERY_NUM_PERF_LEVELS))
  {
    nIndex = ((uint32)id - (uint32)CLOCK_NPA_QUERY_PERF_LEVEL_KHZ);
    if(nIndex < nNumPerfLevels)
    {
      nConfig = pCPUPerfConfig->anPerfLevel[nMinPL + nIndex];
      pResult->type = NPA_QUERY_TYPE_VALUE;
      pResult->data.value = pCPUConfig[nConfig].Mux.nFreqHz / 1000;
      nStatus = NPA_QUERY_SUCCESS;
    }
  }
  
  /*-----------------------------------------------------------------------*/
  /* Returns the number of performance levels.                             */
  /*-----------------------------------------------------------------------*/

  else if(id == CLOCK_NPA_QUERY_NUM_PERF_LEVELS)
  {
    pResult->type = NPA_QUERY_TYPE_VALUE;
    pResult->data.value = nNumPerfLevels;
    nStatus = NPA_QUERY_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* If this is a max frequency at corner voltage indexing request then    */
  /* return the frequency corresponding to this corner voltage.            */
  /*-----------------------------------------------------------------------*/

  else if((id >= CLOCK_NPA_QUERY_CPU_MAX_KHZ_AT_CORNER) &&
          (id < CLOCK_NPA_QUERY_MIN_FREQ_KHZ))
  {
    nIndex = ((uint32)id - (uint32)CLOCK_NPA_QUERY_CPU_MAX_KHZ_AT_CORNER);
    if(nIndex < VCS_CORNER_NUM_OF_CORNERS)
    {
      eResult = Clock_FindCPUMaxConfigAtVoltage(pDrvCtxt, &pCPUConfig, nIndex);
      if (eResult == DAL_SUCCESS)
      {
        pResult->type = NPA_QUERY_TYPE_VALUE;
        pResult->data.value = pCPUConfig->Mux.nFreqHz / 1000;
        nStatus = NPA_QUERY_SUCCESS;
      }
    }
  }
  
  /*-----------------------------------------------------------------------*/
  /* Return the minimum frequency in KHz.                                  */
  /*-----------------------------------------------------------------------*/

  else if (id == CLOCK_NPA_QUERY_MIN_FREQ_KHZ)
  {
    nConfig = pCPUPerfConfig->anPerfLevel[nMinPL];
    pResult->type = NPA_QUERY_TYPE_VALUE;
    pResult->data.value = pCPUConfig[nConfig].Mux.nFreqHz / 1000;
    nStatus = NPA_QUERY_SUCCESS;
  }
  
  /*-----------------------------------------------------------------------*/
  /* Return the CPU's corner vote on the voltage rail.                     */
  /*-----------------------------------------------------------------------*/

  else if (id == CLOCK_NPA_QUERY_CPU_RAIL_VOLTAGE_CORNER)
  {
    pResult->type = NPA_QUERY_TYPE_VALUE;
    pResult->data.value = pImageCtxt->CPUCtxt.pConfig->Mux.eVRegLevel;
    nStatus = NPA_QUERY_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Return the identifier of the rail powering the CPU.                   */
  /*-----------------------------------------------------------------------*/

  else if (id == CLOCK_NPA_QUERY_CPU_RAIL_ID)
  {
    pResult->type = NPA_QUERY_TYPE_VALUE;
    pResult->data.value = pImageCtxt->pBSPConfig->eRail;
    nStatus = NPA_QUERY_SUCCESS;
  }

  return nStatus;

} /* END Clock_NPACPUResourceQuery */


/* =========================================================================
**  Function : Clock_RCECBCallback
** =========================================================================*/
/**
  Callback when the final RCINIT group completes.

  This function is called when the final RCINIT group completes in order for
  the clock driver to reduce its vote on /clk/bimc from MAX_NPA_STATE to 1.
  This is needed to ensure that some entity (the clock driver in this instance)
  maintains a non-zero vote on /clk/bimc so that RPM doesn't power collapse
  BIMC while MPSS is in the active set.

  @return
  None.

  @dependencies
  None.
*/
static void Clock_RCECBCallback
(
  void
)
{

  /*-----------------------------------------------------------------------*/
  /* Create client handle to DCVS enable resource.                         */
  /*-----------------------------------------------------------------------*/

  Clock_NPACPUResources.hClientDCVS =
    npa_create_sync_client(
      CLOCK_NPA_NODE_NAME_DCVS,
      "/clock",
      NPA_CLIENT_REQUIRED);
  if (Clock_NPACPUResources.hClientDCVS == NULL)
  {
    ERR_FATAL(
      "No %s NPA client handle to vote on in RCECB callback",
      CLOCK_NPA_NODE_NAME_DCVS,0,0);
  }

  npa_issue_scalar_request(Clock_NPACPUResources.hClientDCVS, 1);

} /* END Clock_RCECBCallback */
